/* <St> *******************************************************************

  cif_user.h

  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION

    User interface definition.

  =========================================================================

  CHANGES

  version name        date        Discription

  		   Juli 2004      Copyright changed to GNU Lesser GPL
-------------------------------------------------------------------------
  V2.000

  NOTE: The Code from the Windows version of the driver related to 
        driver API was taken over for the most part unchanged

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

/* prevent multiple inclusion */
#ifndef CIFUSER_H_INCLUDED
  #define CIFUSER_H_INCLUDED

#ifdef __cplusplus
  extern "C" {
#endif  /* _cplusplus */

#ifndef	_GNUC_
#    define  _GNUC_
#endif	/* _GNUC_ */

/* ------------------------------------------------------------------------------------ */
/*  global definitions                                                                  */
/* ------------------------------------------------------------------------------------ */

#define MAX_DEV_BOARDS              4       // maximum numbers of boards

/* ------------------------------------------------------------------------------------ */
/*  driver errors                                                                       */
/* ------------------------------------------------------------------------------------ */

#define DRV_NO_ERROR                 0      // no error                                            
#define DRV_BOARD_NOT_INITIALIZED   -1      // DRIVER Board not initialized                        
#define DRV_INIT_STATE_ERROR        -2      // DRIVER Error in internal init state                 
#define DRV_READ_STATE_ERROR        -3      // DRIVER Error in internal read state                 
#define DRV_CMD_ACTIVE              -4      // DRIVER Command on this channel is activ             
#define DRV_PARAMETER_UNKNOWN       -5      // DRIVER Unknown parameter in function occured        
#define DRV_WRONG_DRIVER_VERSION    -6      // DRIVER Version is incompatible with DLL             
                                               
#define DRV_PCI_SET_CONFIG_MODE     -7      // DRIVER Error during PCI set run mode                
#define DRV_PCI_READ_DPM_LENGTH     -8      // DRIVER Could not read PCI dual port memory length   
#define DRV_PCI_SET_RUN_MODE        -9      // DRIVER Error during PCI set run mode                                                                  
                                            
#define DRV_DEV_DPM_ACCESS_ERROR    -10     // DEVICE Dual port ram not accessable(board not found)
#define DRV_DEV_NOT_READY           -11     // DEVICE Not ready (ready flag failed)                
#define DRV_DEV_NOT_RUNNING         -12     // DEVICE Not running (running flag failed)            
#define DRV_DEV_WATCHDOG_FAILED     -13     // DEVICE Watchdog test failed                         
#define DRV_DEV_OS_VERSION_ERROR    -14     // DEVICE Signals wrong OS version                     
#define DRV_DEV_SYSERR              -15     // DEVICE Error in dual port flags                     
#define DRV_DEV_MAILBOX_FULL        -16     // DEVICE Send mailbox is full                         
#define DRV_DEV_PUT_TIMEOUT         -17     // DEVICE PutMessage timeout                           
#define DRV_DEV_GET_TIMEOUT         -18     // DEVICE GetMessage timeout                           
#define DRV_DEV_GET_NO_MESSAGE      -19     // DEVICE No message available                         
#define DRV_DEV_RESET_TIMEOUT       -20     // DEVICE RESET command timeout                        
#define DRV_DEV_NO_COM_FLAG         -21     // DEVICE COM-flag not set                             
#define DRV_DEV_EXCHANGE_FAILED     -22     // DEVICE IO data exchange failed                      
#define DRV_DEV_EXCHANGE_TIMEOUT    -23     // DEVICE IO data exchange timeout                     
#define DRV_DEV_COM_MODE_UNKNOWN    -24     // DEVICE IO data mode unknown                         
#define DRV_DEV_FUNCTION_FAILED     -25     // DEVICE Function call failed                         
#define DRV_DEV_DPMSIZE_MISMATCH    -26     // DEVICE DPM size differs from configuration          
#define DRV_DEV_STATE_MODE_UNKNOWN  -27     // DEVICE State mode unknown

// Error from Interface functions
#define DRV_USR_OPEN_ERROR          -30     // USER Driver not opened (device driver not loaded)   
#define DRV_USR_INIT_DRV_ERROR      -31     // USER Can't connect with device                      
#define DRV_USR_NOT_INITIALIZED     -32     // USER Board not initialized (DevInitBoard not called)
#define DRV_USR_COMM_ERR            -33     // USER IOCTRL function failed                         
#define DRV_USR_DEV_NUMBER_INVALID  -34     // USER Parameter DeviceNumber  invalid                
#define DRV_USR_INFO_AREA_INVALID   -35     // USER Parameter InfoArea unknown                     
#define DRV_USR_NUMBER_INVALID      -36     // USER Parameter Number invalid                       
#define DRV_USR_MODE_INVALID        -37     // USER Parameter Mode invalid                         
#define DRV_USR_MSG_BUF_NULL_PTR    -38     // USER NULL pointer assignment                        
#define DRV_USR_MSG_BUF_TOO_SHORT   -39     // USER Message buffer too short                       
#define DRV_USR_SIZE_INVALID        -40     // USER Parameter Size invalid                         
#define DRV_USR_SIZE_ZERO           -42     // USER Parameter Size with zero length               
#define DRV_USR_SIZE_TOO_LONG       -43     // USER Parameter Size too long                        
#define DRV_USR_DEV_PTR_NULL        -44     // USER Device address null pointer                    
#define DRV_USR_BUF_PTR_NULL        -45     // USER Pointer to buffer is a null pointer            
                                                
#define DRV_USR_SENDSIZE_TOO_LONG   -46     // USER Parameter SendSize too long                    
#define DRV_USR_RECVSIZE_TOO_LONG   -47     // USER Parameter ReceiveSize too long                 
#define DRV_USR_SENDBUF_PTR_NULL    -48     // USER Pointer to send buffer is a null pointer       
#define DRV_USR_RECVBUF_PTR_NULL    -49     // USER Pointer to receive buffer is a null pointer 

#define DRV_DEV_NO_VIRTUAL_MEM      -60     // DEVICE Virtual memory not available
#define DRV_DEV_UNMAP_VIRTUAL_MEM   -61     // DEVICE Unmap virtual memory failed
#define DRV_DEV_REQUEST_IRQ_FAILED  -62     // DEVICE Request irq failed

#define DRV_USR_FILE_OPEN_FAILED    -100    // USER file not opend
#define DRV_USR_FILE_SIZE_ZERO      -101    // USER file size zero
#define DRV_USR_FILE_NO_MEMORY      -102    // USER not enough memory to load file
#define DRV_USR_FILE_READ_FAILED    -103    // USER file read failed
#define DRV_USR_INVALID_FILETYPE    -104    // USER file type invalid
#define DRV_USR_FILENAME_INVALID    -105    // USER file name not valid

#define DRV_RCS_ERROR_OFFSET       1000     // RCS error number start


#ifdef		_GNUC_
#  ifndef	PACKED
#    define	PACKED	__attribute__((packed, aligned(1)))
#  endif	/* PACKED */
#else
#  define	PACKED
#endif

/* ------------------------------------------------------------------------------------ */
/*  message definition                                                                  */
/* ------------------------------------------------------------------------------------ */

// max. length is 288 Bytes, max message length is 255 + 8 Bytes
typedef struct tagMSG_STRUC {
  unsigned char   rx        PACKED;
  unsigned char   tx        PACKED;
  unsigned char   ln        PACKED;
  unsigned char   nr        PACKED;
  unsigned char   a         PACKED;
  unsigned char   f         PACKED;
  unsigned char   b         PACKED;
  unsigned char   e         PACKED;
  unsigned char   data[255] PACKED;
  unsigned char   dummy[25] PACKED;      // for compatibility with older definitions (288 Bytes)
} MSG_STRUC;

/* ------------------------------------------------------------------------------------ */
/*  INFO structure definitions                                                          */
/* ------------------------------------------------------------------------------------ */

// Board operation mode
#define POLLING_MODE        0
#define INTERRUPT_MODE      1

// DEVRESET
#define COLDSTART           2
#define WARMSTART           3
#define BOOTSTART           4

// DEVMBXINFO
#define DEVICE_MBX_EMPTY    0
#define DEVICE_MBX_FULL     1
#define HOST_MBX_EMPTY      0
#define HOST_MBX_FULL       1

// TRIGGERWATCHDOG
#define WATCHDOG_STOP       0
#define WATCHDOG_START      1

// GETINFO InfoArea definitions
#define GET_DRIVER_INFO     1
#define GET_VERSION_INFO    2
#define GET_FIRMWARE_INFO   3
#define GET_TASK_INFO       4
#define GET_RCS_INFO        5
#define GET_DEV_INFO        6
#define GET_IO_INFO         7
#define GET_IO_SEND_DATA    8

// HOST mode definition
#define HOST_NOT_READY      0
#define HOST_READY          1

// DEVREADWRITERAW
#define PARAMETER_READ      1
#define PARAMETER_WRITE     2

// STATE definition
#define STATE_ERR_NON       0
#define STATE_ERR           1

#define STATE_MODE_2        2
#define STATE_MODE_3        3
#define STATE_MODE_4        4

// DEVSPECIALCONTROL
#define SPECIAL_CONTROL_CLEAR   0
#define SPECIAL_CONTROL_SET     1

// DEVDOWNLOAD
#define FIRMWARE_DOWNLOAD       1
#define CONFIGURATION_DOWNLOAD  2

// Device exchange IO information
typedef struct tagIOINFO {
  unsigned char   bComBit         PACKED;      /* Actual state of the COM bit                */
  unsigned char   bIOExchangeMode PACKED;      /* Actual data exchange mode (0..5)           */
  unsigned long   ulIOExchangeCnt PACKED;      /* Exchange IO counter                        */
} IOINFO;

// Device version information
typedef struct tagVERSIONINFO {                /* DEV serial number and OS versions          */
  unsigned long   ulDate             PACKED;
  unsigned long   ulDeviceNo         PACKED;
  unsigned long   ulSerialNo         PACKED;
  unsigned long   ulReserved         PACKED;
  unsigned char   abPcOsName0[4]     PACKED;
  unsigned char   abPcOsName1[4]     PACKED;
  unsigned char   abPcOsName2[4]     PACKED;
  unsigned char   abOemIdentifier[4] PACKED;
} VERSIONINFO;

// Device firmware information
typedef struct tagFIRMWAREINFO {
  unsigned char   abFirmwareName[16]    PACKED;  /* Firmware name                              */
  unsigned char   abFirmwareVersion[16] PACKED;  /* Firmware version                           */
} FIRMWAREINFO;

// Device task state information
typedef struct tagTASKSTATE {
  unsigned char   abTaskState[64] PACKED;        /* Task state field                           */
} TASKSTATE;

// Device task paramater data
typedef struct tagTASKPARAM {
  unsigned char   abTaskParameter[64] PACKED;    /* Task parameter field                       */
} TASKPARAM;

// Device raw data structure
typedef struct tagRAWDATA {
  unsigned char   abRawData[1022] PACKED;        /* Definition of the last kByte               */
} RAWDATA;

// Device task information
typedef struct tagTASKINFO {
  struct  {
    unsigned char  abTaskName[8]  PACKED;        /* Task name                                  */
    unsigned short usTaskVersion  PACKED;        /* Task version                               */
    unsigned char  bTaskCondition PACKED;        /* Actual task condition                      */
    unsigned char  abreserved[5]  PACKED;        /* n.c.                                       */
  } tTaskInfo [7];
} TASKINFO;

// Device operating system (RCS) information
typedef struct tagRCSINFO {
  unsigned short usRcsVersion  PACKED;           /* Device operating system (RCS) version      */
  unsigned char  bRcsError     PACKED;           /* Operating system errors                    */
  unsigned char  bHostWatchDog PACKED;           /* Host watchdog value                        */
  unsigned char  bDevWatchDog  PACKED;           /* Device watchdog value                      */
  unsigned char  bSegmentCount PACKED;           /* RCS segment free counter                   */
  unsigned char  bDeviceAdress PACKED;           /* RCS device base address                    */
  unsigned char  bDriverType   PACKED;           /* RCS driver type                            */
} RCSINFO;

// Device description
typedef struct tagDEVINFO {
  unsigned char  bDpmSize           PACKED;      /* Device dpm size (2,8...)                   */
  unsigned char  bDevType           PACKED;      /* Device type  (manufactor code)             */
  unsigned char  bDevModel          PACKED;      /* Device model (manufactor code)             */
  unsigned char  abDevIdentifier[3] PACKED;      /* Device identification characters           */
} DEVINFO;

/* ------------------------------------------------------------------------------------ */
/*  driver info structure definitions                                                   */
/* ------------------------------------------------------------------------------------ */

// Board information structure
typedef struct tagBOARD_INFO{
  unsigned char abDriverVersion[16]  PACKED;     // DRV driver information string
  struct {
    unsigned short usBoard           PACKED;     // DRV board number
    unsigned short usAvailable       PACKED;     // DRV board is available
    unsigned long  ulPhysicalAddress PACKED;     // DRV physical DPM address
    unsigned short usIrq             PACKED;     // DRV irq number
  } tBoard [MAX_DEV_BOARDS];
  unsigned short   usBoards_detected PACKED;
} BOARD_INFO;

// Internal driver state information structure
typedef struct tagDRIVERINFO{
  unsigned long ulOpenCnt     PACKED;            // DevOpen() counter
  unsigned long CloseCnt      PACKED;            // number of driver close
  unsigned long ulReadCnt     PACKED;            // Number of DevGetMessage commands
  unsigned long ulWriteCnt    PACKED;            // Number of DevPutMessage commands
  unsigned long ulIRQCnt      PACKED;            // Number of board interrupts
  unsigned char bInitMsgFlag  PACKED;            // Actual init sate
  unsigned char bReadMsgFlag  PACKED;            // Actual read mailbox state
  unsigned char bWriteMsgFlag PACKED;            // Actual write mailbox state
  unsigned char bLastFunction PACKED;            // Last driver function
  unsigned char bWriteState   PACKED;            // Actual write command state
  unsigned char bReadState    PACKED;            // Actual read command state
  unsigned char bHostFlags    PACKED;            // Actual host flags
  unsigned char bMyDevFlags   PACKED;            // Actual device falgs
  unsigned char bExIOFlag     PACKED;            // Actual IO flags
  unsigned long ulExIOCnt     PACKED;            // DevExchangeIO() counter
} DRIVERINFO;

// Extended board information structure
typedef struct tagBOARD_INFOEX{
  unsigned char     abDriverVersion[16] PACKED;  // DRV driver information string
  struct {
    unsigned short  usBoard           PACKED;    // DRV board number
    unsigned short  usAvailable       PACKED;    // DRV board is available
    unsigned long   ulPhysicalAddress PACKED;    // DRV physical DPM address
    unsigned short  usIrq             PACKED;    // DRV irq number
    DRIVERINFO      tDriverInfo       PACKED;    // Driver information
    FIRMWAREINFO    tFirmware         PACKED;
    DEVINFO         tDeviceInfo       PACKED;
    RCSINFO         tRcsInfo          PACKED;
    VERSIONINFO     tVersion          PACKED;
  } tBoard [MAX_DEV_BOARDS];
} BOARD_INFOEX;

// Communication state field structure
typedef struct tagCOMSTATE {
  unsigned short    usMode      PACKED;          // Actual STATE mode
  unsigned short    usStateFlag PACKED;          // State flag
  unsigned char     abState[64] PACKED;          // State area
} COMSTATE;

// state information in bLastFunction
#define  FKT_OPEN       1;
#define  FKT_CLOSE      2;
#define  FKT_READ       3;
#define  FKT_WRITE      4;
#define  FKT_IO         5;
// state information in bWriteState and bReadState
#define  STATE_IN       0x01;
#define  STATE_WAIT     0x02;
#define  STATE_OUT      0x03;
#define  STATE_IN_IRQ   0x04;

/* ------------------------------------------------------------------------------------ */
/*  funcion prototypes                                                                  */
/* ------------------------------------------------------------------------------------ */

extern short  DevOpenDriver        ( void);

extern short  DevCloseDriver       ( void);

extern short  DevGetBoardInfo      ( BOARD_INFO  *pvData);

extern short  DevInitBoard         ( unsigned short usDevNumber); 

extern short  DevExitBoard         ( unsigned short usDevNumber);

extern short  DevPutTaskParameter  ( unsigned short usDevNumber,
				     unsigned short usNumber,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevReset             ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned long  ulTimeout);

extern short  DevPutMessage        ( unsigned short usDevNumber,
				     MSG_STRUC      *ptMessage,
				     unsigned long  ulTimeout);

extern short  DevGetMessage        ( unsigned short usDevNumber,
				     unsigned short usSize,
				     MSG_STRUC      *ptMessage,
				     unsigned long  ulTimeout);

extern short  DevGetTaskState      ( unsigned short usDevNumber,
				     unsigned short usNumber,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevGetMBXState       ( unsigned short usDevNumber,
				     unsigned short *pusDevMBXState,
				     unsigned short *pusHostMBXState);

extern short  DevTriggerWatchDog   ( unsigned short usDevNumber,
				     unsigned short usFunction,
				     unsigned short *usDevWatchDog);

extern short  DevGetInfo           ( unsigned short usDevNumber,
				     unsigned short usFunction,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevGetTaskParameter  ( unsigned short usDevNumber,
				     unsigned short usNumber,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevExchangeIO        ( unsigned short usDevNumber,
				     unsigned short usSendOffset,
				     unsigned short usSendSize,
				     void           *pvSendData,
				     unsigned short usReceiveOffset,
				     unsigned short usReceiveSize,
				     void           *pvReceiveData,
				     unsigned long  ulTimeout);

extern short  DevReadSendData      ( unsigned short usDevNumber,
				     unsigned short usOffset,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevSetHostState      ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned long  ulTimeout);
  
extern short  DevExtendedData      ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevGetMBXData        ( unsigned short usDevNumber,
				     unsigned short usHostSize,
				     void           *pvHostData,
				     unsigned short usDevSize,
				     void           *pvDevData);

extern short  DevGetBoardInfoEx    ( void           *pvData);

extern short  DevExchangeIOEx      ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned short usSendOffset,
				     unsigned short usSendSize,
				     void           *pvSendData,
				     unsigned short usReceiveOffset,
				     unsigned short usReceiveSize,
				     void           *pvReceiveData,
				     unsigned long  ulTimeout);

extern short  DevExchangeIOErr     ( unsigned short usDevNumber,
				     unsigned short usSendOffset,
				     unsigned short usSendSize,
				     void           *pvSendData,
				     unsigned short usReceiveOffset,
				     unsigned short usReceiveSize,
				     void           *pvReceiveData,
				     COMSTATE       *ptState,
				     unsigned long  ulTimeout);

extern short  DevReadWriteDPMRaw   ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned short usOffset,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevSpecialControl    ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned short *pusCtrlAck);

extern short  DevDownload          ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned char  *pszFileName,
				     unsigned long          *pdwBytes);

extern short  DevReadWriteDPMData  ( unsigned short usDevNumber,
				     unsigned short usMode,
				     unsigned short usOffset,
				     unsigned short usSize,
				     void           *pvData);

extern short  DevSetOpMode         ( unsigned short usBoard,
				     unsigned short usMode ,
				     unsigned short *usIrq );

#ifdef __cplusplus
}
#endif

#endif // ifndef CIFUSER_H_INCLUDED
