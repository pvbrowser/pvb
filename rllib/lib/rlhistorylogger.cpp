/***************************************************************************
                       rlhistorylogger.cpp  -  description
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
#include "rlhistorylogger.h"
#include "rlcutil.h"
#include <string.h>

rlHistoryLogger::rlHistoryLogger(const char *csvName, int maxHoursPerFile, int maxLinesInMemory)
{
  int val;
  debug = 0;
  first_line = current_line = NULL;
  fout = NULL;
  max_hours_per_file = maxHoursPerFile;
  if(max_hours_per_file <= 0) max_hours_per_file = 1;
  val = max_hours_per_file;
  time_diff.hour  = val % 24;
  val = val / 24;
  time_diff.day   = val % 31; // we are on the save side if we assume a month with 31 days
  val = val / 31;
  time_diff.month = val % 12;
  val = val / 12;
  time_diff.year  = val;
  max_lines_in_memory = maxLinesInMemory;
  if(max_lines_in_memory <= 0) max_lines_in_memory = 1;
  current_file = -1;
  csv_name = new char[strlen(csvName)+1];
  strcpy(csv_name,csvName);
  csv_file_name = new char[strlen(csvName)+132];
  time.getLocalTime();
  file_start_time.getLocalTime();
  openFile(); // jun 2021, hint by george zempekis
}

rlHistoryLogger::~rlHistoryLogger()
{
  mutex.lock();
  if(fout != NULL) fclose(fout);
  delete [] csv_name;
  delete [] csv_file_name;
  if(first_line != NULL)
  {
    rlHistoryLogLine *last_line;
    current_line = first_line;
    while(current_line != NULL)
    {
      last_line = current_line;
      current_line = current_line->next;
      if(last_line != NULL)
      {
        delete [] last_line->line;
        delete last_line;
      }
    }
  }
  mutex.unlock();
}

int rlHistoryLogger::pushLine(const char *text)
{
  mutex.lock();
  time.getLocalTime();
  char *line = new char[strlen(text)+132];
  sprintf(line,"%s\t%s",time.getTimeString(),text);
  if(debug) printf("pushLine=%s\n",line);
  pushLineToMemory(line);
  pushLineToFile(line);
  delete [] line;
  mutex.unlock();
  return 0;
}

int rlHistoryLogger::pushLineToMemory(const char *line)
{
  rlHistoryLogLine *history_line;

  // put line at 1 position
  if(first_line == NULL)
  {
    first_line = new rlHistoryLogLine;
    first_line->line = new char[strlen(line)+1];
    strcpy(first_line->line,line);
    first_line->next = NULL;
  }
  else
  {
    history_line = first_line;
    first_line = new rlHistoryLogLine;
    first_line->line = new char[strlen(line)+1];
    strcpy(first_line->line,line);
    first_line->next = history_line;
  }

  // limit tail of list
  history_line = first_line;
  for(int i=0; i<max_lines_in_memory; i++)
  {
    if(history_line == NULL) break;
    history_line = history_line->next;
  }
  if(history_line != NULL)
  {
    rlHistoryLogLine *last_line;
    current_line = history_line->next;
    while(current_line != NULL)
    {
      last_line = current_line;
      current_line = current_line->next;
      if(last_line != NULL)
      {
        delete [] last_line->line;
        delete last_line;
      }
    }
    history_line->next = NULL;
  }
  return 0;
}

int rlHistoryLogger::pushLineToFile(const char *line)
{
  if(fout == NULL) openFile();
  if((file_start_time + time_diff) < time)
  {
    if(fout != NULL) fclose(fout);
    fout = NULL;
    openFile();
  }
  if(fout != NULL)
  {
    fprintf(fout,"%s\n",line);
    fflush(fout);
  }
  return 0;
}

int rlHistoryLogger::openFile()
{
  if(current_file == -1)
  {
    // find oldest file and open it for writing
    int i_oldest = 0;
    rlTime t,t_oldest;
    t_oldest.getLocalTime(); // this must be newer that any file time
    for(int i=0; i<10; i++)
    {
      sprintf(csv_file_name,"%s%d.csv",csv_name,i);
      if(t.getFileModificationTime(csv_file_name) == 0)
      {
        if(t < t_oldest) i_oldest = i;
      }
    }
    current_file = i_oldest;
    sprintf(csv_file_name,"%s%d.csv",csv_name,i_oldest);
    fout = fopen(csv_file_name,"w");
  }
  else
  {
    // open next file for writing
    current_file++;
    if(current_file >= 10) current_file = 0;
    sprintf(csv_file_name,"%s%d.csv",csv_name,current_file);
    fout = fopen(csv_file_name,"w");
  }
  file_start_time.getLocalTime();
  return 0;
}

const char *rlHistoryLogger::firstLine()
{
  if(first_line == NULL) return "";
  current_line = first_line;
  return current_line->line;
}

const char *rlHistoryLogger::nextLine()
{
  if(current_line == NULL) return "";
  current_line = current_line->next;
  if(current_line == NULL) return "";
  return current_line->line;
}

//#define TESTING_HISTORYLOGGER
#ifdef TESTING_HISTORYLOGGER
int main()
{
  char text[1024];
  int val;
  rlHistoryLogger logger("test", 1);
  logger.debug = 1;
  val = 0;
  while(val >= 0)
  {
    if((val % 10) == 0)
    {
      const char *cptr;
      logger.mutex.lock();
      cptr = logger.firstLine();
      while(*cptr != '\0')
      {
        printf("mem=%s\n",cptr);
        cptr = logger.nextLine();
      }
      logger.mutex.unlock();
    }
    sprintf(text,"%d\t%d\t%d",val,val+1,val+2);
    logger.pushLine(text);
    val++;
    rlsleep(1000);
  }
  return 0;
}
#endif
