#include "libcell.h"

void cpf(short addrtype, _data_buffer * addrbuff, short datatype,
	 _data_buffer * databuff, _data_buffer * buff, int debug)
{
  dprint(DEBUG_TRACE, "cpf entered.  Buff->len = %d\n", buff->len);
  buff->data[buff->len++] = 2;
  buff->len++;
  cpf_data(addrtype, addrbuff, buff, debug);
  cpf_data(datatype, databuff, buff, debug);
  dprint(DEBUG_TRACE, "cpf exited.\n");
  return;
}
