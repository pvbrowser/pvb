/***************************************************************************
                          rlspawn.h  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_SPAWN_H_
#define _RL_SPAWN_H_

#include "rldefine.h"

#ifndef RLUNIX
//#warning This Functionality is only available under Linux/Unix
/*
http://msdn.microsoft.com/en-us/library/windows/desktop/aa365574%28v=vs.85%29.aspx
http://www.tenouk.com/cpluscodesnippet/pipeandchildprocess.html
http://msdn.microsoft.com/en-us/library/windows/desktop/aa365780%28v=vs.85%29.aspx

GetFileNameFromHandle
http://msdn.microsoft.com/en-us/library/windows/desktop/aa366789%28v=vs.85%29.aspx
*/
#endif

/*! <pre>
Spawn an external program.
Redirect \<stdin> \<stdout> \<stderr> of external program to this class

Now you can communicate with this external program over a pipe.
Attention: This class is only available on unix like systems.
</pre> */
class rlSpawn
{
public:
  rlSpawn();
  virtual ~rlSpawn();

  /*! <pre>
  Start an operating system command.
  The output from the command can be read with readLine()
  You can write to the program with writeString() or write()
  Return: 0=success -1=error
  </pre> */
  int spawn(const char *command);

  /*! <pre>
  Read a line from the spawned command.
  When the command terminates NULL is returned.
  </pre> */
  const char *readLine();

  /*! <pre>
  Read a char from the spawned command.
  When the command terminates EOF is returned.
  </pre> */
  int getchar();

  /*! <pre>
  Wait for characters.
  return = 0 // timeout
  return = 1 // characters available
  </pre> */
  int select(int timeout=50);

  /*! <pre>
  Write buf to \<stdin> of spawned command
  buf must be 0 terminated
  Return: number of bytes written
          -1 error
  </pre> */
  int writeString(const char *buf);

  /*! <pre>
  Write buf to \<stdin> of spawned command
  Return: number of bytes written
          -1 error
  </pre> */
  int write(const char *buf, int len);

  /*! <pre>
  similar to printf
  Return: number of bytes written
          -1 error
  </pre> */
  int printf(const char *format, ...);

  /*! <pre>
  Print all outputs from spawned command to \<stdout>
  </pre> */
  void printAll();

  /*! <pre>
  Get FILE pointer fromChild
  </pre> */
  FILE *getFilepointer();

  int pid;

private:
#ifdef RLWIN32
  HANDLE toChildRD;
  HANDLE toChildWR;
  HANDLE fromChildRD;
  HANDLE fromChildWR;
  HANDLE hThread;
  HANDLE hProcess;
#else
  void *toChild,*fromChild;
#endif  
  char line[4096]; // adjust this if the buffer is not big enough
};

#endif
