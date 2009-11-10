#include "libcell.h"

int main(int argc, char **argv)
{
_comm_header *comm;
_tag_detail *tag;
_path *path;
_rack *rack;
_services *services;
//_backplane_data *backplane;
int result,x;

result = 0;
if ((argc < 3) || (argc > 4))
	{
	printf ("readtag (processor) tagname {program name} \n\n");
	printf ("Allows you to read a specific tag from the specified ControlLogix PLC.\n\n");
	printf ("(processor) = ip address or domain name of target PLC.\n");
	printf ("tagname = the tag you wish to read from the PLC.\n");
	printf ("{program name} = optional program name to find the tag in.\n\n");
	exit(-1);
	}
	
// Note that this is quick and dirty - no error checking...

tag = malloc(sizeof(_tag_detail));
memset(tag,0,sizeof(_tag_detail));

rack = malloc(sizeof(_rack));
memset (rack,0,sizeof(_rack));

//backplane = malloc(sizeof(_backplane_data));
//memset (backplane,0,sizeof(_backplane_data));

path = malloc(sizeof(_path));
memset (path,0,sizeof(_path));

services = malloc(sizeof(_services));
memset (services,0,sizeof(_services));

comm = malloc(sizeof(_comm_header));
memset (comm,0,sizeof(_comm_header));

printf ("setting plc name %d %s.\n",strlen(argv[1]), argv[1]);
comm->hostname = argv[1];
printf ("attaching to plc\n");

establish_connection (comm, services, 0);
if (comm->error != 0)
	{
	printf ("Could not attach to %s\n",argv[1]);
	exit(-1);
	}
path->device1 = -1;
path->device2 = -1;
path->device3 = -1;
path->device4 = -1;
path->device5 = -1;
path->device6 = -1;
path->device7 = -1;
path->device8 = -1;

//get_backplane_data(comm, backplane, rack, path, 0);
printf ("polling PLC rack layout\n");
who(comm, rack, NULL, 0);
path->device1 =1;
path->device2 = rack->cpulocation;
printf ("reading tag\n");
if (argc == 3)
	result = read_tag(comm, path, NULL, argv[2], tag, 3);
if (argc == 4)
	result = read_tag(comm, path, argv[3], argv[2], tag, 0);

if (result != 0)
	{
	printf ("reading tag %s failed - does it exist?\n\n",argv[2]);
	exit(-1);
	}
for (x=0; x<tag->datalen; x++)
	printf ("%02X ",tag->data[x]);
printf ("\n\n");
exit(0);
}

