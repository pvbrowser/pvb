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
#ifndef _SYNTAX_H_
#define _SYNTAX_H_
#include <qsyntaxhighlighter.h>
#include <qtextedit.h> 

#define CPP_SYNTAX    0

// add additional language here
#define PYTHON_SYNTAX 1
#define PERL_SYNTAX   2
#define PHP_SYNTAX    3
#define TCL_SYNTAX    4
#define LUA_SYNTAX    5

class syntax: public QSyntaxHighlighter
{
public:  
    syntax(QTextEdit *edit);
    ~syntax();
    int setSyntax(int language);
private:
    QTextEdit *e;
    void highlightBlock(const QString &text);
    int lang_syntax;
};
#endif
