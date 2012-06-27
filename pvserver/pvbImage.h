/***************************************************************************
                          pvbImage.h  -  description
                             -------------------
    begin                : Sun Okt 12 2001
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

#include "BMP.h"

typedef struct
{
  unsigned char b,g,r,reserved;
}LUT;

typedef struct
{
  int  w,h,bpp,nLut;
  LUT  *lut;
  void *image; 
}
PVB_IMAGE;

PVB_IMAGE *pvbImageRead(const char *filename);
void pvbImageFree(PVB_IMAGE *pvbImage);

int pvmyread(int fhdl, char *cptr, int len);
int pvopenBinary(const char *filename);
