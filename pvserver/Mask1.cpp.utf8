/***************************************************************************
                          Mask1.cpp  -  description
                             -------------------
    begin                : Sun Nov 12 2000
    copyright            : (C) 2000 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "pvapp.h"

#define FORi for(i=0;i<8;i++)
#define CW 10 // character width
#define CH 20 // character height

typedef struct  // I assume that this is our data structure
{
  float speed[8];
  float temp[8];
  float force[8];
  float thick[8];
  float target_thick;
  float meas_thick;
  float thick_dev;
}DATA;

// our mask contains the following objects
enum {
ID_MAIN_WIDGET = 0,
ID_BUTTON_MASK1,ID_BUTTON_MASK2,ID_BUTTON_MASK3,ID_BUTTON_MASK4, ID_BUTTON_PERIODIC, ID_BUTTON_MASKVTK, ID_BUTTON_MASKQWT, ID_BUTTON_REQUEST,
ID_LINE_EDIT,
ID_HEADER,
ID_LINE1,
ID_SPEED_LABEL, ID_SPEED0, ID_SPEED1,ID_SPEED2,ID_SPEED3,ID_SPEED4,ID_SPEED5,ID_SPEED6,ID_SPEED7,
ID_TEMP_LABEL,  ID_TEMP0,  ID_TEMP1, ID_TEMP2, ID_TEMP3, ID_TEMP4, ID_TEMP5, ID_TEMP6, ID_TEMP7,
ID_FORCE_LABEL, ID_FORCE0, ID_FORCE1,ID_FORCE2,ID_FORCE3,ID_FORCE4,ID_FORCE5,ID_FORCE6,ID_FORCE7,
ID_THICK_LABEL, ID_THICK0, ID_THICK1,ID_THICK2,ID_THICK3,ID_THICH4,ID_THICK5,ID_THICK6,ID_THICK7,
ID_LINE2,
ID_TARGET_THICK, ID_MEAS_THICK,ID_THICK_DEV,
ID_LINE3,
ID_TAB_DIALOG, ID_TAB1, ID_TAB2, ID_TAB3, ID_TAB_BUTTON1,
ID_END_OF_WIDGETS
};

static int defineMask(PARAM *p)
{
int i,x,y,obj;

  pvStartDefinition(p,ID_END_OF_WIDGETS);

  // define buttons
  pvQPushButton(p,ID_BUTTON_MASK1,0);
  pvQPushButton(p,ID_BUTTON_MASK2,0);
  pvQPushButton(p,ID_BUTTON_MASK3,0);
  pvQPushButton(p,ID_BUTTON_MASK4,0);
  pvQPushButton(p,ID_BUTTON_PERIODIC,0);
  pvQPushButton(p,ID_BUTTON_MASKVTK,0);
  pvQPushButton(p,ID_BUTTON_MASKQWT,0);
  pvQPushButton(p,ID_BUTTON_REQUEST,0);
  pvQLineEdit(p,ID_LINE_EDIT,0);
  x = 0; y = 0;
  pvSetGeometry(p,ID_BUTTON_MASK1,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_MASK2,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_MASK3,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_MASK4,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_PERIODIC,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_MASKVTK,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_MASKQWT,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_REQUEST,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_LINE_EDIT,x,y,100,30);
  pvSetText(p,ID_BUTTON_MASK1,"Show Mask1");
  pvSetText(p,ID_BUTTON_MASK2,"Show Mask2");
  pvSetText(p,ID_BUTTON_MASK3,"Show Mask3");
  pvSetText(p,ID_BUTTON_MASK4,"Show OpenGL");
  pvSetText(p,ID_BUTTON_PERIODIC,"\"Show Periodic\"");
  pvSetText(p,ID_BUTTON_MASKVTK,"Show VTK");
  pvSetText(p,ID_BUTTON_MASKQWT,"Show QWT");
  pvSetText(p,ID_BUTTON_REQUEST,"Request text's");

  pvToolTip(p,ID_BUTTON_MASK1,"äöüß This is the ToolTip for BUTTON_MASK1耶和华见证人：守望台圣经书社正式网页");

  // define the lines
  pvQLabel(p,ID_LINE1,0);
  pvSetGeometry(p,ID_LINE1,150, 10,800,3);
  pvSetBackgroundColor(p,ID_LINE1,255,255,0);
  pvQLabel(p,ID_LINE2,0);
  pvSetGeometry(p,ID_LINE2,150,180,800,3);
  pvSetBackgroundColor(p,ID_LINE2,255,255,0);
  pvQLabel(p,ID_LINE3,0);
  pvSetGeometry(p,ID_LINE3,150,220,800,3);
  pvSetBackgroundColor(p,ID_LINE3,255,255,0);

  // define the arrays
  obj = ID_SPEED_LABEL;
  x = 150; y = 20;
  pvQLabel(p,obj,0);
  pvSetGeometry(p,obj,x,y,130,30);
  pvPrintf(p,obj,"Speed (m/sec):");
  x += 130;
  obj++;
  FORi
  {
    pvQLabel(p,obj,0);
    pvSetGeometry(p,obj,x,y,50,30);
    x += 80;
    obj++;
  }
  obj = ID_TEMP_LABEL;
  x = 150; y += 30;
  pvQLabel(p,obj,0);
  pvSetGeometry(p,obj,x,y,130,30);
  pvPrintf(p,obj,"Temperature (degC) :");
  x += 130;
  obj++;
  FORi
  {
    pvQLabel(p,obj,0);
    pvSetGeometry(p,obj,x,y,50,30);
    x += 80;
    obj++;
  }
  obj = ID_FORCE_LABEL;
  x = 150; y += 30;
  pvQLabel(p,obj,0);
  pvSetGeometry(p,obj,x,y,130,30);
  pvPrintf(p,obj,"Force (kN):");
  x += 130;
  obj++;
  FORi
  {
    pvQLabel(p,obj,0);
    pvSetGeometry(p,obj,x,y,50,30);
    x += 80;
    obj++;
  }
  obj = ID_THICK_LABEL;
  x = 150; y += 30;
  pvQLabel(p,obj,0);
  pvSetGeometry(p,obj,x,y,130,30);
  pvPrintf(p,obj,"Thickness (mm):");
  x += 130;
  obj++;
  FORi
  {
    pvQLabel(p,obj,0);
    pvSetGeometry(p,obj,x,y,50,30);
    x += 80;
    obj++;
  }

  // define the single values
  obj = ID_TARGET_THICK;
  x = 150; y = 187;
  pvQLabel(p,obj,0);
  pvSetGeometry(p,obj,x,y,250,30);
  obj++; x += 270;
  pvQLabel(p,obj,0);
  pvSetGeometry(p,obj,x,y,250,30);
  obj++; x += 270;
  pvQLabel(p,obj,0);
  pvSetGeometry(p,obj,x,y,250,30);
  obj++; x += 270;

  x = 150; y = 250;
  pvQTabWidget(p,ID_TAB_DIALOG,0);
  pvSetGeometry(p,ID_TAB_DIALOG,x,y,400,200);
  pvSetTabPosition(p,ID_TAB_DIALOG,Bottom);

  pvQWidget(p,ID_TAB1,ID_TAB_DIALOG);
  pvQWidget(p,ID_TAB2,ID_TAB_DIALOG);
  pvQWidget(p,ID_TAB3,ID_TAB_DIALOG);

  pvAddTab(p,ID_TAB_DIALOG,ID_TAB1,"Tab1");
  pvAddTab(p,ID_TAB_DIALOG,ID_TAB2,"Tab2");
  pvAddTab(p,ID_TAB_DIALOG,ID_TAB3,"Tab3");

  pvQPushButton(p,ID_TAB_BUTTON1,ID_TAB1);
  pvSetGeometry(p,ID_TAB_BUTTON1,10,10,100,30);
  pvSetText(p,ID_TAB_BUTTON1,"A TabWidget");

  pvEndDefinition(p);
  return 0;
}

static int showData(PARAM *p, DATA *d)
{
int i,obj;

  obj = ID_SPEED0;
  FORi
  {
    if(d->speed[i] > 0.f && d->speed[i] < 0.2f)
      pvSetBackgroundColor(p,obj,255,0,255);
    if(d->speed[i] > 10.f && d->speed[i] < 10.2f)
      pvSetBackgroundColor(p,obj,255,0,0);
    if(d->speed[i] > 20.f && d->speed[i] < 20.2f)
      pvSetBackgroundColor(p,obj,0,255,0);
    if(d->speed[i] > 30.f && d->speed[i] < 30.2f)
      pvSetBackgroundColor(p,obj,0,0,255);
    pvPrintf(p,obj,"%5.2f",d->speed[i]);
    obj++;
  }
  obj = ID_TEMP0;
  FORi

  {
    pvPrintf(p,obj,"%5.2f",d->temp[i]);
    obj++;
  }
  obj = ID_FORCE0;
  FORi
  {
    pvSetPaletteForegroundColor(p,obj,RED);
    pvPrintf(p,obj,"%5.2f",d->force[i]);
    obj++;
  }
  obj = ID_THICK0;
  FORi
  {
    pvSetPaletteForegroundColor(p,obj,BLUE);
    pvPrintf(p,obj,"%5.2f",d->thick[i]);
    obj++;
  }

  pvPrintf(p,ID_TARGET_THICK,"TargetThickness (mm): %5.2f",d->target_thick);
  pvPrintf(p,ID_MEAS_THICK,  "MeasuredThickness (mm): %5.2f",d->meas_thick);
  pvPrintf(p,ID_THICK_DEV,   "ThicknessDeviation (mm): %5.2f",d->thick_dev);
  return 0;
}

static int animate(float *f)
{
  if(*f < 100.0f && *f >= 0.0f ) *f += 0.1f;
  else                           *f = 0.0f;
  return 0;
}

static int readData(DATA *d) // here we just animate the data
{
  int i;

  FORi
  {
    animate(&d->speed[i]);
    animate(&d->temp[i]);
    animate(&d->force[i]);
    animate(&d->thick[i]);
  }
  animate(&d->target_thick);
  animate(&d->meas_thick);
  animate(&d->thick_dev);
  return 0;
}

int show_mask1(PARAM *p)
{
DATA d;
char event[MAX_EVENT_LENGTH];
int  i,val;
char text[MAX_EVENT_LENGTH];

  defineMask(p);
  memset(&d,0,sizeof(DATA));
  readData(&d); // from shared memory or out of database
  showData(p,&d);
  while(1)
  {
    pvPollEvent(p,event);
    switch(pvParseEvent(event, &i, text))
    {
      case NULL_EVENT:
        readData(&d); // from shared memory or out of database
        showData(p,&d);
        break;
      case BUTTON_EVENT:
        printf("BUTTON_EVENT id=%d text=%s\n",i,text);
        if(i == ID_BUTTON_MASK1)    return 1;
        if(i == ID_BUTTON_MASK2)    return 2;
        if(i == ID_BUTTON_MASK3)    return 3;
        if(i == ID_BUTTON_MASK4)    return 4;
        if(i == ID_BUTTON_PERIODIC) return 5;
        if(i == ID_BUTTON_MASKVTK)  return 6;
        if(i == ID_BUTTON_MASKQWT)  return 7;
        if(i == ID_BUTTON_REQUEST)
        {
          pvText(p,ID_BUTTON_MASK1);
          pvText(p,ID_LINE_EDIT);
          //pvHyperlink(p,"pv://nblehrig");
        }
  if(i == ID_TAB_BUTTON1) pvSendUserEvent(p,1,"hallo welt");
        break;
      case BUTTON_PRESSED_EVENT:
        printf("BUTTON_PRESSED_EVENT id=%d\n",i);
        break;
      case BUTTON_RELEASED_EVENT:
        printf("BUTTON_RELEASED_EVENT id=%d\n",i);
        break;
      case TEXT_EVENT:
        printf("TEXT_EVENT id=%d %s\n",i,text);
        if(i < 0 && text[0] != '\0')
        {
          val = pvRunModalDialog(p,300,200,show_modal1,NULL,
                     (readDataCast) readData,
                     (showDataCast) showData,
                     &d);
          printf("ModalDialog return = %d\n",val);
        }
        break;
      case TAB_EVENT:
        sscanf(text,"(%d)",&val);

        printf("TAB_EVENT(%d,page=%d)\n",i,val);
        break;
      case RIGHT_MOUSE_EVENT:
        printf("RIGHT_MOUSE_EVENT id=%d\n",i);
        pvPopupMenu(p,-1,"Menu1,Menu2,,Menu3");
        break;
      case KEYBOARD_EVENT:
        sscanf(text,"(%d",&val);
        printf("KEYBOARD_EVENT modifier=%d key=%d\n",i,val);
        if(i == NormalKey && val == Key_F1)
        {
          printf("F1 pressed\n");
        }        
        break;
      case USER_EVENT:
        printf("USER_EVENT id=%d %s\n",i,text);
        break;
      default:
        printf("UNKNOWN_EVENT id=%d %s\n",i,text);
        break;
    }
  }
}


