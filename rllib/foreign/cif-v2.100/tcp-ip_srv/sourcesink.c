#include <ctype.h>
#include "SOCKET.h"
#include "ODMTCP.h"

extern int fdrvConn;

void
sink(int sockfd)
{
  int	n = 0, j = 0;
  short sRet = DRV_NO_ERROR;
  unsigned short usBoard = 0xff, usCnt = 0;
  GENERAL_BOARD_INFO tBoardsInfo;
  MSG_STRUC  tMessage;
    
  if (pauseinit)
    sleep(pauseinit);

  /* Open the driver */
  if ( (sRet = DevOpenDriver() ) != DRV_NO_ERROR) {
    fdrvConn = 0;
    DBG_PRN( "DevOpenDriver(): Ret = %5d \n", sRet );
  }
  else 	
    fdrvConn = 1;
  
  for ( ; ; ) {
    if ( (n = read(sockfd, rbuf, readlen)) < 0) {
      err_sys("read error");
      
    } else if (n == 0) {
      break;	 	/* connection closed by peer */

    } else if (n != readlen) {
      DBG_PRN("read returned %d, start: %.2X%.2X%.2X%.2X%.2X\n", n, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
      if ( memcmp( rbuf, "ODM", 3) == 0) {
         switch ( rbuf[3]) {
         case BOARD_SELECT_COMMAND:  // client requested BoardInfo
	      memcpy( &wbuf[0], "ODM", 3);
	      wbuf[3] = BOARD_SELECT_ANSWER;
	      j = sizeof(tBoardsInfo);
	      memset( (unsigned char *)&tBoardsInfo, 0x00, j);
              if ( (sRet = DevGetBoardInfoEx( &tBoardsInfo.tBoardInfo )) != DRV_NO_ERROR) {
                 DBG_PRN( "DevGetBoardInfoEx()       RetWert = %5d \n", sRet );
              }
              else {
                 for ( usCnt = 0; usCnt < MAX_DEV_BOARDS; usCnt++)
		   { /* check if board is available */
                     sRet = DevInitBoard(  usCnt );                            
                     tBoardsInfo.sErrorNumber[usCnt] = sRet;  // save error
                     sRet = DevExitBoard( usCnt );
                   }
	      }
	      memcpy( &wbuf[4], (unsigned char *)&tBoardsInfo, j);
	      DBG_PRN("(2)%C%C%C%.2X\n", wbuf[0], wbuf[1], wbuf[2], wbuf[3]);
	  
	      if ( (n = write(sockfd, wbuf, j + 4)) != (j + 4) )
	         err_sys("write returned %d, expected %d", n, writelen);
	      DBG_PRN("wrote %d bytes\n", n);
	  
	      if (pauserw)
	         sleep(pauserw);
              DBG_PRN("\n%.16s\n", &tBoardsInfo.tBoardInfo.tBoard[0].tFirmware.abFirmwareName[0]);
#ifdef SRV_DEBUG
for(j=0;j<16;j++)
 printf("%C", (char)tBoardsInfo.tBoardInfo.tBoard[0].tFirmware.abFirmwareName[j]);
for(j=0;j<sizeof(GENERAL_BOARD_INFO) + 4;j++)
 printf("%.2X-", wbuf[j]);
#endif
	      break;
         case BOARD_SELECT_ANSWER:  // client selected board
	      usBoard = (unsigned short)rbuf[4];
              if ( (sRet = DevInitBoard ( usBoard)) != DRV_NO_ERROR) 
	         DBG_PRN("board %d: DevInitBoard() error: %d \n", usBoard, sRet);
	      else
	         DBG_PRN("board %d initialized\n", usBoard);
	      break;
         default:  
	      DBG_PRN("recved %d bytes: %.2X%.2X%.2X%.2X%.2X ...\n", n, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
	      break;
         }
      }
      else { // this is the bare RCS message
         memcpy((unsigned char*)&tMessage, &rbuf[0], n);
	 /* send client message to the board */
         if ( (sRet = DevPutMessage( usBoard, &tMessage, 2000L)) != DRV_NO_ERROR) 
            DBG_PRN( "Error by driver function DevPutMessage(). (Error: %d)\n", sRet);
         else {  /* get response message from board */
	    for (j = 0; j < 5; j++) {  
	      if ( (sRet = DevGetMessage( usBoard, sizeof(tMessage), &tMessage, 10000L)) != DRV_NO_ERROR) {
                 DBG_PRN( "Error by driver function DevGetMessage(). (Error: %d)\n", sRet);
	         break;
              }
	      if( tMessage.rx == 0 && tMessage.tx == 0) /* what a lot of rubbish! Throw it away! looping... */
              printf("tMessage(Get): %.2X-%.2X-%.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", tMessage.rx,
                                                                                 tMessage.tx,
                                                                                 tMessage.ln,
                                                                                 tMessage.nr,
                                                                                 tMessage.a,
                                                                                 tMessage.f,
                                                                                 tMessage.b,
                                                                                 tMessage.e );
	      else 
	         break;
	    }
         }
         if ( j < 5 ) { // there is message for the client, send it over tcp/ip socket
            if ( (n = write(sockfd, (unsigned char*)&tMessage, sizeof(tMessage))) != sizeof(tMessage))
	       err_sys("write returned %d, expected %d", n, writelen);
            else 
	       DBG_PRN("written %d bytes\n", n);
         } /* else { 'client runs into timeout' } */
      }
    }    
    if (pauserw)
      sleep(pauserw);
  }
  
  if (pauseclose) {
    DBG_PRN("pausing before close\n");
    sleep(pauseclose);
  }
  
  if (close(sockfd) < 0)
    err_sys("close error");		/* since SO_LINGER may be set */
}
