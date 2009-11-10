#ifdef SRV_DEBUG

# define SRV_PRN(lineno,fmt,args...) fprintf(stderr, fmt,lineno,##args)
# define DBG_PRN(fmt,args...) SRV_PRN((__LINE__), __FILE__"::"__FUNCTION__"(L%.4d): "fmt,##args)
#else
#  define DBG_PRN(fmt, args...) /* not debugging: nothing */
#endif


#include	<sys/types.h>
#include	<sys/wait.h>
#include	<sys/socket.h>
#include	<sys/uio.h>
#include	<sys/time.h>		/* struct timeval for select() */
#ifdef	_AIX
#include	<sys/select.h>
#endif
#include	<netinet/in.h>
#ifdef	__bsdi__
#include	<machine/endian.h>	/* required before tcp.h, for BYTE_ORDER */
#endif
#include	<netinet/tcp.h>		/* TCP_NODELAY */
#include	<netdb.h>			/* getservbyname(), gethostbyname() */
#include	<arpa/inet.h>		/* inet_addr() */
#include	<errno.h>
#include	<signal.h>
#include	"OURHDR.h"

#define MAX_DATA_LEN 2048

#define PORT 1099
#define BOARD_SELECT_COMMAND 0
#define BOARD_SELECT_ANSWER  1
				/* declare global variables */
extern int		bindport;
extern int		broadcast;
extern int		cbreak;
extern int		client;
extern int		crlf;
extern int		debug;
extern int		dofork;
extern char		foreignip[];
extern int		foreignport;
extern int		halfclose;
extern int		keepalive;
extern long		linger;
extern int		listenq;
extern int		nodelay;
extern int		nbuf;
extern int		pauseclose;
extern int		pauseinit;
extern int		pauselisten;
extern int		pauserw;
extern int		reuseaddr;
extern int		readlen;
extern int		writelen;
extern int		recvdstaddr;
extern int		rcvbuflen;
extern int		sndbuflen;
extern unsigned char	*rbuf;
extern unsigned char	*wbuf;
extern int		server;
extern int		sourcesink;
extern int		urgwrite;

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff	/* should be in <netinet/in.h> */
#endif

#if	defined(sun) && defined(__GNUC__) && defined(GCC_STRUCT_PROBLEM)
#define	INET_NTOA(foo)	inet_ntoa(&foo)
#else
#define	INET_NTOA(foo)	inet_ntoa(foo)
#endif

	/* function prototypes */
void	buffers(int);
int	cliopen(char *host, char *port);
int	servopen(char *host);///, char *port);
void	sink(int sockfd);
void	sockopts(int sockfd, int doall);
