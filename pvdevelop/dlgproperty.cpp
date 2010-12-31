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
#include "dlgproperty.h"
#include "dlginsertwidget.h"
#include "MyWidgets.h"
#include "interpreter.h"

dlgProperty::dlgProperty(QWidget *_widget)
{
  QString buf;
  form = new Ui_DialogProperty;
  form->setupUi(this);
  widget = _widget;
  modified = ret = 0;
  QString type(widget->statusTip());

  QString st = widget->statusTip();
  form->labelType->setText(st.section(':',0));
  form->spinBoxX->setValue(widget->x());
  form->spinBoxY->setValue(widget->y());
  form->spinBoxW->setValue(widget->width());
  form->spinBoxH->setValue(widget->height());
  form->spinBoxMinWidth->setValue(widget->minimumWidth());
  form->spinBoxMinHeight->setValue(widget->minimumHeight());
  form->spinBoxMaxWidth->setValue(widget->maximumWidth());
  form->spinBoxMaxHeight->setValue(widget->maximumHeight());

  form->lineEditObjectName->setText(widget->objectName());
  buf = widget->whatsThis();
  form->lineEditWhatsThis->setText(buf);
  buf = widget->toolTip();
  form->lineEditToolTip->setText(buf);

  QFont font = widget->font();
  if(font.italic()) form->checkBoxFontItalic->setCheckState(Qt::Checked);
  else              form->checkBoxFontItalic->setCheckState(Qt::Unchecked);
  if(font.underline()) form->checkBoxFontUnderline->setCheckState(Qt::Checked);
  else                 form->checkBoxFontUnderline->setCheckState(Qt::Unchecked);
  if(font.strikeOut()) form->checkBoxFontStrikeout->setCheckState(Qt::Checked);
  else                 form->checkBoxFontStrikeout->setCheckState(Qt::Unchecked);
  if(font.bold()) form->checkBoxFontBold->setCheckState(Qt::Checked);
  else            form->checkBoxFontBold->setCheckState(Qt::Unchecked);
  form->spinBoxFontSize->setValue(font.pointSize());
  QString fontfamily = font.family();
#ifdef PVUNIX
  if(fontfamily == "Sans Serif")
#endif
#ifdef PVWIN32
  if(fontfamily == "MS Shell Dlg 2")
#endif
  {
    fontfamily = "default";
    form->comboBoxFontFamily->addItem(fontfamily);
  }
  else
  {
    form->comboBoxFontFamily->addItem(fontfamily);
    form->comboBoxFontFamily->addItem("default");
  }
  form->comboBoxFontFamily->addItem("Arial");
  form->comboBoxFontFamily->addItem("Times");
  form->comboBoxFontFamily->addItem("Courier");
  form->comboBoxFontFamily->setEditable(true);

  QColor fgColor;
  QColor bgColor;
  if(type.contains("TQPushButton:"))      fgColor = widget->palette().color(QPalette::ButtonText);
  else                                    fgColor = widget->palette().color(QPalette::WindowText);
  if     (type.contains("TQPushButton:")   ||
          type.contains("TQRadio:")        ||
          type.contains("TQCheck:")        )
                                          bgColor = widget->palette().color(QPalette::Button);
  else if(type.contains("TQMultiLineEdit") ||
          type.contains("TQLineEdit")      ||
          type.contains("TQTextBrowser")   )
                                          bgColor = widget->palette().color(QPalette::Base);
  else                                    bgColor = widget->palette().color(QPalette::Window);
  if(widget->statusTip().contains("foreground"))
  {
    form->spinBoxFgRed->setValue(fgColor.red());
    form->spinBoxFgGreen->setValue(fgColor.green());
    form->spinBoxFgBlue->setValue(fgColor.blue());
  }
  else
  {
    form->spinBoxFgRed->setValue(-1);
    form->spinBoxFgGreen->setValue(-1);
    form->spinBoxFgBlue->setValue(-1);
  }
  if(widget->statusTip().contains("background"))
  {
    form->spinBoxBgRed->setValue(bgColor.red());
    form->spinBoxBgGreen->setValue(bgColor.green());
    form->spinBoxBgBlue->setValue(bgColor.blue());
  }
  else
  {
    form->spinBoxBgRed->setValue(-1);
    form->spinBoxBgGreen->setValue(-1);
    form->spinBoxBgBlue->setValue(-1);
  }

  if(type.contains("TQLabel:"))
  {
    form->lineEditText->setText(((MyLabel *)widget)->text());
  }
  else if(type.contains("TQLineEdit:"))
  {
    form->lineEditText->setText(((MyLineEdit *)widget)->text());
  }
  else if(type.contains("TQPushButton:"))
  {
    form->lineEditText->setText(((MyQPushButton *)widget)->text());
  }
  else if(type.contains("TQRadio:"))
  {
    form->lineEditText->setText(((MyRadioButton *)widget)->text());
  }
  else if(type.contains("TQCheck:"))
  {
    form->lineEditText->setText(((MyCheckBox *)widget)->text());
  }
  else if(type.contains("TQButtonGroup:"))
  {
    form->lineEditText->setText(((MyButtonGroup *)widget)->title());
  }
  else if(type.contains("TQGroupBox:"))
  {
    form->lineEditText->setText(((MyGroupBox *)widget)->title());
  }
  else
  {
    form->lineEditText->hide();
  }
  QString quote = form->lineEditText->text().replace("\n","\\n");
  form->lineEditText->setText(quote);

  if(type.contains("TQLabel:") || type.contains("TQFrame:"))
  {
    form->comboBoxShape->insertItem(0,"NoFrame");
    form->comboBoxShape->insertItem(1,"Box");
    form->comboBoxShape->insertItem(2,"Panel");
    form->comboBoxShape->insertItem(3,"WinPanel");
    form->comboBoxShape->insertItem(5,"HLine");
    form->comboBoxShape->insertItem(5,"VLine");
    form->comboBoxShape->insertItem(6,"StyledPanel");
    form->comboBoxShadow->insertItem(0,"Plain");
    form->comboBoxShadow->insertItem(1,"Raised");
    form->comboBoxShadow->insertItem(2,"Sunken");
    if(type.contains("TQFrame:")) form->spinBoxMargin->hide();
    if(type.contains("TQLabel:"))
    {
      MyLabel *label = (MyLabel *) widget;
      int index = 0;
      if     (label->frameShape() == QFrame::Box)         index = 1;
      else if(label->frameShape() == QFrame::Panel)       index = 2;
      else if(label->frameShape() == QFrame::WinPanel)    index = 3;
      else if(label->frameShape() == QFrame::HLine)       index = 4;
      else if(label->frameShape() == QFrame::VLine)       index = 5;
      else if(label->frameShape() == QFrame::StyledPanel) index = 6;
      form->comboBoxShape->setCurrentIndex(index);

      index = 0;
      if     (label->frameShadow() == QFrame::Raised) index = 1;
      else if(label->frameShadow() == QFrame::Sunken) index = 2;
      form->comboBoxShadow->setCurrentIndex(index);

      form->spinBoxLinewidth->setValue(label->lineWidth());
      form->spinBoxMargin->setValue(label->margin());
    }
    if(type.contains("TQFrame:"))
    {
      MyFrame *frame = (MyFrame *) widget;

      int index = 0;
      if     (frame->frameShape() == QFrame::Box)         index = 1;
      else if(frame->frameShape() == QFrame::Panel)       index = 2;
      else if(frame->frameShape() == QFrame::WinPanel)    index = 3;
      else if(frame->frameShape() == QFrame::HLine)       index = 4;
      else if(frame->frameShape() == QFrame::VLine)       index = 5;
      else if(frame->frameShape() == QFrame::StyledPanel) index = 6;
      form->comboBoxShape->setCurrentIndex(index);

      index = 0;
      if     (frame->frameShadow() == QFrame::Raised) index = 1;
      else if(frame->frameShadow() == QFrame::Sunken) index = 2;
      form->comboBoxShadow->setCurrentIndex(index);

      form->spinBoxLinewidth->setValue(frame->lineWidth());
      //form->spinBoxMargin->setValue(frame->margin());
    }
  }
  else
  {
    form->comboBoxShape->hide();
    form->comboBoxShadow->hide();
    form->spinBoxLinewidth->hide();
    form->spinBoxMargin->hide();
    form->labelStyle->hide();
    form->labelLinewidthMargin->hide();
  }

  if(type.contains("TQTable:"))
  {
    form->spinBoxNumRows->setValue(((MyTable *)widget)->rowCount());
    form->spinBoxNumColumns->setValue(((MyTable *)widget)->columnCount());
  }
  else
  {
    form->labelNumRows->hide();
    form->labelNumColumns->hide();
    form->spinBoxNumRows->hide();
    form->spinBoxNumColumns->hide();
  }

  if(type.contains("TQListBox:") || type.contains("TQListView:"))
  {
    form->comboBoxMultiSection->insertItem(0,"SingleSelection");
    form->comboBoxMultiSection->insertItem(1,"MultiSelection");
    form->comboBoxMultiSection->insertItem(2,"NoSelection");
    if(type.contains("TQListBox:"))
    {
      MyListBox *ptr = (MyListBox *) widget;
      int index = 0;
      if     (ptr->selectionMode() == QAbstractItemView::MultiSelection) index = 1;
      else if(ptr->selectionMode() == QAbstractItemView::NoSelection)    index = 2;
      form->comboBoxMultiSection->setCurrentIndex(index);
    }
    else if(type.contains("TQListView:"))
    {
      MyListView *ptr = (MyListView *) widget;
      int index = 0;
      if     (ptr->selectionMode() == QAbstractItemView::MultiSelection)       index = 1;
      else if(ptr->selectionMode() == QAbstractItemView::NoSelection) index = 2;
      form->comboBoxMultiSection->setCurrentIndex(index);
    }
  }
  else
  {
    form->comboBoxMultiSection->hide();
  }

  QObject::connect(form->okButton,     SIGNAL(clicked()),SLOT(slotOk()));
  QObject::connect(form->cancelButton, SIGNAL(clicked()),SLOT(slotCancel()));
  QObject::connect(form->updateButton, SIGNAL(clicked()),SLOT(slotUpdate()));
}

dlgProperty::~dlgProperty()
{
}

int dlgProperty::run()
{
  exec();
  return ret;
}

void dlgProperty::slotOk()
{
  slotUpdate();
  accept();
  ret = 1;
}

void dlgProperty::slotCancel()
{
  reject();
  ret = 0;
}

void dlgProperty::slotUpdate()
{
  QString buf;
  modified = 1;
  QString type(widget->statusTip());
  widget->setObjectName(form->lineEditObjectName->text());
  buf = form->lineEditWhatsThis->text();
  widget->setWhatsThis(buf);
  buf = form->lineEditToolTip->text();
  widget->setToolTip(buf);
  mySetGeometry(widget,form->spinBoxX->value(),form->spinBoxY->value(),
                       form->spinBoxW->value(),form->spinBoxH->value());
  widget->setMinimumSize(form->spinBoxMinWidth->value(),form->spinBoxMinHeight->value());
  widget->setMaximumSize(form->spinBoxMaxWidth->value(),form->spinBoxMaxHeight->value());

  QFont font = widget->font();

  int italic = 0;
  int underline = 0;
  int strikeout = 0;
  int bold = 0;
  int userDefinedFont = 0;
  if(form->checkBoxFontItalic->checkState()    == Qt::Checked) {userDefinedFont = 1; italic = 1;}
  if(form->checkBoxFontUnderline->checkState() == Qt::Checked) {userDefinedFont = 1; underline = 1;}
  if(form->checkBoxFontStrikeout->checkState() == Qt::Checked) {userDefinedFont = 1; strikeout = 1;}
  if(form->checkBoxFontBold->checkState()      == Qt::Checked) {userDefinedFont = 1; bold = 1;}
  int size = form->spinBoxFontSize->value();
#ifdef PVUNIX
  if(size != 9) userDefinedFont = 1;
#endif
#ifdef PVWIN32
  if(size != 8) userDefinedFont = 1;
#endif
  QString fontfamily = form->comboBoxFontFamily->currentText();
#ifdef PVUNIX
  if(fontfamily == "default") fontfamily = "Sans Serif";
#endif
#ifdef PVWIN32
  if(fontfamily == "default") fontfamily = "MS Shell Dlg 2";
#endif
  else                        userDefinedFont = 1;
  QFont newfont(fontfamily,size);
  if(italic)    newfont.setItalic(true);
  if(underline) newfont.setUnderline(true);
  if(strikeout) newfont.setStrikeOut(true);
  if(bold)      newfont.setBold(true);
  widget->setFont(newfont);
  if(userDefinedFont == 1)
  {
    QString st = widget->statusTip();
    if(!st.contains("font:"))
    {
      st.append("font:");
      widget->setStatusTip(st);
    }
  }
  else
  {
    QString st = widget->statusTip();
    if(st.contains("font:"))
    {
      st.remove("font:");
      widget->setStatusTip(st);
    }
  }

  int fgR = form->spinBoxFgRed->value();
  int fgG = form->spinBoxFgGreen->value();
  int fgB = form->spinBoxFgBlue->value();
  int bgR = form->spinBoxBgRed->value();
  int bgG = form->spinBoxBgGreen->value();
  int bgB = form->spinBoxBgBlue->value();
  if(bgR==-1 || bgG==-1 || bgB==-1)
  {
    //widget->unsetPalette();
    QPalette palette;
    widget->setPalette(palette);
    QString st = widget->statusTip();
    st.remove("background:");
    widget->setStatusTip(st);
  }
  if(fgR==-1 || fgG==-1 || fgB==-1)
  {
    //widget->unsetPalette();
    QPalette palette;
    widget->setPalette(palette);
    QString st = widget->statusTip();
    st.remove("foreground:");
    widget->setStatusTip(st);
  }

  if(bgR != -1 && bgG != -1 && bgB != -1)
  {
    if(type.contains("TQPushButton:"))
    {
      mySetBackgroundColor(widget,TQPushButton,bgR,bgG,bgB);
    }
    else if(type.contains("TQRadio:"))
    {
      mySetBackgroundColor(widget,TQRadio,bgR,bgG,bgB);
    }
    else if(type.contains("TQCheck:"))
    {
      mySetBackgroundColor(widget,TQCheck,bgR,bgG,bgB);
    }
    else if(type.contains("TQMultiLineEdit:"))
    {
      mySetBackgroundColor(widget,TQMultiLineEdit,bgR,bgG,bgB);
    }
    else if(type.contains("TQLineEdit:"))
    {
      mySetBackgroundColor(widget,TQLineEdit,bgR,bgG,bgB);
    }
    else if(type.contains("TQTextBrowser:"))
    {
      mySetBackgroundColor(widget,TQTextBrowser,bgR,bgG,bgB);
    }
    else
    {
      mySetBackgroundColor(widget,-1,bgR,bgG,bgB);
    }
    if(!widget->statusTip().contains("background:"))
    {
      QString st = widget->statusTip();
      st.append("background:");
      widget->setStatusTip(st);
    }
  }

  if(fgR != -1 && fgG != -1 && fgB != -1)
  {
    if(type.contains("TQPushButton:"))
    {
      mySetForegroundColor(widget,TQPushButton,fgR,fgG,fgB);
    }
    else
    {
      mySetForegroundColor(widget,-1,fgR,fgG,fgB);
    }
    QString st = widget->statusTip();
    if(!st.contains("foreground:"))
    {
      st.append("foreground:");
      widget->setStatusTip(st);
    }
  }

  QString quote = form->lineEditText->text().replace("\\n","\n");
  if(type.contains("TQLabel:"))
  {
    ((MyLabel *)widget)->setText(quote);
  }
  else if(type.contains("TQLineEdit:"))
  {
    ((MyLineEdit *)widget)->setText(quote);
  }
  else if(type.contains("TQPushButton:"))
  {
    ((MyQPushButton *)widget)->setText(quote);
  }
  else if(type.contains("TQRadio:"))
  {
    ((MyRadioButton *)widget)->setText(quote);
  }
  else if(type.contains("TQCheck:"))
  {
    ((MyCheckBox *)widget)->setText(quote);
  }
  else if(type.contains("TQButtonGroup:"))
  {
    ((MyButtonGroup *)widget)->setTitle(quote);
  }
  else if(type.contains("TQGroupBox:"))
  {
    ((MyGroupBox *)widget)->setTitle(quote);
  }

  if(type.contains("TQLabel:"))
  {
    MyLabel *ptr = (MyLabel *) widget;
    QString txt;
    int shape  = QFrame::NoFrame;
    int shadow = QFrame::Plain;
    txt = form->comboBoxShape->currentText();
    if(txt == "Box")         shape = QFrame::Box;
    if(txt == "Panel")       shape = QFrame::Panel;
    if(txt == "WinPanel")    shape = QFrame::WinPanel;
    if(txt == "HLine")       shape = QFrame::HLine;
    if(txt == "VLine")       shape = QFrame::VLine;
    if(txt == "StyledPanel") shape = QFrame::StyledPanel;
    ptr->setFrameShape((QFrame::Shape) shape);
    txt = form->comboBoxShadow->currentText();
    if(txt == "Raised") shadow = QFrame::Raised;
    if(txt == "Sunken") shadow = QFrame::Sunken;
    ptr->setFrameShadow((QFrame::Shadow) shadow);
    ptr->setLineWidth(form->spinBoxLinewidth->value());
    ptr->setMargin(form->spinBoxMargin->value());
  }
  if(type.contains("TQFrame:"))
  {
    MyFrame *ptr = (MyFrame *) widget;
    QString txt;
    int shape  = QFrame::NoFrame;
    int shadow = QFrame::Plain;
    txt = form->comboBoxShape->currentText();
    if(txt == "Box")         shape = QFrame::Box;
    if(txt == "Panel")       shape = QFrame::Panel;
    if(txt == "WinPanel")    shape = QFrame::WinPanel;
    if(txt == "HLine")       shape = QFrame::HLine;
    if(txt == "VLine")       shape = QFrame::VLine;
    if(txt == "StyledPanel") shape = QFrame::StyledPanel;
    ptr->setFrameShape((QFrame::Shape) shape);
    txt = form->comboBoxShadow->currentText();
    if(txt == "Raised") shadow = QFrame::Raised;
    if(txt == "Sunken") shadow = QFrame::Sunken;
    ptr->setFrameShadow((QFrame::Shadow) shadow);
    ptr->setLineWidth(form->spinBoxLinewidth->value());
    //ptr->setMargin(form->spinBoxMargin->value());
  }

  if(type.contains("TQTable:"))
  {
    ((MyTable *)widget)->setRowCount(form->spinBoxNumRows->value());
    ((MyTable *)widget)->setColumnCount(form->spinBoxNumColumns->value());
  }

  if(type.contains("TQListBox:"))
  {
    QString txt;
    MyListBox *ptr = (MyListBox *) widget;
    txt = form->comboBoxMultiSection->currentText();
    if(txt == "SingleSelection") ptr->setSelectionMode(QAbstractItemView::SingleSelection);
    if(txt == "MultiSelection")  ptr->setSelectionMode(QAbstractItemView::MultiSelection);
    if(txt == "NoSelection")     ptr->setSelectionMode(QAbstractItemView::NoSelection);
  }

  if(type.contains("TQListView:"))
  {
    QString txt;
    MyListView *ptr = (MyListView *) widget;
    txt = form->comboBoxMultiSection->currentText();
    if(txt == "SingleSelection") ptr->setSelectionMode(QAbstractItemView::SingleSelection);
    if(txt == "MultiSelection")  ptr->setSelectionMode(QAbstractItemView::MultiSelection);
    if(txt == "NoSelection")     ptr->setSelectionMode(QAbstractItemView::NoSelection);
  }
}
