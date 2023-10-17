/***************************************************************************
                          rlsocket.cpp  -  description
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

#ifndef IS_OLD_MSVCPP
#ifndef __VMS
#define AF_INET6_IS_AVAILABLE
#endif
#endif

#ifdef RLWIN32
//#if (_WIN32_WINNT < 0x0501)
//#warning mingw does not have helpers modify mingw header in ws2tcpip.h
#include <winsock2.h>
#include <ws2tcpip.h>

#define WTREAD_GNUC5 ( __GNUC__ * 1000 ) + __GNUC_MINOR__
#if WTREAD_GNUC5 < 4008
void WSAAPI freeaddrinfo(struct addrinfo*);
int  WSAAPI getaddrinfo(const char*,const char*,const struct addrinfo*, struct addrinfo**);
int  WSAAPI getnameinfo(const struct sockaddr*,socklen_t,char*,DWORD, char*,DWORD,int);
#endif

//#undef AF_INET6_IS_AVAILABLE
//#endif
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

#ifdef __VMS
#define  MSG_NOSIGNAL 0
#endif

#ifdef PVMAC
#define MSG_NOSIGNAL 0
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "rlsocket.h"
#include "rlwthread.h"
#include "rlcutil.h"

/* windows stuff */
int rlwsa()
{
#ifdef RLWIN32
  static int first = 1;
  WORD    wVersionRequested;
  WSADATA wsadata;
  int     err;

  if(first == 1)
  {
#ifdef IS_OLD_MSVCPP
    wVersionRequested = MAKEWORD(1,1);
#else
    wVersionRequested = MAKEWORD(2,0);
#endif
    err = WSAStartup(wVersionRequested, &wsadata);
    if(err != 0)
    {
      ::printf("Startup error=%d on windows\n",err);
      exit(0);
    }
    first = 0;
  }
#endif
  return 0;
}

int rlScoketWrite(int *socket, const void *buf, int len)
{
  int ret,bytes_left,first_byte;
  const char *cbuf;

  if(socket == NULL) return -1;
  if(*socket == -1) return -1;
  cbuf = (char *) buf;
  bytes_left = len;
  first_byte = 0;

  while(bytes_left > 0)
  {
    ret = send(*socket,&cbuf[first_byte],bytes_left,MSG_NOSIGNAL);
    if(ret <= 0)
    {
      //disconnect();
      if(*socket != -1)
      {
#ifdef RLWIN32
        closesocket(*socket);
#else
        close(*socket);
#endif
      }
      *socket = -1;
      return -1;
    }
    bytes_left -= ret;
    first_byte += ret;
  }

  return first_byte;
}

rlSocket::rlSocket(const char *a, int p, int act)
{
  rlwsa(); // init sockets on windows
  setAdr(a);
  port = p;
  active = act;
  s = -1;
  os = -1;
  first = 1;
  rl_ipversion = 4;
  memset(sockaddr,0,sizeof(sockaddr));
}

rlSocket::rlSocket(int socket)
{
  adr[0] = '\0';
  port   = -1;
  active = 0;
  s      = socket;
  os     = -1;
  first  = 0;
  rl_ipversion = 4;
  memset(sockaddr,0,sizeof(sockaddr));
}

rlSocket::~rlSocket()
{
  disconnect();
  if(os != -1 && active == 0)
  {
#ifdef RLWIN32
    closesocket(os);
#else
    close(os);
#endif
  }
}

void rlSocket::setAdr(const char *a)
{
  adr[0] = '\0';
  if(a == NULL) return;
  if((strlen(a)+1) > sizeof(adr)) return;
  strcpy(adr,a);
}

void rlSocket::setPort(int p)
{
  port = p;
}

int rlSocket::getPort()
{
  return port;
}

void rlSocket::setActive(int act)
{
  active = act;
}

int rlSocket::read(void *buf, int len, int timeout)
{
  int  i,ret;
  char *cbuf;

  if(s == -1) return -1;
  if(select(timeout) == 0) return 0; // timeout

  cbuf = (char *) buf;
  i = 0;
  while(i < len)
  {
    ret = recv(s,&cbuf[i],len-i,0);
    if(ret <= 0)
    {
      disconnect();
      return -1;
    }
  
    //::printf("debug: read len=%d buf=", len);
    //for(int j=0; j<len; j++) ::printf(" %d", cbuf[j]);
    //::printf("\n");

    i += ret;
    if(i < len)
    {
      if(select(timeout) == 0) return 0; // timeout
    }
  }

  return i;
}

int rlSocket::readStr(char *buf, int len, int timeout)
{
  int ret,i;

  if(s == -1) return -1;
  if(select(timeout) == 0) return 0; // timeout

  // ::printf("debug: readStr len=%d", len);
  
  i = 0;
  while(1)
  {
#ifdef RLWIN32
tryagain:
#endif
    ret = recv(s,&buf[i],1,0);
    if(ret <= 0)
    {
#ifdef RLWIN32
      if(WSAEWOULDBLOCK == WSAGetLastError()) goto tryagain;
#endif
      disconnect();
      buf[i] = '\0';
      return -1;
    }
    
    //::printf(" %d\n", buf[i]);
    
    if(buf[i] == '\n')
    {
      buf[i+1] = '\0';
      return i+1;
    }
    if(i >= len-1)
    {
      buf[i+1] = '\0';
      return i+1;
    }
    i++;
  }
}

int rlSocket::readHttpHeader(rlString *header, int timeout)
{
  char line[4096];
  int contentLength = 0;
  
  *header = "";
  while(1)
  {
    int ret = readStr(line, (int) sizeof(line)-1, timeout);
    if(ret < 0) 
    {
      printf("ERROR in rlSocket::readHttpHeader() ret=%d\n", ret);
      return -1;
    }
    *header += line;
    if(strstr(line, "Content-Length:") != NULL)
    {
      sscanf(line,"Content-Length: %d", &contentLength);
    }
    if(strlen(line) <= 2)
    {
      return contentLength;
    }
  }
}

int rlSocket::write(const void *buf, int len)
{
  int ret,bytes_left,first_byte;
  const char *cbuf;

  if(s == -1) return -1;
  cbuf = (char *) buf;
  bytes_left = len;
  first_byte = 0;

  //::printf("debug: write len=%d buf=", len);
  //for(int i=0; i<len; i++) ::printf(" %d", cbuf[i]);
  //::printf("\n");

  while(bytes_left > 0)
  {
    ret = send(s,&cbuf[first_byte],bytes_left,MSG_NOSIGNAL);
    if(ret <= 0)
    {
      disconnect();
      return -1;
    }
    bytes_left -= ret;
    first_byte += ret;
  }

  return first_byte;
}

int rlSocket::connect()
{
  int option;
  int ret;
#ifdef __VMS
  size_t    socklen = sizeof(struct sockaddr);
#else
  socklen_t socklen = sizeof(struct sockaddr);
#endif
  struct sockaddr_in     localAddr;
  struct sockaddr_in     remoteAddr;
  struct hostent        *host;
  struct in_addr         RemoteIpAddress;
#ifdef AF_INET6_IS_AVAILABLE
  struct addrinfo        hints0, hints1;
  struct addrinfo       *res, *ressave;
  int n;
  char portstr[32];
#endif

  if(port <= 0)       return PORT_ERR;
  if(port >= 256*256) return PORT_ERR;
  if(active == 0)
  { // accept calls
    s = -1;
    if(rl_ipversion == 4)
    {
      if(first == 1)
      {
        // create a socket
        os = socket(AF_INET,SOCK_STREAM,0);
        if(os == -1) return SOCKET_ERR;
        // set socket options 
#ifdef __VMS
        option = 1;
        if(setsockopt(os,SOL_SOCKET,SO_KEEPALIVE,&option,sizeof(option)) < 0)
        {
          return SETSOCKOPT_ERR;
        }
#endif
        option = 1;
#ifdef RLWIN32
        setsockopt(os,SOL_SOCKET,SO_REUSEADDR,(const char *) &option,sizeof(option));
#else
        setsockopt(os,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));
#endif
        // Bind our server to the agreed upon port number.
        memset(&localAddr,0,sizeof(localAddr));
        localAddr.sin_port = htons((short) port);
        localAddr.sin_family = AF_INET;
bind:
        ret = bind(os, (struct sockaddr *) &localAddr, sizeof(localAddr));
        if(ret == -1)
        {
          ::printf("warning: could not bind to port=%d\n", port);
          rlwthread_sleep(1000);
          goto bind;
        }
        // Prepare to accept client connections.  Allow up to 5 pending 
        // connections.                                            
        ret = listen(os, 5);
        if(ret == -1) return LISTEN_ERR;
      }
      first = 0;

      // accept connections
      if(os < 0) return ACCEPT_ERR;
      s = accept(os, (struct sockaddr *) &sockaddr[0], &socklen);
      if(s == -1) return ACCEPT_ERR;
    }
    else if(rl_ipversion == 6)
    {
#ifdef AF_INET6_IS_AVAILABLE
      if(first == 1)
      {
        memset(&hints0,0,sizeof(hints0));
        hints0.ai_flags = AI_PASSIVE;
        //hints0.ai_family = AF_UNSPEC;
        hints0.ai_family = AF_INET6;
        hints0.ai_socktype = SOCK_STREAM;
        sprintf(portstr,"%d",port);
        //::printf("server getaddrinfo(%s,%s)\n", adr, portstr);
        n = getaddrinfo(adr, portstr, &hints0, &res);
        if(n != 0)
        {
#ifndef RLWIN32
          ::printf("rlSocket:tcp_listen error for %s port=%s : %s\n", adr, portstr, gai_strerror(n));
#endif
          return -1;
        }
        //::printf("done\n");
        ressave = res;
bindv6:
        do
        {
          os = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
          if(os < 0)                                            continue; // error, try next one
#ifdef __VMS
          option = 1;
          if(setsockopt(os,SOL_SOCKET,SO_KEEPALIVE,&option,sizeof(option)) < 0)
          {
            return SETSOCKOPT_ERR;
          }
#endif
          option = 1;
#ifdef RLWIN32
          setsockopt(os,SOL_SOCKET,SO_REUSEADDR,(const char *) &option,sizeof(option));
#else
          setsockopt(os,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));
#endif
          if(bind(os, res->ai_addr, res->ai_addrlen) == 0)      break;    // success
          s = os;
          disconnect();                                                   // bind error, close and try next one
        }
        while((res = res->ai_next) != NULL);
        if(res == NULL)                                                   // errno from final socket() or bind()
        {
          ::printf("warning: could not bind to port=%d\n", port);
          rlwthread_sleep(1000);
          goto bindv6;
        }
        // Prepare to accept client connections.  Allow up to 5 pending 
        // connections
        ret = listen(os, 5);
        freeaddrinfo(ressave);
        if(ret == -1) return LISTEN_ERR;
      }
      first = 0;
      // accept connections
      if(os < 0) return ACCEPT_ERR;
      s = accept(os, (struct sockaddr *) &sockaddr[0], &socklen);
      if(s == -1) return ACCEPT_ERR;
#else
      ::printf("rlSocket:ERROR IPV6 not available on this platform\n");
#endif
    }
    else
    {
      ::printf("rlSocket:ERROR: rl_ipversion=%d not known\n", rl_ipversion);
    }
  } // end active == 0 
  else if(active == 1)
  {
    disconnect();
    //::printf("debug: adr=%s port=%d\n",adr,port);
    s = -1;
    if(rl_ipversion == 4)
    {
      os = socket(AF_INET,SOCK_STREAM,0);
      if(os == -1) return SOCKET_ERR;
      s = os;

      //::printf("debug: gethostbyname\n");
      // fill destblk structure 
      host = gethostbyname(adr);
      if(host == NULL)
      {
        // See if the host is specified in "dot address" form
        RemoteIpAddress.s_addr = inet_addr(adr);
        if(RemoteIpAddress.s_addr == INADDR_NONE)
        {
          // mur was here: without disconnect the sockets will be created over and over
          disconnect();
          s = -1;
          return INET_ADDR_ERR; // -1
        }
      }
      else
      {
        memcpy(&RemoteIpAddress,host->h_addr,host->h_length);
      }

      memset(&remoteAddr,0,sizeof(remoteAddr));
      remoteAddr.sin_family = AF_INET;
      remoteAddr.sin_port = htons((short) port);
      remoteAddr.sin_addr = RemoteIpAddress;

      //::printf("debug: connect\n");
      ret = ::connect(s, (struct sockaddr *) &remoteAddr, sizeof(remoteAddr));
      //::printf("debug: connect ret=%d\n",ret);
      if(ret == -1) 
      {
        disconnect(); // close s = os
        return CONNECT_ERR;
      }
    }
    else if(rl_ipversion == 6)
    {
#ifdef AF_INET6_IS_AVAILABLE
      sprintf(portstr,"%d",port);
      memset(&hints1, 0, sizeof(hints1));
      hints1.ai_family = AF_UNSPEC;
      hints1.ai_socktype = SOCK_STREAM;
      n = getaddrinfo(adr, portstr, &hints1, &res);
      if(n != 0)
      {
#ifndef RLWIN32
        ::printf("rlSocket:tcp_connect error for %s port=%s : %s\n", adr, portstr, gai_strerror(n));
#endif
        return -1;
      }
      ressave = res;
      do
      {
        s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(s < 0)                                            continue; // ignore this one
        if(::connect(s, res->ai_addr, res->ai_addrlen) == 0) break;    // success
        disconnect();                                                  // ignore this one
      }
      while((res = res->ai_next) != NULL);
      if(res == NULL) ::printf("rlSocket:tcp_connect error for %s port=%s\n", adr, portstr);
      freeaddrinfo(ressave);
#else
      ::printf("rlSocket:ERROR IPV6 not available on this platform\n");
#endif
    }
    else
    {
      ::printf("rlSocket:ERROR: rl_ipversion=%d not known\n", rl_ipversion);
    }
  }
  return s;
}

int rlSocket::disconnect()
{
  if(s != -1)
  {
#ifdef RLWIN32
    closesocket(s);
#else
    close(s);
#endif
  }
  s = -1;
  return 0;
}

int rlSocket::isConnected()
{
  if(s == -1) return 0;
  return 1;
}

int rlSocket::select(int timeout)
{
  struct timeval timout;
  fd_set wset,rset,eset;
  int    ret,maxfdp1;

  if(timeout == 0) return 1;
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
}

int rlSocket::printf(const char *format, ...)
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

int rlSocket::sendProcessViewBrowserButtonEvent(int id)
{
  return printf("QPushButton(%d)\n",id);
}

int rlSocket::setIPVersion(int version)
{
  if(version == 6) rl_ipversion = 6;
  else             rl_ipversion = 4;
  return rl_ipversion;
}

int rlSocket::getIPVersion()
{
  return rl_ipversion;
}

int rlSocket::rlGetsockopt(int sockfd, int level, int optname, void *optval, int *optlen)
{
#ifdef RLWIN32
  return getsockopt(sockfd, level, optname, (char *) optval, optlen);
#elif defined(__VMS)
  size_t len = *optlen;
  int ret = getsockopt(sockfd, level, optname, optval, &len);
  *optlen = len;
  return ret;
#else  
  socklen_t len = *optlen;
  int ret = getsockopt(sockfd, level, optname, optval, &len);
  *optlen = len;
  return ret;
#endif
}

int rlSocket::rlSetsockopt(int sockfd, int level, int optname, const void *optval, int optlen)
{
#ifdef RLWIN32
  return setsockopt(sockfd, level, optname, (const char *) optval, optlen);
#else
  return setsockopt(sockfd, level, optname, optval, optlen);
#endif
}

int rlSocket::rlGetsockopt(int level, int optname)
{
  int option = 1;
  int len = sizeof(option);
  return rlGetsockopt(s,level,optname,&option,&len);
}

int rlSocket::rlSetsockopt(int level, int optname)
{
  int option = 1;
  return rlSetsockopt(s,level,optname,&option,sizeof(option));
}

int rlSocket::readHttpContentLength(int timeout)
{
  char line[256];
  while(1)
  {
    if(readStr(line, (int)sizeof(line) - 1, timeout) < 1) return -1;
    if(strncmp(line,"Content-Length:",15) == 0) break;
  }
  int len = 0;
  sscanf(line,"Content-Length: %d", &len);
  if(readStr(line, (int)sizeof(line) - 1, timeout) < 1) return -1; // read CR LF
  return len;
}
