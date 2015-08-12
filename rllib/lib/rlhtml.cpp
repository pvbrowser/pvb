/***************************************************************************
                           rlhtml.cpp  -  description
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
#include "rlhtml.h"
#include "rlspawn.h"
#include "rlfileload.h"
#include "rlcutil.h"

rlHtml::rlHtml()
{
}

rlHtml::~rlHtml()
{
}

const char * rlHtml::htmlHeader()
{
  return "<html>\n"
         "<head>\n"
         "  <meta charset=\"utf-8\"> <title>rlHtmlDir</title>\n"
         "  <link rel=\"stylesheet\" type=\"text/css\" href=\"rlhtml.css\">\n"
         "</head>\n"
         "<body id=\"rlhtmlbody\">\n";
}

const char * rlHtml::htmlTrailer()
{
  return "</body>\n"
         "</html>\n";
}

int rlHtml::textFile(const char *filename, rlString &text)
{
  struct stat statbuf;
  rlStat(filename, &statbuf);

  char tmp[4096];
  int  maxlen = sizeof(tmp);
  maxlen -= 8;
  rlFileLoad fl;
  if(fl.load(filename) < 0) return -1;
  text  = htmlHeader();
  sprintf(tmp,"<p>File=%s Size=%ldByte, %ldkByte</p><hr>\n", filename, statbuf.st_size, statbuf.st_size/(1024));  
  text += tmp;
  text += "<pre id=\"rlTextFile\">\n";
  const char *line = fl.firstLine();
  while(line != NULL)
  {
    const char *source = line;
    char       *dest   = &tmp[0];
    int i = 1;
    while(*source != '\0' && i<maxlen)
    {
      i++;
      if(*source == '<')
      {
        source++;
        *dest++ = '&';
        *dest++ = 'l';
        *dest++ = 't';
        *dest++ = ';';
      }
      else if(*source == '>')
      {
        source++;
        *dest++ = '&';
        *dest++ = 'g';
        *dest++ = 't';
        *dest++ = ';';
      }
      else
      {
        *dest++ = *source++;
      }
    }
    *dest = '\0';
    strcat(dest,"\n");
    text += tmp;
    line = fl.nextLine();
  }
  text += "</pre>\n";
  text += htmlTrailer();
  return 0;
}


int rlHtml::hexdumpFile(const char *filename, rlString &text)
{
  char tmp[4096];
  int  maxlen = sizeof(tmp);
  maxlen -= 8;
  rlSpawn sp;
  rlString cmd;
  cmd.printf("od -A x -t x1z -v \"%s\"", filename);
  sp.spawn(cmd.text());

  text  = htmlHeader();
  text += "<pre id=\"rlHexdumpFile\">\n";
  const char *line;
  while((line = sp.readLine()) != NULL)
  {
    const char *source = line;
    char       *dest   = &tmp[0];
    int i = 1;
    while(*source != '\0' && i<maxlen)
    {
      i++;
      if(*source == '<')
      {
        source++;
        *dest++ = '&';
        *dest++ = 'l';
        *dest++ = 't';
        *dest++ = ';';
      }
      else if(*source == '>')
      {
        source++;
        *dest++ = '&';
        *dest++ = 'g';
        *dest++ = 't';
        *dest++ = ';';
      }
      else
      {
        *dest++ = *source++;
      }
    }
    *dest = '\0';
    text += tmp;
  }
  text += "</pre>\n";
  text += htmlTrailer();
  return 0;
}

