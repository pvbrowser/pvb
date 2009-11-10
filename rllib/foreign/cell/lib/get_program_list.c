
/* Revised 5/6/02 William Hays - CTI 

   5/7/2002 WRH  Limited program elements to CELL_MAX_PROGS
*/

#include "libcell.h"

int get_program_list(_comm_header * comm, _path * path, _prog_list * progs,
		      int debug)
{
  int x, count, ret =0;
  _data_buffer *buff;
  _encaps_header *head;
  _data_buffer *cpf_buffer;
  _data_buffer *sendbuff;
  _data_buffer *msgbuff;
  _ioi_data *ioi1;

  dprint(DEBUG_TRACE, "get_object_list.c entered.\n");
  if (path == NULL)
  {
    CELLERROR(1,"path structure not allocated");
    return -1;
  }

  if (progs == NULL)
  {
    CELLERROR(2,"tags structure not allocated");
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

  progs->count = 0;
  while (1)
  {
    memset(buff, 0, sizeof(_data_buffer));
    memset(msgbuff, 0, sizeof(_data_buffer));
    memset(head, 0, sizeof(_encaps_header));
    memset(cpf_buffer, 0, sizeof(_data_buffer));
    memset(sendbuff, 0, sizeof(_data_buffer));
    memset(ioi1, 0, sizeof(_ioi_data));

    buff->len = 0;

    msgbuff->data[msgbuff->len++] = LIST_ALL_OBJECTS;
    ioi1->class = OBJECT_SUB_OBJECT;
    ioi1->instance = BASE_INSTANCE;
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
      dprint(DEBUG_VALUES, "Get Program List command returned an error.\n");
      CELLERROR(9,"Get Program List command returned an error");
    }
    if (ret)
    {
      int i;
      for (i=0;i<progs->count;i++)
        free(progs->prog[i]);
      free(buff);
      free(msgbuff);
      free(head);
      free(cpf_buffer);
      free(sendbuff);
      free(ioi1);
      return -1;
    }

    if (debug != DEBUG_NIL)
    {
      dprint(DEBUG_BUILD, "Got good reply to Get Program List Command - %d\n",
	     buff->overall_len);
      for (x = 44; x < buff->overall_len; x++)
	dprint(DEBUG_BUILD, "%02X ", buff->data[x]);
      dprint(DEBUG_BUILD, "\n");
    }
    count = 0;
    for (x = 44; x < buff->overall_len; x += 4)
    {
      if (progs->count + count >= CELL_MAX_PROGS)
      {
        int i;
        /* Sorry, but no more space in the array */
	CELLERROR(20,"Maximum supported CELL_MAX_PROGS reached");

        for (i=0;i<progs->count+count;i++)
          free(progs->prog[i]);

        free(buff);
        free(msgbuff);
        free(head);
        free(cpf_buffer);
        free(sendbuff);
        free(ioi1);
	return -1;
      }
      
      progs->prog[progs->count + count] = 0;
      dprint(DEBUG_BUILD, "Allocating memory for program list entry %d\n",
	     progs->count + count);
      progs->prog[progs->count + count] = malloc(sizeof(_prog_detail));
      if (progs->prog[progs->count + count] == NULL)
      {
        int i;
        char ac_dmesg[80];
	sprintf(ac_dmesg,"Could not allocate memory for program list entry %d.\n",
	       progs->count + count);
        CELLERROR(10,ac_dmesg);

        for (i=0;i<progs->count+count;i++)
          free(progs->prog[i]);
        free(buff);
        free(msgbuff);
        free(head);
        free(cpf_buffer);
        free(sendbuff);
        free(ioi1);
	return -1;
      }

      dprint(DEBUG_BUILD, "Memory allocated: %p\n",
	     progs->prog[progs->count + count]);
      memset(progs->prog[progs->count + count], 0, sizeof(_prog_detail));
      progs->prog[progs->count + count]->base =
	buff->data[x] +
	(buff->data[x + 1] * 0x100) +
	(buff->data[x + 2] * 0x10000) + (buff->data[x + 3] * 0x1000000);
      count++;
    }
    progs->count += count;
    if (buff->data[42] != 6)
      break;
  }

  free(ioi1);
  free(sendbuff);
  free(cpf_buffer);
  free(head);
  free(msgbuff);
  free(buff);
  dprint(DEBUG_TRACE, "Exiting get_program_list.c\n");
  return 0;
}
