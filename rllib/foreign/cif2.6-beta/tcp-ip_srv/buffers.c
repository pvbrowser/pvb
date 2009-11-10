/* <St> *******************************************************************

   buffers.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : tcp-ip server for the Linux CIF Device Driver
  =========================================================================

  DISCRIPTION
    buffers routine               .
  =========================================================================

  CHANGES
  version name      date        Discription
                 
-------------------------------------------------------------------------
  V1.001

  ======================== Copyright =====================================

  Copyright (C) 2004  Hilscher GmbH

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
   
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

  ========================================================================
******************************************************************** <En> */
#include  "SOCKET.h"

void
buffers(int sockfd)
{
  int	n, optlen;

  /* Allocate the read and write buffers. */
  if (rbuf == NULL) {
    if ( (rbuf = (unsigned char *)malloc(readlen)) == NULL)
      err_sys("malloc error for read buffer");
  }
  if (wbuf == NULL) {
    if ( (wbuf = malloc(writelen)) == NULL)
      err_sys("malloc error for write buffer");
  }
  /* Set the socket send and receive buffer sizes (if specified).
     The receive buffer size is tied to TCP's advertised window. */
  if (rcvbuflen) {
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &rcvbuflen,
		   sizeof(rcvbuflen)) < 0)
      err_sys("SO_RCVBUF setsockopt error");
    optlen = sizeof(n);
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &n,
		   &optlen) < 0)
      err_sys("SO_RCVBUF getsockopt error");
    if (n != rcvbuflen)
      err_quit("rcvbuflen = %d, SO_RCVBUF = %d", rcvbuflen, n);
    DBG_PRN("SO_RCVBUF = %d\n", n);
  }
  if (sndbuflen) {
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &sndbuflen,
		   sizeof(sndbuflen)) < 0)
      err_sys("SO_SNDBUF setsockopt error");
    
    optlen = sizeof(n);
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &n,
		   &optlen) < 0)
      err_sys("SO_SNDBUF getsockopt error");
    if (n != sndbuflen)
      err_quit("sndbuflen = %d, SO_SNDBUF = %d", sndbuflen, n);
    DBG_PRN("SO_SNDBUF = %d\n", n);
  }
}
