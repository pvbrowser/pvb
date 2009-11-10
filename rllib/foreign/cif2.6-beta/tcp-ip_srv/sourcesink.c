/* <St> *******************************************************************

   sourcesink.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 28.01.2005
  PROJEKT       : tcp-ip server for the Linux CIF Device Driver
  =========================================================================

  DISCRIPTION
    sink routine               .
  =========================================================================

  CHANGES
  version name      date        Discription
                 
  V1.001	    01/05       RCS messages are sent now to the client
		                considering the real data length in it
                                i.e. client gets message header lenght + 
                                                 dala.len
                                usBoard is now defined extern
-------------------------------------------------------------------------
  V1.001

  ======================== Copyright =====================================

  Copyright (C) 2004  Hilscher GmbH

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
   
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

  ========================================================================
******************************************************************** <En> */
#include <ctype.h>
#include "SOCKET.h"
#include "ODMTCP.h"

extern int fdrvConn;
extern unsigned short usBoard;

void
sink(int sockfd)
{
  int	n = 0, j = 0;
  short sRet = DRV_NO_ERROR;
  unsigned short usCnt = 0;
  GENERAL_BOARD_INFO tBoardsInfo;
  MSG_STRUC  tMessage;
    
  if (pauseinit)
    sleep(pauseinit);

  /* Open the driver */
  if ( (sRet = DevOpenDriver() ) != DRV_NO_ERROR) {
    fdrvConn = 0;
    DBG_PRN( "DevOpenDriver(): Ret = %5d \n", sRet );
  } else 	
    fdrvConn = 1;
  
  for ( ; ; ) {
    if ( (n = read(sockfd, rbuf, readlen)) < 0) 
      err_sys("read error");  
    else if (n == 0)
      break;	 	/* connection closed by peer */
    else { //if (n != readlen) {
      DBG_PRN("read returned %d, start: %.2X%.2X%.2X%.2X%.2X\n", n, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
      if ( memcmp( rbuf, "ODM", 3) == 0) {
        switch ( rbuf[3]) {
        case BOARD_SELECT_COMMAND:  // client requested BoardInfo
	     memcpy( &wbuf[0], "ODM", 3);
	     wbuf[3] = BOARD_SELECT_ANSWER;
	     j = sizeof(tBoardsInfo);
	     DBG_PRN("ODM->BOARD_SELECT_ANSWER, SIZE = %d \n", j);
	     memset( (unsigned char *)&tBoardsInfo, 0x00, j);
             if ( (sRet = DevGetBoardInfoEx( &tBoardsInfo.tBoardInfo )) != DRV_NO_ERROR) {
               DBG_PRN( "DevGetBoardInfoEx()       RetWert = %5d \n", sRet );
             } else {
	       DBG_PRN("ODM->BOARD_SELECT_ANSWER, check if board is available \n");
	       for( usCnt = 0; usCnt < MAX_DEV_BOARDS; usCnt++) { 
                 sRet = DevInitBoard(  usCnt );                            
                 tBoardsInfo.sErrorNumber[usCnt] = sRet;  // save error
                 sRet = DevExitBoard( usCnt );
	       }
	      
	       memcpy( &wbuf[4], (unsigned char *)&tBoardsInfo, j);
	       DBG_PRN("(2)%C%C%C%.2X\n", wbuf[0], wbuf[1], wbuf[2], wbuf[3]);
	  
	       if ( (n = write(sockfd, wbuf, j + 4)) != (j + 4) )
	         err_sys("write returned %d, expected %d", n, writelen);
	       DBG_PRN("wrote %d bytes\n", n);
	  
	       if (pauserw)
	         sleep(pauserw);
               DBG_PRN("\n%.16s\n", &tBoardsInfo.tBoardInfo.tBoard[0].tFirmware.abFirmwareName[0]);
	      }
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
      } else { // this is the bare RCS message
        memcpy((unsigned char*)&tMessage, &rbuf[0], n);	 
        if( (sRet = DevPutMessage( usBoard, &tMessage, 2000L)) != DRV_NO_ERROR) /* send client message to the board */
	   DBG_PRN( "Error by driver function DevPutMessage(). (Error: %d), Board %d\n", sRet, usBoard);
        else {  /* get response message from board */
          DBG_PRN("tMessage(Get): %.2X-%.2X-%.2X-%.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", tMessage.rx, tMessage.tx, tMessage.ln, tMessage.nr, tMessage.a,
                                                                                    tMessage.f,  tMessage.b,  tMessage.e,  tMessage.data[0]);
          memset( (unsigned char*)&tMessage, 0, sizeof( tMessage));
	  if( (sRet = DevGetMessage( usBoard, sizeof( tMessage), &tMessage, 10000L)) != DRV_NO_ERROR) {
            DBG_PRN( "Error by driver function DevGetMessage(). (Error: %d)\n", sRet);
          } else { // there is message for the client, send it over tcp/ip socket
            if ( (n = write( sockfd, (unsigned char*)&tMessage, tMessage.ln +8)) != (tMessage.ln + 8) )
	      err_sys("(forward get msg) write returned %d, expected %d", n, writelen);
            else 
	      DBG_PRN("(forward get msg) written %d bytes\n", n);
	  }
        } 
      } /* else { 'this is the bare RCS message' } */
    }    
    if( pauserw)
      sleep( pauserw);
  }
  
  if( pauseclose) {
    DBG_PRN( "pausing before close\n");
    sleep( pauseclose);
  }
  
  if( close( sockfd) < 0)
    err_sys( "close error");		/* since SO_LINGER may be set */
}
