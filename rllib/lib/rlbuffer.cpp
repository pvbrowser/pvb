/***************************************************************************
                          rlbuffer.cpp  -  description
                             -------------------
    begin                : Sun Aug 24 2014
    copyright            : (C) 2014 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlbuffer.h"

rlBuffer::rlBuffer()
{
  adr = NULL;
  c  = (char *) adr;
  uc = (unsigned char *) adr;
  i  = (int *) adr;
  ui = (unsigned int *) adr;
  l  = (long *) adr;
  ul = (unsigned long *) adr;
  f  = (float *) adr;
  d  = (double *) adr;
}

rlBuffer::~rlBuffer()
{
  if(adr != NULL) delete [] c;
}

int rlBuffer::resize(int size)
{
  if(size <= 0) return -1;
  _size = size;
  if(adr != NULL) delete [] c;

  adr = (void *) new char[_size];  
  c  = (char *) adr;
  uc = (unsigned char *) adr;
  i  = (int *) adr;
  ui = (unsigned int *) adr;
  l  = (long *) adr;
  ul = (unsigned long *) adr;
  f  = (float *) adr;
  d  = (double *) adr;

  return _size;
}

int rlBuffer::size()
{
  return _size;
}

int rlBuffer::setText(const char *text)
{
  int len;
  if((len = strlen(text)) >= _size)
  {
    printf("rlBuffer::setText() text  is too long\n");
    return -1;
  }
  strcpy(c,text);
  return len;
}

char *rlBuffer::line(int i)
{
  if(c == NULL) return NULL;
  char *cptr_start = c;
  for(int ind=0; ind<i; ind++)
  {
    cptr_start = strchr(cptr_start,'\n');
    if(cptr_start != NULL) cptr_start++;
    else return NULL;
  }
  if((cptr_start - c) >= _size) return NULL;
  return cptr_start;
}

