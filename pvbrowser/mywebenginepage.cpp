/***************************************************************************
                         mywebenginepage.cpp  -  description
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
#include "mywebenginepage.h"
#include "opt.h"
extern OPT opt;

static const char *decode(QString text)
{
  static char buf[MAX_PRINTF_LENGTH];
  int maxlen = MAX_PRINTF_LENGTH - 1;

  if(opt.codec == pvbUTF8)
  {
    strncpy(buf, text.toUtf8(), maxlen);
  }
  else
  {
#if QT_VERSION < 0x050000 
    strncpy(buf, text.toAscii(), maxlen);
#endif    
  }
  buf[maxlen-1] = '\0';
  return buf;
}

MyWebEnginePage::MyWebEnginePage(int *sock, int ident, QObject *parent)
                :QWebEnginePage(parent)
{
  if(opt.arg_debug) printf("MyWebEnginePage()\n");
  s = sock;
  id = ident;
}

MyWebEnginePage::~MyWebEnginePage()
{
  if(opt.arg_debug) printf("~MyWebEnginePage()\n");
}

void MyWebEnginePage::slotLinkClicked(const QUrl &link)
{
  char buf[MAX_PRINTF_LENGTH];
  QString url;
  int i;

  url = link.toString();

  // replace "href=\"//awite://" by "href=/"
  while(1)
  {
    i = url.indexOf("awite://");
    if(i < 0) break;
    url = url.replace(i,8,"/");
    if(opt.arg_debug) printf("MyTextBrowser::slotLinkClicked::link clicked = %s\n", (const char *) url.toUtf8());
  }

  if(opt.arg_debug) printf("slotLinkClicked(%s)\n", (const char *) url.toUtf8());
  if(url.startsWith("http://") && (url.endsWith(".pdf") || url.endsWith(".PDF")))
  {
    QString cmd = opt.view_pdf;
    cmd += " ";
    url.replace(" ","%20");
    cmd += url;
//#ifndef PVWIN32
//    cmd +=  " &";
//#endif
    mysystem(cmd.toUtf8());
  }
  else if(
         !url.startsWith("http://audio.") &&
          url.startsWith("http://") && (
          url.endsWith(".mp3",  Qt::CaseInsensitive) || 
          url.endsWith(".ogg",  Qt::CaseInsensitive) || 
          url.endsWith(".m3u",  Qt::CaseInsensitive) || 
          url.endsWith(".asx",  Qt::CaseInsensitive) || 
          url.contains(".pls?", Qt::CaseInsensitive) ||
          url.contains("mp3e",  Qt::CaseInsensitive) ||
          url.startsWith("http://www.youtube.com/watch?") ))
  {
    QString cmd = opt.view_audio;
    cmd += " ";
    url.replace(" ","%20");
    cmd += url;
//#ifndef PVWIN32
//    cmd +=  " &";
//#endif
    mysystem(cmd.toUtf8());
  }
  else if(
         !url.startsWith("http://video.") &&
          url.startsWith("http://") && (
          url.endsWith(".mp4",  Qt::CaseInsensitive) || 
          url.endsWith(".mov",  Qt::CaseInsensitive) || 
          url.endsWith(".ogv",  Qt::CaseInsensitive) || 
          url.endsWith(".avi",  Qt::CaseInsensitive) ))
  {
    QString cmd = opt.view_video;
    cmd += " ";
    url.replace(" ","%20");
    cmd += url;
//#ifndef PVWIN32
//    cmd +=  " &";
//#endif
    mysystem(cmd.toUtf8());
  }
  else
  {
    if(url.length()+40 > MAX_PRINTF_LENGTH) return;
    sprintf(buf,"text(%d,\"%s\")\n", id,decode(url));
    tcp_send(s,buf,strlen(buf));
  }  
}

bool MyWebEnginePage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
  QString surl = url.toString();
  if(opt.arg_debug) printf("acceptNavigationRequest isMainFrame=%d type=%d url=%s\n",
                                                    isMainFrame,   type,   (const char *) surl.toUtf8());
  if(isMainFrame == 1 && type == 0) 
  {
    if(opt.arg_debug) printf("use by pvbrowser\n");
    slotLinkClicked(url);
    return false;
  }
  return true;
}

