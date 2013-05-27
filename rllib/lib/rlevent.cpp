/***************************************************************************
                          rlevent.cpp  -  description
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
#include "rlevent.h"
#include "rlsocket.h"
#include "rltime.h"
#include "rlcutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static rlSocket toEventLogServer("localhost",-1,1); // standard localhost disabled
static rlTime   mytime;
static char rlmodule[32];
static char rllocation[rlMAX_EVENT];
static char rlmessage[rlMAX_EVENT];
#ifdef __VMS
static char rlfinal[rlMAX_EVENT*4];
#else
static char rlfinal[rlMAX_EVENT];
#endif

void rlEventInit(int ac, char **av, const char *module)
{
  const char *cptr;

  rlwsa();
  for(int i=0; i<ac; i++)
  {
    cptr = av[i];
    if(strncmp("-eventhost=",cptr,11) == 0) toEventLogServer.setAdr(&cptr[11]);
    if(strncmp("-eventport=",cptr,11) == 0) toEventLogServer.setPort(atoi(&cptr[11]));
  }
  rlstrncpy(rlmodule,module,sizeof(rlmodule)-1);
}

void rlSetEventLocation(const char *file, int line)
{
#ifdef RLWIN32
  const char *cptr;

  if(strchr(file,'\\') == NULL) cptr = NULL;
  else                          cptr = strrchr(file,'\\');
  if(cptr != NULL) cptr++;
  else             cptr = file;
#endif
#ifdef __VMS
  const char *cptr;

  if(strchr(file,']') == NULL) cptr = NULL;
  else                         cptr = strrchr(file,']');
  if(cptr != NULL) cptr++;
  else             cptr = file;
#endif

#ifdef RLUNIX
  snprintf(rllocation,sizeof(rllocation)-1,":%s:%d",file,line);
#endif

#ifdef __VMS
  sprintf(rllocation,":%s:%d",file,line);
#endif

#ifdef RLWIN32
  _snprintf(rllocation,sizeof(rllocation)-1,":%s:%d",file,line);
#endif
}

void rlEventPrintf(int event_type, const char *format, ...)
{
  if(toEventLogServer.getPort() <= 0) return;
  va_list ap;
  va_start(ap,format);
  rlvsnprintf(rlmessage,sizeof(rlmessage)-1,format,ap);
  va_end(ap);

  if(event_type < 0            ) event_type = rlError;
  if(event_type >= rlEVENT_SIZE) event_type = rlError;
  mytime.getLocalTime();
#ifdef RLUNIX
  snprintf(rlfinal,sizeof(rlfinal)-1,"%s %s %s %s%s\n",
           rlevent_name[event_type],
           mytime.getTimeString(),
           rlmessage,
           rlmodule,
           rllocation
          );
#endif
#ifdef __VMS
  sprintf(rlfinal,"%s %s %s %s%s\n",
           rlevent_name[event_type],
           mytime.getTimeString(),
           rlmessage,
           rlmodule,
           rllocation
          );
  rlfinal[rlMAX_EVENT-1] = '\0';
#endif
#ifdef RLWIN32
  _snprintf(rlfinal,sizeof(rlfinal)-1,"%s %s %s %s%s\n",
           rlevent_name[event_type],
           mytime.getTimeString(),
           rlmessage,
           rlmodule,
           rllocation
          );
#endif
  if(toEventLogServer.isConnected())
  {
    toEventLogServer.printf("%s",rlfinal);
  }
  else
  {
    toEventLogServer.connect();
    if(toEventLogServer.isConnected())
    {
      toEventLogServer.printf("%s",rlfinal);
    } // else give up
  }
}

