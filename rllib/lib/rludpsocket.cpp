/***************************************************************************
                        rludpsocket.cpp  -  description
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
#include "rludpsocket.h"
#include "rlcutil.h"
#include <stdio.h>
#include <string.h>

rlIpAdr::rlIpAdr()
{
  rlwsa(); // init sockets on windows
  memset(&address,0,sizeof(address));
}

rlIpAdr::~rlIpAdr()
{
}

int rlIpAdr::setAdr(const char *adr, int port)
{
  if(port < 0 || port >= 256*256) return -1;
  struct in_addr IpAddress;
  struct hostent *host;

  memset(&IpAddress,0,sizeof(IpAddress));
  memset(&address,0,sizeof(address));

  host = gethostbyname(adr);
  if(host == NULL)
  {
    // See if the host is specified in "dot address" form
    IpAddress.s_addr = inet_addr(adr);
    if(IpAddress.s_addr == INADDR_NONE)
    {
      ::printf("rlIpAdr::setAdr() could not gethostbyname(%s)\n",adr);
      return -1;
    }
  }
  else
  {
    memcpy(&IpAddress,host->h_addr,host->h_length);
  }

  address.sin_family = AF_INET;
  address.sin_port = htons((short) port);
  address.sin_addr = IpAddress;

  return 0;
}

int rlIpAdr::operator==(rlIpAdr &address1)
{
  if(address.sin_family != address1.address.sin_family) return 0;
  if(memcmp(&address.sin_addr,&address1.address.sin_addr,sizeof(address.sin_addr)) == 0) return 1;
  return 0;
}

//###################################################################

rlUdpSocket::rlUdpSocket(int _debug)
{
  debug = _debug;
  if(debug) ::printf("rlUdpSocket() constructor\n");
  rlwsa(); // init sockets on windows
  readflag = writeflag = 0;
  s = socket(AF_INET,SOCK_DGRAM,0);
  if(s < 0)
  {
    s = -1;
    ::printf("rlUdpSocket::rlUdpSocket could not get socket\n");
  }
}

rlUdpSocket::~rlUdpSocket()
{
  if(s > 0)
  {
#ifdef RLWIN32
    closesocket(s);
#else
    close(s);
#endif
  }
}

int rlUdpSocket::setSockopt(int opt)
{
  const int on = 1;
  if(s == -1) return -1;
  // set socket options
#ifdef RLWIN32
  return setsockopt(s,SOL_SOCKET,opt,(const char *) &on,sizeof(on));
#else
  return setsockopt(s,SOL_SOCKET,opt,&on,sizeof(on));
#endif
}

int rlUdpSocket::setSockopt(int level, int optname, void *optval, int optlen)
{
  if(s == -1) return -1;
  if(optlen <= 0) return -1;
  // set socket options
#ifdef RLWIN32
  return setsockopt(s,level,optname,(const char *) &optval,optlen);
#else
  return setsockopt(s,level,optname,optval,optlen);
#endif
}

int rlUdpSocket::bind(int port)
{
  if(port < 0 || port >= 256*256) return -1;

  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons((short) port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  // bind socket
  if(::bind(s, (sockaddr *) &address, sizeof(address)) < 0)
  {
    ::printf("rlUdpSocket::setAdr() bind() failed port=%d\n", port);
    return -1;
  }
  return 0;
}

int rlUdpSocket::select(int timeout)
{
  if(timeout < 0) return -1;
  struct timeval timout;
  fd_set wset,rset,eset;
  int    ret,maxfdp1;

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

int rlUdpSocket::recvfrom(void *buf, int maxlen, rlIpAdr *source, int timeout)
{
  int  ret, len;

  if(timeout >= 0)
  {
    ret = select(timeout);
    if(ret != 1) return -1; // timeout
  }
  if(debug) ::printf("rlUdpSocket()::recvfrom() ...\n");
  len = sizeof(source->address);
#ifdef RLWIN32
  ret = ::recvfrom(s, (char *) buf, maxlen, readflag,
                  (struct sockaddr *) &source->address, (int FAR *) &len);
#endif
#ifdef RLUNIX
  ret = ::recvfrom(s, buf, maxlen, readflag,
                  (struct sockaddr *) &source->address, (socklen_t *) &len);
#endif
#ifdef __VMS
  ret = ::recvfrom(s, buf, maxlen, readflag,
                  (struct sockaddr *) &source->address, (size_t *)  &len);
#endif
  if(ret < 0)
  {
    ::printf("ERROR: rlUdpSocket::read()\n");
    return -2;
  }
  if(debug)
  {
    unsigned char *cbuf = (unsigned char *) buf;
    ::printf("rlUdpSocket()::recvfrom() ret=%d data=[0x%x",ret,cbuf[0]);
    for(int i=1; i<ret; i++) ::printf(",0x%x",cbuf[i]);
    ::printf("]\n");
  }
  return ret;
}

int rlUdpSocket::sendto(const void *buf, int len, rlIpAdr *dest)
{
#ifdef RLWIN32
  int ret = ::sendto(s, (const char *) buf, len, writeflag,
                    (struct sockaddr *) &dest->address, sizeof(struct sockaddr_in));
#else
  int ret = ::sendto(s, buf, len, writeflag,
                    (struct sockaddr *) &dest->address, sizeof(struct sockaddr_in));
#endif
  if(ret < 0) ::printf("ERROR: rlUdpSocket::sendto()\n");
  if(debug)
  {
    unsigned char *cbuf = (unsigned char *) buf;
    ::printf("rlUdpSocket()::sendto() ret=%d data=[0x%x",ret,cbuf[0]);
    for(int i=1; i<ret; i++) ::printf(",0x%x",cbuf[i]);
    ::printf("]\n");
  }
  return ret;
}

int rlUdpSocket::printf(rlIpAdr *dest, const char *format, ...)
{
  int ret;
  char message[rl_PRINTF_LENGTH]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  return sendto(message,strlen(message)+1,dest);
}

//#define TESTING
#ifdef TESTING

int main(int ac, char **av)
{
  int ret;
  char buf[1024];
  rlUdpSocket udp;
  rlIpAdr dest;
  dest.setAdr("localhost",5050);

  if(ac == 2) // client
  {
    while(1)
    {
      sleep(1);
      ret = udp.sendto(av[1], strlen(av[1])+1, &dest);
      if(ret > 0)
      {
        printf("udp.sendto(%s) ret=%d\n",av[1],ret);
      }
      else printf("udp.sendto() failed\n");
    }
  }
  else // server
  {
    rlIpAdr source;
    ret = udp.bind(5050);
    while(1)
    {
      if((ret = udp.recvfrom(buf,sizeof(buf), &source, 1000)) > 0)
      {
        printf("udp.recvfrom(%s) ret=%d\n",buf,ret);
        if(dest == source) printf("dest==source\n");
        else               printf("dest!=source\n");
      }
      else printf("udp.recvfrom() failed or timeout\n");
    }
  }
  return 0;
}

#endif
