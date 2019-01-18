/***************************************************************************
                          rlinifile.h  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by R. Lehrig
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
  const char *i18n(const char *tag, const char *default_text="");
  /*! <pre>
  Use this method for translating text within your application.

  Example:

  #define TR(txt) d->translator.tr(txt)

  typedef struct // (todo: define your data structure here)
  {
        rlIniFile translator;
  }
  DATA;

  static int slotInit(PARAM *p, DATA *d)
  {
    if(p == NULL || d == NULL) return -1;
    d->translator.read("text.ini");
    d->translator.setDefaultSection("DEUTSCH");
    printf("test1=%s\n", TR("umlaute"));
    printf("test2=%s\n", TR("Xumlaute"));
    d->translator.setDefaultSection("ENGLISH");
    printf("test1=%s\n", TR("umlaute"));
    printf("test2=%s\n", TR("Xumlaute"));
  }

  With text.ini:
  [DEUTSCH]
  hello=Hallo
  world=Welt
  umlaute=äöüß

  [ENGLISH]
  hello=Hello
  world=World
  umlaute=german_umlaute=äöüß
  </pre> */
  const char *tr(const char *txt);
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

int rlSetTranslator(const char *language, const char *inifile=NULL);
#ifndef SWIGPYTHON
const char *rltranslate(const char *txt, char **mytext=NULL);
const char *rltranslate2(const char *section, const char *txt, char **mytext=NULL);
#define rltr(txt) rltranslate(txt)

#ifdef  pvtr
#undef  pvtr
#endif
#define pvtr(txt) rltranslate2(p->lang_section,txt,&p->mytext2)
#endif

#endif
