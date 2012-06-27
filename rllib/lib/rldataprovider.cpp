/***************************************************************************
                          rldataprovider.cpp  -  description
                             -------------------
    begin                : Fri Dec 20 2002
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
#include "rldataprovider.h"
#include <stdio.h>
#include <string.h>

rlDataProvider::rlDataProvider(int numInteger, int numFloat, int numString)
{
  int i;

  num_integer = numInteger;
  num_float   = numFloat;
  num_string  = numString;
  if(num_integer < 0) num_integer = 0;
  if(num_float   < 0) num_float   = 0;
  if(num_string  < 0) num_string  = 0;
  ints    = NULL;
  floats  = NULL;
  strings = NULL;
  if(num_integer > 0)
  {
    ints = new int[num_integer];
    for(i=0; i<num_integer; i++) ints[i] = 0;
  }
  if(num_float > 0)
  {
    floats = new float[num_float];
    for(i=0; i<num_float; i++) floats[i] = 0.0f;
  }
  if(num_string > 0)
  {
    strings = new CHARPTR[num_string];
    for(i=0; i<num_string; i++)
    {
      strings[i] = new char [1];
      *strings[i] = '\0';
    }
  }
}

rlDataProvider::~rlDataProvider()
{
  int i;

  if(ints    != NULL) delete [] ints;
  if(floats  != NULL) delete [] floats;
  for(i=0; i<num_string; i++)
  {
    if(strings[i] != NULL) delete [] strings[i];
  }
  if(strings != NULL) delete [] strings;
}

int rlDataProvider::getInt(int id)
{
  int ret;

  if(id < 0)            return -1;
  if(id >= num_integer) return -1;
  mutex.lock();
  ret = ints[id];
  mutex.unlock();
  return ret;
}

float rlDataProvider::getFloat(int id)
{
  float ret;

  if(id < 0)          return 0.0f;
  if(id >= num_float) return 0.0f;
  mutex.lock();
  ret = floats[id];
  mutex.unlock();
  return ret;
}

int rlDataProvider::getIntArray(int id, int *array, int nmax)
{
  int ret;
  const char *cptr = getString(id);
  if(cptr == NULL) return -1;

  ret = 0;
  while(1)
  {
    if(cptr == NULL || *cptr == '\0') break;
    if(*cptr != ',' && *cptr != '\"')
    {
      sscanf(cptr,"%d",&array[ret++]);
      cptr = strchr(cptr,',');
      if(cptr != NULL) cptr++;
      if(ret >= nmax) break;
    }
    else
    {
      cptr++;
    }
  }
  return ret;
}

int rlDataProvider::getFloatArray(int id, float *array, int nmax)
{
  int ret;
  const char *cptr = getString(id);
  if(cptr == NULL) return -1;

  ret = 0;
  while(1)
  {
    if(cptr == NULL || *cptr == '\0') break;
    if(*cptr != ',' && *cptr != '\"')
    {
      sscanf(cptr,"%f",&array[ret++]);
      cptr = strchr(cptr,',');
      if(cptr != NULL) cptr++;
      if(ret >= nmax) break;
    }
    else
    {
      cptr++;
    }
  }
  return ret;
}

const char *rlDataProvider::getString(int id)
{
  const char *ret;

  if(id < 0)           return NULL;
  if(id >= num_string) return NULL;
  mutex.lock();
  ret = strings[id];
  mutex.unlock();
  return ret;
}

int rlDataProvider::setInt(int id, int i)
{
  if(id < 0)            return -1;
  if(id >= num_integer) return -1;
  mutex.lock();
  ints[id] = i;
  mutex.unlock();
  return 0;
}

int rlDataProvider::setFloat(int id, float f)
{
  if(id < 0)          return -1;
  if(id >= num_float) return -1;
  mutex.lock();
  floats[id] = f;
  mutex.unlock();
  return 0;
}

int rlDataProvider::setIntArray(int id, int *array, int num)
{
  char buf[rl_PRINTF_LENGTH],val[100];
  int nmax,ipos,ind;

  if(num <= 0) return -1;
  nmax = rl_PRINTF_LENGTH - 1;
  buf[0] = '\0';
  ipos = strlen(buf);
  for(ind=0; ind<num; ind++)
  {
    sprintf(val,"%d,",array[ind]);
    if(ipos+(int)strlen(val) >= nmax) return -1;
    strcat(buf,val);
    ipos = strlen(buf);
  }
  return setString(id,buf);
}

int rlDataProvider::setFloatArray(int id, float *array, int num)
{
  char buf[rl_PRINTF_LENGTH],val[100];
  int nmax,ipos,ind;

  if(num <= 0) return -1;
  nmax = rl_PRINTF_LENGTH - 1;
  buf[0] = '\0';
  ipos = strlen(buf);
  for(ind=0; ind<num; ind++)
  {
    sprintf(val,"%f,",array[ind]);
    if(ipos+(int)strlen(val) >= nmax) return -1;
    strcat(buf,val);
    ipos = strlen(buf);
  }
  return setString(id,buf);
}

int rlDataProvider::setString(int id, const char *str)
{
  if(id < 0)           return -1;
  if(id >= num_string) return -1;
  mutex.lock();
  if(strings[id] != NULL) delete [] strings[id];
  strings[id] = new char[strlen(str)+1];
  strcpy(strings[id],str);
  mutex.unlock();
  return 0;
}

int rlDataProvider::getIntAndReset(int id)
{
  int ret;

  if(id < 0)            return -1;
  if(id >= num_integer) return -1;
  mutex.lock();
  ret = ints[id];
  ints[id] = -1;
  mutex.unlock();
  return ret;
}

int rlDataProvider::setIntAndWaitForReset(int id, int i)
{
  if(id < 0)            return -1;
  if(id >= num_integer) return -1;
  while(1)
  {
    mutex.lock();
    if(ints[id] == -1)
    {
      ints[id] = i;
      mutex.unlock();
      return 0;
    }
    mutex.unlock();
    rlsleep(10);
  }
}

int rlDataProvider::setInt0Semaphore(int i)
{
  int ret;

  ret = setInt(0,i);
  int0semaphore.incrementSemaphore();
  return ret;
}

int rlDataProvider::getInt0Semaphore()
{
  int0semaphore.waitSemaphore();
  return getInt(0);
}

int rlDataProvider::run(rlSocket *socket)
{
  int   id,ival,ret;
  float fval;
  char  cval[rl_PRINTF_LENGTH];
  const char *cptr;
  rlInterpreter interpreter;

  while(socket->isConnected())
  {
    ret = socket->readStr(interpreter.line,rl_PRINTF_LENGTH-1);
    if(ret <= 0) break;
    if     (interpreter.isCommand("int("))
    {
      sscanf(interpreter.line,"int(%d,%d)",&id,&ival);
      setInt(id,ival);
    }
    else if(interpreter.isCommand("intAndReset("))
    {
      sscanf(interpreter.line,"intAndReset(%d,%d)",&id,&ival);
      setIntAndWaitForReset(id,ival);
    }
    else if(interpreter.isCommand("float("))
    {
      sscanf(interpreter.line,"float(%d,%f)",&id,&fval);
      setFloat(id,fval);
    }
    else if(interpreter.isCommand("string("))
    {
      sscanf(interpreter.line,"string(%d,",&id);
      interpreter.copyStringParam(cval,0);
      setString(id,cval);
    }
    else if(interpreter.isCommand("getInt("))
    {
      sscanf(interpreter.line,"getInt(%d)",&id);
      socket->printf("intResult(%d)\n",getInt(id));
    }
    else if(interpreter.isCommand("getIntAndReset("))
    {
      sscanf(interpreter.line,"getIntAndReset(%d)",&id);
      socket->printf("intResult(%d)\n",getIntAndReset(id));
    }
    else if(interpreter.isCommand("getInt0Semaphore("))
    {
      socket->printf("intResult(%d)\n",getInt0Semaphore());
    }
    else if(interpreter.isCommand("getFloat("))
    {
      sscanf(interpreter.line,"getFloat(%d)",&id);
      socket->printf("floatResult(%f)\n",getFloat(id));
    }
    else if(interpreter.isCommand("getString("))
    {
      sscanf(interpreter.line,"getString(%d)",&id);
      cptr = getString(id);
      if(cptr == NULL) socket->printf("stringResult(\"\")\n");
      else             socket->printf("stringResult(\"%s\")\n",cptr);
    }
  }
  return 0;
}

//------------------------------------------------------------------------------------
#define CLIENT_TIMEOUT 1000

rlDataProviderClient::rlDataProviderClient()
{
  rlwsa();
}

rlDataProviderClient::~rlDataProviderClient()
{
}

int rlDataProviderClient::getInt(rlSocket *socket, int id, int *status)
{
  int ret;

  *status = -1;
  if(socket->isConnected() == 0) return 0;
  socket->printf("getInt(%d)\n",id);
  ret = socket->readStr(interpreter.line,rl_PRINTF_LENGTH-1,CLIENT_TIMEOUT);
  if(ret == 0)  // timeout
  {
    socket->disconnect();
    socket->connect();
    *status = -2;
    return 0;
  }
  if(interpreter.isCommand("intResult("))
  {
    sscanf(interpreter.line,"intResult(%d)",&ret);
    *status = 0;
    return ret;
  }
  return 0;
}

float rlDataProviderClient::getFloat(rlSocket *socket, int id, int *status)
{
  float ret;
  int iret;

  *status = -1;
  if(socket->isConnected() == 0) return 0.0f;
  socket->printf("getFloat(%d)\n",id);
  iret = socket->readStr(interpreter.line,rl_PRINTF_LENGTH-1,CLIENT_TIMEOUT);
  if(iret == 0) // timeout
  {
    socket->disconnect();
    socket->connect();
    *status = -2;
    return 0;
  }
  if(interpreter.isCommand("floatResult("))
  {
    sscanf(interpreter.line,"floatResult(%f)",&ret);
    *status = 0;
    return ret;
  }
  return 0.0f;
}

int rlDataProviderClient::getIntArray(rlSocket *socket, int id, int *array, int nmax)
{
  int status,ret;
  const char *cptr = getString(socket,id,&status);
  if(status != 0) return -1;

  ret = 0;
  while(1)
  {
    if(cptr == NULL || *cptr == '\0') break;
    if(*cptr != ',' && *cptr != '\"')
    {
      sscanf(cptr,"%d",&array[ret++]);
      cptr = strchr(cptr,',');
      if(cptr != NULL) cptr++;
      if(ret >= nmax) break;
    }
    else
    {
      cptr++;
    }
  }
  return ret;
}

int rlDataProviderClient::getFloatArray(rlSocket *socket, int id, float *array, int nmax)
{
  int status,ret;
  const char *cptr = getString(socket,id,&status);
  if(status != 0) return -1;

  ret = 0;
  while(1)
  {
    if(cptr == NULL || *cptr == '\0') break;
    if(*cptr != ',' && *cptr != '\"')
    {
      sscanf(cptr,"%f",&array[ret++]);
      cptr = strchr(cptr,',');
      if(cptr != NULL) cptr++;
      if(ret >= nmax) break;
    }
    else
    {
      cptr++;
    }
  }
  return ret;
}

const char *rlDataProviderClient::getString(rlSocket *socket, int id, int *status)
{
  int ret;

  *status = -1;
  if(socket->isConnected() == 0) return NULL;
  socket->printf("getString(%d)\n",id);
  ret = socket->readStr(interpreter.line,rl_PRINTF_LENGTH-1,CLIENT_TIMEOUT);
  if(ret == 0) // timeout
  {
    socket->disconnect();
    socket->connect();
    *status = -2;
    return 0;
  }
  if(interpreter.isCommand("stringResult("))
  {
    interpreter.copyStringParam(cret,0);
    *status = 0;
    return cret;
  }
  return NULL;
}

int rlDataProviderClient::setInt(rlSocket *socket, int id, int i)
{
  if(socket->isConnected() == 0) return -1;
  return socket->printf("int(%d,%d)\n",id,i);
}

int rlDataProviderClient::setFloat(rlSocket *socket, int id, float f)
{
  if(socket->isConnected() == 0) return -1;
  return socket->printf("float(%d,%d)\n",id,f);
}

int rlDataProviderClient::setIntArray(rlSocket *socket, int id, int *i, int num)
{
  char buf[rl_PRINTF_LENGTH],val[100];
  int nmax,ipos,ind;

  if(num <= 0) return -1;
  nmax = rl_PRINTF_LENGTH - strlen("\")\n") - 1;
  sprintf(buf,"string(%d,\"",id);
  ipos = strlen(buf);
  for(ind=0; ind<num; ind++)
  {
    sprintf(val,"%d,",i[ind]);
    if(ipos+(int)strlen(val) >= nmax) return -1;
    strcat(buf,val);
    ipos = strlen(buf);
  }
  strcat(buf,"\"\n");
  return socket->printf(buf);
}

int rlDataProviderClient::setFloatArray(rlSocket *socket, int id, float *f, int num)
{
  char buf[rl_PRINTF_LENGTH],val[100];
  int nmax,ipos,ind;

  if(num <= 0) return -1;
  nmax = rl_PRINTF_LENGTH - strlen("\")\n") - 1;
  sprintf(buf,"string(%d,\"",id);
  ipos = strlen(buf);
  for(ind=0; ind<num; ind++)
  {
    sprintf(val,"%f,",f[ind]);
    if(ipos+(int)strlen(val) >= nmax) return -1;
    strcat(buf,val);
    ipos = strlen(buf);
  }
  strcat(buf,"\"\n");
  return socket->printf(buf);
}

int rlDataProviderClient::setString(rlSocket *socket, int id, const char *str)
{
  if(socket->isConnected() == 0) return -1;
  return socket->printf("string(%d,\"%d\")\n",id,str);
}

int rlDataProviderClient::getIntAndReset(rlSocket *socket, int id, int *status)
{
  int ret;

  *status = -1;
  if(socket->isConnected() == 0) return 0;
  socket->printf("getIntAndReset(%d)\n",id);
  ret = socket->readStr(interpreter.line,rl_PRINTF_LENGTH-1,CLIENT_TIMEOUT);
  if(ret == 0) // timeout
  {
    socket->disconnect();
    socket->connect();
    *status = -2;
    return 0;
  }
  if(interpreter.isCommand("intResult("))
  {
    sscanf(interpreter.line,"intResult(%d)",&ret);
    *status = 0;
    return ret;
  }
  return 0;
}

int rlDataProviderClient::setIntAndWaitForReset(rlSocket *socket, int id, int i)
{
  if(socket->isConnected() == 0) return -1;
  return socket->printf("intAndReset(%d,%d)\n",id,i);
}

int rlDataProviderClient::getInt0Semaphore(rlSocket *socket, int *status)
{
  int ret;

  *status = -1;
  if(socket->isConnected() == 0) return 0;
  socket->printf("getInt0Semaphore()\n");
  ret = socket->readStr(interpreter.line,rl_PRINTF_LENGTH-1,CLIENT_TIMEOUT*60*60); // 1 hour timeout
  if(ret == 0) { *status = -2; return 0; } // timeout
  if(interpreter.isCommand("intResult("))
  {
    sscanf(interpreter.line,"intResult(%d)",&ret);
    *status = 0;
    return ret;
  }
  return 0;
}

//------------------------------------------------------------------------------------
typedef struct
{
  rlDataProviderThreads *thread;
  int socket;
}WORKER_DATA;

static void *workerThread(void *arg)
{
  WORKER_DATA *worker_data;
  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  if(arg == NULL) return NULL;
  worker_data = (WORKER_DATA *) p->user;
  rlSocket socket(worker_data->socket);

  worker_data->thread->provider->run(&socket);

  return NULL;
}

static void *rlDataProviderAccept(void *arg)
{
  int s;
  rlThread worker;
  WORKER_DATA worker_data;
  rlDataProviderThreads *provider_threads;
  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  if(arg == NULL) return NULL;
  provider_threads = (rlDataProviderThreads *) p->user;
  if(provider_threads == NULL) return NULL;
  rlSocket socket("localhost",provider_threads->port,0);

  while(1)
  {
    s = socket.connect();
    if(s == -1) break;
    worker_data.thread = provider_threads;
    worker_data.socket = s;
    worker.create(workerThread,&worker_data);
    rlsleep(100);
  }
  return NULL;
}

rlDataProviderThreads::rlDataProviderThreads(int Port, rlDataProvider *Provider)
{
  rlwsa();
  port = Port;
  provider = Provider;
}

rlDataProviderThreads::~rlDataProviderThreads()
{
}

void rlDataProviderThreads::start()
{
  if(port <= 0)        return;
  if(port >= 256*256)  return;
  if(provider == NULL) return;
  thread.create(rlDataProviderAccept,this);
}
