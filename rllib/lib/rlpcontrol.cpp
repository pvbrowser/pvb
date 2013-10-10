/***************************************************************************
                          rlpcontrol.cpp  -  description
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
#include "rlpcontrol.h"
#include "rlcutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef RLWIN32
#include <windows.h>
#include <winuser.h>
//#define USE_WINKILL
#endif
#ifdef RLUNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#endif
#ifdef __VMS
#include <unistd.h>
#include <descrip.h>
#include <starlet.h>
#include <ssdef.h>
#include <lib$routines.h>
#include <jpidef.h>
#endif

#ifdef RLWIN32
static UINT rlSIGTERM = 0;
#endif

rlPcontrol::rlPcontrol()
{
  m_pid = -1;
  startup_command = process_name = NULL;
  next = NULL;
#ifdef __VMS
  m_input = m_output = m_error = NULL;
  prio = 4; // normal user priority
#endif
#ifdef RLWIN32
  m_dwProcessId = -1;
  prio = 1; // normal user priority
  if(rlSIGTERM == 0) rlSIGTERM = RegisterWindowMessage("rlSIGTERM");
#endif
#ifdef RLUNIX
  prio = 0;
#endif
}

rlPcontrol::~rlPcontrol()
{
  if(startup_command != NULL) delete [] startup_command;
  if(next != NULL) delete next;
#ifdef RLWIN32
  if(m_pid != -1) CloseHandle((HANDLE) m_pid);
#endif  
#ifdef __VMS
  if(m_input  != NULL) delete [] m_input;
  if(m_output != NULL) delete [] m_output;
  if(m_error  != NULL) delete [] m_error;
#endif
}

const char *rlPcontrol::startupCommand()
{
  return startup_command;
}

const char *rlPcontrol::processName()
{
  return process_name;
}

rlTime *rlPcontrol::processTime()
{
  return &process_time;
}

void rlPcontrol::setPID(long _pid)
{
  if(_pid <= 0) 
  {
    m_pid = -1;
    return;
  }
#ifdef RLWIN32
  if(m_pid != -1) CloseHandle((HANDLE) m_pid);
  m_dwProcessId = _pid;
  m_pid = (long) OpenProcess(PROCESS_ALL_ACCESS,TRUE,m_dwProcessId);
#else
  m_pid = _pid;
#endif
}

long rlPcontrol::pid()
{
#ifdef RLWIN32
  if(m_dwProcessId == -1) return 0;
  if(m_dwProcessId ==  0) return 0;
  return m_dwProcessId;
#else
  if(m_pid == -1) return 0;
  if(m_pid ==  0) return 0;
  return m_pid;
#endif
}

void rlPcontrol::setStartupCommand(const char *command, const char *processname)
{
  if(startup_command != NULL) delete [] startup_command;
  startup_command = new char[strlen(command)+1];
  strcpy(startup_command,command);

  if(process_name != NULL) delete [] process_name;
  process_name = new char[strlen(processname)+1];
  strcpy(process_name,processname);
}

int rlPcontrol::rlstrlen(const char *str)
{
  if(str == NULL) return -1;
  else            return strlen(str);
}

int rlPcontrol::start()
{
  if(startup_command == NULL) return -1;
  if(isAlive())               return -1;
  process_time.getLocalTime();

#ifdef RLUNIX
  if((m_pid = ::fork()) == 0)
  {
    rlexec(startup_command);
    ::exit(0);
  }
  // printf("start pid=%ld\n",m_pid);
  return 0;
#endif

#ifdef __VMS
  int ret;
  struct dsc$descriptor_s image,prcnam,input,output,error,*inputptr,*outputptr,*errorptr;

  image.dsc$w_length   = rlstrlen(startup_command);
  image.dsc$b_dtype    = DSC$K_DTYPE_T;
  image.dsc$b_class    = DSC$K_CLASS_S;
  image.dsc$a_pointer  = startup_command;

  prcnam.dsc$w_length  = rlstrlen(process_name);
  prcnam.dsc$b_dtype   = DSC$K_DTYPE_T;
  prcnam.dsc$b_class   = DSC$K_CLASS_S;
  prcnam.dsc$a_pointer = process_name;

  input.dsc$w_length   = rlstrlen(m_input);
  input.dsc$b_dtype    = DSC$K_DTYPE_T;
  input.dsc$b_class    = DSC$K_CLASS_S;
  input.dsc$a_pointer  = m_input;

  output.dsc$w_length  = rlstrlen(m_output);
  output.dsc$b_dtype   = DSC$K_DTYPE_T;
  output.dsc$b_class   = DSC$K_CLASS_S;
  output.dsc$a_pointer = m_output;

  error.dsc$w_length   = rlstrlen(m_error);
  error.dsc$b_dtype    = DSC$K_DTYPE_T;
  error.dsc$b_class    = DSC$K_CLASS_S;
  error.dsc$a_pointer  = m_error;

  inputptr = outputptr = errorptr = 0;
  if( input.dsc$w_length > 0) inputptr  = &input;
  if(output.dsc$w_length > 0) outputptr = &output;
  if( error.dsc$w_length > 0) errorptr  = &error;

  if( inputptr != 0 &&  inputptr->dsc$a_pointer == NULL) inputptr  = 0;
  if(outputptr != 0 && outputptr->dsc$a_pointer == NULL) outputptr = 0;
  if( errorptr != 0 &&  errorptr->dsc$a_pointer == NULL) errorptr  = 0;

  /*
    printf("sys$creprc(\n");
    printf("  image=%s\n",image.dsc$a_pointer);
    printf("  inputptr=%s\n" ,inputptr->dsc$a_pointer);
    printf("  outputptr=%s\n",outputptr->dsc$a_pointer);
    printf("  errorptr=%s\n" ,errorptr->dsc$a_pointer);
    printf("  prcnam=%s\n",prcnam.dsc$a_pointer);
    printf("  priority=%d\n",prio);
    printf(")\n");
  */
  ret = sys$creprc(&m_pid,&image,inputptr,outputptr,errorptr,0,0,&prcnam,prio,0,0,0,0,0);
  if(ret != SS$_NORMAL) return -1;
  return 0;
#endif

#ifdef RLWIN32
  STARTUPINFO        si; //  = { sizeof(si)};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi;
  DWORD dwCreationFlags;

  if(m_pid != -1) CloseHandle((HANDLE) m_pid);
  dwCreationFlags = CREATE_NO_WINDOW;
  if     (prio == 0) dwCreationFlags |= IDLE_PRIORITY_CLASS;
  else if(prio == 1) dwCreationFlags |= NORMAL_PRIORITY_CLASS;
  else if(prio == 2) dwCreationFlags |= HIGH_PRIORITY_CLASS;
  else if(prio == 3) dwCreationFlags |= REALTIME_PRIORITY_CLASS;
  int ret = (int) CreateProcess( NULL, startup_command
                               , NULL, NULL
                               , FALSE, dwCreationFlags
                               , NULL, NULL
                               , &si, &pi);
  m_pid = (int) pi.hProcess;
  m_dwProcessId = pi.dwProcessId;
  CloseHandle(pi.hThread);
  return ret;
#endif
}

int rlPcontrol::sigterm()
{
  if(m_pid == -1) return -1;
  if(m_pid ==  0) return -1;

#ifdef RLUNIX
  kill(m_pid,SIGTERM);
  //printf("kill pid=%ld\n",m_pid);
#endif

#ifdef __VMS
  kill(m_pid,SIGTERM);
#endif

#ifdef RLWIN32
#ifdef USE_WINKILL
  // does anybody know HOWTO send SIGTERM under windows ?
  // for the moment i just kill the process
  TerminateProcess((HANDLE) m_pid, 0);
  WaitForSingleObject((HANDLE) m_pid, 60000);
  CloseHandle((HANDLE) m_pid);
#else
  // broadcast rlSIGTERM
  // Other prozesses haveto:
  // UINT RegisterWindowMessage("rlSIGTERM");
  // and evaluate their pid from the message
  if(rlSIGTERM != 0)
  {
    PostMessage(HWND_BROADCAST,rlSIGTERM,-1,m_dwProcessId);
    /*
    BroadcastSystemMessage(
      BSF_IGNORECURRENTTASK, // do not send message to this process
      BSM_ALLCOMPONENTS,     // BSM_APPLICATIONS, // broadcast only to applications
      rlSIGTERM,             // PM_MYMSG, // registered private message
      -1,                    // message-specific value
      m_dwProcessId);        // message-specific value
    */
  }
#endif
#endif

  m_pid = -1;
  return 0;
}

int rlPcontrol::sigkill()
{
  if(m_pid == -1) return -1;
  if(m_pid ==  0) return -1;

#ifdef RLUNIX
  kill(m_pid,SIGKILL);
  //printf("kill pid=%ld\n",m_pid);
#endif

#ifdef __VMS
  kill(m_pid,SIGKILL);
#endif

#ifdef RLWIN32
  TerminateProcess((HANDLE) m_pid, 0);
  WaitForSingleObject((HANDLE) m_pid, 60000);
  CloseHandle((HANDLE) m_pid);
#endif

  m_pid = -1;
  return 0;
}

int rlPcontrol::isAlive()
{
#ifdef __VMS
  long  ret,code,mypid;

  if(m_pid == -1) return 0;
  mypid = m_pid;
  code  = JPI$_STATE;
  ret = lib$getjpi(&code,&mypid,0,0,0,0);
  if(ret != SS$_NORMAL)
  {
    //printf("lib$getjpi terminated abnormal\n");
    return 0;
  }
  if(mypid == m_pid) return 1;
  return 0;
#endif

#ifdef RLUNIX
  int ret,status;

  if(m_pid == -1) return 0;
  ret = waitpid(m_pid, &status, WNOHANG);
  //printf("isAlive pid=%ld\n",m_pid);
  if(ret == 0) return 1;
  return 0;
#endif

#ifdef RLWIN32
  long status;

  if(m_pid == -1) return 0;
  if(GetExitCodeProcess((HANDLE) m_pid, (unsigned long *) &status) != 0) // success
  {
    if(status == STILL_ACTIVE) return 1;
    else                       return 0;
  }
  return 0; // failure
#endif
}

rlPcontrol *rlPcontrol::getNext()
{
  return next;
}

rlPcontrol *rlPcontrol::addNew()
{
  rlPcontrol *item;

  item = this;
  while(item->next != NULL) item = item->next;
  item->next = new rlPcontrol();
  return item->next;
}

#ifdef __VMS
void rlPcontrol::setInput(const char *input)
{
  if(m_input != NULL) delete [] m_input;
  m_input = new char [strlen(input)+1];
  strcpy(m_input,input);
}

void rlPcontrol::setOutput(const char *output)
{
  if(m_output != NULL) delete [] m_output;
  m_output = new char [strlen(output)+1];
  strcpy(m_output,output);
}

void rlPcontrol::setError(const char *error)
{
  if(m_error != NULL) delete [] m_error;
  m_error = new char [strlen(error)+1];
  strcpy(m_error,error);
}
#endif

void rlPcontrol::setPriority(int pri)
{
#ifdef __VMS
  if(pri <  0) pri = 0;
  if(pri > 15) pri = 15;
#endif
#ifdef RLWIN32
  if(pri < 0) pri = 0;
  if(pri > 3) pri = 3;
#endif
  prio = pri;
}

int rlPcontrol::priority()
{
  return prio;
}

