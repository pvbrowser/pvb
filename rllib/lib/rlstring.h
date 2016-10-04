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

extern const char rlCRLF[];

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
  rlString(const rlString &text);

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

  int       operator==(const char *s2);
  int       operator==(rlString &s2);
  int       operator==(const rlString &s2);

  int       operator!=(const char *s2);
  int       operator!=(rlString &s2);

  /*! <pre>
  get the text
  </pre> */
  char *text();
  char *text() const;

  /*! <pre>
  set the text
  </pre> */
  int setText(const char *text);

  /*! <pre>
  printf the text
  </pre> */
  int printf(const char *format, ...);

  /*! <pre>
  copy text
  </pre> */
  int strcpy(const char *text);

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

  /*! <pre>
  Remove quotas around a string.
  This might be usefull together with CSV files.
  </pre> */
  int removeQuotas(char c='"');

  /*! <pre>
  Remove "\n" at end of string.
  </pre> */
  int removeNewline();

  /*! <pre>
  Read string from file.
  </pre> */
  int read(const char *filename);

  /*! <pre>
  Write string to file.
  </pre> */
  int write(const char *filename);

  /*! <pre>
  convert to platform independent filename
  </pre> */
  const char *toFilename();

  /*! <pre>
  convert to platform independent directoryname
  </pre> */
  const char *toDirname();

private:
  char *txt;
  char *tmp;
};

#endif
