/****************************************************************************
**
** Copyright (C) 2000-2006 Lehrig Software Engineering.
**
** This file is part of the pvbrowser project.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "../pvbrowser/pvdefine.h"
#include <QtCore>
#include "opt.h"
#ifdef PVWIN32
#include <windows.h>
#endif

OPT opt;

const char *inifile()
{
  static char name[MAXOPT];

  name[0] = '\0';
#ifdef PVUNIX
  strcpy(name,getenv("HOME"));
  strcat(name,"/.pvdevelop.ini");
#endif
#ifdef __VMS
  strcpy(name,"sys$login:pvdevelop.ini");
#endif
#ifdef PVWIN32
  ExpandEnvironmentStrings("%USERPROFILE%",name,sizeof(name)-1);
  if(strcmp(name,"%USERPROFILE%") == 0) strcpy(name,"C:");
  strcat(name,"\\pvdevelop.ini");
#endif
  return name;
}

const char *readIniFile()
{
FILE *fp;
char buf[MAXOPT];
char *cptr;

  while(1)
  {
    fp = fopen(inifile(),"r");
    if(fp != NULL)
    {
      while( fgets(buf,sizeof(buf)-1,fp) != NULL )
      {
        cptr = strchr(buf,'\n');
        if(cptr != NULL) *cptr = '\0';
        if(strncmp(buf,"manual=",7) == 0)
        {
          strcpy(opt.manual,&buf[7]);
#ifdef PVWIN32
          ExpandEnvironmentStrings(&buf[7],opt.manual,sizeof(opt.manual)-1);
#endif
        }
        else if(strncmp(buf,"xGrid=",6) == 0)
        {
          sscanf(buf,"xGrid=%d",&opt.xGrid);
          if(opt.xGrid <= 0) opt.xGrid = 1;
        }
        else if(strncmp(buf,"yGrid=",6) == 0)
        {
          sscanf(buf,"yGrid=%d",&opt.yGrid);
          if(opt.yGrid <= 0) opt.yGrid = 1;
        }
        else if(strncmp(buf,"su=",3) == 0)
        {
          sscanf(buf,"su=%d",&opt.su);
        }
        else if(strncmp(buf,"backupLocation=",15) == 0)
        {
          strcpy(opt.backupLocation,&buf[15]);
        }
        else if(strncmp(buf,"murx=",5) == 0)
        {
          sscanf(buf,"murx=%d",&opt.murx);
        }
      }
      fclose(fp);
      return NULL;
    }
    else // write a default initialisation file
    {
      fp = fopen(inifile(),"w");
      if(fp != NULL)
      {
        fprintf(fp,"xGrid=3\n");
        fprintf(fp,"yGrid=3\n");
#ifdef PVWIN32
        fprintf(fp,"manual=%%PVBDIR%%\\doc\\index.html\n");
#else
        fprintf(fp,"manual=/opt/pvb/doc/index.html\n");
        fprintf(fp,"su=0 # run pvs as root 0|1\n");
        fprintf(fp,"backupLocation=..\n");
#endif
        fclose(fp);
      }
      else
      {
        return "Could not write initialization file";
      }
    }
  }
}
