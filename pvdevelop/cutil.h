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
#ifndef CUTIL_H
#define CUTIL_H

int  action(const char *command);
int  mysystem(const char *command);
void generateInitialProject(const char *name);
void generateInitialMask(int imask);
void generateInitialSlots(int imask);
void addMaskToProject(const char *name, int imask);
void addMaskToMain(int imask);
void addMaskToHeader(int imask);
void lua_generateInitialProject(const char *name);
void lua_generateInitialMask(int imask);
void lua_generateInitialSlots(int imask);
void lua_addMaskToMain(int imask);
void uncommentRllib(const char *project);
void uncommentModbus();
void uncommentSiemenstcp();
void uncommentPpi();
int  writeStartscript(const char *dir, const char *name);
int  export_ui(int imask);

typedef struct _FileLines_
{
  char *line;
  struct _FileLines_ *next;
}FileLines;

int loadFile(const char *name);
int unloadFile();

#endif
