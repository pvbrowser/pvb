/***************************************************************************
                          rlcommandlineinterface.cpp  -  description
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
#include "rlcommandlineinterface.h"
#include "rlstring.h"
#include "rlcutil.h"
#include "rlthread.h"
#include <unistd.h>
#include <stdarg.h>

rlCommandlineInterface::rlCommandlineInterface()
{
  sock = NULL;
  spawn = NULL;
  tty = NULL;
}

rlCommandlineInterface::~rlCommandlineInterface()
{
  if(sock != NULL) delete sock;
  if(spawn != NULL) delete spawn;
}

int rlCommandlineInterface::start(const char *how, const char *command)
{
  if(sock != NULL) delete sock;
  sock = NULL;
  if(spawn != NULL) delete spawn;
  spawn = NULL;
  tty = NULL;

  if(strcmp(how,"pipe") == 0)
  {
    if(command == NULL) return -1;
    spawn = new rlSpawn();
    int ret = spawn->spawn(command);
    if(ret < 0)
    {
      delete spawn;
      spawn = NULL;
      return -1;
    }
    return ret;
  }
  else if(strcmp(how,"stdio") == 0)
  {
    return 1;
  }
  else
  {
    rlString rlhow(how);
    char *cptr, *host;
    host = rlhow.text();
    cptr = strchr(host,':');
    if(cptr == NULL) return -1;
    *cptr = '\0'; cptr++;
    int port = atoi(cptr);
    if(strcmp(host,"server.localhost") == 0)
    {
      sock = new rlSocket(host,port,0);
    }
    else
    {
      if(strcmp(host,"localhost") == 0 && command != NULL)
      {
        rlString cmd(command);
#ifdef RLUNIX
        cmd += " &";
#endif
        rlsystem(cmd.text());
      }
      sock = new rlSocket(host,port,1);
    }
    for(int itry=0; itry<10; itry++)
    {
      sock->connect();
      if(sock->isConnected()) return sock->s;
      rlsleep(10);
    }  
    return -1;
  }
}

int rlCommandlineInterface::start(rlSerial *ttyinterface)
{
  if(sock != NULL) delete sock;
  sock = NULL;
  if(spawn != NULL) delete spawn;
  spawn = NULL;
  tty = ttyinterface;
  if(tty == NULL) return -1;
  return 1;
}

int rlCommandlineInterface::readBlock(void *buf, int len, int timeout)
{
  if(spawn != NULL)
  {
    unsigned char *cbuf = (unsigned char *) buf;
    int i = 0;
    while(i<len)
    {
      if(timeout > 0)
      {
        if(spawn->select(timeout) == 0) return -1;
      }
      cbuf[i++] = spawn->getchar();
    }  
    return len;
  }
  else if(sock != NULL)
  {
    if(sock->isConnected() == 0) return -1;
    int ret = sock->read(buf,len,timeout);
    if(ret <= 0) 
    {
      sock->disconnect();
      return -1;
    }
    return len; 
  }
  else if(tty != NULL)
  {
    int timout = 0;
    if(timeout > 0) timout = timeout;
    return tty->readBlock ((unsigned char *) buf, len, timout);
  }
  else
  {
   int ret = read(0,buf,len);
   return ret;
  }
}

const char *rlCommandlineInterface::readLine(int timeout)
{
  if(spawn != NULL)
  {
    if(timeout > 0)
    {
      if(spawn->select(timeout) == 0) return NULL;
    }
    return spawn->readLine();
  }
  else if(sock != NULL)
  {
    if(sock->isConnected() == 0) return NULL;
    int ret = sock->readStr(line,sizeof(line)-1,timeout);
    if(ret <= 0) 
    {
      sock->disconnect();
      return NULL;
    }
    return line; 
  }
  else if(tty != NULL)
  {
    int ret = tty->readLine((unsigned char *) line,sizeof(line)-1, timeout);
    if(ret <= 0) return NULL;
    return line;
  }
  else
  {
    return fgets(line,sizeof(line)-1,stdin);
  }
}

int rlCommandlineInterface::printf(const char *format, ...)
{
  va_list ap;
  va_start(ap,format);
  int ret = rlvsnprintf(line, sizeof(line) - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;

  if(spawn != NULL)
  {
    return spawn->printf("%s",line);
  }
  else if(sock != NULL)
  {
    if(sock->isConnected() == 0) return -1;
    int ret = sock->printf("%s",line);
   return ret;
  }
  else if(tty != NULL)
  {
    return tty->writeBlock((unsigned char *) line, strlen(line));
  }
  else
  {
   int ret = ::printf("%s",line);
   fflush(stdout);
   return ret;
  }
}

int rlCommandlineInterface::writeBlock(void *buf, int len)
{
  if(spawn != NULL)
  {
    return spawn->write((const char *) buf, len);
  }
  else if(sock != NULL)
  {
    if(sock->isConnected() == 0) return -1;
    return sock->write(buf,len);
  }
  else if(tty != NULL)
  {
    return tty->writeBlock((unsigned char *) buf, len);
  }
  else
  {
    return write(1,buf,len);
  }
}

#ifdef TESTING
int main()
{
  const char *line;
  rlCommandlineInterface cli;

  // connect to a server over TCP
  // or be a TCP server if how = "server.localhost:port"
  printf("localhost:\n");
  cli.start("localhost:50500");
  int i = 0;
  while((line = cli.readLine()) != NULL)
  {
    printf("line=%s",line);
    if(i<5) cli.printf("%s%d\n","hallo",i++);
    else    cli.printf("%s\n","exit");
  }

  // run command and connect it over a pipe to us
  printf("pipe:\n");
  cli.start("pipe","./myecho");
  i = 0;
  while((line = cli.readLine()) != NULL)
  {
    ::printf("line=%s",line);
    if(i<5) cli.printf("%s%d\n","hallo",i++);
    else    cli.printf("%s\n","exit");
  }

  // communicate via stdin / stdout
  printf("stdio:\n");
  cli.start("stdio");
  cli.printf("Bitte tipp was ein\n");
  printf("Du hast '%s' eingegeben\n", cli.readLine());

  return 0;
}
#endif

