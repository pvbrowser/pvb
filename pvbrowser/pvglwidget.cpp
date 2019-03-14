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
#include "pvglwidget.h"
#include "tcputil.h"

#ifdef USE_OPEN_GL

const static int gldebug = 0;
static int app_open_gl_initialized = 0;
static void app_init_open_gl()
{
  if(app_open_gl_initialized == 0)
  {
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    app_open_gl_initialized = 1;
  }  
}

PvGLWidget::PvGLWidget(QWidget *parent, int ident, int *socket, const char *name)
           : QOpenGLWidget(parent) ,  QOpenGLFunctions()
{
  if(gldebug) printf("construct 1\n");
  id = ident;
  s  = socket;
  minobject = maxobject = -1;
  if(name != NULL) setObjectName(name);
  app_init_open_gl();
  setUpdateBehavior(QOpenGLWidget::PartialUpdate);
  if(gldebug) printf("construct end\n");
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
  if(gldebug) printf("gl_interpret begin\n");
  char line[1024];

  //makeCurrent();
  update();
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
    /*
    if(interpret(line) == -1) 
    {
      return;
    }
    */
    if(line[0] != 'g' || line[1] != 'l')
    {
      if(strncmp(line,"pvGlEnd()",9) == 0)
      {
        if(gldebug) printf("gl_interpret stop wait\n");
        char buf[100];
        sprintf( buf, "pvGlEnd()\n");
        tcp_send(s,buf,strlen(buf));
        if(gldebug) printf("gl_interpret end\n");
        return;
      }
    }
    else if(isValid()) // synchronous interpret opengl commands
    {
      if(gldebug) printf("without delay1 if(isValid)\n");      
      int lsize = list.size();
      if(lsize > 0)
      { // interpret the remembered commands
        int i;
        for(i=0; i<lsize; i++)
        {
          QString line = list.at(i);
          if(gldebug) printf("old_line:%s\n", (const char *) line.toUtf8());      
          interpret(line.toUtf8());
        }
        list.clear();
      }  
      if(gldebug) printf("without delay2 if(isValid) interpret:%s\n", line);      
      interpret(line);
    }
    else // remember opengl commands
    {
      if(gldebug) printf("list.append:%s\n", line);
      list.append(line);
    }  
#endif
  }
}

void PvGLWidget::initializeGL()
{
  if(gldebug) printf("initializeGL\n");
  initializeOpenGLFunctions();
  //glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
}

void PvGLWidget::resizeGL( int w, int h )
{
  char buf[80];
  if(w == h) return;
  if(gldebug) printf("resizeGL %d %d\n",w,h);
  sprintf(buf,"resizeGL(%d,%d,%d)\n",id,w,h);
  tcp_send(s,buf,strlen(buf)); // send request to pvserver, requesting the resize opengl commands
}

void PvGLWidget::paintGL()
{
  if(gldebug) printf("paintGL (interpret the remembered opengl commands)\n");
  int i;
  for(i=0; i<list.size(); i++)
  {
    QString line = list.at(i);
    interpret(line.toUtf8());
  }
  list.clear();
  if(i==-1) // 0
  {
    if(gldebug) printf("paintGL (request paintGL)\n");
    char buf[80];
    sprintf(buf,"paintGL(%d)\n",id);
    tcp_send(s,buf,strlen(buf)); // send request to pvserver, requesting the paintGL opengl commands
  }
  if(gldebug) printf("paintGL end i=%d\n", i);
}

void PvGLWidget::updateGL()
{
  //printf("updateGL (trigger a paintGL call)\n");
  //update();
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
  QOpenGLWidget::mouseMoveEvent(event);
}

void PvGLWidget::mousePressEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonPressed(%d) -xy=%d,%d\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QOpenGLWidget::mousePressEvent(event);
}

void PvGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QOpenGLWidget::mouseReleaseEvent(event);
}

void PvGLWidget::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QOpenGLWidget::enterEvent(event);
}

void PvGLWidget::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QOpenGLWidget::leaveEvent(event);
}

#endif

#ifdef PVDEVELOP
/*
int PvGLWidget::interpret(const char *line)
{
  if(line == NULL) return -1;
  return 0; // I'm a dummy
}
*/
#endif


