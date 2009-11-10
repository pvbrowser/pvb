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
#ifndef INSERT_FUNCTION_H
#define INSERT_FUNCTION_H
#include <QDialog>
#include "ui_dlginsertfunction.h"

class QWidget;
class QTreeWidgetItem;

class dlgInsertFunction: public QDialog
{
    Q_OBJECT

public:
    dlgInsertFunction();
    ~dlgInsertFunction();
    QString run();
private:
    QTreeWidgetItem *top(const char *text);
    QTreeWidgetItem *sub(QTreeWidgetItem *root, const char *text);
    Ui_Dialog *form;
};
#endif
