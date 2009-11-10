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
#include "rlserial.h"

#ifdef RLUNIX
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#endif

#ifdef __VMS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <starlet.h>
#include <descrip.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <iodef.h>
typedef struct
{
  short    iostat;
	unsigned short msg_len;
	int      reader_pid;
}IOSB;
#endif

#ifdef RLWIN32
#include <windows.h>
#include <stdio.h>
#endif

#ifdef RM3
#include <stdio.h>
#include <stdlib.h>
#include <rmcomp.h>
#include <rmapi.h>
#include <rio.h>
#include <drvspec.h>
#include <rm3cav.h>
//#include <clean.h>
#include <rcinc.h>
#define RTS_TIME_WAIT 0x008     /* Verzoerungszeit fuer RTS - Signal      */
#endif

#include "rlthread.h"

/*
static void sighandler(int sig)
{
  if(sig == SIGINT)
  {
    closeDevice();
    closeDatabase();
  }
}
*/

rlSerial::rlSerial()
{
  ttysavefd = -1;
  ttystate  = RESET;
  fd        = -1;
  trace     = 0;
}

rlSerial::~rlSerial()
{
  closeDevice();
}

void rlSerial::setTrace(int on)
{
  if(on == 1) trace = 1;
  else        trace = 0;
}

int rlSerial::openDevice(const char *devicename, int speed, int block, int rtscts, int bits, int stopbits, int parity)
{
#ifdef RLUNIX
  struct termios buf;

  if(fd != -1) return -1;
  fd = open(devicename, O_RDWR | O_NOCTTY | O_NDELAY);
  if(fd < 0) { return -1; }

  //signal(SIGINT, sighandler);

  if(tcgetattr(fd, &save_termios) < 0) { return -1; }
  buf = save_termios;
  buf.c_cflag = speed | CLOCAL | CREAD;
  if(rtscts   == 1)  buf.c_cflag |= CRTSCTS;
  if(bits     == 7)  buf.c_cflag |= CS7;
  else               buf.c_cflag |= CS8;
  if(stopbits == 2)  buf.c_cflag |= CSTOPB;
  if(parity == rlSerial::ODD)  buf.c_cflag |= (PARENB | PARODD);
  if(parity == rlSerial::EVEN) buf.c_cflag |= PARENB;
  buf.c_lflag = IEXTEN; //ICANON;
  buf.c_oflag     = OPOST;
  buf.c_cc[VMIN]  = 1;
  buf.c_cc[VTIME] = 0;
#ifndef PVMAC
  buf.c_line      = 0;
#endif
  buf.c_iflag     = IGNBRK | IGNPAR | IXANY;
  if(tcsetattr(fd, TCSAFLUSH, &buf) < 0) { return -1; }
  //if(tcsetattr(fd, TCSANOW, &buf) < 0) { return -1; }
  ttystate = RAW;
  ttysavefd = fd;
  if(block == 1) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);
  tcflush(fd,TCIOFLUSH);
#endif

#ifdef __VMS
  // Please set com parameters at DCL level
  struct dsc$descriptor_s dsc;
  int status;

  dsc.dsc$w_length  = strlen(devicename);
  dsc.dsc$a_pointer = (char *) devicename;
  dsc.dsc$b_class   = DSC$K_CLASS_S;
  dsc.dsc$b_dtype   = DSC$K_DTYPE_T;
  status = SYS$ASSIGN(&dsc,&vms_channel,0,0);
  if(status != SS$_NORMAL) return -1;
#endif

#ifdef RLWIN32
  DWORD ccsize;
  COMMCONFIG cc;
  int baudrate,ret;
  char devname[100];

  if(strlen(devicename) > 80) return -1;
  sprintf(devname,"\\\\.\\%s",devicename);        // Aenderung: allow more than 4 COM ports
  hdl = CreateFile(
                   devname,                       // devicename, // pointer to name of the file
                   GENERIC_READ | GENERIC_WRITE,  // access (read-write) mode
                   0,                             // share mode
                   0,                             // pointer to security attributes
                   OPEN_EXISTING,                 // how to create
                   0,                             // not overlapped I/O
                   0                              // handle to file with attributes to copy
                  );
  if(hdl == INVALID_HANDLE_VALUE)
  {
    printf("CreateFile(%s) failed\n",devicename);
    return -1;
  }

  baudrate = CBR_9600;
  if(speed == B50     ) baudrate = 50;
  if(speed == B75     ) baudrate = 75;
  if(speed == B110    ) baudrate = CBR_110;
  if(speed == B134    ) baudrate = 134;
  if(speed == B150    ) baudrate = 150;
  if(speed == B200    ) baudrate = 200;
  if(speed == B300    ) baudrate = CBR_300;
  if(speed == B600    ) baudrate = CBR_600;
  if(speed == B1200   ) baudrate = CBR_1200;
  if(speed == B1800   ) baudrate = 1800;
  if(speed == B2400   ) baudrate = CBR_2400;
  if(speed == B4800   ) baudrate = CBR_4800;
  if(speed == B9600   ) baudrate = CBR_9600;
  if(speed == B19200  ) baudrate = CBR_19200;
  if(speed == B38400  ) baudrate = CBR_38400;
  if(speed == B57600  ) baudrate = CBR_57600;
  if(speed == B115200 ) baudrate = CBR_115200;
  if(speed == B230400 ) baudrate = 230400;
  if(speed == B460800 ) baudrate = 460800;
  if(speed == B500000 ) baudrate = 500000;
  if(speed == B576000 ) baudrate = 576000;
  if(speed == B921600 ) baudrate = 921600;
  if(speed == B1000000) baudrate = 1000000;
  if(speed == B1152000) baudrate = 1152000;
  if(speed == B1500000) baudrate = 1500000;
  if(speed == B2000000) baudrate = 2000000;
  if(speed == B2500000) baudrate = 2500000;
  if(speed == B3000000) baudrate = 3000000;
  if(speed == B3500000) baudrate = 3500000;
  if(speed == B4000000) baudrate = 4000000;

  GetCommConfig(hdl,&cc,&ccsize);
  //cc.dwSize            = sizeof(cc);  // size of structure
  //cc.wVersion          = 1;           // version of structure
  //cc.wReserved         = 0;           // reserved
  //  DCB   dcb;                      // device-control block
  cc.dcb.DCBlength     = sizeof(DCB); // sizeof(DCB)
  cc.dcb.BaudRate      = baudrate;    // current baud rate
  cc.dcb.fBinary       = 1;           // binary mode, no EOF check
  cc.dcb.fParity       = 1;           // enable parity checking
  cc.dcb.fOutxCtsFlow  = 0;           // CTS output flow control
  if(rtscts == 1) cc.dcb.fOutxCtsFlow = 1;
  cc.dcb.fOutxDsrFlow  = 0;           // DSR output flow control
  cc.dcb.fDtrControl   = DTR_CONTROL_DISABLE;  // DTR flow control type
  cc.dcb.fDsrSensitivity   = 0;       // DSR sensitivity
  cc.dcb.fTXContinueOnXoff = 1;       // XOFF continues Tx
  //cc.dcb.fOutX         = 0;           // XON/XOFF out flow control
  //cc.dcb.fInX          = 0;           // XON/XOFF in flow control
  //cc.dcb.fErrorChar    = 0;           // enable error replacement
  cc.dcb.fNull         = 0;           // enable null stripping
  cc.dcb.fRtsControl   = RTS_CONTROL_DISABLE;
  if(rtscts == 1)  cc.dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;  // RTS flow control
  cc.dcb.fAbortOnError = 0;           // abort reads/writes on error
  //cc.dcb.fDummy2       = 0;           // reserved
  //cc.dcb.wReserved     = 0;           // not currently used
  //cc.dcb.XonLim        = 0;           // transmit XON threshold
  //cc.dcb.XoffLim       = 0;           // transmit XOFF threshold
  cc.dcb.ByteSize      = bits;        // number of bits/byte, 4-8
  cc.dcb.Parity        = 0;           // 0-4=no,odd,even,mark,space
  if(parity == rlSerial::ODD)    cc.dcb.Parity = 1;
  if(parity == rlSerial::EVEN)   cc.dcb.Parity = 2;
  cc.dcb.StopBits      = ONESTOPBIT;  // 0,1,2 = 1, 1.5, 2
  if(stopbits==2) cc.dcb.StopBits = TWOSTOPBITS;
  //cc.dcb.XonChar       = 0;           // Tx and Rx XON character
  //cc.dcb.XoffChar      = 0;           // Tx and Rx XOFF character
  //cc.dcb.ErrorChar     = 0;           // error replacement character
  //cc.dcb.EofChar       = 0;           // end of input character
  //cc.dcb.EvtChar       = 0;           // received event character
  //cc.dcb.wReserved1    = 0;           // reserved; do not use
  cc.dwProviderSubType = PST_RS232;   // type of provider-specific data
  //cc.dwProviderOffset  = 0;           // offset of provider-specific data
  //cc.dwProviderSize    = 0;           // size of provider-specific data
  //cc.wcProviderData[0] = 0;           // provider-specific data

  ret = SetCommConfig(hdl,&cc,sizeof(cc));
  if(ret == 0)
  {
    printf("SetCommConfig ret=%d devicename=%s LastError=%d\n",ret,devicename,GetLastError());
    return -1;
  }
#endif

#ifdef RM3
  RmEntryStruct    CatEntry;        /* Struktur der Deiviceinformationen              */
  int              iStatus;         /* Rckgabewert                                    */
  RmIOStatusStruct DrvSts;          /* Struktur der Rckgabewerte fr RmIO - Funktion   */
  RmBytParmStruct  PBlock;          /* Parameterstruktur fr RmIO - Funktion           */
  static UCD_BYT_PORT Ucd_byt_drv;  /* Struktur zum Setzen der UCD - Werte            */
  ushort           uTimeBd;         /* Timing - Wert der �ertragungsgeschwindigkeit   */
  uint             uMode;           /* Portsteuerungsparameter                        */
  unsigned char    cByte;           /* Byte - Parameter                               */
                                    /* Timing = 748800 / Baudrate;                    */
                                    /**************************************************/
  char byt_com[32];
                                    
  /* COM1=0x3F8 COM2=0x2F8 - Port Adresse           */
  if     (strcmp(devicename,"COM1") == 0)
  {
    strcpy(byt_com,"BYT_COM1");
    com = 0x3f8;
  }
  else if(strcmp(devicename,"COM2") == 0)
  {
    strcpy(byt_com,"BYT_COM2");
    com = 0x2f8;
  }
  else 
  {
    printf("Error: devicename=%s unknown\n",devicename);
    return -1;
  }  
  //printf("Open COM port - inside\n");

  /*
  * Device und Unit - Id auslesen
  */
  if( RmGetEntry( RM_WAIT, byt_com, &CatEntry ) != RM_OK ) /* RM_CONTINUE */
  {
    printf( "Error: %s device not found\n", byt_com);
    return -1;
  }

  device = (int) ((ushort) CatEntry.ide);
  unit   = (int) CatEntry.id;

  /*
  * Ger� reservieren
  */
  if( RmIO( BYT_RESERVE, (unsigned)(device), (unsigned)(unit), 0u, 0u, &DrvSts, &PBlock ) < 0 )
  {
    printf( "Error: Unable to reserve %s device\n", byt_com);
    return -1;
  }

  /*
  * Baudrate ausrechnen
  */
  baudrate = 9600;
  if(speed == B50     ) baudrate = 50;
  if(speed == B75     ) baudrate = 75;
  if(speed == B110    ) baudrate = 110;
  if(speed == B134    ) baudrate = 134;
  if(speed == B150    ) baudrate = 150;
  if(speed == B200    ) baudrate = 200;
  if(speed == B300    ) baudrate = 300;
  if(speed == B600    ) baudrate = 600;
  if(speed == B1200   ) baudrate = 1200;
  if(speed == B1800   ) baudrate = 1800;
  if(speed == B2400   ) baudrate = 2400;
  if(speed == B4800   ) baudrate = 4800;
  if(speed == B9600   ) baudrate = 9600;
  if(speed == B19200  ) baudrate = 19200;
  if(speed == B38400  ) baudrate = 38400;
  if(speed == B57600  ) baudrate = 57600;
  if(speed == B115200 ) baudrate = 115200;
  if(speed == B230400 ) baudrate = 230400;
  if(speed == B460800 ) baudrate = 460800;
  if(speed == B500000 ) baudrate = 500000;
  if(speed == B576000 ) baudrate = 576000;
  if(speed == B921600 ) baudrate = 921600;
  if(speed == B1000000) baudrate = 1000000;
  if(speed == B1152000) baudrate = 1152000;
  if(speed == B1500000) baudrate = 1500000;
  if(speed == B2000000) baudrate = 2000000;
  if(speed == B2500000) baudrate = 2500000;
  if(speed == B3000000) baudrate = 3000000;
  if(speed == B3500000) baudrate = 3500000;
  if(speed == B4000000) baudrate = 4000000;
  uTimeBd = 748800 / baudrate;

  /*
  * Portsteuerungsparameter setzen
  */
  uMode = 0x1000 | DATA_8 | STOP_1 | NOPARITY;

  /*
  * UCD des seriellen Ports auslesen
  */
  PBlock.string = 0;
  PBlock.strlen = 0;
  PBlock.buffer = (char *)&Ucd_byt_drv;
  PBlock.timlen = sizeof(UCD_BYT_PORT);
  PBlock.status = 0;

  iStatus = RmIO( BYT_CREATE_NEW, (unsigned)(device), (unsigned)(unit), 0u, 0u, &DrvSts, &PBlock );

  /*
   * Modus �dern
   */
  Ucd_byt_drv.mobyte[5] |= (ushort) (uMode & 0xFFu);

  /*
   * Timeout setzen
   */
  Ucd_byt_drv.header.timout = timeout;

  /*
   * Werte zuweisen
   */
  PBlock.string = (char*) &Ucd_byt_drv;
  PBlock.strlen = sizeof(UCD_BYT_PORT);
  PBlock.buffer = 0;
  PBlock.timlen = 0;
  PBlock.status = 0;

  iStatus = RmIO( BYT_CREATE_NEW, (unsigned)(device), (unsigned)(unit), 0u, 0u, &DrvSts, &PBlock );

  /*
   * Register 0 und 1 zum Schreiben freigeben
   */
  cByte = inbyte( com + 0x03 );
  outbyte( com + 0x03, (unsigned char)(cByte | 0x80) );

  /*
   * Baudrate setzen
   */
  outbyte( com + 0x00, (ushort) LOW  (uTimeBd) );
  outbyte( com + 0x01, (ushort) HIGH (uTimeBd) );

  /*
   * Register 0 und 1 sperren
   */
  outbyte( com + 0x03, cByte );

  if( iStatus ) printf( "BYT_CREATE_NEW (set ucb): Error status = %X\n", iStatus );
#endif
  
  return 0;
}

int rlSerial::readChar()
{
#ifdef RLUNIX
  int  ret;
  unsigned char buf[2];

  if(fd == -1) return -1;
  ret = read(fd,buf,1);
  if(ret == 1) return buf[0];
  if(ret == 0) return -2;
  return -1;
#endif

#ifdef __VMS
  unsigned char buf[2];

  if(readBlock(buf, 1) < 0) return -1;
  return buf[0];
#endif

#ifdef RLWIN32
  BOOL ret;
  unsigned char buf[2];
  unsigned long len;

  ret = ReadFile(
                 hdl,  // handle of file to read
                 buf,  // pointer to buffer that receives data
                 1,    // number of bytes to read
                 &len, // pointer to number of bytes read
                 NULL  // pointer to structure for data
                );
  if(len > 0)
  {
    if(trace == 1) printf("readChar %d\n",(int) buf[0]);
    return buf[0];
  }
  return -1;
#endif

#ifdef RM3
  int ret;
  unsigned char buf[2];
  
  ret = readBlock(buf,1);
  if(ret < 0) return ret;
  return buf[0];
#endif
}

int rlSerial::writeChar(unsigned char uchar)
{
#ifdef RLUNIX
  int ret;
  if(fd == -1) return -1;
  if(trace == 1) printf("writeChar %d\n",(int)uchar);
  ret = write(fd,&uchar,1);
  if(ret < 0) return -1;
  //tcflush(fd, TCIOFLUSH);
  return ret;
#endif

#ifdef __VMS
  int  status;
  IOSB iosb;

  status = SYS$QIOW(0,vms_channel,IO$_WRITEVBLK | IO$M_CANCTRLO | IO$M_NOFORMAT,
                    &iosb,0,0,&uchar,1,0,0,0,0);
  if(status != SS$_NORMAL) return -1;
  return 1;
#endif

#ifdef RLWIN32
  BOOL ret;
  unsigned long len;

  if(trace == 1) printf("writeChar %d\n",(int)uchar);
  ret = WriteFile(
                  hdl,    // handle to file to write to
                  &uchar, // pointer to data to write to file
                  1,      // number of bytes to write
                  &len,   // pointer to number of bytes written
                  NULL    // pointer to structure for overlapped I/O
                 );

  if(ret) return (int) len;
  return -1;
#endif

#ifdef RM3
  return writeBlock(&uchar, 1);
#endif
}

int rlSerial::readBlock(unsigned char *buf, int len, int timeout)
{
#ifdef RLUNIX
  int c, retlen;

  retlen = 0;
  for(int i=0; i<len; i++)
  {
    if(timeout >= 0)
    {
      if(select(timeout) == 0) break; // timeout
    }
    c = readChar();
    if(c < 0) return c;
    buf[i] = (unsigned char) c;
    retlen = i+1;
  }
  if(retlen <= 0) return -1;
  return retlen;
#endif

#ifdef __VMS
  int   status;
  int   _timeout = 1; // second
  short iosb[4];

  status = SYS$QIOW(0,vms_channel,
                    IO$_READVBLK | IO$M_NOFILTR | IO$M_NOECHO | IO$M_TIMED,
                    iosb,0,0,buf,len,_timeout,0,0,0);
  if(status != SS$_NORMAL) return -1;
  len=iosb[1];
  if(iosb[2] != 0)
  {
    len++;
    buf[len] = iosb[2];
  }
  return len;
#endif

#ifdef RLWIN32
  BOOL ret;
  unsigned long retlen;

  if(timeout >= 0) select(timeout);
  ret = ReadFile(
                 hdl,     // handle of file to read
                 buf,     // pointer to buffer that receives data
                 len,     // number of bytes to read
                 &retlen, // pointer to number of bytes read
                 NULL     // pointer to structure for data
                );
  if(retlen > 0)
  {
    if(trace == 1) printf("readBlock retlen=%d\n",retlen);
    return (int) retlen;
  }
  return -1;
#endif

#ifdef RM3
  RmBytParmStruct  PBlock;    /* Parameterstruktur fr RmIO - Funktion          */
  RmIOStatusStruct DrvSts;    /* Struktur der Rckgabewerte fr RmIO - Funktion */
  int              iStatus;   /* Rckgabewert                                   */
  int              i;         /* Schleifenz�ler                                */
                              /**************************************************/

  /*
   * Schreibparameter setzen
   */
  PBlock.string = 0;
  PBlock.strlen = 0;
  PBlock.buffer = (char*)buf;
  PBlock.timlen = len;
  PBlock.status = 0;

  /*
   * Lesevorgang einleiten
   */
  iStatus = RmIO( BYT_POLL_XBUF_WAIT, (unsigned)device, (unsigned)unit, 0u, 0u, &DrvSts, &PBlock );

  if( iStatus ) printf( "BYT_POLL_XBUF_WAIT (set ucb): Error status = %X\n", iStatus );

  if( !iStatus ) return len;
  return -1;
#endif
}

int rlSerial::writeBlock(const unsigned char *buf, int len)
{
#ifdef RLUNIX
  int ret;

  if(fd == -1) return -1;
  if(trace == 1)
  {
    printf("writeBlock:");
    for(int i=0; i<len; i++) printf(" %d",(int) buf[i]);
    printf("\n");
  }
  ret = write(fd,buf,len);
  //tcflush(fd, TCIOFLUSH);
  return ret;
#endif

#ifdef __VMS
  int  status;
  IOSB iosb;


  status = SYS$QIOW(0,vms_channel,IO$_WRITEVBLK | IO$M_CANCTRLO | IO$M_NOFORMAT,
                    &iosb,0,0,buf,len,0,0,0,0);
  if(status != SS$_NORMAL) return -1;
  return len;
#endif

#ifdef RLWIN32
  BOOL ret;
  unsigned long retlen;

  if(trace == 1)
  {
    printf("writeBlock:");
    for(int i=0; i<len; i++) printf(" %d",(int) buf[i]);
    printf("\n");
  }
  retlen = len;
  ret = WriteFile(
                  hdl,     // handle to file to write to
                  buf,     // pointer to data to write to file
                  len,     // number of bytes to write
                  &retlen, // pointer to number of bytes written
                  NULL     // pointer to structure for overlapped I/O
                 );

  if(ret) return (int) retlen;
  return -1;
#endif

#ifdef RM3
  RmBytParmStruct  PBlock;    /* Parameterstruktur fr RmIO - Funktion          */
  RmIOStatusStruct DrvSts;    /* Struktur der Rckgabewerte fr RmIO - Funktion  */
  unsigned char    cByte;     /* Rckgabewert von ibyte - Funktion              */
  int              iStatus;   /* Rckgabewert                                   */
  int              i;         /* Schleifenz�ler                                */
                              /**************************************************/

  /*
   * Schreibparameter setzen
   */
  PBlock.string = (char*)buf;
  PBlock.strlen = len;
  PBlock.buffer = 0;
  PBlock.timlen = 0;
  PBlock.status = 0;

  /******************************************************
   *                                                    *
   * Toggle mode wird emuliert indem an dieser Stelle   *
   * RTS-Signal gesetzt und sp�er wieder gel�cht wird   *
   *                                                    *
   ******************************************************/

  cByte = inbyte( com + 0x04u );
  outbyte( com + 0x04, (unsigned char)(cByte | 0x02u) );

  /*
   * Schreibvorgang einleiten
   */
  iStatus = RmIO( BYT_WRITE_WAIT, (unsigned)device, (unsigned)unit, 0u, 0u, &DrvSts, &PBlock );

  /******************************************************************
   *                                                                *
   * 8ms warten.Bei der Aenderung der Uebertragungsgeschwindigkeit, *
   * sollte dieser Wert angepasst werden. Hier fuer 9600            *
   *                                                                *
   ******************************************************************/
  RmPauseTask((RTS_TIME_WAIT*9600)/baudrate);

  /*
   * RTS-Signal l�chen
   */
  outbyte( com + 0x04, (unsigned char)(cByte & 0xFDu) );

  if( iStatus ) printf( "BYT_WRITE_WAIT (write block): Error status = %X\n", iStatus );

  if( !iStatus ) return len;
  return -1;
#endif
}

int rlSerial::readLine(unsigned char *buf, int maxlen, int timeout)
{
  int i,c,ret;

  if(maxlen <= 1) return -1;
  ret = 0;
  buf[maxlen-1] = '\0';
  for(i=0; i<maxlen-2; i++)
  {
    ret = i;
    if(timeout > 0)
    {
      int t = select(timeout);
      if(t == 0) return -1;
    }
    c = readChar();
    if(c < 0)
    {
      buf[i] = '\0';
      ret = c;
      break;
    }
    buf[i] = (unsigned char) c;
    if(c < ' ' && c != '\t')
    {
      buf[i+1] = '\0';
      break;
    }
  }
  return ret;
}

int rlSerial::select(int timeout)
{
#ifdef RLUNIX
  struct timeval timout;
  fd_set wset,rset,eset;
  int    ret,maxfdp1;

  if(timeout <= 0) return 1;
  /* setup sockets to read */
  maxfdp1 = fd+1;
  FD_ZERO(&rset);
  FD_SET (fd,&rset);
  FD_ZERO(&wset);
  FD_ZERO(&eset);
  timout.tv_sec  =  timeout / 1000;
  timout.tv_usec = (timeout % 1000) * 1000;

  ret = ::select(maxfdp1,&rset,&wset,&eset,&timout);
  if(ret == 0) return 0; /* timeout */
  return 1;
#endif

#ifdef __VMS
  return 1;
#endif

#ifdef RLWIN32
  COMMTIMEOUTS ctimeout;

  ctimeout.ReadIntervalTimeout          = timeout; 
  ctimeout.ReadTotalTimeoutMultiplier   = 1; 
  ctimeout.ReadTotalTimeoutConstant     = timeout; 
  ctimeout.WriteTotalTimeoutMultiplier  = 1; 
  ctimeout.WriteTotalTimeoutConstant    = timeout; 

  SetCommTimeouts(hdl, &ctimeout);
  return 1;
#endif

#ifdef RM3
  return 1;
#endif
}

int rlSerial::closeDevice()
{
#ifdef RLUNIX
  if(fd == -1) return -1;
  //if(::tcsetattr(fd,TCSAFLUSH,&save_termios) < 0) return -1;
  if(::tcsetattr(fd,TCSANOW,&save_termios) < 0) return -1;
  ::close(fd);
  ttystate = RESET;
  fd = -1;
  return 0;
#endif

#ifdef __VMS
  sys$dassgn(vms_channel);
  return 0;
#endif

#ifdef RLWIN32
  CloseHandle(hdl);
  return 0;
#endif

#ifdef RM3
  RmBytParmStruct  PBlock;    /* Parameterstruktur fr RmIO - Funktion          */
  RmIOStatusStruct DrvSts;    /* Struktur der Rckgabewerte fr RmIO - Funktion */
                              /**************************************************/
  if( RmIO( BYT_RELEASE, (unsigned)(device), (unsigned)(unit), 0u, 0u, &DrvSts, &PBlock ) < 0 )
  {
    printf( "Error: Unable to release device. device: %i, unit: %i\n",device, unit );
    return -1;
  }
  return 0;
#endif
}
