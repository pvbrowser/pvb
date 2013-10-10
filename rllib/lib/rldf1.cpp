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

#include "rldf1.h"
#include <time.h>

//#define DF1_DEBUG


#ifdef DF1_DEBUG
#define DBGPRINTF(x...) {printf(x);fflush(stdout);}
#else
#define DBGPRINTF(x...)
#endif
/*
    df1Buffer
    Simple buffer with DLE encoding for DF1 implementation
*/
class df1Buffer
{
    public:
        df1Buffer(unsigned int maxsize=256);
        virtual ~df1Buffer();
    protected:
    private:
        unsigned int maxsize;
        unsigned int len;
        unsigned char *dat;
    public:    
        unsigned int length() {return len;}
        unsigned char *data() {return dat;}
        unsigned int write( unsigned char c);
        unsigned int writeDLE( unsigned char c);
        unsigned char& operator [] (unsigned int pos);
        void reset() {len=0;}
        void print();
};
df1Buffer::df1Buffer( unsigned int _maxsize)
{
    dat = new unsigned char[_maxsize];
    maxsize = _maxsize;
    len=0;
}

df1Buffer::~df1Buffer()
{
    delete [] dat;
}

unsigned int df1Buffer::write( unsigned char c)
{
    if (len+1<maxsize) {
        dat[len] = c;
        len++;
    }
    return len;
}

unsigned int df1Buffer::writeDLE( unsigned char c)
{
    if (len+1<maxsize) {
        if (c==0x10) {
            dat[len]=c;
            len++;
        }
        dat[len] = c;
        len++;
    }
    return len;
}

unsigned char& df1Buffer::operator [] (unsigned int pos)
{
    if ( pos>=len ) {
        printf("*** rlBuffer read error! Pos:%u Len:%u\n", pos,len);
    }
    return dat[pos];
}

void df1Buffer::print()
{
    printf("Buffer [%d]:",len);
    if(len>0) {
        for(int i=0; i<(int)len; i++) printf(" %02X", (unsigned int)dat[i]);
    }
    printf("\n");
}
/*-----------------------------------------------------------------------------------*/
// CRC COMPUTING
static unsigned short calcCRC (unsigned short crc, unsigned short buffer)
{
    unsigned short temp1, y;
    temp1 = crc ^ buffer;
    crc = (crc & 0xff00) | (temp1 & 0xff);
    for (y = 0; y < 8; y++) {
        if (crc & 1) {
            crc = crc >> 1;
            crc ^= 0xa001;
        } else
            crc = crc >> 1;
    }
    return crc;
}

static unsigned short computeCRC (const unsigned char *buffer, int len)
{
    unsigned short crc = 0;
    for (int x=0; x<len; x++) {
        crc = calcCRC(crc, (unsigned short)buffer[x] );
    }
    crc = calcCRC (crc,0x03); // ETX
    return (crc);
}

/*-----------------------------------------------------------------------------------*/

rlDF1::rlDF1( unsigned char src, int _timeout )
{
    source = src;
    timeout=_timeout;
    tty = NULL;
    active = false;
    nRequests = 0;
    nResponses = 0;
    tns = (unsigned short) time((time_t *)0);           /// Get a random TNS
}

rlDF1::~rlDF1()
{
}

void rlDF1::registerSerial(rlSerial *serial)
{
    tty = serial;
}

int rlDF1::writeBuffer( unsigned char *buffer, int len )
{
    int ret=-1;
    if ( tty!=NULL) {
        ret = tty->writeBlock( buffer, len );
    }
    return ret;
}

int rlDF1::writeBuffer( df1Buffer& buffer )
{
    int ret=-1;
    if ( tty!=NULL) {
        ret = tty->writeBlock( buffer.data(), buffer.length() );
    }
    return ret;
}


int rlDF1::getSymbol(unsigned char *c)
{

    int ret = tty->readBlock( c, 1, timeout);
    if (ret<=0) return FLAG_TIMEOUT;
    if (*c!=DLE) {
        return FLAG_DATA;
    } else {
        ret = tty->readBlock( c, 1, timeout);
        if (ret<=0) return FLAG_TIMEOUT;
        if (*c==DLE) {
            return FLAG_DATA;
        } else {
            return FLAG_CONTROL;
        }

    }
}

int rlDF1::get(unsigned char *c)
{

    int ret = tty->readBlock( c, 1, timeout);
    if (ret<=0) return FLAG_TIMEOUT;
    return FLAG_DATA;
}

int rlDF1::sendENQ()
{
    static unsigned char buf[2];
    DBGPRINTF("\nsendENQ()");
    buf[0] = DLE;
    buf[1] = ENQ;
    return writeBuffer( buf, 2 );
}

int rlDF1::sendACK()
{
    static unsigned char buf[2];
    DBGPRINTF("\nsendACK()");
    buf[0] = DLE;
    buf[1] = ACK;
    return writeBuffer( buf, 2 );
}

int rlDF1::sendNAC()
{
    static unsigned char buf[2];
    DBGPRINTF("\nsendNAC()");
    buf[0] = DLE;
    buf[1] = NAC;
    return writeBuffer( buf, 2 );
}



int rlDF1::cmdSetCPUMode(unsigned char destination, unsigned char mode)
{

    unsigned char cdat[256];
    cdat[0]=0x3A;
    cdat[1]=mode;
    int ret = sendCommand( destination, 0x0F, 0x00, cdat, 2);
    if (ret==retSUCCESS) {
        unsigned char dest,cmd,sts,len;
        ret = receiveAnswer( dest, cmd, sts, cdat, len);
        if (sts!=0) {
            printf("\nSet CPU Mode Execution Error! STS:%02X",sts);
        }
    }
    return ret;
}

int rlDF1::cmdDiagnosticStatus( unsigned char destination, unsigned char *buffer)
{
    unsigned char cdat[256];
    cdat[0]=0x03;
    int ret = sendCommand( destination, 0x06, 0x00, cdat, 1);
    if (ret==retSUCCESS) {
        unsigned char dest,cmd,sts,len;
        ret = receiveAnswer( dest, cmd, sts, cdat, len);
        if ( ret == retSUCCESS ) {
            for (int i=0;i<len;i++) buffer[i]=cdat[i];
            ret = len;
        }
    }
    return ret;
}

int  rlDF1::cmdLogicalRead( unsigned char destination, unsigned char nsize, unsigned char filetype, unsigned char filenum, unsigned char adr, unsigned char sadr, unsigned char *buffer)
{
    unsigned char cdat[256];
    cdat[0]=0xA2;
    cdat[1]=nsize;
    cdat[2]=filenum;
    cdat[3]=filetype;
    cdat[4]=adr;
    cdat[5]=sadr;
    int ret = sendCommand( destination , 0x0F, 0x00, cdat, 6);
    if (ret==retSUCCESS) {
        unsigned char dest,cmd,sts,len;
        ret = receiveAnswer( dest, cmd, sts, cdat, len);
        if ( ret == retSUCCESS ) {
            for (int i=0;i<len;i++) buffer[i]=cdat[i];
            ret = len;
        }
        if (sts!=0) ret = retERROR_STS;
    }
    return ret;
}
int  rlDF1::cmdLogicalWrite( unsigned char destination, unsigned char nsize, unsigned char filetype, unsigned char filenum, unsigned char adr, unsigned char sadr, unsigned char *buffer)
{
    unsigned char cdat[256];
    cdat[0]=0xAA;
    cdat[1]=nsize;
    cdat[2]=filenum;
    cdat[3]=filetype;
    cdat[4]=adr;
    cdat[5]=sadr;
    for (int i=0;i<nsize;i++) cdat[6+i] = buffer[i];
    int ret = sendCommand( destination, 0x0F, 0x00, cdat, 6+nsize);
    if (ret==retSUCCESS) {
        unsigned char dest,cmd,sts,len;
        ret = receiveAnswer( dest, cmd, sts, cdat, len);
        if ( ret == retSUCCESS ) {
            for ( int i=0;i<len;i++) buffer[i] = cdat[i];
            ret = len;
        }
        if (sts!=0) ret = retERROR_STS;
    }
    return ret;
}
//-------------------------------------------------------------------------------


int rlDF1::sendCommand( unsigned char destination, unsigned char cmd, unsigned char sts, unsigned char *cdata, unsigned char len)
{
    df1Buffer msg;
    df1Buffer fullmsg;
    unsigned short crc;

    tns++;
    nRequests++;
    DBGPRINTF("\n\tDF1::sendCommand >");
    DBGPRINTF(" Dest:%02X", destination);
    DBGPRINTF(" CMD:%02X", cmd);
    DBGPRINTF(" STS:%02X", sts);
    DBGPRINTF(" TNS:%04X", tns);
    DBGPRINTF(" DATA [%d]: ",len);
    for (int i=0; i<len; i++) { DBGPRINTF("[%02X] ",cdata[i]); }
    msg.write( destination );
    msg.write( source ); /// SOURCE
    msg.write( cmd );
    msg.write( sts );
    msg.write( (unsigned char)(tns&0x00FF) );
    msg.write( (unsigned char)(tns>>8) );
    for(int i=0; i<len; i++) msg.write( cdata[i] );
    //msg.print();
    crc  = computeCRC( msg.data(), msg.length() );
    DBGPRINTF(" [CRC=%04X] ",crc);
    fullmsg.write( DLE );
    fullmsg.write( STX );
    for(int i=0; i<(int)msg.length(); i++) fullmsg.writeDLE( *(msg.data()+i) );
    fullmsg.write( DLE );
    fullmsg.write( ETX );
    fullmsg.write( (unsigned char)(crc&0x00FF) );
    fullmsg.write( (unsigned char)(crc>>8) );

    // TRANSMIT LOOP
    int naks=0;
    int enq=0;
    int flag;
    unsigned char rxc;
    int ret;
    writeBuffer( fullmsg );
    while(1) {
        flag = getSymbol(&rxc);
        if (( flag==FLAG_CONTROL )&&( rxc == ACK )) {
            DBGPRINTF(" -> ACK RECEIVED");
            ret = retSUCCESS;
            break;
        } else if (( flag==FLAG_CONTROL )&&( rxc == NAC )) {
            naks++;
            DBGPRINTF(" -> NAK RECEIVED:%d",naks);
            if (naks>=3) {
                ret = retERROR_NAC;
                break;
            }
            writeBuffer( fullmsg );
        } else if ( flag == FLAG_TIMEOUT ) {
            enq++;
            DBGPRINTF(" -> TIMEOUT ENQ:%d",enq);
            if (enq>=3) {
                ret = retNORESPONSE;
                break;
            }
            sendENQ();
        }
    }
    return ret;
}

int rlDF1::receiveAnswer( unsigned char &destination, unsigned char &cmd, unsigned char &sts, unsigned char *cdata, unsigned char &len)
{
    int ret;
    static unsigned char response=NAC;
    df1Buffer    msg;
    int flag;
    unsigned char rxc;

    DBGPRINTF("\n\tDF1::receiveAnswer >");
    while(1) {
        flag = getSymbol(&rxc);
        if ( flag == FLAG_TIMEOUT ) {
            DBGPRINTF("\nreceiveAnswer() Timeout....");
            ret = retNORESPONSE;
            break;

        } else  if ( ( flag==FLAG_CONTROL) && ( rxc == STX) ) {

            DBGPRINTF(" DLE STX >")
            msg.reset();
            while ( (flag=getSymbol(&rxc))==FLAG_DATA ) {
                DBGPRINTF("[%02X]", rxc);
                msg.write(rxc);
            }
            if ( rxc != ETX ) {
                DBGPRINTF(" [NO ETX] ");
                sendNAC();
                response = NAC;
                continue;
            }
            DBGPRINTF(" ETX \n\t");
            flag = get(&rxc);         /// CRC LOW
            if ( flag == FLAG_TIMEOUT) {
                sendNAC();
                response = NAC;
                continue;
            }
            DBGPRINTF(" CRCLH:[%02X", rxc);
            unsigned short crc = (unsigned short)rxc;
            flag = get(&rxc);         /// CRC HI
            if ( flag == FLAG_TIMEOUT) {
                sendNAC();
                response = NAC;
                continue;
            }
            DBGPRINTF(":%02X] ", rxc);
            crc += ( (unsigned short)rxc )<<8;
            if ( crc != computeCRC( msg.data(), msg.length() ) ) {
                DBGPRINTF(" [CRC ERROR] ");
                sendNAC();
                response = NAC;
                continue;
            }
            unsigned short tns_ = (unsigned short)msg[POS_TNSL];
            tns_ += ( (unsigned short)msg[POS_TNSH] )<<8;
            DBGPRINTF(" TNS:[%04X] ", tns_);
            if ( tns_ != tns ) {
                DBGPRINTF(" [TNS ERROR] ");
                sendACK();
                response = ACK;
                ret = retERROR_TNS;
                break;
            }
            /* STS is a software error. Handle it in upper layer!
            if ( msg[POS_STS]!=0 ) {
                DBGPRINTF(" [STS=%02X != 0] ",msg[POS_STS]);
                sendACK();
                response = ACK;
                ret = retERROR_STS;
                break;
            }
            */
            destination = msg[POS_DST];
            cmd = msg[POS_CMD];
            sts = msg[POS_STS];
            len = msg.length()-6;
            for (int i=0; i<len; i++) cdata[i] = msg[i + POS_DATA];
            sendACK();
            nResponses++;
            response = ACK;
            ret = retSUCCESS;
            break;
        } else if ( ( flag==FLAG_CONTROL) && ( rxc == ENQ) ) {
            if (response==ACK) sendACK(); else sendNAC();
            ret = retERROR;
            break;
        } else {
            response = NAC;
        }

    }
    return ret;
}
