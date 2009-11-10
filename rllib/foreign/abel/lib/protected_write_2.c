
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



extern struct _data protwrite2 (struct _comm comm1, char *addr, int count, 
			struct _data outdata, int plctype, int debug)
{
struct ethernet_header header,rcvd;
struct pccc_data df1_1;
struct _data retdata;
struct namedata name;
struct custom_pccc custom;
int x,place,d,location,hlen;
	bzero (&header,sizeof(header));
	bzero (&rcvd, sizeof(rcvd));
	bzero (&retdata,sizeof(retdata));
	if (plctype != SLC)
		return retdata;
	header.mode = 1;
	header.submode = PCCC_CMD;
	header.conn = htonl(comm1.commid);
	custom.request_id = 0xe87573;
	df1_1.src=0;
	df1_1.control=5;
	df1_1.dst=0;	
	df1_1.lsap=0;
	df1_1.cmd=0x0f;
	df1_1.sts=0;
	df1_1.tns = htons(comm1.tns);
	df1_1.fnc=0xa9;
	name = nameconv5(addr,plctype,debug);
	retdata.name = name;
	place=0;
	if (count==0)
		count=1;
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
	location=0;

	for (x=0;x<count;x++)
		{
		if (name.floatdata == TRUE)
			{
			d=outdata.data[location+1];
			df1_1.data[place++] = (d & 255);
			df1_1.data[place++] = (d / 256);
			d=outdata.data[location];
			df1_1.data[place++] = (d & 255);
			df1_1.data[place++] = (d / 256);
			location = location + 2;
			}
		if (name.floatdata == FALSE)
			{
			d=outdata.data[x];
			df1_1.data[place++] = (d & 255);
			if (name.typelen > 1)
				df1_1.data[place++] = (d / 256);
			}
		}
	memcpy (&header.df1_stuff, &df1_1, PCCC_DATA_LEN+place);
	header.pccc_length = htons(PCCC_DATA_LEN+place);
	hlen = _ENET_HEADER_LEN + PCCC_DATA_LEN + place;
	
	rcvd = senddf1 (comm1, header, hlen, debug);

	if (rcvd.df1_stuff[4] != (df1_1.cmd + 0x40))
		{
		retdata.len=-1;
		retdata.data[0]=BADCMDRET;
		retdata.data[1]=0;
		return retdata;
		}
	if (rcvd.df1_stuff[5] != 0)
		{
		retdata.len=-1;
		retdata.data[0]=rcvd.df1_stuff[5];           //Error Response - STS
		retdata.data[1]=rcvd.df1_stuff[8];           //Error Response - EXT STS
		return retdata;
		}
return retdata;
}



