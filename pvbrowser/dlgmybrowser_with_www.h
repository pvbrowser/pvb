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
#ifndef TEXT_MY_BROWSER_H
#define TEXT_MY_BROWSER_H
#ifndef NO_WEBKIT

#ifdef USE_WEBKIT
#include <QWebEngineView>
#endif

#include "mainwindow.h"
#include "MyWidgets.h"
#include "dlgmybrowser_ui_with_www.h"

class QNetworkReply;

class dlgMyBrowser: public QWidget
{
    Q_OBJECT

public:
    dlgMyBrowser(int *sock, int ident, QWidget *parent, const char *manual=NULL);
    ~dlgMyBrowser();
    Ui_MyBrowser *form;
    void setUrl(const char *url);
public slots:
    void slotBack();
    void slotHome();
    void slotForward();
    void slotReload();
    void slotFind();
    void slotUrlChanged(const QUrl &url);
    void slotLinkClicked(const QUrl &url);
    void slotTitleChanged(const QString &title);
    void slotUnsupportedContent(QNetworkReply *reply);
    void slotLoadFinished(bool ok);
protected:
    virtual QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
private:
    QString homeurl, homepath, anchor;
    int *s, id;
    MainWindow *mainWindow;
};
#endif
#endif
