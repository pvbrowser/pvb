/****************************************************************************
**
** Copyright (C) 2000-2007 Lehrig Software Engineering.
**
** This file is part of the pvbrowser project.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "../pvbrowser/pvdefine.h"
#include <QDateTime>
#include <QStringList>
#include "opt.h"
#include "cutil.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef PVWIN32
#include <windows.h>
#include <direct.h>
#endif
#ifdef PVUNIX  
#include <unistd.h>
#endif

extern OPT_DEVELOP opt_develop;

extern QStringList tablist;
static int tabstopFound = 0;

int mysystem(const char *command)
{
  char cmd[4096];
#ifdef PVWIN32
  int ret;
  STARTUPINFO         si = { sizeof(si)};
  PROCESS_INFORMATION pi;

  if(strncmp(command,"start",5) == 0 || strncmp(command,"START",5) == 0)
  {
    ExpandEnvironmentStrings(command,cmd,sizeof(cmd)-1);
    ret  = system(cmd);
  }
  else if(strncmp(command,"wait ",5) == 0)
  {
    ExpandEnvironmentStrings(&command[5],cmd,sizeof(cmd)-1);
    ret = (int) CreateProcess( NULL, cmd
                             , NULL, NULL
                             , FALSE, CREATE_NO_WINDOW
                             , NULL, NULL
                             , &si, &pi);
    int pid = (int) pi.hProcess;
    long status;
    while(1)
    {
      if(GetExitCodeProcess((HANDLE) pid, (unsigned long *) &status) != 0) // success
      {
        if(status != STILL_ACTIVE) break;
      }
      else 
      {
        break;
      }  
      Sleep(500);
    }
  }
  else
  {
    ExpandEnvironmentStrings(command,cmd,sizeof(cmd)-1);
    ret = (int) CreateProcess( NULL, cmd
                             , NULL, NULL
                             , FALSE, CREATE_NO_WINDOW
                             , NULL, NULL
                             , &si, &pi);
  }
  return ret;
#else
  strcpy(cmd,command);
  strcat(cmd," &");
  return system(cmd);
#endif
}

static int actionDumpTranslations()
{
  char line[4096], *cptr, *end;

  system("grep \"pvtr(\\\"\" *.h   >  translate.dump.temp");
  system("grep \"pvtr(\\\"\" *.cpp >> translate.dump.temp");
  FILE *fin = fopen("translate.dump.temp","r");
  if(fin == NULL)
  {
    printf("could not wite translate.dump\n");
  }

  printf("[aLANGUAGE]\n");
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    cptr = &line[0];
    while((cptr = strstr(cptr,"pvtr(\"")) != NULL)
    {
      cptr += 6;
      end = strstr(cptr,"\")");
      if(end != NULL)
      {
        *end = '\0';
        end++;
        printf("%s=\n", cptr);
        if(*end != '\0') end++;
        cptr = end;
      }  
    }
  }

  fclose(fin);  
#ifdef PVUNIX  
  unlink("translate.dump.temp");
#endif  
  return 0;
}

int action(const char *command)
{
  FILE *fin;
  char name[1024],cmd[1024],project[1024];
  int imask, ret;
  int checked = 0;

  ret = 0;
  if(strlen(command) > 1024-80)
  {
    printf("You silly stupid DAU. Why use such a long name ???\n");
    printf("%s\n",command);
    return -1;
  }

  if(opt_develop.arg_debug) printf("action(%s)\n",command);
  if(strncmp(command,"qmake=",6) == 0 && opt_develop.script != PV_LUA)
  {
    sscanf(command,"qmake=%s",project);
#ifdef PVUNIX
    sprintf(cmd,"fake_qmake %s %s.pro",opt_develop.fake_qmake, project);
    printf("mysystem(%s)\n", cmd);
    mysystem(cmd);
#else
    sprintf(cmd,"qmake %s.pro",project);
    sprintf(cmd,"start pvb_qmake.bat %s", project);
    //system(cmd);
#endif
  }
  else if(strncmp(command,"insertMask=",11) == 0)
  {
    if(opt_develop.script == PV_LUA)
    {
      sscanf(command,"insertMask=%s",project);
      strcat(project,".pro");
      for(imask=1; ;imask++)
      {
        sprintf(name,"mask%d.lua",imask);
        fin = fopen(name,"r");
        if(fin == NULL)
        {
          generateInitialMask(imask);
          generateInitialSlots(imask);
          addMaskToMain(imask);
          return imask;
        }
        else
        {
          fclose(fin);
        }
      }  
    }
    else
    {
      sscanf(command,"insertMask=%s",project);
      strcat(project,".pro");
      for(imask=1; ;imask++)
      {
        sprintf(name,"mask%d.cpp",imask);
        fin = fopen(name,"r");
        if(fin == NULL)
        {
          generateInitialMask(imask);
          generateInitialSlots(imask);
          addMaskToProject(project,imask);
          addMaskToMain(imask);
          addMaskToHeader(imask);
          return imask;
        }
        else
        {
          fclose(fin);
        }
      }  
    }
  }
  else if(strncmp(command,"make",4) == 0 && opt_develop.script != PV_LUA)
  {
#ifdef PVUNIX
    sscanf(command,"make=%s",project);
    if(opt_develop.arg_action[0] != '\0') 
    {
      sprintf(cmd,"fake_qmake %s %s.pro;make", opt_develop.fake_qmake, project);
      if(opt_develop.arg_debug) printf("cmd1=%s\n",cmd);
      ret = system(cmd);
    }  
    else
    {
      sprintf(cmd,"xterm -e \"fake_qmake %s %s.pro;make;echo ready;read\"", opt_develop.fake_qmake, project);
      if(opt_develop.arg_debug) printf("cmd2=%s\n",cmd);
      mysystem(cmd);
      //mysystem("xterm -e \"make;echo ready;read\"");
    }  
#else
    sscanf(command,"make=%s",name);
    sprintf(cmd,"start pvb_make.bat %s", name);
    ret = system(cmd);
#endif
  }
  else if(strncmp(command,"startserver=",12) == 0)
  {
    sscanf(command,"startserver=%d:%s",&checked, name);
    if(checked == 1)
    {
#ifdef PVUNIX
      if(opt_develop.script == PV_LUA)
      {
        sprintf(cmd,"xterm -e \"su -c ./pvslua\"");
      }
      else
      {
#ifdef PVMAC
        sprintf(cmd,"xterm -e \"make;su -c ./%s.app/Contents/MacOS/%s\"", name, name);
#else
        sprintf(cmd,"xterm -e \"make;su -c ./%s\"", name);
#endif
      }
      if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
      mysystem(cmd);
#else
      if(opt_develop.script == PV_LUA)
      {
        sprintf(cmd,"start pvslua.exe");
      }
      else
      {
        sprintf(cmd,"start pvb_server.bat %s", name);
      }
      if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
      ret = system(cmd);
#endif
    }
    else
    {
#ifdef PVUNIX
      if(opt_develop.script == PV_LUA)
      {
        sprintf(cmd,"xterm -e \"pvslua\"");
      }
      else
      {
#ifdef PVMAC
        sprintf(cmd,"xterm -e \"make;./%s.app/Contents/MacOS/%s\"", name, name);
#else
        sprintf(cmd,"xterm -e \"make;./%s\"", name);
#endif
      }
      if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
      mysystem(cmd);
#else
      if(opt_develop.script == PV_LUA)
      {
        sprintf(cmd,"start pvslua.exe");
      }
      else
      {
        sprintf(cmd,"start pvb_server.bat %s", name);
      }
      if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
      ret = system(cmd);
#endif
    }
  }
  else if(strncmp(command,"startserver=",12) == 0)
  {
    sscanf(command,"startserver=%d:%s",&checked,name);
#ifdef PVUNIX
#ifdef PVMAC
    if(checked == 0)
    {
      sprintf(cmd,"xterm -e \"make;./%s.app/Contents/MacOS/%s;read\"", name, name);
    }
    else
    {
      sprintf(cmd,"xterm -e \"make;sudo ./%s.app/Contents/MacOS/%s\"", name, name);
    }
#else
    if(checked == 0)
    {
      sprintf(cmd,"xterm -e \"make;./%s\"", name);
    }
    else
    {
      sprintf(cmd,"xterm -e \"make;su -c ./%s\"", name);
    }
#endif    
    mysystem(cmd);
#else
    sprintf(cmd,"start pvb_startserver.bat %s", name);
    ret = system(cmd);
#endif
  }
  else if(strcmp(command,"pvbrowser") == 0)
  {
    mysystem("pvbrowser");
  }
  else if(strcmp(command,"dumpTranslations") == 0)
  {
    actionDumpTranslations();
  }  
  if(ret != 0) return 0;
  return 0;
}

void generateInitialProject(const char *name)
{
  char fname[1024],time[1024];
  FILE *fout;
  int ret = 0;

  if(strlen(name) > 1024-80)
  {
    printf("buffer for name=%s is too small\n", name);
    return;
  }

  // add additional language here
  if(opt_develop.script == PV_PYTHON)
  {
    strcpy(opt_develop.arg_project, "pvs");
#ifdef PVWIN32
    ret = system("pvb_copy_python_template.bat");
#else
    ret = system("cp /opt/pvb/language_bindings/python/template/* .");
#endif
    return;
  }
  else if(opt_develop.script == PV_PERL)
  {
    strcpy(opt_develop.arg_project, "pvs");
#ifdef PVWIN32
    ret = system("pvb_copy_perl_template.bat");
#else
    ret = system("cp /opt/pvb/language_bindings/perl/template/* .");
#endif
    return;
  }
  else if(opt_develop.script == PV_PHP)
  {
    strcpy(opt_develop.arg_project, "pvs");
#ifdef PVWIN32
    ret = system("pvb_copy_php_template.bat");
#else
    ret = system("cp /opt/pvb/language_bindings/php/template/* .");
#endif
    return;
  }
  else if(opt_develop.script == PV_TCL)
  {
    strcpy(opt_develop.arg_project, "pvs");
#ifdef PVWIN32
    ret = system("pvb_copy_tcl_template.bat");
#else
    ret = system("cp /opt/pvb/language_bindings/tcl/template/* .");
#endif
    return;
  }
  else if(opt_develop.script == PV_LUA)
  {
    return lua_generateInitialProject(name);
  }

  strcpy(time,QDateTime::currentDateTime().toString().toUtf8());

  strcpy(fname,name); strcat(fname,".pvproject");
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"target=%s\n", name);
  fprintf(fout,"xmax=1280\n");
  fprintf(fout,"ymax=1024\n");
  fclose(fout);

  strcpy(fname,name); strcat(fname,".pro");
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"######################################################################\n");
  fprintf(fout,"# generated by pvdevelop at: %s\n", time);
  fprintf(fout,"######################################################################\n");
  fprintf(fout,"\n");
  fprintf(fout,"TEMPLATE = app\n");
  fprintf(fout,"CONFIG  += warn_on release console\n");
  fprintf(fout,"CONFIG  -= qt\n");
  //the following line cost me a lot of headaches. it causes the apps crash on openSUSE 32 Bit :-(
  //fprintf(fout,"unix:QMAKE_LFLAGS += -static-libgcc\n");
  fprintf(fout,"\n");
  fprintf(fout,"# Input\n");
  fprintf(fout,"#HEADERS += rldefine.h\n");
  fprintf(fout,"HEADERS += pvapp.h      \\\n");
  fprintf(fout,"           mask1_slots.h\n");
  fprintf(fout,"SOURCES += main.cpp     \\\n");
  fprintf(fout,"           mask1.cpp\n");
  fprintf(fout,"\n");
  fprintf(fout,"!macx {\n");
  fprintf(fout,"unix:LIBS         += /usr/lib/libpvsmt.so -lpthread\n");
  fprintf(fout,"#unix:LIBS        += /usr/lib/libpvsid.so\n");
  fprintf(fout,"unix:INCLUDEPATH  += /opt/pvb/pvserver\n");
  fprintf(fout,"unix:LIBS         += /usr/lib/librllib.so\n");
  fprintf(fout,"unix:INCLUDEPATH  += /opt/pvb/rllib/lib\n");
  //fprintf(fout,"#unix:LIBS         += $(PVBDIR)/pvserver/libpvsmt.so -lpthread\n");
  //fprintf(fout,"#unix:LIBS         += $(PVBDIR)/pvserver/libpvsid.so -lpthread\n");
  //fprintf(fout,"#unix:INCLUDEPATH  += $(PVBDIR)/pvserver\n");
  //fprintf(fout,"#unix:LIBS         += $(PVBDIR)/rllib/lib/librllib.so\n");
  //fprintf(fout,"#unix:INCLUDEPATH  += $(PVBDIR)/rllib/lib\n");
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"macx:LIBS         += /opt/pvb/pvserver/libpvsmt.a /usr/lib/libpthread.dylib\n");
  fprintf(fout,"#macx:LIBS        += /opt/pvb/pvserver/libpvsid.a\n");
  fprintf(fout,"macx:INCLUDEPATH  += /opt/pvb/pvserver\n");
  fprintf(fout,"macx:LIBS         += /usr/local/lib/librllib.dylib\n");
  fprintf(fout,"macx:INCLUDEPATH  += /opt/pvb/rllib/lib\n");
  fprintf(fout,"\n");
  fprintf(fout,"#\n");
  fprintf(fout,"# Attention:\n");
  fprintf(fout,"# starting with mingw 4.8 we use mingw pthread and not our own mapping to windows threads\n");
  fprintf(fout,"# you will have to adjust existing pro files\n");
  fprintf(fout,"#\n");
  fprintf(fout,"win32-g++ {\n");
  fprintf(fout,"QMAKE_LFLAGS      += -static-libgcc\n");
  fprintf(fout,"win32:LIBS        += $(PVBDIR)/win-mingw/bin/libserverlib.a \n");
  fprintf(fout,"win32:LIBS        += $(PVBDIR)/win-mingw/bin/librllib.a\n");
  fprintf(fout,"win32:LIBS        += -lws2_32 -ladvapi32 -lpthread\n");
  fprintf(fout,"win32:INCLUDEPATH += $(PVBDIR)/pvserver\n");
  fprintf(fout,"win32:INCLUDEPATH += $(PVBDIR)/rllib/lib\n");
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"#DEFINES += USE_INETD\n");
  fprintf(fout,"TARGET = %s\n", name);
  fclose(fout);

  strcpy(fname,"main.cpp");
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"//***************************************************************************\n");
  fprintf(fout,"//                          main.cpp  -  description\n");
  fprintf(fout,"//                             -------------------\n");
  fprintf(fout,"//  begin            : %s\n", time);
  fprintf(fout,"//  generated by     : pvdevelop (C) Lehrig Software Engineering\n");
  fprintf(fout,"//  email            : lehrig@t-online.de\n");
  fprintf(fout,"//***************************************************************************\n");
  fprintf(fout,"#include \"pvapp.h\"\n");
  fprintf(fout,"// todo: comment me out. you can insert these objects as extern in your masks.\n");
  fprintf(fout,"// Change \"modbus\"(Change if applicable) and \"modbusdaemon_\" with the \"name\" of your file \"name.mkmodbus\".\n");
  fprintf(fout,"//rlModbusClient     modbus(modbusdaemon_MAILBOX,modbusdaemon_SHARED_MEMORY,modbusdaemon_SHARED_MEMORY_SIZE);\n");
  fprintf(fout,"//rlSiemensTCPClient siemensTCP(siemensdaemon_MAILBOX,siemensdaemon_SHARED_MEMORY,siemensdaemon_SHARED_MEMORY_SIZE);\n");
  fprintf(fout,"//rlPPIClient        ppi(ppidaemon_MAILBOX,ppidaemon_SHARED_MEMORY,ppidaemon_SHARED_MEMORY_SIZE);\n");
  fprintf(fout,"\n");
  fprintf(fout,"int pvMain(PARAM *p)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"int ret;\n");
  fprintf(fout,"\n");
  fprintf(fout,"  pvSendVersion(p);\n");
  fprintf(fout,"  pvSetCaption(p,\"%s\");\n", name);
  fprintf(fout,"  pvResize(p,0,1280,1024);\n");
  fprintf(fout,"  //pvScreenHint(p,1024,768); // this may be used to automatically set the zoomfactor\n");
  fprintf(fout,"  ret = 1;\n");
  fprintf(fout,"  pvGetInitialMask(p);\n");
  fprintf(fout,"  if(strcmp(p->initial_mask,\"mask1\") == 0) ret = 1;\n");
  fprintf(fout,"\n");
  fprintf(fout,"  while(1)\n");
  fprintf(fout,"  {\n");
  fprintf(fout,"%s","    if(trace) printf(\"show_mask%d\\n\", ret);\n");
  fprintf(fout,"    switch(ret)\n");
  fprintf(fout,"    {\n");
  fprintf(fout,"      case 1:\n");
  fprintf(fout,"        pvStatusMessage(p,-1,-1,-1,\"mask1\");\n");
  fprintf(fout,"        ret = show_mask1(p);\n");
  fprintf(fout,"        break;\n");
  fprintf(fout,"      default:\n");
  fprintf(fout,"        return 0;\n");
  fprintf(fout,"    }\n");
  fprintf(fout,"  }\n");
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"#ifdef USE_INETD\n");
  fprintf(fout,"int main(int ac, char **av)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"PARAM p;\n");
  fprintf(fout,"\n");
  fprintf(fout,"  pvInit(ac,av,&p);\n");
  fprintf(fout,"  /* here you may interpret ac,av and set p->user to your data */\n");
  fprintf(fout,"  pvMain(&p);\n");
  fprintf(fout,"  return 0;\n");
  fprintf(fout,"}\n");
  fprintf(fout,"#else  // multi threaded server\n");
  fprintf(fout,"int main(int ac, char **av)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"PARAM p;\n");
  fprintf(fout,"int   s;\n");
  fprintf(fout,"\n");
  fprintf(fout,"  pvInit(ac,av,&p);\n");
  fprintf(fout,"  /* here you may interpret ac,av and set p->user to your data */\n");
  fprintf(fout,"  while(1)\n");
  fprintf(fout,"  {\n");
  fprintf(fout,"    s = pvAccept(&p);\n");
  fprintf(fout,"    if(s != -1) pvCreateThread(&p,s);\n");
  fprintf(fout,"  }\n");
  fprintf(fout,"  return 0;\n");
  fprintf(fout,"}\n");
  fprintf(fout,"#endif\n");
  fclose(fout);

  strcpy(fname,"pvapp.h");
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"//***************************************************************************\n");
  fprintf(fout,"//                          pvapp.h  -  description\n");
  fprintf(fout,"//                             -------------------\n");
  fprintf(fout,"//  begin            : %s\n", time);
  fprintf(fout,"//  generated by     : pvdevelop (C) Lehrig Software Engineering\n");
  fprintf(fout,"//  email            : lehrig@t-online.de\n");
  fprintf(fout,"//***************************************************************************\n");
  fprintf(fout,"#ifndef _PVAPP_H_\n");
  fprintf(fout,"#define _PVAPP_H_\n");
  fprintf(fout,"\n");
  fprintf(fout,"#ifdef USE_INETD\n");
  fprintf(fout,"static int trace=0; // todo: set trace=0 if you do not want printf() within event loop\n");
  fprintf(fout,"#else\n");
  fprintf(fout,"static int trace=1; // todo: set trace=0 if you do not want printf() within event loop\n");
  fprintf(fout,"#endif\n");
  fprintf(fout,"\n");
  fprintf(fout,"#include \"processviewserver.h\"\n");
  fprintf(fout,"// todo: comment me out\n");
  fprintf(fout,"//#include \"rlmodbusclient.h\"\n");
  fprintf(fout,"//#include \"rlsiemenstcpclient.h\"\n");
  fprintf(fout,"//#include \"rlppiclient.h\"\n");
  fprintf(fout,"//#include \"modbusdaemon.h\"             // this is generated. Change for name.h -> \"name.mkmodbus\"\n");
  fprintf(fout,"//#include \"siemensdaemon.h\"            // this is generated\n");
  fprintf(fout,"//#include \"ppidaemon.h\"                // this is generated\n");
  fprintf(fout,"\n");
  fprintf(fout,"int show_mask1(PARAM *p);\n");
  fprintf(fout,"\n");
  fprintf(fout,"#endif\n");
  fclose(fout);

  generateInitialMask(1);

  generateInitialSlots(1);
  
  if(ret < 0) printf("generateInitialProject:error in system() call\n");
}

void generateInitialMask(int imask)
{
  FILE *fout;
  char fname[80],maskname[80],time[80];
  int ret = 0;

  if(opt_develop.script == PV_LUA) return lua_generateInitialMask(imask);
  strcpy(time,QDateTime::currentDateTime().toString().toUtf8());
  sprintf(maskname,"mask%d",imask);
  sprintf(fname,"mask%d.cpp",imask);
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"////////////////////////////////////////////////////////////////////////////\n");
  fprintf(fout,"//\n"                                                                          );
  fprintf(fout,"// show_%s for ProcessViewServer created: %s\n" ,maskname , time               );
  fprintf(fout,"//\n"                                                                          );
  fprintf(fout,"////////////////////////////////////////////////////////////////////////////\n");
  fprintf(fout,"#include \"pvapp.h\"\n"                                                        );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------\n");
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"// our mask contains the following objects\n"                                  );
  fprintf(fout,"enum {\n"                                                                      );
  fprintf(fout,"  ID_MAIN_WIDGET = 0,\n"                                                       );
  fprintf(fout,"  ID_END_OF_WIDGETS\n"                                                         );
  fprintf(fout,"};\n"                                                                          );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"static const char *toolTip[] = {\n"                                            );
  fprintf(fout,"  \"\"\n"                                                                      );
  fprintf(fout,"};\n"                                                                          );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"static const char *whatsThis[] = {\n"                                          );
  fprintf(fout,"  \"\"\n"                                                                      );
  fprintf(fout,"};\n"                                                                          );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"static int generated_defineMask(PARAM *p)\n"                                   );
  fprintf(fout,"{\n"                                                                           );
  fprintf(fout,"  int w,h,depth;\n"                                                            );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"  if(p == NULL) return 1;\n"                                                   );
  fprintf(fout,"  w = h = depth = strcmp(toolTip[0],whatsThis[0]);\n"                          );
  fprintf(fout,"  if(w==h) depth=0; // fool the compiler\n"                                    );
  fprintf(fout,"  pvStartDefinition(p,ID_END_OF_WIDGETS);\n"                                   );
  fprintf(fout,"  pvEndDefinition(p);\n"                                                       );
  fprintf(fout,"  return 0;\n"                                                                 );
  fprintf(fout,"}\n"                                                                           );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------\n");
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"#include \"%s_slots.h\"\n", maskname                                           );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"static int defineMask(PARAM *p)\n"                                             );
  fprintf(fout,"{\n"                                                                           );
  fprintf(fout,"  if(p == NULL) return 1;\n"                                                   );
  fprintf(fout,"  generated_defineMask(p);\n"                                                  );
  fprintf(fout,"  // (todo: add your code here)\n"                                             );
  fprintf(fout,"  return 0;\n"                                                                 );
  fprintf(fout,"}\n"                                                                           );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"static int showData(PARAM *p, DATA *d)\n"                                      );
  fprintf(fout,"{\n"                                                                           );
  fprintf(fout,"  if(p == NULL) return 1;\n"                                                   );
  fprintf(fout,"  if(d == NULL) return 1;\n"                                                   );
  fprintf(fout,"  return 0;\n"                                                                 );
  fprintf(fout,"}\n"                                                                           );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"static int readData(DATA *d) // from shared memory, database or something else\n");
  fprintf(fout,"{\n"                                                                           );
  fprintf(fout,"  if(d == NULL) return 1;\n"                                                   );
  fprintf(fout,"  // (todo: add your code here)\n"                                             );
  fprintf(fout,"  return 0;\n"                                                                 );
  fprintf(fout,"}\n"                                                                           );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"\n"                                                                            );
  fprintf(fout,"int show_%s(PARAM *p)\n", maskname                                             );
  fprintf(fout,"%s","{\n"                                                                           );
  fprintf(fout,"%s","  DATA d;\n"                                                                   );
  fprintf(fout,"%s","  char event[MAX_EVENT_LENGTH];\n"                                             );
  fprintf(fout,"%s","  char text[MAX_EVENT_LENGTH];\n"                                              );
  fprintf(fout,"%s","  char str1[MAX_EVENT_LENGTH];\n"                                              );
  fprintf(fout,"%s","  int  i,w,h,val,x,y,button,ret;\n"                                            );
  fprintf(fout,"%s","  float xval, yval;\n"                                                         );
  fprintf(fout,"%s","\n"                                                                            );
  fprintf(fout,"%s","  defineMask(p);\n"                                                            );
  fprintf(fout,"%s","  //rlSetDebugPrintf(1);\n"                                                    );
  fprintf(fout,"%s","  if((ret=slotInit(p,&d)) != 0) return ret;\n"                                 );
  fprintf(fout,"%s","  readData(&d); // from shared memory, database or something else\n"           );
  fprintf(fout,"%s","  showData(p,&d);\n"                                                           );
  fprintf(fout,"%s","  pvClearMessageQueue(p);\n"                                                   );
  fprintf(fout,"%s","  while(1)\n"                                                                  );
  fprintf(fout,"%s","  {\n"                                                                         );
  fprintf(fout,"%s","    pvPollEvent(p,event);\n"                                                   );
  fprintf(fout,"%s","    switch(pvParseEvent(event, &i, text))\n"                                   );
  fprintf(fout,"%s","    {\n"                                                                       );
  fprintf(fout,"%s","      case NULL_EVENT:\n"                                                      );
  fprintf(fout,"%s","        readData(&d); // from shared memory, database or something else\n"     );
  fprintf(fout,"%s","        showData(p,&d);\n"                                                     );
  fprintf(fout,"%s","        if((ret=slotNullEvent(p,&d)) != 0) return ret;\n"                      );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case BUTTON_EVENT:\n"                                                    );
  fprintf(fout,"%s","        if(trace) printf(\"BUTTON_EVENT id=%d\\n\",i);\n"                      );
  fprintf(fout,"%s","        if((ret=slotButtonEvent(p,i,&d)) != 0) return ret;\n"                  );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case BUTTON_PRESSED_EVENT:\n"                                            );
  fprintf(fout,"%s","        if(trace) printf(\"BUTTON_PRESSED_EVENT id=%d\\n\",i);\n"              );
  fprintf(fout,"%s","        if((ret=slotButtonPressedEvent(p,i,&d)) != 0) return ret;\n"           );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case BUTTON_RELEASED_EVENT:\n"                                           );
  fprintf(fout,"%s","        if(trace) printf(\"BUTTON_RELEASED_EVENT id=%d\\n\",i);\n"             );
  fprintf(fout,"%s","        if((ret=slotButtonReleasedEvent(p,i,&d)) != 0) return ret;\n"          );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case TEXT_EVENT:\n"                                                      );
  fprintf(fout,"%s","        if(trace) printf(\"TEXT_EVENT id=%d %s\\n\",i,text);\n"                );
  fprintf(fout,"%s","        if((ret=slotTextEvent(p,i,&d,text)) != 0) return ret;\n"               );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case SLIDER_EVENT:\n"                                                    );
  fprintf(fout,"%s","        sscanf(text,\"(%d)\",&val);\n"                                         );
  fprintf(fout,"%s","        if(trace) printf(\"SLIDER_EVENT val=%d\\n\",val);\n"                   );
  fprintf(fout,"%s","        if((ret=slotSliderEvent(p,i,&d,val)) != 0) return ret;\n"              );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case CHECKBOX_EVENT:\n"                                                  );
  fprintf(fout,"%s","        if(trace) printf(\"CHECKBOX_EVENT id=%d %s\\n\",i,text);\n"            );
  fprintf(fout,"%s","        if((ret=slotCheckboxEvent(p,i,&d,text)) != 0) return ret;\n"           );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case RADIOBUTTON_EVENT:\n"                                               );
  fprintf(fout,"%s","        if(trace) printf(\"RADIOBUTTON_EVENT id=%d %s\\n\",i,text);\n"         );
  fprintf(fout,"%s","        if((ret=slotRadioButtonEvent(p,i,&d,text)) != 0) return ret;\n"        );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case GL_INITIALIZE_EVENT:\n"                                             );
  fprintf(fout,"%s","        if(trace) printf(\"you have to call initializeGL()\\n\");\n"           );
  fprintf(fout,"%s","        if((ret=slotGlInitializeEvent(p,i,&d)) != 0) return ret;\n"            );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case GL_PAINT_EVENT:\n"                                                  );
  fprintf(fout,"%s","        if(trace) printf(\"you have to call paintGL()\\n\");\n"                );
  fprintf(fout,"%s","        if((ret=slotGlPaintEvent(p,i,&d)) != 0) return ret;\n"                 );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case GL_RESIZE_EVENT:\n"                                                 );
  fprintf(fout,"%s","        sscanf(text,\"(%d,%d)\",&w,&h);\n"                                     );
  fprintf(fout,"%s","        if(trace) printf(\"you have to call resizeGL(w,h)\\n\");\n"            );
  fprintf(fout,"%s","        if((ret=slotGlResizeEvent(p,i,&d,w,h)) != 0) return ret;\n"            );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case GL_IDLE_EVENT:\n"                                                   );
  fprintf(fout,"%s","        if((ret=slotGlIdleEvent(p,i,&d)) != 0) return ret;\n"                  );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case TAB_EVENT:\n"                                                       );
  fprintf(fout,"%s","        sscanf(text,\"(%d)\",&val);\n"                                         );
  fprintf(fout,"%s","        if(trace) printf(\"TAB_EVENT(%d,page=%d)\\n\",i,val);\n"               );
  fprintf(fout,"%s","        if((ret=slotTabEvent(p,i,&d,val)) != 0) return ret;\n"                 );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case TABLE_TEXT_EVENT:\n"                                                );
  fprintf(fout,"%s","        sscanf(text,\"(%d,%d,\",&x,&y);\n"                                     );
  fprintf(fout,"%s","        pvGetText(text,str1);\n"                                               );
  fprintf(fout,"%s","        if(trace) printf(\"TABLE_TEXT_EVENT(%d,%d,\\\"%s\\\")\\n\",x,y,str1);\n");
  fprintf(fout,"%s","        if((ret=slotTableTextEvent(p,i,&d,x,y,str1)) != 0) return ret;\n"      );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case TABLE_CLICKED_EVENT:\n"                                             );
  fprintf(fout,"%s","        sscanf(text,\"(%d,%d,%d)\",&x,&y,&button);\n"                          );
  fprintf(fout,"%s","        if(trace) printf(\"TABLE_CLICKED_EVENT(%d,%d,button=%d)\\n\",x,y,button);\n");
  fprintf(fout,"%s","        if((ret=slotTableClickedEvent(p,i,&d,x,y,button)) != 0) return ret;\n" );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case SELECTION_EVENT:\n"                                                 );
  fprintf(fout,"%s","        sscanf(text,\"(%d,\",&val);\n"                                         );
  fprintf(fout,"%s","        pvGetText(text,str1);\n"                                               );
  fprintf(fout,"%s","        if(trace) printf(\"SELECTION_EVENT(column=%d,\\\"%s\\\")\\n\",val,str1);\n");
  fprintf(fout,"%s","        if((ret=slotSelectionEvent(p,i,&d,val,str1)) != 0) return ret;\n"      );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case CLIPBOARD_EVENT:\n"                                                 );
  fprintf(fout,"%s","        sscanf(text,\"(%d\",&val);\n"                                          );
  fprintf(fout,"%s","        if(trace) printf(\"CLIPBOARD_EVENT(id=%d)\\n\",val);\n"                );
  //fprintf(fout,"%s","        if(trace) printf(\"clipboard = \\n%s\\n\",p->clipboard);\n"            );
  fprintf(fout,"%s","        if((ret=slotClipboardEvent(p,i,&d,val)) != 0) return ret;\n"           );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case RIGHT_MOUSE_EVENT:\n"                                               );
  fprintf(fout,"%s","        if(trace) printf(\"RIGHT_MOUSE_EVENT id=%d text=%s\\n\",i,text);\n"    );
  fprintf(fout,"%s","        if((ret=slotRightMouseEvent(p,i,&d,text)) != 0) return ret;\n"         );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case KEYBOARD_EVENT:\n"                                                  );
  fprintf(fout,"%s","        sscanf(text,\"(%d\",&val);\n"                                          );
  fprintf(fout,"%s","        if(trace) printf(\"KEYBOARD_EVENT modifier=%d key=%d\\n\",i,val);\n"   );
  fprintf(fout,"%s","        if((ret=slotKeyboardEvent(p,i,&d,val,i)) != 0) return ret;\n"          );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case PLOT_MOUSE_MOVED_EVENT:\n"                                          );
  fprintf(fout,"%s","        sscanf(text,\"(%f,%f)\",&xval,&yval);\n"                               );
  fprintf(fout,"%s","        if(trace) printf(\"PLOT_MOUSE_MOVE %f %f\\n\",xval,yval);\n"           );
  fprintf(fout,"%s","        if((ret=slotMouseMovedEvent(p,i,&d,xval,yval)) != 0) return ret;\n"    );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case PLOT_MOUSE_PRESSED_EVENT:\n"                                        );
  fprintf(fout,"%s","        sscanf(text,\"(%f,%f)\",&xval,&yval);\n"                               );
  fprintf(fout,"%s","        if(trace) printf(\"PLOT_MOUSE_PRESSED %f %f\\n\",xval,yval);\n"        );
  fprintf(fout,"%s","        if((ret=slotMousePressedEvent(p,i,&d,xval,yval)) != 0) return ret;\n"  );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case PLOT_MOUSE_RELEASED_EVENT:\n"                                       );
  fprintf(fout,"%s","        sscanf(text,\"(%f,%f)\",&xval,&yval);\n"                               );
  fprintf(fout,"%s","        if(trace) printf(\"PLOT_MOUSE_RELEASED %f %f\\n\",xval,yval);\n"       );
  fprintf(fout,"%s","        if((ret=slotMouseReleasedEvent(p,i,&d,xval,yval)) != 0) return ret;\n" );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case MOUSE_OVER_EVENT:\n"                                                );
  fprintf(fout,"%s","        sscanf(text,\"%d\",&val);\n"                                           );
  fprintf(fout,"%s","        if(trace) printf(\"MOUSE_OVER_EVENT %d\\n\",val);\n"                   );
  fprintf(fout,"%s","        if((ret=slotMouseOverEvent(p,i,&d,val)) != 0) return ret;\n"           );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      case USER_EVENT:\n"                                                      );
  fprintf(fout,"%s","        if(trace) printf(\"USER_EVENT id=%d %s\\n\",i,text);\n"                );
  fprintf(fout,"%s","        if((ret=slotUserEvent(p,i,&d,text)) != 0) return ret;\n"               );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","      default:\n"                                                              );
  fprintf(fout,"%s","        if(trace) printf(\"UNKNOWN_EVENT id=%d %s\\n\",i,text);\n"             );
  fprintf(fout,"%s","        break;\n"                                                              );
  fprintf(fout,"%s","    }\n"                                                                       );
  fprintf(fout,"%s","  }\n"                                                                         );
  fprintf(fout,"%s","}\n"                                                                           );
  fclose(fout);
  if(ret < 0) printf("generateInitialMask:error in system() call\n");
}

void generateInitialSlots(int imask)
{
  FILE *fout;
  char fname[80],maskname[80],time[80];

  if(opt_develop.script == PV_LUA) return lua_generateInitialSlots(imask);
  strcpy(time,QDateTime::currentDateTime().toString().toUtf8());
  sprintf(maskname,"mask%d",imask);
  sprintf(fname,"mask%d_slots.h",imask);
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }

  fprintf(fout,"//###############################################################\n" );
  fprintf(fout,"//# %s_slots.h for ProcessViewServer created: %s\n", maskname, time  );
  fprintf(fout,"//# please fill out these slots\n"                                   );
  fprintf(fout,"//# here you find all possible events\n"                             );
  fprintf(fout,"//# Yours: Lehrig Software Engineering\n"                            );
  fprintf(fout,"//###############################################################\n" );
  fprintf(fout,"\n"                                                                  );
  fprintf(fout,"// todo: uncomment me if you want to use this data aquisiton\n");
  fprintf(fout,"// also uncomment this classes in main.cpp and pvapp.h\n");
  fprintf(fout,"// also remember to uncomment rllib in the project file\n");
  fprintf(fout,"//extern rlModbusClient     modbus;  //Change if applicable\n");
  fprintf(fout,"//extern rlSiemensTCPClient siemensTCP;\n");
  fprintf(fout,"//extern rlPPIClient        ppi;\n");
  fprintf(fout,"\n");
  fprintf(fout,"typedef struct // (todo: define your data structure here)\n");
  fprintf(fout,"{\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  char this_python[32];\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"DATA;\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotInit(PARAM *p, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || d == NULL) return -1;\n");
  fprintf(fout,"  //memset(d,0,sizeof(DATA));\n"                                            );
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  strcpy(d->this_python, __FILE__); // __FILE__ = \"maskX_slots.h\" < 15 characters\n");
    fprintf(fout,"  pvPy_SetInstance((pvPyCleanupStruct *) p->app_data, d->this_python);\n");
    fprintf(fout,"\n");
    fprintf(fout,"  PyMethod(\"slotInit\");\n");
    fprintf(fout,"  PyBuildValue(\"(i)\", p->s);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotNullEvent(PARAM *p, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || d == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotNullEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(i)\", p->s);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotButtonEvent(PARAM *p, int id, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotButtonEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(ii)\", p->s, id);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotButtonPressedEvent(PARAM *p, int id, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotButtonPressedEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(ii)\", p->s, id);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotButtonReleasedEvent(PARAM *p, int id, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotButtonReleasedEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(ii)\", p->s, id);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotTextEvent(PARAM *p, int id, DATA *d, const char *text)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotTextEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(ii)\", p->s, id);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotSliderEvent(PARAM *p, int id, DATA *d, int val)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotSliderEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iii)\", p->s, id, val);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotCheckboxEvent(PARAM *p, int id, DATA *d, const char *text)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotCheckboxEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iis)\", p->s, id, text);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotRadioButtonEvent(PARAM *p, int id, DATA *d, const char *text)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotRadioButtonEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iis)\", p->s, id, text);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotGlInitializeEvent(PARAM *p, int id, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotGlInitializeEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(ii)\", p->s, id);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotGlPaintEvent(PARAM *p, int id, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotGlPaintEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(ii)\", p->s, id);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotGlResizeEvent(PARAM *p, int id, DATA *d, int width, int height)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || width < 0 || height < 0) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotGlResizeEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiii)\", p->s, id, width, height);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotGlIdleEvent(PARAM *p, int id, DATA *d)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotGlIdleEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(ii)\", p->s, id);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotTabEvent(PARAM *p, int id, DATA *d, int val)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotTabEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iii)\", p->s, id, val);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotTableTextEvent(PARAM *p, int id, DATA *d, int x, int y, const char *text)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || text == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotTableTextEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiiis)\", p->s, id, x, y, text);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotTableClickedEvent(PARAM *p, int id, DATA *d, int x, int y, int button)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || button < 0) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotTableClickedEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiiii)\", p->s, id, x, y, button);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotSelectionEvent(PARAM *p, int id, DATA *d, int val, const char *text)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || val < -1000 || text == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotSelectionEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiis)\", p->s, id, val, text);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotClipboardEvent(PARAM *p, int id, DATA *d, int val)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == -1 || d == NULL || val < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotClipboardEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiis)\", p->s, id, val, p->clipboard);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotRightMouseEvent(PARAM *p, int id, DATA *d, const char *text)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;\n");
  fprintf(fout,"  //pvPopupMenu(p,-1,\"Menu1,Menu2,,Menu3\");\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotRightMouseEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iis)\", p->s, id, text);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotKeyboardEvent(PARAM *p, int id, DATA *d, int val, int modifier)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || val < -1000 || modifier < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotKeyboardEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiii)\", p->s, id, val, modifier);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotMouseMovedEvent(PARAM *p, int id, DATA *d, float x, float y)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotMouseMovedEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiff)\", p->s, id, x, y);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotMousePressedEvent(PARAM *p, int id, DATA *d, float x, float y)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotMousePressedEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiff)\", p->s, id, x, y);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotMouseReleasedEvent(PARAM *p, int id, DATA *d, float x, float y)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotMouseReleasedEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iiff)\", p->s, id, x, y);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotMouseOverEvent(PARAM *p, int id, DATA *d, int enter)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || enter < -1000) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotMouseOverEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iii)\", p->s, id, enter);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  fprintf(fout,"static int slotUserEvent(PARAM *p, int id, DATA *d, const char *text)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;\n");
  if(opt_develop.script == PV_PYTHON)
  {
    fprintf(fout,"  PyMethod(\"slotUserEvent\");\n");
    fprintf(fout,"  PyBuildValue(\"(iis)\", p->s, id, text);\n");
    fprintf(fout,"  PyReturn\n");
  }
  else
  {
    fprintf(fout,"  return 0;\n");
  }
  fprintf(fout,"}\n");

  fclose(fout);

  //#### Python #####################################################
  if(opt_develop.script == PV_PYTHON)
  {
    sprintf(fname,"mask%d.py",imask);
    fout = fopen(fname,"w");
    if(fout == NULL)
    {
      printf("could not open %s for writing\n", fname);
      return;
    }

    fprintf(fout,"### pvbrowser slots written in python #################\n");
    fprintf(fout,"### begin of generated area, do not edit ##############\n");
    fprintf(fout,"import pv, rllib\n");
    fprintf(fout,"\n");
    fprintf(fout,"class mask%d:\n", imask);
    fprintf(fout,"  p = pv.PARAM()\n");
    fprintf(fout,"### end of generated area, do not edit ################\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotInit(self, s):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotNullEvent(self, s):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotButtonEvent(self, s, id):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotButtonPressedEvent(self, s, id):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotButtonReleasedEvent(self, s, id):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotTextEvent(self, s, id, text):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotSliderEvent(self, s, id, val):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotCheckboxEvent(self, s, id, text):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotRadioButtonEvent(self, s, id, text):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotGlInitializeEvent(self, s, id):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotGlPaintEvent(self, s, id):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotGlResizeEvent(self, s, id, width, height):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotGlIdleEvent(self, s, id):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotTabEvent(self, s, id, val):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotTableTextEvent(self, s, id, x, y, text):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotTableClickedEvent(self, s, id, x, y, button):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotSelectionEvent(self, s, id, val, text):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotClipboardEvent(self, s, id, val):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotRightMouseEvent(self, s, id, text):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotKeyboardEvent(self, s, id, val, modifier):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotMouseMovedEvent(self, s, id, x, y):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotMousePressedEvent(self, s, id, x, y):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotMouseReleasedEvent(self, s, id, x, y):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotMouseOverEvent(self, s, id, enter):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");
    fprintf(fout,"\n");
    fprintf(fout,"  def slotUserEvent(self, s, id, text):\n");
    fprintf(fout,"    self.p.s = self.p.os = s # set socket must be the first command\n");
    fprintf(fout,"    return 0\n");

    fclose(fout);
  }
}

FileLines file_lines;
static int file_loaded = 0;

int loadFile(const char *name)
{
  FILE *fin;
  char line[4096];
  FileLines *fl;

  fl = &file_lines;
  if(name == NULL) return -1;
  if((fin = fopen(name,"r")) == NULL)
  {
    printf("loadFile could not read %s\n",name);
    return -1;
  }

  if(opt_develop.arg_debug) printf("loadFile(%s)\n",name);
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    //if(opt_develop.arg_debug) printf("loadFile line=%s",line);
    fl->next = new FileLines;
    fl = fl->next;
    fl->line = new char [strlen(line)+1];
    strcpy(fl->line,line);
    fl->next = NULL;
  }

  fclose(fin);
  file_loaded = 1;
  return 0;
}

int unloadFile()
{
  FileLines *fl,*flold;

  if(file_loaded == 0) return 0;
  if(opt_develop.arg_debug) printf("unloadFile\n");
  fl = &file_lines;
  fl = fl->next;
  while(fl != NULL)
  {
    //if(opt_develop.arg_debug) printf("unloadFile fl->line=%s",fl->line);
    delete [] fl->line;
    flold = fl;
    fl = fl->next;
    delete flold;
  }
  file_loaded = 0;
  return 0;
}

void addMaskToProject(const char *name, int imask)
{
  FILE *fout;
  FileLines *fl;

  if(opt_develop.arg_debug) printf("addMaskToProject imask=%d name=%s\n",imask,name);
  if(opt_develop.script == PV_LUA) return;
  if(loadFile(name) == -1) return;
  fout = fopen(name,"w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(strstr(fl->line,"HEADERS") != NULL && strstr(fl->line,"pvapp.h") != NULL)
    {
      fprintf(fout,"%s",fl->line);
      fprintf(fout,"           mask%d_slots.h \\\n",imask);
    }
    else if(strstr(fl->line,"SOURCES") != NULL && strstr(fl->line,"main.cpp") != NULL)
    {
      fprintf(fout,"%s",fl->line);
      fprintf(fout,"           mask%d.cpp \\\n",imask);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

void addMaskToMain(int imask)
{
  FILE *fout;
  FileLines *fl;
  int while_found, switch_found, case_found, done;

  if(opt_develop.arg_debug) printf("addMaskToMain imask=%d\n",imask);
  if(opt_develop.script == PV_LUA) return lua_addMaskToMain(imask);
  if(loadFile("main.cpp") == -1) return;
  while_found = switch_found = case_found = done = 0;
  fout = fopen("main.cpp","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(while_found==0 && strstr(fl->line,"while(1)") != NULL)
    {
      while_found = 1;
    }
    if(while_found==1 && switch_found==0 && strstr(fl->line,"switch(ret)") != NULL)
    {
      switch_found = 1;
    }
    if(while_found==1 && switch_found==1 && case_found==0 && strstr(fl->line,"case") != NULL)
    {
      case_found = 1;
    }
    if(while_found==1 && switch_found==1 && case_found==1 && done==0 && strstr(fl->line,"case") != NULL)
    {
      fprintf(fout,"      case %d:\n",imask);
      fprintf(fout,"        pvStatusMessage(p,-1,-1,-1,\"mask%d\");\n",imask);
      fprintf(fout,"        ret = show_mask%d(p);\n",imask);
      fprintf(fout,"        break;\n");
      fprintf(fout,"%s",fl->line);
      done = 1;
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

void addMaskToHeader(int imask)
{
  FILE *fout;
  FileLines *fl;
  int done;

  if(opt_develop.arg_debug) printf("addMaskToHeader imask=%d\n",imask);
  if(opt_develop.script == PV_LUA) return;
  if(loadFile("pvapp.h") == -1) return;
  fout = fopen("pvapp.h","w");
  if(fout == NULL) return;
  done = 0;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(done == 0 && strncmp(fl->line,"int",3) == 0 && strstr(fl->line,"int show_mask") != NULL)
    {
      fprintf(fout,"int show_mask%d(PARAM *p);\n",imask);
      fprintf(fout,"%s",fl->line);
      done = 1;
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

void uncommentRllib(const char *project)
{
  FILE *fout;
  FileLines *fl;

  if(opt_develop.script == PV_LUA) return;
  if(loadFile(project) == -1) return;
  fout = fopen(project,"w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(fl->line[0]=='#' && (strstr(fl->line,"rllib") != NULL || strstr(fl->line,"RLLIB") != NULL))
    {
      fprintf(fout,"%s",&fl->line[1]);
    }
    else if(fl->line[0]=='#' && strstr(fl->line,"rldefine.h") != NULL )
    {
      fprintf(fout,"%s",&fl->line[1]);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

void uncommentModbus()
{
  FILE *fout;
  FileLines *fl;

  if(opt_develop.script == PV_LUA) return;
  if(loadFile("main.cpp") == -1) return;
  fout = fopen("main.cpp","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(strncmp(fl->line,"//rlModbusClient",16) == 0)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();

  if(loadFile("pvapp.h") == -1) return;
  fout = fopen("pvapp.h","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(strncmp(fl->line,"//",2) == 0 && strstr(fl->line,"rlmodbusclient.h") != NULL)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else if(strncmp(fl->line,"//",2) == 0 && strstr(fl->line,"modbusdaemon.h") != NULL)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

void uncommentSiemenstcp()
{
  FILE *fout;
  FileLines *fl;

  if(opt_develop.script == PV_LUA) return;
  if(loadFile("main.cpp") == -1) return;
  fout = fopen("main.cpp","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(strncmp(fl->line,"//rlSiemensTCPClient",20) == 0)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();

  if(loadFile("pvapp.h") == -1) return;
  fout = fopen("pvapp.h","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(strncmp(fl->line,"//",2) == 0 && strstr(fl->line,"rlsiemenstcpclient.h") != NULL)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else if(strncmp(fl->line,"//",2) == 0 && strstr(fl->line,"siemensdaemon.h") != NULL)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

void uncommentPpi()
{
  FILE *fout;
  FileLines *fl;

  if(opt_develop.script == PV_LUA) return;
  if(loadFile("main.cpp") == -1) return;
  fout = fopen("main.cpp","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(strncmp(fl->line,"//rlPPIClient",13) == 0)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();

  if(loadFile("pvapp.h") == -1) return;
  fout = fopen("pvapp.h","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(strncmp(fl->line,"//",2) == 0 && strstr(fl->line,"rlppiclient.h") != NULL)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else if(strncmp(fl->line,"//",2) == 0 && strstr(fl->line,"ppidaemon.h") != NULL)
    {
      fprintf(fout,"%s",&fl->line[2]);
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

int writeStartscript(const char *dir, const char *name)
{
  FILE *fout;
  char buf[80];

  fout = fopen("startscript","w");
  if(fout == NULL) return -1;

  fprintf(fout,"%s","#!/bin/sh\n");
  fprintf(fout,"%s","# generated by pvbuilder. Please adjust DAEMON_PATH and DAEMON to your needs.\n");
  fprintf(fout,"%s","# copy this file to /etc/init.d and link it to runlevel 5 .\n");
  fprintf(fout,"DAEMON_PATH=%s\n",dir);
  fprintf(fout,"DAEMON=%s\n",name);
  fprintf(fout,"%s",". /etc/rc.status\n");
  fprintf(fout,"%s","rc_reset\n");
  fprintf(fout,"%s","case \"$1\" in\n");
  fprintf(fout,"%s","    start)\n");
  fprintf(fout,"%s","      echo -n \"Starting $DAEMON\"\n");
  fprintf(fout,"%s","      startproc $DAEMON_PATH/$DAEMON -sleep=100 -cd=$DAEMON_PATH > /dev/null\n");
  fprintf(fout,"%s","      rc_status -v\n");
  fprintf(fout,"%s","      ;;\n");
  fprintf(fout,"%s","    stop)\n");
  fprintf(fout,"%s","      echo -n \"Shutting down $DAEMON\"\n");
  fprintf(fout,"%s","      killproc -TERM $DAEMON_PATH/$DAEMON\n");
  fprintf(fout,"%s","      rc_status -v\n");
  fprintf(fout,"%s","      ;;\n");
  fprintf(fout,"%s","    try-restart)\n");
  fprintf(fout,"%s","      $0 status >/dev/null &&  $0 restart\n");
  fprintf(fout,"%s","      rc_status\n");
  fprintf(fout,"%s","      ;;\n");
  fprintf(fout,"%s","    restart)\n");
  fprintf(fout,"%s","      $0 stop\n");
  fprintf(fout,"%s","      $0 start\n");
  fprintf(fout,"%s","      rc_status\n");
  fprintf(fout,"%s","      ;;\n");
  fprintf(fout,"%s","    force-reload)\n");
  fprintf(fout,"%s","      echo -n \"Reload service $DAEMON\"\n");
  fprintf(fout,"%s","      killproc -HUP $DAEMON_PATH/$DAEMON\n");
  fprintf(fout,"%s","      rc_status -v\n");
  fprintf(fout,"%s","      ;;\n");
  fprintf(fout,"%s","    reload)\n");
  fprintf(fout,"%s","      echo -n \"Reload service $DAEMON\"\n");
  fprintf(fout,"%s","      killproc -HUP $DAEMON_PATH/$DAEMON\n");
  fprintf(fout,"%s","      rc_status -v\n");
  fprintf(fout,"%s","      ;;\n");
  fprintf(fout,"%s","    status)\n");
  fprintf(fout,"%s","      echo -n \"Checking for service $DAEMON\"\n");
  fprintf(fout,"%s","      checkproc $DAEMON_PATH/$DAEMON\n");
  fprintf(fout,"%s","      rc_status -v\n");
  fprintf(fout,"%s","      ;;\n");
  fprintf(fout,"%s","    *)\n");
  fprintf(fout,"%s","    echo \"Usage: $0 {start|stop|status|try-restart|restart|force-reload|reload}\"\n");
  fprintf(fout,"%s","    exit 1\n");
  fprintf(fout,"%s","    ;;\n");
  fprintf(fout,"%s","esac\n");
  fprintf(fout,"%s","rc_exit\n");

  fclose(fout);
  mysystem("chmod ugoa+x startscript");
 
  sprintf(buf,"%s.service", name); // generate pvs.service for systemd
  fout = fopen(buf,"w");
  if(fout == NULL) return -1;
  fprintf(fout,"%s","# This file may be used to start this pvserver using systemd\n");
  fprintf(fout,"%s","# Please copy this file to \"/usr/lib/systemd/system/\"\n");
  fprintf(fout,"%s","# su \n");
  fprintf(fout,     "# cp %s.service /usr/lib/systemd/system/\n", name);
  fprintf(fout,"%s","# exit\n");
  fprintf(fout,"%s","#\n");
  fprintf(fout,"%s","# Commands:\n");
  fprintf(fout,     "# systemctl start      %s\n", name);
  fprintf(fout,     "# systemctl stop       %s\n", name);
  fprintf(fout,     "# systemctl status     %s\n", name);
  fprintf(fout,     "# systemctl is-enabled %s\n", name);
  fprintf(fout,     "# systemctl enable     /usr/lib/systemd/system/%s.service\n", name);
  fprintf(fout,     "# systemctl disable    %s\n", name);
  fprintf(fout,     "# systemctl help       %s\n", name);
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","[Unit]\n");
  fprintf(fout,"%s","Description=this is a pvserver which is used by a pvbrowser client\n");
  fprintf(fout,"%s","After=syslog.target network.target\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","[Service]\n");
  fprintf(fout,"%s","Type=simple\n");
  fprintf(fout,"%s","User=root\n");
  fprintf(fout,"%s","Group=users\n");
  fprintf(fout,"%s","StandardOutput=null\n");
  fprintf(fout,"%s","StandardError=null\n");
  fprintf(fout,     "ExecStart=%s/%s -cd=%s -port=5050\n", dir, name, dir);
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","[Install]\n");
  fprintf(fout,"%s","WantedBy=multi-user.target\n");                                                                                           
  fclose(fout);

  return 0;
}

#define MAX_IVAL 20

static int export_widgets(FILE *fin, FILE *fout, char *line, int maxline, const char *space, QStringList *traceback, int isTabOrTool)
{
#ifdef PVWIN32
  // supid windows
  char *cptr, *cptrtemp, *p[20], pnull[2], myline[1024], lastConstructor[1024], subspace[1024], ourParentName[1024];
#else
  char *cptr, *cptrtemp, *p[20], pnull[2], myline[maxline+1], lastConstructor[maxline+1], subspace[maxline], ourParentName[maxline];
#endif
  int i;
  int isConstructor, widgetTerminated, loopCount;

  pnull[0] = lastConstructor[0] = '\0';
  strcpy(subspace, space); strcat(subspace," "); // indentation
  tabstopFound = widgetTerminated = loopCount = 0;

  // get ourParentName begin
  cptr = strstr(line,"(p,");
  if(cptr == NULL) return -1;
  cptr++; cptr++; cptr++;
  cptr = strchr(cptr,',');
  if(cptr == NULL) return -1;
  cptr++;
  strcpy(ourParentName, cptr);
  cptr = strchr(ourParentName,',');
  if(cptr == NULL) cptr = strchr(ourParentName,')');
  if(cptr == NULL) return -1;
  *cptr = '\0';
  // get ourParentName end
  traceback->append(ourParentName);

  isConstructor = widgetTerminated = 1;
  while(1) // loop over generated area
  {
    // line now starts with "pv" or "q"   
    // get args begin
get_args:    
    loopCount++;
    strcpy(myline,line);
    for(i=0; i<20; i++) p[i] = &pnull[0];
    cptr = strchr(myline, ',');
    if(cptr != NULL) cptr++;
    int isText = 0;
    for(i=0; i<20 && cptr!=NULL; i++)
    {
      p[i] = cptr;
      cptrtemp = cptr; cptr = NULL;
      if(isText == 0) 
      {
        cptr = strchr(cptrtemp, ',');
        if(cptr == NULL) cptr = strchr(cptrtemp, ')');
        if(cptr != NULL)
        {
          *cptr = '\0';
          cptr++;
        }  
      }  
      else if(isText == 1)
      {
        if(strchr(cptrtemp,'\"') == NULL) cptr = NULL;
        else                              cptr = strrchr(cptrtemp,'\"');
        if(cptr != NULL) 
        {
          *cptr = '\0';
          cptr++;
          if(*cptr == ',') cptr++;
        }  
        isText = 0;
      }  
      if(cptr == NULL) break;
      if(strncmp(cptr,"pvtr(\"",6) == 0)
      {
        isText = 1;
        cptr++; cptr++; cptr++; cptr++; cptr++; cptr++;
      }
      else if(*cptr == '\"') 
      {
        isText = 1;
        cptr++;
      }
      if(opt_develop.script == PV_LUA)
      {
        if(*cptr == ')') break; 
      }
      else
      {
        if(*cptr == ';') break; 
      }
    }
    // get args end

    if(opt_develop.arg_debug) // print args
    {
      printf("export_widgets args for: %s isConstructor=%d\n", line, isConstructor);
      for(i=0; i<20; i++) // print args
      {
        cptr = p[i];
        if(*cptr == '\0') break;
        printf("p[%d]=%s\n", i, cptr);
      }
    }

    //--- begin interpret ---------------------------------------------------------------
    if(opt_develop.script == PV_LUA) cptr = strstr(line, "pv.pvTabOrder(p,");
    else                     cptr = strstr(line, "pvTabOrder(p,");
    if(cptr != NULL)
    { 
      isConstructor = 0;
      if(tabstopFound == 0)
      {
        tabstopFound = 1;
        tablist.append(QString(p[0]));
      }
      tablist.append(QString(p[1]));
    }
    if(isConstructor == 1)
    { 
      strcpy(lastConstructor, line);
      // check recursive begin
      if(strcmp(ourParentName,p[1]) == 0) widgetTerminated = 0;        // we are on same level
      else                                                             // we are not on same level
      {
         QString item;
         for(int i=0; i<traceback->size(); i++)                        // test if p[1] is in traceback
         {
           item = traceback->at(i);
           if(item == p[1]) 
           {
             traceback->removeLast();
             return 0;
           }  
         }
         export_widgets(fin, fout, line, maxline, subspace, traceback, isTabOrTool); // recursive export children
         fprintf(fout,"%s </widget>\n", space); 
         widgetTerminated = 0;
         if(strstr(line,"  return 0;") != NULL)                          return 0;               // finito
         if(strstr(line,"--- end construction of our mask ---") != NULL) return 0;               // finito
         goto get_args;                                                 // goto start of main loop
      } 
      // check recursive end
      if(widgetTerminated == 0 && loopCount > 1) fprintf(fout,"%s</widget>\n", space); 
      widgetTerminated = 1;

      // interpret constructors begin

      // Qt Widgets
      if(strstr(line,"pvQPushButton(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QPushButton\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQLabel(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QLabel\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQLineEdit(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QLineEdit\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQComboBox(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QComboBox\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"editable\" >\n", space);     // editable
        if(strcmp(p[2],"1")) fprintf(fout,"%s  <bool>true</bool>\n", space);
        else                 fprintf(fout,"%s  <bool>false</bool>\n", space);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"insertPolicy\" >\n", space); // insertPolicy
        if     (strcmp("NoInsertion" ,p[3]) == 0) fprintf(fout,"%s  <enum>QComboBox::NoInsert</enum>\n", space);
        else if(strcmp("AtTop"       ,p[3]) == 0) fprintf(fout,"%s  <enum>QComboBox::InsertAtTop</enum>\n", space);
        else if(strcmp("AtCurrent"   ,p[3]) == 0) fprintf(fout,"%s  <enum>QComboBox::InsertAtCurrent</enum>\n", space);
        else if(strcmp("AtBottom"    ,p[3]) == 0) fprintf(fout,"%s  <enum>QComboBox::InsertAtBottom</enum>\n", space);
        else if(strcmp("AfterCurrent",p[3]) == 0) fprintf(fout,"%s  <enum>QComboBox::InsertAfterCurrent</enum>\n", space);
        else                                      fprintf(fout,"%s  <enum>QComboBox::InsertBeforeCurrent</enum>\n", space);
        fprintf(fout,"%s </property>\n", space);     
      }
      else if(strstr(line,"pvQLCDNumber(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QLCDNumber\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"numDigits\" >\n", space);     // numDigits
        fprintf(fout,"%s  <number>%s</number>\n", space, p[2]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"mode\" >\n", space);          // mode
        if     (strcmp("Hex" ,p[3]) == 0) fprintf(fout,"%s  <enum>QLCDNumber::Hex</enum>\n", space);
        else if(strcmp("Oct" ,p[3]) == 0) fprintf(fout,"%s  <enum>QLCDNumber::Oct</enum>\n", space);
        else if(strcmp("Bin" ,p[3]) == 0) fprintf(fout,"%s  <enum>QLCDNumber::Bin</enum>\n", space);
        else                              fprintf(fout,"%s  <enum>QLCDNumber::Dec</enum>\n", space);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"segmentStyle\" >\n", space);  // segmentStyle
        if     (strcmp("Flat"   ,p[4]) == 0) fprintf(fout,"%s  <enum>QLCDNumber::Flat</enum>\n", space);
        else if(strcmp("Filled" ,p[4]) == 0) fprintf(fout,"%s  <enum>QLCDNumber::Filled</enum>\n", space);
        else                                 fprintf(fout,"%s  <enum>QLCDNumber::Outline</enum>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvQRadioButton(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QRadioButton\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQCheckBox(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QCheckBox\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQSlider(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QSlider\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"minimum\" >\n", space);           // minimum
        fprintf(fout,"%s  <number>%s</number>\n", space, p[2]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"maximum\" >\n", space);           // maximum
        fprintf(fout,"%s  <number>%s</number>\n", space, p[3]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"pageStep\" >\n", space);          // pageStep
        fprintf(fout,"%s  <number>%s</number>\n", space, p[4]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"value\" >\n", space);             // value
        fprintf(fout,"%s  <number>%s</number>\n", space, p[5]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"orientation\" >\n",       space); // orientation
        if     (strncmp("v" ,p[6], 1) == 0) fprintf(fout,"%s  <enum>Qt::Vertical</enum>\n", space);
        else if(strncmp("V" ,p[6], 1) == 0) fprintf(fout,"%s  <enum>Qt::Vertical</enum>\n", space);
        else                                fprintf(fout,"%s  <enum>Qt::Horizontal</enum>\n", space);
        fprintf(fout,"%s </property>\n",                            space);
      }
      else if(strstr(line,"pvQTable(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QTableWidget\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"rowCount\" >\n", space);          // rowCount
        fprintf(fout,"%s  <number>%s</number>\n", space, p[2]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"columnCount\" >\n", space);       // columnCount
        fprintf(fout,"%s  <number>%s</number>\n", space, p[3]);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvQTabWidget(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QTabWidget\" name=\"%s\" >\n", space, p[0]);
        isTabOrTool = 1;
      }
      else if(strstr(line,"pvQToolBox(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QToolBox\" name=\"%s\" >\n", space, p[0]);
        isTabOrTool = 2;
      }
      else if(strstr(line,"pvQWidget(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QWidget\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQGroupBox(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QGroupBox\" name=\"%s\" >\n", space, p[0]);
        // p[2] and p[3] not used, because of troll porting murx
        if(strlen(p[4]) > 0)                                           // if there is a title
        {
          fprintf(fout,"%s <property name=\"title\" >\n", space);
          fprintf(fout,"%s  <string>%s</string>\n",       space,  p[4]);
          fprintf(fout,"%s </property>\n",                space);
        }
      }
      else if(strstr(line,"pvQButtonGroup(") != NULL)                  // exchange it by QGroupBox because Qt4 does not support it
      {
        fprintf(fout,"%s<widget class=\"QGroupBox\" name=\"%s\" >\n", space, p[0]);
        // p[2] and p[3] not used, because of troll porting murx
        if(strlen(p[4]) > 0)                                           // if there is a title
        {
          fprintf(fout,"%s <property name=\"title\" >\n", space);
          fprintf(fout,"%s  <string>%s</string>\n",       space,  p[4]);
          fprintf(fout,"%s </property>\n",                space);
        }
      }
      else if(strstr(line,"pvQListBox(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QListWidget\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQListView(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QTreeWidget\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQSpinBox(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QSpinBox\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQDial(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QDial\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"minimum\" >\n", space);           // minimum
        fprintf(fout,"%s  <number>%s</number>\n", space, p[2]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"maximum\" >\n", space);           // maximum
        fprintf(fout,"%s  <number>%s</number>\n", space, p[3]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"pageStep\" >\n", space);          // pageStep
        fprintf(fout,"%s  <number>%s</number>\n", space, p[4]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"value\" >\n", space);             // value
        fprintf(fout,"%s  <number>%s</number>\n", space, p[5]);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvQProgressBar(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QProgressBar\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"maximum\" >\n", space);           // maximum, total_steps
        fprintf(fout,"%s  <number>%s</number>\n", space, p[2]);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"orientation\" >\n",       space); // orientation
        if     (strncmp("v" ,p[3], 1) == 0) fprintf(fout,"%s  <enum>Qt::Vertical</enum>\n", space);
        else if(strncmp("V" ,p[3], 1) == 0) fprintf(fout,"%s  <enum>Qt::Vertical</enum>\n", space);
        else                                fprintf(fout,"%s  <enum>Qt::Horizontal</enum>\n", space);
        fprintf(fout,"%s </property>\n",                            space);
      }
      else if(strstr(line,"pvQTextBrowser(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QTextBrowser\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQMultiLineEdit(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QTextEdit\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"readOnly\" >\n", space);          // editable
        if(strcmp("0",p[2]) == 0) fprintf(fout,"%s  <bool>true</bool>\n", space);
        else                      fprintf(fout,"%s  <bool>false</bool>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvQDateEdit(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QDateEdit\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQDateTimeEdit(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QDateTimeEdit\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQTimeEdit(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QTimeEdit\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQFrame(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QFrame\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"frameShape\" >\n", space);
        if     (strcmp("Panel"      ,p[2]) == 0) fprintf(fout,"%s  <enum>QFrame::Panel</enum>\n",       space);
        else if(strcmp("WinPanel"   ,p[2]) == 0) fprintf(fout,"%s  <enum>QFrame::WinPanel</enum>\n",    space);
        else if(strcmp("HLine"      ,p[2]) == 0) fprintf(fout,"%s  <enum>QFrame::Hline</enum>\n",       space);
        else if(strcmp("VLine"      ,p[2]) == 0) fprintf(fout,"%s  <enum>QFrame::VLine</enum>\n",       space);
        else if(strcmp("StyledPanel",p[2]) == 0) fprintf(fout,"%s  <enum>QFrame::StyledPanel</enum>\n", space);
        else                                     fprintf(fout,"%s  <enum>QFrame::Box</enum>\n",         space);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"frameShadow\" >\n", space);
        if     (strcmp("Sunken"  ,p[3]) == 0) fprintf(fout,"%s  <enum>QFrame::Sunken</enum>\n", space);
        else if(strcmp("Raised"  ,p[3]) == 0) fprintf(fout,"%s  <enum>QFrame::Raised</enum>\n", space);
        else                                  fprintf(fout,"%s  <enum>QFrame::Plain</enum>\n",  space);
        fprintf(fout,"%s </property>\n", space);
        fprintf(fout,"%s <property name=\"lineWidth\" >\n", space);
        fprintf(fout,"%s  <number>%s</number>\n", space, p[4]);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvQLayoutVbox(") != NULL)
      {
        // Layout is not imported/exported -> use internal designer
      }
      else if(strstr(line,"pvQLayoutHbox(") != NULL)
      {
        // Layout is not imported/exported -> use internal designer
      }
      else if(strstr(line,"pvQLayoutGrid(") != NULL)
      {
        // Layout is not imported/exported -> use internal designer
      }
      else if(strstr(line,"pvAddWidgetOrLayout(") != NULL)
      {
        // Layout is not imported/exported -> use internal designer
      }

      // pvbrowser widgets
      else if(strstr(line,"pvQIconView(") != NULL) // is here because Qt4 no longer supports it
      {
        fprintf(fout,"%s<widget class=\"PvbIconview\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQDraw(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"PvbDraw\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQImage(") != NULL || strstr(line,"pvQImageScript(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"PvbImage\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"whatsThis\" >\n", space);
        fprintf(fout,"%s  <string>%s</string>\n", space, p[2]);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvQVtkTclWidget(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"PvbVtk\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQGL(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"PvbOpengl\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQCustomWidget(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"PvbCustomWidget\" name=\"%s\" >\n", space, p[0]);
        fprintf(fout,"%s <property name=\"whatsThis\" >\n", space);
        fprintf(fout,"%s  <string>%s</string>\n", space, p[2]);
        fprintf(fout,"%s </property>\n", space);
      }

      // Qwt widgets
      else if(strstr(line,"pvQwtPlotWidget(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtPlot\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtThermo(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtThermo\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtKnob(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtKnob\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtCounter(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtCounter\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtWheel(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtWheel\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtSlider(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtSlider\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtCompass(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtCompass\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtDial(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtDial\" name=\"%s\" >\n", space, p[0]);
      }
      else if(strstr(line,"pvQwtAnalogClock(") != NULL)
      {
        fprintf(fout,"%s<widget class=\"QwtAnalogClock\" name=\"%s\" >\n", space, p[0]);
      }
      else
      {
        fprintf(fout,"ERROR: unknown line=%s\n", line);
      }

      // interpret constructors end
    }
    else if(isConstructor == 0)
    {
      // interpret non constructors begin

      // Qt properties
      if     (strstr(line,"pvSetGeometry(") != NULL)
      {
        fprintf(fout,"%s <property name=\"geometry\" >\n", space);
        fprintf(fout,"%s  <rect>\n",                       space);
        fprintf(fout,"%s   <x>%s</x>\n",                   space,  p[1]);
        fprintf(fout,"%s   <y>%s</y>\n",                   space,  p[2]);
        fprintf(fout,"%s   <width>%s</width>\n",           space,  p[3]);
        fprintf(fout,"%s   <height>%s</height>\n",         space,  p[4]);
        fprintf(fout,"%s  </rect>\n",                      space);
        fprintf(fout,"%s </property>\n",                   space);
      }
      else if(strstr(line,"pvSetTabPosition(") != NULL)
      {
        fprintf(fout,"%s <property name=\"tabPosition\" >\n", space);
        if     (strcmp("Bottom",p[1]) == 0) fprintf(fout,"%s  <enum>QTabWidget::South</enum>\n", space);
        else if(strcmp("1"     ,p[1]) == 0) fprintf(fout,"%s  <enum>QTabWidget::South</enum>\n", space);
        else if(strcmp("0"     ,p[1]) == 0) fprintf(fout,"%s  <enum>QTabWidget::North</enum>\n", space);
        else if(strcmp("2"     ,p[1]) == 0) fprintf(fout,"%s  <enum>QTabWidget::West</enum>\n",  space);
        else if(strcmp("3"     ,p[1]) == 0) fprintf(fout,"%s  <enum>QTabWidget::East</enum>\n",  space);
        else                                fprintf(fout,"%s  <enum>QTabWidget::North</enum>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetEditable(") != NULL)
      {
        fprintf(fout,"%s <property name=\"readOnly\" >\n", space);
        if(strcmp("1",p[1]) == 0) fprintf(fout,"%s  <bool>false</bool>\n", space);
        else                      fprintf(fout,"%s  <bool>true</bool>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetPaletteBackgroundColor(") != NULL)
      {
        // Not supported by Qt4 Designer
      }
      else if(strstr(line,"pvSetPaletteForegroundColor(") != NULL)
      {
        // Not supported by Qt4 Designer
      }
      else if(strstr(line,"pvSetText(") != NULL)
      {
        if(strstr(lastConstructor,"pvQGroupBox(") != NULL) fprintf(fout,"%s <property name=\"title\" >\n", space);
        else                                               fprintf(fout,"%s <property name=\"text\" >\n",  space);
        //fprintf(fout,"%s  <string>%s</string>\n",          space,  p[1]);
        char pbuf[1024],cbuf[2];
        const char *ptr = p[1];
        int ilen = 0;
        pbuf[0] = '\0';
        while(*ptr != '\0')
        {
          if(*ptr == '&') 
          {
            strcat(pbuf, "&amp;");
          }
          else            
          {
            cbuf[0] = *ptr; cbuf[1] = '\0';
            strcat(pbuf, cbuf);
          }
          ptr++; ilen++;
          if(ilen > 1000) break;
        }
        fprintf(fout,"%s  <string>%s</string>\n",          space, pbuf);
        fprintf(fout,"%s </property>\n",                   space);
      }
      else if(strstr(line,"pvSetChecked(") != NULL)
      {
        fprintf(fout,"%s <property name=\"checked\" >\n", space);
        if(strcmp("1",p[1]) == 0) fprintf(fout,"%s  <bool>true</bool>\n", space);
        else                      fprintf(fout,"%s  <bool>false</bool>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetEchoMode(") != NULL)
      {
        fprintf(fout,"%s <property name=\"echoMode\" >\n", space);
        if     (strcmp("2",p[1]) == 0) fprintf(fout,"%s  <enum>QLineEdit::Password</enum>\n", space);
        else if(strcmp("0",p[1]) == 0) fprintf(fout,"%s  <enum>QLineEdit::NoEcho</enum>\n", space);
        else                           fprintf(fout,"%s  <enum>QLineEdit::Normal</enum>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetMultiSelection(") != NULL)
      {
        fprintf(fout,"%s <property name=\"selectionMode\" >\n", space);
        if     (strcmp("2",p[1]) == 0) fprintf(fout,"%s  <enum>QAbstractItemView::NoSelection</enum>\n", space);
        else if(strcmp("1",p[1]) == 0) fprintf(fout,"%s  <enum>QAbstractItemView::MultiSelection</enum>\n", space);
        else                           fprintf(fout,"%s  <enum>QAbstractItemView::SingleSelection</enum>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetFont(") != NULL)
      {
        fprintf(fout,"%s <property name=\"font\" >\n", space);
        fprintf(fout,"%s  <font>\n", space);
        fprintf(fout,"%s   <family>%s</family>\n", space, p[1]);
        fprintf(fout,"%s   <pointsize>%s</pointsize>\n", space, p[2]);
        //fprintf(fout,"%s   <weight>75</weight>\n", space);
        if(strcmp("1",p[3]) == 0) fprintf(fout,"%s   <bold>true</bold>\n", space);
        else                      fprintf(fout,"%s   <bold>false</bold>\n", space);
        if(strcmp("1",p[4]) == 0) fprintf(fout,"%s   <italic>true</italic>\n", space);
        else                      fprintf(fout,"%s   <italic>false</italic>\n", space);
        if(strcmp("1",p[5]) == 0) fprintf(fout,"%s   <underline>true</underline>\n", space);
        else                      fprintf(fout,"%s   <underline>false</underline>\n", space);
        if(strcmp("1",p[6]) == 0) fprintf(fout,"%s   <strikeout>true</strikeout>\n", space);
        else                      fprintf(fout,"%s   <strikeout>false</strikeout>\n", space);
        fprintf(fout,"%s  </font>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetStyle(") != NULL)
      {
        // Not supported by Qt4 Designer
      }
      else if(strstr(line,"pvToolTip(") != NULL)
      {
        fprintf(fout,"%s <property name=\"toolTip\" >\n", space);
        fprintf(fout,"%s  <string>%s</string>\n", space, p[1]);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvAddTab(") != NULL)
      {
        if(isTabOrTool == 1) // TabWidget
        {
          fprintf(fout,"%s <attribute name=\"title\" >\n",   space);
          fprintf(fout,"%s  <string>%s</string>\n",          space,  p[2]);
          fprintf(fout,"%s </attribute>\n",                  space);
        }
        else if(isTabOrTool == 2) // ToolBox
        {
          fprintf(fout,"%s <attribute name=\"label\" >\n",   space);
          fprintf(fout,"%s  <string>%s</string>\n",          space,  p[2]);
          fprintf(fout,"%s </attribute>\n",                  space);
        }
        else
        {
          printf("isTabOrTool=%d\n", isTabOrTool);
          printf("ERROR: line %s not allowed without TabWidget or ToolBox\n", line);
        }
      }
      else if(strstr(line,"pvSetMinSize(") != NULL)
      {
        fprintf(fout,"%s <property name=\"minimumSize\" >\n", space);
        fprintf(fout,"%s  <size>\n", space);
        fprintf(fout,"%s   <width>%s</width>\n", space, p[1]);
        fprintf(fout,"%s   <height>%s</height>\n", space, p[2]);
        fprintf(fout,"%s  </size>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetMaxSize(") != NULL)
      {
        fprintf(fout,"%s <property name=\"maximumSize\" >\n", space);
        fprintf(fout,"%s  <size>\n", space);
        fprintf(fout,"%s   <width>%s</width>\n", space, p[1]);
        fprintf(fout,"%s   <height>%s</height>\n", space, p[2]);
        fprintf(fout,"%s  </size>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }
      else if(strstr(line,"pvSetAlignment(") != NULL)
      {
        QString align;
        int flags = 0;
        if(strstr(line,"AlignLeft") != NULL)    
        { 
          flags++; align += "Qt::AlignLeft";    
        }
        if(strstr(line,"AlignRight") != NULL)  
        { 
          if(flags) align += "|";
          flags++; align += "Qt::AlignRight";   
        }
        if(strstr(line,"AlignHCenter") != NULL) 
        { 
          if(flags) align += "|";
          flags++; align += "Qt::AlignHCenter"; 
        }
        if(strstr(line,"AlignJustify") != NULL) 
        { 
          if(flags) align += "|";
          flags++; align += "Qt::AlignJustify"; 
        }
        if(strstr(line,"AlignTop") != NULL)    
        { 
          if(flags) align += "|";
          flags++; align += "Qt::AlignTop";     
        }
        if(strstr(line,"AlignBottom") != NULL)  
        { 
          if(flags) align += "|";
          flags++; align += "Qt::AlignBottom";  
        }
        if(strstr(line,"AlignVCenter") != NULL) 
        { 
          if(flags) align += "|";
          flags++; align += "Qt::AlignVCenter"; 
        }
        fprintf(fout,"%s <property name=\"alignment\" >\n", space);
        fprintf(fout,"%s  <set>%s</set>\n", space, (const char *) align.toUtf8());
        fprintf(fout,"%s </property>\n", space);
      }
      // Qwt properties. Currently these properties are ignored and set to default values. Init them in slotInit().
      else if(strstr(line,"qpwSetCanvasBackground(") != NULL)
      {
      }
      else if(strstr(line,"qwtEnableAxis(") != NULL)
      {
      }
      else if(strstr(line,"qwtScaleSetPosition(") != NULL)
      {
      }
      else if(strstr(line,"qwtScaleSetTitleColor(") != NULL)
      {
      }
      else if(strstr(line,"qwtKnobSetBorderWidth(") != NULL)
      {
      }
      else if(strstr(line,"qwtKnobSetTotalAngle(") != NULL)
      {
      }
      else if(strstr(line,"qwtKnobSetSymbol(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetNumButtons(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetStep(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetMinValue(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetMaxValue(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetStepButton1(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetStepButton2(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetStepButton3(") != NULL)
      {
      }
      else if(strstr(line,"qwtCounterSetValue(") != NULL)
      {
      }
      else if(strstr(line,"qwtWheelSetMass(") != NULL)
      {
      }
      else if(strstr(line,"qwtWheelSetTotalAngle(") != NULL)
      {
      }
      else if(strstr(line,"qwtWheelSetViewAngle(") != NULL)
      {
      }
      else if(strstr(line,"qwtWheelSetTickCnt(") != NULL)
      {
      }
      else if(strstr(line,"qwtWheelSetInternalBorder(") != NULL)
      {
      }
      else if(strstr(line,"qwtSliderSetThumbLength(") != NULL)
      {
      }
      else if(strstr(line,"qwtSliderSetThumbWidth(") != NULL)
      {
      }
      else if(strstr(line,"qwtSliderSetBorderWidth(") != NULL)
      {
      }
      else if(strstr(line,"qwtSliderSetBgStyle(") != NULL)
      {
      }
      else if(strstr(line,"qwtSliderSetScalePos(") != NULL)
      {
      }
      else if(strstr(line,"qwtSliderSetValue(") != NULL)
      {
      }
      // interpret non constructors end
    }
    //--- end interpret -----------------------------------------------------------------

    // get next line of information begin 
    isConstructor = 0;
    char *fret;
    while((fret =fgets(line, maxline, fin)) != NULL)
    {
      cptr = strchr(line,'\n');
      if(cptr != NULL) *cptr = '\0';
      if(opt_develop.script == PV_LUA)
      {
        if(strstr(line,"--- end construction of our mask ---") != NULL)
        {
          return 0; // end reached
        }  
      }
      else
      {
        if(strstr(line,"  return 0;") != NULL)
        {
          return 0; // end reached
        }  
      }
      if     (strstr(line,"pvDownloadFile(")     != NULL) ;                  // not necessary in ui file
      else if(strstr(line,"pvGl")                != NULL) ;                  // not necessary in ui file
      else if(strstr(line,"pvQLayout")           != NULL) ;                  // not necessary in ui file
      else if(strstr(line,"pvAddWidgetOrLayout") != NULL) ;                  // not necessary in ui file
      else if(strstr(line,"pvAddStretch")        != NULL) ;                  // not necessary in ui file
      else if(strstr(line,"initializeGL")        != NULL) ;                  // not necessary in ui file
      else if(strstr(line,"pvEndDefinition(")    != NULL) fprintf(fout,"%s</widget>\n", space); 
      else if(strstr(line,"pv")                  != NULL) break;             // information found
      else if(strstr(line,"qpw")                 != NULL) break;             // information found
      else if(strstr(line,"qwt")                 != NULL) break;             // information found
      else if(strstr(line,"pvSetFont(")          != NULL)
      {
        // pvSetFont(p,welcomeLabel,"Sans Serif",18,1,1,0,0);
        fprintf(fout,"%s <property name=\"font\" >\n", space);
        fprintf(fout,"%s  <font>\n", space);
        fprintf(fout,"%s   <family>%s</family>\n",        space, p[1]);
        fprintf(fout,"%s    <pointsize>%s</pointsize>\n", space, p[2]);
        fprintf(fout,"%s    <weight>%s</weight>\n",       space, p[3]);
        fprintf(fout,"%s    <italic>%s</italic>\n",       space, p[4]);
        fprintf(fout,"%s    <bold>%s</bold>\n",           space, p[5]);
        fprintf(fout,"%s    <underline>%s</underline>\n", space, p[6]);
        fprintf(fout,"%s    <strikeout>%s</strikeout>\n", space, p[7]);
        fprintf(fout,"%s  </font>\n", space);
        fprintf(fout,"%s </property>\n", space);
      }  
      else                                             isConstructor = 1; // empty line found
    }
    if(fret == NULL) return -1;
    // get next line of information end 
  }
}

int export_ui(int imask)
{
  FILE *fin, *fout;
  char *cptr, line[4096];
  QStringList traceback;
  int isTabOrTool = 0;

  traceback.clear();
  if(opt_develop.script == PV_LUA) sprintf(line, "mask%d.lua", imask);
  else                     sprintf(line, "mask%d.cpp", imask);
  fin = fopen(line,"r");
  if(fin == NULL)
  {
    printf("export_ui: could not open %s\n", line);
    return -1;
  }
  sprintf(line, "mask%d.ui", imask);
  fout = fopen(line,"w");
  if(fout == NULL)
  {
    printf("export_ui: could not write %s\n", line);
    fclose(fin);
    return -1;
  }

  fprintf(fout, "<ui version=\"4.0\" >\n");
  fprintf(fout, " <class>Form</class>\n");
  fprintf(fout, " <widget class=\"QWidget\" name=\"Form\" >\n");
  fprintf(fout, "  <property name=\"geometry\" >\n");
  fprintf(fout, "   <rect>\n");
  fprintf(fout, "    <x>0</x>\n");
  fprintf(fout, "    <y>0</y>\n");
  fprintf(fout, "    <width>%d</width>\n", opt_develop.xmax);
  fprintf(fout, "    <height>%d</height>\n", opt_develop.ymax);
  fprintf(fout, "   </rect>\n");
  fprintf(fout, "  </property>\n");
  fprintf(fout, "  <property name=\"windowTitle\" >\n");
  fprintf(fout, "   <string>Form</string>\n");
  fprintf(fout, "  </property>\n");

  // find start
  while(fgets(line, sizeof(line)-1, fin) != NULL)
  {
    if(strstr(line,"pvStartDefinition(p,") != NULL) goto start1;
  }
  fclose(fin);
  fclose(fout);
  return -1;

start1:
  while(fgets(line, sizeof(line)-1, fin) != NULL)
  {
    if(strstr(line,"pvDownloadFile(")  != NULL) ; // not necessary in ui file
    else if(strstr(line,"pv") != NULL) goto start2;
    else if(strstr(line,"q")  != NULL) goto start2;
  }
  fclose(fin);
  fclose(fout);
  return -1;

start2:  
  cptr = strchr(line,'\n');
  if(cptr != NULL) *cptr = '\0';
  tablist.clear();
  export_widgets(fin, fout, line, sizeof(line)-1, "  ", &traceback, isTabOrTool);

  fprintf(fout, " </widget>\n");
  if(tabstopFound)
  {
    fprintf(fout, " <tabstops>\n");
    for(int i=0; i<tablist.size(); i++)
    {
      QString item = tablist.at(i);
      fprintf(fout,"  <tabstop>%s</tabstop>\n",(const char *) item.toUtf8().data());
    }
    fprintf(fout, " </tabstops>\n");
    tablist.clear();
    tabstopFound = 0;
  }
  fprintf(fout, " <resources/>\n");
  fprintf(fout, " <connections/>\n");
  fprintf(fout, "</ui>\n");
  fclose(fin);
  fclose(fout);
  return 0;
}

void lua_addMaskToMain(int imask)
{
  FILE *fout;
  FileLines *fl;
  int while_found, else_found, done, dofile_found;

  if(opt_develop.arg_debug) printf("lua_addMaskToMain imask=%d\n",imask);
  if(loadFile("main.lua") == -1) return;
  while_found = else_found = done = dofile_found = 0;
  fout = fopen("main.lua","w");
  if(fout == NULL) return;
  fl = &file_lines;
  while(fl->next != NULL)
  {
    fl = fl->next;
    if(dofile_found==0 && strstr(fl->line,"dofile(\"mask") != NULL)
    {
      fprintf(fout,"dofile(\"mask%d.lua\")    -- include your masks here\n",imask);
      dofile_found = 1;
    }
    else if(while_found==0 && 
      strstr(fl->line,"while 1 do") != NULL &&
      strstr(fl->line,"show your masks") != NULL )
    {
      while_found = 1;
    }
    if(while_found==1 && else_found==0 && 
      strstr(fl->line,"if") == NULL &&
      strstr(fl->line,"else") != NULL)
    {
      else_found = 1;
    }
    if(else_found==1 && done==0)
    {
      fprintf(fout,"    elseif (ret==%d) then\n",imask);
      fprintf(fout,"      ret = showMask%d(p);\n",imask);
      fprintf(fout,"%s",fl->line);
      done = 1;
    }
    else
    {
      fprintf(fout,"%s",fl->line);
    }
  }
  fclose(fout);
  unloadFile();
}

void lua_generateInitialMask(int imask)
{
  char fname[1024];
  FILE *fout;
  
  sprintf(fname,"mask%d.lua", imask);
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"%s","--------------------------------------------------------------------------------------\n");
  fprintf(fout,"%s","-- this file is generated by pvdevelop. DO NOT EDIT !!!\n");
  fprintf(fout,"%s","--------------------------------------------------------------------------------------\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,     "function showMask%d(p)\n", imask);
  fprintf(fout,"%s","  --- begin variables that are private to this mask ----------------------------------\n");
  fprintf(fout,"%s","  iarray = pv.IntegerArray()                  -- see pv.getIntegers(text,iarray) below\n");
  fprintf(fout,"%s","  farray = pv.FloatArray()                    -- see pv.getFloats(text,farray) below\n");
  fprintf(fout,"%s","  --- begin construction of our mask -------------------------------------------------\n");
  fprintf(fout,"%s","  ID_MAIN_WIDGET = 0                          -- begin of our widget names\n");
  fprintf(fout,"%s","  PushButtonBack = 1\n");
  fprintf(fout,"%s","  ID_END_OF_WIDGETS = 2                       -- end of our widget names\n");
  fprintf(fout,"%s","  pv.pvStartDefinition(p,ID_END_OF_WIDGETS)\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  pv.pvQPushButton(p,PushButtonBack,0)\n");
  fprintf(fout,"%s","  pv.pvSetGeometry(p,PushButtonBack,10,10,111,40)\n");
  fprintf(fout,"%s","  pv.pvSetText(p,PushButtonBack,\"Lua test\")\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  pv.pvEndDefinition(p)\n");
  fprintf(fout,"%s","  --- end construction of our mask ---------------------------------------------------\n");
  fprintf(fout,"%s","  --- end variables that are private to this mask ------------------------------------\n");
  fprintf(fout,     "  dofile(\"mask%d_slots.lua\")                   -- include our slot functions\n", imask);
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  if trace == 1 then print(\"show mask1\") end\n");
  fprintf(fout,"%s","  pv.pvClearMessageQueue(p)                   -- clear all pending events\n");
  fprintf(fout,"%s","  ret = slotInit(p)                           -- intitialize our variables\n");
  fprintf(fout,"%s","  if ret ~= 0 then return ret end             -- return number of next mask to call\n");
  fprintf(fout,"%s","  while(1)                                    -- event loop\n");
  fprintf(fout,"%s","  do\n");
  fprintf(fout,"%s","    event  = pv.pvGetEvent(p)                 -- get the next event\n");
  fprintf(fout,"%s","    result = pv.pvParseEventStruct(p,event)   -- parse the event\n");
  fprintf(fout,"%s","    id     = result.event\n");
  fprintf(fout,"%s","    i      = result.i\n");
  fprintf(fout,"%s","    text   = result.text\n");
  fprintf(fout,"%s","                                              -- now call the according slot function\n");
  fprintf(fout,"%s","    if     id == pv.NULL_EVENT then\n");
  fprintf(fout,"%s","        ret = slotNullEvent(p)\n");
  fprintf(fout,"%s","    elseif id == pv.BUTTON_EVENT then    \n");
  fprintf(fout,"%s","        if trace==1 then print(\"BUTTON_EVENT id=\", i) end\n");
  fprintf(fout,"%s","        ret = slotButtonEvent(p,i)\n");
  fprintf(fout,"%s","    elseif id == pv.BUTTON_PRESSED_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"BUTTON_PRESSED_EVENT id=\",i) end\n");
  fprintf(fout,"%s","        ret=slotButtonPressedEvent(p,i)\n");
  fprintf(fout,"%s","    elseif id == pv.BUTTON_RELEASED_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"BUTTON_RELEASED_EVENT id=\",i) end\n");
  fprintf(fout,"%s","        ret=slotButtonReleasedEvent(p,i)\n");
  fprintf(fout,"%s","    elseif id == pv.TEXT_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"TEXT_EVENT id=\",i,\" text=\",text) end\n");
  fprintf(fout,"%s","        ret=slotTextEvent(p,i,text)\n");
  fprintf(fout,"%s","    elseif id == pv.SLIDER_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"SLIDER_EVENT val=\",iarray.i0) end\n");
  fprintf(fout,"%s","        ret=slotSliderEvent(p,i,iarray.i0)\n");
  fprintf(fout,"%s","    elseif id == pv.CHECKBOX_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"CHECKBOX_EVENT id=\",i,\" text=\",text) end\n");
  fprintf(fout,"%s","        ret=slotCheckboxEvent(p,i,text)\n");
  fprintf(fout,"%s","    elseif id == pv.RADIOBUTTON_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"RADIOBUTTON_EVENT id=\",i,\" text=\",text) end\n");
  fprintf(fout,"%s","        ret=slotRadioButtonEvent(p,i,text)\n");
  fprintf(fout,"%s","    elseif id == pv.GL_INITIALIZE_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"you have to call initializeGL()\") end\n");
  fprintf(fout,"%s","        ret=slotGlInitializeEvent(p,i)\n");
  fprintf(fout,"%s","    elseif id == pv.GL_PAINT_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"you have to call paintGL()\") end\n");
  fprintf(fout,"%s","        ret=slotGlPaintEvent(p,i)\n");
  fprintf(fout,"%s","    elseif id == pv.GL_RESIZE_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"you have to call resizeGL(w,h)\") end\n");
  fprintf(fout,"%s","        ret=slotGlResizeEvent(p,i,iarray.i0,iarray.i1)\n");
  fprintf(fout,"%s","    elseif id == pv.GL_IDLE_EVENT then\n");
  fprintf(fout,"%s","        ret=slotGlIdleEvent(p,i)\n");
  fprintf(fout,"%s","    elseif id == pv.TAB_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"TAB_EVENT id=\",i,\"page=\",iarray.i0) end\n");
  fprintf(fout,"%s","        ret=slotTabEvent(p,i,iarray.i0)\n");
  fprintf(fout,"%s","    elseif id == pv.TABLE_TEXT_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        pv.pvlock(p)\n");
  fprintf(fout,"%s","        str1 = pv.getTextFromText(text)\n");
  fprintf(fout,"%s","        pv.pvunlock(p)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"TABLE_TEXT_EVENT id=\",i,\" x=\",iarray.i0,\" y=\",iarray.i1,\" text=\",str1) end\n");
  fprintf(fout,"%s","        ret=slotTableTextEvent(p,i,iarray.i0,iarray.i1,str1)\n");
  fprintf(fout,"%s","    elseif id == pv.TABLE_CLICKED_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"TABLE_CLICKED_EVENT id=\",i,\" x=\",iarray.i0,\" y=\",iarray.i1,\" button=\",iarray.i2) end\n");
  fprintf(fout,"%s","        ret=slotTableClickedEvent(p,i,iarray.i0,iarray.i1,iarray.i2)\n");
  fprintf(fout,"%s","    elseif id == pv.SELECTION_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        pv.pvlock(p)\n");
  fprintf(fout,"%s","        str1 = pv.getTextFromText(text)\n");
  fprintf(fout,"%s","        pv.pvunlock(p)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"SELECTION_EVENT id=\",i,\" column=\",iarray.i0,\" text=\",str1) end\n");
  fprintf(fout,"%s","        ret=slotSelectionEvent(p,i,iarray.i0,str1)\n");
  fprintf(fout,"%s","    elseif id == pv.CLIPBOARD_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"CLIPBOARD_EVENT id=\",iarray.i0) end\n");
  //fprintf(fout,"%s","        if trace == 1 then print(\"clipboard = \",p.clipboard) end\n");
  fprintf(fout,"%s","        ret=slotClipboardEvent(p,i,iarray.i0)\n");
  fprintf(fout,"%s","    elseif id == pv.RIGHT_MOUSE_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"RIGHT_MOUSE_EVENT id=\",i,\" text=\",text) end\n");
  fprintf(fout,"%s","        ret=slotRightMouseEvent(p,i,text)\n");
  fprintf(fout,"%s","    elseif id == pv.KEYBOARD_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"KEYBOARD_EVENT modifier=\",i,\" key=\",iarray.i0) end\n");
  fprintf(fout,"%s","        ret=slotKeyboardEvent(p,i,iarray.i0,i)\n");
  fprintf(fout,"%s","    elseif id == pv.PLOT_MOUSE_MOVED_EVENT then\n");
  fprintf(fout,"%s","        pv.getFloats(text,farray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"PLOT_MOUSE_MOVE \",farray.f0,farray.f1) end\n");
  fprintf(fout,"%s","        ret=slotMouseMovedEvent(p,i,farray.f0,farray.f1)\n");
  fprintf(fout,"%s","    elseif id == pv.PLOT_MOUSE_PRESSED_EVENT then\n");
  fprintf(fout,"%s","        pv.getFloats(text,farray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"PLOT_MOUSE_PRESSED \",farray.f0,farray.f1) end\n");
  fprintf(fout,"%s","        ret=slotMousePressedEvent(p,i,farray.f0,farray.f1)\n");
  fprintf(fout,"%s","    elseif id == pv.PLOT_MOUSE_RELEASED_EVENT then\n");
  fprintf(fout,"%s","        pv.getFloats(text,farray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"PLOT_MOUSE_RELEASED \",farray.f0,farray.f1) end\n");
  fprintf(fout,"%s","        ret=slotMouseReleasedEvent(p,i,farray.f0,farray.f1)\n");
  fprintf(fout,"%s","    elseif id == pv.MOUSE_OVER_EVENT then\n");
  fprintf(fout,"%s","        pv.getIntegers(text,iarray)\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"MOUSE_OVER_EVENT \",iarray.i0) end\n");
  fprintf(fout,"%s","        ret=slotMouseOverEvent(p,i,iarray.i0)\n");
  fprintf(fout,"%s","    elseif id == pv.USER_EVENT then\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"USER_EVENT id=\",i,\" text=\",text) end\n");
  fprintf(fout,"%s","        ret=slotUserEvent(p,i,text)\n");
  fprintf(fout,"%s","    else\n");
  fprintf(fout,"%s","        if trace == 1 then print(\"UNKNOWN_EVENT id=\",i,\" text=\",text) end\n");
  fprintf(fout,"%s","        ret = 0\n");
  fprintf(fout,"%s","    end    \n");
  fprintf(fout,"%s","    if ret ~= 0 then return ret end                -- return number of next mask to call\n");
  fprintf(fout,"%s","  end                                              -- end of event loop\n");
  fprintf(fout,"%s","  return 0                                         -- never come here\n");
  fprintf(fout,"%s","end\n");
  fclose(fout);
}

void lua_generateInitialSlots(int imask)
{
  char fname[1024];
  FILE *fout;
  
  sprintf(fname,"mask%d_slots.lua", imask);
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"%s","-------------------------------------------------------------------------------------\n");
  fprintf(fout,     "-- mask%d_slots.lua       Please edit this file in order to define your logic\n", imask);
  fprintf(fout,"%s","-------------------------------------------------------------------------------------\n");
  fprintf(fout,"%s","                         -- here you may define variables local for your mask\n");
  fprintf(fout,"%s","                         -- also see the variables in the generated maskX.lua\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotInit(p)     -- this function will be called before the event loop starts\n");
  fprintf(fout,"%s","  print(\"slotInit called\")\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotNullEvent(p)\n");
  fprintf(fout,"%s","  print(\"slotNullEvent called\")\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotButtonEvent(p,id)\n");
  fprintf(fout,"%s","  print(\"local button PushButtonBack=\", PushButtonBack)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotButtonPressedEvent(p,id)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotButtonReleasedEvent(p,id)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotTextEvent(p,id,text)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotSliderEvent(p,id,val)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotCheckboxEvent(p,id,text)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotRadioButtonEvent(p,id,text)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotGlInitializeEvent(p,id)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotGlPaintEvent(p,id)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotGlResizeEvent(p,id,width,height)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotGlIdleEvent(p,id)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotTabEvent(p,id,val)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotTableTextEvent(p,id,x,y,text)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotTableClickedEvent(p,id,x,y,button)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotSelectionEvent(p,id,val,text)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotClipboardEvent(p,id,val)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotRightMouseEvent(p,id,text)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotKeyboardEvent(p,id,val,modifier)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotMouseMovedEvent(p,id,x,y)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotMousePressedEvent(p,id,x,y)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotMouseReleasedEvent(p,id,x,y)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotMouseOverEvent(p,id,enter)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","function slotUserEvent(p,id,text)\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","end\n");
  fclose(fout);
}

void lua_generateInitialProject(const char *name)
{
  char fname[1024];
  FILE *fout;

  if(name == NULL) return;
  strcpy(fname,name); strcat(fname,".pvproject");
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"target=%s\n", name);
  fprintf(fout,"xmax=1280\n");
  fprintf(fout,"ymax=1024\n");
  fprintf(fout,"script=Lua\n");
  fclose(fout);

  strcpy(fname,"main.lua");
  fout = fopen(fname,"w");
  if(fout == NULL)
  {
    printf("could not open %s for writing\n", fname);
    return;
  }
  fprintf(fout,"%s","-------------------------------------------------------------------------------------\n");
  fprintf(fout,"%s","-- pvserver in lua     run: pvslua -port=5050 -cd=/your/directory/with/your/lua/code\n");
  fprintf(fout,"%s","-------------------------------------------------------------------------------------\n");
  fprintf(fout,"%s","trace = 1              -- here you may put variables global for all your masks\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","dofile(\"mask1.lua\")    -- include your masks here\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","-------------------------------------------------------------------------------------\n");
  fprintf(fout,"%s","function luaMain(ptr)  -- pvserver Lua Main Program\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  p = pv.getParam(ptr) -- get the PARAM structure\n");
  fprintf(fout,"%s","  pv.pvSetCaption(p,string.format(\"Hello Lua %d\",123))\n");
  fprintf(fout,"%s","  pv.pvResize(p,0,1280,1024)\n");
  fprintf(fout,"%s","  pv.pvGetInitialMask(p)\n");
  fprintf(fout,"%s","  print(\"Inital mask = \", p.initial_mask)\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  ret = 1\n");
  fprintf(fout,"%s","  while 1 do           -- show your masks\n");
  fprintf(fout,"%s","    if     (ret==1) then\n");
  fprintf(fout,"%s","      ret = showMask1(p)\n");
  fprintf(fout,"%s","    else\n");
  fprintf(fout,"%s","      ret = 1\n");
  fprintf(fout,"%s","    end\n");
  fprintf(fout,"%s","  end\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  pv.pvThreadFatal(p,\"Lua calling ThreadFatal\")\n");
  fprintf(fout,"%s","  return 0\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","end\n");
  fclose(fout);

  lua_generateInitialMask(1);
  lua_generateInitialSlots(1);
}

