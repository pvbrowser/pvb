/***************************************************************************
                          rlfileload.cpp  -  description
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
#include "rlfileload.h"
#include <stdio.h>
#include <string.h>

rlFileLoad::rlFileLoad()
{
  loaded = debug = 0;
  file_lines.line = NULL;
  file_lines.next = NULL;
  current_line = NULL;
}

rlFileLoad::~rlFileLoad()
{
  unload();
}

int rlFileLoad::load(const char *filename)
{
  FILE *fin;
  rlFileLines *fl;
  char line[rl_PRINTF_LENGTH], *cptr;

  unload();
  fin = fopen(filename,"r");
  if(fin == NULL) return -1;

  fl = &file_lines;
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    cptr = strchr(line,'\n');
    if(cptr != NULL) *cptr = '\0';
    cptr = strchr(line,0x0D);
    if(cptr != NULL) *cptr = '\0';
    if(debug) printf("rlFileLoad::load line=%s\n",line);
    fl->next = new rlFileLines;
    fl = fl->next;
    fl->line = new char [strlen(line)+1];
    strcpy(fl->line,line);
    fl->next = NULL;
  }

  fclose(fin);
  loaded = 1;
  return 1;
}

void rlFileLoad::unload()
{
  rlFileLines *fl,*flold;

  if(loaded == 0) return;
  fl = &file_lines;
  fl = fl->next;
  while(fl != NULL)
  {
    if(debug) printf("rlFileLoad::unload line=%s",fl->line);
    delete [] fl->line;
    flold = fl;
    fl = fl->next;
    delete flold;
  }
  loaded = 0;
}

const char *rlFileLoad::firstLine()
{
  if(loaded == 0) return NULL;
  current_line = &file_lines;
  current_line = current_line->next;
  if(current_line == NULL) return NULL;
  if(debug) printf("rlFileLoad::firstLine=%s",current_line->line);
  return current_line->line;
}

const char *rlFileLoad::nextLine()
{
  if(loaded == 0) return NULL;
  if(current_line != NULL) current_line = current_line->next;
  if(current_line == NULL) return NULL;
  if(debug) printf("rlFileLoad::nextLine=%s",current_line->line);
  return current_line->line; 
}

void rlFileLoad::setDebug(int state)
{
  if(state == 0) debug = 0;
  else           debug = 1;
}

int rlFileLoad::text2rlstring(rlString &rlstring)
{
  rlstring = "";
  if(loaded == 0) return -1;
  const char *line = firstLine();
  while(line != NULL)
  {
    rlstring += line;
    rlstring += "\n";
    line = nextLine();
  }
  return 0;
}

