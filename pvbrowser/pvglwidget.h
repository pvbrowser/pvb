/***************************************************************************
                          pvglwidget.h  -  description
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

#ifndef PVGLWIDGET_H
#define PVGLWIDGET_H
#define PVGLWIDGET_H_V5

#ifdef USE_OPEN_GL
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QStringList>
#include <qevent.h>

/**
  *@author R. Lehrig
  */

class PvGLWidget : public QOpenGLWidget , protected QOpenGLFunctions
{
  Q_OBJECT

public:
  PvGLWidget(QWidget *parent=0, int ident=0, int *socket=NULL , const char *name=0);
  ~PvGLWidget();
  void gl_interpret();
  int paint_done;
  void updateGL();

protected:
  // these 3 methods are empty (it is done by the server)
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int w, int h);

private:
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void enterEvent(QEvent *event);
  virtual void leaveEvent(QEvent *event);
  GLvoid *glgetbytes();
  void glputbytes(void *ptr, int len);
  void gllongresponse(long l);
  int interpret(const char *line);
  int *s;  // socket
  int id; // id of OpenGL widget
  GLsizei minobject, maxobject; // OpenGL lists that we have to free in the destructor
  QStringList list;
};

#endif
#endif
