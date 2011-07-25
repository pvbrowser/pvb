/***************************************************************************
                          rlreport.cpp  -  description
                             -------------------
    begin                : Mon Jul 04 2011
    copyright            : (C) 2011 pvbrowser
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include <stdarg.h>
#include "rlreport.h"
#include "rlspreadsheet.h"
#include "rlcutil.h"

rlReport::rlReport()
{
  fout = NULL;
}

rlReport::~rlReport()
{
  close();
}

int rlReport::open(const char *filename)
{
  char *cptr;
  file = "";
  if(filename == NULL) return -1;
  if(fout != NULL) fclose(fout);
  file = filename;
  cptr = strstr(file.text(),".tex");
  if(cptr != NULL) *cptr = '\0';
  fout = fopen(filename,"w");
  if(fout == NULL) return -1;
  return 0;
}

int rlReport::close()
{
  if(fout == NULL) return -1;
  fclose(fout);
  fout = NULL;
  return 0;
}
  
int rlReport::printf(const char *format, ...)
{
  int ret;
  char line[rl_PRINTF_LENGTH]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(line, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  fprintf(fout,"%s",line);
  return ret;
}

int rlReport::beginDocument()
{
  return printf("\\begin{document}\n");
}

int rlReport::endDocument()
{
  return printf("\\end{document}\n");
}

int rlReport::include(const char *filename, rlIniFile *ini)
{
  FILE *fin;
  char line[rl_PRINTF_LENGTH]; // should be big enough
  char *cptr, *start, *section, *name;

  if(fout == NULL) return -1; 
  if(filename == NULL) return -1;
  if(ini == NULL)
  {
    fin = fopen(filename,"r");
    if(fin == NULL)
    {
      printf  ("rlReport::include() ERROR: Could not find %s\n",filename);
      ::printf("rlReport::include() ERROR: Could not find %s\n",filename);
      return -1;
    }  
    while(fgets(line,sizeof(line)-1,fin) != NULL)
    {
      fprintf(fout,"%s",line);
    }
    fclose(fin);
    return 0;
  }
  else
  {
    fin = fopen(filename,"r");
    if(fin == NULL)
    {
      printf  ("rlReport::include() ERROR: Could not find %s\n",filename);
      ::printf("rlReport::include() ERROR: Could not find %s\n",filename);
      return -1;
    }  
    while(fgets(line,sizeof(line)-1,fin) != NULL)
    {
      start = &line[0];
      while(*start != '\0')
      {
        cptr = strstr(start,"\\$[");
        if(cptr == NULL)
        {
          fprintf(fout,"%s",start);
          break;
        }
        else
        {
          *cptr = '\0';
          fprintf(fout,"%s", start);
          cptr += 3;
          section = cptr;
          cptr = strchr(cptr,']');
          if(cptr == NULL)
          {
            ::printf("rlReport::include ERROR in line=%s", line);
            fclose(fin);
            return -1;
          }
          *cptr = '\0';
          cptr++;
          cptr++;
          name = cptr;
          cptr = strchr(cptr,']');
          if(cptr == NULL)
          {
            ::printf("rlReport::include ERROR in line=%s", line);
            fclose(fin);
            return -1;
          }
          *cptr = '\0';
          cptr++;
          start = cptr;
          fprintf(fout,"%s", ini->text(section,name));
        }
      }  
    }
    fclose(fin);
    return 0;
  }  
}

int rlReport::includeHeader(const char *documentclass, const char *language, const char *inputenc, const char *layout)
{
  if(fout == NULL) return -1; 
  if(documentclass == NULL) documentclass = "\\documentclass[a4paper]{article}"; 
  if(language      == NULL) language      = "\\usepackage[english]{babel}"; 
  if(inputenc      == NULL) inputenc      = "\\usepackage[utf8]{inputenc}";
  if(layout        == NULL) layout        = "\\setlength{\\parindent}{0pt} \\setlength{\\topmargin}{-50pt} \\setlength{\\oddsidemargin}{0pt}  \\setlength{\\textwidth}{480pt} \\setlength{\\textheight}{700pt}";

  fprintf(fout,"%s","% --- header begin ------------------------------------------------------------------------------------------------\n");
  fprintf(fout,     "  %s\n", documentclass);
  fprintf(fout,     "  %s\n", inputenc);
  fprintf(fout,"%s","  \\usepackage[pdftex]{hyperref}\n");
  fprintf(fout,"%s","  \\pdfcatalog{/UseThumbs /UseOutlines}\n");
  fprintf(fout,"%s","  \\usepackage{amssymb}\n");
  fprintf(fout,"%s","  \\newcommand{\\arrow}{\\begin{math}->\\end{math}}    % define arrow symbol\n");
  fprintf(fout,"%s","  \\newcommand{\\asterix}{\\begin{math}*\\end{math}}   % define asterix symbol\n");
  fprintf(fout,     "  %s\n", language);
  fprintf(fout,"%s","  \\usepackage{textcomp}                               % define euro\n");
  fprintf(fout,"%s","  \\usepackage{longtable}                              % use tables over several pages\n");
  fprintf(fout,"%s","  \\usepackage{colortbl}\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  %  % --- documentation support for sourcecode -------------\n");
  fprintf(fout,"%s","  \\usepackage{listings}\n");
  fprintf(fout,"%s","  \\usepackage{color}\n");
  fprintf(fout,"%s","  \\definecolor{white}{rgb}{1.0,1.0,1.0}\n");
  fprintf(fout,"%s","  \\definecolor{gray}{rgb}{0.5,0.5,0.5}\n");
  fprintf(fout,"%s","  \\definecolor{darkred}{rgb}{0.5,0.0,0.0}\n");
  fprintf(fout,"%s","  \\definecolor{lightgray}{rgb}{0.95,0.95,0.95}\n");
  fprintf(fout,"%s","  \\definecolor{mygray}{gray}{0.55}\n");
  fprintf(fout,"%s","  \\newcommand{\\simplecodestyle}{\\lstset{basicstyle=\\ttfamily\\small\\mdseries,backgroundcolor=\\color{lightgray},breaklines=true}}\n");
  fprintf(fout,"%s","  \\newcommand{\\codestyle}{\\lstset{basicstyle=\\ttfamily\\small\\mdseries,keywordstyle=\\bfseries\\color{blue},commentstyle=\\color{gray},stringstyle=\\color{darkred},backgroundcolor=\\color{lightgray},breaklines=true}}\n");
  fprintf(fout,"%s","  \\newcommand{\\simplecodebegin}[1]{\\simplecodestyle\\begin{lstlisting}[frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8},title={#1}]}\n");
  fprintf(fout,"%s","  \\newcommand{\\bashbegin}[1]{\\codestyle\\begin{lstlisting}[language=bash,frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8},title={#1}]}\n");
  fprintf(fout,"%s","  \\newcommand{\\bashsimple}[0]{\\codestyle\\begin{lstlisting}[language=bash,frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8}]}\n");
  fprintf(fout,"%s","  \\newcommand{\\cppbegin}[1]{\\codestyle\\begin{lstlisting}[language=C++,frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8},title={#1}]}\n");
  fprintf(fout,"%s","  \\newcommand{\\cppsimple}[0]{\\codestyle\\begin{lstlisting}[language=C++,frame=single,flexiblecolumns=true],tabsize={8},inputencoding={utf8}}\n");
  fprintf(fout,"%s","  \\newcommand{\\cppend}{\\end{lstlisting}}\n");
  fprintf(fout,"%s","  \\newcommand{\\pybegin}[1]{\\codestyle\\begin{lstlisting}[language=Python,frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8},title={#1}]}\n");
  fprintf(fout,"%s","  \\newcommand{\\xmlbegin}[1]{\\codestyle\\begin{lstlisting}[language=XML,frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8},title={#1}]}\n");
  fprintf(fout,"%s","  \\newcommand{\\tclbegin}[1]{\\codestyle\\begin{lstlisting}[language=tcl,frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8},title={#1}]}\n");
  fprintf(fout,"%s","  \\lstdefinelanguage{mylua}\n");
  fprintf(fout,"%s","    {keywords={while,for,elseif,if,else,then,do,end,return,function,and,or,not,nil,true,false},\n");
  fprintf(fout,"%s","    sensitive=true,\n");
  fprintf(fout,"%s","    morecomment=[l][\\color{mygray}]{--},\n");
  fprintf(fout,"%s","    string=[b],\n");
  fprintf(fout,"%s","    }\n");
  fprintf(fout,"%s","  \\newcommand{\\luabegin}[1]{\\codestyle\\begin{lstlisting}[language=mylua,frame=single,flexiblecolumns=true,tabsize={8},inputencoding={utf8},title={#1}]}\n");
  fprintf(fout,"%s","  %   % --- usage example begin -----------------------------\n");
  fprintf(fout,"%s","  %   \\cppbegin{Titel of our C++ code snippet}\n");
  fprintf(fout,"%s","  %     printf(\"hello world\\n\");\n");
  fprintf(fout,"%s","  %   \\end{lstlisting}\n");
  fprintf(fout,"%s","  %   % --- usage example end -------------------------------\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  %   % --- graphics support --------------------------------\n");
  fprintf(fout,"%s","  \\usepackage{graphicx}\n");
  fprintf(fout,"%s","  \\usepackage{float}\n");
  fprintf(fout,"%s","  \\newcommand{\\img}[3]{\n");
  fprintf(fout,"%s","    \\setlength\\fboxsep{0pt}\n");
  fprintf(fout,"%s","    \\setlength\\fboxrule{1pt}\n");
  fprintf(fout,"%s","    \\begin{figure}\n");
  fprintf(fout,"%s","    \\begin{center}\n");
  fprintf(fout,"%s","    \\fbox{\\includegraphics[scale=#1]{#2}}\n");
  fprintf(fout,"%s","    \\caption{#3}\n");
  fprintf(fout,"%s","    \\end{center}\n");
  fprintf(fout,"%s","    \\end{figure}\n");
  fprintf(fout,"%s","  }\n");
  fprintf(fout,"%s","  \\newcommand{\\imgH}[3]{\n");
  fprintf(fout,"%s","    \\setlength\\fboxsep{0pt}\n");
  fprintf(fout,"%s","    \\setlength\\fboxrule{1pt}\n");
  fprintf(fout,"%s","    \\begin{figure}[H]\n");
  fprintf(fout,"%s","    \\begin{center}\n");
  fprintf(fout,"%s","    \\fbox{\\includegraphics[scale=#1]{#2}}\n");
  fprintf(fout,"%s","    \\caption{#3}\n");
  fprintf(fout,"%s","    \\end{center}\n");
  fprintf(fout,"%s","    \\end{figure}\n");
  fprintf(fout,"%s","  }\n");
  fprintf(fout,"%s","\n");
  fprintf(fout,"%s","  %   % --- definition of the paper layout ------------------- \n");
  fprintf(fout,    "  %s\n", layout);
  fprintf(fout,"%s","% --- header end --------------------------------------------------------------------------------------------------\n");
  return 0;
}

int rlReport::includeCSV(const char *filename, int use_first_row_as_title, const char *legend, char delimitor)
{
  int x,y,xmax,ymax;
  rlSpreadsheetTable t(delimitor);
  if(fout == NULL) return -1;
  if(filename == NULL) return -1;
  if(t.read(filename) < 0) 
  {
    printf  ("rlReport::includeCSV() ERROR: Could not find %s\n",filename);
    ::printf("rlReport::includeCSV() ERROR: Could not find %s\n",filename);
    return -1;
  }  
 
  x = xmax = 1;
  while(t.exists(x,1) && xmax < 1024) xmax = x++; 
  y = ymax = 1;
  while(t.exists(1,y) && ymax < 256*256) ymax = y++; 

  printf("\\begin{center}\n");
  printf("\\begin{longtable}");
  
  x = 1;
  printf("{");
  while(x <= xmax) { printf(" | l"); x++; }
  printf(" | }\n");
  printf("    \\hline\n");
  for(y=1; y<=ymax; y++)
  {
    if(y==1 && use_first_row_as_title) printf("    \\rowcolor{gray}\n");;
    printf("      ");
    for(x=1; x<=xmax; x++)
    {
      printf("%s ", t.text(x,y));
      if(x < xmax) printf("& ");
    }
    printf(" \\\\\n");
    if(y < ymax) printf("    \\hline\n");
  }
  printf("    \\hline\n");
  if(legend != NULL) printf("  \\caption{%s}\n", legend);

  printf("\\end{longtable}\n");
  printf("\\end{center}\n");
  return 0;
}

int rlReport::includeImage(const char *filename, const char *legend, float scale)
{
  if(fout == NULL) return -1;
  if(filename == NULL) return -1;
  if(legend == NULL) legend = "";
  printf("\\img{%.1f}{%s}{%s}\n", scale, filename, legend);
  return 0;
}

int rlReport::spawn(const char *command)
{
  char cmd[rl_PRINTF_LENGTH]; // should be big enough
 
  if(fout == NULL) return -1;
  if(command == NULL) return -1;
  sprintf(cmd,"%s > %s.temp", command, file.text());
  system(cmd);
  sprintf(cmd,"%s.temp", file.text());
  return include(cmd);
}

int rlReport::pdflatex(const char *command)
{
  char cmd[rl_PRINTF_LENGTH]; // should be big enough
  if(command == NULL)
  {
    sprintf(cmd,"pdflatex -interaction=nonstopmode %s.tex", file.text());
    system(cmd); // run pdflatex twice
    return system(cmd);
  }
  else
  {
    sprintf(cmd,"%s %s.tex", command, file.text());
    system(cmd); // run pdflatex twice
    return system(cmd);
  }
}
  
