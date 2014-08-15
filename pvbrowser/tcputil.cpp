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
#ifdef _WIN32
#include "winsock2.h"
#endif

#include "pvdefine.h"
#include "opt.h"

#ifndef IS_OLD_MSVCPP
#ifndef USE_SYMBIAN
#define AF_INET6_IS_AVAILABLE
#endif
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

// functions from plugin
// Von Asgard aus schlugen sie eine Brücke
// auf dass ihnen Midgard nie entrücke,
// das Reich der ersten Menschen.

typedef int (*pvb_com_con_func)(const char *adr, int port, int *use);
pvb_com_con_func pvb_com_con = NULL;

typedef int (*pvb_com_rec_func)(int ind, int s, char *msg, int len, int *use);
pvb_com_rec_func pvb_com_rec = NULL;

typedef int (*pvb_com_rec_binary_func)(int ind, int s, char *msg, int len, int *use);
pvb_com_rec_binary_func pvb_com_rec_binary = NULL;

typedef int (*pvb_com_send_func)(int ind, int s, const char *msg, int len, int *use);
pvb_com_send_func pvb_com_send = NULL;

typedef int (*pvb_com_close_func)(int ind, int s, int *use);
pvb_com_close_func pvb_com_close = NULL;

typedef int (*pvb_com_plugin_on_func)(int ind, const char *cmd);
pvb_com_plugin_on_func pvb_com_plugin_on = NULL;

#ifndef PVWIN32
#include <dlfcn.h>
#define RL_RTLD_LAZY RTLD_LAZY
void *bifrost;
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#define closesocket close
#endif

#ifdef PVWIN32
#define RL_RTLD_LAZY 0
HMODULE bifrost;
#ifdef IS_OLD_MSVCPP
#include "winsock.h"
#include <windows.h>
#else
#if (_WIN32_WINNT < 0x0501)
#warning mingw does not have ipv6 helpers modify mingw header in ws2tcpip.h
#endif
#include "winsock2.h"
#include <windows.h>
#include <ws2tcpip.h>
#if QT_VERSION < 0x050000
void WSAAPI freeaddrinfo(struct addrinfo*);
int  WSAAPI getaddrinfo(const char*,const char*,const struct addrinfo*, struct addrinfo**);
int  WSAAPI getnameinfo(const struct sockaddr*,socklen_t,char*,DWORD, char*,DWORD,int);
#endif
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
#include "mainwindow.h"
int socket_array[MAX_TABS+2];
int use_pvb_com_plugin[MAX_TABS+2];

static int connect_timed(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int is_localhost)
{
  int ret;
  struct timeval timeout;
  int debug = opt.arg_debug;
  int connect_timeout = opt.connect_timeout;

  if(connect_timeout <= 0 || is_localhost) return ::connect(sockfd, addr, addrlen); // use standard timeout

  if(debug) printf("start connect_timed\n");

#ifdef PVWIN32
  u_long mode = 1;
  if(ioctlsocket(sockfd, FIONBIO, &mode) != NO_ERROR) return -1;    // set socket non blocking
#else
  int opt;
  if((opt = fcntl(sockfd, F_GETFL, NULL)) < 0) return -1;           // get current options
  if(fcntl(sockfd, F_SETFL, opt | O_NONBLOCK) < 0) return -1;       // set socket non blocking
#endif  
  if(debug) printf("try connect\n");
  if((ret = ::connect(sockfd, addr, addrlen)) < 0)                  // try to connect
  {
    if(debug) printf("test in progress\n");
#ifdef PVWIN32
    if(1)
#else
    if(errno == EINPROGRESS)                                        // if connect is in progress 
#endif  
    {                                                               // then
      if(debug) printf("wait %d seconds\n", connect_timeout);
      timeout.tv_sec  = connect_timeout;                            // wait N seconds
      timeout.tv_usec = 0;                                          // for completion
      fd_set wait_set;
      FD_ZERO(&wait_set);
      FD_SET(sockfd, &wait_set);
      ret = ::select(sockfd + 1, NULL, &wait_set, NULL, &timeout);
    }
    if(debug) printf("after wait\n");
  } 
  else                                                              // if connect
  {                                                                 //
    if(debug) printf("directly connected\n");
    ret = 1;                                                        // directly connected
  }
#ifdef PVWIN32
  mode = 0;
  if(ioctlsocket(sockfd, FIONBIO, &mode) != NO_ERROR) return -1;    // set socket blocking
#else
  if(fcntl(sockfd, F_SETFL, opt) < 0) return -1;                    // restore socket options
#endif
  if(debug) printf("end ret=%d\n", ret);
  if(ret == 1) return 0;                                            // return success
  printf("connect_timed::ERROR could not connect within %d seconds (tcputil.cpp)\n", connect_timeout);
  return -1;
}

int tcp_init()
{
  if(opt.arg_debug) printf("tcp_init(%s)\n", opt.pvb_com_plugin);
  // int socket_array and use of pvb_com_plugin
  for(int i=0; i<MAX_TABS+2; i++)
  {
    socket_array[i] = -1;
    use_pvb_com_plugin[i] = 0;
  }  

  char buf[MAXOPT] = "";
  if(strchr(opt.pvb_com_plugin,'/') != NULL || strchr(opt.pvb_com_plugin,'\\') != NULL)
  { // absolute path given
    if(strlen(opt.pvb_com_plugin) < MAXOPT) strcpy(buf, opt.pvb_com_plugin);
  }
  else
  { // relative path to PWD given
    if(strlen(opt.pvb_com_plugin) + strlen(opt.initial_dir) + 4 < MAXOPT)
    {
      strcpy(buf, opt.initial_dir);
#ifdef PVWIN32
      strcat(buf,"\\");
#else
      strcat(buf,"/");
#endif
      strcat(buf, opt.pvb_com_plugin);
    }
  }

  // load pvb_com plugin
  pvb_com_con = NULL;
  pvb_com_rec = NULL;
  pvb_com_rec_binary = NULL;
  pvb_com_send = NULL;
  pvb_com_close = NULL;
  pvb_com_plugin_on = NULL;
  if(strlen(buf) > 0)
  {
    if(opt.arg_debug) printf("Load communication plugin: tcp_init(%s)\n", buf);
#ifdef PVWIN32
    ::SetLastError(0);
    bifrost = ::LoadLibraryA(buf);
    ::SetLastError(0);
    if(bifrost != NULL)
    {
      pvb_com_con        = (pvb_com_con_func)        ::GetProcAddress(bifrost, "pvb_com_con");
      pvb_com_rec        = (pvb_com_rec_func)        ::GetProcAddress(bifrost, "pvb_com_rec");
      pvb_com_rec_binary = (pvb_com_rec_binary_func) ::GetProcAddress(bifrost, "pvb_com_rec_binary");
      pvb_com_send       = (pvb_com_send_func)       ::GetProcAddress(bifrost, "pvb_com_send");
      pvb_com_close      = (pvb_com_close_func)      ::GetProcAddress(bifrost, "pvb_com_close");
      pvb_com_plugin_on  = (pvb_com_plugin_on_func)  ::GetProcAddress(bifrost, "pvb_com_plugin_on");
    }  
#else
    bifrost = ::dlopen(buf, RL_RTLD_LAZY);
    if(bifrost != NULL)
    {
      pvb_com_con        = (pvb_com_con_func)        ::dlsym(bifrost, "pvb_com_con");
      pvb_com_rec        = (pvb_com_rec_func)        ::dlsym(bifrost, "pvb_com_rec");
      pvb_com_rec_binary = (pvb_com_rec_binary_func) ::dlsym(bifrost, "pvb_com_rec_binary");
      pvb_com_send       = (pvb_com_send_func)       ::dlsym(bifrost, "pvb_com_send");
      pvb_com_close      = (pvb_com_close_func)      ::dlsym(bifrost, "pvb_com_close");
      pvb_com_plugin_on  = (pvb_com_plugin_on_func)  ::dlsym(bifrost, "pvb_com_plugin_on");
    }  
#endif
    if(opt.arg_debug)
    {
      printf("bifrost = %ld\n", (long) bifrost);
      printf("pvb_com_con        = %ld\n", (long) pvb_com_con);
      printf("pvb_com_rec        = %ld\n", (long) pvb_com_rec);
      printf("pvb_com_rec_binary = %ld\n", (long) pvb_com_rec_binary);
      printf("pvb_com_send       = %ld\n", (long) pvb_com_send);
      printf("pvb_com_close      = %ld\n", (long) pvb_com_close);
      printf("pvb_com_plugin_on  = %ld\n", (long) pvb_com_plugin_on);
    }  
  }
  else
  {
    printf("could not load pvb_com_plugin=%s\n", buf);
  }
  return 0;
}

int tcp_free()
{
  if(bifrost != NULL)
  {
#ifdef PVWIN32
    ::FreeLibrary(bifrost);
#else
    ::dlclose(bifrost);
#endif
  }
  return 0;
}

int tcp_con(const char *adr, int port)
{
  static int                s, ind;
  static struct sockaddr_in remoteAddr;
  static struct hostent     *host;
  static struct in_addr     RemoteIpAddress;
  int                       ret;
#ifdef AF_INET6_IS_AVAILABLE
  static struct addrinfo    hints, *res, *ressave;
  int                       n;
  char                      portstr[32];
#endif
  int is_localhost = 0;

  if(opt.arg_log) printf("conn=%s port=%d\n", adr, port);
  if(strcmp(adr,"localhost") == 0) is_localhost = 1;
  if(strcmp(adr,"127.0.0.1") == 0) is_localhost = 1;
  if(strcmp(adr,"::1")       == 0) is_localhost = 1;
  if(pvb_com_con != NULL)
  {
    int use_plugin = 0;
    s = (pvb_com_con)(adr,port,&use_plugin);
    if(opt.arg_debug) printf("plugin returned use_plugin=%d\n", use_plugin);
    if(s > 0)
    {
      for(ind=1; ind<=MAX_TABS; ind++)
      {
        if(socket_array[ind] == -1)
        {
          socket_array[ind] = s;
          use_pvb_com_plugin[ind] = use_plugin;
          if(opt.arg_debug) printf("use_pvb_con_plugin[%d]=%d\n", ind, s);
          break;
        }
      }
      return ind;
    }  
  }
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

    ret = connect_timed(s, (struct sockaddr *) &remoteAddr, sizeof(remoteAddr), is_localhost);
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
      if(s < 0)                                                                continue; // ignore this one
      if(::connect_timed(s, res->ai_addr, res->ai_addrlen, is_localhost) == 0) break;    // success
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
  for(ind=1; ind<=MAX_TABS; ind++)
  {
    if(socket_array[ind] == -1)
    {
      socket_array[ind] = s;
      use_pvb_com_plugin[ind] = 0;
      break;
    }
  }
  return ind;
}

int tcp_rec(int *s_ind, char *msg, int len)
{
  int ret,i;

  if(s_ind == NULL) { msg[0] = '\n'; msg[1] = '\0'; return -1; }
  if(*s_ind == -1)  { msg[0] = '\n'; msg[1] = '\0'; return -1; }
  int s = socket_array[*s_ind];
  if(use_pvb_com_plugin[*s_ind])
  {
    ret = (pvb_com_rec)(*s_ind,s,msg,len,use_pvb_com_plugin);
    if(ret < 0)
    {
      socket_array[*s_ind] = -1;
      *s_ind = -1;
    }
    return ret;
  }  
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
    ret = recv(s,&msg[i],1,0);
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
        if(s != -1)
        {
          goto tryagain;
        }  
      }
#endif
      if(s != -1) closesocket(s);
      socket_array[*s_ind] = -1;
      *s_ind = -1;
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
      if(msg[0] == '@')
      {
        if(strncmp(msg,"@plugin(", 8) == 0 && pvb_com_plugin_on != NULL)
        {
          use_pvb_com_plugin[*s_ind] = 1;
          (pvb_com_plugin_on)(*s_ind,msg);
        }
      }
      return i;
    }
    i++;
  }
  msg[i] = '\0';
  if(opt.arg_log) printf("recv=%s", msg);
  return i;
}

int tcp_rec_binary(int *s_ind, char *msg, int len)
{
  int ret,i;

  if(s_ind == NULL) return -1;
  if(*s_ind == -1) return -1;
  int s = socket_array[*s_ind];
  if(use_pvb_com_plugin[*s_ind])
  {
    ret = (pvb_com_rec_binary)(*s_ind,s,msg,len,use_pvb_com_plugin);
    if(ret == -1)
    {
      socket_array[*s_ind] = -1;
      *s_ind = -1;
    }
    return ret;
  }  
  i = 0;
  while(i < len)
  {
#ifdef PVWIN32
tryagain:
#endif
    ret = recv(s,&msg[i],len-i,0);
    if(ret <= 0)
    {
#ifdef PVWIN32
      if(WSAEWOULDBLOCK == WSAGetLastError())
      {
        if(s != -1)
        {
          goto tryagain;
        }  
      }
#endif
      if(s != -1) closesocket(s);
      socket_array[*s_ind] = -1;
      *s_ind = -1;
      return -1;
    }
    i += ret;
  }
  if(opt.arg_log) printf("recv_binary len=%d bytes\n", i);
  return i;
}

int tcp_send(int *s_ind, const char *msg, int len)
{
  int ret,bytes_left,first_byte;

  ret = 0;
  if(s_ind == NULL) return -1;
  if(*s_ind == -1) return -1;
  int s = socket_array[*s_ind];
  if(use_pvb_com_plugin[*s_ind])
  {
    ret = (pvb_com_send)(*s_ind,s,msg,len,use_pvb_com_plugin);
    if(ret == -1)
    {
      socket_array[*s_ind] = -1;
      *s_ind = -1;
    }
    return ret;
  }  
  bytes_left = len;
  first_byte = 0;

  while(bytes_left > 0)
  {
    ret = send(s,&msg[first_byte],bytes_left,MSG_NOSIGNAL);
    if(ret <= 0) 
    { 
      closesocket(s); 
      socket_array[*s_ind] = -1;
      *s_ind = -1; 
      return -1; 
    }
    bytes_left -= ret;
    first_byte += ret;
  }
  if(opt.arg_log) printf("send=%s", msg);
  return ret;
}

int tcp_close(int *s)
{
  if(s == NULL) return -1;
  int stemp = socket_array[ *s ];
  if(stemp == -1) return -1;
  tcp_send(s,"shutup\n",7);
  socket_array[ *s ] = -1;
  if(pvb_com_close != NULL)
  {
    int ret = (pvb_com_close)(*s, stemp,use_pvb_com_plugin);
    use_pvb_com_plugin[ *s ] = 0;
    if(ret == 1) return 0;
  }
  use_pvb_com_plugin[ *s ] = 0;
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
