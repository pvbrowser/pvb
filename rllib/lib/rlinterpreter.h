/***************************************************************************
                          rlinterpreter.h  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_INTERPRETER_H_
#define _RL_INTERPRETER_H_

#include "rldefine.h"

/*! <pre>
A class for interpreting text.
</pre> */
class rlInterpreter
{
public:
  rlInterpreter(int Maxline=rl_PRINTF_LENGTH);
  virtual ~rlInterpreter();

  char *line;
  int  isCommand(const char *command);
  void copyStringParam(char *destination, int index);
  int  maxchar();

private:
  int maxline;
};

#endif
