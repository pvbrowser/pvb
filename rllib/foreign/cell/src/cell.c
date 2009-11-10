#include "libcell.h"

void helpme(void)
{
  printf("Test program for ControlLogix Communications\n\n");
  printf("test [-d -q] hostname\n\n");
  printf("-q	  = quiet mode.  Print almost nothing.\n");
  printf("-d	  = Increase debug mode.  Each -d increments debug mode value.\n");
  printf("	    Mode 0 - (default) Print no debug information.\n");
  printf("	    Mode 1 - Print ControlLogix Values as they are discovered.\n");
  printf("	    Mode 2 - Print entry and exit info for each routine.\n");
  printf("	    Mode 3 - Print data packet as sent and received from the CL.\n");
  printf("	    Mode 4 - Print data packet building inforation.\n");
  printf("	    A higher debug mode includes all lower modes.\n\n");
  printf("hostname = FQDN or IP address of ControlLogix PLC to talk to.\n\n");

}




int main(int argc, char **argv)
{
  _comm_header *comm;
  _path *path;
  _services *services;
  _backplane_data *backplane;
  _rack *rack;
  _tag_data *configtags, *detailtags;
  _tag_data *program_tags[64];
  _tag_list *list;
  _prog_list *progs;
  _struct_list *structs;
  int x, y, z, debug, program_count, quiet, result;
  int c1 = 0, c68 = 0, c69 = 0;
#ifdef WIN32
  int optind = 1;
  debug = DEBUG_DATA;
#endif

#ifndef WIN32
  debug = 0;
#endif

  program_count = 0;
  quiet = FALSE;
  if (argc == 1)
  {
    helpme();
    exit(1);
  }
#ifndef WIN32
  while ((x = getopt(argc, argv, "dhq?")) != -1)
  {
    switch (x)
    {
    case 'd':
      debug++;
      break;
    case 'q':
      quiet = TRUE;
      break;
    case 'h':
    case '?':
      helpme();
      exit(1);
    }
  }

#endif

  if (argv[optind] == NULL)
  {
    printf("Can not connect - need a host name to connect to.\n");
    exit(-1);
  }

  rack = malloc(sizeof(_rack));
  if (rack == NULL)
  {
    printf("Could not allocate memory for rack structure.\n");
    exit(-1);
  }

  comm = malloc(sizeof(_comm_header));
  if (comm == NULL)
  {
    printf("Could not allocate memory for comm header.\n");
    exit(-1);
  }
  services = malloc(sizeof(_services));
  if (services == NULL)
  {
    printf("Could not allocate memory for services structure.\n");
    free(comm);
    exit(-1);
  }

  backplane = malloc(sizeof(_backplane_data));
  if (backplane == NULL)
  {
    printf("Could not allocate memory for backplane structure.\n");
    free(comm);
    free(services);
    exit(-1);
  }
  path = malloc(sizeof(_path));
  if (path == NULL)
  {
    printf("Could not allocate memory for path structure.\n");
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }
  configtags = malloc(sizeof(_tag_data));
  if (configtags == NULL)
  {
    printf("Could not allocate memory for configtags structure.\n");
    free(path);
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }
  detailtags = malloc(sizeof(_tag_data));
  if (detailtags == NULL)
  {
    printf("Could not allocate memory for detailtags structure.\n");
    free(path);
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }
  progs = malloc(sizeof(_prog_list));
  if (progs == NULL)
  {
    printf("Could not allocate memory for program list structure.\n");
    free(detailtags);
    free(path);
    free(backplane);
    free(comm);
    free(services);
    exit(-1);
  }
  structs = malloc(sizeof(_struct_list));
  if (structs == NULL)
  {
    printf("Could not allocate memory for structure list.\n");
    exit(-1);
  }
  list = malloc(sizeof(_tag_list));
  if (list == NULL)
  {
    printf ("Could not allocate memory for tag list.\n");
    exit(-1);
  }



  memset(services, 0, sizeof(_services));
  memset(comm, 0, sizeof(_comm_header));
  memset(backplane, 0, sizeof(_backplane_data));
  memset(rack, 0, sizeof(_rack));
  memset(configtags, 0, sizeof(_tag_data));
  memset(detailtags, 0, sizeof(_tag_data));
  memset(progs, 0, sizeof(_prog_list));
  memset(structs, 0, sizeof(_struct_list));
  memset(list, 0, sizeof(_tag_list));

  comm->hostname = argv[optind];
  if (quiet == FALSE)
    printf("Connecting to host %s\n", comm->hostname);
  establish_connection(comm, services, debug);
  if (comm->error != OK)
  {
    printf("An Error occured while connecting to host %s.\n", comm->hostname);
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

  if (quiet == FALSE)
    printf("Got Session_ID = %ld\n", comm->session_id);
  get_backplane_data(comm, backplane, rack, path, debug);
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "RX_Bad_m :%02X     ", backplane->rx_bad_m);
    dprint(DEBUG_VALUES, "ERR+Threshold: %02X     ",
	   backplane->err_threshold);
    dprint(DEBUG_VALUES, "RX_Bad_CRC :%02X    ", backplane->rx_bad_crc);
    dprint(DEBUG_VALUES, "RX_Bus_Timeout :%02X\n", backplane->rx_bus_timeout);
    dprint(DEBUG_VALUES, "TX_Bad_CRC :%02X   ", backplane->tx_bad_crc);
    dprint(DEBUG_VALUES, "TX_Bus_Timeout :%02X   ",
	   backplane->tx_bus_timeout);
    dprint(DEBUG_VALUES, "TX_Retry :%02X\n", backplane->tx_retry);
    dprint(DEBUG_VALUES, "Status :%02X    ", backplane->status);
    dprint(DEBUG_VALUES, "Address :%04X\n", backplane->address);
    dprint(DEBUG_VALUES, "Major Rev :%02X  ", backplane->rev_major);
    dprint(DEBUG_VALUES, "Minor Rev :%02X  ", backplane->rev_minor);
    dprint(DEBUG_VALUES, "Serial Number :%08lX  ", backplane->serial_number);
    dprint(DEBUG_VALUES, "Backplane size :%d\n", backplane->rack_size);
  }
  who(comm, rack, NULL, debug);
  path->device1 = 1;
  path->device2 = rack->cpulocation;
  path->device3 = -1;
  path->device4 = -1;
  path->device5 = -1;
  path->device6 = -1;
  path->device7 = -1;
  path->device8 = -1;

  get_object_config_list(comm, path, 0, configtags, debug);
  get_object_details_list(comm, path, 0, detailtags, debug);
  get_program_list(comm, path, progs, debug);
/*  get_struct_list(comm, path, structs, debug);


printf ("Got %d structs in the program.\n",structs->count);
for (x=0;x<structs->count; x++)
	{
	printf ("Struct #%02d, type id = %08lX\n",x,structs->base[x]->base);
	get_struct_config (comm, path, structs->base[x], debug);
	get_struct_details (comm, path, structs->base[x], debug);
	printf ("Element count = %d    Detail size = %d\n",structs->base[x]->count, structs->base[x]->detailsize);
	printf ("Element link-id = %04X\n",structs->base[x]->linkid);
	printf ("Structure base name = %s\n",structs->base[x]->name);
	for (y=0;y<structs->base[x]->count;y++)
		{
		printf ("Element #%d, type = %04X, arraysize = %d name = %s\n",y,structs->base[x]->data[y]->type, structs->base[x]->data[y]->arraysize, structs->base[x]->data[y]->name);
		}
	printf ("----------------------------------------\n");
	}

exit;
*/

  y = detailtags->count;
  if (configtags->count > y)
    y = configtags->count;

  printf("Reading tags from ControlLogix...\n");
  for (z = 0; z < y; z++)
  {
    if (z < detailtags->count)
      get_object_details(comm, path, detailtags->tag[z], debug);
    if (z < configtags->count)
      get_object_config(comm, path, configtags->tag[z], debug);
    printf
      ("%08lX - %08lX - %08lX - %08lX - %04X - %04X - %04X - %s - %ld - %ld - %ld - %ld\n",
       detailtags->tag[z]->topbase, detailtags->tag[z]->base,
       detailtags->tag[z]->id, detailtags->tag[z]->linkid,
       detailtags->tag[z]->size, detailtags->tag[z]->type,
       detailtags->tag[z]->alias_type, detailtags->tag[z]->name,
       detailtags->tag[z]->alias_linkid - detailtags->tag[z]->linkid,
       detailtags->tag[z]->arraysize1, detailtags->tag[z]->arraysize2,
       detailtags->tag[z]->arraysize3);

  }
  printf("Checking for aliases...\n");

  for (z = 0; z < detailtags->count; z++)
    aliascheck(detailtags->tag[z], NULL, detailtags, DEBUG_VALUES);

  for (z = 0; z < y; z++)
  {
    if (z < detailtags->count)
    {
    switch (detailtags->tag[z]->type & 255)
      {
      case 0x68:
      case 0x69:
	break;
      default:
	read_object_value(comm, path, detailtags->tag[z], debug);
      }

      if (detailtags->tag[z]->topbase != 0)
	c1++;
      if (z < detailtags->count)
      {
	printf
	  ("%08lX - %08lX - %08lX - %08lX - %08lX - %08lX - %04lX - %04X - %04X - %s - %ld - %ld - %ld - %ld\n",
	   detailtags->tag[z]->topbase, detailtags->tag[z]->base,
	   detailtags->tag[z]->id, detailtags->tag[z]->linkid,
	   detailtags->tag[z]->alias_topbase, detailtags->tag[z]->alias_base,
	   detailtags->tag[z]->alias_id, detailtags->tag[z]->type,
	   detailtags->tag[z]->alias_type, detailtags->tag[z]->name,
	   detailtags->tag[z]->alias_linkid - detailtags->tag[z]->linkid,
	   detailtags->tag[z]->arraysize1, detailtags->tag[z]->arraysize2,
	   detailtags->tag[z]->arraysize3);
      }
      if (z < configtags->count)
      {
	printf("%08lX - %08lX - %04X\n", configtags->tag[z]->topbase,
	       configtags->tag[z]->linkid, configtags->tag[z]->type);
      }

/*

		if (strncmp ( detailtags->tag[z]->name, "IntArray",strlen(detailtags->tag[z]->name)))
			{
			read_object_value(comm, path, detailtags->tag[z], debug);
			printf ("Old:\n");
			for (x=0;x<detailtags->tag[z]->datalen;x++)
				printf ("%02X ",detailtags->tag[z]->data[x]);
			printf ("\n"); 
			for (x=0;x<detailtags->tag[z]->datalen; x++)
				detailtags->tag[z]->data[x]++;
			write_object_value(comm, path, detailtags->tag[z], 4);
			read_object_value(comm, path, detailtags->tag[z], debug);
			printf ("New:\n");
			for (x=0;x<detailtags->tag[z]->datalen;x++)
				printf ("%02X ",detailtags->tag[z]->data[x]);
			printf ("\n"); 
			}			
*/

    }
  }
/*
  for (z = 0; z < y; z+=5)
  {
    if (z < detailtags->count)
    {
    switch (detailtags->tag[z]->type & 255)
      {
      case 0x68:
      case 0x69:
	break;
      default:
	list->count = 1;
	list->tag[0] = detailtags->tag[z];
	if (z+1 < y)
		list->tag[list->count++] = detailtags->tag[z+1];
	if (z+2 < y)
		list->tag[list->count++] = detailtags->tag[z+2];
	if (z+3 < y)
		list->tag[list->count++] = detailtags->tag[z+3];
	if (z+4 < y)
		list->tag[list->count++] = detailtags->tag[z+4];
	read_multi_object_value(comm, path, list, debug);
      }

    }
  }

*/
for (z=0; z<detailtags->count; z++)
	{
	printf ("%s : ",detailtags->tag[z]->name);
	for (x=0;x<detailtags->tag[z]->datalen;x++)
		printf ("%02X ",detailtags->tag[z]->data[x]);
	printf ("\n"); 
	}


//exit(0);
  printf("%d objects with topbase != 0.\n", c1);
  printf("%d objects type 0x68, %d objects type 0x69.\n", c68, c69);
  printf("configtags count = %d    detailtags count = %d\n",
	 configtags->count, detailtags->count);
  printf("Got %d programs...\n", progs->count);

  for (x = 0; x < progs->count; x++)
  {
    program_tags[x] = malloc(sizeof(_tag_data));



    if (program_tags[x] == NULL)
    {
      printf("Could not allocate memory for program tag structure.\n");
      exit(-1);
    }

    get_program_details(comm, path, progs->prog[x], debug);
    for (y = 0; y < detailtags->count; y++)
    {
      if (detailtags->tag[y]->linkid == progs->prog[x]->linkid)
      {
	strcpy(progs->prog[x]->name, detailtags->tag[y]->name);
      }
    }
    printf("id for program #%d is %08lX   link ID = %08lX - %s\n", x,
	   progs->prog[x]->base, progs->prog[x]->linkid,
	   progs->prog[x]->name);
  }




  for (x = 0; x < progs->count; x++)
  {
    if (quiet == FALSE)
      printf("Tag list for %s\n", progs->prog[x]->name);
    get_object_details_list(comm, path, progs->prog[x]->base, program_tags[x],
			    debug);
    printf("tag count = %d\n\n", program_tags[x]->count);

    for (y = 0; y < program_tags[x]->count; y++)
    {
      get_object_details(comm, path, program_tags[x]->tag[y], debug);
/*      aliascheck(program_tags[x]->tag[y], program_tags[x], detailtags,
		 DEBUG_VALUES);
      if (quiet == FALSE)
	printf("%08lX  %08lX  %08lX  %08lX   ",
	       program_tags[x]->tag[y]->topbase,
	       program_tags[x]->tag[y]->base, program_tags[x]->tag[y]->id,
	       program_tags[x]->tag[y]->linkid);
      if (quiet == FALSE)
      {
	printf("type = %04X   Name = %s\n", program_tags[x]->tag[y]->type,
	       program_tags[x]->tag[y]->name);
	printf("arraysize1 = %ld    arraysize2 = %ld    arraysize3 = %ld\n",
	       program_tags[x]->tag[y]->arraysize1,
	       program_tags[x]->tag[y]->arraysize2,
	       program_tags[x]->tag[y]->arraysize3);
      }
*/
    }
    if (quiet == FALSE)
      printf("-----------------------------------\n");
  }

  for (x = 0; x < detailtags->count; x++)
  {
    if ((detailtags->tag[x]->size != 0)
	|| (detailtags->tag[x]->alias_size != 0))
    {
      result = read_object_value(comm, path, detailtags->tag[x], debug);
      if (result != 0)
      {
	printf("Got non-zero return from read_object_value - %02X\n", result);
	printf
	  ("%08lX - %08lX - %08lX - %08lX - %08lX - %08lX - %08lX - %04X - %s - %ld - %ld - %ld\n",
	   detailtags->tag[x]->topbase, detailtags->tag[x]->base,
	   detailtags->tag[x]->id, detailtags->tag[x]->linkid,
	   detailtags->tag[x]->alias_topbase, detailtags->tag[x]->alias_base,
	   detailtags->tag[x]->alias_id, detailtags->tag[x]->type,
	   detailtags->tag[x]->name, detailtags->tag[x]->arraysize1,
	   detailtags->tag[x]->arraysize2, detailtags->tag[x]->arraysize3);
      }

      if (quiet == FALSE)
      {
	printf("Value(s) for %s: ", detailtags->tag[x]->name);
	for (y = 0; y < detailtags->tag[x]->size; y++)
	  printf("%02X ", detailtags->tag[x]->data[y]);
	printf("\n");
      }

    }
  }

  for (x = 0; x < progs->count; x++)
  {
    if (quiet == FALSE)
      printf("Reading %d Data Values for program %s tags...\n",
	     program_tags[x]->count, progs->prog[x]->name);
    for (y = 0; y < program_tags[x]->count; y++)
    {
      if ((program_tags[x]->tag[y]->type & 255) > 0xbf)
      {
	read_object_value(comm, path, program_tags[x]->tag[y], debug);
	if (quiet == FALSE)
	{
	  printf("Value(s) for %s: ", program_tags[x]->tag[y]->name);
	  for (z = 0; z < program_tags[x]->tag[y]->datalen; z++)
	    printf("%02X ", program_tags[x]->tag[y]->data[z]);
	  printf("\n");
	}
      }
      if (quiet == FALSE)
	printf("---------------------------------\n");
    }
  }
  closesocket(comm->file_handle);
  exit(0);
}
