// Please replace this dummy by the real hilscher cif driver

#include "rldefine.h"
#include "rlthread.h"
#include "CIFUSER.h"
#include "rcs_user.h"     /* Include file for RCS definition    */
#include "asc_user.h"     /* Include file for ASCII protocols   */
#include "nvr_user.h"     /* Include file for 3964R protocol    */

  short APIENTRY DevOpenDriver        ( unsigned short usDevNumber) { return -1; }

  short APIENTRY DevCloseDriver       ( unsigned short usDevNumber) { return -1; }

  short APIENTRY DevGetBoardInfo      ( unsigned short usDevNumber,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevInitBoard         ( unsigned short usDevNumber,
                                        void           *pDevAddress) { return -1; }

  short APIENTRY DevExitBoard         ( unsigned short usDevNumber) { return -1; }

  short APIENTRY DevPutTaskParameter  ( unsigned short usDevNumber,
                                        unsigned short usNumber,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevReset             ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevPutMessage        ( unsigned short usDevNumber,
                                        MSG_STRUC      *ptMessage,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevGetMessage        ( unsigned short usDevNumber,
                                        unsigned short usSize,
                                        MSG_STRUC      *ptMessage,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevGetTaskState      ( unsigned short usDevNumber,
                                        unsigned short usNumber,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevGetMBXState       ( unsigned short usDevNumber,
                                        unsigned short *pusDevMBXState,
                                        unsigned short *pusHostMBXState) { return -1; }

  short APIENTRY DevTriggerWatchDog   ( unsigned short usDevNumber,
                                        unsigned short usFunction,
                                        unsigned short *usDevWatchDog) { return -1; }

  short APIENTRY DevGetInfo           ( unsigned short usDevNumber,
                                        unsigned short usFunction,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevGetTaskParameter  ( unsigned short usDevNumber,
                                        unsigned short usNumber,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevExchangeIO        ( unsigned short usDevNumber,
                                        unsigned short usSendOffset,
                                        unsigned short usSendSize,
                                        void           *pvSendData,
                                        unsigned short usReceiveOffset,
                                        unsigned short usReceiveSize,
                                        void           *pvReceiveData,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevReadSendData      ( unsigned short usDevNumber,
                                        unsigned short usOffset,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevSetHostState      ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevExtendedData      ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevGetMBXData        ( unsigned short usDevNumber,
                                        unsigned short usHostSize,
                                        void           *pvHostData,
                                        unsigned short usDevSize,
                                        void           *pvDevData) { return -1; }

  short APIENTRY DevGetBoardInfoEx    ( unsigned short usDevNumber,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevExchangeIOEx      ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned short usSendOffset,
                                        unsigned short usSendSize,
                                        void           *pvSendData,
                                        unsigned short usReceiveOffset,
                                        unsigned short usReceiveSize,
                                        void           *pvReceiveData,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevExchangeIOErr     ( unsigned short usDevNumber,
                                        unsigned short usSendOffset,
                                        unsigned short usSendSize,
                                        void           *pvSendData,
                                        unsigned short usReceiveOffset,
                                        unsigned short usReceiveSize,
                                        void           *pvReceiveData,
                                        COMSTATE       *ptState,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevReadWriteDPMRaw   ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned short usOffset,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevSpecialControl    ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned short *pusCtrlAck) { return -1; }

  short APIENTRY DevDownload          ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned char  *pszFileName,
                                        DWORD          *pdwBytes) { return -1; }

  short APIENTRY DevReadWriteDPMData  ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned short usOffset,
                                        unsigned short usSize,
                                        void           *pvData) { return -1; }

  /*-------------------*/
  /* Special functions */
  /*-------------------*/
  short APIENTRY DevGetDPMPtr         ( unsigned short usMode,
                                        unsigned short usDevNumber,
                                        void           *pvUserData,
                                        unsigned long  *pulDPMSize,
                                        unsigned char  **pDPMBase,
                                        long           *plError) { return -1; }

  /*---------------------------*/
  /* CIF100 spezific functions */
  /*---------------------------*/
  short APIENTRY DevPerformance       ( void *ptData) { return -1; }
  short APIENTRY DevRAMTest           ( void *ptData) { return -1; }

  short APIENTRY DevTimer             ( unsigned short usDevNumber,
                                        unsigned short usControl,
                                        unsigned short usMode,
                                        unsigned short usResolution,
                                        char           bTickCount) { return -1; }

  short APIENTRY DevBackupRAM         ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned long  ulOffset,
                                        unsigned long  ulSize,
                                        void           *pvData) { return -1; }

  short APIENTRY DevRAMrw             ( unsigned short usDevNumber,
                                        unsigned long  ulDevStartAdd,
                                        unsigned short usDataLen,
                                        void           *pvSendData,
                                        void           *pvReceiveData,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevDMAdown           ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevClearConfig       ( unsigned short usDevNumber) { return -1; }

  short APIENTRY DevIsPLCDataReady    ( unsigned short usDevNumber,
                                        unsigned short *pusState) { return -1; }

  short APIENTRY DevExchangePLCData   ( unsigned short usDevNumber,
                                        unsigned short usSendSize,
                                        unsigned short usReceiveSize,
                                        unsigned long  ulTimeout) { return -1; }

  short APIENTRY DevHWPortControl     ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned short *pusState) { return -1; }

  short APIENTRY DevMemoryPtr         ( unsigned short usDevNumber,
                                        unsigned short usMode,
                                        unsigned long  usSize,
                                        CIF_PLC_DRIVER_INFO *ptPLCData) { return -1; }


