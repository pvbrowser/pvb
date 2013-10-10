/***************************************************************************
                      rlsvganimator.cpp  -  description
                             -------------------
    begin                : Tue Apr 10 2006
    copyright            : (C) 2006 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rlcutil.h"
#include "rlsvganimator.h"
#include "rlsvgcat.h"
#ifndef RLWIN32
#include "rlspawn.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#ifdef RLWIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#define  MSG_NOSIGNAL 0
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "unistd.h"
#endif

#ifdef __VMS
#define  MSG_NOSIGNAL 0
#endif

#ifdef PVMAC
#define  MSG_NOSIGNAL 0
#endif

#define MAXLINE 256*256

rlSvgPosition::rlSvgPosition()
{
  sx = 1.0f;
  alpha = x0 = y0 = cx = cy = 0.0f;
  init.sx = init.alpha = init.x0 = init.y0 = init.w = init.h = -1;
}

rlSvgPosition::rlSvgPosition(float sx_init, float a_init, float x0_init, float y0_init, float w_init, float h_init)
{
  sx = 1.0f;
  alpha = x0 = y0 = cx = cy = 0.0f;
  init.sx = sx_init;
  init.alpha = a_init;
  init.x0 = x0_init;
  init.y0 = y0_init;
  init.w = w_init;
  init.h = h_init;
}

rlSvgPosition::~rlSvgPosition()
{
}

void rlSvgPosition::setInit(float x0_init, float y0_init, float w_init, float h_init)
{
  init.x0 = x0_init;
  init.y0 = y0_init;
  init.w = w_init;
  init.h = h_init;
}

void rlSvgPosition::move(float x, float y)
{
  x0 = x;
  y0 = y;
}

void rlSvgPosition::moveRelative(float dx, float dy)
{
  x0 += dx;
  y0 += dy;
}

void rlSvgPosition::scale(float s)
{
  sx = s;
}

void rlSvgPosition::scaleRelative(float ds)
{
  sx *= ds;
}

void rlSvgPosition::rotate(float _alpha, float _cx, float _cy)
{
  alpha = _alpha;
  cx = _cx;
  cy = _cy;
}


rlSvgAnimator::rlSvgAnimator()
{
  s = NULL;
  id = 0;
  isModified = 1;

  // zoomer follows
  svgX0 = svgY0 = 0.0f;
  svgScale = 1.0f;
  svgWindowWidth  = 640.0f;
  svgWindowHeight = 480.0f;
  svgMouseX0 = svgMouseY0 = 0.0f;
  svgMouseX1 = svgMouseY1 = 0.0f;
  main_object_name.setText("main");
}

rlSvgAnimator::~rlSvgAnimator()
{
}

int rlSvgAnimator::tcpsend(const char *buf, int len)
{
  int ret,bytes_left,first_byte;

  if(s == NULL) return -1;
  //rlDebugPrintf("tcpsend len=%d buf=%s",len,buf);
  bytes_left = len;
  first_byte = 0;
  while(bytes_left > 0)
  {
    if(*s == 1) // use stdout
    {
#ifdef __VMS
      ret = send(1,&buf[first_byte],bytes_left,MSG_NOSIGNAL);
#else
      ret = write(1,&buf[first_byte],bytes_left);
#endif
      if(ret <= 0) 
      {
        *s = -1;
        return -1;
      }
    }
    else // use socket
    {
      ret = send(*s,&buf[first_byte],bytes_left,MSG_NOSIGNAL);
      if(ret <= 0) 
      {
        *s = -1;
        return -1;
      }
    }
    bytes_left -= ret;
    first_byte += ret;
  }
  return 0;
}

int rlSvgAnimator::read(const char *infile, rlIniFile *inifile)
{
  isModified = 1;
#ifdef RLUNIX
  rlSpawn rlsvg;
  int ret, use_stdout;
  char command[MAXLINE];
  const char *line;

  inifileState = 0;
  inifileCount = 0;
  if(inifile == NULL) use_stdout = 0;
  use_stdout = 0;
  if(s != NULL && *s == 1) use_stdout = 1;

  rlDebugPrintf("rlSvgAnimator step=1 infile=%s\n",infile);
  if(use_stdout == 0) 
  {
    sprintf(command,"rlsvgcat %s",infile);
    ret = rlsvg.spawn(command);
    if(ret < 0) return -1;
  }

  rlDebugPrintf("rlSvgAnimator step=2\n");

  if(id != 0 && s != NULL) // new style programming
  {
    sprintf(command,"gsvgRead(%d)\n",id);
    tcpsend(command,strlen(command));
    if(use_stdout == 0)
    {
      while((line = rlsvg.readLine()) != NULL)
      {
        tcpsend(line,strlen(line));
        if(inifile != NULL) fillIniFile(inifile, line);
      }
    }
    else
    {
      sprintf(command,"/usr/bin/rlsvgcat %s",infile);
      ret = system(command);
      if(ret == -1) return -1;
      if(inifile != NULL) 
      {
        char outfile[80];
        sprintf(outfile,"PVTMP%d.svg",*s);
        sprintf(command,"/usr/bin/rlsvgcat %s %s",infile,outfile);
        ret = system(command);
        if(ret == -1) return -1;
        fileFillIniFile(outfile, inifile);
        unlink(outfile);
      }  
    }
    sprintf(command,"\n<svgend></svgend>\n");
    tcpsend(command,strlen(command));
    return 0;
  }
  else 
  {
    return -1;
  }  
#else
  // Windows: use file instead of pipe
  FILE *fin;
  int  ret;
  char filename[1024];
  char cmd[1024];
  char command[MAXLINE];
  char line[MAXLINE];
  char *cptr;

  inifileState = 0;
  inifileCount = 0;
  if(s==NULL) sprintf(filename,"PVTEMP%s",infile);
  else        sprintf(filename,"PVTEMP%d%s",*s,infile);
  sprintf(cmd,"rlsvgcat %s %s",infile,filename);
  ret = system(cmd);
  if(ret < 0) return -1;
  fin = fopen(filename,"r");
  if(fin == NULL) return -1;
  sprintf(command,"gsvgRead(%d)\n",id);
  tcpsend(command,strlen(command));
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    cptr = strchr(line,0x0d);
    if(cptr != NULL) *cptr = '\0';
    cptr = strchr(line,0x0a);
    if(cptr != NULL) *cptr = '\0';
    tcpsend(line,strlen(line));
    tcpsend("\n",strlen("\n"));
    if(inifile != NULL) fillIniFile(inifile, line);
  }
  sprintf(command,"\n<svgend></svgend>\n");
  tcpsend(command, strlen(command));;
  unlink(filename);
  return 0;
#endif
}

int rlSvgAnimator::setSocket(int *socket)
{
  isModified = 1;
  s = socket;
  return 0;
}

int rlSvgAnimator::setId(int _id)
{
  isModified = 1;
  id = _id;
  return 0;
}

int rlSvgAnimator::writeSocket()
{
  char buf[80];
  
  isModified = 0;

  if(s == NULL)
  {
    printf("rlSvgAnimator: ERROR please use setSocket first\n");
    return -1;
  }
  if(id != 0)
  {
    sprintf(buf,"gupdateSVG(%d)\n",id);
    tcpsend(buf, strlen(buf));
    return 0;
  }
  return -1;
}

#define MAXBUF 1024

int rlSvgAnimator::svgPrintf(const char *objectname, const char *tag, const char *format, ...)
{
  char buf[MAXBUF+40];
  char text[MAXBUF];
  int  len = strlen(objectname);

  isModified = 1;
  if(id != 0)
  {
    sprintf(buf,"gsvgPrintf(%d)\n",id);
    tcpsend(buf, strlen(buf));
    sprintf(buf,"%s\n",objectname);
    tcpsend(buf, strlen(buf));
    sprintf(buf,"%s\n",tag);
    tcpsend(buf, strlen(buf));
    va_list ap;
    va_start(ap,format);
#ifdef RLWIN32
    _vsnprintf(text, MAXBUF - 1, format, ap);
#endif
#ifdef __VMS
    vsprintf(text, format, ap);
#endif
#ifdef RLUNIX
    vsnprintf(text, MAXBUF - 1, format, ap);
#endif
    va_end(ap);
    strcat(text,"\n");
    len = strlen(text);
    tcpsend(text, len);
    return len;
  }
  return -1;
}

int rlSvgAnimator::svgRecursivePrintf(const char *objectname, const char *tag, const char *format, ...)
{
  char buf[MAXBUF+40];
  char text[MAXBUF];
  int  len;

  isModified = 1;
  sprintf(buf,"gsvgRecursivePrintf(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",tag);
  tcpsend(buf, strlen(buf));
  va_list ap;
  va_start(ap,format);
#ifdef RLWIN32
  _vsnprintf(text, MAXBUF - 1, format, ap);
#endif
#ifdef __VMS
  vsprintf(text, format, ap);
#endif
#ifdef RLUNIX
  vsnprintf(text, MAXBUF - 1, format, ap);
#endif
  va_end(ap);
  strcat(text,"\n");
  len = strlen(text);
  tcpsend(text, len);
  return len;
}

int rlSvgAnimator::svgSearchAndReplace(const char *objectname, const char *tag, const char *before, const char *after)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgSearchAndReplace(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",tag);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",before);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",after);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::svgRecursiveSearchAndReplace(const char *objectname, const char *tag, const char *before, const char *after)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgRecursiveSearchAndReplace(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",tag);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",before);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",after);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::svgTextPrintf(const char *objectname, const char *format, ...)
{
  char buf[MAXBUF+40];
  char text[MAXBUF];
  int  len = strlen(objectname);

  isModified = 1;
  if(id != 0)
  {
    sprintf(buf,"gsvgTextPrintf(%d)\n",id);
    tcpsend(buf, strlen(buf));
    sprintf(buf,"%s\n",objectname);
    tcpsend(buf, strlen(buf));
    va_list ap;
    va_start(ap,format);
#ifdef RLWIN32
    _vsnprintf(text, MAXBUF - 1, format, ap);
#endif
#ifdef __VMS
    vsprintf(text, format, ap);
#endif
#ifdef RLUNIX
    vsnprintf(text, MAXBUF - 1, format, ap);
#endif
    va_end(ap);
    strcat(text,"\n");
    len = strlen(text);
    tcpsend(text, len);
    return len;
  }
  return -1;
}

int rlSvgAnimator::svgRemoveStyleOption(const char *objectname, const char *option)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgRemoveStyleOption(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n","style=");
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",option);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::svgRecursiveRemoveStyleOption(const char *objectname, const char *option)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgRecursiveRemoveStyleOption(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n","style=");
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",option);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::svgChangeStyleOption(const char *objectname, const char *option, const char *value)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgChangeStyleOption(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n","style=");
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",option);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",value);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::svgRecursiveChangeStyleOption(const char *objectname, const char *option, const char *value)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgRecursiveChangeStyleOption(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n","style=");
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",option);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",value);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::svgSetStyleOption(const char *objectname, const char *value)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgSetStyleOption(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n","style=");
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",value);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::svgRecursiveSetStyleOption(const char *objectname, const char *value)
{
  char buf[MAXBUF+40];

  isModified = 1;
  sprintf(buf,"gsvgRecursiveSetStyleOption(%d)\n",id);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",objectname);
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n","style=");
  tcpsend(buf, strlen(buf));
  sprintf(buf,"%s\n",value);
  tcpsend(buf, strlen(buf));
  return 0;
}

int rlSvgAnimator::show(const char *objectname, int state)
{
  rlDebugPrintf("rlSvgAnimator.show state=%d objectname=%s\n",state,objectname);
  isModified = 1;
  if(id != 0)
  {
    char buf[MAXLINE];
    sprintf(buf,"gsvgShow(%d,%d)\n",id,state);
    //printf("buf=%s",buf);
    tcpsend(buf, strlen(buf));
    sprintf(buf,"%s\n",objectname);
    tcpsend(buf, strlen(buf));
    return 0;
  }
  return -1;
}

int rlSvgAnimator::setMatrix(const char *objectname, rlSvgPosition &pos)
{
  isModified = 1;
  return setMatrix(objectname, pos.sx, pos.alpha, pos.x0, pos.y0, pos.cx, pos.cy);
}

int rlSvgAnimator::setMatrix(const char *objectname, float sx, float alpha, float x0, float y0, float cx_in, float cy_in)
{
  // sx    = scale x
  // alpha = rotation
  // x0,y0 = tranlation
  // cx,cy = rotation center
  //
  // matrix:
  // a11 a12 a13
  // a21 a22 a23
  // a31 a32 a33

  float a11, a12, a13, a21, a22, a23; // a31, a32, a33;
  float cx, cy;

  isModified = 1;
  cx = cx_in + x0;
  cy = cy_in + y0;

  a11 = sx*((float) cos(alpha));
  a12 = -sx*((float) sin(alpha));
  a13 = (x0 - cx)*((float) cos(alpha)) - (y0 - cy)*((float) sin(alpha)) + cx;
  a21 = sx*((float) sin(alpha));
  a22 = sx*((float) cos(alpha));
  a23 = (x0 - cx)*((float) sin(alpha)) + (y0 - cy)*((float) cos(alpha)) + cy;
  //a31 = 0.0f;
  //a32 = 0.0f;
  //a33 = 1.0f;

  return svgPrintf(objectname,"transform=","matrix(%f,%f,%f,%f,%f,%f)",a11,a21,a12,a22,a13,a23);

/*********************************************************************************************

Definition
CTM := CurrentTransformationMatrix
t   := translate
r   := rotate
s   := scale

Equotiation
CTM = t(cx,cy)*r(a)*t(-cx,-cy)*t(x0,y0)*s(sx)

      |1 0 cx| |cos(a) -sin(a) 0| |1 0 -cx| |1 0 x0| |sx 0  0|
CTM = |0 1 cy|*|sin(a)  cos(a) 0|*|0 1 -cy|*|0 1 y0|*|0  sx 0|                            # OK
      |0 0 1 | |  0       0    1| |0 0  1 | |0 0 1 | |0  0  1|

      |1 0 cx| |cos(a) -sin(a) 0| |1 0 -cx| |sx 0  x0|
CTM = |0 1 cy|*|sin(a)  cos(a) 0|*|0 1 -cy|*|0  sx y0|                                    # OK
      |0 0 1 | |  0       0    1| |0 0  1 | |0  0  1 |

      |1 0 cx| |cos(a) -sin(a) 0| |sx 0  x0-cx|
CTM = |0 1 cy|*|sin(a)  cos(a) 0|*|0  sx y0-cy|                                           # OK
      |0 0 1 | |  0       0    1| |0  0  1    |

      |1 0 cx| |sx*cos(a) -sx*sin(a) (x0-cx)*cos(a)-(y0-cy)*sin(a)|
CTM = |0 1 cy|*|sx*sin(a)  sx*cos(a) (x0-cx)*sin(a)+(y0-cy)*cos(a)|                       # OK
      |0 0 1 | |  0          0                        1           |

      |sx*cos(a) -sx*sin(a) (x0-cx)*cos(a)-(y0-cy)*sin(a)+cx|
CTM = |sx*sin(a)  sx*cos(a) (x0-cx)*sin(a)+(y0-cy)*cos(a)+cy|                             # OK
      |  0          0                     1                 |

Check
with sx = 1 and a,x0,y0,cx,cy = 0

      |1 0 0|
CTM = |0 1 0|                                                                             # OK
      |0 0 1|

|x'|         |x|   |sx*cos(a)*x-sx*sin(a)*y+(x0-cx)*cos(a)-(y0-cy)*sin(a)+cx|   |1*x-0*y|
|y'| = CTM * |y| = |sx*sin(a)*x+sx*cos(a)*y+(x0-cx)*sin(a)+(y0-cy)*cos(a)+cy| = |0*x+1*y| # OK
|0 |         |0|   |                       0                                |   |   0   |

**********************************************************************************************/
}

int rlSvgAnimator::fillIniFile(rlIniFile *inifile, const char *line)
{
  char myline[MAXLINE],*cptr, *cptr2;
  const char *id;
  int i;

  strcpy(myline,line);
  cptr = strchr(myline,'\n');
  if(cptr != NULL) *cptr = '\0';

  if(myline[0] == '>' || strncmp(myline,"/>",2) == 0)
  { // end of data -> fill inifile
    if(inifileState == 1) // if id was found
    {
      for(i=0; i<inifileCount; i++)
      {
        id = inifileID.text();
        inifile->setText(id, inifileTable.text(1,i+1), inifileTable.text(2,i+1));
      }
    }
    inifileState = 0;
    inifileCount = 0;
    inifileID.clear();
    inifileTable.clear();
  }
  else if(myline[0] == '<')
  { // ignore this
  }
  else if(strncmp(myline,"id=",3) == 0)
  { // remenber id
    cptr = strchr(myline,'=');
    if(cptr != NULL)
    {
      cptr++; cptr++;
      cptr2 = strchr(cptr,'\"');
      if(cptr2 != NULL) *cptr2 = '\0';
      inifileID.setText(cptr);
      inifileState = 1; // id was found
    }
  }
  else
  { // fill this in table
    cptr = strchr(myline,'=');
    if(cptr != NULL)
    {
      *cptr = '\0';
      cptr++; cptr++;
      cptr2 = strrchr(cptr,'\"');
      if(cptr2 != NULL) *cptr2 = '\0';
      inifileTable.setText(1,inifileCount+1,myline);
      inifileTable.setText(2,inifileCount+1,cptr);
      inifileCount++;
    }
  }

  return 0;
}

int rlSvgAnimator::fileFillIniFile(const char *infile, rlIniFile *inifile)
{
  char line[MAXLINE];
  FILE *fin;

  fin = fopen(infile,"r");
  if(fin == NULL) return -1;
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    fillIniFile(inifile, line);
  }
  fclose(fin);
  return 0;
}

// zoomer methods follows

int rlSvgAnimator::setMainObject(const char *main_object)
{
  isModified = 1;
  if(main_object == NULL) return -1; 
  main_object_name.setText(main_object);
  return 0;
}

const char *rlSvgAnimator::mainObject()
{
  return main_object_name.text();
}

int rlSvgAnimator::setXY0(float x0, float y0)
{
  isModified = 1;
  svgX0 = x0;
  svgY0 = y0;
  return 0;
}

float rlSvgAnimator::x0()
{
  return svgX0;
}

float rlSvgAnimator::y0()
{
  return svgY0;
}

int rlSvgAnimator::setMouseXY0(float x0, float y0)
{
  isModified = 1;
  svgMouseX0 = x0;
  svgMouseY0 = y0;
  return 0;
}

float rlSvgAnimator::mouseX0()
{
  return svgMouseX0;
}

float rlSvgAnimator::mouseY0()
{
  return svgMouseY0;
}

int rlSvgAnimator::setMouseXY1(float x1, float y1)
{
  isModified = 1;
  svgMouseX1 = x1;
  svgMouseY1 = y1;
  return 0;
}

float rlSvgAnimator::mouseX1()
{
  return svgMouseX1;
}

float rlSvgAnimator::mouseY1()
{
  return svgMouseY1;
}

int rlSvgAnimator::setScale(float scale)
{
  isModified = 1;
  svgScale = scale;
  return 0;
}

float rlSvgAnimator::scale()
{
  return svgScale;
}

int rlSvgAnimator::zoomCenter(float newScale)
{
  float oldX0, oldY0;

  isModified = 1;
  if (newScale > 1000) newScale = 1000.0f;

  oldX0 = (svgX0 - ((svgWindowWidth) * (1.0f - svgScale)) / 2.0f) / svgScale;
  oldY0 = (svgY0 - ((svgWindowHeight) * (1.0f - svgScale)) / 2.0f) / svgScale;

  svgScale  = newScale;

  svgX0 = oldX0*svgScale + ((svgWindowWidth) * (1.0f - svgScale)) / 2.0f;
  svgY0 = oldY0*svgScale + ((svgWindowHeight) * (1.0f - svgScale)) / 2.0f;

  return 0;
}

int rlSvgAnimator::zoomRect()
{
  float newScale, scale1, scale2, rectWidth, rectHeight;

  isModified = 1;
  rectWidth  = svgMouseX1-svgMouseX0;
  rectHeight = svgMouseY1-svgMouseY0;
  svgX0 = svgX0 + svgWindowWidth/2  - (svgMouseX0 + rectWidth/2);
  svgY0 = svgY0 + svgWindowHeight/2 - (svgMouseY0 + rectHeight/2);
  rectWidth  = rectWidth  / svgScale;
  rectHeight = rectHeight / svgScale;
  scale1 = svgWindowWidth  / rectWidth;
  scale2 = svgWindowHeight / rectHeight;

  if(scale1 < scale2) newScale = scale1;
  else                newScale = scale2;
  zoomCenter(newScale);

  return 0;
}

int rlSvgAnimator::setMainObjectMatrix()
{
  isModified = 1;
  setMatrix(main_object_name.text(), svgScale, 0.0f, svgX0, svgY0, 0.0f, 0.0f);
  return 0;
}

int rlSvgAnimator::setWindowSize(int width, int height)
{
  isModified = 1;
  svgWindowWidth  = (float) width;
  svgWindowHeight = (float) height;
  return 0;
}

float rlSvgAnimator::windowWidth()
{
  return svgWindowWidth;
}

float rlSvgAnimator::windowHeight()
{
  return svgWindowHeight;
}

int rlSvgAnimator::moveMainObject(float x, float y)
{
  isModified = 1;
  float xx0 = x0() + (x - mouseX0());
  float yy0 = y0() + (y - mouseY0());
  setXY0(xx0,yy0);
  setMainObjectMatrix();
  return 0;
}


