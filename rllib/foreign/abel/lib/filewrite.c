
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



extern struct _data filewrite (struct _comm comm1, char *addr, int count, 
			struct _data outdata, int plctype, int debug)
{
struct ethernet_header header,rcvd;
struct pccc_word_read df1_1;
struct _data retdata;
struct namedata name;
struct custom_pccc custom;
int x,place,d,hlen;

	bzero (&header,sizeof(header));
	bzero (&rcvd, sizeof(rcvd));
	bzero (&retdata,sizeof(retdata));
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
	df1_1.fnc=3;
	df1_1.offset = 0;
	name = nameconv5(addr,plctype,debug);
	for (x=0;x<name.len;x++)
		df1_1.data[x] = name.data[x];
	place = name.len;
	if (count == 0)
		count = 1;
	count = count * 2;
	for (x=0;x<(count/2);x++)
		{
		d=outdata.data[x];
		df1_1.data[place] = (d & 255);
		place++;
		df1_1.data[place] = (d / 256);
		place++;
		}
	df1_1.trans = (count/2);
	place++;
	memcpy (&header.df1_stuff, &df1_1, PCCC_WORD_LEN+place);
	header.pccc_length = htons(PCCC_WORD_LEN+place);
	hlen = _ENET_HEADER_LEN + PCCC_WORD_LEN + place;
	
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



