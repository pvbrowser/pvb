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
#include "dlginsertwidget.h"
#include "MyWidgets.h"
#include "qimagewidget.h"
#include "qdrawwidget.h"
#include "qwtplotwidget.h"
#include "qwtwidgets.h"
#include "opt.h"
#include <qlcdnumber.h>

extern OPT_DEVELOP opt_develop;

dlgInsertWidget::dlgInsertWidget()
{
  form = new Ui_DialogInsertWidget;
  form->setupUi(this);

  form->comboBoxTabWidget->addItem("North");
  form->comboBoxTabWidget->addItem("South");
  form->comboBoxTabWidget->addItem("West");
  form->comboBoxTabWidget->addItem("East");

  form->comboBoxMode->addItem("HEX");
  form->comboBoxMode->addItem("DEC");
  form->comboBoxMode->addItem("OCT");
  form->comboBoxMode->addItem("BIN");

  form->comboBoxSegmentStyle->addItem("Outline");
  form->comboBoxSegmentStyle->addItem("Filled");
  form->comboBoxSegmentStyle->addItem("Flat");

  form->comboBoxSliderOrientation->addItem("Horizontal");
  form->comboBoxSliderOrientation->addItem("Vertical");

  //form->comboBoxFrameShape->addItem("NoFrame");
  form->comboBoxFrameShape->addItem("Box");
  form->comboBoxFrameShape->addItem("Panel");
  form->comboBoxFrameShape->addItem("WinPanel");
  form->comboBoxFrameShape->addItem("HLine");
  form->comboBoxFrameShape->addItem("VLine");
  form->comboBoxFrameShape->addItem("StyledPanel");

  form->comboBoxFrameShadow->addItem("Plain");
  form->comboBoxFrameShadow->addItem("Raised");
  form->comboBoxFrameShadow->addItem("Sunken");

  form->comboBoxProgressBarOrientation->addItem("Horizontal");
  form->comboBoxProgressBarOrientation->addItem("Vertical");

  ret = 1;
  myrootwidget = NULL;

  QObject::connect(form->okButton,     SIGNAL(clicked()),SLOT(slotOk()));
  QObject::connect(form->cancelButton, SIGNAL(clicked()),SLOT(slotCancel()));
}

dlgInsertWidget::~dlgInsertWidget()
{
}

void dlgInsertWidget::run()
{
  exec();
}

static int nullSocket = -1;

QWidget *dlgInsertWidget::newWidget(QWidget *root, QWidget *parent, int x, int y)
{
  QWidget *w = NULL;
  if(parent == NULL) return NULL;
  ret = 1;

  if(opt_develop.arg_debug) printf("newWidget at(%d,%d)\n",x,y);
  if     (form->wTabWidget->isChecked())
  {
    bool ok;
    if(myrootwidget != NULL)
    {
      myrootwidget->releaseMouse();
      myrootwidget->releaseKeyboard();
    }  
    QString text = QInputDialog::getText(this, "pvdevelop: Add TabWidget",
                                               "Title of first tab", QLineEdit::Normal,
                                               "", &ok);
    if(ok && !text.isEmpty())
    {
      w = new MyQTabWidget(&nullSocket,0,parent);
      w->setStatusTip("TQTabWidget:");
      if(form->comboBoxTabWidget->currentText() == "North")
        ((MyQTabWidget *)w)->setTabPosition(QTabWidget::North);
      if(form->comboBoxTabWidget->currentText() == "South")
        ((MyQTabWidget *)w)->setTabPosition(QTabWidget::South);
      if(form->comboBoxTabWidget->currentText() == "West")
        ((MyQTabWidget *)w)->setTabPosition(QTabWidget::West);
      if(form->comboBoxTabWidget->currentText() == "East")
        ((MyQTabWidget *)w)->setTabPosition(QTabWidget::East);
      QWidget *tab = new QWidget();
      tab->setStatusTip("TQWidget:");
      setDefaultObjectName(root,tab);
      ((MyQTabWidget *)w)->addTab(tab,text);
    }
  }
  else if(form->wLabel->isChecked())
  {
    w = new MyLabel(&nullSocket,0,parent);
    ((MyLabel *)w)->setText("Label");
    w->setStatusTip("TQLabel:");
  }
  else if(form->wPushButton->isChecked())
  {
    w = new MyQPushButton(&nullSocket,0,parent);
    ((MyQPushButton *)w)->setText("PushButton");
    w->setStatusTip("TQPushButton:");
  }
  else if(form->wRadioButton->isChecked())
  {
    w = new MyRadioButton(&nullSocket,0,parent);
    ((MyRadioButton *)w)->setText("RadioButton");
    w->setStatusTip("TQRadio:");
  }
  else if(form->wCheckBox->isChecked())
  {
    w = new MyCheckBox(&nullSocket,0,parent);
    ((MyCheckBox *)w)->setText("CheckBox");
    w->setStatusTip("TQCheck:");
  }
  else if(form->wLineEdit->isChecked())
  {
    w = new MyLineEdit(&nullSocket,0,parent);
    ((MyLineEdit *)w)->setText("LineEdit");
    w->setStatusTip("TQLineEdit:");
  }
  else if(form->wSpinBox->isChecked())
  {
    int min  = form->spinBoxMin->value();
    int max  = form->spinBoxMax->value();
    int step = form->spinBoxStep->value();
    w = new MySpinBox(&nullSocket,0,min,max,step,parent);
    w->setStatusTip("TQSpinBox:");
  }
  else if(form->wComboxBox->isChecked())
  {
    w = new MyComboBox(&nullSocket,0,parent);
    w->setStatusTip("TQComboBox:");
  }
  else if(form->wLCDNumber->isChecked())
  {
    int numDigits = form->spinBoxNumDigits->value();
    int mode = QLCDNumber::Hex;
    int segmentStyle = QLCDNumber::Outline;
    if(form->comboBoxMode->currentText() == "DEC") mode = QLCDNumber::Dec;
    if(form->comboBoxMode->currentText() == "OCT") mode = QLCDNumber::Oct;
    if(form->comboBoxMode->currentText() == "BIN") mode = QLCDNumber::Bin;
    if(form->comboBoxSegmentStyle->currentText() == "Filled") segmentStyle = QLCDNumber::Filled;
    if(form->comboBoxSegmentStyle->currentText() == "Flat")   segmentStyle = QLCDNumber::Flat;
    w = new QLCDNumber(numDigits,parent);
    ((QLCDNumber *)w)->setMode((QLCDNumber::Mode) mode);
    ((QLCDNumber *)w)->setSegmentStyle((QLCDNumber::SegmentStyle) segmentStyle);
    w->setStatusTip("TQLCDNumber:");
  }
  else if(form->wSlider->isChecked())
  {
    int min  = form->spinBoxSliderMin->value();
    int max  = form->spinBoxSliderMax->value();
    int step = form->spinBoxSliderStep->value();
    int val  = form->spinBoxSliderVal->value();
    int ori  = Qt::Horizontal;
    if(form->comboBoxSliderOrientation->currentText() == "Vertical") ori = Qt::Vertical;
    w = new MySlider(&nullSocket,0,min,max,step,val,(Qt::Orientation) ori,parent);
    w->setStatusTip("TQSlider:");
  }
  else if(form->wDial->isChecked())
  {
    int min  = form->spinBoxDialMin->value();
    int max  = form->spinBoxDialMax->value();
    int step = form->spinBoxDialStep->value();
    int val  = form->spinBoxDialValue->value();
    w = new MyDial(&nullSocket,0,min,max,step,val,parent);
    w->setStatusTip("TQDial:");
  }
  else if(form->wProgressBar->isChecked())
  {
    int totalSteps = form->spinBoxProgressBarTotalSteps->value();
    Qt::Orientation ori  = Qt::Horizontal;
    if(form->comboBoxProgressBarOrientation->currentText() == "Vertical") ori = Qt::Vertical;
    w = new MyProgressBar(&nullSocket,0,totalSteps,ori,parent);
    w->setStatusTip("TQProgressBar:");
  }
  else if(form->wDateEdit->isChecked())
  {
    w = new MyQDateEdit(&nullSocket,0,parent);
    w->setStatusTip("TQDateEdit:");
  }
  else if(form->wTimeEdit->isChecked())
  {
    w = new MyQTimeEdit(&nullSocket,0,parent);
    w->setStatusTip("TQTimeEdit:");
  }
  else if(form->wDateTimeEdit->isChecked())
  {
    w = new MyQDateTimeEdit(&nullSocket,0,parent);
    w->setStatusTip("TQDateTimeEdit:");
  }
  else if(form->wGroupBox->isChecked())
  {
    int ori  = Qt::Horizontal; // not used in Qt4
    int columns = 1;           // not used in Qt4
    QString title = form->lineEditGroupBoxTitle->text();
    w = new MyGroupBox(&nullSocket,0,columns,(Qt::Orientation) ori,title,parent);
    w->setStatusTip("TQGroupBox:");
  }
  else if(form->wFrame->isChecked())
  {
    int shape = QFrame::NoFrame;
    if(form->comboBoxFrameShape->currentText() == "Box")         shape = QFrame::Box;
    if(form->comboBoxFrameShape->currentText() == "Panel")       shape = QFrame::Panel;
    if(form->comboBoxFrameShape->currentText() == "WinPanel")    shape = QFrame::WinPanel;
    if(form->comboBoxFrameShape->currentText() == "HLine")       shape = QFrame::HLine;
    if(form->comboBoxFrameShape->currentText() == "VLine")       shape = QFrame::VLine;
    if(form->comboBoxFrameShape->currentText() == "StyledPanel") shape = QFrame::StyledPanel;
    int shadow = QFrame::Plain;
    if(form->comboBoxFrameShadow->currentText() == "Raised") shadow = QFrame::Raised;
    if(form->comboBoxFrameShadow->currentText() == "Sunken") shadow = QFrame::Sunken;
    int line_width = form->spinBoxFrameLineWidth->value();
    int margin = 1; // not used in Qt4
    w = new MyFrame(&nullSocket,0,(QFrame::Shape) shape, (QFrame::Shadow) shadow,line_width,margin,parent);
    w->setStatusTip("TQFrame:");
  }
  else if(form->wTable->isChecked())
  {
    int numRows = 2;
    int numColumns = 2;
    w = new MyTable(&nullSocket,0,numRows,numColumns,parent);
    w->setStatusTip("TQTable:");
  }
  else if(form->wMultiLineEdit->isChecked())
  {
    int editable = form->spinBoxMultiLineEditable->value();
    int maxLines = form->spinBoxMultiLineMaxLines->value();
    w = new MyMultiLineEdit(&nullSocket,0,editable,maxLines,parent);
    w->setStatusTip("TQMultiLineEdit:");
  }
  else if(form->wTextBrowser->isChecked())
  {
    w = new QTextBrowser(parent);
    w->setStatusTip("TQTextBrowser:");
  }
  else if(form->wListView->isChecked())
  {
    w = new MyListView(&nullSocket,0,parent);
    w->setStatusTip("TQListView:");
  }
  else if(form->wListBox->isChecked())
  {
    w = new MyListBox(&nullSocket,0,parent);
    w->setStatusTip("TQListBox:");
  }
  else if(form->wIconView->isChecked())
  {
    w = new MyIconView(&nullSocket,0,parent);
    w->setStatusTip("TQIconView:");
  }
  else if(form->wImage->isChecked())
  {
    QString filename = form->lineEditImageFileName->text();
    QImageWidget *image = new QImageWidget(&nullSocket,0,parent);
    w = image;
    if(!filename.isEmpty())
    {
      image->setWhatsThis(filename);
    }
    //qt3 image->setBackgroundMode(Qt::FixedColor);
    image->setAutoFillBackground(true);
    image->setPalette(QPalette(QColor(128,0,0)));
    w->setStatusTip("TQImage:");
  }
  else if(form->wDraw->isChecked())
  {
    QDrawWidget *draw = new QDrawWidget(parent,"draw",0,&nullSocket,0);
    w = draw;
    QString filename = form->lineEditDrawFileName->text();
    if(!filename.isEmpty())
    {
      draw->setWhatsThis(filename);
    }
    w->setStatusTip("TQDraw:");
  }
  else if(form->wOpenGL->isChecked())
  {
    int shape = QFrame::Panel;
    int shadow = QFrame::Raised;
    int line_width = 10;
    int margin = 1; // not used in Qt4
    w = new MyFrame(&nullSocket,0,(QFrame::Shape) shape, (QFrame::Shadow) shadow,line_width,margin,parent);
    w->setStatusTip("TQGl:");
  }
  else if(form->wVTK->isChecked())
  {
    int shape = QFrame::Panel;
    int shadow = QFrame::Sunken;
    int line_width = 10;
    int margin = 1; // not used in Qt4
    w = new MyFrame(&nullSocket,0,(QFrame::Shape) shape, (QFrame::Shadow) shadow,line_width,margin,parent);
    w->setStatusTip("TQVtk:");
  }
  else if(form->wToolBox->isChecked())
  {
    QString text;
    bool ok;
    if(myrootwidget != NULL)
    {
      myrootwidget->releaseMouse();
      myrootwidget->releaseKeyboard();
    }  
    text = QInputDialog::getText(this, "pvdevelop: Add ToolBox",
                                       "Title of first item", QLineEdit::Normal,
                                       "", &ok);
    if(ok && !text.isEmpty())
    {
      w = new MyQToolBox(&nullSocket,0,parent);
      w->setStatusTip("TQToolBox:");
      QWidget *item = new QWidget();
      item->setStatusTip("TQWidget:");
      setDefaultObjectName(root,item);
      ((MyQToolBox *)w)->addItem(item,text);
    }
    if(myrootwidget != NULL)
    {
      myrootwidget->releaseMouse();
      myrootwidget->releaseKeyboard();
    }  
    text = QInputDialog::getText(this, "pvdevelop: Add ToolBox",
                                       "Title of second item", QLineEdit::Normal,
                                       "", &ok);
    if(ok && !text.isEmpty())
    {
      QWidget *item = new QWidget();
      item->setStatusTip("TQWidget:");
      setDefaultObjectName(root,item);
      ((MyQToolBox *)w)->addItem(item,text);
    }
  }
  else if(form->wQwtPlot->isChecked())
  {
    w = new QwtPlotWidget(&nullSocket,0,parent);
    w->setStatusTip("TQwtPlotWidget:");
  }
  else if(form->wQwtThermo->isChecked())
  {
    w = new MyQwtThermo(&nullSocket,0,parent);
    w->setStatusTip("TQwtThermo:");
  }
  else if(form->wQwtKnob->isChecked())
  {
    w = new MyQwtKnob(&nullSocket,0,parent);
    w->setStatusTip("TQwtKnob:");
  }
  else if(form->wQwtCounter->isChecked())
  {
    w = new MyQwtCounter(&nullSocket,0,parent);
    w->setStatusTip("TQwtCounter:");
  }
  else if(form->wQwtWheel->isChecked())
  {
    w = new MyQwtWheel(&nullSocket,0,parent);
    w->setStatusTip("TQwtWheel:");
  }
  else if(form->wQwtSlider->isChecked())
  {
    w = new MyQwtSlider(&nullSocket,0,parent);
    w->setStatusTip("TQwtSlider:");
  }
  else if(form->wQwtCompass->isChecked())
  {
    w = new MyQwtCompass(&nullSocket,0,parent);
    w->setStatusTip("TQwtCompass:");
  }
  else if(form->wQwtDial->isChecked())
  {
    w = new MyQwtDial(&nullSocket,0,parent);
    w->setStatusTip("TQwtDial:");
  }
  else if(form->wQwtAnalogClock->isChecked())
  {
    w = new MyQwtAnalogClock(&nullSocket,0,parent);
    w->setStatusTip("TQwtAnalogClock:");
  }
  else if(form->wCustomWidget->isChecked())
  {
    int shape = QFrame::Panel;
    int shadow = QFrame::Raised;
    int line_width = 5;
    int margin = 1; // not used in Qt4
    w = new MyFrame(&nullSocket,0,(QFrame::Shape) shape, (QFrame::Shadow) shadow,line_width,margin,parent);
    w->setToolTip("Set whatsThis='/library/widgettype:arguments'");
    w->setWhatsThis(form->lineEditCustomWidget->text());
    w->setStatusTip("TQCustomWidget:");
    QLabel *xitem = new QLabel(w);
    xitem->setGeometry(5,2,4096,50);
    xitem->setText(form->lineEditCustomWidget->text());
  }
  else
  {
    printf("newWidget: no radio checked !\n");
  }

  if(w != NULL)
  {
    int width,height;
    x = (x/opt_develop.xGrid)*opt_develop.xGrid;
    y = (y/opt_develop.yGrid)*opt_develop.yGrid;
    width  = (100/opt_develop.xGrid)*opt_develop.xGrid;
    height = (30/opt_develop.yGrid)*opt_develop.yGrid;
    if(width  < opt_develop.xGrid) width  = opt_develop.xGrid;
    if(height < opt_develop.yGrid) height = opt_develop.yGrid;
    mySetGeometry(w,x,y,width,height);
    setDefaultObjectName(root,w);
    w->show();
  }

  return w;
}

void myMove(QWidget *w, int x, int y)
{
  if(w == NULL)
  {
    return;
  }
  w->move(x,y);
}

void myResize(QWidget *w, int width, int height)
{
  if(w == NULL) return;
  if(w->statusTip().startsWith("TQImage:") ||
     w->statusTip().startsWith("TQDraw:")
    )
  {
    int x = w->x();
    int y = w->y();
    mySetGeometry(w,x,y,width,height);
  }
  else
  {
    w->resize(width,height);
  }
}

void mySetGeometry(QWidget *w, int x, int y, int width, int height)
{
  if(w == NULL) return;
  if(w->statusTip().startsWith("TQImage:"))
  {
    QImageWidget *image = (QImageWidget *) w;
    image->setGeometry(x,y,width,height);
    QString filename = w->whatsThis();
    if(!filename.isEmpty()) image->setImage(filename.toUtf8());
    image->clearMask();
  }
  else if(w->statusTip().startsWith("TQDraw:"))
  {
    QDrawWidget *draw = (QDrawWidget *) w;
    draw->setGeometry(x,y,width,height);
    draw->resize(width,height);
    QString filename = w->whatsThis();
    if(filename.isEmpty())
    {
      draw->beginDraw(1);
      draw->setBackgroundColor(128,128,0);
      draw->endDraw();
    }
    else
    {
      if(opt_develop.arg_debug) printf("draw->playSVG(%s)\n", (const char *) filename.toUtf8());
      draw->beginDraw(1);
      draw->setBackgroundColor(255,255,255);
      draw->playSVG(filename.toUtf8());
      draw->endDraw();
    }
  }
  else
  {
    //printf("mySetGeometry(%s,%d,%d,%d,%d)\n",
    //      (const char *)w->statusTip().toUtf8(), x,y,width,height);
    w->setGeometry(x,y,width,height);
  }
}

void dlgInsertWidget::findMax(QObject *w, int *max)
{
  int val;
  QString name;
  QObject *subitem;
  QList<QObject *> levellist;
  levellist.clear();
  levellist = w->children();
  for(int i=0; i<levellist.size(); i++)
  {
    subitem = levellist.at(i);
    name = subitem->objectName();
    if(name.startsWith("obj"))
    {
      if(name.length() < 80)
      {
        char buf[81];
        strcpy(buf,name.toUtf8());
        if(isdigit(buf[3]))
        {
          sscanf(buf,"obj%d",&val);
          if(val > *max) *max = val;
        }
      }
    }
    findMax(subitem,max);
  }
}

void dlgInsertWidget::setDefaultObjectName(QWidget *root, QWidget *item)
{
  int max = 0;
  findMax(root,&max);
  max++;
  QString name;
  //rlmurx-was-here name.sprintf("obj%d",max);
  name = QString::asprintf("obj%d",max);
  item->setObjectName(name);
}

void dlgInsertWidget::slotOk()
{
  ret = 1;
  accept();
}

void dlgInsertWidget::slotCancel()
{
  ret = 0;
  reject();
}
