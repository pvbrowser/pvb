/* <St> *******************************************************************
  cifdev_i.h

  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION

    Internal driver interface definition

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
#ifndef CIFDEV_I_H
# define CIFDEV_I_H

#ifdef _cplusplus
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
#define DRV_BOARD_NOT_INITIALIZED   -1      // DRIVER board not initialized
#define DRV_INIT_STATE_ERROR        -2      // DRIVER error in internal init state
#define DRV_READ_STATE_ERROR        -3      // DRIVER error in internal read state
#define DRV_CMD_ACTIVE              -4      // DRIVER command on this chanal is activ
#define DRV_PARAMETER_UNKNOWN       -5      // DRIVER unknown parameter in function occured
#define DRV_WRONG_DRIVER_VERSION    -6      // DRIVER driver version is incompatible with DLL

#define DRV_PCI_SET_CONFIG_MODE     -7      // DRIVER error during PCI set config mode
#define DRV_PCI_READ_DPM_LENGTH     -8      // DRIVER could not read PCI DPM length
#define DRV_PCI_SET_RUN_MODE        -9      // DRIVER error during PCI set run mode

#define DRV_DEV_DPM_ACCESS_ERROR    -10     // DEVICE dual port ram not accessable
#define DRV_DEV_NOT_READY           -11     // DEVICE not ready (ready flag failed)
#define DRV_DEV_NOT_RUNNING         -12     // DEVICE not running (running flag failed)
#define DRV_DEV_WATCHDOG_FAILED     -13     // DEVICE watch dog test failed
#define DRV_DEV_OS_VERSION_ERROR    -14     // DEVICE signals wrong OS version
#define DRV_DEV_SYSERR              -15     // DEVICE error in dual port flags
#define DRV_DEV_MAILBOX_FULL        -16     // DEVICE send mailbox is full
#define DRV_DEV_PUT_TIMEOUT         -17     // DEVICE PutMessage timeout
#define DRV_DEV_GET_TIMEOUT         -18     // DEVICE GetMessage timeout
#define DRV_DEV_GET_NO_MESSAGE      -19     // DEVICE no message available
#define DRV_DEV_RESET_TIMEOUT       -20     // DEVICE RESET command timeout
#define DRV_DEV_NO_COM_FLAG         -21     // DEVICE COM-flag not set
#define DRV_DEV_EXCHANGE_FAILED     -22     // DEVICE IO data exchange failed
#define DRV_DEV_EXCHANGE_TIMEOUT    -23     // DEVICE IO data exchange timeout
#define DRV_DEV_COM_MODE_UNKNOWN    -24     // DEVICE IO data mode unknown
#define DRV_DEV_FUNCTION_FAILED     -25     // DEVICE Function call failed
#define DRV_DEV_DPMSIZE_MISMATCH    -26     // DEVICE DPM size differs from configuration
#define DRV_DEV_STATE_MODE_UNKNOWN  -27     // DEVICE State mode unknown

// Error from Interface functions
#define DRV_USR_OPEN_ERROR          -30     // USER driver not opened
#define DRV_USR_INIT_DRV_ERROR      -31     // USER can't connect with DEV board
#define DRV_USR_NOT_INITIALIZED     -32     // USER board not initialized
#define DRV_USR_COMM_ERR            -33     // USER IOCTRL function faild
#define DRV_USR_DEV_NUMBER_INVALID  -34     // USER parameter for DEV number invalid
#define DRV_USR_INFO_AREA_INVALID   -35     // USER parameter InfoArea unknown
#define DRV_USR_NUMBER_INVALID      -36     // USER parameter Number invalid
#define DRV_USR_MODE_INVALID        -37     // USER parameter Mode invalid
#define DRV_USR_MSG_BUF_NULL_PTR    -38     // USER NULL pointer assignment
#define DRV_USR_MSG_BUF_TOO_SHORT   -39     // USER Messagebuffer too short
#define DRV_USR_SIZE_INVALID        -40     // USER size parameter invalid
#define DRV_USR_SIZE_ZERO           -42     // USER size parameter with zero length
#define DRV_USR_SIZE_TOO_LONG       -43     // USER size parameter too long
#define DRV_USR_DEV_PTR_NULL        -44     // USER device address null pointer
#define DRV_USR_BUF_PTR_NULL        -45     // USER pointer to buffer is a null pointer

#define DRV_USR_SENDSIZE_TOO_LONG   -46     // USER SendSize parameter too long
#define DRV_USR_RECVSIZE_TOO_LONG   -47     // USER ReceiveSize parameter too long
#define DRV_USR_SENDBUF_PTR_NULL    -48     // USER pointer to buffer is a null pointer
#define DRV_USR_RECVBUF_PTR_NULL    -49     // USER pointer to buffer is a null pointer

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


/* ------------------------------------------------------------------------------------ */
/*  message definition                                                                  */
/* ------------------------------------------------------------------------------------ */

#ifdef		_GNUC_
#  ifndef	PACKED
#    define	PACKED	__attribute__((packed, aligned(1)))
#  endif	/* PACKED */
#else
#  define	PACKED
#endif

// max. length is 255 + 8 Bytes
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
  unsigned char   dummy[25] PACKED;      
} MSG_STRUC;

/* ------------------------------------------------------------------------------------ */
/* DEV DPM DATA structures                                                              */
/* ------------------------------------------------------------------------------------ */

typedef struct tagIOINFO {
  unsigned char   bComBit          PACKED;
  unsigned char   bIOExchangeMode  PACKED;
  unsigned long   ulIOExchangeCnt  PACKED;
} IOINFO;

typedef struct tagVERSIONINFO {             /* DEV serial number and OS versions           */
  unsigned long   ulDate PACKED    PACKED;
  unsigned long   ulDeviceNo       PACKED;
  unsigned long   ulSerialNo       PACKED;
  unsigned long   ulReserved       PACKED;
  unsigned char   PcOsName0[4]     PACKED;
  unsigned char   PcOsName1[4]     PACKED;
  unsigned char   PcOsName2[4]     PACKED;
  unsigned char   OemIdentifier[4] PACKED;
} VERSIONINFO;

typedef struct tagFIRMWAREINFO {
  unsigned char   FirmwareName[16]     PACKED;
  unsigned char   FirmwareVersion[16]  PACKED;
} FIRMWAREINFO;

typedef struct tagTASKSTATE {
  unsigned char   TaskState[64]    PACKED;
} TASKSTATE;

typedef struct tagTASKPARAM {
  unsigned char   TaskParameter[64] PACKED;
} TASKPARAM;

typedef struct tagRAWDATA {
  unsigned char   abRawData[1022] PACKED;
} RAWDATA;

typedef struct tagTASKINFO {
  struct {
    char           TaskName[8]   PACKED;          /* Task name            */
    unsigned short Version       PACKED;          /* Task version         */
    unsigned char  TaskCondition PACKED;
    unsigned char  reserved[5]   PACKED;          /* n.c.                 */
  } tInfo [7];
} TASKINFO;

typedef struct tagRCSINFO {
  unsigned short   RcsVersion   PACKED;           /* Operationsystem Version */
  unsigned char    RcsError     PACKED;
  unsigned char    HostWatchDog PACKED;
  unsigned char    DevWatchDog  PACKED;
  unsigned char    SegmentCount PACKED;
  unsigned char    DeviceAdress PACKED;
  unsigned char    DriverType   PACKED;
} RCSINFO;

typedef struct tagDEVINFO {
  unsigned char    DpmSize          PACKED;
  unsigned char    DevType          PACKED;
  unsigned char    DevModel         PACKED;
  unsigned char    DevIdentifier[3] PACKED;
} DEVINFO;

/* ------------------------------------------------------------------------------------ */
/*  driver info structure definitions                                                   */
/* ------------------------------------------------------------------------------------ */

// Board information structure
typedef struct tagBOARD_INFO{
  unsigned char   abDriverVersion[16] PACKED; // DRV driver information string
  struct {
    unsigned short  usBoard           PACKED; // DRV board number
    unsigned short  usAvailable       PACKED; // DRV board is available
    unsigned long   ulPhysicalAddress PACKED; // DRV physical DPM address
    unsigned short  usIrq             PACKED; // DRV irq number
  } tBoard [MAX_DEV_BOARDS];
  unsigned short    usBoards_detected PACKED;
} BOARD_INFO;

// Driver information structure
typedef struct tagDRIVERINFO{
  unsigned long      OpenCnt      PACKED;     // number of driver open
  unsigned long      CloseCnt     PACKED;     // number of driver close
  unsigned long      ReadCnt      PACKED;     // number of DevGetMessage commands
  unsigned long      WriteCnt     PACKED;     // number of DevPutMessage commands
  unsigned long      IRQCnt       PACKED;     // number of IRQs
  unsigned char      InitMsgFlag  PACKED;     // DPM state init
  unsigned char      ReadMsgFlag  PACKED;     // DPM state read  message
  unsigned char      WriteMsgFlag PACKED;     // DPM state write message
  unsigned char      LastFunction PACKED;     // DRV last function in driver
  unsigned char      WriteState   PACKED;     // DRV actual write state
  unsigned char      ReadState    PACKED;     // DRV actual read state
  unsigned char      HostFlags    PACKED;     // DPM HostFlags (PcFlags)
  unsigned char      MyDevFlags   PACKED;     // DPM (internal) DevFlags
  unsigned char      ExComBit     PACKED;     // COM bit
  unsigned long      ExIOCnt      PACKED;     // IO data exchange count
} DRIVERINFO;

// Extended board information structure
typedef struct tagBOARD_INFOEX{
  unsigned char       abDriverVersion[16] PACKED;   // DRV driver information string
  struct {
    unsigned short    usBoard             PACKED;   // DRV board number
    unsigned short    usAvailable         PACKED;   // DRV board is available
    unsigned long     ulPhysicalAddress   PACKED;   // DRV physical DPM address
    unsigned short    usIrq               PACKED;   // DRV irq number
    DRIVERINFO        tDriverInfo         PACKED;   // Driver information
    FIRMWAREINFO      tFirmware           PACKED;
    DEVINFO           tDeviceInfo         PACKED;
    RCSINFO           tRcsInfo            PACKED;
    VERSIONINFO       tVersion            PACKED;
  } tBoard [MAX_DEV_BOARDS];
} BOARD_INFOEX;

// State field structure
typedef struct tagCOMSTATE {
  unsigned short      usMode      PACKED;       // Actual STATE mode
  unsigned short      usStateFlag PACKED;       // State flag
  unsigned char       abState[64] PACKED;       // State area
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
/*  IOCTRL data structures                                                              */
/* ------------------------------------------------------------------------------------ */

// IOCTRL funktion defines, always step 8
/*
 * Ioctl definitions
 */

/* Use 'c' as magic number */
#define CIF_IOC_MAGIC         'c'

#define CIF_IOCRESET          _IO(CIF_IOC_MAGIC, 0)

#define CIF_IOCTLNOFUNCTION   _IO  (CIF_IOC_MAGIC, 0)
#define CIF_IOCTLBOARDINFO    _IOWR(CIF_IOC_MAGIC, 1, 256)
#define CIF_IOCTLINITDRV      _IOWR(CIF_IOC_MAGIC, 2, 13)
#define CIF_IOCTLPARAMETER    _IOW (CIF_IOC_MAGIC, 3, 71)
#define CIF_IOCTLRESETDEV     _IOW (CIF_IOC_MAGIC, 4, 13)
#define CIF_IOCTLPUTMSG       _IO  (CIF_IOC_MAGIC, 5)
#define CIF_IOCTLGETMSG       _IO  (CIF_IOC_MAGIC, 6)
#define CIF_IOCTLTASKSTATE    _IO  (CIF_IOC_MAGIC, 7)
#define CIF_IOCTLMBXINFO      _IO  (CIF_IOC_MAGIC, 8)
#define CIF_IOCTLTRIGGERWD    _IO  (CIF_IOC_MAGIC, 9)
#define CIF_IOCTLGETINFO      _IO  (CIF_IOC_MAGIC, 10)
#define CIF_IOCTLEXITDRV      _IO  (CIF_IOC_MAGIC, 11)
#define CIF_IOCTLGETPARAMETER _IO  (CIF_IOC_MAGIC, 12)
#define CIF_IOCTLEXIO         _IO  (CIF_IOC_MAGIC, 13)
#define CIF_IOCTLSETHOST      _IO  (CIF_IOC_MAGIC, 14)
#define CIF_IOCTLREADSEND     _IO  (CIF_IOC_MAGIC, 15)
#define CIF_IOCTLEXTDATA      _IO  (CIF_IOC_MAGIC, 16)
#define CIF_IOCTLGETMBX       _IO  (CIF_IOC_MAGIC, 17)
#define CIF_IOCTLBOARDINFOEX  _IO  (CIF_IOC_MAGIC, 18)

#define CIF_IOCTLEXIOEX       _IO  (CIF_IOC_MAGIC, 19)
#define CIF_IOCTLEXIOERR      _IO  (CIF_IOC_MAGIC, 20)
#define CIF_IOCTLRWRAW        _IO  (CIF_IOC_MAGIC, 21)
#define CIF_IOCTLSPCONTROL    _IO  (CIF_IOC_MAGIC, 22)
#define CIF_IOCTLGETDPMPTR    _IO  (CIF_IOC_MAGIC, 23)
#define CIF_IOCTLRWDPMDATA    _IO  (CIF_IOC_MAGIC, 24)

#define CIF_IOCTL_IRQ_POLL    _IOWR(CIF_IOC_MAGIC, 25, 13)

#define CIF_IOC_MAXNR   25

// interface structure for ioctl's
typedef struct cif_ioctl_data {
  unsigned long   lInpBuffLen PACKED;
  unsigned long   lOutBuffLen PACKED;
  unsigned char  *pInpBuff    PACKED;
  unsigned char  *pOutBuff    PACKED;
} cif_ioctl_data;

// SET BOARD OPERATION MODE
typedef struct tagDEVIO_SETOPMODE {
  unsigned short  usBoard PACKED; 
  unsigned short  usMode  PACKED;               // Interrupt/polling mode
  unsigned short  usIrq   PACKED;
  short           sError  PACKED;
} DEVIO_SETOPMODE;

// GETBOARDINFORMATION
typedef struct tagDEVIO_GETBOARDINFOCMD {
  unsigned short   usDevNumber  PACKED;         // n.a.
  unsigned short   usInfoLen    PACKED;         // Information length
  BOARD_INFO      *ptBoardInfo  PACKED;
  short            sError       PACKED;
} DEVIO_GETBOARDINFOCMD;

// Extnded GETBOARDINFORMATION
typedef struct tagDEVIO_GETBOARDINFOEXCMD {
  unsigned short  usDevNumber PACKED;           // n.a.
  unsigned short  usInfoLen   PACKED;           // Information length
  BOARD_INFOEX  * ptBoard     PACKED;
  short           sError      PACKED;
} DEVIO_GETBOARDINFOEXCMD;

// RESETDEV

#define COLDSTART 2
#define WARMSTART 3
#define BOOTSTART 4

typedef struct tagDEVIO_RESETCMD {
  unsigned short   usBoard   PACKED;            // DEV board number
  unsigned short   usMode    PACKED;            // DEV function
  unsigned long    ulTimeout PACKED;            // Service timeout
  unsigned long    ulDpmSize PACKED;
  short            sError    PACKED;
} DEVIO_RESETCMD;

// PUTTASKPARAMETER
typedef struct tagDEVIO_PUTPARAMETERCMD {
  unsigned short   usBoard           PACKED;    // DEV board number
  unsigned short   usTaskParamNum    PACKED;    // Number of the parameter area
  unsigned short   usTaskParamLen    PACKED;    // Lenght of parameter data
  unsigned char    TaskParameter[64] PACKED;
  short            sError            PACKED;
} DEVIO_PUTPARAMETERCMD;

// PUTMESSAGE
typedef struct tagDEVIO_PUTMESSAGECMD {
  unsigned short  usBoard   PACKED;             // DEV board number
  MSG_STRUC       tMsg      PACKED;             // Message data
  unsigned long   ulTimeout PACKED;             // Service timeout
  short           sError    PACKED;
} DEVIO_PUTMESSAGECMD;

// GETMESSAGE
typedef struct tagDEVIO_GETMESSAGECMD {
  unsigned short  usBoard   PACKED;             // DEV board number
  unsigned long   ulTimeout PACKED;             // Service timeout
  unsigned long   ulMsgSize PACKED;             // User message buffer size
  MSG_STRUC       tMsg      PACKED;             // Message data
  short           sError    PACKED;
} DEVIO_GETMESSAGECMD;

// GETTASKSTATE
typedef struct tagDEVIO_GETTASKSTATECMD {
  unsigned char   ucBoard       PACKED;         // DEV board number
  unsigned short  usStateNum    PACKED;         // Task state field number
  unsigned short  usStateLen    PACKED;         // Lenght of state data
  unsigned char   TaskState[64] PACKED;
  short           sError        PACKED;
} DEVIO_GETTASKSTATECMD;

// DEVMBXINFO

#define DEVICE_MBX_EMPTY    0
#define DEVICE_MBX_FULL     1
#define HOST_MBX_EMPTY      0
#define HOST_MBX_FULL       1
#define HOST_MBX_SYSERR     2

typedef struct tagDEVIO_MBXINFOCMD {
  unsigned char   ucBoard        PACKED;        // DEV board number
  unsigned short  usDevMbxState  PACKED;        // State of the device mailbox
  unsigned short  usHostMbxState PACKED;        // State of the host mailbox
  short           sError         PACKED;
} DEVIO_MBXINFOCMD;

// Board operation mode
#define POLLING_MODE        0
#define INTERRUPT_MODE      1

// TRIGGERWATCHDOG  and SETHOSTSTATE
#define WATCHDOG_STOP          0
#define WATCHDOG_START         1
#define HOST_NOT_READY         0
#define HOST_READY             1
#define SPECIAL_CONTROL_CLEAR  0
#define SPECIAL_CONTROL_SET    1

typedef struct tagDEVIO_TRIGGERCMD {
  unsigned short  usBoard        PACKED;        // DEV board number
  unsigned short  usMode         PACKED;        // DEV function
  unsigned long   ulTimeout      PACKED;        // DEV timeout
  unsigned short  usTriggerValue PACKED;        // DEV trigger value
  short           sError         PACKED;
} DEVIO_TRIGGERCMD;

// GETINFO
// InfoArea definitions

#define GET_DRIVER_INFO     1
#define GET_VERSION_INFO    2
#define GET_FIRMWARE_INFO   3
#define GET_TASK_INFO       4
#define GET_RCS_INFO        5
#define GET_DEV_INFO        6
#define GET_IO_INFO         7
#define GET_IO_SEND_DATA    8

typedef struct tagDEVIO_GETDEVINFOCMD {
  unsigned short     usBoard     PACKED;        // DEV board number
  unsigned short     usInfoArea  PACKED;        // Number of info area
  unsigned short     usInfoLen   PACKED;        // Lenght of info data
  unsigned char   *  pabInfoData PACKED;        // Pointer to info data area
  short              sError      PACKED;
} DEVIO_GETDEVINFOCMD;

// EXITDRV
typedef struct tagDEVIO_EXITCMD {
  unsigned short  usBoard        PACKED;        // DEV board number
  unsigned short  usDrvOpenCount PACKED;        // dríver opencount
  short           sError         PACKED;
} DEVIO_EXITCMD;

// GETTASKPARAMETER
typedef struct tagDEVIO_GETPARAMETERCMD {
  unsigned short  usBoard           PACKED;     // DEV board number
  unsigned short  usTaskParamNum    PACKED;     // Number of the parameter area
  unsigned short  usTaskParamLen    PACKED;     // Lenght of parameter data
  unsigned char   TaskParameter[64] PACKED;
  short           sError            PACKED;
} DEVIO_GETPARAMETERCMD;

// EXIO
typedef struct tagDEVIO_EXIOCMD {
  unsigned short usBoard         PACKED;        // DEV board number
  unsigned short usSendOffset    PACKED;        // Byte offset send data
  unsigned short usSendLen       PACKED;        // Length of send data
  unsigned char  *pabSendData    PACKED;        // Send data buffer
  unsigned short usReceiveOffset PACKED;        // Byte offset receive data
  unsigned short usReceiveLen    PACKED;        // Length of receive data
  unsigned char  *pabReceiveData PACKED;        // Receive data buffer
  unsigned long  ulTimeout       PACKED;        // Service timeout
  short          sError          PACKED;
} DEVIO_EXIOCMD;

// EXIOEX
typedef struct tagDEVIO_EXIOCMDEX {
  unsigned short     usBoard         PACKED;    // DEV board number
  unsigned short     usSendOffset    PACKED;    // Byte offset send data
  unsigned short     usSendLen       PACKED;    // Length of send data
  unsigned char   *  pabSendData     PACKED;    // Send data buffer
  unsigned short     usReceiveOffset PACKED;    // Byte offset receive data
  unsigned short     usReceiveLen    PACKED;    // Length of receive data
  unsigned char   *  pabReceiveData  PACKED;    // Receive data buffer
  unsigned long      ulTimeout       PACKED;    // Service timeout
  short              sError          PACKED;    // --- Equal to "ExcahangeIOCmd"
  unsigned short     usMode          PACKED;    // External exchange mode
} DEVIO_EXIOCMDEX;

// EXIOERR
#define STATE_ERR_NON       0
#define STATE_ERR           1

#define STATE_MODE_2        2
#define STATE_MODE_3        3
#define STATE_MODE_4        4

typedef struct tagDEVIO_EXIOCMDERR {
  unsigned short    usBoard         PACKED;     // DEV board number
  unsigned short    usSendOffset    PACKED;     // Byte offset send data
  unsigned short    usSendLen       PACKED;     // Length of send data
  unsigned char    *pabSendData     PACKED;     // Send data buffer
  unsigned short    usReceiveOffset PACKED;     // Byte offset receive data
  unsigned short    usReceiveLen    PACKED;     // Length of receive data
  unsigned char    *pabReceiveData  PACKED;     // Receive data buffer
  unsigned long     ulTimeout       PACKED;     // Service timeout
  short             sError          PACKED;     // --- Equal to "ExcahangeIOCmd"
  COMSTATE         *ptStateData     PACKED;     // State data buffer
} DEVIO_EXIOCMDERR;

// READIO
typedef struct tagDEVIO_READSENDCMD {
  unsigned short     usBoard      PACKED;       // DEV board number
  unsigned short     usReadOffset PACKED;       // Byte offset send/receive data
  unsigned short     usReadLen    PACKED;       // Length of send/receive data
  unsigned char   *  pabReadData  PACKED;       // Read send data buffer
  short              sError       PACKED;
} DEVIO_READSENDCMD;

// ExtData
#define EXTDATASIZE 20
typedef struct tagDEVIO_EXTDATACMD {
  unsigned short  usBoard    PACKED;           // DEV board number
  unsigned short  usMode     PACKED;           // DEV mode
  unsigned char  *pabExtData PACKED;           // DEV Extended data
  short           sError     PACKED;
} DEVIO_EXTDATACMD;

// GetMbxData
typedef struct tagDEVIO_GETMBXCMD {
  unsigned short  usBoard        PACKED;       // DEV board number
  unsigned short  usDevLen       PACKED;       // DEV length of dev data
  unsigned short  usHostLen      PACKED;       // DEV length of host data
  unsigned char   abHostMbx[288] PACKED;       // DEV pointer to host data buffer
  unsigned char   abDevMbx[288]  PACKED;       // DEV pointer to device data buffer
  short           sError         PACKED;
} DEVIO_GETMBXCMD;

// ReadWriteRawData
#define PARAMETER_READ    1
#define PARAMETER_WRITE   2
typedef struct tagDEVIO_RWRAWDATACMD {
  unsigned short  usBoard  PACKED;             // DEV board number
  unsigned short  usMode   PACKED;             // DEV read or write
  unsigned short  usOffset PACKED;             // DEV offset in the DPM last kByte
  unsigned short  usLen    PACKED;             // DEV length of data
  unsigned char   *pabData PACKED;             // DEV pointer to data buffer
  short           sError   PACKED;
} DEVIO_RWRAWDATACMD;
// ReadWriteDPMData
typedef struct tagDEVIO_RWDPMDATACMD {
  unsigned short  usBoard  PACKED;             // DEV board number
  unsigned short  usMode   PACKED;             // DEV read or write
  unsigned short  usOffset PACKED;             // DEV offset in the DPM last kByte
  unsigned short  usLen    PACKED;             // DEV length of data
  unsigned char   *pabData PACKED;             // DEV pointer to data buffer
  short           sError   PACKED;
} DEVIO_RWDPMDATACMD;


// GetDPMPtr
#define DPM_PTR_OPEN_DRV  1
#define DPM_PTR_OPEN_USR  2
#define DPM_PTR_CLOSE     3

typedef struct tagDEVIO_GETDPMPTR {
  unsigned  short  usMode     PACKED;          // DEV mode
  unsigned  short  usBoard    PACKED;          // DEV board number
  void            *pvUserData PACKED;          // DEV user data for pmi
  unsigned long   *pulDPMSize PACKED;          // DEV DPM size in bytes
  unsigned char  **pDPMBase   PACKED;          // DEV pointer to data buffer
  unsigned long    lError     PACKED;          // DEV system error
  short            sError     PACKED;          // DEV driver error
} DEVIO_GETDPMPTR;

#ifdef _cplusplus
  }
#endif

#endif /* CIFDEV_I_H */
