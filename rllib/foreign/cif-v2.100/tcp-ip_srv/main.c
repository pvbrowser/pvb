#include   "SOCKET.h"

			/* define global variables */
char	*host;
char	*port;

int	bindport;		/* 0 or TCP port number to bind */
int	broadcast;		/* SO_BROADCAST */
int	cbreak;			/* set terminal to cbreak mode */
int	client = 1;		/* acting as client is the default */
int	crlf;			/* convert newline to CR/LF & vice versa */
int	debug;			/* SO_DEBUG */
int	dofork;			/* concurrent server, do a fork() */
char	foreignip[32];		/* foreign IP address, dotted-decimal string */
int	foreignport;		/* foreign port number */
int	halfclose;		/* TCP half close option */
int	keepalive;		/* SO_KEEPALIVE */
long	linger = -1;		/* 0 or positive turns on option */
int	listenq = 5;		/* listen queue for TCP Server */
int	nodelay;		/* TCP_NODELAY (Nagle algorithm) */
int	nbuf = 1024;		/* number of buffers to write (sink mode) */
int	pauseclose;		/* seconds to sleep after recv FIN, before close */
int	pauseinit;		/* seconds to sleep before first read */
int	pauselisten;		/* seconds to sleep after listen() */
int	pauserw;		/* seconds to sleep before each read or write */
int	reuseaddr;		/* SO_REUSEADDR */
int	readlen = MAX_DATA_LEN;//1024;		/* default read length for socket */
int	writelen = MAX_DATA_LEN;//1024;	/* default write length for socket */
int	recvdstaddr;		/* IP_RECVDSTADDR option */
int	rcvbuflen;		/* size for SO_RCVBUF */
int	sndbuflen;		/* size for SO_SNDBUF */
unsigned char   *rbuf;	 		/* pointer that is malloc'ed */
unsigned char    *wbuf;			/* pointer that is malloc'ed */
int	server;			/* to act as server requires -s option */
int	sourcesink;		/* source/sink mode */
int	urgwrite;		/* write urgent byte after this write */

int     fdrvConn = 0;    	/* listening child process can keep track of drv use */

int
srv_run(char *host)
{
  int			fd, newfd, i, on, pid;
  unsigned long		inaddr;
  struct sockaddr_in	cli_addr, serv_addr;
  
  /* Initialize the socket address structure */
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  
  /* Caller normally wildcards the local Internet address, meaning
     a connection will be accepted on any connected interface.
     We only allow an IP address for the "host", not a name. */
  if (host == NULL)
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);		/* wildcard */
  else {
    if ( (inaddr = inet_addr(host)) == INADDR_NONE)
      err_quit("invalid host name for server: %s", host);
    serv_addr.sin_addr.s_addr = inaddr;
  }
  
  /* See if "port" is a service name or number */
 serv_addr.sin_port = htons( PORT);
  
  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err_sys("socket() error");
  
  if (reuseaddr) {
    on = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		   (char *) &on, sizeof (on)) < 0)
      err_sys("setsockopt of SO_REUSEADDR error");
  }
  
  /* Bind our well-known port so the client can connect to us. */
  if (bind(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    err_sys("can't bind local address");
  
  buffers(fd);		/* may set receive buffer size; must do here to get
			   correct window advertised on SYN */
  sockopts(fd, 0);	/* only set some socket options for fd */
  
  listen(fd, listenq);
  
  if (pauselisten)
    sleep(pauselisten);	/* lets connection queue build up */

  if (dofork)
    TELL_WAIT();	/* initialize synchronization primitives */
  
  for ( ; ; ) {
    i = sizeof(cli_addr);
    if ( (newfd = accept(fd, (struct sockaddr *) &cli_addr, &i)) < 0)
      err_sys("accept() error");
    
    DBG_PRN("concurent server? %d\n", dofork);  
    if (dofork) {
      if ( (pid = fork()) < 0)
	err_sys("fork error");
      
      if (pid > 0) {
        DBG_PRN("srv - (parent)\n");
	close(newfd);	/* parent closes connected socket */
	WAIT_CHILD();	/* wait for child to output to terminal */
	if( waitpid( pid, NULL, 0) != pid)
	  err_sys("waitpid error");
	continue;	/* and back to for(;;) for another accept() */
      } else {

        DBG_PRN("srv - (child, closing)\n");
	close(fd);		/* child closes listening socket */
      }
    }
    
    /* child (or iterative server) continues here */
    /* Call getsockname() to find local address bound to socket:
       local internet address is now determined (if multihomed). */
    i = sizeof(serv_addr);
    if (getsockname(newfd, (struct sockaddr *) &serv_addr, &i) < 0)
       err_sys("getsockname() error");
      
    DBG_PRN("connection on %s.%d ",
	    INET_NTOA(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
    DBG_PRN("from %s.%d\n",
	    INET_NTOA(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
    DBG_PRN("srv - (child, running)\n");
    
    buffers(newfd);	/* setsockopt() again, in case it didn't propagate
			   from listening socket to connected socket */
    sockopts(newfd, 1);	/* can set all socket options for this socket */
    
    if (dofork)
      TELL_PARENT(getppid());	/* tell parent we're done with terminal */
    
    return(newfd);
  }
}

int
main( void)
{
  int  fd;
  
  dofork  = 1; sourcesink = 1; pauseclose = 2;
  fd = srv_run(host);
  
  if (sourcesink)
    sink(fd);			/* ignore stdin/stdout */
  
  exit(0);
}
