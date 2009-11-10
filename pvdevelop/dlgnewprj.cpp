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
#include "dlgnewprj.h"

dlgnewprj::dlgnewprj()
{
  uidlg = new Ui_newprj;
  uidlg->setupUi(this);
  uidlg->lineEditDirectory->setText(QDir::current().path());
  uidlg->lineEditName->setText("pvs");
  connect(uidlg->pushButtonOk,    SIGNAL(clicked()),this,SLOT(accept()));
  connect(uidlg->pushButtonCancel,SIGNAL(clicked()),this,SLOT(reject()));
  connect(uidlg->pushButtonDir,   SIGNAL(clicked()),this,SLOT(getdir()));
}

dlgnewprj::~dlgnewprj()
{
  delete uidlg;
}

void dlgnewprj::getdir()
{
  QString dir = QFileDialog::getExistingDirectory();
  if(dir == "") return;
  uidlg->lineEditDirectory->setText(dir);
}
