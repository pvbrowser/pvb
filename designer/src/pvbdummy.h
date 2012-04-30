/****************************************************************************
**
** Copyright (C) 2000-2008 Lehrig Software Engineering.
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
#ifndef PVB_DUMMY_H
#define PVB_DUMMY_H

#include <QLabel>

class PvbDraw : public QLabel
{
  Q_OBJECT
  public:
    PvbDraw(QWidget * parent=0 );
    ~PvbDraw();
};

class PvbIconview : public QLabel
{
  Q_OBJECT
  public:
    PvbIconview(QWidget * parent=0);
    ~PvbIconview();
};

class PvbImage : public QLabel
{
  Q_OBJECT
  public:
    PvbImage(QWidget * parent=0);
    ~PvbImage();
};

class PvbOpengl : public QLabel
{
  Q_OBJECT
  public:
    PvbOpengl(QWidget * parent=0);
    ~PvbOpengl();
};

class PvbVtk : public QLabel
{
  Q_OBJECT
  public:
    PvbVtk(QWidget * parent=0);
    ~PvbVtk();
};

class PvbCustomWidget : public QLabel
{
  Q_OBJECT
  public:
    PvbCustomWidget(QWidget * parent=0);
    ~PvbCustomWidget();
};

#endif
