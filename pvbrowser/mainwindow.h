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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if QT_VERSION < 0x050000
  #define VERSION "5.15.3 - qt4"
#else
  #define VERSION "5.15.3 - qt5"
#endif

#if defined PVB_FOOTPRINT_BASIC
#define WEBVERSION " Small Footprint without Webkit or Blink"
#elif defined USE_GOOGLE_WEBKIT_FORK
#define WEBVERSION " Google Blink"
#elif defined NO_WEBKIT
#define WEBVERSION " no Webkit"
#else
#define WEBVERSION " Webkit"
#endif

#include "interpreter.h"

#include <QMainWindow>
#include <QScrollArea>
#include <QThread>
#include <QSemaphore>
#include <QTabBar>

#ifdef USE_SYMBIAN
    #define  MAX_TABS 5
    #define  MAX_TAB_TEXT_LENGTH 20
    #define  MAX_DOCK_WIDGETS   32
#else

    #define  MAX_TABS 32
    #define  MAX_TAB_TEXT_LENGTH 20
    #define  MAX_DOCK_WIDGETS   32
#endif
#define  DOCK_WIDGETS_BASE -1000
typedef  MyQDockWidget* MyQDockWidgetPtr;

class QAction;
class QMenu;
class QTextEdit;
class QComboBox;
class QLabel;
class MainWindow;
class MyQWidget;
#ifdef PVB_FOOTPRINT_BASIC
#include "dlgmybrowser_without_www.h"
#include "dlgmybrowser_ui_without_www.h"
#else
#include "dlgmybrowser_with_www.h"
#include "dlgmybrowser_ui_with_www.h"
#endif
#include "dlgtextbrowser.h"

class MainWindow;

class MyScrollArea : public QScrollArea
{
  Q_OBJECT
  public:
    MyScrollArea(QWidget *parent=0);
    ~MyScrollArea();
    void wheelEvent(QWheelEvent *event);
  protected:
    bool event(QEvent *event);
  private:
    MainWindow *mw;
};            

class MyThread : public QThread
{
  Q_OBJECT
  public:
    void run();
    MainWindow *pv;
  signals:
    void dataReady(int ind);
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Q_PROPERTY(QString url WRITE slotUrl)

  Q_CLASSINFO("ClassID", "{5a22176d-118f-4185-9653-9f98958a6df8}")
  Q_CLASSINFO("InterfaceID", "{2df735ba-da4f-4fb7-8f35-b8dfbf8cfd9a}")
  Q_CLASSINFO("EventsID", "{449de213-f8bd-4d2e-a2cf-eab407c03245}")

  Q_CLASSINFO("MIME", "pvbrowser/pv:pvb:pvbrowser:Interactive web applications and HMI/SCADA")
  Q_CLASSINFO("ToSuperClass", "pvbrowser")
  Q_CLASSINFO("DefaultProperty", "url")

  typedef struct
  {
    int s;           // socket
    int in_use;      // 1 if is used else 0
    int w,h;         // width, height
    int pvsVersion;  // version of pvserver
    QString          url;
    QString          manual_url;
    QWidget         *rootWidget;
    Interpreter      interpreter;
    int              hasLayout;
    MyQDockWidgetPtr dock[MAX_DOCK_WIDGETS];
  }PVBTAB;

public:
  MainWindow();
  ~MainWindow();
  //int *s; // the socket
  int maxfd, currentTab, numTabs;
  QSemaphore semaphore;
  void setCurrentFile(const QString &fileName);
  void setTabText(const char *text);
  MyScrollArea *scroll;
  QString curFile;
  QToolBar *fileToolBar;
  QToolBar *tabToolBar;
  QTabBar  *tabBar;
#ifdef NO_WEBKIT  
#else
  dlgTextBrowser *textbrowser;
#endif  
  PVBTAB pvbtab[MAX_TABS];
  QComboBox *urlComboBox;
  MyThread mythread;
  QMap <QString,QLibrary *> libs; // custom widget plugin libs
  QMap <QString,QWidget *(*)(const char * , int *, int , QWidget *, const char * )> newCustomWidget;
  QPushButton *busyWidget;
  QString busyWidgetText;
  void showBusyWidget(int milliseconds, const char *text);
  void hideBusyWidget();

public slots:
  void slotEditToolStatus(int edit, int tool, int status);
  void slotUrl(const QString &url);
  void dataReceived(int ind);
  void slotReconnect();
  void slotTimeOut();

protected:
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  void closeEvent(QCloseEvent *event);

public slots:
  void slotWhatsThis();
  void slotFileOpt();
  void slotWindow();
  void slotStorebmp();
  void slotGohome();
  void slotLogbmp();
  void slotLogpvm();
  void slotPrint();
  void slotCopy();
  void slotEditmenu();
  void slotToolbar();
  void slotStatusbar();
  void slotMaximized();
  void slotFullscreen();
  void slotManual();
  void about();
  void slotExit();
  void slotTabChanged(int index);
  void slotNewTab();
  void slotDeleteTab();
  void slotBusyWidgetTimeout();
  void snapshot(QPixmap &pm);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  QString strippedName(const QString &fullFileName);
  //void resizeEvent(QResizeEvent *event);
  void readHosts();
  void add_host(const char *host);
  void appendIniFile(const char *host);

  QPushButton *deleteTab;
  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;
  QAction *whatsthisAct;
  QAction *optionAct;
  QAction *windowAct;
  QAction *reconnectAct;
  QAction *reconnectActToolBar;
  QAction *editmenuActToolBar;
  QAction *statusbarActToolBar;
  QAction *storebmpAct;
  QAction *gohomeAct;
  QAction *logbmpAct;
  QAction *logpvmAct;
  QAction *printAct;
  QAction *newtabActToolBar;
  QAction *newtabAct;
  QAction *newAct;
  QAction *openAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *exitAct;
  QAction *copyAct;
  QAction *editmenuAct;
  QAction *toolbarAct;
  QAction *statusbarAct;
  QAction *maximizedAct;
  QAction *fullscreenAct;
  QAction *manualAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QAction *logoAct;

  int isReconnect;
  QString url;
  QLabel *logoLabel;
  QTimer *timer;
  QTimer *busyWidgetTimer;
};

#endif
