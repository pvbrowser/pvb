/***************************************************************************
                          rlinifile.h  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by Rainer Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_INI_FILE_H_
#define _RL_INI_FILE_H_

#include "rldefine.h"
#include "rlstring.h"

/*! <pre>
class for INI files as known from Windows.
</pre> */
class rlIniFile
{
public:
  rlIniFile();
  virtual ~rlIniFile();
  int read(const char *filename);
  int write(const char *filename);
  const char *filename();
  const char *text(const char *section, const char *name);
  void setText(const char *section, const char *name, const char *text);
  int  printf(const char *section, const char *name, const char *format, ...);
  void remove(const char *section);
  void remove(const char *section, const char *name);
  const char *firstSection();
  const char *nextSection();
  const char *firstName(const char *section);
  const char *nextName(const char *section);
  void setDefaultSection(const char *section);
  const char *defaultSection();
  /*! <pre>
  Use this method for translating text within your application.

  Example:
  ini->setDefaultSection("german");
  printf("german_text=%s\n", ini->i18n("text1","This is text1") );
  </pre> */
  const char *i18n(const char *tag, const char *default_text);
private:
  typedef struct _rlSectionName_
  {
    _rlSectionName_ *nextName;
    char            *name;
    char            *param;
  }rlSectionName;

  typedef struct _rlSection_
  {
    _rlSection_   *nextSection;
    rlSectionName *firstName;
    char          *name;
  }rlSection;

  void copyIdentifier(char *buf, const char *line);
  void copyName(char *buf, const char *line);
  void copyParam(char *buf, const char *line);
  void deleteSectionNames(rlSection *section);
  rlSection *_firstSection;
  int currentSection, currentName;
  rlString fname;
  rlString default_section;
};

#endif
