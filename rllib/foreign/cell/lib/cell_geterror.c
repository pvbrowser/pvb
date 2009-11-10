
/* Created 06/20/2002 William Hays - CTI */


/* These global variables keep the last error information
   for use by the application to give meaningful error results
*/

#include <stdio.h>
#include <string.h>

extern int cell_errno;
extern char cell_err_msg[256];
extern int cell_line;
extern char cell_file[256];

void cell_geterror(int *p_errno, char *err_msg, int debug)
{
  sprintf(err_msg, "%d %s", cell_errno, cell_err_msg);
  *p_errno = cell_errno;

  if (debug)
  {
    char ac_tmp[128];
    sprintf(ac_tmp, " Line %d File %s\n", cell_line, cell_file);
    strcat(err_msg,ac_tmp);
  }
}
