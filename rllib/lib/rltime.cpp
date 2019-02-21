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
#include <math.h>
#include <stdlib.h>

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

// gmtime_r can be defined by mingw
#ifndef gmtime_r
static struct tm* gmtime_r(const time_t* t, struct tm* r)
{
  // gmtime is threadsafe in windows because it uses TLS
  struct tm *theTm = gmtime(t);
  if (theTm) {
    *r = *theTm;
    return r;
  } else {
    return 0;
  }
}
#endif // gmtime_r

// gmtime_r can be defined by mingw
#ifndef localtime_r
static struct tm* localtime_r(const time_t* t, struct tm* r)
{
  // localtime is threadsafe in windows because it uses TLS
  struct tm *theTm = localtime(t);
  if (theTm) {
    *r = *theTm;
    return r;
  } else {
    return 0;
  }
}
#endif // localtime_r

#endif

void rlTime::normalizeAsDate()
{
  if (year)
  {
    // read members
    tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year  = year - 1900;
    t.tm_mon   = month - 1;
    t.tm_mday  = day;
    t.tm_hour  = hour;
    t.tm_min   = minute;
    t.tm_sec   = second;

    // normalize milliseconds
    while (millisecond > 1000)
    {
      ++t.tm_sec;
      millisecond -= 1000;
    }

    // normalize time structure
    auto ep = timegm(&t);
    gmtime_r(&ep, &t);

    // set members
    year        = 1900 + t.tm_year;
    month       = 1 + t.tm_mon;
    day         = t.tm_mday;
    hour        = t.tm_hour;
    minute      = t.tm_min;
    second      = t.tm_sec;
  }
}

rlTime::rlTime(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond)
{
  year        = Year;
  month       = Month;
  day         = Day;
  hour        = Hour;
  minute      = Minute;
  second      = Second;
  millisecond = Millisecond;

  normalizeAsDate();
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
  normalizeAsDate();
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
  normalizeAsDate();
}

const char *rlTime::getTimeString()
{
  sprintf(time_string,"%04d-%02d-%02d %02d:%02d:%02d %03d",year, month, day, hour, minute, second, millisecond);
  return time_string;
}

const char *rlTime::getIsoTimeString()
{
  sprintf(iso_time_string,"%04d-%02d-%02dT%02d:%02d:%02d.%03d",year, month, day, hour, minute, second, millisecond);
  return iso_time_string;
}

void rlTime::getLocalTime()
{
#ifdef RLUNIX
  time_t t;
  struct tm       *tms;
  struct tm       tmsbuf;
  struct timeval  tv;
  struct timezone tz;

  time(&t);
  tms = localtime_r(&t, &tmsbuf);
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
  struct tm tmsbuf;

#ifdef RLUNIX
  if(lstat(filename,&statbuf)) return -1;
#else
  if(stat(filename,&statbuf)) return -1;
#endif
  tms = localtime_r(&statbuf.st_mtime, &tmsbuf);

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

rlTime& rlTime::operator+=(time_t seconds)
{
  if (0 > seconds)
    return this->operator -=(-seconds);

  auto d = std::div(seconds, time_t(60));
  second += d.rem;
  if (d.quot)
  {
    d = std::div(d.quot, time_t(60));
    minute += d.rem;
    if (d.quot)
    {
      d = std::div(d.quot, time_t(24));
      hour += d.rem;
      if (d.quot)
      {
        d = std::div(d.quot, time_t(31));
        day += d.rem;
        if (d.quot)
        {
          d = std::div(d.quot, time_t(12));
          month += d.rem;
          year += d.quot;
        }
      }
    }
  }

  this->normalizeAsDate();

  return *this;
}

rlTime& rlTime::operator-=(time_t seconds)
{
  if (0 > seconds)
    return this->operator +=(-seconds);

  auto d = std::div(seconds, time_t(60));
  second -= d.rem;
  if (second < 0)
  {
    ++d.quot;
    second += 60;
  }
  if (d.quot)
  {
    d = std::div(d.quot, time_t(60));
    minute -= d.rem;
    if (minute < 0)
    {
      ++d.quot;
      minute += 60;
    }
    if (d.quot)
    {
      d = std::div(d.quot, time_t(24));
      hour -= d.rem;
      if (hour < 0)
      {
        ++d.quot;
        hour += 24;
      }
      if (d.quot)
      {
        d = std::div(d.quot, time_t(31));
        day -= d.rem;
        if (day < 0)
        {
          ++d.quot;
          day += 31;
        }
        if (d.quot)
        {
          d = std::div(d.quot, time_t(12));
          month -= d.rem;
          if (month < 0)
          {
            ++d.quot;
            month += 12;
          }
          year -= d.quot;
        }
      }
    }
  }

  this->normalizeAsDate();

  return *this;
}

rlTime rlTime::operator+(time_t seconds) const
{
  rlTime t(*this);

  t += seconds;

  return t;
}

rlTime rlTime::operator-(time_t seconds) const
{
  rlTime t(*this);

  t -= seconds;

  return t;
}

double  rlTime::operator-(const rlTime &time) const
{
  return this->secondsSinceEpoche() - time.secondsSinceEpoche();
}

int rlTime::operator==(const rlTime &time) const
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

int rlTime::operator<=(const rlTime &time) const
{
  if((*this) == time) return 1;
  if((*this) <  time) return 1;
  return 0;
}

int rlTime::operator>=(const rlTime &time) const
{
  if((*this) == time) return 1;
  if((*this) >  time) return 1;
  return 0;
}

int rlTime::operator<(const rlTime &time) const
{
  if (this->year < time.year)
    return 1;
  else if (this->year == time.year)
  {
    if (this->month < time.month)
      return 1;
    else if (this->month == time.month)
    {
      if (this->day < time.day)
        return 1;
      else if (this->day == time.day)
      {
        if (this->hour < time.hour)
          return 1;
        else if (this->hour == time.hour)
        {
          if (this->minute < time.minute)
            return 1;
          else if (this->minute == time.minute)
          {
            if (this->second < time.second)
              return 1;
            else if (this->second == time.second)
            {
              if (this->millisecond < time.millisecond)
                return 1;
            }
          }
        }
      }
    }
  }

  return 0;
}

int rlTime::operator>(const rlTime &time) const
{
  if (this->year > time.year)
    return 1;
  else if (this->year == time.year)
  {
    if (this->month > time.month)
      return 1;
    else if (this->month == time.month)
    {
      if (this->day > time.day)
        return 1;
      else if (this->day == time.day)
      {
        if (this->hour > time.hour)
          return 1;
        else if (this->hour == time.hour)
        {
          if (this->minute > time.minute)
            return 1;
          else if (this->minute == time.minute)
          {
            if (this->second > time.second)
              return 1;
            else if (this->second == time.second)
            {
              if (this->millisecond > time.millisecond)
                return 1;
            }
          }
        }
      }
    }
  }

  return 0;
}

time_t rlTime::timegm(struct tm* tm_)
{
  auto t = mktime(tm_);
  struct tm ltm;
  struct tm gtm;
  auto lt = localtime_r(&t, &ltm);
  auto gt = gmtime_r(&t, &gtm);
  auto diff = mktime(lt) - mktime(gt);
  return t + diff;
}

double rlTime::secondsSinceEpoche() const
{
  struct tm begin;
  struct tm test;

  memset(&begin,0,sizeof(tm));
  memset(&test,0,sizeof(tm));

  begin.tm_year = 70;
  begin.tm_mon  = 0;
  begin.tm_mday = 2;  // see below
  begin.tm_hour = 0;
  begin.tm_min  = 0;
  begin.tm_sec  = 0;
  begin.tm_isdst = 0;

  test.tm_year = year - 1900;
  test.tm_mon  = month - 1;
  test.tm_mday = day;
  test.tm_hour = hour;
  test.tm_min  = minute;
  test.tm_sec  = second;
  test.tm_isdst = -1;

  time_t t0 = timegm(&begin) - 86400; // a weak workaround: on several platform (especially Windows 7 in Europe Timezones) mktime() is not capable of converting 19070-01-01T00:00:00 into seconds, because they try to convert it into UTC instead of localtime
  time_t t1 = mktime(&test);          // this might be error prone, see above

  return difftime(t1,t0) + (((double) millisecond) / 1000);
}

const char* rlTime::formatTimeDiff(double tdiff, enum FormatLargestUnit fmt, unsigned bufferLength, char* buffer)
{
  if (0 < bufferLength)
  {
    if (not buffer)
      buffer = new char[bufferLength];

    bool isNegative = (tdiff < 0);
    tdiff = fabs(tdiff);
    int milliseconds = ((int) (tdiff * 1000)) % 1000;
    time_t seconds = (time_t) tdiff;
    lldiv_t minutes, hours, days, weeks;

    minutes = lldiv(seconds, 60);

    const char* fmtString = "%s%d:%02d.%03d";
    switch (fmt)
    {
    case MinutesSecondsFraction:
      fmtString = "%s%d:%02d.%03d";
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), minutes.quot, minutes.rem, milliseconds);
      break;
    case HoursMinutesSecondsFraction:
      fmtString = "%s%d:%02d:%02d.%03d";
      hours = lldiv(minutes.quot, 60);
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), hours.quot, hours.rem, minutes.rem, milliseconds);
      break;
    case DaysHoursMinutesSecondsFraction:
      fmtString = "%s%d:%02d:%02d:%02d.%03d";
      hours = lldiv(minutes.quot, 60);
      days = lldiv(hours.quot, 24);
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), days.quot, days.rem, hours.rem, minutes.rem, milliseconds);
      break;
    case WeeksDaysHoursMinutesSecondsFraction:
      fmtString = "%s%d:%d:%02d:%02d:%02d.%03d";
      hours = lldiv(minutes.quot, 60);
      days = lldiv(hours.quot, 24);
      weeks = lldiv(days.quot, 7);
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), weeks.quot, weeks.rem, days.rem, hours.rem, minutes.rem, milliseconds);
      break;
    case MinutesSeconds:
      fmtString = "%s%d:%02d";
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), minutes.quot, minutes.rem);
      break;
    case HoursMinutesSeconds:
      fmtString = "%s%d:%02d:%02d";
      hours = lldiv(minutes.quot, 60);
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), hours.quot, hours.rem, minutes.rem);
      break;
    case DaysHoursMinutesSeconds:
      fmtString = "%s%d:%02d:%02d:%02d";
      hours = lldiv(minutes.quot, 60);
      days = lldiv(hours.quot, 24);
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), days.quot, days.rem, hours.rem, minutes.rem);
      break;
    case WeeksDaysHoursMinutesSeconds:
      fmtString = "%s%d:%d:%02d:%02d:%02d";
      hours = lldiv(minutes.quot, 60);
      days = lldiv(hours.quot, 24);
      weeks = lldiv(days.quot, 7);
      snprintf(buffer, bufferLength, fmtString, (isNegative ? "-" : ""), weeks.quot, weeks.rem, days.rem, hours.rem, minutes.rem);
      break;
    }
}

  return buffer;
}

const char* rlTime::formatTimeDiff(const rlTime& t1, const rlTime& t2, enum FormatLargestUnit fmt, unsigned bufferLength, char* buffer)
{
  return formatTimeDiff(t2 - t1, fmt, bufferLength, buffer);
}

std::string rlTime::formatTimeDiffString(double tdiff, enum FormatLargestUnit fmt)
{
  char strBuffer[32];

  const char* result = formatTimeDiff(tdiff, fmt, sizeof(strBuffer), strBuffer);

  std::string diffStr(result);

  return diffStr;
}

std::string rlTime::formatTimeDiffString(const rlTime& t1, const rlTime& t2, enum FormatLargestUnit fmt)
{
  return formatTimeDiffString(t2 - t1, fmt);
}

