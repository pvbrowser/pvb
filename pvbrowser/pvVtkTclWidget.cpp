/***************************************************************************
                          pvtcl.cpp  -  description
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

#include "pvdefine.h"
#include "pvVtkTclWidget.h"
#include "opt.h"
#include <QMouseEvent>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include "tcputil.h"
#include <stdarg.h>

extern OPT opt;

static int rlvsnprintf(char *text, int len, const char *format, va_list ap)
{
  int ret;

#ifdef PVWIN32
  if(format == NULL || format[0] == '\0')
  {
    text[0] = '\0';
    return 1;
  }
  ret = _vsnprintf(text, len, format, ap);
#endif
#ifdef __VMS
  static char vms_is_deprecated[rl_PRINTF_LENGTH];
  if(format == NULL || format[0] == '\0')
  {
    text[0] = '\0';
    return 1;
  }
  ret = vsprintf(vms_is_deprecated, format, ap);
  rlstrncpy(text,vms_is_deprecated,len);
#endif
#ifdef PVUNIX
  if(format == NULL || format[0] == '\0')
  {
    text[0] = '\0';
    return 1;
  }
  ret = vsnprintf(text, len, format, ap);
#endif
  return ret;
}

pvVtkTclWidget::pvVtkTclWidget(QWidget *parent, const char *name, int Id, int *sock) 
               : QVTKWidget(parent)
{
  int error;
  char buf[80];
  vtkRenderWindow *temp0;

  tclcommand = NULL; 
  interp = NULL;
  if(name != NULL) setObjectName(name);
  interp = Tcl_CreateInterp();
  if(interp == NULL) return;
#ifdef PVUNIX
  tclcommand = vtkTclCommand::New();
#endif
#ifdef PVWIN32
  tclcommand = newVtkTclCommand();
  //tclcommand = vtkTclCommand::New();
  //tclcommand = new vtkTclCommand();
#endif
  if(tclcommand == NULL)
  {
    printf("ERROR: pvVtkTclWidget::tclcommand == NULL\n");
    Tcl_DeleteInterp(interp);
    return;
  }
  tclcommand->SetInterp(interp);
#ifdef PVUNIX
  Tcl_AppInit(interp);
#endif
#ifdef PVWIN32
  Tcl_Init(interp);
#endif
  interpret("package require vtk");
  interpret("package require vtkinteraction");
  interpret("package require vtktesting");
  interpret("vtkRenderWindow renWin");
  sprintf(buf,"renWin SetSize %d %d",width(),height());
  interpret(buf);

  // create sandbox
  if(Tcl_DeleteCommand(interp,"eof")       == -1) printf("could not delete Tcl eof\n");
  if(Tcl_DeleteCommand(interp,"fblocked")  == -1) printf("could not delete Tcl fblocked\n");
  if(Tcl_DeleteCommand(interp,"gets")      == -1) printf("could not delete Tcl gets\n");
  if(Tcl_DeleteCommand(interp,"fcopy")     == -1) printf("could not delete Tcl fcopy\n");
  if(Tcl_DeleteCommand(interp,"close")     == -1) printf("could not delete Tcl close\n");
  if(Tcl_DeleteCommand(interp,"read")      == -1) printf("could not delete Tcl read\n");
  if(Tcl_DeleteCommand(interp,"seek")      == -1) printf("could not delete Tcl seek\n");
  if(Tcl_DeleteCommand(interp,"flush")     == -1) printf("could not delete Tcl flush\n");
  if(Tcl_DeleteCommand(interp,"fileevent") == -1) printf("could not delete Tcl fileevent\n");
  if(Tcl_DeleteCommand(interp,"puts")      == -1) printf("could not delete Tcl puts\n");
  if(Tcl_DeleteCommand(interp,"tell")      == -1) printf("could not delete Tcl tell\n");
  if(Tcl_DeleteCommand(interp,"pid")       == -1) printf("could not delete Tcl pid\n");

  error = 0;
  temp0 = (vtkRenderWindow *)(vtkTclGetPointerFromObject("renWin",(char *) "vtkRenderWindow",interp,error));
  if(!error && temp0 != NULL)
  {
    if(opt.arg_debug) printf("Setting Tcl interp\n");
    SetRenderWindow(temp0);
    Tcl_ResetResult(interp);
  }
  id = Id;
  s  = sock;
}

pvVtkTclWidget::~pvVtkTclWidget()
{
  interpret("vtkCommand DeleteAllObjects");
  if(tclcommand != NULL) tclcommand->Delete();
  if(interp     != NULL) Tcl_DeleteInterp(interp);
  tclcommand = NULL; 
  interp = NULL;
//#ifndef PVUNIX // will be deleted in base class
//  RemoveRenderer(renderer);
//#endif
}

void pvVtkTclWidget::interpret(const char *format, ...)
{
  int estimated_length, ret;
  va_list ap;

  estimated_length = strlen(format) + 4096;
  char *buf = new char[estimated_length];

  va_start(ap,format);
  ret = rlvsnprintf(buf,estimated_length -1 , format, ap);
  va_end(ap);

  if(ret > 0 && tclcommand != NULL)
  {
#ifdef PVUNIX
    tclcommand->SetStringCommand(buf);
#endif
#ifdef PVWIN32
    //tclcommand->SetStringCommand(buf);
    if(tclcommand->StringCommand) { delete [] tclcommand->StringCommand; }
    tclcommand->StringCommand = new char[strlen(buf)+1];
    strcpy(tclcommand->StringCommand, buf);
#endif
    tclcommand->Execute(NULL, 0, NULL);
  }

  delete [] buf;
}

void pvVtkTclWidget::interpretFile(const char *filename)
{
  if(interp != NULL) Tcl_EvalFile(interp,filename);
}

void pvVtkTclWidget::updateGL()
{
  // only because of qt3->qt4 port
}

void pvVtkTclWidget::mouseMoveEvent(QMouseEvent *event)
{
  char buf[100];
  sprintf( buf, "QPlotMouseMoved(%d,%d,%d)\n",id, event->x(), event->x());
  tcp_send(s,buf,strlen(buf));
  QVTKWidget::mouseMoveEvent(event);
}

void pvVtkTclWidget::mousePressEvent(QMouseEvent *event)
{
  char buf[100];
  sprintf( buf, "QPlotMousePressed(%d,%d,%d)\n",id, event->x(), event->y());
  tcp_send(s,buf,strlen(buf));
  QVTKWidget::mousePressEvent(event);
}

void pvVtkTclWidget::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[100];
  sprintf( buf, "QPlotMouseReleased(%d,%d,%d)\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QVTKWidget::mouseReleaseEvent(event);
}

void pvVtkTclWidget::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QVTKWidget::enterEvent(event);
}

void pvVtkTclWidget::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QVTKWidget::leaveEvent(event);
}
