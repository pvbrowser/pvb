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
