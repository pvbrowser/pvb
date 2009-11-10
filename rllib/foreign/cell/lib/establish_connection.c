#include "libcell.h"

void establish_connection(_comm_header * comm, _services * services,
        int debug)
{

  dprint(DEBUG_TRACE, "Entering establish_connection.c\n");
  cell_attach(comm, debug);
  if (comm->error != OK)
  {
    if (debug != DEBUG_NIL)
    {
      switch (comm->error)
      {
      case NOHOST:
  dprint(DEBUG_TRACE, "Could not resolve host name.\n");
  break;
      case NOCONNECT:
  dprint(DEBUG_TRACE, "Could not connect to host %s.\n",
         comm->hostname);
  break;
      case BADADDR:
  dprint(DEBUG_TRACE, "Bad address for host %s.\n", comm->hostname);
  break;
      default:
  dprint(DEBUG_TRACE, "Unknown error occured - %d\n", comm->error);
  break;
      }
    }
    return;
  }
  dprint(DEBUG_VALUES, "Session ID = %ld\n", comm->session_id);
  dprint(DEBUG_VALUES, "File Handle = %d\n", comm->file_handle);

  list_targets(comm, debug);
  list_services(comm, services, debug);
  list_interfaces(comm, debug);
  register_session(comm, debug);
  dprint(DEBUG_TRACE, "establish_connection.c exited\n");
  return;
}
