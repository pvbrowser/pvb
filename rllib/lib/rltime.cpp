/***************************************************************************
                          rltime.cpp  -  description
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
#include "rltime.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifdef RLUNIX
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef __VMS
#include <ssdef.h>
#include <iodef.h>
#include <stdio.h>
#include <string.h>
#include <starlet.h>
#include <descrip.h>
#include <lib$routines.h>
#include <libdef.h>
#include <jpidef.h>
typedef struct
{
  short year;
  short month;
  short day;
  short hour;
  short min;
  short sec;
  short hth;
}
TDS;
typedef struct
{
  long word_1;
  long word_2;
}
VAX_BIN_TIME;
#endif

#ifdef RLWIN32
#include <windows.h>
#include <mmsystem.h>
#endif

rlTime::rlTime(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond)
{
  year        = Year;
  month       = Month;
  day         = Day;
  hour        = Hour;
  minute      = Minute;
  second      = Second;
  millisecond = Millisecond;
}

rlTime::~rlTime()
{
}

void rlTime::setTimeFromString(const char *time_string)
{
  year        = 0;
  month       = 0;
  day         = 0;
  hour        = 0;
  minute      = 0;
  second      = 0;
  millisecond = 0;
  sscanf(time_string,"%d-%d-%d %d:%d:%d %d",&year,&month,&day, &hour,&minute,&second, &millisecond);
}

void rlTime::setTimeFromIsoString(const char *iso_time_string)
{
  year        = 0;
  month       = 0;
  day         = 0;
  hour        = 0;
  minute      = 0;
  second      = 0;
  millisecond = 0;
  sscanf(iso_time_string,"%d-%d-%dT%d:%d:%d.%d",&year,&month,&day, &hour,&minute,&second, &millisecond);
}

const char *rlTime::getTimeString()
{
  sprintf(time_string,"%04d-%02d-%02d %02d:%02d:%02d %04d",year,month,day, hour,minute,second, millisecond);
  return time_string;
}

const char *rlTime::getIsoTimeString()
{
  sprintf(time_string,"%04d-%02d-%02dT%02d:%02d:%02d.%d",year,month,day, hour,minute,second, millisecond);
  return time_string;
}

void rlTime::getLocalTime()
{
#ifdef RLUNIX
  time_t t;
  struct tm       *tms;
  struct timeval  tv;
  struct timezone tz;

  time(&t);
  tms = localtime(&t);
  gettimeofday(&tv, &tz);

  /* adjust year and month */
  tms->tm_year += 1900;
  tms->tm_mon  += 1;

  millisecond = (int)tv.tv_usec / 1000;
  second      = (int)tms->tm_sec;
  minute      = (int)tms->tm_min;
  hour        = (int)tms->tm_hour;
  day         = (int)tms->tm_mday;
  month       = (int)tms->tm_mon;
  year        = (int)tms->tm_year;
#endif

#ifdef __VMS
  TDS    tds;
  sys$numtim(&tds, 0);
  millisecond  = (int)tds.hth * 10;
  second       = (int)tds.sec;
  minute       = (int)tds.min;
  hour         = (int)tds.hour;
  day          = (int)tds.day;
  month        = (int)tds.month;
  year         = (int)tds.year;
#endif

#ifdef RLWIN32
  SYSTEMTIME st;
  GetLocalTime(&st);
  millisecond  = st.wMilliseconds;
  second       = st.wSecond;
  minute       = st.wMinute;
  hour         = st.wHour;
  day          = st.wDay;
  month        = st.wMonth;
  year         = st.wYear;
#endif
}

int rlTime::getFileModificationTime(const char *filename)
{
  struct stat statbuf;
  struct tm *tms;

#ifdef RLUNIX
  if(lstat(filename,&statbuf)) return -1;
#else
  if(stat(filename,&statbuf)) return -1;
#endif
  tms = localtime(&statbuf.st_mtime);

  /* adjust year and month */
  tms->tm_year += 1900;
  tms->tm_mon  += 1;

  millisecond = 0;
  second      = (int)tms->tm_sec;
  minute      = (int)tms->tm_min;
  hour        = (int)tms->tm_hour;
  day         = (int)tms->tm_mday;
  month       = (int)tms->tm_mon;
  year        = (int)tms->tm_year;

  return 0;
}

void rlTime::setLocalTime()
{
#ifdef RLUNIX
  struct timeval tv;
  struct tm t;

  t.tm_mday  = day;
  t.tm_mon   = month - 1;
  t.tm_year  = year - 1900;
  t.tm_hour  = hour;
  t.tm_min   = minute;
  t.tm_sec   = second;
  tv.tv_sec  = mktime(&t);
  tv.tv_usec = 1000 * millisecond;
  settimeofday(&tv,NULL);
#endif

#ifdef __VMS
  VAX_BIN_TIME vbt;
  struct dsc$descriptor_s  d_time;
  char smonth[12][4],buf[64];

  // Initialize month array
  memset (smonth   , 0, sizeof(smonth));
  memcpy (smonth  [0], "JAN", 3);
  memcpy (smonth  [1], "FEB", 3);
  memcpy (smonth  [2], "MAR", 3);
  memcpy (smonth  [3], "APR", 3);
  memcpy (smonth  [4], "MAY", 3);
  memcpy (smonth  [5], "JUN", 3);
  memcpy (smonth  [6], "JUL", 3);
  memcpy (smonth  [7], "AUG", 3);
  memcpy (smonth  [8], "SEP", 3);
  memcpy (smonth  [9], "OCT", 3);
  memcpy (smonth [10], "NOV", 3);
  memcpy (smonth [11], "DEC", 3);
  // Create time buffer
  sprintf(buf, "%02d-%3.3s-%04d %02d:%02d:%02d.%02d",
                day,
                smonth[month-1],
                year,
                hour,
                minute,
                second,
                millisecond / 10);

  // Fill string descriptor
  d_time.dsc$w_length  = strlen(buf);
  d_time.dsc$b_dtype   = DSC$K_DTYPE_T;
  d_time.dsc$b_class   = DSC$K_CLASS_S;
  d_time.dsc$a_pointer = buf;
  // Convert time buf to VAX bin time
  sys$bintim(&d_time, &vbt);
  // Set the system time
  sys$setime(&vbt);
#endif

#ifdef RLWIN32
  SYSTEMTIME st;
  st.wDay          = day;
  st.wMonth        = month;
  st.wYear         = year;
  st.wHour         = hour;
  st.wMinute       = minute;
  st.wSecond       = second;
  st.wMilliseconds = millisecond;
  SetSystemTime(&st);
#endif
}

rlTime& rlTime::operator+=(rlTime &time)
{
  rlTime t;
  t = *this + time;
  *this = t;
  return *this;
}

rlTime& rlTime::operator-=(rlTime &time)
{
  rlTime t;
  t = *this - time;
  *this = t;
  return *this;
}

rlTime rlTime::operator+(rlTime &time)
{
  int maxmonth,y,m;
  rlTime t;

  t.year        = year        + time.year;
  t.month       = month       + time.month;
  t.day         = day         + time.day;
  t.hour        = hour        + time.hour;
  t.minute      = minute      + time.minute;
  t.second      = second      + time.second;
  t.millisecond = millisecond + time.millisecond;

  y = t.year;
  if(t.month > 12 || (t.month==12 && t.day==31 && t.hour>=24)) y++;
  m = t.month;
  if(t.month > 12 || (t.month==12 && t.day==31 && t.hour>=24)) m = 1;

  switch(m % 12)
  {
    case 1: // january
      maxmonth = 31;
      break;
    case 2: // february
      maxmonth = 28;
      // Annus bisextilis (calendario Gregoriano)
      if(y%4==0) 
      {
        maxmonth = 29;
        int hth = y % 100;
        int special = y % 400; // 1900-+-2100-2200-2300-+-2500-2600-2700
        if(hth == 0 && special != 0) maxmonth = 28;
      }  
      break;
    case 3: // march
      maxmonth = 31;
      break;
    case 4: // april
      maxmonth = 30;
      break;
    case 5: // may
      maxmonth = 31;
      break;
    case 6: // june
      maxmonth = 30;
      break;
    case 7: // july
      maxmonth = 31;
      break;
    case 8: // august
      maxmonth = 31;
      break;
    case 9: // september
      maxmonth = 30;
      break;
    case 10: // october
      maxmonth = 31;
      break;
    case 11: // november
      maxmonth = 30;
      break;
    case 12: // december
      maxmonth = 31;
      break;
    default:
      maxmonth = 31;
      break;
  }

  if(t.millisecond >= 1000) { t.second++; t.millisecond -= 1000; }
  if(t.second >= 60)        { t.minute++; t.second      -= 60; }
  if(t.minute >= 60)        { t.hour++,   t.minute      -= 60; }
  if(t.hour >= 24)          { t.day++;    t.hour        -= 24; }
  if(t.day > maxmonth)      { t.month++;  t.day         -= maxmonth; }
  if(t.month > 12)          { t.year++;   t.month       -= 12; }
  return t;
}

rlTime rlTime::operator-(rlTime &time)
{
  int maxmonth,y,m;
  rlTime t;

  y = 0;
  t.year        = year        - time.year;
  t.month       = month       - time.month;
  t.day         = day         - time.day;
  t.hour        = hour        - time.hour;
  t.minute      = minute      - time.minute;
  t.second      = second      - time.second;
  t.millisecond = millisecond - time.millisecond;

  if(t.millisecond < 0) { t.second--; t.millisecond += 1000; }
  if(t.second < 0)      { t.minute--; t.second      += 60; }
  if(t.minute < 0)      { t.hour--,   t.minute      += 60; }
  if(t.hour < 0)        { t.day--;    t.hour        += 24; }

  if(t.day < 0)
  {
    t.month--;
    y = t.year;
    m = t.month;
    if(m <= 0) { m += 12; y--; }
    switch(m % 12)
    {
      case 1: // january
        maxmonth = 31;
        break;
      case 2: // february
        maxmonth = 28;
        // Annus bisextilis (calendario Gregoriano)
        if(y%4==0) 
        {
          maxmonth = 29;
          int hth = y % 100;
          int special = y % 400; // 1900-+-2100-2200-2300-+-2500-2600-2700
          if(hth == 0 && special != 0) maxmonth = 28;
        }  
        break;
      case 3: // march
        maxmonth = 31;
        break;
      case 4: // april
        maxmonth = 30;
        break;
      case 5: // may
        maxmonth = 31;
        break;
      case 6: // june
        maxmonth = 30;
        break;
      case 7: // july
        maxmonth = 31;
        break;
      case 8: // august
        maxmonth = 31;
        break;
      case 9: // september
        maxmonth = 30;
        break;
      case 10: // october
        maxmonth = 31;
        break;
      case 11: // november
        maxmonth = 30;
        break;
      case 12: // december
        maxmonth = 31;
        break;
      default:
        maxmonth = 31;
        break;
    }
    t.day += maxmonth; 
  }

  if(y >= 0)
  {
    //printf("after christ was born. thus everything is ok.\n");                           
  }
  else
  {
    //printf("before christ was born. now also ok\n");
                      { t.month++;  t.day         -= 30;   }
    if(t.day    < 30) { t.day++;    t.hour        -= 24;   }
    if(t.hour   < 0 ) { t.hour++;   t.minute      -= 60;   }
    if(t.minute < 0 ) { t.minute++; t.second      -= 60;   } 
    if(t.second < 0 ) { t.second++; t.millisecond -= 1000; }
  }

  return t;
}

int rlTime::operator==(rlTime &time)
{
  if(year        != time.year)        return 0;
  if(month       != time.month)       return 0;
  if(day         != time.day)         return 0;
  if(hour        != time.hour)        return 0;
  if(minute      != time.minute)      return 0;
  if(second      != time.second)      return 0;
  if(millisecond != time.millisecond) return 0;

  return 1;
}

int rlTime::operator<(rlTime &time)
{
  rlTime diff,t1;

  t1.year        = year;
  t1.month       = month;
  t1.day         = day;
  t1.hour        = hour;
  t1.minute      = minute;
  t1.second      = second;
  t1.millisecond = millisecond;
  //printf("<t1=%s\n",t1.getTimeString());
  //printf("<time=%s\n",time.getTimeString());
  diff = t1 - time;
  //printf("<diff=%s\n",diff.getTimeString());
  if(diff.year        < 0) return 1;
  if(diff.month       < 0) return 1;
  if(diff.day         < 0) return 1;
  if(diff.hour        < 0) return 1;
  if(diff.minute      < 0) return 1;
  if(diff.second      < 0) return 1;
  if(diff.millisecond < 0) return 1;
  return 0;
}

int rlTime::operator<=(rlTime &time)
{
  if((*this) == time) return 1;
  if((*this) <  time) return 1;
  return 0;
}

int rlTime::operator>(rlTime &time)
{
  rlTime diff,t1;

  t1.year        = year;
  t1.month       = month;
  t1.day         = day;
  t1.hour        = hour;
  t1.minute      = minute;
  t1.second      = second;
  t1.millisecond = millisecond;
  //printf(">t1=%s\n",t1.getTimeString());
  //printf(">time=%s\n",time.getTimeString());
  diff = time - t1;
  //printf(">diff=%s\n",diff.getTimeString());
  if(diff.year        < 0)        return 1;
  if(diff.month       < 0)        return 1;
  if(diff.day         < 0)        return 1;
  if(diff.hour        < 0)        return 1;
  if(diff.minute      < 0)        return 1;
  if(diff.second      < 0)        return 1;
  if(diff.millisecond < 0)        return 1;
  return 0;
}

int rlTime::operator>=(rlTime &time)
{
  if((*this) == time) return 1;
  if((*this) >  time) return 1;
  return 0;
}

double rlTime::secondsSinceEpoche()
{
  struct tm begin;
  struct tm test;

  memset(&begin,0,sizeof(tm));
  memset(&test,0,sizeof(tm));

  begin.tm_year = 70;
  begin.tm_mon  = 0;
  begin.tm_mday = 1;
  begin.tm_hour = 0;
  begin.tm_min  = 0;
  begin.tm_sec  = 0;

  test.tm_year = year - 1900;
  test.tm_mon  = month - 1;
  test.tm_mday = day;
  test.tm_hour = hour;
  test.tm_min  = minute;
  test.tm_sec  = second;

  time_t t0 = mktime(&begin);
  time_t t1 = mktime(&test);

  return difftime(t1,t0) + (((double) millisecond) / 1000);
}

