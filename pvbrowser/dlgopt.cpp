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
#include "pvdefine.h"
#include <qlayout.h>
#include <qfile.h>
#include <stdio.h>
#include "dlgopt.h"
#include "opt.h"

extern OPT opt;

DlgOpt::DlgOpt(QWidget *parent) : QDialog(parent)
{
  if(opt.arg_debug) printf("DlgOpt constructor start\n");
  initDialog();

  filename = "";

  QObject::connect(QPushButton_ok,            SIGNAL(clicked()), this,SLOT(okClicked()));
  QObject::connect(QPushButton_cancel,        SIGNAL(clicked()), this,SLOT(reject()));
  QObject::connect(QPushButton_reset_inifile, SIGNAL(clicked()), this,SLOT(slotResetInifile()));
  edit1->setLineWrapMode(QTextEdit::NoWrap);
  edit1->setTextInteractionFlags(Qt::TextEditorInteraction);

  /* add layout begin */
  QVBoxLayout *vlayout = new QVBoxLayout(this);
  QHBoxLayout *hlayout = new QHBoxLayout();

#ifdef USE_MAEMO
  vlayout->addLayout(hlayout);
  vlayout->addWidget(edit1);
  hlayout->addSpacing(10);
  hlayout->addSpacing(200);
  hlayout->addWidget(QPushButton_reset_inifile);
  hlayout->addSpacing(10);
  hlayout->addWidget(QPushButton_cancel);
  hlayout->addSpacing(10);
  hlayout->addWidget(QPushButton_ok);
  hlayout->addSpacing(200);
  vlayout->activate();
#else
  vlayout->addWidget(edit1);
  vlayout->addLayout(hlayout);
  hlayout->addSpacing(10);
  hlayout->addSpacing(200);
  hlayout->addWidget(QPushButton_reset_inifile);
  hlayout->addSpacing(10);
  hlayout->addWidget(QPushButton_cancel);
  hlayout->addSpacing(10);
  hlayout->addWidget(QPushButton_ok);
  hlayout->addSpacing(200);
  vlayout->activate();
#endif  
  /* add layout end */
}

DlgOpt::~DlgOpt()
{
}

void  DlgOpt::initDialog()
{
  if(opt.arg_debug) printf("DlgOpt initDialog start\n");
#ifdef USE_MAEMO
  edit1= new QTextEdit(this);
  //edit1->setGeometry(10,10,380,241);
  //edit1->setMinimumSize(0,0);
  //edit1->setMaximumSize(1280,1024);
#else
  this->resize(400,300);
  this->setMinimumSize(0,0);
  edit1= new QTextEdit(this);
  edit1->setGeometry(10,10,380,241);
  edit1->setMinimumSize(0,0);
  edit1->setMaximumSize(1280,1024);
#endif  
  edit1->setPlainText("");
  int fsize = 12;
  if(opt.appfontsize > 0) fsize = opt.appfontsize;
  QFont font("Courier", fsize);        
  edit1->setFont(font);

  QPushButton_ok= new QPushButton(this);
  QPushButton_ok->setGeometry(210,260,100,30);
  QPushButton_ok->setMinimumSize(0,0);
  QPushButton_ok->setText(tr("Ok"));

  QPushButton_cancel= new QPushButton(this);
  QPushButton_cancel->setGeometry(110,260,100,30);
  QPushButton_cancel->setMinimumSize(0,0);
  QPushButton_cancel->setText(tr("Cancel"));

  QPushButton_reset_inifile= new QPushButton(this);
  QPushButton_reset_inifile->setGeometry(110,260,100,30);
  QPushButton_reset_inifile->setMinimumSize(0,0);
  QPushButton_reset_inifile->setText(tr("Reset inifile"));

#ifdef USE_MAEMO
  showFullScreen();
#endif
  if(opt.arg_debug) printf("DlgOpt initDialog end\n");
}

void DlgOpt::setFilename(const char *file)
{
  if(opt.arg_debug) printf("DlgOpt setFilename begin\n");
  QString line;
  QFile qf(file);
  filename = file;
  //printf("filename = %s\n",file);
  if(!filename.isEmpty())
  {
    if(qf.open(QIODevice::ReadOnly))
    {
      if(opt.arg_debug) printf("DlgOpt setFilename set text from inifile filename=%s will eventually crash with chinese text\n", file);
      edit1->clear();
      QTextStream ts( &qf );
      ts.setCodec(QTextCodec::codecForName("UTF-8"));
//QString murx = ts.readAll();
//printf("text(%s)\n", (const char *) murx.toUtf8());
//edit1->setPlainText( murx );
// chrash with chinese text
      edit1->setPlainText( ts.readAll() );
      qf.close();
    }
  }
  if(opt.arg_debug) printf("DlgOpt setFilename end\n");
}

void DlgOpt::okClicked()
{
  if(opt.arg_debug) printf("DlgOpt okClicked begin\n");
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
  if(opt.arg_debug) printf("DlgOpt okClicked end\n");
}

void DlgOpt::slotResetInifile()
{
  if(opt.arg_debug) printf("DlgOpt remove(%s)\n", (const char *) filename.toUtf8());
  remove((const char *) filename.toUtf8());  
  QMessageBox::information(this, "pvbrowser inifile removed", tr("Please restart pvbrowser"));
  accept();
}


