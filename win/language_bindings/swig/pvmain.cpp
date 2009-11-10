// Just a dummy

#include <stdlib.h>
#include "../../../pvserver/processviewserver.h"

int pvMain(PARAM *p)
{
  if(p==NULL) return 0;
  return 1;
}

int *new_int(int ivalue)
{
  int *i = (int *) malloc(sizeof(ivalue));
  *i = ivalue;
  return i;
}
int get_int(int *i)
{
  return *i;
}

void delete_int(int *i)
{
  free(i);
}
