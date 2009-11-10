
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int unregister_session(_comm_header * comm, int debug)
{

  _encaps_header *head;
  _data_buffer *buff;
  dprint(DEBUG_TRACE, "--- UnRegister_session.c entered\n");

  head = malloc(sizeof(_encaps_header));
  if (head == NULL)
  {
    CELLERROR(1,"Could not allocate memory for UnRegister_session.");
    return -1;
  }
  buff = malloc(sizeof(_data_buffer));
  if (buff == NULL)
  {
    CELLERROR(2,"Could not allocate transmit buffer memory.");
    return -1;
  }

  memset(buff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));

  fill_header(comm, head, debug);


  head->command = ENCAPS_UnRegister_Session;

  memcpy(buff->data, head, ENCAPS_Header_Length);
  buff->overall_len = ENCAPS_Header_Length;

  senddata(buff, comm, debug); /* If this fails, oh-well, we are closing the socket */
  
  closesocket(comm->file_handle);
  free(head);
  free(buff);
  dprint(DEBUG_TRACE, "unregister_session.c exited.\n");
  return 0;
}
