/***************************************************************************
                          Mask2.cpp  -  description
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
#include "pvapp.h"
#include <math.h>

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
ID_LABEL,
ID_IMAGE,ID_LABEL_ON_IMAGE,ID_BUTTON_ON_IMAGE,
ID_TEMP,
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
  pvSetText(p,ID_LABEL,"This is mask 2");

  pvQImage(p,ID_IMAGE,0,"caster.bmp", &w, &h, &depth);
  pvSetGeometry(p,ID_IMAGE,100,0,w,h);

  x = 100+w;
  pvQLabel(p,ID_TEMP,0);
  pvSetGeometry(p,ID_TEMP,x,0,100,20);
  pvSetBackgroundColor(p,ID_TEMP,RED);

  pvQLabel(p,ID_LABEL_ON_IMAGE,ID_IMAGE);
  pvSetGeometry(p,ID_LABEL_ON_IMAGE,350,400,60,30);

  pvQPushButton(p,ID_BUTTON_ON_IMAGE,ID_IMAGE);
  pvSetGeometry(p,ID_BUTTON_ON_IMAGE,350,430,60,30);
  pvSetText(p,ID_BUTTON_ON_IMAGE,"Button");

  pvEndDefinition(p);
  return 0;
}

static int showData(PARAM *p, DATA *d)
{
static int i=0;
static int x=350;
static int y=400;

  if(d == NULL) d = NULL; // compiler please be silent
  i++;
  if(i>=100) i=0;
  pvMove(p,ID_LABEL_ON_IMAGE , x, y-i);
  pvPrintf(p,ID_LABEL_ON_IMAGE,"Cnt: %d",i);
  pvPrintf(p,ID_TEMP," T = %d degC",1120+i);

  return 0;
}

static int readData(DATA *d) // here we just animate the data
{
  if(d == NULL) d = NULL; // compiler please be silent
  return 0;
}

int show_mask2(PARAM *p)
{
DATA d;
char event[MAX_EVENT_LENGTH];
char text[MAX_EVENT_LENGTH];
int  i;

  defineMask(p);
  pvDownloadFile(p,"k3b_success1.wav");
  pvPlaySound(p,"k3b_success1.wav");
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
        if(i == ID_BUTTON_MASK3) return 3;
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


