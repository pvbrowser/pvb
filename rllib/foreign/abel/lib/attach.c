
// This is an example program for communicating with an Allen Bradley PLC-5 over
// Ethernet.  This is only to demonstrate the methodology needed.  This program is not
// necessarily useful for any other purpose.

//  Copyright (1999) by Ron Gage 

#include "libabplc5.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

int x,l,junk;

char dataaddr[32];

/* This routine will initiate a communications link to a PLC5.
It returns a pointer to a _comm structure.*/

struct _comm abel_attach (char *host, int debug)

{
struct hostent *hostinfo;
struct _comm comm1;
struct sockaddr_in address;
struct ethernet_header header, rcvd;
struct custom_connect custom;
int result,len,place,response_len;
byte junk[32];
hostinfo = gethostbyname(host);
if (!hostinfo)
  {
  comm1.error=NOHOST;
  return comm1;
  }
  
comm1.handle = socket (AF_INET, SOCK_STREAM, 0);
if (comm1.handle == -1)
  { 
  perror ("attach_socket");
  comm1.error=NOCONNECT;
  return comm1;
  }
address.sin_family = AF_INET;
address.sin_addr = *(struct in_addr *)*hostinfo ->h_addr_list;
if (address.sin_addr.s_addr == 0)
  {
  comm1.error=BADADDR;
  return comm1;
  }
address.sin_port = htons(2222);
result = 0;
len = sizeof(address);
result = connect(comm1.handle, (struct sockaddr *)&address, len);
if (result < 0)
  {
  perror ("attach_connect");
  comm1.error=NOCONNECT;
  return comm1;
  }
comm1.error=OK;
place=0;
bzero (&header,sizeof(header));
bzero (&rcvd, sizeof(rcvd));
bzero (&custom, sizeof(custom));
header.mode = 1;
header.submode = CONNECT_CMD;
header.conn = 0;
header.pccc_length = 0;
custom.version = htons(PCCC_VERSION);
custom.backlog = htons(PCCC_BACKLOG);
memcpy (&header.custom, &custom, _CUSTOM_LEN);

if (debug == TRUE)
  {
  memcpy (&junk, &header, _ENET_HEADER_LEN);
  for (x=0;x<_ENET_HEADER_LEN;x++)
    printf ("%02X  ",junk[x]);
  printf ("\n");
  }
    
write (comm1.handle, &header, _ENET_HEADER_LEN);
response_len = read (comm1.handle, &rcvd, 255);

if (response_len != 28)
  {
  comm1.error=NOCONNECT;
  return comm1;
  }
if (debug==TRUE)
  {
  memcpy (&junk, &rcvd, 28);
  for (x=0;x<28;x++)
    printf ("%02X  ",junk[x]);
  printf ("\n");
  }
  
comm1.commid = ntohl(rcvd.conn);
comm1.tns = (unsigned short) time ((time_t *)0);
comm1.mode = ETHERNET;
if (debug==TRUE)
  printf ("tns = %d   connid = %ld\n",comm1.tns,comm1.commid);
return comm1;
}

