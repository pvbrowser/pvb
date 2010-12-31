/****************************************************************************
**
** Copyright (C) 2000-2006 Lehrig Software Engineering.
**
** This file is part of the pvbrowser project.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "../pvbrowser/pvdefine.h"
#include <string.h>
#include <ctype.h>
#include "opt.h"
#include <QWidget>
#include <QStringList>
#include <QRegExp>
#include <QFile>
#include <QtUiTools>
#include "dlginsertwidget.h"
#include "dlgeditlayout.h"
#include "widgetgenerator.h"
#include "cutil.h"
#include "MyWidgets.h"
#include "interpreter.h"
#include "pvserver.h"
#include "designer.h"

extern OPT opt;
extern dlgeditlayout *editlayout;

QStringList tablist;
static QStringList strlist;
static QStringList enumlist;
static QStringList tooltiplist;
static QStringList whatsthislist;
static char line[1024];
static int iitem = 1;
static int isConstructor = 1;

#define MAX_IVAL 20

static QWidget *theroot;

static QWidget *findChild(const char *item) // Yes I know: It's a dirty hack                                                    // because of MSDEV 6.0 :-)
{
  //printf("findChild(%s)\n",item);
  QWidget *ret;
#ifdef PVWIN32
  QString txt = item;
  ret = qFindChild<QWidget *>(theroot, txt);
#else
  ret = theroot->findChild<QWidget *>(item);
#endif
  if(ret == NULL) printf("findChild(%s) ret == NULL\n",item);
  return ret;
}

static int createStrList(QObject *item)
{
  QObject *subitem;
  QList<QObject *> levellist;
  levellist.clear();
  levellist = item->children();
  for(int i=0; i<levellist.size(); i++)
  {
    subitem = levellist.at(i);
    if(subitem->isWidgetType() &&
      ((QWidget *) subitem)->statusTip().startsWith("TQTabWidget:"))
    {
      strlist.append(subitem->objectName());
      QList<QObject *> childlist;
      QList<QObject *> rlist;
      childlist.clear();
      childlist = subitem->children();
      for(int ic=0; ic<childlist.size(); ic++)
      {
        rlist.clear();
        rlist = childlist.at(ic)->children();
        for(int ir=0; ir<rlist.size(); ir++)
        {
          int isub = rlist.size()-ir-1;
          if(!rlist.at(isub)->objectName().isEmpty())
          {
            //printf("objectName=%s\n",
            //      (const char *) rlist.at(isub)->objectName().toAscii());
            strlist.append(rlist.at(isub)->objectName());
            createStrList(rlist.at(isub));
          }
        }
      }
    }
    else
    {
      if(subitem->isWidgetType() &&
        ((QWidget *) subitem)->statusTip().startsWith("TQ"))
      {
        strlist.append(subitem->objectName());
      }
      createStrList(subitem);
    }
  }
/*
  QObject *subitem;
  QList<QObject *> levellist;
  levellist.clear();
  levellist = item->children();
  for(int i=0; i<levellist.size(); i++)
  {
    subitem = levellist.at(i);
    if(subitem->isWidgetType() &&
      ((QWidget *) subitem)->statusTip().startsWith("TQ"))
    {
      strlist.append(subitem->objectName());
    }
    createStrList(subitem);
  }
*/
  return 0;
}

static int sortWidgets(QObject *item)
{
  QObject *subitem;
  QList<QObject *> levellist;
  levellist.clear();
  levellist = item->children();
  for(int i=0; i<levellist.size(); i++)
  {
    subitem = levellist.at(i);
    if(subitem->isWidgetType() &&
      ((QWidget *) subitem)->statusTip().startsWith("TQTabWidget")) //xlehrig
    {
      QList<QObject *> sublist;
      sublist.clear();
      sublist = subitem->children();
      QList<QObject *> subsublist;
      subsublist.clear();
      subsublist = sublist.at(0)->children();
      for(int index=subsublist.size()-1; index>=0; index--)
      {
        //printf("index=%d\n",index);
        ((QTabWidget *) subitem)->setCurrentIndex(index);
      }
    }
    sortWidgets(subitem);
  }
  return 0;
}

static int getStrList(QWidget *root)
{
  sortWidgets(root);
  strlist.clear();
  createStrList(root);
  return 0;
}

static int generateLayoutConstuctors(FILE *fout)
{
  QTextEdit *e = editlayout->uidlg->textEditConstructors;
  if(e == NULL || fout == NULL) return -1;

  QString txt;
  QTextBlock line = e->document()->begin();
  while(line.isValid())
  {
    txt = line.text();
    txt.remove(" ");
    txt.remove("\n");
    if(txt.contains("pvQLayout") && txt.contains("(") && txt.contains(")"))
    {
      fprintf(fout,"  %s\n",(const char *) txt.toUtf8().constData());
      fprintf(fout,"\n");
    }
    line = line.next();
  }
  return 0;
}

static int generateLayoutDefinition(FILE *fout)
{
  QTextEdit *e = editlayout->uidlg->textEditDef;
  if(e == NULL || fout == NULL) return -1;

  QString txt;
  QTextBlock line = e->document()->begin();
  while(line.isValid())
  {
    txt = line.text();
    txt.remove(" ");
    txt.remove("\n");
    if(txt.contains("pvAdd") && txt.contains("(") && txt.contains(")"))
    {
      fprintf(fout,"  %s\n",(const char *) txt.toUtf8().constData());
    }
    line = line.next();
  }
  fprintf(fout,"\n");
  return 0;
}

static int generateWidgetEnum(FILE *fout, QWidget *root)
{
  QString item;
  QWidget *widget;

  theroot = root;
  fprintf(fout,"// our mask contains the following objects\n");
  fprintf(fout,"enum {\n");
  fprintf(fout,"  ID_MAIN_WIDGET = 0,\n");

  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toAscii()); // root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      fprintf(fout,"  %s,\n",(const char *) widget->objectName().toAscii());
    }
    else
    {
      printf("WARNING generateWidgetEnum:findChild=%s not found\n",(const char *) item.toAscii());
    }
  }

  // may be we have to include the layout BEGIN
  QTextEdit *e = editlayout->uidlg->textEditConstructors;
  if(e != NULL)
  {
    QString txt;
    QTextBlock line = e->document()->begin();
    while(line.isValid())
    {
      txt = line.text();
      txt.remove(" ");
      txt.remove("\n");
      if(txt.contains("pvQLayout") && txt.contains("(") && txt.contains(")"))
      {
        char buf[1024];
        if     (txt.contains("(p,0,")) ;              // we have nothing todo
        else if(txt.contains("(p,ID_MAIN_WIDGET,")) ; // we have nothing todo
        else if(txt.length() < (int) (sizeof(buf)-1))
        {
          char *start, *end;
          strcpy(buf,txt.toUtf8().constData());
          start = strchr(buf,',');
          if(start != NULL)
          {
            start++;
            end = strchr(start,',');
            if(end != NULL) *end = '\0';
            fprintf(fout,"  %s,\n",start);
          }
        }
        else printf("too long layout string: %s\n",(const char *) txt.toUtf8().constData());
      }
      line = line.next();
    }
  }
  // may be we have to include the layout END

  fprintf(fout,"  ID_END_OF_WIDGETS\n");
  fprintf(fout,"};\n");
  fprintf(fout,"\n");
  return 0;
}

static const char *quote(QString &text)
{
  const char *cptr;
  static char buf[1024];
  int i;

  i = 0; // convert to utf8 and quote '"' and '\n'
  cptr = text.toUtf8().constData();
  while(*cptr != '\0' && i < (int) (sizeof(buf)-2))
  {
    if     (*cptr == '\"')
    {
      buf[i++] = '\\';
      buf[i++] = *cptr;
    }  
    else if(*cptr == '\n') 
    {
      buf[i++] = '\\';
      buf[i++] = 'n';
    }
    else
    {
      buf[i++] = *cptr;
    }
    cptr++;
  }
  buf[i] = '\0';
  return buf;
}

static int generateDefineMaskWidget(FILE *fout, QWidget *widget, const char *tabparentname)
{
  QString qbuf,type,tooltip,whatsthis,statustip,text;
  char buf[1024], itemname[512],parentname[512],*cptr;
  int x,y,w,h;

  if(fout == NULL || widget == NULL)
  {
    printf("WARNING: generateDefineMaskWidget pointer == NULL\n");
    return -1;
  }
  // get values begin ######################################################
  qbuf = widget->statusTip(); // parse statusTip
  strcpy(buf,qbuf.toAscii());
  cptr = strstr(buf,":");
  if(cptr != NULL) *cptr = '\0';
  type = buf;

  tooltip   = widget->toolTip();
  whatsthis = widget->whatsThis();
  statustip = widget->statusTip();
  // get values end ########################################################

  // get properties begin ##################################################
  x = widget->x();
  y = widget->y();
  w = widget->width();
  h = widget->height();
  strcpy(itemname,"error: too long");
  strcpy(parentname,"error: too long");
  if(strlen(widget->objectName().toAscii()) < (sizeof(itemname)-1))
    strcpy(itemname,widget->objectName().toAscii());
  if(strlen(widget->parent()->objectName().toAscii()) < (sizeof(parentname)-1))
    strcpy(parentname,widget->parent()->objectName().toAscii());
  // get properties end ####################################################

  if(opt.arg_debug) printf("generateDefineMaskWidget(%s,%s)\n",(const char *) type.toAscii(), (const char *) widget->objectName().toAscii());

  if     (type == "TQWidget")
  {
    fprintf(fout,"  pvQWidget(p,%s,%s);\n",itemname,tabparentname);
    //fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    QWidget *grandpa = NULL;
    QWidget *grandpa_parent = NULL;
    QWidget *pa = (QWidget *) widget->parent();
    if(pa != NULL) grandpa = (QWidget *) widget->parent()->parent();
    if(grandpa != NULL) grandpa_parent = (QWidget *) widget->parent()->parent()->parent();
    if(grandpa != NULL && grandpa->statusTip().startsWith("TQTabWidget:"))
    {
      QTabWidget *tab = (QTabWidget *) grandpa;
      QString txt = tab->tabText(tab->indexOf(widget));
      fprintf(fout,"  pvAddTab(p,%s,%s,\"%s\");\n",tabparentname,itemname,quote(txt));
    }
    else if(grandpa_parent != NULL && grandpa_parent->statusTip().startsWith("TQToolBox:"))
    {
      QToolBox *tool = (QToolBox *) grandpa_parent;
      QString txt = tool->itemText(tool->indexOf(widget));
      fprintf(fout,"  pvAddTab(p,%s,%s,\"%s\");\n",tabparentname,itemname,quote(txt));
    }
    else
    {
      printf("unknown ancestor of TQWidget\n");
      if(pa != NULL) printf("pa->statusTip=%s\n",(const char *) pa->statusTip().toAscii());
      if(grandpa != NULL) printf("grandpa->statusTip=%s\n",(const char *) grandpa->statusTip().toAscii());
      if(grandpa->parent() != NULL) printf("grandpa->parent()->statusTip=%s\n",(const char *) ((QWidget *) grandpa->parent())->statusTip().toAscii());
    }
    iitem++;
  }
  else if(type == "TQPushButton")
  {
    fprintf(fout,"  pvQPushButton(p,%s,%s);\n",itemname,parentname);
    MyQPushButton *obj = (MyQPushButton *) widget;
    text = obj->text();
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if(!text.isEmpty()) fprintf(fout,"  pvSetText(p,%s,\"%s\");\n",itemname, quote(text));
    iitem++;
  }
  else if(type == "TQLabel")
  {
    MyLabel *obj = (MyLabel *) widget;
    text = obj->text();
    fprintf(fout,"  pvQLabel(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if(!text.isEmpty()) fprintf(fout,"  pvSetText(p,%s,\"%s\");\n",itemname, quote(text));
    if(obj->statusTip().contains(":align:"))
    {
      fprintf(fout,"  pvSetAlignment(p,%s,%d);\n",itemname,(int) obj->alignment());
    }
    iitem++;
  }
  else if(type == "TQLineEdit")
  {
    MyLineEdit *obj = (MyLineEdit *) widget;
    text = obj->text();
    fprintf(fout,"  pvQLineEdit(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if(!text.isEmpty()) fprintf(fout,"  pvSetText(p,%s,\"%s\");\n",itemname, quote(text));
    if(obj->isReadOnly())
    {
      fprintf(fout,"  pvSetEditable(p,%s,0);\n",itemname);
    }
    if(obj->echoMode() == QLineEdit::Password)
    {
      fprintf(fout,"  pvSetEchoMode(p,%s,2);\n",itemname);
    }
    else if(obj->echoMode() == QLineEdit::NoEcho)
    {
      fprintf(fout,"  pvSetEchoMode(p,%s,0);\n",itemname);
    }
    if(obj->statusTip().contains(":align:"))
    {
      fprintf(fout,"  pvSetAlignment(p,%s,%d);\n",itemname,(int) obj->alignment());
    }
    iitem++;
  }
  else if(type == "TQMultiLineEdit")
  {
    QString tooltip;
    int editable, maxlines;
    MyMultiLineEdit *obj = (MyMultiLineEdit *) widget;
    text = obj->toPlainText();
    if(obj->isReadOnly()) editable = 0;
    else                  editable = 1;
    tooltip = obj->toolTip();
    maxlines = obj->maxlines;
    fprintf(fout,"  pvQMultiLineEdit(p,%s,%s,%d,%d);\n",itemname,parentname,editable,maxlines);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQTextBrowser")
  {
    fprintf(fout,"  pvQTextBrowser(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQListView")
  {
    MyListView *obj = (MyListView *) widget;
    fprintf(fout,"  pvQListView(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if     (obj->selectionMode() == QAbstractItemView::MultiSelection)
    {
      fprintf(fout,"  pvSetMultiSelection(p,%s,1);\n",itemname);
    }
    else if(obj->selectionMode() == QAbstractItemView::NoSelection)
    {
      fprintf(fout,"  pvSetMultiSelection(p,%s,2);\n",itemname);
    }
    else
    {
      fprintf(fout,"  pvSetMultiSelection(p,%s,0);\n",itemname);
    }
    iitem++;
  }
  else if(type == "TQComboBox")
  {
    MyComboBox *obj = (MyComboBox *) widget;
    QString insertionpolicy = "AtBottom";
    int editable = 0;
    if(obj->isEditable()) editable = 1;
    if((int) obj->insertPolicy() == 0) insertionpolicy = "NoInsertion";
    if((int) obj->insertPolicy() == 1) insertionpolicy = "AtTop";
    if((int) obj->insertPolicy() == 2) insertionpolicy = "AtCurrent";
    if((int) obj->insertPolicy() == 3) insertionpolicy = "AtBottom";
    if((int) obj->insertPolicy() == 4) insertionpolicy = "AfterCurrent";
    if((int) obj->insertPolicy() == 5) insertionpolicy = "BeforeCurrent";
    fprintf(fout,"  pvQComboBox(p,%s,%s,%d,%s);\n",itemname,parentname,editable,(const char *) insertionpolicy.toAscii());
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQLCDNumber")
  {
    QLCDNumber *obj = (QLCDNumber *) widget;
    int numdigits;
    numdigits = obj->numDigits();
    QString style = "Outline";
    if(obj->segmentStyle() == QLCDNumber::Filled) style = "Filled";
    if(obj->segmentStyle() == QLCDNumber::Flat)   style = "Flat";
    QString mode = "Hex";
    if(obj->mode() == QLCDNumber::Dec) mode = "Dec";
    if(obj->mode() == QLCDNumber::Oct) mode = "Oct";
    if(obj->mode() == QLCDNumber::Bin) mode = "Bin";
    fprintf(fout,"  pvQLCDNumber(p,%s,%s,%d,%s,%s);\n",itemname,parentname,numdigits, (const char *) style.toAscii(), (const char *) mode.toAscii());
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQSlider")
  {
    MySlider *obj = (MySlider *) widget;
    int pagestep = obj->pageStep();
    int value    = obj->value();
    int minvalue = obj->minimum();
    int maxvalue = obj->maximum();
    QString orientation = "Vertical";
    if(obj->orientation() == Qt::Horizontal ) orientation = "Horizontal";
    fprintf(fout,"  pvQSlider(p,%s,%s,%d,%d,%d,%d,%s);\n",itemname,parentname,minvalue,maxvalue,pagestep,value,(const char *) orientation.toAscii());
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQButtonGroup")
  {
    MyButtonGroup *obj = (MyButtonGroup *) widget;
    text = obj->title();
    fprintf(fout,"  pvQButtonGroup(p,%s,%s,-1,HORIZONTAL,\"%s\");\n",itemname,parentname,quote(text));
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQGroupBox")
  {
    MyGroupBox *obj = (MyGroupBox *) widget;
    text = obj->title();
    fprintf(fout,"  pvQGroupBox(p,%s,%s,-1,HORIZONTAL,\"%s\");\n",itemname,parentname,quote(text));
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQListBox")
  {
    MyListBox *obj = (MyListBox *) widget;
    fprintf(fout,"  pvQListBox(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if     (obj->selectionMode() == QAbstractItemView::MultiSelection)
    {
      fprintf(fout,"  pvSetMultiSelection(p,%s,1);\n",itemname);
    }
    else if(obj->selectionMode() == QAbstractItemView::NoSelection)
    {
      fprintf(fout,"  pvSetMultiSelection(p,%s,2);\n",itemname);
    }
    else
    {
      fprintf(fout,"  pvSetMultiSelection(p,%s,0);\n",itemname);
    }
    iitem++;
  }
  else if(type == "TQIconView")
  {
    fprintf(fout,"  pvQIconView(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQTable")
  {
    MyTable *obj = (MyTable *) widget;
    fprintf(fout,"  pvQTable(p,%s,%s,%d,%d);\n",itemname,parentname,obj->rowCount(),obj->columnCount());
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    //if(obj->isReadOnly())
    if(!obj->isEnabled())
    {
      fprintf(fout,"  pvSetEditable(p,%s,0);\n",itemname);
    }
    iitem++;
  }
  else if(type == "TQSpinBox")
  {
    MySpinBox *obj = (MySpinBox *) widget;
    fprintf(fout,"  pvQSpinBox(p,%s,%s,%d,%d,1);\n",itemname,parentname,obj->minimum(),obj->maximum());
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQDial")
  {
    MyDial *obj = (MyDial *) widget;
    fprintf(fout,"  pvQDial(p,%s,%s,%d,%d,%d,%d);\n",itemname,parentname,obj->minimum(),obj->maximum(),obj->pageStep(),obj->value());
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQProgressBar")
  {
    MyProgressBar *obj = (MyProgressBar *) widget;
    QString orientation = "Horizontal";
    if(obj->orientation() == Qt::Vertical ) orientation = "Vertical";
    fprintf(fout,"  pvQProgressBar(p,%s,%s,%d,%s);\n",itemname,parentname,obj->maximum(),(const char *) orientation.toAscii());
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQRadio")
  {
    MyRadioButton *obj = (MyRadioButton *) widget;
    text = obj->text();
    fprintf(fout,"  pvQRadioButton(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if(!text.isEmpty()) fprintf(fout,"  pvSetText(p,%s,\"%s\");\n",itemname, quote(text));
    if(obj->isChecked())
    {
      fprintf(fout,"  pvSetChecked(p,%s,1);\n",itemname);
    }
    iitem++;
  }
  else if(type == "TQCheck")
  {
    MyCheckBox *obj = (MyCheckBox *) widget;
    text = obj->text();
    fprintf(fout,"  pvQCheckBox(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if(!text.isEmpty()) fprintf(fout,"  pvSetText(p,%s,\"%s\");\n",itemname, quote(text));
    if(obj->isChecked())
    {
      fprintf(fout,"  pvSetChecked(p,%s,1);\n",itemname);
    }
    iitem++;
  }
  else if(type == "TQFrame")
  {
    MyFrame *obj = (MyFrame *) widget;
    QString shape = "NoFrame";
    if(obj->frameShape() == QFrame::StyledPanel) shape = "StyledPanel";
    if(obj->frameShape() == QFrame::VLine)       shape = "VLine";
    if(obj->frameShape() == QFrame::HLine)       shape = "HLine";
    if(obj->frameShape() == QFrame::WinPanel)    shape = "WinPanel";
    if(obj->frameShape() == QFrame::Panel)       shape = "Panel";
    if(obj->frameShape() == QFrame::Box)         shape = "Box";
    QString shadow = "Plain";
    if(obj->frameShadow() == QFrame::Raised)     shadow = "Raised";
    if(obj->frameShadow() == QFrame::Sunken)     shadow = "Sunken";
    int linewidth = obj->lineWidth();
    int margin = 1; //obj->margin();
    fprintf(fout,"  pvQFrame(p,%s,%s,%s,%s,%d,%d);\n",itemname,parentname,(const char *) shape.toAscii(), (const char *) shadow.toAscii(), linewidth, margin);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQDraw")
  {
    fprintf(fout,"  pvQDraw(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQImage")
  {
    if(whatsthis.contains(".bmp") || whatsthis.contains(".BMP"))
    {
      fprintf(fout,"  pvQImage(p,%s,%s,\"%s\",&w,&h,&depth);\n",itemname,parentname,(const char *) whatsthis.toAscii());
      fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    }
    else
    {
      fprintf(fout,"  pvDownloadFile(p,\"%s\");\n",(const char *) whatsthis.toAscii());
      fprintf(fout,"  pvQImage(p,%s,%s,\"%s\",&w,&h,&depth);\n",itemname,parentname,(const char *) whatsthis.toAscii());
      fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    }
    iitem++;
  }
  else if(type == "TQGl")
  {
    fprintf(fout,"  pvQGL(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    fprintf(fout,"  pvGlBegin(p,%s);\n",itemname);
    fprintf(fout,"  initializeGL(p);    // (todo: write your gl initialization routine) see example\n");
    fprintf(fout,"  resizeGL(p,%d,%d);  // (todo: write your resize routine) see example\n",w,h);
    fprintf(fout,"  pvGlEnd(p);\n");
    iitem++;
  }
  else if(type == "TQVtk")
  {
    fprintf(fout,"  pvQVtkTclWidget(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQTabWidget")
  {
    MyQTabWidget *obj = (MyQTabWidget *) widget;
    fprintf(fout,"  pvQTabWidget(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    int tabposition = 0;
    if(obj->tabPosition() == QTabWidget::South) tabposition = 1;
    if(obj->tabPosition() == QTabWidget::West)  tabposition = 2;
    if(obj->tabPosition() == QTabWidget::East)  tabposition = 3;
    if(tabposition != 0)
    {
      fprintf(fout,"  pvSetTabPosition(p,%s,%d);\n",itemname,tabposition);
    }
    iitem++;
  }
  else if(type == "TQToolBox")
  {
    fprintf(fout,"  pvQToolBox(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    //int tabposition = 0;
    //if(tabposition != 0)
    //{
    //  fprintf(fout,"  pvSetTabPosition(p,%s,%d);\n",itemname,tabposition);
    //}
    iitem++;
  }
  else if(type == "TQwtPlotWidget")
  {
    QwtPlotWidget *obj = (QwtPlotWidget *) widget;
    fprintf(fout,"  pvQwtPlotWidget(p,%s,%s,%d,%d);\n",itemname,parentname,obj->nCurves,obj->nMarker);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    QColor c = obj->canvasBackground();
    fprintf(fout,"  qpwSetCanvasBackground(p,%s,%d,%d,%d);\n",itemname,c.red(),c.green(),c.blue());
    //qwtmurx
    if(obj->axisEnabled(QwtPlot::yLeft))   fprintf(fout,"  qpwEnableAxis(p,%s,yLeft);\n",itemname);
    if(obj->axisEnabled(QwtPlot::yRight))  fprintf(fout,"  qpwEnableAxis(p,%s,yRight);\n",itemname);
    if(obj->axisEnabled(QwtPlot::xBottom)) fprintf(fout,"  qpwEnableAxis(p,%s,xBottom);\n",itemname);
    if(obj->axisEnabled(QwtPlot::xTop))    fprintf(fout,"  qpwEnableAxis(p,%s,xTop);\n",itemname);
    //if(obj->yLeftAxisEnabled())   fprintf(fout,"  qpwEnableAxis(p,%s,yLeft);\n",itemname);
    //if(obj->yRightAxisEnabled())  fprintf(fout,"  qpwEnableAxis(p,%s,yRight);\n",itemname);
    //if(obj->xBottomAxisEnabled()) fprintf(fout,"  qpwEnableAxis(p,%s,xBottom);\n",itemname);
    //if(obj->xTopAxisEnabled())    fprintf(fout,"  qpwEnableAxis(p,%s,xTop);\n",itemname);
    text = obj->title().text();
    if(!text.isEmpty()) fprintf(fout,"  qpwSetTitle(p,%s,\"%s\");\n",itemname,quote(text));
    iitem++;
  }
  else if(type == "TQwtScale")
  {
    MyQwtScale *obj = (MyQwtScale *) widget;
    fprintf(fout,"  pvQwtScale(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    if(obj->scaleDraw()->alignment() == QwtScaleDraw::LeftScale)   fprintf(fout,"  qwtScaleSetPosition(p,%s,ScaleLeft);\n",itemname);
    if(obj->scaleDraw()->alignment() == QwtScaleDraw::RightScale)   fprintf(fout,"  qwtScaleSetPosition(p,%s,ScaleRight);\n",itemname);
    if(obj->scaleDraw()->alignment() == QwtScaleDraw::TopScale)   fprintf(fout,"  qwtScaleSetPosition(p,%s,ScaleTop);\n",itemname);
    if(obj->scaleDraw()->alignment() == QwtScaleDraw::BottomScale)   fprintf(fout,"  qwtScaleSetPosition(p,%s,ScaleBottom);\n",itemname);
    text = obj->title().text();
    if(!text.isEmpty()) fprintf(fout,"  qwtScaleSetTitle(p,%s,\"%s\");\n",itemname,quote(text));
    text = obj->title().font().family();
    int size      = obj->title().font().pointSize();
    int bold, italic, underline, strikeout;
    bold = italic = underline = strikeout = 0;
    if(obj->title().font().bold())      bold = 1;
    if(obj->title().font().italic())    italic = 1;
    if(obj->title().font().underline()) underline = 1;
    if(obj->title().font().strikeOut()) strikeout = 1;
    if(obj->statusTip().contains(":font:")) fprintf(fout,"  qwtScaleSetTitleFont(p,%s,\"%s\",%d,%d,%d,%d,%d);\n",itemname,quote(text),size,bold,italic,underline,strikeout);
    QColor c = obj->title().color();
    fprintf(fout,"  qwtScaleSetTitleColor(p,%s,%d,%d,%d);\n",itemname,c.red(),c.green(),c.blue());
    iitem++;
  }
  else if(type == "TQwtThermo")
  {
    fprintf(fout,"  pvQwtThermo(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQwtKnob")
  {
    MyQwtKnob *obj = (MyQwtKnob *) widget;
    fprintf(fout,"  pvQwtKnob(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
/*
    if(readOnly == "true" ) fout << "  qwtKnobSetReadOnly(p," << id << ",1);" << ENDL;
    if(readOnly == "false") fout << "  qwtKnobSetReadOnly(p," << id << ",0);" << ENDL;
    if(mass     != "") fout << "  qwtKnobSetMass(p," << id << "," << mass << ");" << ENDL;
    if(orientation != "") fout << "  qwtKnobSetOrientation(p," << id << "," << orientation << ");" << ENDL;
*/
    fprintf(fout,"  qwtKnobSetKnobWidth(p,%s,%d);\n",itemname,obj->knobWidth());
    fprintf(fout,"  qwtKnobSetBorderWidth(p,%s,%d);\n",itemname,obj->borderWidth());
    fprintf(fout,"  qwtKnobSetTotalAngle(p,%s,%f);\n",itemname,obj->totalAngle());
    if(obj->symbol() == QwtKnob::Line)
      fprintf(fout,"  qwtKnobSetSymbol(p,%s,KnobLine);\n",itemname);
    else 
      fprintf(fout,"  qwtKnobSetSymbol(p,%s,KnobDot);\n",itemname);
    iitem++;
  }
  else if(type == "TQwtCounter")
  {
    MyQwtCounter *obj = (MyQwtCounter *) widget;
    fprintf(fout,"  pvQwtCounter(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    fprintf(fout,"  qwtCounterSetNumButtons(p,%s,%d);\n",itemname,obj->numButtons());
    fprintf(fout,"  qwtCounterSetStep(p,%s,%f);\n",itemname,obj->step());
    fprintf(fout,"  qwtCounterSetMinValue(p,%s,%f);\n",itemname,obj->minVal());
    fprintf(fout,"  qwtCounterSetMaxValue(p,%s,%f);\n",itemname,obj->maxVal());
    fprintf(fout,"  qwtCounterSetStepButton1(p,%s,%d);\n",itemname,obj->stepButton1());
    fprintf(fout,"  qwtCounterSetStepButton2(p,%s,%d);\n",itemname,obj->stepButton2());
    fprintf(fout,"  qwtCounterSetStepButton3(p,%s,%d);\n",itemname,obj->stepButton3());
    fprintf(fout,"  qwtCounterSetValue(p,%s,%f);\n",itemname,obj->value());
    iitem++;
  }
  else if(type == "TQwtWheel")
  {
    MyQwtWheel *obj = (MyQwtWheel *) widget;
    fprintf(fout,"  pvQwtWheel(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
/*
    if(readOnly == "true" ) fout << "  qwtWheelSetReadOnly(p," << id << ",1);" << ENDL;
    if(readOnly == "false") fout << "  qwtWheelSetReadOnly(p," << id << ",0);" << ENDL;
*/
    fprintf(fout,"  qwtWheelSetMass(p,%s,%f);\n",itemname,obj->mass());
/*
    if(orientation != "") fout << "  qwtWheelSetOrientation(p," << id << "," << orientation << ");" << ENDL;
*/
    fprintf(fout,"  qwtWheelSetTotalAngle(p,%s,%f);\n",itemname,obj->viewAngle());
    fprintf(fout,"  qwtWheelSetViewAngle(p,%s,%f);\n",itemname,obj->viewAngle());
    fprintf(fout,"  qwtWheelSetTickCnt(p,%s,%d);\n",itemname,obj->tickCnt());
    fprintf(fout,"  qwtWheelSetInternalBorder(p,%s,%d);\n",itemname,obj->internalBorder());
    iitem++;
  }
  else if(type == "TQwtSlider")
  {
    MyQwtSlider *obj = (MyQwtSlider *) widget;
    fprintf(fout,"  pvQwtSlider(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
/*
    if(readOnly == "true" ) fout << "  qwtSliderSetReadOnly(p," << id << ",1);" << ENDL;
    if(readOnly == "false") fout << "  qwtSliderSetReadOnly(p," << id << ",0);" << ENDL;
    if(mass     != "") fout << "  qwtSliderSetMass(p," << id << "," << mass << ");" << ENDL;
    if(orientation != "") fout << "  qwtSliderSetOrientation(p," << id << "," << orientation << ");" << ENDL;
*/
    fprintf(fout,"  qwtSliderSetThumbLength(p,%s,%d);\n",itemname,obj->thumbLength());
    fprintf(fout,"  qwtSliderSetThumbWidth(p,%s,%d);\n",itemname,obj->thumbWidth());
    fprintf(fout,"  qwtSliderSetBorderWidth(p,%s,%d);\n",itemname,obj->borderWidth());
    //enum BGSTYLE { BgTrough = 0x1, BgSlot = 0x2, BgBoth = BgTrough | BgSlot};
    if(obj->bgStyle() == QwtSlider::BgTrough)
      fprintf(fout,"  qwtSliderSetBgStyle(p,%s,SliderBgTrough);\n",itemname);
    if(obj->bgStyle() == QwtSlider::BgSlot)
      fprintf(fout,"  qwtSliderSetBgStyle(p,%s,SliderBgSlot);\n",itemname);
    if(obj->bgStyle() == QwtSlider::BgBoth)
      fprintf(fout,"  qwtSliderSetBgStyle(p,%s,SliderBgBoth);\n",itemname);
    //enum ScalePos { None, Left, Right, Top, Bottom };
    //qwtmurx
    if(obj->scalePosition() == QwtSlider::NoScale)
      fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderNone);\n",itemname);
    if(obj->scalePosition() == QwtSlider::LeftScale)
      fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderLeft);\n",itemname);
    if(obj->scalePosition() == QwtSlider::RightScale)
      fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderRight);\n",itemname);
    if(obj->scalePosition() == QwtSlider::TopScale)
      fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderTop);\n",itemname);
    if(obj->scalePosition() == QwtSlider::BottomScale)
      fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderBottom);\n",itemname);
    //if(obj->scalePosition() == QwtSlider::None)
    //  fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderNone);\n",itemname);
    //if(obj->scalePosition() == QwtSlider::Left)
    //  fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderLeft);\n",itemname);
    //if(obj->scalePosition() == QwtSlider::Right)
    //  fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderRight);\n",itemname);
    //if(obj->scalePosition() == QwtSlider::Top)
    //  fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderTop);\n",itemname);
    //if(obj->scalePosition() == QwtSlider::Bottom)
    //  fprintf(fout,"  qwtSliderSetScalePos(p,%s,SliderBottom);\n",itemname);
    fprintf(fout,"  qwtSliderSetValue(p,%s,%f);\n",itemname,obj->value());
    iitem++;
  }
  else if(type == "TQwtCompass")
  {
    fprintf(fout,"  pvQwtCompass(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQwtAnalogClock")
  {
    fprintf(fout,"  pvQwtAnalogClock(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQwtDial")
  {
    fprintf(fout,"  pvQwtDial(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQDateEdit")
  {
    fprintf(fout,"  pvQDateEdit(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQTimeEdit")
  {
    fprintf(fout,"  pvQTimeEdit(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
/*
    if(timeDisplay != "")
    {
      int h,m,s,ap;
      h = m = s = ap = 0;
      if(strstr((const char *) timeDisplay, "Hours")   != NULL) h = 1;
      if(strstr((const char *) timeDisplay, "Minutes") != NULL) m = 1;
      if(strstr((const char *) timeDisplay, "Seconds") != NULL) s = 1;
      if(strstr((const char *) timeDisplay, "AMPM")    != NULL) ap = 1;
      fout << "  pvSetTimeEditDisplay(p," << id << "," << h << "," << m << "," << s << "," << ap << ");" << ENDL;
    }
*/
    iitem++;
  }
  else if(type == "TQDateTimeEdit")
  {
    fprintf(fout,"  pvQDateTimeEdit(p,%s,%s);\n",itemname,parentname);
    fprintf(fout,"  pvSetGeometry(p,%s,%d,%d,%d,%d);\n",itemname,x,y,w,h);
    iitem++;
  }
  else if(type == "TQVbox")
  {
    iitem++;
  }
  else if(type == "TQHbox")
  {
    iitem++;
  }
  else if(type == "TQGrid")
  {
    iitem++;
  }

  if(statustip.contains(":foreground:"))
  {
    QColor col;
    if(type == "TQPushButton") col = widget->palette().color(QPalette::ButtonText);
    else                       col = widget->palette().color(QPalette::WindowText);
    fprintf(fout,"  pvSetPaletteForegroundColor(p,%s,%d,%d,%d);\n",itemname,col.red(),col.green(),col.blue());
  }
  if(statustip.contains(":background:"))
  {
    QColor col;
    if     (type == "TQPushButton") col = widget->palette().color(QPalette::Button);
    else if(type == "TQRadio")      col = widget->palette().color(QPalette::Button);
    else if(type == "TQCheck")      col = widget->palette().color(QPalette::Button);
    else if(type == "TQMultiLineEdit" ||
            type == "TQLineEdit"      ||
            type == "TQTextBrowser"   )
                                    col = widget->palette().color(QPalette::Base);
    else                            col = widget->palette().color(QPalette::Window);
    fprintf(fout,"  pvSetPaletteBackgroundColor(p,%s,%d,%d,%d);\n",itemname,col.red(),col.green(),col.blue());
  }
  QFont f = widget->font();
  /*
  if(f.family()    != "Sans Serif" ||
     f.pointSize() != 9 ||
     f.bold()      != 0 ||
     f.italic()    != 0 ||
     f.underline() != 0 ||
     f.strikeOut() != 0 )
  */
  if(statustip.contains("font:"))
  {
    if(f.family()    == "MS Shell Dlg 2" &&
       f.pointSize() == 8 &&
       f.bold()      == 0 &&
       f.italic()    == 0 &&
       f.underline() == 0 &&
       f.strikeOut() == 0 )
    {
    }
    else
    {
      fprintf(fout,"  pvSetFont(p,%s,\"%s\",%d,%d,%d,%d,%d);\n",
        itemname,(const char *) f.family().toAscii(),
        f.pointSize(),f.bold(),f.italic(),f.underline(),f.strikeOut());
    }
  }
  if(!tooltip.isEmpty())   fprintf(fout,"  pvToolTip(p,%s,\"%s\");\n",itemname, quote(tooltip));
  if(!whatsthis.isEmpty()) fprintf(fout,"  pvSetWhatsThis(p,%s,\"%s\");\n",itemname, quote(whatsthis));
  if(type == "TQLabel")
  {
    MyLabel *label = (MyLabel *) widget;
    QString shape, shadow;

    int linewidth = label->lineWidth();
    int margin = label->margin();

    shape = "NoFrame";
    if(label->frameShape() == QFrame::NoFrame)     shape = "NoFrame";
    if(label->frameShape() == QFrame::Box)         shape = "Box";
    if(label->frameShape() == QFrame::Panel)       shape = "Panel";
    if(label->frameShape() == QFrame::StyledPanel) shape = "StyledPanel";
    if(label->frameShape() == QFrame::HLine)       shape = "HLine";
    if(label->frameShape() == QFrame::VLine)       shape = "VLine";
    if(label->frameShape() == QFrame::WinPanel)    shape = "WinPanel";

    shadow = "Plain";
    if(label->frameShadow() == QFrame::Plain)  shadow = "Plain";
    if(label->frameShadow() == QFrame::Raised) shadow = "Raised";
    if(label->frameShadow() == QFrame::Sunken) shadow = "Sunken";

    if(label->frameShape() != QFrame::NoFrame)
      fprintf(fout,"  pvSetStyle(p,%s,%s,-1,-1,-1);\n",itemname,(const char *) shape.toAscii());
    if(label->frameShadow() != QFrame::Plain)
      fprintf(fout,"  pvSetStyle(p,%s,-1,%s,-1,-1);\n",itemname,(const char *) shadow.toAscii());
    if(linewidth != 1)
      fprintf(fout,"  pvSetStyle(p,%s,-1,-1,%d,-1);\n",itemname,linewidth);
    if(margin != 0)
      fprintf(fout,"  pvSetStyle(p,%s,-1,-1,-1,%d);\n",itemname,margin);
  }
  w = widget->minimumWidth();
  h = widget->minimumHeight();
  if(w > 0 || h > 0)
  {
    fprintf(fout,"  pvSetMinSize(p,%s,%d,%d);\n",itemname,w,h);
  }
  w = widget->maximumWidth();
  h = widget->maximumHeight();
  if(w < 5000 || h < 5000)
  {
    fprintf(fout,"  pvSetMaxSize(p,%s,%d,%d);\n",itemname,w,h);
  }
  fprintf(fout,"\n");
  return 0;
}

static int generateDefineMaskWidgets(FILE *fout, QWidget *root)
{
  QString item;
  QWidget *widget;
  char tabparentname[512];

  theroot = root;
  fprintf(fout,"static int generated_defineMask(PARAM *p)\n");
  fprintf(fout,"{\n");
  fprintf(fout,"  int w,h,depth;\n");
  fprintf(fout,"\n");
  fprintf(fout,"  if(p == NULL) return 1;\n");
  fprintf(fout,"  w = h = depth = strcmp(toolTip[0],whatsThis[0]);\n");
  fprintf(fout,"  if(widgetType[0] == -1) return 1;\n");
  fprintf(fout,"  if(w==h) depth=0; // fool the compiler\n");
  fprintf(fout,"  pvStartDefinition(p,ID_END_OF_WIDGETS);\n");
  fprintf(fout,"\n");

  strcpy(tabparentname,"0");
  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toAscii()); //root->findChild<QWidget *>(item);
    if(widget->statusTip().startsWith("TQTabWidget:"))
    {
      strcpy(tabparentname,(const char *) widget->objectName().toAscii());
    }
    if(widget->statusTip().startsWith("TQToolBox:"))
    {
      strcpy(tabparentname,(const char *) widget->objectName().toAscii());
    }
    if(widget->statusTip().startsWith("TQWidget:"))
    {
      QWidget *p = (QWidget *) widget->parent();
      if(p != NULL)
      {
        QWidget *gp = (QWidget *) p->parent();
        if(gp != NULL)
        {
          // printf("tabparentname=%s\n",(const char *) gp->objectName().toAscii());
          strcpy(tabparentname,(const char *) gp->objectName().toAscii());
        }  
      }
    }
    if(widget != NULL)
    {
      generateDefineMaskWidget(fout,widget,tabparentname);
    }
    else
    {
      printf("WARNING generateDefineMaskWidgets:findChild=%s not found\n",(const char *) item.toAscii());
    }
  }

  // generateLayout
  generateLayoutConstuctors(fout);
  generateLayoutDefinition(fout);

  fprintf(fout,"  pvEndDefinition(p);\n");

  // generate TabOrder
  for(int i=0; i<tablist.size(); i++)
  {
    QString item = tablist.at(i);
    fprintf(fout,"%s",(const char *) item.toUtf8().constData());
  }

  fprintf(fout,"  return 0;\n");
  fprintf(fout,"}\n");
  fprintf(fout,"\n");
  return 0;
}

static int generateToolTip(FILE *fout, QWidget *root)
{
  QString item,qbuf;
  QWidget *widget;

  theroot = root;
  fprintf(fout,"  static const char *toolTip[] = {\n");
  fprintf(fout,"  \"\",\n");

  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toAscii()); //root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      qbuf = widget->toolTip();
      fprintf(fout,"  \"%s\",\n",quote(qbuf));
    }
    else
    {
      printf("WARNING generateToolTip:findChild=%s not found\n",(const char *) item.toAscii());
    }
  }

  fprintf(fout,"  \"\"};\n");
  fprintf(fout,"\n");

  return 0;
}

static int generateWhatsThis(FILE *fout, QWidget *root)
{
  QString item,qbuf;
  QWidget *widget;

  theroot = root;
  fprintf(fout,"  static const char *whatsThis[] = {\n");
  fprintf(fout,"  \"\",\n");

  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toAscii()); //root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      qbuf = widget->whatsThis();
      fprintf(fout,"  \"%s\",\n",quote(qbuf));
    }
    else
    {
      printf("WARNING generateWhatsThis:findChild=%s not found\n",(const char *) item.toAscii());
    }
  }

  fprintf(fout,"  \"\"};\n");
  fprintf(fout,"\n");

  return 0;
}

static int generateWidgetType(FILE *fout, QWidget *root)
{
  QString item,qbuf;
  QWidget *widget;
  char    buf[1024],*cptr;

  theroot = root;
  fprintf(fout,"  static const int widgetType[ID_END_OF_WIDGETS+1] = {\n  0,\n");

  // loop over widgets
  for(int i=0; i<strlist.size(); i++)
  {
    item = strlist.at(i);
    widget = findChild(item.toAscii()); //root->findChild<QWidget *>(item);
    if(widget != NULL)
    {
      qbuf = widget->statusTip(); // parse statusTip
      strcpy(buf,qbuf.toAscii());
      cptr = strstr(buf,":");
      if(cptr != NULL) *cptr = '\0';
      if(strncmp(buf,"TQ",2) != 0) strcpy(buf,"0");
      fprintf(fout,"  %s,\n",buf);
    }
    else
    {
      printf("WARNING generateWidgetType:findChild=%s not found\n",(const char *) item.toAscii());
    }
  }

  fprintf(fout,"  -1 };\n");
  fprintf(fout,"\n");

  return 0;
}

/*
  if     (type == "TQWidget")
  {
  }
  else if(type == "TQPushButton")
  {
  }
  else if(type == "TQLabel")
  {
  }
  else if(type == "TQLineEdit")
  {
  }
  else if(type == "TQComboBox")
  {
  }
  else if(type == "TQLCDNumber")
  {
  }
  else if(type == "TQButtonGroup")
  {
  }
  else if(type == "TQRadio")
  {
  }
  else if(type == "TQCheck")
  {
  }
  else if(type == "TQSlider")
  {
  }
  else if(type == "TQFrame")
  {
  }
  else if(type == "TQImage")
  {
  }
  else if(type == "TQDraw")
  {
  }
  else if(type == "TQGl")
  {
  }
  else if(type == "TQTabWidget")
  {
  }
  else if(type == "TQGroupBox")
  {
  }
  else if(type == "TQListBox")
  {
  }
  else if(type == "TQTable")
  {
  }
  else if(type == "TQSpinBox")
  {
  }
  else if(type == "TQDial")
  {
  }
  else if(type == "TQProgressBar")
  {
  }
  else if(type == "TQMultiLineEdit")
  {
  }
  else if(type == "TQTextBrowser")
  {
  }
  else if(type == "TQListView")
  {
  }
  else if(type == "TQIconView")
  {
  }
  else if(type == "TQVtk")
  {
  }
  else if(type == "TQwtPlotWidget")
  {
  }
  else if(type == "TQwtScale")
  {
  }
  else if(type == "TQwtThermo")
  {
  }
  else if(type == "TQwtKnob")
  {
  }
  else if(type == "TQwtCounter")
  {
  }
  else if(type == "TQwtWheel")
  {
  }
  else if(type == "TQwtSlider")
  {
  }
  else if(type == "TQwtDial")
  {
  }
  else if(type == "TQwtCompass")
  {
  }
  else if(type == "TQwtAnalogClock")
  {
  }
  else if(type == "TQDateEdit")
  {
  }
  else if(type == "TQTimeEdit")
  {
  }
  else if(type == "TQDateTimeEdit")
  {
  }
  else if(type == "TQToolBox")
  {
  }
  else if(type == "TQVbox")
  {
  }
  else if(type == "TQHbox")
  {
  }
  else if(type == "TQGrid")
  {
  }
*/

static int generateGeneratedArea(FILE *fout, QWidget *root)
{
  strlist.clear();
  iitem = 1;

  fprintf(fout,"// _begin_of_generated_area_ (do not edit -> use ui2pvc) -------------------\n");
  fprintf(fout,"\n");

  getStrList(root);
  generateWidgetEnum(fout, root);
  generateToolTip(fout, root);
  generateWhatsThis(fout, root);
  generateWidgetType(fout,root);
  generateDefineMaskWidgets(fout, root);

  fprintf(fout,"// _end_of_generated_area_ (do not edit -> use ui2pvc) ---------------------\n");
  strlist.clear();
  return 0;
}

extern FileLines file_lines;

int generateMask(const char *filename, QWidget *root)
{
  FILE *fout;
  FileLines *fl;
  int found_begin, found_end, done_end, done;

  if(loadFile(filename) != 0) return -1;
  if(opt.murx)
  {
    fout = fopen("murx.cpp","w");
  }
  else
  {
    fout = fopen(filename,"w");
  }
  if(fout == NULL)
  {
    unloadFile();
    printf("ERROR: generateMask could not write %s\n",filename);
    return -1;
  }
  if(opt.arg_debug) printf("generateMask %s\n",filename);

  found_begin = found_end = done_end = done = 0;
  fl = &file_lines;
  fl = fl->next;
  while(fl != NULL)
  {
    if(strstr(fl->line,"_begin_of_generated_area_") != NULL) found_begin = 1;
    if(strstr(fl->line,"_end_of_generated_area_")   != NULL) found_end = 1;
    if(found_begin == 0)
    {
      fprintf(fout,"%s",fl->line);
    }
    else if(found_begin == 1 && found_end == 0)
    {
      if(done==0) generateGeneratedArea(fout, root);
      done = 1;
    }
    else if(found_begin == 1 && found_end == 1)
    {
      if(done_end==1) fprintf(fout,"%s",fl->line);
      done_end = 1;
    }
    fl = fl->next;
  }

  fclose(fout);
  unloadFile();
  if(opt.arg_debug) printf("generateMask end\n");
  return 0;
}

static int isCommand(const char *cmd)
{
  const char *cptr;
  cptr = strstr(line,"pv");
  if(cptr == NULL) return 0;

  for(int i=0; i<(int) (sizeof(line)-1); i++)
  {
    if(cptr[i] == '\0')   return 0;
    if(cmd[i]  == '\0')   return 1;
    if(cptr[i] != cmd[i]) return 0;
  }
  return 0;
}

static void getParams(char *id, char *parent, int *ival, char *text, char *cval)
{
  int i,iival;
  char *cptr, *cptr1, *cptr2, *cstart;
  if(id     == NULL) return;
  if(parent == NULL) return;
  if(ival   == NULL) return;
  if(text   == NULL) return;

  cstart = NULL;
  id[0] = parent[0] = '\0';

  // get text
  cptr1 = strchr(line,'\"');
  text[0] = '\0';
  if(cptr1 != NULL)
  {
    i = 0;
    cptr1++;
    while(*cptr1 != '\0')
    {
      if(*cptr1 == '\"')
      {
        break;
      }
      else if(*cptr1 == '\\' && cptr1[1] == 'n')
      {
        text[i++] = '\n';
        cptr1++;
        cptr1++;
      }
      else if(*cptr1 == '\\')
      {
        cptr1++;
        text[i++] = *cptr1;
        cptr1++;
      }
      else
      {
        text[i++] = *cptr1;
        cptr1++;
      }
    }
    text[i] = '\0';
  }

  // get id
  cptr1 = strchr(line,',');
  if(cptr1 != NULL)
  {
    cptr1++;
    cptr2 = strchr(cptr1,',');
    if(cptr2 != NULL)
    {
      i = 0;
      cptr = cptr1;
      while(cptr != cptr2)
      {
        id[i++] = *cptr;
        cptr++;
      }
      id[i] = '\0';
      cstart = cptr2;
      cstart++; // now cstart is positioned after id
    }
  }

  // get parent
  if(cstart != NULL && isalpha(*cstart))
  {
    cptr1 = cstart;
    cptr2 = strchr(cptr1,',');
    if(cptr2 == NULL) cptr2 = strchr(cptr1,')');
    if(cptr2 != NULL)
    {
      i = 0;
      cptr = cptr1;
      while(cptr != cptr2)
      {
        parent[i++] = *cptr;
        cptr++;
      }
      parent[i] = '\0';
      cstart = cptr2;
      cstart++; // now cstart is positioned after parent
    }
  }

  // get ival
  if(cstart != NULL)
  {
    iival = 0;
    while(*cstart != '\0')
    {
      if(isdigit(*cstart) || *cstart == '-')
      {
        sscanf(cstart,"%d",&ival[iival++]);
        cptr = strchr(cstart,',');
        if(cptr == NULL) cptr = strchr(cptr1,')');
        if(cptr == NULL)
        {
          break; // no more parameters
        }
        else
        {
          cstart = cptr;
          cstart++; // now cstart is positioned at next parameter
        }
      }
      else if(*cstart == '\"') // it might be the text
      {
        cptr = strrchr(line,'\"');
        if(cptr != NULL)
        {
          cptr++;
          if(*cptr != ',') break;
          cstart = cptr;
          cstart++; // now cstart is positioned at next parameter
        }
        else
        {
          break;
        }
      }
      else if(isalpha(*cstart)) // ival is a #define or enum
      {
        i = 0;
        cptr = cstart;
        while(*cptr != ',' && *cptr != '\"' && *cptr != ')')
        {
          cval[i++] = *cptr;
          cptr++;
        }
        cval[i] = '\0';
        cptr++;
        cstart = cptr;
        iival++;
      }
      else
      {
        break;
      }
    }
  }

  if(parent[0] == '\0')
  {
    strcpy(parent,"0"); // this might be the parent
    if(opt.arg_debug > 1) printf("isConstructor=%d parent=%s id=%s line=%s",isConstructor,parent,id,line);
    if(isConstructor == 1)
    {
      for(int i=0; i<(MAX_IVAL-1); i++)
      {
        ival[i] = ival[i+1];
      }
    }
  }

  if(opt.arg_debug > 1) printf("getParams: id=%s parent=%s text='%s' ival=%d:%d:%d:%d cval='%s' %s",id,parent,text,ival[0],ival[1],ival[2],ival[3],cval,line);
}

/*
  int   pvStartDefinition (PARAM *p, int num_objects)
  int   pvQLayoutVbox (PARAM *p, int id, int parent)
  int   pvQLayoutHbox (PARAM *p, int id, int parent)
  int   pvQLayoutGrid (PARAM *p, int id, int parent)
  int   pvQWidget (PARAM *p, int id, int parent)
  int   pvQLabel (PARAM *p, int id, int parent)
  int   pvQComboBox (PARAM *p, int id, int parent, int editable, int policy)
  int   pvQLineEdit (PARAM *p, int id, int parent)
  int   pvQPushButton (PARAM *p, int id, int parent)
  int   pvQLCDNumber (PARAM *p, int id, int parent, int numDigits, int segmentStyle, int mode)
  int   pvQSlider (PARAM *p, int id, int parent, int minValue, int maxValue, int pageStep, int value, int orientation)
  int   pvQButtonGroup (PARAM *p, int id, int parent, int columns, int orientation, const char *title)
  int   pvQRadioButton (PARAM *p, int id, int parent)
  int   pvQCheckBox (PARAM *p, int id, int parent)
  int   pvQFrame (PARAM *p, int id, int parent, int shape, int shadow, int line_width, int margin)
  int   pvQDraw (PARAM *p, int id, int parent)
  int   pvQImage (PARAM *p, int id, int parent, const char *imagename, int *w, int *h, int *depth)
  int   pvQGL (PARAM *p, int id, int parent)
  int   pvQTabWidget (PARAM *p, int id, int parent)
  int   pvQToolBox (PARAM *p, int id, int parent)
  int   pvQGroupBox (PARAM *p, int id, int parent, int columns, int orientation, const char *title)
  int   pvQListBox (PARAM *p, int id, int parent)
  int   pvQTable (PARAM *p, int id, int parent, int rows, int columns)
  int   pvQSpinBox (PARAM *p, int id, int parent, int min, int max, int step)
  int   pvQDial (PARAM *p, int id, int parent, int min, int max, int page_step, int value)
  int   pvQProgressBar (PARAM *p, int id, int parent, int total_steps)
  int   pvQMultiLineEdit (PARAM *p, int id, int parent, int editable, int max_lines)
  int   pvQTextBrowser (PARAM *p, int id, int parent)
  int   pvQListView (PARAM *p, int id, int parent)
  int   pvQIconView (PARAM *p, int id, int parent)
  int   pvQVtkTclWidget (PARAM *p, int id, int parent)
  int   pvQwtPlotWidget (PARAM *p, int id, int parent, int nCurves, int nMarker)
  int   pvQwtScale (PARAM *p, int id, int parent, int pos)
  int   pvQwtThermo (PARAM *p, int id, int parent)
  int   pvQwtKnob (PARAM *p, int id, int parent)
  int   pvQwtCounter (PARAM *p, int id, int parent)
  int   pvQwtWheel (PARAM *p, int id, int parent)
  int   pvQwtSlider (PARAM *p, int id, int parent)
  int   pvQwtDial (PARAM *p, int id, int parent)
  int   pvQwtCompass (PARAM *p, int id, int parent)
  int   pvQwtAnalogClock (PARAM *p, int id, int parent)
  int   pvQDateEdit (PARAM *p, int id, int parent)
  int   pvQTimeEdit (PARAM *p, int id, int parent)
  int   pvQDateTimeEdit (PARAM *p, int id, int parent)
  int   pvEndDefinition (PARAM *p)
  int   pvAddWidgetOrLayout (PARAM *p, int id, int item, int row, int col)
  int   pvAddStretch (PARAM *p, int id, int param)
  int   pvTabOrder (PARAM *p, int id1, int id2)
*/

static int getWidget(FILE *fin, QWidget *root)
{
  char id[sizeof(line)],parent[sizeof(line)],text[sizeof(line)],cval[sizeof(line)];
  int ival[MAX_IVAL];
  int itemtype = -1;
  int constructorFound;
  QWidget *pw; // parent widget
  QWidget *item = root;
  static int s = -1; // socket
  QString st,qtext;

  pw = root;
  if(root == NULL) return -1;
  do
  {
    if(strstr(line,"pv") == NULL) return 0;
    if(opt.arg_debug) printf("getWidget line=%s",line);
    id[0] = parent[0] = text[0] = cval[0] = '\0';
    for(int i=0; i<MAX_IVAL; i++)
    {
      ival[i] = -1;
    }
    isConstructor = 1;
    if(isCommand("pvDownloadFile(") == 1) // this may come in front of constructor
    {
    }
    else
    {
      getParams(id,parent,ival,text,cval);
      qtext = QString::fromUtf8(text);
      theroot = root;
      if(parent[0] == '0') pw = root;
      else                 pw = findChild(parent); //root->findChild<QWidget *>(parent);
      if(pw == NULL)       pw = root;
    }

    // begin constructors
    constructorFound = 1;
    if(isCommand("pvQLayoutVbox(") == 1)
    {
      if(editlayout != NULL)
      {
        QString txt = QString::fromUtf8(line);
        txt.remove(" ");
        txt.remove("\n");
        editlayout->uidlg->textEditConstructors->append(txt);
        if     (strstr(line,"(p,0,") != NULL)
          item = (QWidget *) new QVBoxLayout();
        else if(strstr(line,"(p,ID_MAIN_WIDGET,") != NULL)
          item = (QWidget *) new QVBoxLayout();
        else
          item = (QWidget *) new QVBoxLayout(pw);
        itemtype = TQVbox;
      }
    }
    else if(isCommand("pvQLayoutHbox(") == 1)
    {
      if(editlayout != NULL)
      {
        QString txt = QString::fromUtf8(line);
        txt.remove(" ");
        txt.remove("\n");
        editlayout->uidlg->textEditConstructors->append(txt);
        if     (strstr(line,"(p,0,") != NULL)
          item = (QWidget *) new QHBoxLayout();
        else if(strstr(line,"(;p,ID_MAIN_WIDGET,") != NULL)
          item = (QWidget *) new QHBoxLayout();
        else
          item = (QWidget *) new QHBoxLayout(pw);
        itemtype = TQHbox;
      }
    }
    else if(isCommand("pvQLayoutGrid(") == 1)
    {
      if(editlayout != NULL)
      {
        QString txt = QString::fromUtf8(line);
        txt.remove(" ");
        txt.remove("\n");
        editlayout->uidlg->textEditConstructors->append(txt);
        if     (strstr(line,"(p,0,") != NULL)
          item = (QWidget *) new QGridLayout();
        else if(strstr(line,"(p,ID_MAIN_WIDGET,") != NULL)
          item = (QWidget *) new QGridLayout();
        else
          item = (QWidget *) new QGridLayout(pw);
        itemtype = TQGrid;
        //item->setStatusTip("TQGrid:");
        //iitem++;
      }
    }
    else if(isCommand("pvQWidget(") == 1)
    {
      if(pw != NULL && pw->statusTip().startsWith("TQTabWidget:"))
      {
        QList<QObject *> childs;
        childs.clear();
        childs = pw->children();
        if(childs.count() > 0)
        {
          item = (QWidget *) new MyQWidget(&s, 0, (QWidget *) childs.at(0), id);
          item->setStatusTip("TQWidget:");
        }
        else
        {
          printf("TQTabWidget: childs->count() <= 0\n");
        }
      }
      else if(pw != NULL && pw->statusTip().startsWith("TQToolBox:"))
      {
        item = (QWidget *) new MyQWidget(&s, 0, pw, id);
        item->setStatusTip("TQWidget:");
      }
      else
      {
        printf("unknown parent of TQWidget\n");
      }
      itemtype = TQWidget;
      iitem++;
    }
    else if(isCommand("pvQLabel(") == 1)
    {
      item = (QWidget *) new MyLabel(&s, 0, pw, id);
      itemtype = TQLabel;
      item->setStatusTip("TQLabel:");
      item->setAutoFillBackground(true);
      iitem++;
    }
    else if(isCommand("pvQComboBox(") == 1)
    {
      item = (QWidget *) new MyComboBox(&s, 0, pw, id);
      MyComboBox *comboitem = (MyComboBox *) item;
      itemtype = TQComboBox;
      if(ival[0] == 1) comboitem->setEditable(1);
      else             comboitem->setEditable(0);
      int insertionpolicy = (int) QComboBox::InsertAtBottom;
      if(strstr(line,",NoInsertion)")   != NULL) insertionpolicy = (int) QComboBox::NoInsert;
      if(strstr(line,",AtTop)")         != NULL) insertionpolicy = (int) QComboBox::InsertAtTop;
      if(strstr(line,",AtCurrent)")     != NULL) insertionpolicy = (int) QComboBox::InsertAtCurrent;
      if(strstr(line,",AtBottom)")      != NULL) insertionpolicy = (int) QComboBox::InsertAtBottom;
      if(strstr(line,",AfterCurrent)")  != NULL) insertionpolicy = (int) QComboBox::InsertAfterCurrent;
      if(strstr(line,",BeforeCurrent)") != NULL) insertionpolicy = (int) QComboBox::InsertBeforeCurrent;
      comboitem->setInsertPolicy((QComboBox::InsertPolicy) insertionpolicy);
      item->setStatusTip("TQComboBox:");
      iitem++;
    }
    else if(isCommand("pvQLineEdit(") == 1)
    {
      item = (QWidget *) new MyLineEdit(&s, 0, pw, id);
      itemtype = TQLineEdit;
      item->setStatusTip("TQLineEdit:");
      iitem++;
    }
    else if(isCommand("pvQPushButton(") == 1)
    {
      item = (QWidget *) new MyQPushButton(&s, 0, pw, id);
      itemtype = TQPushButton;
      item->setStatusTip("TQPushButton:");
      iitem++;
    }
    else if(isCommand("pvQLCDNumber(") == 1)
    {
      item = (QWidget *) new QLCDNumber(pw);
      QLCDNumber *lcditem = (QLCDNumber *) item;
      item->setObjectName(id);
      lcditem->setNumDigits(ival[0]);
      if(strstr(line,",Hex") != NULL) lcditem->setMode(QLCDNumber::Hex);
      if(strstr(line,",Dec") != NULL) lcditem->setMode(QLCDNumber::Dec);
      if(strstr(line,",Oct") != NULL) lcditem->setMode(QLCDNumber::Oct);
      if(strstr(line,",Bin") != NULL) lcditem->setMode(QLCDNumber::Bin);
      if(strstr(line,",Outline") != NULL) lcditem->setSegmentStyle(QLCDNumber::Outline);
      if(strstr(line,",Filled")  != NULL) lcditem->setSegmentStyle(QLCDNumber::Filled);
      if(strstr(line,",Flat")    != NULL) lcditem->setSegmentStyle(QLCDNumber::Flat);
      itemtype = TQLCDNumber;
      item->setStatusTip("TQLCDNumber:");
      iitem++;
    }
    else if(isCommand("pvQSlider(") == 1)
    {
      int ori = ival[4];
      if(ori == -1)
      {
        if(cval[0] == 'h' || cval[0] == 'H') ori = Qt::Horizontal;
        else                                 ori = Qt::Vertical;
      }
      item = (QWidget *) new MySlider(&s, 0, ival[0], ival[1], ival[2], ival[3], (Qt::Orientation) ori, pw, id);;
      itemtype = TQSlider;
      item->setStatusTip("TQSlider:");
      iitem++;
    }
    else if(isCommand("pvQButtonGroup(") == 1)
    {
      int ori = ival[1];
      if(ori == -1)
      {
        if(cval[0] == 'h' || cval[0] == 'H') ori = Qt::Horizontal;
        else                                 ori = Qt::Vertical;
      }
      item = (QWidget *) new MyButtonGroup(&s, 0, ival[0], (Qt::Orientation) ori, qtext, pw, id);
      itemtype = TQButtonGroup;
      item->setStatusTip("TQButtonGroup:");
      iitem++;
    }
    else if(isCommand("pvQRadioButton(") == 1)
    {
      item = (QWidget *) new MyRadioButton(&s, 0, pw, id);
      itemtype = TQRadio;
      item->setStatusTip("TQRadio:");
      item->setAutoFillBackground(true);
      iitem++;
    }
    else if(isCommand("pvQCheckBox(") == 1)
    {
      item = (QWidget *) new MyCheckBox(&s, 0, pw, id);
      itemtype = TQCheck;
      item->setStatusTip("TQCheck:");
      item->setAutoFillBackground(true);
      iitem++;
    }
    else if(isCommand("pvQFrame(") == 1)
    {
      int shape      = ival[0];
      int shadow     = ival[1];
      int line_width = ival[2];
      int margin     = ival[3];

      shape = QFrame::NoFrame;
      if     (strstr(line,",StyledPanel,") != NULL) shape = QFrame::StyledPanel;
      else if(strstr(line,",VLine,")       != NULL) shape = QFrame::VLine;
      else if(strstr(line,",HLine,")       != NULL) shape = QFrame::HLine;
      else if(strstr(line,",WinPanel,")    != NULL) shape = QFrame::WinPanel;
      else if(strstr(line,",Panel,")       != NULL) shape = QFrame::Panel;
      else if(strstr(line,",Box,")         != NULL) shape = QFrame::Box;

      shadow = QFrame::Plain;
      if(strstr(line,",Raised,")           != NULL) shadow = QFrame::Raised;
      else if(strstr(line,",Sunken,")      != NULL) shadow = QFrame::Sunken;
      else if(strstr(line,",MShadow,")     != NULL) shadow = QFrame::Plain;

      item = (QWidget *) new MyFrame(&s, 0, shape, shadow, line_width, margin, pw, id);
      itemtype = TQFrame;
      item->setStatusTip("TQFrame:");
      iitem++;
    }
    else if(isCommand("pvQDraw(") == 1)
    {
      item = (QWidget *) new QDrawWidget(pw, id);
      itemtype = TQDraw;
      item->setStatusTip("TQDraw:");
      QString fname = whatsthislist.at(iitem);
      fname.replace('\\', "");
      item->setWhatsThis(fname);
      iitem++;
    }
    else if(isCommand("pvQImage(") == 1)
    {
      item = (QImageWidget *) new QImageWidget(&s, 0, pw, id);
      itemtype = TQImage;
      item->setStatusTip("TQImage:");
      qtext.replace('\\', "");
      item->setWhatsThis(qtext);
      if(strlen(qtext.toAscii()) > 0)
      {
        ((QImageWidget *)item)->setImage(qtext.toAscii());
      }
      iitem++;
    }
//#ifndef PVMAC
    else if(isCommand("pvQGL(") == 1)
    {
      int shape = QFrame::Panel;
      int shadow = QFrame::Raised;
      int line_width = 10;
      int margin = 1; // not used in Qt4
      item = new MyFrame(&s,0,(QFrame::Shape) shape, (QFrame::Shadow) shadow,line_width,margin,pw);
      //item = (QWidget *) new PvGLWidget(pw, 0, &s);
      item->setObjectName(id);
      itemtype = TQGl;
      item->setStatusTip("TQGl:");
      iitem++;
    }
//#endif
    else if(isCommand("pvQTabWidget(") == 1)
    {
      item = (QWidget *) new MyQTabWidget(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQTabWidget;
      item->setStatusTip("TQTabWidget:");
      iitem++;
    }
    else if(isCommand("pvQToolBox(") == 1)
    {
      item = (QWidget *) new MyQToolBox(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQToolBox;
      item->setStatusTip("TQToolBox:");
      iitem++;
    }
    else if(isCommand("pvQGroupBox(") == 1)
    {
      int columns = ival[0];
      int ori = ival[1];
      if(ori == -1)
      {
        if(cval[0] == 'h' || cval[0] == 'H') ori = Qt::Horizontal;
        else                                 ori = Qt::Vertical;
      }
      item = (QWidget *) new MyGroupBox(&s ,0 ,columns ,(Qt::Orientation) ori, qtext, pw);
      item->setObjectName(id);
      itemtype = TQGroupBox;
      item->setStatusTip("TQGroupBox:");
      iitem++;
    }
    else if(isCommand("pvQListBox(") == 1)
    {
      item = (QWidget *) new MyListBox(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQListBox;
      item->setStatusTip("TQListBox:");
      iitem++;
    }
    else if(isCommand("pvQTable(") == 1)
    {
      item = (QWidget *) new MyTable(&s, 0, ival[0], ival[1], pw);
      item->setObjectName(id);
      itemtype = TQTable;
      item->setStatusTip("TQTable:");
      iitem++;
    }
    else if(isCommand("pvQSpinBox(") == 1)
    {
      item = (QWidget *) new MySpinBox(&s, 0, ival[0], ival[1], ival[2], pw);
      item->setObjectName(id);
      itemtype = TQSpinBox;
      item->setStatusTip("TQSpinBox:");
      iitem++;
    }
    else if(isCommand("pvQDial(") == 1)
    {
      item = (QWidget *) new MyDial(&s, 0, ival[0], ival[1], ival[2], ival[3], pw);
      item->setObjectName(id);
      itemtype = TQDial;
      item->setStatusTip("TQDial:");
      iitem++;
    }
    else if(isCommand("pvQProgressBar(") == 1)
    {
      int ori = ival[1];
      if(ori == -1)
      {
        if(cval[0] == 'v' || cval[0] == 'V') ori = Qt::Vertical;
        else                                 ori = Qt::Horizontal;
      }
      item = (QWidget *) new MyProgressBar(&s, 0, ival[0], (Qt::Orientation) ori, pw);
      item->setObjectName(id);
      itemtype = TQProgressBar;
      item->setStatusTip("TQProgressBar:");
      iitem++;
    }
    else if(isCommand("pvQMultiLineEdit(") == 1)
    {
      char buf[80];
      item = (QWidget *) new MyMultiLineEdit(&s, 0, ival[0], ival[1], pw);
      item->setObjectName(id);
      sprintf(buf,"TQMultiLineEdit:maxlines=%d:",ival[1]);
      itemtype = TQMultiLineEdit;
      item->setStatusTip("TQMultiLineEdit:");
      iitem++;
    }
    else if(isCommand("pvQTextBrowser(") == 1)
    {
      item = (QWidget *) new MyTextBrowser(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQTextBrowser;
      item->setStatusTip("TQTextBrowser:");
      iitem++;
    }
    else if(isCommand("pvQListView(") == 1)
    {
      item = (QWidget *) new MyListView(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQListView;
      item->setStatusTip("TQListView:");
      iitem++;
    }
    else if(isCommand("pvQIconView(") == 1)
    {
      item = (QWidget *) new MyIconView(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQIconView;
      item->setStatusTip("TQIconView:");
      iitem++;
    }
    else if(isCommand("pvQVtkTclWidget(") == 1)
    {
      int shape = QFrame::Panel;
      int shadow = QFrame::Sunken;
      int line_width = 10;
      int margin = 1; // not used in Qt4
      item = (QWidget *) new MyFrame(&s,0,(QFrame::Shape) shape, (QFrame::Shadow) shadow,line_width,margin,pw);
      //item = (QWidget *) new pvVtkTclWidget(pw ,"vtkWidget", 0, &s);
      itemtype = TQVtk;
      item->setStatusTip("TQVtk:");
      iitem++;
    }
    else if(isCommand("pvQwtPlotWidget(") == 1)
    {
      item = (QWidget *) new QwtPlotWidget(&s, 0, pw, ival[0], ival[1]);
      item->setObjectName(id);
      itemtype = TQwtPlotWidget;
      item->setStatusTip("TQwtPlotWidget:");
      iitem++;
    }
    else if(isCommand("pvQwtScale(") == 1)
    {
      int pos = ival[0];
      if(strstr(line,"ScaleLeft")   != NULL) pos = PV::ScaleLeft;
      if(strstr(line,"ScaleRight")  != NULL) pos = PV::ScaleRight;
      if(strstr(line,"ScaleTop")    != NULL) pos = PV::ScaleTop;
      if(strstr(line,"ScaleBottom") != NULL) pos = PV::ScaleBottom;
      item = (QWidget *) new MyQwtScale(&s, 0,(QwtScaleDraw::Alignment) pos, pw);
      item->setObjectName(id);
      itemtype = TQwtScale;
      item->setStatusTip("TQwtScale:");
      iitem++;
    }
    else if(isCommand("pvQwtThermo(") == 1)
    {
      item = (QWidget *) new MyQwtThermo(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtThermo;
      item->setStatusTip("TQwtThermo:");
      iitem++;
    }
    else if(isCommand("pvQwtKnob(") == 1)
    {
      item = (QWidget *) new MyQwtKnob(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtKnob;
      item->setStatusTip("TQwtKnob:");
      iitem++;
    }
    else if(isCommand("pvQwtCounter(") == 1)
    {
      item = (QWidget *) new MyQwtCounter(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtCounter;
      item->setStatusTip("TQwtCounter:");
      iitem++;
    }
    else if(isCommand("pvQwtWheel(") == 1)
    {
      item = (QWidget *) new MyQwtWheel(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtWheel;
      item->setStatusTip("TQwtWheel:");
      iitem++;;
    }
    else if(isCommand("pvQwtSlider(") == 1)
    {
      item = (QWidget *) new MyQwtSlider(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtSlider;
      item->setStatusTip("TQwtSlider:");
      iitem++;
    }
    else if(isCommand("pvQwtDial(") == 1)
    {
      item = (QWidget *) new MyQwtDial(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtDial;
      item->setStatusTip("TQwtDial:");
      iitem++;
    }
    else if(isCommand("pvQwtCompass(") == 1)
    {
      item = (QWidget *) new MyQwtCompass(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtCompass;
      item->setStatusTip("TQwtCompass:");
      iitem++;
    }
    else if(isCommand("pvQwtAnalogClock(") == 1)
    {
      item = (QWidget *) new MyQwtAnalogClock(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQwtAnalogClock;
      item->setStatusTip("TQwtAnalogClock:");
      iitem++;
    }
    else if(isCommand("pvQDateEdit(") == 1)
    {
      item = (QWidget *) new MyQDateEdit(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQDateEdit;
      item->setStatusTip("TQDateEdit:");
      iitem++;
    }
    else if(isCommand("pvQTimeEdit(") == 1)
    {
      item = (QWidget *) new MyQTimeEdit(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQTimeEdit;
      item->setStatusTip("TQTimeEdit:");
      iitem++;
    }
    else if(isCommand("pvQDateTimeEdit(") == 1)
    {
      item = (QWidget *) new MyQDateTimeEdit(&s, 0, pw);
      item->setObjectName(id);
      itemtype = TQDateTimeEdit;
      item->setStatusTip("TQDateTimeEdit:");
      iitem++;
    }
    else
    {
      constructorFound = 0;
    }

    if(constructorFound == 0)
    {
      isConstructor = 0;
      getParams(id,parent,ival,text,cval);
      qtext = QString::fromUtf8(text);
      if(parent[0] == '0') pw = root;
      else                 pw = findChild(parent); //root->findChild<QWidget *>(parent);
      if(pw == NULL)       pw = root;
    }
    else
    {
      isConstructor = 1;
    }
    // end constructors
    // begin attributes
    if(isCommand("pvSetGeometry(") == 1)
    {
      if(itemtype == TQImage)
      {
        ((QImageWidget *)item)->setGeometry(ival[0],ival[1],ival[2],ival[3]);
      }
      else if(itemtype == TQDraw)
      {
        ((QDrawWidget *)item)->setGeometry(ival[0],ival[1],ival[2],ival[3]);
      }
      else
      {
        item->setGeometry(ival[0],ival[1],ival[2],ival[3]);
      }

      if(itemtype == TQDraw)
      {
        myResize(item,ival[2],ival[3]);
      }
      if(itemtype == TQVtk)
      {
        myResize(item,ival[2],ival[3]);
      }
    }
    else if(isCommand("pvSetTabPosition(") == 1)
    {
      if(itemtype == TQTabWidget)
      {
        ((MyQTabWidget *)item)->setTabPosition((QTabWidget::TabPosition) ival[0]);
      }
    }
    else if(isCommand("pvSetEditable(") == 1)
    {
      if(itemtype == TQTable)
      {
        if(ival[0] == 1) ((MyTable *)item)->setEnabled(true);
        else             ((MyTable *)item)->setEnabled(false);
      }
      else if(itemtype == TQComboBox)
      {
        ((MyComboBox *)item)->setEditable(ival[0]);
      }
      else if(itemtype == TQMultiLineEdit)
      {
        if(ival[0] == 1) ((MyMultiLineEdit *)item)->setReadOnly(false);
        else             ((MyMultiLineEdit *)item)->setReadOnly(true);
      }
      else
      {
        printf("pvSetEditable unknown itemtype\n");
      }
    }
    else if(isCommand("pvSetPaletteBackgroundColor(") == 1)
    {
      int r,g,b;
      r = ival[0];
      g = ival[1];
      b = ival[2];
      mySetBackgroundColor(item,itemtype,r,g,b);
      st = item->statusTip();
      if(!st.contains("background:"))
      {
        st.append("background:");
        item->setStatusTip(st);
      }
    }
    else if(isCommand("pvSetPaletteForegroundColor(") == 1)
    {
      int r,g,b;
      r = ival[0];
      g = ival[1];
      b = ival[2];
      mySetForegroundColor(item,itemtype,r,g,b);
      st = item->statusTip();
      if(!st.contains("foreground:"))
      {
        st.append("foreground:");
        item->setStatusTip(st);
      }
    }
    else if(isCommand("pvSetText(") == 1)
    {
      switch(itemtype)
      {
        case TQLabel:
          ((MyLabel *) item)->setText(qtext);
          break;
        case TQPushButton:
          ((MyQPushButton *) item)->setText(qtext);
          break;
        case TQRadio:
          ((MyRadioButton *) item)->setText(text);
          break;
        case TQCheck:
          ((MyCheckBox *) item)->setText(qtext);
          break;
        case TQLineEdit:
          ((MyLineEdit *) item)->setText(qtext);
          break;
        case TQGroupBox:
          ((MyGroupBox *) item)->setTitle(qtext);
          break;
        default:
          printf("unknown pvSetText(%s) itemtype=%d\n",text,itemtype);
          break;
      }
    }
    else if(isCommand("pvSetChecked(") == 1)
    {
      int state = ival[0];
      if(itemtype == TQRadio)
      {
        MyRadioButton *but = (MyRadioButton *) item;
        if(state == 1) but->setChecked(true);
        else           but->setChecked(false);
      }
      else if(itemtype == TQCheck)
      {
        MyCheckBox *but = (MyCheckBox *) item;
        if(state == 1) but->setChecked(true);
        else           but->setChecked(false);
      }
      st = item->statusTip();
      if(!st.contains("check:"))
      {
        st.append("check:");
        item->setStatusTip(st);
      }
    }
    else if(isCommand("pvSetEchoMode(") == 1)
    {
      int mode = ival[0];
      if(itemtype == TQLineEdit)
      {
        MyLineEdit *le = (MyLineEdit *) item;
        if     (mode == 0) le->setEchoMode(QLineEdit::NoEcho);
        else if(mode == 2) le->setEchoMode(QLineEdit::Password);
        else               le->setEchoMode(QLineEdit::Normal);
      }
      st = item->statusTip();
      if(!st.contains("echomode:"))
      {
        st.append("echomode:");
        item->setStatusTip(st);
      }
    }
    else if(isCommand("pvSetMultiSelection(") == 1)
    {
      int mode = ival[0];
      if(itemtype == TQListView)
      {;
        MyListView *lv = (MyListView *) item;
        lv->setMultiSelection(mode);
      }
      else if(itemtype == TQListBox)
      {
        MyListBox *lb = (MyListBox *) item;
        if      (mode == 1) lb->setSelectionMode(QAbstractItemView::MultiSelection);
        else if (mode == 2) lb->setSelectionMode(QAbstractItemView::NoSelection);
        else                lb->setSelectionMode(QAbstractItemView::SingleSelection);
      }
      st = item->statusTip();
      if(!st.contains("multiselect:"))
      {
        st.append("multiselect:");
        item->setStatusTip(st);
      }
    }
    else if(isCommand("pvSetFont(") == 1)
    {
      int pointsize = ival[0];
      int bold      = ival[1];
      int italic    = ival[2];
      int underline = ival[3];
      int strikeout = ival[4];
      QFont font(qtext, pointsize);
      if(bold      == 0) font.setBold(false);
      if(italic    == 0) font.setItalic(false);
      if(underline == 0) font.setUnderline(false);
      if(strikeout == 0) font.setStrikeOut(false);
      if(bold      == 1) font.setBold(true);
      if(italic    == 1) font.setItalic(true);
      if(underline == 1) font.setUnderline(true);
      if(strikeout == 1) font.setStrikeOut(true);
      if(itemtype == TQMultiLineEdit)
      {
        MyMultiLineEdit *me = (MyMultiLineEdit *) item;
        me->setCurrentFont(font);
      }
      else
      {
        QWidget *w = (QWidget *) item;
        w->setFont(font);
      }
      st = item->statusTip();
      if(!st.contains(":font:"))
      {
        st.append("font:");
        item->setStatusTip(st);
      }
    }
    else if(isCommand("pvSetStyle(") == 1)
    {
      int shape,shadow,line_width,margin;
      shape      = ival[0];
      shadow     = ival[1];
      line_width = ival[2];
      margin     = ival[3];

      if(strstr(line,",NoFrame,")     != NULL) shape = QFrame::NoFrame;
      if(strstr(line,",Box,")         != NULL) shape = QFrame::Box;
      if(strstr(line,",Panel,")       != NULL) shape = QFrame::Panel;
      if(strstr(line,",WinPanel,")    != NULL) shape = QFrame::WinPanel;
      if(strstr(line,",HLine,")       != NULL) shape = QFrame::HLine;
      if(strstr(line,",VLine,")       != NULL) shape = QFrame::VLine;
      if(strstr(line,",StyledPanel,") != NULL) shape = QFrame::StyledPanel;

      if(strstr(line,",Plain,")  != NULL) shadow = QFrame::Plain;
      if(strstr(line,",Raised,") != NULL) shadow = QFrame::Raised;
      if(strstr(line,",Sunken,") != NULL) shadow = QFrame::Sunken;

      if(itemtype == TQLabel)
      {
        MyLabel *ptr = (MyLabel *) item;
        if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
        if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
        if(line_width != -1) ptr->setLineWidth(line_width);
        if(margin     != -1) ptr->setMargin(margin);
      }
      else if(itemtype == TQFrame)
      {
        MyFrame *ptr = (MyFrame *) item;
        if(shape      != -1) ptr->setFrameShape((QFrame::Shape) shape);
        if(shadow     != -1) ptr->setFrameShadow((QFrame::Shadow) shadow);
        if(line_width != -1) ptr->setLineWidth(line_width);
        //if(margin     != -1) ptr->setMargin(margin);
      }
    }
    else if(isCommand("pvToolTip(") == 1)
    {
      item->setToolTip(qtext);
    }
    else if(isCommand("pvAddTab(") == 1)
    {
      QWidget *w1 = (QWidget *) findChild(id); //root->findChild<QWidget *>(id);
      QWidget *w2 = (QWidget *) findChild(parent); //root->findChild<QWidget *>(parent);
      if(w1 == NULL) printf("w1==NULL\n");
      if(w2 == NULL) printf("w2==NULL\n");
      if(w1 != NULL && w1->statusTip().contains("TQTabWidget"))
      {
        MyQTabWidget *ptr = (MyQTabWidget *) w1;
        ptr->addTab(w2, qtext);
      }
      else if(w1 != NULL && w1->statusTip().contains("TQToolBox"))
      {
        MyQToolBox *ptr = (MyQToolBox *) w1;
        ptr->addItem(w2, qtext);
      }
    }
    else if(isCommand("pvSetMinSize(") == 1)
    {
      item->setMinimumSize(ival[0],ival[1]);
    }
    else if(isCommand("pvSetMaxSize(") == 1)
    {
      item->setMaximumSize(ival[0],ival[1]);
    }
    // end atrributes
    else if(isCommand("pvDownloadFile(") == 1)
    {
    }
    else if(isCommand("pvAddWidgetOrLayout(") == 1)
    {
      if(editlayout != NULL)
      {
        QString txt = QString::fromUtf8(line);
        txt.remove(" ");
        txt.remove("\n");
        editlayout->uidlg->textEditDef->append(txt);
      }
    }
    else if(isCommand("pvAddStretch(") == 1)
    {
      if(editlayout != NULL)
      {
        QString txt = QString::fromUtf8(line);
        txt.remove(" ");
        txt.remove("\n");
        editlayout->uidlg->textEditDef->append(txt);
      }
    }
    else if(isCommand("pvGlBegin(") == 1)
    {
    }
    else if(isCommand("pvGlEnd(") == 1)
    {
    }
    else if(isCommand("pvToolTip(") == 1)
    {
    }
    else if(isCommand("pvSetWhatsThis(") == 1)
    {
    }
    else if(constructorFound == 0)
    {
      printf("getWidget unknown line=%s",line);
    }
    if(iitem > 0 &&
       itemtype != TQDraw  &&
       itemtype != TQImage &&
       itemtype != TQVbox  &&
       itemtype != TQHbox  &&
       itemtype != TQGrid  &&
       isCommand("pvDownloadFile(") != 1)
    {
      if(whatsthislist.count() >= iitem)
      {
        QString qtext = whatsthislist.at(iitem-1);
        //qtext.replace('\\', "");
        item->setWhatsThis(qtext);
      }
    }
    //printf("whatsThis(%s) objectname(%s)\n"
    //       ,(const char *) item->whatsThis().toAscii()
    //       ,(const char *) item->objectName().toAscii());
  }
  while(fgets(line,sizeof(line)-1,fin) != NULL);
  return -1;
}

static int getWidgets(FILE *fin, QWidget *root)
{
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    if(strstr(line,"pvEndDefinition(p);") != NULL) return 0;
    if(strstr(line,"pv") != NULL) getWidget(fin,root);
  }
  return -1;
}

static int appendEnum()
{
  char *cptr_begin, *cptr_end;

  cptr_begin = &line[0];
  while(*cptr_begin==' ') cptr_begin++;
  cptr_end = cptr_begin;
  while(*cptr_end != ' ' && *cptr_end != ',' && *cptr_end != '\n' && *cptr_end != '\0') cptr_end++;
  *cptr_end = '\0';
  enumlist.append(cptr_begin);
  if(opt.arg_debug && cptr_begin != NULL)
  {
    printf("appendEnum:");
    printf("%s\n",cptr_begin);
  }
  return 0;
}

static int appendToolTip()
{
  char *cptr_begin, *cptr_end;

  //printf("appendToolTip=%s",line);
  cptr_begin = strchr(line,'\"');
  cptr_end   = strrchr(line,'\"');
  if(cptr_begin != NULL)
  {
    cptr_begin++;
    if(cptr_end != NULL)
    {
      *cptr_end = '\0';
      tooltiplist.append(QString::fromUtf8(cptr_begin));
    }
    else tooltiplist.append("error");
  }
  else tooltiplist.append("error");
  if(opt.arg_debug && cptr_begin != NULL)
  {
    printf("appendToolTip:");
    printf("%s\n",cptr_begin);
  }
  return 0;
}

static int appendWhatsThis()
{
  char *cptr_begin, *cptr_end;

  //printf("appendWhatsThis=%s",line);
  cptr_begin = strchr(line,'\"');
  cptr_end   = strrchr(line,'\"');
  if(cptr_begin != NULL)
  {
    cptr_begin++;
    if(cptr_end != NULL)
    {
      *cptr_end = '\0';
      whatsthislist.append(QString::fromUtf8(cptr_begin));
    }
    else whatsthislist.append("error");
  }
  else whatsthislist.append("error");
  if(opt.arg_debug && cptr_begin != NULL)
  {
    printf("appendWhatsThis:");
    printf("%s\n",cptr_begin);
  }
  return 0;
}

int getWidgetsFromMask(const char *filename, QWidget *root)
{
  FILE *fin;
  int ret, found_begin, found_start;
  int found_enum_start, found_enum_end;
  int found_tooltip_start, found_tooltip_end;
  int found_whatsthis_start, found_whatsthis_end;

  iitem = 1;
  enumlist.clear();
  tooltiplist.clear();
  whatsthislist.clear();
  tablist.clear();
  if(editlayout != NULL)
  {
    editlayout->uidlg->textEditConstructors->clear();
    editlayout->uidlg->textEditDef->clear();
  }
  ret = 0;
  fin = fopen(filename,"r");
  if(fin == NULL)
  {
    printf("getWidgetsFromMask could not open %s\n", filename);
    return -1;
  }

  // find start
  found_begin = found_start = 0;
  found_enum_start = found_enum_end = 0;
  found_tooltip_start = found_tooltip_end = 0;
  found_whatsthis_start = found_whatsthis_end = 0;
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    if(strstr(line,"_begin_of_generated_area_") != NULL) found_begin = 1;
    if(found_begin==1)
    {
      if(strstr(line,"enum") != NULL) found_enum_start = 1;
      if(found_enum_start==1 && found_enum_end==0)
      {
        if(strstr(line,"};") != NULL) found_enum_end = 1;
        else if(strstr(line,"enum") != NULL) ;
        else appendEnum();
      }

      if(strstr(line,"toolTip") != NULL) found_tooltip_start = 1;
      if(found_tooltip_start==1 && found_tooltip_end==0)
      {
        if(strstr(line,"};") != NULL) found_tooltip_end = 1;
        else if(strstr(line,"toolTip") != NULL) ;
        else appendToolTip();
      }

      if(strstr(line,"whatsThis") != NULL) found_whatsthis_start = 1;
      if(found_whatsthis_start==1 && found_whatsthis_end==0)
      {
        if(strstr(line,"};") != NULL) found_whatsthis_end = 1;
        else if(strstr(line,"whatsThis") != NULL) ;
        else appendWhatsThis();
      }

      if(strstr(line,"pvStartDefinition(p,ID_END_OF_WIDGETS);") != NULL)
      {
        found_start = 1;
        break;
      }
    }
  }

  if(found_start==1)
  {
    getWidgets(fin,root);
    while(fgets(line,sizeof(line)-1,fin) != NULL)
    {
      if(isCommand("pvTabOrder(") == 1) // get TabOrder
      {
        tablist.append(QString::fromUtf8(line));
      }
      if(strstr(line,"return") != NULL) break;
    }
  }
  else
  {
    printf("WARNING getWidgetsFromMask could not get widgets from %s\n", filename);
    ret = -1;
  }

  fclose(fin);
  enumlist.clear();
  tooltiplist.clear();
  whatsthislist.clear();
  return ret;
}

// add additional language here
#include "generatepython.h"
#include "generatephp.h"
#include "generateperl.h"
#include "generatetcl.h"

#ifdef PVWIN32
static int win32Hack(QWidget *root, const char *uifile, QList<QObject *> *widgets)
{
  if(root == NULL || uifile == NULL) return -1;
  char line[4096], *cptr;
  FILE *fin;
  theroot = root;
  int  first = 1;

  fin = fopen(uifile,"r");
  if(fin == NULL)
  {
    printf("win32Hack: could not open %s\n", uifile);
    return -1;
  }

  widgets->clear();
  while(fgets(line, sizeof(line)-1, fin) != NULL)
  {
    //cptr = strstr(line, "<widget class=");
    cptr = strstr(line, "<widget");
    if(cptr != NULL)
    {
      cptr = strstr(cptr, "name=\"");
      if(cptr == NULL)
      {
        printf("win32Hack: ERROR no name in widget line=%s\n", line);
        return -1;
      }
      else
      {
        char *name;
        name = strchr(cptr, '\"');
        name++;
        cptr = strchr(name, '\"');
        if(cptr == NULL)
        {
          printf("win32Hack: ERROR no terminatin \" in widget line=%s\n", line);
          return -1;
        }
        else
        {
          *cptr = '\0';
          if(first == 0)
          {
            printf("win32Hack:findChild(%s)\n", name);
            QWidget *widget = findChild(name);
            if(widget == NULL)
            {
              printf("win32Hack: ERROR could not find widget name=%s\n", name);
              //return -1;
            }
            else
            {
              widgets->append(widget);
            }
          }
          first = 0;
        }
      }
    }
  }
  
  fclose(fin);
  return 0;
}
#endif

static int setWidgetTree(QWidget *root, const char *uifile)
{
  if(root == NULL) return -1;
  char buf[80];
  int iobj = 1;
  root->setObjectName("ID_MAIN_WIDGET");
  QList<QObject *> widgets;

  if(uifile == NULL) return -1; // uifile only needed for win32Hack()
#ifdef PVWIN32
  win32Hack(root, uifile, &widgets);
#else
  widgets = root->findChildren<QObject *>(QRegExp("*", Qt::CaseInsensitive, QRegExp::Wildcard));
#endif
  QWidget *widget;
  printf("widgets.size() = %d\n", widgets.size());
  for(int i=0; i<widgets.size(); i++)
  {
    widget = (QWidget *) widgets.at(i);
    if(widget->isWidgetType())
    {
      if(widget->objectName().startsWith("qt_"))
      {
        //printf("qt_: %s\n", (const char *) widget->objectName().toAscii());
        sprintf(buf,"qtobj%d", iobj++);
        widget->setObjectName(buf);
      }
      else if(widget->objectName() == "")
      {
        //printf(": %s\n", (const char *) widget->objectName().toAscii());
        sprintf(buf,"qtobj%d", iobj++);
        widget->setObjectName(buf);
      }
      else if(widget->inherits("QPushButton"))
      {
        widget->setStatusTip("TQPushButton:");
      }
      else if(widget->inherits("QLineEdit"))
      {
        widget->setStatusTip("TQLineEdit:");
      }
      else if(widget->inherits("QComboBox"))
      {
        widget->setStatusTip("TQComboBox:");
      }
      else if(widget->inherits("QLCDNumber"))
      {
        widget->setStatusTip("TQLCDNumber:");
      }
      else if(widget->inherits("Qunknown"))
      {
        widget->setStatusTip("TQButtonGroup:");
      }
      else if(widget->inherits("QRadioButton"))
      {
        widget->setStatusTip("TQRadio:");
      }
      else if(widget->inherits("QCheckBox"))
      {
        widget->setStatusTip("TQCheck:");
      }
      else if(widget->inherits("QSlider"))
      {
        widget->setStatusTip("TQSlider:");
      }
      else if(widget->inherits("QTableWidget"))
      {
        widget->setStatusTip("TQTable:");
      }
      else if(widget->inherits("PvbImage"))
      {
        widget->setStatusTip("TQImage:");
      }
      else if(widget->inherits("PvbDraw"))
      {
        widget->setStatusTip("TQDraw:");
      }
      else if(widget->inherits("PvbOpengl"))
      {
        widget->setStatusTip("TQGl:");
      }
      else if(widget->inherits("QTabWidget"))
      {
        widget->setStatusTip("TQTabWidget:");
      }
      else if(widget->inherits("QGroupBox"))
      {
        widget->setStatusTip("TQGroupBox:");
      }
      else if(widget->inherits("Qunknown"))
      {
        widget->setStatusTip("TQListBox:");
      }
      else if(widget->inherits("QSpinBox"))
      {
        widget->setStatusTip("TQSpinBox:");
      }
      else if(widget->inherits("QDial"))
      {
        widget->setStatusTip("TQDial:");
      }
      else if(widget->inherits("QProgressBar"))
      {
        widget->setStatusTip("TQProgressBar:");
      }
      else if(widget->inherits("QTextBrowser"))
      {
        widget->setStatusTip("TQTextBrowser:");
      }
      else if(widget->inherits("QTextEdit"))
      {
        widget->setStatusTip("TQMultiLineEdit:");
      }
      else if(widget->inherits("QListView"))
      {
        widget->setStatusTip("TQListView:");
      }
      else if(widget->inherits("QListWidget"))
      {
        widget->setStatusTip("TQListView:");
      }
      else if(widget->inherits("PvbIconview"))
      {
        widget->setStatusTip("TQIconView:");
      }
      else if(widget->inherits("PvbVtk"))
      {
        widget->setStatusTip("TQVtk:");
      }
      else if(widget->inherits("QwtPlot"))
      {
        widget->setStatusTip("TQwtPlotWidget:");
      }
      else if(widget->inherits("Qunknown"))
      {
        widget->setStatusTip("TQwtScale:");
      }
      else if(widget->inherits("QwtThermo"))
      {
        widget->setStatusTip("TQwtThermo:");
      }
      else if(widget->inherits("QwtKnob"))
      {
        widget->setStatusTip("TQwtKnob:");
      }
      else if(widget->inherits("QwtCounter"))
      {
        widget->setStatusTip("TQwtCounter:");
      }
      else if(widget->inherits("QwtWheel"))
      {
        widget->setStatusTip("TQwtWheel:");
      }
      else if(widget->inherits("QwtSlider"))
      {
        widget->setStatusTip("TQwtSlider:");
      }
      else if(widget->inherits("QwtCompass"))
      {
        widget->setStatusTip("TQwtCompass:");
      }
      else if(widget->inherits("QwtAnalogClock"))
      {
        widget->setStatusTip("TQwtAnalogClock:");
      }
      else if(widget->inherits("QwtDial"))
      {
        widget->setStatusTip("TQwtDial:");
      }
      else if(widget->inherits("QDateEdit"))
      {
        widget->setStatusTip("TQDateEdit:");
      }
      else if(widget->inherits("QTimeEdit"))
      {
        widget->setStatusTip("TQTimeEdit:");
      }
      else if(widget->inherits("QDateTimeEdit"))
      {
        widget->setStatusTip("TQDateTimeEdit:");
      }
      else if(widget->inherits("QToolBox"))
      {
        widget->setStatusTip("TQToolBox:");
      }
      else if(widget->inherits("Q"))
      {
        widget->setStatusTip("TQVbox:");
      }
      else if(widget->inherits("Q"))
      {
        widget->setStatusTip("TQHbox:");
      }
      else if(widget->inherits("Q"))
      {
        widget->setStatusTip("TQGrid:");
      }
      else if(widget->inherits("QLabel"))
      {
        widget->setStatusTip("TQLabel:");
      }
      else if(widget->inherits("QFrame"))
      {
        widget->setStatusTip("TQFrame:");
      }
      else
      {
        widget->setStatusTip("TQWidget:");
      }
    }
    else // not a widget type
    {
      //printf("not a widget type %s\n", (const char *) widget->objectName().toAscii());
      if     (widget->inherits("QVBoxLayout"))
      {
        printf("TQVbox\n");
        //widget->setStatusTip("TQVbox:");
      }
      else if(widget->inherits("QHBoxLayout"))
      {
        printf("TQHbox\n");
        //widget->setStatusTip("TQHbox:");
      }
      else if(widget->inherits("QGridLayout"))
      {
        printf("TQGrid\n");
        //widget->setStatusTip("TQGrid:");
      }
    }
  }
  return 0;
}

static int perhapsSetTabOrder(const char *uifile)
{
  FILE *fin;
  char *cptrbegin, *cptrend;
  QString lastTab, currentTab;

  tablist.clear();
  fin = fopen(uifile,"r");
  if(fin == NULL)
  {
    printf("could not open %s\n", uifile);
    return -1;
  }

  while(fgets(line, sizeof(line)-1, fin) != NULL)
  {
    if(strstr(line,"<tabstop>") != NULL)
    {
      cptrbegin = strchr(line,'>');
      if(cptrbegin != NULL)
      {
        cptrbegin++;
        cptrend = strchr(cptrbegin,'<');
        if(cptrend != NULL)
        {
          *cptrend = '\0';
          if(lastTab == "")
          {
            lastTab = cptrbegin;
          }
          else
          {
            currentTab = cptrbegin;
            tablist.append("  pvTabOrder(p," + lastTab + "," + currentTab + ");\n");
            lastTab = currentTab;
          }  
        }
      }
    }
  }

  fclose(fin);
  return 0;
}

static int perhapsSetLayout(const char *cppfile)
{
  FILE *fin;
  if(editlayout == NULL) return -1;
  QTextEdit *e = editlayout->uidlg->textEditConstructors;
  int found = 0;

  e->clear();
  fin = fopen(cppfile,"r");
  if(fin == NULL)
  {
    printf("could not open %s\n", cppfile);
    return -1;
  }

  while(fgets(line, sizeof(line)-1, fin) != NULL)
  {
    if(strstr(line,"pvQLayout")       != NULL) found = 1;
    if(strstr(line,"pvEndDefinition") != NULL) break;
    if(found == 1)
    {
      e->append(line);
    }
  }

  fclose(fin);
  return 0;
}

int importUi(const char *uifile, Designer *designer)
{
  char filename[80], *cptr;
  int imask;
  QWidget *root;

  printf("importUi(%s)\n", uifile);
  if(strlen(uifile) > 70)
  {
    printf("filename too long %s\n", uifile);
    return -1;
  }
  perhapsSetTabOrder(uifile);
  QFile ui(uifile);
  bool ret = ui.open(QIODevice::ReadOnly);
  if(ret == false)
  {
    printf("could not open %s\n", uifile);
    return -1;
  }
  QUiLoader uiloader(designer->root);
  if( (root = uiloader.load(&ui)) == NULL)
  {
    printf("could not load ui file %s\n", uifile);
    return -1;
  }
  strcpy(filename, uifile);
  cptr = strchr(filename, '.');
  if(cptr != NULL) *cptr = '\0';
  strcat(filename,".cpp");
  sscanf(filename, "mask%d", &imask);
  perhapsSetLayout(filename);
  if(setWidgetTree(root, uifile) < 0)
  {
    printf("error: setWidgetTree %s\n", uifile);
    return -1;
  }
  designer->root = (MyRootWidget *) root;
  generateMask(filename, designer->root);
  // add additional language here
  if(opt.script == PV_PYTHON)
  {
    generatePython(imask, designer->root);
  }
  if(opt.script == PV_PHP)
  {
    generatePHP(imask, designer->root);
  }
  if(opt.script == PV_PERL)
  {
    generatePerl(imask, designer->root);
  }
  if(opt.script == PV_TCL)
  {
    generateTcl(imask, designer->root);
  }
  designer->root->modified = 0;
  return 0;
}


