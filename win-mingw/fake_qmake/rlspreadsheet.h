/***************************************************************************
                          rlspreadsheet.h  -  description
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
#ifndef _RL_SPREADSHEET_H_
#define _RL_SPREATSHEET_H_

#include "rldefine.h"

class rlSpreadsheetCell
{
public:
  rlSpreadsheetCell(const char *text=0);
  ~rlSpreadsheetCell();
  const char *text();
  void setText(const char *text);
  int  printf(const char *format, ...);
  void clear();
  void setNextCell(rlSpreadsheetCell *next);
  rlSpreadsheetCell *getNextCell();
  int  exists();
private:
  char *txt;
  rlSpreadsheetCell *nextCell;
};

class rlSpreadsheetRow
{
public:
  rlSpreadsheetRow();
  ~rlSpreadsheetRow();
  const char *text(int column);
  void setText(int column, const char *text);
  int  printf(int column, const char *format, ...);
  void clear();
  void setNextRow(rlSpreadsheetRow *next);
  rlSpreadsheetRow *getNextRow();
  void readRow(const unsigned char *line, char delimitor='\t');
  void writeRow(void *fp, char delimitor='\t');
  int  exists(int column);
private:
  rlSpreadsheetCell *firstCell;
  rlSpreadsheetRow  *nextRow;
};

class rlSpreadsheetTable
{
public:
  rlSpreadsheetTable(char delimitor='\t');
  ~rlSpreadsheetTable();
  const char *text(int column, int row);
  void setText(int column, int row, const char *text);
  int  printf(int column, int row, const char *format, ...);
  void clear();
  int  read(const char *filename);
  int  write(const char *filename);
  void setNextTable(rlSpreadsheetTable *next);
  rlSpreadsheetTable *getNextTable();
  int  exists(int column, int row);
private:
  char delimitor;
  rlSpreadsheetRow   *firstRow;
  rlSpreadsheetTable *nextTable;
};

class rlSpreadsheetWorkbook
{
public:
  rlSpreadsheetWorkbook(char delimitor='\t');
  ~rlSpreadsheetWorkbook();
  const char *text(int column, int row, int page);
  void setText(int column, int row, int page, const char *text);
  int  printf(int column, int row, int page, const char *format, ...);
  void clear();
  int  read(const char *filename);
  int  write(const char *filename);
  int  exists(int column, int row, int page);
private:
  char delimitor;
  rlSpreadsheetTable *firstTable;
};

#endif
