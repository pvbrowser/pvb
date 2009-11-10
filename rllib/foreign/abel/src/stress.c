
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
struct _data data;
struct plc5stat status;
	int count,sts,extsts,lcount,type;
	count=0;
	lcount=0;
	if (argc == 3)
		count = 1;
	if (argc == 4)
		count = atoi(argv[3]);
	if ((argc < 3) || (argc > 4))
		{
		printf ("\nThis program will read a register from an Ethernet connected Allen Bradley\n");
		printf ("PLC-5.  It should also work with a Pyramid Integrator.\n\n");
		printf ("Correct Usage:\nab <plc ip addr> <plc register> {<quantity>}\n");
		printf ("ab 192.168.10.5 n7:0   - will read one integer from N7:0 on plc 192.168.10.5\n");
		printf ("ab 192.168.10.5 n7:30 4 - will read four integers from N7:30 on plc 192.168.10.5\n"); 
		printf ("\n\n");
		exit (-1);
		}

	comm=abel_attach(argv[1],FALSE);
	if (comm.error != 0)
		{
		printf ("Could not connect.  Check your address and try again.\n");
		exit (-1);
		}
	status = getstatus(comm,FALSE);
	type = PLC5;
	if (status.type == 0xde)
		type = PLC5250;
	comm.tns = comm.tns + 4;
do
	{
	lcount++;
	data=word_read(comm,argv[2],count,type,FALSE);
	if (data.len < 0)
		{
		sts = data.data[0];
		extsts = data.data[1];
		printf ("An error occured.  The PLC STS byte is %d, the EXT STS byte is %d\n",sts,extsts);
		if (sts != 0xf0)
			printf ("Primary Error code is %s\n",errors[(sts/16)]);
		if (extsts != 0)
			printf ("Extended error code is %s\n",ext_errors[extsts]);
		}
	printf ("Loop # %d   got %d bytes back.\n",lcount,data.len);
//	for (x=0;x<data.len;x++)
//		printf ("%d\n",data.data[x]);
	comm.tns = comm.tns + 4;
	} while (data.len >= 0);
	
	close (comm.handle);
	exit (0);
}

