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
#ifndef OPT_H
#define OPT_H

#ifndef MAX_PRINTF_LENGTH
#define MAX_PRINTF_LENGTH 1024+16 // must be bigger than or equal to MAX_PRINTF_LENGTH of ProcessViewServer
#endif
#define MAXOPT 1024

enum
{
  pvbUTF8 = 0,
  pvbNone
};

typedef struct
{
  // inifile args
  char manual[MAXOPT];      // index.html
  int port;                 //=5050   # default port
  int sshport;              //=50500  # local port for ssh connections
  int zoom;                 //=100    # zoom factor in %
  int fontzoom;             //=100    # zoom factor for fonts in %
  int appfontsize;          //=0      # size of the application font. If appfontsize == 0 then use default font.
  int autoreconnect;        //=0      # 0|1
  int exitpassword;         //=0      # 0|1
  int menubar;              //=1      # 0|1
  int toolbar;              //=1      # 0|1
  int statusbar;            //=1      # 0|1
  int scrollbars;           //=1      # 0|1
  int fullscreen;           //=0      # 0|1
  int maximized;            //=0      # 0|1
  int tabs_above_toolbar;   //=0      # 0|1
  int cursor_shape;         //=
  int echo_table_updates;   //=0      # 0|1
  int use_webkit_for_svg;   //=1    # 0|1
  int enable_webkit_plugins;//=0      # 0|1
  char temp[MAXOPT];        //=/tmp
  char customlogo[MAXOPT];  //=/home/lehrig/cc/joschi/custom.bmp
  char newwindow[MAXOPT];   //=pvbrowser
  char ssh[MAXOPT];         //=ssh
  char initialhost[MAXOPT]; //=pv://localhost
  char language[MAXOPT];    // = german
  char language_section[MAXOPT];
  int  codec;               // strongly advised to use pvbUTF8
  int  closed;
  int  cookies;             // 0=NO 1=YES 2=ASK
  int  i_have_started_servers; // count number of start commands
  char view_audio[MAXOPT];  // vlc
  char view_video[MAXOPT];  // vlc
  char view_pdf[MAXOPT];    // okular
  char view_img[MAXOPT];    // gimp
  char view_svg[MAXOPT];    // inkscape
  char view_txt[MAXOPT];    // kwrite
  char view_csv[MAXOPT];    // ooffice
  char view_html[MAXOPT];   // firefox
  char generate_cookie[MAXOPT]; // generate a cookie to give you permission to login to the pvserver
  char pvb_com_plugin[MAXOPT];  // pvb communication plugin
  char pvb_widget_plugindir[MAXOPT]; // directory for custom widget plugins
  int  connect_timeout;     // connect() timeout in seconds if 0 then standard timeout
  char initial_dir[MAXOPT]; // getcwd at startup
  int  ffmpeg_available;    // ffmpeg is in PATH
  int  ffplay_available;    // ffplay is in PATH
  char proxyadr[MAXOPT];    // default: localhost
  int  proxyport;           // default: -1 # which means proxy not used
  int  replace_svg_symbol_by_g; // currently always on, because QSvgRenderer does not handle the symbol element, This might become an option in the future

  // command line args
  int  arg_debug;
  int  arg_localini;
  char arg_ini[MAXOPT];
  char arg_font[MAXOPT];
  char arg_host[MAXOPT];
  int  arg_disable;
  int  arg_x;
  int  arg_y;
  int  arg_w;
  int  arg_h;
  int  arg_log;
  int  arg_fillbackground;  // murnleitner special
  const char *arg_av0;
}OPT;

int wsa();
const char *inifile();
const char *passfile();
const char *pvpass(const char *p);
void setDefaultOptions();
const char *readIniFile();
int winWaitpid();
int mysystem(const char *command);

#endif
