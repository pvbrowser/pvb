
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int ioi(_data_buffer * buff, _ioi_data * ioi1, _ioi_data * ioi2, int debug)
{
  int count, x, start;

  dprint(DEBUG_TRACE, "ioi.c entered.\n");
  if (ioi1 == NULL)
  {
    CELLERROR(1,"First IOI structure not defined");
    return -1;
  }

  count = 0;
  start = buff->len;
  if (ioi1->class != -1)
  {
    if ((ioi1->class < 256) & (ioi1->class >= 0))
      count++;
    if ((ioi1->class > 255) & (ioi1->class < 65536))
      count += 2;
    if ((ioi1->class < 0) | (ioi1->class > 65535))
      count += 4;
  }
  dprint(DEBUG_BUILD, "class = %lX, count = %d\n", ioi1->class, count);

  if (ioi1->instance != -1)
  {
    if ((ioi1->instance < 256) & (ioi1->instance >= 0))
      count++;
    if ((ioi1->instance > 255) & (ioi1->instance < 65536))
      count += 2;
    if ((ioi1->instance < 0) | (ioi1->instance > 65535))
      count += 4;
  }
  dprint(DEBUG_BUILD, "instance = %lX, count = %d\n", ioi1->instance, count);

  if (ioi1->member != -1)
  {
    if ((ioi1->member < 256) & (ioi1->member >= 0))
      count++;
    if ((ioi1->member > 255) & (ioi1->member < 65536))
      count += 2;
    if ((ioi1->member < 0) | (ioi1->member > 65535))
      count += 4;
  }
  dprint(DEBUG_BUILD, "member = %lX, count = %d\n", ioi1->member, count);

  if (ioi1->point != -1)
  {
    if ((ioi1->point < 256) & (ioi1->point >= 0))
      count++;
    if ((ioi1->point > 255) & (ioi1->point < 65536))
      count += 2;
    if ((ioi1->point < 0) | (ioi1->point > 65535))
      count += 4;
  }
  dprint(DEBUG_BUILD, "point = %lX, count = %d\n", ioi1->point, count);

  if (ioi1->attribute != -1)
  {
    if ((ioi1->attribute < 256) & (ioi1->attribute >= 0))
      count++;
    if ((ioi1->attribute > 255) & (ioi1->attribute < 65536))
      count += 2;
    if ((ioi1->attribute < 0) | (ioi1->attribute > 65535))
      count += 4;
  }
  dprint(DEBUG_BUILD, "attribute = %lX, count = %d\n", ioi1->attribute,
	 count);

  if (ioi1->tagname != NULL)
  {
    count += (strlen(ioi1->tagname)/2)+1;
    dprint(DEBUG_BUILD, "tagname = %s, length = %d\n", ioi1->tagname,
	   strlen(ioi1->tagname));
  }
  if (ioi2 != NULL)
  {
    if (ioi2->class != -1)
    {
      if ((ioi2->class < 256) & (ioi2->class >= 0))
	count++;
      if ((ioi2->class > 255) & (ioi2->class < 65536))
	count += 2;
      if ((ioi2->class < 0) | (ioi2->class > 65535))
	count += 4;
    }
    dprint(DEBUG_BUILD, "class = %lX, count = %d\n", ioi2->class, count);

    if (ioi2->instance != -1)
    {
      if ((ioi2->instance < 256) & (ioi2->instance >= 0))
	count++;
      if ((ioi2->instance > 255) & (ioi2->instance < 65536))
	count += 2;
      if ((ioi2->instance < 0) | (ioi2->instance > 65535))
	count += 4;
    }
    dprint(DEBUG_BUILD, "instance = %lX, count = %d\n", ioi2->instance,
	   count);

    if (ioi2->member != -1)
    {
      if ((ioi2->member < 256) & (ioi2->member >= 0))
	count++;
      if ((ioi2->member > 255) & (ioi2->member < 65536))
	count += 2;
      if ((ioi2->member < 0) | (ioi2->member > 65535))
	count += 4;
    }
    dprint(DEBUG_BUILD, "member = %lX, count = %d\n", ioi2->member, count);

    if (ioi2->point != -1)
    {
      if ((ioi2->point < 256) & (ioi2->point >= 0))
	count++;
      if ((ioi2->point > 255) & (ioi2->point < 65536))
	count += 2;
      if ((ioi2->point < 0) | (ioi2->point > 65535))
	count += 4;
    }
    dprint(DEBUG_BUILD, "point = %lX, count = %d\n", ioi2->point, count);

    if (ioi2->attribute != -1)
    {
      if ((ioi2->attribute < 256) & (ioi2->attribute >= 0))
	count++;
      if ((ioi2->attribute > 255) & (ioi2->attribute < 65536))
	count += 2;
      if ((ioi2->attribute < 0) | (ioi2->attribute > 65535))
	count += 4;
    }
    dprint(DEBUG_BUILD, "attribute = %lX, count = %d\n", ioi2->attribute,
	   count);

    if (ioi2->tagname != NULL)
    {
      count += strlen(ioi2->tagname);
      dprint(DEBUG_BUILD, "tagname = %s, length = %d\n", ioi2->tagname,
	     strlen(ioi2->tagname));
    }
  }


  if (count != 0)
    buff->data[buff->len++] = count;

  if (ioi1->class != -1)
  {
    if ((ioi1->class < 256) & (ioi1->class >= 0))
    {
      buff->data[buff->len++] = 0x20;
      buff->data[buff->len++] = (byte) ioi1->class;
    }
    if ((ioi1->class > 255) & (ioi1->class < 65536))
    {
      buff->data[buff->len++] = 0x21;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->class & 255;
      buff->data[buff->len++] = ioi1->class / 0x100;
    }
    if ((ioi1->class < 0) | (ioi1->class > 65535))
    {
      buff->data[buff->len++] = 0x22;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->class & 255;
      buff->data[buff->len++] = (ioi1->class / 0x100) & 255;
      buff->data[buff->len++] = (ioi1->class / 0x10000) & 255;
      buff->data[buff->len++] = (ioi1->class / 0x1000000);
    }
  }

  if (ioi1->instance != -1)
  {
    if ((ioi1->instance < 256) & (ioi1->instance >= 0))
    {
      buff->data[buff->len++] = 0x24;
      buff->data[buff->len++] = (byte) ioi1->instance;
    }
    if ((ioi1->instance > 255) & (ioi1->instance < 65536))
    {
      buff->data[buff->len++] = 0x25;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->instance & 255;
      buff->data[buff->len++] = ioi1->instance / 0x100;
    }
    if ((ioi1->instance < 0) | (ioi1->instance > 65535))
    {
      buff->data[buff->len++] = 0x26;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->instance & 255;
      buff->data[buff->len++] = (ioi1->instance / 0x100) & 255;
      buff->data[buff->len++] = (ioi1->instance / 0x10000) & 255;
      buff->data[buff->len++] = (ioi1->instance / 0x1000000);
    }
  }

  if (ioi1->member != -1)
  {
    if ((ioi1->member < 256) & (ioi1->member >= 0))
    {
      buff->data[buff->len++] = 0x28;
      buff->data[buff->len++] = (byte) ioi1->member;
    }
    if ((ioi1->member > 255) & (ioi1->member < 65536))
    {
      buff->data[buff->len++] = 0x29;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->member & 255;
      buff->data[buff->len++] = ioi1->member / 0x100;
    }
    if ((ioi1->member < 0) | (ioi1->member > 65535))
    {
      buff->data[buff->len++] = 0x2a;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->member & 255;
      buff->data[buff->len++] = (ioi1->member / 0x100) & 255;
      buff->data[buff->len++] = (ioi1->member / 0x10000) & 255;
      buff->data[buff->len++] = (ioi1->member / 0x1000000);
    }
  }

  if (ioi1->point != -1)
  {
    if ((ioi1->point < 256) & (ioi1->point >= 0))
    {
      buff->data[buff->len++] = 0x2c;
      buff->data[buff->len++] = (byte) ioi1->point;
    }
    if ((ioi1->point > 255) & (ioi1->point < 65536))
    {
      buff->data[buff->len++] = 0x2d;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->point & 255;
      buff->data[buff->len++] = ioi1->point / 0x100;
    }
    if ((ioi1->point < 0) | (ioi1->point > 65535))
    {
      buff->data[buff->len++] = 0x2e;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->point & 255;
      buff->data[buff->len++] = (ioi1->point / 0x100) & 255;
      buff->data[buff->len++] = (ioi1->point / 0x10000) & 255;
      buff->data[buff->len++] = (ioi1->point / 0x1000000);
    }
  }

  if (ioi1->attribute != -1)
  {
    if ((ioi1->attribute < 256) & (ioi1->attribute >= 0))
    {
      buff->data[buff->len++] = 0x30;
      buff->data[buff->len++] = (byte) ioi1->attribute;
    }
    if ((ioi1->attribute > 255) & (ioi1->attribute < 65536))
    {
      buff->data[buff->len++] = 0x31;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->attribute & 255;
      buff->data[buff->len++] = ioi1->attribute / 0x100;
    }
    if ((ioi1->attribute < 0) | (ioi1->attribute > 65535))
    {
      buff->data[buff->len++] = 0x32;
      buff->data[buff->len++] = 0;
      buff->data[buff->len++] = ioi1->attribute & 255;
      buff->data[buff->len++] = (ioi1->attribute / 0x100) & 255;
      buff->data[buff->len++] = (ioi1->attribute / 0x10000) & 255;
      buff->data[buff->len++] = (ioi1->attribute / 0x1000000);
    }
  }

/* Reserve space for the "electronic key" object type */


  if (ioi1->tagname != NULL)
  {
    buff->data[buff->len++] = 0x91;
    buff->data[buff->len++] = strlen(ioi1->tagname);

    memcpy(&buff->data[buff->len], ioi1->tagname, strlen(ioi1->tagname));
    buff->len = buff->len + strlen(ioi1->tagname);
    if (strlen(ioi1->tagname) & 1)
      buff->data[buff->len++] = 0;
  }
  if (ioi2 != NULL)
  {

    if (ioi2->class != -1)
    {
      if ((ioi2->class < 256) & (ioi2->class >= 0))
      {
	buff->data[buff->len++] = 0x20;
	buff->data[buff->len++] = (byte) ioi2->class;
      }
      if ((ioi2->class > 255) & (ioi2->class < 65536))
      {
	buff->data[buff->len++] = 0x21;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->class & 255;
	buff->data[buff->len++] = ioi2->class / 0x100;
      }
      if ((ioi2->class < 0) | (ioi2->class > 65535))
      {
	buff->data[buff->len++] = 0x22;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->class & 255;
	buff->data[buff->len++] = (ioi2->class / 0x100) & 255;
	buff->data[buff->len++] = (ioi2->class / 0x10000) & 255;
	buff->data[buff->len++] = (ioi2->class / 0x1000000);
      }
    }

    if (ioi2->instance != -1)
    {
      if ((ioi2->instance < 256) & (ioi2->instance >= 0))
      {
	buff->data[buff->len++] = 0x24;
	buff->data[buff->len++] = (byte) ioi2->instance;
      }
      if ((ioi2->instance > 255) & (ioi2->instance < 65536))
      {
	buff->data[buff->len++] = 0x25;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->instance & 255;
	buff->data[buff->len++] = ioi2->instance / 0x100;
      }
      if ((ioi2->instance < 0) | (ioi2->instance > 65535))
      {
	buff->data[buff->len++] = 0x26;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->instance & 255;
	buff->data[buff->len++] = (ioi2->instance / 0x100) & 255;
	buff->data[buff->len++] = (ioi2->instance / 0x10000) & 255;
	buff->data[buff->len++] = (ioi2->instance / 0x1000000);
      }
    }

    if (ioi2->member != -1)
    {
      if ((ioi2->member < 256) & (ioi2->member >= 0))
      {
	buff->data[buff->len++] = 0x28;
	buff->data[buff->len++] = (byte) ioi2->member;
      }
      if ((ioi2->member > 255) & (ioi2->member < 65536))
      {
	buff->data[buff->len++] = 0x29;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->member & 255;
	buff->data[buff->len++] = ioi2->member / 0x100;
      }
      if ((ioi2->member < 0) | (ioi2->member > 65535))
      {
	buff->data[buff->len++] = 0x2a;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->member & 255;
	buff->data[buff->len++] = (ioi2->member / 0x100) & 255;
	buff->data[buff->len++] = (ioi2->member / 0x10000) & 255;
	buff->data[buff->len++] = (ioi2->member / 0x1000000);
      }
    }

    if (ioi2->point != -1)
    {
      if ((ioi2->point < 256) & (ioi2->point >= 0))
      {
	buff->data[buff->len++] = 0x2c;
	buff->data[buff->len++] = (byte) ioi2->point;
      }
      if ((ioi2->point > 255) & (ioi2->point < 65536))
      {
	buff->data[buff->len++] = 0x2d;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->point & 255;
	buff->data[buff->len++] = ioi2->point / 0x100;
      }
      if ((ioi2->point < 0) | (ioi2->point > 65535))
      {
	buff->data[buff->len++] = 0x2e;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->point & 255;
	buff->data[buff->len++] = (ioi2->point / 0x100) & 255;
	buff->data[buff->len++] = (ioi2->point / 0x10000) & 255;
	buff->data[buff->len++] = (ioi2->point / 0x1000000);
      }
    }

    if (ioi2->attribute != -1)
    {
      if ((ioi2->attribute < 256) & (ioi2->attribute >= 0))
      {
	buff->data[buff->len++] = 0x30;
	buff->data[buff->len++] = (byte) ioi2->attribute;
      }
      if ((ioi2->attribute > 255) & (ioi2->attribute < 65536))
      {
	buff->data[buff->len++] = 0x31;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->attribute & 255;
	buff->data[buff->len++] = ioi2->attribute / 0x100;
      }
      if ((ioi2->attribute < 0) | (ioi2->attribute > 65535))
      {
	buff->data[buff->len++] = 0x32;
	buff->data[buff->len++] = 0;
	buff->data[buff->len++] = ioi2->attribute & 255;
	buff->data[buff->len++] = (ioi2->attribute / 0x100) & 255;
	buff->data[buff->len++] = (ioi2->attribute / 0x10000) & 255;
	buff->data[buff->len++] = (ioi2->attribute / 0x1000000);
      }
    }

/* Reserve space for the "electronic key" object type */


    if (ioi2->tagname != NULL)
    {
      strcpy(&buff->data[buff->len], ioi2->tagname);
      buff->len = buff->len + strlen(ioi2->tagname);
    }
  }

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Buffer Started at %d, Length = %d, buff->len = %d\n",
	   start, buff->len - start, buff->len);
    for (x = 0; x < (buff->len - start); x++)
      dprint(DEBUG_BUILD, "%02X  ", buff->data[x + start]);
    dprint(DEBUG_BUILD, "\n");
  }
  dprint(DEBUG_TRACE, "ioi.c exited.\n");
  return 0;
}
