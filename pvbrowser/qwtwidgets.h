/***************************************************************************
                          qwtwidgets.h  -  description
                             -------------------
    begin                : Fri Aug 25 2005
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
#ifndef _MY_QWT_WIDGETS_H_
#define _MY_QWT_WIDGETS_H_

#include "qwt_scale_widget.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_div.h"
#include "qwt_scale_engine.h"
#include "qwt_scale_map.h"
#include "qwt_thermo.h"
#include "qwt_knob.h"
#include "qwt_counter.h"
#include "qwt_wheel.h"
#include "qwt_slider.h"
#include "qwt_dial.h"
#include "qwt_analog_clock.h"
#include "qwt_compass.h"
#include "qwt_compass_rose.h"
#include "qwt_dial_needle.h"

// --- QWT -----------------------------------------------------------------------------
class MyQwtScale : public QwtScaleWidget
{
    Q_OBJECT
public:
    MyQwtScale(int *sock, int ident, int position=0, QWidget *parent=0, const char *name=0);
    ~MyQwtScale();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtThermo : public QwtThermo
{
    Q_OBJECT
public:
    MyQwtThermo(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtThermo();

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtKnob : public QwtKnob
{
    Q_OBJECT
public:
    MyQwtKnob(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtKnob();

public slots:
    void slotValueChanged(double value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtCounter : public QwtCounter
{
    Q_OBJECT
public:
    MyQwtCounter(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtCounter();

public slots:
    void slotValueChanged(double value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtWheel : public QwtWheel
{
    Q_OBJECT
public:
    MyQwtWheel(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtWheel();

public slots:
    void slotValueChanged(double value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtSlider : public QwtSlider
{
    Q_OBJECT
public:
    MyQwtSlider(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtSlider();

public slots:
    void slotValueChanged(double value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtDial : public QwtDial
{
    Q_OBJECT
public:
    MyQwtDial(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtDial();

public slots:
    void slotValueChanged(double value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtAnalogClock : public QwtAnalogClock
{
    Q_OBJECT
public:
    MyQwtAnalogClock(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtAnalogClock();

public slots:
    void slotValueChanged(double value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

class MyQwtCompass : public QwtCompass
{
    Q_OBJECT
public:
    MyQwtCompass(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtCompass();

public slots:
    void slotValueChanged(double value);

private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
};

#endif
