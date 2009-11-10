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
	int mode,type;
	struct results result;
	struct plc5stat data;
	struct _comm comm;
	type = PLC5;
	mode = -1;
	if (argc != 3)
		{
		printf ("ABSETMODE - sets the run mode of an Ethernet connected PLC-5\n");
		printf ("\nusage: abstat <plc5> mode - where <plc5> is either the IP address or\n");
		printf ("the domain name of the PLC5 you wish to change.\n");
		printf ("mode = the operational mode you wish to plc to be set to.  Valid modes\n");
		printf ("are: run, test, prog.  No other strings are accepted and these strings\n");
		printf ("must be in lower case.\n\n");
		exit (-1);
		}

	comm=abel_attach(argv[1],FALSE);
	if (comm.error != 0)
		{
		printf ("Could not connect.  Check your address and try again.\n");
		exit (-1);
		}
	data = getstatus(comm,FALSE);
	comm.tns = comm.tns + 4;
	switch (data.type)
		{
		case 0xeb:
			type = PLC5;
			break;
		case 0xde:
			type = PLC5250;
			break;
		case 0xee:
			type = SLC;
			break;
		}
		
	if (strcasecmp (argv[2],"prog") == 0)
		{
		mode = PROGRAM_MODE;
		if (data.type == 0xee)
			mode = SLC_PROGRAM_MODE;
		}
	if (strcasecmp (argv[2],"test") == 0)
		{
		mode = TEST_MODE;
		if (data.type == 0xee)
			mode = SLC_TEST_CONT;
		}
	if (strcasecmp (argv[2],"run") == 0)
		{
		mode = RUN_MODE;
		if (data.type == 0xee)
			mode = SLC_RUN_MODE;
		}
	if (mode >= 0)
		result = setplcmode (comm, type, mode, FALSE);
	if (result.sts != 0)
		{
		printf ("An error occured.  The PLC STS byte is %d, the EXT STS byte is %d\n",result.sts,result.extsts);
		if (result.sts != 0xf0)
			printf ("Primary Error code is %s\n",errors[(result.sts/16)]);
		if (result.extsts != 0)
			printf ("Extended error code is %s\n",ext_errors[result.extsts]);
		}
	close (comm.handle);
	exit(0);
	}