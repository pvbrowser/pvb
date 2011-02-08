/* File : language_binding.i */
%module pv
%{
/* Put headers and other declarations here */
#include "../pvserver/processviewserver.h"
#include "sql/qtdatabase.h"
PARAM *getParam(unsigned long p);
int pvQImageScript(PARAM *p, int id, int parent, const char *imagename);
int *new_int(int ivalue);
int  get_int(int *i);
void delete_int(int *i);
%}
%include "../pvserver/processviewserver.h"
%include "sql/qtdatabase.h"
PARAM *getParam(unsigned long p);
int pvQImageScript(PARAM *p, int id, int parent, const char *imagename);
int *new_int(int ivalue);
int  get_int(int *i);
void delete_int(int *i);
