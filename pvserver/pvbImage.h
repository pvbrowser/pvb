/***************************************************************************
                          pvbImage.h  -  description
                             -------------------
    begin                : Sun Okt 12 2001
    copyright            : (C) 2000 by Rainer Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/*********************************************************************************
 *                                                                               *
 * - The ProcessViewServer library is OpenSource software.                       *
 *   It can be used free for non commercial or education purposes.               *
 *   You can also evaluate the software as long as you desire.                   *
 *   If you want to use them in a commercial context you have to get a license.  *
 *   If you want to get a license please contact lehrig@t-online.de              *
 *   (C) Lehrig Software Engineering                                             *
 *                                                                               *
 ********************************************************************************/

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
