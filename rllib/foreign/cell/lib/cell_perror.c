
/* Created 05/03/2002 William Hays - CTI */


/* These global variables keep the last error information
   for use by the application to give meaningful error results
*/

#include <stdio.h>

int cell_errno;
char cell_err_msg[256];
int cell_line;
char cell_file[256];

void cell_perror(const char *s, int debug)
{
  if (s)
    fprintf(stderr, s);

  fprintf(stderr, "%d %s", cell_errno, cell_err_msg);

  if (debug)
    fprintf(stderr, " Line %d File %s\n", cell_line, cell_file);
  else
    fprintf(stderr, "\n");
}
