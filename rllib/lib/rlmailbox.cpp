/***************************************************************************
                          rlmailbox.cpp  -  description
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
#endif
#include "rlmailbox.h"
#include "rlcutil.h"

#ifdef RLUNIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#endif

#ifdef __VMS
#include <lib$routines.h>
#include <libdef.h>
#include <dvidef.h>
#include <ssdef.h>
#include <iodef.h>
#include <descrip.h>
#include <starlet.h>
typedef struct
{
  short    iostat;
  unsigned short msg_len;
  int      reader_pid;
}IOSB;
#endif

#ifdef RLWIN32
#include <windows.h>
#endif

rlMailbox::rlMailbox(const char *mbxname)
{
#ifdef RLUNIX
FILE  *fp;
key_t key;

  status = OK;
  buffer = NULL;
  buffer_size = 0;
  name = new char[strlen(mbxname)+1];
  strcpy(name,mbxname);
  // create file
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    fp = fopen(name,"w");
    if(fp == NULL)
    {
      int ret;
      char buf[1024];
      sprintf(buf,"could not write mailbox=%s\n",mbxname);
      ret = ::write(1,buf,strlen(buf));
      if(ret < 0) exit(-1);
      sprintf(buf,"you have to run this program as root !!!\n");
      ret = ::write(1,buf,strlen(buf));
      if(ret < 0) exit(-1);
      exit(-1);
    }
    fchmod(fileno(fp),0x0fff);
    if(fp == NULL) { status=COULD_NOT_CREATE_MAILBOX; return; }
  }
  fclose(fp);
  // getkey

  // old stuff, without suggestions from Stefan Lievens 
  // key = ftok(name, 'R');

  key = ftok(name, 'b');
  if(key == ((key_t) (-1))) { status=COULD_NOT_GET_KEY; return; }
  // get chanid

  // old stuff, without suggestions from Stefan Lievens 
  // chanid = msgget(key,IPC_CREAT);
  chanid = msgget(key, 0600 | IPC_CREAT);
  if(chanid == -1) { status=COULD_NOT_GET_CHAN_ID; return; }
#endif

#ifdef __VMS
  long   ret;
  short  chan_id;
  struct dsc$descriptor_s dmbxname;

  chanid = -1;
  status = OK;
  name = new char[strlen(mbxname)+1];
  strcpy(name,mbxname);
  dmbxname.dsc$w_length  = strlen(name);
  dmbxname.dsc$a_pointer = name;
  dmbxname.dsc$b_dtype   = DSC$K_DTYPE_T;
  dmbxname.dsc$b_class   = DSC$K_CLASS_S;
  ret = sys$crembx((char)1,                     // 0 temp 1 permanence flag
                   &chan_id,                    // i/o channel
                   MAX_MAILBOX,                 // max msg length
                   MAX_MAILBOX*4,               // max buffer size
                   (long)0,
                   (long)0,
                   &dmbxname);                   // mailbox name
  if(ret == SS$_NORMAL) { chanid = chan_id; return; }
  else                  { status = COULD_NOT_CREATE_MAILBOX; return; }
#endif

#ifdef RLWIN32
  chanid = -1;
  status = OK;
  name = new char[strlen(mbxname)+1];
  strcpy(name,mbxname);
#endif
}

rlMailbox::~rlMailbox()
{
  delete [] name;
  if(buffer != NULL) delete [] buffer;
  if(chanid < 0) return;

#ifdef __VMS
  sys$dassgn((short)chanid);
#endif

#ifdef RLWIN32
  CloseHandle((HANDLE) chanid);
#endif
}

int rlMailbox::write(const void *buf, int len)
{
  status = OK;

#ifdef RLUNIX
  int retlen;
  unsigned char *message = new unsigned char [sizeof(long) + len];
  long *lptr = (long *) message;
  *lptr = 1; // mtype
  memcpy(&message[sizeof(long)],buf,len);
  retlen = msgsnd(chanid,(struct msgbuf *) message, len, 0);
  delete [] message;
  return retlen;
#endif

#ifdef __VMS
  int  ret;
  IOSB iosb;
  ret = sys$qiow (0,
                  (short) chanid,
                  IO$_WRITEVBLK | IO$M_NOW | IO$M_NORSWAIT,
                  &iosb,
                  0,0,
                  buf,
                  len,0,0,0,0);
  len = iosb.msg_len;
  if     (ret == SS$_MBFULL) return MAILBOX_FULL;
  else if(ret != SS$_NORMAL) return MAILBOX_ERROR;
  else                       return len; // Success
#endif

#ifdef RLWIN32
  BOOL bret;
  unsigned long numWritten;

  if(chanid == -1)
  {
    HANDLE h;
    char   mbxname[1024];
    strcpy(mbxname,"\\\\.\\mailslot\\"); strcat(mbxname,name);
    h = CreateFile(
              mbxname,                             // pointer to name of the file
              GENERIC_READ | GENERIC_WRITE,        // access (read-write) mode
              FILE_SHARE_READ | FILE_SHARE_WRITE,  // share mode
              NULL,                                // pointer to security attributes
              OPEN_EXISTING,                       // how to create
              FILE_ATTRIBUTE_NORMAL,               // file attributes
              NULL                                 // handle to file with attributes to copy
                  );
    if(h == INVALID_HANDLE_VALUE)
    {
      status = GetLastError();
      return MAILBOX_ERROR;
    }
    chanid = (int) h;
  }

  bret = WriteFile(
            (HANDLE) chanid,      // handle to file to write to
            buf,                  // pointer to data to write to file
            len,                  // number of bytes to write
            &numWritten,          // pointer to number of bytes written
            NULL                  // pointer to structure for overlapped I/O
                  );
  if(bret==0)
  {
    status = GetLastError();
    CloseHandle((HANDLE) chanid);
    chanid = -1;
    return MAILBOX_ERROR;
  }
  return numWritten;
#endif
}

int rlMailbox::read(void *buf, int maxlen, int wait)
{
  char *cbuf;
  status = OK;
  cbuf = (char *) buf;

#ifdef RLUNIX
  int len;
  unsigned char *message = new unsigned char [sizeof(long) + maxlen];
  if(wait == WAIT ) len = msgrcv(chanid,(struct msgbuf *) message, maxlen,0,0);
  else              len = msgrcv(chanid,(struct msgbuf *) message, maxlen,0,IPC_NOWAIT);
  if(len < maxlen && len >= 0) 
  {
    memcpy(buf,&message[sizeof(long)],len);
    cbuf[len] = '\0';
  }
  else
  {
    cbuf[0] = '\0';
  }
  delete [] message;
  return len;
#endif

#ifdef __VMS
  int  ret,len;
  IOSB iosb;
  if(wait == NOWAIT)
  {
    ret = sys$qiow(0,
                   (short) chanid,
                   IO$_READVBLK | IO$M_NOW,       // I/O CODE
                   &iosb,
                   0,0,
                   buf,
                   maxlen,0,0,0,0);
  }
  else
  {
    ret = sys$qiow(0,
                   (short) chanid,
                   IO$_READVBLK,                  // I/O CODE
                   &iosb,
                   0,0,
                   buf,
                   maxlen,0,0,0,0);
  }
  len = (int) iosb.msg_len;
  if(len < maxlen && len >= 0) cbuf[len] = '\0';
  if     (ret == SS$_NORMAL && iosb.iostat == SS$_NORMAL)   return len;
  else if(iosb.iostat == SS$_NORMAL)                        { status = -1; return MAILBOX_ERROR; }
  else if(ret         == SS$_NORMAL)
  {
    if(wait == NOWAIT && iosb.iostat == SS$_ENDOFFILE)      { status = -2; return MAILBOX_ERROR; }
    else                                                    { status = -3; return MAILBOX_ERROR; }
  }
                                                              status = -4; return MAILBOX_ERROR;
#endif

#ifdef RLWIN32
  HANDLE h;
  char   mbxname[1024];
  unsigned long lenRead;
  BOOL   bret,bret2;

  if(chanid == -1)
  {
    strcpy(mbxname,"\\\\.\\mailslot\\"); strcat(mbxname,name);
    h = CreateMailslot(
                 mbxname,                // pointer to string for mailslot name
                 MAX_MAILBOX,            // maximum message size
                 MAILSLOT_WAIT_FOREVER,  // milliseconds before read time-out
                 NULL);                  // pointer to security structure
    if(h == INVALID_HANDLE_VALUE) { status = GetLastError(); return MAILBOX_ERROR; }
    chanid = (int) h;

    bret2 = SetMailslotInfo((HANDLE) chanid, MAILSLOT_WAIT_FOREVER);
    if(bret2 == 0) { status = GetLastError();  return MAILBOX_ERROR; }
  }

  if(wait == NOWAIT) // begin wait
  {
    lenRead = 0;
    bret2 = SetMailslotInfo((HANDLE) chanid, 0);
    if(bret2 == 0) { status = GetLastError(); return MAILBOX_ERROR; }
    bret = ReadFile(
             (HANDLE) chanid,                        // handle of file to read
             buf,                                    // pointer to buffer
             maxlen,                                 // number of bytes to read
             &lenRead,                               // pointer to number of bytes read
             NULL                                    // pointer to structure for data
                   );
    bret2 = SetMailslotInfo((HANDLE) chanid, MAILSLOT_WAIT_FOREVER);
    if(bret2 == 0) { status = GetLastError(); return MAILBOX_ERROR; }
    if(bret == 0)  { status = GetLastError(); return MAILBOX_ERROR; }
    if((int) lenRead < maxlen && (int) lenRead >= 0) cbuf[lenRead] = '\0';
    return lenRead;
  } // end wait

  lenRead = 0;
  bret = ReadFile(
           (HANDLE) chanid,                        // handle of file to read
           buf,                                    // pointer to buffer
           maxlen,                                 // number of bytes to read
           &lenRead,                               // pointer to number of bytes read
           NULL                                    // pointer to structure for data
                 );
  if(bret == 0) { status = GetLastError(); return MAILBOX_ERROR; }
  if((int) lenRead < maxlen && (int) lenRead >= 0) cbuf[lenRead] = '\0';
  return lenRead;
#endif
}

void rlMailbox::clear()
{
  char *buf = new char[MAX_MAILBOX];
  while(read(buf,MAX_MAILBOX,NOWAIT) > 0);
  delete [] buf;
}

int rlMailbox::printf(const char *format, ...)
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

int rlMailbox::setReadBufferSize(int size)
{
  if(buffer != NULL) delete [] buffer;
  buffer_size = size;
  buffer = new char[size];
  return size;
}

const char *rlMailbox::read(int wait)
{
  if(buffer == NULL)
  {
    buffer_size = 4096;
    buffer = new char[buffer_size];
  }
  int ret = read(buffer,buffer_size,wait);
  if(ret < 0) buffer[0] = '\0';
  return buffer;
}

int rlMailbox::write(const char *message)
{
  return write(message,strlen(message)+1);
}


