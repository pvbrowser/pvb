/***************************************************************************
                rldataacquisitionprovider.h  -  description
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
#ifndef _RL_DAQ_PROVIDER_H_
#define _RL_DAQ_PROVIDER_H_

#include "rldefine.h"
#include "rlsharedmemory.h"

/*! <pre>
This class is usefull to implement your own data acquisition according to the pvbrowser principle.
It is used within pvb/template/dataacquisition/client/data_acquisition_provider_template.cpp
Starting from there you can implement your data acquisition.
</pre> */
class rlDataAcquisitionProvider
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
    int  lifeCounter;       // 0...65536 incremented each cycle
    int  spare[7];          // for future use
    char cspare[32];        // for future use
  }SHM_HEADER;
  
  enum DAQ_PROVIDER_ENUM
  {
    DAQ_PROVIDER_ERROR = 256*256*128
  };

#ifdef RLWIN32  
  rlDataAcquisitionProvider(int maxNameLength=31, const char *shared_memory="c:\\automation\\shm\\dataacquisition.shm", long shared_memory_size=65536);
#else
  rlDataAcquisitionProvider(int maxNameLength=31, const char *shared_memory="/srv/automation/shm/dataacquisition.shm", long shared_memory_size=65536);
#endif
  virtual    ~rlDataAcquisitionProvider();
  int         readItemList(const char *filename); // return DAQ_PROVIDER_ERROR | num_items
  const char *firstItem();
  const char *nextItem();
  const char *stringValue(const char *variable);
  int         intValue(const char *variable);
  float       floatValue(const char *variable);
  int         setStringValue(const char *variable, const char *value);
  int         setIntValue(const char *variable, int value);
  int         setFloatValue(const char *variable, float value);
  int         readErrorCount();
  int         writeErrorCount();
  int         lifeCounter();
  int         setReadErrorCount(int count);
  int         setWriteErrorCount(int count);
  int         setLifeCounter(int count);
  int         shmStatus(); // 0 if shared memory is ok | DAQ_PROVIDER_ERROR if shared memory is not ok
  int         setAllowAddValues(int allow, int maxItemNameLength);

private:
  SHM_HEADER     *shmheader;
  char           *shmvalues;
  int             current_item, allow_add_values;
  rlSharedMemory *shm;
  long            sharedMemorySize;
};

#endif

