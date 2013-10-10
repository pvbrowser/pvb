/***************************************************************************
                  rldataacquisitionprovider.cpp  -  description
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
#include "rldataacquisitionprovider.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

rlDataAcquisitionProvider::rlDataAcquisitionProvider(int maxNameLength, const char *shared_memory, long shared_memory_size)
{
  shm = new rlSharedMemory(shared_memory,shared_memory_size);
  shmheader = (SHM_HEADER *) shm->getUserAdr();
  shmvalues = ((char *)shmheader) + sizeof(SHM_HEADER);
  memset(shmheader,0,shared_memory_size);
  shmheader->maxItemNameLength = 0;
  shmheader->maxNameLength = maxNameLength;
  shmheader->numItems = 0;
  shmheader->readErrorCount = 0;
  shmheader->writeErrorCount = 0;
  sharedMemorySize = shared_memory_size;
  strcpy(shmheader->ident, "daq");
  allow_add_values = 0;
}

rlDataAcquisitionProvider::~rlDataAcquisitionProvider()
{
  delete shm;
}

int rlDataAcquisitionProvider::readItemList(const char *filename)
{
  if(filename == NULL) return DAQ_PROVIDER_ERROR;

  FILE *fin;
  char line[1024], *cptr, *cptr2;
  int  maxItemNameLength,numItems,len,i;
  int  value_offset, delta_index;
  int  ilong;

  // get maxItemNameLength and numItems
  fin = fopen(filename,"r");
  if(fin == NULL)
  {
    printf("could not open itemlist %s\n",filename);
    return DAQ_PROVIDER_ERROR;
  }
  maxItemNameLength = numItems = 0;
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    if(line[0] > ' ' && line[0] != '#')
    {
      len = strlen(line);
      if(len > maxItemNameLength) maxItemNameLength = len;
      numItems++;
    }
  }
  fclose(fin);
  shmheader->maxItemNameLength = maxItemNameLength;
  shmheader->numItems = numItems;

  // read items
  fin = fopen(filename,"r");
  if(fin == NULL)
  {
    printf("could not open itemlist %s\n",filename);
    return DAQ_PROVIDER_ERROR;
  }
  cptr = shmvalues;
  value_offset = shmheader->maxItemNameLength + 1;
  delta_index  = value_offset + shmheader->maxNameLength + 1;
  ilong = sharedMemorySize - sizeof(SHM_HEADER);
  if(ilong < numItems*delta_index)
  {
    printf("rlDataAcquisitionProvider::shared memmory is too small sharedMemorySize=%ld sizeNeeded=%d\n", sharedMemorySize, (int) (numItems*delta_index + sizeof(SHM_HEADER)));
    return DAQ_PROVIDER_ERROR;
  }
  i = 0;
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    if(line[0] > ' ' && line[0] != '#')
    {
      len = strlen(line);
      if(len > shmheader->maxNameLength) line[shmheader->maxNameLength] = '\0';
      cptr2 = strchr(line,'\n');
      if(cptr2 != NULL) *cptr2 = '\0';
      cptr2 = strchr(line,' ');
      if(cptr2 != NULL) *cptr2 = '\0';
      cptr2 = strchr(line,'\t');
      if(cptr2 != NULL) *cptr2 = '\0';
      strcpy(cptr + (i*delta_index), line);
      i++;
    }
  }
  fclose(fin);
  strcpy(shmheader->ident, "daq");

  return 0;
}

const char *rlDataAcquisitionProvider::firstItem()
{
  int value_offset, delta_index;
  const char *cptr;

  current_item = 0;
  if(shmheader == NULL)                   return NULL;
  if(shmheader->numItems <= 0)            return NULL;
  if(strcmp(shmheader->ident,"daq") != 0) return NULL;

  value_offset = shmheader->maxItemNameLength + 1;
  delta_index  = value_offset + shmheader->maxNameLength + 1;

  cptr = shmvalues;
  return cptr + (current_item * delta_index);
}

const char *rlDataAcquisitionProvider::nextItem()
{
  int value_offset, delta_index;
  const char *cptr;

  current_item++;
  if(shmheader == NULL)                   return NULL;
  if(shmheader->numItems <= 0)            return NULL;
  if(strcmp(shmheader->ident,"daq") != 0) return NULL;
  if(shmheader->numItems <= current_item) return NULL;

  value_offset = shmheader->maxItemNameLength + 1;
  delta_index  = value_offset + shmheader->maxNameLength + 1;

  cptr = shmvalues;
  return cptr + (current_item * delta_index);
}

const char *rlDataAcquisitionProvider::stringValue(const char *variable)
{
  int value_offset, delta_index, nmax, i;
  const char *cptr;

  if(shmheader == NULL) return "DAQ_ERROR";
  if(strcmp(shmheader->ident,"daq") != 0) return "DAQ_ERROR";
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

  return "DAQ_ERROR";
}

int rlDataAcquisitionProvider::intValue(const char *variable)
{
  const char *cptr;
  int ret;

  cptr = stringValue(variable);
  if(isdigit(*cptr))
  {
    ret = DAQ_PROVIDER_ERROR;
    sscanf(cptr,"%d",&ret);
    return ret;
  }
  return DAQ_PROVIDER_ERROR;
}

float rlDataAcquisitionProvider::floatValue(const char *variable)
{
  const char *cptr;
  float ret;

  cptr = stringValue(variable);
  if(isdigit(*cptr) || *cptr == '-')
  {
    ret = DAQ_PROVIDER_ERROR;
    sscanf(cptr,"%f",&ret);
    return ret;
  }
  return DAQ_PROVIDER_ERROR;
}

int rlDataAcquisitionProvider::setStringValue(const char *variable, const char *value)
{
  if(variable == NULL || value == NULL || shmheader == NULL) return DAQ_PROVIDER_ERROR;
  if(strcmp(shmheader->ident,"daq") != 0)                    return DAQ_PROVIDER_ERROR;

  int value_offset, delta_index, nmax, i;
  char *cptr;

  if((int) strlen(variable) > shmheader->maxItemNameLength)  return DAQ_PROVIDER_ERROR;

  int len = strlen(value);
  char *val = new char[len+1];
  strcpy(val,value);
  if(len > shmheader->maxNameLength) val[shmheader->maxNameLength] = '\0';
 
  value_offset = shmheader->maxItemNameLength + 1;
  delta_index  = value_offset + shmheader->maxNameLength + 1;
  nmax         = shmheader->numItems;

  cptr = shmvalues;
  for(i=0; i<nmax; i++)
  {
    if(strcmp(cptr,variable) == 0)
    {
      strcpy(cptr + value_offset, val); // update existing value
      delete [] val;
      return 0;
    }
    cptr += delta_index;
  }

  if(allow_add_values == 1) // insert a new value
  {
    if(sharedMemorySize > ( (long) sizeof(SHM_HEADER) + (i*delta_index) + delta_index ) )
    {
      strcpy(cptr, variable);           // insert the value
      strcpy(cptr + value_offset, val); // insert the value
      delete [] val;
      shmheader->numItems += 1;
      return 0;
    }
    else
    {
      ::printf("rlDataAcquisitionProvider: ERROR shared memory too small to add variable=%s val=%s please increase shared memory size", variable, val);
    }
  }

  delete [] val;
  return DAQ_PROVIDER_ERROR;
}

int rlDataAcquisitionProvider::setIntValue(const char *variable, int value)
{
  char cval[128];

  sprintf(cval,"%d",value);
  return setStringValue(variable,cval);
}

int rlDataAcquisitionProvider::setFloatValue(const char *variable, float value)
{
  char cval[128];

  sprintf(cval,"%f",value);
  return setStringValue(variable,cval);
}

int rlDataAcquisitionProvider::readErrorCount()
{
  if(shmheader == NULL) return DAQ_PROVIDER_ERROR;
  return shmheader->readErrorCount;
}

int rlDataAcquisitionProvider::writeErrorCount()
{
  if(shmheader == NULL) return DAQ_PROVIDER_ERROR;
  return shmheader->writeErrorCount;
}

int rlDataAcquisitionProvider::lifeCounter()
{
  if(shmheader == NULL) return DAQ_PROVIDER_ERROR;
  return shmheader->lifeCounter;
}

int rlDataAcquisitionProvider::setReadErrorCount(int count)
{
  if(shmheader == NULL) return DAQ_PROVIDER_ERROR;
  shmheader->readErrorCount = count;
  return 0;
}

int rlDataAcquisitionProvider::setWriteErrorCount(int count)
{
  if(shmheader == NULL) return DAQ_PROVIDER_ERROR;
  shmheader->writeErrorCount = count;
  return 0;
}

int rlDataAcquisitionProvider::setLifeCounter(int count)
{
  if(shmheader == NULL) return DAQ_PROVIDER_ERROR;
  shmheader->lifeCounter = count;
  return 0;
}

int rlDataAcquisitionProvider::shmStatus()
{
  if(shmheader == NULL) return DAQ_PROVIDER_ERROR;
  if(shm->status == rlSharedMemory::OK) return 0;
  return DAQ_PROVIDER_ERROR;
}

int rlDataAcquisitionProvider::setAllowAddValues(int allow, int maxItemNameLength)
{
  if(allow == 0) allow_add_values = 0;
  else           allow_add_values = 1;
  if(maxItemNameLength > 0) shmheader->maxItemNameLength = maxItemNameLength;
  return 0;
}


