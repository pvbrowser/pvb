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
#include "../pvbrowser/pvdefine.h"
#include "syntax.h"
#include <ctype.h>
#include <stdio.h>

const char *cKeywords[] = {
  "break", "case", "continue", "default", "do", "else", "enum", "extern",
  "for", "goto", "if", /*"interrupt",*/  "return", "sizeof", "struct",
  "switch", "typedef", "union", "while", 0L};

const char *cTypes[] = {
  "auto", "char", "const", "double", "float", "int", "long", "register",
  "short", "signed", "static", "unsigned", "void", "volatile", 0L};

// ISO/IEC 14882:1998 . Sec. 2.11.1 (aka ANSI C++)
// keyword "const" (apart from a type spec.) is also a keyword, so it is named inside this array
// what about typeof?
const char *cppKeywords[] = {
  "asm", "catch", "class", "const_cast", "const", "delete", "dynamic_cast",
  "explicit", "export", "false", "friend", "inline", "namespace", "new",
  "operator", "private", "protected", "public", "reinterpret_cast",
  "static_cast", "template", "this", "throw", "true", "try", "typeid",
  "typename", "using", "virtual",
  // alternative representations  (these words are reserved and shall not be used otherwise)
  //  ISO/IEC 14882:1998 . Sec. 2.11.2
  "and_eq", "and", "bitand", "bitor", "compl", "not_eq", "not", "or_eq", "or",
  "xor_eq", "xor", 0L};

const char *cppTypes[] = {
  "bool", "wchar_t", "mutable", 0L};

// add additional language here
const char *pyKeywords[] = {
  "import", "from", "as", "class", "def", "del", "global", "lambda", "and", "assert",
  "in", "is", "not", "or", "exec", "print", "break", "continue", "elif", "else", "except",
  "finally", "for", "if", "pass", "raise", "return", "try", "while", "yield", "__import__",
  "abs", "all", "any", "apply", "basestring", "bool", "buffer", "callable", "chr", "classmethod",
  "cmp", "coerce", "compile", "complex", "delattr", "dict", "dir", "divmod", "enumerate", "eval",
  "execfile", "file", "filter", "float", "frozenset", "getattr", "globals", "hasattr", "hash",
  "hex", "input", "int", "intern", "isinstance", "issubclass", "iter", "len", "list",
  "locals", "long", "map", "max", "min", "object", "oct", "open", "ord", "pow", "property", "range",
  "raw_input", "reduce", "reload", "repr", "reversed", "round", "set", "setattr", "slice", "sorted",
  "staticmethod", "str", "sum", "super", "tuple", "type", "unichr", "unicode", "vars", "xrange", "zip",
  "None", "self", "True", "False", "NotImplemented", "Ellipsis", "SIGNAL", "SLOT", "connect", 0L};
  // "id" removed

const char *luaKeywords[] = {
  "local", "in", "break", "while", "until", "for", "elseif", "if", "else", "then", "do", "end", "repeat", "return", 
  "function", "and", "or", "not", "nil", "true", "false", 0L};

const char *luaLibs[] = {
  "string", "os", "file", "io", "math", "package", "table", "debug", "coroutine", "pv", "rllib",
  "assert", "collectgarbage", "dofile", "error", "getfenv", "getmetatable", "ipairs", "load", "loadfile", "loadstring", "module", "next", "pairs",
  "pcall", "print", "rawequal", "rawget", "rawset", "require", "select", "setfenv", "setmetatable", "tonumber", "tostring", "type", "unpack", "xpcall", 
  0L};

const char *luaMine[] = {
  "p", "id", "x", "y", "width", "height", "text", "button", "modifier", "enter", "val", 0L};

syntax::syntax(QTextEdit *edit) 
       :QSyntaxHighlighter(edit)
{
  e = edit;
}

syntax::~syntax()
{
}

int syntax::setSyntax(int language)
{
  lang_syntax = CPP_SYNTAX;
  // add additional language here
  if(language == PYTHON_SYNTAX) lang_syntax = language;
  if(language == PERL_SYNTAX)   lang_syntax = language;
  if(language == PHP_SYNTAX)    lang_syntax = language;
  if(language == TCL_SYNTAX)    lang_syntax = language;
  if(language == LUA_SYNTAX)    lang_syntax = language;
  return 0;
}

void syntax::highlightBlock(const QString &text)
{
  char buf[1024];
  int start;
  int i,len;
  QColor red(255,0,0); 
  QColor green(0,255,0); 
  QColor blue(0,0,255);
  QColor grey(128,128,128);
  QColor black(0,0,0);

  if(text.isNull()) return;
  if(text.length() >= (int) sizeof(buf)) return;
  strcpy(buf,text.toUtf8());

  setFormat(0,sizeof(buf)-1,black);
  if(lang_syntax == CPP_SYNTAX)
  {
  //##########################################################################
  for(i=0; cKeywords[i] != 0L; i++)
  {
    len = strlen(cKeywords[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],cKeywords[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '(' || buf[start+len] == ':' || buf[start+len] == ';'))
      {
        if(start == 0)                 setFormat(start,len,red);
        else if(isspace(buf[start-1])) setFormat(start,len,red);
      }
    }
  }
  //##########################################################################
  for(i=0; cppKeywords[i] != 0L; i++)
  {
    len = strlen(cppKeywords[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],cppKeywords[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '(' || buf[start+len] == ':' || buf[start+len] == ';'))
      {
        if(start == 0)                 setFormat(start,len,red);
        else if(isspace(buf[start-1])) setFormat(start,len,red);
      }
    }
  }
  //##########################################################################
  for(i=0; cTypes[i] != 0L; i++)
  {
    len = strlen(cTypes[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],cTypes[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '(' || buf[start+len] == ':' || buf[start+len] == ';'))
      {
        if(start == 0)                 setFormat(start,len,blue);
        else if(isspace(buf[start-1])) setFormat(start,len,blue);
        else if(buf[start-1] == '(')   setFormat(start,len,blue);
      }
    }
  }
  //##########################################################################
  for(i=0; cppTypes[i] != 0L; i++)
  {
    len = strlen(cppTypes[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],cppTypes[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '(' || buf[start+len] == ':' || buf[start+len] == ';'))
      {
        if(start == 0)                 setFormat(start,len,blue);
        else if(isspace(buf[start-1])) setFormat(start,len,blue);
        else if(buf[start-1] == '(')   setFormat(start,len,blue);
      }
    }
  }
  //##########################################################################
  for(start=0; buf[start] != '\0'; start++)
  {
    if(buf[start] == '/' && buf[start+1] == '/')
    {
      setFormat(start,strlen(&buf[start]),grey);
      return;
    }
    else if(buf[start] == '(' || buf[start] == ')')
    {
      setFormat(start,1,blue);
    }
    else if(buf[start] == '[' || buf[start] == ']')
    {
      setFormat(start,1,blue);
    }
    else if(buf[start] == '{' || buf[start] == '}')
    {
      setFormat(start,1,blue);
    }
  }
  }

  // add additional language here
  if(lang_syntax == PYTHON_SYNTAX)
  {
  //##########################################################################
  for(i=0; pyKeywords[i] != 0L; i++)
  {
    len = strlen(pyKeywords[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],pyKeywords[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '(' || buf[start+len] == ')' || buf[start+len] == ':' || buf[start+len] == ',' || buf[start+len]=='.'))
      {
        if(start == 0) setFormat(start,len,red);
        else if(isspace(buf[start-1]) || buf[start-1] == '(' || buf[start-1] == ')' || buf[start-1] == ',') setFormat(start,len,red);
      }
    }
  }
  //##########################################################################
  for(start=0; buf[start] != '\0'; start++)
  {
    if(buf[start] == '#')
    {
      setFormat(start,strlen(&buf[start]),grey);
      return;
    }
    else if(buf[start] == '(' || buf[start] == ')')
    {
      setFormat(start,1,blue);
    }
    else if(buf[start] == '[' || buf[start] == ']')
    {
      setFormat(start,1,blue);
    }
    else if(buf[start] == '{' || buf[start] == '}')
    {
      setFormat(start,1,blue);
    }
    else if(buf[start] == '\"' ||  buf[start] == '\'')
    {
      len = 1;
      i = start+1;
      while(buf[i] != '\0')
      {
        if(buf[i] == '\"' ||  buf[i] == '\'')
        {
          len++;
          break;
        }
        else
        {
          len++;
        }
        i++;
      }
      setFormat(start,len,green);
      start += len;
    }
  }
  }

  if(lang_syntax == PERL_SYNTAX)
  {
  }

  if(lang_syntax == PHP_SYNTAX)
  {
  }

  if(lang_syntax == TCL_SYNTAX)
  {
  }

  if(lang_syntax == LUA_SYNTAX)
  {
  //##########################################################################
  for(i=0; luaKeywords[i] != 0L; i++)
  {
    len = strlen(luaKeywords[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],luaKeywords[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '(' || buf[start+len] == ':' || buf[start+len] == '\0'))
      {
        if(start == 0)                 setFormat(start,len,red);
        else if(isspace(buf[start-1])) setFormat(start,len,red);
      }
    }
  }
  //##########################################################################
  for(i=0; luaLibs[i] != 0L; i++)
  {
    len = strlen(luaLibs[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],luaLibs[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '(' || buf[start+len] == '.' || buf[start+len] == ':' || buf[start+len] == '\0'))
      {
        if(start == 0)                 setFormat(start,len,blue);
        else if(isspace(buf[start-1])) setFormat(start,len,blue);
        else if(buf[start-1] == '(')   setFormat(start,len,blue);
        else if(buf[start-1] == ',')   setFormat(start,len,blue);
      }
    }
  }
  //##########################################################################
  for(i=0; luaMine[i] != 0L; i++)
  {
    len = strlen(luaMine[i]);
    for(start=0; buf[start] != '\0'; start++)
    {
      if(strncmp(&buf[start],luaMine[i],len) == 0 && (isspace(buf[start+len]) || buf[start+len] == '.' || buf[start+len] == ',' || buf[start+len] == ')'))
      {
        if(start == 0)                 setFormat(start,len,green);
        else if(isspace(buf[start-1])) setFormat(start,len,green);
        else if(buf[start-1] == '(')   setFormat(start,len,green);
        else if(buf[start-1] == ',')   setFormat(start,len,green);
      }
    }
  }
  //##########################################################################
  for(start=0; buf[start] != '\0'; start++)
  {
    if(buf[start] == '-' && buf[start+1] == '-')
    {
      setFormat(start,strlen(&buf[start]),grey);
      return;
    }
    else if(buf[start] == '(' || buf[start] == ')')
    {
      setFormat(start,1,blue);
    }
    else if(buf[start] == '[' || buf[start] == ']')
    {
      setFormat(start,1,blue);
    }
    else if(buf[start] == '{' || buf[start] == '}')
    {
      setFormat(start,1,blue);
    }
  }
  //##########################################################################
  }


  //##########################################################################
  // highlight strings
  i = 0;
  while(buf[i] != '\0')
  {
    if(buf[i] == '\"')
    {
      start = i;
      i++;
      while(buf[i] != '\"' || buf[i-1] == '\\')
      {
	if(buf[i] == '\0') return;
	i++;
      }
      i++;
      setFormat(start,i-start,green);
    }
    else
    {
      i++;
    }  
  }	  
  return;
}
