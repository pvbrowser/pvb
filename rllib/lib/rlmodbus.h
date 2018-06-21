/***************************************************************************
                          rlmodbus.h  -  description
                             -------------------
    begin                : Tue Mar 13 2003
    copyright            : (C) 2003 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_MODBUS_H_
#define _RL_MODBUS_H_

#include "rldefine.h"
#include "rlsocket.h"
#include "rlserial.h"

/*! <pre>
This class implements the modbus protocol.
You can use serial interfaces or TCP/IP.
Modbus RTU and ASCII are available.

All Modbus requests include "slave" and "function".
Then some bytes follow, which are specific to a given function.
The request is then terminated by a checksum.

This table shows the bytes that are specific to a given function.
</pre>

<TABLE CELLSPACING="0" COLS="4" BORDER="0">
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP><B>Function</B></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><B>Query</B></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><B>Response</B></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><B><BR></B></TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="62" ALIGN="LEFT" VALIGN=TOP>01 Read Coil Status</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Start adr high<BR>Start adr low<BR>Number of points high<BR>Number of points low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Data Byte Count<BR>Data1<BR>Data2 …</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>8 points per byte</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="62" ALIGN="LEFT" VALIGN=TOP>02 Read Input Status</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Start adr high<BR>Start adr low<BR>Number of points high<BR>Number of points low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Data Byte Count<BR>Data1 <BR>Data2 …</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>8 points per byte</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="77" ALIGN="LEFT" VALIGN=TOP>03 Read Holding Registers</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Start adr high<BR>Start adr low<BR>Number of points high<BR>Number of points low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Data Byte Count<BR>Data1 high<BR>Data1 low<BR>Data2 high<BR>Data2 low…</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>1 point needs 2 bytes</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="77" ALIGN="LEFT" VALIGN=TOP>04 Read Input Registers</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Start adr high<BR>Start adr low<BR>Number of points high<BR>Number of points low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Data Byte Count<BR>Data1 high<BR>Data1 low<BR>Data2 high<BR>Data2 low…</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>1 point needs 2 bytes</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="62" ALIGN="LEFT" VALIGN=TOP>05 Force Single Coil</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Coil adr high<BR>Coil adr low<BR>Force data high<BR>Force data low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Coil adr high<BR>Coil adr low<BR>Force data high<BR>Force data low </TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Force data ON = 0x0ff00<BR>Force data OFF = 0</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="62" ALIGN="LEFT" VALIGN=TOP>06 Preset Single Register</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Register adr high<BR>Register adr low<BR>Preset data high<BR>Preset data low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Register adr high<BR>Register adr low<BR>Preset data high<BR>Preset data low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>07 Read Exception Status</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Coil data</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>8 exception status coils returned</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="62" ALIGN="LEFT" VALIGN=TOP>11 Fetch Comm Event Counter</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Status high<BR>Status low<BR>Event Count high<BR>Event Count low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>12 Fetch Comm Event Log</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>See: PI_MODBUS_300.pdf</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="136" ALIGN="LEFT" VALIGN=TOP>15 Force Multiple Coils</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Coil adr high<BR>Coil adr low<BR>Number of coils high<BR>Number of coils low<BR>Force data byte count<BR>Force data1<BR>Force data2 ...</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Coil adr high<BR>Coil adr low<BR>Number of coils high<BR>Number of coils low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>8 coils per byte</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="136" ALIGN="LEFT" VALIGN=TOP>16 Preset Multiple Registers</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Start adr high<BR>Start adr low<BR>Number of registers high<BR>Number of registers low<BR>Data byte count<BR>Data1 high<BR>Data1 low<BR>Data2 high<BR>Data2 low …</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Start adr high<BR>Start adr low<BR>Number of registers high<BR>Number of registers low</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>17 Report Slave ID</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>Data Byte count ~ device specific</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>See: PI_MODBUS_300.pdf</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>20 Read General Reference</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>See: PI_MODBUS_300.pdf</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>21 Write General Reference</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>See: PI_MODBUS_300.pdf</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>22 Mask Write 4X Register</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>See: PI_MODBUS_300.pdf</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>23 Read/Write 4X Registers</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>See: PI_MODBUS_300.pdf</TD>
	</TR>
	<TR>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" HEIGHT="17" ALIGN="LEFT" VALIGN=TOP>24 Read FIFO Queue</TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP><BR></TD>
		<TD STYLE="border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000" ALIGN="LEFT" VALIGN=TOP>See: PI_MODBUS_300.pdf</TD>
	</TR>
</TABLE>

*/

class rlModbus
{
  public:
/*! <pre>
data exchanged by Modbus is either: 
1byte unsigned for coils 
2byte unsigned for registers

Helper union for converting Modbus data to data types that are not standard Modbus data types
Example:
rlModbus::DATA data;
data.u_short[0] = registers[0]; // store an unsigned 16 bit register in union data
printf("print registers[0] as signed int value=%d\n", data.s_short[0]);
</pre> */
    typedef union
    {
      unsigned char  u_char[4];  // 4 * 8bit_data,  unsigned
      char           s_char[4];  // 4 * 8bit_data,  signed
      unsigned short u_short[2]; // 2 * 16bit_data, signed
      short          s_short[2]; // 2 * 16bit_data, signed
      unsigned int   u_int;      // 1 * 32bit_data, unsigned
      int            s_int;      // 1 * 32bit_data, signed
      float          s_float;    // 1 * 32bit_data, signed
    }DATA;

    enum Modbus
    {
      MODBUS_CHECKSUM_ERROR = -2,
      MODBUS_ERROR          = -1,
      MODBUS_SUCCESS        = 0,
      MODBUS_RTU            = 1,
      MODBUS_ASCII          = 2
    };

    enum ModbusFunctionCodes
    {
      ReadCoilStatus         = 1,
      ReadInputStatus        = 2,
      ReadHoldingRegisters   = 3,
      ReadInputRegisters     = 4,
      ForceSingleCoil        = 5,
      PresetSingleRegister   = 6,
      ReadExceptionStatus    = 7,
      FetchCommEventCtr      = 11,
      FetchCommEventLog      = 12,
      ForceMultipleCoils     = 15,
      PresetMultipleRegs     = 16,
      ReportSlaveID          = 17,
      ReadGeneralReference   = 20,
      WriteGeneralReference  = 21,
      MaskWrite4XRegisters   = 22,
      ReadWrite4XRegisters   = 23,
      ReadFifoQueue          = 24
    };

    rlModbus(long max_telegram_length = 1024, int mode = MODBUS_RTU, char end_delimitor = 0x0a);
    virtual ~rlModbus();
    int  write   (int  slave, int  function, const unsigned char *data, int len, int *transactionID = NULL);
    int  request (int  slave, int  function, int start_adr, int num_register);
    int  response(int *slave, int *function, unsigned char *data, int timeout=1000);
    int  readRequest(int *slave, int *function, unsigned char *data, int timeout=1000, int *transactionID = NULL);
    void registerSocket(rlSocket *socket);
    void registerSerial(rlSerial *serial);
    int  data2int(const unsigned char *data);
    int  int2data(int val, unsigned char *data);
    int  intsize();
    int  autoreconnectSocket;
    int  readCoilStatus          (int slave, int start_adr,    int number_of_coils,     unsigned char *status, int timeout=1000);
    int  readInputStatus         (int slave, int start_adr,    int number_of_inputs,    unsigned char *status, int timeout=1000);
/*! <pre>
We assume positive values for registers: 0 <= registers < 256*256
</pre> */
    int  readHoldingRegisters    (int slave, int start_adr,    int number_of_registers, int *registers,        int timeout=1000);
/*! <pre>
We assume positive values for registers: 0 <= registers < 256*256
</pre> */
    int  readInputRegisters      (int slave, int start_adr,    int number_of_registers, int *registers,        int timeout=1000);
    int  forceSingleCoil         (int slave, int coil_adr,     int value,                                      int timeout=1000);
/*! <pre>
We assume positive values for registers: 0 <= value < 256*256
</pre> */
    int  presetSingleRegister    (int slave, int register_adr, int value,                                      int timeout=1000);
/*! <pre>
We assume positive values for registers: 0 <= registers < 256*256
</pre> */
    int  forceMultipleCoils      (int slave, int coil_adr,     int number_of_coils,     unsigned char *coils,  int timeout=1000);
/*! <pre>
We assume positive values for registers: 0 <= registers < 256*256
</pre> */
    int  presetMultipleRegisters (int slave, int start_adr,    int number_of_registers, int *registers,        int timeout=1000);

  private:
    int  buf2int_rtu(unsigned char *buf);
    void int2buf_rtu(int i, unsigned char *buf);
    int  buf2int_ascii(unsigned char *buf);
    void int2buf_ascii(int i, unsigned char *buf);
    void insertLRC(int len);
    void insertCRC(int len);
    int  LRCerror(int len);
    int  CRCerror(int len);
    rlSocket *s;
    rlSerial *tty;
    unsigned char *tel;
    long     maxtel;
    int      mode;
    char     delimitor;
};

#endif

