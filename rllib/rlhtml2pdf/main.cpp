//
// print HTML2PDF
//
#include "main.h"

int main(int argc, char **argv)
{
  rlString  inputfilename;
  rlString  outputfilename("rlhtml2pdf_output.pdf");
  rlString  printername;
  rlString  defaultfont;
  rlString  defaultstyle;
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
  int debug = 0;

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
        QStringList printerlist = QPrinterInfo::availablePrinterNames();
        for(int i=0; i<printerlist.size(); i++)
        {
          printf("printername%d = %s\n", i, (const char *) printerlist.at(i).toUtf8());
        }
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
    printf("usage: rlhtml2pdf <-options> inputfilename <outputfilename|printername>\n");
    printf("### global options ####################################################\n");
    printf("  -debug # switches on some printf(messages) for debugging\n");
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
    printf("  -font=defaultfont  # family,pointSize,pixelSize,styleHint,weight,style,underline,strikeOut,fixedPitch,rawMode\n");
    printf("                     # Examples: -font=Times,10,-1,0,50,0,0,0,0,0\n");
    printf("    family,          # Times|Arial|Courier\n");
    printf("    pointSize,       # 10\n");
    printf("    pixelSize,       # -1\n");
    printf("    styleHint,       # 0\n");
    printf("    weight,          # 0=Thin 12=ExtraLight 25=Light 50=Normal 57=Medium 63=DemoBold 75=Bold 81=ExtraBold 87=Black50\n");
    printf("    style,           # 0=Normal 1=Italic 2=Oblique\n");
    printf("    underline,       # 0|1\n");
    printf("    strikeOut,       # 0|1\n");
    printf("    fixedPitch,      # 0|1\n");
    printf("    rawMode,         # 0|1\n");
    printf("  -css=defaultstyle.css\n");
    printf("  -margin=%%f         # qt document margin\n");
    printf("  -indentwidth=%%f    # qt document indent width\n");
    printf("  -textwidth=%%f      # qt document text width\n");
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

  QPrinter printer(QPrinter::HighResolution); //create your QPrinter (don't need to be high resolution, anyway)
  if     (ax ==  0) printer.setPageSize(QPrinter::A0);
  else if(ax ==  1) printer.setPageSize(QPrinter::A1);
  else if(ax ==  2) printer.setPageSize(QPrinter::A2);
  else if(ax ==  3) printer.setPageSize(QPrinter::A3);
  else if(ax ==  4) printer.setPageSize(QPrinter::A4);
  else if(ax ==  5) printer.setPageSize(QPrinter::A5);
  else if(ax ==  6) printer.setPageSize(QPrinter::A6);
  else if(ax ==  7) printer.setPageSize(QPrinter::A7);
  else if(ax ==  8) printer.setPageSize(QPrinter::A8);
  else if(ax ==  9) printer.setPageSize(QPrinter::A9);
  else if(ax == 10) printer.setPageSize(QPrinter::B0);
  else if(ax == 11) printer.setPageSize(QPrinter::B1);
  else if(ax == 12) printer.setPageSize(QPrinter::B2);
  else if(ax == 13) printer.setPageSize(QPrinter::B3);
  else if(ax == 14) printer.setPageSize(QPrinter::B4);
  else if(ax == 15) printer.setPageSize(QPrinter::B5);
  else if(ax == 16) printer.setPageSize(QPrinter::B6);
  else if(ax == 17) printer.setPageSize(QPrinter::B7);
  else if(ax == 18) printer.setPageSize(QPrinter::B8);
  else if(ax == 19) printer.setPageSize(QPrinter::B9);
  else              printer.setPageSize(QPrinter::A4);
  if(landscape == 0) printer.setOrientation(QPrinter::Portrait);
  else               printer.setOrientation(QPrinter::Landscape);
  if(debug) printf("margins %f %f %f %f\n",lm,rm,tm,bm);
  printer.setPageMargins(lm,tm,rm,bm,QPrinter::Millimeter);
  printer.setFullPage(false);
  if(strlen(printername.text()) > 0)
  {
    printer.setPrinterName(printername.text());
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
  html.read(inputfilename.text());
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
