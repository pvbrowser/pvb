/***************************************************************************
                          rlsharedmemory.h  -  description
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
#ifndef _RL_SHARED_MEMORY_H_
#define _RL_SHARED_MEMORY_H_

#include "rldefine.h"
#include "rlwthread.h"

/*! <pre>
class for a shared memory.

A shared memory is a piece of RAM that is shared between processes.
If many processes have access to the same RAM, it is necessary to lock the shared memory.
The read/write methods will do this and copy the data to local data.
If you want direct access, you may use getUserAdr().
</pre> */
class rlSharedMemory
{
public:
  enum SharedMemoryEnum
  {
    OK = 0,
    ERROR_FILE,
    ERROR_SHMGET,
    ERROR_SHMAT,
    ERROR_SHMCTL
  };

  /*! rwmode := access rights under unix. default 0600 user=read,write */
  rlSharedMemory(const char *name, unsigned long size, int rwmode=0600);
  virtual ~rlSharedMemory();
  int   deleteSharedMemory();
  int   write(unsigned long offset, const void *buf, int len);
  int   read (unsigned long offset, void *buf, int len);
  int   readInt(unsigned long offset, int index);
  int   readShort(unsigned long offset, int index);
  int   readByte(unsigned long offset, int index);
  float readFloat(unsigned long offset, int index);
  int   writeInt(unsigned long offset, int index, int val);
  int   writeShort(unsigned long offset, int index, int val);
  int   writeByte(unsigned long offset, int index, unsigned char val);
  int   writeFloat(unsigned long offset, int index, float val);
  void  *getUserAdr();
  int   shmKey();
  int   shmId();
  unsigned long size();
  int   status;
  char *name;
private:
  int id;
  int shmkey;
  char *base_adr;
  char *user_adr;
  unsigned long _size;
  pthread_mutex_t *mutex;
#ifdef RLWIN32
  HANDLE hSharedFile;
  OVERLAPPED overlapped;
#elif defined(RLUNIX)
  int fdlock;
#endif
};

#endif
