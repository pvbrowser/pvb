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
#include <qlayout.h>
#include <qfile.h>
#include <stdio.h>
#include "dlgopt.h"

DlgOpt::DlgOpt(QWidget *parent) : QDialog(parent)
{
  initDialog();

  filename = "";

  QObject::connect(QPushButton_ok,    SIGNAL(clicked()),this,SLOT(okClicked()));
  QObject::connect(QPushButton_cancel,SIGNAL(clicked()),this,SLOT(reject()));
  edit1->setLineWrapMode(QTextEdit::NoWrap);

  /* add layout begin */
  QVBoxLayout *vlayout = new QVBoxLayout(this);
  QHBoxLayout *hlayout = new QHBoxLayout();

  vlayout->addWidget(edit1);
  vlayout->addLayout(hlayout);
  hlayout->addSpacing(10);
  hlayout->addSpacing(200);
  hlayout->addWidget(QPushButton_cancel);
  hlayout->addSpacing(10);
  hlayout->addWidget(QPushButton_ok);
  hlayout->addSpacing(200);
  vlayout->activate();
  /* add layout end */
}

DlgOpt::~DlgOpt()
{
}

void  DlgOpt::initDialog()
{
  this->resize(400,300);
  this->setMinimumSize(0,0);
  edit1= new QTextEdit(this);
  edit1->setGeometry(10,10,380,241);
  edit1->setMinimumSize(0,0);
  edit1->setMaximumSize(1280,1024);
  edit1->setPlainText("");
  QFont font("Courier", 12);
  edit1->setFont(font);

  QPushButton_ok= new QPushButton(this);
  QPushButton_ok->setGeometry(210,260,100,30);
  QPushButton_ok->setMinimumSize(0,0);
  QPushButton_ok->setText(tr("Ok"));

  QPushButton_cancel= new QPushButton(this);
  QPushButton_cancel->setGeometry(110,260,100,30);
  QPushButton_cancel->setMinimumSize(0,0);
  QPushButton_cancel->setText(tr("Cancel"));
}

void DlgOpt::setFilename(const char *file)
{
  QString line;
  QFile qf(file);
  filename = file;
  //printf("filename = %s\n",file);
  if(!filename.isEmpty())
  {
    if(qf.open(QIODevice::ReadOnly))
    {
      edit1->clear();
      QTextStream ts( &qf );
      ts.setCodec(QTextCodec::codecForName("UTF-8"));
      edit1->setPlainText( ts.readAll() );
      qf.close();
    }
  }
}

void DlgOpt::okClicked()
{
  QString text;
  QFile qf(filename);
  if(qf.open(QIODevice::WriteOnly))
  {
    QTextStream t( &qf );
    t.setCodec(QTextCodec::codecForName("UTF-8"));
    t << edit1->toPlainText();
    qf.close();
  }
  accept();
}
