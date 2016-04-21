/***************************************************************************
                          rlsvgsdi.cpp  -  description
                             -------------------
    begin                : Tu Mar 17 2016
    copyright            : (C) 2016 by R. Lehrig
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
#include "rlsvgvdi.h"
#include "rlcutil.h"
#include <math.h>
#include <stdarg.h>
#ifdef RLWIN32
#include <winsock2.h>
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

#ifdef PVMAC
#define MSG_NOSIGNAL 0
#endif

rlSvgVdi::rlSvgVdi()
{
  sout = NULL;
  fout = NULL;
  pipe = NULL;
  idForPvbrowser = 0;
  outputState = 0;
  fontsize = 10;
  fontstring.printf("font-family=\"%s\" font-weight=\"%s\" font-size=\"%d\"", "Times", "normal", fontsize);
  strcpy(stroke,"#000000"); // black
  lineoptions.printf("stroke=\"%s\"", stroke);
  textoptions.printf("stroke=\"%s\" %s", stroke, fontstring.text());
  strcpy(floatFormat,"%.2f");
  dx = dy = 1.0f;
  sr = 3;
  boxx = boxy = boxw = boxh = 0;
  xmin = 0.0f;
  xmax = 100.0f;
  ymin = 0.0f;
  ymax = 100.0f;
  width = 1;
  xold = yold = 0;
}

rlSvgVdi::~rlSvgVdi()
{
  endOutput();
}

int rlSvgVdi::setOutput(int *_socket_out, int _idForPvbrowser)
{
  pipe = NULL;
  sout = _socket_out;
  if(*sout < 0)
  {
    outputState = -1;
    return -1;
  }
  if(strlen(outputfilename.text()) == 0)
  {
  }
  else
  {
    if(fout != NULL) fclose(fout);
  }
  fout = NULL;
  outputfilename = "";
  idForPvbrowser = _idForPvbrowser;
  outputState = 1;
  if(idForPvbrowser > 0)
  {
    char buf[80];
    sprintf(buf,"gsvgRead(%d)\n", idForPvbrowser);
    perhapsSend(buf);
  }
  return 0;
}

int rlSvgVdi::setOutput(FILE *_fout)
{
  pipe = NULL;
  sout = NULL;
  if(strlen(outputfilename.text()) == 0)
  {
  }
  else
  {
    if(fout != NULL) fclose(fout);
  }
  fout = _fout;
  if(fout == NULL)
  {
    outputState = -1;
    return -1;
  }
  outputfilename = "";
  outputState = 1;
  return 0;
}

int rlSvgVdi::setOutput(const char *_outputfilename)
{
  pipe = NULL;
  sout = NULL;
  if(strlen(outputfilename.text()) == 0)
  {
  }
  else
  {
    if(fout != NULL) fclose(fout);
  }
  outputfilename = _outputfilename;
  fout = fopen(_outputfilename,"w");
  if(fout == NULL) 
  {
    outputState = -1;
    return -1;
  } 
  outputState = 1;
  return 0;
}

int rlSvgVdi::setOutput(rlSpawn *_pipe)
{
  pipe = _pipe;
  sout = NULL;
  if(strlen(outputfilename.text()) == 0)
  {
  }
  else
  {
    if(fout != NULL) fclose(fout);
  }
  outputfilename = "";
  if(pipe == NULL)
  {
    outputState = -1;
    return -1;
  }
  outputState = 1;
  return 0;
}

int rlSvgVdi::endOutput()
{
  if(sout != NULL && idForPvbrowser > 0)
  {
    char buf[80];
    sprintf(buf,"\n<svgend></svgend>\n");
    perhapsSend(buf);
    sprintf(buf,"gbeginDraw(%d)\n", idForPvbrowser);
    perhapsSend(buf);
    sprintf(buf,"gupdateSVG(%d)\n", idForPvbrowser);
    perhapsSend(buf);
    sprintf(buf,"gendDraw\n");
    perhapsSend(buf);
  }
  if(strlen(outputfilename.text()) == 0)
  {
  }
  else
  {
    if(fout != NULL) fclose(fout);
    fout = NULL;
  }
  if(pipe != NULL)
  {
    char buf[4];
    buf[0] = EOF;
    buf[1] = '\0';
    pipe->writeString(buf);
    pipe = NULL;
  }
  svg  = "";
  svg2 = "";
  svg3 = "";
  if(outputState < 0) return -1;
  outputState = 0;
  return 0;
}

int rlSvgVdi::perhapsSend(const char *text)
{
  if(outputState <= 0) return -1;
  if(sout != NULL)
  {
    int ret,bytes_left,first_byte;
    const char *cbuf;
    if(sout == NULL) return -1;
    if(*sout == -1) return -1;
    cbuf = text;
    bytes_left = strlen(text);
    first_byte = 0;

    while(bytes_left > 0)
    {
      ret = send(*sout,&cbuf[first_byte],bytes_left,MSG_NOSIGNAL);
      if(ret <= 0)
      {
        //disconnect();
        if(*sout != -1)
        {
#ifdef RLWIN32
          closesocket(*sout);
#else
          close(*sout);
#endif
        }
        *sout = -1;
        outputState = -1;
        return -1;
      }
      bytes_left -= ret;
      first_byte += ret;
    }
  }
  else if(fout != NULL)
  {
    if(fputs(text, fout) < 0)
    {
      outputState = -1;
    }  
  }
  else if(pipe != NULL)
  {
    if(pipe->writeString(text) < 0)
    {
      outputState = -1;
    }
  }
  else
  {
    outputState = 0;
  }
  return 0;
}

const char *rlSvgVdi::svgHeader(int width, int height, int rbackground, int gbackground, int bbackground)
{
  svg.printf("<svg width=\"%dpx\" height=\"%dpx\" viewBox=\"0 0 %d %d\" style=\"background: rgb(%d,%d,%d)\"\n", width, height, width, height, rbackground, gbackground, bbackground);
  svg += "  xmlns=\"http://www.w3.org/2000/svg\" version=\"1.2\" baseProfile=\"tiny\">\n";
  svg += "  <desc>svgvdi for smf files</desc>\n";
  perhapsSend(svg.text());
  return svg.text();
}

const char *rlSvgVdi::svgTrailer()
{
  svg = "</svg>\n";
  perhapsSend(svg.text());
  return svg.text();
}

const char *rlSvgVdi::drawEllipse(int x, int y, int rx, int ry)
{
  svg.printf("<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" %s />\n", x, y, rx, ry, lineoptions.text());
  perhapsSend(svg.text());
  return svg.text();
}

const char* rlSvgVdi::moveTo(int x, int y)
{
  xold = x;
  yold = y;
  return "";
}

const char *rlSvgVdi::lineTo(int x, int y)
{
  int xnew, ynew;
  xnew = x;
  ynew = y;
  svg.printf("<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" %s />\n", xold, yold, xnew, ynew, lineoptions.text());
  xold = xnew;
  yold = ynew;
  perhapsSend(svg.text());
  return svg.text();
}

const char *rlSvgVdi::line(int x1, int y1, int x2, int y2)
{
  int xnew, ynew;
  xold = x2;
  yold = y2;
  xnew = x1;
  ynew = y1;
  svg.printf("<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" %s />\n", xold, yold, xnew, ynew, lineoptions.text());
  perhapsSend(svg.text());
  return svg.text();
}

const char *rlSvgVdi::textInAxis(float x, float y, const char *txt, int alignment)
{
  float dy;
/*
  ymax-ymin        dy
  -----------   =  ----------
  boxh             fontsize/2
*/
  if(alignment == ALIGN_CENTER)
  {
    dy = ((ymax-ymin) * (fontsize/2) ) / (boxh);
    return text(tx(x), ty(y+dy), txt, alignment);
  }
  else
  {
    return text(tx(x), ty(y), txt, alignment);
  }
}

const char *rlSvgVdi::text(int x, int y, const char *txt, int alignment)
{
  switch(alignment)
  {
    case ALIGN_LEFT:     //  |example
      svg.printf("<text x=\"%d\" y=\"%d\" %s>%s</text>\n", x, y, textoptions.text(), txt);
      break;
    case ALIGN_CENTER:   //  exa|mple
      svg.printf("<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" %s>%s</text>\n", x, y, textoptions.text(), txt);
      break;
    case ALIGN_RIGHT:    //  example|
      svg.printf("<text x=\"%d\" y=\"%d\" text-anchor=\"end\" %s>%s</text>\n", x, y, textoptions.text(), txt);
      break;
    case ALIGN_VERT_CENTER:
      svg.printf("<g transform=\"translate(%d,%d)\" >\n  <g transform=\"rotate(-90)\" >\n    <text x=\"0\" y=\"0\" text-anchor=\"middle\" %s>%s</text>\n  </g>\n</g>\n", x, y, textoptions.text(), txt);
      break;
    default:
      svg.printf("<text x=\"%d\" y=\"%d\" %s>%s/text>\n", x, y, textoptions.text(), txt);
      break;
  }
  perhapsSend(svg.text());
  return svg.text();
}

const char *rlSvgVdi::box(int x, int y, int w, int h)
{
  boxx = x;
  boxy = y;
  boxw = w;
  boxh = h;
  svg2  = moveTo(x  ,y  );
  svg2 += lineTo(x+w,y  );
  svg2 += lineTo(x+w,y+h);
  svg2 += lineTo(x  ,y+h);
  svg2 += lineTo(x  ,y  );
  return svg2.text();
}

const char *rlSvgVdi::boxWithText(int x, int y, int w, int h, int fontsize, const char *xlabel, const char *ylabel, const char * rylabel)
{
  svg3 = box(x,y,w,h);
  if(xlabel  != NULL) svg3 += text(x+w/2,y+h+fontsize*3,xlabel,ALIGN_CENTER);
  if(ylabel  != NULL) svg3 += text(x-fontsize*5,y+h/2,ylabel,ALIGN_VERT_CENTER);
  if(rylabel != NULL) svg3 += text(x+w+fontsize*5,y+h/2,rylabel,ALIGN_VERT_CENTER);
  return svg3.text();
}

const char *rlSvgVdi::rect(int x, int y, int w, int h)
{
  svg2  = moveTo(x   ,y  );
  svg2 += lineTo(x+w ,y  );
  svg2 += lineTo(x+w ,y+h);
  svg2 += lineTo(x   ,y+h);
  svg2 += lineTo(x   ,y  );
  return svg2.text();
}

static int beautyval(char *text)
{
  char *cptr = strchr(text,'.');
  if(cptr != NULL)
  {
    char *end = cptr + strlen(cptr) - 1;
    while(end != cptr) // eliminate trailing 0
    {
      if     (*end == '0') { *end = '\0'; }
      else if(*end == '.') { *end = '\0'; break; }
      else                 { break; }
      end--;
    }
    if(cptr == end) *cptr = '\0';
  }
  return 0;
}

const char *rlSvgVdi::xGrid()
{
  svg2 = "";
  for(float x=(xmin+dx); x<xmax; x+=dx)
  {
    svg2 += moveTo(tx(x),boxy);
    svg2 += lineTo(tx(x),boxy+boxh);
  }
  return svg2.text();
}

const char *rlSvgVdi::yGrid()
{
  svg2 = "";
  for(float y=(ymin+dy); y<=(ymax-dy); y+=dy)
  {
    svg2 += moveTo(boxx,ty(y));
    svg2 += lineTo(boxx+boxw, ty(y));
  }
  return svg2.text();
}

const char *rlSvgVdi::xAxis(float start, float delta, float end, int draw)
{
  float x;
  int   y;
  char  txt[80];

  svg2 = "";
  xmin = start;
  dx   = delta;
  xmax = end;
  if(draw==0) return "";

  x = start;
  y = boxy + boxh;

  while(x <= end)
  {
    svg2 += moveTo(tx(x),y);
    svg2 += lineTo(tx(x),y+8);
    sprintf(txt,floatFormat,x);
    beautyval(txt);
    svg2 += text(tx(x),y+((3*fontsize)/2),txt,ALIGN_CENTER);
    x += delta;
  }

  if(start < 0.0f && end > 0.0f)
  {
    svg2 += moveTo(tx(0.0f),boxy);
    svg2 += lineTo(tx(0.0f),boxy+boxh);
  }
  return svg2.text();
}

const char *rlSvgVdi::yAxis(float start, float delta, float end, int draw)
{
float y;
int   x;
char  txt[80];

  svg2 = "";
  ymin = start;
  dy   = delta;
  ymax = end;
  if(draw==0) return "";
  x = boxx;
  y = start;

  while(y <= end)
  {
    svg2 += moveTo(x,ty(y));
    svg2 += lineTo(x-8,ty(y));
    sprintf(txt,floatFormat,y);
    beautyval(txt);
    svg2 += text(x-10,ty(y),txt,ALIGN_RIGHT);
    y += delta;
  }


  if(start < 0.0f && end > 0.0f)
  {
    svg2 += moveTo(boxx,ty(0.0f));
    svg2 += lineTo(boxx+boxw,ty(0.0f));
  }
  return svg2.text();
}

const char *rlSvgVdi::rightYAxis(float start, float delta, float end, int draw)
{
float y;
int   x;
char  txt[80];

  ymin = start;
  dy   = delta;
  ymax = end;
  if(draw==0) return "";
  x = boxx+boxw;
  y = start;

  svg2 = "";
  while(y <= end)
  {
    xold = x;
    yold = ty(y);
    svg2 += moveTo(xold, yold);
    svg2 += lineTo(xold+8,yold);
    //p.drawLine(x,ty(y),xold,yold);
    sprintf(txt,floatFormat,y);
    beautyval(txt);
    svg2 += text(xold+10,yold,txt,ALIGN_LEFT);
    y += delta;
  }

  if(start < 0.0f && end > 0.0f)
  {
    int xnew, ynew;
    xold = boxx+boxw;
    yold = ty(0.0f);
    xnew = boxx+boxw;
    ynew = ty(0.0f);
    svg2 += moveTo(xold,yold);
    svg2 += lineTo(xnew,ynew);
    //p.drawLine(xold,yold,xnew,ynew);
    xold = xnew;
    yold = ynew;
  }
  return svg2.text();
}

const char *rlSvgVdi::drawSymbol(int x, int y, int type)
{
  //const char *cptr;
  int xnew, ynew;
  svg2 = "";
  switch(type)
  {
    case LINESTYLE_CIRCLE:
    case LINESTYLE_CIRCLE_NO_LINE:
      svg2 += drawEllipse(x-sr,y-sr,sr,sr);
      break;
    case LINESTYLE_CROSS:
    case LINESTYLE_CROSS_NO_LINE:
      xold = x+sr;
      yold = y-sr;
      svg2 += line(x-sr,y+sr,xold,yold);
      xold = x+sr;
      yold = y+sr;
      svg2 += line(x-sr,y-sr,xold,yold);
      break;
    case LINESTYLE_RECT:
    case LINESTYLE_RECT_NO_LINE:
      xold = x+sr;
      yold = y-sr;
      svg2 += line(x-sr,y-sr,xold,yold);
      xnew = x+sr;
      ynew = y+sr;
      svg2 += line(xold,yold,xnew,ynew);
      xold = xnew;
      yold = ynew;
      xnew = x-sr;
      ynew = y+sr;
      svg2 += line(xold,yold,xnew,ynew);
      xold = xnew;
      yold = ynew;
      xnew = x-sr;
      ynew = y-sr;
      svg2 += line(xold,yold,xnew,ynew);
      xold = xnew;
      yold = ynew;
      break;
    default:
      break;
  }
  return svg2.text();
}

const char *rlSvgVdi::line(float *x, float *y, int n, int linestyle)
{
  char buf[80];
  svg3 = "";
  if(linestyle != LINESTYLE_CROSS_NO_LINE && linestyle != LINESTYLE_CIRCLE_NO_LINE &&linestyle != LINESTYLE_RECT_NO_LINE )
  {
    //svg3 = moveTo(tx(x[0]),ty(y[0]));
    //for(int i=1; i<n; i++) svg3 += lineTo(tx(x[i]),ty(y[i]));
    svg3 = "<polyline points=\"";
    for(int i=0; i<n; i++)
    {
      sprintf(buf," %d,%d", tx(x[i]), ty(y[i]));
      svg3 += buf;
    }
    svg3 += "\"\n";
    svg3 += lineoptions.text();
    svg3 += " fill=\"none\"/>\n";
    perhapsSend(svg3.text());
  }  
  if(linestyle != LINESTYLE_NONE)
  {
    for(int i=1; i<n; i++) svg3 += drawSymbol(tx(x[i]),ty(y[i]),linestyle);
  }
  return svg3.text();
}

const char *rlSvgVdi::setColor(int r, int g, int b)
{
  sprintf(stroke,"#%02x%02x%02x", r, g, b);
  lineoptions.printf("stroke=\"%s\"", stroke);
  if(strlen(dasharray.text()) > 0)
  {
    lineoptions += " stroke-dasharray=\"";
    lineoptions += dasharray.text();
    lineoptions += "\"";
  }
  if(width != 1)
  {
    rlString buf;
    buf.printf(" stroke-width=\"%d\"", width);
    lineoptions += buf.text();
  }
  textoptions.printf("stroke=\"%s\" %s", stroke, fontstring.text());
  return "";
}

const char *rlSvgVdi::setFont(const char *family, int size, int weight, int italic)
{
  fontsize = size;
  if(weight == 1) fontstring.printf("font-family=\"%s\" font-weight=\"%s\" font-size=\"%d\"", family, "normal", size);
  else            fontstring.printf("font-family=\"%s\" font-weight=\"%s\" font-size=\"%d\"", family, "bold",   size);
  if(italic == 1) fontstring += " font-style=\"italic\"";
  textoptions.printf("stroke=\"%s\" %s", stroke, fontstring.text());
  return "";
}

const char *rlSvgVdi::setWidth(int w)
{
  width = w;
  lineoptions.printf("stroke=\"%s\"", stroke);
  if(strlen(dasharray.text()) > 0)
  {
    lineoptions += " stroke-dasharray=\"";
    lineoptions += dasharray.text();
    lineoptions += "\"";
  }
  if(width != 1)
  {
    rlString buf;
    buf.printf(" stroke-width=\"%d\"", width);
    lineoptions += buf.text();
  }
  return "";
}

const char *rlSvgVdi::setLinestyle(const char *_dasharray)
{
  dasharray = _dasharray;
  lineoptions.printf("stroke=\"%s\"", stroke);
  if(strlen(dasharray.text()) > 0)
  {
    lineoptions += " stroke-dasharray=\"";
    lineoptions += dasharray.text();
    lineoptions += "\"";
  }
  if(width != 1)
  {
    rlString buf;
    buf.printf(" stroke-width=\"%d\"", width);
    lineoptions += buf.text();
  }
  return "";
}

const char *rlSvgVdi::comment(const char *text)
{
  svg.printf("<!-- %s -->\n", text);
  perhapsSend(svg.text());
  return svg.text();
}

const char *rlSvgVdi::svg_put(const char *text)
{
  perhapsSend(text);
  return text;
}

const char *rlSvgVdi::svg_printf(const char *format, ...)
{
  int ret;
  char message[rl_PRINTF_LENGTH]; // should be big enough
  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return "";
  return svg_put(message);
}

const char *rlSvgVdi::svg_d(int *x, int *y, int count)
{
  char buf[80];
  svg = "d=\"";
  sprintf(buf,"M %d %d", x[0], y[0]);
  svg += buf;
  for(int i=1; i<count; i++)
  {
    sprintf(buf," L %d %d", x[i], y[i]);
    svg += buf;
  }
  svg += "\"\n";
  perhapsSend(svg.text());
  return svg.text();
}
    
const char *rlSvgVdi::svg_points(int *x, int *y, int count)
{
  char buf[80];
  svg = "points=\"";
  for(int i=0; i<count; i++)
  {
    sprintf(buf," %d,%d", x[i], y[i]);
    svg += buf;
  }
  svg += "\"\n";
  perhapsSend(svg.text());
  return svg.text();
}

int rlSvgVdi::tx(float x)
{
  return (int)(boxx + (((x-xmin)*boxw)/(xmax-xmin)));    // x          i
}                                                        // xmax-xmin  boxw

int rlSvgVdi::ty(float y)
{
  int i0;
  i0 = (int)((ymin * boxh)/(ymax-ymin));
  return i0 + (int)(boxy+boxh-((y*boxh)/(ymax-ymin))); // y          i        ymin       i0
}                                                      // ymax-ymin  boxh     ymax-ymin  boxh

int rlSvgVdi::getFontsize()
{
  return fontsize;
}

int rlSvgVdi::getBoxX()
{
  return boxx;
}

int rlSvgVdi::getBoxY()
{
  return boxy;
}

int rlSvgVdi::getBoxW()
{
  return boxw;
}

int rlSvgVdi::getBoxH()
{
  return boxh;
}

/*
int interpret(const char *linebuf)
{
int x,y,w,h,r,g,b,n,i;

  if(opt.arg_debug > 1) printf("rlSvgVdi=%s",linebuf);
  switch(linebuf[1])
  {
    case 'b':
      if(strncmp(linebuf,"gbeginDraw",10) == 0)
      {
        strcpy(floatFormat,"%.2f");
        beginDraw(1);
      }
      else if(strncmp(linebuf,"gbox",4) == 0)
      {
        sscanf(linebuf,"gbox(%d,%d,%d,%d)",&x,&y,&w,&h);
        box(x,y,w,h);
      }
      break;
    case 'd':
      if(strncmp(linebuf,"gdrawArc",8) == 0)
      {
        int astart,alen;
        sscanf(linebuf,"gdrawArc(%d,%d,%d,%d,%d,%d)",&x,&y,&w,&h,&astart,&alen);
        p.drawArc(zx(x),zy(y),zx(w),zy(h),astart*16,alen*16);
      }
      else if(strncmp(linebuf,"gdrawPie",8) == 0)
      {
        int astart,alen;
        sscanf(linebuf,"gdrawPie(%d,%d,%d,%d,%d,%d)",&x,&y,&w,&h,&astart,&alen);
        p.drawPie(zx(x),zy(y),zx(w),zy(h),astart*16,alen*16);
      }
      else if(strncmp(linebuf,"gdrawPolygon",12) == 0)
      {
        char buf[80];
        int cnt;
        sscanf(linebuf,"gdrawPolygon(%d)",&cnt);
        QPolygon points(cnt);
        i=0;
        while(1)
        {
          gets(buf,sizeof(buf)-1);
          if(buf[0] == '}') break;
          sscanf(buf,"%d,%d",&x,&y);
          points.setPoint(i++,zx(x),zy(y));
        }
        p.drawPolygon(points);
      }
      break;
    case 'e':
      if(strncmp(linebuf,"gendDraw",8) == 0)
      {
        strcpy(floatFormat,"%.2f");
        endDraw();
        return -1;
      }
      break;
    case 'l':
      if(strncmp(linebuf,"glineTo",7) == 0)
      {
        sscanf(linebuf,"glineTo(%d,%d)",&x,&y);
        lineTo(x,y);
      }
      else if(strncmp(linebuf,"gline(",6) == 0)
      {
        int maxarray = MAXARRAY;
        const char *cptr;
        cptr = strstr(linebuf,"-n=");
        if(cptr != NULL) sscanf(cptr,"-n=%d", &maxarray);
        if(maxarray < 1) maxarray = MAXARRAY;
        float xa[maxarray],ya[maxarray];
        char buf[80];
        for(n=0; 1; n++)
        {
          if(gets(buf,sizeof(buf)-1) == -1) break;
          if(buf[0] == '}') break;
          if(n >= maxarray) break;
          sscanf(buf,"%f,%f",&xa[n],&ya[n]);
        }
        if(n>0) line(xa,ya,n);
      }
      break;
    case 'm':
      if(strncmp(linebuf,"gmoveTo",7) == 0)
      {
        sscanf(linebuf,"gmoveTo(%d,%d)",&x,&y);
        moveTo(x,y);
      }
      break;
    case 'p':
      if(strncmp(linebuf,"gplaySVGsocket",14) == 0)
      {
        socketPlaySVG();
      }
      else if(strncmp(linebuf,"gplaySVG",8) == 0)
      {
        char filename[1024];
        getText(linebuf,filename);
        playSVG(filename);
      }
      break;
    case 'r':
      if(strncmp(linebuf,"grect",5) == 0)
      {
        sscanf(linebuf,"grect(%d,%d,%d,%d)",&x,&y,&w,&h);
        rect(x,y,w,h);
      }
      else if(strncmp(linebuf,"grightYAxis",11) == 0)
      {
        float start,delta,end;
        int draw;
        sscanf(linebuf,"grightYAxis(%f,%f,%f,%d)",&start,&delta,&end,&draw);
        rightYAxis(start,delta,end,draw);
      }
      else if(strncmp(linebuf,"grotate",7) == 0)
      {
        float rx;
        sscanf(linebuf,"grotate(%f)",&rx);
        p.rotate(rx);
      }
      break;
    case 's':
      if(strncmp(linebuf,"gsetBackgroundColor",19) == 0)
      {
        sscanf(linebuf,"gsetBackgroundColor(%d,%d,%d)",&r,&g,&b);
        setBackgroundColor(r,g,b);
      }
      else if(strncmp(linebuf,"gsetColor",9) == 0)
      {
        sscanf(linebuf,"gsetColor(%d,%d,%d)",&r,&g,&b);
        setColor(r,g,b);
      }
      else if(strncmp(linebuf,"gsetWidth",9) == 0)
      {
        QPen pen;
        sscanf(linebuf,"gsetWidth(%d)",&w);
        pen = p.pen();
        pen.setWidth(w);
        p.setPen(pen);
      }
      else if(strncmp(linebuf,"gsetStyle",9) == 0)
      {
        QPen pen;
        sscanf(linebuf,"gsetStyle(%d)",&i);
        pen = p.pen();
        if((Qt::PenStyle) i == Qt::NoPen) i = 1;
        pen.setStyle((Qt::PenStyle) i);
        //if(i==0) pen.setStyle(Qt::SolidLine);
        //if(i==1) pen.setStyle(Qt::DotLine);
        p.setPen(pen);
      }
      else if(strncmp(linebuf,"gsetFont",8) == 0)
      {
        char family[80];
        const char *cptr;
        int  size,weight,italic;
        getText(linebuf,family);
        cptr = strstr(linebuf,"\",");
        if(cptr != NULL)
        {
          cptr += 2;
          sscanf(cptr,"%d,%d,%d",&size,&weight,&italic);
          setFont(family,size,weight,italic);
        }
      }
      else if(strncmp(linebuf,"gsetLinestyle",13) == 0)
      {
        sscanf(linebuf,"gsetLinestyle(%d)",&i);
        setLinestyle(i);
      }
      else if(strncmp(linebuf,"gsetFloatFormat",15) == 0)
      {
        char txt[1024];
        getText(linebuf,txt);
        strcpy(floatFormat,txt);
      }
      else if(strncmp(linebuf,"gscale",6) == 0)
      {
        float rx,ry;
        sscanf(linebuf,"gscale(%f,%f)",&rx,&ry);
        p.scale(rx,ry);
      }
      break;
    case 't':
      if(strncmp(linebuf,"gtext(",6) == 0)
      {
        char txt[1024];
        int alignment;
        sscanf(linebuf,"gtext(%d,%d,%d,",&x,&y,&alignment);
        getText(linebuf,txt);
        text(x,y,alignment,txt);
      }
      else if(strncmp(linebuf,"gtextInAxis(",12) == 0)
      {
        char txt[1024];
        int alignment;
        float fx,fy;
        sscanf(linebuf,"gtextInAxis(%f,%f,%d,",&fx,&fy,&alignment);
        getText(linebuf,txt);
        textInAxis(fx,fy,alignment,txt);
      }
      else if(strncmp(linebuf,"gtranslate",10) == 0)
      {
        float rx,ry;
        sscanf(linebuf,"gtranslate(%f,%f)",&rx,&ry);
        p.translate(rx,ry);
      }
      break;
    case 'u':
      if(strncmp(linebuf,"gupdateSVG(",11) == 0)
      {
        if(svgAnimator != NULL) svgAnimator->update(0);
      }
      break;
    case 'x':
      if(strncmp(linebuf,"gxAxis",6) == 0)
      {
        float start,delta,end;
        int draw;
        sscanf(linebuf,"gxAxis(%f,%f,%f,%d)",&start,&delta,&end,&draw);
        xAxis(start,delta,end,draw);
      }
      else if(strncmp(linebuf,"gxGrid",6) == 0)
      {
        xGrid();
      }
      break;
    case 'y':
      if(strncmp(linebuf,"gyAxis",6) == 0)
      {
        float start,delta,end;
        int draw;
        sscanf(linebuf,"gyAxis(%f,%f,%f,%d)",&start,&delta,&end,&draw);
        yAxis(start,delta,end,draw);
      }
      else if(strncmp(linebuf,"gyGrid",6) == 0)
      {
        yGrid();
      }
      break;
  }
  return 0;
}
*/

