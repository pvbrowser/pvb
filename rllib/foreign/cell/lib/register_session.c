
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int register_session(_comm_header * comm, int debug)
{
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *receive_buffer;
  int x;
  int ret=0;

  dprint(DEBUG_TRACE, "--- Register_session.c entered\n");

  head = malloc(sizeof(_encaps_header));
  if (head == NULL)
  {
    CELLERROR(1,"Could not allocate memory for Register_session.");
    return -1;
  }

  buff = malloc(sizeof(_data_buffer));
  if (buff == NULL)
  {
    CELLERROR(2,"Could not allocate memory for transmit buffer.");
    free(head);
    return -1;
  }

  receive_buffer = malloc(sizeof(_data_buffer));
  if (receive_buffer == NULL)
  {
    CELLERROR(3,
      "Could not allocate memory for Receive Buffer - Register_session.");
    free(head);
    free(buff);
    return -1;
  }


  memset(buff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));
  memset(receive_buffer, 0, sizeof(_data_buffer));

  fill_header(comm, head, debug);


  head->command = ENCAPS_Register_Session;
  head->len = 4;
  memcpy(buff->data, head, ENCAPS_Header_Length);
  buff->data[ENCAPS_Header_Length] = 1;	/* Protocol Version Number */

  buff->overall_len = ENCAPS_Header_Length + 4;

  ret = senddata(buff, comm, debug);

  if (!ret) ret = readdata(receive_buffer, comm, debug);

  if (ret)
  {
    free(head);
    free(buff);
    free(receive_buffer);
    dprint(DEBUG_TRACE, "register_session.c failed, returning %d\n",ret);
    return ret;
  }
  memset(head, 0, ENCAPS_Header_Length);

  memcpy(head, receive_buffer->data, ENCAPS_Header_Length);
  comm->session_id = head->handle;

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "command = %02X\n", head->command);
    dprint(DEBUG_VALUES, "len = %d\n", head->len);
    dprint(DEBUG_VALUES, "handle = %ld\n", head->handle);
    dprint(DEBUG_VALUES, "status = %ld\n", head->status);
    dprint(DEBUG_VALUES, "context = ");
    for (x = 0; x < 8; x++)
      dprint(DEBUG_VALUES, "%02X ", head->context[x]);
    dprint(DEBUG_VALUES, "\nOptions = %ld\n", head->options);
  }

  free(head);
  free(buff);
  free(receive_buffer);

  dprint(DEBUG_TRACE, "register_session.c exited.\n");
  return 0;
}
