// start pvserver + pvbrowser as normal application
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rlcutil.h"
#include "rlinifile.h"
#include "rlstring.h"

#ifdef _WIN32
#define SEP "\\" 
#else
#define SEP "/"
#endif

int main(int ac, char **av)
{
  if(ac < 2)
  {
    printf("usage: start_pvbapp file.ini\n");
    printf("OR\n");
    printf("usage: start_pvbapp command arguments...\n");
    return -1;
  }
  if(strstr(av[1],".ini") == NULL && strstr(av[1],".INI") == NULL) 
  {
    rlString cmd;
    for(int i=1; i<ac; i++)
    {
      cmd += av[i];
      cmd += " ";
    }
    return rlsystem(cmd.text());
  }

  char command[4096];
  rlIniFile ini;
  if(ini.read(av[1]) != 0) 
  {
    printf("could not read %s\n", av[1]);
    return -1;
  }  

  // create command for preparation
  if(strcmp(ini.text("prepare","start"), "1") == 0) // test if we want to start a data acquisition for example
  {
    strcpy(command, ini.text("prepare","command"));
    //printf("command=%s\n", command);
    rlsystem(command); // start it
  }

  // create command for starting pvserver
  strcpy(command, ini.text("pvserver","path"));
  strcat(command, SEP);
  strcat(command, ini.text("pvserver","executable"));
  strcat(command, " \"-cd=");
  strcat(command, ini.text("pvserver","path"));
  strcat(command, "\" ");
  strcat(command, ini.text("pvserver","parameters"));
  //printf("command=%s\n", command);
  rlsystem(command); // start the pvserver in background

  // create command for starting pvbrowser
  if(strcmp(ini.text("pvbrowser","start"), "1") == 0) // test if we want to start pvbrowser
  {
    strcpy(command, ini.text("pvbrowser","path"));
    strcat(command, SEP);
    strcat(command, ini.text("pvbrowser","executable"));
    strcat(command, " ");
    strcat(command, ini.text("pvbrowser","parameters"));
    //printf("command=%s\n", command);
    rlsystem(command); // start pvbrowser
  }
  return 0;
}
