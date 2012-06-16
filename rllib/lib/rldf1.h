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

#include "rlserial.h"
class df1Buffer;

class rlDF1
{
public:

/*! <pre>
    Initialize class
    src = id of local device
    timeout = receive timeout in ms
</pre> */
    rlDF1(unsigned char src=0, int timeout=1000);
    virtual ~rlDF1();
    void registerSerial(rlSerial *serial);

    enum DF1ret {
        retERROR_TNS      = -6,
        retERROR_STS      = -5,
        retERROR_CRC      = -4,
        retERROR_NAC      = -3,
        retERROR          = -2,
        retNORESPONSE     = -1,
        retSUCCESS        = 0
    };


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

public:
/*! <pre>
    Some statistics...
</pre> */
    int  requests() {return nRequests;};
    int  responses() {return nResponses;};
    void clearStats() { nRequests=0; nResponses=0;};
};


#endif // _RL_DF1_H_

/*
// test program for rlDF1
#include <rldf1.h>

// Remote ID
const unsigned char id=1;

rlSerial ser;
rlDF1 myplc;


int main(int argc, char *argv[])
{
    int ret;
    if ( ser.openDevice( "/dev/ttyS0", B19200, 0, 0 )!= 0 ) exit(-1);
    printf("\nDevice OK");
    myplc.registerSerial( &ser );               /// Register Serial Port


    printf("\n1.TEST CONNECTIVITY");
    ret = myplc.cmdSetCPUMode( id, rlDF1::NOCHANGE );     /// For Connectivity Test
    if (ret<0) {
        printf("\nDevice Error %d\n",ret);
        exit(-1);
    }
    rlsleep(3000);


    printf("\n2.SET PLC IN PROGRAM MODE");
    ret = myplc.cmdSetCPUMode( id, rlDF1::PROGRAM );     /// PROGRAM MODE
    if (ret<0) {
        printf("\nDevice Error %d\n",ret);
        exit(-1);
    }
    rlsleep(3000);


    printf("\n3.SET PLC IN RUN MODE");
    ret = myplc.cmdSetCPUMode( id, rlDF1::RUN );     /// RUN MODE
    if (ret<0) {
        printf("\nDevice Error %d\n",ret);
        exit(-1);
    }
    rlsleep(3000);


    printf("\n4.GET PLC STATUS BYTES (Read Manual for details)");
    unsigned char rbytes[256];
    ret = myplc.cmdDiagnosticStatus( id, rbytes );
    if (ret<0) {
        printf("\nDevice Error %d\n",ret);
        exit(-1);
    }
    for(int i=0;i<ret;i++) printf("\n%d -> \t%d \t%02X", i, rbytes[i], rbytes[i]);
    printf("\n");
    rlsleep(3000);


    printf("\n5.LOGICAL READ - Read 2 bytes from Inputs - I1:0");
    ret = myplc.cmdLogicalRead( id, 2, 0x8C, 1, 0, 0, rbytes );  /// 0x8C:Read Inputs by Slot: 2 bytes from Data File I1, starting at Address 0 [ I1:0 ]
    if (ret<0) {
        printf("\nDevice Error %d\n",ret);
        exit(-1);
    }
    for(int i=0;i<ret;i++) printf("\n%d -> \t%d \t%02X", i, rbytes[i], rbytes[i]);
    printf("\n");
    rlsleep(3000);


    printf("\n6.LOGICAL WRITE - Write 2 bytes to Outputs - Q0:0 > CYCLE FROM 0.0 to 0.7, RESET 0.8-0.15\n");
    for(int i=0;i<8;i++) {
        printf("SET Q0:0.%d",i);
        rbytes[0]=0x01<<i;
        rbytes[1]=0;
        ret = myplc.cmdLogicalWrite( id, 2, 0x8B, 0, 0, 0, rbytes );  /// 0x8B:Write Outputs by Slot: 2 bytes to Data File Q0, starting at Address 0 [ Q0:0 ]
        if (ret<0) {
            printf("\nDevice Error %d\n",ret);
            exit(-1);
        }
        printf("\n");
        rlsleep(1000);
    }
    printf("\n7.LOGICAL WRITE - Write 2 bytes to Outputs - Q0:0 > RESET 0.0-0.15");
    rbytes[0]=0;    /// Reset Outputs 0-15
    rbytes[1]=0;
    ret = myplc.cmdLogicalWrite( id, 2, 0x8B, 0, 0, 0, rbytes );  /// 0x8B:Write Outputs by Slot: 2 bytes to Data File Q0, starting at Address 0 [ Q0:0 ]
    if (ret<0) {
        printf("\nDevice Error %d\n",ret);
        exit(-1);
    }
    printf("\nDuring execution of this program, we had %d Requests and %d Responses to/from PLC\n", myplc.requests(), myplc.responses());


    ser.closeDevice();
}
*/
