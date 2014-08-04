/***************************************************************************
                          rlfifo.cpp  -  description
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
#include <stdio.h>
#include <stdarg.h>
#include "rlfifo.h"
#include "rlcutil.h"
#include "string.h"

rlFifo::rlFifo(int maxmessages)
{
  maxmes = maxmessages;
  nmes = 0;
  list = NULL;
  rlwthread_mutex_init(&mutex, NULL);
  rlwrapinit_semaphore(&semaphore, 1000);
}

rlFifo::~rlFifo()
{
MessageList *ptr,*lastptr;

  ptr = list;
  if(ptr != NULL)
  {
    do
    {
      lastptr = ptr;
      ptr = ptr->next;
      delete [] lastptr->mes;
      delete lastptr;
    }
    while(ptr != NULL);
  }
  rlwthread_mutex_destroy(&mutex);
  rlwrapdestroy_semaphore(&semaphore);
}

int rlFifo::read(void *buf, int maxlen)
{
  int retlen;
  MessageList *ptr;
  char *cbuf;

  cbuf = (char *) buf;
  rlwrapwait_semaphore(&semaphore);
  rlwthread_mutex_lock(&mutex);

  if(list->len > maxlen)
  {
    rlwrapincrement_semaphore(&semaphore);
    rlwthread_mutex_unlock(&mutex);
    return MESSAGE_TO_BIG;
  }

  ptr = list;
  if(ptr->next == NULL)
  {
    retlen = ptr->len;
    memcpy(buf,ptr->mes,retlen);
    delete [] ptr->mes;
    delete ptr;
    list = NULL;
    nmes--;
    rlwthread_mutex_unlock(&mutex);
    if(retlen < maxlen && retlen >= 0) cbuf[retlen] = '\0';
    return retlen;
  }
  ptr = ptr->next;
  retlen = list->len;
  memcpy(buf,list->mes,retlen);
  delete [] list->mes;
  delete list;
  list = ptr;
  nmes--;
  rlwthread_mutex_unlock(&mutex);
  if(retlen < maxlen && retlen >= 0) cbuf[retlen] = '\0';
  return retlen;
}

int rlFifo::poll()
{
  if(nmes > 0) return DATA_AVAILABLE;
  return              NO_DATA_AVAILABLE;
}

int rlFifo::nmesAvailable()
{
  if(nmes > 0) return nmes;
  return 0;
}

int rlFifo::write(const void *buf, int len)
{
  MessageList *ptr;

  if(maxmes == 0);
  else if(nmes >= maxmes) return FIFO_FULL;

  rlwthread_mutex_lock(&mutex);
  if(list == NULL)
  {
    list = new MessageList;
    list->mes  = new char [len];
    list->len  = len;
    list->next = NULL;
    nmes++;
    memcpy(list->mes,buf,len);
    rlwthread_mutex_unlock(&mutex);
    rlwrapincrement_semaphore(&semaphore);
    return len;
  }

  // go to end of list
  ptr = list;
  while(ptr->next != NULL) ptr = ptr->next;

  ptr->next = new MessageList;
  ptr = ptr->next;
  ptr->mes  = new char [len];
  ptr->len  = len;
  ptr->next = NULL;
  nmes++;
  memcpy(ptr->mes,buf,len);
  rlwthread_mutex_unlock(&mutex);
  rlwrapincrement_semaphore(&semaphore);
  return len;
}

int rlFifo::printf(const char *format, ...)
{
  int ret;
  char message[rl_PRINTF_LENGTH]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  return write(message,strlen(message));
}
