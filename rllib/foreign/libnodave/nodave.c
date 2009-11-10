/*
 Part of Libnodave, a free communication libray for Siemens S7 200/300/400 via
 the MPI adapter 6ES7 972-0CA22-0XAC
 or  MPI adapter 6ES7 972-0CA23-0XAC
 or  MPI adapter 6ES7 972-0CA33-0XAC
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
#include "nodave.h"
#include <stdio.h>
#include "log.h"
#include <string.h>

#ifdef _WIN32
#include <windows.h>
HANDLE WINAPI setPort(char * devname, char* baud,char parity)
{
	HANDLE hComm;
	DCB dcb;
	
       hComm = CreateFile( devname, 
       GENERIC_READ | GENERIC_WRITE,       
       0, 
       0,
       OPEN_EXISTING,
       FILE_FLAG_WRITE_THROUGH,
       0);
	printf("setPort %s\n",devname);
	printf("setPort %s\n",baud);
	printf("setPort %c\n",parity);
	

	printf("Handle to %s opened! %d\n",devname,hComm);
	GetCommState(hComm,&dcb);
	printf("got Comm State. %d\n ",dcb.BaudRate);
	dcb.ByteSize = 8;
	dcb.fOutxCtsFlow=FALSE;
	dcb.fOutxDsrFlow=FALSE;
//	dcb.fDtrControl=DTR_CONTROL_DISABLE; // this seems to be the evil. Guess do not understand the meaning of this parameter

	dcb.fDtrControl=DTR_CONTROL_ENABLE;
	
	dcb.fDsrSensitivity=FALSE;
    dcb.fInX=FALSE;
	dcb.fOutX=FALSE;
	dcb.fNull=FALSE;
	dcb.fAbortOnError=FALSE;
	dcb.fBinary=TRUE;
	dcb.fParity=TRUE;
	dcb.fOutxCtsFlow=FALSE;
	dcb.fOutxDsrFlow=FALSE;
//	dcb.fRtsControl=FALSE;   // this seems to be the evil. Guess do not understand the meaning of this parameter
	dcb.fRtsControl=TRUE;
	dcb.fTXContinueOnXoff=TRUE;
	dcb.StopBits=2;  ///that was 2 !!!
	if (0==strncmp(baud,"115200",6))
	dcb.BaudRate = CBR_115200;
    else if (0==strncmp(baud,"57600",5))
	dcb.BaudRate = CBR_57600;
    else if (0==strncmp(baud,"38400",5))	
	dcb.BaudRate = CBR_38400;
    else if (0==strncmp(baud,"19200",5))	
	dcb.BaudRate = CBR_19200;
    else if (0==strncmp(baud,"9600",4))	
	dcb.BaudRate = CBR_9600;
    else if (0==strncmp(baud,"4800",4))	
	dcb.BaudRate = CBR_4800;
    else if (0==strncmp(baud,"2400",4))	
	dcb.BaudRate = CBR_2400;
    else if (0==strncmp(baud,"1200",4))	
	dcb.BaudRate = CBR_1200;
    else if (0==strncmp(baud,"600",3))	
	dcb.BaudRate = CBR_600;
    else if (0==strncmp(baud,"300",3))		
	dcb.BaudRate = CBR_300;
    else
	printf("illegal Baudrate: %s\n", baud);
    
	parity=tolower(parity);
    if (parity == 'e')
	dcb.Parity = 2;
	else if (parity == 'o')
	dcb.Parity = 1;
	else if (parity == 'n')
	dcb.Parity = 0;
	else
	printf("illegal parity mode:%c\n", parity);

	SetCommState(hComm,&dcb);
printf("got Comm State. %d\n ",dcb.BaudRate);
		
printf("Comm State set.\n");
//	*wfd=hComm;
	return hComm;
}
#endif

/**
    Library specific:
**/
#ifdef LINUX
#define HAVE_UNISTD
#define HAVE_SELECT
#define DECL2
#endif

#ifdef CYGWIN
#define HAVE_UNISTD
#define HAVE_SELECT
#define DECL2
#endif


#ifdef HAVE_UNISTD
#include <unistd.h>
#define daveWriteFile(a,b,c,d) d=write(a,b,c)
#else
#ifdef BCCWIN
//#define DECL2 WINAPI
#define DECL2
#endif
#define daveWriteFile(a,b,c,d) WriteFile(a,b,c, &d, NULL)
#include <winsock2.h>

void setTimeOut(daveInterface * di, int tmo) {
    COMMTIMEOUTS cto;
    GetCommTimeouts(di->fd.rfd, &cto);
    cto.ReadIntervalTimeout=0;
    cto.ReadTotalTimeoutMultiplier=0;
    cto.ReadTotalTimeoutConstant=tmo/1000;
    SetCommTimeouts(di->fd.rfd,&cto);
}    
#endif
/* 
    This will setup a new interface structure from an initialized
    serial interface's handle and a name.
*/
daveInterface * DECL2 daveNewInterface(_daveOSserialType nfd, char * nname, int localMPI, int protocol, int speed){
    daveInterface * di=(daveInterface *) calloc(1, sizeof(daveInterface));
    if (di) {
  di->name=nname;
  di->fd=nfd;
  di->localMPI=localMPI;
  di->protocol=protocol;
  di->timeout=1000000; /* 0.1 seconds */
  di->nextConnection=0x14;
  di->speed=speed;
  
  di->initAdapter=_daveReturnOkDummy; 
  di->connectPLC=_daveReturnOkDummy;  
  di->disconnectPLC=_daveReturnOkDummy; 
  di->disconnectAdapter=_daveReturnOkDummy; 
  di->listReachablePartners=_daveListReachablePartnersDummy;
  
  switch (protocol) {
      case daveProtoMPI:
    di->initAdapter=_daveInitAdapterMPI1;
    di->connectPLC=_daveConnectPLCMPI1;
    di->disconnectPLC=_daveDisconnectPLCMPI;
    di->disconnectAdapter=_daveDisconnectAdapterMPI;
    di->exchange=_daveExchangeMPI;
    di->sendMessage=_daveSendMessageMPI;
    di->getResponse=_daveGetResponseMPI;
    di->listReachablePartners=_daveListReachablePartnersMPI;
    break;  
        
      case daveProtoMPI2:
    di->initAdapter=_daveInitAdapterMPI2;
    di->connectPLC=_daveConnectPLCMPI2;
    di->disconnectPLC=_daveDisconnectPLCMPI;
    di->disconnectAdapter=_daveDisconnectAdapterMPI;
    di->exchange=_daveExchangeMPI;
    di->sendMessage=_daveSendMessageMPI;
    di->getResponse=_daveGetResponseMPI;
    di->listReachablePartners=_daveListReachablePartnersMPI;
    di->nextConnection=0x3;
    break;

      case daveProtoISOTCP:
      case daveProtoISOTCP243:
    di->getResponse=_daveGetResponseISO_TCP;
    di->connectPLC=_daveConnectPLCTCP;
    di->exchange=_daveExchangeTCP;
    break;    
    
      case daveProtoPPI:
    di->getResponse=_daveGetResponsePPI;
    di->exchange=_daveExchangePPI;
    di->connectPLC=_daveConnectPLCPPI;
    di->timeout=150000; /* 0.15 seconds */
    break;
      case daveProtoMPI_IBH:
    di->exchange=_daveExchangeIBH;
    di->connectPLC=_daveConnectPLC_IBH;
    di->disconnectPLC=_daveDisconnectPLC_IBH;
    di->sendMessage=_daveSendMessageMPI_IBH;
    di->getResponse=_daveGetResponseMPI_IBH;
    di->listReachablePartners=_daveListReachablePartnersMPI_IBH;
    break;  
  }
#ifdef BCCWIN 
  setTimeOut(di, di->timeout);
#endif
    }
    return di;  
}
/*
    debugging:
    set debug level by setting variable daveDebug. Debugging is split into
    several topics. Output goes to stderr.
    The file descriptor is written after the module name, so you may
    distinguish messages from multiple connections.
    
    naming: all stuff begins with dave... to avoid conflicts with other
    namespaces. Things beginning with _dave.. are not intended for
    public use.
*/

int daveDebug=0;

EXPORTSPEC void DECL2 setDebug(int nDebug) {
    daveDebug=nDebug;
}

/**
    PDU handling:
**/

/*
    set up the header. Needs valid header pointer
*/
void DECL2 _daveInitPDUheader(PDU * p, int type) {
    memset(p->header, 0, sizeof(PDUHeader)); 
    if (type==2 || type==3)
  p->hlen=12;
    else
  p->hlen=10;
    p->param=p->header+p->hlen;
    ((PDUHeader*)p->header)->P=0x32;
    ((PDUHeader*)p->header)->type=type;
    p->dlen=0;
    p->plen=0;
    p->udlen=0;
    p->data=NULL;
    p->udata=NULL;
}

/*
    add parameters after header, adjust pointer to data.
    needs valid header
*/
void DECL2 _daveAddParam(PDU * p,uc * param,us len) {
#ifdef ARM_FIX
    us templen;
#endif    
    p->plen=len;
    memcpy(p->param, param, len);
#ifdef ARM_FIX
    templen=bswap_16(len);
    memcpy(&(((PDUHeader*)p->header)->plen),&templen,sizeof(us));
#else
    ((PDUHeader*)p->header)->plen=bswap_16(len);
#endif    
    p->data=p->param+len;
    p->dlen=0;
}

/*
    add data after parameters, set dlen
    needs valid header,parameters
*/
void DECL2 _daveAddData(PDU * p,void * data,int len) {
#ifdef ARM_FIX
    us templen;
#endif    
    uc * dn= p->data+p->dlen;
    p->dlen+=len;
    memcpy(dn, data, len);
#ifdef ARM_FIX
    templen=bswap_16(p->dlen);
    memcpy(&(((PDUHeader*)p->header)->dlen),&templen,sizeof(us));
#else    
    ((PDUHeader*)p->header)->dlen=bswap_16(p->dlen);
#endif    
}

/*
    add values after value header in data, adjust dlen and data count.
    needs valid header,parameters,data,dlen
*/
void DECL2 _daveAddValue(PDU * p,void * data,int len) {
    us dCount;
    uc * dtype;
    dtype=p->data+p->dlen-4+1;      /* position of first byte in the 4 byte sequence */
#ifdef ARM_FIX    
    memcpy(&dCount, (p->data+p->dlen-4+2), sizeof(us));
#else
    dCount=* ((us *)(p->data+p->dlen-4+2));  /* changed for multiple write */
#endif    
    dCount=bswap_16(dCount);
    if (daveDebug & daveDebugPDU)
  LOG2("dCount: %d\n", dCount);
    if (*dtype==4) {  /* bit data, length is in bits */
  dCount+=8*len;
    } else if (*dtype==9) { /* byte data, length is in bytes */
  dCount+=len;
    } else if (* dtype==3) {  /* bit data, length is in bits */
  dCount+=len;  
    } else {
  if (daveDebug & daveDebugPDU)
      LOG2("unknown data type/length: %d\n", *dtype);
    }     
    if (p->udata==NULL) p->udata=p->data+4;
    p->udlen+=len;  
    if (daveDebug & daveDebugPDU)
  LOG2("dCount: %d\n", dCount);
    dCount=bswap_16(dCount);
#ifdef ARM_FIX    
    memcpy((p->data+p->dlen-4+2), &dCount, sizeof(us));
#else
    *((us *)(p->data+p->dlen-4+2))=dCount;
#endif    
    _daveAddData(p, data, len);
}

/*
    add data in user data. Add a user data header, if not yet present.
*/
void DECL2 _daveAddUserData(PDU * p, uc * da, int len) {
    uc udh[]={0xff,9,0,0};
    if (p->dlen==0) {
  if (daveDebug & daveDebugPDU)
      LOG1("adding user data header.\n"); 
  _daveAddData(p, udh, sizeof(udh));
    }
    _daveAddValue(p, da, len);
}

void DECL2 davePrepareReadRequest(daveConnection * dc, PDU *p) {
    uc pa[]=  {daveFuncRead,0};
    p->header=dc->msgOut+dc->PDUstartO;
    _daveInitPDUheader(p,1);
    _daveAddParam(p, pa, sizeof(pa));
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}    

void DECL2 davePrepareWriteRequest(daveConnection * dc, PDU *p) {
    uc pa[]=  {daveFuncWrite, 0};
    p->header=dc->msgOut+dc->PDUstartO;   
    _daveInitPDUheader(p,1);
    _daveAddParam(p, pa, sizeof(pa));
    p->dlen=0;
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}    

void DECL2 daveAddVarToReadRequest(PDU *p, int area, int DBnum, int start, int byteCount) {
    uc pa[]=  {
    0x12, 0x0a, 0x10,
    0x02,
    0,0,    /* length in bytes */
    0,0,    /* DB number */
    0,    /* area code */
    0,0,0   /* start address in bits */
    };
#ifdef ARM_FIX        
    us templen;
#endif    
//    pa[3]=4;
    if ((area==daveAnaIn) || (area==daveAnaOut)) {
  pa[3]=4;
  start*=8;     /* bits */
    } else if ((area==daveTimer) || (area==daveCounter)||(area==daveTimer200) || (area==daveCounter200)) {
  pa[3]=area;
    } else {
  start*=8;     /* bits */
    }
    pa[4]=byteCount / 256;    
    pa[5]=byteCount & 0xff;   
    pa[6]=DBnum / 256;    
    pa[7]=DBnum & 0xff;   
    pa[8]=area;   
    pa[11]=start & 0xff;
    pa[10]=(start / 0x100) & 0xff;
    pa[9]=start / 0x10000; 
    
    p->param[1]++;
    memcpy(p->param+p->plen, pa, sizeof(pa));
    p->plen+=sizeof(pa);
    
#ifdef ARM_FIX    
    templen=bswap_16(p->plen);
    memcpy(&(((PDUHeader*)p->header)->plen), &templen, sizeof(us));
#else
    ((PDUHeader*)p->header)->plen=bswap_16(p->plen);
#endif    
    p->data=p->param+p->plen;
    p->dlen=0;
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}    

void DECL2 daveAddBitVarToReadRequest(PDU *p, int area, int DBnum, int start, int byteCount) {
    uc pa[]=  {
    0x12, 0x0a, 0x10,
    0x01,   /* single bit */
    0,0,    /* length in bytes */
    0,0,    /* DB number */
    0,    /* area code */
    0,0,0   /* start address in bits */
    };
#ifdef ARM_FIX        
    us templen;
#endif    
    pa[4]=byteCount / 256;    
    pa[5]=byteCount & 0xff;   
    pa[6]=DBnum / 256;    
    pa[7]=DBnum & 0xff;   
    pa[8]=area;   
    pa[11]=start & 0xff;
    pa[10]=(start / 0x100) & 0xff;
    pa[9]=start / 0x10000; 
    
    p->param[1]++;
    memcpy(p->param+p->plen, pa, sizeof(pa));
    p->plen+=sizeof(pa);
#ifdef ARM_FIX    
    templen=bswap_16(p->plen);
    memcpy(&(((PDUHeader*)p->header)->plen), &templen, sizeof(us));
#else
    ((PDUHeader*)p->header)->plen=bswap_16(p->plen);
#endif    
    p->data=p->param+p->plen;
    p->dlen=0;
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    }
}    

void DECL2 daveAddVarToWriteRequest(PDU *p, int area, int DBnum, int start, int byteCount, void * buffer) {
    uc saveData[1024];
    uc da[]=  {0,4,0,0,};
    uc pa[]=  {
    0x12, 0x0a, 0x10,
    0x02,   /* unit (for count?, for consistency?) byte */
    0,0,    /* length in bytes */
    0,0,    /* DB number */
    0,    /* area code */
    0,0,0   /* start address in bits */
    };
#ifdef ARM_FIX        
    us templen;
#endif    
    if ((area==daveTimer) || (area==daveCounter)||(area==daveTimer200) || (area==daveCounter200)) {    
  pa[3]=area;
  pa[4]=((byteCount+1)/2) / 0x100;
  pa[5]=((byteCount+1)/2) & 0xff;
    } else if ((area==daveAnaIn) || (area==daveAnaOut)) {
  pa[3]=4;
  pa[4]=((byteCount+1)/2) / 0x100;
  pa[5]=((byteCount+1)/2) & 0xff;
    } else {  
  pa[4]=byteCount / 0x100;    
  pa[5]=byteCount & 0xff; 
    }
    pa[6]=DBnum / 256;    
    pa[7]=DBnum & 0xff;   
    pa[8]=area;   
    start*=8;     /* number of bits */
    pa[11]=start & 0xff;
    pa[10]=(start / 0x100) & 0xff;
    pa[9]=start / 0x10000; 
    p->param[1]++;
/*    
    if(p->dlen%2) {
  _daveAddData(p, da, 1); / * add a null byte to fill up last data to word boundary * /
  LOG1("****************adding a fill byte\n");
    }
*/    
    if(p->dlen){
   memcpy(saveData, p->data, p->dlen);
//   memcpy(p->data+sizeof(pa), p->data, p->dlen);
   memcpy(p->data+sizeof(pa), saveData, p->dlen);
    }  
    memcpy(p->param+p->plen, pa, sizeof(pa));
    p->plen+=sizeof(pa);
#ifdef ARM_FIX    
    templen=bswap_16(p->plen);
    memcpy(&(((PDUHeader*)p->header)->plen), &templen, sizeof(us));
#else
    ((PDUHeader*)p->header)->plen=bswap_16(p->plen);
#endif    
    p->data=p->param+p->plen;
    _daveAddData(p, da, sizeof(da));
    _daveAddValue(p, buffer, byteCount);
//    if(p->dlen%2) {
    if(byteCount%2) {
  _daveAddData(p, da, 1); /* add a null byte to fill up last data to word boundary */
//  LOG1("*************************adding a fill byte\n");
    }    
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}    

void DECL2 daveAddBitVarToWriteRequest(PDU *p, int area, int DBnum, int start, int byteCount, void * buffer) {
    uc saveData[1024];
    uc da[]=  {0,3,0,0,};
    uc pa[]=  {
    0x12, 0x0a, 0x10,
    0x01,   /* single bit */
    0,0,    /* insert length in bytes here */
    0,0,    /* insert DB number here */
    0,    /* change this to real area code */
    0,0,0   /* insert start address in bits */
    };
#ifdef ARM_FIX        
    us templen;
#endif    
    if ((area==daveTimer) || (area==daveCounter)||(area==daveTimer200) || (area==daveCounter200)) {    
  pa[3]=area;
  pa[4]=((byteCount+1)/2) / 0x100;
  pa[5]=((byteCount+1)/2) & 0xff;
    } else if ((area==daveAnaIn) || (area==daveAnaOut)) {
  pa[3]=4;
  pa[4]=((byteCount+1)/2) / 0x100;
  pa[5]=((byteCount+1)/2) & 0xff;
    } else {  
  pa[4]=byteCount / 0x100;    
  pa[5]=byteCount & 0xff;     
    }
    pa[6]=DBnum / 256;    
    pa[7]=DBnum & 0xff;   
    pa[8]=area;   
    pa[11]=start & 0xff;
    pa[10]=(start / 0x100) & 0xff;
    pa[9]=start / 0x10000; 
    p->param[1]++;
    if(p->dlen){
   memcpy(saveData, p->data, p->dlen);
//   memcpy(p->data+sizeof(pa), p->data, p->dlen); // cannot do this, memory may overlap
   memcpy(p->data+sizeof(pa), saveData, p->dlen);
    }  
    memcpy(p->param+p->plen, pa, sizeof(pa));
    p->plen+=sizeof(pa);
#ifdef ARM_FIX    
    templen=bswap_16(p->plen);
    memcpy(&(((PDUHeader*)p->header)->plen), &templen, sizeof(us));
#else
    ((PDUHeader*)p->header)->plen=bswap_16(p->plen);
#endif    
    p->data=p->param+p->plen;
    _daveAddData(p, da, sizeof(da));
    _daveAddValue(p, buffer, byteCount);
//    if(p->dlen%2) {
    if(byteCount%2) {
  _daveAddData(p, da, 1); /* add a null byte to fill up last data to word boundary */
  LOG1("*************************adding a fill byte\n");
    }    
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}    

/*
    Get the eror code from a PDU, if one.
*/
int DECL2 daveGetPDUerror(PDU * p) {
    if (p->header[1]==2 || p->header[1]==3) {
  return daveGetU16from(p->header+10);
    } else
  return 0;
}

/*
    Sets up pointers to the fields of a received message. 
*/
int DECL2 _daveSetupReceivedPDU(daveConnection * dc, PDU * p) {
  int res = daveResCannotEvaluatePDU;    
  p->header=dc->msgIn+dc->PDUstartI;
  
        if (p->header[1]==2 || p->header[1]==3) {
      p->hlen=12;
  } else
      p->hlen=10;
  p->param=p->header+p->hlen;    
  if (daveDebug & daveDebugPDU)
      _daveDump("PDU header", p->header, p->hlen);
  p->plen=256*p->header[6] + p->header[7];
  p->data=p->param+p->plen;
        p->dlen=256*p->header[8] + p->header[9];
  p->udlen=0; 
  p->udata=NULL;
  if ((p->param[0]==daveFuncRead)|| (p->param[0]==0)) {
      if ((p->data[0]==255)&&(p->dlen>4)) {
    res=daveResOK;
    p->udata=p->data+4;
    p->udlen=p->data[2]*0x100+p->data[3];
    if (p->data[1]==4) {
            p->udlen>>=3; /* len is in bits, adjust */
    } else if (p->data[1]==9) {
        /* len is already in bytes, ok */
    } else if (p->data[1]==3) {
        /* len is in bits, but there is a byte per result bit, ok */
    } else {
        if (daveDebug & daveDebugPDU)
      LOG2("fixme: what to do with data type %d?\n",p->data[1]);
    }     
      } else {
    if ((p->data[0]==0x0A)) {
        res=daveResItemNotAvailable;    
    }
    if ((p->data[0]==0x05)) {
        res=daveAddressOutOfRange;
    }
    if ((p->data[0]==0x06)) {
        res=daveResMultipleBitsNotSupported;
    }
    if ((p->data[0]==0x03)) {
        res=daveResItemNotAvailable200;
    }
      }
  } else if (p->param[0]==daveFuncWrite) {
      if ((p->data[0]==255)) {
    res=daveResOK;
      } else if ((p->data[0]==0x0A)) {
    res=daveResItemNotAvailable;    
      } else if ((p->data[0]==0x06)) {
          res=daveResMultipleBitsNotSupported;
      } else if ((p->data[0]==0x03)) {
          res=daveResItemNotAvailable200;
      } else if ((p->data[0]==0x07)) {
          res=daveWriteDataSizeMismatch;
      }
  }
  if (daveDebug & daveDebugPDU) {
      _daveDumpPDU(p);
  }
  return res;
}

/*****
    Utilities:
****/
/*
    This is an extended memory compare routine. It can handle don't care and stop flags 
    in the sample data. A stop flag lets it return success.
*/
int DECL2 _daveMemcmp(us * a, uc *b, size_t len) {
    unsigned int i;
    us * a1=(us*)a;
    uc * b1=(uc*)b;
    for (i=0;i<len;i++){
  if (((*a1)&0xff)!=*b1) {
      if (((*a1)&0x100)!=0x100) {
//    LOG3("want:%02X got:%02X\n",*a1,*b1);
     return i+1;
      }  
      if (((*a1)&0x200)==0x200) return 0;
  }
  a1++;
  b1++;
    }
    return 0;
}

/*
    Hex dump:
*/
void DECL2 _daveDump(char * name,uc*b,int len) {
    int j;
    LOG2("%s: ",name);
    if (len>daveMaxRawLen) len=daveMaxRawLen;   /* this will avoid to dump zillions of chars */
    for (j=0; j<len; j++){
  LOG2("%02X,",b[j]);
    }
    LOG1("\n");
}

int DECL2 daveGetPDUError(PDU * p) {
    return 0x100*p->header[10]+p->header[11];
}

char * DECL2 daveStrPDUError(int e) {
    switch(e) {
  case 0: return "ok"; 
  case 0x8000: return "function already occupied.";
  case 0x8001: return "not allowed in current operating status.";
  case 0x8101: return "hardware fault.";
  case 0x8103: return "object access not allowed.";
  case 0x8104: return "context is not supported.";
  case 0x8105: return "invalid address.";
  case 0x8106: return "data type not supported.";
  case 0x8107: return "data type not consistent.";
  case 0x810A: return "object does not exist.";
  case 0x8500: return "incorrect PDU size.";
  case 0x8702: return "address invalid."; ;
  case 0xd201: return "block name syntax error.";
  case 0xd202: return "syntax error function parameter.";
  case 0xd203: return "syntax error block type.";
  case 0xd204: return "no linked block in storage medium.";
  case 0xd205: return "object already exists.";
  case 0xd206: return "object already exists.";
  case 0xd207: return "block exists in EPROM.";
  case 0xd209: return "block does not exist.";
  case 0xd20e: return "no block does not exist.";
  case 0xd210: return "block number too big.";
  default: return "unknown error!!";
    } 
}

/*
    Hex dump PDU:
*/
void DECL2 _daveDumpPDU(PDU * p) {
    int i,dl;
    uc * pd;
    _daveDump("PDU header", p->header, p->hlen);
    LOG3("plen: %d dlen: %d\n",p->plen, p->dlen);
    if(p->plen>0) _daveDump("Parameter",p->param,p->plen);
    if ((p->plen==2)&&(p->param[0]==daveFuncRead)) {
  pd=p->data;
  for (i=0;i<p->param[1];i++) {
      _daveDump("Data hdr ",pd,4);
      
      dl=0x100*pd[2]+pd[3];
      if (pd[1]==4) dl/=8;
      pd+=4;        
      _daveDump("Data     ",pd,dl);
      if(i<p->param[1]-1) dl=dl+(dl%2);   // the PLC places extra bytes at the end of all 
            // but last result, if length is not a multiple 
            // of 2
      pd+=dl;
  }
    } else if ((p->header[1]==1)&&/*(p->plen==2)&&*/(p->param[0]==daveFuncWrite)) {
  pd=p->data;
  for (i=0;i<p->param[1];i++) {
      _daveDump("Write Data hdr ",pd,4);
      
      dl=0x100*pd[2]+pd[3];
      if (pd[1]==4) dl/=8;
      pd+=4;        
      _daveDump("Data     ",pd,dl);
      if(i<p->param[1]-1) dl=dl+(dl%2);   // the PLC places extra bytes at the end of all 
            // but last result, if length is not a multiple 
            // of 2
      pd+=dl;
  }
    } else {    
  if(p->dlen>0) {
      if(p->udlen==0)
    _daveDump("Data     ",p->data,p->dlen);
      else
    _daveDump("Data hdr ",p->data,4);
  } 
  if(p->udlen>0) _daveDump("result Data ",p->udata,p->udlen);
    }
    if ((p->header[1]==2)||(p->header[1]==3)) {
  LOG2("error: %s\n",daveStrPDUError(daveGetPDUerror(p)));
    } 
}

/*
    name Objects:
*/
char * DECL2 daveBlockName(uc bn) {
    switch(bn) {
  case daveBlockType_OB: return "OB";
  case daveBlockType_DB: return "DB";
  case daveBlockType_SDB: return "SDB";
  case daveBlockType_FC: return "FC";
  case daveBlockType_SFC: return "SFC";
  case daveBlockType_FB: return "FB";
  case daveBlockType_SFB: return "SFB";
  default:return "unknown block type!";
    }    
}

char * DECL2 daveAreaName(uc n) {
    switch (n) {
  case daveDB:    return "DB";
  case daveInputs:  return "Inputs";
  case daveOutputs: return "Outputs";
  case daveFlags:   return "Flags";
  default:return "unknown area!";
    } 
}

/*
    Functions to load blocks from PLC:
*/
void DECL2 _daveConstructUpload(PDU *p,char blockType, int blockNr) {
    uc pa[]=  {0x1d,
    0,0,0,0,0,0,0,9,0x5f,0x30,0x41,48,48,48,48,49,65};
    pa[11]=blockType;
    sprintf((char*)(pa+12),"%05d",blockNr);
    pa[17]='A';
    _daveInitPDUheader(p,1);
    _daveAddParam(p, pa, sizeof(pa));
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}

void DECL2 _daveConstructDoUpload(PDU * p, int uploadID) {
    uc pa[]=  {0x1e,0,0,0,0,0,0,1};
    pa[7]=uploadID;
    _daveInitPDUheader(p,1);
    _daveAddParam(p, pa, sizeof(pa));
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}    

void DECL2 _daveConstructEndUpload(PDU * p, int uploadID) {
    uc pa[]=  {0x1f,0,0,0,0,0,0,1};
    pa[7]=uploadID;
    _daveInitPDUheader(p,1);
    _daveAddParam(p, pa, sizeof(pa));
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
}    

uc paMakeRun[]= {
    0x28,0,0,0,0,0,0,0xFD,0,0,9,'P','_','P','R','O','G','R','A','M'
};
    
uc paMakeStop[]= {
    0x29,0,0,0,0,0,9,'P','_','P','R','O','G','R','A','M'
};


EXPORTSPEC int DECL2 daveStop(daveConnection*dc) {
    int res;
    PDU p,p2;
    p.header=dc->msgOut+dc->PDUstartO;
    _daveInitPDUheader(&p, 1);
    _daveAddParam(&p, paMakeStop, sizeof(paMakeStop));
    res=_daveExchange(dc, &p);
    if (res==daveResOK) {
  res=_daveSetupReceivedPDU(dc,&p2);
  if (daveDebug & daveDebugPDU) {
      _daveDumpPDU(&p2);
  }
    }
    return res;
}
  
EXPORTSPEC int DECL2 daveStart(daveConnection*dc) {
    int res;
    PDU p,p2;
    p.header=dc->msgOut+dc->PDUstartO;
    _daveInitPDUheader(&p, 1);
    _daveAddParam(&p, paMakeRun, sizeof(paMakeRun));
    res=_daveExchange(dc, &p);
    if (res==daveResOK) {
  res=_daveSetupReceivedPDU(dc, &p2);
  if (daveDebug & daveDebugPDU) {
      _daveDumpPDU(&p2);
  }
    }
    return res;
}
  


/*
    Build a PDU with user data ud, send it and prepare received PDU.
*/
int DECL2 daveBuildPDU(daveConnection * dc, PDU*p2,uc *pa,int psize, uc *ud,int usize) {
    int res;
    PDU p;
    p.header=dc->msgOut+dc->PDUstartO;
    _daveInitPDUheader(&p, 7);
    _daveAddParam(&p, pa, psize);
    _daveAddUserData(&p, ud, usize);
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(&p);
    } 
    res=_daveExchange(dc, &p);
    if (res==daveResOK) {
  res=_daveSetupReceivedPDU(dc,p2);
  if (daveDebug & daveDebugPDU) {
      _daveDumpPDU(p2);
  }
    } 
    return res;
}     

int DECL2 daveListBlocksOfType(daveConnection * dc,uc type,daveBlockEntry * buf) {
    int res,i, len;
    PDU p2;
    uc * buffer=(uc*)buf;
    uc pa[]={0,1,18,4,17,67,2,0};
    uc da[]={'0','0'};
    uc pam[]={0,1,18,8,0x12,0x43,2,1,0,0,0,0};
    uc dam[]={0,0x0a,0,0};
    da[1]=type;
    res=daveBuildPDU(dc, &p2,pa, sizeof(pa), da, sizeof(da));
//    printf("here we are\n");
    len=0;
    while (p2.param[9]!=0) {
  if (buffer!=NULL) memcpy(buffer+len,p2.udata,p2.udlen);
        dc->resultPointer=p2.udata;
        dc->_resultPointer=p2.udata;
        len+=p2.udlen;
//  printf("more data\n");
  res=daveBuildPDU(dc, &p2,pam, sizeof(pam), dam, sizeof(dam));
    }
    
    
    if (res==daveResOK) {
  if (buffer!=NULL) memcpy(buffer+len,p2.udata,p2.udlen);
//        if (buffer!=NULL) memcpy(buffer,p2.udata,p2.udlen);
        dc->resultPointer=p2.udata;
        dc->_resultPointer=p2.udata;
        len+=p2.udlen;
    } else {
  LOG2("daveListBlocksOfType: %d\n",res);
    }
    dc->AnswLen=len;
    res=len/sizeof(daveBlockEntry);
    for (i=0; i<res; i++) {
        buf[i].number=bswap_16(buf[i].number);
    }
    return res;
}    

/*
    doesn't work on S7-200
*/
int DECL2 daveGetOrderCode(daveConnection * dc,char * buf) {
    int res=0;
    PDU p2;
    uc pa[]={0,1,18,4,17,68,1,0};
    uc da[]={1,17,0,1};  /* SZL-ID 0x111 index 1 */
    daveBuildPDU(dc, &p2,pa, sizeof(pa), da, sizeof(da));
    if (buf) {
  memcpy(buf, p2.udata+10, daveOrderCodeSize);
  buf[daveOrderCodeSize]=0;
    } 
    return res;
}

int DECL2 daveReadSZL(daveConnection * dc, int ID, int index, void * buffer) {
    int res,len;
    PDU p2;
    uc pa[]={0,1,18,4,0x11,0x44,1,0};
    uc da[]={1,17,0,1};  /* SZL-ID 0x111 index 1 */
    
    uc pam[]={0,1,18,8,0x12,0x44,1,1,0,0,0,0};
    uc dam[]={0,0x0a,0,0};
    da[0]=ID / 0x100;
    da[1]=ID % 0x100;
    da[2]=index / 0x100;
    da[3]=index % 0x100;
    res=daveBuildPDU(dc, &p2,pa, sizeof(pa), da, sizeof(da));
    
//    printf("here we are\n");
    len=0;
    while (p2.param[9]!=0) {
  if (buffer!=NULL) memcpy((uc *)buffer+len,p2.udata,p2.udlen);
        dc->resultPointer=p2.udata;
        dc->_resultPointer=p2.udata;
        len+=p2.udlen;
//  printf("more data\n");
  res=daveBuildPDU(dc, &p2,pam, sizeof(pam), dam, sizeof(dam));
    }
    
    
    if (res==daveResOK) {
  if (buffer!=NULL) memcpy((uc *)buffer+len,p2.udata,p2.udlen);
//        if (buffer!=NULL) memcpy(buffer,p2.udata,p2.udlen);
        dc->resultPointer=p2.udata;
        dc->_resultPointer=p2.udata;
        len+=p2.udlen;
    } 
    dc->AnswLen=len;
    return res;
}

int DECL2 daveGetBlockInfo(daveConnection * dc, daveBlockInfo *dbi, uc type, int number)
{
    int res;
    uc pa[]={0,1,18,4,17,67,3,0};    /* param */
    uc da[]={'0',0,'0','0','0','1','0','A'};
    PDU p2;
    sprintf((char*)(da+2),"%05d",number);
    da[1]=type;
    da[7]='A';
    daveBuildPDU(dc, &p2,pa, sizeof(pa), da, sizeof(da));    
    if (dbi && p2.udlen==sizeof(daveBlockInfo)) {
  memcpy(dbi, p2.udata, p2.udlen);
  dbi->number=bswap_16(dbi->number);
        dbi->length=bswap_16(dbi->length);
    }
    return res; 
}

int DECL2 daveListBlocks(daveConnection * dc,daveBlockTypeEntry * buf) {
    int res,i;
    PDU p2;
    uc pa[]={0,1,18,4,17,67,1,0};
    uc da[]={10,0,0,0}; 
    daveBuildPDU(dc, &p2,pa, sizeof(pa), da, sizeof(da));    
    res=p2.udlen/sizeof(daveBlockTypeEntry);
    if (buf) {
  memcpy(buf, p2.udata, p2.udlen);
  for (i=0; i<res; i++) {
      buf[i].count=bswap_16(buf[i].count);
  } 
    } 
    return res;
}

/*
    Read len bytes from PLC memory area "area", data block DBnum. 
    Return the Number of bytes read.
    If a buffer pointer is provided, data will be copied into this buffer.
    If it's NULL you can get your data from the resultPointer in daveConnection long
    as you do not send further requests.
*/
int DECL2 daveReadBytes(daveConnection * dc,int area, int DBnum, int start,int len, void * buffer){
    PDU p1,p2;
    int errorState,res;
    
    p1.header=dc->msgOut+dc->PDUstartO;
    davePrepareReadRequest(dc, &p1);
    daveAddVarToReadRequest(&p1, area, DBnum, start, len);
    errorState=_daveExchange(dc, &p1);
    if (errorState) return errorState;
    res=_daveSetupReceivedPDU(dc, &p2);
    if (daveDebug & daveDebugPDU)
  LOG3("_daveSetupReceivedPDU() returned: %d=%s\n", res,daveStrerror(res));
    if (res!=0) {
  dc->resultPointer=NULL;
  dc->_resultPointer=NULL;
  dc->AnswLen=0;
  return res;
    }
    if (p2.udlen==0) {
  dc->resultPointer=NULL;
  dc->_resultPointer=NULL;
  dc->AnswLen=0;
  return daveResCPUNoData; 
    } 
    if (p2.param[0]==daveFuncRead) {
/*  if (p2.udata==NULL) 
//      errorState+=p2.data[0]; 
//  else {
*/
      if (buffer!=NULL) memcpy(buffer,p2.udata,p2.udlen);
      dc->resultPointer=p2.udata;
      dc->_resultPointer=p2.udata;
      dc->AnswLen=p2.udlen;
//      LOG2("Value: %d\n",p2.udata[0]);  //verify we get real results in benchmarks 
//  (not just the same). PLC has a program that does nothing (for speed) but increment MB0
//  (to see something change).
/*  }    */
    } else {
  errorState|=2048;
    }
    return errorState;
}

/*
    Read len BITS from PLC memory area "area", data block DBnum. 
    Return the Number of bytes read.
    If a buffer pointer is provided, data will be copied into this buffer.
    If it's NULL you can get your data from the resultPointer in daveConnection long
    as you do not send further requests.
*/
int DECL2 daveReadBits(daveConnection * dc,int area, int DBnum, int start,int len, void * buffer){
    PDU p1,p2;
    int errorState,res;
    
    p1.header=dc->msgOut+dc->PDUstartO;
    davePrepareReadRequest(dc, &p1);
    daveAddBitVarToReadRequest(&p1, area, DBnum, start, len);
    
    errorState=_daveExchange(dc, &p1);
    res=_daveSetupReceivedPDU(dc, &p2);
    if (daveDebug & daveDebugPDU)
  LOG3("_daveSetupReceivedPDU() returned: %d=%s\n", res,daveStrerror(res));
    if (res!=0) {
  dc->resultPointer=NULL;
  dc->_resultPointer=NULL;
  dc->AnswLen=0;
  return res;
    }
    if (p2.udlen==0) {
  dc->resultPointer=NULL;
  dc->_resultPointer=NULL;
  dc->AnswLen=0;
  return daveResCPUNoData; 
    } 
    if (p2.param[0]==daveFuncRead) {
        if (buffer!=NULL) memcpy(buffer,p2.udata,p2.udlen);
        dc->resultPointer=p2.udata;
        dc->_resultPointer=p2.udata;
        dc->AnswLen=p2.udlen;
    } else {
  errorState|=2048;
    }
    return errorState;
}


/*
    Execute a predefined read request. Store results into the resultSet structure.
*/
int DECL2 daveExecReadRequest(daveConnection * dc, PDU *p, daveResultSet* rl){
    PDU p2;
    uc * q;
    daveResult * cr, *c2;
    int errorState, i, len, rlen;
    errorState=_daveExchange(dc, p);
    _daveSetupReceivedPDU(dc, &p2);
    if (p2.udlen==0) {
  dc->resultPointer=NULL;
  dc->_resultPointer=NULL;
  dc->AnswLen=0;
  return daveResCPUNoData; 
    } 
    if (p2.param[0]==daveFuncRead) {
      i=0;
      if (rl!=NULL) {
    cr=calloc(p2.param[1], sizeof(daveResult));
    rl->numResults=p2.param[1];
    rl->results=cr;
    c2=cr;
    q=p2.data;
    rlen=p2.dlen;
    while (i<p2.param[1]) {
/*        printf("result %d: %d  %d %d %d\n",i, *q,q[1],q[2],q[3]); */
        if ((*q==255)&&(rlen>4)) {
      len=q[2]*0x100+q[3];
      if (q[1]==4) {
              len>>=3;  /* len is in bits, adjust */
      } else if (q[1]==9) {
          /* len is already in bytes, ok */
      } else if (q[1]==3) {
          /* len is in bits, but there is a byte per result bit, ok */
      } else {
          if (daveDebug & daveDebugPDU)
        LOG2("fixme: what to do with data type %d?\n",q[1]);
      }
        } else {
      len=0;
        } 
/*        printf("Store result %d length:%d\n", i, len); */
        c2->length=len;
        if(len>0){
       c2->bytes=malloc(len);
       memcpy(c2->bytes, q+4, len);
        }  
        c2->error=daveUnknownError;
        if (q[0]==0x0A) { /* 300 and 400 families */
      c2->error=daveResItemNotAvailable;    
        } else
        if (q[0]==0x03) { /* 200 family */
      c2->error=daveResItemNotAvailable;    
        } else
        if (q[0]==0x05) {
      c2->error=daveAddressOutOfRange;    
        } else
        if (q[0]==0xFF) {
      c2->error=daveResOK;    
        }
/*        printf("Error %d\n", c2->error); */
        q+=len+4;
        rlen-=len;
        if ((len % 2)==1) {
            q++;
            rlen--;
        } 
        c2++;
            i++;
    }
      } 
      
/*  }    */
    } else {
  errorState|=2048;
    }
    return errorState;
}

/*
    Execute a predefined write request.
*/
int DECL2 daveExecWriteRequest(daveConnection * dc, PDU *p, daveResultSet* rl){
    PDU p2;
    uc * q;
    daveResult * cr, *c2;
    int errorState, i, rlen;
    errorState=_daveExchange(dc, p);
    _daveSetupReceivedPDU(dc, &p2);
    if (p2.param[0]==daveFuncWrite) {
        if (rl!=NULL) {
      cr=calloc(p2.param[1], sizeof(daveResult));
      rl->numResults=p2.param[1];
      rl->results=cr;
      c2=cr;
      q=p2.data;
      rlen=p2.dlen;
      i=0;
      while (i<p2.param[1]) {
/*    printf("result %d: %d  %d %d %d\n",i, *q,q[1],q[2],q[3]); */
    c2->error=daveUnknownError;
    if (q[0]==0x0A) { /* 300 and 400 families */
        c2->error=daveResItemNotAvailable;    
    } else if (q[0]==0x03) {  /* 200 family */
        c2->error=daveResItemNotAvailable;    
    } else if (q[0]==0x05) {
        c2->error=daveAddressOutOfRange;    
    } else if (q[0]==0xFF) {
        c2->error=daveResOK;   
    } else if (q[0]==0x07) {
            c2->error=daveWriteDataSizeMismatch;
    } 
/*        printf("Error %d\n", c2->error); */
    q++;
    c2++;
    i++;
      } 
  } else {
      errorState|=2048;
  }    
    }
    return errorState;
}


int DECL2 daveUseResult(daveConnection * dc, daveResultSet rl, int n){
    daveResult * dr;
    if (rl.numResults==0) return daveEmptyResultSetError;
    if (n>=rl.numResults) return daveEmptyResultSetError;
    dr = &(rl.results[n]);
    if (dr->error!=0) return dr->error;
    if (dr->length<=0) return daveEmptyResultError;
    
    dc->resultPointer=dr->bytes;
    dc->_resultPointer=dr->bytes;
    return 0;
}

void DECL2 daveFreeResults(daveResultSet * rl){
    daveResult * r;
    int i;
/*    printf("result set: %p\n",rl); */
    for (i=0; i<rl->numResults; i++) {
        r=&(rl->results[i]);
/*  printf("result: %p bytes at:%p\n",r,r->bytes); */
  if (r->bytes!=NULL) free(r->bytes);
    }
/*    free(rl); */ /* This is NOT malloc'd by library but in the application's memory space! */
}


/* 
    This will setup a new connection structure using an initialized
    daveInterface and PLC's MPI address.
*/
daveConnection * DECL2 daveNewConnection(daveInterface * di, int MPI, int rack, int slot) {
    daveConnection * dc=(daveConnection *) calloc(1,sizeof(daveConnection));
    if (dc) {
  dc->iface=di;
  dc->MPIAdr=MPI;

  dc->templ.src_conn=20;  /* for MPI over IBHLink */
  dc->templ.MPI=MPI;
  dc->templ.localMPI=di->localMPI; //????
  dc->rack=rack;
  dc->slot=slot;
  
  dc->maxPDUlength=240;
  dc->connectionNumber=di->nextConnection;  // 1/10/05 trying Andrew's patch
//  di->nextConnection++;       // 1/10/05 trying Andrew's patch
  
  switch (di->protocol) {
      case daveProtoMPI:    /* my first Version of MPI */
    dc->PDUstartO=8;  /* position of PDU in outgoing messages */
    dc->PDUstartI=8;  /* position of PDU in incoming messages */
    di->ackPos=6;   /* position of 0xB0P in ack packet */
    break;
      case daveProtoMPI2:   /* Andrew's Version of MPI */
    dc->PDUstartO=6;  /* position of PDU in outgoing messages */
    dc->PDUstartI=6;  /* position of PDU in incoming messages */
    di->ackPos=4;   /* position of 0xB0P in ack packet */
    break;  

      case daveProtoPPI:
    dc->PDUstartO=3;  /* position of PDU in outgoing messages */
    dc->PDUstartI=7;  /* position of PDU in incoming messages */
    break;  
    
      case daveProtoISOTCP:
      case daveProtoISOTCP243:
    dc->PDUstartO=7;  /* position of PDU in outgoing messages */
    dc->PDUstartI=7;  /* position of PDU in incoming messages */
    di->timeout=1500000;
    break;  
      case daveProtoMPI_IBH:  
    dc->PDUstartI= sizeof(IBHpacket)+sizeof(MPIheader); 
    dc->PDUstartO= sizeof(IBHpacket)+sizeof(MPIheader); // 02/01/2005 
    break;
      case daveProtoPPI_IBH:  
    dc->PDUstartI=14; // sizeof(IBHpacket)+7; 
    dc->PDUstartO=14;// sizeof(IBHpacket)+7; // 02/01/2005  
    break;  
      default:
    dc->PDUstartO=8;  /* position of PDU in outgoing messages */
    dc->PDUstartI=8;  /* position of PDU in incoming messages */
    fprintf(stderr,"Unknown protocol on interface %s\n",di->name);  
  }    
#ifdef BCCWIN 
  setTimeOut(di, di->timeout);
#endif
    }
    return dc;  
}

int DECL2 daveWriteBytes(daveConnection * dc,int area, int DB, int start, int len, void * buffer) {
    PDU p1,p2;
    int errorState, res;
    p1.header=dc->msgOut+dc->PDUstartO;
    davePrepareWriteRequest(dc, &p1);
    daveAddVarToWriteRequest(&p1, area, DB, start, len, buffer);
    errorState=_daveExchange(dc, &p1);
    res=_daveSetupReceivedPDU(dc, &p2);
    if (res!=0) return res;
    if (p2.param[0]==daveFuncWrite) {
  return errorState;    
    } else {
  return errorState|2048;
    }
}

int DECL2 daveWriteBits(daveConnection * dc,int area, int DB, int start, int len, void * buffer) {
    PDU p1,p2;
    int errorState,res;
    p1.header=dc->msgOut+dc->PDUstartO;
    davePrepareWriteRequest(dc,&p1);
    daveAddBitVarToWriteRequest(&p1, area, DB, start, len, buffer);
    errorState=_daveExchange(dc, &p1);
    res=_daveSetupReceivedPDU(dc, &p2);
    if (res!=0) return res;
    if (p2.param[0]==daveFuncWrite) {
  return errorState;    
    } else {
  return errorState|2048;
    }
}


int DECL2 initUpload(daveConnection * dc,char blockType, int blockNr, int * uploadID){
    PDU p1,p2;
    int errorState;
    p1.header=dc->msgOut+dc->PDUstartO;
    _daveConstructUpload(&p1, blockType, blockNr);
    errorState=_daveExchange(dc, &p1);
    if (daveDebug & daveDebugUpload) {
        LOG2("error:%d\n",errorState);
        FLUSH;
    } 
    if (errorState) goto error;
    _daveSetupReceivedPDU(dc, &p2);
    * uploadID=p2.param[7];
    errorState=0x100*p2.header[10]+p2.header[11];
error:    
    return errorState;
}


int DECL2 doUpload(daveConnection*dc, int * more, uc**buffer, int*len, int uploadID){
    PDU p1,p2;
    int errorState,netLen;
    p1.header=dc->msgOut+dc->PDUstartO;
    _daveConstructDoUpload(&p1, uploadID);
    errorState=_daveExchange(dc, &p1);
    if (daveDebug & daveDebugUpload) {
  LOG2("error:%d\n",errorState);
  FLUSH;
    } 
    *more=0;
    if (errorState) goto error;
    _daveSetupReceivedPDU(dc, &p2);
    *more=p2.param[1];
    netLen=p2.data[1] /* +256*p2.data[0]; */ /* for long PDUs, I guess it is so */;
    if (*buffer) {
  memcpy(*buffer,p2.data+4,netLen);
  *buffer+=netLen;
  if (daveDebug & daveDebugUpload) {
      LOG2("buffer:%p\n",*buffer);
      FLUSH;
  }    
    }
    *len+=netLen;
error:
    return errorState;
}

int DECL2 endUpload(daveConnection*dc, int uploadID){
    PDU p1,p2;
    int errorState;
    
    p1.header=dc->msgOut+dc->PDUstartO;
    _daveConstructEndUpload(&p1,uploadID);
    
    errorState=_daveExchange(dc, &p1);
    if (daveDebug & daveDebugUpload) {
        LOG2("error:%d\n",errorState);
        FLUSH;
    } 
    if (errorState) goto error;
    _daveSetupReceivedPDU(dc, &p2);
error:
    return errorState;
}

/*
    error code to message string conversion:
*/
char * DECL2 daveStrerror(int code) {
    switch (code) {
  case daveResOK: return "ok";
  case daveResMultipleBitsNotSupported:return "the CPU does not support reading a bit block of length<>1";
  case daveResItemNotAvailable: return "the desired item is not available in the PLC";
  case daveResItemNotAvailable200: return "the desired item is not available in the PLC (200 family)";
  case daveAddressOutOfRange: return "the desired address is beyond limit for this PLC";
  case daveResCPUNoData : return "the PLC returned a packet with no result data";
  case daveUnknownError : return "the PLC returned an error code not understood by this library";
  case daveEmptyResultError : return "this result contains no data";
  case daveEmptyResultSetError: return "cannot work with an undefined result set";
  case daveResCannotEvaluatePDU: return "cannot evaluate the received PDU";
  case daveWriteDataSizeMismatch: return "Write data size error";
/*
    error code to message string conversion:
*/
  default: return "no message defined!";
    }
}

float DECL2 daveGetFloat(daveConnection * dc) {
    union {
  float a;
  uc b[4];
    } f;
    f.b[3]=*((uc*)dc->resultPointer++);
    f.b[2]=*((uc*)dc->resultPointer++);
    f.b[1]=*((uc*)dc->resultPointer++);
    f.b[0]=*((uc*)dc->resultPointer++);
    return (f.a);
}

int DECL2 daveGetByte(daveConnection * dc){
    return *((uc*)dc->resultPointer++);
}

int DECL2 daveGetInteger(daveConnection * dc) {
    union {
  int a;
  uc b[4];
    } f;
    f.b[3]=*((uc*)dc->resultPointer++);
    f.b[2]=*((uc*)dc->resultPointer++);
    f.b[1]=*((uc*)dc->resultPointer++);
    f.b[0]=*((uc*)dc->resultPointer++);
    return (f.a);
}

unsigned DECL2 int daveGetDWORD(daveConnection * dc) {
    union {
  unsigned int a;
  uc b[4];
    } f;
    f.b[3]=*((uc*)dc->resultPointer++);
    f.b[2]=*((uc*)dc->resultPointer++);
    f.b[1]=*((uc*)dc->resultPointer++);
    f.b[0]=*((uc*)dc->resultPointer++);
    return (f.a);
}    

unsigned DECL2 int daveGetUnsignedInteger(daveConnection * dc) {
    union {
  unsigned int a;
  uc b[4];
    } f;
    f.b[3]=*((uc*)dc->resultPointer++);
    f.b[2]=*((uc*)dc->resultPointer++);
    f.b[1]=*((uc*)dc->resultPointer++);
    f.b[0]=*((uc*)dc->resultPointer++);
    return (f.a);
}   

unsigned int DECL2 daveGetWORD(daveConnection * dc) {
    union {
  short a;
  uc b[2];
    } f;
    f.b[1]=*((uc*)dc->resultPointer++);
    f.b[0]=*((uc*)dc->resultPointer++);
    return (f.a);
}    

int DECL2 daveGetByteat(daveConnection * dc, int pos) {
    uc* p=(uc*)dc->_resultPointer;
    p+=pos;
    return *p;
}

unsigned int DECL2 daveGetWORDat(daveConnection * dc, int pos) {
    union {
  short a;
  uc b[2];
    } f;
    uc* p=(uc*)dc->_resultPointer;
    p+=pos;
    f.b[1]=*p;p++;
    f.b[0]=*p;
    return (f.a);
}

unsigned int DECL2 daveGetDWORDat(daveConnection * dc, int pos) {
    union {
  unsigned int a;
  uc b[4];
    } f;
    uc* p=(uc*)dc->_resultPointer;
    p+=pos;
    f.b[0]=*p;p++;
    f.b[1]=*p;p++;
    f.b[2]=*p;p++;
    f.b[3]=*p;
    return (f.a);
}

float DECL2 daveGetFloatat(daveConnection * dc, int pos) {
    union {
  float a;
  uc b[4];
    } f;
    uc* p=(uc*)dc->_resultPointer;
    p+=pos;
    f.b[3]=*p;p++;
    f.b[2]=*p;p++;
    f.b[1]=*p;p++;
    f.b[0]=*p;
    return (f.a);
}

float DECL2 toPLCfloat(float ff) {
    union {
  float a;
  uc b[4];
    } f;
    uc c;
    f.a=ff;
    c=f.b[0];
    f.b[0]=f.b[3];
    f.b[3]=c;
    c=f.b[1];
    f.b[1]=f.b[2];
    f.b[2]=c;
    return (f.a);
}

short DECL2 bswap_16(short ff) {
#ifdef LITTLEENDIAN
    union {
  short a;
  uc b[2];
    } f;
    uc c;
    f.a=ff;
    c=f.b[0];
    f.b[0]=f.b[1];
    f.b[1]=c;
    return (f.a);
#else
    printf("Here we are in BIG ENDIAN!!!\n");
    return (ff);
#endif    
}

int DECL2 bswap_32(int ff) {
#ifdef LITTLEENDIAN
    union {
  int a;
  uc b[4];
    } f;
    uc c;
    f.a=ff;
    c=f.b[0];
    f.b[0]=f.b[3];
    f.b[3]=c;
    c=f.b[1];
    f.b[1]=f.b[2];
    f.b[2]=c;
    return f.a;
#else
    printf("Here we are in BIG ENDIAN!!!\n");
    return ff;
#endif       
}

/**
    Timer and Counter conversion functions:
**/
/*  
    get time in seconds from current read position:
*/
float DECL2 daveGetSeconds(daveConnection * dc) {
    uc b[2],a;
    float f;
    b[1]=*(dc->resultPointer)++;
    b[0]=*(dc->resultPointer)++;
    f=b[0] & 0xf;
    f+=10*((b[0] & 0xf0)>>4);
    f+=100*(b[1] & 0xf);
    a=((b[1] & 0xf0)>>4);
    switch (a) {
  case 0: f*=0.01;break;
  case 1: f*=0.1;break;
  case 3: f*=10.0;break;
    }
    return (f);    
}
/*  
    get time in seconds from random position:
*/
float DECL2 daveGetSecondsAt(daveConnection * dc, int pos) {
    float f;
    uc b[2],a;
    uc* p=(uc*)dc->_resultPointer;
    p+=pos;
    b[1]=*p;
    p++;
    b[0]=*p;
    f=b[0] & 0xf;
    f+=10*((b[0] & 0xf0)>>4);
    f+=100*(b[1] & 0xf);
    a=((b[1] & 0xf0)>>4);
    switch (a) {
  case 0: f*=0.01;break;
  case 1: f*=0.1;break;
  case 3: f*=10.0;break;
    }
    return (f);
}
/*  
    get counter value from current read position:
*/
int DECL2 daveGetCounterValue(daveConnection * dc) {
    uc b[2];
    int f;
    b[1]=*(dc->resultPointer)++;
    b[0]=*(dc->resultPointer)++;
    f=b[0] & 0xf;
    f+=10*((b[0] & 0xf0)>>4);
    f+=100*(b[1] & 0xf);
    return (f);    
}
/*  
    get counter value from random read position:
*/
int DECL2 daveGetCounterValueAt(daveConnection * dc,int pos){
    int f;
    uc b[2];
    uc* p=(uc*)dc->_resultPointer;
    p+=pos;
    b[1]=*p;
    p++;
    b[0]=*p;
    f=b[0] & 0xf;
    f+=10*((b[0] & 0xf0)>>4);
    f+=100*(b[1] & 0xf);
    return (f);
}


/*
    dummy functions for protocols not providing a specific function:
*/

int DECL2 _daveReturnOkDummy(void * dummy){
    return 0;
}

int DECL2 _daveListReachablePartnersDummy (daveInterface * di, char * buf) {
    return 0;
}

/*
    MPI specific functions:
*/

/* 
    This writes a single chracter to the serial interface:
*/

void DECL2 _daveSendSingle(daveInterface * di,  /* serial interface */
         uc c       /* chracter to be send */
         ) 
{
    unsigned long i;
    daveWriteFile(di->fd.wfd, &c, 1, i);
}

#ifdef HAVE_SELECT
int DECL2 _daveReadSingle(daveInterface * di) {
  fd_set FDS;
  struct timeval t;
  char res;
//  t.tv_sec = 1;
//  t.tv_usec = 10;
  t.tv_sec = di->timeout / 1000000;
  t.tv_usec = (di->timeout % 1000000);

  FD_ZERO(&FDS);
  FD_SET(di->fd.rfd, &FDS);
  if(select(di->fd.rfd + 1, &FDS, NULL, NULL, &t)>0) {
      if (read(di->fd.rfd, &res, 1)==1) return res;
  }    
  return 0;
}
#endif 

#ifdef BCCWIN
int DECL2 _daveReadSingle(daveInterface * di) {
    unsigned long i;
    char res;
    ReadFile(di->fd.rfd, &res, 1, &i,NULL);
    if ((daveDebug & daveDebugSpecialChars)!=0)
  LOG3("readSingle %d chars. 1st %02X\n",i,res);
    if(i==1){
   return res;
    } else {   
  return 0;
    } 
}
#endif 

#ifdef HAVE_SELECT
int DECL2 _daveReadMPI(daveInterface * di, uc *b) {
  fd_set FDS;
  struct timeval t;
  int charsAvail;
  int res=0,state=0;
  uc bcc=0;
rep:  
//  t.tv_sec = 1;
//  t.tv_usec = 10;
  t.tv_sec = di->timeout / 1000000;
  t.tv_usec = (di->timeout % 1000000);
  
  FD_ZERO(&FDS);
  FD_SET(di->fd.rfd, &FDS);
  charsAvail = select(di->fd.rfd + 1, &FDS, NULL, NULL, &t);
  if (!charsAvail) {
      LOG1("timeout !\n");
      return (0);
  } else {
      res+= read(di->fd.rfd, b+res, 1);
      if ((res==1) && (*(b+res-1)==DLE)) {
    if ((daveDebug & daveDebugSpecialChars)!=0)
        LOG1("readMPI single DLE.\n");
    return 1;
      }   
      if ((res==1) && (*(b+res-1)==STX)) {
    if ((daveDebug & daveDebugSpecialChars)!=0)
        LOG1("readMPI single STX.\n");
          return 1;
      }
      if (*(b+res-1)==DLE) {
    if (state==0) {
        state=1;
        if ((daveDebug & daveDebugSpecialChars)!=0)
/*      LOG1("readMPI 1st DLE in data.\n") */
      ;
    } else if (state==1) {
        state=0;
        res--;    /* forget this DLE */
        if ((daveDebug & daveDebugSpecialChars)!=0)
/*            LOG1("readMPI 2nd DLE in data.\n") */
        ;
    }
      }   
      if (state==3) {
          if ((daveDebug & daveDebugSpecialChars)!=0)
      LOG3("readMPI: packet end, got BCC: %x. I calc: %x\n",*(b+res-1),bcc);
    if ((daveDebug & daveDebugRawRead)!=0)  
        _daveDump("answer",b,res);  
    return res;             
      } else {
    bcc=bcc^(*(b+res-1));
      }
        
      if (*(b+res-1)==ETX) if (state==1) {
          state=3;
          if ((daveDebug & daveDebugSpecialChars)!=0)
        LOG1("readMPI: DLE ETX,packet end.\n");
      } 
      goto rep;
  } 
    }
#endif

#ifdef BCCWIN
int DECL2 _daveReadMPI(daveInterface * di, uc *b) {
  int charsAvail, res=0,state=0;
  unsigned long i;
  uc bcc=0;
rep:  
  ReadFile(di->fd.rfd, b+res, 1, &i, NULL);
  charsAvail = (i>0);
  if (!charsAvail) {
      LOG1("timeout !\n");
      return (0);
  } else {
      res+= i;
      if ((res==1) && (*(b+res-1)==DLE)) {
    if ((daveDebug & daveDebugSpecialChars)!=0)
        LOG1("readMPI single DLE.\n");
    return 1;
      }   
      if ((res==1) && (*(b+res-1)==STX)) {
    if ((daveDebug & daveDebugSpecialChars)!=0)
        LOG1("readMPI single STX.\n");
          return 1;
      }
      if (*(b+res-1)==DLE) {
    if (state==0) {
        state=1;
        if ((daveDebug & daveDebugSpecialChars)!=0)
/*      LOG1("readMPI 1st DLE in data.\n") */
      ;
    } else if (state==1) {
        state=0;
        res--;    /* forget this DLE */
        if ((daveDebug & daveDebugSpecialChars)!=0)
/*            LOG1("readMPI 2nd DLE in data.\n") */
        ;
    }
      }   
      if (state==3) {
          if ((daveDebug & daveDebugSpecialChars)!=0)
      LOG3("readMPI: packet end, got BCC: %x. I calc: %x\n",*(b+res-1),bcc);
    if ((daveDebug & daveDebugRawRead)!=0)  
        _daveDump("answer",b,res);  
    return res;             
      } else {
    bcc=bcc^(*(b+res-1));
      }
        
      if (*(b+res-1)==ETX) if (state==1) {
          state=3;
          if ((daveDebug & daveDebugSpecialChars)!=0)
        LOG1("readMPI: DLE ETX,packet end.\n");
      } 
      goto rep;
  } 
    }
#endif

int DECL2 _daveReadMPI2(daveInterface * di, uc *b) {
  int res=_daveReadMPI(di, b);
  if (res>1) {
    _daveSendSingle(di, DLE);
    _daveSendSingle(di, STX);
  }
  return res;
}

int DECL2 _daveGetAck(daveInterface*di, int nr) {
    int res;    
    uc b1[daveMaxRawLen];
    if (daveDebug & daveDebugPacket)
  LOG2("%s enter getAck ack\n", di->name);
    res = _daveReadMPI(di, b1);
/*    LOG2("getAck ack got %d bytes.\n", res); */
    if (res<0) return res-10;
    if ((res!=di->ackPos+6) /*&& ((res!=13) && (nr==DLE))*/) {  /* fixed by Andrew Rostovtsew */
  if (daveDebug & daveDebugPrintErrors) {
          LOG4("%s *** getAck wrong length %d for ack. Waiting for %d\n dump:", di->name, res, nr);
      _daveDump("wrong ack:",b1,res);
  }
  return -1;
    } 
    if (b1[di->ackPos]!=0xB0) {
  if (daveDebug & daveDebugPrintErrors) {
          LOG3("%s *** getAck char[6] %x no ack\n", di->name, b1[di->ackPos+2]);
  }    
  return -2;
    }   
    if (b1[di->ackPos+2]!=nr) {
  if (daveDebug & daveDebugPrintErrors) {
          LOG4("%s *** getAck got: %d need: %d\n", di->name, b1[di->ackPos+2],nr);
  }    
  return -3;
    } 
/*    LOG4("%s *** getAck got: %d need: %d\n", di->name, b1[di->ackPos+2],nr); */
    return 0;
}


#define tmo_normal 95000

#ifdef HAVE_UNISTD
/* 
    This reads as many chracters as it can get and returns the number:
*/
int DECL2 _daveReadChars(daveInterface * di,  /* serial interface */
        uc *b,    /* a buffer */
        int tmo,    /* timeout in us */
        int max   /* limit */
    )
{
    int charsAvail;
    int res;
    fd_set FDS;         
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = tmo;
    FD_ZERO(&FDS);
    FD_SET(di->fd.rfd, &FDS);
    charsAvail = (select(di->fd.rfd + 1, &FDS, NULL, NULL, &t) > 0);
          
    if (!charsAvail) {
  if ((daveDebug & daveDebugRawRead)!=0)
      LOG2("%s timeout !\n", di->name);
  return (0);
    } else {
  if ((daveDebug & daveDebugRawRead)!=0)
      LOG3("%s _daveReadChars reading up to %d chars.\n", di->name,max);
  res = read(di->fd.rfd, b, max);
  return (res);
    } 
}

#else
#ifdef BCCWIN
/* 
    This reads as many chracters as it can get and returns the number:
*/
int DECL2 _daveReadChars(daveInterface * di,  /* serial interface */
        uc *b,    /* a buffer */
        int tmo,    /* timeout in us */
        int max   /* limit */
    )
{
    unsigned long res;
    setTimeOut(di, tmo);
    
    ReadFile(di->fd.rfd, b, max, &res, NULL);
    if (res<=0) {
  if ((daveDebug & daveDebugRawRead)!=0)
      LOG2("%s timeout !\n", di->name);
  return (0);
    } else {
  if ((daveDebug & daveDebugRawRead)!=0)
      LOG3("%s _daveReadChars reading up to %d chars.\n", di->name,max);
  return (res);
    } 
}

#else
#error put the code for receive with time limit for your OS here!    
#endif 
#endif

/* 
    This sends a string after doubling DLEs in the String
    and adding DLE,ETX and bcc.
*/
int DECL2 _daveSendWithCRC(daveInterface * di, /* serial interface */
        uc *b,     /* a buffer containing the message */
        int size     /* the size of the string */
    )
{   
    uc target[daveMaxRawLen]; 
    unsigned long wr;
    int i,targetSize=0;
    int bcc=DLE^ETX; /* preload */
    for (i=0; i<size; i++) {
  target[targetSize]=b[i];targetSize++;
  if (DLE==b[i]) {
      target[targetSize]=DLE;
      targetSize++;
  }  else 
      bcc=bcc^b[i]; /* The doubled DLE effectively contributes nothing */
    };
    target[targetSize]=DLE;
    target[targetSize+1]=ETX;
    target[targetSize+2]=bcc;
    targetSize+=3;
    daveWriteFile(di->fd.wfd, target, targetSize, wr);
    if (daveDebug & daveDebugPacket)
      _daveDump("_daveSendWithCRC",target, targetSize);
    return 0;
}

/* 
    This adds a prefix to a string and theen sends it
    after doubling DLEs in the String
    and adding DLE,ETX and bcc.
*/
int DECL2 _daveSendWithPrefix(daveConnection * dc, uc *b, int size)
{
    uc target[daveMaxRawLen];
    uc fix[]= {04,0x80,0x80,0x0C,0x03,0x14};
    uc fix2[]= {0x00,0x0c,0x03,0x03};
/*    if (daveDebug & daveDebugPacket)
  LOG3("%s enter _daveSendWithPrefix %d chars.\n", dc->iface->name,size); */
    if (dc->iface->protocol==daveProtoMPI2) { 
  fix2[2]=dc->connectionNumber2;    // 1/10/05 trying Andrew's patch
  fix2[3]=dc->connectionNumber;     // 1/10/05 trying Andrew's patch
  memcpy(target,fix2,sizeof(fix2));
  memcpy(target+sizeof(fix2),b,size);
  return _daveSendWithCRC(dc->iface,target,size+sizeof(fix2));
    }  else {
  fix[4]=dc->connectionNumber2;     // 1/10/05 trying Andrew's patch
  fix[5]=dc->connectionNumber;    // 1/10/05 trying Andrew's patch
      memcpy(target,fix,sizeof(fix));
  memcpy(target+sizeof(fix),b,size);
  target[1]|=dc->MPIAdr;
//  target[2]|=dc->iface->localMPI;
  memcpy(target+sizeof(fix),b,size);
  return _daveSendWithCRC(dc->iface,target,size+sizeof(fix));
    } 
}

int DECL2 _daveSendWithPrefix2(daveConnection * dc, int size)
{
    uc fix[]= {04,0x80,0x80,0x0C,0x03,0x14};
    uc fix2[]= {0x00, 0x0C, 0x03, 0x03};
    
    if (dc->iface->protocol==daveProtoMPI2) {
  fix2[2]=dc->connectionNumber2;    // 1/10/05 trying Andrew's patch
  fix2[3]=dc->connectionNumber;     // 1/10/05 trying Andrew's patch
  memcpy(dc->msgOut, fix2, sizeof(fix2));
  dc->msgOut[sizeof(fix2)]=0xF1;
/*  if (daveDebug & daveDebugPacket)
      _daveDump("_daveSendWithPrefix2",dc->msgOut, size+sizeof(fix2)); */
  return _daveSendWithCRC(dc->iface, dc->msgOut, size+sizeof(fix2));
    }
    else if (dc->iface->protocol==daveProtoMPI) {
  fix[4]=dc->connectionNumber2;   // 1/10/05 trying Andrew's patch
  fix[5]=dc->connectionNumber;    // 1/10/05 trying Andrew's patch
  memcpy(dc->msgOut, fix, sizeof(fix));
  dc->msgOut[1]|=dc->MPIAdr;
//  dc->msgOut[2]|=dc->iface->localMPI; //???
  dc->msgOut[sizeof(fix)]=0xF1;
/*  if (daveDebug & daveDebugPacket)
      _daveDump("_daveSendWithPrefix2",dc->msgOut,size+sizeof(fix)); */
  return _daveSendWithCRC(dc->iface, dc->msgOut, size+sizeof(fix));
    }
    return -1; /* shouldn't happen. */
}

/* 
    Sends an ackknowledge message for the message number nr:
*/
int DECL2 _daveSendAck(daveConnection * dc, int nr)
{
    uc m[3];
    if (daveDebug & daveDebugPacket)
  LOG3("%s sendAck for message %d \n", dc->iface->name,nr);
    m[0]=0xB0;
    m[1]=0x01;
    m[2]=nr;
    return _daveSendWithPrefix(dc, m, 3);
}

/*
    Executes part of the dialog necessary to send a message:
*/
int DECL2 _daveSendDialog2(daveConnection * dc, int size)
{
    int a;
    _daveSendSingle(dc->iface, STX);
    if (_daveReadSingle(dc->iface)!=DLE) {
        LOG2("%s *** no DLE before send.\n", dc->iface->name);      
  return -1;
    } 
    if (size>5){
//  if (dc->messageNumber==0) dc->messageNumber=1;
  dc->msgOut[dc->iface->ackPos+1]=(dc->messageNumber);  
  if(dc->msgOut[dc->iface->ackPos+6]==0)    /* do not number already numbered PDUs 12/10/04 */
      dc->msgOut[dc->iface->ackPos+6]=(dc->messageNumber+1)&0xff;
  dc->needAckNumber=dc->messageNumber;    
  dc->messageNumber++;  
  dc->messageNumber&=0xff;  /* !! */
  if (dc->messageNumber==0) dc->messageNumber=1;
    } 
    _daveSendWithPrefix2(dc, size);
    a=_daveReadSingle(dc->iface);
    if (a!=DLE) {
  LOG3("%s *** no DLE after send(1) %02x.\n", dc->iface->name,a);
  a=_daveReadSingle(dc->iface);
  if (a!=DLE) {
      LOG3("%s *** no DLE after send(2) %02x.\n", dc->iface->name,a);
      _daveSendWithPrefix2(dc, size);
      a=_daveReadSingle(dc->iface);
      if (a!=DLE) {
    LOG3("%s *** no DLE after resend(3) %02x.\n", dc->iface->name,a);
    _daveSendSingle(dc->iface, STX);
    a=_daveReadSingle(dc->iface);
    if (a!=DLE) {
            LOG2("%s *** no DLE before resend.\n", dc->iface->name);      
        return -1;
    } else {
        _daveSendWithPrefix2(dc, size);
        a=_daveReadSingle(dc->iface);
        if (a!=DLE) {
          LOG2("%s *** no DLE before resend.\n", dc->iface->name);
      return -1;
        } else {
      LOG2("%s *** got DLE after repeating whole transmisson.\n", dc->iface->name);
      return 0;
        } 
    }
      } else  
    LOG3("%s *** got DLE after resend(3) %02x.\n", dc->iface->name,a);
  }    
  
    }
    return 0;
}

int DECL2 _daveGetResponseMPI(daveConnection *dc) {
    int res;
    res= _daveReadSingle(dc->iface);
    if (res!=STX) {
  if (daveDebug & daveDebugPrintErrors) {
      LOG2("%s *** _daveGetResponseMPI no STX before answer data.\n", dc->iface->name);     
  }        
  res= _daveReadSingle(dc->iface);
    }
    _daveSendSingle(dc->iface,DLE);
    if (daveDebug & daveDebugExchange) {
        LOG2("%s _daveGetResponseMPI receive message.\n", dc->iface->name);     
    } 
    res = _daveReadMPI2(dc->iface,dc->msgIn);
/*  LOG3("%s *** _daveExchange read result %d.\n", dc->iface->name, res); */
    if (res<=0) {
  if (daveDebug & daveDebugPrintErrors) {
      LOG2("%s *** _daveGetResponseMPI no answer data.\n", dc->iface->name);      
  }        
  return -3;
    } 
    if (daveDebug & daveDebugExchange) {
        LOG3("%s _daveGetResponseMPI got %d bytes\n", dc->iface->name, dc->AnswLen);      
    }    
    if (_daveReadSingle(dc->iface)!=DLE) {
        if (daveDebug & daveDebugPrintErrors) {
            LOG2("%s *** _daveGetResponseMPI: no DLE.\n", dc->iface->name);     
        } 
  return -5;
    }    
    _daveSendAck(dc, dc->msgIn[dc->iface->ackPos+1]);
    if (_daveReadSingle(dc->iface)!=DLE) {
        if (daveDebug & daveDebugPrintErrors) {
            LOG2("%s *** _daveGetResponseMPI: no DLE after Ack.\n", dc->iface->name);     
        }
  return -6;
    }    
    return 0;
}

/*
    Sends a message and gets ackknowledge:
*/
int DECL2 _daveSendMessageMPI(daveConnection * dc, PDU * p) {
    if (daveDebug & daveDebugExchange) {
        LOG2("%s enter _daveExchangeMPI\n", dc->iface->name);     
    }    
    if (_daveSendDialog2(dc, 2+p->hlen+p->plen+p->dlen)) {
  LOG2("%s *** _daveExchangeMPI error in _daveSendDialog.\n",dc->iface->name);          
//  return -1;  
    } 
    if (daveDebug & daveDebugExchange) {
        LOG3("%s _daveExchangeMPI send done. needAck %x\n", dc->iface->name,dc->needAckNumber);     
    } 
    
    if (_daveReadSingle(dc->iface)!=STX) {
  if (daveDebug & daveDebugPrintErrors) {
      LOG2("%s *** _daveExchangeMPI no STX after _daveSendDialog.\n",dc->iface->name);
  }    
  if ( _daveReadSingle(dc->iface)!=STX) {
      if (daveDebug & daveDebugPrintErrors) {
    LOG2("%s *** _daveExchangeMPI no STX after _daveSendDialog.\n",dc->iface->name);
      } 
      return -2;
  } else {
      if (daveDebug & daveDebugPrintErrors) {
    LOG2("%s *** _daveExchangeMPI got STX after retry.\n",dc->iface->name);
      }
  }    
    }
    _daveSendSingle(dc->iface,DLE);
/*    LOG2("%s *** _daveExchangeMPI after sendSingle.\n",dc->iface->name); */
    _daveGetAck(dc->iface, dc->needAckNumber);
/*    LOG2("%s *** _daveExchangeMPI after getAck.\n",dc->iface->name); */
    _daveSendSingle(dc->iface,DLE);
/*    LOG2("%s *** _daveExchangeMPI after sendSingle.\n",dc->iface->name); */
    return 0;
}

int DECL2 _daveExchangeMPI(daveConnection * dc, PDU * p) {
    _daveSendMessageMPI(dc, p);
    dc->AnswLen=0;
    return _daveGetResponseMPI(dc);
}

/* 
    Send a string of init data to the MPI adapter.
*/
int DECL2 _daveInitStep(daveInterface * di, int nr, uc *fix, int len, char * caller) {
    _daveSendSingle(di, STX);
    if (_daveReadSingle(di)!=DLE){
  if (daveDebug & daveDebugInitAdapter)
      LOG3("%s %s no answer (DLE) from adapter.\n", di->name, caller);
        if (_daveReadSingle(di)!=DLE){
      if (daveDebug & daveDebugInitAdapter)
          LOG3("%s %s no answer (DLE) from adapter.\n", di->name, caller);
      return nr;
  }    
    }  
    if (daveDebug & daveDebugInitAdapter)
  LOG4("%s %s step %d.\n", di->name, caller, nr);
    _daveSendWithCRC(di, fix, len);
    if (_daveReadSingle(di)!=DLE) return nr+1;
    if (daveDebug & daveDebugInitAdapter)
  LOG4("%s %s step %d.\n", di->name, caller,nr+1);
    if (_daveReadSingle(di)!=STX) return nr+2;
    if (daveDebug & daveDebugInitAdapter)
  LOG4("%s %s step %d.\n", di->name, caller,nr+2);
    _daveSendSingle(di, DLE);
    return 0;
}    

/* 
    This initializes the MPI adapter. Andrew's version.
*/
int DECL2 _daveInitAdapterMPI2(daveInterface * di)  /* serial interface */
{
    uc b3[]={
  0x01,0x03,0x02,0x17, 0x00,0x9F,0x01,0x3C,
  0x00,0x90,0x01,0x14, 0x00,  /* ^^^ MaxTsdr */
  0x00,0x5,
  0x02,/* Bus speed */

  0x00,0x0F,0x05,0x01,0x01,0x03,0x80,/* from topserverdemo */
  /*^^ - Local mpi */
    };    
    
    int res;
    uc b1[daveMaxRawLen];
    b3[16]=di->localMPI;
    if (di->speed==daveSpeed500k)
  b3[7]=0x64;
    if (di->speed==daveSpeed1500k)
  b3[7]=0x96;
    b3[15]=di->speed;

//    if (daveDebug & daveDebugInitAdapter) 
//  LOG2("%s enter initAdapter(2).\n", di->name);
//    res = _daveReadChars(di, b1, 3*tmo_normal,daveMaxRawLen);
    
    res=_daveInitStep(di, 1, b3, sizeof(b3),"initAdapter()");
//    res = _daveReadChars(di, b1, tmo_normal,daveMaxRawLen);
    res= _daveReadMPI(di, b1);
    if (daveDebug & daveDebugInitAdapter) 
  LOG2("%s initAdapter() success.\n", di->name);
   _daveSendSingle(di,DLE);
    di->users=0;  /* there cannot be any connections now */
    return 0;
}

/* 
    Initializes the MPI adapter.
*/
int DECL2 _daveInitAdapterMPI1(daveInterface * di) {
    uc b2[]={
  0x01,0x0D,0x02,
    };
    us answ1[]={0x01,0x0D,0x20,'V','0','0','.','8','3'};
    us adapter0330[]={0x01,0x03,0x20,'E','=','0','3','3','0'};
    us answ2[]={0x01,0x03,0x20,'V','0','0','.','8','3'};
    
    uc b3[]={
  0x01,0x03,0x02,0x27, 0x00,0x9F,0x01,0x3C,
  0x00,0x90,0x01,0x14, 0x00,
  0x00,0x05,
  0x02,
  0x00,0x1F,0x02,0x01,0x01,0x03,0x80,
//  ^localMPI
    };    
    uc v1[]={
  0x01,0x0C,0x02,
    };
    int res;
    uc b1[daveMaxRawLen];
    if (daveDebug & daveDebugInitAdapter)
  LOG2("%s enter initAdapter(1).\n", di->name);
  
    res=_daveInitStep(di, 1, b2, sizeof(b2),"initAdapter()");
    if (res) {
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() fails.\n", di->name);
  return -44;    
    }     
    
    res= _daveReadMPI(di, b1);
    _daveSendSingle(di,DLE);
    
    if (_daveMemcmp(answ1, b1, sizeof(answ1)/2)) return 4;
    
    b3[16]=di->localMPI;

    if (di->speed==daveSpeed500k)
  b3[7]=0x64;
    if (di->speed==daveSpeed1500k)
  b3[7]=0x96;
    b3[15]=di->speed;
//    b3[sizeof(b3)-1]^=di->localMPI; /* 'patch' the checksum */
    res=_daveInitStep(di, 4, b3, sizeof(b3),"initAdapter()"); 
    if (res) {
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() fails.\n", di->name);
  return -54;    
    }
/*
    The following extra lines seem to be necessary for 
    TS adapter 6ES7 972-0CA33-0XAC:
*/        
    res= _daveReadMPI(di, b1);
    _daveSendSingle(di,DLE);    
    if (!_daveMemcmp(adapter0330, b1, sizeof(adapter0330)/2)) {
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() found Adapter E=0330.\n", di->name);
  _daveSendSingle(di,STX);
  res= _daveReadMPI2(di, b1);
  _daveSendWithCRC(di, v1, sizeof(v1)); 
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() Adapter E=0330 step 7.\n", di->name);
  if (_daveReadSingle(di)!=DLE) return 8;
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() Adapter E=0330 step 8.\n", di->name);
  res= _daveReadMPI(di, b1);
  if (res!=1 || b1[0]!=STX) return 9;
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() Adapter E=0330 step 9.\n", di->name);
  _daveSendSingle(di,DLE);
/* This needed the exact Adapter version:    */
/* instead, just read and waste it */ 
  res= _daveReadMPI(di, b1);
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() Adapter E=0330 step 10.\n", di->name);
  _daveSendSingle(di,DLE);    
  return 0;    
    
    } else if (!_daveMemcmp(answ2, b1, sizeof(answ2)/2)) {
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() success.\n", di->name);
  di->users=0;  /* there cannot be any connections now */
  return 0;
    } else {
  if (daveDebug & daveDebugInitAdapter)
      LOG2("%s initAdapter() failed.\n", di->name);
  return -56;    
    }
}

/*
    It seems to be better to complete this subroutine even if answers
    from adapter are not as expected.
*/
int DECL2 _daveDisconnectAdapterMPI(daveInterface * di) {
    int res;
    uc m2[]={
        1,4,2
    };
    uc b1[daveMaxRawLen];
    if (daveDebug & daveDebugInitAdapter) 
  LOG2("%s enter DisconnectAdapter()\n", di->name); 
    _daveSendSingle(di, STX);
    res=_daveReadMPI(di,b1);
/*    if ((res!=1)||(b1[0]!=DLE)) return -1; */
    _daveSendWithCRC(di, m2, sizeof(m2));   
    if (daveDebug & daveDebugInitAdapter) 
  LOG2("%s daveDisconnectAdapter() step 1.\n", di->name); 
    res=_daveReadMPI(di, b1);
/*    if ((res!=1)||(b1[0]!=DLE)) return -2; */
    res=_daveReadMPI(di, b1);
/*    if ((res!=1)||(b1[0]!=STX)) return -3; */
    if (daveDebug & daveDebugInitAdapter) 
  LOG2("%s daveDisconnectAdapter() step 2.\n", di->name); 
    _daveSendSingle(di, DLE);
    _daveReadChars(di, b1, tmo_normal, daveMaxRawLen);
    _daveSendSingle(di, DLE);
    if (daveDebug & daveDebugInitAdapter) 
  _daveDump("got",b1,10);
    return 0; 
}

/*
    This doesn't work yet. I'm not sure whether it is possible to get that
    list after having connected to a PLC.
*/
int DECL2 _daveListReachablePartnersMPI(daveInterface * di,char * buf) {
    uc b1[daveMaxRawLen];
    uc m1[]={1,7,2};
    int res;
    res=_daveInitStep(di, 1, m1, sizeof(m1),"listReachablePartners()"); 
    if (res) return 0;
    res=_daveReadMPI(di,b1);
//    LOG2("res %d\n", res);  
    if(136==res){
  _daveSendSingle(di,DLE);
        memcpy(buf,b1+6,126);
  return 126;
    } else
  return 0; 
}   

int DECL2 _daveDisconnectPLCMPI(daveConnection * dc)
{
    int res;
    uc m[]={
        0x80
    };
    uc b1[daveMaxRawLen];
    
    _daveSendSingle(dc->iface, STX);
    
    res=_daveReadMPI(dc->iface,b1);
    if ((res!=1)||(b1[0]!=DLE)) {
  if (daveDebug & daveDebugPrintErrors)
          LOG2("%s *** no DLE before send.\n", dc->iface->name);      
  return -1;
    }
    _daveSendWithPrefix(dc, m, 1);  
    
    res=_daveReadMPI(dc->iface,b1);
    if ((res!=1)||(b1[0]!=DLE)) {
  if (daveDebug & daveDebugPrintErrors)
      LOG2("%s *** no DLE after send.\n", dc->iface->name);     
  return -2;
    }    
    
    _daveSendSingle(dc->iface, DLE);
    
    res=_daveReadMPI(dc->iface,b1);
    if ((res!=1)||(b1[0]!=STX)) return 6;
    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveDisConnectPLC() step 6.\n", dc->iface->name);  
    res=_daveReadMPI(dc->iface,b1);
    if (daveDebug & daveDebugConnect) 
  _daveDump("got",b1,10);
    _daveSendSingle(dc->iface, DLE);
    return 0;
}    

/*
    build the PDU for a PDU length negotiation    
*/
int DECL2 _daveNegPDUlengthRequest(daveConnection * dc, PDU *p) {
    uc pa[]=  {0xF0, 0 ,0, 1, 0, 1, 3, 0xC0,};
    int res;
    PDU p2;
    p->header=dc->msgOut+dc->PDUstartO;
    _daveInitPDUheader(p,1);
    _daveAddParam(p, pa, sizeof(pa));
    if (daveDebug & daveDebugPDU) {
  _daveDumpPDU(p);
    } 
    res=_daveExchange(dc, p);
    _daveSetupReceivedPDU(dc, &p2);
    dc->maxPDUlength=daveGetU16from(p2.param+6);
    if (daveDebug & daveDebugConnect) {
  LOG2("\n*** Partner offered PDU length: %d\n\n",dc->maxPDUlength);
    } 
    return res;
}    

/* 
    Open connection to a PLC. This assumes that dc is initialized by
    daveNewConnection and is not yet used.
    (or reused for the same PLC ?)
*/
int DECL2 _daveConnectPLCMPI2(daveConnection * dc) {
    int res;
    PDU p1;
    uc b1[daveMaxRawLen];
    
    uc b4[]={
  0x00,0x0d,0x00,0x03,0xe0,0x04,0x00,0x80,
  0x00,0x02,0x01,0x06,
  0x01,
  0x00,
  0x00,0x01,
  0x02,0x03,0x01,0x00
           /*^^ MPI ADDR */
  };

    us t4[]={
  0x00,0x0c,0x103,0x103,0xd0,0x04,0x00,0x80,
  0x01,0x06,
  0x00,0x02,0x00,0x01,0x02,
  0x03,0x01,0x00,
  0x01,0x00,0x10,0x03,0x4d
    };
    uc b5[]={ 
  0x05,0x01,
    };
    
    us t5[]={    
  0x00,
  0x0c,
  0x103,0x103,0x05,0x01,0x10,0x03,0x1b
    };

    b4[3]=dc->connectionNumber; // 1/10/05 trying Andrew's patch
    b4[sizeof(b4)-3]=dc->MPIAdr;  
    t4[15]=dc->MPIAdr;
    t4[sizeof(t4)/2-1]^=dc->MPIAdr; /* 'patch' the checksum */

    _daveInitStep(dc->iface, 1, b4, sizeof(b4),"connectPLC(2)");    
    res=_daveReadMPI2(dc->iface,b1);
    if (_daveMemcmp(t4, b1, res)) return 3;
    dc->connectionNumber2=b1[3]; // 1/10/05 trying Andrew's patch

    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC() step 3.\n", dc->iface->name); 
    res=_daveReadMPI(dc->iface,b1);
    if ((res!=1)||(b1[0]!=DLE)) return 4;

    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC() step 4.\n", dc->iface->name); 
    _daveSendWithPrefix(dc, b5, sizeof(b5));    
    res=_daveReadMPI(dc->iface,b1);
    if ((res!=1)||(b1[0]!=DLE)) return 5;
    res=_daveReadMPI(dc->iface,b1);
    if ((res!=1)||(b1[0]!=STX)) return 5;
    
    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC() step 5.\n", dc->iface->name); 
    _daveSendSingle(dc->iface, DLE);
    
    res=_daveReadMPI2(dc->iface,b1);
    if (_daveMemcmp(t5, b1, res)) return 6;
    
    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC() step 6.\n", dc->iface->name); 
    
    dc->messageNumber=0;  
    res= _daveNegPDUlengthRequest(dc, &p1);
    return 0;
}

/* 
    Open connection to a PLC. This assumes that dc is initialized by
    daveNewConnection and is not yet used.
    (or reused for the same PLC ?)
*/
int DECL2 _daveConnectPLCMPI1(daveConnection * dc) {
    int res;
    PDU p1;
    uc b4[]={
  0x04,0x80,0x80,0x0D,0x00,0x14,0xE0,0x04,
  0x00,0x80,0x00,0x02,
  0x00,
  0x02,
  0x01,0x00,
  0x01,0x00,
    };

    us t4[]={
  0x04,0x80,0x180,0x0C,0x114,0x103,0xD0,0x04, // 1/10/05 trying Andrew's patch
  0x00,0x80,
  0x00,0x02,0x00,0x02,0x01,
  0x00,0x01,0x00,
    };
    uc b5[]={ 
  0x05,0x01,
    };
    us t5[]={    
  0x04,
  0x80,
  0x180,0x0C,0x114,0x103,0x05,0x01,
    };
    b4[1]|=dc->MPIAdr;  
    b4[5]=dc->connectionNumber; // 1/10/05 trying Andrew's patch
    
    t4[1]|=dc->MPIAdr;  
    t5[1]|=dc->MPIAdr;  

    _daveInitStep(dc->iface, 1, b4, sizeof(b4),"connectPLC(1)");
    
    res= _daveReadMPI2(dc->iface,dc->msgIn);
    if (_daveMemcmp(t4, dc->msgIn, sizeof(t4)/2)) return 3;
    dc->connectionNumber2=dc->msgIn[5]; // 1/10/05 trying Andrew's patch
    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC(1) step 4.\n", dc->iface->name);  
    
    if (_daveReadSingle(dc->iface)!=DLE) return 4;
    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC() step 5.\n", dc->iface->name); 
    _daveSendWithPrefix(dc, b5, sizeof(b5));    
    if (_daveReadSingle(dc->iface)!=DLE) return 5;
    if (_daveReadSingle(dc->iface)!=STX) return 5;
        
    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC() step 6.\n", dc->iface->name); 
    _daveSendSingle(dc->iface, DLE);
    res= _daveReadMPI2(dc->iface,dc->msgIn);
    if (_daveMemcmp(t5, dc->msgIn, sizeof(t5)/2)) return 6;
    if (daveDebug & daveDebugConnect) 
  LOG2("%s daveConnectPLC() step 6.\n", dc->iface->name); 
    dc->messageNumber=0;  
    res= _daveNegPDUlengthRequest(dc, &p1);
    return 0;
}

/*
    Protocol specific functions for ISO over TCP:
*/
#ifdef HAVE_SELECT
int DECL2 _daveReadOne(daveInterface * di, uc *b) {
  fd_set FDS;
  struct timeval t;
  FD_ZERO(&FDS);
  FD_SET(di->fd.rfd, &FDS);
  
  t.tv_sec = di->timeout / 1000000;
  t.tv_usec = di->timeout % 1000000;
/*  if (daveDebug & daveDebugByte) 
      LOG2("timeout %d\n",di->timeout); */
  if (select(di->fd.rfd + 1, &FDS, NULL, NULL, &t) <= 0) 
  {
      if (daveDebug & daveDebugByte) LOG1("timeout in readOne.\n");
      return (0);
  } else {
      return read(di->fd.rfd, b, 1);
  } 
};
#endif

#ifdef BCCWIN
int DECL2 _daveReadOne(daveInterface * di, uc *b) {
    unsigned long i;
    char res;
    ReadFile(di->fd.rfd, b, 1, &i,NULL);
    return i;
}
#endif 

#ifdef HAVE_SELECT
/*
    Read one complete packet. The bytes 3 and 4 contain length information.
    This version needs a socket filedescriptor that is set to O_NONBLOCK or
    it will hang, if there are not enough bytes to read.
    The advantage may be that the timeout is not used repeatedly.
*/
int DECL2 _daveReadISOPacket(daveInterface * di,uc *b) {
  int res,length;
  fd_set FDS;
  struct timeval t;
  FD_ZERO(&FDS);
  FD_SET(di->fd.rfd, &FDS);
  
  t.tv_sec = di->timeout / 1000000;
  t.tv_usec = di->timeout % 1000000;
  if (select(di->fd.rfd + 1, &FDS, NULL, NULL, &t) <= 0) {
      if (daveDebug & daveDebugByte) LOG1("timeout in ReadISOPacket.\n");
      return 0;
  } else {
  res=read(di->fd.rfd, b, 4);
        if (res<4) {
      if (daveDebug & daveDebugByte) {
    LOG2("res %d ",res);
    _daveDump("readISOpacket: short packet", b, res);
      }
      return (0); /* short packet */
  }
  length=b[3]+0x100*b[2];
  res+=read(di->fd.rfd, b+4, length-4);
  if (daveDebug & daveDebugByte) {
      LOG3("readISOpacket: %d bytes read, %d needed\n",res, length);
      _daveDump("readISOpacket: packet", b, res);    
  }
  return (res);
    }
}

int DECL2 _daveReadIBHPacket(daveInterface * di,uc *b) {
    int res,length;
    fd_set FDS;
    struct timeval t;
    FD_ZERO(&FDS);
    FD_SET(di->fd.rfd, &FDS);
  
    t.tv_sec = di->timeout / 1000000;
    t.tv_usec = di->timeout % 1000000;
    if (select(di->fd.rfd + 1, &FDS, NULL, NULL, &t) <= 0) {
        if (daveDebug & daveDebugByte) LOG1("timeout in ReadIBHPacket.\n");
      return 0;
    } else {
        res=read(di->fd.rfd, b, 3);
  if (res<3) {
      if (daveDebug & daveDebugByte) {
          LOG2("res %d ",res);
          _daveDump("readIBHpacket: short packet", b, res);
      }
      return (0); /* short packet */
  }
  length=b[2]+8; //b[3]+0x100*b[2];
  res+=read(di->fd.rfd, b+3, length-3);
  if (daveDebug & daveDebugByte) {
      LOG3("readIBHpacket: %d bytes read, %d needed\n",res, length);
      _daveDump("readIBHpacket: packet", b, res);    
  }
  return (res);
    }
}
#endif /* HAVE_SELECT */

#ifdef BCCWIN

int DECL2 _daveReadISOPacket(daveInterface * di,uc *b) {
  int res,i,length;
  i=recv((SOCKET)(di->fd.rfd), b, 4, 0);
  res=i;
  if (res <= 0) {
      if (daveDebug & daveDebugByte) LOG1("timeout in ReadISOPacket.\n");
      return 0;
  } else {
          if (res<4) {
      if (daveDebug & daveDebugByte) {
    LOG2("res %d ",res);
    _daveDump("readISOpacket: short packet", b, res);
      }
      return (0); /* short packet */
  }
  length=b[3]+0x100*b[2];
  i=recv((SOCKET)(di->fd.rfd), b+4, length-4, 0);
  res+=i;
  if (daveDebug & daveDebugByte) {
      LOG3("readISOpacket: %d bytes read, %d needed\n",res, length);
      _daveDump("readISOpacket: packet", b, res);    
  }
  return (res);
    }
}

int DECL2 _daveReadIBHPacket(daveInterface * di,uc *b) {
    int res,length;
    fd_set FDS;
    struct timeval t;
    FD_ZERO(&FDS);
    FD_SET((SOCKET)(di->fd.rfd), &FDS);
  
    t.tv_sec = di->timeout / 1000000;
    t.tv_usec = di->timeout % 1000000;
    if (select(/*di->fd.rfd +*/ 1, &FDS, NULL, NULL, &t) <= 0) {
        if (daveDebug & daveDebugByte) LOG1("timeout in ReadIBHPacket.\n");
      return 0;
    } else {
//  res=read(di->fd.rfd, b, 3);
  res=recv((SOCKET)(di->fd.rfd), b, 3, 0);
        if (res<3) {
      if (daveDebug & daveDebugByte) {
    LOG2("res %d ",res);
    _daveDump("readIBHpacket: short packet", b, res);
      }
      return (0); /* short packet */
  }
  length=b[2]+8; //b[3]+0x100*b[2];
//  res+=read(di->fd.rfd, b+3, length-3);
  res+=recv((SOCKET)(di->fd.rfd), b+3, length-3, 0);
  if (daveDebug & daveDebugByte) {
      LOG3("readIBHpacket: %d bytes read, %d needed\n",res, length);
      _daveDump("readIBHpacket: packet", b, res);    
  }
  return (res);
    }
}

/*
int DECL2 _daveReadIBHPacket(daveInterface * di,uc *b) {
  int res,i,length;
  i=recv((SOCKET)(di->fd.rfd), b, 3, 0);
  res=i;
  if (res <= 0) {
      if (daveDebug & daveDebugByte) LOG1("timeout in ReadIBHPacket.\n");
      return 0;
  } else {
          if (res<3) {
      if (daveDebug & daveDebugByte) {
    LOG2("res %d ",res);
    _daveDump("readIBHpacket: short packet", b, res);
      }
      return (0); // short packet 
  }
  length=b[2]+8; //b[3]+0x100*b[2];
  i=recv((SOCKET)(di->fd.rfd), b+3, length-3, 0);
  res+=i;
  if (daveDebug & daveDebugByte) {
      LOG3("readIBHpacket: %d bytes read, %d needed\n",res, length);
      _daveDump("readIBHpacket: packet", b, res);    
  }
  return (res);
    }
}
*/

#endif /* */

int DECL2 _daveSendISOPacket(daveConnection * dc, int size) {
    unsigned long i;
    size+=4;
    *(dc->msgOut+3)=size % 0xFF;
    *(dc->msgOut+2)=size / 0x100;
    *(dc->msgOut+1)=0;
    *(dc->msgOut+0)=3;
    if(dc->msgOut[5]==0xF0) {
  if (dc->messageNumber==0xff) {  
      dc->msgOut[11]=0xcc;
      dc->msgOut[12]=0xc1;
      dc->messageNumber++;  
  }    
    } else  
  dc->messageNumber=0xff; 
  
    if (daveDebug & daveDebugByte) 
  _daveDump("send packet: ",dc->msgOut,size);
#ifdef HAVE_SELECT
    daveWriteFile(dc->iface->fd.wfd, dc->msgOut, size, i);
#endif    
#ifdef BCCWIN
    send((unsigned int)(dc->iface->fd.wfd), dc->msgOut, size, 0);
#endif
    return 0;
}

int DECL2 _daveGetResponseISO_TCP(daveConnection * dc) {
    return _daveReadISOPacket(dc->iface,dc->msgIn);
}
/*
    Executes the dialog around one message:
*/
int DECL2 _daveExchangeTCP(daveConnection * dc, PDU * p) {
    int res;
    PDU p2;
    if (daveDebug & daveDebugExchange) {
        LOG2("%s enter _daveExchangeTCP\n", dc->iface->name);     
    }    
    *(dc->msgOut+6)=0x80;
    *(dc->msgOut+5)=0xf0;
    *(dc->msgOut+4)=0x02;
    _daveSendISOPacket(dc,3+p->hlen+p->plen+p->dlen);
    res=_daveReadISOPacket(dc->iface,dc->msgIn);
    
    if (res>0) {
   _daveSetupReceivedPDU(dc, &p2);
  if (daveDebug & daveDebugExchange) {
      LOG3("%s _daveExchangeTCP got %d result bytes\n", dc->iface->name, dc->AnswLen);
  }    
  return 0;
    }
    return -1;
}

int DECL2 _daveConnectPLCTCP(daveConnection * dc) {
    int res, success, retries;
    uc b4[]={
  0x11,0xE0,0x00,
  0x00,0x00,0x01,0x00,
  0xC1,2,1,0,
  0xC2,2,0,1,
  0xC0,1,9,
    };
    uc b243[]={
  0x11,0xE0,0x00,
  0x00,0x00,0x01,0x00,
  0xC1,2,'M','W',
  0xC2,2,'M','W',
  0xC0,1,9,
    };
    
    PDU p1; 
    success=0;
    retries=0;
    do {
  if (dc->iface->protocol==daveProtoISOTCP243) {
      memcpy(dc->msgOut+4, b243, sizeof(b243)); 
  } else {
      memcpy(dc->msgOut+4, b4, sizeof(b4)); 
      
//      printf("******** do inc %d\n",a);
      dc->msgOut[17]=dc->rack+1;
      dc->msgOut[18]=dc->slot;
  } 
        _daveSendISOPacket(dc, sizeof(b4)); /* sizes are identical */
  res=_daveReadISOPacket(dc->iface,dc->msgIn);
        if (daveDebug & daveDebugConnect) {
      LOG2("%s daveConnectPLC() step 1. ", dc->iface->name);  
      _daveDump("got packet: ", dc->msgIn, res);
  }
  if (res==22) {
      success=1;
  } else {
      if (daveDebug & daveDebugPrintErrors){
    LOG2("%s error in daveConnectPLC() step 1. retrying...", dc->iface->name);  
      } 
  }
  retries++;
    } while ((success==0)&&(retries<3));
    if (success==0) return -1;
    
    retries=0;
    do {
  res= _daveNegPDUlengthRequest(dc, &p1);
  if (res==0) {
      return res;
  } else {
      if (daveDebug & daveDebugPrintErrors){
    LOG2("%s error in daveConnectPLC() step 1. retrying...", dc->iface->name);  
      } 
  }
    } while (retries<3);  
    return -1;
}

/*
    Changes: 
    07/19/04 removed unused vars.
*/

/*
    Changes: 
    07/19/04 added return values in daveInitStep and daveSendWithPrefix2.
    09/09/04 applied patch for variable Profibus speed from Andrew Rostovtsew.
*/

/* PPI specific functions: */
#define tmo_normalPPI 140000

void DECL2 _daveSendLength(daveInterface * di, int len) {
    uc c[]={104,0,0,104};
    unsigned long i;
    c[1]=len;
    c[2]=len;
    daveWriteFile(di->fd.wfd, c, 4, i);
    if ((daveDebug & daveDebugByte)!=0) {
  _daveDump("I send", c, 4);
    } 
}

void DECL2 _daveSendIt(daveInterface * di, uc * b, int size) {
    unsigned long i;
    us sum = 0;
    for (i=0;i<size;i++) {
  sum+=b[i];
    }
    sum=sum & 0xff;
    b[size]=sum;
    size++;
    b[size]=SYN;
    size++;
    daveWriteFile(di->fd.wfd, b, size, i);
    
    if ((daveDebug & daveDebugByte)!=0) {
  LOG2("send %ld\n",i);
  _daveDump("I send", b, size);
    } 
}

void DECL2 _daveSendRequestData(daveConnection * dc,int alt) {
    uc b[]={DLE,0,0,0x5C,0,0};
    unsigned long i;
    b[1]=dc->MPIAdr;
    b[2]=dc->iface->localMPI;
    if(alt) b[3]=0x7c; else b[3]=0x5c;
    daveWriteFile(dc->iface->fd.wfd, b, 1, i);
    _daveSendIt(dc->iface, b+1, sizeof(b)-3);
}


int seconds, thirds;

int _daveGetResponsePPI(daveConnection *dc) {
    int res, expectedLen, expectingLength, i, sum, alt;
    uc * b;
    res = 0;
    expectedLen=6;
    expectingLength=1;
    b=dc->msgIn;
    alt=1;
    while ((expectingLength)||(res<expectedLen)) {
  i = _daveReadChars(dc->iface, dc->msgIn+res, 2000000, daveMaxRawLen);
  res += i;
  if ((daveDebug & daveDebugByte)!=0) {
      LOG3("i:%d res:%d\n",i,res);
      FLUSH;
  }        
  if (i == 0) {
//            return 512
      ;
  } else {
      if ( (expectingLength) && (res==1) && (b[0] == 0xE5)) {
    if(alt) {
        _daveSendRequestData(dc,alt);
        res=0;
        alt=0;
    } else {
        _daveSendRequestData(dc,alt);
        res=0;
        alt=1;
    }
      }
      if ( (expectingLength) && (res>=4) && (b[0] == b[3]) && (b[1] == b[2]) ) {
          expectedLen=b[1]+6;
          expectingLength=0;
      }
  } 
    }
    if ((daveDebug & daveDebugByte)!=0) {
  LOG2("res %d testing lastChar\n",res);
    } 
    if (b[res-1]!=SYN) {
      LOG1("block format error\n");
      return 1024;
    }
    if ((daveDebug & daveDebugByte)!=0) {
  LOG1("testing check sum\n");
    } 
    sum=0;
    for (i=4; i<res-2; i++){
        sum+=b[i];
    }
    sum=sum&0xff;
    if ((daveDebug & daveDebugByte)!=0) {
  LOG3("I calc: %x sent: %x\n", sum, b[res-2]);
    } 
    if (b[res-2]!=sum) {
  if ((daveDebug & daveDebugByte)!=0) {
          LOG1("checksum error\n");
  } 
        return 2048;
    }
    return 0;
} 

int DECL2 _daveExchangePPI(daveConnection * dc,PDU * p1) {
    int i,res=0,len;
    dc->msgOut[0]=dc->MPIAdr; /* address ? */
    dc->msgOut[1]=dc->iface->localMPI;  
    dc->msgOut[2]=108;  
    len=3+p1->hlen+p1->plen+p1->dlen; /* The 3 fix bytes + all parts of PDU */
    _daveSendLength(dc->iface, len);      
    _daveSendIt(dc->iface, dc->msgOut, len);
//    i = _daveReadChars(dc->iface, dc->msgIn+res, tmo_normalPPI, daveMaxRawLen);
    i = _daveReadChars(dc->iface, dc->msgIn+res, tmo_normalPPI, 1);
    if ((daveDebug & daveDebugByte)!=0) {
  LOG3("i:%d res:%d\n",i,res);
  _daveDump("got",dc->msgIn,i); // 5.1.2004
    } 
    if (i == 0) {
  seconds++;
  _daveSendLength(dc->iface, len);      
  _daveSendIt(dc->iface, dc->msgOut, len);
  i = _daveReadChars(dc->iface, dc->msgIn+res, 2*tmo_normalPPI, daveMaxRawLen);
  if (i == 0) {
      thirds++;
      _daveSendLength(dc->iface, len);      
      _daveSendIt(dc->iface, dc->msgOut, len);
      i = _daveReadChars(dc->iface, dc->msgIn+res, 4*tmo_normalPPI, daveMaxRawLen);
      if (i == 0) {
    LOG1("timeout in _daveExchangePPI!\n");
    FLUSH;
        return 512;
      } 
  }    
    }
    _daveSendRequestData(dc,0); 
    return _daveGetResponsePPI(dc);
}    

int DECL2 _daveConnectPLCPPI(daveConnection * dc) {
    PDU p;
    return _daveNegPDUlengthRequest(dc,&p);
} 

/* 
    "generic" functions calling the protocol specific ones (or the dummies)
*/
int DECL2 daveInitAdapter(daveInterface * di) {
    return di->initAdapter(di);
}

int DECL2 daveConnectPLC(daveConnection * dc) {
    return dc->iface->connectPLC(dc);
}

int DECL2 daveDisconnectPLC(daveConnection * dc) {
    return dc->iface->disconnectPLC(dc);
}

int DECL2 daveDisconnectAdapter(daveInterface * di) {
    return di->disconnectAdapter(di);
}

int DECL2 _daveExchange(daveConnection * dc, PDU *p) {
    return dc->iface->exchange(dc, p);
}

int DECL2 daveSendMessage(daveConnection * dc, PDU *p) {
    return dc->iface->sendMessage(dc, p);
}

int DECL2 daveListReachablePartners(daveInterface * di, char * buf) {
    return di->listReachablePartners(di, buf);
}

int DECL2 daveGetResponse(daveConnection * dc) {
    return dc->iface->getResponse(dc);
}

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
EXPORTSPEC int DECL2 daveGetS8from(uc *b) {
    char* p=(char*)b;
    return *p;
}

EXPORTSPEC int DECL2 daveGetU8from(uc *b) {
    return *b;
}

EXPORTSPEC int DECL2 daveGetS16from(uc *b) {
    union {
  short a;
  uc b[2];
    } u;
    u.b[1]=*b;
    b++;
    u.b[0]=*b;
    return u.a;
}

EXPORTSPEC int DECL2 daveGetU16from(uc *b) {
    union {
  unsigned short a;
  uc b[2];
    } u;
    u.b[1]=*b;
    b++;
    u.b[0]=*b;
    return u.a;
}

EXPORTSPEC int DECL2 daveGetS32from(uc *b) {
    union {
  int a;
  uc b[4];
    } u;
    u.b[3]=*b;
    b++;
    u.b[2]=*b;
    b++;
    u.b[1]=*b;
    b++;
    u.b[0]=*b;
    return u.a;
}

EXPORTSPEC unsigned int DECL2 daveGetU32from(uc *b) {
    union {
  unsigned int a;
  uc b[4];
    } u;
    u.b[3]=*b;
    b++;
    u.b[2]=*b;
    b++;
    u.b[1]=*b;
    b++;
    u.b[0]=*b;
    return u.a;
}

EXPORTSPEC float DECL2 daveGetFloatfrom(uc *b) {
    union {
  float a;
  uc b[4];
    } u;
    u.b[3]=*b;
    b++;
    u.b[2]=*b;
    b++;
    u.b[1]=*b;
    b++;
    u.b[0]=*b;
    return u.a;
}

/*
    Get a value from the current position in the last result read on the connection dc.
    This will increment an internal pointer, so the next value is read from the position
    following this value.
*/
EXPORTSPEC int DECL2 daveGetS8(daveConnection * dc) {
    char * p=(char *) dc->resultPointer;
    dc->resultPointer++;
    return *p;
}

EXPORTSPEC int DECL2 daveGetU8(daveConnection * dc) {
    uc * p=dc->resultPointer;
    dc->resultPointer++;
    return *p;
}    

EXPORTSPEC int DECL2 daveGetS16(daveConnection * dc) {
    union {
  short a;
  uc b[2];
    } u;
    u.b[1]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[0]=*(dc->resultPointer);
    dc->resultPointer++;
    return u.a;
}

EXPORTSPEC int DECL2 daveGetU16(daveConnection * dc) {
    union {
  unsigned short a;
  uc b[2];
    } u;
    u.b[1]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[0]=*(dc->resultPointer);
    dc->resultPointer++;
    return u.a;
}

EXPORTSPEC int DECL2 daveGetS32(daveConnection * dc) {
    union {
  int a;
  uc b[4];
    } u;
    u.b[3]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[2]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[1]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[0]=*(dc->resultPointer);
    dc->resultPointer++;
    return u.a;
}

EXPORTSPEC unsigned int DECL2 daveGetU32(daveConnection * dc) {
    union {
  unsigned int a;
  uc b[4];
    } u;
    u.b[3]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[2]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[1]=*(dc->resultPointer);
    dc->resultPointer++;
    u.b[0]=*(dc->resultPointer);
    dc->resultPointer++;
    return u.a;
}
/*
    Get a value from a given position in the last result read on the connection dc.
*/
EXPORTSPEC int DECL2 daveGetS8at(daveConnection * dc, int pos) {
    char * p=(char *)(dc->_resultPointer);
    p+=pos;
    return *p;
}

EXPORTSPEC int DECL2 daveGetU8at(daveConnection * dc, int pos)  {
    uc * p=(uc *)(dc->_resultPointer);
    p+=pos;
    return *p;
}

EXPORTSPEC int DECL2 daveGetS16at(daveConnection * dc, int pos) {
    union {
  short a;
  uc b[2];
    } u;
    uc * p=(uc *)(dc->_resultPointer);
    p+=pos;
    u.b[1]=*p;
    p++;
    u.b[0]=*p;
    return u.a;
}

EXPORTSPEC int DECL2 daveGetU16at(daveConnection * dc, int pos) {
    union {
  unsigned short a;
  uc b[2];
    } u;
    uc * p=(uc *)(dc->_resultPointer);
    p+=pos;
    u.b[1]=*p;
    p++;
    u.b[0]=*p;
    return u.a;
}

EXPORTSPEC int DECL2 daveGetS32at(daveConnection * dc, int pos) {
    union {
  int a;
  uc b[4];
    } u;
    uc * p=dc->_resultPointer;
    p+=pos;
    u.b[3]=*p;
    p++;
    u.b[2]=*p;
    p++;
    u.b[1]=*p;
    p++;
    u.b[0]=*p;
    return u.a;
}

EXPORTSPEC unsigned int DECL2 daveGetU32at(daveConnection * dc, int pos) {
    union {
  unsigned int a;
  uc b[4];
    } u;
    uc * p=(uc *)(dc->_resultPointer);
    p+=pos;
    u.b[3]=*p;
    p++;
    u.b[2]=*p;
    p++;
    u.b[1]=*p;
    p++;
    u.b[0]=*p;
    return u.a;
}
/*
    put one byte into buffer b:
*/
EXPORTSPEC uc * DECL2 davePut8(uc *b,int v) {
    *b = v & 0xff;
    b++;
    return b;
}

EXPORTSPEC uc * DECL2 davePut16(uc *b,int v) {
    union {
  short a;
  uc b[2];
    } u;
    u.a=v;
    *b=u.b[1];
    b++;
    *b=u.b[0];
    b++;
    return b;
}

EXPORTSPEC uc * DECL2 davePut32(uc *b, int v) {
    union {
  int a;
  uc b[2];
    } u;
    u.a=v;
    *b=u.b[3];
    b++;
    *b=u.b[2];
    b++;
    *b=u.b[1];
    b++;
    *b=u.b[0];
    b++;
    return b;
}

EXPORTSPEC uc * DECL2 davePutFloat(uc *b,float v) {
    union {
  float a;
  uc b[2];
    } u;
    u.a=v;
    *b=u.b[3];
    b++;
    *b=u.b[2];
    b++;
    *b=u.b[1];
    b++;
    *b=u.b[0];
    b++;
    return b;
}

EXPORTSPEC void DECL2 davePut8at(uc *b, int pos, int v) {
    union {
  short a;
  uc b[2];
    } u;
    u.a=v;
    b+=pos;
    *b=v & 0xff;
}

EXPORTSPEC void DECL2 davePut16at(uc *b, int pos, int v) {
    union {
  short a;
  uc b[2];
    } u;
    u.a=v;
    b+=pos;
    *b=u.b[1];
    b++;
    *b=u.b[0];
}

EXPORTSPEC void DECL2 davePut32at(uc *b, int pos, int v) {
    union {
  int a;
  uc b[2];
    } u;
    u.a=v;
    b+=pos;
    *b=u.b[3];
    b++;
    *b=u.b[2];
    b++;
    *b=u.b[1];
    b++;
    *b=u.b[0];
}

EXPORTSPEC void DECL2 davePutFloatat(uc *b, int pos,float v) {
    union {
  float a;
  uc b[2];
    } u;
    u.a=v;
    b+=pos;
    *b=u.b[3];
    b++;
    *b=u.b[2];
    b++;
    *b=u.b[1];
    b++;
    *b=u.b[0];
}

userReadFunc readCallBack=NULL;
userWriteFunc writeCallBack=NULL;

void _daveConstructReadResponse(PDU * p) {
    uc pa[]={4,1}; 
    uc da[]={0xFF,4,0,0}; 
    _daveInitPDUheader(p,3);
    _daveAddParam(p, pa, sizeof(pa));
    _daveAddData(p, da, sizeof(da));    
}

void _daveConstructBadReadResponse(PDU * p) {
    uc pa[]={4,1}; 
    uc da[]={0x0A,0,0,0}; 
    _daveInitPDUheader(p,3);
    _daveAddParam(p, pa, sizeof(pa));
    _daveAddData(p, da, sizeof(da));    
}

void _daveConstructWriteResponse(PDU * p) {
    uc pa[]={5,1}; 
    uc da[]={0xFF}; 
    _daveInitPDUheader(p,3);
    _daveAddParam(p, pa, sizeof(pa));
    _daveAddData(p, da, sizeof(da));    
}

void _daveHandleRead(PDU * p1,PDU * p2) {
    int result;
    uc * userBytes;
    int bytes=0x100*p1->param[6]+p1->param[7];
    int DBnumber=0x100*p1->param[8]+p1->param[9];
    int area=p1->param[10];
    int start=0x10000*p1->param[11]+0x100*p1->param[12]+p1->param[13];
    LOG5("read %d bytes from %s %d beginning at %d.\n",
  bytes, daveAreaName(area),DBnumber,start);
    if (readCallBack) 
  userBytes=readCallBack(area, DBnumber,start, bytes, &result); 
    _daveConstructReadResponse(p2); 
    _daveAddValue(p2, userBytes, bytes);
    _daveDumpPDU(p2);
};

void _daveHandleWrite(PDU * p1,PDU * p2) {
    int result,bytes=0x100*p1->param[6]+p1->param[7];
    int DBnumber=0x100*p1->param[8]+p1->param[9];
    int area=p1->param[10];
    int start=0x10000*p1->param[11]+0x100*p1->param[12]+p1->param[13];
    LOG5("write %d bytes to %s %d beginning at %d.\n",
  bytes, daveAreaName(area),DBnumber,start);
    if (writeCallBack)  
  writeCallBack(area, DBnumber,start, bytes, &result, p1->data+4);  
    LOG1("after callback\n");
    FLUSH;
    _daveConstructWriteResponse(p2);  
    LOG1("after ConstructWriteResponse()\n");
    FLUSH;
    _daveDumpPDU(p2);
    LOG1("after DumpPDU()\n");
    FLUSH;
};


/*
    10/04/2003  PPI has an address. Implemented now.
    06/03/2004  Fixed a bug in _davePPIexchange, which caused timeouts
    when the first call to readChars returned less then 4 characters.
*/

EXPORTSPEC int _daveWriteIBH(daveInterface * di, void * buffer, int len) {
    int res;
    if (daveDebug & daveDebugByte) {
  _daveDump("writeIBH: ", buffer, len);
    } 
#ifdef HAVE_SELECT    
    res=write(di->fd.wfd, buffer, len);
#endif    
#ifdef BCCWIN
//    res=send((SOCKET)(di->fd.wfd), buffer, ((len+1)/2)*2, 0);
    res=send((SOCKET)(di->fd.wfd), buffer, len, 0);
#endif
    return res;
}    

int _davePackPDU(daveConnection * dc,PDU *p) {
    IBHpacket * ibhp;
    MPIheader * hm= (MPIheader*) (dc->msgOut+sizeof(IBHpacket)); // MPI headerPDU begins packet header
//    LOG1("enter packPDU\n");
    memcpy(hm, &(dc->templ), sizeof(MPIheader));  // copy MPI header from template
//    LOG1("packPDU 3\n");
//    hm->MPI=dc->MPIAdr;       // set MPI address of PLC //?????
    hm->len=2+p->hlen+p->plen+p->dlen;    // set MPI length
    hm->func=0xf1;        // set MPI "function code"
//    LOG1("packPDU 1\n");
    ibhp = (IBHpacket*) dc->msgOut;
    ibhp->ch1=7;
    ibhp->ch2=0xff;
    ibhp->len=hm->len+5;
//    LOG1("packPDU 2\n");
    if(p->header[4]==0)
  p->header[4]=1+hm->packetNumber;    // give the PDU a number
//    LOG1("packPDU 3\n");
    ibhp->packetNumber=dc->packetNumber;
//    LOG1("packPDU 4\n");
    dc->packetNumber++;
//    LOG1("packPDU 5\n");
    ibhp->rFlags=0x82;
    ibhp->sFlags=0;
    dc->templ.packetNumber++; 
    if(dc->templ.packetNumber==0)dc->templ.packetNumber=1;
//    LOG1("leave packPDU\n");
    return 0;
}    

uc _MPIack[]={
  0x07,0xff,0x08,0x05,0x00,0x00,0x82,0x00, 0x15,0x14,0x02,0x00,0x03,0xb0,0x01,0x00,
};


void _daveSendMPIAck_IBH(daveConnection*dc) {
    _MPIack[15]=dc->msgIn[16];
    _MPIack[8]=dc->templ.src_conn;
    _MPIack[9]=dc->templ.dst_conn;
    _MPIack[10]=dc->templ.MPI;
    _daveWriteIBH(dc->iface,_MPIack,sizeof(_MPIack));
}

/*
    send a network level ackknowledge
*/
void _daveSendIBHNetAck(daveConnection * dc) {
//    0xff,0x07,0x05,0x02,0x82,0x00,0x00,0x00, 0x14,0x00,0x03,0x01,0x09,
    IBHpacket * p;
    uc ack[13];
    memcpy(ack, dc->msgIn, sizeof(ack));
    p= (IBHpacket*) ack;
//    p->sFlags; p->sFlags=p->rFlags; p->rFlags=d;
//    p->ch1; p->ch1=p->ch2; p->ch2=c;    // certainly nonsense, but I cannot test it
            // at the moment, and because it DID work,
            // I'll leave it as it is.
    p->len=sizeof(ack)-sizeof(IBHpacket);
    ack[11]=1;
    ack[12]=9;
//    LOG2("Sending net level ack for number: %d\n",p->packetNumber);
    _daveWriteIBH(dc->iface, ack,sizeof(ack));
}


uc _MPIconnectResponse[]={ 
  0xff,0x07,0x13,0x00,0x00,0x00,0xc2,0x02, 0x14,0x14,0x03,0x00,0x00,0x22,0x0c,0xd0,
  0x04,0x00,0x80,0x00,0x02,0x00,0x02,0x01, 0x00,0x01,0x00,
};

/*
    packet analysis. mixes all levels.
*/
int __daveAnalyze(daveConnection * dc) {
    int haveResp;
    IBHpacket * p, *p2;
    MPIheader * pm;
    MPIheader2 * m2;
    PDU p1,pr;
    uc resp[2000];
    
//    if (dc->AnswLen==0) return _davePtEmpty;
    haveResp=0;
    
    p= (IBHpacket*) dc->msgIn;
    dc->needAckNumber=-1;   // Assume no ack
    if (daveDebug & daveDebugPacket){
  LOG2("Channel: %d\n",p->ch1);
  LOG2("Channel: %d\n",p->ch2);
        LOG2("Length:  %d\n",p->len);
  LOG2("Number:  %d\n",p->packetNumber);
        LOG3("sFlags:  %04x rFlags:%04x\n",p->sFlags,p->rFlags);
    }    
    if (p->rFlags==0x82) {
  pm= (MPIheader*) (dc->msgIn+sizeof(IBHpacket));
  if (daveDebug & daveDebugMPI){    
      LOG2("srcconn: %d\n",pm->src_conn);
      LOG2("dstconn: %d\n",pm->dst_conn);
      LOG2("MPI:     %d\n",pm->MPI);
      LOG2("MPI len: %d\n",pm->len);
      LOG2("MPI func:%d\n",pm->func);
  }    
  if (pm->func==0xf1) {
      if (daveDebug & daveDebugMPI)    
    LOG2("MPI packet number: %d needs ackknowledge\n",pm->packetNumber);
      dc->needAckNumber=pm->packetNumber;
      _daveSetupReceivedPDU(dc, &p1);
// construct response:      
      pr.header=resp+sizeof(IBHpacket)+sizeof(MPIheader2);
      p2= (IBHpacket*) resp;
      p2->ch1=p->ch2;
      p2->ch2=p->ch1;
      p2->packetNumber=0;
      p2->sFlags=0;
      p2->rFlags=0x2c2;
      
      m2= (MPIheader2*) (resp+sizeof(IBHpacket));
      m2->src_conn=pm->src_conn;
      m2->dst_conn=pm->dst_conn;
      m2->MPI=pm->MPI;
      m2->xxx1=0;
      m2->xxx2=0;
      m2->xx22=0x22;
      if (p1.param[0]==daveFuncRead) {
#ifdef passiveMode      
    _daveHandleRead(&p1,&pr);
    haveResp=1;
#endif
    m2->len=pr.hlen+pr.plen+pr.dlen+2;
    p2->len=m2->len+7;
      } else if (p1.param[0]==daveFuncWrite) {
#ifdef passiveMode      
    _daveHandleWrite(&p1,&pr);
    haveResp=1;
#endif    
    m2->len=pr.hlen+pr.plen+pr.dlen+2;
    p2->len=m2->len+7;
      } else {
    if (daveDebug & daveDebugPDU)    
        LOG2("Unsupported PDU function code: %d\n",p1.param[0]);
    return _davePtUnknownPDUFunc;
      }
      
  }
  if (pm->func==0xb0) {
          LOG2("Ackknowledge for packet number: %d\n",*(dc->msgIn+15));
      return _davePtMPIAck;
  }
  if (pm->func==0xe0) {
      LOG2("Connect to MPI: %d\n",pm->MPI);
      memcpy(resp, _MPIconnectResponse, sizeof(_MPIconnectResponse));
      resp[8]=pm->src_conn;
      resp[9]=pm->src_conn;
      resp[10]=pm->MPI;
      haveResp=1;
  }    
    } 
    
    if (p->rFlags==0x2c2) {
  MPIheader2 * pm= (MPIheader2*) (dc->msgIn+sizeof(IBHpacket));
  if (daveDebug & daveDebugMPI) {
      LOG2("srcconn: %d\n",pm->src_conn);
      LOG2("dstconn: %d\n",pm->dst_conn);
      LOG2("MPI:     %d\n",pm->MPI);
      LOG2("MPI len: %d\n",pm->len);
      LOG2("MPI func:%d\n",pm->func);
  }
  if (pm->func==0xf1) {
      if (daveDebug & daveDebugMPI)    
    LOG1("analyze 1\n");
      dc->needAckNumber=pm->packetNumber;
      dc->PDUstartI= sizeof(IBHpacket)+sizeof(MPIheader2);
      _daveSendMPIAck_IBH(dc);
      
      return 55;
      
/*  
      if (daveDebug & daveDebugMPI)    
    LOG2("MPI packet number: %d\n",pm->packetNumber);
      dc->needAckNumber=pm->packetNumber;
//      p1.header=((uc*)pm)+sizeof(MPIheader2);
      dc->PDUstartI= sizeof(IBHpacket)+sizeof(MPIheader2);
      _daveSetupReceivedPDU(dc, &p1);
      
      if (p1.param[0]==daveFuncRead) {
    LOG1("read Response\n");
     _daveSendMPIAck_IBH(dc);
     dc->resultPointer=p1.data+4;
     dc->_resultPointer=p1.data+4;
     dc->AnswLen=p1.dlen-4;
    return _davePtReadResponse;
      } else if (p1.param[0]==daveFuncWrite) {
     _daveSendMPIAck_IBH(dc);
     LOG1("write Response\n");
     return _davePtWriteResponse;
      } else {
    LOG2("Unsupported PDU function code: %d\n",p1.param[0]);
      }
*/      
  }
  
  if (pm->func==0xb0) {
      if (daveDebug & daveDebugMPI)    
        LOG2("Ackknowledge for packet number: %d\n",pm->packetNumber);
  } else {
      LOG2("Unsupported MPI function code !!: %d\n",pm->func);
      _daveSendMPIAck_IBH(dc);
  }
    }
/*
    Sending IBHNetAck also for packets with sFlags=082 nearly doubles the speed for LINUX and
    speeds up windows version to the level of LINUX.
    Thanks to Axel Kinting for this proposal and for his patience finding it out!
*/    
    if (((p->sFlags==0x82)||(p->sFlags==0x82))&&(p->packetNumber)&&(p->len)) _daveSendIBHNetAck(dc);
    if (haveResp) {
  _daveWriteIBH(dc->iface, resp, resp[2]+8);
  _daveDump("I send response:", resp, resp[2]+8);
    } 
    return 0;
};


int _daveExchangeIBH(daveConnection * dc, PDU * p) {
    _daveSendMessageMPI_IBH(dc, p);
    return _daveGetResponseMPI_IBH(dc);
}

int _daveSendMessageMPI_IBH(daveConnection * dc, PDU * p) {
    int res;
    _davePackPDU(dc, p);
    res=_daveWriteIBH(dc->iface, dc->msgOut, dc->msgOut[2]+8);
    if (daveDebug & daveDebugPDU)    
  _daveDump("I send request: ",dc->msgOut, dc->msgOut[2]+8);
    return res; 
} 

int _daveGetResponseMPI_IBH(daveConnection * dc) {
    int res,count,pt;
    count=0;
    do {
  res=_daveReadIBHPacket(dc->iface, dc->msgIn);
  count++;
  pt=__daveAnalyze(dc);
  if (daveDebug & daveDebugExchange)    
      LOG2("ExchangeIBH packet type:%d\n",pt);
    } while ((pt!=55)&&(count<5));
    return 0;
}


/*
    This performs initialization steps with sampled byte sequences. If chal is <>NULL
    it will send this byte sequence.
    It will then wait for a packet and compare it to the sample.
*/
int _daveInitStepIBH(daveInterface * iface, uc * chal, int cl, us* resp,int rl, uc*b) {
    int res,res2,a=0;
    if (daveDebug & daveDebugConnect) 
  LOG1("_daveInitStepIBH before write.\n");
    if (chal) res=_daveWriteIBH(iface, chal, cl);
    if (daveDebug & daveDebugConnect) 
  LOG2("_daveInitStepIBH write returned %d.\n",res);
    if (res<0) return 100;
    res=_daveReadIBHPacket(iface, b);
/*
    We may get a network layer ackknowledge and an MPI layer ackknowledge, which we discard.
    So, normally at least the 3rd packet should have the desired response. 
    Waiting for more does:
  -discard extra packets resulting from last step.
  -extend effective timeout.
*/    
    while (a<5) {
  if (a) {
      res=_daveReadIBHPacket(iface, b);
//      _daveDump("got:",b,res);
  }
  if (res>0) {
      res2=_daveMemcmp(resp,b,rl/2);
      if (0==res2) {
    if (daveDebug & daveDebugInitAdapter) 
        LOG3("*** Got response %d %d\n",res,rl);
    return a;
      }  else { 
    if (daveDebug & daveDebugInitAdapter)  
        LOG2("wrong! %d\n",res2);
      }
  }
  a++;
    }
    return a;
}

//#if 0
uc chal0[]={
  0x00,0xff,0x03,0x00,0x00,0x00,0x04,0x00, 0x03,0x07,0x02,
};

us resp0[]={
  0xff,0x00,0xfe,0x00, 0x04,0x00,0x00,0x00, 
  0x00,0x07,0x02,0x03,0x1f,
  0x100,    // MPI address of NetLink
  0x02,0x00,
  0x103,    // 187,5, MPI = 3
      // 19,5, PB = 1
  0x00,0x00,0x00,0x102,
  0x00,0x02,0x3e, 
  0x19f,    // 187,5, MPI = 9F
      // 19,5, PB = 64
  0x101,    // 187,5, MPI = 1
      // 19,5, PB = 0
  
  0x101,    // 187,5, MPI = 1
      // 19,5, PB = 0
  0x00,
  0x102,    // 187,5, MPI = 1 
      // 19,5, PB = 2
  0x00,
  0x116,    // 187,5, MPI = 3c  
      // 19,5, PB = 16
  0x00,
  0x13c,    // 187,5, MPI = 90
      // 19,5, PB = 3c
      
  0x101,      
  0x110,
  0x127,0x100,0x100,0x114,0x101, 0x100,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x0e,
  'H', 'i','l','s','c','h','e','r',' ','G','m','b','H',' ',
  
  0x200, /* do not compare the rest */
  };

uc chal1[]={ 
  0x07,0xff,0x08,0x01,0x00,0x00,0x96,0x00, 
      0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x01,
};

us resp1[]={ 
  0xff,0x07,0x87,0x01,0x96,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x7f,0x0a,0x02,
};

uc chal2[]={
  0x00,0x10,0x01,0x00,0x00,0x00,0x01,0x00, 0x0f,
};

us resp2[]={
  0x10,0x00,
      0x20,
      0x00,
      0x01,0x00,0x00,0x00, 
      0x01,0x106,0x120,0x103,0x17,0x00,0x43,0x00,
      0x00,0x00,
      0x122,0x121,
      0x00,0x00,0x00,0x00, 0x49,0x42,0x48,0x53,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
};

/*
    10,00,20,00,01,00,00,00,
    01,12,20,04,17,00,43,
    00,00,00,
    78,16,  // serial number
    00,00,00,00,49,42,48,53,00,00,00,00,00,00,00,00,00,00,00,00,
*/

// 00: FF 00 00 01 97 29 00 00
// 01: 01 ff 06 01 97 04 00 00 14 ff f0 f0 f0 f0
// 07: ff 07 02 01 97 00 00 00 14 00,
uc chal3[]={
  0x07,0xff,0x06,0x01,0x00,0x00,0x97,0x00, 0x15,0xff,0xf0,0xf0,0xf0,0xf0,
};

us resp3[]={
  0xff,0x107,0x02,0x01,0x97,0x00,0x00,0x00, 0x114,0x100,
};

uc chal8[]={
  0x07,0xff,0x11,0x02,0x00,0x00,0x82,0x00, 0x14,0x00,0x02,0x01,0x0c,0xe0,0x04,0x00,
  0x80,0x00,0x02,0x00,0x02,0x01,0x00,0x01, 0x00,
};

us resp7[]={
  0xff,0x07,0x13,0x00,0x00,0x00,0xc2,0x02, 0x115,0x114,0x102,0x100,0x00,0x22,0x0c,0xd0,
  0x04,0x00,0x80,0x00,0x02,0x00,0x02,0x01, 0x00,0x01,0x00,
};

uc chal011[]={
  0x07,0xff,0x07,0x03,0x00,0x00,0x82,0x00, 0x15,0x14,0x02,0x00,0x02,0x05,0x01,
};

us resp09[]={
  0xff,0x07,0x09,0x00,0x00,0x00,0xc2,0x02, 0x115,0x114,0x102,0x100,0x00,0x22,0x02,0x05,
  0x01,
};


/*
    Connect to a PLC via IBH-NetLink. Returns 0 for success and a negative number on errors.
*/
int _daveConnectPLC_IBH(daveConnection*dc) {
    int a, res, retries;
    PDU p1;
    uc b[daveMaxRawLen];
//    LOG1("enter _daveConnectPLC_IBH()\n");
    dc->iface->timeout=500000;
    dc->templ.localMPI=0;
    dc->templ.src_conn=20-1;
    dc->templ.dst_conn=20-1;
    retries=0;
    do {
  LOG1("trying next ID:\n");
  dc->templ.src_conn++;
  chal3[8]=dc->templ.src_conn;
  a=_daveInitStepIBH(dc->iface, chal3,sizeof(chal3),resp3,sizeof(resp3),b);
  retries++;
    } while ((b[9]!=0) && (retries<10));
    if (daveDebug & daveDebugConnect) 
  LOG2("_daveInitStepIBH 4:%d\n",a); if (a>3) /* !!! */ return -4;;
    chal8[10]=dc->MPIAdr; 
//    LOG2("setting MPI %d\n",dc->MPIAdr);
    chal8[8]=dc->templ.src_conn;
    a=_daveInitStepIBH(dc->iface, chal8,sizeof(chal8),resp7,sizeof(resp7),b);
    dc->templ.dst_conn=b[9];
    if (daveDebug & daveDebugConnect) 
  LOG3("_daveInitStepIBH 5:%d connID: %d\n",a, dc->templ.dst_conn); if (a>3) return -5;

    
    chal011[8]=dc->templ.src_conn;
    chal011[9]=dc->templ.dst_conn;
    chal011[10]=dc->MPIAdr; //??????
    a=_daveInitStepIBH(dc->iface, chal011,sizeof(chal011),resp09,sizeof(resp09),b);
    dc->templ.dst_conn=b[9];
    if (daveDebug & daveDebugConnect) 
  LOG3("_daveInitStepIBH 5a:%d connID: %d\n",a, dc->templ.dst_conn); if (a>3) return -5;
    
    dc->packetNumber=4;
    dc->messageNumber=0;
//    dc->templ.packetNumber=1; 
    res= _daveNegPDUlengthRequest(dc, &p1);
    return 0;
}    

uc chal31[]={
  0x07,0xff,0x06,0x08,0x00,0x00,0x82,0x00, 0x14,0x14,0x02,0x00,0x01,0x80,
};

/*
    Disconnect from a PLC via IBH-NetLink. 
    Returns 0 for success and a negative number on errors.
*/
int _daveDisconnectPLC_IBH(daveConnection*dc) {
    uc b[daveMaxRawLen];
    chal31[8]=dc->templ.src_conn;
    chal31[9]=dc->templ.dst_conn;
    _daveWriteIBH(dc->iface, chal31, sizeof(chal31));
    _daveReadIBHPacket(dc->iface, b);
#ifdef BCCWIN    
#else    
    _daveReadIBHPacket(dc->iface, b);
#endif    
    return 0;
}   

void _daveSendMPIAck2(daveConnection *dc) {
    IBHpacket * p;
    uc c;
    uc ack[18];
    memcpy(ack,dc->msgIn,sizeof(ack));
    p= (IBHpacket*) ack;
//    us d=p->sender; p->sender=p->receiver; p->receiver=d;
    p->rFlags|=0x240; //Why that?
    c=p->ch1; p->ch1=p->ch2; p->ch2=c;
    p->len=sizeof(ack)-sizeof(IBHpacket);
    p->packetNumber=0;  // this may mean: no net work level acknowledge
    ack[13]=0x22;
    ack[14]=3;
    ack[15]=176;
    ack[16]=1;
    ack[17]=dc->needAckNumber;
    _daveDump("send MPI-Ack2",ack,sizeof(ack));
    _daveWriteIBH(dc->iface,ack,sizeof(ack));
};

int _davePackPDU_PPI(daveConnection * dc,PDU *p) {
    IBHpacket * ibhp;
    uc IBHPPIHeader[]={0xff,0xff,2,0,8,0x14};
//    MPIheader * hm= (MPIheader*) (dc->msgOut+sizeof(IBHpacket)); // MPI headerPDU begins packet header
//    LOG1("enter packPDU\n");
    memcpy(dc->msgOut+sizeof(IBHpacket), &IBHPPIHeader, 6); // copy MPI header from template
//    LOG1("packPDU 3\n");
//    hm->len=2+p->hlen+p->plen+p->dlen;    // set MPI length
//    hm->func=0xf1;        // set MPI "function code"
//    LOG1("packPDU 1\n");
    
    ibhp = (IBHpacket*) dc->msgOut;
    ibhp->ch1=7;
    ibhp->ch2=0xff;
    ibhp->len=p->hlen+p->plen+p->dlen+5;    // set MPI length
    *(dc->msgOut+sizeof(IBHpacket)+5)=p->hlen+p->plen+p->dlen+0;
//    =hm->len+5;
//    LOG1("packPDU 2\n");
//    if(p->header[4]==0)
//  p->header[4]=1+hm->packetNumber;    // give the PDU a number
//    LOG1("packPDU 3\n");
    ibhp->packetNumber=dc->packetNumber;
//    LOG1("packPDU 4\n");
    dc->packetNumber++;
//    LOG1("packPDU 5\n");
    ibhp->rFlags=0x80;
    ibhp->sFlags=0;
    dc->msgOut[3]=dc->templ.packetNumber;
    dc->templ.packetNumber++; 
    if(dc->templ.packetNumber==0)dc->templ.packetNumber=1;
//    dc->msgOut[0]=0;
    dc->msgOut[6]=0x82;
//    dc->msgOut[6]=0;
//    LOG1("leave packPDU\n");
    return 0;
}    

/*
    send a network level ackknowledge
*/
void _daveSendIBHNetAckPPI(daveConnection * dc) {
    uc ack[]={0,0xff,0,7,0xff,0,0,0,};
    ack[3]=dc->templ.packetNumber;
    dc->templ.packetNumber++; 
    _daveWriteIBH(dc->iface, ack, sizeof(ack));
}

int _daveListReachablePartnersMPI_IBH(daveInterface * di, char * buf) {
    int a, i;
    uc b[2*daveMaxRawLen];
    a=_daveInitStepIBH(di, chal1,sizeof(chal1),resp1,16,b);
    if (daveDebug & daveDebugListReachables) 
  LOG2("_daveListReachablePartnersMPI_IBH:%d\n",a); 
//    if (a>3) return -2;
    for (i=0;i<126;i++) {
  if (b[i+16]==0xFF) buf[i]=0x10; else buf[i]=0x30;
//  LOG3(" %d %d\n",i, b[i+16]); 
    } 
    return 126;
}    


/*
    Changes: 
    09/09/04 applied patch for variable Profibus speed from Andrew Rostovtsew.
    12/09/04 removed debug printf from daveConnectPLC.
    12/09/04 found and fixed a bug in daveFreeResults(): The result set is provided by the 
       application and not necessarily dynamic. So we shall not free() it.
    12/10/04 added single bit read/write functions.
    12/12/04 added Timer/Counter read functions.
    12/13/04 changed dumpPDU to dump multiple results from daveFuncRead
    12/15/04 changed comments to pure C style 
    12/15/04 replaced calls to write() with makro daveWriteFile.
    12/15/04 removed daveSendDialog. Was only used in 1 place.
    12/16/04 removed daveReadCharsPPI. It is replaced by daveReadChars.
    12/30/04 Read Timers and Counters from 200 family. These are different internal types!
    01/02/05 Hopefully fixed local MPI<>0.
    01/10/05 Fixed some debug levels in connectPLCMPI
    01/10/05 Splitted daveExchangeMPI into the send and receive parts. They are separately
       useable when communication is initiated by PLC.
    01/10/05 Code cleanup. Some more things in connectPLC can be done using genaral
       MPI communication subroutines.
    01/10/05 Partially applied changes from Andrew Rostovtsew for multiple MPI connections
       over the same adapter.
    01/11/05 Lasts steps in connect PLC can be done with exchangeMPI.
    01/26/05 replaced _daveConstructReadRequest by the sequence prepareReadRequest, addVarToReadRequest
    01/26/05 added multiple write
    02/02/05 added readIBHpacket
    02/05/05 merged in fixes for (some?) ARM processors.
    02/06/05 Code cleanup.
*/
