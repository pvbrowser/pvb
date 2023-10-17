/***************************************************************************
                          rljson.cpp -  description
                             -------------------
    begin                : Sat Jul 29 2023
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
#include "rljson.h"

rlJSON::rlJSON()
{
}

rlJSON::~rlJSON()
{
}

int rlJSON::initRecord(rlJSONrecord *record)
{
  record->startsWithText = 0;         // "text"
  record->startsWithOpenBracket = 0;  // {
  record->startsWithCloseBracket = 0; // }
  record->startsWithEndArray = 0;     // ]
  record->text.setText("");           // white space in front of the tag name is ignored
  record->optColon = 0;               // a colon must follow when text is a tag
  record->optOpenBracket = 0;         // a openBracket { might follow
  record->optBeginArray = 0;          // a beginArray [ might follow
  record->optParamType = 0;           // 0=default=text 1=integer 2=float
  record->param.setText("");          // the parameter in text form
  record->intParam = 0;               // 0   if there is no integer number
  record->floatParam = 0.0f;          // 0.0 if there is no floating point number
  record->optComma = 0;               // tells us that more elements follow
  return 0;
}

int rlJSON::interpret(const char *line, rlJSONrecord *record)
{
  if(line == NULL) return -1;
  if(record == NULL) return -1;
  initRecord(record);

  int i=0;
  while(line[i] == ' ') i++;
  if     (line[i] == '"') record->startsWithText = 1;
  else if(line[i] == '{') record->startsWithOpenBracket = 1;
  else if(line[i] == '}') record->startsWithCloseBracket = 1;
  else if(line[i] == ']') record->startsWithEndArray = 1;

  if(record->startsWithText)
  {
    i++;
    rlString text(&line[i]);
    char *start = text.text();
    char *end = strchr(start,'"');
    if(end != NULL)
    {
      *end = '\0';
      record->text = start;
      end++;
      start = end;
      start = strchr(start,'"');
      if(start != NULL)
      {
        start++;
        end = strchr(start,'"');
        if(end != NULL)
        {
          *end = '\0';
          record->param = start;
        }
      }
    }  
  }  
  else if(record->startsWithOpenBracket == 1)
  {
  }
  else if(record->startsWithCloseBracket == 1)
  {
  }
  else if(record->startsWithEndArray == 1)
  {
  }
  else // this must be either an empty line or an integer or float parameter
  {
    if(strchr(line,'.') != NULL)
    { 
      record->optParamType = 2; // float
      record->param = &line[i];
      char *cptr = record->param.text();
      cptr = strchr(cptr,',');
      if(cptr != NULL) *cptr = '\0';
    }
    else
    {
     if(line[i] >= '0' && line[i] <= '9') record->optParamType = 1; // integer
     else if(line[i] == '-')              record->optParamType = 1; // integer
     else if(line[i] == '+')              record->optParamType = 1; // integer
     if(record->optParamType == 1) record->param = &line[i];
     char *cptr = record->param.text();
     cptr = strchr(cptr,',');
     if(cptr != NULL) *cptr = '\0';
    }
  }

  if(strchr(line,':') != NULL) record->optColon = 1;       // a colon must follow when text is a tag
  if(strchr(line,'{') != NULL) record->optOpenBracket = 1; // a openBracket { might follow
  if(strchr(line,'[') != NULL) record->optBeginArray = 1;  // a beginArray [ might follow
  if(strchr(line,',') != NULL) record->optComma = 1;       // tells us that more elements follow

  return 0;
}

int rlJSON::print(FILE *fout, rlJSONrecord *record, const char *indent)
{
  if(record == NULL) return -1;
  if(indent != NULL) printf("%s",indent);
  if(record->startsWithText)
  {
    fprintf(fout,"\"%s\"",record->text.text());
    if(record->optOpenBracket == 0 && record->optBeginArray == 0)
    {
      if(record->optColon == 1)
      {
        fprintf(fout,": ");
        if(record->optParamType == 0) fprintf(fout,"\"%s\"",record->param.text());
        else                          fprintf(fout,"%s",record->param.text());
        if(record->optComma == 1) fprintf(fout,",");
        fprintf(fout,"\n");
      }
      if(record->optColon == 0)
      {
        if(record->optComma == 1) fprintf(fout,",");
        fprintf(fout,"\n");
      }
    }  
    else if(record->optOpenBracket == 1)
    {
      if(record->optColon == 1) fprintf(fout,":");
      fprintf(fout," ");
      if(record->optParamType == 0) fprintf(fout,"\"%s\"",record->param.text());
      else                          fprintf(fout,"%s",record->param.text());
      fprintf(fout," {");
      fprintf(fout,"\n");
    }
    else if(record->optBeginArray == 1)
    {
      if(record->optColon == 1) fprintf(fout,":");
      fprintf(fout," [\n");
    }
  }  
  else if(record->startsWithOpenBracket == 1)
  {
    fprintf(fout,"{"); 
    fprintf(fout,"\n");
  }
  else if(record->startsWithCloseBracket == 1)
  {
    fprintf(fout,"}"); 
    if(record->optComma == 1) fprintf(fout,",");
    fprintf(fout,"\n");
  }
  else if(record->startsWithEndArray == 1)
  {
    fprintf(fout,"]"); 
    if(record->optComma == 1) fprintf(fout,",");
    fprintf(fout,"\n");
  }
  return 0;
}

#define TESTING
#ifdef TESTING
int main()
{
  printf("Test rljson begin:\n");
  rlJSON rljson;
  rlJSONrecord record;
  int ret = rljson.initRecord(&record);
  printf("rljson.initRecord ret=%d\n", ret);
  
  record.startsWithText = 1;
  record.text.setText("id");
  record.optColon = 1;
  record.optOpenBracket = 1;
  record.optParamType = 1;   // 0=default=text 1=integer 2=float
  record.param.setText("overture:places:place:1");
  record.optComma = 0;

  printf("startsWithText=%d\n", record.startsWithText);
  printf("startsWithOpenBracket=%d\n", record.startsWithOpenBracket);
  printf("startsWithCloseBracket=%d\n", record.startsWithCloseBracket);
  printf("startsWithEndArray=%d\n", record.startsWithEndArray);
  printf("optColon=%d\n", record.optColon);
  printf("optOpenBracket=%d\n", record.optOpenBracket);
  printf("optBeginArray=%d\n", record.optBeginArray);
  printf("optComma=%d\n", record.optComma);
  printf("optParamType=%d (0=default=text 1=integer 2=float)\n", record.optParamType);

  rljson.print(stdout,&record);
  printf("rljson.print ret=%d\n", ret);
  
  FILE *fin = fopen("/home/lehrig/temp/json1.txt","r");
  if(fin == NULL)
  {
    printf("could not open /home/lehrig/temp/json1.txt\n");
  }
  else
  {
    char line[1024];
    while(fgets(line,1023,fin) != NULL)
    {
      printf("json1.txt: %s", line);
      rljson.interpret(line, &record);
      printf("DEBUG: text=%s param=%s withText=%d\n"
             "startsWith: openBracket=%d closeBracket=%d endArray=%d\n"
             "opt: colon=%d openBracket=%d beginArray=%d paramType=%d comma=%d\n", 
             record.text.text(), record.param.text(), record.startsWithText, 
             record.startsWithOpenBracket, record.startsWithCloseBracket, record.startsWithEndArray,
             record.optColon, record.optOpenBracket, record.optBeginArray, record.optParamType, record.optComma
          );
      /*
      int startsWithText;         // "text"
      int startsWithOpenBracket;  // {
      int startsWithCloseBracket; // }
      int startsWithEndArray;     // ]
      rlString text;      // white space in front of the tag name is ignored
      int optColon;       // a colon must follow when text is a tag
      int optOpenBracket; // a openBracket { might follow
      int optBeginArray;  // a beginArray [ might follow
      int optParamType;   // 0=default=text 1=integer 2=float
      rlString param;     // the parameter in text form
      int   intParam;     // 0   if there is no integer number
      float floatParam;   // 0.0 if there is no floating point number
      int optComma;       // tells us that more elements follow
      */
    }
  }  
  fclose(fin);
  return 0;
}
#endif

