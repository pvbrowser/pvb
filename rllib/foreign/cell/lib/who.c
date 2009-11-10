
/* Revised 5/6/02 William Hays - CTI 

   5/7/2002 WRH  Limited rack identities to CELL_MAX_RACKID
*/

#include "libcell.h"

int who(_comm_header * comm, _rack * rack, _path * basepath, int debug)
{
  _port *port;
  _path *path;
  int x;

  dprint(DEBUG_TRACE, "who.c entered.\n");
  port = malloc(sizeof(_port));
  if (port == NULL)
  {
    CELLERROR(1,"Could not allocate memory for port structure");
    return -1;
  }
  path = malloc(sizeof(_path));
  if (path == NULL)
  {
    CELLERROR(2,"Could not allocate memory for path structure");
    free(port);
    return -1;
  }

  if (rack == NULL)
  {
    CELLERROR(3,"Rack structure not allocated");
    free(port);
    free(path);
    return -1;
  }

  if (rack->size == 0)
  {
    CELLERROR(4,"Rack size not set");
    free(port);
    free(path);
    return -1;
  }

  if (rack->size >= CELL_MAX_RACKID)
    {
      CELLERROR(20,"Exceeded maximum CELL_MAX_RACKID array size");
      free(port);
      free(path);
      return -1;
    }


  for (x = 0; x < rack->size; x++)
  {
    if (rack->identity[x] == NULL)
    {
      dprint(DEBUG_BUILD, "Allocating memory for identity object %d.\n", x);
      rack->identity[x] = malloc(sizeof(_identity));
      if (rack->identity[x] == NULL)
      {
	CELLERROR(5,"Could not allocate memory for identity object");
	free(port);
	free(path);
	return -1;
      }
    }
    memset(rack->identity[x], 0, sizeof(_identity));
    strcpy(rack->identity[x]->name, "--Empty Slot--");
    rack->identity[x]->namelen = strlen(rack->identity[x]->name);
  }

/* For more information about paths, please see the docs/paths.txt file */

  if (basepath == NULL)
    {
    path->device1 = -1;
    path->device2 = -1;
    path->device3 = -1;
    path->device4 = -1;
    path->device5 = -1;
    path->device6 = -1;
    path->device7 = -1;
    path->device8 = -1;
    }
  else
    {
    memcpy (path, basepath, sizeof(_path));
    }

  dprint(DEBUG_VALUES, "Got slot number %d for communications card.\n", rack->slot);
  get_device_data(comm, path, rack->identity[rack->slot], debug);
  for (x = 0; x < rack->size; x++)
  {
    char temp[33];

    memset (&temp,0,33);
    if (rack->identity[x]->ID == 0)
    {
      dprint(DEBUG_VALUES, "Getting slot %d data...\n", x);
      if (basepath == NULL)
        {
        path->device1 = 1;
        path->device2 = x;
        path->device3 = -1;

        path->device4 = -1;
        path->device5 = -1;
        path->device6 = -1;
        path->device7 = -1;
        path->device8 = -1;
        }
      else
        {
        memcpy (path, basepath, sizeof(_path));
        if ((basepath->device6 != -1) && (basepath->device7 == -1))
          {
          path->device7 = 1;
          path->device8 = x;
          }
        if ((basepath->device5 != -1) && (basepath->device6 == -1))
          {
          path->device6 = 1;
          path->device7 = x;
          }
        if ((basepath->device4 != -1) && (basepath->device5 == -1))
          {
          path->device5 = 1;
          path->device6 = x;
          }
        if ((basepath->device3 != -1) && (basepath->device4 == -1))
          {
          path->device4 = 1;
          path->device5 = x;
          }
        if ((basepath->device2 != -1) && (basepath->device3 == -1))
          {
          path->device3 = 1;
          path->device4 = x;
          }
        if ((basepath->device1 != -1) && (basepath->device2 == -1))
          {
          path->device2 = 1;
          path->device3 = x;
          }
        }
        
      get_device_data(comm, path, rack->identity[x], debug);
    }
    /* This is important to note:  The identity->name  field is fixed at 32 characters and
    and there is no guarentee that it is null terminated.  To use it safely, you must copy
    it to a null terminated buffer and use it from there.  Yes, there are cards with 32 
    non-null characters in the name string.  */


    strncpy (temp, rack->identity[x]->name, 32);
    dprint(DEBUG_VALUES, "Slot %d - %s\n", x, temp);
    if (rack->identity[x]->type == 0x0e)
    {
      rack->cpulocation = x;
      dprint(DEBUG_VALUES, "CPU location %d memorized.\n", x);
    }
    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_VALUES, "ID = %02X   type = %02X - ",
	     rack->identity[x]->ID, rack->identity[x]->type);
      if (rack->identity[x]->type < 32)
      {
	dprint(DEBUG_VALUES, "%s", PROFILE_Names[rack->identity[x]->type]);
      }
      else
      {
	dprint(DEBUG_VALUES, "Unknown type.");
      }
      dprint(DEBUG_VALUES, "\n");
      dprint(DEBUG_VALUES, "Product Code = %02X    Revision = %d.%02d\n",
	     rack->identity[x]->product_code, rack->identity[x]->rev_hi,
	     rack->identity[x]->rev_lo);
      dprint(DEBUG_VALUES,
	     "Device Status = %04X    Device serial number - %08lX\n",
	     rack->identity[x]->status, rack->identity[x]->serial);
      dprint(DEBUG_VALUES, "---------------------------\n");
    }
  }
  free(port);
  free(path);
  dprint(DEBUG_TRACE, "who.c exited.\n");
  return 0;
}
