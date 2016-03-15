/***************************************************************************
                          rlhtmldir.h  -  description
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
#ifndef _RL_HTMLDIR_H_
#define _RL_HTMLDIR_H_

#include "rlhtml.h"
#include "rlspreadsheet.h"

/*! <pre>
class for a file selector with html
</pre> */
class rlHtmlDir : public rlHtml
{
public:
  enum
  {
    list_files_and_directories = 1,
    list_directories_only = 2,
    list_files_only = 3
  }WHAT_TO_LIST;

  rlHtmlDir();
  virtual ~rlHtmlDir();

  /*! <pre>
  dir := ".." | "absolute_path" | "relative_path"
  </pre> */
  int cd(const char *dir, int is_initial_dir=0);

  /*! <pre>
  html can be configured with rlhtml.css
  </pre> */
  const char *getHtml(int generate_html_header_and_trailer=1);

  /*! <pre>
  list of filenames found
  return: number of files found
  </pre> */
  int getFilenames(rlSpreadsheetRow *row);

  rlString initialPath;       // initial path
  rlString currentPath;       // current path
  rlString pattern;           // search pattern for rlfind, default *
  int recursive;              // 0|1
  int list_what;              // enum WHAT_TO_LIST
  int hide_hidden_files;      // 0|1
  rlString textHome;          // default Home:
  rlString textPath;          // default Path:
  rlString textrlHtmlDirCSS;  // default rlhtml.css

  rlString startHtml;         // default html is empty
  rlString endHtml;           // default html is empty
private:
  rlString html;              // header + startHtml + rlfind_results + endHtml + trailer
};

#endif
