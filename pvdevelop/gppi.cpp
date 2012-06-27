/***************************************************************************
 *   Copyright (C) 2005 by R. Lehrig                                       *
 *   lehrig@t-online.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//
// mkppi name.mkppi  (C) R. Lehrig 2004
//

#include "../pvbrowser/pvdefine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char line[256*256];
static char target[4096];
static char shared_memory[4096];
static char mailbox[4096];
static char tty[4096];
static char dave_speed[4096];
static int  baudrate;
static int  idletime;
static int  base;
static char area[4096];
static int  shared_memory_size;
static char eventhost[4096];
static int  eventport;

static void init(const char *name)
{
  int dbnum,start,len,cyclecnt;
  char *cptr;
  FILE *fp,*fout;
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    exit(-1);
  }

  strcpy(target,"ppidaemon");
  strcpy(tty,"/dev/ttyS0");
  baudrate = 9600;
  strcpy(dave_speed,"daveSpeed187k");
  idletime = 0;
  eventhost[0] = '\0';
  eventport = -1;
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("target=",line,7) == 0)
    {
      sscanf(line,"target=%s",target);
    }
    else if(strncmp("tty=",line,4) == 0)
    {
      sscanf(line,"tty=%s",tty);
    }
    else if(strncmp("baudrate=",line,9) == 0)
    {
      sscanf(line,"baudrate=%d",&baudrate);
    }
    else if(strncmp("dave_speed=",line,11) == 0)
    {
      sscanf(line,"dave_speed=%s",dave_speed);
    }
    else if(strncmp("idletime=",line,9) == 0)
    {
      sscanf(line,"idletime=%d",&idletime);
    }
    else if(strncmp("mailbox=",line,8) == 0)
    {
      sscanf(line,"mailbox=%s",mailbox);
    }
    else if(strncmp("shared_memory=",line,14) == 0)
    {
      sscanf(line,"shared_memory=%s",shared_memory);
    }
    else if(strncmp("eventlog",line,8) == 0)
    {
      cptr = strstr(line,"port=");
      if(cptr != NULL) sscanf(cptr,"port=%d",&eventport);
      cptr = strstr(line,"host=");
      if(cptr != NULL)
      {
        strcpy(eventhost,&cptr[5]);
        cptr = strstr(eventhost," ");
        if(cptr != NULL) *cptr = '\0';
      }
    }    
  }

  fclose(fp);

  printf("target=%s\n",target);
  printf("shared_memory=%s\n",shared_memory);
  printf("mailbox=%s\n",mailbox);
  printf("tty=%s\n",tty);
  printf("baudrate=%d\n",baudrate);
  printf("dave_speed=%s\n",dave_speed);  
  if(eventport != -1) printf("eventlog host=%s port=%d\n",eventhost,eventport);

  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    exit(-1);
  }

  sprintf(line,"%s.h",target);
  fout = fopen(line,"w");
  if(fout == NULL)
  {
    printf("could not write %s\n",line);
    fclose(fp);
    exit(-1);
  }
  fprintf(fout,"// PPI Signal assignment of %s\n",target);
  fprintf(fout,"#define %s_SHARED_MEMORY \"%s\"\n",target,shared_memory);
  fprintf(fout,"#define %s_MAILBOX \"%s\"\n",target,mailbox);

  cyclecnt = 1;
  base = 0;
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("cycle",line,5) == 0)
    {
      dbnum = start = len = -1;
      cptr = strstr(line,"dbnum=");
      if(cptr!=NULL) sscanf(cptr,"dbnum=%d",&dbnum);
      cptr = strstr(line,"start=");
      if(cptr!=NULL) sscanf(cptr,"start=%d",&start);
      cptr = strstr(line,"len=");
      if(cptr!=NULL) sscanf(cptr,"len=%d",&len);
      if(dbnum==-1 || start==-1 || len==-1)
      {
        printf("parameter missing in: %s\n",line);
        fclose(fp);
        exit(-1);
      }
      fprintf(fout,"\n#define %s_CYCLE%d_BASE %d\n",target,cyclecnt,base);
      fprintf(fout,"#define %s_CYCLE%d_NUM_BYTES %d\n",target,cyclecnt,len);
      base += len;
      cyclecnt++;
    }
  }
  fprintf(fout,"\n#define %s_SHARED_MEMORY_SIZE %d\n",target,base);
  shared_memory_size = base;
  
  fclose(fp);
  fclose(fout);
}

static void generate(const char *name)
{
  FILE *fout,*fp;
  char *cptr;
  int  slave,dbnum,start,len;

  sprintf(line,"%s.cpp",target);
  printf("generate:%s.cpp\n",target);
  fout = fopen(line,"w");
  if(fout == NULL)
  {
    printf("could not write %s\n",line);
    exit(-1);
  }

  fprintf(fout,"%s","//\n");
  fprintf(fout,"%s","// PPI daemon template                                 (C) R. Lehrig 2004\n");
  fprintf(fout,"%s","//\n");
  fprintf(fout,"%s","//\n");
  fprintf(fout,"%s","// Attention: this program must be run as super user\n");
  fprintf(fout,"%s","//            because libnodave only supports unix, you can't use this program on windows\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","#include <stdio.h>\n");
  fprintf(fout,"%s","#include <stdlib.h>\n");
  fprintf(fout,"%s","#include \"rlsharedmemory.h\"\n");
  fprintf(fout,"%s","#include \"rlmailbox.h\"\n");
  fprintf(fout,"%s","#include \"rlthread.h\"\n");
  fprintf(fout,"%s","#include \"rlcutil.h\"\n");
  if(eventport != -1)
  {
    fprintf(fout,"%s","#include \"rlevent.h\"\n");
  }
  fprintf(fout,"%s","#ifdef unix\n");
  fprintf(fout,"%s","#define LINUX  // needed for libnodave\n");
  fprintf(fout,"%s","#else\n");
  fprintf(fout,"%s","#define BCCWIN // needed for libnodave\n");
  fprintf(fout,"%s","#endif\n");
  fprintf(fout,"%s","extern \"C\"\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","#include \"setport.h\"\n");
  fprintf(fout,"%s","#include \"nodave.h\"\n");
  fprintf(fout,"%s","};\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"#define IDLETIME %d\n",idletime);
  fprintf(fout,"#define SERIAL_DEVICE \"%s\"\n",tty);
  fprintf(fout,"#define BAUDRATE \"%d\"\n",baudrate);
  fprintf(fout,"rlSharedMemory shm(\"%s\",%d);\n",shared_memory,shared_memory_size);
  fprintf(fout,"rlMailbox      mbx(\"%s\");\n",mailbox);
  fprintf(fout,"%s","rlThread       thread;\n");
  fprintf(fout,"%s","rlThread       watchdog;\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","static daveConnection   *dc[32+1];\n");
  fprintf(fout,"%s","static daveInterface    *di;\n");
  fprintf(fout,"%s","static _daveOSserialType fds;\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","// watchdog\n");
  fprintf(fout,"%s","static const char *av0 = \"\";\n");
  fprintf(fout,"%s","static int watchcnt1 = 0;\n");
  fprintf(fout,"%s","void *watchdogthread(void *arg)\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","  int cnt1 = -1;\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  while(1)\n");
  fprintf(fout,"%s","  {\n");
  fprintf(fout,"%s","    rlsleep(5000);\n");
  fprintf(fout,"%s","    if(cnt1 == watchcnt1) break;\n");
  fprintf(fout,"%s","    cnt1 = watchcnt1;\n");
  fprintf(fout,"%s","  }\n");
  fprintf(fout,"%s","  rlsleep(100);\n");
  fprintf(fout,"%s","#ifdef unix\n");
  fprintf(fout,"%s","  rlexec(av0);\n");
  fprintf(fout,"%s","#endif\n");
  fprintf(fout,"%s","  exit(0); // pcontrol may start me again if rlexec fails\n");
  fprintf(fout,"%s","  return arg;\n");
  fprintf(fout,"%s","}\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","// read mailbox and write to modbus\n");
  fprintf(fout,"%s","void *reader(void *arg)\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","  int ret,slave,area,dbnum,start,len,buflen;\n");
  fprintf(fout,"%s","  unsigned char buf[1024+1];\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  mbx.clear(); // clear old messages\n");
  fprintf(fout,"%s","  while((buflen = mbx.read(buf,sizeof(buf))) > 0)\n");
  fprintf(fout,"%s","  {\n");
  fprintf(fout,"%s","    slave = buf[0];\n");
  fprintf(fout,"%s","    area  = buf[1];\n");
  fprintf(fout,"%s","    dbnum = buf[2]*256 + buf[3];\n");
  fprintf(fout,"%s","    start = buf[4]*256 + buf[5];\n");
  fprintf(fout,"%s","    len   = buf[6]*256 + buf[7];\n");
  fprintf(fout,"%s","    if(len > 0 && slave >=0 && slave < 32)\n");
  fprintf(fout,"%s","    {\n");
  fprintf(fout,"%s","      thread.lock();\n");
  fprintf(fout,"%s","      //int daveWriteBytes(daveConnection * dc,int area, int DBnum, int start,int len, void * buffer);\n");
  fprintf(fout,"%s","      ret = daveWriteBytes(dc[slave],area,dbnum,start,len,&buf[8]);\n");
  fprintf(fout,"%s","      thread.unlock();\n");
  fprintf(fout,"%s","      if(ret!=0) printf(\"daveWriteBytes ret=%d\\n\",ret);\n");  
  if(eventport != -1)
  {
    fprintf(fout,"%s","      if(ret != 0) { rlEvent(rlWarning,\"daveWriteBytes ret=%d slave=%d\",ret,slave); }\n");
  }  
  fprintf(fout,"%s","    }\n");
  fprintf(fout,"%s","  }\n");
  fprintf(fout,"%s","  return arg;\n");
  fprintf(fout,"%s","}\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","// read cycle on ppi\n");
  fprintf(fout,"%s","int ppiCycle(int offset, daveConnection *dc, int area, int dbnum, int start, int len)\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","  unsigned char data[1024+1];\n");
  fprintf(fout,"%s","  int ret;\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  if(len > 256) return -1;\n");
  fprintf(fout,"%s","  watchcnt1++;\n");
  fprintf(fout,"%s","  if(watchcnt1 > 10000) watchcnt1 = 0;\n");
  fprintf(fout,"%s","  thread.lock();\n");
  fprintf(fout,"%s","  ret = daveReadBytes(dc,area,dbnum,start,len,data);\n");
  fprintf(fout,"%s","  thread.unlock();\n");
  fprintf(fout,"%s","  if(ret == 0) shm.write(offset,data,len);\n");
  fprintf(fout,"%s","  if(ret != 0) printf(\"daveReadBytes ret=%d\\n\",ret);\n");
  if(eventport != -1)
  {
    fprintf(fout,"%s","  if(ret != 0) { rlEvent(rlWarning,\"daveReadBytes ret=%d\",ret); rlsleep(5000); }\n");
  }  
  fprintf(fout,"%s","  return len;\n");
  fprintf(fout,"%s","}\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","int main(int ac, char **av)\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","  int offset,ret,adr;\n");
  if(eventport != -1)
  {
       fprintf(fout,"  char *argv[] = {\"\",\"-eventhost=%s\",\"-eventport=%d\"};\n",eventhost,eventport);
       fprintf(fout,"\n");
       fprintf(fout,"  rlEventInit(3,argv,\"%s:\");\n",target);
  }
  fprintf(fout,"%s","  if(ac > 0) av0 = av[0];\n");
  fprintf(fout,"%s","  fds.rfd=setPort(SERIAL_DEVICE,BAUDRATE,'E');\n");
  fprintf(fout,"%s","  fds.wfd=fds.rfd;\n");
  fprintf(fout,"%s","  if(fds.rfd>0)\n"); 
  fprintf(fout,"%s","  {\n"); 
  fprintf(fout,"    printf(\"ppidaemon starting\\n\");\n");
  if(eventport != -1)
  {
    fprintf(fout,"    rlEvent(rlInfo,\"%s starting\");\n",target); 
  }    
  fprintf(fout,     "    di = daveNewInterface(fds,\"IF1\",0,daveProtoPPI,%s);\n",dave_speed);
  fprintf(fout,"%s","    for(adr=1; adr<32; adr++) dc[adr] = daveNewConnection(di,adr,0,0);\n");
  fprintf(fout,"%s","    thread.create(reader,NULL);\n");
  fprintf(fout,"%s","    watchdog.create(watchdogthread,NULL);\n");
  fprintf(fout,"%s","    while(1)\n");
  fprintf(fout,"%s","    {\n");
  fprintf(fout,"%s","      rlsleep(IDLETIME);\n");
  fprintf(fout,"%s","      offset = 0;\n");
  fprintf(fout,"%s","      //    ppiCycle(offset, dc, area, dbnum, start, len);\n");
  
  // write cycles
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    fclose(fout);
    exit(-1);
  }
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("cycle",line,5) == 0)
    {
      area[0] = '\0';
      slave = dbnum = start = len = -1;
      cptr = strstr(line,"slave=");
      if(cptr!=NULL) sscanf(cptr,"slave=%d",&slave);
      cptr = strstr(line,"dbnum=");
      if(cptr!=NULL) sscanf(cptr,"dbnum=%d",&dbnum);
      cptr = strstr(line,"start=");
      if(cptr!=NULL) sscanf(cptr,"start=%d",&start);
      cptr = strstr(line,"len=");
      if(cptr!=NULL) sscanf(cptr,"len=%d",&len);
      cptr = strstr(line,"area=");
      if(cptr!=NULL) strncpy(area,&cptr[5],100);
      cptr = strstr(area," ");
      if(cptr!=NULL) *cptr = '\0';
      
      if(area[0] == '\0' || slave==-1 || dbnum==-1 || start==-1 || len==-1)
      {
        printf("parameter missing in: %s\n",line);
        fclose(fp);
        fclose(fout);
        exit(-1);
      }
      fprintf(fout,"      ret = ppiCycle(offset, dc[%d], %s, %d, %d, %d);\n",slave,area,dbnum,start,len);
      fprintf(fout,"      if(ret>0) offset += ret; else continue;\n");
    }
  }
  fclose(fp);
  
  fprintf(fout,"%s","    }\n");
  fprintf(fout,"%s","  }\n");
  fprintf(fout,"  printf(\"\\nerror opening SERIAL_DEVICE\\n\");");  
  fprintf(fout,"%s","  return 0;\n");
  fprintf(fout,"%s","}\n");

  fclose(fout);
}

int gppi(const char *name)
{
  strcpy(target,"ppidaemon");
  init(name);
  generate(name);
  return 0;
}
