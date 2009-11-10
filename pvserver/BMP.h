/***************************************************************************
                          BMP.h  -  description
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
