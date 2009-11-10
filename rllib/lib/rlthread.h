/***************************************************************************
                          rlthread.h  -  description
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
#ifndef _RL_THREAD_H_
#define _RL_THREAD_H_

#include "rldefine.h"
#include "rlwthread.h"

class rlThread;

/*! <pre>
This parameter is given to the thread function.
</pre> */
typedef struct
{
  rlThread *thread;
  void     *user;
  int      running;
}
THREAD_PARAM;

/*! <pre>
Thread functions based on POSIX threads.
</pre> */
class rlThread
{
public:
  rlThread(int max_semphore=1000);
  virtual ~rlThread();
  /*! <pre>
  Create a new thread
  Your thread function looks like:

  void *threadFunction(void *arg)
  {
    THREAD_PARAM *p = (THREAD_PARAM *) arg;
    YOUR_DATA    *d = (YOUR_DATA *) p->user;

    for(int i=0; i<50; i++)
    {
      p->thread->lock();
      // do something critical
      printf("this is the thread\n");
      p->thread->unlock();
    }

    return NULL;
  }
  </pre> */
  int create(void *(*func)(void*), void *argument);

  /*! <pre>
  Try to lock the mutex.
  return 0 if already locked
  return !0 if lock sucessfull
  </pre> */
  int trylock();

  /*! <pre>
  Lock the mutex.
  </pre> */
  int lock();

  /*! <pre>
  Unlock the mutex.
  </pre> */
  int unlock();

  /*! <pre>
  Wait until semaphore is signaled
  </pre> */
  int waitSemaphore();

  /*! <pre>
  Increment the value of the semaphore
  </pre> */
  int incrementSemaphore();

  /*! <pre>
  Wait for termination of thread and get the exit status
  </pre> */
  int join(void **status);

  /*! <pre>
  Cancel the thread
  </pre> */
  int cancel();

  /*! <pre>
  Terminate the thread and return exit status
  </pre> */
  void threadExit(void *status);

  pthread_t       tid;
  pthread_attr_t  attr;
  pthread_mutex_t mutex;
  WSEMAPHORE      semaphore;
private:
  THREAD_PARAM    arg;
};

/*! <pre>
Mutex functions based on POSIX threads.
</pre> */
class rlMutex
{
public:
  rlMutex(const pthread_mutexattr_t *attr = NULL);
  virtual ~rlMutex();

  /*! <pre>
  Try to lock the mutex.
  return 0 if already locked
  return !0 if lock sucessfull
  </pre> */
  int trylock();

  /*! <pre>
  Lock the mutex.
  </pre> */
  int lock();

  /*! <pre>
  Unlock the mutex.
  </pre> */
  int unlock();

  pthread_mutex_t mutex;
};

/*! <pre>
Semaphore functions based on POSIX threads.
</pre> */
class rlSemaphore
{
public:
  rlSemaphore(int max_semaphore = 1000);
  virtual ~rlSemaphore();

  /*! <pre>
  Wait until semaphore is signaled
  </pre> */
  int waitSemaphore();

  /*! <pre>
  Increment the value of the semaphore
  </pre> */
  int incrementSemaphore();

  WSEMAPHORE      semaphore;
};

#endif
