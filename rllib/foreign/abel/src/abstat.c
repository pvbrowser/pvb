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
	int count,x,id_found;
	struct _comm comm;
	struct plc5stat plcstatus;
	count=0;
	if (argc != 2)
		{
		printf ("ABSTAT - retrieve current status from an Ethernet connected PLC-5\n");
		printf ("\nusage: abstat <plc5> - where <plc5> is either the IP address or\n");
		printf ("the domain name of the PLC5 you wish to query.\n\n");
		exit (-1);
		}

	comm=abel_attach(argv[1],TRUE);
	if (comm.error != 0)
		{
		printf ("Could not connect.  Check your address and try again.\n");
		exit (-1);
		}
	plcstatus = getstatus (comm, TRUE);
	switch (plcstatus.type)
		{
		case 0xeb:		// Our main friend, the PLC-5 family
		printf ("PLC Mode: %s  ",runmode[plcstatus.mode]);
		if (plcstatus.faulted == TRUE)
 			printf ("Faulted  ");
		if (plcstatus.download == TRUE)
			printf ("Download mode  ");
		if (plcstatus.testedits == TRUE)
			printf ("Testing Edits  ");
		if (plcstatus.edithere == TRUE)
			printf ("Edits present");
		printf ("\n");
		printf ("Processor Type byte: %02X    - PLC 5 Family.\n",plcstatus.type);
		id_found=0;
		for (x=0;x<id_count;x++)
			{
			if (plcstatus.expansion_type == plc_index[x])
				{
				printf ("PLC type: %s",plc_id[x]);
				id_found = 1;
				}
			}
		if (id_found==0)
			printf ("PLC type: UNKNOWN - byte = %02X",plcstatus.expansion_type);
		printf ("   Revision %c Series %c\n",plcstatus.rev, plcstatus.series);
		printf ("PLC Memory size (bytes) = %ld   (words) = %ld\n",plcstatus.memsize,plcstatus.memsize/2);
		printf ("Processor DH+ address: %d\n",plcstatus.dhaddr);
		if (plcstatus.io_addr == 0xfd)
			printf ("Processor is a Remote IO Scanner\n");
		if (plcstatus.io_addr != 0xfd)
			printf ("Processor is a Remote IO Adaptor, Address %d\n",plcstatus.io_addr);
		printf ("IO Config: ");
		if ((plcstatus.io_conf & 1) != 1)
			printf ("Double Density  ");
		if ((plcstatus.io_conf & 2) == 2)
			printf ("Adaptor Mode  ");
		if ((plcstatus.io_conf & 4) == 4)
			printf ("Top half  ");
		if ((plcstatus.io_conf & 32) == 32)
			printf ("Half Rack  ");
		if ((plcstatus.io_conf & 64) == 64)
			printf ("DH+ = 115k");
		printf ("\n");
		printf ("%d Data table files,  %d Program files\n",plcstatus.tablefiles,plcstatus.progfiles);
		printf ("IO Forcing status: ");
		if (plcstatus.forces_on == TRUE)
			printf ("Forces Active  ");
		if (plcstatus.forces_on != TRUE)
			printf ("Forces Not Active  ");
		if (plcstatus.forces_here == TRUE)
			printf ("Force Data Present");
		if (plcstatus.forces_here != TRUE)
			printf ("No Force Data Present");
		printf ("\n");
		if (plcstatus.memprot == 0)
			printf ("Memory is not protected\n");
		if (plcstatus.memprot != 0)
			printf ("Memory is protected\n");
		if (plcstatus.mem_bad != TRUE)
			printf ("Memory checks out OK\n");
		if (plcstatus.mem_bad == TRUE)
			printf ("Memory is Faulted\n");
		if (plcstatus.debug == 0)
			printf ("PLC is not in DEBUG mode\n");
		if (plcstatus.debug != 0)
			printf ("PLC is in DEBUG mode\n");
		break;
	case 0xde:			// Pyramid
		printf ("Processor Type byte: %02X    - Pyramid Integrator.\n",plcstatus.type);
		printf ("Processor ID Byte: %02X\n",plcstatus.expansion_type);
		printf ("PLC Mode: %s  ",runmode[plcstatus.mode]);
		if (plcstatus.faulted == TRUE)
 			printf ("Faulted  ");
		if (plcstatus.download == TRUE)
			printf ("Download mode  ");
		if (plcstatus.testedits == TRUE)
			printf ("Testing Edits  ");
		if (plcstatus.edithere == TRUE)
			printf ("Edits present");
		printf ("\n");
		if (plcstatus.forces_on == TRUE)
			printf ("Forces Active  ");
		if (plcstatus.forces_on != TRUE)
			printf ("Forces not Active  ");
		if (plcstatus.offline == TRUE)
			printf ("PLC Offline  ");
		if (plcstatus.offline != TRUE)
			printf ("PLC Online  ");
		if (plcstatus.backup_active == TRUE)
			printf ("HotBackup Active  ");
		if (plcstatus.partner_active == TRUE)
			printf ("Partner System Active  ");
		if (plcstatus.edit_resource_allocated == TRUE)
			printf ("Edit Resource Allocated\n");
		if (plcstatus.edit_resource_allocated != TRUE)
			printf ("Edit Resource not allocated\n");
		if (plcstatus.outputs_reset == TRUE)
			printf ("Outputs Reset  ");
		if (plcstatus.mem_bad == TRUE)
			printf ("Memory Faulted  ");
		if (plcstatus.mem_bad != TRUE)
			printf (" Memory checks OK  ");
		if (plcstatus.sfc_forces_active == TRUE)
			printf ("SFC Forces Active  ");
		printf ("\n");
		printf ("PLC Station Address: %d\n",plcstatus.dhaddr);
		printf ("Program Sequence Counter: %d\n",plcstatus.prog_change_count);
		printf ("Data Change Counter: %d\n",plcstatus.data_change_count);
		printf ("User Data Change Counter: %d\n",plcstatus.user_data_change_count);
		break;
	case 0xee:			//SLC-500
		printf ("Processor Type byte: %02X    - SLC-500 Family.\n",plcstatus.type);
		printf ("Processor ID Byte: %02X\n",plcstatus.expansion_type);
		printf ("PLC Mode: %s  ",slcmodes[plcstatus.mode]);
		printf ("PLC Interface type: %02X\n",plcstatus.interface);
		printf ("Revision %c Series %c\n",plcstatus.rev, plcstatus.series);
		printf ("PLC ASCII name: %s\n",plcstatus.name);
		if (plcstatus.error_word != 0)
			printf ("Error word contains %d\n",plcstatus.error_word);
		if (plcstatus.forces_here == TRUE)
			printf ("Forces Present  ");
		if (plcstatus.forces_here != TRUE)
			printf ("Forces Not Present  ");
		if (plcstatus.forces_on == TRUE)
			printf ("Forces ACTIVE\n");
		if (plcstatus.forces_on != TRUE)
			printf ("Forces NOT active\n");
		if (plcstatus.comm_active == TRUE)
			printf ("Communications Channel Active.\n");
		if (plcstatus.protection_power_loss == TRUE)
			printf ("Protection Power Lost\n");
		if (plcstatus.startup_protect_fault == TRUE)
			printf ("Startup Protection Fault\n");
		if (plcstatus.load_rom_on_error == TRUE)
			printf ("Load EPROM on PLC error\n");
		if (plcstatus.load_rom_always == TRUE)
			printf ("Always load EPROM\n");
		if (plcstatus.load_rom_and_run == TRUE)
			printf ("Load EPROM and run\n");
		if (plcstatus.plc_halted == TRUE)
			printf ("Major Error - PLC Halted\n");
		if (plcstatus.locked == TRUE)
			printf ("PLC Locked\n");
		if (plcstatus.first_scan == TRUE)
			printf ("PLC in First Scan\n");
		printf ("PLC has %dk RAM in it\n",(short)plcstatus.memsize);
		if (plcstatus.mem_bad == TRUE)
			printf ("Memory is corrupted\n");
		if (plcstatus.program_owner == 0x3f)
			printf ("Program has no owner.\n");
		if (plcstatus.program_owner != 0x3f)
			printf ("Node %d is the current program owner.\n",plcstatus.program_owner);				
		break;
	}

		
		
		
		

	


	close (comm.handle);
	exit (0);
}
