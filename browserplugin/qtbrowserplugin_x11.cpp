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
#include <QtGui>
#include <QtGui/QX11EmbedWidget>

#include "qtbrowserplugin.h"
#include "qtbrowserplugin_p.h"

#include "qtnpapi.h"

#include <stdlib.h>

static bool ownsqapp = false;
static QMap<QtNPInstance*, QX11EmbedWidget*> clients;

extern "C" bool qtns_event(QtNPInstance *, NPEvent *)
{
    return false;
}

extern "C" void qtns_initialize(QtNPInstance* This)
{
    if (!qApp) {
        ownsqapp = true;
        static int argc = 0;
        static char **argv = {0};

        // Workaround to avoid re-initilaziation of glib
        char* envvar = qstrdup("QT_NO_THREADED_GLIB=1");
        // Unavoidable memory leak; the variable must survive plugin unloading
        ::putenv(envvar);

        (void)new QApplication(argc, argv);
    }
    if (!clients.contains(This)) {
        QX11EmbedWidget* client = new QX11EmbedWidget;
        QHBoxLayout* layout = new QHBoxLayout(client);
        layout->setMargin(0);
        clients.insert(This, client);
    }
}

extern "C" void qtns_destroy(QtNPInstance* This)
{
    QMap<QtNPInstance*, QX11EmbedWidget*>::iterator it = clients.find(This);
    if (it == clients.end())
        return;
    delete it.value();
    clients.erase(it);
}

extern "C" void qtns_shutdown()
{
    if (clients.count() > 0) {
        QMap<QtNPInstance*, QX11EmbedWidget*>::iterator it = clients.begin();
        while (it != clients.end()) {
            delete it.value();
            ++it;
        }
        clients.clear();
    }

    if (!ownsqapp)
        return;

    // check if qApp still runs widgets (in other DLLs)
    QWidgetList widgets = qApp->allWidgets();
    int count = widgets.count();
    for (int w = 0; w < widgets.count(); ++w) {
        // ignore all Qt generated widgets
        QWidget *widget = widgets.at(w);
        if (widget->windowFlags() & Qt::Desktop)
            count--;
    }
    if (count) // qApp still used
        return;

    delete qApp;
    ownsqapp = false;
}

extern "C" void qtns_embed(QtNPInstance *This)
{
    Q_ASSERT(qobject_cast<QWidget*>(This->qt.object));

    QMap<QtNPInstance*, QX11EmbedWidget*>::iterator it = clients.find(This);
    if (it == clients.end())
        return;
    QX11EmbedWidget* client = it.value();
    This->qt.widget->setParent(client);
    client->layout()->addWidget(This->qt.widget);
    client->embedInto(This->window);
    client->show();
}

extern "C" void qtns_setGeometry(QtNPInstance *This, const QRect &rect, const QRect &)
{
    Q_ASSERT(qobject_cast<QWidget*>(This->qt.object));

    QMap<QtNPInstance*, QX11EmbedWidget*>::iterator it = clients.find(This);
    if (it == clients.end())
        return;
    QX11EmbedWidget* client = it.value();
    client->setGeometry(QRect(0, 0, rect.width(), rect.height()));
}

/*
extern "C" void qtns_print(QtNPInstance * This, NPPrint *printInfo)
{
    NPWindow* printWindow = &(printInfo->print.embedPrint.window);
    void* platformPrint = printInfo->print.embedPrint.platformPrint;
    // #### Nothing yet.
}
*/
