/***************************************************************************
                          rlbussignaldatabase.cpp  -  description
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef RLWIN32
#include <winsock2.h>
#endif
#include <mysql.h>
#include "rlbussignaldatabase.h"
#include "rlcutil.h"

static int   mysql_is_initialized = 0;
static MYSQL mysql;

rlBussignalDatabase::rlBussignalDatabase()
{
  databaseName = NULL;
  tableName    = NULL;
  connection   = NULL;
  if(mysql_is_initialized == 0)
  {
    mysql_init(&mysql);
    mysql_is_initialized = 1;
  }
}

rlBussignalDatabase::~rlBussignalDatabase()
{
  closeDatabase();
  if(databaseName != NULL) delete [] databaseName;
  if(tableName != NULL) delete [] tableName;
}

int rlBussignalDatabase::openDatabase(const char *database, const char *table)
{
  if(databaseName != NULL) delete [] databaseName;
  if(tableName != NULL) delete [] tableName;
  databaseName = new char[strlen(database)+1];
  strcpy(databaseName,database);
  tableName    = new char[strlen(table)+1];
  strcpy(tableName,table);
  connection = (void *) mysql_real_connect(&mysql, "localhost","bususr","buspw",databaseName,0,0,0);
  if(connection == NULL)
  {
    printf((const char *) mysql_error(&mysql));
    return -1;
  }
  return 0;
}

int rlBussignalDatabase::writeDatabaseInt(const char *item, int val)
{
  sprintf(buf,"%d",val);
  sprintf(typebuf,"I1");
  return writeDatabaseString(item);
}

int rlBussignalDatabase::writeDatabaseIntArray(const char *item, int *val, int len)
{
  int  i;
  char vbuf[80];

  buf[0] = '\0';
  for(i=0; i<len; i++)
  {
    sprintf(vbuf,"%d,",val[i]);
    if(strlen(buf)+strlen(vbuf) >= sizeof(buf)-1) return -1;
    strcat(buf,vbuf);
  }
  sprintf(typebuf,"I%d",len);
  return writeDatabaseString(item);
}

int rlBussignalDatabase::writeDatabaseFloat(const char *item, float val)
{
  sprintf(buf,"%f",val);
  sprintf(typebuf,"F1");
  return writeDatabaseString(item);
}

int rlBussignalDatabase::writeDatabaseFloatArray(const char *item, float *val, int len)
{
  int  i;
  char vbuf[80];

  buf[0] = '\0';
  for(i=0; i<len; i++)
  {
    sprintf(vbuf,"%f,",val[i]);
    if(strlen(buf)+strlen(vbuf) >= sizeof(buf)-1) return -1;
    strcat(buf,vbuf);
  }
  sprintf(typebuf,"F%d",len);
  return writeDatabaseString(item);
}

int rlBussignalDatabase::writeDatabaseString(const char *item, char *val)
{
  sprintf(typebuf,"S%d",strlen(val));
  rlstrncpy(buf,val,sizeof(buf)-1);
  return writeDatabaseString(item);
}

int rlBussignalDatabase::writeDatabaseString(const char *item)
{
  int  ret;
  char sqlbuf[rl_PRINTF_LENGTH];

  // Try an UPDATE
  ret = snprintf(sqlbuf,sizeof(sqlbuf)-1,"UPDATE %s SET datatype='%s', datavalue='%s' WHERE name='%s'",tableName,typebuf,buf,item);
  if(ret < 0)
  {
    printf("sqlbuf too small in writeDatabaseString\n");
    return -1;
  }
  ret = myquery(sqlbuf);
  if(ret > 0) return 0; // success

  // Try an INSERT
  ret = snprintf(sqlbuf,sizeof(sqlbuf)-1,"INSERT INTO %s VALUES ('%s','%s','%s')",tableName,item,typebuf,buf);
  if(ret < 0)
  {
    printf("sqlbuf too small in writeDatabaseString\n");
    return -1;
  }
  ret = myquery(sqlbuf);
  if(ret >= 0) return 0; // success
  return -1;             // failure
}

int rlBussignalDatabase::myquery(const char *query)
{
  int state,num_rows;
  MYSQL_RES *result;

  //printf("query=%s\n",query);
  state = mysql_query((MYSQL *) connection, query);
  if(state != 0)
  {
    //printf("query failed state=%d query=%s\n",state,query);
    return -1;
  }
  num_rows = mysql_affected_rows((MYSQL *) connection);
  result = mysql_store_result((MYSQL *) connection);
  mysql_free_result(result);
  //printf("num_rows=%d\n",num_rows);
  return num_rows;
}

int rlBussignalDatabase::readDatabase(const char *item, char *type, char *value)
{
  char query[1024];
  int state,num_rows;
  MYSQL_RES *result;
  MYSQL_ROW row;

  *type = *value = '\0';
  sprintf(query,"select * from bus where name = '%s'",item);
  //printf("query=%s\n",query);
  state = mysql_query((MYSQL *) connection, query);
  if(state != 0)
  {
    //printf("query failed state=%d query=%s\n",state,query);
    return -1;
  }
  num_rows = mysql_affected_rows((MYSQL *) connection);
  result = mysql_store_result((MYSQL *) connection);
  if(result != NULL)
  {
    row = mysql_fetch_row(result);
    if(row != NULL)
    {
      if(row[1] != NULL) strcpy(type ,row[1]);
      if(row[2] != NULL) strcpy(value,row[2]);
    }
    mysql_free_result(result);
  }
  //printf("num_rows=%d\n",num_rows);
  return num_rows;
}

int rlBussignalDatabase::closeDatabase()
{
  mysql_close((MYSQL *) connection);
  return 0;
}

