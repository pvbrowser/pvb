/***************************************************************************
                          rlsocket.h  -  description
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
#ifndef _RL_SOCKET_H_
#define _RL_SOCKET_H_

#include "rldefine.h"
#include "rlstring.h"
#ifdef RLWIN32
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "unistd.h"
#endif

/*! <pre>
you have to call this function before you use any sockets
(at least under windows)
</pre> */
#define wsa rlwsa
int rlwsa();
int rlScoketWrite(int *socket, const void *buf, int len);

/*! <pre>
class for encapsulating TCP/IP socket calls
</pre> */
class rlSocket
{
public:

  enum SocketEnum
  {
    SOCKET_ERR     = -1,
    SETSOCKOPT_ERR = -2,
    LISTEN_ERR     = -3,
    ACCEPT_ERR     = -4,
    INET_ADDR_ERR  = -5,
    CONNECT_ERR    = -6,
    PORT_ERR       = -7
  };

  /*! <pre>
      construct a new rlSocket but do not connect
      adr  = hostname | dotted address
      port = port number of socket
      active = 0 wait for connections with accept()
      active = 1 open the connection with connect()
      active = 2 neither accept() nor connect()
  </pre> */
  rlSocket(const char *adr, int port, int active);

  /*! <pre>
      construct a new rlSocket
      use connection on open socket
  </pre> */
  rlSocket(int socket);


  /*! <pre>
      destruct the socket
      attention if active = 0 the socket will still be bound to port
  </pre> */
  virtual ~rlSocket();

  /*! <pre>
      set adr to a different adr than in the constructor
  </pre> */
  void setAdr(const char *adr);

  /*! <pre>
      set port to a different port than in the constructor
  </pre> */
  void setPort(int port);
  
  /*! <pre>
      get port
  </pre> */
  int getPort();

  /*! <pre>
      set port active = 0|1
  </pre> */
  void setActive(int active);

  /*! <pre>
      read a block of data
      len = length of data to be read
      timeout = 0 wait indefinite
      timeout > 0 wait at maximum for timeout milliseconds
      return > 0 length of message read
      return == 0 timeout
      return < 0 error
  </pre> */
  int read(void *buf, int len, int timeout=0);

  /*! <pre>
      read a '\n' terminated string
      len = max length of data to be read
      timeout = 0 wait indefinite
      timeout > 0 wait at maximum for timeout milliseconds
      return > 0 length of message read
      return == 0 timeout
      return < 0 error
  </pre> */
  int readStr(char *buf, int len, int timeout=0);

  /*! <pre>
      read a http header and return Content-Length
      return < 0 error
  </pre> */
  int readHttpHeader(rlString *header, int timeout=0);

  /*! <pre>
      write a block of data
      return > 0 length of data written
      return < 0 error
  </pre> */
  int write(const void *buf, int len);

  /*! <pre>
      similar to printf
      return > 0 length of data written
      return < 0 error
  </pre> */
  int printf(const char *format, ...);

  /*! <pre>
      connect
      return >= 0 socket used
      return < 0  error (see: enum SocketEnum)
  </pre> */
  int connect();

  /*! <pre>
      disconnect
      return = 0
  </pre> */
  int disconnect();

  /*! <pre>
      wait for data arriving on socket
      timeout > 0 timeout in milliseconds
      timeout == 0 indefinite timeout
      return = 1 DATA_AVAILABLE
      return = 0 TIMEOUT
  </pre> */
  int select(int timeout=0);

  /*! <pre>
      return == 1 socket is connected
      return == 0 socket is not connected
  </pre> */
  int isConnected();

  /*! <pre>
      default: prefer IPV4
      if(ip==6) prefer IPV6
      else      prefer IPV4
  </pre> */
  int setIPVersion(int version);

  /*! <pre>
      return == 4 IPV4
      return == 6 IPV6
  </pre> */
  int getIPVersion();

  /*! <pre>
      This method is intendet for data providers implemented as ProcessViewServer
  </pre> */
  int sendProcessViewBrowserButtonEvent(int id);

  /*! <pre>
      this is the real socket used for communication
      s >= 0  connected
      s == -1 disconnected
  </pre> */
  int  s;

  /*! <pre>
      portable version of getsockopt
  </pre> */
  static int rlGetsockopt(int sockfd, int level, int optname, void *optval, int *optlen);
  
  /*! <pre>
      portable version of setsockopt
  </pre> */
  static int rlSetsockopt(int sockfd, int level, int optname, const void *optval, int optlen);

  /*! <pre>
      get an option from this->s
  </pre> */
  int rlGetsockopt(int level, int optname);

  /*! <pre>
      set an option on this->s
  </pre> */
  int rlSetsockopt(int level, int optname);

  /*! <pre>
      read the response to a http get request until Content-Length is received
      Tip: char line[256];
  </pre> */
  int readHttpContentLength(int timeout);
  
  /*! <pre>
      this array can be casted to (struct sockaddr *) &sockaddr[0];
      in case of active==0 it will store sockaddr of the last client
      (48 bytes spare)

      struct sockaddr {
        unsigned short    sa_family;    // address family, AF_xxx
        char              sa_data[14];  // 14 bytes of protocol address
      };

      // IPv4 AF_INET sockets:
      struct sockaddr_in {
        short            sin_family;   // e.g. AF_INET, AF_INET6
        unsigned short   sin_port;     // e.g. htons(3490)
        struct in_addr   sin_addr;     // see struct in_addr, below
        char             sin_zero[8];  // zero this if you want to
      };
      struct in_addr {
        unsigned long s_addr;          // load with inet_pton()
      };

      // IPv6 AF_INET6 sockets:
      struct sockaddr_in6 {
        u_int16_t       sin6_family;   // address family, AF_INET6
        u_int16_t       sin6_port;     // port number, Network Byte Order
        u_int32_t       sin6_flowinfo; // IPv6 flow information
        struct in6_addr sin6_addr;     // IPv6 address
        u_int32_t       sin6_scope_id; // Scope ID
      };
      struct in6_addr {
          unsigned char   s6_addr[16];   // load with inet_pton()
      };
  </pre> */
  unsigned char sockaddr[16+48];

private:
  char adr[132];
  int  port;
  int  active;
  int  os;
  int  first;
  int  prefer_ipv6;
  int  rl_ipversion;
};

#endif
