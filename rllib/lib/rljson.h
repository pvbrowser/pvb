/***************************************************************************
                          rljson.h  -  description
                             -------------------
    begin                : Sat Jul 29 2023
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
#ifndef _RL_JSON_H_
#define _RL_JSON_H_

#include "rldefine.h"
#include "rlstring.h"

extern const char rlCRLF[];

/*! <pre>
We assume that a JSON text line is well formed and in the following syntax
</pre> */
typedef struct
{
  int startsWithText;         // "text"
  int startsWithOpenBracket;  // {
  int startsWithCloseBracket; // }
  int startsWithEndArray;     // ]
  rlString text;      // white space in front of the tag name is ignored
  int optColon;       // a colon must follow when text is a tag
  int optOpenBracket; // a openBracket { might follow
  int optBeginArray;  // a beginArray [ might follow
  int optParamType;   // 0=default=text 1=integer 2=float
  rlString param;     // the parameter in text form
  int   intParam;     // 0   if there is no integer number
  float floatParam;   // 0.0 if there is no floating point number
  int optComma;       // tells us that more elements follow
}rlJSONrecord;

/*! <pre>
class for a simple JSON
</pre> */
class rlJSON
{
public:
  /*! <pre>
  construct the class
  </pre> */
  rlJSON();

  /*! <pre>
  destruct the class
  </pre> */
  virtual ~rlJSON();

  /*! <pre>
  init a rlJSONrecord
  </pre> */
  int initRecord(rlJSONrecord *record);

  /*! <pre>
  interpret a JSON formatted line of text
  </pre> */
  int interpret(const char *line, rlJSONrecord *record);

  /*! <pre>
  print a JSON formatted line of text
  </pre> */
  int print(FILE *fout, rlJSONrecord *record, const char *indent="");
};

#endif
