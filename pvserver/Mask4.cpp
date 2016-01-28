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

// our mask contains the following objects
enum {
ID_MAIN_WIDGET = 0,
ID_BUTTON_MASK1,ID_BUTTON_MASK2,ID_BUTTON_MASK3,
ID_GL,
ID_GROUP,
ID_RADIO1,ID_RADIO2,
ID_CHECK1,ID_CHECK2,
ID_SLIDER1,ID_SLIDER2,ID_SLIDER3,
ID_END_OF_WIDGETS
};

typedef struct
{
  int i;
}DATA;

static PARAM *myparam;
static GLdouble frustSize = 0.5;
static GLdouble frustNear = 1.0;
static GLdouble frustFar  = 200.0;
static GLfloat  scale = 1.5f;
static GLfloat  xRot  = 0.0f;
static GLfloat  yRot  = 0.0f;
static GLfloat  zRot  = 0.0f;
static GLfloat  mat_specular[] = {1.0,1.0,1.0,1.0};
static GLfloat  mat_shininess[] = {50.0};
static GLfloat  light_position[] = {1.0,1.0,1.0,1.0};
static GLfloat  white_light[] = {1.0,1.0,1.0,1.0};
static GLuint listarray[100];
static int num_listarray = 0;
#define SEND_OPEN_GL_EXAMPLE

static void initializeGL()
{
  glClearColor(0.0,1.0,0.0,0.0);		  // Let OpenGL clear to green
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);

  glShadeModel(GL_SMOOTH);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
}

static void initialize2GL() // opengl functions with responses
{
  //pvSendFile(myparam,"scene.gl");
  //pvSendFile(myparam,"scene1.gl");
  //pvSendFile(myparam,"scene2.gl");
#ifdef SEND_OPEN_GL_EXAMPLE
  //listarray[0] = glGenLists(1);
  //glNewList(listarray[0], GL_COMPILE);
  num_listarray = pvSendOpenGL(myparam,"out.txt",listarray,100);
  printf("num_listarray=%d\n", num_listarray);
  //glEndList();
#else
  int ind = 0;

  listarray[ind] = glGenLists(1);
  glNewList(listarray[ind], GL_COMPILE);
  pvSendFile(myparam,"r.gl");
  glEndList();
  ind++;

  listarray[ind] = glGenLists(1);
  glNewList(listarray[ind], GL_COMPILE);
  pvSendFile(myparam,"cherry.gl");
  glEndList();
  ind++;

  num_listarray = ind;
#endif

/*
  glClearColor(0.0,1.0,0.0,0.0);		  // Let OpenGL clear to green
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);
  list = glGenLists(1);
  glNewList( list, GL_COMPILE );
  glColor3f(1.0,0.0,0.0);
  glOrtho(0.0,1.0,0.0,1.0,-1.0,1.0);

  glBegin(GL_POLYGON);
    glVertex3f(0.25,0.25,0.0);
    glVertex3f(0.75,0.25,0.0);
    glVertex3f(0.75,0.75,0.0);
    glVertex3f(0.25,0.75,0.0);
  glEnd();

  glEndList();
  object = list;
*/
}

static void resizeGL(int w, int h)
{
  glViewport( 0, 0, (GLint)w, (GLint)h );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 200.0);
  //glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 200.0);
}

static void paintGL()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glFrustum(-frustSize, frustSize, -frustSize, frustSize, frustNear, frustFar);
  glTranslatef( 0.0, 0.0, -3.0 );
  glScalef( scale, scale, scale );
  glRotatef( xRot, 1.0, 0.0, 0.0 );
  glRotatef( yRot, 0.0, 1.0, 0.0 );
  glRotatef( zRot, 0.0, 0.0, 1.0 );
  for(int i=0; i< num_listarray; i++) glCallList(listarray[i]);
}

static int defineMask(PARAM *p)
{
int  x,y;

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

  // define radio buttons
  pvQButtonGroup(p,ID_GROUP,0,1,HORIZONTAL,"RBut:");
  x = 0; y += 40;
  pvSetGeometry(p,ID_GROUP,x+2,y,95,90);

  pvQRadioButton(p,ID_RADIO1,ID_GROUP);
  pvSetGeometry(p,ID_RADIO1,0,0,90,30);
  pvSetText(p,ID_RADIO1,"Radio1");

  pvQRadioButton(p,ID_RADIO2,ID_GROUP);
  pvSetGeometry(p,ID_RADIO2,0,30,80,30);
  pvSetText(p,ID_RADIO2,"Radio2");

  // define check boxes
  pvQCheckBox(p,ID_CHECK1,0);
  x = 0; y += 90;
  pvSetGeometry(p,ID_CHECK1,x,y,100,30);
  pvSetText(p,ID_CHECK1,"Check1");
  pvQCheckBox(p,ID_CHECK2,0);
  x = 0; y += 30;
  pvSetGeometry(p,ID_CHECK2,x,y,100,30);
  pvSetText(p,ID_CHECK2,"Check2");
  pvSetChecked(p,ID_CHECK2,1);

  // define sliders
  pvQSlider(p,ID_SLIDER1,0,1,10,1,0,VERTICAL);
  pvSetGeometry(p,ID_SLIDER1,750,10,20,200);
  pvQSlider(p,ID_SLIDER2,0,1,10,1,0,VERTICAL);
  pvSetGeometry(p,ID_SLIDER2,780,10,20,200);
  pvQSlider(p,ID_SLIDER3,0,10,500,1,0,VERTICAL);
  pvSetGeometry(p,ID_SLIDER3,810,10,20,200);

  /* define a OpenGL Widget */
  pvQGL(p,ID_GL,0);
  pvSetGeometry(p,ID_GL,100,0,640,480);
  pvGlBegin(p,ID_GL);
  initializeGL();
  resizeGL(640,480);
  pvGlEnd(p);

  pvEndDefinition(p);
  return 0;
}

static int showData(PARAM *p, DATA *d)
{
  if(d == NULL) d = NULL; // compiler please be silent
  
  xRot  += 1.0f;
  if(xRot > 360.0f) xRot = 0.0f;
  yRot  += 1.0f;
  if(yRot > 360.0f) yRot = 0.0f;
  zRot  += 1.0f;
  if(zRot > 360.0f) zRot = 0.0f;

  pvGlBegin(p,ID_GL);
  paintGL();
  pvGlEnd(p);

  pvGlUpdate(p,ID_GL);
  return 0;
}

static int readData(DATA *d) // here we just animate the data
{
  if(d == NULL) d = NULL; // compiler please be silent
  return 0;
}

int show_mask4(PARAM *p)
{
DATA d;
char event[MAX_EVENT_LENGTH];
char text[MAX_EVENT_LENGTH];
int  i,val;
int  w,h;
GLdouble dval;

  myparam = p;
  defineMask(p);
  memset(&d,0,sizeof(DATA));
  readData(&d); // from shared memory or out of database
  showData(p,&d);
  pvGlBegin(p,ID_GL);
  initialize2GL();
  pvGlEnd(p);
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
        //if(i == ID_BUTTON_MASK3) return 3;
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
      case GL_INITIALIZE_EVENT:
        //initializeGL();
        break;
      case GL_PAINT_EVENT:
        pvGlBegin(p,ID_GL);
        paintGL();
        pvGlEnd(p);
        break;
      case GL_RESIZE_EVENT:
        sscanf(text,"(%d,%d)",&w,&h);
        pvGlBegin(p,ID_GL);
        resizeGL(w,h);
        pvGlEnd(p);
        break;
      case GL_IDLE_EVENT:
        break;
      case SLIDER_EVENT:
        sscanf(text,"(%d)",&val);
        dval = val;
        dval /= 50.0;
        if(i==ID_SLIDER1) frustSize = dval;
        if(i==ID_SLIDER2) frustNear = dval;
        if(i==ID_SLIDER3) frustFar  = dval;
        printf("dval=%f\n",(float)dval);
        break;
      case CHECKBOX_EVENT:
        printf("CHECKBOX_EVENT id=%d %s\n",i,text);
        break;
      case RADIOBUTTON_EVENT:
        printf("RADIOBUTTON_EVENT id=%d %s\n",i,text);
        break;
      default:
        printf("UNKNOWN_EVENT id=%d %s\n",i,text);
        break;
    }
  }
}


