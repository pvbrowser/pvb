/*
 Part of Libnodave, a free communication libray for Siemens S7 300/400.
 This program simulates the IBHLink MPI-Ethernet-Adapter from IBH-Softec.
 www.ibh-softec.de
 
 (C) Thomas Hergenhahn (thomas.hergenhahn@web.de) 2002.

 Libnodave is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 Libnodave is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Visual; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#include "log.h"

#define ThisModule "IBHtest : "
#define uc unsigned char

#include "nodave.h"
#include "ibhsamples6.c"

#include <sys/time.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>


#include "accepter.c"
#include <byteswap.h>


#define bSize 1256
#define us unsigned short

#define debug 12

/*
    many bytes. hopefully enough to serve any read request.
*/
uc dummyRes[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,12,13,14,1,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,12,13,14,1,
5,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,5,16,1,2,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,12,13,14,1,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,12,13,14,1,
5,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,5,16,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,};
/*
    a read callback function
*/
uc * dummyRead (int area, int DBnumber, int start, int len, int * res) {
    printf("User callback should deliver pointer to %d bytes from %s %d beginning at %d.\n",
	len, daveAreaName(area),DBnumber,start);
    *res=0;	
    
    return dummyRes;
};

void myWrite (int area, int DBnumber, int start, int len, int * res, uc*bytes) {
    printf("User callback1 should write %d bytes to %s %d beginning at %d.\n",
	len, daveAreaName(area),DBnumber,start);
    printf("User callback 1.\n");	
    *res=0;
    start=start/8;	
    memcpy(dummyRes+start,bytes,len);
    printf("User callback done.\n");
    fflush(stdout);
};

int handleSystemMessage(PDU * p1,PDU * p2) {
    int number=0x100*(p1->data[4])+p1->data[5];
    int count=0x100*(p1->data[6])+p1->data[7];
    printf("number: %d\n",number);
    printf("count: %d\n",count);
    if (number==292) {
	uc pa[]={0x00,0x01,0x12,0x08,0x12,0x84,0x01,0x03, 0x00,0x00,0x00,0x00,}; 
	uc va[]={
	    0x00,0x14,0x00,0x01,0x43,0x02,0xff,0x68, 0xc7,0x00,0x00,0x00,
	    0x08,0x10,0x77,0x10,0x03,0x07,0x10,0x13, 0x22,0x26,0x58,0x85,
	};    
	_daveInitPDUheader(p2,7);
	_daveAddParam(p2, pa, sizeof(pa));
        _daveAddData(p2, p1->data, 8);
	p1->data[3]=4;	// 4 bytes have been copied to result string
	_daveAddValue(p2, va, sizeof(va));    
	_daveDumpPDU(p2);    
	return 1;
    } else if (number==306) {
	uc pa[]={0x00,0x01,0x12,0x08,0x12,0x84,0x01,0x01, 0x00,0x00,0x00,0x00,}; 
	uc va306[]={
	        0,40,0,1,0,4,0, 1, 0,0,0,
		1, 0,2,0,0,0,0,86,86,0,0,
		0, 0,0,0,0,0,0, 0, 0,0,0,
	        0, 0,0,0,0,0,0, 0, 0,0,0,
	};    
	_daveInitPDUheader(p2,7);
	_daveAddParam(p2, pa, sizeof(pa));
        _daveAddData(p2, p1->data, 8);
	p1->data[3]=4;	// 4 bytes have been copied to result string
	_daveAddValue(p2, va306, sizeof(va306));    
	_daveDumpPDU(p2);    
	return 1;
    } else if (number==273) {	/* read order code (MLFB) */
	uc pa[]={0x00,0x01,0x12,0x08,0x12,0x84,0x01,0x03, 0x00,0x00,0x00,0x00,}; 
	uc va273[]={
	    0x00,0x1c,0x00,0x01,0x43,0x02,
	    '6','E','S', 
//	    '7',' ','3','1','5','-','2','A','F','0','3','-','0','A','B','0',
	    '7',' ','L','I','B','N','O','D','A','V','E','-','T','.','H','.',
	    ' ',0x00,0xc0,0x00,0x02,0x00,0x00,
	};    
	_daveInitPDUheader(p2,7);
	_daveAddParam(p2, pa, sizeof(pa));
        _daveAddData(p2, p1->data, 8);
	p1->data[3]=4;	// 4 bytes have already been copied to result string
	_daveAddValue(p2, va273, sizeof(va273));    
	_daveDumpPDU(p2);    
	return 1;	
    } else if (number==305) {
	uc pa[]={0x00,0x01,0x12,0x08,0x12,0x84,0x01,0x01, 0x00,0x00,0x00,0x00,}; 
	uc va306[]={
//	        0,40,0,1,0,4,0, 1, 0,0,0,
//		1, 0,2,0,0,0,0,86,86,0,0,
//		0, 0,0,0,0,0,0, 0, 0,0,0,
//	        0, 0,0,0,0,0,0, 0, 0,0,0,
		0, 40,  0,  1,  0,  2,190,253, 15,  0,
		0,  0,  0,  0,  0,  0,  0,  0, 60,  1,
		0,  0,	0,  0,125,  0,  0,  5,  3,  4, 
		0,  0,  0,  0,  0, 12,  0, 10,  0,  0,
		0,  9,  0,  0,
	};    
	_daveInitPDUheader(p2,7);
	_daveAddParam(p2, pa, sizeof(pa));
        _daveAddData(p2, p1->data, 8);
	p1->data[3]=4;	// 4 bytes have been copied to result string
	_daveAddValue(p2, va306, sizeof(va306));    
	_daveDumpPDU(p2);    
	return 1;	
    } else if (number==0) {
	uc pa[]={0x00,0x01,0x12,0x08,0x12,0x84,0x01,0x01, 0x00,0x00,0x00,0x00,}; 
	uc va[]={
//	255,9,0,114,0,0,0,0,
	0,2,0,53,0,0,15,0,0,17,1,17,15,17,0,18,
	1,18,15,18,0,19,0,20,0,21,1,21,0,23,1,23,15,
23,0,24,1,24,15,24,0,25,15,25,15,26,15,27,0,26,0,
27,0,33,10,33,15,33,2,34,0,35,15,35,0,36,1,36,
4,36,5,36,1,49,1,50,2,50,0,116,1,116,15,116,12,
145,13,145,10,145,0,146,2,146,6,146,15,146,0,177,0,178,0,
179,0,180,0,160,1,160,12,139,126,
	};    
	_daveInitPDUheader(p2,7);
	_daveAddParam(p2, pa, sizeof(pa));
        _daveAddData(p2, p1->data, 8);
	p1->data[3]=4;	// 4 bytes have been copied to result string
	_daveAddValue(p2, va, sizeof(va));    
	_daveDumpPDU(p2);    
	return 1;	
    } else if (number==1060) {
	uc pa[]={0x00,0x01,0x12,0x08,0x12,0x84,0x01,0x01, 0x00,0x00,0x00,0x00,}; 
	uc va[]={
//	255,9,0,114,0,0,0,0,
	0,20,0,1,81,68,255,8,0,0,0,0,0,0,0,0,148,1,2,22,83,88,104,17,
	};    
	_daveInitPDUheader(p2,7);
	_daveAddParam(p2, pa, sizeof(pa));
        _daveAddData(p2, p1->data, 8);
	p1->data[3]=4;	// 4 bytes have been copied to result string
	_daveAddValue(p2, va, sizeof(va));    
	_daveDumpPDU(p2);    
	return 1;		
    } else 
    return 0;
};

int handleNegociate(PDU * p1,PDU * p2) {
    uc pa[]={0xF0,0x0,0x0,0x1, 0x00,0x01,0x00,0xF0}; 
    _daveInitPDUheader(p2,3);
    _daveAddParam(p2, pa, sizeof(pa));
    return 1;		
};

int gpacketNumber=0;

uc r5[]={ 
		0xff,0x07,0x13,0x00,0x00,0x00,0xc2,0x02, 0x14,0x14,0x03,0x00,0x00,0x22,0x0c,0xd0,
		0x04,0x00,0x80,0x00,0x02,0x00,0x02,0x01,0x00,0x01,0x00,
	    };

void analyze(daveConnection * dc) {
    IBHpacket * p2;
    MPIheader2 * m2;
    uc resp[2000];
    int haveResp=0;
    PDU p1,pr;
    IBHpacket * p= (IBHpacket*) dc->msgIn;
    dc->needAckNumber=-1;		// Assume no ack
/*    
    printf("Channel: %d\n",p->ch1);
    printf("Channel: %d\n",p->ch2);
    printf("Length:  %d\n",p->len);
    printf("Number:  %d\n",p->packetNumber);
    printf("sFlags:  %04x rFlags:%04x\n",p->sFlags,p->rFlags);
*/    
    if (p->rFlags==0x82) {
	MPIheader * pm= (MPIheader*) (dc->msgIn+sizeof(IBHpacket));
	printf("srcconn: %d\n",pm->src_conn);
	printf("dstconn: %d\n",pm->dst_conn);
	printf("MPI:     %d\n",pm->MPI);
	printf("MPI len: %d\n",pm->len);
	printf("MPI func:%d\n",pm->func);
	if (pm->func==0xf1) {
	    printf("0xf1:PDU transport, MPI packet number: %d\n",pm->packetNumber);
	    dc->needAckNumber=pm->packetNumber;
	    dc->PDUstartI=sizeof(IBHpacket)+sizeof(MPIheader);
//	    p1.header=((uc*)pm)+sizeof(MPIheader);
	    _daveSetupReceivedPDU(dc, &p1);
	    _daveDumpPDU(&p1);
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
	    m2->packetNumber=gpacketNumber;
	    printf("before _daveHandleWrite() %p\n",m2);
	    gpacketNumber++;
	    if (p1.param[0]==daveFuncRead) {
		_daveHandleRead(&p1,&pr);
		haveResp=1;
		m2->func=0xf1; //!! guessed
		m2->len=pr.hlen+pr.plen+pr.dlen+2;
		p2->len=m2->len+7;
	    } else if (p1.param[0]==daveFuncWrite) {
		printf("before _daveHandleWrite() %p\n",m2);
		_daveHandleWrite(&p1,&pr);
		printf("after _daveHandleWrite() %p\n",m2);
		fflush(stdout);
		haveResp=1;
//		m2->func=0xf1; //!! guessed
		printf("after _daveHandleWrite()\n");
		fflush(stdout);
		m2->len=pr.hlen+pr.plen+pr.dlen+2;
		printf("after _daveHandleWrite()\n");
		fflush(stdout);
		p2->len=m2->len+7;
		printf("after _daveHandleWrite()\n");
		fflush(stdout);
	    } else if (p1.param[0]==240) {
		printf("PDU function code: %d, negociate PDU len\n",p1.param[0]);
		_daveDump("packet:",dc->msgIn,dc->msgIn[2]+8);
		handleNegociate(&p1,&pr);
		haveResp=1;
		m2->func=0xf1; //!! guessed
		m2->len=pr.hlen+pr.plen+pr.dlen+2;
		p2->len=m2->len+7;	
	    } else if (p1.param[0]==0) {
		printf("PDU function code: %d, system Messaga ?\n",p1.param[0]);
		_daveSendMPIAck2(dc);
//		_daveDump("packet:",b,b[2]+8);
		haveResp=handleSystemMessage(&p1,&pr);
		pr.header[4]=p1.header[4];		// give the PDU a number
		m2->func=0xf1; //!! guessed
		m2->len=pr.hlen+pr.plen+pr.dlen+2;
		p2->len=m2->len+7;	
	    } else {
		printf("Unsupported PDU function code: %d\n",p1.param[0]);
		
	    }
	    
	}
	if (pm->func==0xb0) {
    	    printf("Ackknowledge for packet number: %d\n",*(dc->msgIn+15));
	}
	if (pm->func==0xe0) {
	    printf("Connect to MPI: %d\n",pm->MPI);
	    
	    memcpy(resp, r5, sizeof(r5));
	    resp[8]=pm->src_conn;
	    resp[9]=pm->src_conn;
	    resp[10]=pm->MPI;
	    haveResp=1;
	}    
    }	
    if ((p->rFlags==0x82)&&(p->packetNumber)&&(p->len)) {
	printf("before _daveSendIBHNetAck()\n");
	fflush(stdout);
	_daveSendIBHNetAck(dc);
    }	
    if (haveResp) {
	printf("have response\n");
	fflush(stdout);
	_daveDump("I sent:",resp,resp[2]+8);    
	fflush(stdout);
	write(dc->iface->fd.rfd,resp,resp[2]+8);
	_daveDump("I sent:",resp,resp[2]+8);    
	fflush(stdout);
    }	
};

typedef struct _portInfo {
    int fd;
}portInfo;

#define mymemcmp _daveMemcmp
void *portServer(void *arg)
{
    int waitCount, res, pcount, r2;
    _daveOSserialType s;
    daveInterface * di;
    daveConnection * dc;
    
    portInfo * pi=(portInfo *) arg;
    LOG2(ThisModule "portMy fd is:%d\n", 
	pi->fd);
    FLUSH;
    waitCount= 0;
    daveDebug=daveDebugAll;
    pcount=0;
    
    s.rfd=pi->fd;
    s.wfd=pi->fd;
    di=daveNewInterface(s,"IF",0,daveProtoMPI_IBH,daveSpeed187k);
    di->timeout=900000;
    dc=daveNewConnection(di,0,0,0);
    while (waitCount < 1000) {
	dc->AnswLen=_daveReadIBHPacket(dc->iface, dc->msgIn);
	if (dc->AnswLen>0) {
	    res=dc->AnswLen;
	    LOG2(ThisModule "%d ", pcount);		
	    _daveDump("packet", dc->msgIn, dc->AnswLen);
	    waitCount = 0;
	    analyze(dc);
		    
	    r2=2*res;

	    if(r2==sizeof(cha0)) {
	        if (0==mymemcmp(cha0, dc->msgIn, res)) {
			LOG1(ThisModule "found challenge 0, write response 0\n");
			write(pi->fd, res0, sizeof(res0));
		    }
	    }
	    if(r2==sizeof(cha1)) {
	        if (0==mymemcmp(cha1, dc->msgIn, res)) {
		    LOG1(ThisModule "found challenge 1, write response 1\n");
		    write(pi->fd, res1, sizeof(res1));
		}
	    }
	    if(r2==sizeof(cha2)) {
	        if (0==mymemcmp(cha2, dc->msgIn, res)) {
		    LOG1(ThisModule "found challenge 2, write response 2\n");
		    write(pi->fd, res2, sizeof(res2));
		}
	    }
	    if(r2==sizeof(cha3)) {
	        if (0==mymemcmp(cha3, dc->msgIn, res)) {
		    LOG1(ThisModule "found challenge 3, write response 3\n");
		    res3[8]=dc->msgIn[8];    
		    write(pi->fd, res3, sizeof(res3));
		}
	    }

	    if(r2==sizeof(cha8)) {
	        if (0==mymemcmp(cha8, dc->msgIn, res)) {
		    LOG1(ThisModule "found challenge 8, write response 7\n");
		    res7[8]=dc->msgIn[8];    
		    res7[9]=dc->msgIn[9];    
		    res7[10]=dc->msgIn[10];    
		    write(pi->fd, res7, sizeof(res7));
		}
	    }

	    if(r2==sizeof(cha11)) {
	        if (0==mymemcmp(cha11, dc->msgIn, res)) {
		    LOG1(ThisModule "found challenge 11, write response 10\n");
		    res10[8]=dc->msgIn[8];    
		    res10[9]=dc->msgIn[9];    
//			res10[10]=dc->msgIn[10];    
//			res10[32]=dc->msgIn[28];    
//			res10[34]=dc->msgIn[30];    
//			sendMPIAck2(dc->msgIn, pi->fd, ackPacketNumber);
		    dc->needAckNumber=0;
		    _daveSendMPIAck2(dc);
		    write(pi->fd, res10, sizeof(res10));
		}
	    }
	    pcount++;
	} else {
	    waitCount++;
	}    
    }
    LOG1(ThisModule "portserver: I closed my fd.\n");
    FLUSH;
    return NULL;
}


/*
    This waits in select for a file descriptor from accepter and starts a new child server
    with this file descriptor.
*/
int PID;
int main(int argc, char **argv)
{
    portInfo pi;
    fd_set FDS;
    int filedes[2], res, newfd;
    pthread_attr_t attr;
    pthread_t ac, ps;
    accepter_info ai;
    PID=getpid();
    if (argc<2) {
	printf("Usage: ibhtest port\n");
	printf("Example: ibhtest 1099 (used by IBHNetLink)\n");
	return -1;
    }
    readCallBack=dummyRead;
    writeCallBack=myWrite;
    
    pipe(filedes);
    ai.port = atol(argv[1]);
    LOG2(ThisModule "Main serv: %d\n", ai.port);
    LOG2(ThisModule "Main serv: Accepter pipe fd: %d\n", ai.fd);
    ai.fd = filedes[1];
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    res=pthread_create(&ac, &attr, accepter, &ai /*&filedes[1] */ );
    do {
	FD_ZERO(&FDS);
	FD_SET(filedes[0], &FDS);

	LOG2(ThisModule "Main serv: about to select on %d\n",
	       filedes[0]);
	FLUSH;
	if (select(filedes[0] + 1, &FDS, NULL, &FDS, NULL) > 0) {
	    LOG1(ThisModule "Main serv: about to read\n");
	    res = read(filedes[0], &pi.fd, sizeof(pi.fd));
	    ps=0;		   
	    pthread_attr_init(&attr);
	    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	    res=pthread_create(&ps, &attr, portServer, &pi);
	    if(res) {
		LOG2(ThisModule
		   "Main serv: create error:%s\n", strerror(res));		   
		close(newfd);
		usleep(100000);
	    }	   
	}
    }
    while (1);
    return 0;
}


/*
    Changes:
    
    14/07/2003 give a hint about usage
    02/01/2005 fixed argc, it's 2 if there is 1 argument.
*/
