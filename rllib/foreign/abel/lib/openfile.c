
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

/* This routine will read the data from a PLC5.  


*/

extern struct _data openfile (struct _comm comm1, struct _file file, int debug)

{
struct ethernet_header header,rcvd;
struct pccc_data df1_1;
struct _data data;
struct custom_pccc custom;
int hlen;
	bzero (&header,sizeof(header));
	bzero (&rcvd, sizeof(rcvd));
	bzero (&custom, sizeof(custom));
	header.mode = 1;
	header.submode = PCCC_CMD;
	header.conn = htonl(comm1.commid);
	custom.request_id = htonl(0x7377e8);
	memcpy (&header.custom, &custom, _CUSTOM_LEN);

	df1_1.src=0;
	df1_1.control=5;
	df1_1.dst=0;	
	df1_1.lsap=0;
	df1_1.cmd=0x0f;
	df1_1.sts=0;
	df1_1.tns=htons(comm1.tns);
	df1_1.fnc=0x81;
	memcpy (&df1_1.data[0], &file, sizeof(file));

	memcpy (&header.df1_stuff, &df1_1, PCCC_DATA_LEN + sizeof(file));
	header.pccc_length = htons(PCCC_DATA_LEN + sizeof(file));
	hlen = _ENET_HEADER_LEN + PCCC_DATA_LEN + sizeof(file);
	
	rcvd = senddf1 (comm1, header, hlen, debug);

	if (rcvd.df1_stuff[4] != (df1_1.cmd + 0x40))
		{
		data.len=-1;
		data.data[0]=BADCMDRET;
		data.data[1]=0;
		return data;
		}
	if (rcvd.df1_stuff[5] != 0)
		{
		data.len=-1;
		data.data[0]=rcvd.df1_stuff[5];           //Error Response - STS
		data.data[1]=rcvd.df1_stuff[8];           //Error Response - EXT STS
		return data;
		}
	data.data[0] = rcvd.df1_stuff[8];
	data.data[1] = rcvd.df1_stuff[9];
	data.len = 1;
	
return data;
}


