/***************************************************************************
                          rlfifo.h  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_FIFO_H_
#define _RL_FIFO_H_

#include "rldefine.h"
#include "rlthread.h"

/*! <pre>
class for a fifo. (first in first out)
</pre> */
class rlFifo
{
private:
  typedef struct _MessageList_
  {
    char *mes;
    int  len;
    struct _MessageList_ *next;
  }MessageList;

public:
  rlFifo(int maxmessages=0);
  virtual ~rlFifo();

  enum FifoEnum {
       DATA_AVAILABLE=-1, NO_DATA_AVAILABLE=-2, MESSAGE_TO_BIG=-3, FIFO_FULL=-4
  };

  int read(void *buf, int maxlen);
  int poll();
  int nmesAvailable();
  int write(const void *buf, int len);
  int printf(const char *format, ...);

private:
  int maxmes;
  int nmes;
  MessageList *list;
  WSEMAPHORE semaphore;
  pthread_mutex_t mutex;
};

#endif
