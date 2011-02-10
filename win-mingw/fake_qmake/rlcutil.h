/***************************************************************************
                          rlcutil.cpp  -  description
                             -------------------
    begin                : Wed Dec 11 2002
    copyright            : (C) 2002 by Rainer Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_CUTIL_H_
#define _RL_CUTIL_H_

#include <stdarg.h>
#include "rldefine.h"

/*! <pre>
  like printf for debugging 
</pre> */
int rlSetDebugPrintf(int state);
int rlDebugPrintf(const char *format, ...);

/*! <pre>
  like printf in the last line of a terminal
</pre> */
int rlLastLinePrintf(const char *format, ...);

#ifdef unix
/*! <pre>
  call execvp(arg[0],arg) on unix
</pre> */
int rlexec(const char *command);
#endif

/*! <pre>
  encode plain text password p
</pre> */
const char *rlpass(const char *p);

/*! <pre>
  strncpy + terminate with '\\0'
</pre> */
char *rlstrncpy(char *dest, const char *source, int n);

/*! <pre>
  like vsnprintf but portable
</pre> */
int rlvsnprintf(char *text, int len, const char *format, va_list ap);

/*! <pre>
  like snprintf but portable
</pre> */
int rlsnprintf(char *text, int len, const char *format, ...);

/*! <pre>
  set signal handler for signal SIGTERM
</pre> */
void rlSetSigtermHandler(void (*handler)(void *arg), void *arg);

/*! <pre>
  context = 0 must be 0 on first call
</pre> */
const char *rlFindFile(const char *pattern, int *context);

/** <pre>
returns:
~/.name               on Linux/Unix
sys$login:name        on OpenVMS
%USERPROFILE%\\name    on Windows
</pre> */
const char *rlGetInifile(const char *name);

/** <pre>
swaps bytes
</pre> */
int rlSwapShort(int val);

/** <pre>
Send command to a bus system
</pre> */
int rlEib1     (int command);
int rlEib2     (int command);
int rlLon1     (int command);
int rlLon2     (int command);
int rlProfibus1(int command);
int rlProfibus2(int command);
int rlCan1     (int command);
int rlCan2     (int command);

#endif
