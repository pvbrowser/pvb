/****************************************************************************
**
** Copyright (C) 2000-2006 Lehrig Software Engineering.
**
** This file is part of the pvbrowser project.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef DESIGNER_H
#define DESIGNER_H
#include <QString>
#include <QStatusBar>
#include <qwidget.h>
#include "dlginsertwidget.h"

class QScrollArea;

class MyRootWidget : public QWidget
{
  public:
    MyRootWidget(MyRootWidget *parent);
    ~MyRootWidget();
    void setScroll(QScrollArea *_scroll);
    QStatusBar *statusBar;
    int modified;
    void deleteLastChild();
    void showProperties();
    void GrabMouse();
    void ReleaseMouse();
    void EditLayout();
    void MoveKey(int key);
    QMainWindow  *mainWindow;
  private:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QWidget *getChild(int x, int y);
    void showWidgets(QObject *item);
    void restoreColors(QWidget *item);
    void copyAttributes(QWidget *source, QWidget *destination);
    int  aboveDesignArea(int x, int y, int gx, int gy);
    void printStatusMessage(QWidget *child);
    void setCursor(Qt::CursorShape cursor);
    Qt::CursorShape currentCursor;
    QScrollArea *scroll;
    int xOld,yOld,buttonDown,xChild0,yChild0,wChild0,hChild0,grabbed,tabbing,copying, inMiddle,parentLevel,reparentDone;
    QWidget *clickedChild, *lastChild, *lastTabChild, *parentLevel0, *parentLevel1, *parentLevel2;
    QPalette savedPalette;
    QString savedStatusTip;
    QString lastTab;
    dlgInsertWidget insert;
};

class Designer
{
  public:
    Designer(const char *mask);
    ~Designer();
    MyRootWidget *root;
  private:
    QString filename;
    int xclick, yclick;
    int imask;
};

#endif
