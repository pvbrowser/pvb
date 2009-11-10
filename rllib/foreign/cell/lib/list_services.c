
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int list_services(_comm_header * comm, _services * services, int debug)
{
  _encaps_header *head;
  _data_buffer *buff;
  void *temp;
  int ret;

  dprint(DEBUG_TRACE, "--- List_services.c entered\n");

  head = malloc(sizeof(_encaps_header));
  if (head == NULL)
  {
    CELLERROR(1,"Could not allocate memory for list_services.");
    return -1;
  }

  buff = malloc(sizeof(_data_buffer));
  if (buff == NULL)
  {
    CELLERROR(2,"Could not allocate transmit buffer.");
    free(head);
    return -1;
  }


  memset(buff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));
  memset(services, 0, sizeof(_services));

  fill_header(comm, head, debug);

  head->command = ENCAPS_List_Services;

  memcpy(buff->data, head, ENCAPS_Header_Length);
  buff->overall_len = ENCAPS_Header_Length;
  ret = senddata(buff, comm, debug);
  if (!ret) ret = readdata(buff, comm, debug);

  /* even on error, none of these functions will fail, so let fall out normally */

  (unsigned long)temp = (unsigned long)buff + ENCAPS_Header_Length + 2; /* point to start of actual data */

  memcpy(services, temp, sizeof(_services));

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "Item Type = %X\n", services->type);
    dprint(DEBUG_VALUES, "Descriptor Length = %d\n", services->length);
    dprint(DEBUG_VALUES, "Protocol Version = %d\n", services->version);
    dprint(DEBUG_VALUES, "Flags = %04X\n", services->flags);
    dprint(DEBUG_VALUES, "Service name = %s\n", services->name);
  }

  free(head);
  free(buff);

  dprint(DEBUG_TRACE, "list_services.c exiting.\n");

  return ret;
}
