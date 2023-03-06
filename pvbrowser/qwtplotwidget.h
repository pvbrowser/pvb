/***************************************************************************
                          qwtplotwidget.h  -  description
                             -------------------
    begin                : Mon Mai 26 2003
    copyright            : (C) 2003 by R. Lehrig
                         : Angel Maza
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

#ifndef QWTPLOTWIDGET_H
#define QWTPLOTWIDGET_H

#include <time.h>
#include <qwidget.h>
#include <qdatetime.h>
#include <QMouseEvent>
#include "qwt_plot.h"
#include "qwt_scale_draw.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_grid.h"
#include "qwt_legend.h"

/**
  *@author R. Lehrig
  */

typedef QwtPlotCurve*  MyCurve;
typedef QwtPlotMarker* MyMarker;

class QwtPlotWidget : public QwtPlot
{
  Q_OBJECT
  public:
    QwtPlotWidget(int *sock, int ident, QWidget *parent, int numberCurves=10, int numberMarker=1000);
    ~QwtPlotWidget();
    int interpret(const char *command, double *x=NULL, double *y=NULL);
    int sendJpeg2clipboard();
    int  nMarker;
    int  nCurves;
    int  autolegend, enablelegend, legendframestyle;
    //long *curves;
    //long *marker;
    MyCurve  *curves;
    MyMarker *marker;
    QwtPlotGrid    grid;
    QwtLegend     *legend;

  private:
    int getText(const char *str, QString &text);
    int isCommand(const char *command);
    const char *line;
    int *s,id;
  public slots:
    void slotMouseMoved( const QMouseEvent& );
    void slotMousePressed( const QMouseEvent& );
    void slotMouseReleased( const QMouseEvent& );
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
};

class UserScaleDraw: public QwtScaleDraw
{
public:
  UserScaleDraw(char * text)
  {
    setLabelRotation(-50.0);
    setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
    strcpy( out, text );
  }

  //virtual QString label(double v) const
  //rlmurx-was-here virtual QwtText label(double v) const
  //virtual QwtText QwtAbstractScaleDraw::label(double v) const
#ifdef PVB_FOOTPRINT_OLD_VERSION_BEFORE_QWT62
  virtual QwtText label(double v) const
  {
    time_t curtime;
//  struct tm *loctime;
//  char time[1024];
    curtime = (int) v;
    QDateTime qdt;
    qdt.setTime_t( curtime );
//  loctime = localtime( &curtime );
//  return qdt.toString( "yyyy-MM-dd \n hh:mm:ss" );
    QwtText qwttext;
    qwttext.setText(qdt.toString( out ));
    return qwttext;
  }
#endif
private:
  char out[1024];
};
#endif
