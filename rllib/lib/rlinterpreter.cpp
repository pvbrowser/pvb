/***************************************************************************
                          rlinterpreter.cpp  -  description
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
#include <stdlib.h>
#include "rlinterpreter.h"

rlInterpreter::rlInterpreter(int Maxline)
{
  line = NULL;
  maxline = Maxline;
  if(maxline <= 0) return;
  line = new char[maxline];
}

rlInterpreter::~rlInterpreter()
{
  //if(line != NULL) delete [] line;
}

int  rlInterpreter::isCommand(const char *command)
{
  int i = 0;
  while(command[i] != '\0')
  {
    if(command[i] != line[i]) return 0;
    i++;
  }
  return 1;
}

void rlInterpreter::copyStringParam(char *destination, int index)
{
  int iparen = 0;
  int i = 0;
  int ndest;
  *destination = '\0';
  // find " number index*2
  while(line[i] != '\0')
  {
    if(line[i] == '\"' && (i > 0 || line[i-1] != '\\')) iparen++;
    if(iparen == (2*index + 1))
    {
      ndest = 0;
      i++;
      while(line[i] != '\0' && (line[i] != '\"' && line[i-1] != '\\'))
      {
        if(ndest >= maxline-1)
        {
          *destination = '\0';
          return;
        }
        *destination++ = line[i++];
        ndest++;
      }
      *destination = '\0';
      return;
    }
    i++;
  }
}

int rlInterpreter::maxchar()
{
  return maxline-1;
}

