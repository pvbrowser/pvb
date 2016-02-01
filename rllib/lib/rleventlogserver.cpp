/***************************************************************************
                       rleventlogserver.cpp  -  description
                             -------------------
    begin                : Wed Dec 18 2002
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
#include "rlsocket.h"
#include "rleventlogserver.h"
#include "rlcutil.h"
#include "rltime.h"
#include "stdio.h"
#include "string.h"

rlEventLogServer::rlEventLogServer(const char *file, int max)
{
  for(int i=0; i<rlMAX_MESSAGES; i++) // init memory
  {
    sprintf(&memory[i*rlMAX_EVENT]," ");
  }
  front = -1;
  cnt   = 0;
  if(file != NULL)
  {
    filename = new char [strlen(file)+1];
    strcpy(filename,file);
  }
  fp = NULL;
  max_events = max;
  num_events = 0;
}

rlEventLogServer::~rlEventLogServer()
{
  if(filename != NULL) delete [] filename;
  if(fp != NULL) fclose((FILE *) fp);
}

const char *rlEventLogServer::getEvent(char *buf, int *num)
{
  char *cptr;

  mutex.lock();
  if(*num == -1 && front != -1) *num = front+1;   // read all
  if(*num >= rlMAX_MESSAGES) *num = -1;
  if(*num == front || front == -1)                // nothing to read
  {
    buf[0] = '\0';
    mutex.unlock();
    return NULL;
  }
  else                                            // read next record
  {
    (*num)++;
    if(*num >= rlMAX_MESSAGES) 
    {
      *num = 0;
      if(*num == front)
      {
        buf[0] = '\0';
        mutex.unlock();
        return NULL;
      }  
    }
    strcpy(buf,&memory[(*num)*rlMAX_EVENT]);
    cptr = strchr(buf,'\n');
    if(cptr != NULL) *cptr = '\0';                // delete newline
    mutex.unlock();
    // if(buf[0] < ' ') return "";
    return buf;
  }
}

void rlEventLogServer::putEvent(const char *event)
{
  mutex.lock();
  front++;
  if(front >= rlMAX_MESSAGES) front = 0;
  rlstrncpy(&memory[front*rlMAX_EVENT],event,rlMAX_EVENT-1);
  if(fp == NULL && filename != NULL)
  {
    char name[1024],time[50];
    rlTime t;

    t.getLocalTime();
    strcpy(time,t.getTimeString()); time[10] = '_'; time[13] = '_'; time[16] = '\0';
    strcpy(name,filename);
    strcat(name,time);
    strcat(name,".rlEventLog");
    //printf("openEventlog(%s)\n",name);
#ifdef __VMS
    fp = (void *) fopen(name,"w","shr=get");
#else
    fp = (void *) fopen(name,"w");
#endif  
  }
  if(fp != NULL && filename != NULL)
  {
    fprintf((FILE *) fp,"%s",event);
    fflush((FILE *) fp);
    num_events++;
  }
  if(num_events > max_events && filename != NULL)
  {
    num_events = 0;
    fclose((FILE *) fp);
    fp = NULL;
  }
  mutex.unlock();
}

//------------------------------------------------------------------------------------------
typedef struct
{
  rlEventLogServerThreads *thread;
  int socket;
}
WORKER_DATA;

static void *workerThread(void *arg)
{
  char                     message[rlMAX_EVENT];
  int                      s,ret;
  WORKER_DATA             *worker_data;
  rlEventLogServerThreads *thread;
  rlSocket                *socket;

  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  worker_data     = (WORKER_DATA *) p->user;
  s               = worker_data->socket;
  thread          = worker_data->thread;
  socket          = new rlSocket(s);

  while(socket->isConnected())
  {
    ret = socket->readStr(message,sizeof(message)-1);
    if(ret > 0) thread->event_log_server->putEvent(message);
  }
  delete socket;
  return NULL;
}

static void *rlAcceptThread(void *arg)
{
  int port,s;
  rlThread worker;
  WORKER_DATA worker_data;
  rlEventLogServerThreads *thread;
  rlSocket *socket;
  THREAD_PARAM *p = (THREAD_PARAM *) arg;

  thread = (rlEventLogServerThreads *) p->user;
  port = thread->getPort();
  socket = new rlSocket("localhost",port,0);
  while(1)
  {
    s = socket->connect();
    if(s == -1) break;
    worker_data.thread = thread;
    worker_data.socket = s;
    worker.create(workerThread,&worker_data);
    rlsleep(100);
  }
  delete socket;
  return NULL;
}

rlEventLogServerThreads::rlEventLogServerThreads(int Port, rlEventLogServer *EventLogServer)
{
  rlwsa();
  port = Port;
  event_log_server = EventLogServer;
}

rlEventLogServerThreads::~rlEventLogServerThreads()
{
}

void rlEventLogServerThreads::start()
{
  if(port <= 0)                return;
  if(port >= 256*256)          return;
  if(event_log_server == NULL) return;
  acceptThread.create(rlAcceptThread,this);
}

int rlEventLogServerThreads::getPort()
{
  return port;
}
