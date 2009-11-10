// to be done
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
#ifndef TCL_H
#define TCL_H

int generateTcl(int imask, QWidget *root)
{
  if(imask < 1)    return -1;
  if(root == NULL) return -1;

  // if main module not exists -> generate it
  // generate this mask

  printf("generateTcl not implemented jet\n");

  return 0;
}

#endif
