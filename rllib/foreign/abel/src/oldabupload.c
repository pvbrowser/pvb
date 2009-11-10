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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

int main (int argc, char *argv[])
{
	int count;
	struct _comm comm;
	struct _upload_data upload;
	struct results result;
	count=0;
	if (argc != 2)
		{
		printf ("ABUPLOAD - Reads all memory in an Ethernet connected PLC-5\n");
		printf ("\nusage: abupload <plc5> - where <plc5> is either the IP address or\n");
		printf ("the domain name of the PLC5 you wish to upload.\n\n");
		exit (-1);
		}

	comm=abel_attach(argv[1],FALSE);
	if (comm.error != 0)
		{
		printf ("Could not connect.  Check your address and try again.\n");
		exit (-1);
		}
	upload = uploadreq(comm, TRUE);
	if (upload.sts != 0)
		{
		printf ("An error occurred.  STS = %02X, EXTSTS = %02X\n",upload.sts,upload.extsts);
		if (upload.sts != 0xf0)
			printf ("Primary Error code: %s\n",errors[(upload.sts/16)]);
		if (upload.extsts != 0)
			printf ("Secondary Error code: %s\n",ext_errors[upload.extsts]);
		exit (-1);
		}
		
	printf ("Uploadable Segment Count = %d\n",upload.uploadable_seg_count);
/*	for (x=0;x<upload.uploadable_seg_count;x++)
		printf ("Seg Start: %08lX   Seg End: %08lX\n",upload.upload_seg_start[x],upload.upload_seg_end[x]);*/
	printf ("Comparable Segment Count = %d\n",upload.comparable_seg_count);
/*	for (x=0;x<upload.comparable_seg_count;x++)
		printf ("Seg Start: %08lX   Seg End: %08lX\n",upload.comparable_seg_start[x],upload.comparable_seg_end[x]);*/
	comm.tns= comm.tns+4;
/*	for (a=upload.upload_seg_start[0];a<upload.upload_seg_end[0];a=a+240)
		{
		data = readphysical(comm, a ,240,FALSE);
		for (x=0;x<data.len;x++)
			printf ("%02X ",data.data[x]);
		comm.tns = comm.tns+4;
		}*/
	result = uploaddone(comm,FALSE);
		

	


	close (comm.handle);
	exit (0);
}
