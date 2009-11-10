
/* Revised 5/6/02 William Hays - CTI */

#include "libcell.h"

int write_object_range_value(_comm_header * comm, _path * path, _tag_detail * tag,
			unsigned offset, unsigned count, int debug)
{
  unsigned long a, b;
  int x, size, totalsize;
  int tagsize;
  int ret=0;
  _tag_detail *basetag;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;
  _ioi_data *ioi2 = NULL;
  _ioi_data ioiCM;

  dprint(DEBUG_TRACE, "write_object_range_value.c entered.\n");
  ioi2 = NULL;

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

  if ((count + offset) > getsize(tag))
    {
    CELLERROR(21,"Count plus offset larger than object.");
    return -1;
    }

  buff = malloc(sizeof(_data_buffer));
  if (buff == NULL)
  {
    CELLERROR(3, "Could not allocate memory for data buffer");
    return -1;
  }

  msgbuff = malloc(sizeof(_data_buffer));
  if (msgbuff == NULL)
  {
    CELLERROR(4, "Could not allocate memory for msgdata buffer");
    free(buff);
    return -1;
  }

  head = malloc(sizeof(_encaps_header));
  if (head == NULL)
  {
    CELLERROR(5, "Could not allocate memory for Encaps Header");
    free(buff);
    free(msgbuff);
    return -1;
  }

  cpf_buffer = malloc(sizeof(_data_buffer));
  if (cpf_buffer == NULL)
  {
    CELLERROR(6, "Could not allocate memory for cpf_buffer");
    free(buff);
    free(msgbuff);
    free(head);
    return -1;
  }

  sendbuff = malloc(sizeof(_data_buffer));
  if (sendbuff == NULL)
  {
    CELLERROR(7, "Could not allocate memory for send buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    return -1;
  }

  ioi1 = malloc(sizeof(_ioi_data));
  if (ioi1 == NULL)
  {
    CELLERROR(8, "Could not allocate memory for ioi1 buffer");
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    return -1;
  }
  basetag = NULL;

  if (tag->alias_size != 0)
  {
    /* This is where we attempt to handle writing aliases.  It is not as clean as I would have liked it
       to be, and it probably won't work in all circumstances as it is written, but it should cover the 
       important aspects.  This is particularily designed to handle aliases pointing into an array of
       objects (i.e. an IO container).  Other cases may or may not work. 

       Please note the importance of how this is executed.  We must read the entire IO container first, 
       then we modify the relevent data within the container, then we write the data back out.  This must
       take place as quickly as possible to minimize the effect of the write routine stepping on values that
       may have been updated by the PLC in the background. */


    basetag = malloc(sizeof(_tag_detail));
    if (basetag == NULL)
    {
      CELLERROR(9, "Could not allocate memory for temporary tag");
      free(buff);
      free(msgbuff);
      free(head);
      free(cpf_buffer);
      free(sendbuff);
      free(ioi1);
      return -1;
    }

    memset(basetag, 0, sizeof(_tag_detail));
    basetag->topbase = tag->alias_topbase;
    basetag->base = tag->alias_base;
    basetag->id = tag->alias_id;
    basetag->linkid = tag->alias_linkid;
    basetag->type = tag->alias_type;
    basetag->size = tag->alias_size;
    tagsize = getsize(basetag);
    if (tagsize)
      basetag->data = malloc(tagsize);

    read_object_range_value(comm, path, basetag, offset, count, debug);

    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_VALUES, "Initial data for alias write: ");
      for (x = 0; x < basetag->datalen; x++)
	dprint(DEBUG_VALUES, "%02X ", basetag->data[x]);
      dprint(DEBUG_VALUES, "\n");
    }
    memcpy(basetag->data + (tag->linkid - tag->alias_linkid), tag->data,
	   tag->size);

    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_VALUES, "Data to be written: ");
      for (x = 0; x < basetag->datalen; x++)
	dprint(DEBUG_VALUES, "%02X ", basetag->data[x]);
      dprint(DEBUG_VALUES, "\n");
    }

    write_object_range_value(comm, path, basetag, offset, count, debug);

    free(basetag->data);

    free(basetag);
    free(buff);
    free(msgbuff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);
    return 0;
  }


  totalsize = offset + count;
  size = 0;
    memset(ioi1, 0, sizeof(_ioi_data));
    ioi1->class = OBJECT_CONFIG;
    ioi1->instance = tag->topbase & 0xffff;
    if (tag->alias_topbase != 0)
      ioi1->instance = tag->alias_topbase & 0xffff;

    ioi1->member = -1;
    ioi1->point = -1;
    ioi1->attribute = -1;
    ioi1->tagname = NULL;
    a = tag->id;
    b = tag->topbase & 0xffff;
    if (tag->alias_id != 0)
      a = tag->alias_id;
    if (tag->alias_topbase != 0)
      b = tag->alias_topbase & 0xffff;
    if ((a != 0) && (a == b))
    {
      ioi1->class = OBJECT_SUB_OBJECT;
      ioi1->instance = tag->base;
      if (tag->alias_base != 0)
	ioi1->instance = tag->alias_base;

      if (ioi2 == NULL)
        ioi2 = malloc(sizeof(_ioi_data));

      if (ioi2 == NULL)
      {
	CELLERROR(10,"Could not allocate memory for ioi2 buffer");
        free(buff);
        free(msgbuff);
        free(head);
        free(cpf_buffer);
        free(sendbuff);
        free(ioi1);
	return -1;
      }
      memset(ioi2, 0, sizeof(_ioi_data));
      ioi2->class = OBJECT_CONFIG;
      ioi2->instance = tag->id;
      if (tag->alias_id != 0)
	ioi2->instance = tag->alias_id;
      ioi2->member = -1;
      ioi2->point = -1;
      ioi2->attribute = -1;
      ioi2->tagname = NULL;
    }

  while (1)
  {
    size = totalsize - (offset);
    if (size > CELL_PKT_SIZE)
      size = CELL_PKT_SIZE;
    dprint(DEBUG_TRACE,
	   "Write loop entered - totalsize = %d, size = %d, offset = %d\n",
	   totalsize, size, offset);
    memset(buff, 0, sizeof(_data_buffer));
    memset(msgbuff, 0, sizeof(_data_buffer));
    memset(head, 0, sizeof(_encaps_header));
    memset(cpf_buffer, 0, sizeof(_data_buffer));
    memset(sendbuff, 0, sizeof(_data_buffer));


    buff->len = 0;

    msgbuff->data[msgbuff->len++] = PUT_SINGLE_PROP;
    ioi(msgbuff, ioi1, ioi2, debug);
    msgbuff->data[msgbuff->len++] = ((offset) & 255);
    msgbuff->data[msgbuff->len++] = ((offset) / 0x100);
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = (size & 255);
    msgbuff->data[msgbuff->len++] = (size / 256);
    memcpy(msgbuff->data + msgbuff->len, tag->data + offset , size);
    msgbuff->len += size;
    if ((size & 1) == 0)
      msgbuff->data[msgbuff->len++] = 0;
//msgbuff->data[msgbuff->len++] = 0;

    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_BUILD, "Msgbuff = ");
      for (x = 0; x < msgbuff->len; x++)
	dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
      dprint(DEBUG_BUILD, "\n");
    }

    cpf_buffer->data[cpf_buffer->len++] = PDU_Unconnected_Send;
    ioiCM.class = CONNECTION_MANAGER;
    ioiCM.instance = FIRST_INSTANCE;
    ioiCM.member = -1;
    ioiCM.point = -1;
    ioiCM.attribute = -1;
    ioiCM.tagname = NULL;

    ioi(cpf_buffer, &ioiCM, NULL, debug);
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
      dprint(DEBUG_VALUES, "Write Object Range Value command returned an error.\n");
    }
    if (ret)
    {
      if (ioi2 != NULL)
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
      dprint(DEBUG_VALUES,
	     "Got good reply to Write Object Range Value Command - %d\n",
	     buff->overall_len);
      for (x = 44; x < buff->overall_len; x++)
	dprint(DEBUG_VALUES, "%02X ", buff->data[x]);
      dprint(DEBUG_VALUES, "\n");
    }
    if (size + offset  >= totalsize)
      break;
    offset += size;
  }

  if (ioi2 != NULL)
     free(ioi2);
  free(buff);
  free(msgbuff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(ioi1);
  dprint(DEBUG_TRACE, "write_object_range_value.c exited.\n");

  return 0;
}
