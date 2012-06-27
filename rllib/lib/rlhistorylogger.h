/***************************************************************************
                       rlhistorylogger.h  -  description
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
#ifndef _RL_HISTORY_LOGGER_H_
#define _RL_HISTORY_LOGGER_H_

#include "rldefine.h"
#include "rltime.h"
#include "rlthread.h"
#include <stdio.h>

typedef struct _rlHistoryLogLine_
{
  _rlHistoryLogLine_ *next;
  char *line;
}rlHistoryLogLine;

/*! <pre>
This class logs tab separated text including time stamp in 10 csv files + actual values in memory
This is for archiveing historical data with time stamp.
You should separate the text in pushLine with tab.
</pre> */
class rlHistoryLogger
{
public:
  rlHistoryLogger(const char *csvName, int maxHoursPerFile, int maxLinesInMemory=100);
  virtual ~rlHistoryLogger();
  int pushLine(const char *text);
  const char *firstLine();
  const char *nextLine();
  rlMutex mutex;
  int debug;
private:
  int pushLineToMemory(const char *line);
  int pushLineToFile(const char *line);
  int openFile();
  rlHistoryLogLine *first_line,*current_line;
  rlTime time,file_start_time,time_diff;
  FILE *fout;
  int max_hours_per_file, max_lines_in_memory, current_file;
  char *csv_name, *csv_file_name;
};
#endif
