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
#ifndef DLG_PROPERTY_H
#define DLG_PROPERTY_H

#include <QDialog>
#include "ui_dlgproperty.h"

class dlgProperty: public QDialog
{
    Q_OBJECT

public:
    dlgProperty(QWidget *_widget);
    ~dlgProperty();
    int run();
    int modified;
public slots:
    void slotOk();
    void slotCancel();
    void slotUpdate();
private:
    QWidget *widget;
    int ret;
    Ui_DialogProperty *form;
};

#endif

