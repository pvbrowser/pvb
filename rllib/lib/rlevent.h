/***************************************************************************
                          rlevent.h  -  description
                             -------------------
    begin                : Wed Dec 18 2002
    copyright            : (C) 2002 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
/*! <pre>
These functions allow you to send event log messages over TCP/IP to the event log server
implemented by the pvserver pcontrol (available as separate download)

See also the class rlEventLogServer.
</pre> */
#ifndef _RL_EVENT_H_
#define _RL_EVENT_H_

#include "rldefine.h"
#define rlMAX_EVENT 256 // maximum size of an event message

static const char rlevent_name[][4] = { "INF", "WAR", "ERR", "CRI", "FAT", "TST" };
enum EventTypes
{
  rlInfo = 0,
  rlWarning,
  rlError,
  rlCritical,
  rlFatal,
  rlTest,
  rlEVENT_SIZE
};

#ifdef XXXunix
#define rlEvent(typ, format, args...) { rlSetEventLocation(__FILE__,__LINE__); rlEventPrintf( typ, format, args); }
#else
#define rlEvent rlSetEventLocation(__FILE__,__LINE__);rlEventPrintf
#endif

/*! <pre>
initialize rlEvent
parameters:
-eventhost=adr
-eventport=num
</pre> */
void rlEventInit(int ac, char **av, const char *module);

/*! <pre>
set the location where rlEvent is called
</pre> */
void rlSetEventLocation(const char *file, int line);

/*! <pre>
output the message
</pre> */
void rlEventPrintf(int event_type, const char *format, ...);

#endif
