/***************************************************************************
                          rlstring.h  -  description
                             -------------------
    begin                : Wed Jan 02 2008
    copyright            : (C) Lehrig Software Enigineering
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_STRING_H_
#define _RL_STRING_H_

#include <string.h>
#include "rldefine.h"

/*! <pre>
class for a simple ANSI-C like string.
</pre> */
class rlString
{
public:
  /*! <pre>
  construct the string
  </pre> */
  rlString(const char *text="");
  rlString(rlString &text);
  rlString(rlString *text);

  /*! <pre>
  destruct the string
  </pre> */
  virtual ~rlString();

  rlString& operator=(const char *s2);
  rlString& operator=(rlString &s2);
  
  rlString& operator+(const char *s2);
  rlString& operator+(rlString &s2);
  
  rlString& operator+=(const char *s2);
  rlString& operator+=(rlString &s2);

  /*! <pre>
  get the text
  </pre> */
  char *text();

  /*! <pre>
  set the text
  </pre> */
  int setText(const char *text);

  /*! <pre>
  printf the text
  </pre> */
  int printf(const char *format, ...);

  /*! <pre>
  append text
  </pre> */
  int cat(const char *text);

  /*! <pre>
  converst string to upper case
  </pre> */
  int upper();

  /*! <pre>
  converst string to upper case
  </pre> */
  int lower();

  /*! <pre>
  test if string starts with startstr
  </pre> */
  int startsWith(const char *startstr);

  /*! <pre>
  case insensitive string compare
  </pre> */
  int strnocasecmp(const char *other);

  /*! <pre>
  case insensitive string compare starting n characters
  </pre> */
  int strnnocasecmp(const char *other, int n);

  /*! <pre>
  strstr()
  </pre> */
  char *strstr(const char *substring);

  /*! <pre>
  strchr()
  </pre> */
  char *strchr(int c);

  /*! <pre>
  strchr()
  </pre> */
  char *strrchr(int c);

private:
  char *txt;
};

#endif
