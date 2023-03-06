/***************************************************************************
                          rlmodbus.cpp  -  description
                             -------------------
    begin                : Tue Mar 13 2003
    copyright            : (C) 2003 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlmodbus.h"
#include <stdio.h>
#include <string.h>

rlModbus::rlModbus(long max_telegram_length, int _mode, char end_delimitor)
{
  if(max_telegram_length < 256) max_telegram_length = 256;
  tel = new unsigned char[max_telegram_length];
  maxtel = max_telegram_length;
  mode = _mode;
  delimitor = end_delimitor;
  s = NULL;
  tty = NULL;
  autoreconnectSocket = 1;
}

rlModbus::~rlModbus()
{
  if(tel != NULL) delete [] tel;
}

int rlModbus::data2int(const unsigned char *data)
{
  return (data[0]*256)+data[1];
}

int rlModbus::int2data(int val, unsigned char *data)
{
  data[0] = (unsigned char) val / 256;
  data[1] = (unsigned char) val & 0x0ff;
  return 0;
}

int rlModbus::intsize()
{
  return 2;
}

int rlModbus::write(int slave, int function, const unsigned char *data, int datalen, int *transactionID)
{
  int len,i;

  if(slave < 0 || slave > 255) return MODBUS_ERROR;
  len = 0;
  if(mode == MODBUS_ASCII)
  {
    tel[len++] = ':';
    sprintf((char *) &tel[len], "%02X", slave);          len += 2;
    sprintf((char *) &tel[len], "%02X", function);       len += 2;
    for(i=0; i<datalen; i++)
    {
      sprintf((char *) &tel[len], "%02X",(int) data[i]); len += 2;
      if((len+4) > maxtel) return MODBUS_ERROR;
    }
    insertLRC(len);                                     len += 2;
    tel[len++] = 0x0d;
    if(delimitor == 0x0a) tel[len++] = 0x0a;
  }
  else if(mode == MODBUS_RTU)
  {
    if(s != NULL)
    {
      if(transactionID == NULL)
      {
        tel[len++] = 0;       // bytes 0,1 Transaction ID. Not important. Usually zero when making a request, the server will copy them faithfully into the response.
        tel[len++] = 0;
      }
      else
      {
        tel[len++] = ((*transactionID) & 0xFF00) / 256;         // bytes 0,1 Transaction ID.
        tel[len++] = (*transactionID) & 0xFF;
      }
      tel[len++] = 0;         // bytes 2,3 Protocol number. Must be zero.
      tel[len++] = 0;
      tel[len++] = 0;         // byte 4 Length (upper byte). Since all requests will be less than 256 bytes in length (!), this will always be zero.
      tel[len++] = 2+datalen; // byte 5 Length (lower byte). Equal to the number of bytes which follow
    }
    tel[len++] = (unsigned char) slave;
    tel[len++] = (unsigned char) function;
    for(i=0; i<datalen; i++)
    {
      tel[len++] = data[i];
      if((len+2) > maxtel) return MODBUS_ERROR;
    }
    insertCRC(len);                                     len += 2;
  }
  else return MODBUS_ERROR;

  if(s != NULL)
  {
    if(s->isConnected() == 0)
    {
      if(autoreconnectSocket) s->connect();
      if(s->isConnected() == 0) return MODBUS_ERROR;
    }
    if(s->write(tel,len-2) < 0) return MODBUS_ERROR; // don't send LRC or CRC
  }
  else if(tty != NULL)
  {
    if(tty->writeBlock(tel,len) < 0) return MODBUS_ERROR;
  }
  else return MODBUS_ERROR;
  return MODBUS_SUCCESS;
}

int rlModbus::request(int slave, int function, int start_adr, int num_register)
{
  unsigned char data[4];

  data[0] = (unsigned char) ( start_adr / 256 );
  data[1] = (unsigned char) ( start_adr & 0x0ff );
  data[2] = (unsigned char) ( num_register / 256 ); 
  data[3] = (unsigned char) ( num_register & 0x0ff );
  return write(slave, function, data, 4);
}

int rlModbus::response(int *slave, int *function, unsigned char *data, int timeout)
{
  unsigned char *telptr;
  int ret,len,byte_count,idata,i,itel,val;

  len = 0;
  if(mode != MODBUS_ASCII && mode != MODBUS_RTU) return MODBUS_ERROR;
  if(s != NULL)
  {
    if(s->isConnected() == 0) return MODBUS_ERROR;
    if(mode == MODBUS_RTU)
    {
      if(s->read((char *) tel, 6, timeout) <= 0) return MODBUS_ERROR;
      // bytes 0,1 Transaction ID faithfully copied from the request message
      // bytes 2,3 Protocol number always zero
      // byte 4 Response length (upper byte) Always zero
      // byte 5 Response length (lower byte). Equal to the number of bytes which follow
      // Here comes the normal Modus telegram
      if(s->read((char *) tel, 2, timeout) <= 0) return MODBUS_ERROR;
      *slave     = tel[0];
      *function  = tel[1];
      byte_count = tel[5] - 3;
      if(s->read((char *) tel, 1, timeout) <= 0)            return MODBUS_ERROR;
      switch(*function)
      {
        case ReadCoilStatus:
        case ReadInputStatus:
        case ReadHoldingRegisters:
        case ReadInputRegisters:
        case FetchCommEventLog:
        case ReportSlaveID:
        case ReadGeneralReference:
        case WriteGeneralReference:
        case ReadWrite4XRegisters:
          //byte_count = tel[0];
          if(s->read((char *) data, byte_count, timeout) <= 0)  return MODBUS_ERROR;
          return byte_count;
        case ForceSingleCoil:
        case PresetSingleRegister:
        case FetchCommEventCtr:
        case ForceMultipleCoils:
        case PresetMultipleRegs:
          //byte_count = 4;
          if(s->read((char *) data, byte_count, timeout) <= 0)  return MODBUS_ERROR;
          return byte_count;
        case ReadExceptionStatus:
          //byte_count = 1;
          if(s->read((char *) data, byte_count, timeout) <= 0)  return MODBUS_ERROR;
          return byte_count;
        case MaskWrite4XRegisters:
          //byte_count = 6;
          if(s->read((char *) data, byte_count, timeout) <= 0)  return MODBUS_ERROR;
          return byte_count;
        case ReadFifoQueue:
          if(s->read((char *) tel, 2, timeout) <= 0)            return MODBUS_ERROR;
          //byte_count = tel[0]*256 + tel[1];
          if(s->read((char *) data, byte_count, timeout) <= 0)  return MODBUS_ERROR;
          return byte_count;
        default:
          return MODBUS_ERROR;
      }
    }
  }
  else if(tty != NULL)
  {
    if(mode == MODBUS_ASCII)
    {
      //printf("modbus ascii\n");
      for(i=0; i<maxtel; i++)
      {
        ret = tty->select(timeout);
        if(ret == 0) return MODBUS_ERROR;
        //printf("readChar\n");
        itel = tty->readChar();
        if(itel < 0) return MODBUS_ERROR;
        tel[i] = (unsigned char) itel;
        if(tel[i] == 0x0d && delimitor != 0x0a) break;
        if(tel[i] == 0x0a)                      break;
      }
      tel[i] = '\0';
      telptr = (unsigned char *) strchr((const char *) tel,':');
      if(telptr == NULL) return MODBUS_ERROR;
      len++;
      sscanf((char *) &telptr[len],"%02X",slave);       len += 2;
      sscanf((char *) &telptr[len],"%02X",function);    len += 2;
      switch(*function)
      {
        case ReadCoilStatus:
        case ReadInputStatus:
        case ReadHoldingRegisters:
        case ReadInputRegisters:
        case FetchCommEventLog:
        case ReportSlaveID:
        case ReadGeneralReference:
        case WriteGeneralReference:
        case ReadWrite4XRegisters:
          sscanf((char *) &telptr[len],"%02X",&byte_count);    len += 2;
          for(idata=0; idata<byte_count; idata++)
          {
            sscanf((const char *) &telptr[len], "%02X", &val); len += 2;
            data[idata] = val;
          }
          data[idata] = 0x0ff; // terminator, this data can't come over modbus
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case ForceSingleCoil:
        case PresetSingleRegister:
        case FetchCommEventCtr:
        case ForceMultipleCoils:
        case PresetMultipleRegs:
          byte_count = 8;
          for(idata=0; idata<(byte_count/2); idata++)
          {
            data[idata] = buf2int_ascii(&telptr[len]);     len += 2;
          }
          data[idata] = 0x0ff; // terminator, this data can't come over modbus
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case ReadExceptionStatus:
          byte_count = 2;
          for(idata=0; idata<(byte_count/2); idata++)
          {
            data[idata] = buf2int_ascii(&telptr[len]);     len += 2;
          }
          data[idata] = 0x0ff; // terminator, this data can't come over modbus
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case MaskWrite4XRegisters:
          byte_count = 12;
          for(idata=0; idata<(byte_count/2); idata++)
          {
            data[idata] = buf2int_ascii(&telptr[len]);     len += 2;
          }
          data[idata] = 0x0ff; // terminator, this data can't come over modbus
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case ReadFifoQueue:
          sscanf((char *) &telptr[len],"%04X",&byte_count); len += 4;
          for(idata=0; idata<(byte_count/2); idata++)
          {
            data[idata] = buf2int_ascii(&telptr[len]);     len += 2;
          }
          data[idata] = 0x0ff; // terminator, this data can't come over modbus
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return byte_count;
          break;
        default:
          return MODBUS_ERROR;
      }
    }
    else if(mode == MODBUS_RTU)
    {
      ret = tty->select(timeout);
      if(ret == 0)                             return MODBUS_ERROR;
      if(tty->readBlock(tel, 2, timeout) <= 0) return MODBUS_ERROR;
      *slave     = tel[len++];
      *function  = tel[len++];
      byte_count = tel[5] - 3;
      switch(*function)
      {
        case ReadCoilStatus:
        case ReadInputStatus:
        case ReadHoldingRegisters:
        case ReadInputRegisters:
        case FetchCommEventLog:
        case ReportSlaveID:
        case ReadGeneralReference:
        case WriteGeneralReference:
        case ReadWrite4XRegisters:
          if(tty->select(timeout) == 0)                        return MODBUS_ERROR;
          if(tty->readBlock(&tel[len], 1, timeout) <= 0)       return MODBUS_ERROR;
          //byte_count = tel[len++];
          if(tty->select(timeout) == 0)                        return MODBUS_ERROR;
          if(tty->readBlock(data, byte_count+2, timeout) <= 0) return MODBUS_ERROR;
          memcpy(&tel[len],data,byte_count+2); len += byte_count + 2;
          if(CRCerror(len) == 1)                      return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case ForceSingleCoil:
        case PresetSingleRegister:
        case FetchCommEventCtr:
        case ForceMultipleCoils:
        case PresetMultipleRegs:
          //byte_count = 4;
          if(tty->select(timeout) == 0)                        return MODBUS_ERROR;
          if(tty->readBlock(data, byte_count+2, timeout) <= 0) return MODBUS_ERROR;
          memcpy(&tel[len],data,byte_count+2); len += byte_count + 2;
          if(CRCerror(len) == 1)                      return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case ReadExceptionStatus:
          //byte_count = 1;
          if(tty->select(timeout) == 0)                        return MODBUS_ERROR;
          if(tty->readBlock(data, byte_count+2, timeout) <= 0) return MODBUS_ERROR;
          memcpy(&tel[len],data,byte_count+2); len += byte_count + 2;
          if(CRCerror(len) == 1)                      return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case MaskWrite4XRegisters:
          //byte_count = 6;
          if(tty->select(timeout) == 0)                        return MODBUS_ERROR;
          if(tty->readBlock(data, byte_count+2, timeout) <= 0) return MODBUS_ERROR;
          memcpy(&tel[len],data,byte_count+2); len += byte_count + 2;
          if(CRCerror(len) == 1)                      return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case ReadFifoQueue:
          if(tty->select(timeout) == 0)                        return MODBUS_ERROR;
          if(tty->readBlock(&tel[len], 2, timeout) <= 0)       return MODBUS_ERROR;
          //byte_count = tel[len]*256 + tel[len+1]; len += 2;
          if(tty->select(timeout) == 0)                        return MODBUS_ERROR;
          if(tty->readBlock(data, byte_count+2, timeout) <= 0) return MODBUS_ERROR;
          memcpy(&tel[len],data,byte_count+2); len += byte_count + 2;
          if(CRCerror(len) == 1)                      return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        default:
          return MODBUS_ERROR;
      }
    }
  }
  else return MODBUS_ERROR;
  return MODBUS_SUCCESS;
}

int rlModbus::readRequest(int *slave, int *function, unsigned char *data, int timeout, int *transactionID)
{
  unsigned char *telptr;
  int ret,len,byte_count,i,itel,val;

  len = 0;
  if(mode != MODBUS_ASCII && mode != MODBUS_RTU) return MODBUS_ERROR;
  if(s != NULL)
  {
    if(s->isConnected() == 0) return MODBUS_ERROR;
    if(mode == MODBUS_RTU)
    {
      if(s->read((char *) tel, 6, timeout) <= 0) return MODBUS_ERROR;
      if(transactionID != NULL) *transactionID = tel[0] * 256 + tel[1]; // return transactionID
      // bytes 0,1 Transaction ID faithfully copied from the request message
      // bytes 2,3 Protocol number always zero
      // byte 4 Response length (upper byte) Always zero
      // byte 5 Response length (lower byte). Equal to the number of bytes which follow
      // Here comes the normal Modus telegram
      if(s->read((char *) tel, 2, timeout) <= 0) return MODBUS_ERROR;
      *slave     = tel[0];
      *function  = tel[1];
      switch(*function)
      {
        case ReadCoilStatus:
        case ReadInputStatus:
        case ReadHoldingRegisters:
        case ReadInputRegisters:
        case ForceSingleCoil:
        case PresetSingleRegister:
          if(s->read((char *) data, 4, timeout) <= 0)              return MODBUS_ERROR;
          return 4;
        case ReadExceptionStatus:
        case FetchCommEventCtr:
        case FetchCommEventLog:
        case ReportSlaveID:
          return 0;
        case ForceMultipleCoils:
        case PresetMultipleRegs:
          if(s->read((char *) data, 5, timeout) <= 0)              return MODBUS_ERROR;
          byte_count = data[4];
          if(s->read((char *) &data[4], byte_count, timeout) <= 0) return MODBUS_ERROR;
          return 4+byte_count;
        case ReadGeneralReference:
        case WriteGeneralReference:
          if(s->read((char *) data, 1, timeout) <= 0)              return MODBUS_ERROR;
          byte_count = data[0];
          if(s->read((char *) data, byte_count, timeout) <= 0)     return MODBUS_ERROR;
          return byte_count;
        case MaskWrite4XRegisters:
          if(s->read((char *) data, 6, timeout) <= 0)              return MODBUS_ERROR;
          return 6;
        case ReadWrite4XRegisters:
          if(s->read((char *) data, 9, timeout) <= 0)              return MODBUS_ERROR;
          byte_count = data[8];
          if(s->read((char *) &data[8], byte_count, timeout) <= 0) return MODBUS_ERROR;
          return 8+byte_count;
        case ReadFifoQueue:
          if(s->read((char *) data, 2, timeout) <= 0)              return MODBUS_ERROR;
          return 2;
        default:
          return MODBUS_ERROR;
      }
    }
  }
  else if(tty != NULL)
  {
    if(mode == MODBUS_ASCII)
    {
      //printf("modbus ascii\n");
      for(i=0; i<maxtel; i++)
      {
        ret = tty->select(timeout);
        if(ret == 0) return MODBUS_ERROR;
        //printf("readChar\n");
        itel = tty->readChar();
        if(itel < 0) return MODBUS_ERROR;
        tel[i] = (unsigned char) itel;
        if(tel[i] == 0x0d && delimitor != 0x0a) break;
        if(tel[i] == 0x0a)                      break;
      }
      tel[i] = '\0';
      telptr = (unsigned char *) strchr((const char *) tel,':');
      if(telptr == NULL) return MODBUS_ERROR;
      len++;
      sscanf((char *) &telptr[len],"%02X",slave);       len += 2;
      sscanf((char *) &telptr[len],"%02X",function);    len += 2;
      switch(*function)
      {
        case ReadCoilStatus:
        case ReadInputStatus:
        case ReadHoldingRegisters:
        case ReadInputRegisters:
        case ForceSingleCoil:
        case PresetSingleRegister:
          sscanf((char *) &telptr[len],"%02X",&val); data[0] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[1] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[2] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[3] = (unsigned char) val; len += 2;
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return 4;
        case ReadExceptionStatus:
        case FetchCommEventCtr:
        case FetchCommEventLog:
        case ReportSlaveID:
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return 0;
        case ForceMultipleCoils:
        case PresetMultipleRegs:
          sscanf((char *) &telptr[len],"%02X",&val); data[0] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[1] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[2] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[3] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&byte_count);                         len += 2;
          for(i=0; i<byte_count; i++)
          {
            sscanf((char *) &telptr[len],"%02X",&val); data[4+i] = (unsigned char) val; len += 2;
          }
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return 4+byte_count;
        case ReadGeneralReference:
        case WriteGeneralReference:
          sscanf((char *) &telptr[len],"%02X",&byte_count);                           len += 2;
          for(i=0; i<byte_count; i++)
          {
            sscanf((char *) &telptr[len],"%02X",&val); data[i] = (unsigned char) val; len += 2;
          }
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case MaskWrite4XRegisters:
          sscanf((char *) &telptr[len],"%02X",&val); data[0] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[1] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[2] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[3] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[4] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[5] = (unsigned char) val; len += 2;
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return 6;
        case ReadWrite4XRegisters:
          sscanf((char *) &telptr[len],"%02X",&val); data[0] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[1] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[2] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[3] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[4] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[5] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[6] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[7] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&byte_count);                         len += 2;
          for(i=0; i<byte_count; i++)
          {
            sscanf((char *) &telptr[len],"%02X",&val); data[8+i] = (unsigned char) val; len += 2;
          }
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return 8+byte_count;
        case ReadFifoQueue:
          sscanf((char *) &telptr[len],"%02X",&val); data[0] = (unsigned char) val; len += 2;
          sscanf((char *) &telptr[len],"%02X",&val); data[1] = (unsigned char) val; len += 2;
          if(LRCerror(len) == 1) return MODBUS_CHECKSUM_ERROR;
          return 2;
        default:
          return MODBUS_ERROR;
      }
    }
    else if(mode == MODBUS_RTU)
    {
      ret = tty->select(timeout);
      if(ret == 0)                             return MODBUS_ERROR;
      if(tty->readBlock(tel, 2, timeout) <= 0) return MODBUS_ERROR;
      *slave     = tel[len++];
      *function  = tel[len++];
      ret = tty->select(timeout);
      if(ret == 0)                    return MODBUS_ERROR;
      switch(*function)
      {
        case ReadCoilStatus:
        case ReadInputStatus:
        case ReadHoldingRegisters:
        case ReadInputRegisters:
        case ForceSingleCoil:
        case PresetSingleRegister:
          if(tty->readBlock(data, 4+2, timeout) <= 0)              return MODBUS_ERROR;
          memcpy(&tel[len],data,4+2); len += 4+2;
          if(CRCerror(len) == 1)                          return MODBUS_CHECKSUM_ERROR;
          return 4;
        case ReadExceptionStatus:
        case FetchCommEventCtr:
        case FetchCommEventLog:
        case ReportSlaveID:
          if(CRCerror(len) == 1)                          return MODBUS_CHECKSUM_ERROR;
          return 0;
        case ForceMultipleCoils:
        case PresetMultipleRegs:
          if(tty->readBlock(data, 5, timeout) <= 0)                return MODBUS_ERROR;
          memcpy(&tel[len],data,5); len += 5;
          byte_count = data[4];
          if(tty->readBlock(&data[4], byte_count+2, timeout) <= 0) return MODBUS_ERROR;
          memcpy(&tel[len],&data[4],byte_count+2); len += byte_count+2;
          if(CRCerror(len) == 1)                          return MODBUS_CHECKSUM_ERROR;
          return 4+byte_count;
        case ReadGeneralReference:
        case WriteGeneralReference:
          if(tty->readBlock(data, 1, timeout) <= 0)                return MODBUS_ERROR;
          memcpy(&tel[len],data,1); len++;
          byte_count = data[0];
          if(tty->readBlock(data, byte_count+2, timeout) <= 0)     return MODBUS_ERROR;
          memcpy(&tel[len],data,byte_count+2); len += byte_count+2;
          if(CRCerror(len) == 1)                          return MODBUS_CHECKSUM_ERROR;
          return byte_count;
        case MaskWrite4XRegisters:
          if(tty->readBlock(data, 6+2, timeout) <= 0)              return MODBUS_ERROR;
          memcpy(&tel[len],data,6+2); len += 6+2;
          if(CRCerror(len) == 1)                          return MODBUS_CHECKSUM_ERROR;
          return 6;
        case ReadWrite4XRegisters:
          if(tty->readBlock(data, 9, timeout) <= 0)                return MODBUS_ERROR;
          memcpy(&tel[len],data,9); len += 9;
          byte_count = data[8];
          if(tty->readBlock(&data[8], byte_count+2, timeout) <= 0) return MODBUS_ERROR;
          memcpy(&tel[len],&data[8],byte_count+2); len += byte_count+2;
          if(CRCerror(len) == 1)                          return MODBUS_CHECKSUM_ERROR;
          return 8+byte_count;
        case ReadFifoQueue:
          if(tty->readBlock(data, 2+2, timeout) <= 0)              return MODBUS_ERROR;
          memcpy(&tel[len],data,2+2); len += 2+2;
          if(CRCerror(len) == 1)                          return MODBUS_CHECKSUM_ERROR;
          return 2;
        default:
          return MODBUS_ERROR;
      }
    }
  }
  else return MODBUS_ERROR;
  return MODBUS_SUCCESS;
}

void rlModbus::registerSocket(rlSocket *socket)
{
  tty = NULL;
  s = socket;
}

void rlModbus::registerSerial(rlSerial *serial)
{
  s = NULL;
  tty = serial;
}

int rlModbus::buf2int_rtu(unsigned char *buf)
{
  return (buf[0]*256 + buf[1]);
}

void rlModbus::int2buf_rtu(int i, unsigned char *buf)
{
  int high, low;

  high = i / 256;
  low  = i & 0x0ff;
  buf[0] = (unsigned char) high;
  buf[1] = (unsigned char) low;
}

int rlModbus::buf2int_ascii(unsigned char *buf)
{
  int val;

  sscanf((char *) buf,"%04X",&val);
  return val;
}

void rlModbus::int2buf_ascii(int i, unsigned char *buf)
{
  sprintf((char *) buf,"%04X",i);
}

void rlModbus::insertLRC(int len)
{
  unsigned char lrc;
  int i,high,low,val;

  if(len < 0) return;
  lrc = 0;
  for(i=1; i<len; i+=2) // exclude starting ':' and trailing <CR><LF>
  {
    sscanf((const char *) &tel[i],   "%1X", &high);
    sscanf((const char *) &tel[i+1], "%1X", &low);
    val = high*16 + low;
    lrc += val;
  }
  lrc = ((unsigned char)(-((char) lrc)));
  sprintf((char *) &tel[len],"%02X",(unsigned int) lrc);
}

int rlModbus::LRCerror(int len)
{
  unsigned char *cptr;
  unsigned char lrc;
  int i,high,low,val;

  if(len < 0) return 1;
  tel[maxtel-1] = '\0';
  cptr = (unsigned char *) strchr((char *)tel,':');
  if(cptr == NULL) return 1;
  cptr++;
  lrc = 0;
  for(i=1; i<len+2; i+=2) // exclude starting ':' and trailing <CR><LF>
  {                       // len is without lrc -> len+2
    sscanf((const char *) cptr++, "%1X", &high);
    sscanf((const char *) cptr++, "%1X", &low);
    val = high*16 + low;
    lrc += val;
  }
  if(lrc == 0) return 0; // lrc ok
  return 1;              // lrc error
}

/* Table of CRC values for high-order byte */
static const unsigned char array_crc_low[] =
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const unsigned char array_crc_high[] =
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

void rlModbus::insertCRC(int len)
{
  unsigned char crc_high, crc_low;
  unsigned index;
  int i;

  if(len < 0) return;
  crc_high = crc_low = 0xff;
  for(i=0; i<len; i++)
  {
    index = crc_low ^ tel[i];
    crc_low  = crc_high ^ array_crc_low[index];
    crc_high = array_crc_high[index];
  }
  tel[len]   = crc_low;
  tel[len+1] = crc_high;
}

int rlModbus::CRCerror(int len)
{
  unsigned char crc_high, crc_low;
  unsigned index;
  int i;

  if(len < 2) return 1;
  crc_high = crc_low = 0xff;
  for(i=0; i<len-2; i++)
  {
    index = crc_low ^ tel[i];
    crc_low  = crc_high ^ array_crc_low[index];
    crc_high = array_crc_high[index];
  }
  if(crc_low  != tel[len-2]) return 1;
  if(crc_high != tel[len-1]) return 1;
  return 0;
}

int rlModbus::readCoilStatus(int slave, int start_adr, int number_of_coils, unsigned char *status, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (start_adr / 256) & 0x0ff;
  data[1] = start_adr & 0x0ff;
  data[2] = (number_of_coils / 256) & 0x0ff;
  data[3] = number_of_coils & 0x0ff;
  ret = write(slave, ReadCoilStatus, data, 4);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, status, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != ReadCoilStatus) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, ReadCoilStatus, ret_function, timeout);
    return MODBUS_ERROR;
  }  

  return ret;
}

int rlModbus::readInputStatus(int slave, int start_adr, int number_of_inputs, unsigned char *status, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (start_adr / 256) & 0x0ff;
  data[1] = start_adr & 0x0ff;
  data[2] = (number_of_inputs / 256) & 0x0ff;
  data[3] = number_of_inputs & 0x0ff;
  ret = write(slave, ReadInputStatus, data, 4);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, status, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != ReadInputStatus) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, ReadInputStatus, ret_function, timeout);
    return MODBUS_ERROR;
  }  

  return ret;
}

int rlModbus::readHoldingRegisters(int slave, int start_adr, int number_of_registers, int *registers, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (start_adr / 256) & 0x0ff;
  data[1] = start_adr & 0x0ff;
  data[2] = (number_of_registers / 256) & 0x0ff;
  data[3] = number_of_registers & 0x0ff;
  ret = write(slave, ReadHoldingRegisters, data, 4);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, data, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != ReadHoldingRegisters) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, ReadHoldingRegisters, ret_function, timeout);
    return MODBUS_ERROR;
  }  
  int j = 0;
  for(int i=0; i<ret; i+=2) 
  {
    registers[j++] = ((data[i] * 256) & 0x0ff) + (data[i+1] & 0x0ff);
  }  

  return ret;
}

int rlModbus::readInputRegisters(int slave, int start_adr, int number_of_registers, int *registers, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (start_adr / 256) & 0x0ff;
  data[1] = start_adr & 0x0ff;
  data[2] = (number_of_registers / 256) & 0x0ff;
  data[3] = number_of_registers & 0x0ff;
  ret = write(slave, ReadInputRegisters, data, 4);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, data, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != ReadInputRegisters) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, ReadInputRegisters, ret_function, timeout);
    return MODBUS_ERROR;
  }  
  int j=0;
  for(int i=0; i<ret; i+=2) 
  {
    registers[j++] = ((data[i] * 256) & 0x0ff) + (data[i+1] & 0x0ff);
  }  

  return ret;
}

int rlModbus::forceSingleCoil(int slave, int coil_adr, int value, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (coil_adr / 256) & 0x0ff;
  data[1] = coil_adr & 0x0ff;
  data[2] = 0;
  data[3] = 0;
  if(value) data[2] = 0x0ff;
  ret = write(slave, ForceSingleCoil, data, 4);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, data, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != ForceSingleCoil) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, ForceSingleCoil, ret_function, timeout);
    return MODBUS_ERROR;
  }  

  return ret;
}

int rlModbus::presetSingleRegister(int slave, int register_adr, int value, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (register_adr / 256) & 0x0ff;
  data[1] = register_adr & 0x0ff;
  data[2] = (value / 256) & 0x0ff;
  data[3] = value & 0x0ff;
  ret = write(slave, PresetSingleRegister, data, 4);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, data, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != PresetSingleRegister) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, PresetSingleRegister, ret_function, timeout);
    return MODBUS_ERROR;
  }  

  return ret;
}

int rlModbus::forceMultipleCoils(int slave, int coil_adr, int number_of_coils, unsigned char *coils, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (coil_adr / 256) & 0x0ff;
  data[1] = coil_adr & 0x0ff;
  data[2] = (number_of_coils / 256) & 0x0ff;
  data[3] = number_of_coils & 0x0ff;
  data[4] = (number_of_coils / 8) + 1;
  int i;
  for(i=0; i<data[4]; i++) data[5+i] = coils[i];
  ret = write(slave, ForceMultipleCoils, data, 5+i);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, data, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != ForceMultipleCoils) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, ForceMultipleCoils, ret_function, timeout);
    return MODBUS_ERROR;
  }  

  return ret;
}

int rlModbus::presetMultipleRegisters(int slave, int start_adr, int number_of_registers, int *registers, int timeout)
{
  int ret;
  int ret_slave, ret_function;
  unsigned char data[256];

  data[0] = (start_adr / 256) & 0x0ff;
  data[1] = start_adr & 0x0ff;
  data[2] = (number_of_registers / 256) & 0x0ff;
  data[3] = number_of_registers & 0x0ff;
  data[4] = (number_of_registers * 2) & 0x0ff;
  int j=5;
  for(int i=0; i<number_of_registers; i++)
  {
    data[j++] = (registers[i] / 256) & 0x0ff;
    data[j++] = registers[i] & 0x0ff;
  }
  ret = write(slave, PresetMultipleRegs, data, j);
  if(ret < 0) return MODBUS_ERROR;

  ret = response(&ret_slave, &ret_function, data, timeout);
  if(ret < 0 || ret_slave != slave || ret_function != PresetMultipleRegs) 
  {
    printf("rlMOdbus::ERROR response ret=%d slave=%d ret_slave=%d function=%d ret_function=%d timeout=%d\n", 
                                     ret, slave, ret_slave, PresetMultipleRegs, ret_function, timeout);
    return MODBUS_ERROR;
  }  

  return ret;
}


