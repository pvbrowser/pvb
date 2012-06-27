/***************************************************************************
                          Mask3.cpp  -  description
                             -------------------
    begin                : Mon Nov 13 2000
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
#include <math.h>
#include "pvapp.h"

#define FORi for(int i=0;i<8;i++)
#define CW 10 // character width
#define CH 20 // character height
#define PI 3.141592654f

typedef struct  // I assume that this is our data structure
{
  float x[100];
  float y[100];
  int   n;
}DATA;

// our mask contains the following objects
enum {
ID_MAIN_WIDGET = 0,
ID_BUTTON_MASK1,ID_BUTTON_MASK2,ID_BUTTON_MASK3,
ID_LABEL,ID_IMAGE,
ID_LCD1,ID_LCD2,ID_LCD3,
ID_FRAME,ID_FRAME2,
ID_GRAPHIC,ID_GRAPHIC2,
ID_PRINT,
ID_COMBO1, ID_COMBO2,
ID_END_OF_WIDGETS
};

static int defineMask(PARAM *p)
{
int x,y,w,h,depth;

  pvStartDefinition(p,ID_END_OF_WIDGETS);

  // define buttons
  pvQPushButton(p,ID_BUTTON_MASK1,0);
  pvQPushButton(p,ID_BUTTON_MASK2,0);
  pvQPushButton(p,ID_BUTTON_MASK3,0);
  x = 0; y = 0;
  pvSetGeometry(p,ID_BUTTON_MASK1,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_MASK2,x,y,100,30);
  x = 0; y += 30;
  pvSetGeometry(p,ID_BUTTON_MASK3,x,y,100,30);
  pvSetText(p,ID_BUTTON_MASK1,"Show Mask1");
  pvSetText(p,ID_BUTTON_MASK2,"Show Mask2");
  pvSetText(p,ID_BUTTON_MASK3,"Show Mask3");

  x = 0; y += 30;
  pvQLabel(p,ID_LABEL,0);
  pvSetGeometry(p,ID_LABEL,x,y,100,30);
  pvSetText(p,ID_LABEL,"This is mask 3");

  pvQImage(p,ID_IMAGE,0,"homer.bmp", &w, &h, &depth);
  if(w>0 && h>0) pvSetGeometry(p,ID_IMAGE,100,0,w,h);

  pvQLCDNumber(p,ID_LCD1,0,5,Outline,HEX);
  pvQLCDNumber(p,ID_LCD2,0,5,Filled,DEC);
  pvQLCDNumber(p,ID_LCD3,0,5,Flat,DEC);
  pvSetGeometry(p,ID_LCD1,100+w,0,100,50);
  pvSetGeometry(p,ID_LCD2,100+w,50,100,50);
  pvSetGeometry(p,ID_LCD3,100+w,100,100,50);

  pvQFrame(p,ID_FRAME,ID_IMAGE,Box,Raised,3,3);
  pvSetGeometry(p,ID_FRAME,20,20,200+6,150+6);
  pvQDraw(p,ID_GRAPHIC,ID_FRAME);
  pvSetGeometry(p,ID_GRAPHIC,3,3,200,150);
  pvQPushButton(p,ID_PRINT,ID_GRAPHIC);
  pvSetGeometry(p,ID_PRINT,5,5,50,20);
  pvSetText(p,ID_PRINT,"Print");

  pvQFrame(p,ID_FRAME2,0,Box,Sunken,3,3);
  pvSetGeometry(p,ID_FRAME2,100+w,150,400+6,300+6);
  pvQDraw(p,ID_GRAPHIC2,ID_FRAME2);
  pvSetGeometry(p,ID_GRAPHIC2,3,3,400,300);
  //pvSetGeometry(p,ID_GRAPHIC2,100+w,150,400,300);
  pvSetBackgroundColor(p,ID_GRAPHIC2,128,128,128);

  pvQComboBox(p,ID_COMBO1,0,0,AtTop);
  pvSetGeometry(p,ID_COMBO1,150+w,h-200,200,30);
  pvSetText(p,ID_COMBO1,"one");
  pvSetText(p,ID_COMBO1,"two");
  pvSetText(p,ID_COMBO1,"three");
  pvSetText(p,ID_COMBO1,"four");
  pvSetText(p,ID_COMBO1,"fife");
  pvQComboBox(p,ID_COMBO2,0,1,AtTop);
  pvSetGeometry(p,ID_COMBO2,150+w,h-150,200,30);
  pvSetText(p,ID_COMBO2,"one test");
  pvSetText(p,ID_COMBO2,"two test");
  pvSetText(p,ID_COMBO2,"three test");
  pvSetText(p,ID_COMBO2,"four test");
  pvSetText(p,ID_COMBO2,"fife test");

  pvEndDefinition(p);
  return 0;
}

static int drawGraphic(PARAM *p, int id, DATA *d)
{
int x,y,w,h,fontsize;
int xa[4],ya[4];

  xa[0] = 3;  ya[0] = 60;
  xa[1] = 30; ya[1] = 40;
  xa[2] = 15; ya[2] = 70;
  xa[3] = 3;  ya[3] = 60;

  x = 80;
  y = 30;
  w = 100;
  h = 80;
  fontsize = 10;

  gBeginDraw  (p,id);
  gSetColor   (p,BLACK);
  gSetFont    (p,TIMES,fontsize,Normal,0);
  gBoxWithText(p,x,y,w,h,fontsize,NULL,NULL,NULL);
  gXAxis      (p,0,1.0f,2.0f*PI,1);
  gYAxis      (p,-1.5f,0.5f,1.5f,1);
  gSetColor   (p,RED);
  gSetWidth   (p,4);
  gLine       (p,d->x,d->y,d->n);
  gDrawPolygon(p,xa,ya,4);
  gEndDraw    (p);
  return 0;
}

static int drawGraphic2(PARAM *p, int id, DATA *d)
{
int x,y,w,h,fontsize;

  x = 80;
  y = 50;
  w = 350;
  h = 180;
  fontsize = 10;

  gBeginDraw    (p,id);
  //gSetFloatFormat(p,"%5.2f");
  gComment      (p,"This is a comment in the metafile");
  gSetColor     (p,BLACK);
  gSetFont      (p,TIMES,fontsize,Normal,0);
  gBoxWithText  (p,x,y,w,h,fontsize,"phi/rad","sin(phi)",NULL);
  gXAxis        (p,0,1.0f,2.0f*PI,1);
  gYAxis        (p,-1.5f,0.5f,1.5f,1);
  //gSetStyle     (p,1);
  gSetStyle     (p,DotLine);
  gXGrid        (p);
  gYGrid        (p);
  //gSetStyle     (p,0);
  gSetColor     (p,RED);
  gSetStyle     (p,DashDotLine);
  /* gSetLinestyle (p,LINESTYLE_RECT); */
  gLine         (p,d->x,d->y,d->n);
  gSetColor     (p,BLUE);
  gText         (p,x+w/2,y-fontsize*2,"Date: 19-NOV-2000",ALIGN_CENTER);
  fontsize = 24;
  gSetFont      (p,TIMES,fontsize,Bold,1);
  gText         (p,x+w/2,y-fontsize*2,"This is a Diagram",ALIGN_CENTER);
  gDrawPie      (p,10,10,30,30,0,130);
  gRect         (p,10,50,30,30);
  gEndDraw      (p);
  return 0;
}

static int showData(PARAM *p, DATA *d)
{
char buf[80];
static int   num = 0;
static float val = 0.0f;
static long  mod = 0;
int i;

  drawGraphic    (p,ID_GRAPHIC,d);
  drawGraphic2   (p,ID_GRAPHIC2,d);
  if((mod % 8) == 0)
  {
    num++;
    if(num > 9) num = 0;
    val += 0.1f;
    if(val > 15.0f) val = 0.0f;
    sprintf(buf,"A%d",num);
    pvDisplayNum   (p,ID_LCD1,num);
    pvDisplayFloat (p,ID_LCD2,val);
    pvDisplayStr   (p,ID_LCD3,buf);

    i = num % 3;
    if     (i==0) pvSetImage(p,ID_IMAGE,"homer.bmp.orig");
    else if(i==1) pvSetImage(p,ID_IMAGE,"homer2.bmp");
    else          pvSetImage(p,ID_IMAGE,"homer.bmp.transp");
  }
  mod++;
  if(mod > 1024*1024) mod = 0;
  return 0;
}

static int readData(DATA *d) // here we just animate the data
{
  int i;                  // x    i
  float x;                // 2PI  100
  static float phi = 0.0f;

  phi += 0.1f;
  if(phi > (1000.0f*2.0f*PI)) phi = 0.0f;

  for(i=0; i<100; i++)
  {
    x = (((float) i) * 2.0f * PI) / 100.0f;
    d->x[i] = x;
    d->y[i] = (float) sin(x+phi);
  }
  d->n = 100;
  return 0;
}

int show_mask3(PARAM *p)
{
DATA d;
char event[MAX_EVENT_LENGTH];
char text[MAX_EVENT_LENGTH];
int  i;

  pvDownloadFile(p,"homer.bmp.orig");
  pvDownloadFile(p,"homer.bmp.transp");
  pvDownloadFile(p,"homer2.bmp");
  defineMask(p);
  pvResize(p,ID_MAIN_WIDGET,1280,1024);
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
        printf("BUTTON_EVENT id=%d\n",i);
        if(i == ID_BUTTON_MASK1) return 1;
        if(i == ID_BUTTON_MASK2) return 2;
        if(i == ID_BUTTON_MASK3) pvDownloadFile(p,"homer.bmp.orig");
        if(i == ID_PRINT)        pvPrint(p,ID_GRAPHIC);
        printf("BUTTON_EVENT done\n");
        break;
      case BUTTON_PRESSED_EVENT:
        printf("BUTTON_PRESSED_EVENT id=%d\n",i);
        break;
      case BUTTON_RELEASED_EVENT:
        printf("BUTTON_RELEASED_EVENT id=%d\n",i);
        break;
      case TEXT_EVENT:
        printf("TEXT_EVENT id=%d %s\n",i,text);
        break;
      default:
        printf("UNKNOWN_EVENT id=%d %s\n",i,text);
        break;
    }
  }
}


