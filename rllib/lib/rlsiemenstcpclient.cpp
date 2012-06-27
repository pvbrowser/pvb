/***************************************************************************
                 rlsiemenstcpclient.cpp  -  description
                             -------------------
    begin                : Wed Jan 07 2004
    copyright            : (C) 2004 by R. Lehrig
    email                : lehrig@t-online.de

    S7_200 update        : Wed Mar 21 2007
    copyright            : (C) 2007 by Aljosa Merljak
    Email                : aljosa.merljak@datapan.si
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include <string.h>
#include "rlsiemenstcpclient.h"
#include "rlsiemenstcp.h"
#define BUFSIZE 2048

typedef union
{
  unsigned char  b[4];
  unsigned short us[2];
  short          s[2];
  unsigned int   ui;
  int            i;
  float          f;
}SWAP;

rlSiemensTCPClient::rlSiemensTCPClient(const char *mbxname, const char *shmname, int shmsize, int _have_to_swap)
                   :rlMailbox(mbxname), rlSharedMemory(shmname, shmsize)
{
  have_to_swap = _have_to_swap;
}

rlSiemensTCPClient::~rlSiemensTCPClient()
{
}

int rlSiemensTCPClient::write(int slave, int org, int dbnr, int start_adr, int len, const unsigned char *inbuf, int function=rlSiemensTCP::WriteByte)
{
  unsigned char buf[BUFSIZE+8];
  int len_byte;

  if(len <= 0)     return -1;
  if(slave < 0)    return -1;
  if(slave >= 256) return -1;
  len_byte = len;
  buf[0] = slave;
  buf[1] = org;
  buf[2] = dbnr / 256;
  buf[3] = dbnr & 0x0ff;
  buf[4] = start_adr / 256;
  buf[5] = start_adr & 0x0ff;
  buf[6] = len / 256;
  buf[7] = len & 0x0ff;
  buf[8] = function;
  memcpy(&buf[9],inbuf,len_byte);
  rlMailbox::write((void *) buf, len_byte + 9);
  return 0;
}

int rlSiemensTCPClient::writeBit(int slave, int org, int dbnum, int start, int offset, int len, const unsigned char *val)
{
  if(len < 0)       return -1;
  if(len > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  int start_adr = start * 8 + offset;
  return write(slave,org,dbnum,start_adr,len,val,rlSiemensTCP::WriteBit);
}

int rlSiemensTCPClient::writeByte(int slave, int org, int dbnum, int start, int len, const unsigned char *val)
{
  if(len < 0)       return -1;
  if(len > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  return write(slave,org,dbnum,start,len,val,rlSiemensTCP::WriteByte);
}

int rlSiemensTCPClient::writeFloat(int slave, int org, int dbnum, int start, int len, const float *val)
{
  SWAP swap;
  int i,len_byte;
  unsigned char buf[BUFSIZE];

  len_byte = len * 4;
  if(len_byte < 0)       return -1;
  if(len_byte > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len_byte; i+=4)
    {
      swap.f   = val[i/4];
      buf[i+3] = swap.b[0];  
      buf[i+2] = swap.b[1];  
      buf[i+1] = swap.b[2];  
      buf[i]   = swap.b[3];  
    }
  }
  else
  {
    for(i=0; i<len_byte; i+=4)
    {
      swap.f   = val[i/4];
      buf[i]   = swap.b[0];  
      buf[i+1] = swap.b[1];  
      buf[i+2] = swap.b[2];  
      buf[i+3] = swap.b[3];       
    }
  }  
  return write(slave,org,dbnum,start,len_byte,buf,rlSiemensTCP::WriteByte);
}

int rlSiemensTCPClient::writeDword(int slave, int org, int dbnum, int start, int len, const int *val)
{
  SWAP swap;
  int i,len_byte;
  unsigned char buf[BUFSIZE];
  
  len_byte = len * 4;
  if(len_byte < 0)       return -1;
  if(len_byte > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len_byte; i+=4)
    {
      swap.i   = val[i/4];
      buf[i+3] = swap.b[0];  
      buf[i+2] = swap.b[1];  
      buf[i+1] = swap.b[2];  
      buf[i]   = swap.b[3];  
    }
  }  
  else
  {
    for(i=0; i<len_byte; i+=4)
    {
      swap.i   = val[i/4];
      buf[i]   = swap.b[0];  
      buf[i+1] = swap.b[1];  
      buf[i+2] = swap.b[2];  
      buf[i+3] = swap.b[3];       
    }
  }  
  return write(slave,org,dbnum,start,len_byte,buf,rlSiemensTCP::WriteByte);
}

int rlSiemensTCPClient::writeShort(int slave, int org, int dbnum, int start, int len, const short *val)
{
  SWAP swap;
  int i,len_byte;
  unsigned char buf[BUFSIZE];
  
  len_byte = len * 2;
  if(len_byte < 0)       return -1;
  if(len_byte > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len_byte; i+=2)
    {
      swap.s[0] = val[i/2];
      buf[i+1]  = swap.b[0];  
      buf[i]    = swap.b[1];  
    }
  }  
  else
  {
    for(i=0; i<len_byte; i+=2)
    {
      swap.s[0] = val[i/2];
      buf[i]    = swap.b[0];  
      buf[i+1]  = swap.b[1];  
    }
  }  
  return write(slave,org,dbnum,start,len_byte,buf,rlSiemensTCP::WriteByte);
}

int rlSiemensTCPClient::writeUDword(int slave, int org, int dbnum, int start, int len, const unsigned int *val)
{
  SWAP swap;
  int i,len_byte;
  unsigned char buf[BUFSIZE];
  
  len_byte = len * 4;
  if(len_byte < 0)       return -1;
  if(len_byte > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len_byte; i+=4)
    {
      swap.ui  = val[i/4];
      buf[i+3] = swap.b[0];  
      buf[i+2] = swap.b[1];  
      buf[i+1] = swap.b[2];  
      buf[i]   = swap.b[3];  
    }
  }  
  else
  {
    for(i=0; i<len_byte; i+=4)
    {
      swap.ui  = val[i/4];
      buf[i]   = swap.b[0];  
      buf[i+1] = swap.b[1];  
      buf[i+2] = swap.b[2];  
      buf[i+3] = swap.b[3];       
    }
  }  
  return write(slave,org,dbnum,start,len_byte,buf,rlSiemensTCP::WriteByte);
}

int rlSiemensTCPClient::writeUShort(int slave, int org, int dbnum, int start, int len, const unsigned short *val)
{
  SWAP swap;
  int i,len_byte;
  unsigned char buf[BUFSIZE];
  
  len_byte = len * 2;
  if(len_byte < 0)       return -1;
  if(len_byte > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 255)   return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len_byte; i+=2)
    {
      swap.us[0] = val[i/2];
      buf[i+1]   = swap.b[0];  
      buf[i]     = swap.b[1];  
    }
  }
  else
  {
    for(i=0; i<len_byte; i+=2)
    {
      swap.us[0] = val[i/2];
      buf[i]     = swap.b[0];  
      buf[i+1]   = swap.b[1];  
    }
  }      
  return write(slave,org,dbnum,start,len_byte,buf,rlSiemensTCP::WriteByte);
}

int rlSiemensTCPClient::read(int offset, int len)
{
  return rlSharedMemory::read(offset,buf,len);
}

float rlSiemensTCPClient::Float(int index)
{
  SWAP swap;
  if(index*4+4 > (int) BUFSIZE) return 0.0f;
  if(have_to_swap == 1)
  {
    swap.b[0] = buf[4*index+3];  
    swap.b[1] = buf[4*index+2];  
    swap.b[2] = buf[4*index+1];  
    swap.b[3] = buf[4*index];
  }
  else
  {
    swap.b[0] = buf[4*index];  
    swap.b[1] = buf[4*index+1];  
    swap.b[2] = buf[4*index+2];  
    swap.b[3] = buf[4*index+3];
  }    
  return swap.f;
}

int rlSiemensTCPClient::Dword(int index)
{
  SWAP swap;
  if(index*4+4 > (int) BUFSIZE) return 0;
  if(have_to_swap == 1)
  {
    swap.b[0] = buf[4*index+3];  
    swap.b[1] = buf[4*index+2];  
    swap.b[2] = buf[4*index+1];  
    swap.b[3] = buf[4*index];
  }    
  else
  {
    swap.b[0] = buf[4*index];  
    swap.b[1] = buf[4*index+1];  
    swap.b[2] = buf[4*index+2];  
    swap.b[3] = buf[4*index+3];
  }    
  return swap.i;
}

int rlSiemensTCPClient::Short(int index)
{
  SWAP swap;
  if(index*2+2 > (int) BUFSIZE) return 0;
  if(have_to_swap == 1)
  {
    swap.b[0] = buf[2*index+1];  
    swap.b[1] = buf[2*index];  
  }    
  else
  {
    swap.b[0] = buf[2*index];  
    swap.b[1] = buf[2*index+1];  
  }    
  return swap.s[0];
}

unsigned int rlSiemensTCPClient::UDword(int index)
{
  SWAP swap;
  if(index*4+4 > (int) BUFSIZE) return 0;
  if(have_to_swap == 1)
  {
    swap.b[0] = buf[4*index+3];  
    swap.b[1] = buf[4*index+2];  
    swap.b[2] = buf[4*index+1];  
    swap.b[3] = buf[4*index];  
  }  
  else
  {
    swap.b[0] = buf[4*index];  
    swap.b[1] = buf[4*index+1];  
    swap.b[2] = buf[4*index+2];  
    swap.b[3] = buf[4*index+3];
  }    
  return swap.ui;
}

unsigned int rlSiemensTCPClient::UShort(int index)
{
  SWAP swap;
  if(index*2+4 > (int) BUFSIZE) return 0;
  if(have_to_swap == 1)
  {
    swap.b[0] = buf[2*index+1];  
    swap.b[1] = buf[2*index];  
  }    
  else
  {
    swap.b[0] = buf[2*index];  
    swap.b[1] = buf[2*index+1];  
  }    
  return swap.us[0];
}
