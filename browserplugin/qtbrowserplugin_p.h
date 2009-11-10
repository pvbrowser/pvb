/****************************************************************************
**
** Copyright (C) 2003-2008 Trolltech ASA. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** Trolltech sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include <QtCore/QVariant>
#include <QtCore/QMutexLocker>
#include <QtGui/QWidget>

#ifdef Q_WS_X11
#   include <X11/Xlib.h>

class QtNPStream;
class QtNPBindable;
#endif

struct QtNPInstance
{
    NPP npp;

    short fMode;

#ifdef Q_WS_WIN
    typedef HWND Widget;
#endif
#ifdef Q_WS_X11
    typedef Window Widget;
    Display *display;
#endif
#ifdef Q_WS_MAC
    typedef NPPort* Widget;
    QWidget *rootWidget;
#endif

    Widget window;

    QRect geometry;
    QString mimetype;
    QByteArray htmlID;
    union {
        QObject* object;
        QWidget* widget;
    } qt;
    QtNPStream *pendingStream;
    QtNPBindable* bindable;
    QObject *filter;

    QMap<QByteArray, QVariant> parameters;

    qint32 notificationSeqNum;
    QMutex seqNumMutex;
    qint32 getNotificationSeqNum()
        {
            QMutexLocker locker(&seqNumMutex);

            if (++notificationSeqNum < 0)
                notificationSeqNum = 1;
            return notificationSeqNum;
        }

};
