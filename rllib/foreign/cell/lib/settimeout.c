
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int settimeout(byte priority, byte timeout, _data_buffer * buff, int debug)
{
  char ac_dmesg[60];
  dprint(DEBUG_TRACE,
	 "settimeout.c entered, priority = %02X, timeout = %02X\n", priority,
	 timeout);

  if (buff == NULL)
  {
    CELLERROR(1,"buffer not allocated");
    return -1;
  }
  if (priority == 0)
  {
    sprintf(ac_dmesg,"Priority value invalid: %d",priority);

    CELLERROR(2,ac_dmesg);
    return -1;
  }
  if (timeout == 0)
  {
    sprintf(ac_dmesg,"Timeout value invalid: %d",timeout);

    CELLERROR(3,"Timeout value invalid :%d");
    return -1;
  }
  buff->data[buff->len++] = priority;
  buff->data[buff->len++] = timeout;
  dprint(DEBUG_TRACE, "Exiting settimeout.c\n");
  return -0;
}
