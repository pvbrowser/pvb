/***************************************************************************
                      rlhilschercif.h  -  description
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
#ifndef _RL_HILSCHER_CIF_H_
#define _RL_HILSCHER_CIF_H_

#include "rldefine.h"
#include "rlthread.h"
#ifdef RLUNIX
#include "cif_user.h"     /* Include file for device driver API */
#else
#include "CIFUSER.h"
#endif
#include "rcs_user.h"     /* Include file for RCS definition    */
#include "asc_user.h"     /* Include file for ASCII protocols   */
#include "nvr_user.h"     /* Include file for 3964R protocol    */

/*! <pre>
This class is for data access to Hilscher CIF cards
like PROFIBUS ...
It uses the driver provided by Hilscher and has access to it's dual ported RAM.

Attention: In order to use this class on Linux as normal user you have to set
chmod ugoa+rw /dev/cif
</pre> */
class rlHilscherCIF
{
public:
  rlHilscherCIF();
  virtual ~rlHilscherCIF();
  int  debug;

  int  open();  // convenience method
  int  close(); // convenience method

  int  devGetMessage(int timeout); // use mailbox, uses tMessage
  int  devPutMessage(int timeout); // use mailbox, please set tMessage
  int  devExchangeIO(int sendOffset, int sendSize, unsigned char *sendData,
                     int receiveOffset, int receiveSize, unsigned char *receiveData,
                     int timeout);

  int  devOpenDriver();
  int  devInitBoard();
  int  devGetInfo(int info);      // info = GET_FIRMWARE_INFO | GET_IO_INFO
  int  devSetHostState(int mode); // mode = HOST_READY | HOST_NOT_READY
  int  devPutTaskParameter();
  int  devReset();
  int  devExitBoard();
  int  devCloseDriver();
  void printFirmwareInfo();

  unsigned short  usBoardNumber;           // Board number, 0-3
  unsigned short  usDevState, usHostState;
  unsigned char   abInfo[300];             // Buffer for various information
  ASC_PARAMETER   aParameter;              // Parameters for ASCII protocolls
  IOINFO          tIoInfo;                 // IO information structure
  RCS_MESSAGETELEGRAM_10  tMessage;

  rlMutex mutex;

private:
  int isOpen;
};
#endif
