/***************************************************************************
                          rlmodbus.h  -  description
                             -------------------
    begin                : Tue Mar 13 2003
    copyright            : (C) 2003 by Rainer Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_MODBUS_H_
#define _RL_MODBUS_H_

#include "rldefine.h"
#include "rlsocket.h"
#include "rlserial.h"

/*! <pre>
This class implements the modbus protocol.
You can use serial interfaces or TCP/IP.
Modbus RTU and ASCII are available.
</pre> */
class rlModbus
{
  public:
    enum Modbus
    {
      MODBUS_CHECKSUM_ERROR = -2,
      MODBUS_ERROR          = -1,
      MODBUS_SUCCESS        = 0,
      MODBUS_RTU            = 1,
      MODBUS_ASCII          = 2
    };

    enum ModbusFunctionCodes
    {
      ReadCoilStatus         = 1,
      ReadInputStatus        = 2,
      ReadHoldingRegisters   = 3,
      ReadInputRegisters     = 4,
      ForceSingleCoil        = 5,
      PresetSingleRegister   = 6,
      ReadExceptionStatus    = 7,
      FetchCommEventCtr      = 11,
      FetchCommEventLog      = 12,
      ForceMultipleCoils     = 15,
      PresetMultipleRegs     = 16,
      ReportSlaveID          = 17,
      ReadGeneralReference   = 20,
      WriteGeneralReference  = 21,
      MaskWrite4XRegisters   = 22,
      ReadWrite4XRegisters   = 23,
      ReadFifoQueue          = 24
    };

    rlModbus(long max_telegram_length = 1024, int mode = MODBUS_RTU, char end_delimitor = 0x0a);
    virtual ~rlModbus();
    int  write   (int  slave, int  function, const unsigned char *data, int len, int *transactionID = NULL);
    int  request (int  slave, int  function, int start_adr, int num_register);
    int  response(int *slave, int *function, unsigned char *data, int timeout=1000);
    int  readRequest(int *slave, int *function, unsigned char *data, int timeout=1000, int *transactionID = NULL);
    void registerSocket(rlSocket *socket);
    void registerSerial(rlSerial *serial);
    int  data2int(const unsigned char *data);
    int  int2data(int val, unsigned char *data);
    int  intsize();
  private:
    int  buf2int_rtu(unsigned char *buf);
    void int2buf_rtu(int i, unsigned char *buf);
    int  buf2int_ascii(unsigned char *buf);
    void int2buf_ascii(int i, unsigned char *buf);
    void insertLRC(int len);
    void insertCRC(int len);
    int  LRCerror(int len);
    int  CRCerror(int len);
    rlSocket *s;
    rlSerial *tty;
    unsigned char *tel;
    long     maxtel;
    int      mode;
    char     delimitor;
};

#endif

