#include "libcell.h"

void cpf_data(short datatype, _data_buffer * cpf_data, _data_buffer * buff,
	      int debug)
{
  int x, len, start;
  start = buff->len;
  dprint(DEBUG_TRACE, "cpf_data entered - datatype = %d, buff->len = %d\n",
	 datatype, buff->len);
  len = 0;
  if (datatype != CPH_Null)
    len = cpf_data->len;
  buff->data[buff->len++] = datatype & 255;
  buff->data[buff->len++] = datatype / 0x100;
  buff->data[buff->len++] = len & 255;
  buff->data[buff->len++] = len / 0x100;
  if (datatype != CPH_Null)
  {
    memcpy(buff->data + buff->len, cpf_data->data, len);
    buff->len += len;
  }
  dprint(DEBUG_TRACE, "cpf_data exited.\n");
  dprint(DEBUG_DATA, "Copied %d bytes...\n", buff->len - start);
  dprint(DEBUG_DATA, "buff->len = %d\n", buff->len);
  if (debug != DEBUG_NIL)
  {
    for (x = start; x < buff->len; x++)
      dprint(DEBUG_BUILD, "%02X ", buff->data[x]);
    dprint(DEBUG_BUILD, "\n");
  }
  return;
}
