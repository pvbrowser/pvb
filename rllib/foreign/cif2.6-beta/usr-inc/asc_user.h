/* <St> *******************************************************************

  FILENAME    : ASC_USER.H

  -------------------------------------------------------------------------
  CREATED BY  : R. Mayer,  Hilscher GmbH
  CREATED AT  : 29.05.96
  PROJECT     : ASC
  =========================================================================

  FUNCTION :
    User interface ASCII protocol

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

/* Mode */

#define ASC_MODE_SLAVE          0
#define ASC_MODE_MASTER         1

/* Stop mode */

#define ASC_ENDMODE_TIMEOUT     0       /* Timeout                          */
#define ASC_ENDMODE_ID          1       /* Identifier                       */
#define ASC_ENDMODE_QUITT       2       /* Acknowledg telegram              */
#define ASC_ENDMODE_ID_QUITT    3       /* Identifier / Acknowledg telegram */
#define ASC_ENDMODE_NUMBER      4       /* Character number                 */
#define ASC_ENDMODE_DEFNUMBER   5       /* Predefined character number      */

/* Checksum */

#define ASC_CHKSUM_NONE         0
#define ASC_CHKSUM_BIN7         1
#define ASC_CHKSUM_BIN8         2
#define ASC_CHKSUM_BCC          3
#define ASC_CHKSUM_BCC_ASCII    4

/* Checksum area */

#define ASC_CHK_AREA_DATA_ONLY    0     /* Data only                        */
#define ASC_CHK_AREA_DATA_STARTID 1     /* Data with start identifier       */
#define ASC_CHK_AREA_DATA_ENDID   2     /* Data with stop identifier        */
#define ASC_CHK_AREA_FULL         3     /* Full telegram                    */

/* Filter */

#define ASC_FILTER_NONE         0
#define ASC_FILTER_DOUBLE_CHAR  1

/* Timeouts */

#define ASC_TIMEOUT_NONE        0

/* Telegram definition */

#define ASC_TELEGRAM_LENGTH_ZERO  0

/* Telegram sequenz */

#define ASC_TELEGRAM_SEQUENZ_NONE 0


/* ======================================================================== */
/* Protocol parameter structure                                             */
/* ======================================================================== */

typedef struct ASC_PARAMETRtag {
  unsigned char         bScl;           /* Communication line number        */
  unsigned char         bRtsControl;    /* RTS control                      */
  unsigned char         bBaudrate;      /* Baudrate                         */
  unsigned char         bDataBits;      /* Number of data bits              */
  unsigned char         bStopBits;      /* Number of stop bits              */
  unsigned char         bParityBit;     /* Parity                           */
  unsigned char         bMode;          /* Mode                             */
  unsigned char         bEndMode;       /* End mode                         */
  unsigned char         bCheckMode;     /* Check mode                       */
  unsigned char         bCheckArea;     /* Check area                       */
  unsigned char         bFilterMode;    /* Filter mode                      */
  unsigned short        usFilterCharacter; /* Filter characters             */
  unsigned short        usTelTimeout;   /* Telegram timeout                 */
  unsigned short        usStartTimeout; /* Telegram start timeout           */
  unsigned short        usCharTimeout;  /* Charater timeout                 */
  unsigned char         bRetries;       /* Telegram retries                 */
  unsigned char         bErrorLed;      /* Mode of the error LED            */
  unsigned char         bTelStartLen;   /* Telegram start length            */
  unsigned char         bTelStart[8];   /* Start telegram                   */
  unsigned char         bTelEndLen;     /* End telegram length              */
  unsigned char         bTelEnd[8];     /* End telegram                     */
  unsigned char         bTelAckLen;     /* ACK telegram length              */
  unsigned char         bTelAck[8];     /* ACK telegram                     */
  unsigned char         bTelNackLen;    /* NACK telegram length             */
  unsigned char         bTelNack[8];    /* NACK telegram                    */
  unsigned char         bTelDeviceLen;  /* Device telegram length           */
  unsigned short        usTelFollowTime;/* Telegram following time          */
} ASC_PARAMETER;

/* ======================================================================== */
/* Protocol task state structure                                            */
/* ======================================================================== */

typedef struct ASC_STATEtag {
  unsigned char         bTaskState;     /* Task state                       */
  unsigned long         ulTxCount;      /* Transmitt telegram count         */
  unsigned long         ulRxCount;      /* Receive telegram count           */
  unsigned short        usTxErrorCount; /* Transmitt error count            */
  unsigned short        usRxErrorCount; /* Receive error count              */
  unsigned short        usErrorBits;    /* Error bits                       */
  unsigned char         bError;         /* Last error                       */
} ASC_STATE;


#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack()              /* Byte Alignment   */
#endif

/* === eof 'USER.H' === */
