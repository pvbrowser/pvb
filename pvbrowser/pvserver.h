/***************************************************************************
                          processviewserver.h  -  description
                             -------------------
    begin                : Son Nov 12 09:43:38 CET 2000
    copyright            : (C) 2000 by R. Lehrig
                         : Angel Maza
                         : Martin Bangieff
    email                : lehrig@t-online.de
 ***************************************************************************/

/*********************************************************************************
 *                                                                               *
 *   This program is free software; you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by        *
 *   the Free Software Foundation; either version 2 of the License, or           *
 *   (at your option) any later version.                                         *
 *                                                                               *
 ********************************************************************************/

/* Definitions (Events, Fonts, Colors ...) */

#ifndef PVSERVER_H
#define PVSERVER_H

namespace PV
{

/* these are the linestyles used in line(x,y,n) */
enum Linestyle {
LINESTYLE_NONE=0,
LINESTYLE_CIRCLE,
LINESTYLE_CROSS,
LINESTYLE_RECT
};

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
enum fonttype
{
  NORMAL = 0,
  ITALIC = 1
};

/* insertion policies for QComboBox */
enum Policy { NoInsertion=0, AtTop, AtCurrent, AtBottom, AfterCurrent, BeforeCurrent };

/* definition for LCD numbers */
enum Mode         { HEX=0, DEC, OCT, BINx };
enum Mode2        { Hex=0, Dec, Oct, Bin };
enum SegmentStyle { Outline=0, Filled, Flat };

/* definitions for QSlider */
enum Orientation
{
  HORIZONTAL = 0,
  VERTICAL = 1
};

enum Orientation2
{
  Horizontal = 0,
  Vertical = 1
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

enum TextBrowserPos{
Home = 0,
Forward,
Backward};

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
Key_Escape = 0x1000,
Key_Pause = 0x1008,
Key_Print = 0x1009,
Key_SysReq = 0x100a,
Key_PageUp = 0x1016,
Key_PageDown = 0x1017,
Key_F1 = 0x1030,
Key_F2 = 0x1031,
Key_F3 = 0x1032,
Key_F4 = 0x1033,
Key_F5 = 0x1034,
Key_F6 = 0x1035,
Key_F7 = 0x1036,
Key_F8 = 0x1037,
Key_F9 = 0x1038,
Key_F10 = 0x1039,
Key_F11 = 0x103a,
Key_F12 = 0x103b
};

enum QpwLegend      { BottomLegend = 0, TopLegend, LeftLegend, RightLegend};
enum QwtAxis        { yLeft, yRight, xBottom, xTop, axisCnt };
enum QwtAutoscale   {None = 0, IncludeRef = 1, Symmetric = 2, Floating = 4, Logarithmic = 8, Inverted = 16 };
enum ScalePosition  { ScaleLeft, ScaleRight, ScaleTop, ScaleBottom };
enum ThermoPosition { ThermoNone, ThermoLeft, ThermoRight, ThermoTop, ThermoBottom};
enum KnobSymbol     { KnobLine, KnobDot };
enum CounterButton  { CounterButton1, CounterButton2, CounterButton3, CounterButtonCnt };
enum SliderScalePos { SliderNone, SliderLeft, SliderRight, SliderTop, SliderBottom };
enum SliderBGSTYLE  { SliderBgTrough = 0x1, SliderBgSlot = 0x2, SliderBgBoth = SliderBgTrough | SliderBgSlot};
enum DialShadow     { DialPlain = Plain, DialRaised = Raised, DialSunken = Sunken };
enum DialMode       { RotateNeedle, RotateScale };
enum CompassNeedle  { QwtCompassNeedle1 = 1, QwtCompassNeedle2, QwtCompassNeedle3, QwtCompassNeedle4, QwtCompassLineNeedle };
enum PenStyle       { NoPen, SolidLine, DashLine, DotLine, DashDotLine, DashDotDotLine, MPenStyle = 0x0f };
enum MarkerSymbol   { MarkerNone, MarkerEllipse, MarkerRect, MarkerDiamond, MarkerTriangle, MarkerDTriangle, MarkerUTriangle, MarkerLTriangle, MarkerRTriangle, MarkerCross, MarkerXCross, MarkerStyleCnt };

enum Order          { DMY, MDY, YMD, YDM };

};
#endif
