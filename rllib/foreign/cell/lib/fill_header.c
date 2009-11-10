#include "libcell.h"

void fill_header(_comm_header * comm, _encaps_header * head, int debug)
{
  dprint(DEBUG_TRACE, "fill header entered\n");
  memcpy(head->context, comm->context, 8);
  head->handle = comm->session_id;
  dprint(DEBUG_TRACE, "fill header exited\n");
  return;
}
