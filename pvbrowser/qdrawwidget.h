/***************************************************************************
                          qdrawwidget.h  -  description
                             -------------------
    begin                : Fri Nov 17 2000
    copyright            : (C) 2000 by Rainer Lehrig
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
#ifndef _QDRAW_WIDGET_H_
#define _QDRAW_WIDGET_H_

#include <stdio.h>
#include <QtGui>
#include <QSvgRenderer>

#define MAXARRAY 1024*4  // maximum array size for line(x,y,n)

class QDrawWidget;

typedef struct _SVG_LINE_
{
  char *line;
  struct _SVG_LINE_ *next;
}SVG_LINE;

class pvSvgAnimator
{
  public:
    pvSvgAnimator(int *socket, QDrawWidget *draw);
    ~pvSvgAnimator();
    int read();
    int update(int on_printer);
    int wrapTransformation(int iline, SVG_LINE *last_open, const char *objectname, const char *tag, const char *text);
    int svgPrintf(const char *objectname, const char *tag, const char *text, const char *after=NULL);
    int svgRecursivePrintf(const char *objectname, const char *tag, const char *text, const char *after=NULL);
    int svgTextPrintf(const char *objectname, const char *text);
    int show(const char *objectname, int state); // state := 0|1
#if QT_VERSION >= 0x040201
    int perhapsSetOverrideCursor(int x, int y, float zoomx, float zoomy, int buttons);
    int perhapsSendSvgEvent(const char *event, int *s, int id,int x, int y, float zoomx, float zoomy);
#endif
    int testoutput();
  private:
    int closefile();
    void perhapsFixQtBugOnPath(SVG_LINE *next_line, const char *line);
    SVG_LINE *first;
    char *comment;
    int *s;
    int num_lines;
    QDrawWidget *draw;
};

/* font alignment */
enum FontAlignment {     /*                  */
ALIGN_LEFT=0,            /*  example         */
ALIGN_CENTER,            /*      example     */
ALIGN_RIGHT,             /*          example */
ALIGN_VERT_CENTER        /*      e           */
};                       /*      x           */
                         /*      a           */
                         /*      m           */
                         /*      p           */
                         /*      l           */
                         /*      e           */
                         /*                  */
/* linestyle for lines in axis */
enum Linestyle {
LINESTYLE_NONE=0,
LINESTYLE_CIRCLE,
LINESTYLE_CROSS,
LINESTYLE_RECT
};

class QDrawWidget : public QWidget
{
    Q_OBJECT
public:
    QDrawWidget( QWidget *parent=0, const char *name=0, int wFlags=0, int *sock=NULL, int ident=0);
    ~QDrawWidget();

    void beginDraw();
    void endDraw();
    void showFromSocket(int *s);
    void showFromFile(const char *filename);
    void getDimensions(const char *filename, int *width, int *height);
    void print();
    void logToFile(const char *filename);
    void logBmpToFile(const char *filename);
    void copyToClipboard();
    void moveTo(int x, int y);
    void lineTo(int x, int y);
    void text(int x, int y, int alignment, const char *text);
    void textInAxis(float x, float y, int alignment, const char *text);
    void box(int x, int y, int w, int h);
    void rect(int x, int y, int w, int h);
    void xAxis(float start, float delta, float end, int draw);
    void yAxis(float start, float delta, float end, int draw);
    void xGrid();
    void yGrid();
    void rightYAxis(float start, float delta, float end, int draw);
    void drawSymbol(int x, int y, int type);
    void line(float *x, float *y, int n);
    void setColor(int r, int g, int b);
    void setFont(const char *family, int size, int weight, int italic);
    void setLinestyle(int style);
    void setZoomX(float zx);
    void setZoomY(float zy);
    void setBackgroundColor(int r, int g, int b);
    void playSVG(const char *filename);
    void socketPlaySVG();
    void svgUpdate(QByteArray &stream);
    void printSVG(QByteArray &stream);
    //QString filename;
    int  hasLayout;
    void layoutResize(int w, int h);

    virtual void resize(int w, int h);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void setGeometry(int x, int y, int w, int h);
    virtual void setGeometry(const QRect &r);

    int      interpret(const char *command);
    pvSvgAnimator *svgAnimator;
    int selectorState;
    QSvgRenderer renderer;
    int originalCursor;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    int      interpretall();
    int      gets(char *line, int len);
    int      getText(const char *line, char *text);
    int      *s;
    int      id;
    FILE     *fp;
    FILE     *flog;
    QPainter p;
    int      origwidth, origheight;
    QPixmap  *buffer;
    int      xold, yold;
    int      zx(int x);
    int      zy(int y);
    float    xmin,dx,xmax;
    float    ymin,dy,ymax;
    int      tx(float x);
    int      ty(float y);
    float    zoomx,zoomy;
    int      autoZoomX, autoZoomY;
    int      linestyle;
    int      boxx,boxy,boxw,boxh;
    int      br,bg,bb; // background colors
    int      fontsize;
    int      sr;       // symbol radius
    int      pressedX, pressedY, movedX, movedY;
    char     floatFormat[80];
};

#endif
