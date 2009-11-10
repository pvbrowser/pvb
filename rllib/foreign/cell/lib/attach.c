
/* Revised 5/6/02 William Hays - CTI 

   5/7/2002 WRH Added setsockopt to disable Nagle algorithm, added ms_timeout init
 
*/

#include "libcell.h"



void cell_attach(_comm_header * comm1, int debug)
{
  struct hostent *hostinfo;
  struct sockaddr_in address;
  int result, len;
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup (0x0101,&wsaData);
#endif

  dprint(DEBUG_TRACE, "Entering attach.c\n");

  hostinfo = gethostbyname(comm1->hostname);
  if (!hostinfo)
  {
    comm1->error = NOHOST;
    return;
  }

  comm1->file_handle = socket(AF_INET, SOCK_STREAM, 0);
  if (comm1->file_handle == -1)
  {
    comm1->error = NOCONNECT;
    return;
  }

  comm1->ms_timeout=CELL_DFLT_TIMEOUT;

  address.sin_family = AF_INET;
  address.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
  if (address.sin_addr.s_addr == 0)
  {
    comm1->error = BADADDR;
    return;
  }
  address.sin_port = htons(0xaf12);
  result = 0;
  len = sizeof(address);
  result = connect(comm1->file_handle, (struct sockaddr *) &address, len);
  if (result < 0)
  {
    comm1->error = NOCONNECT;
    return;
  }


#ifndef WIN32
  /* Disable the Nagle algorithm for send colescing */
  len = 1;
  if (setsockopt(comm1->file_handle, IPPROTO_TCP,
     TCP_NODELAY,&len,sizeof(len)) == -1)
  {
    /* Nothing really to do if this fails.. just continue */
  }
#endif
  comm1->error = OK;
  dprint(DEBUG_TRACE, "Exiting attach.c\n");

  return;
}
