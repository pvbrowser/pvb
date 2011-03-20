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

#ifdef USE_WEBKIT
#include <QWebFrame>
#endif

#include "pvdefine.h"
#include "dlgmybrowser.h"
#include "dlgmybrowser_ui.h"
#include "opt.h"

extern OPT opt;

pvQWebView::pvQWebView(QWidget *parent)
           :QWebView(parent)
{
}

pvQWebView::~pvQWebView()
{
}

QWebView *pvQWebView::createWindow(QWebPage::WebWindowType type)
{
  QWebHitTestResult r = page()->mainFrame()->hitTestContent(pressPos);
  if(!r.linkUrl().isEmpty() && type == QWebPage::WebBrowserWindow) 
  {
    QString cmd = opt.newwindow;
    if(cmd.isEmpty()) cmd = "pvbrowser";
    cmd += " \"";
    cmd += r.linkUrl().toString();;
    cmd += "\"";
#ifdef PVUNIX
    cmd += " &";
    int ret = system(cmd.toUtf8());
#endif
#ifdef PVWIN32
    int ret = mysystem(cmd.toUtf8());
#endif
    if(ret < 0) printf("ERROR system(%s)\n", (const char *) cmd.toUtf8());
  }
  return NULL;
}

void pvQWebView::mousePressEvent(QMouseEvent *event)
{
  pressPos = event->pos();
  QWebView::mousePressEvent(event);
}

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
  QObject::connect(form->pushButtonFind,SIGNAL(clicked())              ,this, SLOT(slotFind()));
  QObject::connect(form->lineEditPattern,SIGNAL(returnPressed())       ,this, SLOT(slotFind()));
  QObject::connect(form->browser,SIGNAL(urlChanged(const QUrl &))      ,this, SLOT(slotUrlChanged(const QUrl &)));
  QObject::connect(form->browser,SIGNAL(linkClicked(const QUrl &))     ,this, SLOT(slotLinkClicked(const QUrl &)));
  QObject::connect(form->browser,SIGNAL(titleChanged(const QString &)) ,this, SLOT(slotTitleChanged(const QString &)));
  QObject::connect(form->browser,SIGNAL(loadFinished(bool))            ,this, SLOT(slotLoadFinished(bool)));
  //QWebPage *page = form->browser->page();
  //QObject::connect(page,SIGNAL(unsupportedContent(QNetworkReply *)),this, SLOT(slotUnsupportedContent(QNetworkReply *)));
  //enabling plugins leads to problems
  //see: https://bugs.webkit.org/show_bug.cgi?id=56552 that we have reported
  if(opt.enable_webkit_plugins)
  {
    if(opt.arg_debug) printf("enable_webkit_plugins\n");
    form->browser->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    form->browser->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
  }
  else
  {
    if(opt.arg_debug) printf("do not enable_webkit_plugins\n");
  }
#endif
  if(manual == NULL) return;
}

dlgMyBrowser::~dlgMyBrowser()
{
  delete form;
}

QWebView *dlgMyBrowser::createWindow(QWebPage::WebWindowType type)
{
  if(opt.arg_debug) printf("dlgMyBrowser::createWindow type=%d\n", (int) type);
  if(type == QWebPage::WebBrowserWindow)
  {
    QAction *act = form->browser->pageAction(QWebPage::OpenLinkInNewWindow);
    QString str = act->text();
    printf("TODO: find out howto get the url text=%s\n", (const char *) str.toUtf8());
    //act->trigger();
  }  
  return NULL;
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

void dlgMyBrowser::slotFind()
{
#ifdef USE_WEBKIT  
  QString pattern = form->lineEditPattern->text();
  QWebPage *page = form->browser->page();
  if(page == NULL) return;
  page->findText(pattern,QWebPage::FindWrapsAroundDocument);
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
  QStringList list = text.split("#"); // split anchor
  anchor = url.fragment();
  if(opt.arg_debug) printf("dlgMyBrowser::slotLinkClicked url=%s fragment=%s\n", (const char *) text.toUtf8(), (const char *) anchor.toUtf8());
  QString webpath = url.host() + url.path();
  anchor = url.fragment();
  mainWindow->urlComboBox->setEditText(text);
  if(text.startsWith("pv:") || text.startsWith("pvssh:"))
  {
    return mainWindow->slotUrl(text);
  }
  else if(text.endsWith(".pdf") || text.endsWith(".PDF"))
  {
    QString cmd = opt.view_pdf;
    cmd += " ";
    text.replace(" ","%20");
    cmd += text;
#ifndef PVWIN32
    cmd +=  " &";
#endif
    mysystem(cmd.toUtf8());
    return;
  }
  else if(text.endsWith(".mp3",  Qt::CaseInsensitive) ||
          text.endsWith(".ogg",  Qt::CaseInsensitive) || 
          text.endsWith(".m3u",  Qt::CaseInsensitive) || 
          text.endsWith(".asx",  Qt::CaseInsensitive) || 
          text.contains(".pls?", Qt::CaseInsensitive) || 
          text.contains("mp3e",  Qt::CaseInsensitive) || 
          text.startsWith("http://www.youtube.com/watch?") )
  {
     QString cmd = opt.view_audio;
     cmd += " ";
     text.replace(" ","%20");
     cmd += text;
#ifndef PVWIN32
     cmd +=  " &";
#endif
     mysystem(cmd.toUtf8());
  }
  else if(text.endsWith(".mp4",  Qt::CaseInsensitive) || 
          text.endsWith(".mov",  Qt::CaseInsensitive) || 
          text.endsWith(".ogv",  Qt::CaseInsensitive) || 
          text.endsWith(".avi",  Qt::CaseInsensitive) )
  {
     QString cmd = opt.view_video;
     cmd += " ";
     text.replace(" ","%20");
     cmd += text;
#ifndef PVWIN32
     cmd +=  " &";
#endif
     mysystem(cmd.toUtf8());
  }
  else if(webpath == homepath)
  {
    QString u = list.at(0);
    form->browser->setUrl(QUrl(u));
  }
  else
  {
    QString u = list.at(0);
    form->browser->load(QUrl(u));
    homepath = webpath;
  }
#endif
}

void dlgMyBrowser::slotLoadFinished(bool ok)
{
  if(ok == false) return;
  
#ifdef USE_WEBKIT  
  if(opt.arg_debug) printf("dlgMyBrowser::slotLoadFinshed anchor=%s\n",  (const char *) anchor.toUtf8());
#if QT_VERSION >= 0x040700
  if(anchor.length() > 0)
  {
    QWebPage *page = form->browser->page();
    if(page != NULL)
    {
      page->currentFrame()->scrollToAnchor(anchor);
    }
  }
#endif  
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

