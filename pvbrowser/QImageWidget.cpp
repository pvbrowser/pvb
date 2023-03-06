/***************************************************************************
                          QImageWidget.cpp  -  description
                             -------------------
    begin                : Fri Nov 17 2000
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
#include "pvdefine.h"
#include <stdio.h>
#include <qwidget.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qpoint.h>
#include "qimagewidget.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>
#include "tcputil.h"
#include "opt.h"

extern OPT opt;

QImageWidget::QImageWidget(int *sock, int ident, QWidget *parent, const char *name, int wFlags )
             : QWidget(parent)
{
  image = QImage(); // construct a null image
  original_image = QImage();
  xx = yy = w = h = 0;
  s = sock;
  id = ident;
  if(name != NULL) setObjectName(name);
  //qt3 setBackgroundMode(Qt::NoBackground);
  //setAutoFillBackground(false);
  if(wFlags == -1000) return; //trollmurx
}

QImageWidget::~QImageWidget()
{
}

void QImageWidget::setImage(const QImage *newimage)
{
  image = newimage->copy();
  if(w > 0 && h > 0 && ( w < image.width() || h < image.height() ) )
  {
    image = image.scaled(w, h, Qt::KeepAspectRatio);
  }
  else if(w > image.width() || h > image.height())
  {
    //qt3 image = image.smoothScale(w,h,Qt::KeepAspectRatio);
    image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
  clearMask();
  perhapsSetMask();
  w = image.width();
  h = image.height();
  original_image = image.copy();
}

void QImageWidget::paintEvent( QPaintEvent *e )
{
  if( !image.isNull() )
  {
    QPainter p;
    p.begin(this);
    p.setClipRect(e->rect());
    p.drawImage(QPoint(0,0), image);
    p.end();
  }
}

void QImageWidget::mousePressEvent( QMouseEvent *e)
{
char buf[80];

  if(e->button() == Qt::LeftButton)
  {
    sprintf(buf,"QPushButton(%d) -xy=%d,%d\n",id, e->x(), e->y());
    tcp_send(s,buf,strlen(buf));
  }
  else if(e->button() == Qt::RightButton)
  {
    sprintf(buf,"QMouseRight(%d)\n",id);
    tcp_send(s,buf,strlen(buf));
  }
  QWidget::mousePressEvent(e);
}

void QImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
  char buf[80];

  if(event == NULL) return;
  sprintf(buf,"QPushButtonReleased(%d) -xy=%d,%d\n",id, event->x(), event->y());
  if(underMouse()) tcp_send(s,buf,strlen(buf));
  QWidget::mouseReleaseEvent(event);
}

void QImageWidget::enterEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,1)\n",id);
  tcp_send(s,buf,strlen(buf));
  QWidget::enterEvent(event);
}

void QImageWidget::leaveEvent(QEvent *event)
{
  char buf[100];
  sprintf(buf, "mouseEnterLeave(%d,0)\n",id);
  tcp_send(s,buf,strlen(buf));
  QWidget::leaveEvent(event);
}

void QImageWidget::setGeometry(int nx, int ny, int nw, int nh)
{
  xx = nx;
  yy = ny;
  w  = nw;
  h  = nh;
  scale(w,h);
  QWidget::setGeometry(xx,yy,w,h);
  repaint(0,0,w,h);
}
/*
{
  xx = nx;
  yy = ny;
  w  = nw;
  h  = nh;
  //move(xx,yy);
  //resize(w,h);
  image = original_image.copy();
  QWidget::setGeometry(xx,yy,w,h);
  if(w > 0 && h > 0 && ( w < image.width() || h < image.height() ) )
  {
    image = image.scaled(w, h, Qt::KeepAspectRatio);
    clearMask();
    perhapsSetMask();
  }
  else if(w > image.width() || h > image.height())
  {
    //qt3 image = image.smoothScale(w,h,Qt::KeepAspectRatio);
    image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    clearMask();
    perhapsSetMask();
  }
  repaint(0,0,w,h);
}
*/

void QImageWidget::scale(int w, int h)
{
  //printf("scale(%d,%d)\n",w,h);
  image = original_image.copy();
  if(w > 0 && h > 0 && ( w < image.width() || h < image.height() ) )
  {
    image = image.scaled(w, h, Qt::KeepAspectRatio);
    clearMask();
    perhapsSetMask();
  }
  else if(w > image.width() || h > image.height())
  {
    //qt3 image = image.smoothScale(w,h,Qt::KeepAspectRatio);
    image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    clearMask();
    perhapsSetMask();
  }
}

void QImageWidget::resizeEvent(QResizeEvent *event)
{
  QSize size = event->size();
  //printf("resizeEvent(%d,%d)\n",size.width(),size.height());
  scale(size.width(), size.height());
  return QWidget::resizeEvent(event);
}

void QImageWidget::setImage(const char *filename, int rotate)
{
  image.load(filename);
  if(rotate)
  {
    //rlmurx-was-here QMatrix m;
    //                m.rotate(rotate);
    //                image = image.transformed(m);
    QTransform m;
    m.rotate(rotate);
    image = image.transformed(m);
  }
/*  
//#ifdef USE_MAEMO 
//  unlink(filename);
//#endif  
  clearMask();
  if(w > 0 && h > 0 && ( w < image.width() || h < image.height() ) )
  {
    //printf("set1: setImage %s xy=%d,%d w=%d h=%d width=%d height=%d\n", filename, 
    //x(), y(), w, h, 
    //                                  image.width(),image.height());
    image = image.scaled(w, h, Qt::KeepAspectRatio);
  }
  else if(w > image.width() || h > image.height())
  {
    //printf("set2: setImage %s xy=%d,%d w=%d h=%d width=%d height=%d\n", filename, 
    //x(),y(),w, h, 
    //                                  image.width(),image.height());
    //qt3 image = image.smoothScale(w,h,Qt::KeepAspectRatio);
    image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  if(strstr(filename,".bmp") != NULL || strstr(filename,".BMP") != NULL)
  { // it may be a bmp with transparent background
#if QT_VERSION < 0x050000
    int n = image.numColors();
#else
    int n = image.colorCount();
#endif
    for(int icol=0; icol<n; icol++)
    {
      QRgb qcol = image.color(icol);
      if(qRed(qcol) == 1 && qGreen(qcol) == 1 && qBlue(qcol) == 1)
      { // image has transparent background
        //image.setAlphaBuffer(true);
        image.setColor(icol,qRgba(1,1,1,0));
        image.createAlphaMask();
      }
    }
  }
  perhapsSetMask();
*/  
  original_image = image.copy();
  scale(width(),height());
  repaint();
}

void QImageWidget::setJpegImage(unsigned char *buffer, int buffersize, int rotate)
{
  if(opt.arg_debug) printf("QImageWidget::setJpegImage buffersize=%d\n", buffersize);
  image.loadFromData(buffer, buffersize, "JPG");
  clearMask();
  if(rotate)
  {
    //rlmurx-was-here QMatrix m;
    //                m.rotate(rotate);
    //                image = image.transformed(m);
    QTransform m;
    m.rotate(rotate);
    image = image.transformed(m);
  }
/*  
  if(w > 0 && h > 0 && ( w < image.width() || h < image.height() ) )
  {
    //printf("set1: setImage %s xy=%d,%d w=%d h=%d width=%d height=%d\n", filename, 
    //x(), y(), w, h, 
    //                                  image.width(),image.height());
    image = image.scaled(w, h, Qt::KeepAspectRatio);
  }
  else if(w > image.width() || h > image.height())
  {
    //printf("set2: setImage %s xy=%d,%d w=%d h=%d width=%d height=%d\n", filename, 
    //x(),y(),w, h, 
    //                                  image.width(),image.height());
    //qt3 image = image.smoothScale(w,h,Qt::KeepAspectRatio);
    image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  perhapsSetMask();
*/  
  original_image = image.copy();
  scale(width(),height());
  repaint();
}

void QImageWidget::setRGBA(unsigned char *buffer, int width, int height, int rotate)
{
  if(opt.arg_debug) printf("QImageWidget::setRGBA width=%d height=%d rotate=%d\n", width,height,rotate);
  QImage tmpimage(width,height,QImage::Format_ARGB32);
  /*
  unsigned int rgba,amask;

  int ind = 0;
  for(int iy=0; iy<height; iy++)
  {
    for(int ix=0; ix<width; ix++)
    {
      rgba = (buffer[ind]*256 + buffer[ind+1])*256 + buffer[ind+2];
      //rgba |= 0x0ff000000;
      amask = buffer[ind+3];
      amask = amask << 24;
      rgba |= amask;
      tmpimage.setPixel(ix,iy,QRgb(rgba));
      ind += 4;
    }
  }
  */
  int ind = 0;
  for(int iy=0; iy<height; iy++)
  {
    uchar *dest = tmpimage.scanLine(iy);
    memcpy(dest, &buffer[ind], width*4);
    ind += (width * 4);
  }
  
  image = tmpimage.copy();

  clearMask();
  if(rotate)
  {
    //rlmurx-was-here QMatrix m;
    //                m.rotate(rotate);
    //                image = image.transformed(m);
    QTransform m;
    m.rotate(rotate);
    image = image.transformed(m);
  }
  if(opt.arg_debug) printf("image width=%d height=%d\n", image.width() , image.height());
  if(w > 0 && h > 0 && ( w < image.width() || h < image.height() ) )
  {
    //printf("set1: setImage %s xy=%d,%d w=%d h=%d width=%d height=%d\n", filename, 
    //x(), y(), w, h, 
    //                                  image.width(),image.height());
    image = image.scaled(w, h, Qt::KeepAspectRatio);
  }
  else if(w > image.width() || h > image.height())
  {
    //printf("set2: setImage %s xy=%d,%d w=%d h=%d width=%d height=%d\n", filename, 
    //x(),y(),w, h, 
    //                                  image.width(),image.height());
    //qt3 image = image.smoothScale(w,h,Qt::KeepAspectRatio);
    image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
  
  perhapsSetMask();
  original_image = image.copy();
  repaint();
}

void QImageWidget::perhapsSetMask()
{
/* qt3
  if(image.hasAlphaBuffer())
  {
    mask = image.createAlphaMask();
    setMask(mask);
  }
*/
  if(image.hasAlphaChannel())
  {
/* qt4porting ???
    mask = image.createAlphaMask();
    setMask(mask);
*/
  }
}

int QImageWidget::sendJpeg2clipboard()
{
  char buf[80];
  QByteArray bytes;
  QBuffer qb_buffer(&bytes);
  qb_buffer.open(QIODevice::WriteOnly);
  original_image.save(&qb_buffer, "JPG"); // writes pixmap into bytes in JPG format
  sprintf(buf,"@clipboard(%d,%d)\n", id, (int) qb_buffer.size());
  tcp_send(s,buf,strlen(buf));
  return tcp_send(s, qb_buffer.data(), qb_buffer.size());
}  

