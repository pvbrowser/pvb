/***************************************************************************
                          rlwebcam.h  -  description
                             -------------------
    begin                : Mo Aug 24 2009
    copyright            : (C) 2009 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_WEBCAM_H_
#define _RL_WEBCAM_H_

#include "rldefine.h"
#include "rlsocket.h"
#include "rlstring.h"

/*! <pre>
class for handling networked webcams over http:// that use motion jpeg

If you do not know under which URL your webcam provides the M-JPEG video stream
you may use tcpdump to figure it out.

Example:

tcpdump -X -i eth0 -t -q -s 0 "host 192.168.1.200 && port 80" | grep -A 10 GET

IP myhost.46727 > 192.168.1.200.http: tcp 97
 0x0000:  4500 0089 edb6 4000 4006 c891 c0a8 010e  E.....@.@.......
 0x0010:  c0a8 01c8 b687 0050 d99f 8b7d 0003 d5b2  .......P...}....
 0x0020:  5018 16d0 2460 0000 4745 5420 2f63 6769  P...$`..GET./cgi
 0x0030:  2d62 696e 2f53 7472 6561 6d3f 5669 6465  -bin/Stream?Vide
 0x0040:  6f20 4175 7468 6f72 697a 6174 696f 6e3a  o.Authorization:
 0x0050:  2042 6173 6963 2059 5752 7461 5734 3663  .Basic.YWRtaW46c
 0x0060:  4746 7a63 3364 7663 6d51 3d3f 7765 6263  GFzc3dvcmQ=?webc
 0x0070:  616d 5057 443d 526f 6f74 436f 6f6b 6965  amPWD=RootCookie
 0x0080:  3030 3030 300d 0a0d 0a                   00000....

Usage example for pvbrowser slot functions:

#include "rlwebcam.h"

typedef struct // (todo: define your data structure here)
{
  rlWebcam webcamBig;
}
DATA;

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  p->sleep = 20;
  p->force_null_event = 0;
  d->webcamBig.debug = 0;
  d->webcamBig.filename.printf("%swebcam.jpg", p->file_prefix);
  d->webcamBig.setUrl("http://192.168.1.200/cgi-bin/Stream?Video Authorization: Basic YWRtaW46cGFzc3dvcmQ=?webcamPWD=RootCookie00000");
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  if(const char *fname = d->webcamBig.getFrame()) // OR if(const char *fname = d->webcamBig.getSnapshot()) 
  {
    pvDownloadFileAs(p,fname,"webcam.jpg");
    pvSetImage(p,WebcamBig,"webcam.jpg"); // WebcamBig is a pvQImage object that accepts jpeg images
  }
  return 0;
}

</pre> */
class rlWebcam
{
public:
  rlWebcam();
  virtual ~rlWebcam();
  int setUrl(const char *url);
  int disconnect();
  const char *getSnapshot(int timeout=3000);
  const char *getFrame(int timeout=3000, int requestOnly=0);
  int         getFrameBuffer(unsigned char *buffer, int maxbuffer, int timeout=3000);
  const char *getUrl();
  const char *getHost();
  int getPort();
  const char *getPath();
  int debug;
  rlString filename;
  rlSocket *sock;

private:
  rlString url, temp1, temp2, temp3;
};
#endif
