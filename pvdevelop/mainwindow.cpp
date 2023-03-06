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
#include <QFile>

#include "mainwindow.h"
#include "dlgopt.h"
#include "dlgnewprj.h"
#include "dlginsertfunction.h"
#include "dlgpastewidget.h"
#include "dlgtextbrowser.h"
#include "dlgdaemon.h"
#include "opt.h"
#include "cutil.h"
#include "widgetgenerator.h"
#include "syntax.h"

extern OPT_DEVELOP opt_develop;

static const char exportWarning[] = "You are not allowed to define the layout management within Qt Designer.\n"
                                    "You will have to define a possible layout management within pvdevelop.\n"
                                    "This is done within the graphical designer of pvdevelop.\n"
                                    "There right click the mouse and choose \"Edit layout\" from the context menu.\n"
                                    "\n"
                                    "If you define a layout management within Qt Designer the geometry of your widgets will get lost after importing again to pvdevelop!!!\n"
                                    "\n"
                                    "Do you want to continue ?\n";

MainWindow::MainWindow()
{
  if(opt_develop.arg_debug) printf("MainWindow start\n");

  imask = 1;
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  readSettings();

  editor   = NULL;
  designer = NULL;
  //viewDesigner();
  viewEditor();

  doChecked = true;
  currentMask = 1;
  setCurrentFile("");

  dlgtextbrowser = new dlgTextBrowser(opt_develop.manual);

  if(opt_develop.arg_project[0] != '\0')
  {
    QString test = opt_develop.arg_project;
    test += ".pro";
    FILE *fin = fopen(test.toUtf8(),"r");
    if(fin != NULL)
    {
      fclose(fin);
      name = opt_develop.arg_project;
      load(name + ".pro");
      editor->radioProject->setChecked(true);
      perhapsFixCONFIG();
    }
    else if(opt_develop.script == PV_LUA)
    {
      test = "main.lua";
      fin = fopen(test.toUtf8(),"r");
      if(fin != NULL)
      {
        fclose(fin);
        name = opt_develop.arg_project;
        load("main.lua");
        editor->radioMain->setChecked(true);
      }  
    }
  }

  if(opt_develop.arg_debug) printf("MainWindow end\n");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if(editor == NULL) viewEditor();
  if(maybeSave())
  {
    writeSettings();
    if(dlgtextbrowser != NULL) delete dlgtextbrowser;
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

void MainWindow::newFile()
{
  QString project;
  int ret;
  if (maybeSave())
  {
    editor->edit->clear();
    setCurrentFile("");
  }

  dlgnewprj dlg;
  ret = dlg.exec();
  name = dlg.uidlg->lineEditName->text();
  if(ret == QDialog::Accepted && !name.isEmpty())
  {
    name = name.simplified();
    name = name.replace(" ","_");
    name.truncate(sizeof(opt_develop.arg_project) - 2);
    strcpy(opt_develop.arg_project,(const char *) name.toUtf8());
    if(dlg.uidlg->lineEditDirectory->text() != "") QDir::setCurrent(dlg.uidlg->lineEditDirectory->text());
    int language = dlg.uidlg->comboBoxLanguage->currentIndex();
    if(opt_develop.arg_debug) printf("comboBoxLanguage->currentText=%s language=%d\n",
           (const char *) dlg.uidlg->comboBoxLanguage->currentText().toUtf8(),
           language);
    // language = 0 C/C++
    // language = 1 Lua
    // language = 2 Phyton (experimental)
    if     (language == 1) opt_develop.script = PV_LUA;
    else if(language == 2) opt_develop.script = PV_PYTHON;
    else                   opt_develop.script = 0; // C/C++
    if(opt_develop.script == PV_PYTHON)
    {
      name = "pvs";
      strcpy(opt_develop.arg_project, "pvs");
#ifdef PVWIN32
      system("pvb_copy_python_template.bat");
#else
      ret = system("cp /opt/pvb/language_bindings/python/template/* .");
      if(ret < 0) printf("ERROR system(cp /opt/pvb/language_bindings/python/template/* .)\n");
#endif
    }
    else
    {
      if(opt_develop.arg_debug) printf("calling generateInitialProject language=%d\n", language);      
      generateInitialProject(name.toUtf8());
    }
    if(opt_develop.arg_debug) printf("script=%d\n", opt_develop.script);
    ret = readProject();
    if(ret == -1) { name.clear(); return; }
    // add additional language here
    if     (opt_develop.script == PV_PYTHON) editor->radioScript->show();
    else if(opt_develop.script == PV_PERL)   editor->radioScript->show();
    else if(opt_develop.script == PV_PHP)    editor->radioScript->show();
    else if(opt_develop.script == PV_TCL)    editor->radioScript->show();
    else                             editor->radioScript->hide();
    if(opt_develop.script == PV_LUA)         editor->radioHeader->hide();
    else                             editor->radioHeader->show();
    if(opt_develop.script == PV_LUA)         editor->radioProject->hide();
    else                             editor->radioProject->show();
    setCurrentFile(name + ".pro");
    project = "qmake=" + name;
    action(project.toUtf8());
    viewDesigner();
  }
}

int readProject()
{
  FILE *fin;
  char line[1024],*cptr;
  QString project = opt_develop.arg_project;
  project += ".pvproject";
  fin = fopen(project.toUtf8(),"r");
  if(fin == NULL)
  {
    return -1;
  }

  strcpy(opt_develop.target, "pvs");
  opt_develop.xmax = 1280;
  opt_develop.ymax = 1024;
  opt_develop.script = 0;
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    if(strncmp(line,"target=",7) == 0)
    {
      cptr = strchr(line,'\n');
      if(cptr != NULL) *cptr = '\0';
      cptr = strchr(line,'=');
      if(cptr == NULL)
      {
        fclose(fin);
        return -1;
      }
      cptr++;
      strcpy(opt_develop.target,cptr);
    }
    else if(strncmp(line,"xmax=",5) == 0)
    {
      sscanf(line,"xmax=%d",&opt_develop.xmax);
    }
    else if(strncmp(line,"ymax=",5) == 0)
    {
      sscanf(line,"ymax=%d",&opt_develop.ymax);
    }
    else if(strncmp(line,"script=Python",13) == 0)
    {
      opt_develop.script = PV_PYTHON;
    }
    else if(strncmp(line,"script=Perl",11) == 0)
    {
      opt_develop.script = PV_PERL;
    }
    else if(strncmp(line,"script=PHP",10) == 0)
    {
      opt_develop.script = PV_PHP;
    }
    else if(strncmp(line,"script=Tcl",10) == 0)
    {
      opt_develop.script = PV_TCL;
    }
    else if(strncmp(line,"script=Lua",10) == 0)
    {
      opt_develop.script = PV_LUA;
    }
    // add additional language here
  }
  fclose(fin);
  if(opt_develop.arg_debug) printf("Project: target=%s xmax=%d ymax=%d script=%d\n", opt_develop.target, opt_develop.xmax, opt_develop.ymax, opt_develop.script);
  return 0;
}

void MainWindow::open()
{
  char *cptr;
  int ret;
  if(maybeSave())
  {
    QString fileName = QFileDialog::getOpenFileName(this,"Choose project","","*.pvproject");
    if(!fileName.isEmpty())
    {
      QDir dir(fileName);
      dir.cdUp();
      QString path = dir.path();
      if(opt_develop.arg_debug) printf("mainwindow().open().setCurrent(%s)\n",(const char *) path.toUtf8());
      QDir::setCurrent(path);
      if(fileName.length() < (int) sizeof(opt_develop.arg_project)) strcpy(opt_develop.arg_project, (const char *) fileName.toUtf8());
      cptr = strchr(opt_develop.arg_project, '.');
      if(cptr != NULL) *cptr = '\0';

      ret = readProject();
      if(ret == -1) { name.clear(); return; }
      // add additional language here
      if     (opt_develop.script == PV_PYTHON) editor->radioScript->show();
      else if(opt_develop.script == PV_PERL)   editor->radioScript->show();
      else if(opt_develop.script == PV_PHP)    editor->radioScript->show();
      else if(opt_develop.script == PV_TCL)    editor->radioScript->show();
      else                             editor->radioScript->hide();
      if(opt_develop.script == PV_LUA)         editor->radioHeader->hide();
      else                             editor->radioHeader->show();
      if(opt_develop.script == PV_LUA)         editor->radioProject->hide();
      else                             editor->radioProject->show();
      name = opt_develop.target;

#ifdef PVUNIX
      char cmd[1024];
      sprintf(cmd,"fake_qmake %s %s.pro -o Makefile", opt_develop.fake_qmake, (const char *) name.toUtf8());
      if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
      ret = system(cmd);
      if(ret < 0) printf("ERROR system(%s)\n", cmd);
#endif
      if(opt_develop.script == PV_LUA)
      {
        load("main.lua");
        if(editor != NULL) editor->radioMain->setChecked(true);
      }
      else
      {
        load(name + ".pro");
        if(editor != NULL) editor->radioProject->setChecked(true);
        perhapsFixCONFIG();
      }
    }
  }
}

void MainWindow::perhapsFixCONFIG()
{
  if(editor != NULL)
  {
    QTextCursor cursor = editor->edit->document()->find("CONFIG");
    if(cursor.isNull() == false)
    {
      cursor.clearSelection();
      cursor.movePosition(QTextCursor::NextWord);
      cursor.select(QTextCursor::WordUnderCursor);
      QString txt = cursor.selectedText();
      if(txt == "=")
      {
        cursor.insertText("+=");
      }
    }  
  }
}

bool MainWindow::save()
{
  if(designer != NULL)
  {
    char buf[80];
    if(opt_develop.script == PV_LUA)
    {
      sprintf(buf,"mask%d.lua",imask);
    }
    else
    {
      sprintf(buf,"mask%d.cpp",imask);
    }
    generateMask(buf, designer->root);
    // add additional language here
    if(opt_develop.script == PV_PYTHON)
    {
      generatePython(imask, designer->root);
    }
    if(opt_develop.script == PV_PHP)
    {
      generatePHP(imask, designer->root);
    }
    if(opt_develop.script == PV_PERL)
    {
      generatePerl(imask, designer->root);
    }
    if(opt_develop.script == PV_TCL)
    {
      generateTcl(imask, designer->root);
    }
    designer->root->modified = 0;
    return true;
  }

  if(curFile.isEmpty()) return false; // saveAs();
  else                  return saveFile(curFile);
}

bool MainWindow::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName(this);
  if(fileName.isEmpty()) return false;
  return saveFile(fileName);
}

void MainWindow::slotBackup()
{
  QString command;
  QString message = "Now running:\n";
  QString localname = name;
  localname.remove(".pro");
  //rlmurx-was-here command.sprintf("tar -zcf %s/%s.tar.gz .",opt_develop.backupLocation,(const char *) localname.toUtf8());
  command = QString::asprintf("tar -zcf %s/%s.tar.gz .",opt_develop.backupLocation,(const char *) localname.toUtf8());
  message.append(command);
  QMessageBox::information(this, tr("pvdevelop"),message);
  int ret = system(command.toUtf8());
  if(ret < 0) printf("ERROR system(%s)\n", (const char *) command.toUtf8());
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About pvdevelop"),
            tr("(C) 2000-2019 Lehrig Software Engineering\n"
               "develop pvserver's easily\n"
               "IDE for editing and designing pvserver and data acquisition\n"
               "Have a lot of fun: Your's pvbrowser community\n"
               "http://pvbrowser.org"));
}

void MainWindow::documentWasModified()
{
  setWindowModified(editor->edit->document()->isModified());
}

void MainWindow::createActions()
{
  if(opt_develop.arg_debug) printf("createActions\n");;

  optAct = new QAction(QIcon(":/images/option.png"), tr("O&ptions"), this);
  optAct->setStatusTip(tr("Edit options"));
  connect(optAct, SIGNAL(triggered()), this, SLOT(slotFileOpt()));

  newAct = new QAction(QIcon(":/images/new.png"), tr("&New pvserver"), this);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("Create a new visualization"));
  //connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  //connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
  saveAct->setShortcut(tr("Ctrl+S"));
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  //saveAsAct = new QAction(tr("Save &As..."), this);
  //saveAsAct->setStatusTip(tr("Save the document under a new name"));
  //connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

#ifdef PVUNIX
  backupAct = new QAction(QIcon(":/images/save.png"), tr("&Backup"), this);
  backupAct->setShortcut(tr("Ctrl+B"));
  backupAct->setStatusTip(tr("Create *.tar.gz file for backup purpose"));
  connect(backupAct, SIGNAL(triggered()), this, SLOT(slotBackup()));
#endif

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  //connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
  cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));

  copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
  copyAct->setShortcut(tr("Ctrl+C"));
  copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));

  pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
  pasteAct->setShortcut(tr("Ctrl+V"));
  pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));

  searchAct = new QAction(tr("&Find"), this);
  searchAct->setShortcut(tr("Ctrl+F"));
  searchAct->setStatusTip(tr("Find string in editor"));

  replaceAct = new QAction(tr("&Replace"), this);
  replaceAct->setShortcut(tr("Ctrl+R"));
  replaceAct->setStatusTip(tr("Replace strings in editor"));

  viewEditorAct = new QAction(QIcon(":/images/editor.png"), tr("&Editor"), this);
  //viewEditorAct->setShortcut(tr("Ctrl+E"));
  viewEditorAct->setStatusTip(tr("Show the Editor"));
  connect(viewEditorAct, SIGNAL(triggered()), this, SLOT(viewEditor()));

  viewDesignerAct = new QAction(QIcon(":/images/designer.png"), tr("&Designer"), this);
  //viewDesignerAct->setShortcut(tr("Ctrl+D"));
  viewDesignerAct->setStatusTip(tr("Show the Designer"));
  connect(viewDesignerAct, SIGNAL(triggered()), this, SLOT(viewDesigner()));

  actionEditorAct = new QAction(QIcon(":/images/editor.png"), tr("&Editor"), this);
  actionEditorAct->setShortcut(tr("Ctrl+E"));
  actionEditorAct->setStatusTip(tr("Show the Editor"));
  connect(actionEditorAct, SIGNAL(triggered()), this, SLOT(viewEditor()));

  actionDesignerAct = new QAction(QIcon(":/images/designer.png"), tr("&Designer"), this);
  actionDesignerAct->setShortcut(tr("Ctrl+D"));
  actionDesignerAct->setStatusTip(tr("Show the Designer"));
  connect(actionDesignerAct, SIGNAL(triggered()), this, SLOT(viewDesigner()));

  actionQtDesignerAct = new QAction(QIcon(":/images/importui.png"), tr("Design UI-&File with Qt Designer"), this);
  actionQtDesignerAct->setStatusTip(tr("qtdesigner UI-File"));

  actionExportUIAct = new QAction(QIcon(":/images/importui.png"), tr("Export UI-&File"), this);
  //actionExportUIAct->setShortcut(tr("Ctrl+U"));
  actionExportUIAct->setStatusTip(tr("Export UI-File"));
  //connect(actionExportUIAct, SIGNAL(triggered()), this, SLOT(slotExportUI()));

  actionImportUIAct = new QAction(QIcon(":/images/importui.png"), tr("Import &UI-File"), this);
  actionImportUIAct->setShortcut(tr("Ctrl+U"));
  actionImportUIAct->setStatusTip(tr("Import UI-File"));
  //connect(actionImportUIAct, SIGNAL(triggered()), this, SLOT(slotImportUI()));

  actionInsertMaskAct = new QAction(QIcon(":/images/insertmask.png"), tr("&Insert Mask"), this);
  actionInsertMaskAct->setShortcut(tr("Ctrl+I"));
  actionInsertMaskAct->setStatusTip(tr("Insert a new mask"));
  //connect(actionInsertMaskAct, SIGNAL(triggered()), this, SLOT(slotInsertMask()));

  actionMakeAct = new QAction(QIcon(":/images/make.png"), tr("&Make"), this);
  actionMakeAct->setShortcut(tr("Ctrl+M"));
  actionMakeAct->setStatusTip(tr("Make project"));
  //connect(actionMakeAct, SIGNAL(triggered()), this, SLOT(slotMake()));

  actionStartServerAct = new QAction(QIcon(":/images/startserver.png"), tr("&StartServer"), this);
  actionStartServerAct->setShortcut(tr("Ctrl+T"));
  actionStartServerAct->setStatusTip(tr("Start the Server"));
  //connect(actionStartServerAct, SIGNAL(triggered()), this, SLOT(slotStartServer()));

  actionStartPvbrowserAct = new QAction(QIcon(":/images/app.png"), tr("&pvbrowser"), this);
  actionStartPvbrowserAct->setShortcut(tr("Ctrl+P"));
  actionStartPvbrowserAct->setStatusTip(tr("Start pvbrowser"));
  //connect(actionStartPvbrowserAct, SIGNAL(triggered()), this, SLOT(slotStartPvbrowser()));

  rllibUncommentRllibAct = new QAction(tr("Uncomment &rllib"), this);
  rllibUncommentRllibAct->setStatusTip(tr("Uncomment rllib"));
  //connect(rllibUncommentRllibAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentRllib()));

  rllibUncommentModbusAct = new QAction(tr("Uncomment &modbus"), this);
  rllibUncommentModbusAct->setStatusTip(tr("Uncomment modbus"));
  //connect(rllibUncommentModbusAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentModbus()));

  rllibUncommentSiemenstcpAct = new QAction(tr("Uncomment &Siemens TCP"), this);
  rllibUncommentSiemenstcpAct->setStatusTip(tr("Uncomment Siemens TCP"));
  //connect(rllibUncommentSiemenstcpAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentSiemenstcp()));

  rllibUncommentPpiAct = new QAction(tr("Uncomment &ppi"), this);
  rllibUncommentPpiAct->setStatusTip(tr("Uncomment ppi"));
  //connect(rllibUncommentPpiAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentPpi()));

  daemonModbusAct = new QAction(tr("&Modbus"), this);
  daemonModbusAct->setStatusTip(tr("Modbus daemon"));
  //connect(daemonModbusAct, SIGNAL(triggered()), this, SLOT(slotDaemonModbus()));

  daemonSiemenstcpAct = new QAction(tr("&Siemens TCP"), this);
  daemonSiemenstcpAct->setStatusTip(tr("Siemens TCP daemon"));
  //connect(daemonSiemenstcpAct, SIGNAL(triggered()), this, SLOT(slotDaemonSiemenstcp()));

  daemonPpiAct = new QAction(tr("&PPI"), this);
  daemonPpiAct->setStatusTip(tr("PPI daemon"));
  //connect(daemonPpiAct, SIGNAL(triggered()), this, SLOT(slotDaemonPpi()));

  linuxWriteStartscriptAct = new QAction(tr("&Write Startscript"), this);
  linuxWriteStartscriptAct->setStatusTip(tr("Write Startscript for daemon"));
  //connect(linuxWriteStartscriptAct, SIGNAL(triggered()), this, SLOT(slotLinuxWriteStartscript()));

  aboutManualAct = new QAction(tr("&Manual"), this);
  aboutManualAct->setShortcut(tr("Ctrl+H"));
  aboutManualAct->setStatusTip(tr("View Manual"));
  connect(aboutManualAct, SIGNAL(triggered()), this, SLOT(slotAboutManual()));

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  cutAct->setEnabled(false);
  copyAct->setEnabled(false);
}

void MainWindow::connectActions()
{
  if(opt_develop.arg_debug) printf("connectActions begin1\n");
  if(editor == NULL) return;
  if(opt_develop.arg_debug) printf("connectActions begin2\n");
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
  connect(searchAct, SIGNAL(triggered()), this, SLOT(slotSearch()));
  connect(replaceAct, SIGNAL(triggered()), this, SLOT(slotReplace()));
  connect(actionQtDesignerAct, SIGNAL(triggered()), this, SLOT(slotQtDesigner()));
  connect(actionExportUIAct, SIGNAL(triggered()), this, SLOT(slotExportUI()));
  connect(actionImportUIAct, SIGNAL(triggered()), this, SLOT(slotImportUI()));
  connect(actionInsertMaskAct, SIGNAL(triggered()), this, SLOT(slotInsertMask()));
  connect(actionMakeAct, SIGNAL(triggered()), this, SLOT(slotMake()));
  connect(actionStartServerAct, SIGNAL(triggered()), this, SLOT(slotStartServer()));
  connect(actionStartPvbrowserAct, SIGNAL(triggered()), this, SLOT(slotStartPvbrowser()));
  connect(rllibUncommentRllibAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentRllib()));
  connect(rllibUncommentModbusAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentModbus()));
  connect(rllibUncommentSiemenstcpAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentSiemenstcp()));
  connect(rllibUncommentPpiAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentPpi()));
  connect(daemonModbusAct, SIGNAL(triggered()), this, SLOT(slotDaemonModbus()));
  connect(daemonSiemenstcpAct, SIGNAL(triggered()), this, SLOT(slotDaemonSiemenstcp()));
  connect(daemonPpiAct, SIGNAL(triggered()), this, SLOT(slotDaemonPpi()));
  connect(linuxWriteStartscriptAct, SIGNAL(triggered()), this, SLOT(slotLinuxWriteStartscript()));
  connect(cutAct, SIGNAL(triggered()), editor->edit, SLOT(cut()));
  connect(copyAct, SIGNAL(triggered()), editor->edit, SLOT(copy()));
  connect(pasteAct, SIGNAL(triggered()), editor->edit, SLOT(paste()));

  if(opt_develop.arg_debug) printf("connectActions 1\n");

  newAct->setEnabled(true);
  openAct->setEnabled(true);
  exitAct->setEnabled(true);
  searchAct->setEnabled(true);
  replaceAct->setEnabled(true);
  actionQtDesignerAct->setEnabled(true);
  actionExportUIAct->setEnabled(true);
  actionImportUIAct->setEnabled(true);
  actionInsertMaskAct->setEnabled(true);
  actionMakeAct->setEnabled(true);
  actionStartServerAct->setEnabled(true);
  actionStartPvbrowserAct->setEnabled(true);
  rllibUncommentRllibAct->setEnabled(true);
  rllibUncommentModbusAct->setEnabled(true);
  rllibUncommentSiemenstcpAct->setEnabled(true);
  rllibUncommentPpiAct->setEnabled(true);
  daemonModbusAct->setEnabled(true);
  daemonSiemenstcpAct->setEnabled(true);
  daemonPpiAct->setEnabled(true);
  linuxWriteStartscriptAct->setEnabled(true);
  cutAct->setEnabled(true);
  copyAct->setEnabled(true);
  pasteAct->setEnabled(true);

  if(opt_develop.script == PV_LUA)
  {
    actionMakeAct->setEnabled(false);
    rllibUncommentRllibAct->setEnabled(false);
  }
  //#################################################################

  if(opt_develop.arg_debug) printf("connectActions 2\n");

  connect(editor->edit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
  connect(editor->edit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));

  connect(editor->edit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

  connect(editor->radioProject, SIGNAL(toggled(bool)), this, SLOT(slotRadioProject(bool)));
  connect(editor->radioMain, SIGNAL(toggled(bool)), this, SLOT(slotRadioMain(bool)));
  connect(editor->radioHeader, SIGNAL(toggled(bool)), this, SLOT(slotRadioHeader(bool)));
  connect(editor->radioSlots, SIGNAL(toggled(bool)), this, SLOT(slotRadioSlots(bool)));
  if(opt_develop.arg_debug) printf("connectActions 3\n");
  connect(editor->radioScript, SIGNAL(toggled(bool)), this, SLOT(slotRadioScript(bool)));
  if(opt_develop.arg_debug) printf("connectActions 4\n");
  connect(editor->radioMask, SIGNAL(toggled(bool)), this, SLOT(slotRadioMask(bool)));

  connect(editor->spinBoxMask, SIGNAL(valueChanged(int)), this, SLOT(slotSpinBoxMask(int)));
  connect(editor->comboBoxEvent, SIGNAL(activated(int)), this, SLOT(slotComboEvents(int)));

  connect(editor->pushButtonInsertFunction, SIGNAL(clicked()), this, SLOT(slotInsertFunction()));

  connect(editor->widgetname, SIGNAL(itemClicked( QListWidgetItem*)), this, SLOT(slotWidgetname(QListWidgetItem *)));
  if(opt_develop.arg_debug) printf("connectActions end\n");
}

void MainWindow::disconnectActions()
{
  if(opt_develop.arg_debug) printf("disconnectActions begin\n");;
  disconnect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
  disconnect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  disconnect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
  disconnect(searchAct, SIGNAL(triggered()), this, SLOT(slotSearch()));
  disconnect(replaceAct, SIGNAL(triggered()), this, SLOT(slotReplace()));
  disconnect(actionQtDesignerAct, SIGNAL(triggered()), this, SLOT(slotQtDesigner()));
  disconnect(actionExportUIAct, SIGNAL(triggered()), this, SLOT(slotExportUI()));
  disconnect(actionImportUIAct, SIGNAL(triggered()), this, SLOT(slotImportUI()));
  disconnect(actionInsertMaskAct, SIGNAL(triggered()), this, SLOT(slotInsertMask()));
  disconnect(actionMakeAct, SIGNAL(triggered()), this, SLOT(slotMake()));
  disconnect(actionStartServerAct, SIGNAL(triggered()), this, SLOT(slotStartServer()));
  disconnect(actionStartPvbrowserAct, SIGNAL(triggered()), this, SLOT(slotStartPvbrowser()));
  disconnect(rllibUncommentRllibAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentRllib()));
  disconnect(rllibUncommentModbusAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentModbus()));
  disconnect(rllibUncommentSiemenstcpAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentSiemenstcp()));
  disconnect(rllibUncommentPpiAct, SIGNAL(triggered()), this, SLOT(slotRllibUncommentPpi()));
  disconnect(daemonModbusAct, SIGNAL(triggered()), this, SLOT(slotDaemonModbus()));
  disconnect(daemonSiemenstcpAct, SIGNAL(triggered()), this, SLOT(slotDaemonSiemenstcp()));
  disconnect(daemonPpiAct, SIGNAL(triggered()), this, SLOT(slotDaemonPpi()));
  disconnect(linuxWriteStartscriptAct, SIGNAL(triggered()), this, SLOT(slotLinuxWriteStartscript()));
  disconnect(cutAct, SIGNAL(triggered()), editor->edit, SLOT(cut()));
  disconnect(copyAct, SIGNAL(triggered()), editor->edit, SLOT(copy()));
  disconnect(pasteAct, SIGNAL(triggered()), editor->edit, SLOT(paste()));

  newAct->setEnabled(false);
  openAct->setEnabled(false);
  exitAct->setEnabled(false);
  searchAct->setEnabled(false);
  replaceAct->setEnabled(false);
  actionQtDesignerAct->setEnabled(false);
  actionExportUIAct->setEnabled(false);
  actionImportUIAct->setEnabled(false);
  actionInsertMaskAct->setEnabled(false);
  actionMakeAct->setEnabled(false);
  actionStartServerAct->setEnabled(false);
  actionStartPvbrowserAct->setEnabled(false);
  rllibUncommentRllibAct->setEnabled(false);
  rllibUncommentModbusAct->setEnabled(false);
  rllibUncommentSiemenstcpAct->setEnabled(false);
  rllibUncommentPpiAct->setEnabled(false);
  daemonModbusAct->setEnabled(false);
  daemonSiemenstcpAct->setEnabled(false);
  daemonPpiAct->setEnabled(false);
  linuxWriteStartscriptAct->setEnabled(false);
  cutAct->setEnabled(false);
  copyAct->setEnabled(false);
  pasteAct->setEnabled(false);

  //#####################################################################

  disconnect(editor->edit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
  disconnect(editor->edit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));

  disconnect(editor->edit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

  disconnect(editor->radioProject, SIGNAL(toggled(bool)), this, SLOT(slotRadioProject(bool)));
  disconnect(editor->radioMain, SIGNAL(toggled(bool)), this, SLOT(slotRadioMain(bool)));
  disconnect(editor->radioHeader, SIGNAL(toggled(bool)), this, SLOT(slotRadioHeader(bool)));
  disconnect(editor->radioSlots, SIGNAL(toggled(bool)), this, SLOT(slotRadioSlots(bool)));
  if(opt_develop.arg_debug) printf("disconnectActions 1\n");
  disconnect(editor->radioScript, SIGNAL(toggled(bool)), this, SLOT(slotRadioScript(bool)));
  if(opt_develop.arg_debug) printf("disconnectActions 2\n");
  disconnect(editor->radioMask, SIGNAL(toggled(bool)), this, SLOT(slotRadioMask(bool)));

  disconnect(editor->spinBoxMask, SIGNAL(valueChanged(int)), this, SLOT(slotSpinBoxMask(int)));
  disconnect(editor->comboBoxEvent, SIGNAL(activated(int)), this, SLOT(slotComboEvents(int)));

  disconnect(editor->pushButtonInsertFunction, SIGNAL(clicked()), this, SLOT(slotInsertFunction()));

  disconnect(editor->widgetname, SIGNAL(itemClicked( QListWidgetItem*)), this, SLOT(slotWidgetname(QListWidgetItem *)));
  if(opt_develop.arg_debug) printf("disconnectActions end\n");;
}

void MainWindow::createMenus()
{
  if(opt_develop.arg_debug) printf("createMenus\n");;

  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(optAct);
  fileMenu->addSeparator();
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  //fileMenu->addAction(saveAsAct);
#ifdef PVUNIX
  fileMenu->addAction(backupAct);
#endif
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
  editMenu->addSeparator();
  editMenu->addAction(searchAct);
  editMenu->addAction(replaceAct);

  actionMenu = menuBar()->addMenu(tr("&Action"));
  actionMenu->addAction(actionEditorAct);
  actionMenu->addAction(actionDesignerAct);
//#ifndef PVWIN32  
  actionMenu->addAction(actionQtDesignerAct);
//#endif  
  actionMenu->addAction(actionExportUIAct);
  actionMenu->addAction(actionImportUIAct);
  actionMenu->addAction(actionInsertMaskAct);
  actionMenu->addAction(actionMakeAct);
  actionMenu->addAction(actionStartServerAct);
  actionMenu->addAction(actionStartPvbrowserAct);

  rllibMenu = menuBar()->addMenu(tr("&RLlib"));
  rllibMenu->addAction(rllibUncommentRllibAct);
  rllibMenu->addAction(rllibUncommentModbusAct);
  rllibMenu->addAction(rllibUncommentSiemenstcpAct);
  rllibMenu->addAction(rllibUncommentPpiAct);

  daemonMenu = menuBar()->addMenu(tr("&Daemon"));
  daemonMenu->addAction(daemonModbusAct);
  daemonMenu->addAction(daemonSiemenstcpAct);
  daemonMenu->addAction(daemonPpiAct);

#ifdef PVUNIX
  linuxMenu = menuBar()->addMenu(tr("&Linux"));
  linuxMenu->addAction(linuxWriteStartscriptAct);
#endif

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutManualAct);
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
  if(opt_develop.arg_debug) printf("createToolBars\n");;

  fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  editToolBar = addToolBar(tr("Edit"));
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);

  viewToolBar = addToolBar(tr("View"));
  viewToolBar->addAction(viewEditorAct);
  viewToolBar->addAction(viewDesignerAct);
}

void MainWindow::createStatusBar()
{
  if(opt_develop.arg_debug) printf("createStatusBar\n");;

  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
  if(opt_develop.arg_debug) printf("readSettings\n");;
  QSettings settings("Trolltech", "Application Example");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
}

void MainWindow::writeSettings()
{
  if(opt_develop.arg_debug) printf("writeSettings\n");;
  QSettings settings("Trolltech", "Application Example");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
  if(editor->edit->document()->isModified())
  {
    int ret = QMessageBox::warning(this, tr("pvdevelop"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
    if(ret == QMessageBox::Yes)          return save();
    else if (ret == QMessageBox::Cancel) return false;
  }
  return true;
}

void MainWindow::loadFile(const QString &fileName)
{
  QFile file(fileName);
  if(!file.open(QFile::ReadOnly | QFile::Text))
  {
    QMessageBox::warning(this, tr("pvdevelop"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
    return;
  }

  QTextStream in(&file);
  QApplication::restoreOverrideCursor();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  editor->setSyntax(CPP_SYNTAX);
  // add additional language here
  if(fileName.contains(".py"))  editor->setSyntax(PYTHON_SYNTAX);
  if(fileName.contains(".pm"))  editor->setSyntax(PERL_SYNTAX);
  if(fileName.contains(".php")) editor->setSyntax(PHP_SYNTAX);
  if(fileName.contains(".tcl")) editor->setSyntax(TCL_SYNTAX);
  if(fileName.contains(".lua")) editor->setSyntax(LUA_SYNTAX);

  editor->edit->setPlainText(in.readAll());
  QApplication::restoreOverrideCursor();

  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
  FILE *fout = fopen(fileName.toUtf8(),"w");
  if(fout == NULL)
  {
    QMessageBox::warning(this, tr("pvdevelop"),
                             tr("Cannot write file %1")
                             .arg(fileName)
                             );
    return false;
  }

  QApplication::restoreOverrideCursor();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  fprintf(fout,"%s",editor->edit->toPlainText().toUtf8().data());
  QApplication::restoreOverrideCursor();
  fclose(fout);

  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File saved"), 2000);
  return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
  if(editor == NULL) return;
  if(opt_develop.arg_debug) printf("setCurrentFile=%s\n", (const char *) fileName.toUtf8());

  curFile = fileName;
  editor->edit->document()->setModified(false);
  setWindowModified(false);

  QString shownName;
  if (curFile.isEmpty()) shownName = "no project loaded";
  else                   shownName = strippedName(curFile);

  setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("pvdevelop")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

void MainWindow::viewEditor()
{
  if(editor != NULL) return;
  if(opt_develop.arg_debug) printf("viewEditor begin\n");;

  if(editor != NULL || designer != NULL) centralWidget()->hide();
  if(designer != NULL)
  {
    designer->root->releaseMouse();
    designer->root->releaseKeyboard();
    delete designer;
  }
  designer = NULL;
  editor = new Editor();
  hide();
  //setCentralWidget(editor->edit);
  setCentralWidget(editor->root);
  show();
  centralWidget()->show();
  connectActions();

  if(!name.isEmpty())
  {
    if(opt_develop.script == PV_LUA)
    {
      load("main.lua");
      editor->radioMain->setChecked(true);
    }
    else
    {
      load(name + ".pro");
      editor->radioProject->setChecked(true);
      perhapsFixCONFIG();
    }
    editor->spinBoxMask->setValue(currentMask);
  }

  if(opt_develop.script == 0) editor->radioScript->hide();
  if(opt_develop.arg_debug) printf("viewEditor end\n");;
}

void MainWindow::viewDesigner()
{
  char buf[80];
  QString shownName;

  if(designer != NULL) return;
  if(beginMenu() == 0)
  {
    viewEditor();
    return;
  }
  if(opt_develop.arg_debug) printf("viewDesigner begin\n");;

  imask = 1;
  if(editor != NULL) imask = editor->spinBoxMask->value();
  if(editor != NULL || designer != NULL) centralWidget()->hide();
  disconnectActions();
  if(editor != NULL) delete editor;
  editor = NULL;
  scroll = new QScrollArea;
  sprintf(buf,"mask%d.cpp",imask);
  if(opt_develop.arg_debug) printf("viewDesigner before new Designer buf=%s\n", buf);
  designer = new Designer(buf);
  if(opt_develop.arg_debug) printf("viewDesigner after new Designer\n");
  designer->root->mainWindow = this;
  designer->root->resize(opt_develop.xmax,opt_develop.ymax);
  scroll->setMaximumSize(opt_develop.xmax,opt_develop.ymax);
  scroll->setWidget(designer->root);
  designer->root->setScroll(scroll);
  designer->root->statusBar = statusBar();
  hide();
  setCentralWidget(scroll);
  if(opt_develop.arg_debug) printf("viewDesigner before show\n");;
  show();
  if(opt_develop.arg_debug) printf("viewDesigner before centralWidget->show\n");;
  centralWidget()->show();
  if(opt_develop.arg_debug) printf("viewDesigner after centralWidget->show\n");;
  if (curFile.isEmpty()) shownName = "no project loaded";
  //rlmurx-was-here else                   shownName.sprintf("design mask%d   ClickRightMouse->PopupDialog   Alt+Click->PropertyDialog   Shift+Click->InsertDialog   Ctrl+Click->InsertLastSelectedWidget   Ctrl-Z->UndoLastMove   R->ReleaseMouse",imask);
  else                   shownName = QString::asprintf("design mask%d   ClickRightMouse->PopupDialog   Alt+Click->PropertyDialog   Shift+Click->InsertDialog   Ctrl+Click->InsertLastSelectedWidget   Ctrl-Z->UndoLastMove   R->ReleaseMouse",imask);
  //setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("pvdevelop")));
  setWindowTitle(tr("%1[*]").arg(shownName));

  drawDrawWidgets(designer->root);
  if(opt_develop.arg_debug) printf("viewDesigner end\n");;
}

void MainWindow::slotSearch()
{
  searchreplace.runFind(editor);
}

void MainWindow::slotReplace()
{
  searchreplace.runReplace(editor);
}

void MainWindow::slotMake()
{
  char act[1024];

  sprintf(act,"make=%s", (const char *) name.toUtf8());
  if(maybeSave()) action(act);
}

void MainWindow::slotQtDesigner()
{
  int ret;
  char cmd[1024];
  if(opt_develop.arg_debug) printf("qtdesigner ui %s imask=%d\n", (const char *) name.toUtf8(), imask);

  if(name.isEmpty())
  {
    QMessageBox::information(this,"pvdevelop","No project loaded",QMessageBox::Ok);
    return;
  }
  
  ret = QMessageBox::warning(this, tr("pvdevelop"),
                     tr(exportWarning),
                     QMessageBox::Yes ,
                     QMessageBox::No  | QMessageBox::Default);
  if(ret == QMessageBox::No) return;

  if(opt_develop.script == PV_LUA)
  {
    load("main.lua");
    editor->radioMain->setChecked(true);
  }
  else
  {
    load(name + ".pro");
    editor->radioProject->setChecked(true);
    perhapsFixCONFIG();
  }

  int mymask = editor->spinBoxMask->value();
  hide();
#ifdef PVWIN32  
  sprintf(cmd,"wait pvdevelop -action=designerUi:%d %s", mymask, (const char *) name.toUtf8());
  if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
  ret = mysystem(cmd);
  if(ret < 0) printf("ERROR system(%s)\n", cmd);
#else
  sprintf(cmd,"pvdevelop -action=designerUi:%d %s", mymask,  (const char *) name.toUtf8());
  if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
  system(cmd);
#endif
  show();
}

void MainWindow::slotExportUI()
{
  char file[80];
  FILE *fin;
  
  imask = 1;
  if(editor != NULL) imask = editor->spinBoxMask->value();
  if(opt_develop.arg_debug) printf("export ui %s imask=%d\n", (const char *) name.toUtf8(), imask);
 
  if(name.isEmpty())
  {
    QMessageBox::information(this,"pvdevelop","No project loaded",QMessageBox::Ok);
    return;
  }

  sprintf(file, "mask%d.ui", imask);
  fin = fopen(file, "r");
  if(fin != NULL)
  {
    fclose(fin);
    int ret = QMessageBox::warning(this, tr("pvdevelop"),
                     tr("ui-File already exists.\n"
                        "Do you want to overwrite it ?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No);
    if(ret == QMessageBox::No) return;
  }
  int ret = QMessageBox::warning(this, tr("pvdevelop"),
                     tr(exportWarning),
                     QMessageBox::Yes ,
                     QMessageBox::No  | QMessageBox::Default);
  if(ret == QMessageBox::No) return;
  export_ui(imask);
}

void MainWindow::slotImportUI()
{
  char cmd[1024];
  if(opt_develop.arg_debug) printf("import ui %s imask=%d\n", (const char *) name.toUtf8(), imask);

  if(name.isEmpty())
  {
    QMessageBox::information(this,"pvdevelop","No project loaded",QMessageBox::Ok);
    return;
  }

  if(opt_develop.script == PV_LUA)
  {
    load("main.lua");
    editor->radioMain->setChecked(true);
  }
  else
  {
    load(name + ".pro");
    editor->radioProject->setChecked(true);
    perhapsFixCONFIG();
  }

  int mymask = editor->spinBoxMask->value();
#ifdef PVUNIX
  sprintf(cmd,"xterm -e \"pvdevelop -action=importUi:%d %s && echo hit return && read\"", mymask,  (const char *) name.toUtf8());
  if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
  mysystem(cmd);
#else
  sprintf(cmd,"start pvb_import_ui.bat %s %d", (const char *) name.toUtf8(), mymask);
  if(opt_develop.arg_debug) printf("cmd=%s\n",cmd);
  int ret = system(cmd);
  if(ret < 0) printf("ERROR system(%s)\n", cmd);
#endif
}

void MainWindow::slotInsertMask()
{
  char act[1024];

  if(strlen(name.toUtf8()) > 1024-80) return;
  if(designer != NULL) viewEditor();
  sprintf(act,"insertMask=%s", (const char *) name.toUtf8());
  imask = action(act);
  slotSpinBoxMask(imask);
  editor->spinBoxMask->setValue(imask);
  viewDesigner();
}

void MainWindow::slotStartServer()
{
  int checked = 0;
  char command[1024];

  if(editor == NULL) viewEditor();
  if(editor->checkBoxSuServer->checkState() == Qt::Checked) checked = 1;
  sprintf(command,"startserver=%d:%s",checked, (const char *) name.toUtf8());
  if(maybeSave()) action(command);
}

void MainWindow::slotStartPvbrowser()
{
  action("pvbrowser");
}

void MainWindow::load( const QString &fileName )
{
  int ret;

  if(opt_develop.script == PV_LUA) editor->setSyntax(LUA_SYNTAX);
  if(name.isEmpty())
  {
    QMessageBox::information(this,"pvdevelop","No project loaded",QMessageBox::Ok);
    return;
  }
  if(maybeSave() == false)
  {
    doChecked = false;
    if(curFile.startsWith(name + ".pro"))   editor->radioProject->setChecked(true);
    else if(curFile.startsWith("main.cpp")) editor->radioMain->setChecked(true);
    else if(curFile.startsWith("pvapp.h"))  editor->radioHeader->setChecked(true);
    else if(curFile.contains("_slots.h"))   editor->radioSlots->setChecked(true);
    else if(curFile.contains(".py"))        editor->radioScript->setChecked(true);
    else if(curFile.contains("main.lua"))   editor->radioMain->setChecked(true);
    else if(curFile.contains("_slots.lua")) editor->radioSlots->setChecked(true);
    else if(curFile.startsWith("mask"))     editor->radioMask->setChecked(true);
    doChecked = true;
    return;
  }
  if(opt_develop.arg_debug) printf("load(%s)\n",(const char *) fileName.toUtf8());
  if(curFile != fileName) filePos[curFile] = editor->edit->textCursor().position(); // remember file pos
  if(opt_develop.arg_debug) printf("curFile=%s pos=%d\n", (const char *) curFile.toUtf8(), filePos[curFile]);
  QFile f(fileName);
  if(!f.open(QIODevice::ReadOnly))
  {
    if(fileName.contains("_slots") && opt_develop.script != PV_LUA)
    {
      editor->spinBoxMask->setValue(1);
      f.setFileName("mask1_slots.h");
      setCurrentFile("mask1_slots.h");
      if(!f.open(QIODevice::ReadOnly)) return;
    }
    else if(fileName.contains("_slots") && opt_develop.script == PV_LUA)
    {
      editor->spinBoxMask->setValue(1);
      f.setFileName("mask1_slots.lua");
      setCurrentFile("mask1_slots.lua");
      if(!f.open(QIODevice::ReadOnly)) return;
    }
    else if(fileName.contains(".py"))
    {
      editor->spinBoxMask->setValue(1);
      f.setFileName("mask1.py");
      setCurrentFile("mask1.py");
      if(!f.open(QIODevice::ReadOnly)) return;
    }
    else if(fileName.startsWith("mask") && opt_develop.script != PV_LUA)
    {
      editor->spinBoxMask->setValue(1);
      f.setFileName("mask1.cpp");
      setCurrentFile("mask1.cpp");
      if(!f.open(QIODevice::ReadOnly)) return;
    }
    else if(fileName.startsWith("mask") && opt_develop.script == PV_LUA)
    {
      editor->spinBoxMask->setValue(1);
      f.setFileName("mask1.lua");
      setCurrentFile("mask1.lua");
      if(!f.open(QIODevice::ReadOnly)) return;
    }
    else
    {
      return;
    }
  }
  QTextStream ts( &f );
  ts.setCodec("UTF-8");

  editor->setSyntax(CPP_SYNTAX);
  if(fileName.contains(".py"))  editor->setSyntax(PYTHON_SYNTAX);
  if(fileName.contains(".lua")) editor->setSyntax(LUA_SYNTAX);

  editor->edit->setPlainText( ts.readAll() );
  editor->edit->document()->setModified( false );
  setCurrentFile(fileName);
  f.close();
  getWidgetNames(fileName);
  if(fileName.startsWith("mask") && fileName.contains("_slots"))
  {
    slotComboEvents(editor->comboBoxEvent->currentIndex());
  }

  ret = readProject();
  if(ret == -1) { name.clear(); return; }
  // add additional language here
  if     (opt_develop.script == PV_PYTHON) editor->radioScript->show();
  else if(opt_develop.script == PV_PERL)   editor->radioScript->show();
  else if(opt_develop.script == PV_PHP)    editor->radioScript->show();
  else if(opt_develop.script == PV_TCL)    editor->radioScript->show();
  else                             editor->radioScript->hide();
  if(opt_develop.script == PV_LUA)         editor->radioHeader->hide();
  else                             editor->radioHeader->show();
  if(opt_develop.script == PV_LUA)         editor->radioProject->hide();
  else                             editor->radioProject->show();
  statusBar()->showMessage( tr("Loaded document %1").arg(fileName), 2000 );

  int file_pos = filePos[fileName]; // position cursor
  if(opt_develop.arg_debug) printf("set to filePos=%d\n", file_pos);
  QTextCursor cursor(editor->edit->textCursor());
  cursor.setPosition(file_pos);
  editor->edit->setTextCursor(cursor);
  editor->edit->ensureCursorVisible();
}

void MainWindow::slotRadioProject(bool checked)
{
  if(checked & doChecked) load(name + ".pro");
}

void MainWindow::slotRadioMain(bool checked)
{
  if     (checked & doChecked && opt_develop.script==PV_LUA) load("main.lua");
  else if(checked & doChecked                      ) load("main.cpp");
}

void MainWindow::slotRadioHeader(bool checked)
{
  if(checked & doChecked) load("pvapp.h");
}

void MainWindow::slotRadioSlots(bool checked)
{
  if(checked & doChecked)
  {
    QString fname;
    if(opt_develop.script == PV_LUA)
    {
      //rlmurx-was-here fname.sprintf("mask%d_slots.lua",editor->spinBoxMask->value());
      fname = QString::asprintf("mask%d_slots.lua",editor->spinBoxMask->value());
    }
    else
    {
      //rlmurx-was-here fname.sprintf("mask%d_slots.h",editor->spinBoxMask->value());
      fname = QString::asprintf("mask%d_slots.h",editor->spinBoxMask->value());
    }  
    load(fname.toUtf8());
  }
}

void MainWindow::slotRadioScript(bool checked)
{
  if(checked & doChecked)
  {
    QString fname;
    //rlmurx-was-here fname.sprintf("mask%d.py",editor->spinBoxMask->value());
    fname = QString::asprintf("mask%d.py",editor->spinBoxMask->value());
    load(fname.toUtf8());
  }
}

void MainWindow::slotRadioMask(bool checked)
{
  if(checked & doChecked)
  {
    QString fname;
    if(opt_develop.script == PV_LUA) 
    {
      //rlmurx-was-here fname.sprintf("mask%d.lua",editor->spinBoxMask->value());
      fname = QString::asprintf("mask%d.lua",editor->spinBoxMask->value());
    }
    else
    {
      //rlmurx-was-here fname.sprintf("mask%d.cpp",editor->spinBoxMask->value());
      fname = QString::asprintf("mask%d.cpp",editor->spinBoxMask->value());
    }
    load(fname.toUtf8());
  }
}

void MainWindow::slotComboEvents(int i)
{
  if((curFile.contains("_slots.h") || curFile.contains("_slots.lua") || curFile.contains(".py")) && editor != NULL)
  {
    QString what = editor->comboBoxEvent->itemText(i);
    if(what.contains("//###"))
    {
    }
    else if(what.contains("typedef"))
    {
    }
    else
    {
      what = "slot" + what;
    }

    QPoint pos; // scroll to top of document
    QTextCursor cursor = editor->edit->cursorForPosition(pos);
    editor->edit->setTextCursor(cursor);
    editor->edit->ensureCursorVisible();

    if(editor->edit->find(what))
    {
      editor->edit->ensureCursorVisible();
    }
    else if(editor->edit->find(what,QTextDocument::FindBackward))
    {
      editor->edit->ensureCursorVisible();
    }
  }
}

void MainWindow::slotSpinBoxMask(int i)
{
  char myname[1024];
  FILE *fin;

  if(name.isEmpty())
  {
    editor->spinBoxMask->setValue(1);
    QMessageBox::information(this,"pvdevelop","No project loaded",QMessageBox::Ok);
    return;
  }

  if(opt_develop.script == PV_LUA) sprintf(myname,"mask%d.lua",i); // test if mask exists
  else                     sprintf(myname,"mask%d.cpp",i); // test if mask exists
  fin = fopen(myname,"r");
  if(fin == NULL)
  {
    editor->spinBoxMask->setValue(1);
    currentMask = 1;
    return;
  }
  fclose(fin);

  if(curFile.contains(".py")) // load file
  {
    sprintf(myname,"mask%d.py",i);
    load(myname);
  }
  else if(curFile.contains("_slots.lua"))
  {
    sprintf(myname,"mask%d_slots.lua",i);
    load(myname);
  }
  else if(curFile.contains("_slots.h"))
  {
    sprintf(myname,"mask%d_slots.h",i);
    load(myname);
  }
  else if(curFile.contains("mask") && opt_develop.script == PV_LUA)
  {
    sprintf(myname,"mask%d.lua",i);
    load(myname);
  }
  else if(curFile.contains("mask"))
  {
    sprintf(myname,"mask%d.cpp",i);
    load(myname);
  }
  currentMask = i;
}

void MainWindow::slotRllibUncommentRllib()
{
  if(opt_develop.arg_debug) printf("slotRllibUncommentRllib()\n");
  if(beginMenu())
  {
    QString project = name + ".pro";
    uncommentRllib(project.toUtf8());
    endMenu();
  }
}

void MainWindow::slotRllibUncommentModbus()
{
  if(opt_develop.arg_debug) printf("slotRllibUncommentModbus()\n");
  if(beginMenu())
  {
    uncommentModbus();
    endMenu();
  }
}

void MainWindow::slotRllibUncommentSiemenstcp()
{
  if(opt_develop.arg_debug) printf("slotRllibUncommentSiemenstcp()\n");
  if(beginMenu())
  {
    uncommentSiemenstcp();
    endMenu();
  }
}

void MainWindow::slotRllibUncommentPpi()
{
  if(opt_develop.arg_debug) printf("slotRllibUncommentPpi()\n");
  if(beginMenu())
  {
    uncommentPpi();
    endMenu();
  }
}

void MainWindow::slotDaemonModbus()
{
  if(opt_develop.arg_debug) printf("slotDaemonModbus()\n");
  if(beginMenu())
  {
    dlgDaemon dlg(MODBUS_DAEMON);
    dlg.run();
  }
}

void MainWindow::slotDaemonSiemenstcp()
{
  if(opt_develop.arg_debug) printf("slotDaemonSiemenstcp()\n");
  if(beginMenu())
  {
    dlgDaemon dlg(SIEMENSTCP_DAEMON);
    dlg.run();
  }
}

void MainWindow::slotDaemonPpi()
{
  if(opt_develop.arg_debug) printf("slotDaemonPpi()\n");
  if(beginMenu())
  {
    dlgDaemon dlg(PPI_DAEMON);
    dlg.run();
  }
}

void MainWindow::slotLinuxWriteStartscript()
{
  if(opt_develop.arg_debug) printf("slotLinuxWriteStartscript()\n");
  if(beginMenu())
  {
    int ret;
    ret = writeStartscript(QDir::current().path().toUtf8(), name.toUtf8());
    if(ret != 0)
    {
      QMessageBox::information(this,"pvdevelop","Could not write startsript",QMessageBox::Ok);
    }
    else
    {
      QMessageBox::information(this,"pvdevelop",
                                    "traditional startscript written to project directory\n"
                                    "please review it, copy it to /etc/init.d/ and activate it through runlevel editor\n"
                                    "\n"
                                    "systemd <name>.service startscript also written to project directory",
                                    QMessageBox::Ok);
    }
  }
}

void MainWindow::slotAboutManual()
{
  if(opt_develop.arg_debug) printf("slotAboutManual()\n");
  if(dlgtextbrowser != NULL) dlgtextbrowser->show();
}

void MainWindow::slotInsertFunction()
{
  if(opt_develop.arg_debug) printf("slotInsertFunction()\n");
  if(name.isEmpty())
  {
    QMessageBox::information(this,"pvdevelop","No project loaded",QMessageBox::Ok);
    return;
  }
  dlgInsertFunction dlg;
  QString ret = dlg.run();
  if(opt_develop.arg_debug) printf("ret=%s\n",(const char *) ret.toUtf8());
  if(ret.isEmpty()) return;
  if((ret.contains(");") || ret.contains("=") || ret.contains("BIT")) && !ret.contains("(...)"))
  {
    if(curFile.contains(".py")) ret.remove(";");
    if(opt_develop.script == PV_LUA)
    {
      ret.remove(";");
      QString tmp;
      if(ret.startsWith("pv")) tmp = "pv.";
      if(ret.startsWith("rl")) tmp = "rllib.";
      tmp += ret;
      ret = tmp;
    }  
    editor->edit->textCursor().insertText(ret);
    return;
  }
  QMessageBox::information(this,"pvdevelop","Please choose function from tree",QMessageBox::Ok);
}

int MainWindow::beginMenu()
{
  if(name.isEmpty())
  {
    QMessageBox::information(this,"pvdevelop","No project loaded",QMessageBox::Ok);
    return 0;
  }
  maybeSave();
  return 1;
}

void MainWindow::endMenu()
{
  if(editor != NULL)
  {
    if(opt_develop.script == PV_LUA)
    {
      load(name + "main.lua");
      editor->radioMain->setChecked(true);
    }  
    else
    {
      load(name + ".pro");
      editor->radioProject->setChecked(true);
      perhapsFixCONFIG();
    }  
  }
}

void MainWindow::getWidgetNames(const QString &filename)
{
  if(editor == NULL) return;
  if(opt_develop.arg_debug) printf("getWidgetNames(%s)\n",(const char *) filename.toUtf8());

  char buf[1024];
  int i;
  QString line;
  editor->widgetname->clear();
  if(!filename.startsWith("mask")) return;
  QTextStream in;
  QString name;
  if(opt_develop.script == PV_LUA) name = "mask" + editor->spinBoxMask->text() + ".lua";
  else                     name = "mask" + editor->spinBoxMask->text() + ".cpp";
  QFile mask(name);
  if(!mask.open(QFile::ReadOnly)) return;

  in.setDevice(&mask);
  while(1)
  {
    line = in.readLine();
    if(line.isNull()) break;
    if(line.contains("ID_MAIN_WIDGET")) break;
  }
  while(1)
  {
    line = in.readLine();
    if(line.isNull()) break;
    if(line.contains("ID_END_OF_WIDGETS")) break;
    else
    {
      line.remove(" ");
      if(line.length() < (int) (sizeof(buf) - 1))
      {
        strcpy(buf,line.toUtf8());
        for(i=0; buf[i] != '\0'; i++)
        {
          if(buf[i]==',' || buf[i]=='=' || buf[i]==';')
          {
            buf[i] = '\0';
            break;
          }
        }
        editor->widgetname->addItem(buf);
      }  
    }
  }
  mask.close();
}

void MainWindow::slotWidgetname(QListWidgetItem *item)
{
  QString text = item->text();
  if(opt_develop.arg_debug) printf("slotWidgetname %s\n", (const char *) text.toUtf8());

  dlgPasteWidget dlg;
  QString ret = dlg.run(text.toUtf8(), &curFile);
  if(!ret.isEmpty())
  {
    editor->edit->textCursor().insertText(ret);
    return;
  }
}

void MainWindow::slotFileOpt()
{
int ret;

  if(opt_develop.arg_debug) printf("slotFileOpt\n");
  DlgOpt *dlg = new DlgOpt(this);
  dlg->setWindowTitle("pvdevelop options");
  dlg->setFilename(inifile());
  ret = dlg->exec();
  if(ret == QDialog::Accepted)
  {
    readIniFile();
    delete dlgtextbrowser;
    dlgtextbrowser = new dlgTextBrowser(opt_develop.manual);
  }
  delete dlg;
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
  if(event->modifiers() & Qt::AltModifier)     opt_develop.altPressed = 1;
  else                                         opt_develop.altPressed = 0;
  if(event->modifiers() & Qt::ControlModifier) opt_develop.ctrlPressed = 1;
  else                                         opt_develop.ctrlPressed = 0;
  if(event->modifiers() & Qt::ShiftModifier)   opt_develop.shiftPressed = 1;
  else                                         opt_develop.shiftPressed = 0;

  if(designer != NULL && designer->root != NULL)
  {
    if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
      designer->root->deleteLastChild();
      designer->root->stackClear();
    }
    else if(event->key() == Qt::Key_P)
    {
      designer->root->showProperties();
    }
    else if(event->key() == Qt::Key_G)
    {
      designer->root->GrabMouse();
    }
    else if(event->key() == Qt::Key_R)
    {
      designer->root->ReleaseMouse();
    }
    else if(event->key() == Qt::Key_L)
    {
      designer->root->EditLayout();
    }
    else if(event->key() == Qt::Key_Left  || 
            event->key() == Qt::Key_Right ||
            event->key() == Qt::Key_Up    ||
            event->key() == Qt::Key_Down  )
    {
      designer->root->MoveKey(event->key());
    }
    else if(event->key() == Qt::Key_Z && opt_develop.ctrlPressed)
    {
      designer->root->pop(NULL);
    }
  }
  //printf("ctrlPressed=%d\n",opt_develop.ctrlPressed);
  QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
  opt_develop.altPressed = 0;
  opt_develop.ctrlPressed = 0;
  opt_develop.shiftPressed = 0;
  //printf("ctrlPressed=%d\n",opt_develop.ctrlPressed);
  QWidget::keyPressEvent(event);
}
