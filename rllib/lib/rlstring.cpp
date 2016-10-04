/***************************************************************************
                          rlmailbox.cpp  -  description
                             -------------------
    begin                : Wed Jan 02 2008
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

#include <stdarg.h>
#include <sys/stat.h>

#include "rlstring.h"
#include "rlcutil.h"

const char rlCRLF[3] = {0x0d, 0x0a, 0};

rlString::rlString(const char *text)
{
  txt = new char [strlen(text)+1];
  tmp = NULL;
  ::strcpy(txt, text);
}

rlString::rlString(rlString &s2)
{
  txt = new char [strlen(s2.text())+1];
  tmp = NULL;
  ::strcpy(txt,s2.text());
}

rlString::rlString(rlString *s2)
{
  txt = new char [strlen(s2->text())+1];
  tmp = NULL;
  ::strcpy(txt,s2->text());
}

rlString::rlString(const rlString &s2)
{
  txt = new char [strlen(s2.text())+1];
  tmp = NULL;
  ::strcpy(txt,s2.text());
}


rlString::~rlString()
{
  delete [] txt;
  if(tmp != NULL) delete [] tmp;
}

rlString& rlString::operator=(const char *s2)
{
  this->setText(s2);
  return *this;
}

rlString& rlString::operator=(rlString &s2)
{
  this->setText(s2.text());
  return *this;
}

rlString& rlString::operator+(const char *s2)
{
  this->cat(s2);
  return *this;
}

rlString& rlString::operator+(rlString &s2)
{
  this->cat(s2.text());
  return *this;
}

rlString& rlString::operator+=(const char *s2)
{
  this->cat(s2);
  return *this;
}

rlString& rlString::operator+=(rlString &s2)
{
  this->cat(s2.text());
  return *this;
}

int rlString::operator==(const char *s2)
{
  if(strcmp(txt,s2) == 0) return 1;
  return 0;
}

int rlString::operator==(rlString &s2)
{
  if(strcmp(txt,s2.text()) == 0) return 1;
  return 0;
}

int rlString::operator==(const rlString &s2)
{
  if(strcmp(txt,s2.text()) == 0) return 1;
  return 0;
}

int rlString::operator!=(const char *s2)
{
  if(strcmp(txt,s2) != 0) return 1;
  return 0;
}

int rlString::operator!=(rlString &s2)
{
  if(strcmp(txt,s2.text()) != 0) return 1;
  return 0;
}

char * rlString::text()
{
  return txt;
}

char * rlString::text() const
{
  return txt;
}

int rlString::setText(const char *text)
{
  int ret = strlen(text);
  delete [] txt;
  txt = new char [ret+1];
  ::strcpy(txt, text);
  return ret;
}

int rlString::printf(const char *format, ...)
{
  int ret;
  char mystring[rl_PRINTF_LENGTH]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(mystring, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  delete [] txt;
  txt = new char [strlen(mystring)+1];
  ::strcpy(txt, mystring);
  return ret;
}

int rlString::strcpy(const char *text)
{
  char *cptr = txt;
  int  len = strlen(text);
  txt = new char [len+1];
  ::strcpy(txt, text);
  delete [] cptr;
  return len;
}

int rlString::cat(const char *text)
{
  char *cptr = txt;
  int  len = strlen(txt) + strlen(text);
  txt = new char [len+1];
  ::strcpy(txt, cptr);
  ::strcat(txt, text);
  delete [] cptr;
  return len;
}

char *rlString::strstr(const char *substring)
{
  if(substring == NULL) return NULL;
  return ::strstr(txt,substring);
}

int rlString::upper()
{
  return ::rlupper(txt);
}

int rlString::lower()
{
  return ::rllower(txt);
}

int rlString::startsWith(const char *startstr)
{
  return ::rlStartsWith(txt,startstr);
}

int rlString::strnocasecmp(const char *other)
{
  rlString my,o;
  my.setText(txt);
  my.lower();
  o.setText(other);
  o.lower();
  return ::strcmp(my.text(),o.text());
}

int rlString::strnnocasecmp(const char *other, int n)
{
  rlString my,o;
  my.setText(txt);
  my.lower();
  o.setText(other);
  o.lower();
  return ::strncmp(my.text(),o.text(),n);
}

char *rlString::strchr(int c)
{
  return ::strchr(txt,c);
}

char *rlString::strrchr(int c)
{
  return ::strrchr(txt,c);
}

int rlString::removeQuotas(char q)
{
  char c;
  int  state=0, inquotas=0, j=0;
  int  len = strlen(txt);

  for(int i=0;i<len;i++) 
  {
    c = txt[i];
    switch(state) 
    {
      case 0: //START
        if(c==q) 
        {
          state=1;
          inquotas=1;
          break;
        }
        state=1;
      case 1: // BODY
        if(inquotas==1) 
        {
          if(c==q) 
          {
             inquotas=0;
             break;
          }
        }
        txt[j++]=c;
        break;
      default:
        break;
    }
  }
  txt[j++]=0;
  return j;
}

int rlString::removeNewline()
{
  if(txt == NULL) return -1;
  char *cptr = ::strchr(txt,'\n');
  if(cptr != NULL) *cptr = '\0';
  return 0;
}

int rlString::read(const char *filename)
{
  struct stat statbuf;
#ifdef RLUNIX
  if(lstat(filename,&statbuf)) return -1;
#else
  if(stat(filename,&statbuf)) return -1;
#endif
  FILE *fin = fopen(filename,"r");
  if(fin == NULL) return -1;
  if(txt != NULL) delete [] txt;
  txt = new char [statbuf.st_size+1];
  fread(txt, 1, statbuf.st_size, fin);
  txt[statbuf.st_size] = '\0';
  fclose(fin);
  return statbuf.st_size; 
}

int rlString::write(const char *filename)
{
  if(txt == NULL) return -1;
  FILE *fout = fopen(filename,"w");
  if(fout == NULL) return -1;
  int len = strlen(txt);
  fprintf(fout,"%s",txt);
  fclose(fout);
  return len;
}

const char *rlString::toFilename()
{
#ifdef RLUNIX
  int len = ::strlen(txt);
  if(tmp != NULL) delete [] tmp;
  tmp = new char [len+1];
  ::strcpy(tmp,txt);
  return tmp;
#endif
#ifdef RLWIN32
  char *cptr = &txt[0];
  if(*cptr == '/') cptr++;
  int len = ::strlen(cptr);
  if(tmp != NULL) delete [] tmp;
  tmp = new char [len+1];
  ::strcpy(tmp,cptr);
  cptr = &tmp[0];
  while(*cptr != '\0')
  {
    if(*cptr == '/') *cptr = '\\';
    cptr++;
  }
  return tmp;
#endif
#ifdef __VMS
  char *cptr = &txt[0];
  if(*cptr == '/') cptr++;
  int len = ::strlen(cptr);
  if(tmp != NULL) delete [] tmp;
  tmp = new char [len+1];
  ::strcpy(tmp,cptr);
  cptr = ::strchr(&tmp[0],'/');
  if(cptr != NULL) *cptr = '[';
  cptr = ::strrchr(&tmp[0],'/');
  if(cptr != NULL) *cptr = ']';
  cptr = &tmp[0];
  while(*cptr != '\0')
  {
    if(*cptr == '/') *cptr = '.';
    cptr++;
  }
  return tmp;
#endif
}

const char *rlString::toDirname()
{
#ifdef RLUNIX
  int len = ::strlen(txt);
  if(tmp != NULL) delete [] tmp;
  tmp = new char [len+1];
  ::strcpy(tmp,txt);
  return tmp;
#endif
#ifdef RLWIN32
  char *cptr = &txt[0];
  if(*cptr == '/') cptr++;
  int len = ::strlen(cptr);
  if(tmp != NULL) delete [] tmp;
  tmp = new char [len+1];
  ::strcpy(tmp,cptr);
  cptr = &tmp[0];
  while(*cptr != '\0')
  {
    if(*cptr == '/') *cptr = '\\';
    cptr++;
  }
  return tmp;
#endif
#ifdef __VMS
  char *cptr = &txt[0];
  if(*cptr == '/') cptr++;
  int len = ::strlen(cptr);
  if(tmp != NULL) delete [] tmp;
  tmp = new char [len+2];
  ::strcpy(tmp,cptr);
  cptr = ::strchr(&tmp[0],'/');
  if(cptr != NULL) *cptr = '[';
  cptr = &tmp[0];
  while(*cptr != '\0')
  {
    if(*cptr == '/') *cptr = '.';
    cptr++;
  }
  ::strcat(tmp,"]");
  return tmp;
#endif
}

