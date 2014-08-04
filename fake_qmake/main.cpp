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

#include "fake_qmake.h"

FILE *fout = NULL;;
rlFileLoad fpro;

int n_sources = 0;
int n_headers = 0;
int n_libs = 0;
int n_includepath = 0;
int n_defines = 0;
int n_qmake_lflags = 0;
int n_config = 0;
int n_cflags = 0;
int n_cxxflags = 0;

rlSpreadsheetRow row_sources;
rlSpreadsheetRow row_headers;
rlSpreadsheetRow row_libs;
rlSpreadsheetRow row_includepath;
rlSpreadsheetRow row_defines;
rlSpreadsheetRow row_qmake_lflags;
rlSpreadsheetRow row_config;
rlSpreadsheetRow row_cflags;
rlSpreadsheetRow row_cxxflags;

rlString str_target;
rlString str_template;

rlString line;
rlString cmdline;
rlString fname("pvs.pro");
#ifdef RLWIN32
rlString fmake("Makefile.win");
#else
rlString fmake("Makefile");
#endif

#ifdef RLWIN32
const char *builddir = "release";
char destination[64] = "release\\";
#else
const char *builddir = "";
char destination[64] = "";
#endif

static void perhapsRunQmake(int ac, char **av)
{
#if defined(RLWIN32)
  struct stat buf;
  if(getenv("QTDIR") == NULL) return;
  rlString name(getenv("QTDIR"));
  name.cat("\\bin\\qmake.exe");
  int ret = stat(name.text(),&buf);
  if(ret < 0)
  {
    printf("stat(\"%s\") ret=%d\n", name.text(), ret);
    return;
  }  
  rlString cmd(name.text());
  for(int i=1; i<ac; i++) { cmd.cat(" "); cmd.cat(av[i]); }
  printf("running real: %s\n", cmd.text());
  ret = system(cmd.text());
  exit(ret);
#elif defined(RLMAC)
  rlString cmd("qmake");
  for(int i=1; i<ac; i++) { cmd.cat(" "); cmd.cat(av[i]); }
  printf("running real: %s\n", cmd.text());
  int ret = system(cmd.text());
  exit(ret);
#elif defined(RLUNIX)
  struct stat buf;
  const char *candidates[] = { "/usr/bin/qmake",
                               "/usr/bin/qmake-qt4",
                               "/usr/lib/qt4/bin/qmake-qt4",
                               "/usr/lib/qt4/bin/qmake",
                               "/usr/lib64/qt4/bin/qmake-qt4",
                               "/usr/lib64/qt4/bin/qmake",
                               "" };
  const char *name;
  for(int j=0; ;j++)
  {
    name = candidates[j];
    if(*name == '\0') break;
    int ret = stat(name,&buf);
    if(ret >= 0)
    {
      rlString cmd(name);
      for(int i=1; i<ac; i++) { cmd.cat(" "); cmd.cat(av[i]); }
      printf("running real: %s\n", cmd.text());
      ret = system(cmd.text());
      exit(ret);
    }
  }
#endif
}

int main(int ac, char **av)
{
  int fake = 0;
  for(int i=0; i<ac; i++) { cmdline.cat(av[i]); cmdline.cat(" "); }
  for(int i=1; i<ac; i++)
  {
    const char *arg = av[i];
    if(strcmp(av[i],"-o") == 0)
    {
      i++;
      fmake = av[i];
    }
    else if(strcmp(av[i],"-builddir") == 0)
    {
      i++;
      builddir = av[i];
      if(strlen(builddir) + 4 < sizeof(destination))
      {
        strcpy(destination, builddir); strcat(destination,"/");
      }
    }
    else if(*arg != '-')
    {
      fname = av[i];
    }
    else if(strcmp(av[i],"-fake") == 0)
    {
      fake = 1;
    }
    else
    {
      printf("fake_qmake is a fake of qmake from qt with a very reduced set of features.\n");
      printf("but sufficient to build a pvserver or other simple commandline tools with MinGW.\n");
      printf("if you need all the qmake functions install the qt development package.\n");
      printf("usage:   fake_qmake <-fake> <name.pro> <-o makefile>   <-builddir directory>\n");
#ifdef RLWIN32
      printf("default: fake_qmake          name.pro   -o Makefile.win -builddir release\n");
#else
      printf("default: fake_qmake          name.pro   -o Makefile\n");
#endif
      return -1;
    }
  }
  
  if(fake == 0)
  {
    perhapsRunQmake(ac,av);
  }  
  printf("running fake_qmake\n");

  if(fpro.load(fname.text()) >= 0)
  {
    if(interpretProFile() >= 0)
    {
      fout = fopen(fmake.text(),"w");
      if(fout == NULL)
      {
        printf("could not write makefile=%s\n",fmake.text());
        return -1;
      }
      if(writeMakefile() < 0)
      {
        printf("error writeing makefile=%s\n", fmake.text());
      }
      fclose(fout);
    }
    else
    {
      printf("error interpreting file=%s line=%s\n", fname.text(), line.text());
    }
  }
  else
  {
    printf("could not load project_file=%s\n", fname.text());
  }

  return 0;
}

