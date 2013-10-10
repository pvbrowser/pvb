/***************************************************************************
                          rlcommandlineinterface.h  -  description
                             -------------------
    begin                : Sat Mar 27 2010
    copyright            : (C) 2010 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_COMMANDLINE_INTERFACE_H_
#define _RL_COMMANDLINE_INTERFACE_H_

#include "rldefine.h"
#include "rlsocket.h"
#include "rlserial.h"
#include "rlspawn.h"

/*! <pre>
Commandline interface that allows applications to communicate over pipe || socket || serial line || stdio .

The parameters of start() are as follows:
start("stdio");                    // use stdin and stdout
start("pipe","command");           // run "command" and connect it's stdio with us (runs on unix only)
start("host:5050");                // connect to "host" on port 5050 and communicate with the server (tip: define a server with xinted on "host" port 5050)
start("localhost:5050","command"); // run "command" and then try to connect to "localhost" port 5050 for communication with "command"
start("server.localhost:5050");    // act as a server on "localhost" port 5050 (can serve only 1 client. if you want to serve more clients use rlSocket)
start(tty);                        // use serial interface for communication

Return values:
start()      returns -1 on error
readLine()   returns the read string or NULL
readBlock()  returns the number of read bytes or -1 on error
printf()     returns the number of written characters or -1 on error
writeBlock() returns the number of written bytes or -1 on error
</pre> */
class rlCommandlineInterface
{
public:
  rlCommandlineInterface();
  virtual    ~rlCommandlineInterface();
  int         start(const char *how, const char *command=NULL);
  int         start(rlSerial *tty);
  const char *readLine(int timeout=0);
  int         readBlock(void *buf, int len, int timeout=0);
  int         printf(const char *format, ...);
  int         writeBlock(void *buf, int len);
private:
  char        line[rl_PRINTF_LENGTH];
  rlSocket    *sock;
  rlSpawn     *spawn;
  rlSerial    *tty;
};
#endif

