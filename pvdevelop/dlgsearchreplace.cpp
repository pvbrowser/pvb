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
#include "dlgsearchreplace.h"
#include <qmessagebox.h>
#include <qtextedit.h>
#include <QTextCursor>
#include <QClipboard>

dlgsearchreplace::dlgsearchreplace()
{
  uidlg = new Ui_DialogSearchReplace;
  uidlg->setupUi(this);
  editor = NULL;
  found = 0;
  connect(uidlg->replaceButton, SIGNAL(clicked()),this,SLOT(slotReplace()));
  connect(uidlg->findNextButton,SIGNAL(clicked()),this,SLOT(slotFindNext()));
/*
    QLabel *labelFind;
    QLabel *labelReplace;
    QLineEdit *lineEditFind;
    QLineEdit *lineEditReplace;
    QPushButton *replaceButton;
    QPushButton *findNextButton;
    QPushButton *closeButton;
*/
}

dlgsearchreplace::~dlgsearchreplace()
{
}

void dlgsearchreplace::runFind(Editor *_editor)
{
  editor = _editor;
  if(editor == NULL) return;
  uidlg->labelReplace->hide();
  uidlg->lineEditReplace->hide();
  uidlg->replaceButton->hide();
  exec();
}

void dlgsearchreplace::runReplace(Editor *_editor)
{
  editor = _editor;
  if(editor == NULL) return;
  uidlg->labelReplace->show();
  uidlg->lineEditReplace->show();
  uidlg->replaceButton->show();
  exec();
}

void dlgsearchreplace::slotFindNext()
{
  if(editor->edit->find(uidlg->lineEditFind->text())) found = 1;
  else                                                found = 0;

  if(found == 0)
  {
    int ret = QMessageBox::question(0,"pvdevelop: find","Goto top of file ?",QMessageBox::No,QMessageBox::Yes);
    if(ret == QMessageBox::Yes)
    {
      QTextCursor cursor;
      cursor.setPosition(0,QTextCursor::MoveAnchor);
      editor->edit->setTextCursor(cursor);
      editor->edit->ensureCursorVisible();
      slotFindNext();
      return;
    }
    else
    {
      QTextCursor cursor;
      cursor.setPosition(0,QTextCursor::MoveAnchor);
      editor->edit->setTextCursor(cursor);
      editor->edit->ensureCursorVisible();
      editor->edit->find(" ");
      return;
    }
  }
}

void dlgsearchreplace::slotReplace()
{
  if(found == 1)
  {
    editor->edit->cut();
    QApplication::clipboard()->setText(uidlg->lineEditReplace->text());
    editor->edit->paste();
  }
  slotFindNext();
}
