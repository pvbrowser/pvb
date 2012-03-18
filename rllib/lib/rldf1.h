/***************************************************************************
                          rldf1.cpp  -  description
                             -------------------

This Class implements basic functions of DF1 Full Duplex protocall as
described in 1770-6.5.16 Publication of AB.

This Class implements:
    - Data Link Layer ( Message Frames )
    - Application Layer ( Message Packets )
    - Some Basic Communication Commands for reading and writting PLC files.
      It is easy to add more Communication Commands.

Some basic knowledge about the related PLCs is required.
The class supports only serial communication through RS232 port.

DANGER: DON'T connect to a PLC unless you are certain it is safe to do so!!!
You are ABSOLUTELY RESPONSIBLE if you affect the operation of a running PLC.

Evangelos Arkalis
mail:arkalis.e@gmail.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

#ifndef _RL_DF1_H_
#define _RL_DF1_H_

#include <rlserial.h>
class df1Buffer;

class rlDF1
{
public:

    enum DF1ret {
        ERROR_TNS      = -6,
        ERROR_STS      = -5,
        ERROR_CRC      = -4,
        ERROR_NAC      = -3,
        ERROR          = -2,
        NORESPONSE     = -1,
        SUCCESS        = 0
    };


/*! <pre>
    Initialize class
    src = id of local device
    timeout = receive timeout in ms
</pre> */
    rlDF1(unsigned char src=0, int timeout=1000);
    virtual ~rlDF1();
    void registerSerial(rlSerial *serial);

/*! <pre>
      Command: Set CPU mode ( See page 7-26 )
      destination = destination ID
      mode = CPUMODE value
</pre> */
    enum CPUMODE {
        PROGRAM = 0,
        TEST,
        RUN,
        NOCHANGE
    };
    int cmdSetCPUMode(unsigned char destination, unsigned char mode);
/*! <pre>
        Command: Diagnostic Status
        Reads a block of status information from an interface module.
        The function returns the status information in data.
        (see Chapter 10, “Diagnostic Status Information.”)

        destination = destination ID
        data = buffer for data return
        returns the number of valid bytes in data or Error Code
</pre> */
    int  cmdDiagnosticStatus( unsigned char destination, unsigned char *data);
/*! <pre>
        Command: protected typed logical read with three address fields
        *Most Important command*. Reads data from a logical address.
        Parameters:
            destination = destination ID
            nsize = size of data to be read
            filetype = Type of file with number filenum
                            84 hex: status
                            85 hex: bit
                            86 hex: timer
                            87 hex: counter
                            88 hex: control
                            89 hex: integer
                            8A hex: floating point
                            8B hex: output logical by slot
                            8C hex: input logical by slot
                            8D hex: string
                            8E hex: ASCII
                            8F hex: BCD
            filenum = filenumber in PLC memory
            adr  = Elements Address (see Manual)
            sadr = Subelements Address (see Manual)
            data = buffer for data return
        returns the number of valid bytes in data or Error Code
</pre> */
    int  cmdLogicalRead( unsigned char destination, unsigned char nsize, unsigned char filetype, unsigned char filenum, unsigned char adr, unsigned char sadr, unsigned char *buffer);
/*! <pre>
        Command: protected typed logical write with three address fields
        *Most Important command*. Writes data to a logical address.
        Parameters:
            destination = destination ID
            nsize = size of data to write
            filetype = Type of file with number filenum
                            84 hex: status
                            85 hex: bit
                            86 hex: timer
                            87 hex: counter
                            88 hex: control
                            89 hex: integer
                            8A hex: floating point
                            8B hex: output logical by slot
                            8C hex: input logical by slot
                            8D hex: string
                            8E hex: ASCII
                            8F hex: BCD
            filenum = filenumber in PLC memory
            adr  = Elements Address (see Manual)
            sadr = Subelements Address (see Manual)
            data = buffer of data to write
        returns Error Code
</pre> */
    int  cmdLogicalWrite( unsigned char destination, unsigned char nsize, unsigned char filetype, unsigned char filenum, unsigned char adr, unsigned char sadr, unsigned char *buffer);
/*! <pre>
    Some statistics...
</pre> */
    int  requests() {return nRequests;};
    int  responses() {return nResponses;};
    void clearStats() { nRequests=0; nResponses=0;};
private:
/*! <pre>
      Functions for Application Layer.
      Normally private. If you want to add new Commands do it with new public functions.

      sendCommand implements sending with retries and handling
        destination = id of remote device
        cmd = Command Code (see page 6-3)
        sts = Status Code (see page 6-6 )
        cdata = Pointer to Command Specific data packet ( Chapter 7 )
        len = Length of cdata

      receiveAnswer gets answer frame from remote.

</pre> */
    int  sendCommand( unsigned char destination, unsigned char cmd, unsigned char sts, unsigned char *cdata, unsigned char len);
    int  receiveAnswer( unsigned char &destination, unsigned char &cmd, unsigned char &sts, unsigned char *cdata, unsigned char &len);
/*! <pre>
      Functions for Data Link Layer.
      Internal Use ONLY.
</pre> */
    enum DF1BytePos {
        POS_DST         =0,
        POS_SRC         =1,
        POS_CMD         =2,
        POS_STS         =3,
        POS_TNSL        =4,
        POS_TNSH        =5,
        POS_DATA        =6
    };
    enum DF1ControlChars {      /// Page 2-6 of Ref Manual
        SOH = 0x01,
        STX = 0x02,
        ETX = 0x03,
        EOT = 0x04,
        ENQ = 0x05,
        ACK = 0x06,
        DLE = 0x10,
        NAC = 0x15
    };
    enum DF1Flags {
        FLAG_TIMEOUT         =-1,
        FLAG_DATA            =0,
        FLAG_CONTROL         =1
    };
    int writeBuffer( unsigned char *buffer, int len );
    int writeBuffer( df1Buffer& buffer );
    int getSymbol(unsigned char *c);
    int get(unsigned char *c);
    int sendENQ();
    int sendACK();
    int sendNAC();


    rlSerial *tty;
    bool     active;
    unsigned short tns;

    int              nRequests;
    int              nResponses;
    unsigned char   source;
    int              timeout;
};


#endif // _RL_DF1_H_
