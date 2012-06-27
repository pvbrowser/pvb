/***************************************************************************
                      rlhilschercif.cpp  -  description
                             -------------------
    begin                : Tue Feb 13 2007
    copyright            : (C) 2007 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlhilschercif.h"
#include <stdio.h>

// On Windows add *dll.lib to Makefile
#ifdef RLUNIX
#include "cif_api.c"
#endif

rlHilscherCIF::rlHilscherCIF()
{
  debug = 0;
  isOpen = 0;
  usBoardNumber = 0; // Board number, 0-3

  // Fill in protokoll parameters
  aParameter.bScl               =    1; /* Interface number                     */
  aParameter.bRtsControl        =    0; /* Bus handling                         */
  aParameter.bBaudrate          =   10; /* Baud rate                            */
  aParameter.bDataBits          =    8; /* Number of data bits                  */
  aParameter.bStopBits          =    1; /* Number of stop bits                  */
  aParameter.bParityBit         =    1; /* Parity                               */
  aParameter.bMode              =    1; /* Interface mode                       */
  aParameter.bEndMode           =    0; /* Stop mod                             */
  aParameter.bCheckMode         =    0; /* Checksum mode                        */
  aParameter.bCheckArea         =    0; /* Checksum area                        */
  aParameter.bFilterMode        =    0; /* Character filter mode                */
  aParameter.usFilterCharacter  =    0; /* Characters                           */
  aParameter.usTelTimeout       = 1000; /* Telegram timeout                     */
  aParameter.usStartTimeout     =    0; /* Start timeout                        */
  aParameter.usCharTimeout      =    0; /* Character timeout                    */
  aParameter.bRetries           =    0; /* Retray number                        */
  aParameter.bErrorLed          =    1; /* Error LED                            */
  aParameter.bTelStartLen       =    0; /* Length of telegram start             */
  aParameter.bTelStart[0]       =    0; /* Telegram beginning                   */
  aParameter.bTelEndLen         =    0; /* Length of telegram end               */
  aParameter.bTelEnd[0]         =    0; /* Telegram end                         */
  aParameter.bTelAckLen         =    0; /* Length  of ACK telegram              */
  aParameter.bTelAck[0]         =    0; /* ACK telegramm                        */
  aParameter.bTelNackLen        =    0; /* Length of NAK telegram               */
  aParameter.bTelNack[0]        =    0; /* NAK telegram                         */
  aParameter.bTelDeviceLen      =    0; /* Length of the device telegram        */

  tMessage.rx = 0x0ff;
}

rlHilscherCIF::~rlHilscherCIF()
{
  if(isOpen) close();
}

int rlHilscherCIF::open()
{
  int ret;
  isOpen = 1;
  if((ret = devOpenDriver())               != DRV_NO_ERROR) return ret;
  if((ret = devInitBoard())                != DRV_NO_ERROR) return ret;
  if((ret = devGetInfo(GET_FIRMWARE_INFO)) != DRV_NO_ERROR) return ret;
  if((ret = devSetHostState(HOST_READY))   != DRV_NO_ERROR) return ret;
  if(debug) printFirmwareInfo();
  devPutTaskParameter();
  devReset();
  if(debug) printf("cif.open() success\n");
  return ret;
}

int rlHilscherCIF::close()
{
  int ret;
  isOpen = 0;
  if((ret = devSetHostState(HOST_NOT_READY)) != DRV_NO_ERROR) return ret;
  ret     = devCloseDriver();
  return ret;
}

int rlHilscherCIF::devOpenDriver()
{
  isOpen = 1;
#ifdef RLUNIX
  int ret = DevOpenDriver();
#else
  int ret = DevOpenDriver(usBoardNumber);
#endif
  if(debug) printf("DevOpenDriver ret=%d\n",ret);
  return ret;
}

int rlHilscherCIF::devInitBoard()
{
#ifdef RLUNIX
  int ret = DevInitBoard(usBoardNumber);
#else
  int ret = DevInitBoard(usBoardNumber,NULL);
#endif
  if(debug) printf("DevInitBoard ret=%d\n",ret);
  return ret;
}

int rlHilscherCIF::devGetInfo(int info)
{
  if(info == GET_FIRMWARE_INFO)
  {
    int ret = DevGetInfo(usBoardNumber,
                         GET_FIRMWARE_INFO,
                         sizeof(FIRMWAREINFO),
                         &abInfo[0]);
    if(debug) printf("DevGetInfo(GET_FIRMWARE_INFO) ret=%d\n", ret);
    return ret;
  }
  if(info == GET_IO_INFO)
  {
    // Read the mode of the IO-Communication, 0 = MODE UNKNOWN
    int ret = DevGetInfo(usBoardNumber,  // DeviceNumber
                         GET_IO_INFO,    // InfoArea
                         sizeof(IOINFO), // Size of bytes to read
                         &tIoInfo);      // Pointer to user buffer

    if(debug)
    {
      printf("DevGetInfo (IOINFO) ret=%d \n", ret);
      printf("ExchangeIO information\n"
             "  IO-Communication mode : %d\n"
             "  IO-COM bit            : %d\n"
             "  IO-Count              : %ld\n",
             tIoInfo.bIOExchangeMode, tIoInfo.bComBit, tIoInfo.ulIOExchangeCnt);
      if(tIoInfo.bIOExchangeMode == 0)
      {
        printf( "  --- IO-Communication NOT AVAILABLE ---\n");
      }
      else
      {
        printf( "  --- IO-Communication AVAILABLE ---\n");
        // Test if IO-Communication is running
        if(tIoInfo.bComBit == 0)
        {
          printf("  IO-Communication is not running, so all function\n"
                 "  returning an error !\n");
        }
      }
    }
    return ret;
  }
  printf("rlHilscherCIF::devGetInfo() unknown info\n");
  return -1;
}

int rlHilscherCIF::devSetHostState(int mode)
{
  int ret = DevSetHostState(usBoardNumber,
                            mode,
                            0L);
  if(debug) printf("DevSetHostState mode=%d ret=%d\n", mode, ret);
  return ret;
}

int rlHilscherCIF::devPutTaskParameter()
{
  // Write parameters to communication board
  int ret = DevPutTaskParameter(usBoardNumber,        // Device number
                                1,                    // Protokoll task number
                                sizeof(aParameter),   // Lenght of bytes
                                &aParameter );        // Pointer to data  buffer
  if(debug) printf("DevPutTaskParameter ret=%d (Interface number 1)\n", ret);
  return ret;
}

int rlHilscherCIF::devReset()
{
  // Reset board so it can take the parameters
  int ret = DevReset((unsigned char)  usBoardNumber, // Device number
                     (unsigned short) WARMSTART,     // Reset mode, Warmstar
                     (unsigned long)  8000L);        // Timeout  8000 ms
  if(debug) printf("DevResetBoard ret=%d\n", ret);
  return ret;
}

int rlHilscherCIF::devExitBoard()
{
  int ret = DevExitBoard(usBoardNumber);
  if(debug) printf("DevExitBoard ret=%d \n", ret);
  return ret;
}

int rlHilscherCIF::devCloseDriver()
{
  isOpen = 0;
#ifdef RLUNIX
  int ret = DevCloseDriver();
#else
  int ret = DevCloseDriver(usBoardNumber);
#endif
  if(debug) printf( "DevCloseDriver ret=%d\n", ret);
  return ret;
}

int rlHilscherCIF::devGetMessage(int timeout)
{
  if(isOpen == 0)
  {
    printf("rlHilscherCIF::devGetMessage ERROR device is not open\n");
    return -1;
  }
  if(tMessage.rx == 0x0ff)
  {
    printf("rlHilscherCIF::devGetMessage() ERROR you have to set tMessage first\n");
    return -1;
  }
  if(debug) printf("rlHilscherCIF::devGetMessage() timeout=%d", timeout);
  int ret = DevGetMessage(usBoardNumber,            // Device Number
                          sizeof(tMessage),         // Users buffer size
                          (MSG_STRUC *)&tMessage,   // Pointer to user buffer
                          (long) timeout);          // Timeout
  if(debug) printf("DevGetMessage ret=%d\n", ret);
  if(ret == DRV_NO_ERROR)
  {
    // Message available
    if(debug) printf( "  RX: 0x%02X, TX: 0x%02X, LN: 0x%02X, NR: 0x%02X\n",
              tMessage.rx,
              tMessage.tx,
              tMessage.ln,
              tMessage.nr);
    if(debug) printf( "  A:  0x%02X, F:  0x%02X, B:  0x%02X, E:  0x%02X\n",
              tMessage.a,
              tMessage.f,
              tMessage.b,
              tMessage.e);
  }
  return ret;
}

int rlHilscherCIF::devPutMessage(int timeout)
{
  if(isOpen == 0)
  {
    printf("rlHilscherCIF::devPutMessage ERROR device is not open\n");
    return -1;
  }
  if(debug) printf("rlHilscherCIF::devPutMessage() timeout=%d", timeout);
  int ret = DevPutMessage(usBoardNumber,            // Device Number
                          (MSG_STRUC *)&tMessage,   // Pointer to user buffer
                          timeout);                 // Timeout
  if(debug) printf("DevPutMessage ret=%d\n", ret);
  return ret;
}

int rlHilscherCIF::devExchangeIO(int sendOffset, int sendSize, unsigned char *sendData,
                                 int receiveOffset, int receiveSize, unsigned char *receiveData,
                                 int timeout)
{
  if(isOpen == 0)
  {
    printf("rlHilscherCIF::devExchangeIO ERROR device is not open\n");
    return -1;
  }
  // Run ExchangeIO
  int ret = DevExchangeIO(usBoardNumber,
                          (unsigned short) sendOffset,
                          (unsigned short) sendSize,
                          sendData,
                          (unsigned short) receiveOffset,
                          (unsigned short) receiveSize,
                          receiveData,
                          timeout);
  if(ret != DRV_NO_ERROR)
  {
    if(debug) printf("DevExchangeIO ret=%d timeout\n", ret);
  }
  else
  {
    // Data exchange is done, print some information
    if(debug && sendSize >= 4 && receiveSize >= 4)
    {
      printf("  ExchangeIO data:\n"
             "  Send data:                  Receive data\n");
      printf("  byte 0 : %d                 byte 0 : %d\n", sendData[0],receiveData[0]);
      printf("  byte 1 : %d                 byte 1 : %d\n", sendData[1],receiveData[1]);
      printf("  byte 2 : %d                 byte 2 : %d\n", sendData[2],receiveData[2]);
      printf("  byte 3 : %d                 byte 3 : %d\n", sendData[3],receiveData[3]);
    }
  }
  return ret;
}

void rlHilscherCIF::printFirmwareInfo()
{
  unsigned char   abTemp[30]; // Temporary buffer
  memcpy(&abTemp, &abInfo[0], 16);
  abTemp[16] = 0;
  printf("FirmwareName:     %s\n",&abTemp[0]);
  memcpy(&abTemp, &abInfo[16], 16);
  abTemp[16] = 0;
  printf("FirmwareVersion:  %s\n",&abTemp[0]);
}

//#define TESTING
#ifdef TESTING
int main()
{
  rlHilscherCIF cif;
  int c = 0;
  int i = 0;
  unsigned char sendData[512];
  unsigned char receiveData[512];

  cif.debug = 1;
  if(cif.open() != DRV_NO_ERROR) return -1;

  while(c != 'x')
  {
    printf("hit RETURN 'x'=end ");
    c = getchar();

    // Write test data to Send buffer
    sendData[0] = (unsigned char) i++;
    sendData[1] = 0xFF;
    sendData[2] = 2;
    sendData[3] = 3;
    cif.devExchangeIO(0,4,sendData,
                      0,4,receiveData,
                      1000);
    if(i >= 256) i = 0;
  }
  return 0;
}

int xmain( void )
{
  unsigned short  usBoardNumber = 0;          /* Board number, 0-3 */
  unsigned short  usDevState, usHostState;

  unsigned char   abTemp[30];                 /* Temporary buffer */
  unsigned char   abInfo[300];                /* Buffer for various information */
  short           sRet;                       /* Return value */

  /* Protocol parameter definition */
  ASC_PARAMETER   aParameter;         /* Parameters for ASCII protocolls */

  /* Information structure */
  IOINFO          tIoInfo;            /* IO information structure */

  /* Data structure for message based communication */
  RCS_MESSAGETELEGRAM_10  tMessage;

  /* Data areas for IO based communication */
  unsigned char   tIOSendData[512];   /* Buffer for send data */
  unsigned char   tIORecvData[512];   /* Buffer for receive data */


  printf( "HILSCHER GmbH - Communication test program \n\n");
  
  printf( "\n*** Open communication driver ***\n");
  printf(   "*** Board number: %d           ***\n\n", usBoardNumber);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Open the driver */
  if ( (sRet = DevOpenDriver()) != DRV_NO_ERROR) {
    printf( "DevOpenDriver       RetWert = %5d \n", sRet );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Initialize board */
  } else if ( (sRet = DevInitBoard ( usBoardNumber)) != DRV_NO_ERROR) {
    printf( "DevInitBoard        RetWert = %5d \n", sRet );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Read Firmware information field */
  } else if ( (sRet = DevGetInfo( usBoardNumber,                    /* DeviceNumber        */
                                  GET_FIRMWARE_INFO,                /* InfoArea            */
                                  sizeof(FIRMWAREINFO),             /* Size of bytes to read */
                                  &abInfo[0])) != DRV_NO_ERROR ) {  /* Pointer to user buffer */

    printf( "DevGetInfo (GET_FIRMWARE_INFO) RetWert = %5d \n", sRet );
    
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Signal board, application is running */
  } else if ( (sRet = DevSetHostState( usBoardNumber,               /* DeviceNumber        */
                                       HOST_READY,                  /* Mode                */
                                       0L)) != DRV_NO_ERROR ) {     /* Pointer to user buffer */
    
    printf( "DevSetHostState (HOST_READY) RetWert = %5d \n", sRet );

  } else {

    /* Print firmware information */
    memcpy ( &abTemp, &abInfo[0], 16);
    abTemp[16] = 0;
    printf( "  FirmwareName:     %s\n", &abTemp[0] );
    memcpy ( &abTemp, &abInfo[16], 16);
    abTemp[16] = 0;
    printf( "  FirmwareVersion:  %s\n", &abTemp[0]);
    printf( "\n\n" );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Read mailbox information */
    if ( (sRet = DevGetMBXState( usBoardNumber,                     /* DeviceNumber        */
                                 &usDevState,                       /* Buffer for device state */
                                 &usHostState)) != DRV_NO_ERROR )  {/* Buffer for host state */
      // function error
      printf( "DevGetMBXState          RetWert = %5d\n", sRet );
      
    } else {

      // show actual state
      printf( "DevGetMBXState device MBX state = %5d       (0=empty, 1=full)\n", usDevState);
      printf( "DevGetMBXState host   MBX state = %5d       (0=empty, 1=full)\n", usHostState);

    }


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Fill in protokoll parameters */

    aParameter.bScl               =    1; /* Interface number                     */
    aParameter.bRtsControl        =    0; /* Bus handling                         */
    aParameter.bBaudrate          =   10; /* Baud rate                            */
    aParameter.bDataBits          =    8; /* Number of data bits                  */
    aParameter.bStopBits          =    1; /* Number of stop bits                  */
    aParameter.bParityBit         =    1; /* Parity                               */
    aParameter.bMode              =    1; /* Interface mode                       */
    aParameter.bEndMode           =    0; /* Stop mod                             */
    aParameter.bCheckMode         =    0; /* Checksum mode                        */
    aParameter.bCheckArea         =    0; /* Checksum area                        */
    aParameter.bFilterMode        =    0; /* Character filter mode                */
    aParameter.usFilterCharacter  =    0; /* Characters                           */
    aParameter.usTelTimeout       = 1000; /* Telegram timeout                     */
    aParameter.usStartTimeout     =    0; /* Start timeout                        */
    aParameter.usCharTimeout      =    0; /* Character timeout                    */
    aParameter.bRetries           =    0; /* Retray number                        */
    aParameter.bErrorLed          =    1; /* Error LED                            */
    aParameter.bTelStartLen       =    0; /* Length of telegram start             */
    aParameter.bTelStart[0]       =    0; /* Telegram beginning                   */
    aParameter.bTelEndLen         =    0; /* Length of telegram end               */
    aParameter.bTelEnd[0]         =    0; /* Telegram end                         */
    aParameter.bTelAckLen         =    0; /* Length  of ACK telegram              */
    aParameter.bTelAck[0]         =    0; /* ACK telegramm                        */
    aParameter.bTelNackLen        =    0; /* Length of NAK telegram               */
    aParameter.bTelNack[0]        =    0; /* NAK telegram                         */
    aParameter.bTelDeviceLen      =    0; /* Length of the device telegram        */

    /* Write parameters to communication board */
    sRet = DevPutTaskParameter( usBoardNumber,        /* Device number             */
                                1,                    /* Protokoll task number     */
                                sizeof(aParameter),   /* Lenght of bytes           */
                                &aParameter );        /* Pointer to data  buffer   */
    printf( "DevPutTaskParameter     RetWert = %5d       (Interface number 1)\n", sRet );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Reset board so it can take the parameters */

    sRet = DevReset( (unsigned char)  usBoardNumber, /* Device number          */
                     (unsigned short) WARMSTART,     /* Reset mode, Warmstart  */
                     (unsigned long)  8000L);        /* Timeout  8000 ms       */
    printf( "DevResetBoard           RetWert = %5d \n", sRet );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Wait for user response */
    printf( "\nPress RETURN to continue .... \n");
    getchar();                                         
                                                                                   
    /* ========================================================================= */
    /* Test Message transfer                                                     */                                                                                   
    /* ========================================================================= */
    
    printf( "\n*** Test for message transfer ***\n\n");
    
    /* Tray to read a message */
    printf( "  DevGetMessage       Wait for a maximum of %ld ms\n", 5000L );

    sRet = DevGetMessage (  usBoardNumber,            /* Device Number             */
                            sizeof(tMessage),         /* Users buffer size         */
                            (MSG_STRUC *)&tMessage,   /* Pointer to user buffer    */
                            5000L );                  /* Timeout                   */
    printf( "  DevGetMessage       RetWert = %5d \n", sRet );


    /*===========================================================================*/
    /* Test message transfer                                                     */
    /*===========================================================================*/
    /* Build a message */
    tMessage.rx         = 0x01;
    tMessage.tx         = 0x10;
    tMessage.ln         =   12;
    tMessage.nr         =    1;
    tMessage.a          =    0;
    tMessage.f          =    0;
    tMessage.b          =   17;
    tMessage.e          = 0x00;
    tMessage.device_adr =    0;
    tMessage.data_area  =    0;
    tMessage.data_adr   =    0;
    tMessage.data_idx   =    0;
    tMessage.data_cnt   =    4;
    tMessage.data_type  =   10;
    tMessage.function   =    2;
    tMessage.d[0]       =    1;   /* Some user data */
    tMessage.d[1]       =    2;   /* Some user data */
    tMessage.d[2]       =    3;   /* Some user data */
    tMessage.d[3]       =    4;   /* Some user data */

    sRet = DevPutMessage (  usBoardNumber,            /* Device Number             */
                            (MSG_STRUC *)&tMessage,   /* Pointer to user buffer    */
                            5000L );                  /* Timeout                   */
    printf( "  DevPutMessage       RetWert = %5d \n", sRet );


    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Wait for user response */
    printf( "\nPress RETURN to continue .... \n");
    getchar();

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Tray to read a message */
    printf( "  DevGetMessage       Wait for a maximum of %ld ms\n", 20000L );
    
    sRet = DevGetMessage (  0,                        /* Device Number             */
                            sizeof(tMessage),         /* Users buffer size         */
                            (MSG_STRUC *)&tMessage,   /* Pointer to user buffer    */
                            20000L );                 /* Timeout                   */
    printf( "  DevGetMessage       RetWert = %5d \n", sRet );

    if ( sRet == DRV_NO_ERROR) {
      /* Message available */
      printf( "  RX: 0x%02X, TX: 0x%02X, LN: 0x%02X, NR: 0x%02X \n",
              tMessage.rx,
              tMessage.tx,
              tMessage.ln,
              tMessage.nr          );
      printf( "  A:  0x%02X, F:  0x%02X, B:  0x%02X, E:  0x%02X \n",
              tMessage.a,
              tMessage.f,
              tMessage.b,
              tMessage.e           );
    } /* endif */


    /* ========================================================================= */
    /* Test for ExchangeIO                                                       */
    /* ========================================================================= */

    /* ========================================================================= */
    /* Test for ExchangeIO                                                       */
    /* ========================================================================= */
    /* Test if IO-Communication is available */
                                         
    printf( "\n\n*** Test for ExchangeIO functions ***\n\n");
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Read the mode of the IO-Communication, 0 = MODE UNKNOWN */
    sRet = DevGetInfo( usBoardNumber,                 /* DeviceNumber        */
                       GET_IO_INFO,                   /* InfoArea            */
                       sizeof(IOINFO),                /* Size of bytes to read */
                       &tIoInfo);                     /* Pointer to user buffer */

    printf( "  DevGetInfo (IOINFO) RetWert = %5d \n", sRet );

    /* Print IO information */
    printf( "  ExchangeIO information\n"
            "  IO-Communication mode : %d\n"
            "  IO-COM bit            : %d\n"
            "  IO-Count              : %ld\n",
            tIoInfo.bIOExchangeMode, tIoInfo.bComBit, tIoInfo.ulIOExchangeCnt);
    
    if ( tIoInfo.bIOExchangeMode == 0) {
      
      /* No IO-Communication available */
      printf( "  --- IO-Communication NOT AVAILABLE ---\n");
      
    } else {
      
      /* IO-Communication is available */
      printf( "  --- IO-Communication AVAILABLE ---\n");
    
      /* Test if IO-Communication is running */
      if ( tIoInfo.bComBit == 0) {
        /* No Communication is running */
        printf( "  IO-Communication is not running, so all function\n"
                "  returning an error !.\n"); 
        
      }                                                                                
        
      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* Read the hole send area to update internal data */
      if ( (sRet = DevGetInfo( usBoardNumber,                           /* DeviceNumber        */
                               GET_IO_SEND_DATA,                        /* InfoArea            */
                               sizeof(tIOSendData),                     /* Size of bytes to read */
                               (void*)&tIOSendData[0])) != DRV_NO_ERROR ) {/* Pointer to user buffer */
    
        printf( "  DevGetInfo (IO_SEND_DATA) RetWert = %5d \n", sRet );
          
      } else {
        
        /* Write test data to Send buffer */
        tIOSendData[0] = 0xC7;
        tIOSendData[1] = 0xFF;
        tIOSendData[2] = 2;        
        tIOSendData[3] = 3;
        
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        /* Run ExchangeIO */
        if ( (sRet = DevExchangeIO ( usBoardNumber,
                                     0,               /* usSendOffset    */
                                     4,               /* usSendSize      */
                                     &tIOSendData[0], /* *pvSendData     */
                                     0,               /* usReceiveOffset */
                                     4,               /* usReceiveSize   */
                                     &tIORecvData[0], /* *pvReceiveData  */
                                     100L )) != DRV_NO_ERROR  ) {    /* ulTimeout       */
                                       
          printf( "DevExchangeIO RetWert = %5d \n", sRet );
            
        } else {
          
          /* Data exchange is done, print some information */
          printf( "  ExchangeIO data:\n"
                  "  Send data:                  Receive data\n");
          printf( "  byte 0 : %d                 byte 0 : %d\n", tIOSendData[0],tIORecvData[0]);
          printf( "  byte 1 : %d                 byte 1 : %d\n", tIOSendData[1],tIORecvData[1]);
          printf( "  byte 2 : %d                 byte 2 : %d\n", tIOSendData[2],tIORecvData[2]);
          printf( "  byte 3 : %d                 byte 3 : %d\n", tIOSendData[3],tIORecvData[3]);
        }
      }
    }
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Wait for user response */
    printf( "\nPress RETURN to continue .... \n");
    getchar();
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Signal board, application is not running */
    if ( (sRet = DevSetHostState( usBoardNumber,                /* DeviceNumber        */
                                  HOST_NOT_READY,             /* Mode                */
                                  0L)) != DRV_NO_ERROR ) {    /* Pointer to user buffer */
    
      printf( "DevSetHostState (HOST_NOT_READY) RetWert = %5d \n", sRet );
    }

    printf( "\n*** Close communication driver ***\n\n");
    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Close communication */

    sRet = DevExitBoard( usBoardNumber); /* Device number  */
    printf( "DevExitBoard        RetWert = %5d \n", sRet );

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /* Close Driver */

    sRet = DevCloseDriver();
    printf( "DevCloseDriver      RetWert = %5d \n", sRet );
  }

  return(0);
}

#endif
