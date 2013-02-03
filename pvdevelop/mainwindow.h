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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "editor.h"
#include "designer.h"
#include "dlgsearchreplace.h"

class QAction;
class QMenu;
class QTextEdit;
class QScrollArea;
class QListWidgetItem;
class dlgTextBrowser;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void slotFileOpt();
  void newFile();
  void open();
  void perhapsFixCONFIG();
  bool save();
  bool saveAs();
  void slotBackup();
  void viewEditor();
  void viewDesigner();
  void slotQtDesigner();
  void slotExportUI();
  void slotImportUI();
  void slotInsertMask();
  void slotMake();
  void slotStartServer();
  void slotStartPvbrowser();
  void slotRllibUncommentRllib();
  void slotRllibUncommentModbus();
  void slotRllibUncommentSiemenstcp();
  void slotRllibUncommentPpi();
  void slotDaemonModbus();
  void slotDaemonSiemenstcp();
  void slotDaemonPpi();
  void slotLinuxWriteStartscript();
  void slotAboutManual();
  void about();
  void documentWasModified();
  void slotRadioProject(bool checked);
  void slotRadioMain(bool checked);
  void slotRadioHeader(bool checked);
  void slotRadioSlots(bool checked);
  void slotRadioScript(bool checked);
  void slotRadioMask(bool checked);
  void slotComboEvents(int i);
  void slotSpinBoxMask(int i);
  void slotInsertFunction();
  void slotWidgetname(QListWidgetItem *item);
  void slotSearch();
  void slotReplace();

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void connectActions();
  void disconnectActions();
  void readSettings();
  void writeSettings();
  bool maybeSave();
  void loadFile(const QString &fileName);
  bool saveFile(const QString &fileName);
  void setCurrentFile(const QString &fileName);
  QString strippedName(const QString &fullFileName);
  void load(const QString &fileName);
  void getWidgetNames(const QString &filename);
  void keyPressEvent(QKeyEvent * event);
  void keyReleaseEvent(QKeyEvent * event);

  //QTextEdit *textEdit;
  QString curFile;
  QString name;
  bool doChecked;
  int currentMask;

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *actionMenu;
  QMenu *rllibMenu;
  QMenu *daemonMenu;
  QMenu *linuxMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *editToolBar;
  QToolBar *viewToolBar;
  QAction *optAct;
  QAction *newAct;
  QAction *openAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *backupAct;
  QAction *exitAct;
  QAction *cutAct;
  QAction *copyAct;
  QAction *pasteAct;
  QAction *searchAct;
  QAction *replaceAct;
  QAction *viewEditorAct;
  QAction *viewDesignerAct;
  QAction *actionEditorAct;
  QAction *actionDesignerAct;
  QAction *actionQtDesignerAct;
  QAction *actionExportUIAct;
  QAction *actionImportUIAct;
  QAction *actionInsertMaskAct;
  QAction *actionMakeAct;
  QAction *actionStartServerAct;
  QAction *actionStartPvbrowserAct;
  QAction *rllibUncommentRllibAct;
  QAction *rllibUncommentModbusAct;
  QAction *rllibUncommentSiemenstcpAct;
  QAction *rllibUncommentPpiAct;
  QAction *daemonModbusAct;
  QAction *daemonSiemenstcpAct;
  QAction *daemonPpiAct;
  QAction *linuxWriteStartscriptAct;
  QAction *aboutManualAct;
  QAction *aboutAct;
  QAction *aboutQtAct;

  int  beginMenu();
  void endMenu();
  Editor   *editor;
  Designer *designer;
  QScrollArea *scroll;
  dlgTextBrowser *dlgtextbrowser;
  dlgsearchreplace searchreplace;
  int imask;
  QMap<QString, int> filePos;
};

int readProject();

#endif
