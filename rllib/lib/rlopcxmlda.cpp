/***************************************************************************
                          rlopcxmlda.cpp  -  description
                             -------------------
    begin                : Mon Aug 27 2007
    copyright            : (C) 20071 by pvbrowser
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlopcxmlda.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

rlOpcXmlDa::rlOpcXmlDa(const char *mailbox, const char *shared_memory, long shared_memory_size)
{
  mbx = new rlMailbox(mailbox);
  shm = new rlSharedMemory(shared_memory,shared_memory_size);
  shmheader = (SHM_HEADER *) shm->getUserAdr();
  shmvalues = ((const char *)shmheader) + sizeof(SHM_HEADER);
}

rlOpcXmlDa::~rlOpcXmlDa()
{
  delete mbx;
  delete shm;
}

const char *rlOpcXmlDa::stringValue(const char *variable)
{
  int value_offset, delta_index, nmax, i;
  const char *cptr;

  if(shmheader == NULL) return "OPCXMLDA_ERROR";
  if(strcmp(shmheader->ident,"opc") != 0) return "OPCXMLDA_ERROR";
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

  return "OPCXMLDA_ERROR";
}

int rlOpcXmlDa::intValue(const char *variable)
{
  const char *cptr;
  int ret;

  cptr = stringValue(variable);
  if(isdigit(*cptr))
  {
    ret = OPCXMLDA_ERROR;
    sscanf(cptr,"%d",&ret);
    return ret;
  }
  return OPCXMLDA_ERROR;
}

float rlOpcXmlDa::floatValue(const char *variable)
{
  const char *cptr;
  float ret;

  cptr = stringValue(variable);
  if(isdigit(*cptr) || *cptr == '-')
  {
    ret = OPCXMLDA_ERROR;
    sscanf(cptr,"%f",&ret);
    return ret;
  }
  return OPCXMLDA_ERROR;
}

int rlOpcXmlDa::writeStringValue(const char *variable, const char *value)
{
  mbx->printf("%s,%s\n",variable,value);
  return 0;
}

int rlOpcXmlDa::writeIntValue(const char *variable, int value)
{
  mbx->printf("%s,%d\n",variable,value);
  return 0;
}

int rlOpcXmlDa::writeFloatValue(const char *variable, float value)
{
  mbx->printf("%s,%f\n",variable,value);
  return 0;
}

int rlOpcXmlDa::readErrorCount()
{
  if(shmheader == NULL) return OPCXMLDA_ERROR;
  return shmheader->readErrorCount;
}

int rlOpcXmlDa::writeErrorCount()
{
  if(shmheader == NULL) return OPCXMLDA_ERROR;
  return shmheader->writeErrorCount;
}

int rlOpcXmlDa::shmStatus()
{
  if(shmheader == NULL) return OPCXMLDA_ERROR;
  if(shm->status == rlSharedMemory::OK) return 0;
  return OPCXMLDA_ERROR;
}

