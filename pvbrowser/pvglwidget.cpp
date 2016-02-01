/***************************************************************************
                          pvglwidget.cpp  -  description
                             -------------------
    begin                : Wed Nov 29 2000
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
#include <stdio.h>
#ifdef BROWSERPLUGIN
#include "pvglwidget.v4.h"
#else
#include "pvglwidget.h"
#endif
#include "tcputil.h"

#ifdef USE_OPEN_GL
PvGLWidget::PvGLWidget(QWidget *parent, int ident, int *socket, const char *name, const QGLWidget* shareWidget)
           : QGLWidget(parent, shareWidget)
{
  id = ident;
  s  = socket;
  paint_done = 0;
  minobject = maxobject = -1;
  if(name != NULL) setObjectName(name);
  makeCurrent();
  //printf("construct\n");
}

PvGLWidget::~PvGLWidget()
{
  //if(isValid()) printf("valid\n");
  //else          printf("not valid\n");
  makeCurrent();
  glDeleteLists(minobject, maxobject-minobject+1);
}

void PvGLWidget::gl_interpret()
{
  char line[1024];
  while(1)
  {
#ifdef PVWIN32
retry: //XXXLEHRIG
#endif
    if(tcp_rec(s,line,sizeof(line)-1) < 0) 
    {

#ifdef PVWIN32
      goto retry;
#else
      return;
#endif
    }
#ifndef PVDEVELOP
    if(interpret(line) == -1) 
    {
      return;
    }
#endif
  }
}

void PvGLWidget::initializeGL()
{
  //printf("init\n");
  // empty (done by the server)
}

void PvGLWidget::resizeGL( int w, int h )
{
  char buf[80];
  if(w == h) return;
  //printf("resize %d %d\n",w,h);
  sprintf(buf,"resizeGL(%d,%d,%d)\n",id,w,h);
  tcp_send(s,buf,strlen(buf));
  // empty (done by the server)
}

void PvGLWidget::paintGL()
{
  char buf[80];

  //printf("paint\n");
  if(paint_done > 0)
  {
    paint_done = 0;
    return;
  }
  paint_done = 1;
  sprintf(buf,"paintGL(%d)\n",id);
  tcp_send(s,buf,strlen(buf));
  // empty (done by the server)
}

/*
void PvGLWidget::mousePressEvent(QMouseEvent *e)
{
  if(e->button() == RightButton)
  {
    char buf[80];

    sprintf(buf,"QMouseRight(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
}
*/

void PvGLWidget::mouseMoveEvent(QMouseEvent *event)
{
  char buf[100];
  sprintf( buf, "QPlotMouseMoved(%d,%d,%d)\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QGLWidget::mouseMoveEvent(event);
}

void PvGLWidget::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QGLWidget::mousePressEvent(event);
}

void PvGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QGLWidget::mouseReleaseEvent(event);
}

void PvGLWidget::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QGLWidget::enterEvent(event);
}

void PvGLWidget::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QGLWidget::leaveEvent(event);
}

#endif
