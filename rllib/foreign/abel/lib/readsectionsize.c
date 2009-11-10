
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

extern struct _size_data readsectionsize (struct _comm comm1, struct namedata name, int plctype, int debug)

{
struct ethernet_header header,rcvd;
struct _size_data data;
struct pccc_data df1_1;
struct custom_pccc custom;
int place,hlen;
	bzero (&header,sizeof(header));
	bzero (&rcvd, sizeof(rcvd));
	bzero (&data, sizeof(data));
	header.mode = 1;
	header.submode = 7;
	header.conn = htonl(comm1.commid);
	custom.request_id = 0xe87573;
	custom.name_id = 0;
	memcpy (&header.custom, &custom, _CUSTOM_LEN);
	df1_1.src=0;
	df1_1.control=5;
	df1_1.dst=0;	
	df1_1.lsap=0;
	df1_1.cmd=0x0f;
	df1_1.sts=0;
	df1_1.tns=htons(comm1.tns);
	df1_1.fnc=0x29;
	for (x=0;x<name.len;x++)
		df1_1.data[x] = name.data[x];
	place = name.len;

	memcpy (&header.df1_stuff, &df1_1, PCCC_DATA_LEN+place);
	header.pccc_length = htons(PCCC_DATA_LEN + place);
	hlen = _ENET_HEADER_LEN + PCCC_DATA_LEN + place;
	
	rcvd = senddf1 (comm1, header, hlen, debug);

	if (rcvd.df1_stuff[4] != (df1_1.cmd + 0x40))
		{
		data.sts = -1;
		return data;
		}
	if (rcvd.df1_stuff[5] != 0)
		{
		data.sts=rcvd.df1_stuff[5];           //Error Response - STS
		data.extsts=rcvd.df1_stuff[8];           //Error Response - EXT STS
		return data;
		}

	data.size_lo = (rcvd.df1_stuff[9]*256)+rcvd.df1_stuff[8];
	data.size_hi = (rcvd.df1_stuff[11]*256)+rcvd.df1_stuff[10];
	data.count = (rcvd.df1_stuff[13]*256)+rcvd.df1_stuff[12];
	data.write_priv = rcvd.df1_stuff[14];
	data.read_priv = rcvd.df1_stuff[15];
	data.type = rcvd.df1_stuff[16];
	

return data;

}


