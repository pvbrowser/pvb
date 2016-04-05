/***************************************************************************
                          rlspawn.cpp  -  description
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
#include "rldefine.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifndef RLWIN32
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#else
#include <windows.h>
#include <psapi.h>
#endif
#include "rlspawn.h"
#include "rlcutil.h"
#ifdef RLUNIX
#include <sys/wait.h>
#include <signal.h>
#endif
#include "rlstring.h"

#ifdef RLWIN32_NOT_USABLE
WaitNamedPipe function
Minimum supported client Windows 2000 Professional [desktop apps only]
http://msdn.microsoft.com/en-us/library/windows/desktop/aa365800%28v=vs.85%29.aspx
static int GetFileNameFromHandle(HANDLE hFile, char *pszFilename, const int max_path) 
{
  int bSuccess = 0;
  //TCHAR pszFilename[MAX_PATH+1];
  HANDLE hFileMap;

  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi); 

  if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
  {
     printf(TEXT("Cannot map a file with a length of zero.\n"));
     return FALSE;
  }

  // Create a file mapping object.
  hFileMap = CreateFileMapping(hFile, 
                    NULL, 
                    PAGE_READONLY,
                    0, 
                    1,
                    NULL);

  if (hFileMap) 
  {
    // Create a file mapping to get the file name.
    void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

    if (pMem) 
    {
      if (GetMappedFileName (GetCurrentProcess(), 
                             pMem, 
                             pszFilename,
                             max_path)) 
      {

        // Translate path with device name to drive letters.
        TCHAR szTemp[1024];
        szTemp[0] = '\0';

        if (GetLogicalDriveStrings((int) sizeof(szTemp) -1, szTemp)) 
        {
          TCHAR szName[max_path];
          TCHAR szDrive[3] = TEXT(" :");
          int   bFound = 0;
          TCHAR* p = szTemp;

          do 
          {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, max_path))
            {
              int uNameLen = strlen(szName);
              rlString rlName(szName);
              if (uNameLen < max_path) 
              {
                //bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0
                //         && *(pszFilename + uNameLen) == _T('\\');
                bFound = rlName.strnnocasecmp(pszFilename, uNameLen) == 0
                         && *(pszFilename + uNameLen) == _T('\\');

                if (bFound) 
                {
                  // Reconstruct pszFilename using szTempFile
                  // Replace device path with DOS path
                  TCHAR szTempFile[max_path];
                  StringCchPrintf(szTempFile,
                            max_path,
                            TEXT("%s%s"),
                            szDrive,
                            pszFilename+uNameLen);
                  StringCchCopyN(pszFilename, max_path+1, szTempFile, _tcslen(szTempFile));
                }
              }
            }

            // Go to the next NULL character.
            while (*p++);
          } while (!bFound && *p); // end of string
        }
      }
      bSuccess = 1;
      UnmapViewOfFile(pMem);
    } 

    CloseHandle(hFileMap);
  }
  //_tprintf(TEXT("File name is %s\n"), pszFilename);
  return bSuccess;
}
#endif

rlSpawn::rlSpawn()
{
#ifdef RLWIN32 
  fromChildRD = NULL;
  fromChildRD = NULL;
  toChildRD = NULL;
  toChildWR = NULL;
  hThread = NULL;
  hProcess = NULL;
  pid = 0;
#else  
  toChild = fromChild = NULL;
  pid = 0;
#endif  
}

rlSpawn::~rlSpawn()
{
#ifdef RLWIN32 
  if(fromChildRD != NULL) CloseHandle(fromChildRD);
  if(fromChildWR != NULL) CloseHandle(fromChildWR);
  if(toChildRD   != NULL) CloseHandle(toChildRD);
  if(toChildWR   != NULL) CloseHandle(toChildWR);
  if(hThread     != NULL) CloseHandle(hThread);
  if(hProcess    != NULL) CloseHandle(hProcess);
#else  
  if(toChild   != NULL) ::fclose((FILE*) toChild);
  if(fromChild != NULL) ::fclose((FILE*) fromChild);
#endif
}

int rlSpawn::spawn(const char *command)
{
#ifdef RLWIN32 
  if(fromChildRD != NULL) CloseHandle(fromChildRD);
  if(fromChildWR != NULL) CloseHandle(fromChildWR);
  if(toChildRD   != NULL) CloseHandle(toChildRD);
  if(toChildWR   != NULL) CloseHandle(toChildWR);
  if(hThread     != NULL) CloseHandle(hThread);
  if(hProcess    != NULL) CloseHandle(hProcess);
  fromChildRD = NULL;
  fromChildWR = NULL;
  toChildRD   = NULL;
  toChildWR   = NULL;
  hThread     = NULL;
  hProcess    = 0;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  char cmd[strlen(command)+1];
  strcpy(cmd,command);

  if(!CreatePipe(&fromChildRD, &fromChildWR, &saAttr, 0))
  {
    printf("CreatePipe() - pipe for child process STDOUT failed\n");
    return -1;
  }
  if(!SetHandleInformation(fromChildRD, HANDLE_FLAG_INHERIT, 0)) // Ensure the read handle to the pipe for STDOUT is not inherited
  {
    printf("SetHandleInformation() - pipe STDOUT read handle failed for inheritance\n");
    CloseHandle(fromChildRD);
    fromChildRD = NULL;
    CloseHandle(fromChildWR);
    fromChildWR = NULL;
    return -2;
  }
  if(!CreatePipe(&toChildRD, &toChildWR, &saAttr, 0))
  {
    printf("CreatePipe() - pipe for child process STDIN failed\n");
    CloseHandle(fromChildRD);
    fromChildRD = NULL;
    CloseHandle(fromChildWR);
    fromChildWR = NULL;
    return -3;
  }
  if(!SetHandleInformation(toChildWR, HANDLE_FLAG_INHERIT, 0)) // Ensure the write handle to the pipe for STDIN is not inherited
  {
    printf("SetHandleInformation() - pipe STDIN write handle failed for inheritance\n");
    CloseHandle(fromChildRD);
    fromChildRD = NULL;
    CloseHandle(fromChildWR);
    fromChildWR = NULL;
    CloseHandle(toChildRD);
    toChildRD = NULL;
    CloseHandle(toChildWR);
    toChildWR = NULL;
    return -4;
  }  

  STARTUPINFO si; //  = { sizeof(si)};
  memset(&si,0,sizeof(si));
  si.cb = sizeof(si);
  si.hStdError  = fromChildWR;
  si.hStdOutput = fromChildWR;
  si.hStdInput  = toChildRD;
  si.dwFlags   |= STARTF_USESTDHANDLES;

  PROCESS_INFORMATION pi;
  memset(&pi,0,sizeof(pi));

  DWORD dwCreationFlags = 0;

  int ret = (int) CreateProcess( NULL, cmd
                                , NULL, NULL
                                , TRUE, dwCreationFlags
                                , NULL, NULL
                                , &si, &pi);
  if(ret)
  { // success
    hProcess = pi.hProcess;
    hThread  = pi.hThread;
    return 1;
  }
  // fail
  CloseHandle(fromChildRD);
  fromChildRD = NULL;
  CloseHandle(fromChildWR);
  fromChildWR = NULL;
  CloseHandle(toChildRD);
  toChildRD = NULL;
  CloseHandle(toChildWR);
  toChildWR = NULL;
  return -1;
#else  
  int to_child[2],from_child[2],ret;

  if(toChild   != NULL) ::fclose((FILE*) toChild);
  if(fromChild != NULL) ::fclose((FILE*) fromChild);
  toChild = fromChild = NULL;

  ret = ::pipe(to_child);
  if(ret == -1) return -1;
  ret = ::pipe(from_child);
  if(ret == -1) return -1;

  if((pid = ::fork()) == 0)
  {
    if(to_child[0] != 0) // stdin
    {
      ::dup2(to_child[0],0);
      ::close(to_child[0]);
    }
    if(from_child[1] != 2) // stderr
    {
      ::dup2(from_child[1] ,2);
    }
    if(from_child[1] != 1) // stdout
    {
      ::dup2(from_child[1],1);
      ::close(from_child[1]);
    }
    ::close(to_child[1]);
    ::close(from_child[0]);
    ::rlexec(command);
    ::exit(0);
  }

  ::close(to_child[0]);
  ::close(from_child[1]);
  toChild   = (void*) ::fdopen(to_child[1],"w");
  if(toChild == NULL)   {                            return -1; }
  fromChild = (void*) ::fdopen(from_child[0],"r");
  if(fromChild == NULL) { ::fclose((FILE*) toChild); return -1; }
  return pid;
#endif
}

const char *rlSpawn::readLine()
{
#ifdef RLWIN32
  char *cptr;
  if(fromChildRD == NULL) return NULL;
  int i,c;
  for(i=0; i < (int) sizeof(line) - 1; i++)
  {
    if((c = getchar()) < 0)
    {
      if(i==0) return NULL;
      line[i] = '\0';
      return line;
    }  
    line[i] = (char) c;
    if(c == '\n')
    {
      cptr = strchr(line,0x0d);
      if(cptr != NULL)
      {
        cptr[0] = '\n';
        cptr[1] = '\0';
        return line;
      }
      line[i+1] = '\0';
      return line;
    }
  }
  line[i] = '\0';
  return line;
#else
  if(fromChild == NULL) return NULL;
  if(::fgets(line,sizeof(line)-1,(FILE*) fromChild) == NULL)
  {
#ifdef RLUNIX
    if(pid != 0)
    {
      int status;
      waitpid(pid, &status, 0);
      kill(pid,SIGHUP);
    }
#endif
    return NULL;
  }
  return line;
#endif  
}

int rlSpawn::getchar()
{
#ifdef RLWIN32
  if(fromChildRD == NULL) return EOF;
  DWORD readed = 0;
  unsigned char buf[4];
  int ret = (int) ReadFile(fromChildRD, buf, 1, &readed, NULL);
  if(ret)
  { // success
    if(readed == 1) return buf[0];
  }
  if(fromChildRD != NULL) CloseHandle(fromChildRD);
  if(fromChildWR != NULL) CloseHandle(fromChildWR);
  if(toChildRD   != NULL) CloseHandle(toChildRD);
  if(toChildWR   != NULL) CloseHandle(toChildWR);
  if(hThread     != NULL) CloseHandle(hThread);
  if(hProcess    != NULL) CloseHandle(hProcess);
  fromChildRD = NULL;
  fromChildWR = NULL;
  toChildRD   = NULL;
  toChildWR   = NULL;
  hThread     = NULL;
  hProcess    = NULL;
  return EOF;
#else
  if(fromChild == NULL) return EOF;
  return ::fgetc((FILE*) fromChild);
#endif  
}

int rlSpawn::write(const char *buf, int len)
{
#ifdef RLWIN32
  if(toChildWR == NULL) return -1;
  DWORD written = 0;
  int ret = (int) WriteFile(toChildWR, buf, len, &written, NULL);
  if(ret)
  { // success
    return written;
  }
  return -1;
#else  
  if(toChild == NULL) return -1;
  return ::write(fileno((FILE*)toChild),buf,len);
#endif
}

int rlSpawn::printf(const char *format, ...)
{
  int ret;
  char message[rl_PRINTF_LENGTH]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  return write(message,strlen(message));
}

int rlSpawn::writeString(const char *buf)
{
  if(buf[0] == EOF && buf[1] == '\0')
  {
#ifdef RLWIN32 
    if(fromChildRD != NULL) CloseHandle(fromChildRD);
    if(fromChildWR != NULL) CloseHandle(fromChildWR);
    if(toChildRD   != NULL) CloseHandle(toChildRD);
    if(toChildWR   != NULL) CloseHandle(toChildWR);
    if(hThread     != NULL) CloseHandle(hThread);
    if(hProcess    != NULL) CloseHandle(hProcess);
    fromChildRD = NULL;
    fromChildWR = NULL;
    toChildRD   = NULL;
    toChildWR   = NULL;
    hThread     = NULL;
    hProcess    = NULL;
#else  
    if(toChild   != NULL) ::fclose((FILE*) toChild);
    if(fromChild != NULL) ::fclose((FILE*) fromChild);
    toChild   = NULL;
    fromChild = NULL;
#endif
    return 0;
  } 
#ifdef RLWIN32
  if(toChildWR == NULL) return -1;
  int len = strlen(buf);
  return write(buf,len+1);
#else
  if(toChild == NULL) return -1;
  return fprintf((FILE*)toChild,"%s",buf);
#endif  
}

void rlSpawn::printAll()
{
  const char *cptr;
  while((cptr=readLine()) != NULL) ::printf("%s",cptr);
}

int rlSpawn::select(int timeout)
{
#ifdef RLWIN32
  // windows does not support select on ReadFile
  // ReadFile will return if there are no more bytes available
  //if( _kbhit() ) return 1;
  if(1) return 1;
  if(timeout > 0)         return 0;
  if(fromChildRD == NULL) return 0;
  return 0;
#else
  struct timeval timout;
  fd_set wset,rset,eset;
  int    ret,maxfdp1,s;

  if(fromChild == NULL) return -1;
  s = fileno((FILE *) fromChild);
  /* setup sockets to read */
  maxfdp1 = s+1;
  FD_ZERO(&rset);
  FD_SET (s,&rset);
  FD_ZERO(&wset);
  FD_ZERO(&eset);
  timout.tv_sec  =  timeout / 1000;
  timout.tv_usec = (timeout % 1000) * 1000;

  ret = ::select(maxfdp1,&rset,&wset,&eset,&timout);
  if(ret == 0) return 0; /* timeout */
  return 1;
#endif  
}

FILE *rlSpawn::getFilepointer()
{
#ifdef RLWIN32
  return NULL;
#else
  return (FILE *) fromChild;
#endif  
}
  
int rlSpawn::sigkill()
{
#ifdef RLUNIX
  if(pid == -1) return -1;
  if(pid ==  0) return -1;
  kill(pid,SIGKILL);
  //printf("kill pid=%ld\n",m_pid);
#endif

#ifdef __VMS
  if(pid == -1) return -1;
  if(pid ==  0) return -1;
  kill(pid,SIGKILL);
#endif

#ifdef RLWIN32
  if(hProcess ==  0) return -1;
  TerminateProcess((HANDLE) hProcess, 0);
  WaitForSingleObject((HANDLE) hProcess, 60000);
  CloseHandle((HANDLE) hProcess);
  CloseHandle((HANDLE) hThread);
  hThread = 0;
  hProcess = 0;
#endif

  pid = 0;
  return 0;
}

int rlSpawn::readJpegBuffer(unsigned char *buffer, int maxbuffer)
{
  int c1, c2;

  // search for startOfImage ff d8
  while(1)
  {
    if((c1 = getchar()) < 0) return -1;
    if(c1 == 0x0ff)
    {
      if((c2 = getchar()) < 0) return -2;
      if(c2 == 0x0d8)
      {
        break;
      }
    }
  }

  int ind = 0;
  buffer[ind++] = (unsigned char) c1;
  buffer[ind++] = (unsigned char) c2;
  while(1) // read until endOfImage ff d9
  {
    if(ind >= maxbuffer) return -3;
    if((c1 = getchar()) < 0) return -4;
    buffer[ind++] = (unsigned char) c1;
    if(c1 == 0x0ff)
    {
      if((c2 = getchar()) < 0) return -5;
      buffer[ind++] = (unsigned char) c2;
      if(c2 == 0x0d9)
      {
        return ind;;
      }
    }
  }  
}

