/***************************************************************************
                          rlsiemenstcp.h  -  description
                             -------------------
    begin                : Mon Mar 08 2004
    copyright            : (C) 2004 by Rainer Lehrig
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
#ifndef _RL_SIEMENS_TCP_H_
#define _RL_SIEMENS_TCP_H_

#include "rldefine.h"
#include "rlsocket.h"

/*! <pre>
class for communication with Siemens PLC's via TCP
</pre> */
class rlSiemensTCP : public rlSocket
{
public:
  enum ORG
  {
    ORG_DB   = 1,
    ORG_M    = 2,
    ORG_E    = 3,
    ORG_A    = 4,
    ORG_PEPA = 5,
    ORG_Z    = 6,
    ORG_T    = 7
  };
  enum PLC_TYPE
  {
    S7_200    = 1,
    S7_300    = 2,
    S7_400    = 3,
    S5        = 4,
    RACK_SLOT = 5
  };
  enum SiemensFunctionCodes
  {
    WriteBit   = 1,
    WriteByte  = 2
  };
  rlSiemensTCP(const char *adr, int _plc_type, int _fetch_write = 1, int rackNumber = -1, int slotNumber = -1);
  virtual ~rlSiemensTCP();
  /*! <pre>
  </pre> */
  int write(int org, int dbnr, int start_adr, int length, const unsigned char *buf, int function=WriteByte);
  int fetch(int org, int dbnr, int start_adr, int length, unsigned char *buf);
private:
  void doConnect();
  int read_iso(unsigned char *buf);
  int write_iso(unsigned char *buf, int len);
  int getOrg(int org);
  int write_bit(int& i, int org, int dbnr, int start_adr, int len, const unsigned char *buf);
  int write_byte(int& i, int org, int dbnr, int start_adr, int length, const unsigned char *buf);
  typedef struct
  {
    unsigned char version;
    unsigned char reserved;
    unsigned char length_high;
    unsigned char length_low;
  }IH; // iso header
  typedef struct
  {
    unsigned char ident[2];
    unsigned char header_len;
    unsigned char ident_op_code;
    unsigned char op_code_len;
    unsigned char op_code;
    unsigned char ident_org_block;
    unsigned char len_org_block;
    unsigned char org_block;
    unsigned char dbnr;
    unsigned char start_adr[2];
    unsigned char len[2];
    unsigned char spare1;
    unsigned char spare1_len;
  }WH; // write header
  typedef struct
  {
    unsigned char ident[2];
    unsigned char header_len;
    unsigned char ident_op_code;
    unsigned char op_code_len;
    unsigned char op_code;
    unsigned char ack_block;
    unsigned char ack_block_len;
    unsigned char error_block;
    unsigned char spare1;
    unsigned char spare1_len;
    unsigned char spare2[5];
  }WA; // write ack
  typedef struct
  {
    unsigned char ident[2];
    unsigned char header_len;
    unsigned char ident_op_code;
    unsigned char op_code_len;
    unsigned char op_code;
    unsigned char ident_org_block;
    unsigned char len_org_block;
    unsigned char org_block;
    unsigned char dbnr;
    unsigned char start_adr[2];
    unsigned char len[2];
    unsigned char spare1;
    unsigned char spare1_len;
  }FH; // fetch header
  typedef struct
  {
    unsigned char ident[2];
    unsigned char header_len;
    unsigned char ident_op_code;
    unsigned char op_code_len;
    unsigned char op_code;
    unsigned char ack_block;
    unsigned char ack_block_len;
    unsigned char error_block;
    unsigned char spare1;
    unsigned char spare1_len;
    unsigned char spare2[5];
  }FA; // fetch ack
  WH wh;
  WA wa;
  FH fh;
  FA fa;
  IH ih;
  int rack_number, slot_number;
  int plc_type;
  int fetch_write;
  unsigned char pdu[2048];
};

#endif
