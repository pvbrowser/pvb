/***************************************************************************
                          rldataacquisition.h  -  description
                             -------------------
    begin                : Mon Sep 03 2007
    copyright            : (C) 2007 by pvbrowser
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_DAQ_H_
#define _RL_DAQ_H_

#include "rldefine.h"
#include "rlmailbox.h"
#include "rlsharedmemory.h"

/*! <pre>
This class is for data acquisition within pvserver according to the pvbrowser principle.
It communicates by the means of a shared memory and a mailbox.
Use it together with rlDataAcquisitionProvider.
</pre> */
class rlDataAcquisition
{
public:
  // shared memory header
  typedef struct
  {
    char ident[4];          // must be "daq"
    int  maxItemNameLength; // maximum length of an item name
    int  maxNameLength;     // maximum length of the item value
    int  numItems;          // number of items in shared memory
    int  readErrorCount;    // 0...65536 incremented by each read error
    int  writeErrorCount;   // 0...65536 incremented by each write error
    int  lifeCounter;       // 0...65536 incremented on each cycle
    int  spare[7];          // for future use
    char cspare[32];        // for future use
  }SHM_HEADER;
  
  enum DAQ_ENUM
  {
    DAQ_ERROR = 256*256*128
  };

#ifdef RLWIN32  
  rlDataAcquisition(const char *mailbox="c:\\automation\\mbx\\dataacquisition.mbx", const char *shared_memory="c:\\automation\\shm\\dataacquisition.shm", long shared_memory_size=65536);
#else
  rlDataAcquisition(const char *mailbox="/srv/automation/mbx/dataacquisition.mbx", const char *shared_memory="/srv/automation/shm/dataacquisition.shm", long shared_memory_size=65536);
#endif
  virtual    ~rlDataAcquisition();
  const char *stringValue(const char *variable);
  int         intValue(const char *variable);
  float       floatValue(const char *variable);
  int         writeStringValue(const char *variable, const char *value);
  int         writeIntValue(const char *variable, int value);
  int         writeFloatValue(const char *variable, float value);
  /*! Incremented by the daemon on each read error */
  int         readErrorCount();
  /*! Incremented by the daemon on each write error */
  int         writeErrorCount();
  /*! Incremented by the daemon in each cycle */
  int         lifeCounter();
  const char *firstVariable();
  const char *nextVariable();
  int         shmStatus(); // 0 if shared memory is ok | DAQ_ERROR if shared memory is not ok
  int         shmKey();    // key of shared memory
  int         shmId();     // id of shared memory

private:
  SHM_HEADER     *shmheader;
  const char     *shmvalues;
  rlMailbox      *mbx;
  rlSharedMemory *shm;
  int             iCurrent;
};

#endif

