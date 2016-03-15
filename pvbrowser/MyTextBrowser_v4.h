/***************************************************************************
                          MyTextBrowser_v4.h  -  description
                             -------------------
    begin                : Fri Jan 15 2016
    copyright            : (C) 2000 by R. Lehrig
                         : M. Bangieff
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
#ifndef _MY_TEXTBROWSER_V4_H_
#define _MY_TEXTBROWSER_V4_H_

#include <QMouseEvent>
#include <QTextBrowser>
#include <QPrinter>
#ifndef NO_WEBKIT
#include <QWebView>
#endif

#ifdef NO_WEBKIT
class MyTextBrowser : public QTextBrowser
#else
class MyTextBrowser : public QWebView
#endif
{
    Q_OBJECT
public:
    MyTextBrowser(int *sock, int ident, QWidget * parent=0, const char * name=0);
    ~MyTextBrowser();
    virtual bool event(QEvent *e);
    void moveContent(int pos);
    void setHTML(QString &text);
    void htmlOrSvgDump(const char *filename);
    void tbSetText(QString &text);
    void tbMoveCursor(int cur);
    void tbScrollToAnchor(QString &text);
    void setSOURCE(QString &temp, QString &text);
    void setZOOM_FACTOR(int factor);
    void PRINT(QPrinter *printer);
    QString home;
    int homeIsSet;
    QString mHeader;
    QString mStyle;
    int xOldScroll, yOldScroll;

public slots:
    void slotLinkClicked(const QUrl &link);
    void slotUrlChanged(const QUrl &link);
    void slotLoadFinished(bool ok);
    void slotPRINTER();

protected:
#ifdef NO_WEBKIT
#else
    virtual QWebView *createWindow(QWebPage::WebWindowType type);
#endif    
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
  
private:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    int *s,id;
    float factor;
    QPoint pressPos;
};

//const char *decode(QString text)

#endif
