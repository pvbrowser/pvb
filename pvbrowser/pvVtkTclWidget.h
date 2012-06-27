/***************************************************************************
                          pvtcl.h  -  description
                             -------------------
    begin                : Mon Jun 10 2002
    copyright            : (C) 2002 by R. Lehrig
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

#ifndef PV_VTK_TCL_WIDGET_H
#define PV_VTK_TCL_WIDGET_H

#ifndef __VMS
  #ifndef NO_VTK
    #ifndef USE_VTK
      #define USE_VTK
    #endif
  #endif
#endif

#ifdef USE_VTK

#include "QVTKWidget.h"
#include "vtkRenderWindow.h"
#include "vtkTclUtil.h"
#include "tcl.h"

/**
  *@author R. Lehrig
  */

class pvVtkTclWidget : public QVTKWidget
{
  Q_OBJECT
public: 
  pvVtkTclWidget(QWidget *parent=0, const char *name=0, int id=-1, int *s=NULL);
  ~pvVtkTclWidget();
  void interpret(const char *command, ...);
  void interpretFile(const char *filename);
  Tcl_Interp        *interp;
  vtkTclCommand     *tclcommand;
  void updateGL();

private:
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void enterEvent(QEvent *event);
  virtual void leaveEvent(QEvent *event);
  int id; // id of widget
  int *s; // socket
};

#endif
#endif
