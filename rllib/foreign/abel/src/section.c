
// This is an example program for communicating with an Allen Bradley PLC-5 over
// Ethernet.  This is only to demonstrate the methodology needed.  This program is not
// necessarily useful for any other purpose.

//  Copyright (1999) by Ron Gage 

#include "../lib/libabplc5.h"
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


int main (int argc, char *argv[])
{
struct _comm comm;
struct _size_data data, data1;
	int count,x,y;
	struct namedata name;
	count=0;

	comm=abel_attach(argv[1],FALSE);
	if (comm.error != 0)
		{
		printf ("Could not connect.  Check your address and try again.\n");
		exit (-1);
		}
	uploadreq (comm,FALSE);
	comm.tns = comm.tns + 4;
	setpriv (comm, FALSE);
	comm.tns = comm.tns + 4;
	for (x=0;x<7;x++)
		{
		bzero (&data, sizeof(data));
		name.data[0]=1;
		name.data[1]=x;
		name.len=2;
		data=readsectionsize(comm, name, PLC5, FALSE);
		printf ("Section %d is size %d.\n",x,data.count);
		comm.tns = comm.tns + 4;
		if (data.count != 0)
			for (y=0;y<data.count;y++)
				{
				bzero (&data1, sizeof(data1));
				name.data[0]=3;
				name.data[1]=x;
				name.data[2]=y;
				name.len=3;
				data1 = readsectionsize(comm, name, PLC5, FALSE);
				printf ("	Section %d.%d has %d entries.\n",x,y,data1.count);
				comm.tns = comm.tns + 4;
				}
		}


		
	uploaddone (comm,FALSE);
	close (comm.handle);
	exit (0);
}

