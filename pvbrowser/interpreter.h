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
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "opt.h"
#include <QtGui>

// application specific includes
#include "MyWidgets.h"
#include "qdrawwidget.h"
#include "qimagewidget.h"
#ifdef USE_OPEN_GL
#include "pvglwidget.h"
#endif
#ifndef NO_QWT
#include "qwtplotwidget.h"
#endif
#ifdef USE_VTK
#include "pvVtkTclWidget.h"
#endif

enum
{
TQWidget = 0,
TQPushButton,
TQLabel,
TQLineEdit,
TQComboBox,
TQLCDNumber,
TQButtonGroup,
TQRadio,
TQCheck,
TQSlider,
TQFrame,
TQImage,
TQDraw,
TQGl,
TQTabWidget,
TQGroupBox,
TQListBox,
TQTable,
TQSpinBox,
TQDial,
TQProgressBar,
TQMultiLineEdit,
TQTextBrowser,
TQListView,
TQIconView,
TQVtk,
TQwtPlotWidget,
TQwtScale,
TQwtThermo,
TQwtKnob,
TQwtCounter,
TQwtWheel,
TQwtSlider,
TQwtDial,
TQwtCompass,
TQwtAnalogClock,
TQDateEdit,
TQTimeEdit,
TQDateTimeEdit,
TQToolBox,
TQVbox,
TQHbox,
TQGrid,
TQDock_Widget,
TQCustomWidget
};

typedef struct
{
  QWidget *w;
  int     type;
  int     x,y,width,height;
  int     fontsize;
}ALL;

class MainWindow;

/**
  Events send to custom widgets
  */
const QEvent::Type pvbEventNumber = (QEvent::Type) (QEvent::User + 1);

class PvbEvent : public QEvent
{
public:
  PvbEvent(const char *_command, QString _param, QEvent::Type _event=pvbEventNumber);
  virtual ~PvbEvent();
  const char *command;
  QString  param;
};

/**
  * This Class is the interpreter.
  */
class Interpreter
{
  public:
    Interpreter();
    ~Interpreter();
    void showMyBrowser(const char *url);
    void registerMainWindow(MainWindow *mainWindow, int *sock);
    void interpret(const char *command);
    void setPath(const char *p);
    void metaFileLog();
    void metaFileBmp();
    void slotModalTerminate();
    void perhapsCloseModalDialog();
    int  width();
    int  height();
    void zoomMask(int percent);
    QString temp;
    int  hasLayout;
    int  percentZoomMask;

//protected:
//    bool eventFilter( QObject *o, QEvent *e );

  private:
    void interpreta(const char *command);
    void interpretb(const char *command);
    void interpretc(const char *command);
    void interpretd(const char *command);
    void interprete(const char *command);
    void interpretf(const char *command);
    void interpretg(const char *command);
    void interpreth(const char *command);
    void interpreti(const char *command);
    void interpretm(const char *command);
    void interpretp(const char *command);
    void interpretr(const char *command);
    void interprets(const char *command);
    void interprett(const char *command);
    void interpretv(const char *command);
    void interpretw(const char *command);
    void interpretq(const char *command);
    void interpretz(const char *command);
    void interpretQ(const char *command);
    int  get_text(const char *c, QString &t);
    void addPopupMenuAction(QMenu *m, const char *cmd);
    void popupMenu(int id_return, const char *menu);
    QImage *readBmpFromSocket(int w, int h, char **d);
    void downloadFile(const char *file);
    void myCreateHeuristicMask(QPixmap &pm, QImage *temp);
    void mySetAlphaBuffer(QImage *temp);
    void deleteWidget(QWidget *w);

    int *s; // the socket
    ALL **all;
    ALL **allBase;
    ALL **allModal;
    MainWindow *mainWindow;
    MyDialog *modalDialog;
    QWidget *mainLayout;
    int      mainLayoutType;

    QWidget *mainWidget;
    int  nmax;
    int  nmaxBase;
    int  nmaxModal;

    char path[1024];
    // variables for interpretx
    int  i,p,n,x,y,w,h,r,g,b,zoom,zoom_original;
    QString text;
    time_t timet; // remote file time for caching
};

#endif

