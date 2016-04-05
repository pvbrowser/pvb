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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

rlSvgCat::rlSvgCat()
{
  fin = fout = 0;
  s = -1;
  line = NULL;
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

  if(infile == NULL) fin = stdin;
  else               fin = (void *) fopen(infile,"r");
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
  if(infile == NULL) fin = stdin;
  else               fin = (void *) fopen(infile,"r");
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

int  rlSvgCat::sendOverSocket(char *line_in, int s)
{
  if(reopenSocket(NULL,s) != 0) return -1;
  catline(line_in);
  return 0;
}

void rlSvgCat::cat()
{
  line = new char [256*256];
  while(fgets(line,256*256-1,(FILE *) fin) != 0)
  {
    catline(line);
  }
  delete [] line;
  line = NULL;
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
  int found_equal = 0;
  int found_quotes = 0;
  int quote_count = 0;
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
    else if(line[i] == '=')
    {
      found_equal = 1;
      fputc(line[i++],out);
    }
    else if((line[i] == ' ' || line[i] == '\t') && found_equal==1)
    {
      if(found_quotes == 0 && line[i+1] != '/') fputc('\n',out);
      else if(found_quotes == 1)                fputc(' ',out);
      while(line[i] == ' ' || line[i] == '\t') i++;
      found_equal = 0;
    }
    else if(line[i] == '\"')
    {
      found_quotes++;
      quote_count++;
      if(found_quotes == 2) found_quotes = 0;
      fputc(line[i++],out);
    }
    else if((line[i] == ' ' || line[i] == '\t') && found_quotes == 0)
    {
      if(quote_count > 0)
      {
        fputc('\n',out);
        i++;
      }
      else
      {
        fputc(line[i++],out);
      }
    }
    else if(line[i] == '\\' && line[i+1]=='\n') //may2013 handle ...
    {
      if(fgets(line,sizeof(line)-1,(FILE *) fin) == 0) exit(0);
      i = 0; // ... line spanning over more than 1 line
    }
    else
    {
      fputc(line[i++],out);
    }
  }
  return i;
}

void rlSvgCat::catline(char *line_in)
{
  int i = 0;
  FILE *out = (FILE *) fout;
  line = line_in;

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
    /* removed after report by H. Schauerte
       normalize desc as well
    else if(strncmp(&line[i],"<desc",5) == 0)
    {
      fprintf(out,"%s",&line[i]);
      return;
    }
    */
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
      if(strncmp(&line[i],"</tspan>",8) == 0)
      {
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc('\n',out);
        i--;
      }
      else if(strncmp(&line[i],"</TSPAN>",8) == 0)
      {
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc(line[i++],out);
        fputc('\n',out);
        i--;
      }
      else
      {
        fputc(line[i++],out);
        fputc(line[i++],out);
        i = outUntil(i,">");
        fputc('\n',out);
      }  
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

int pipe2cpp(int ac, char **av)
{
  char *cptr;
  char line[256*256+1];
  char printf_string[256] = "vdi.svg_printf";
  char from[256] = "";
  char to[256] = "";
  for(int i=2; i<ac; i++)
  {
    char *arg = av[i];
    if(strncmp(arg,"-printf=",8) == 0)
    {
      strcpy(printf_string,&arg[8]);
    }
    else if(strncmp(arg,"-from=",6) == 0)
    {
      strcpy(from,&arg[6]);
    }
    else if(strncmp(arg,"-to=",4) == 0)
    {
      strcpy(to,&arg[4]);
    }
  }

  int doit = 0;
  if(from[0] == '\0') doit = 1;
  while(fgets(line,255*255,stdin) != NULL)
  {
    if(from[0] != '\0' && strstr(line,from) != NULL) doit = 1;

    if(doit)
    {
      printf("  %s(\"", printf_string);
      cptr = &line[0];
      while(*cptr != '\0')
      {
        if(*cptr == '"')
        {
          printf("\\\"");
        }
        else if(*cptr == '\\')
        {
          printf("\\\\");
        }
        else if(*cptr == '\n')
        {
        }
        else
        {
          printf("%c",*cptr);
        }
        cptr++;
      }
      printf("\\n\");\n");
    }  

    if(to[0]   != '\0' && strstr(line,to)   != NULL) doit = 0;
  }           
  return 0;
}

//#define RLSVGCAT_MAIN_OFF
#ifndef RLSVGCAT_MAIN_OFF
void print_usage()
{
  printf("usage: rlsvgcat file.svg <outputfile>\n");
  printf("       rlsvgcat -pipe2cpp <-printf=string> <-from=pattern> <-to=pattern>\n");
}

int main(int ac, char **av)
{
  rlSvgCat svgcat;

  if(ac > 1)
  {
    const char *arg = av[1];
    if(*arg == '-')
    {
      if(strcmp(arg,"-pipe2cpp") == 0)
      {
        pipe2cpp(ac,av);
      }
      else
      {
        print_usage();
      }
      return 0;
    }
  }

  if(ac == 1)
  {
    if(svgcat.open(NULL) == 0)
    {
      svgcat.cat();
      return 0;
    }
  }
  else if(ac == 2)
  {
    if(svgcat.open(av[1]) == 0)
    {
      svgcat.cat();
      return 0;
    }
  }
  else if(ac == 3)
  {
    if(svgcat.open(av[1],av[2]) == 0)
    {
      svgcat.cat();
      return 0;
    }
  }
  else
  {
    print_usage();
  }  
  return -1;
}
#endif
