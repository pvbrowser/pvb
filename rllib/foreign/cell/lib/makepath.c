#include "libcell.h"

int makepath(_path * path, _data_buffer * buff, int debug)
{
  int count = 0;

  dprint(DEBUG_TRACE, "path.c entered.\n");
  if (buff == NULL)
  {
    CELLERROR(1,"Buffer for path.c not allocated.");
    return -1;
  }
  if (path == NULL)
  {
    CELLERROR(2,"Path structure not set - path.c");
    return -1;
  }
  if (path->device1 != -1)
    count++;
  if (path->device2 != -1)
    count++;
  if (path->device3 != -1)
    count++;
  if (path->device4 != -1)
    count++;
  if (path->device5 != -1)
    count++;
  if (path->device6 != -1)
    count++;
  if (path->device7 != -1)
    count++;
  if (path->device8 != -1)
    count++;

  if (count != 0)
  {
    count = count / 2;
    buff->data[buff->len++] = count & 255;
    buff->data[buff->len++] = count / 0x100;
  }

  if (path->device1 != -1)
    buff->data[buff->len++] = path->device1;
  if (path->device2 != -1)
    buff->data[buff->len++] = path->device2;
  if (path->device3 != -1)
    buff->data[buff->len++] = path->device3;
  if (path->device4 != -1)
    buff->data[buff->len++] = path->device4;
  if (path->device5 != -1)
    buff->data[buff->len++] = path->device5;
  if (path->device6 != -1)
    buff->data[buff->len++] = path->device6;
  if (path->device7 != -1)
    buff->data[buff->len++] = path->device7;
  if (path->device8 != -1)
    buff->data[buff->len++] = path->device8;

  dprint(DEBUG_TRACE, "path.c exited, added %d path items...\n", count);
  return 0;

}
