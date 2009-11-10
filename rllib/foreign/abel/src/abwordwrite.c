
// This is an example program for communicating with an Allen Bradley PLC-5 over
// Ethernet.  This is only to demonstrate the methodology needed.  This program is not
// necessarily useful for any other purpose.

//  Copyright (1999) by Ron Gage 

#include <libabplc5.h>
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
struct _data data,ret;
struct plc5stat status;
struct namedata name;
struct floatstuff fdata;
	int count,x,sts,extsts,type;
	byte buf[32];
	count=0;
	if ((argc < 3) || (argc > 4))
		{
		printf ("\nThis program will write registers to an Ethernet connected Allen Bradley\n");
		printf ("PLC-5.  It should also work with a Pyramid Integrator.\n\n");
		printf ("This example program will not work on a SLC-500.\n\n");
		printf ("Correct Usage:\nabwordwrite <plc ip addr> <plc register> {<quantity>}\n");
		printf ("abwordwrite 192.168.10.5 n7:0   - will write one integer from N7:0 on plc 192.168.10.5\n");
		printf ("abwordwrite plc n7:30 4 - will write four integers from N7:30 on plc 192.168.10.5\n"); 
		printf ("\n\n");
		exit (-1);
		}
	if (argc == 4)
		count = atoi(argv[3]);
	if (argc == 3)
		count = 1;
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
	name = nameconv5(argv[2],type,FALSE);
	if (name.floatdata != TRUE)
		{
		for (x=0;x<count;x++)
			{
			printf ("Entry #%d of %d:",x+1,count);
			fgets (buf,10,stdin);
			printf ("\n");
			data.data[x]=atoi(buf);
			}
		}
	if (name.floatdata == TRUE)
		{
		count =  count*2;
		for (x=0;x<count;x=x+2)
			{
			printf ("Entry #%d of %d:",(x/2)+1,count/2);
			bzero (&buf,sizeof(buf));
			fgets (buf,10,stdin);
			printf ("\n");
			fdata = ftoi (atof(buf));
			data.data[x] = fdata.data1;
			data.data[x+1] = fdata.data2;
			}
		}
	ret = word_write (comm, argv[2], count, data, type, TRUE);
	if (ret.len < 0)
		{
		sts = ret.data[0];
		extsts = ret.data[1];
		printf ("An error occured.  The PLC STS byte is %d, the EXT STS byte is %d\n",sts,extsts);
		if (sts != 0xf0)
			printf ("Primary Error code is %s\n",errors[(sts/16)]);
		if (extsts != 0)
			printf ("Extended error code is %s\n",ext_errors[extsts]);
		}

		
	close (comm.handle);
	exit (0);
}

