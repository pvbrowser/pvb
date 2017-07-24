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
// mksiemens name.mksiemens  (C) R. Lehrig 2004
/*
    S7_200 update        : Wed Mar 21 2007
    copyright            : (C) 2007 by Aljosa Merljak
    Email                : aljosa.merljak@datapan.si
*/

#include "../pvbrowser/pvdefine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char line[256*256];
static char target[4096];
static char shared_memory[4096];
static char mailbox[4096];
static int  idletime;
static int  shared_memory_size;
static char eventhost[4096];
static int  eventport;

static void init(const char *name)
{
  int base,dbnum,start,len,cyclecnt;
  char *cptr;
  FILE *fp,*fout;
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    exit(-1);
  }

  strcpy(target,"siemensdaemon");
  idletime = 100;
  eventhost[0] = '\0';
  eventport = -1;
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("target=",line,7) == 0)
    {
      sscanf(line,"target=%s",target);
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
    else if(strncmp("slave=",line,6) == 0)
    {    
      printf("%s",line);
    }  
  }

  fclose(fp);

  printf("target=%s\n",target);
  printf("shared_memory=%s\n",shared_memory);
  printf("mailbox=%s\n",mailbox);
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
  fprintf(fout,"// Siemens Signal assignment of %s\n",target);
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
  char *cptr,adr[256],plc_type[256],org[256];
  int  slave,dbnum,start,len,fetch_write,function,rack_slot;

  sprintf(line,"%s.cpp",target);
  fout = fopen(line,"w");
  if(fout == NULL)
  {
    printf("could not write %s\n",line);
    exit(-1);
  }

  fprintf(fout,"%s","//\n");
  fprintf(fout,"%s","// SiemensTCP daemon template                                 (C) R. Lehrig 2004\n");
  fprintf(fout,"%s","//\n");
  fprintf(fout,"%s","//\n");
  fprintf(fout,"%s","// Attention: this program must be run as super user\n");
  fprintf(fout,"%s","//\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","#include <stdio.h>\n");
  fprintf(fout,"%s","#include <stdlib.h>\n");
  fprintf(fout,"%s","#include \"rlsiemenstcp.h\"\n");
  fprintf(fout,"%s","#include \"rlsharedmemory.h\"\n");
  fprintf(fout,"%s","#include \"rlmailbox.h\"\n");
  fprintf(fout,"%s","#include \"rlthread.h\"\n");
  fprintf(fout,"%s","#include \"rlcutil.h\"\n");
  if(eventport != -1)
  {
    fprintf(fout,"%s","#include \"rlevent.h\"\n");
  }
  fprintf(fout,"%s","\n");
  fprintf(fout,"#define %s_CYCLE_TIME %d\n",target,idletime);
  fprintf(fout,"%s","\n");

  // write slaves
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    fclose(fout);
    exit(-1);
  }
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("slave=",line,6) == 0)
    {
      slave = -1;
      fetch_write = 1;
      function = rack_slot = -1;
      adr[0] = plc_type[0] = '\0';
      sscanf(line,"slave=%d",&slave);
      cptr = strstr(line,"adr=");
      if(cptr!=NULL) strncpy(adr,&cptr[4],100);
      cptr = strstr(adr," ");
      if(cptr!=NULL) *cptr = '\0';
      cptr = strstr(line,"type=");
      if(cptr!=NULL) strncpy(plc_type,&cptr[5],100);
      cptr = strstr(plc_type," ");
      if(cptr!=NULL) *cptr = '\0';
      cptr = strstr(plc_type,"\n");
      if(cptr!=NULL) *cptr = '\0';
      cptr = strstr(line,"fetch_write=");
      if(cptr!=NULL) sscanf(cptr,"fetch_write=%d",&fetch_write);
      cptr = strstr(line,"function=");
      if(cptr!=NULL) sscanf(cptr,"function=%d",&function);
      cptr = strstr(line,"rack_slot=");
      if(cptr!=NULL) sscanf(cptr,"rack_slot=%d",&rack_slot);
      
      if(adr[0] == '\0' || plc_type[0] == '\0' || slave < 0)
      {
        printf("parameter missing in: %s\n",line);
        fclose(fp);
        fclose(fout);
        exit(-1);
      }
      fprintf(fout,"rlSiemensTCP   slave%d(\"%s\",rlSiemensTCP::%s,%d,%d,%d);\n",slave,adr,plc_type,fetch_write,function,rack_slot);
    }
  }
  fclose(fp);    
  
  fprintf(fout,"rlSharedMemory shm(\"%s\",%d);\n",shared_memory,shared_memory_size);
  fprintf(fout,"rlMailbox      mbx(\"%s\");\n",mailbox);
  fprintf(fout,"%s","rlThread       thread;\n");
  fprintf(fout,"%s","rlSiemensTCP   *slave_array[256];\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","// read mailbox and write to siemensTCP\n");
  fprintf(fout,"%s","void *reader(void *arg)\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","  int buflen,slave,org,dbnr,start_adr,len,function;\n");
  fprintf(fout,"%s","  unsigned char buf[2048+8];\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  mbx.clear(); // clear old messages\n");
  fprintf(fout,"%s","  while((buflen = mbx.read(buf,sizeof(buf))) > 0)\n");
  fprintf(fout,"%s","  {\n");
  fprintf(fout,"%s","    slave     = buf[0];\n");
  fprintf(fout,"%s","    org       = buf[1];\n");
  fprintf(fout,"%s","    dbnr      = buf[2]*256 + buf[3];\n");
  fprintf(fout,"%s","    start_adr = buf[4]*256 + buf[5];\n");
  fprintf(fout,"%s","    len       = buf[6]*256 + buf[7];\n");
  fprintf(fout,"%s","    function  = buf[8];\n");
  fprintf(fout,"%s","    if(slave >= 0 && slave < 256 && slave_array[slave] != NULL)\n");
  fprintf(fout,"%s","    {\n");
  fprintf(fout,"%s","      thread.lock();\n");
  fprintf(fout,"%s","      slave_array[slave]->write(org,dbnr,start_adr,len,&buf[9],function);\n");
  fprintf(fout,"%s","      thread.unlock();\n");
  fprintf(fout,"%s","    }\n");  
  if(eventport != -1)
  {
    fprintf(fout,"%s","    else { rlEvent(rlError,\"wrong param send to mailbox\"); }\n");
  }  
  fprintf(fout,"%s","  }\n");
  fprintf(fout,"%s","  return arg;\n");
  fprintf(fout,"%s","}\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","// read cycle on SiemensTCP\n");
  fprintf(fout,"%s","int siemensTCPCycle(int slave, int offset, int org, int dbnr, int start_adr, int len)\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","  unsigned char data[4096];\n");
  fprintf(fout,"%s","  int ret;\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  if(slave_array[slave] == NULL) return len;\n");
  fprintf(fout,"%s","  thread.lock();\n");
  fprintf(fout,"%s","  ret = slave_array[slave]->fetch(org,dbnr,start_adr,len,data);\n");
  fprintf(fout,"%s","  thread.unlock();\n");
  if(eventport != -1)
  {
    fprintf(fout,"%s","  if(ret < 0) { rlEvent(rlWarning,\"fetch ret=%d slave=%d\",ret,slave); rlsleep(5000); }\n");
  }  
  fprintf(fout,"%s","  if(ret > 0) shm.write(offset,data,ret);\n");
  fprintf(fout,"%s","  else        printf(\"E\"); // Error\n");
  fprintf(fout,"%s","  return len;\n");
  fprintf(fout,"%s","}\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","int main()\n");
  fprintf(fout,"%s","{\n");
  fprintf(fout,"%s","  int offset,ret,i;\n");
  if(eventport != -1)
  {
    fprintf(fout,"  char *argv[] = {\"\",\"-eventhost=%s\",\"-eventport=%d\"};\n",eventhost,eventport);
    fprintf(fout,"\n");
    fprintf(fout,"  rlEventInit(3,argv,\"%s:\");\n",target);
    fprintf(fout,"  rlEvent(rlInfo,\"%s starting\");\n",target); 
  }    
  
  // write slaves array
  fprintf(fout,"%s","  for(i=0; i<256; i++) slave_array[i] = NULL;\n");
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    fclose(fout);
    exit(-1);
  }
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("slave=",line,6) == 0)
    {
      slave = -1;
      adr[0] = plc_type[0] = '\0';
      sscanf(line,"slave=%d",&slave);      
      if(slave < 0)
      {
        printf("parameter missing in: %s\n",line);
        fclose(fp);
        fclose(fout);
        exit(-1);
      }
      fprintf(fout,"  slave_array[%d] = &slave%d;\n",slave,slave);
    }
  }
  fclose(fp);    
  
  fprintf(fout,"%s","\n");  
  fprintf(fout,"%s","  //rlSetDebugPrintf(1); // set debugging on\n");
  fprintf(fout,"  thread.create(reader,NULL);\n");
  fprintf(fout,"  while(1)\n");
  fprintf(fout,"  {\n");
  fprintf(fout,"    rlsleep(%s_CYCLE_TIME);\n",target);
  fprintf(fout,"    offset = 0;\n");
  fprintf(fout,"    // SiemensTCPCycle(int slave, int offset, int org, int dbnr, int start_adr, int len);\n");
  
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
      org[0] = '\0';
      slave = dbnum = start = len = -1;
      cptr = strstr(line,"slave=");
      if(cptr!=NULL) sscanf(cptr,"slave=%d",&slave);
      cptr = strstr(line,"dbnum=");
      if(cptr!=NULL) sscanf(cptr,"dbnum=%d",&dbnum);
      cptr = strstr(line,"start=");
      if(cptr!=NULL) sscanf(cptr,"start=%d",&start);
      cptr = strstr(line,"len=");
      if(cptr!=NULL) sscanf(cptr,"len=%d",&len);
      cptr = strstr(line,"org=");
      if(cptr!=NULL) strncpy(org,&cptr[4],100);
      cptr = strstr(org," ");
      if(cptr!=NULL) *cptr = '\0';
      
      if(slave==-1 || org[0]=='\0' || dbnum==-1 || start==-1 || len==-1)
      {
        printf("parameter missing in: %s\n",line);
        fclose(fp);
        fclose(fout);
        exit(-1);
      }
      fprintf(fout,"    ret = siemensTCPCycle(%d,offset, rlSiemensTCP::%s, %d, %d, %d);\n",slave,org,dbnum,start,len);
      fprintf(fout,"    if(ret>0) offset += ret; else continue;\n");
    }
  }
  fclose(fp);
  
  fprintf(fout,"  }\n");
  fprintf(fout,"\n");
  fprintf(fout,"  return 0;\n");
  fprintf(fout,"}\n");
  
  fclose(fout);
}

int gsiemens(const char *name)
{
  strcpy(target,"siemensdaemon");
  init(name);
  generate(name);
  return 0;
}
