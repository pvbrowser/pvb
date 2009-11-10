
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

int p,x,l,junk;

char dataaddr[32];

/* This routine will read the data from a PLC5.  


*/

extern struct _data protread3 (struct _comm comm1, char *addr, int count, int plctype, int debug)

{

struct ethernet_header header,rcvd;
struct pccc_data df1_1;
struct namedata name;
struct _data data;
struct custom_pccc custom;
int place,index,hlen;
	bzero (&header,sizeof(header));
	bzero (&rcvd, sizeof(rcvd));
	bzero (&custom, sizeof(custom));
	bzero (&data, sizeof(data));
	if (plctype != SLC)
		return data;
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
	df1_1.fnc=0xa2;
	name = nameconv5(addr,plctype,debug);
	data.name = name;
	place=0;
	df1_1.data[place++] = name.typelen * count;
	if (name.file > 254)
		{
		df1_1.data[place++] = 0xff;
		df1_1.data[place++] = (name.file & 255);
		df1_1.data[place++] = (name.file / 256);
		}
	if (name.file < 255)
		df1_1.data[place++] = name.file;
	df1_1.data[place++] = name.type;
	if (name.element > 254)
		{
		df1_1.data[place++] = 0xff;
		df1_1.data[place++] = (name.element & 255);
		df1_1.data[place++] = (name.element / 256);
		}
	if (name.element < 255)
		df1_1.data[place++] = name.element;
	df1_1.data[place++] = name.subelement;
	
	memcpy (&header.df1_stuff, &df1_1, PCCC_DATA_LEN + place);
	header.pccc_length = htons(PCCC_DATA_LEN + place);
	hlen = _ENET_HEADER_LEN + PCCC_DATA_LEN + place;
		
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
	data.len = count;
	if (name.floatdata == TRUE)
		count = count * 2;
	p=0;
	for (x=0;x<count;x++)
		{
		index = 8+(x*name.typelen);
		switch (name.typelen)
			{
			case 1:
				data.data[p++] = rcvd.df1_stuff[index];
				break;
			case 2:
				data.data[p++] = (rcvd.df1_stuff[index+1]*256) + rcvd.df1_stuff[index];
				break;
			case 4:
				data.data[p++] = (rcvd.df1_stuff[index+3]*256) + rcvd.df1_stuff[index+2];
				data.data[p++] = (rcvd.df1_stuff[index+1]*256) + rcvd.df1_stuff[index];
				break;
			}			
			
		}
return data;
}


