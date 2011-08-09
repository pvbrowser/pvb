/***************************************************************************
                          rlreport.h  -  description
                             -------------------
    begin                : Sun Jul 03 2011
    copyright            : (C) 2011 pvbrowser
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_REPORT_H_
#define _RL_REPORT_H_

#include <stdio.h>
#include "rldefine.h"
#include "rlinifile.h"
#include "rlstring.h"

/*! <pre>
class for generating PDF files with LaTeX

//### typical usage begin  #############################################
int report(PARAM *p)
{
  rlReport r;
  rlString filename;
  filename  = p->file_prefix; // use an individual filename for each client
  filename += ".tex";
  r.open(filename.text());
  r.includeHeader("\\documentclass[a4paper]{article}","\\usepackage[ngerman]{babel}"); // german article on A4 paper
  //// here we may include our own header definitions

  r.beginDocument();
  //// --- begin here we use the methods: printf(), include(), includeCSV(), includeImage(), spawn() --------------
  r.printf("\\section{Teil 1}\n");
  r.printf("Hallo Welt\n");
  r.includeImage("test.jpg","Testbild",0.8f);
  r.includeCSV("test.csv",1,"Test CSV");
  r.printf("\\cppbegin{main.cpp}\n");
  r.include("main.cpp");
  r.printf("\\end{lstlisting}\n");
  r.printf("\\simplecodebegin{Verzeichnis Inhalt}\n");
  r.spawn("ls -al");
  r.printf("\\end{lstlisting}\n");
  //// --- end here we use the methods: printf(), include(), includeCSV(), includeImage(), spawn() ----------------
  r.endDocument();
  r.close();

  r.pdflatex(); // pdflatex -interaction=nonstopmode file.tex

  filename  = p->file_prefix;
  filename += ".pdf";
  pvDownloadFileAs(p,filename.text(), "report.pdf");
  pvClientCommand(p,"pdf","report.pdf"); // open report.pdf on the client using the pdf-viewer
  return 0;
}
//### typical usage end  ###############################################
</pre> */
class rlReport
{
public:

  rlReport();
  virtual ~rlReport();

  /*! <pre>
      open the output file for writeing
  </pre> */
  int open(const char *filename);

  /*! <pre>
      close the output file
  </pre> */
  int close();
  
  /*! <pre>
      print text to the output file
  </pre> */
  int printf(const char *format, ...);

  /*! <pre>
      print "\\begin{document}"  to the output file
  </pre> */
  int beginDocument();

  /*! <pre>
      print "\\end{document}"  to the output file
  </pre> */
  int endDocument();

  /*! <pre>
      Include a file to the output.
      If ini != NULL then the content of ini can be used as text modules as follows:
      Withnin file we use something like \\$[SECTION][NAME] to address the text module we want to include.
  </pre> */
  int include(const char *filename, rlIniFile *ini=NULL);

  /*! <pre>
      Include the LaTeX header to the output file.
      If optional_parameter == NULL then output the default parameters.
      Examples:
        includeHeader("\\usepackage[a4paper]{article}", "\\usepackage[ngerman]{babel}");
        includeHeader("\\usepackage[a4paper]{book}",    "\\usepackage[english,greek]{babel}");
      See:
        http://en.wikibooks.org/wiki/LaTeX/Internationalization
      Our default LaTeX header is within the fprintf() statements of rlReport::includeHeader()
        You may printf() or include() more header definitions and then call
        r.beginDocument();
  </pre> */
  int includeHeader(const char *documentclass = "\\documentclass[a4paper]{article}", 
                    const char *language      = "\\usepackage[english]{babel}", 
                    const char *inputenc      = "\\usepackage[utf8]{inputenc}",
                    const char *layout        = "\\setlength{\\parindent}{0pt} \\setlength{\\topmargin}{-50pt} \\setlength{\\oddsidemargin}{0pt} \\setlength{\\textwidth}{480pt} \\setlength{\\textheight}{700pt}");

  /*! <pre>
      print a CSV table  to the output file
      filename := name.csv
      use_first_row_as_title := 0 | 1
      legend := NULL | text_describing_the_table
  </pre> */
  int includeCSV(const char *filename, int use_first_row_as_title=1, const char *legend=NULL, char delimitor='\t');

  /*! <pre>
      Include a graphic into our document.
      filename := name.jpg | name.png
      legend := NULL | text_describeing_the_image
  </pre> */
  int includeImage(const char *filename, const char *legend=NULL, float scale=1.0f);

  /*! <pre>
      Run the external "command" and include what is printed by "command" to the output file.
      This works as follows:
        sprintf(cmd,"%s > %s.temp", command, file.text())
        system(cmd);
        include(file.text() + ".temp");
  </pre> */
  int spawn(const char *command);
  /*! <pre>
      Run "pdflatex -interaction=nonstopmode file.tex" if command==NULL or
      what you specify by your own command.
  </pre> */
  int pdflatex(const char *command = NULL);
  
private:
  FILE *fout;    // output file pointer
  rlString file; // name of the output file
};

#endif

