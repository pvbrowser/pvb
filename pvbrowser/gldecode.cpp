#include "pvdefine.h"
#define GL_GLEXT_LEGACY
/***************************************************************************
                          gldecode.cpp  -  description
                             -------------------
    begin                : Sun Dec 10 2000
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
/*****************************************************************************
OpenGL decoder for use with Process View Browser    (C) R. Lehrig 2000
                                                    lehrig@t-online.de
*****************************************************************************/

#include "pvdefine.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include "pvglwidget.h"


#ifdef USE_OPEN_GL

#ifdef PVGLWIDGET_H_V5
#include <QOpenGLFunctions>
#else
#ifdef PVMAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

#include "tcputil.h"

#ifndef PVWIN32
#define PVWIN32
#endif
typedef int MyGLenum;

void PvGLWidget::gllongresponse(long l)
{
char buf[80];

  sprintf(buf,"gllongresponse(%ld);\n",l);
  tcp_send(s,buf,strlen(buf));
}

GLvoid *PvGLWidget::glgetbytes()
{
char   line[1024];
int    len;
GLvoid *ptr;

  while(1)
  {
    tcp_rec(s,line,sizeof(line)-1);
    if(strncmp(line,"glbytes(",8) == 0)
    {
      sscanf(line,"glbytes(%d)",&len);
      break;
    }
  }
  ptr = malloc(len);
  if(ptr == NULL)
  {
    QMessageBox::critical( 0, "Process View Browser",
    "Could not malloc memory in glgetbytes()\nout of memory\nProcess View Browser will now exit.");
    exit(0);
  }
  tcp_rec_binary(s,(char *)ptr,len);
  return ptr;
}

void PvGLWidget::glputbytes(void *ptr, int len)
{
char buf[80];

  sprintf(buf,"glputbytes(%d);\n", len);
  tcp_send(s,buf,strlen(buf));
  tcp_send(s,(const char *)ptr, len);
}

int PvGLWidget::interpret(const char *line)
{
  if(line[0] != 'g' || line[1] != 'l')
  {
    if(strncmp(line,"pvGlEnd()",9) == 0)
    {
      tcp_send(s,"pvGlEnd();\n",11);
      return -1;
    }
    return 1; // interpret only gl commands
  }

  switch(line[2])
  {  
        /*********** input functions not jet implemented *******************
        GLAPI GLboolean GLAPIENTRY glAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences );
        GLAPI GLboolean GLAPIENTRY glAreTexturesResidentEXT( GLsizei n, const GLuint *textures, GLboolean *residences );
        GLAPI GLboolean GLAPIENTRY glIsEnabled( GLenum cap );
        GLAPI GLboolean GLAPIENTRY glIsList( GLuint list );
        GLAPI GLboolean GLAPIENTRY glIsTexture( GLuint texture );
        GLAPI GLboolean GLAPIENTRY glIsTextureEXT( GLuint texture );
        GLAPI GLenum GLAPIENTRY glGetError( void );
        GLAPI GLint GLAPIENTRY glRenderMode( GLenum mode );
        GLAPI const GLubyte* GLAPIENTRY glGetString( GLenum name );
        ********************************************************************/
    case 'A':
        if(strncmp(line,"glAccum(",8) == 0)
        {
          GLenum op; GLfloat value;
          sscanf(line,"glAccum(%d,%f)",(MyGLenum *) &op,&value);
          glAccum(op,value);
        }
        else if(strncmp(line,"glActiveTextureARB(",19) == 0)
        {
          GLenum texture;
          sscanf(line,"glActiveTextureARB(%d)",(MyGLenum *) &texture);
#ifndef PVWIN32
          glActiveTextureARB(texture);
#endif
        }
        else if(strncmp(line,"glAlphaFunc(",12) == 0)
        {
          GLenum func; GLclampf ref;
          sscanf(line,"glAlphaFunc(%d,%f)",(MyGLenum *) &func,&ref);
          glAlphaFunc(func,ref);
        }
        else if(strncmp(line,"glArrayElement(",15) == 0)
        {
          GLint i;
          sscanf(line,"glArrayElement(%d)",&i);
          glArrayElement(i);
        }
        else if(strncmp(line,"glArrayElementEXT(",18) == 0)
        {
          GLint i;
          sscanf(line,"glArrayElementEXT(%d)",&i);
#ifndef PVWIN32
          //glArrayElementEXT(i);
#endif
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'B':
        if(strncmp(line,"glBegin(",8) == 0)
        {
          GLenum mode;
          sscanf(line,"glBegin(%d)",(MyGLenum *) &mode);
          glBegin(mode);
        }
        else if(strncmp(line,"glBindTexture(",14) == 0)
        {
          GLenum target; GLuint texture;
          sscanf(line,"glBindTexture(%d,%d)",(MyGLenum *) &target,&texture);
          glBindTexture(target,texture);
        }
        else if(strncmp(line,"glBindTextureEXT(",17) == 0)
        {
          GLenum target; GLuint texture;
          sscanf(line,"glBindTextureEXT(%d,%d)",(MyGLenum *) &target,&texture);
#ifndef PVWIN32
          //glBindTextureEXT(target,texture);
#endif
        }
        else if(strncmp(line,"glBitmap(",9) == 0)
        {
          GLsizei width; GLsizei height; GLfloat xorig; GLfloat yorig; GLfloat xmove; GLfloat ymove; GLubyte *bitmap ;
          sscanf(line,"glBitmap(%d,%d,%f,%f,%f,%f)",&width,&height,&xorig,&yorig,&xmove,&ymove);
          bitmap = (GLubyte *) glgetbytes();
          glBitmap(width,height,xorig,yorig,xmove,ymove,bitmap);
          free(bitmap);
        }
        else if(strncmp(line,"glBlendColor(",13) == 0)
        {
          GLclampf red; GLclampf green; GLclampf blue; GLclampf alpha;
          sscanf(line,"glBlendColor(%f,%f,%f,%f)",&red,&green,&blue,&alpha);
#ifndef PVWIN32
          glBlendColor(red,green,blue,alpha);
#endif
        }
        else if(strncmp(line,"glBlendColorEXT(",16) == 0)
        {
          GLclampf red; GLclampf green; GLclampf blue; GLclampf alpha;
          sscanf(line,"glBlendColorEXT(%f,%f,%f,%f)",&red,&green,&blue,&alpha);
#ifndef PVWIN32
          //glBlendColorEXT(red,green,blue,alpha);
#endif
        }
        else if(strncmp(line,"glBlendEquation(",16) == 0)
        {
          GLenum mode;
          sscanf(line,"glBlendEquation(%d)",(MyGLenum *) &mode);
#ifndef PVWIN32
          //glBlendEquation(mode);
#endif
        }
        else if(strncmp(line,"glBlendEquationEXT(",19) == 0)
        {
          GLenum mode;
          sscanf(line,"glBlendEquationEXT(%d)",(MyGLenum *) &mode);
#ifndef PVWIN32
          //glBlendEquationEXT(mode);
#endif
        }
        else if(strncmp(line,"glBlendFunc(",12) == 0)
        {
          GLenum sfactor; GLenum dfactor;
          sscanf(line,"glBlendFunc(%d,%d)",(MyGLenum *) &sfactor,(MyGLenum *) &dfactor);
          glBlendFunc(sfactor,dfactor);
        }
        //else if(strncmp(line,"glBlendFuncSeparateINGR(",24) == 0)
        //{
        //  GLenum sfactorRGB; GLenum dfactorRGB; GLenum sfactorAlpha; GLenum dfactorAlpha;
        //  sscanf(line,"glBlendFuncSeparateINGR(%d,%d,%d,%d)",(MyGLenum *) &sfactorRGB,(MyGLenum *) &dfactorRGB,(MyGLenum *) &sfactorAlpha,(MyGLenum *) &dfactorAlpha);
        //  glBlendFuncSeparateINGR(sfactorRGB,dfactorRGB,sfactorAlpha,dfactorAlpha);
        //}
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'C':
      if(strncmp(line,"glCall",6) == 0)
      {
        if(strncmp(line,"glCallList(",11) == 0)
        {
          GLuint list;
          sscanf(line,"glCallList(%d)",&list);
          glCallList(list);
        }
        else if(strncmp(line,"glCallLists(",12) == 0)
        {
          GLsizei n; GLenum type; GLvoid *lists;
          sscanf(line,"glCallLists(%d,%d)",&n,(MyGLenum *) &type);
          lists = glgetbytes();
          glCallLists(n,type,lists);
          free(lists);
        }
      }
      else if(strncmp(line,"glClear",7) == 0)
      {
        if(strncmp(line,"glClear(",8) == 0)
        {
          GLbitfield mask;
          sscanf(line,"glClear(%d)",&mask);
          glClear(mask);
        }
        else if(strncmp(line,"glClearAccum(",13) == 0)
        {
          GLfloat red; GLfloat green; GLfloat blue; GLfloat alpha;
          sscanf(line,"glClearAccum(%f,%f,%f,%f)",&red,&green,&blue,&alpha);
          glClearAccum(red,green,blue,alpha);
        }
        else if(strncmp(line,"glClearColor(",13) == 0)
        {
          GLclampf red; GLclampf green; GLclampf blue; GLclampf alpha;
          sscanf(line,"glClearColor(%f,%f,%f,%f)",&red,&green,&blue,&alpha);
          glClearColor(red,green,blue,alpha);
        }
        else if(strncmp(line,"glClearDepth(",13) == 0)
        {
          GLfloat depth;
          sscanf(line,"glClearDepth(%f)",&depth);
          glClearDepth(depth);
        }
        else if(strncmp(line,"glClearIndex(",13) == 0)
        {
          GLfloat c;
          sscanf(line,"glClearIndex(%f)",&c);
          glClearIndex(c);
        }
        else if(strncmp(line,"glClearStencil(",15) == 0)
        {
          GLint s;
          sscanf(line,"glClearStencil(%d)",&s);
          glClearStencil(s);
        }
      }
      else if(strncmp(line,"glClient",8) == 0)
      {
        if(strncmp(line,"glClientActiveTextureARB(",25) == 0)
        {
          GLenum texture;
          sscanf(line,"glClientActiveTextureARB(%d)",(MyGLenum *) &texture);
#ifndef PVWIN32
          glClientActiveTextureARB(texture);
#endif
        }
      }
      else if(strncmp(line,"glClip",6) == 0)
      {
        if(strncmp(line,"glClipPlane(",12) == 0)
        {
          GLenum plane; GLfloat equation[4]; GLdouble dequation[4];
          sscanf(line,"glClipPlane(%d,%f,%f,%f,%f)",(MyGLenum *) &plane,&equation[0],&equation[1],&equation[2],&equation[3]);
          dequation[0] = equation[0];
          dequation[1] = equation[1];
          dequation[2] = equation[2];
          dequation[3] = equation[2];
          glClipPlane(plane,dequation);
        }
      }
      else if(strncmp(line,"glColor3",8) == 0)
      {
        if(strncmp(line,"glColor3b(",10) == 0)
        {
          int r,g,b;
          sscanf(line,"glColor3b(%d,%d,%d)",&r,&g,&b);
          glColor3b((GLbyte)r,(GLbyte)g,(GLbyte)b);
        }
        else if(strncmp(line,"glColor3bv(",11) == 0)
        {
          GLbyte v[4];
          int r,g,b;
          sscanf(line,"glColor3bv(%d,%d,%d)",&r,&g,&b);
          v[0] = (GLbyte) r;
          v[1] = (GLbyte) g;
          v[2] = (GLbyte) b;
          glColor3bv(v);
        }
        else if(strncmp(line,"glColor3d(",10) == 0)
        {
          GLfloat red; GLfloat green; GLfloat blue;
          sscanf(line,"glColor3d(%f,%f,%f)",&red,&green,&blue);
          glColor3d(red,green,blue);
        }
        else if(strncmp(line,"glColor3dv(",11) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glColor3dv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glColor3fv(v);
        }
        else if(strncmp(line,"glColor3f(",10) == 0)
        {
          GLfloat red; GLfloat green; GLfloat blue;
          sscanf(line,"glColor3f(%f,%f,%f)",&red,&green,&blue);
          glColor3f(red,green,blue);
        }
        else if(strncmp(line,"glColor3fv(",11) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glColor3fv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glColor3fv(v);
        }
        else if(strncmp(line,"glColor3i(",10) == 0)
        {
          GLint red; GLint green; GLint blue;
          sscanf(line,"glColor3i(%d,%d,%d)",&red,&green,&blue);
          glColor3i(red,green,blue);
        }
        else if(strncmp(line,"glColor3iv(",11) == 0)
        {
          GLint v[4];
          sscanf(line,"glColor3iv(%d,%d,%d)",&v[0],&v[1],&v[2]);
          glColor3iv(v);
        }
        else if(strncmp(line,"glColor3s(",10) == 0)
        {
          GLshort red; GLshort green; GLshort blue;
          sscanf(line,"glColor3s(%hd,%hd,%hd)",&red,&green,&blue);
          glColor3s(red,green,blue);
        }
        else if(strncmp(line,"glColor3sv(",11) == 0)
        {
          GLshort v[4];
          sscanf(line,"glColor3sv(%hd,%hd,%hd)",&v[0],&v[1],&v[2]);
          glColor3sv(v);
        }
        else if(strncmp(line,"glColor3ub(",11) == 0)
        {
          int r,g,b;
          sscanf(line,"glColor3ub(%d,%d,%d)",&r,&g,&b);
          glColor3ub((GLubyte)r,(GLubyte)g,(GLubyte)b);
        }
        else if(strncmp(line,"glColor3ubv(",12) == 0)
        {
          GLubyte v[4];
          int r,g,b;
          sscanf(line,"glColor3ubv(%d,%d,%d)",&r,&g,&b);
          v[0] = (GLubyte) r;
          v[1] = (GLubyte) g;
          v[2] = (GLubyte) b;
          glColor3ubv(v);
        }
        else if(strncmp(line,"glColor3ui(",11) == 0)
        {
          GLuint red; GLuint green; GLuint blue;
          sscanf(line,"glColor3ui(%d,%d,%d)",&red,&green,&blue);
          glColor3ui(red,green,blue);
        }
        else if(strncmp(line,"glColor3uiv(",12) == 0)
        {
          GLuint v[4];
          sscanf(line,"glColor3uiv(%d,%d,%d)",&v[0],&v[1],&v[2]);
          glColor3uiv(v);
        }
        else if(strncmp(line,"glColor3us(",11) == 0)
        {
          GLushort red; GLushort green; GLushort blue;
          sscanf(line,"glColor3us(%hd,%hd,%hd)",&red,&green,&blue);
          glColor3us(red,green,blue);
        }
        else if(strncmp(line,"glColor3usv(",12) == 0)
        {
          GLushort v[4];
          sscanf(line,"glColor3usv(%hd,%hd,%hd)",&v[0],&v[1],&v[2]);
          glColor3usv(v);
        }
      }
      else if(strncmp(line,"glColor4",8) == 0)
      {
        if(strncmp(line,"glColor4b(",10) == 0)
        {
          int r,g,b,a;
          sscanf(line,"glColor4b(%d,%d,%d,%d)",&r,&g,&b,&a);
          glColor4b((GLbyte)r,(GLbyte)g,(GLbyte)b,(GLbyte)a);
        }
        else if(strncmp(line,"glColor4bv(",11) == 0)
        {
          GLbyte v[4];
          int r,g,b,a;
          sscanf(line,"glColor4bv(%d,%d,%d,%d)",&r,&g,&b,&a);
          v[0] = (GLbyte) r;
          v[1] = (GLbyte) g;
          v[2] = (GLbyte) b;
          v[3] = (GLbyte) a;
          glColor4bv(v);
        }
        else if(strncmp(line,"glColor4d(",10) == 0)
        {
          GLfloat red; GLfloat green; GLfloat blue; GLfloat alpha;
          sscanf(line,"glColor4d(%f,%f,%f,%f)",&red,&green,&blue,&alpha);
          glColor4f(red,green,blue,alpha);
        }
        else if(strncmp(line,"glColor4dv(",11) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glColor4dv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glColor4fv(v);
        }
        else if(strncmp(line,"glColor4f(",10) == 0)
        {
          GLfloat red; GLfloat green; GLfloat blue; GLfloat alpha;
          sscanf(line,"glColor4f(%f,%f,%f,%f)",&red,&green,&blue,&alpha);
          glColor4f(red,green,blue,alpha);
        }
        else if(strncmp(line,"glColor4fv(",11) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glColor4fv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glColor4fv(v);
        }
        else if(strncmp(line,"glColor4i(",10) == 0)
        {
          GLint red; GLint green; GLint blue; GLint alpha;
          sscanf(line,"glColor4i(%d,%d,%d,%d)",&red,&green,&blue,&alpha);
          glColor4i(red,green,blue,alpha);
        }
        else if(strncmp(line,"glColor4iv(",11) == 0)
        {
          GLint v[4];
          sscanf(line,"glColor4iv(%d,%d,%d,%d)",&v[0],&v[1],&v[2],&v[3]);
          glColor4iv(v);
        }
        else if(strncmp(line,"glColor4s(",10) == 0)
        {
          GLshort red; GLshort green; GLshort blue; GLshort alpha;
          sscanf(line,"glColor4s(%hd,%hd,%hd,%hd)",&red,&green,&blue,&alpha);
          glColor4s(red,green,blue,alpha);
        }
        else if(strncmp(line,"glColor4sv(",11) == 0)
        {
          GLshort v[4];
          sscanf(line,"glColor4sv(%hd,%hd,%hd,%hd)",&v[0],&v[1],&v[2],&v[3]);
          glColor4sv(v);
        }
        else if(strncmp(line,"glColor4ub(",11) == 0)
        {
          int r,g,b,a;
          sscanf(line,"glColor4ub(%d,%d,%d,%d)",&r,&g,&b,&a);
          glColor4ub((GLubyte)r,(GLubyte)g,(GLubyte)b,(GLubyte)a);
        }
        else if(strncmp(line,"glColor4ubv(",12) == 0)
        {
          GLubyte v[4];
          int r,g,b,a;
          sscanf(line,"glColor4ubv(%d,%d,%d,%d)",&r,&g,&b,&a);
          v[0] = (GLubyte) r;
          v[1] = (GLubyte) g;
          v[2] = (GLubyte) b;
          v[3] = (GLubyte) a;

          glColor4ubv(v);
        }
        else if(strncmp(line,"glColor4ui(",11) == 0)
        {
          GLuint red; GLuint green; GLuint blue; GLuint alpha;
          sscanf(line,"glColor4ui(%d,%d,%d,%d)",&red,&green,&blue,&alpha);
          glColor4ui(red,green,blue,alpha);
        }
        else if(strncmp(line,"glColor4uiv(",12) == 0)
        {
          GLuint v[4];
          sscanf(line,"glColor4uiv(%d,%d,%d,%d)",&v[0],&v[1],&v[2],&v[3]);
          glColor4uiv(v);
        }
        else if(strncmp(line,"glColor4us(",11) == 0)
        {
          GLushort red; GLushort green; GLushort blue; GLushort alpha;
          sscanf(line,"glColor4us(%hd,%hd,%hd,%hd)",&red,&green,&blue,&alpha);
          glColor4us(red,green,blue,alpha);
        }
        else if(strncmp(line,"glColor4usv(",12) == 0)
        {
          GLushort v[4];
          sscanf(line,"glColor4usv(%hd,%hd,%hd,%hd)",&v[0],&v[1],&v[2],&v[3]);
          glColor4usv(v);
        }
      }
      else if(strncmp(line,"glColorM",8) == 0)
      {
        if(strncmp(line,"glColorMask(",12) == 0)
        {
          int r,g,b,a;
          sscanf(line,"glColorMask(%d,%d,%d,%d)",&r,&g,&b,&a);
          glColorMask((GLboolean)r,(GLboolean)g,(GLboolean)b,(GLboolean)a);
        }
        else if(strncmp(line,"glColorMaterial(",16) == 0)
        {
          GLenum face; GLenum mode;
          sscanf(line,"glColorMaterial(%d,%d)",(MyGLenum *) &face,(MyGLenum *) &mode);
          glColorMaterial(face,mode);
        }
      }
      else if(strncmp(line,"glColorP",8) == 0)
      {
        if(strncmp(line,"glColorPointer(",15) == 0)
        {
          GLint size; GLenum type; GLsizei stride; GLvoid *ptr;
          sscanf(line,"glColorPointer(%d,%d,%d)",&size,(MyGLenum *) &type,&stride);
          ptr = glgetbytes();
          glColorPointer(size,type,stride,ptr);
          free(ptr);
        }
        else if(strncmp(line,"glColorPointerEXT(",18) == 0)
        {
          GLint size; GLenum type; GLsizei stride; GLsizei count; GLvoid *ptr;
          sscanf(line,"glColorPointerEXT(%d,%d,%d,%d)",&size,(MyGLenum *) &type,&stride,&count);
          ptr = glgetbytes();
#ifndef PVWIN32
          //glColorPointerEXT(size,type,stride,count,ptr);
#endif
          free(ptr);
        }
      }
      else if(strncmp(line,"glColorS",8) == 0)
      {
        if(strncmp(line,"glColorSubTable(",16) == 0)
        {
          GLenum target; GLsizei start; GLsizei count; GLenum format; GLenum type; GLvoid *data;
          sscanf(line,"glColorSubTable(%d,%d,%d,%d,%d)",(MyGLenum *) &target,&start,&count,(MyGLenum *) &format,(MyGLenum *) &type);
          data = glgetbytes();
#ifndef PVWIN32
          glColorSubTable(target,start,count,format,type,data);
#endif
          free(data);
        }
        else if(strncmp(line,"glColorSubTableEXT(",19) == 0)
        {
          GLenum target; GLsizei start; GLsizei count; GLenum format; GLenum type; GLvoid *data;
          sscanf(line,"glColorSubTableEXT(%d,%d,%d,%d,%d)",(MyGLenum *) &target,&start,&count,(MyGLenum *) &format,(MyGLenum *) &type);
          data = glgetbytes();
#ifndef PVWIN32
          //glColorSubTableEXT(target,start,count,format,type,data);
#endif
          free(data);
        }
      }
      else if(strncmp(line,"glColorT",8) == 0)
      {
        if(strncmp(line,"glColorTable(",13) == 0)
        {
          GLenum target; GLenum internalformat; GLsizei width; GLenum format; GLenum type; GLvoid *table;
          sscanf(line,"glColorTable(%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&width,(MyGLenum *) &format,(MyGLenum *) &type);
          table = glgetbytes();
#ifndef PVWIN32
          glColorTable(target,internalformat,width,format,type,table);
#endif
          free(table);
        }
        else if(strncmp(line,"glColorTableEXT(",16) == 0)
        {
          GLenum target; GLenum internalformat; GLsizei width; GLenum format; GLenum type; GLvoid *table;
          sscanf(line,"glColorTableEXT(%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&width,(MyGLenum *) &format,(MyGLenum *) &type);
          table = glgetbytes();
#ifndef PVWIN32
          //glColorTableEXT(target,internalformat,width,format,type,table);
#endif
          free(table);
        }
        else if(strncmp(line,"glColorTableParameterfv(",24) == 0)
        {
          GLenum target; GLenum pname; GLfloat params[4];
          sscanf(line,"glColorTableParameterfv(%d,%d,%f,%f,%f,%f)",(MyGLenum *) &target,(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
#ifndef PVWIN32
          glColorTableParameterfv(target,pname,params);
#endif
        }
        else if(strncmp(line,"glColorTableParameteriv(",24) == 0)
        {
          GLenum target; GLenum pname; GLint params[4];
          sscanf(line,"glColorTableParameteriv(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
#ifndef PVWIN32
          glColorTableParameteriv(target,pname,params);
#endif
        }
      }
      else if(strncmp(line,"glConvo",7) == 0)
      {
        if(strncmp(line,"glConvolutionFilter1D(",22) == 0)
        {
          GLenum target; GLenum internalformat; GLsizei width; GLenum format; GLenum type; GLvoid *image;
          sscanf(line,"glConvolutionFilter1D(%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&width,(MyGLenum *) &format,(MyGLenum *) &type);
          image = glgetbytes();
#ifndef PVWIN32
          glConvolutionFilter1D(target,internalformat,width,format,type,image);
#endif
          free(image);
        }
        else if(strncmp(line,"glConvolutionFilter2D(",22) == 0)
        {
          GLenum target; GLenum internalformat; GLsizei width; GLsizei height; GLenum format; GLenum type; GLvoid *image;
          sscanf(line,"glConvolutionFilter2D(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&width,&height,(MyGLenum *) &format,(MyGLenum *) &type);
          image = glgetbytes();
#ifndef PVWIN32
          glConvolutionFilter2D(target,internalformat,width,height,format,type,image);
#endif
          free(image);
        }
        else if(strncmp(line,"glConvolutionParameterf(",24) == 0)
        {
          GLenum target; GLenum pname; GLfloat params;
          sscanf(line,"glConvolutionParameterf(%d,%d,%f)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
#ifndef PVWIN32
          glConvolutionParameterf(target,pname,params);
#endif
        }
        else if(strncmp(line,"glConvolutionParameterfv(",25) == 0)
        {
          GLenum target; GLenum pname; GLfloat params;
          sscanf(line,"glConvolutionParameterfv(%d,%d,%f)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
#ifndef PVWIN32
          glConvolutionParameterfv(target,pname,&params);
#endif
        }
        else if(strncmp(line,"glConvolutionParameteri(",24) == 0)
        {
          GLenum target; GLenum pname; GLint params;
          sscanf(line,"glConvolutionParameteri(%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
#ifndef PVWIN32
          glConvolutionParameteri(target,pname,params);
#endif
        }
        else if(strncmp(line,"glConvolutionParameteriv(",25) == 0)
        {
          GLenum target; GLenum pname; GLint params;
          sscanf(line,"glConvolutionParameteriv(%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
#ifndef PVWIN32
          glConvolutionParameteriv(target,pname,&params);
#endif
        }
      }
      else if(strncmp(line,"glCopy",6) == 0)
      {
        if(strncmp(line,"glCopyColorSubTable(",20) == 0)
        {
          GLenum target; GLsizei start; GLint x; GLint y; GLsizei width;
          sscanf(line,"glCopyColorSubTable(%d,%d,%d,%d,%d)",(MyGLenum *) &target,&start,&x,&y,&width);
#ifndef PVWIN32
          glCopyColorSubTable(target,start,x,y,width);
#endif
        }
        else if(strncmp(line,"glCopyColorTable(",17) == 0)
        {
          GLenum target; GLenum internalformat; GLint x; GLint y; GLsizei width;
          sscanf(line,"glCopyColorTable(%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&x,&y,&width);
#ifndef PVWIN32
          glCopyColorTable(target,internalformat,x,y,width);
#endif
        }
        else if(strncmp(line,"glCopyConvolutionFilter1D(",26) == 0)
        {
          GLenum target; GLenum internalformat; GLint x; GLint y; GLsizei width;
          sscanf(line,"glCopyConvolutionFilter1D(%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&x,&y,&width);
#ifndef PVWIN32
          glCopyConvolutionFilter1D(target,internalformat,x,y,width);
#endif
        }
        else if(strncmp(line,"glCopyConvolutionFilter2D(",26) == 0)
        {
          GLenum target; GLenum internalformat; GLint x; GLint y; GLsizei width; GLsizei height;
          sscanf(line,"glCopyConvolutionFilter2D(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&x,&y,&width,&height);
#ifndef PVWIN32
          glCopyConvolutionFilter2D(target,internalformat,x,y,width,height);
#endif
        }
        else if(strncmp(line,"glCopyPixels(",13) == 0)
        {
          GLint x; GLint y; GLsizei width; GLsizei height; GLenum type;
          sscanf(line,"glCopyPixels(%d,%d,%d,%d,%d)",&x,&y,&width,&height,(MyGLenum *) &type);
          glCopyPixels(x,y,width,height,type);
        }
        else if(strncmp(line,"glCopyTexImage1D(",17) == 0)
        {
          GLenum target; GLint level; GLenum internalformat; GLint x; GLint y; GLsizei width; GLint border;
          sscanf(line,"glCopyTexImage1D(%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,(MyGLenum *) &internalformat,&x,&y,&width,&border);
          glCopyTexImage1D(target,level,internalformat,x,y,width,border);
        }
        else if(strncmp(line,"glCopyTexImage2D(",17) == 0)
        {
          GLenum target; GLint level; GLenum internalformat; GLint x; GLint y; GLsizei width; GLsizei height; GLint border;
          sscanf(line,"glCopyTexImage2D(%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,(MyGLenum *) &internalformat,&x,&y,&width,&height,&border);
          glCopyTexImage2D(target,level,internalformat,x,y,width,height,border);
        }
        else if(strncmp(line,"glCopyTexSubImage1D(",20) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLint x; GLint y; GLsizei width;
          sscanf(line,"glCopyTexSubImage1D(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&x,&y,&width);
          glCopyTexSubImage1D(target,level,xoffset,x,y,width);
        }
        else if(strncmp(line,"glCopyTexSubImage2D(",20) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLint yoffset; GLint x; GLint y; GLsizei width; GLsizei height;
          sscanf(line,"glCopyTexSubImage2D(%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&yoffset,&x,&y,&width,&height);
          glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height);
        }
        else if(strncmp(line,"glCopyTexSubImage3D(",20) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLint yoffset; GLint zoffset; GLint x; GLint y; GLsizei width; GLsizei height;
          sscanf(line,"glCopyTexSubImage3D(%d,%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&yoffset,&zoffset,&x,&y,&width,&height);
#ifndef PVWIN32
          //glCopyTexSubImage3D(target,level,xoffset,yoffset,zoffset,x,y,width,height);
#endif
        }
        else if(strncmp(line,"glCopyTexSubImage3DEXT(",23) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLint yoffset; GLint zoffset; GLint x; GLint y; GLsizei width; GLsizei height;
          sscanf(line,"glCopyTexSubImage3DEXT(%d,%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&yoffset,&zoffset,&x,&y,&width,&height);
#ifndef PVWIN32
          //glCopyTexSubImage3DEXT(target,level,xoffset,yoffset,zoffset,x,y,width,height);
#endif
        }
      }
      else if(strncmp(line,"glCull",6) == 0)
      {
        if(strncmp(line,"glCullFace(",11) == 0)
        {
          GLenum mode;
          sscanf(line,"glCullFace(%d)",(MyGLenum *) &mode);
          glCullFace(mode);
        }
        else
        {
          printf("unkown: %s",line);
        }
      }
      else
      {
        printf("unkown: %s",line);
      }
      break;
    case 'D':
        if(strncmp(line,"glDeleteLists(",14) == 0)
        {
          GLuint list; GLsizei range;
          sscanf(line,"glDeleteLists(%d,%d)",&list,&range);
          glDeleteLists(list,range);
        }
        else if(strncmp(line,"glDeleteTextures(",17) == 0)
        {
          GLsizei n; GLuint *textures;
          sscanf(line,"glDeleteTextures(%d)",&n);
          textures = (GLuint *) glgetbytes();
          glDeleteTextures(n,textures);
          free(textures);
        }
        else if(strncmp(line,"glDeleteTexturesEXT(",20) == 0)
        {
          GLsizei n; GLuint *textures;
          sscanf(line,"glDeleteTexturesEXT(%d)",&n);
          textures = (GLuint *) glgetbytes();
#ifndef PVWIN32
          //glDeleteTexturesEXT(n,textures);
#endif
          free(textures);
        }
        else if(strncmp(line,"glDepthFunc(",12) == 0)
        {
          GLenum func;
          sscanf(line,"glDepthFunc(%d)",(MyGLenum *) &func);
          glDepthFunc(func);
        }
        else if(strncmp(line,"glDepthMask(",12) == 0)
        {
          int flag;
          sscanf(line,"glDepthMask(%d)",&flag);
          glDepthMask((GLboolean) flag);
        }
        else if(strncmp(line,"glDepthRange(",13) == 0)
        {
          GLfloat near_val; GLfloat far_val;
          sscanf(line,"glDepthRange(%f,%f)",&near_val,&far_val);
          glDepthRange(near_val,far_val);
        }
        else if(strncmp(line,"glDisable(",10) == 0)
        {
          GLenum cap;
          sscanf(line,"glDisable(%d)",(MyGLenum *) &cap);
          glDisable(cap);
        }
        else if(strncmp(line,"glDisableClientState(",21) == 0)
        {
          GLenum cap;
          sscanf(line,"glDisableClientState(%d)",(MyGLenum *) &cap);
          glDisableClientState(cap);
        }
        else if(strncmp(line,"glDrawArrays(",13) == 0)
        {
          GLenum mode; GLint first; GLsizei count;
          sscanf(line,"glDrawArrays(%d,%d,%d)",(MyGLenum *) &mode,&first,&count);
          glDrawArrays(mode,first,count);
        }
        else if(strncmp(line,"glDrawArraysEXT(",16) == 0)
        {
          GLenum mode; GLint first; GLsizei count;
          sscanf(line,"glDrawArraysEXT(%d,%d,%d)",(MyGLenum *) &mode,&first,&count);
#ifndef PVWIN32
          //glDrawArraysEXT(mode,first,count);
#endif
        }
        else if(strncmp(line,"glDrawBuffer(",13) == 0)
        {
          GLenum mode;
          sscanf(line,"glDrawBuffer(%d)",(MyGLenum *) &mode);
          glDrawBuffer(mode);
        }
        else if(strncmp(line,"glDrawElements(",15) == 0)
        {
          GLenum mode; GLsizei count; GLenum type; GLvoid *indices;
          sscanf(line,"glDrawElements(%d,%d,%d)",(MyGLenum *) &mode,&count,(MyGLenum *) &type);
          indices = glgetbytes();
          glDrawElements(mode,count,type,indices);
          free(indices);
        }
        else if(strncmp(line,"glDrawPixels(",13) == 0)
        {
          GLsizei width; GLsizei height; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glDrawPixels(%d,%d,%d,%d)",&width,&height,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
          glDrawPixels(width,height,format,type,pixels);
          free(pixels);
        }
        else if(strncmp(line,"glDrawRangeElements(",20) == 0)
        {
          GLenum mode; GLuint start; GLuint end; GLsizei count; GLenum type; GLvoid *indices;
          sscanf(line,"glDrawRangeElements(%d,%d,%d,%d,%d)",(MyGLenum *) &mode,&start,&end,&count,(MyGLenum *) &type);
          indices = glgetbytes();
#ifndef PVWIN32
          glDrawRangeElements(mode,start,end,count,type,indices);
#endif
          free(indices);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'E':
        if(strncmp(line,"glEdgeFlag(",11) == 0)
        {
          int flag;
          sscanf(line,"glEdgeFlag(%d)",&flag);
          glEdgeFlag((GLboolean) flag);
        }
        else if(strncmp(line,"glEdgeFlagPointer(",18) == 0)
        {
          GLsizei stride; GLint ptr;
          sscanf(line,"glEdgeFlagPointer(%d,%d)",&stride,&ptr);
          glEdgeFlagPointer(stride,&ptr);
        }
        else if(strncmp(line,"glEdgeFlagPointerEXT(",21) == 0)
        {
          GLsizei stride; GLsizei count; int iptr;
          sscanf(line,"glEdgeFlagPointerEXT(%d,%d,%d)",&stride,&count,&iptr);
#ifndef PVWIN32
          //glEdgeFlagPointerEXT(stride,count,&ptr);
#endif
        }
        else if(strncmp(line,"glEdgeFlagv(",12) == 0)
        {
          GLboolean flag; int iflag;
          sscanf(line,"glEdgeFlagv(%d)",&iflag);
          flag = (GLboolean) iflag;
          glEdgeFlagv(&flag);
        }
        else if(strncmp(line,"glEnable(",9) == 0)
        {
          GLenum cap;
          sscanf(line,"glEnable(%d)",(MyGLenum *) &cap);
          glEnable(cap);
        }
        else if(strncmp(line,"glEnableClientState(",20) == 0)
        {
          GLenum cap;
          sscanf(line,"glEnableClientState(%d)",(MyGLenum *) &cap);
          glEnableClientState(cap);
        }
        else if(strncmp(line,"glEnd(",6) == 0)
        {
          sscanf(line,"glEnd()");
          glEnd();
        }
        else if(strncmp(line,"glEndList(",10) == 0)
        {
          sscanf(line,"glEndList()");
          glEndList();
          QApplication::restoreOverrideCursor();
        }
        else if(strncmp(line,"glEvalCoord1d(",14) == 0)
        {
          GLfloat u;
          sscanf(line,"glEvalCoord1d(%f)",&u);
          glEvalCoord1d(u);
        }
        else if(strncmp(line,"glEvalCoord1dv(",15) == 0)
        {
          GLfloat u; GLdouble d;
          sscanf(line,"glEvalCoord1dv(%f)",&u);
          d = u;
          glEvalCoord1dv(&d);
        }
        else if(strncmp(line,"glEvalCoord1f(",14) == 0)
        {
          GLfloat u;
          sscanf(line,"glEvalCoord1f(%f)",&u);
          glEvalCoord1f(u);
        }
        else if(strncmp(line,"glEvalCoord1fv(",15) == 0)
        {
          GLfloat u ;
          sscanf(line,"glEvalCoord1fv(%f)",&u);
          glEvalCoord1fv(&u);
        }
        else if(strncmp(line,"glEvalCoord2d(",14) == 0)

        {
          GLfloat u; GLfloat v;
          sscanf(line,"glEvalCoord2d(%f,%f)",&u,&v);
          glEvalCoord2d(u,v);
        }
        else if(strncmp(line,"glEvalCoord2dv(",15) == 0)
        {
          GLfloat u[2]; GLdouble d[2];
          sscanf(line,"glEvalCoord2dv(%f,%f)",&u[0],&u[1]);
          d[0] = u[0]; d[1] = u[1];
          glEvalCoord2dv(d);
        }
        else if(strncmp(line,"glEvalCoord2f(",14) == 0)
        {
          GLfloat u; GLfloat v;
          sscanf(line,"glEvalCoord2f(%f,%f)",&u,&v);
          glEvalCoord2f(u,v);
        }
        else if(strncmp(line,"glEvalCoord2fv(",15) == 0)
        {
          GLfloat u[2];
          sscanf(line,"glEvalCoord2fv(%f,%f)",&u[0],&u[1]);
          glEvalCoord2fv(u);
        }
        else if(strncmp(line,"glEvalMesh1(",12) == 0)
        {
          GLenum mode; GLint i1; GLint i2;
          sscanf(line,"glEvalMesh1(%d,%d,%d)",(MyGLenum *) &mode,&i1,&i2);
          glEvalMesh1(mode,i1,i2);
        }
        else if(strncmp(line,"glEvalMesh2(",12) == 0)
        {
          GLenum mode; GLint i1; GLint i2; GLint j1; GLint j2;
          sscanf(line,"glEvalMesh2(%d,%d,%d,%d,%d)",(MyGLenum *) &mode,&i1,&i2,&j1,&j2);
          glEvalMesh2(mode,i1,i2,j1,j2);
        }
        else if(strncmp(line,"glEvalPoint1(",13) == 0)
        {
          GLint i;
          sscanf(line,"glEvalPoint1(%d)",&i);
          glEvalPoint1(i);
        }
        else if(strncmp(line,"glEvalPoint2(",13) == 0)
        {
          GLint i; GLint j;
          sscanf(line,"glEvalPoint2(%d,%d)",&i,&j);
          glEvalPoint2(i,j);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'F':
        if(strncmp(line,"glFeedbackBuffer(",17) == 0)
        {
          GLsizei size; GLenum type; GLfloat *buffer;
          sscanf(line,"glFeedbackBuffer(%d,%d)",&size,(MyGLenum *) &type);
          buffer = (GLfloat *) glgetbytes();
          glFeedbackBuffer(size,type,buffer);
          free(buffer);
        }
        else if(strncmp(line,"glFinish(",9) == 0)
        {
          sscanf(line,"glFinish()");
          glFinish();
        }
        else if(strncmp(line,"glFlush(",8) == 0)
        {
          sscanf(line,"glFlush()");
          glFlush();
        }
        else if(strncmp(line,"glFogf(",7) == 0)
        {
          GLenum pname; GLfloat param;
          sscanf(line,"glFogf(%d,%f)",(MyGLenum *) &pname,&param);
          glFogf(pname,param);
        }
        else if(strncmp(line,"glFogfv(",8) == 0)
        {
          GLenum pname; GLfloat params;
          sscanf(line,"glFogfv(%d,%f)",(MyGLenum *) &pname,&params);
          glFogfv(pname,&params);
        }
        else if(strncmp(line,"glFogi(",7) == 0)
        {
          GLenum pname; GLint param;
          sscanf(line,"glFogi(%d,%d)",(MyGLenum *) &pname,&param);
          glFogi(pname,param);
        }
        else if(strncmp(line,"glFogiv(",8) == 0)
        {
          GLenum pname; GLint params;
          sscanf(line,"glFogiv(%d,%d)",(MyGLenum *) &pname,&params);
          glFogiv(pname,&params);
        }
        else if(strncmp(line,"glFrontFace(",12) == 0)
        {
          GLenum mode;
          sscanf(line,"glFrontFace(%d)",(MyGLenum *) &mode);
          glFrontFace(mode);
        }
        else if(strncmp(line,"glFrustum(",10) == 0)
        {
          GLfloat left; GLfloat right; GLfloat bottom; GLfloat top; GLfloat near_val; GLfloat far_val;
          sscanf(line,"glFrustum(%f,%f,%f,%f,%f,%f)",&left,&right,&bottom,&top,&near_val,&far_val);
          glFrustum(left,right,bottom,top,near_val,far_val);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'G':
        if(strncmp(line,"glGenTextures(",14) == 0)
        {
          GLsizei n; GLuint *textures;
          sscanf(line,"glGenTextures(%d)",&n);
          //rl2008  textures = (GLuint *) glgetbytes();
          textures = (GLuint *) malloc(n*sizeof(GLuint));
          glGenTextures(n,textures);
          glputbytes(textures, n*sizeof(GLuint));
          free(textures);
        }
        else if(strncmp(line,"glGenTexturesEXT(",17) == 0)
        {
          GLsizei n; GLuint *textures;
          sscanf(line,"glGenTexturesEXT(%d)",&n);
          //rl2008 textures = (GLuint *) glgetbytes();
          textures = (GLuint *) malloc(n*sizeof(GLuint));
#ifndef PVWIN32
          //glGenTexturesEXT(n,textures);
#endif
          glputbytes(textures, n*sizeof(GLuint));
          free(textures);
        }
        else if(strncmp(line,"glGenLists(",11) == 0)
        {
          GLsizei range; GLsizei ret;
          sscanf(line,"glGenLists(%ld)",(long*) &range);
          ret = glGenLists(range);
          if(minobject == -1)
          {
            minobject = maxobject = ret;
          }
          else
          {
            if(ret > maxobject) maxobject = ret;
            if(ret < minobject) minobject = ret;
          }
          gllongresponse(ret);
        }

        /* get routines not implemented jet
        GLAPI void GLAPIENTRY glGetBooleanv( GLenum pname, GLboolean *params )
        {
          sscanf(line,"glGetBooleanv( GLenum pname, GLboolean *params )");

        }

        GLAPI void GLAPIENTRY glGetClipPlane( GLenum plane, GLdouble *equation )
        {
          sscanf(line,"glGetClipPlane( GLenum plane, GLdouble *equation )");
        }

        GLAPI void GLAPIENTRY glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table )
        {
          sscanf(line,"glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table )");
        }

        GLAPI void GLAPIENTRY glGetColorTableEXT( GLenum target, GLenum format, GLenum type, GLvoid *table )
        {
          sscanf(line,"glGetColorTableEXT( GLenum target, GLenum format, GLenum type, GLvoid *table )");
        }

        GLAPI void GLAPIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetColorTableParameterfvEXT( GLenum target, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetColorTableParameterfvEXT( GLenum target, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetColorTableParameterivEXT( GLenum target, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetColorTableParameterivEXT( GLenum target, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image )
        {
          sscanf(line,"glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image )");
        }

        GLAPI void GLAPIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetDoublev( GLenum pname, GLdouble *params )
        {
          sscanf(line,"glGetDoublev( GLenum pname, GLdouble *params )");
        }

        GLAPI void GLAPIENTRY glGetFloatv( GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetFloatv( GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values )
        {
          sscanf(line,"glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values )");
        }

        GLAPI void GLAPIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetIntegerv( GLenum pname, GLint *params )
        {
          sscanf(line,"glGetIntegerv( GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetLightfv( GLenum light, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetLightiv( GLenum light, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble *v )
        {
          sscanf(line,"glGetMapdv( GLenum target, GLenum query, GLdouble *v )");
        }

        GLAPI void GLAPIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v )
        {
          sscanf(line,"glGetMapfv( GLenum target, GLenum query, GLfloat *v )");
        }

        GLAPI void GLAPIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v )
        {
          sscanf(line,"glGetMapiv( GLenum target, GLenum query, GLint *v )");
        }

        GLAPI void GLAPIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetMaterialiv( GLenum face, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values )
        {
          sscanf(line,"glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values )");
        }

        GLAPI void GLAPIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetPixelMapfv( GLenum map, GLfloat *values )
        {
          sscanf(line,"glGetPixelMapfv( GLenum map, GLfloat *values )");
        }

        GLAPI void GLAPIENTRY glGetPixelMapuiv( GLenum map, GLuint *values )
        {
          sscanf(line,"glGetPixelMapuiv( GLenum map, GLuint *values )");
        }

        GLAPI void GLAPIENTRY glGetPixelMapusv( GLenum map, GLushort *values )
        {
          sscanf(line,"glGetPixelMapusv( GLenum map, GLushort *values )");
        }

        GLAPI void GLAPIENTRY glGetPointerv( GLenum pname, void **params )
        {
          sscanf(line,"glGetPointerv( GLenum pname, void **params )");
        }

        GLAPI void GLAPIENTRY glGetPointervEXT( GLenum pname, void **params )
        {
          sscanf(line,"glGetPointervEXT( GLenum pname, void **params )");
        }

        GLAPI void GLAPIENTRY glGetPolygonStipple( GLubyte *mask )
        {
          sscanf(line,"glGetPolygonStipple( GLubyte *mask )");
        }

        GLAPI void GLAPIENTRY glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span )
        {
          sscanf(line,"glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span )");
        }

        GLAPI void GLAPIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetTexEnviv( GLenum target, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params )
        {
          sscanf(line,"glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params )");
        }

        GLAPI void GLAPIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetTexGeniv( GLenum coord, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )
        {
          sscanf(line,"glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )");
        }

        GLAPI void GLAPIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params )
        {
          sscanf(line,"glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params )");
        }

        GLAPI void GLAPIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params )");
        }

        GLAPI void GLAPIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params)
        {
          sscanf(line,"glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params)");
        }

        GLAPI void GLAPIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params )
        {
          sscanf(line,"glGetTexParameteriv( GLenum target, GLenum pname, GLint *params )");
        }
        */
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'H':
        if(strncmp(line,"glHint(",7) == 0)
        {
          GLenum target; GLenum mode;
          sscanf(line,"glHint(%d,%d)",(MyGLenum *) &target,(MyGLenum *) &mode);
          glHint(target,mode);
        }
        //else if(strncmp(line,"glHintPGI(",10) == 0)
        //{
        //  GLenum target; GLint mode;
        //  sscanf(line,"glHintPGI(%d,%d)",(MyGLenum *) &target,&mode);
        //  glHintPGI(target,mode);
        //}
        else if(strncmp(line,"glHistogram(",12) == 0)
        {
          GLenum target; GLsizei width; GLenum internalformat; int sink;
          sscanf(line,"glHistogram(%d,%d,%d,%d)",(MyGLenum *) &target,&width,(MyGLenum *) &internalformat,&sink);
#ifndef PVWIN32
          glHistogram(target,width,internalformat,(GLboolean) sink);
#endif
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'I':
        if(strncmp(line,"glIndexMask(",12) == 0)
        {
          GLuint mask;
          sscanf(line,"glIndexMask(%d)",&mask);
          glIndexMask(mask);
        }
        else if(strncmp(line,"glIndexPointer(",15) == 0)
        {
          GLenum type; GLsizei stride; GLvoid *ptr;
          sscanf(line,"glIndexPointer(%d,%d)",(MyGLenum *) &type,&stride);
          ptr = glgetbytes();
          glIndexPointer(type,stride,ptr);
          free(ptr);
        }
        else if(strncmp(line,"glIndexPointerEXT(",18) == 0)
        {
          GLenum type; GLsizei stride; GLsizei count; GLvoid *ptr;
          sscanf(line,"glIndexPointerEXT(%d,%d,%d)",(MyGLenum *) &type,&stride,&count);
          ptr = glgetbytes();
#ifndef PVWIN32
          //glIndexPointerEXT(type,stride,count,ptr);
#endif
          free(ptr);
        }
        else if(strncmp(line,"glIndexd(",9) == 0)
        {
          GLfloat c;
          sscanf(line,"glIndexd(%f)",&c);
          glIndexd(c);
        }
        else if(strncmp(line,"glIndexdv(",10) == 0)
        {
          GLfloat c; GLdouble d;
          sscanf(line,"glIndexdv(%f)",&c);
          d = c;
          glIndexdv(&d);
        }
        else if(strncmp(line,"glIndexf(",9) == 0)
        {
          GLfloat c;
          sscanf(line,"glIndexf(%f)",&c);
          glIndexf(c);
        }
        else if(strncmp(line,"glIndexfv(",10) == 0)
        {
          GLfloat c;
          sscanf(line,"glIndexfv(%f)",&c);
          glIndexfv(&c);
        }
        else if(strncmp(line,"glIndexi(",9) == 0)
        {
          GLint c;
          sscanf(line,"glIndexi(%d)",&c);
          glIndexi(c);
        }
        else if(strncmp(line,"glIndexiv(",10) == 0)
        {
          GLint c;
          sscanf(line,"glIndexiv(%d)",&c);
          glIndexiv(&c);
        }
        else if(strncmp(line,"glIndexs(",9) == 0)
        {
          GLshort c;
          sscanf(line,"glIndexs(%hd)",&c);
          glIndexs(c);
        }
        else if(strncmp(line,"glIndexsv(",10) == 0)
        {
          GLshort c;
          sscanf(line,"glIndexsv(%hd)",&c);
          glIndexsv(&c);
        }
        else if(strncmp(line,"glIndexub(",10) == 0)
        {
          int c;
          sscanf(line,"glIndexub(%d)",&c);
          glIndexub((GLubyte) c);
        }
        else if(strncmp(line,"glIndexubv(",11) == 0)
        {
          GLubyte c; int ic;
          sscanf(line,"glIndexubv(%d)",&ic);
          c = (GLubyte) ic;
          glIndexubv(&c);
        }
        else if(strncmp(line,"glInitNames(",12) == 0)
        {
          sscanf(line,"glInitNames()");
          glInitNames();
        }
        else if(strncmp(line,"glInterleavedArrays(",20) == 0)
        {
          GLenum format; GLsizei stride; GLvoid *pointer;
          sscanf(line,"glInterleavedArrays(%d,%d)",(MyGLenum *) &format,&stride);
          pointer = glgetbytes();
          glInterleavedArrays(format,stride,pointer);
          free(pointer);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'L':
        if(strncmp(line,"glLightModelf(",14) == 0)
        {
          GLenum pname; GLfloat param;
          sscanf(line,"glLightModelf(%d,%f)",(MyGLenum *) &pname,&param);
          glLightModelf(pname,param);
        }
        else if(strncmp(line,"glLightModelfv(",15) == 0)
        {
          GLenum pname; GLfloat params[4];
          sscanf(line,"glLightModelfv(%d,%f,%f,%f,%f)",(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
          glLightModelfv(pname,params);
        }
        else if(strncmp(line,"glLightModeli(",14) == 0)
        {
          GLenum pname; GLint param;
          sscanf(line,"glLightModeli(%d,%d)",(MyGLenum *) &pname,&param);
          glLightModeli(pname,param);
        }
        else if(strncmp(line,"glLightModeliv(",15) == 0)
        {
          GLenum pname; GLint params[4];
          sscanf(line,"glLightModeliv(%d,%d,%d,%d,%d)",(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
          glLightModeliv(pname,params);
        }
        else if(strncmp(line,"glLightf(",9) == 0)
        {
          GLenum light; GLenum pname; GLfloat param;
          sscanf(line,"glLightf(%d,%d,%f)",(MyGLenum *) &light,(MyGLenum *) &pname,&param);
          glLightf(light,pname,param);
        }
        else if(strncmp(line,"glLightfv(",10) == 0)
        {
          GLenum light; GLenum pname; GLfloat params[4];
          sscanf(line,"glLightfv(%d,%d,%f,%f,%f,%f)",(MyGLenum *) &light,(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
          glLightfv(light,pname,params);
        }
        else if(strncmp(line,"glLighti(",9) == 0)
        {
          GLenum light; GLenum pname; GLint param;
          sscanf(line,"glLighti(%d,%d,%d)",(MyGLenum *) &light,(MyGLenum *) &pname,&param);
          glLighti(light,pname,param);
        }
        else if(strncmp(line,"glLightiv(",10) == 0)
        {
          GLenum light; GLenum pname; GLint params[4];
          sscanf(line,"glLightiv(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &light,(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
          glLightiv(light,pname,params);
        }
        else if(strncmp(line,"glLineStipple(",14) == 0)
        {
          GLint factor; GLushort pattern;
          sscanf(line,"glLineStipple(%d,%hd)",&factor,&pattern);
          glLineStipple(factor,pattern);
        }
        else if(strncmp(line,"glLineWidth(",12) == 0)
        {
          GLfloat width;
          sscanf(line,"glLineWidth(%f)",&width);
          glLineWidth(width);
        }
        else if(strncmp(line,"glListBase(",11) == 0)
        {
          GLuint base;
          sscanf(line,"glListBase(%d)",&base);
          glListBase(base);
        }
        else if(strncmp(line,"glLoadIdentity(",15) == 0)
        {
          sscanf(line,"glLoadIdentity()");
          glLoadIdentity();
        }
        else if(strncmp(line,"glLoadMatrixd(",14) == 0)
        {
          GLdouble *m;
          sscanf(line,"glLoadMatrixd()");
          m = (GLdouble *) glgetbytes();
          glLoadMatrixd(m);
          free(m);
        }
        else if(strncmp(line,"glLoadMatrixf(",14) == 0)
        {
          GLfloat *m;
          sscanf(line,"glLoadMatrixf()");
          m = (GLfloat *) glgetbytes();
          glLoadMatrixf(m);
          free(m);
        }
        else if(strncmp(line,"glLoadName(",11) == 0)
        {
          GLuint name;
          sscanf(line,"glLoadName(%d)",&name);
          glLoadName(name);
        }
        else if(strncmp(line,"glLockArraysEXT(",16) == 0)
        {
          GLint first; GLsizei count;
          sscanf(line,"glLockArraysEXT(%d,%d)",&first,&count);
#ifndef PVWIN32
          //glLockArraysEXT(first,count);
#endif
        }
        else if(strncmp(line,"glLogicOp(",10) == 0)
        {
          GLenum opcode;
          sscanf(line,"glLogicOp(%d)",(MyGLenum *) &opcode);
          glLogicOp(opcode);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'M':
        if(strncmp(line,"glMap1d(",8) == 0)
        {
          GLenum target; GLfloat u1; GLfloat u2; GLint stride; GLint order; GLdouble *points;
          sscanf(line,"glMap1d(%d,%f,%f,%d,%d)",(MyGLenum *) &target,&u1,&u2,&stride,&order);
          points = (GLdouble *) glgetbytes();
          glMap1d(target,u1,u2,stride,order,points);
          free(points);
        }
        else if(strncmp(line,"glMap1f(",8) == 0)
        {

          GLenum target; GLfloat u1; GLfloat u2; GLint stride; GLint order; GLfloat *points;
          sscanf(line,"glMap1f(%d,%f,%f,%d,%d)",(MyGLenum *) &target,&u1,&u2,&stride,&order);
          points = (GLfloat *) glgetbytes();
          glMap1f(target,u1,u2,stride,order,points);
          free(points);
        }
        else if(strncmp(line,"glMap2d(",8) == 0)
        {
          GLenum target; GLfloat u1; GLfloat u2; GLint ustride; GLint uorder; GLfloat v1; GLfloat v2; GLint vstride; GLint vorder; GLdouble *points;
          sscanf(line,"glMap2d(%d,%f,%f,%d,%d,%f,%f,%d,%d)",(MyGLenum *) &target,&u1,&u2,&ustride,&uorder,&v1,&v2,&vstride,&vorder);
          points = (GLdouble *) glgetbytes();
          glMap2d(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points);
          free(points);
        }
        else if(strncmp(line,"glMap2f(",8) == 0)
        {
          GLenum target; GLfloat u1; GLfloat u2; GLint ustride; GLint uorder; GLfloat v1; GLfloat v2; GLint vstride; GLint vorder; GLfloat *points;
          sscanf(line,"glMap2f(%d,%f,%f,%d,%d,%f,%f,%d,%d)",(MyGLenum *) &target,&u1,&u2,&ustride,&uorder,&v1,&v2,&vstride,&vorder);
          points = (GLfloat *) glgetbytes();
          glMap2f(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points);
          free(points);
        }
        else if(strncmp(line,"glMapGrid1d(",12) == 0)
        {
          GLint un; GLfloat u1; GLfloat u2;
          sscanf(line,"glMapGrid1d(%d,%f,%f)",&un,&u1,&u2);
          glMapGrid1d(un,u1,u2);
        }
        else if(strncmp(line,"glMapGrid1f(",12) == 0)
        {
          GLint un; GLfloat u1; GLfloat u2;
          sscanf(line,"glMapGrid1f(%d,%f,%f)",&un,&u1,&u2);
          glMapGrid1f(un,u1,u2);
        }
        else if(strncmp(line,"glMapGrid2d(",12) == 0)
        {
          GLint un; GLfloat u1; GLfloat u2; GLint vn; GLfloat v1; GLfloat v2;
          sscanf(line,"glMapGrid2d(%d,%f,%f,%d,%f,%f)",&un,&u1,&u2,&vn,&v1,&v2);
          glMapGrid2d(un,u1,u2,vn,v1,v2);
        }

        else if(strncmp(line,"glMapGrid2f(",12) == 0)
        {
          GLint un; GLfloat u1; GLfloat u2; GLint vn; GLfloat v1; GLfloat v2;
          sscanf(line,"glMapGrid2f(%d,%f,%f,%d,%f,%f)",&un,&u1,&u2,&vn,&v1,&v2);
          glMapGrid2f(un,u1,u2,vn,v1,v2);
        }
        else if(strncmp(line,"glMaterialf(",12) == 0)
        {
          GLenum face; GLenum pname; GLfloat param;
          sscanf(line,"glMaterialf(%d,%d,%f)",(MyGLenum *) &face,(MyGLenum *) &pname,&param);
          glMaterialf(face,pname,param);
        }
        else if(strncmp(line,"glMaterialfv(",13) == 0)
        {
          GLenum face; GLenum pname; GLfloat params[4];
          sscanf(line,"glMaterialfv(%d,%d,%f,%f,%f,%f)",(MyGLenum *) &face,(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
          glMaterialfv(face,pname,params);
        }
        else if(strncmp(line,"glMateriali(",12) == 0)
        {
          GLenum face; GLenum pname; GLint param;
          sscanf(line,"glMateriali(%d,%d,%d)",(MyGLenum *) &face,(MyGLenum *) &pname,&param);
          glMateriali(face,pname,param);
        }
        else if(strncmp(line,"glMaterialiv(",13) == 0)
        {
          GLenum face; GLenum pname; GLint params[4];
          sscanf(line,"glMaterialiv(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &face,(MyGLenum *) &pname,&params[0],&params[1],&params[2],&params[3]);
          glMaterialiv(face,pname,params);
        }
        else if(strncmp(line,"glMatrixMode(",13) == 0)
        {
          GLenum mode;
          sscanf(line,"glMatrixMode(%d)",(MyGLenum *) &mode);
          glMatrixMode(mode);
        }
        else if(strncmp(line,"glMinmax(",9) == 0)
        {
          GLenum target; GLenum internalformat; int isink;
          sscanf(line,"glMinmax(%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&isink);
#ifndef PVWIN32
          glMinmax(target,internalformat,(GLboolean) isink);
#endif
        }
        else if(strncmp(line,"glMultMatrixd(",14) == 0)
        {
          GLdouble *m;
          m = (GLdouble *) glgetbytes();
          glMultMatrixd(m);
          free(m);
        }
        else if(strncmp(line,"glMultMatrixf(",14) == 0)
        {
          GLfloat *m;
          m = (GLfloat *) glgetbytes();
          glMultMatrixf(m);
          free(m);
        }
        else if(strncmp(line,"glMultiTexCoord1dARB(",21) == 0)
        {
          GLenum target; GLfloat s;
          sscanf(line,"glMultiTexCoord1dARB(%d,%f)",(MyGLenum *) &target,&s);
#ifndef PVWIN32
          glMultiTexCoord1dARB(target,s);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord1dvARB(",22) == 0)
        {
          GLenum target; GLfloat v;
          sscanf(line,"glMultiTexCoord1dvARB(%d,%f)",(MyGLenum *) &target,&v);
#ifndef PVWIN32
          glMultiTexCoord1fvARB(target,&v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord1fARB(",21) == 0)
        {
          GLenum target; GLfloat s;
          sscanf(line,"glMultiTexCoord1fARB(%d,%f)",(MyGLenum *) &target,&s);
#ifndef PVWIN32
          glMultiTexCoord1fARB(target,s);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord1fvARB(",22) == 0)
        {
          GLenum target; GLfloat v;
          sscanf(line,"glMultiTexCoord1fvARB(%d,%f)",(MyGLenum *) &target,&v);
#ifndef PVWIN32
          glMultiTexCoord1fvARB(target,&v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord1iARB(",21) == 0)
        {
          GLenum target; GLint s;
          sscanf(line,"glMultiTexCoord1iARB(%d,%d)",(MyGLenum *) &target,&s);
#ifndef PVWIN32
          glMultiTexCoord1iARB(target,s);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord1ivARB(",22) == 0)
        {
          GLenum target; GLint v;
          sscanf(line,"glMultiTexCoord1ivARB(%d,%d)",(MyGLenum *) &target,&v);
#ifndef PVWIN32
          glMultiTexCoord1ivARB(target,&v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord1sARB(",21) == 0)
        {
          GLenum target; GLshort s;
          sscanf(line,"glMultiTexCoord1sARB(%d,%hd)",(MyGLenum *) &target,&s);
#ifndef PVWIN32
          glMultiTexCoord1sARB(target,s);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord1svARB(",22) == 0)
        {
          GLenum target; GLshort v;
          sscanf(line,"glMultiTexCoord1svARB(%d,%hd)",(MyGLenum *) &target,&v);
#ifndef PVWIN32
          glMultiTexCoord1svARB(target,&v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2dARB(",21) == 0)
        {
          GLenum target; GLfloat s; GLfloat t;
          sscanf(line,"glMultiTexCoord2dARB(%d,%f,%f)",(MyGLenum *) &target,&s,&t);
#ifndef PVWIN32
          glMultiTexCoord2dARB(target,s,t);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2dvARB(",22) == 0)
        {
          GLenum target; GLfloat v[2];
          sscanf(line,"glMultiTexCoord2dvARB(%d,%f,%f)",(MyGLenum *) &target,&v[0],&v[1]);
#ifndef PVWIN32
          glMultiTexCoord2fvARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2fARB(",21) == 0)
        {
          GLenum target; GLfloat s; GLfloat t;
          sscanf(line,"glMultiTexCoord2fARB(%d,%f,%f)",(MyGLenum *) &target,&s,&t);
#ifndef PVWIN32
          glMultiTexCoord2fARB(target,s,t);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2fvARB(",22) == 0)
        {
          GLenum target; GLfloat v[2];
          sscanf(line,"glMultiTexCoord2fvARB(%d,%f,%f)",(MyGLenum *) &target,&v[0],&v[1]);
#ifndef PVWIN32
          glMultiTexCoord2fvARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2iARB(",21) == 0)
        {
          GLenum target; GLint s; GLint t;
          sscanf(line,"glMultiTexCoord2iARB(%d,%d,%d)",(MyGLenum *) &target,&s,&t);
#ifndef PVWIN32
          glMultiTexCoord2iARB(target,s,t);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2ivARB(",22) == 0)
        {
          GLenum target; GLint v[2];
          sscanf(line,"glMultiTexCoord2ivARB(%d,%d,%d)",(MyGLenum *) &target,&v[0],&v[1]);
#ifndef PVWIN32
          glMultiTexCoord2ivARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2sARB(",21) == 0)
        {
          GLenum target; GLshort s; GLshort t;
          sscanf(line,"glMultiTexCoord2sARB(%d,%hd,%hd)",(MyGLenum *) &target,&s,&t);
#ifndef PVWIN32
          glMultiTexCoord2sARB(target,s,t);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord2svARB(",22) == 0)
        {
          GLenum target; GLshort v[2];
          sscanf(line,"glMultiTexCoord2svARB(%d,%hd,%hd)",(MyGLenum *) &target,&v[0],&v[1]);
#ifndef PVWIN32
          glMultiTexCoord2svARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3dARB(",21) == 0)
        {
          GLenum target; GLfloat s; GLfloat t; GLfloat r;
          sscanf(line,"glMultiTexCoord3dARB(%d,%f,%f,%f)",(MyGLenum *) &target,&s,&t,&r);
#ifndef PVWIN32
          glMultiTexCoord3dARB(target,s,t,r);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3dvARB(",22) == 0)
        {
          GLenum target; GLfloat v[4];
          sscanf(line,"glMultiTexCoord3dvARB(%d,%f,%f,%f)",(MyGLenum *) &target,&v[0],&v[1],&v[2]);
#ifndef PVWIN32
          glMultiTexCoord3fvARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3fARB(",21) == 0)
        {
          GLenum target; GLfloat s; GLfloat t; GLfloat r;
          sscanf(line,"glMultiTexCoord3fARB(%d,%f,%f,%f)",(MyGLenum *) &target,&s,&t,&r);
#ifndef PVWIN32
          glMultiTexCoord3fARB(target,s,t,r);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3fvARB(",22) == 0)
        {
          GLenum target; GLfloat v[4];
          sscanf(line,"glMultiTexCoord3fvARB(%d,%f,%f,%f)",(MyGLenum *) &target,&v[0],&v[1],&v[2]);
#ifndef PVWIN32
          glMultiTexCoord3fvARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3iARB(",21) == 0)
        {
          GLenum target; GLint s; GLint t; GLint r;
          sscanf(line,"glMultiTexCoord3iARB(%d,%d,%d,%d)",(MyGLenum *) &target,&s,&t,&r);
#ifndef PVWIN32
          glMultiTexCoord3iARB(target,s,t,r);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3ivARB(",22) == 0)
        {
          GLenum target; GLint v[4];
          sscanf(line,"glMultiTexCoord3ivARB(%d,%d,%d,%d)",(MyGLenum *) &target,&v[0],&v[1],&v[2]);
#ifndef PVWIN32
          glMultiTexCoord3ivARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3sARB(",21) == 0)
        {
          GLenum target; GLshort s; GLshort t; GLshort r;
          sscanf(line,"glMultiTexCoord3sARB(%d,%hd,%hd,%hd)",(MyGLenum *) &target,&s,&t,&r);
#ifndef PVWIN32
          glMultiTexCoord3sARB(target,s,t,r);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord3svARB(",22) == 0)
        {
          GLenum target; GLshort v[4];
          sscanf(line,"glMultiTexCoord3svARB(%d,%hd,%hd,%hd)",(MyGLenum *) &target,&v[0],&v[1],&v[2]);
#ifndef PVWIN32
          glMultiTexCoord3svARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4dARB(",21) == 0)
        {
          GLenum target; GLfloat s; GLfloat t; GLfloat r; GLfloat q;
          sscanf(line,"glMultiTexCoord4dARB(%d,%f,%f,%f,%f)",(MyGLenum *) &target,&s,&t,&r,&q);
#ifndef PVWIN32
          glMultiTexCoord4dARB(target,s,t,r,q);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4dvARB(",22) == 0)
        {
          GLenum target; GLfloat v[4];
          sscanf(line,"glMultiTexCoord4dvARB(%d,%f,%f,%f,%f)",(MyGLenum *) &target,&v[0],&v[1],&v[2],&v[3]);
#ifndef PVWIN32
          glMultiTexCoord4fvARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4fARB(",21) == 0)
        {
          GLenum target; GLfloat s; GLfloat t; GLfloat r; GLfloat q;
          sscanf(line,"glMultiTexCoord4fARB(%d,%f,%f,%f,%f)",(MyGLenum *) &target,&s,&t,&r,&q);
#ifndef PVWIN32
          glMultiTexCoord4fARB(target,s,t,r,q);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4fvARB(",22) == 0)
        {
          GLenum target; GLfloat v[4];
          sscanf(line,"glMultiTexCoord4fvARB(%d,%f,%f,%f,%f)",(MyGLenum *) &target,&v[0],&v[1],&v[2],&v[3]);
#ifndef PVWIN32
          glMultiTexCoord4fvARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4iARB(",21) == 0)
        {
          GLenum target; GLint s; GLint t; GLint r; GLint q;
          sscanf(line,"glMultiTexCoord4iARB(%d,%d,%d,%d,%d)",(MyGLenum *) &target,&s,&t,&r,&q);
#ifndef PVWIN32
          glMultiTexCoord4iARB(target,s,t,r,q);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4ivARB(",22) == 0)
        {
          GLenum target; GLint v[4];
          sscanf(line,"glMultiTexCoord4ivARB(%d,%d,%d,%d,%d)",(MyGLenum *) &target,&v[0],&v[1],&v[2],&v[3]);
#ifndef PVWIN32
          glMultiTexCoord4ivARB(target,v);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4sARB(",21) == 0)
        {
          GLenum target; GLshort s; GLshort t; GLshort r; GLshort q;
          sscanf(line,"glMultiTexCoord4sARB(%d,%hd,%hd,%hd,%hd)",(MyGLenum *) &target,&s,&t,&r,&q);
#ifndef PVWIN32
          glMultiTexCoord4sARB(target,s,t,r,q);
#endif
        }
        else if(strncmp(line,"glMultiTexCoord4svARB(",22) == 0)
        {
          GLenum target; GLshort v[4];
          sscanf(line,"glMultiTexCoord4svARB(%d,%hd,%hd,%hd,%hd)",(MyGLenum *) &target,&v[0],&v[1],&v[2],&v[3]);
#ifndef PVWIN32
          glMultiTexCoord4svARB(target,v);
#endif
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'N':
        if(strncmp(line,"glNewList(",10) == 0)
        {
          GLuint list; GLenum mode;
          sscanf(line,"glNewList(%d,%d)",&list,(MyGLenum *) &mode);
          glNewList(list,mode);
          QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        }
        else if(strncmp(line,"glNormal3b(",11) == 0)
        {
          int x,y,z;
          sscanf(line,"glNormal3b(%d,%d,%d)",&x,&y,&z);
          glNormal3b((GLbyte)x,(GLbyte)y,(GLbyte)z);
        }
        else if(strncmp(line,"glNormal3bv(",12) == 0)
        {
          GLbyte v[4]; int x,y,z;
          sscanf(line,"glNormal3bv(%d,%d,%d)",&x,&y,&z);
          v[0] = (GLbyte) x;
          v[1] = (GLbyte) y;
          v[2] = (GLbyte) z;
          glNormal3bv(v);
        }
        else if(strncmp(line,"glNormal3d(",11) == 0)
        {
          GLfloat nx; GLfloat ny; GLfloat nz;
          sscanf(line,"glNormal3d(%f,%f,%f)",&nx,&ny,&nz);
          glNormal3d(nx,ny,nz);
        }
        else if(strncmp(line,"glNormal3dv(",12) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glNormal3dv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glNormal3fv(v);
        }
        else if(strncmp(line,"glNormal3f(",11) == 0)
        {
          GLfloat nx; GLfloat ny; GLfloat nz;
          sscanf(line,"glNormal3f(%f,%f,%f)",&nx,&ny,&nz);
          glNormal3f(nx,ny,nz);
        }
        else if(strncmp(line,"glNormal3fv(",12) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glNormal3fv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glNormal3fv(v);
        }
        else if(strncmp(line,"glNormal3i(",11) == 0)
        {
          GLint nx; GLint ny; GLint nz;
          sscanf(line,"glNormal3i(%d,%d,%d)",&nx,&ny,&nz);
          glNormal3i(nx,ny,nz);
        }
        else if(strncmp(line,"glNormal3iv(",12) == 0)
        {
          GLint v[4];
          sscanf(line,"glNormal3iv(%d,%d,%d)",&v[0],&v[1],&v[2]);
          glNormal3iv(v);
        }
        else if(strncmp(line,"glNormal3s(",11) == 0)
        {
          GLshort nx; GLshort ny; GLshort nz;
          sscanf(line,"glNormal3s(%hd,%hd,%hd)",&nx,&ny,&nz);
          glNormal3s(nx,ny,nz);
        }
        else if(strncmp(line,"glNormal3sv(",12) == 0)
        {
          GLshort v[4];
          sscanf(line,"glNormal3sv(%hd,%hd,%hd)",&v[0],&v[1],&v[2]);
          glNormal3sv(v);
        }
        else if(strncmp(line,"glNormalPointer(",16) == 0)
        {
          GLenum type; GLsizei stride; GLvoid *ptr;
          sscanf(line,"glNormalPointer(%d,%d)",(MyGLenum *) &type,&stride);
          ptr = glgetbytes();
          glNormalPointer(type,stride,ptr);
          free(ptr);
        }
        else if(strncmp(line,"glNormalPointerEXT(",19) == 0)
        {
          GLenum type; GLsizei stride; GLsizei count; GLvoid *ptr;
          sscanf(line,"glNormalPointerEXT(%d,%d,%d)",(MyGLenum *) &type,&stride,&count);
          ptr = glgetbytes();
#ifndef PVWIN32
          //glNormalPointerEXT(type,stride,count,ptr);
#endif
          free(ptr);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'O':
        if(strncmp(line,"glOrtho(",8) == 0)
        {
          GLfloat left; GLfloat right; GLfloat bottom; GLfloat top; GLfloat near_val; GLfloat far_val;
          sscanf(line,"glOrtho(%f,%f,%f,%f,%f,%f)",&left,&right,&bottom,&top,&near_val,&far_val);
          glOrtho(left,right,bottom,top,near_val,far_val);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'P':
        if(strncmp(line,"glPassThrough(",14) == 0)
        {
          GLfloat token;
          sscanf(line,"glPassThrough(%f)",&token);
          glPassThrough(token);
        }
        else if(strncmp(line,"glPixelMapfv(",13) == 0)
        {
          GLenum map; GLint mapsize; GLfloat *values;
          sscanf(line,"glPixelMapfv(%d,%d)",(MyGLenum *) &map,&mapsize);
          values = (GLfloat *) glgetbytes();
          glPixelMapfv(map,mapsize,values);
          free(values);
        }
        else if(strncmp(line,"glPixelMapuiv(",14) == 0)
        {
          GLenum map; GLint mapsize; GLuint *values;
          sscanf(line,"glPixelMapuiv(%d,%d)",(MyGLenum *) &map,&mapsize);
          values = (GLuint *) glgetbytes();
          glPixelMapuiv(map,mapsize,values);
          free(values);
        }
        else if(strncmp(line,"glPixelMapusv(",14) == 0)
        {
          GLenum map; GLint mapsize; GLushort *values;
          sscanf(line,"glPixelMapusv(%d,%d)",(MyGLenum *) &map,&mapsize);
          values = (GLushort *) glgetbytes();
          glPixelMapusv(map,mapsize,values);
          free(values);
        }
        else if(strncmp(line,"glPixelStoref(",14) == 0)
        {
          GLenum pname; GLfloat param;
          sscanf(line,"glPixelStoref(%d,%f)",(MyGLenum *) &pname,&param);
          glPixelStoref(pname,param);
        }
        else if(strncmp(line,"glPixelStorei(",14) == 0)
        {
          GLenum pname; GLint param;
          sscanf(line,"glPixelStorei(%d,%d)",(MyGLenum *) &pname,&param);
          glPixelStorei(pname,param);
        }
        else if(strncmp(line,"glPixelTransferf(",17) == 0)
        {
          GLenum pname; GLfloat param;
          sscanf(line,"glPixelTransferf(%d,%f)",(MyGLenum *) &pname,&param);
          glPixelTransferf(pname,param);
        }
        else if(strncmp(line,"glPixelTransferi(",17) == 0)
        {
          GLenum pname; GLint param;
          sscanf(line,"glPixelTransferi(%d,%d)",(MyGLenum *) &pname,&param);
          glPixelTransferi(pname,param);
        }
        else if(strncmp(line,"glPixelZoom(",12) == 0)
        {
          GLfloat xfactor; GLfloat yfactor;
          sscanf(line,"glPixelZoom(%f,%f)",&xfactor,&yfactor);
          glPixelZoom(xfactor,yfactor);
        }
        else if(strncmp(line,"glPointParameterfEXT(",21) == 0)

        {
          GLenum pname; GLfloat param;
          sscanf(line,"glPointParameterfEXT(%d,%f)",(MyGLenum *) &pname,&param);
#ifndef PVWIN32
          //glPointParameterfEXT(pname,param);
#endif
        }
        else if(strncmp(line,"glPointParameterfvEXT(",22) == 0)
        {
          GLenum pname; GLfloat params;
          sscanf(line,"glPointParameterfvEXT(%d,%f)",(MyGLenum *) &pname,&params);
#ifndef PVWIN32
          //glPointParameterfvEXT(pname,&params);
#endif
        }
        else if(strncmp(line,"glPointSize(",12) == 0)
        {
          GLfloat size;
          sscanf(line,"glPointSize(%f)",&size);
          glPointSize(size);
        }
        else if(strncmp(line,"glPolygonMode(",14) == 0)
        {
          GLenum face; GLenum mode;
          sscanf(line,"glPolygonMode(%d,%d)",(MyGLenum *) &face,(MyGLenum *) &mode);
          glPolygonMode(face,mode);
        }
        else if(strncmp(line,"glPolygonOffset(",16) == 0)
        {
          GLfloat factor; GLfloat units;
          sscanf(line,"glPolygonOffset(%f,%f)",&factor,&units);
          glPolygonOffset(factor,units);
        }
        else if(strncmp(line,"glPolygonOffsetEXT(",19) == 0)
        {
          GLfloat factor; GLfloat bias;
          sscanf(line,"glPolygonOffsetEXT(%f,%f)",&factor,&bias);
#ifndef PVWIN32
//        glPolygonOffsetEXT(factor,bias);
#endif
        }
        else if(strncmp(line,"glPolygonStipple(",17) == 0)
        {
          GLubyte *mask;
          sscanf(line,"glPolygonStipple()");
          mask = (GLubyte *) glgetbytes();
          glPolygonStipple(mask);
          free(mask);
        }
        else if(strncmp(line,"glPopAttrib(",12) == 0)
        {
          glPopAttrib();
        }
        else if(strncmp(line,"glPopClientAttrib(",18) == 0)
        {
          glPopClientAttrib();
        }
        else if(strncmp(line,"glPopMatrix(",12) == 0)
        {
          glPopMatrix();
        }
        else if(strncmp(line,"glPopName(",10) == 0)
        {
          glPopName();
        }
        else if(strncmp(line,"glPrioritizeTextures(",21) == 0)
        {
          GLsizei n; GLuint *textures; GLclampf *priorities;
          sscanf(line,"glPrioritizeTextures(%d)",&n);
          textures   = (GLuint *)   glgetbytes();
          priorities = (GLclampf *) glgetbytes();
          glPrioritizeTextures(n,textures,priorities);
          free(textures);
          free(priorities);
        }
        else if(strncmp(line,"glPrioritizeTexturesEXT(",24) == 0)
        {
          GLsizei n; GLuint *textures; GLclampf *priorities;
          sscanf(line,"glPrioritizeTexturesEXT(%d)",&n);
          textures   = (GLuint *)   glgetbytes();
          priorities = (GLclampf *) glgetbytes();
#ifndef PVWIN32
          //glPrioritizeTexturesEXT(n,textures,priorities);
#endif
          free(textures);
          free(priorities);
        }
        else if(strncmp(line,"glPushAttrib(",13) == 0)
        {
          GLbitfield mask;
          sscanf(line,"glPushAttrib(%d)",&mask);
          glPushAttrib(mask);
        }
        else if(strncmp(line,"glPushClientAttrib(",19) == 0)
        {
          GLbitfield mask;
          sscanf(line,"glPushClientAttrib(%d)",&mask);
          glPushClientAttrib(mask);
        }
        else if(strncmp(line,"glPushMatrix(",13) == 0)
        {
          glPushMatrix();
        }
        else if(strncmp(line,"glPushName(",11) == 0)
        {
          GLuint name;
          sscanf(line,"glPushName(%d)",&name);
          glPushName(name);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'R':
        if(strncmp(line,"glRasterPos2d(",14) == 0)
        {
          GLfloat x; GLfloat y;
          sscanf(line,"glRasterPos2d(%f,%f)",&x,&y);
          glRasterPos2d(x,y);
        }
        else if(strncmp(line,"glRasterPos2dv(",15) == 0)
        {
          GLfloat v[2];
          sscanf(line,"glRasterPos2dv(%f,%f)",&v[0],&v[1]);
          glRasterPos2fv(v);
        }
        else if(strncmp(line,"glRasterPos2f(",14) == 0)
        {
          GLfloat x; GLfloat y;
          sscanf(line,"glRasterPos2f(%f,%f)",&x,&y);
          glRasterPos2f(x,y);
        }
        else if(strncmp(line,"glRasterPos2fv(",15) == 0)
        {
          GLfloat v[2];
          sscanf(line,"glRasterPos2fv(%f,%f)",&v[0],&v[1]);
          glRasterPos2fv(v);
        }
        else if(strncmp(line,"glRasterPos2i(",14) == 0)
        {
          GLint x; GLint y;
          sscanf(line,"glRasterPos2i(%d,%d)",&x,&y);
          glRasterPos2i(x,y);
        }
        else if(strncmp(line,"glRasterPos2iv(",15) == 0)
        {
          GLint v[2];
          sscanf(line,"glRasterPos2iv(%d,%d)",&v[0],&v[1]);
          glRasterPos2iv(v);
        }
        else if(strncmp(line,"glRasterPos2s(",14) == 0)
        {
          GLshort x; GLshort y;
          sscanf(line,"glRasterPos2s(%hd,%hd)",&x,&y);
          glRasterPos2s(x,y);
        }
        else if(strncmp(line,"glRasterPos2sv(",15) == 0)

        {
          GLshort v[2];
          sscanf(line,"glRasterPos2sv(%hd,%hd)",&v[0],&v[1]);
          glRasterPos2sv(v);
        }
        else if(strncmp(line,"glRasterPos3d(",14) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glRasterPos3d(%f,%f,%f)",&x,&y,&z);
          glRasterPos3d(x,y,z);
        }
        else if(strncmp(line,"glRasterPos3dv(",15) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glRasterPos3dv(%f,%f,%f)",&v[0],&v[1],&v[3]);
          glRasterPos3fv(v);
        }
        else if(strncmp(line,"glRasterPos3f(",14) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glRasterPos3f(%f,%f,%f)",&x,&y,&z);
          glRasterPos3f(x,y,z);
        }
        else if(strncmp(line,"glRasterPos3fv(",15) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glRasterPos3fv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glRasterPos3fv(v);
        }
        else if(strncmp(line,"glRasterPos3i(",14) == 0)
        {
          GLint x; GLint y; GLint z;
          sscanf(line,"glRasterPos3i(%d,%d,%d)",&x,&y,&z);
          glRasterPos3i(x,y,z);
        }
        else if(strncmp(line,"glRasterPos3iv(",15) == 0)
        {
          GLint v[4];
          sscanf(line,"glRasterPos3iv(%d,%d,%d)",&v[0],&v[1],&v[2]);
          glRasterPos3iv(v);
        }
        else if(strncmp(line,"glRasterPos3s(",14) == 0)
        {
          GLshort x; GLshort y; GLshort z;
          sscanf(line,"glRasterPos3s(%hd,%hd,%hd)",&x,&y,&z);
          glRasterPos3s(x,y,z);
        }
        else if(strncmp(line,"glRasterPos3sv(",15) == 0)
        {
          GLshort v[4];
          sscanf(line,"glRasterPos3sv(%hd,%hd,%hd)",&v[0],&v[1],&v[2]);
          glRasterPos3sv(v);
        }
        else if(strncmp(line,"glRasterPos4d(",14) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z; GLfloat w;
          sscanf(line,"glRasterPos4d(%f,%f,%f,%f)",&x,&y,&z,&w);
          glRasterPos4d(x,y,z,w);
        }
        else if(strncmp(line,"glRasterPos4dv(",15) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glRasterPos4dv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glRasterPos4fv(v);
        }
        else if(strncmp(line,"glRasterPos4f(",14) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z; GLfloat w;
          sscanf(line,"glRasterPos4f(%f,%f,%f,%f)",&x,&y,&z,&w);
          glRasterPos4f(x,y,z,w);
        }
        else if(strncmp(line,"glRasterPos4fv(",15) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glRasterPos4fv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glRasterPos4fv(v);
        }
        else if(strncmp(line,"glRasterPos4i(",14) == 0)
        {
          GLint x; GLint y; GLint z; GLint w;
          sscanf(line,"glRasterPos4i(%d,%d,%d,%d)",&x,&y,&z,&w);
          glRasterPos4i(x,y,z,w);
        }
        else if(strncmp(line,"glRasterPos4iv(",15) == 0)
        {
          GLint v[4];
          sscanf(line,"glRasterPos4iv(%d,%d,%d,%d)",&v[0],&v[1],&v[2],&v[3]);
          glRasterPos4iv(v);
        }
        else if(strncmp(line,"glRasterPos4s(",14) == 0)
        {
          GLshort x; GLshort y; GLshort z; GLshort w;
          sscanf(line,"glRasterPos4s(%hd,%hd,%hd,%hd)",&x,&y,&z,&w);
          glRasterPos4s(x,y,z,w);
        }
        else if(strncmp(line,"glRasterPos4sv(",15) == 0)
        {
          GLshort v[4];
          sscanf(line,"glRasterPos4sv(%hd,%hd,%hd,%hd)",&v[0],&v[1],&v[2],&v[3]);
          glRasterPos4sv(v);
        }
        else if(strncmp(line,"glReadBuffer(",13) == 0)
        {
          GLenum mode;
          sscanf(line,"glReadBuffer(%d)",(MyGLenum *) &mode);
          glReadBuffer(mode);
        }

        /* not implemented jet
        GLAPI void GLAPIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
        {
          sscanf(line,"glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )");
        }
        */
        else if(strncmp(line,"glRectd(",8) == 0)
        {
          GLfloat x1; GLfloat y1; GLfloat x2; GLfloat y2;
          sscanf(line,"glRectd(%f,%f,%f,%f)",&x1,&y1,&x2,&y2);
          glRectd(x1,y1,x2,y2);
        }
        else if(strncmp(line,"glRectdv(",9) == 0)
        {
          GLfloat v1[2]; GLfloat v2[2];
          sscanf(line,"glRectfv(%f,%f,%f,%f)",&v1[0],&v1[1],&v2[0],&v2[1]);
          glRectfv(v1,v2);
        }
        else if(strncmp(line,"glRectf(",8) == 0)
        {
          GLfloat x1; GLfloat y1; GLfloat x2; GLfloat y2;
          sscanf(line,"glRectf(%f,%f,%f,%f)",&x1,&y1,&x2,&y2);
          glRectf(x1,y1,x2,y2);
        }
        else if(strncmp(line,"glRectfv(",9) == 0)
        {
          GLfloat v1[2]; GLfloat v2[2];
          sscanf(line,"glRectfv(%f,%f,%f,%f)",&v1[0],&v1[1],&v2[0],&v2[1]);
          glRectfv(v1,v2);
        }
        else if(strncmp(line,"glRecti(",8) == 0)
        {
          GLint x1; GLint y1; GLint x2; GLint y2;
          sscanf(line,"glRecti(%d,%d,%d,%d)",&x1,&y1,&x2,&y2);
          glRecti(x1,y1,x2,y2);
        }
        else if(strncmp(line,"glRectiv(",9) == 0)
        {
          GLint v1[2]; GLint v2[2];
          sscanf(line,"glRectiv(%d,%d,%d,%d)",&v1[0],&v1[1],&v2[0],&v2[1]);
          glRectiv(v1,v2);
        }
        else if(strncmp(line,"glRects(",8) == 0)
        {
          GLshort x1; GLshort y1; GLshort x2; GLshort y2;
          sscanf(line,"glRects(%hd,%hd,%hd,%hd)",&x1,&y1,&x2,&y2);
          glRects(x1,y1,x2,y2);
        }
        else if(strncmp(line,"glRectsv(",9) == 0)
        {
          GLshort v1[2]; GLshort v2[2];
          sscanf(line,"glRectsv(%hd,%hd,%hd,%hd)",&v1[0],&v1[1],&v2[0],&v2[1]);
          glRectsv(v1,v2);
        }
        else if(strncmp(line,"glResetHistogram(",17) == 0)
        {
          GLenum target;
          sscanf(line,"glResetHistogram(%d)",(MyGLenum *) &target);
#ifndef PVWIN32
          glResetHistogram(target);
#endif
        }
        else if(strncmp(line,"glResetMinmax(",14) == 0)
        {
          GLenum target;
          sscanf(line,"glResetMinmax(%d)",(MyGLenum *) &target);
#ifndef PVWIN32
          glResetMinmax(target);
#endif
        }
        else if(strncmp(line,"glResizeBuffersMESA(",20) == 0)
        {
#ifndef PVWIN32
//        glResizeBuffersMESA();
#endif
        }
        else if(strncmp(line,"glRotated(",10) == 0)
        {
          GLfloat angle; GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glRotated(%f,%f,%f,%f)",&angle,&x,&y,&z);
          glRotated(angle,x,y,z);
        }
        else if(strncmp(line,"glRotatef(",10) == 0)
        {
          GLfloat angle; GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glRotatef(%f,%f,%f,%f)",&angle,&x,&y,&z);
          glRotatef(angle,x,y,z);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'S':
        if(strncmp(line,"glScaled(",9) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glScaled(%f,%f,%f)",&x,&y,&z);
          glScaled(x,y,z);
        }
        else if(strncmp(line,"glScalef(",9) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glScalef(%f,%f,%f)",&x,&y,&z);
          glScalef(x,y,z);
        }
        else if(strncmp(line,"glScissor(",10) == 0)
        {
          GLint x; GLint y; GLsizei width; GLsizei height;

          sscanf(line,"glScissor(%d,%d,%d,%d)",&x,&y,&width,&height);
          glScissor(x,y,width,height);
        }

        /* not implemented jet
        GLAPI void GLAPIENTRY glSelectBuffer( GLsizei size, GLuint *buffer )
        {
          sscanf(line,"glSelectBuffer( GLsizei size, GLuint *buffer )");
        }
        */
        else if(strncmp(line,"glSeparableFilter2D(",20) == 0)
        {
          GLenum target; GLenum internalformat; GLsizei width; GLsizei height; GLenum format; GLenum type; GLvoid *row; GLvoid *column;
          sscanf(line,"glSeparableFilter2D(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &internalformat,&width,&height,(MyGLenum *) &format,(MyGLenum *) &type);
          row    = glgetbytes();
          column = glgetbytes();
#ifndef PVWIN32
          glSeparableFilter2D(target,internalformat,width,height,format,type,row,column);
#endif
          free(row);
          free(column);
        }
        else if(strncmp(line,"glShadeModel(",13) == 0)
        {
          GLenum mode;
          sscanf(line,"glShadeModel(%d)",(MyGLenum *) &mode);
          glShadeModel(mode);
        }
        else if(strncmp(line,"glStencilFunc(",14) == 0)
        {
          GLenum func; GLint ref; GLuint mask;
          sscanf(line,"glStencilFunc(%d,%d,%d)",(MyGLenum *) &func,&ref,&mask);
          glStencilFunc(func,ref,mask);
        }
        else if(strncmp(line,"glStencilMask(",14) == 0)
        {
          GLuint mask;
          sscanf(line,"glStencilMask(%d)",&mask);
          glStencilMask(mask);
        }
        else if(strncmp(line,"glStencilOp(",12) == 0)
        {
          GLenum fail; GLenum zfail; GLenum zpass;
          sscanf(line,"glStencilOp(%d,%d,%d)",(MyGLenum *) &fail,(MyGLenum *) &zfail,(MyGLenum *) &zpass);
          glStencilOp(fail,zfail,zpass);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'T':
        if(strncmp(line,"glTexCoord1d(",13) == 0)
        {
          GLfloat s;
          sscanf(line,"glTexCoord1d(%f)",&s);
          glTexCoord1d(s);
        }
        else if(strncmp(line,"glTexCoord1dv(",14) == 0)
        {
          GLfloat v;
          sscanf(line,"glTexCoord1dv(%f)",&v);
          glTexCoord1fv(&v);
        }
        else if(strncmp(line,"glTexCoord1f(",13) == 0)
        {
          GLfloat s;
          sscanf(line,"glTexCoord1f(%f)",&s);
          glTexCoord1f(s);
        }
        else if(strncmp(line,"glTexCoord1fv(",14) == 0)
        {
          GLfloat v;
          sscanf(line,"glTexCoord1fv(%f)",&v);
          glTexCoord1fv(&v);
        }
        else if(strncmp(line,"glTexCoord1i(",13) == 0)
        {
          GLint s;
          sscanf(line,"glTexCoord1i(%d)",&s);
          glTexCoord1i(s);
        }
        else if(strncmp(line,"glTexCoord1iv(",14) == 0)
        {
          GLint v;
          sscanf(line,"glTexCoord1iv(%d)",&v);
          glTexCoord1iv(&v);
        }
        else if(strncmp(line,"glTexCoord1s(",13) == 0)
        {
          GLshort s;
          sscanf(line,"glTexCoord1s(%hd)",&s);
          glTexCoord1s(s);
        }
        else if(strncmp(line,"glTexCoord1sv(",14) == 0)
        {
          GLshort v;
          sscanf(line,"glTexCoord1sv(%hd)",&v);
          glTexCoord1sv(&v);
        }
        else if(strncmp(line,"glTexCoord2d(",13) == 0)
        {
          GLfloat s; GLfloat t;
          sscanf(line,"glTexCoord2d(%f,%f)",&s,&t);
          glTexCoord2d(s,t);
        }
        else if(strncmp(line,"glTexCoord2dv(",14) == 0)
        {
          GLfloat v[2];
          sscanf(line,"glTexCoord2dv(%f,%f)",&v[0],&v[1]);
          glTexCoord2fv(v);
        }
        else if(strncmp(line,"glTexCoord2f(",13) == 0)
        {
          GLfloat s; GLfloat t;
          sscanf(line,"glTexCoord2f(%f,%f)",&s,&t);
          glTexCoord2f(s,t);
        }
        else if(strncmp(line,"glTexCoord2fv(",14) == 0)
        {
          GLfloat v[2];
          sscanf(line,"glTexCoord2fv(%f,%f)",&v[0],&v[1]);
          glTexCoord2fv(v);
        }
        else if(strncmp(line,"glTexCoord2i(",13) == 0)
        {
          GLint s; GLint t;
          sscanf(line,"glTexCoord2i(%d,%d)",&s,&t);
          glTexCoord2i(s,t);
        }
        else if(strncmp(line,"glTexCoord2iv(",14) == 0)
        {
          GLint v[2];
          sscanf(line,"glTexCoord2iv(%d,%d)",&v[0],&v[1]);
          glTexCoord2iv(v);
        }
        else if(strncmp(line,"glTexCoord2s(",13) == 0)
        {
          GLshort s; GLshort t;
          sscanf(line,"glTexCoord2s(%hd,%hd)",&s,&t);
          glTexCoord2s(s,t);
        }
        else if(strncmp(line,"glTexCoord2sv(",14) == 0)
        {
          GLshort v[2];
          sscanf(line,"glTexCoord2sv(%hd,%hd)",&v[0],&v[1]);
          glTexCoord2sv(v);
        }
        else if(strncmp(line,"glTexCoord3d(",13) == 0)
        {
          GLfloat s; GLfloat t; GLfloat r;
          sscanf(line,"glTexCoord3d(%f,%f,%f)",&s,&t,&r);
          glTexCoord3d(s,t,r);
        }
        else if(strncmp(line,"glTexCoord3dv(",14) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glTexCoord3dv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glTexCoord3fv(v);
        }
        else if(strncmp(line,"glTexCoord3f(",13) == 0)
        {
          GLfloat s; GLfloat t; GLfloat r;
          sscanf(line,"glTexCoord3f(%f,%f,%f)",&s,&t,&r);
          glTexCoord3f(s,t,r);
        }
        else if(strncmp(line,"glTexCoord3fv(",14) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glTexCoord3fv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glTexCoord3fv(v);
        }
        else if(strncmp(line,"glTexCoord3i(",13) == 0)
        {
          GLint s; GLint t; GLint r ;
          sscanf(line,"glTexCoord3i(%d,%d,%d)",&s,&t,&r);
          glTexCoord3i(s,t,r);
        }
        else if(strncmp(line,"glTexCoord3iv(",14) == 0)
        {
          GLint v[4];
          sscanf(line,"glTexCoord3iv(%d,%d,%d)",&v[0],&v[1],&v[2]);
          glTexCoord3iv(v);
        }
        else if(strncmp(line,"glTexCoord3s(",13) == 0)
        {
          GLshort s; GLshort t; GLshort r;
          sscanf(line,"glTexCoord3s(%hd,%hd,%hd)",&s,&t,&r);
          glTexCoord3s(s,t,r);
        }
        else if(strncmp(line,"glTexCoord3sv(",14) == 0)
        {
          GLshort v[4];
          sscanf(line,"glTexCoord3sv(%hd,%hd,%hd)",&v[0],&v[1],&v[2]);
          glTexCoord3sv(v);
        }
        else if(strncmp(line,"glTexCoord4d(",13) == 0)
        {
          GLfloat s; GLfloat t; GLfloat r; GLfloat q;
          sscanf(line,"glTexCoord4d(%f,%f,%f,%f)",&s,&t,&r,&q);
          glTexCoord4d(s,t,r,q);
        }
        else if(strncmp(line,"glTexCoord4dv(",14) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glTexCoord4dv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glTexCoord4fv(v);
        }
        else if(strncmp(line,"glTexCoord4f(",13) == 0)
        {
          GLfloat s; GLfloat t; GLfloat r; GLfloat q;
          sscanf(line,"glTexCoord4f(%f,%f,%f,%f)",&s,&t,&r,&q);
          glTexCoord4f(s,t,r,q);
        }
        else if(strncmp(line,"glTexCoord4fv(",14) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glTexCoord4fv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glTexCoord4fv(v);
        }
        else if(strncmp(line,"glTexCoord4i(",13) == 0)
        {
          GLint s; GLint t; GLint r; GLint q;
          sscanf(line,"glTexCoord4i(%d,%d,%d,%d)",&s,&t,&r,&q);
          glTexCoord4i(s,t,r,q);
        }
        else if(strncmp(line,"glTexCoord4iv(",14) == 0)
        {
          GLint v[4];
          sscanf(line,"glTexCoord4iv(%d,%d,%d,%d)",&v[0],&v[1],&v[2],&v[3]);
          glTexCoord4iv(v);
        }
        else if(strncmp(line,"glTexCoord4s(",13) == 0)
        {
          GLshort s; GLshort t; GLshort r; GLshort q;
          sscanf(line,"glTexCoord4s(%hd,%hd,%hd,%hd)",&s,&t,&r,&q);
          glTexCoord4s(s,t,r,q);
        }
        else if(strncmp(line,"glTexCoord4sv(",14) == 0)
        {
          GLshort v[4];
          sscanf(line,"glTexCoord4sv(%hd,%hd,%hd,%hd)",&v[0],&v[1],&v[2],&v[3]);
          glTexCoord4sv(v);
        }
        else if(strncmp(line,"glTexCoordPointer(",18) == 0)
        {
          GLint size; GLenum type; GLsizei stride; GLvoid *ptr;
          sscanf(line,"glTexCoordPointer(%d,%d,%d)",&size,(MyGLenum *) &type,&stride);
          ptr = glgetbytes();
          glTexCoordPointer(size,type,stride,ptr);
          free(ptr);
        }
        else if(strncmp(line,"glTexCoordPointerEXT(",21) == 0)
        {
          GLint size; GLenum type; GLsizei stride; GLsizei count; GLvoid *ptr;
          sscanf(line,"glTexCoordPointerEXT(%d,%d,%d,%d)",&size,(MyGLenum *) &type,&stride,&count);
          ptr = glgetbytes();
#ifndef PVWIN32
          //glTexCoordPointerEXT(size,type,stride,count,ptr);
#endif
          free(ptr);
        }
        else if(strncmp(line,"glTexEnvf(",10) == 0)
        {
          GLenum target; GLenum pname; GLfloat param;
          sscanf(line,"glTexEnvf(%d,%d,%f)",(MyGLenum *) &target,(MyGLenum *) &pname,&param);
          glTexEnvf(target,pname,param);
        }
        else if(strncmp(line,"glTexEnvfv(",11) == 0)
        {
          GLenum target; GLenum pname; GLfloat params;
          sscanf(line,"glTexEnvfv(%d,%d,%f)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
          glTexEnvfv(target,pname,&params);
        }
        else if(strncmp(line,"glTexEnvi(",10) == 0)
        {
          GLenum target; GLenum pname; GLint param;
          sscanf(line,"glTexEnvi(%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &pname,&param);
          glTexEnvi(target,pname,param);
        }
        else if(strncmp(line,"glTexEnviv(",11) == 0)
        {
          GLenum target; GLenum pname; GLint params;
          sscanf(line,"glTexEnviv(%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
          glTexEnviv(target,pname,&params);
        }
        else if(strncmp(line,"glTexGend(",10) == 0)
        {
          GLenum coord; GLenum pname; GLfloat param;
          sscanf(line,"glTexGend(%d,%d,%f)",(MyGLenum *) &coord,(MyGLenum *) &pname,&param);
          glTexGend(coord,pname,param);
        }
        else if(strncmp(line,"glTexGendv(",11) == 0)
        {
          GLenum coord; GLenum pname; GLfloat params;
          sscanf(line,"glTexGendv(%d,%d,%f)",(MyGLenum *) &coord,(MyGLenum *) &pname,&params);
          glTexGenfv(coord,pname,&params);
        }
        else if(strncmp(line,"glTexGenf(",10) == 0)
        {
          GLenum coord; GLenum pname; GLfloat param;
          sscanf(line,"glTexGenf(%d,%d,%f)",(MyGLenum *) &coord,(MyGLenum *) &pname,&param);
          glTexGenf(coord,pname,param);
        }
        else if(strncmp(line,"glTexGenfv(",11) == 0)
        {
          GLenum coord; GLenum pname; GLfloat params;
          sscanf(line,"glTexGenfv(%d,%d,%f)",(MyGLenum *) &coord,(MyGLenum *) &pname,&params);
          glTexGenfv(coord,pname,&params);
        }
        else if(strncmp(line,"glTexGeni(",10) == 0)
        {
          GLenum coord; GLenum pname; GLint param;
          sscanf(line,"glTexGeni(%d,%d,%d)",(MyGLenum *) &coord,(MyGLenum *) &pname,&param);
          glTexGeni(coord,pname,param);
        }
        else if(strncmp(line,"glTexGeniv(",11) == 0)
        {
          GLenum coord; GLenum pname; GLint params;
          sscanf(line,"glTexGeniv(%d,%d,%d)",(MyGLenum *) &coord,(MyGLenum *) &pname,&params);
          glTexGeniv(coord,pname,&params);
        }
        else if(strncmp(line,"glTexImage1D(",13) == 0)
        {
          GLenum target; GLint level; GLint internalFormat; GLsizei width; GLint border; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexImage1D(%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&internalFormat,&width,&border,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
          glTexImage1D(target,level,internalFormat,width,border,format,type,pixels);
          free(pixels);
        }
        else if(strncmp(line,"glTexImage2D(",13) == 0)
        {
          GLenum target; GLint level; GLint internalFormat; GLsizei width; GLsizei height; GLint border; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexImage2D(%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&internalFormat,&width,&height,&border,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
          glTexImage2D(target,level,internalFormat,width,height,border,format,type,pixels);
          free(pixels);
        }
        else if(strncmp(line,"glTexImage3D(",13) == 0)
        {
          GLenum target; GLint level; GLint internalFormat; GLsizei width; GLsizei height; GLsizei depth; GLint border; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexImage3D(%d,%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&internalFormat,&width,&height,&depth,&border,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
#ifndef PVWIN32
          glTexImage3D(target,level,internalFormat,width,height,depth,border,format,type,pixels);
#endif
          free(pixels);
        }
        else if(strncmp(line,"glTexImage3DEXT(",16) == 0)
        {
          GLenum target; GLint level; GLenum internalFormat; GLsizei width; GLsizei height; GLsizei depth; GLint border; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexImage3DEXT(%d,%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,(MyGLenum *) &internalFormat,&width,&height,&depth,&border,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
#ifndef PVWIN32
          //glTexImage3DEXT(target,level,internalFormat,width,height,depth,border,format,type,pixels);
#endif
          free(pixels);
        }
        else if(strncmp(line,"glTexParameterf(",16) == 0)
        {
          GLenum target; GLenum pname; GLfloat param;
          sscanf(line,"glTexParameterf(%d,%d,%f)",(MyGLenum *) &target,(MyGLenum *) &pname,&param);
          glTexParameterf(target,pname,param);
        }
        else if(strncmp(line,"glTexParameterfv(",17) == 0)
        {
          GLenum target; GLenum pname; GLfloat params;
          sscanf(line,"glTexParameterfv(%d,%d,%f)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
          glTexParameterfv(target,pname,&params);
        }
        else if(strncmp(line,"glTexParameteri(",16) == 0)
        {
          GLenum target; GLenum pname; GLint param;
          sscanf(line,"glTexParameteri(%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &pname,&param);
          glTexParameteri(target,pname,param);
        }
        else if(strncmp(line,"glTexParameteriv(",17) == 0)
        {
          GLenum target; GLenum pname; GLint params;
          sscanf(line,"glTexParameteriv(%d,%d,%d)",(MyGLenum *) &target,(MyGLenum *) &pname,&params);
          glTexParameteriv(target,pname,&params);
        }
        else if(strncmp(line,"glTexSubImage1D(",16) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLsizei width; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexSubImage1D(%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&width,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
          glTexSubImage1D(target,level,xoffset,width,format,type,pixels);
          free(pixels);
        }
        else if(strncmp(line,"glTexSubImage2D(",16) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLint yoffset; GLsizei width; GLsizei height; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexSubImage2D(%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&yoffset,&width,&height,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
          glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
          free(pixels);
        }
        else if(strncmp(line,"glTexSubImage3D(",16) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLint yoffset; GLint zoffset; GLsizei width; GLsizei height; GLsizei depth; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexSubImage3D(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&yoffset,&zoffset,&width,&height,&depth,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
#ifndef PVWIN32
          glTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
#endif
          free(pixels);
        }

        else if(strncmp(line,"glTexSubImage3DEXT(",19) == 0)
        {
          GLenum target; GLint level; GLint xoffset; GLint yoffset; GLint zoffset; GLsizei width; GLsizei height; GLsizei depth; GLenum format; GLenum type; GLvoid *pixels;
          sscanf(line,"glTexSubImage3DEXT(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",(MyGLenum *) &target,&level,&xoffset,&yoffset,&zoffset,&width,&height,&depth,(MyGLenum *) &format,(MyGLenum *) &type);
          pixels = glgetbytes();
#ifndef PVWIN32
          //glTexSubImage3DEXT(target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
#endif
          free(pixels);
        }
        else if(strncmp(line,"glTranslated(",13) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glTranslated(%f,%f,%f)",&x,&y,&z);
          glTranslated(x,y,z);
        }
        else if(strncmp(line,"glTranslatef(",13) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glTranslatef(%f,%f,%f)",&x,&y,&z);
          glTranslatef(x,y,z);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'U':
        if(strncmp(line,"glUnlockArraysEXT(",18) == 0)
        {
#ifndef PVWIN32
          //glUnlockArraysEXT();
#endif
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'V':
        if(strncmp(line,"glVertex2d(",11) == 0)
        {
          GLfloat x; GLfloat y;
          sscanf(line,"glVertex2d(%f,%f)",&x,&y);
          glVertex2d(x,y);
        }
        else if(strncmp(line,"glVertex2dv(",12) == 0)
        {
          GLfloat v[2];
          sscanf(line,"glVertex2dv(%f,%f)",&v[0],&v[1]);
          glVertex2fv(v);
        }
        else if(strncmp(line,"glVertex2f(",11) == 0)
        {
          GLfloat x; GLfloat y;
          sscanf(line,"glVertex2f(%f,%f)",&x,&y);
          glVertex2f(x,y);
        }
        else if(strncmp(line,"glVertex2fv(",12) == 0)
        {
          GLfloat v[2];
          sscanf(line,"glVertex2fv(%f,%f)",&v[0],&v[1]);
          glVertex2fv(v);
        }
        else if(strncmp(line,"glVertex2i(",11) == 0)
        {
          GLint x; GLint y;
          sscanf(line,"glVertex2i(%d,%d)",&x,&y);
          glVertex2i(x,y);
        }
        else if(strncmp(line,"glVertex2iv(",12) == 0)
        {
          GLint v[2];
          sscanf(line,"glVertex2iv(%d,%d)",&v[0],&v[1]);
          glVertex2iv(v);
        }
        else if(strncmp(line,"glVertex2s(",11) == 0)
        {
          GLshort x; GLshort y;
          sscanf(line,"glVertex2s(%hd,%hd)",&x,&y);
          glVertex2s(x,y);
        }
        else if(strncmp(line,"glVertex2sv(",12) == 0)
        {
          GLshort v[2];
          sscanf(line,"glVertex2sv(%hd,%hd)",&v[0],&v[1]);
          glVertex2sv(v);
        }
        else if(strncmp(line,"glVertex3d(",11) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glVertex3d(%f,%f,%f)",&x,&y,&z);
          glVertex3d(x,y,z);
        }
        else if(strncmp(line,"glVertex3dv(",12) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glVertex3dv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glVertex3fv(v);
        }
        else if(strncmp(line,"glVertex3f(",11) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z;
          sscanf(line,"glVertex3f(%f,%f,%f)",&x,&y,&z);
          glVertex3f(x,y,z);
        }
        else if(strncmp(line,"glVertex3fv(",12) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glVertex3fv(%f,%f,%f)",&v[0],&v[1],&v[2]);
          glVertex3fv(v);
        }
        else if(strncmp(line,"glVertex3i(",11) == 0)
        {
          GLint x; GLint y; GLint z;
          sscanf(line,"glVertex3i(%d,%d,%d)",&x,&y,&z);
          glVertex3i(x,y,z);
        }
        else if(strncmp(line,"glVertex3iv(",12) == 0)
        {
          GLint v[4];
          sscanf(line,"glVertex3iv(%d,%d,%d)",&v[0],&v[1],&v[2]);
          glVertex3iv(v);
        }
        else if(strncmp(line,"glVertex3s(",11) == 0)
        {
          GLshort x; GLshort y; GLshort z;
          sscanf(line,"glVertex3s(%hd,%hd,%hd)",&x,&y,&z);
          glVertex3s(x,y,z);
        }
        else if(strncmp(line,"glVertex3sv(",12) == 0)
        {
          GLshort v[4];
          sscanf(line,"glVertex3sv(%hd,%hd,%hd)",&v[0],&v[1],&v[2]);
          glVertex3sv(v);
        }
        else if(strncmp(line,"glVertex4d(",11) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z; GLfloat w;
          sscanf(line,"glVertex4d(%f,%f,%f,%f)",&x,&y,&z,&w);
          glVertex4d(x,y,z,w);
        }
        else if(strncmp(line,"glVertex4dv(",12) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glVertex4dv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glVertex4fv(v);
        }
        else if(strncmp(line,"glVertex4f(",11) == 0)
        {
          GLfloat x; GLfloat y; GLfloat z; GLfloat w;
          sscanf(line,"glVertex4f(%f,%f,%f,%f)",&x,&y,&z,&w);
          glVertex4f(x,y,z,w);
        }
        else if(strncmp(line,"glVertex4fv(",12) == 0)
        {
          GLfloat v[4];
          sscanf(line,"glVertex4fv(%f,%f,%f,%f)",&v[0],&v[1],&v[2],&v[3]);
          glVertex4fv(v);
        }
        else if(strncmp(line,"glVertex4i(",11) == 0)
        {
          GLint x; GLint y; GLint z; GLint w;
          sscanf(line,"glVertex4i(%d,%d,%d,%d)",&x,&y,&z,&w);
          glVertex4i(x,y,z,w);
        }
        else if(strncmp(line,"glVertex4iv(",12) == 0)
        {
          GLint v[4];
          sscanf(line,"glVertex4iv(%d,%d,%d,%d)",&v[0],&v[1],&v[2],&v[3]);
          glVertex4iv(v);
        }
        else if(strncmp(line,"glVertex4s(",11) == 0)
        {
          GLshort x; GLshort y; GLshort z; GLshort w;
          sscanf(line,"glVertex4s(%hd,%hd,%hd,%hd)",&x,&y,&z,&w);
          glVertex4s(x,y,z,w);
        }
        else if(strncmp(line,"glVertex4sv(",12) == 0)
        {
          GLshort v[4];
          sscanf(line,"glVertex4sv(%hd,%hd,%hd,%hd)",&v[0],&v[1],&v[2],&v[3]);
          glVertex4sv(v);
        }
        else if(strncmp(line,"glVertexPointer(",16) == 0)
        {
          GLint size; GLenum type; GLsizei stride; GLvoid *ptr;
          sscanf(line,"glVertexPointer(%d,%d,%d)",&size,(MyGLenum *) &type,&stride);
          ptr = glgetbytes();
          glVertexPointer(size,type,stride,ptr);
          free(ptr);
        }
        else if(strncmp(line,"glVertexPointerEXT(",19) == 0)
        {
          GLint size; GLenum type; GLsizei stride; GLsizei count; GLvoid *ptr;
          sscanf(line,"glVertexPointerEXT(%d,%d,%d,%d)",&size,(MyGLenum *) &type,&stride,&count);
          ptr = glgetbytes();
#ifndef PVWIN32
          //glVertexPointerEXT(size,type,stride,count,ptr);
#endif
          free(ptr);
        }
        else if(strncmp(line,"glViewport(",11) == 0)
        {
          GLint x; GLint y; GLsizei width; GLsizei height;
          sscanf(line,"glViewport(%d,%d,%d,%d)",&x,&y,&width,&height);

          glViewport(x,y,width,height);
        }
        else
        {
          printf("unkown: %s",line);
        }
        break;
    case 'W':
        /* MESA routines not implented jet
        GLAPI void GLAPIENTRY glWindowPos2dMESA( GLdouble x, GLdouble y )
        {
          sscanf(line,"glWindowPos2dMESA( GLdouble x, GLdouble y )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos2dvMESA( const GLdouble *p )
        {
          sscanf(line,"glWindowPos2dvMESA( const GLdouble *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos2fMESA( GLfloat x, GLfloat y )
        {
          sscanf(line,"glWindowPos2fMESA( GLfloat x, GLfloat y )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos2fvMESA( const GLfloat *p )
        {
          sscanf(line,"glWindowPos2fvMESA( const GLfloat *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos2iMESA( GLint x, GLint y )
        {
          sscanf(line,"glWindowPos2iMESA( GLint x, GLint y )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos2ivMESA( const GLint *p )
        {
          sscanf(line,"glWindowPos2ivMESA( const GLint *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos2sMESA( GLshort x, GLshort y )
        {
          sscanf(line,"glWindowPos2sMESA( GLshort x, GLshort y )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos2svMESA( const GLshort *p )
        {
          sscanf(line,"glWindowPos2svMESA( const GLshort *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z )
        {
          sscanf(line,"glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3dvMESA( const GLdouble *p )
        {
          sscanf(line,"glWindowPos3dvMESA( const GLdouble *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z )
        {
          sscanf(line,"glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3fvMESA( const GLfloat *p )
        {
          sscanf(line,"glWindowPos3fvMESA( const GLfloat *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3iMESA( GLint x, GLint y, GLint z )
        {
          sscanf(line,"glWindowPos3iMESA( GLint x, GLint y, GLint z )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3ivMESA( const GLint *p )
        {
          sscanf(line,"glWindowPos3ivMESA( const GLint *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3sMESA( GLshort x, GLshort y, GLshort z )
        {
          sscanf(line,"glWindowPos3sMESA( GLshort x, GLshort y, GLshort z )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos3svMESA( const GLshort *p )
        {
          sscanf(line,"glWindowPos3svMESA( const GLshort *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w)
        {
          sscanf(line,"glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w)");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4dvMESA( const GLdouble *p )
        {
          sscanf(line,"glWindowPos4dvMESA( const GLdouble *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
        {
          sscanf(line,"glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4fvMESA( const GLfloat *p )
        {
          sscanf(line,"glWindowPos4fvMESA( const GLfloat *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w )
        {
          sscanf(line,"glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4ivMESA( const GLint *p )
        {
          sscanf(line,"glWindowPos4ivMESA( const GLint *p )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w )
        {
          sscanf(line,"glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w )");
        }
        
        GLAPI void GLAPIENTRY glWindowPos4svMESA( const GLshort *p )
        {
          sscanf(line,"glWindowPos4svMESA( const GLshort *p )");
        }
        */
        break;
  } // end switch(line[2])

  return 0;
}

#endif // USE_OPEN_GL
