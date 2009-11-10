/* An attempt at reading a single element from a struct.
   Copyright (c) 2002 and beyond - Ron Gage  */
 
 
#include "libcell.h"


int read_struct_element (_comm_header *comm, _path *path, _tag_detail *tag,
		 _struct_list *structs, unsigned num, int debug)
{

int x,y,offset;

offset = 0;
dprint (DEBUG_TRACE, "read_struct_element.c entered.\n");

if (comm == NULL)
	{
	CELLERROR(1,"Comm struct not allocated.");
	return -1;
	}

if (path == NULL)
	{
	CELLERROR(2,"Path struct not allocated.");
	return -1;
	}

if (tag == NULL)
	{
	CELLERROR(3,"Tag struct not allocated.");
	return -1;
	}

if (structs == NULL)
	{
	CELLERROR(4, "Struct list not allocated.");
	return -1;
	}

if (structs->count == 0)
	{
	CELLERROR(10,"No Structs found in PLC.");
	return -1;
	}
dprint (DEBUG_VALUES,"Searching for struct type %04X.\n",tag->type);

for (x=0; x < structs->count; x++)
	{
	if ((tag->type & 0x0fff) == (structs->base[x]->base & 0x0fff))
		{
		if (num == 0)
			offset = 0;
		else
			{
			for (y=0; y < num; y++)
				{
				offset += get_element_size(structs->base[x]->data[y]->type);
				}
			}
		return (read_object_range_value(comm, path, tag, offset, get_element_size(structs->base[x]->data[num]->type), debug));
		}
	}
CELLERROR(21,"Struct DataType not found.");
return -1;
}
		