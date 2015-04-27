/****************************************************************************
**
** Copyright (C) 2000-2013 Lehrig Software Engineering.
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

static const int debug = 0;

#include <sys/stat.h>
#include <sys/types.h>
#include "fake_qmake.h"

#define IS_UNKNOWN      0
#define IS_HEADERS      1
#define IS_SOURCES      2
#define IS_LIBS         3
#define IS_INCLUDEPATH  4
#define IS_DEFINES      5
#define IS_QMAKE_LFLAGS 6
#define IS_CFLAGS       7
#define IS_CXXFLAGS     8
#define IS_CONFIG       9

#if defined(RLWIN32)
#include <windows.h>
const char filter1[]  = "win32:";
const char filter2[]  = "win32";
const char filter1b[] = "win32-g++:";
const char filter2b[] = "win32-g++";
#elif defined(RLMAC)
const char filter1[] = "macx:";
const char filter2[] = "macx";
#elif defined(RLUNIX)
const char filter1[] = "unix:";
const char filter2[] = "unix";
#endif

extern rlString fname;
extern rlString fmake;

static int n_already_depend = 0;
static rlSpreadsheetRow row_already_depend;

enum Assignment
{
  equal = 0,
  minus,
  plus
};

static int assignment = equal;

static int filter2Found(const char *line)
{
  const char *cptr;
  for(int i=1; i<n_config; i++)
  {
    cptr = strstr(line,row_config.text(i));
    if(cptr != NULL) return 1;
  }
  return 0;
}

static int filter1Found(const char *line)
{
  int i;
  const char *cptr;
  rlString needle;
  for(i=1; i<n_config; i++)
  {
    needle.strcpy(row_config.text(i));
    needle.cat(":");
    cptr = strstr(line,needle.text());
    //printf("strstr(%s,%s) cptr=%d\n", line, needle.text(), cptr);
    if(cptr != NULL) return 1;
  }
  return 0;
}

static void printDebug()
{
  if(1) return;
  int i;
  printf("TEMPLATE='%s'\n",     str_template.text());
  printf("TARGET='%s'\n",       str_target.text());
  for(i=1; i<=n_config; i++)       printf("config[%d] ='%s'\n", i, row_config.text(i));
  for(i=1; i<=n_headers; i++)      printf("headers[%d] = '%s'\n", i, row_headers.text(i));
  for(i=1; i<=n_sources; i++)      printf("sources[%d] = '%s'\n", i, row_sources.text(i));
  for(i=1; i<=n_libs; i++)         printf("libs[%d] = '%s'\n", i, row_libs.text(i));
  for(i=1; i<=n_includepath; i++)  printf("includepath[%d] = '%s'\n", i, row_includepath.text(i));
  for(i=1; i<=n_defines; i++)      printf("defines[%d] = '%s'\n", i, row_defines.text(i));
  for(i=1; i<=n_qmake_lflags; i++) printf("qmake_lflags[%d] = '%s'\n", i, row_qmake_lflags.text(i));
  for(i=1; i<=n_cflags; i++)       printf("cflags[%d]='%s'\n", i, row_cflags.text(i));
  for(i=1; i<=n_cxxflags; i++)     printf("cxxflags[%d]='%s'\n", i, row_cxxflags.text(i));
}

static int n_words;
static rlSpreadsheetRow row_words;
static int getWords(const char *text)
{
  n_words = 0;
  rlString tmp(text);
  char *cptr, *end;

  if(*text == '#') return n_words;
  if(debug) printf("getWords1(%s)\n", text);
  cptr = strstr(tmp.text(),"=");
  if(cptr == NULL) 
  {
    cptr = tmp.text();
  }  
  else             
  {
    *cptr = 0;
    cptr++;
    if(strstr(tmp.text(),":") != NULL)
    {
      if(filter1Found(tmp.text()) == 0) return n_words;
      //if(strstr(tmp.text(),filter1) == NULL) return n_words;
    }
  }
  while(*cptr != '\0')
  {
    while(*cptr <= ' ' && *cptr != '\0') cptr++;
    if(*cptr != '\0')
    {
      //if(debug) printf("cptr='%s'\n", cptr);
      end = cptr + 1;
      if(*cptr <= ' ')                    return n_words;  
      if(*cptr == '\\' && cptr[1] <= ' ') return n_words;  
      while(*end > ' ') end++;
      if(*end == '\0')
      {
        *end = '\0';
      }
      else
      {
        *end = '\0';
        end++;
      }
      row_words.setText(++n_words, cptr);
      if(debug) printf("getWords2 word(%d)='%s'\n", n_words, row_words.text(n_words));
      cptr = end;
      //if(debug) printf("cptr2='%s'\n", cptr);
    }
  }
  return n_words;
}

int interpretProFile()
{
  int iitem = IS_UNKNOWN;
  char *cptr;
  int relevant = 1;

  if(*destination != '\0') rlMkdir(destination);

  n_config++;
  row_config.setText(n_config,filter2);
#ifdef RLWIN32
  n_config++;
  row_config.setText(n_config,filter2b);
#endif

  const char *ccptr = fpro.firstLine();
  while(ccptr != NULL)
  {
    line.strcpy(ccptr);
    cptr = strchr(line.text(),'\n');
    if(cptr != NULL) *cptr = ' ';

    if(relevant && *ccptr != '#' && strstr(ccptr,"=") != NULL)
    {
      if     (strstr(ccptr,"-=") != NULL) assignment = minus;
      else if(strstr(ccptr,"+=") != NULL) assignment = plus;
      else                                assignment = equal;

      if     (strstr(ccptr,"TEMPLATE") != NULL)
      {
        cptr = strstr(line.text(),"=");
        if(cptr != NULL)
        {
          cptr++;
          while(*cptr <= ' ') cptr++;
          str_template.strcpy(cptr);
        }  
      }
      else if(strstr(ccptr,"CONFIG") != NULL)
      {
        iitem = IS_CONFIG;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"HEADERS") != NULL)
      {
        iitem = IS_HEADERS;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"SOURCES") != NULL && strstr(ccptr,"RESOURCES") == NULL)
      {
        iitem = IS_SOURCES;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"LIBS") != NULL)
      {
        iitem = IS_LIBS;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"INCLUDEPATH") != NULL)
      {
        iitem = IS_INCLUDEPATH;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"DEFINES") != NULL)
      {
        iitem = IS_DEFINES;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"QMAKE_LFLAGS") != NULL)
      {
        iitem = IS_QMAKE_LFLAGS;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"CFLAGS") != NULL)
      {
        iitem = IS_CFLAGS;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"CXXFLAGS") != NULL)
      {
        iitem = IS_CXXFLAGS;
        iitem = getItemsFromLine(iitem);
      }
      else if(strstr(ccptr,"TARGET") != NULL)
      {
        cptr = strstr(line.text(),"=");
        if(cptr != NULL)
        {
          cptr++;
          while(*cptr <= ' ') cptr++;
          str_target.strcpy(cptr);
        }  
      }
    }
    else if(relevant && *ccptr != '#' && iitem != IS_UNKNOWN)
    {
      iitem = getItemsFromLine(iitem);
    }
    else if(*ccptr != '#')
    {
      if(debug) printf("else text=%s\n", line.text());
      if     (strstr(line.text(),"}") != NULL) 
      {
        relevant = 1;
      }
      else if(strstr(line.text(),"{") != NULL)
      {
        if(strstr(line.text(),"!") != NULL)
        {
          //if(strstr(line.text(),filter2) == NULL) relevant = 1;
          if(filter2Found(line.text()) == 0)      relevant = 1;
          else                                    relevant = 0;
        }
        //else if(strstr(line.text(),filter2) != NULL)
        else if(filter2Found(line.text()) == 1)
        {
          relevant = 1;
        }
        else
        {
          relevant = 0;
          cptr = line.text(); // eventually user inserted config
          while(*cptr > ' ') cptr++;
          *cptr = '\0';
          for(int i=1; i<=n_config; i++)
          {
            if(strcmp(row_config.text(i),line.text()) == 0) relevant = 1;
          }
        }
      }  
    }
    ccptr = fpro.nextLine();
  }
  printDebug();
  return 0;
}

static void remove(rlSpreadsheetRow *row, int *n, const char *item)
{
  int front = 1;
  int tail  = 1;
  while(front <= *n)
  {
    const char *ftext = row->text(front);
    if(strcmp(item,ftext) == 0)
    {
      if     (front == tail) tail--;
      else if(front  > tail) row->setText(tail,ftext);
      front++;
      *n = *n - 1;
    }
    else
    {
      if     (front  > tail) row->setText(tail,ftext);
      front++;
      tail++;
    }
  }
}

int getItemsFromLine(int iitem)
{
  const char *item;
  getWords(line.text());
  for(int i=1; i<=n_words; i++)
  {
    item = row_words.text(i);
    switch(iitem)
    {
      case IS_SOURCES:
        if(assignment == equal) { n_sources = 0; assignment = plus; }
        if(assignment == plus)  row_sources.setText(++n_sources, item);
        else                    remove(&row_sources,&n_sources,item);
        break;
      case IS_HEADERS:    
        if(assignment == equal) { n_headers = 0; assignment = plus; }
        if(assignment == plus)  row_headers.setText(++n_headers, item);
        else                    remove(&row_headers,&n_headers,item);
        break;
      case IS_LIBS:        
        if(assignment == equal) { n_libs = 0; assignment = plus; }
        if(assignment == plus)  row_libs.setText(++n_libs, item);
        else                    remove(&row_libs,&n_libs,item);
        break;
      case IS_INCLUDEPATH: 
        if(assignment == equal) { n_includepath = 0; assignment = plus; }
        if(assignment == plus)  row_includepath.setText(++n_includepath, item);
        else                    remove(&row_includepath,&n_includepath,item);
        break;
      case IS_DEFINES:
        if(assignment == equal) { n_defines = 0; assignment = plus; }
        if(assignment == plus)  row_defines.setText(++n_defines, item);
        else                    remove(&row_defines,&n_defines,item);
        break;
      case IS_QMAKE_LFLAGS:
        if(assignment == equal) { n_qmake_lflags = 0; assignment = plus; }
        if(assignment == plus)  row_qmake_lflags.setText(++n_qmake_lflags, item);
        else                    remove(&row_qmake_lflags,&n_qmake_lflags,item);
        break;
      case IS_CFLAGS:
        if(assignment == equal) { n_cflags = 0; assignment = plus; }
        if(assignment == plus)  row_cflags.setText(++n_cflags, item);
        else                    remove(&row_cflags,&n_cflags,item);
        break;
      case IS_CXXFLAGS:
        if(assignment == equal) { n_cxxflags = 0; assignment = plus; }
        if(assignment == plus)  row_cxxflags.setText(++n_cxxflags, item);
        else                    remove(&row_cxxflags,&n_cxxflags,item);
        break;
      case IS_CONFIG:
        if(assignment == equal) { n_config = 1; assignment = plus; }
        if(assignment == plus)  row_config.setText(++n_config, item);
        else                    remove(&row_config,&n_config,item);
        break;
      default:
        break;
    }
  }
  
  item = strstr(line.text(),"\\");
  if     (item == NULL)   return IS_UNKNOWN;
  else if(item[1] <= ' ') return iitem;
  else                    return IS_UNKNOWN;
}

static const char *getExtension(const char *text)
{
  const char *cptr = strchr(text,'.');
  return cptr;
}

static const char *getName(const char *text)
{
  static rlString txt;
  txt.strcpy(text);
  char *cptr = strrchr(txt.text(),'.');
  if(cptr != NULL) *cptr = '\0';
  cptr = strrchr(txt.text(),'/');
  if(cptr != NULL)
  {
    cptr++;
    return cptr;
  }
  cptr = strrchr(txt.text(),'\\');
  if(cptr != NULL)
  {
    cptr++;
    return cptr;
  }
  return txt.text();
}

static const char *strip(const char *text)
{
  static rlString txt;
  
  const char *cptr = strrchr(text,'/');
  if(cptr == NULL)
  {
    cptr = strrchr(text,'\\');
  }
  if(cptr != NULL) txt.strcpy(++cptr);
  else             txt.strcpy(text);
  return txt.text();
}

static void printDependInHeader(int i)
{
  rlFileLoad f;
  rlString name;
  char *cptr;
  if(f.load(row_headers.text(i)) < 0)
  {
    printf("INFORMATION: could not open %s for testing dependencies\n", row_headers.text(i));
    return;
  }

  const char *ccptr = f.firstLine();
  while(ccptr != NULL)
  {
    if(strncmp(ccptr,"#include ",9) == 0)
    {
      ccptr = strchr(ccptr,'"');
      if(ccptr != NULL)
      {
        ccptr++;
        name.strcpy(ccptr);
        cptr = strchr(name.text(),'"');
        if(cptr != NULL)
        {
          int iin = i;
          *cptr = '\0';
          for(int j=1; j<=n_headers; j++)
          {
            if(strcmp(name.text(), strip(row_headers.text(j))) == 0)
            {
              int already = 0;
              for(int d=1; d<=n_already_depend; d++)
              {
                if(strcmp(name.text(), row_already_depend.text(d)) == 0) already = 1;
              }
              if(already == 0)
              {
                if(i != iin) 
                {
                  row_already_depend.setText(n_already_depend++,name.text());
                  fprintf(fout," %s", row_headers.text(j));
                  printDependInHeader(j);
                } 
              }  
              break;
            }
          }          
        }
      }
    }
    ccptr = f.nextLine();
  }
}

static void printDepend(int i)
{
  rlFileLoad f;
  rlString name;
  char *cptr;
  n_already_depend = 0;

  if(f.load(row_sources.text(i)) < 0)
  {
    printf("WARNING: could not open %s for testing dependencies\n", row_sources.text(i));
    return;
  }
  const char *ccptr = f.firstLine();
  while(ccptr != NULL)
  {
    if(strncmp(ccptr,"#include ",9) == 0)
    {
      ccptr = strchr(ccptr,'"');
      if(ccptr != NULL)
      {
        ccptr++;
        name.strcpy(ccptr);
        cptr = strchr(name.text(),'"');
        if(cptr != NULL)
        {
          *cptr = '\0';
          for(int j=1; j<=n_headers; j++)
          {
            if(strcmp(name.text(), strip(row_headers.text(j))) == 0)
            {
              int already = 0;
              for(int d=1; d<=n_already_depend; d++)
              {
                if(strcmp(name.text(), row_already_depend.text(d)) == 0) already = 1;
              }
              if(already == 0)
              {
                row_already_depend.setText(n_already_depend++,name.text());
                fprintf(fout," %s", row_headers.text(j));
                printDependInHeader(i);
              }  
              break;
            }
          }          
        }
      }
    }
    ccptr = f.nextLine();
  }
}

#ifdef RLWIN32
const char *slosh(const char *text)
{
  static rlString txt;
  txt.strcpy(text);
  char *cptr = txt.text();
  while(*cptr != '\0')
  {
    if(*cptr == '/') *cptr = '\\';
    cptr++;
  }
  return txt.text();
}
#endif

int writeMakefile()
{
#if defined(RLWIN32)
int i;
int is_staticlib = 0;
fprintf(fout,"#############################################################################\n");
fprintf(fout,"# Makefile for building: %s\n", str_target.text());
fprintf(fout,"# Generated by fake_qmake\n");
fprintf(fout,"# Project:  %s\n", fname.text());
fprintf(fout,"# Template: %s\n", str_template.text());
fprintf(fout,"# Command:  %s\n", cmdline.text());
fprintf(fout,"#############################################################################\n");
fprintf(fout,"\n");
fprintf(fout,"####### Compiler, tools and options\n");
fprintf(fout,"\n");
fprintf(fout,"CC            = gcc\n");
fprintf(fout,"CXX           = g++\n");
if(strcmp(str_template.text(),"app") == 0)
{
  fprintf(fout,"DEFINES       = -DQT_LARGEFILE_SUPPORT");
  for(i=1; i<=n_defines; i++) 
  {
    if(strcmp(row_defines.text(i),"UNICODE") != 0) 
    {
      if(strlen(row_defines.text(i)) > 0)
        fprintf(fout," -D%s", row_defines.text(i));
    }    
  }  
}
else
{
  fprintf(fout,"DEFINES       = -DQT_LARGEFILE_SUPPORT");
  for(i=1; i<=n_defines; i++) 
  {
    if(strcmp(row_defines.text(i),"UNICODE") != 0) 
    {
      if(strlen(row_defines.text(i)) > 0)
        fprintf(fout," -D%s", row_defines.text(i));
    }    
  }  
}
fprintf(fout,"\n");
fprintf(fout,"CFLAGS        = -O2 -Wall $(DEFINES)\n");
fprintf(fout,"CXXFLAGS      = -O2 -Wall $(DEFINES)\n");
fprintf(fout,"INCPATH       =");
for(i=1; i<=n_includepath; i++) fprintf(fout," -I\"%s\"", slosh(row_includepath.text(i)));
fprintf(fout,"\n");
fprintf(fout,"LINK          = g++\n");
if     (strcmp(str_template.text(),"app") == 0)
{
  fprintf(fout,"LFLAGS        =");
  for(i=1; i<=n_qmake_lflags; i++) fprintf(fout," %s", row_qmake_lflags.text(i));
  fprintf(fout," -Wl,-s \n");
}
else if(strcmp(str_template.text(),"lib") == 0)
{
  for(i=1; i<=n_config; i++)
  {
    if(strcmp(row_config.text(i),"staticlib") == 0) is_staticlib = 1;
  }
  if(is_staticlib == 0)
  {
    fprintf(fout,"LFLAGS        =");
    for(i=1; i<=n_qmake_lflags; i++) fprintf(fout," %s", row_qmake_lflags.text(i));
    fprintf(fout," -Wl,-s -mthreads -shared -Wl,--out-implib,%slib%s.a\n", destination, str_target.text());
  }
}
fprintf(fout,"LIBS          =");
for(i=1; i<=n_libs; i++) fprintf(fout," %s", row_libs.text(i));
fprintf(fout,"\n");
fprintf(fout,"QMAKE         = fake_qmake.exe\n");
fprintf(fout,"IDL           = midl\n");
fprintf(fout,"ZIP           = zip -r -9\n");
fprintf(fout,"DEF_FILE      = \n");
fprintf(fout,"RES_FILE      = \n");
fprintf(fout,"COPY          = copy /y\n");
fprintf(fout,"SED           = \n");
fprintf(fout,"COPY_FILE     = $(COPY)\n");
fprintf(fout,"COPY_DIR      = xcopy /s /q /y /i\n");
fprintf(fout,"DEL_FILE      = del\n");
fprintf(fout,"DEL_DIR       = rmdir\n");
fprintf(fout,"MOVE          = move\n");
fprintf(fout,"CHK_DIR_EXISTS= if not exist\n");
fprintf(fout,"MKDIR         = mkdir\n");
fprintf(fout,"INSTALL_FILE    = $(COPY_FILE)\n");
fprintf(fout,"INSTALL_PROGRAM = $(COPY_FILE)\n");
fprintf(fout,"INSTALL_DIR     = $(COPY_DIR)\n");
fprintf(fout,"\n");
fprintf(fout,"####### Files\n");
fprintf(fout,"\n");
fprintf(fout,"SOURCES       =");
for(i=1; i<=n_sources; i++)
{
  if(i==n_sources) fprintf(fout,"\t%s\n\n",row_sources.text(i));
  else             fprintf(fout,"\t%s \\\n",row_sources.text(i));
}  
fprintf(fout,"OBJECTS       =");
for(i=1; i<=n_sources ; i++)
{
  if(i==n_sources) fprintf(fout,"\t%s%s.o\n\n",  destination, getName(row_sources.text(i)));
  else             fprintf(fout,"\t%s%s.o \\\n", destination, getName(row_sources.text(i)));
}  
fprintf(fout,"DIST           = \n");
fprintf(fout,"QMAKE_TARGET   = %s\n", str_target.text());
fprintf(fout,"DESTDIR        = %s #avoid trailing-slash linebreak\n", destination);
fprintf(fout,"DESTDIR_TARGET = %s%s\n", destination, str_target.text());
if(     strcmp(str_template.text(),"app") == 0)                      fprintf(fout,"TARGET         = %s.exe\n", str_target.text());
else if(strcmp(str_template.text(),"lib") == 0 && is_staticlib)      fprintf(fout,"TARGET         = lib%s.a\n", str_target.text());
else if(strcmp(str_template.text(),"lib") == 0 && is_staticlib == 0) fprintf(fout,"TARGET         = %s.dll\n", str_target.text());
if(     strcmp(str_template.text(),"app") == 0)                      fprintf(fout,"DESTDIR_TARGET         = %s%s.exe\n", destination, str_target.text());
else if(strcmp(str_template.text(),"lib") == 0 && is_staticlib)      fprintf(fout,"DESTDIR_TARGET         = %slib%s.a\n", destination, str_target.text());
else if(strcmp(str_template.text(),"lib") == 0 && is_staticlib == 0) fprintf(fout,"DESTDIR_TARGET         = %s%s.dll\n", destination, str_target.text());
fprintf(fout,"\n");
fprintf(fout,"####### Implicit rules\n");
fprintf(fout,"\n");
fprintf(fout,".SUFFIXES: .cpp .cc .cxx .c\n");
fprintf(fout,"\n");
fprintf(fout,".cpp.o:\n");
fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<\n");
fprintf(fout,"\n");
fprintf(fout,".cc.o:\n");
fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<\n");
fprintf(fout,"\n");
fprintf(fout,".cxx.o:\n");
fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<\n");
fprintf(fout,"\n");
fprintf(fout,".c.o:\n");
fprintf(fout,"\t$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<\n");
fprintf(fout,"\n");
fprintf(fout,"####### Build rules\n");
fprintf(fout,"\n");
fprintf(fout,"first: all\n");
fprintf(fout,"all: %s $(DESTDIR_TARGET)\n", fmake.text());
fprintf(fout,"\n");
fprintf(fout,"$(DESTDIR_TARGET):  $(OBJECTS)\n"); 
if(strcmp(str_template.text(),"app") == 0)
{
  fprintf(fout,"\t$(LINK) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)\n");
}
else if(strcmp(str_template.text(),"lib") == 0)
{
  if(is_staticlib) fprintf(fout,"\tar -ru $(DESTDIR_TARGET) $(OBJECTS)\n");
  else             fprintf(fout,"\t$(LINK) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)\n");;
}
fprintf(fout,"\n");
fprintf(fout,"qmake:  FORCE\n");
fprintf(fout,"\t%s\n", cmdline.text());
fprintf(fout,"\n");
fprintf(fout,"clean:\n"); 
fprintf(fout,"\t-$(DEL_FILE) $(OBJECTS)");
fprintf(fout,"\n");
fprintf(fout,"####### Compile\n");
fprintf(fout,"\n");
for(i=1; i<=n_sources; i++)
{
  fprintf(fout,"%s%s.o: %s", destination, getName(getName(row_sources.text(i))), row_sources.text(i));
  printDepend(i);
  fprintf(fout,"\n");
  if(strcmp(getExtension(row_sources.text(i)),".c") == 0)
  {
    fprintf(fout,"\t$(CC) -c $(CFLAGS) $(INCPATH) -o %s%s.o %s\n", destination, getName(row_sources.text(i)), row_sources.text(i));
  }
  else
  {
    fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o %s%s.o %s\n", destination, getName(row_sources.text(i)), row_sources.text(i));
  }
  fprintf(fout,"\n");
  
  //CreateDirectoryA("release",NULL);
}

#elif defined(RLMAC)
printf("writeMakefile not implemented on RLMAC\n");

#elif defined(RLUNIX)
int i;
fprintf(fout,"#########################################################################################\n");
fprintf(fout,"# Makefile for building: %s\n", str_target.text());
fprintf(fout,"# Generated by fake_qmake If fake_qmake is not sufficient please use the original qmake\n");
fprintf(fout,"# Project:  %s.pro\n", str_target.text());
fprintf(fout,"# Template: %s\n", str_template.text());
fprintf(fout,"# Command:  %s\n", cmdline.text());
fprintf(fout,"#########################################################################################\n");
fprintf(fout,"\n");
fprintf(fout,"####### Compiler, tools and options\n");
fprintf(fout,"\n");
fprintf(fout,"CC            = gcc\n");
fprintf(fout,"CXX           = g++\n");
fprintf(fout,"DEFINES       =");
for(i=1; i<=n_defines; i++) 
{
  if(strlen(row_defines.text(i)) > 0)
    fprintf(fout," -D%s", row_defines.text(i));
}    
fprintf(fout,"\n");
//fprintf(fout,"CFLAGS        = -m64 -pipe -O2 -Wall -W $(DEFINES)");
fprintf(fout,"CFLAGS        = -pipe -O2 -Wall -W $(DEFINES)");
for(i=1; i<=n_cflags; i++) fprintf(fout," %s", row_cflags.text(i));
fprintf(fout,"\n");
//fprintf(fout,"CXXFLAGS      = -m64 -pipe -O2 -Wall -W $(DEFINES)");
fprintf(fout,"CXXFLAGS      = -pipe -O2 -Wall -W $(DEFINES)");
for(i=1; i<=n_cxxflags; i++) fprintf(fout," %s", row_cxxflags.text(i));
fprintf(fout,"\n");
fprintf(fout,"INCPATH       = -I.");
for(i=1; i<=n_includepath; i++) fprintf(fout," -I%s", row_includepath.text(i));
fprintf(fout,"\n");
fprintf(fout,"LINK          = g++\n");
if     (strcmp(str_template.text(),"app") == 0)
{
  //fprintf(fout,"LFLAGS        = -m64 -Wl,-O1");
  fprintf(fout,"LFLAGS        = -Wl,-O1");
  for(i=1; i<=n_qmake_lflags; i++) fprintf(fout," %s", row_qmake_lflags.text(i));
}
else if(strcmp(str_template.text(),"lib") == 0)
{
  //fprintf(fout,"LFLAGS        = -m64 -Wl,-O1 -shared -Wl,-soname,lib%s.so.1", str_target.text());
  fprintf(fout,"LFLAGS        = -Wl,-O1 -shared -Wl,-soname,lib%s.so.1", str_target.text());
  for(i=1; i<=n_qmake_lflags; i++) fprintf(fout," %s", row_qmake_lflags.text(i));
  fprintf(fout," -ldl -lpthread");
}
fprintf(fout,"\n");
fprintf(fout,"LIBS          = $(SUBLIBS)");
for(i=1; i<=n_libs; i++) fprintf(fout," %s", row_libs.text(i));
fprintf(fout,"\n");
fprintf(fout,"AR            = ar cqs\n");
fprintf(fout,"RANLIB        = \n");
fprintf(fout,"QMAKE         = fake_qmake\n");
fprintf(fout,"TAR           = tar -cf\n");
fprintf(fout,"COMPRESS      = gzip -9f\n");
fprintf(fout,"COPY          = cp -f\n");
fprintf(fout,"SED           = sed\n");
fprintf(fout,"COPY_FILE     = $(COPY)\n");
fprintf(fout,"COPY_DIR      = $(COPY) -r\n");
fprintf(fout,"STRIP         = strip\n");
fprintf(fout,"INSTALL_FILE  = install -m 644 -p\n");
fprintf(fout,"INSTALL_DIR   = $(COPY_DIR)\n");
fprintf(fout,"INSTALL_PROGRAM = install -m 755 -p\n");
fprintf(fout,"DEL_FILE      = rm -f\n");
fprintf(fout,"SYMLINK       = ln -f -s\n");
fprintf(fout,"DEL_DIR       = rmdir\n");
fprintf(fout,"MOVE          = mv -f\n");
fprintf(fout,"CHK_DIR_EXISTS= test -d\n");
fprintf(fout,"MKDIR         = mkdir -p\n");
fprintf(fout,"\n");
fprintf(fout,"####### Files\n");
fprintf(fout,"\n");
fprintf(fout,"SOURCES = \\\n");
for(i=1; i<=n_sources; i++)
{
  fprintf(fout,"\t\t%s", row_sources.text(i));
  if(i==n_sources) fprintf(fout,"\n\n");
  else             fprintf(fout," \\\n");
}
fprintf(fout,"OBJECTS = \\\n");
for(i=1; i<=n_sources; i++)
{
  fprintf(fout,"\t\t%s%s.o", destination, getName(row_sources.text(i)));
  if(i==n_sources) fprintf(fout,"\n\n");
  else             fprintf(fout," \\\n");
}
if     (strcmp(str_template.text(),"app") == 0)
{
  fprintf(fout,"TARGET = %s\n", str_target.text());
  fprintf(fout,"DESTDIR        = %s #avoid trailing-slash linebreak\n", destination);
  fprintf(fout,"DESTDIR_TARGET = %s%s\n", destination, str_target.text());
}
else if(strcmp(str_template.text(),"lib") == 0)
{
  int staticlib_found = 0;
  for(int i=1; i<= n_config; i++)
  {
    if(strcmp(row_config.text(i),"staticlib") == 0) staticlib_found = 1;
  }
  if(staticlib_found) fprintf(fout,"TARGET  = lib%s.a\n", str_target.text());
  else                fprintf(fout,"TARGET  = lib%s.so.1.0.0\n", str_target.text());
  fprintf(fout,"TARGETA = lib%s.a\n", str_target.text());
  fprintf(fout,"TARGETD = lib%s.so.1.0.0\n", str_target.text());
  fprintf(fout,"TARGET0 = lib%s.so\n", str_target.text());
  fprintf(fout,"TARGET1 = lib%s.so.1\n", str_target.text());
  fprintf(fout,"TARGET2 = lib%s.so.1.0\n", str_target.text());
  fprintf(fout,"DESTDIR        = %s #avoid trailing-slash linebreak\n", destination);
  if(staticlib_found) fprintf(fout,"DESTDIR_TARGET = %slib%s.a\n", destination, str_target.text());
  else                fprintf(fout,"DESTDIR_TARGET = %slib%s.so.1.0.0\n", destination, str_target.text());
  fprintf(fout,"DESTDIR_TARGETA = %slib%s.a\n", destination, str_target.text());
  fprintf(fout,"DESTDIR_TARGETD = %slib%s.so.1.0.0\n", destination, str_target.text());
  fprintf(fout,"DESTDIR_TARGET0 = %slib%s.so\n", destination, str_target.text());
  fprintf(fout,"DESTDIR_TARGET1 = %slib%s.so.1\n", destination, str_target.text());
  fprintf(fout,"DESTDIR_TARGET2 = %slib%s.so.1.0\n", destination, str_target.text());
}
fprintf(fout,"\n");
fprintf(fout,"first: all\n");
fprintf(fout,"####### Implicit rules\n");
fprintf(fout,"\n");
fprintf(fout,".SUFFIXES: .o .c .cpp .cc .cxx .C\n");
fprintf(fout,"\n");
fprintf(fout,".cpp.o:\n");
fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n");
fprintf(fout,"\n");
fprintf(fout,".cc.o:\n");
fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n");
fprintf(fout,"\n");
fprintf(fout,".cxx.o:\n");
fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n");
fprintf(fout,"\n");
fprintf(fout,".C.o:\n");
fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n");
fprintf(fout,"\n");
fprintf(fout,".c.o:\n");
fprintf(fout,"\t$(CC) -c $(CFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n");
fprintf(fout,"\n");
fprintf(fout,"####### Build rules\n");
fprintf(fout,"\n");
fprintf(fout,"all: Makefile $(DESTDIR_TARGET)\n");
fprintf(fout,"\n");
if     (strcmp(str_template.text(),"app") == 0)
{
  fprintf(fout,"$(DESTDIR_TARGET):  $(OBJECTS)\n");  
  fprintf(fout,"\t$(LINK) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)\n");
}
else if(strcmp(str_template.text(),"lib") == 0)
{
  int staticlib_found = 0;
  for(int i=1; i<= n_config; i++)
  {
    if(strcmp(row_config.text(i),"staticlib") == 0) staticlib_found = 1;
  }
  if(staticlib_found == 0)
  {
    fprintf(fout,"$(DESTDIR_TARGET):  $(OBJECTS)\n");  
    fprintf(fout,"\t-$(DEL_FILE) $(TARGET) $(DESTDIR_TARGET0) $(DESTDIR_TARGET1) $(DESTDIR_TARGET2)\n");
    fprintf(fout,"\t$(LINK) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS) $(OBJCOMP)\n");
    fprintf(fout,"\t-ln -s $(DESTDIR_TARGET) $(DESTDIR_TARGET0)\n");
    fprintf(fout,"\t-ln -s $(DESTDIR_TARGET) $(DESTDIR_TARGET1)\n");
    fprintf(fout,"\t-ln -s $(DESTDIR_TARGET) $(DESTDIR_TARGET2)\n");
    fprintf(fout,"\n");
  }  
  fprintf(fout,"\n");
  fprintf(fout,"staticlib: $(DESTDIR_TARGETA)\n");
  fprintf(fout,"\n");
  fprintf(fout,"$(DESTDIR_TARGETA):  $(OBJECTS) $(OBJCOMP)\n");
  fprintf(fout,"\t-$(DEL_FILE) $(DESTDIR_TARGETA)\n");
  fprintf(fout,"\t$(AR) $(DESTDIR_TARGETA) $(OBJECTS)\n");

}
fprintf(fout,"\n");
fprintf(fout,"%s: %s\n", fmake.text(), fname.text());
fprintf(fout,"\t%s\n", cmdline.text());
fprintf(fout,"\n");
fprintf(fout,"clean:\n"); 
fprintf(fout,"\t-$(DEL_FILE) $(OBJECTS)\n");
fprintf(fout,"\t-$(DEL_FILE) *~ core *.core\n");
fprintf(fout,"\n");
fprintf(fout,"####### Compile\n");
fprintf(fout,"\n");
for(i=1; i<=n_sources; i++)
{
  fprintf(fout,"%s%s.o: %s", destination, getName(row_sources.text(i)), row_sources.text(i));
  printDepend(i);
  fprintf(fout,"\n");
  if(strcmp(getExtension(row_sources.text(i)),".c") == 0)
  {
    fprintf(fout,"\t$(CC) -c $(CFLAGS) $(INCPATH) -o %s%s.o %s\n", destination, getName(row_sources.text(i)), row_sources.text(i));
  }
  else
  {
    fprintf(fout,"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o %s%s.o %s\n", destination, getName(row_sources.text(i)), row_sources.text(i));
  }
  fprintf(fout,"\n");
}

#endif
  return 0;
}


