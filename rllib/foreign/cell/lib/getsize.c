
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

unsigned short getsize(_tag_detail * tag)
{
  int size=0;

  if (tag == NULL)
  {
    CELLERROR(1,"getsize routine called without tag structure.");
    return 0;
  }
  size = tag->size;
  if (tag->alias_size != 0)
    size = tag->alias_size;

  if (tag->arraysize1 == 0)
    return (size);
  if (tag->arraysize2 == 0)
    return (unsigned short)(size * tag->arraysize1);
  if (tag->arraysize3 == 0)
    return (unsigned short)(size * tag->arraysize1 * tag->arraysize2);
  return (unsigned short)(size * tag->arraysize1 * tag->arraysize2 * tag->arraysize3);
}
