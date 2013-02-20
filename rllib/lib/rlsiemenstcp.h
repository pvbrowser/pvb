/***************************************************************************
                          rlsiemenstcp.h  -  description
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
#ifndef _RL_SIEMENS_TCP_H_
#define _RL_SIEMENS_TCP_H_

#include "rldefine.h"
#include "rlsocket.h"

/*! <pre>
class for communication with Siemens PLC's via TCP

There are problems with reverse engineering.
But meanwhile it is getting much clearer.

There is the old Fetch/Write protocol from the old S5 PLC (fetch_write=1) (done).
And there is the current Siemens PLC protocol introduced with the S7 series of PLC (fetch_write=0).

The only thing that seems to be different between the individual PLC types is the setup of the connection.
The communication itself seems to be the same across all PLC types.

The setup of the communication includes the exchange of the TSAPs.
Siemens seems to choose different combinations there.

You may use rlSiemensTCP with the individual plc_type for conveniance.
But you can set the whole connect_block and use ANY_SIEMENS_COMPATIBLE_PLC.

unsigned char cb[22];
rlSiemensTCP *plc = new rlSiemensTCP(adr,ANY_SIEMENS_COMPATIBLE_PLC);
plc->getDefaultConnectBlock(cb);
cb[16] = 2;
cb[17] = 1;
cb[18] = 0;
plc->setConnectBlock(cb);
Now you can read/write the PLC.

The following matrix shows our knowledge up to now.
------------------------------------------------------------------------------
        | CB16         CB17         CB18
------------------------------------------------------------------------------
S7_200  |  2           'M'          'W'
------------------------------------------------------------------------------
S7_300  |  2            1            2
------------------------------------------------------------------------------
S7_400  |  2            1            3
------------------------------------------------------------------------------
S7_1200 |  2            1            0
------------------------------------------------------------------------------
LOGO    | function     rack+1       slot (LOGO 0BA7, 
        |                                 client TSAP 01.00.
        |                                 server TSAP 20.00 
        |                                 according to mhe_fr from our forum)
------------------------------------------------------------------------------

# according to an unproofen theory siemens chooses the TSAP as follows
# connect_block[17] = 2; Function (1=PG,2=OP,3=Step7Basic)
# connect_block[18] = upper_3_bit_is_rack / lower_5_bit_is_slot
#   if(function != -1) connect_block[17] = function;
#   if(rack_slot != -1) connect_block[18] = rack_slot;

Please use Wireshark or tcpdump if the settings of the above matrix do not work for you.
Send us your results. 
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
    ANY_SIEMENS_COMPATIBLE_PLC = 0,
    S7_200    = 1,
    S7_300    = 2,
    S7_400    = 3,
    S5        = 4,
    RACK_SLOT = 5,
    S7_1200   = 6   // patch from user slammera from our forum (8 Nov 2012)
  };
  enum SiemensFunctionCodes
  {
    WriteBit   = 1,
    WriteByte  = 2
  };
  rlSiemensTCP(const char *adr, int _plc_type, int _fetch_write = 1, int function = -1, int rack_slot = -1);
  virtual ~rlSiemensTCP();
  /*! <pre>
  </pre> */
  int getDefaultConnectBlock(unsigned char *connect_block);
  int setConnectBlock(const unsigned char *connect_block);
  int getConnectBlock(unsigned char *connect_block);
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
  int function, rack_slot;
  int plc_type;
  int fetch_write;
  unsigned char pdu[2048];
  int use_cb;
  unsigned char cb[22];
};

#endif
