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
#ifndef DLGOPT_H
#define DLGOPT_H

#include <QtCore>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QDialog>

class DlgOpt : public QDialog
{
  Q_OBJECT

public:
  DlgOpt(QWidget *parent=0);
  ~DlgOpt();
  void setFilename(const char *filename);

protected:
  void initDialog();
  QTextEdit   *edit1;
  QPushButton *QPushButton_ok;
  QPushButton *QPushButton_cancel;
  QPushButton *QPushButton_reset_inifile;

private:
  QString filename;

private slots:
  void okClicked();
  void slotResetInifile();

private: 
};

#endif
