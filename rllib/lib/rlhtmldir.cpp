/***************************************************************************
                          rlhtmldir.cpp  -  description
                             -------------------
    begin                : Thu Jul 30 2015
    copyright            : (C) Lehrig Software Enigineering
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlhtmldir.h"
#include "rlspawn.h"

rlHtmlDir::rlHtmlDir()
          :rlHtml()
{
  hide_hidden_files = 0;
  pattern     = "*";
  initialPath = "";
  currentPath = "";
  recursive   = 0;
  list_what   = list_files_and_directories;
  textHome    = "Home:";
  textPath    = "Path:";
  textrlHtmlDirCSS = "rlhtml.css";
}

rlHtmlDir::~rlHtmlDir()
{
}

int rlHtmlDir::cd(const char *dirIn, int is_initial_dir)
{
  rlString dirCopy;
  dirCopy = dirIn;
  const char *dir = dirCopy.text();
  if(*dir == '.')
  {
    if(dir[1] == '.')
    {
      if(strlen( initialPath.text() ) < strlen( currentPath.text() ))
      {
        char *cptr = strrchr(currentPath.text(),'/');
        if(cptr != NULL)
        {
          if(cptr != currentPath.text()) *cptr = '\0'; // cd ..
          return 0;
        }
      }
    }
    return -1;
  }
  else if(*dir == '/')                                // cd /absolute/path
  {
    currentPath = dir;
    if(is_initial_dir) initialPath = dir;
  }
  else if(*dir == '~')
  {
    currentPath = initialPath;                        // cd home
  }
  else
  {
    currentPath += "/";
    currentPath += dir;                               // cd into subdirectory
  }
  return 0;
}

const char * rlHtmlDir::getHtml(int generate_html_header_and_trailer)
{
  html = "";
  if(generate_html_header_and_trailer)
  {
    html = htmlHeader();
  }          

  html += startHtml.text();
  html += "<pre class=\"rlhtmldirpre\">\n";

  rlSpawn spawn;
  rlString cmd;
  const char *line;
  rlString file;
  rlString htmlline;
  char *cptr;
#ifdef RLWIN32
  if(recursive) cmd.printf("rlfind \"%s\" -name \"%s\"", currentPath.text(), pattern.text());
  else          cmd.printf("rlfind \"%s\"", currentPath.text());
  //else          cmd.printf("rlfind \"%s\" -only \"%s\"", currentPath.text(), pattern.text());
#else
  if(recursive) cmd.printf("sh -c \"rlfind '%s' -name '%s' | sort\"", currentPath.text(), pattern.text());
  else          cmd.printf("sh -c \"rlfind '%s' -only '%s' | sort\"", currentPath.text(), pattern.text());
#endif
  if(1) //recursive == 0)
  {
    htmlline.printf("%s <a  class=\"rlhtmldirhome\" href=\"ftp://rlhtmldirhome%s\">%s</a>\n", textHome.text(), initialPath.text(), initialPath.text());
    html += htmlline.text();
    htmlline.printf("%s %s\n", textPath.text(), currentPath.text());
    html += htmlline.text();
    if(strcmp(initialPath.text(),currentPath.text()) != 0)
    {
      htmlline.printf("<a class=\"rlhtmldirup\" href=\"ftp://rlhtmldirup/dir/up\">..</a>\n");
      html += htmlline.text();
    }  
  } 
  spawn.spawn(cmd.text());
  while(1)
  {
    line = spawn.readLine();
    // printf("line=%s", line);
    if(line == NULL) break;
    if(strncmp(line,"#EOF#",5) == 0) break;
    file = line;
    file.removeNewline();
    cptr = strstr(file.text()," [");
    if(cptr != NULL)
    {
      if     (strcmp(cptr," [d]") == 0)
      {
        // list = 1 (list files and directories)
        // list = 2 (list directories only)
        // list = 3 (list files only)
        if(list_what != list_files_only) 
        {
          if(cptr != NULL) *cptr = '\0';
          if(strcmp(initialPath.text(),file.text()) != 0 && strcmp(currentPath.text(),file.text()) != 0 )
          {
            cptr = strrchr(file.text(),'/');
            if(cptr == NULL) cptr = file.text();
            cptr++;
            if(recursive) htmlline.printf("+<a  class=\"rlhtmldir\" href=\"ftp://rlhtmldir%s\">%s</a>\n", file.text(), file.text());
            else          htmlline.printf("+<a  class=\"rlhtmldir\" href=\"ftp://rlhtmldir%s\">%s</a>\n", file.text(), cptr);
            html += htmlline.text();
          }
        }  
      }
      else if(strcmp(cptr," [f]") == 0)
      {
        // list = 1 (list files and directories)
        // list = 2 (list directories only)
        // list = 3 (list files only)
        if(list_what != list_directories_only) 
        {
          if(cptr != NULL) *cptr = '\0';
          cptr = strrchr(file.text(),'/');
          if(cptr == NULL) cptr = file.text();
          cptr++;
          if(hide_hidden_files == 0 || (hide_hidden_files == 1 && strncmp(cptr,".",1) != 0))
          {
            if(recursive) htmlline.printf(" <a class=\"rlhtmlfile\" href=\"ftp://rlhtmlfile%s\">%s</a>\n", file.text(), file.text());
            else          htmlline.printf(" <a class=\"rlhtmlfile\" href=\"ftp://rlhtmlfile%s\">%s</a>\n", file.text(), cptr);
            html += htmlline.text();
          }  
        }  
      }
    }
  }  

  html += "</pre>\n";
  html += endHtml.text();
          
  if(generate_html_header_and_trailer)
  {
    html += htmlTrailer();
  }

  //printf("HTML=\n%s", html.text());
  return html.text();
}

int rlHtmlDir::getFilenames(rlSpreadsheetRow *row)
{
  if(row == NULL) return 0;
  rlSpawn spawn;
  rlString cmd;
  const char *line;
  rlString file;
#ifdef RLWIN32
  if(recursive) cmd.printf("rlfind \"%s\" -name \"%s\"", currentPath.text(), pattern.text());
  else          cmd.printf("rlfind \"%s\" -only \"%s\"", currentPath.text(), pattern.text());
#else
  if(recursive) cmd.printf("sh -c \"rlfind '%s' -name '%s' | sort\"", currentPath.text(), pattern.text());
  else          cmd.printf("sh -c \"rlfind '%s' -only '%s' | sort\"", currentPath.text(), pattern.text());
#endif
  spawn.spawn(cmd.text());
  row->clear();
  int i=1;
  while(1)
  {
    line = spawn.readLine();
    if(line == NULL) break;
    file = line;
    if(hide_hidden_files == 0 || (hide_hidden_files == 1 && strncmp(file.text(),".",1) != 0))
    {      
      file.removeNewline();
      row->setText(i++,file.text());
      //printf("file=%s\n", file.text());
    }  
  }  
  return i-1;
}

