
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

extern struct _typed_data typed_read (struct _comm comm1, char *addr, int count, int plctype, int debug)

{
struct ethernet_header header,rcvd;
struct pccc_word_read df1_1;
struct namedata name;
struct _typed_data data;
struct custom_pccc custom;
int place,start,hlen,y;
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
	df1_1.fnc=0x68;
	df1_1.offset = 0;
	name = nameconv5(addr,plctype,debug);
	memcpy (&data.name,&name,sizeof(name));;
	for (x=0;x<name.len;x++)
		df1_1.data[x] = name.data[x];
	place = name.len;
	if (count == 0)
		count = 1;
	df1_1.trans= (count);
	df1_1.data[place++] = ((byte)count & 255);
	df1_1.data[place++] = ((byte) count / 256);
	memcpy (&header.df1_stuff, &df1_1, PCCC_WORD_LEN+place);
	hlen = _ENET_HEADER_LEN + PCCC_WORD_LEN + place;
	header.pccc_length = htons(PCCC_WORD_LEN + place);

	rcvd = senddf1 (comm1, header, hlen, debug);

	if (rcvd.df1_stuff[4] != (df1_1.cmd + 0x40))
		{
		data.typelen=-1;
		data.data[0]=BADCMDRET;
		data.data[1]=0;
		return data;
		}
	if (rcvd.df1_stuff[5] != 0)
		{
		data.typelen=-1;
		data.data[0]=rcvd.df1_stuff[5];           //Error Response - STS
		data.data[1]=rcvd.df1_stuff[8];           //Error Response - EXT STS
		return data;
		}
	start=9;
	data.type = (rcvd.df1_stuff[8] / 16);
	data.typelen = (rcvd.df1_stuff[8] & 15);
	if (data.type > 7)
		{
		data.type = rcvd.df1_stuff[start++];
		}
	if (data.typelen > 7)
		{
		data.typelen = rcvd.df1_stuff[start++];
		}
	if (data.type == 9)
		{
		data.type = (rcvd.df1_stuff[start] / 16);
		data.typelen = (rcvd.df1_stuff[start++] & 15);
		if (data.type == 9)
			data.type = rcvd.df1_stuff[start++];
		if (data.typelen == 9)
			data.typelen = rcvd.df1_stuff[start++];
		}
	if (name.floatdata == TRUE)
		count = count * 2;	
	data.len = count;
	if ((plctype == PLC5)||(plctype == SLC))
		switch (name.section)
			{
			case 0:
			case 2:
				y=0;
				for (x=0;x<count;x++)
					{
					switch (data.typelen)
						{
						case 1:
							data.data[y++] = rcvd.df1_stuff[start++];
							break;
						case 2:
							data.data[y++] = (rcvd.df1_stuff[start+1]*256) + rcvd.df1_stuff[start];
							start = start + data.typelen;
							break;
						case 4:
							data.data[y++] = (rcvd.df1_stuff[start+3]*256) + 	rcvd.df1_stuff[start+2];
							data.data[y++] = (rcvd.df1_stuff[start+1]*256) + rcvd.df1_stuff[start+0];
							start = start + data.typelen;
							break;
						case 6:
							data.data[y++] = (rcvd.df1_stuff[start+1]*256) + rcvd.df1_stuff[start];
							start += 2;
							data.data[y++] = (rcvd.df1_stuff[start+1]*256) + rcvd.df1_stuff[start];
							start += 2;
							data.data[y++] = (rcvd.df1_stuff[start+1]*256) + rcvd.df1_stuff[start];
							start += 2;
							break;
						}
					}
				break;
			
		case 1:
		case 3:
		case 4:
		case 5:
		case 6:
			data.len = data.typelen;
			data.len++;
			for (x=0;x<data.len;x++)
				{
				data.data[x] = rcvd.df1_stuff[x+start];
				}
		}
	if (plctype == PLC5250)
		for (x=0;x<data.len;x++)
			{
			switch (data.typelen)
				{
				case 1:
					data.data[x] = rcvd.df1_stuff[start++];
					break;
				case 2:
					data.data[x] = (rcvd.df1_stuff[start+1]*256) + rcvd.df1_stuff[start];
					start = start + data.typelen;
					break;
				case 4:
					data.data[x++] = (rcvd.df1_stuff[start+3]*256) + 	rcvd.df1_stuff[start+2];
					data.data[x] = (rcvd.df1_stuff[start+1]*256) + rcvd.df1_stuff[start+0];
					start = start + data.typelen;
					break;
				}
			}
		
return data;
}




