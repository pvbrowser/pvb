/*
 Test and demo program for Libnodave, a free communication libray for Siemens S7.
 
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
    void usage(void);
    void wait(void);
#define WIN_STYLE    
#endif


void usage()
{
    printf("Usage: testPPI [-d] [-w] serial port.\n");
    printf("-w will try to write to Flag words. It will overwrite FB0 to FB15 (MB0 to MB15) !\n");
    printf("-d will produce a lot of debug messages.\n");
    printf("-b will run benchmarks. Specify -b and -w to run write benchmarks.\n");
    printf("-l will run a test to determine maximum length of a block in read.\n");
    printf("-m will run a test for multiple variable reads.\n");
    printf("-c will write 0 to the PLC memory used in write tests.\n");
    printf("-s stops the PLC.\n");
    printf("-r tries to put the PLC in run mode.\n");
    printf("--readout read program and data blocks from PLC.\n");
#ifdef UNIX_STYLE    
    printf("Example: testPPI -w /dev/ttyS0\n");
#endif    
#ifdef WIN_STYLE    
    printf("Example: testPPI -w COM1\n");
#endif    
}

void wait() {
    uc c;
    printf("Press return to continue.\n");
#ifdef UNIX_STYLE        
    read(0,&c,1);
#endif
}    

void loadBlocksOfType(daveConnection * dc, int blockType) {
    int i,j,uploadID, len, more;
#ifdef UNIX_STYLE
    int fd;
#endif	        
#ifdef WIN_STYLE	    
    HANDLE fd;
    unsigned long res;
#endif	        
    char blockName [20];
    uc blockBuffer[20000],*bb;
    daveBlockEntry dbe[256];   
    j=daveListBlocksOfType(dc, blockType, dbe);
    printf("%d blocks of type %s\n",j,daveBlockName(blockType));
    for (i=0; i<j; i++) {
	printf("%s%d  %d %d\n",
	    daveBlockName(blockType),
	    dbe[i].number, dbe[i].type[0],dbe[i].type[1]);	
	bb=blockBuffer;
	len=0;
	if (0==initUpload(dc, blockType, dbe[i].number, &uploadID)) {
    	    do {
		doUpload(dc,&more,&bb,&len,uploadID);
	    } while (more);
	    sprintf(blockName,"%s%d.mc7",daveBlockName(blockType), dbe[i].number);	
#ifdef UNIX_STYLE	    
    	    fd=open(blockName,O_RDWR|O_CREAT|O_TRUNC,0644);
    	    write(fd, blockBuffer, len);
    	    close(fd);
#endif	    
#ifdef WIN_STYLE
	    fd = CreateFile(blockName,
    	      GENERIC_WRITE, 0, 0, 2,
    		FILE_FLAG_WRITE_THROUGH, 0);
    	    WriteFile(fd, blockBuffer, len, &res, NULL);
    	    CloseHandle(fd);
#endif	    
    	    endUpload(dc,uploadID);
	}    
    }
}


#include "benchmark.c"
//extern 
int seconds,thirds;

int main(int argc, char **argv) {
    int i, a,b,c,adrPos, doWrite, doBenchmark, doMultiple, doClear, doNewfunctions,
	doExperimental, doRun, doStop, doReadout,
	res;
    float d;
    daveInterface * di;
    daveConnection * dc;
    daveResultSet rs;
    _daveOSserialType fds;
    PDU p;
    
    adrPos=1;
    doWrite=0;
    doBenchmark=0;
    doMultiple=0;
    doClear=0;
    doNewfunctions=0;
    doExperimental=0;
    doRun=0;
    doStop=0;
    doReadout=0;
    
    if (argc<2) {
	usage();
	exit(-1);
    }    

    while (argv[adrPos][0]=='-') {
	if (strcmp(argv[adrPos],"-s")==0) {
	    doStop=1;
	} else
	if (strcmp(argv[adrPos],"-r")==0) {
	    doRun=1;
	} else
	if (strncmp(argv[adrPos],"--readout",9)==0) {
	    doReadout=1;
	} else
	if (strcmp(argv[adrPos],"-d")==0) {
	    daveDebug=daveDebugAll;
	} 
	else if (strcmp(argv[adrPos],"-w")==0) {
	    doWrite=1;
	} 
	else if (strcmp(argv[adrPos],"-b")==0) {
	    doBenchmark=1;
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
	else if (strcmp(argv[adrPos],"-e")==0) {
	    doExperimental=1;
	}    
	adrPos++;
	if (argc<=adrPos) {
	    usage();
	    exit(-1);
	}	
    }    
    
    fds.rfd=setPort(argv[adrPos],"9600",'E');
    fds.wfd=fds.rfd;
    if (fds.rfd>0) { 
	di =daveNewInterface(fds, "IF1", 0, daveProtoPPI, daveSpeed187k);
	dc =daveNewConnection(di,2,0,0);  // insert your PPI address here
	daveConnectPLC(dc);
//
// just try out what else might be readable in an S7-200 (on your own risk!):
//
/*
	for (i=0;i<=255;i++) {
	    printf("Trying to read 8 bytes from area %d.\n",i);
	    wait();
	    res=daveReadBytes(dc,i,0,0,10,NULL);
	    printf("Function result: %d %s\n",res,daveStrerror(res));
	    _daveDump("Data",dc->resultPointer,dc->AnswLen);
	}    
*/	    
	printf("Trying to read 64 bytes (32 words) from data block 1.\n This is V memory of the 200.\n");
	wait();
        res=daveReadBytes(dc,daveDB,1,0,64,NULL);
	if (res==0) {
	    a=daveGetWORD(dc);
	    printf("VW0: %d\n",a);
	    a=daveGetWORD(dc);
	    printf("VW2: %d\n...\n",a);
	}
//	a=daveGetWORDat(dc,62);
//	printf("DB1:DW32: %d\n",a);
	
	printf("Trying to read 16 bytes from FW0.\n");
	wait();
/*
 * Some comments about daveReadBytes():
 *
 * The 200 family PLCs have the V area. This is accessed like a datablock with number 1.
 * This is not a quirk or convention introduced by libnodave, but the command transmitted 
 * to the PLC is exactly the same that would read from DB1 of a 300 or 400.
 *
 * to read VD68 and VD72 use:
 * 	daveReadBytes(dc, daveDB, 1, 68, 6, NULL);
 * to read VD68 and VD72 into your applications buffer appBuffer use:	
 * 	daveReadBytes(dc, daveDB, 1, 68, 6, appBuffer);
 * to read VD68 and VD78 into your applications buffer appBuffer use:	
 * 	daveReadBytes(dc, daveDB, 1, 68, 14, appBuffer);
 * this reads DBD68 and DBD78 and everything in between and fills the range
 * appBuffer+4 to appBuffer+9 with unwanted bytes, but is much faster than:
 *	daveReadBytes(dc, daveDB, 1, 68, 4, appBuffer);
 *	daveReadBytes(dc, daveDB, 1, 78, 4, appBuffer+4);
 */	
	res=daveReadBytes(dc,daveFlags,0,0,16,NULL);
	if (res==0) {
/*
 *	daveGetDWORD(dc); reads a word (2 bytes) from the current buffer position and increments
 *	an internal pointer by 2, so next daveGetXXX() wil read from the new position behind that
 *	word.	
 */	
    	    a=daveGetDWORD(dc);
            b=daveGetDWORD(dc);
	    c=daveGetDWORD(dc);
    	    d=daveGetFloat(dc);
	    printf("FD0: %d\n",a);
	    printf("FD4: %d\n",b);
	    printf("FD8: %d\n",c);
	    printf("FD12: %f\n",d);
/*
	    d=daveGetFloatAt(dc,12);
	    printf("FD12: %f\n",d);
*/	    
	}	    
	
	if(doNewfunctions) {
//	    saveDebug=daveDebug;
//	    daveDebug=daveDebugAll;
	    
	    printf("Trying to read 2 bit from I0.2\n");
	    res=daveReadBits(dc, daveInputs, 0, 2, 1,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    if (res==0) {	
		c=daveGetByte(dc);
		printf("Bit: %d\n",c);
	    }	
//	    daveDebug=0;	
	    printf("Trying to read 2 bits from DB1.DBX0.1=V0.1\n");
	    res=daveReadBits(dc, daveDB, 1, 1, 2,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    if (res==0) {	
		c=daveGetByte(dc);
		printf("Bit: %d\n",c);
	    }	
	    printf("Trying to read 0 bits from DB1.DBX0.1=V0.1\n");
	    res=daveReadBits(dc, daveDB, 1, 1, 0,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    if (res==0) {	
		c=daveGetByte(dc);
		printf("Bit: %d\n",c);
	    }
	    printf("Trying to read 1 bit from DB1.DBX0.1=V0.1\n");
	    res=daveReadBits(dc, daveDB, 1, 1, 1,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    if (res==0) {	
		c=daveGetByte(dc);
		printf("Bit: %d\n",c);
	    }	
	    a=0;
	    printf("Writing 0 to QB0\n");
	    res=daveWriteBytes(dc, daveOutputs, 0, 0, 1, &a);
//	    daveDebug=daveDebugAll;
	    a=1;
	    printf("About to write 1 to Q0.4\n");
	    wait();
	    res=daveWriteBits(dc, daveOutputs, 0, 4, 1, &a);
	    a=255;
	    printf("About to write %d to QB0\n",a);
	    wait();
	    res=daveWriteBytes(dc, daveOutputs, 0, 0, 1, &a);
	    a=0;
	    printf("About to write 0 to Q0.3\n");
	    wait();
	    res=daveWriteBits(dc, daveOutputs, 0, 4, 1, &a);
	    
	    printf("Trying to read 1 word from AQW0\n");
	    res=daveReadBytes(dc, daveAnaOut, 0, 0, 1,NULL);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    a=2341;
	    printf("Trying to write 1 word to AQW0\n");
	    res=daveWriteBytes(dc, daveAnaOut, 0, 0, 2,&a);
	    printf("function result:%d=%s\n", res, daveStrerror(res));
	    
	    printf("Trying to read 3 items from Timers\n");
	    res=daveReadBytes(dc, daveTimer200, 0, 0, 4,NULL);
	    printf("function result:%d=%s length:%d\n", res, daveStrerror(res),dc->AnswLen);
	    if (res==0) {
		printf("Bytes:");
		    for (b= 0;b<dc->AnswLen;b++) {
			c=daveGetByte(dc);
			printf(" %0x, ",c);
		    }
		    printf("\n");
/*		    
		d=daveGetSeconds(dc);
		printf("Times(by getSeconds()  ): %0.3f, ",d);
		d=daveGetSeconds(dc);
		printf("%0.3f, ",d);
		d=daveGetSeconds(dc);
		printf("%0.3f, ",d);
		d=daveGetSeconds(dc);
		printf(" %0.3f\n",d);
	    
		d=daveGetSecondsAt(dc,0);
		printf("Times(by getSecondsAt()): %0.3f, ",d);
		d=daveGetSecondsAt(dc,2);
		printf("%0.3f, ",d);
		d=daveGetSecondsAt(dc,4);
		printf("%0.3f, ",d);
		d=daveGetSecondsAt(dc,6);
		printf(" %0.3f\n",d);
*/		
	    }
	    printf("Trying to read 3 items from Counters\n");
	    res=daveReadBytes(dc, daveCounter200, 0, 0, 3,NULL);
	    printf("function result:%d=%s length:%d\n", res, daveStrerror(res),dc->AnswLen);
	    if (res==0) {	
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
	    }	
	}    
	if(doExperimental) {
	    for (i= 0;i<8;i++) {
		printf("Trying to read 14 items from %d\n",i);
		res=daveReadBytes(dc, i, 0, 0, 54,NULL);
		printf("function result:%d=%s length:%d\n", res, daveStrerror(res),dc->AnswLen);
		
		if (res==0) {	
		    printf("Bytes:");
		    for (b= 0;b<dc->AnswLen;b++) {
			c=daveGetByte(dc);
			printf(" %0x, ",c);
		    }
		    printf("\n");
		}    
	    }
//	    daveDebug=saveDebug;
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
	    daveAddVarToReadRequest(&p,daveSysInfo,0,0,24);
	    daveAddVarToReadRequest(&p,daveFlags,0,12,2);
	    daveAddVarToReadRequest(&p,daveAnaIn,0,0,2);
	    daveAddVarToReadRequest(&p,daveAnaOut,0,0,2);
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
		
	    printf("DB 6 Word 20 (not present in 200): ");	
	    res=daveUseResult(dc, rs, 2); // 3rd result
	    if (res==0) {
		a=daveGetInteger(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));
		
	    printf("System Information: ");	
	    res=daveUseResult(dc, rs, 3); // 4th result
	    if (res==0) {
		for (i=0;i<40;i++) {
		a=daveGetByte(dc);
        	printf("%c",a);
		}	
        	printf("\n");
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));	
		
	    printf("Flag Word 12: ");		
	    res=daveUseResult(dc, rs, 4); // 5th result
	    if (res==0) {
		a=daveGetWORD(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));	
		
	    printf("non existing result: ");		
	    res=daveUseResult(dc, rs, 4); // 5th result
	    if (res==0) {
		a=daveGetWORD(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));		
	    
	    printf("Analog Input Word 0:");		
	    res=daveUseResult(dc, rs, 5); // 6th result
	    if (res==0) {
		a=daveGetWORD(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));
	    
	    printf("Analog Output Word 0:");		
	    res=daveUseResult(dc, rs, 6); // 7th result
	    if (res==0) {
		a=daveGetWORD(dc);
        	printf("%d\n",a);
	    } else 
		printf("*** Error: %s\n",daveStrerror(res));
	    daveFreeResults(&rs);    
/*	    
	    for (i=0; i<rs.numResults;i++) {
		r2=&(rs.results[i]);
		printf("result: %s length:%d\n",daveStrerror(r2->error), r2->length);
		res=daveUseResult(dc, rs, i);
		if (r2->length>0) _daveDump("bytes",r2->bytes,r2->length);
		if (r2->bytes!=NULL) {
	    	    _daveDump("bytes",r2->bytes,r2->length);
	            d=daveGetFloat(dc);
	            printf("FD12: %f\n",d);
		}	 
	    }
*/	    
	}	    

	if(doWrite) {
    	    printf("Now we write back these data after incrementing the first to by 1,2 and 3 and the first two floats by 1.1.\n");
    	    wait();
/*
    Attention! you need to bswap little endian variables before using them as a buffer for
    daveWriteBytes() or before copying them into a buffer for daveWriteBytes()!
*/	    
    	    a=bswap_32(a+1);
    	    daveWriteBytes(dc,daveFlags,0,0,4,&a);
    	    b=bswap_32(b+2);
    	    daveWriteBytes(dc,daveFlags,0,4,4,&b);
    	    c=bswap_32(c+3);
	    daveWriteBytes(dc,daveFlags,0,8,4,&c);
    	    d=toPLCfloat(d+1.1);
    	    daveWriteBytes(dc,daveFlags,0,12,4,&d);
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
	if(doStop) {
	    daveStop(dc);
	}
	if(doRun) {
	    daveStart(dc);
	}
	if(doReadout) {
	    loadBlocksOfType(dc, daveBlockType_OB);
	    loadBlocksOfType(dc, daveBlockType_DB);
	    loadBlocksOfType(dc, daveBlockType_SDB);
	}    

	
	if(doBenchmark) {
	    rBenchmark(dc);
	    if(doWrite) {
		wBenchmark(dc);
	    } // doWrite
	} // doBenchmark

	return 0;
    } else {
	printf("Couldn't open serial port %s\n",argv[adrPos]);	
	return -1;
    }	
}

/*
    Changes: 
    07/19/04 added return values in main().
    09/09/04 applied patch for variable Profibus speed from Andrew Rostovtsew.
    09/09/04 removed unused include byteswap.h
    09/10/04 removed SZL read, it doesn?t work on 200 family.
    09/11/04 added multiple variable read example code.
*/
