/***************************************************************************
                          processviewserver.h  -  description
                             -------------------
    begin                : Son Nov 12 09:43:38 CET 2000
    copyright            : (C) 2000 by R. Lehrig
                         : Angel Maza
                         : Martin Bangieff
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

/* Definitions (Events, Fonts, Colors ...) */

#ifndef PROCESSVIEWSERVER_H
#define PROCESSVIEWSERVER_H

const char pvserver_version[] = "5.15.3";

// define WIN
#ifdef _WIN32
#define PVWIN32
#include <winsock2.h>
#include <windows.h>
#define RL_RTLD_LAZY 0
#endif

//*** define platform *************************
// define unix if not already defined
#ifdef __unix__
#ifndef unix
#define unix
#endif
#endif
#ifdef unix
#define PVUNIX
#include <dlfcn.h>
#define RL_RTLD_LAZY RTLD_LAZY
#endif
//*********************************************

#include <stdio.h>
#include <string.h>
/*
#ifdef __VMS
#include "vmsgl.h"
#endif
#ifdef _WIN32
#include "vmsgl.h"
#endif
#ifdef unix
//#include <GL/gl.h>
#include "vmsgl.h"
#endif
*/
#include "vmsgl.h"

#ifndef USE_INETD
#include "wthread.h"
#endif

/** @defgroup DefinesAndEnums Defines and Enums
 *  These are the Defines and Enums
 *  @{ */
/** #define USE_INETD // comment this out if you want to use inetd instead of a multithreaded server
*/

#define pv_STDIN  0
#define pv_STDOUT 1
//#define pv_CACHE                 /* do file chaching, comment out if not wanted */

#define MAX_PRINTF_LENGTH 1024   /*! max length of pvPrintf buffer (must be less than or equal to MAX_PRINTF_LENGTH of ProcessViewBrowser) */
#define MAX_EVENT_LENGTH  1024   /*! max length of an event        */
#define MAX_CLIENTS       100    /*! max number of clients         */

enum MainWindowIds {
ID_ROOTWIDGET   = 0,      /// # pvHide(p,ID_EDITBAR) pvShow(p,ID_EDITBAR)
ID_EDITBAR      = -1,     /// # pvHide(p,ID_TOOLBAR) pvShow(p,ID_TOOLBAR)
ID_TOOLBAR      = -2,     /// # pvHide(p,ID_STATUSBAR) pvShow(p,ID_STATUSBAR)
ID_STATUSBAR    = -3,     /// # pvResize(p,ID_MAINWINDOW,width,height)
ID_MAINWINDOW   = -4,     /// # pvHide(p,ID_HELP) pvShow(p,ID_HELP)
ID_HELP         = -5,     /// # pvPrintf(p,ID_COOKIE,"%s=%s",cookie_name,cookie_values) pvPrintf(p,ID_COOKIE,cookie_name)
ID_COOKIE       = -6,     /// # pvPrintf(p,ID_TAB,"%s", "title");
ID_TAB          = -7,     /// # pvText(p,ID_OPTIONS); 
ID_OPTIONS      = -8,     
ID_DOCK_WIDGETS = -1000   /// # leave some space for dock widgets
};

#define MAX_DOCK_WIDGETS 32

/* these are the possible events */
enum PvEvent {
NULL_EVENT=1,
BUTTON_EVENT,
TEXT_EVENT,
SLIDER_EVENT,
CHECKBOX_EVENT,
RADIOBUTTON_EVENT,
GL_IDLE_EVENT,
GL_PAINT_EVENT,
GL_INITIALIZE_EVENT,
GL_RESIZE_EVENT,
TAB_EVENT,
TABLE_CLICKED_EVENT,
TABLE_TEXT_EVENT,
SELECTION_EVENT,
CLIPBOARD_EVENT,
BUTTON_PRESSED_EVENT,
BUTTON_RELEASED_EVENT,
RIGHT_MOUSE_EVENT,
KEYBOARD_EVENT,
PLOT_MOUSE_MOVED_EVENT,
PLOT_MOUSE_PRESSED_EVENT,
PLOT_MOUSE_RELEASED_EVENT,
USER_EVENT,
MOUSE_OVER_EVENT
};

/* these are the supported widget types */
enum
{
TQWidget = 0,
TQPushButton,
TQLabel,
TQLineEdit,
TQComboBox,
TQLCDNumber,
TQButtonGroup,
TQRadio,
TQCheck,
TQSlider,
TQFrame,
TQImage,
TQDraw,
TQGl,
TQTabWidget,
TQGroupBox,
TQListBox,
TQTable,
TQSpinBox,
TQDial,
TQProgressBar,
TQMultiLineEdit,
TQTextBrowser,
TQListView,
TQIconView,
TQVtk,
TQwtPlotWidget,
TQwtScale,
TQwtThermo,
TQwtKnob,
TQwtCounter,
TQwtWheel,
TQwtSlider,
TQwtDial,
TQwtCompass,
TQwtAnalogClock,
TQDateEdit,
TQTimeEdit,
TQDateTimeEdit,
TQToolBox,
TQVbox,
TQHbox,
TQGrid,
TQCustomWidget
};

/* these are the linestyles used in line(x,y,n) */
enum Linestyle {
LINESTYLE_NONE=0,
LINESTYLE_CIRCLE,
LINESTYLE_CROSS,
LINESTYLE_RECT
};

/* these are the available fonts */
#define HELVETICA  "Helvetica"
#define TIMES      "Times"
#define COURIER    "Courier"
#define OLDENGLISH "OldEnglish"
#define SYSTEM     "System"
#define ANYSTYLE   "AnyStyle"

/* font weight */
enum Weight { Light = 25, Normal = 50, DemiBold = 63, Bold = 75, Black = 87 };

/* font alignment */
enum FontAlignment {     /*                  */
ALIGN_LEFT=0,            /*  example         */
ALIGN_CENTER,            /*      example     */
ALIGN_RIGHT,             /*          example */
ALIGN_VERT_CENTER        /*      e           */
};                       /*      x           */
                         /*      a           */
                         /*      m           */
                         /*      p           */
                         /*      l           */
                         /*      e           */

// alignment used in QLabel, QLineEdit 
enum AlignmentFlags {
  AlignAuto   = 0x0000,   // text alignment
  AlignLeft   = 0x0001,
  AlignRight    = 0x0002,
  AlignHCenter    = 0x0004,
  AlignJustify    = 0x0008,
  AlignHorizontal_Mask  = AlignLeft | AlignRight | AlignHCenter | AlignJustify,
  AlignTop    = 0x0010,
  AlignBottom   = 0x0020,
  AlignVCenter    = 0x0040,
  AlignVertical_Mask  = AlignTop | AlignBottom | AlignVCenter,
  AlignCenter   = AlignVCenter | AlignHCenter
};
    
enum TextFlags {
  SingleLine    = 0x0080, // misc. flags
  DontClip      = 0x0100,
  ExpandTabs    = 0x0200,
  ShowPrefix    = 0x0400,
  WordBreak     = 0x0800,
  BreakAnywhere = 0x1000,
  DontPrint     = 0x2000,
  Underline     = 0x01000000,
  Overline      = 0x02000000,
  StrikeOut     = 0x04000000,
  IncludeTrailingSpaces = 0x08000000,
  NoAccel       = 0x4000
};

enum TextCursor {
  NoMove = 0,        // Keep the cursor where it is
  Start,             // Move to the start of the document.
  StartOfLine,       // Move to the start of the current line.
  StartOfBlock,      // Move to the start of the current block.
  StartOfWord,       // Move to the start of the current word.
  PreviousBlock,     // Move to the start of the previous block.
  PreviousCharacter, // Move to the previous character.
  PreviousWord,      // Move to the beginning of the previous word.
  Up,                // Move up one line.
  Left,              // Move left one character.
  WordLeft,          // Move left one word.
  End,               // Move to the end of the document.
  EndOfLine,         // Move to the end of the current line.
  EndOfWord,         // Move to the end of the current word.
  EndOfBlock,        // Move to the end of the current block.
  NextBlock,         // Move to the beginning of the next block.
  NextCharacter,     // Move to the next character.
  NextWord,          // Move to the next word.
  Down,              // Move down one line.
  Right,             // Move right one character.
  WordRight
};

/* font italic can be one of this */
enum FontNormalItalic
{
  NORMAL = 0,
  ITALIC = 1
};

/* insertion policies for QComboBox */
enum Policy { NoInsertion=0, AtTop, AtCurrent, AtBottom, AfterCurrent, BeforeCurrent };

/* some predefined colors */
#define RED         255,0,0
#define GREEN       0,255,0
#define BLUE        0,0,255
#define WHITE       255,255,255
#define BLACK       0,0,0
#define YELLOW      255,255,0
#define LILA        255,0,255
#define CYAN        0,255,255
#define DARK_GREY   128,128,128
#define LIGHT_GREY  180,180,180
#define WHEAT       213,213,154
#define DARK_RED    128,0,0
#define DARK_GREEN  0,128,0
#define DARK_LILA   128,0,128
#define DARK_CYAN   0,128,128
#define DARK_YELLOW 200,200,0
#define DARK_BLUE   0,0,128

/* definition for LCD numbers */
enum Mode         { HEX=0, DEC, OCT, BINx };
enum Mode2        { Hex=0, Dec, Oct, Bin };
enum SegmentStyle { Outline=0, Filled, Flat };

/* definitions for QSlider */
enum ORIENTATION
{
  HORIZONTAL = 0,
  VERTICAL   = 1
  //jose HORIZONTAL = 1,
  //jose VERTICAL   = 2
};

enum Orientation
{
  Horizontal = HORIZONTAL,
  Vertical   = VERTICAL
};

/* definitions for MouseShape */
enum MouseShape {
  ArrowCursor          = 0, //   The standard arrow cursor.
  UpArrowCursor        = 1, //   An arrow pointing upwards toward the top of the screen.
  CrossCursor          = 2, //  A crosshair cursor,
                          //typically used to help the user accurately select a point on the screen.
  WaitCursor           = 3, // An hourglass or watch cursor,
                          // usually shown during operations that prevent the user
                          // from interacting with the application.
  IBeamCursor          = 4, // A caret or ibeam cursor, indicating that a widget can accept and display text input.
  SizeVerCursor        = 5, // A cursor used for elements that are used to vertically resize top-level windows.
  SizeHorCursor        = 6, // A cursor used for elements that are used to horizontally resize top-level windows.
  SizeFDiagCursor      = 8, // A cursor used for elements that are used to diagonally resize top-level
                          // windows at their top-left and bottom-right corners.
  SizeBDiagCursor      = 7, // A cursor used for elements that are used to diagonally resize top-level
                          // windows at their top-right and bottom-left corners.
  SizeAllCursor        = 9, // A cursor used for elements that are used to resize top-level windows in any direction.
  BlankCursor         = 10, // A blank/invisible cursor, typically used when the cursor shape needs to be hidden.
  SplitVCursor        = 11, // A cursor used for vertical splitters, indicating that a handle can be
                          // dragged horizontally to adjust the use of available space.
  SplitHCursor        = 12, // A cursor used for horizontal splitters, indicating that a handle can be
                          // dragged vertically to adjust the use of available space.
  PointingHandCursor  = 13, // A pointing hand cursor that is typically used for clickable elements
                          // such as hyperlinks.
  ForbiddenCursor     = 14, // A slashed circle cursor, typically used during drag and drop operations to
                          // indicate that dragged content cannot be dropped on particular widgets
                          // or inside certain regions.
  OpenHandCursor      = 17, // A cursor representing an open hand, typically used to indicate that the area
                          // under the cursor is the visible part of a canvas that the user
                          // can click and drag in order to scroll around.
  ClosedHandCursor    = 18, // A cursor representing an open hand, typically used to indicate that a
                          // dragging operation is in progress that involves scrolling.
  WhatsThisCursor     = 15, // An arrow with a question mark, typically used
  BusyCursor          = 16
};

/* definitions for QFrame */
enum Shape {
NoFrame  = 0,                          /* no frame */
Box      = 0x0001,                     /* rectangular box */
Panel    = 0x0002,                     /* rectangular panel */
WinPanel = 0x0003,                     /* rectangular panel (Windows) */
HLine    = 0x0004,                     /* horizontal line */
VLine    = 0x0005,                     /* vertical line */
StyledPanel    = 0x0006,               /* rectangular panel depending on the GUI style */
PopupPanel     = 0x0007,               /* rectangular panel depending on the GUI style */
MenuBarPanel   = 0x0008,
ToolBarPanel   = 0x0009,
LineEditPanel  = 0x000a,
TabWidgetPanel = 0x000b,
GroupBoxPanel  = 0x000c,
MShape         = 0x000f               /* mask for the shape */
};

enum Shadow{
Plain    = 0x0010,                    /* plain line */
Raised   = 0x0020,                    /* raised shadow effect */
Sunken   = 0x0030,                    /* sunken shadow effect */
MShadow  = 0x00f0 };                  /* mask for the shadow */

enum FileDialogs{
FileOpenDialog = 0,
FileSaveDialog,
FindDirectoryDialog};

enum MessageBoxTypes{
BoxInformation = 0,
BoxWarning,
BoxCritical};

enum MessageBoxButtons{
MessageBoxOk              = 0x00000400,
MessageBoxOpen            = 0x00002000,
MessageBoxSave            = 0x00000800,
MessageBoxCancel          = 0x00400000,
MessageBoxClose           = 0x00200000,
MessageBoxDiscard         = 0x00800000,
MessageBoxApply           = 0x02000000,
MessageBoxReset           = 0x04000000,
MessageBoxRestoreDefaults = 0x08000000,
MessageBoxHelp            = 0x01000000,
MessageBoxSaveAll         = 0x00001000,
MessageBoxYes             = 0x00004000,
MessageBoxYesToAll        = 0x00008000,
MessageBoxNo              = 0x00010000,
MessageBoxNoToAll         = 0x00020000,
MessageBoxAbort           = 0x00040000,
MessageBoxRetry           = 0x00080000,
MessageBoxIgnore          = 0x00100000,
MessageBoxNoButton
};

enum TextBrowserPos{
Home = 0,
Forward,
Backward,
Reload};

enum TabWidgetPos{
Top = 0,
Bottom};

enum KeyboardModifiers{
ShiftButton   = 4,
ControlButton = 3,
AltButton     = 2,
NormalKey     = 1
};

enum KeyCodes{
Key_Escape = 0x1000000,
Key_Pause = 0x1000008,
Key_Print = 0x1000009,
Key_SysReq = 0x100000a,
Key_PageUp = 0x1000016,
Key_PageDown = 0x1000017,
Key_F1 = 0x1000030,
Key_F2 = 0x1000031,
Key_F3 = 0x1000032,
Key_F4 = 0x1000033,
Key_F5 = 0x1000034,
Key_F6 = 0x1000035,
Key_F7 = 0x1000036,
Key_F8 = 0x1000037,
Key_F9 = 0x1000038,
Key_F10 = 0x1000039,
Key_F11 = 0x100003a,
Key_F12 = 0x100003b
};

enum QpwLegend      { BottomLegend = 0, TopLegend, LeftLegend, RightLegend};
enum QwtAxis        { yLeft, yRight, xBottom, xTop, axisCnt };
enum QwtAutoscale   { pvNone = 0, IncludeRef = 1, Symmetric = 2, Floating = 4, Logarithmic = 8, Inverted = 16 };
//rlmurx-was-here enum ScalePosition  { ScaleLeft, ScaleRight, ScaleTop, ScaleBottom };
enum ScalePosition  { ScaleNone, ScaleLeft, ScaleRight, ScaleTop, ScaleBottom };
enum ThermoPosition { ThermoNone, ThermoLeft, ThermoRight, ThermoTop, ThermoBottom};
enum KnobSymbol     { KnobLine, KnobDot };
enum CounterButton  { CounterButton1, CounterButton2, CounterButton3, CounterButtonCnt };
enum SliderScalePos { SliderNone, SliderLeft, SliderRight, SliderTop, SliderBottom };
enum SliderBGSTYLE  { SliderBgTrough = 0x1, SliderBgSlot = 0x2, SliderBgBoth = SliderBgTrough | SliderBgSlot};
enum DialShadow     { DialPlain = Plain, DialRaised = Raised, DialSunken = Sunken };
enum DialMode       { RotateNeedle, RotateScale };
enum DialNeedle     { QwtDialNeedle1 = 1, QwtDialNeedle2, QwtDialNeedle3, QwtDialNeedle4, QwtDialLineNeedle, QwtDialArrowNeedle};
enum CompassNeedle  { QwtCompassNeedle1 = 1, QwtCompassNeedle2, QwtCompassNeedle3, QwtCompassNeedle4, QwtCompassLineNeedle };
enum PenStyle       { NoPen, SolidLine, DashLine, DotLine, DashDotLine, DashDotDotLine, MPenStyle = 0x0f };
enum MarkerSymbol   { MarkerNone, MarkerEllipse, MarkerRect, MarkerDiamond, MarkerTriangle, MarkerDTriangle, MarkerUTriangle, MarkerLTriangle, MarkerRTriangle, MarkerCross, MarkerXCross, MarkerStyleCnt };

enum ButtonClicked  { NoButton = 0, LeftButton, MiddleButton, RightButton };

enum Order          { DMY, MDY, YMD, YDM };

enum SetTextOption  { HTML_HEADER = 1, HTML_STYLE, HTML_BODY };

typedef struct
{
  int  event;
  int  i;
  char text[MAX_EVENT_LENGTH];
}
PARSE_EVENT_STRUCT;

/* thread parameters */
typedef struct _PARAM_
{
  int  s;                                /* socket                             */
  int  os;                               /* original socket                    */
  int  port;                             /* our port                           */
  int  language;                         /* language or DEFAULT_LANGUAGE       */
  int  convert_units;                    /* 1 if units must be converted       */
  FILE *fp;                              /* filepointer                        */
  int  sleep;                            /* sleep time in milliseconds         */
  int  (*cleanup)(void *);               /* cleanup for user code              */
  void *app_data;                        /* application data for cleanup       */
  void *user;                            /* pointer to user data               */
  char *clipboard;                       /* pointer to clipboard text | NULL   */
  long clipboard_length;                 /* sizeof clipboard contents          */
  int  modal;                            /* modal dialog                       */
  int  (*readData)(void *d);             /* modal dialog                       */
  int  (*showData)(_PARAM_ *p, void *d); /* modal dialog                       */
  void *modal_d;                         /* modal dialog                       */
  void *modalUserData;                   /* modal dialog                       */
  PARSE_EVENT_STRUCT parse_event_struct;
  float *x;                              /* array buffer for script language   */
  float *y;                              /* array buffer for script language   */
  int   nxy;                             /* number of elements in arry         */
  char  url[MAX_PRINTF_LENGTH];          /* url the client is using            */
  char  initial_mask[MAX_PRINTF_LENGTH]; /* initial mask user wants to see     */
  char  file_prefix[32];                 /* prefix for temporary files         */
                                         /* files with this prefix will be     */
                                         /* deleted on connection lost         */
  int   free;                            /* free structure                     */
  char  version[32];                     /* pvbrowser VERSION of client        */
  char  pvserver_version[32];            /* pvserver VERSION                   */
  int   exit_on_bind_error;              /* exit if we can not bind on port    */
  int   hello_counter;                   /* for thread timeout if no @hello    */
  int   local_milliseconds;              /* time of last call to select()      */
  int   force_null_event;                /* force null_event for better update */
                                         /* if the user has tabs within his    */
                                         /* client the invisible tab are       */
                                         /* paused by default                  */
  int   allow_pause;                     /* 0 not allowed else allowed         */
  int   pause;                           /* pause=1 if tab invisible else 0    */
                                         /* you can test pause in NULL_EVENT   */
  int   my_pvlock_count;                 /* used to avoid deadlock by repeated */
                                         /* call of pvlock                     */                                       
  int   num_additional_widgets;          /* additional widgets after           */
                                         /* ID_END_OF_WIDGETS                  */
  int   mouse_x,  mouse_y;               /* last mouse pos when pressed        */
  char  *mytext;                         /* buffer for internal use only       */
  const char *communication_plugin;      /* pointer to commandline arg or NULL */ 
  int   use_communication_plugin;        /* can also be set at runtime         */
  char  lang_section[32];                /* use pvSelectLanguage()             */
  char  *mytext2;                        /* temp used in language translation  */
  int   http;                            /* 0|1 talk http                      */
  FILE  *fptmp;                          /* temporary file pointer             */
  int   fhdltmp;                         /* temporary file handle              */
  int   iclientsocket;                   /* 0 <= iclientsockert < MAX_CLIENTS index into clientsocket[] */
  int   is_binary;                       /* 0 for text message 1 otherwise     */
  int   button;                          /* last clicked button                */
}PARAM;

#ifndef __VMS
typedef int (*plugin_pvAccept)(PARAM *p);
typedef int (*plugin_pvtcpsend_binary)(PARAM *p, const char *buf, int len);
typedef int (*plugin_pvtcpreceive)(PARAM *p, char *buf, int maxlen);
typedef int (*plugin_pvtcpreceive_binary)(PARAM *p, char *buf, int maxlen);
typedef int (*plugin_closesocket)(int s, PARAM *p);
#endif

#define DEFAULT_LANGUAGE 0

#ifndef pvtr
#define pvtr(txt) txt
#endif

enum UNIT_CONVERSION
{
   MM2INCH = 1,
   INCH2MM ,
   CM2FOOT ,
   FOOT2CM ,
   CM2YARD ,
   YARD2CM ,
   KM2MILE ,
   MILE2KM ,
   KM2NAUTICAL_MILE ,
   NAUTICAL_MILE2KM ,
   QMM2SQINCH ,
   SQINCH2QMM ,
   QCM2SQFOOT ,
   SQFOOT2QCM ,
   QM2SQYARD ,
   SQYARD2QM ,
   QM2ACRE ,
   ACRE2QM ,
   QKM2SQMILE ,
   SQMILE2QKM ,
   ML2TEASPOON ,
   TEASPOON2ML ,
   ML2TABLESPOON ,
   TABLESPOON2ML ,
   ML2OUNCE ,
   OUNCE2ML ,
   L2CUP ,
   CUP2L ,
   L2PINT ,
   PINT2L ,
   L2QUART ,
   QUART2L ,
   L2GALLON ,
   GALLON2L ,
   GR2OUNCE ,
   OUNCE2GR ,
   KG2POUND ,
   POUND2KG ,
   T2TON ,
   TON2T ,
   C2FAHRENHEIT ,
   FAHRENHEIT2C
};

static const char null_string[] = "";

enum TextEvents
{
  PLAIN_TEXT_EVENT = 0,
  SVG_LEFT_BUTTON_PRESSED,
  SVG_MIDDLE_BUTTON_PRESSED,
  SVG_RIGHT_BUTTON_PRESSED,
  SVG_LEFT_BUTTON_RELEASED,
  SVG_MIDDLE_BUTTON_RELEASED,
  SVG_RIGHT_BUTTON_RELEASED,
  SVG_BOUNDS_ON_ELEMENT,
  SVG_MATRIX_FOR_ELEMENT,
  WIDGET_GEOMETRY,
  PARENT_WIDGET_ID
};

typedef struct
{
  int i[10];
  int i0,i1,i2,i3,i4,i5,i6,i7,i8,i9;
}IntegerArray;

typedef struct
{
  float f[10];
  int f0,f1,f2,f3,f4,f5,f6,f7,f8,f9;
}FloatArray;

typedef struct
{
  int millisecond;
  int second;
  int minute;
  int hour;
  int day;
  int month;
  int year;
}pvTime;

typedef struct
{
  int s;                 // socket
  int version;           // ip version 4|6
  unsigned char adr[16]; // remote ip address
}pvAddressTableItem;

typedef struct
{
  pvAddressTableItem adr[MAX_CLIENTS];
}pvAddressTable;

/* this is for convenience when you want to write files */
#define PARAM_P  PARAM p;pvInit(0,NULL,&p);

/* wait with pvWait(INITIALIZE_GL) before you initialize you OpenGL scene */
#define INITIALIZE_GL "initializeGL"
#define RESIZE_GL     "resizeGL"

/** @} */ // end of group

/** @defgroup IniAndGlobal Initialisation and global routines
 *  These routines are used for initialisation. And also some global usable routines are available.
 *  @{ */
int glencode_set_param(PARAM *p);
/*! <pre>
If you access variables that are global to the server or
if you want to use malloc() and free()
you must surround the operations with
pvlock() and pvunlock()
because these operations are not thread save.
</pre> */
int pvlock(PARAM *p);
/*! <pre>
If you access variables that are global to the server or
if you want to use malloc() and free()
you must surround the operations with
pvlock() and pvunlock()
because these operations are not thread save.
</pre> */
int pvunlock(PARAM *p);
/*! <pre>
Same as system(command); but portable
</pre> */
int pvsystem(const char *command);
/*! <pre>
Get local time
</pre> */
void pvGetLocalTime(pvTime *pvtime);
/*! <pre>
Test if access is allowed by files "allow.ipv4" and "deny.ipv4" in your local directory
adr := dottet ip address
trace = 1 print messages on stdout
trace = 0 do not print messages on stdout
return = 1 access allowed
return = 0 access is not allowed

Example allow.ipv4:
1.0.0.127/32         # allow localhost
192.168.1.0/24       # allow 192.168.1.0 - 192.168.1.255
# insert more areas here

Example deny.ipv4:
# deny a individual address
192.168.2.14/32
# insert more areas here

The number behind the / is the number of significant bits of the ip address.
Every pvserver will evaluate "allow.ipv4 and "deny.ipv4" when client connects.
</pre> */
int pvIsAccessAllowed(const char *adr, int trace);
/*! <pre>
Send version of pvserver to client
</pre> */
int pvSendVersion(PARAM *p);
/*! <pre>
Allocate x,y array for script language
</pre> */
int pvXYAllocate(PARAM *p, int n);
/*! <pre>
Get integer array from string for script language
</pre> */
int getIntegers(const char *text, IntegerArray *ia);
/*! <pre>
Get integer array from string for script language
</pre> */
int getFloats(const char *text, FloatArray *fa);
/*! <pre>
Get text in parentesis from text for script language
</pre> */
const char *getTextFromText(const char *text);
/*! <pre>
Set x,y array for script language
</pre> */
int pvSetXY(PARAM *p, int i, float x, float y);
/*! <pre>
Get a pointer to the socket for script language
</pre> */
int *pvGetSocketPointer(PARAM *p);
/*! <pre>
see pvInit
Init for script languages
</pre> */
int pvInitInternal(PARAM *p);
/*! <pre>
(Test) pvInit must be called in main(). It interprets the command line switches that it knows.
Afterwards you can interpret your command line switches. It is possible to set p.user to data
of your choice. This data will be available in the worker threads. (Caution: the worker threads are
only allowed to read the p.user data because it is shared among all clients)
Then there must be a while(1) in which new clients are accepted. For each client a new thread
is created.

 int main(int ac, char **av)
 {
 PARAM p;
 int   s;

   pvInit(ac,av,&p);
   // here you may interpret ac,av and set p.user to your data
   while(1)
   {
     s = pvAccept(&p);
     if(s != -1) pvCreateThread(&p,s);
   }
   return 0;
 }
</pre> */
int pvInit(int ac, char **av, PARAM *p);
/*! <pre>
see pvInit
</pre> */
int pvAccept(PARAM *p);
/*! <pre>
see pvInit
</pre> */
int pvCreateThread(PARAM *p, int s);
/*! <pre>
Get the initial mask the user wants to see
p->initial_mask is a string identifying the initial mask 
</pre> */
int pvGetInitialMask(PARAM *p);
/*! <pre>
pvMain is your main worker thread. It could look as follows.
The main worker thread is never closed. It will be closed automatically when the client disconnects.

int pvMain(PARAM *p)
{
int ret;

  // here you can initialize your worker thread
  pvSetCleanup(p,your_exit_handler,your_app_data); // if cleanup is necessary
  pvResize(p,0,970,600);  // this will resize your working area
  ret = showMask1(p);
  while(1)
  {
    switch(ret)
    {
      case 1:
        ret = showMask1(p);
        break;
      case 2:
        ret = showMask2(p);
        break;
      case 3:
        ret = showMask3(p);
        break;
      default:
        return 0;
    }
  }
}
</pre> */
int pvMain(PARAM *p);
/*! <pre>
If it is necessary to cleanup your application when the main worker thread terminates
you can set an exit handler that receives the data in app_data.
Call this function in pvMain. See also pvMain.
</pre> */
int pvSetCleanup(PARAM *p, int (*cleanup)(void *), void *app_data);
/*! <pre>
If it is necessary to cleanup your application when the main worker thread terminates
you can set an exit handler that receives the data in app_data.
Call this function in pvMain. See also pvMain.
</pre> */
char *pvGetEvent(PARAM *p);
/*! <pre>
This function will return the next event as soon as it is available.
The maximum wait time is p->sleep in milliseconds (default 100).
You can specify a different wait time on the commandline (-sleep=1000)

Example:

 int showMask1(PARAM *p)
 {
 DATA d;
 char event[MAX_EVENT_LENGTH];
 int  i;
 char text[MAX_EVENT_LENGTH];

   defineMask1(p);
   readData1(&d); // from shared memory or out of database
   showData1(p,&d);
   while(1)
   {
     pvPollEvent(p,event);
     switch(pvParseEvent(event, &i, text))
     {
       case NULL_EVENT:
         readData1(&d); // from shared memory or out of database
         showData1(p,&d);
         break;
       case BUTTON_EVENT:
         ...
         break;
       case TEXT_EVENT:
         ...
         break;
       default:
         printf("UNKNOWN_EVENT id=\%d \%s\\n",i,text);
         break;
     }
   }
 }
</pre> */
int pvPollEvent(PARAM *p, char *event);
/*! <pre>
waits for an event.
</pre> */
int pvWait(PARAM *p, const char *pattern);
/*! <pre>
update OpenGL widget
</pre> */
int pvGlUpdate(PARAM *p, int id);
/*! <pre>
Sleep for milliseconds.
</pre> */
int pvSleep(int milliseconds);
/*! <pre>
Output a warning message.
</pre> */
int pvWarning(PARAM *p, const char *text);
/*! <pre>
Output a fatal message and terminate the whole server.
</pre> */
int pvMainFatal(PARAM *p, const char *text);
/*! <pre>
Output a fatal message and terminate the worker thread.
</pre> */
int pvThreadFatal(PARAM *p, const char *text);
/*! <pre>
Output a screenHint for calculating the zoom factor
Optimal screen width=w height=h .
</pre> */
int pvScreenHint(PARAM *p, int w, int h);
/*! <pre>
Set mouse shape
#MouseShape.
</pre> */
int pvSetMouseShape(PARAM *p, int shape);
/*! <pre>
Set whatsThis text
Allowed Widgets: all Widgets
</pre> */
int pvSetWhatsThis(PARAM *p, int id, const char *text);
/*! <pre>
printf whatsThis text
Allowed Widgets: all Widgets
</pre> */
int pvWhatsThisPrintf(PARAM *p, int id, const char *format, ...);
/*! <pre>
Run command on client.
command := pdf | img | svg | txt | csv | html | audio | video | save_as
See view.pdf, view.img, view.svg ... within pvbrowser options.
</pre> */
int pvClientCommand(PARAM *p, const char *command, const char *filename, int downloadFile=0);
/*! <pre>
Write "text" to a file "filename" in temp directory at client.
</pre> */
int pvWriteTextToFileAtClient(PARAM *p, const char *text, const char *filename);
/*! <pre>
Zoom the whole mask.
Zoom factor in percent.
</pre> */
int pvZoomMask(PARAM *p, int percent);
/*! <pre>
Set the URL which will be used for Help->Manual within pvbrowser.
default: index.html
You could also set the URL of a webserver which hosts your documentation.
Example:
pvSetManualUrl(p,"http://your.server.org");
</pre> */
int pvSetManualUrl(PARAM *p, const char *url);
/*! <pre>
Select the section for language translations with the pvtr() macro.

processviewserver.h defines the macro
#define pvtr(txt) txt

If you use something like pvSetText(p,id,pvtr("Hello World"));
the above macro will return the original untranslated text.

If you #include "rlinifile.h" the macro pvtr will be redefined to
#define pvtr(txt) rltranslate2(p->lang_section,txt)
Thus the subroutine rltranslate2() will be called.
This routine will also return the untranslated text until you call

rlSetTranslator("GERMAN","translation.ini");

within your main() program.
The above call to rlSetTranslator() will read the ini file "translation.ini" and
set the default section to "GERMAN".
This will be the default language within your pvserver.

If your translation.ini looks like
[GERMAN]
Hello World=Hallo Welt

the call to pvtr("Hello World") will return "Hallo Welt".
If there is no translation for a phrase then the original untranslated text will be returned.

Now you might want that each client can choose his own language.
This can be done by calling

pvSelectLanguage(p,"YOUR_LANGUAGE");

within pvMain() or one of your masks.
Hint: after changing the language you should return from the mask
with a return value that will call the mask again and will show the mask in the new language.

If the strings within the ini file include "=" characters you must quote them as "\=".
Also tabs and newline characters must be quoted as "\t" and "\n".
Within the graphical designer of pvdevelop you must use 2 quotes instead of one "\\=", "\\t" and "\\n".

Background:
We use an INI file for language translation.
The section names the language.
The original text is used to select the translation.
</pre> */
int pvSelectLanguage(PARAM *p, const char *section);

/*! <pre>
This function is intended for script languages.
It will translate text to different languages.
See pvSelectLanguage()
C/C++ pvservers may use the pvtr(txt) macro instead.
</pre> */
//const char *pvTr(PARAM *p, const char *text);
/** @} */ // end of group

/** @defgroup Contruction Construction
 *  These are the construction routines for the widgets.
 *  Normally you need not to use them because these calls are generated by ui2pvc.
 *  @{ */

/*! <pre>
Call this function first when you want to define a new mask.
Your enum for the mask should always contain ID_END_OF_WIDGETS as the last element.

Example:

pvStartDefinition(p,ID_END_OF_WIDGETS);

You can allocate space for additional widgets after ID_END_OF_WIDGETS by setting 
p->num_additional_widgets = number;
</pre> */
int pvStartDefinition(PARAM *p, int num_objects);
/*! <pre>
Creates a new QWidget. It's id can be used to identify it. It's parent widget is parent.
You can set one widget on top of another widget if you set parent to the parent widgets id.
When parent == 0 the widget will be set on the background screen (ID_MAIN_WIDGET).
QWidget draws nothing, but it is useful to group objects hierarchically.

Functions that apply to this widget:
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
</pre> */
int pvQWidget(PARAM *p, int id, int parent);

/*! <pre>
Layout Vbox
</pre> */
int pvQLayoutVbox(PARAM *p, int id, int parent);

/*! <pre>
Layout Hbox
</pre> */
int pvQLayoutHbox(PARAM *p, int id, int parent);

/*! <pre>
Layout Grid
</pre> */
int pvQLayoutGrid(PARAM *p, int id, int parent);

/*! <pre>
Creates a new QLabel. See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetAlignment()
pvSetText()
pvPrintf()
pvSetBackgroundColor()
pvText()
pvSetStyle()
pvSetMovie()
pvMovieControl()
pvMovieSpeed()
</pre> */
int pvQLabel(PARAM *p, int id, int parent);
/*! <pre>
editable = 0  not editable
editable = 1  user can edit combo box
#Policy.
Creates a new QComboBox. See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetAlignment()
pvSetText()
pvPrintf()
pvClear()
pvInsertItem()
pvRemoveItem()
pvRemoveItemByName()
pvSetCurrentItem()
pvSetEditable()
</pre> */
int pvQComboBox(PARAM *p, int id, int parent, int editable, int policy);
/*! <pre>
Creates a new QLineEdit. See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetAlignment()
pvSetText()
pvPrintf()
pvText()
pvSetEditable()
pvSetStyle()
pvSetEchoMode()
</pre> */
int pvQLineEdit(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QPushButton. See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetText()
pvPrintf()
pvSetPixmap()
pvText()
</pre> */
int pvQPushButton(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QLCDNumber.
#Mode. #SegmentStyle.
enum Mode         { HEX=0, DEC, OCT, BIN };
enum SegmentStyle { Outline=0, Filled, Flat };

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvDisplayNum()
pvDisplayFloat()
pvDisplayStr()
pvSetStyle()
</pre> */
int pvQLCDNumber(PARAM *p, int id, int parent, int numDigits, int segmentStyle, int mode);
/*! <pre>
Creates a new QSlider. See also pvQWidget().
orientation = HORIZONTAL|VERTICAL

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetMinValue()
pvSetMaxValue()
pvSetValue()
</pre> */
int pvQSlider(PARAM *p, int id, int parent, int minValue, int maxValue, int pageStep, int value, int orientation);
/*! <pre>
Creates a new QButtonGroup. See also pvQWidget().
orientation = HORIZONTAL|VERTICAL

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
</pre> */
int pvQButtonGroup(PARAM *p, int id, int parent, int columns, int orientation, const char *title);
/*! <pre>
Creates a new QRadioButton. See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetText()
pvPrintf()
pvSetChecked()
</pre> */
int pvQRadioButton(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QCheckBox. See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetText()
pvPrintf()
pvSetChecked()
</pre> */
int pvQCheckBox(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QFrame. See also pvQWidget().
#Shape. #Shadow.

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetStyle()
</pre> */
int pvQFrame(PARAM *p, int id, int parent, int shape, int shadow, int line_width, int margin);
/*! <pre>
Creates a new QDrawWidget. See also pvQWidget().
This type of widget can be used to draw diagrams and whatever you want.

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetSelector()
pvPrintSvgOnPrinter()
pvRenderTreeDump()
pvSetBufferTransparency()
pvSaveDrawBuffer()
See Module: Graphics
</pre> */
int pvQDraw(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QImage. See also pvQWidget().

If you specify the name of a 8bpp bmp file:
w = width of image will be returned   (w must be a multiple of 2)
h = height of image will be returned  (h must be a multiple of 2)
depth = number of bits per pixel (currently only 8 is supported)
A color of red=1 green=1 blue=1 will be treated as transparent

If you specify any other file format:
1) The format must be supported by Qt
2) you have to download the file to the client first
   see: int pvDownloadFile(PARAM *p, const char *filename);
w = 0
h = 0
depth = 0
will be returned

Using Qt Designer:
Insert a QFrame
set paletteBackgroundPixmap in the Property Editor pane and select a graphics file
set whatsThis=filename in the Property Editor pane

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetImage()
pvSetBufferedJpgImage()
pvPassThroughOneJpegFrame()
pvSendJpegFrame()
 </pre> */
int pvQImage(PARAM *p, int id, int parent, const char *imagename, int *w=NULL, int *h=NULL, int *depth=NULL);
/*! <pre>
Creates a new OpenGL Widget . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
See Module: OpenGL
</pre> */
int pvQGL(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QTabWidget . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetValue()
pvSetTabPosition()
pvEnableTabBar()
</pre> */
int pvQTabWidget(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QToolBox . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetValue()
pvSetStyle()
</pre> */
int pvQToolBox(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QGroupBox . See also pvQWidget().
orientation = HORIZONTAL|VERTICAL

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetStyle()
</pre> */
int pvQGroupBox(PARAM *p, int id, int parent, int columns, int orientation, const char *title);
/*! <pre>
Creates a new QListBox . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvClear()
pvChangeItem()
pvInsertItem()
pvRemoveItem()
pvRemoveItemByName()
pvSelection()
pvSetMultiSelection()
pvListBoxSetSelected();
</pre> */
int pvQListBox(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QTable . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetTableText()
pvSetTableButton()
pvSetTableCheckBox()
pvSetTableComboBox()
pvSetTableLabel()
pvTablePrintf()
pvClear();
pvSetColumnWidth()
pvSetRowHeight()
pvSetWordWrap()
pvSetTablePixmap()
pvEnsureCellVisible()
pvSetEditable()
pvTableSetEnabled()
pvTableSetHeaderResizeEnabled()
pvSetNumRows()
pvSetNumCols()
pvInsertRows()
pvInsertColumns()
pvRemoveRow()
pvRemoveColumn()
pvSetTableTextAlignment();
pvSave()
pvMysqldump()
pvCSVcreate()
pvCSVdump()
pvCSV()
</pre> */
int pvQTable(PARAM *p, int id, int parent, int rows, int columns);
/*! <pre>
Creates a new QSpinBox . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetMinValue()
pvSetMaxValue()
pvSetValue()
</pre> */
int pvQSpinBox(PARAM *p, int id, int parent, int min, int max, int step);
/*! <pre>
Creates a new QDial . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetMinValue()
pvSetMaxValue()
pvSetValue()
</pre> */
int pvQDial(PARAM *p, int id, int parent, int min, int max, int page_step, int value);
/*! <pre>
Creates a new QProgressBar . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetMinValue()
pvSetMaxValue()
pvSetValue()
pvSetStyle()
</pre> */
int pvQProgressBar(PARAM *p, int id, int parent, int total_steps, int orientation = Horizontal);
/*! <pre>
Creates a new QMultiLineEdit . See also pvQWidget().
editable = 0|1
if max_lines == -1 then no limit
Because of port from Qt3->Qt4 max_lines must be ignored.
Please use pvSetValue() instead.

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetText()
pvPrintf()
pvClear()
pvText()
pvSetEditable()
pvSetValue()
pvMoveCursor()
</pre> */
int pvQMultiLineEdit(PARAM *p, int id, int parent, int editable, int max_lines);
/*! <pre>
Creates a new QTextBrowser . See also pvQWidget().
QTextBrowser is now a WebKit Widget
editable = 0|1

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetText()
pvPrintf()
pvSetSource()
pvMoveContent()
pvMoveCursor()
pvScrollToAnchor()
pvSetZoomFactor()
pvPrintHtmlOnPrinter()
</pre> */
int pvQTextBrowser(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QListView . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvClear()
pvAddColumn()
pvRemoveAllColumns()
pvSetListViewText()
pvListViewPrintf()
pvListViewSetSelected()
pvSetListViewPixmap()
pvRemoveListViewItem()
pvSelection()
pvSetMultiSelection()
pvSetSorting()
pvListViewEnsureVisible()
pvListViewSetHidden()
pvListViewSetStandardPopupMenu()
</pre> */
int pvQListView(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QIconView . See also pvQWidget().

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvClear()
pvRemoveIconViewItem()
pvSetIconViewItem()
</pre> */
int pvQIconView(PARAM *p, int id, int parent);
/*! <pre>
Creates a new QVtkTclWidget . See also pvQWidget().

You can use this widget to use VTK for 3D Graphics.
Use pvVtkTcl()          to send single Tcl commands.
Use pvVtkTclPrintf()    to send single Tcl commands (use like printf).
Use pvVtkTclScript()    to send Tcl programs.

The constructor also runs the Tcl commands:
package require vtk
package require vtkinteraction
package require vtktesting
vtkRenderer renderer4
vtkRenderer renderer3
vtkRenderer renderer2
vtkRenderer renderer

renderer4 is connected to your widget !!!
renderer3 is connected to your widget !!!
renderer2 is connected to your widget !!!
renderer  is connected to your widget !!!

The destructor also runs the Tcl command:
vtkCommand DeleteAllObjects
</pre> */
int pvQVtkTclWidget(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QWT Plot Widget.
nCurves = Max Number of Curves in Plot
nMarker = Max Number of Markers in Plot

See Module: QwtPlotWidget 
</pre> */
int pvQwtPlotWidget(PARAM *p, int id, int parent, int nCurves, int nMarker);
/*! <pre>
Call this function to create a QwtScale.

See Module: QwtScale 
</pre> */ 
int pvQwtScale(PARAM *p, int id, int parent, int pos);
/*! <pre>
Call this function to create a QwtThermo.

See Module: QwtThermo 
</pre> */
int pvQwtThermo(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QwtKnob.

See Module: QwtKnob
</pre> */
int pvQwtKnob(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QwtCounter.

See Module: QwtCounter 
</pre> */
int pvQwtCounter(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QwtWheel.

See Module: QwtWheel 
</pre> */
int pvQwtWheel(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QwtSlider.

See Module: QwtSlider 
</pre> */
int pvQwtSlider(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QwtDial.

See Module: QwtDial 
</pre> */
int pvQwtDial(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QwtCompass.

See Module: QwtCompass 
</pre> */
int pvQwtCompass(PARAM *p, int id, int parent);
/*! <pre>
Call this function to create a QwtAnalogClock.

See Module: QwtAnalog Clock 
</pre> */
int pvQwtAnalogClock(PARAM *p, int id, int parent);
/*! <pre>
create a QDateEdit widget.

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetDate()
pvSetMinDate()
pvSetMaxDate()
pvSetDateOrder()
</pre> */
int pvQDateEdit(PARAM *p, int id, int parent);
/*! <pre>
create a QTimeEdit widget.

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetTime()
pvSetMinTime()
pvSetMaxTime()
pvSetTimeEditDisplay()
</pre> */
int pvQTimeEdit(PARAM *p, int id, int parent);
/*! <pre>
create a QDateTimeEdit widget.

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetDate()
pvSetMinDate()
pvSetMaxDate()
pvSetTime()
pvSetMinTime()
pvSetMaxTime()
pvSetDateOrder()
</pre> */
int pvQDateTimeEdit(PARAM *p, int id, int parent);
/*! <pre>
create a QCustomWidget provided by a plugin.
name := "/library/widgettype<:arg>" defined by the whats_this property in the designer.
        library is the name without the platform dependend extension (.so, .dll, .dylib).
        The library_filename will be PVB_WIDGET_PLUGINDIR/library.platform_extension.
        PVB_WIDGET_PLUGINDIR is defined in the pvbrowser ini file.
arg  := argument for the widget constructor defined by the whats_this property in the designer. 

Functions that apply to this widget:
pvSetWhatsThis()
pvWhatsThisPrintf()
pvToolTip()
pvSetGeometry()
pvSetMinSize()
pvSetMaxSize()
pvMove();
pvResize();
pvHide();
pvShow();
pvSetStyleSheet()
pvPrintfStyleSheet();
pvSetPaletteBackgroundColor()
pvSetPaletteForegroundColor()
pvSetFontColor()
pvSetFont()
pvSetEnabled()
pvSetFocus()
pvCopyToClipboard()
pvSaveAsBmp()
pvSetWidgetProperty()
</pre> */
int pvQCustomWidget(PARAM *p, int id, int parent, const char *name, const char *arg=NULL);
/*! <pre>
Call this function when you are finished with the definition of your mask.
</pre> */
int pvEndDefinition(PARAM *p);
/*! <pre>
add widget or layout to layout.
</pre> */
int pvAddWidgetOrLayout(PARAM *p, int id, int item, int row, int col);
/*! <pre>
add stretch to layout
</pre> */
int pvAddStretch(PARAM *p, int id, int param);
/*! <pre>
set TAB order.
</pre> */
int pvTabOrder(PARAM *p, int id1, int id2);
/*! <pre>
delete widget and it's children.
</pre> */
int pvDeleteWidget(PARAM *p, int id);
/** @} */ // end of group


/** @defgroup Output Output
 *  These are the output routines
 *  @{ */
/*! <pre>
Set program Title.
</pre> */
int pvSetCaption(PARAM *p, const char *text);
/*! <pre>
Play Sound. filename should point to a *.wav audio file. First pvDownloadFile(PARAM *p, const char *filename);
</pre> */
int pvPlaySound(PARAM *p, const char *filename);
/*! <pre>
Output a beep.
</pre> */
int pvBeep(PARAM *p);
/*! <pre>
Output status message.
If r = g = b = -1 -> normal background color
</pre> */
int pvStatusMessage(PARAM *p, int r, int g, int b, const char *format, ...);
/*! <pre>
Set a QToolTip for the widget. It will be displayed near the widget when you move over.
</pre> */
int pvToolTip(PARAM *p, int id, const char *text);
/*! <pre>
Set the text of a widget.
option := -1 || HTML_HEADER || HTML_STYLE || HTML_BODY
Allowed widgets: QLabel, QPushButton, QLineEdit, QMultiLineEdit, QComboBox, QRadioButton, QCheckBox, QTextBrowser
</pre> */
int pvSetTextEx(PARAM *p, int id, const char *text, int option);
/*! <pre>
Set the text of a widget.
Allowed widgets: QLabel, QPushButton, QLineEdit, QMultiLineEdit, QComboBox, QRadioButton, QCheckBox, QTextBrowser
</pre> */
int pvSetText(PARAM *p, int id, const char *text);
/*! <pre>
Set the text of a widget.
The functions works like printf()
Allowed widgets: QLabel, QPushButton, QLineEdit, QMultiLineEdit, QComboBox, QRadioButton, QCheckBox, QTextBrowser, QGroupBox
</pre> */
int pvPrintf(PARAM *p, int id, const char *format, ...);
/*! <pre>
Set the style sheet of a widget. (See Qt documentation for style sheets)
Allowed widgets: all widgets
id := id_of_widget or ID_ROOTWIDGET, ID_EDITBAR, ID_TOOLBAR, ID_STATUSBAR, ID_MAINWINDOW or id_of_dock_widget
</pre> */
int pvSetStyleSheet(PARAM *p, int id, const char *text);
/*! <pre>
Printf the style sheet of a widget. (See Qt documentation for style sheets)
The functions works like printf()
Allowed widgets: all widgets
id := id_of_widget or ID_ROOTWIDGET, ID_EDITBAR, ID_TOOLBAR, ID_STATUSBAR, ID_MAINWINDOW or  id_of_dock_widget
</pre> */
int pvPrintfStyleSheet(PARAM *p, int id, const char *format, ...);
/*! <pre>
Set the minimum value of a widget.
Allowed widgets: QSlider, QSpinBox, QDial, QProgressBar
</pre> */
int pvSetMinValue(PARAM *p, int id, int value);
/*! <pre>
Set the maximum value of a widget.
Allowed widgets: QSlider, QSpinBox, QDial, QProgressBar
</pre> */
int pvSetMaxValue(PARAM *p, int id, int value);
/*! <pre>
Set the value of a widget.
Allowed widgets: QSlider, QSpinBox, QDial, QProgressBar, QTabWidget, QToolBox
For QMultiLineEdit set maxlines = value
</pre> */
int pvSetValue(PARAM *p, int id, int value);
/*! <pre>
Clear the content of a widget.
Allowed widgets: QTable, QListBox, QComboBox, QMultiLineEdit, QListView, QIconView
</pre> */
int pvClear(PARAM *p, int id);
/*! <pre>
Set the content of a widget.
if bmp_file == NULL no pixmap is drawn
else bmp_file = name of PNG file or an 8bpp bitmap file
Allowed widgets: QListBox
</pre> */
int pvChangeItem(PARAM *p, int id, int index, const char *bmp_file, const char *text, int download_icon=1);
/*! <pre>
Insert an item.
if bmp_file == NULL no pixmap is drawn
else bmp_file = name of PNG file or an 8bpp bitmap file
if index == -1 append text at the end of the list
Allowed widgets: QListBox, QComboBox
</pre> */
int pvInsertItem(PARAM *p, int id, int index, const char *bmp_file, const char *text, int download_icon=1);
/*! <pre>
Remove an item.
Allowed widgets: QListBox, QComboBox
</pre> */
int pvRemoveItem(PARAM *p, int id, int index);
/*! <pre>
Remove an item by it's name.
Allowed widgets: QListBox, QComboBox
</pre> */
int pvRemoveItemByName(PARAM *p, int id, const char *name);
/*! <pre>
Add a Column in a QListView.
Allowed widgets: QListView
</pre> */
int pvAddColumn(PARAM *p, int id, const char *text, int size);
/*! <pre>
Remove all Columns in a QListView.
Allowed widgets: QListView
</pre> */
int pvRemoveAllColumns(PARAM *p, int id);
/*! <pre>
Set the text of a table cell.
if x == -1 then set row text
if y == -1 then set column text

Example for colored table cells:
pvTableText(p,Table1,0,0,"color(255,0,0)this is the cell text");
prepend: color(r,g,b)

Allowed widgets: QTable
</pre> */
int pvSetTableText(PARAM *p, int id, int x, int y, const char *text);
/*! <pre>
Set a table cell to Button.
Allowed widgets: QTable
</pre> */
int pvSetTableButton(PARAM *p, int id, int x, int y, const char *text);
/*! <pre>
Set a table cell to CheckBox.
Allowed widgets: QTable
</pre> */
int pvSetTableCheckBox(PARAM *p, int id, int x, int y, int state, const char *text);
/*! <pre>
Set a table cell to ComboBox.

example:
pvSetTableComboBox(p,Table1,2,1,1,"choice1,choice2,choice3");

You can add a # in front of choice in order to make it the selectedChoice.
example: make choice2 the selectedChoice
pvSetTableComboBox(p,Table1,2,1,1,"choice1,#choice2,choice3");

Allowed widgets: QTable
</pre> */
int pvSetTableComboBox(PARAM *p, int id, int x, int y, int editable, const char *textlist);
/*! <pre>
Set a table cell to label
Allowed widgets: QTable
</pre> */
int pvSetTableLabel(PARAM *p, int id, int x, int y, const char *text);
/*! <pre>
Set the text of a table cell.
Works as printf(format,...);
if x == -1 then set row text
if y == -1 then set column text

Example for colored table cells:
pvTablePrintf(p,Table1,0,0,"color(255,0,0)this is the cell text");
prepend: color(r,g,b)

Allowed widgets: QTable
</pre> */
int pvTablePrintf(PARAM *p, int id, int x, int y, const char *format, ...);
/*! <pre>
Set the text alignment of a table cell.

alignment :=
  AlignLeft     # Aligns with the left edge.
  AlignRight    # Aligns with the right edge.
  AlignHCenter  # Centers horizontally in the available space.
  AlignJustify  # Justifies the text in the available space.

Allowed widgets: QTable
</pre> */
int pvSetTableTextAlignment(PARAM *p, int id, int x, int y, int alignment);
/*! <pre>
Run mysqldump and populate table.
See: mysqldump --help
Runs: mysqldump -X command

Allowed widgets: QTable
</pre> */
int pvMysqldump(PARAM *p, int id, const char *command);
/*! <pre>
dump CSV file and populate table.

Allowed widgets: QTable
</pre> */
int pvCSVdump(PARAM *p, int id, const char *filename, char delimitor='\t');
/*! <pre>
create CSV file by calling "command > filename".
</pre> */
int pvCSVcreate(PARAM *p, const char *command, const char *filename);
/*! <pre>
dump CSV file to table=id by calling "command".
</pre> */
int pvCSV(PARAM *p, int id, const char *command, char delimitor='\t');
/*! <pre>
Set the text of a ListViewItem.
example:
path = "/dir/subdir/subsubdir"
path := similar to a unix directory path

Example for colored cells:
pvSetListViewText(p,myListView,"/path/to",0,"color(255,0,0)this is the colored text");
prepend: color(r,g,b)

Allowed widgets: QListView
</pre> */
int pvSetListViewText(PARAM *p, int id, const char *path, int column, const char *text);
/*! <pre>
Set the text of a ListViewItem.
Works as printf(format,...);
example:
path = "/dir/subdir/subsubdir"
path := similar to a unix directory path

Example for colored cells:
pvListViewPrintf(p,myListView,"/path/to",0,"color(255,0,0)this is the colored text");
prepend: color(r,g,b)

Allowed widgets: QListView
</pre> */
int pvListViewPrintf(PARAM *p, int id, const char *path, int column, const char *format, ...);
/*! <pre>
selected = 0 | 1 | 2
if selected == 0 item is unselected
if selected == 1 item is selected
if selected == 2 item is selected and all other tree branches are closed
</pre> */
int pvListViewSetSelected(PARAM *p, int id, const char *path, int column, int selected);
/*! <pre>
selected = 0 | 1 | 2
if selected == 0 item is unselected
if selected == 1 item is selected
</pre> */
int pvListBoxSetSelected(PARAM *p, int id, int index, int selected);
#define pvSetColumnWith pvSetColumnWidth
/*! <pre>
Set the width of a table column.
if column == -1 then 
  set border width
  if width >  0  -> set width
  if width == 0  -> hide
  if width == -1 -> resize to contents
Allowed widgets: QTable
</pre> */
int pvSetColumnWidth(PARAM *p, int id, int column, int width);
/*! <pre>
Set the height of a table row.
if row == -1 then set border height
if row >= 0 && height == -1 then set autoresizeRowToContents=1
if row >= 0 && height == -2 then set autoresizeRowToContents=0
default autoresizeRowToContents=0
Allowed widgets: QTable
</pre> */
int pvSetRowHeight(PARAM *p, int id, int row, int height);
/*! <pre>
Set word wrap for table.
wrap = 0 | 1
Allowed widgets: QTable
</pre> */
int pvSetWordWrap(PARAM *p, int id, int wrap);
/*! <pre>
Set the pixmap.
Use PNG file.

also:
bmp_file = name of an 8bpp bitmap file
if bmp_file == NULL reset pixmap
Allowed widgets: QPushButton
</pre> */
int pvSetPixmap(PARAM *p, int id, const char *bmp_file, int download_icon=1);
/*! <pre>
Set the pixmap of a table cell.
Use PNG file.

also:
bmp_file = name of an 8bpp bitmap file
Allowed widgets: QTable
</pre> */
int pvSetTablePixmap(PARAM *p, int id, int x, int y, const char *bmp_file, int download_icon=1);
/*! <pre>
Set the file to show in QTextBrowser.
html_file = file to start with
or
http://webpage.url
Allowed widgets: QTextBrowser
</pre> */
int pvSetSource(PARAM *p, int id, const char *html_file);
/*! <pre>
Set a new image in an existing image.
if(rotate == +90) then rotate +90 degrees
if(rotate == -90) then rotate -90 degrees
Allowed widgets: QImage
</pre> */
int pvSetImage(PARAM *p, int id, const char *filename, int rotate=0);
/*! <pre>
Set a new image in an existing image.
For example get a frame from a MJPG webcam with the rlWebcam method
  int getFrameBuffer(unsigned char *buffer, int maxbuffer, int timeout=3000);
and send it to the pvbrowser client with this function.
if(rotate == +90) then rotate +90 degrees
if(rotate == -90) then rotate -90 degrees
Allowed widgets: QImage
</pre> */
int pvSetBufferedJpgImage(PARAM *p, int id, const unsigned char *buffer, int buffersize, int rotate=0);
/*! <pre>
Set the transparency for the drawing buffer.
"int a;" is in the range of 0-255 where 0 is fully transparent and 255 is fully opaque
Allowed widgets: QDraw
</pre> */
int pvSetBufferTransparency(PARAM *p, int id, int a);
/*! <pre>
Set the background color of the widget.
Allowed widgets: QLabel, QDraw
</pre> */
int pvSetBackgroundColor(PARAM *p, int id, int r, int g, int b);
/*! <pre>
Set the background color of the widget.
r=-1,g=-1,b=-1 := unsetPalette()
Allowed widgets: all widgets
</pre> */
int pvSetPaletteBackgroundColor(PARAM *p, int id, int r, int g, int b);
/*! <pre>
Set the foreground color of the widget.
r=-1,g=-1,b=-1 := unsetPalette()
Allowed widgets: all widgets
</pre> */
int pvSetPaletteForegroundColor(PARAM *p, int id, int r, int g, int b);
/*! <pre>
Set the font color of the widget.
r=-1,g=-1,b=-1 := unsetPalette()
Allowed widgets: QMultiLineEdit
</pre> */
int pvSetFontColor(PARAM *p, int id, int r, int g, int b);
/*! <pre>
Set the font of the widget. The font will be propagated to all children.
pointsize (in pitch)
bold      = 0|1
italic    = 0|1
underline = 0|1
strikeout = 0|1
Allowed widgets: all widgets
</pre> */
int pvSetFont(PARAM *p, int id, const char *family, int pointsize, int bold, int italic , int underline, int strikeout);
/*! <pre>
Display num on a QLCDNumber
</pre> */
int pvDisplayNum(PARAM *p, int id, int num);
/*! <pre>
Display float on a QLCDNumber
</pre> */
int pvDisplayFloat(PARAM *p, int id, float val);
/*! <pre>
Display string on a QLCDNumber
</pre> */
int pvDisplayStr(PARAM *p, int id, const char *str);
/*! <pre>
Add a Tab to an QTabDialog
id       := id of TabWidget
id_child := id of the QWidget for this tab
str      := text on the tab
</pre> */
int pvAddTab(PARAM *p, int id, int id_child, const char *str);
/*! <pre>
example:
path = "/dir/subdir/subsubdir"
path := similar to a unix directory path
Set a QListView pixmap
Use PNG file.
</pre> */
int pvSetListViewPixmap(PARAM *p, int id, const char *path, const char *bmp_file, int column, int download_icon=1);
/*! <pre>
example:
path = "/dir/subdir/subsubdir"
path := similar to a unix directory path
Remove a QListView item
</pre> */
int pvRemoveListViewItem(PARAM *p, int id, const char *path);
/*! <pre>
Remove a QIconView item
</pre> */
int pvRemoveIconViewItem(PARAM *p, int id, const char *text);
/*! <pre>
Insert a QIconViewItem pixmap and text
</pre> */
int pvSetIconViewItem(PARAM *p, int id, const char *bmp_file, const char *text, int download_icon=1);
/*! <pre>
Set date order, enum Order { DMY, MDY, YMD, YDM }
allowed widgets: QDateEdit, QDateTimeEdit
</pre> */
int pvSetDateOrder(PARAM *p, int id, int order);
/*! <pre>
Set date
allowed widgets: QDateEdit, QDateTimeEdit
</pre> */
int pvSetDate(PARAM *p, int id, int year, int month, int day);
/*! <pre>
Set min date
allowed widgets: QDateEdit, QDateTimeEdit
</pre> */
int pvSetMinDate(PARAM *p, int id, int year, int month, int day);
/*! <pre>
Set max date
allowed widgets: QDateEdit, QDateTimeEdit
</pre> */
int pvSetMaxDate(PARAM *p, int id, int year, int month, int day);
/*! <pre>
Set time
allowed widgets: QTimeEdit, QDateTimeEdit
</pre> */
int pvSetTime(PARAM *p, int id, int hour, int minute, int second=0, int msec=0);
/*! <pre>
Set time
allowed widgets: QTimeEdit, QDateTimeEdit
</pre> */
int pvSetMinTime(PARAM *p, int id, int hour, int minute, int second=0, int msec=0);
/*! <pre>
Set time
allowed widgets: QTimeEdit, QDateTimeEdit
</pre> */
int pvSetMaxTime(PARAM *p, int id, int hour, int minute, int second=0, int msec=0);
/*! <pre>
Ensure that the table cell is visible, scroll if necessary
allowed widgets: QTable
</pre> */
int pvEnsureCellVisible(PARAM *p, int id, int row, int col=1);
/*! <pre>
Move the cursor
allowed widgets: QTextBrowser, QMultiLineEdit
cursor := #TextCursor
</pre> */
int pvMoveCursor(PARAM *p, int id, int cursor);
/*! <pre>
Scroll to anchor
allowed widgets: QTextBrowser
</pre> */
int pvScrollToAnchor(PARAM *p, int id, const char *anchor);
/*! <pre>
Set zoom factor of the HTML page
allowed widgets: QTextBrowser
</pre> */
int pvSetZoomFactor(PARAM *p, int id, float factor);
/*! <pre>
Print the HTML page on a printer
allowed widgets: QTextBrowser
</pre> */
int pvPrintHtmlOnPrinter(PARAM *p, int id);
/*! <pre>
Set a property of a custom widget.
See the documentation of the custom widget plugin.
allowed widgets: QCustomWidget
</pre> */
int pvSetWidgetProperty(PARAM *p, int id, const char *name, const char *value);
/*! <pre>
Pass 1 JPEG frame from a camera or other source to the pvbrowser client.
Allowed widgets: QImage
</pre> */
int pvPassThroughOneJpegFrame(PARAM *p, int id, int source_fhdl, int inputIsSocket=1, int rotate=0);
/*! <pre>
Send 1 JPEG frame from a camera or other source to the pvbrowser client.
Allowed widgets: QImage
</pre> */
int pvSendJpegFrame(PARAM *p, int id, unsigned char *frame, int rotate=0);
/*! <pre>
Send a RGBA image to the pvbrowser client.
Allowed widgets: QImage
</pre> */
int pvSendRGBA(PARAM *p, int id, const unsigned char *image, int width, int height, int rotate=0);
/*! <pre>
Save rendered buffer as PNG or JPG file in temporary directory.
filename is without path.
Allowed widgets: QDrawWidget
</pre> */
int pvSaveDrawBuffer(PARAM *p, int id, const char *filename);
/** @} */ // end of group

/** @defgroup Input Input
 *  These are the input routines
 *  @{ */
/*! <pre>
Request a waitpid() call on the client.
The response is send as TEXT_MESSAGE with id=ID_MAINWINDOW.
waitpid_response=%d
</pre> */
int pvWaitpid(PARAM *p);
/*! <pre>
Request the text from the widget. The text will arrive in an TEXT_EVENT.
In case of a QMultiLineEdit the text will arrive in a CLIPBOARD_EVENT.
Allowed widgets: QLabel, QPushButton, QLineEdit, QMultiLineEdit
</pre> */
int pvText(PARAM *p, int id);
/*! <pre>
Request a jpeg from the widget. The jpeg will arrive in an CLIPBOARD_EVENT.
Allowed widgets: QWidget, QImage, QDraw, QwtPlot
</pre> */
int pvRequestJpeg(PARAM *p, int id);
/*! <pre>
Request the geometry from the widget. The text will arrive in an TEXT_EVENT.
geometry:x,y,width,height
Allowed widgets: all widgets
</pre> */
int pvRequestGeometry(PARAM *p, int id);
/*! <pre>
Request the parent id for the widget. The text will arrive in an TEXT_EVENT.
parent:parent
Allowed widgets: all widgets
</pre> */
int pvRequestParentWidgetId(PARAM *p, int id);
/*! <pre>
Request the selected items from a QListBox or QListView. The text will arrive in an SELECTION_EVENT.
Allowed widgets: QListBox, QListView
</pre> */
int pvSelection(PARAM *p, int id);
/*! <pre>
Request the bounds of an object within a SVG graphic.
The response will arrive in a TEXT_EVENT
Response: name=x,y,width,height
Allowed widgets: QDrawWidget
</pre> */
int pvRequestSvgBoundsOnElement(PARAM *p, int id, const char *objectname);
/*! <pre>
Request the matrix for an object within a SVG graphic.
The response will arrive in a TEXT_EVENT
Response: name=m11, m12, m21, m22, det, dx, dy
Allowed widgets: QDrawWidget
</pre> */
int pvRequestSvgMatrixForElement(PARAM *p, int id, const char *objectname);
/** @} */ // end of group

/** @defgroup State State
 *  These are the state routines
 *  @{ */
/*! <pre>
Move within html page.
pos = Home|Backward|Forward|Reload
or
pos = Qt::KeyboardModifiers ored with ascii key. Where key >= ' ' && key < 128
      Qt::NoModifier      0x00000000  No modifier key is pressed.
      Qt::ShiftModifier   0x02000000  A Shift key on the keyboard is pressed.
      Qt::ControlModifier 0x04000000  A Ctrl key on the keyboard is pressed.
      Qt::AltModifier     0x08000000  An Alt key on the keyboard is pressed.
Allowed widgets: QTextBrowser
</pre> */
int pvMoveContent(PARAM *p, int id, int pos);
/*! <pre>
Set the Geometry of the widget.
</pre> */
int pvSetGeometry(PARAM *p, int id, int x, int y, int w, int h);
/*! <pre>
Set the Minimum Size of the widget.
</pre> */
int pvSetMinSize(PARAM *p, int id, int w, int h);
/*! <pre>
Set the Maximum Size of the widget.
</pre> */
int pvSetMaxSize(PARAM *p, int id, int w, int h);
/*! <pre>
Set Alignment. #AlignmentFlags
Allowed widgets: QLabel QLineEdit
</pre> */
int pvSetAlignment(PARAM *p, int id, int alignment);
/*! <pre>
Set the state (0,1) of a button
Allowed widgets: QRadioButton, QCheckBox
</pre> */
int pvSetChecked(PARAM *p, int id, int state);
/*! <pre>
Move the widget to a new position.
</pre> */
int pvMove(PARAM *p, int id, int x, int y);
/*! <pre>
Resize the widget.
See also #MainWindowIds.
</pre> */
int pvResize(PARAM *p, int id, int w, int h);
/*! <pre>
Hide the widget.
See also #MainWindowIds.
</pre> */
int pvHide(PARAM *p, int id);
/*! <pre>
Show the widget.
See also #MainWindowIds.
</pre> */
int pvShow(PARAM *p, int id);
/*! <pre>
Reparent a widget
All widgets allowed
</pre> */
int pvSetParent(PARAM *p, int id, int id_parent);
/*! <pre>
Set multi selection of QListBox, QListView.
mode = 0=SingleSelection|1=MultiSelection|2=NoSelection
</pre> */
int pvSetMultiSelection(PARAM *p, int id, int mode);
/*! <pre>
Set echo mode of QLineEdit.
mode = 0=NoEcho|1=Normal|2=Password
</pre> */
int pvSetEchoMode(PARAM *p, int id, int mode);
/*! <pre>
Set editable 0|1
Allowed widgets QLineEdit, QMultiLineEdit, QTable, QComboBox
</pre> */
int pvSetEditable(PARAM *p, int id, int editable);
/*! <pre>
Set enabled 0|1
Allowed widgets all widgets
</pre> */
int pvSetEnabled(PARAM *p, int id, int enabled);
/*! <pre>
Set keyboad focus
Allowed widgets all widgets
</pre> */
int pvSetFocus(PARAM *p, int id);
/*! <pre>
Set enabled 0|1
Allowed widgets QTable
</pre> */
int pvTableSetEnabled(PARAM *p, int id, int x, int y, int enabled);
/*! <pre>
horizontal = 0 is vertical
horizontal = 1 is horizontal
enabled := 0|1
section := section in horizontal or vertical direction
Allowed widgets QTable
</pre> */
int pvTableSetHeaderResizeEnabled(PARAM *p, int id, int horizontal ,int enabled, int section);
/*! <pre>
Sort column
mode=0 decending
mode=1 ascendin
column=-1 do not allow sorting (this is the default)
Allowed widgets QListView
</pre> */
int pvSetSorting(PARAM *p, int id, int column, int mode);
/*! <pre>
Set tab position 0=Top | 1=Bottom | 2=West | 3=East
Allowed widgets QTabWidget
</pre> */
int pvSetTabPosition(PARAM *p, int id, int pos);
/*! <pre>
Enable the TabBar 0=disable | 1=enable
Allowed widgets QTabWidget
</pre> */
int pvEnableTabBar(PARAM *p, int id, int state);
/*! <pre>
Set num rows in table
Allowed widgets QTable
</pre> */
int pvSetNumRows(PARAM *p, int id, int num);
/*! <pre>
Set num cols in table
Allowed widgets QTable
</pre> */
int pvSetNumCols(PARAM *p, int id, int num);
/*! <pre>
Insert count rows
Allowed widgets QTable
</pre> */
int pvInsertRows(PARAM *p, int id, int row, int count=1);
/*! <pre>
Insert count columns
Allowed widgets QTable
</pre> */
int pvInsertColumns(PARAM *p, int id, int col, int count=1);
/*! <pre>
Remove a row
Allowed widgets QTable
</pre> */
int pvRemoveRow(PARAM *p, int id, int row);
/*! <pre>
Remove a colum
Allowed widgets QTable
</pre> */
int pvRemoveColumn(PARAM *p, int id, int col);
/*! <pre>
Set current item
Allowed widgets QComboBox
</pre> */
int pvSetCurrentItem(PARAM *p, int id, int index);
/*! <pre>
Set display of time edit
values may be 0 or 1
Allowed widgets QTimeEdit
</pre> */
int pvSetTimeEditDisplay(PARAM *p, int id, int hour, int minute, int second, int ampm);
/*! <pre>
Ensures that the given item is visible.
example:
path = "/dir/subdir/subsubdir"
path := similar to a unix directory path
Allowed widgets QListView
</pre> */
int pvListViewEnsureVisible(PARAM *p, int id, const char *path);
/*! <pre>
Opens/Closes the given item.
open = 0 | 1
example:
path = "/dir/subdir/subsubdir"
path := similar to a unix directory path
Allowed widgets QListView
</pre> */
int pvListViewSetOpen(PARAM *p, int id, const char *path, int open);
/*! <pre>
Hide or show column
</pre> */
int pvListViewSetHidden(PARAM *p, int id, int column, int hidden);
/*! <pre>
Set standard popup menu of list view.
standard_menu = 0 do not show standard popup menu
standard_menu = 1 show standard popup menu and do not allow to hide column 1
standard_menu = N show standard popup menu and do not allow to hide column N
</pre> */
int pvListViewSetStandardPopupMenu(PARAM *p, int id, int standard_menu);
/*! <pre>
Set style of QLabel or QFrame
#Shape. #Shadow.

if parameter == -1 -> do not set this paramter

Allowed widgets QLabel QLineEdit QFrame QGroupBox QLCDNumber QProgressBar QToolBox
</pre> */
int pvSetStyle(PARAM *p, int id, int shape, int shadow, int line_width, int margin);
/*! <pre>
Set to an animated MNG or GIF
background = 0|1

Allowed widgets QLabel
</pre> */
int pvSetMovie(PARAM *p, int id, int background, const char *filename);
/*! <pre>
step == -2 restart
step == -1 unpause
step == 0  pause
step >  0  step step steps

Allowed widgets QLabel
</pre> */
int pvMovieControl(PARAM *p, int id, int step);
/*! <pre>
speed in percent

Allowed widgets QLabel
</pre> */
int pvMovieSpeed(PARAM *p, int id, int speed);
/*! <pre>
add tab icon

Allowed widgets QTabWidget QToolBox
</pre> */
int pvAddTabIcon(PARAM *p, int id, int position, const char *bmp_file, int download_icon=1);
/*! <pre>
set cell widget within a table

Allowed widgets QTable
</pre> */
int pvSetCellWidget(PARAM *p, int id, int parent, int row, int column);
/*! <pre>
set margins within layout

Allowed widgets Layouts
</pre> */
int pvSetContentsMargins(PARAM *p, int id, int xleft, int ytop, int xright, int ybottom);
/*! <pre>
set spacing within layout

Allowed widgets Layouts
</pre> */
int pvSetSpacing(PARAM *p, int id, int param);
/** @} */ // end of group


/** @defgroup VTK Visualization Tool Kit
 *  These are the vtk output routines
 *  @{ */
/*! <pre>
Send a single Tcl command to the widget
</pre> */
int pvVtkTcl(PARAM *p, int id, const char *tcl_command);
/*! <pre>
Send a single Tcl command to the widget. Used like printf()
</pre> */
int pvVtkTclPrintf(PARAM *p, int id, const char *format, ...);
/*! <pre>
Send a file with Tcl commands to the widget.
</pre> */
int pvVtkTclScript(PARAM *p, int id, const char *filename);
/** @} */ // end of group

/** @defgroup Hyperlink Hyperlink
 *  These are the hyperlink routines
 *  @{ */
/*! <pre>
Connect to a different server.
eg. link = "pv://hostname:5050"
If you add "+" at the beginning of the URL a new pvbrowser will be started with that URL.
eg. link = "+pv://hostname:5050"
</pre> */
int pvHyperlink(PARAM *p, const char *link);
/** @} */ // end of group

/** @defgroup Misc Misc
 *  These are the misc routines
 *  @{ */
/*! <pre>
Send a USER_EVENT to ourself
</pre> */
int pvSendUserEvent(PARAM *p, int id, const char *text);
/*! <pre>
write the following to file
during the file is open nothing will be send to tcp
return=0 failure
return=1 success
</pre> */
int pvWriteFile(PARAM *p, const char *filename, int width, int height);
/*! <pre>
close the open file
</pre> */
int pvCloseFile(PARAM *p);
/*! <pre>
The function will get the "" surrounded text out of command.
This is useful to retrieve the text from an event.
Used in Script languages
</pre> */
char *pvGetTextParam(PARAM *p, const char *command);
/*! <pre>
The function will get the "" surrounded text out of command.
This is useful to retrieve the text from an event.
</pre> */
int pvGetText(const char *command, char *text);
/*! <pre>
This fuction will parse the event. It is a stub for script languages.
</pre> */
PARSE_EVENT_STRUCT *pvParseEventStruct(PARAM *p, const char *event);
/*! <pre>
This fuction will parse the event. It returns the event. If there is a text in the event
it will be returned. Otherwise *text will be '\\0'.
</pre> */
int pvParseEvent(const char *event, int *id, char *text);
/*! <pre>
Copy the widget to the clipboard.
Allowed widgets: all widgets
</pre> */
int pvCopyToClipboard(PARAM *p, int id);
/*! <pre>
Print the contents of the widget on a printer. The user will see the print dialog.
Allowed widgets: all widgets
</pre> */
int pvPrint(PARAM *p, int id);
/*! <pre>
Save the widget to file on local node. The user will see a SaveAs dialog.
Allowed widgets: QTable
</pre> */
int pvSave(PARAM *p, int id);
/*! <pre>
Save the widget to a file on the client computer. (vector form)
Allowed widgets: QDraw
</pre> */
int pvSave(PARAM *p, int id, const char *filename);
/*! <pre>
Save the widget to a file on the client computer. (bitmap file)
Allowed widgets: all widgets
</pre> */
int pvSaveAsBmp(PARAM *p, int id, const char *filename);
/*! <pre>
Dump the HTML or SVG code on the client.
Allowed widgets: QTextBrowser/WebKit widget for HTML and QDraw/SVG widget for SVG
</pre> */
int pvHtmlOrSvgDump(PARAM *p, int id, const char *filename);
/*! <pre>
Dump the SVG rendering data to a SVG file on the client.
Allowed widgets: QDraw/SVG widget
</pre> */
int pvRenderTreeDump(PARAM *p, int id, const char *filename);
/*! <pre>
send file to browser
</pre> */
int pvSendFile(PARAM *p, const char *filename);
/*! <pre>
download file to temp directory of the browser
The file in the temp directory is called newname
</pre> */
int pvDownloadFileAs(PARAM *p, const char *filename, const char *newname);
/*! <pre>
download file to temp directory of the browser
</pre> */
int pvDownloadFile(PARAM *p, const char *filename);

/*! <pre>
Set the limit of clients the pvserver accepts from 1 ip address 
</pre> */
int pvSetMaxClientsPerIpAdr(int max_clients);
/*! <pre>
Get the limit of clients the pvserver accepts from 1 ip address 
</pre> */
int pvMaxClientsPerIpAdr();
/*! <pre>
Set the total limit of clients the pvserver accepts 
0 < max_clients <= MAX_CLIENTS
</pre> */
int pvSetMaxClients(int max_clients);
/*! <pre>
Set the total limit of clients the pvserver accepts 
0 < max_clients <= MAX_CLIENTS
</pre> */
int pvMaxClients();
/*! <pre>
Get the table of connected clients.
It is an array of MAX_CLIENT elements.
</pre> */
const pvAddressTableItem *pvGetAdrTableItem(); 

/*! <pre>
read all pending messages from tcp
</pre> */
int pvClearMessageQueue(PARAM *p);
int pvtcpsend(PARAM *p, const char *buf, int len);
int pvtcpsendstring(PARAM *p, const char *buf);
int pvtcpsend_binary(PARAM *p, const char *buf, int len);
int pvtcpreceive(PARAM *p, char *buf, int maxlen);
int pvtcpreceive_binary(PARAM *p, char *buf, int maxlen);

/** @} */ // end of group

/** @defgroup http http helper functions
 *  These functions you can use to act as httpd.
 *  It is possible to implement your pvserver to act as pvbrowser server and httpd at the same time.
 *  Please read the manual in PDF format.
 *
 *  Note: You must use the -http option to start your pvserver.
 *
 *  ./pvs -http
 *
 *  This will suppress the automatic sending of pvsVersion() at pvserver startup.
 *
 *  The full HTTP/1.1 specification can be found here:
 *
 *  https://www.w3.org/Protocols/rfc2616/rfc2616.html
 *
 *  In future we might add more helper function based on this specification.
 *  But you can implement helper functions at your own using.
 *
 *  int pvtcpsend(PARAM *p, const char *buf, int len);
 *
 *  int pvtcpsendstring(PARAM *p, const char *buf);
 *
 *  int pvtcpsend_binary(PARAM *p, const char *buf, int len);
 *
 *  int pvtcpreceive(PARAM *p, char *buf, int maxlen);
 *
 *  int pvtcpreceive_binary(PARAM *p, char *buf, int maxlen);
 *
 *  Your own httpd helper functions could be included within the upstream project.
 *  Please take care that the helper functions are usefull for wide use cases.
 *  @{ */
/*! <pre>
send http response using chunks

Example:
  pvtcpsendstring(p,
    "HTTP/1.1 200 OK\n"
    "Date: Wed, 05 Aug 2015 14:10:30 GMT\n"
    "Server: Apache/2.2.4 (Linux/SUSE)\n"
    "X-Powered-By: PHP/5.2.11\n"
    "Keep-Alive: timeout=150, max=1000\n"
    "Connection: Keep-Alive\n"
    "Transfer-Encoding: chunked\n"
    "Content-Type: text/html\n"
    "\n");
  pvSendHttpChunks(p,"test.html");
</pre> */
int pvSendHttpChunks(PARAM *p, const char *filename);
/*! <pre>
send http response using Content-Length

Example: 
  sprintf(buf,"HTTP/1.1 200 OK\n"); 
  pvtcpsendstring(p,buf); 
  sprintf(buf,"Server: pvserver-%s\n", pvserver_version); 
  pvtcpsendstring(p,buf); 
  sprintf(buf,"Keep-Alive: timeout=15, max=100\n"); 
  pvtcpsendstring(p,buf); 
  sprintf(buf,"Connection: Keep-Alive\n"); 
  pvtcpsendstring(p,buf); 
  sprintf(buf,"Content-Type: text/html\n"); 
  pvtcpsendstring(p,buf); 
  pvSendHttpContentLength(p,"test.html"); 
</pre> */
int pvSendHttpContentLength(PARAM *p, const char *filename);
/*! <pre>
send http response file
default Content-Type = text/html
</pre> */
int pvSendHttpResponseFile(PARAM *p, const char *filename, const char *content_type="text/html");
/*! <pre>
send http response using HTML text.
</pre> */
int pvSendHttpResponse(PARAM *p, const char *html);
/** @} */ // end of group

/** @defgroup OpenGL OpenGL
 *  These are the routines that you must use when you want OpenGL.
 *  You can use any OpenGL function between pvGlBegin() and pvGlEnd() .
 *
 *  You can also send files that are generated by our code generators 
 *  e.G. Autocad DWF2OpenGL
 *  @{ */
/*! <pre>
Call this function when you want to begin with OpenGL commands
</pre> */
int pvGlBegin(PARAM *p, int id);
// /*! <pre>
// This class is for reading a font for use within OpenGL 
// </pre> */
class glFont
{
public:
  glFont();
  ~glFont();
  /*! <pre>
  read font file
  </pre> */
  int  read(const char *filename);
  /*! <pre>
  get lineHeight
  </pre> */
  int  lineHeight();
  /*! <pre>
  get width of a character
  </pre> */
  int  charWidth(unsigned char c);
  /*! <pre>
  get width of string
  </pre> */
  int  stringWidth(const char *str);
  /*! <pre>
  draw string at position x,y
  push matrix, translate and rotate first if you want to place it in 3D
  </pre> */
  void drawString(float x, float y, const char *str);
  /*! <pre>
  zoom all texts. default: zoom=1.0
  </pre> */
  void setZoom(float factor);
  /*! <pre>
  rotate font in degree angle.
  </pre> */
  void setRotation(int angle);
  /*! <pre>
  for internal use only.
  </pre> */
  void setFontSize(int pitch, float factor);

private:
  // Information about a glyph. Tex_y2 can be calculated from tex_y1
  // and _tex_line_height (see below). Advance is the width of the
  // glyph in screen space.
  struct Glyph
  {
    float tex_x1, tex_y1, tex_x2;
    int advance;
  };
  // An array to store the glyphs.
  //Glyph* _glyphs;
  Glyph  _glyphs[256];
  // A table to quickly get the glyph belonging to a character.
  Glyph* _table[256];
  // The line height, in screen space and in texture space, and the
  // OpenGL id of the font texture.
  GLuint _line_height, _texture;
  float _tex_line_height;
  double zoom,zoom0;
  int    angle_128x128;
};
/*! <pre>
send OpenGL file to pvbrowser

return: number of entries within listarray

This file could be generated by our Autocad DWF2OpenGL generator for example.
</pre> */
int pvSendOpenGL(PARAM *p, const char *filename, GLuint *listarray, int max_array, glFont *proportional=NULL, glFont *fixed=NULL);
/*! <pre>
Call this function when you are finished with OpenGL commands
</pre> */
int pvGlEnd(PARAM *p);
/** @} */ // end of group

/** @defgroup Dialogs Dialogs
 *  Some dialogs
 *  @{ */
/*! <pre>
Open a file selection dialog on the client
id_return is send in a TEXT_EVENT
type = FileOpenDialog|FileSaveDialog|FindDirectoryDialog
#FileDialogs.
</pre> */
int pvFileDialog(PARAM *p, int id_return, int type);
/*! <pre>
Open a PopupMenu on the client
id_return is send in a TEXT_EVENT
The Text will be the selected item or ""
Example for text:
menu1,menu2,,menu3
(Two commas means, separator)
You can add a checkbox in front of each entry with "#c(1)"
You can add a icon in front of each entry with "#i(name.png)"
Example for text:
"menu1,,#c(1)#i(icon.png)menu2,#c(0)menu3,#i(icon.png)menu4"
</pre> */
int pvPopupMenu(PARAM *p, int id_return, const char *text);
/*! <pre>
Open a MessageBox on the client
id_return is send in a SLIDER_EVENT
type = BoxInformation|BoxWarning|BoxCritical
#MessageBoxTypes.
if buttonX == 0 the button is not shown
#MessageBoxButtons
</pre> */
int pvMessageBox(PARAM *p, int id_return, int type, const char *text, int button0, int button1, int button2);
/*! <pre>
Open a InputDialog on the client
id_return is send in a TEXT_EVENT
if user clicked Cancel TEXT_EVENT will return ""
</pre> */
int pvInputDialog(PARAM *p, int id_return, const char *text, const char *default_text);
/*! <pre>
Open and run a modal dialog box
</pre> */
#define readDataCast int (*)(void *)
#define showDataCast int (*)(PARAM *, void *)
int pvRunModalDialog(PARAM *p, int width, int height, int (*showMask)(PARAM *p), void *userData,
                     int (*readData)(void *d),
                     int (*showData)(PARAM *p, void *d),
                     void *d);
/*! <pre>
For script languages:
pvRunModalDialogScript(...)
mask->show()
pvTerminateModalDialog(...)
</pre> */
int pvRunModalDialogScript(PARAM *p, int width, int height);
/*! <pre>
Terminate the modal dialog box
</pre> */
int pvTerminateModalDialog(PARAM *p);
/*! <pre>
Call this function from a ModalDialog in order to update the base window
</pre> */
int pvUpdateBaseWindow(PARAM *p);
/*! <pre>
Call this function from a script language
pvUpdateBaseWindowOnOff($p,1);
base->readData();
base->showData();
pvUpdateBaseWindowOnOff($p,0);
</pre> */
int pvUpdateBaseWindowOnOff(PARAM *p, int onoff);
/*! <pre>
This function will add a QDockWidget (floating modeless docking dialog)

title       := title of the docking dialog
dock_id     := ID_DOCK_WIDGETS + n . Where n = 0...31 MAX_DOCK_WIDGETS 
root_id     := id of root object. root_id is the id of a widget out of the designed widgets. 
               The root object will be reparent to the dock widget. Thus disappering in the main mask.
allow_close := 0|1 allow the user to close (hide) the dialog
floating    := 0|1 moveable by user
allow_X     := 0|1 allow dock widget to be docked on the according position. 

Functions that apply to dock_id:
pvSetGeometry();
pvMove();
pvResize();
pvHide();
pvShow();

You can control the size of the area occupied for the dock area by specifying the
min/man size property of the root_id widget.
</pre> */
int pvAddDockWidget(PARAM *p,const char *title, int dock_id, int root_id, int allow_close=0, int floating=1, int allow_left=1, int allow_right=0, int allow_top=0, int allow_bottom=0);
/*! <pre>
This function will delete the DockWidget.
You may reuse the widgets within the DockWidget for other purposes.
If delete_widgets == 1 the widgets within the dock will be deleted.
If delete_widgets == 0 the widgets within the dock will be reparent to the main widget of the mask.
</pre> */
int pvDeleteDockWidget(PARAM *p, int dock_id, int delete_widget=0);
/** @} */ // end of group

/** @defgroup QwtPlotWidget QwtPlotWidget
 *  Commands for the QwtPlotWidget
 *  @{ */
/*! <pre>
Set/Overwrite a curve
c = index of curve
count = number of coordinates in x,y
</pre> */
int qpwSetCurveData(PARAM *p, int id, int c, int count, double *x, double *y);
/*! <pre>
Set/Overwrite a curve
See pvXYAllocate
c = index of curve
</pre> */
int qpwSetBufferedCurveData(PARAM *p, int id, int c);
/*! <pre>
replot the widget
</pre> */
int qpwReplot(PARAM *p, int id);
/*! <pre>
Set the title of the plot
</pre> */
int qpwSetTitle(PARAM *p, int id, const char *text);
/*! <pre>
Set the background color of the canvas
</pre> */
int qpwSetCanvasBackground(PARAM *p, int id, int r, int g, int b);
/*! <pre>
val = 0|1
</pre> */
int qpwEnableOutline(PARAM *p, int id, int val);
/*! <pre>
Set outine pen color
</pre> */
int qpwSetOutlinePen(PARAM *p, int id, int r, int g, int b);
/*! <pre>
val = 0|1
</pre> */
int qpwSetAutoLegend(PARAM *p, int id, int val);
/*! <pre>
val = 0|1
</pre> */
int qpwEnableLegend(PARAM *p, int id, int val);
/*! <pre>
pos = Left   |
      Right  |
      Top    |
      Bottom |
      Center |
</pre> */
int qpwSetLegendPos(PARAM *p, int id, int pos);
/*! <pre>
style = #Shape | #Shadow
</pre> */
int qpwSetLegendFrameStyle(PARAM *p, int id, int style);
/*! <pre>
enable grid
</pre> */
int qpwEnableGridXMin(PARAM *p, int id);
/*! <pre>
style = #PenStyle.
</pre> */
int qpwSetGridMajPen(PARAM *p, int id, int r, int g, int b, int style);
/*! <pre>
style = #PenStyle.
</pre> */
int qpwSetGridMinPen(PARAM *p, int id, int r, int g, int b, int style);
/*! <pre>
pos = yLeft | yRight | xBottom | xTop
</pre> */
int qpwEnableAxis(PARAM *p, int id, int pos);
/*! <pre>
pos = yLeft | yRight | xBottom | xTop
</pre> */
int qpwSetAxisTitle(PARAM *p, int id, int pos, const char *text);
/*! <pre>
pos = yLeft | yRight | xBottom | xTop
val = #QwtAutoscale.
</pre> */
int qpwSetAxisOptions(PARAM *p, int id, int pos, int val);
/*! <pre>
pos = yLeft | yRight | xBottom | xTop
</pre> */
int qpwSetAxisMaxMajor(PARAM *p, int id, int pos, int val);
/*! <pre>
pos = yLeft | yRight | xBottom | xTop
</pre> */
int qpwSetAxisMaxMinor(PARAM *p, int id, int pos, int val);
/*! <pre>
index = index of curve 0...
</pre> */
int qpwInsertCurve(PARAM *p, int id, int index, const char *text);
/*! <pre>
index = index of curve 0...
</pre> */
int qpwRemoveCurve(PARAM *p, int id, int index);
/*! <pre>
index = index of curve 0...
style = #PenStyle.
</pre> */
int qpwSetCurvePen(PARAM *p, int id, int index, int r, int g, int b, int width=1, int style=SolidLine);
/*! <pre>
index = index of curve 0...
symbol = #MarkerSymbol.
</pre> */
int qpwSetCurveSymbol(PARAM *p, int id, int index, int symbol, int r1, int g1, int b1,
                                                               int r2, int g2, int b2,
                                                               int w, int h);
/*! <pre>
index = index of curve 0...
pos = yLeft | yRight | xBottom | xTop
</pre> */
int qpwSetCurveYAxis(PARAM *p, int id, int index, int pos);
/*! <pre>
index = index of marker 0...
</pre> */
int qpwInsertMarker(PARAM *p, int id, int index);
/*! <pre>
index = index of marker 0...
style = NoLine | HLine | VLine | Cross
</pre> */
int qpwSetMarkerLineStyle(PARAM *p, int id, int index, int style);
/*! <pre>
index = index of marker 0...
style =
</pre> */
int qpwSetMarkerPos(PARAM *p, int id, int index, float x, float y);
/*! <pre>
index = index of marker 0...
</pre> */
int qpwSetMarkerLabelAlign(PARAM *p, int id, int index, int align);
/*! <pre>
index = index of marker 0...
style =
</pre> */
int qpwSetMarkerPen(PARAM *p, int id, int index, int r, int g, int b, int style);
/*! <pre>
number = Number of Marker
text = Text to out
</pre> */
int qpwSetMarkerLabel(PARAM *p, int id, int number, const char * text);
/*! <pre>
index = index of marker 0...
style =
</pre> */
int qpwSetMarkerFont(PARAM *p, int id, int index, const char *family, int size, int style);
/*! <pre>
index = index of marker 0...
symbol = #MarkerSymbol.
</pre> */
int qpwSetMarkerSymbol(PARAM *p, int id, int index, int symbol, int r1, int g1, int b1,
                                                                int r2, int g2, int b2,
                                                                int w, int h);
/*! <pre>
index = index of marker 0...
pos = yLeft | yRight | xBottom | xTop
</pre> */
int qpwInsertLineMarker(PARAM *p, int id, int index, const char *text, int pos);
/*! <pre>
pos = yLeft | yRight | xBottom | xTop
text = format yyyy-MM-dd hh:mm:ss as QDateTime.toString
</pre> */
int qpwSetAxisScaleDraw( PARAM *p, int id, int pos, const char * text );
/*! <pre>
pos = yLeft | yRight | xBottom | xTop
</pre> */
int qpwSetAxisScale( PARAM *p, int id, int pos, float min, float max, float step);
/** @} */ // end of group

/* These fuctions can only be used on a QDrawWidget */
/********* graphic functions ********************************************************/

/** @defgroup Graphics Graphics
 *  These are the graphis routines usable with pyQDrawWidget()
 *  @{ */
/*! <pre>
Zoom the image in X direction. (default: zoom=1.0)
If zoom < 0.0 then keep aspect ratio, factor = |zoom|.
</pre> */
int pvSetZoomX(PARAM *p, int id, float zoom);
/*! <pre>
Zoom the image in Y direction. (default: zoom=1.0)
If zoom < 0.0 then keep aspect ratio, factor = |zoom|.
</pre> */
int pvSetZoomY(PARAM *p, int id, float zoom);
/*! <pre>
Call this function before you write graphical commands to a file.
</pre> */
int gWriteFile(const char *file);
/*! <pre>
Call this function after you have written graphical commands to a file.
</pre> */
int gCloseFile();
/*! <pre>
Call this function before you start drawing the widget.
</pre> */
int gBeginDraw(PARAM *p, int id);
/*! <pre>
Draws a rectangle frame. Later you can use it's coordinates to draw xAxis, yAxiy and ryAxis.
</pre> */
int gBox(PARAM *p, int x, int y, int w, int h);
/*! <pre>
Draws a filled rectangle.
</pre> */
int gRect(PARAM *p, int x, int y, int w, int h);
/*! <pre>
Call this function when you are finished with drawing.
</pre> */
int gEndDraw(PARAM *p);
/*! <pre>
Draw a line from the actual position to x,y.
</pre> */
int gLineTo(PARAM *p, int x, int y);
/*! <pre>
Draw a line in a Axis.
See pvXYAllocate
See also #Linestyle.
</pre> */
int gBufferedLine(PARAM *p);
/*! <pre>
Draw a line in a Axis n=number of values in x,y.
See also #Linestyle.
</pre> */
int gLine(PARAM *p, float *x, float *y, int n);
/*! <pre>
Move to x,y.
</pre> */
int gMoveTo(PARAM *p, int x, int y);
/*! <pre>
Draw a Axis on the right side of the diagram.
It starts with start in steps of delta until end is reached.
</pre> */
int gRightYAxis(PARAM *p, float start, float delta, float end, int draw);
/*! <pre>
Set the drawing color.
</pre> */
int gSetColor(PARAM *p, int r, int g, int b);
/*! <pre>
Set the line width
</pre> */
int gSetWidth(PARAM *p, int w);
/*! <pre>
style = #PenStyle.
</pre> */
int gSetStyle(PARAM *p, int style);
/*! <pre>
Draw an arc. For circle write:
  gDrawArc(p,x-radius/2,y-radius/2,radius/2,radius/2,0,360);
</pre> */
int gDrawArc(PARAM *p, int x, int y, int w, int h, int start_angle, int angle_length);
/*! <pre>
Draw an pie (filled part of a circle) . For filled circle write:
  gDrawArc(p,x-radius/2,y-radius/2,radius/2,radius/2,0,360);
</pre> */
int gDrawPie(PARAM *p, int x, int y, int w, int h, int start_angle, int angle_length);
/*! <pre>
x,y is an array with the edge points.
n is the number of points
</pre> */
int gDrawPolygon(PARAM *p, int *x, int *y, int n);
/*! <pre>
Set a font. For the availabe fonts see Definitions (Events, Fonts, Colors ...)
</pre> */
int gSetFont(PARAM *p, const char *family, int size, int weight, int italic);
/*! <pre>
Set the linestyle of a line in the Axis. You can draw a simple line or a line with centered symbols.
</pre> */
int gSetLinestyle(PARAM *p, int style);
/*! <pre>
Draw a text at x,y.
For alignment see Definitions (Events, Fonts, Colors ...)
</pre> */
int gText(PARAM *p, int x, int y, const char *text, int alignment);
/*! <pre>
Draw a text at x,y in Axis.
For alignment see Definitions (Events, Fonts, Colors ...)
</pre> */
int gTextInAxis(PARAM *p, float x, float y, const char *text, int alignment);
/*! <pre>
Set the format in which the Axis is drawn.
default: "%f"
For example set
"%5.2f"
if you want two digits behind the .
</pre> */
int gSetFloatFormat(PARAM *p, const char *text);
/*! <pre>
Draw a Axis on the bottom of the diagram.
It starts with start in steps of delta until end is reached.
</pre> */
int gXAxis(PARAM *p, float start, float delta, float end, int draw);
/*! <pre>
Draw a Axis on the left side of the diagram.
It starts with start in steps of delta until end is reached.
</pre> */
int gYAxis(PARAM *p, float start, float delta, float end, int draw);
/*! <pre>
Draw a grid orthogonal to the x-axis in the diagram.
</pre> */
int gXGrid(PARAM *p);
/*! <pre>
Draw a grid orthogonal to the y-axis in the diagram.
</pre> */
int gYGrid(PARAM *p);
/*! <pre>
This is a convenience function that draws a box and write the xlabel ylabel and rylabel
in the given font.
If one of the text parameters is NULL no text is drawn for that item.
</pre> */
int gBoxWithText(PARAM *p, int x, int y, int w, int h, int fontsize, const char *xlabel, const char *ylabel, const char *rylabel);
/*! <pre>
Write a comment in the metafile
</pre> */
int gComment(PARAM *p, const char *comment);
/*! <pre>
Play SVG file
You have to download the file first using:
int pvDownloadFile(PARAM *p, const char *filename);

Attention:
This is a deprecated function.
It only works with the Qt3 version of pvbrowser client.
Now use rlSvgAnumator from rllib.
</pre> */
int gPlaySVG(PARAM *p, const char *filename);
/*! <pre>
Play SVG string over the socket

Attention:
This is a deprecated function.
It only works with the Qt3 version of pvbrowser client.
Now use rlSvgAnumator from rllib.
</pre> */
int gSocketPlaySVG(PARAM *p, const char *svgstring);
/*! <pre>
Translate the graphic
</pre> */
int gTranslate(PARAM *p, float x, float y);
/*! <pre>
Rotate the graphic
Rotates the coordinate system angle degrees counterclockwise.
</pre> */
int gRotate(PARAM *p, float angle);
/*! <pre>
Scale the graphic
</pre> */
int gScale(PARAM *p, float sx, float sy);
/*! <pre>
Set Selector in QDraw Widget 0|1
default 1
</pre> */
int pvSetSelector(PARAM *p, int id, int state);
/*! <pre>
Print the SVG on the QDraw widget on a printer
</pre> */
int pvPrintSvgOnPrinter(PARAM *p, int id);

/** @} */ // end of group

/** @defgroup QwtScale QwtScale
 *  QwtScale Widget
 *  @{ */
/*! <pre>
</pre> */
int qwtScaleSetTitle(PARAM *p, int id, const char *text);
/*! <pre>
</pre> */
int qwtScaleSetTitleColor(PARAM *p, int id, int r, int g, int b);
/*! <pre>
</pre> */
int qwtScaleSetTitleFont(PARAM *p, int id, const char *family, int pointsize, int bold, int italic, int underline, int strikeout);
/*! <pre>
#ScalePosition.
</pre> */
int qwtScaleSetTitleAlignment(PARAM *p, int id, int flags);
/*! <pre>
</pre> */
int qwtScaleSetBorderDist(PARAM *p, int id, int start, int end);
/*! <pre>
</pre> */
int qwtScaleSetBaselineDist(PARAM *p, int id, int bd);
/*! <pre>
</pre> */
int qwtScaleSetScaleDiv(PARAM *p, int id, float lBound, float hBound, int maxMaj, int maxMin, int log, float step, int ascend);
/*! <pre>
</pre> */
int qwtScaleSetLabelFormat(PARAM *p, int id, int f, int prec, int fieldWidth);
/*! <pre>
#ScalePosition.
</pre> */
int qwtScaleSetLabelAlignment(PARAM *p, int id, int align);
/*! <pre>
</pre> */
int qwtScaleSetLabelRotation(PARAM *p, int id, float rotation);
/*! <pre>
#ScalePosition.
</pre> */
int qwtScaleSetPosition(PARAM *p, int id, int position);
/** @} */ // end of group
/** @defgroup QwtThermo QwtThermo
 *  QwtThermo Widget
 *  @{ */
/*! <pre>
logarithmic = 0 | 1
</pre> */
int qwtThermoSetScale(PARAM *p, int id, float min, float max, float step, int logarithmic);
/*! <pre>
orientation = HORIZONTAL|VERTICAL
#ThermoPosition.
</pre> */
int qwtThermoSetOrientation(PARAM *p, int id, int orientation, int position);
/*! <pre>
</pre> */
int qwtThermoSetBorderWidth(PARAM *p, int id, int width);
/*! <pre>
</pre> */
int qwtThermoSetFillColor(PARAM *p, int id, int r, int g, int b);
/*! <pre>
</pre> */
int qwtThermoSetAlarmColor(PARAM *p, int id, int r, int g, int b);
/*! <pre>
</pre> */
int qwtThermoSetAlarmLevel(PARAM *p, int id, float level);
/*! <pre>
</pre> */
int qwtThermoSetAlarmEnabled(PARAM *p, int id, int tf);
/*! <pre>
</pre> */
int qwtThermoSetPipeWidth(PARAM *p, int id, int width);
/*! <pre>
</pre> */
int qwtThermoSetRange(PARAM *p, int id, float vmin, float vmax, float step=0.0f);
/*! <pre>
</pre> */
int qwtThermoSetMargin(PARAM *p, int id, int margin);
/*! <pre>
</pre> */
int qwtThermoSetValue(PARAM *p, int id, float value);
/** @} */ // end of group
/** @defgroup QwtKnob QwtKnob
 *  QwtKnob Widget
 *  @{ */
/*! <pre>
logarithmic = 0 | 1
</pre> */
int qwtKnobSetScale(PARAM *p, int id, float min, float max, float step, int logarithmic);
/*! <pre>
</pre> */
int qwtKnobSetMass(PARAM *p, int id, float mass);
/*! <pre>
orientation = HORIZONTAL|VERTICAL
</pre> */
int qwtKnobSetOrientation(PARAM *p, int id, int orientation);
/*! <pre>
rdonly = 0 | 1
</pre> */
int qwtKnobSetReadOnly(PARAM *p, int id, int rdonly);
/*! <pre>
</pre> */
int qwtKnobSetKnobWidth(PARAM *p, int id, int width);
/*! <pre>
</pre> */
int qwtKnobSetTotalAngle(PARAM *p, int id, float angle);
/*! <pre>
</pre> */
int qwtKnobSetBorderWidth(PARAM *p, int id, int width);
/*! <pre>
#KnobSymbol.
</pre> */
int qwtKnobSetSymbol(PARAM *p, int id, int symbol);
/*! <pre>
</pre> */
int qwtKnobSetValue(PARAM *p, int id, float value);
/** @} */ // end of group
/** @defgroup QwtCounter QwtCounter
 *  QwtCounter Widget
 *  @{ */
/*! <pre>
</pre> */
int qwtCounterSetStep(PARAM *p, int id, float step);
/*! <pre>
</pre> */
int qwtCounterSetMinValue(PARAM *p, int id, float value);
/*! <pre>
</pre> */
int qwtCounterSetMaxValue(PARAM *p, int id, float value);
/*! <pre>
</pre> */
int qwtCounterSetStepButton1(PARAM *p, int id, int n);
/*! <pre>
</pre> */
int qwtCounterSetStepButton2(PARAM *p, int id, int n);
/*! <pre>
</pre> */
int qwtCounterSetStepButton3(PARAM *p, int id, int n);
/*! <pre>
</pre> */
int qwtCounterSetNumButtons(PARAM *p, int id, int n);
/*! <pre>
#CounterButton.
</pre> */
int qwtCounterSetIncSteps(PARAM *p, int id, int button, int n);
/*! <pre>
</pre> */
int qwtCounterSetValue(PARAM *p, int id, float value);
/** @} */ // end of group
/** @defgroup QwtWheel QwtWheel
 *  QwtWheel Widget
 *  @{ */
/*! <pre>
</pre> */
int qwtWheelSetMass(PARAM *p, int id, float mass);
/*! <pre>
orientation = HORIZONTAL|VERTICAL
</pre> */
int qwtWheelSetOrientation(PARAM *p, int id, int orientation);
/*! <pre>
rdonly = 0 | 1
</pre> */
int qwtWheelSetReadOnly(PARAM *p, int id, int rdonly);
/*! <pre>
</pre> */
int qwtWheelSetTotalAngle(PARAM *p, int id, float angle);
/*! <pre>
</pre> */
int qwtWheelSetTickCnt(PARAM *p, int id, int cnt);
/*! <pre>
</pre> */
int qwtWheelSetViewAngle(PARAM *p, int id, float angle);
/*! <pre>
</pre> */
int qwtWheelSetInternalBorder(PARAM *p, int id, int width);
/*! <pre>
</pre> */
int qwtWheelSetWheelWidth(PARAM *p, int id, int width);
/*! <pre>
</pre> */
int qwtWheelSetValue(PARAM *p, int id, float value);
/** @} */ // end of group
/** @defgroup QwtSlider QwtSlider
 *  QwtSlider Widget
 *  @{ */
/*! <pre>
logarithmic = 0 | 1
</pre> */
int qwtSliderSetScale(PARAM *p, int id, float min, float max, float step, int logarithmic);
/*! <pre>
</pre> */
int qwtSliderSetMass(PARAM *p, int id, float mass);
/*! <pre>
orientation = HORIZONTAL|VERTICAL
</pre> */
int qwtSliderSetOrientation(PARAM *p, int id, int orientation);
/*! <pre>
rdonly = 0 | 1
</pre> */
int qwtSliderSetReadOnly(PARAM *p, int id, int rdonly);
/*! <pre>
#SliderBGSTYLE.
</pre> */
int qwtSliderSetBgStyle(PARAM *p, int id, int style);
/*! <pre>
</pre> */
int qwtSliderSetScalePos(PARAM *p, int id, int pos);
/*! <pre>
</pre> */
int qwtSliderSetThumbLength(PARAM *p, int id, int length);
/*! <pre>
</pre> */
int qwtSliderSetThumbWidth(PARAM *p, int id, int width);
/*! <pre>
</pre> */
int qwtSliderSetBorderWidth(PARAM *p, int id, int width);
/*! <pre>
</pre> */
int qwtSliderSetMargins(PARAM *p, int id, int x, int y);
/*! <pre>
</pre> */
int qwtSliderSetValue(PARAM *p, int id, float value);
/** @} */ // end of group
/** @defgroup QwtCompass QwtCompass
 *  QwtCompass Widget
 *  @{ */
/*! <pre>
</pre> */
int qwtCompassSetSimpleCompassRose(PARAM *p, int id, int numThorns, int numThornLevels, float width=0.2f);
/*! <pre>
</pre> */
int qwtCompassSetRange(PARAM *p, int id, float vmin, float vmax, float step=0.0f);
/*! <pre>
</pre> */
int qwtCompassSetMass(PARAM *p, int id, float mass);
/*! <pre>
rdonly = 0 | 1
</pre> */
int qwtCompassSetReadOnly(PARAM *p, int id, int rdonly);
/*! <pre>
#DialShadow.
</pre> */
int qwtCompassSetFrameShadow(PARAM *p, int id, int shadow);
/*! <pre>
</pre> */
int qwtCompassShowBackground(PARAM *p, int id, int show);
/*! <pre>
</pre> */
int qwtCompassSetLineWidth(PARAM *p, int id, int width);
/*! <pre>
#DialMode.
</pre> */
int qwtCompassSetMode(PARAM *p, int id, int mode);
/*! <pre>
</pre> */
int qwtCompassSetWrapping(PARAM *p, int id, int wrap);
/*! <pre>
</pre> */
int qwtCompassSetScale(PARAM *p, int id, int maxMajIntv, int maxMinIntv, float step);
/*! <pre>
</pre> */
int qwtCompassSetScaleArc(PARAM *p, int id, float min, float max);
/*! <pre>
</pre> */
int qwtCompassSetOrigin(PARAM *p, int id, float o);
/*! <pre>
which = #CompassNeedle.
</pre> */
int qwtCompassSetNeedle(PARAM *p, int id, int which, int r1=0, int g1=0, int b1=0, int r2=255, int g2=255, int b2=255, int r3=128, int g3=128, int b3=128);
/*! <pre>
</pre> */
int qwtCompassSetValue(PARAM *p, int id, float value);
/** @} */ // end of group
/** @defgroup QwtDial QwtDial
 *  QwtDial Widget
 *  @{ */
/*! <pre>
obsolete, no longer used
</pre> */
int qwtDialSetRange(PARAM *p, int id, float vmin, float vmax, float step=0.0f);
/*! <pre>
obsolte, no longer used
</pre> */
int qwtDialSetMass(PARAM *p, int id, float mass);
/*! <pre>
rdonly = 0 | 1
</pre> */
int qwtDialSetReadOnly(PARAM *p, int id, int rdonly);
/*! <pre>
#DialShadow.
</pre> */
int qwtDialSetFrameShadow(PARAM *p, int id, int shadow);
/*! <pre>
</pre> */
int qwtDialShowBackground(PARAM *p, int id, int show);
/*! <pre>
</pre> */
int qwtDialSetLineWidth(PARAM *p, int id, int width);
/*! <pre>
#DialMode.
</pre> */
int qwtDialSetMode(PARAM *p, int id, int mode);
/*! <pre>
</pre> */
int qwtDialSetWrapping(PARAM *p, int id, int wrap);
/*! <pre>
parameter step is no longer used
</pre> */
int qwtDialSetScale(PARAM *p, int id, int maxMajIntv, int maxMinIntv, float step);
/*! <pre>
</pre> */
int qwtDialSetScaleArc(PARAM *p, int id, float min, float max);
/*! <pre>
</pre> */
int qwtDialSetOrigin(PARAM *p, int id, float o);
/*! <pre>
which = #DialNeedle.
</pre> */
int qwtDialSetNeedle(PARAM *p, int id, int which, int r1=0, int g1=0, int b1=0, int r2=255, int g2=255, int b2=255, int r3=128, int g3=128, int b3=128);
/*! <pre>
</pre> */
int qwtDialSetValue(PARAM *p, int id, float value);
/** @} */ // end of group
/** @defgroup QwtAnalogClock QwtAnalogClock
 *  QwtAnalogClock Widget
 *  @{ */
/*! <pre>
</pre> */
int qwtAnalogClockSetTime(PARAM *p, int id, int hour, int minute, int second);
/*! <pre>
</pre> */
int qwtAnalogClockSetMass(PARAM *p, int id, float mass);
/*! <pre>
rdonly = 0 | 1
</pre> */
int qwtAnalogClockSetReadOnly(PARAM *p, int id, int rdonly);
/*! <pre>
#DialShadow.
</pre> */
int qwtAnalogClockSetFrameShadow(PARAM *p, int id, int shadow);
/*! <pre>
</pre> */
int qwtAnalogClockShowBackground(PARAM *p, int id, int show);
/*! <pre>
</pre> */
int qwtAnalogClockSetLineWidth(PARAM *p, int id, int width);
/*! <pre>
#DialMode.
</pre> */
int qwtAnalogClockSetMode(PARAM *p, int id, int mode);
/*! <pre>
</pre> */
int qwtAnalogClockSetWrapping(PARAM *p, int id, int wrap);
/*! <pre>
</pre> */
int qwtAnalogClockSetScale(PARAM *p, int id, int maxMajIntv, int maxMinIntv, float step);
/*! <pre>
</pre> */
int qwtAnalogClockSetScaleArc(PARAM *p, int id, float min, float max);
/*! <pre>
</pre> */
int qwtAnalogClockSetOrigin(PARAM *p, int id, float o);
/*! <pre>
which = #CompassNeedle.
if(which==QwtCompassNeedle1)    QwtCompassNeedle1(r1,g1,b1);
if(which==QwtCompassNeedle2)    QwtCompassNeedle2(r1,g1,b1, r2,g2,b2);
if(which==QwtCompassNeedle3)    QwtCompassNeedle3(r1,g1,b1, r2,g2,b2);
if(which==QwtCompassNeedle4)    QwtCompassNeedle4(r1,g1,b1, r2,g2,b2, r3,g3,b3);
if(which==QwtCompassLineNeedle) QwtCompassLineNeedle(r1,g1,b1);
</pre> */
int qwtAnalogClockSetNeedle(PARAM *p, int id, int which, int r1=0, int g1=0, int b1=0, int r2=255, int g2=255, int b2=255, int r3=128, int g3=128, int b3=128);
/*! <pre>
</pre> */
int qwtAnalogClockSetValue(PARAM *p, int id, float value);
/** @} */ // end of group

/** @defgroup UnitConversion UnitConversion
 *  unit
 *  @{ */
/*! <pre>
#UNIT_CONVERSION.
</pre> */
float unit(PARAM *p, float val, int conversion);
/** @} */ // end of group

/** @defgroup  TextEvents TextEvents
 *  These are parsing functions for TEXT_EVENT
 *  <PRE>
 *  Example for useing TEXT_EVENTS:
 *  ------------------------------------------
 *  static int slotTextEvent(PARAM *p, int id, DATA *d, const char *text)
 *  {
 *    if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
 *    float x,y,w,h;
 *    float m11,m12,m21,m22,det,dx,dy;
 *    switch(textEventType(text))
 *    {
 *      case PLAIN_TEXT_EVENT:
 *        printf("plain\n");
 *        break;
 *      case WIDGET_GEOMETRY:
 *        int X,Y,W,H;
 *        getGeometry(text,&X,&Y,&W,&H);
 *        printf("geometry(%d)=%d,%d,%d,%d\n",id,X,Y,W,H);
 *        break;
 *      case PARENT_WIDGET_ID:
 *        int PID;
 *        getParentWidgetId(text,&PID);
 *        printf("parent(%d)=%d\n",id,PID);
 *        break;
 *      case SVG_LEFT_BUTTON_PRESSED:
 *        printf("left pressed\n");
 *        printf("objectname=%s\n",svgObjectName(text));
 *        break;
 *      case SVG_MIDDLE_BUTTON_PRESSED:
 *        printf("middle pressed\n");
 *        printf("objectname=%s\n",svgObjectName(text));
 *        break;
 *      case SVG_RIGHT_BUTTON_PRESSED:
 *        printf("right pressed\n");
 *        printf("objectname=%s\n",svgObjectName(text));
 *        break;
 *      case SVG_LEFT_BUTTON_RELEASED:
 *        printf("left released\n");
 *        printf("objectname=%s\n",svgObjectName(text));
 *        break;
 *      case SVG_MIDDLE_BUTTON_RELEASED:
 *        printf("middle released\n");
 *        printf("objectname=%s\n",svgObjectName(text));
 *        break;
 *      case SVG_RIGHT_BUTTON_RELEASED:
 *        printf("right released\n");
 *        break;
 *      case SVG_BOUNDS_ON_ELEMENT:
 *        getSvgBoundsOnElement(text, &x, &y, &w, &h);
 *        printf("bounds object=%s xywh=%f,%f,%f,%f\n",svgObjectName(text),x,y,w,h);
 *        break;
 *      case SVG_MATRIX_FOR_ELEMENT:
 *        getSvgMatrixForElement(text, &m11, &m12, &m21, &m22, &det, &dx, &dy);
 *        printf("matrix object=%s m=%f,%f,%f,%f det=%f dx=%f dy=%f\n",svgObjectName(text),
 *                                 m11,m12,m21,m22,det,dx,dy);
 *        break;
 *      default:
 *        printf("default\n");
 *        break;
 *    }
 *    return 0;
 *  }
 *  </PRE>
 *
 *  @{ */
/*! <pre>
Return the enum #TextEvents
Allowed widgets: QDrawWidget
</pre> */
int textEventType(const char *text);
/*! <pre>
Returns the object name within a SVG graphic from a TEXT_EVENT
or "" if not found
Allowed widgets: QDrawWidget
</pre> */
const char *svgObjectName(const char *text);
/*! <pre>
Returns the bounds on object within a SVG graphic from a TEXT_EVENT
Allowed widgets: QDrawWidget
</pre> */
int getSvgBoundsOnElement(const char *text, float *x, float *y, float *width, float *height);
/*! <pre>
Returns the matrix for object within a SVG graphic from a TEXT_EVENT
Allowed widgets: QDrawWidget
</pre> */
int getSvgMatrixForElement(const char *text, float *m11, float *m12, float *m21, float *m22,
                                             float *det, float *dx,  float *dy);
/*! <pre>
Returns the geometry for a widget from a TEXT_EVENT
Allowed widgets: all widgets
</pre> */
int getGeometry(const char *text, int *x, int *y, int *width, int *height);
/*! <pre>
Returns the parent id for a widget from a TEXT_EVENT
Allowed widgets: all widgets
</pre> */
int getParentWidgetId(const char *text, int *parent);
/** @} */ // end of group

/** @defgroup Classe Classes from pvslib
 *  Here are classes for the pvslib.
 *  @{ */
/*! <pre>
Use this class for manageing p->num_additional_widgets by widget names
Example:

typedef struct // (todo: define your data structure here)
{
  pvWidgetIdManager mgr;
}
DATA;

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  int id;
          
  d->mgr.init(p,ID_END_OF_WIDGETS);

  id = d->mgr.newId("test1");
  pvQPushButton(p,id,0);
  pvSetGeometry(p,id,270,40,100,30);
  pvSetText(p,id,"test1");
  pvShow(p,id);

  return 0;
}                                                      }

static int slotButtonPressedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  if(id == obj1)
  {
    printf("knowns id's:\n");
    int i = d->mgr.first();
    while(i > 0)
    {
      printf("name=%s id=%d\n", d->mgr.name(i),i);
      i = d->mgr.next();
    }
  }
  return 0;
}

</pre> */
#ifndef __VMS
#include <string.h>
#include <string>
#include <iostream>
#include <map>

class pvWidgetIdManager
{
public:
  pvWidgetIdManager();
  virtual ~pvWidgetIdManager();
  int init(PARAM *p, int id_start);
  virtual int newId(const char *name);
  virtual int deleteWidget(PARAM *p, const char *name);
  virtual int id(const char *name);
  virtual int isInMap(const char *name);
  int isInMap(int id);
  int firstId();
  int nextId();
  int endId();
  const char *name(int id);
  int idStart();
  virtual int readEnumFromMask(const char *maskname);
private:
  int insertBasicId(int id, const char *name);
  int id_start, num_additional_widgets;
  int *free;
  std::map      <std::string, int>           id_list;  
  std::multimap <std::string, int>::iterator it;
};
#endif
/** @} */ // end of group

#endif
