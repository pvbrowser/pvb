/***************************************************************************
                          rlwebcam.cpp  -  description
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
#include "rlwebcam.h"

rlWebcam::rlWebcam()
{
  debug = 0;
  sock = NULL;
}

rlWebcam::~rlWebcam()
{
  if(sock != NULL) delete sock;
}

int rlWebcam::setUrl(const char *urlstr)
{
  if(urlstr == NULL) return -1;
  url.setText(urlstr);
  if(sock != NULL) delete sock;
  sock = NULL;
  if(getHost() == NULL || getPort() < 0) return -1;
  sock = new rlSocket(getHost(),getPort(),1);
  return 0;
}

int rlWebcam::disconnect()
{
  if(sock == NULL) return -1;
  if(sock->isConnected()) sock->disconnect();
  return 0;
}

const char * rlWebcam::getSnapshot(int timeout)
{
  if(sock == NULL)
  {
    printf("rlWebcam::ERROR sock==NULL\n");
    return NULL;
  }
  if(sock->isConnected()) sock->disconnect();
  const char *cptr = getFrame(timeout);
  sock->disconnect();
  return cptr;
}

const char * rlWebcam::getFrame(int timeout, int requestOnly)
{
  unsigned char c1,c2;
  unsigned char buffer[1024*1024];

  if(sock == NULL)
  {
    printf("rlWebcam::ERROR sock==NULL\n");
    return NULL;
  }
  if(sock->isConnected() == 0) 
  {
    sock->connect();
    sock->printf("GET /%s%c%c%c%c",getPath(),0x0d,0x0a,0x0d,0x0a);
  }  
  if(sock->isConnected() == 0)
  {
    printf("rlWebcam::ERROR sock->isConnected() == 0\n");
    return NULL;
  }

  // read http header
  char line[256];
  while(1)
  {
    if(sock->readStr(line,(int) sizeof(line)-1,timeout) < 1) return NULL;
    if(debug) printf("getFrame:: Header:%s",line);
    if(strncmp(line,"Content-Length:",15) == 0) break;    
  }
  
  int len = 0;
  sscanf(line,"Content-Length: %d", &len);
  if(debug) printf("getFrame:: len=%d\n",len);
  if(len > (int) (sizeof(buffer) - 1)) return NULL;

  if(requestOnly) return NULL;
  // search for startOfImage
  while(1)
  {
    if(sock->read(&c1,1,timeout) < 1) return NULL;
    if(debug) printf("%02x ", c1);
    if(c1 == 0x0ff)
    {
      if(sock->read(&c2,1,timeout) < 1) return NULL;
      if(debug) printf("%02x ", c2);
      if(c2 == 0x0d8)
      {
        if(debug) printf("\nrlWebcam::Found startOfImage\n");
        break;
      }
    }
  }

  // open output file
  if(filename.text() == NULL)
  {
    printf("rlWebcam::ERROR you forgot to set filename\n");
    return NULL;
  }
  FILE *fout = fopen(filename.text(),"w");
  if(fout == NULL)
  {
    printf("rlWebcam::ERROR could not write file %s\n", filename.text());
    return NULL;
  }
  //fputc(c1, fout);
  //fputc(c2, fout);

  int ind = 0;
  buffer[ind++] = c1;
  buffer[ind++] = c2;
  len = sock->read(&buffer[2],len-2,timeout);
  //len = recv(sock->s,&buffer[2],len-2,MSG_WAITALL);
  if(len < 1) return NULL;
  ind = len + 2;

  fwrite(buffer, ind, 1, fout);

  /*
  // read until endOfImage
  while(1)
  {
    //if(sock->read(&c1,1,timeout) < 1) return NULL;
    if(recv(sock->s,&c1,1,0) < 1) return NULL;
    fputc(c1, fout);
    if(debug) printf("%02x ", c1);
    if(c1 == 0x0ff)
    {
      //if(sock->read(&c2,1,timeout) < 1) return NULL;
      if(recv(sock->s,&c2,1,0) < 1) return NULL;
      fputc(c2, fout);
      if(debug) printf("%02x ", c2);
      if(c2 == 0x0d9)
      {
        if(debug) printf("\nrlWebcam::Found endOfImage\n");
        break;
      }
    }
  }
  */

  // close output file
  fclose(fout);
  return filename.text();
}

int rlWebcam::getFrameBuffer(unsigned char *buffer, int maxbuffer, int timeout)
{
  unsigned char c1,c2;

  if(sock == NULL)
  {
    printf("rlWebcam::ERROR sock==NULL\n");
    return -1;
  }
  if(sock->isConnected() == 0) 
  {
    sock->connect();
    sock->printf("GET /%s%c%c%c%c",getPath(),0x0d,0x0a,0x0d,0x0a);
  }  
  if(sock->isConnected() == 0)
  {
    printf("rlWebcam::ERROR sock->isConnected() == 0\n");
    return -2;
  }

  // read http header
  char line[256];
  while(1)
  {
    if(sock->readStr(line,(int) sizeof(line)-1,timeout) < 1) return -1;
    if(debug) printf("getFrameBuffer:: Header:%s",line);
    if(strncmp(line,"Content-Length:",15) == 0) break;    
  }
  
  int len = 0;
  sscanf(line,"Content-Length: %d", &len);
  if(debug) printf("getFrameBuffer:: len=%d\n",len);

  // search for startOfImage
  while(1)
  {
    if(sock->read(&c1,1,timeout) < 1) return -3;
    if(debug) printf("%02x ", c1);
    if(c1 == 0x0ff)
    {
      if(sock->read(&c2,1,timeout) < 1) return -4;
      if(debug) printf("%02x ", c2);
      if(c2 == 0x0d8)
      {
        if(debug) printf("\nrlWebcam::Found startOfImage\n");
        break;
      }
    }
  }

  int ind = 0;
  buffer[ind++] = c1;
  buffer[ind++] = c2;
  len = sock->read(&buffer[2],len-2,timeout);
  //len = recv(sock->s,&buffer[2],len-2,MSG_WAITALL);
  if(len < 1) return -5;
  ind = len + 2;

  /*
  // read until endOfImage
  while(1)
  {
    //if(sock->read(&c1,1,timeout) < 1) return -5;
    if(recv(sock->s,&c1,1,0) < 1) return -5;
    //fputc(c1, fout);
    buffer[ind++] = c1;
    if(ind >= maxbuffer) return -5;
    if(debug) printf("%02x ", c1);
    if(c1 == 0x0ff)
    {
      //if(sock->read(&c2,1,timeout) < 1) return -6;
      if(recv(sock->s,&c2,1,0) < 1) return -6;
      //fputc(c2, fout);
      buffer[ind++] = c2;
      if(ind >= maxbuffer) return -7;
      if(debug) printf("%02x ", c2);
      if(c2 == 0x0d9)
      {
        if(debug) printf("\nrlWebcam::Found endOfImage\n");
        break;
      }
    }
  }

  // close output file
  // fclose(fout);
  */

  return ind;
}

const char * rlWebcam::getUrl()
{
  return url.text();
}

const char * rlWebcam::getHost()
{
  if(url.startsWith("http://") == 0)
  {
    printf("rlWebcam::wrong url syntax in %s\n", url.text());
    printf("url syntax: http://host:port/path_to_webcam_cgi_script\n");
    return NULL;
  }
  char *cptr = url.text();
  cptr += 7;
  temp1.setText(cptr);
  cptr = temp1.strchr('/');
  if(cptr != NULL) *cptr = '\0';
  cptr = temp1.strchr(':');
  if(cptr != NULL) *cptr = '\0';
  if(debug) printf("rlWebcam:host=%s\n", temp1.text());
  return temp1.text();
}

int rlWebcam::getPort()
{
  int port = 80;
  if(url.startsWith("http://") == 0)
  {
    printf("rlWebcam::wrong url syntax in %s\n", url.text());
    printf("url syntax: http://host:port/path_to_webcam_cgi_script\n");
    return -1;
  }
  char *cptr = url.text();
  cptr += 7;
  temp2.setText(cptr);
  cptr = temp2.strchr('/');
  if(cptr != NULL) *cptr = '\0';
  cptr = temp2.strchr(':');
  if(cptr != NULL) sscanf(cptr,":%d", &port);
  if(debug) printf("rlWebcam:port=%d\n", port);
  return port;
}

const char * rlWebcam::getPath()
{
  if(url.startsWith("http://") == 0)
  {
    printf("rlWebcam::wrong url syntax in %s\n", url.text());
    printf("url syntax: http://host:port/path_to_webcam_cgi_script\n");
    return NULL;
  }
  char *cptr = url.text();
  cptr += 7;
  temp3.setText(cptr);
  cptr = temp3.strchr('/');
  if(cptr == NULL) return "";
  cptr++;
  if(debug) printf("rlWebcam:path=%s\n", cptr);
  return cptr;
}

