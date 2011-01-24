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
#include "pvdefine.h"
#include <QApplication>
#include <QIcon>
#include <QSplashScreen>
#include <QMessageBox>
#include "images/splash.xpm"
#include "mainwindow.h"
#include "opt.h"
#include "tcputil.h"

extern OPT opt;

void init(int ac, char **av)
{
  setlocale(LC_NUMERIC, "C");
  opt.arg_debug    = 0;     // default command line args
  opt.arg_localini = 0;
  opt.arg_ini[0]   = '\0';
  opt.arg_font[0]  = '\0';
  opt.arg_host[0]  = '\0';
  opt.arg_disable  = 0;
  opt.arg_x        = -1;
  opt.arg_y        = -1;
  opt.arg_w        = -1;
  opt.arg_h        = -1;
  opt.arg_log      = 0;
  opt.arg_murn     = 0;
  for(int i=0; i<ac; i++)  // read command line args
  {
    if(strncmp(av[i],"-h",2) == 0 || strncmp(av[i],"--",2) == 0)
    {
      printf("pvbrowser %s (C) Lehrig Software Engineering, lehrig@t-online.de\n", VERSION);
      printf("usage:   pvbrowser <-debug<=level>> <-log> <-ini=filename> <-font=name<:size>> <host<:port></mask>> <-disable> <-geometry=x:y:w:h> <-global_strut=width:height> <-delay=milliseconds>\n");
      printf("example: pvbrowser\n");
      printf("example: pvbrowser localhost\n");
      printf("example: pvbrowser localhost:5050\n");
      printf("example: pvbrowser -font=courier localhost\n");
      printf("example: pvbrowser -font=arial:14 localhost:5050 -disable\n");
      printf("example: pvbrowser -geometry=0:0:640:480\n");
      printf("example: pvbrowser -global_strut=50:50 # set minimum size for embedded systems\n");
      exit(0);
    }
    if(strncmp(av[i],"-debug",6) == 0)
    {
      opt.arg_debug = 1; // print debugging info
      if(strncmp(av[i],"-debug=",7) == 0)
      {
        sscanf(av[i],"-debug=%d",&opt.arg_debug);
      }
    }
    if(strncmp(av[i],"-log",4) == 0)
    {
      opt.arg_log = 1; // print logging info
    }
    if(strncmp(av[i],"-murn",5) == 0)
    {
      opt.arg_murn = 1; // murnleitner special
    }
    if(strncmp(av[i],"-ini=",5) == 0)
    {
      if(strlen(av[i]) < MAXOPT)
      {
        const char *arg = av[i];
        opt.arg_localini = 1; // use inifile in local directory
        strcpy(opt.arg_ini,&arg[5]);
      }
    }
    if(strncmp(av[i],"-font=",6) == 0)
    {
      sscanf(av[i],"-font=%s",opt.arg_font); // use font from command line
    }
    if(strncmp(av[i],"-disable",8) == 0)
    {
      opt.arg_disable = 1; // disable some menus
    }
    if(strncmp(av[i],"-geometry=",10) == 0)
    {
      sscanf(av[i],"-geometry=%d:%d:%d:%d",&opt.arg_x,&opt.arg_y,&opt.arg_w,&opt.arg_h); // define geometry of window
    }
    if(i>0 && strncmp(av[i],"-",1) != 0)
    {
      strcpy(opt.arg_host,av[i]);
    }
    if(strncmp(av[i],"-delay=",7) == 0) // delay startup by milliseconds
    {
      int delay = 0;
      sscanf(av[i],"-delay=%d", &delay);
      if(delay < 0) delay = 0;
      if(opt.arg_debug) printf("delay startup by %d milliseconds\n", delay);
      tcp_sleep(delay);
    }
    if(strncmp(av[i],"-global_strut=",14) == 0) // set minimum size of some widgets on embedded systems
    {
      int width,height;
      sscanf(av[i],"-global_strut=%d:%d", &width, &height);
      QSize strut(width,height);
      QApplication::setGlobalStrut(strut);
    }
  }
}

void perhapsSetFont(QApplication &app)
{
  int  fsize;
  char *cptr, font[MAXOPT];

  if(opt.arg_font[0] == '\0') return;
  strcpy(font,opt.arg_font);
  fsize = 12;
  cptr = strchr(font,':');
  if(cptr != NULL)
  {
    *cptr = '\0';
     cptr++;
     sscanf(cptr,"%d",&fsize);
  }
  app.setFont(QFont(font, fsize));
}

#ifdef BROWSERPLUGIN
int pvbinit()
{
  char *argv[] = {"pvbrowser", ""};
  wsa(); // init windows sockets
  init(1,argv);
  return 0;
}
#else
int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(pvbrowser);

  wsa(); // init windows sockets
  QApplication app(argc, argv);
  QPixmap pm(splash);
  QSplashScreen *splash = new QSplashScreen(pm);
  splash->show();
  init(argc,argv);
  perhapsSetFont(app);
  QIcon appIcon(":/images/app.png");
  app.setWindowIcon(appIcon);
  MainWindow mainWin;
  app.processEvents();
  splash->finish(&mainWin);
  delete splash;
  mainWin.show();
  app.processEvents();
  mainWin.slotReconnect();
  mainWin.slotTimeOut();
  mainWin.mythread.start(QThread::HighestPriority);
  return app.exec();
}
#endif
