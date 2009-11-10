/*
 Test and demo program for Libnodave, a free communication libray for Siemens S7.
 This is a test for multiple connections to the same CPU.
 
 **********************************************************************
 * WARNING: This and other test programs overwrite data in your PLC.  *
 * DO NOT use it on PLC's when anything is connected to their outputs.*
 * This is alpha software. Use entirely on your own risk.             * 
 **********************************************************************
 
 (C) Thomas Hergenhahn (thomas.hergenhahn@web.de) 2002, 2003.

 This is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 This is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Libnodave; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nodave.h"
#include "setport.h"

#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#define UNIX_STYLE
#endif

#ifdef CYGWIN
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#define UNIX_STYLE
#endif

#ifdef BCCWIN
#include <time.h>
    void usage();
    void wait();
#define WIN_STYLE    
#endif

void usage()
{
    printf("Usage: testMPI2 [-d] [-w] serial port.\n");
    printf("-w will try to write to Flag words. It will overwrite FB0 to FB15 (MB0 to MB15) !\n");
    printf("-d will produce a lot of debug messages.\n");
    printf("-b will run benchmarks. Specify -b and -w to run write benchmarks.\n");
    printf("-z will read some SZL list items (diagnostic information).\n");
    printf("-l will run a test to determine maximum length of a block in read.\n");
    printf("-2 uses a slightly different version of the MPI protocol. Try it, if your Adapter doesn't work.\n");
    printf("-m will run a test for multiple variable reads.\n");
    printf("-c will write 0 to the PLC memory used in write tests.\n");
    printf("-n will test newly added functions.\n");
    printf("-<number> will set the speed of MPI/PROFIBUS network to this value (in kBaud).\n  Default is 187.\n  Supported values are 9, 19, 45, 93, 187, 500 and 1500.\n");
    printf("--mpi=<number> will use number as the MPI adddres of the PLC. Default is 2.\n");
    printf("--local=<number> will set the local MPI adddres to number. Default is 0.\n");
#ifdef UNIX_STYLE
    printf("Example: testMPI2 -w /dev/ttyS0\n");
#endif    
#ifdef WIN_STYLE    
    printf("Example: testMPI2 -w COM1\n");
#endif    
}

void wait() {
    uc c;
    printf("Press return to continue.\n");
#ifdef UNIX_STYLE
    read(0,&c,1);
#endif    
}    

void readSZL(daveConnection *dc,int id, int index) {
    int res;
    printf("Trying to read SZL-ID %04X index %02X.\n",id,index);
    res=daveReadSZL(dc,id,index,NULL);
    printf("Function result: %d %s\n",res,daveStrerror(res));
    _daveDump("Data",dc->resultPointer,dc->AnswLen);
}    

void readAll(daveConnection *dc,daveConnection* dc2,daveConnection *dc3) {
int a,b,c;
float d;
	printf("Trying to read 16 bytes from FW0 using connection 1.\n");
	daveReadBytes(dc, daveFlags, 0, 0, 16,NULL);
        a=daveGetS32(dc);	
        b=daveGetS32(dc);
        c=daveGetS32(dc);
        d=daveGetFloat(dc);
	printf("FD0: %d\n",a);
	printf("FD4: %d\n",b);
	printf("FD8: %d\n",c);
	printf("FD12: %f\n",d);

	printf("Trying to read 16 bytes from FW0 using connection 2.\n");
	wait();
	daveReadBytes(dc2, daveFlags, 0, 0, 16,NULL);
        a=daveGetS32(dc2);	
        b=daveGetS32(dc2);
        c=daveGetS32(dc2);
        d=daveGetFloat(dc2);
	printf("FD0: %d\n",a);
	printf("FD4: %d\n",b);
	printf("FD8: %d\n",c);
	printf("FD12: %f\n",d);

	printf("Trying to read 16 bytes from FW0 using connection 3.\n");
	wait();
	daveReadBytes(dc3, daveFlags, 0, 0, 16,NULL);
        a=daveGetS32(dc3);	
        b=daveGetS32(dc3);
        c=daveGetS32(dc3);
        d=daveGetFloat(dc3);
	printf("FD0: %d\n",a);
	printf("FD4: %d\n",b);
	printf("FD8: %d\n",c);
	printf("FD12: %f\n",d);
}	

void readDc(daveConnection *dc) {
int a,b,c;
float d;
	printf("Trying to read 16 bytes from FW0.\n");
	daveReadBytes(dc, daveFlags, 0, 0, 16,NULL);
        a=daveGetS32(dc);	
        b=daveGetS32(dc);
        c=daveGetS32(dc);
        d=daveGetFloat(dc);
	printf("FD0: %d\n",a);
	printf("FD4: %d\n",b);
	printf("FD8: %d\n",c);
	printf("FD12: %f\n",d);
}	

#include "benchmark.c"

int main(int argc, char **argv) {
    int i,j, a,b,c, adrPos, doWrite, doBenchmark, 
	doSZLread, doMultiple, doClear, doNewfunctions,
	initSuccess,
	saveDebug,
	res, useProto, speed, localMPI, plcMPI;
    PDU p;	
    float d;
    char buf1 [davePartnerListSize];
    daveInterface * di;
    daveConnection * dc, * dc2, * dc3, * dc4;
    _daveOSserialType fds;
    daveResultSet rs;
    
    adrPos=1;
    doWrite=0;
    doBenchmark=0;
    doSZLread=0;
    doMultiple=0;
    doClear=0;
    doNewfunctions=0;
    
    useProto=daveProtoMPI;
    speed=daveSpeed187k;
    localMPI=0;
    plcMPI=2;
    
    if (argc<2) {
	usage();
	exit(-1);
    }    

    while (argv[adrPos][0]=='-') {
//	printf("arg %s\n",argv[adrPos]);
	if (strcmp(argv[adrPos],"-d")==0) {
	    daveDebug=daveDebugAll;
	} else
	if (strcmp(argv[adrPos],"-w")==0) {
	    doWrite=1;
	} else
	if (strcmp(argv[adrPos],"-b")==0) {
	    doBenchmark=1;
	} else
	if (strncmp(argv[adrPos],"--local=",8)==0) {
	    localMPI=atol(argv[adrPos]+8);
	    printf("setting local MPI address to:%d\n",localMPI);
	} else
	if (strncmp(argv[adrPos],"--mpi=",6)==0) {
	    plcMPI=atol(argv[adrPos]+6);
	    printf("setting MPI address of PLC to:%d\n",plcMPI);
	} else
	if (strcmp(argv[adrPos],"-z")==0) {
	    doSZLread=1;
	}
	else if (strcmp(argv[adrPos],"-m")==0) {
	    doMultiple=1;
	}
	else if (strcmp(argv[adrPos],"-c")==0) {
	    doClear=1;
	}
	else if (strcmp(argv[adrPos],"-n")==0) {
	    doNewfunctions=1;
	}
	else if (strcmp(argv[adrPos],"-2")==0) {
	    useProto=daveProtoMPI2;
	}
	else if (strcmp(argv[adrPos],"-3")==0) {
	    useProto=daveProtoMPI3;
	} 
 	else if (strcmp(argv[adrPos],"-9")==0) {
 	    speed=daveSpeed9k;
 	} 
 	else if (strcmp(argv[adrPos],"-19")==0) {
 	    speed=daveSpeed19k;
 	} 
 	else if (strcmp(argv[adrPos],"-45")==0) {
 	    speed=daveSpeed45k;
 	} 
 	else if (strcmp(argv[adrPos],"-93")==0) {
 	    speed=daveSpeed93k;
 	} 
 	else if (strcmp(argv[adrPos],"-500")==0) {
 	    speed=daveSpeed500k;
 	} 
 	else if (strcmp(argv[adrPos],"-1500")==0) {
 	    speed=daveSpeed1500k;
 	} 
 	
	adrPos++;
	if (argc<=adrPos) {
	    usage();
	    exit(-1);
	}	
    }    
    
    fds.rfd=setPort(argv[adrPos],"38400",'O');
    fds.wfd=fds.rfd;
    initSuccess=0;	
    if (fds.rfd>0) { 
	di =daveNewInterface(fds, "IF1", localMPI, useProto, speed);
	for (i=0; i<3; i++) {
	    if (0==daveInitAdapter(di)) {
		initSuccess=1;	
		a= daveListReachablePartners(di,buf1);
		printf("daveListReachablePartners List length: %d\n",a);
		if (a>0) {
		    for (j=0;j<a;j++) {
			if (buf1[j]==daveMPIReachable) printf("Device at address:%d\n",j);
		    }	
		}
		break;	
	    } else daveDisconnectAdapter(di);
	}
	if (!initSuccess) {
	    printf("Couldn't connect to Adapter!.\n Please try again. You may also try the option -2 for some adapters.\n");	
	    return -3;
	}
	dc =daveNewConnection(di,plcMPI,0,0);  // insert your MPI address here
	dc2 =daveNewConnection(di,plcMPI,0,0);  // insert your MPI address here
	dc3 =daveNewConnection(di,plcMPI,0,0);  // insert your MPI address here
	dc4 =daveNewConnection(di,plcMPI,0,0);  // insert your MPI address here
	printf("ConnectPLC\n");
	if (0==daveConnectPLC(dc)) {;
	    if (0==daveConnectPLC(dc2)) {
		printf("2nd Connection established.\n");
	    }
	    if (0==daveConnectPLC(dc3)) {
		printf("3rd Connection established.\n");
	    }
//	    if (0==daveConnectPLC(dc4)) {
//		printf("4th Connection established.\n");
//	    }
	    readAll(dc,dc2,dc3);

	if(doNewfunctions) {
	    saveDebug=daveDebug;
//	    daveDebug=daveDebugPDU;
	    printf("Trying to read a single bit from DB11.DBX0.1\n");
	    res=daveReadBits(dc, daveDB, 11, 1, 1,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    
	    printf("Trying to read two consecutive bits from DB11.DBX0.1\n");;
	    res=daveReadBits(dc, daveDB, 11, 1, 2,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    
	    printf("Trying to read no bit (length 0) from DB11.DBX0.1\n");
	    res=daveReadBits(dc, daveDB, 11, 1, 0,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));

	    daveDebug=daveDebugPDU;	    
	    printf("Trying to read a single bit from DB11.DBX0.3\n");
	    res=daveReadBits(dc, daveDB, 11, 3, 1,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	
	    printf("Trying to read a single bit from E0.2\n");
	    res=daveReadBits(dc, daveInputs, 0, 2, 1,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
//	    daveDebug=0;	
	    
	    a=0;
	    printf("Writing 0 to EB0\n");
	    res=daveWriteBytes(dc, daveOutputs, 0, 0, 1, &a);
//	    daveDebug=daveDebugAll;
	    a=1;
	    printf("Trying to set single bit E0.5\n");
	    res=daveWriteBits(dc, daveOutputs, 0, 5, 1, &a);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	
	    printf("Trying to read 1 byte from AAW0\n");
	    res=daveReadBytes(dc, daveAnaIn, 0, 0, 2,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    
	    a=2341;
	    printf("Trying to write 1 word (2 bytes) to AAW0\n");
	    res=daveWriteBytes(dc, daveAnaOut, 0, 0, 2,&a);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	
	    printf("Trying to read 4 items from Timers\n");
	    res=daveReadBytes(dc, daveTimer, 0, 0, 4,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    d=daveGetSeconds(dc);
	    printf("Time: %0.3f, ",d);
	    d=daveGetSeconds(dc);
	    printf("%0.3f, ",d);
	    d=daveGetSeconds(dc);
	    printf("%0.3f, ",d);
	    d=daveGetSeconds(dc);
	    printf(" %0.3f\n",d);
	    
	    d=daveGetSecondsAt(dc,0);
	    printf("Time: %0.3f, ",d);
	    d=daveGetSecondsAt(dc,2);
	    printf("%0.3f, ",d);
	    d=daveGetSecondsAt(dc,4);
	    printf("%0.3f, ",d);
	    d=daveGetSecondsAt(dc,6);
	    printf(" %0.3f\n",d);
	    
	    printf("Trying to read 4 items from Counters\n");
	    res=daveReadBytes(dc, daveCounter, 0, 0, 4,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    c=daveGetCounterValue(dc);
	    printf("Count: %d, ",c);
	    c=daveGetCounterValue(dc);
	    printf("%d, ",c);
	    c=daveGetCounterValue(dc);
	    printf("%d, ",c);
	    c=daveGetCounterValue(dc);
	    printf(" %d\n",c);
	    
	    c=daveGetCounterValueAt(dc,0);
	    printf("Count: %d, ",c);
	    c=daveGetCounterValueAt(dc,2);
	    printf("%d, ",c);
	    c=daveGetCounterValueAt(dc,4);
	    printf("%d, ",c);
	    c=daveGetCounterValueAt(dc,6);
	    printf(" %d\n",c);
	    
	    davePrepareReadRequest(dc, &p);
	    daveAddVarToReadRequest(&p,daveInputs,0,0,1);
	    daveAddVarToReadRequest(&p,daveFlags,0,0,4);
	    daveAddVarToReadRequest(&p,daveDB,6,20,2);
	    daveAddVarToReadRequest(&p,daveTimer,0,0,4);
	    daveAddVarToReadRequest(&p,daveTimer,0,1,4);
	    daveAddVarToReadRequest(&p,daveTimer,0,2,4);
	    daveAddVarToReadRequest(&p,daveCounter,0,0,4);
	    daveAddVarToReadRequest(&p,daveCounter,0,1,4);
	    daveAddVarToReadRequest(&p,daveCounter,0,2,4);
	    
	    res=daveExecReadRequest(dc, &p, &rs);
	    if (doWrite) {
		
	    }
	    
	    daveDebug=saveDebug;
	}

//	daveDebug=daveDebugAll;
	
	if(doSZLread) {
	    readSZL(dc,0x92,0x0);
	    readSZL(dc,0xB4,0x1024);
	    readSZL(dc,0x111,0x1);
	    readSZL(dc,0xD91,0x0);
	    readSZL(dc,0x232,0x4);
	    readSZL(dc,0x1A0,0x0);
	}    
	if(doMultiple) {
    	    printf("Now testing read multiple variables.\n"
		"This will read 1 Byte from inputs,\n"
		"4 bytes from flags, 2 bytes from DB6\n"
		" and other 2 bytes from flags\n");
    	    wait();
	    davePrepareReadRequest(dc, &p);
	    daveAddVarToReadRequest(&p,daveInputs,0,0,1);
	    daveAddVarToReadRequest(&p,daveFlags,0,0,4);
	    daveAddVarToReadRequest(&p,daveDB,6,20,2);
	    daveAddVarToReadRequest(&p,daveFlags,0,12,2);
	    res=daveExecReadRequest(dc, &p, &rs);
	    
	    printf("Input Byte 0 ");
	    res=daveUseResult(dc, rs, 0); // first result
	    if (res==0) {
		a=daveGetByte(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));
		
	    printf("Flag DWord 0 ");	
	    res=daveUseResult(dc, rs, 1); // 2nd result
	    if (res==0) {
		a=daveGetInteger(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));
		
	    printf("DB 6 Word 20: ");	
	    res=daveUseResult(dc, rs, 2); // 3rd result
	    if (res==0) {
		a=daveGetInteger(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));
		
	    printf("Flag Word 12: ");		
	    res=daveUseResult(dc, rs, 3); // 4th result
	    if (res==0) {
		a=daveGetWORD(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));	
		
	    printf("non existing result (we read 4 items, but try to use a 5th one): ");		
	    res=daveUseResult(dc, rs, 4); // 5th result
	    if (res==0) {
		a=daveGetWORD(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));		
	    
	    daveFreeResults(&rs);  
	}	    

	if(doWrite) {
    	    printf("Now we write back these data after incrementing the integers by 1,2 and 3 and the float by 1.1.\n");
    	    wait();
/*
    Attention! you need to bswap little endian variables before using them as a buffer for
    daveWriteBytes() or before copying them into a buffer for daveWriteBytes()!
*/	    
    	    a=bswap_32(a-1);
    	    daveWriteBytes(dc,daveFlags,0,0,4,&a);
    	    b=bswap_32(b-2);
    	    daveWriteBytes(dc,daveFlags,0,4,4,&b);
    	    c=bswap_32(c-3);
	    daveWriteBytes(dc,daveFlags,0,8,4,&c);
    	    d=toPLCfloat(d-1.1);
    	    daveWriteBytes(dc,daveFlags,0,12,4,&d);
/*
 *   Read back and show the new values, so users may notice the difference:
 */	    
    	    daveReadBytes(dc,daveFlags,0,0,16,NULL);
    	    a=daveGetDWORD(dc);
    	    b=daveGetDWORD(dc);
    	    c=daveGetDWORD(dc);
    	    d=daveGetFloat(dc);
	    printf("FD0: %d\n",a);
	    printf("FD4: %d\n",b);
	    printf("FD8: %d\n",c);
	    printf("FD12: %f\n",d);
	} // doWrite
	if(doClear) {
    	    printf("Now writing 0 to the bytes FB0...FB15.\n");
    	    wait();
	    a=0;
    	    daveWriteBytes(dc,daveFlags,0,0,4,&a);
    	    daveWriteBytes(dc,daveFlags,0,4,4,&a);
	    daveWriteBytes(dc,daveFlags,0,8,4,&a);
    	    daveWriteBytes(dc,daveFlags,0,12,4,&a);
	    daveReadBytes(dc,daveFlags,0,0,16,NULL);
    	    a=daveGetDWORD(dc);
    	    b=daveGetDWORD(dc);
    	    c=daveGetDWORD(dc);
    	    d=daveGetFloat(dc);
	    printf("FD0: %d\n",a);
	    printf("FD4: %d\n",b);
	    printf("FD8: %d\n",c);
	    printf("FD12: %f\n",d);
	} // doClear
	if(doBenchmark) {
	    rBenchmark(dc);
	    if(doWrite) {
		wBenchmark(dc);
	    } // doWrite
	} // doBenchmark

	printf("Now disconnecting 1st\n");
	daveDisconnectPLC(dc);
	wait();
//	readAll(dc,dc2,dc3);
	printf("Now disconnecting 2nd\n");
	daveDisconnectPLC(dc2);
	wait();
//	readAll(dc,dc2,dc3);
	printf("Now disconnecting 3rd\n");
	daveDisconnectPLC(dc3);
//	readAll(dc,dc2,dc3);
	wait();
	
	printf("Now reconnecting 3rd\n");
	wait();
	daveDebug=0xffff;
	daveConnectPLC(dc4);
	readDc(dc4);
//	printf("Now disconnecting 4th\n");
//	daveDisconnectPLC(dc3);
//	wait();
	daveDisconnectAdapter(di);
	return 0;
	} else {
	    printf("Couldn't connect to PLC.\n Please try again. You may also try the option -2 for some adapters.\n");	
	    daveDisconnectAdapter(di);
	    daveDisconnectAdapter(di);
	    daveDisconnectAdapter(di);
	    return -2;
	}
    } else {
	printf("Couldn't open serial port %s\n",argv[adrPos]);	
	return -1;
    }	
}

/*
    Changes: 
    07/19/04 removed unused vars.
    09/09/04 applied patch for variable Profibus speed from Andrew Rostovtsew.
    09/09/04 removed unused include byteswap.h
    09/11/04 added multiple variable read example code.
    12/09/04 removed debug printf from arg processing.
    12/12/04 added Timer/counter read functions.
*/
