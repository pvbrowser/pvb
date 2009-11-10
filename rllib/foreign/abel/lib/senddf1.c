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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

struct ethernet_header senddf1 (struct _comm comm1, struct ethernet_header header, int hlen, int debug)
{

	byte junk[255],response[255];
	struct ethernet_header rcvd;
	int x,response_len;
	switch (comm1.mode)
		{
		case ETHERNET:
		if (debug==1)
			{
			memcpy (&junk, &header,hlen);
			printf ("Sending...\n");
			for (x=0; x<hlen; x++)
				printf ("%02X  ",junk[x]);
			printf ("\n\n");
			}
		
		write (comm1.handle, &header,hlen);
		response_len = read (comm1.handle, &response, 255);

		if (debug==1)
			{
			printf ("Received %d bytes back...\n",response_len);
			for (x=0;x<response_len;x++)
				printf ("%02X  ",response[x]);
			printf ("\n");
			}
	
		memcpy (&rcvd, &response, response_len);

		return rcvd;
		}
	

return rcvd;
}

