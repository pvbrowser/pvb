/***************************************************************************
                       rlhistoryreader.cpp  -  description
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
#include "rlhistoryreader.h"
#include "rlcutil.h"
#include <string.h>

#define MAXBUF 256*256

rlHistoryReader::rlHistoryReader()
{
  debug = 0;
  first_line = current_line = NULL;
  fin = NULL;
  current_file = -1;
  csv_name = NULL;
  csv_file_name = NULL;
}

rlHistoryReader::~rlHistoryReader()
{
  clean();
  if(csv_name != NULL) delete [] csv_name;
  csv_name = NULL;
  if(csv_file_name != NULL) delete [] csv_file_name;
  csv_file_name = NULL;
}

int rlHistoryReader::read(const char *csvName, rlTime *start, rlTime *end)
{
  char *buf;
  clean();
  if(csv_name != NULL) delete [] csv_name;
  csv_name = NULL;
  if(csv_file_name != NULL) delete [] csv_file_name;
  csv_file_name = NULL;

  first_line = current_line = NULL;
  fin = NULL;
  current_file = -1;
  csv_name = new char[strlen(csvName)+1];
  strcpy(csv_name,csvName);
  csv_file_name = new char[strlen(csvName)+132];

  buf = new char[MAXBUF];
  for(int i=0; i<10; i++)
  {
    openFile();
    if(debug) printf("reading=%s\n",csv_file_name);
    if(fin == NULL) break;
    while(fgets(buf,MAXBUF-1,fin) != NULL)
    {
      time.setTimeFromString(buf);
      if(time < *start)
      {
        if(debug) printf("too old=%s",buf);
      }
      else if(time > *end)
      {
        if(debug) printf("too new=%s",buf);
        break;
      }
      else
      {
        if(debug) printf("pushLineToMemory=%s",buf);
        pushLineToMemory(buf);
      }
    }
    fclose(fin);
    fin = NULL;
  }
  delete [] buf;
  return 0;
}

int rlHistoryReader::clean()
{
  if(fin != NULL) fclose(fin);
  fin = NULL;
  if(first_line != NULL)
  {
    rlHistoryReaderLine *last_line;
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
  return 0;
}

int rlHistoryReader::openFile()
{
  if(current_file == -1)
  {
    // find oldest file and open it for reading
    int i_oldest = 0;
    rlTime t,t_oldest;
    t_oldest.getLocalTime(); // this must be newer that any file time
    for(int i=0; i<10; i++)
    {
      sprintf(csv_file_name,"%s%d.csv",csv_name,i);
      if(t.getFileModificationTime(csv_file_name) == 0)
      {
        if(debug) printf("try openFile=%s\n",csv_file_name);
        if(t < t_oldest)
        {
          i_oldest = i;
          t_oldest = t;
        }
      }
    }
    current_file = i_oldest;
    sprintf(csv_file_name,"%s%d.csv",csv_name,i_oldest);
    if(debug) printf("oldestFile=%s\n",csv_file_name);
    fin = fopen(csv_file_name,"r");
    if(debug && fin != NULL) printf("success openFile=%s\n",csv_file_name);
  }
  else
  {
    // open next file for reading
    current_file++;
    if(current_file >= 10) current_file = 0;
    sprintf(csv_file_name,"%s%d.csv",csv_name,current_file);
    fin = fopen(csv_file_name,"r");
    if(debug && fin != NULL) printf("success openFile=%s\n",csv_file_name);
  }
  return 0;
}

int rlHistoryReader::pushLineToMemory(const char *line)
{
  rlHistoryReaderLine *history_line;

  // put line at 1 position
  if(first_line == NULL)
  {
    first_line = new rlHistoryReaderLine;
    first_line->line = new char[strlen(line)+1];
    strcpy(first_line->line,line);
    first_line->next = NULL;
  }
  else
  {
    history_line = first_line;
    first_line = new rlHistoryReaderLine;
    first_line->line = new char[strlen(line)+1];
    strcpy(first_line->line,line);
    first_line->next = history_line;
  }
  return 0;
}

const char *rlHistoryReader::firstLine()
{
  if(first_line == NULL) return "";
  current_line = first_line;
  return current_line->line;
}

const char *rlHistoryReader::nextLine()
{
  if(current_line == NULL) return "";
  current_line = current_line->next;
  if(current_line == NULL) return "";
  return current_line->line;
}

int rlHistoryReader::cat(const char *csvName, FILE *fout)
{
  char *buf;
  clean();
  if(csv_name != NULL) delete [] csv_name;
  csv_name = NULL;
  if(csv_file_name != NULL) delete [] csv_file_name;
  csv_file_name = NULL;

  first_line = current_line = NULL;
  fin = NULL;
  current_file = -1;
  csv_name = new char[strlen(csvName)+1];
  strcpy(csv_name,csvName);
  csv_file_name = new char[strlen(csvName)+132];

  buf = new char[MAXBUF];
  for(int i=0; i<10; i++)
  {
    openFile();
    if(fin == NULL) break;
    while(fgets(buf,MAXBUF-1,fin) != NULL)
    {
      fprintf(fout,"%s",buf);
    }
    fclose(fin);
    fin = NULL;
  }
  delete [] buf;
  return 0;
}

//#define TESTING_HISTORYREADER1
#ifdef TESTING_HISTORYREADER1
int main()
{
  const char *cptr;
  rlTime start;
  rlTime end;
  start.getLocalTime();
  start.hour = 0;
  end.getLocalTime();

  rlHistoryReader reader;
  reader.debug = 1;
  reader.read("test", &start, &end);

  cptr = reader.firstLine();
  while(*cptr != '\0')
  {
    printf("read=%s",cptr);
    cptr = reader.nextLine();
  }
  return 0;
}
#endif

//#define TESTING_HISTORYREADER2
#ifdef TESTING_HISTORYREADER2
int main()
{
  rlHistoryReader reader;
  reader.debug = 1;
  reader.cat("test", stdout);
  return 0;
}
#endif
