/****************************************************************************
**
** Copyright (C) 2000-2006 Lehrig Software Engineering.
**
** This file is part of the pvbrowser project.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "pvdefine.h"
#include "opt.h"

#ifndef IS_OLD_MSVCPP
#define AF_INET6_IS_AVAILABLE
#endif

#define _QTSOCK_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __FreeBSD__
#define MSG_NOSIGNAL 0
#endif

#ifdef __VMS
#define MSG_NOSIGNAL 0
#endif

#ifndef PVWIN32
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define closesocket close
#endif

#ifdef PVWIN32
#ifdef IS_OLD_MSVCPP
#include "winsock.h"
#else
//#if (_WIN32_WINNT < 0x0501)
//#warning mingw does not have ipv6 helpers modify mingw header in ws2tcpip.h
#include "winsock2.h"
#include <ws2tcpip.h>
void WSAAPI freeaddrinfo(struct addrinfo*);
int  WSAAPI getaddrinfo(const char*,const char*,const struct addrinfo*, struct addrinfo**);
int  WSAAPI getnameinfo(const struct sockaddr*,socklen_t,char*,DWORD, char*,DWORD,int);
//#undef AF_INET6_IS_AVAILABLE
//#endif
#endif
#define MSG_NOSIGNAL 0
#include <qapplication.h>
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#ifdef AF_INET6_IS_AVAILABLE
static int rl_ipversion = 6; // default: use ip6. client will try ipv4 if server does not support ipv6
#else
static int rl_ipversion = 4; // only necessary because of MSVSCPP 6.0
#endif

extern OPT opt;

int tcp_con(const char *adr, int port)
{
  static int                s;
  static struct sockaddr_in remoteAddr;
  static struct hostent     *host;
  static struct in_addr     RemoteIpAddress;
  int                       ret;
#ifdef AF_INET6_IS_AVAILABLE
  static struct addrinfo    hints, *res, *ressave;
  int                       n;
  char                      portstr[32];
#endif

  if(opt.arg_log) printf("conn=%s:%d\n", adr, port);
  if(rl_ipversion == 4)
  {
    s = socket(AF_INET,SOCK_STREAM,0);
    if(s == -1) return -1;

    /* fill destblk structure */
    host = gethostbyname(adr);
    if(host == NULL)
    {
      /* See if the host is specified in "dot address" form */
      if(strchr(adr,'.') == NULL) return -1;
      RemoteIpAddress.s_addr = inet_addr(adr);
      if(RemoteIpAddress.s_addr == 0x0ffffffff) return -1;
    }
    else
    {
       memcpy(&RemoteIpAddress,host->h_addr,host->h_length);
    }

    memset(&remoteAddr,0,sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(port);
    remoteAddr.sin_addr = RemoteIpAddress;

    ret = connect(s, (struct sockaddr *) &remoteAddr, sizeof(remoteAddr));
    //if(ret == -1)
    if(ret < 0)
    {
      closesocket(s);
      return -1;
    }
  }
  else if(rl_ipversion == 6)
  {
#ifdef AF_INET6_IS_AVAILABLE
    sprintf(portstr,"%d",port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    n = getaddrinfo(adr, portstr, &hints, &res);
    if(n != 0)
    {
#ifndef PVWIN32
      ::printf("tcp_con 1: error for %s port=%s : %s\n", adr, portstr, gai_strerror(n));
#endif
      return -1;
    }
    ressave = res;
    do
    {
      s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
      if(s < 0)                                            continue; // ignore this one
      if(::connect(s, res->ai_addr, res->ai_addrlen) == 0) break;    // success
      closesocket(s);
      s = -1;
    }
    while((res = res->ai_next) != NULL);
    if(res == NULL) ::printf("tcp_con 2: error for %s port=%s\n", adr, portstr);
    freeaddrinfo(ressave);
#else
    ::printf("tcp_con 3:ERROR IPV6 not available on this platform\n");
#endif
  }
  else
  {
    printf("tcp_con 4: ipversion=%d is not supported\n", rl_ipversion);
  }
  return s;
}

int tcp_rec(int *s, char *msg, int len)
{
  int ret,i;

  if(s == NULL) { msg[0] = '\n'; msg[1] = '\0'; return -1; }
  if(*s == -1)  { msg[0] = '\n'; msg[1] = '\0'; return -1; }

  /*
      struct timeval timout;
      fd_set rset;
      FD_ZERO(&rset);
      FD_SET (*s,&rset);
      timout.tv_sec  = 0;
      timout.tv_usec = 500;
      ret = ::select(*s+1,&rset,NULL,NULL,&timout);
      if(ret <= 0)
      {
        printf("select error\n");
        msg[0] = '\n'; msg[1] = '\0';
        return -1;
      }
  */

  i = 0;
  while(i < len-1)
  {
#ifdef PVWIN32
tryagain:
#endif
    ret = recv(*s,&msg[i],1,0);
    if(ret <= 0)
    {
#ifdef PVWIN32
      if(WSAEWOULDBLOCK == WSAGetLastError())
      {
        if(i==0)
        {
          msg[0] = '\0';
          return -2;
        }  
        qApp->processEvents();
        if(*s != -1)
        {
          goto tryagain;
        }  
      }
#endif
      if(*s != -1) closesocket(*s);
      *s = -1;
      msg[0] = '\n';
      msg[1] = '\0';
      if(opt.arg_log) printf("close connection\n");;
      return -1;
    }
    if(msg[i] == '\n')
    {
      i++;
      msg[i] = '\0';
      if(opt.arg_log) printf("recv=%s", msg);
      return i;
    }
    i++;
  }
  msg[i] = '\0';
  if(opt.arg_log) printf("recv=%s", msg);
  return i;
}

int tcp_rec_binary(int *s, char *msg, int len)
{
  int ret,i;

  if(s == NULL) return -1;
  if(*s == -1) return -1;
  i = 0;
  while(i < len)
  {
#ifdef PVWIN32
tryagain:
#endif
    ret = recv(*s,&msg[i],len-i,0);
    if(ret <= 0)
    {
#ifdef PVWIN32
      if(WSAEWOULDBLOCK == WSAGetLastError())
      {
        if(*s != -1)
        {
          goto tryagain;
        }  
      }
#endif
      if(*s != -1) closesocket(*s);
      *s = -1;
      return -1;
    }
    i += ret;
  }
  if(opt.arg_log) printf("recv_binary len=%d bytes\n", i);
  return i;
}

int tcp_send(int *s, const char *msg, int len)
{
  int ret,bytes_left,first_byte;

  ret = 0;
  if(s == NULL) return -1;
  if(*s == -1) return -1;
  bytes_left = len;
  first_byte = 0;

  while(bytes_left > 0)
  {
    ret = send(*s,&msg[first_byte],bytes_left,MSG_NOSIGNAL);
    if(ret <= 0) { closesocket(*s); *s = -1; return -1; }
    bytes_left -= ret;
    first_byte += ret;
  }
  if(opt.arg_log) printf("send=%s", msg);
  return ret;
}

int tcp_close(int *s)
{
  int stemp = *s;
  if(s == NULL) return -1;
  if(stemp == -1) return -1;
  tcp_send(s,"shutup\n",7);
  *s = -1;
  if(stemp != -1) closesocket(stemp);
  if(opt.arg_log) printf("close connection\n");;
  return 0;
}

int tcp_sleep(int msec)
{
#ifdef PVWIN32
  Sleep(msec);
  return 0;
#endif

#ifdef PVUNIX
  fd_set wset,rset,eset;
  struct timeval timeout;

  FD_ZERO(&rset);
  FD_ZERO(&wset);
  FD_ZERO(&eset);
  timeout.tv_sec  = msec / 1000;
  timeout.tv_usec = (msec % 1000) * 1000;
  select(1,&rset,&wset,&eset,&timeout);
  return 0;
#endif

#ifdef __VMS
  struct timespec interval;

  interval.tv_sec  =  msec / 1000;
  interval.tv_nsec = (msec % 1000) * 1000 * 1000; /* wait msec msec */
  pthread_delay_np(&interval);
  return 0;
#endif
}
