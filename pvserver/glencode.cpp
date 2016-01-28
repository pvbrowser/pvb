/***************************************************************************
                          glencode.cpp  -  description
                             -------------------
    begin                : Sat Dec 9 2000
    copyright            : (C) 2000 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
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
OpenGL encoder for use with Process View Browser    (C) R. Lehrig 2000
                                                    lehrig@t-online.de
*****************************************************************************/
#ifndef _GL_ENCODER_
#define _GL_ENCODER_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "processviewserver.h"
#include "pvbImage.h"
#ifdef PVWIN32
#include <io.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#define PI 3.1415926535897932384626433832795

#ifndef USE_INETD
pthread_mutex_t param_mutex;
#endif
static PARAM *param = NULL;

/* helper functions */
int glencode_set_param(PARAM *p)
{
  param = p;
  return 0;
}

static void glprintf(const char *format,...)
{
  char text[1024];
  va_list ap;

  va_start(ap,format);
#ifdef PVWIN32
  _vsnprintf(text, sizeof(text)-1, format, ap);
#endif
#ifdef __VMS
  vsprintf(text, format, ap);
#endif
#ifdef PVUNIX
  vsnprintf(text, sizeof(text)-1, format, ap);
#endif
  va_end(ap);

  pvtcpsend(param,text,strlen(text));
}

static void glbytes(int len, const void *ptr)
{
  glprintf("glbytes(%d)\n",len);
  pvtcpsend_binary(param,(const char *) ptr,len);
}

static long gllongresponse()
{
  long ret;
  char buf[80];

  while(1)
  {
    pvtcpreceive(param,buf,sizeof(buf)-1);
    if(strncmp(buf,"gllongresponse(",15) == 0)
    {
      sscanf(buf,"gllongresponse(%ld",&ret);
      return ret;
    }
  }
}

static void *glputbytes()
{
  void *ptr;
  int  len;
  char buf[80];

  while(1)
  {
    pvtcpreceive(param,buf,sizeof(buf)-1);
    if(strncmp(buf,"glputbytes(",11) == 0)
    {
      sscanf(buf,"glputbytes(%d",&len);
      if(len <= 0) return NULL;
      ptr = malloc(len);
      pvtcpreceive_binary(param, (char *) ptr, len);
      return ptr;
    }
  }
}

int pvGlBegin(PARAM *p, int id)
{
#ifndef USE_INETD
  pvlock(p);
#endif
  param = p;
  glprintf("pvGlBegin(%d)\n",id);
  return 0;
}

int pvGlEnd(PARAM *p)
{
  glprintf("pvGlEnd()\n");
  pvWait(p,"pvGlEnd");
#ifndef USE_INETD
  pvunlock(p);
#endif
  return 0;
}

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

GLAPI void GLAPIENTRY glAccum( GLenum op, GLfloat value )
{
  glprintf("glAccum(%d,%f);\n",op,value);
}

GLAPI void GLAPIENTRY glActiveTextureARB(GLenum texture)
{
  glprintf("glActiveTextureARB(%d);\n",texture);
}

GLAPI void GLAPIENTRY glAlphaFunc( GLenum func, GLclampf ref )
{
  glprintf("glAlphaFunc(%d,%f);\n",func,ref);
}

GLAPI void GLAPIENTRY glArrayElement( GLint i )
{
  glprintf("glArrayElement(%d);\n",i);
}

GLAPI void GLAPIENTRY glArrayElementEXT( GLint i )
{
  glprintf("glArrayElementEXT(%d);\n",i);
}

GLAPI void GLAPIENTRY glBegin( GLenum mode )
{
  glprintf("glBegin(%d);\n",mode);
}

GLAPI void GLAPIENTRY glBindTexture( GLenum target, GLuint texture )
{
  glprintf("glBindTexture(%d,%d);\n",target,texture);
}

GLAPI void GLAPIENTRY glBindTextureEXT( GLenum target, GLuint texture )
{
  glprintf("glBindTextureEXT(%d,%d);\n",target,texture);
}

GLAPI void GLAPIENTRY glBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove,
                                const GLubyte *bitmap )
{
  glprintf("glBitmap(%d,%d,%f,%f,%f,%f);\n",width,height,xorig,yorig,xmove,ymove);
  glbytes(width*height,bitmap);
}

GLAPI void GLAPIENTRY glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
  glprintf("glBlendColor(%f,%f,%f,%f);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glBlendColorEXT( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
  glprintf("glBlendColorEXT(%f,%f,%f,%f);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glBlendEquation( GLenum mode )
{
  glprintf("glBlendEquation(%d);\n",mode);
}

GLAPI void GLAPIENTRY glBlendEquationEXT( GLenum mode )
{
  glprintf("glBlendEquationEXT(%d);\n",mode);
}

GLAPI void GLAPIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor )
{
  glprintf("glBlendFunc(%d,%d);\n",sfactor,dfactor);
}

GLAPI void GLAPIENTRY glBlendFuncSeparateINGR( GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha )
{
  glprintf("glBlendFuncSeparateINGR(%d,%d,%d,%d);\n",sfactorRGB,dfactorRGB,sfactorAlpha,dfactorAlpha);
}

GLAPI void GLAPIENTRY glCallList( GLuint list )
{
  glprintf("glCallList(%d);\n",list);
}

GLAPI void GLAPIENTRY glCallLists( GLsizei n, GLenum type, const GLvoid *lists )
{
  glprintf("glCallLists(%d,%d);\n",n,type);
  glbytes(n*8,lists);
}

GLAPI void GLAPIENTRY glClear( GLbitfield mask )
{
  glprintf("glClear(%d);\n",mask);
}

GLAPI void GLAPIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
  glprintf("glClearAccum(%f,%f,%f,%f);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
  glprintf("glClearColor(%f,%f,%f,%f);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glClearDepth( GLclampd depth )

{
  glprintf("glClearDepth(%f);\n",depth);
}

GLAPI void GLAPIENTRY glClearIndex( GLfloat c )
{
  glprintf("glClearIndex(%f);\n",c);
}

GLAPI void GLAPIENTRY glClearStencil( GLint s )
{
  glprintf("glClearStencil(%d);\n",s);
}

GLAPI void GLAPIENTRY glClientActiveTextureARB(GLenum texture)
{
  glprintf("glClientActiveTextureARB(%d);\n",texture);
}

GLAPI void GLAPIENTRY glClipPlane( GLenum plane, const GLdouble *equation )
{
  glprintf("glClipPlane(%d,%f,%f,%f,%f);\n",plane,(float)equation[0],(float)equation[1],(float)equation[2],(float)equation[3]);
}

GLAPI void GLAPIENTRY glColor3b( GLbyte red, GLbyte green, GLbyte blue )
{
  glprintf("glColor3b(%d,%d,%d);\n",red,green,blue);
}

GLAPI void GLAPIENTRY glColor3bv( const GLbyte *v )
{
  glprintf("glColor3bv(%d,%d,%d);\n",(int)v[0],(int)v[1],(int)v[2]);
}



GLAPI void GLAPIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue )
{
  glprintf("glColor3d(%f,%f,%f);\n",(float)red,(float)green,(float)blue);
}


GLAPI void GLAPIENTRY glColor3dv( const GLdouble *v )
{
  glprintf("glColor3dv(%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2]);
}

GLAPI void GLAPIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
  glprintf("glColor3f(%f,%f,%f);\n",red,green,blue);
}

GLAPI void GLAPIENTRY glColor3fv( const GLfloat *v )
{
  glprintf("glColor3fv(%f,%f,%f);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glColor3i( GLint red, GLint green, GLint blue )
{
  glprintf("glColor3i(%d,%d,%d);\n",red,green,blue);
}



GLAPI void GLAPIENTRY glColor3iv( const GLint *v )
{
  glprintf("glColor3iv(%d,%d,%d);\n",v[0],v[1],v[2]);

}

GLAPI void GLAPIENTRY glColor3s( GLshort red, GLshort green, GLshort blue )
{
  glprintf("glColor3s(%d,%d,%d);\n",red,green,blue);
}

GLAPI void GLAPIENTRY glColor3sv( const GLshort *v )
{
  glprintf("glColor3sv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
  glprintf("glColor3ub(%d,%d,%d);\n",(int)red,(int)green,(int)blue);
}

GLAPI void GLAPIENTRY glColor3ubv( const GLubyte *v )
{
  glprintf("glColor3ubv(%d,%d,%d);\n",(int)v[0],(int)v[1],(int)v[2]);
}

GLAPI void GLAPIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue )
{
  glprintf("glColor3ui(%d,%d,%d);\n",red,green,blue);
}

GLAPI void GLAPIENTRY glColor3uiv( const GLuint *v )
{
  glprintf("glColor3uiv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glColor3us( GLushort red, GLushort green, GLushort blue )
{
  glprintf("glColor3us(%d,%d,%d);\n",red,green,blue);
}

GLAPI void GLAPIENTRY glColor3usv( const GLushort *v )
{
  glprintf("glColor3usv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
  glprintf("glColor4b(%d,%d,%d,%d);\n",(int)red,(int)green,(int)blue,(int)alpha);
}

GLAPI void GLAPIENTRY glColor4bv( const GLbyte *v )
{
  glprintf("glColor4bv(%d,%d,%d,%d);\n",(int)v[0],(int)v[1],(int)v[2],(int)v[3]);
}

GLAPI void GLAPIENTRY glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
  glprintf("glColor4d(%f,%f,%f,%f);\n",(float)red,(float)green,(float)blue,(float)alpha);
}

GLAPI void GLAPIENTRY glColor4dv( const GLdouble *v )
{
  glprintf("glColor4dv(%f,%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2],(float)v[3]);
}

GLAPI void GLAPIENTRY glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
  glprintf("glColor4f(%f,%f,%f,%f);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glColor4fv( const GLfloat *v )
{
  glprintf("glColor4fv(%f,%f,%f,%f);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glColor4i( GLint red, GLint green, GLint blue, GLint alpha )
{
  glprintf("glColor4i(%d,%d,%d,%d);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glColor4iv( const GLint *v )
{
  glprintf("glColor4iv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
  glprintf("glColor4s(%d,%d,%d,%d);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glColor4sv( const GLshort *v )
{
  glprintf("glColor4sv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
  glprintf("glColor4ub(%d,%d,%d,%d);\n",(int)red,(int)green,(int)blue,(int)alpha);

}

GLAPI void GLAPIENTRY glColor4ubv( const GLubyte *v )
{
  glprintf("glColor4ubv(%d,%d,%d,%d);\n",(int)v[0],(int)v[1],(int)v[2],(int)v[3]);
}

GLAPI void GLAPIENTRY glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
  glprintf("glColor4ui(%d,%d,%d,%d);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glColor4uiv( const GLuint *v )
{
  glprintf("glColor4uiv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
  glprintf("glColor4us(%d,%d,%d,%d);\n",red,green,blue,alpha);
}

GLAPI void GLAPIENTRY glColor4usv( const GLushort *v )
{
  glprintf("glColor4usv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
  glprintf("glColorMask(%d,%d,%d,%d);\n",(int)red,(int)green,(int)blue,(int)alpha);
}

GLAPI void GLAPIENTRY glColorMaterial( GLenum face, GLenum mode )
{
  glprintf("glColorMaterial(%d,%d);\n",face,mode);
}

GLAPI void GLAPIENTRY glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr )
{
  glprintf("glColorPointer(%d,%d,%d);\n",size,type,stride);
  glbytes(size*(8+stride),ptr);
}

GLAPI void GLAPIENTRY glColorPointerEXT( GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr )
{
  glprintf("glColorPointerEXT(%d,%d,%d,%d);\n",size,type,stride,count);
  glbytes((size*(8+stride))*count,ptr);
}

GLAPI void GLAPIENTRY glColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data )
{
  glprintf("glColorSubTable(%d,%d,%d,%d,%d);\n",target,start,count,format,type);
  glbytes(count*8*4,data);
}

GLAPI void GLAPIENTRY glColorSubTableEXT( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data )
{
  glprintf("glColorSubTableEXT(%d,%d,%d,%d,%d);\n",target,start,count,format,type);
  glbytes(count*8*4,data);
}

GLAPI void GLAPIENTRY glColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table )
{
  glprintf("glColorTable(%d,%d,%d,%d,%d);\n",target,internalformat,width,format,type);
  glbytes(width*8*4,table);
}

GLAPI void GLAPIENTRY glColorTableEXT( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table )
{
  glprintf("glColorTableEXT(%d,%d,%d,%d,%d);\n",target,internalformat,width,format,type);
  glbytes(width*8*4,table);
}

GLAPI void GLAPIENTRY glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
  glprintf("glColorTableParameterfv(%d,%d,%f,%f,%f,%f);\n",target,pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params)
{
  glprintf("glColorTableParameteriv(%d,%d,%d,%d,%d,%d);\n",target,pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image )
{
  glprintf("glConvolutionFilter1D(%d,%d,%d,%d,%d);\n",target,internalformat,width,format,type);

  glbytes(width,image);
}

GLAPI void GLAPIENTRY glConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image )
{
  glprintf("glConvolutionFilter2D(%d,%d,%d,%d,%d,%d);\n",target,internalformat,width,height,format,type);
  glbytes(width*height,image);
}

GLAPI void GLAPIENTRY glConvolutionParameterf( GLenum target, GLenum pname, GLfloat params )
{
  glprintf("glConvolutionParameterf(%d,%d,%f);\n",target,pname,params);

}

GLAPI void GLAPIENTRY glConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
  glprintf("glConvolutionParameterfv(%d,%d,%f);\n",target,pname,params[0]);
}

GLAPI void GLAPIENTRY glConvolutionParameteri( GLenum target, GLenum pname, GLint params )
{
  glprintf("glConvolutionParameteri(%d,%d,%d);\n",target,pname,params);
}

GLAPI void GLAPIENTRY glConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params )
{
  glprintf("glConvolutionParameteriv(%d,%d,%d);\n",target,pname,params[0]);
}

GLAPI void GLAPIENTRY glCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width )
{
  glprintf("glCopyColorSubTable(%d,%d,%d,%d,%d);\n",target,start,x,y,width);
}

GLAPI void GLAPIENTRY glCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width )
{
  glprintf("glCopyColorTable(%d,%d,%d,%d,%d);\n",target,internalformat,x,y,width);
}

GLAPI void GLAPIENTRY glCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width )
{
  glprintf("glCopyConvolutionFilter1D(%d,%d,%d,%d,%d);\n",target,internalformat,x,y,width);
}

GLAPI void GLAPIENTRY glCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
  glprintf("glCopyConvolutionFilter2D(%d,%d,%d,%d,%d,%d);\n",target,internalformat,x,y,width,height);
}

GLAPI void GLAPIENTRY glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
  glprintf("glCopyPixels(%d,%d,%d,%d,%d);\n",x,y,width,height,type);
}

GLAPI void GLAPIENTRY glCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border )
{
  glprintf("glCopyTexImage1D(%d,%d,%d,%d,%d,%d,%d);\n",target,level,internalformat,x,y,width,border);
}

GLAPI void GLAPIENTRY glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
  glprintf("glCopyTexImage2D(%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,internalformat,x,y,width,height,border);
}

GLAPI void GLAPIENTRY glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
  glprintf("glCopyTexSubImage1D(%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,x,y,width);
}

GLAPI void GLAPIENTRY glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
  glprintf("glCopyTexSubImage2D(%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,yoffset,x,y,width,height);
}

GLAPI void GLAPIENTRY glCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
  glprintf("glCopyTexSubImage3D(%d,%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,yoffset,zoffset,x,y,width,height);
}

GLAPI void GLAPIENTRY glCopyTexSubImage3DEXT( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
  glprintf("glCopyTexSubImage3DEXT(%d,%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,yoffset,zoffset,x,y,width,height);
}

GLAPI void GLAPIENTRY glCullFace( GLenum mode)
{
  glprintf("glCullFace(%d);\n",mode);
}

GLAPI void GLAPIENTRY glDeleteLists( GLuint list, GLsizei range )
{
  glprintf("glDeleteLists(%d,%d);\n",list,range);
}

GLAPI void GLAPIENTRY glDeleteTextures( GLsizei n, const GLuint *textures)
{
  glprintf("glDeleteTextures(%d);\n",n);
  glbytes(n*sizeof(GLuint),textures);
}

GLAPI void GLAPIENTRY glDeleteTexturesEXT( GLsizei n, const GLuint *textures)
{
  glprintf("glDeleteTexturesEXT(%d);\n",n);
  glbytes(n*sizeof(GLuint),textures);
}

GLAPI void GLAPIENTRY glDepthFunc( GLenum func )
{
  glprintf("glDepthFunc(%d);\n",func);
}

GLAPI void GLAPIENTRY glDepthMask( GLboolean flag )
{
  glprintf("glDepthMask(%d);\n",(int) flag);
}

GLAPI void GLAPIENTRY glDepthRange( GLclampd near_val, GLclampd far_val )
{
  glprintf("glDepthRange(%f,%f);\n",(float) near_val, (float)far_val);
}

GLAPI void GLAPIENTRY glDisable( GLenum cap )
{
  glprintf("glDisable(%d);\n",cap);
}

GLAPI void GLAPIENTRY glDisableClientState( GLenum cap )
{
  glprintf("glDisableClientState(%d);\n",cap);
}

GLAPI void GLAPIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count )
{
  glprintf("glDrawArrays(%d,%d,%d);\n",mode,first,count);
}

GLAPI void GLAPIENTRY glDrawArraysEXT( GLenum mode, GLint first, GLsizei count )
{
  glprintf("glDrawArraysEXT(%d,%d,%d);\n",mode,first,count);

}

GLAPI void GLAPIENTRY glDrawBuffer( GLenum mode )
{
  glprintf("glDrawBuffer(%d);\n",mode);
}

GLAPI void GLAPIENTRY glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
  glprintf("glDrawElements(%d,%d,%d);\n",mode,count,type);
  glbytes(count*8,indices);
}

GLAPI void GLAPIENTRY glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  glprintf("glDrawPixels(%d,%d,%d,%d);\n",width,height,format,type);
  glbytes(width*height*4,pixels);
}

GLAPI void GLAPIENTRY glDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices )
{
  glprintf("glDrawRangeElements(%d,%d,%d,%d,%d);\n",mode,start,end,count,type);
  glbytes(count*8,indices);
}

GLAPI void GLAPIENTRY glEdgeFlag( GLboolean flag )
{
  glprintf("glEdgeFlag(%d);\n",(int)flag);
}

GLAPI void GLAPIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid *ptr )
{
  int *ip;
  ip = (int *) ptr;
  glprintf("glEdgeFlagPointer(%d,%d);\n",stride,*ip);
}

GLAPI void GLAPIENTRY glEdgeFlagPointerEXT( GLsizei stride, GLsizei count, const GLboolean *ptr )
{
  glprintf("glEdgeFlagPointerEXT(%d,%d,%d);\n",stride,count,*ptr);
}

GLAPI void GLAPIENTRY glEdgeFlagv( const GLboolean *flag )
{
  glprintf("glEdgeFlagv(%d);\n",(int) *flag);
}

GLAPI void GLAPIENTRY glEnable( GLenum cap )
{
  glprintf("glEnable(%d);\n",cap);
}

GLAPI void GLAPIENTRY glEnableClientState( GLenum cap )
{
  glprintf("glEnableClientState(%d);\n",cap);
}

GLAPI void GLAPIENTRY glEnd( void )
{
  glprintf("glEnd();\n");
}

GLAPI void GLAPIENTRY glEndList( void )
{
  glprintf("glEndList();\n");
}

GLAPI void GLAPIENTRY glEvalCoord1d( GLdouble u )
{
  glprintf("glEvalCoord1d(%f);\n",(float)u);
}

GLAPI void GLAPIENTRY glEvalCoord1dv( const GLdouble *u )
{
  glprintf("glEvalCoord1dv(%f);\n",(float)*u);
}

GLAPI void GLAPIENTRY glEvalCoord1f( GLfloat u )
{
  glprintf("glEvalCoord1f(%f);\n",u);
}

GLAPI void GLAPIENTRY glEvalCoord1fv( const GLfloat *u )
{
  glprintf("glEvalCoord1fv(%f);\n",*u);
}

GLAPI void GLAPIENTRY glEvalCoord2d( GLdouble u, GLdouble v )
{
  glprintf("glEvalCoord2d(%f,%f);\n",(float)u,(float)v);
}

GLAPI void GLAPIENTRY glEvalCoord2dv( const GLdouble *u )
{
  glprintf("glEvalCoord2dv(%f,%f);\n",(float)u[0],(float)u[1]);
}

GLAPI void GLAPIENTRY glEvalCoord2f( GLfloat u, GLfloat v )
{
  glprintf("glEvalCoord2f(%f,%f);\n",u,v);
}

GLAPI void GLAPIENTRY glEvalCoord2fv( const GLfloat *u )
{
  glprintf("glEvalCoord2fv(%f,%f);\n",u[0],u[1]);
}

GLAPI void GLAPIENTRY glEvalMesh1( GLenum mode, GLint i1, GLint i2 )
{
  glprintf("glEvalMesh1(%d,%d,%d);\n",mode,i1,i2);
}

GLAPI void GLAPIENTRY glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
  glprintf("glEvalMesh2(%d,%d,%d,%d,%d);\n",mode,i1,i2,j1,j2);
}

GLAPI void GLAPIENTRY glEvalPoint1( GLint i )
{
  glprintf("glEvalPoint1(%d);\n",i);
}

GLAPI void GLAPIENTRY glEvalPoint2( GLint i, GLint j )
{

  glprintf("glEvalPoint2(%d,%d);\n",i,j);
}

GLAPI void GLAPIENTRY glFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer )
{
  glprintf("glFeedbackBuffer(%d,%d);\n",size,type);
  glbytes(size*sizeof(GLfloat),buffer);
}

GLAPI void GLAPIENTRY glFinish( void )
{
  glprintf("glFinish();\n");
}

GLAPI void GLAPIENTRY glFlush( void )
{
  glprintf("glFlush();\n");
}

GLAPI void GLAPIENTRY glFogf( GLenum pname, GLfloat param )
{
  glprintf("glFogf(%d,%f);\n",pname,param);
}


GLAPI void GLAPIENTRY glFogfv( GLenum pname, const GLfloat *params )
{
  glprintf("glFogfv(%d,%f);\n",pname,*params);
}

GLAPI void GLAPIENTRY glFogi( GLenum pname, GLint param )
{
  glprintf("glFogi(%d,%d);\n",pname,param);
}

GLAPI void GLAPIENTRY glFogiv( GLenum pname, const GLint *params )
{
  glprintf("glFogiv(%d,%d);\n",pname,*params);
}

GLAPI void GLAPIENTRY glFrontFace( GLenum mode )
{
  glprintf("glFrontFace(%d);\n",mode);
}

GLAPI void GLAPIENTRY glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val )
{
  glprintf("glFrustum(%f,%f,%f,%f,%f,%f);\n",(float)left,(float)right,(float)bottom,(float)top,(float)near_val,(float)far_val);
}

GLAPI void GLAPIENTRY glGenTextures( GLsizei n, GLuint *textures )
{
  glprintf("glGenTextures(%d);\n",n);
  //rl2008 glbytes(n*sizeof(GLuint),textures);
  void *ptr = glputbytes();
  if(ptr == NULL) return;
  memcpy(textures,ptr,n*sizeof(GLuint));
  free(ptr);
}

GLAPI void GLAPIENTRY glGenTexturesEXT( GLsizei n, GLuint *textures )
{
  glprintf("glGenTexturesEXT(%d);\n",n);
  //rl2008 glbytes(n*sizeof(GLuint),textures);
  void *ptr = glputbytes();
  if(ptr == NULL) return;
  memcpy(textures,ptr,n*sizeof(GLuint));
  free(ptr);
}

GLAPI GLuint GLAPIENTRY glGenLists( GLsizei range )
{
  glprintf("glGenLists(%d);\n",range);
  return (GLuint) gllongresponse();
}

/* get routines not implemented jet
GLAPI void GLAPIENTRY glGetBooleanv( GLenum pname, GLboolean *params )
{
  glprintf("glGetBooleanv( GLenum pname, GLboolean *params );\n");
}

GLAPI void GLAPIENTRY glGetClipPlane( GLenum plane, GLdouble *equation )
{
  glprintf("glGetClipPlane( GLenum plane, GLdouble *equation );\n");
}


GLAPI void GLAPIENTRY glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table )
{
  glprintf("glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table );\n");
}

GLAPI void GLAPIENTRY glGetColorTableEXT( GLenum target, GLenum format, GLenum type, GLvoid *table )
{
  glprintf("glGetColorTableEXT( GLenum target, GLenum format, GLenum type, GLvoid *table );\n");
}

GLAPI void GLAPIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  glprintf("glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetColorTableParameterfvEXT( GLenum target, GLenum pname, GLfloat *params )
{
  glprintf("glGetColorTableParameterfvEXT( GLenum target, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params )
{
  glprintf("glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetColorTableParameterivEXT( GLenum target, GLenum pname, GLint *params )
{
  glprintf("glGetColorTableParameterivEXT( GLenum target, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image )
{
  glprintf("glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image );\n");
}


GLAPI void GLAPIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  glprintf("glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params )
{
  glprintf("glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetDoublev( GLenum pname, GLdouble *params )
{
  glprintf("glGetDoublev( GLenum pname, GLdouble *params );\n");
}

GLAPI void GLAPIENTRY glGetFloatv( GLenum pname, GLfloat *params )
{
  glprintf("glGetFloatv( GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values )
{
  glprintf("glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values );\n");
}

GLAPI void GLAPIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  glprintf("glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params )
{
  glprintf("glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetIntegerv( GLenum pname, GLint *params )
{
  glprintf("glGetIntegerv( GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params )
{
  glprintf("glGetLightfv( GLenum light, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params )
{
  glprintf("glGetLightiv( GLenum light, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble *v )
{
  glprintf("glGetMapdv( GLenum target, GLenum query, GLdouble *v );\n");
}

GLAPI void GLAPIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v )
{
  glprintf("glGetMapfv( GLenum target, GLenum query, GLfloat *v );\n");
}

GLAPI void GLAPIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v )
{
  glprintf("glGetMapiv( GLenum target, GLenum query, GLint *v );\n");
}

GLAPI void GLAPIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )
{
  glprintf("glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params )
{
  glprintf("glGetMaterialiv( GLenum face, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values )
{
  glprintf("glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values );\n");
}

GLAPI void GLAPIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  glprintf("glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params )
{
  glprintf("glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetPixelMapfv( GLenum map, GLfloat *values )
{
  glprintf("glGetPixelMapfv( GLenum map, GLfloat *values );\n");
}

GLAPI void GLAPIENTRY glGetPixelMapuiv( GLenum map, GLuint *values )
{
  glprintf("glGetPixelMapuiv( GLenum map, GLuint *values );\n");
}

GLAPI void GLAPIENTRY glGetPixelMapusv( GLenum map, GLushort *values )
{
  glprintf("glGetPixelMapusv( GLenum map, GLushort *values );\n");
}

GLAPI void GLAPIENTRY glGetPointerv( GLenum pname, void **params )
{
  glprintf("glGetPointerv( GLenum pname, void **params );\n");
}

GLAPI void GLAPIENTRY glGetPointervEXT( GLenum pname, void **params )
{
  glprintf("glGetPointervEXT( GLenum pname, void **params );\n");
}

GLAPI void GLAPIENTRY glGetPolygonStipple( GLubyte *mask )
{
  glprintf("glGetPolygonStipple( GLubyte *mask );\n");
}

GLAPI void GLAPIENTRY glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span )
{
  glprintf("glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span );\n");
}

GLAPI void GLAPIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params )
{
  glprintf("glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params )
{
  glprintf("glGetTexEnviv( GLenum target, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params )
{
  glprintf("glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params );\n");
}

GLAPI void GLAPIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params )
{
  glprintf("glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params )
{
  glprintf("glGetTexGeniv( GLenum coord, GLenum pname, GLint *params );\n");
}

GLAPI void GLAPIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )
{
  glprintf("glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels );\n");
}

GLAPI void GLAPIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params )
{
  glprintf("glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );\n");
}

GLAPI void GLAPIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params )
{

  glprintf("glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );\n");
}


GLAPI void GLAPIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params)
{
  glprintf("glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params);\n");
}

GLAPI void GLAPIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params )
{
  glprintf("glGetTexParameteriv( GLenum target, GLenum pname, GLint *params );\n");
}
*/

GLAPI void GLAPIENTRY glHint( GLenum target, GLenum mode )
{
  glprintf("glHint(%d,%d);\n",target,mode);
}

GLAPI void GLAPIENTRY glHintPGI(GLenum target, GLint mode)
{
  glprintf("glHintPGI(%d,%d);\n",target,mode);
}

GLAPI void GLAPIENTRY glHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink )
{
  glprintf("glHistogram(%d,%d,%d,%d);\n",target,width,internalformat,(int) sink);
}

GLAPI void GLAPIENTRY glIndexMask( GLuint mask )
{
  glprintf("glIndexMask(%d);\n",mask);
}

GLAPI void GLAPIENTRY glIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr )
{
  glprintf("glIndexPointer(%d,%d);\n",type,stride);
  glbytes(8,ptr);
}

GLAPI void GLAPIENTRY glIndexPointerEXT( GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr )
{
  glprintf("glIndexPointerEXT(%d,%d,%d);\n",type,stride,count);
  glbytes(count*8,ptr);
}

GLAPI void GLAPIENTRY glIndexd( GLdouble c )

{
  glprintf("glIndexd(%f);\n",(float)c);
}

GLAPI void GLAPIENTRY glIndexdv( const GLdouble *c )
{
  glprintf("glIndexdv(%f);\n",(float)*c);
}

GLAPI void GLAPIENTRY glIndexf( GLfloat c )
{
  glprintf("glIndexf(%f);\n",c);
}

GLAPI void GLAPIENTRY glIndexfv( const GLfloat *c )
{
  glprintf("glIndexfv(%f);\n",*c);
}

GLAPI void GLAPIENTRY glIndexi( GLint c )
{
  glprintf("glIndexi(%d);\n",c);
}

GLAPI void GLAPIENTRY glIndexiv( const GLint *c )
{
  glprintf("glIndexiv(%d);\n",*c);
}

GLAPI void GLAPIENTRY glIndexs( GLshort c )
{
  glprintf("glIndexs(%d);\n",c);
}

GLAPI void GLAPIENTRY glIndexsv( const GLshort *c )
{
  glprintf("glIndexsv(%d);\n",*c);
}

GLAPI void GLAPIENTRY glIndexub( GLubyte c )
{
  glprintf("glIndexub(%d);\n",(int)c);
}

GLAPI void GLAPIENTRY glIndexubv( const GLubyte *c )
{
  glprintf("glIndexubv(%d);\n",(int) *c);
}

GLAPI void GLAPIENTRY glInitNames( void )
{
  glprintf("glInitNames();\n");
}

GLAPI void GLAPIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer )
{
  glprintf("glInterleavedArrays(%d,%d);\n",format,stride);
  glbytes(16*sizeof(GLfloat),pointer);
}

GLAPI void GLAPIENTRY glLightModelf( GLenum pname, GLfloat param )
{
  glprintf("glLightModelf(%d,%f);\n",pname,param);
}

GLAPI void GLAPIENTRY glLightModelfv( GLenum pname, const GLfloat *params )
{
  glprintf("glLightModelfv(%d,%f,%f,%f,%f);\n",pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glLightModeli( GLenum pname, GLint param )
{
  glprintf("glLightModeli(%d,%d);\n",pname,param);
}

GLAPI void GLAPIENTRY glLightModeliv( GLenum pname, const GLint *params )
{
  glprintf("glLightModeliv(%d,%d,%d,%d,%d);\n",pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glLightf( GLenum light, GLenum pname, GLfloat param )
{
  glprintf("glLightf(%d,%d,%f);\n",light,pname,param);
}

GLAPI void GLAPIENTRY glLightfv( GLenum light, GLenum pname, const GLfloat *params )
{
  glprintf("glLightfv(%d,%d,%f,%f,%f,%f);\n",light,pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glLighti( GLenum light, GLenum pname, GLint param )
{
  glprintf("glLighti(%d,%d,%d);\n",light,pname,param);
}

GLAPI void GLAPIENTRY glLightiv( GLenum light, GLenum pname, const GLint *params )
{
  glprintf("glLightiv(%d,%d,%d,%d,%d,%d);\n",light,pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glLineStipple( GLint factor, GLushort pattern )
{
  glprintf("glLineStipple(%d,%d);\n",factor,pattern);
}

GLAPI void GLAPIENTRY glLineWidth( GLfloat width )
{
  glprintf("glLineWidth(%f);\n",width);
}

GLAPI void GLAPIENTRY glListBase( GLuint base )
{
  glprintf("glListBase(%d);\n",base);
}

GLAPI void GLAPIENTRY glLoadIdentity( void )
{
  glprintf("glLoadIdentity();\n");
}

GLAPI void GLAPIENTRY glLoadMatrixd( const GLdouble *m )
{
  glprintf("glLoadMatrixd();\n");
  glbytes(16*sizeof(GLdouble),m);
}

GLAPI void GLAPIENTRY glLoadMatrixf( const GLfloat *m )
{
  glprintf("glLoadMatrixf();\n");
  glbytes(16*sizeof(GLfloat),m);
}

GLAPI void GLAPIENTRY glLoadName( GLuint name )
{
  glprintf("glLoadName(%d);\n",name);
}

GLAPI void GLAPIENTRY glLockArraysEXT( GLint first, GLsizei count )
{
  glprintf("glLockArraysEXT(%d,%d);\n",first,count);
}

GLAPI void GLAPIENTRY glLogicOp( GLenum opcode )
{
  glprintf("glLogicOp(%d);\n",opcode);
}

GLAPI void GLAPIENTRY glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points )
{
  glprintf("glMap1d(%d,%f,%f,%d,%d);\n",target,(float)u1,(float)u2,stride,order);
  glbytes(4*sizeof(GLdouble),points);
}

GLAPI void GLAPIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points )
{
  glprintf("glMap1f(%d,%f,%f,%d,%d);\n",target,u1,u2,stride,order);
  glbytes(4*sizeof(GLfloat),points);
}

GLAPI void GLAPIENTRY glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points )
{
  glprintf("glMap2d(%d,%f,%f,%d,%d,%f,%f,%d,%d);\n",target,(float)u1,(float)u2,ustride,uorder,(float)v1,(float)v2,vstride,vorder);
  glbytes(ustride*vstride*sizeof(GLdouble),points);
}

GLAPI void GLAPIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points )
{
  glprintf("glMap2f(%d,%f,%f,%d,%d,%f,%f,%d,%d);\n",target,u1,u2,ustride,uorder,v1,v2,vstride,vorder);
  glbytes(ustride*vstride*sizeof(GLfloat),points);
}

GLAPI void GLAPIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 )
{
  glprintf("glMapGrid1d(%d,%f,%f);\n",un,(float)u1,(float)u2);
}

GLAPI void GLAPIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 )
{
  glprintf("glMapGrid1f(%d,%f,%f);\n",un,u1,u2);
}

GLAPI void GLAPIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 )
{
  glprintf("glMapGrid2d(%d,%f,%f,%d,%f,%f);\n",un,(float)u1,(float)u2,vn,(float)v1,(float)v2);
}

GLAPI void GLAPIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 )
{
  glprintf("glMapGrid2f(%d,%f,%f,%d,%f,%f);\n",un,u1,u2,vn,v1,v2);
}

GLAPI void GLAPIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param )
{
  glprintf("glMaterialf(%d,%d,%f);\n",face,pname,param);
}

GLAPI void GLAPIENTRY glMaterialfv( GLenum face, GLenum pname, const GLfloat *params )
{
  glprintf("glMaterialfv(%d,%d,%f,%f,%f,%f);\n",face,pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glMateriali( GLenum face, GLenum pname, GLint param )
{
  glprintf("glMateriali(%d,%d,%d);\n",face,pname,param);
}

GLAPI void GLAPIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint *params )
{
  glprintf("glMaterialiv(%d,%d,%d,%d,%d,%d);\n",face,pname,params[0],params[1],params[2],params[3]);
}

GLAPI void GLAPIENTRY glMatrixMode( GLenum mode )
{
  glprintf("glMatrixMode(%d);\n",mode);
}


GLAPI void GLAPIENTRY glMinmax( GLenum target, GLenum internalformat, GLboolean sink )
{
  glprintf("glMinmax(%d,%d,%d);\n",target,internalformat,(int) sink);
}

GLAPI void GLAPIENTRY glMultMatrixd( const GLdouble *m )
{
  glprintf("glMultMatrixd();\n");
  glbytes(16*sizeof(GLdouble),m);
}

GLAPI void GLAPIENTRY glMultMatrixf( const GLfloat *m )
{
  glprintf("glMultMatrixf();\n");
  glbytes(16*sizeof(GLfloat),m);
}


GLAPI void GLAPIENTRY glMultiTexCoord1dARB(GLenum target, GLdouble s)
{
  glprintf("glMultiTexCoord1dARB(%d,%f);\n",target,(float)s);
}

GLAPI void GLAPIENTRY glMultiTexCoord1dvARB(GLenum target, const GLdouble *v)
{
  glprintf("glMultiTexCoord1dvARB(%d,%f);\n",target,(float)*v);
}

GLAPI void GLAPIENTRY glMultiTexCoord1fARB(GLenum target, GLfloat s)
{
  glprintf("glMultiTexCoord1fARB(%d,%f);\n",target,s);
}

GLAPI void GLAPIENTRY glMultiTexCoord1fvARB(GLenum target, const GLfloat *v)
{
  glprintf("glMultiTexCoord1fvARB(%d,%f);\n",target,*v);
}

GLAPI void GLAPIENTRY glMultiTexCoord1iARB(GLenum target, GLint s)
{
  glprintf("glMultiTexCoord1iARB(%d,%d);\n",target,s);
}

GLAPI void GLAPIENTRY glMultiTexCoord1ivARB(GLenum target, const GLint *v)
{
  glprintf("glMultiTexCoord1ivARB(%d,%d);\n",target,*v);
}

GLAPI void GLAPIENTRY glMultiTexCoord1sARB(GLenum target, GLshort s)
{
  glprintf("glMultiTexCoord1sARB(%d,%d);\n",target,s);
}

GLAPI void GLAPIENTRY glMultiTexCoord1svARB(GLenum target, const GLshort *v)
{
  glprintf("glMultiTexCoord1svARB(%d,%d);\n",target,*v);
}

GLAPI void GLAPIENTRY glMultiTexCoord2dARB(GLenum target, GLdouble s, GLdouble t)
{
  glprintf("glMultiTexCoord2dARB(%d,%f,%f);\n",target,(float)s,(float)t);
}

GLAPI void GLAPIENTRY glMultiTexCoord2dvARB(GLenum target, const GLdouble *v)
{
  glprintf("glMultiTexCoord2dvARB(%d,%f,%f);\n",target,(float)v[0],(float)v[1]);
}

GLAPI void GLAPIENTRY glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t)
{
  glprintf("glMultiTexCoord2fARB(%d,%f,%f);\n",target,s,t);
}


GLAPI void GLAPIENTRY glMultiTexCoord2fvARB(GLenum target, const GLfloat *v)
{
  glprintf("glMultiTexCoord2fvARB(%d,%f,%f);\n",target,v[0],v[1]);
}

GLAPI void GLAPIENTRY glMultiTexCoord2iARB(GLenum target, GLint s, GLint t)
{
  glprintf("glMultiTexCoord2iARB(%d,%d,%d);\n",target,s,t);
}

GLAPI void GLAPIENTRY glMultiTexCoord2ivARB(GLenum target, const GLint *v)
{
  glprintf("glMultiTexCoord2ivARB(%d,%d,%d);\n",target,v[0],v[1]);
}

GLAPI void GLAPIENTRY glMultiTexCoord2sARB(GLenum target, GLshort s, GLshort t)
{
  glprintf("glMultiTexCoord2sARB(%d,%d,%d);\n",target,s,t);
}

GLAPI void GLAPIENTRY glMultiTexCoord2svARB(GLenum target, const GLshort *v)
{
  glprintf("glMultiTexCoord2svARB(%d,%d,%d);\n",target,v[0],v[1]);
}

GLAPI void GLAPIENTRY glMultiTexCoord3dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
  glprintf("glMultiTexCoord3dARB(%d,%f,%f,%f);\n",target,(float)s,(float)t,(float)r);
}

GLAPI void GLAPIENTRY glMultiTexCoord3dvARB(GLenum target, const GLdouble *v)
{
  glprintf("glMultiTexCoord3dvARB(%d,%f,%f,%f);\n",target,(float)v[0],(float)v[1],(float)v[2]);
}


GLAPI void GLAPIENTRY glMultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
  glprintf("glMultiTexCoord3fARB(%d,%f,%f,%f);\n",target,s,t,r);
}

GLAPI void GLAPIENTRY glMultiTexCoord3fvARB(GLenum target, const GLfloat *v)
{
  glprintf("glMultiTexCoord3fvARB(%d,%f,%f,%f);\n",target,v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glMultiTexCoord3iARB(GLenum target, GLint s, GLint t, GLint r)
{
  glprintf("glMultiTexCoord3iARB(%d,%d,%d,%d);\n",target,s,t,r);

}

GLAPI void GLAPIENTRY glMultiTexCoord3ivARB(GLenum target, const GLint *v)
{
  glprintf("glMultiTexCoord3ivARB(%d,%d,%d,%d);\n",target,v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glMultiTexCoord3sARB(GLenum target, GLshort s, GLshort t, GLshort r)
{
  glprintf("glMultiTexCoord3sARB(%d,%d,%d,%d);\n",target,s,t,r);
}

GLAPI void GLAPIENTRY glMultiTexCoord3svARB(GLenum target, const GLshort *v)
{
  glprintf("glMultiTexCoord3svARB(%d,%d,%d,%d);\n",target,v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glMultiTexCoord4dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
  glprintf("glMultiTexCoord4dARB(%d,%f,%f,%f,%f);\n",target,(float)s,(float)t,(float)r,(float)q);
}

GLAPI void GLAPIENTRY glMultiTexCoord4dvARB(GLenum target, const GLdouble *v)
{
  glprintf("glMultiTexCoord4dvARB(%d,%f,%f,%f,%f);\n",target,(float)v[0],(float)v[1],(float)v[2],(float)v[3]);
}

GLAPI void GLAPIENTRY glMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
  glprintf("glMultiTexCoord4fARB(%d,%f,%f,%f,%f);\n",target,s,t,r,q);
}

GLAPI void GLAPIENTRY glMultiTexCoord4fvARB(GLenum target, const GLfloat *v)
{
  glprintf("glMultiTexCoord4fvARB(%d,%f,%f,%f,%f);\n",target,v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glMultiTexCoord4iARB(GLenum target, GLint s, GLint t, GLint r, GLint q)
{
  glprintf("glMultiTexCoord4iARB(%d,%d,%d,%d,%d);\n",target,s,t,r,q);
}

GLAPI void GLAPIENTRY glMultiTexCoord4ivARB(GLenum target, const GLint *v)
{
  glprintf("glMultiTexCoord4ivARB(%d,%d,%d,%d,%d);\n",target,v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glMultiTexCoord4sARB(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
  glprintf("glMultiTexCoord4sARB(%d,%d,%d,%d,%d);\n",target,s,t,r,q);
}

GLAPI void GLAPIENTRY glMultiTexCoord4svARB(GLenum target, const GLshort *v)
{
  glprintf("glMultiTexCoord4svARB(%d,%d,%d,%d,%d);\n",target,v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glNewList( GLuint list, GLenum mode )
{
  glprintf("glNewList(%d,%d);\n",list,mode);
}

GLAPI void GLAPIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz )
{
  glprintf("glNormal3b(%d,%d,%d);\n",(int)nx,(int)ny,(int)nz);
}

GLAPI void GLAPIENTRY glNormal3bv( const GLbyte *v )
{
  glprintf("glNormal3bv(%d,%d,%d);\n",(int)v[0],(int)v[1],(int)v[2]);
}

GLAPI void GLAPIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz )
{
  glprintf("glNormal3d(%f,%f,%f);\n",(float)nx,(float)ny,(float)nz);
}

GLAPI void GLAPIENTRY glNormal3dv( const GLdouble *v )
{
  glprintf("glNormal3dv(%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2]);
}

GLAPI void GLAPIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )
{
  glprintf("glNormal3f(%f,%f,%f);\n",nx,ny,nz);
}

GLAPI void GLAPIENTRY glNormal3fv( const GLfloat *v )
{
  glprintf("glNormal3fv(%f,%f,%f);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glNormal3i( GLint nx, GLint ny, GLint nz )
{
  glprintf("glNormal3i(%d,%d,%d);\n",nx,ny,nz);
}

GLAPI void GLAPIENTRY glNormal3iv( const GLint *v )
{
  glprintf("glNormal3iv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz )
{
  glprintf("glNormal3s(%d,%d,%d);\n",nx,ny,nz);
}

GLAPI void GLAPIENTRY glNormal3sv( const GLshort *v )
{
  glprintf("glNormal3sv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr )
{
  glprintf("glNormalPointer(%d,%d);\n",type,stride);
  glbytes(4*sizeof(GLdouble),ptr);
}

GLAPI void GLAPIENTRY glNormalPointerEXT( GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr )
{
  glprintf("glNormalPointerEXT(%d,%d,%d);\n",type,stride,count);
  glbytes(count*4*sizeof(GLdouble),ptr);
}

GLAPI void GLAPIENTRY glOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val )
{
  glprintf("glOrtho(%f,%f,%f,%f,%f,%f);\n",(float)left,(float)right,(float)bottom,(float)top,(float)near_val,(float)far_val);
}

GLAPI void GLAPIENTRY glPassThrough( GLfloat token )
{
  glprintf("glPassThrough(%f);\n",token);
}


GLAPI void GLAPIENTRY glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values )
{
  glprintf("glPixelMapfv(%d,%d);\n",map,mapsize);
  glbytes(mapsize*sizeof(GLfloat),values);
}

GLAPI void GLAPIENTRY glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values )
{
  glprintf("glPixelMapuiv(%d,%d);\n",map,mapsize);
  glbytes(mapsize*sizeof(GLuint),values);
}

GLAPI void GLAPIENTRY glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values )
{
  glprintf("glPixelMapusv(%d,%d);\n",map,mapsize);
  glbytes(mapsize*sizeof(GLushort),values);
}

GLAPI void GLAPIENTRY glPixelStoref( GLenum pname, GLfloat param )
{
  glprintf("glPixelStoref(%d,%f);\n",pname,param);
}

GLAPI void GLAPIENTRY glPixelStorei( GLenum pname, GLint param )
{
  glprintf("glPixelStorei(%d,%d);\n",pname,param);
}

GLAPI void GLAPIENTRY glPixelTransferf( GLenum pname, GLfloat param )
{
  glprintf("glPixelTransferf(%d,%f);\n",pname,param);
}


GLAPI void GLAPIENTRY glPixelTransferi( GLenum pname, GLint param )
{
  glprintf("glPixelTransferi(%d,%d);\n",pname,param);
}

GLAPI void GLAPIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor )
{
  glprintf("glPixelZoom(%f,%f);\n",xfactor,yfactor);
}

GLAPI void GLAPIENTRY glPointParameterfEXT( GLenum pname, GLfloat param )
{
  glprintf("glPointParameterfEXT(%d,%f);\n",pname,param);
}

GLAPI void GLAPIENTRY glPointParameterfvEXT( GLenum pname, const GLfloat *params )
{
  glprintf("glPointParameterfvEXT(%d,%f);\n",pname,params[0]);
}

GLAPI void GLAPIENTRY glPointSize( GLfloat size )
{
  glprintf("glPointSize(%f);\n",size);
}

GLAPI void GLAPIENTRY glPolygonMode( GLenum face, GLenum mode )
{
  glprintf("glPolygonMode(%d,%d);\n",face,mode);
}

GLAPI void GLAPIENTRY glPolygonOffset( GLfloat factor, GLfloat units )
{
  glprintf("glPolygonOffset(%f,%f);\n",factor,units);
}

GLAPI void GLAPIENTRY glPolygonOffsetEXT( GLfloat factor, GLfloat bias )
{
  glprintf("glPolygonOffsetEXT(%f,%f);\n",factor,bias);
}

GLAPI void GLAPIENTRY glPolygonStipple( const GLubyte *mask )
{
  glprintf("glPolygonStipple();\n");
  glbytes(8*16*sizeof(char),mask);
}

GLAPI void GLAPIENTRY glPopAttrib( void )
{
  glprintf("glPopAttrib();\n");
}

GLAPI void GLAPIENTRY glPopClientAttrib( void )
{
  glprintf("glPopClientAttrib();\n");
}

GLAPI void GLAPIENTRY glPopMatrix( void )
{
  glprintf("glPopMatrix();\n");
}

GLAPI void GLAPIENTRY glPopName( void )
{
  glprintf("glPopName();\n");
}

GLAPI void GLAPIENTRY glPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities )
{
  glprintf("glPrioritizeTextures(%d);\n",n);
  glbytes(n*sizeof(GLuint),textures);
  glbytes(n*sizeof(GLclampf),priorities);
}

GLAPI void GLAPIENTRY glPrioritizeTexturesEXT( GLsizei n, const GLuint *textures, const GLclampf *priorities )
{
  glprintf("glPrioritizeTexturesEXT(%d);\n",n);
  glbytes(n*sizeof(GLuint),textures);
  glbytes(n*sizeof(GLclampf),priorities);
}

GLAPI void GLAPIENTRY glPushAttrib( GLbitfield mask )
{
  glprintf("glPushAttrib(%d);\n",mask);
}

GLAPI void GLAPIENTRY glPushClientAttrib( GLbitfield mask )
{
  glprintf("glPushClientAttrib(%d);\n",mask);
}

GLAPI void GLAPIENTRY glPushMatrix( void )
{
  glprintf("glPushMatrix();\n");
}

GLAPI void GLAPIENTRY glPushName( GLuint name )
{

  glprintf("glPushName(%d);\n",name);
}

GLAPI void GLAPIENTRY glRasterPos2d( GLdouble x, GLdouble y )
{
  glprintf("glRasterPos2d(%f,%f);\n",(float)x,(float)y);
}

GLAPI void GLAPIENTRY glRasterPos2dv( const GLdouble *v )
{
  glprintf("glRasterPos2dv(%f,%f);\n",(float)v[0],(float)v[1]);
}

GLAPI void GLAPIENTRY glRasterPos2f( GLfloat x, GLfloat y )
{
  glprintf("glRasterPos2f(%f,%f);\n",x,y);
}

GLAPI void GLAPIENTRY glRasterPos2fv( const GLfloat *v )
{
  glprintf("glRasterPos2fv(%f,%f);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glRasterPos2i( GLint x, GLint y )
{
  glprintf("glRasterPos2i(%d,%d);\n",x,y);
}

GLAPI void GLAPIENTRY glRasterPos2iv( const GLint *v )
{
  glprintf("glRasterPos2iv(%d,%d);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glRasterPos2s( GLshort x, GLshort y )
{
  glprintf("glRasterPos2s(%d,%d);\n",x,y);
}

GLAPI void GLAPIENTRY glRasterPos2sv( const GLshort *v )
{
  glprintf("glRasterPos2sv(%d,%d);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z )
{
  glprintf("glRasterPos3d(%f,%f,%f);\n",(float)x,(float)y,(float)z);
}

GLAPI void GLAPIENTRY glRasterPos3dv( const GLdouble *v )
{
  glprintf("glRasterPos3dv(%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[3]);
}

GLAPI void GLAPIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z )
{
  glprintf("glRasterPos3f(%f,%f,%f);\n",x,y,z);
}

GLAPI void GLAPIENTRY glRasterPos3fv( const GLfloat *v )
{
  glprintf("glRasterPos3fv(%f,%f,%f);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glRasterPos3i( GLint x, GLint y, GLint z )
{
  glprintf("glRasterPos3i(%d,%d,%d);\n",x,y,z);
}

GLAPI void GLAPIENTRY glRasterPos3iv( const GLint *v )
{
  glprintf("glRasterPos3iv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z )
{
  glprintf("glRasterPos3s(%d,%d,%d);\n",x,y,z);
}

GLAPI void GLAPIENTRY glRasterPos3sv( const GLshort *v )
{
  glprintf("glRasterPos3sv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  glprintf("glRasterPos4d(%f,%f,%f,%f);\n",(float)x,(float)y,(float)z,(float)w);
}

GLAPI void GLAPIENTRY glRasterPos4dv( const GLdouble *v )
{
  glprintf("glRasterPos4dv(%f,%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2],(float)v[3]);
}

GLAPI void GLAPIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  glprintf("glRasterPos4f(%f,%f,%f,%f);\n",x,y,z,w);
}

GLAPI void GLAPIENTRY glRasterPos4fv( const GLfloat *v )
{
  glprintf("glRasterPos4fv(%f,%f,%f,%f);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w )
{
  glprintf("glRasterPos4i(%d,%d,%d,%d);\n",x,y,z,w);
}

GLAPI void GLAPIENTRY glRasterPos4iv( const GLint *v )
{
  glprintf("glRasterPos4iv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
  glprintf("glRasterPos4s(%d,%d,%d,%d);\n",x,y,z,w);
}

GLAPI void GLAPIENTRY glRasterPos4sv( const GLshort *v )
{
  glprintf("glRasterPos4sv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glReadBuffer( GLenum mode )

{
  glprintf("glReadBuffer(%d);\n",mode);
}

/* not implemented jet
GLAPI void GLAPIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
  glprintf("glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );\n");
}
*/

GLAPI void GLAPIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )
{
  glprintf("glRectd(%f,%f,%f,%f);\n",(float)x1,(float)y1,(float)x2,(float)y2);
}

GLAPI void GLAPIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 )
{
  glprintf("glRectdv(%f,%f,%f,%f);\n",(float)v1[0],(float)v1[1],(float)v2[0],(float)v2[1]);
}

GLAPI void GLAPIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
  glprintf("glRectf(%f,%f,%f,%f);\n",x1,y1,x2,y2);
}

GLAPI void GLAPIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 )
{
  glprintf("glRectfv(%f,%f,%f,%f);\n",v1[0],v1[1],v2[0],v2[1]);
}

GLAPI void GLAPIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 )
{

  glprintf("glRecti(%d,%d,%d,%d);\n",x1,y1,x2,y2);
}

GLAPI void GLAPIENTRY glRectiv( const GLint *v1, const GLint *v2 )
{
  glprintf("glRectiv(%d,%d,%d,%d);\n",v1[0],v1[1],v2[0],v2[1]);
}

GLAPI void GLAPIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 )
{

  glprintf("glRects(%d,%d,%d,%d);\n",x1,y1,x2,y2);
}

GLAPI void GLAPIENTRY glRectsv( const GLshort *v1, const GLshort *v2 )
{
  glprintf("glRectsv(%d,%d,%d,%d);\n",v1[0],v1[1],v2[0],v2[1]);
}

GLAPI void GLAPIENTRY glResetHistogram( GLenum target )
{
  glprintf("glResetHistogram(%d);\n",target);
}

GLAPI void GLAPIENTRY glResetMinmax( GLenum target )
{
  glprintf("glResetMinmax(%d);\n",target);
}

GLAPI void GLAPIENTRY glResizeBuffersMESA( void )
{
  glprintf("glResizeBuffersMESA();\n");
}

GLAPI void GLAPIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
  glprintf("glRotated(%f,%f,%f,%f);\n",(float)angle,(float)x,(float)y,(float)z);
}

GLAPI void GLAPIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
  glprintf("glRotatef(%f,%f,%f,%f);\n",angle,x,y,z);
}

GLAPI void GLAPIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z )
{
  glprintf("glScaled(%f,%f,%f);\n",(float)x,(float)y,(float)z);
}

GLAPI void GLAPIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z )
{
  glprintf("glScalef(%f,%f,%f);\n",x,y,z);
}

GLAPI void GLAPIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height)
{
  glprintf("glScissor(%d,%d,%d,%d);\n",x,y,width,height);
}


/* not implemented jet
GLAPI void GLAPIENTRY glSelectBuffer( GLsizei size, GLuint *buffer )
{
  glprintf("glSelectBuffer( GLsizei size, GLuint *buffer );\n");
}
*/

GLAPI void GLAPIENTRY glSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column )
{
  glprintf("glSeparableFilter2D(%d,%d,%d,%d,%d,%d);\n",target,internalformat,width,height,format,type);
  glbytes(width*sizeof(char),row);

  glbytes(height*sizeof(char),column);
}

GLAPI void GLAPIENTRY glShadeModel( GLenum mode )
{
  glprintf("glShadeModel(%d);\n",mode);
}

GLAPI void GLAPIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask )
{
  glprintf("glStencilFunc(%d,%d,%d);\n",func,ref,mask);
}

GLAPI void GLAPIENTRY glStencilMask( GLuint mask )
{
  glprintf("glStencilMask(%d);\n",mask);
}

GLAPI void GLAPIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
  glprintf("glStencilOp(%d,%d,%d);\n",fail,zfail,zpass);
}

GLAPI void GLAPIENTRY glTexCoord1d( GLdouble s )
{
  glprintf("glTexCoord1d(%f);\n",(float)s);
}

GLAPI void GLAPIENTRY glTexCoord1dv( const GLdouble *v )
{
  glprintf("glTexCoord1dv(%f);\n",(float)v[0]);
}

GLAPI void GLAPIENTRY glTexCoord1f( GLfloat s )
{
  glprintf("glTexCoord1f(%f);\n",s);
}

GLAPI void GLAPIENTRY glTexCoord1fv( const GLfloat *v )
{
  glprintf("glTexCoord1fv(%f);\n",v[0]);
}

GLAPI void GLAPIENTRY glTexCoord1i( GLint s )
{

  glprintf("glTexCoord1i(%d);\n",s);
}

GLAPI void GLAPIENTRY glTexCoord1iv( const GLint *v )
{
  glprintf("glTexCoord1iv(%d);\n",v[0]);
}

GLAPI void GLAPIENTRY glTexCoord1s( GLshort s )
{
  glprintf("glTexCoord1s(%d);\n",s);
}

GLAPI void GLAPIENTRY glTexCoord1sv( const GLshort *v )
{
  glprintf("glTexCoord1sv(%d);\n",v[0]);
}

GLAPI void GLAPIENTRY glTexCoord2d( GLdouble s, GLdouble t )
{
  glprintf("glTexCoord2d(%f,%f);\n",(float)s,(float)t);
}

GLAPI void GLAPIENTRY glTexCoord2dv( const GLdouble *v )
{
  glprintf("glTexCoord2dv(%f,%f);\n",(float)v[0],(float)v[1]);
}

GLAPI void GLAPIENTRY glTexCoord2f( GLfloat s, GLfloat t )
{
  glprintf("glTexCoord2f(%f,%f);\n",s,t);
}

GLAPI void GLAPIENTRY glTexCoord2fv( const GLfloat *v )
{
  glprintf("glTexCoord2fv(%f,%f);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glTexCoord2i( GLint s, GLint t )
{
  glprintf("glTexCoord2i(%d,%d);\n",s,t);
}

GLAPI void GLAPIENTRY glTexCoord2iv( const GLint *v )
{
  glprintf("glTexCoord2iv(%d,%d);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glTexCoord2s( GLshort s, GLshort t )
{
  glprintf("glTexCoord2s(%d,%d);\n",s,t);
}

GLAPI void GLAPIENTRY glTexCoord2sv( const GLshort *v )
{
  glprintf("glTexCoord2sv(%d,%d);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r )
{
  glprintf("glTexCoord3d(%f,%f,%f);\n",(float)s,(float)t,(float)r);
}

GLAPI void GLAPIENTRY glTexCoord3dv( const GLdouble *v )
{
  glprintf("glTexCoord3dv(%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2]);
}

GLAPI void GLAPIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r )
{
  glprintf("glTexCoord3f(%f,%f,%f);\n",s,t,r);
}

GLAPI void GLAPIENTRY glTexCoord3fv( const GLfloat *v )
{
  glprintf("glTexCoord3fv(%f,%f,%f);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glTexCoord3i( GLint s, GLint t, GLint r )
{
  glprintf("glTexCoord3i(%d,%d,%d);\n",s,t,r);
}

GLAPI void GLAPIENTRY glTexCoord3iv( const GLint *v )
{
  glprintf("glTexCoord3iv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r )
{
  glprintf("glTexCoord3s(%d,%d,%d);\n",s,t,r);
}

GLAPI void GLAPIENTRY glTexCoord3sv( const GLshort *v )
{
  glprintf("glTexCoord3sv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
  glprintf("glTexCoord4d(%f,%f,%f,%f);\n",(float)s,(float)t,(float)r,(float)q);
}

GLAPI void GLAPIENTRY glTexCoord4dv( const GLdouble *v )
{
  glprintf("glTexCoord4dv(%f,%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2],(float)v[3]);
}

GLAPI void GLAPIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
  glprintf("glTexCoord4f(%f,%f,%f,%f);\n",s,t,r,q);
}

GLAPI void GLAPIENTRY glTexCoord4fv( const GLfloat *v )
{
  glprintf("glTexCoord4fv(%f,%f,%f,%f);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q )
{
  glprintf("glTexCoord4i(%d,%d,%d,%d);\n",s,t,r,q);
}

GLAPI void GLAPIENTRY glTexCoord4iv( const GLint *v )
{
  glprintf("glTexCoord4iv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q )
{
  glprintf("glTexCoord4s(%d,%d,%d,%d);\n",s,t,r,q);
}

GLAPI void GLAPIENTRY glTexCoord4sv( const GLshort *v )
{
  glprintf("glTexCoord4sv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr )
{
  glprintf("glTexCoordPointer(%d,%d,%d);\n",size,type,stride);
  glbytes(size*sizeof(GLdouble),ptr);
}

GLAPI void GLAPIENTRY glTexCoordPointerEXT( GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr )
{
  glprintf("glTexCoordPointerEXT(%d,%d,%d,%d);\n",size,type,stride,count);
  glbytes(count*size*sizeof(GLdouble),ptr);
}

GLAPI void GLAPIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
  glprintf("glTexEnvf(%d,%d,%f);\n",target,pname,param);
}

GLAPI void GLAPIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params )
{
  glprintf("glTexEnvfv(%d,%d,%f);\n",target,pname,params[0]);
}

GLAPI void GLAPIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param )
{
  glprintf("glTexEnvi(%d,%d,%d);\n",target,pname,param);
}

GLAPI void GLAPIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params )
{
  glprintf("glTexEnviv(%d,%d,%d);\n",target,pname,params[0]);
}

GLAPI void GLAPIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param )
{
  glprintf("glTexGend(%d,%d,%f);\n",coord,pname,(float)param);
}

GLAPI void GLAPIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params )
{
  glprintf("glTexGendv(%d,%d,%f);\n",coord,pname,(float)params[0]);
}

GLAPI void GLAPIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param )
{
  glprintf("glTexGenf(%d,%d,%f);\n",coord,pname,param);
}

GLAPI void GLAPIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params )
{
  glprintf("glTexGenfv(%d,%d,%f);\n",coord,pname,params[0]);
}

GLAPI void GLAPIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param )
{
  glprintf("glTexGeni(%d,%d,%d);\n",coord,pname,param);
}

GLAPI void GLAPIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params )
{
  glprintf("glTexGeniv(%d,%d,%d);\n",coord,pname,params[0]);
}

GLAPI void GLAPIENTRY glTexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  glprintf("glTexImage1D(%d,%d,%d,%d,%d,%d,%d);\n",target,level,internalFormat,width,border,format,type);
  glbytes(width,pixels);
}

GLAPI void GLAPIENTRY glTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  glprintf("glTexImage2D(%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,internalFormat,width,height,border,format,type);
  glbytes(width*height,pixels);
}

GLAPI void GLAPIENTRY glTexImage3D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  glprintf("glTexImage3D(%d,%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,internalFormat,width,height,depth,border,format,type);
  glbytes(width*height*depth,pixels);
}

GLAPI void GLAPIENTRY glTexImage3DEXT( GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  glprintf("glTexImage3DEXT(%d,%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,internalFormat,width,height,depth,border,format,type);
  glbytes(width*height*depth,pixels);
}

GLAPI void GLAPIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
  glprintf("glTexParameterf(%d,%d,%f);\n",target,pname,param);
}

GLAPI void GLAPIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
  glprintf("glTexParameterfv(%d,%d,%f);\n",target,pname,params[0]);
}


GLAPI void GLAPIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param )
{
  glprintf("glTexParameteri(%d,%d,%d);\n",target,pname,param);
}

GLAPI void GLAPIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params )
{
  glprintf("glTexParameteriv(%d,%d,%d);\n",target,pname,params[0]);

}

GLAPI void GLAPIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels )
{
  glprintf("glTexSubImage1D(%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,width,format,type);
  glbytes(width,pixels);
}

GLAPI void GLAPIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  glprintf("glTexSubImage2D(%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,yoffset,width,height,format,type);
  glbytes(width*height,pixels);
}

GLAPI void GLAPIENTRY glTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels)
{
  glprintf("glTexSubImage3D(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,yoffset,zoffset,width,height,depth,format,type);
  glbytes(width*height*depth,pixels);
}

GLAPI void GLAPIENTRY glTexSubImage3DEXT( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels)
{
  glprintf("glTexSubImage3DEXT(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d);\n",target,level,xoffset,yoffset,zoffset,width,height,depth,format,type);
  glbytes(width*height*depth,pixels);
}

GLAPI void GLAPIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z )
{
  glprintf("glTranslated(%f,%f,%f);\n",(float)x,(float)y,(float)z);
}

GLAPI void GLAPIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
  glprintf("glTranslatef(%f,%f,%f);\n",x,y,z);
}

GLAPI void GLAPIENTRY glUnlockArraysEXT( void )
{
  glprintf("glUnlockArraysEXT();\n");
}

GLAPI void GLAPIENTRY glVertex2d( GLdouble x, GLdouble y )
{
  glprintf("glVertex2d(%f,%f);\n",(float)x,(float)y);
}

GLAPI void GLAPIENTRY glVertex2dv( const GLdouble *v )
{
  glprintf("glVertex2dv(%f,%f);\n",(float)v[0],(float)v[1]);
}

GLAPI void GLAPIENTRY glVertex2f( GLfloat x, GLfloat y )
{
  glprintf("glVertex2f(%f,%f);\n",x,y);
}

GLAPI void GLAPIENTRY glVertex2fv( const GLfloat *v )
{
  glprintf("glVertex2fv(%f,%f);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glVertex2i( GLint x, GLint y )
{
  glprintf("glVertex2i(%d,%d);\n",x,y);
}

GLAPI void GLAPIENTRY glVertex2iv( const GLint *v )
{
  glprintf("glVertex2iv(%d,%d);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glVertex2s( GLshort x, GLshort y )
{
  glprintf("glVertex2s(%d,%d);\n",x,y);
}


GLAPI void GLAPIENTRY glVertex2sv( const GLshort *v )
{
  glprintf("glVertex2sv(%d,%d);\n",v[0],v[1]);
}

GLAPI void GLAPIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z )
{
  glprintf("glVertex3d(%f,%f,%f);\n",(float)x,(float)y,(float)z);
}

GLAPI void GLAPIENTRY glVertex3dv( const GLdouble *v )
{
  glprintf("glVertex3dv(%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2]);
}

GLAPI void GLAPIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
  glprintf("glVertex3f(%f,%f,%f);\n",x,y,z);
}

GLAPI void GLAPIENTRY glVertex3fv( const GLfloat *v )
{
  glprintf("glVertex3fv(%f,%f,%f);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glVertex3i( GLint x, GLint y, GLint z )
{
  glprintf("glVertex3i(%d,%d,%d);\n",x,y,z);
}

GLAPI void GLAPIENTRY glVertex3iv( const GLint *v )
{
  glprintf("glVertex3iv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glVertex3s( GLshort x, GLshort y, GLshort z )
{
  glprintf("glVertex3s(%d,%d,%d);\n",x,y,z);
}

GLAPI void GLAPIENTRY glVertex3sv( const GLshort *v )
{
  glprintf("glVertex3sv(%d,%d,%d);\n",v[0],v[1],v[2]);
}

GLAPI void GLAPIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  glprintf("glVertex4d(%f,%f,%f,%f);\n",(float)x,(float)y,(float)z,(float)w);
}

GLAPI void GLAPIENTRY glVertex4dv( const GLdouble *v )
{
  glprintf("glVertex4dv(%f,%f,%f,%f);\n",(float)v[0],(float)v[1],(float)v[2],(float)v[3]);
}

GLAPI void GLAPIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  glprintf("glVertex4f(%f,%f,%f,%f);\n",x,y,z,w);
}

GLAPI void GLAPIENTRY glVertex4fv( const GLfloat *v )
{
  glprintf("glVertex4fv(%f,%f,%f,%f);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w )
{
  glprintf("glVertex4i(%d,%d,%d,%d);\n",x,y,z,w);
}

GLAPI void GLAPIENTRY glVertex4iv( const GLint *v )
{
  glprintf("glVertex4iv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )

{
  glprintf("glVertex4s(%d,%d,%d,%d);\n",x,y,z,w);
}

GLAPI void GLAPIENTRY glVertex4sv( const GLshort *v )
{
  glprintf("glVertex4sv(%d,%d,%d,%d);\n",v[0],v[1],v[2],v[3]);
}

GLAPI void GLAPIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr )
{
  glprintf("glVertexPointer(%d,%d,%d);\n",size,type,stride);
  glbytes(size*sizeof(GLdouble)+stride,ptr);
}

GLAPI void GLAPIENTRY glVertexPointerEXT( GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *ptr )
{
  glprintf("glVertexPointerEXT(%d,%d,%d,%d);\n",size,type,stride,count);
  glbytes(count*(size*sizeof(GLdouble)+stride),ptr);
}

GLAPI void GLAPIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
  glprintf("glViewport(%d,%d,%d,%d);\n",x,y,width,height);
}

/* MESA routines not implented jet
GLAPI void GLAPIENTRY glWindowPos2dMESA( GLdouble x, GLdouble y )
{
  glprintf("glWindowPos2dMESA( GLdouble x, GLdouble y );\n");
}

GLAPI void GLAPIENTRY glWindowPos2dvMESA( const GLdouble *p )
{
  glprintf("glWindowPos2dvMESA( const GLdouble *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos2fMESA( GLfloat x, GLfloat y )
{
  glprintf("glWindowPos2fMESA( GLfloat x, GLfloat y );\n");
}


GLAPI void GLAPIENTRY glWindowPos2fvMESA( const GLfloat *p )
{
  glprintf("glWindowPos2fvMESA( const GLfloat *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos2iMESA( GLint x, GLint y )
{
  glprintf("glWindowPos2iMESA( GLint x, GLint y );\n");
}

GLAPI void GLAPIENTRY glWindowPos2ivMESA( const GLint *p )
{
  glprintf("glWindowPos2ivMESA( const GLint *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos2sMESA( GLshort x, GLshort y )

{
  glprintf("glWindowPos2sMESA( GLshort x, GLshort y );\n");
}


GLAPI void GLAPIENTRY glWindowPos2svMESA( const GLshort *p )
{
  glprintf("glWindowPos2svMESA( const GLshort *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z )
{
  glprintf("glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z );\n");
}

GLAPI void GLAPIENTRY glWindowPos3dvMESA( const GLdouble *p )
{

  glprintf("glWindowPos3dvMESA( const GLdouble *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z )

{
  glprintf("glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z );\n");
}

GLAPI void GLAPIENTRY glWindowPos3fvMESA( const GLfloat *p )
{
  glprintf("glWindowPos3fvMESA( const GLfloat *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos3iMESA( GLint x, GLint y, GLint z )
{
  glprintf("glWindowPos3iMESA( GLint x, GLint y, GLint z );\n");
}

GLAPI void GLAPIENTRY glWindowPos3ivMESA( const GLint *p )
{
  glprintf("glWindowPos3ivMESA( const GLint *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos3sMESA( GLshort x, GLshort y, GLshort z )
{
  glprintf("glWindowPos3sMESA( GLshort x, GLshort y, GLshort z );\n");
}

GLAPI void GLAPIENTRY glWindowPos3svMESA( const GLshort *p )
{
  glprintf("glWindowPos3svMESA( const GLshort *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
  glprintf("glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w);\n");
}

GLAPI void GLAPIENTRY glWindowPos4dvMESA( const GLdouble *p )
{
  glprintf("glWindowPos4dvMESA( const GLdouble *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  glprintf("glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w );\n");
}

GLAPI void GLAPIENTRY glWindowPos4fvMESA( const GLfloat *p )
{

  glprintf("glWindowPos4fvMESA( const GLfloat *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w )
{
  glprintf("glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w );\n");
}

GLAPI void GLAPIENTRY glWindowPos4ivMESA( const GLint *p )
{
  glprintf("glWindowPos4ivMESA( const GLint *p );\n");
}

GLAPI void GLAPIENTRY glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w )
{
  glprintf("glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w );\n");
}

GLAPI void GLAPIENTRY glWindowPos4svMESA( const GLshort *p )

{
  glprintf("glWindowPos4svMESA( const GLshort *p );\n");
}
*/

// This is how glyphs are stored in the file.
struct Glyph_Buffer
{
  unsigned char ascii, width;
  unsigned short x, y;
};

glFont::glFont()
{
  zoom  = 1.0;
  zoom0 = 1.0;
}

glFont::~glFont()
{
}

int glFont::read(const char *filename)
{
  char buf[80];
  int i;
  // Open the file and check whether it is any good (a font file
  // starts with "F0")
  int fhdl = pvopenBinary(filename);
  if(fhdl<=0)
  {
    close(fhdl);
    printf("glFont: could not read %s\n", filename);
    return -1;
  }
  pvmyread(fhdl, buf, 2);
  if(buf[0] != 'F' || buf[1] != '0')
  {
    close(fhdl);
    printf("glFont: %s Not a valid font file.", filename);
    return -1;
  }

  // Get the texture size, the number of glyphs and the line height.
  int width, height, n_chars;
  width = height = _line_height = n_chars = 0;
  pvmyread(fhdl, (char *) &width,        4); //sizeof(width));
  pvmyread(fhdl, buf, 4);
  pvmyread(fhdl, (char *) &height,       4); //sizeof(height));
  pvmyread(fhdl, buf, 4);
  pvmyread(fhdl, (char *) &_line_height, 4); //sizeof(_line_height));
  pvmyread(fhdl, (char *) &n_chars,      4); //sizeof(n_chars));
  pvmyread(fhdl, buf, 4);
  //printf("glFont: w=%d h=%d lh=%d nc=%d\n", width, height, _line_height, n_chars);

  _tex_line_height = (float) _line_height/((float) height);

  // Make the glyph table.
  //_glyphs = new Glyph[n_chars];
  for(i=0; i<256; i++)
  {
    _table[i] = NULL;
  }
  // Read every glyph, store it in the glyph array and set the right
  // pointer in the table.
  Glyph_Buffer buffer;
  for(i=0; i<n_chars && i<256; i++)
  {
    pvmyread(fhdl, (char *) &buffer, 6); //sizeof(buffer));
    _glyphs[i].tex_x1 = (float) ((float) buffer.x / (float) width);
    _glyphs[i].tex_x2 = (float) ((float) (buffer.x + buffer.width) / (float) width);
    _glyphs[i].tex_y1 = (float) ((float)  buffer.y / (float) height);
    _glyphs[i].advance = (int) buffer.width;
    _table[buffer.ascii] = _glyphs + i;
    //printf("x=%d y=%d w=%d a=%c ", buffer.x, buffer.y, buffer.width, buffer.ascii);
  }
  _line_height = _line_height;

  //printf("end\n");
  // All chars that do not have their own glyph are set to point to
  // the default glyph.
  Glyph* default_glyph = _table[(unsigned char)'\xFF'];
  for (i=0; i<256; i++)
  {
    if(_table[i] == NULL) _table[i] = default_glyph;
  }

  //printf("read w=%d h=%d\n", width, height);
  // Store the actual texture in an array.
  unsigned char *tex_data = new unsigned char[width * height];
  pvmyread(fhdl, (char *) tex_data, width*height);
  //printf("after read w=%d h=%d\n", width, height);

  // Generate an alpha texture with it.
printf("read1\n");  
  glGenTextures(1, &_texture);
printf("read2\n");  
  glBindTexture(GL_TEXTURE_2D, _texture);
printf("read3\n");  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
printf("read4\n");  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
printf("read5\n");  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, width, height, 0, GL_ALPHA,
               GL_UNSIGNED_BYTE, tex_data);
printf("read6\n");  
  // And delete the texture memory block
  delete[] tex_data;
  close(fhdl);
  return 0;
}

int glFont::lineHeight()
{
  return _line_height;
}

int glFont::charWidth(unsigned char c)
{
  return _table[c]->advance;
}

int glFont::stringWidth(const char *str)
{
  int total = 0;
  int len = strlen(str);
  for(int i=0; i<len; i++)
  {
    total += charWidth(str[i]);
  }
  return total;
}

void glFont::setZoom(float factor)
{
  zoom = factor;
}

void glFont::setFontSize(int pitch, float factor)
{
  zoom0 = pitch;
  zoom0 = zoom0 * factor;
  zoom0 = zoom0 / 24.0; // font generator generates 24 pitch font
}

void glFont::setRotation(int angle)
{
  angle_128x128 = angle+1; 
}

void glFont::drawString(float x, float y, const char *str)
{
  int rot= 90*angle_128x128/(128*128);
  glBindTexture(GL_TEXTURE_2D, _texture);

  // Simply draw quads textured with the current glyph for every
  // character, updating the x position as we go along.
  double zz = zoom0 * zoom;
  int len = strlen(str);
  //printf("rot=%d str=%s\n", rot, str);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if(rot == 0)
  {
    glBegin(GL_QUADS);
    for(int i = 0; i<len; i++)
    {
      Glyph *glyph = _table[(unsigned int) str[i]];
      glTexCoord2f(glyph->tex_x1, glyph->tex_y1);
      glVertex2f(x, y);
      glTexCoord2f(glyph->tex_x1, glyph->tex_y1 + _tex_line_height);
      glVertex2f((float) x, (float) (y + _line_height*zz));
      glTexCoord2f(glyph->tex_x2, glyph->tex_y1 + _tex_line_height);
      glVertex2f((float) (x + glyph->advance*zz), (float) (y + _line_height*zz));
      glTexCoord2f(glyph->tex_x2, glyph->tex_y1);
      glVertex2f((float) (x + (float) (glyph->advance*zz)), (float) y);
      x += (float) (glyph->advance*zz);
    }
    glEnd();
  }
  else
  {
    glPushMatrix();
    glTranslatef(x,y,0.0f);
    glRotatef((float) rot,0.0,0.0,1.0);
    x = 0.0f;
    y = 0.0f;
    glBegin(GL_QUADS);
    for(int i = 0; i<len; i++)
    {
      Glyph *glyph = _table[(unsigned int) str[i]];
      glTexCoord2f(glyph->tex_x1, glyph->tex_y1);
      glVertex2f(x, y);
      glTexCoord2f(glyph->tex_x1, glyph->tex_y1 + _tex_line_height);
      glVertex2f((float) x, (float) (y + _line_height*zz));
      glTexCoord2f(glyph->tex_x2, glyph->tex_y1 + _tex_line_height);
      glVertex2f((float) (x + glyph->advance*zz), (float) (y + _line_height*zz));
      glTexCoord2f(glyph->tex_x2, glyph->tex_y1);
      glVertex2f((float) (x + (float) (glyph->advance*zz)), (float) y);
      x += (float) (glyph->advance*zz);
    }
    glEnd();
    glPopMatrix();
  }  
  glDisable(GL_BLEND);
}

#endif
