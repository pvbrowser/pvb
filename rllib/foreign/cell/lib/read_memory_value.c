
/* Revised 5/6/02 William Hays - CTI */
#include "libcell.h"

int read_memory_value(_comm_header * comm, _path * path,
			    _tag_detail *tag, int debug)
{
  int x, start, size, offset, totalsize;
  unsigned long junk,stuff;
  int ret=0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;
  _ioi_data *ioi2;
  _data_buffer *ioilist;
  

  dprint(DEBUG_TRACE, "read_memory_value.c entered.\n");
  ioi2 = NULL;
  size = 0;
  offset = 0;
  totalsize = 0;


  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
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
  ioi2 = malloc(sizeof(_ioi_data));
  if (ioi2 == NULL)
    {
    CELLERROR(9,"Could not allocate memory for ioi2 buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    return -1;
    }
  ioilist = malloc(sizeof(_data_buffer));
  if (ioilist == NULL)
    {
    CELLERROR(10,"Couldn't allocate ioi list buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    free(ioi2);
    return -1;
    }

  buff->len = 0;
// while (1){
  dprint(DEBUG_TRACE,
	 "Loop entered.  totalsize = %d   size = %d, offset = %d\n",
	 totalsize, size, offset);
  memset(buff, 0, sizeof(_data_buffer));
  memset(msgbuff, 0, sizeof(_data_buffer));
  memset(head, 0, sizeof(_encaps_header));
  memset(cpf_buffer, 0, sizeof(_data_buffer));
  memset(sendbuff, 0, sizeof(_data_buffer));
  memset(ioi1, 0, sizeof(_ioi_data));
  memset(ioi2, 0, sizeof(_ioi_data));
  memset(ioilist, 0, sizeof(_data_buffer));

/*  msgbuff->data[msgbuff->len++] = DATA_TYPE_CONFIG;
  ioi1->class = DATA_OBJECT;
  ioi1->instance = BASE_INSTANCE;
  ioi1->member = -1;
  ioi1->point = -1;
  ioi1->attribute = -1;
  ioi1->tagname = NULL;
  ioi(msgbuff,ioi1,NULL, debug);
  msgbuff->data[msgbuff->len++] = (byte)(list->count & 255);
  msgbuff->data[msgbuff->len++] = (byte)(list->count / 256); */
    junk = tag->linkid;
    totalsize = getsize(tag);
    ioilist->data[ioilist->len++] = DATA_TYPE_CONFIG;
    ioi1->class = DATA_OBJECT;
    ioi1->instance =BASE_INSTANCE;
    ioi1->member = -1;
    ioi1->point = -1;
    ioi1->attribute = -1;
    ioi1->tagname = NULL;
    ioi(ioilist,ioi1,NULL,debug);
    ioilist->data[ioilist->len++] = (byte)(junk & 255);
    ioilist->data[ioilist->len++] = (byte)((junk >> 8) & 255);
    ioilist->data[ioilist->len++] = (byte)((junk >> 16) & 255);
    ioilist->data[ioilist->len++] = (byte)((junk >> 24) & 255);
    junk = getsize(tag)/4;
//    junk = 1;
    ioilist->data[ioilist->len++] = (byte)(junk & 255);
    ioilist->data[ioilist->len++] = (byte)((junk >> 8) & 255);
      
  memcpy (msgbuff->data + msgbuff->len, ioilist->data, ioilist->len);
  msgbuff->len += ioilist->len;
  

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
    dprint(DEBUG_VALUES, "Read Memory Value command returned an error.\n");
  }
  if (ret)
  {
    free(ioi2);
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    return -1;
  }
//buff = buff + ENCAPS_Header_Length;
  if (debug != DEBUG_NIL)
  {
    dprint(DEBUG_BUILD, "Got good reply to Read Memory Value Command - %d\n",
	   buff->overall_len);
    for (x = 44; x < buff->overall_len; x++)
      dprint(DEBUG_BUILD, "%02X ", buff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }
  dprint(DEBUG_TRACE, "offset = %d, size = %d\n", offset, size);


  start = 44;
  junk = buff->data[start] + (buff->data[start+1]*256);
  junk++;
  start += (junk*2);
        if (tag->data != NULL)
          memcpy (tag->data, buff->data + start, 4);





  x = buff->data[42];
     free(ioi2);
  free(buff);
  free(msgbuff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(ioi1);
  dprint(DEBUG_TRACE, "read_memory_value.c exited.\n");
  return x;
}
