/***************************************************************************
                          MyTextBrowser_without_www.cpp  -  description
                             -------------------
    begin                : Fr Jan 15 2016
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
#include <sys/stat.h>
#include "opt.h"
#include "mainwindow.h"
#include "interpreter.h"

#include "qtabbar.h"
#include "qpainter.h"
#include "qmessagebox.h"
#include <QPixmap>
#include <QMouseEvent>
#include <QPrintDialog>
#include "tcputil.h"
#include "MyTextBrowser_without_www.h"

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
MyTextBrowser::MyTextBrowser(int *sock, int ident, QWidget *parent, const char *name)
              :QTextBrowser(parent)
{
  s = sock;
  id = ident;
  
  if(opt.arg_debug) printf("MyTextBrowser()\n");
  
  homeIsSet = 0;
  factor = 1.0f;
  if(name != NULL) setObjectName(name);
  mHeader = "<html>\n<head><meta charset=\"utf-8\">\n<title>MyTextBrowser</title>\n</head><body>\n";
  xOldScroll = yOldScroll = 0;
  setOpenLinks(false);
  //connect(this, SIGNAL(anchorClicked(const QUrl &)), SLOT(slotLinkClicked(const QUrl &)));
  //connect(this, SIGNAL(urlChanged(const QUrl &)), SLOT(slotUrlChanged(const QUrl &)));
  connect(this, SIGNAL(anchorClicked(const QUrl &)), SLOT(slotUrlChanged(const QUrl &)));
}

MyTextBrowser::~MyTextBrowser()
{
  printf("~MyTextBrowser()\n");
}

void MyTextBrowser::tbSetText(QString &text)
{
  int vPos = verticalScrollBar()->value();   // make cursor not jumping (ernst murnleitner)
  int hPos = horizontalScrollBar()->value();
  QTextDocument *doc = document();
  if(doc != NULL) doc->setHtml(text); // ,url);
  verticalScrollBar()->setValue(vPos);
  horizontalScrollBar()->setValue(hPos);
}

void MyTextBrowser::tbMoveCursor(int cur)
{
  moveCursor((QTextCursor::MoveOperation) cur);
}

void MyTextBrowser::tbScrollToAnchor(QString &text)
{
  scrollToAnchor(text);
}

bool MyTextBrowser::event(QEvent *e)
{
#if QT_VERSION >= 0x040600
  if(e->type() == QEvent::Gesture)
  {
    QGestureEvent *ge = static_cast<QGestureEvent*>(e);
    if(ge->gesture(Qt::PinchGesture)) return false;
  }
#endif
  return QTextBrowser::event(e);
}

void MyTextBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);
  QAction *printAct = menu.addAction("Print");
  connect(printAct, SIGNAL(triggered()), this, SLOT(slotPRINTER()));
  menu.exec(event->globalPos());
}

void MyTextBrowser::keyPressEvent(QKeyEvent *event)
{
  if(event->matches(QKeySequence::ZoomIn))
  {
    factor = factor*1.1f;
    zoomIn();
  }
  else if(event->matches(QKeySequence::ZoomOut))
  {
    factor = factor*0.9f;
    zoomOut();
  }
  else
  {
    QTextBrowser::keyPressEvent(event);
  }
}

void MyTextBrowser::moveContent(int pos)
{
  if(pos >= '\t' || pos < 0)
  {
     activateWindow();
     char text[16];
     text[0] = pos & 0x0ff;
     text[1] = '\0';
     int modifiers = pos & 0x07fffff00;
     int key = pos & 0x0ff;
     if     ((pos & 0x0ff) == '\t') key = Qt::Key_Tab;
     else if((pos & 0x0ff) == 0x0d) key = Qt::Key_Return;
     QKeyEvent pressEvent(  QEvent::KeyPress,   (Qt::Key) key, (Qt::KeyboardModifiers) modifiers, text);
     QKeyEvent releaseEvent(QEvent::KeyRelease, (Qt::Key) key, (Qt::KeyboardModifiers) modifiers, text);
     if((pos & 0x0ff) == '\t') QWidget::setFocus(Qt::TabFocusReason);
     keyPressEvent(&pressEvent);
     keyReleaseEvent(&releaseEvent);
     return;
  }
  char buf[MAX_PRINTF_LENGTH];
  QString myurl;

  if(myurl.isEmpty()) return;
  if(opt.arg_debug) printf("moveContent(%s)\n", (const char *) myurl.toUtf8());
  if(myurl.length()+40 > MAX_PRINTF_LENGTH) return;
  sprintf(buf,"text(%d,\"%s\")\n", id,decode(myurl));
  tcp_send(s,buf,strlen(buf));
}

void MyTextBrowser::setHTML(QString &text)
{
  int i;

  // replace href="/any_string" (href to file starting from root directory) by href="awite://any_string"
  while(1)
  {
    i = text.indexOf("href=\"/");
    if(i < 0) break;
    text = text.replace(i,7,"href=\"awite://");
  }

  if(opt.arg_debug) printf("MyTextBrowser::setHTML:: %s\n", (const char *) text.toUtf8());
  QString base;
  //rlmurx-was-here base.sprintf("file://%s", opt.temp);
  base = QString::asprintf("file://%s", opt.temp);

  setHtml(text);
}

static QString myDummyFilename;
static void    myDummyToHtml(QString html)
{
  FILE *fout = fopen(myDummyFilename.toUtf8(),"w");
  if(fout == NULL)
  {
    printf("could not write %s\n", (const char *) myDummyFilename.toUtf8());
    return;
  }
  fputs(html.toUtf8(), fout);
  fclose(fout);
}

void MyTextBrowser::htmlOrSvgDump(const char *filename)
{
  FILE *fout = fopen(filename,"w");
  if(fout == NULL)
  {
    printf("could not write %s\n", filename);
    return;
  }
  QString xml = toHtml();
  fputs(xml.toUtf8(), fout);
  fclose(fout);
}

void MyTextBrowser::slotUrlChanged(const QUrl &link)
{
  char buf[MAX_PRINTF_LENGTH];
  QString url;

  url = link.toString();
  if(opt.arg_debug) printf("slotUrlChanged(%s)\n", (const char *) url.toUtf8());
  if(url.length()+40 > MAX_PRINTF_LENGTH) return;
  sprintf(buf,"text(%d,\"%s\")\n", id,decode(url));
  tcp_send(s,buf,strlen(buf));
}

void MyTextBrowser::slotLoadFinished(bool ok)
{
  if(ok)
  {
  }  
}

void MyTextBrowser::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  pressPos = event->pos();
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QTextBrowser::mousePressEvent(event);
}

void MyTextBrowser::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QTextBrowser::mouseReleaseEvent(event);
}

void MyTextBrowser::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTextBrowser::enterEvent(event);
}

void MyTextBrowser::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QTextBrowser::leaveEvent(event);
}

//###################################################################################

void MyTextBrowser::setSOURCE(QString &temp, QString &text)
{
  setSource(QUrl::fromLocalFile(temp + text));
  reload();
  if(homeIsSet == 0)
  {
    home      = text;
    homeIsSet = 1;
    if(opt.arg_debug) printf("home=%s\n", (const char *) text.toUtf8());
  }
}

void MyTextBrowser::setZOOM_FACTOR(int factor)
{
  if(factor > 1) zoomIn();
  else           zoomOut();
}

void MyTextBrowser::PRINT(QPrinter *printer)
{
  if(printer == NULL) return;
  if(opt.arg_debug)printf("in PRINT printer\n");
  printer->setOutputFormat(QPrinter::PdfFormat);
  print(printer);
  printer->newPage();
}

void MyTextBrowser::slotPRINTER()
{
  QPrinter printer;
  printer.setColorMode(QPrinter::Color);
  QPrintDialog dialog(&printer);
  if (dialog.exec() == QDialog::Accepted)
  {
    PRINT(&printer);
  } 
}
