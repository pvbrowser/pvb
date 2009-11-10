/* <St> *******************************************************************

   sockopts.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : tcp-ip server for the Linux CIF Device Driver
  =========================================================================

  DISCRIPTION
    sockopts routine               .
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
sockopts(int sockfd, int doall)
{
  int 	option, optlen;

  /* "doall" is 0 for a server's listening socket (i.e., before
     accept() has returned.)  Some socket options such as SO_KEEPALIVE
     don't make sense at this point, while others like SO_DEBUG do. */

  if (debug) {
    option = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_DEBUG,
		   (char *) &option, sizeof(option)) < 0)
      err_sys("SO_DEBUG setsockopt error");
    
    option = 0;
    optlen = sizeof(option);
    if (getsockopt(sockfd, SOL_SOCKET, SO_DEBUG,
		   (char *) &option, &optlen) < 0)
      err_sys("SO_DEBUG getsockopt error");
    if (option == 0)
      err_quit("SO_DEBUG not set (%d)", option);
    DBG_PRN("SO_DEBUG set\n");
  }
  
  if (broadcast && doall) {
    option = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
		   (char *) &option, sizeof(option)) < 0)
      err_sys("SO_BROADCAST setsockopt error");
    
    option = 0;
    optlen = sizeof(option);
    if (getsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
		   (char *) &option, &optlen) < 0)
      err_sys("SO_BROADCAST getsockopt error");
    if (option == 0)
      err_quit("SO_BROADCAST not set (%d)", option);
    
    DBG_PRN("SO_BROADCAST set\n");
  }
}
