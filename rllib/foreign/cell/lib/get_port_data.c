
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int get_port_data(_comm_header * comm, _path * path, _port * port, int debug)
{
  int x,ret=0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;

  dprint(DEBUG_TRACE, "get_port_data.c entered.\n");

  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
    return -1;
  }
  if (port == NULL)
  {
    CELLERROR(2,"port structure not allocated");
    return -1;
  }

  buff = malloc(sizeof(_data_buffer));
  if (buff == NULL)
  {
    CELLERROR(3,"Could not allocate memory for data buffer");
    return -1;
  }

  msgbuff = malloc(sizeof(_data_buffer));
  if (msgbuff == NULL)
  {
    CELLERROR(4,"Could not allocate memory for msgdata buffer");
    free(buff);
    return -1;
  }

  head = malloc(sizeof(_encaps_header));
  if (head == NULL)
  {
    CELLERROR(5,"Could not allocate memory for Encaps Header");
    free(buff);
    free(msgbuff);
    return -1;
  }

  sendbuff = malloc(sizeof(_data_buffer));
  if (sendbuff == NULL)
  {
    CELLERROR(6,"Could not allocate memory for send buffer");
    free(buff);
    free(msgbuff);
    free(head);
    return -1;
  }

  ioi1 = malloc(sizeof(_ioi_data));
  if (ioi1 == NULL)
  {
    CELLERROR(7,"Could not allocate memory for ioi buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(sendbuff);
    return -1;
  }

  memset(buff, 0, sizeof(_data_buffer));
  memset(msgbuff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));
  memset(sendbuff, 0, sizeof(_data_buffer));
  memset(ioi1, 0, sizeof(_ioi_data));

  buff->len = 0;

  msgbuff->data[msgbuff->len++] = GET_ATTRIBUTE_SINGLE;

  ioi1->class = PORT_OBJECT;
  ioi1->instance = BASE_INSTANCE;
  ioi1->member = -1;
  ioi1->point = -1;
  ioi1->attribute = ENTRY_PORT;
  ioi1->tagname = NULL;

  ioi(msgbuff, ioi1, NULL, debug);
  makepath(path, msgbuff, debug);
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Msgbuff = ");
    for (x = 0; x < msgbuff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }

  sendRRdata(10, comm, head, buff, debug);
  cpf(CPH_Null, NULL, CPH_Unconnected_message, msgbuff, buff, debug);
  head->len = buff->len;
  memcpy(sendbuff->data, head, ENCAPS_Header_Length);
  sendbuff->overall_len = ENCAPS_Header_Length;
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Loading sendbuffer with command data.\n");
    for (x = 0; x < buff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", buff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }
  memcpy(sendbuff->data + sendbuff->overall_len, buff->data, buff->len);
  sendbuff->overall_len += buff->len;
  ret = senddata(sendbuff, comm, debug);
  if (!ret) ret = readdata(buff, comm, debug);
  memset(head, 0, ENCAPS_Header_Length);
  memcpy(head, buff->data, ENCAPS_Header_Length);
  if (head->status != 0)
  {
    ret=-1;
    dprint(DEBUG_VALUES, "Get port Data command returned an error.\n");
    CELLERROR(8,"Get port data command returned an error");
  }
  if (ret)
  {
    free(buff);
    free(msgbuff);
    free(head);
    free(sendbuff);
    free(ioi);
    return -1;
  }

//buff = buff + ENCAPS_Header_Length;
  memset(port, 0, sizeof(_port));
  memcpy(port, buff->data + 40, sizeof(_port));

  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "Port slot reported as %d.\n", port->slot);
    dprint(DEBUG_VALUES, "Got good reply to Get Port Data Command\n");
    for (x = 40; x < sizeof(_port) + 39; x++)
      dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
    dprint(DEBUG_VALUES, "\n");
  }

  free(buff);
  free(msgbuff);
  free(head);
  free(sendbuff);
  free(ioi);

  dprint(DEBUG_TRACE, "get_port_data.c exiting.\n");
  return 0;
}
