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
#include <assert.h>

#ifdef RLCPP11
#include <type_traits>
#endif

/*! <pre>
class for handling time. It supports also simple formatting of time differences.

If the year is smaller than 1970 a relative time is assumed in contrast to an absolute point in time for
years greater than 1970.

For good clarity of what you want express, avoid years and months for relative times and express relative
times as days, hours etc.

Relative times are normalized in a fashion that, as of its nonlinear nature of months length and year length,
finally the most rough granularity component for relative times is expressed in days. This way are assumptions of
month length and year length are taken out of calculations as early as possible.
</pre> */
class rlTimeEx
{
public:
  // local time type, on some platforms there is no 2038 safe definition available which we can avoid this way
  typedef int64_t time_t;   ///< for applying relative adjustments to the time, it's a redefintion on most modern platform, but anyway
  typedef double  ftime_t;  ///< for applying relative adjustments in fractions of a second

  struct RelativeTime; ///< a proxy type for adding relative time to absolutes

  explicit
  rlTimeEx(int Year=0, int Month=0, int Day=0, int Hour=0, int Minute=0, int Second=0, int Millisecond=0);
#ifdef RLCPP11
  rlTimeEx(double&) = delete;
#endif
  virtual ~rlTimeEx();
  const char *getTimeString();
  const char *getIsoTimeString();
  const char *toString(const char *format);
  rlTimeEx& getLocalTime();
  int     getFileModificationTime(const char *filename);

  /*! <pre>
  format: sscanf(time_string,"%d-%d-%d %d:%d:%d %d",&year,&month,&day, &hour,&minute,&second, &millisecond);
  </pre> */
  rlTimeEx& setTimeFromString(const char *time_string);
  rlTimeEx& setTimeFromIsoString(const char *iso_time_string);
  rlTimeEx& setTimeFromSeconds(double seconds);   // we assume 30.5 days per month

  void    setLocalTime();
  double  secondsSinceEpoche() const;
  double  seconds() const;

  // implicitly converted rlTimeEx objects for doing relative adjustments to objects
  rlTimeEx& operator+=  (const RelativeTime &time);
  rlTimeEx& operator-=  (const RelativeTime &time);
  rlTimeEx  operator+   (const RelativeTime &time) const;
  rlTimeEx  operator-   (const RelativeTime &time) const; // one must cast explicitly to get an rlTimeEx object back, i.e. t2 - RelativeTime(t1);

  // integral time spans, pre C++11 compiler users must convert argument value explicitly to time_t by time_t(seconds)
  rlTimeEx& operator+=  (time_t seconds); ///< adjust absolute time by number of seconds
  rlTimeEx& operator-=  (time_t seconds); ///< adjust absolute time by number of seconds
  rlTimeEx  operator+   (time_t seconds) const; ///< make new object with absolute time adjusted by number of seconds
  rlTimeEx  operator-   (time_t seconds) const; ///< make new object with absolute time adjusted by number of seconds

  // floating point time spans with millisecond resolution, pre C++11 compiler users must convert argument value to ftime_t by ftime_t(seconds)
  rlTimeEx& operator += (const ftime_t& seconds);
  rlTimeEx& operator -= (const ftime_t& seconds);
  rlTimeEx  operator +  (const ftime_t& seconds) const;
  rlTimeEx  operator -  (const ftime_t& seconds) const;

#ifdef RLCPP11
  // some templates to catch all other integer type arguments and convert them to time_t
  template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
  rlTimeEx& operator+=  (argType seconds); ///< adjust absolute time by number of seconds
  template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
  rlTimeEx& operator-=  (argType seconds); ///< adjust absolute time by number of seconds
  template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
  rlTimeEx  operator+   (argType seconds) const; ///< make new object with absolute time adjusted by number of seconds
  template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
  rlTimeEx  operator-   (argType seconds) const; ///< make new object with absolute time adjusted by number of seconds

  // some templates to catch all other floating point type arguments and convert them to ftime_t
  template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
  rlTimeEx& operator+=  (argType seconds); ///< adjust absolute time by number of seconds
  template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
  rlTimeEx& operator-=  (argType seconds); ///< adjust absolute time by number of seconds
  template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
  rlTimeEx  operator+   (argType seconds) const; ///< make new object with absolute time adjusted by number of seconds
  template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
  rlTimeEx  operator-   (argType seconds) const; ///< make new object with absolute time adjusted by number of seconds
#endif

  double  operator-   (const rlTimeEx &time) const; ///< difference of two points in time in seconds.milliseconds

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
  const char* formatTimeDiff(ftime_t, enum FormatLargestUnit = HoursMinutesSecondsFraction, unsigned bufferLength = 32, char* buffer = 0);

  ///< Caller chooses formatting template, default is Hours:Minutes:Seconds.Milliseconds, caller provides buffer, or buffer must be deleted (delete[])
  static
  const char* formatTimeDiff(const rlTimeEx& t1, const rlTimeEx& t2, enum FormatLargestUnit = HoursMinutesSecondsFraction, unsigned bufferLength = 32, char* buffer = 0);

  ///< Caller chooses formatting template, default is Hours:Minutes:Seconds.Milliseconds, returned object manages string memory
  static
  std::string formatTimeDiffString(ftime_t, enum FormatLargestUnit = HoursMinutesSecondsFraction);

  ///< Caller chooses formatting template, default is Hours:Minutes:Seconds.Milliseconds, returned object manages string memory
  static
  std::string formatTimeDiffString(const rlTimeEx& t1, const rlTimeEx& t2, enum FormatLargestUnit = HoursMinutesSecondsFraction);

  static
  time_t timegm(struct tm* tm_); ///< emulates the POSIX function, which is i.e. under Windows not available

  void normalizeAsDate(); ///< normalizes odd constructions of time and date, such 2014-03-36 is normalized to 2014-04-05, simelar oddities for the time are corrected.
  void normalizeAsRelativeTime(); // we assume 30.5 days per month and 365.25 days per year and accumulate months and years in the days component

  struct RelativeTime // inline implmentation of the proxy type for adding relative time to absolutes
  {
    inline
    RelativeTime(const rlTimeEx& rlt)
    : m_myParent_(rlt)
    {
      assert(rlt.objectHoldsRelativeTime);
    }

    inline
    operator double() const
    {
      return m_myParent_.seconds();
    }

    const rlTimeEx& m_myParent_;
  };

private:
  char    time_string[32*2];   // 2001-11-23 12:52:60 056
  char    iso_time_string[32]; // 2001-11-23T12:52:60.056

  bool    objectHoldsRelativeTime:1;

#ifndef RLCPP11
  explicit rlTimeEx(double&);  // intentionally no implementation, as this shall not be explicit or implicit used
#endif
};

#ifdef RLCPP11
// implementation of the template declarations and overloads above
template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
rlTimeEx& rlTimeEx::operator+=  (argType seconds)
{
  return this->operator +=(time_t(seconds));
}

template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
rlTimeEx& rlTimeEx::operator-=  (argType seconds)
{
  return this->operator -=(time_t(seconds));
}

template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
rlTimeEx  rlTimeEx::operator+   (argType seconds) const
{
  return this->operator +(time_t(seconds));
}

template<typename argType, typename std::enable_if<std::is_integral<argType>::value, argType>::type* = nullptr>
rlTimeEx  rlTimeEx::operator-   (argType seconds) const
{
  return this->operator -(time_t(seconds));
}

template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
rlTimeEx& rlTimeEx::operator+=  (argType seconds)
{
  return this->operator +=(ftime_t(seconds));
}

template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
rlTimeEx& rlTimeEx::operator-=  (argType seconds)
{
  return this->operator -=(ftime_t(seconds));
}

template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
rlTimeEx  rlTimeEx::operator+   (argType seconds) const
{
  return this->operator +(ftime_t(seconds));
}

template<typename argType, typename std::enable_if<std::is_floating_point<argType>::value, argType>::type* = nullptr>
rlTimeEx  rlTimeEx::operator-   (argType seconds) const
{
  return this->operator -(ftime_t(seconds));
}
#endif

#endif
