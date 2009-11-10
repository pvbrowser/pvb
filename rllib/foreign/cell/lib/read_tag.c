
/* Revised 5/3/02 William Hays - CTI */

#include "libcell.h"

int read_tag(_comm_header * comm, _path * path, char *program, 
          char *tagname, _tag_detail *tag,  int debug)
{
  int x, size, totalsize;
  int offset = 0;
  unsigned long a, b;
  int ret=0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;
  _ioi_data *ioi2=NULL;
  _ioi_data ioiCM;  //*** new

  dprint(DEBUG_TRACE, "read_tag.c entered.\n");
  size = 0;


  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
    return -1;
  }

  if (tagname == NULL)
    {
    CELLERROR (2,"You need to specify a tag");
    return -1;
    }  

  buff = malloc(sizeof(_data_buffer));

  if (buff == NULL)
  {
    CELLERROR(3,
       "Could not allocate memory for data buffer");
    return -1;
  }

  msgbuff = malloc(sizeof(_data_buffer));
  if (msgbuff == NULL)
  {
    CELLERROR(4,
      "Could not allocate memory for msgdata buffer");
    free(buff);
    return -1;
  }

  head = malloc(sizeof(_encaps_header));
  if (head == NULL)
  {
    CELLERROR(5,
      "Could not allocate memory for Encaps Header");
    free(msgbuff);
    free(buff);
    return -1;
  }

  cpf_buffer = malloc(sizeof(_data_buffer));
  if (cpf_buffer == NULL)
  {
    CELLERROR(6,
      "Could not allocate memory for cpf_buffer");
    free(msgbuff);
    free(buff);
    free(head);
    return -1;
  }

  sendbuff = malloc(sizeof(_data_buffer));
  if (sendbuff == NULL)
  {
    CELLERROR(7,
      "Could not allocate memory for send buffer - read_tag.c");
    free(msgbuff);
    free(buff);
    free(head);
    free(cpf_buffer);
    return -1;
  }

  ioi1 = malloc(sizeof(_ioi_data));
  if (ioi1 == NULL)
  {
    CELLERROR(8,
      "Could not allocate memory for ioi1 buffer - read_tag.c");
    free(msgbuff);
    free(buff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    return -1;
  }

  tag->datalen = 0;
  totalsize = 0;
  buff->len = 0;
    memset(ioi1, 0, sizeof(_ioi_data));
    ioi1->class = OBJECT_CONFIG;
//    ioi1->class = -1;
    ioi1->instance = 1;
    ioi1->member = -1;
    ioi1->point = -1;
    ioi1->attribute = -1;
    ioi1->tagname = tagname;
    if (program != NULL)
      {
      if (ioi2 == NULL)
        ioi2 = malloc(sizeof(_ioi_data));

      if (ioi2 == NULL)
      {
  CELLERROR(20,
    "Could not allocate memory for ioi2 buffer - read_tag.c");
  free(msgbuff);
  free(buff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(ioi1);
  return -1;
      }
      memset(ioi2, 0, sizeof(_ioi_data));
      ioi2->class = OBJECT_CONFIG;
//      ioi2->class = -1;
      //ioi2->instance = -1
      ioi2->member = -1;
      ioi2->point = -1;
      ioi2->attribute = -1;
      ioi2->tagname = tagname;
//      memset (ioi1->tagname,0,strlen(tagname));
      ioi1->tagname = program;
    }

    
    size = 32;
    if (size > CELL_PKT_SIZE)
      size = CELL_PKT_SIZE;

    dprint(DEBUG_TRACE,
     "Loop entered.  totalsize = %d   size = %d\n",
     totalsize, size);

    memset(buff, 0, sizeof(_data_buffer));
    memset(msgbuff, 0, sizeof(_data_buffer));
    memset(head, 0, sizeof(_encaps_header));
    memset(cpf_buffer, 0, sizeof(_data_buffer));
    memset(sendbuff, 0, sizeof(_data_buffer));

//      msgbuff->data[msgbuff->len++] = DATA_TYPE_CONFIG;
    msgbuff->data[msgbuff->len++] = GET_SINGLE_PROP;
    ioi(msgbuff, ioi1, ioi2, debug);

/*
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = offset & 255;
    msgbuff->data[msgbuff->len++] = offset / 0x100;
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = 0;
    msgbuff->data[msgbuff->len++] = size & 255;
    msgbuff->data[msgbuff->len++] = size / 256;
*/
    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_BUILD, "Msgbuff = ");
      for (x = 0; x < msgbuff->len; x++)
  dprint(DEBUG_BUILD, "%02X ", msgbuff->data[x]);
      dprint(DEBUG_BUILD, "\n");
    }

    cpf_buffer->data[cpf_buffer->len++] = PDU_Unconnected_Send;
    ioiCM.class = CONNECTION_MANAGER; //*** changed
    ioiCM.instance = FIRST_INSTANCE; //*** changed
    ioiCM.member = -1; //*** changed
    ioiCM.point = -1; //*** changed
    ioiCM.attribute = -1; //*** changed
    ioiCM.tagname = NULL; //*** changed

    ioi(cpf_buffer, &ioiCM, NULL, debug); //*** changed
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
      dprint(DEBUG_VALUES, "Read Tag command returned an error.\n");
      CELLERROR(20, "Read Tag command returned an error");
    }
    if (ret)
    {
      if (ioi2 != NULL)
        free(ioi2);
      free(msgbuff);
      free(buff);
      free(head);
      free(cpf_buffer);
      free(sendbuff);
      free(ioi1);
      return -1;
    }

//buff = buff + ENCAPS_Header_Length;
    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_BUILD,
       "Got good reply to Read Tag Command - %d\n",
       buff->overall_len);
      for (x = 44; x < buff->overall_len; x++)
  dprint(DEBUG_BUILD, "%02X ", buff->data[x]);
      dprint(DEBUG_BUILD, "\n");
    }
  if ((x = buff->data[42])) //*** added all the way down to ...
    {
    if (ioi2 != NULL)
      free(ioi2);

    free(msgbuff);
    free(buff);
    free(head);
    free(cpf_buffer);
    free(sendbuff);
    free(ioi1);

    dprint(DEBUG_TRACE, "read_tag.c exited. datalen = %d\n", 
tag->datalen);

    return x;
    }         //***  here
    dprint(DEBUG_TRACE, "size = %d\n", size);
    memcpy(&tag->data[0], buff->data + 44, size);


    tag->datalen += size;

  x = buff->data[42];
  if (ioi2 != NULL)
    free(ioi2);

  free(msgbuff);
  free(buff);
  free(head);
  free(cpf_buffer);
  free(sendbuff);
  free(ioi1);

  dprint(DEBUG_TRACE, "read_tag.c exited. datalen = %d\n", 
tag->datalen);

  return x;
}

