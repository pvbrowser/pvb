/***************************************************************************
                          rlbuffer.h  -  description
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
#ifndef _RL_BUFFER_H_
#define _RL_BUFFER_H_

#include "rldefine.h"

/*! <pre>
class for handling memory buffers.
</pre> */
class rlBuffer
{
public:
  rlBuffer();
  virtual ~rlBuffer();
  int resize(int size);
  int size();
  int setText(const char *text);
  char *line(int i);
  void          *adr;
  char          *c;
  unsigned char *uc;
  int           *i;
  unsigned int  *ui;
  long          *l;
  unsigned long *ul;
  float         *f;
  double        *d;

private:
  int _size;
};
#endif
