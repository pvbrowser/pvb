/***************************************************************************
                          rlcutil.cpp  -  description
                             -------------------
    begin                : Wed Dec 11 2002
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
Some C functions.
</pre> */
#ifndef _RL_CUTIL_H_
#define _RL_CUTIL_H_

#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rldefine.h"

/*! <pre>
  like printf for debugging 
</pre> */
int rlSetDebugPrintf(int state);
int rlDebugPrintf(const char *format, ...);

/*! <pre>
  test if input line is available
  #include <sys/select.h>
</pre> */
int rlInputAvailable();

/*! <pre>
  like printf in the last line of a terminal
</pre> */
int rlLastLinePrintf(const char *format, ...);

#ifdef RLUNIX
/*! <pre>
  call execvp(arg[0],arg) on unix
</pre> */
#ifndef SWIG
int rlexec(const char *command);
#endif
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
  strncpy + terminate with '\\0'
  terminates on '\\n' or '\\0'
  '\\n' is not copied
</pre> */
char *rlstrlinecpy(char *dest, const char *source, int n);

/*! <pre>
  like vsnprintf but portable
</pre> */
#ifndef SWIG
int rlvsnprintf(char *text, int len, const char *format, va_list ap);
#endif

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

/** <pre>
Call internet browser
</pre> */
int rlBrowser(const char *htmlfile);

/** <pre>
Call system(command)
</pre> */
int rlsystem(const char *command);

/** <pre>
Submit a pvserver
Example:
rlSubmitPvserver("HOME","/temp/murx","pvs","-exit_on_bind_error -exit_after_last_client_terminates");
</pre> */
int rlSubmitPvserver(const char *env, const char *path, const char *pvs, const char *options=NULL);

/** <pre>
Get option from string
return = 0 # not found
return = 1 # found
</pre> */
int rlOption(const char *string, const char *option);

/** <pre>
Get option from string
</pre> */
int rlIntOption(const char *string, const char *option, int _default);

/** <pre>
Get option from string
</pre> */
float rlFloatOption(const char *string, const char *option, float _default);

/** <pre>
Get option from string
</pre> */
const char *rlTextOption(const char *string, const char *option, const char *_default);

/** <pre>
Copy a Textfile (no binary file)
</pre> */
int rlCopyTextfile(const char *source, const char *destination);

/** <pre>
convert str to upper case
</pre> */
int rlupper(char *str);

/** <pre>
convert str to lower case
</pre> */
int rllower(char *str);

/** <pre>
test if str starts with startstr
</pre> */
int rlStartsWith(const char *str, const char *startstr);

/** <pre>
test if str ends with endstr
</pre> */
int rlEndsWith(const char *str, const char *endstr);

/** <pre>
test if str matches with wild
</pre> */
int rlStrMatch(const char *str, const char *wild);

/** <pre>
same as stat
</pre> */
#ifndef SWIG
int rlStat(const char *filepath, struct stat *buf);
#endif

/** <pre>
read data from file
return := number of bytes read | -1
</pre> */
int rlFRead(FILE *fin, void *data, int len);

/** <pre>
write data to file
return := number of bytes written | -1
</pre> */
int rlFWrite(FILE *fout, void *data, int len);

/** <pre>
write data to file
return := number of bytes written | -1
</pre> */
int rlWriteFile(const char *filename, void *data, int len);

/** <pre>
same as mkdir
</pre> */
int rlMkdir(const char *dir, int mode=0744);

/** <pre>
Set bit bitnumber in value
Return value
</pre> */
int rlBitSet(int bitnumber, int *value);

/** <pre>
Clear bit bitnumber in value
Return value
</pre> */
int rlBitClear(int bitnumber, int *value);

/** <pre>
XOR bit bitnumber in value
Return value
</pre> */
int rlBitChange(int bitnumber, int *value);

/** <pre>
Test bit bitnumber in value
Return 0 | 1
</pre> */
int rlBitTest(int bitnumber, int *value);

/** <pre>
Push value to front of buffer.
Shift all other values.
</pre> */
void rlPushToDoubleBuffer(double val, double *buffer, int size);

/** <pre>
Push value to front of buffer.
Shift all other values.
</pre> */
void rlPushToFloatBuffer(float val, float *buffer, int size);


#endif
