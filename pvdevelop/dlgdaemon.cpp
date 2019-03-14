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
#include "dlgdaemon.h"
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#ifdef PVWIN32
#include <process.h>
#endif
QString target;

dlgDaemon::dlgDaemon(int _what)
{
  form = new Ui_DialogDaemon;
  form->setupUi(this);
  what = _what;
  QObject::connect(form->pushButtonCompile,SIGNAL(clicked()),SLOT(slotCompile()));
  QObject::connect(form->pushButtonCancel, SIGNAL(clicked()),SLOT(reject()));
  QObject::connect(form->pushButtonClose,  SIGNAL(clicked()),SLOT(slotClose()));
  QObject::connect(form->pushButtonOpen,  SIGNAL(clicked()),SLOT(slotOpen()));
  QObject::connect(form->pushButtonNew,  SIGNAL(clicked()),SLOT(slotNew()));
  switch(what)
  {
    case PPI_DAEMON:
      setWindowTitle("pvdevelop Make a PPI Daemon");
      form->labelFile->hide();
      form->pushButtonOpen->hide();
      load("ppidaemon.mkppi");
      break;
    case SIEMENSTCP_DAEMON:
      setWindowTitle("pvdevelop Make a SienensTCP Daemon");
      form->labelFile->hide();
      form->pushButtonOpen->hide();
      load("siemensdaemon.mksiemens");
      break;
    case MODBUS_DAEMON:
      setWindowTitle("pvdevelop Make a Modbus Daemon");
      form->labelFile->show();
      form->pushButtonOpen->show();
      load("modbusdaemon.mkmodbus");
      break;
    default:
      break;
  }
}

dlgDaemon::~dlgDaemon()
{
}

QString dlgDaemon::run()
{
  exec();
  return "";
}

void dlgDaemon::slotCompile()
{
  int ret = 0;
  QString Buf;
  QFileInfo f_info(filename);
  save();
  generate();
  switch(what)
  {
    case PPI_DAEMON:
#ifdef PVUNIX
#ifdef PVMAC
      QMessageBox::information(this,"pvdevelop","libnodave is not available for OS-X",QMessageBox::Ok);
      //ret = system("xterm -e \"g++ ppidaemon.cpp /opt/pvb/rllib/nodave.o /opt/pvb/rllib/setport.o -o ppidaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.dylib /usr/lib/libpthread.dylib;echo compiler_finished;read\"");
#else
      //ret = system("xterm -e \"g++ ppidaemon.cpp /opt/pvb/rllib/nodave.o /opt/pvb/rllib/setport.o -o ppidaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.so -lpthread;echo compiler_finished;read\"");
      ret = system("xterm -e \"g++ -c ppidaemon.cpp  -o ppidaemon.o -I/opt/pvb/rllib/lib -L/usr/lib/; g++ ppidaemon.o /opt/pvb/rllib/lib/nodave.o /opt/pvb/rllib/lib/setport.o /usr/lib/librllib.so -lpthread -o ppidaemon;echo compiler_finished;read\"");
#endif
#else
      ret = system("start pvb_make_ppidaemon.bat");
#endif
      break;
    case SIEMENSTCP_DAEMON:
#ifdef PVUNIX
#ifdef PVMAC
      ret = system("xterm -e \"g++ siemensdaemon.cpp -o siemensdaemon -I/opt/pvb/rllib/lib -L/usr/lib/ -L/usr/local/lib/ /usr/local/lib/librllib.dylib /usr/lib/libpthread.dylib;echo compiler_finished;read\"");
#else
      //ret = system("xterm -e \"g++ siemensdaemon.cpp -o siemensdaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.so -lpthread;echo compiler_finished;read\"");
      ret = system("xterm -e \"g++ -c siemensdaemon.cpp  -o siemensdaemon.o -I/opt/pvb/rllib/lib -L/usr/lib/; g++ siemensdaemon.o /usr/lib/librllib.so -lpthread -o siemensdaemon ;echo compiler_finished;read\"");
#endif
#else
      ret = system("start pvb_make_siemensdaemon.bat");
#endif
      break;
    case MODBUS_DAEMON:
#ifdef PVUNIX
#ifdef PVMAC
      //ret = system("xterm -e \"g++ modbusdaemon.cpp -o modbusdaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.dylib /usr/lib/libpthread.dylib;echo compiler_finished;read\"");
      Buf = "xterm -e g++ \""+target+".cpp\" -o "+f_info.baseName()+" -I/opt/pvb/rllib/lib -L/usr/lib/ -L/usr/local/lib/ /usr/local/lib/librllib.dylib /usr/lib/libpthread.dylib;echo compiler_finished;read";
#else
      //ret = system("xterm -e \"g++ modbusdaemon.cpp -o modbusdaemon -I/opt/pvb/rllib/lib -L/usr/lib/ /usr/lib/librllib.so -lpthread;echo compiler_finished;read\"");
      //ret = system("xterm -e \"g++ -c modbusdaemon.cpp  -o modbusdaemon.o -I/opt/pvb/rllib/lib -L/usr/lib/; g++ modbusdaemon.o /usr/lib/librllib.so -lpthread -o modbusdaemon;echo compiler_finished;read\"");
      Buf = "xterm -e 'g++ -c \""+target+".cpp\"  -o \""+target+".o\" -I/opt/pvb/rllib/lib -L/usr/lib/; g++ \""+target+".o\" /usr/lib/librllib.so -lpthread -o "+f_info.baseName()+";echo compiler_finished;read'";
#endif
#else
      //ret = system("start pvb_make_modbusdaemon.bat");
      Buf = "g++ \""+target+".cpp\" \"%PVBDIR%\\win-mingw\\bin\\librllib.a\" -lws2_32 \"-I%PVBDIR%\\rllib\\lib\" -ladvapi32  -static-libgcc -o \""+f_info.baseName()+".exe\"";
      Buf = "echo "+Buf+" & "+Buf+" & pause";
#endif
      ret = system(Buf.toUtf8().data());
      break;
    default:
      break;
  }
  if(ret < 0) return;
}

void dlgDaemon::slotClose()
{
  save();
  slotCompile();
  accept();
}

void dlgDaemon::slotOpen()
{
   filename =  QFileDialog::getOpenFileName(this,"Modbus - Open File","","modbus (*.mkmodbus)");
   if(!filename.isEmpty())
   {
       load(QDir::toNativeSeparators(filename));
   }
}
void dlgDaemon::slotNew()
{
   filename =  QFileDialog::getSaveFileName(this,"Modbus - New File","","*.mkmodbus");
   if(!filename.isEmpty())
   {
       if(!filename.contains(".mkmodbus"))
       {
           filename.append(".mkmodbus");
       }
       load(QDir::toNativeSeparators(filename));
   }
}

void dlgDaemon::load(QString name)
{
  filename = name;
  target = name.remove(".mkmodbus");
  form->textEdit->clear();
  form->labelFile->setText(filename);
  QFile f(filename);
  if(f.open(QIODevice::ReadOnly))
  {
    QTextStream ts( &f );
    form->textEdit->document()->setPlainText( ts.readAll() );
  }
  else
  {
    switch(what)
    {
      case PPI_DAEMON:
        form->textEdit->append("############################################");
        form->textEdit->append("# Project file for generating ppi daemon   #");
        form->textEdit->append("# mkppi name.mkppi                         #");
        form->textEdit->append("############################################");
        form->textEdit->append("shared_memory=/srv/automation/shm/ppi.shm");
        form->textEdit->append("mailbox=/srv/automation/mbx/ppi.mbx");
        form->textEdit->append("tty=/dev/ttyS0");
        form->textEdit->append("baudrate=9600");
        form->textEdit->append("# daveSpeed9k | daveSpeed19k | daveSpeed187k | daveSpeed500k daveSpeed1500k | daveSpeed45k | daveSpeed93k");
        form->textEdit->append("dave_speed=daveSpeed187k");
        form->textEdit->append("idletime=50 milliseconds");
        form->textEdit->append("#eventlog host=localhost port=6000");
        form->textEdit->append("# area :=");
        form->textEdit->append("# daveSD");
        form->textEdit->append("# daveInputs");
        form->textEdit->append("# daveOutputs");
        form->textEdit->append("# daveFlags");
        form->textEdit->append("# daveDB         //data blocks");
        form->textEdit->append("# daveDI         //not tested");
        form->textEdit->append("# daveLocal      //not tested");
        form->textEdit->append("# daveV          //don't know what it is");
        form->textEdit->append("# daveCounter    //not tested");
        form->textEdit->append("# daveTimer      //not tested");
        form->textEdit->append("# cycle := area, dbnum, start, len");
        form->textEdit->append("cycle1 slave=2 area=daveDB dbnum=1 start=0  len=64");
        form->textEdit->append("cycle2 slave=2 area=daveDB dbnum=1 start=64 len=64");
        break;
      case SIEMENSTCP_DAEMON:
        form->textEdit->append("################################################");
        form->textEdit->append("# Project file for generating siemens daemon   #");
        form->textEdit->append("# mksiemens name.mksiemens                     #");
        form->textEdit->append("################################################");
        form->textEdit->append("shared_memory=/srv/automation/shm/siemens.shm");
        form->textEdit->append("mailbox=/srv/automation/mbx/siemens.mbx");
        form->textEdit->append("# type := S7_200 | S7_300 | S7_400 | S5 | S7_1200 | LOGO");
        form->textEdit->append("slave=0 adr=192.168.1.101 type=S7_200 fetch_write=1 function=-1 rack_slot=-1");
        form->textEdit->append("slave=1 adr=192.168.1.102 type=S7_200 fetch_write=0 function=-1 rack_slot=-1");
        form->textEdit->append("idletime=50 milliseconds");
        form->textEdit->append("#eventlog host=localhost port=6000");
        form->textEdit->append("# ORG := ORG_DB | ORG_M | ORG_E | ORG_A | ORG_PEPA | ORG_Z | ORG_T");
        form->textEdit->append("# cycle := slave + org + dbnum + start + len");
        form->textEdit->append("cycle1 slave=0 org=ORG_E  dbnum=0 start=0 len=8");
        form->textEdit->append("cycle2 slave=0 org=ORG_A  dbnum=0 start=0 len=8");
        form->textEdit->append("cycle3 slave=0 org=ORG_DB dbnum=0 start=0 len=32");
        break;
      case MODBUS_DAEMON:
        form->textEdit->append("############################################");
        form->textEdit->append("# Project file for generating modbusdaemon #");
        form->textEdit->append("# mkmodbus name.mkmodbus                   #");
        form->textEdit->append("############################################");
        form->textEdit->append("shared_memory=/srv/automation/shm/modbus.shm");
        form->textEdit->append("mailbox=/srv/automation/mbx/modbus.mbx");
        form->textEdit->append("#communication=serial");
        form->textEdit->append("tty=/dev/ttyS0");
        form->textEdit->append("baudrate=9600");
        form->textEdit->append("rtscts=1");
        form->textEdit->append("parity=0   # 0=NONE 1=ODD 2=EVEN");
        form->textEdit->append("protocol=0 # 0=RTU  1=ASCII");
        form->textEdit->append("communication=socket");
        form->textEdit->append("tcpadr=lehrig2");
        form->textEdit->append("tcpport=502");
        form->textEdit->append("#eventlog host=localhost port=6000");
        form->textEdit->append("# function := 1=ReadCoilStatus | 2=ReadInputStatus | 3=ReadHoldingRegisters | 4=ReadInputRegisters");
        form->textEdit->append("#             1 coil per bit   | 1 input per bit   | 2 byte per register    | 2byte per register");
        form->textEdit->append("cycle1 slave=1 function=1 start_adr=0 num_register=3");
        form->textEdit->append("cycle2 slave=2 function=2 start_adr=0 num_register=3");
        form->textEdit->append("cycle3 slave=3 function=3 start_adr=0 num_register=3");
        form->textEdit->append("cycle4 slave=4 function=4 start_adr=0 num_register=3");
        form->textEdit->append("cycle5 slave=5 function=2 start_adr=0 num_register=3");
        form->textEdit->append("cycle6 slave=6 function=2 start_adr=0 num_register=3");
        form->textEdit->append("cycle7 slave=7 function=2 start_adr=0 num_register=3");
        break;
      default:  
        break;
    }
  }
  form->textEdit->document()->setModified(false);
}

void dlgDaemon::save()
{
  QString text = form->textEdit->document()->toPlainText();
  QFile f(filename);
  if(!f.open(QIODevice::WriteOnly)) return;
  QTextStream t( &f );
  t << text;
  f.close();
  form->textEdit->document()->setModified(false);
}

int gppi(const char *name);
int gsiemens(const char *name);
int gmodbus(const char *name);

void dlgDaemon::generate()
{
  switch(what)
  {
    case PPI_DAEMON:
      gppi(filename.toUtf8());
      break;
    case SIEMENSTCP_DAEMON:
      gsiemens(filename.toUtf8());
      break;
    case MODBUS_DAEMON:
      gmodbus(filename.toUtf8());
      break;
    default:
      break;
  }
}
