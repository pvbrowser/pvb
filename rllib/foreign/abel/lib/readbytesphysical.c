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

extern struct _data readphysical (struct _comm comm1, long start_addr, int count, int debug)
{
struct ethernet_header header, rcvd;
struct pccc_memread df1_2;
struct _data data;
struct custom_pccc custom;
int hlen,x;

	if (count > 240)
		count = 240;
	bzero (&header,sizeof(header));
	bzero (&rcvd, sizeof(rcvd));
	header.mode = 1;
	header.submode = PCCC_CMD;
	header.conn = htonl(comm1.commid);
	custom.request_id = htonl(0xe887573);
	custom.name_id=0;
	memcpy (&header.custom, &custom, _CUSTOM_LEN);
	df1_2.src=0;
	df1_2.control=5;
	df1_2.dst=0;	
	df1_2.lsap=0;
	df1_2.cmd=0x0f;
	df1_2.sts=0;
	df1_2.tns=htons(comm1.tns);
	df1_2.fnc=0x17;
	df1_2.addr=start_addr;
	df1_2.count=count;
	hlen = _ENET_HEADER_LEN + PCCC_MEM_LEN ;
	memcpy (&header.df1_stuff, &df1_2, PCCC_MEM_LEN);
	header.pccc_length = htons(PCCC_MEM_LEN);

	rcvd = senddf1 (comm1, header, hlen, debug);

	bzero (&data, sizeof(data));
	if (rcvd.df1_stuff[4] != (df1_2.cmd + 0x40))
		{
		data.len = -1;
		return data;
		}
	if (rcvd.df1_stuff[5] != 0)
		{
		data.len = -1;
		data.data[0] = rcvd.df1_stuff[5]; //sts
		data.data[1] = rcvd.df1_stuff[8]; //extsts
		return data;
		}
	for (x=0;x<(rcvd.pccc_length-8);x++)
		data.data[x] = rcvd.df1_stuff[x+8];
	data.len = rcvd.pccc_length-8;
return data;
}

