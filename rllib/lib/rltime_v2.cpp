/***************************************************************************
                      rltime_v2.cpp  -  description
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
#include "rltime_v2.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>
#include <limits>

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

namespace ns_rltime_v2
{

void rlTime::normalizeAsRelativeTime()
{
  assert(objectHoldsRelativeTime && "object must represent relative time, otherwise normalization as relative time is not defined");

  // accumulate years and months into days, assuming 30.5 days per month and 365.25 days per year
  if (year != 0)
  {
    day += year * 365 + (year / 4);
    year = 0;
  }

  if (month != 0)
  {
    day += month * 30 + (month / 2);
    month = 0;
  }

  if (
  (1000 <= std::abs(millisecond)) or
      (60 <= std::abs(second)) or
      (60 <= std::abs(minute)) or
      (24 <= std::abs(hour))
      )
  {
    time_t linear_value = second + (minute * 60) + (hour * 60 * 60) + (day * 24 * 60 * 60);

    // normalize milliseconds
    std::lldiv_t d = std::lldiv(millisecond, 1000);
    millisecond = d.rem;
    while (0 > millisecond)
    {
      millisecond += 1000;
      --d.quot;
    }

    // adjust linear value
    linear_value += d.quot;

    // normalize milliseconds to negative, if linear value is negative
    if (0 > linear_value)
    {
      if (0 < millisecond)
      {
        millisecond -= 1000;
        ++linear_value;
      }
    }

    // decompose linear value
    if (linear_value)
    {
      d = std::lldiv(linear_value, 60);
      second = d.rem;
      if (d.quot)
      {
        d = std::lldiv(d.quot, 60);
        minute = d.rem;
        if (d.quot)
        {
          d = std::lldiv(d.quot, 24);
          hour = d.rem;
          day = d.quot;
        }
      }
    }
  }
}

void rlTime::normalizeAsDate()
{
  assert(not objectHoldsRelativeTime && "object must represent absolute time and date, otherwise normalization as time and date is not defined");
  assert(year >= 1970);

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
    time_t ep = timegm(&t);
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

  memset(time_string, 0, sizeof(time_string));
  memset(iso_time_string, 0, sizeof(iso_time_string));

  if (1970 <= year)
  {
    objectHoldsRelativeTime = false;
  normalizeAsDate();
}
  else
{
    objectHoldsRelativeTime = true;
    normalizeAsRelativeTime();
  }
}

rlTime::~rlTime()
{}

/***
 * Read an absolute point in time from time_string.
 *
 * This method expects a time formatted as in return from getTimeString(),
 * following the template "YYYY-MM-DD hh:mm:ss ms".
 * @param time_string
 */
rlTime& rlTime::setTimeFromString(const char *time_string)
{
  year        = 0;
  month       = 0;
  day         = 0;
  hour        = 0;
  minute      = 0;
  second      = 0;
  millisecond = 0;
  objectHoldsRelativeTime = false;
  sscanf(time_string,"%d-%d-%d %d:%d:%d %d",&year,&month,&day, &hour,&minute,&second, &millisecond);
  normalizeAsDate();

  return *this;
}

/***
 * Read an absolute point in time from time_string.
 *
 * This method expects a time formatted as in return from getIsoTimeString(),
 * following the template "YYYY-MM-DDThh:mm:ss.xxx".
 * @param time_string
 */
rlTime& rlTime::setTimeFromIsoString(const char *iso_time_string)
{
  year        = 0;
  month       = 0;
  day         = 0;
  hour        = 0;
  minute      = 0;
  second      = 0;
  millisecond = 0;
  objectHoldsRelativeTime = false;
  sscanf(iso_time_string,"%d-%d-%dT%d:%d:%d.%d",&year,&month,&day, &hour,&minute,&second, &millisecond);
  normalizeAsDate();

  return *this;
}

/*! <pre>
 * seconds := delta time
 * milliseconds within the fraction
 * calculating with 1 month <=> 30.5 days
 </pre> */
void rlTime::setTimeFromSeconds(double seconds)
{ // we assume that the average month has 30.5 days
  double mod = fmod(seconds * 1000, 1000.);
  millisecond = (int) mod;

  mod = fmod(seconds, 60.);
  second = (int) mod;

  seconds /= 60;
  mod = fmod(seconds, 60.);
  minute = (int) mod;

  seconds /= 60;
  mod = fmod(seconds, 24.);
  hour = (int) mod;

  seconds /= 24;
  day = int(seconds);

//  mod = fmod(seconds, 30.5);
//  day = (int) mod;
//
//  seconds /= 30.5;
//  mod = fmod(seconds, 12);
//  month = (int) mod;
//
//  seconds /= 12;
//  year = (int) seconds;

  objectHoldsRelativeTime = true;
  normalizeAsRelativeTime();

  return *this;
}


const char *rlTime::getTimeString()
{
  assert(not objectHoldsRelativeTime && "object must hold an absolute time and date");
  memset(time_string, 0, sizeof(time_string));
  sprintf(time_string,"%04d-%02d-%02d %02d:%02d:%02d %03d",year, month, day, hour, minute, second, millisecond);
  return time_string;
}

const char *rlTime::getIsoTimeString()
{
  assert(not objectHoldsRelativeTime && "object must hold an absolute time and date");
  memset(iso_time_string, 0, sizeof(iso_time_string));
  sprintf(iso_time_string,"%04d-%02d-%02dT%02d:%02d:%02d.%03d",year, month, day, hour, minute, second, millisecond);
  return iso_time_string;
}

/*! <pre>
 * Returns the datetime as a string. The format parameter determines the format of the result string.
 *
 * These expressions may be used for the date:
 * Expression	Output
 * d	the day as number without a leading zero (1 to 31)
 * dd	the day as number with a leading zero (01 to 31)
 * M	the month as number without a leading zero (1-12)
 * MM	the month as number with a leading zero (01-12)
 * MMM	the abbreviated localized month name (e.g. 'Jan' to 'Dec').
 * yy	the year as two digit number (00-99)
 * yyyy	the year as four digit number
 *
 * These expressions may be used for the time:
 * Expression	Output
 * h	the hour without a leading zero (1 to 12 if AM/PM display)
 * hh	the hour with a leading zero (01 to 12 if AM/PM display)
 * H	the hour without a leading zero (0 to 23, even with AM/PM display)
 * HH	the hour with a leading zero (00 to 23, even with AM/PM display)
 * m	the minute without a leading zero (0 to 59)
 * mm	the minute with a leading zero (00 to 59)
 * s	the whole second without a leading zero (0 to 59)
 * ss	the whole second with a leading zero where applicable (00 to 59)
 * z	the fractional part of the second, to go after a decimal point, without trailing zeroes (0 to 999).
 * zzz	the fractional part of the second, to millisecond precision, including trailing zeroes where applicable (000 to 999).
 * AP or A	use AM/PM display. A/AP will be replaced by either "AM" or "PM".
 * ap or a	use am/pm display. a/ap will be replaced by either "am" or "pm".
 *
 * All other input characters will be copyed
 *
 * Example format strings (assumed that the rlTime is 21 May 2001 14:13:09.120):
 * Format	Result
 * dd.MM.yyyy	    21.05.2001
 * ddd MMMM d yy	Tue May 21 01
 * hh:mm:ss.zzz	  14:13:09.120
 * hh:mm:ss.z	    14:13:09.12
 * h:m:s ap	      2:13:9 pm
 </pre> */
const char *rlTime::toString(const char *format)
{
  memset(time_string, 0, sizeof(time_string));

  // See:
  // https://doc.qt.io/qt-5/qdatetime.html#toString
  //
  char buf[16];
  char *dest = time_string;
  while(*format != '\0')
  {
    if     (strncmp(format,"dd",2) == 0)
    {
      sprintf(buf,"%02d",day);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 2;
    }
    else if(strncmp(format,"d",1) == 0)
    {
      sprintf(buf,"%d",day);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 1;
    }
    else if(strncmp(format,"MMM",3) == 0)
    {
      buf[0] = '\0';
      if(month == 1) strcpy(buf,"Jan");
      if(month == 2) strcpy(buf,"Feb");
      if(month == 3) strcpy(buf,"Mar");
      if(month == 4) strcpy(buf,"Apr");
      if(month == 5) strcpy(buf,"May");
      if(month == 6) strcpy(buf,"Jun");
      if(month == 7) strcpy(buf,"Jul");
      if(month == 8) strcpy(buf,"Aug");
      if(month == 9) strcpy(buf,"Sep");
      if(month == 10) strcpy(buf,"Oct");
      if(month == 11) strcpy(buf,"Nov");
      if(month == 12) strcpy(buf,"Dec");
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 3;
    }
    else if(strncmp(format,"MM",2) == 0)
    {
      sprintf(buf,"%02d",month);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 2;
    }
    else if(strncmp(format,"M",1) == 0)
    {
      sprintf(buf,"%d",month);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 1;
    }
    else if(strncmp(format,"yyyy",4) == 0)
    {
      sprintf(buf,"%4d",year);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 4;
    }
    else if(strncmp(format,"yy",2) == 0)
    {
      sprintf(buf,"%4d",year);
      strcpy(dest,&buf[2]);
      dest   += strlen(&buf[2]);
      format += 2;
    }
    else if(strncmp(format,"hh",2) == 0)
    {
      if     (hour >  12) sprintf(buf,"%02d", hour - 12);
      else if(hour == 0)  sprintf(buf,"%02d", 12);
      else                sprintf(buf,"%02d", hour);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 2;
    }
    else if(strncmp(format,"h",1) == 0)
    {
      if     (hour >  12) sprintf(buf,"%2d", hour - 12);
      else if(hour == 0)  sprintf(buf,"%2d", 12);
      else                sprintf(buf,"%2d", hour);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 1;
    }
    else if(strncmp(format,"HH",2) == 0)
    {
      sprintf(buf,"%02d",hour);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 2;
    }
    else if(strncmp(format,"H",1) == 0)
    {
      sprintf(buf,"%d",hour);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 1;
    }
    else if(strncmp(format,"mm",2) == 0)
    {
      sprintf(buf,"%02d",minute);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 2;
    }
    else if(strncmp(format,"m",1) == 0)
    {
      sprintf(buf,"%d",minute);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 1;
    }
    else if(strncmp(format,"ss",2) == 0)
    {
      sprintf(buf,"%02d",second);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 2;
    }
    else if(strncmp(format,"s",1) == 0)
    {
      sprintf(buf,"%d",second);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 1;
    }
    else if(strncmp(format,"zzz",3) == 0)
    {
      sprintf(buf,"%03d",millisecond);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 3;
    }
    else if(strncmp(format,"z",1) == 0)
    {
      sprintf(buf,"%d",millisecond);
      strcpy(dest,buf);
      dest   += strlen(buf);
      format += 1;
    }
    else if(strncmp(format,"AP",2) == 0)
    {
      if     (hour == 0) strcpy(dest,"PM");
      else if(hour < 13) strcpy(dest,"AM");
      else               strcpy(dest,"PM");
      dest   += strlen("AM");
      format += 2;
    }
    else if(strncmp(format,"ap",2) == 0)
    {
      if     (hour == 0) strcpy(dest,"pm");
      else if(hour < 13) strcpy(dest,"am");
      else               strcpy(dest,"pm");
      dest   += strlen("am");
      format += 2;
    }
    else if(strncmp(format,"A",1) == 0)
    {
      if     (hour == 0) strcpy(dest,"PM");
      else if(hour < 13) strcpy(dest,"AM");
      else               strcpy(dest,"PM");
      dest   += strlen("AM");
      format += 1;
    }
    else if(strncmp(format,"a",1) == 0)
    {
      if     (hour == 0) strcpy(dest,"pm");
      else if(hour < 13) strcpy(dest,"am");
      else               strcpy(dest,"pm");
      dest   += strlen("am");
      format += 1;
    }
    else
    {
      *dest++ = *format++;
    }
    if(dest - time_string + 6 > (int) sizeof(time_string)) break;
  }
  *dest = '\0';
  return time_string;
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

  objectHoldsRelativeTime = false;

  return *this;
}

int rlTime::getFileModificationTime(const char *filename)
{
  struct stat statbuf;
  struct tm *tms;
  struct tm tmsbuf;

#ifdef RLUNIX
  if(lstat(filename, &statbuf)) return -1;
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
  objectHoldsRelativeTime = false;

  return 0;
}

void rlTime::setLocalTime()
{
  assert(not this->objectHoldsRelativeTime && "cannot set local time/clock from relative time object");

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

  objectHoldsRelativeTime = false;
}

rlTime& rlTime::operator-=(const rlTime &time)
{
  if (0 > seconds)
    return this->operator -=(-seconds);

  std::lldiv_t d = std::lldiv(seconds, time_t(60));
  second += d.rem;
  if(d.quot)
  {
    d = std::lldiv(d.quot, time_t(60));
    minute += d.rem;
    if(d.quot)
    {
      d = std::lldiv(d.quot, time_t(24));
      hour += d.rem;
      if (d.quot)
      {
        if (not objectHoldsRelativeTime)
        {
          d = std::lldiv(d.quot, time_t(31));
        day += d.rem;
        if(d.quot)
        {
            d = std::lldiv(d.quot, time_t(12));
          month += d.rem;
          year += d.quot;
        }
      }
        else // do not split remaining days down to months and years, as this is not linear
        {
          day += d.quot;
        }
      }
    }
  }

  // now normalize the values, as this is not done above
  if (not objectHoldsRelativeTime)
    normalizeAsDate();
  else
    normalizeAsRelativeTime();

  return *this;
}

rlTime& rlTime::operator-=(time_t seconds)
{
  if (0 > seconds)
    return this->operator +=(-seconds);

  std::lldiv_t d = std::lldiv(seconds, time_t(60));
  second -= d.rem;
  if(second < 0)
  {
    ++d.quot;
    second += 60;
  }
  if(d.quot)
  {
    d = std::lldiv(d.quot, time_t(60));
    minute -= d.rem;
    if(minute < 0)
    {
      ++d.quot;
      minute += 60;
    }
    if(d.quot)
    {
      d = std::lldiv(d.quot, time_t(24));
      hour -= d.rem;
      if(hour < 0)
      {
        ++d.quot;
        hour += 24;
      }
      if(d.quot)
      {
        if (not objectHoldsRelativeTime)
        {
          d = std::lldiv(d.quot, time_t(31));
        day -= d.rem;
        if(day < 0)
        {
          ++d.quot;
          day += 31;
        }
        if(d.quot)
        {
            d = std::lldiv(d.quot, time_t(12));
          month -= d.rem;
          if(month < 0)
          {
            ++d.quot;
            month += 12;
          }
          year -= d.quot;
        }
      }
        else // do not split remaining days down to months and years, as this is not linear
        {
          day -= d.quot;
        }
      }
    }
  }

  // now normalize the values, as this is not done above
  if (not objectHoldsRelativeTime)
    normalizeAsDate();
  else
    normalizeAsRelativeTime();

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

rlTime& rlTime::operator +=(const double& seconds)
{
  int ms = fmod(round(seconds * 1000.), 1000.);
  time_t sec = time_t(seconds);

  millisecond += ms;
  *this += sec;

  return *this;
}

rlTime& rlTime::operator -=(const double& seconds)
{
  int ms = fmod(round(seconds * 1000.), 1000.);
  time_t sec = time_t(seconds);

  millisecond -= ms;
  *this -= sec;

  return *this;
}

rlTime rlTime::operator +(const double& seconds) const
                              {
  rlTime t(*this);

  t += seconds;

  return t;
}

rlTime rlTime::operator -(const double& seconds) const
                              {
  rlTime t(*this);

  t -= seconds;

  return t;
}

rlTime& rlTime::operator+=(const RelativeTime &t)
{
  time_t linear_value = t.m_myParent_.second + (t.m_myParent_.minute * 60) + (t.m_myParent_.hour * 60 * 60) + (t.m_myParent_.day * 24 * 60 * 60);
  this->millisecond += t.m_myParent_.millisecond;
  *this += linear_value;

  return *this;
}

rlTime& rlTime::operator-=(const RelativeTime &t)
{
  time_t linear_value = t.m_myParent_.second + (t.m_myParent_.minute * 60) + (t.m_myParent_.hour * 60 * 60) + (t.m_myParent_.day * 24 * 60 * 60);
  this->millisecond -= t.m_myParent_.millisecond;
  *this -= linear_value;

  return *this;
}

rlTime rlTime::operator+(const RelativeTime &time) const
{
  rlTime t(*this);

  t += time;

  return t;
}

rlTime rlTime::operator-(const RelativeTime &time) const
{
  rlTime t(*this);

  t -= time;

  return t;
}

double rlTime::operator-(const rlTime &time) const
                           {
  if (this->objectHoldsRelativeTime and time.objectHoldsRelativeTime)
    return (this->seconds() - time.seconds());
  else if (not this->objectHoldsRelativeTime and time.objectHoldsRelativeTime)
    return this->secondsSinceEpoche() - time.seconds();
  else if (not this->objectHoldsRelativeTime and not time.objectHoldsRelativeTime)
    return (this->secondsSinceEpoche() - time.secondsSinceEpoche());
  else
  {
    assert(not time.objectHoldsRelativeTime && "you cannot run the difference between relative and absolute time this way (relative minus absolute)");
    return std::numeric_limits<double>::quiet_NaN();; // just to make code inspection tools happy, in case assert() is defined empty
  }
}

int rlTime::operator==(const rlTime &time) const
                         {
  if (objectHoldsRelativeTime != time.objectHoldsRelativeTime)
    return 0;
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
  if (objectHoldsRelativeTime != time.objectHoldsRelativeTime)
    return 0;
  if((*this) == time) return 1;
  if((*this) <  time) return 1;
  return 0;
}

int rlTime::operator>=(const rlTime &time) const
                         {
  if (objectHoldsRelativeTime != time.objectHoldsRelativeTime)
    return 0;
  if((*this) == time) return 1;
  if((*this) >  time) return 1;
  return 0;
}

int rlTime::operator<(const rlTime &time) const
                        {
  if (objectHoldsRelativeTime != time.objectHoldsRelativeTime)
    return 0;
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
  if (objectHoldsRelativeTime != time.objectHoldsRelativeTime)
    return 0;
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
  time_t t = mktime(tm_);
  struct tm ltm;
  struct tm gtm;
  struct tm *lt = localtime_r(&t, &ltm);
  struct tm *gt = gmtime_r(&t, &gtm);
  time_t diff = mktime(lt) - mktime(gt);
  return t + diff;
}

double rlTime::secondsSinceEpoche() const
{
  assert(not objectHoldsRelativeTime && "object must hold an absolute time and date");

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

  time_t t0 = timegm(&begin) - 86400; // a weak workaround: on several platform (especially Windows 7 in Europe Timezones) mktime() is not capable of converting 1970-01-01T00:00:00 into seconds, because they try to convert it into UTC instead of localtime
  time_t t1 = mktime(&test);          // this might be error prone, see above

  return difftime(t1,t0) + (((double) millisecond) / 1000);
}

/**
 * seconds := delta time
 * milliseconds within the fraction
 */
double rlTime::seconds() const
{
  assert(objectHoldsRelativeTime && "object must hold an relative time");

  double ret = (((double) millisecond) / 1000);
  ret += second + minute * 60 + hour * 60 * 60 + day * 24 * 60 * 60;
  ret += month*60*60*30.5 + year*60*60*30.5*12;
  return ret;
}

const char* rlTime::formatTimeDiff(double tdiff, enum FormatLargestUnit fmt, unsigned bufferLength, char* buffer)
{
  if(0 < bufferLength)
  {
    if(not buffer) buffer = new char[bufferLength];

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

const char *rlTime::formatTimeDiffString(double tdiff, enum FormatLargestUnit fmt)
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

} // end of namespace ns_rltime_v2
