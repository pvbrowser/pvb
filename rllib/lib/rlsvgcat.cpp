/***************************************************************************
                          rlsvgcat.cpp  -  description
                             -------------------
    begin                : Tue Apr 09 2006
    copyright            : (C) 2006 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlsvgcat.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

rlSvgCat::rlSvgCat()
{
  fin = fout = 0;
  s = -1;
}

rlSvgCat::~rlSvgCat()
{
  close();
}

void rlSvgCat::close()
{
  if(s == -1)
  {
    if(fout != 0 && fout != stdout) fclose((FILE *) fout);
  }
  else
  {
    fflush((FILE *) fout);
  }
  if(fin != 0) fclose((FILE *) fin);
  fin = fout = 0;
}

int rlSvgCat::open(const char *infile, const char *outfile)
{
  fin = fout = NULL;
  fin = (void *) fopen(infile,"r");
  if(fin == 0)
  {
    printf("could not open %s\n",infile);
    return -1;
  }
  if(outfile != 0)
  {
    fout = (void *) fopen(outfile,"w");
    if(fout == 0)
    {
      printf("could not write %s\n",outfile);
      return -1;
    }
  }
  else
  {
    fout = (void *) stdout;
  }
  return 0;
}

int rlSvgCat::reopenSocket(const char *infile, int socket)
{
  s = socket;
  if(s < 0)
  {
    s = -1;
    return -1;
  }
  fin = fout = NULL;
  fin = (void *) fopen(infile,"r");
  if(fin == 0)
  {
    printf("could not open %s\n",infile);
    return -1;
  }
  fout = fdopen(s,"w");
  if(fout == NULL)
  {
    s = -1;
    return -1;
  }
  return 0;
}

void rlSvgCat::cat()
{
  while(fgets(line,sizeof(line)-1,(FILE *) fin) != 0)
  {
    catline();
  }
}

int rlSvgCat::outUntil(int i, const char *tag)
{
  FILE *out = (FILE *) fout;
  int len = strlen(tag);
  while(line[i] != '\0' && line[i] != '\n')
  {
    if(strncmp(&line[i],tag,len) == 0)
    {
      for(int it=0; it<len; it++)
      {
        fputc(line[i++],out);
      }
      if(line[i] == '<') i--;
      break;
    }
    else
    {
      fputc(line[i++],out);
    }
  }
  return i;
}

int rlSvgCat::outUntilEnd(int i)
{
  FILE *out = (FILE *) fout;
  while(line[i] != '\0' && line[i] != '\n')
  {
    if(line[i] == '>')
    {
      //xlehrig fputc(line[i++],out);
      fputc(line[i],out);
      break;
    }
    if(line[i] == ' ' || line[i] == '\t')
    {
      break;
    }
    else
    {
      fputc(line[i++],out);
    }
  }
  fputc('\n',out);
  return i;
}

int rlSvgCat::outValue(int i)
{
  //printf("outValue=%s",&line[i]);
  FILE *out = (FILE *) fout;
  while(line[i] != '\0' && line[i] != '\n')
  {
    if(line[i] == '<')
    {
      i--;
      break;
    }
    else if(line[i] == '>')
    {
      fputc('\n',out);
      fputc(line[i],out);
      break;
    }
    else if(line[i] == '/' && line[i+1] == '>')
    {
      fputc('\n',out);
      fputc(line[i++],out);
      fputc(line[i],out);
      break;
    }
    else
    {
      fputc(line[i++],out);
    }
  }
  return i;
}

void rlSvgCat::catline()
{
  int i = 0;
  FILE *out = (FILE *) fout;

  while(line[i] != '\0' && line[i] != '\n')
  {
    while(line[i] == ' ' || line[i] == '\t' ) i++; // eliminate leading spaces
    // compare tags
    if     (strncmp(&line[i],"<?",2) == 0)
    {
      fputc(line[i++],out);
      fputc(line[i++],out);
      i = outUntil(i,"?>");
      fputc('\n',out);
    }
    else if(strncmp(&line[i],"<!",2) == 0) // comment
    {
      fprintf(out,"%s",&line[i]);
      if(strstr(&line[i],">") != NULL) return;
      while(fgets(line,sizeof(line)-1,(FILE *) fin) != 0)
      {
        fprintf(out,"%s",line);
        if(strstr(line,">") != NULL) return;
      }
      return;
    }
    else if(strncmp(&line[i],"<desc",5) == 0)
    {
      fprintf(out,"%s",&line[i]);
      return;
    }
    else if(strncmp(&line[i],"</",2) == 0)
    {
      if(strncmp(&line[i],"</svg>",6) == 0)
      {
        fprintf(out,"</svg>");
        return;
      }
      if(strncmp(&line[i],"</SVG>",6) == 0)
      {
        fprintf(out,"</SVG>");
        return;
      }
      fputc(line[i++],out);
      fputc(line[i++],out);
      i = outUntil(i,">");
      fputc('\n',out);
    }
    else if(strncmp(&line[i],"<",1) == 0)
    {
      fputc(line[i++],out);
      i = outUntilEnd(i);
    }
    else if(strncmp(&line[i],"/>",2) == 0)
    {
      fputc(line[i++],out);
      fputc(line[i++],out);
      fputc('\n',out);
    }
    else if(strncmp(&line[i],">",1) == 0)
    {
      fputc(line[i++],out);
      fputc('\n',out);
    }
    else
    {
      i = outValue(i);
      fputc('\n',out);
    }
    i++;
  }
}

