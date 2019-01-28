/***************************************************************************
                          rltimeex.h  -  description
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
#ifndef _RL_TIME_EX_H_
#define _RL_TIME_EX_H_

#include "rldefine.h"
#include <time.h>
#include <stdint.h>
#include <string>

/*! <pre>
class for handling time.
</pre> */
class rlTimeEx
{
public:
  // local time type, on some platforms there is no 2038 safe definition available which we can avoid this way
  typedef int64_t time_t;   ///< for applying relative adjustments to the time, it's a redefintion on most modern platform, but anyway
  typedef double  ftime_t;  ///< for applying relative adjustments in fractions of a second

  rlTimeEx(int Year=0, int Month=0, int Day=0, int Hour=0, int Minute=0, int Second=0, int Millisecond=0);
#ifdef RLCPP11
  rlTimeEx(double&) = delete;
#endif
  virtual ~rlTimeEx();
  const char *getTimeString();
  const char *getIsoTimeString();
  const char *toString(const char *format);
  void    getLocalTime();
  int     getFileModificationTime(const char *filename);

  /*! <pre>
  format: sscanf(time_string,"%d-%d-%d %d:%d:%d %d",&year,&month,&day, &hour,&minute,&second, &millisecond);
  </pre> */
  void    setTimeFromString(const char *time_string);
  void    setTimeFromIsoString(const char *iso_time_string);
  void    setTimeFromSeconds(double const &seconds);   // we assume 30.5 days per month

  void    setLocalTime();
  double  secondsSinceEpoche() const;
  double  seconds() const;                      // we assume 30.5 days per month

  rlTimeEx& operator+=  (rlTimeEx &time);
  rlTimeEx& operator-=  (rlTimeEx &time);
  rlTimeEx  operator+   (rlTimeEx &time) const;
  rlTimeEx  operator-   (rlTimeEx &time) const;

  rlTimeEx& operator+=  (double const &seconds);       // we assume 30.5 days per month
  rlTimeEx& operator-=  (double const &seconds);       // we assume 30.5 days per month
  rlTimeEx  operator+   (double const &seconds) const; // we assume 30.5 days per month
  rlTimeEx  operator-   (double const &seconds) const; // we assume 30.5 days per month
  rlTimeEx& operator+=  (time_t seconds); ///< adjust absolute time by number of seconds
  rlTimeEx& operator-=  (time_t seconds); ///< adjust absolute time by number of seconds
  rlTimeEx  operator+   (time_t seconds) const; ///< make new object with absolute time adjusted by number of seconds
  rlTimeEx  operator-   (time_t seconds) const; ///< make new object with absolute time adjusted by number of seconds

  rlTimeEx& operator += (const ftime_t& seconds);       // we assume 30.5 days per month
  rlTimeEx& operator -= (const ftime_t& seconds);       // we assume 30.5 days per month
  rlTimeEx  operator +  (const ftime_t& seconds) const; // we assume 30.5 days per month
  rlTimeEx  operator -  (const ftime_t& seconds) const; // we assume 30.5 days per month

  int     operator==  (const rlTimeEx &time) const;
  int     operator<   (const rlTimeEx &time) const;
  int     operator<=  (const rlTimeEx &time) const;
  int     operator>   (const rlTimeEx &time) const;
  int     operator>=  (const rlTimeEx &time) const;

  int     year;
  int     month;
  int     day;
  int     hour;
  int     minute;
  int     second;
  int     millisecond;

  enum FormatLargestUnit
  {
    MinutesSecondsFraction = 0, ///< "0:00.000", needs at least 9 bytes buffer
    HoursMinutesSecondsFraction, ///< "0:00:00.000", needs at least 12 bytes buffer
    DaysHoursMinutesSecondsFraction, ///< "0:00:00:00.000", needs at least 15 bytes buffer
    WeeksDaysHoursMinutesSecondsFraction, ///< "0:0:00:00:00.000", needs at least 17 bytes buffer
    MinutesSeconds, ///< "0:00", needs at least 5 bytes buffer
    HoursMinutesSeconds, ///< "0:00:00", needs at least 8 bytes buffer
    DaysHoursMinutesSeconds, ///< "0:00:00:00", needs at least 11 bytes buffer
    WeeksDaysHoursMinutesSeconds, ///< "0:0:00:00:00", needs at least 13 bytes buffer
  };

  ///< Caller chooses formatting template, default is Hours:Minutes:Seconds.Milliseconds, caller provides buffer, or, NULL pointer, buffer must be deleted (delete[])
  static
  const char* formatTimeDiff(double, enum FormatLargestUnit = HoursMinutesSecondsFraction, unsigned bufferLength = 32, char* buffer = 0);

  ///< Caller chooses formatting template, default is Hours:Minutes:Seconds.Milliseconds, caller provides buffer, or buffer must be deleted (delete[])
  static
  const char* formatTimeDiff(const rlTime& t1, const rlTime& t2, enum FormatLargestUnit = HoursMinutesSecondsFraction, unsigned bufferLength = 32, char* buffer = 0);

  ///< Caller chooses formatting template, default is Hours:Minutes:Seconds.Milliseconds, returned object manages string memory
  static
  std::string formatTimeDiffString(double, enum FormatLargestUnit = HoursMinutesSecondsFraction);

  ///< Caller chooses formatting template, default is Hours:Minutes:Seconds.Milliseconds, returned object manages string memory
  static
  std::string formatTimeDiffString(const rlTime& t1, const rlTime& t2, enum FormatLargestUnit = HoursMinutesSecondsFraction);

  static
  time_t timegm(struct tm* tm_); ///< emulates the POSIX function, which is i.e. under Windows not available

  void normalizeAsDate(); ///< normalizes odd constructions of time and date, such 2014-03-36 is normalized to 2014-04-05, simelar oddities for the time are corrected.

private:
  char    time_string[32*2];   // 2001-11-23 12:52:60 056
  char    iso_time_string[32]; // 2001-11-23T12:52:60.056

#ifndef RLCPP11
  explicit rlTimeEx(double&);  // intentionally no implementation, as this shall not be explicit or implicit used
#endif
};
#endif
