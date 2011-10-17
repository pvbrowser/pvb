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
#include "pvdefine.h"
#include <stdlib.h>

#include "opt.h"
#include "interpreter.h"
#include "mainwindow.h"
#include "pvserver.h"
#include "dlgmybrowser.h"

#include <qmovie.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qevent.h>
#include <qsizepolicy.h> 
#include <qlayout.h>
#include <qsound.h>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QDesktopWidget>
#include <QWebFrame>
#ifdef PVWIN32
#include <io.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include "tcputil.h"
#include <qlineedit.h>

#ifdef PVUNIX
#include <utime.h>
#endif

#ifdef PVWIN32
#include <sys/utime.h>
#endif

#ifndef PVWIN32
#define closesocket close
#endif

#define LEFT_PRINT_MARGIN  10
#define UPPER_PRINT_MARGIN 70

extern OPT opt;
extern QString l_print_header;

Interpreter::Interpreter()
{
  mainWindow = NULL;
  mainLayout = NULL;
  hasLayout = 0;
  timet = time(NULL);
  zoom = zoom_original = 100;
  allModal = allBase = NULL;
  allBase = allModal = 0;
  modalDialog = NULL;
  mainWidget = NULL;

#ifdef USE_OPEN_GL
  //QGLWidget gl;
  //gl.makeCurrent();
#endif

  s    = NULL;
  all  = NULL;
  nmax = 0;
  setPath(qApp->argv()[0]);
}

Interpreter::~Interpreter()
{
  if(all != NULL) free(all);
  if(s != NULL && *s != -1) tcp_close(s);
  if(s != NULL) *s = -1;
  all = NULL;
}

void Interpreter::deleteWidget(QWidget *w)
{
  int i,j;
  QWidget *child;
  if(w == NULL) return;
  if(w == all[0]->w) return; // never delete root widget

  // reset all[] array
  QList<QWidget *> widgets = w->findChildren<QWidget *>();
  for(i=1; i<nmax; i++)
  {
    if(all[i]->w == w)
    {
      if(opt.arg_debug) printf("delete_widget %d\n", i);
      all[i]->w = NULL;
      all[i]->type = -1;
      break;
    }
  }
  for(j=0; j < widgets.size(); j++) 
  {
    child = widgets.at(j);
    for(i=1; i<nmax; i++)
    {
      if(all[i]->w == child)
      {
        if(opt.arg_debug) printf("delete_child %d\n", i);
        all[i]->w = NULL;
        all[i]->type = -1;
        break;
      }
    }
  }

  // delete the widget(s)
  delete w;
}

int Interpreter::width()
{
  if(mainWidget == NULL) return 1280;
  return mainWidget->width();
}

int Interpreter::height()
{
  if(mainWidget == NULL) return 1024;
  return mainWidget->height();
}

void Interpreter::myCreateHeuristicMask(QPixmap &pm, QImage *temp)
{
  //pm = *temp; //qt3 version
  pm = QPixmap::fromImage(*temp);
  pm.setMask(pm.createHeuristicMask());
}

void Interpreter::mySetAlphaBuffer(QImage *temp)
{
  //qt3version temp->setAlphaBuffer(true);
  if(temp == NULL) return; //this is not needed by qt4 ???
}

void Interpreter::registerMainWindow(MainWindow *mainwin, int *sock)
{
  zoom = zoom_original = opt.zoom;
  mainWindow = mainwin;
  s = sock;
}

void Interpreter::slotModalTerminate()
{
  char buf[80];
  if(all != NULL)
  {
    free(all);
  }
  if(allBase != NULL) all = allBase;
  allBase = allModal = NULL;
  nmax = nmaxBase;
  sprintf(buf,"QPushButton(0)\n");
  tcp_send(s,buf,strlen(buf));
  modalDialog = NULL;
}

QImage *Interpreter::readBmpFromSocket(int w, int h, char **d)
{
  char *data,text[MAX_PRINTF_LENGTH],*cptr;
  int y,ret,r,g,b,ii,icol;
  QImage *temp;
  //QRgb rgb[4*256];
  QVector<QRgb> colors;

  data = NULL;
  if(w == -1 && h == -1) // load bmp from file (e.g. PHP server)
  {
    temp = new QImage();
    temp->load("temp.bmp");
    QRgb col;
    for(icol=0; icol < temp->numColors(); icol++)
    {
      col = temp->color(icol);
      if(qRed(col) == 1 && qGreen(col) == 1 && qBlue(col) == 1)
      {
        col = qRgba(1,1,1,0); // transparent color
        temp->setColor(icol, col);
      }
    }
    mySetAlphaBuffer(temp);
    *d = data;
    return temp;
  }

  if(w == -2 && h == -2) // load png from file
  {
    tcp_rec(s,text,sizeof(text));
    cptr = strstr(text,"\n");
    if(cptr != NULL) *cptr = '\0';
    // cut off path begin
#ifdef PVUNIX
    cptr = strrchr(text,'/');
#endif
#ifdef __VMS
    cptr = strrchr(text,']');
#endif
#ifdef PVWIN32
    cptr = strrchr(text,'\\');
#endif
    if(cptr == NULL) cptr = &text[0];
    else             cptr++;
    // cut off path end
    if(opt.arg_debug) printf("readBmpFromSocket: %s\n",cptr);
    temp = new QImage();
    temp->load(cptr);
#ifdef USE_MAEMO
    unlink(text);
#endif    
    if(opt.arg_debug) printf("readBmpFromSocket: %s return\n",cptr);
    *d = data;
    return temp;
  }

  data = new char[w*h];

  if(data==NULL) qFatal("out of memory -> exit");
  for(y=0; y<h; y++) // get image
  {
    ret = tcp_rec_binary(s, data+(y*w), w);
    if(ret == -1) { delete [] data; return NULL; }
  }

  for(icol=0; 1; icol++) // get color table
  {
#ifdef PVWIN32
retry:
#endif
    ret = tcp_rec(s, text, sizeof(text)-1);
    if(ret == -1) { delete [] data; return NULL; };
#ifdef PVWIN32
    if(ret == -2) goto retry;
#endif
    if(strncmp(text,"end",3) == 0) break;
    sscanf(text,"qRgb(%d,%d,%d,%d)",&ii,&r,&g,&b);
    //if(r==1 && g==1 && b==1) rgb[icol] = qRgba(r,g,b,0);
    //else                     rgb[icol] = qRgba(r,g,b,255);
    if(r==1 && g==1 && b==1) colors.append(qRgba(r,g,b,0));
    else                     colors.append(qRgba(r,g,b,255));
  }

  if(opt.arg_debug) printf("readBmpFromSocket: w=%d h=%d data[0]=%x\n",w,h,data[0]);
  if(opt.arg_debug) printf("will crash on SuSE 10.2 with original lib from Trolltech it is OK :-( \n");
  //temp = new QImage((uchar *) data,w,h,8,rgb,icol,QImage::IgnoreEndian);
  temp = new QImage((uchar *) data,w,h,QImage::Format_Indexed8);
  if(opt.arg_debug) printf("after new QImage\n");
  temp->setColorTable(colors);
  *d = data;
  mySetAlphaBuffer(temp);
  return temp;
}

void Interpreter::metaFileLog()
{
QString dir;

  QFileDialog dlg;
  dir = dlg.getExistingDirectory();
  if(!dir.isEmpty())
  {
    for(int i=1; i<nmax; i++)
    {
      if(all[i]->type == TQDraw)
      {
        char filename[MAX_PRINTF_LENGTH],buf[80];
        sprintf(buf,"pvMetaFile%d.pvm",i);
        strcpy(filename,(const char *) dir.toUtf8()); strcat(filename,buf);
        QDrawWidget *ptr = (QDrawWidget *) all[i]->w;
        if(opt.arg_debug) printf("logpvm(%s)\n",filename);
        if(ptr != NULL) ptr->logToFile(filename);
      }
    }
  }
}

void Interpreter::metaFileBmp()
{
QString dir;

  QFileDialog dlg;
  dir = dlg.getExistingDirectory();
  if(!dir.isEmpty())
  {
    for(int i=1; i<nmax; i++)
    {
      if(all[i]->type == TQDraw)
      {
        char filename[MAX_PRINTF_LENGTH],buf[80];
        sprintf(buf,"pvMetaFile%d.bmp",i);
        strcpy(filename,(const char *) dir.toUtf8()); strcat(filename,buf);
        QDrawWidget *ptr = (QDrawWidget *) all[i]->w;
        if(opt.arg_debug) printf("logbmp(%s)\n",filename);
        if(ptr != NULL) ptr->logBmpToFile(filename);
      }
    }
  }
}

void Interpreter::setPath(const char *p)
{
  char *cptr;

  strcpy(path,p);
#ifdef PVUNIX
  cptr = strrchr(path,'/');
#endif
#ifdef __VMS
  cptr = strrchr(path,']');
#endif
#ifdef PVWIN32
  cptr = strrchr(path,'\\');
#endif
  if(cptr != NULL)
  {
    cptr++;
    while(*cptr != '\0') *cptr++ = '\0';
  }
  else
  {
    cptr = &path[0];
    while(*cptr != '\0') *cptr++ = '\0';
  }
}

void Interpreter::downloadFile(const char *file)
{
  struct utimbuf utb;
  int   ret;
  short len;
  char  buf[4096];
  QString filename;

  if(strstr(file,"..") != NULL) filename = temp + "suspicious.dat";
  else                          filename = temp + file;

  unlink(filename.toUtf8());
#ifdef PVUNIX
  int fhdl = ::open(filename.toUtf8(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP);
#endif
#ifdef PVWIN32
  int fhdl = ::open(filename.toUtf8(), _O_CREAT | _O_WRONLY | _O_BINARY, _S_IREAD | _S_IWRITE);
#endif
#ifdef __VMS
  int fhdl = ::open(filename.toUtf8(), O_CREAT | O_WRONLY, 0);
#endif
  if(fhdl <= 0)
  {
    QMessageBox::warning(mainWindow,
    "pvbrowser","Could not open: " + filename);
    return;
  }

  QApplication::setOverrideCursor( Qt::WaitCursor );
  while(1)
  {
    ret = tcp_rec_binary(s, (char *) &len, 2);
    if(ret == -1) break;
    len = ntohs(len);
    if(len <= 0)  break;
    if(len > (short) sizeof(buf))
    {
      QMessageBox::warning(mainWindow,
      "pvbrowser","buffer too small in downloadFile: " + filename);
      break;
    }
    ret = tcp_rec_binary(s, buf, len);
    if(ret == -1) break;
    ret = ::write(fhdl,buf,ret);
    if(ret == -1) break;
  }
#ifndef PVWIN32
  ::fsync(fhdl);
#endif
  ::close(fhdl);
  //time_t timet; // remote file time for caching
  utb.actime  = timet;
  utb.modtime = timet;
  utime(file,&utb); // set file modification time
  QApplication::restoreOverrideCursor();
}


int Interpreter::get_text(const char *c, QString &txt)
{
  char mytext[MAX_PRINTF_LENGTH+16],*t,*cptr;
  int i;

  t = &mytext[0];
  *t = '\0';
  while(*c != '\0' && *c != '"') c++; // search for first "
  if(*c == '\0')
  {
    if(tcp_rec(s,t,MAX_PRINTF_LENGTH-1) < 0) return -1;
    cptr = strchr(t,'\n');
    if(cptr != NULL) *cptr = '\0';
    while(1)
    {
      cptr = strchr(mytext,27); // Escape
      if(cptr != NULL) *cptr = '\n';
      else break;
    }
    if(opt.codec == pvbUTF8) txt = QString::fromUtf8(mytext);
    else                     txt = mytext;
    return 0;
  }
  c++;
  i = 0;
  while(*c != '\0') // until terminating " is seen
  {
    if(i >= MAX_PRINTF_LENGTH)
    {
      *t = '\0';
      return 1;
    }
    i++;
    if(*c == '\\')
    {
      c++;
      if     (*c == '"')  *t++ = '"';
      else if(*c == '\n') *t++ = '\n';
      else if(*c == '\t') *t++ = '\t';
      else if(*c == '\\') *t++ = '\\';
      else if(*c == '\0') break;
      c++;
    }
    else if(*c == '"')
    {
      *t = '\0';
      break;
    }
    else
    {
      *t++ = *c++;
    }
  }
  *t = '\0';

  while(1)
  {
    cptr = strchr(mytext,27); // Escape
    if(cptr != NULL) *cptr = '\n';
    else break;
  }
  if(opt.codec == pvbUTF8) txt = QString::fromUtf8(mytext);
  else                     txt = mytext;

  if(opt.arg_debug > 0 && opt.codec == pvbNone) printf("codec==None\n");
  return 0;
}

void Interpreter::addPopupMenuAction(QMenu *m, const char *cmd)
{
  QAction *act;
  const char *text, *arg;
  text = strrchr(cmd,')');
  if(cmd[0] != '#' || text == NULL) text = cmd;
  else                              text++;
  act = m->addAction(text);

  while((arg = strchr(cmd,'#')) != NULL)
  {
    if     (strncmp(cmd,"#c(0)",5) == 0)
    {
      act->setCheckable(true);
      act->setChecked(false);
    }
    else if(strncmp(cmd,"#c(1)",5) == 0)
    {
      act->setCheckable(true);
      act->setChecked(true);
    }
    else if(strncmp(cmd,"#i(",3) == 0)
    {
      char name[1024];
      const char *end;
      int len;
      
      cmd++; cmd++; cmd++;
      end = strchr(cmd,')');
      if(end != NULL)
      {
        len = end - cmd;
        if(len < (int) sizeof(name))
        {
          strncpy(name,cmd,len);
          name[len] = '\0';
          act->setIcon(QIcon(name));
        }  
      }  
    }
    cmd++;
  } 
}

void Interpreter::popupMenu(int id_return, const char *menu)
{
  char buf[800],check[16];
  int  i,ifirst;
  QMenu popupMenu;
  QAction *ret;

  ifirst = 0;
  for(i=0;; i++)
  {
    if(menu[i] == ',' || menu[i] == '\0')
    {
      if(i == ifirst && i == 0)
      {
        popupMenu.addSeparator();
      }
      else if(menu[i] != '\0' && menu[i+1] == ',')
      {
        if(i > 0 && menu[i-1] != ',')
        {
          strncpy(buf,&menu[ifirst],i-ifirst);
          buf[i-ifirst] = '\0';
          addPopupMenuAction(&popupMenu, buf);
        }
        popupMenu.addSeparator();
      }
      else
      {
        if(i > 0 && menu[i-1] != ',')
        {
          strncpy(buf,&menu[ifirst],i-ifirst);
          buf[i-ifirst] = '\0';
          addPopupMenuAction(&popupMenu, buf);
        }
      }
      ifirst = i+1;
    }
    if(menu[i] == '\0') break;
  }

  ret = popupMenu.exec(QCursor::pos());
  check[0] = '\0';
  if(ret != NULL && ret->isCheckable())
  {
    if(ret->isChecked()) strcpy(check,"#c(1)");
    else                 strcpy(check,"#c(0)");
  }
  if(ret != NULL)
  {
    sprintf(buf,"text(%d,\"%s%s\")\n", id_return, check, (const char *) ret->text().toUtf8());
  }
  else
  {
    sprintf(buf,"text(%d,\"\")\n", id_return);
  }
  tcp_send(s,buf,strlen(buf));
}

void Interpreter::interpret(const char *command)
{
  setlocale(LC_NUMERIC,"C"); // damn plugins may reset my locale. ensure my locale is set.
  switch(command[0])
  {
    case 'a':
      interpreta(command);
      break;
    case 'b':
      interpretb(command);
      break;
    case 'c':
      interpretc(command);
      break;
    case 'd':
      interpretd(command);
      break;
    case 'e':
      interprete(command);
      break;
    case 'f':
      interpretf(command);
      break;
    case 'g':
      interpretg(command);
      break;
    case 'h':
      interpreth(command);
      break;
    case 'i':
      interpreti(command);
      break;
    case 'm':
      interpretm(command);
      break;
    case 'p':
      interpretp(command);
      break;
    case 'r':
      interpretr(command);
      break;
    case 's':
      interprets(command);
      break;
    case 't':
      interprett(command);
      break;
    case 'v':
      interpretv(command);
      break;
    case 'q':
      interpretq(command);
      break;
    case 'z':
      interpretz(command);
      break;
    case 'Q':
      interpretQ(command);
      break;
    default:
      break;
  }
}

void Interpreter::interpreta(const char *command)
{
  if(strncmp(command,"addDockWidget(",14) == 0)
  {
    int dock_id,root_id,allow_close,floating,allow_left,allow_right,allow_top,allow_bottom;
    sscanf(command,"addDockWidget(%d,%d,%d,%d,%d,%d,%d,%d,",
      &dock_id,&root_id,
      &allow_close,&floating,
      &allow_left,&allow_right,&allow_top,&allow_bottom);
    get_text(command,text);
    if(root_id < 0) return;
    if(root_id >= nmax) return;
    int id_dock = dock_id - DOCK_WIDGETS_BASE;
    if( id_dock >= 0 && id_dock < MAX_DOCK_WIDGETS)
    {
      if(mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock] == NULL)
      {
        if(all[root_id] != NULL)
        {
          MyQDockWidget *dock = new MyQDockWidget(text,s,root_id,dock_id,mainWindow);
          mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock] = dock; 
          int allowed_areas = Qt::NoDockWidgetArea;
          if(allow_left)   allowed_areas |= Qt::LeftDockWidgetArea;
          if(allow_right)  allowed_areas |= Qt::RightDockWidgetArea;
          if(allow_top)    allowed_areas |= Qt::TopDockWidgetArea;
          if(allow_bottom) allowed_areas |= Qt::BottomDockWidgetArea;
          dock->setAllowedAreas((Qt::DockWidgetAreas) allowed_areas);
          if(floating == 0) dock->setFloating(false);
          else              dock->setFloating(true);
          QDockWidget::DockWidgetFeatures features = dock->features();
          if(allow_close == 0 && features&QDockWidget::DockWidgetClosable)
          {
            features ^=QDockWidget::DockWidgetClosable;
          }  
          else
          {
            features |= QDockWidget::DockWidgetClosable;
          } 
          dock->setFeatures(features);
          if     (allow_left   == 1) mainWindow->addDockWidget(Qt::LeftDockWidgetArea, dock); 
          else if(allow_right  == 1) mainWindow->addDockWidget(Qt::RightDockWidgetArea, dock); 
          else if(allow_top    == 1) mainWindow->addDockWidget(Qt::TopDockWidgetArea, dock); 
          else if(allow_bottom == 1) mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock); 
          else                       mainWindow->addDockWidget(Qt::LeftDockWidgetArea, dock);
          QWidget *w = (QWidget *) all[root_id]->w;
          if(w != NULL)
          {
            int x      = mainWindow->x() + w->x();
            int y      = mainWindow->y() + w->y();
            int width  = w->width();
            int height = w->height();
            QWidget *cw = mainWindow->centralWidget();
            if(cw != NULL)
            {
              x += cw->x();
              y += cw->y();
            }

            w->setParent(dock);
            dock->setWidget(w);

            dock->w = width;
            dock->h = height;
            dock->setGeometry(x,y,width,height);
          }
        }
      }
      else
      {
        printf("addDockWidget dock_id=%d already in use\n", dock_id);
      }
    }
    else
    {
      printf("addDockWidget dock_id=%d out of range\n", dock_id);
    }
  }
  else if(strncmp(command,"addTab(",7) == 0)
  {
    int child;
    sscanf(command,"addTab(%d,%d,",&i,&child);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(child < 0) return;
    if(child >= nmax) return;
    if(all[i]->w == NULL) return;
    if(all[child]->w == NULL) return;
    if(all[i]->type == TQTabWidget)
    {
      MyQTabWidget *ptr = (MyQTabWidget *) all[i]->w;
      if(ptr != NULL) ptr->addTab(all[child]->w, text);
    }
    else if(all[i]->type == TQToolBox)
    {
      MyQToolBox *ptr = (MyQToolBox *) all[i]->w;
      if(ptr != NULL) ptr->addItem(all[child]->w, text);
    }
  }
  else if(strncmp(command,"addColumn(",10) == 0)
  {
    int size;
    sscanf(command,"addColumn(%d,%d,",&i,&size);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListView)
    {
      MyListView *ptr = (MyListView *) all[i]->w;
      if(ptr != NULL) ptr->addColumn(text, size);
    }
  }
  else if(strncmp(command,"addStretch(",11) == 0)
  {
    int param;
    sscanf(command,"addStretch(%d,%d,",&i,&param);
    if(i == 0)
    {
      if(mainLayoutType == TQVbox) ((QVBoxLayout *) mainLayout)->addStretch(param);
      if(mainLayoutType == TQHbox) ((QHBoxLayout *) mainLayout)->addStretch(param);
      //if(mainLayoutType == TQGrid) ((QGridLayout *) mainLayout)->addStretch(param);
    }
    else
    {
      if(i < 0) return;
      if(i >= nmax) return;
      if(all[i]->type == TQVbox && all[i]->w != NULL) ((QVBoxLayout *) all[i]->w)->addStretch(param);
      if(all[i]->type == TQHbox && all[i]->w != NULL) ((QHBoxLayout *) all[i]->w)->addStretch(param);
      //if(all[i]->type == TQGrid) ((QGridLayout *) all[i]->w)->addStretch(param);
    }
  }
  else if(strncmp(command,"addWidgetOrLayout(",18) == 0)
  {
    int item,type,row,col;
    QWidget *w;
    sscanf(command,"addWidgetOrLayout(%d,%d,%d,%d",&i,&item,&row,&col);
    if(i < 0) return;
    if(i >= nmax) return;
    if(i == 0) w = mainLayout;
    else       w = all[i]->w;
    if(i == 0) type = mainLayoutType;
    else       type = all[i]->type;
    if(type == TQVbox && w != NULL)
    {
      QVBoxLayout *ptr = (QVBoxLayout *) w;
      if     (all[item]->type == TQVbox) ptr->addLayout((QVBoxLayout *) all[item]->w);
      else if(all[item]->type == TQHbox) ptr->addLayout((QHBoxLayout *) all[item]->w);
      else if(all[item]->type == TQGrid) ptr->addLayout((QGridLayout *) all[item]->w);
      else                               ptr->addWidget(all[item]->w);
    }
    if(type == TQHbox && w != NULL)
    {
      QHBoxLayout *ptr = (QHBoxLayout *) w;
      if     (all[item]->type == TQVbox) ptr->addLayout((QVBoxLayout *) all[item]->w);
      else if(all[item]->type == TQHbox) ptr->addLayout((QHBoxLayout *) all[item]->w);
      else if(all[item]->type == TQGrid) ptr->addLayout((QGridLayout *) all[item]->w);
      else                               ptr->addWidget(all[item]->w);
    }
    if(type == TQGrid && w != NULL)
    {
      QGridLayout *ptr = (QGridLayout *) w;
      if     (all[item]->type == TQVbox) ptr->addLayout((QVBoxLayout *) all[item]->w,row,col);
      else if(all[item]->type == TQHbox) ptr->addLayout((QHBoxLayout *) all[item]->w,row,col);
      else if(all[item]->type == TQGrid) ptr->addLayout((QGridLayout *) all[item]->w,row,col);
      else                               ptr->addWidget(all[item]->w,row,col);
    }
  }
}

void Interpreter::interpretb(const char *command)
{
  if(strncmp(command,"beep(",5) == 0)
  {
    qApp->beep();
  }
}

void Interpreter::interpretc(const char *command)
{
  if(strncmp(command,"clear(",6) == 0)
  {
    sscanf(command,"clear(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListBox)
    {
      MyListBox *ptr = (MyListBox *) all[i]->w;
      if(ptr != NULL) ptr->clear();
    }
    else if(all[i]->type == TQMultiLineEdit)
    {
      MyMultiLineEdit *ptr = (MyMultiLineEdit *) all[i]->w;
      if(ptr != NULL) 
      {
        ptr->clear();
        ptr->num_lines = 0;
      }  
    }
    else if(all[i]->type == TQListView)
    {
      MyListView *ptr = (MyListView *) all[i]->w;
      if(ptr != NULL) ptr->clear();
    }
    else if(all[i]->type == TQIconView)
    {
      MyIconView *ptr = (MyIconView *) all[i]->w;
      if(ptr != NULL) ptr->clear();
    }
    else if(all[i]->type == TQComboBox)
    {
      MyComboBox *ptr = (MyComboBox *) all[i]->w;
      if(ptr != NULL) ptr->clear();
    }
  }
  else if(strncmp(command,"changeItem(",11) == 0)
  {
    int index,pixmap;
    char *data,text2[80];
    sscanf(command,"changeItem(%d,%d,%d,",&i,&index,&pixmap);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListBox)
    {
      MyListBox *ptr = (MyListBox *) all[i]->w;
      //QPixmap pm;
      if(pixmap==0) 
      {
        QPixmap pm;
        if(ptr != NULL) ptr->changeItem(pm,text,index);
      }
      else
      {
        tcp_rec(s,text2,sizeof(text2));
        sscanf(text2,"(%d,%d)",&w,&h);
        QPixmap pm; //pm(w,h);
        QImage *temp = readBmpFromSocket(w,h,&data);
        if(temp != NULL)
        {
          myCreateHeuristicMask(pm,temp);
          if(ptr != NULL) ptr->changeItem(pm,text,index);
          if(data != NULL) delete [] data;
          if(temp != NULL) delete temp;
        }
      }
    }
    else
    {
      if(pixmap==0) ;
      else
      {
        tcp_rec(s,text2,sizeof(text2));
        sscanf(text2,"(%d,%d)",&w,&h);
        QImage *temp = readBmpFromSocket(w,h,&data);
        if(temp != NULL) delete temp;
        if(data != NULL) delete [] data;
      }
    }
  }
}

void Interpreter::interpretd(const char *command)
{
  if(strncmp(command,"deleteDockWidget(",17) == 0)
  {
    int dock_id = 0, delete_widgets = 0;
    sscanf(command,"deleteDockWidget(%d,%d", &dock_id, &delete_widgets);
    int id_dock = dock_id - DOCK_WIDGETS_BASE;
    if( id_dock >= 0 && id_dock < MAX_DOCK_WIDGETS)
    {
      MyQDockWidget *dock = mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock];
      if(dock != NULL)
      {
        QWidget *w = dock->widget();
        if(w != NULL) // never delete the contents. instead reparent it to main widget
        {
          if(delete_widgets == 1)
          {
            deleteWidget(w);
          }
          else
          {
            w->setParent(all[0]->w);
            w->hide();
          }
        }  
        delete dock;
        mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock] = NULL;
      }
      else
      {
        printf("deleteDockWidget dock_id=%d already NULL\n", dock_id);
      }
    }
    else
    {
      printf("deleteDockWidget dock_id=%d out of range\n", dock_id);
    }
  }
  else if(strncmp(command,"deleteWidget(",13) == 0)
  {
    int id;
    sscanf(command,"deleteWidget(%d", &id);
    if(id > 0 && id < nmax)
    {
      deleteWidget(all[id]->w);
    }  
  }
  else if(strncmp(command,"displayNum(",11) == 0)
  {
    sscanf(command,"displayNum(%d,%d)",&i,&n);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQLCDNumber)
    {
      QLCDNumber *ptr = (QLCDNumber *) all[i]->w;
      if(ptr != NULL) ptr->display(n);
    }
  }
  else if(strncmp(command,"displayFloat(",13) == 0)
  {
    float val;
    sscanf(command,"displayFloat(%d,%f)",&i,&val);

    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQLCDNumber)
    {
      QLCDNumber *ptr = (QLCDNumber *) all[i]->w;
      if(ptr != NULL) ptr->display((double) val);
    }
  }
  else if(strncmp(command,"displayStr(",11) == 0)
  {
    sscanf(command,"displayStr(%d,",&i);
    get_text(command,text);

    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQLCDNumber)
    {
      QLCDNumber *ptr = (QLCDNumber *) all[i]->w;
      if(ptr != NULL) ptr->display(text);
    }
  }
  else if(strncmp(command,"downloadFile(",13) == 0)
  {
    get_text(command,text);
    downloadFile(text.toUtf8());
  }
}

void Interpreter::interprete(const char *command)
{
  if(strncmp(command,"enableTabBar(",13) == 0)
  {
    int state;
    sscanf(command,"enableTabBar(%d,%d)",&i,&state);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTabWidget)
    {
      MyQTabWidget *p = (MyQTabWidget *) all[i]->w;
      p->enableTabBar(state);
    }
  }
  else if(strncmp(command,"endDefinition",13) == 0) // endDefinition of new MainWidget
  {
    if(allBase == NULL)
    {
      if(opt.arg_debug) printf("endDefinition begin\n");
      delete mainWindow->scroll->takeWidget();
      w = mainWindow->width();
      h = mainWindow->height();
      mainWindow->pvbtab[mainWindow->currentTab].w = w;
      mainWindow->pvbtab[mainWindow->currentTab].h = h;
      all[0]->w->resize(w,h);
      if(mainWindow->scroll == NULL)
      {
        printf("BIG FAT ERROR: scroll==NULL\n");
        exit(-1);
      }
      if(hasLayout == 1)
      {
        mainWindow->pvbtab[mainWindow->currentTab].hasLayout = 1;
        mainWindow->scroll->setWidgetResizable(true);
      }
      else
      {
        mainWindow->pvbtab[mainWindow->currentTab].hasLayout = 0;
        mainWindow->scroll->setWidgetResizable(false);
        if(all[0]->w != NULL) all[0]->w->resize(1280,1024); // resize to default screen dimension
      }
      if(opt.scrollbars == 0)
      {
        mainWindow->scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mainWindow->scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      }
      mainWindow->scroll->setWidget(all[0]->w);
      mainWindow->pvbtab[mainWindow->currentTab].rootWidget = all[0]->w;
      // workaround for qt
      QApplication::postEvent(mainWindow, new QResizeEvent(QSize(w-1,h-1),QSize(w,h))); // force qt to update sliders
      //QCoreApplication::processEvents();
      //QApplication::postEvent(mainWindow, new QResizeEvent(QSize(w,h),QSize(w-1,h-1)));
      if(opt.arg_debug) printf("endDefinition end\n");
    }
    else
    {
      QString capt;
      if(opt.arg_debug) printf("endDefinition allBase != NULL modalDialog\n");
      capt.sprintf("%s - pvbrowser",(const char *) mainWindow->curFile.toUtf8());
      modalDialog->setWindowTitle(capt);
      modalDialog->show();
    }
    if(opt.arg_debug) printf("endDefinition end\n");
  }
  else if(strncmp(command,"ensureCellVisible(",18) == 0)
  {
    int row,col;
    sscanf(command,"ensureCellVisible(%d,%d,%d)",&i,&row,&col);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      MyTable *p = (MyTable *) all[i]->w;
      //p->ensureCellVisible(row,col);
      if(p != NULL) p->scrollToItem(p->item(row,col));
    }
  }
  else if(strncmp(command,"ensureListVisible(",18) == 0)
  {
    sscanf(command,"ensureListVisible(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListView)
    {
      get_text(command,text);
      MyListView *l = (MyListView *) all[i]->w;
      if(l != NULL) l->ensureVisible(text.toUtf8(), (MyListViewItem *) l->firstChild(NULL));
    }
  }
}

void Interpreter::interpretf(const char *command)
{
  if(strncmp(command,"fileCache(",10) == 0) // cache downloaded files
  {
    struct stat statbuf;
    struct tm remote;
    int ret;
    char buf[80];

    memset(&remote,0,sizeof(remote));
    sscanf(command,"fileCache(%d,%d,%d,%d,%d,%d,%d,%d,%d"
      ,&remote.tm_sec
      ,&remote.tm_min
      ,&remote.tm_hour
      ,&remote.tm_mon
      ,&remote.tm_year
      ,&remote.tm_mday
      ,&remote.tm_wday
      ,&remote.tm_yday
      ,&remote.tm_isdst);
    get_text(command,text);
    timet = mktime(&remote);
    ret = stat(text.toUtf8(),&statbuf);
    if(ret < 0) // file does not exist
    {
      strcpy(buf,"cache(0)\n");   // file was not cached
      tcp_send(s,buf,strlen(buf));
    }
    else if(memcmp(&statbuf.st_mtime, &timet,sizeof(time_t)) == 0)
    {
      strcpy(buf,"cache(1)\n"); // file was cached
      tcp_send(s,buf,strlen(buf));
    }
    else
    {
      strcpy(buf,"cache(0)\n"); // file time is different (not cached)
      tcp_send(s,buf,strlen(buf));
    }
  }
  else if(strncmp(command,"fileDialog(",11) == 0) // Open, Save, FindDirectory
  {
    QString result;
    int id_return,type;
    sscanf(command,"fileDialog(%d,%d",&id_return,&type);
    if     (type==0) result = QFileDialog::getOpenFileName();
    else if(type==1) result = QFileDialog::getSaveFileName();
    else if(type==2) result = QFileDialog::getExistingDirectory();
    text.sprintf("text(%d,\"%s\")\n", id_return, (const char *) result.toUtf8());
    tcp_send(s,text.toUtf8(),strlen(text.toUtf8()));
  }
}

void Interpreter::interpretg(const char *command)
{
  if(strncmp(command,"gbeginDraw(",11) == 0) // start drawing
  {
    sscanf(command,"gbeginDraw(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        p->hasLayout = mainWindow->pvbtab[mainWindow->currentTab].hasLayout;
        p->showFromSocket(s);
      }
    }
  }
  else if(strncmp(command,"gCopyToClipboard(",17) == 0)
  {
    sscanf(command,"gCopyToClipboard(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL) p->copyToClipboard();
    }
    else
    {
      QWidget *w = all[i]->w;
      if(w != NULL) 
      {
        QPixmap pm(QPixmap::grabWindow(w->winId(),0,0,w->width(),w->height()));
        QApplication::clipboard()->setPixmap(pm);
      }
    }
  }
  else if(strncmp(command,"gsvgPrintOnPrinter(",19) == 0)
  {
    sscanf(command,"gsvgPrintOnPrinter(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        if(p->svgAnimator != NULL)
        {
          p->svgAnimator->update(1);
        }
      }
    }
  }
   else if(strncmp(command,"gsvgPrintf(",11) == 0)
  {
    sscanf(command,"gsvgPrintf(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        char *cptr,objectname[MAX_PRINTF_LENGTH],tag[MAX_PRINTF_LENGTH],text[MAX_PRINTF_LENGTH];
        tcp_rec(s,objectname,sizeof(objectname));
        tcp_rec(s,tag,sizeof(tag));
        tcp_rec(s,text,sizeof(text));
        cptr = strchr(objectname,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(tag,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(text,'\n');
        if(cptr != NULL) *cptr = '\0';
        if(p->svgAnimator != NULL)
        {
          p->svgAnimator->svgPrintf(objectname,tag,text,NULL);
        }
      }
    }
  }
  else if(strncmp(command,"gsvgRecursivePrintf(",20) == 0)
  {
    sscanf(command,"gsvgRecursivePrintf(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        char *cptr,objectname[MAX_PRINTF_LENGTH],tag[MAX_PRINTF_LENGTH],text[MAX_PRINTF_LENGTH];
        tcp_rec(s,objectname,sizeof(objectname));
        tcp_rec(s,tag,sizeof(tag));
        tcp_rec(s,text,sizeof(text));
        cptr = strchr(objectname,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(tag,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(text,'\n');
        if(cptr != NULL) *cptr = '\0';
        if(p->svgAnimator != NULL)
        {
          p->svgAnimator->svgRecursivePrintf(objectname,tag,text,NULL);
        }
      }
    }
  }
  else if(strncmp(command,"gsvgRecursiveSearchAndReplace(",30) == 0)
  {
    sscanf(command,"gsvgRecursiveSearchAndReplace(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        char *cptr,objectname[MAX_PRINTF_LENGTH],tag[MAX_PRINTF_LENGTH],text[MAX_PRINTF_LENGTH],after[MAX_PRINTF_LENGTH];
        tcp_rec(s,objectname,sizeof(objectname));
        tcp_rec(s,tag,sizeof(tag));
        tcp_rec(s,text,sizeof(text));
        tcp_rec(s,after,sizeof(after));
        cptr = strchr(objectname,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(tag,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(text,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(after,'\n');
        if(cptr != NULL) *cptr = '\0';
        if(p->svgAnimator != NULL)
        {
          p->svgAnimator->svgRecursivePrintf(objectname,tag,text,after);
        }
      }
    }
  }
  else if(strncmp(command,"gsvgSearchAndReplace(",21) == 0)
  {
    sscanf(command,"gsvgSearchAndReplace(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        char *cptr,objectname[MAX_PRINTF_LENGTH],tag[MAX_PRINTF_LENGTH],text[MAX_PRINTF_LENGTH],after[MAX_PRINTF_LENGTH];
        tcp_rec(s,objectname,sizeof(objectname));
        tcp_rec(s,tag,sizeof(tag));
        tcp_rec(s,text,sizeof(text));
        tcp_rec(s,after,sizeof(after));
        cptr = strchr(objectname,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(tag,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(text,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(after,'\n');
        if(cptr != NULL) *cptr = '\0';
        if(p->svgAnimator != NULL)
        {
          p->svgAnimator->svgPrintf(objectname,tag,text,after);
        }
      }
    }
  }
  else if(strncmp(command,"gsvgTextPrintf(",15) == 0)
  {
    sscanf(command,"gsvgTextPrintf(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        char *cptr,objectname[MAX_PRINTF_LENGTH],text[MAX_PRINTF_LENGTH];
        tcp_rec(s,objectname,sizeof(objectname));
        tcp_rec(s,text,sizeof(text));
        cptr = strchr(objectname,'\n');
        if(cptr != NULL) *cptr = '\0';
        cptr = strchr(text,'\n');
        if(cptr != NULL) *cptr = '\0';
        if(p->svgAnimator != NULL)
        {
          p->svgAnimator->svgTextPrintf(objectname,text);
        }
      }
    }
  }
  else if(strncmp(command,"gsvgShow(",9) == 0)
  {
    int state;
    sscanf(command,"gsvgShow(%d,%d)",&i,&state);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        char *cptr,objectname[MAX_PRINTF_LENGTH];
        tcp_rec(s,objectname,sizeof(objectname));
        cptr = strchr(objectname,'\n');
        if(cptr != NULL) *cptr = '\0';
        if(p->svgAnimator != NULL)
        {
          p->svgAnimator->show(objectname,state); // state := 0|1
        }
      }
    }
  }
  else if(strncmp(command,"gsvgRead(",9) == 0)
  {
    sscanf(command,"gsvgRead(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL)
      {
        if(p->svgAnimator != NULL) delete p->svgAnimator;
        p->svgAnimator = new pvSvgAnimator(s,p);
        p->svgAnimator->read();
        if(opt.arg_debug > 1) p->svgAnimator->testoutput();
      }
    }
  }
  else if(strncmp(command,"gSetZoomX(",10) == 0)
  {
    float zx;
    sscanf(command,"gSetZoomX(%d,%f)",&i,&zx);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL) p->setZoomX(zx);
    }
  }
  else if(strncmp(command,"gSetZoomY(",10) == 0)
  {
    float zy;
    sscanf(command,"gSetZoomY(%d,%f)",&i,&zy);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL) p->setZoomY(zy);
    }
  }
  else if(strncmp(command,"gSave(",6) == 0)
  {
    sscanf(command,"gSave(%d,",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL) p->logToFile(text.toUtf8());
    }
  }
  else if(strncmp(command,"gSaveAsBmp(",11) == 0)
  {
    sscanf(command,"gSaveAsBmp(%d,",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *p = (QDrawWidget *) all[i]->w;
      if(p != NULL) p->logBmpToFile(text.toUtf8());
    }
    else
    {
      QWidget *w = (QDrawWidget *) all[i]->w;
      if(w != NULL)
      {
        QPixmap pm(QPixmap::grabWindow(w->winId(),0,0,w->width(),w->height()));
        pm.toImage().save(text);
      }
    }
  }
}

void Interpreter::interpreth(const char *command)
{
  if(strncmp(command,"hide(",5) == 0) // hide the widget
  {
    sscanf(command,"hide(%d)",&i);
    if(i < 0)
    {
      if(i == -1) mainWindow->menuBar()->hide();
      if(i == -2) mainWindow->fileToolBar->hide();
      if(i == -3) mainWindow->statusBar()->hide();
      if(i == -4) mainWindow->hide();
      if(i == -5) 
      {
        if(mainWindow->textbrowser == NULL) mainWindow->textbrowser = new dlgTextBrowser;
        mainWindow->textbrowser->hide();
      }  
      int id_dock = i - DOCK_WIDGETS_BASE;
      if( id_dock >= 0 && id_dock < MAX_DOCK_WIDGETS)
      {
        MyQDockWidget *dock = mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock]; 
        if(dock != NULL) dock->hide();
      }
      return;
    }
    if(i >= nmax) return;
    // redraw problem reported by ernst murnleitner
#if QT_VERSION < 0x040400
    if(all[i]->w != NULL) all[i]->w->setAttribute(Qt::WA_PaintOnScreen);
#endif
    //if(all[i]->w != NULL) all[i]->w->setAttribute(Qt::WA_PaintUnclipped);
    if(all[i]->w != NULL) all[i]->w->hide();
  }
  else if(strncmp(command,"hyperlink(",10) == 0) // follow a hyperlink to a different computer
  {
    get_text(command,text);
    if(text[0] == '+')
    {
      QString cmd = "pvbrowser " + text.mid(1);
#ifdef PVUNIX
      cmd += " &";
#endif
      mysystem(cmd.toUtf8());
    }
    else
    {
      mainWindow->slotUrl(text);
    }
  }
}

void Interpreter::interpreti(const char *command)
{
  if(strncmp(command,"insertCols(",11) == 0)
  {
    int col = 0;
    int count = 0;
    sscanf(command,"insertCols(%d,%d,%d",&i,&col,&count);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      MyTable *ptr = (MyTable *) all[i]->w;
      //ptr->insertColumns(col,count);
      if(ptr != NULL) 
      {
        for(int icount=0; icount<count; icount++) ptr->insertColumn(col+icount);
      }
    }
  }
  else if(strncmp(command,"insertItem(",11) == 0)
  {
    int index,pixmap;
    char *data,text2[80],*cptr;
    sscanf(command,"insertItem(%d,%d,%d,",&i,&index,&pixmap);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListBox)
    {
      MyListBox *ptr = (MyListBox *) all[i]->w;
      //QPixmap pm;
      if(pixmap==0) 
      {
        QPixmap pm;
        if(ptr != NULL) ptr->insertItem(pm,text,index);
      }
      else
      {
        tcp_rec(s,text2,sizeof(text2));
        sscanf(text2,"(%d,%d)",&w,&h);
        QImage *temp = readBmpFromSocket(w,h,&data);
        if(temp != NULL)
        {
          QPixmap pm; //pm(w,h);
          myCreateHeuristicMask(pm,temp);
          if(ptr != NULL) ptr->insertItem(pm,text,index);
          if(data != NULL) delete [] data;
          if(temp != NULL) delete temp;
        }
      }
    }
    else if(all[i]->type == TQComboBox)
    {
      char text2[80];
      MyComboBox *ptr = (MyComboBox *) all[i]->w;
      if(index == -1)
      {
        if(ptr != NULL) index = ptr->count() + 1; 
      }
      //QPixmap pm;
      if(pixmap==0)
      {
        if(ptr != NULL) ptr->insertItem(index,text);
      }
      else
      {
        tcp_rec(s,text2,sizeof(text2));
        sscanf(text2,"(%d,%d)",&w,&h);
        if(w==-2 && h==-2) // it is a png file
        {
          QString filename;
          tcp_rec(s,text2,sizeof(text2)); // text2 = filename of image
          cptr = strchr(text2,'\n');
          if(cptr != NULL) *cptr = '\0';
          filename = temp + text2;
          QPixmap pm(filename.toUtf8());
          if(ptr != NULL) ptr->insertItem(index,pm,text);
        }
        else
        {
          QImage *temp = readBmpFromSocket(w,h,&data);
          QPixmap pm; //pm(w,h);
          if(temp != NULL)
          {
            myCreateHeuristicMask(pm,temp);
            if(ptr != NULL) ptr->insertItem(index,pm,text);
            if(data != NULL) delete [] data;
            if(temp != NULL) delete temp;
          }
        }
      }
    }
    else
    {
      if(pixmap==0) ;
      else
      {
        tcp_rec(s,text2,sizeof(text2));
        sscanf(text2,"(%d,%d)",&w,&h);
        if(w > 0 && h > 0)
        {
          QImage *temp = readBmpFromSocket(w,h,&data);
          if(temp != NULL) delete temp;
          if(data != NULL) delete [] data;
        }
      }
    }
  }
  else if(strncmp(command,"insertRows(",11) == 0)
  {
    int row = 0;
    int count = 0;
    sscanf(command,"insertRows(%d,%d,%d",&i,&row,&count);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      MyTable *ptr = (MyTable *) all[i]->w;
      //ptr->insertRows(row,count);
      if(ptr != NULL)
      {
        for(int icount=0; icount<count; icount++) ptr->insertRow(row+icount);
      }
    }
  }
  else if(strncmp(command,"inputDialog(",12) == 0)
  {
    char *cptr;
    char default_text[MAX_PRINTF_LENGTH] = "";
    bool ok;
    int  id_return;
    sscanf(command,"inputDialog(%d,",&id_return);
    get_text(command,text);
    for(int i=0; i<text.length(); i++)
    {
      if(text[i] == 12) text[i] = '\n'; // replace FF
    }
    tcp_rec(s,default_text,sizeof(default_text));
    cptr = strchr(default_text,'\n');
    if(cptr != NULL) *cptr = '\0';
    if(opt.arg_debug) printf("QInputDialog text=%s default_text=%s\n", (const char *) text.toUtf8(), default_text);
    QString result = QInputDialog::getText(mainWindow,"pvbrowser",text,QLineEdit::Normal,QString::fromUtf8(default_text),&ok);
    if(ok && result.length() > 0)
    {
      result.truncate(MAX_PRINTF_LENGTH-80);
      text.sprintf("text(%d,\"%s\")\n", id_return, (const char *) result.toUtf8());
      tcp_send(s,text.toUtf8(),strlen(text.toUtf8()));
    }
    else
    {
      text.sprintf("text(%d,\"\")\n", id_return);
      tcp_send(s,text.toUtf8(),strlen(text.toUtf8()));
    }
  }
}

void Interpreter::interpretm(const char *command)
{
  if(strncmp(command,"move(",5) == 0) // move the widget
  {
    sscanf(command,"move(%d,%d,%d)",&i,&x,&y);
    if(zoom != 100)
    {
      x = (x*zoom)/100;
      y = (y*zoom)/100;
    }
    if(i < 0)
    {
      int id_dock = i - DOCK_WIDGETS_BASE;
      if( id_dock >= 0 && id_dock < MAX_DOCK_WIDGETS)
      {
        MyQDockWidget *dock = mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock]; 
        x += mainWindow->x();
        y += mainWindow->y();
        QWidget *cw = mainWindow->centralWidget();
        if(cw != NULL)
        {
          x += cw->x();
          y += cw->y();
        }
        if(dock != NULL) dock->move(x,y);
      }
      return;
    }        
    if(i >= nmax) return;
    if(all[i]->w != NULL)
    { // remember original geometry
      all[i]->x = x;
      all[i]->y = y;
    }      
    if(all[i]->w != NULL) all[i]->w->move(x,y);
  }
  else if(strncmp(command,"messageBox(",11) == 0) // open a messageBox()
  {
    char buf[80];
    int ret,type,b0,b1,b2,id_return;
    sscanf(command,"messageBox(%d,%d,%d,%d,%d,",&id_return,&type,&b0,&b1,&b2);
    get_text(command,text);
    for(int i=0; i<text.length(); i++)
    {
      if(text[i] == 12) text[i] = '\n'; // replace FF
    }
    ret = -1;
    if(opt.arg_debug) printf("QMessageBox type=%d text=%s\n",type,(const char *) text.toUtf8());
    if(type==0) ret = QMessageBox::information(mainWindow,"pvbrowser",text,b0,b1,b2);
    if(type==1) ret = QMessageBox::warning    (mainWindow,"pvbrowser",text,b0,b1,b2);
    if(type==2) ret = QMessageBox::critical   (mainWindow,"pvbrowser",text,b0,b1,b2);
    sprintf(buf,"slider(%d,%d)\n",id_return,ret);
    tcp_send(s,buf,strlen(buf));
  }
  else if(strncmp(command,"moveContent(",12) == 0) // move in QTextBrowser
  {
    int pos;
    sscanf(command,"moveContent(%d,%d",&i,&pos);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTextBrowser)
    {
      MyTextBrowser *ptr = (MyTextBrowser *) all[i]->w;
      if(ptr != NULL) ptr->moveContent(pos);
    }
  }
  else if(strncmp(command,"moveCursor(",11) == 0)
  {
    char buf[MAX_PRINTF_LENGTH];
    int cur, cursor;
    sscanf(command,"moveCursor(%d,%d)",&i,&cur);
    buf[0] = '\0';
    if(i < 0) return;
    if(i >= nmax) return;

    cursor = QTextCursor::Start;
    if     (cur == PV::NoMove)            cursor = QTextCursor::NoMove;
    else if(cur == PV::StartOfLine)       cursor = QTextCursor::StartOfLine;
    else if(cur == PV::StartOfBlock)      cursor = QTextCursor::StartOfBlock;
    else if(cur == PV::StartOfWord)       cursor = QTextCursor::StartOfWord;
    else if(cur == PV::PreviousBlock)     cursor = QTextCursor::PreviousBlock;
    else if(cur == PV::PreviousCharacter) cursor = QTextCursor::PreviousCharacter;
    else if(cur == PV::PreviousWord)      cursor = QTextCursor::PreviousWord;
    else if(cur == PV::Up)                cursor = QTextCursor::Up;
    else if(cur == PV::Left)              cursor = QTextCursor::Left;
    else if(cur == PV::WordLeft)          cursor = QTextCursor::WordLeft;
    else if(cur == PV::End)               cursor = QTextCursor::End;
    else if(cur == PV::EndOfLine)         cursor = QTextCursor::EndOfLine;
    else if(cur == PV::EndOfWord)         cursor = QTextCursor::EndOfWord;
    else if(cur == PV::EndOfBlock)        cursor = QTextCursor::EndOfBlock;
    else if(cur == PV::NextBlock)         cursor = QTextCursor::NextBlock;
    else if(cur == PV::NextCharacter)     cursor = QTextCursor::NextCharacter;
    else if(cur == PV::NextWord)          cursor = QTextCursor::NextWord;
    else if(cur == PV::Down)              cursor = QTextCursor::Down;
    else if(cur == PV::Right)             cursor = QTextCursor::Right;
    else if(cur == PV::WordRight)         cursor = QTextCursor::WordRight;

    if(all[i]->type == TQTextBrowser)
    {
      MyTextBrowser *w = (MyTextBrowser *) all[i]->w;
      if     (cur == PV::NoMove)            w->pageAction(QWebPage::NoWebAction);
      else if(cur == PV::StartOfLine)       w->pageAction(QWebPage::MoveToStartOfLine);
      else if(cur == PV::StartOfBlock)      w->pageAction(QWebPage::MoveToStartOfBlock);      
      //else if(cur == PV::StartOfWord)       ;
      else if(cur == PV::PreviousBlock)     w->pageAction(QWebPage::QWebPage::MoveToPreviousLine);
      else if(cur == PV::PreviousCharacter) w->pageAction(QWebPage::MoveToPreviousChar); 
      else if(cur == PV::PreviousWord)      w->pageAction(QWebPage::MoveToPreviousWord);
      else if(cur == PV::Up)                w->pageAction(QWebPage::MoveToStartOfDocument); // ???
      //else if(cur == PV::Left)              ;
      //else if(cur == PV::WordLeft)          ;
      else if(cur == PV::End)               w->pageAction(QWebPage::MoveToEndOfDocument);
      else if(cur == PV::EndOfLine)         w->pageAction(QWebPage::MoveToEndOfLine);
      //else if(cur == PV::EndOfWord)         ;
      else if(cur == PV::EndOfBlock)        w->pageAction(QWebPage::MoveToEndOfBlock);
      else if(cur == PV::NextBlock)         w->pageAction(QWebPage::QWebPage::MoveToNextLine);       
      else if(cur == PV::NextCharacter)     w->pageAction(QWebPage::MoveToNextChar);
      else if(cur == PV::NextWord)          w->pageAction(QWebPage::MoveToNextWord);
      else if(cur == PV::Down)              w->pageAction(QWebPage::MoveToEndOfDocument); // ???
      //else if(cur == PV::Right)             ;
      //else if(cur == PV::WordRight)         ;
      //tb w->moveCursor((QTextCursor::MoveOperation) cursor);
    }
    else if(all[i]->type == TQMultiLineEdit)
    {
      MyMultiLineEdit *w = (MyMultiLineEdit *) all[i]->w;
      w->moveCursor((QTextCursor::MoveOperation) cursor);
    }
  }
  else if(strncmp(command,"modalInit(",10) == 0) // init modalDialog
  {
    int width, height;
    sscanf(command,"modalInit(%d,%d",&width,&height);
    allBase = all;
    nmaxBase = nmax;
    modalDialog = new MyDialog(this,s,0,mainWindow);
    modalDialog->resize((width*zoom)/100,(height*zoom)/100);
    modalDialog->setFixedSize((width*zoom)/100,(height*zoom)/100);
  }
  else if(strncmp(command,"modalTerminate(",15) == 0) // termnate modalDialog
  {
    if(modalDialog != NULL)
    {
      if(opt.arg_debug) printf("command=%s\n",command);
      delete modalDialog;
      modalDialog = NULL;
    }
  }
  else if(strncmp(command,"modalBase(",10) == 0) // switch all to baseDialog
  {
    if(allBase != NULL)
    {
      all = allBase;
      nmax = nmaxBase;
    }
  }
  else if(strncmp(command,"modalModal(",11) == 0) // switch all to modalDialog
  {
    if(allModal != NULL)
    {
      all = allModal;
      nmax = nmaxModal;
    }
  }
  else if(strncmp(command,"movie(",6) == 0) // control movie within QLabel
  {
    int func,param;
    sscanf(command,"movie(%d,%d,%d",&i,&func,&param);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQLabel)
    {
      MyLabel *ptr = (MyLabel *) all[i]->w;
      switch(func)
      {
        case 0: // pvSetMovie
          get_text(command,text);
          //qt4porting if(param == 0) ptr->setBackgroundMode(Qt::NoBackground);
          //qt4porting if(param == 1) ptr->setBackgroundMode(Qt::PaletteBackground);
          //ptr->setAutoResize(true);
          if(ptr != NULL) ptr->setScaledContents(true);
          if(ptr != NULL) ptr->setMovie(new QMovie(text));
          break;
        case 1: // pvMovieControl
          if(param == -2 && ptr != NULL) ptr->movie()->start();
          if(param == -1 && ptr != NULL) ptr->movie()->setPaused(false);
          if(param ==  0 && ptr != NULL) ptr->movie()->setPaused(true);
          //if(param >   0) ptr->movie()->step(param);
          break;
        case 2: // pvMovieSpeed
          if(ptr != NULL) ptr->movie()->setSpeed(param);
          break;
        default:
          break;  
      }
    }
  }
}

void Interpreter::interpretp(const char *command)
{
  if(strncmp(command,"print(",6) == 0) // print the widget
  {
    sscanf(command,"print(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *ptr = (QDrawWidget *) all[i]->w;
      if(ptr != NULL) ptr->print();
    }
    else
    {
      QPrinter printer;
      printer.setOrientation(QPrinter::Landscape);
      printer.setColorMode(QPrinter::Color);
      QWidget *w = all[i]->w;
      if(w != NULL)
      {
        QPixmap pm(QPixmap::grabWindow(w->winId(),0,0,w->width(),w->height()));
        //if(printer.setup(mainWindow))
        QPrintDialog dialog(&printer, mainWindow);
        if(dialog.exec() == QDialog::Accepted)
        {
          QPainter painter;
          painter.begin(&printer);
//#ifndef PVWIN32
//        if(printer.orientation() == QPrinter::Landscape)
//        {
//          painter.rotate(-90.0f);
//          painter.translate(-painter.viewport().width(),0.0f);
//        }
//#endif
          if(strlen(l_print_header.toUtf8()) > 0)
          {
            QString date_time;
            date_time  = l_print_header;
            date_time += QDateTime::currentDateTime().toString(Qt::TextDate);
            painter.drawText(LEFT_PRINT_MARGIN,UPPER_PRINT_MARGIN - 30,1280,100,Qt::TextExpandTabs | Qt::TextDontClip, date_time);
          }
          painter.drawPixmap(LEFT_PRINT_MARGIN,UPPER_PRINT_MARGIN,pm);
          painter.end();
        }
      }
    }
  }
  else if(strncmp(command,"printHTML(",10) == 0) // print QWebView
  {
    sscanf(command,"printHTML(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTextBrowser)
    {
      MyTextBrowser *ptr = (MyTextBrowser *) all[i]->w;
      QPrinter printer;
      //printer.setOrientation(QPrinter::Landscape);
      printer.setColorMode(QPrinter::Color);
      QWidget *w = all[i]->w;
      if(w != NULL)
      {
        QPrintDialog dialog(&printer, mainWindow);
        if(dialog.exec() == QDialog::Accepted)
        {
          ptr->print(&printer);
        }
      }
    }
  }
#ifdef USE_OPEN_GL
  else if(strncmp(command,"pvGlUpdate(",11) == 0) // update OpenGL widget
  {
    PvGLWidget *ptr;
    sscanf(command,"pvGlUpdate(%d)",&i);

    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQGl)
    {
      ptr = (PvGLWidget *) all[i]->w;
      if(ptr != NULL)
      {
        ptr->paint_done = 1;
        ptr->updateGL();
      }  
    }
  }
  else if(strncmp(command,"pvGlBegin(",10) == 0) // begin OpenGL commands
  {
    GLenum mode;
    PvGLWidget *ptr;

    sscanf(command,"pvGlBegin(%d,%d)",&i,(int *) &mode);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQGl)
    {
      ptr = (PvGLWidget *) all[i]->w;
      if(ptr != NULL) ptr->makeCurrent();
      if(ptr != NULL) ptr->gl_interpret();
    }
  }
#endif
  else if(strncmp(command,"pvsVersion(",11) == 0)
  {
    int v1, v2, v3;
    v1 = v2 = v3 = 0;
    sscanf(command,"pvsVersion(%d.%d.%d)", &v1, &v2, &v3);
    mainWindow->pvbtab[mainWindow->currentTab].pvsVersion = (v1*256 + v2)*256 + v3;
  }
  else if(strncmp(command,"playSound(",10) == 0) // play a (WAV) sound
  {
    get_text(command,text);
    printf("playSound(\"%s\")\n",(const char *) text.toUtf8());
#ifdef USE_ANDROID
    qApp->beep();
#else
    if(QSound::isAvailable())
    {
      QSound::play(text);
    }
    else
    {
      printf("sound not available on this platform\n");
      printf("On Microsoft Windows the underlying multimedia system is used; only WAVE format sound files are supported.\n");
      printf("On X11 the Network Audio System is used if available, otherwise all operations work silently. NAS supports WAVE and AU files.\n");
      printf("On Macintosh, ironically, we use QT (QuickTime) for sound, this means all QuickTime formats are supported by Qt/Mac.\n");
      printf("On Qt/Embedded, a built-in mixing sound server is used, which accesses /dev/dsp directly. Only the WAVE format is supported.\n");
      qApp->beep();
    }
#endif    
  }
  else if(strncmp(command,"popupMenu(",10) == 0) // open a popupMenu()
  {
    int id_return;
    sscanf(command,"popupMenu(%d",&id_return);
    get_text(command,text);
    popupMenu(id_return,text.toUtf8());
  }
}

void Interpreter::interpretr(const char *command)
{
  if(strncmp(command,"removeAllColums(",16) == 0)
  {
    sscanf(command,"removeAllColums(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListView)
    {
      MyListView *ptr = (MyListView *) all[i]->w;
      if(ptr != NULL) ptr->removeAllColumns();
    }
  }
  else if(strncmp(command,"removeCol(",10) == 0)
  {
    int col = 0;
    sscanf(command,"removeCol(%d,%d",&i,&col);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      MyTable *ptr = (MyTable *) all[i]->w;
      if(ptr != NULL) ptr->removeColumn(col);
    }
  }
  else if(strncmp(command,"removeRow(",10) == 0)
  {
    int row = 0;
    sscanf(command,"removeRow(%d,%d",&i,&row);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      MyTable *ptr = (MyTable *) all[i]->w;
      if(ptr != NULL) ptr->removeRow(row);
    }
  }
  else if(strncmp(command,"resize(",7) == 0) // resize the widget
  {
    sscanf(command,"resize(%d,%d,%d)",&i,&w,&h);
    if(zoom != 100)
    {
      w = (w*zoom)/100;
      h = (h*zoom)/100;
      if(w < 1) w = 1;
      if(h < 1) h = 1;
    }
    if(i == -4)
    {
      mainWindow->resize(w,h);
      return;
    }
    else if(i == 0)
    {
      if(mainWindow->scroll != NULL)
      {
        mainWindow->pvbtab[mainWindow->currentTab].w = w;
        mainWindow->pvbtab[mainWindow->currentTab].h = h;
        mainWindow->scroll->resize(w,h);
        QEvent event(QEvent::Resize);
        QApplication::sendEvent(mainWindow, &event);
      }
    }
    if(i < 0)
    {
      int id_dock = i - DOCK_WIDGETS_BASE;
      if( id_dock >= 0 && id_dock < MAX_DOCK_WIDGETS)
      {
        MyQDockWidget *dock = mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock]; 
        if(dock != NULL) 
        {
          dock->w = w;
          dock->h = h;
          dock->resize(w,h);
        }  
      }
      return;
    }  
    if(i >= nmax) return;
    if(all[i]->w != NULL)
    { // remember original geometry
      all[i]->width  = w;
      all[i]->height = h;
    }      
    if(all[i]->type == TQDraw)
    {
      QDrawWidget *iw = (QDrawWidget *) all[i]->w;
      if(iw != NULL) iw->resize(w,h);
      if(zoom != 100)
      {
        float fzoom = ((float)zoom)/100.0f;
        if(iw != NULL) iw->setZoomX(fzoom);
        if(iw != NULL) iw->setZoomY(fzoom);
      }
    }
    else if(all[i]->type == TQLCDNumber)
    {
      QLCDNumber *iw = (QLCDNumber *) all[i]->w;
      if(iw != NULL) iw->resize(w,h);
    }
    else
    {
      if(all[i]->w != NULL) all[i]->w->resize(w,h);
    }
  }
  else if(strncmp(command,"removeItem(",11) == 0)
  {
    int index;
    sscanf(command,"removeItem(%d,%d",&i,&index);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListBox)
    {
      MyListBox *ptr = (MyListBox *) all[i]->w;
      if(ptr != NULL) ptr->removeItem(index);
    }
    else if(all[i]->type == TQComboBox)
    {
      MyComboBox *ptr = (MyComboBox *) all[i]->w;
      if(ptr != NULL) ptr->removeItem(index);
    }
  }
  else if(strncmp(command,"removeItemByName(",17) == 0)
  {
    sscanf(command,"removeItemByName(%d",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListBox)
    {
      MyListBox *ptr = (MyListBox *) all[i]->w;
      if(ptr != NULL) ptr->removeItemByName(text);
    }
    else if(all[i]->type == TQComboBox)
    {
      MyComboBox *ptr = (MyComboBox *) all[i]->w;
      if(ptr != NULL) ptr->removeItemByName(text);
    }
  }
  else if(strncmp(command,"removeListViewItem(",19) == 0)
  {
    sscanf(command,"removeListViewItem(%d",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListView)
    {
      MyListView *ptr = (MyListView *) all[i]->w;
      if(ptr != NULL) ptr->deleteListViewItem(text.toUtf8(), (MyListViewItem *) ptr->firstChild(NULL));
    }
  }
  else if(strncmp(command,"removeIconViewItem(",19) == 0)
  {
    sscanf(command,"removeIconViewItem(%d",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQIconView)
    {
      MyIconView *ptr = (MyIconView *) all[i]->w;
      if(ptr != NULL) ptr->deleteIconViewItem(text);
    }
  }
  else if(strncmp(command,"requestGeometry(",16) == 0)
  {
    char buf[MAX_PRINTF_LENGTH];
    sscanf(command,"requestGeometry(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    QWidget *ptr = all[i]->w;
    if(ptr != NULL)
    {
      sprintf(buf,"text(%d,\"geometry:%d,%d,%d,%d\n", i, ptr->x(), ptr->y(), ptr->width(), ptr->height());
      tcp_send(s,buf,strlen(buf));
    }
  }
  else if(strncmp(command,"requestParent(",14) == 0)
  {
    char buf[MAX_PRINTF_LENGTH];
    sscanf(command,"requestParent(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    QWidget *ptr = all[i]->w;
    if(ptr != NULL)
    {
      QWidget *parent = (QWidget *) ptr->parent();
      for(int pid=0; pid<=nmax; pid++)
      {
        if(parent == all[pid]->w)
        {
          sprintf(buf,"text(%d,\"parent:%d\n", i, pid);
          tcp_send(s,buf,strlen(buf));
          return;
        }
      }
      sprintf(buf,"text(%d,\"parent:%d\n", i, -1);
      tcp_send(s,buf,strlen(buf));
    }
  }
  else if(strncmp(command,"requestSvgBoundsOnElement(",26) == 0)
  {
    sscanf(command,"requestSvgBoundsOnElement(%d",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      char buf[MAX_PRINTF_LENGTH];
      QDrawWidget *ptr = (QDrawWidget *) all[i]->w;
      if(ptr != NULL)
      {
        QRectF rectf = ptr->renderer.boundsOnElement(text);
        sprintf(buf,"text(%d,\"svgBoundsOnElement:%f,%f,%f,%f=%s\"\n", i, rectf.x(), rectf.y(), rectf.width(), rectf.height(), (const char *) text.toUtf8());
        tcp_send(s,buf,strlen(buf));
      }
    }
  }
  else if(strncmp(command,"requestSvgMatrixForElement(",27) == 0)
  {
    sscanf(command,"requestSvgMatrixForElement(%d",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQDraw)
    {
      char buf[MAX_PRINTF_LENGTH];
      QDrawWidget *ptr = (QDrawWidget *) all[i]->w;
      if(ptr != NULL)
      {
        QMatrix m = ptr->renderer.matrixForElement(text);
        sprintf(buf,"text(%d,\"svgMatrixForElement:%f,%f,%f,%f,%f,%f,%f=%s\"\n", i, 
        m.m11(), m.m12(), m.m21(), m.m22(), m.det(), m.dx(), m.dy(), (const char *) text.toUtf8());
        tcp_send(s,buf,strlen(buf));
      }
    }
  }
}

void Interpreter::interprets(const char *command)
{
  if(strncmp(command,"set",3) == 0)
  {
    switch(command[3])
    {
      case 'A':
        if(strncmp(command,"setAlignment(",13) == 0) // set text alignment
        {
          int alignment,qtalignment;
          sscanf(command,"setAlignment(%d,%d",&i,&alignment);
          if(i < 0) return;
          if(i >= nmax) return;

          qtalignment = 0;

          if(alignment & PV::AlignLeft)    qtalignment |= Qt::AlignLeft;
          if(alignment & PV::AlignRight)   qtalignment |= Qt::AlignRight;
          if(alignment & PV::AlignHCenter) qtalignment |= Qt::AlignHCenter;
          if(alignment & PV::AlignJustify) qtalignment |= Qt::AlignJustify;

          if(alignment & PV::AlignTop)     qtalignment |= Qt::AlignTop;
          if(alignment & PV::AlignBottom)  qtalignment |= Qt::AlignBottom;
          if(alignment & PV::AlignVCenter) qtalignment |= Qt::AlignVCenter;

          if(all[i]->type == TQLabel)
          {
            MyLabel *ptr = (MyLabel *) all[i]->w;
            if(ptr != NULL) ptr->setAlignment((Qt::Alignment) qtalignment);
          }
          else if(all[i]->type == TQLineEdit)
          {
            QLineEdit *ptr = (QLineEdit *) all[i]->w;
            if(ptr != NULL) ptr->setAlignment((Qt::Alignment) qtalignment);
          }
        }
        break;
      case 'B':
        if(strncmp(command,"setBackgroundColor(",19) == 0)
        {
          sscanf(command,"setBackgroundColor(%d,%d,%d,%d)",&i,&r,&g,&b);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQPushButton)
          {
            QPushButton *but = (QPushButton *) all[i]->w;
            if(but != NULL)
            {
              QPixmap pm = QPixmap(but->width()/2,but->height());
              pm.fill(QColor(r,g,b));
              but->setIcon(pm);
            }
          }
          else
          {
            if(all[i]->w != NULL) mySetBackgroundColor(all[i]->w,all[i]->type,r,g,b);
          }
        }
        else if(strncmp(command,"setBoxSelected(",15) == 0) // set selection of ListBox
        {
          int index,mode;
          sscanf(command,"setBoxSelected(%d,%d,%d",&i,&index,&mode);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListBox)
          {
            MyListBox *lb = (MyListBox *) all[i]->w;
            if(lb != NULL) lb->item(index)->setSelected(mode);
          }
        }
        break;
      case 'C':
        if(strncmp(command,"setChecked(",11) == 0) // set button state
        {
          int state;
          sscanf(command,"setChecked(%d,%d)",&i,&state);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQRadio)
          {
            MyRadioButton *but = (MyRadioButton *) all[i]->w;
            if(but != NULL)
            {
              if(state == 1) but->setChecked(true);
              else           but->setChecked(false);
            }
          }
          else if(all[i]->type == TQCheck)
          {
            MyCheckBox *but = (MyCheckBox *) all[i]->w;
            if(but != NULL)
            {
              if(state == 1) but->setChecked(true);
              else           but->setChecked(false);
            }
          }
        }
        else if(strncmp(command,"setCurrentItem(",15) == 0) // set current item for comboBox
        {
          int index;
          sscanf(command,"setCurrentItem(%d,%d",&i,&index);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQComboBox)
          {
            MyComboBox *t = (MyComboBox *) all[i]->w;
            if(t != NULL) t->setCurrentIndex(index);
          }
        }
        else if(strncmp(command,"setColumnWidth(",15) == 0) // set column with of the table
        {
          int column, width;
          sscanf(command,"setColumnWidth(%d,%d,%d",&i,&column,&width);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            if(column >= 0)
            {
              if(t != NULL)
              {
                t->setColumnWidth(column,width);
              }  
            }
            else
            {
              if(opt.arg_debug) printf("setColumnWidth leftBorder=%d\n",width);
              //t->setLeftMargin(width);
              if(t != NULL)
              { // we have to fool Qt4, because currently it does not support this
                if(width == 0)     // hide 
                {
                  t->verticalHeader()->hide();
                }
                else if(width > 0) // set fixed width
                {
                  t->verticalHeader()->setResizeMode(QHeaderView::Fixed);
                  t->verticalHeader()->setFixedWidth(width);
                  t->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
                  t->verticalHeader()->resizeSection(0,width);
                  t->verticalHeader()->setResizeMode(QHeaderView::Fixed);
                }
                else               // resize to contents
                {
                  t->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
                  t->verticalHeader()->setMinimumWidth(0);
                  t->verticalHeader()->setMaximumWidth(99999);
                  t->verticalHeader()->resizeSection(0,width);
                  t->verticalHeader()->setResizeMode(QHeaderView::Fixed);
                }
              }
            }
          }
        }
        break;
      case 'D':
        if(strncmp(command,"setDate(",8) == 0) // set date
        {
          int year, month, day;
          sscanf(command,"setDate(%d,%d,%d,%d)",&i,&year,&month,&day);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQDateEdit)
          {
            QDateEdit *e = (QDateEdit *) all[i]->w;
            if(e != NULL) e->setDate(QDate(year,month,day));
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            if(e != NULL) e->setDate(QDate(year,month,day));
          }
        }
        else if(strncmp(command,"setDateOrder(",13) == 0) // set date order
        {
          int order;
          sscanf(command,"setDateOrder(%d,%d)",&i,&order);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQDateEdit)
          {
            QDateEdit *e = (QDateEdit *) all[i]->w;
            switch(order)
            {
              case PV::YDM:
                if(e != NULL) e->setDisplayFormat("yyyy.dd.MM");
                break;
              case PV::YMD:
                if(e != NULL) e->setDisplayFormat("yyyy.MM.dd");
                break;
              case PV::MDY:
                if(e != NULL) e->setDisplayFormat("MM.dd.yyyy");
                break;
              case PV::DMY:
              default:
                if(e != NULL) e->setDisplayFormat("dd.MM.yyyy");
                break;
            }
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            switch(order)
            {
              case PV::YDM:
                if(e != NULL) e->setDisplayFormat("yyyy.dd.MM");
                break;
              case PV::YMD:
                if(e != NULL) e->setDisplayFormat("yyyy.MM.dd");
                break;
              case PV::MDY:
                if(e != NULL) e->setDisplayFormat("MM.dd.yyyy");
                break;
              case PV::DMY:
              default:
                if(e != NULL) e->setDisplayFormat("dd.MM.yyyy");
                break;
            }
          }
        }
        break;
      case 'E':
        if(strncmp(command,"setEchoMode(",12) == 0) // set echo mode in QLineEdit
        {
          int mode;
          sscanf(command,"setEchoMode(%d,%d",&i,&mode);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQLineEdit)
          {
            MyLineEdit *le = (MyLineEdit *) all[i]->w;
            if(le != NULL)
            {
              if     (mode == 0) le->setEchoMode(QLineEdit::NoEcho);
              else if(mode == 2) le->setEchoMode(QLineEdit::Password);
              else               le->setEchoMode(QLineEdit::Normal);
            }
          }
        }
        else if(strncmp(command,"setEditable(",12) == 0) // set editable in QLineEdit, QMultiLineEdit
        {
          int mode;
          sscanf(command,"setEditable(%d,%d",&i,&mode);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQLineEdit)
          {
            MyLineEdit *le = (MyLineEdit *) all[i]->w;
            if(le != NULL)
            {
              if     (mode == 0) le->setReadOnly(true);
              else if(mode == 1) le->setReadOnly(false);
            }
          }
          else if(all[i]->type == TQMultiLineEdit)
          {
            MyMultiLineEdit *le = (MyMultiLineEdit *) all[i]->w;
            if(le != NULL)
            {
              if     (mode == 0) le->setReadOnly(true);
              else if(mode == 1) le->setReadOnly(false);
            }
          }
          else if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            //if     (mode == 0) t->setReadOnly(true);
            //else if(mode == 1) t->setReadOnly(false);
            //if     (mode == 0) t->setEnabled(false);
            //else if(mode == 1) t->setEnabled(true);
            if(t != NULL) t->setEditable(mode);
          }
          else if(all[i]->type == TQComboBox)
          {
            MyComboBox *cb = (MyComboBox *) all[i]->w;
            if(cb != NULL)
            {
              if     (mode == 0) cb->setEditable(false);
              else if(mode == 1) cb->setEditable(true);
            }
          }
        }
        else if(strncmp(command,"setEnabled(",11) == 0) // set enabled all widgets
        {
          int mode;
          sscanf(command,"setEnabled(%d,%d",&i,&mode);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->w != NULL) all[i]->w->setEnabled(mode);
        }
        break;
      case 'F':
        if(strncmp(command,"setFont(",8) == 0) // set font
        {
          int pointsize,bold,italic,underline,strikeout;
          sscanf(command,"setFont(%d,%d,%d,%d,%d,%d",&i,&pointsize,&bold,&italic,&underline,&strikeout);
          get_text(command,text); // text = family of font

          if(i < 0) return;
          if(i >= nmax) return;
          if(zoom != 100 || opt.fontzoom != 100)
          {
            pointsize = (pointsize*zoom*opt.fontzoom)/(100*100);
          }
          QFont font(text);
          font.setPointSize(pointsize);
          if(bold      == 0) font.setBold(false);
          if(italic    == 0) font.setItalic(false);
          if(underline == 0) font.setUnderline(false);
          if(strikeout == 0) font.setStrikeOut(false);
          if(bold      == 1) font.setBold(true);
          if(italic    == 1) font.setItalic(true);
          if(underline == 1) font.setUnderline(true);
          if(strikeout == 1) font.setStrikeOut(true);
          if(all[i]->type == TQMultiLineEdit)
          {
            MyMultiLineEdit *me = (MyMultiLineEdit *) all[i]->w;
            if(me != NULL) me->setCurrentFont(font);
          }
          else
          {
            QWidget *w = (QWidget *) all[i]->w;
            if(w != NULL) w->setFont(font);
          }
        }
        else if(strncmp(command,"setFontColor(",13) == 0)
        {
          int r,g,b;
          sscanf(command,"setFontColor(%d,%d,%d,%d",&i,&r,&g,&b);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQMultiLineEdit)
          {
            MyMultiLineEdit *me = (MyMultiLineEdit *) all[i]->w;
            if(me != NULL) me->setTextColor(QColor(r,g,b));
          }
          else
          {
            if(all[i]->w != NULL) mySetForegroundColor(all[i]->w,all[i]->type,r,g,b);
          }
        }
        break;
      case 'G':
        if(strncmp(command,"setGeometry(",12) == 0) // set widget geometry
        {
          sscanf(command,"setGeometry(%d,%d,%d,%d,%d)",&i,&x,&y,&w,&h);
          if(zoom != 100 && hasLayout == 0)
          {
            x = (x*zoom)/100;
            y = (y*zoom)/100;
            w = (w*zoom)/100;
            h = (h*zoom)/100;
            if(w < 1) w = 1;
            if(h < 1) h = 1;
          }
          if(i == 0)
          {
            printf("view->resize2 not implemented\n");
            //view->resize(w,h);
            //if(scroll != NULL)
            //{
            //  scroll->resize(w,h);
            //  scroll->resizeContents(w,h);
            //}
          }
          if(i < 0) 
          {
            int id_dock = i - DOCK_WIDGETS_BASE;
            if( id_dock >= 0 && id_dock < MAX_DOCK_WIDGETS)
            {
              MyQDockWidget *dock = mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock]; 
              x += mainWindow->x();
              y += mainWindow->y();
              QWidget *cw = mainWindow->centralWidget();
              if(cw != NULL)
              {
                x += cw->x();
                y += cw->y();
              }
              if(dock != NULL)
              {
                dock->w = w;
                dock->h = h;
                dock->setGeometry(x,y,w,h);
              }  
            }
            return;
          }  
          if(i >= nmax) return;
          if(all[i]->w != NULL)
          { // remember original geometry
            all[i]->x      = x;
            all[i]->y      = y;
            all[i]->width  = w;
            all[i]->height = h;
          }  
          if(all[i]->type == TQImage)
          {
            QImageWidget *iw = (QImageWidget *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQDraw)
          {
            QDrawWidget *iw = (QDrawWidget *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
            if(iw != NULL) iw->resize(w,h);
            if(zoom != 100)
            {
              float fzoom = ((float)zoom)/100.0f;
              if(iw != NULL) iw->setZoomX(fzoom);
              if(iw != NULL) iw->setZoomY(fzoom);
            }
          }
          else if(all[i]->type == TQLCDNumber)
          {
            QLCDNumber *iw = (QLCDNumber *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
#ifdef USE_OPEN_GL
          else if(all[i]->type == TQGl)
          {
            PvGLWidget *iw = (PvGLWidget *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
#endif
#ifdef USE_VTK
          else if(all[i]->type == TQVtk)
          {
            pvVtkTclWidget *iw = (pvVtkTclWidget *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
#endif
#ifndef NO_QWT
          else if(all[i]->type == TQwtPlotWidget)
          {
            QwtPlotWidget *iw = (QwtPlotWidget *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtScale)
          {
            QwtScaleWidget *iw = (QwtScaleWidget *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtThermo)
          {
            QwtThermo *iw = (QwtThermo *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtKnob)
          {
            QwtKnob *iw = (QwtKnob *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtCounter)
          {
            QwtCounter *iw = (QwtCounter *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtWheel)
          {
            QwtWheel *iw = (QwtWheel *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtSlider)
          {
            QwtSlider *iw = (QwtSlider *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtDial)
          {
            QwtDial *iw = (QwtDial *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtCompass)
          {
            QwtCompass *iw = (QwtCompass *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
          else if(all[i]->type == TQwtAnalogClock)
          {
            QwtAnalogClock *iw = (QwtAnalogClock *) all[i]->w;
            if(iw != NULL) iw->setGeometry(x,y,w,h);
          }
#endif
          else
          {
            if(all[i]->w != NULL)
            {
              if(all[i]->w != NULL) all[i]->w->setGeometry(x,y,w,h);
            }
          }
        }
        break;
      case 'H':
        if(strncmp(command,"setHidden(",10) == 0) // hide column
        {
          int col, hidden;
          sscanf(command,"setHidden(%d,%d,%d,",&i,&col,&hidden);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListView)
          {
            MyListView *ptr = (MyListView *) all[i]->w;
            if(hidden) ptr->hideColumn(col);
            else       ptr->showColumn(col);
          }
        }  
        break;
      case 'I':
        if(strncmp(command,"setIconViewItem(",16) == 0) // set text and pixmap in QIconView
        {
          //QPixmap pm;
          char *data;

          sscanf(command,"setIconViewItem(%d,%d,%d,",&i,&w,&h);
          get_text(command,text);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQIconView)
          {
            MyIconView *iv = (MyIconView*) all[i]->w;
            if(w==0 && h==0)
            {
              QPixmap pm;
              if(iv != NULL) iv->setIconViewItem(text,pm);
            }
            else
            {
              QImage *temp = readBmpFromSocket(w,h,&data);
              QPixmap pm; //pm(w,h);
              if(opt.arg_debug) printf("setIconViewItem 1\n");
              if(temp != NULL)
              {
                myCreateHeuristicMask(pm,temp);
              }
              if(iv != NULL) iv->setIconViewItem(text,pm);
              if(opt.arg_debug) printf("setIconViewItem 2\n");
              if(data != NULL) delete [] data;
              if(temp != NULL) delete temp;
              if(opt.arg_debug) printf("setIconViewItem 3\n");
            }
          }
          else
          {
            if(w==0 && h==0) ;
            else
            {
              QImage *temp = readBmpFromSocket(w,h,&data);
              if(temp != NULL) delete temp;
              if(data != NULL) delete [] data;
            }
          }
        }
        else if(strncmp(command,"setImage(",9) == 0) // set new image in existing image
        {
          sscanf(command,"setImage(%d,",&i);
          get_text(command,text); // text = filename of image
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQImage)
          {
            QString filename;
            filename = temp + text;
            QImageWidget *iw = (QImageWidget *) all[i]->w;
            if(iw != NULL) 
            {
              iw->setImage(filename.toUtf8());
            }  
          }
        }
        break;
      case 'J':
        break;
      case 'K':
        break;
      case 'L':
        if(strncmp(command,"setListViewPixmap(",18) == 0) // set pixmap in list view
        {
          //QPixmap pm;
          char *data;
          int column;
          sscanf(command,"setListViewPixmap(%d,%d,%d,%d,",&i,&w,&h,&column);
          get_text(command,text);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListView)
          {
            MyListView *lv = (MyListView*) all[i]->w;
            if(w==0 && h==0)
            {
              QPixmap pm;
              if(lv != NULL) lv->setListViewPixmap(text.toUtf8(),column,pm);
            }
            else
            {
              QImage *temp = NULL;
              temp = readBmpFromSocket(w,h,&data);
              QPixmap pm; //pm(w,h);
              if(temp != NULL)
              {
                myCreateHeuristicMask(pm,temp);
              }
              if(lv != NULL) lv->setListViewPixmap(text.toUtf8(),column,pm);
              if(data != NULL) delete [] data;
              if(temp != NULL) delete temp;
            }
          }
          else
          {
            if(w==0 && h==0) ;
            else
            {
              QImage *temp = NULL;
              temp= readBmpFromSocket(w,h,&data);
              if(temp != NULL) delete temp;
              if(data != NULL) delete [] data;
            }
          }
        }
        else if(strncmp(command,"setListViewText(",16) == 0) // set text in QListView
        {
          int column;
          QString path;
          char text2[MAX_PRINTF_LENGTH];
          sscanf(command,"setListViewText(%d,%d",&i,&column);
          get_text(command,path);
          tcp_rec(s,text2,sizeof(text2));
          get_text(text2,text);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListView)
          {
            MyListView *lv = (MyListView*) all[i]->w;
            if(lv != NULL) lv->setListViewText(path.toUtf8(),column,text);
          }
        }
        break;
      case 'M':
        if(strncmp(command,"setMinimumSize(",15) == 0) // set minimum w,h
        {
          sscanf(command,"setMinimumSize(%d,%d,%d)",&i,&w,&h);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->w != NULL) all[i]->w->setMinimumSize(w,h);
        }
        else if(strncmp(command,"setMaximumSize(",15) == 0) // set maximum w,h
        {
          sscanf(command,"setMaximumSize(%d,%d,%d)",&i,&w,&h);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->w != NULL) all[i]->w->setMaximumSize(w,h);
        }
        else if(strncmp(command,"setMouseShape(",14) == 0) // set mouse shape
        {
          int shape = 0;
          sscanf(command,"setMouseShape(%d",&shape);
#ifdef PVWIN32
          // damn windows may reset cursor otherwise
          QApplication::processEvents();
#endif
          QApplication::restoreOverrideCursor();
          QApplication::setOverrideCursor(QCursor((Qt::CursorShape) shape));
        }
        else if(strncmp(command,"setMultiSelection(",18) == 0) // set selection mode in QListView
        {
          int mode;
          sscanf(command,"setMultiSelection(%d,%d",&i,&mode);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListView)
          {
            MyListView *lv = (MyListView *) all[i]->w;
            if(lv != NULL) lv->setMultiSelection(mode);
          }
          else if(all[i]->type == TQListBox)
          {
            MyListBox *lb = (MyListBox *) all[i]->w;
            if(lb != NULL)
            {
              if      (mode == 1) lb->setSelectionMode(QAbstractItemView::MultiSelection);
              else if (mode == 2) lb->setSelectionMode(QAbstractItemView::NoSelection);
              else                lb->setSelectionMode(QAbstractItemView::SingleSelection);
            }
          }
        }
        else if(strncmp(command,"setMinDate(",11) == 0) // set min date
        {
          int year, month, day;
          sscanf(command,"setMinDate(%d,%d,%d,%d)",&i,&year,&month,&day);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQDateEdit)
          {
            QDateEdit *e = (QDateEdit *) all[i]->w;
            if(e != NULL) e->setMinimumDate(QDate(year,month,day));
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            if(e != NULL) e->setMinimumDate(QDate(year,month,day));
          }
        }
        else if(strncmp(command,"setMaxDate(",11) == 0) // set max date
        {
          int year, month, day;
          sscanf(command,"setMaxDate(%d,%d,%d,%d)",&i,&year,&month,&day);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQDateEdit)
          {
            QDateEdit *e = (QDateEdit *) all[i]->w;
            if(e != NULL) e->setMaximumDate(QDate(year,month,day));
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            if(e != NULL) e->setMaximumDate(QDate(year,month,day));
          }
        }
        else if(strncmp(command,"setMinTime(",11) == 0) // set min time
        {
          int hour, minute, second, msec;
          sscanf(command,"setMinTime(%d,%d,%d,%d,%d)",&i,&hour,&minute,&second,&msec);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTimeEdit)
          {
            QTimeEdit *e = (QTimeEdit *) all[i]->w;
            if(e != NULL) e->setMinimumTime(QTime(hour,minute,second,msec));
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            if(e != NULL) e->setMinimumTime(QTime(hour,minute,second,msec));
          }
        }
        else if(strncmp(command,"setMaxTime(",11) == 0) // set max time
        {
          int hour, minute, second, msec;
          sscanf(command,"setMaxTime(%d,%d,%d,%d,%d)",&i,&hour,&minute,&second,&msec);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTimeEdit)
          {
            QTimeEdit *e = (QTimeEdit *) all[i]->w;
            if(e != NULL) e->setMaximumTime(QTime(hour,minute,second,msec));
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            if(e != NULL) e->setMaximumTime(QTime(hour,minute,second,msec));
          }
        }
        break;
      case 'N':
        if(strncmp(command,"setNumRows(",11) == 0) // set table num rows
        {
          int num;
          sscanf(command,"setNumRows(%d,%d",&i,&num);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            //t->setNumRows(num);
            if(t != NULL) t->setRowCount(num);
          }
        }
        else if(strncmp(command,"setNumCols(",11) == 0) // set table num cols
        {
          int num;
          sscanf(command,"setNumCols(%d,%d",&i,&num);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            //t->setNumCols(num);
            if(t != NULL) t->setColumnCount(num);
          }
        }
        break;
      case 'O':
        if(strncmp(command,"setOpen(",8) == 0)
        {
          int open;
          sscanf(command,"setOpen(%d,%d",&i,&open);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListView)
          {
            get_text(command,text);
            MyListView *l = (MyListView *) all[i]->w;
            if(l != NULL) l->setItemOpen(text.toUtf8(),open, (MyListViewItem *) l->firstChild(NULL));
          }
        }
        break;
      case 'P':
        if(strncmp(command,"setParent(",10) == 0)
        {
          int iparent = 0;
          sscanf(command,"setParent(%d,%d)",&i,&iparent);
          if(i < 0) return;
          if(i >= nmax) return;
          if(iparent < 0) return;
          if(iparent >= nmax) return;
          if(all[i] == NULL || all[iparent] == NULL) return;
          if(all[i]->w != NULL && all[iparent]->w != NULL) 
          {
            QWidget *oldparent = all[i]->w->parentWidget();
            QWidget *newparent = all[iparent]->w;
            if(oldparent != newparent) // if not already parent
            {
              int x = all[iparent]->w->x();
              int y = all[iparent]->w->y();
              int width = all[iparent]->w->width();
              int height = all[iparent]->w->height();

              all[i]->w->setParent(newparent);

              all[i]->w->setGeometry(x,y,width,height);
            }  
          }  
        }
        else if(strncmp(command,"setPixmap(",10) == 0) // set pixmap
        {
          //QPixmap pm;
          char *data;
          sscanf(command,"setPixmap(%d,%d,%d)",&i,&w,&h);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQPushButton)
          {
            QPushButton *b = (QPushButton*) all[i]->w;
            if(w==0 && h==0)
            {
              QPixmap pm;
              if(b != NULL) b->setIcon(pm);
            }
            else
            {
              QImage *temp = readBmpFromSocket(w,h,&data);
              QPixmap pm; //pm(w,h);
              if(temp != NULL)
              {
                myCreateHeuristicMask(pm,temp);
              }
              if(b != NULL) b->setIcon(pm);
              if(data != NULL) delete [] data;
              if(temp != NULL) delete temp;
            }
          }
          else
          {
            if(w==0 && h==0) ;
            else
            {
              QImage *temp = readBmpFromSocket(w,h,&data);
              if(temp != NULL) delete temp;
              if(data != NULL) delete [] data;
            }
          }
        }
        else if(strncmp(command,"setPaletteBackgroundColor(",26) == 0)
        {
          sscanf(command,"setPaletteBackgroundColor(%d,%d,%d,%d)",&i,&r,&g,&b);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->w != NULL) mySetBackgroundColor(all[i]->w,all[i]->type,r,g,b);
        }
        else if(strncmp(command,"setPaletteForegroundColor(",26) == 0)
        {
          sscanf(command,"setPaletteForegroundColor(%d,%d,%d,%d)",&i,&r,&g,&b);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->w != NULL) mySetForegroundColor(all[i]->w,all[i]->type,r,g,b);
        }
        break;
      case 'Q':
        break;
      case 'R':
        if(strncmp(command,"setRowHeight(",13) == 0) // set row height of the table
        {
          int row, height;
          sscanf(command,"setRowHeight(%d,%d,%d",&i,&row,&height);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            if(opt.arg_debug) printf("setRowHeight height=%d\n",height);
            MyTable *t = (MyTable *) all[i]->w;
            if(height < 0)
            {
              if(t != NULL)
              {
                if(height == -1) t->autoresize = 1;
                if(height == -2) t->autoresize = 0;
              }              
            }
            else if(row >= 0)
            {
              if(t != NULL)
              {
                t->verticalHeader()->resizeSection(row,height);
              }
            }
            else
            {
              if(t != NULL)
              {
                t->horizontalHeader()->resize(t->horizontalHeader()->width(),height);
                //if(t->cornerWidget() == NULL) t->setCornerWidget(new QWidget);
                //t->cornerWidget()->setMinimumWidth(t->verticalHeader()->width());
                //t->cornerWidget()->setMinimumHeight(t->horizontalHeader()->height());
              }
            }
          }
        }
        break;
      case 'S':
        if(strncmp(command,"setSelected(",12) == 0) // set selection of ListView
        {
          int col, mode; // martin@bangieff "col not used ???"
          sscanf(command,"setSelected(%d,%d,%d",&i,&col,&mode);
          get_text(command,text); // text = path
          if(i < 0) return;
          if(i >= nmax) return;
          if(opt.arg_debug > 0) printf("setSelected(%d,%d,%d,%s)\n",i,col,mode, (const char *) text.toUtf8());
          if(all[i]->type == TQListView)
          {
            MyListView *lv = (MyListView *) all[i]->w;
            if(opt.arg_debug > 0) printf("setSelected 1\n");
            if(lv != NULL) lv->setSelected(mode,text.toUtf8());
            if(opt.arg_debug > 0) printf("setSelected 2\n");
          }
        }
        else if(strncmp(command,"setSelector(",12) == 0) // set selector in QDraw
        {
          int state;
          sscanf(command,"setSelector(%d,%d",&i,&state);
          if(i < 0) return;
          if(i >= nmax) return;
          if(opt.arg_debug > 0) printf("setSelector(%d,%d)\n",i,state);
          if(all[i]->type == TQDraw)
          {
            QDrawWidget *lv = (QDrawWidget *) all[i]->w;
            if(lv != NULL)
            {
              if(state == 0) lv->selectorState = 0;
              else           lv->selectorState = 1;
            }
          }
        }
        else if(strncmp(command,"setSource(",10) == 0) // set source of QTextBrowser
        {
          sscanf(command,"setSource(%d",&i);
          get_text(command,text); // text = html filename
          if(i == -5) // ID_HELP
          {
            if(mainWindow->textbrowser == NULL) mainWindow->textbrowser = new dlgTextBrowser;
            mainWindow->textbrowser->form->textBrowser->load(QUrl(text));
#ifdef USE_ANDROID
            // android permission problems
            // google does not allow Qt to access local storage
            // see: http://www.techjini.com/blog/2009/01/10/android-tip-1-contentprovider-accessing-local-file-system-from-webview-showing-image-in-webview-using-content/
            //      http://groups.google.com/group/android-developers/msg/45977f54cf4aa592
            if(strstr(text.toUtf8(),"://") == NULL)
            {
              struct stat sb;
              if(stat(text.toUtf8(), &sb) < 0) return;
              char buf[sb.st_size+1];
              FILE *fin = fopen(text.toUtf8(),"r");
              if(fin == NULL) return;
              fread(buf,1,sb.st_size,fin);
              fclose(fin);
              mainWindow->textbrowser->form->textBrowser->setHtml(buf);
            }
            else
            {
              mainWindow->textbrowser->form->textBrowser->load(QUrl(text));
            }
#else
            mainWindow->textbrowser->form->textBrowser->load(QUrl(text));
#endif
            return;
          }
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTextBrowser)
          {
            MyTextBrowser *t = (MyTextBrowser *) all[i]->w;
            if(t != NULL) 
            {
#ifdef USE_ANDROID
              // android permission problems
              // google does not allow Qt to access local storage
              // see: http://www.techjini.com/blog/2009/01/10/android-tip-1-contentprovider-accessing-local-file-system-from-webview-showing-image-in-webview-using-content/
              //      http://groups.google.com/group/android-developers/msg/45977f54cf4aa592
              if(strstr(text.toUtf8(),"://") == NULL)
              {
                  struct stat sb;
                  if(stat(text.toUtf8(), &sb) < 0) return;
                  char buf[sb.st_size+1];
                  FILE *fin = fopen(text.toUtf8(),"r");
                  if(fin == NULL) return;
                  fread(buf,1,sb.st_size,fin);
                  fclose(fin);
                  t->setHtml(buf);
              }
              else
              {
                t->load(QUrl(text));
              }
#else
              t->load(QUrl(text));
#endif
              if(t->homeIsSet == 0)
              {
                t->home      = text;
                t->homeIsSet = 1;
                if(opt.arg_debug) printf("home=%s\n", (const char *) text.toUtf8());
              }
              //tb t->setSource(QUrl::fromLocalFile(temp + text));
              //tb t->reload();
            }
          }
        }
        else if(strncmp(command,"setSorting(",11) == 0) // set sorting of ListView
        {
          int col,mode;
          sscanf(command,"setSorting(%d,%d,%d",&i,&col,&mode);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListView)
          {
            MyListView *lv = (MyListView *) all[i]->w;
            if(lv != NULL) lv->setSorting(col,mode);
          }
        }
        else if(strncmp(command,"setStandardPopup(",17) == 0) // set popup menu
        {
          int menu;
          sscanf(command,"setStandardPopup(%d,%d",&i,&menu);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQListView)
          {
            MyListView *lv = (MyListView *) all[i]->w;
            if(lv != NULL) lv->hasStandardPopupMenu = menu;
          }
        }
        else if(strncmp(command,"setStyle(",9) == 0) // set style of QLabel QFrame
        {
          int shape,shadow,line_width,margin;
          sscanf(command,"setStyle(%d,%d,%d,%d,%d",&i,&shape,&shadow,&line_width,&margin);
          if(i < 0) return;
          if(i >= nmax) return;
          switch(shape)
          {
            case PV::NoFrame:
              shape = QFrame::NoFrame;
              break;
            case PV::Box:
              shape = QFrame::Box;
              break;
            case PV::Panel:
              shape = QFrame::Panel;
              break;
            case PV::WinPanel:
              shape = QFrame::WinPanel;
              break;
            case PV::HLine:
              shape = QFrame::HLine;
              break;
            case PV::VLine:
              shape = QFrame::VLine;
              break;
            case PV::StyledPanel:
              shape = QFrame::StyledPanel;
              break;
            case PV::PopupPanel:
              shape = QFrame::NoFrame;
              break;
            case PV::MenuBarPanel:
              shape = QFrame::NoFrame;
              break;
            case PV::ToolBarPanel:
              shape = QFrame::NoFrame;
              break;
            case PV::LineEditPanel:
              shape = QFrame::NoFrame;
              break;
            case PV::TabWidgetPanel:
              shape = QFrame::NoFrame;
              break;
            case PV::GroupBoxPanel:
              shape = QFrame::NoFrame;
              break;
            case PV::MShape:
              shape = QFrame::NoFrame;
              break;
            default:
              shape = -1;
              break;
          }
          switch(shadow)
          {
            case PV::Plain:
              shadow = QFrame::Plain;
              break;
            case PV::Raised:
              shadow = QFrame::Raised;
              break;
            case PV::Sunken:
              shadow = QFrame::Sunken;
              break;
            case PV::MShadow:
              shadow = QFrame::Plain;
              break;
            default:
              shadow = -1;
              break;
          }
          if(all[i]->type == TQLabel)
          {
            MyLabel *ptr = (MyLabel *) all[i]->w;
            if(ptr != NULL)
            {
              if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
              if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
              if(line_width != -1) ptr->setLineWidth(line_width);
              if(margin     != -1) ptr->setMargin(margin);
            }
          }
          else if(all[i]->type == TQLineEdit)
          {
            //trollmurx MyLineEdit *ptr = (MyLineEdit *) all[i]->w;
            //trollmurx if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
            //trollmurx if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
            //trollmurx if(line_width != -1) ptr->setLineWidth(line_width);
            //trollmurx if(margin     != -1) ptr->setMargin(margin);
          }
          else if(all[i]->type == TQFrame)
          {
            MyFrame *ptr = (MyFrame *) all[i]->w;
            if(ptr != NULL)
            {
              if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
              if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
              if(line_width != -1) ptr->setLineWidth(line_width);
              //trollmurx if(margin     != -1) ptr->setMargin(margin);
            }
          }
          else if(all[i]->type == TQGroupBox)
          {
            //trollmurx MyGroupBox *ptr = (MyGroupBox *) all[i]->w;
            //trollmurx if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
            //trollmurx if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
            //trollmurx if(line_width != -1) ptr->setLineWidth(line_width);
            //trollmurx if(margin     != -1) ptr->setMargin(margin);
          }
          else if(all[i]->type == TQLCDNumber)
          {
            //trollmurx QLCDNumber *ptr = (QLCDNumber *) all[i]->w;
            //trollmurx if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
            //trollmurx if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
            //trollmurx if(line_width != -1) ptr->setLineWidth(line_width);
            //trollmurx if(margin     != -1) ptr->setMargin(margin);
          }
          else if(all[i]->type == TQProgressBar)
          {
            //trollmurx MyProgressBar *ptr = (MyProgressBar *) all[i]->w;
            //trollmurx if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
            //trollmurx if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
            //trollmurx if(line_width != -1) //trollmurx ptr->setLineWidth(line_width);
            //trollmurx if(margin     != -1) ptr->setMargin(margin);
          }
          else if(all[i]->type == TQToolBox)
          {
            //trollmurx MyQToolBox *ptr = (MyQToolBox *) all[i]->w;
            //trollmurx if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
            //trollmurx if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
            //trollmurx if(line_width != -1) ptr->setLineWidth(line_width);
            //trollmurx if(margin     != -1) ptr->setMargin(margin);
          }
        }
        break;
      case 'T':
        if(strncmp(command,"setText(",8) == 0) // set widget text
        {
          sscanf(command,"setText(%d,",&i);
          if(i >= nmax && i > 0) return;
          get_text(command,text);
          if(opt.arg_debug) printf("setText text='%s'\n",(const char *)text.toUtf8());
          if(strncmp(text.toUtf8(),"alloc(",6) == 0) // allocate big buffer for big text
          {
            int len,ret;
            char *buf, *cptr;
            sscanf(text.toUtf8(),"alloc(%d,",&len);
            //printf("alloc(%d)\n",len);
            buf = new char[len+1];
            ret = tcp_rec_binary(s, buf, len);
            buf[len] = '\0';
            cptr = &buf[0];
            while((cptr = strchr(cptr,27)) != NULL) *cptr = '\n'; // escape
            //text = buf; // this didn't support unicode, fixed in next line
            text = QString::fromUtf8(buf);
            buf[len] = '\0';
            delete [] buf;
          }
          if(i == -6) // ID_COOKIE
          {
            FILE *f;
            char *cptr;
            char cbuf[MAX_PRINTF_LENGTH];
            char buf[MAX_PRINTF_LENGTH];
            strcpy(cbuf,text.toUtf8());
            cptr = strchr(cbuf,'=');
            if(cptr == NULL) // request cookie
            {
              strcat(cbuf,".cookie");
              f = fopen(cbuf,"r");
              if(f == NULL)
              {
                printf("could not read Cookie %s\n", (const char *) text.toUtf8());
                sprintf(buf,"text(-6,\"%s\")\n", "null");
                tcp_send(s,buf,strlen(buf));
              }
              else
              {
                if(fgets(cbuf,sizeof(cbuf)-1,f) == NULL)
                {
                  printf("could not read a line from Cookie %s\n", (const char *) text.toUtf8());
                  sprintf(buf,"text(-6,\"%s\")\n", "null");
                  tcp_send(s,buf,strlen(buf));
                }
                else
                {
                  cptr = strchr(cbuf,'\n');
                  if(cptr != NULL) *cptr = '\0';
                  sprintf(buf,"text(-6,\"%s\")\n", cbuf);
                  tcp_send(s,buf,strlen(buf));
                }
                fclose(f);
              }
            }
            else // set cookie
            {
              *cptr = '\0';
              strcat(cbuf,".cookie");
              if(opt.cookies != 0)
              {
                int doit = 1;
                if(opt.cookies != 1)
                {
                  int ret = QMessageBox::question(0,"pvbrowser","Accept Cookie ?",QMessageBox::No,QMessageBox::Yes);
                  if(ret == QMessageBox::Yes) doit = 1;
                }
                if(doit)
                {
                  f = fopen(cbuf,"w");
                  if(f == NULL)
                  {
                    printf("could not write Cookie %s\n", (const char *) text.toUtf8());
                  }
                  else
                  {
                    fprintf(f,"%s\n", (const char *) text.toUtf8());
                    fclose(f);
                  }
                }
              }
            }
          }
          else if(i == -7) // ID_TAB
          {
            mainWindow->tabBar->setTabText(mainWindow->tabBar->currentIndex(),text);
          }
          else if(i <= 0)
          {
            text = text.left(40);
            mainWindow->setCurrentFile(text);
          }
          else if(all[i]->type == TQPushButton)
          {
            QPushButton *b = (QPushButton*) all[i]->w;
            if(b != NULL) b->setText(text);
          }
          else if(all[i]->type == TQLabel)
          {
            MyLabel *l = (MyLabel*) all[i]->w;
            if(l != NULL) l->setText(text);
          }
          else if(all[i]->type == TQLineEdit)
          {
            MyLineEdit *e = (MyLineEdit*) all[i]->w;
            if(e != NULL) e->setText(text);
          }
          else if(all[i]->type == TQMultiLineEdit)
          {
            MyMultiLineEdit *e = (MyMultiLineEdit*) all[i]->w;
            if(e != NULL) e->setText(text);
          }
          else if(all[i]->type == TQComboBox)
          {
            MyComboBox *c = (MyComboBox*) all[i]->w;
            //if(c != NULL) c->insertItem(text);
            if(c != NULL) c->addItem(text);
          }
          else if(all[i]->type == TQRadio)
          {
            MyRadioButton *c = (MyRadioButton*) all[i]->w;
            if(c != NULL) c->setText(text);
          }
          else if(all[i]->type == TQCheck)
          {
            MyCheckBox *c = (MyCheckBox *) all[i]->w;
            if(c != NULL) c->setText(text);
          }
          else if(all[i]->type == TQGroupBox)
          {
            MyGroupBox *c = (MyGroupBox *) all[i]->w;
            if(c != NULL) c->setTitle(text);
          }
          else if(all[i]->type == TQTextBrowser)
          {
            MyTextBrowser *c = (MyTextBrowser *) all[i]->w;
            // mur was here: make cursor not jumping
            //int x = c->contentsX();
            //int y = c->contentsY();
            if(c != NULL)
            {
              //tb int vPos = c->verticalScrollBar()->value();   // make cursor not jumping (ernst murnleitner)
              //tb int hPos = c->horizontalScrollBar()->value();
              // QTextDocument *doc = c->document();
              // if(doc != NULL) doc->setHtml(text);
              int x = 0;
              int y = 0;
              if(c->page() != NULL && c->page()->mainFrame() != NULL)
              {
                x = c->page()->mainFrame()->scrollBarValue(Qt::Horizontal);
                y = c->page()->mainFrame()->scrollBarValue(Qt::Vertical);
              }
              c->setHTML(text);
              if(c->page() != NULL && c->page()->mainFrame() != NULL)
              {
                c->page()->mainFrame()->setScrollBarValue(Qt::Horizontal,x);
                c->page()->mainFrame()->setScrollBarValue(Qt::Vertical,y);
              }  
              //tb c->verticalScrollBar()->setValue(vPos);
              //tb c->horizontalScrollBar()->setValue(hPos);
            }
          }
          if(opt.arg_debug) printf("setText end\n");
        }
        else if(strncmp(command,"setTableButton(",15) == 0) // set button in table
        {
          sscanf(command,"setTableButton(%d,%d,%d",&i,&x,&y);
          get_text(command,text);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            if(t != NULL) t->setTableButton(x,y,text);
          }
        }
        else if(strncmp(command,"setTableText(",13) == 0) // set text in table
        {
          sscanf(command,"setTableText(%d,%d,%d,",&i,&x,&y);
          get_text(command,text);
          if(i < 0) return;
          if(i >= nmax) return;
          else if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            if(x >= 0 && y >=0)
            {
              int r,g,b;
              if(text.startsWith("color("))
              {
                sscanf(text.toUtf8(),"color(%d,%d,%d",&r,&g,&b);
                text = text.section(')',1);
              }
              else
              {
                r = g = b = 255;
              }
              if(t != NULL)
              {
                QWidget *cw = t->cellWidget(x,y);
                if(cw == NULL)
                {
                  QTableWidgetItem *item = t->item(x,y);
                  if(item != NULL)
                  {
                    t->updates += 1;
                    item->setText(text);
                    // troll porting murx
                    if(t->wrap)
                    {
                      // since Qt4.3 t->setWordWrap() can be done. see below 
                      // but not adjustRow()
                      // also not working  item->setTextAlignment(Qt::AlignJustify);
                      //  item->setWordWrap(true);
                      //  t->adjustRow(x);
                    }
                  }
                  else
                  {
                    t->updates += 2;
                    item = new QTableWidgetItem(text);
                    t->setItem(x,y,item);
                  }
                  if(item != NULL) item->setBackgroundColor(QColor(r,g,b));
                  if(t->autoresize == 1)
                  {
                    t->resizeRowToContents(x);
                  }
                }
                else // cw != NULL
                {
                  if     (cw->inherits("QLabel"))
                  {
                    QLabel *label = (QLabel *) cw;
                    label->setText(text);
                    mySetBackgroundColor(label,TQLabel,r,g,b);
                  }
                  else if(cw->inherits("QPushButton"))
                  {
                    QPushButton *button = (QPushButton *) cw;
                    button->setText(text);
                    mySetBackgroundColor(button,TQPushButton,r,g,b);
                  }
                  else if(cw->inherits("QCheckBox"))
                  {
                    QCheckBox *check = (QCheckBox *) cw;
                    check->setText(text);
                    mySetBackgroundColor(check,TQCheck,r,g,b);
                  }
                }
              }
            }
            else if(x<0 && y >= 0)
            {
              //t->horizontalHeader()->setLabel(y,text);
              QTableWidgetItem *item = t->horizontalHeaderItem(y);
              if(item != NULL) item->setText(text);
              else t->setHorizontalHeaderItem(y,new QTableWidgetItem(text));
            }
            else if(y<0 && x >= 0)
            {
              //t->verticalHeader()->setLabel(x,text);
              QTableWidgetItem *item = t->verticalHeaderItem(x);
              if(item != NULL) item->setText(text);
              else t->setVerticalHeaderItem(x,new QTableWidgetItem(text));
            }
          }
        }
        else if(strncmp(command,"setTableTextAlignment(",22) == 0) // set text alignment in table
        {
          int alignment;
          sscanf(command,"setTableTextAlignment(%d,%d,%d,%d,",&i,&x,&y,&alignment);
          if(i < 0) return;
          if(i >= nmax) return;
          else if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            if(x >= 0 && y >=0)
            {
              QTableWidgetItem *item = t->item(x,y);
              if(item != NULL)
              {
                if     (alignment == PV::AlignLeft)    item->setTextAlignment(Qt::AlignLeft);
                else if(alignment == PV::AlignRight)   item->setTextAlignment(Qt::AlignRight);
                else if(alignment == PV::AlignHCenter) item->setTextAlignment(Qt::AlignHCenter);
                else if(alignment == PV::AlignJustify) item->setTextAlignment(Qt::AlignJustify);
              }
            }
          }
        }
        else if(strncmp(command,"setTablePixmap(",15) == 0) // set pixmap in table
        {
          char *data,text2[80];
          sscanf(command,"setTablePixmap(%d,%d,%d",&i,&x,&y);
          if(i < 0) return;
          if(i >= nmax) return;
          else if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            //QPixmap pm;
            tcp_rec(s,text2,sizeof(text2));
            sscanf(text2,"(%d,%d)",&w,&h);
            if(w==0 && h==0)
            {
              if(t != NULL)
              {
                QPixmap pm;
                QTableWidgetItem *item = t->item(y,x);
                if(item != NULL)
                {
                  item->setIcon(pm);
                }
                else
                {
                  item = new QTableWidgetItem();
                  item->setIcon(pm);
                  t->setItem(y,x,item);
                }
              }
            }
            else
            {
              QImage *temp = readBmpFromSocket(w,h,&data);
              QPixmap pm; //pm(w,h);
              if(temp != NULL)
              {
                myCreateHeuristicMask(pm,temp);
              }
              QTableWidgetItem *item = t->item(y,x);
              if(item != NULL)
              {
                item->setIcon(pm);
              }
              else
              {
                item = new QTableWidgetItem();
                item->setIcon(pm);
                if(t != NULL) t->setItem(y,x,item);
              }
              if(data != NULL) delete [] data;
              if(temp != NULL) delete temp;
            }
          }
          else
          {
            tcp_rec(s,text2,sizeof(text2));
            sscanf(text2,"(%d,%d)",&w,&h);
            if(w==0 && h==0) ;
            else
            {
              QImage *temp = readBmpFromSocket(w,h,&data);
              if(temp != NULL) delete temp;
              if(data != NULL) delete [] data;
            }
          }
        }
        else if(strncmp(command,"setTableCheckBox(",17) == 0) // set checkbox in table
        {
          int state;
          sscanf(command,"setTableCheckBox(%d,%d,%d,%d,",&i,&x,&y,&state);
          get_text(command,text);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            if(t != NULL) t->setTableCheckBox(x,y,state,text);
          }
        }
        else if(strncmp(command,"setTableComboBox(",17) == 0) // set combobox in table
        {
          int editable;
          sscanf(command,"setTableComboBox(%d,%d,%d,%d,",&i,&x,&y,&editable);
          get_text(command,text);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            if(t != NULL) t->setTableComboBox(x,y,editable,text.toUtf8());
          }
        }
        else if(strncmp(command,"setTableLabel(",14) == 0) // set label in table
        {
          sscanf(command,"setTableLabel(%d,%d,%d",&i,&x,&y);
          get_text(command,text);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTable)
          {
            MyTable *t = (MyTable *) all[i]->w;
            if(t != NULL) t->setTableLabel(x,y,text);
          }
        }
        else if(strncmp(command,"setTabPosition(",15) == 0) // set tab position Top|Bottom
        {
          int pos;
          sscanf(command,"setTabPosition(%d,%d",&i,&pos);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTabWidget)
          {
            MyQTabWidget *tab = (MyQTabWidget *) all[i]->w;
            if(tab != NULL)
            {
              if     (pos == 1) tab->setTabPosition(QTabWidget::South);
              else if(pos == 2) tab->setTabPosition(QTabWidget::West);
              else if(pos == 3) tab->setTabPosition(QTabWidget::East);
              else              tab->setTabPosition(QTabWidget::North);
            }
          }
        }
        else if(strncmp(command,"setTime(",8) == 0) // set time
        {
          int hour, minute, second, msec;
          sscanf(command,"setTime(%d,%d,%d,%d,%d)",&i,&hour,&minute,&second,&msec);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQTimeEdit)
          {
            QTimeEdit *e = (QTimeEdit *) all[i]->w;
            if(e != NULL) e->setTime(QTime(hour,minute,second,msec));
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            if(e != NULL) e->setTime(QTime(hour,minute,second,msec));
          }
          else if(all[i]->type == TQwtAnalogClock)
          {
            MyQwtAnalogClock *e = (MyQwtAnalogClock *) all[i]->w;
            if(e != NULL) e->setTime(QTime(hour,minute,second));
          }
        }
        else if(strncmp(command,"setTimeEditDisplay(",19) == 0) // set time edit display
        {
          int hour, minute, second, ampm, mask;
          sscanf(command,"setTimeEditDisplay(%d,%d,%d,%d,%d)",&i,&hour,&minute,&second,&ampm);
          if(i < 0) return;
          if(i >= nmax) return;
          mask = 0;
          if(all[i]->type == TQTimeEdit)
          {
            QTimeEdit *e = (QTimeEdit *) all[i]->w;
            if(hour   == 1) mask |= QDateTimeEdit::HourSection;
            if(minute == 1) mask |= QDateTimeEdit::MinuteSection;
            if(second == 1) mask |= QDateTimeEdit::SecondSection;
            if(ampm   == 1) mask |= QDateTimeEdit::AmPmSection;
            if(e != NULL) e->setCurrentSection((QDateTimeEdit::Section) mask);
          }
          else if(all[i]->type == TQDateTimeEdit)
          {
            QDateTimeEdit *e = (QDateTimeEdit *) all[i]->w;
            if(hour   == 1) mask |= QDateTimeEdit::HourSection;
            if(minute == 1) mask |= QDateTimeEdit::MinuteSection;
            if(second == 1) mask |= QDateTimeEdit::SecondSection;
            if(ampm   == 1) mask |= QDateTimeEdit::AmPmSection;
            if(e != NULL) e->setCurrentSection((QDateTimeEdit::Section) mask);
          }
        }
        break;
      case 'U':
        break;
      case 'V':
        if(strncmp(command,"setValue(",9) == 0) // set value
        {
          int value;
          sscanf(command,"setValue(%d,%d",&i,&value);
          if(i < 0) return;
          if(i >= nmax) return;
          if(all[i]->type == TQSpinBox)
          {
            MySpinBox *b = (MySpinBox*) all[i]->w;
            if(b != NULL) b->setValue(value);
          }
          else if(all[i]->type == TQDial)
          {
            MyDial *b = (MyDial*) all[i]->w;
            if(b != NULL) b->setValue(value);
          }
          else if(all[i]->type == TQProgressBar)
          {
            MyProgressBar *b = (MyProgressBar*) all[i]->w;
            if(b != NULL) b->setValue(value);
          }
          else if(all[i]->type == TQSlider)
          {
            MySlider *b = (MySlider*) all[i]->w;
            if(b != NULL) b->setValue(value);
          }
          else if(all[i]->type == TQTabWidget)
          {
            MyQTabWidget *b = (MyQTabWidget*) all[i]->w;
            if(b != NULL) b->setCurrentIndex(value);
          }
          else if(all[i]->type == TQMultiLineEdit)
          {
            MyMultiLineEdit *b = (MyMultiLineEdit*) all[i]->w;
            if(b != NULL) b->maxlines = value;
          }
          else if(all[i]->type == TQToolBox)
          {
            MyQToolBox *b = (MyQToolBox*) all[i]->w;
            if(b != NULL) b->setCurrentIndex(value);
          }
        }
        break;
      case 'W':
        if(strncmp(command,"setWhatsThis(",13) == 0) // set whatsThis text
        {
          int ind, len;
          char text[MAX_PRINTF_LENGTH];
          sscanf(command,"setWhatsThis(%d,",&i);
          tcp_rec(s,text,sizeof(text)-1);
          len = strlen(text);
          for(ind=0; ind<len; ind++) { if(text[ind] == 27) text[ind] = '\n'; } // substitute Escape
          if(opt.arg_debug) printf("setWhatsThis text[%d]='%s'\n",i,text);
          if(i >= nmax && i > 0) return;
          all[i]->w->setWhatsThis(QString::fromUtf8(text));
        }
        break;
      case 'X':
        break;
      case 'Y':
        break;
      case 'Z':
        if(strncmp(command,"setZoomFactor(",14) == 0) // set zoom factor of HTML page
        {
#if QT_VERSION >= 0x040500
          float factor;
          sscanf(command,"setZoomFactor(%d,%f",&i,&factor);
          if(i >= nmax && i > 0) return;
          MyTextBrowser *b = (MyTextBrowser*) all[i]->w;
          if(b != NULL) b->setZoomFactor(factor);
#endif          
        }
        break;
      default:
        break;
    }
  }
  else if(strncmp(command,"scrollToAnchor(",15) == 0) // TextBrowser / WebKit
  {
    sscanf(command,"scrollToAnchor(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    get_text(command,text);
#if QT_VERSION >= 0x040700    
    if(all[i]->type == TQTextBrowser)
    {
      MyTextBrowser *tb = (MyTextBrowser *) all[i]->w;
      if(tb != NULL) 
      {
        QWebPage *page = tb->page();
        if(page != NULL)
        {
          page->currentFrame()->scrollToAnchor(text);
        }
      }  
    }
#endif    
  }
  else if(strncmp(command,"selection(",10) == 0) // request selection in QListView
  {
    sscanf(command,"selection(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQListView)
    {
      MyListView *lv = (MyListView *) all[i]->w;
      if(lv != NULL) lv->slotSendSelected();
    }
    else if(all[i]->type == TQListBox)
    {
      MyListBox *lb = (MyListBox *) all[i]->w;
      if(lb != NULL) lb->slotSendSelected();
    }
  }
  else if(strncmp(command,"show(",5) == 0) // show the widget
  {
    sscanf(command,"show(%d)",&i);
    if(i < 0)
    {
      if(i == -1) mainWindow->menuBar()->show();
      if(i == -2) mainWindow->fileToolBar->show();
      if(i == -3) mainWindow->statusBar()->show();
      if(i == -4) mainWindow->show();
      if(i == -5)
      {
        if(mainWindow->textbrowser == NULL) mainWindow->textbrowser = new dlgTextBrowser;
        mainWindow->textbrowser->show();
      }  
      int id_dock = i - DOCK_WIDGETS_BASE;
      if( id_dock >= 0 && id_dock < MAX_DOCK_WIDGETS)
      {
        MyQDockWidget *dock = mainWindow->pvbtab[mainWindow->currentTab].dock[id_dock]; 
        if(dock != NULL) dock->show();
      }
      return;
    }
    if(i >= nmax) return;
    // redraw problem reported by ernst murnleitner
    if(all[i]->w != NULL)
    {
#if QT_VERSION < 0x040400
       all[i]->w->setAttribute(Qt::WA_PaintOnScreen);
#endif
       //all[i]->w->setAttribute(Qt::WA_PaintUnclipped);
       all[i]->w->show();
    }
  }
  else if(strncmp(command,"startDefinition(",16) == 0) // startDefinition of new MainWidget
  {
    char *ptr;
    mainLayout = NULL;
    hasLayout = 0;
    if(opt.arg_debug) printf("startDefinition command=%s",command);
    sscanf(command,"startDefinition(%d)",&n);
    if(allBase == NULL)
    {
      if(opt.arg_debug) printf("startDefinition free(all)\n");
      if(all != NULL) free(all);
      mainWidget = new MyQWidget(s,0,NULL);
    }
    if(opt.arg_debug) printf("startDefinition malloc(all)\n");
    ptr = (char *) malloc(n*sizeof(ptr) + n*sizeof(ALL));
    if(ptr==NULL) qFatal("out of memory -> exit");
    all = (ALL **) ptr;
    ptr += n*sizeof(ptr);
    for(i=0; i<n; i++) 
    {
      all[i] = (ALL *) (ptr + i*sizeof(ALL));
      all[i]->x = all[i]->y = all[i]->width = all[i]->height = -1;
    }  
    if(allBase == NULL)
    {
      if(opt.arg_debug) printf("startDefinition all[0]->w = v;\n");
      all[0]->w = mainWidget;
      for(int ii=0; ii<MAX_DOCK_WIDGETS; ii++) 
      {
        if(mainWindow->pvbtab[mainWindow->currentTab].dock[ii] != NULL) 
        {
          delete mainWindow->pvbtab[mainWindow->currentTab].dock[ii];
          mainWindow->pvbtab[mainWindow->currentTab].dock[ii] = NULL;
        }  
      }  
    }
    else
    {
      if(opt.arg_debug) printf("startDefinition all[0]->w = modalDialog;\n");
      all[0]->w = modalDialog;
    }

    nmax = n;
    if(opt.arg_debug) printf("startDefinition 2\n");
    for(i=1; i<n; i++)
    {
      all[i]->w = (QWidget *) NULL;
      all[i]->type = -1;
    }
    if(allBase != NULL)
    {
      if(opt.arg_debug) printf("startDefinition allBase != NULL\n");
      allModal = all;
      nmaxModal = nmax;
    }
    if(opt.arg_debug) printf("startDefinition end\n");
  }
  else if(strncmp(command,"save(",5) == 0) // save table as TAB separated text file
  {
    sscanf(command,"save(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      MyTable *table = (MyTable *) all[i]->w;
      if(table != NULL) table->saveTextfile();
    }
  }
  else if(strncmp(command,"screenHint(",11) == 0) // adjust zoom factor
  {
    int w,h,dw;
    sscanf(command,"screenHint(%d,%d)",&w,&h);
    dw = QApplication::desktop()->width();
    // dh = QApplication::desktop()->height();
    // dw = (zoom / 100) w;
    // ( dw / w ) * 100 = zoom;
    if(w>0 && dw>0) zoom = (dw *100) / w;
  }
  else if(strncmp(command,"statusMessage(",14) == 0) // output status message
  {
    int r,g,b;
    sscanf(command,"statusMessage(%d,%d,%d",&r,&g,&b);
    get_text(command,text);
    mySetBackgroundColor(mainWindow->statusBar(),-1,r,g,b);
    mainWindow->statusBar()->showMessage(text);
  }
}

void Interpreter::interprett(const char *command)
{
  if(strncmp(command,"text(",5) == 0)
  {
    QString quote;
    char buf[MAX_PRINTF_LENGTH];
    sscanf(command,"text(%d)",&i);
    buf[0] = '\0';
    if(i < 0)
    {
      if(i == -8) // ID_OPTIONS
      {
        sprintf(buf,"text(-8,\"-zoom=%d -fontzoom=%d -menubar=%d -toolbar=%d -statusbar=%d -scrollbars=%d -fullscreen=%d -maximized=%d -tabs_above_toolbar=%d -cookies=%d -echo_table_updates=%d -enable_webit_plugins=%d -murn=%d -language=%s\")\n",
        opt.zoom,
        opt.fontzoom,
        opt.menubar,
        opt.toolbar,
        opt.statusbar,
        opt.scrollbars,
        opt.fullscreen,
        opt.maximized,
        opt.tabs_above_toolbar,
        opt.cookies,
        opt.echo_table_updates,
        opt.enable_webkit_plugins,
        opt.arg_fillbackground,
        opt.language
        );
        tcp_send(s,buf,strlen(buf));
      }
      return;
    }  
    if(i >= nmax) return;
    if(all[i]->type == TQLabel)
    {
      MyLabel *w = (MyLabel *) all[i]->w;
      if(w != NULL)
      {
        quote = w->text();
        quote.replace("\n","\\n");
        sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) quote.toUtf8());
        if(buf[0] != '\0') tcp_send(s,buf,strlen(buf));
      }
    }
    else if(all[i]->type == TQPushButton)
    {
      MyQPushButton *w = (MyQPushButton *) all[i]->w;
      if(w != NULL)
      {
        quote = w->text();
        quote.replace("\n","\\n");
        sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) quote.toUtf8());
        if(buf[0] != '\0') tcp_send(s,buf,strlen(buf));
      }
    }
    else if(all[i]->type == TQRadio)
    {
      MyRadioButton *w = (MyRadioButton *) all[i]->w;
      if(w != NULL)
      {
        quote = w->text();
        quote.replace("\n","\\n");
        sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) quote.toUtf8());
        if(buf[0] != '\0') tcp_send(s,buf,strlen(buf));
      }
    }
    else if(all[i]->type == TQCheck)
    {
      MyCheckBox *w = (MyCheckBox *) all[i]->w;
      if(w != NULL)
      {
        quote = w->text();
        quote.replace("\n","\\n");
        sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) quote.toUtf8());
        if(buf[0] != '\0') tcp_send(s,buf,strlen(buf));
      }
    }
    else if(all[i]->type == TQLineEdit)
    {
      MyLineEdit *w = (MyLineEdit *) all[i]->w;
      if(w != NULL)
      {
        quote = w->text();
        quote.replace("\n","\\n");
        sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) quote.toUtf8());
        if(buf[0] != '\0') tcp_send(s,buf,strlen(buf));
      }
    }
    else if(all[i]->type == TQComboBox)
    {
      MyComboBox *w = (MyComboBox *) all[i]->w;
      if(w != NULL)
      {
        quote = w->currentText();
        quote.replace("\n","\\n");
        sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) quote.toUtf8());
        //sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) w->currentText());
        if(buf[0] != '\0') tcp_send(s,buf,strlen(buf));
      }
    }
    else if(all[i]->type == TQGroupBox)
    {
      MyGroupBox *w = (MyGroupBox *) all[i]->w;
      if(w != NULL)
      {
        quote = w->title();
        quote.replace("\n","\\n");
        sprintf(buf,"text(%d,\"%s\")\n", i, (const char *) quote.toUtf8());
        if(buf[0] != '\0') tcp_send(s,buf,strlen(buf));
      }
    }
    else if(all[i]->type == TQMultiLineEdit)
    {
      MyMultiLineEdit *w = (MyMultiLineEdit *) all[i]->w;
      if(w != NULL)w->slotSendToClipboard();
    }
  }
  else if(strncmp(command,"tableSetEnabled(",16) == 0)
  {
    int x,y,enabled;
    sscanf(command,"tableSetEnabled(%d,%d,%d,%d)",&i,&x,&y,&enabled);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      QTableWidget *t;
      QTableWidgetItem *item;
      t = (QTableWidget *) all[i]->w;
      if(t != NULL)
      {
        item = t->item(y,x);
        QWidget *widget = t->cellWidget(y,x);
        if(widget != NULL)
        {
          widget->setEnabled(enabled);
        }
        else if(item != 0)
        {
          if(enabled) item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
          else        item->setFlags(0);
        }
      }
    }
  }
  else if(strncmp(command,"tableSetHeaderResizeEnabled(",28) == 0)
  {
    int horizontal,enabled,section;
    sscanf(command,"tableSetHeaderResizeEnabled(%d,%d,%d,%d)",&i,&horizontal,&enabled,&section);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      QTableWidget *t;
      t = (QTableWidget *) all[i]->w;
      if(t != NULL)
      {
        if(horizontal == 1)
        {
          //t->horizontalHeader()->setResizeEnabled(enabled,section);
          if(enabled) t->horizontalHeader()->setResizeMode(section,QHeaderView::Interactive);
          else        t->horizontalHeader()->setResizeMode(section,QHeaderView::Custom);
        }
        else
        {
          //t->verticalHeader()->setResizeEnabled(enabled,section);
          if(enabled) t->verticalHeader()->setResizeMode(section,QHeaderView::Interactive);
          else        t->verticalHeader()->setResizeMode(section,QHeaderView::Custom);
        }
      }
    }
  }
  else if(strncmp(command,"tableWrap(",10) == 0)
  {
    int wrap;
    sscanf(command,"tableWrap(%d,%d",&i,&wrap);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQTable)
    {
      MyTable *t = (MyTable *) all[i]->w;
      if(t != NULL) 
      {
        t->wrap = wrap;
#if QT_VERSION >= 0x040300
        if(wrap)
        {
           t->setWordWrap(true);
        }
        else
        {
           t->setWordWrap(false);
        }
#endif
      }
    }
  }
  else if(strncmp(command,"tabOrder(",9) == 0)
  {
    int id1,id2;
    sscanf(command,"tabOrder(%d,%d",&id1,&id2);
    if(id1 >= 0 && id2 >= 0)
    {
      if(id1 < nmax && id2 < nmax && all[id1]->w != NULL && all[id2]->w != NULL)
      {
        QWidget::setTabOrder(all[id1]->w, all[id2]->w);
      }
    }
  }
}

void Interpreter::interpretv(const char *command)
{
  if(command == NULL) return;
#ifdef USE_VTK
  if(strncmp(command,"vtkTcl(",7) == 0) // interpret a VTK Tcl command
  {
    char text[MAX_PRINTF_LENGTH];
    sscanf(command,"vtkTcl(%d)",&i);
    tcp_rec(s,text,sizeof(text));
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQVtk)
    {
      pvVtkTclWidget *w = (pvVtkTclWidget *) all[i]->w;
      if(w != NULL)
      {
        w->interpret(text);
        if(strstr(text,"renderer") != NULL && strstr(text,"Render") != NULL ) w->updateGL();
      }
    }
  }
  else if(strncmp(command,"vtkTclScript(",13) == 0) // interpret a VTK Tcl file
  {
    sscanf(command,"vtkTclScript(%d,",&i);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(all[i]->type == TQVtk)
    {
      pvVtkTclWidget *w = (pvVtkTclWidget *) all[i]->w;
      if(w != NULL) w->interpretFile(text.toUtf8());
    }
  }
#endif
  if(strncmp(command,"view.audio", 10) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_audio);
      strcat(buf, " ");
      strcat(buf, &command[10]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_audio) >= 3) mysystem(buf);
    }  
  }
  else if(strncmp(command,"view.video", 10) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_video);
      strcat(buf, " ");
      strcat(buf, &command[10]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_video) >= 3) mysystem(buf);
    }  
  }
  else if(strncmp(command,"view.pdf", 8) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_pdf);
      strcat(buf, " ");
      strcat(buf, &command[8]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_pdf) >= 3) mysystem(buf);
    }  
  }
  else if(strncmp(command,"view.img", 8) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_img);
      strcat(buf, " ");
      strcat(buf, &command[8]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_img) >= 3) mysystem(buf);
    }  
  }
  else if(strncmp(command,"view.svg", 8) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_svg);
      strcat(buf, " ");
      strcat(buf, &command[8]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_svg) >= 3) mysystem(buf);
    }  
  }
  else if(strncmp(command,"view.txt", 8) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_txt);
      strcat(buf, " ");
      strcat(buf, &command[8]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_txt) >= 3) mysystem(buf);
    }  
  }
  else if(strncmp(command,"view.csv", 8) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_csv);
      strcat(buf, " ");
      strcat(buf, &command[8]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_csv) >= 3) mysystem(buf);
    }  
  }
  else if(strncmp(command,"view.html", 9) == 0)
  {
    char *cptr, buf[1024];
    if(strlen(command) <= (sizeof(buf) - 128))
    {
      strcpy(buf, opt.view_html);
      strcat(buf, " ");
      strcat(buf, &command[9]);
      cptr = strstr(buf,"\n");
      if(cptr != NULL) *cptr = '\0';
#ifndef PVWIN32
      strcat(buf, " &");
#endif
      if(strlen(opt.view_html) >= 3) mysystem(buf);
    }  
  }
}

void Interpreter::interpretq(const char *command)
{
  if(command == NULL) return; // damn compiler warnings
#ifndef NO_QWT
  if(strncmp(command,"qpw(",4) == 0)
  {
    int ret;
    char qwtcommand[1024];
    sscanf(command,"qpw(%d)",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    QwtPlotWidget *pw = (QwtPlotWidget *) all[i]->w;
    ret = tcp_rec(s, qwtcommand, sizeof(qwtcommand)-1);
    if(ret == -1) return;
    if(all[i]->type != TQwtPlotWidget) return;
    if(opt.arg_debug) printf("qwtplot=%s",qwtcommand);
    if(strncmp(qwtcommand,"setCurveData(",13) == 0)
    {
      int cnt,c,count;
      char param[256];
      sscanf(qwtcommand,"setCurveData(%d,%d",&c,&count);
      if(count <= 0) return;
      double *x = new double[count];
      double *y = new double[count];
      for(cnt=0; cnt<count; cnt++)
      {
        ret = tcp_rec(s, param, sizeof(param)-1);
        if(ret == -1)
        {
          delete [] x;
          delete [] y;
          return;
        }
        sscanf(param,"%lf %lf",&x[cnt],&y[cnt]);
      }
      pw->interpret(qwtcommand,x,y);
      delete [] x;
      delete [] y;
    }
    else
    {
      pw->interpret(qwtcommand);
    }
  }
  else if(strncmp(command,"qwt(",4) == 0) // Commands for the QwtWidgets
  {
    int ret;
    char qwtcommand[1024];
    sscanf(command,"qwt(%d)",&i);
    ret = tcp_rec(s, qwtcommand, sizeof(qwtcommand)-1);
    if(ret == -1) return;
    if(i < 0) return;
    if(i >= nmax) return;
    if(opt.arg_debug) printf("qwtwidget=%s",qwtcommand);
    if(all[i]->type == TQwtScale)
    {
      // QwtScale
      QwtScaleWidget *wi = (QwtScaleWidget *) all[i]->w;
      if(strncmp(qwtcommand,"setTitle(",9) == 0)
      {
        get_text(qwtcommand,text);
        if(wi != NULL) wi->setTitle(text);
      }
      else if(strncmp(qwtcommand,"setTitleColor(",14) == 0)
      {
        int r,g,b;
        sscanf(qwtcommand,"setTitleColor(%d,%d,%d",&r,&g,&b);
        if(wi != NULL) wi->title().setColor(QColor(r,g,b));
      }
      else if(strncmp(qwtcommand,"setTitleFont(",13) == 0)
      {
        int pointsize,bold,italic,underline,strikeout;
        sscanf(qwtcommand,"setTitleFont(%d,%d,%d,%d,%d",&pointsize,&bold,&italic,&underline,&strikeout);
        get_text(qwtcommand,text); // text = family of font
        if(zoom != 100 || opt.fontzoom != 100)
        {
          pointsize = (pointsize*zoom*opt.fontzoom)/(100*100);
        }
        QFont font(text);
        font.setPointSize(pointsize);
        if(bold      == 1) font.setBold(true);
        if(italic    == 1) font.setItalic(true);
        if(underline == 1) font.setUnderline(true);
        if(strikeout == 1) font.setStrikeOut(true);
        if(wi != NULL) wi->title().setFont(font);
      }
      else if(strncmp(qwtcommand,"setTitleAlignment(",18) == 0)
      {
        int flags;
        sscanf(qwtcommand,"setTitleAlignment(%d",&flags);
        //murx wi->setTitleAlignment(flags);
      }
      else if(strncmp(qwtcommand,"setBorderDist(",14) == 0)
      {
        int start,end;
        sscanf(qwtcommand,"setBorderDist(%d,%d",&start,&end);
        if(wi != NULL) wi->setBorderDist(start,end);
      }
      else if(strncmp(qwtcommand,"setBaselineDist(",16) == 0)
      {
        //qwtmurx method was removed by qwt
        //int bd;
        //sscanf(qwtcommand,"setBaselineDist(%d",&bd);
        //if(wi != NULL) wi->setBaselineDist(bd);
      }
      else if(strncmp(qwtcommand,"setScaleDiv(",12) == 0)
      {
        double lBound, hBound, step;
        int maxMaj, maxMin, log, ascend;
        QwtScaleDiv sd;
        sscanf(qwtcommand,"setScaleDiv(%lf,%lf,%d,%d,%d,%lf,%d",
                           &lBound,&hBound,&maxMaj,&maxMin,&log,&step,&ascend);
        //murx sd.rebuild(lBound,hBound,maxMaj,maxMin,log,step,ascend);
        //murx wi->setScaleDiv(sd);
      }
      else if(strncmp(qwtcommand,"setScaleDraw(",13) == 0)
      {
        //setScaleDraw(QwtScaleDraw *);
      }
      else if(strncmp(qwtcommand,"setLabelFormat(",15) == 0)
      {
        int f,prec,fieldWidth;
        sscanf(qwtcommand,"setLabelFormat(%d,%d,%d",&f,&prec,&fieldWidth);
        //murx wi->setLabelFormat(f,prec,fieldWidth);
      }
      else if(strncmp(qwtcommand,"setLabelAlignment(",18) == 0)
      {
        int align;
        sscanf(qwtcommand,"setLabelAlignment(%d",&align);
        //murx wi->setLabelAlignment(align);
        if(wi != NULL) wi->setLabelAlignment((Qt::Alignment) align);
      }
      else if(strncmp(qwtcommand,"setLabelRotation(",17) == 0)
      {
        double rotation;
        sscanf(qwtcommand,"setLabelRotation(%lf",&rotation);
        if(wi != NULL) wi->setLabelRotation(rotation);
      }
      else if(strncmp(qwtcommand,"setPosition(",12) == 0)
      {
        int pos;
        sscanf(qwtcommand,"setPosition(%d",&pos);
        //murx wi->setPosition((QwtScale::Position) pos);
        if(pos == PV::Horizontal && wi != NULL)
          wi->setAlignment(QwtScaleDraw::BottomScale);
        else if(wi != NULL)
          wi->setAlignment(QwtScaleDraw::LeftScale);
      }
    }
    else if(all[i]->type == TQwtThermo)
    {
      // QwtThermo
      QwtThermo *wi = (QwtThermo *) all[i]->w;
      if(strncmp(qwtcommand,"setScale(",9) == 0)
      {
        double min,max,step;
        int logarithmic;
        sscanf(qwtcommand,"setScale(%lf,%lf,%lf,%d",&min,&max,&step,&logarithmic);
        //murx wi->setScale(min,max,step,logarithmic);
        if(wi != NULL) wi->setScale(min,max,step);
      }
      else if(strncmp(qwtcommand,"setOrientation(",15) == 0)
      {
        int ori,pos;
        sscanf(qwtcommand,"setOrientation(%d,%d",&ori,&pos);
        if(ori == PV::Horizontal) ori = Qt::Horizontal;
        else                      ori = Qt::Vertical;
        //qwtmurx
        if     (pos == PV::ThermoNone)  pos = QwtThermo::NoScale;
        else if(pos == PV::ThermoTop)   pos = QwtThermo::TopScale;
        else if(pos == PV::ThermoBottom)pos = QwtThermo::BottomScale;
        else if(pos == PV::ThermoLeft)  pos = QwtThermo::LeftScale;
        else if(pos == PV::ThermoRight) pos = QwtThermo::RightScale;
        //if     (pos == PV::ThermoNone)  pos = QwtThermo::None;
        //else if(pos == PV::ThermoTop)   pos = QwtThermo::Top;
        //else if(pos == PV::ThermoBottom)pos = QwtThermo::Bottom;
        //else if(pos == PV::ThermoLeft)  pos = QwtThermo::Left;
        //else if(pos == PV::ThermoRight) pos = QwtThermo::Right;
        if(wi != NULL) wi->setOrientation((Qt::Orientation) ori, (QwtThermo::ScalePos) pos);
      }
      else if(strncmp(qwtcommand,"setBorderWidth(",15) == 0)
      {
        int w;
        sscanf(qwtcommand,"setBorderWidth(%d",&w);
        if(wi != NULL) wi->setBorderWidth(w);
      }
      else if(strncmp(qwtcommand,"setFillColor(",13) == 0)
      {
        int r,g,b;
        sscanf(qwtcommand,"setFillColor(%d,%d,%d",&r,&g,&b);
        if(wi != NULL) wi->setFillColor(QColor(r,g,b));
      }
      else if(strncmp(qwtcommand,"setAlarmColor(",14) == 0)
      {
        int r,g,b;
        sscanf(qwtcommand,"setAlarmColor(%d,%d,%d",&r,&g,&b);
        if(wi != NULL) wi->setAlarmColor(QColor(r,g,b));
      }
      else if(strncmp(qwtcommand,"setAlarmLevel(",14) == 0)
      {
        double v;
        sscanf(qwtcommand,"setAlarmLevel(%lf",&v);
        if(wi != NULL) wi->setAlarmLevel(v);
      }
      else if(strncmp(qwtcommand,"setAlarmEnabled(",16) == 0)
      {
        int tf;
        sscanf(qwtcommand,"setAlarmEnabled(%d",&tf);
        if(wi != NULL) wi->setAlarmEnabled(tf);
      }
      else if(strncmp(qwtcommand,"setPipeWidth(",13) == 0)
      {
        int w;
        sscanf(qwtcommand,"setPipeWidth(%d",&w);
        if(wi != NULL) wi->setPipeWidth(w);
      }
      else if(strncmp(qwtcommand,"setRange(",9) == 0)
      {
        double vmin,vmax;
        float step = 0.0;
        sscanf(qwtcommand,"setRange(%lf,%lf,%f",&vmin,&vmax,&step);
        if(wi != NULL) wi->setRange(vmin,vmax,step);
      }
      else if(strncmp(qwtcommand,"setMargin(",10) == 0)
      {
        int m;
        sscanf(qwtcommand,"setMargin(%d",&m);
        if(wi != NULL) wi->setMargin(m);
      }
      else if(strncmp(qwtcommand,"setValue(",9) == 0)
      {
        double val;
        sscanf(qwtcommand,"setValue(%lf",&val);
        if(wi != NULL) wi->setValue(val);
      }
    }
    else if(all[i]->type == TQwtKnob)
    {
      // QwtKnob
      QwtKnob *wi = (QwtKnob *) all[i]->w;
      if(strncmp(qwtcommand,"setScale(",9) == 0)
      {
        double min,max,step;
        int logarithmic;
        sscanf(qwtcommand,"setScale(%lf,%lf,%lf,%d",&min,&max,&step,&logarithmic);
        //murx wi->setScale(min,max,step,logarithmic);
        if(wi != NULL) 
        {
          wi->setScale(min,max,step);
          wi->setRange(min,max);
        }  
      }
      else if(strncmp(qwtcommand,"setMass(",8) == 0)
      {
        float mass;
        sscanf(qwtcommand,"setMass(%f",&mass);
        if(wi != NULL) wi->setMass(w);
      }
      else if(strncmp(qwtcommand,"setOrientation(",15) == 0)
      {
        int o;
        sscanf(qwtcommand,"setOrientation(%d",&o);
        if(o == PV::Horizontal) o = Qt::Horizontal;
        else                    o = Qt::Vertical;
        if(wi != NULL) wi->setOrientation((Qt::Orientation) o);
      }
      else if(strncmp(qwtcommand,"setReadOnly(",12) == 0)
      {
        int w;
        sscanf(qwtcommand,"setReadOnly(%d",&w);
        if(wi != NULL) wi->setReadOnly(w);
      }
      else if(strncmp(qwtcommand,"setKnobWidth(",13) == 0)
      {
        int w;
        sscanf(qwtcommand,"setKnobWidth(%d",&w);
        if(wi != NULL) wi->setKnobWidth(w);
      }
      else if(strncmp(qwtcommand,"setTotalAngle(",14) == 0)
      {
        double angle;
        sscanf(qwtcommand,"setTotalAngle(%lf",&angle);
        if(wi != NULL) wi->setTotalAngle(angle);
      }
      else if(strncmp(qwtcommand,"setBorderWidth(",15) == 0)
      {
        int bw;
        sscanf(qwtcommand,"setBorderWidth(%d",&bw);
        if(wi != NULL) wi->setBorderWidth(bw);
      }
      else if(strncmp(qwtcommand,"setSymbol(",10) == 0)
      {
        int s;
        sscanf(qwtcommand,"setSymbol(%d",&s);
        if(wi != NULL) wi->setSymbol((QwtKnob::Symbol) s);
      }
      else if(strncmp(qwtcommand,"setValue(",9) == 0)
      {
        double val;
        sscanf(qwtcommand,"setValue(%lf",&val);
        if(wi != NULL) wi->setValue(val);
      }
    }
    else if(all[i]->type == TQwtCounter)
    {
      // QwtCounter
      QwtCounter *wi = (QwtCounter *) all[i]->w;
      if(strncmp(qwtcommand,"setStep(",8) == 0)
      {
        double s;
        sscanf(qwtcommand,"setStep(%lf",&s);
        if(wi != NULL) wi->setStep(s);
      }
      else if(strncmp(qwtcommand,"setMinValue(",12) == 0)
      {
        double m;
        sscanf(qwtcommand,"setMinValue(%lf",&m);
        if(wi != NULL) wi->setMinValue(m);
      }
      else if(strncmp(qwtcommand,"setMaxValue(",12) == 0)
      {
        double m;
        sscanf(qwtcommand,"setMaxValue(%lf",&m);
        if(wi != NULL) wi->setMaxValue(m);
      }
      else if(strncmp(qwtcommand,"setStepButton1(",15) == 0)
      {
        int n;
        sscanf(qwtcommand,"setStepButton1(%d",&n);
        if(wi != NULL) wi->setStepButton1(n);
      }
      else if(strncmp(qwtcommand,"setStepButton2(",15) == 0)
      {
        int n;
        sscanf(qwtcommand,"setStepButton2(%d",&n);
        if(wi != NULL) wi->setStepButton2(n);
      }
      else if(strncmp(qwtcommand,"setStepButton3(",15) == 0)
      {
        int n;
        sscanf(qwtcommand,"setStepButton3(%d",&n);
        if(wi != NULL) wi->setStepButton3(n);
      }
      else if(strncmp(qwtcommand,"setNumButtons(",14) == 0)
      {
        int n;
        sscanf(qwtcommand,"setNumButtons(%d",&n);
        if(wi != NULL) wi->setNumButtons(n);
      }
      else if(strncmp(qwtcommand,"setIncSteps(",12) == 0)
      {
        int btn,n;
        sscanf(qwtcommand,"setIncSteps(%d,%d",&btn,&n);
        if(wi != NULL) wi->setIncSteps((QwtCounter::Button) btn, n);
      }
      else if(strncmp(qwtcommand,"setValue(",9) == 0)
      {
        double val;
        sscanf(qwtcommand,"setValue(%lf",&val);
        if(wi != NULL) wi->setValue(val);
      }
    }
    else if(all[i]->type == TQwtWheel)
    {
      // QwtWheel
      QwtWheel *wi = (QwtWheel *) all[i]->w;
      if(strncmp(qwtcommand,"setMass(",8) == 0)
      {
        float mass;
        sscanf(qwtcommand,"setMass(%f",&mass);
        if(wi != NULL) wi->setMass(w);
      }
      else if(strncmp(qwtcommand,"setOrientation(",15) == 0)
      {
        int o;
        sscanf(qwtcommand,"setOrientation(%d",&o);
        if(o == PV::Horizontal && wi != NULL)
          wi->setOrientation(Qt::Horizontal);
        else if(wi != NULL)
          wi->setOrientation(Qt::Vertical);
        //wi->setOrientation((Qt::Orientation) o);
      }
      else if(strncmp(qwtcommand,"setReadOnly(",12) == 0)
      {
        int w;
        sscanf(qwtcommand,"setReadOnly(%d",&w);
        if(wi != NULL) wi->setReadOnly(w);
      }
      else if(strncmp(qwtcommand,"setTotalAngle(",14) == 0)
      {
        double angle;
        sscanf(qwtcommand,"setTotalAngle(%lf",&angle);
        if(wi != NULL) wi->setTotalAngle(angle);
      }
      else if(strncmp(qwtcommand,"setTickCnt(",11) == 0)
      {
        int cnt;
        sscanf(qwtcommand,"setTickCnt(%d",&cnt);
        if(wi != NULL) wi->setTickCnt(cnt);
      }
      else if(strncmp(qwtcommand,"setViewAngle(",13) == 0)
      {
        double angle;
        sscanf(qwtcommand,"setViewAngle(%lf",&angle);
        if(wi != NULL) wi->setViewAngle(angle);
      }
      else if(strncmp(qwtcommand,"setInternalBorder(",18) == 0)
      {
        int width;
        sscanf(qwtcommand,"setInternalBorder(%d",&width);
        if(wi != NULL) wi->setInternalBorder(width);
      }
      else if(strncmp(qwtcommand,"setMass(",8) == 0)
      {
        double val;
        sscanf(qwtcommand,"setMass(%lf",&val);
        if(wi != NULL) wi->setMass(val);
      }
      else if(strncmp(qwtcommand,"setWheelWidth(",14) == 0)
      {
        int width;
        sscanf(qwtcommand,"setWheelWidth(%d",&width);
        if(wi != NULL) wi->setWheelWidth(width);
      }
      else if(strncmp(qwtcommand,"setValue(",9) == 0)
      {
        double val;
        sscanf(qwtcommand,"setValue(%lf",&val);
        if(wi != NULL) wi->setValue(val);
      }
    }
    else if(all[i]->type == TQwtSlider)
    {
      // QwtSlider
      QwtSlider *wi = (QwtSlider *) all[i]->w;
      if(strncmp(qwtcommand,"setScale(",9) == 0)
      {
        double min,max,step;
        int logarithmic;
        sscanf(qwtcommand,"setScale(%lf,%lf,%lf,%d",&min,&max,&step,&logarithmic);
        //murx wi->setScale(min,max,step,logarithmic);
        if(wi != NULL) wi->setScale(min,max,step);
      }
      else if(strncmp(qwtcommand,"setMass(",8) == 0)
      {
        float mass;
        sscanf(qwtcommand,"setMass(%f",&mass);
        if(wi != NULL) wi->setMass(w);
      }
      else if(strncmp(qwtcommand,"setOrientation(",15) == 0)
      {
        int o;
        sscanf(qwtcommand,"setOrientation(%d",&o);
        if(o == PV::Horizontal && wi != NULL)
          wi->setOrientation(Qt::Horizontal);
        else if(wi != NULL)
          wi->setOrientation(Qt::Vertical);
      }
      else if(strncmp(qwtcommand,"setReadOnly(",12) == 0)
      {
        int w;
        sscanf(qwtcommand,"setReadOnly(%d",&w);
        if(wi != NULL) wi->setReadOnly(w);
      }
      else if(strncmp(qwtcommand,"setBgStyle(",11) == 0)
      {
        int st;
        sscanf(qwtcommand,"setBgStyle(%d",&st);
        if(wi != NULL) wi->setBgStyle((QwtSlider::BGSTYLE) st);
      }
      else if(strncmp(qwtcommand,"setScalePos(",12) == 0)
      {
        int st;
        sscanf(qwtcommand,"setScalePos(%d",&st);
        switch(st)
        {
          //qwtmurx
          case PV::SliderLeft:
            if(wi != NULL) wi->setScalePosition(QwtSlider::LeftScale);
            break;
          case PV::SliderRight:
            if(wi != NULL) wi->setScalePosition(QwtSlider::RightScale);
            break;
          case PV::SliderTop:
            if(wi != NULL) wi->setScalePosition(QwtSlider::TopScale);
            break;
          case PV::SliderBottom:
            if(wi != NULL) wi->setScalePosition(QwtSlider::BottomScale);
            break;
          default:
            if(wi != NULL) wi->setScalePosition(QwtSlider::NoScale);
            break;
          //case PV::SliderLeft:
          //  if(wi != NULL) wi->setScalePosition(QwtSlider::Left);
          //  break;
          //case PV::SliderRight:
          //  if(wi != NULL) wi->setScalePosition(QwtSlider::Right);
          //  break;
          //case PV::SliderTop:
          //  if(wi != NULL) wi->setScalePosition(QwtSlider::Top);
          //  break;
          //case PV::SliderBottom:
          //  if(wi != NULL) wi->setScalePosition(QwtSlider::Bottom);
          //  break;
          //default:
          //  if(wi != NULL) wi->setScalePosition(QwtSlider::None);
          //  break;
        }
      }
      else if(strncmp(qwtcommand,"setThumbLength(",15) == 0)
      {
        int l;
        sscanf(qwtcommand,"setThumbLength(%d",&l);
        if(wi != NULL) wi->setThumbLength(l);
      }
      else if(strncmp(qwtcommand,"setThumbWidth(",14) == 0)
      {
        int w;
        sscanf(qwtcommand,"setThumbWidth(%d",&w);
        if(wi != NULL) wi->setThumbWidth(w);
      }
      else if(strncmp(qwtcommand,"setBorderWidth(",15) == 0)
      {
        int bw;
        sscanf(qwtcommand,"setBorderWidth(%d",&bw);
        if(wi != NULL) wi->setBorderWidth(bw);
      }
      else if(strncmp(qwtcommand,"setMargins(",11) == 0)
      {
        int x,y;
        sscanf(qwtcommand,"setMargins(%d,%d",&x,&y);
        if(wi != NULL) wi->setMargins(x,y);
      }
      else if(strncmp(qwtcommand,"setValue(",9) == 0)
      {
        double val;
        sscanf(qwtcommand,"setValue(%lf",&val);
        if(wi != NULL) wi->setValue(val);
      }
    }
    else if(all[i]->type == TQwtDial        || 
            all[i]->type == TQwtCompass     || 
            all[i]->type == TQwtAnalogClock )
    {
      // QwtCompass
      // QwtCompass *wi = (QwtCompass *) all[i]->w;
      QwtDial *wi = (QwtDial *) all[i]->w;
      if(strncmp(qwtcommand,"setRange(",9) == 0)
      {
        double vmin,vmax;
        float step = 0.0;
        sscanf(qwtcommand,"setRange(%lf,%lf,%f",&vmin,&vmax,&step);
        if(wi != NULL) wi->setRange(vmin,vmax,step);
      }
      else if(strncmp(qwtcommand,"setMass(",8) == 0)
      {
        float mass;
        sscanf(qwtcommand,"setMass(%f",&mass);
        if(wi != NULL) wi->setMass(w);
      }
      else if(strncmp(qwtcommand,"setOrientation(",15) == 0)
      {
        int o;
        sscanf(qwtcommand,"setOrientation(%d",&o);
        if(o == PV::Horizontal) o = Qt::Horizontal;
        else                    o = Qt::Vertical;
        if(wi != NULL) wi->setOrientation((Qt::Orientation) o);
      }
      else if(strncmp(qwtcommand,"setReadOnly(",12) == 0)
      {
        int w;
        sscanf(qwtcommand,"setReadOnly(%d",&w);
        if(wi != NULL) wi->setReadOnly(w);
      }
      else if(strncmp(qwtcommand,"setSimpleCompassRose(",21) == 0 &&
              all[i]->type == TQwtCompass                         )
      {
        int numThorns,numThornLevels;
        double width;
        sscanf(qwtcommand,"setSimpleCompassRose(%d,%d,%lf",&numThorns,&numThornLevels,&width);
        QwtSimpleCompassRose *rose = new QwtSimpleCompassRose(numThorns, numThornLevels);
        rose->setWidth(width);
        if(wi != NULL) ((QwtCompass *) wi)->setRose(rose);
      }
      else if(strncmp(qwtcommand,"setLabelMap(",12) == 0)
      {
        //setLabelMap(const QMap<double, QString> &map);
      }
      else if(strncmp(qwtcommand,"setFrameShadow(",15) == 0)
      {
        int s;
        sscanf(qwtcommand,"setFrameShadow(%d",&s);
        if(wi != NULL) wi->setFrameShadow((QwtDial::Shadow) s);
      }
      else if(strncmp(qwtcommand,"showBackground(",15) == 0)
      {
        int b;
        sscanf(qwtcommand,"showBackground(%d",&b);
        if(wi != NULL) wi->showBackground(b);
      }
      else if(strncmp(qwtcommand,"setLineWidth(",13) == 0)
      {
        int w;
        sscanf(qwtcommand,"setLineWidth(%d",&w);
        if(wi != NULL) wi->setLineWidth(w);
      }
      else if(strncmp(qwtcommand,"setMode(",8) == 0)
      {
        int m;
        sscanf(qwtcommand,"setMode(%d",&m);
        if(wi != NULL) wi->setMode((QwtDial::Mode) m);
      }
      else if(strncmp(qwtcommand,"setWrapping(",12) == 0)
      {
        int w;
        sscanf(qwtcommand,"setWrapping(%d",&w);
        if(wi != NULL) wi->setWrapping(w);
      }
      else if(strncmp(qwtcommand,"setScale(",9) == 0)
      {
        int maxMajIntv,maxMinIntv;
        double step;
        sscanf(qwtcommand,"setScale(%d,%d,%lf",&maxMajIntv,&maxMinIntv,&step);
        if(wi != NULL) wi->setScale(maxMajIntv,maxMinIntv,step);
      }
      else if(strncmp(qwtcommand,"setScaleArc(",12) == 0)
      {
        double min,max;
        sscanf(qwtcommand,"setScaleArc(%lf,%lf",&min,&max);
        if(wi != NULL) wi->setScaleArc(min,max);
      }
      else if(strncmp(qwtcommand,"setOrigin(",10) == 0)
      {
        double o;
        sscanf(qwtcommand,"setOrigin(%lf",&o);
        if(wi != NULL) wi->setOrigin(o);
      }
      else if(strncmp(qwtcommand,"setNeedle(",10) == 0)
      {
        int which,r1,r2,r3,g1,g2,g3,b1,b2,b3;
        sscanf(qwtcommand,"setNeedle(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",&which,
                                       &r1,&g1,&b1,
                                       &r2,&g2,&b2,
                                       &r3,&g3,&b3);
        if(wi != NULL)
        {
          if     (which==1) wi->setNeedle(new QwtCompassWindArrow(QwtCompassWindArrow::Style2, QColor(r1,g1,b1), QColor(r2,g2,b2)));
          else if(which==2) wi->setNeedle(new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::ThinStyle, QColor(r1,g1,b1), QColor(r2,g2,b2)));
	        else if(which==6) wi->setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow,true, QColor(r1,g1,b1), QColor(r2,g2,b2)));
          else              wi->setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Ray,true, QColor(r1,g1,b1), QColor(r2,g2,b2)));
        }
      }
      else if(strncmp(qwtcommand,"setValue(",9) == 0)
      {
        double val;
        sscanf(qwtcommand,"setValue(%lf",&val);
        if(wi != NULL) wi->setValue(val);
      }
    }
  }
#endif //#ifndef NO_QWT
}

void Interpreter::interpretz(const char *command)
{
  if(command == NULL) return; // damn compiler warnings
  if(strncmp(command,"zoomMask(",9) == 0)
  {
    int x,y,w,h,percent;
    sscanf(command,"zoomMask(%d", &percent);
    for(int i=0; i<nmax; i++)
    {
      if(all[i]->w != NULL)
      {
        if(all[i]->x >= 0 && all[i]->y >= 0 && all[i]->width >= 0 && all[i]->height >= 0)
        {
          x = (all[i]->x      * percent) / 100;
          y = (all[i]->y      * percent) / 100;
          w = (all[i]->width  * percent) / 100;
          h = (all[i]->height * percent) / 100;
          all[i]->w->setGeometry(x,y,w,h);
        }
      }
    }
  }
}

void Interpreter::interpretQ(const char *command)
{
  if(strncmp(command,"QWidget(",8) == 0) // create a new QWidget
  {
    sscanf(command,"QWidget(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = new MyQWidget(s,i,all[p]->w);
    all[i]->type = TQWidget;
  }
  else if(strncmp(command,"QPushButton(",12) == 0) // create a new QPushButton
  {
    sscanf(command,"QPushButton(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQPushButton(s,i,all[p]->w);
    all[i]->type = TQPushButton;
  }
  else if(strncmp(command,"QLabel(",7) == 0) // create a new QLabel
  {
    sscanf(command,"QLabel(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyLabel(s,i,all[p]->w);
    all[i]->type = TQLabel;
  }
  else if(strncmp(command,"QLineEdit(",10) == 0) // create a new QLineEdit
  {
    MyLineEdit *ptr;
    sscanf(command,"QLineEdit(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyLineEdit(s,i,all[p]->w);
    all[i]->type = TQLineEdit;
    ptr = (MyLineEdit *) all[i]->w;
  }
  else if(strncmp(command,"QComboBox(",10) == 0) // create a new QComboBox
  {
    int editable,policy;
    QComboBox *ptr;
    sscanf(command,"QComboBox(%d,%d,%d,%d)",&i,&p,&editable,&policy);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyComboBox(s,i,all[p]->w);
    all[i]->type = TQComboBox;

    ptr = (MyComboBox *) all[i]->w;
    if(editable == 1) ptr->setEditable(true);
    if     (policy == 0) ptr->setInsertPolicy(QComboBox::NoInsert);
    else if(policy == 1) ptr->setInsertPolicy(QComboBox::InsertAtTop);
    else if(policy == 2) ptr->setInsertPolicy(QComboBox::InsertAtCurrent);
    else if(policy == 3) ptr->setInsertPolicy(QComboBox::InsertAtBottom);
    else if(policy == 4) ptr->setInsertPolicy(QComboBox::InsertAfterCurrent);
    else if(policy == 5) ptr->setInsertPolicy(QComboBox::InsertBeforeCurrent);
  }
  else if(strncmp(command,"QLCDNumber(",11) == 0) // create a new QLCDNumber
  {
    uint numDigits,segmentStyle,mode;

    sscanf(command,"QLCDNumber(%d,%d,%d,%d,%d)",&i,&p,&numDigits,&segmentStyle,&mode);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new QLCDNumber(numDigits,all[p]->w);
    all[i]->type = TQLCDNumber;
    QLCDNumber *ptr = (QLCDNumber *) all[i]->w;
    switch(mode)
    {
      case 0:
        ptr->setMode(QLCDNumber::Hex);
        break;
      case 1:
        ptr->setMode(QLCDNumber::Dec);
        break;
      case 2:
        ptr->setMode(QLCDNumber::Oct);
        break;
      case 3:
        ptr->setMode(QLCDNumber::Bin);
        break;
      default:
        break;
    }
    switch(segmentStyle)
    {
      case 0:
        ptr->setSegmentStyle(QLCDNumber::Outline);
        break;
      case 1:
        ptr->setSegmentStyle(QLCDNumber::Filled);
        break;
      case 2:
        ptr->setSegmentStyle(QLCDNumber::Flat);
        break;
      default:
        break;
    }
  }
  else if(strncmp(command,"QDrawWidget(",12) == 0) // create a new QDrawWidget
  {
    sscanf(command,"QDrawWidget(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;

    all[i]->w = (QWidget *) new QDrawWidget(all[p]->w,0,0,s,i);
    all[i]->type = TQDraw;
  }
  else if(strncmp(command,"QImage(",7) == 0) // create a new QImage
  {
    int  depth;
    sscanf(command,"QImage(%d,%d,%d,%d,%d",&i,&p,&w,&h,&depth);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    if(depth == 0) // image format other than 8bpp bmp
    {
      QString filename;
      get_text(command,text); // text = filename of image
      filename = temp + text;
      QImageWidget *imagewidget = new QImageWidget(s, i, all[p]->w);
      all[i]->w = (QWidget *) imagewidget;
      all[i]->type = TQImage;
      imagewidget->setImage(filename.toUtf8());
      return;
    }
    else if(depth == 8)
    {
      char *data;
      QImage *temp = readBmpFromSocket(w,h,&data);
      if(temp != NULL)
      {
        QImageWidget *imagewidget = new QImageWidget(s, i, all[p]->w);
        all[i]->w = (QWidget *) imagewidget;
        all[i]->type = TQImage;
        mySetAlphaBuffer(temp);
        imagewidget->setImage(temp);
        if(data != NULL) delete [] data;
        if(temp != NULL) delete temp;
      }
    }
  }
#ifdef USE_OPEN_GL
  else if(strncmp(command,"QPvGlWidget(",12) == 0) // create a new PvGlWidget
  {
    sscanf(command,"QPvGlWidget(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new PvGLWidget(all[p]->w,i,s);
    all[i]->type = TQGl;
  }
#endif
#ifdef USE_VTK
  else if(strncmp(command,"QVtkTclWidget(",14) == 0) // create a new pvVtkTclWidget
  {
    sscanf(command,"QVtkTclWidget(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new pvVtkTclWidget(all[p]->w,"vtkWidget",i,s);
    all[i]->type = TQVtk;
  }
#endif
#ifndef NO_QWT
  else if(strncmp(command,"QwtPlotWidget(",14) == 0) // create a new QwtPlotWidget
  {
    int nCurves, nMarker;
    sscanf(command,"QwtPlotWidget(%d,%d,%d,%d",&i,&p,&nCurves,&nMarker);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new QwtPlotWidget(s,i,all[p]->w,nCurves,nMarker);
    all[i]->type = TQwtPlotWidget;
  }
  else if(strncmp(command,"QwtScale(",9) == 0) // create a new QwtScale
  {
    int pos;
    sscanf(command,"QwtScale(%d,%d,%d",&i,&p,&pos);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtScale(s,i,(QwtScaleDraw::Alignment) pos, all[p]->w);
    all[i]->type = TQwtScale;
  }
  else if(strncmp(command,"QwtThermo(",10) == 0) // create a new QwtThermo
  {
    sscanf(command,"QwtThermo(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtThermo(s,i,all[p]->w);
    all[i]->type = TQwtThermo;
  }
  else if(strncmp(command,"QwtKnob(",8) == 0) // create a new QwtKnob
  {
    sscanf(command,"QwtKnob(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtKnob(s,i,all[p]->w);
    all[i]->type = TQwtKnob;
  }
  else if(strncmp(command,"QwtCounter(",11) == 0) // create a new QwtCounter
  {
    sscanf(command,"QwtCounter(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtCounter(s,i,all[p]->w);
    all[i]->type = TQwtCounter;
  }
  else if(strncmp(command,"QwtWheel(",9) == 0) // create a new QwtWheel
  {
    sscanf(command,"QwtWheel(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtWheel(s,i,all[p]->w);
    all[i]->type = TQwtWheel;
  }
  else if(strncmp(command,"QwtSlider(",10) == 0) // create a new QwtSlider
  {
    sscanf(command,"QwtSlider(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtSlider(s,i,all[p]->w);
    all[i]->type = TQwtSlider;
  }
  else if(strncmp(command,"QwtDial(",8) == 0) // create a new QwtDial
  {
    sscanf(command,"QwtDial(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtDial(s,i,all[p]->w);
    all[i]->type = TQwtDial;
  }
  else if(strncmp(command,"QwtCompass(",11) == 0) // create a new QwtCompass
  {
    sscanf(command,"QwtCompass(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtCompass(s,i,all[p]->w);
    all[i]->type = TQwtCompass;
  }
  else if(strncmp(command,"QwtAnalogClock(",15) == 0) // create a new QwtAnalogClock
  {
    sscanf(command,"QwtAnalogClock(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyQwtAnalogClock(s,i,all[p]->w);
    all[i]->type = TQwtAnalogClock;
  }
#endif //#ifndef NO_QWT
  else if(strncmp(command,"QSlider(",8) == 0) // create a new Slider
  {
    int minValue; int maxValue; int pageStep; int value; int orientation, qtori;
    sscanf(command,"QSlider(%d,%d,%d,%d,%d,%d,%d)",&i,&p,&minValue,&maxValue,&pageStep,&value,&orientation);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    if(orientation == PV::Horizontal) qtori = Qt::Horizontal;
    else                              qtori = Qt::Vertical;
    all[i]->w = (QWidget *) new MySlider(s,i,minValue,maxValue,pageStep,value,(Qt::Orientation) qtori,all[p]->w);
    all[i]->type = TQSlider;
  }
  else if(strncmp(command,"QRadio(",7) == 0) // create a new radio button
  {
    sscanf(command,"QRadio(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyRadioButton(s,i,all[p]->w);
    all[i]->type = TQRadio;
  }
  else if(strncmp(command,"QButtonGroup(",13) == 0) // create a new button group
  {
    int columns; int o;
    sscanf(command,"QButtonGroup(%d,%d,%d,%d,",&i,&p,&columns,&o);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    /* in Qt4 we use GroupBox instead of ButtonGroup !!!
      all[i]->w = (QWidget *) new MyButtonGroup(s,i,columns,(Qt::Orientation) o,text,all[p]->w);
      all[i]->type = TQButtonGroup;
    */
    if(o == PV::Horizontal) o = Qt::Horizontal;
    else                    o = Qt::Vertical;
    all[i]->w = (QWidget *) new MyGroupBox(s,i,columns,(Qt::Orientation) o,text,all[p]->w);
    all[i]->type = TQGroupBox;

  }
  else if(strncmp(command,"QCheckBox(",10) == 0) // create a new check box
  {
    sscanf(command,"QCheckBox(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyCheckBox(s,i,all[p]->w);
    all[i]->type = TQCheck;
  }
  else if(strncmp(command,"QFrame(",7) == 0) // create a new frame
  {
    int shape; int shadow; int line_width; int margin;
    sscanf(command,"QFrame(%d,%d,%d,%d,%d,%d)",&i,&p,&shape,&shadow,&line_width,&margin);

    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyFrame(s,i,shape,shadow,line_width,margin,all[p]->w);
    all[i]->type = TQFrame;
  }
  else if(strncmp(command,"QTabWidget(",11) == 0) // create a new QTabWidget
  {
    sscanf(command,"QTabWidget(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QTabWidget *) new MyQTabWidget(s,i,all[p]->w);
    all[i]->type = TQTabWidget;
  }
  else if(strncmp(command,"QToolBox(",9) == 0) // create a new QToolBox
  {
    sscanf(command,"QToolBox(%d,%d)",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QToolBox *) new MyQToolBox(s,i,all[p]->w);
    all[i]->type = TQToolBox;
  }
  else if(strncmp(command,"QGroupBox(",10) == 0) // create a new QGroupBox
  {
    int columns; int o;

    sscanf(command,"QGroupBox(%d,%d,%d,%d,",&i,&p,&columns,&o);
    get_text(command,text);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    if(o == PV::Horizontal) o = Qt::Horizontal;
    else                    o = Qt::Vertical;
    all[i]->w = (QWidget *) new MyGroupBox(s,i,columns,(Qt::Orientation) o,text,all[p]->w);
    all[i]->type = TQGroupBox;
  }
  else if(strncmp(command,"QListBox(",9) == 0) // create a new QListBox
  {
    sscanf(command,"QListBox(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyListBox(s,i,all[p]->w);
    all[i]->type = TQListBox;
  }
  else if(strncmp(command,"QTable(",7) == 0) // create a new QTable
  {
    int row; int col;
    sscanf(command,"QTable(%d,%d,%d,%d",&i,&p,&row,&col);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyTable(s,i,row,col,all[p]->w);
    all[i]->type = TQTable;
  }
  else if(strncmp(command,"QSpinBox(",9) == 0) // create a new QSpinBox
  {
    int min; int max; int step;
    sscanf(command,"QSpinBox(%d,%d,%d,%d,%d",&i,&p,&min,&max,&step);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MySpinBox(s,i,min,max,step,all[p]->w);
    all[i]->type = TQSpinBox;
  }
  else if(strncmp(command,"QDial(",6) == 0) // create a new QDial
  {
    int min; int max; int pageStep; int value;

    sscanf(command,"QDial(%d,%d,%d,%d,%d,%d",&i,&p,&min,&max,&pageStep,&value);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyDial(s,i,min,max,pageStep,value,all[p]->w);
    all[i]->type = TQDial;
  }
  else if(strncmp(command,"QProgressBar(",13) == 0) // create a new QProgressBar
  {
    int totalSteps;
    int orientation = (int) PV::Horizontal;
    Qt::Orientation qtori;
    sscanf(command,"QProgressBar(%d,%d,%d,%d",&i,&p,&totalSteps,&orientation);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    if(orientation == PV::Vertical) qtori = Qt::Vertical;
    else                            qtori = Qt::Horizontal;
    all[i]->w = (QWidget *) new MyProgressBar(s,i,totalSteps,qtori,all[p]->w);
    all[i]->type = TQProgressBar;
  }
  else if(strncmp(command,"QMultiLineEdit(",15) == 0) // create a new QMultiLineEdit
  {
    int editable,maxLines;
    sscanf(command,"QMultiLineEdit(%d,%d,%d,%d",&i,&p,&editable,&maxLines);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyMultiLineEdit(s,i,editable,maxLines,all[p]->w);
    all[i]->type = TQMultiLineEdit;
  }
  else if(strncmp(command,"QTextBrowser(",13) == 0) // create a new QTextBrowser
  {
    sscanf(command,"QTextBrowser(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyTextBrowser(s,i,all[p]->w);
    all[i]->type = TQTextBrowser;
  }
  else if(strncmp(command,"QListView(",10) == 0) // create a new QListView
  {
    sscanf(command,"QListView(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyListView(s,i,all[p]->w);
    all[i]->type = TQListView;
  }
  else if(strncmp(command,"QIconView(",10) == 0) // create a new QIconView
  {
    sscanf(command,"QIconView(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    all[i]->w = (QWidget *) new MyIconView(s,i,all[p]->w);
    all[i]->type = TQIconView;
  }
  else if(strncmp(command,"QDateEdit(",10) == 0) // create a new QDateEdit
  {
    sscanf(command,"QDateEdit(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    MyQDateEdit *ptr = new MyQDateEdit(s,i,all[p]->w);
    ptr->pvsVersion = mainWindow->pvbtab[mainWindow->currentTab].pvsVersion;
    all[i]->w = (QWidget *) ptr; 
    all[i]->type = TQDateEdit;
  }
  else if(strncmp(command,"QTimeEdit(",10) == 0) // create a new QTimeEdit
  {
    sscanf(command,"QTimeEdit(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    MyQTimeEdit *ptr = new MyQTimeEdit(s,i,all[p]->w);
    ptr->pvsVersion = mainWindow->pvbtab[mainWindow->currentTab].pvsVersion;
    all[i]->w = (QWidget *) ptr; 
    all[i]->type = TQTimeEdit;
  }
  else if(strncmp(command,"QDateTimeEdit(",14) == 0) // create a new QDateTimeEdit
  {
    sscanf(command,"QDateTimeEdit(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    MyQDateTimeEdit *ptr = new MyQDateTimeEdit(s,i,all[p]->w);
    ptr->pvsVersion = mainWindow->pvbtab[mainWindow->currentTab].pvsVersion;
    all[i]->w = (QWidget *) ptr; 
    all[i]->type = TQDateTimeEdit;
  }
  else if(strncmp(command,"QToolTip(",9) == 0) // create a new QToolTip
  {
    sscanf(command,"QToolTip(%d",&i);
    if(i < 0) return;
    if(i >= nmax) return;
    get_text(command,text);
    if(all[i]->w != NULL) all[i]->w->setToolTip(text);
  }
  else if(strncmp(command,"QLayoutVbox(",12) == 0) // vbox layout
  {
    sscanf(command,"QLayoutVbox(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    if(i == 0)
    {
      mainLayout = (QWidget *) new QVBoxLayout(all[0]->w);
      mainLayoutType = TQVbox;
      ((QVBoxLayout *) mainLayout)->setMargin(11);
      ((QVBoxLayout *) mainLayout)->setSpacing(6);
    }
    else
    { 
      if(p >= 0) 
      {
        all[i]->w = (QWidget *) new QVBoxLayout(all[p]->w);
      }
      else
      {
        all[i]->w = (QWidget *) new QVBoxLayout();
      }
      all[i]->type = TQVbox;
    }
    hasLayout = 1;
  }
  else if(strncmp(command,"QLayoutHbox(",12) == 0) // hbox layout
  {
    sscanf(command,"QLayoutHbox(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    if(opt.arg_debug) printf("QLayoutHbox()\n");
    if(i == 0)
    {
      mainLayout = (QWidget *) new QHBoxLayout(all[0]->w);
      mainLayoutType = TQHbox;
      ((QHBoxLayout *) mainLayout)->setMargin(11);
      ((QHBoxLayout *) mainLayout)->setSpacing(6);
    }
    else
    {
      if(p >= 0) 
      {
        all[i]->w = (QWidget *) new QHBoxLayout(all[p]->w);
      }  
      else
      {
        all[i]->w = (QWidget *) new QHBoxLayout();
      }
      all[i]->type = TQHbox;
    }
    hasLayout = 1;
    if(opt.arg_debug) printf("QLayoutHbox() end\n");
  }
  else if(strncmp(command,"QLayoutGrid(",12) == 0) // grid layout
  {
    sscanf(command,"QLayoutGrid(%d,%d",&i,&p);
    if(i < 0) return;
    if(i >= nmax) return;
    if(p >= nmax) return;
    if(i == 0)
    {
      mainLayout = (QWidget *) new QGridLayout(all[0]->w);
      mainLayoutType = TQGrid;
      ((QGridLayout *) mainLayout)->setMargin(11);
      ((QGridLayout *) mainLayout)->setSpacing(6);
    }
    else
    {
      if(p >= 0) 
      {
        all[i]->w = (QWidget *) new QGridLayout(all[p]->w);
      }  
      else
      {
        all[i]->w = (QWidget *) new QGridLayout();
      }
      all[i]->type = TQGrid;
    }
    hasLayout = 1;
  }

  if(opt.arg_fillbackground == 1 && i>0 && i<nmax) // murnleitner special
  {
    if(all[i]->type != TQVbox && all[i]->type != TQHbox && all[i]->type != TQGrid)
    {
      if(all[i]->w != NULL) all[i]->w->setAutoFillBackground(true);
      // all[i]->w->setBackgroundMode(Qt::PaletteBackground);
      // all[i]->w->setForegroundRole(QPalette::WindowText);
      // all[i]->w->setForegroundRole(QPalette::ButtonText);
      // all[i]->w->setBackgroundRole(QPalette::ButtonText);
      // all[i]->w->setBackgroundRole(QPalette::Window);
      // all[i]->w->setForegroundRole(QPalette::WindowText);
    }
  }

  // font zoom issue reported by ernst murnleitner
  //if(i == 0 && (zoom != 100 || opt.fontzoom != 100) && hasLayout == 0)
  if(i == 0 && (zoom != 100 || opt.fontzoom != 100) )
  {
    QFont font = qApp->font();
    font.setPointSize((font.pointSize()*zoom*opt.fontzoom)/(100*100));
    if(all[i]->w != NULL) all[i]->w->setFont(font);
  }
}

void Interpreter::showMyBrowser(const char *url)
{
  if(opt.arg_debug) printf("showMyBrowser begin\n");

  if(1) // startDefinition
  {
    char *ptr;
    mainLayout = NULL;
    hasLayout = 1;
    //if(opt.arg_debug) printf("startDefinition command=%s",command);
    //sscanf(command,"startDefinition(%d)",&n);
    n = 2;
    if(allBase == NULL)
    {
      if(opt.arg_debug) printf("startDefinition free(all)\n");
      if(all != NULL) free(all);
      //v = new MyQWidget(s,0,mainWindow);
      dlgMyBrowser *browser = new dlgMyBrowser(s,0,mainWindow);
      if(opt.arg_debug) printf("showMyBrowser url=%s\n", url);
      browser->setUrl(url);
      mainWidget = (MyQWidget *) browser;
    }
    if(opt.arg_debug) printf("startDefinition malloc(all)\n");
    ptr = (char *) malloc(n*sizeof(ptr) + n*sizeof(ALL));
    if(ptr==NULL) qFatal("out of memory -> exit");
    all = (ALL **) ptr;
    ptr += n*sizeof(ptr);
    for(i=0; i<n; i++)
    {
      all[i] = (ALL *) (ptr + i*sizeof(ALL));
      all[i]->x = all[i]->y = all[i]->width = all[i]->height = -1;
    }
    if(allBase == NULL)
    {
      if(opt.arg_debug) printf("startDefinition all[0]->w = v;\n");
      all[0]->w = mainWidget;
    }
    else
    {
      if(opt.arg_debug) printf("startDefinition all[0]->w = modalDialog;\n");
      all[0]->w = modalDialog;
    }

    nmax = n;
    if(opt.arg_debug) printf("startDefinition 2\n");
    for(i=1; i<n; i++)
    {
      all[i]->w = (QWidget *) NULL;
      all[i]->type = -1;
    }
    if(allBase != NULL)
    {
      if(opt.arg_debug) printf("startDefinition allBase != NULL\n");
      allModal = all;
      nmaxModal = nmax;
    }
    if(opt.arg_debug) printf("startDefinition end\n");
  }

  if(1) // endDefinition
  {
    if(allBase == NULL)
    {
      if(opt.arg_debug) printf("endDefinition begin\n");
      delete mainWindow->scroll->takeWidget();
      w = mainWindow->width();
      h = mainWindow->height();
      mainWindow->pvbtab[mainWindow->currentTab].w = w;
      mainWindow->pvbtab[mainWindow->currentTab].h = h;
      all[0]->w->resize(w,h);
      if(mainWindow->scroll == NULL)
      {
        printf("BIG FAT ERROR: scroll==NULL\n");
        exit(-1);
      }
      if(hasLayout == 1)
      { 
        mainWindow->scroll->setWidgetResizable(true);
      }  
      else
      {
        mainWindow->scroll->setWidgetResizable(false);
        if(all[0]->w != NULL) all[0]->w->resize(1280,1024); // resize to default screen dimension
      }  
      mainWindow->scroll->setWidget(all[0]->w);
      // workaround for qt
      QApplication::postEvent(mainWindow, new QResizeEvent(QSize(w-1,h-1),QSize(w,h))); // force qt to update sliders
      //QCoreApplication::processEvents();
      //QApplication::postEvent(mainWindow, new QResizeEvent(QSize(w,h),QSize(w-1,h-1)));
      if(opt.arg_debug) printf("endDefinition end\n");
    }
    else
    {
      QString capt;
      if(opt.arg_debug) printf("endDefinition allBase != NULL modalDialog\n");
      capt.sprintf("%s - pvbrowser",(const char *) mainWindow->curFile.toUtf8());
      modalDialog->setWindowTitle(capt);
      modalDialog->show();
    }
    if(opt.arg_debug) printf("endDefinition end\n");
  }
  if(opt.arg_debug) printf("showMyBrowser end\n");
}

void Interpreter::perhapsCloseModalDialog()
{
  if(modalDialog != NULL)
  {
    delete modalDialog;
    modalDialog = NULL;
  }
}

