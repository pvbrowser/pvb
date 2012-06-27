/***************************************************************************
                 rlmodbusclient.cpp  -  description
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
#include "rlmodbusclient.h"

rlModbusClient::rlModbusClient(const char *mbxname, const char *shmname, int shmsize)
               :rlMailbox(mbxname), rlSharedMemory(shmname, shmsize)
{
}

rlModbusClient::~rlModbusClient()
{
}

int rlModbusClient::write(int slave, int function, const unsigned char *data, int datalen)
{
  unsigned char buf[512];
  int i;

  if(datalen < 0)   return -1;
  if(datalen > 256) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  if(function < 0)  return -1;
  buf[0] = slave;
  buf[1] = function;
  for(i=0; i<datalen; i++)
  {
    buf[i+2] = data[i];
  }
  rlMailbox::write((void *) buf,datalen+2);
  return 0;
}

int rlModbusClient::writeSingleCoil(int slave, int adr, int value)
{
  unsigned char data[8];

  data[0] = adr/256; data[1] = adr & 0x0ff;
  data[2] = 0; data[3] = 0;
  if(value != 0) data[2] = 0x0ff;
  return write(slave, rlModbus::ForceSingleCoil, data, 4);
}

int rlModbusClient::writeMultipleCoils(int slave, int adr, const unsigned char *values, int num_coils)
{
  unsigned char data[512];
  int i,byte_count;

  if(num_coils <= 0) return -1;
  byte_count = ((num_coils-1)/8) + 1;
  data[0] = adr/256;       data[1] = adr & 0x0ff;
  data[2] = num_coils/256; data[3] = num_coils & 0x0ff;
  data[4] = byte_count;
  for(i=0; i<byte_count; i++)
  {
    if((5+i) > (int) sizeof(data)) return -1;
    data[5+i] = values[i];
  }
  return write(slave, rlModbus::ForceMultipleCoils, data, 5+byte_count);
}

int rlModbusClient::writePresetSingleRegister(int slave, int adr, int value)
{
  unsigned char data[8];

  data[0] = adr/256;   data[1] = adr & 0x0ff;
  data[2] = value/256; data[3] = value & 0x0ff;
  return write(slave, rlModbus::PresetSingleRegister, data, 4);
}

int rlModbusClient::writePresetMultipleRegisters(int slave, int adr, const int *values, int num_values)
{
  unsigned char data[512];
  int i;

  data[0] = adr/256;        data[1] = adr & 0x0ff;
  data[2] = num_values/256; data[3] = num_values & 0x0ff;
  data[4] = num_values*2; // byte count
  for(i=0; i<num_values; i++)
  {
    if((5+i*2) > (int) sizeof(data)) return -1;
    data[5+i*2] = values[i]/256; data[6+i*2] = values[i] & 0x0ff;
  }
  return write(slave, rlModbus::PresetMultipleRegs, data, 5+2*num_values);
}

int rlModbusClient::readBit(int offset, int number)
{
  int ret;
  unsigned char buf[128+1],b;

  if(number < 0)   return -1;
  if(number > 128) return -1;
  rlSharedMemory::read(offset,buf,(number/8)+1);
  b = buf[number/8];
  switch(number%8)
  {
    case 0:
      ret = b & BIT0;
      break;
    case 1:
      ret = b & BIT1;
      break;
    case 2:
      ret = b & BIT2;
      break;
    case 3:
      ret = b & BIT3;
      break;
    case 4:
      ret = b & BIT4;
      break;
    case 5:
      ret = b & BIT5;
      break;
    case 6:
      ret = b & BIT6;
      break;
    case 7:
      ret = b & BIT7;
      break;
    default:
      return -1;
  }
  if(ret > 0) return 1;
  return 0;
}

int rlModbusClient::readByte(int offset, int number)
{
  unsigned char buf[4],b;

  if(number < 0)   return -1;
  if(number > 128) return -1;
  rlSharedMemory::read(offset+number,buf,1);
  b = buf[0];
  return b;
}

int rlModbusClient::readShort(int offset, int number)
{
  unsigned char buf[4];
  int s;

  if(number < 0)  return -1;
  if(number > 64) return -1;
  rlSharedMemory::read(offset+(number*2),buf,2);
  s = buf[0]*256 + buf[1];
  return s;
}
