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

extern struct results closefile (struct _comm comm1, int filetag, int debug)
{
struct ethernet_header header, rcvd;
struct pccc_data df1_2;
struct results result;
struct custom_pccc custom;
int hlen;
	bzero (&header,sizeof(header));
	bzero (&rcvd, sizeof(rcvd));
	header.mode = 1;
	header.submode = PCCC_CMD;
	header.conn = htonl(comm1.commid);
	custom.request_id = htonl(0xe87573);
	custom.name_id = 0;
	memcpy (&header.custom, &custom, _CUSTOM_LEN);
	df1_2.src=0;
	df1_2.control = 5;
	df1_2.dst=0;	
	df1_2.lsap = 0;
	df1_2.cmd=0x0f;
	df1_2.sts=0;
	df1_2.tns=htons(comm1.tns);
	df1_2.fnc=0x82;
	df1_2.data[0] = (filetag / 256);
	df1_2.data[1] = (filetag & 255);
	hlen = _ENET_HEADER_LEN + PCCC_DATA_LEN + 2;
	memcpy (&header.df1_stuff, &df1_2, PCCC_DATA_LEN + 2);
	header.pccc_length = htons(PCCC_DATA_LEN + 2);

	rcvd = senddf1 (comm1, header, hlen, debug);

	result.sts = rcvd.df1_stuff[5];
	result.extsts = rcvd.df1_stuff[8];
	if (rcvd.df1_stuff[4] != (df1_2.cmd + 0x40))
		result.sts = 255;

return result;
}

