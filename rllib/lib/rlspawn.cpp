/***************************************************************************
                          rlspawn.cpp  -  description
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
#include "rldefine.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifndef RLWIN32
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#endif
#include "rlspawn.h"
#include "rlcutil.h"
#ifdef RLUNIX
#include <sys/wait.h>
#include <signal.h>
#endif

rlSpawn::rlSpawn()
{
  toChild = fromChild = NULL;
  pid = 0;
}

rlSpawn::~rlSpawn()
{
  if(toChild   != NULL) ::fclose((FILE*) toChild);
  if(fromChild != NULL) ::fclose((FILE*) fromChild);
}

int rlSpawn::spawn(const char *command)
{
#ifdef RLWIN32 
  return -1;
#else  
  int to_child[2],from_child[2],ret;

  if(toChild   != NULL) ::fclose((FILE*) toChild);
  if(fromChild != NULL) ::fclose((FILE*) fromChild);
  toChild = fromChild = NULL;

  ret = ::pipe(to_child);
  if(ret == -1) return -1;
  ret = ::pipe(from_child);
  if(ret == -1) return -1;

  if((pid = ::fork()) == 0)
  {
    if(to_child[0] != 0) // stdin
    {
      ::dup2(to_child[0],0);
      ::close(to_child[0]);
    }
    if(from_child[1] != 2) // stderr
    {
      ::dup2(from_child[1] ,2);
    }
    if(from_child[1] != 1) // stdout
    {
      ::dup2(from_child[1],1);
      ::close(from_child[1]);
    }
    ::close(to_child[1]);
    ::close(from_child[0]);
    ::rlexec(command);
    ::exit(0);
  }

  ::close(to_child[0]);
  ::close(from_child[1]);
  toChild   = (void*) ::fdopen(to_child[1],"w");
  if(toChild == NULL)   {                            return -1; }
  fromChild = (void*) ::fdopen(from_child[0],"r");
  if(fromChild == NULL) { ::fclose((FILE*) toChild); return -1; }
  return pid;
#endif
}

const char *rlSpawn::readLine()
{
  if(fromChild == NULL) return NULL;
  if(::fgets(line,sizeof(line)-1,(FILE*) fromChild) == NULL)
  {
#ifdef RLUNIX
    if(pid != 0)
    {
      int status;
      waitpid(pid, &status, 0);
      kill(pid,SIGHUP);
    }
#endif
    return NULL;
  }
  return line;
}

int rlSpawn::getchar()
{
  if(fromChild == NULL) return EOF;
  return ::fgetc((FILE*) fromChild);
}

int rlSpawn::write(const char *buf, int len)
{
#ifdef RLWIN32
  return -1;
#else  
  if(toChild == NULL) return -1;
  return ::write(fileno((FILE*)toChild),buf,len);
#endif
}

int rlSpawn::printf(const char *format, ...)
{
  int ret;
  char message[rl_PRINTF_LENGTH]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  return write(message,strlen(message));
}

int rlSpawn::writeString(const char *buf)
{
  if(toChild == NULL) return -1;
  return fprintf((FILE*)toChild,"%s",buf);
}

void rlSpawn::printAll()
{
  const char *cptr;
  while((cptr=readLine()) != NULL) ::printf("%s",cptr);
}

int rlSpawn::select(int timeout)
{
#ifdef RLWIN32
  return -1;
#else
  struct timeval timout;
  fd_set wset,rset,eset;
  int    ret,maxfdp1,s;

  if(fromChild == NULL) return -1;
  s = fileno((FILE *) fromChild);
  /* setup sockets to read */
  maxfdp1 = s+1;
  FD_ZERO(&rset);
  FD_SET (s,&rset);
  FD_ZERO(&wset);
  FD_ZERO(&eset);
  timout.tv_sec  =  timeout / 1000;
  timout.tv_usec = (timeout % 1000) * 1000;

  ret = ::select(maxfdp1,&rset,&wset,&eset,&timout);
  if(ret == 0) return 0; /* timeout */
  return 1;
#endif  
}


