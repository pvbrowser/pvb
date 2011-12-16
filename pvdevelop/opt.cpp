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
#include "../pvbrowser/opt.h"
#include <QtCore>
#include "opt.h"
#ifdef PVWIN32
#include <windows.h>
#endif

OPT         opt;
OPT_DEVELOP opt_develop;

const char *inifile()
{
  static char name[MAXOPT_DEVELOP];

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
char buf[MAXOPT_DEVELOP];
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
          strcpy(opt_develop.manual,&buf[7]);
#ifdef PVWIN32
          ExpandEnvironmentStrings(&buf[7],opt_develop.manual,sizeof(opt_develop.manual)-1);
#endif
        }
        else if(strncmp(buf,"xGrid=",6) == 0)
        {
          sscanf(buf,"xGrid=%d",&opt_develop.xGrid);
          if(opt_develop.xGrid <= 0) opt_develop.xGrid = 1;
        }
        else if(strncmp(buf,"yGrid=",6) == 0)
        {
          sscanf(buf,"yGrid=%d",&opt_develop.yGrid);
          if(opt_develop.yGrid <= 0) opt_develop.yGrid = 1;
        }
        else if(strncmp(buf,"su=",3) == 0)
        {
          sscanf(buf,"su=%d",&opt_develop.su);
        }
        else if(strncmp(buf,"backupLocation=",15) == 0)
        {
          strcpy(opt_develop.backupLocation,&buf[15]);
        }
        else if(strncmp(buf,"murx=",5) == 0)
        {
          sscanf(buf,"murx=%d",&opt_develop.murx);
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

void setDefaultOptions()
{
  opt.arg_debug = 1;
  opt.port=5050;
  opt.sshport=50500;
  opt.zoom=100;
  opt.fontzoom=100;
  opt.autoreconnect=0;
  opt.exitpassword=0;
  opt.menubar=1;
  opt.toolbar=1;
  opt.statusbar=1;
  opt.scrollbars=1;
  opt.fullscreen=0;
  opt.maximized=0;
  opt.tabs_above_toolbar=0;
  opt.echo_table_updates=0;
#if QT_VERSION >= 0x040601
  opt.use_webkit_for_svg=1;
#else
  opt.use_webkit_for_svg=0;
#endif
  opt.enable_webkit_plugins=0;
  opt.temp[0] = '\0';
  opt.customlogo[0] = '\0';
  opt.newwindow[0] = '\0';
  opt.ssh[0] = '\0';
  opt.i_have_started_servers = 0;
  opt.view_audio[0] = '\0';
  opt.view_video[0] = '\0';
  opt.view_pdf[0] = '\0';
  opt.view_img[0] = '\0';
  opt.view_svg[0] = '\0';
  opt.view_txt[0] = '\0';
  opt.view_html[0] = '\0';
  opt.initialhost[0] = '\0';
  opt.language[0] = '\0';
  opt.codec = pvbUTF8; // strongly advised to use pvbUTF8
  opt.closed = 0;
  opt.cookies = 1; // YES
  strcpy(opt.manual,"index.html");
}

