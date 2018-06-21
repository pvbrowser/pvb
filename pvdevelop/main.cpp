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
#include <locale.h>
#include "../pvbrowser/pvdefine.h"
#include "../pvbrowser/opt.h"
#include <QApplication>
#include <QIcon>
#include <QDir>

#include "mainwindow.h"
#include "opt.h"
#include "cutil.h"
#include "designer.h"
#include "widgetgenerator.h"

extern OPT_DEVELOP opt_develop;
QString l_copy            = "&Copy"; 
QString l_copy_plus_title = "Cop&y including title"; 

void usage()
{
  printf("####################################################\n");
  printf("# Develop pvserver for pvbrowser                   #\n");
  printf("####################################################\n");
  printf("usage: pvdevelop <-action=<action>> <-programming_language=language> <-fake_qmake> <-h> project\n");
  printf("example: pvdevelop pvs\n");
  printf("example: pvdevelop -action=writeInitialProject pvs\n");
  printf("-action=writeInitialProject\n");
  printf("-action=insertMask\n");
  printf("-action=make\n");
  printf("-action=makeModbus\n");
  printf("-action=makeSiemensTCP\n");
  printf("-action=makePPI\n");
  printf("-action=uncommentRLLIB\n");
  printf("-action=uncommentModbus\n");
  printf("-action=uncommentSiemensTCP\n");
  printf("-action=uncommentPPI\n");
  printf("-action=writeStartscript\n");
  printf("-action=writeDimension:<xmax>:<ymax>\n");
  printf("-action=importUi:<masknumber>\n");
  printf("-action=exportUi:<masknumber>\n");
  printf("-action=designerUi:<masknumber>\n");
  printf("-action=dumpTranslations\n");
  printf("-programming_language=<Lua|Python>\n");
  exit(0);
}

void getargs(int ac, char **av)
{
  int i;
  const char *arg;
  char *cptr;
  char buf[1024];

  opt_develop.xGrid = opt_develop.yGrid = 3; // default grid for designer
  opt_develop.murx = 0;              // default option for testing pvdevelop
  opt_develop.ctrlPressed = 0;
  opt_develop.su = 1;                // run pvs as root
  opt_develop.xmax = 1280;           // default dimension for designer
  opt_develop.ymax = 1024;
  strcpy(opt_develop.backupLocation,"..");
  opt_develop.arg_debug = 0;
  opt_develop.arg_action[0] = '\0';
  opt_develop.script = 0;
  opt_develop.arg_mask_to_generate = -1;
  strcpy(opt_develop.arg_project, "pvs");
  strcpy(opt_develop.manual,"/opt/pvb/doc/index.html");
  strcpy(opt_develop.fake_qmake, "");
  for(i=1; i<ac; i++)
  {
    arg = av[i];
    if(strncmp(arg,"-debug",6) == 0)
    {
      opt_develop.arg_debug = 1;
    }
    else if(strncmp(arg,"-action=",8) == 0)
    {
      sscanf(arg,"-action=%s",opt_develop.arg_action);
    }
    else if(strncmp(arg,"-programming_language=",22) == 0)
    {
      sscanf(arg,"-programming_language=%s",buf);
      // add additional language here
      if     (strcmp(buf,"Python") == 0) opt_develop.script = PV_PYTHON;
      else if(strcmp(buf,"Perl")   == 0) opt_develop.script = PV_PERL;
      else if(strcmp(buf,"PHP")    == 0) opt_develop.script = PV_PHP;
      else if(strcmp(buf,"Tcl")    == 0) opt_develop.script = PV_TCL;
      else if(strcmp(buf,"Lua")    == 0) opt_develop.script = PV_LUA;
      else
      {
        printf("programming_language=%s not supported\n", buf);
      }
    }
    else if(strncmp(arg,"-fake",5) == 0)
    {
      strcpy(opt_develop.fake_qmake,"-fake");
    }
    else if(arg[0] != '-')
    {
      if(strlen(arg) < sizeof(opt_develop.arg_project)-1) 
      {
        strcpy(opt_develop.arg_project,arg);
        cptr = strchr(opt_develop.arg_project,'.');
        if(cptr != NULL) *cptr = '\0';
      }
    }
    else if(strncmp(arg,"-",1) == 0)
    {
      usage();
    }
  }
  readProject();
}

int gppi(const char *name);
int gsiemens(const char *name);
int gmodbus(const char *name);

void  perhapsDoAction()
{
  FILE *fout;
  char buf[2*MAXOPT_DEVELOP];
  int ret = 0;

  if(opt_develop.arg_action[0]=='\0') return;
  if(opt_develop.arg_project[0]=='\0')
  {
    printf("no project given\n");
    usage();
  }
  if(strcmp(opt_develop.arg_action,"writeInitialProject") == 0)
  {
    generateInitialProject(opt_develop.arg_project);
    sprintf(buf,"qmake=%s",opt_develop.arg_project);
    action(buf);
  }
  else if(strcmp(opt_develop.arg_action,"insertMask") == 0)
  {
    sprintf(buf,"insertMask=%s",opt_develop.arg_project);
    action(buf);
  }
  else if(strcmp(opt_develop.arg_action,"make") == 0)
  {
    action("make");
  }
  else if(strcmp(opt_develop.arg_action,"makeModbus") == 0)
  {
    gmodbus("modbusdaemon.mkmodbus");
#ifdef PVUNIX
    ret = system("g++ -c modbusdaemon.cpp -o modbusdaemon.o -I/opt/pvb/rllib/lib  ; g++ modbusdaemon.o /usr/lib/librllib.so -lpthread -o modbusdaemon");
#else
    ret = system("start pvb_make_modbusdaemon.bat");
#endif
  }
  else if(strcmp(opt_develop.arg_action,"makeSiemensTCP") == 0)
  {
    gsiemens("siemensdaemon.mksiemens");
#ifdef PVUNIX
    ret = system("g++ -c siemensdaemon.cpp -o siemensdaemon.o -I/opt/pvb/rllib/lib  ; g++ siemensdaemon.o /usr/lib/librllib.so -lpthread -o siemensdaemon");
#else
    ret = system("start pvb_make_siemensdaemon.bat");
#endif
  }
  else if(strcmp(opt_develop.arg_action,"makePPI") == 0)
  {
    gppi("ppidaemon.mkppi");
#ifdef PVUNIX
    ret = system("g++ -c ppidaemon.cpp -o ppidaemon.o -I/opt/pvb/rllib/lib  ; g++ ppidaemon.o /opt/pvb/rllib/lib/nodave.o /opt/pvb/rllib/lib/setport.o /usr/lib/librllib.so -lpthread -o ppidaemon");
#else
    ret = system("start pvb_make_ppidaemon.bat");
#endif
  }
  else if(strcmp(opt_develop.arg_action,"uncommentRLLIB") == 0)
  {
    sprintf(buf,"%s.pro",opt_develop.arg_project);
    uncommentRllib(buf);
  }
  else if(strcmp(opt_develop.arg_action,"uncommentModbus") == 0)
  {
    uncommentModbus();
  }
  else if(strcmp(opt_develop.arg_action,"uncommentSiemensTCP") == 0)
  {
    uncommentSiemenstcp();
  }
  else if(strcmp(opt_develop.arg_action,"uncommentPPI") == 0)
  {
    uncommentPpi();
  }
  else if(strcmp(opt_develop.arg_action,"writeStartscript") == 0)
  {
    ret = writeStartscript(".", opt_develop.arg_project);
  }
  else if(strncmp(opt_develop.arg_action,"writeDimension:",15) == 0)
  {
    int xmax, ymax;

    xmax = 1280;
    ymax = 1024;
    sscanf(opt_develop.arg_action,"writeDimension:%d:%d",&xmax,&ymax);
    sprintf(buf,"%s.pvproject",opt_develop.arg_project);
    fout = fopen(buf,"w");
    if(fout == NULL)
    {
      printf("could not write %s\n",buf);
      exit(-1);
    }
    fprintf(fout,"target=%s\n",opt_develop.arg_project);
    fprintf(fout,"xmax=%d\n",xmax);
    fprintf(fout,"ymax=%d\n",ymax);

    // add additional language here
    if(opt_develop.script == PV_PYTHON)
    {
      fprintf(fout,"script=Python\n");
    }
    if(opt_develop.script == PV_PERL)
    {
      fprintf(fout,"script=PERL\n");
    }
    if(opt_develop.script == PV_PHP)
    {
      fprintf(fout,"script=PHP\n");
    }
    if(opt_develop.script == PV_TCL)
    {
      fprintf(fout,"script=Tcl\n");
    }
    if(opt_develop.script == PV_LUA)
    {
      fprintf(fout,"script=Lua\n");
    }
    fclose(fout);
  }
  else if(strncmp(opt_develop.arg_action,"importUi:",9) == 0)
  {
    if(strncmp(opt_develop.arg_action,"importUi:mask",13) == 0)
    {
      sscanf(opt_develop.arg_action,"importUi:mask%d", &opt_develop.arg_mask_to_generate);
    }
    else
    {
      const char *cptr = &opt_develop.arg_action[9];
      if(isdigit(*cptr))
      {
        sscanf(cptr,"%d",&opt_develop.arg_mask_to_generate);
      }
      else
      {
        cptr = strstr(cptr,"mask");
        if(cptr != NULL)
        {
          sscanf(cptr,"mask%d",&opt_develop.arg_mask_to_generate);
        }
      }
    }  
    if(opt_develop.arg_mask_to_generate > 0)
    {
      char maskname[80];
      sprintf(maskname,"mask%d", opt_develop.arg_mask_to_generate);
      Designer *designer = new Designer(maskname);
      sprintf(maskname,"mask%d.ui", opt_develop.arg_mask_to_generate);
      importUi(maskname, designer);
      delete designer;
#ifdef PVUNIX
      action("make");
#endif
    }
    else printf("error: arg_mask_to_generate <= 0\n");
  }
  else if(strncmp(opt_develop.arg_action,"exportUi:",9) == 0)
  {
    if(strncmp(opt_develop.arg_action,"exportUi:mask",13) == 0)
    {
      sscanf(opt_develop.arg_action,"exportUi:mask%d", &opt_develop.arg_mask_to_generate);
    }
    else
    {
      const char *cptr = &opt_develop.arg_action[9];
      if(isdigit(*cptr))
      {
        sscanf(cptr,"%d",&opt_develop.arg_mask_to_generate);
      }
      else
      {
        cptr = strstr(cptr,"mask");
        if(cptr != NULL)
        {
          sscanf(cptr,"mask%d",&opt_develop.arg_mask_to_generate);
        }
      }
    }  
    if(opt_develop.arg_mask_to_generate > 0)
    {
      printf("ATTENTION: You are not allowed to export an ui file, define a layout management in Qt Designer and import the ui file again to pvdevelop\n");
      printf("           You must define a possible layout management within pvdevelop instead.\n");
      export_ui(opt_develop.arg_mask_to_generate);
    }
    else printf("error: arg_mask_to_generate <= 0\n");
  }
  else if(strncmp(opt_develop.arg_action,"designerUi:",11) == 0)
  {
    if(strncmp(opt_develop.arg_action,"designerUi:mask",15) == 0)
    {
      sscanf(opt_develop.arg_action,"designerUi:mask%d", &opt_develop.arg_mask_to_generate);
    }
    else
    {
      const char *cptr = &opt_develop.arg_action[11];
      if(isdigit(*cptr))
      {
        sscanf(cptr,"%d",&opt_develop.arg_mask_to_generate);
      }
      else
      {
        cptr = strstr(cptr,"mask");
        if(cptr != NULL)
        {
          sscanf(cptr,"mask%d",&opt_develop.arg_mask_to_generate);
        }
      }
    }  
    if(opt_develop.arg_mask_to_generate > 0)
    {
      char maskname[80];
      sprintf(maskname,"mask%d", opt_develop.arg_mask_to_generate);
      Designer *designer = new Designer(maskname);
      sprintf(maskname,"mask%d.ui", opt_develop.arg_mask_to_generate);
      printf("ATTENTION: You are not allowed to export an ui file, define a layout management in Qt Designer and import the ui file again to pvdevelop\n");
      printf("           You must define a possible layout management within pvdevelop instead.\n");
      export_ui(opt_develop.arg_mask_to_generate);
      char cmd[1024];
#ifdef PVWIN32      
      sprintf(cmd,"wait \"%%QTDIR%%\\bin\\designer.exe\" %s", maskname);
      mysystem(cmd);
#else      
      sprintf(cmd,"designer %s", maskname);
      system(cmd);
#endif      
      importUi(maskname, designer);
      delete designer;
#ifdef PVUNIX
      action("make");
#endif
      printf("ATTENTION: You are not allowed to export an ui file, define a layout management in Qt Designer and import the ui file again to pvdevelop\n");
      printf("           You must define a possible layout management within pvdevelop instead.\n");
    }
    else printf("error: arg_mask_to_generate <= 0\n");
  }
  else if(strcmp(opt_develop.arg_action,"dumpTranslations") == 0)
  {
    action("dumpTranslations");
  }
  else
  {
    printf("unknown action\n");
    usage();
  }
  if(ret != 0)
  {
    printf("error\n");
  }
  exit(0);
}

int main(int argc, char *argv[])
{
    setlocale(LC_NUMERIC, "C");
    getargs(argc,argv);
    setDefaultOptions();
    readIniFile();

    Q_INIT_RESOURCE(pvdevelop);

    QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");
    perhapsDoAction();
    QIcon appIcon(":/images/app.png");
    app.setWindowIcon(appIcon);
    MainWindow mainWin;
    mainWin.showMaximized();
    if(opt_develop.murx) QDir::setCurrent("/home/lehrig/temp/murx");
    return app.exec();
}
