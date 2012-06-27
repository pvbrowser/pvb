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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "rlspreadsheet.h"
#include "rlcutil.h"

static const char null_string[] = "";

// rlSpreadsheetCell
rlSpreadsheetCell::rlSpreadsheetCell(const char *Text)
{
  if(Text != NULL)
  {
    txt = new char[strlen(Text)+1];
    strcpy(txt,Text);
  }
  else
  {
    txt = NULL;
  }
  nextCell = NULL;
}

rlSpreadsheetCell::~rlSpreadsheetCell()
{
  if(txt != NULL) delete [] txt;
}

const char *rlSpreadsheetCell::text()
{
  if(txt == NULL) return null_string;
  return txt;
}

void rlSpreadsheetCell::setText(const char *Text)
{
  if(txt  != NULL) delete [] txt;
  if(Text == NULL)
  {
    txt = NULL;
    return;
  }
  txt = new char[strlen(Text)+1];
  strcpy(txt,Text);
}

int rlSpreadsheetCell::printf(const char *format, ...)
{
  int ret;
  char buf[rl_PRINTF_LENGTH_SPREADSHEET]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(buf, rl_PRINTF_LENGTH_SPREADSHEET - 1, format, ap);
  va_end(ap);
  setText(buf);
  return ret;
}

void rlSpreadsheetCell::clear()
{
  if(txt != NULL) delete [] txt;
  txt = NULL;
}

void rlSpreadsheetCell::setNextCell(rlSpreadsheetCell *next)
{
  nextCell = next;
}

rlSpreadsheetCell  *rlSpreadsheetCell::getNextCell()
{
  return nextCell;
}

int rlSpreadsheetCell::exists()
{
  return 1;
}

// rlSpreadsheetRow
rlSpreadsheetRow::rlSpreadsheetRow()
{
  firstCell = NULL;
  nextRow   = NULL;
}

rlSpreadsheetRow::~rlSpreadsheetRow()
{
  rlSpreadsheetCell *item,*last;
  item = firstCell;
  while(item != NULL)
  {
    last = item;
    item = item->getNextCell();
    if(item != last) delete last;
  }
}

const char *rlSpreadsheetRow::text(int column)
{
  int c = 1;
  rlSpreadsheetCell *item;

  item = firstCell;
  while(item != NULL)
  {
    if(c == column) return item->text();
    item = item->getNextCell();
    c++;
  }
  return null_string;
}

void rlSpreadsheetRow::setText(int column, const char *text)
{
  int c = 1;
  rlSpreadsheetCell *item;

  if(column < 1) return;
  if(firstCell == NULL) firstCell = new rlSpreadsheetCell;

  item = firstCell;
  while(1)
  {
    if(c == column)  { item->setText(text); return; }
    if(item->getNextCell() == NULL) item->setNextCell(new rlSpreadsheetCell);
    item = item->getNextCell();
    c++;
  }
}

int rlSpreadsheetRow::printf(int column, const char *format, ...)
{
  int ret;
  char buf[rl_PRINTF_LENGTH_SPREADSHEET]; // should be big enough
  
  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(buf, rl_PRINTF_LENGTH_SPREADSHEET - 1, format, ap);
  va_end(ap);
  setText(column,buf);
  return ret;
}

void rlSpreadsheetRow::clear()
{
  rlSpreadsheetCell *item;

  item = firstCell;
  while(item != NULL)
  {
    item->clear();
    item = item->getNextCell();
  }
}

void rlSpreadsheetRow::setNextRow(rlSpreadsheetRow *next)
{
  nextRow = next;
}

rlSpreadsheetRow  *rlSpreadsheetRow::getNextRow()
{
  return nextRow;
}

rlSpreadsheetCell *rlSpreadsheetRow::getFirstCell()
{
  return firstCell;
}

void rlSpreadsheetRow::readRow(const unsigned char *line, char delimitor)
{
  int i,tab1,tab2,col;
  unsigned char *celltext;

  clear();

  tab1 = tab2 = -1;
  col = 1;
  i = 0;
  while(line[i] != '\0')
  {
    if(line[i] == delimitor || line[i] == '\n' || line[i] == 0x0d) // normally delimitor='\t'
    {
      tab1 = tab2;
      tab2 = i;
      celltext = new unsigned char[tab2-tab1+1];
      if(tab2 > tab1)
      {
        strncpy((char *) celltext,(const char *) &line[tab1+1],tab2-tab1);
        celltext[tab2-tab1-1] = '\0';
      }
      else
      {
        celltext[0] = '\0';
      }
      setText(col++, (char *) celltext);
      delete [] celltext;
    }
    i++;
  }

  return;
}

void rlSpreadsheetRow::writeRow(void *fp, char delimitor)
{
  rlSpreadsheetCell *cell;
  if(fp == NULL) return;
  cell = firstCell;
  while(cell != NULL)
  {
    fprintf((FILE *) fp,"%s",cell->text());
    cell = cell->getNextCell();
    if(cell != NULL) fprintf((FILE *) fp, "%c", delimitor);
  }
  fprintf((FILE *) fp,"\n");
}

int rlSpreadsheetRow::exists(int column)
{
  rlSpreadsheetCell *item;
  int c;

  c = 1;
  item = firstCell;
  while(item != NULL)
  {
    if(c == column) return 1;
    c++;
    item = item->getNextCell();
  }
  return 0;
}

// rlSpreadsheetTable
rlSpreadsheetTable::rlSpreadsheetTable(char del)
{
  firstRow  = NULL;
  nextTable = NULL;
  delimitor = del;
}

rlSpreadsheetTable::~rlSpreadsheetTable()
{
  rlSpreadsheetRow *item,*last;
  item = firstRow;
  while(item != NULL)
  {
    last = item;
    item = item->getNextRow();
    if(item != last) delete last;
  }
}

const char *rlSpreadsheetTable::text(int column, int row)
{
  int r = 1;
  rlSpreadsheetRow *item;

  item = firstRow;
  while(item != NULL)
  {
    if(r == row) return item->text(column);
    item = item->getNextRow();
    r++;
  }
  return null_string;
}

void rlSpreadsheetTable::setText(int column, int row, const char *text)
{
  int r = 1;
  rlSpreadsheetRow *item,*nextitem;

  if(row < 1) return;
  if(firstRow == NULL) firstRow = new rlSpreadsheetRow;
  item = firstRow;
  while(1)
  {
    if(r == row) { item->setText(column,text); return; }
    nextitem = item->getNextRow();
    if(nextitem == NULL) item->setNextRow(new rlSpreadsheetRow);
    item = item->getNextRow();
    r++;
  }
}

int rlSpreadsheetTable::printf(int column, int row, const char *format, ...)
{
  int ret;
  char buf[rl_PRINTF_LENGTH_SPREADSHEET]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(buf, rl_PRINTF_LENGTH_SPREADSHEET - 1, format, ap);
  va_end(ap);
  setText(column,row,buf);
  return ret;
}

void rlSpreadsheetTable::clear()
{
  rlSpreadsheetRow *item;

  item = firstRow;
  while(item != NULL)
  {
    item->clear();
    item = item->getNextRow();
  }
}

int rlSpreadsheetTable::read(const char *filename)
{
  FILE *fp;
  rlSpreadsheetRow *item,*last,*next;
  unsigned char *line;
  int r = 1;
  if(filename == NULL) return -1;

  // delete old table
  item = firstRow;
  while(item != NULL)
  {
    last = item;
    item = item->getNextRow();
    if(item != last) delete last;
  }

  // read new table
  fp = fopen(filename,"r");
  if(fp == NULL) return -1;
  line = new unsigned char[256*256+1];
  while(fgets((char *) line,256*256,fp) != NULL)
  {
    if(r==1)
    {
      item = firstRow = new rlSpreadsheetRow;
    }
    else
    {
      next = new rlSpreadsheetRow;
      item->setNextRow(next);
      item = next;
    }
    item->readRow(line,delimitor);
    r++;
  }
  delete [] line;
  fclose(fp);
  return r - 1; // number of lines that have been read
}

int rlSpreadsheetTable::write(const char *filename)
{
  rlSpreadsheetRow *item;
  FILE *fp;
  if(filename == NULL) return -1;
  fp = fopen(filename,"w");
  if(fp == NULL) return -1;

  item = firstRow;
  while(item != NULL)
  {
    item->writeRow((void *) fp, delimitor);
    item = item->getNextRow();
  }

  fclose(fp);
  return 0;
}

void rlSpreadsheetTable::setNextTable(rlSpreadsheetTable *next)
{
  nextTable = next;
}

rlSpreadsheetRow *rlSpreadsheetTable::getFirstRow()
{
  return firstRow;
}

rlSpreadsheetTable  *rlSpreadsheetTable::getNextTable()
{
  return nextTable;
}

int rlSpreadsheetTable::exists(int column, int row)
{
  rlSpreadsheetRow *item;
  int r;

  r = 1;
  item = firstRow;
  while(item != NULL)
  {
    if(r == row) return item->exists(column);
    r++;
    item = item->getNextRow();
  }
  return 0;
}

void rlSpreadsheetTable::setDelimitor(char _delimitor)
{
  delimitor = _delimitor;
}

// rlSpreadsheetWorkbook
rlSpreadsheetWorkbook::rlSpreadsheetWorkbook(char del)
{
  firstTable = NULL;
  delimitor = del;
}

rlSpreadsheetWorkbook::~rlSpreadsheetWorkbook()
{
  rlSpreadsheetTable *item,*last;
  item = firstTable;
  while(item != NULL)
  {
    last = item;
    item = item->getNextTable();
    if(item != last) delete last;
  }
}

const char *rlSpreadsheetWorkbook::text(int column, int row, int page)
{
  int p = 1;
  rlSpreadsheetTable *item;

  item = firstTable;
  while(item != NULL)
  {
    if(p == page) return item->text(column, row);
    item = item->getNextTable();
    p++;
  }
  return null_string;
}

void rlSpreadsheetWorkbook::setText(int column, int row, int page, const char *text)
{
  int p = 1;
  rlSpreadsheetTable *item,*nextitem;

  if(page < 1) return;
  if(firstTable == NULL) firstTable = new rlSpreadsheetTable;
  item = firstTable;
  while(1)
  {
    if(p == page) { item->setText(column, row, text); return; }
    nextitem = item->getNextTable();
    if(nextitem == NULL) item->setNextTable(new rlSpreadsheetTable);
    item = item->getNextTable();
    p++;
  }
}

int rlSpreadsheetWorkbook::printf(int column, int row, int page, const char *format, ...)
{
  int ret;
  char buf[rl_PRINTF_LENGTH_SPREADSHEET]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(buf, rl_PRINTF_LENGTH_SPREADSHEET - 1, format, ap);
  va_end(ap);
  setText(column,row,page,buf);
  return ret;
}

void rlSpreadsheetWorkbook::clear()
{
  rlSpreadsheetTable *item;

  item = firstTable;
  while(item != NULL)
  {
    item->clear();
    item = item->getNextTable();
  }
}

int rlSpreadsheetWorkbook::read(const char *filename)
{
  FILE *fp;
  rlSpreadsheetTable *item,*last,*next;
  int p = 1;
  char buf[16],*fname;
  if(filename == NULL) return -1;

  // delete old workbook
  item = firstTable;
  while(item != NULL)
  {
    last = item;
    item = item->getNextTable();
    if(item != last) delete last;
  }

  // read new workbook
  fname = new char[strlen(filename)+16];
  while(1)
  {
    strcpy(fname,filename);
    sprintf(buf,"%d.txt",p);
    strcat(fname,buf);
    // test if file exists
    fp = fopen(fname,"r");
    if(fp == NULL) break;
    fclose(fp);
    if(p==1)
    {
      item = firstTable = new rlSpreadsheetTable(delimitor);
    }
    else
    {
      next = new rlSpreadsheetTable(delimitor);
      item->setNextTable(next);
      item = next;
    }
    if(item->read(fname) < 0) break;
    p++;
  }

  delete [] fname;
  return 0;
}

int rlSpreadsheetWorkbook::write(const char *filename)
{
  rlSpreadsheetTable *item = NULL;
  int p = 1;
  char buf[16],*fname;
  if(filename == NULL) return -1;

  // write workbook
  fname = new char[strlen(filename)+16];
  while(1)
  {
    strcpy(fname,filename);
    sprintf(buf,"%d.txt",p);
    strcat(fname,buf);
    if(p==1) item = firstTable;
    else     item = item->getNextTable();
    if(item == NULL)     break;
    if(item->write(fname) < 0) break;
    p++;
  }

  delete [] fname;
  return 0;
}

int rlSpreadsheetWorkbook::exists(int column, int row, int page)
{
  rlSpreadsheetTable *item;
  int p;

  p = 1;
  item = firstTable;
  while(item != NULL)
  {
    if(p == page) return item->exists(column,row);
    p++;
    item = item->getNextTable();
  }
  return 0;
}

rlSpreadsheetTable *rlSpreadsheetWorkbook::getFirstTable()
{
  return firstTable;
}

void rlSpreadsheetWorkbook::setDelimitor(char _delimitor)
{
  delimitor = _delimitor;
  rlSpreadsheetTable *table = getFirstTable();
  while(table != NULL)
  {
    table->setDelimitor(delimitor);
    table = table->getNextTable();
  }
}


