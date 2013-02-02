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
#include <QtGui>
#include "syntax.h"
#include "editor.h"
#include "opt.h"

extern OPT_DEVELOP opt_develop;

Editor::Editor()
{
  root = new QWidget();

  viewLayout = new QHBoxLayout(root); // this, 11, 6, "viewLayout");

  t = new QToolBox(root);
  t->setMaximumSize( QSize(170,32767) );
  t->setCurrentIndex(0);

  page1 = new QWidget(t);
  page1->setBackgroundRole(QPalette::Window);
  t->addItem(page1, QString::fromLatin1("Module"));

  buttonGroup1 = new QGroupBox("File",page1);
  buttonGroup1->setGeometry( QRect(0,0,170,170) );

  radioProject = new QRadioButton("Project",buttonGroup1);
  radioProject->setGeometry( QRect(10,20,130,25) );

  radioMain = new QRadioButton("Main",buttonGroup1);
  radioMain->setGeometry( QRect(10,50,130,25) );

  radioHeader = new QRadioButton("Header",buttonGroup1);
  radioHeader->setGeometry( QRect(10,80,130,25) );

  radioMask = new QRadioButton("Masks",buttonGroup1);
  radioMask->setGeometry( QRect(10,110,130,25) );
  radioMask->setChecked( false );

  radioSlots = new QRadioButton("Slots",buttonGroup1);
  radioSlots->setGeometry( QRect(10,140,70,25) ); //130

  radioScript = new QRadioButton("Script",buttonGroup1);
  radioScript->setGeometry( QRect(65,140,50,25) );

  textLabelLine = new QLabel("Line=0",buttonGroup1);
  textLabelLine->setGeometry( QRect(100,20,80,25) );

  textLabel1 = new QLabel("Masknr.",buttonGroup1);
  textLabel1->setGeometry( QRect(120,110,80,25) );

  spinBoxMask = new QSpinBox(buttonGroup1);
  spinBoxMask->setGeometry( QRect(120,140,45,25) );
  spinBoxMask->setMaximum(1000);
  spinBoxMask->setMinimum(1);

  buttonGroup1 = new QGroupBox("Slot Position",page1);
  buttonGroup1->setGeometry( QRect(0,180,170,60) );

  comboBoxEvent = new QComboBox(buttonGroup1);
  comboBoxEvent->setGeometry( QRect(10,25,150,25) );
  comboBoxEvent->insertItem(0,"//###");
  comboBoxEvent->insertItem(1,"typedef");
  comboBoxEvent->insertItem(2,"Init");
  comboBoxEvent->insertItem(3,"NullEvent");
  comboBoxEvent->insertItem(4,"ButtonEvent");
  comboBoxEvent->insertItem(5,"ButtonPressedEvent");
  comboBoxEvent->insertItem(6,"ButtonReleasedEvent");
  comboBoxEvent->insertItem(7,"TextEvent");
  comboBoxEvent->insertItem(8,"SliderEvent");
  comboBoxEvent->insertItem(9,"CheckboxEvent");
  comboBoxEvent->insertItem(10,"RadioButtonEvent");
  comboBoxEvent->insertItem(11,"GlInitializeEvent");
  comboBoxEvent->insertItem(12,"GlPaintEvent");
  comboBoxEvent->insertItem(13,"GlResizeEvent");
  comboBoxEvent->insertItem(14,"GlIdleEvent");
  comboBoxEvent->insertItem(15,"TabEvent");
  comboBoxEvent->insertItem(16,"TableTextEvent");
  comboBoxEvent->insertItem(17,"TableClickedEvent");
  comboBoxEvent->insertItem(18,"SelectionEvent");
  comboBoxEvent->insertItem(19,"ClipboardEvent");
  comboBoxEvent->insertItem(20,"RightMouseEvent");
  comboBoxEvent->insertItem(21,"KeyboardEvent");
  comboBoxEvent->insertItem(22,"MouseMovedEvent");
  comboBoxEvent->insertItem(23,"MousePressedEvent");
  comboBoxEvent->insertItem(24,"MouseReleasedEvent");
  comboBoxEvent->insertItem(25,"MouseOverEvent");
  comboBoxEvent->insertItem(26,"UserEvent");

  checkBoxSuServer = new QCheckBox("su --command=\"./pvs\"",page1);
  checkBoxSuServer->setGeometry( QRect(0,250,170,25) );
  checkBoxSuServer->setChecked(opt_develop.su);

  pushButtonInsertFunction = new QPushButton("Insert Function",page1);
  pushButtonInsertFunction->setGeometry( QRect(0,280,170,25) );

  page2 = new QWidget(t);
  //page1->setBackgroundRole(QPalette::Window);
  t->addItem( page2, QString::fromLatin1("Widget Names"));

  widgetname = new QListWidget(page2);
  widgetLayout = new QVBoxLayout(page2);
  widgetLayout->setSpacing(0);
  widgetLayout->setMargin(0);
  widgetLayout->addWidget(widgetname);

  edit = new QTextEdit(root);
  edit->setFont(QFont("Courier", 12));
  edit->setLineWrapMode( QTextEdit::NoWrap );
  highlighter = new syntax(edit);

  viewLayout->addWidget(t);
  viewLayout->addWidget(edit);

  connect(edit, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
}

Editor::~Editor()
{
  if(checkBoxSuServer->isChecked()) opt_develop.su = 1;
  else                              opt_develop.su = 0;
  delete highlighter;
  delete root;
}

int Editor::setSyntax(int language)
{
  highlighter->setSyntax(language);
  return 0;
}

void Editor::slotCursorPositionChanged()
{
  char text[80];
  int line;

  QTextCursor currentPosition = edit->textCursor();
  currentPosition.movePosition(QTextCursor::StartOfLine);

  QTextCursor cursor = QTextCursor(edit->document());
  cursor.movePosition(QTextCursor::StartOfLine);

  for(line=1; cursor < currentPosition; line++)
  {
    cursor.movePosition(QTextCursor::Down);
  }

  sprintf(text,"Line=%d",line);
  textLabelLine->setText(text);
}
