/***************************************************************************
                          rlsiemenstcp.cpp  -  description
                             -------------------
    begin                : Mon Mar 08 2004
    copyright            : (C) 2004 by R. Lehrig
    email                : lehrig@t-online.de

    S7_200 update        : Wed Mar 21 2007
    copyright            : (C) 2007 by Aljosa Merljak
    Email                : aljosa.merljak@datapan.si
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

#include "rlsiemenstcp.h"
#include "rlwthread.h"
#include "rlcutil.h"
#include <stdio.h>
#include <string.h>
#define TIMEOUT 2000
#define ISO_PORT 102

extern int rlDebugPrintfState;

rlSiemensTCP::rlSiemensTCP(const char *a, int _plc_type, int _fetch_write, int _function, int _rack_slot)
             :rlSocket(a,ISO_PORT,1)
{
  plc_type = _plc_type;
  fetch_write = _fetch_write;
  function = _function;
  rack_slot = _rack_slot;
  //doConnect();
  use_cb = 0;
  unsigned char connect_block[22];
  getDefaultConnectBlock(connect_block);
  memcpy(cb,connect_block,sizeof(cb));
}

rlSiemensTCP::~rlSiemensTCP()
{
  rlSocket::disconnect();
}

int rlSiemensTCP::getDefaultConnectBlock(unsigned char *connect_block)
{
  static const unsigned char default_connect_block[] =
    {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,1  ,0  ,0xC2,2,0  ,1  ,0xC0,1,9};
  memcpy(connect_block,default_connect_block,sizeof(default_connect_block));
  return 22;
}

int rlSiemensTCP::setConnectBlock(const unsigned char *connect_block)
{
  memcpy(cb,connect_block,sizeof(cb));
  use_cb = 1;
  return 0;
}

int rlSiemensTCP::getConnectBlock(unsigned char *connect_block)
{
  memcpy(connect_block,cb,sizeof(cb));
  return 0;
}

void rlSiemensTCP::doConnect()
{
  int i,i2,ret,length;
  static const unsigned char s7_200_connect_block[] =
    {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,'M','W',0xC2,2,'M','W',0xC0,1,9};
  static const unsigned char s7_300_connect_block[] =
    {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,1  ,0  ,0xC2,2,1  ,2  ,0xC0,1,9};
  static const unsigned char s7_400_connect_block[] =
    {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,1  ,0  ,0xC2,2,1  ,3  ,0xC0,1,9};
  static const unsigned char s7_1200_connect_block[] =
    {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,1  ,0  ,0xC2,2,1  ,0  ,0xC0,1,9};
  static const unsigned char logo_connect_block[] =
    {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00, 0xC0,1,9, 0xC1,2,2,0, 0xC2,2,2,0};
  //{3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,2, 0   ,0xC2,2,2  ,0  ,0xC0,1,9}; // this should be equivalent to the above (see detailed description)
  static const unsigned char other_connect_block[] =
    {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,1  ,0  ,0xC2,2,0  ,1  ,0xC0,1,9};
  //######### Description of the individual bytes within the connect_block (connect request) ###
  //{3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,1  ,0  ,0xC2,2,0  ,1  ,0xC0,1,9}; 
  // |         |    |                            |              |              |- Identifier: Maximum TPDU size will follow, 1 byte, 
  // |         |    |                            |              |                             where 9 means max 512 octets
  // |         |    |                            |              |---------------- Identifier: Called TSAP will follow, 2 byte, 
  // |         |    |                            |                                            where (1=PG, 2=OP, 3=Step7Basic), 
  // |         |    |                            |                                            rack=0 slot=1 (upper_3_bit_is_rack / 
  // |         |    |                            |                                            lower_5_bit_is_slot)
  // |         |    |                            |------------------------------- Identifier: Calling TSAP will follow, 2 byte,
  // |         |    |                                                                         where (1=PG, 2=OP) 3=Step7Basic), 
  // |         |    |                                                                         rack=0 slot=0 (upper_3_bit_is_rack / 
  // |         |    |                                                                         lower_5_bit_is_slot)
  // |         |    |------------------------------------------------------------ 0xEO = Connection Request Code, 
  // |         |                                                                              destination ref high/low = 0,0
  // |         |                                                                              source ref high/low = 0,1
  // |         |                                                                              Class and Option = 0
  // |         |----------------------------------------------------------------- 0x11 = 17 = Number of bytes that follow
  // |--------------------------------------------------------------------------- ISO_HEADER: version=3 reserved=0
  //                                                                                          lenght_high=0 length_low=22 byte
  //#############################################################################################
  unsigned char connect_block[22];

  unsigned char connect_block2[] = 
    {0x03,0x00,0x00,0x19,0x02,0xF0,0x80,0x32,0x01,0x00,0x00,0xCC,0xC1,0x00,0x08,0x00,0x00,0xF0,0x00,0x00,0x01,0x00,0x01,0x03,0xC0};  
  unsigned char buf[512];

  if(use_cb)
  {
    memcpy(connect_block,cb,sizeof(cb));
  }
  else
  {
    if     (plc_type == S7_200)  memcpy(connect_block,s7_200_connect_block,sizeof(connect_block));
    else if(plc_type == S7_300)  memcpy(connect_block,s7_300_connect_block,sizeof(connect_block));
    else if(plc_type == S7_400)  memcpy(connect_block,s7_400_connect_block,sizeof(connect_block));
    else if(plc_type == S7_1200) memcpy(connect_block,s7_1200_connect_block,sizeof(connect_block));
    else if(plc_type == LOGO)    memcpy(connect_block,logo_connect_block,sizeof(connect_block));
    else                         memcpy(connect_block,other_connect_block,sizeof(connect_block));

    // according to an unproofen theory siemens chooses the TSAP as follows
    // connect_block[17] = 2; Function (1=PG,2=OP,3=Step7Basic)
    // connect_block[18] = upper_3_bit_is_rack / lower_5_bit_is_slot
    // Hint: use tcpdump to figure it out (host = ip_adr of your PLC)
    // tcpdump -A -i eth0 -t -q -s 0 "host 192.168.1.14 && port 102"
    if(function  != -1) connect_block[17] = function;
    if(rack_slot != -1) connect_block[18] = rack_slot;
  }

  for(i=0; i<3; i++)
  {
    if(rlSocket::connect() >= 0)
    {
      // exchange TSAP
      rlDebugPrintf("write connect_block\n");
      rlSocket::write(connect_block,sizeof(connect_block));
      ret = rlSocket::read(&ih,sizeof(ih),TIMEOUT);
      rlDebugPrintf("read ih ret=%d\n",ret);
      if(ret <= 0) { rlSocket::disconnect(); continue; }
      length = ih.length_high*256 + ih.length_low;
      rlDebugPrintf("read buf length=%d\n",length);
      ret = rlSocket::read(buf,length-sizeof(ih),TIMEOUT);
      rlDebugPrintf("read buf ret=%d\n",ret);
      if(ret <= 0) { rlSocket::disconnect(); continue; }
      if(length == 22)
      {
        for(i2=0; i2<3; i2++)
        {
          rlDebugPrintf("write connect_block2\n");
          rlSocket::write(connect_block2,sizeof(connect_block2));
          ret = rlSocket::read(&ih,sizeof(ih),TIMEOUT);
          rlDebugPrintf("read2 ih ret=%d\n",ret);
          length = ih.length_high*256 + ih.length_low;
          rlDebugPrintf("read2 buf length=%d\n",length);
          ret = rlSocket::read(buf,length-sizeof(ih),TIMEOUT);
          rlDebugPrintf("read2 buf ret=%d\n",ret);
          if(ret <= 0) { rlSocket::disconnect(); continue; }
          if(ret > 0) 
          {
            rlDebugPrintf("connect success\n");
            return;
          }  
        }
        rlSocket::disconnect();
        return;
      }
    }
    else
    {
      rlsleep(100);
      rlDebugPrintf("connect failed\n");
    }
  }
}

int rlSiemensTCP::getOrg(int org)
{
  int ret;
  switch(org)
  {
    /* before suggestions from sps-forum.de
    case ORG_DB:   ret = 0x84;  break;    //[10] Datenbaustein
    case ORG_M:    ret = 0x83;  break;    //[10] Merker
    case ORG_E:    ret = 0x81;  break;    //[10] Eingang
    case ORG_A:    ret = 0x82;  break;    //[10] Ausgang
    case ORG_PEPA: ret = 0x80;  break;    //[10] Peripheral Area R/W [tested by VSA]
    case ORG_Z:    ret = 0x84;  break;    //[10] not tested
    case ORG_T:    ret = 29;    break;    //[10] Timer
    default:       return 0x83; break;
    */
    case ORG_DB:   ret = 0x84;  break;    //[10] Datenbaustein
    case ORG_M:    ret = 0x83;  break;    //[10] Merker
    case ORG_E:    ret = 0x81;  break;    //[10] Eingang
    case ORG_A:    ret = 0x82;  break;    //[10] Ausgang
    case ORG_PEPA: ret = 0x80;  break;    //[10] Peripheral Area R/W [tested by VSA]
    case ORG_Z:    ret = 28;              //[10] Zaehler // according sps-forum.de ---
                   if(plc_type==S7_200) ret = 30;        // according sps-forum.de ...   
                   break;
    case ORG_T:    ret = 29;              //[10] Timer
                   if(plc_type==S7_200) ret = 31;        // according sps-forum.de ...
                   break;                                // J. Kühner and R. Hönle
    // Attention: The encoding of Zaehler and Timer will be different on S7_200 than to other plc models
    default:       return 0x83; break;
  }
  return ret;
}
int rlSiemensTCP::write(int org, int dbnr, int start_adr, int len, const unsigned char *buf, int function)
{
  int i,ibuf,ret,len_byte,length;
  if(rlSocket::isConnected() == 0) doConnect();
  if(rlSocket::isConnected() == 0) return -1;

  len_byte = len;
  if(len_byte > (int) sizeof(pdu)) return -1;
  //if(org == ORG_DB) len_byte *= 2;
  //if(org == ORG_Z)  len_byte *= 2;
  //if(org == ORG_T)  len_byte *= 2;
  
  if((plc_type == S5 || plc_type == S7_300 || plc_type == S7_400) && fetch_write == 1)
  {
    rlDebugPrintf("using fetch_write\n");
    length = sizeof(ih) + sizeof(wh) + len_byte;
    unsigned char total_buf[sizeof(ih) + sizeof(wh) + sizeof(pdu)];
    ih.version  = 3;
    ih.reserved = 0;
    ih.length_high = length / 256;
    ih.length_low  = length & 0x0ff;
    wh.ident[0]        = 'S';
    wh.ident[1]        = '5';
    wh.header_len      = 16;
    wh.ident_op_code   = 1;
    wh.op_code_len     = 3;
    wh.op_code         = 3;
    wh.ident_org_block = 3;
    wh.len_org_block   = 8;
    wh.org_block       = (unsigned char) org;
    wh.dbnr            = (unsigned char) dbnr;
    wh.start_adr[0]    = (unsigned char) start_adr / 256;
    wh.start_adr[1]    = (unsigned char) start_adr & 0x0ff;;
    wh.len[0]          = (unsigned char) len / 256;
    wh.len[1]          = (unsigned char) len & 0x0ff;;
    wh.spare1          = 0x0ff;
    wh.spare1_len      = 2;
    memcpy(total_buf,                       &ih, sizeof(ih));
    memcpy(total_buf+sizeof(ih),            &wh, sizeof(wh));
    memcpy(total_buf+sizeof(ih)+sizeof(wh), buf, len_byte);
    ret = rlSocket::write(total_buf, sizeof(ih)+sizeof(wh)+len_byte);
    rlDebugPrintf("write total_buf ret=%d\n",ret);
    if(ret < 0) return ret;
    /*
    ret = rlSocket::write(&ih,sizeof(ih));
    rlDebugPrintf("write ih ret=%d\n",ret);
    if(ret < 0) return ret;
    ret = rlSocket::write(&wh,sizeof(wh));
    rlDebugPrintf("write wh ret=%d\n",ret);
    if(ret < 0) return ret;
    ret = rlSocket::write(buf,len_byte);
    rlDebugPrintf("write buf ret=%d\n",ret);
    if(ret < 0) return ret;
    */
    ret = rlSocket::read(&ih,sizeof(ih),TIMEOUT);
    rlDebugPrintf("read ih ret=%d\n",ret);
    if(ret <= 0) return ret;
    ret = rlSocket::read(&wa,sizeof(wa),TIMEOUT);
    rlDebugPrintf("read wa ret=%d\n",ret);
    if(ret <= 0) return ret;
    if(wa.error_block != 0) return -1;
  }
  else
  {
    rlDebugPrintf("not using fetch_write\n");
    i = 0;
    pdu[i++] = 0x02;
    pdu[i++] = 0xF0;
    pdu[i++] = 0x80;
    pdu[i++] = 0x32;
    pdu[i++] = 0x01;
    pdu[i++] = 0x00;
    pdu[i++] = 0x00;
    pdu[i++] = 0x00;
    pdu[i++] = 0x00;
    pdu[i++] = 0x00;
    // The S7 update by Aljosa Merljak was tested on S7_200 only
    // You could set your plc_type to S7_200 also, even if you have S7_300 || S7_400
    // But only the else part has been tested with S7_300 and S7_400 up to now
    //if(plc_type == S7_200)
    // Hi Ken, currently only S7_200 was tested with this. But try if this also works with S7_400
    if(plc_type >0) // works for all known plc_type == S7_200 || plc_type == S7_300 || plc_type == S7_400)
    {
      ret = 0;
      switch(function)
      {
         case WriteBit:   ret = write_bit  (i, org, dbnr, start_adr, len, buf); break;
         case WriteByte:  ret = write_byte (i, org, dbnr, start_adr, len, buf); break;
      }
      if(ret < 0) return ret;
    }
    else           // will only work for WriteByte with len_byte = 4
    {
      pdu[i++] = 0x0E;
      pdu[i++] = 0x00;
      pdu[i++] = 0x08;
      pdu[i++] = 0x05; //0 write
      pdu[i++] = 0x01; //1
      pdu[i++] = 0x12; //2
      pdu[i++] = 0x0A; //3
      pdu[i++] = 0x10; //4
      pdu[i++] = 0x02; //5
      pdu[i++] = len_byte / 256;   //6 0x00;
      pdu[i++] = len_byte & 0x0ff; //7 0x04;
      pdu[i++] = dbnr / 256;       //8 0x00;
      pdu[i++] = dbnr & 0x0ff;     //9 0x00;
      pdu[i++] = getOrg(org);      //10 
      pdu[i++] = ((start_adr*8)/0x010000) & 0x0ff; //0x00;  // [11] start adr/bits
      pdu[i++] = ((start_adr*8)/0x0100)   & 0x0ff; //0x00;  // [12] start adr/bits
      pdu[i++] =  (start_adr*8)           & 0x0ff; //0x00;  // [13] start adr/bits
      pdu[i++] = 0x00;
      pdu[i++] = 0x04;
      pdu[i++] = 0x00;
      pdu[i++] = 0x20;
      for(ibuf=0; ibuf<len_byte; ibuf++)
      {
        pdu[i++] = buf[ibuf];
        if(i > (int) sizeof(pdu)) return -1;
      }
    }  
    ret = write_iso(pdu,i);
    if(ret < 0) return ret;
    ret = read_iso(pdu);
    if(ret < 0) return ret;
    if(pdu[15] != 0x05) return -1;
    if(pdu[16] != 0x01) return -1;

    // CODE from Víctor Centelles
    if(pdu[17] != 0xff)
    {
      if(pdu[17] == 0x0a){
        fprintf( stderr, " > Error: Trying to access a DB that does not exist\n");
        fprintf( stderr, "          Please, check that DB is set.   (error code: 10 (0x0a))\n");
        return -(pdu[17]);
      }
      else if(pdu[17] == 0x05){
        fprintf(stderr, " > Error: Trying to access an address that does not exist.\n");
        fprintf(stderr, "          Please, check the address range. (error code: 5 (0x05))\n");
        return -(pdu[17]);
      }
      else if(pdu[17] == 0x07){
        fprintf(stderr, " > Error: the write data size doesn't fit item size\n"); // NO TESTED!!!
        fprintf(stderr, "          Please, check the data size.     (error code: 7 (0x07))\n");
        return -(pdu[17]);
      }
      else{
        fprintf(stderr, " > Error: unknown error  (código %x!=0xff)\n", pdu[17]);
        return -(pdu[17]);
      }
    }
  }

  return len_byte;
}

int rlSiemensTCP::write_bit(int& i, int org, int dbnr, int start_adr, int len, const unsigned char *buf)
{
  int j;
  pdu[i++] = 14 + 12 * (len - 1);
  pdu[i++] = 0x00;
  pdu[i++] = 6 * len - 1;
  pdu[i++] = 0x05;
  pdu[i++] = len;    
  for(j=0; j<len; j++)
  {
    pdu[i++] = 0x12;
    pdu[i++] = 0x0a;
    pdu[i++] = 0x10;
    pdu[i++] = 0x01;
    pdu[i++] = len / 256;                           //6 0x00;
    pdu[i++] = 0x01;                                //7 number of bytes in group
    pdu[i++] = dbnr / 256;                          //8 0x00;
    pdu[i++] = dbnr & 0x0ff;                        //9 0x00;
    pdu[i++] = getOrg(org);                         //10 
    pdu[i++] = ((start_adr / 8)/0x010000)  & 0x0ff;
    pdu[i++] =  (start_adr / 0x0100)       & 0x0ff; //0x00;  // [12] start adr/bits
    pdu[i++] =  (start_adr + j)            & 0x0ff; //0x00;  // [13] start adr/bits     
  }
  for(j=0; j<len; j++)
  {
    pdu[i++] = 0x00;
    pdu[i++] = 0x03;
    pdu[i++] = 0x00;
    pdu[i++] = 0x01;
    pdu[i++] = (buf[j]>0) ? 0x01 : 0x00;
    if(j < len - 1 ) pdu[i++] = 0x00;
    if(i > (int) sizeof(pdu)) return -1;
  }
  return i;
}

int rlSiemensTCP::write_byte(int& i, int org, int dbnr, int start_adr, int len, const unsigned char *buf)
{
  pdu[i++] = 0x0e;
  pdu[i++] = 0x00;
  pdu[i++] = 5 + len - 1;
  pdu[i++] = 0x05;
  pdu[i++] = 0x01;
  pdu[i++] = 0x12;
  pdu[i++] = 0x0a;
  pdu[i++] = 0x10;
  pdu[i++] = 0x02;
  pdu[i++] = len / 256;                       //6 0x00;
  pdu[i++] = len & 0x0ff;                     //7 number of bytes
  pdu[i++] = dbnr / 256;                      //8 0x00;
  pdu[i++] = dbnr & 0x0ff;                    //9 0x00;
  pdu[i++] = getOrg(org);                     //10 
  pdu[i++] =   start_adr   /0x10000  & 0x0ff;
  pdu[i++] = ((start_adr*8)/0x0100)  & 0x0ff; //0x00;  // [12] start adr/bits
  pdu[i++] =  (start_adr*8)          & 0x0ff; //0x00;  // [13] start adr/bits     
  pdu[i++] = 0x00;
  pdu[i++] = 0x04;
  pdu[i++] = (len * 8) / 256;
  pdu[i++] = (len * 8) & 0xff;
  for(int ibuf=0; ibuf<len; ibuf++)
  {
    pdu[i++] = buf[ibuf];
    if(i > (int) sizeof(pdu)) return -1;
  }
  return i;
}

int rlSiemensTCP::fetch(int org, int dbnr, int start_adr, int len, unsigned char *buf)
{
  int i,ret,len_byte,length;

  if(rlSocket::isConnected() == 0) doConnect();
  if(rlSocket::isConnected() == 0) return -1;

  len_byte = len;
  //if(org == ORG_DB) len_byte *= 2;
  //if(org == ORG_Z)  len_byte *= 2;
  //if(org == ORG_T)  len_byte *= 2;

  if((plc_type == S5 || plc_type == S7_300 || plc_type == S7_400) && fetch_write == 1)
  {
    length = sizeof(ih) + sizeof(fh);
    ih.version  = 3;
    ih.reserved = 0;
    ih.length_high = length / 256;
    ih.length_low  = length & 0x0ff;
    fh.ident[0]        = 'S';
    fh.ident[1]        = '5';
    fh.header_len      = 16;
    fh.ident_op_code   = 1;
    fh.op_code_len     = 3;
    fh.op_code         = 5;
    fh.ident_org_block = 3;
    fh.len_org_block   = 8;
    fh.org_block       = (unsigned char) org;
    fh.dbnr            = (unsigned char) dbnr;
    fh.start_adr[0]    = (unsigned char) start_adr / 256;
    fh.start_adr[1]    = (unsigned char) start_adr & 0x0ff;;
    fh.len[0]          = (unsigned char) len / 256;
    fh.len[1]          = (unsigned char) len & 0x0ff;;
    fh.spare1          = 0x0ff;
    fh.spare1_len      = 2;
    unsigned char total_buf[sizeof(ih)+sizeof(fh)];
    memcpy(total_buf,             &ih, sizeof(ih));
    memcpy(total_buf+sizeof(ih),  &fh, sizeof(fh));
    ret = rlSocket::write(total_buf,   sizeof(ih)+sizeof(fh));
    rlDebugPrintf("fetch write ih ret=%d\n",ret);
    if(ret < 0) return ret;  
    /*
    ret = rlSocket::write(&ih,sizeof(ih));
    rlDebugPrintf("fetch write ih ret=%d\n",ret);
    if(ret < 0) return ret;  
    ret = rlSocket::write(&fh,sizeof(fh));
    rlDebugPrintf("fetch write fh ret=%d\n",ret);
    if(ret < 0) return ret;
    */
    ret = rlSocket::read(&ih,sizeof(ih),TIMEOUT);
    rlDebugPrintf("fetch read ih ret=%d\n",ret);
    if(ret <= 0) return ret;
    ret = rlSocket::read(&fa,sizeof(fa),TIMEOUT);
    rlDebugPrintf("fetch read fa ret=%d\n",ret);
    if(ret <= 0) return ret;
    if(fa.error_block != 0) return -1;
    ret = rlSocket::read(buf,len_byte,TIMEOUT);
    rlDebugPrintf("fetch read buf ret=%d\n",ret);
    if(ret <= 0) return ret;
  }
  else
  {
    rlDebugPrintf("fetch:starting org=%d dbnr=%d start_adr=%d len=%d\n", org, dbnr, start_adr, len);
    i = 0;
    pdu[i++] = 0x02;  // [0]
    pdu[i++] = 0xF0;  // [0]
    pdu[i++] = 0x80;  // [0]
    pdu[i++] = 0x32;  // [0]
    pdu[i++] = 0x01;  // [0]
    pdu[i++] = 0x00;  // [0]
    pdu[i++] = 0x00;  // [0]
    pdu[i++] = 0x00;  // [0]
    pdu[i++] = 0x00;  // [0]
    pdu[i++] = 0x00;  // [0]
    pdu[i++] = 0x0E;  // [0]
    pdu[i++] = 0x00;  // [0]
    pdu[i++] = 0x00;  // [0]
    pdu[i++] = 0x04;  // [0]  read
    pdu[i++] = 0x01;  // [1]
    pdu[i++] = 0x12;  // [2]
    pdu[i++] = 0x0A;  // [3]
    pdu[i++] = 0x10;  // [4]
    pdu[i++] = 0x02;  // [5] 
    pdu[i++] = len_byte / 256;              //0x00;  // [6]  len/bytes
    pdu[i++] = len_byte & 0x0ff;            //0x40;  // [7]  len/bytes
    pdu[i++] = dbnr / 256;                  //0x00;  // [8]  dbnum
    pdu[i++] = dbnr & 0x0ff;                //0x01;  // [9]  dbnum
    pdu[i++] = getOrg(org);                 //10 
    pdu[i]   = ((start_adr*8)/0x010000) & 0x0ff; //0x00;  // [11] start adr/bits
    if (plc_type == S7_200) pdu[i] = start_adr / 0x10000;
    i++;
    pdu[i++] = ((start_adr*8)/0x0100)   & 0x0ff; //0x00;  // [12] start adr/bits
    pdu[i++] =  (start_adr*8)           & 0x0ff; //0x00;  // [13] start adr/bits
    ret = write_iso(pdu,i);
    if(ret < 0)
    {
      rlDebugPrintf("fetch:write_iso error ret==%d -> return -1\n", ret);
      return ret;
    }  
    ret = read_iso(pdu);
    if(ret < 0)
    {
      rlDebugPrintf("fetch:read_iso error ret==%d -> return -1\n", ret);
      return ret;
    }  
    if(pdu[15] != 0x04)
    {
      rlDebugPrintf("fetch:pdu[15]=%d is not equal 0x04-> return -1\n", pdu[15]);
      return -1;
    }  
    if(pdu[16] != 0x01)
    {
      rlDebugPrintf("fetch:pdu[16]=%d is not equal 0x04-> return -1\n", pdu[16]);
      return -1;
    }  
    i = 21;
    if(ret < i+len_byte) return -1;
    for(int ibuf = 0; ibuf < len_byte; ibuf++)
    {
      buf[ibuf] = pdu[i++];
    }
  }
  rlDebugPrintf("fetch:success len_byte=%d\n", len_byte);

  return len_byte;
}

int rlSiemensTCP::read_iso(unsigned char *buf)
{
  int i,ret,len;

  ret = rlSocket::read(&ih,sizeof(ih),TIMEOUT);
  if(ret < 0)                
  { 
    rlDebugPrintf("read_iso:failure to read iso header ret=%d -> disconnecting\n", ret);
    rlSocket::disconnect(); 
    return ret; 
  }
  if(ih.version != 3)
  { 
    rlDebugPrintf("read_iso:header vesion mismatch version==%d -> disconnecting\n", ret);
    rlSocket::disconnect(); 
    return -1;  
  }
  len = ih.length_high*256 + ih.length_low - 4;
  if(len <= 0)                
  { 
    rlDebugPrintf("read_iso:len==%d from iso header is negative -> disconnecting\n", len);
    rlSocket::disconnect(); 
    return -1;  
  }
  if(len > (int) sizeof(pdu))
  { 
    rlDebugPrintf("read_iso:len==%d from iso header is larger than max PDU size -> disconnecting\n", len);
    rlSocket::disconnect(); 
    return -1;  
  }
  ret = rlSocket::read(buf,len,TIMEOUT);
  if(ret < 0)                 
  { 
    rlDebugPrintf("read_iso:read buf got timeout -> disconnecting\n");
    rlSocket::disconnect(); 
    return ret; 
  }
  if(rlDebugPrintfState != 0)
  {
    ::printf("read_iso() len=%d\n", len);
    for(i=0; i<len; i++) ::printf("%02x,",buf[i]);
    ::printf("\n");
  }
  return len;
}

int rlSiemensTCP::write_iso(unsigned char *buf, int len)
{
  int i,ret;

  if(len > (int) sizeof(pdu)) return -1;
  if(rlSocket::isConnected() == 0) doConnect();
  if(rlSocket::isConnected() == 0) return -1;
 
  // speedup siemens communication as suggested by Vincent Segui Pascual
  // do only 1 write
  unsigned char total_buf[sizeof(ih)+sizeof(pdu)];
  ih.version  = 3;
  ih.reserved = 0;
  ih.length_high = (len+4) / 256;
  ih.length_low  = (len+4) & 0x0ff;
  memcpy(total_buf,              &ih, sizeof(ih));
  memcpy(total_buf + sizeof(ih), buf, sizeof(ih)+len);
  ret = rlSocket::write(total_buf,    sizeof(ih)+len);
  if(ret < 0)
  { 
    rlDebugPrintf("write_iso:failure to write buf -> disconnecting\n");
    rlSocket::disconnect(); 
    return ret; 
  }
/*  
  ih.version  = 3;
  ih.reserved = 0;
  ih.length_high = (len+4) / 256;
  ih.length_low  = (len+4) & 0x0ff;
  ret = rlSocket::write(&ih,sizeof(ih));
  if(ret < 0)
  { 
    rlDebugPrintf("write_iso:failure to write iso header -> disconnecting\n");
    rlSocket::disconnect(); 
    return ret; 
  }  
  ret = rlSocket::write(buf,len);
  if(ret < 0)
  { 
    rlDebugPrintf("write_iso:failure to write buf -> disconnecting\n");
    rlSocket::disconnect(); 
    return ret; 
  }
*/
  if(rlDebugPrintfState != 0)
  {
    ::printf("write_iso() len=%d\n", len);
    for(i=0; i<len; i++) ::printf("%02x,",buf[i]);
    ::printf("\n");
  }
  return len;
}
