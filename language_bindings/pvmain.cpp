// Just a dummy

#include <stdio.h>
#include <stdlib.h>
#include "../pvserver/processviewserver.h"

PARAM *getParam(unsigned long p)
{
  PARAM *param = (PARAM *) p;
  //printf("getParam p = %d\n", param);
  //printf("getParam p->s = %d\n", param->s);
  return param;
}

int pvQImageScript(PARAM *p, int id, int parent, const char *imagename)
{
  pvQImage(p,id,parent,imagename,NULL,NULL,NULL);
  return 0;
}

#ifndef LUA
int pvMain(PARAM *p)
{
  if(p==NULL) return 0;
  return 1;
}
#endif

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

