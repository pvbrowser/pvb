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
#include "rlstring.h"
#include "rlcutil.h"

rlString::rlString(const char *text)
{
  txt = new char [strlen(text)+1];
  ::strcpy(txt, text);
}

rlString::rlString(rlString &s2)
{
  txt = new char [strlen(s2.text())+1];
  ::strcpy(txt,s2.text());
}

rlString::rlString(rlString *s2)
{
  txt = new char [strlen(s2->text())+1];
  ::strcpy(txt,s2->text());
}

rlString::~rlString()
{
  delete [] txt;
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

