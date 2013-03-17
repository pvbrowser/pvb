/***************************************************************************
                          rlsharedmemory.cpp  -  description
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
#include "rlsharedmemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RLWIN32
#include <unistd.h>
#endif

#ifdef RLUNIX
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#ifdef __VMS
#include <starlet.h>
#include <lib$routines.h>
#include <descrip.h>
#include <ssdef.h>
#include <syidef.h>
#include <secdef.h>
#include <rms.h>
#include <errno.h>
#include <psldef.h>
#include <dvidef.h>
typedef struct
{
  long start;
  long end;
}
ADD;
#endif

#ifdef RLWIN32
#include <windows.h>
#endif

static void myinit(pthread_mutex_t *mutex)
{
  int *cnt = (int *) mutex;
  *cnt = 0;
}

static void mylock(pthread_mutex_t *mutex, int increment)
{
  volatile int *cnt = (int *) mutex;
  while(1)
  {
retry:
    if(*cnt == 0)
    { // try to lock the counter
      (*cnt) += increment;
      if(*cnt > 1)
      {
        (*cnt) -= increment;
        goto retry; // another process also wanted to lock the counter
      }
      return;       // now we can do it
    }
    rlwthread_sleep(1);
  }
}

static void myunlock(pthread_mutex_t *mutex)
{
  int *cnt = (int *) mutex;
  if(*cnt > 0) (*cnt)--;
}

rlSharedMemory::rlSharedMemory(const char *shmname, unsigned long Size, int rwmode)
{
#ifdef RLUNIX
  struct shmid_ds buf;
  FILE *fp;
  int file_existed;

  status  = OK;
  name = new char[strlen(shmname)+1];
  strcpy(name,shmname);
  size    = Size + sizeof(*mutex);

  // create file
  file_existed = 1;
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    file_existed = 0;
    fp = fopen(name,"w");
    if(fp == NULL)     
    {
      int ret; 
      char buf[1024];
      sprintf(buf,"could not write shm=%s\n",shmname);
      ret = ::write(1,buf,strlen(buf));
      if(ret < 0) exit(-1);
      sprintf(buf,"you have to run this program as root !!!\n");
      ret = ::write(1,buf,strlen(buf));
      if(ret < 0) exit(-1);
      status=ERROR_FILE;
      exit(-1);
    }
  }
  fclose(fp);

  // old stuff, without suggestions from Stefan Lievens 
  //shmkey  = ftok(name,0);
  //
  //id  = shmget(shmkey, size, IPC_CREAT);

  //shmkey  = ftok(name, 'R');
  shmkey  = ftok(name, 'b');

  //id  = shmget(shmkey, size, 0600 | IPC_CREAT);
  id  = shmget(shmkey, size, rwmode | IPC_CREAT);
  if(id < 0)           { status=ERROR_SHMGET; return; }

  base_adr = (char *) shmat(id,NULL,0);
  if(base_adr == NULL) { status=ERROR_SHMAT;  return; }

  if(shmctl(id, IPC_STAT, &buf) != 0) { status=ERROR_SHMCTL; return; };

  mutex     = (pthread_mutex_t *) base_adr;
  user_adr  = base_adr + sizeof(*mutex);
  //if(file_existed == 0) rlwthread_mutex_init(mutex,NULL);
  if(file_existed == 0) myinit(mutex);
#endif

#ifdef __VMS
  int file_existed = 0;
  long ret,fd,page_size,pagelets,pagelet_size,file_block_size,flags,item,ident[2];
  FILE *fp;
  ADD  add_in,add_ret;
  struct dsc$descriptor_s section_name;
  struct FAB fab;

  status  = OK;
  name = new char[strlen(shmname)+1];
  strcpy(name,shmname);
  size    = Size + sizeof(*mutex);
  // The file block size is fixed
  file_block_size = 512; // Bytes
  // Get the page size
  item = SYI$_PAGE_SIZE;
  ret = lib$getsyi( &item       ,
                    &page_size  ,
                    0           ,
                    0           ,
                    0           ,
                    0
                   );
  if(ret != SS$_NORMAL) { status=ERROR_FILE; return; }
  // Fill descriptor for section name
  section_name.dsc$w_length  = strlen(name);
  section_name.dsc$a_pointer = name;
  section_name.dsc$b_dtype   = DSC$K_DTYPE_T;
  section_name.dsc$b_class   = DSC$K_CLASS_S;
  // The pagelet size is fixed
  pagelet_size = 512; // Bytes
  // Get memory
  if(size % page_size == 0) pagelets = size / pagelet_size;
  else                      pagelets = (size / page_size + 1) * (page_size / pagelet_size);
  ret = sys$expreg(pagelets,&add_ret,0,0);
  if(ret != SS$_NORMAL) { status=ERROR_FILE; return; }
  // Set the addresses
  base_adr = (char *) add_ret.start;
  user_adr = base_adr + sizeof(*mutex);
  mutex    = (pthread_mutex_t *) base_adr;
  if(base_adr == NULL) { status=ERROR_SHMAT;  return; }
  // Fill the fab
  fab = cc$rms_fab;                       // Initialize fab
  fab.fab$b_fac = fab.fab$b_fac | FAB$M_PUT | FAB$M_DEL | FAB$M_GET | FAB$M_UPD;
  fab.fab$l_fna = name;
  fab.fab$b_fns = strlen(name);
  fab.fab$l_fop = fab.fab$l_fop
                      | FAB$M_CIF            // create file if not existent
                      | FAB$M_CTG            // contiguous
                      | FAB$M_UFO;           // user open
  fab.fab$b_shr =   fab.fab$b_shr            // shareble access
                      | FAB$M_SHRPUT
                      | FAB$M_UPI;
  fab.fab$l_alq = pagelets * pagelet_size / file_block_size;
  // Open the section file
  ret = sys$create (&fab);
  if(ret != RMS$_NORMAL && ret != RMS$_CREATED)
  {
    sys$close (&fab);
    status=ERROR_FILE;
    return;
  }
  // Set the channel
  fd = fab.fab$l_stv;
  // Fill the input address
  add_in.start = add_ret.start;
  add_in.end   = add_ret.end;
  // Clear ident
  ident[0] = 0;
  ident[1] = 0;
  // Set flags
  flags = 0;
  flags = SEC$M_GBL | SEC$M_WRT | SEC$M_PERM;
  // Create and map the section
  ret = sys$crmpsc(&add_in ,&add_ret,
                     (long)0       ,  // acmode
                     flags         ,  // flags
                     &section_name ,  // gsdnam
                     &ident        ,  // ident
                     (long)0       ,  // relpag
                     (short)fd     ,  // chan
                     pagelets      ,  // pagcnt
                     (long)0       ,  // vbn
                     (long)0       ,  // prot
                     (long)0          // pfc
                   );
  if(ret != SS$_NORMAL && ret != SS$_CREATED)
  {
    sys$close(&fab);
    status=ERROR_FILE;
    return;
  }
  // Test the section addresses
  if(add_in.start != add_ret.start || add_in.end != add_ret.end)
  {
    sys$close(&fab);
    status=ERROR_FILE;
    return;
  }
  // Close the section file
  ret = sys$close(&fab);
  // rlwthread_mutex_init(mutex,NULL);
  if(file_existed == 0) myinit(mutex);
#endif

#ifdef RLWIN32
  HANDLE hFile, hShmem;
  int file_existed;

  status  = OK;
  name = new char[strlen(shmname)+1];
  strcpy(name,shmname);
  size    = Size + sizeof(*mutex);

  file_existed = 1;
  hFile = CreateFile(name,
                     GENERIC_READ | GENERIC_WRITE,
                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                     NULL,
                     OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL
                     );
  if(hFile ==  INVALID_HANDLE_VALUE)
  {
    file_existed = 0;
    hFile = CreateFile(name,
                       GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL,
                       CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL
                       );
  }
  if(hFile == INVALID_HANDLE_VALUE) { status=ERROR_FILE; return; }
  hShmem = CreateFileMapping(
    hFile,
    NULL,                // no security attributes
    PAGE_READWRITE,      // read/write access
    0,                   // size: high 32-bits
    size,                // size: low 32-bits
    NULL);               // name of map object
  if(hShmem == NULL) { status=ERROR_FILE; return; }
  base_adr = (char *) MapViewOfFile(
    hShmem,              // object to map view of
    FILE_MAP_WRITE,      // read/write access
    0,                   // high offset:  map from
    0,                   // low offset:   beginning
    0);                  // default: map entire file
  if(base_adr == NULL) { status=ERROR_FILE; return; }
  id     = (int) hShmem;
  shmkey = (int) hFile;
  mutex     = (pthread_mutex_t *) base_adr;
  user_adr  = base_adr + sizeof(*mutex);
  if(file_existed == 0) rlwthread_mutex_init(mutex,NULL);
  else
  {
    WaitForSingleObject(mutex, 3000); // timeout in milliseconds != INFINITE
    ReleaseMutex(mutex);              // avoid deadlock
  }
#endif
  if(rwmode == 0) return; // no warning of unused parameter
}

rlSharedMemory::~rlSharedMemory()
{
  delete [] name;
#ifdef RLWIN32
  if(status != OK) return;
  CloseHandle((HANDLE) id);
  CloseHandle((HANDLE) shmkey);
#endif
}

int rlSharedMemory::deleteSharedMemory()
{
#ifdef RLUNIX
  struct shmid_ds buf;
  if(status != OK) return -1;
  rlwthread_mutex_destroy(mutex);
  shmctl(id, IPC_RMID, &buf);
  size = 0;
  return 0;
#endif

#ifdef __VMS
  int ret;
  ADD add_in,add_ret;
  struct dsc$descriptor_s section_name;

  if(status != OK) return -1;
  rlwthread_mutex_destroy(mutex);
  // Fill descriptor for section name
  section_name.dsc$w_length  = strlen(name);
  section_name.dsc$a_pointer = name;
  section_name.dsc$b_dtype   = DSC$K_DTYPE_T;
  section_name.dsc$b_class   = DSC$K_CLASS_S;
  // Delete the section
  ret = sys$dgblsc(0,&section_name,0);
  if(ret != SS$_NORMAL) return -1;
  // Fill the input address
  add_in.start = (long) base_adr;
  add_in.end   = (long) base_adr + size;
  // Free the memory
  ret = sys$deltva(&add_in,&add_ret,0);
  if(ret != SS$_NORMAL) return -1;
  // Test the section addresses
  if(add_in.start != add_ret.start || add_in.end != add_ret.end) return -1;
  return 0;
#endif

#ifdef RLWIN32
  if(status != OK) return -1;
  rlwthread_mutex_destroy(mutex);
  UnmapViewOfFile(base_adr);
  CloseHandle((HANDLE) id);
  CloseHandle((HANDLE) shmkey);
  status = ~OK;
  return 0;
#endif
}

int rlSharedMemory::write(unsigned long offset, const void *buf, int len)
{
  void *ptr;
  if(status != OK)      return -1;
  if(len <= 0)          return -1;
  if(offset+len > size) return -1;
  ptr = user_adr + offset;
#ifdef RLWIN32
  rlwthread_mutex_lock(mutex);   // Linux and OpenVMS don't support PTHREAD_PROCESS_SHARED (windows does not support pthread at all)
#else
  mylock(mutex,1);
#endif
  memcpy(ptr,buf,len);
#ifdef RLWIN32
  rlwthread_mutex_unlock(mutex); // Linux and OpenVMS don't support PTHREAD_PROCESS_SHARED (windows does not support pthread at all)
#else
  myunlock(mutex);
#endif
  return len;
}

int rlSharedMemory::read(unsigned long offset, void *buf, int len)
{
  void *ptr;
  if(status != OK)      return -1;
  if(len <= 0)          return -1;
  if(offset+len > size) return -1;
  ptr = user_adr + offset;
#ifdef RLWIN32
  rlwthread_mutex_lock(mutex);   // Linux and OpenVMS don't support PTHREAD_PROCESS_SHARED (windows does not support pthread at all)
#else
  mylock(mutex,1);
#endif
  memcpy(buf,ptr,len);
#ifdef RLWIN32
  rlwthread_mutex_unlock(mutex); // Linux and OpenVMS don't support PTHREAD_PROCESS_SHARED (windows does not support pthread at all)
#else
  myunlock(mutex);
#endif
  return len;
}

int rlSharedMemory::readInt(unsigned long offset, int index)
{
  int val;
  if(index < 0) return -1;
  read(offset+index*sizeof(val),&val,sizeof(val));
  return val;
}

int rlSharedMemory::readShort(unsigned long offset, int index)
{
  short int val;
  if(index < 0) return -1;
  read(offset+index*sizeof(val),&val,sizeof(val));
  return val;
}

int rlSharedMemory::readByte(unsigned long offset, int index)
{
  char val;
  if(index < 0) return -1;
  read(offset+index*sizeof(val),&val,sizeof(val));
  return val;
}

float rlSharedMemory::readFloat(unsigned long offset, int index)
{
  float val;
  if(index < 0) return -1;
  read(offset+index*sizeof(val),&val,sizeof(val));
  return val;
}

int rlSharedMemory::writeInt(unsigned long offset, int index, int val)
{
  int ret;
  if(index < 0) return -1;
  ret = write(offset+index*sizeof(val),&val,sizeof(val));
  return ret;
}

int rlSharedMemory::writeShort(unsigned long offset, int index, int val)
{
  int ret;
  short int val2;

  if(index < 0) return -1;
  val2 = (short int) val;
  ret = write(offset+index*sizeof(val2),&val2,sizeof(val2));
  return ret;
}

int rlSharedMemory::writeByte(unsigned long offset, int index, unsigned char val)
{
  int ret;
  if(index < 0) return -1;
  ret = write(offset+index*sizeof(val),&val,sizeof(val));
  return ret;
}

int rlSharedMemory::writeFloat(unsigned long offset, int index, float val)
{
  int ret;
  if(index < 0) return -1;
  ret = write(offset+index*sizeof(val),&val,sizeof(val));
  return ret;
}

void *rlSharedMemory::getUserAdr()
{
  return (void *) user_adr;
}

int rlSharedMemory::shmKey()
{
  return shmkey;
}

int rlSharedMemory::shmId()
{
  return id;
}

