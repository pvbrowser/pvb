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
#ifndef OPT_H
#define OPT_H

#define MAX_PRINTF_LENGTH 1024
#define MAXOPT 1024

// add additional language here
#define PV_PYTHON 1
#define PV_PERL   2
#define PV_PHP    3
#define PV_TCL    4

typedef struct
{
  // inifile args
  char manual[MAXOPT];
  int  xGrid;       // grid for graphical designer
  int  yGrid;
  int  murx;        // option for testing pvdevelop
  int  ctrlPressed; // ctrl key pressed
  int  su;          // run pvs as root
  char backupLocation[MAXOPT];

  // options from project file
  char target[MAXOPT];
  int  xmax;
  int  ymax;
  int  script;

  // command line args
  int  arg_debug;
  char arg_project[MAXOPT];
  char arg_action[MAXOPT];
  int  arg_mask_to_generate;
}OPT;

const char *inifile();
const char *readIniFile();

#endif
