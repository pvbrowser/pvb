/* <St> *******************************************************************

   cif_ioctl.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION
    IOCTLs               .
  =========================================================================

  CHANGES
  version name      date        Discription
                 March 2001
  		 Juli  2004     Redesigned for the 2.6 Kernel
 		                Copyright changed to GNU Lesser GPL
-------------------------------------------------------------------------
  V2.600

  NOTE: as groundwork for this source code served Windows version of 
        the CIF device driver

  ======================== Copyright =====================================

  Copyright (C) 2004  Hilscher GmbH

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
   
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 
  ========================================================================
******************************************************************** <En> */

#include <linux/kernel.h>     /* printk() */
#include <linux/slab.h>       /* kmalloc() */
#include <linux/fs.h>         /* everything... */
#include <linux/errno.h>      /* error codes */
#include <linux/types.h>      /* size_t */
#include <linux/fcntl.h>      /* O_ACCMODE */
#include <linux/ioctl.h>
#include <linux/interrupt.h>

#include <asm/uaccess.h>      /* VERIFY_READ etc. */
#include <asm/system.h>       /* cli(), *_flags */
#include <asm/segment.h>
#include <asm/io.h>

#include "../inc/cif_types.h"
#include "../inc/cif_dev_i.h"  /* local definitions */
#include "../inc/cif_dev.h"    /* local definitions */

extern  int              cif_nr_devs;

extern  DEV_INSTANCE    *cif_dev_get_at      ( int n);
extern  unsigned char   *pabDrvVersion;      // Driver version
extern  int              cif_register_irq    ( DEV_INSTANCE *dev);

extern  unsigned short   cif_check_hardware  ( DEV_INSTANCE *);
extern  void             cif_boot_start      ( DEV_INSTANCE *);
extern  void             cif_cold_start      ( DEV_INSTANCE *);
extern  void             cif_warm_start      ( DEV_INSTANCE *);
extern  unsigned short   cif_get_devMBXinfo  ( DEV_INSTANCE *);
extern  unsigned short   cif_get_hostMBXinfo ( DEV_INSTANCE *);
extern  unsigned short   cif_test_ready      ( DEV_INSTANCE *);
extern  unsigned short   cif_wait_put_msg    ( DEV_INSTANCE *, unsigned long);
extern  unsigned short   cif_wait_get_msg    ( DEV_INSTANCE *, unsigned long);
extern  unsigned short   cif_wait_reset      ( DEV_INSTANCE *, unsigned long);
extern  unsigned short   cif_test_com        ( DEV_INSTANCE *);
extern  unsigned short   cif_wait_com        ( DEV_INSTANCE *, unsigned long);
extern  unsigned short   cif_get_io_state    ( DEV_INSTANCE *);
extern  void             cif_clear_host_state( DEV_INSTANCE *);
extern  void             cif_get_msg_status  ( DEV_INSTANCE *, unsigned char);
extern  short            cif_test_ext_version( DEV_INSTANCE *, unsigned short, unsigned char *);
extern  long             cif_exchange_io     ( DEV_INSTANCE *, DEVIO_EXIOCMD *,unsigned char);
extern  unsigned short   cif_get_state       ( DEV_INSTANCE *);
extern  void             cif_state_done      ( DEV_INSTANCE *);
extern  void             cif_send_msg        ( DEV_INSTANCE *);
extern  void             cif_quit_msg        ( DEV_INSTANCE *);
extern  void             cif_set_host_state  ( DEV_INSTANCE *);
extern  void             cif_set_irq_state   ( unsigned short, DEV_INSTANCE *);

int cif_ioctl (struct inode *inode, struct file *filp,
               unsigned int cmd, unsigned long arg)
{
DEVIO_GETBOARDINFOCMD   *  ptBoardCmd;
DEVIO_RESETCMD          *  pResetCmd;
DEVIO_PUTMESSAGECMD     *  pPutMsgCmd;
DEVIO_GETMESSAGECMD     *  pGetMsgCmd;
DEVIO_PUTPARAMETERCMD   *  pPutParamCmd;
DEVIO_GETTASKSTATECMD   *  pGetTskStateCmd;
DEVIO_MBXINFOCMD        *  ptGetMBXInfoCmd;
DEVIO_TRIGGERCMD        *  ptGetTriggerCmd;
DEVIO_GETDEVINFOCMD     *  ptGetInfoCmd, *pvData;
DEVIO_EXITCMD           *  pExitCmd;
DEVIO_GETPARAMETERCMD   *  pGetParamCmd;
DEVIO_EXIOCMD           *  pExIOCmd, *pExIO;
DEVIO_EXIOCMDEX         *  pExIOCmdEx, *pExIOEx;
DEVIO_EXIOCMDERR        *  pExIOCmdErr, *pExIOErr;
DEVIO_READSENDCMD       *  pReadSendCmd;
DEVIO_EXTDATACMD        *  pExtCmd;
DEVIO_GETMBXCMD         *  pGetMbxCmd;
DEVIO_GETBOARDINFOEXCMD *  ptBoardExCmd;
DEVIO_RWRAWDATACMD      *  pRWRawCmd;
DEVIO_RWDPMDATACMD      *  pRWDPMCmd;
DEVIO_SETOPMODE         *  ptSetOpMode;

DEV_INSTANCE            *  dev = NULL;

unsigned long              flags; /* for the spin locks */ 
unsigned short             usIdx, usMsgLen, usDummy;
long                       lRet = 0;
int                        result = 0;
DEVIO_GETBOARDINFOCMD   *  pDatA = (DEVIO_GETBOARDINFOCMD *)arg;
DEVIO_GETBOARDINFOEXCMD *  pDatB = (DEVIO_GETBOARDINFOEXCMD *)arg;

int err = 0, size = _IOC_SIZE(cmd); /* the size bitfield in cmd */
    
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (_IOC_TYPE(cmd) != CIF_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd)   >  CIF_IOC_MAXNR) return -ENOTTY;

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, size);
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, (void *)arg, size);
    if (err) return -EFAULT;

    switch(cmd) {
    //****************************************************************************************
    //  SET Interrupt || Polling mode
    //****************************************************************************************

    case CIF_IOCTL_IRQ_POLL:    

      ptSetOpMode = (DEVIO_SETOPMODE *)kmalloc( sizeof(DEVIO_SETOPMODE), GFP_KERNEL);
      copy_from_user ( (unsigned char *)ptSetOpMode,
                       (unsigned char *)arg,
		       sizeof( DEVIO_SETOPMODE) );
      ptSetOpMode->sError = DRV_NO_ERROR;    // clear error
      // at last one application must be init
      if ( ptSetOpMode->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLTASKSTATE: BOARD NOT INITIALIZED\n");
	  ptSetOpMode->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( ptSetOpMode->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLTASKSTATE: BOARD NOT INITIALIZED\n");
	  ptSetOpMode->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0){
	  DBG_PRN("CIF_IOCTLTASKSTATE: BOARD NOT INITIALIZED\n");
	  ptSetOpMode->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
	if ( ptSetOpMode->usMode == POLLING_MODE) {
	  ptSetOpMode->usIrq = dev->usBoardIrq_scanned; // tell the app which irq was desabled!
	  if( dev->usBoardIrq != 0) {
	    DBG_PRN("Desabling interrupt: free Irq %d\n", dev->usBoardIrq);
	    // Disable physical interrupt on the hardware
	    cif_set_irq_state( HW_INTERRUPT_DISABLE, dev);
	    free_irq( (unsigned int)(dev->usBoardIrq),  dev);
	    dev->usBoardIrq = 0;
	  }
	}
	else if (ptSetOpMode->usMode == INTERRUPT_MODE) {
	  DBG_PRN("Enabling interruprt: actual_irq = %d, scanned_irq = %d\n", dev->usBoardIrq, dev->usBoardIrq_scanned);
	  if ( dev->usBoardIrq != 0) {
	    cif_set_irq_state( HW_INTERRUPT_DISABLE, dev);
	    free_irq( (unsigned int)(dev->usBoardIrq), dev);
	  }
	  else
	    dev->usBoardIrq = dev->usBoardIrq_scanned;
	  result = cif_register_irq( dev);
	  if (result) {
	    DBG_PRN("can't get assigned irq %i\n", dev->usBoardIrq_scanned);
	    ptSetOpMode->sError = DRV_DEV_REQUEST_IRQ_FAILED;
	    dev->usBoardIrq     = 0; // driver have to operate in polling mode because request_irq failed!
	    ptSetOpMode->usIrq  = 0; // tell the app about it !
	  }
	  else {
	    DBG_PRN("CIF interrupt handler registration OK. Board = %d, request_irq: %X\n", 
		    ptSetOpMode->usBoard, result);
	    cif_set_irq_state( HW_INTERRUPT_ENABLE, dev);
	    ptSetOpMode->usIrq = dev->usBoardIrq; // tell the app which irq were registered for the board!
	  }
	}
      }
      copy_to_user ( (unsigned char *)arg,
		     (unsigned char *)ptSetOpMode,
		     sizeof( DEVIO_SETOPMODE) );
      lRet = sizeof(DEVIO_SETOPMODE);
      kfree( ptSetOpMode);
      break;
    //****************************************************************************************
    //  BOARDINFO
    //****************************************************************************************

    case CIF_IOCTLBOARDINFO:      // reads the DEV board information

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      ptBoardCmd = kmalloc(sizeof(DEVIO_GETBOARDINFOCMD), GFP_KERNEL);
      ptBoardCmd->ptBoardInfo = kmalloc(sizeof(BOARD_INFO), GFP_KERNEL);
      DBG_PRN("CIF_IOCTLBOARDINFO:  ptBoardInfo = %p,  size = %X\n", 
	      ptBoardCmd, sizeof(BOARD_INFO));
      DBG_PRN("arg = %lX, ptBI=%p\n", arg, pDatA->ptBoardInfo);

      ptBoardCmd->sError = 0;
      // write driver version
      memcpy((unsigned char *)&(ptBoardCmd->ptBoardInfo->abDriverVersion[0]), 
             (unsigned char *)pabDrvVersion, 
             sizeof(ptBoardCmd->ptBoardInfo->abDriverVersion) );
      DBG_PRN("CIF_IOCTLBOARDINFO: Written, x = %s\n", ptBoardCmd->ptBoardInfo->abDriverVersion);
      ptBoardCmd->ptBoardInfo->usBoards_detected = cif_nr_devs;
      for ( usIdx = 0; usIdx < cif_nr_devs; usIdx++) {
	dev = cif_dev_get_at( usIdx);
        // fill in board data
        ptBoardCmd->ptBoardInfo->tBoard[usIdx].usBoard           = usIdx;
        ptBoardCmd->ptBoardInfo->tBoard[usIdx].usAvailable       = dev->bActive;
        ptBoardCmd->ptBoardInfo->tBoard[usIdx].ulPhysicalAddress = dev->ulBoardAddress;
        ptBoardCmd->ptBoardInfo->tBoard[usIdx].usIrq             = dev->usBoardIrq;
     }
      copy_to_user ( (unsigned char *)(pDatA->ptBoardInfo),
                     (unsigned char *)ptBoardCmd->ptBoardInfo,
		     sizeof(BOARD_INFO) );
      copy_to_user ( (unsigned char *)arg,
                     (unsigned char *)ptBoardCmd,
		     sizeof(DEVIO_GETBOARDINFOCMD) );
      DBG_PRN("CIF_IOCTLBOARDINFO: DONE\n");
      kfree( ptBoardCmd->ptBoardInfo);
      kfree( ptBoardCmd);
      // return length of output data
      lRet = sizeof(DEVIO_GETBOARDINFOCMD);

      break;

    //****************************************************************************************
    //  BOARDINFOEX
    //****************************************************************************************

    case CIF_IOCTLBOARDINFOEX:    // read extended DEV board information

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      ptBoardExCmd           = kmalloc(sizeof(DEVIO_GETBOARDINFOEXCMD), GFP_KERNEL);
      ptBoardExCmd->ptBoard  = kmalloc(sizeof(BOARD_INFOEX), GFP_KERNEL);
      ptBoardExCmd->sError   = 0;    // clear error

      DBG_PRN("CIF_IOCTLBOARDEXINFO: Read\n");
      // write driver version
      memcpy ( (unsigned char *)&(ptBoardExCmd->ptBoard->abDriverVersion[0]),
	       (unsigned char *)pabDrvVersion,
	       sizeof(ptBoardExCmd->ptBoard->abDriverVersion));

      for ( usIdx = 0; usIdx < cif_nr_devs; usIdx++) {
	dev = cif_dev_get_at( usIdx);
        // fill in board data
        ptBoardExCmd->ptBoard->tBoard[usIdx].usBoard           = usIdx;
        ptBoardExCmd->ptBoard->tBoard[usIdx].usAvailable       = dev->bActive;
        ptBoardExCmd->ptBoard->tBoard[usIdx].ulPhysicalAddress = dev->ulBoardAddress;
        ptBoardExCmd->ptBoard->tBoard[usIdx].usIrq             = dev->usBoardIrq;

        // load DEV instance to real structure
        // Board must be installed to read all other information
        if ( dev->bActive != 0) {
          // driver info
          if ( dev->usBoardIrq != 0) {
            // copy actual state information, interrupt is running
            dev->tStateInfo.HostFlags    = dev->bHostFlags;     // only for information
            dev->tStateInfo.MyDevFlags   = dev->bMyDevFlags;    // only for information
            dev->tStateInfo.InitMsgFlag  = dev->bInitMsgFlag;   // only for information
            dev->tStateInfo.ReadMsgFlag  = dev->bReadMsgFlag;   // only for information
            dev->tStateInfo.WriteMsgFlag = dev->bWriteMsgFlag;  // only for information
          } else {
            // copy actual state information, polling is activ
            dev->tStateInfo.HostFlags    = *(dev->pbHostFlags); // only for information
            dev->tStateInfo.MyDevFlags   = *(dev->pbDevFlags);  // only for information
            dev->tStateInfo.InitMsgFlag  = 0;                   // only for information
            dev->tStateInfo.ReadMsgFlag  = 0;                   // only for information
            dev->tStateInfo.WriteMsgFlag = 0;                   // only for information
          }
          // Set driver information
          memcpy ( (unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tDriverInfo,
		   (unsigned char *)&(dev->tStateInfo),
		   sizeof( DRIVERINFO));

          // firmware info
          memcpy ( (unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tFirmware,
		   (unsigned char *)&(dev->ptDpmAddress->tFiwInfo),
		   sizeof( FIRMWAREINFO));
          // device info
          memcpy ( (unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tDeviceInfo,
		   (unsigned char *)&(dev->ptDpmAddress->tDevInfo),
		   sizeof( DEVINFO));
          // RCS info
          memcpy ( (unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tRcsInfo,
		   (unsigned char *)&(dev->ptDpmAddress->tRcsInfo),
		   sizeof( RCSINFO));
          // version info
          memcpy ( (unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tVersion,
		   (unsigned char *)&(dev->ptDpmAddress->tDevVersion),
		   sizeof( VERSIONINFO));

        } else {
          // initialize with zero
          memset((unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tDriverInfo, 0, sizeof( DRIVERINFO));
          // firmware info
          memset((unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tFirmware, 0, sizeof( FIRMWAREINFO));
          // device info
          memset((unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tDeviceInfo, 0, sizeof( DEVINFO));
          // RCS info
          memset((unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tRcsInfo, 0,  sizeof( RCSINFO));
          // Version nfo
          memset((unsigned char *)&ptBoardExCmd->ptBoard->tBoard[usIdx].tVersion, 0,  sizeof( VERSIONINFO));
        }
      }
      copy_to_user ( (unsigned char *)(pDatB->ptBoard),
                     (unsigned char *)ptBoardExCmd->ptBoard,
		     sizeof(BOARD_INFOEX) );
      copy_to_user ( (unsigned char *)arg,
                     (unsigned char *)ptBoardExCmd,
		     sizeof(DEVIO_GETBOARDINFOEXCMD) );
      DBG_PRN("CIF_IOCTLBOARDINFOEX: DONE\n");
      kfree( ptBoardExCmd->ptBoard);
      kfree( ptBoardExCmd);
      // return length of output data
      lRet = sizeof(DEVIO_GETBOARDINFOEXCMD);

      break;

    //****************************************************************************************
    //  INITDRV
    //****************************************************************************************

    case CIF_IOCTLINITDRV:  // init DRV from application

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pResetCmd = kmalloc( sizeof(DEVIO_RESETCMD), GFP_KERNEL);
      copy_from_user ( (unsigned char *)pResetCmd,
                       (unsigned char *)arg,
		       sizeof( DEVIO_RESETCMD) );
      pResetCmd->sError = 0;    // clear error
      DBG_PRN("CIF_IOCTLINITDRV: usBoard (%d)\n", pResetCmd->usBoard);
      // is driver activ
      if ( pResetCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLINITDRV: BOARD NOT INITIALIZED\n");
	  pResetCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pResetCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLINITDRV: BOARD NOT INITIALIZED\n");
	  pResetCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        //----------------------------------
        // Driver is activ for this board
        //----------------------------------

        // lets application open
        if ( dev->usOpenCounter == 0) {

          // 1st time INITDRV is called, run hardware test for this board
          // Reset error -14 is available
          if ( dev->sDrvInitError == DRV_DEV_OS_VERSION_ERROR)  dev->sDrvInitError = DRV_NO_ERROR;

          if ( cif_check_hardware(dev) == FALSE) {

            // hardwaretest failed
            if ( dev->usRcsError != 0)  pResetCmd->sError = dev->usRcsError;  // RCS error ocurred
            else pResetCmd->sError = dev->sDrvInitError;                            // or real init error
            DBG_PRN("InitDrv ERROR = (%d)\n", pResetCmd->sError);
          } else {
            // 1st time INITDRV is called
            cif_get_msg_status( dev,*(dev->pbHostFlags));// read Board state flags
            dev->bMyDevFlags = *(dev->pbDevFlags);  // store actual flags in internal structure
            // Board is available
            dev->usOpenCounter++;                           // add number of logged applications
            DBG_PRN("InitDrv OK, OpenCount = (%d)\n", dev->usOpenCounter);
          }
        } else {// DRV is running
          dev->usOpenCounter++;      // new application logged in
          DBG_PRN("InitDrv already done, OpenCount = (%d)\n", dev->usOpenCounter);
        }
        // return DPM size
        pResetCmd->ulDpmSize = (unsigned long)(dev->ptDpmAddress->tDevInfo.DpmSize);
        // store OpenCount for information display
        dev->tStateInfo.OpenCnt = dev->usOpenCounter;
        DBG_PRN("DpmSize = %ld\n", pResetCmd->ulDpmSize);
      }
      copy_to_user ( (unsigned char *)arg,
		     (unsigned char *)pResetCmd,
		     sizeof( DEVIO_RESETCMD) );
      kfree( pResetCmd);
      // return length of output data
      lRet = sizeof(DEVIO_RESETCMD);

    break;

    //****************************************************************************************
    //  RESETDEV
    //****************************************************************************************
    case CIF_IOCTLRESETDEV:       // reset DEV
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pResetCmd = kmalloc( sizeof(DEVIO_RESETCMD), GFP_KERNEL);
      copy_from_user ( (unsigned char *)pResetCmd,
                       (unsigned char *)arg,
		       sizeof( DEVIO_RESETCMD) );
      pResetCmd->sError = 0;    // clear error
      DBG_PRN("CIF_IOCTLRESETDEV: BOARD = %d, mode = %d, tout = %ld\n",
	      pResetCmd->usBoard, pResetCmd->usMode, pResetCmd->ulTimeout);

      // is DEV board installed an driver activ
      if ( pResetCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLRESETDEV: BOARD NOT INITIALIZED\n");
	  pResetCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pResetCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLRESETDEV: BOARD NOT INITIALIZED\n");
	  pResetCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0){
	  DBG_PRN("CIF_IOCTLRESETDEV: BOARD NOT INITIALIZED\n");
	  pResetCmd->sError = DRV_BOARD_NOT_INITIALIZED;
       } else if (dev->bInitState != FALSE) {
        // service is active
        DBG_PRN("CIF_IOCTLRESETDEV: CMD ACTIVE\n");
        pResetCmd->sError = DRV_CMD_ACTIVE;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        // check if read or write service active
        // activate function

        // disable interrupt until we are ready
        spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex); 

        if ( pResetCmd->usMode == 4) {
          // boot
          DBG_PRN("CIF_IOCTLRESETDEV: ResetDev WARM BOOT (BOOTSTART)\n");
	  cif_boot_start(dev);
        } else if ( pResetCmd->usMode == 3) {
          // warm boot
          DBG_PRN("CIF_IOCTLRESETDEV: ResetDev WARM BOOT\n");
	  cif_warm_start(dev);
        } else {
          // cold boot
          DBG_PRN("CIF_IOCTLRESETDEV: ResetDev COLD BOOT\n");
	  cif_cold_start(dev);
        }

        // wait for RDY is gone and init state OK
        dev->bInitState = TRUE;    // start init

        // enable interrupt we are ready
        spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);

        // Wait for reset is running
        if ( cif_wait_reset(dev, pResetCmd->ulTimeout) == FALSE) {

          // No reset occured, timeout
          DBG_PRN("CIF_IOCTLRESETDEV: TIMEOUT\n");
          DBG_PRN("bInitMsgFlag=(%x)\n", dev->bInitMsgFlag);
          pResetCmd->sError = DRV_DEV_RESET_TIMEOUT;

        } else {

          //  No Timeout, test actual state
          switch ( dev->bInitMsgFlag ) {
            case INI_MSG_RDYRUN:
            case INI_MSG_RDY:
              // Init is ready
              pResetCmd->sError = DRV_NO_ERROR;
              DBG_PRN("CIF_IOCTLRESETDEV: OK\n");
              break;
            /*
            case INI_MSG_RDY:
              // Init error, quit to caller
              pResetCmd->sError = DRV_DEV_NOT_RUNNING;
	      DBG_PRN("CIF_IOCTLRESETDEV: DEVICE NOT RUNNING\n");
              break;
            */
            default:
              // Init unknown state
              pResetCmd->sError = DRV_INIT_STATE_ERROR;
              DBG_PRN("CIF_IOCTLRESETDEV: INIT STATE ERROR=(%x)\n", dev->bInitMsgFlag);
              break;
          } /* endswitch */
        }
        dev->bInitState = FALSE;   // stop init
      }
      // return length of output data
      lRet = sizeof(DEVIO_RESETCMD);
      copy_to_user ( (unsigned char *)arg,
		     (unsigned char *)pResetCmd,
		     sizeof( DEVIO_RESETCMD) );
      kfree( pResetCmd);

      break;

    //****************************************************************************************
    //  PUTPARAMETER
    //****************************************************************************************

    case CIF_IOCTLPARAMETER:  // set task parameter

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pPutParamCmd  = (DEVIO_PUTPARAMETERCMD  *)kmalloc( sizeof(DEVIO_PUTPARAMETERCMD), GFP_KERNEL);
      copy_from_user ( (unsigned char *)pPutParamCmd,
                       (unsigned char *)arg,
		       sizeof( DEVIO_PUTPARAMETERCMD) );
      pPutParamCmd->sError = 0;    // clear error
      DBG_PRN("CIF_IOCTLPARAMETER: usBoard (%d)\n", pPutParamCmd->usBoard);
      DBG_PRN("                    usTaskParameter (%d)\n", pPutParamCmd->usTaskParamNum);
      // is DEV board installed an driver activ
      if ( pPutParamCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLPARAMETER: BOARD NOT INITIALIZED\n");
	  pPutParamCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pPutParamCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLPARAMETER: BOARD NOT INITIALIZED\n");
	  pPutParamCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0){
	  DBG_PRN("CIF_IOCTLPARAMETER: BOARD NOT INITIALIZED\n");
	  pPutParamCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        switch (pPutParamCmd->usTaskParamNum) {
          case 1:   // Task 1
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tKpt1Param),
		     (unsigned char *)&(pPutParamCmd->TaskParameter),
		     pPutParamCmd->usTaskParamLen);

	    DBG_PRN("PutParameter Task 1 OK\n");
            break;
          case 2:   // Task 2
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tKpt2Param),
		     (unsigned char *)&(pPutParamCmd->TaskParameter),
		     pPutParamCmd->usTaskParamLen);
	    DBG_PRN("PutParameter Task 2 OK\n");
            break;
          case 3:   // Task 3
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tDevMbx),
		     (unsigned char *)&(pPutParamCmd->TaskParameter),
		     pPutParamCmd->usTaskParamLen);
	    DBG_PRN("PutParameter Task 3 OK\n");
            break;
          case 4:   // Task 4
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tDevMbx) + sizeof(TASKPARAM),
		     (unsigned char *)&(pPutParamCmd->TaskParameter),
		     pPutParamCmd->usTaskParamLen);
	    DBG_PRN("PutParameter Task 4 OK\n");
            break;
          case 5:   // Task 5
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tDevMbx) + ( 2 * sizeof(TASKPARAM)),
		     (unsigned char *)&(pPutParamCmd->TaskParameter),
		     pPutParamCmd->usTaskParamLen);
	    DBG_PRN("PutParameter Task 5 OK\n");
            break;
          case 6:   // Task 6
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tDevMbx) + ( 3 * sizeof(TASKPARAM)),
		     (unsigned char *)&(pPutParamCmd->TaskParameter),
		     pPutParamCmd->usTaskParamLen);
	    DBG_PRN("PutParameter Task 6 OK\n");
            break;
          case 7:   // Task 7
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tDevMbx) + ( 4 * sizeof(TASKPARAM)),
		     (unsigned char *)&(pPutParamCmd->TaskParameter),
		     pPutParamCmd->usTaskParamLen);
	    DBG_PRN("PutParameter Task 7 OK\n");
            break;
          default:
            // parameter error
            pPutParamCmd->sError = DRV_USR_NUMBER_INVALID;
            break;
        }
      }
      copy_to_user ( (unsigned char *)arg,
		     (unsigned char *)pPutParamCmd,
		     sizeof( DEVIO_PUTPARAMETERCMD) );
      kfree( pPutParamCmd);
      // return length of output data
      lRet = sizeof(DEVIO_PUTPARAMETERCMD);

      break;

    //****************************************************************************************
    //  GETINFO
    //****************************************************************************************

    case CIF_IOCTLGETINFO:    // read DEV firmware information field
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pvData = (DEVIO_GETDEVINFOCMD  *)arg;
      ptGetInfoCmd = (DEVIO_GETDEVINFOCMD  *)kmalloc( sizeof(DEVIO_GETDEVINFOCMD), GFP_KERNEL);
      copy_from_user ( (unsigned char *)ptGetInfoCmd,
                       (unsigned char *)arg,
		       sizeof( DEVIO_GETDEVINFOCMD) );
      ptGetInfoCmd->sError = 0;    // clear error

      // setup length of output datas
      lRet = sizeof(ptGetInfoCmd->sError);
      // Board must be installed, then all info functions available
      if ( ptGetInfoCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLGETINFO: BOARD NOT INITIALIZED\n");
	  ptGetInfoCmd->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = 0;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( ptGetInfoCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLGETINFO: BOARD NOT INITIALIZED\n");
	  ptGetInfoCmd->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = 0;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        // read the InfoArea
        switch ( ptGetInfoCmd->usInfoArea) {
          case GET_DRIVER_INFO:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_DRIVER_INFO)\n");
            if ( dev->usBoardIrq != 0) {
              // copy actual state information, interrupt is running
              dev->tStateInfo.HostFlags    = dev->bHostFlags;   // only for information
              dev->tStateInfo.MyDevFlags   = dev->bMyDevFlags;  // only for information
              dev->tStateInfo.InitMsgFlag  = dev->bInitMsgFlag; // only for information
              dev->tStateInfo.ReadMsgFlag  = dev->bReadMsgFlag; // only for information
              dev->tStateInfo.WriteMsgFlag = dev->bWriteMsgFlag;// only for information
            } else {
              // copy actual state information, polling is activ
              dev->tStateInfo.HostFlags    = *(dev->pbHostFlags); // only for information
              dev->tStateInfo.MyDevFlags   = *(dev->pbDevFlags);  // only for information
              dev->tStateInfo.InitMsgFlag  = dev->bInitMsgFlag;   // only for information;
              dev->tStateInfo.ReadMsgFlag  = dev->bReadMsgFlag;   // only for information;
              dev->tStateInfo.WriteMsgFlag = dev->bWriteMsgFlag;  // only for information;
            }
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)&(dev->tStateInfo),
			   ptGetInfoCmd->usInfoLen);
	    // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
            break;

          case GET_VERSION_INFO:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_VERSION_INFO)\n");
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)&(dev->ptDpmAddress->tDevVersion),
			   ptGetInfoCmd->usInfoLen);
            // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
            break;

          case GET_FIRMWARE_INFO:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_FIRMWARE_INFO)\n");
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)&(dev->ptDpmAddress->tFiwInfo),
			   ptGetInfoCmd->usInfoLen);
            // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
            break;

          case GET_TASK_INFO:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_TASK_INFO)\n");
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)&(dev->ptDpmAddress->tTaskInfo),
			   ptGetInfoCmd->usInfoLen);
            // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
            break;

          case GET_RCS_INFO:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_RCS_INFO)\n");
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)&(dev->ptDpmAddress->tRcsInfo),
			   ptGetInfoCmd->usInfoLen);
            // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
            break;

          case GET_DEV_INFO:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_DEV_INFO) DevId: %c%c%c, size: %d\n", 
		    dev->ptDpmAddress->tDevInfo.DevIdentifier[0], 
		    dev->ptDpmAddress->tDevInfo.DevIdentifier[1], 
		    dev->ptDpmAddress->tDevInfo.DevIdentifier[2], ptGetInfoCmd->usInfoLen);
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)&(dev->ptDpmAddress->tDevInfo),
			   ptGetInfoCmd->usInfoLen);
	    //	    DBG_PRN("CIF_IOCTLGETINFO: (GET_DEV_INFO) DevId: (%c)(%c)(%c),(%c-%c-%c)\n", 
	    //ptGetInfoCmd->abInfoData[0],ptGetInfoCmd->abInfoData[1],ptGetInfoCmd->abInfoData[2], 
	    //ptGetInfoCmd->abInfoData[3],ptGetInfoCmd->abInfoData[4],ptGetInfoCmd->abInfoData[5]);
            // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
            break;

          case GET_IO_INFO:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_IO_INFO)\n");
	    ptGetInfoCmd->pabInfoData = (unsigned char *)kmalloc(ptGetInfoCmd->usInfoLen, GFP_KERNEL);
            ((IOINFO *)(ptGetInfoCmd->pabInfoData))->bComBit         =(unsigned char)cif_test_com (dev);
            ((IOINFO *)(ptGetInfoCmd->pabInfoData))->bIOExchangeMode =(unsigned char)((dev->ptDpmAddress->tRcsInfo.DriverType & 0xF0) >> 4);
            ((IOINFO *)(ptGetInfoCmd->pabInfoData))->ulIOExchangeCnt = dev->tStateInfo.ExIOCnt;
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)(ptGetInfoCmd->pabInfoData),
			   ptGetInfoCmd->usInfoLen);
            // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
	    kfree(ptGetInfoCmd->pabInfoData);
            break;

          case GET_IO_SEND_DATA:
	    DBG_PRN("CIF_IOCTLGETINFO: (GET_IO_SEND_DATA)\n");
            copy_to_user ( (unsigned char *)(pvData->pabInfoData),
			   (unsigned char *)dev->pbDpmSendArea,
			   ptGetInfoCmd->usInfoLen);
            // return length of all output data
            lRet += ptGetInfoCmd->usInfoLen;
            break;

          default:
            // InfoArea unknown
            ptGetInfoCmd->sError = DRV_PARAMETER_UNKNOWN;
	    lRet = 0;
        }
      }
      DBG_PRN("CIF_IOCTLGETINFO: END\n");
      copy_to_user ( (unsigned char *)arg, 
		     (unsigned char *)ptGetInfoCmd,    
		     sizeof( DEVIO_GETDEVINFOCMD) );
      kfree( ptGetInfoCmd);
      break;

    //****************************************************************************************
    //  SetHostState
    //****************************************************************************************

    case CIF_IOCTLSETHOST:        // set the host state flag

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      ptGetTriggerCmd = (DEVIO_TRIGGERCMD  *)kmalloc( sizeof(DEVIO_TRIGGERCMD), GFP_KERNEL);
      copy_from_user ( (unsigned char *)ptGetTriggerCmd,
                       (unsigned char *)arg,
		       sizeof( DEVIO_TRIGGERCMD) );
      ptGetTriggerCmd->sError = 0;    // clear error
      DBG_PRN("CIF_IOCTLSETHOST: usBoard (%d)\n",ptGetTriggerCmd->usBoard);
      // at last one application must be init
      if ( ptGetTriggerCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLSETHOST: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( ptGetTriggerCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLSETHOST: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0){
	  DBG_PRN("CIF_IOCTLSETHOST: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if ( cif_test_ready(dev) == FALSE) {          // this reflects the DEV RDY flag
        // DEV RDY failed
	DBG_PRN("CIF_IOCTLSETHOST: device READY failed\n");
        ptGetTriggerCmd->sError = DRV_DEV_NOT_READY;
      } else if ( (dev->bCOMEqState  != FALSE) ||         // test for service activ
                  (dev->bCOMNeqState != FALSE)  ) {
        // service is active
	DBG_PRN("CIF_IOCTLSETHOST: SetHostSate CMD ACTIVE\n");
        ptGetTriggerCmd->sError = DRV_CMD_ACTIVE;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        // check DEV RDY flag

        // clear host ready and wait for COM-bit is set
        if ( ptGetTriggerCmd->usMode == HOST_READY) {
	  DBG_PRN("CIF_IOCTLHOSTSTATE: Acquire SYNCH POINT\n");
	  // disable interrupt until we are ready
	  /* The process context must use spin_lock_irq() because it knows that       */
	  /* interrupts are always enabled while executing the device ioctl() method. */
 	  spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex_ioctl);

	  // clear host ready bit in bDevFlags and write it
	  cif_clear_host_state(dev);

	  // test if COM bit is set
	  if ( cif_test_com ( dev) == TRUE) {
            // COM bit is set
            // enable interrupt we are ready
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq( &dev->mutex_ioctl);
          } else if ( ptGetTriggerCmd->ulTimeout == 0L ) {
            // No Timeout, don't wait for the COM-Bit
            // enable interrupt we are ready
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq( &dev->mutex_ioctl); 
          } else {
            // we have to wait for the COM-bit
            dev->bCOMNeqState = TRUE;

            // enable interrupt we are ready
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq( &dev->mutex_ioctl);

            if ( cif_wait_com(dev, ptGetTriggerCmd->ulTimeout) == FALSE) {
              // Timeout, error to user
	      DBG_PRN("CIF_IOCTLHOSTSTATE: Timeout wait for COM-Bit SET\n");
              ptGetTriggerCmd->sError = DRV_DEV_NO_COM_FLAG;
            }
            dev->bCOMNeqState = FALSE;
          }
          DBG_PRN("                Host state SET OK\n");
        } else {
          // disable interrupt until we are ready
          spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex_ioctl);

          // set host ready bit in bDevFlags and write it
          cif_set_host_state(dev);

          // test if COM bit is clear
          if ( cif_test_com ( dev) == FALSE) {
            // COM bit is clear
            // enable interrupt we are ready
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq( &dev->mutex_ioctl);
          } else if ( ptGetTriggerCmd->ulTimeout == 0L ) {
            // No Timeout, don't wait for the COM-Bit
            // enable interrupt we are ready
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);
          } else {
            // we have to wait for COM-bit is clear
            dev->bCOMEqState = TRUE;
            // enable interrupt we are ready
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);
            if ( cif_wait_com( dev, ptGetTriggerCmd->ulTimeout) == FALSE) {
              // Timeout, error to user
	      DBG_PRN("CIF_IOCTLHOSTSTATE: Timeout wait for COM-Bit CLEAR\n");
              ptGetTriggerCmd->sError = DRV_DEV_NO_COM_FLAG;
            }
            dev->bCOMEqState = FALSE;
          }
	  DBG_PRN("Host state CLEAR OK\n");
        }
      }
      // return length of output data
      lRet = sizeof(ptGetTriggerCmd->sError);
      copy_to_user ( (unsigned char *)arg,
		     (unsigned char *)ptGetTriggerCmd,
		     sizeof( DEVIO_TRIGGERCMD) );
      kfree ( ptGetTriggerCmd);
      break;

    //****************************************************************************************
    //  GETTASKSTATE
    //****************************************************************************************

    case CIF_IOCTLTASKSTATE:       // read Task state data

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pGetTskStateCmd = (DEVIO_GETTASKSTATECMD *)kmalloc( sizeof(DEVIO_GETTASKSTATECMD), GFP_KERNEL);
      copy_from_user ( (unsigned char *)pGetTskStateCmd,
                       (unsigned char *)arg,
		       sizeof( DEVIO_GETTASKSTATECMD) );
      pGetTskStateCmd->sError = 0;    // clear error
      // at last one application must be init
      if ( pGetTskStateCmd->ucBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLTASKSTATE: BOARD NOT INITIALIZED\n");
	  pGetTskStateCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pGetTskStateCmd->ucBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLTASKSTATE: BOARD NOT INITIALIZED\n");
	  pGetTskStateCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0){
	  DBG_PRN("CIF_IOCTLTASKSTATE: BOARD NOT INITIALIZED\n");
	  pGetTskStateCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        if (pGetTskStateCmd->usStateNum == 1) {
          // copy data do applicaton buffer
          memcpy ( (unsigned char *)&(pGetTskStateCmd->TaskState[0]), 
		   (unsigned char *)&(dev->ptDpmAddress->tKpt1State),
		   pGetTskStateCmd->usStateLen);
	  DBG_PRN("GetTaskState Task 1 OK\n");
        } else {
          // copy data do applicaton buffer
          memcpy ( (unsigned char *)&(pGetTskStateCmd->TaskState[0]), 
		   (unsigned char *)&(dev->ptDpmAddress->tKpt2State),
		   pGetTskStateCmd->usStateLen);
	  DBG_PRN("GetTaskState Task 2 OK\n");
        }
      }
      copy_to_user ( (unsigned char *)arg,
		     (unsigned char *)pGetTskStateCmd,
		     sizeof( DEVIO_GETTASKSTATECMD) );
      // return length of output data
      lRet = sizeof(DEVIO_GETTASKSTATECMD);
      kfree( pGetTskStateCmd);
      break;

    //****************************************************************************************
    //  MAILBOXINFO
    //****************************************************************************************

    case CIF_IOCTLMBXINFO:        // get MBX info

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      ptGetMBXInfoCmd = (DEVIO_MBXINFOCMD *)kmalloc( sizeof(DEVIO_MBXINFOCMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)ptGetMBXInfoCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_MBXINFOCMD) );
      ptGetMBXInfoCmd->sError = 0;    // clear error
      DBG_PRN("CIF_IOCTLMBXINFO: BOARD %d, nr.devs = %d\n", ptGetMBXInfoCmd->ucBoard, cif_nr_devs);
      // at last one application must be init
      if ( ptGetMBXInfoCmd->ucBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLMBXINFO: BOARD NOT INITIALIZED\n");
	  ptGetMBXInfoCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( ptGetMBXInfoCmd->ucBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLMBXINFO: BOARD NOT INITIALIZED\n");
	  ptGetMBXInfoCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0){
	  DBG_PRN("CIF_IOCTLMBXINFO: BOARD NOT INITIALIZED\n");
	  ptGetMBXInfoCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
	// load DEV instance to real structure
        //----------------------------------
        // all things well lets work
        //----------------------------------
        // check DEV RDY flag
        if ( cif_test_ready(dev) == FALSE) {
          // DEV RDY failed
	  DBG_PRN("CIF_IOCTLMBXINFO: device READY failed\n");
          ptGetMBXInfoCmd->sError = DRV_DEV_NOT_READY;
        } else {
          ptGetMBXInfoCmd->usDevMbxState  = cif_get_devMBXinfo(dev);
          ptGetMBXInfoCmd->usHostMbxState = cif_get_hostMBXinfo(dev);
	  DBG_PRN("CIF_IOCTLMBXINFO: devMBX: %X, hostMBX: %X\n",
		  ptGetMBXInfoCmd->usDevMbxState,ptGetMBXInfoCmd->usHostMbxState);
        }
      }
      copy_to_user  ( (unsigned char *)arg,
		      (unsigned char *)ptGetMBXInfoCmd,
		      sizeof( DEVIO_MBXINFOCMD) );
      // return length of output data
      lRet = sizeof(DEVIO_MBXINFOCMD);
      kfree( ptGetMBXInfoCmd);
      break;

    //****************************************************************************************
    //  EXITDRV
    //****************************************************************************************

    case CIF_IOCTLEXITDRV:    // application exits
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pExitCmd  = (DEVIO_EXITCMD  *)kmalloc( sizeof(DEVIO_EXITCMD), GFP_KERNEL); // pointer to Command data
      copy_from_user  ( (unsigned char *)pExitCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_EXITCMD) );
      pExitCmd->sError = 0;    // clear error
      DBG_PRN("CIF_IOCTLEXITDRV: ucBoard (%d)\n", pExitCmd->usBoard);

      if ( pExitCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLEXITDRV: BOARD NOT INITIALIZED\n");
	  pExitCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pExitCmd->usBoard)) == NULL) {
	  DBG_PRN("CIF_IOCTLEXITDRV: BOARD NOT INITIALIZED\n");
	  pExitCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0) {
          pExitCmd->usDrvOpenCount = dev->usOpenCounter;
	  DBG_PRN("CIF_IOCTLEXITDRV: BOARD NOT INITIALIZED\n");
	  pExitCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        /* check for open read/write */
        if ( dev->usOpenCounter == 1) {
          /* last close */
          // disable interrupt until we are ready
          spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex_ioctl);

          if (dev->bReadState == TRUE) {
            dev->bReadState = FALSE;
	    wake_up_interruptible( &(dev->pReadSemaphore));
          } /* endif */

          if (dev->bWriteState == TRUE) {
            dev->bWriteState = FALSE;
	    wake_up_interruptible( &(dev->pWriteSemaphore));
          } /* endif */

          if (dev->bInitState == TRUE) {
            dev->bInitState = FALSE;
	    wake_up_interruptible( &(dev->pInitSemaphore));
          } /* endif */

          if ( (dev->bExIOEqState == TRUE) ||
               (dev->bExIONeqState == TRUE)  ) {
            dev->bInitState = FALSE;
	    wake_up_interruptible( &(dev->pInitSemaphore));
          } /* endif */

          if ( (dev->bCOMEqState == TRUE) ||
               (dev->bCOMNeqState == TRUE)  ) {
            dev->bInitState = FALSE;
	    wake_up_interruptible( &(dev->pInitSemaphore));
          } /* endif */

          // enable interrupt we are ready
          spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);

        } /* endif */

        dev->usOpenCounter--;    // Number of applications
        dev->tStateInfo.OpenCnt = dev->usOpenCounter;

        // return OpenCounter for application interface
        pExitCmd->usDrvOpenCount = dev->usOpenCounter;

        DBG_PRN("exit OK, OpenCount = (%d)\n", dev->usOpenCounter);
      }
      DBG_PRN("exit OK, OpenCount = (%d)\n", dev->usOpenCounter);
      copy_to_user  ( (unsigned char *)arg, 
		      (unsigned char *)pExitCmd,
		      sizeof( DEVIO_EXITCMD) );

      // length of output data
      lRet = sizeof(DEVIO_EXITCMD);
      kfree( pExitCmd);
      break;

    //****************************************************************************************
    //  CIF_ReadSendData
    //****************************************************************************************

    case CIF_IOCTLREADSEND:           // Read the send data area

      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pReadSendCmd  = (DEVIO_READSENDCMD *)kmalloc( sizeof(DEVIO_READSENDCMD), GFP_KERNEL); // pointer to Command data
      copy_from_user  ( (unsigned char *)pReadSendCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_READSENDCMD) );
      pReadSendCmd->sError = 0;    // clear error
      // length of output data
      lRet = sizeof(pReadSendCmd->sError);
      if (  pReadSendCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLREADSEND: BOARD NOT INITIALIZED\n");
	  pReadSendCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pReadSendCmd->usBoard)) == NULL) {
	  DBG_PRN("CIF_IOCTLREADSEND: BOARD NOT INITIALIZED\n");
	  pReadSendCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0) {
	  DBG_PRN("CIF_IOCTLREADSEND: BOARD NOT INITIALIZED\n");
	  pReadSendCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if ( cif_test_ready(dev) == FALSE) {       // this reflects the DEV RDY flag
        // DEV RDY failed
        pReadSendCmd->sError = DRV_DEV_NOT_READY;
      } else if ( cif_test_com (dev) == FALSE ) {         // this reflects the COM-Bit
        // COM failed
        pReadSendCmd->sError = DRV_DEV_NO_COM_FLAG;
      } else {
        // read data from the send area
        copy_to_user ( (unsigned char *)pReadSendCmd->pabReadData,
                       (unsigned char *)&(dev->pbDpmSendArea[pReadSendCmd->usReadOffset]),
		       pReadSendCmd->usReadLen );
        // return length of output data
        lRet += pReadSendCmd->usReadLen;
      }
      copy_to_user  ( (unsigned char *)arg,
		      (unsigned char *)pReadSendCmd,
		      sizeof( DEVIO_READSENDCMD) );
      kfree( pReadSendCmd);
      break;

    //****************************************************************************************
    //  Read Send and Receive mailbox
    //****************************************************************************************

    case CIF_IOCTLGETMBX:              // Check extended version
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pGetMbxCmd = (DEVIO_GETMBXCMD  *)kmalloc( sizeof(DEVIO_GETMBXCMD), GFP_KERNEL); // pointer to Command data
      copy_from_user  ( (unsigned char *)pGetMbxCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_GETMBXCMD) );
      pGetMbxCmd->sError = 0;   // Clear error
      // length of output data
      lRet = sizeof(pGetMbxCmd->sError);
      if ( pGetMbxCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLLGETMBX: BOARD NOT INITIALIZED\n");
	  pGetMbxCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pGetMbxCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLLGETMBX: BOARD NOT INITIALIZED\n");
	  pGetMbxCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {// Copy send and receive mbx into user data
        // read data
        memcpy ( (unsigned char *)pGetMbxCmd->abDevMbx,
		 (unsigned char *)&(dev->ptDpmAddress->tDevMbx),
		 pGetMbxCmd->usDevLen);

        memcpy ( (unsigned char *)pGetMbxCmd->abHostMbx,
		 (unsigned char *)&(dev->ptDpmAddress->tPcMbx),
		 pGetMbxCmd->usHostLen);

	DBG_PRN("Get MBX data OK\n");
	copy_to_user  ( (unsigned char *)arg,
			(unsigned char *)pGetMbxCmd,
			sizeof( DEVIO_GETMBXCMD) );
      }
      // return length of output data
      lRet += pGetMbxCmd->usDevLen + pGetMbxCmd->usHostLen;
      kfree( pGetMbxCmd);
      break;

    //****************************************************************************************
    //  TRIGGERWATCHDOG
    //****************************************************************************************

    case CIF_IOCTLTRIGGERWD:  // triggers the DEV watchdog
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      ptGetTriggerCmd  = (DEVIO_TRIGGERCMD *)kmalloc( sizeof(DEVIO_TRIGGERCMD), GFP_KERNEL); 
      copy_from_user  ( (unsigned char *)ptGetTriggerCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_TRIGGERCMD) );
      ptGetTriggerCmd->sError = 0;    // clear error

      // load DEV instance to real structure
      DBG_PRN("CIF_IOCTLTRIGGERWD: ucBoard (%d)\n",ptGetTriggerCmd->usBoard);
      // at last one application must be init
      if (  ptGetTriggerCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLTRIGGERWD: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( ptGetTriggerCmd->usBoard)) == NULL) {
	  DBG_PRN("CIF_IOCTLTRIGGERWD: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0) {
	  DBG_PRN("CIF_IOCTLTRIGGERWD: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        // check DEV RDY flag
        if ( cif_test_ready(dev) == FALSE) {  // this reflects the DEV RDY flag
          // DEV RDY failed
	  DBG_PRN("CIF_IOCTLTRIGGERWD: device READY failed\n");
          ptGetTriggerCmd->sError = DRV_DEV_NOT_READY;
        } else if (ptGetTriggerCmd->usMode == 1) {
          // run wd trigger, read PcWatchDog, write to DevWatchDog
          ptGetTriggerCmd->usTriggerValue = (unsigned short)dev->ptDpmAddress->tRcsInfo.HostWatchDog;
          dev->ptDpmAddress->tRcsInfo.DevWatchDog = (unsigned char)ptGetTriggerCmd->usTriggerValue;
          //DBG_PRN("                WatchDog trigger OK\n");
        } else {
          // stop wd trigger
          dev->ptDpmAddress->tRcsInfo.DevWatchDog = 0;
          ptGetTriggerCmd->usTriggerValue = 0;
          //DBG_PRN("                WatchDog STOP\n");
         }
      }
      copy_to_user  ( (unsigned char *)arg,
		      (unsigned char *)ptGetTriggerCmd,
		      sizeof( DEVIO_TRIGGERCMD) );
      // return length of output data
      lRet = sizeof(DEVIO_TRIGGERCMD);
      kfree( ptGetTriggerCmd);
      break;

    //****************************************************************************************
    //  SpecialControl
    //****************************************************************************************

    case CIF_IOCTLSPCONTROL:             // Set the special control BIT in the last DPM cell
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      ptGetTriggerCmd  = (DEVIO_TRIGGERCMD  *)kmalloc(sizeof(DEVIO_TRIGGERCMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)ptGetTriggerCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_TRIGGERCMD) );
      ptGetTriggerCmd->sError = 0;    // clear error
      // load DEV instance to real structure
      DBG_PRN("CIF_IOCTLSPCONTROL: ucBoard (%d)\n",ptGetTriggerCmd->usBoard);
      // at last one application must be init
      if (  ptGetTriggerCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLSPCONTROL: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( ptGetTriggerCmd->usBoard)) == NULL) {
	  DBG_PRN("CIF_IOCTLSPCONTROL: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0) {
	  DBG_PRN("CIF_IOCTLSPCONTROL: BOARD NOT INITIALIZED\n");
	  ptGetTriggerCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if ( cif_test_ready(dev) == FALSE) {          // this reflects the DEV RDY flag
        // DEV RDY failed
	DBG_PRN("CIF_IOCTLSPCONTROL: device READY failed\n");
	ptGetTriggerCmd->sError = DRV_DEV_NOT_READY;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        // disable interrupt until we are ready
        spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex_ioctl);
        // clear host ready and wait for COM-bit is set
        switch ( ptGetTriggerCmd->usMode) {
          case SPECIAL_CONTROL_CLEAR:
            if ( dev->usBoardIrq != 0) {
              dev->bMyDevFlags  &= (~SPCCOM_FLAG);
            } else {
              dev->bMyDevFlags = (unsigned char)(*(dev->pbDevFlags) & (~SPCCOM_FLAG));
            }
            *(dev->pbDevFlags)  = dev->bMyDevFlags;
            break;

          case SPECIAL_CONTROL_SET:
            if ( dev->usBoardIrq != 0) {
              dev->bMyDevFlags |= SPCCOM_FLAG;
            } else {
              dev->bMyDevFlags = (unsigned char)(*(dev->pbDevFlags) | SPCCOM_FLAG);
            }
            *(dev->pbDevFlags)  = dev->bMyDevFlags;
            break;
          default:
            ptGetTriggerCmd->sError = DRV_USR_MODE_INVALID;
            break;
        } /* endswitch */
        spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);
        // Deliever actual SPC_CTRL_ACK state
        if ( dev->usBoardIrq != 0) {
          // copy actual state information, interrupt is running
          if (dev->bHostFlags &  SPCACK_FLAG) ptGetTriggerCmd->usTriggerValue = SPECIAL_CONTROL_SET;
          else ptGetTriggerCmd->usTriggerValue = SPECIAL_CONTROL_CLEAR;
        } else {
          // copy actual state information, polling is activ
          if (*(dev->pbHostFlags) & SPCACK_FLAG) ptGetTriggerCmd->usTriggerValue = SPECIAL_CONTROL_SET;
          else ptGetTriggerCmd->usTriggerValue = SPECIAL_CONTROL_CLEAR;
        }
      }
      copy_to_user  ( (unsigned char *)arg,
		      (unsigned char *)ptGetTriggerCmd,
		      sizeof( DEVIO_TRIGGERCMD) );
      // return length of output data
      lRet = sizeof(ptGetTriggerCmd->sError);
      kfree( ptGetTriggerCmd);
      break;

    //****************************************************************************************
    //  CIF_GetTaskParameter
    //****************************************************************************************

    case CIF_IOCTLGETPARAMETER:     // read task parameters
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pGetParamCmd = (DEVIO_GETPARAMETERCMD *)kmalloc(sizeof(DEVIO_GETPARAMETERCMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pGetParamCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_GETPARAMETERCMD) );
      pGetParamCmd->sError = 0;    // clear error
      // length of output data
      lRet = sizeof(pGetParamCmd->sError);
      DBG_PRN("IOCTGETPARAM: ucBoard (%d)\n",pGetParamCmd->usBoard);
      if (  pGetParamCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLSPCONTROL: BOARD NOT INITIALIZED\n");
	  pGetParamCmd->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pGetParamCmd->usBoard)) == NULL) {
	  DBG_PRN("CIF_IOCTLSPCONTROL: BOARD NOT INITIALIZED\n");
	  pGetParamCmd->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else if( dev->usOpenCounter == 0) {
	  DBG_PRN("CIF_IOCTLSPCONTROL: BOARD NOT INITIALIZED\n");
	  pGetParamCmd->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        switch (pGetParamCmd->usTaskParamNum) {
          case 1:   // Task 1
            memcpy ( (unsigned char *)&(pGetParamCmd->TaskParameter[0]),
		     (unsigned char *)&(dev->ptDpmAddress->tKpt1Param),
		     pGetParamCmd->usTaskParamLen);
            lRet += pGetParamCmd->usTaskParamLen;
	    DBG_PRN("GetParameter Task 1 OK\n");
            break;
          case 2:   // Task 2
            memcpy ( (unsigned char *)&(pGetParamCmd->TaskParameter[0]),
		     (unsigned char *)&(dev->ptDpmAddress->tKpt2Param),
		     pGetParamCmd->usTaskParamLen);
            lRet += pGetParamCmd->usTaskParamLen;
	    DBG_PRN("GetParameter Task 2 OK\n");
            break;
          default:
            // parameter error
            pGetParamCmd->sError = DRV_USR_NUMBER_INVALID;
	    lRet = -1;
            break;
        } // end switch
        copy_to_user ( (unsigned char *)arg,
                       (unsigned char *)pGetParamCmd,
		       sizeof( DEVIO_GETPARAMETERCMD) );
      }
      kfree( pGetParamCmd);
      break;

    //****************************************************************************************
    //  Check extended Version
    //****************************************************************************************

    case CIF_IOCTLEXTDATA:              // Check extended version
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pExtCmd = (DEVIO_EXTDATACMD *)kmalloc(sizeof(DEVIO_EXTDATACMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pExtCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_EXTDATACMD) );
      pExtCmd->sError = 0;   // Clear error
      if ( pExtCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLEXTDATA: BOARD NOT INITIALIZED\n");
	  pExtCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pExtCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLEXTDATA: BOARD NOT INITIALIZED\n");
	  pExtCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        // search string in DPM of the boards
        // Check for board is available
        if ( dev->sDrvInitError == DRV_DEV_DPM_ACCESS_ERROR) {
          // Return error
          pExtCmd->sError = dev->sDrvInitError;
        } else {
          // DPM access is AVAILABLE, board is activ
          // Check all entries of all boards
          pExtCmd->sError = cif_test_ext_version(dev, pExtCmd->usMode, pExtCmd->pabExtData);
        }
      }
      // return length of output data
      lRet = sizeof(DEVIO_EXTDATACMD);
      kfree( pExtCmd);
      break;

    //****************************************************************************************
    //  ReadWrite raw data
    //****************************************************************************************

    case CIF_IOCTLRWRAW:             // Read and write data of the last kByte
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pRWRawCmd = (DEVIO_RWRAWDATACMD *)kmalloc(sizeof(DEVIO_RWRAWDATACMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pRWRawCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_RWRAWDATACMD) );
      DBG_PRN("CIF_IOCTLRWRAW: ucBoard (%d)\n",pRWRawCmd->usBoard);
      pRWRawCmd->sError = 0;   // Clear error
      // length of output data
      lRet = sizeof(pRWRawCmd->sError);
      if ( pRWRawCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLRWRAW: BOARD NOT INITIALIZED\n");
	  pRWRawCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pRWRawCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLRWRAW: BOARD NOT INITIALIZED\n");
	  pRWRawCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        switch ( pRWRawCmd->usMode) {
          case PARAMETER_READ:
            // Read data
            memcpy ( (unsigned char *)pRWRawCmd->pabData,
		     (unsigned char *)&(dev->ptDpmAddress->tDevMbx) + pRWRawCmd->usOffset,
		     pRWRawCmd->usLen);
            break;
          case PARAMETER_WRITE:
            // Write data
            memcpy ( (unsigned char *)&(dev->ptDpmAddress->tDevMbx) + pRWRawCmd->usOffset,
		     (unsigned char *)pRWRawCmd->pabData,
		     pRWRawCmd->usLen);
            break;

          default:
            pRWRawCmd->sError = DRV_USR_MODE_INVALID;
        } /* endswitch */
      }
      // return length of output data
      lRet += pRWRawCmd->usLen;
      kfree( pRWRawCmd);
      break;

    //****************************************************************************************
    //  ReadWriteDPMData
    //****************************************************************************************

    case CIF_IOCTLRWDPMDATA:             // Read and write data of the whole DPM
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pRWDPMCmd = (DEVIO_RWDPMDATACMD *)kmalloc(sizeof(DEVIO_RWDPMDATACMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pRWDPMCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_RWDPMDATACMD) );
      DBG_PRN("CIF_IOCTLRWDPMDATA: ucBoard (%d)\n",pRWDPMCmd->usBoard);
      pRWDPMCmd->sError = 0;     // Clear error
      // length of output data
      lRet = sizeof(pRWDPMCmd->sError);
      if ( pRWDPMCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLRWDPMDATA: BOARD NOT INITIALIZED\n");
	  pRWDPMCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pRWDPMCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLRWDPMDATA: BOARD NOT INITIALIZED\n");
	  pRWDPMCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {      
        switch ( pRWDPMCmd->usMode) {
          case PARAMETER_READ:
            // Read data
            memcpy ( (unsigned char *)pRWDPMCmd->pabData,
		     (unsigned char *)(dev->pDpmBase + pRWDPMCmd->usOffset),
		     pRWDPMCmd->usLen);
            break;
          case PARAMETER_WRITE:
            // Write data
            memcpy ( (unsigned char *)(dev->pDpmBase + pRWDPMCmd->usOffset),
		     (unsigned char *)pRWDPMCmd->pabData,
		     pRWDPMCmd->usLen);
            break;
          default:
            pRWDPMCmd->sError = DRV_USR_MODE_INVALID;
        } /* endswitch */
      }
      // return length of output data
      lRet += pRWDPMCmd->usLen;
      kfree( pRWDPMCmd);
      break;

    //****************************************************************************************
    //  CIF_ExchangeIO
    //****************************************************************************************

    case CIF_IOCTLEXIO:             // IO data exchange
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pExIO    = (DEVIO_EXIOCMD *)arg;
      pExIOCmd = (DEVIO_EXIOCMD *)kmalloc(sizeof(DEVIO_EXIOCMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pExIOCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_EXIOCMD) );
      DBG_PRN("CIF_IOCTLEXIO: -ExchangeIO-   usBoard (%d)\n",pExIOCmd->usBoard);
      pExIOCmd->sError = 0; // Clear command
      lRet = sizeof(pExIOCmd->sError);
      if ( pExIOCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLEXIO: BOARD NOT INITIALIZED\n");
	  pExIOCmd->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pExIOCmd->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLEXIO: BOARD NOT INITIALIZED\n");
	  pExIOCmd->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else {      
	pExIOCmd->pabSendData    = (unsigned char *)kmalloc(pExIOCmd->usSendLen, GFP_KERNEL);
	copy_from_user( (unsigned char *)(pExIOCmd->pabSendData),
			(unsigned char *)(pExIO->pabSendData),
			pExIOCmd->usSendLen);
	pExIOCmd->pabReceiveData = (unsigned char *)kmalloc(pExIOCmd->usReceiveLen, GFP_KERNEL);
	//----------------------------------
	// run device exchange IO function
	//----------------------------------
	lRet = cif_exchange_io( dev, 
			        pExIOCmd, 
				(unsigned char)((dev->ptDpmAddress->tRcsInfo.DriverType & 0xF0) >>4));
	DBG_PRN("CIF_IOCTLEXIO: lRet = (%ld)\n", lRet);
	copy_to_user( (unsigned char *)(pExIO->pabReceiveData),
		      (unsigned char *)(pExIOCmd->pabReceiveData),
		      pExIOCmd->usReceiveLen);
	kfree( pExIOCmd->pabSendData);
	kfree( pExIOCmd->pabReceiveData);
      }
      copy_to_user( (unsigned char *)arg,
		    (unsigned char *)pExIOCmd,
		    sizeof( DEVIO_EXIOCMD) );
      kfree( pExIOCmd);
      break;

    //****************************************************************************************
    //  DevExchangeIOEx
    //****************************************************************************************

    case CIF_IOCTLEXIOEX:           // IO data exchange with external mode
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pExIOEx    = (DEVIO_EXIOCMDEX *)arg;
      pExIOCmdEx = (DEVIO_EXIOCMDEX *)kmalloc(sizeof(DEVIO_EXIOCMDEX), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pExIOCmdEx,
			(unsigned char *)arg,
			sizeof( DEVIO_EXIOCMDEX) );
      pExIOCmdEx->sError = 0; // Clear command
      DBG_PRN("CIF_IOCTLEXIOEX: usBoard (%d)\n",pExIOCmdEx->usBoard);
      if ( pExIOCmdEx->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLEXIOEX(1): BOARD NOT INITIALIZED\n");
	  pExIOCmdEx->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pExIOCmdEx->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLEXIOEX(2): BOARD NOT INITIALIZED\n");
	  pExIOCmdEx->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else {      
	pExIOCmdEx->pabSendData = (unsigned char *)kmalloc(pExIOCmdEx->usSendLen, GFP_KERNEL);
	copy_from_user( (unsigned char *)(pExIOCmdEx->pabSendData),
			(unsigned char *)(pExIOEx->pabSendData),
			pExIOCmdEx->usSendLen);
	pExIOCmdEx->pabReceiveData = (unsigned char *)kmalloc(pExIOCmdEx->usReceiveLen, GFP_KERNEL);
	//----------------------------------
	// run device exchange IO function
	//----------------------------------
	lRet = cif_exchange_io(  dev, 
				 (DEVIO_EXIOCMD *)pExIOCmdEx,
				 (unsigned char)pExIOCmdEx->usMode );
	DBG_PRN("CIF_IOCTLEXIO: lRet = (%ld)\n", lRet);
	copy_to_user( (unsigned char *)(pExIOEx->pabReceiveData),
		      (unsigned char *)(pExIOCmdEx->pabReceiveData),
		      pExIOCmdEx->usReceiveLen);
	kfree( pExIOCmdEx->pabSendData);
	kfree( pExIOCmdEx->pabReceiveData);
      }
      copy_to_user( (unsigned char *)arg,
		    (unsigned char *)pExIOCmdEx,
		    sizeof( DEVIO_EXIOCMDEX) );
      kfree( pExIOCmdEx);
      break;

    //****************************************************************************************
    //  DevExchangeIOErr
    //****************************************************************************************

    case CIF_IOCTLEXIOERR:           // IO data exchange
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pExIOErr    = (DEVIO_EXIOCMDERR *)arg;
      pExIOCmdErr = (DEVIO_EXIOCMDERR *)kmalloc(sizeof(DEVIO_EXIOCMDERR), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pExIOCmdErr,
			(unsigned char *)arg,
			sizeof( DEVIO_EXIOCMDERR) );
      pExIOCmdErr->sError = 0; // Clear command
      DBG_PRN("IOCTLEXIOERR(0) : usBoard (%d)\n",pExIOCmdErr->usBoard);
      if ( pExIOCmdErr->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLEXIOERR(1): BOARD NOT INITIALIZED\n");
	  pExIOCmdErr->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pExIOCmdErr->usBoard)) == NULL){
	  DBG_PRN("CIF_IOCTLEXIOERR(2): BOARD NOT INITIALIZED\n");
	  pExIOCmdErr->sError = DRV_BOARD_NOT_INITIALIZED;
	  lRet = -1;
      } else {      
	pExIOCmdErr->pabSendData = (unsigned char *)kmalloc(pExIOCmdErr->usSendLen, GFP_KERNEL);
	copy_from_user( (unsigned char *)(pExIOCmdErr->pabSendData),
			(unsigned char *)(pExIOErr->pabSendData),
			pExIOCmdErr->usSendLen);
	pExIOCmdErr->pabReceiveData = (unsigned char *)kmalloc(pExIOCmdErr->usReceiveLen, GFP_KERNEL);
	pExIOCmdErr->ptStateData = (COMSTATE *)kmalloc( sizeof( COMSTATE), GFP_KERNEL);

	lRet = cif_exchange_io( dev, (DEVIO_EXIOCMD *)pExIOCmdErr, 
			       (unsigned char)((dev->ptDpmAddress->tRcsInfo.DriverType & 0xF0) >> 4) );
	// get COM error state and flags
	// Return actual COM error state
	pExIOCmdErr->ptStateData->usMode =(unsigned short)(dev->ptDpmAddress->tRcsInfo.DriverType & 0x0F);
	switch (pExIOCmdErr->ptStateData->usMode) {// swicht error mode
        case STATE_MODE_3:
	  DBG_PRN("CIF_IOCTLEXIOERR: state mode 1\n");
          // Transfer only StateFlag from HostStates !!!!!
          if ( dev->usBoardIrq == 0) {
            // polling mode
            if ( (*(dev->pbHostFlags) & STATECOM_FLAG ) == 0x00)
              usDummy = STATE_ERR_NON;
            else
              usDummy = STATE_ERR;
          } else {// Interrupt mode, read only host state            
            if ( (dev->bHostFlags & STATECOM_FLAG) == 0x00)
              usDummy = STATE_ERR_NON;
            else
              usDummy = STATE_ERR;
          }
          // Set usStateFlag
          pExIOCmdErr->ptStateData->usStateFlag = usDummy;
          // Copy state field into buffer
          memcpy ( (unsigned char *)pExIOCmdErr->ptStateData->abState,
		   (unsigned char *)&(dev->ptDpmAddress->tKpt2State),
		   sizeof(pExIOCmdErr->ptStateData->abState));
          break;
        case STATE_MODE_4:
	  DBG_PRN("CIF_IOCTLEXIOERR: state mode 2\n");
          // disable interrupt
	  spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex_ioctl);
          // Get actual state
          if((pExIOCmdErr->ptStateData->usStateFlag = (unsigned char)cif_get_state(dev)) == STATE_ERR) {
            // copy state field
            memcpy ( (unsigned char *)pExIOCmdErr->ptStateData->abState,
		     (unsigned char *)&(dev->ptDpmAddress->tKpt2State),
		     sizeof(pExIOCmdErr->ptStateData->abState));
            // Toggle quittung
            cif_state_done(dev);
          }
          // enable interrupt
	  spin_unlock_irqrestore ( &dev->mutex, flags);
          break;
        default:
          // State mode not available (unknown), set usStateFlag to 0xFF
          // to signal, function not implemented
          pExIOCmdErr->ptStateData->usStateFlag = 0xFF;
          break;
	} /* end switch */
	copy_to_user( (unsigned char *)(pExIOErr->pabReceiveData),
		      (unsigned char *)(pExIOCmdErr->pabReceiveData),
		      pExIOCmdErr->usReceiveLen);
	copy_to_user( (unsigned char *)(pExIOErr->ptStateData),
		      (unsigned char *)(pExIOCmdErr->ptStateData),
		      sizeof( COMSTATE));
	kfree( pExIOCmdErr->pabSendData);
	kfree( pExIOCmdErr->pabReceiveData);
	kfree( pExIOCmdErr->ptStateData);
      }
      copy_to_user( (unsigned char *)arg,
		    (unsigned char *)pExIOCmdErr,
		    sizeof( DEVIO_EXIOCMDERR) );
      kfree( pExIOCmdErr);
      break;

    //****************************************************************************************
    //  PUTMESSAGE
    //****************************************************************************************

    case CIF_IOCTLPUTMSG:     // send message
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pPutMsgCmd = (DEVIO_PUTMESSAGECMD *)kmalloc(sizeof(DEVIO_PUTMESSAGECMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pPutMsgCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_PUTMESSAGECMD) );
      DBG_PRN("CIF_IOCTLPUTMSG:  usBoard (%d), timeout 0x%lX\n",pPutMsgCmd->usBoard, pPutMsgCmd->ulTimeout);
      pPutMsgCmd->sError = 0;   // Clear error
      // at last one application must be init
      if (  pPutMsgCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLPUTMSG: BOARD NOT INITIALIZED\n");
	  pPutMsgCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pPutMsgCmd->usBoard)) == NULL) {
	  DBG_PRN("CIF_IOCTLPUTMSG: BOARD NOT INITIALIZED\n");
	  pPutMsgCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0) {
	  DBG_PRN("CIF_IOCTLPUTMSG: BOARD NOT INITIALIZED\n");
	  pPutMsgCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        // set driver info data
        dev->tStateInfo.WriteCnt++;
        dev->tStateInfo.LastFunction = FKT_WRITE;
        dev->tStateInfo.WriteState = STATE_IN;
        // all things well lets work
        // check DEV RDY flag
        if (cif_test_ready(dev) == FALSE) {           // this reflects the DEV RDY flag
          // DEV RDY failed
	  DBG_PRN("IOCTLPUTMSG: board READY failed\n");
          pPutMsgCmd->sError = DRV_DEV_NOT_READY;
        } else if (dev->bWriteState != FALSE) {      // check if write service is active
          // service is active
	  DBG_PRN("IOCTLPUTMSG: CMD ACTIVE\n");
          pPutMsgCmd->sError = DRV_CMD_ACTIVE;
        } else {
          // disable interrupt until we are ready
          spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex_ioctl);
          if ( cif_get_devMBXinfo(dev) == DEVICE_MBX_EMPTY) { // check if mailbox is free
            // no activ service and DEV mailbox is free, activate service, do not wait
            // copy data into DEV DPM
            memcpy( (unsigned char *)&(dev->ptDpmAddress->tDevMbx),
		    (unsigned char *)&(pPutMsgCmd->tMsg),
		    (unsigned short)(pPutMsgCmd->tMsg.ln + 8));
            // send message
            cif_send_msg(dev);
            // enable interrupt we are ready now
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);
          } else if ( pPutMsgCmd->ulTimeout == 0L ) {
            // enable interrupt we are ready now
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);
            // we have not to wait, return with error MBX full
            pPutMsgCmd->sError = DRV_DEV_MAILBOX_FULL;
	    DBG_PRN("IOCTLPUTMSG: MBX IS FULL\n");
          } else { // we have to wait for MBX empty
            dev->tStateInfo.WriteState = STATE_WAIT;
            // activate command
            dev->bWriteState = TRUE;
            // enable interrupt we are ready now
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);
            // wait for DEV message transfer, interrupt signals transfer
	    if ( cif_wait_put_msg(dev, pPutMsgCmd->ulTimeout) == FALSE) { // timeout in Ticks, 1 Tick = 10 msec
              //  Timeout occured
              pPutMsgCmd->sError = DRV_DEV_PUT_TIMEOUT;
	      DBG_PRN("IOCTLPUTMSG: PutMessage TIMEOUT\n");
            } else {
              // MBX must be free (see interupt service), message can be put into the MBX
              // copy data into DEV DPM
              memcpy( (unsigned char *)&(dev->ptDpmAddress->tDevMbx),
		      (unsigned char *)&(pPutMsgCmd->tMsg),
		      (unsigned short)(pPutMsgCmd->tMsg.ln + 8));
              // disable interrupt
              spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex_ioctl);
              // send message
              cif_send_msg(dev);
              // enable interrupt we are ready now
              spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex_ioctl);
	      DBG_PRN("IOCTLPUTMSG: PutMessage OK\n");
            }
            dev->bWriteState = FALSE;   // delete pending action
          }
        }
	dev->tStateInfo.WriteState = STATE_OUT;
      }
      // dev->tStateInfo.WriteState = STATE_OUT;
      // return length of output data
      lRet = sizeof(DEVIO_PUTMESSAGECMD);
      copy_to_user  ( (unsigned char *)arg,
		      (unsigned char *)pPutMsgCmd,
		      sizeof( DEVIO_PUTMESSAGECMD) );
      kfree( pPutMsgCmd);
      break;

    //****************************************************************************************
    //  GETMESSAGE
    //****************************************************************************************

    case CIF_IOCTLGETMSG:   // receive message
      //----------------------------------
      // load pointers to needed Data
      //----------------------------------
      pGetMsgCmd = (DEVIO_GETMESSAGECMD *)kmalloc(sizeof(DEVIO_GETMESSAGECMD), GFP_KERNEL);
      copy_from_user  ( (unsigned char *)pGetMsgCmd,
			(unsigned char *)arg,
			sizeof( DEVIO_GETMESSAGECMD) );
        
      // load DEV instance to real structure
      DBG_PRN("CIF_IOCTLGETMSG:  usBoard (%d), timeout 0x%lX\n",pGetMsgCmd->usBoard, pGetMsgCmd->ulTimeout);
      pGetMsgCmd->sError = 0; // Clear error
      // at last one application must be init
      if (  pGetMsgCmd->usBoard >= cif_nr_devs ) {
	  DBG_PRN("CIF_IOCTLGETMSG: BOARD NOT INITIALIZED\n");
	  pGetMsgCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( (dev = (DEV_INSTANCE *)cif_dev_get_at( pGetMsgCmd->usBoard)) == NULL) {
	  DBG_PRN("CIF_IOCTLGETMSG: BOARD NOT INITIALIZED\n");
	  pGetMsgCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else if( dev->usOpenCounter == 0) {
	  DBG_PRN("CIF_IOCTLGETMSG: BOARD NOT INITIALIZED\n");
	  pGetMsgCmd->sError = DRV_BOARD_NOT_INITIALIZED;
      } else {
        //----------------------------------
        // all things well lets work
        //----------------------------------
        // set driver info data
        dev->tStateInfo.ReadCnt++;
        dev->tStateInfo.LastFunction = FKT_READ;
        dev->tStateInfo.ReadState = FKT_READ;
        // check DEV RDY flag
        if (cif_test_ready(dev) == FALSE) {       // this reflects the DEV RDY flag
          // DEV RDY failed
	  DBG_PRN("IOCTLGETMSG: GetMessage DEV-READY failed\n");
          pGetMsgCmd->sError = DRV_DEV_NOT_READY;
        } else if (dev->bReadState != FALSE) {   // check if read service is active
          // service is active
	  DBG_PRN("IOCTLGETMSG: GetMessage CMD ACTIVE\n");
          pGetMsgCmd->sError = DRV_CMD_ACTIVE;
        } else {
          // disable interrupt until we are ready
          spin_lock_irqsave ( &dev->mutex, flags);
          // no activ service, if a message available, read it
          if ( cif_get_hostMBXinfo(dev) == HOST_MBX_FULL) {// no activ service, if a message available, read it
	    DBG_PRN("IOCTLGETMSG: GetMessage, HOST_MBX_FULL, Message is available\n");
            // message is available
            // get message len
            usMsgLen = (unsigned short)(dev->ptDpmAddress->tPcMbx.ln + 8);
            // read data from DEV-DPM
            memcpy ( (unsigned char *)&(pGetMsgCmd->tMsg),
		     (unsigned char *)&(dev->ptDpmAddress->tPcMbx),
		     (unsigned short)usMsgLen);
            // quit message read to DEV
            cif_quit_msg(dev);
            // enable interrupt we are ready now
            spin_unlock_irqrestore ( &dev->mutex, flags);
          } else if ( pGetMsgCmd->ulTimeout == 0L ) {
            // enable interrupt we are ready now
            spin_unlock_irqrestore ( &dev->mutex, flags);
            // NO Message available, no timeout
            pGetMsgCmd->sError = DRV_DEV_GET_NO_MESSAGE;
	    DBG_PRN("IOCTLGETMSG: GetMessage, No Message\n");
          } else {
            // No Message available, wait for Message
            // no activ service, activate service
            dev->bReadState = TRUE;
            dev->tStateInfo.ReadState = STATE_WAIT;
            // enable interrupt we are ready now
            spin_unlock_irqrestore ( &dev->mutex, flags);
            // wait timeout
	    if ( cif_wait_get_msg(dev, pGetMsgCmd->ulTimeout) == FALSE) {// Timeout in Ticks, 1 Tick = 10 msec
              //  Timeout
              pGetMsgCmd->sError = DRV_DEV_GET_TIMEOUT;
	      DBG_PRN("IOCTLGETMSG: GetMessage TIMEOUT\n");
            } else {
              // message available, read ist
              // get message len
              usMsgLen = (unsigned short)(dev->ptDpmAddress->tPcMbx.ln + 8);
              // read data from DEV-DPM
              memcpy ( (unsigned char *)&(pGetMsgCmd->tMsg),
		       (unsigned char *)&(dev->ptDpmAddress->tPcMbx),
		       (unsigned short)usMsgLen);
              // quit message read to DEV
              spin_lock_irqsave ( &dev->mutex, flags);
              cif_quit_msg(dev);
              spin_unlock_irqrestore ( &dev->mutex, flags);
              DBG_PRN("IOCTLGETMSG: GetMessage OK\n");
            }
            dev->bReadState = FALSE;  // delete pending action
          }
        }
	dev->tStateInfo.ReadState = STATE_OUT;
      }
      //dev->tStateInfo.ReadState = STATE_OUT;
      // return length of output data
      copy_to_user  ( (unsigned char *)arg,
		      (unsigned char *)pGetMsgCmd,
		      sizeof( DEVIO_GETMESSAGECMD) );
      lRet = sizeof(DEVIO_GETMESSAGECMD);
      kfree( pGetMsgCmd);
      break;
      //****************************************************************************************
      //  default
      //****************************************************************************************

    default:
      lRet = 0;
      DBG_PRN("IO CONTROL FUNCTION UNKNOWN (%d)\n", cmd);
      break;
  } // end switch
  return lRet;
}
