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

(1) There is the old Fetch/Write protocol from the old S5 PLC (fetch_write=1).

(2) And there is the current Siemens PLC protocol introduced with the S7 series of PLC (fetch_write=0).

According to
http://www.ietf.org/rfc/rfc0905.txt
the client will send a connection request to the PLC after it has establisched a TCP connection().

Here is a example connect_block for a Siemens S7 PLC (CBxx in hex):
CB00= 3, ISO_HEADER_VERSION
CB01= 0, ISO_HEADER_RESERVED
CB02= 0, ISO_HEADER_LENGHT_HIGH
CB03=16, ISO_HEADER_LENGHT_LOW = 22 Byte (hex 16)
CB04=11, Length Indicator Field = 17 dec = 22 byte_total_length - 1 byte_length_indicator - 4 byte_ISO_HEADER
CB05=E0, Connection Request Code (Bits 8-5) 1110=E, Initial Credit Allocation (Bits 4-1) Class 0
CB06= 0, DESTINATION-REF-HIGH
CB07= 0, DESTINATION-REF-LOW
CB08= 0, SOURCE-REF-HIGH
CB09= 1, SOURCE-REF-LOW
CB10= 0, Class and Option
CB11=C1, Identifier: Calling TSAP will follow
CB12= 2, Parameter Length, 2 byte will follow
CB13= 1, Remote TSAP, free to choose on client side       (1=PG,2=OP,3=Step7Basic) suggested
CB14= 0, Remote TSAP, free to choose on client side       (upper_3_bit_is_rack / lower_5_bit_is_slot) suggested
CB15=C2, Identifier: Called TSAP will follow
CB16= 2, Parameter Length, 2 byte will follow
CB17= 1, Local TSAP,  set within Step7 = 1                (1=PG,2=OP,3=Step7Basic)
CB18= 0, Local TSAP,  set within Step7 = 0...connectionN  (upper_3_bit_is_rack / lower_5_bit_is_slot)
CB19=C0, Identifier: Maximum TPDU size will follow
CB20= 1, Parameter Length, 1 byte will follow 
CB21= 9, max 512 octets 

For the different PLC types the connect_block looks as follows:
s7_200  = {3,0,0,16,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,'M','W',0xC2,2,'M','W',0xC0,1,9} 
s7_300  = {3,0,0,16,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,  1,0  ,0xC2,2,  1,2  ,0xC0,1,9} on S7_300 slot of cpu is always 2
s7_400  = {3,0,0,16,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,  1,0  ,0xC2,2,  1,3  ,0xC0,1,9} on S7_400 slot of cpu is always 3
s7_1200 = {3,0,0,16,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,  1,0  ,0xC2,2,  1,0  ,0xC0,1,9} slot may be 0 || 1 and TSAP 03.01 || 10.00
s7_logo = {3,0,0,22,0x11,0xE0,0x00,0x00,0x00,0x01,0x00,0xC1,2,  2,0  ,0xC2,2,  2,0  ,0xC0,1,9} 
For S7_200 and S7_1200 read: (only symbolic access to DB1) 
http://support.automation.siemens.com/WW/llisapi.dll?func=cslib.csinfo&lang=en&objid=21601611&caller=view

According to the Remote TSAP Siemens makes the following statement:
######################################################################################
Remote TSAP (Remote Transport Service Access Point, entfernter Dienstzugangspunkt)
The representation is the same as with the Local TSAP, 
but the second byte has another meaning:
- first Byte: contains a device id (allowed 02 or 03)
02 OS (Operating Station Bedienen und Beobachten)
03 other
suggested: 02
- second Byte: contains the adressing within the SIMATIC S7-CPU,
divided in:
Bit 7 ... 5 Rack (Subsystem) of the S7-CPU
Bit 4 ... 0 Slot of the S7-CPU
Hint: It is suggested to choose the same settings for Byte 1 in Remote and Local TSAP
######################################################################################

When you use our rlSiemensTCP class you can do it as follows:

unsigned char cb[22];
rlSiemensTCP *plc = new rlSiemensTCP(adr);
plc->getDefaultConnectBlock(cb);
cb[13] = 1; // set 1 Byte of Remote TSAP
cb[14] = 0; // set 2 Byte of Remote TSAP
cb[17] = 1; // set Local TSAP of the PLC to the
cb[18] = 0; // configuration done within Step 7 
plc->setConnectBlock(cb);

Now you can read/write the PLC.

The following matrix shows some combinations:
--------------------------------------------
             | CB13    CB14    CB17   CB18
--------------------------------------------
S7_200       | 'M'     'W'      'M'    'W'
--------------------------------------------
S7_300       |  1       0        1      2
--------------------------------------------
S7_400       |  1       0        1      3
--------------------------------------------
S7_1200      |  1       0        1      0
--------------------------------------------
S7_logo 0BA7 |  2       0        2      0
--------------------------------------------

CB17 = (1=PG,2=OP,3=Step7Basic)
CB18 = (upper_3_bit_is_rack / lower_5_bit_is_slot)
Thus the above would be:
A TSAP within Step 7 of 10.00 results in: cb[17] = PG; cb[18] = 0; // rack=0 slot=0 
A TSAP within Step 7 of 10.01 results in: cb[17] = PG; cb[18] = 1; // rack=0 slot=1
A TSAP within Step 7 of 10.02 results in: cb[17] = PG; cb[18] = 2; // rack=0 slot=2
A TSAP within Step 7 of 10.03 results in: cb[17] = PG; cb[18] = 3; // rack=0 slot=3

You may use rlSiemensTCP with the individual plc_type for conveniance.
But you can set the whole connect_block and use ANY_SIEMENS_COMPATIBLE_PLC.

Please use Wireshark or tcpdump if the settings of the above matrix do not work for you.
Send us your results.

PS: Still wondering about 'M' 'W' on S7-200
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
    ANY_SIEMENS_COMPATIBLE_PLC = 1000,
    S7_200    = 1,
    S7_300    = 2,
    S7_400    = 3,
    S5        = 4,
    RACK_SLOT = 5,
    S7_1200   = 6,  // patch from user slammera from our forum (8 Nov 2012)
    LOGO      = 7   // LOGO! 0BA7, according to jjmg_engenharia from our forum (3 Sep 2013)
  };
  enum SiemensFunctionCodes
  {
    WriteBit   = 1,
    WriteByte  = 2
  };
  rlSiemensTCP(const char *adr, int _plc_type=rlSiemensTCP::ANY_SIEMENS_COMPATIBLE_PLC, int _fetch_write = 1, int function = -1, int rack_slot = -1);
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
