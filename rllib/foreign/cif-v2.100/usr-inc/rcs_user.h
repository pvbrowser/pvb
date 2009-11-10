/* <St> *******************************************************************

  FILENAME    : RCS_USER.H

  -------------------------------------------------------------------------
  CREATED BY  : R.Mayer,  Hilscher GmbH
  CREATED AT  : 10.05.96
  PROJECT     : global
  =========================================================================

  FUNCTION :
    General RCS definitions

  =========================================================================

  CHANGES OF REVISIONS :

  Version Name      Date      Change
  -------------------------------------------------------------------------

  V1.002  Mayer     14.05.98  RCS_TELEGRAMHEADERDATA_10 structure included

  V1.001  Mayer     29.05.96  Task errors included

  V1.000  Mayer     10.05.96  created from the file rc090203.h

  ******************************************************************** <En> */

#ifdef __cplusplus
}
#endif

#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack(1)             /* Byte Alignment   */
#endif

/* ======================================================================== */
/* General task errors                                                      */
/* ======================================================================== */

#define TASK_F_OK                              0
#define TASK_F_NO_COMMUNICATION                1
#define TASK_F_IDLE                            2

#define TASK_F_INIT_BASE                       50

#define TASK_F_PARITY                          100
#define TASK_F_FRAMING                         101
#define TASK_F_OVERRUN                         102
#define TASK_F_DATACOUNT                       103
#define TASK_F_CHECKSUM                        104
#define TASK_F_TIMEOUT                         105
#define TASK_F_PROTOCOL                        106
#define TASK_F_DATA                            107
#define TASK_F_NACK                            108

#define TASK_F_PROTOCOL_BASE                   110

#define TASK_F_MESSAGEHEADER                   150
#define TASK_F_MESSAGESIZE                     151
#define TASK_F_MESSAGECOMMAND                  152
#define TASK_F_MESSAGESTRUCTURE                153
#define TASK_F_MESSAGEERROR                    154
#define TASK_F_MESSAGETIMEOUT                  155

#define TASK_F_TELEGRAMHEADER                  160
#define TASK_F_DEVICE_ADR                      161
#define TASK_F_DATA_AREA                       162
#define TASK_F_DATA_ADR                        163
#define TASK_F_DATA_IDX                        164
#define TASK_F_DATA_CNT                        165
#define TASK_F_DATA_TYPE                       166
#define TASK_F_FUNCTION                        167

#define TASK_F_MESSAGE_BASE                    170

#define TASK_F_NOT_INITIALIZED                 200
#define TASK_F_BUSY                            201
#define TASK_F_SEGMENT                         202
#define TASK_F_USER                            203

#define TASK_F_DATABASE                        210
#define TASK_F_DATABASE_WRITE                  211
#define TASK_F_DATABASE_READ                   212
#define TASK_F_STRUCTURE                       213
#define TASK_F_PARAMETER                       214
#define TASK_F_CONFIGURATION                   215
#define TASK_F_FUNCTIONLIST                    216
#define TASK_F_SYSTEM                          217

#define TASK_F_SYSTEM_BASE                     220


/* ======================================================================== */
/* Task commands                                                            */
/* ======================================================================== */

#define TASK_B_10                           16     /* RCS_TELEGRAMHEADER_10 */
#define TASK_B_11                           17     /* Task specific         */


/* ======================================================================== */
/* Message definitions                                                      */
/* ======================================================================== */

/* ----------------- Definition of the standard telegram header ----------- */
/* Keyword: MESSAGE    ---------------------------------------------------- */
/* TelegramFunCtion */

#define TASK_TFC_UNUSED                        0
#define TASK_TFC_READ                          1
#define TASK_TFC_WRITE                         2
#define TASK_TFC_QUERRY                        3

/* TelegramDataArea */
/* 'data_area'      */

#define TASK_TDA_UNUSED                        0
#define TASK_TDA_BIT                           1
#define TASK_TDA_BYTE                          2
#define TASK_TDA_WORD                          3
#define TASK_TDA_DWORD                         4
#define TASK_TDA_FLOAT                         5

/* TelegramDataType */
/* 'data_type' for MOTOROLA data types !                                    */
/* For INTEL data types, the 'RCS_TDT_IDF_MASK' flag must be set            */

#define TASK_TDT_UNUSED                        0
#define TASK_TDT_BOOLEAN                       1
#define TASK_TDT_INT8                          2
#define TASK_TDT_INT16                         3
#define TASK_TDT_INT32                         4
#define TASK_TDT_UINT8                         5
#define TASK_TDT_UINT16                        6
#define TASK_TDT_UINT32                        7
#define TASK_TDT_FLOAT                         8
#define TASK_TDT_ASCII                         9
#define TASK_TDT_STRING                        10
#define TASK_TDT_BIT                           14

#define TASK_TDT_IDF_MSK                     0X80

/* ======================================================================== */
/* Message structure definitions                                            */
/* ======================================================================== */

/* -------------------- General  RCS definitions -------------------------- */
/* Keyword: MESSAGE  ------------------------------------------------------ */

#define RCS_SEGMENT_LEN                        288
#define RCS_MESSAGEHEADER_LEN                  8
#define RCS_TELEGRAMHEADER_LEN                 8

/* ------------------------ RCS message definition ------------------------ */

typedef struct RCS_MESSAGEHEADERtag {
   unsigned char        rx;             /* receiver                         */
   unsigned char        tx;             /* transmitter                      */
   unsigned char        ln;             /* lenght                           */
   unsigned char        nr;             /* number                           */
   unsigned char        a;              /* answer                           */
   unsigned char        f;              /* fault                            */
   unsigned char        b;              /* command                          */
   unsigned char        e;              /* extension                        */
} RCS_MESSAGEHEADER;

typedef struct RCS_MESSAGEtag {
   unsigned char        rx;             /* receiver                         */
   unsigned char        tx;             /* transmitter                      */
   unsigned char        ln;             /* lenght                           */
   unsigned char        nr;             /* number                           */
   unsigned char        a;              /* answer                           */
   unsigned char        f;              /* fault                            */
   unsigned char        b;              /* command                          */
   unsigned char        e;              /* extension                        */
   unsigned char        d[ RCS_SEGMENT_LEN-RCS_MESSAGEHEADER_LEN ]; /* data */
} RCS_MESSAGE;


/* ----------------- Standard telegram header ----------------------------- */
/* Keyword: MESSAGE, TASK_B_10  --------------------------------------------*/

typedef struct RCS_TELEGRAMHEADER_10tag {
   unsigned char        device_adr;     /* device address                   */
   unsigned char        data_area;      /* data area                        */
   unsigned short       data_adr;       /* data address                     */
   unsigned char        data_idx;       /* data index                       */
   unsigned char        data_cnt;       /* data count                       */
   unsigned char        data_type;      /* data type                        */
   unsigned char        function;       /* function                         */
} RCS_TELEGRAMHEADER_10;

typedef struct RCS_MESSAGETELEGRAMHEADER_10_tag {
   unsigned char        rx;             /* receiver                         */
   unsigned char        tx;             /* transmitter                      */
   unsigned char        ln;             /* lenght                           */
   unsigned char        nr;             /* number                           */
   unsigned char        a;              /* answer                           */
   unsigned char        f;              /* fault                            */
   unsigned char        b;              /* command                          */
   unsigned char        e;              /* extension                        */
   unsigned char        device_adr;     /* device address                   */
   unsigned char        data_area;      /* data area                        */
   unsigned short       data_adr;       /* data address                     */
   unsigned char        data_idx;       /* data index                       */
   unsigned char        data_cnt;       /* data count                       */
   unsigned char        data_type;      /* data type                        */
   unsigned char        function;       /* function                         */
} RCS_MESSAGETELEGRAMHEADER_10;

typedef struct RCS_TELEGRAMHEADERDATA_10tag {
   unsigned char        device_adr;
   unsigned char        data_area;
   unsigned short       data_adr;
   unsigned char        data_idx;
   unsigned char        data_cnt;
   unsigned char        data_type;
   unsigned char        function;
   unsigned char        d[ RCS_SEGMENT_LEN-RCS_MESSAGEHEADER_LEN-RCS_TELEGRAMHEADER_LEN];
} RCS_TELEGRAMHEADERDATA_10;

typedef struct RCS_MESSAGETELEGRAM_10tag {
   unsigned char        rx;             /* receiver                         */
   unsigned char        tx;             /* transmitter                      */
   unsigned char        ln;             /* lenght                           */
   unsigned char        nr;             /* number                           */
   unsigned char        a;              /* answer                           */
   unsigned char        f;              /* fault                            */
   unsigned char        b;              /* command                          */
   unsigned char        e;              /* extension                        */
   unsigned char        device_adr;     /* device address                   */
   unsigned char        data_area;      /* data area                        */
   unsigned short       data_adr;       /* data address                     */
   unsigned char        data_idx;       /* data index                       */
   unsigned char        data_cnt;       /* data count                       */
   unsigned char        data_type;      /* data type                        */
   unsigned char        function;       /* function                         */
   unsigned char        d[ RCS_SEGMENT_LEN-RCS_MESSAGEHEADER_LEN-RCS_TELEGRAMHEADER_LEN];
} RCS_MESSAGETELEGRAM_10;


#if defined( _MSC_VER)          /* Microsoft C */
    #pragma pack()              /* Byte Alignment   */
#endif

#ifdef __cplusplus
}
#endif

/* === eof 'RCS_USER.H' === */
