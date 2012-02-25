//
// small windows debugging tool
//
#include <stdio.h>

int main()
{
  char buf[256*256];
  printf("--------------------------------------------------\n");
  printf("small tool for debugging window apps on windows\n");
  printf("usage:   your_app | pvmore\n");
  printf("example: pvbrowser -debug=2 pvbrowser.org | pvmore\n");
  printf("--------------------------------------------------\n");
  while(fgets(buf,sizeof(buf)-1,stdin) != NULL)
  {
    printf("%s", buf);
    fflush(stdout);
  }
}

