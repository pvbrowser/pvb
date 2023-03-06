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
#ifdef PVB_FOOTPRINT_BASIC
#define FOOTPRINT_OHNE
#endif
#ifdef USE_ANDROID
#define FOOTPRINT_OHNE
#endif

#include "pvdefine.h"
#include "dlgtextbrowser.h"
#include "webkit_ui_dlgtextbrowser.h"
#ifndef FOOTPRINT_OHNE
#include <QWebEngineSettings>
#endif
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
#ifndef FOOTPRINT_OHNE
  form->textBrowser->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
#endif

  if(manual == NULL)
  {
    setWindowTitle(tr("pvbrowser Manual"));
  }
  else
  {
    if(strlen(manual) < (int) (sizeof(cmd)-1) ) strcpy(buf,manual);
  }

#ifdef PVWIN32
  ExpandEnvironmentStringsA(buf,cmd,sizeof(cmd)-1);
#else
  strcpy(cmd,buf);
#endif

  QFile fin(cmd);
  if(fin.exists())
  {
    // this is damn slow on windows begin
#ifdef PVDEVELOP
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#ifdef FOOTPRINT_OHNE
    form->textBrowser->setSource(QUrl::fromLocalFile(cmd));
#else
    form->textBrowser->load(QUrl::fromLocalFile(cmd));
#endif    
    QApplication::restoreOverrideCursor();
#endif
    // this is damn slow on windows end
    home = cmd;
    homeIsSet = 1;
  }
  else
  {
    form->textBrowser->setHtml("<html><head></head><body>Sorry no application specific help specified.</body></html>");
  }

  QObject::connect(form->pushButtonFind,SIGNAL(clicked()),this,SLOT(slotFind()));
  QObject::connect(form->lineEditPattern,SIGNAL(returnPressed()),this,SLOT(slotFind()));
  QObject::connect(form->pushButtonClose,SIGNAL(clicked()),this,SLOT(hide()));
  QObject::connect(form->pushButtonHome,SIGNAL(clicked()),this,SLOT(slotHome()));
  QObject::connect(form->pushButtonBack,SIGNAL(clicked()),this,SLOT(slotBack()));
  find = 0;
}

dlgTextBrowser::~dlgTextBrowser()
{
  delete form;
}

void dlgTextBrowser::slotFind()
{
  find = 1;
  QString pattern = form->lineEditPattern->text();
#ifdef FOOTPRINT_OHNE
  form->textBrowser->find(pattern);
#else
  QWebEnginePage *page = form->textBrowser->page();
  if(page == NULL) return;
  //v5diff page->findText(pattern,QWebEnginePage::FindWrapsAroundDocument);
  page->findText(pattern);
#endif
}

void dlgTextBrowser::slotBack()
{
  if(find)
  {
    find = 0;
    return;
  }
#ifdef FOOTPRINT_OHNE
  form->textBrowser->backward();
#else
  form->textBrowser->back();
#endif
}

void dlgTextBrowser::slotHome()
{
#ifdef FOOTPRINT_OHNE
  if(homeIsSet) form->textBrowser->setSource(QUrl(home));
#else
  if(homeIsSet) form->textBrowser->load(QUrl(home));
#endif
}


