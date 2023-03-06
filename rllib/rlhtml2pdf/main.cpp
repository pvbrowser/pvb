/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mar 2016
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
#include "main.h"
int debug = 0;

void applyFilter(char *line, const char *tableopt, const char *imgopt)
{
  rlString lbuf(line);
  char *cptr = lbuf.text();
  int idest = 0;

  while(*cptr != '\0')
  {
    if(strncmp(cptr,"<table",6) == 0 || strncmp(cptr,"<TABLE",6) == 0)
    {
      while(*cptr != '>' && *cptr != '\0')
      {
        line[idest++] = *cptr++;
      }
      line[idest++] = ' ';
      int isource = 0;
      while(tableopt[isource] != '\0') line[idest++] = tableopt[isource++];
      line[idest++] = ' ';
      line[idest++] = *cptr;
      line[idest]   = '\0';
      if(debug) printf("applyFilter1: line=%s tableopt=%s\n", line, tableopt);
      cptr++;
    }
    else if(strncmp(cptr,"<img",4) == 0 || strncmp(cptr,"<IMG",4) == 0)
    {
      while(*cptr != '\\' && *cptr != '>' && *cptr != '\0')
      {
        line[idest++] = *cptr++;
      }
      line[idest++] = ' ';
      int isource = 0;
      while(imgopt[isource] != '\0') line[idest++] = imgopt[isource++];
      line[idest++] = ' ';
      line[idest++] = *cptr;
      line[idest]   = '\0';
      if(debug) printf("applyFilter2: line=%s imgopt=%s\n", line, imgopt);
      cptr++;
    }
    else
    {
      line[idest++] = *cptr++;
      line[idest]   = '\0';
    }
  }
}

int readInput(rlString *html, rlString *inputfilename, rlString *tableopt, rlString *imgopt)
{
  char line[4096],*cptr1,*cptr2;
  FILE *fin;
  if(strcmp(inputfilename->text(),"-") == 0)
  {
    fin = stdin;
  }
  else
  {
    fin = fopen(inputfilename->text(),"r");
    if(fin == NULL)
    {
      printf("could not read file %s\n", inputfilename->text());
      return -1;
    }
  }

  int filter = 1;
  if(strlen(tableopt->text()) == 0 && strlen(imgopt->text()) == 0) filter = 0;

  while(fgets(line, sizeof(line)-128, fin) != NULL)
  {
    if(filter)
    {
      cptr1 = strstr(line,"<table");
      cptr2 = strstr(line,"<img");
      if(cptr1 != NULL || cptr2 != NULL)
      {
        applyFilter(line,tableopt->text(),imgopt->text());
      }
    }
    *html += line;
  }

  if(strcmp(inputfilename->text(),"-") != 0) fclose(fin);
  return 0;
  //if(strlen(tableopt.text()) == 0 && strlen(imgopt.text()) == 0)
  //{
  //  html->read(inputfilename->text());
  //}  
}

int main(int argc, char **argv)
{
  rlString inputfilename;
  rlString outputfilename("rlhtml2pdf_output.pdf");
  rlString printername;
  rlString defaultfont;
  rlString defaultstyle;
  rlString tableopt;
  rlString imgopt;
  float margin      = -1.0f;
  float indentwidth = -1.0f;
  float textwidth   = -1.0f;
  int   use_design_metrics = -1;
  float lm  = 0.0f;  
  float rm  = 0.0f;  
  float tm  = 0.0f;  
  float bm  = 0.0f;  
  int   ask = 0;
  int   ax  = 4;
  int   landscape = 0;

  QApplication a(argc, argv);
  for(int i=1; i<argc; i++)
  {
    char *arg = argv[i];
    if(*arg == '-')
    {
      if     (strncmp(arg,"-debug",6) == 0)
      {
        debug = 1;
      }
      else if(strncmp(arg,"-tableopt=",10) == 0)
      {
        tableopt = &arg[10];
      }
      else if(strncmp(arg,"-imgopt=",8) == 0)
      {
        imgopt = &arg[8];
      }
      else if(strncmp(arg,"-margin=",8) == 0)
      {
        float fval;
        sscanf(arg,"-margin=%f",&fval);
        margin = fval;
      }
      else if(strncmp(arg,"-indentwidth=",13) == 0)
      {
        float fval;
        sscanf(arg,"-indentwidth=%f",&fval);
        indentwidth = fval;
      }
      else if(strncmp(arg,"-textwidth=",11) == 0)
      {
        float fval;
        sscanf(arg,"-textwidth=%f",&fval);
        textwidth = fval;
      }
      else if(strncmp(arg,"-use_design_metrics=",20) == 0)
      {
        sscanf(arg,"-use_design_metrics=%d",&use_design_metrics);
      }
      else if(strncmp(arg,"-rm=",4) == 0)
      {
        float fval;
        sscanf(arg,"-rm=%f",&fval);
        rm = fval;
      }
      else if(strncmp(arg,"-tm=",4) == 0)
      {
        float fval;
        sscanf(arg,"-tm=%f",&fval);
        tm = fval;
      }
      else if(strncmp(arg,"-bm=",4) == 0)
      {
        float fval;
        sscanf(arg,"-bm=%f",&fval);
        bm = fval;
      }
      else if(strncmp(arg,"-ask",4) == 0)
      {
        ask = 1;
      }
      else if(strncmp(arg,"-list",5) == 0)
      {
#if QT_VERSION >= 0x050000
        QStringList printerlist = QPrinterInfo::availablePrinterNames();
        for(int i=0; i<printerlist.size(); i++)
        {
          printf("printername%d = %s\n", i, (const char *) printerlist.at(i).toUtf8());
        }
#endif        
        return 0;
      }  
      else if(strncmp(arg,"-font=",6) == 0)
      {
        defaultfont = &arg[6];
      }
      else if(strncmp(arg,"-css=",5) == 0)
      {
        defaultstyle.read(&arg[5]);
      }
      else if(strncmp(arg,"-ax=",4) == 0)
      {
        sscanf(arg,"-ax=%d",&ax);
      }
      else if(strncmp(arg,"-landscape",10) == 0)
      {
        landscape = 1;
      }
      else if(strcmp(arg,"-") == 0)
      {
        inputfilename = "-"; // stdin;
      }
    }
    else if(strlen(inputfilename.text()) == 0)
    {
      inputfilename = arg;
    }
    else
    {
      if(strstr(arg,".pdf") == NULL && strstr(arg,".PDF") == NULL)
      {
        printername = arg;
      }
      else
      {
        outputfilename = arg;
      }  
    }
  }
  if(strlen(inputfilename.text()) == 0)
  {
    printf("usage: rlhtml2pdf <-options> inputfilename <outputfilename|printername|defaultprinter>\n");
    printf("### global options ####################################################\n");
    printf("  -debug # switches on some printf(messages) for debugging\n");
    printf("  -      # a standalone - means input from stdin\n");
    printf("  # some HTML elements may be replaced globally before rendering to PDF\n");
    printf("  -imgopt=string   # Example: width=\"80%%\"\n");
    printf("  -tableopt=string # Example: align=\"center\" border=\"3\" cellpadding=\"8\" cellspacing=\"0\" width=\"50%%\">\n");
    printf("### QPrinter options ##################################################\n");
    printf("  -lm=%%f # mm left margin\n");
    printf("  -rm=%%f # mm right margin\n");
    printf("  -tm=%%f # mm top margin\n");
    printf("  -bm=%%f # mm bottom margin\n");
    printf("  -ask   # for which printer\n");
    printf("  -list  # available printers\n");
    printf("  -ax=%%d # where pagesize is:\n");
    printf("         # 0=A0, 10=B0\n");
    printf("         # 1=A1, 11=B1\n");
    printf("         # 2=A2, 12=B2\n");
    printf("         # 3=A3, 13=B3\n");
    printf("         # 4=A4, 14=B4\n");
    printf("         # 5=A5, 15=B5\n");
    printf("         # 6=A6, 16=B6\n");
    printf("         # 7=A7, 17=B7\n");
    printf("         # 8=A8, 18=B8\n");
    printf("         # 9=A9, 19=B9\n");
    printf("  -landscape  # default is portrait\n");
    printf("### QTextDocument options #############################################\n");
    printf("  -font=defaultfont      # family,pointSize,pixelSize,styleHint,weight,style,underline,strikeOut,fixedPitch,rawMode\n");
    printf("                         # Examples: -font=Times,10,-1,0,50,0,0,0,0,0\n");
    printf("    family,              # Times|Arial|Courier\n");
    printf("    pointSize,           # 10\n");
    printf("    pixelSize,           # -1\n");
    printf("    styleHint,           # 0\n");
    printf("    weight,              # 0=Thin 12=ExtraLight 25=Light 50=Normal 57=Medium 63=DemoBold 75=Bold 81=ExtraBold 87=Black50\n");
    printf("    style,               # 0=Normal 1=Italic 2=Oblique\n");
    printf("    underline,           # 0|1\n");
    printf("    strikeOut,           # 0|1\n");
    printf("    fixedPitch,          # 0|1\n");
    printf("    rawMode,             # 0|1\n");
    printf("  -css=defaultstyle.css  # The style sheet needs to be compliant to CSS 2.1 syntax.\n");
    printf("  -margin=%%f             # qt document margin\n");
    printf("  -indentwidth=%%f        # qt document indent width\n");
    printf("  -textwidth=%%f          # qt document text width\n");
    printf("  -use_design_metrics=%%d # 0|1 qt document use design metrics\n");
    return 0;
  }

  // eventually the user wants to print out a pdf file
  if(strstr(inputfilename.text(),".pdf") != NULL || strstr(inputfilename.text(),".PDF") != NULL)
  {
    if(strlen(printername.text()) > 0)
    {
      rlString cmd("lp -d ");
      cmd += printername.text();
      cmd += " ";
      cmd += inputfilename.text();
      return system(cmd.text());
    }
    else if(ask)
    {
      QPrinter printer(QPrinter::HighResolution); //create your QPrinter (don't need to be high resolution, anyway)
      QPrintDialog dialog(&printer);
      if(dialog.exec() != QDialog::Accepted)
      {
        return 0;
      }
      rlString cmd("lp -d ");
      cmd += printer.printerName().toUtf8();
      cmd += " ";
      cmd += inputfilename.text();
      if(debug) printf("cmd = %s\n", cmd.text());
      return system(cmd.text());
    }
    else
    {
      rlString cmd("lp ");
      cmd += inputfilename.text();
      return system(cmd.text());
    }
  }
  //QString defaultprinter = QPrinterInfo::defaultPrinterName();
  //printf("defaultprinter = %s\n", (const char *) defaultprinter.toUtf8());
  //QPrinter *printer = new QPrinter(QPrinterInfo::printerInfo(defaultprinter));

  QPrinter printer(QPrinterInfo::defaultPrinter(), QPrinter::HighResolution); //create your QPrinter (don't need to be high resolution, anyway)
  QPageSize pageSizeA0(QPageSize::PageSizeId::A0);
  QPageSize pageSizeA1(QPageSize::PageSizeId::A1);
  QPageSize pageSizeA2(QPageSize::PageSizeId::A2);
  QPageSize pageSizeA3(QPageSize::PageSizeId::A3);
  QPageSize pageSizeA4(QPageSize::PageSizeId::A4);
  QPageSize pageSizeA5(QPageSize::PageSizeId::A5);
  QPageSize pageSizeA6(QPageSize::PageSizeId::A6);
  QPageSize pageSizeA7(QPageSize::PageSizeId::A7);
  QPageSize pageSizeA8(QPageSize::PageSizeId::A8);
  QPageSize pageSizeA9(QPageSize::PageSizeId::A9);
  QPageSize pageSizeB0(QPageSize::PageSizeId::B0);
  QPageSize pageSizeB1(QPageSize::PageSizeId::B1);
  QPageSize pageSizeB2(QPageSize::PageSizeId::B2);
  QPageSize pageSizeB3(QPageSize::PageSizeId::B3);
  QPageSize pageSizeB4(QPageSize::PageSizeId::B4);
  QPageSize pageSizeB5(QPageSize::PageSizeId::B5);
  QPageSize pageSizeB6(QPageSize::PageSizeId::B6);
  QPageSize pageSizeB7(QPageSize::PageSizeId::B7);
  QPageSize pageSizeB8(QPageSize::PageSizeId::B8);
  QPageSize pageSizeB9(QPageSize::PageSizeId::B9);
  if     (ax ==  0) printer.setPageSize(pageSizeA0);
  else if(ax ==  1) printer.setPageSize(pageSizeA1);
  else if(ax ==  2) printer.setPageSize(pageSizeA2);
  else if(ax ==  3) printer.setPageSize(pageSizeA3);
  else if(ax ==  4) printer.setPageSize(pageSizeA4);
  else if(ax ==  5) printer.setPageSize(pageSizeA5);
  else if(ax ==  6) printer.setPageSize(pageSizeA6);
  else if(ax ==  7) printer.setPageSize(pageSizeA7);
  else if(ax ==  8) printer.setPageSize(pageSizeA8);
  else if(ax ==  9) printer.setPageSize(pageSizeA9);
  else if(ax == 10) printer.setPageSize(pageSizeB0);
  else if(ax == 11) printer.setPageSize(pageSizeB1);
  else if(ax == 12) printer.setPageSize(pageSizeB2);
  else if(ax == 13) printer.setPageSize(pageSizeB3);
  else if(ax == 14) printer.setPageSize(pageSizeB4);
  else if(ax == 15) printer.setPageSize(pageSizeB5);
  else if(ax == 16) printer.setPageSize(pageSizeB6);
  else if(ax == 17) printer.setPageSize(pageSizeB7);
  else if(ax == 18) printer.setPageSize(pageSizeB8);
  else if(ax == 19) printer.setPageSize(pageSizeB9);
  else              printer.setPageSize(pageSizeA4);

  if(landscape == 0) printer.setPageOrientation(QPageLayout::Portrait);
  else               printer.setPageOrientation(QPageLayout::Landscape);
  
  if(debug) printf("margins %f %f %f %f\n",lm,rm,tm,bm);
  printer.setPageMargins(QMarginsF(lm,tm,rm,bm),QPageLayout::Millimeter);
  printer.setFullPage(false);
  if(strlen(printername.text()) > 0)
  {
    if(strstr(printername.text(),"defaultprinter") != NULL)
    {
      printer.setPrinterName(printername.text());
    }  
  }
  else
  {
    printer.setOutputFileName(outputfilename.text());
  }
  printer.setOutputFormat(QPrinter::PdfFormat); //you can use native format of system usin QPrinter::NativeFormat
  /*
  QPainter painter(&printer); // create a painter which will paint 'on printer'.
  painter.setFont(QFont("Tahoma",8));
  painter.drawText(200,200,"Test");
  painter.end();
  */

  if(ask == 1)
  {
    QPrintDialog dialog(&printer);
    if(dialog.exec() != QDialog::Accepted)
    {
      return 0;
    }
  }  

  rlString html;
  readInput(&html,&inputfilename,&tableopt,&imgopt);
  //html.read(inputfilename.text());
  QTextDocument doc;
  if(strlen(defaultfont.text()) > 0) 
  {
    if(debug) printf("defaultfont = %s\n",defaultfont.text());
    QFont font;
    if(debug) printf("font = %s\n", (const char *) font.toString().toUtf8());
    font.fromString(defaultfont.text());
    doc.setDefaultFont(font);
  }  
  if(strlen(defaultstyle.text()) > 0) 
  {
    if(debug) printf("defaultstyle = %s\n",defaultstyle.text());
    doc.setDefaultStyleSheet(defaultstyle.text());
  }  
  if(margin > -0.01f) 
  {
    if(debug) printf("margin = %f\n",margin);
    doc.setDocumentMargin(margin);
  }  
  if(indentwidth > -0.01f) 
  {
    if(debug) printf("indentwidth = %f\n",indentwidth);
    doc.setIndentWidth(indentwidth);
  }  
  if(textwidth > -0.01f) 
  {
    if(debug) printf("textwidth = %f\n",textwidth);
    doc.setTextWidth(textwidth);
  }  
  if(use_design_metrics == 0) 
  {
    if(debug) printf("use_design_metrics = false\n");
    doc.setUseDesignMetrics(false);
  }  
  if(use_design_metrics == 1) 
  {
    if(debug) printf("use_design_metrics = true\n");
    doc.setUseDesignMetrics(true);
  }  
  doc.setHtml(html.text());
  doc.print(&printer);

  return 0;
}

/*
QPrinter printer(QPrinter::HighResolution); //create your QPrinter (don't need to be high resolution, anyway)
printer.setPageSize(QPrinter::A4);
printer.setOrientation(QPrinter::Portrait);
printer.setPageMargins (15,15,15,15,QPrinter::Millimeter);
printer.setFullPage(false);
printer.setOutputFileName("output.pdf");
printer.setOutputFormat(QPrinter::PdfFormat); //you can use native format of system usin QPrinter::NativeFormat
QPainter painter(&printer); // create a painter which will paint 'on printer'.
painter.setFont(QFont("Tahoma",8));
painter.drawText(200,200,"Test");
painter.end();
*/
