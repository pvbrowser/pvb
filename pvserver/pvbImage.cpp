/***************************************************************************
                          pvbImage.cpp  -  description
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
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "processviewserver.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef PVWIN32
#include <io.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <fcntl.h>
#include <memory.h>
#include "pvbImage.h"

int pvmyread(int fhdl, char *cptr, int len)
{
#ifdef __VMS
  /* i once thought that vms was a good os */
  int ret, chunk, left;
  ret  = 0;
  left = len;
  while(ret < len)
  {
    chunk = read(fhdl,cptr+ret,left); 
    if(chunk <= 0) return chunk;
    ret  += chunk;
    left -= chunk;
  }
  return ret;
#else
  return read(fhdl,cptr,len);
#endif
}

int pvopenBinary(const char *filename)
{
#ifdef PVWIN32
  int fhdl = ::open(filename, _O_RDONLY | _O_BINARY);
#endif
#ifdef PVUNIX
  int fhdl = ::open(filename, O_RDONLY);
#endif
#ifdef __VMS
  int fhdl = ::open(filename, O_RDONLY, 0, "shr=get", "shr=put", "shr=upd");
#endif
  if(fhdl == -1) printf("pvopenBinary(%s) failed\n",filename);
  return fhdl;
}

unsigned short swap_short(short i)
{
  unsigned short ret;
#ifdef SWAP_BYTES
  ret = 256*(i & 0x0ff) + (i/256);
#else
  ret = i;
#endif  
  return ret;
}

unsigned int swap_long(short i)
{
  unsigned int ret;
#ifdef SWAP_BYTES
  unsigned char buf[4];
  buf[0] =  i                & 0x0ff;
  buf[1] = (i/256)           & 0x0ff;
  buf[2] = (i/(256*256))     & 0x0ff;
  buf[3] = (i/(256*256*256)) & 0x0ff;
  ret = buf[0]*256*256*256 + buf[1]*256*256 + buf[2]*256 + buf[3]; 
#else
  ret = i;
#endif  
  return ret;
}

PVB_IMAGE *pvbImageRead(const char *filename)
{
  PVB_IMAGE *image;
  myBITMAPFILEHEADER fileheader;
  myBITMAPINFOHEADER infoheader;
  int i, ioffset, ret; // isize
  LUT *rgb;
  char *cptr;

  /* open file */
  int fhdl = pvopenBinary(filename);
  if(fhdl == -1) return NULL;

  /* read file header */
  ret = pvmyread(fhdl,(char *) &fileheader,sizeof(myBITMAPFILEHEADER));
  if(ret < 0)                                               return NULL;
  if(memcmp(&fileheader.bfType,"BM",2) != 0) { close(fhdl); return NULL; }
  //isize   = 256*256*swap_short(fileheader.bfSize[1])    + swap_short(fileheader.bfSize[0]);
  ioffset = 256*256*swap_short(fileheader.bfOffBits[1]) + swap_short(fileheader.bfOffBits[0]);

  /* read infoheader */
  lseek(fhdl, 14, SEEK_SET);
  ret = pvmyread(fhdl,(char *) &infoheader,sizeof(myBITMAPINFOHEADER));
  if(ret < 0)                                { close(fhdl); return NULL; }

  if(infoheader.biBitCount != 8)             { close(fhdl); return NULL; }

  /* construct image object */
  image = (PVB_IMAGE *) malloc(sizeof(PVB_IMAGE));
  if(image == NULL)                          { close(fhdl); return NULL; }
  image->w     = swap_long(infoheader.biWidth);
  image->h     = swap_long(infoheader.biHeight);
  image->bpp   = swap_short(infoheader.biBitCount);
  image->nLut  = swap_long(infoheader.biClrUsed);
  image->lut   = NULL;
  image->image = NULL; 

  /* read LUT */
  if(image->nLut > 0)
  {
    image->lut = (LUT *) malloc(sizeof(LUT)*image->nLut);
    if(image->lut == NULL) { pvbImageFree(image); close(fhdl); return NULL; }
    rgb = image->lut;
    for(i=0; i<image->nLut; i++)
    {
      ret = pvmyread(fhdl,(char *) rgb,sizeof(LUT));
      if(ret < 0)          { pvbImageFree(image); close(fhdl); return NULL; }
      rgb++;
    }
  }

  /* read the image */
  lseek(fhdl, ioffset, SEEK_SET);
  image->image = malloc(image->w * image->h);
  if(image->image == NULL) { pvbImageFree(image); close(fhdl); return NULL; }
  cptr = (char *) image->image + (image->w * image->h);
  for(i=0; i<image->h; i++)
  {
    cptr -= image->w;
    ret = pvmyread(fhdl,cptr,image->w);
    if(ret != image->w) 
       printf("read %d bytes should read %d bytes\n",ret,image->w);
    if(ret < 0)            { pvbImageFree(image); close(fhdl); return NULL; }
  }

  close(fhdl);
  return image;
}

void pvbImageFree(PVB_IMAGE *pvbImage)
{
  if(pvbImage == NULL) return;
  if(pvbImage->lut   != NULL) free(pvbImage->lut);
  if(pvbImage->image != NULL) free(pvbImage->image);
  free(pvbImage);
}

