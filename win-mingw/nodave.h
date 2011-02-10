/*
 Part of Libnodave, a free communication libray for Siemens S7 200/300/400 via
 the MPI adapter 6ES7 972-0CA22-0XAC
 or  MPI adapter 6ES7 972-0CA23-0XAC
 or  TS adapter 6ES7 972-0CA33-0XAC
 or  MPI adapter 6ES7 972-0CA11-0XAC,
 IBH-NetLink or CPs 243, 343 and 443
 
 (C) Thomas Hergenhahn (thomas.hergenhahn@web.de) 2002..2004

 Libnodave is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 Libnodave is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Libnodave; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#ifdef CPLUSPLUS
extern "C" {
#endif

#ifndef __nodave
#define __nodave

#ifdef LINUX
#define DECL2
#define EXPORTSPEC
typedef struct {
    int rfd;
    int wfd;
} _daveOSserialType;
#include <stdlib.h>
#else    
#ifdef CYGWIN
typedef struct {
    int rfd;
    int wfd;
} _daveOSserialType;
#include <stdlib.h>
#else    
#ifdef BCCWIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
//#define DECL2 WINAPI
#define DECL2 
#define EXPORTSPEC
/*  
  #ifdef DOEXPORT
  #define EXPORTSPEC __declspec (dllexport)
  #else
  #define EXPORTSPEC __declspec (dllimport)
  #endif
*/  
typedef struct {
    HANDLE rfd;
    HANDLE wfd;
} _daveOSserialType;
#else
#error Fill in what you need for your OS or API.
#endif
#endif
#endif

/*
    Define this if your system doesn't have byteswap.h or if you experience difficulties with
    the inline functions
*/

/* this is now done in the Makefile: */
/* #define LINUX */

/* #include "log.h" */
/*
    some frequently used ASCII control codes:
*/
#define DLE 0x10
#define ETX 0x03
#define STX 0x02
#define SYN 0x16
#define NAK 0x15
/*
    Protocol types to be used with newInterface:
*/
#define daveProtoMPI  0 /* MPI for S7 300/400 */
#define daveProtoMPI2 1 /* MPI for S7 300/400, "Andrew's version" */
#define daveProtoMPI3 2 /* MPI for S7 300/400, Step 7 Version, not yet implemented */
#define daveProtoPPI  10  /* PPI for S7 200 */

#define daveProtoISOTCP 122 /* ISO over TCP */
#define daveProtoISOTCP243 123  /* ISO over TCP with CP243 */

#define daveProtoMPI_IBH 223  /* MPI with IBH NetLink MPI to ethernet gateway */
#define daveProtoPPI_IBH 224  /* PPI with IBH NetLink PPI to ethernet gateway */

/*
 *    ProfiBus speed constants:
*/
#define daveSpeed9k     0
#define daveSpeed19k    1
#define daveSpeed187k   2
#define daveSpeed500k   3
#define daveSpeed1500k  4
#define daveSpeed45k    5
#define daveSpeed93k    6

/*
    Some MPI function codes (yet unused ones may be incorrect).
*/
#define daveFuncOpenS7Connection  0xF0
#define daveFuncRead      0x04
#define daveFuncWrite     0x05
#define daveFuncStartUpload   0x1D
#define daveFuncUpload      0x1E
#define daveFuncEndUpload   0x1F
/*
    S7 specific constants:
*/
#define daveBlockType_OB  '8'
#define daveBlockType_DB  'A'
#define daveBlockType_SDB 'B'
#define daveBlockType_FC  'C'
#define daveBlockType_SFC 'D'
#define daveBlockType_FB  'E'
#define daveBlockType_SFB 'F'
/*
    Use these constants for parameter "area" in daveReadBytes and daveWriteBytes
*/    
#define daveSysInfo 0x3   /* System info of 200 family */
#define daveSysFlags  0x5 /* System flags of 200 family */
#define daveAnaIn  0x6    /* analog inputs of 200 family */
#define daveAnaOut  0x7   /* analog outputs of 200 family */

#define daveInputs 0x81    
#define daveOutputs 0x82    
#define daveFlags 0x83
#define daveDB 0x84 /* data blocks */
#define daveDI 0x85 /* not tested */
#define daveLocal 0x86  /* not tested */
#define daveV 0x87  /* don't know what it is */
#define daveCounter 28
#define daveTimer 29  
#define daveCounter200 30
#define daveTimer200 31 

/**
    Library specific:
**/
/*
    Result codes. Genarally, 0 means ok, 
    >0 are results (also errors) reported by the PLC
    <0 means error reported by library code.
*/
#define daveResOK 0       /* means all ok */
#define daveResMultipleBitsNotSupported 6   /* CPU tells it does not support to read a bit block with a */
            /* length other than 1 bit. */
#define daveResItemNotAvailable200 3    /* means a a piece of data is not available in the CPU, e.g. */
            /* when trying to read a non existing DB or bit bloc of length<>1 */
            /* This code seems to be specific to 200 family. */
              
#define daveResItemNotAvailable 10    /* means a a piece of data is not available in the CPU, e.g. */
            /* when trying to read a non existing DB */

#define daveAddressOutOfRange 5     /* means the data address is beyond the CPUs address range */
#define daveWriteDataSizeMismatch 7   /* means the write data size doesn't fit item size */
#define daveResCannotEvaluatePDU -123    
#define daveResCPUNoData -124 
#define daveUnknownError -125 
#define daveEmptyResultError -126 
#define daveEmptyResultSetError -127 
/*
    error code to message string conversion:
    Call this function to get an explanation for error codes returned by other functions.
*/
EXPORTSPEC char * DECL2 daveStrerror(int code);

/* 
    Max number of bytes in a single message. 
    An upper limit for MPI over serial is:
    8   transport header
    +2*240  max PDU len *2 if every character were a DLE
    +3    DLE,ETX and BCC
    = 491
    
    Later I saw some programs offering up to 960 bytes in PDU size negotiation    
    
    Max number of bytes in a single message. 
    An upper limit for MPI over serial is:
    8   transport header
    +2*960  max PDU len *2 if every character were a DLE
    +3    DLE,ETX and BCC
    = 1931
    
    For now, we take the rounded max of all this to determine our buffer size. This is ok
    for PC systems, where one k less or more doesn't matter.
*/
#define daveMaxRawLen 2048
/*
    Some definitions for debugging:
*/
#define daveDebugRawRead    0x01  /* Show the single bytes received */
#define daveDebugSpecialChars   0x02  /* Show when special chars are read */
#define daveDebugRawWrite 0x04  /* Show the single bytes written */
#define daveDebugListReachables 0x08  /* Show the steps when determine devices in MPI net */
#define daveDebugInitAdapter  0x10  /* Show the steps when Initilizing the MPI adapter */
#define daveDebugConnect  0x20  /* Show the steps when connecting a PLC */
#define daveDebugPacket   0x40
#define daveDebugByte     0x80
#define daveDebugCompare  0x100
#define daveDebugExchange   0x200
#define daveDebugPDU    0x400 /* debug PDU handling */
#define daveDebugUpload   0x800 /* debug PDU loading program blocks from PLC */
#define daveDebugMPI    0x1000
#define daveDebugPrintErrors  0x2000  /* Print error messages */
#define daveDebugPassive  0x4000


#define daveDebugAll 0xffff
/*
  IBH-NetLink packet types:
*/
#define _davePtEmpty -2
#define _davePtMPIAck -3
#define _davePtUnknownMPIFunc -4
#define _davePtUnknownPDUFunc -5
#define _davePtReadResponse 1
#define _davePtWriteResponse 2


extern EXPORTSPEC int daveDebug;

EXPORTSPEC void DECL2 setDebug(int nDebug);
/*
    Some data types:
*/
#define uc unsigned char
#define us unsigned short
#define u32 unsigned int

/* 
    This is a wrapper for the serial or ethernet interface. This is here to make porting easier.
*/

typedef struct _daveConnection  daveConnection;
typedef struct _daveInterface  daveInterface;

/*
    Helper struct to manage PDUs. This is NOT the part of the packet I would call PDU, but
    a set of pointers that ease access to the "private parts" of a PDU.
*/
typedef struct {
    uc * header;  /* pointer to start of PDU (PDU header) */
    uc * param;   /* pointer to start of parameters inside PDU */
    uc * data;    /* pointer to start of data inside PDU */
    uc * udata;   /* pointer to start of data inside PDU */
    int hlen;   /* header length */
    int plen;   /* parameter length */
    int dlen;   /* data length */
    int udlen;    /* user or result data length */
} PDU;

/*
    Definitions of prototypes for the protocol specific functions. The library "switches" 
    protocol by setting pointers to the protol specific implementations.
*/
typedef int (*_initAdapterFunc) ();
typedef int (*_connectPLCFunc) ();
typedef int (*_disconnectPLCFunc) ();
typedef int (*_disconnectAdapterFunc) ();
typedef int (*_exchangeFunc) (daveConnection *, PDU *);
typedef int (*_sendMessageFunc) (daveConnection *, PDU *);
typedef int (*_getResponseFunc) (daveConnection *);
typedef int (*_listReachablePartnersFunc) (daveInterface * di, char * buf);

/* 
    This groups an interface together with some information about it's properties
    in the library's context.
*/
struct _daveInterface {
    _daveOSserialType fd; /* some handle for the serial interface */
    int users;    /* a counter used when multiple PLCs are accessed via */
      /* the same serial interface and adapter. */
    int localMPI; /* the adapter's MPI address */
    char * name;  /* just a name that can be used in programs dealing with multiple */
      /* daveInterfaces */
    int timeout;  /* Timeout in microseconds used in transort. */
    int protocol; /* The kind of transport protocol used on this interface. */
    int speed;    /* The MPI or Profibus speed */
    int ackPos;   /* position of some packet number that has to be repeated in ackknowledges */
    int nextConnection;
    _initAdapterFunc initAdapter;   /* pointers to the protocol */
    _connectPLCFunc connectPLC;     /* specific implementations */
    _disconnectPLCFunc disconnectPLC;   /* of these functions */
    _disconnectAdapterFunc disconnectAdapter;
    _exchangeFunc exchange;
    _sendMessageFunc sendMessage;
    _getResponseFunc getResponse;
    _listReachablePartnersFunc listReachablePartners;
};

EXPORTSPEC
daveInterface * DECL2 daveNewInterface(_daveOSserialType nfd, char * nname, int localMPI, int protocol, int speed);

/* 
    This is the packet header used by IBH ethernet NetLink. 
*/
typedef struct {
    uc ch1; // logical connection or channel ?
    uc ch2; // logical connection or channel ?
    uc len; // number of bytes counted from the ninth one.
    uc packetNumber;  // a counter, response packets refer to request packets
    us sFlags;    // my guess
    us rFlags;    // my interpretation
} IBHpacket;

/*
    Header for MPI packets on IBH-NetLink:
*/

typedef struct {
    uc src_conn;
    uc dst_conn;
    uc MPI;
    uc localMPI;
    uc len;
    uc func;
    uc packetNumber;
} MPIheader;

typedef struct {
    uc src_conn;
    uc dst_conn;
    uc MPI;
    uc xxx1;
    uc xxx2;
    uc xx22;
    uc len;
    uc func;
    uc packetNumber;
}  MPIheader2;


/* 
    This holds data for a PLC connection;
*/

struct _daveConnection {
    daveInterface * iface; /* pointer to used interface */
    int MPIAdr;   /* The PLC's address */
    int messageNumber;  /* current message number */
    int needAckNumber;  /* message number we need ackknowledge for */
    int AnswLen;  /* length of last message */
    PDU rcvdPDU;
    MPIheader templ;  /* template of MPI Header, setup once, copied in and then modified */
    uc msgIn[daveMaxRawLen];
    uc msgOut[daveMaxRawLen];
    uc * resultPointer; /* used to retrieve single values from the result byte array */
    uc * _resultPointer;
    int PDUstartO;  /* position of PDU in outgoing messages. This is different for different transport methodes. */
    int PDUstartI;  /* position of PDU in incoming messages. This is different for different transport methodes. */
    int rack;   /* rack number for ISO over TCP */
    int slot;   /* slot number for ISO over TCP */
    int maxPDUlength;
    int connectionNumber;
    int connectionNumber2;
    uc  packetNumber; /* packetNumber in transport layer */
}; 

/* 
    Setup a new connection structure using an initialized
    daveInterface and PLC's MPI address.
*/
EXPORTSPEC 
daveConnection * DECL2 daveNewConnection(daveInterface * di, int MPI,int rack, int slot);


typedef struct {
    uc type[2];
    unsigned short count;
} daveBlockTypeEntry;

typedef struct {
    unsigned short number;
    uc type[2];
} daveBlockEntry;

typedef struct {
    uc type[2];
    uc x1[2];  /* 00 4A */
    uc w1[2];  /* some word var? */
    char pp[2]; /* allways 'pp' */
    uc x2[4];  /* 00 4A */
    unsigned short number; /* the block's number */
    uc x3[26];  /* ? */
    unsigned short length; /* the block's length */
    uc x4[16];
    uc name[8];
    uc x5[12];
} daveBlockInfo;
/**
    PDU handling:
    PDU is the central structure present in S7 communication.
    It is composed of a 10 or 12 byte header,a parameter block and a data block.
    When reading or writing values, the data field is itself composed of a data
    header followed by payload data
**/
typedef struct {
    uc P; /* allways 0x32 */
    uc type;  /* Header type, one of 1,2,3 or 7. type 2 and 3 headers are two bytes longer. */
    uc a,b; /* currently unknown. Maybe it can be used for long numbers? */
    us number;  /* A number. This can be used to make sure a received answer */
    /* corresponds to the request with the same number. */
    us plen;  /* length of parameters which follow this header */
    us dlen;  /* length of data which follow the parameters */
    uc result[2]; /* only present in type 2 and 3 headers. This contains error information. */
} PDUHeader;
/*
    set up the header. Needs valid header pointer in the struct p points to.
*/
EXPORTSPEC void DECL2 _daveInitPDUheader(PDU * p, int type);
/*
    add parameters after header, adjust pointer to data.
    needs valid header
*/
EXPORTSPEC void DECL2 _daveAddParam(PDU * p,uc * param,us len);
/*
    add data after parameters, set dlen
    needs valid header,and valid parameters.
*/
EXPORTSPEC void DECL2 _daveAddData(PDU * p,void * data,int len);
/*
    add values after value header in data, adjust dlen and data count.
    needs valid header,parameters,data,dlen
*/
EXPORTSPEC void DECL2 _daveAddValue(PDU * p,void * data,int len);
/*
    add data in user data. Add a user data header, if not yet present.
*/
EXPORTSPEC void DECL2 _daveAddUserData(PDU * p, uc * da, int len);
/*
    set up pointers to the fields of a received message
*/
EXPORTSPEC int DECL2 _daveSetupReceivedPDU(daveConnection * dc,PDU * p);
/*
    Get the eror code from a PDU, if one.
*/
EXPORTSPEC int DECL2 daveGetPDUerror(PDU * p);
/*
    send PDU to PLC and retrieve the answer
*/
EXPORTSPEC int DECL2 _daveExchange(daveConnection * dc,PDU *p);
/*
    retrieve the answer
*/
EXPORTSPEC int DECL2 daveGetResponse(daveConnection * dc);
/*
    send PDU to PLC
*/
EXPORTSPEC int DECL2 daveSendMessage(daveConnection * dc, PDU * p);

/******
    
    Utilities:
    
****/
/*
    Hex dump PDU:
*/
EXPORTSPEC void DECL2 _daveDumpPDU(PDU * p);

/*
    This is an extended memory compare routine. It can handle don't care and stop flags 
    in the sample data. A stop flag lets it return success, if there were no mismatches
    up to this point.
*/
EXPORTSPEC int DECL2 _daveMemcmp(us * a, uc *b, size_t len);

/*
    Hex dump. Write the name followed by len bytes written in hex and a newline:
*/
EXPORTSPEC void DECL2 _daveDump(char * name,uc*b,int len);

/*
    name Objects:
*/
EXPORTSPEC char * DECL2 daveBlockName(uc bn);
EXPORTSPEC char * DECL2 daveAreaName(uc n);

/*
    Data conversion convenience functions:
*/

EXPORTSPEC int DECL2 daveGetByte(daveConnection * dc);

EXPORTSPEC float DECL2 daveGetFloat(daveConnection * dc);

EXPORTSPEC int DECL2 daveGetInteger(daveConnection * dc);

EXPORTSPEC unsigned int DECL2 daveGetDWORD(daveConnection * dc);

EXPORTSPEC unsigned int DECL2 daveGetUnsignedInteger(daveConnection * dc);

EXPORTSPEC unsigned int DECL2 daveGetWORD(daveConnection * dc);

EXPORTSPEC int DECL2 daveGetByteat(daveConnection * dc, int pos);

EXPORTSPEC unsigned int DECL2 daveGetWORDat(daveConnection * dc, int pos);

EXPORTSPEC unsigned int DECL2 daveGetDWORDat(daveConnection * dc, int pos);

EXPORTSPEC float DECL2 daveGetFloatat(daveConnection * dc, int pos);

/*
int daveGetBytesLeft(daveConnection * dc) {
    return ( dc->AnswLen-(int)(dc->resultPointer)+(int)(dc->rcvdPDU.data));
}
*/
EXPORTSPEC float DECL2 toPLCfloat(float ff);

EXPORTSPEC short DECL2 bswap_16(short ff);

EXPORTSPEC int DECL2 bswap_32(int ff);

/**
    Newer conversion routines. As the terms WORD, INT, INTEGER etc have different meanings
    for users of different programming languages and compilers, I choose to provide a new 
    set of conversion routines named according to the bit length of the value used. The 'U'
    or 'S' stands for unsigned or signed.
**/
/*
    Get a value from the position b points to. B is typically a pointer to a buffer that has
    been filled with daveReadBytes:
*/
EXPORTSPEC int DECL2 daveGetS8from(uc *b);
EXPORTSPEC int DECL2 daveGetU8from(uc *b);
EXPORTSPEC int DECL2 daveGetS16from(uc *b);
EXPORTSPEC int DECL2 daveGetU16from(uc *b);
EXPORTSPEC int DECL2 daveGetS32from(uc *b);
EXPORTSPEC unsigned int DECL2 daveGetU32from(uc *b);
EXPORTSPEC float DECL2 daveGetFloatfrom(uc *b);
/*
    Get a value from the current position in the last result read on the connection dc.
    This will increment an internal pointer, so the next value is read from the position
    following this value.
*/
EXPORTSPEC int DECL2 daveGetS8(daveConnection * dc);
EXPORTSPEC int DECL2 daveGetU8(daveConnection * dc);
EXPORTSPEC int DECL2 daveGetS16(daveConnection * dc);
EXPORTSPEC int DECL2 daveGetU16(daveConnection * dc);
EXPORTSPEC int DECL2 daveGetS32(daveConnection * dc);
EXPORTSPEC unsigned int DECL2 daveGetU32(daveConnection * dc);
/*
    Get a value from a given position in the last result read on the connection dc.
*/
EXPORTSPEC int DECL2 daveGetS8at(daveConnection * dc, int pos);
EXPORTSPEC int DECL2 daveGetU8at(daveConnection * dc, int pos);
EXPORTSPEC int DECL2 daveGetS16at(daveConnection * dc, int pos);
EXPORTSPEC int DECL2 daveGetU16at(daveConnection * dc, int pos);
EXPORTSPEC int DECL2 daveGetS32at(daveConnection * dc, int pos);
EXPORTSPEC unsigned int DECL2 daveGetU32at(daveConnection * dc, int pos);
/*
    put one byte into buffer b:
*/
EXPORTSPEC uc * DECL2 davePut8(uc *b,int v);
EXPORTSPEC uc * DECL2 davePut16(uc *b,int v);
EXPORTSPEC uc * DECL2 davePut32(uc *b,int v);
EXPORTSPEC uc * DECL2 davePutFloat(uc *b,float v);
EXPORTSPEC void DECL2 davePut8at(uc *b, int pos, int v);
EXPORTSPEC void DECL2 davePut16at(uc *b, int pos, int v);
EXPORTSPEC void DECL2 davePut32at(uc *b, int pos, int v);
EXPORTSPEC void DECL2 davePutFloatat(uc *b,int pos, float v);
/**
    Timer and Counter conversion functions:
**/
/*  
    get time in seconds from current read position:
*/
EXPORTSPEC float DECL2 daveGetSeconds(daveConnection * dc);
/*  
    get time in seconds from random position:
*/
EXPORTSPEC float DECL2 daveGetSecondsAt(daveConnection * dc, int pos);
/*  
    get counter value from current read position:
*/
EXPORTSPEC int DECL2 daveGetCounterValue(daveConnection * dc);
/*  
    get counter value from random read position:
*/
EXPORTSPEC int DECL2 daveGetCounterValueAt(daveConnection * dc,int pos);

/*
    Functions to load blocks from PLC:
*/
EXPORTSPEC void DECL2 _daveConstructUpload(PDU *p,char blockType, int blockNr);

EXPORTSPEC void DECL2 _daveConstructDoUpload(PDU * p, int uploadID);

EXPORTSPEC void DECL2 _daveConstructEndUpload(PDU * p, int uploadID);
/*
    Get the PLC's order code as ASCIIZ. Buf must provide space for
    21 characters at least.
*/

#define daveOrderCodeSize 21
EXPORTSPEC int DECL2 daveGetOrderCode(daveConnection * dc,char * buf);
/*
    connect to a PLC. returns 0 on success.
*/

EXPORTSPEC int DECL2 daveConnectPLC(daveConnection * dc);
/* 
    Read len bytes from the PLC. Start determines the first byte.
    Area denotes whether the data comes from FLAGS, DATA BLOCKS,
    INPUTS or OUTPUTS. The reading and writing of other data
    like timers and counters is not supported.
    DB is the number of the data block to be used. Set it to zero
    for other area types.
    Buffer is a pointer to a memory block provided by the calling
    program. If the pointer is not NULL, the result data will be copied thereto.
    Hence it must be big enough to take up the result.
    In any case, you can also retrieve the result data using the get<type> macros
    on the connection pointer.
    
    FIXME:  Existence of DB is not checked.
    There is no error message for nonexistent data blocks.
    There is no check for max. message len or 
    automatic splitting into multiple messages.
*/

EXPORTSPEC int DECL2 daveReadBytes(daveConnection * dc, int area, int DB, int start, int len, void * buffer);
/* 
    Write len bytes from buffer to the PLC. 
    Start determines the first byte.
    Area denotes whether the data goes to FLAGS, DATA BLOCKS,
    INPUTS or OUTPUTS. The writing of other data
    like timers and counters is not supported.
    DB is the number of the data block to be used. Set it to zero
    for other area types.
    FIXME:  Existence of DB is not checked.
    There is no error message for nonexistent data blocks.
    There is no check for max. message len or
    automatic splitting into multiple messages.
*/
EXPORTSPEC int DECL2 daveWriteBytes(daveConnection * dc,int area, int DB, int start, int len, void * buffer);

/* 
    Bit manipulation:
*/
EXPORTSPEC int DECL2 daveReadBits(daveConnection * dc, int area, int DB, int start, int len, void * buffer);
EXPORTSPEC int DECL2 daveWriteBits(daveConnection * dc,int area, int DB, int start, int len, void * buffer);
/*
    PLC diagnostic and inventory functions:
*/
EXPORTSPEC int DECL2 daveReadSZL(daveConnection * dc, int ID, int index, void * buf);
EXPORTSPEC int DECL2 daveListBlocksOfType(daveConnection * dc,uc type,daveBlockEntry * buf);
EXPORTSPEC int DECL2 daveListBlocks(daveConnection * dc,daveBlockTypeEntry * buf);
/*
    PLC program read functions:
*/
EXPORTSPEC int DECL2 initUpload(daveConnection * dc,char blockType, int blockNr, int * uploadID);
EXPORTSPEC int DECL2 doUpload(daveConnection*dc, int * more, uc**buffer, int*len, int uploadID);
EXPORTSPEC int DECL2 endUpload(daveConnection*dc, int uploadID);

EXPORTSPEC int DECL2 daveStop(daveConnection*dc);
EXPORTSPEC int DECL2 daveStart(daveConnection*dc);
  
/*
    Multiple variable support:
*/
typedef struct {
    int error;
    int length;
    uc * bytes;
} daveResult;

typedef struct {
    int numResults;
    daveResult * results;
} daveResultSet;


/* use this to initialize a multivariable read: */
EXPORTSPEC void DECL2 davePrepareReadRequest(daveConnection * dc, PDU *p);
/* Adds a new variable to a prepared request: */
EXPORTSPEC void DECL2 daveAddVarToReadRequest(PDU *p, int area, int DBnum, int start, int bytes);
/* Executes the complete request. */
EXPORTSPEC int DECL2 daveExecReadRequest(daveConnection * dc, PDU *p, daveResultSet * rl);
/* Lets the functions daveGet<data type> work on the n-th result: */
EXPORTSPEC int DECL2 daveUseResult(daveConnection * dc, daveResultSet rl, int n);
/* Frees the memory occupied by the result structure */
EXPORTSPEC void DECL2 daveFreeResults(daveResultSet * rl);
/* Adds a new bit variable to a prepared request: */
EXPORTSPEC void DECL2 daveAddBitVarToReadRequest(PDU *p, int area, int DBnum, int start, int byteCount);

/* use this to initialize a multivariable write: */
EXPORTSPEC void DECL2 davePrepareWriteRequest(daveConnection * dc, PDU *p);
/* Adds a new variable to a prepared request: */
EXPORTSPEC void DECL2 daveAddVarToWriteRequest(PDU *p, int area, int DBnum, int start, int bytes, void * buffer);
/* Adds a new bit variable to a prepared write request: */
EXPORTSPEC void DECL2 daveAddBitVarToWriteRequest(PDU *p, int area, int DBnum, int start, int byteCount, void * buffer);
/* Executes the complete request. */
EXPORTSPEC int DECL2 daveExecWriteRequest(daveConnection * dc, PDU *p, daveResultSet * rl);


EXPORTSPEC int DECL2 daveInitAdapter(daveInterface * di);
EXPORTSPEC int DECL2 daveConnectPLC(daveConnection * dc);
EXPORTSPEC int DECL2 daveDisconnectPLC(daveConnection * dc);

EXPORTSPEC int DECL2 daveDisconnectAdapter(daveInterface * di);
EXPORTSPEC int DECL2 daveListReachablePartners(daveInterface * di,char * buf);

EXPORTSPEC int DECL2 _daveReturnOkDummy(void * dummy);
EXPORTSPEC int DECL2 _daveListReachablePartnersDummy(daveInterface * di,char * buf);

EXPORTSPEC int DECL2 _daveNegPDUlengthRequest(daveConnection * dc, PDU *p);

/* MPI specific functions */

#define daveMPIReachable 0x30
#define daveMPIunused 0x10
#define davePartnerListSize 126

EXPORTSPEC int DECL2 _daveListReachablePartnersMPI(daveInterface * di,char * buf);
EXPORTSPEC int DECL2 _daveInitAdapterMPI1(daveInterface * di);
EXPORTSPEC int DECL2 _daveInitAdapterMPI2(daveInterface * di);
EXPORTSPEC int DECL2 _daveConnectPLCMPI1(daveConnection * dc);
EXPORTSPEC int DECL2 _daveConnectPLCMPI2(daveConnection * dc);
EXPORTSPEC int DECL2 _daveDisconnectPLCMPI(daveConnection * dc);
EXPORTSPEC int DECL2 _daveDisconnectAdapterMPI(daveInterface * di);
EXPORTSPEC int DECL2 _daveExchangeMPI(daveConnection * dc,PDU * p1);

EXPORTSPEC int DECL2 _daveListReachablePartnersMPI3(daveInterface * di,char * buf);
EXPORTSPEC int DECL2 _daveInitAdapterMPI3(daveInterface * di);
EXPORTSPEC int DECL2 _daveConnectPLCMPI3(daveConnection * dc);
EXPORTSPEC int DECL2 _daveDisconnectPLCMPI3(daveConnection * dc);
EXPORTSPEC int DECL2 _daveDisconnectAdapterMPI3(daveInterface * di);
EXPORTSPEC int DECL2 _daveExchangeMPI3(daveConnection * dc,PDU * p1);

/* ISO over TCP specific functions */
EXPORTSPEC int DECL2 _daveExchangeTCP(daveConnection * dc,PDU * p1);
EXPORTSPEC int DECL2 _daveConnectPLCTCP(daveConnection * dc);
/*
    make internal PPI functions available for experimental use:
*/
EXPORTSPEC int DECL2 _daveExchangePPI(daveConnection * dc,PDU * p1);
EXPORTSPEC void DECL2 _daveSendLength(daveInterface * di, int len);
EXPORTSPEC void DECL2 _daveSendRequestData(daveConnection * dc, int alt);
EXPORTSPEC void DECL2 _daveSendIt(daveInterface * di, uc * b, int size);
EXPORTSPEC int DECL2 _daveGetResponsePPI(daveConnection *dc);
EXPORTSPEC int DECL2 _daveReadChars(daveInterface * di, uc *b, int tmo, int max);
EXPORTSPEC int DECL2 _daveConnectPLCPPI(daveConnection * dc);

/*
    make internal MPI functions available for experimental use:
*/
EXPORTSPEC int DECL2 _daveReadMPI(daveInterface * di, uc *b);
EXPORTSPEC void DECL2 _daveSendSingle(daveInterface * di, uc c);
EXPORTSPEC int DECL2 _daveSendAck(daveConnection * dc, int nr);
EXPORTSPEC int DECL2 _daveGetAck(daveInterface*di, int nr);
EXPORTSPEC int DECL2 _daveSendDialog2(daveConnection * dc, int size);
EXPORTSPEC int DECL2 _daveSendWithPrefix(daveConnection * dc, uc * b, int size);
EXPORTSPEC int DECL2 _daveSendWithPrefix2(daveConnection * dc, int size);
EXPORTSPEC int DECL2 _daveSendWithCRC(daveInterface * di, uc *b, int size);
EXPORTSPEC int DECL2 _daveReadSingle(daveInterface * di);
EXPORTSPEC int DECL2 _daveReadOne(daveInterface * di, uc *b);
EXPORTSPEC int DECL2 _daveReadMPI2(daveInterface * di, uc *b);
EXPORTSPEC int DECL2 _daveGetResponseMPI(daveConnection *dc);
EXPORTSPEC int DECL2 _daveSendMessageMPI(daveConnection * dc, PDU * p);

/*
    make internal ISO_TCP functions available for experimental use:
*/
/*
    Read one complete packet. The bytes 3 and 4 contain length information.
*/
EXPORTSPEC int DECL2 _daveReadISOPacket(daveInterface * di,uc *b);
EXPORTSPEC int DECL2 _daveGetResponseISO_TCP(daveConnection *dc);


typedef uc * (*userReadFunc) (int , int, int, int, int *);
typedef void (*userWriteFunc) (int , int, int, int, int *,uc *);
extern userReadFunc readCallBack;
extern userWriteFunc writeCallBack;

void _daveConstructReadResponse(PDU * p);
void _daveConstructWriteResponse(PDU * p);
void _daveConstructBadReadResponse(PDU * p);
void _daveHandleRead(PDU * p1,PDU * p2);
EXPORTSPEC void _daveHandleWrite(PDU * p1,PDU * p2);
/*
    make internal IBH functions available for experimental use:
*/
EXPORTSPEC int DECL2 _daveReadIBHPacket(daveInterface * di,uc *b);
EXPORTSPEC int DECL2 _daveWriteIBH(daveInterface * di, void * buffer, int len);
EXPORTSPEC int DECL2 _davePackPDU(daveConnection * dc,PDU *p);
EXPORTSPEC void DECL2 _daveSendIBHNetAck(daveConnection * dc);
EXPORTSPEC int DECL2 _daveExchangeIBH(daveConnection * dc, PDU * p);
EXPORTSPEC int DECL2 _daveConnectPLC_IBH(daveConnection*dc);
EXPORTSPEC int DECL2 _daveDisconnectPLC_IBH(daveConnection*dc);
EXPORTSPEC void DECL2 _daveSendMPIAck2(daveConnection *dc);
EXPORTSPEC int DECL2 _daveGetResponseMPI_IBH(daveConnection *dc);
EXPORTSPEC int DECL2 _daveSendMessageMPI_IBH(daveConnection * dc, PDU * p);
EXPORTSPEC int DECL2 _daveListReachablePartnersMPI_IBH(daveInterface *di, char * buf);

#endif /* _nodave */

#ifdef CPLUSPLUS
 }
#endif


/*
    Changes: 
    07/19/04 added the definition of daveExchange().
    09/09/04 applied patch for variable Profibus speed from Andrew Rostovtsew.
    09/09/04 applied patch from Bryan D. Payne to make this compile under Cygwin and/or newer gcc.
    12/09/04 added daveReadBits(), daveWriteBits()
    12/09/04 added some more comments.
    12/09/04 changed declaration of _daveMemcmp to use typed pointers.
    01/15/04 generic getResponse, more internal functions, use a single dummy to replace 
       initAdapterDummy,
    01/26/05 replaced _daveConstructReadRequest by the sequence prepareReadRequest, addVarToReadRequest
    01/26/05 added multiple write      
    02/02/05 added readIBHpacket
    02/06/05 replaced _daveConstructBitWriteRequest by the sequence prepareWriteRequest, addBitVarToWriteRequest
    02/08/05 removed inline functions. 
*/
