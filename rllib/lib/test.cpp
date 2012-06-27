//
// SiemensTCP test                                (C) R. Lehrig 2005
//

#include <stdio.h>
#include <stdlib.h>
#include "rlsiemenstcp.h"
#include "rlcutil.h"

int main()
{
  int i,ret;
  unsigned char buf[512];
  
  rlSetDebugPrintf(0); // set debugging on
  rlSiemensTCP plc("192.168.1.101",rlSiemensTCP::S7_200,0); 
  
  //int write (int org, int dbnr, int start_adr, int length, const unsigned char *buf)
  //int fetch (int org, int dbnr, int start_adr, int length, unsigned char *buf)

  for(i=1; i< 6; i++)
  {
    printf("ORG_DB begin\n");
    buf[0] = buf[1] = buf[2] = buf[3] = i;
    ret = plc.write(rlSiemensTCP::ORG_DB,1,0,4,buf);
    printf("plc.write: ret=%d %x %x %x %x\n",ret,buf[0],buf[1],buf[2],buf[3]);
    buf[0] = buf[1] = buf[2] = buf[3] = 0;
    ret = plc.fetch(rlSiemensTCP::ORG_DB,1,0,4,buf);
    printf("plc.fetch: ret=%d %x %x %x %x\n",ret,buf[0],buf[1],buf[2],buf[3]);
    printf("ORG_DB end\n");
    
    printf("ORG_M begin\n");
    buf[0] = buf[1] = buf[2] = buf[3] = i;
    ret = plc.write(rlSiemensTCP::ORG_M,0,0,4,buf);
    printf("plc.write: ret=%d %x %x %x %x\n",ret,buf[0],buf[1],buf[2],buf[3]);
    buf[0] = buf[1] = buf[2] = buf[3] = 0;
    ret = plc.fetch(rlSiemensTCP::ORG_M,0,0,4,buf);
    printf("plc.fetch: ret=%d %x %x %x %x\n",ret,buf[0],buf[1],buf[2],buf[3]);
    printf("ORG_M end\n");
  }
  return 0;
}
