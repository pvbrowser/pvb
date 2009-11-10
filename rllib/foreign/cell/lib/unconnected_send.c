
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int unconnected_send(_data_buffer * buff, int debug)
{
  dprint(DEBUG_TRACE, "unconnected_send.c entered.\n");
  if (buff == NULL)
  {
    CELLERROR(1,"Buffer for unconnected_send.c not allocated.");
    return -1;
  }
  buff->data[buff->len++] = PDU_Unconnected_Send;
  dprint(DEBUG_TRACE, "unconnected_send.c exited...\n");
  return 0;
}
