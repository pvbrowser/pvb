
/* Revised 5/3/02 William Hays - CTI */

#include "libcell.h"

int read_object_value(_comm_header * comm, _path * path, _tag_detail * tag,
		      int debug)
{
return read_object_range_value (comm, path, tag, 0, getsize(tag),debug);
}

