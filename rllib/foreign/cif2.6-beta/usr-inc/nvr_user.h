/* <St> *******************************************************************

  FILENAME    : NVR_USER.H

  -------------------------------------------------------------------------
  CREATED BY  : R. Mayer,  Hilscher GmbH
  CREATED AT  : 29.05.96
  PROJECT     : NVR
  =========================================================================

  FUNCTION :
    User interface NVR protocol

  =========================================================================

  CHANGES OF REVISIONS :

  Version Name      Date      Change
  -------------------------------------------------------------------------

  V1.000  Mayer     29.05.96  Created

  ******************************************************************** <En> */

#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack(1)             /* Byte Alignment   */
#endif

/* ======================================================================== */
/* Protocol definition                                                      */
/* ======================================================================== */

#define NVR_MODE_MSB_LSB        0
#define NVR_MODE_LSB_MSB        1
#define NVR_MODE_BYTE           2
#define NVR_MODE_TRANSPARENT    3


/* ======================================================================== */
/* Protocol parameter structure                                             */
/* ======================================================================== */

typedef struct NVR_PARAMETRtag {
  unsigned char         bScl;           /* Communication line number        */
  unsigned char         bRtsControl;    /* RTS control                      */
  unsigned char         bBaudrate;      /* Baudrate                         */
  unsigned char         bDataBits;      /* Number of data bits              */
  unsigned char         bStopBits;      /* Number of stop bits              */
  unsigned char         bParityBit;     /* Parity                           */
  unsigned char         bPriority;      /* Priority                         */
  unsigned short        usTimeout;      /* Timeout                          */
  unsigned char         bReceiveMode;   /* Receive mode                     */
  unsigned char         bSendMode;      /* Send mode                        */
  unsigned char         bErrorLed;      /* Mode of the error LED            */
} NVR_PARAMETER;

/* ======================================================================== */
/* Protocol task state structure                                            */
/* ======================================================================== */

typedef struct NVR_STATEtag {
  unsigned char         bTaskState;     /* Task state                       */
  unsigned long         ulTxCount;      /* Transmitt telegram count         */
  unsigned long         ulRxCount;      /* Receive telegram count           */
  unsigned char         bTxRetryCount;  /* Number of transmitt retries      */
  unsigned char         bRxRetryCount;  /* Number of receive retries        */
  unsigned short        usTxErrorCount; /* Transmitt error count            */
  unsigned short        usRxErrorCount; /* Receive error count              */
  unsigned short        usErrorBits;    /* Error bits                       */
  unsigned char         bError;         /* Last error                       */
} NVR_STATE;


#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack()              /* Byte Alignment   */
#endif
