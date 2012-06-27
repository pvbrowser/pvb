/***************************************************************************
                   rlppiclient.cpp  -  description
                             -------------------
    begin                : Mon Jul 12 2004
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
#include "rlppiclient.h"
#include <stdio.h>
#include <string.h>
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

rlPPIClient::rlPPIClient(const char *mbxname, const char *shmname, int shmsize, int _have_to_swap)
            :rlMailbox(mbxname), rlSharedMemory(shmname, shmsize)
{
  have_to_swap = _have_to_swap;
}

rlPPIClient::~rlPPIClient()
{
}

int rlPPIClient::write(int slave, int area, int dbnum, int start, int len, const unsigned char *data)
{
  unsigned char buf[BUFSIZE+8];
  
  if(len < 0)        return -1;
  if(len > BUFSIZE)  return -1;
  if(slave < 0)      return -1;
  if(slave > 31)     return -1;
      
  buf[0] = slave;
  buf[1] = area;
  buf[2] = dbnum / 256;
  buf[3] = dbnum & 0x0ff;
  buf[4] = start / 256;
  buf[5] = start & 0x0ff;
  buf[6] = len / 256;
  buf[7] = len & 0x0ff;
  memcpy(&buf[8],data,len);
  rlMailbox::write((void *) buf,len+8);
  return 0;
}
    
int rlPPIClient::writeFloat(int slave, int area, int dbnum, int start, int len, const float *val)
{
  SWAP swap;
  int i;
  unsigned char buf[BUFSIZE];
  
  if(len < 0)       return -1;
  if(len > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 31)    return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len; i+=4)
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
    for(i=0; i<len; i+=4)
    {
      swap.f   = val[i/4];
      buf[i]   = swap.b[0];  
      buf[i+1] = swap.b[1];  
      buf[i+2] = swap.b[2];  
      buf[i+3] = swap.b[3];       
    }
  }  
  return write(slave,area,dbnum,start,len,buf);
}

int rlPPIClient::writeDword(int slave, int area, int dbnum, int start, int len, const int *val)
{
  SWAP swap;
  int i;
  unsigned char buf[BUFSIZE];
  
  if(len < 0)       return -1;
  if(len > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 31)    return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len; i+=4)
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
    for(i=0; i<len; i+=4)
    {
      swap.i   = val[i/4];
      buf[i]   = swap.b[0];  
      buf[i+1] = swap.b[1];  
      buf[i+2] = swap.b[2];  
      buf[i+3] = swap.b[3];       
    }
  }  
  return write(slave,area,dbnum,start,len,buf);
}

int rlPPIClient::writeShort(int slave, int area, int dbnum, int start, int len, const short *val)
{
  SWAP swap;
  int i;
  unsigned char buf[BUFSIZE];
  
  if(len < 0)       return -1;
  if(len > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 31)    return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len; i+=2)
    {
      swap.s[0] = val[i/2];
      buf[i+1]  = swap.b[0];  
      buf[i]    = swap.b[1];  
    }
  }  
  else
  {
    for(i=0; i<len; i+=2)
    {
      swap.s[0] = val[i/2];
      buf[i]    = swap.b[0];  
      buf[i+1]  = swap.b[1];  
    }
  }  
  return write(slave,area,dbnum,start,len,buf);
}

int rlPPIClient::writeUDword(int slave, int area, int dbnum, int start, int len, const unsigned int *val)
{
  SWAP swap;
  int i;
  unsigned char buf[BUFSIZE];
  
  if(len < 0)       return -1;
  if(len > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 31)    return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len; i+=4)
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
    for(i=0; i<len; i+=4)
    {
      swap.ui  = val[i/4];
      buf[i]   = swap.b[0];  
      buf[i+1] = swap.b[1];  
      buf[i+2] = swap.b[2];  
      buf[i+3] = swap.b[3];       
    }
  }  
  return write(slave,area,dbnum,start,len,buf);
}

int rlPPIClient::writeUShort(int slave, int area, int dbnum, int start, int len, const unsigned short *val)
{
  SWAP swap;
  int i;
  unsigned char buf[BUFSIZE];
  
  if(len < 0)       return -1;
  if(len > BUFSIZE) return -1;
  if(slave < 0)     return -1;
  if(slave > 31)    return -1;
  if(have_to_swap == 1)
  {
    for(i=0; i<len; i+=2)
    {
      swap.us[0] = val[i/2];
      buf[i+1]   = swap.b[0];  
      buf[i]     = swap.b[1];  
    }
  }
  else
  {
    for(i=0; i<len; i+=2)
    {
      swap.us[0] = val[i/2];
      buf[i]     = swap.b[0];  
      buf[i+1]   = swap.b[1];  
    }
  }      
  return write(slave,area,dbnum,start,len,buf);
}

int rlPPIClient::read(int offset, int len)
{
  return rlSharedMemory::read(offset,buf,len);
}

float rlPPIClient::Float(int index)
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

int rlPPIClient::Dword(int index)
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

int rlPPIClient::Short(int index)
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
    swap.b[0] = buf[4*index];  
    swap.b[1] = buf[4*index+1];  
  }    
  return swap.s[0];
}

unsigned int rlPPIClient::UDword(int index)
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

unsigned int rlPPIClient::UShort(int index)
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
    swap.b[0] = buf[4*index];  
    swap.b[1] = buf[4*index+1];  
  }    
  return swap.us[0];
}
