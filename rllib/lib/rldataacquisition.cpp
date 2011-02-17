/***************************************************************************
                          rldataacquisition.cpp  -  description
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
#include "rldataacquisition.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

rlDataAcquisition::rlDataAcquisition(const char *mailbox, const char *shared_memory, long shared_memory_size)
{
  mbx = new rlMailbox(mailbox);
  shm = new rlSharedMemory(shared_memory,shared_memory_size);
  shmheader = (SHM_HEADER *) shm->getUserAdr();
  shmvalues = ((const char *)shmheader) + sizeof(SHM_HEADER);
  iCurrent = -1;
}

rlDataAcquisition::~rlDataAcquisition()
{
  delete mbx;
  delete shm;
}

const char *rlDataAcquisition::stringValue(const char *variable)
{
  int value_offset, delta_index, nmax, i;
  const char *cptr;

  if(shmheader == NULL) return "DAQ_ERROR: shmheader==NULL";
  if(strcmp(shmheader->ident,"daq") != 0) return "DAQ_ERROR: shmheader->ident is false";
  value_offset = shmheader->maxItemNameLength + 1;
  delta_index  = value_offset + shmheader->maxNameLength + 1;
  nmax         = shmheader->numItems;

  cptr = shmvalues;
  for(i=0; i<nmax; i++)
  {
    if(strcmp(cptr,variable) == 0)
    {
      return cptr + value_offset;
    }
    cptr += delta_index;
  }

  return "DAQ_ERROR: variable not found in shared memory";
}

int rlDataAcquisition::intValue(const char *variable)
{
  const char *cptr;
  int ret;

  cptr = stringValue(variable);
  if(isdigit(*cptr))
  {
    ret = DAQ_ERROR;
    sscanf(cptr,"%d",&ret);
    return ret;
  }
  return DAQ_ERROR;
}

float rlDataAcquisition::floatValue(const char *variable)
{
  const char *cptr;
  float ret;

  cptr = stringValue(variable);
  if(isdigit(*cptr) || *cptr == '-')
  {
    ret = DAQ_ERROR;
    sscanf(cptr,"%f",&ret);
    return ret;
  }
  return DAQ_ERROR;
}

int rlDataAcquisition::writeStringValue(const char *variable, const char *value)
{
  mbx->printf("%s,%s\n",variable,value);
  return 0;
}

int rlDataAcquisition::writeIntValue(const char *variable, int value)
{
  mbx->printf("%s,%d\n",variable,value);
  return 0;
}

int rlDataAcquisition::writeFloatValue(const char *variable, float value)
{
  mbx->printf("%s,%f\n",variable,value);
  return 0;
}

int rlDataAcquisition::readErrorCount()
{
  if(shmheader == NULL) return DAQ_ERROR;
  return shmheader->readErrorCount;
}

int rlDataAcquisition::writeErrorCount()
{
  if(shmheader == NULL) return DAQ_ERROR;
  return shmheader->writeErrorCount;
}

int rlDataAcquisition::lifeCounter()
{
  if(shmheader == NULL) return DAQ_ERROR;
  return shmheader->lifeCounter;
}

const char *rlDataAcquisition::firstVariable()
{
  const char *cptr;

  if(shmheader == NULL) return "DAQ_ERROR";
  if(strcmp(shmheader->ident,"daq") != 0) return "DAQ_ERROR";
  cptr = shmvalues;
  iCurrent = 1;
  return cptr;
}

const char *rlDataAcquisition::nextVariable()
{
  int value_offset, delta_index, nmax, i;
  const char *cptr;

  if(iCurrent < 0) return NULL;
  if(shmheader == NULL) return "DAQ_ERROR";
  if(strcmp(shmheader->ident,"daq") != 0) return "DAQ_ERROR";
  value_offset = shmheader->maxItemNameLength + 1;
  delta_index  = value_offset + shmheader->maxNameLength + 1;
  nmax         = shmheader->numItems;

  cptr = shmvalues;
  for(i=0; i<nmax; i++)
  {
    if(i == iCurrent)
    {
      iCurrent++;
      return cptr;
    }
    cptr += delta_index;
  }

  iCurrent = -1;
  return NULL;
}

int rlDataAcquisition::shmStatus()
{
  if(shmheader == NULL) return DAQ_ERROR;
  if(shm->status == rlSharedMemory::OK) return 0;
  return DAQ_ERROR;
}

int rlDataAcquisition::shmKey()
{
  if(shm == NULL) return -1;
  return shm->shmKey();
}

int rlDataAcquisition::shmId()
{
  if(shm == NULL) return -1;
  return shm->shmId();
}


