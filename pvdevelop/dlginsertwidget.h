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
#ifndef DLG_INSERT_WIDGET_H
#define DLG_INSERT_WIDGET_H

#include "ui_dlginsertwidget.h"

void myMove(QWidget *w, int x, int y);
void myResize(QWidget *w, int width, int height);
void mySetGeometry(QWidget *w, int x, int y, int width, int height);

class dlgInsertWidget: public QDialog
{
    Q_OBJECT

public:
    dlgInsertWidget();
    ~dlgInsertWidget();
    void run();
    QWidget *newWidget(QWidget *root, QWidget *parent, int x, int y);
    void setDefaultObjectName(QWidget *root, QWidget *item);
    int  ret;
    QWidget *myrootwidget;
public slots:
    void slotOk();
    void slotCancel();
    void findMax(QObject *w, int *max);
private:
    Ui_DialogInsertWidget *form;
};

#endif

