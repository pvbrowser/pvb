/***************************************************************************
                        rl3964r.cpp  -  description
                             -------------------
    begin                : Wed Jan 14 2004
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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "rlcutil.h"
#include "rl3964r.h"
#define STX 2
#define ETX 3
#define ENQ 5
#define EOT 4
#define ETB 23
#define DLE 16
#define NAK 21
#define ACK 6

enum State
{
  IDLE            = 0,
  SENDING         = 1,
  RECEIVING       = 2,
  WANT_TO_SEND    = 3,
  WANT_TO_RECEIVE = 4
};

static const char *statestr[] = {"idle","sending","receiving","want_to_send","want_to_receive"};

static void *receiverThread(void *arg)
{
  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  rl3964R      *d = (rl3964R *) p->user;
  unsigned char c;
  int ret,send_retry;

  send_retry = 0;
  while(d->run)
  {
    c = 0x0ff;
    ret = d->tty.select(1000);
    if(ret == 1) c = d->tty.readChar();
    p->thread->lock();

    d->dprintf("receiverThread: c=%d state=%s\n",c,statestr[d->state]);
    if(c==STX && d->state==IDLE)
    {
      d->dprintf("STX IDLE\n");
      d->state = RECEIVING;
      d->dprintf("write DLE\n");
      d->tty.writeChar(DLE);
      d->dprintf("call receive\n");
      d->receive();
      d->dprintf("after receive\n");
      d->state = IDLE;
    }
    else if(c==STX && d->state==WANT_TO_SEND && d->priority==rl3964R::lowPriority)
    {
      d->dprintf("receiverThread: STX WANT_TO_SEND lowPriority\n");
      d->state = RECEIVING;
      d->tty.writeChar(DLE);
      d->receive();
      d->state = WANT_TO_SEND;
    }
    else if(c==STX && d->state==WANT_TO_SEND && d->priority==rl3964R::highPriority)
    {
      d->dprintf("receiverThread: STX WANT_TO_SEND highPriority\n");
      d->tty.writeChar(NAK);
      d->tty.writeChar(STX);
    }
    else if(c==DLE && d->state==WANT_TO_SEND)
    {
      d->dprintf("receiverThread: DLE WANT_TO_SEND\n");
      d->send();
      d->state = IDLE;
      d->receiver.incrementSemaphore();
    }
    else if(c==NAK)
    {
      d->dprintf("receiverThread: NAK\n");
    }
    else if(d->state==WANT_TO_SEND)
    {
      if(send_retry < 1)
      {
      }
      else if(send_retry < 3)
      {
        d->tty.writeChar(STX);
        send_retry++;
        d->dprintf("receiverThread: WANT_TO_SEND send=STX retry=%d\n",send_retry+1);
      } 
      else
      {
        send_retry = 0;
        d->dprintf("receiverThread: WANT_TO_SEND failed after 3 retries\n");
        d->state = IDLE;
        d->receiver.incrementSemaphore();
      }
    }
    else
    {
      d->dprintf("receiverThread: IDLE\n");
      d->state = IDLE;
    }

    p->thread->unlock();
  }

  return NULL;
}

rl3964R::rl3964R(int _priority)
{
  priority = _priority;
  readCallback = NULL;
  debug = 0;
  isOpen = 0;
  state = IDLE;
  send_result = 0;
  tel_receive_length = tel_send_length = -1;
}

rl3964R::~rl3964R()
{
  close();
}

int rl3964R::open(const char *devicename, int _baudrate)
{
  int ret;

  if(isOpen == 0)
  {
    //int openDevice(const char *devicename, int speed=B9600, int block=1, int rtscts=1, int bits=8, int stopbits=1, int parity=NONE);
    ret = tty.openDevice(devicename,_baudrate,1,0,8,1,rlSerial::EVEN);
    if(ret >= 0)
    {
      isOpen = 1;
      run = 1;
      receiver.create(receiverThread,this);
    }
    return ret;
  }
  return -1;
}

int rl3964R::close()
{
  if(isOpen == 1)
  {
    isOpen = 0;
    run = 0;
    dprintf("close(): cancel\n");
    receiver.cancel();
    dprintf("close(): closeDevice\n");
    tty.closeDevice();
    dprintf("close(): return\n");
    return 0;
  }
  isOpen = 0;
  return -1;
}

int rl3964R::setReadCallback( void (*_readCallback)(const unsigned char *buf, int len))
{
  readCallback = _readCallback;
  return 0;
}

int rl3964R::write(const unsigned char *buf, int len)
{
  dprintf("write() len=%d\n",len);
  if(len >= (int) sizeof(tel_send)) return -1;
  receiver.lock();
  tel_send_length = len;
  memcpy(tel_send,buf,len);
  state = WANT_TO_SEND;
  dprintf("write() STX\n");
  tty.writeChar(STX);
  dprintf("write() unlock\n");
  receiver.unlock();
  dprintf("write() waitSemaphore\n");
  receiver.waitSemaphore();
  dprintf("write() return len=%d\n",len);
  if(send_result < 0) return -1;
  return len;
}

int rl3964R::send()
{
  int i,bcc,c,ret;

  dprintf("send()");
  //bcc = STX;
  bcc = 0;
  for(i=0; i<tel_send_length; i++)
  {
    switch(tel_send[i])
    {
      case DLE:
        tty.writeChar(DLE);
        tty.writeChar(DLE);
        bcc = bcc ^ DLE;
        bcc = bcc ^ DLE;
        break;
      default:
        tty.writeChar(tel_send[i]);
        bcc = bcc ^ tel_send[i];
        break;
    }
  }
  tty.writeChar(DLE);
  bcc = bcc ^ DLE;
  tty.writeChar(ETX);
  bcc = bcc ^ ETX;
  tty.writeChar(bcc);
  ret = tty.select(1000);
  if(ret == 1) c = tty.readChar();
  else
  {
    send_result = -1;
    dprintf("send(): partner was sending nothing\n");
    return -1;
  }  
  if(c == DLE)
  {
    send_result = 0;
    dprintf(" success\n");
    return tel_send_length;
  }
  send_result = -1;
  dprintf(" failure\n");
  return -1;
}

int rl3964R::receive()
{
  int i,c,c2,bcc,received_bcc,ret;

  dprintf("receive()\n");
  //bcc = STX;
  bcc = 0;
  i = received_bcc = 0;
  tel_receive[i++] = c =  STX;
  while(c > 0 && i < (int) sizeof(tel_receive))
  {
    ret = tty.select(1000);
    if(ret == 1) c = tty.readChar();
    else
    {
      dprintf("receive(): partner was sending nothing\n");
      return -1;
    }  
    dprintf(" %x\n",c);
    switch(c)
    {
      case -1:
      case -2:
        return -1;
      case DLE:
        bcc = bcc ^ c;
        c2 = tty.readChar();
        dprintf(" %x\n",c2);
        bcc = bcc ^ c2;
        if(c2 < 0) return -1;
        tel_receive[i++] = c2;
        if(c2 == ETX)
        {
          c2 = tty.readChar();
          dprintf(" %x\n",c2);
          if(c2 < 0) return -1;
          tel_receive[i++] = c2; // bcc
          received_bcc = c2;
          dprintf(" bcc=%d received_bcc=%d\n",bcc,received_bcc);
          c = -1;         
        }
        break;
      default:
        bcc = bcc ^ c;
        tel_receive[i++] = c;
        break;
    }
  }
  tel_receive_length = i;

  if(bcc == received_bcc)
  {
    tty.writeChar(DLE);
    dprintf(" success\n");
    if(readCallback != NULL) (readCallback)(&tel_receive[1],tel_receive_length-3);
    return tel_receive_length-3;
  }
  tty.writeChar(NAK);
  dprintf(" failure\n");
  return -1;
}

int rl3964R::dprintf(const char *format, ...)
{
  char message[rl_PRINTF_LENGTH]; // should be big enough
  int ret;

  if(debug != 1) return 0;
  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  printf("%s",message);
  return ret;
}

