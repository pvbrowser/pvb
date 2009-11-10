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
