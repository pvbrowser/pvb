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
#include <stdio.h>
#include <qglobal.h>

#if QT_VERSION >= 0x040400
#ifndef NO_WEBKIT
#define USE_WEBKIT
#include <QNetworkReply>
#endif
#endif

#include "pvdefine.h"
#include "dlgmybrowser.h"
#include "dlgmybrowser_ui.h"
#include "opt.h"

extern OPT opt;

dlgMyBrowser::dlgMyBrowser(int *sock, int ident, QWidget *parent, const char *manual)
{
  s = sock;
  id = ident;
  mainWindow = (MainWindow *) parent;
  form = new Ui_MyBrowser;
  form->setupUi(this);
#ifdef USE_WEBKIT  
  QObject::connect(form->pushButtonBack,SIGNAL(clicked())              ,this, SLOT(slotBack()));
  QObject::connect(form->pushButtonHome,SIGNAL(clicked())              ,this, SLOT(slotHome()));
  QObject::connect(form->pushButtonForward,SIGNAL(clicked())           ,this, SLOT(slotForward()));
  QObject::connect(form->pushButtonReload,SIGNAL(clicked())            ,this, SLOT(slotReload()));
  QObject::connect(form->browser,SIGNAL(urlChanged(const QUrl &))      ,this, SLOT(slotUrlChanged(const QUrl &)));
  QObject::connect(form->browser,SIGNAL(linkClicked(const QUrl &))     ,this, SLOT(slotLinkClicked(const QUrl &)));
  QObject::connect(form->browser,SIGNAL(titleChanged(const QString &)) ,this, SLOT(slotTitleChanged(const QString &)));
  //QWebPage *page = form->browser->page();
  //QObject::connect(page,SIGNAL(unsupportedContent(QNetworkReply *)),this, SLOT(slotUnsupportedContent(QNetworkReply *)));
#endif
  if(manual == NULL) return;
}

dlgMyBrowser::~dlgMyBrowser()
{
  delete form;
}

void dlgMyBrowser::setUrl(const char *url)
{
  if(url == NULL) return; 
  homeurl = url;
#ifdef USE_WEBKIT
  if(opt.arg_debug) printf("dlgMyBrowser::setUrl url=%s\n", url);
  form->browser->setUrl(QUrl(url));
  form->browser->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
  //form->browser->page()->setLinkDelegationPolicy(QWebPage::DontDelegateLinks);
  homepath = QUrl(url).host() + QUrl(url).path();
#endif
  mainWindow->setTabText("www");
  //mainWindow->setTabText(form->browser->title().toUtf8());
}

void dlgMyBrowser::slotBack()
{
#ifdef USE_WEBKIT
  form->browser->back();
  QString text = form->browser->url().toString();
  mainWindow->urlComboBox->setEditText(text);
#endif
}

void dlgMyBrowser::slotHome()
{
#ifdef USE_WEBKIT  
  form->browser->setUrl(QUrl(homeurl));
  QString text = form->browser->url().toString();
  mainWindow->urlComboBox->setEditText(text);
#endif
}

void dlgMyBrowser::slotForward()
{
#ifdef USE_WEBKIT  
  form->browser->forward();
  QString text = form->browser->url().toString();
  mainWindow->urlComboBox->setEditText(text);
#endif
}

void dlgMyBrowser::slotReload()
{
#ifdef USE_WEBKIT  
  form->browser->reload();
#endif
}

void dlgMyBrowser::slotUrlChanged(const QUrl &url)
{
  if(url.isEmpty()) return;
#ifdef USE_WEBKIT  
  QString text = url.toString();
  int currentTab = mainWindow->currentTab;
  mainWindow->pvbtab[currentTab].url = text;
  mainWindow->urlComboBox->setEditText(text);
  if(opt.arg_debug) printf("dlgMyBrowser::slotUrlChanged url=%s\n", (const char *) text.toUtf8());
  // anchors not working now
  // void QWebFrame::scrollToAnchor ( const QString & anchor )
  // will be introduced in Qt 4.7
#endif
}

void dlgMyBrowser::slotLinkClicked(const QUrl &url)
{
  if(url.isEmpty()) return;
#ifdef USE_WEBKIT  
  QString text = url.toString();
  //printf("link=%s\n", (const char *) text.toUtf8());
  QString webpath = url.host() + url.path();
  //int currentTab = mainWindow->currentTab;
  //mainWindow->pvbtab[currentTab].url = text;
  mainWindow->urlComboBox->setEditText(text);
  if(text.startsWith("pv:") || text.startsWith("pvssh:"))
  {
    return mainWindow->slotUrl(text);
  }
  else if(webpath == homepath)
  {
    form->browser->setUrl(url);
  }
  else
  {
    if(opt.arg_debug) printf("dlgMyBrowser::slotLinkClicked url=%s\n", (const char *) text.toUtf8());
    form->browser->load(url);
    homepath = webpath;
  }
#endif
}

void dlgMyBrowser::slotTitleChanged(const QString &title_in)
{
#ifdef USE_WEBKIT  
  QString title = title_in;
  if(title.length() > MAX_TAB_TEXT_LENGTH)
  {
    title.truncate(MAX_TAB_TEXT_LENGTH);
    title.append("...");
  }
  mainWindow->tabBar->setTabText(mainWindow->currentTab,title);
#endif
}

void dlgMyBrowser::slotUnsupportedContent(QNetworkReply *reply)
{
  if(reply == NULL) return;
#ifdef USE_WEBKIT
  QString text = reply->url().toString();
  if(text.startsWith("pv:") || text.startsWith("pvssh:"))
  {
    return mainWindow->slotUrl(text);
  }
#endif
}

