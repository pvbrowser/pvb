
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int get_struct_details(_comm_header * comm, _path * path, _struct_base * tag,
			int debug)
{
  byte *buffer;
  int x, y, size, offset, start, start2;
  int ret=0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;
  _ioi_data *ioi2;

  dprint(DEBUG_TRACE, "get_struct_details.c entered.\n");
  ioi2 = NULL;
  size = 0;
  offset = 0;
  start2 = 0;

  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
    return -1;
  }

  if (tag == NULL)
  {
    CELLERROR(2,"tag structure not allocated");
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
    CELLERROR(8,"Could not allocate memory for ioi1 buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    return -1;
  }

  buffer = malloc(8192);
  if (buffer == NULL)
  {
    CELLERROR(9,"Could not allocate data assembly buffer.");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    return -1;
  }

  memset(buffer, 0, 8192);


  buff->len = 0;
  while (1)
  {
    size = tag->detailsize - offset;
    if (size > CELL_PKT_SIZE)
      size = CELL_PKT_SIZE;
    memset(buff, 0, sizeof(_data_buffer));
    memset(msgbuff, 0, sizeof(_data_buffer));
    memset(head, 0, sizeof(_encaps_header));
    memset(cpf_buffer, 0, sizeof(_data_buffer));
    memset(sendbuff, 0, sizeof(_data_buffer));
    memset(ioi1, 0, sizeof(_ioi_data));

    msgbuff->data[msgbuff->len++] = DATA_TYPE_CONFIG;
    ioi1->class = STRUCT_LIST;
    ioi1->instance = tag->base;
    ioi1->member = -1;
    ioi1->point = -1;
    ioi1->attribute = -1;
    ioi1->tagname = NULL;

    ioi(msgbuff, ioi1, ioi2, debug);

    msgbuff->data[msgbuff->len++] = offset & 255;
    msgbuff->data[msgbuff->len++] = offset / 0x100;
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = size & 255;
    msgbuff->data[msgbuff->len++] = size / 0x100;
    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_BUILD, "Msgbuff = ");
      for (x = 0; x < msgbuff->len; x++)
	dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
      dprint(DEBUG_BUILD, "\n");
    }

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
      ret = -1;
      dprint(DEBUG_VALUES, "Get Struct Details command returned an error.\n");
    }
    if (ret)
    {
      free(buff);
      free(msgbuff);
      free(head);
      free(cpf_buffer);
      free(sendbuff);
      free(ioi1);
      free(buffer);
      return -1;
    }
//buff = buff + ENCAPS_Header_Length;
    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_VALUES,
	     "Got good reply to Get Struct Details Command - %d\n",
	     buff->overall_len);
      for (x = 44; x < buff->overall_len; x++)
	dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
      dprint(DEBUG_VALUES, "\n");
    }
    dprint(DEBUG_TRACE, "offset = %d, size = %d\n", offset, size);
    memcpy(&buffer[start2], &buff->data[44], size);
    start2 += size;
    if (size != 255)
      break;
    offset += size;
  }

  start = 0;

/* Parse off the data types for each element */

  for (x = 0; x < tag->count; x++)
  {
    tag->data[x]->arraysize = buffer[start] + (buffer[start + 1] * 256);
    tag->data[x]->type = buffer[start + 2] + (buffer[start + 3] * 256);
    start += 8;
  }


/* Parse off the name for the struct. */
  y = 0;
  while (1)
  {
    x = buffer[start++];
    if (x == ';')
      break;
    tag->name[y++] = x;
  }

/* Next, skip past the unicode type data  */

  while (1)
  {
    x = buffer[start++];
    if (x == 0)
      break;
  }

/* Finally, get the names for each of the elements for the
structure.  Each element is null terminated so strcpy should work */

  for (x = 0; x < tag->count; x++)
  {
    strcpy(tag->data[x]->name, &buffer[start]);
    start += strlen(tag->data[x]->name) + 1;
  }

  free(buff);
  free(msgbuff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(ioi1);
  free(buffer);
  dprint(DEBUG_TRACE, "get_struct_details.c exited. \n");
  return 0;
}
