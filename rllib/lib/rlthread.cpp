/***************************************************************************
                          rlthread.cpp  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by Rainer Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlthread.h"

rlThread::rlThread(int max_semaphore)
{
  rlwthread_attr_init(&attr);
  arg.thread = this;
  arg.user   = NULL;
  rlwthread_mutex_init(&mutex, NULL);
  rlwrapinit_semaphore(&semaphore, max_semaphore);
  arg.running = 0;
}

rlThread::~rlThread()
{
  arg.running = 0;
  rlwthread_mutex_destroy(&mutex);
  rlwrapdestroy_semaphore(&semaphore);
}

int rlThread::create(void *(*func)(void*), void *user)
{
  arg.user = user;
  arg.running = 1;
  return rlwthread_create(&tid, &attr, func, &arg);
}

int rlThread::trylock()
{
  return rlwthread_mutex_trylock(&mutex);
}

int rlThread::lock()
{
  return rlwthread_mutex_lock(&mutex);
}

int rlThread::unlock()
{
  return rlwthread_mutex_unlock(&mutex);
}

int rlThread::waitSemaphore()
{
  return rlwrapwait_semaphore(&semaphore);
}

int rlThread::incrementSemaphore()
{
  return rlwrapincrement_semaphore(&semaphore);
}

void rlThread::threadExit(void *status)
{
  arg.running = 0;
  rlwthread_exit(status);
}

int rlThread::join(void **status)
{
  return rlwthread_join(tid, status);
}

int rlThread::cancel()
{
  arg.running = 0;
  return rlwthread_cancel(tid);
}

//------------ class rlMutex ----------------------------------------------
rlMutex::rlMutex(const pthread_mutexattr_t *attr)
{
  rlwthread_mutex_init(&mutex, attr);
}

rlMutex::~rlMutex()
{
  rlwthread_mutex_destroy(&mutex);
}

int rlMutex::trylock()
{
  return rlwthread_mutex_trylock(&mutex);
}

int rlMutex::lock()
{
  return rlwthread_mutex_lock(&mutex);
}

int rlMutex::unlock()
{
  return rlwthread_mutex_unlock(&mutex);
}

//------------ class rlSemaphore ------------------------------------------
rlSemaphore::rlSemaphore(int max_semaphore)
{
  rlwrapinit_semaphore(&semaphore, max_semaphore);
}

rlSemaphore::~rlSemaphore()
{
  rlwrapdestroy_semaphore(&semaphore);
}

int rlSemaphore::waitSemaphore()
{
  return rlwrapwait_semaphore(&semaphore);
}

int rlSemaphore::incrementSemaphore()
{
  return rlwrapincrement_semaphore(&semaphore);
}
