/***************************************************************************
                          rlplc.cpp -  description
                             -------------------
    begin                : Tue Dec 11 2008
    copyright            : (C) 2008 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include <string.h>
#include "rlplc.h"

rlPlcState::rlPlcState(int numInt, int numFloat, int numDouble, const char *shared_memory)
{
  max_int = numInt;
  if(max_int <= 0) max_int = 1;
  max_float = numFloat;
  if(max_float <= 0) max_float = 1;
  max_double = numDouble;
  if(max_double <= 0) max_double = 1;
  if(shared_memory == NULL)
  { // use local memory
    i     = new int[max_int];
    i_old = new int[max_int];
    f     = new float[max_float];
    f_old = new float[max_float];
    d     = new double[max_double];
    d_old = new double[max_double];
    memset(i    ,0,sizeof(int)*max_int);
    memset(i_old,0,sizeof(int)*max_int);
    memset(f    ,0,sizeof(float)*max_float);
    memset(f_old,0,sizeof(float)*max_float);
    memset(d    ,0,sizeof(double)*max_double);
    memset(d_old,0,sizeof(double)*max_double);
  }
  else
  { // map memory to shared memory
    shm = new rlSharedMemory(shared_memory, sizeof(int)*max_int*2 + sizeof(float)*max_float*2 + sizeof(double)*max_double*2 );
    if(shm->status == rlSharedMemory::OK)
    {
      void *ptr = shm->getUserAdr();
      i     = (int *)    ptr;
      i_old = (int *)    ptr + sizeof(int)*max_int;
      f     = (float *)  ptr + sizeof(int)*max_int*2;
      f_old = (float *)  ptr + sizeof(int)*max_int*2 + sizeof(float)*max_float;
      d     = (double *) ptr + sizeof(int)*max_int*2 + sizeof(float)*max_float*2;
      d_old = (double *) ptr + sizeof(int)*max_int*2 + sizeof(float)*max_float*2 + sizeof(double)*max_double;
    }
    else
    {
      printf("ERROR: rlPlcState sharedMemoryStatus(%s) is not OK\n", shared_memory);
    }
  }
}

rlPlcState::~rlPlcState()
{
  if(shm == NULL)
  {
    delete [] i;
    delete [] i_old;
    delete [] f;
    delete [] f_old;
    delete [] d;
    delete [] d_old;
  }
  else
  {
    delete shm;
  }
}

void rlPlcState::clear()
{
  if(i     != NULL) memset(i    ,0,sizeof(int)*max_int);
  if(i_old != NULL) memset(i_old,0,sizeof(int)*max_int);
  if(f     != NULL) memset(f    ,0,sizeof(float)*max_float);
  if(f_old != NULL) memset(f_old,0,sizeof(float)*max_float);
  if(d     != NULL) memset(d    ,0,sizeof(double)*max_double);
  if(d_old != NULL) memset(d_old,0,sizeof(double)*max_double);
}

void rlPlcState::rememberState()
{
  memcpy(i_old,i,sizeof(int)*max_int);
  memcpy(f_old,f,sizeof(float)*max_float);
  memcpy(d_old,d,sizeof(double)*max_double);
}

int rlPlcState::intChanged(int index)
{
  if(index<0 || index >= max_int) return 0;
  if(i[index] == i_old[index])    return 0;
  else                            return 1;
}

int rlPlcState::floatChanged(int index)
{
  if(index<0 || index >= max_float) return 0;
  if(f[index] == f_old[index])      return 0;
  else                              return 1;
}

int rlPlcState::doubleChanged(int index)
{
  if(index<0 || index >= max_double) return 0;
  if(d[index] == d_old[index])       return 0;
  else                               return 1;
}

int rlPlcState::intHasIncreased(int index)
{
  if(index<0 || index >= max_int) return 0;
  if(i[index] > i_old[index])     return 1;
  else                            return 0;
}

int rlPlcState::floatHasIncreased(int index)
{
  if(index<0 || index >= max_float) return 0;
  if(f[index] > f_old[index])       return 1;
  else                              return 0;
}

int rlPlcState::doubleHasIncreased(int index)
{
  if(index<0 || index >= max_double) return 0;
  if(d[index] > d_old[index])        return 1;
  else                               return 0;
}

int rlPlcState::intHasDecreased(int index)
{
  if(index<0 || index >= max_int) return 0;
  if(i[index] < i_old[index])     return 1;
  else                            return 0;
}

int rlPlcState::floatHasDecreased(int index)
{
  if(index<0 || index >= max_float) return 0;
  if(f[index] < f_old[index])       return 1;
  else                              return 0;
}

int rlPlcState::doubleHasDecreased(int index)
{
  if(index<0 || index >= max_double) return 0;
  if(d[index] < d_old[index])        return 1;
  else                               return 0;
}

int rlPlcState::deltaInt(int index)
{
  if(index<0 || index >= max_int) return 0;
  return i[index] - i_old[index];
}

float rlPlcState::deltaFloat(int index)
{
  if(index<0 || index >= max_float) return 0;
  return f[index] - f_old[index];
}

double rlPlcState::deltaDouble(int index)
{
  if(index<0 || index >= max_double) return 0;
  return d[index] - d_old[index];
}

void rlPlcState::set(int index, int bit)
{
  if(index<0 || index >= max_int) return;
  i[index] = i[index] | bit;
}

void rlPlcState::clear(int index, int bit)
{
  if(index<0 || index >= max_int) return;
  i[index] = i[index] & ~bit;
}

int rlPlcState::isSet(int index, int bit)
{
  if(index<0 || index >= max_int) return 0;
  if(i[index] & bit) return 1;
  else               return 0;
}

int rlPlcState::isClear(int index, int bit)
{
  if(index<0 || index >= max_int) return 0;
  if(i[index] & bit) return 0;
  else               return 1;
}

int rlPlcState::hasBeenSet(int index, int bit)
{
  if(index<0 || index >= max_int) return 0;
  if(i[index] & bit) // is bit set ?
  {
    if(i_old[index] & bit) return 0; // both are set
    else                   return 1; // old one was not set
  }
  return 0;
}

int rlPlcState::hasBeenCleared(int index, int bit)
{
  if(index<0 || index >= max_int) return 0;
  if((i[index] & bit) == 0) // is bit clear ?
  {
    if((i_old[index] & bit) == 0) return 0; // both are clear
    else                          return 1; // old one was set
  }
  return 0;
}

int rlPlcState::maxInt()
{
  return max_int - 1;
}
    
int rlPlcState::maxFloat()
{
  return max_float - 1;
}

int rlPlcState::maxDouble()
{
  return max_float - 1;
}

int rlPlcState::getInt(int index)
{
  if(index<0 || index >= max_int) return 0;
  return i[index];
}

float rlPlcState::getFloat(int index)
{
  if(index<0 || index >= max_float) return 0;
  return f[index];
}

double rlPlcState::getDouble(int index)
{
  if(index<0 || index >= max_double) return 0;
  return d[index];
}

int rlPlcState::getOldInt(int index)
{
  if(index<0 || index >= max_int) return 0;
  return i_old[index];
}

float rlPlcState::getOldFloat(int index)
{
  if(index<0 || index >= max_float) return 0;
  return f_old[index];
}

double rlPlcState::getOldDouble(int index)
{
  if(index<0 || index >= max_double) return 0;
  return d_old[index];
}

//###################################################

rlPlcMem::rlPlcMem()
{
  i     = 0;
  i_old = 0;
  f     = 0.0f;
  f_old = 0.0f;
  d     = 0.0;
  d_old = 0.0;
}

rlPlcMem::~rlPlcMem()
{
}

void rlPlcMem::rememberState()
{
  i_old = i;
  f_old = f;
  d_old = d;
}

int rlPlcMem::intChanged()
{
  if(i == i_old) return 0;
  else           return 1;
}

int rlPlcMem::floatChanged()
{
  if(f == f_old) return 0;
  else           return 1;
}

int rlPlcMem::doubleChanged()
{
  if(d == d_old) return 0;
  else           return 1;
}

int rlPlcMem::intHasIncreased()
{
  if(i > i_old)  return 1;
  else           return 0;
}

int rlPlcMem::floatHasIncreased()
{
  if(f > f_old)  return 1;
  else           return 0;
}

int rlPlcMem::doubleHasIncreased()
{
  if(d > d_old) return 1;
  else          return 0;
}

int rlPlcMem::intHasDecreased()
{
  if(i < i_old) return 1;
  else          return 0;
}

int rlPlcMem::floatHasDecreased()
{
  if(f < f_old) return 1;
  else          return 0;
}

int rlPlcMem::doubleHasDecreased()
{
  if(d < d_old) return 1;
  else          return 0;
}

int rlPlcMem::deltaInt()
{
  return i - i_old;
}

float rlPlcMem::deltaFloat()
{
  return f - f_old;
}

double rlPlcMem::deltaDouble()
{
  return d - d_old;
}

void rlPlcMem::set(int bit)
{
  i = i | bit;
}

void rlPlcMem::clear(int bit)
{
  i = i & ~bit;
}

int rlPlcMem::isSet(int bit)
{
  if(i & bit) return 1;
  else        return 0;
}

int rlPlcMem::isClear(int bit)
{
  if(i & bit) return 0;
  else        return 1;
}

int rlPlcMem::hasBeenSet(int bit)
{
  if(i & bit) // is bit set ?
  {
    if(i_old & bit) return 0; // both are set
    else            return 1; // old one was not set
  }
  return 0;
}

int rlPlcMem::hasBeenCleared(int bit)
{
  if((i & bit) == 0) // is bit clear ?
  {
    if((i_old & bit) == 0) return 0; // both are clear
    else                   return 1; // old one was set
  }
  return 0;
}

