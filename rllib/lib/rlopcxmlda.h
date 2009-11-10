/***************************************************************************
                          rlopcxmlda.h  -  description
                             -------------------
    begin                : Mon Aug 27 2007
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
/**
use this class together with opcxmlda_client.
opcxmlda_client will fill the shared memory with variables read over opc xml-da.
opcxmlda_client will receive outgoing writes through it's mailbox.
*/

#ifndef _RL_OPC_XML_DA_H_
#define _RL_OPC_XML_DA_H_

#include "rldefine.h"
#include "rlmailbox.h"
#include "rlsharedmemory.h"

/*! <pre>
This class is for data acquisition within pvserver according to the pvbrowser principle.
It communicates with the daemon opcxmlda_client by the means of a shared memory and a mailbox.
</pre> */
class rlOpcXmlDa
{
public:
  // shared memory header
  typedef struct
  {
    char ident[4];          // must be "opc"
    int  maxItemNameLength; // maximum length of an item name
    int  maxNameLength;     // maximum length of the item value
    int  numItems;          // number of items in shared memory
    int  readErrorCount;    // 0...65536 incremented by each read error
    int  writeErrorCount;   // 0...65536 incremented by each write error
    int  spare[8];          // for future use
    char cspare[32];        // for future use
  }SHM_HEADER;
  
  enum OPC_XML_DA_ENUM
  {
    OPCXMLDA_ERROR = 256*256*128
  };

#ifdef RLWIN32  
  rlOpcXmlDa(const char *mailbox="c:\\automation\\mbx\\opcxmlda.mbx", const char *shared_memory="c:\\automation\\shm\\opcxmlda.shm", long shared_memory_size=65536);
#else
  rlOpcXmlDa(const char *mailbox="/srv/automation/mbx/opcxmlda.mbx", const char *shared_memory="/srv/automation/shm/opcxmlda.shm", long shared_memory_size=65536);
#endif
  virtual    ~rlOpcXmlDa();
  const char *stringValue(const char *variable);
  int         intValue(const char *variable);
  float       floatValue(const char *variable);
  int         writeStringValue(const char *variable, const char *value);
  int         writeIntValue(const char *variable, int value);
  int         writeFloatValue(const char *variable, float value);
  int         readErrorCount();
  int         writeErrorCount();
  int         shmStatus(); // 0 if shared memory is ok | ERROR if shared memory is not ok

private:
  SHM_HEADER     *shmheader;
  const char     *shmvalues;
  rlMailbox      *mbx;
  rlSharedMemory *shm;
};

#endif

