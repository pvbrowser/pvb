/***************************************************************************
                          MyWidgets.cpp  -  description
                             -------------------
    begin                : Mon Dec 11 2000
    copyright            : (C) 2000 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "pvdefine.h"
#include "pvserver.h"
#include <stdio.h>
#include <ctype.h>
#include "opt.h"
#include "mainwindow.h"
#include "interpreter.h"

#include "qtabbar.h"
#include "qpainter.h"
#include "qmessagebox.h"
#include <QPixmap>
#include <QMouseEvent>
//v5diff #ifndef NO_WEBKIT
//v5diff #include <QWebHistory>
//v5diff #include <QWebFrame>
//v5diff #endif
#include "tcputil.h"

#ifdef PVB_FOOTPRINT_BASIC
#warning "We use: PVB_FOOTPRINT_BASIC (in fact this is an INFO and not a WARNING)"
#else
#warning "We use: PVB_FOOTPRINT with WWW (in fact this is an INFO and not a WARNING)"
#endif

#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
#warning "We use: PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62 (in fact this is an INFO and not a WARNING)"
#else
#warning "We use: QWT62 (in fact this is an INFO and not a WARNING)"
#endif

extern OPT opt;

extern QString l_file;
extern QString l_options;
extern QString l_new_window;
extern QString l_reconnect;
extern QString l_save_as_bmp;
extern QString l_log_as_bmp;
extern QString l_log_as_pvm;
extern QString l_print;
extern QString l_new_tab;
extern QString l_delete_tab;
extern QString l_exit;
extern QString l_edit;
extern QString l_copy;
extern QString l_copy_plus_title;
extern QString l_view;
extern QString l_editmenu;
extern QString l_toolbar;
extern QString l_statusbar;
extern QString l_maximized;
extern QString l_fullscreen;
extern QString l_help;
extern QString l_manual;
extern QString l_about;

extern QString l_status_connection_lost;
extern QString l_status_connected;
extern QString l_status_could_not_connect;
extern QString l_status_reconnect;
extern QString l_status_options;
extern QString l_status_new_window;
extern QString l_status_save_as_bmp;
extern QString l_status_log_as_bmp;
extern QString l_status_log_as_pvm;
extern QString l_status_print;
extern QString l_status_new_tab;
extern QString l_status_exit;
extern QString l_status_copy;
extern QString l_status_editmenu;
extern QString l_status_toolbar;
extern QString l_status_statusbar;
extern QString l_status_toggle_maximized;
extern QString l_status_toggle_full_screen;
extern QString l_status_manual;
extern QString l_status_about;

extern QString l_print_header;

static const char *decode(QString text)
{
  static char buf[MAX_PRINTF_LENGTH];
  int maxlen = MAX_PRINTF_LENGTH - 1;

  if(opt.codec == pvbUTF8)
  {
    strncpy(buf, text.toUtf8(), maxlen);
  }
  else
  {
#if QT_VERSION < 0x050000 
    strncpy(buf, text.toAscii(), maxlen);
#endif    
  }
  buf[maxlen-1] = '\0';
  return buf;
}

////////////////////////////////////////////////////////////////////////////////
MyDialog::MyDialog(Interpreter *inter, int *sock, int ident, QWidget * parent, const char *name, bool modal)
         :QDialog(parent, Qt::Widget)
{
  setAttribute(Qt::WA_DeleteOnClose);
  s  = sock;
  id = ident;
  interpreter = inter;
  setModal(modal);
  if(name != NULL) setObjectName(name);
  QIcon appIcon(":/images/app.png");
  setWindowIcon(appIcon);
}

MyDialog::~MyDialog()
{
#ifndef PVDEVELOP
  interpreter->slotModalTerminate();
#endif
}

void MyDialog::done(int result)
{
#ifndef PVDEVELOP
  char buf[80];
  sprintf(buf,"QPushButton(-1)\n");
  tcp_send(s,buf,strlen(buf));
  //interpreter->slotModalTerminate();
#endif
  if(result == 0) return;
}

////////////////////////////////////////////////////////////////////////////////
MyQWidget::MyQWidget(int *sock, int ident, QWidget * parent, const char * name)
          :QWidget(parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
}

MyQWidget::~MyQWidget()
{
}

////////////////////////////////////////////////////////////////////////////////
MyLabel::MyLabel(int *sock, int ident, QWidget * parent, const char *name)
              :QLabel(parent)
{
  s = sock;
  id = ident;
  row = col = -1;
  if(name != NULL) setObjectName(name);
}

MyLabel::~MyLabel()
{
}

void MyLabel::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  if(row != -1 || col != -1)
  {
    sprintf(buf,"QTableValue(%d,%d,%d,\"%s\")\n",id,row,col,decode(text()));
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
    tcp_send(s,buf,strlen(buf));
    QLabel::mousePressEvent(event);
  }  
}

void MyLabel::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  if(row != -1 || col != -1)
  {
  }
  else
  {
    sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
    if(underMouse()) tcp_send(s,buf,strlen(buf));
  }  
  QLabel::mouseReleaseEvent(event);
}

void MyLabel::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QLabel::enterEvent(event);
}

void MyLabel::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QLabel::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyQPushButton::MyQPushButton(int *sock, int ident, QWidget * parent, const char * name)
              :QPushButton(parent)
{
  s = sock;
  id = ident;
  row = col = -1;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(pressed()),  SLOT(slotPressed()));
  connect(this, SIGNAL(released()), SLOT(slotReleased()));
  connect(this, SIGNAL(clicked()),  SLOT(slotClicked()));
}

MyQPushButton::~MyQPushButton()
{
}

void MyQPushButton::slotClicked()
{
char buf[80];

  if(row != -1 || col != -1)
  {
    sprintf(buf,"QTableValue(%d,%d,%d,\"%s\")\n",id,row,col,decode(text()));
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    sprintf(buf,"QPushButton(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }  
}

void MyQPushButton::slotPressed()
{
char buf[80];

  if(row != -1 || col != -1) return;
  sprintf(buf,"QPushButtonPressed(%d)\n",id);
  tcp_send(s,buf,strlen(buf));
}

void MyQPushButton::slotReleased()
{
char buf[80];

  if(row != -1 || col != -1) return;
  sprintf(buf,"QPushButtonReleased(%d)\n",id);
  tcp_send(s,buf,strlen(buf));
}

void MyQPushButton::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QPushButton::enterEvent(event);
}

void MyQPushButton::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QPushButton::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyLineEdit::MyLineEdit(int *sock, int ident, QWidget * parent, const char * name)
           :QLineEdit(parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged(const QString &)));
  connect(this, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
}

MyLineEdit::~MyLineEdit()
{
}

void MyLineEdit::slotTextChanged(const QString &txt)
{
  char buf[MAX_PRINTF_LENGTH];

  if(txt.length()+40 > MAX_PRINTF_LENGTH) return;
  sprintf(buf,"text(%d,\"%s\")\n", id, decode(txt));
  tcp_send(s,buf,strlen(buf));
}

void MyLineEdit::slotReturnPressed()
{
  char buf[MAX_PRINTF_LENGTH];

  sprintf(buf,"QPushButton(%d,\"%s\")\n",id, decode(text()));
  tcp_send(s,buf,strlen(buf));
}

void MyLineEdit::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QLineEdit::mousePressEvent(event);
}

void MyLineEdit::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QLineEdit::mouseReleaseEvent(event);
}

void MyLineEdit::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QLineEdit::enterEvent(event);
}

void MyLineEdit::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QLineEdit::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyComboBox::MyComboBox(int *sock, int ident, QWidget * parent, const char * name)
           :QComboBox(parent)
{
  s = sock;
  id = ident;
  row = col = -1;
  //setCompleter(0);
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(activated(const QString &)), SLOT(slotActivated(const QString &)));
}

MyComboBox::~MyComboBox()
{
}

void MyComboBox::slotActivated(const QString &txt)
{
  char buf[MAX_PRINTF_LENGTH];
  QString txt2 = txt;
  
  if(txt2.isEmpty())
  {
    //rlmurx-was-here txt2.sprintf("index%d", currentIndex());
    txt2 = QString::asprintf("index%d", currentIndex());
  }

  if(row != -1 || col != -1)
  {
    sprintf(buf,"QTableValue(%d,%d,%d,\"%s\")\n",id,row,col,decode(txt2));
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    if(txt2.length()+40 > MAX_PRINTF_LENGTH) return;
    sprintf(buf,"text(%d,\"%s\")\n", id, decode(txt2));
    tcp_send(s,buf,strlen(buf));
  }
}

void MyComboBox::removeItemByName(QString name)
{
  int i = 0;
  while(i < count())
  {
    if(itemText(i) == name) removeItem(i);
    i++;
  }
}

void MyComboBox::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QComboBox::mousePressEvent(event);
}

void MyComboBox::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QComboBox::mouseReleaseEvent(event);
}

void MyComboBox::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QComboBox::enterEvent(event);
}

void MyComboBox::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QComboBox::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MySlider::MySlider(int *sock, int ident, int minValue, int maxValue, int pageStep, int value, Qt::Orientation orientation, QWidget * parent, const char * name)
         :QSlider(orientation,parent)
{
  s = sock;
  id = ident;
  setMaximum(maxValue);
  setMinimum(minValue);
  setPageStep(pageStep);
  setValue(value);
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
}

MySlider::~MySlider()
{
}

void MySlider::slotValueChanged(int value)
{
  char buf[80];

  sprintf(buf,"slider(%d,%d)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MySlider::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QSlider::mousePressEvent(event);
}

void MySlider::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QSlider::mouseReleaseEvent(event);
}

void MySlider::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QSlider::enterEvent(event);
}

void MySlider::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QSlider::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyCheckBox::MyCheckBox(int *sock, int ident, QWidget * parent, const char * name)
           :QCheckBox(parent)
{
  s = sock;
  id = ident;
  row = col = -1;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(clicked()), SLOT(slotClicked()));
}

MyCheckBox::~MyCheckBox()
{
}

void MyCheckBox::slotClicked()
{
char buf[80];

  if(row != -1 || col != -1)
  {
    QString txt;
    QString sbuf = text();
    if(isChecked()) txt = "1," + sbuf;
    else            txt = "0," + sbuf;
    sprintf(buf,"QTableValue(%d,%d,%d,\"%s\")\n",id,row,col,decode(txt));
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    if(isChecked()) sprintf(buf,"check(%d,1)\n",id);
    else            sprintf(buf,"check(%d,0)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
}

void MyCheckBox::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QCheckBox::mousePressEvent(event);
}

void MyCheckBox::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QCheckBox::mouseReleaseEvent(event);
}

void MyCheckBox::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QCheckBox::enterEvent(event);
}

void MyCheckBox::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QCheckBox::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyRadioButton::MyRadioButton(int *sock, int ident, QWidget * parent, const char * name)
              :QRadioButton(parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(toggled(bool)), SLOT(slotToggled(bool)));
}

MyRadioButton::~MyRadioButton()
{
}

void MyRadioButton::slotToggled(bool on)
{
char buf[80];

  if(on) sprintf(buf,"radio(%d,1)\n",id);
  else   sprintf(buf,"radio(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
}

void MyRadioButton::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QRadioButton::mousePressEvent(event);
}

void MyRadioButton::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QRadioButton::mouseReleaseEvent(event);
}

void MyRadioButton::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QRadioButton::enterEvent(event);
}

void MyRadioButton::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QRadioButton::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyButtonGroup::MyButtonGroup(int *sock, int ident, int columns, Qt::Orientation o, QString title, QWidget * parent, const char * name)
              :QGroupBox(title,parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  if(columns == (int) o) return; // troll porting murx
}

MyButtonGroup::~MyButtonGroup()
{
}

void MyButtonGroup::mousePressEvent(QMouseEvent *e)
{
  char buf[80];

  if(e->button() == Qt::RightButton)
  {
    sprintf(buf,"QMouseRight(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, e->x(), e->y());
    tcp_send(s,buf,strlen(buf));
  }
  //QGroupBox::mousePressEvent(event);
}

void MyButtonGroup::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  //QGroupBox::mouseReleaseEvent(event);
}

void MyButtonGroup::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QGroupBox::enterEvent(event);
}

void MyButtonGroup::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QGroupBox::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyFrame::MyFrame(int *sock, int ident, int shape, int shadow, int line_width, int margin,
                 QWidget * parent, const char * name)
        :QFrame(parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  setFrameShape  ((QFrame::Shape)  shape);
  setFrameShadow ((QFrame::Shadow) shadow);
  setLineWidth   (line_width);
  //setMargin      (margin);
  if(margin == -1000) return;
}

MyFrame::~MyFrame()
{
}

void MyFrame::mousePressEvent(QMouseEvent *e)
{
  char buf[80];

  if(e->button() == Qt::RightButton)
  {
    sprintf(buf,"QMouseRight(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, e->x(), e->y());
    tcp_send(s,buf,strlen(buf));
  }
  //QFrame::mousePressEvent(event);
}

void MyFrame::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  //QFrame::mouseReleaseEvent(event);
}

void MyFrame::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QFrame::enterEvent(event);
}

void MyFrame::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QFrame::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyQTabWidget::MyQTabWidget(int *sock, int ident, QWidget *parent, const char *name)
             :QTabWidget(parent)
{
  id = ident;
  s  = sock;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(currentChanged(int)), SLOT(slotCurrentChanged(int)));
}

MyQTabWidget::~MyQTabWidget()
{
}

void MyQTabWidget::slotCurrentChanged(int index)
{
char buf[80];

  sprintf(buf,"tab(%d,%d)\n",id,index);
  tcp_send(s,buf,strlen(buf));
}

void MyQTabWidget::enableTabBar(int state)
{
  if(state == 0) tabBar()->setEnabled(false);
  else           tabBar()->setEnabled(true);
}

void MyQTabWidget::mousePressEvent(QMouseEvent *e)
{
  char buf[80];

  if(e->button() == Qt::RightButton)
  {
    sprintf(buf,"QMouseRight(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, e->x(), e->y());
    tcp_send(s,buf,strlen(buf));
  }
  //QTabWidget::mousePressEvent(event);
}

void MyQTabWidget::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QTabWidget::mouseReleaseEvent(event);
}

void MyQTabWidget::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QTabWidget::enterEvent(event);
}

void MyQTabWidget::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QTabWidget::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyQToolBox::MyQToolBox(int *sock, int ident, QWidget *parent, const char *name)
             :QToolBox(parent)
{
  id = ident;
  s  = sock;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(currentChanged(int)), SLOT(slotCurrentChanged(int)));
}

MyQToolBox::~MyQToolBox()
{
}

void MyQToolBox::slotCurrentChanged(int index)
{
char buf[80];

  sprintf(buf,"tab(%d,%d)\n",id,index);
  tcp_send(s,buf,strlen(buf));
}

void MyQToolBox::mousePressEvent(QMouseEvent *e)
{
  char buf[80];

  if(e->button() == Qt::RightButton)
  {
    sprintf(buf,"QMouseRight(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, e->x(), e->y());
    tcp_send(s,buf,strlen(buf));
  }
  //QToolBox::mousePressEvent(event);
}

void MyQToolBox::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  //QToolBox::mouseReleaseEvent(event);
}

void MyQToolBox::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QToolBox::enterEvent(event);
}

void MyQToolBox::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QToolBox::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyGroupBox::MyGroupBox(int *sock, int ident, int columns, Qt::Orientation o, QString title, QWidget * parent, const char * name)
           :QGroupBox(title,parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  if(columns == (int) o) return; // troll porting murx
}

MyGroupBox::~MyGroupBox()
{
}

void MyGroupBox::mousePressEvent(QMouseEvent *e)
{
  char buf[80];

  if(e->button() == Qt::RightButton)
  {
    sprintf(buf,"QMouseRight(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
  else
  {
    sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, e->x(), e->y());
    tcp_send(s,buf,strlen(buf));
  }
  //QGroupBox::mousePressEvent(event);
}

void MyGroupBox::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  //QGroupBox::mouseReleaseEvent(event);
}

void MyGroupBox::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QGroupBox::enterEvent(event);
}

void MyGroupBox::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  if(event == NULL) return;
  //QGroupBox::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyListBox::MyListBox(int *sock, int ident, QWidget *parent, const char *name)
          :QListWidget(parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotClicked(QListWidgetItem *)));
  connect(this, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), SLOT(slotCurrentChanged(QListWidgetItem *, QListWidgetItem *)));
  connect(this, SIGNAL(itemSelectionChanged()), SLOT(slotSendSelected()));
}

MyListBox::~MyListBox()
{
}

void MyListBox::insertItem(QPixmap &pixmap, const QString &text, int index)
{
  int row = index;
  QListWidgetItem *item;

  if(pixmap.isNull()) item = new QListWidgetItem(text);
  else                item = new QListWidgetItem(pixmap, text);
  if(row == -1) QListWidget::addItem(item);
  else          QListWidget::insertItem(row,item);
}

void MyListBox::changeItem(QPixmap &pixmap, const QString & text, int index)
{
  int row = index;
  QListWidgetItem *item;

  item = QListWidget::takeItem(row);
  if(item != NULL) delete item;
  if(pixmap.isNull()) item = new QListWidgetItem(text);
  else                item = new QListWidgetItem(pixmap, text);
  QListWidget::insertItem(row,item);
}

void MyListBox::removeItem(int index)
{
  int row = index;
  QListWidgetItem *item;

  item = QListWidget::item(row);
  if(item != NULL) delete item;
}

void MyListBox::removeItemByName(QString name)
{
  QList<QListWidgetItem *> list;

  QString txt = "*";
  list = findItems(txt,Qt::MatchWildcard);
  for(int i=0; i < list.size(); i++)
  {
    QListWidgetItem *item = list.at(i);
    if(item != NULL)
    {
      if(item->text() == name)
      {
        delete item;
        return;
      }
    }
  }
}

void MyListBox::clear()
{
  QListWidget::clear();
}

void MyListBox::slotClicked(QListWidgetItem *item)
{
char buf[80];

  if(item == NULL) return;
  sprintf(buf,"QListBox(%d,\"%s\")\n",id,decode(item->text()));
  tcp_send(s,buf,strlen(buf));
}

void MyListBox::slotCurrentChanged(QListWidgetItem *item, QListWidgetItem *previous)
{
char buf[80];

  if(item == NULL) return;
  sprintf(buf,"QListBox(%d,\"%s\")\n",id,decode(item->text()));
  tcp_send(s,buf,strlen(buf));
  if(previous == NULL) return;
}

void MyListBox::slotSendSelected()
{
char buf[MAX_PRINTF_LENGTH];
int i,cnt;

  cnt = count();
  for(i=0; i<cnt; i++)
  {
    QListWidgetItem *item = QListWidget::item(i);
    //rlmurx-was-here if(isItemSelected(item))
    if(item->isSelected())
    {
      sprintf(buf,"selected(%d,%d,\"%s\")\n",id,i,decode(item->text()));
      tcp_send(s,buf,strlen(buf));
    }
  }
  sprintf(buf,"selected(%d,-1,\"(null)\")\n",id);
  tcp_send(s,buf,strlen(buf));
}

void MyListBox::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QListWidget::mousePressEvent(event);
}

void MyListBox::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QListWidget::mouseReleaseEvent(event);
}

void MyListBox::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QListWidget::enterEvent(event);
}

void MyListBox::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QListWidget::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyTable::MyTable(int *sock, int ident, int numRows, int numColumns, QWidget *parent, const char *name)
        :QTableWidget(numRows,numColumns,parent)
{
  s = sock;
  id = ident;
  wrap = 1;
  autoresize = 0;
  is_editable = 1;
#if QT_VERSION >= 0x040300                  
  setWordWrap(true);
#endif
  button = read_only = updates = 0;
  if(name != NULL) setObjectName(name);
#ifdef USE_MAEMO  
  QPalette palette;
  QColor   color(128,128,128);
  palette.setColor(QPalette::Text, color);
  //palette.setColor(QPalette::WindowText, color);
  //palette.setColor(QPalette::ButtonText, color);
  setPalette(palette);
#endif
  connect(horizontalHeader(), SIGNAL(sectionClicked(int)), SLOT(slotColClicked(int)));
  connect(verticalHeader()  , SIGNAL(sectionClicked(int)), SLOT(slotRowClicked(int)));
  connect(this, SIGNAL(cellClicked(int,int)), SLOT(slotClicked(int,int)));
  connect(this, SIGNAL(currentCellChanged(int,int,int,int)), SLOT(slotCurrentChanged(int,int,int,int)));
  connect(this, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)));
  connect(this, SIGNAL(activated(QModelIndex)), SLOT(slotActivated(QModelIndex)));
}

MyTable::~MyTable()
{
}

void MyTable::setEditable(int editable)
{
  if(editable) is_editable = 1;
  else         is_editable = 0;
  for(int row=0; row<rowCount(); row++)
  {
    for(int column=0; column<columnCount(); column++)
    {
      QTableWidgetItem *tableitem = item(row,column);
      if(tableitem != NULL)
      {
        if(editable == 1) tableitem->setFlags(Qt::ItemIsEditable);
        else              tableitem->setFlags(Qt::ItemIsEnabled);
      }
    }
  }
}

void MyTable::setTableButton(int row, int col, QString text)
{
  int r,g,b;
  r = g = b = -2;
  if(text.startsWith("color("))
  {
    sscanf(text.toUtf8(),"color(%d,%d,%d",&r,&g,&b);
    text = text.section(')',1);
  }
  MyQPushButton *button = new MyQPushButton(s,id,0);
  button->setText(text);
  button->row = row;
  button->col = col;
  setCellWidget(row,col,button);
  if(r!=-2 && g!=-2 && b!=-2) mySetBackgroundColor(button,TQPushButton,r,g,b);
}

void MyTable::setTableCheckBox(int row, int col, int state, QString text)
{
  int r,g,b;
  r = g = b = -2;
  if(text.startsWith("color("))
  {
    sscanf(text.toUtf8(),"color(%d,%d,%d",&r,&g,&b);
    text = text.section(')',1);
  }
  MyCheckBox *check = new MyCheckBox(s,id,0);
  check->setText(text);
  if(state) check->setCheckState(Qt::Checked);
  else      check->setCheckState(Qt::Unchecked);
  check->row = row;
  check->col = col;
  setCellWidget(row,col,check);
  if(r!=-2 && g!=-2 && b!=-2) mySetBackgroundColor(check,TQCheck,r,g,b);
}

void MyTable::setTableComboBox(int row, int col, int editable, const char *menu)
{
  char buf[800];
  int  i,ifirst,commaCnt,currentIndex;
  QStringList list;
  ifirst = commaCnt = currentIndex = 0;
  for(i=0;; i++)
  {
    if(menu[i] == ',') 
    {
      commaCnt++;
      if(menu[i+1] == '#') currentIndex = commaCnt;
    }
    if(menu[i] == ',' || menu[i] == '\0')
    {
      if(i == ifirst && i == 0)
      {
      }
      else if(menu[i] != '\0' && menu[i+1] == ',')
      {
        if(i > 0 && menu[i-1] != ',')
        {
          strncpy(buf,&menu[ifirst],i-ifirst);
          buf[i-ifirst] = '\0';
          if(buf[0] == '#')
          {
            list.append(&buf[1]);
          }
          else
          {
            list.append(buf);
          }
        }
      }
      else
      {
        if(i > 0 && menu[i-1] != ',')
        {
          strncpy(buf,&menu[ifirst],i-ifirst);
          buf[i-ifirst] = '\0';
          if(buf[0] == '#')
          {
            list.append(&buf[1]);
          }
          else
          {
            list.append(buf);
          }
        }
      }
      ifirst = i+1;
    }
    if(menu[i] == '\0') break;
  }

  MyComboBox *combo = new MyComboBox(s,id,0);
  combo->addItems(list);
  combo->setEditable(editable);
  combo->row = row;
  combo->col = col;
  if(currentIndex > 0)
  {
    combo->setCurrentIndex(currentIndex);
  }
  setCellWidget(row,col,combo); 
  if(opt.arg_debug) printf("setCellWidget combo\n");
  //printf("ComboBox(%d,%d,%d,%s)\n",row,col,editable,text);
}

void MyTable::setTableLabel(int row, int col, QString text)
{
  MyLabel *label = new MyLabel(s,id,0);
  label->setText(text);
  label->row = row;
  label->col = col;
  setCellWidget(row,col,label);
}

void MyTable::slotRowClicked(int section)
{
char buf[80];

  sprintf(buf,"QTable(%d,%d,%d,%d)\n",id,section,-1,1);
  tcp_send(s,buf,strlen(buf));
}

void MyTable::slotColClicked(int section)
{
char buf[80];

  sprintf(buf,"QTable(%d,%d,%d,%d)\n",id,-1,section,1);
  tcp_send(s,buf,strlen(buf));
}

void MyTable::slotActivated(QModelIndex index)
{
  if(opt.arg_debug) printf("MyTable::slotActivated()\n");
  if(is_editable) return;
  QTableWidgetItem *tableitem = itemFromIndex(index);
  if(tableitem == NULL)
  {
    tableitem = new QTableWidgetItem();
    tableitem->setText("");
    if(is_editable == 0) tableitem->setFlags(Qt::ItemIsEnabled); // remove editable flag
    setItem(currentRow(),currentColumn(),tableitem);
    if(opt.arg_debug) printf("setTableItem\n");
  }
}

void MyTable::mousePressEvent(QMouseEvent *event)
{
  //char buf[80];

  updates = 0;
  if     (event->button() == Qt::LeftButton)  button = 1;
  else if(event->button() == Qt::MidButton)   button = 2;
  else if(event->button() == Qt::RightButton) button = 3;
  else                                        button = 0;
  //if(button == 3)
  //{
  //  sprintf(buf,"QMouseRight(%d)\n",id);
  //  tcp_send(s,buf,strlen(buf));
  //}

  if(button == 3)
  {
    QMenu popupMenu;
    QAction *ret;
    QString buf;

    popupMenu.addAction(l_copy);
    popupMenu.addAction(l_copy_plus_title);
    popupMenu.addAction("Save table as CSV file");
    if(opt.view_csv[0] != '\0')
    {
      //rlmurx-was-here buf.sprintf("Open table with %s", opt.view_csv);
      buf = QString::asprintf("Open table with %s", opt.view_csv);
      popupMenu.addAction(buf);
    }  
    ret = popupMenu.exec(QCursor::pos());
    if(ret != NULL)
    {
      if(ret->text().startsWith(l_copy_plus_title)) 
      {
        copyToClipboard(1);
      }  
      else if(ret->text().startsWith(l_copy)) 
      {
        copyToClipboard(0);
      }  
      else if(ret->text().startsWith("Save")) 
      {
        saveTextfile();
      }  
      else
      {
        char buf[MAX_PRINTF_LENGTH];
        saveTextfile("table.csv");
        if(strlen(opt.view_csv) < 900)
        {
          strcpy(buf, opt.view_csv);
          strcat(buf, " table.csv");
//#ifndef PVWIN32
//          strcat(buf, " &");
//#endif
          if(strlen(opt.view_csv) >= 3) mysystem(buf);
        }  
      }  
    }
  }  
  QTableWidget::mousePressEvent(event);
}

//void MyTable::slotClicked(int row, int col, int button)
void MyTable::slotClicked(int row, int col)
{
char buf[80];

  sprintf(buf,"QTable(%d,%d,%d,%d)\n",id,row,col,button);
  tcp_send(s,buf,strlen(buf));
}

void MyTable::slotCurrentChanged( int row, int col, int oldrow, int oldcol)
{
char buf[80];

  sprintf(buf,"QTable(%d,%d,%d,0)\n",id,row,col);
  tcp_send(s,buf,strlen(buf));
  if(oldrow == oldcol) return;
}

void MyTable::slotValueChanged(int row, int col)
{
  char buf[MAX_PRINTF_LENGTH];
  const char *cptr;

  if(opt.echo_table_updates == 0)
  {
    if(updates > 0)
    {
      if(opt.arg_debug) printf("MyTable::slotValueChanged: updates=%d\n", updates);
      updates--;
      return;
    }
  }  
  updates = 0;
  if(opt.arg_debug) printf("MyTable::slotValueChanged\n");
  QString txt;
  QString celltext = item(row,col)->text();
  if(celltext.startsWith("color("))
  {
    txt = celltext.mid(1+celltext.indexOf(')'));
  }
  else
  {
    txt = item(row,col)->text();
  }
  cptr = decode(txt);
  if(strlen(cptr) > MAX_PRINTF_LENGTH-40) return;
  sprintf(buf,"QTableValue(%d,%d,%d,\"%s\")\n",id,row,col,cptr);
  tcp_send(s,buf,strlen(buf));
}

void MyTable::copyToClipboard(int title)
{
  int x,y;
  QString text,cell;

  if(title)
  {
    cell = "";
    for(x=0; x<columnCount(); x++)
    {
      if(horizontalHeaderItem(x) == NULL) cell = "";
      else                                cell = horizontalHeaderItem(x)->text();
      text += "\t";
      text += cell;
    }
    text += "\n";
  }

  for(y=0; y<rowCount(); y++)
  {
    if(title)
    {
      if(verticalHeaderItem(y) == NULL)   cell = "";
      else                                cell = verticalHeaderItem(y)->text();
      if(cell.isEmpty()) cell = "";
      text += cell;
      text += "\t";
    }
    for(x=0; x<columnCount(); x++)
    {
      if(item(y,x) == NULL) cell = "";
      else                  cell = item(y,x)->text();
      if(cell.isEmpty())    cell = "";
      text += cell;
      if(x < columnCount() - 1) text += "\t";
    }
    text += "\n";
  }

  QApplication::clipboard()->setText(text);
}

void MyTable::saveTextfile(const char *filename)
{
  QFileDialog dlg;
  QString name,cell;
  FILE *fp;
  int x,y,ret;

  if(filename == NULL)
  {
    //rlmurx-was-here name = dlg.getSaveFileName(NULL,QString::null,opt.temp,"*.csv");
    name = dlg.getSaveFileName(NULL,QString(),opt.temp,"*.csv");
  }
  else
  {
    name = filename;
  }
  if(name.isEmpty()) return;
  if(filename == NULL)
  {
    fp = fopen(name.toUtf8(),"r");
    if(fp != NULL)
    {
      fclose(fp);
      ret = QMessageBox::warning(this,"Save Table","File already exists: Overwrite ?",QMessageBox::Yes,QMessageBox::No,0);
      if(ret == QMessageBox::No) return;
    }
  }  
  fp = fopen(name.toUtf8(),"w");
  if(fp == NULL)
  {
    QMessageBox::warning(this,"Save Table","could not write file",QMessageBox::Ok,0,0);
    return;
  }
  for(y=-1; y<rowCount(); y++)
  {
    if(y == -1)                              cell = "";
    else if(verticalHeaderItem(y) == NULL)   cell = "";
    else                                     cell = verticalHeaderItem(y)->text();
    if(cell.isEmpty())                       cell = "";
    else                                     fprintf(fp,"%s\t",(const char *) cell.toUtf8());
    for(x=0; x<columnCount(); x++)
    {
      if(y == -1)
      {
        if(horizontalHeaderItem(x) == NULL) cell = "";
        else                                cell = horizontalHeaderItem(x)->text();
      }
      else if(item(y,x) == NULL) cell = "";
      else                       cell = item(y,x)->text();
      if(cell.isEmpty())         cell = "";
      if(x == 0) fprintf(fp,"%s"  ,(const char *) cell.toUtf8());
      else       fprintf(fp,"\t%s",(const char *) cell.toUtf8());
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
}

void MyTable::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTableWidget::enterEvent(event);
}

void MyTable::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTableWidget::leaveEvent(event);
}

void MyTable::clear()
{
  for(int y=0; y<rowCount(); y++)
  {
    for(int x=0; x<columnCount(); x++)
    {
      QTableWidgetItem *tableitem = item(y,x);
      if(tableitem == NULL)
      {
        tableitem = new QTableWidgetItem();
        tableitem->setText("");
        setItem(y,x,tableitem);
      }
      else
      {
        tableitem->setText("");
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
MySpinBox::MySpinBox(int *sock, int ident, int minValue, int maxValue, int step, QWidget *parent, const char *name)
          :QSpinBox(parent)
{
  s = sock;
  id = ident;
  setMinimum(minValue);
  setMaximum(maxValue);
  setSingleStep(step);
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
}

MySpinBox::~MySpinBox()
{
}

void MySpinBox::slotValueChanged(int value)
{
char buf[80];

  sprintf(buf,"slider(%d,%d)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MySpinBox::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QSpinBox::mousePressEvent(event);
}

void MySpinBox::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QSpinBox::mouseReleaseEvent(event);
}

void MySpinBox::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QSpinBox::enterEvent(event);
}

void MySpinBox::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QSpinBox::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyDial::MyDial(int *sock, int ident, int minValue, int maxValue, int pageStep, int value, QWidget *parent, const char *name)
       :QDial(parent)
{
  s = sock;
  id = ident;
  setMaximum(maxValue);
  setMinimum(minValue);
  setPageStep(pageStep);
  setValue(value);
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(valueChanged(int)), SLOT(slotValueChanged(int)));
}

MyDial::~MyDial()
{
}

void MyDial::setValue(int value)
{
  QDial::setValue(value);
}

void MyDial::slotValueChanged(int value)
{
char buf[80];

  sprintf(buf,"slider(%d,%d)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyDial::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QDial::mousePressEvent(event);
}

void MyDial::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QDial::mouseReleaseEvent(event);
}

void MyDial::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QDial::enterEvent(event);
}

void MyDial::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QDial::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyProgressBar::MyProgressBar(int *sock, int ident, int totalSteps, Qt::Orientation ori, QWidget *parent, const char *name)
              :QProgressBar(parent)
{
  s = sock;
  id = ident;
  setOrientation(ori);
  setMaximum(totalSteps);
  setObjectName(name);
}

MyProgressBar::~MyProgressBar()
{
}

void MyProgressBar::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QProgressBar::mousePressEvent(event);
}

void MyProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QProgressBar::mouseReleaseEvent(event);
}

void MyProgressBar::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QProgressBar::enterEvent(event);
}

void MyProgressBar::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QProgressBar::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyMultiLineEdit::MyMultiLineEdit(int *sock, int ident, int editable, int maxLines, QWidget *parent, const char *name)
                :QTextEdit(parent)
{
  s = sock;
  id = ident;
  maxlines = maxLines;
  num_lines = 0;
  if(name != NULL) setObjectName(name);
  if     (editable == 0) setReadOnly(true);
  else if(editable == 1) setReadOnly(false);
  setLineWrapMode(QTextEdit::NoWrap);
}

MyMultiLineEdit::~MyMultiLineEdit()
{
}

void MyMultiLineEdit::setText(const QString &text)
{
  /* trollmurx
  if(maxlines != -1)
  {
    if(lines() >= maxlines)
    {
      removeParagraph(0);
    }
  }
  setCursorPosition(lines()-1,paragraphLength(lines()-1));
  */
  if(maxlines != -1)
  {
    if(num_lines >= maxlines)
    {
      QFont  font  = currentFont();
      QColor color = textColor();
      QTextCursor cursor = textCursor();
      cursor.movePosition(QTextCursor::Start);
      cursor.clearSelection();
      cursor.select(QTextCursor::BlockUnderCursor);
      cursor.removeSelectedText();
      cursor.deleteChar();
      cursor.movePosition(QTextCursor::End);
      setCurrentFont(font);
      setTextColor(color);
      num_lines--;
    }
  }
  append(text);
  num_lines++;
  textCursor().movePosition(QTextCursor::End);
}

void MyMultiLineEdit::slotSendToClipboard()
{
  char buf[80];

  QString txt = document()->toPlainText();
  int len = strlen(txt.toUtf8());
  //char text[len+1]; // MSVC can't do this
  char *text = new char[len+1];
  strcpy(text,txt.toUtf8());
  sprintf(buf,"@clipboard(%d,%d)\n", id,len);
  tcp_send(s,buf,strlen(buf));
  tcp_send(s,text,len);
  delete [] text;
}

void MyMultiLineEdit::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QTextEdit::mousePressEvent(event);
}

void MyMultiLineEdit::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QTextEdit::mouseReleaseEvent(event);
}

void MyMultiLineEdit::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTextEdit::enterEvent(event);
}

void MyMultiLineEdit::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTextEdit::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
static int starts_with(const char *path1, const char *path2)
{
  int i;
  for(i=0; path1[i] != '\0'; i++)
  {
    if(path2[i] == '\0')
    {
      if(path1[i] == '/') return 1;
      else                return 0;
    }
    if(path1[i] != path2[i]) return 0;
  }
  if(path2[i] != '/' && path2[i] != '\0') return 0; // RL 15.10.2004
  return 1;
}

const char *root_path(const char *path, int num_slash)
{
  static char buf[MAX_PRINTF_LENGTH];
  int i,cnt;

  i   = 0;
  cnt = -1;
  while(i < (int) sizeof(buf))
  {
    if(path[i] == '/') cnt++;
    if(cnt == num_slash) break;
    if(path[i] == '\0')  break;
    buf[i] = path[i];
    i++;
  }
  buf[i] = '\0';
  return buf;
}

MyListView::MyListView(int *sock, int ident, QWidget *parent, const char *name)
           :QTreeWidget(parent)
{
  s = sock;
  id = ident;
  recursion = icol = 0;
  hasStandardPopupMenu = 1;
  if(name != NULL) setObjectName(name);
  setSortingEnabled(false);
  connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(slotClicked(QTreeWidgetItem *, int)));
  connect(this, SIGNAL(itemSelectionChanged()), SLOT(slotSendSelected()));
  headerItem()->setHidden(false);
  for(int i=0; i<20; i++) colwidth[i] = 100;

  header()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(header(), SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotCustomContextMenuRequested(const QPoint &)));
}

MyListView::~MyListView()
{
}

void MyListView::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::RightButton)
  {
    if(opt.arg_debug) printf("rightButtonPressed\n");
    //QPoint parent0 = mapToGlobal(QPoint(0,0));
    //int x = event->x() - parent0.x();
    //int y = event->y() - parent0.y();
    //MyListViewItem *item = (MyListViewItem *) childAt(x,y);
    slotRightButtonPressed(NULL, currentColumn());
  }
  else
  {
    char buf[80];
    sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
    tcp_send(s,buf,strlen(buf));
  }
  QTreeWidget::mousePressEvent(event);
}

void MyListView::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QTreeWidget::mouseReleaseEvent(event);
}

void MyListView::addColumn(QString text, int size)
{
  if(opt.arg_debug) printf("addColumn(%s) icol=%d\n",(const char *) text.toUtf8(), icol);
  setColumnCount(icol+1);
  headerItem()->setText(icol,text);
  if(size > 0)
  {
    if(opt.arg_debug) printf("addColumn: icol=%d size=%d currentColumn=%d columnCount=%d\n", icol, size, currentColumn(), columnCount());
    //QSize qsize;
    //qsize.setWidth(size);
    //qsize.setHeight(20);
    //headerItem()->setSizeHint(icol,qsize);
    if(icol >= 0 && icol < 20)
    {
      colwidth[icol] = size;
      for(int i=0; i<columnCount(); i++)
      {
        setColumnWidth(i, colwidth[i]);
      }
    }  
  }
  setColumnHidden(icol,false);
  icol++;
}

void MyListView::removeAllColumns()
{
  if(opt.arg_debug) printf("removeAllColumns\n");
  setColumnCount(1);
  icol = 0;
}

void MyListView::setMultiSelection(int mode)
{
  // mode := 0=SingleSelection|1=MultiSelection|2=NoSelection
  if(mode == 0)
  {
    setSelectionMode(QAbstractItemView::SingleSelection);
  }
  else if(mode == 1)
  {
    setSelectionMode(QAbstractItemView::MultiSelection);
  }
  else
  {
    setSelectionMode(QAbstractItemView::NoSelection);
  }
}

void MyListView::setSorting(int col, int mode)
{
  //Sort column
  //mode=0 decending
  //mode=1 ascendin
  //column=-1 do not allow sorting (this is the default)
  //Allowed widgets QListView
  if(col < 0)
  {
    setSortingEnabled(false);
  }
  else
  {
    setSortingEnabled(true);
    if(mode == 0)
    {
      sortItems(col,Qt::DescendingOrder);
    }
    else
    {
      sortItems(col,Qt::AscendingOrder);
    }
  }
}

MyListViewItem *MyListView::firstChild(MyListViewItem *parent)
{
  MyListViewItem *item;
  if(recursion >= MAX_TREE_RECURSION) return NULL;
  ichild[recursion] = 0;
  if(parent == NULL) // child of ListView
  {
    item = (MyListViewItem *) topLevelItem(ichild[recursion]++);
  }
  else               // child of ListViewItem
  {
    item = NULL;
    if(parent->childCount() > 0) item = (MyListViewItem *) parent->child(ichild[recursion]++);
  }
  return item;
}

MyListViewItem *MyListView::nextSibling(MyListViewItem *sibling, QTreeWidgetItem *parent)
{
  MyListViewItem *item = NULL;
  if(sibling == NULL) return NULL;
  if(parent == NULL) // child of ListView
  {
    if(topLevelItemCount() >= ichild[recursion]) item = (MyListViewItem *) topLevelItem(ichild[recursion]++);
  }
  else                                   // child of ListViewItem
  {
    if(parent->childCount() >= ichild[recursion]) item = (MyListViewItem *) parent->child(ichild[recursion]++);
  }
  return item;
}

void MyListView::insertItem(MyListViewItem *item, MyListViewItem *parent, int num_slash)
{
  if(num_slash == 1) // child of ListView
  {
    addTopLevelItem(item);
  }
  else               // child of ListViewItem
  {
    parent->addChild(item);
  }
}

void MyListView::nameVersionSetListViewText(const char *path, int column, QString &text, MyListViewItem *parent, const char *relpath, int num_slash)
{
  MyListViewItem *item;
  const char *cptr;

  item = firstChild(parent);
  while(item != NULL)
  {
    if(strcmp(path,item->path.toUtf8()) == 0) // update existing item
    { 
      if(text.startsWith("color("))
      {
        int r,g,b;
        sscanf(text.toUtf8(),"color(%d,%d,%d",&r,&g,&b);
        text = text.section(')',1);
        item->setBackground(column,QBrush(QColor(r,g,b)));
      }
      item->setText(column,text); 
      return; 
    }     
    if(starts_with(path,item->path.toUtf8()))
    {
      cptr = strchr(&relpath[1],'/');
      if(cptr == NULL) return;
      recursion++;
      nameVersionSetListViewText(path,column,text,item,cptr,num_slash+1); // recurse
      recursion--;
      return;
    }
    item = nextSibling(item,parent);
  }
  if(num_slash == 1)   item = new MyListViewItem((MyListView *) NULL); // add root path
  else                 item = new MyListViewItem((MyListViewItem *) NULL); // add root path
  item->path = root_path(path,num_slash);
  insertItem(item,parent,num_slash);
  if(item->path == path)
  {
    if(text.startsWith("color("))
    {
      int r,g,b;
      sscanf(text.toUtf8(),"color(%d,%d,%d",&r,&g,&b);
      text = text.section(')',1);
      item->setBackground(column,QBrush(QColor(r,g,b)));
    }
    item->setText(column,text); 
    return; 
  }
  cptr = strchr(&relpath[1],'/');
  if(cptr == NULL) return;
  recursion++;
  nameVersionSetListViewText(path,column,text,item,cptr,num_slash+1); // recurse
  recursion--;
  return;
}

void MyListView::nameVersionSetListViewPixmap(const char *path, int column, QPixmap &pixmap, MyListViewItem *parent, const char *relpath, int num_slash)
{
  MyListViewItem *item;
  const char *cptr;

  item = firstChild(parent);
  while(item != NULL)
  {
    if(strcmp(path,item->path.toUtf8()) == 0) { item->setIcon(column,pixmap); return; }  // update existing item
    if(starts_with(path,item->path.toUtf8()))
    {
      cptr = strchr(&relpath[1],'/');
      if(cptr == NULL) return;
      recursion++;
      nameVersionSetListViewPixmap(path,column,pixmap,item,cptr,num_slash+1);     // recurse
      recursion--;
      return;
    }
    item = nextSibling(item,parent);
  }
  if(num_slash == 1)   item = new MyListViewItem((MyListView *)NULL);     // add root path
  else                 item = new MyListViewItem((MyListViewItem *)NULL);   // add root path
  item->path = root_path(path,num_slash);
  insertItem(item,parent,num_slash);
  if(item->path == path) { item->setIcon(column,pixmap); return; }
  cptr = strchr(&relpath[1],'/');
  if(cptr == NULL) return;
  recursion++;
  nameVersionSetListViewPixmap(path,column,pixmap,item,cptr,num_slash+1);         // recurse
  recursion--;
  return;
}

void MyListView::setListViewText(const char *path, int column, QString &text)
{
  if(path[0] != '/') return;
  //triggerUpdate(); //rlehrig not necessary ?
  recursion = 0;
  nameVersionSetListViewText(path,column,text,NULL,path,1);
}

void MyListView::setListViewPixmap(const char *path, int column, QPixmap &pixmap)
{
  if(path[0] != '/') return;
  //triggerUpdate(); //rlehrig not necessary ?
  recursion = 0;
  nameVersionSetListViewPixmap(path,column,pixmap,NULL,path,1);
}

int MyListView::deleteListViewItem(const char *path, MyListViewItem *item)
{
  MyListViewItem *child;
  int ret;

  while(item != NULL)
  {
    if(strcmp(item->path.toUtf8(),path) == 0)
    {
      delete item;
      return 1;
    }
    recursion++;
    child = firstChild(item);
    if(child != NULL)
    {
      ret = deleteListViewItem(path,child);
      if(ret != 0)
      {
        recursion--;
        return ret;
      }
    }
    recursion--;
    item = nextSibling(item,item->parent());
  }
  return 0;
}

int MyListView::ensureVisible(const char *path, MyListViewItem *item)
{
  MyListViewItem *child;
  int ret;

  while(item != NULL)
  {
    if(strcmp(item->path.toUtf8(),path) == 0)
    {
      scrollToItem(item, QAbstractItemView::EnsureVisible);
      return 1;
    }
    recursion++;
    child = firstChild(item);
    if(child != NULL)
    {
      ret = ensureVisible(path,child);
      if(ret != 0)
      {
        recursion--;
        return ret;
      }
    }
    recursion--;
    item = nextSibling(item,item->parent());
  }
  return 0;
}

int MyListView::setItemOpen(const char *path, int open, MyListViewItem *item)
{
  MyListViewItem *child;
  int ret;

  while(item != NULL)
  {
    if(strcmp(item->path.toUtf8(),path) == 0)
    {
      //rlmurx-was-here setItemExpanded(item, (bool) open);
      item->setExpanded((bool) open);
      return 1;
    }
    recursion++;
    child = firstChild(item);
    if(child != NULL)
    {
      ret = setItemOpen(path,open,child);
      if(ret != 0)
      {
        recursion--;
        return ret;
      }
    }
    recursion--;
    item = nextSibling(item,item->parent());
  }
  return 0;
}

void MyListView::closeTree(MyListViewItem *lvi, int mode)
{
  recursion++;
  if(lvi)
  {
    //rlmurx-was-here if(mode==2)  setItemExpanded(lvi, false);
    if(mode==2)  lvi->setExpanded(false);
    //rlmurx-was-here setItemSelected(lvi,false);
    lvi->setSelected(false);
    //rlehrig not necessary ? lvi->repaint();
    closeTree(firstChild(lvi), mode);
    closeTree(nextSibling(lvi,lvi->parent()), mode);
  }
  recursion--;
  return;
}

void MyListView::setSelected(int mode, const char *path)
{
  const char *ptr;

  ptr = &path[1];
  MyListViewItem *plvi;
  MyListViewItem *lvi = firstChild(NULL);

  plvi=lvi;
  closeTree(plvi, mode);

  while((ptr=strchr(ptr, '/')))
  {
    do
    {
      if((!strncmp((const char *) plvi->path.toUtf8(), path, ptr-(const char *)path)) &&
          (strlen((const char *) plvi->path.toUtf8())==(unsigned)(ptr-(const char *)path)))
      {
        //rlmurx-was-here setItemExpanded(plvi, (bool) mode);
        plvi->setExpanded((bool) mode);
        break;
      }
      plvi = nextSibling(plvi,plvi->parent());
    }
    while(plvi);
    if(!plvi) break;
    plvi = firstChild(plvi);
    ptr++;
  }

  while(plvi)
  {
    if(!strcmp((const char *) plvi->path.toUtf8(), path))
    {
      //rlmurx-was-here setItemExpanded(plvi, (bool) mode); //plvi->setOpen(mode);
      plvi->setExpanded((bool) mode); //plvi->setOpen(mode);
      //rlmurx-was-here setItemSelected(plvi, (bool) mode);
      plvi->setSelected((bool) mode);
      doSendSelected(plvi);
      break;
    }
    plvi = nextSibling(plvi,plvi->parent());
  }

  //rllehrig not necessary ? repaint();
}

void MyListView::standardPopupMenu()
{
  QMenu m;
  QAction *act;
  int col;

  for(col=0; col<headerItem()->columnCount(); col++)
  {
    act = m.addAction(headerItem()->text(col));
    act->setCheckable(true);
    if(isColumnHidden(col)) act->setChecked(false);
    else                    act->setChecked(true);
    if(col+1 == hasStandardPopupMenu) act->setEnabled(false);
  }
  act = m.exec(QCursor::pos());
  if(act != NULL)
  {
    QString txt = act->text();
    for(col=0; col<headerItem()->columnCount(); col++)
    {
      if(txt == headerItem()->text(col))
      {
        if(act->isChecked()) setColumnHidden(col,false);
        else                 setColumnHidden(col,true);
        return;
      }
    }
  }
}

void MyListView::slotCustomContextMenuRequested(const QPoint &pos)
{
  char buf[MAX_PRINTF_LENGTH];
  int col = header()->logicalIndexAt(pos);

  if(opt.arg_debug) printf("slotCustomContextMenuRequested(%d)\n", col);

  if(hasStandardPopupMenu)
  {
    standardPopupMenu();
  }   
  else
  {
    sprintf(buf,"selected(%d,%d,\"%s\")\n", id, col, "headerContextMenuRequested");
    tcp_send(s,buf,strlen(buf));
  }  
}

void MyListView::slotClicked(QTreeWidgetItem *item, int column)
{
  char buf[MAX_PRINTF_LENGTH];
  MyListViewItem *myitem = (MyListViewItem *) item;

  if(item == NULL) return;
  if(opt.arg_debug) printf("clicked\n");
  if(opt.arg_debug) printf("path=%s\n",(const char *) myitem->path.toUtf8());
  int col = icol; //columnCount();
  while(col > 0)
  {
    col--;
    //sprintf(buf,"text(%d,\"%s\")\n", id, (const char *) item->text(col));
    sprintf(buf,"selected(%d,%d,\"%s\")\n", id, col,decode(myitem->text(col)));
    tcp_send(s,buf,strlen(buf));
  }
  sprintf(buf,"selected(%d,-1,\"%s\")\n", id, decode(myitem->path));
  tcp_send(s,buf,strlen(buf));
  if(column == -1000) return;
}

void MyListView::slotSendSelected()
{
char buf[MAX_PRINTF_LENGTH];

  recursion = 0;
  doSendSelected(firstChild(NULL));
  sprintf(buf,"selected(%d,-2,\"(null)\")\n",id);
  tcp_send(s,buf,strlen(buf));
}

void MyListView::doSendSelected(MyListViewItem *item)
{
char buf[MAX_PRINTF_LENGTH];
int column;

  while(item != NULL)
  {
    //rlmurx-was-here if(isItemSelected(item))
    if(item->isSelected())
    {
      column = icol; //columnCount();
      while(column > 0)
      {
        column--;
        if(opt.arg_debug) printf("doSendSelected column=%d text=%s\n",column,decode(item->text(column)));
        sprintf(buf,"selected(%d,%d,\"%s\")\n", id, column, decode(item->text(column)));
        tcp_send(s,buf,strlen(buf));
      }
      MyListViewItem *myitem = (MyListViewItem *) item;
      sprintf(buf,"selected(%d,-1,\"%s\")\n", id, decode(myitem->path));
      if(opt.arg_debug) printf("doSendSelected path=%s\n",decode(myitem->path));
      tcp_send(s,buf,strlen(buf));
    }
    recursion++;
    if(firstChild(item) != NULL)
    {
      doSendSelected(firstChild(item));
    }
    recursion--;
    item = nextSibling(item,item->parent());
  }
}

void MyListView::slotRightButtonPressed(QTreeWidgetItem *item, int column)
{
  char buf[MAX_PRINTF_LENGTH];

  MyListViewItem *myitem = (MyListViewItem *) item;
  if(item == NULL)
  {
    sprintf(buf,"QMouseRight(%d,\"%d,\")\n",id,column);
  }
  else
  {
    sprintf(buf,"QMouseRight(%d,\"%d,%s\")\n",id,column, decode(myitem->path));
  }
  tcp_send(s,buf,strlen(buf));
}

void MyListView::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTreeWidget::enterEvent(event);
}

void MyListView::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTreeWidget::leaveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
MyIconView::MyIconView(int *sock, int ident, QWidget *parent, const char *name)
           :QListWidget(parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  QListView::setViewMode(QListView::IconMode);
  setDragDropMode(QAbstractItemView::NoDragDrop);
  connect(this, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotClicked(QListWidgetItem *)));
}

MyIconView::~MyIconView()
{
}

void MyIconView::setIconViewItem(QString &text, QPixmap &pixmap)
{
  QListWidgetItem *item;

  if(pixmap.isNull()) item = new QListWidgetItem(text);
  else                item = new QListWidgetItem(pixmap, text);
  QListWidget::addItem(item);
}

void MyIconView::deleteIconViewItem(QString &text)
{
  QList<QListWidgetItem *> list;

  if(text.isEmpty()) return;
  QString txt = "*";
  list = findItems(txt,Qt::MatchWildcard);
  for(int i=0; i < list.size(); i++)
  {
    QListWidgetItem *item = list.at(i);
    if(item != NULL)
    {
      if(item->text() == text)
      {
        delete item;
        return;
      }
    }
  }
}

void MyIconView::slotClicked(QListWidgetItem *item)
{
char buf[MAX_PRINTF_LENGTH];

  if(item == NULL) return;
  if(item->text().length()+40 > MAX_PRINTF_LENGTH) return;
  sprintf(buf,"text(%d,\"%s\")\n", id, decode(item->text()));
  tcp_send(s,buf,strlen(buf));
}

void MyIconView::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QListWidget::mousePressEvent(event);
}

void MyIconView::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QListWidget::mouseReleaseEvent(event);
}

void MyIconView::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QListWidget::enterEvent(event);
}

void MyIconView::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QListWidget::leaveEvent(event);
}

#ifndef NO_QWT
// --- QWT --------------------------------------------------------------
MyQwtScale::MyQwtScale(int *sock, int ident, int position, QWidget *parent, const char *name)
           :QwtScaleWidget((QwtScaleDraw::Alignment)position,parent)
{
  s = sock;
  id = ident;
  if(position == 0) setAlignment((QwtScaleDraw::Alignment) Qt::Horizontal);
  else              setAlignment((QwtScaleDraw::Alignment) Qt::Vertical);
  if(name != NULL)  setObjectName(name);
}

MyQwtScale::~MyQwtScale()
{
}

void MyQwtScale::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtScaleWidget::mousePressEvent(event);
}

void MyQwtScale::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtScaleWidget::mouseReleaseEvent(event);
}

void MyQwtScale::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtScaleWidget::enterEvent(event);
}

void MyQwtScale::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtScaleWidget::leaveEvent(event);
}

MyQwtThermo::MyQwtThermo(int *sock, int ident, QWidget *parent, const char *name)
            :QwtThermo(parent)
{
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
}

MyQwtThermo::~MyQwtThermo()
{
}

void MyQwtThermo::setOrientation(Qt::Orientation o) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
#else
  // issue-reported-by-jose
  QwtThermo::setOrientation(o);
#endif
  if(o) return;
}

#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
void MyQwtThermo::setOrientation(Qt::Orientation o, QwtThermo::ScalePos pos) //rlmurx-was-here
{
  printf("in MyQwtThermo::setOrientation(Qt::Orientation o, QwtThermo::ScalePos pos)\n");
  if(o) return;
  if(pos) return;
  //not implementes
}
#endif

void MyQwtThermo::setFillColor(QColor rgb) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtThermo::setFillColor(rgb);
#else
  QwtThermo::setFillBrush(rgb);
#endif
}

void MyQwtThermo::setAlarmColor(QColor rgb) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtThermo::setAlarmColor(rgb);
#else
  QwtThermo::setAlarmBrush(rgb);
#endif
}

void MyQwtThermo::setMargin(int m) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtThermo::setMargin(m);
#else
  setBorderWidth(m);
#endif
}

void MyQwtThermo::setRange(double min, double max, double step) //rlmurx-was-here jjmg was here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtThermo::setRange(min,max,step);
#else
//#warning "not implemented QwtThermo::setRange(min,max,step); (rlmurx)"
    QwtAbstractScale::setScaleStepSize(step); //jjmg was here
    QwtAbstractScale::setScale(min,max); //jjmg was here
  if(min == max) return;
  if(step) return;
#endif
}

void MyQwtThermo::setScale(double vmin, double vmax, double step) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtThermo::setScale(vmin,vmax,step);
#else
  QwtAbstractScale::setScaleStepSize(step); //jjmg was here
  QwtAbstractScale::setScale(vmin,vmax); //jjmg was here
  //QwtThermo::setScale(vmin,vmax,step);
  if(step) return;
#endif
}

void MyQwtThermo::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtThermo::mousePressEvent(event);
}

void MyQwtThermo::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtThermo::mouseReleaseEvent(event);
}

void MyQwtThermo::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtThermo::enterEvent(event);
}

void MyQwtThermo::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtThermo::leaveEvent(event);
}

MyQwtKnob::MyQwtKnob(int *sock, int ident, QWidget *parent, const char *name)
          :QwtKnob(parent)
{
  s = sock;
  id = ident;
  connect(this, SIGNAL(valueChanged(double)), SLOT(slotValueChanged(double)));
  if(name != NULL) setObjectName(name);
}

MyQwtKnob::~MyQwtKnob()
{
}

void MyQwtKnob::setScale(double vmin, double vmax, double step) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtKnob::setScale(vmin,vmax,step);
#else
  // issue-reported-by-jose
  QwtKnob::setScale(vmin,vmax);
  if(step) return;
#endif
}

void MyQwtKnob::setRange(int vmin, int vmax, int step) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtKnob::setRange(vmin,vmax,step);
#else
  double dmin = vmin;
  double dmax = vmax;
  setRange(dmin,dmax,false);
  if(step) return;
#endif
}

void MyQwtKnob::setMass(int w) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtKnob::setMass(w);
#else
  if(w) return;
//only relevant for QwtWheel
//#warning "not implemented: QwtKnob::setMass : Set the slider's mass for flywheel effect. (rlmurx)"
#endif
}

void MyQwtKnob::setOrientation(Qt::Orientation o) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtKnob::setOrientation(o);
#else
  QwtAbstractSlider *objptr = (QwtAbstractSlider *) QwtKnob::QwtAbstractSlider::qt_metacast("orientation");
  objptr->setProperty("orientation",o);
#endif
}

void MyQwtKnob::slotValueChanged(double value)
{
  char buf[80];

  if(opt.arg_debug) printf("KnobValue=%f\n", (float) value);
  sprintf(buf,"slider(%d,%lf)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyQwtKnob::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtKnob::mousePressEvent(event);
}

void MyQwtKnob::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtKnob::mouseReleaseEvent(event);
}

void MyQwtKnob::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtKnob::enterEvent(event);
}

void MyQwtKnob::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtKnob::leaveEvent(event);
}

MyQwtCounter::MyQwtCounter(int *sock, int ident, QWidget *parent, const char *name)
             :QwtCounter(parent)
{
  s = sock;
  id = ident;
  connect(this, SIGNAL(valueChanged(double)), SLOT(slotValueChanged(double)));
  if(name != NULL) setObjectName(name);
}

MyQwtCounter::~MyQwtCounter()
{
}

void MyQwtCounter::slotValueChanged(double value)
{
char buf[80];

  sprintf(buf,"slider(%d,%lf)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyQwtCounter::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtCounter::mousePressEvent(event);
}

void MyQwtCounter::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtCounter::mouseReleaseEvent(event);
}

void MyQwtCounter::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtCounter::enterEvent(event);
}

void MyQwtCounter::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtCounter::leaveEvent(event);
}

MyQwtWheel::MyQwtWheel(int *sock, int ident, QWidget *parent, const char *name)
           :QwtWheel(parent)
{
  s = sock;
  id = ident;
  setRange(0, 100);
  setValue(0.0);
  setMass(0.2);
  setTotalAngle(360.0);
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(valueChanged(double)), SLOT(slotValueChanged(double)));
  // return; //rlmurx-was-here clang issue
}

MyQwtWheel::~MyQwtWheel()
{
}

void MyQwtWheel::setOrientation(Qt::Orientation o) //rlmurx-was-here
{
  if(o) return;
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  //not implementes
#else
  //not implemented
#endif
}

void MyQwtWheel::setReadOnly(int b) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtWheel::setReadOnly(b);
#else
  if(b) return;
//is virtual in qwt
//#warning "not implemented QwtWheel::setReadOnly(b); (rlmurx)"
#endif
}

void MyQwtWheel::setInternalBorder(int width) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtWheel::setInternalBorder(width);
#else
  if(width) return;
//
//#warning "not implemented QwtWheel::setInternalBorder(width); (rlmurx)"
#endif
}

void MyQwtWheel::slotValueChanged(double value)
{
char buf[80];

  sprintf(buf,"slider(%d,%lf)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyQwtWheel::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtWheel::mousePressEvent(event);
}

void MyQwtWheel::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtWheel::mouseReleaseEvent(event);
}

void MyQwtWheel::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtWheel::enterEvent(event);
}

void MyQwtWheel::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtWheel::leaveEvent(event);
}

MyQwtSlider::MyQwtSlider(int *sock, int ident, QWidget *parent, const char *name)
            :QwtSlider(parent)
{
  s = sock;
  id = ident;
  connect(this, SIGNAL(valueChanged(double)), SLOT(slotValueChanged(double)));
  if(name != NULL) setObjectName(name);
}

MyQwtSlider::~MyQwtSlider()
{
}

void MyQwtSlider::setOrientation(Qt::Orientation o) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtSlider::setOrientation(o);
#else
  QwtAbstractSlider *objptr = (QwtAbstractSlider *) QwtDial::QwtAbstractSlider::qt_metacast("orientation");
  objptr->setProperty("orientation",o);
#endif
}

void MyQwtSlider::setMass(int w) //rlmurx-was-here
{
  if(w) return;
//only relevant for QwtWheel
//#warning "not implemented: QwtSlider::setMass : Set the slider's mass for flywheel effect. (rlmurx)"
}

void MyQwtSlider::setScale(int min, int max, int step, int logarithmic) //rlmurx-was-here
{
  if(min == max) return;
  if(step == 1) return;
  if(logarithmic) return;
//#warning "not implemented: QwtSlider::setScale : because it is not necessary (rlmurx)"
}

#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
void MyQwtSlider::setScale(double d1, double d2, double d3, QwtThermo::ScalePos pos)
{
  if(d1 < d2) return;
  if(d1 < d3) return;
  if(pos)    return;
//#warning "not implemented: QwtSlider::setScale : because it is not necessary (rlmurx)"
}
#endif

void MyQwtSlider::setBgStyle(int s) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtSlider::BGSTYLE sty = (QwtSlider::BGSTYLE) s;
  QwtSlider::setBgStyle(sty);
#else
  setBgStyle(s);
#endif
}

void MyQwtSlider::setScalePosition(int pos)
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtSlider::setScalePosition((QwtSlider::ScalePos) pos);
  switch(pos)
  {
    case PV::SliderLeft:
      QwtSlider::setScalePosition(QwtSlider::LeftScale);
      break;
    case PV::SliderRight:
      QwtSlider::setScalePosition(QwtSlider::RightScale);
      break;
    case PV::SliderTop:
      QwtSlider::setScalePosition(QwtSlider::TopScale);
      break;
    case PV::SliderBottom:
      QwtSlider::setScalePosition(QwtSlider::BottomScale);
      break;
    default:
      QwtSlider::setScalePosition(QwtSlider::NoScale);
      break;
  }
#else
#warning "//rlmurx-was-here : for me this is big murx"
  switch(pos)
  {
    case PV::SliderLeft:
      QwtSlider::setScalePosition(QwtSlider::ScalePosition::TrailingScale);
      break;
    case PV::SliderRight:
      QwtSlider::setScalePosition(QwtSlider::ScalePosition::LeadingScale);
      break;
    case PV::SliderTop:
      QwtSlider::setScalePosition(QwtSlider::ScalePosition::TrailingScale);
      break;
    case PV::SliderBottom:
      QwtSlider::setScalePosition(QwtSlider::ScalePosition::LeadingScale);
      break;
    default:
      //QwtSlider::setScalePosition(QwtSlider::ScalePosition::TrailingScale);
      break;
  }    
#endif
}

void MyQwtSlider::setThumbWidth(int width) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtSlider::setThumbWidth(width);
#else
  if(width) return;
//#warning "not implemented QwtSlider::setThumbWidth(width); (rlmurx)"
#endif
}

void MyQwtSlider::setThumbLength(int length) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtSlider::setThumbLength(length);
#else
  if(length) return;
//#warning "not implemented QwtSlider::setThumbLength(width); (rlmurx)"
#endif
}

void MyQwtSlider::setMargins(int m1, int m2) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtSlider::setMargins(m1, m2);
#else
  if(m1 == m2) return;
  QwtSlider::scaleEngine()->setMargins(m1,m2);
//#warning "not implemented QwtSlider::setMargins(m1, m2); (rlmurx)"
#endif
}

void MyQwtSlider::slotValueChanged(double value)
{
  char buf[80];

  sprintf(buf,"slider(%d,%lf)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyQwtSlider::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtSlider::mousePressEvent(event);
}

void MyQwtSlider::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtSlider::mouseReleaseEvent(event);
}

void MyQwtSlider::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtSlider::enterEvent(event);
}

void MyQwtSlider::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtSlider::leaveEvent(event);
}

MyQwtDial::MyQwtDial(int *sock, int ident, QWidget *parent, const char *name)
          :QwtDial(parent)
{
  s = sock;
  id = ident;
  connect(this, SIGNAL(valueChanged(double)), SLOT(slotValueChanged(double)));
  // Set default properties and create needle
  setScaleArc(30,330);
  setScale(10,2,0);
  setRange(0,10);
  setFrameShadow(Sunken);
  setLineWidth(5);  //for frame shadow
  setReadOnly(true);
  setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow,true, QColor(0,0,0), QColor(0,0,0)));  // color: needle, center
  if(name != NULL) setObjectName(name);
}

MyQwtDial::~MyQwtDial()
{
}

void MyQwtDial::setMass(int m) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtDial::setMass(m);
#else
  if(m) return;
//only relevant for QwtWheel
//#warning "not impemented: QwtDial::setMass : Set the slider's mass for flywheel effect. (rlmurx)"
#endif
}

void MyQwtDial::setOrientation(Qt::Orientation o) //rlmurx-was-here
{
  // issue-reported-by-jose
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtDial::setOrientation(o);
#else
  if(o) return;
#endif
}

void MyQwtDial::showBackground(int b) //rlmurx-was-here
{
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  QwtDial::showBackground(b);
#else
  if(b) return;
//
//#warning "not implemented QwtDial::showBackground(b); (rlmurx)"
#endif
}

#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
void MyQwtDial::setScale(int min, int max, int step) //rlmurx-was-here
{
  // issue-reported-by-jose
  QwtDial::setScale((double) min, (double) max);
  if(min == max) return;
  if(step == 1) return;
  //rlmurx-was-here we do not need this in the old version
}
#else
void MyQwtDial::setScale(int min, int max, int step) //rlmurx-was-here
{
  // issue-reported-by-jose
  QwtDial::setScale((double) min, (double) max);
  if(min == max) return;
  if(step == 1) return;
  //rlmurx-was-here we do not need this in the old version
}
#endif

#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
void MyQwtDial::setRange(int min, int max, int rlmurx) //rlmurx-was-here
{
  if(min == max) return;
  if(rlmurx) return;
  //rlmurx-was-here we do not ne4ed this in the old version
}
#else
void MyQwtDial::setRange(double min, double max, double rlmurx) //rlmurx-was-here
{
  //  QwtAbstractScale::setScaleStepSize(rlmurx); //jjmg was here
    QwtAbstractScale::setScale(min,max); //jjmg was here
  if(min == max) return;
  if(rlmurx) return;
}
#endif

void MyQwtDial::slotValueChanged(double value)
{
  char buf[80];

  sprintf(buf,"slider(%d,%lf)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyQwtDial::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtDial::mousePressEvent(event);
}

void MyQwtDial::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtDial::mouseReleaseEvent(event);
}

void MyQwtDial::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtDial::enterEvent(event);
}

void MyQwtDial::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtDial::leaveEvent(event);
}

MyQwtCompass::MyQwtCompass(int *sock, int ident, QWidget *parent, const char *name)
             :QwtCompass(parent)
{
  s = sock;
  id = ident;
  connect(this, SIGNAL(valueChanged(double)), SLOT(slotValueChanged(double)));
  if(name != NULL) setObjectName(name);
  //setScaleTicks(1,10,20);
  //setScaleArc(-120.0,120.0);
  //setRange(-20.0,20.0);
  //setScaleOptions(7);
}

MyQwtCompass::~MyQwtCompass()
{
}

void MyQwtCompass::setOrientation(Qt::Orientation o) //rlmurx-was-here
{
  if(o) return;
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  //not implementes
#else
  //not implemented
#endif
}

void MyQwtCompass::slotValueChanged(double value)
{
  char buf[80];

  sprintf(buf,"slider(%d,%lf)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyQwtCompass::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtCompass::mousePressEvent(event);
}

void MyQwtCompass::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtCompass::mouseReleaseEvent(event);
}

void MyQwtCompass::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtCompass::enterEvent(event);
}

void MyQwtCompass::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtCompass::leaveEvent(event);
}

MyQwtAnalogClock::MyQwtAnalogClock(int *sock, int ident, QWidget *parent, const char *name)
                :QwtAnalogClock(parent)
{
  s = sock;
  id = ident;
  connect(this, SIGNAL(valueChanged(double)), SLOT(slotValueChanged(double)));
  if(name != NULL) setObjectName(name);
}

MyQwtAnalogClock::~MyQwtAnalogClock()
{
}

void MyQwtAnalogClock::setOrientation(Qt::Orientation o) //rlmurx-was-here
{
  if(o) return;
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  //not implementes
#else
  //not implemented
#endif
}

void MyQwtAnalogClock::slotValueChanged(double value)
{
  char buf[80];

  sprintf(buf,"slider(%d,%lf)\n",id,value);
  tcp_send(s,buf,strlen(buf));
}

void MyQwtAnalogClock::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QwtAnalogClock::mousePressEvent(event);
}

void MyQwtAnalogClock::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QwtAnalogClock::mouseReleaseEvent(event);
}

void MyQwtAnalogClock::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtAnalogClock::enterEvent(event);
}

void MyQwtAnalogClock::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QwtAnalogClock::leaveEvent(event);
}

#endif // #ifndef NO_QWT

MyQDateEdit::MyQDateEdit(int *sock, int ident, QWidget *parent, const char *name)
            :QDateEdit(parent)
{
  s = sock;
  id = ident;
  pvsVersion = 0;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(dateChanged(const QDate &)), SLOT(slotValueChanged(const QDate &)));
}

MyQDateEdit::~MyQDateEdit()
{
}

void MyQDateEdit::slotValueChanged(const QDate &date)
{
char buf[80];

  if(pvsVersion <= 0x040600) sprintf(buf,"text(%d,\"%d:%d:%d\")\n", id, date.year(), date.month(), date.day());
  else                       sprintf(buf,"text(%d,\"%04d-%02d-%02d\")\n", id, date.year(), date.month(), date.day());
  if(date.isValid()) tcp_send(s,buf,strlen(buf));
}

void MyQDateEdit::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QDateEdit::mousePressEvent(event);
}

void MyQDateEdit::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QDateEdit::mouseReleaseEvent(event);
}

void MyQDateEdit::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QDateEdit::enterEvent(event);
}

void MyQDateEdit::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QDateEdit::leaveEvent(event);
}

MyQTimeEdit::MyQTimeEdit(int *sock, int ident, QWidget *parent, const char *name)
            :QTimeEdit(parent)
{
  s = sock;
  id = ident;
  pvsVersion = 0;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(timeChanged(const QTime &)), SLOT(slotValueChanged(const QTime &)));
}

MyQTimeEdit::~MyQTimeEdit()
{
}

void MyQTimeEdit::slotValueChanged(const QTime &time)
{
char buf[80];

  if(pvsVersion <= 0x040600) sprintf(buf,"text(%d,\"%d.%d.%d.%d\")\n", id, time.hour(), time.minute(), time.second(), time.msec());
  else                       sprintf(buf,"text(%d,\"%02d:%02d:%02d.%d\")\n", id, time.hour(), time.minute(), time.second(), time.msec());
  if(time.isValid()) tcp_send(s,buf,strlen(buf));
}

void MyQTimeEdit::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QTimeEdit::mousePressEvent(event);
}

void MyQTimeEdit::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QTimeEdit::mouseReleaseEvent(event);
}

void MyQTimeEdit::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTimeEdit::enterEvent(event);
}

void MyQTimeEdit::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTimeEdit::leaveEvent(event);
}

MyQDateTimeEdit::MyQDateTimeEdit(int *sock, int ident, QWidget *parent, const char *name)
                :QDateTimeEdit(parent)
{
  s = sock;
  id = ident;
  pvsVersion = 0;
  if(name != NULL) setObjectName(name);
  connect(this, SIGNAL(dateTimeChanged(const QDateTime &)), SLOT(slotValueChanged(const QDateTime &)));
}

MyQDateTimeEdit::~MyQDateTimeEdit()
{
}

void MyQDateTimeEdit::slotValueChanged(const QDateTime &date_time)
{
char buf[200];

  if(pvsVersion <= 0x040600) sprintf(buf,"text(%d,\"%d:%d:%d-%d.%d.%d.%d\")\n", id, 
                                                                 date_time.date().year(),
                                                                 date_time.date().month(),
                                                                 date_time.date().day(),
                                                                 date_time.time().hour(),
                                                                 date_time.time().minute(),
                                                                 date_time.time().second(),
                                                                 date_time.time().msec());
  else sprintf(buf,"text(%d,\"%04d-%02d-%02dT%02d:%02d:%02d.%d\")\n", id, 
                                                                 date_time.date().year(),
                                                                 date_time.date().month(),
                                                                 date_time.date().day(),
                                                                 date_time.time().hour(),
                                                                 date_time.time().minute(),
                                                                 date_time.time().second(),
                                                                 date_time.time().msec());
  if(date_time.isValid()) tcp_send(s,buf,strlen(buf));
}

void MyQDateTimeEdit::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QDateTimeEdit::mousePressEvent(event);
}

void MyQDateTimeEdit::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QDateTimeEdit::mouseReleaseEvent(event);
}

void MyQDateTimeEdit::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QDateTimeEdit::enterEvent(event);
}

void MyQDateTimeEdit::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QDateTimeEdit::leaveEvent(event);
}

MyQDockWidget::MyQDockWidget(QString &title, int *sock, int ident, int dockID, QWidget *parent, const char *name)
             :QDockWidget(title, parent)
{
  s = sock;
  id = ident;
  dock_id = dockID;
  w = h = 400;
  x_original = 400;
  y_original = 400;
  w_original = 400;
  h_original = 400;
  connect(this, SIGNAL(topLevelChanged(bool)), SLOT(slotTopLevelChanged(bool)));
  if(name != NULL) setObjectName(name);
}

MyQDockWidget::~MyQDockWidget()
{
}

void MyQDockWidget::slotTopLevelChanged(bool toplevel)
{
  if(toplevel)
  {
    resize(w,h);
  }

  if (!isFloating() && allowedAreas() == Qt::NoDockWidgetArea)
  {
    setFloating(true);
  }
}

void mySetForegroundColor(QWidget *w, int type, int r, int g, int b)
{
  if(w == NULL) return;
  if(r==-1 && g==-1 && b==-1)
  {
    //w->unsetPalette();
    QPalette palette;
    w->setPalette(palette);
    return;
  }
  QPalette palette = w->palette();
  if(type == TQPushButton)
  {
    palette.setColor(QPalette::ButtonText,QColor(r,g,b));
    ((MyQPushButton *)w)->setPalette(palette);
  }
  else
  {
    palette.setColor(QPalette::WindowText,QColor(r,g,b));
    w->setPalette(palette);
  }
}

void mySetBackgroundColor(QWidget *w, int type, int r, int g, int b)
{
  if(opt.arg_debug) printf("mySetBackgroundColor: type=%d r=%d g=%d b=%d\n",type,r,g,b);
  if(w == NULL) return;
#if QT_VERSION > 0x050000
  if(type == TQDraw)
  {
    ((QDrawWidget *)w)->setBackgroundColor(r,g,b);
  }
  else
  {
    char text[80];
    sprintf(text,"background-color: rgb(%d,%d,%d)", r, g , b);
    if(r == -1 && g == -1 && b == -1)
    {
      sprintf(text,"background-color: rgb(%d,%d,%d)", 236, 235 , 235);
    }
    w->setStyleSheet(text);
  }  
#else
#ifdef PVWIN32
  // WinXP style does not support background colors
  static int     first = 1;
  static QStyle *style = NULL;
  if(first)
  {
#if QT_VERSION < 0x050000 
    style = new QWindowsStyle;
#endif    
    first = 0;
  }  
#endif
  if(r==-1 && g==-1 && b==-1)
  {
    //w->unsetPalette();
    QPalette palette;
    w->setPalette(palette);
    return;
  }
  QPalette palette = w->palette();
  if(type == TQPushButton ||
     type == TQRadio      ||
     type == TQCheck      )
  {
    //palette.setColor(QPalette::Button,QColor(r,g,b));
#ifdef PVWIN32
    w->setStyle(style);
#endif
    w->setAutoFillBackground(false);
    QBrush brush(QColor(r,g,b,255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active,   QPalette::Button, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
    w->setPalette(palette);
  }
  else if(type == TQMultiLineEdit ||
          type == TQLineEdit      ||
          type == TQTextBrowser   )
  {
#ifdef PVWIN32
    w->setStyle(style);
#endif
    w->setAutoFillBackground(false);
    QBrush brush(QColor(r,g,b,255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active,   QPalette::Base, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
    w->setPalette(palette);
    //w->setAutoFillBackground(true);
    //palette.setColor(QPalette::Base,QColor(r,g,b));
    //w->setPalette(palette);
  }
  else if(type == TQDraw)
  {
    ((QDrawWidget *)w)->setBackgroundColor(r,g,b);
  }
  else
  {
    w->setAutoFillBackground(true);
    palette.setColor(QPalette::Window,QColor(r,g,b));
    w->setPalette(palette);
  }
#endif  
}
