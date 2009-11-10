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
#ifndef DLG_NEW_PRJ_H
#define DLG_NEW_PRJ_H

#include "ui_dlgnewprj.h"

class dlgnewprj : public QDialog
{
  Q_OBJECT

  public:
    Ui_newprj *uidlg;
    dlgnewprj();
    ~dlgnewprj();

  private slots:
    void getdir();
};

#endif
