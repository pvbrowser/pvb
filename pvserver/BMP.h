/***************************************************************************
                          BMP.h  -  description
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
typedef struct mytagBITMAPFILEHEADER 
{ // bmfh 
    unsigned short int bfType; 
    unsigned short int bfSize[2]; 
    unsigned short int bfReserved1; 
    unsigned short int bfReserved2; 
    unsigned short int bfOffBits[2]; 
} myBITMAPFILEHEADER; 

typedef struct mytagBITMAPINFOHEADER{ // bmih 
    unsigned int       biSize; 
    unsigned int       biWidth; 
    unsigned int       biHeight; 
    unsigned short int biPlanes; 
    unsigned short int biBitCount; 
    unsigned int       biCompression; 
    unsigned int       biSizeImage; 
    unsigned int       biXPelsPerMeter; 
    unsigned int       biYPelsPerMeter; 
    unsigned int       biClrUsed; 
    unsigned int       biClrImportant; 
} myBITMAPINFOHEADER; 
 
typedef struct mytagBITMAPCOREHEADER { // bmch 
    unsigned int       bcSize; 
    unsigned short int bcWidth; 
    unsigned short int bcHeight; 
    unsigned short int bcPlanes; 
    unsigned short int bcBitCount; 
} myBITMAPCOREHEADER; 
 
typedef struct mytagRGBTRIPLE { // rgbt 
    unsigned char rgbtBlue; 
    unsigned char rgbtGreen; 
    unsigned char rgbtRed; 
} myRGBTRIPLE; 
 
typedef struct mytagRGBQUAD { // rgbq 
    unsigned char rgbBlue; 
    unsigned char rgbGreen; 
    unsigned char rgbRed; 
    unsigned char rgbReserved; 
} myRGBQUAD; 
 
/* following BITMAPFILEHEADER */
typedef struct mytagBITMAPINFO {
    myBITMAPINFOHEADER    bmiHeader;
    myRGBQUAD             bmiColors[1];
} myBITMAPINFO;

/* following BITMAPFILEHEADER */
typedef struct my_BITMAPCOREINFO {    // bmci 
    myBITMAPCOREHEADER  bmciHeader; 
    myRGBTRIPLE         bmciColors[1]; 
} myBITMAPCOREINFO;
