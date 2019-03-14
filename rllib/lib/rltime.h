
/***************************************************************************
                          rltime.h  -  description
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
#ifndef _RL_TIME_V1_H_
#define _RL_TIME_V1_H_

#include "rldefine.h"

/*! <pre>
class for handling time.
</pre> */
class rlTime
{
public:
  rlTime(int Year=0, int Month=0, int Day=0, int Hour=0, int Minute=0, int Second=0, int Millisecond=0);
  virtual ~rlTime();
  const char *version();
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
  void    setLocalTime();
  double  secondsSinceEpoche();
  rlTime& operator+=  (rlTime &time);
  rlTime& operator-=  (rlTime &time);
  rlTime  operator+   (rlTime &time);
  rlTime  operator-   (rlTime &time);
  int     operator==  (rlTime &time);
  int     operator<   (rlTime &time);
  int     operator<=  (rlTime &time);
  int     operator>   (rlTime &time);
  int     operator>=  (rlTime &time);
  int     year;
  int     month;
  int     day;
  int     hour;
  int     minute;
  int     second;
  int     millisecond;
private:
  char    time_string[32];     // 2001-11-23 12:52:60 056
  char    iso_time_string[32]; // 2001-11-23T12:52:60.056
};

#endif
