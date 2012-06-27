/***************************************************************************
                          rlbussignaldatabase.h  -  description
                             -------------------
    begin                : Mon Aug 02 2002
    copyright            : (C) 2002 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_BUSSIGNAL_DATABASE_H_
#define _RL_BUSSIGNAL_DATABASE_H_

#include "rldefine.h"

class rlBussignalDatabase
{
  public:
    rlBussignalDatabase();
    virtual ~rlBussignalDatabase();
    int openDatabase(const char *database, const char *table);
    int writeDatabaseInt(const char *item, int val);
    int writeDatabaseIntArray(const char *item, int *val, int len);
    int writeDatabaseFloat(const char *item, float val);
    int writeDatabaseFloatArray(const char *item, float *val, int len);
    int writeDatabaseString(const char *item, char *val);
    int readDatabase(const char *item, char *type, char *value);
    int closeDatabase();
  private:
    int  writeDatabaseString(const char *item);
    int  myquery(const char *query);
    void *database;
    void *connection;
    char *databaseName;
    char *tableName;
    char buf[rl_PRINTF_LENGTH];
    char typebuf[16];
};

#endif
