/***************************************************************************
                        rleibnetip.cpp  -  description
                             -------------------
    begin                : WEd Apr 04 2007
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
#include "rleibnetip.h"
#include "rltime.h"
#include "rldataacquisitionprovider.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define EIB_HEADERSIZE        6
#define EIB_VERSION           0x10
#define EIB_ADRSIZE           8
//not sizeof(struct sockaddr_in)
#define EIB_CRICRDSIZE        4
#define TUNNEL_CONNECTION     4
#define REMLOG_CONNECTION     6
#define OBJSVR_CONNECTION     8
#define TUNNEL_LINKLAYER      2
#define E_NO_ERROR            0
#define E_NO_MORE_CONNECTIONS 0x24
#define E_SEQUENCE_NUMBER     0x04

#define L_Data_Req 0x11
#define L_Data_Con 0x2E
#define L_Data_Ind 0x29

enum ServiceType
{
  SEARCH_REQUEST            = 0x0201,
  SEARCH_RESPONSE           = 0x0202,
  DESCRIPTION_REQUEST       = 0x0203,
  DESCRIPTION_RESPONSE      = 0x0204,
  CONNECT_REQUEST           = 0x0205,
  CONNECT_RESPONSE          = 0x0206,
  CONNECTIONSTATE_REQUEST   = 0x0207,
  CONNECTIONSTATE_RESPONSE  = 0x0208,
  DISCONNECT_REQUEST        = 0x0209,
  DISCONNECT_RESPONSE       = 0x020A,
  TUNNELLING_REQUEST        = 0x0420,
  TUNNELLING_ACK            = 0x0421
};

static void *eib_reader(void *arg) // thread
{
  THREAD_PARAM *p = (THREAD_PARAM *) arg;
  rlEIBnetIP *eib = (rlEIBnetIP *) p->user;
  rlEIBnetIP::PDU pdu;
  rlTime now, last, diff;
  int ret, len;
  int recseq = 0;
  int expected_recseq = 0;
  unsigned char b[4];

  last.getLocalTime();
  while(eib->running)
  {
    if(eib->isConnected() == 0)
    {
      eib->connect();
      expected_recseq = 0;
      last.getLocalTime();
    }
    ret = eib->recv(&pdu, sizeof(pdu));
    now.getLocalTime();
    if(ret > 0)
    {
      switch (ntohs(pdu.servicetype))
      {
        case DISCONNECT_REQUEST:
          eib->disconnect();
          expected_recseq = 0;
          break;
        case CONNECTIONSTATE_RESPONSE:
          if(eib->debug) ::printf("eib_reader() CONNECTIONSTATE_RESPONSE\n");
          break;
        case TUNNELLING_REQUEST:
          if(eib->debug)
            ::printf("eib_reader() TUNNELING_REQUEST sequenzecounter=%d\n",recseq);
          recseq = pdu.data[2];                 // sequencecounter
          b[0]   = pdu.data[0];                 // remember 4 bytes of data
          b[1]   = pdu.data[1];
          b[2]   = pdu.data[2];
          b[3]   = pdu.data[3];
          len    = ntohs(pdu.totalsize) - 6;    // remember len (-headerlength)
          pdu.headersize  = EIB_HEADERSIZE;     // fill acknowledge
          pdu.version     = EIB_VERSION;
          pdu.servicetype = htons(TUNNELLING_ACK);
          pdu.totalsize   = htons(EIB_HEADERSIZE+4);
          pdu.data[0]     = 4;                  // structlength
          pdu.data[1]     = eib->channelid;     // channelid
          pdu.data[2]     = recseq;             // sequencecounter
          pdu.data[3]     = E_NO_ERROR;         // typespecific
          if(recseq != expected_recseq)
          {
            if(eib->debug)
              ::printf("eib_reader() recseq=%d expected_recseq=%d\n",recseq,expected_recseq);
            // we simply ignore the sequencecounter
            // pdu.data[4] = E_SEQUENCE_NUMBER;
          }
          expected_recseq = (expected_recseq+1) & 0x0ff;
          eib->rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), eib->server);
          pdu.data[0] = b[0];                   // restore received data
          pdu.data[1] = b[1];
          pdu.data[2] = b[2];
          pdu.data[3] = b[3];
          eib->storeBuffer(&pdu.data[4],len-4); // +EIBNETIP_COMMON_CONNECTION_HEADER
          break;
        case TUNNELLING_ACK:
          if(pdu.data[3] == E_NO_ERROR)
          {
            eib->tunnel_ack = 1;
            if(eib->debug) ::printf("eib_reader() TUNNELLING_ACK typespecific=0x%x\n",pdu.data[3]);
          }
          else
          {
            eib->tunnel_ack = -1;
            if(eib->debug) ::printf("eib_reader() TUNNELLING_NACK typespecific=0x%x\n",pdu.data[3]);
          }
          break;
        default:
          ::printf("eib_reader() unknown servicetype=0x%x\n",ntohs(pdu.servicetype));
          break;
      }
    }
    diff = now - last;
    if(eib->isConnected() && eib->channelid != -1 && diff.second > 50)
    { // send heartbeat
      if(eib->debug) ::printf("send heartbeat\n");
      pdu.headersize  = EIB_HEADERSIZE;
      pdu.version     = EIB_VERSION;
      pdu.servicetype = htons(CONNECTIONSTATE_REQUEST);
      pdu.totalsize   = htons(EIB_HEADERSIZE+EIB_ADRSIZE*2+EIB_CRICRDSIZE);
      pdu.data[0]     = eib->channelid;
      pdu.data[1]     = 0;
      eib->rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), eib->server);
      last.getLocalTime();
    }
  }
  return NULL;
  /*
  for(int i=0; i<50; i++)
  {
    p->thread->lock();
    //do something critical
    printf("this is the thread\n");
    p->thread->unlock();
  }
  */
}

rlEIBnetIP::rlEIBnetIP(int num_signals, int _debug, rlDataAcquisitionProvider *_provider) : rlUdpSocket(_debug)
{
  debug = _debug;
  provider = _provider;
  watch_eib = 0;
  if(debug) ::printf("rlEIBnetIP() constructor\n");
  char *cptr;
  mem = NULL;
  memsize = 0;
  running = 0;
  maxvalues = 0;
  is_connected = 0;
  channelid = -1;
  server = NULL;
  send_sequencecounter = 0;
  //rlUdpSocket::setSockopt(SO_BROADCAST);
  rlUdpSocket::bind(rlEIBnetIP::PORT);
  if(num_signals <= 0) return;
  memsize = sizeof(EIB_TEL)*num_signals;
  maxvalues = num_signals;
  cptr = new char[memsize];
  memset(cptr,0,memsize);
  mem = cptr;
  setSourceAdr("/0/0/001");
}

rlEIBnetIP::~rlEIBnetIP()
{
  if(running) stopReading();
  if(mem != NULL)
  {
    char *cptr = (char *) mem;
    delete [] cptr;
  }
  sendDisconnectRequest();
}

int rlEIBnetIP::storeBuffer(unsigned char *buf, int len)
{
  EIB_TEL tel;
  short unsigned int *sptr;
  int i;

  if(len <= 0 || mem == NULL) return -1;
  if(debug)
  {
    ::printf("rlEIBnetIP::storeBuffer() buf=[0x%x",buf[0]);
    for(i=1; i<len; i++) ::printf(",0x%x",buf[i]);
    ::printf("]\n");
  }

  tel.mc = buf[0];
  tel.addi1       = buf[1];
  tel.ctrl1       = buf[2];
  tel.ctrl2       = buf[3];
  sptr            = (unsigned short *) &buf[4];
  tel.saddr       = ntohs(*sptr);
  sptr            = (unsigned short *) &buf[6];
  tel.daddr       = ntohs(*sptr);
  tel.apci_length = buf[8];
  tel.apci        = buf[9];
  tel.val[0] = buf[10];
  tel.val[1] = buf[10+1];
  tel.val[2] = buf[10+2];
  tel.val[3] = buf[10+3];
  for(i=0; i<(len-10); i++) tel.val[i] = buf[10+i];
  switch (tel.mc)
  {
    case L_Data_Req: //0x11:
      ::printf("rlEIBnetIP::storeBuffer() messagecode=0x%x L_Data_Req\n",tel.mc);
      break;
    case L_Data_Ind: //0x29:
      break;
    case L_Data_Con: //0x2E:
      break;
    default:
      ::printf("rlEIBnetIP::storeBuffer() unknown messagecode=0x%x L_Data_Con\n",tel.mc);
      return -1;
  }
  if(debug)
  {
    ::printf("\nmc=0x%x addi1=0x%x ctrl1=0x%x ctrl2=0x%x saddr=0x%x daddr=0x%x acpi_length=0x%x apci=0x%x",
    tel.mc,
    tel.addi1,
    tel.ctrl1,
    tel.ctrl2,
    tel.saddr,
    tel.daddr,
    tel.apci_length,
    tel.apci);
    for(i=10; i<len; i++) ::printf(" val[%d]=0x%x",i-10,tel.val[i-10]);
    ::printf("\n");
  }

  if(watch_eib) printTelegram(&tel);
  if(provider != NULL) return storeInProvider(&tel);

  // store EIB_TEL in *mem
  EIB_TEL *memptr = (EIB_TEL *) mem;
  for(i=0; i<maxvalues; i++)
  {
    if(memptr[i].mc == 0)
    {
      //if(debug) ::printf("rlEIBnetIP::storeBuffer() insert new value\n");
      thread.lock();
      memcpy(&memptr[i],&tel,sizeof(tel));
      thread.unlock();
      return 0;
    }
    //else if(memptr[i].saddr == tel.saddr && memptr[i].daddr == tel.daddr)
    else if(memptr[i].daddr == tel.daddr)
    {
      //if(debug) ::printf("rlEIBnetIP::storeBuffer() update existing value\n");
      thread.lock();
      memcpy(&memptr[i],&tel,sizeof(tel));
      thread.unlock();
      return 0;
    }
  }
  ::printf("rlEIBnetIP::storeBuffer() maxvalues=%d too small\n",maxvalues);
  return -1;
}

int rlEIBnetIP::printTelegram(EIB_TEL *tel)
{
  int s1,s2,s3,d1,d2,d3,val,length;

  s1 = tel->saddr/(8*256);
  s2 = (tel->saddr/256) & 0x0ff;
  s3 = tel->saddr & 0x0ff;
  d1 = tel->daddr/(8*256);
  d2 = (tel->daddr/256) & 0x0ff;
  d3 = tel->daddr & 0x0ff;
  length = tel->apci_length;
  val = 0;
  if(length == 1)
  {
    val = tel->val[0];
  }
  else if(length == 2)
  {
    char *cptr = (char *) &tel->val[0];
    val = (((*cptr)*256)+tel->val[1]);
  }
  else if(length == 3)
  {
    char *cptr = (char *) &tel->val[0];
    val = (((*cptr)*256)+tel->val[1])*tel->val[2];
  }
  else if(length == 4)
  {
    char *cptr = (char *) &tel->val[0];
    val = ((((*cptr)*256)+tel->val[1])*(tel->val[2]*256) * tel->val[3]);
  }
  ::printf("src=/%d/%d/%03d\tdest=/%d/%d/%03d\tval=0x%x\tlen=%d\n",s1,s2,s3,d1,d2,d3,val,length);
  return 0;
}

int rlEIBnetIP::storeInProvider(EIB_TEL *tel)
{
  if(provider == NULL || tel == NULL) return -1;
  //int s1,s2,s3;
  int d1,d2,d3,val,length;
  char name[128], value[128];

  //s1 = tel->saddr/(8*256);
  //s2 = (tel->saddr/256) & 0x0ff;
  //s3 = tel->saddr & 0x0ff;
  d1 = tel->daddr/(8*256);
  d2 = (tel->daddr/256) & 0x0ff;
  d3 = tel->daddr & 0x0ff;
  length = tel->apci_length;
  val = 0;
  if(length == 1)
  {
    val = tel->val[0];
  }
  else if(length == 2)
  {
    char *cptr = (char *) &tel->val[0];
    val = (((*cptr)*256)+tel->val[1]);
  }
  else if(length == 3)
  {
    char *cptr = (char *) &tel->val[0];
    val = (((*cptr)*256)+tel->val[1])*tel->val[2];
  }
  else if(length == 4)
  {
    char *cptr = (char *) &tel->val[0];
    val = ((((*cptr)*256)+tel->val[1])*(tel->val[2]*256) * tel->val[3]);
  }
  //::printf("/%d/%d/%d\t/%d/%d/%d\t0x%x\t%d\n",s1,s2,s3,d1,d2,d3,val,length);
  sprintf(name,"/%d/%d/%03d",d1,d2,d3);
  sprintf(value,"%d",val);
  if(debug) ::printf("storeInProvicer: name=%s value=%s", name, value);
  provider->setStringValue(name, value);
  return 0;
}

int rlEIBnetIP::dump(FILE *fout)
{
  if(fout == NULL) return -1;
  if(debug) ::printf("rlEIBnetIP::dump()\n");
  int s1,s2,s3,d1,d2,d3,val,length;
  int i = 0;
  EIB_TEL *tel;
  EIB_TEL *memptr = (EIB_TEL *) mem;

  val = 0;
  thread.lock();
  while(1)
  {
    tel = &memptr[i];
    if(tel->mc == 0)   break;
    if(i >= maxvalues) break;
    s1 = tel->saddr/(8*256);
    s2 = (tel->saddr/256) & 0x0ff;
    s3 = tel->saddr & 0x0ff;
    d1 = tel->daddr/(8*256);
    d2 = (tel->daddr/256) & 0x0ff;
    d3 = tel->daddr & 0x0ff;
    length = tel->apci_length;
    if(length == 1)
    {
      val = tel->val[0];
    }
    else if(length == 2)
    {
      char *cptr = (char *) &tel->val[0];
      val = (((*cptr)*256)+tel->val[1]);
    }
    else if(length == 3)
    {
      char *cptr = (char *) &tel->val[0];
      val = (((*cptr)*256)+tel->val[1])*tel->val[2];
    }
    else if(length == 4)
    {
      char *cptr = (char *) &tel->val[0];
      val = ((((*cptr)*256)+tel->val[1])*(tel->val[2]*256) * tel->val[3]);
    }
    fprintf(fout,"/%d/%d/%d\t/%d/%d/%d\t0x%x\t%d\n",s1,s2,s3,d1,d2,d3,val,length);
    i++;
  }
  thread.unlock();
  return i;
}

int rlEIBnetIP::setValuesFromCSV(const char *filename)
{
  FILE *fin;
  char line[1024], *cptr;
  int  s1,s2,s3,d1,d2,d3,i,val,len;
  unsigned int uval;

  if(filename == NULL) return -1;
  fin = fopen(filename,"r");
  if(fin == NULL)
  {
    ::printf("rlEIBnetIP::setValuesFromCSV(%s) could not open file\n",filename);
    return -1;
  }

  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    d1 = d2 = d3 = 0;
    cptr = strchr(line,'/');
    if(cptr == NULL) break;
    sscanf(cptr,"/%d/%d/%d",&s1,&s2,&s3);
    i = 0;
    while(line[i] != '\0')
    {
      if(line[i] == ' ' || line[i] == '\t')
      {
        while(line[i] == ' ' || line[i] == '\t') i++;
        if(line[i] == '/')
        {
          cptr = strstr(&line[i],"0x");
          if(cptr == NULL)
          {
            sscanf(&line[i],"/%d/%d/%d %d %d",&d1,&d2,&d3,&val,&len);
          }
          else
          {
            sscanf(&line[i],"/%d/%d/%d %x %d",&d1,&d2,&d3,&val,&len);
          }
        }
        break;
      }
      i++;
    }
    if(line[i] == '\0') return rlEIBnetIP::EIBERROR;
    cptr = strchr(line,'/');
    if(cptr != NULL)
    {
      sprintf(line,"/%d/%d/%d",d1,d2,d3);
      for(i=0; i<100; i++) // try setting even if hardware responds very slowly
      {
        memcpy(&uval,&val,sizeof(uval));
        uval = setValueUnsigned(line,uval,len);
        if(uval != rlEIBnetIP::EIBERROR) break;
      }
    }
  }

  fclose(fin);
  return 0;
}

int rlEIBnetIP::connect()
{
  if(server == NULL) return rlEIBnetIP::EIBERROR;
  if(debug) ::printf("rlEIBnetIP()::connect()\n");
  PDU pdu,response;
  unsigned char cricrd[EIB_CRICRDSIZE];

  if(is_connected == 1)
  {
    ::printf("rlEIBnetIP::connect() already connected\n");
    return 1;
  }

  channelid = -1;
  memset(&response,0,sizeof(response));
  pdu.headersize  = EIB_HEADERSIZE;
  pdu.version     = EIB_VERSION;
  pdu.servicetype = htons(CONNECT_REQUEST);
  pdu.totalsize   = htons(EIB_HEADERSIZE+EIB_ADRSIZE*2+EIB_CRICRDSIZE);
  memcpy(&pdu.data[0],&client.address,EIB_ADRSIZE);
  memcpy(&pdu.data[8],&client.address,EIB_ADRSIZE);
  cricrd[0] = EIB_CRICRDSIZE;    // structlength
  cricrd[1] = TUNNEL_CONNECTION; // connectiontypecode
  cricrd[2] = TUNNEL_LINKLAYER;  // data1
  cricrd[3] = 0;                 // data2
  memcpy(&pdu.data[8*2],cricrd,EIB_CRICRDSIZE);
  is_connected = 0;

  rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), server);
  recv(&response, sizeof(PDU));

  if(ntohs(response.servicetype) == CONNECT_RESPONSE)
  {
    if(debug)
    {
      ::printf("response.headersize  = 0x%x\n",response.headersize);
      ::printf("response.version     = 0x%x\n",response.version);
      ::printf("response.servicetype = 0x%x\n",ntohs(response.servicetype));
      ::printf("response.totalsize   = 0x%x\n",ntohs(response.totalsize));
      ::printf("response.channelid   = 0x%x\n",response.data[0]);
      ::printf("response.status      = 0x%x\n",response.data[1]);
    }
    if(response.data[1] == E_NO_ERROR)
    {
      channelid = response.data[0];
      if(debug) ::printf("CONNECT_RESPONSE == success\n");
      is_connected = 1;
      send_sequencecounter = 0;
      return 1;
    }
    ::printf("response == CONNECT_RESPONSE status=0x%x failed\n",response.data[1]);
    return -1; //
  }
  else
  {
    ::printf("response != CONNECT_RESPONSE\n");
    return -1;
  }
}

int rlEIBnetIP::disconnect()
{
  if(server == NULL) return rlEIBnetIP::EIBERROR;
  if(debug) ::printf("rlEIBnetIP()::disconnect()\n");
  PDU pdu;

  pdu.headersize  = EIB_HEADERSIZE;
  pdu.version     = EIB_VERSION;
  pdu.servicetype = htons(DISCONNECT_RESPONSE);
  pdu.totalsize   = htons(EIB_HEADERSIZE+2+EIB_ADRSIZE);
  pdu.data[0]     = channelid; // channelid
  pdu.data[1]     = 0;         // reserved
  memcpy(&pdu.data[2],&client.address,EIB_ADRSIZE);
  rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), server);
  is_connected = 0;
  channelid = -1;
  return rlEIBnetIP::SUCCESS;
}

int rlEIBnetIP::sendDisconnectRequest()
{
  if(debug) ::printf("rlEIBnetIP()::sendDisconnectRequest()\n");
  if(is_connected == 0) return -1;
  PDU pdu;

  pdu.headersize  = EIB_HEADERSIZE;
  pdu.version     = EIB_VERSION;
  pdu.servicetype = htons(DISCONNECT_REQUEST);
  pdu.totalsize   = htons(EIB_HEADERSIZE+2+EIB_ADRSIZE);
  pdu.data[0]     = channelid; // channelid
  pdu.data[1]     = 0;         // reserved
  memcpy(&pdu.data[2],&client.address,EIB_ADRSIZE);
  rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), server);
  is_connected = 0;
  channelid = -1;
  return rlEIBnetIP::SUCCESS;
}

int rlEIBnetIP::isConnected()
{
  return is_connected;
}

int rlEIBnetIP::startReading()
{
  running = 1;
  is_connected = 0;
  channelid = -1;
  thread.create(eib_reader,this);
  return rlEIBnetIP::SUCCESS;
}

int rlEIBnetIP::stopReading()
{
  running = 0;
  is_connected = 0;
  channelid = -1;
  thread.cancel();
  return rlEIBnetIP::SUCCESS;
}

int rlEIBnetIP::setSourceAdr(const char *adr)
{
  const char *cptr;
  int a1,a2,a3;

  // interpret name
  cptr = strchr(adr,'/');
  if(cptr == NULL)
  {
    ::printf("USER_ERROR: rlEIBnetIP::setSourceAdr() wrong adr=%s\n", adr);
    return -1;
  }
  sscanf(cptr,"/%d/%d/%d",&a1,&a2,&a3);
  saddr = ((a1*8)+a2)*256+a3;
  return 0;
}

int rlEIBnetIP::setValue(const char *name, int val, int length, int addi1, int ctrl, int apci)
{
  unsigned int uval;
  memcpy(&uval,&val,sizeof(uval));
  return setValueUnsigned(name, uval, length, addi1, ctrl, apci);
}

int rlEIBnetIP::value(const char *name)
{
  int val;
  unsigned int uval;
  uval = valueUnsigned(name);
  memcpy(&val,&uval,sizeof(val));
  return val;
}

int rlEIBnetIP::setValueFloat(const char *name, float val, int length, int addi1, int ctrl, int apci)
{
  unsigned int uval,s,e,m;
  float fval,eval;
  if(length == -1 || length == 2)
  {
    length = 2;
    if(val < 0.0f) s = 0x80000000;
    else           s = 0;
    m = (unsigned int) (val*100.0f);
    m = m/100;
    // m*2^e = fabsf(val)
    // 2^e = fabsf(val)/m;
    //   e = ln(fabsf(val)/m)
    if(m == 0) e = 0;
    else
    {
      fval = fabsf(val)/(float) m;
      eval = logf(fval);
      e = ((unsigned int) eval) * 8* 256;
    }
    uval = s | e | m;
  }
  else if(length == 4)
  {
    memcpy(&uval,&val,sizeof(uval));
  }
  else
  {
    return rlEIBnetIP::EIBERROR;
  }
  return setValueUnsigned(name, uval, length, addi1, ctrl, apci);
}

float rlEIBnetIP::valueFloat2(const char *name)
{
  float val,sign;
  unsigned int uval,e,m;
  uval = valueUnsigned(name);
  if(uval & 0x080000000) sign = 1.0f;
  else                   sign = -1.0f;
  uval = uval | 0x07fffffff; // eliminate sign
  m = uval & 0x07ffff;
  e = uval / (8*256);
  val = sign*(0.01f*m)*(2^e);
  return val;
}

float rlEIBnetIP::valueFloat4(const char *name)
{
  float val;
  unsigned int uval;
  uval = valueUnsigned(name);
  memcpy(&val,&uval,sizeof(float));
  return val;
}

unsigned int rlEIBnetIP::valueUnsigned(const char *name)
{
  if(name == NULL || mem == NULL) return rlEIBnetIP::EIBERROR;
  unsigned int val,a1,a2,a3,length;
  const char *cptr;
  EIB_TEL tel;
  EIB_TEL *memptr = (EIB_TEL *) mem;

  // interpret name
  memset(&tel,0xff,sizeof(EIB_TEL));
  cptr = strchr(name,'/');
  if(cptr == NULL)
  {
    ::printf("USER_ERROR: rlEIBnetIP::value() wrong name=%s\n", name);
    return rlEIBnetIP::EIBERROR;
  }
  sscanf(cptr,"/%d/%d/%d",&a1,&a2,&a3);
  tel.saddr = (short) saddr;
  tel.daddr = ((a1*8)+a2)*256+a3;

  for(int i=0; i<maxvalues; i++)
  {
    if(memptr[i].mc == 0) break;
    //if(memptr[i].saddr == tel.saddr && memptr[i].daddr == tel.daddr)
    if(memptr[i].daddr == tel.daddr)
    {
      thread.lock();
      val = 0;
      length = memptr[i].apci_length;
      if(length == 1)
      {
        val = memptr[i].val[0];
      }
      else if(length == 2)
      {
        char *cptr = (char *) &memptr[i].val[0];
        val = (((*cptr)*256)+memptr[i].val[1]);
      }
      else if(length == 3)
      {
        char *cptr = (char *) &memptr[i].val[0];
        val = (((*cptr)*256)+memptr[i].val[1])*memptr[i].val[2];
      }
      else if(length == 4)
      {
        char *cptr = (char *) &memptr[i].val[0];
        val = ((((*cptr)*256)+memptr[i].val[1])*(memptr[i].val[2]*256) * memptr[i].val[3]);
      }
      thread.unlock();
      if(length < 1 || length > 4 || length == 3)
      {
        ::printf("rlEIBnetIP::value(0x%x) unknown length=%d\n",val,length);
        return rlEIBnetIP::EIBERROR;
      }
      return val;
    }
  }
  return rlEIBnetIP::EIBERROR;
}

int rlEIBnetIP::getText(const char *name, char *text, int maxlen)
{
  if(name == NULL || text == NULL || maxlen <= 0) return rlEIBnetIP::EIBERROR;
  unsigned int a1,a2,a3; //,length;
  int j;
  const char *cptr;
  char buf[16];
  EIB_TEL tel;
  EIB_TEL *memptr = (EIB_TEL *) mem;

  text[0] = '\0';
  // interpret name
  memset(&tel,0xff,sizeof(EIB_TEL));
  cptr = strchr(name,'/');
  if(cptr == NULL)
  {
    ::printf("USER_ERROR: rlEIBnetIP::getText() wrong name=%s\n", name);
    return rlEIBnetIP::EIBERROR;
  }
  sscanf(cptr,"/%d/%d/%d",&a1,&a2,&a3);
  tel.saddr = (short) saddr;
  tel.daddr = ((a1*8)+a2)*256+a3;

  for(int i=0; i<maxvalues; i++)
  {
    if(memptr[i].mc == 0) break;
    //if(memptr[i].saddr == tel.saddr && memptr[i].daddr == tel.daddr)
    if(memptr[i].daddr == tel.daddr)
    {
      thread.lock();
      //length = memptr[i].apci_length;
      for(j=0; j<14; j++) buf[j] = memptr[i].val[j];
      buf[14] = '\0';
      j = 0;
      while(j<maxlen && j<14)
      {
        text[j] = buf[j];
        j++;
      }
      return j;
    }
  }
  return 0;
}

int rlEIBnetIP::setText(const char *name, const char *text)
{
  if(name == NULL || text == NULL) return rlEIBnetIP::EIBERROR; 
  char buf[16];
  int length = strlen(text);
  unsigned int a1,a2,a3,daddr;
  int i,retry;
  const char *cptr;
  PDU pdu;
  EIB_TEL tel;
  EIB_TEL *memptr = (EIB_TEL *) mem;

  if(length > 14) length = 14;
  memset(buf,0,sizeof(buf));
  for(i=0; i<length; i++) buf[i] = text[i];

  // interpret name
  cptr = strchr(name,'/');
  if(cptr == NULL)
  {
    ::printf("USER_ERROR: rlEIBnetIP::setText() wrong name=%s text=%s\n", name, text);
    return rlEIBnetIP::EIBERROR;
  }
  sscanf(cptr,"/%d/%d/%d",&a1,&a2,&a3);
  daddr = ((a1*8)+a2)*256+a3;

  // fill EIBnet/IP PDU
  pdu.headersize   = EIB_HEADERSIZE;
  pdu.version      = EIB_VERSION;
  pdu.servicetype  = htons(TUNNELLING_REQUEST);
  pdu.totalsize    = htons(EIB_HEADERSIZE+4+10+length);
  pdu.data[0]      = 4;                    // structlength
  pdu.data[1]      = channelid;            // channelid
  pdu.data[2]      = send_sequencecounter; // sequencecounter
  pdu.data[3]      = E_NO_ERROR;           // typespecific
  tel.mc           = L_Data_Req; // L_Data_Req=0x11, L_Data_Con=0x2E L_Data_Ind=0x29
  tel.addi1        = 0x0;
  tel.ctrl1        = 0xbc;
  tel.ctrl2        = 0xe0;
  tel.saddr        = (unsigned short) htons((short) saddr);   //0x1);
  tel.daddr        = (unsigned short) htons((short) daddr);   //(0x100)
  tel.apci_length  = 0x1;
  tel.apci         = 0x0;
  for(i=0; i<14; i++) tel.val[i] = buf[i];
  memcpy(&pdu.data[4],&tel,sizeof(EIB_TEL));

  tunnel_ack = retry = 0;
  while(1)
  {
    // send value over EIBnet/IP
    rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), server);
    rlsleep(10);
    if     (tunnel_ack == 1) // OK
    {
      send_sequencecounter = (send_sequencecounter+1) & 0x0ff;
      if(debug) ::printf("rlEIBnetIP::setText(%s) ACK\n",text);
      break;
    }
    else if(tunnel_ack == -1) // failure
    {
      if(debug) ::printf("rlEIBnetIP::setText(%s) NACK\n",text);
      retry++;
    }
    else
    {
      if(debug) ::printf("rlEIBnetIP::setText(%s) timeout\n",text);
      retry++;
    }
    if(retry >= 2)
    {
      is_connected = 0 ;
      ::printf("rlEIBnetIP::setText() connection lost\n");
      return rlEIBnetIP::EIBERROR;
    }
  }
  tunnel_ack = 0;

  tel.saddr = ntohs(tel.saddr); // convert network -> host byte order
  tel.daddr = ntohs(tel.daddr);
  // remember value within mem
  for(i=0; i< maxvalues; i++)
  {
    if(memptr[i].mc == 0)
    {
      if(debug) ::printf("rlEIBnetIP::setText() insert new value\n");
      thread.lock();
      memcpy(&memptr[i],&tel,sizeof(tel));
      thread.unlock();
      return length;
    }
    //else if(memptr[i].saddr == tel.saddr && memptr[i].daddr == tel.daddr)
    else if(memptr[i].daddr == tel.daddr)
    {
      if(debug) ::printf("rlEIBnetIP::setText() update existing value\n");
      thread.lock();
      memcpy(&memptr[i],&tel,sizeof(tel));
      thread.unlock();
      return length;
    }
  }
  ::printf("ERROR rlEIBnetIP::setText() not enough signals specified max=%d\n",maxvalues);
  return rlEIBnetIP::EIBERROR;
}

int rlEIBnetIP::setValueUnsigned(const char *name, unsigned int val, int length, int addi1, int ctrl, int apci)
{
  if(name == NULL || mem == NULL || channelid == -1) return rlEIBnetIP::EIBERROR;
  unsigned int i,a1,a2,a3,daddr;
  int retry;
  const char *cptr;
  PDU pdu;
  EIB_TEL tel;
  EIB_TEL *memptr = (EIB_TEL *) mem;

  // interpret name
  cptr = strchr(name,'/');
  if(cptr == NULL)
  {
    ::printf("USER_ERROR: rlEIBnetIP::setValue() wrong name=%s val=%d\n", name, val);
    return rlEIBnetIP::EIBERROR;
  }
  sscanf(cptr,"/%d/%d/%d",&a1,&a2,&a3);
  daddr = ((a1*8)+a2)*256+a3;

  // fill EIBnet/IP PDU
  pdu.headersize   = EIB_HEADERSIZE;
  pdu.version      = EIB_VERSION;
  pdu.servicetype  = htons(TUNNELLING_REQUEST);
  if(length == -1)
    pdu.totalsize    = htons(EIB_HEADERSIZE+4+10+1);
  else
    pdu.totalsize    = htons(EIB_HEADERSIZE+4+10+length);
  pdu.data[0]      = 4;                    // structlength
  pdu.data[1]      = channelid;            // channelid
  pdu.data[2]      = send_sequencecounter; // sequencecounter
  pdu.data[3]      = E_NO_ERROR;           // typespecific
  tel.mc           = L_Data_Req; // L_Data_Req=0x11, L_Data_Con=0x2E L_Data_Ind=0x29

  tel.addi1        = 0x0;
  tel.ctrl1        = 0xbc;
  tel.ctrl2        = 0xe0;
  tel.saddr        = (unsigned short) htons((short) saddr);   //0x1);
  tel.daddr        = (unsigned short) htons((short) daddr);   //(0x100)
  tel.apci_length  = 0x1;
  tel.apci         = 0x0;

  if(addi1  != -1) tel.addi1 = (unsigned char) ((addi1 & 0x0ff));
  if(ctrl   != -1)
  {
    tel.ctrl1 = (unsigned char) ((ctrl & 0x0ff00)/256);
    tel.ctrl2 = (unsigned char) ((ctrl & 0x0ff));
  }
  if(length != -1) tel.apci_length = (unsigned char) length;
  if(apci   != -1) tel.apci = (unsigned char) apci;

  if(length == -1 || length == 1)
  {
    tel.val[0]       = (unsigned char) val; // 0x80; // 0x81
    tel.val[1]       = (unsigned char) 0x0;
    tel.val[2]       = (unsigned char) 0x0;
    tel.val[3]       = (unsigned char) 0x0;
  }
  else if(length == 2)
  {
    tel.val[0]       = (unsigned char) ((val & 0x0ff00)/256);
    tel.val[1]       = (unsigned char) (val & 0x0ff);
    tel.val[2]       = (unsigned char) 0x0;
    tel.val[3]       = (unsigned char) 0x0;
  }
  else if(length == 3)
  {
    tel.val[0]       = (unsigned char) ((val & 0x0ff000000)/(256*256));
    tel.val[1]       = (unsigned char)  (val & 0x0ff0000)/256;
    tel.val[2]       = (unsigned char)  (val & 0x0ff00);
    tel.val[3]       = (unsigned char)  (val & 0x0ff);
  }
  else if(length == 4)
  {
    tel.val[0]       = (unsigned char) ((val & 0x0ff000000)/(256*256*256));
    tel.val[1]       = (unsigned char) ((val & 0x0ff0000)/(256*256));
    tel.val[2]       = (unsigned char) ((val & 0x0ff00)/(256));
    tel.val[3]       = (unsigned char) (val & 0x0ff);
  }
  else
  {
    ::printf("rlEIBnetIP::setValue(0x%x) unknown length=%d\n",val,length);
    return rlEIBnetIP::EIBERROR;
  }
  memcpy(&pdu.data[4],&tel,sizeof(EIB_TEL));

  if(debug)
  {
    ::printf("rlEIBnetIP::setValue() mc          = 0x%x\n", tel.mc);
    ::printf("rlEIBnetIP::setValue() addi1       = 0x%x\n", tel.addi1);
    ::printf("rlEIBnetIP::setValue() ctrl1       = 0x%x\n", tel.ctrl1);
    ::printf("rlEIBnetIP::setValue() ctrl2       = 0x%x\n", tel.ctrl2);
    ::printf("rlEIBnetIP::setValue() saddr       = 0x%x\n", ntohs(tel.saddr));
    ::printf("rlEIBnetIP::setValue() daddr       = 0x%x\n", ntohs(tel.daddr));
    ::printf("rlEIBnetIP::setValue() apci_length = 0x%x\n", tel.apci_length);
    ::printf("rlEIBnetIP::setValue() apci        = 0x%x\n", tel.apci);
    ::printf("rlEIBnetIP::setValue() val[0]      = 0x%x\n", tel.val[0]);
    ::printf("rlEIBnetIP::setValue() val[1]      = 0x%x\n", tel.val[1]);
    ::printf("rlEIBnetIP::setValue() val[2]      = 0x%x\n", tel.val[2]);
    ::printf("rlEIBnetIP::setValue() val[3]      = 0x%x\n", tel.val[3]);
  }

  tunnel_ack = retry = 0;
  while(1)
  {
    // send value over EIBnet/IP
    rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), server);
    rlsleep(10);
    if     (tunnel_ack == 1) // OK
    {
      send_sequencecounter = (send_sequencecounter+1) & 0x0ff;
      if(debug) ::printf("rlEIBnetIP::setValue(0x%x) ACK\n",val);
      break;
    }
    else if(tunnel_ack == -1) // failure
    {
      if(debug) ::printf("rlEIBnetIP::setValue(0x%x) NACK\n",val);
      retry++;
    }
    else
    {
      if(debug) ::printf("rlEIBnetIP::setValue(0x%x) timeout\n",val);
      retry++;
    }
    if(retry >= 2)
    {
      is_connected = 0 ;
      ::printf("rlEIBnetIP::setValue() connection lost\n");
      return rlEIBnetIP::EIBERROR;
    }
  }
  tunnel_ack = 0;

  tel.saddr = ntohs(tel.saddr); // convert network -> host byte order
  tel.daddr = ntohs(tel.daddr);

  if(provider != NULL) return 0;

  // remember value within mem
  for(i=0; i< (unsigned int) maxvalues; i++)
  {
    if(memptr[i].mc == 0)
    {
      if(debug) ::printf("rlEIBnetIP::setValue(0x%x) insert new value\n",val);
      thread.lock();
      memcpy(&memptr[i],&tel,sizeof(tel));
      thread.unlock();
      return 0;
    }
    //else if(memptr[i].saddr == tel.saddr && memptr[i].daddr == tel.daddr)
    else if(memptr[i].daddr == tel.daddr)
    {
      if(debug) ::printf("rlEIBnetIP::setValue(0x%x) update existing value\n",val);
      thread.lock();
      memcpy(&memptr[i],&tel,sizeof(tel));
      thread.unlock();
      return 0;
    }
  }
  ::printf("ERROR rlEIBnetIP::setValue() not enough signals specified max=%d\n",maxvalues);
  return rlEIBnetIP::EIBERROR;
}

int rlEIBnetIP::setServer(rlIpAdr *_server)
{
  if(_server == NULL) return rlEIBnetIP::EIBERROR;
  server = _server;
  PDU pdu;
  int ret;

  ret = rlEIBnetIP::getDescription(&pdu);
  if(ret > 0)
  {
    if(debug) ::printf("rlEIBnetIP::setServer() found server\n");
    memcpy(&server->address,&from.address,sizeof(server->address));
    return 0;
  }
  else
  {
    ::printf("rlEIBnetIP::setServer() could not find server\n");
    return -1;
  }
}

int rlEIBnetIP::setClient(rlIpAdr *_client)
{
  // eib murx
  unsigned char murx1[8], murx2[8];
  memcpy(murx1,&_client->address,sizeof(murx1));
  murx2[0] = murx1[0]; // sin_len
  murx2[1] = murx1[1]; // sin_family
  murx2[6] = murx1[2]; // sin_port
  murx2[7] = murx1[3]; // sin_port
  murx2[2] = murx1[4]; // sin_addr
  murx2[3] = murx1[5]; // sin_addr
  murx2[4] = murx1[6]; // sin_addr
  murx2[5] = murx1[7]; // sin_addr
  memcpy(&client.address,murx2,sizeof(murx2));
  return 0;
}

int rlEIBnetIP::recv(void *buf, int maxlen)
{
  int ret;
  while(1)
  {
    ret = rlUdpSocket::recvfrom(buf, maxlen, &from, 1000);
    if(ret < 0)         return ret;
    if(*server == from) return ret;
  }
}

int rlEIBnetIP::getDescription(PDU *buf)
{
  if(server == NULL) return rlEIBnetIP::EIBERROR;
  if(debug) ::printf("rlEIBnetIP()::getDescription()\n");
  PDU pdu;
  int ret;

  pdu.headersize  = EIB_HEADERSIZE;
  pdu.version     = EIB_VERSION;
  pdu.servicetype = htons(DESCRIPTION_REQUEST);
  pdu.totalsize   = htons(EIB_HEADERSIZE+EIB_ADRSIZE*2);
  memcpy(pdu.data,&client.address,EIB_ADRSIZE*2);

  rlUdpSocket::sendto(&pdu, ntohs(pdu.totalsize), server);
  while(1)
  {
    ret = rlUdpSocket::recvfrom(buf, sizeof(pdu), &from, 1000);
    if(ret < 0)
    {
      if(debug) ::printf("rlEIBnetIP()::getDescription() timeout\n");
      return ret;
    }
    if(ntohs(buf->servicetype) == DESCRIPTION_RESPONSE)
    {
      if(debug) ::printf("rlEIBnetIP()::getDescription() got description\n");
      return ret;
    }
  }
}

//#define TESTING
#ifdef TESTING

int main()
{
  int ret, val;
  rlEIBnetIP eib(1000);
  rlIpAdr client;
  rlIpAdr server;
  char line[1024];

  client.setAdr("nb3lehrig",rlEIBnetIP::PORT);
  //server.setAdr("192.168.1.30",rlEIBnetIP::PORT);
  server.setAdr("eibnet1",rlEIBnetIP::PORT);
  ret = eib.setClient(&client);
  if(ret < 0) return -1;
  ret = eib.setServer(&server);
  if(ret < 0) return -1;
  eib.debug = 1;
  eib.startReading();

  line[0] = '\0';
  while(1)
  {
    scanf("%s",line);
    if(line[0] == 'x') break;
    if(line[0] == 'd')
    {
      eib.dump(stdout);
    }
    else if(line[0] == 's')
    {
      eib.setValuesFromCSV("test.csv");
    }
    else if(line[0] == 't')
    {
      eib.setText("/0/1/000","test");
    }
    else
    {
      sscanf(line,"%x",&val);
      eib.setValue("/0/1/000", val);
      printf("eib.value(/0/1/000)=0x%x\n",eib.value("/0/1/000"));
    }
    printf("x=exit d=dump s=set t=text value=\n");
  }
  return 0;
}

#endif
