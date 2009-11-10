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
#include "../pvbrowser/pvdefine.h"
#include <QFileDialog>
#include "dlgeditlayout.h"

dlgeditlayout::dlgeditlayout()
{
  uidlg = new Ui_DialogEditLayout;
  uidlg->setupUi(this);
  uidlg->textEditConstructors->setMaximumHeight(150);
}

dlgeditlayout::~dlgeditlayout()
{
  delete uidlg;
}
