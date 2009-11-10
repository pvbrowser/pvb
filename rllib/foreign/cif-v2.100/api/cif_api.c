/* <St> *******************************************************************
  cif_api.c

  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION

    CIF device driver api

  =========================================================================

  CHANGES

  version name        date        Discription
                  March 2001
  -------------------------------------------------------------------------
  V2.000

  NOTE: as groundwork for this header served Windows version of 
        the CIF device driver

  ======================== Copyright =====================================
  Complete package is copyrighted by Hilscher GmbH and is licensed through 
  the GNU General Public License. 
  You should have received a copy of the GNU Library General Public
  License along with this package; if not, please refer to www.gnu.org
  ========================================================================
******************************************************************** <En> */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include "../inc/cif_types.h"
#include "../inc/cif_dev_i.h" 
#include "../usr-inc/rcsdef.h"      

#define INVALID_HANDLE_VALUE -1
int hDevDrv = INVALID_HANDLE_VALUE;         // handle to cifDevDrv
static  unsigned short  DrvOpenCount = 0;   // no Application is logged in

typedef struct DEV_DPM_SIZEtag {
  unsigned long ulDpmSize;
  unsigned long ulDpmIOSize;
} DEV_DPM_SIZE;

DEV_DPM_SIZE tDevDPMSize[MAX_DEV_BOARDS] = { {0L,0L},
                                             {0L,0L},
                                             {0L,0L},
                                             {0L,0L} }; // DPM size of each board

/* <ST> =================================================================================
  Function: DevOpenDriver
            opens a connection to the device driver
  ---------------------------------------------------------------------------------------
  Input   : 
  Output  : -
  Return  : DRV_NO_ERROR           - driver opens successfully
            DRV_USR_OPEN_DRV_ERROR - open error
  ================================================================================= <En> */

short  DevOpenDriver ()
{
short sRet = DRV_NO_ERROR;

  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    // first application, search CIF device driver
    if ( (hDevDrv =  open("/dev/cif",O_RDONLY) ) == -1 )  {    // handle of file with attributes to copy
      // error open CIF driver
      sRet = DRV_USR_OPEN_ERROR;
    } else {
      // driver is opend for the 1st. time, store application in DrvOpencount
      DrvOpenCount = 1;
    }
  } else {
    // driver is already opend, store application in DrvOpencount
    DrvOpenCount++;
  }
  //printf("*** DevOpenDriver(): OpenCount = %d, hDevDrv = %d\n\n", DrvOpenCount, hDevDrv); 
  return sRet;
}

/* <ST> =================================================================================

  Function: DevCloseDriver
            closes the connection to the DEV driver
 ---------------------------------------------------------------------------------------
  Input   : usDevNumber      - not asigned
  Output  : -
  Return  : DRV_NO_ERROR            - driver closed successfully
            DRV_USR_NOT_INITIALIZED - driver not initialized
  ================================================================================= <En> */

short  DevCloseDriver ()
{
short sRet = DRV_NO_ERROR;

  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else {
    // test if other applications are logged in
    if (DrvOpenCount > 1) {
      // there are more applications logged in, do not close the driver
      DrvOpenCount--;
    } else {
      // no more application are logged in, close File Handle
      close( hDevDrv);    // handle of object to close
      DrvOpenCount = 0;
      hDevDrv = INVALID_HANDLE_VALUE;
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevGetBoardInfo
            reads special board informations from the driver
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber      - not asigned
            usSize           - length
            *pvData          - data pointer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevGetBoardInfo ( BOARD_INFO *pvData)
{
  DEVIO_GETBOARDINFOCMD  *tBuffer = NULL;
  BOARD_INFO             *ptBoardInfo = NULL;
  short                  sRet = DRV_NO_ERROR;

  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else {

    if ( (tBuffer = (DEVIO_GETBOARDINFOCMD *)malloc(sizeof(DEVIO_GETBOARDINFOCMD))) == NULL)
      return DRV_USR_SENDBUF_PTR_NULL;

    if ( (tBuffer->ptBoardInfo = (BOARD_INFO *)malloc(sizeof(BOARD_INFO))) == NULL) {
      free(tBuffer);
      return DRV_USR_SENDBUF_PTR_NULL;
    }
    tBuffer->sError = sRet;
    tBuffer->usInfoLen   = sizeof( BOARD_INFO);
    ptBoardInfo = tBuffer->ptBoardInfo;

    if ( !ioctl(hDevDrv, CIF_IOCTLBOARDINFO, tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLBOARDINFO: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
       tBuffer->ptBoardInfo = ptBoardInfo;
       //printf("sRet=%X, &tBuffer=%lX, ->ptBI=%lX\n",sRet,(unsigned long)tBuffer,(unsigned long)ptBoardInfo);
       //printf("BufferLen = %X\n", tBuffer->usInfoLen);
       //printf("Vers = %s\n", ptBoardInfo->abDriverVersion);
       //printf("Add = %lX\n", ptBoardInfo->tBoard[0].ulPhysicalAddress);
       //printf("IRQ = %d\n",  ptBoardInfo->tBoard[0].usIrq);
       memcpy( pvData, ptBoardInfo, sizeof( BOARD_INFO));
       sRet = tBuffer->sError;
    }
    free( ptBoardInfo);
    free( tBuffer);
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevGetBoardInfoEx
            reads special board informations from the driver
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber      - not asigned
            usSize           - length
            *pvData          - data pointer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevGetBoardInfoEx ( BOARD_INFOEX *pvData)
{
  DEVIO_GETBOARDINFOEXCMD *tBuffer = NULL;
  BOARD_INFOEX            *ptBoardInfoEx = NULL;
  short                    sRet = DRV_NO_ERROR;

  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else {
    if( (tBuffer = (DEVIO_GETBOARDINFOEXCMD *)malloc(sizeof(DEVIO_GETBOARDINFOEXCMD))) == NULL)
      return DRV_USR_SENDBUF_PTR_NULL;
    tBuffer->usInfoLen    = sizeof( BOARD_INFOEX);
    if( (tBuffer->ptBoard = (BOARD_INFOEX *)malloc(sizeof(BOARD_INFOEX))) == NULL) {
      free(tBuffer);
      return DRV_USR_SENDBUF_PTR_NULL;
    }
    ptBoardInfoEx = tBuffer->ptBoard;
    tBuffer->usInfoLen = sizeof( BOARD_INFOEX);
    tBuffer->sError    = sRet;

    if ( !ioctl(hDevDrv, CIF_IOCTLBOARDINFOEX, tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLBOARDINFOEX: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      sRet = tBuffer->sError;
      tBuffer->ptBoard = ptBoardInfoEx;
      memcpy( pvData, ptBoardInfoEx, sizeof( BOARD_INFOEX));
    }
    free( ptBoardInfoEx);
    free( tBuffer);
  }
  return sRet;
}

/* <ST> =================================================================================
  Function: DevSetOpMode
            set polling || interrupt operation mode
  ---------------------------------------------------------------------------------------
  Input   : usBoard   - number of the DEV board (0..3)
            usMode    - INTERRUPT_MODE || POLLING_MODE
  Return  : DRV_NO_ERROR     - function successfull
            DRV_DEV_IRQ_REQUEST_FAILED  - function failed
  ================================================================================= <En> */

short DevSetOpMode ( unsigned short usBoard,
		     unsigned short usMode,
		     unsigned short *usIrq)
{
DEVIO_SETOPMODE  tBuffer;
short   sRet = DRV_NO_ERROR;

  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if (usBoard >= MAX_DEV_BOARDS) {
    // number is invalid
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else {
    // clear all data buffers
    tBuffer.usBoard = usBoard;
    tBuffer.usMode  = usMode;
    tBuffer.sError  = sRet;

    if ( !ioctl(hDevDrv, CIF_IOCTL_IRQ_POLL, &tBuffer) ) {
      //fprintf(stderr, "CIFAPI (CIF_IOCTL_IRQ_POLL): %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    } else {
      sRet   = tBuffer.sError;
      *usIrq = tBuffer.usIrq;
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevInitBoard
            initializes a DEV board
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board (0..3)
            *pDevAddress  - not used
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short DevInitBoard ( unsigned short usDevNumber)
{
DEVIO_RESETCMD  tBuffer;
short           sRet = DRV_NO_ERROR;

  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if (usDevNumber >= MAX_DEV_BOARDS) {
    // number is invalid
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else {
    // clear all data buffers
    tBuffer.usBoard = usDevNumber;
    tBuffer.sError  = sRet;

    if ( !ioctl(hDevDrv, CIF_IOCTLINITDRV, &tBuffer) ) {
      //fprintf(stderr, "CIFAPI (CIF_IOCTLINITDRV): %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    } else {
        sRet = tBuffer.sError;
	//printf("CIFAPI (CIF_IOCTLINITDRV): sRet = %d\n", sRet);
	if ( sRet == DRV_NO_ERROR) 
	  {
	    // Save the DPM size for further function calls and calculate the length
	    // of the DPM-IO data area
	    tDevDPMSize[usDevNumber].ulDpmSize    = tBuffer.ulDpmSize;
	    tDevDPMSize[usDevNumber].ulDpmIOSize  = ((tBuffer.ulDpmSize * 1024) - 1024) /2;
	    //printf("CIFAPI: DpmSize = %ld, DpmIOSize = %ld\n", tDevDPMSize[ucDevNumber].ulDpmSize, tDevDPMSize[ucDevNumber].ulDpmIOSize);
	  }
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevReset
            reset a DEV board
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board (0..3)
            usMode        - reset mode              (2,3)
            ulTimeout     - function timeout in milliseconds
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short DevReset ( unsigned char  usDevNumber,
                 unsigned short usMode,
                 unsigned long  ulTimeout)
{
DEVIO_RESETCMD      tBuffer;
unsigned long       lBytesReturned ;
short               sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
   sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usMode != COLDSTART) &&
              (usMode != WARMSTART) &&
              (usMode != BOOTSTART)  ) {
    sRet = DRV_USR_MODE_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.usBoard   = usDevNumber;
    tBuffer.usMode    = usMode;
    tBuffer.ulTimeout = ulTimeout;
    tBuffer.sError    = sRet;
    //printf("CIF_IOCTLRESETDEV: BOARD = %d, mode = %d, tout = %ld\n",
    //       tBuffer.ucBoard, tBuffer.usMode, tBuffer.ulTimeout);

    if ( !ioctl(hDevDrv, CIF_IOCTLRESETDEV, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLRESETDEV: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      sRet = tBuffer.sError;
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevGetInfo
            read the differnt information areas from a DEV board
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usInfoArea    - 1..n
            usSize        - size of the users data buffer
            pvData        - pointer to users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
================================================================================= <En> */

short DevGetInfo ( unsigned short usDevNumber,
		   unsigned short usInfoArea,
		   unsigned short usSize,
		   void           *pvData)
{
DEVIO_GETDEVINFOCMD tBuffer;
unsigned long       lBytesReturned;
short               sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
     sRet = DRV_USR_NOT_INITIALIZED;
   } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( usSize == 0) {
    sRet = DRV_USR_SIZE_ZERO;
  } else {
    // test area spezific data
    switch ( usInfoArea) {
      case GET_VERSION_INFO:
        if ( usSize >  sizeof(VERSIONINFO) ) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
        break;
      case GET_DRIVER_INFO:
        if ( usSize >  sizeof(DRIVERINFO) ) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
        break;
      case GET_FIRMWARE_INFO:
        if ( usSize >  sizeof(FIRMWAREINFO) ) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
        break;
      case GET_RCS_INFO:
        if ( usSize >  sizeof(RCSINFO) ) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
        break;
      case GET_DEV_INFO:
        if ( usSize >  sizeof(DEVINFO) ) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
        break;
      case GET_TASK_INFO:
        if ( usSize >  sizeof(TASKINFO) ) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
	break;
      case GET_IO_INFO:
        if ( usSize >  sizeof(IOINFO) ) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
        break;
      case GET_IO_SEND_DATA:
        if ( usSize > tDevDPMSize[usDevNumber].ulDpmIOSize) {
          sRet = DRV_USR_SIZE_TOO_LONG;
        }
        break;
      default:
        sRet = DRV_USR_INFO_AREA_INVALID;
    } /* end switch */
    if ( sRet == DRV_NO_ERROR) {
      // complete buffer length with return data
      // usDataBufferLen = usSize;
      // fill in parameter data
      lBytesReturned = 0;
      // set output buffer
      tBuffer.usBoard       = usDevNumber;
      tBuffer.usInfoArea    = usInfoArea;
      tBuffer.usInfoLen     = usSize;
      tBuffer.pabInfoData   = pvData;   // needed in kernel-space to copy data back to it!
      tBuffer.sError        = sRet;
      // activate function
      if ( !ioctl(hDevDrv, CIF_IOCTLGETINFO, &tBuffer) ) {
	//fprintf(stderr, "(CIFAPI: (CIF_IOCTLGETINFO): %s\n", strerror(errno));
	sRet = DRV_USR_COMM_ERR;
      }
      else {
	//  memcpy(pvData, tBuffer.pabInfoData, usSize); already copied by driver direct !!!
	sRet = tBuffer.sError;
      }
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevPutTaskParameter
            write communication parameters
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usNumber      - number of the parameter area  (1..7)
            usSize        - size of the parameter area    ( >0, <= max.Size)
            pvData        - pointer to task parameter buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevPutTaskParameter  ( unsigned short usDevNumber,
			      unsigned short usNumber,
			      unsigned short usSize,
			      void           *pvData)
{
DEVIO_PUTPARAMETERCMD tBuffer;
unsigned long         lBytesReturned ;
short                 sRet = DRV_NO_ERROR;
  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if (usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usNumber < 1) ||
	      (usNumber > 7)  ) {  // 3...7 wird in MailBox memory area geschrieben 
    sRet = DRV_USR_NUMBER_INVALID;
  } else if (usSize == 0) {
    sRet = DRV_USR_SIZE_ZERO;
  } else if ( usSize > sizeof(TASKPARAM)) {
    sRet = DRV_USR_SIZE_TOO_LONG;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set command buffer
    tBuffer.usBoard         = usDevNumber;
    tBuffer.usTaskParamNum  = usNumber;
    tBuffer.usTaskParamLen  = usSize;
    memcpy ( (unsigned char *)&(tBuffer.TaskParameter), (unsigned char *)pvData, usSize);
    tBuffer.sError          = sRet;
    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLPARAMETER, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLRESETDEV: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else
      sRet = tBuffer.sError;
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevGetTaskState
            read a task state field form a DEV board
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usNumber      - number of the status field    (1,2)
            usSize        - size of the users data buffer (>0..<= 64)
            pvData        - pointer to users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevGetTaskState( unsigned short usDevNumber,
			unsigned short usNumber,
			unsigned short usSize,
			void           *pvData)
{
DEVIO_GETTASKSTATECMD   tBuffer;
unsigned long           lBytesReturned;
short                   sRet = DRV_NO_ERROR;

 // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
   sRet = DRV_USR_NOT_INITIALIZED;
 } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
 } else if ( usNumber < 1 ||
	     usNumber > 2  ) {
   sRet = DRV_USR_NUMBER_INVALID;
 } else if (usSize == 0) {
   sRet = DRV_USR_SIZE_ZERO;
 } else  if (usSize >  sizeof(TASKSTATE) ) {
   sRet = DRV_USR_SIZE_TOO_LONG;
 } else {
   // fill in parameter data
   lBytesReturned = 0;

   // set command buffer
   tBuffer.ucBoard     = usDevNumber;
   tBuffer.usStateNum  = usNumber;
   tBuffer.usStateLen  = usSize;
   //memcpy( tBuffer.TaskState, pvData, usSize);
   tBuffer.sError      = sRet;
   // activate function
   if ( !ioctl(hDevDrv, CIF_IOCTLTASKSTATE, &tBuffer) ) {
     //fprintf(stderr, "CIF_IOCTLTASKSTATE: %s\n", strerror(errno));
     sRet = DRV_USR_COMM_ERR;
   }
   else {
     memcpy( pvData, tBuffer.TaskState, usSize);
     sRet = tBuffer.sError;
   }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevGetMBXState
            Test send and receive MBX
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber       - number of the DEV board       (0)
            *pusDevMbxState   - 0 = empty, 1 = full
            *pusHostMBXState  - 0 = empty, 1 = full
  Output  : -
  Return  : DRV_NO_ERROR      - function successfull
  ================================================================================= <En> */

short  DevGetMBXState ( unsigned short usDevNumber,
			unsigned short *pusDevMbxState,
			unsigned short *pusHostMbxState)
{
DEVIO_MBXINFOCMD  tBuffer;
unsigned long     lBytesReturned ;
short             sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.ucBoard        = usDevNumber;
    tBuffer.usDevMbxState  = 0x00;//*pusDevMbxState;
    tBuffer.usHostMbxState = 0x00;//*pusHostMbxState;
    tBuffer.sError         = sRet;
    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLMBXINFO, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLMBXINFO: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      *pusDevMbxState  = tBuffer.usDevMbxState;
      *pusHostMbxState = tBuffer.usHostMbxState;
      sRet = tBuffer.sError;
    }
  }
  return sRet;
}

/* <ST> =================================================================================
  Function: DevGetMBXData
            reads the device and the host mailbox
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber      - not asigned
            usHostSize       - length of the host data to be read
            *pvHostData      - buffer for host data pointer
            usDevSize        - length of the device data to be read
            *pvHostData      - buffer for device data pointer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */
short DevGetMBXData ( unsigned short usDevNumber,
		      unsigned short usHostSize,
		      void           *pvHostData,
		      unsigned short usDevSize,
		      void           *pvDevData)
{
DEVIO_GETMBXCMD   tBuffer;
unsigned long     lBytesReturned;
short             sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usHostSize == 0) ||
              (usDevSize == 0)   ) {
    sRet = DRV_USR_SIZE_ZERO;
  } else if ( (usHostSize > sizeof(MSG_STRUC)) ||
              (usDevSize  > sizeof(MSG_STRUC))   ){
    sRet = DRV_USR_SIZE_TOO_LONG;
  } else {
    // fill in parameter data
    lBytesReturned = 0;

    tBuffer.usBoard     = usDevNumber;
    tBuffer.usHostLen   = usHostSize;
    tBuffer.usDevLen    = usDevSize;
    tBuffer.sError     = sRet;

    // activate function
    if ( !ioctl( hDevDrv,                    // handle of the device
		 CIF_IOCTLGETMBX,            // control code of operation to perform
		 (void *)&tBuffer,           // address of buffer for input data
		 sizeof(tBuffer),            // size of input buffer
		 NULL,                       // address of output buffer
		 0,                          // size of output buffer
		 (unsigned long *)&lBytesReturned,   // address of actual bytes of output
		 NULL                        // address of overlapped structure
		 )  ) {
      //fprintf(stderr, "CIF_IOCTLGETMBX: %s\n", strerror(errno));
      // function error
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      memcpy( pvDevData,  tBuffer.abDevMbx,  usDevSize); 
      memcpy( pvHostData, tBuffer.abHostMbx, usHostSize); 
      sRet = tBuffer.sError;
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevExitBoard
            closes the connection to a DEV board
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber - number of the DEV board (0..3)
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
           != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short DevExitBoard ( unsigned short usDevNumber)
{
DEVIO_EXITCMD   tBuffer;
unsigned long   lBytesReturned;
unsigned short  usDrvOpenCount;
short           sRet = DRV_NO_ERROR;

  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    // number is invalid
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else {
    // valid handle available, driver is open
    // clear all data buffers
    lBytesReturned         = 0;
    tBuffer.usBoard        = usDevNumber;      // [in]
    //tBuffer.usDrvOpenCount = usDrvOpenCount; // [out] 
    //tBuffer.sError         = sRet;           // [out]

    if ( !ioctl(hDevDrv, CIF_IOCTLEXITDRV, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLEXITDRV: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    } else {
      tBuffer.usDrvOpenCount = usDrvOpenCount; // [out] 
      sRet = tBuffer.sError;                   // [out]
    }
  }

  return sRet;
}

/* <ST> =================================================================================
  Function: DevReadSendData
            reads the send data from the send area
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usReadOffset  -
            usReadSize    - size of the users data buffer
            pvReadData    - pointer to users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevReadSendData ( unsigned short usDevNumber,
			 unsigned short usOffset,
			 unsigned short usSize,
			 void           *pvData)
{
DEVIO_READSENDCMD   tBuffer;
unsigned long       lBytesReturned;
short               sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
   sRet = DRV_USR_NOT_INITIALIZED;
 } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usSize != 0) &&
              ((usSize + usOffset) > (unsigned short)tDevDPMSize[usDevNumber].ulDpmIOSize) ) {
    sRet = DRV_USR_SIZE_TOO_LONG;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.usBoard      = usDevNumber;
    tBuffer.usReadOffset = usOffset;
    tBuffer.usReadLen    = usSize;
    tBuffer.pabReadData  = pvData;
    tBuffer.sError       = sRet;

    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLREADSEND, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLREADSEND: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      sRet = tBuffer.sError;      
    }
  }
  return sRet;
}

/* <ST> =================================================================================
  Function: DevTriggerWatchDog
            triggers the PC watchdog an delivers the actual DEV watchdog
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber     - number of the DEV board       (0..3)
            usMode          - 0 = start, 1 = stop
            *pusDevWatchDog - size of the users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short DevTriggerWatchDog ( unsigned short usDevNumber,
			   unsigned short usMode,
			   unsigned short *pusDevWatchDog)
{
DEVIO_TRIGGERCMD  tBuffer;
unsigned long     lBytesReturned ;
short             sRet = DRV_NO_ERROR;
  // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
   sRet = DRV_USR_NOT_INITIALIZED;
 } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( usMode > WATCHDOG_START) {
    sRet = DRV_USR_MODE_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned  = 0;
    // set output buffer
    tBuffer.usBoard = usDevNumber;
    tBuffer.usMode  = usMode;
    //tBuffer.pusTriggerValue = pusDevWatchDog;
    tBuffer.sError  = sRet;

    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLTRIGGERWD, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLREADSEND: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      *pusDevWatchDog = tBuffer.usTriggerValue;
      sRet = tBuffer.sError;      
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevSpecialControl
            Change the special control value in DevFlags
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber     - number of the DEV board       (0..3)
            usMode          - 0 = SPC_CTRL_CLEAR, 1 = SPC_CTRL_SET
            *pusDevWatchDog - size of the users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short DevSpecialControl ( unsigned short usDevNumber,
			  unsigned short usMode,
			  unsigned short *pusCtrlAck)
{
DEVIO_TRIGGERCMD  tBuffer;
unsigned long     lBytesReturned ;
short             sRet = DRV_NO_ERROR;
  // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
   sRet = DRV_USR_NOT_INITIALIZED;
 } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( pusCtrlAck == NULL) {
    sRet = DRV_USR_BUF_PTR_NULL;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.usBoard = usDevNumber;
    tBuffer.usMode  = usMode;
    tBuffer.sError  = sRet;

    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLSPCONTROL, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLREADSEND: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else { 
      *pusCtrlAck = tBuffer.usTriggerValue;
      sRet = tBuffer.sError;      
    }
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevExtendedData
            extended data function
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber      - not asigned
            usMode           - 1 ...n
            usSize           - length of user data
            *pvData          - data pointer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevExtendedData ( unsigned short usDevNumber,
			 unsigned short usMode,
			 unsigned short usSize,
			 void           *pvData)
{
DEVIO_EXTDATACMD  tBuffer;
unsigned long     lBytesReturned;
short             sRet = DRV_NO_ERROR;
  // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
   sRet = DRV_USR_NOT_INITIALIZED;
 } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( usSize == 0) {
    sRet = DRV_USR_SIZE_ZERO;
  } else if ( usSize > EXTDATASIZE) {
    sRet = DRV_USR_SIZE_TOO_LONG;
  } else if ( (usMode == 0) ||
              (usMode > 100) ) {
    sRet = DRV_USR_MODE_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned = 0;

    tBuffer.usBoard    = usDevNumber;
    tBuffer.usMode     = usMode;
    tBuffer.pabExtData = pvData;
    tBuffer.sError     = sRet;

    // insert error for older driver versions, because function which are not included
    // in the driver returns always DeviceIoControl success without setting an
    // error into the tDataBuffer.
    sRet = DRV_USR_COMM_ERR;
    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLEXTDATA, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLEXTDATA: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else 
      sRet = tBuffer.sError;      
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevGetTaskParameter
            reads the task parameters from task 1 and 2
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usArea        - 1,2
            usSize        - size of the users data buffer
            pvData        - pointer to users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevGetTaskParameter  ( unsigned short usDevNumber,
			      unsigned short usNumber,
			      unsigned short usSize,
			      void           *pvData)
{
DEVIO_GETPARAMETERCMD   tBuffer;
unsigned long           lBytesReturned ;
short                   sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
 if ( hDevDrv == INVALID_HANDLE_VALUE) {
   sRet = DRV_USR_NOT_INITIALIZED;
 } else if (usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usNumber < 1) ||
              (usNumber > 2)  ) {
    sRet = DRV_USR_NUMBER_INVALID;
  } else if (usSize == 0) {
    sRet = DRV_USR_SIZE_ZERO;
  } else if ( usSize > sizeof(TASKPARAM)) {
    sRet = DRV_USR_SIZE_TOO_LONG;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set command buffer
    tBuffer.usBoard        = usDevNumber;
    tBuffer.usTaskParamNum = usNumber;
    tBuffer.usTaskParamLen = usSize;
    //tBuffer.ptTaskParam    = pvData;
    tBuffer.sError         = sRet;
    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLGETPARAMETER, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLGETPARAMETER: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      memcpy ( (unsigned char *)pvData, (unsigned char *)&(tBuffer.TaskParameter), usSize);
      sRet = tBuffer.sError;      
    }
  }
  return sRet;
}

/* <ST> =================================================================================
  Function: DevReadWriteDPMData
            Function to access the whole DPM
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber     - number of the DEV board       (0..3)
            *pusBytes       - Amount of bytes sended
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevReadWriteDPMData  ( unsigned short usDevNumber,
			      unsigned short usMode,
			      unsigned short usOffset,
			      unsigned short usSize,
			      void           *pvData)
{
DEVIO_RWDPMDATACMD  tBuffer;
unsigned long       lBytesReturned;
short               sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if (usDevNumber >= MAX_DEV_BOARDS ) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usMode != PARAMETER_READ) &&
              (usMode != PARAMETER_WRITE) ) {
    sRet = DRV_USR_MODE_INVALID;
  } else if ( ( usSize != 0) &&
              ( (usSize + usOffset) > (unsigned short)(tDevDPMSize[usDevNumber].ulDpmSize * 1024))) {
    sRet = DRV_USR_SIZE_TOO_LONG;;
  } else {
    // fill in parameter data
    lBytesReturned   = 0;
    tBuffer.usBoard  = usDevNumber;
    tBuffer.usMode   = usMode;
    tBuffer.usOffset = usOffset;
    tBuffer.usLen    = usSize;
    tBuffer.pabData  = pvData;
    tBuffer.sError   = sRet;

    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLRWDPMDATA, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLRWDPMDATA: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else
      sRet = tBuffer.sError;      
  } /* endif */
  return sRet;
} /* end funktion */

/* <ST> =================================================================================
  Function: DevReadWriteDPMRaw
            reads and writes bytes from the DPM, last KByte
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0)
            usMode        - function read = 1, write = 2
            usOffset      - offset in the RAW data area
            usSize        - number of bytes to read or write
            pvData        - pointer to the user data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevReadWriteDPMRaw  ( unsigned short usDevNumber,
			     unsigned short usMode,
			     unsigned short usOffset,
			     unsigned short usSize,
			     void           *pvData)
{
DEVIO_RWRAWDATACMD  tBuffer;
unsigned long       lBytesReturned;
short               sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if (usDevNumber >= MAX_DEV_BOARDS ) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usSize + usOffset) > sizeof(RAWDATA) ) {
    sRet = DRV_USR_SIZE_TOO_LONG;
  } else if ( (usMode != PARAMETER_READ) &&
              (usMode != PARAMETER_WRITE) ) {
    sRet = DRV_USR_MODE_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned   = 0;
    tBuffer.usBoard  = usDevNumber;
    tBuffer.usMode   = usMode;
    tBuffer.usOffset = usOffset;
    tBuffer.usLen    = usSize;
    tBuffer.pabData  = pvData;
    tBuffer.sError   = sRet;

    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLRWRAW, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLRWRAW: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else
      sRet = tBuffer.sError;      
  } /* endif */
  return sRet;
} /* end funktion */

/* <ST> =================================================================================
  Function: DevExchangeIO
            reads and writes IO datas
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usInfoArea    - 1..n
            usSize        - size of the users data buffer
            pvData        - pointer to users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevExchangeIO ( unsigned short usDevNumber,
		       unsigned short usSendOffset,
		       unsigned short usSendSize,
		       void           *pvSendData,
		       unsigned short usReceiveOffset,
		       unsigned short usReceiveSize,
		       void           *pvReceiveData,
		       unsigned long  ulTimeout)
{
DEVIO_EXIOCMD     tBuffer;
unsigned long     lBytesReturned;
short             sRet = DRV_NO_ERROR;
int               lRet = 0;
  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usSendSize != 0) &&
              ((usSendSize + usSendOffset) > (unsigned short)tDevDPMSize[usDevNumber].ulDpmIOSize) ) {
    sRet = DRV_USR_SENDSIZE_TOO_LONG;
  } else if ( (usReceiveSize != 0) &&
              ((usReceiveSize + usReceiveOffset) > (unsigned short)tDevDPMSize[usDevNumber].ulDpmIOSize) ) {
    sRet = DRV_USR_RECVSIZE_TOO_LONG;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.usBoard         = usDevNumber;
    tBuffer.usReceiveOffset = usReceiveOffset;
    tBuffer.usReceiveLen    = usReceiveSize;
    tBuffer.usSendOffset    = usSendOffset;
    tBuffer.usSendLen       = usSendSize;
    tBuffer.pabSendData     = pvSendData;
    tBuffer.pabReceiveData  = pvReceiveData;
    tBuffer.sError          = sRet;
    tBuffer.ulTimeout       = ulTimeout;

    // activate function
    lRet =  ioctl (hDevDrv, CIF_IOCTLEXIO, &tBuffer);
    if ( lRet <= 0) {
      //fprintf(stderr, "CIF_IOCTLEXIO: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else 
      sRet = tBuffer.sError;
  }
  return sRet;
}

/* <ST> =================================================================================
  Function: DevExchangeIOEx
            reads and writes IO datas
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usMode        - Exchange mode
            usInfoArea    - 1..n
            usSize        - size of the users data buffer
            pvData        - pointer to users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short DevExchangeIOEx (  unsigned short usDevNumber,
			 unsigned short usMode,
			 unsigned short usSendOffset,
			 unsigned short usSendSize,
			 void           *pvSendData,
			 unsigned short usReceiveOffset,
			 unsigned short usReceiveSize,
			 void           *pvReceiveData,
			 unsigned long  ulTimeout)
{
DEVIO_EXIOCMDEX   tBuffer;
unsigned long     lBytesReturned;
short             sRet = DRV_NO_ERROR;
int               lRet = 0;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {    
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usSendSize != 0) &&
              ((usSendSize + usSendOffset) > (unsigned short)tDevDPMSize[usDevNumber].ulDpmIOSize) ) {
    sRet = DRV_USR_SENDSIZE_TOO_LONG;
  } else if ( (usReceiveSize != 0) &&
              ((usReceiveSize + usReceiveOffset) > (unsigned short)tDevDPMSize[usDevNumber].ulDpmIOSize) ) {
    sRet = DRV_USR_RECVSIZE_TOO_LONG;
  } else if ( usMode > 4) {
    sRet = DRV_USR_MODE_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned = 0;

    // set output buffer
    tBuffer.usBoard         = usDevNumber;
    tBuffer.usReceiveOffset = usReceiveOffset;
    tBuffer.usReceiveLen    = usReceiveSize;
    tBuffer.usSendOffset    = usSendOffset;
    tBuffer.usSendLen       = usSendSize;
    tBuffer.pabSendData     = pvSendData;
    tBuffer.pabReceiveData  = pvReceiveData;
    tBuffer.ulTimeout       = ulTimeout;
    tBuffer.sError          = sRet;
    tBuffer.usMode          = (unsigned short)(usMode + 1); // Driver uses 1..5

    // activate function
    lRet =  ioctl (hDevDrv, CIF_IOCTLEXIOEX, &tBuffer);
    if ( lRet <= 0) {
      //fprintf(stderr, "CIF_IOCTLEXIO: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else 
      sRet = tBuffer.sError;
  }
  return sRet;
}


/* <ST> =================================================================================
  Function: DevExchangeIOErr
            reads and writes IO datas with state field information
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usMode        - Exchange mode
            usInfoArea    - 1..n
            usSize        - size of the users data buffer
            pvData        - pointer to users data buffer
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevExchangeIOErr(  unsigned short usDevNumber,
			  unsigned short usSendOffset,
			  unsigned short usSendSize,
			  void           *pvSendData,
			  unsigned short usReceiveOffset,
			  unsigned short usReceiveSize,
			  void           *pvReceiveData,
			  COMSTATE       *ptState,
			  unsigned long  ulTimeout)
{
DEVIO_EXIOCMDERR  tBuffer;
unsigned long     lBytesReturned;
short             sRet = DRV_NO_ERROR;
int               lRet = 0;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (usSendSize != 0) &&
              ((usSendSize + usSendOffset) > (unsigned short)tDevDPMSize[usDevNumber].ulDpmIOSize) ) {
    sRet = DRV_USR_SENDSIZE_TOO_LONG;
  } else if ( (usReceiveSize != 0) &&
              ((usReceiveSize + usReceiveOffset) > (unsigned short)tDevDPMSize[usDevNumber].ulDpmIOSize) ) {
    sRet = DRV_USR_RECVSIZE_TOO_LONG;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.usBoard         = usDevNumber;
    tBuffer.usReceiveOffset = usReceiveOffset;
    tBuffer.usReceiveLen    = usReceiveSize;
    tBuffer.usSendOffset    = usSendOffset;
    tBuffer.usSendLen       = usSendSize;
    tBuffer.pabSendData     = pvSendData;
    tBuffer.pabReceiveData  = pvReceiveData;
    tBuffer.ptStateData     = ptState;
    tBuffer.sError          = sRet;
    tBuffer.ulTimeout       = ulTimeout;

    // activate function
    lRet =  ioctl (hDevDrv, CIF_IOCTLEXIOERR, &tBuffer);
    if ( lRet <= 0) {
      //fprintf(stderr, "CIF_IOCTLEXIO: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else 
      sRet = tBuffer.sError;
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevSetHostState
            set the host state
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board       (0..3)
            usMode        - 0 = Host not ready, 1 = Host ready
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevSetHostState ( unsigned short usDevNumber,
			 unsigned short usMode,
			 unsigned long  ulTimeout)
{
DEVIO_TRIGGERCMD     tBuffer;
unsigned long        lBytesReturned;
short                sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( usMode > HOST_READY) {
    sRet = DRV_USR_MODE_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned = 0;

    // set output buffer
    tBuffer.usBoard   = usDevNumber;
    tBuffer.usMode    = usMode;
    tBuffer.ulTimeout = ulTimeout;
    tBuffer.sError    = sRet;
    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLSETHOST, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLSETHOST: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else
      sRet = tBuffer.sError;      
  }
  return sRet;
}

/* <ST> =================================================================================

  Function: DevPutMessage
            send a message to a DEV board
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board (0..3)
            ptMessage     - pointer to users message
            ulTimeout     - function timeout in milliseconds
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevPutMessage ( unsigned short usDevNumber,
		       MSG_STRUC      *ptMessage,
		       unsigned long  ulTimeout)
{
DEVIO_PUTMESSAGECMD  tBuffer;
unsigned long        lBytesReturned;
short                sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.usBoard   = usDevNumber;
    tBuffer.ulTimeout = ulTimeout;
    memcpy( &tBuffer.tMsg, ptMessage, sizeof(MSG_STRUC));
    tBuffer.sError    = sRet;

    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLPUTMSG, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLPUTMSG: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else
      sRet = tBuffer.sError;      
  }
  return sRet;
}


/* <ST> =================================================================================

  Function: DevGetMessage
            read a message from a DEV board
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - number of the DEV board     (0..3)
            usSize        - size of the users data area (>0..<= sizeof(MSG_STRUC))
            ptMessage     - pointer to the users data buffer
            ulTimeout     - function timeout in milliseconds
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevGetMessage ( unsigned short usDevNumber,
		       unsigned short usSize,
		       MSG_STRUC      *ptMessage,
		       unsigned long  ulTimeout)
{
DEVIO_GETMESSAGECMD   tBuffer;
unsigned long         lBytesReturned;
short                 sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( usSize == 0 ||
              usSize > sizeof(MSG_STRUC)) {
    sRet = DRV_USR_SIZE_INVALID;
  } else {
    // fill in parameter data
    lBytesReturned = 0;
    // set output buffer
    tBuffer.usBoard   = usDevNumber;
    tBuffer.ulTimeout = ulTimeout;
    tBuffer.sError    = sRet;
    // activate function
    if ( !ioctl(hDevDrv, CIF_IOCTLGETMSG, &tBuffer) ) {
      //fprintf(stderr, "CIF_IOCTLGETMSG: %s\n", strerror(errno));
      sRet = DRV_USR_COMM_ERR;
    }
    else {
      memcpy( ptMessage, &tBuffer.tMsg, sizeof(MSG_STRUC));
      sRet = tBuffer.sError;
    }
  }
  return sRet;
}

/* ======================================================================================
                                       DOWNLOAD part
   =================================================================================== */

/* ------------------------------------------------------------------------------------ */
/*  Locale variables                                                                    */
/* ------------------------------------------------------------------------------------ */

#define MAX_DOWNLOAD_MSG_LENGTH  240
#define TO_COLDSTART             10000L
#define TO_SEND_MSG              500L
#define TO_1ST_MSG               10000L
#define TO_CONT_MSG              1000L
#define TO_LAST_MSG              15000L

// File data structure
typedef struct tagFILEDATA {
  int           fd;
  //struct stat   *Info;//struct _stat  *Info;
  char          *pabBuffer;
  long          lSize;
} FILEDATA;

typedef struct tagDEVICE_TYPE_INFO {
  unsigned char  bHerstellerkennung;
  unsigned char  bDeviceType;
  unsigned char  bDeviceModel;
  unsigned char  bReserve3;
} DEVICE_TYPE_INFO;

/* <ST> =================================================================================
  Function: CloseFile
            Close a previously opened file by OpenFile(), free all memory and handles
  ---------------------------------------------------------------------------------------
  Input   : ptFile        - File data structure
  Output  : -
  Return  : -
  ================================================================================= <En> */
void closeFile ( FILEDATA *ptFile) 
{
  if ( ptFile->fd >= 0) {
    if ( ptFile->pabBuffer != NULL) {
      free( ptFile->pabBuffer);
    }
    close( ptFile->fd);
    
    // Clear file structure
    memset ( ptFile, 0, sizeof(FILEDATA));
  }
}

/* <ST> =================================================================================
  Function: openFile
            Open a file and load the file into memory
  ---------------------------------------------------------------------------------------
  Input   : pwFileName    - Filename including a path
            ptFile        - File data structure
  Output  : -
  Return  : DRV_NO_ERROR  - File successfully opened and loaded into memory
  ================================================================================= <En> */
short openFile ( const char *fileName, FILEDATA *ptFile) 
{
  unsigned long  lNumberOfBytesRead;
  short          sRet = DRV_NO_ERROR;
  // Clear file structure
  memset ( ptFile, 0, sizeof(FILEDATA));
  // open the existing file
  ptFile->fd = open( fileName, O_RDONLY);

  if ( ptFile->fd < 0 ) {
    // File not opend
    sRet = DRV_USR_FILE_OPEN_FAILED;
  } else {
    // Get the file size
    ptFile->lSize = lseek( ptFile->fd, 0, SEEK_END);
    if ( ptFile->lSize <= 0) {
      // File size is zero
      sRet = DRV_USR_FILE_SIZE_ZERO;
    } else {
      if( lseek( ptFile->fd, 0, SEEK_SET) < 0)
	sRet = DRV_USR_FILE_READ_FAILED;
      else {
	// Allocate memory for file data
	if ( (ptFile->pabBuffer = (char*)malloc( ptFile->lSize)) == NULL) {
	  // Error by memory allocation
	  sRet = DRV_USR_FILE_NO_MEMORY;
	  closeFile( ptFile);
	} else {
	  // Read file data into memory
	  if ( ( lNumberOfBytesRead = read( ptFile->fd,              // handle of file to read 
					    ptFile->pabBuffer,       // address of buffer that receives data 
					    ptFile->lSize) ) < 0) {  // number of bytes to read 
	    // File read into memory failed
	    sRet = DRV_USR_FILE_READ_FAILED;
	    closeFile( ptFile);
	  } 
	}
      }
    }
  } 
  return sRet;
}

//----------------------------------------------------------------------------------------
// Free receive mailbox
//----------------------------------------------------------------------------------------
short FreeRecvMailbox( unsigned short usDevNumber)
{
  // Read all messages from the device
  MSG_STRUC tRecvMsg;
  unsigned short usDevState, usHostState, usIdx;
  short sRet;

  // Read until no msg available
  usIdx = 0;    
  do {
    if ( (sRet = DevGetMBXState(  usDevNumber,
                                  &usDevState,
                                  &usHostState)) != DRV_NO_ERROR) {
      // Something wrong, end function
      break;
    } else {
      // Check if message available
      if ( usHostState == HOST_MBX_FULL) {
        // Read message and throw away
        DevGetMessage( usDevNumber,
                       sizeof(MSG_STRUC),
                       &tRecvMsg,
                       100L);
      } else {
        break;
      }
    }
    usIdx++;
  } while  ( usIdx < 150); // A maximum of 150 times

  return sRet;
}

//----------------------------------------------------------------------------------------
// Create checksum
//----------------------------------------------------------------------------------------
unsigned short CreateChecksum( unsigned char *pabData,
                               long          lDataLen,
                               MSG_STRUC     *ptSendMsg)
{
  long            lIdx, lTempLen;
  unsigned short  usCheckSum, usTemp;
  unsigned char   *pByte;

  // Create length for checksum calculating
  usCheckSum  = 0;    // Clear checksum
  lTempLen    = lDataLen - 16384 - 64;

  pByte = &ptSendMsg->data[3];
  // Calculate the Checksum, start with the first message Msg.d[3] up to Msg.d[64]
  for ( lIdx = 0; lIdx < 31; lIdx++) {
    usTemp = (*(pByte+1));
    usTemp = (unsigned short)((usTemp << 8) | (*pByte));
    usCheckSum = (unsigned short)(usCheckSum + usTemp);
    pByte +=2;        // Next short value
  }

  pByte = &pabData[64];
  // Create cheksum for the rest of the file
  do {
    usTemp = (*(pByte+1));
    usTemp = (unsigned short)((usTemp << 8) | (*pByte));
    usCheckSum = (unsigned short)(usCheckSum + usTemp);
    pByte += 2;       // Next short value
    lTempLen-=2;
  } while (lTempLen > 0);

  // Return 
  return usCheckSum;
}

//----------------------------------------------------------------------------------------
// Transfer messages
//----------------------------------------------------------------------------------------
short TransferMessage( unsigned short usDevNumber,
                       MSG_STRUC      *ptSendMsg,
                       MSG_STRUC      *ptRecvMsg,
                       long           lTimeout)
{
  long  lCount  = 0;
  short sRet    = DRV_NO_ERROR;

  //printf("cifAPI:(1) %.2x-%.2x-%.2x-%.2x-%.2x-%.2x-%.2x - %lx\n", 
  //       ptSendMsg->tx,ptSendMsg->rx,ptSendMsg->a,ptSendMsg->b,ptSendMsg->nr,ptSendMsg->f,ptSendMsg->ln, lTimeout);
  if ( (sRet = DevPutMessage ( usDevNumber,                
                               ptSendMsg,                  
                               TO_SEND_MSG)) == DRV_NO_ERROR) {
    do {
      if ( (sRet = DevGetMessage ( usDevNumber,            
                                   sizeof(MSG_STRUC),      
                                   ptRecvMsg,              
                                   lTimeout)) == DRV_NO_ERROR) {
	//printf("cifAPI:(2) %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", 
	//ptRecvMsg->tx,ptRecvMsg->rx,ptRecvMsg->a,ptRecvMsg->b,ptRecvMsg->nr,ptRecvMsg->f);
        // Check on message errors
        if ( (ptRecvMsg->tx == ptSendMsg->rx) &&
             (ptRecvMsg->rx == ptSendMsg->tx) &&
             (ptRecvMsg->a  == ptSendMsg->b)  &&
             (ptRecvMsg->b  == 0)             &&
             (ptRecvMsg->nr == ptSendMsg->nr)  ) {
          // Check on message error
	  //printf("cifAPI:(3) %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", 
	  //       ptRecvMsg->tx,ptRecvMsg->rx,ptRecvMsg->a,ptRecvMsg->b,ptRecvMsg->nr,ptRecvMsg->f);
          if ( ptRecvMsg->f != 0) {
            sRet = (short)(ptRecvMsg->f + DRV_RCS_ERROR_OFFSET);
            break;
          } else {
            break;
          }
        }
      }
      //  else
      //    printf("cifAPI: else { %c-%c-%c-%c-%c-%c\n", ptRecvMsg->tx,ptRecvMsg->rx,ptRecvMsg->a,ptRecvMsg->b,ptRecvMsg->nr,ptRecvMsg->f);
      lCount++;
    } while ( lCount < 10);
  }
  //sleep(0);
  return sRet;
}

//----------------------------------------------------------------------------------------
// Read device information
//----------------------------------------------------------------------------------------
short ReadDeviceInformation( unsigned short usDevNumber,
                             const char     *fileName)
{
  long      dwStrLen;
  short     sRet;
  MSG_STRUC         tSendMsg, tRecvMsg;
  DEVICE_TYPE_INFO  *pInfo;

  memset( &tSendMsg, 0, sizeof( MSG_STRUC));
  memset( &tRecvMsg, 0, sizeof( MSG_STRUC));

  if ( (sRet = FreeRecvMailbox( usDevNumber)) == DRV_NO_ERROR) {
    // Insert data into message
    tSendMsg.rx     = RCS_TASK;
    tSendMsg.tx     = MSG_SYSTEM_TX;
    tSendMsg.ln     = 1;
    tSendMsg.nr     = 0;
    tSendMsg.a      = 0;
    tSendMsg.f      = 0;
    tSendMsg.b      = RCS_B_SYSFKT;
    tSendMsg.e      = 0;
    // Insert data
    tSendMsg.data[0] =  MODE_GET_PROJ_WERTE_HW;
    if ( (sRet = TransferMessage ( usDevNumber,
                                   &tSendMsg,
                                   &tRecvMsg,
                                   TO_SEND_MSG)) == DRV_NO_ERROR) {
      // Check entry
      dwStrLen = strlen( fileName);
      pInfo = (DEVICE_TYPE_INFO*)&tRecvMsg.data[0];
      if ( (pInfo->bHerstellerkennung != (char)(toupper( fileName[dwStrLen-3]))) ||
           (pInfo->bDeviceType        != (char)(toupper( fileName[dwStrLen-2]))) ||
           (pInfo->bDeviceModel       != (char)(toupper( fileName[dwStrLen-1])))  ) {
        sRet = DRV_USR_INVALID_FILETYPE;
      }
    }
  }

  return sRet;
}

/* <ST> =================================================================================
  Function: RunFirmwareDownload
            Download a firmware to a device
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - Device number
            pwFile        - File name including a path
            *pdwBytes     - Already loaded bytes
  Output  : -
  Return  : DRV_NO_ERROR  - Download successfully
  ================================================================================= <En> */
short RunFirmwareDownload( unsigned short usDevNumber,
                           FILEDATA       *ptFile,
                           unsigned long  *pdwByte)
{
  long            lFileLength, lSendLen, lActIdx;
  unsigned short  usCheckSum, usTemp;
  unsigned char   *pabData;
  MSG_STRUC       tSendMsg, tRecvMsg;
  BOOL            fRet;
  short           sRet;
  unsigned long   dwState = RCS_FIRST_MSK;  

  // Load pointer to data
  pabData   = (unsigned char*)ptFile->pabBuffer; 
  lActIdx   = 0;      // Clear actual index
  *pdwByte  = lActIdx;

  // Clear messages
  memset( &tSendMsg, 0, sizeof(MSG_STRUC));
  memset( &tRecvMsg, 0, sizeof(MSG_STRUC));

  // Set data length
  lFileLength = ptFile->lSize;
  // Set maximum message length
  if ( lFileLength > MAX_DOWNLOAD_MSG_LENGTH) lSendLen = MAX_DOWNLOAD_MSG_LENGTH;
  else lSendLen = lFileLength;

  // Set program state
  dwState = RCS_FIRST_MSK;
  fRet = TRUE;
  // Run download
  do {
    switch ( dwState) {
      //--------------------------
      // Send first message
      //--------------------------
      case RCS_FIRST_MSK:
        // Set HOST_READY for download, maybe it is disabled
        sRet = DevSetHostState(  usDevNumber,
                                 HOST_READY,
                                 0L);
        // Insert data into message
        tSendMsg.rx     = RCS_TASK;
        tSendMsg.tx     = MSG_SYSTEM_TX;
        tSendMsg.ln     = 65;
        tSendMsg.nr     = 1;
        tSendMsg.a      = 0;
        tSendMsg.f      = 0;
        tSendMsg.b      = RCS_B_LOADFKT;
        tSendMsg.e      = RCS_FIRST_MSK;
        // Insert data
        tSendMsg.data[0]      = 6;
        memcpy( &tSendMsg.data[3], "RCSCODE", 7);
        usTemp = (unsigned short)( lFileLength / 16);
        tSendMsg.data[13]     = (unsigned char)(usTemp & 0x00FF);
        tSendMsg.data[14]     = (unsigned char)((usTemp >> 8) & 0x00FF);
        tSendMsg.data[15]     = 6;
        memcpy( &tSendMsg.data[17], "RCSCODE", 7);
        tSendMsg.data[27]     = 255;
        tSendMsg.data[49]     = 255;

        // Create check sum
        usCheckSum = CreateChecksum( pabData,
                                     lFileLength,
                                     &tSendMsg);                        
        // Insert check sum
        tSendMsg.data[1] = (unsigned char)(usCheckSum & 0x00FF);
        tSendMsg.data[2] = (unsigned char)((usCheckSum >> 8) & 0x00FF);

        // Process message
        if ( (sRet = TransferMessage( usDevNumber,
                                      &tSendMsg,
                                      &tRecvMsg,
                                      TO_1ST_MSG)) != DRV_NO_ERROR ) {
          // Could not process this message
          fRet = FALSE;
        } else {
          // ----------------------------
          // Message send second message
          // ----------------------------
          // Save fimware length given in this RCS message
          // Set download length to this size
          lFileLength = *(long*)(&tRecvMsg.data[0]);

          // Send next message
          tSendMsg.rx = RCS_TASK;
          tSendMsg.tx = MSG_SYSTEM_TX;
          tSendMsg.ln = (unsigned char)lSendLen;
          tSendMsg.nr = ++tSendMsg.nr;
          tSendMsg.a  = 0;
          tSendMsg.f  = 0;
          tSendMsg.b  = RCS_B_LOADFKT;
          tSendMsg.e  = RCS_CONT_MSK;

          // Set byte 0 to 63 to byte 1 to 65 of the 1st message
          memcpy ( &tSendMsg.data[0], &tSendMsg.data[1], 64);

          // Insert byte 64 to 239 from the abData[64]
          memcpy ( &tSendMsg.data[64], &pabData[64], (unsigned char)(lSendLen - 64));

          // Process message
          if ( (sRet = TransferMessage( usDevNumber,
                                        &tSendMsg,
                                        &tRecvMsg,
                                        TO_1ST_MSG)) != DRV_NO_ERROR ) {
            // Could not process this message
            fRet = FALSE;
          } else {
            // No error, send next message
            lActIdx += lSendLen;                  // Add send size to actual index
            *pdwByte  = lActIdx;

            // Calculate next message length
            if ( lFileLength <= (lSendLen + lActIdx)) {
              lSendLen = lFileLength - lActIdx;   // Set length to rest of data
              dwState = RCS_LAST_MSK;
            } else {
              dwState = RCS_CONT_MSK;
            }
          }
        }    
        break;
      //--------------------------
      // Send continue message
      //--------------------------
      case RCS_CONT_MSK:
        tSendMsg.e = RCS_CONT_MSK;
        tSendMsg.ln = (unsigned char)lSendLen;
        tSendMsg.nr = ++tSendMsg.nr;

        // Send next message
        memcpy ( &tSendMsg.data[0], &pabData[lActIdx], lSendLen);

        // Process message
        if ( (sRet = TransferMessage( usDevNumber,
                                      &tSendMsg,
                                      &tRecvMsg,
                                      TO_CONT_MSG)) != DRV_NO_ERROR ) {
          // Could not process this message
          fRet = FALSE;
        } else {
	//printf("(A)===========================flen = %ld, Sndlen = %ld, ActIdx = %ld\n", lFileLength, lSendLen, lActIdx);
          // No error, send next message
          lActIdx += lSendLen;                    // Add send size to actual index
          *pdwByte  = lActIdx;

          // Calculate next message length
          if ( lFileLength <= (lSendLen + lActIdx)) {
            lSendLen = lFileLength - lActIdx;   // Set length to rest of data
            dwState = RCS_LAST_MSK;
          } else {
            dwState = RCS_CONT_MSK;
          }
        }
        break;
      //--------------------------
      // Send last message
      //--------------------------
      case RCS_LAST_MSK:

        tSendMsg.ln = (unsigned char)lSendLen;
        tSendMsg.nr = ++tSendMsg.nr;
        tSendMsg.e  = RCS_LAST_MSK;

        memcpy ( &tSendMsg.data[0], &pabData[lActIdx] , lSendLen);

        // Process message
        sRet = TransferMessage( usDevNumber,
                                &tSendMsg,
                                &tRecvMsg,
                                TO_LAST_MSG);
        *pdwByte  = (lActIdx + lSendLen);
        fRet = FALSE;   // END after last message 
        break;
      default:
        // unkonwn, leave command
        sRet = DRV_DEV_FUNCTION_FAILED;
        fRet = FALSE;   
        break;
    }
  } while ( fRet == TRUE);
  return sRet;
}

/* <ST> =================================================================================
  Function: RunConfigDownload
            Download a configuration to a device
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - Device number
            pwFile        - File name including a path
            *pdwBytes     - Already loaded bytes
  Output  : -
  Return  : DRV_NO_ERROR  - Download successfully
  ================================================================================= <En> */
short RunConfigDownload( unsigned short usDevNumber,
                         FILEDATA       *ptFile,
                         unsigned long  *pdwByte)
{
  unsigned long   dwState;
  long            lSendLen, lActIdx, lOffset, lFileLength;
  unsigned char   *pabData;
  MSG_STRUC       tSendMsg, tRecvMsg;
  BOOL            fRet;
  short           sRet = DRV_NO_ERROR;
  unsigned char   ab1StMsgBuf1[16];
  unsigned char   ab1StMsgBuf2[60];

  // Load pointer to data
  pabData = (unsigned char*)ptFile->pabBuffer; 

  // Clear messages
  memset( &tSendMsg, 0, sizeof(MSG_STRUC));
  memset( &tRecvMsg, 0, sizeof(MSG_STRUC));

  // Craete 1st data buffer from DBM file
  memcpy( &ab1StMsgBuf1[0], &pabData[44], 16);

  // Craete 2nd data buffer from DBM file
  lOffset = (*(long*)(&pabData[40])) + ((*(short*)(&pabData[60])) * 2)  + 14 ;
  lOffset = (*(long*)(&pabData[40])) + ((*(short*)(&pabData[lOffset])) *2) + 12 ;
  memcpy( &ab1StMsgBuf2[0], &pabData[lOffset], sizeof(ab1StMsgBuf2));

  // Set data length
  lFileLength = ptFile->lSize;
  // Set maximum message length
  if ( lFileLength > MAX_DOWNLOAD_MSG_LENGTH) lSendLen = MAX_DOWNLOAD_MSG_LENGTH;
  else lSendLen = lFileLength;

  lActIdx  = 0;
  *pdwByte = lActIdx;

  // Set program state
  dwState = RCS_FIRST_MSK;
  fRet = TRUE;
  do {
    switch ( dwState) {
      //--------------------------
      // Send first message
      //--------------------------
      case RCS_FIRST_MSK:       // Send first message
        // Set HOST_READY for download, maybe it is disabled
        sRet =  DevSetHostState( usDevNumber, HOST_READY, 0L);                 
        // Insert data into message
        tSendMsg.rx = RCS_TASK;                       // Receiver task number
        tSendMsg.tx = MSG_SYSTEM_TX;                  // Transmitter task number
        tSendMsg.ln = 51;                             // Message length first message
        tSendMsg.nr = 1;                              // Message number 
        tSendMsg.a  = 0;                              // Answer byte cleared
        tSendMsg.f  = 0;                              // Error byte cleared
        tSendMsg.b  = RCS_B_LOADFKT;                  // Function code
        tSendMsg.e  = RCS_FIRST_MSK;                  // First message
        
        tSendMsg.data[0]  = MODE_DOWNLOAD_DBM;         // Download a configuration file
        // Copy buffer 1st to message
        memcpy ( &tSendMsg.data[1], &ab1StMsgBuf1[0], sizeof(ab1StMsgBuf1));
        // Copy buffer 2nd to message, 34 Byte
        memcpy ( &tSendMsg.data[17], &ab1StMsgBuf2[0], 34);

	//printf("cifAPI: %ld\n", dwState);
        // Process message
        if ( (sRet = TransferMessage( usDevNumber,
                                      &tSendMsg,
                                      &tRecvMsg,
                                      TO_1ST_MSG)) != DRV_NO_ERROR ) {
          // Could not process this message
          fRet = FALSE;
        } else {
          // Set Index for next message, start at offset 44
          lActIdx = 44;
          *pdwByte = lActIdx;
          // Message send, goto next message
          *pdwByte = lFileLength;
          dwState = RCS_CONT_MSK;
        }
        break;
      //--------------------------
      // Send continue message
      //--------------------------
      case RCS_CONT_MSK:    
	//printf("cifAPI: (case RCS_CONT_MSK:) %ld\n", lActIdx);
        // Insert data into message
        tSendMsg.rx = RCS_TASK;
        tSendMsg.tx = MSG_SYSTEM_TX;
        tSendMsg.ln = (unsigned char)lSendLen;
        tSendMsg.nr = ++tSendMsg.nr;
        tSendMsg.a  = 0;
        tSendMsg.f  = 0;
        tSendMsg.b  = RCS_B_LOADFKT;
        tSendMsg.e  = RCS_CONT_MSK;

        memcpy ( &tSendMsg.data[0], &pabData[lActIdx] , lSendLen);

        // Process message
        if ( (sRet = TransferMessage( usDevNumber,
                                      &tSendMsg,
                                      &tRecvMsg,
                                      TO_CONT_MSG)) != DRV_NO_ERROR ) {
          // Could not process this message
          fRet = FALSE;
        } else {
          // No error, send next message
          lActIdx += lSendLen;                        // Add send size to actual index
          *pdwByte = lActIdx;
          // Calculate next message length
          if ( lFileLength <= (lSendLen + lActIdx)) {
            lSendLen = lFileLength - lActIdx;         // Set length to rest of data
            dwState = RCS_LAST_MSK;                   // Set to last mask
          } else {
            // Continue message
            dwState = RCS_CONT_MSK;
          }
        }
        break;
      //--------------------------
      // Send last message
      //--------------------------
      case RCS_LAST_MSK:
        // Insert message data
        tSendMsg.rx = RCS_TASK;
        tSendMsg.tx = MSG_SYSTEM_TX;
        tSendMsg.ln = (unsigned char)lSendLen;
        tSendMsg.nr = ++tSendMsg.nr;
        tSendMsg.a  = 0;
        tSendMsg.f  = 0;
        tSendMsg.b  = RCS_B_LOADFKT;
        tSendMsg.e  = RCS_LAST_MSK;

        memcpy ( &tSendMsg.data[0], &pabData[lActIdx] , lSendLen);

        // Process message
        sRet = TransferMessage( usDevNumber,
                                &tSendMsg,
                                &tRecvMsg,
                                TO_LAST_MSG);
        *pdwByte = (lActIdx + lSendLen);
        fRet = FALSE;   // END after last message 
        break;
      default:
        // unkonwn, leave command
        sRet = DRV_DEV_FUNCTION_FAILED;
        fRet = FALSE;
        break;
    }
  } while (fRet == TRUE);
  return sRet;
}

/* <ST> =================================================================================
  Function: FirmwareDownload
            Download a firmware file to a device
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - Device number
            pwFile        - File name including a path
            *pdwBytes     - Already loaded bytes
  Output  : -
  Return  : DRV_NO_ERROR  - Download successfully
  ================================================================================= <En> */
short FirmwareDownload  ( unsigned short  usDevNumber,
                          const char      *fileName,
                          unsigned long  *pdwByte)
{
  short     sRet = DRV_NO_ERROR;
  FILEDATA  tFileData;

  // Read Device Information
  if ( (sRet = ReadDeviceInformation( usDevNumber, fileName)) == DRV_NO_ERROR) {
    // Firmware name is OK
    if ( (sRet = openFile ( fileName, &tFileData)) == DRV_NO_ERROR) {
      // Firmware file opened and loaded into RAM
      sRet = RunFirmwareDownload( usDevNumber, &tFileData, pdwByte);
      closeFile ( &tFileData);
    }
  }
  return sRet;
}

/* <ST> =================================================================================
  Function: ConfigDownload
            Download a configuration file created by SyCon to a device
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber   - Device number
            pwFile        - File name including a path
            *pdwBytes     - Already loaded bytes
  Output  : -
  Return  : DRV_NO_ERROR  - Download successfully
  ================================================================================= <En> */
short ConfigDownload(  unsigned short  usDevNumber,
                       const char      *fileName,
                       unsigned long   *pdwByte)
{
  unsigned short sRet = DRV_NO_ERROR;
  FILEDATA       tFileData;

  if ( (sRet = openFile ( fileName, &tFileData)) == DRV_NO_ERROR) {
    // Configuration file opened and loaded into RAM
    sRet = RunConfigDownload( usDevNumber, &tFileData, pdwByte);
    closeFile ( &tFileData);
  }
  return sRet;
}

/* <ST> =================================================================================
  Function: DevDownload
            Download a firmware or configuration file
  ---------------------------------------------------------------------------------------
  Input   : usDevNumber     - number of the DEV board       (0..3)
            usMode          - 1 = FIRMWARE_DOWNLOAD, 2 = CONFIGURATION_DOWNLOAD
            *pdwBytes       - Amount of bytes sended
  Output  : -
  Return  : DRV_NO_ERROR     - function successfull
            != DRV_NO_ERROR  - function failed
  ================================================================================= <En> */

short  DevDownload ( unsigned short usDevNumber,
		     unsigned short usMode,
		     unsigned char  *pszFileName,
		     long           *pdwBytes)
{
short sRet = DRV_NO_ERROR;

  // valid handle available, driver is open
  if ( hDevDrv == INVALID_HANDLE_VALUE) {
    sRet = DRV_USR_NOT_INITIALIZED;
  } else if ( usDevNumber >= MAX_DEV_BOARDS) {
    sRet = DRV_USR_DEV_NUMBER_INVALID;
  } else if ( (pdwBytes     == NULL) ||
              (pszFileName  == NULL) ||
              (pdwBytes     == NULL)  ) {
    sRet = DRV_USR_BUF_PTR_NULL;
  } else if ( strlen( (const char*)pszFileName) == 0) {
    sRet = DRV_USR_FILENAME_INVALID;
  } else {
    switch ( usMode) {
    case  FIRMWARE_DOWNLOAD:
      sRet = FirmwareDownload( usDevNumber,
                               (const char*)pszFileName,
                               pdwBytes);
      break;
    case  CONFIGURATION_DOWNLOAD:
      sRet = ConfigDownload(  usDevNumber,
                              (const char*)pszFileName,
                              pdwBytes);
      break;
    default:
        sRet = DRV_USR_MODE_INVALID;
    } // end switch
  }
  return sRet;
}
