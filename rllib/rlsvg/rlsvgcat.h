/***************************************************************************
                          rlsvgcat.h  -  description
                             -------------------
    begin                : Tue Apr 09 2006
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
#ifndef _RL_SVG_CAT_
#define _RL_SVG_CAT_

class rlSvgCat
{
  public:
    rlSvgCat();
    ~rlSvgCat();
    int  open(const char *infile, const char *outfile = 0);
    int  reopenSocket(const char *infile, int s);
    int  sendOverSocket(char *line_in, int s);
    void cat();
    void close();
  private:
    int  outUntil(int i, const char *tag);
    int  outUntilEnd(int i);
    int  outValue(int i);
    void catline(char *line_in);
    void *fin, *fout;
    int  s;
    char *line;
};
#endif
