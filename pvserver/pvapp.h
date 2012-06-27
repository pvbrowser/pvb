/***************************************************************************
                          pvapp.h  -  description
                             -------------------
    begin                : Sun Nov 12 2000
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
#ifndef _PVAPP_H_
#define _PVAPP_H_

#include "processviewserver.h"

int show_mask1(PARAM *p);
int show_mask2(PARAM *p);
int show_mask3(PARAM *p);
int show_mask4(PARAM *p);
int show_periodic(PARAM *p);
int show_maskvtk(PARAM *p);
int show_qwt(PARAM *p);
int show_modal1(PARAM *p);

#endif
