/***************************************************************************
                          rlfileload.h  -  description
                             -------------------
    begin                : Fri Jul 28 2006
    copyright            : (C) 2006 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_FILE_LOAD_H_
#define _RL_FILE_LOAD_H_

#include "rldefine.h"
#include "rlstring.h"

typedef struct _rlFileLines_
{
  char *line;
  struct _rlFileLines_ *next;
}rlFileLines;

/*! <pre>
This class loads a text file to memory.
Then you can iterate to it's lines.
</pre> */
class rlFileLoad
{
  public:
    rlFileLoad();
    virtual ~rlFileLoad();
    int load(const char *filename);
    void unload();
    const char *firstLine();
    const char *nextLine();
    void setDebug(int state);
    int text2rlstring(rlString &rlstring);
  private:
    int loaded;
    int debug;
    rlFileLines file_lines;
    rlFileLines *current_line;
};

#endif
