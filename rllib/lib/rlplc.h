/***************************************************************************
                          rlplc.h  -  description
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
#ifndef _RL_PLC_H_
#define _RL_PLC_H_

#include "rldefine.h"
#include "rlsharedmemory.h"

class rlPlcState
{
  public:
    rlPlcState(int numInt=100, int numFloat=100, int numDouble=0, const char *shared_memory=NULL);
    virtual ~rlPlcState();
    int    *i, *i_old;
    float  *f, *f_old;
    double *d, *d_old;
    void   clear();
    void   rememberState();
    int    intChanged(int index);
    int    floatChanged(int index);
    int    doubleChanged(int index);
    int    intHasIncreased(int index);
    int    floatHasIncreased(int index);
    int    doubleHasIncreased(int index);
    int    intHasDecreased(int index);
    int    floatHasDecreased(int index);
    int    doubleHasDecreased(int index);
    int    deltaInt(int index);
    float  deltaFloat(int index);
    double deltaDouble(int index);
    void   set(int index, int bit);
    void   clear(int index, int bit);
    int    isSet(int index, int bit);
    int    isClear(int index, int bit);
    int    hasBeenSet(int index, int bit);
    int    hasBeenCleared(int index, int bit);
    int    maxInt();
    int    maxFloat();
    int    maxDouble();
    int    getInt(int index);
    float  getFloat(int index);
    double getDouble(int index);
    int    getOldInt(int index);
    float  getOldFloat(int index);
    double getOldDouble(int index);
    rlSharedMemory *shm;

  private:
    int max_int, max_float, max_double;
};

class rlPlcMem
{
  public:
    rlPlcMem();
    virtual ~rlPlcMem();
    int    i, i_old;
    float  f, f_old;
    double d, d_old;
    void   rememberState();
    int    intChanged();
    int    floatChanged();
    int    doubleChanged();
    int    intHasIncreased();
    int    floatHasIncreased();
    int    doubleHasIncreased();
    int    intHasDecreased();
    int    floatHasDecreased();
    int    doubleHasDecreased();
    int    deltaInt();
    float  deltaFloat();
    double deltaDouble();
    void   set(int bit);
    void   clear(int bit);
    int    isSet(int bit);
    int    isClear(int bit);
    int    hasBeenSet(int bit);
    int    hasBeenCleared(int bit);
};

#endif

