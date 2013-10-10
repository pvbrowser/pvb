/***************************************************************************
                          rldataprovider.h  -  description
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
#ifndef _RL_DATA_PROVIDER_H_
#define _RL_DATA_PROVIDER_H_

#include "rldefine.h"
#include "rlsocket.h"
#include "rlthread.h"
#include "rlinterpreter.h"

/*! <pre>
This class is a container for data.
You can get/set the data identified by id.
The run method starts a separate thread that makes rlDataProvider available over TCP.
</pre> */
class rlDataProvider
{
  public:
    rlDataProvider(int numInteger, int numFloat=0, int numString=0);
    virtual ~rlDataProvider();
    int         getInt   (int id);
    float       getFloat (int id);
    int         getIntArray   (int id, int   *i, int nmax);
    int         getFloatArray (int id, float *f, int nmax);
    const char *getString(int id);
    int         setInt   (int id, int         i);
    int         setFloat (int id, float       f);
    int         setIntArray   (int id, int    *i, int num);
    int         setFloatArray (int id, float  *f, int num);
    int         setString(int id, const char *str);
    int         getIntAndReset(int id);
    int         setIntAndWaitForReset(int id, int i);
    int         setInt0Semaphore(int i);
    int         getInt0Semaphore();
    int         run(rlSocket *socket);
  private:
    typedef char* CHARPTR;
    int     *ints;
    float   *floats;
    char    **strings;
    int     num_integer, num_float, num_string;
    rlMutex mutex;
    rlSemaphore int0semaphore;
};

/*! <pre>
This class is a client that can access rlDataProvider over TCP.
</pre> */
class rlDataProviderClient
{
  public:
    rlDataProviderClient();
    virtual ~rlDataProviderClient();
    int         getInt               (rlSocket *socket, int id, int   *status);
    float       getFloat             (rlSocket *socket, int id, int   *status);
    int         getIntArray          (rlSocket *socket, int id, int   *array, int nmax);
    int         getFloatArray        (rlSocket *socket, int id, float *array, int nmax);
    const char *getString            (rlSocket *socket, int id, int   *status);
    int         setInt               (rlSocket *socket, int id, int         i);
    int         setFloat             (rlSocket *socket, int id, float       f);
    int         setIntArray          (rlSocket *socket, int id, int        *i, int num);
    int         setFloatArray        (rlSocket *socket, int id, float      *f, int num);
    int         setString            (rlSocket *socket, int id, const char *str);
    int         getIntAndReset       (rlSocket *socket, int id, int   *status);
    int         setIntAndWaitForReset(rlSocket *socket, int id, int         i);
    int         getInt0Semaphore     (rlSocket *socket,         int   *status);
  private:
    rlInterpreter interpreter;
    char  cret[rl_PRINTF_LENGTH];
};

/*! <pre>
This class starts a separate thread.
The thread is accepting clients, that want access to rlDataProvider.
</pre> */
class rlDataProviderThreads
{
  public:
    rlDataProviderThreads(int Port, rlDataProvider *Provider);
    virtual ~rlDataProviderThreads();
    void start();
    rlDataProvider *provider;
    rlThread        thread;
    int port;
};

#endif
