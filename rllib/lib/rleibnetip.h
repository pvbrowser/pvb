/***************************************************************************
                        rleibnetip.h  -  description
                             -------------------
    begin                : Wed Apr 04 2007
    copyright            : (C) 2007 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_EIB_NET_IP_H_
#define _RL_EIB_NET_IP_H_

#include "rldefine.h"
#include "rludpsocket.h"
#include "rlthread.h"
#include <stdio.h>

#define EIB_ON  0x81
#define EIB_OFF 0x80

class rlDataAcquisitionProvider;

/*! <pre>
class for EIBnet/IP access
</pre> */
class rlEIBnetIP : public rlUdpSocket
{
public:

  enum EIBnetIP
  {
    PORT              = 3671,
    SUCCESS           = 0,
    EIBERROR          = 0x0ffffffff,
    TIMEOUT           = -1,
    COULD_NOT_CONNECT = -2
  };

  typedef struct
  {
    unsigned char  headersize;
    unsigned char  version;
    unsigned short servicetype;
    unsigned short totalsize;
    unsigned char  data[128-6];
  }PDU;

  typedef struct
  {
    unsigned char  mc;          // 0x29
    unsigned char  addi1;       // 0x0
    unsigned char  ctrl1;       // 0xbc
    unsigned char  ctrl2;       // 0xe0
    unsigned short saddr;       // 0x1
    unsigned short daddr;       // 0x100
    unsigned char  apci_length; // 0x1
    unsigned char  apci;        // 0x0
    unsigned char  val[14];     // 0x80
  }EIB_TEL;

  // this may be used by the application programmer
  rlEIBnetIP(int num_signals = 1000, int debug = 0, rlDataAcquisitionProvider *provider = NULL);
  virtual ~rlEIBnetIP();

  int setServer(rlIpAdr *server);
  int setClient(rlIpAdr *client);
  int startReading();
  int stopReading();
  int value(const char *name);
  unsigned int valueUnsigned(const char *name);
  float valueFloat2(const char *name);
  float valueFloat4(const char *name);
  int setValue(const char *name, int val, int length=-1, int addi1=-1, int ctrl=-1, int apci=-1);
  int setValueUnsigned(const char *name, unsigned int val, int length=-1, int addi1=-1, int ctrl=-1, int apci=-1);
  int setValueFloat(const char *name, float val, int length=-1, int addi1=-1, int ctrl=-1, int apci=-1);
  int setText(const char *name, const char *text);
  int getText(const char *name, char *text, int maxlen);
  int setSourceAdr(const char *adr);
  int dump(FILE *fout);
  int setValuesFromCSV(const char *filename);
  int debug;
  int watch_eib;

  // the rest is for internal use only
  int connect();
  int disconnect();
  int isConnected();
#ifndef SWIG_SESSION
  int getDescription(PDU *pdu);
#endif
  int recv(void *buf, int maxlen);
  int storeBuffer(unsigned char *buf, int len);
  int sendDisconnectRequest();

  void     *mem;
  int      memsize;
  int      running;
  int      channelid;
  rlThread thread;
  rlIpAdr  *server;
  int      send_sequencecounter;
  int      tunnel_ack;
  rlDataAcquisitionProvider *provider;

private:
  int      printTelegram(EIB_TEL *tel);
  int      storeInProvider(EIB_TEL *tel);
  rlIpAdr  client;
  rlIpAdr  from;
  int      maxvalues;
  int      is_connected;
  short    saddr;
};

#endif
