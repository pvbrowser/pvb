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
    void setOrientation(Qt::Orientation o); //rlmurx-was-here

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
    void setOrientation(Qt::Orientation o);         //rlmurx-was-here
    void setFillColor(QColor rgb);                  //rlmurx-was-here
    void setAlarmColor(QColor rgb);                 //rlmurx-was-here
    void setRange(double vmin, double vmax, double step);    //rlmurx-was-here jjmg was here
    void setMargin(int m);                          //rlmurx-was-here
    void setScale(double s1, double s2, double s3); //rlmurx-was-here

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
    void setOrientation(Qt::Orientation o);         //rlmurx-was-here
    void setMass(int w);                            //rlmurx-was-here
    void setRange(int vmin, int vmax, int step=-1); //rlmurx-was-here
    void setScale(double s1, double s2, double s3); //rlmurx-was-here

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
    void setOrientation(Qt::Orientation o); //rlmurx-was-here

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
    void setOrientation(Qt::Orientation o); //rlmurx-was-here
    void setReadOnly(int w);                //rlmurx-was-here
    void setInternalBorder(int width);      //rlmurx-was-here

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
    enum BGSTYLE //rlmurx-was-here (BGSTYLE was removed in qwt6.2x)
    {
        BgTrough = 0x1,
        BgSlot = 0x2,
        BgBoth = BgTrough | BgSlot
    };

    MyQwtSlider(int *sock, int ident, QWidget *parent=0, const char *name=0);
    ~MyQwtSlider();
    void setOrientation(Qt::Orientation o); //rlmurx-was-here
    void setScale(int min, int max, int step, int logarithmic=123); //rlmurx-was-here
    void setMass(int w);                    //rlmurx-was-here
    void setScalePosition(int pos);         //rlmurx-was-here
    void setBgStyle(int st);                //rlmurx-was-here
    void setThumbLength(int l);             //rlmurx-was-here
    void setThumbWidth(int w);              //rlmurx-was-here
    void setMargins(int x, int y);          //rlmurx-was-here

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
    void setOrientation(Qt::Orientation o);         //rlmurx-was-here
    void setScale(int min, int max, int step);      //rlmurx-was-here
    void setRange(double vmin, double vmax, double step=-1); //rlmurx-was-here jjmg was here
    void setMass(int w);                            //rlmurx-was-here
    void showBackground(int b);                     //rlmurx-was-here

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
    void setOrientation(Qt::Orientation o);         //rlmurx-was-here

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
    void setOrientation(Qt::Orientation o); //rlmurx-was-here

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
