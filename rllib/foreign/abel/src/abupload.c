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
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

int main (int argc, char *argv[])
{
	int count,x,y,file;
	struct _size_data section[7],data1[7][1024];
	struct namedata name;
	struct _comm comm;
	struct _upload_data upload;
	struct results result;
	struct plc5stat stat;
	count=0;
	if (argc != 3)
		{
		printf ("ABUPLOAD - Reads all memory in an Ethernet connected PLC-5\n");
		printf ("\nusage: abupload <plc5> <file>- where <plc5> is either the IP address or\n");
		printf ("the domain name of the PLC5 you wish to upload and <file> is the file you\n");
		printf ("want to store the data into.\n\n");
		exit (-1);
		}

	comm=abel_attach(argv[1],FALSE);
	if (comm.error != 0)
		{
		printf ("Could not connect.  Check your address and try again.\n");
		exit (-1);
		}
	stat = getstatus(comm, FALSE);
	if (stat.type != 0xeb)
		{
		close(comm.handle);
		printf ("Wrong PLC type - a PLC-5 is required for this command.\n");
		exit(-1);
		}
	file = open(argv[2],O_RDWR | O_TRUNC);
	if (file == -1)
		{
		printf ("Could not create file %s for writing.\n",argv[2]);
		exit(-1);
		}
		
	count=0;

	uploadreq (comm,FALSE);
	comm.tns = comm.tns + 4;
	setpriv (comm, FALSE);
	comm.tns = comm.tns + 4;
	for (x=0;x<7;x++)
		{
		memset (&section[x],0, sizeof(section[x]));
		name.data[0]=1;
		name.data[1]=x;
		name.len=2;
		section[x]=readsectionsize(comm, name, PLC5, FALSE);
		printf ("Section %d has %d files.\n",x,section[x].count);
		write (file,&section[x].count, sizeof(section[x].count));
		comm.tns = comm.tns + 4;
		}
	for (x=0;x<7;x++)
		{
		if (section[x].count != 0)
			{
			printf ("Reading file sizes from section %d\n",x);
			for (y=0;y<section[x].count;y++)
				{
				bzero (&data1[x][y], sizeof(data1[x][y]));
				name.data[0]=3;
				name.data[1]=x;
				if (y < 255)
					{
					name.data[2]=y;
					name.len=3;
					}
				if (y >= 255)
					{
					name.data[2]=0xff;
					name.data[3]=(y & 255);
					name.data[4]=(y / 256);
					name.len = 5;
					}
				data1[x][y] = readsectionsize(comm, name, PLC5, FALSE);
				write (file,&data1[x][y].count,sizeof(data1[x][y].count));
				comm.tns = comm.tns + 4;
				}
			}
		}

		
	uploaddone (comm,FALSE);
	close (comm.handle);
	close (file);
	exit (0);
}


