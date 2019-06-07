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
#include "pvdefine.h"
#include <QtCore>
#include <QMessageBox>
#include "opt.h"
#ifdef USE_ANDROID
#include <sys/stat.h> // android
#include <sys/types.h> // android
#endif
#ifdef PVWIN32
#include <windows.h>
#include <direct.h>
#endif
#ifdef PVUNIX
#include <unistd.h>
#endif

OPT opt;

QString l_file                      = "&File";
QString l_options                   = "&Options";
QString l_new_window                = "&New Window";
QString l_reconnect                 = "&Reconnect";
QString l_save_as_bmp               = "&Save as BMP ...";
QString l_log_as_bmp                = "Log Metafiles as &BMP ...";
QString l_log_as_pvm                = "Log Metafiles as P&VM ...";
QString l_print                     = "&Print";
QString l_new_tab                   = "New &Tab";
QString l_delete_tab                = "Close Tab";
QString l_exit                      = "E&xit";
QString l_edit                      = "&Edit";
QString l_copy                      = "&Copy";
QString l_copy_plus_title           = "Cop&y including title";
QString l_view                      = "&View";
QString l_editmenu                  = "&Editmenu";
QString l_toolbar                   = "&Toolbar";
QString l_statusbar                 = "&Statusbar";
QString l_maximized                 = "&Maximized";
QString l_fullscreen                = "&Fullscreen";
QString l_help                      = "&Help";
QString l_manual                    = "&Manual";
QString l_about                     = "&About ...";

QString l_status_connection_lost    = "Connection to server lost, CTRL-R for reconnect";
QString l_status_connected          = "Connected to server";
QString l_status_could_not_connect  = "Could not connect to server, CTRL-R for reconnect";
QString l_status_reconnect          = "Reconnect to host";
QString l_status_options            = "Views/changes pvbrowser options";
QString l_status_new_window         = "Opens a new pvbrowser window";
QString l_status_save_as_bmp        = "Saves the actual screen as BMP file";
QString l_status_log_as_bmp         = "Log QDrawWidgets as BMP file";
QString l_status_log_as_pvm         = "Log QDrawWidgets as PVM file";
QString l_status_print              = "Prints the actual screen";
QString l_status_new_tab            = "Opens a new Tab";
QString l_status_exit               = "Closes pvbrowser";
QString l_status_copy               = "Copies the screen to clipboard";
QString l_status_editmenu           = "Enables/disables the editmenu";
QString l_status_toolbar            = "Enables/disables the toolbar";
QString l_status_statusbar          = "Enables/disables the statusbar";
QString l_status_toggle_maximized   = "Maximized On/Off";
QString l_status_toggle_full_screen = "FullScreenMode On/Off";
QString l_status_manual             = "Shows application specific help";
QString l_status_about              = "Shows the aboutbox";

QString l_print_header              = "Printed by pvbrowser at: ";

const char *writeableLocation()
{
  static QString text;
  //text = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  text = getenv("HOME");
  return text.toUtf8();
}

int wsa()
{
#ifdef PVWIN32
  WORD    wVersionRequested;
  WSADATA wsadata;
  int     err;

#ifdef IS_OLD_MSVCPP
  wVersionRequested = MAKEWORD(1,1);
#else
  wVersionRequested = MAKEWORD(2,0);
#endif
  err = WSAStartup(wVersionRequested, &wsadata);
  if(err != 0)
  {
    printf("Startup error=%d on windows\n",err);
    exit(0);
  }
#endif
  return 0;
}

void getLanguage(const char *bufin)
{
  char *cptr;
  char phrase[MAXOPT],buf[MAXOPT];
  QString qphrase;

  strcpy(buf,bufin);
  cptr = strchr(buf,'='); // get phrase
  cptr++;
  if(*cptr == ' ') cptr ++;
  strcpy(phrase,cptr);
  cptr = strchr(phrase,'\n');
  if(cptr != NULL) *cptr = '\0';
  qphrase = QString::fromUtf8(phrase);

  if     (strncmp(buf,"-print_header",13)              == 0) l_print_header = qphrase;
  else if(strncmp(buf,"-file",5)                       == 0) l_file = qphrase;
  else if(strncmp(buf,"-options",8)                    == 0) l_options = qphrase;
  else if(strncmp(buf,"-new_window",11)                == 0) l_new_window = qphrase;
  else if(strncmp(buf,"-new_tab",8)                    == 0) l_new_tab = qphrase;
  else if(strncmp(buf,"-delete_tab",11)                == 0) l_delete_tab = qphrase;
  else if(strncmp(buf,"-reconnect",10)                 == 0) l_reconnect = qphrase;
  else if(strncmp(buf,"-save_as_bmp",12)               == 0) l_save_as_bmp = qphrase;
  else if(strncmp(buf,"-log_as_bmp",11)                == 0) l_log_as_bmp = qphrase;
  else if(strncmp(buf,"-log_as_pvm",11)                == 0) l_log_as_pvm = qphrase;
  else if(strncmp(buf,"-print",6)                      == 0) l_print = qphrase;
  else if(strncmp(buf,"-exit",5)                       == 0) l_exit = qphrase;
  else if(strncmp(buf,"-edit",5)                       == 0) l_edit = qphrase;
  else if(strncmp(buf,"-copy_plus_title",16)           == 0) l_copy_plus_title = qphrase;
  else if(strncmp(buf,"-copy",5)                       == 0) l_copy = qphrase;
  else if(strncmp(buf,"-toolbar",8)                    == 0) l_toolbar = qphrase;
  else if(strncmp(buf,"-statusbar",10)                 == 0) l_statusbar = qphrase;
  else if(strncmp(buf,"-maximized",10)                 == 0) l_maximized = qphrase;
  else if(strncmp(buf,"-fullscreen",11)                == 0) l_fullscreen = qphrase;
  else if(strncmp(buf,"-view",5)                       == 0) l_view = qphrase;
  else if(strncmp(buf,"-help",5)                       == 0) l_help = qphrase;
  else if(strncmp(buf,"-manual",7)                     == 0) l_manual = qphrase;
  else if(strncmp(buf,"-about",6)                      == 0) l_about = qphrase;
  else if(strncmp(buf,"-status_connection_lost",23)    == 0) l_status_connection_lost = qphrase;
  else if(strncmp(buf,"-status_connected",17)          == 0) l_status_connected = qphrase;
  else if(strncmp(buf,"-status_could_not_connect",25)  == 0) l_status_could_not_connect = qphrase;
  else if(strncmp(buf,"-status_reconnect",17)          == 0) l_status_reconnect = qphrase;
  else if(strncmp(buf,"-status_options",15)            == 0) l_status_options = qphrase;
  else if(strncmp(buf,"-status_new_window",18)         == 0) l_status_new_window = qphrase;
  else if(strncmp(buf,"-status_new_tab",15)            == 0) l_status_new_tab = qphrase;
  else if(strncmp(buf,"-status_save_as_bmp",19)        == 0) l_status_save_as_bmp = qphrase;
  else if(strncmp(buf,"-status_log_as_bmp",18)         == 0) l_status_log_as_bmp = qphrase;
  else if(strncmp(buf,"-status_log_as_pvm",18)         == 0) l_status_log_as_pvm = qphrase;
  else if(strncmp(buf,"-status_print",13)              == 0) l_status_print = qphrase;
  else if(strncmp(buf,"-status_exit",12)               == 0) l_status_exit = qphrase;
  else if(strncmp(buf,"-status_copy",12)               == 0) l_status_copy = qphrase;
  else if(strncmp(buf,"-status_editmenu",16)           == 0) l_status_editmenu = qphrase;
  else if(strncmp(buf,"-status_toolbar",15)            == 0) l_status_toolbar = qphrase;
  else if(strncmp(buf,"-status_statusbar",17)          == 0) l_status_statusbar = qphrase;
  else if(strncmp(buf,"-status_toggle_maximized",24)   == 0) l_status_toggle_maximized = qphrase;
  else if(strncmp(buf,"-status_toggle_full_screen",26) == 0) l_status_toggle_full_screen = qphrase;
  else if(strncmp(buf,"-status_manual",14)             == 0) l_status_manual = qphrase;
  else if(strncmp(buf,"-status_about",13)              == 0) l_status_about = qphrase;
}

const char *inifile()
{
  static char name[MAXOPT];

  if(opt.arg_localini == 1)
  {
    strcpy(name,opt.arg_ini);
    return name;
  }
#ifdef PVUNIX
#ifdef USE_ANDROID
  strcpy(name,writeableLocation());
  strcat(name,"/pvbrowser/pvbrowser.ini"); // android
#elif defined(USE_SYMBIAN)
  //SECUREID (magic number!)  0xE537072d
  //in symbian the normal dir is under \\Private\\<secureid>
  //but we prefer a pvb dir...
  if(!mkdir("\\pvb",S_IWUSR)){
    qDebug()<<"Error creating pvb dir";
  }
  strcpy(name,"\\pvb\\pvbrowser.ini");
#else
  strcpy(name,getenv("HOME"));
  strcat(name,"/.pvbrowser.ini");
#endif
#endif
#ifdef __VMS
  strcpy(name,"sys$login:pvbrowser.ini");
#endif
#ifdef PVWIN32
  ExpandEnvironmentStringsA("%USERPROFILE%",name,sizeof(name)-1);
  if(strcmp(name,"%USERPROFILE%") == 0) strcpy(name,"C:");
  strcat(name,"\\pvbrowser.ini");
#endif
  return name;
}

const char *passfile()
{
  static char name[MAXOPT];

  if(opt.arg_localini == 1)
  {
    strcpy(name,"pvbrowserpass.ini");
    return name;
  }
#ifdef PVUNIX
#ifdef USE_ANDROID
  strcpy(name,writeableLocation());
  strcat(name,"/pvbrowser/pvbrowserpass.ini"); // android
#elif defined(USE_SYMBIAN)
  strcpy(name,"\\pvb\\pvbrowserpass.ini");
#else
  strcpy(name,getenv("HOME"));
  strcat(name,"/.pvbrowserpass.ini");
#endif
#endif
#ifdef __VMS
  strcpy(name,"sys$login:pvbrowserpass.ini");
#endif
#ifdef PVWIN32
  ExpandEnvironmentStringsA("%USERPROFILE%",name,sizeof(name)-1);
  if(strcmp(name,"%USERPROFILE%") == 0) strcpy(name,"C:");
  strcat(name,"\\pvbrowserpass.ini");
#endif
  return name;
}

const char *pvpass(const char *p)
{
  static char ret[4*16+8];
  char buf[80];
  int  i,val;

  ret[0] = '\0';
  for(i=0; p[i] != '\0' && i<=16; i++)
  {
    val = p[i] * 16;
    sprintf(buf,"%04X",val);
    strcat(ret,buf);
  }
  return ret;
}

void setDefaultOptions()
{
  opt.port=5050;
  opt.sshport=50500;
  opt.zoom=100;
  opt.fontzoom=100;
  opt.appfontsize=0;
  opt.autoreconnect=0;
  opt.exitpassword=0;
  opt.menubar=1;
  opt.toolbar=1;
  opt.statusbar=1;
  opt.scrollbars=1;
  opt.fullscreen=0;
  opt.maximized=0;
  opt.tabs_above_toolbar=0;
  opt.cursor_shape=-1;
  opt.echo_table_updates=0;
#if QT_VERSION >= 0x040601
  opt.use_webkit_for_svg=0;
#else
  opt.use_webkit_for_svg=0;
#endif
  opt.enable_webkit_plugins=0;
  opt.temp[0] = '\0';
  opt.customlogo[0] = '\0';
  opt.newwindow[0] = '\0';
  opt.ssh[0] = '\0';
  opt.i_have_started_servers = 0;
  opt.view_audio[0] = '\0';
  opt.view_video[0] = '\0';
  opt.view_pdf[0] = '\0';
  opt.view_img[0] = '\0';
  opt.view_svg[0] = '\0';
  opt.view_txt[0] = '\0';
  opt.view_html[0] = '\0';
  opt.initialhost[0] = '\0';
  opt.language[0] = '\0';
  opt.codec = pvbUTF8; // strongly advised to use pvbUTF8
  opt.closed = 0;
  opt.cookies = 1; // YES
  opt.generate_cookie[0] = '\0';
  opt.pvb_com_plugin[0] = '\0';
  opt.pvb_widget_plugindir[0] = '\0';
  strcpy(opt.manual,"index.html");
  opt.connect_timeout = 3;
  getcwd(opt.initial_dir, sizeof(opt.initial_dir) - 1);
  opt.ffmpeg_available = 0;
  opt.ffplay_available = 0;
  strcpy(opt.proxyadr,"localhost");
  opt.proxyport = -1;
  opt.replace_svg_symbol_by_g = 1;
}

const char *readIniFile()
{
FILE *fp;
char buf[MAXOPT],buf2[MAXOPT],cmd[MAXOPT];
const char *cptr;
int i;

  setDefaultOptions();

  while(1)
  {
    fp = fopen(inifile(),"r");
    if(fp != NULL)
    {
      while( fgets(buf,sizeof(buf)-1,fp) != NULL )
      {
        char *ptr = strchr(buf,'\n');
        if(ptr != NULL) *ptr = '\0';
        if(strncmp(buf,"port=",5) == 0)
        {
          sscanf(buf,"port=%d",&opt.port);
        }
        else if(strncmp(buf,"sshport=",8) == 0)
        {
          sscanf(buf,"sshport=%d",&opt.sshport);
        }
        else if(strncmp(buf,"zoom=",5) == 0)
        {
          sscanf(buf,"zoom=%d",&opt.zoom);
          if(opt.zoom < 1) opt.zoom = 1;
        }
        else if(strncmp(buf,"fontzoom=",9) == 0)
        {
          sscanf(buf,"fontzoom=%d",&opt.fontzoom);
        }
        else if(strncmp(buf,"appfontsize=",12) == 0)
        {
          sscanf(buf,"appfontsize=%d",&opt.appfontsize);
        }
        else if(strncmp(buf,"autoreconnect=",14) == 0)
        {
          sscanf(buf,"autoreconnect=%d",&opt.autoreconnect);
        }
        else if(strncmp(buf,"scrollbars=",11) == 0)
        {
          sscanf(buf,"scrollbars=%d",&opt.scrollbars);
        }
        else if(strncmp(buf,"cookies=",8) == 0)
        {
          sscanf(buf,"cookies=%d",&opt.cookies);
        }
        else if(strncmp(buf,"generate_cookie=",16) == 0)
        {
          strcpy(opt.generate_cookie,&buf[16]);
        }
        else if(strncmp(buf,"pvb_com_plugin=",15) == 0)
        {
          strcpy(opt.pvb_com_plugin,&buf[15]);
        }
        else if(strncmp(buf,"pvb_widget_plugindir=",21) == 0)
        {
          strcpy(opt.pvb_widget_plugindir,&buf[21]);
#ifdef PVWIN32
          strcat(opt.pvb_widget_plugindir,"\\");
#else
#ifdef USE_SYMBIAN
          strcat(opt.pvb_widget_plugindir,"\\");
#else
          strcat(opt.pvb_widget_plugindir,"/");
#endif
#endif
        }
        else if(strncmp(buf,"enable_webkit_plugins=",22) == 0)
        {
          sscanf(buf,"enable_webkit_plugins=%d",&opt.enable_webkit_plugins);
        }
        else if(strncmp(buf,"temp=",5) == 0)
        {
          int ret;
          strcpy(buf2, &buf[5]);
#ifdef PVUNIX
#ifdef USE_SYMBIAN
           if(!mkdir(buf2,S_IWUSR))
           {
             qDebug()<<"Error creating temp dir";
           }
#else
          sprintf(cmd,"mkdir -p %s", buf2);
          if(system(cmd) != 0) printf("could not create temporary directory: %s\n", cmd);
#endif
#endif
#ifdef PVWIN32
          ExpandEnvironmentStringsA(buf2,buf,sizeof(buf)-1);
          if(strstr(buf,"%") != NULL) QMessageBox::warning(NULL,buf,"readIniFile temp directory unknown: adjust pvbrowser.ini temp=");
          strcpy(buf2,buf);
#endif
          ret = chdir(buf2);
          if(ret < 0) 
          {
#ifdef PVUNIX
            QMessageBox::warning(NULL,buf2,"readIniFile could not change to temp dir: adjust ~/.pvbrowser.ini temp=");
            ret = system("xterm -e \"vi ~/.pvbrowser.ini\" &");
#endif
#ifdef PVWIN32
            QMessageBox::warning(NULL,buf2,"readIniFile could not change to temp dir: adjust pvbrowser.ini temp=");
            sprintf(buf,"notepad %s",inifile());
            system(buf);
#endif
            return "Error chdir";
          }
#ifdef PVUNIX
#ifdef USE_SYMBIAN
          strcat(buf2,"\\");
#else
          strcat(buf2,"/");
#endif
#endif
#ifdef PVWIN32
          strcat(buf2,"\\");
#endif
          strcpy(opt.temp,buf2);
#ifdef PVUNIX
          system("which ffmpeg > ffmpeg.dat && which ffplay >> ffmpeg.dat");
          FILE *fin = fopen("ffmpeg.dat","r");
          if(fin != NULL)
          {
            char line[1024];
            fgets(line, (int) sizeof(line)-1, fin);
            if(strstr(line,"ffmpeg") != NULL) opt.ffmpeg_available = 1;
            fgets(line, (int) sizeof(line)-1, fin);
            if(strstr(line,"ffplay") != NULL) opt.ffplay_available = 1;
            if(opt.arg_debug) printf("ffmpeg_available=%d ffplay_available=%d\n", opt.ffmpeg_available, opt.ffplay_available);
            fclose(fin);
            system("rm ffmpeg.dat");
          }
#endif
        }
        else if(strncmp(buf,"proxyadr=",9) == 0)
        {
          buf[MAXOPT - 1] = '\0';
          strcpy(opt.proxyadr,&buf[9]);
          char *cptr;
          cptr = strchr(opt.proxyadr,'\n');
          if(cptr != NULL) *cptr = '\0';
          cptr = strchr(opt.proxyadr,' ');
          if(cptr != NULL) *cptr = '\0';
        }
        else if(strncmp(buf,"proxyport=",10) == 0)
        {
          sscanf(buf,"proxyport=%d",&opt.proxyport);
        }
        else if(strncmp(buf,"toolbar=",8) == 0)
        {
          sscanf(buf,"toolbar=%d",&opt.toolbar);
        }
        else if(strncmp(buf,"statusbar=",10) == 0)
        {
          sscanf(buf,"statusbar=%d",&opt.statusbar);
        }
        else if(strncmp(buf,"menubar=",8) == 0)
        {
          sscanf(buf,"menubar=%d",&opt.menubar);
        }
        else if(strncmp(buf,"initialhost=",12) == 0)
        {
          sscanf(buf,"initialhost=%s",buf2);
          strcpy(opt.initialhost,buf2);
        }
        else if(strncmp(buf,"fullscreen=",11) == 0)
        {
          sscanf(buf,"fullscreen=%d",&opt.fullscreen);
        }
        else if(strncmp(buf,"maximized=",10) == 0)
        {
          sscanf(buf,"maximized=%d",&opt.maximized);
        }
        else if(strncmp(buf,"tabs_above_toolbar=",19) == 0)
        {
          sscanf(buf,"tabs_above_toolbar=%d",&opt.tabs_above_toolbar);
        }
        else if(strncmp(buf,"cursor_shape=",13) == 0)
        {
          sscanf(buf,"cursor_shape=%d",&opt.cursor_shape);
        }
        else if(strncmp(buf,"exitpassword=",13) == 0)
        {
          sscanf(buf,"exitpassword=%d",&opt.exitpassword);
        }
        else if(strncmp(buf,"echo_table_updates=",19) == 0)
        {
          sscanf(buf,"echo_table_updates=%d",&opt.echo_table_updates);
        }
        else if(strncmp(buf,"use_webkit_for_svg=",19) == 0)
        {
          sscanf(buf,"use_webkit_for_svg=%d",&opt.use_webkit_for_svg);
        }
        else if(strncmp(buf,"enable_webkit_plugins=",22) == 0)
        {
          sscanf(buf,"enable_webkit_plugins=%d",&opt.enable_webkit_plugins);
        }
        else if(strncmp(buf,"newwindow=",10) == 0)
        {
          sscanf(buf,"newwindow=%s",buf);
          strcpy(opt.newwindow,buf);
        }
        else if(strncmp(buf,"ssh=",4) == 0)
        {
          strcpy(opt.ssh,&buf[4]);
        }
        else if(strncmp(buf,"start=",6) == 0)
        {
          mysystem(&buf[6]);
          opt.i_have_started_servers++;
        }
        else if(strncmp(buf,"fillbackground=1",16) == 0)
        {
          opt.arg_fillbackground = 1;
        }
        else if(strncmp(buf,"view.audio=",11) == 0)
        {
          strcpy(opt.view_audio,&buf[11]);
        }
        else if(strncmp(buf,"view.video=",11) == 0)
        {
          strcpy(opt.view_video,&buf[11]);
        }
        else if(strncmp(buf,"view.pdf=",9) == 0)
        {
          strcpy(opt.view_pdf,&buf[9]);
        }
        else if(strncmp(buf,"view.img=",9) == 0)
        {
          strcpy(opt.view_img,&buf[9]);
        }
        else if(strncmp(buf,"view.svg=",9) == 0)
        {
          strcpy(opt.view_svg,&buf[9]);
        }
        else if(strncmp(buf,"view.txt=",9) == 0)
        {
          strcpy(opt.view_txt,&buf[9]);
        }
        else if(strncmp(buf,"view.csv=",9) == 0)
        {
          strcpy(opt.view_csv,&buf[9]);
        }
        else if(strncmp(buf,"view.html=",10) == 0)
        {
          strcpy(opt.view_html,&buf[10]);
        }
        else if(strncmp(buf,"language =",10) == 0)
        {
          cptr = strchr(buf,'=');
          cptr++;
          if(*cptr == ' ') cptr++;
          sscanf(cptr,"%s",opt.language);
          opt.language[100] = '\0';
        }
        else if(strncmp(buf,"customlogo=",11) == 0)
        {
#ifdef PVWIN32
          strcpy(buf2, buf);
          ExpandEnvironmentStringsA(buf2,buf,sizeof(buf)-1);
          if(strstr(buf,"%") != NULL) QMessageBox::warning(NULL,buf,"readIniFile customlogo directory unknown: adjust pvbrowser.ini temp=");
#endif
          cptr = strchr(buf,'=');
          cptr++;
          if(*cptr == ' ') cptr++;
          strcpy(opt.customlogo, cptr);
        }
        else if(strncmp(buf,"codec=",6) == 0)
        {
          opt.codec = pvbUTF8;
          if(strstr(buf,"=None") != NULL) opt.codec = pvbNone;
        }
        else if(buf[0] != '#' && strchr(buf,'{') != NULL)
        {
          for(i=0; buf[i] != ' ' && buf[i] != '{'; i++)
          {
            opt.language_section[i] = buf[i];
          }
          opt.language_section[i] = '\0';
        }
        else if(buf[0] == '-' && strcmp(opt.language,opt.language_section) == 0)
        {
          getLanguage(buf);
        }
        else if(strncmp(buf,"connect_timeout=",16) == 0)
        {
          sscanf(buf,"connect_timeout=%d",&opt.connect_timeout);
        }
        if(opt.arg_debug) printf("Option: %s\n", buf);
      }
      fclose(fp);
      return NULL;
    }
    else // write a default initialisation file
    {
#ifdef USE_ANDROID
      chdir(writeableLocation());
      mkdir("pvbrowser", 0x0fff); // android
      mkdir("pvbrowser/temp", 0x0fff); // android             
#endif
#ifdef USE_SYMBIAN
      mkdir("\\pvb",S_IWUSR);
#endif
      fp = fopen(inifile(),"w");
      if(fp != NULL)
      {
        fprintf(fp,"###############################################\n");
        fprintf(fp,"# This is the default initialization file for\n");
        fprintf(fp,"# ProcessViewBrowser\n");
        fprintf(fp,"###############################################\n");
        fprintf(fp,"port=5050               # default port\n");
        fprintf(fp,"sshport=50500           # local port for ssh connections\n");
        fprintf(fp,"zoom=100                # zoom factor in percent\n");
        fprintf(fp,"fontzoom=100            # zoom factor for fonts in percent.\n");
        fprintf(fp,"appfontsize=0           # size of the application font. If appfontsize == 0 then use default font.\n");
        fprintf(fp,"fillbackground=0        # 0|1 transparency in dialog boxes\n");
        fprintf(fp,"codec=utf8              # utf8 | None where None uses latin charset\n");
        fprintf(fp,"autoreconnect=0         # 0|1\n");
        fprintf(fp,"exitpassword=0          # 0|1\n");
        fprintf(fp,"menubar=1               # 0|1\n");
        fprintf(fp,"toolbar=1               # 0|1\n");
        fprintf(fp,"statusbar=1             # 0|1\n");
        fprintf(fp,"scrollbars=1            # 0|1\n");
        fprintf(fp,"fullscreen=0            # 0|1\n");
        fprintf(fp,"maximized=0             # 0|1\n");
        fprintf(fp,"tabs_above_toolbar=0    # 0|1\n");
        fprintf(fp,"cursor_shape=-1         # -1 or Qt::CursorShape\n");
        fprintf(fp,"cookies=1               # 0=No 1=Yes 2=Ask\n");
        fprintf(fp,"echo_table_updates=0    # 0|1\n");
#if QT_VERSION >= 0x040601
        fprintf(fp,"use_webkit_for_svg=0    # 0|1\n");
#else
        fprintf(fp,"use_webkit_for_svg=0    # 0|1\n");
#endif
#ifdef PVUNIX
        fprintf(fp,"enable_webkit_plugins=0 # 0|1\n"); // Linux bug in NSPlugin/Webkit/JavaScript
#else
        fprintf(fp,"enable_webkit_plugins=1 # 0|1\n");
#endif
        fprintf(fp,"# temporary directory\n");
#ifdef PVUNIX
#ifdef USE_ANDROID
        fprintf(fp,"temp=%s/pvbrowser/temp\n", writeableLocation());
#else
#ifdef USE_SYMBIAN
        if(!mkdir("\\pvb\\temp",S_IWUSR))
        {
           qDebug()<<"Error creating temp dir";
        }
        fprintf(fp,"temp=\\pvb\\temp\n");
#else        
        sprintf(buf,"mkdir -p /tmp/pvb-%s", getenv("USER"));
        if(system(buf) != 0) printf("could not create temporary directory: %s\n", buf);
        fprintf(fp,"temp=/tmp/pvb-%s\n", getenv("USER"));
#endif
#endif
#endif
#ifdef PVWIN32
        //fprintf(fp,"temp=%%PVBDIR%%\\win\\temp\n");
        fprintf(fp,"temp=%%TEMP%%\n");
#endif
#ifdef __VMS
        fprintf(fp,"temp=sys$login:\n");
#endif
        fprintf(fp,"# your custom logo\n");
#ifdef PVUNIX
        fprintf(fp,"customlogo=/opt/pvb/gamsleiten.png\n");
#endif
#ifdef PVWIN32
        fprintf(fp,"customlogo=%%PVBDIR%%\\gamsleiten.png\n");
#endif
#ifdef __VMS
        fprintf(fp,"customlogo=[ProcessView.processview.processview]gamsleiten.png\n");
#endif
        fprintf(fp,"# howto start \"New Window\", can be commented out\n");
        fprintf(fp,"#newwindow=pvbrowser\n");
        fprintf(fp,"# windows users may put their putty.exe here\n");
        fprintf(fp,"ssh=ssh\n");
        fprintf(fp,"# we will automatically connect to this node\n");
#ifdef USE_MAEMO
        fprintf(fp,"initialhost=pv://pvbrowser.org\n");
#else
        fprintf(fp,"initialhost=pv://localhost\n");
#endif
        fprintf(fp,"connect_timeout=3       # in seconds (if 0 then use standard timeout)\n");
        fprintf(fp,"# define the programs that handle the following file formats\n");
#ifdef  PVWIN32
        fprintf(fp,"view.pdf=\"c:\\path\\to\\acroread\"\n");
        fprintf(fp,"view.img=mspaint\n");
        fprintf(fp,"view.svg=\"c:\\path\\to\\inkscape\"\n");
        fprintf(fp,"view.txt=\"c:\\windows\\system32\\write\"\n");
        fprintf(fp,"view.csv=\"c:\\path\\to\\ooffice\"\n");
        fprintf(fp,"view.html=\"c:\\path\\to\\firefox\"\n");
        fprintf(fp,"view.audio=\"c:\\path\\to\\vlc\"\n");
        fprintf(fp,"view.video=\"c:\\path\\to\\vlc\"\n");
        fprintf(fp,"#pvb_com_plugin=c:\\path\\to\\pvb_com_plugin.dll\n");
        fprintf(fp,"pvb_widget_plugindir=%%PVBDIR%%\\pvb\\pvbrowser\n");
#elif   defined(PVMAC)
        fprintf(fp,"view.pdf=okular\n");
        fprintf(fp,"view.img=gimp\n");
        fprintf(fp,"view.svg=inkscape\n");
        fprintf(fp,"view.txt=kwrite\n");
        fprintf(fp,"view.csv=ooffice\n");
        fprintf(fp,"view.html=firefox\n");
        fprintf(fp,"view.audio=vlc\n");
        fprintf(fp,"view.video=vlc\n");
        fprintf(fp,"#pvb_com_plugin=/path/to/pvb_com_plugin.dylib\n");
        fprintf(fp,"pvb_widget_plugindir=/opt/pvb/pvbrowser\n");
#elif   defined(PVUNIX)
        fprintf(fp,"view.pdf=okular\n");
        fprintf(fp,"view.img=gimp\n");
        fprintf(fp,"view.svg=inkscape\n");
        fprintf(fp,"view.txt=kwrite\n");
        fprintf(fp,"view.csv=ooffice\n");
        fprintf(fp,"view.html=firefox\n");
        fprintf(fp,"view.audio=vlc\n");
        fprintf(fp,"view.video=vlc\n");
#ifdef USE_SYMBIAN
        fprintf(fp,"#pvb_com_plugin=/path/to/pvb_com_plugin.so\n");
        fprintf(fp,"pvb_widget_plugindir=\\sys\\bin\n");
#elif  defined(USE_ANDROID)        
        fprintf(fp,"#pvb_com_plugin=/path/to/pvb_com_plugin.so\n");
        fprintf(fp,"pvb_widget_plugindir=%s/pvbrowser\n", writeableLocation());
#else
        fprintf(fp,"#pvb_com_plugin=/path/to/pvb_com_plugin.so\n");
        fprintf(fp,"pvb_widget_plugindir=/opt/pvb/pvbrowser\n");
#endif
#endif
        fprintf(fp,"generate_cookie=pvb_generate_cookie\n");
        fprintf(fp,"proxyadr=localhost      # ip adr of proxy that might encrypt the network traffic\n");
        fprintf(fp,"proxyport=-1            # -1 if proxy is not used\n");

        fprintf(fp,"##################################################################\n");
        fprintf(fp,"#\n");
        fprintf(fp,"# Language settings for ProcessViewBrowser\n");
        fprintf(fp,"# you may input your own language (I only know german and english)\n");
        fprintf(fp,"# example: comment out: language = german\n");
        fprintf(fp,"#          comment in:  #language = default\n");
        fprintf(fp,"#          restart ProcessViewBrowser\n");
        fprintf(fp,"##################################################################\n");
        fprintf(fp,"language = default\n");
        fprintf(fp,"#language = german\n");
        fprintf(fp,"#language = french\n");
        fprintf(fp,"#language = spanish\n");
        fprintf(fp,"#language = italian\n");
        fprintf(fp,"#language = chinese\n");
        fprintf(fp,"#---------------------------------------\n");
        fprintf(fp,"german {\n");
        fprintf(fp,"-file               = &Datei\n");
        fprintf(fp,"-options            = &Optionen\n");
        fprintf(fp,"-new_window         = &Neues Fenster\n");
        fprintf(fp,"-new_tab            = Neuer &Tab\n");
        fprintf(fp,"-delete_tab         = Schliesst den aktuellen Tab\n");
        fprintf(fp,"-reconnect          = &Reconnect\n");
        fprintf(fp,"-save_as_bmp        = &Speichere als BMP ...\n");
        fprintf(fp,"-log_as_bmp         = Logge Metafiles als &BMP ...\n");
        fprintf(fp,"-log_as_pvm         = Logge Metafiles als P&VM ...\n");
        fprintf(fp,"-print              = &Drucken\n");
        fprintf(fp,"-exit               = Schli&essen\n");
        fprintf(fp,"-edit               = &Bearbeiten\n");
        fprintf(fp,"-copy               = &Kopieren\n");
        fprintf(fp,"-copy_plus_title    = Ko&pieren mit Titel\n");
        fprintf(fp,"-view               = &Ansicht\n");
        fprintf(fp,"-help               = &Hilfe\n");
        fprintf(fp,"-toolbar            = &Werkzeugleiste\n");
        fprintf(fp,"-statusbar          = &Statuszeile\n");
        fprintf(fp,"-maximized          = &Maximiert\n");
        fprintf(fp,"-fullscreen         = &Vollbildmodus\n");
        fprintf(fp,"-Manual             = &Manual\n");
        fprintf(fp,"-about              = &About ...\n");
        fprintf(fp,"-status_connection_lost    = Verbindung zu Server verloren, CTRL-R fuer erneute Verbindung\n");
        fprintf(fp,"-status_connected          = Verbunden mit Server\n");
        fprintf(fp,"-status_could_not_connect  = Konnte nicht mit Server verbinden, CTRL-R fuer erneute Verbindung\n");
        fprintf(fp,"-status_reconnect          = Erneut mit host verbinden\n");
        fprintf(fp,"-status_options            = Ansehen/Ändern der ProcessViewBrowser Optionen\n");
        fprintf(fp,"-status_new_window         = Öffnet ein neues ProcessViewBrowser Fenster\n");
        fprintf(fp,"-status_new_tab            = Öffnet einen neuen Tab\n");
        fprintf(fp,"-status_save_as_bmp        = Speichert den aktuellen Bildschirm als BMP Datei\n");
        fprintf(fp,"-status_log_as_bmp         = Logge QDrawWidgets als BMP Datei\n");
        fprintf(fp,"-status_log_as_pvm         = Logge QDrawWidgets als PVM Datei\n");
        fprintf(fp,"-status_print              = Druckt den aktuellen Bildschirm\n");
        fprintf(fp,"-status_exit               = Schliesst pvbrowser\n");
        fprintf(fp,"-status_copy               = Kopiere den aktuellen Bildschirm in die Zwischenablage\n");
        fprintf(fp,"-status_toolbar            = Werkzeugleiste (an/aus)\n");
        fprintf(fp,"-status_statusbar          = Statuszeile (an/aus)\n");
        fprintf(fp,"-status_toggle_full_screen = Vollbildmodus (an/aus)\n");
        fprintf(fp,"-status_manual             = Anwendungsspezifische Hilfe\n");
        fprintf(fp,"-status_about              = Aboutbox anzeigen\n");
        fprintf(fp,"-print_header              = Gedruckt von pvbrowser am: \n");
        fprintf(fp,"}\n");
        fprintf(fp,"#---------------------------------------\n");
        fprintf(fp,"french {\n");
        fprintf(fp,"-file               = Fichier\n");
        fprintf(fp,"-options            = Option\n");
        fprintf(fp,"-new_window         = Nouvelle Fenêtre\n");
        fprintf(fp,"-new_tab            = Nouvelle &Tab\n");
        fprintf(fp,"-reconnect          = Reconnection\n");
        fprintf(fp,"-save_as_bmp        = Sauvegarder en BMP ...\n");
        fprintf(fp,"-log_as_bmp         = Enregistrer en BMP ...\n");
        fprintf(fp,"-log_as_pvm         = Enregistrer en PVM ...\n");
        fprintf(fp,"-print              = Imprimer\n");
        fprintf(fp,"-exit               = Sortir\n");
        fprintf(fp,"-edit               = éditer\n");
        fprintf(fp,"-copy               = Copier\n");
        fprintf(fp,"-copy_plus_title    = Copier + title\n");
        fprintf(fp,"-view               = Voir\n");
        fprintf(fp,"-toolbar            = Bare d'outil\n");
        fprintf(fp,"-statusbar          = Bare d'état\n");
        fprintf(fp,"-toggle_full_screen = Permuter la grandeur de fenêtre\n");
        fprintf(fp,"-help               = Aide\n");
        fprintf(fp,"-booklet            = Livret\n");
        fprintf(fp,"-manual             = Manuel\n");
        fprintf(fp,"-about              = à propos\n");
        fprintf(fp,"-recent_urls        = Dernier sites visité\n");
        fprintf(fp,"-status_connection_lost   = Connexion perdu, Faire CTRL-R pour reconnecter en local\n");
        fprintf(fp,"-status_connected         = Connexion\n");
        fprintf(fp,"-status_could_not_connect = Ne peut pas connecter, faire CTRL-R pour reconnecter local\n");
        fprintf(fp,"-status_options     = Options\n");
        fprintf(fp,"-status_new_window  = Nouvelle fenêtre\n");
        fprintf(fp,"-status_save_as_bmp = Sauvegarder au BMP\n");
        fprintf(fp,"-status_log_as_bmp  = QDrawWidgets est enregistrer au BMP\n");
        fprintf(fp,"-status_log_as_pvm  = QDrawWidgets est enregistrer au PVM\n");
        fprintf(fp,"-status_print       = Imprimer\n");
        fprintf(fp,"-status_exit        = Sortie de ProcessViewBrowser\n");
        fprintf(fp,"-status_copy        = Copie\n");
        fprintf(fp,"-status_toolbar     = Bare d'outil\n");
        fprintf(fp,"-status_statusbar   = bare d'état\n");
        fprintf(fp,"-status_toggle_full_screen = Permuter mode pleine écran\n");
        fprintf(fp,"-status_booklet     = Livret sur ProcessViewServer\n");
        fprintf(fp,"-status_manual      = Documentation de ProcessViewServer\n");
        fprintf(fp,"-status_about       = à propos\n");
        fprintf(fp,"-print_header       = Imprimer avec pvbrowser\n");
        fprintf(fp,"}\n");
        fprintf(fp,"#---------------------------------------\n");
        fprintf(fp,"spanish {\n");
        fprintf(fp,"-file               = &Fichero\n");
        fprintf(fp,"-options            = &Opciones\n");
        fprintf(fp,"-new_window         = &Nueva Ventana\n");
        fprintf(fp,"-new_tab            = Nueva &Tab\n");
        fprintf(fp,"-reconnect          = &Reconectar\n");
        fprintf(fp,"-save_as_bmp        = &Guardar como BMP...\n");
        fprintf(fp,"-log_as_bmp         = Log Metafiles como &BMP...\n");
        fprintf(fp,"-log_as_pvm         = Log Metafiles como P&VM...\n");
        fprintf(fp,"-print              = &Imprimir\n");
        fprintf(fp,"-exit               = &Salir\n");
        fprintf(fp,"-edit               = &Editar\n");
        fprintf(fp,"-copy               = &Copiar\n");
        fprintf(fp,"-copy_plus_title    = Co&piar + title\n");
        fprintf(fp,"-view               = &Ver\n");
        fprintf(fp,"-toolbar            = &Barra Herramientas\n");
        fprintf(fp,"-statusbar          = &Barra Estado\n");
        fprintf(fp,"-toggle_full_screen = Pantalla Completa (On/Off)\n");
        fprintf(fp,"-help               = &Ayuda\n");
        fprintf(fp,"-booklet            = &Manuales\n");
        fprintf(fp,"-manual             = &Documentación...\n");
        fprintf(fp,"-about              = &Acerca de...\n");
        fprintf(fp,"-recent_urls        = URLs Recientes\n");
        fprintf(fp,"-status_connection_lost   = Conexión al Servidor perdida, CTRL-R para reconecta\n");
        fprintf(fp,"-status_connected         = Conectado al Servidor\n");
        fprintf(fp,"-status_reconnect   = Reconectar al Servidor\n");
        fprintf(fp,"-status_could_not_connect = No se pudo conectar al Servidor, CTRL-R para reconectar\n");
        fprintf(fp,"-status_options     = Ver/Cambiar las Opciones ProcessViewBrowser\n");
        fprintf(fp,"-status_new_window  = Abrir nueva ventana de ProcessViewBrowser\n");
        fprintf(fp,"-status_save_as_bmp = Guardar pantalla actual como fichero BMP\n");
        fprintf(fp,"-status_log_as_bmp  = Logge QDrawWidgets como fichero BMP\n");
        fprintf(fp,"-status_log_as_pvm  = Logge QDrawWidgets como fichero PVM\n");
        fprintf(fp,"-status_print       = Imprimir la pantalla actual\n");
        fprintf(fp,"-status_exit        = Salir de ProcessViewBrowser\n");
        fprintf(fp,"-status_copy        = Copiar la pantalla al Portapapeles\n");
        fprintf(fp,"-status_toolbar     = Barra de Herramientas (On/Off)\n");
        fprintf(fp,"-status_statusbar   = Barra de Estado (On/Off)\n");
        fprintf(fp,"-status_toggle_full_screen = Pantalla Completa (On/Off)\n");
        fprintf(fp,"-status_booklet     = Mostrar ProcessViewServer Manual\n");
        fprintf(fp,"-status_manual      = Mostrar ProcessViewServer Documentación\n");
        fprintf(fp,"-status_about       = Acerca de...\n");
        fprintf(fp,"-print_header       = Impreso desde ProcessViewBrowser\n");
        fprintf(fp,"}\n");
        fprintf(fp,"#---------------------------------------\n");
        fprintf(fp,"italian {\n");
        fprintf(fp,"-file               = &File\n");
        fprintf(fp,"-options            = &Opzioni\n");
        fprintf(fp,"-new_window         = &Nuova Finestra\n");
        fprintf(fp,"-new_tab            = Nuovo &Tab\n");
        fprintf(fp,"-reconnect          = &Riconnessione\n");
        fprintf(fp,"-save_as_bmp        = &Salva come BMP...\n");
        fprintf(fp,"-log_as_bmp         = Log come &BMP...\n");
        fprintf(fp,"-log_as_pvm         = Log come P&VM...\n");
        fprintf(fp,"-print              = Sta&mpa\n");
        fprintf(fp,"-exit               = &Esci\n");
        fprintf(fp,"-edit               = &Modifica\n");
        fprintf(fp,"-copy               = &Copia\n");
        fprintf(fp,"-view               = &Visualizza\n");
        fprintf(fp,"-toolbar            = &Barra dei tool\n");
        fprintf(fp,"-maximized          = &Massimizza\n");
        fprintf(fp,"-statusbar          = &Barra di stato\n");
        fprintf(fp,"-toggle_full_screen = Schermo Completo (On/Off)\n");
        fprintf(fp,"-help               = &Aiuto\n");
        fprintf(fp,"-booklet            = &Manuali\n");
        fprintf(fp,"-manual             = &Documentazione...\n");
        fprintf(fp,"-about              = &Informazioni su...\n");
        fprintf(fp,"-recent_urls        = URLs Recenti\n");
        fprintf(fp,"-status_connection_lost   = Connessione col Server persa, CTRL-R per riconnettere\n");
        fprintf(fp,"-status_connected         = Connesso al Server\n");
        fprintf(fp,"-status_reconnect         = Reconnettere col Server\n");
        fprintf(fp,"-status_could_not_connect = Connessione col servere fallita, CTRL-R per riconnettere\n");
        fprintf(fp,"-status_options     = Vedere/Modificare le Opzioni di ProcessViewBrowser\n");
        fprintf(fp,"-status_new_window  = Aprire nuova finestra di ProcessViewBrowser\n");
        fprintf(fp,"-status_save_as_bmp = Salvare la schermata attuale come file BMP\n");
        fprintf(fp,"-status_log_as_bmp  = Registrando QDrawWidgets come file BMP\n");
        fprintf(fp,"-status_log_as_pvm  = Registrando QDrawWidgets come file PVM\n");
        fprintf(fp,"-status_print       = Stampare la schermata attuale\n");
        fprintf(fp,"-status_exit        = Uscita da ProcessViewBrowser\n");
        fprintf(fp,"-status_copy        = Copia\n");
        fprintf(fp,"-status_toolbar     = Barra dei Tools (On/Off)\n");
        fprintf(fp,"-status_statusbar   = Barra di Stato (On/Off)\n");
        fprintf(fp,"-status_toggle_full_screen = A Tutto schermo (On/Off)\n");
        fprintf(fp,"-status_booklet     = Visualizza il Manuale di ProcessViewServer\n");
        fprintf(fp,"-status_manual      = Visualizza la Documentazione di ProcessViewServer\n");
        fprintf(fp,"-status_about       = Informazioni su...\n");
        fprintf(fp,"-print_header       = Stampato da ProcessViewBrowser\n");
        fprintf(fp,"}\n");
        fprintf(fp,"\n");
        fprintf(fp,"chinese {\n");
        fprintf(fp,"-file               = &文件\n");
        fprintf(fp,"-options            = &选项\n");
        fprintf(fp,"-new_window         = &新建窗口\n");
        fprintf(fp,"-new_tab            = &新建标签\n");
        fprintf(fp,"-delete_tab         = 关闭当前标签\n");
        fprintf(fp,"-reconnect          = &重新连接\n");
        fprintf(fp,"-save_as_bmp        = &保存为BMP格式 ...\n");
        fprintf(fp,"-log_as_bmp         = &作为BMP图元文件记录 ...？\n");
        fprintf(fp,"-log_as_pvm         = &作为VMP图元文件记录 ...？\n");
        fprintf(fp,"-print              = &打印\n");
        fprintf(fp,"-exit               = &退出\n");
        fprintf(fp,"-edit               = &编辑\n");
        fprintf(fp,"-copy               = &复制\n");
        fprintf(fp,"-copy_plus_title    = &复制（加标题）\n");
        fprintf(fp,"-view               = &查看\n");
        fprintf(fp,"-help               = &帮助\n");
        fprintf(fp,"-toolbar            = &工具栏\n");
        fprintf(fp,"-statusbar          = &状态栏\n");
        fprintf(fp,"-maximized          = &最大化\n");
        fprintf(fp,"-fullscreen         = &全屏\n");
        fprintf(fp,"-Manual             = &帮助手册（手动？）\n");
        fprintf(fp,"-about              = &关于 ...\n");
        fprintf(fp,"-status_connection_lost    = 状态_连接服务器丢失，“CTRL-R”为重新连接\n");
        fprintf(fp,"-status_connected          = 状态_服务器已连接\n");
        fprintf(fp,"-status_could_not_connect  = 状态_无法连接到服务器，“CTRL-R”为重新连接\n");
        fprintf(fp,"-status_reconnect          = 状态_重新连接\n");
        fprintf(fp,"-status_options            = 状态_查看/更改浏览器选项\n");
        fprintf(fp,"-status_new_window         = 状态_打开一个新的浏览器窗口\n");
        fprintf(fp,"-status_new_tab            = 状态_打开一个新标签\n");
        fprintf(fp,"-status_save_as_bmp        = 状态_将当前屏幕保存为BMP文件\n");
        fprintf(fp,"-status_log_as_bmp         = 状态_作为BMP图元文件记录\n");
        fprintf(fp,"-status_log_as_pvm         = 状态_作为PVM图元文件记录\n");
        fprintf(fp,"-status_print              = 状态_打印当前屏幕\n");
        fprintf(fp,"-status_exit               = 状态_关闭pvbrowser\n");
        fprintf(fp,"-status_copy               = 状态_将当前屏幕复制到剪切板\n");
        fprintf(fp,"-status_toolbar            = 状态_工具栏(开/关)\n");
        fprintf(fp,"-status_statusbar          = 状态_状态栏（开/关）\n");
        fprintf(fp,"-status_toggle_full_screen = 状态_全屏模式（开/关）\n");
        fprintf(fp,"-status_manual             = 状态_特殊应用帮助\n");
        fprintf(fp,"-status_about              = 状态_显示\"关于\"\n");
        fprintf(fp,"-print_header              = 状态_打印标题\n");
        fprintf(fp,"}\n");
        fprintf(fp,"# other languages\n");
        fprintf(fp,"##################################################################\n");
        fprintf(fp,"#\n");
        fprintf(fp,"# End of Language settings for pvbrowser\n");
        fprintf(fp,"#\n");
        fprintf(fp,"##################################################################\n");
        fprintf(fp,"#\n");
        fprintf(fp,"# This is the list of recent nodes you have visited\n");
        fprintf(fp,"host=pv://localhost\n");
        fclose(fp);
      }
      else
      {
        return "Could not write initialization file";
      }
    }
  }
}

#ifdef PVUNIX
static int rlexec(const char *command)
{
  char *buf;
  char *arg[512];
  int i,iarg,ret;

  buf = new char [strlen(command)+1];
  strcpy(buf,command);
  iarg = 0;
  i = 0;
  while(buf[i] != '\0')
  {
    if(buf[i] == '\"')
    {
      i++;
      arg[iarg++] = &buf[i];
      while(buf[i] != '\"' && buf[i] != '\0') i++;
      if(buf[i] == '\0') break;
      buf[i] = '\0';
    }
    else if(buf[i] != ' ' || i == 0)
    {
      arg[iarg++] = &buf[i];
      while(buf[i] != ' ' && buf[i] != '\0') i++;
      if(buf[i] == '\0') break;
      buf[i] = '\0';
    }
    i++;
  }
  arg[iarg] = NULL;

  ret = execvp(arg[0],arg);
  delete [] buf;
  return ret;
}
#endif

#ifdef PVWIN32
static PROCESS_INFORMATION pi;
int winWaitpid()
{
  int ret;
  ret = WaitForSingleObject(pi.hProcess,0);
  return ret;
}
#endif

int mysystem(const char *command)
{
#ifdef PVWIN32
  int ret;
  STARTUPINFOA         si = { sizeof(si)};
  //PROCESS_INFORMATION pi;
  char cmd[4096];

  if(strncmp(command,"start",5) == 0 || strncmp(command,"START",5) == 0)
  {
    ExpandEnvironmentStringsA(command,cmd,sizeof(cmd)-1);
    ret = system(cmd);
  }
  else
  {
    ExpandEnvironmentStringsA(command,cmd,sizeof(cmd)-1);
    ret = (int) CreateProcessA( NULL, cmd
                             , NULL, NULL
                             , FALSE, CREATE_NO_WINDOW
                             , NULL, NULL
                             , &si, &pi);
  }
  return ret;
#else
  if(opt.arg_debug) printf("run client command: %s\n", command);
  //return system(command);
  pid_t pid = fork();
  if(pid == -1)
  {
    printf("Fork failed %s\n", command);
  }
  else if(pid == 0)
  {
    rlexec(command);
  }
  return pid;
#endif
}
