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
#include <QApplication>
#include <QIcon>
#include <QDir>

#include "mainwindow.h"
#include "opt.h"
#include "cutil.h"
#include "designer.h"
#include "widgetgenerator.h"

extern OPT opt;
QString l_copy            = "&Copy"; 
QString l_copy_plus_title = "Cop&y including title"; 

void usage()
{
  printf("####################################################\n");
  printf("# Develop pvserver for pvbrowser                   #\n");
  printf("####################################################\n");
  printf("usage: pvdevelop <-action=<action>> <-programming_language=language> <-h> project\n");
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
  printf("-programming_language=<Python>\n");
  exit(0);
}

void getargs(int ac, char **av)
{
  int i;
  const char *arg;
  char *cptr;
  char buf[1024];

  opt.xGrid = opt.yGrid = 3; // default grid for designer
  opt.murx = 0;              // default option for testing pvdevelop
  opt.ctrlPressed = 0;
  opt.su = 1;                // run pvs as root
  opt.xmax = 1280;           // default dimension for designer
  opt.ymax = 1024;
  strcpy(opt.backupLocation,"..");
  opt.arg_debug = 0;
  opt.arg_action[0] = '\0';
  opt.script = 0;
  opt.arg_mask_to_generate = -1;
  strcpy(opt.arg_project, "pvs");
  strcpy(opt.manual,"/opt/pvb/doc/index.html");
  for(i=1; i<ac; i++)
  {
    arg = av[i];
    if(strncmp(arg,"-debug",6) == 0)
    {
      opt.arg_debug = 1;
    }
    else if(strncmp(arg,"-action=",8) == 0)
    {
      sscanf(arg,"-action=%s",opt.arg_action);
    }
    else if(strncmp(arg,"-programming_language=",22) == 0)
    {
      sscanf(arg,"-programming_language=%s",buf);
      // add additional language here
      if     (strcmp(buf,"Python") == 0) opt.script = PV_PYTHON;
      else if(strcmp(buf,"Perl")   == 0) opt.script = PV_PERL;
      else if(strcmp(buf,"PHP")    == 0) opt.script = PV_PHP;
      else if(strcmp(buf,"Tcl")    == 0) opt.script = PV_TCL;
      else
      {
        printf("programming_language=%s not supported\n", buf);
      }
    }
    else if(arg[0] != '-')
    {
      if(strlen(arg) < sizeof(opt.arg_project)-1) 
      {
        strcpy(opt.arg_project,arg);
        cptr = strchr(opt.arg_project,'.');
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
  char buf[MAXOPT];
  int ret = 0;

  if(opt.arg_action[0]=='\0') return;
  if(opt.arg_project[0]=='\0')
  {
    printf("no project given\n");
    usage();
  }
  if(strcmp(opt.arg_action,"writeInitialProject") == 0)
  {
    generateInitialProject(opt.arg_project);
    sprintf(buf,"qmake=%s",opt.arg_project);
    action(buf);
  }
  else if(strcmp(opt.arg_action,"insertMask") == 0)
  {
    sprintf(buf,"insertMask=%s",opt.arg_project);
    action(buf);
  }
  else if(strcmp(opt.arg_action,"make") == 0)
  {
    action("make");
  }
  else if(strcmp(opt.arg_action,"makeModbus") == 0)
  {
    gmodbus("modbusdaemon.mkmodbus");
#ifdef PVUNIX
    ret = system("g++ -c modbusdaemon.cpp -o modbusdaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.so -pthread; g++ modbusdaemon.o /usr/lib/librllib.so -pthread -o modbusdaemon");
#else
    ret = system("start pvb_make_modbusdaemon.bat");
#endif
  }
  else if(strcmp(opt.arg_action,"makeSiemensTCP") == 0)
  {
    gsiemens("siemensdaemon.mksiemens");
#ifdef PVUNIX
    ret = system("g++ -c siemensdaemon.cpp -o siemensdaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.so -pthread; g++ siemensdaemon.o /usr/lib/librllib.so -pthread -o siemensdaemon");
#else
    ret = system("start pvb_make_siemensdaemon.bat");
#endif
  }
  else if(strcmp(opt.arg_action,"makePPI") == 0)
  {
    gppi("ppidaemon.mkppi");
#ifdef PVUNIX
    ret = system("g++ -c ppidaemon.cpp -o ppidaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.so -pthread; g++ ppidaemon.o /opt/pvb/rllib/lib/nodave.o /opt/pvb/rllib/lib/setport.o /usr/lib/librllib.so -pthread -o ppidaemon");
#else
    ret = system("start pvb_make_ppidaemon.bat");
#endif
  }
  else if(strcmp(opt.arg_action,"uncommentRLLIB") == 0)
  {
    sprintf(buf,"%s.pro",opt.arg_project);
    uncommentRllib(buf);
  }
  else if(strcmp(opt.arg_action,"uncommentModbus") == 0)
  {
    uncommentModbus();
  }
  else if(strcmp(opt.arg_action,"uncommentSiemensTCP") == 0)
  {
    uncommentSiemenstcp();
  }
  else if(strcmp(opt.arg_action,"uncommentPPI") == 0)
  {
    uncommentPpi();
  }
  else if(strcmp(opt.arg_action,"writeStartscript") == 0)
  {
    ret = writeStartscript(".", opt.arg_project);
  }
  else if(strncmp(opt.arg_action,"writeDimension:",15) == 0)
  {
    int xmax, ymax;

    xmax = 1280;
    ymax = 1024;
    sscanf(opt.arg_action,"writeDimension:%d:%d",&xmax,&ymax);
    sprintf(buf,"%s.pvproject",opt.arg_project);
    fout = fopen(buf,"w");
    if(fout == NULL)
    {
      printf("could not write %s\n",buf);
      exit(-1);
    }
    fprintf(fout,"target=%s\n",opt.arg_project);
    fprintf(fout,"xmax=%d\n",xmax);
    fprintf(fout,"ymax=%d\n",ymax);

    // add additional language here
    if(opt.script == PV_PYTHON)
    {
      fprintf(fout,"script=Python\n");
    }
    if(opt.script == PV_PERL)
    {
      fprintf(fout,"script=PERL\n");
    }
    if(opt.script == PV_PHP)
    {
      fprintf(fout,"script=PHP\n");
    }
    if(opt.script == PV_TCL)
    {
      fprintf(fout,"script=Tcl\n");
    }
    fclose(fout);
  }
  else if(strncmp(opt.arg_action,"importUi:",9) == 0)
  {
    sscanf(opt.arg_action,"importUi:%d", &opt.arg_mask_to_generate);
    if(opt.arg_mask_to_generate > 0)
    {
      char maskname[80];
      sprintf(maskname,"mask%d", opt.arg_mask_to_generate);
      Designer *designer = new Designer(maskname);
      sprintf(maskname,"mask%d.ui", opt.arg_mask_to_generate);
      importUi(maskname, designer);
      delete designer;
#ifdef PVUNIX
      action("make");
#endif
    }
    else printf("error: arg_mask_to_generate <= 0\n");
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
    readIniFile();

    Q_INIT_RESOURCE(pvdevelop);

    QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");
    perhapsDoAction();
    QIcon appIcon(":/images/app.png");
    app.setWindowIcon(appIcon);
    MainWindow mainWin;
    mainWin.showMaximized();
    if(opt.murx) QDir::setCurrent("/home/lehrig/temp/murx");
    return app.exec();
}
