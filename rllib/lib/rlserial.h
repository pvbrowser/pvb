/***************************************************************************
                          rlserial.cpp  -  description
                             -------------------
    begin                : Sat Dec 21 2002
    copyright            : (C) 2002 by Rainer Lehrig
    email                : lehrig@t-online.de

    RMOS implementation:
    Copyright            : (C) 2004 Zertrox GbR
    Written by           : Alexander Feller
    Email                : feller@zertrox.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
/*! <pre>
With this class you can communicate over a serial line.
</pre> */
#ifndef _RL_SERIAL_H_
#define _RL_SERIAL_H_

#include "rlthread.h"

#ifdef RLUNIX
#include <termios.h>
#endif

#ifndef B0
// speed will be defined by termios.h, this is just for documentation
#define  B0       0000000  /* hang up */
#define  B50      0000001
#define  B75      0000002
#define  B110     0000003
#define  B134     0000004
#define  B150     0000005
#define  B200     0000006
#define  B300     0000007
#define  B600     0000010
#define  B1200    0000011
#define  B1800    0000012
#define  B2400    0000013
#define  B4800    0000014
#define  B9600    0000015
#define  B19200   0000016
#define  B38400   0000017
#define  B57600   0010001
#define  B115200  0010002
#define  B230400  0010003
#define  B460800  0010004
#define  B500000  0010005
#define  B576000  0010006
#define  B921600  0010007
#define  B1000000 0010010
#define  B1152000 0010011
#define  B1500000 0010012
#define  B2000000 0010013
#define  B2500000 0010014
#define  B3000000 0010015
#define  B3500000 0010016
#define  B4000000 0010017
#endif

/*! <pre>
With this class you can communicate over a serial line.
</pre> */
class rlSerial
{
  public:
    enum Parity
    {
      NONE = 0,
      ODD ,
      EVEN
    };

    rlSerial();
    virtual ~rlSerial();
    /**
    <pre>
    On OpenVMS please set the tt parameters at DCL level E.g.:
    $ set term /perm/pasthru  -
               /eightbit      -
               /nottsync      -
               /nohostsync    -
               /noreadsync    -
               /type          -
               /noline        -
               /altype        -
               /parity=even   -
               /speed=9600 tta1:

    devicename := /dev/ttyX (unix) | COMx (Windows) | ttaX: (OpenVMS) 
    speed      := B50 ... B4000000                                    
    block      := 0 | 1 (bool)                                        
    rtscts     := 0 | 1 (bool)                                        
    bits       := 7 | 8 (unix) | 4-8 (Windows)                        
    stopbits   := 1 | 2                                               
    parity     := rlSerial::NONE | rlSerial::ODD | rlSerial::EVEN     
    return == 0  success, return < 0 error
    </pre>           
    */
    int openDevice(const char *devicename, int speed=B9600, int block=1, int rtscts=1, int bits=8, int stopbits=1, int parity=rlSerial::NONE);
    int select(int timeout=500);
    int readChar();
    int writeChar(unsigned char uchar);
    int readBlock(unsigned char *buf, int len, int timeout=-1);
    int writeBlock(const unsigned char *buf, int len);
    int readLine(unsigned char *buf, int maxlen, int timeout=1000);
    int closeDevice();
		//! on := 0 | 1 (bool)
    void setTrace(int on);
  private:
#ifdef RLUNIX
    struct termios save_termios;
#endif
#ifdef __VMS
    unsigned short int vms_channel;
#endif
#ifdef RLWIN32
    HANDLE hdl;
#endif
#ifdef RM3
    int device, uint, com, baudrate;
#endif
    enum { RESET, RAW, CBREAK } ttystate;
    int ttysavefd;
    int fd,trace;
};

#endif
