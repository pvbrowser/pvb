/***************************************************************************
                          wthread.c  -  description
                             -------------------
    begin                : Sun Jan 02 2000
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
/***********************************************************************************

Wrapper for posix threads (UNIX,VMS,windows)

(C) Rainer Lehrig 2000                                       lehrig@t-online.de

***********************************************************************************/
#include <errno.h>
#include "rlwthread.h"
#ifdef RLUNIX
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#endif

/***************************************/
/* init attributes with default values */
/***************************************/
int rlwthread_attr_init(pthread_attr_t *attr)
{
#ifdef RLWIN32
  memset(attr,0,sizeof(pthread_attr_t));
  return 0;
#else
  return pthread_attr_init(attr);
#endif
}

/* #pdoku headerlevel 2 ^^wthread_create */
/*******************************************/
/* create a thread                         */
/* tid:  returned thread_id                */
/* attr: thread creation attributes        */
/* func: thread start routine              */
/* arg:  parameter to thread start routine */
/*******************************************/
int rlwthread_create(pthread_t *tid, const pthread_attr_t *attr,
                      void *(*func)(void*), void *arg)
{
#ifdef RLWIN32
  HANDLE handle;
  int ThreadId;
  int dwStackSize = 0;
  if(attr != NULL) dwStackSize = attr->__stacksize;
  handle = CreateThread( NULL,                    /* pointer to thread security attributes */
                         dwStackSize,             /* initial thread stack size, in bytes   */
 (LPTHREAD_START_ROUTINE)func,                    /* pointer to thread function            */
                         arg,                     /* argument for new thread               */
                         0,                       /* creation flags                        */
       (unsigned long *) &ThreadId                /* pointer to returned thread identifier */
                       );
  *tid = (pthread_t) handle;
  if(handle == NULL) return -1;
  else               return 0;
#else
  int ret = pthread_create(tid,attr,func,arg);
  pthread_detach(*tid);
  return ret;
#endif
}

void rlwthread_close_handle(pthread_t *tid)
{
#ifdef RLWIN32
  CloseHandle((HANDLE) *tid);
#else
  if(tid == NULL) return;
#endif
}

/**************************************************************/
/* terminate a thread                                         */
/* call this function within a thread to terminate the thead  */
/* or return from the thread start routine                    */
/* status: return value from the thread (see wthread_join)    */
/**************************************************************/
void rlwthread_exit(void *status)
{
#ifdef RLWIN32
  DWORD *ptr;
  ptr = (DWORD *) status;
  if(status == NULL) ExitThread((DWORD) 0);
  else               ExitThread(*ptr);
#else
  pthread_exit(status);
#endif
}

/* #pdoku headerlevel 2 ^^wthread_join */
/************************************/
/* wait for termination of thread   */
/* tid:    thread id                */
/* status: return value from thread */
/************************************/
#define USE_OLD_JOIN
int rlwthread_join(pthread_t tid, void **status)
{
#ifdef RLWIN32

#ifdef USE_OLD_JOIN
  DWORD exitcode;
  while(1)
  {
    GetExitCodeThread((HANDLE) tid,&exitcode);
    if(exitcode != STILL_ACTIVE) return exitcode;
    Sleep(10); /* sleep 10 msec */
  }
#else
  int result = 1;
  DWORD exitcode;
  DWORD dwWait = WaitForSingleObject((HANDLE) tid, INFINITE);
  if(dwWait == WAIT_OBJECT_0)
  {
    if(GetExitCodeThread((HANDLE) tid, &exitcode) == TRUE)
    {
      result = 0;
      *status = (int) exitcode; // ???
    }
    else
    {
      result = GetLastError();
    }
  }
  else if(dwWait == WAIT_FAILED)
  {
    result = GetLastError();
  }
  return result;
#endif

#else
  return pthread_join(tid,status);
#endif
}

/**************************/
/* init a mutex           */
/* mptr: pointer to mutex */
/* atrr: mutex atrributes */
/**************************/
int rlwthread_mutex_init(pthread_mutex_t *mptr,
                          const pthread_mutexattr_t *attr)
{
#ifdef RLWIN32
  HANDLE handle = CreateMutex(NULL, FALSE, NULL);
  if(handle) *mptr = handle;
  //old InitializeCriticalSection(mptr);
  return 0;
#else
  return pthread_mutex_init(mptr,attr);
#endif
}

/**************************/
/* destroy a mutex        */
/* mptr: pointer to mutex */
/**************************/
int rlwthread_mutex_destroy(pthread_mutex_t *mptr)
{
#ifdef RLWIN32
  CloseHandle(*mptr);
  //old DeleteCriticalSection(mptr);
  return 0;
#else
  return pthread_mutex_destroy(mptr);
#endif
}

/**************************/
/* lock a mutex           */
/* mptr: pointer to mutex */
/**************************/
int rlwthread_mutex_lock(pthread_mutex_t *mptr)
{
#ifdef RLWIN32
  if(WaitForSingleObject(*mptr, INFINITE) == WAIT_OBJECT_0) return 0;
  //old EnterCriticalSection(mptr); // pointer to critical section object
  return 0;
#else
  return pthread_mutex_lock(mptr);
#endif
}

/********************************/
/* try to lock a mutex          */
/* mptr: pointer to mutex       */
/* return 0 if already locked   */
/* return !0 if lock sucessfull */
/********************************/

#ifdef RLWIN32
WINBASEAPI BOOL WINAPI TryEnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection);
#endif

int rlwthread_mutex_trylock(pthread_mutex_t *mptr)
{
#ifdef RLWIN32
  DWORD ret;

  ret = WaitForSingleObject(*mptr, 0);
  if(ret == WAIT_OBJECT_0) return 1;
  return 0;
  //old ret = TryEnterCriticalSection(mptr); // pointer to critical section object
  return ret;
#else
  int ret;

  ret = pthread_mutex_trylock(mptr);
  if(ret == EBUSY) return 0;
  return 1;
#endif
}

/**************************/
/* unlock a mutex         */
/* mptr: pointer to mutex */
/**************************/
int rlwthread_mutex_unlock(pthread_mutex_t *mptr)
{
#ifdef RLWIN32
  ReleaseMutex(*mptr);
  //old LeaveCriticalSection(mptr);
  return 0;
#else
  return pthread_mutex_unlock(mptr);
#endif
}

/**************************/
/* kill a thread          */
/* tid handel of thread   */
/**************************/
int rlwthread_cancel(pthread_t tid)
{
#ifdef RLWIN32
  return (int) CloseHandle((HANDLE) tid);
#else
  return pthread_cancel(tid);
#endif
}

/***********************************************/
/* initialize a semaphore                      */
/* s is a pointer to a WSEMAPHORE struct       */
/* cmax is the maximum number of the semaphore */
/* return 0                                    */
/***********************************************/
int rlwrapinit_semaphore(WSEMAPHORE *s, int cmax)
{
/* Create a semaphore with initial count=0 max. counts of cmax. */
#ifdef RLWIN32

  s->cmax = cmax;
  s->hSemaphore = CreateSemaphore(
    NULL,   /* no security attributes */
    0,      /* initial count */
    cmax,   /* maximum count */
    NULL);  /* unnamed semaphore */

  if(s->hSemaphore == NULL) return -1; /* Check for error. */
  return 0;

#else

  s->cmax   = cmax;
  s->nready = 0;
  rlwthread_mutex_init(&s->mutex, NULL);
  pthread_cond_init(&s->cond, NULL);
  return 0;

#endif
}

int rlwrapdestroy_semaphore(WSEMAPHORE *s)
{
#ifdef RLWIN32
  CloseHandle(s->hSemaphore);
#else
  rlwthread_mutex_destroy(&s->mutex);
#endif
  return 0;
}

/***********************************************************************/
/* increment a semaphore                                               */
/* s is a pointer to a WSEMAPHORE struct                               */
/* the producer will call this function whenever new data is available */
/* return 0=OK -1=error                                                */
/***********************************************************************/
int rlwrapincrement_semaphore(WSEMAPHORE *s)
{
/* Increment the count of the semaphore. */
#ifdef RLWIN32

  if(!ReleaseSemaphore(
        s->hSemaphore,  /* handle of semaphore */
        1,              /* increase count by one */
        NULL) )         /* not interested in previous count */
  {
    return -1; /* Deal with the error. */
  }
  return 0;

#else

  pthread_mutex_lock(&s->mutex);
  if(s->nready == 0) pthread_cond_signal(&s->cond);
  s->nready++;
  pthread_mutex_unlock(&s->mutex);
  return 0;

#endif
}

/*********************************************************************************/
/* wait for a semaphore                                                          */
/* s is a pointer to a WSEMAPHORE struct                                         */
/* the consumer will call this function to wait for new data to become available */
/* return 0                                                                      */
/*********************************************************************************/
int rlwrapwait_semaphore(WSEMAPHORE *s)
{
#ifdef RLWIN32

  int ret;
  ret = WaitForSingleObject(
        s->hSemaphore,   /* handle of semaphore */
        INFINITE);       /* infinite time-out interval */
  if(ret) return 0;
  return 0;

#else

  pthread_mutex_lock(&s->mutex);
  while(s->nready == 0)
  {
    pthread_cond_wait(&s->cond,&s->mutex);
  }  
  s->nready--;
  pthread_mutex_unlock(&s->mutex);
  return 0;

#endif
}

int rlwthread_sleep(long msec)
{
#ifdef RLWIN32
  Sleep(msec);
  return 0;
#endif

#ifdef RLUNIX
  fd_set wset,rset,eset;
  struct timeval timeout;

  FD_ZERO(&rset);
  FD_ZERO(&wset);
  FD_ZERO(&eset);
  timeout.tv_sec  = msec / 1000;
  timeout.tv_usec = (msec % 1000) * 1000;
  select(1,&rset,&wset,&eset,&timeout);
  return 0;
#endif

#ifdef __VMS
  struct timespec interval;

  interval.tv_sec  =  msec / 1000;
  interval.tv_nsec = (msec % 1000) * 1000 * 1000; /* wait msec msec */
  pthread_delay_np(&interval);
  return 0;
#endif
}

void rlsleep(long msec)
{
  rlwthread_sleep(msec);
}
/*** test routine ***/
/**************************************************************/
/* This is an example how to use the wthread_xxx functions    */
/**************************************************************/
#define TESTINGx
#ifdef  TESTING
#include "stdio.h"
#include "stdlib.h"
#ifndef RLWIN32
#include "unistd.h"
#endif

/******************************************/
/* here we define 2 mutexes (semaphores)  */
/* the mutexes are global two all threads */
/******************************************/
pthread_mutex_t mutex1, mutex2;
WSEMAPHORE semaphore;

/************************************************************************/
/* this is the tread function                                           */
/* it's name is given as parameter to wthread_create                    */
/* *arg is an argument given by the last parameter of wthread_create    */
/* you can get the return value by pthread_join (status)                */
/************************************************************************/
void *thread(void *arg)
{
  int i=0;
  printf("thread start : %s\n",(const char *) arg);
  while(i++ < 50)
  {
    rlwrapwait_semaphore(&semaphore);
    printf("thread %d\n",i);
    /* sleep(1); */
  }
  return NULL;
}

int main()
{
  pthread_t tid; /* this is the thread_id */
  void *status;  /* this is the return value of the tread */
  int i;

  /*****************************************************************************/
  /* here we initialize 2 mutexes                                              */
  /* the mutexes are defined global to all threads                             */
  /* for compatibility with MSWINDOWS the second parameter should be NULL      */
  /*                                                                           */
  /* in order to mark critical sections we can now use the following functions */
  /* wthread_mutex_lock                                                        */
  /* wthread_mutex_unlock                                                      */
  /* if a mutex is locked by a thread and an other thread is calling           */
  /* wthread_mutex_lock it will be blocked until the first thread calls        */
  /* the wthread_mutex_unlock function                                         */
  /*                                                                           */
  /* this function is the reverse of wthread_mutex_init                        */
  /* wthread_mutex_destroy                                                     */
  /*****************************************************************************/
  rlwthread_mutex_init(&mutex1, NULL);
  rlwthread_mutex_init(&mutex2, NULL);

  rlwrapinit_semaphore(&semaphore, 1000);
  printf("create a thread\n");
  /****************************************************************/
  /* this call will create a new thread                           */
  /* tid is the returned thread_id                                */
  /*     you can use this id for example in wthread_join          */
  /* in this example the second parameter is NULL                 */
  /*     you can call wthread_attr_init(pthread_attr_t *attr)     */
  /*     to initialize the start attributes                       */
  /*     pthread_attr_t attr;                                     */
  /*     wthread_attr_init(&attr);                                */
  /*     wthread_create(&tid, &attr, thread, "hello thread");     */
  /* thread is the name of the thread function (see above)        */
  /* the last parameter is given to the thread function           */
  /****************************************************************/
  rlwthread_create(&tid, NULL, thread, "hello thread");

  i=0;
  while(i++ < 50)
  {
    printf("main %d\n",i);
    rlwrapincrement_semaphore(&semaphore);
    if(i%4 == 0) Sleep(10);
  }

  printf("waiting for thread\n");
  /*****************************************************/
  /* this call waits for the termination of thread tid */
  /* the return value of the thread is given to status */
  /*****************************************************/
  rlwthread_join(tid, &status);
  printf("finish\n");
  return 0;
}
#endif
