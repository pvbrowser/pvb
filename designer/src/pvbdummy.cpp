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
#include "pvbdummy.h"

PvbDraw::PvbDraw(QWidget *parent) : QLabel(parent)
{
  setFrameShape(QFrame::Box);
  setPixmap(QPixmap(":/pixmaps/pvbdraw.png"));
}

PvbDraw::~PvbDraw()
{
}


PvbIconview::PvbIconview(QWidget *parent) : QLabel(parent)
{
  setFrameShape(QFrame::Box);
  setPixmap(QPixmap(":/pixmaps/pvbiconview.png"));
}

PvbIconview::~PvbIconview()
{
}


PvbImage::PvbImage(QWidget *parent) : QLabel(parent)
{
  setFrameShape(QFrame::Box);
  setPixmap(QPixmap(":/pixmaps/pvbimage.png"));
}

PvbImage::~PvbImage()
{
}


PvbOpengl::PvbOpengl(QWidget *parent) : QLabel(parent)
{
  setFrameShape(QFrame::Box);
  setPixmap(QPixmap(":/pixmaps/pvbopengl.png"));
}

PvbOpengl::~PvbOpengl()
{
}


PvbVtk::PvbVtk(QWidget *parent) : QLabel(parent)
{
  setFrameShape(QFrame::Box);
  setPixmap(QPixmap(":/pixmaps/pvbvtk.png"));
}

PvbVtk::~PvbVtk()
{
}

PvbCustomWidget::PvbCustomWidget(QWidget *parent) : QLabel(parent)
{
  setFrameShape(QFrame::Box);
  setPixmap(QPixmap(":/pixmaps/pvbcustomwidget.png"));
}

PvbCustomWidget::~PvbCustomWidget()
{
}


