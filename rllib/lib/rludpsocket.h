/***************************************************************************
                        rludpsocket.h  -  description
                             -------------------
    begin                : Tue Apr 03 2007
    copyright            : (C) 2007 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_UDP_SOCKET_H_
#define _RL_UDP_SOCKET_H_

#include "rldefine.h"
#include "rlsocket.h"

#ifdef RLWIN32
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <direct.h>
#define  MSG_NOSIGNAL 0
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "unistd.h"
#endif

/*! <pre>
class for encapsulating ip  addresses
</pre> */
class rlIpAdr
{
public:
  rlIpAdr();
  virtual ~rlIpAdr();
  int setAdr(const char *adr, int port);
  int operator== (rlIpAdr &address1);
  struct sockaddr_in address;
};

/*! <pre>
class for encapsulating UDP socket calls
</pre> */
class rlUdpSocket
{
public:
  rlUdpSocket(int debug = 0);
  virtual ~rlUdpSocket();

  /*! <pre>
  setsocketopt for SOL_SOCKET level
  </pre> */
  int setSockopt(int opt);

  /*! <pre>
  setsocketopt with full control
  </pre> */
  int setSockopt(int level, int optname, void *optval, int optlen);

  /*! <pre>
  return > 0 -> socket else error
  </pre> */
  int bind(int port);

  /*! <pre>
  return == 0 -> timeout
  </pre> */
  int select(int timeout);

  /*! <pre>
  return > 0 -> number of bytes read return == 0 -> timeout else error
  </pre> */
  int recvfrom(void *buf, int maxlen, rlIpAdr *source, int timeout = -1);

  /*! <pre>
  return >= 0 -> number of bytes written else error
  </pre> */
  int sendto(const void *buf, int len, rlIpAdr *dest);

  /*! <pre>
  return >= 0 -> number of bytes written else error
  </pre> */
  int printf(rlIpAdr *dest, const char *format, ...);

  int debug, readflag, writeflag;

private:
  struct sockaddr_in address;
  int s;
};

#endif
