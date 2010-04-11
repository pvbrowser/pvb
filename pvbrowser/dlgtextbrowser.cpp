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
#include "dlgtextbrowser.h"
#include "webkit_ui_dlgtextbrowser.h"
#include <QPushButton>
#include <QInputDialog>
#include <QLineEdit>
#include <QFile>
#include <qmessagebox.h>
#include "opt.h"
#include <stdio.h>
#include <string.h>
#ifdef PVWIN32
#include <windows.h>
#endif

extern OPT opt;

dlgTextBrowser::dlgTextBrowser(const char *manual)
{
  char cmd[1024],buf[1024];

  homeIsSet = 0;
  strcpy(buf,"index.html");
  
  form = new Ui_DialogTextBrowser;
  form->setupUi(this);

  if(manual == NULL)
  {
    setWindowTitle(QApplication::translate("DialogTextBrowser", "pvbrowser Manual"));
  }
  else
  {
    if(strlen(manual) < (int) (sizeof(cmd)-1) ) strcpy(buf,manual);
  }

#ifdef PVWIN32
  ExpandEnvironmentStrings(buf,cmd,sizeof(cmd)-1);
#else
  strcpy(cmd,buf);
#endif

  QFile fin(cmd);
  if(fin.exists())
  {
    // this is damn slow on windows begin
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    form->textBrowser->load(QUrl::fromLocalFile(cmd));
    QApplication::restoreOverrideCursor();
    // this is damn slow on windows end
    home = cmd;
    homeIsSet = 1;
  }
  else
  {
    form->textBrowser->setHtml("<html><head></head><body>Sorry no application specific help specified.</body></html>");
  }

  QObject::connect(form->pushButtonBack,SIGNAL(clicked()),form->textBrowser,SLOT(back()));
  QObject::connect(form->pushButtonHome,SIGNAL(clicked()),this,SLOT(slotHome()));
  QObject::connect(form->pushButtonClose,SIGNAL(clicked()),this,SLOT(hide()));
  QObject::connect(form->pushButtonFind,SIGNAL(clicked()),this,SLOT(slotFind()));
}

dlgTextBrowser::~dlgTextBrowser()
{
  delete form;
}

void dlgTextBrowser::slotFind()
{
  bool ok, found;
  form->textBrowser->pageAction(QWebPage::MoveToStartOfDocument) ; //moveCursor(QTextCursor::Start);
  while(1)
  {
    QString text = QInputDialog::getText(this, tr("Find"), tr("String to search for:"), QLineEdit::Normal, findWhat, &ok);
    if(ok && !text.isEmpty())
    {
      findWhat = text;
      found = form->textBrowser->findText(text);
      if(found == false)
      {
        QMessageBox::information(NULL,"pvbrowser","String not found"); 
        hide();
        show();
        return;
      }
    }
    else
    {
      return;
    }
  }
}

void dlgTextBrowser::slotHome()
{
  if(homeIsSet) form->textBrowser->load(QUrl::fromLocalFile(home));
}


