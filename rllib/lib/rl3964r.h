/***************************************************************************
                        rl3964r.h  -  description
                             -------------------
    begin                : Wed Jan 14 2004
    copyright            : (C) 2004 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_3964R_H_
#define _RL_3964R_H_

#include "rldefine.h"
#include "rlserial.h"
#include "rlthread.h"

/*! <pre>
This class implements the siemens 3964R dust protocol.
The read messages must be handled within the callback routine.
</pre> */
class rl3964R
{
  public:
    enum priorityEnum
    {
      highPriority = 0,
      lowPriority
    };

    rl3964R(int _priority = highPriority);
    virtual ~rl3964R();
    int open(const char *devicename, int _baudrate = B9600);
    int close();
    int setReadCallback( void (*_readCallback)(const unsigned char *buf, int len));
    int write(const unsigned char *buf, int len);

    int send();
    int receive();
    rlThread receiver;
    rlSerial tty;
    int state;
    int priority;
    int run;
    int debug;
    int dprintf(const char *format, ...);
  private:
    void (*readCallback)(const unsigned char *buf, int len);
    unsigned char tel_send[512];
    unsigned char tel_receive[512];
    int tel_send_length;
    int tel_receive_length;
    int isOpen;
    int send_result;
};

#endif

