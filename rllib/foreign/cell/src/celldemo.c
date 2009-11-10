
/* Revised 5/9/2002 WRH  Different programming style, free all malloc memory */

#include "libcell.h"


void helpme(void)
{
  printf("Demonstration program for ControlLogix Communications\n\n");
  printf
    ("CELL is a communications library that allows you to communicate with\n");
  printf("an Allen Bradley ControlLogix PLC from a Linux/Unix PC.\n");
  printf
    ("For more information regarding CELL, please visit the CELL website:\n");
  printf("http://www.rongage.org/cell\n\n");
  printf("celldemo [-d -q] hostname\n\n");
  printf("-q	  = quiet mode.  Print almost nothing.\n");
  printf
    ("-d	  = Increase debug mode.  Each -d increments debug mode value.\n");
  printf("	    Mode 0 - (default) Print no debug information.\n");
  printf
    ("	    Mode 1 - Print ControlLogix Values as they are discovered.\n");
  printf("	    Mode 2 - Print entry and exit info for each routine.\n");
  printf
    ("	    Mode 3 - Print data packet as sent and received from the CL.\n");
  printf("	    Mode 4 - Print data packet building inforation.\n");
  printf("	    A higher debug mode includes all lower modes.\n\n");
  printf("hostname = FQDN or IP address of ControlLogix PLC to talk to.\n\n");

}



int main(int argc, char **argv)
{
  _comm_header comm;
  _path path;
  _services services;
  _backplane_data backplane;
  _rack rack;
  _tag_data configtags, detailtags;
  _tag_data *program_tags[64];
  _prog_list progs;

  int x, y, z, z1, debug, program_count, quiet, junk;
  int c1 = 0, c68 = 0, c69 = 0;

  program_count = 0;
  debug = 0;
  quiet = FALSE;
  if (argc == 1)
  {
    helpme();
    exit(1);
  }
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


  if (argv[optind] == NULL)
  {
    printf("Can not connect - need a host name to connect to.\n");
    exit(-1);
  }

  /* Clear memory structures */

  memset(&services, 0, sizeof(_services));
  memset(&comm, 0, sizeof(_comm_header));
  memset(&backplane, 0, sizeof(_backplane_data));
  memset(&rack, 0, sizeof(_rack));
  memset(&configtags, 0, sizeof(_tag_data));
  memset(&detailtags, 0, sizeof(_tag_data));
  memset(&progs, 0, sizeof(_prog_list));

  memset(&program_tags,0, sizeof(program_tags));


  /* Connect */

  comm.hostname = argv[optind];
  if (quiet == FALSE)
    printf("Connecting to host %s\n", comm.hostname);
  establish_connection(&comm, &services, debug);
  if (comm.error != OK)
  {
    printf("An Error occured while connecting to host %s.\n", comm.hostname);
    exit(-1);
  }


  if (quiet == FALSE)
    printf("Got Session_ID = %ld\n", comm.session_id);
  path.device1 = -1;
  path.device2 = -1;
  path.device3 = -1;
  path.device4 = -1;
  path.device5 = -1;
  path.device6 = -1;
  path.device7 = -1;
  path.device8 = -1;

  if (get_backplane_data(&comm, &backplane, &rack, &path, debug))
  {
    cell_perror(NULL,debug);
    goto fail_exit;
  }

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "RX_Bad_m :%02X     ", backplane.rx_bad_m);
    dprint(DEBUG_VALUES, "ERR+Threshold: %02X     ",
	   backplane.err_threshold);
    dprint(DEBUG_VALUES, "RX_Bad_CRC :%02X    ", backplane.rx_bad_crc);
    dprint(DEBUG_VALUES, "RX_Bus_Timeout :%02X\n", backplane.rx_bus_timeout);
    dprint(DEBUG_VALUES, "TX_Bad_CRC :%02X   ", backplane.tx_bad_crc);
    dprint(DEBUG_VALUES, "TX_Bus_Timeout :%02X   ",
	   backplane.tx_bus_timeout);
    dprint(DEBUG_VALUES, "TX_Retry :%02X\n", backplane.tx_retry);
    dprint(DEBUG_VALUES, "Status :%02X    ", backplane.status);
    dprint(DEBUG_VALUES, "Address :%04X\n", backplane.address);
    dprint(DEBUG_VALUES, "Major Rev :%02X  ", backplane.rev_major);
    dprint(DEBUG_VALUES, "Minor Rev :%02X  ", backplane.rev_minor);
    dprint(DEBUG_VALUES, "Serial Number :%08lX  ", backplane.serial_number);
    dprint(DEBUG_VALUES, "Backplane size :%d\n", backplane.rack_size);
  }

  if (who(&comm, &rack, NULL, debug))
  {
    cell_perror(NULL,debug);
    goto fail_exit;
  }

  if (debug == 0)
  {
    for (x = 0; x < backplane.rack_size; x++)
    {
      printf("ID = %02X   type = %02X - ", rack.identity[x]->ID,
	     rack.identity[x]->type);
      if (rack.identity[x]->type < 32)
      {
	printf("%s", PROFILE_Names[rack.identity[x]->type]);
      }
      else
      {
	printf("Unknown type.");
      }
      printf("\n");
      printf("Product Code = %02X    Revision = %d.%02d\n",
	     rack.identity[x]->product_code, rack.identity[x]->rev_hi,
	     rack.identity[x]->rev_lo);
      printf("Device Status = %04X    Device serial number - %08lX\n",
	     rack.identity[x]->status, rack.identity[x]->serial);
      printf("---------------------------\n");
    }
  }

  path.device1 = 1;
  path.device2 = rack.cpulocation;
  path.device3 = -1;
  path.device4 = -1;

  if (get_object_config_list(&comm, &path, 0, &configtags, debug))
  {
    cell_perror(NULL,debug);
    goto fail_exit;
  }

  if (get_object_details_list(&comm, &path, 0, &detailtags, debug))
  {
    cell_perror(NULL,debug);
    goto fail_exit;
  }

  if (get_program_list(&comm, &path, &progs, debug))
  {
    cell_perror(NULL,debug);
    goto fail_exit;
  }


  y = detailtags.count;
  if (configtags.count > y)
    y = configtags.count;
  if (y > 10)
    y = 10;
  for (z = 0; z < y; z++)
  {
    if (z < detailtags.count)
    {
      if (get_object_details(&comm, &path, detailtags.tag[z], debug))
      {
        cell_perror(NULL,debug);
        goto fail_exit;
      }

      if (get_object_config(&comm, &path, configtags.tag[z], debug))
      {
        cell_perror("Calling config",debug);
        goto fail_exit;
      }

      switch (detailtags.tag[z]->type & 255)
      {
      case 0x68:
	c68++;
	break;
      case 0x69:
	c69++;
	break;
      default:
	if (read_object_value(&comm, &path, detailtags.tag[z], debug)<0)
        {
          cell_perror("Reading object value 1",debug);
          goto fail_exit;
        }
      }

      if (detailtags.tag[z]->topbase != 0)
	c1++;
      if (z < detailtags.count)
      {
	printf("%08lX - %08lX - %08lX - %04X - %s - %ld - %ld - %ld",
	       detailtags.tag[z]->topbase, detailtags.tag[z]->base,
	       detailtags.tag[z]->linkid, detailtags.tag[z]->type,
	       detailtags.tag[z]->name,
	       detailtags.tag[z]->arraysize1, detailtags.tag[z]->arraysize2,
	       detailtags.tag[z]->arraysize3);
	printf("	");
      }
      else
      {
	printf("0 - 0 - 0       ");
      }
      if (z < configtags.count)
      {
	printf("%08lX - %08lX - %04X\n", configtags.tag[z]->topbase,
	       configtags.tag[z]->linkid, configtags.tag[z]->type);
      }
      else
      {
	printf("0 - 0 - 0\n");
      }

      for (x = 0; x < detailtags.tag[z]->datalen; x++)
	printf("%02X ", detailtags.tag[z]->data[x]);
      printf("\n");


    }
  }
  
  printf("%d objects with topbase != 0.\n", c1);
  printf("%d objects type 0x68, %d objects type 0x69.\n", c68, c69);
  printf("configtags count = %d    detailtags count = %d\n",
	 configtags.count, detailtags.count);

  printf("Got %d programs...\n", progs.count);
  for (x = 0; x < progs.count; x++)
  {
    program_tags[x] = malloc(sizeof(_tag_data));
    if (program_tags[x] == NULL)
    {
      printf("Could not allocate memory for program tag structure.\n");
      exit(-1);
    }

    if (get_program_details(&comm, &path, progs.prog[x], debug))
    {
      cell_perror(NULL,debug);
      goto fail_exit;
    }

    for (y = 0; y < detailtags.count; y++)
    {
      if (detailtags.tag[y]->linkid == progs.prog[x]->linkid)
      {
	strcpy(progs.prog[x]->name, detailtags.tag[y]->name);
      }
    }
    printf("id for program #%d is %08lX   link ID = %08lX - %s\n", x,
	   progs.prog[x]->base, progs.prog[x]->linkid,
	   progs.prog[x]->name);
  }




  for (x = 0; x < progs.count; x++)
  {
    if (quiet == FALSE)
      printf("Tag list for %s\n", progs.prog[x]->name);
    if (get_object_details_list(&comm, &path, progs.prog[x]->base, program_tags[x],
			    debug))
    {
      cell_perror(NULL,debug);
      goto fail_exit;
    }
    printf("tag count = %02X\n\n", program_tags[x]->count);
    z = program_tags[x]->count;
    if (z > 10)
      z = 10;
    for (y = 0; y < z; y++)
    {
      if (quiet == FALSE)
	printf("%08lX  %08lX    ", program_tags[x]->tag[y]->id,
	       program_tags[x]->tag[y]->base);
      if (get_object_details(&comm, &path, program_tags[x]->tag[y], debug))
      {
        cell_perror(NULL,debug);
        goto fail_exit;
      }
      if (quiet == FALSE)
      {
	printf("type = %04X   Name = %s\n", program_tags[x]->tag[y]->type,
	       program_tags[x]->tag[y]->name);
	printf("arraysize1 = %ld    arraysize2 = %ld    arraysize3 = %ld\n",
	       program_tags[x]->tag[y]->arraysize1,
	       program_tags[x]->tag[y]->arraysize2,
	       program_tags[x]->tag[y]->arraysize3);
      }
    }
    if (quiet == FALSE)
      printf("-----------------------------------\n");
  }
  z = detailtags.count;
  if (z > 10)
    z = 10;

  for (x = 0; x < z; x++)
  {
    junk = detailtags.tag[x]->type & 255;
    if ((junk > 0xbf) || (junk == 0x82) || (junk == 0x83) || (junk == 0x84))
    {
      if (read_object_value(&comm, &path, detailtags.tag[x], debug)<0)
      {
        cell_perror(NULL,debug);
        goto fail_exit;
      }
      if (quiet == FALSE)
      {
	printf("Value(s) for %s: ", detailtags.tag[x]->name);
	for (y = 0; y < detailtags.tag[x]->datalen; y++)
	  printf("%02X ", detailtags.tag[x]->data[y]);
	printf("\n");
      }

    }
  }

  for (x = 0; x < progs.count; x++)
  {
    if (quiet == FALSE)
      printf("Reading %d Data Values for program %s tags...\n",
	     program_tags[x]->count, progs.prog[x]->name);
    z = program_tags[x]->count;
    if (z > 10)
      z = 10;
    for (y = 0; y < z; y++)
    {
      if ((program_tags[x]->tag[y]->type & 255) > 0xbf)
      {
	if (read_object_value(&comm, &path, program_tags[x]->tag[y], debug)<0)
        {
//          cell_perror("Last read object value",debug);
          goto fail_exit;
        }
	if (quiet == FALSE)
	{
	  printf("Value(s) for %s: ", program_tags[x]->tag[y]->name);
	  for (z1 = 0; z < program_tags[x]->tag[y]->datalen; z1++)
	    printf("%02X ", program_tags[x]->tag[y]->data[z1]);
	  printf("\n");
	}
      }
      if (quiet == FALSE)
	printf("---------------------------------\n");
    }
  }

fail_exit:
  close(comm.file_handle);

  /* Release allocated memory for tags, programs, etc... */

  for (x = 0; x < progs.count; x++)
  {
    if (program_tags[x])
    {
      for (y = 0; y<program_tags[x]->count; y++)
         free(program_tags[x]->tag[y]);
      free(program_tags[x]);
    }
  }

  for (y = 0; y < detailtags.count; y++)
  {
    free(detailtags.tag[y]);
  }

  for (y = 0; y < configtags.count; y++)
  {
    free(configtags.tag[y]);
  }

  exit(0);
}
