/***************************************************************************
                          rleventlogserver.h  -  description
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
#ifndef _RL_EVENT_LOG_SERVER_H_
#define _RL_EVENT_LOG_SERVER_H_

#include "rldefine.h"
#include "rlevent.h"
#include "rlthread.h"

#define rlMAX_MESSAGES 128

/*! <pre>
A class for implementing an event log server.
You may use it with rlevent.h
</pre> */
class rlEventLogServer
{
  public:
    /*! <pre>
    the event log files will be called:
    filename<date-and-time>.rlEventLog
    max_events = max number of events in one file
    files may be purged by cron
    </pre> */
    rlEventLogServer(const char *filename=NULL, int max_events=10000);
    virtual ~rlEventLogServer();

    /*! <pre>
    num is the event number
    num = -1 will retrieve all events
    </pre> */
    const char *getEvent(char *buf, int *num);
    void        putEvent(const char *event);

  private:
    char    memory[rlMAX_MESSAGES*rlMAX_EVENT];
    rlMutex mutex;
    int     front;
    int     cnt;
    char    *filename;
    void    *fp;
    int     max_events, num_events;
};

/*! <pre>
This class starts a separate thread, that runs a rlEventLogServer log server.
It waits on TCP port for clients. See rlevent.h
</pre> */
class rlEventLogServerThreads
{
  public:
    /*! <pre>
    event_log_server will not be deleted by the destructor
    </pre> */
    rlEventLogServerThreads(int port, rlEventLogServer *event_log_server);
    virtual ~rlEventLogServerThreads();
    void start();
    int getPort();
    rlEventLogServer *event_log_server;

  private:
    rlThread acceptThread;
    int port;
};

#endif
