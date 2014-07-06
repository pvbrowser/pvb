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
#include "pvdefine.h"
#include <QtGui>
#include <QTimer>
#include "mainwindow.h"
#include <qtbrowserplugin.h>

QTNPFACTORY_BEGIN("pvbrowser Plugin", "plugin for the pv:// protocol See: http://pvbrowser.org")
    QTNPCLASS(MainWindow)
QTNPFACTORY_END()

#ifdef QAXSERVER
#include <ActiveQt/QAxFactory>
QAXFACTORY_BEGIN("{aa3216bf-7e20-482c-84c6-06167bacb616}", "{08538ca5-eb7a-4f24-a3c4-a120c6e04dc4}")
    QAXCLASS(MainWindow)
QAXFACTORY_END()
#endif
