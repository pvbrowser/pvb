
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
struct namedata name;
	int x;
	printf ("About to convert name %s\n",argv[1]);
	name = nameconv5(argv[1],SLC,TRUE);
	if (name.len == 0)
		name = nameconv5(argv[1],PLC5250,TRUE);
	for (x=0;x<name.len;x++)
		printf ("%02X  ",name.data[x]);
	printf ("\n");


	exit (0);
}

