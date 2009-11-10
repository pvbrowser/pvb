/*
 Test and demo program for Libnodave, a free communication libray for Siemens S7.
 
 **********************************************************************
 * WARNING: This and other test programs overwrite data in your PLC.  *
 * DO NOT use it on PLC's when anything is connected to their outputs.*
 * This is alpha software. Use entirely on your own risk.             * 
 **********************************************************************
 
 (C) Thomas Hergenhahn (thomas.hergenhahn@web.de) 2002, 2003, 2004.

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

//#define PLAY_WITH_KEEPALIVE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nodave.h"
#include "openSocket.h"

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

#ifdef PLAY_WITH_KEEPALIVE
#include <sys/socket.h>
#endif
#include <errno.h>


void readSZL(daveConnection *dc,int id, int index) {
    int res, SZLid, indx, SZcount, SZlen,i,j,len;
    uc * d,*dd;
    uc ddd[3000];
    printf("Trying to read SZL-ID %04X index %02X.\n",id,index);
    res=daveReadSZL(dc,id,index, ddd);
    printf("Function result: %d %s len:%d\n",res,daveStrerror(res),dc->AnswLen);
//    _daveDump("Data",dc->resultPointer,dc->AnswLen);
    _daveDump("Data",ddd,dc->AnswLen);
    
    if ((dc->AnswLen)>=4) {
	d=ddd;
        dd=ddd;
	dd+=8;
	len=dc->AnswLen-8;
	SZLid=0x100*d[0]+d[1]; 
        indx=0x100*d[2]+d[3]; 
	printf("result SZL ID %04X %02X \n",SZLid,indx);
    
	if ((dc->AnswLen)>=8) {
    	    SZlen=0x100*d[4]+d[5]; 
    	    SZcount=0x100*d[6]+d[7]; 
	    printf("%d elements of %d bytes\n",SZcount,SZlen);
	    if(len>0){
	    for (i=0;i<SZcount;i++){
		if(len>0){
		for (j=0; j<SZlen; j++){
		    if(len>0){
		    printf("%02X,",*dd);
		    dd++;
		    }
		    len--;
		}
		printf("\n");
		}
	    }
	    }
	}
    }
    printf("\n");
}    

void readSZLAll(daveConnection *dc) {
    uc SzlList[1000];
    int res, SZLid, indx, SZcount, SZlen,i,j, rid, rind;
    uc * d,*dd;
    
    res=daveReadSZL(dc,0,0,SzlList);
    printf("%d %d\n",res,dc->AnswLen);
    if ((dc->AnswLen)>=4) {
	d=dc->resultPointer;
        dd=SzlList;
	dd+=8;
	SZLid=0x100*d[0]+d[1]; 
        indx=0x100*d[2]+d[3]; 
	printf("result SZL ID %04X %02X \n",SZLid,indx);
    
	if ((dc->AnswLen)>=8) {
    	    SZlen=0x100*d[4]+d[5]; 
    	    SZcount=0x100*d[6]+d[7]; 
	    printf("%d elements of %d bytes\n",SZcount,SZlen);
	    for (i=0;i<SZcount;i++){
//		rind=*dd;
//		rid=*(dd+1); 
		rid=*(dd+1)+256*(*dd); 
		rind=0;
		printf("ID:%04X Index:%02X\n",rid,rind);
		readSZL(dc, rid, rind);
		for (j=0; j<SZlen; j++){
		    printf("%02X,",*dd);
		    dd++;
		}
		printf("\nID:%04X Index:%02X\n",rid,rind);
		
	    }
	}
    }
    printf("\n");
}


void wait() {
    uc c;
    printf("Press return to continue.\n");
#ifdef UNIX_STYLE
    read(0,&c,1);
#endif
}    

void usage()
{
    printf("Usage: testISO_TCP [-d] [-w] IP-Address of CP\n");
    printf("-2 uses a protocol variant for the CP243. You need to set it, if you use such a CP.\n");
    printf("-w will try to write to Flag words. It will overwrite FB0 to FB15 (MB0 to MB15) !\n");
    printf("-d will produce a lot of debug messages.\n");
    printf("-b will run benchmarks. Specify -b and -w to run write benchmarks.\n");
    printf("-l will run a test to determine maximum length of a block in read.\n");
    printf("-m will run a test for multiple variable reads.\n");
    printf("-c will write 0 to the PLC memory used in write tests.\n");
    printf("-z will read some SZL list items (diagnostic information).\n"
	    "  Works 300 and 400 family only.\n");
    printf("-s stops the PLC.\n");
    printf("-r tries to put the PLC in run mode.\n");
    printf("--readout read program and data blocks from PLC.\n");
    printf("--readoutall read all program and data blocks from PLC. Includes SFBs and SFCs.\n");
    printf("--slot=<number> sets slot for PLC (default is 2).\n");
    printf("Example: testISO_TCP -w 192.168.19.1\n");
}

void loadBlocksOfType(daveConnection * dc, int blockType) {
    int j, i, uploadID, len, more;
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

int main(int argc, char **argv) {
    int a,b,c,adrPos,doWrite,doBenchmark, doSZLread, doMultiple, doClear,
	res, useProtocol,doSZLreadAll, doRun, doStop, doReadout, doSFBandSFC,useSlot;
#ifdef PLAY_WITH_KEEPALIVE    	
    int opt;
#endif    
    float d;
    daveInterface * di;
    daveConnection * dc;
    _daveOSserialType fds;
    PDU p;
    daveResultSet rs;
    
    daveDebug=daveDebugPrintErrors;
    adrPos=1;
    doWrite=0;
    doBenchmark=0;
    doMultiple=0;
    doClear=0;
    doSZLread=0;
    doSZLreadAll=0;
    doRun=0;
    doStop=0;
    doReadout=0;
    doSFBandSFC=0;
    
    useProtocol=daveProtoISOTCP;
    useSlot=2;
    
    
    if (argc<2) {
	usage();
	exit(-1);
    }    
    
    while (argv[adrPos][0]=='-') {
	if (strcmp(argv[adrPos],"-2")==0) {
	    useProtocol=daveProtoISOTCP243;
	} else
	if (strcmp(argv[adrPos],"-s")==0) {
	    doStop=1;
	} else if (strcmp(argv[adrPos],"-r")==0) {
	    doRun=1;
	} else if (strncmp(argv[adrPos],"--readoutall",12)==0) {
	    doReadout=1;
	    doSFBandSFC=1;
	} else if (strncmp(argv[adrPos],"--readout",9)==0) {
	    doReadout=1;
	} else if (strncmp(argv[adrPos],"--slot=",7)==0) {
	    useSlot=atol(argv[adrPos]+7);
	} else if (strcmp(argv[adrPos],"-d")==0) {
	    daveDebug=daveDebugAll;
	} else
	if (strcmp(argv[adrPos],"-w")==0) {
	    doWrite=1;
	} else
	if (strcmp(argv[adrPos],"-b")==0) {
	    doBenchmark=1;
	} else
	if (strcmp(argv[adrPos],"-z")==0) {
	    doSZLread=1;
	} else
	if (strcmp(argv[adrPos],"-a")==0) {
	    doSZLreadAll=1;
	} else
	if (strcmp(argv[adrPos],"-m")==0) {
	    doMultiple=1;
	}
	adrPos++;
	if (argc<=adrPos) {
	    usage();
	    exit(-1);
	}	
    }    
    
    fds.rfd=openSocket(102, argv[adrPos]);
#ifdef PLAY_WITH_KEEPALIVE
    errno=0;    
    opt=1;
    res=setsockopt(fds.rfd, SOL_SOCKET, SO_KEEPALIVE, &opt, 4);
    LOG3("setsockopt %s %d\n", strerror(errno),res);
#endif
    fds.wfd=fds.rfd;
    
    if (fds.rfd>0) { 
	di =daveNewInterface(fds,"IF1",0, useProtocol, daveSpeed187k);
	di->timeout=5000000;
	dc =daveNewConnection(di,2,0,useSlot);  // insert your rack and slot here
	
	if (0==daveConnectPLC(dc)) {
	    printf("Connected.\n");
//	    di->timeout=1000;
/*		
	printf("Trying to read 1 byte from system data,address 0x1e3 like MicroWin does.\n");
	daveReadBytes(dc,3,0,0x1e3,1,NULL);
	
	printf("Trying to read 20 bytes from system data,address 0 like MicroWin does.\n");
	daveReadBytes(dc,3,0,0,20,NULL);
*/	
	printf("Trying to read 64 bytes (16 dwords) from data block 1.\n");
	wait();
	res=daveReadBytes(dc,daveDB,1,0,64,NULL);
        if(0==res) {
	    a=daveGetWORD(dc);
	    printf("DB1:DW0: %d\n",a);
	    a=daveGetWORD(dc);
	    printf("DB1:DW1: %d\n...\n",a);
	    a=daveGetWORDat(dc,62);
	    printf("DB1:DW32: %d\n",a);
	} else 
	    printf("failed! (%d)\n",res);  
	printf("Trying to read 16 bytes from FW0.\n");
	wait();
/*
 * Some comments about daveReadBytes():
 *
 * Here we read flags and not data blocks, because we cannot know which data blocks will
 * exist in a user's PLC, but flags are always present. To read from data block 5 use:
 * 	daveReadBytes(dc, daveDB, 5, 0, 16, NULL);
 * to read DBD68 and DBW72 use:
 * 	daveReadBytes(dc, daveDB, 5, 68, 6, NULL);
 * to read DBD68 and DBW72 into your applications buffer appBuffer use:	
 * 	daveReadBytes(dc, daveDB, 5, 68, 6, appBuffer);
 * to read DBD68 and DBD78 into your applications buffer appBuffer use:	
 * 	daveReadBytes(dc, daveDB, 5, 68, 14, appBuffer);
 * this reads DBD68 and DBD78 and everything in between and fills the range
 * appBuffer+4 to appBuffer+9 with unwanted bytes, but is much faster than:
 *	daveReadBytes(dc, daveDB, 5, 68, 4, appBuffer);
 *	daveReadBytes(dc, daveDB, 5, 78, 4, appBuffer+4);
 *
 * Users of the 200 family please note:
 * The 200 family PLCs have the V area. This is accessed like a datablock with number 1.
 * This is not a quirk or convention introduced by libnodave, but the command transmitted 
 * to the PLC is exactly the same that would read from DB1 off a 300 or 400.
 *
 */	
	res=daveReadBytes(dc,daveFlags,0,0,16,NULL);
	if (0==res) { 
/*
 *	daveGetDWORD(dc); reads a word (2 bytes) from the current buffer position and increments
 *	an internal pointer by 2, so next daveGetXXX() wil read from the new position behind that
 *	word. If you want to read from a position you specify, use daveGetWORDat(dc, position).	
 */	
    	    a=daveGetDWORD(dc);
    	    b=daveGetDWORD(dc);
    	    c=daveGetDWORD(dc);
    	    d=daveGetFloat(dc);
	    printf("FD0: %d\n",a);
	    printf("FD4: %d\n",b);
	    printf("FD8: %d\n",c);
	    printf("FD12: %f\n",d);
	}  else 
	    printf("failed! (%d)\n",res);  
	
	if(doSZLread) {
	    readSZL(dc,0xA0,0x0);
	    readSZL(dc,0x92,0x0);
	    readSZL(dc,0xB4,0x1024);
	    readSZL(dc,0x111,0x1);
	    readSZL(dc,0x111,0x2);
	    readSZL(dc,0xD91,0x0);
	    readSZL(dc,0x232,0x4);
	    readSZL(dc,0x1A0,0x0);
	}    
	if(doSZLreadAll) {
	    readSZLAll(dc);
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
		
	    printf("non existing result: ");		
	    res=daveUseResult(dc, rs, 4); // 5th result
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
    	    printf("Now we write back these data after incrementing the first 3 by 1,2,3 and the float by 1.1.\n");
	    wait();
/*
    Attention! you need to bswap little endian variables before using them as a buffer for
    daveWriteBytes() or before copying them into a buffer for daveWriteBytes()!
*/	    
    	    a=bswap_32(a+1);
    	    daveWriteBytes(dc,daveDB,17,00,4,&a);
    	    b=bswap_32(b+1);
    	    daveWriteBytes(dc,daveDB,17,4,4,&b);
    	    c=bswap_32(c+1);
	    daveWriteBytes(dc,daveDB,17,8,4,&c);
    	    d=toPLCfloat(d+1.1);
    	    daveWriteBytes(dc,daveDB,17,12,4,&d);
    	    if(0==daveReadBytes(dc,daveDB,17,0,16,NULL)) {
		a=daveGetDWORD(dc);
    		b=daveGetDWORD(dc);
    		c=daveGetDWORD(dc);
    		d=daveGetFloat(dc);
		printf("FD0: %d\n",a);
		printf("FD4: %d\n",b);
		printf("FD8: %d\n",c);
		printf("FD12: %f\n",d);
	    } else {
		
	    }	
    	    wait();
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
	    loadBlocksOfType(dc, daveBlockType_FB);
	    loadBlocksOfType(dc, daveBlockType_FC);
	    loadBlocksOfType(dc, daveBlockType_DB);
	    loadBlocksOfType(dc, daveBlockType_SDB);
	    if (doSFBandSFC){	    
		loadBlocksOfType(dc, daveBlockType_SFB);
	        loadBlocksOfType(dc, daveBlockType_SFC);
	    }
	}    
	if(doBenchmark) {
	    rBenchmark(dc);
	    if(doWrite) {
		wBenchmark(dc);
	    } // doWrite
	} // doBenchmark

	printf("Finished.\n");
	return 0;
	} else {
	    printf("Couldn't connect to PLC.\n Please make sure you use the -2 option with a CP243 but not with CPs 343 or 443.\n");	
	    return -2;
	}
    } else {
	printf("Couldn't open TCP port. \nPlease make sure a CP is connected and the IP address is ok. \n");	
    	return -1;
    }    
}

/*
    Changes: 
    07/19/04 removed unused vars.
    09/09/04 applied patch for variable Profibus speed from Andrew Rostovtsew.
    09/09/04 removed unused includes byteswap.h. Made some code to test TCP keepalive conditional.
    03/03/05 added slot select option.
*/
