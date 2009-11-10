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
#include "rlcutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#ifdef RLUNIX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#endif

#ifdef __VMS
#include <descrip.h>
#include <rmsdef.h>
#include <ssdef.h>
#include <iodef.h>
#include <unixio.h>
#include <file.h>
#include <lib$routines.h>
#endif

#ifdef RLWIN32
#include <windows.h>
#include <winreg.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#endif

int rlDebugPrintfState = 0;

int rlSetDebugPrintf(int state)
{
  if(state == 0) rlDebugPrintfState = 0;
  else           rlDebugPrintfState = 1;
  return 0;
}

int rlDebugPrintf(const char *format, ...)
{
  int ret;
  char message[rl_PRINTF_LENGTH]; // should be big enough
  
  if(rlDebugPrintfState == 0) return 0;
  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  return printf("%s",message);
}

int rlLastLinePrintf(const char *format, ...)
{
  int ret,i;
  char message[rl_PRINTF_LENGTH]; // should be big enough
  
  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  message[79] = '\0';
  printf("%c[80D",27); // move 80 columns left 
  printf("%s",message);
  for(i=strlen(message); i<80; i++) printf(" ");
  return 80;
}

#ifdef RLUNIX
//------------------------------------------------------------------------
int rlexec(const char *command)
{
  char *buf;
  char *arg[512];
  int i,iarg,ret;

  buf = new char [strlen(command)+1];
  strcpy(buf,command);
  iarg = 0;
  for(i=0; buf[i] != '\0'; i++)
  {
    if(buf[i] == '\"')
    {
      i++;
      arg[iarg++] = &buf[i];
      while(buf[i] != '\"' && buf[i] != '\0') i++;
      if(buf[i] == '\"') buf[i++] = '\0';
    }
    else
    {
      if(buf[i] != ' ' && (i==0 || buf[i-1] == '\0'))
      {
        arg[iarg++] = &buf[i];
      }
      if(buf[i] == ' ')
      {
        buf[i] = '\0';
      }
    }
  }
  arg[iarg] = NULL;

  ret = execvp(arg[0],arg);
  delete [] buf;
  return ret;
}
#endif

//------------------------------------------------------------------------
const char *rlpass(const char *p)
{
  static char ret[4*16+8];
  char buf[80];
  int  i,val;

  ret[0] = '\0';
  for(i=0; p[i] != '\0' && i<=16; i++)
  {
    val = p[i] * 16;
    sprintf(buf,"%04X",val);
    strcat(ret,buf);
  }

  return ret;
}

//------------------------------------------------------------------------
char *rlstrncpy(char *dest, const char *source, int n)
{
  int i;

  for(i=0; i<n; i++)
  {
    if(source[i] == '\0') break;
    dest[i] = source[i];
  }
  dest[i] = '\0';
  return dest;
}

//------------------------------------------------------------------------
int rlvsnprintf(char *text, int len, const char *format, va_list ap)
{
  int ret;

#ifdef RLWIN32
  if(format == NULL || format[0] == '\0')
  {
    text[0] = '\0';
    return 1;
  }
  ret = _vsnprintf(text, len, format, ap);
#endif
#ifdef __VMS
  static char vms_is_deprecated[rl_PRINTF_LENGTH];
  if(format == NULL || format[0] == '\0')
  {
    text[0] = '\0';
    return 1;
  }
  ret = vsprintf(vms_is_deprecated, format, ap);
  rlstrncpy(text,vms_is_deprecated,len);
#endif
#ifdef RLUNIX
  if(format == NULL || format[0] == '\0')
  {
    text[0] = '\0';
    return 1;
  }
  ret = vsnprintf(text, len, format, ap);
#endif
  return ret;
}

//------------------------------------------------------------------------
int rlsnprintf(char *text, int len, const char *format, ...)
{
  int ret;

  va_list ap;
  va_start(ap,format);
#ifdef RLWIN32
  ret = _vsnprintf(text, len, format, ap);
#endif
#ifdef __VMS
  static char vms_is_deprecated[rl_PRINTF_LENGTH];
  ret = vsprintf(vms_is_deprecated, format, ap);
  rlstrncpy(text,vms_is_deprecated,len);
#endif
#ifdef RLUNIX
  ret = vsnprintf(text, len, format, ap);
#endif
  va_end(ap);
  return ret;
}

//------------------------------------------------------------------------
static void *rlsigtermarg = NULL;
static void (*rlUserSigtermHandler)(void *arg) = NULL;

static void rlSigtermHandler(int sig)
{
  printf("entering rlSigtermHandler sig=%d\n",sig);
  if(sig == SIGTERM && rlUserSigtermHandler != NULL) rlUserSigtermHandler(rlsigtermarg);
#ifdef RLUNIX
  fflush(stdout);
  fsync(fileno(stdout));
#endif
  exit(0);
}

void rlSetSigtermHandler(void (*handler)(void *arg), void *arg)
{
  rlsigtermarg = arg;
  rlUserSigtermHandler = handler;
  signal(SIGTERM,rlSigtermHandler);
}

//------------------------------------------------------------------------
const char *rlFindFile(const char *pattern, int *context)
{
  static char freturn[512];

#ifdef RLUNIX
  static DIR *dirp;
  static struct dirent *dp;

  if(*context == 0) dirp = opendir(".");
  *context = 1;

  while((dp = readdir(dirp)) != NULL)
  {
    if(dp->d_name[0] == '.') ;
    else if(strstr(dp->d_name,pattern) != NULL)
    {
      strcpy(freturn,dp->d_name);
      return freturn;
    }
  }
  closedir(dirp);
  return NULL;
#endif

#ifdef __VMS
  int i,ret;
  static char file[512] = "";
  static char wildcard[80];
  struct dsc$descriptor_s dwildcard;
  struct dsc$descriptor_s dfreturn;

  strcpy(wildcard,pattern);

  dwildcard.dsc$w_length  = strlen(wildcard);
  dwildcard.dsc$a_pointer = wildcard;
  dwildcard.dsc$b_dtype   = DSC$K_DTYPE_T;
  dwildcard.dsc$b_class   = DSC$K_CLASS_S;

  dfreturn.dsc$w_length  = sizeof(freturn);
  dfreturn.dsc$a_pointer = &freturn[0];
  dfreturn.dsc$b_dtype   = DSC$K_DTYPE_T;
  dfreturn.dsc$b_class   = DSC$K_CLASS_S;

  ret = LIB$FIND_FILE(&dwildcard,&dfreturn,context,0,0,0,0);
  if     (ret == RMS$_NMF)    return NULL; // no more files found
  else if(ret != RMS$_NORMAL) return NULL;
  else if(strcmp(freturn,file) == 0) { file[0] = '\0'; return NULL; }
  i=0;
  while(freturn[i] > ' ')i++;
  freturn[i] = '\0';
  return freturn;
#endif

#ifdef RLWIN32
  static WIN32_FIND_DATA wfd;
  static HANDLE hFindFile;

  if(*context == 0) // find first
  {
    *context = 1;
    hFindFile = FindFirstFile(pattern,&wfd);
    if(hFindFile == INVALID_HANDLE_VALUE)    return NULL;
    else                                     strcpy(freturn,(const char *) &wfd.cFileName);
  }
  else // subsequent find
  {
    if(FindNextFile(hFindFile,&wfd) == TRUE) strcpy(freturn,(const char *) &wfd.cFileName);
    else                                   { FindClose(hFindFile); return NULL; }
  }
  return freturn;
#endif
}

//------------------------------------------------------------------------
const char *rlGetInifile(const char *name)
{
  static char buf[1024];

#ifdef RLUNIX
  strcpy(buf,getenv("HOME"));
  strcat(buf,"/.");
#endif
#ifdef __VMS
  strcpy(buf,"sys$login:");
#endif
#ifdef RLWIN32
  ExpandEnvironmentStrings("%USERPROFILE%",buf,sizeof(buf)-1);
  if(strcmp(buf,"%USERPROFILE%") == 0) strcpy(buf,"C:");
  strcat(buf,"\\");;
#endif

  strcat(buf,name);
  return buf;
}

int rlSwapShort(int val)
{
  return (val & 0x0ff)*256 + val/256;
}

//------------------------------------------------------------------------
int rlEib1(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -eib1 %d",command);
  return system(buf);
}

int rlEib2(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -eib2 %d",command);
  return system(buf);
}

int rlLon1(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -lon1 %d",command);
  return system(buf);
}

int rlLon2(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -lon2 %d",command);
  return system(buf);
}

int rlProfibus1(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -profibus1 %d",command);
  return system(buf);
}

int rlProfibus2(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -profibus2 %d",command);
  return system(buf);
}

int rlCan1(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -can1 %d",command);
  return system(buf);
}

int rlCan2(int command)
{
  char buf[80];

  sprintf(buf,"buscommand -can2 %d",command);
  return system(buf);
}

#ifdef RLWIN32
static int get_iexplore(char *buf)
{
  HKEY applications,iexplore,shell,open,command;
  long ret;
  unsigned long size,type;
  char *cptr;

  buf[0] = '\0';

  ret = RegOpenKey(
    HKEY_CLASSES_ROOT,        // handle to open key
    "Applications",           // address of name of subkey to open
    &applications             // address of handle to open key
  );
  if(ret != ERROR_SUCCESS)
  {
    return -1;
  }

  ret = RegOpenKey(
    applications,             // handle to open key
    "iexplore.exe",           // address of name of subkey to open
    &iexplore                 // address of handle to open key
  );
  if(ret != ERROR_SUCCESS)
  {
    RegCloseKey(applications);
    return -1;
  }

  ret = RegOpenKey(
    iexplore,                 // handle to open key
    "shell",                  // address of name of subkey to open
    &shell                    // address of handle to open key
  );
  if(ret != ERROR_SUCCESS)
  {
    RegCloseKey(applications);
    RegCloseKey(iexplore);
    return -1;
  }

  ret = RegOpenKey(
    shell,                    // handle to open key
    "open",                   // address of name of subkey to open
    &open                     // address of handle to open key
  );
  if(ret != ERROR_SUCCESS)
  {
    RegCloseKey(applications);
    RegCloseKey(iexplore);
    RegCloseKey(shell);
    return -1;
  }

  ret = RegOpenKey(
    open,                     // handle to open key
    "command",                // address of name of subkey to open
    &command                  // address of handle to open key
  );
  if(ret != ERROR_SUCCESS)
  {
    RegCloseKey(applications);
    RegCloseKey(iexplore);
    RegCloseKey(shell);
    RegCloseKey(open);
    return -1;
  }

  ret = RegQueryValueEx(
    command,               // handle to key to query
    "",                    // address of name of value to query
    NULL,                  // reserved
    &type,                 // address of buffer for value type
    (unsigned char *) buf, // address of data buffer
    &size                  // address of data buffer size
  );

  RegCloseKey(applications);
  RegCloseKey(iexplore);
  RegCloseKey(shell);
  RegCloseKey(open);
  RegCloseKey(command);

  if(ret != ERROR_SUCCESS) return -1;

  cptr = strstr(buf," %1");
  if(cptr != NULL) *cptr = '\0';

  return 0;
}

static int mysystem(const char *command)
{
  int ret;
  STARTUPINFO         si = { sizeof(si)};
  PROCESS_INFORMATION pi;
  char cmd[4096];

  strcpy(cmd,command);
  ret = (int) CreateProcess( NULL, cmd
                           , NULL, NULL
                           , FALSE, CREATE_NO_WINDOW
                           , NULL, NULL
                           , &si, &pi);
  return ret;
}
#endif

int rlsystem(const char *command)
{
#ifdef RLWIN32
   return mysystem(command);
#else
  return system(command);
#endif
}

int rlBrowser(const char *htmlfile)
{
  char buf[4096];
  int ret = 0;

#ifdef RLUNIX
  sprintf(buf,"konqueror %s &", htmlfile);
  ret = system(buf);
#endif
#ifdef RLWIN32
  if(get_iexplore(buf) < 0) return -1;
  strcat(buf," ");
  strcat(buf,htmlfile);
  ret = mysystem(buf);
#endif
  return ret;
}//------------------------------------------------------------------------

int rlOption(const char *string, const char *option)
{
  const char *cptr;

  cptr = strstr(string,option);
  if(cptr == NULL) return 0;
  return 1;
}

int rlIntOption(const char *string, const char *option, int def)
{
  const char *cptr;
  int ret;

  cptr = strstr(string,option);
  if(cptr == NULL) return def;

  cptr = strstr(cptr,"=");
  if(cptr == NULL) return def;

  sscanf(cptr,"=%d",&ret);
  return ret;
}

float rlFloatOption(const char *string, const char *option, float def)
{
  const char *cptr;
  float ret;

  cptr = strstr(string,option);
  if(cptr == NULL) return def;

  cptr = strstr(cptr,"=");
  if(cptr == NULL) return def;

  sscanf(cptr,"=%f",&ret);
  return ret;
}

const char *rlTextOption(const char *string, const char *option, const char *def)
{
  int i = 0;
  const char *cptr;
  char quote;
  static char ret[rl_PRINTF_LENGTH];

  cptr = strstr(string,option);
  if(cptr == NULL) return def;

  cptr = strstr(cptr,"=");
  if(cptr == NULL) return def;

  cptr++;
  quote = *cptr;
  cptr++;
  while(i < ((int) sizeof(ret)-1))
  {
    if(cptr[i] == quote || cptr[i] == '\0') break;
    ret[i] = cptr[i];
    i++;
  }
  ret[i] = '\0';
  return ret;
}

int rlCopyTextfile(const char *source, const char *destination)
{
  FILE *fin, *fout;

  if(source == NULL || destination == NULL) return -1;
  fin = fopen(source,"r");
  if(fin == NULL)
  {
    printf("rlCopyTextfile: could not read %s\n",source);
    return -1;
  }
  fout = fopen(destination,"w");
  if(fout == NULL)
  {
    fclose(fin);
    printf("rlCopyTextfile: could not write %s\n",destination);
    return -1;
  }

  char *line = new char[256*256];
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    fprintf(fout,"%s",line);
  }
  delete [] line;

  fclose(fin);
  fclose(fout);
  return 0;
}

int rlupper(char *str)
{
  if(str == NULL) return -1;
  while(*str != '\0')
  {
    *str = toupper(*str);
    str++;
  }
  return 0;
}

int rllower(char *str)
{
  if(str == NULL) return -1;
  while(*str != '\0')
  {
    *str = tolower(*str);
    str++;
  }
  return 0;
}

int rlStartsWith(const char *str, const char *startstr)
{
  int ret;
  if(str == NULL || startstr == NULL) return 0;
  ret = strncmp(str,startstr,strlen(startstr));
  if(ret == 0) return 1;
  return 0;
}
