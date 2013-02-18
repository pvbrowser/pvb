/****************************************************************************
**
** Copyright (C) 2000-2013 Lehrig Software Engineering.
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

#include <sys/types.h>
#include <sys/stat.h>
#include "rlfileload.h"
#include "rlspreadsheet.h"
#include "rlstring.h"
#include "rlcutil.h"

extern FILE *fout;
extern rlFileLoad fpro;

extern int n_sources;
extern int n_headers;
extern int n_libs;
extern int n_includepath;
extern int n_defines;
extern int n_qmake_lflags;
extern int n_config;
extern int n_cflags;
extern int n_cxxflags;

extern rlSpreadsheetRow row_sources;
extern rlSpreadsheetRow row_headers;
extern rlSpreadsheetRow row_libs;
extern rlSpreadsheetRow row_includepath;
extern rlSpreadsheetRow row_defines;
extern rlSpreadsheetRow row_qmake_lflags;
extern rlSpreadsheetRow row_config;
extern rlSpreadsheetRow row_cflags;
extern rlSpreadsheetRow row_cxxflags;

extern rlString str_target;
extern rlString str_template;

extern rlString line;
extern rlString cmdline;

extern const char *builddir;;
extern char destination[];

int interpretProFile();
int getItemsFromLine(int iitem); // items := sources | includes | libs | includepath | defines
int writeMakefile();

