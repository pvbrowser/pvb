/***************************************************************************
                       rlhistoryreader.h  -  description
                             -------------------
    begin                : Wed Dec 06 2006
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
#ifndef _RL_HISTORY_READER_H_
#define _RL_HISTORY_READER_H_

#include "rldefine.h"
#include "rltime.h"
#include <stdio.h>

typedef struct _rlHistoryReaderLine_
{
  _rlHistoryReaderLine_ *next;
  char *line;
}rlHistoryReaderLine;

/*! <pre>
This class reads tab separated text including time stamp from 10 csv files
Use it together with rlHistoryLogger.

Example usage within pvbrowser slot function:

#include "rlhistoryreader.h"

typedef struct // (todo: define your data structure here)
{
  rlHistoryReader hist;
}
DATA;

...snip...

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == buttonShowPlot)
  {
    rlTime tStart, tDiff, tEnd;
    tStart.year  = 2009;
    tStart.month = 8;
    tStart.day   = 29;
    tStart.hour  = 0;

    tDiff.hour = 24;

    tEnd = tStart + tDiff;

    printf("start=%s diff=%s end=%s\n",tStart.getTimeString(), tDiff.getTimeString(), tEnd.getTimeString());

    if(d->hist.read("test", &tStart, &tEnd) < 0)
    {
      printf("could not read test csv file\n");
      return 0;
    }
    const char *line = d->hist.firstLine();
    while(*line != '\\0')
    {
      printf("line=%s", line);
      const char *values = strchr(line,'\t');
      if(values != NULL)
      {
        float val1, val2;
        sscanf(values,"\t%f\t%f", &val1, &val2);
        printf("val1=%f val2=%f\n", val1, val2);
        //// fill your buffer for plotting here
      }
      line = d->hist.nextLine();
    }
    //// display your xy-graphic from the filled buffer here
  }
  return 0;
}
</pre> */
class rlHistoryReader
{
public:
  rlHistoryReader();
  virtual ~rlHistoryReader();
  int read(const char *csvName, rlTime *start, rlTime *end);
  const char *firstLine();
  const char *nextLine();
  int clean();
  int cat(const char *csvName, FILE *fout);
  int debug;
private:
  int openFile();
  int pushLineToMemory(const char *line);
  rlHistoryReaderLine *first_line,*current_line;
  rlTime time;
  FILE *fin;
  int current_file;
  char *csv_name, *csv_file_name;
};
#endif
