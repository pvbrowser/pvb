#include "libcell.h"


/* Revised 5/7/02 William Hays - CTI */

int senddata(const _data_buffer * buff, _comm_header * comm, int debug)
{
  int x;
#ifndef WIN32
  fd_set s_writefds;
  int nfds;
  struct timeval s_timeout;
#endif

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_TRACE, "--- Senddata.c entered.\n");
    dprint(DEBUG_DATA, "Sending the following data out:\n");
    for (x = 0; x < buff->overall_len; x++)
      dprint(DEBUG_DATA, "%02X ", buff->data[x]);
    dprint(DEBUG_DATA, "\n");
    dprint(DEBUG_DATA, "Overall Length = %d\n", buff->overall_len);
  }


#ifndef WIN32
  FD_ZERO(&s_writefds);

  FD_SET(comm->file_handle,&s_writefds);

  nfds = comm->file_handle+1;

  s_timeout.tv_sec = 0;
  s_timeout.tv_usec = comm->ms_timeout * 1000;

  if (!select (nfds, NULL, &s_writefds, NULL, &s_timeout))
  {
    /* Error during communications */
    CELLERROR(2,"Communcations Error");
    return -1;
  }

  if (comm->file_handle != 0)
    write(comm->file_handle, buff->data, buff->overall_len);
#endif

#ifdef WIN32
    send (comm->file_handle, buff->data, buff->overall_len,0);
#endif
  dprint(DEBUG_TRACE, "senddata.c exiting.\n");

  return 0;
}
