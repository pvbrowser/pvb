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
#ifndef TEXT_BROWSER_H
#define TEXT_BROWSER_H
#include <QDialog>
#include "webkit_ui_dlgtextbrowser.h"

class dlgTextBrowser: public QDialog
{
    Q_OBJECT

public:
    dlgTextBrowser(const char *manual=NULL);
    ~dlgTextBrowser();
    Ui_DialogTextBrowser *form;
    int homeIsSet;
    QString home;
public slots:
    void slotFind();
    void slotHome();
    void slotBack();
private:
    QString findWhat;
    int find;
};
#endif
