/***************************************************************************
                    rlmodbusclient.h  -  description
                             -------------------
    begin                : Wed Jan 07 2004
    copyright            : (C) 2004 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_MODBUS_CLIENT_H_
#define _RL_MODBUS_CLIENT_H_

#include "rldefine.h"
#include "rlmodbus.h"
#include "rlmailbox.h"
#include "rlsharedmemory.h"

/*! <pre>
This class is for data aqusition over a modbus daemon as created by pvdevelop.
It communicates over a shared memory and a mailbox according to the pvbrowser principle.
</pre> */
class rlModbusClient : public rlMailbox, public rlSharedMemory
{
  public:
    rlModbusClient(const char *mbxname, const char *shmname, int shmsize);
    virtual ~rlModbusClient();
    int write(int slave, int function, const unsigned char *data, int len);
    int writeSingleCoil(int slave, int adr, int value);
    int writeMultipleCoils(int slave, int adr, const unsigned char *values, int num_coils);
    int writePresetSingleRegister(int slave, int adr, int value);
    int writePresetMultipleRegisters(int slave, int adr, const int *values, int num_values);
    int readBit(int offset, int number);
    int readByte(int offset, int number);
    int readShort(int offset, int number);
};

#endif

