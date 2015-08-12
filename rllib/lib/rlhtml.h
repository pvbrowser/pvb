/***************************************************************************
                           rlhtml.h  -  description
                             -------------------
    begin                : Thu Jul 30 2015
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
#ifndef _RL_HTML_H_
#define _RL_HTML_H_

#include "rldefine.h"
#include "rlstring.h"

/*! <pre>
conveniance base class for html
</pre> */
class rlHtml
{
public:
  rlHtml();
  virtual ~rlHtml();

  /*! <pre>
  html can be configured with rlhtml.css
  </pre> */
  const char *htmlHeader();
  const char *htmlTrailer();
  /*! <pre>
  html representation of text file
  </pre> */
  int textFile(const char *filename, rlString &text);
  /*! <pre>
  hexdump representation of text file
  </pre> */
  int hexdumpFile(const char *filename, rlString &text);
};

#endif
