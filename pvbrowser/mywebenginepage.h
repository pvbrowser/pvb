/***************************************************************************
                         mywebenginepage.h  -  description
                             -------------------
    begin                : Mon Sep 09 2019
    copyright            : (C) 2000 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _MYWEBENGINEPAGE_H_
#define _MYWEBENGINEPAGE_H_

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QUrl>
#include "pvdefine.h"
#include "tcputil.h"

class MyWebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    MyWebEnginePage(int *sock, int ident, QObject *parent);
    virtual ~MyWebEnginePage();
protected:
    void slotLinkClicked(const QUrl &link);
    virtual bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
private:
    int *s,id;
};

#endif

