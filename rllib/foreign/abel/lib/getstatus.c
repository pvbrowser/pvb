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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

extern struct plc5stat getstatus (struct _comm comm1, int debug)
{
struct ethernet_header header, rcvd;
struct pccc_normal df1_2;
struct plc5stat plc5status;
struct custom_pccc custom;
int hlen,x;
long msize;
bzero (&header,sizeof(header));
bzero (&rcvd, sizeof(rcvd));
header.mode = 1;
header.submode = PCCC_CMD;
header.conn = htonl(comm1.commid);
custom.request_id = 0xe87573;
custom.name_id = 0;
memcpy (&header.custom, &custom, _CUSTOM_LEN);
df1_2.src=0;
df1_2.control = 5;
df1_2.dst=0;	
df1_2.lsap = 0;
df1_2.cmd=0x06;
df1_2.sts=0;
df1_2.tns=htons(comm1.tns);
df1_2.fnc=3;
hlen = _ENET_HEADER_LEN + PCCC_DATA_LEN;
memcpy (&header.df1_stuff, &df1_2, PCCC_DATA_LEN);
header.pccc_length = htons(PCCC_DATA_LEN);

rcvd = senddf1 (comm1, header, hlen, debug);

bzero (&plc5status, sizeof(plc5status));

if (rcvd.df1_stuff[4] != (df1_2.cmd + 0x40))
	{
	return plc5status;
	}
if (rcvd.df1_stuff[5] != 0)
	{
	return plc5status;
	}
switch (rcvd.df1_stuff[9])
	{
	case 0xeb:		// plc5
		plc5status.mode = rcvd.df1_stuff[8] & 7;
		plc5status.faulted = (rcvd.df1_stuff[8] & 8);
		plc5status.download = (rcvd.df1_stuff[8] & 16);
		plc5status.testedits = (rcvd.df1_stuff[8] & 64);
		plc5status.edithere = (rcvd.df1_stuff[8] & 128);
		plc5status.type = rcvd.df1_stuff[9];
		plc5status.expansion_type = rcvd.df1_stuff[10];
		plc5status.series = (rcvd.df1_stuff[15] / 16)+ 'A';
		plc5status.rev = (rcvd.df1_stuff[15] & 15)+ 'A';
		msize=(long)rcvd.df1_stuff[11];
		msize=msize+(long)(rcvd.df1_stuff[12]*256l);
		msize=msize+(long)(rcvd.df1_stuff[13]*65536l);
		plc5status.memsize = msize;
		plc5status.dhaddr = rcvd.df1_stuff[16];
		plc5status.io_addr = rcvd.df1_stuff[17];
		plc5status.io_conf = rcvd.df1_stuff[18];
		plc5status.tablefiles = rcvd.df1_stuff[19]+ (rcvd.df1_stuff[20]*256);
		plc5status.progfiles = rcvd.df1_stuff[21]+ (rcvd.df1_stuff[22]*256);
		plc5status.forces_here=(rcvd.df1_stuff[23] & 16);
		plc5status.forces_on=(rcvd.df1_stuff[23] & 1);
		plc5status.memprot = (rcvd.df1_stuff[24] != 0);
		plc5status.mem_bad = FALSE;
		if(rcvd.df1_stuff[25] != 0)
			plc5status.mem_bad = TRUE;
		plc5status.debug = (rcvd.df1_stuff[26] != 0);
		plc5status.hold_file = (rcvd.df1_stuff[27]*256)+rcvd.df1_stuff[28];
		plc5status.hold_element = (rcvd.df1_stuff[29]*256)+rcvd.df1_stuff[30];
		plc5status.edit_time_sec = (rcvd.df1_stuff[31]*256)+rcvd.df1_stuff[32];
		plc5status.edit_time_min = (rcvd.df1_stuff[33]*256)+rcvd.df1_stuff[34];
		plc5status.edit_time_hour= (rcvd.df1_stuff[35]*256)+rcvd.df1_stuff[36];
		plc5status.edit_time_day = (rcvd.df1_stuff[37]*256)+rcvd.df1_stuff[38];
		plc5status.edit_time_month=(rcvd.df1_stuff[39]*256)+rcvd.df1_stuff[40];
		plc5status.edit_time_year= (rcvd.df1_stuff[41]*256)+rcvd.df1_stuff[42];
		plc5status.cmdport = rcvd.df1_stuff[43];
				
		return plc5status;
	case 0xde:		// Pyramid Integrator
		plc5status.mode = rcvd.df1_stuff[8] & 7;
		plc5status.faulted = (rcvd.df1_stuff[8] & 8);
		plc5status.download = (rcvd.df1_stuff[8] & 16);
		plc5status.offline = (rcvd.df1_stuff[8] & 32);
		plc5status.testedits = (rcvd.df1_stuff[8] & 64);
		plc5status.edithere = (rcvd.df1_stuff[8] & 128);
		plc5status.type = rcvd.df1_stuff[9];
		plc5status.expansion_type = rcvd.df1_stuff[10];
		plc5status.forces_on = (rcvd.df1_stuff[11] & 1);
		plc5status.backup_active = (rcvd.df1_stuff[11] & 2);
		plc5status.partner_active = (rcvd.df1_stuff[11] & 4);
		plc5status.edit_resource_allocated = (rcvd.df1_stuff[11] & 8);
		plc5status.outputs_reset = (rcvd.df1_stuff[11] & 16);
		plc5status.mem_bad = (rcvd.df1_stuff[11] & 32);
		plc5status.sfc_forces_active = (rcvd.df1_stuff[11] & 64);
		plc5status.edit_resource_allocated = (rcvd.df1_stuff[11] & 128);
		plc5status.dhaddr = rcvd.df1_stuff[12];
		return plc5status;
	case 0xee:		// SLC 5/05
		plc5status.testedits = (rcvd.df1_stuff[8] & 64);
		plc5status.edithere = (rcvd.df1_stuff[8] & 128);
		plc5status.type = rcvd.df1_stuff[9];
		plc5status.interface = rcvd.df1_stuff[10];
		plc5status.expansion_type = rcvd.df1_stuff[11];
		plc5status.series = (rcvd.df1_stuff[12] & 15)+'A';
		plc5status.rev = (rcvd.df1_stuff[12] / 16)+'A';
		for (x=13;x<24;x++)
			plc5status.name[x-13] = rcvd.df1_stuff[x];
		plc5status.error_word = (rcvd.df1_stuff[25]*256) + rcvd.df1_stuff[24];
		plc5status.mode = (rcvd.df1_stuff[26] & 31);
		plc5status.forces_on = (rcvd.df1_stuff[26] & 32);
		plc5status.forces_here = (rcvd.df1_stuff[26] & 64);
		plc5status.comm_active = (rcvd.df1_stuff[26] & 128);
		plc5status.protection_power_loss = (rcvd.df1_stuff[27] & 1);
		plc5status.startup_protect_fault = (rcvd.df1_stuff[27] & 2);
		plc5status.load_rom_on_error = (rcvd.df1_stuff[27] & 4);
		plc5status.load_rom_always = (rcvd.df1_stuff[27] & 8);
		plc5status.load_rom_and_run = (rcvd.df1_stuff[27] & 16);
		plc5status.plc_halted = (rcvd.df1_stuff[27] & 32);
		plc5status.locked = (rcvd.df1_stuff[27] & 64);
		plc5status.first_scan = (rcvd.df1_stuff[27] & 128);
		plc5status.program_id = (rcvd.df1_stuff[29]*256) + rcvd.df1_stuff[28];
		plc5status.memsize = rcvd.df1_stuff[30];
		plc5status.mem_bad = (rcvd.df1_stuff[31] & 1);
		plc5status.program_owner = rcvd.df1_stuff[31]/4;
		return plc5status;
	}
return plc5status;
}	
