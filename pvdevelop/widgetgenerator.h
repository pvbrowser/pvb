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
#ifndef WIDGET_GENERATOR_H
#define WIDGET_GENERATOR_H
#include <stdio.h>
class QWidget;
class Designer;

int importUi(const char *uifile, Designer *designer);
int generateMask(const char *filename, QWidget *root);
int getWidgetsFromMask(const char *filename, QWidget *root);
int generatePython(int imask, QWidget *root);
int generatePHP(int imask, QWidget *root);
int generatePerl(int imask, QWidget *root);
int generateTcl(int imask, QWidget *root);
int generateLua(int imask, QWidget *root);
int drawDrawWidgets(QWidget *root);

#endif
