
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int get_device_data(_comm_header * comm, _path * path, _identity * identity,
		     int debug)
{
  int x,ret=0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;

  dprint(DEBUG_TRACE, "get_device_data.c entered.\n");

  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
    return -1;
  }
  if (identity == NULL)
  {
    CELLERROR(2,"identity structure not allocated");
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

  cpf_buffer = malloc(sizeof(_data_buffer));
  if (cpf_buffer == NULL)
  {
    CELLERROR(6,"Could not allocate memory for cpf_buffer");
    free(buff);
    free(msgbuff);
    free(head);
    return -1;
  }

  sendbuff = malloc(sizeof(_data_buffer));
  if (sendbuff == NULL)
  {
    CELLERROR(7,"Could not allocate memory for send buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    return -1;
  }

  ioi1 = malloc(sizeof(_ioi_data));
  if (ioi1 == NULL)
  {
    CELLERROR(8,"Could not allocate memory for ioi buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    return -1;
  }

  memset(buff, 0, sizeof(_data_buffer));
  memset(msgbuff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));
  memset(cpf_buffer, 0, sizeof(_data_buffer));
  memset(sendbuff, 0, sizeof(_data_buffer));
  memset(ioi1, 0, sizeof(_ioi_data));

  buff->len = 0;

  msgbuff->data[msgbuff->len++] = GET_ATTRIBUTE_ALL;
  ioi1->class = IDENTITY;
  ioi1->instance = FIRST_INSTANCE;
  ioi1->member = -1;
  ioi1->point = -1;
  ioi1->attribute = -1;
  ioi1->tagname = NULL;


  ioi(msgbuff, ioi1, NULL, debug);
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Msgbuff = ");
    for (x = 0; x < msgbuff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }


  if (path->device1 != -1)
  {
    cpf_buffer->data[cpf_buffer->len++] = PDU_Unconnected_Send;
    ioi1->class = CONNECTION_MANAGER;
    ioi1->instance = FIRST_INSTANCE;
    ioi1->member = -1;
    ioi1->point = -1;
    ioi1->attribute = -1;
    ioi1->tagname = NULL;

    ioi(cpf_buffer, ioi1, NULL, debug);
    settimeout(6, 0x9a, cpf_buffer, debug);
    cpf_buffer->data[cpf_buffer->len++] = msgbuff->len & 255;
    cpf_buffer->data[cpf_buffer->len++] = msgbuff->len / 0x100;
  }
  makepath(path, msgbuff, debug);
  memcpy(cpf_buffer->data + cpf_buffer->len, msgbuff->data, msgbuff->len);
  cpf_buffer->len += msgbuff->len;

  sendRRdata(10, comm, head, buff, debug);
  cpf(CPH_Null, NULL, CPH_Unconnected_message, cpf_buffer, buff, debug);

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
    dprint(DEBUG_VALUES, "Get device Data command returned an error.\n");
    CELLERROR(9,"Get device data command returned an error");
  }
  if (ret)
  {
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    return -1;
  }

//buff = buff + ENCAPS_Header_Length;
  if (buff->overall_len > 68)
  {
    memset(identity, 0, sizeof(_identity));
    memcpy(identity, buff->data + 44, sizeof(_identity));
  }
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_VALUES, "Got good reply to Get Identity Data Command - %d\n",
	   buff->overall_len);
    for (x = 44; x < buff->overall_len; x++)
      dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
    dprint(DEBUG_VALUES, "\n");
  }

  free(buff);
  free(msgbuff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(ioi1);

  dprint(DEBUG_TRACE, "Exiting get_device_data.c\n");

  return 0;
}
