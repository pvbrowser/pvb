/***************************************************************************
                          wthread.h  -  description
                             -------------------
    begin                : Sun Jan 02 2000
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
/***********************************************************************************

Wrapper for posix threads (UNIX,VMS,windows)

(C) R. Lehrig 2001                                       lehrig@t-online.de

***********************************************************************************/

#ifndef _RL_WTHREAD_H_
#define _RL_WTHREAD_H_

#ifndef SWIG

#include "rldefine.h"

#ifdef RLWIN32
#define WTREAD_GNUC10 ( __GNUC__ * 1000 ) + __GNUC_MINOR__
#if WTREAD_GNUC10 < 4008
#define RLWIN32THREAD
#else
#include <pthread.h>
#endif
#endif

#ifdef RLWIN32THREAD

#include <windows.h>
#include <winbase.h>
#include <stddef.h>
#include <string.h>

#ifndef _WRAPTHREAD_
#ifndef _WTHREAD_H_
typedef unsigned long int pthread_t;

/* Attributes for threads */
typedef struct __sched_param
{
  int sched_priority;
}SCHED_PARAM;

typedef struct
{
  int     __detachstate;
  int     __schedpolicy;
  struct  __sched_param __schedparam;
  int     __inheritsched;
  int     __scope;
  size_t  __guardsize;
  int     __stackaddr_set;
  void   *__stackaddr;
  size_t  __stacksize;
}pthread_attr_t;

typedef HANDLE pthread_mutex_t;
//old typedef CRITICAL_SECTION pthread_mutex_t;
typedef long             pthread_mutexattr_t;
#endif
#endif

#else  /* VMS or UNIX or new GCC on Windows*/
#ifndef WIN_PTHREADS_H
#include <pthread.h>
#endif
#endif /* end of MSWINDOWS */

#ifndef _WRAPTHREAD_
#ifndef _WTHREAD_H_
typedef struct
{
#ifdef RLWIN32THREAD
  int    cmax;
  HANDLE hSemaphore;
#else
  int              cmax;
  int              nready;
  pthread_mutex_t  mutex;
  pthread_cond_t   cond;
#endif
}WSEMAPHORE;
#endif
#endif

/* function prototypes */
//#ifdef __cplusplus
//extern "C" {
//#endif
int  rlwthread_attr_init(pthread_attr_t *attr);
int  rlwthread_create(pthread_t *tid, const pthread_attr_t *attr,
                      void *(*func)(void*), void *arg);
void rlwthread_close_handle(pthread_t *tid);
void rlwthread_exit(void *status);
int  rlwthread_join(pthread_t tid, void **status);
int  rlwthread_mutex_init(pthread_mutex_t *mptr, const pthread_mutexattr_t *attr);
int  rlwthread_mutex_destroy(pthread_mutex_t *mptr);
int  rlwthread_mutex_lock(pthread_mutex_t *mptr);
int  rlwthread_mutex_trylock(pthread_mutex_t *mptr);
int  rlwthread_mutex_unlock(pthread_mutex_t *mptr);
int  rlwthread_cancel(pthread_t tid);
int  rlwrapinit_semaphore(WSEMAPHORE *s, int cmax);
int  rlwrapdestroy_semaphore(WSEMAPHORE *s);
int  rlwrapincrement_semaphore(WSEMAPHORE *s);
int  rlwrapwait_semaphore(WSEMAPHORE *s);
int  rlwthread_sleep(long msec);
void rlsleep(long msec);
//#ifdef __cplusplus
//};
//#endif

#else
// SWIG
void rlsleep(long msec);
#endif

#endif
