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
// mkmodbus name.mkmodbus  (C) R. Lehrig 2003-2004
//

#include "../pvbrowser/pvdefine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
QFileInfo f_info;
static char line[256*256];
static char *target;
static char shared_memory[4096];
static char mailbox[4096];
#define SERIAL 1
#define SOCKET 2
static int  communication;
static char tty[4096];
static int  baudrate;
static int  rtscts;
static int  parity;
static int  protocol;
static char tcpadr[4096];
static int  tcpport;
static int  base;
static char eventhost[4096];
static int  eventport;

static void init(const char *name)
{
  int i,slave,function,start_adr,num_register,cyclecnt;
  char *cptr;
  FILE *fp,*fout;
  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    exit(-1);
  }

  eventhost[0] = '\0';
  eventport = -1;
  rtscts = 1;
  parity = 0;
  protocol = 0; // RTU
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("target=",line,7) == 0)
    {
      sscanf(line,"target=%s",target);
    }
    else if(strncmp("tcpport=",line,8) == 0)
    {
      sscanf(line,"tcpport=%d",&tcpport);
    }
    else if(strncmp("tcpadr=",line,7) == 0)
    {
      sscanf(line,"tcpadr=%s",tcpadr);
    }
    else if(strncmp("tty=",line,4) == 0)
    {
      sscanf(line,"tty=%s",tty);
    }
    else if(strncmp("baudrate=",line,9) == 0)
    {
      sscanf(line,"baudrate=%d",&baudrate);
    }
    else if(strncmp("rtscts=",line,7) == 0)
    {
      sscanf(line,"rtscts=%d",&rtscts);
    }
    else if(strncmp("parity=",line,7) == 0)
    {
      sscanf(line,"parity=%d",&parity);
    }
    else if(strncmp("protocol=",line,9) == 0)
    {
      sscanf(line,"protocol=%d",&protocol);
    }
    else if(strncmp("communication=",line,14) == 0)
    {
      communication = SERIAL;
      if(strstr(line,"socket") != NULL) communication = SOCKET;
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
  if     (communication == SERIAL) printf("communication=serial\n");
  else if(communication == SOCKET) printf("communication=socket\n");
  else                           { printf("undefined communication -> exit\n"); exit(-1); }
  printf("tty=%s\n",tty);
  printf("baudrate=%d\n",baudrate);
  printf("rtscts=%d\n",rtscts);
  printf("parity=%d\n",parity);
  printf("protocol=%d\n",protocol);
  printf("tcpadr=%s\n",tcpadr);
  printf("tcpport=%d\n",tcpport);
  if(eventport != -1) printf("eventlog host=%s port=%d\n",eventhost,eventport);

  fp = fopen(name,"r");
  if(fp == NULL)
  {
    printf("could not open %s\n",name);
    exit(-1);
  }

  sprintf(line,"%s.h",f_info.filePath().remove(".mkmodbus").toUtf8().data());
  fout = fopen(line,"w");
  if(fout == NULL)
  {
    printf("could not write %s\n",line);
    fclose(fp);
    exit(-1);
  }
  fprintf(fout,"// Modbus Signal assignment of %s\n",target);
  fprintf(fout,"#define %s_SHARED_MEMORY \"%s\"\n",target,shared_memory);
  fprintf(fout,"#define %s_MAILBOX \"%s\"\n",target,mailbox);

  cyclecnt = 1;
  base = 0;
  while(fgets(line,sizeof(line)-1,fp) != NULL)
  {
    if(strncmp("cycle",line,5) == 0)
    {
      slave = function = start_adr = num_register = -1;
      cptr = strstr(line,"slave=");
      if(cptr!=NULL) sscanf(cptr,"slave=%d",&slave);
      cptr = strstr(line,"function=");
      if(cptr!=NULL) sscanf(cptr,"function=%d",&function);
      cptr = strstr(line,"start_adr=");
      if(cptr!=NULL) sscanf(cptr,"start_adr=%d",&start_adr);
      cptr = strstr(line,"num_register=");
      if(cptr!=NULL) sscanf(cptr,"num_register=%d",&num_register);
      if(slave==-1 || function==-1 || start_adr==-1 || num_register==-1)
      {
        printf("parameter missing in: %s\n",line);
        fclose(fp);
        exit(-1);
      }
      fprintf(fout,"\n#define %s_CYCLE%d_BASE %d\n",target,cyclecnt,base);
      fprintf(fout,"#define %s_CYCLE%d_NUM %d\n",target,cyclecnt,num_register);
      i = num_register;
      if(function==1 || function==2)
      {
        i = num_register / 8;
        if((num_register % 8) > 0) i++;
      }
      else
      {
        i = num_register*2;
      }
      fprintf(fout,"#define %s_CYCLE%d_NUM_BYTES %d\n",target,cyclecnt,i);
      base += i;
      cyclecnt++;
    }
  }
  fprintf(fout,"\n");
  fprintf(fout,"#define %s_LIFE_COUNTER_BASE %d\n",target,base); base += 2;
  fprintf(fout,"#define %s_READ_ERROR_COUNT_BASE %d\n",target,base); base += 2;
  fprintf(fout,"#define %s_WRITE_ERROR_COUNT_BASE %d\n",target,base); base += 2;
  fprintf(fout,"\n#define %s_SHARED_MEMORY_SIZE %d\n",target,base);

  fclose(fp);
  fclose(fout);
}

static void generate(const char *name)
{
  FILE *fout,*fp;
  char *cptr;
  int  slave,function,start_adr,num_register;

  sprintf(line,"%s.cpp",f_info.filePath().remove(".mkmodbus").toUtf8().data());
  fout = fopen(line,"w");
  if(fout == NULL)
  {
    printf("could not write %s\n",line);
    exit(-1);
  }

  fprintf(fout,"//\n");
  fprintf(fout,"// Modbus daemon template                                 (C) R. Lehrig 2003\n");
  fprintf(fout,"//\n");
  fprintf(fout,"//\n");
  fprintf(fout,"// Attention: this program must be run as super user\n");
  fprintf(fout,"//\n");
  fprintf(fout,"\n");
  fprintf(fout,"#include <stdio.h>\n");
  fprintf(fout,"#include <stdlib.h>\n");
  fprintf(fout,"#include \"rlmodbus.h\"\n");
  fprintf(fout,"#include \"rlsharedmemory.h\"\n");
  fprintf(fout,"#include \"rlmailbox.h\"\n");
  fprintf(fout,"#include \"rlthread.h\"\n");
  fprintf(fout,"#include \"rlcutil.h\"\n");
  if(eventport != -1)
  {
    fprintf(fout,"%s","#include \"rlevent.h\"\n");
  }
  fprintf(fout,"#include \"%s.h\"\n",target);
  fprintf(fout,"\n");
  if(baudrate <= 0) baudrate = 9600;
  fprintf(fout,"#define MODBUS_IDLETIME (4*1000)/%d\n",baudrate/100);
  fprintf(fout,"#define SERIAL_DEVICE   \"%s\"\n",tty);
  if(protocol == 1 && communication == SERIAL) 
  {
    fprintf(fout,"rlModbus       modbus(1024+1,rlModbus::MODBUS_ASCII);\n");
  }  
  else
  {
    fprintf(fout,"rlModbus       modbus(256+1,rlModbus::MODBUS_RTU);\n");
  }  
  if(communication == SERIAL)
  {
    fprintf(fout,"rlSerial       serial;\n");
  }
  else
  {
    fprintf(fout,"rlSocket       sock(\"%s\",%d,1);\n",tcpadr,tcpport);
  }
  fprintf(fout,"rlSharedMemory shm(\"%s\",%d);\n",shared_memory,base);
  fprintf(fout,"rlMailbox      mbx(\"%s\");\n",mailbox);
  fprintf(fout,"rlThread       thread;\n");
  fprintf(fout,"rlThread       watchdog;\n");
  fprintf(fout,"short          lifeCounter     = 0;\n");
  fprintf(fout,"short          readErrorCount  = 0;\n");
  fprintf(fout,"short          writeErrorCount = 0;\n");
  fprintf(fout,"\n");
  fprintf(fout,"// watchdog\n");
  fprintf(fout,"static const char *av0 = \"\";\n");
  fprintf(fout,"static int watchcnt1 = 0;\n");
  fprintf(fout,"void *watchdogthread(void *arg)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  int cnt1 = -1;\n");
  fprintf(fout,"\n");
  fprintf(fout,"  while(1)\n");
  fprintf(fout,"  {\n");
  fprintf(fout,"    rlsleep(60000);\n");
  fprintf(fout,"    if(cnt1 == watchcnt1) break;\n");
  fprintf(fout,"    cnt1 = watchcnt1;\n");
  fprintf(fout,"  }\n");
  if(communication == SERIAL) fprintf(fout,"  serial.closeDevice();\n");
  fprintf(fout,"  rlsleep(100);\n");
  fprintf(fout,"#ifdef unix\n");
  fprintf(fout,"  rlexec(av0);\n");
  fprintf(fout,"#endif\n");
  fprintf(fout,"  exit(0); // pcontrol may start me again if rlexec fails\n");
  fprintf(fout,"  return arg;\n");
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"// read mailbox and write to modbus\n");
  fprintf(fout,"void *reader(void *arg)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  int ret,slave,function,buflen;\n");
  fprintf(fout,"  unsigned char buf[256+1];\n");
  fprintf(fout,"\n");
  fprintf(fout,"  mbx.clear(); // clear old messages\n");
  fprintf(fout,"  while((buflen = mbx.read(buf,sizeof(buf))) > 0)\n");
  fprintf(fout,"  {\n");
  fprintf(fout,"    slave        = buf[0];\n");
  fprintf(fout,"    function     = buf[1];\n");
  fprintf(fout,"    thread.lock();\n");
  fprintf(fout,"    rlsleep(MODBUS_IDLETIME);\n");
  fprintf(fout,"    ret = modbus.write( slave, function, &buf[2], buflen-2);\n");
  fprintf(fout,"    ret = modbus.response( &slave, &function, buf);\n");
  if(communication != SERIAL) fprintf(fout,"    if(ret != rlModbus::MODBUS_SUCCESS) sock.disconnect();\n");
  if(eventport != -1)
  {
    fprintf(fout,"%s","    if(ret <= 0) { rlEvent(rlWarning,\"modbus.response mbx ret=%d slave=%d function=%d\",ret, slave, function); }\n");
  }  
  fprintf(fout,"    rlsleep(MODBUS_IDLETIME);\n");
  fprintf(fout,"    thread.unlock();\n");
  fprintf(fout,"    if(ret < 0)\n");
  fprintf(fout,"    {\n");
  fprintf(fout,"      writeErrorCount++;\n");
  fprintf(fout,"      if(writeErrorCount >= 256*128) writeErrorCount = 0;\n");
  fprintf(fout,"      shm.write(%s_WRITE_ERROR_COUNT_BASE,&writeErrorCount,2);\n",target);
  fprintf(fout,"    }\n");
  strcpy(line, "    printf(\"mbx ret=%d slave=%d function=%d buf[2]=%d\\n\",ret,slave,function,buf[2]);");
  fprintf(fout,"%s\n",line);
  fprintf(fout,"  }\n");
  fprintf(fout,"  return arg;\n");
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"// read cycle on modbus\n");
  fprintf(fout,"int modbusCycle(int offset, int slave, int function, int start_adr, int num_register)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  unsigned char data[256+1];\n");
  fprintf(fout,"  int ret;\n");
  fprintf(fout,"\n");
  fprintf(fout,"  watchcnt1++;\n");
  fprintf(fout,"  if(watchcnt1 > 10000) watchcnt1 = 0;\n");
  fprintf(fout,"  rlsleep(MODBUS_IDLETIME);\n");
  fprintf(fout,"  thread.lock();\n");
  fprintf(fout,"  ret = modbus.request(slave, function, start_adr, num_register);\n");
  fprintf(fout,"  if(ret >= 0) ret = modbus.response( &slave, &function, data);\n");
  if(communication == SOCKET) fprintf(fout,"  if(ret < 0) sock.disconnect();\n");
  fprintf(fout,"  thread.unlock();\n");
  fprintf(fout,"  if(ret > 0) shm.write(offset,data,ret);\n");
  fprintf(fout,"  else\n");
  fprintf(fout,"  {\n");
  fprintf(fout,"    readErrorCount++;\n");
  fprintf(fout,"    if(readErrorCount >= 256*128) readErrorCount = 0;\n");
  fprintf(fout,"    shm.write(%s_READ_ERROR_COUNT_BASE,&readErrorCount,2);\n",target);
  fprintf(fout,"  }\n");
  if(eventport != -1)
  {
    fprintf(fout,"%s","  if(ret <= 0) { rlEvent(rlWarning,\"modbus.response cycle ret=%d slave=%d function=%d start_adr=%d num_register=%d\",ret,slave,function,start_adr,num_register); rlsleep(5000); }\n");
  }  
  strcpy(line, "  printf(\"cycle ret=%d slave=%d function=%d data[0]=%d\\n\",ret,slave,function,data[0]);");
  fprintf(fout,"%s\n",line);
  fprintf(fout,"  return ret;\n");
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"int main(int ac, char **av)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  int offset,ret,first;\n");
  if(eventport != -1)
  {
       fprintf(fout,"  char *argv[] = {\"\",\"-eventhost=%s\",\"-eventport=%d\"};\n",eventhost,eventport);
       fprintf(fout,"\n");
       fprintf(fout,"  rlEventInit(3,argv,\"%s:\");\n",target);
  }
  fprintf(fout,"  if(ac > 0) av0 = av[0];\n");
  fprintf(fout,"  first = 1;\n");
  if(communication == SERIAL)
  {
    sprintf(line,"  while(serial.openDevice(SERIAL_DEVICE,B%d,1,%d,8,1,rlSerial::",baudrate,rtscts);
    if     (parity == 2) strcat(line,"EVEN) < 0)");
    else if(parity == 1) strcat(line,"ODD) < 0)");
    else                 strcat(line,"NONE) < 0)");
    fprintf(fout,"%s\n",line);
    //fprintf(fout,"  while(serial.openDevice(SERIAL_DEVICE,B%d) < 0)\n",baudrate);
    fprintf(fout,"  {\n");
    fprintf(fout,"%s","    if(first==1) printf(\"could not open serial device %s\\n\",SERIAL_DEVICE);\n");
    fprintf(fout,"    first = 0;\n");
    fprintf(fout,"    rlsleep(1000);\n");
    fprintf(fout,"  }\n");
  }
  fprintf(fout,"%s","  printf(\"\\n%s starting\\n\",av0);\n");
  if(eventport != -1)
  {
    fprintf(fout,"  rlEvent(rlInfo,\"%s starting\");\n",target); 
  }    
  if(communication == SERIAL)
  {
    fprintf(fout,"  modbus.registerSerial(&serial);\n");
  }
  else
  {
    fprintf(fout,"  modbus.registerSocket(&sock);\n");
  }
  fprintf(fout,"  thread.create(reader,NULL);\n");
  fprintf(fout,"  watchdog.create(watchdogthread,NULL);\n");
  fprintf(fout,"\n");
  fprintf(fout,"  shm.write(%s_LIFE_COUNTER_BASE,&lifeCounter,2);\n",target);
  fprintf(fout,"  shm.write(%s_READ_ERROR_COUNT_BASE,&readErrorCount,2);\n",target);
  fprintf(fout,"  shm.write(%s_WRITE_ERROR_COUNT_BASE,&writeErrorCount,2);\n",target);
  fprintf(fout,"  while(1)\n");
  fprintf(fout,"  {\n");
  fprintf(fout,"    lifeCounter++;\n");
  fprintf(fout,"    if(lifeCounter >= 256*128) lifeCounter = 0;\n");
  fprintf(fout,"    shm.write(%s_LIFE_COUNTER_BASE,&lifeCounter,2);\n",target);
  fprintf(fout,"    offset = 0;\n");
  fprintf(fout,"    //    modbusCycle(offset, slave, function, start_adr, num_register);\n");
  //fprintf(fout,"    ret = modbusCycle(offset, 1, rlModbus::ReadInputStatus, 0, 10);\n");

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
      slave = function = start_adr = num_register = -1;
      cptr = strstr(line,"slave=");
      if(cptr!=NULL) sscanf(cptr,"slave=%d",&slave);
      cptr = strstr(line,"function=");
      if(cptr!=NULL) sscanf(cptr,"function=%d",&function);
      cptr = strstr(line,"start_adr=");
      if(cptr!=NULL) sscanf(cptr,"start_adr=%d",&start_adr);
      cptr = strstr(line,"num_register=");
      if(cptr!=NULL) sscanf(cptr,"num_register=%d",&num_register);
      if(slave==-1 || function==-1 || start_adr==-1 || num_register==-1)
      {
        printf("parameter missing in: %s\n",line);
        fclose(fp);
        fclose(fout);
        exit(-1);
      }
      fprintf(fout,"    ret = modbusCycle(offset,%d,%d,%d,%d);\n",slave,function,start_adr,num_register);
      fprintf(fout,"    if(ret>0) offset += ret; else continue;\n");
    }
  }
  fclose(fp);

  fprintf(fout,"  }\n");
  fprintf(fout,"\n");
  fprintf(fout,"  // we will never come here\n");
  if(communication == SERIAL) fprintf(fout,"  serial.closeDevice();\n");
  fprintf(fout,"  return 0;\n");
  fprintf(fout,"}\n");

  fclose(fout);
}

int gmodbus(const char *name)
{
    f_info = QFileInfo(name);
    target = f_info.fileName().remove(".mkmodbus").toUtf8().data();
    init(name);
    generate(name);
    return 0;
}
