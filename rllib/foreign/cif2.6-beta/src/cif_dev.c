/* <St> *******************************************************************

  cif_dev.c

  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver functions
  =========================================================================

  DISCRIPTION

    CIF-Functions and Tools

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

/* ---------------------------------------------------------------------- */
/*  Includes                                                              */
/* ---------------------------------------------------------------------- */

#include <linux/sched.h>
#include <linux/kernel.h>  /* printk() */
#include <linux/slab.h>    /* kmalloc() */
#include <linux/fs.h>      /* everything... */
#include <linux/errno.h>   /* error codes */
#include <linux/time.h>    /* timeval */
#include <linux/delay.h>   /* size_t  */

#include "../inc/cif_types.h"
#include "../inc/cif_dev_i.h"
#include "../inc/cif_dev.h"

/* ---------------------------------------------------------------------- */
/* Prototypes                                                             */
/* ---------------------------------------------------------------------- */

/* DEV handling functions */
void            cif_cold_start     ( DEV_INSTANCE *);
void            cif_warm_start     ( DEV_INSTANCE *);
void            cif_send_msg       ( DEV_INSTANCE *);

void            cif_quit_error     ( DEV_INSTANCE *);
void            cif_quit_msg       ( DEV_INSTANCE *);

unsigned short  cif_test_dpm_access( DEV_INSTANCE *);
unsigned short  cif_test_rdy_flag  ( DEV_INSTANCE *, unsigned short *);
unsigned short  cif_test_dpm_size  ( DEV_INSTANCE *);

/* ---------------------------------------------------------------------- */
/*  Locals                                                                */
/* ---------------------------------------------------------------------- */

#define DEV_IDENTIFIER_ANZ  4
#define DEV_IDENTIFIER_LEN  3
unsigned char abDevIdentifier[DEV_IDENTIFIER_ANZ][DEV_IDENTIFIER_LEN] = { {'C','I','F'},
									  {'C','O','M'},
									  {'C','O','P'},
									  {'D','E','V'} }; 
unsigned char abTestDPM [DEV_IDENTIFIER_LEN] = { 0x55, 0xAA, 0x00 };

long ms_to_jiffies( unsigned long ulTimeout)
{
  return ( (long) ((ulTimeout * HZ) / 1000) );
}

/* <ST> ====================================================================

  Function: cif_test_hardware
            test that the DEV hardware is present and all things well
            - DEV DPM is accessable and DEV board is really at this address
            - RDY-Flags of the DEV present


  --------------------------------------------------------------------------

  Input   : ptGlobalDrv   - pointer to global DRV structure

  Output  : -

  Return  : TRUE   - Hardware check OK
            FALSE  - Hardware check failed

  ========================================================================= <En> */

void cif_test_hardware( DEV_INSTANCE * ptDevInst)
{
  // board is successfully initialized
  // Test if DEV is present at defined location, begin with board 0
  ptDevInst->sDrvInitError = 0;    // clear driver error

  if ( cif_test_dpm_access( ptDevInst) == FALSE) {        // Test dpm access
    // DPM access is NOT AVAILABLE
    ptDevInst->sDrvInitError = DRV_DEV_DPM_ACCESS_ERROR;
    DBG_PRN("DPM ACCESS, ERROR = (%d)\n", ptDevInst->sDrvInitError);
  } else if ( cif_test_rdy_flag(ptDevInst, &(ptDevInst->usRcsError)) == FALSE) {     // Test DEV RDY-flag
    // DPM access is available
    ptDevInst->sDrvInitError = DRV_DEV_NOT_READY;
    DBG_PRN("NO RDY-FLAG, ERROR = (%d)\n", ptDevInst->sDrvInitError);
  } else if ( cif_test_dpm_size( ptDevInst) == FALSE) {
    // Compare the DPM size of the board with the configuration
    ptDevInst->sDrvInitError = DRV_DEV_DPMSIZE_MISMATCH;
    DBG_PRN("DPM SIZE MISMATCH (%d)\n", ptDevInst->sDrvInitError);
    }
  DBG_PRN("done\n");
}


/* <ST> =================================================================================

  Function: cif_test_dpm_access
            read and write DEV identifier in the DEV DPM
  ---------------------------------------------------------------------------------------
  Input   : ptGlobalDrv   - pointer to global DRV structure
  Output  : -
  Return  : TRUE       - Hardware check passed
            FALSE      - Hardware check failed
  ================================================================================= <En> */
unsigned short cif_test_dpm_access( DEV_INSTANCE * ptDevInst)
{
  unsigned char  abOrgIdentifier[DEVNAMELEN];
  unsigned char  abTestString   [DEVNAMELEN];
  unsigned short usIdx = 0, usIdFound = 0, usRet = 0;

  DBG_PRN("ptDpmAddress: %lX\n", (unsigned long)ptDevInst->ptDpmAddress);
  memcpy ( (unsigned char *)&abOrgIdentifier[0],
           (unsigned char *)&(ptDevInst->ptDpmAddress->tDevInfo.DevIdentifier[0]),
           DEVNAMELEN);

  DBG_PRN("Dev. Identifier: %.3s", ptDevInst->ptDpmAddress->tDevInfo.DevIdentifier);

  for ( usIdx = 0, usIdFound = FALSE; (usIdx < DEV_IDENTIFIER_ANZ) && (usIdFound == FALSE); usIdx++) {
    if ( (abOrgIdentifier[0] == abDevIdentifier[usIdx][0]) &&
         (abOrgIdentifier[1] == abDevIdentifier[usIdx][1]) &&
         (abOrgIdentifier[2] == abDevIdentifier[usIdx][2])  ) {
      usIdFound = TRUE;
    }
  } // end for
  if ( usIdFound == FALSE) { // No Identifier found, return fault
    DBG_PRN("NO IDENTIFIER FOUND\n");
    usRet = FALSE;
  } else { // DEV identifiere is OK, write test identifier
    memcpy ( (unsigned char *)&(ptDevInst->ptDpmAddress->tDevInfo.DevIdentifier[0]),
             (unsigned char *)&(abTestDPM[0]),
             DEV_IDENTIFIER_LEN);
    // read test identifier
    memcpy ( (unsigned char *)&abTestString[0],
             (unsigned char *)&(ptDevInst->ptDpmAddress->tDevInfo.DevIdentifier[0]),
             DEVNAMELEN);
    if ( (abTestString[0] != abTestDPM[0]) ||
         (abTestString[1] != abTestDPM[1]) ||
         (abTestString[2] != abTestDPM[2])  ) {
      // Can't write test identifier
      DBG_PRN("READ TEST IDENTIFIER FAILED\n");
      usRet = FALSE;
    } else { // test identifiere is OK, write orignal identifier to dev DPM
      memcpy ( (unsigned char *)&(ptDevInst->ptDpmAddress->tDevInfo.DevIdentifier[0]),
               (unsigned char *)&(abOrgIdentifier[0]),
               DEV_IDENTIFIER_LEN);
      // read original identifier
      memcpy ( (unsigned char *)&abTestString[0],
	       (unsigned char *)&(ptDevInst->ptDpmAddress->tDevInfo.DevIdentifier[0]),
	       DEVNAMELEN);
      if ( (abTestString[0] != abOrgIdentifier[0]) ||
           (abTestString[1] != abOrgIdentifier[1]) ||
           (abTestString[2] != abOrgIdentifier[2])  ) {
        // Can't write original identifier
        DBG_PRN("WRITE ORGINAL IDENTIFIER FAILED\n");
        usRet = FALSE;
      } else { // write original identifier is OK, DPM is accessable
	DBG_PRN("Done\n");
        usRet = TRUE;
      }
    }
  }

  return usRet;
}

/* <ST> =================================================================================

  Function: cif_test_rdy_flag
            read DEV 'RDY' flag status
  ---------------------------------------------------------------------------------------

  Input   : ptGlobalDrv   - pointer to global DRV structure
  Output  : -
  Return  : TRUE       - Hardware check passed
            FALSE      - Hardware check failed
  ================================================================================= <En> */

unsigned short cif_test_rdy_flag( DEV_INSTANCE * ptDevInst, unsigned short * usRcsError)
{
  unsigned short usRet = FALSE;

  *usRcsError = 0;            // clear RCS error

  // test RDY flag in DEV HostFlags
  if ( (*(ptDevInst->pbHostFlags) & INI_MSG_RDY) == 0) { // RDY flag not set, read RCS error byte
    if ( (*usRcsError = (unsigned short)ptDevInst->ptDpmAddress->tRcsInfo.RcsError) >= 250) {
      *usRcsError = 0;
      ptDevInst->usSpecialRcsError = TRUE;
      usRet = TRUE;
    } else {
      ptDevInst->usSpecialRcsError = FALSE;
      *usRcsError = (unsigned short)ptDevInst->ptDpmAddress->tRcsInfo.RcsError;
      *usRcsError += DRV_RCS_ERROR_OFFSET;
    }
  } else { // RDY flag is set
    usRet = TRUE;
    DBG_PRN("RDY flag is set\n");
  }

  DBG_PRN("Done\n");

  return usRet;
}


/* <ST> =================================================================================

  Function: cif_test_dpm_size
            compare the DPM size with the configuration

  ---------------------------------------------------------------------------------------

  Input   : ptDevInst     - pointer to DEV instance

  Output  : -

  Return  : TRUE       - Watchdog check passed
            FALSE      - Watchdog check failed

  ================================================================================= <En> */

unsigned short cif_test_dpm_size( DEV_INSTANCE * ptDevInst)
{
  unsigned short usCifEntry;
  unsigned short usRet = FALSE;

  // Compare DPM size from the board with the configuration
  // Allow 1 KByte entry in CIF board
  // If BOOTSTRAPLOADER is running, this will show always DPMSize = 1,
  // so use the size which is konfigured !

  // Check if BOOTSTRAPLOADER is running
  if ( (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[0] == 'B') &&
       (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[1] == 'o') &&
       (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[2] == 'o') &&
       (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[3] == 't') && 
       (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[4] == 's') && 
       (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[5] == 't') && 
       (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[6] == 'r') &&
       (ptDevInst->ptDpmAddress->tFiwInfo.FirmwareName[7] == 'a')  ) {
    // This is the BOOTSTRAPLOADER, use DPMSize like configured
    usRet = TRUE;
  } else if ( ( usCifEntry = (unsigned short)(ptDevInst->ptDpmAddress->tDevInfo.DpmSize)) == 1) {
    // Allow a DPMSize configuration of 2 KByte
    if ( ptDevInst->ulDPMSize == 2) usRet = TRUE;
  } else if ( ptDevInst->ulDPMSize == (unsigned long)(ptDevInst->ptDpmAddress->tDevInfo.DpmSize) ) {
    // DPM size is equal to the configuration
    usRet = TRUE;
  }

  DBG_PRN("Done\n");

  return usRet;
}


/* <ST> =================================================================================

  Function: cif_test_ext_version
            test the DEV's version entries for customers

  ---------------------------------------------------------------------------------------

  Input   : ptDevInst     - pointer to DEV instance
            pabOsVersion  - pointer to driver version entry

  Output  : -

  Return  : TRUE       - OS version is allowed
            FALSE      - OS version is not allowed

  ================================================================================= <En> */

short cif_test_ext_version( DEV_INSTANCE * ptDevInst, unsigned short usMode, unsigned char * pabExtVersion)
{
  short sRet = DRV_DEV_OS_VERSION_ERROR;

  //---------------------------------------------------------------
  // Check if board is OK
  //---------------------------------------------------------------
  if (  (ptDevInst->sDrvInitError != DRV_NO_ERROR)               &&
       ((ptDevInst->sDrvInitError != DRV_DEV_OS_VERSION_ERROR)   ||
        (ptDevInst->usRcsError    != DRV_NO_ERROR)               )) {

    // Return original error
    if ( ptDevInst->sDrvInitError != DRV_NO_ERROR) sRet = ptDevInst->sDrvInitError;
    else sRet = ptDevInst->usRcsError;

  } else {

    //---------------------------------------------------------------
    // Check user entries
    // Empty characters not allowed !!!!!!!
    //---------------------------------------------------------------
    if ( (pabExtVersion[0] == 0) &&
         (pabExtVersion[1] == 0) &&
         (pabExtVersion[2] == 0) &&
         (pabExtVersion[3] == 0)  ) {

      // Clear orignal entry from driver startup
      ptDevInst->sDrvInitError = DRV_DEV_OS_VERSION_ERROR;

    } else {

      switch (usMode) {
        //------------------------------------------------------------------------------
        // Check for customer entry
        //------------------------------------------------------------------------------
        case 1:

          // Clear orignal entry from driver startup
          ptDevInst->sDrvInitError = DRV_DEV_OS_VERSION_ERROR;

          if ( (pabExtVersion[0] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName0[0]) &&
               (pabExtVersion[1] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName0[1]) &&
               (pabExtVersion[2] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName0[2]) &&
               (pabExtVersion[3] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName0[3])  ) {
            // OS version is allowed
            ptDevInst->sDrvInitError = DRV_NO_ERROR;
          } else if ( (pabExtVersion[0] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName1[0]) &&
                      (pabExtVersion[1] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName1[1]) &&
                      (pabExtVersion[2] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName1[2]) &&
                      (pabExtVersion[3] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName1[3])  ) {
            // OS version is allowed
            ptDevInst->sDrvInitError = DRV_NO_ERROR;
          } else if ( (pabExtVersion[0] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName2[0]) &&
                      (pabExtVersion[1] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName2[1]) &&
                      (pabExtVersion[2] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName2[2]) &&
                      (pabExtVersion[3] == ptDevInst->ptDpmAddress->tDevVersion.PcOsName2[3])  ) {
            // OS version is allowed
            ptDevInst->sDrvInitError = DRV_NO_ERROR;
          }
          // return actual error state
          sRet = ptDevInst->sDrvInitError;
        break;

        //------------------------------------------------------------------------------
        // Check for OPC Server entry
        //------------------------------------------------------------------------------
        case 98: // Check for OPC Server entry
          ptDevInst->sDrvInitError = DRV_NO_ERROR;
          sRet = ptDevInst->sDrvInitError;
        break;

        //------------------------------------------------------------------------------
        // Check for HILSCHER entry
        //------------------------------------------------------------------------------
        case 99:
          // Return always NO_ERROR
          ptDevInst->sDrvInitError = DRV_NO_ERROR;
          sRet = ptDevInst->sDrvInitError;
        break;

        //------------------------------------------------------------------------------
        // UNKNOWN mode, clear board access
        //------------------------------------------------------------------------------
        default: // Set error
          // Clear orignal entry from driver startup
          ptDevInst->sDrvInitError = DRV_DEV_OS_VERSION_ERROR;
          sRet = DRV_DEV_OS_VERSION_ERROR;
        break;
      }
    }    
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: cif_check_hardware
            test that the DEV hardware entries during the test at driver startup

  ---------------------------------------------------------------------------------------

  Input   : ptGlobalDrv   - pointer to global DRV structure

  Output  : -

  Return  : TRUE   - Hardware check OK
            FALSE  - Hardware check failed

  ================================================================================= <En> */

unsigned short cif_check_hardware( DEV_INSTANCE * ptDevInst)
{
  unsigned short usRet = FALSE;

  // board is successfully initialized
  if ( ptDevInst->bActive != TRUE) {
    ptDevInst->sDrvInitError = DRV_BOARD_NOT_INITIALIZED;
    usRet = FALSE;
  // test on errors
  } else if ( ptDevInst->sDrvInitError != DRV_NO_ERROR) {
    usRet = FALSE;
  } else if ( ptDevInst->usRcsError != DRV_NO_ERROR) {
    usRet = FALSE;
  } else {
    usRet = TRUE;
  }

  return usRet;
}

/* <ST> =================================================================================

  Function: cif_boot_start
            Device boot start
  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_boot_start( DEV_INSTANCE * dev)
{
  /* write WStart-Bit in DevFlags */
  dev->bMyDevFlags |= (RESET_FLAG | INIT_FLAG);
  *(dev->pbDevFlags) = dev->bMyDevFlags;

  DBG_PRN("BStart: DevFlags   =(%x)\n", *(dev->pbDevFlags));
  DBG_PRN("        bMyDevFlags=(%x)\n", dev->bMyDevFlags);
}

/* <ST> =================================================================================

  Function: cif_cold_start
            Device cold start
  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_cold_start( DEV_INSTANCE * dev)
{
  /* write cold start Bit in DevFlags */
  dev->bMyDevFlags |= RESET_FLAG;
  *(dev->pbDevFlags) = dev->bMyDevFlags;

  DBG_PRN("DevFlags   =(%x)\n", *(dev->pbDevFlags));
  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);
}


/* <ST> =================================================================================

  Function: cif_warm_start
            Device warm start
  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_warm_start( DEV_INSTANCE * dev)
{
  /* write warm startm bit in DevFlags */
  dev->bMyDevFlags |= INIT_FLAG;
  *(dev->pbDevFlags) = dev->bMyDevFlags;

  DBG_PRN("DevFlags   =(%x)\n", *(dev->pbDevFlags));
  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);
}

/* <ST> =================================================================================

  Function: cif_set_host_state

  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_set_host_state( DEV_INSTANCE * dev)
{
  if ( dev->usBoardIrq != 0) {
    dev->bMyDevFlags  |=  NOTREADY_FLAG;
  } else {
    dev->bMyDevFlags = (unsigned char)(*(dev->pbDevFlags) | NOTREADY_FLAG);
  }

  DBG_PRN("DevFlags   =(%x)\n", *(dev->pbDevFlags));
  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);

  *(dev->pbDevFlags)  = dev->bMyDevFlags;
}

/* <ST> =================================================================================

  Function: cif_clear_host_state

  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_clear_host_state( DEV_INSTANCE * dev)
{
  if ( dev->usBoardIrq != 0) {
    dev->bMyDevFlags  &= (~NOTREADY_FLAG);
  } else {
    dev->bMyDevFlags = (unsigned char)(*(dev->pbDevFlags) & (~NOTREADY_FLAG));
  }

  DBG_PRN("DevFlags   =(%x)\n", *(dev->pbDevFlags));
  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);

  *(dev->pbDevFlags)  = dev->bMyDevFlags;
}

/* <ST> =================================================================================

  Function: cif_send_msg
            Signal new message in DEV mailbox is available
  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_send_msg( DEV_INSTANCE * dev)
{
  dev->bWriteMsgFlag = DEVICE_MBX_FULL;
  if ( dev->usBoardIrq != 0) {
    dev->bMyDevFlags  ^= DEVCOM_FLAG;
  } else {
    dev->bMyDevFlags = (unsigned char)(*(dev->pbDevFlags) ^ DEVCOM_FLAG);
  }
  *(dev->pbDevFlags)  = dev->bMyDevFlags;

  DBG_PRN("DevFlags   =(%x)\n", *(dev->pbDevFlags));
  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);
}

/* <ST> =================================================================================

  Function: cif_quit_msg
            Signal message is read from the PC
  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_quit_msg(  DEV_INSTANCE * dev)
{
  dev->bReadMsgFlag = HOST_MBX_EMPTY;
  if ( dev->usBoardIrq != 0) { // interrupt mode
    dev->bMyDevFlags ^= HOSTACK_FLAG;
  } else { // polling mode
    dev->bMyDevFlags    = (unsigned char)(*(dev->pbDevFlags) ^ HOSTACK_FLAG);
  }
  *(dev->pbDevFlags)  = dev->bMyDevFlags;
  DBG_PRN("DevFlags   =(%x)\n", *(dev->pbDevFlags));
  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);
}

/* <ST> =================================================================================

  Function: cif_quit_error
            Set tDevInst.ReadMsgFlag to message non
  ---------------------------------------------------------------------------------------

  Input   : -

  Output  : -

  Return  : -

  ================================================================================= <En> */

void  cif_quit_error( DEV_INSTANCE * dev)
{
  dev->bReadMsgFlag = HOST_MBX_EMPTY;
}


/* <ST> =================================================================================

  Function: cif_get_devMBXinfo
            returns the state of the send mailbox
  ---------------------------------------------------------------------------------------

  Input   : dev  - pointer to device instance

  Output  : -

  Return  : 0 = DEVICE_MBX_EMPTY
            1 = DEVICE_MBX_FULL

  ================================================================================= <En> */

unsigned short cif_get_devMBXinfo( DEV_INSTANCE * dev)
{

  if ( dev->usBoardIrq == 0) { // polling mode
    if ( ((*(dev->pbHostFlags) ^ *(dev->pbDevFlags)) & DEVCOM_FLAG) == 0x00) dev->bWriteMsgFlag = DEVICE_MBX_EMPTY;
    else dev->bWriteMsgFlag = DEVICE_MBX_FULL;
  }
  // return actual state, interrupt will set this flag also !
  return ((unsigned short)dev->bWriteMsgFlag);
}

/* <ST> =================================================================================

  Function: cif_get_hostMBXinfo
            returns the state of the send mailbox
  ---------------------------------------------------------------------------------------

  Input   : dev  - pointer to device instance

  Output  : -

  Return  : 0 = HOST_MBX_EMPTY
            1 = HOST_MBX_FULL

  ================================================================================= <En> */

unsigned short  cif_get_hostMBXinfo( DEV_INSTANCE * dev)
{

  if (dev->usBoardIrq == 0) { // polling mode
    if ( ((*(dev->pbHostFlags) ^ *(dev->pbDevFlags)) & HOSTACK_FLAG) == 0x00) dev->bReadMsgFlag = HOST_MBX_EMPTY;
    else dev->bReadMsgFlag = HOST_MBX_FULL;
  }
  // return actual state, interrupt will set this flag also !
  return ((unsigned short)dev->bReadMsgFlag);
}

/* <ST> =================================================================================

  Function: cif_test_ready
            test if device is ready
  ---------------------------------------------------------------------------------------

  Input   : dev  - pointer to device instance

  Output  : -

  Return  : FALSE  = device not ready
            TRUE   = device ready

  ================================================================================= <En> */

unsigned short  cif_test_ready( DEV_INSTANCE * dev)
{
  if ( dev->usBoardIrq != 0) { // interrupt is running, do not read the flags from the DPM
    if ( (dev->usSpecialRcsError == TRUE) &&
         ((dev->bInitMsgFlag & INI_MSG_RDY) == 0) )
      return (TRUE);
    if ( dev->bInitMsgFlag & INI_MSG_RDY)  return (TRUE);
    else  return (FALSE);
  } else {
    if ( (dev->usSpecialRcsError == TRUE) &&
         ((*(dev->pbHostFlags) & INI_MSG_RDY) == 0) ) {
      return (TRUE);
    }
    if ( (*(dev->pbHostFlags) & INI_MSG_RDY))  return (TRUE);
    else  return (FALSE);
  }
}

/* <ST> =================================================================================

  Function: cif_wait_put_msg
            wait function for PutMessage
  ---------------------------------------------------------------------------------------

  Input   : dev  - pointer to device instance
            ulTimeout - Timeout

  Output  : -

  Return  : FALSE = timeout
            TRUE  = no timeout

  ================================================================================= <En> */

unsigned short  cif_wait_put_msg( DEV_INSTANCE * dev, unsigned long ulTimeout)
{
  long  timeout = ms_to_jiffies( ulTimeout);
  u64  expire_64  = 0;
  unsigned short usRet   = FALSE;

  if (dev->usBoardIrq != 0) { //interrupt is activ
    DBG_PRN("(INT)PutMessage WAIT\n");
    // wait timeout, FALSE == timeout
    interruptible_sleep_on_timeout( &(dev->pWriteSemaphore), timeout);
    if( dev->bWriteMsgFlag == DEVICE_MBX_EMPTY) usRet = TRUE;
  } else { // poll function
    DBG_PRN("(POLL)PutMessage WAIT\n");
    // wait for mailbox is empty
    expire_64 =  (u64)timeout + get_jiffies_64();
    while ( ( ((*(dev->pbHostFlags)^ *(dev->pbDevFlags)) & DEVACK_FLAG) != 0x00 ) &&
	    ( get_jiffies_64() <= expire_64 ) ) 
      { 
	schedule();
      }
    if ( get_jiffies_64() > expire_64)  usRet = FALSE; // timeout
    else                    usRet = TRUE;  // no timeout
  }
  return usRet;
}

/* <ST> =================================================================================

  Function: cif_wait_get_msg
            wait function for GetMessage
  ---------------------------------------------------------------------------------------
  Input   : dev  - pointer to device instance
            ulTimeout - Timeout
  Output  : -
  Return  : FALSE = Timeout
            TRUE  = No Timeout
  ================================================================================= <En> */

unsigned short  cif_wait_get_msg( DEV_INSTANCE * dev, unsigned long ulTimeout)
{
           long   timeout = ms_to_jiffies( ulTimeout);
  u64  expire_64  = 0;
  unsigned short  usRet   = FALSE;

  if (dev->usBoardIrq != 0) { // interrupt is activ
    // No Message available, wait for Message
    // wait timeout, FALSE == timeout
    interruptible_sleep_on_timeout( &(dev->pReadSemaphore), timeout);
    if( dev->bReadMsgFlag == HOST_MBX_FULL) usRet = TRUE;
  } else { // poll function
    DBG_PRN("(POLL)GetMessage WAIT\n");
    // wait for message is available
    expire_64 = (u64)timeout + get_jiffies_64();
    while ( ( ( (*(dev->pbHostFlags)^ *(dev->pbDevFlags)) & HOSTCOM_FLAG) == 0x00 )  &&
	    ( get_jiffies_64() <= expire_64 ) ) 
      { 
	schedule();
      }
    if ( get_jiffies_64() > expire_64)  usRet = FALSE; // timeout
    else                    usRet = TRUE;  // no timeout
  }
  return usRet;
}

/* <ST> =================================================================================

  Function: cif_wait_reset
            returns the state of the send mailbox
  ---------------------------------------------------------------------------------------
  Input   : dev  - pointer to device instance
  Output  : -
  Return  : FALSE = Timeout
            TRUE  = No Timeout
  ================================================================================= <En> */

unsigned short  cif_wait_reset( DEV_INSTANCE * dev, unsigned long ulTimeout)
{
           long  timeout  = ms_to_jiffies( ulTimeout);
           long  rtimeout = 0;
  u64  expire_64 = 0;
  unsigned short usRet    = TRUE;

  expire_64 = (u64)timeout + get_jiffies_64();
  if (dev->usBoardIrq !=0) { // interrupt function

    // wait for reset is active, DevFlags == 0
    while ( ( (*(dev->pbDevFlags) & (INIT_FLAG | RESET_FLAG)) != 0x00 )  &&
	    ( get_jiffies_64() <= expire_64 ) )  
    {
      schedule();
    }
    // Check timeout
    if ( get_jiffies_64() > expire_64) usRet = FALSE;    // timeout
    else {
      rtimeout = interruptible_sleep_on_timeout( &(dev->pInitSemaphore), timeout);
      if ( !rtimeout) usRet = FALSE;
      else            usRet = TRUE;
    }
  } else {  // poll function
    DBG_PRN("(POLL)WaitReset WAIT\n");
    // wait for reset is active, DevFlags == 0
    while ( ( (*(dev->pbDevFlags) & (INIT_FLAG | RESET_FLAG)) != 0x00 )  &&
	    ( get_jiffies_64() <= expire_64 ) )  
      {
	schedule();
      }
    if ( get_jiffies_64() > expire_64)  usRet = FALSE;
    else { // wait for RDY is set
      expire_64 = get_jiffies_64() + (u64)timeout;
      while ( ( (*(dev->pbHostFlags) & INI_MSG_RDY) == 0x00 ) &&
	      ( get_jiffies_64() <= expire_64 ) ) 
	{ 
	  schedule();
	}
      if ( get_jiffies_64() > expire_64) usRet = FALSE;    // timeout
    }

    // update bInitMsgFlag from DPM, like in interrupt
    dev->bMyDevFlags  = *(dev->pbDevFlags);
    dev->bInitMsgFlag = (unsigned char) (*(dev->pbHostFlags) & INI_MSG_RDYRUN);
  }
  return usRet;
}

/* <ST> =================================================================================

  Function: cif_test_com
            test the device COM flag
  ---------------------------------------------------------------------------------------

  Input   : dev  - pointer to device instance

  Output  : -

  Return  : FALSE  = COM flag not set
            TRUE   = COM flag set

  ================================================================================= <En> */

unsigned short  cif_test_com( DEV_INSTANCE * dev)
{

  if ( dev->usBoardIrq != 0) { // interrupt is running, do not read the flags from the DPM
    if ( dev->bHostFlags & COM_FLAG_RDY) return (TRUE);
    else return (FALSE);
  } else {
    if ( (*(dev->pbHostFlags) & COM_FLAG_RDY)) return (TRUE);
    else return (FALSE);
  }
}


/* <ST> =================================================================================

  Function: cif_wait_com
            waits for the COM bit
  ---------------------------------------------------------------------------------------

  Input   : dev  - pointer to device instance
            ulTimeout - Timeout

  Output  : -

  Return  : FALSE = Timeout
            TRUE  = No Timeout

  ================================================================================= <En> */

unsigned short  cif_wait_com( DEV_INSTANCE * dev, unsigned long ulTimeout)
{
           long  rtimeout = 0; // rest timeout from interruptible_sleep_on_timeout( ... );
           long  timeout  = ms_to_jiffies( ulTimeout);
  unsigned long  expire   = 0;
  unsigned short usRet    = FALSE;

  if (dev->usBoardIrq != 0) { // interrupt is active
    rtimeout = interruptible_sleep_on_timeout( &(dev->pInitSemaphore), timeout);
    if (!rtimeout) usRet = FALSE;
    else           usRet = TRUE;
  } else { // poll function

    expire  = timeout + jiffies;
    if ( dev->bCOMNeqState == TRUE) {// wait for COM bit is set (not equal)
      while ( ( ((*(dev->pbHostFlags)^ *(dev->pbDevFlags)) & COM_FLAG_RDY) == 0x00) &&
	    ( jiffies <= expire ) )
	{
	  schedule();
	}
    } else { // wait for COM bit is clear
      while ( ( ((*(dev->pbHostFlags)^ *(dev->pbDevFlags)) & COM_FLAG_RDY) != 0x00)  &&
	    ( jiffies <= expire ) )
	{
	  schedule();
	}
    }
    if ( jiffies > expire) usRet = FALSE;  // timeout
    else usRet = TRUE;                     // no timeout
  }

  return usRet;
}

/* <ST> =================================================================================

  Function: cif_get_io_state
            returns the actual IO state
  ---------------------------------------------------------------------------------------

  Input   : dev  - pointer to device instance

  Output  : -

  Return  : EXCHGIO_EQUAL
            EXCHGIO_NOT_EQUAL
  ================================================================================= <En> */

unsigned char  cif_get_io_state ( DEV_INSTANCE * dev)
{
  if ( dev->usBoardIrq == 0) { // polling mode
    if ( ( (*(dev->pbHostFlags) ^ *(dev->pbDevFlags)) & PDCOM_FLAG) == 0x00)
      dev->bExIOFlag = EXCHGIO_EQUAL;
    else
      dev->bExIOFlag = EXCHGIO_NOT_EQUAL;
  }
  //  DBG_PRN("HostFlags=(%x), MyDevFlags=(%x)\n", dev->bHostFlags, dev->bMyDevFlags);
  // return actual state, interrupt will set this flag also !
  return (dev->bExIOFlag);
}

/* <ST> =================================================================================

  Function: cif_exchange_io_done
            Signal data exchange is done
  ---------------------------------------------------------------------------------------
  Input   : dev  - device instance
  Output  : -
  Return  : -
  ================================================================================= <En> */

void  cif_exchange_io_done( DEV_INSTANCE * dev)
{
  // set internal state
  if (dev->bExIOFlag == EXCHGIO_EQUAL) dev->bExIOFlag = EXCHGIO_NOT_EQUAL;
  else dev->bExIOFlag = EXCHGIO_EQUAL;

  if ( dev->usBoardIrq != 0) { // interrupt mode
    dev->bMyDevFlags ^= PDCOM_FLAG;
  } else { // poll mode
    dev->bMyDevFlags = (unsigned char)(*(dev->pbDevFlags) ^ PDCOM_FLAG);
  }

  DBG_PRN("pbDevFlags=(%x)\n", *(dev->pbDevFlags));
  *(dev->pbDevFlags) = dev->bMyDevFlags;

  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);
}

/* <ST> =================================================================================

  Function: cif_wait_exchange_io
            wait for exchange IO function
  ---------------------------------------------------------------------------------------
  Input   : dev  - pointer to device instance
            ulTimeout - Timeout
  Output  : -
  Return  : FALSE = timeout
            TRUE  = no timeout
  ================================================================================= <En> */

unsigned short  cif_wait_exchange_io( DEV_INSTANCE * dev, unsigned long ulTimeout)
{
           long  rtimeout = 0; // rest timeout from interruptible_sleep_on_timeout( ... );
	   long  timeout  = ms_to_jiffies( ulTimeout);
  unsigned long  expire   = 0;
  unsigned short usRet    = TRUE;

  if (dev->usBoardIrq != 0) { // interrupt is activ
    DBG_PRN("(INT)ExchangeIO WAIT\n");
    // wait timeout, FALSE == timeout
    rtimeout = interruptible_sleep_on_timeout( &(dev->pExIOSemaphore), timeout);
    if ( !rtimeout)  // timeout
      usRet = FALSE;
    else
      usRet = TRUE;
  } else { // poll function
    DBG_PRN("(POLL)ExchangeIO WAIT\n");
    expire  = timeout + jiffies;
    if ( dev->bExIOEqState == TRUE) { // wait for IO bits EQUAL
      DBG_PRN("(POLL)Wait equal state\n");
      while ( ( ((*(dev->pbHostFlags)^ *(dev->pbDevFlags)) & PDACK_FLAG) != 0x00 ) &&
	      ( jiffies <= expire ) )
	{
	  schedule();
	}
    } else { // wait for IO bits NOT EQUAL
      DBG_PRN("(POLL)Wait not equal state\n");
      while ( ( ((*(dev->pbHostFlags)^ *(dev->pbDevFlags)) & PDACK_FLAG) == 0x00 ) &&
	      ( jiffies <= expire ) )
	{
	  schedule();
	}
    }
    if ( jiffies > expire) usRet = FALSE;  // timeout
    else                   usRet = TRUE;   // no timeout
  }
  return usRet;
}

/* <ST> =================================================================================
  Function: cif_exchangeIO
            Run data exchange
  ---------------------------------------------------------------------------------------
  Input   : dev  - device instance
  Output  : -
  Return  : -
  ================================================================================= <En> */

long cif_exchange_io( DEV_INSTANCE  *dev,
		      DEVIO_EXIOCMD *pExIO,
		      unsigned char  bExchangeMode)
{
           long  lRet;
  unsigned long  flags;
  // length of output data
  lRet = sizeof(pExIO->sError);

  DBG_PRN("dev->usOpenCounter=%d, pExIO->usBoard=%d\n", dev->usOpenCounter, pExIO->usBoard);
  if ( dev->usOpenCounter == 0 ||
       pExIO->usBoard >= MAX_DEV_BOARDS) {
    DBG_PRN("              BOARD NOT INITIALIZED\n");
    pExIO->sError = DRV_BOARD_NOT_INITIALIZED;
  } else {
    //----------------------------------
    // all things well lets work
    //----------------------------------
    // check DEV RDY flag
    if ( (cif_test_ready(dev)) == FALSE) {       // this reflects the DEV RDY flag
      // DEV RDY failed
      DBG_PRN("ExchangeIO DEV-READY failed\n");
      pExIO->sError = DRV_DEV_NOT_READY;
    } else if (dev->bExIOEqState  != FALSE ||
              (dev->bExIONeqState != FALSE)  ){   // check if exchange service is active
      // service is active
      DBG_PRN("ExchangeIO CMD ACTIVE\n");
      pExIO->sError = DRV_CMD_ACTIVE;
    } else {
      dev->tStateInfo.ExIOCnt++;
      // use COM mode of the board
      switch ( bExchangeMode) {      
        case 1: // MODE 1
        case 2: // MODE 2
	  DBG_PRN("ExchangeIO MODE 1/2\n");
          // disable interrupt until we are ready
          spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex);
          // if com-bit not equal, we can write and read our datas
          if ( (cif_get_io_state ( dev)) == EXCHGIO_NOT_EQUAL) {
            // we can work
            // write data
            memcpy ( (unsigned char *)&(dev->pbDpmSendArea[pExIO->usSendOffset]),
		     (unsigned char *)pExIO->pabSendData,
		     pExIO->usSendLen);
            // read data
            memcpy ( (unsigned char *)pExIO->pabReceiveData,
		     (unsigned char *)&(dev->pbDpmReceiveArea[pExIO->usReceiveOffset]),
		     pExIO->usReceiveLen);

            // toggle COM-bit
            cif_exchange_io_done ( dev);
            lRet += pExIO->usReceiveLen;
            // done with interrupt
            spin_unlock_irqrestore ( &dev->mutex, flags); //spin_unlock_irq ( &dev->mutex);
          } else if ( pExIO->ulTimeout == 0L) {
            // done with interrupt
            spin_unlock_irqrestore ( &dev->mutex, flags);//spin_unlock_irq ( &dev->mutex);
            // we have not to wait, return with no data exchange
            pExIO->sError = DRV_DEV_EXCHANGE_FAILED;
	    DBG_PRN("DATA EXCHANGE FAILED\n");
          } else {
            // Wait for data exchange EXCHG_IO_NOT_EQUAL
            // no activ service, activate service
            dev->bExIONeqState = TRUE;
            // done with interrupt
            spin_unlock_irqrestore ( &dev->mutex, flags);//spin_unlock_irq ( &dev->mutex);
            // wait timeout
            if ( cif_wait_exchange_io(dev, pExIO->ulTimeout) == FALSE) {
              //  Timeout
              pExIO->sError = DRV_DEV_EXCHANGE_TIMEOUT;
	      DBG_PRN("TIMEOUT\n");
            } else {
              // state EXIO_NOT_EQAUL available, do data exchange
              // write data
              memcpy ( (unsigned char *)&(dev->pbDpmSendArea[pExIO->usSendOffset]),
		       (unsigned char *)pExIO->pabSendData,
		       pExIO->usSendLen);
              // read data
              memcpy ( (unsigned char *)pExIO->pabReceiveData,
		       (unsigned char *)&(dev->pbDpmReceiveArea[pExIO->usReceiveOffset]),
		       pExIO->usReceiveLen);
              // toggle COM-bit
	      spin_lock_irqsave ( &dev->mutex, flags); //spin_lock_irq ( &dev->mutex);
              cif_exchange_io_done( dev);
              spin_unlock_irqrestore ( &dev->mutex, flags);//spin_unlock_irq ( &dev->mutex);
              lRet += pExIO->usReceiveLen;
            }
            dev->bExIONeqState = FALSE;  // delete pending action
          }
          break;
        case 3: // MODE 3
	  DBG_PRN("ExchangeIO MODE 3\n");
          // we can write and read our datas, don't use the com-bits
          // write data
          memcpy ( (unsigned char *)&(dev->pbDpmSendArea[pExIO->usSendOffset]),
		   (unsigned char *)pExIO->pabSendData,
		   pExIO->usSendLen);
          // read data
          memcpy ( (unsigned char *)pExIO->pabReceiveData,
		   (unsigned char *)&(dev->pbDpmReceiveArea[pExIO->usReceiveOffset]),
		   pExIO->usReceiveLen);
          lRet += pExIO->usReceiveLen;
          break;
        case 4: // MODE 4
        case 5: // MODE 5
	  DBG_PRN("ExchangeIO MODE 4/5\n");
          // disable interrupt until we are ready
          spin_lock_irqsave ( &dev->mutex, flags);//spin_lock_irq ( &dev->mutex);
          // if com-bits equal we can write and read our datas
          if ( (cif_get_io_state ( dev)) == EXCHGIO_EQUAL) {
            // we can work
            // write data
            memcpy ( (unsigned char *)&(dev->pbDpmSendArea[pExIO->usSendOffset]),
		     (unsigned char *)pExIO->pabSendData,
		     pExIO->usSendLen);
            // read data
            memcpy ( (unsigned char *)pExIO->pabReceiveData,
		     (unsigned char *)&(dev->pbDpmReceiveArea[pExIO->usReceiveOffset]),
		     pExIO->usReceiveLen);
            // toggle COM-bit
            cif_exchange_io_done ( dev);
            lRet += pExIO->usReceiveLen;
            // done with interrupt
            spin_unlock_irqrestore ( &dev->mutex, flags);//spin_unlock_irq ( &dev->mutex);
          } else if ( pExIO->ulTimeout == 0L) {
            // done with interrupt
            spin_unlock_irqrestore ( &dev->mutex, flags);//spin_unlock_irq ( &dev->mutex);
            // we have not to wait, return with no data exchange
            pExIO->sError = DRV_DEV_EXCHANGE_FAILED;
	    DBG_PRN("DATA EXCHANGE FAILED\n");
          } else {
            // Wait for data exchange EXCHG_IO_EQUAL
            // no activ service, activate service
            dev->bExIOEqState = TRUE;
            // ready for interrupt
            spin_unlock_irqrestore ( &dev->mutex, flags);//spin_unlock_irq ( &dev->mutex);
            // wait timeout
            if ( cif_wait_exchange_io(dev, pExIO->ulTimeout) == FALSE) {
              //  Timeout
              pExIO->sError = DRV_DEV_EXCHANGE_TIMEOUT;
	      DBG_PRN("TIMEOUT\n");
            } else {
              // state EXIO_EQAUL available, do data exchange
              // write data
              memcpy ( (unsigned char *)&(dev->pbDpmSendArea[pExIO->usSendOffset]),
		       (unsigned char *)pExIO->pabSendData,
		       pExIO->usSendLen);
              // read data
              memcpy ( (unsigned char *)pExIO->pabReceiveData,
		       (unsigned char *)&(dev->pbDpmReceiveArea[pExIO->usReceiveOffset]),
		       pExIO->usReceiveLen);
              // toggle COM-bit
	      spin_lock_irqsave ( &dev->mutex, flags);//spin_lock_irq ( &dev->mutex);
              cif_exchange_io_done( dev);
              spin_unlock_irqrestore ( &dev->mutex, flags);//spin_unlock_irq ( &dev->mutex);
              lRet += pExIO->usReceiveLen;
            }
            dev->bExIOEqState = FALSE;  // delete pending action
          }
          break;
        default: // MODE unknown
	  DBG_PRN("MODE unknown\n");
          pExIO->sError = DRV_DEV_COM_MODE_UNKNOWN;
          break;
      } /* end switch */
      if ( cif_test_com (dev) == FALSE ) { // this reflects the COM-Bit
        // COM failed
	DBG_PRN("ExchangeIO COM flag not set\n");
        pExIO->sError = DRV_DEV_NO_COM_FLAG;
      }
    }
  }
  return lRet;
}

/* <ST> =================================================================================

  Function: cif_get_state
            returns the actual state
  ---------------------------------------------------------------------------------------
  Input   : dev  - pointer to device instance
  Output  : -
  Return  : EXCHGIO_EQUAL
            EXCHGIO_NOT_EQUAL
  ================================================================================= <En> */

unsigned short  cif_get_state ( DEV_INSTANCE * dev)
{
  if ( dev->usBoardIrq == 0) {
    // polling mode
    if ( ( (*(dev->pbHostFlags) ^ *(dev->pbDevFlags)) & STATECOM_FLAG) == 0x00)
      dev->bStateFlag = STATE_ERR_NON;
    else
      dev->bStateFlag = STATE_ERR;
  }
  DBG_PRN("bStateFlag = (%x)\n", dev->bStateFlag);

  // return actual state, interrupt will also set this flag !
  return ((unsigned short)dev->bStateFlag);
}

/* <ST> =================================================================================

  Function: cif_state_done
            set COM quittung
  ---------------------------------------------------------------------------------------
  Input   : dev  - pointer to device instance
  Output  : -
  Return  : -         
  ================================================================================= <En> */

void cif_state_done ( DEV_INSTANCE * dev)
{
  DBG_PRN("DEV instance (%p)\n", dev);

  dev->bStateFlag = STATE_ERR_NON;

  // set internal state
  if ( dev->usBoardIrq != 0) {
    // interrupt mode
    dev->bMyDevFlags ^= STATEACK_FLAG;
  } else {
    // poll mode
    dev->bMyDevFlags  = (unsigned char)(*(dev->pbDevFlags) ^ STATEACK_FLAG);
  }
  *(dev->pbDevFlags)  = dev->bMyDevFlags;

  DBG_PRN("bMyDevFlags=(%x)\n", dev->bMyDevFlags);
}
