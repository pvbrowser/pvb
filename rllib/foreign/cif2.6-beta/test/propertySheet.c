
/* <St> *******************************************************************

   propertySheet.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver setup und test program
  =========================================================================

  DISCRIPTION
    utility functions               .
  =========================================================================

  CHANGES
  version name      date        Discription                 
   		 Juli  2004     Copyright changed to GNU Lesser GPL		                
-------------------------------------------------------------------------
  V2.600

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

#include <gtk/gtk.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../inc/cif_types.h"    
#include "../usr-inc/cif_user.h"    /* Include file for device driver API */
#include "../usr-inc/rcs_user.h"    /* Include file for RCS definition    */
#include "../usr-inc/asc_user.h"    /* Include file for ASCII protocols   */
#include "../usr-inc/nvr_user.h"    /* Include file for 3964R protocol    */
#include "../usr-inc/defines.h"     /* Include file for 3964R protocol    */

extern BOOL fQuitting;

// Driver version         1234567890123456
char pabDrvVersion[17] = "CIFDriver V2.100"; // let it be initialized

unsigned short  usBoardAct = 0xFFFF;
unsigned short DEV_INFO_FLAGS = 0x0000;
static gint nPageAct = 0;
/*{{ BoardsPage */
GtkWidget *BoardsPage = NULL, *label_boards, *DrvVerInf;
GtkWidget *FirmwareName, *FirmwareVer, *Error;
GtkWidget *BPradio_0 = NULL, *BPradio_1 = NULL, *BPradio_2 = NULL, *BPradio_3 = NULL;
GtkWidget *BPradio_4 = NULL, *BPradio_5 = NULL;
GSList    *group = NULL;
    gint   context_id_Boards;
GtkWidget *status_bar_Boards;
/*}} BoardsPage */

/*{{ ExchangeIO-Page */
extern    GtkWidget *make_menu_item( gchar *name, GtkSignalFunc  callback, gpointer data );
GtkWidget *status_bar_ExIO;
GtkWidget *ExIOSnd=NULL, *ExIORcv=NULL;
GtkWidget *ExIOPage, *ExIOLenRcv, *ExIOLenSnd, *ExIOffRcv, *ExIOffSnd;
GtkWidget *ExIO_COMerrMode, *ExIO_COMerrState, *ExIO_COMeMode, *ExIO_COMeState;
GtkWidget *heading, *boxCOM, *checkCOMerr, *frameCOM, *handshakeMode, *ExIO_hbox3, *ExIO_hbox32;
    gint  context_id_ExIO;
gint      pExIORcvTimer, pExIOSndTimer;
BOOL      fExIORcvTimer = FALSE, fExIOSndTimer = FALSE;
#define   RCV_CYCLIC         0x01
#define   SND_CYCLIC         0x02
#define   GET_PROC_DATA      0x04
#define   SND_PROC_DATA      0x08

BOOL     fCheckComState = TRUE;
COMSTATE tComState;
unsigned short usExIOfun = 1, usIOExchangeMode = 2;
unsigned char EXIO_FLAGS = 0x00;
/*}} ExchangeIO-Page */

/*{{ ErrorPage */
GtkWidget *ErrorPage = NULL;
GtkWidget *ErrorList;
/*}} ErrorPage */

/*{{ DrvInfoPage */
GtkWidget *DrvInfoPage    = NULL;
GtkWidget *entryDrvInfo1  = NULL, *entryDrvInfo3  = NULL, *entryDrvInfo4  = NULL;
GtkWidget *entryDrvInfo5  = NULL, *entryDrvInfo6  = NULL, *entryDrvInfo7  = NULL, *entryDrvInfo8  = NULL;
GtkWidget *entryDrvInfo9  = NULL, *entryDrvInfo10 = NULL, *entryDrvInfo11 = NULL, *entryDrvInfo12 = NULL;
GtkWidget *entryDrvInfo13 = NULL, *entryDrvInfo14 = NULL, *entryDrvInfo15 = NULL, *labDrvInfo     = NULL;
GtkWidget *status_bar_DrvInfo;
    gint  context_id_DrvInfo;
/*}} DrvInfoPage */

/*{{ BoardInfoPage */
GtkWidget *BoardInfoPage = NULL;
GtkWidget *labBoardInfo1 = NULL, *labBoardInfo2 = NULL, *labBoardInfo3 = NULL;
GtkWidget *labBoardInfo4 = NULL, *labBoardInfo5 = NULL, *labBoardInfo6 = NULL, *labBoardInfo7 = NULL;
GtkWidget *entryBoardInfo1 = NULL, *entryBoardInfo2 = NULL, *entryBoardInfo3 = NULL, *entryBoardInfo4 = NULL;
GtkWidget *entryBoardInfo5 = NULL, *entryBoardInfo6 = NULL, *entryBoardInfo7 = NULL;
GtkWidget *status_bar_BoardInfo;
    gint  context_id_BoardInfo;
/*}} BoardInfoPage */

/*{{ DrvFunPage */
extern GtkWidget *DrvFunPage;
/*}} DrvFunPage */

/*{{ Msg-TransferPage */
GtkWidget *status_bar_Msg;
    gint  context_id_Msg;
GtkWidget *MsgTransPage = NULL;
GtkWidget *eMsgPutRX, *eMsgPutTX, *eMsgPutNR, *eMsgPutLN, *eMsgPutA, *eMsgPutF, *eMsgPutB, *eMsgPutE;
GtkWidget *eMsgGetRX, *eMsgGetTX, *eMsgGetNR, *eMsgGetLN, *eMsgGetA, *eMsgGetF, *eMsgGetB, *eMsgGetE;
GtkWidget *MsgGetData, *MsgPutData;
gint      pMsgTimer;
BOOL      fMsgTimer = FALSE;
unsigned short  usDataLen  = 0;
unsigned char   bMessageNr = 0;
#define  GET_CYCLIC             0x01
#define  PUT_CYCLIC             0x02
#define  PUT_START              0x04
#define  GET_START              0x08
#define  TIMER_ACTIVE           0x80
#define  AUTO_NUM               0x10
unsigned char MSG_TRANS_FLAGS = 0x00;
/*}} Msg-TransferPage */

typedef struct tagHARDWARE_INFO{
  BOOL           fIrq;                // Interrupt || polling mode
  BOOL           fInfo;               // Board Info available?
  BOOL           fInit;               // Board Initialized

  unsigned char  bDpmSize;
  unsigned char  bDevType;
  unsigned char  bDevModel;
  unsigned char  abDevIdentifier[3];

  BOARD_INFO     tBoardInfo;
} HARDWARE_INFO;

static HARDWARE_INFO tHardwareInfo;

void PopulatePages ();

GtkWidget *CreateEditField ();
char **CreateBarBitmap (int height, int width, int size, char *sColor);
void PopulateErrorList ();
extern void PopulateDrvFun ();
void PopulateDrvInfo ();
void PopulateBoardInfo ();
void PopulateBoards();
void PopulateExIO_ ();
void PopulateExIO ();
void PopulateExIOEx ();
void PopulateMsgTrans ();
void GetHitsForHour (int nHours, long *hits, long *size);

BOOL StringToHex    ( char           *pcsInText,
		      unsigned short usUserBufferLen,
		      unsigned char  *pabUserBuffer,
		      unsigned short *pusOutputLen,
		      GtkStatusbar   *pStatus,
		      gint           context_id);

void HexToTextBox   ( GtkText       *pwEdit, 
		      gshort        usBuffLen,
		      unsigned char *pabUserBuffer);
BOOL CheckEditEntry ( GtkWidget      *pEntry,
		      unsigned short *usUserBuffer,
		      long           lMaxSize, 
		      char           *chErr) ;
BOOL CheckEditEntryC( GtkWidget     *pEntry,
		      unsigned char *bUserBuffer,
		      long           lMaxSize,
		      char          *chErr); 

static void PageSwitch (GtkWidget *widget, GtkNotebookPage *page, gint page_num)
{
  if( nPageAct == 2 && page_num != 2) {
    if ( fExIORcvTimer) {
      gtk_timeout_remove ( pExIORcvTimer);
      fExIORcvTimer = FALSE;
    }
    if ( fExIOSndTimer) {
      gtk_timeout_remove ( pExIOSndTimer);
      fExIOSndTimer = FALSE;
    }
  }
  if( nPageAct == 1 && page_num != 1) {
    if ( MSG_TRANS_FLAGS & TIMER_ACTIVE) {
      gtk_timeout_remove ( pMsgTimer);
      fMsgTimer = FALSE;
      MSG_TRANS_FLAGS &= ~TIMER_ACTIVE;
      MSG_TRANS_FLAGS &= ~PUT_START;
      MSG_TRANS_FLAGS &= ~GET_START;
    }
  }
  if( (page_num == 1) && (nPageAct != 1) && !fQuitting) {
    bMessageNr = 0;
    gtk_entry_set_text(  GTK_ENTRY( eMsgPutNR), "0");   
    gtk_text_backward_delete ( (GtkText *) MsgGetData, gtk_text_get_length( (GtkText *)MsgGetData) );
    //    gtk_text_backward_delete ( GTK_TEXT( MsgPutData), gtk_text_get_length( GTK_TEXT( MsgPutData)) );
  }
  if(  (page_num == 2) && (nPageAct != 2) && !fQuitting) {
    gtk_text_backward_delete ( (GtkText *)ExIORcv, gtk_text_get_length( (GtkText *)ExIORcv) );
    //    gtk_text_backward_delete ( GTK_TEXT( ExIOSnd), gtk_text_get_length( GTK_TEXT( ExIOSnd)) );
  }
  if( nPageAct != 3 && page_num == 3) {
    gtk_label_set_text( GTK_LABEL( labBoardInfo1),  "");
    gtk_label_set_text( GTK_LABEL( labBoardInfo2),  "");
    gtk_label_set_text( GTK_LABEL( labBoardInfo3),  "");
    gtk_label_set_text( GTK_LABEL( labBoardInfo4),  "");
    gtk_label_set_text( GTK_LABEL( labBoardInfo5),  "");
    gtk_label_set_text( GTK_LABEL( labBoardInfo6),  "");
    gtk_label_set_text( GTK_LABEL( labBoardInfo7),  "");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "");
  }
  if( nPageAct != 4 && page_num == 4) {
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo1),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo3),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo4),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo5),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo6),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo7),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo8),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo9),  "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo10), "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo11), "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo12), "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo13), "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo14), "");
    gtk_entry_set_text(GTK_ENTRY( entryDrvInfo15), "");
  }
  nPageAct = page_num;
}

GtkWidget *AddPage (GtkWidget *notebook, char *szName)
{
    GtkWidget *label;
    GtkWidget *frame;

    label = gtk_label_new (szName);
    gtk_widget_show (label);

    frame = gtk_frame_new (szName);
    gtk_widget_show (frame);

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);

    return (frame);
}

void CreateNotebook (GtkWidget *window)
{
    GtkWidget *notebook;

    notebook = gtk_notebook_new ();

    gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
			  GTK_SIGNAL_FUNC (PageSwitch), NULL);

    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);

    gtk_box_pack_start (GTK_BOX (window), notebook, TRUE, TRUE, 0);

    gtk_container_border_width (GTK_CONTAINER (notebook), 10);

    /* --- Add pages to the notebook --- */
    BoardsPage    = AddPage (notebook, "Boards");
    MsgTransPage  = AddPage (notebook, "Message Transfer");
    ExIOPage      = AddPage (notebook, "Exchange IO");
    BoardInfoPage = AddPage (notebook, "Board Info");
    DrvInfoPage   = AddPage (notebook, "Driver Info");
    DrvFunPage    = AddPage (notebook, "Driver Functions");
    ErrorPage     = AddPage (notebook, "Error List");
      
    gtk_widget_show_all (window);
    PopulatePages ();
}

void LabelModify (GtkWidget *, gpointer);

void PopulatePages ()
{
  gint   boards_detected;
  char   buff[96];
  short  sRet = DRV_NO_ERROR;
  BOARD_INFO    tBoardInfo;

  PopulateBoards ();
  PopulateMsgTrans ();
  PopulateExIOEx ();
  PopulateBoardInfo ();
  PopulateDrvInfo ();
  PopulateDrvFun ();
  PopulateErrorList ();

  tHardwareInfo.fInfo = FALSE;
  DEV_INFO_FLAGS = 0x0000; // init each time
  if ( (sRet = DevOpenDriver() ) != DRV_NO_ERROR) {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards);
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, "Device Driver not found!");
  } else {
    DEV_INFO_FLAGS |= DRV_OPEN; 
  }
  if (DEV_INFO_FLAGS & DRV_OPEN) {
    if ( ( sRet = DevGetBoardInfo( &tBoardInfo)) == DRV_NO_ERROR) {
      tHardwareInfo.fInfo = TRUE;
      boards_detected = tBoardInfo.usBoards_detected;
      tHardwareInfo.tBoardInfo.usBoards_detected = tBoardInfo.usBoards_detected;
      g_snprintf(buff, 40, "(boards detected: %d)", boards_detected);
      gtk_label_set_text (GTK_LABEL ( label_boards), buff);
      g_snprintf( pabDrvVersion, sizeof( pabDrvVersion), "%s\n", tBoardInfo.abDriverVersion);
      gtk_entry_set_text ( (GtkEntry *) DrvVerInf, pabDrvVersion);
     }
    else {
      tHardwareInfo.fInfo = FALSE;
      gtk_container_foreach (GTK_CONTAINER ( BPradio_4), 
			    (GtkCallback) LabelModify, GINT_TO_POINTER( 0));
    }
  }
}

GtkWidget *CreateRadio (GtkWidget *box, GSList **group, char *szLabel)
{
    GtkWidget *radio;

    radio = gtk_radio_button_new_with_label (*group, szLabel);

    *group = gtk_radio_button_group (GTK_RADIO_BUTTON (radio));

    gtk_box_pack_start (GTK_BOX (box), radio, FALSE, FALSE, 10);

    gtk_widget_show (radio);

    return (radio);
}

void LabelModify (GtkWidget *child, gpointer data)
{
  unsigned short usIrq = (gint)data;
  char   buff[96];//   *szText;
  
  if (GTK_IS_LABEL (child)) {
    //gtk_label_get (GTK_LABEL (child), &szText);
    /* --- Set the text based on the current text --- */
    //if (!strcmp (szText, "interrupt mode")) {
    if( usIrq == 0)
      gtk_label_set (GTK_LABEL (child), "interrupt mode");
    else {
      g_snprintf( buff, 36, "interrupt mode (irq: %d)", usIrq);
      gtk_label_set (GTK_LABEL (child), buff);
    }
  }
}

void OnSetMode (GtkWidget *widget, gpointer *data)
{
  unsigned short  usMode = (gint)data;
  unsigned short  usIrq  = 0;
  gint  usBoard = 0, Irq;
  short sRet = 0;
  char  buff[96];

  if( DEV_INFO_FLAGS & BOARD_0)
    usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
    usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
    usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
    usBoard = 3;
  else
    usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Msg, "Board not initialized!");
    goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
    g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Msg, buff);
    goto leave;
  } 
  else {
      if ( (sRet = DevSetOpMode( usBoard, usMode, &usIrq)) == DRV_NO_ERROR) {	
	if( usMode == INTERRUPT_MODE) {
	  Irq = usIrq;
	  gtk_container_foreach (GTK_CONTAINER ( BPradio_4), 
				 (GtkCallback) LabelModify, GINT_TO_POINTER( Irq));
	  tHardwareInfo.fIrq  = TRUE;
	  tHardwareInfo.tBoardInfo.tBoard[usBoard].usIrq = Irq;
	  g_snprintf(buff, 50, "Interrupt mode active for Board %d\n", usBoard);
	}
	else {
	  Irq = usIrq;
	  tHardwareInfo.fIrq  = FALSE;
	  gtk_container_foreach (GTK_CONTAINER ( BPradio_4), 
				 (GtkCallback) LabelModify, GINT_TO_POINTER( 0));
	  if( Irq == 0)
	    g_snprintf(buff, 80, "polling mode active for Board %d \n", usBoard);
	  else
	    g_snprintf(buff, 80, "polling mode active for Board %d (IRQ %d desabled!)\n", usBoard,  Irq);
	}
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, buff);
      }
      else {
	g_snprintf(buff, 58, "Error by driver function DevSetOpmode(). (Error: %d)", sRet); 
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_Boards );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_Boards, buff);
      }
  }
 leave: ;
}

void OnBoard (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard = (gint)data;
  unsigned char   abTemp[30];               
  unsigned char   abInfo[512];              
  short           sRet;
  gint            irq = 0;
  char            buff[96];
  DEVINFO        *pDev = NULL;
  BOARD_INFO      tBoardInfo;

  tHardwareInfo.fInfo = FALSE;
  tHardwareInfo.fInit = FALSE;
  tHardwareInfo.fIrq  = TRUE;
  tHardwareInfo.tBoardInfo.tBoard[usBoard].usBoard = usBoard;

  gtk_container_foreach (GTK_CONTAINER ( BPradio_4), (GtkCallback) LabelModify, GINT_TO_POINTER( 0));

  if( DEV_INFO_FLAGS & BOARD_0)
    usBoardAct = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
    usBoardAct = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
    usBoardAct = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
    usBoardAct = 3;
  else
    usBoardAct = 0xFFFF;
  if( usBoardAct != 0xFFFF) {
    if ( (sRet = DevExitBoard(usBoardAct)) != DRV_NO_ERROR) {
      g_snprintf(buff, 56, " Error in DevExitBoard(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards);
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, buff);
    }
  }

  gtk_entry_set_text( GTK_ENTRY(FirmwareName), "");
  gtk_entry_set_text( GTK_ENTRY(FirmwareVer), "");
  gtk_entry_set_text( GTK_ENTRY(Error), "");

  if (DEV_INFO_FLAGS & DRV_OPEN) {
    DEV_INFO_FLAGS = 0x0000; // init each time
    DEV_INFO_FLAGS |= DRV_OPEN; 
    if ( (sRet = DevInitBoard ( usBoard)) != DRV_NO_ERROR) {
      g_snprintf(buff, 60, "Board Initialization failed!  ERROR %d", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards);
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, buff);
      goto leave;
    } else {
      switch(usBoard) {
      case 0:
	DEV_INFO_FLAGS |= BOARD_0;
	usBoardAct = 0;
	break;
      case 1:
	usBoardAct = 1;
	DEV_INFO_FLAGS |= BOARD_1;
	break;
      case 2:
	usBoardAct = 2;
	DEV_INFO_FLAGS |= BOARD_2;
	break;
      case 3:
	usBoardAct = 3;
	DEV_INFO_FLAGS |= BOARD_3;
	break;
      }
      tHardwareInfo.fInit = TRUE;
      DEV_INFO_FLAGS |= DEV_INIT; 
      g_snprintf(buff, 20, "Board Initialized!");
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards);
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, buff);
    } 
  }
  if (DEV_INFO_FLAGS & DEV_INIT) {
    if ( ( sRet = DevGetBoardInfo( &tBoardInfo)) == DRV_NO_ERROR) {
      irq = tBoardInfo.tBoard[usBoard].usIrq;
      gtk_container_foreach (GTK_CONTAINER ( BPradio_4), 
			     (GtkCallback) LabelModify, GINT_TO_POINTER( irq));
      tHardwareInfo.fInfo = TRUE;
    }
    else {
      tHardwareInfo.fInfo = FALSE;
      gtk_container_foreach (GTK_CONTAINER ( BPradio_4), 
			    (GtkCallback) LabelModify, GINT_TO_POINTER( 0));
    }
    if ((sRet = DevGetInfo( usBoard,                          /* DeviceNumber        */
			    GET_FIRMWARE_INFO,                /* InfoArea            */
			    sizeof(FIRMWAREINFO),             /* Size of bytes to read */
			    &abInfo[0])) != DRV_NO_ERROR ) {  /* Pointer to user buffer */
      g_snprintf(buff, 54, "DevGetInfo (GET_FIRMWARE_INFO) failed!  ERROR %d", sRet); 
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards);
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, buff);
      goto leave;
    } else {
      DEV_INFO_FLAGS |= DEV_INFO_FI; 
      memcpy ( &abTemp, &abInfo[0], 16);
      abTemp[16] = 0;

      gtk_entry_set_text( GTK_ENTRY(FirmwareName), &abTemp[0]);
      memcpy ( &abTemp, &abInfo[16], 16);
      abTemp[16] = 0;
      gtk_entry_set_text( GTK_ENTRY(FirmwareVer), &abTemp[0]);
      g_snprintf(buff, 50, " Board-%.1d initialized", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards);
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, buff);
    }
  }

  if (DEV_INFO_FLAGS & DEV_INFO_FI) {
  if ( (sRet = DevGetInfo( usBoard,
			   GET_DEV_INFO,
			   sizeof(DEVINFO),
			   &abInfo[0])) != DRV_NO_ERROR) {
    g_snprintf(buff, 54, "Error by driver function DevGetInfo(). (Error: %d)", sRet); 
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Boards), context_id_Boards);
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Boards), context_id_Boards, buff);
    goto leave;
  } else {
    DEV_INFO_FLAGS |= DEV_INFO; 
    // Load  structure pointer
    pDev = (DEVINFO *)&abInfo[0];
    tHardwareInfo.bDpmSize  = pDev->bDpmSize;
    tHardwareInfo.bDevType  = pDev->bDevType;
    tHardwareInfo.bDevModel = pDev->bDevModel;
    memcpy( &tHardwareInfo.abDevIdentifier[0], &pDev->abDevIdentifier[0], sizeof( pDev->abDevIdentifier) );

    tHardwareInfo.fIrq  = TRUE;
    tHardwareInfo.tBoardInfo.tBoard[usBoard].usIrq = tBoardInfo.tBoard[usBoard].usIrq;
    memcpy( &tHardwareInfo.tBoardInfo.abDriverVersion[0], 
	    &tBoardInfo.abDriverVersion[0], 
	    sizeof( tBoardInfo.abDriverVersion) );
    tHardwareInfo.tBoardInfo.tBoard[usBoard].ulPhysicalAddress = tBoardInfo.tBoard[usBoard].ulPhysicalAddress;
  }
 }
 leave: ;
}

gint PutMsg_( unsigned short  usBoard) //static gint PutMsg_( gpointer *data) 
{
  MSG_STRUC  tMessage;
  short      sRet = DRV_NO_ERROR;
  //  unsigned short  usBoard = (gint)data;
  char *chPutData, buff[96];

  // create message header
  if ( ( CheckEditEntryC( eMsgPutRX, &tMessage.rx, 255, &buff[0]) != TRUE) ||
       ( CheckEditEntryC( eMsgPutTX, &tMessage.tx, 255, &buff[0]) != TRUE) ||
       ( CheckEditEntryC( eMsgPutA , &tMessage.a,  255, &buff[0]) != TRUE) ||
       ( CheckEditEntryC( eMsgPutF , &tMessage.f,  255, &buff[0]) != TRUE) ||
       ( CheckEditEntryC( eMsgPutB , &tMessage.b,  255, &buff[0]) != TRUE) ||
       ( CheckEditEntryC( eMsgPutE , &tMessage.e,  255, &buff[0]) != TRUE) ){
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, buff);
  } else {
    chPutData = gtk_editable_get_chars( GTK_EDITABLE( MsgPutData), (gint) 0, (gint) -1);
    if ( StringToHex( chPutData,
		      sizeof(tMessage.data),
		      (unsigned char*)&tMessage.data[0],
		      &usDataLen,
		      (GtkStatusbar *)status_bar_Msg,
		      context_id_Msg) ) {
      if ( !(MSG_TRANS_FLAGS & AUTO_NUM))
	CheckEditEntryC( eMsgPutNR, &bMessageNr, 255, &buff[0]);
      }
      tMessage.nr = bMessageNr;
      g_snprintf( buff, 16, "%d", bMessageNr);
      gtk_entry_set_text(  GTK_ENTRY( eMsgPutNR), buff);   
      tMessage.ln = (unsigned char)usDataLen;
      g_snprintf( buff, 16, "%d", tMessage.ln);
      gtk_entry_set_text(  GTK_ENTRY( eMsgPutLN), buff);   
      if ( (sRet = DevPutMessage( usBoard, &tMessage, 100L)) != DRV_NO_ERROR) {
	g_snprintf(buff, 64, "Error by driver function DevPutMessage(). (Error: %d)", sRet);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, buff);
      } else {
	if ( MSG_TRANS_FLAGS & AUTO_NUM) {
	  bMessageNr++;
	  //g_snprintf( buff, 16, "%d", bMessageNr);
	  //gtk_entry_set_text(  GTK_ENTRY( eMsgPutNR), buff);   
	}
	if ( MSG_TRANS_FLAGS & PUT_CYCLIC) {
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "   DevPutMessage() running ...");
	}
	else {
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "   DevPutMessage() done !");
	}
      }
  }
  return TRUE;
}

gint GetMsg_( unsigned short usBoard) //static gint GetMsg_( gpointer *data) 
{
  MSG_STRUC  tMsg;
  char       buff[96];
  short      sRet = DRV_NO_ERROR;
  //  unsigned short  usBoard = (gint)data;

  if ( (sRet = DevGetMessage( usBoard, sizeof(tMsg), &tMsg, 1000L)) != DRV_NO_ERROR) {
    g_snprintf(buff, 64, "Error by driver function DevGetMessage(). (Error: %d)", sRet);
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, buff);
  } else {
    g_snprintf( buff, 8, "%d", tMsg.rx); // or HEX "%.2X"
    gtk_entry_set_text( GTK_ENTRY( eMsgGetRX), &buff[0]);
    g_snprintf( buff, 8, "%d", tMsg.tx);
    gtk_entry_set_text( GTK_ENTRY( eMsgGetTX), buff);
    g_snprintf( buff, 8, "%d", tMsg.ln);
    gtk_entry_set_text( GTK_ENTRY( eMsgGetLN), buff);
    g_snprintf( buff, 8, "%d", tMsg.nr);
    gtk_entry_set_text( GTK_ENTRY( eMsgGetNR), buff);
    g_snprintf( buff, 8, "%d", tMsg.a);
    gtk_entry_set_text( GTK_ENTRY( eMsgGetA),  buff);
    g_snprintf( buff, 8, "%d", tMsg.f);
    gtk_entry_set_text( GTK_ENTRY( eMsgGetF),  buff);
    g_snprintf( buff, 8, "%d", tMsg.b);
    gtk_entry_set_text( GTK_ENTRY( eMsgGetB),  buff);
    g_snprintf( buff, 8, "%d", tMsg.e);
    gtk_entry_set_text( GTK_ENTRY( eMsgGetE),  buff);
    if( tMsg.ln > 0) {
      HexToTextBox( (GtkText *)MsgGetData, tMsg.ln, &tMsg.data[0]);
      if(  MSG_TRANS_FLAGS & GET_CYCLIC) {
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "DevGetMessage() running ...");
      }
      else {
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "DevGetMessage() done!");
      }
    }
    //else {
    //gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    //gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "DevGetMessage(): Data buffer is empty!");
    //}
  }
  return TRUE;
}

static gint
Timer_MsgTransfer( gpointer *data) 
{
  unsigned short  usBoard = (gint)data;
  char   buff[96];

  fMsgTimer = TRUE;
  MSG_TRANS_FLAGS |= TIMER_ACTIVE;
  if( ( MSG_TRANS_FLAGS & GET_START) && ( MSG_TRANS_FLAGS & PUT_START))
    g_snprintf( buff, 64, "Message Transfer running ( Get & Put )...");
  else if( MSG_TRANS_FLAGS & GET_START)
    g_snprintf( buff, 64, "Message Transfer running ( Get )...");
  else if( MSG_TRANS_FLAGS & PUT_START)
    g_snprintf( buff, 64, "Message Transfer running ( Put )...");
  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
  gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, buff);
  if( MSG_TRANS_FLAGS & GET_START)
    GetMsg_ ( usBoard);
  if( MSG_TRANS_FLAGS & PUT_START)
    PutMsg_ ( usBoard);
  return TRUE;
}

void GetCyclic (GtkWidget *widget, gpointer *data)
{
  if ( MSG_TRANS_FLAGS & GET_CYCLIC) {
    MSG_TRANS_FLAGS &= ~GET_CYCLIC;
    if( MSG_TRANS_FLAGS & GET_START) {
      MSG_TRANS_FLAGS &= ~GET_START;
      if( !(MSG_TRANS_FLAGS & PUT_START)) {
	gtk_timeout_remove( pMsgTimer);
	fMsgTimer = FALSE;
	MSG_TRANS_FLAGS &= ~TIMER_ACTIVE;
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "Msg Transfer halted!");
      }
    }
  }
  else 
    MSG_TRANS_FLAGS |= GET_CYCLIC;
}

void PutCyclic (GtkWidget *widget, gpointer *data)
{
  if ( MSG_TRANS_FLAGS & PUT_CYCLIC) {
    MSG_TRANS_FLAGS &= ~PUT_CYCLIC;
    if( MSG_TRANS_FLAGS & PUT_START) {
      MSG_TRANS_FLAGS &= ~PUT_START;
      if( !(MSG_TRANS_FLAGS & GET_START)) {
	gtk_timeout_remove( pMsgTimer);
	fMsgTimer = FALSE;
	MSG_TRANS_FLAGS &= ~TIMER_ACTIVE;
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "Msg Transfer halted!");
      }
    }
  }
  else
    MSG_TRANS_FLAGS |= PUT_CYCLIC;
}

void GetMsg (GtkWidget *widget, gpointer *data)
{
  gint  usBoard = 0;
  char  buff[96];
  if( DEV_INFO_FLAGS & BOARD_0)
    usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
    usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
    usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
    usBoard = 3;
  else
    usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "No Board selected");
    goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
    g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, buff);
    goto leave;
  } 
  else {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "");
    if ( (MSG_TRANS_FLAGS & GET_CYCLIC) && !(MSG_TRANS_FLAGS & GET_START) ) {
      MSG_TRANS_FLAGS |= GET_START;
      //      pMsgGetTimer = gtk_timeout_add( 50, (GtkFunction)GetMsg_, GINT_TO_POINTER(usBoard));
      if( !(MSG_TRANS_FLAGS & TIMER_ACTIVE))
	pMsgTimer = gtk_timeout_add( 10, (GtkFunction)Timer_MsgTransfer, GINT_TO_POINTER(usBoard));
    }
    else
      GetMsg_( usBoard);//GetMsg_( GINT_TO_POINTER(usBoard));
  }
 leave: ;
}

void PutMsg (GtkWidget *widget, gpointer *data)
{
  gint  usBoard = 0;
  char  buff[96];
  usDataLen  = 0;
  ///bMessageNr = 0;
  if( DEV_INFO_FLAGS & BOARD_0)
    usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
    usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
    usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
    usBoard = 3;
  else
    usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "No Board selected");
    goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
    g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, buff);
    goto leave;
  } 
  else {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_Msg), context_id_Msg );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_Msg), context_id_Msg, "");
    if ( (MSG_TRANS_FLAGS & PUT_CYCLIC) && !(MSG_TRANS_FLAGS & PUT_START) ) {
      MSG_TRANS_FLAGS |= PUT_START;
      //      pMsgPutTimer = gtk_timeout_add( 50, (GtkFunction)PutMsg_, GINT_TO_POINTER(usBoard));
      if( !(MSG_TRANS_FLAGS & TIMER_ACTIVE))
	pMsgTimer = gtk_timeout_add( 10, (GtkFunction)Timer_MsgTransfer, GINT_TO_POINTER(usBoard));
    }
    else
      PutMsg_( usBoard);//PutMsg_( GINT_TO_POINTER(usBoard));
  }
 leave: ;
}

void AutoNum (GtkWidget *widget, gpointer *data)
{
  if ( MSG_TRANS_FLAGS & AUTO_NUM)
    MSG_TRANS_FLAGS &= ~AUTO_NUM;
  else
    MSG_TRANS_FLAGS |= AUTO_NUM;
}

void PopulateBoards ()
{
    GtkWidget *vbox, *vboxx, *vboxI, *hbox, *hboxfr;
    GtkWidget *frame;
    GSList    *group = NULL, *group1 = NULL;
    GtkWidget *widget;

    vbox = gtk_vbox_new (FALSE, 0);
    label_boards = gtk_label_new("(boards detected: )");
    frame = gtk_frame_new("Select Board");
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 4);
    hbox = gtk_hbox_new (TRUE, 1);
    gtk_container_add (GTK_CONTAINER (frame), hbox);
    BPradio_0 = CreateRadio (hbox, &group, "Board 0");
    gtk_signal_connect (GTK_OBJECT (BPradio_0), "released",//"clicked",
			GTK_SIGNAL_FUNC (OnBoard), GINT_TO_POINTER(0));
    BPradio_1 = CreateRadio (hbox, &group, "Board 1");
    gtk_signal_connect (GTK_OBJECT (BPradio_1), "released",//"clicked",
			GTK_SIGNAL_FUNC (OnBoard), GINT_TO_POINTER(1));
    BPradio_2 = CreateRadio (hbox, &group, "Board 2");
    gtk_signal_connect (GTK_OBJECT (BPradio_2), "released",//"clicked",
			GTK_SIGNAL_FUNC (OnBoard), GINT_TO_POINTER(2));
    BPradio_3 = CreateRadio (hbox, &group, "Board 3");
    gtk_signal_connect (GTK_OBJECT (BPradio_3), "released",//"clicked",
			GTK_SIGNAL_FUNC (OnBoard), GINT_TO_POINTER(3));
    gtk_box_pack_start(GTK_BOX(hbox), label_boards, FALSE, FALSE, 2);

    frame = gtk_frame_new("Set board operation mode");
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 4);
    hbox = gtk_hbox_new (TRUE, 2);
    gtk_container_add (GTK_CONTAINER (frame), hbox);
    BPradio_4 = CreateRadio (hbox, &group1, "interrupt mode");
    gtk_signal_connect (GTK_OBJECT (BPradio_4), "released",
			GTK_SIGNAL_FUNC (OnSetMode), GINT_TO_POINTER( INTERRUPT_MODE));
    BPradio_5 = CreateRadio (hbox, &group1, "polling mode");
    gtk_signal_connect (GTK_OBJECT (BPradio_5), "released",
			GTK_SIGNAL_FUNC (OnSetMode), GINT_TO_POINTER( POLLING_MODE));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON( BPradio_4), TRUE);

    frame = gtk_frame_new("Firmware info");

    hboxfr = gtk_hbox_new (FALSE, 0);
    vboxx = gtk_vbox_new  (FALSE, 0);
    vboxI = gtk_vbox_new  (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (frame),  hboxfr);
    gtk_container_add (GTK_CONTAINER (hboxfr), vboxx);
    gtk_container_add (GTK_CONTAINER (hboxfr), vboxI);
    hbox = gtk_hbox_new (FALSE, 0);

    widget = CreateEditField ("Firmware Name: ");
    gtk_widget_set_usize( widget, 100, 10);
    gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE, 4);
    FirmwareName = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY ( FirmwareName), "");
    gtk_entry_set_editable(GTK_ENTRY( FirmwareName), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox), FirmwareName, FALSE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, FALSE, 0);

    hbox = gtk_hbox_new (FALSE, 0);
    widget = CreateEditField ("Firmware version: ");
    gtk_widget_set_usize( widget, 100, 10);
    gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE, 4);
    FirmwareVer = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY ( FirmwareVer), "");
    gtk_entry_set_editable(GTK_ENTRY( FirmwareVer), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox), FirmwareVer, FALSE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (vboxx), hbox, TRUE, TRUE, 0);

    /* right part of frame */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vboxI), hbox, FALSE, FALSE, 0);
    widget = CreateEditField ("Error: ");
    gtk_widget_set_usize( widget, 100, 10);
    gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE, 4);
    Error = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY ( Error), "");
    gtk_entry_set_editable(GTK_ENTRY( Error), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox), Error, FALSE, TRUE, 4);

    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 6);

    frame = gtk_frame_new("Driver Version Info");
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
    hbox = gtk_vbox_new (TRUE, 2);
    DrvVerInf = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY ( DrvVerInf), "CIFDev ...");
    gtk_entry_set_editable(GTK_ENTRY( DrvVerInf), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox), DrvVerInf, FALSE, TRUE, 4);
    gtk_container_add (GTK_CONTAINER (frame), hbox);

    status_bar_Boards = gtk_statusbar_new ();      
    gtk_box_pack_start (GTK_BOX (vbox), status_bar_Boards, FALSE, FALSE, 0);

    context_id_Boards = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar_Boards), "Boards");
    gtk_container_add (GTK_CONTAINER (BoardsPage), vbox);
    gtk_widget_show_all (vbox);
}

static gint
ExchangeIO_Snd ( gpointer *data)
{
           short  sRet = DRV_NO_ERROR;
  unsigned short  usSendOffset, usSendLength, usDataLen;
  unsigned short  usBoard = 0;
  unsigned char   abSendIOData[512];
           char   buff[96], *chSnd;  
  IOINFO   tIoInfo;

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
      goto leave;
  }
  else {
    sRet = DevGetInfo( usBoard, 
		       GET_IO_INFO,
		       sizeof(tIoInfo),
		       &tIoInfo);
    if ( tIoInfo.bComBit == 0) {
     gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
     gtk_statusbar_push(GTK_STATUSBAR(status_bar_ExIO),context_id_ExIO,"Device state: No Data Exchange   ");
    } else {
     gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
     gtk_statusbar_push(GTK_STATUSBAR(status_bar_ExIO),context_id_ExIO,"Device state: Data Exchange   ");
    }
    if ( ( CheckEditEntry( ExIOffSnd , &usSendOffset, sizeof(abSendIOData), &buff[0]) != TRUE) ||
	 ( CheckEditEntry( ExIOLenSnd, &usSendLength, sizeof(abSendIOData), &buff[0]) != TRUE)  ) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
      gtk_statusbar_push(GTK_STATUSBAR(status_bar_ExIO),context_id_ExIO, buff);
      goto leave;
    } else {
      chSnd = gtk_editable_get_chars( GTK_EDITABLE( ExIOSnd), (gint) 0, (gint) -1);
      if ( StringToHex(	chSnd,
			sizeof(abSendIOData),
			&abSendIOData[0],
			&usDataLen,
			(GtkStatusbar *)status_bar_ExIO,
			context_id_ExIO) ) {
	switch( usExIOfun) {
	case 1:
	  if ( (sRet = DevExchangeIO( usBoard, 
				      usSendOffset,
				      usSendLength,
				      &abSendIOData[0],
				      0,
				      0,
				      NULL,
				      100L)) != DRV_NO_ERROR) {
	    g_snprintf(buff, 58, "Error by driver function DevExchangeIO(). (Error: %d)", sRet); 
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
	  } else {
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, "  Data exchange ...");
	  }
	  break;
	case 2:
	  if ( (sRet = DevExchangeIOEx( usBoard, 
					usIOExchangeMode,
					usSendOffset,
					usSendLength,
					&abSendIOData[0],
					0,
					0,
					NULL,
					100L)) != DRV_NO_ERROR) {
	    g_snprintf(buff, 58, "Error by driver function DevExchangeIOEx(). (Error: %d)", sRet); 
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
	  } else {
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, "  Data exchange ...");
	  }
	  break;
	case 3:
	  if ( (sRet = DevExchangeIOErr( usBoard, 
					 usSendOffset,
					 usSendLength,
					 &abSendIOData[0],
					 0,
					 0,
					 NULL,
					 &tComState,
					 100L)) != DRV_NO_ERROR) {
	    g_snprintf(buff, 58, "Error by driver function DevExchangeIOErr(). (Error: %d)", sRet); 
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
	  } else {
	    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	    gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, "  Data exchange ...");
	  }
	  break;
	default:
	  break;
	} /* end switch */
      }
      else if ( fExIOSndTimer) {
	gtk_timeout_remove ( pExIOSndTimer);
	fExIOSndTimer = FALSE;
      }
    }
  }
 leave:   return TRUE;
}

void 
ExchangeIO_Snd_ (GtkWidget *widget, gpointer *data)
{
  if ( (EXIO_FLAGS & SND_CYCLIC) && !fExIOSndTimer ) {
    fExIOSndTimer = TRUE;
    pExIOSndTimer = gtk_timeout_add( 100, (GtkFunction)ExchangeIO_Snd, data);
  }
  else
    ExchangeIO_Snd ( data);
}

static gint
ExchangeIO_Rcv ( gpointer *data)
{
           short  sRet = DRV_NO_ERROR;
  unsigned short  usBoard = 0;
  unsigned short  usReadOffset, usReadSize;
  unsigned char   abIOReadData[512];
           char   buff[96];  
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
      goto leave;
  } 
  if ( ( CheckEditEntry( ExIOffRcv , &usReadOffset, sizeof(abIOReadData), &buff[0]) != TRUE) ||
       ( CheckEditEntry( ExIOLenRcv, &usReadSize, sizeof(abIOReadData), &buff[0]) != TRUE)  ) {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
    gtk_statusbar_push(GTK_STATUSBAR(status_bar_ExIO),context_id_ExIO, buff);
    goto leave;
  } else {
    switch( usExIOfun) {
    case 1:
      if ( (sRet = DevExchangeIO( usBoard, 
				  0,
				  0,
				  NULL,
				  usReadOffset,
				  usReadSize,
				  &abIOReadData[0],
				  100L)) == DRV_NO_ERROR) {
	HexToTextBox( (GtkText *)ExIORcv, 
		      usReadSize,
		      &abIOReadData[0]);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, " Data exchange ...");
      }
      else {
	g_snprintf(buff, 58, "Error by driver function DevExchangeIO(). (Error: %d)", sRet); 
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
      }
      break;
    case 2:
      if ( (sRet = DevExchangeIOEx( usBoard, 
				    usIOExchangeMode,
				    0,
				    0,
				    NULL,
				    usReadOffset,
				    usReadSize,
				    &abIOReadData[0],
				    100L)) == DRV_NO_ERROR) {
	HexToTextBox( (GtkText *)ExIORcv, 
		      usReadSize,
		      &abIOReadData[0]);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, " Data exchange ...");
      }
      else {
	g_snprintf(buff, 58, "Error by driver function DevExchangeIOEx(). (Error: %d)", sRet); 
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
      }
      break;
    case 3:
      if ( (sRet = DevExchangeIOErr( usBoard, 
				     0,
				     0,
				     NULL,
				     usReadOffset,
				     usReadSize,
				     &abIOReadData[0],
				     &tComState,
				     100L)) == DRV_NO_ERROR) {
	if (fCheckComState == TRUE) {
	  // Show COM mode
	  g_snprintf( buff, 12, "%d", tComState.usMode);
	  gtk_entry_set_text( (GtkEntry *)ExIO_COMerrMode, buff);

	  // Test DriverType (LOW nibble)
	  switch ( tComState.usMode) {
	  case STATE_MODE_3:
	    // Cyclic transfer of the COM StateFlag and state field
	    g_snprintf( buff, 12, "%d", tComState.usStateFlag);
	    gtk_entry_set_text( (GtkEntry *)ExIO_COMerrState, buff);
	    if ( tComState.usStateFlag != 0) {
	      // Show actual data
	      HexToTextBox( (GtkText *)ExIORcv, 
			    sizeof(tComState.abState),
			    &tComState.abState[0] );
	    } else { // Initialize list box
	      HexToTextBox( (GtkText *)ExIORcv, 0, "");
	    }
	    break;
	  case STATE_MODE_4: 
	    // States available if error flag is set
	    if ( tComState.usStateFlag != 0) {
	      // Show COM state flag
	      g_snprintf( buff, 12, "%d", tComState.usStateFlag);
	      gtk_entry_set_text( (GtkEntry *)ExIO_COMerrState, buff);
	      // Show actual data
	      HexToTextBox( (GtkText *)ExIORcv, 
			    sizeof(tComState.abState),
			    &tComState.abState[0] );
	    }
	    break;	    
	  default:
	    // Mode is unknown, read state information by yourself
	    // Show COM state flag
	    g_snprintf( buff, 12, "%d", tComState.usStateFlag);
	    gtk_entry_set_text( (GtkEntry *)ExIO_COMerrState, buff);
	    break;
	  }
	} else {
	  HexToTextBox( (GtkText *)ExIORcv, 
			usReadSize,
			&abIOReadData[0]);
	  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	  gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, " Data exchange ...");
	} 
      } else {
	g_snprintf(buff, 58, "Error by driver function DevExchangeIOErr(). (Error: %d)", sRet); 
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_ExIO), context_id_ExIO, buff);
      }
      break;
    } /* end switch */
  }
 leave: return TRUE;
}

void 
ExchangeIO_Rcv_ (GtkWidget *widget, gpointer *data)
{
  if ( EXIO_FLAGS & RCV_CYCLIC) {
    fExIORcvTimer = TRUE;
    pExIORcvTimer = gtk_timeout_add( 100, (GtkFunction)ExchangeIO_Rcv, data);
  }
  else
    ExchangeIO_Rcv ( data);
}

void 
ExIORcvCyclic (GtkWidget *widget, gpointer *data)
{
  if ( EXIO_FLAGS & RCV_CYCLIC) {
    EXIO_FLAGS &= ~RCV_CYCLIC;
    if ( fExIORcvTimer) {
      gtk_timeout_remove( pExIORcvTimer);
      fExIORcvTimer = FALSE;
    }
  }
  else
    EXIO_FLAGS |= RCV_CYCLIC;
}

void 
ExIOSndCyclic (GtkWidget *widget, gpointer *data)
{
  if ( EXIO_FLAGS & SND_CYCLIC) {
    EXIO_FLAGS &= ~SND_CYCLIC;
    if ( fExIOSndTimer) {
      gtk_timeout_remove( pExIOSndTimer);
      fExIOSndTimer = FALSE;
    }
  }
  else
    EXIO_FLAGS |= SND_CYCLIC;
}

void PopulateErrorList ()
{
  GtkWidget   *main_vbox;
  GtkWidget   *table;
  GtkWidget   *vscrollbar;
  GdkFont     *fixed_font;
  GdkColormap *cmap;
  GdkColor    color;

  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER ( ErrorPage), main_vbox);
  
  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
  gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
  gtk_box_pack_start (GTK_BOX (main_vbox), table, TRUE, TRUE, 0);
  gtk_widget_show (table);

  ErrorList = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(ErrorList), FALSE);
  gtk_table_attach (GTK_TABLE (table), ErrorList, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (ErrorList);

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (ErrorList)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show (vscrollbar);

  /* Get the system color map and allocate the color red */
  cmap = gdk_colormap_get_system();
  color.red = 0xffaf;
  color.green = 0;
  color.blue = 0;
  if (!gdk_color_alloc(cmap, &color)) {
    g_error("couldn't allocate color");
  }

  /* Load a fixed font */
  fixed_font = gdk_font_load ("-misc-fixed-medium-r-*-*-*-110-*-*-*-*-*-*");
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &ErrorList->style->black, NULL,           
		   " 0\tno error\n\n", -1);
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		    "-1\tDRIVER Board not initialized\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-2\tDRIVER Error in internal init state\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-3\tDRIVER Error in internal read state\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-4\tDRIVER Command on this channel is activ\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-5\tDRIVER Unknown parameter in function occured\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-6\tDRIVER Version is incompatible with DLL\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-7\tDRIVER Error during PCI set run mode\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-8\tDRIVER Could not read PCI dual port memory length\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-9\tDRIVER Error during PCI set run mode\n\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-10\tDEVICE Dual port ram not accessable(board not found)\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-11\tDEVICE Not ready (ready flag failed)\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-12\tDEVICE Not running (running flag failed)\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-13\tDEVICE Watchdog test failed\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-14\tDEVICE Signals wrong OS version\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-15\tDEVICE Error in dual port flags\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-16\tDEVICE Send mailbox is full\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-17\tDEVICE PutMessage timeout\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-18\tDEVICE GetMessage timeout\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-19\tDEVICE No message available\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-20\tDEVICE RESET command timeout\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-21\tDEVICE COM-flag not set\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-22\tDEVICE IO data exchange failed\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-23\tDEVICE IO data exchange timeout\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-24\tDEVICE IO data mode unknown\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-25\tDEVICE Function call failed\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-26\tDEVICE DPM size differs from configuration\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-27\tDEVICE State mode unknown\n\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-30\tUSER Driver not opened (device driver not loaded)\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-31\tUSER Can't connect with device\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-32\tUSER Board not initialized (DevInitBoard not called)\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-33\tUSER IOCTRL function failed\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-34\tUSER Parameter DeviceNumber invalid\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-35\tUSER Parameter InfoArea unknown\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-36\tUSER Parameter Number invalid\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-37\tUSER Parameter Mode invalid\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-38\tUSER NULL pointer assignment\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-39\tUSER Message buffer too short\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-40\tUSER Parameter Size invalid\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-42\tUSER Parameter Size with zero length\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-43\tUSER Parameter Size too long\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-44\tUSER Device address null pointer\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-45\tUSER Pointer to buffer is a null pointer\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-46\tUSER Parameter SendSize too long\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-47\tUSER Parameter ReceiveSize too long\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-48\tUSER Pointer to send buffer is a null pointer\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-49\tUSER Pointer to receive buffer is a null pointer\n\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-60\tDEVICE Virtual memory not available\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-61\tDEVICE Unmap virtual memory failed\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-62\tDEVICE Request irq failed\n\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-100\tUSER file not opend\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-101\tUSER file size zero\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-102\tUSER not enough memory to load file\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-103\tUSER file read failed\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-104\tUSER file type invalid\n", 
		   -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "-105\tUSER file name not valid\n\n", -1);            

  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1000\tRCS error number start\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1020\tRCS database not valid for this device\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1021\tRCS data base segment not configured or not existent\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1022\tRCS number of message wrong\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1023\tRCS number of data mismatch\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1024\tRCS wrong sequence identifier\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1025\tRCS checksum mismatch\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1038\tRCS data base is not compatible to the firmware\n", -1);            
  gtk_text_insert( (GtkText *)ErrorList, fixed_font, &color, NULL, 
		   "1044\tRCS message length mismatch\n", -1);            
  gtk_text_set_point( (GtkText *)ErrorList, 1);
  /* Thaw the text widget, allowing the updates to become visible */  
  gtk_text_thaw (GTK_TEXT (ErrorList));

  gtk_widget_show_all(ErrorPage);
}

void PopulateExIO ()
{
  GtkWidget *main_vbox, *vboxL, *vbbox;
  GtkWidget *hbox, *hboxR, *hboxL , *hboxXY;
  GtkWidget *frame;
  GtkWidget *label;
  GtkWidget *snd, *cyclic;

  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER ( ExIOPage), main_vbox);
  
  hboxXY = gtk_hbox_new(TRUE, 6);

  hbox  = gtk_hbox_new(FALSE, 6);
  hboxL = gtk_hbox_new(FALSE, 0);
  hboxR = gtk_hbox_new(FALSE, 0);
  vboxL = gtk_vbox_new(FALSE, 10);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxL, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxR, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vboxL), hbox, FALSE, FALSE, 0);

  frame = gtk_frame_new("Receive Process Data");
  gtk_container_add( GTK_CONTAINER(frame),  vboxL);

  label = gtk_label_new("Offset");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxL), label, FALSE, FALSE, 0);
  ExIOffRcv = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOffRcv, 60, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIOffRcv), "0");
  gtk_box_pack_start (GTK_BOX (hboxL), ExIOffRcv, FALSE, FALSE, 0);
  label = gtk_label_new("Length");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxR), label, FALSE, FALSE, 0);
  ExIOLenRcv = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOLenRcv, 60, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIOLenRcv), "16");
  gtk_box_pack_start (GTK_BOX (hboxR), ExIOLenRcv, FALSE, FALSE, 0);
  vbbox = gtk_vbox_new(FALSE, 0);

  cyclic = gtk_toggle_button_new_with_label("cyclic");
  gtk_container_add( GTK_CONTAINER(vbbox), cyclic);
  gtk_signal_connect (GTK_OBJECT (cyclic), "toggled", 
		      GTK_SIGNAL_FUNC (ExIORcvCyclic),NULL);
  snd   = gtk_button_new_with_label("get");
  gtk_signal_connect (GTK_OBJECT (snd), "released", 
		      GTK_SIGNAL_FUNC (ExchangeIO_Rcv_),NULL);
  gtk_box_pack_start (GTK_BOX(vbbox),  snd, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox),  vbbox, FALSE, FALSE, 0);

  ExIORcv = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(ExIORcv), FALSE);
  gtk_container_add (GTK_CONTAINER (vboxL), ExIORcv);
  gtk_container_add (GTK_CONTAINER (hboxXY), frame);
  gtk_container_add (GTK_CONTAINER (main_vbox/*ExIOPage*/), hboxXY);

  hbox  = gtk_hbox_new(FALSE, 15);
  hboxL = gtk_hbox_new(FALSE, 0);
  hboxR = gtk_hbox_new(FALSE, 0);
  vboxL = gtk_vbox_new(FALSE, 10);
  vbbox = gtk_vbox_new(FALSE, 0);

  cyclic = gtk_toggle_button_new_with_label("cyclic");
  gtk_container_add( GTK_CONTAINER(vbbox), cyclic);
  gtk_signal_connect (GTK_OBJECT (cyclic), "toggled", 
		      GTK_SIGNAL_FUNC (ExIOSndCyclic),NULL);

  snd = gtk_button_new_with_label("send");
  gtk_signal_connect (GTK_OBJECT (snd), "released", 
		      GTK_SIGNAL_FUNC (ExchangeIO_Snd_),NULL);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxL, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxR, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbbox),  snd, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vboxL), hbox,  FALSE, FALSE, 0);

  frame = gtk_frame_new("Send Process Data");
  gtk_container_add( GTK_CONTAINER(frame),  vboxL);

  label = gtk_label_new("Offset");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxL), label, FALSE, FALSE, 0);
  ExIOffSnd = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOffSnd, 60, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIOffSnd), "0");
  gtk_box_pack_start (GTK_BOX (hboxL), ExIOffSnd, FALSE, FALSE, 0);
  label = gtk_label_new("Length");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxR), label, FALSE, FALSE, 0);
  ExIOLenSnd = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOLenSnd, 60, 20);
  gtk_entry_set_text (GTK_ENTRY (ExIOLenSnd), "16");
  gtk_box_pack_start (GTK_BOX (hboxR), ExIOLenSnd, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox), vbbox, FALSE, FALSE, 0);

  ExIOSnd = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(ExIOSnd), TRUE);
  gtk_container_add (GTK_CONTAINER (vboxL), ExIOSnd);
  gtk_container_add (GTK_CONTAINER (hboxXY), frame);

  status_bar_ExIO = gtk_statusbar_new ();      
  gtk_box_pack_start (GTK_BOX (main_vbox), status_bar_ExIO, TRUE, TRUE, 0);
  context_id_ExIO = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar_ExIO), "ExchangeIO");

  gtk_widget_show_all(ExIOPage);
}

void 
ExchangeIOvar (GtkWidget *widget, gpointer *data)
{
  PopulateExIO();
}

void combo_ExIO (GtkWidget *widget, gpointer *data)
{
   usExIOfun = (gint)data;
   switch(usExIOfun) {
   case 1:
     gtk_label_set_text( (GtkLabel *)heading, "I/O data transfer");
     gtk_widget_hide( (GtkWidget *)boxCOM);
     gtk_widget_hide( (GtkWidget *)ExIO_COMerrMode);
     gtk_widget_hide( (GtkWidget *)ExIO_COMerrState);
     gtk_widget_hide( (GtkWidget *)ExIO_COMeMode);
     gtk_widget_hide( (GtkWidget *)ExIO_COMeState);
     gtk_widget_hide( (GtkWidget *)handshakeMode);
     gtk_widget_hide( (GtkWidget *)checkCOMerr);
     gtk_widget_hide( (GtkWidget *)frameCOM);
     break;
   case 2:
     gtk_label_set_text( (GtkLabel *)heading, "I/O data transfer for COM modules");
     gtk_widget_show( (GtkWidget *)handshakeMode);
     gtk_widget_show( (GtkWidget *)boxCOM);
     gtk_widget_show( (GtkWidget *)frameCOM);
     gtk_widget_hide( (GtkWidget *)ExIO_COMerrMode);
     gtk_widget_hide( (GtkWidget *)ExIO_COMerrState);
     gtk_widget_hide( (GtkWidget *)ExIO_COMeMode);
     gtk_widget_hide( (GtkWidget *)ExIO_COMeState);
     gtk_widget_hide( (GtkWidget *)checkCOMerr);
     gtk_widget_hide( (GtkWidget *)ExIO_hbox3);
     gtk_widget_show( (GtkWidget *)ExIO_hbox32);
     break;
   case 3:
     gtk_label_set_text( (GtkLabel *)heading, "I/O data transfer with COM error");
     gtk_widget_show( (GtkWidget *)ExIO_COMerrState);
     gtk_widget_show( (GtkWidget *)ExIO_COMerrMode);
     gtk_widget_show( (GtkWidget *)ExIO_COMeMode);
     gtk_widget_show( (GtkWidget *)ExIO_COMeState);
     gtk_widget_show( (GtkWidget *)checkCOMerr);
     gtk_widget_show( (GtkWidget *)frameCOM);
     gtk_widget_hide( (GtkWidget *)handshakeMode);
     gtk_widget_hide( (GtkWidget *)boxCOM);
     gtk_widget_hide( (GtkWidget *)ExIO_hbox32);
     gtk_widget_show( (GtkWidget *)ExIO_hbox3);
     break;
   default:
     break;
   }
}

void combo_ExIOsync (GtkWidget *widget, gpointer *data)
{
   usIOExchangeMode = (gint)data;
}

GtkWidget *CrMenuComboExIO_COM ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();
  
  menuitem = make_menu_item ("No synchron", GTK_SIGNAL_FUNC (combo_ExIOsync), 
			     GINT_TO_POINTER( 1));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("Synchron device controlled", GTK_SIGNAL_FUNC (combo_ExIOsync),
			     GINT_TO_POINTER( 2));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("Asynchron device controlled", GTK_SIGNAL_FUNC (combo_ExIOsync),
			     GINT_TO_POINTER( 3));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("Synchron HOST controlled", GTK_SIGNAL_FUNC (combo_ExIOsync),
			     GINT_TO_POINTER( 4));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("Asynchron HOST controlled", GTK_SIGNAL_FUNC (combo_ExIOsync),
			     GINT_TO_POINTER( 5));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);

  return (omenu);
}

GtkWidget *CrMenuComboExIO ()
{
  GtkWidget *omenu;
  GtkWidget *menu;
  GtkWidget *menuitem;
  
  omenu = gtk_option_menu_new();
  menu = gtk_menu_new();
  menuitem = make_menu_item ("DevExchangeIO()", GTK_SIGNAL_FUNC (combo_ExIO), 
			     GINT_TO_POINTER( 1));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("DevExchangeIOEx()", GTK_SIGNAL_FUNC (combo_ExIO),
			     GINT_TO_POINTER( 2));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  menuitem = make_menu_item ("DevExchangeIOErr()", GTK_SIGNAL_FUNC (combo_ExIO),
			     GINT_TO_POINTER( 3));
  gtk_menu_append (GTK_MENU (menu), menuitem);
  gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
  gtk_widget_show (omenu);
  
  return (omenu);
}

void 
showCOMerr (GtkWidget *widget, gpointer *data)
{
  memset( &tComState, 0, sizeof(tComState));

  if( GTK_TOGGLE_BUTTON( checkCOMerr)->active) {
    fCheckComState = TRUE;
  } else {
    fCheckComState = FALSE;
  }
}

void PopulateExIOEx ()
{
  GtkWidget *main_vbox, *vboxL, *vbbox, *hbox, *hboxR, *hboxL , *hboxXY, *vbox_;
  GtkWidget *frame, *label, *hbox31, *boxfunc, *snd, *cyclic;

  main_vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER ( ExIOPage), main_vbox);
  
  hboxXY = gtk_hbox_new(FALSE, 6);

  hbox  = gtk_hbox_new(FALSE, 5);
  hboxL = gtk_hbox_new(FALSE, 0);
  hboxR = gtk_hbox_new(FALSE, 0);
  vboxL = gtk_vbox_new(FALSE, 4);
  vbox_ = gtk_vbox_new(TRUE,  4);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxL, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxR, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vboxL), hbox, FALSE, FALSE, 0);

  ExIO_hbox3  = gtk_hbox_new(FALSE, 12);
  hbox31 = gtk_hbox_new(FALSE, 5);
  ExIO_hbox32 = gtk_hbox_new(FALSE, 5);
  boxfunc = CrMenuComboExIO ();
  heading = gtk_label_new("I/O Data transfer");// "... for COM-Modules", "... with COM error"
  gtk_widget_set_usize( heading, 200, 16);
  gtk_box_pack_start (GTK_BOX( hbox31), boxfunc, FALSE, FALSE, 6);
  gtk_box_pack_start (GTK_BOX( hbox31), heading, FALSE, FALSE, 6);
  gtk_box_pack_start (GTK_BOX( main_vbox), hbox31, FALSE, FALSE, 0);
  frameCOM = gtk_frame_new("COM part");
  gtk_container_add (GTK_CONTAINER (main_vbox), frameCOM);

  checkCOMerr = gtk_check_button_new_with_label("show COM state error");
  gtk_widget_set_usize( checkCOMerr, 160, 60);
  gtk_signal_connect (GTK_OBJECT(checkCOMerr), "toggled",
                       GTK_SIGNAL_FUNC( showCOMerr), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkCOMerr), TRUE);

  ExIO_COMeMode = gtk_label_new("COM error mode:");
  gtk_widget_set_usize( ExIO_COMeMode, 100, 20);
  gtk_box_pack_start (GTK_BOX (ExIO_hbox3), ExIO_COMeMode, FALSE, FALSE, 0);
  ExIO_COMerrMode = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize( ExIO_COMerrMode, 60, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIO_COMerrMode), "0");
  gtk_box_pack_start (GTK_BOX (ExIO_hbox3), ExIO_COMerrMode, FALSE, FALSE, 0);
  ExIO_COMeState = gtk_label_new("COM error state:");
  gtk_widget_set_usize( ExIO_COMeState, 100, 20);
  ExIO_COMerrState = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize( ExIO_COMerrState, 60, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIO_COMerrState), "0");

  gtk_box_pack_start (GTK_BOX (ExIO_hbox3), ExIO_COMeState, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (ExIO_hbox3), ExIO_COMerrState, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (ExIO_hbox3), checkCOMerr, FALSE, FALSE, 0);
  boxCOM  = CrMenuComboExIO_COM ();
  handshakeMode = gtk_label_new("Handshake mode:");
  gtk_widget_set_usize( handshakeMode, 100, 60);
  gtk_box_pack_start (GTK_BOX (ExIO_hbox32), handshakeMode, FALSE, TRUE, 4);
  gtk_box_pack_start (GTK_BOX (ExIO_hbox32),  boxCOM, FALSE, TRUE, 4);
  gtk_container_add (GTK_CONTAINER (vbox_), ExIO_hbox32);//boxCOM);
  gtk_container_add (GTK_CONTAINER (vbox_), ExIO_hbox3);
  gtk_container_add (GTK_CONTAINER (frameCOM), vbox_);

  frame = gtk_frame_new("Receive Process Data");
  gtk_container_add( GTK_CONTAINER(frame),  vboxL);

  label = gtk_label_new("Offset");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxL), label, FALSE, FALSE, 0);
  ExIOffRcv = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOffRcv, 50, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIOffRcv), "0");
  gtk_box_pack_start (GTK_BOX (hboxL), ExIOffRcv, FALSE, FALSE, 0);
  label = gtk_label_new("Length");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxR), label, FALSE, FALSE, 0);
  ExIOLenRcv = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOLenRcv, 50, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIOLenRcv), "16");
  gtk_box_pack_start (GTK_BOX (hboxR), ExIOLenRcv, FALSE, FALSE, 0);
  vbbox = gtk_hbox_new(FALSE, 0);

  cyclic = gtk_toggle_button_new_with_label("cyclic");
  gtk_widget_set_usize( cyclic, 40, 20);
  gtk_container_add( GTK_CONTAINER(vbbox), cyclic);
  gtk_signal_connect (GTK_OBJECT (cyclic), "toggled", 
		      GTK_SIGNAL_FUNC (ExIORcvCyclic),NULL);
  snd   = gtk_button_new_with_label("get");
  gtk_widget_set_usize( snd, 40, 20);
  gtk_signal_connect (GTK_OBJECT (snd), "released", 
		      GTK_SIGNAL_FUNC (ExchangeIO_Rcv_),NULL);
  gtk_box_pack_start (GTK_BOX(vbbox), snd, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox),  vbbox, FALSE, FALSE, 0);

  ExIORcv = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(ExIORcv), FALSE);
  gtk_container_add (GTK_CONTAINER (vboxL), ExIORcv);
  gtk_container_add (GTK_CONTAINER (hboxXY), frame);
  gtk_container_add (GTK_CONTAINER (main_vbox/*ExIOPage*/), hboxXY);

  hbox  = gtk_hbox_new(FALSE, 5);
  hboxL = gtk_hbox_new(FALSE, 0);
  hboxR = gtk_hbox_new(FALSE, 0);
  vboxL = gtk_vbox_new(FALSE, 4);
  vbbox = gtk_hbox_new(FALSE,  0);

  cyclic = gtk_toggle_button_new_with_label("cyclic");
  gtk_widget_set_usize( cyclic, 40, 20);
  gtk_container_add( GTK_CONTAINER(vbbox), cyclic);
  gtk_signal_connect (GTK_OBJECT (cyclic), "toggled", 
		      GTK_SIGNAL_FUNC (ExIOSndCyclic),NULL);

  snd   = gtk_button_new_with_label("send");
  gtk_widget_set_usize( snd, 40, 20);
  gtk_signal_connect (GTK_OBJECT (snd), "released", 
		      GTK_SIGNAL_FUNC (ExchangeIO_Snd_),NULL);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxL, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox),  hboxR, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbbox),  snd, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vboxL), hbox,  FALSE, FALSE, 0);

  frame = gtk_frame_new("Send Process Data");
  gtk_container_add( GTK_CONTAINER(frame),  vboxL);

  label = gtk_label_new("Offset");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxL), label, FALSE, FALSE, 0);
  ExIOffSnd = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOffSnd, 50, 20);
  gtk_entry_set_text (GTK_ENTRY ( ExIOffSnd), "0");
  gtk_box_pack_start (GTK_BOX (hboxL), ExIOffSnd, FALSE, FALSE, 0);
  label = gtk_label_new("Length");
  gtk_widget_set_usize(label, 40, 20);
  gtk_box_pack_start (GTK_BOX (hboxR), label, FALSE, FALSE, 0);
  ExIOLenSnd = gtk_entry_new_with_max_length (10);
  gtk_widget_set_usize(ExIOLenSnd, 50, 20);
  gtk_entry_set_text (GTK_ENTRY (ExIOLenSnd), "16");
  gtk_box_pack_start (GTK_BOX (hboxR), ExIOLenSnd, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox), vbbox, FALSE, FALSE, 3);

  ExIOSnd = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(ExIOSnd), TRUE);
  gtk_container_add (GTK_CONTAINER (vboxL), ExIOSnd);
  gtk_container_add (GTK_CONTAINER (hboxXY), frame);

  status_bar_ExIO = gtk_statusbar_new ();      
  gtk_box_pack_start (GTK_BOX (main_vbox), status_bar_ExIO, FALSE, TRUE, 0);
  context_id_ExIO = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar_ExIO), "ExchangeIO");

  gtk_widget_show_all(ExIOPage);
  gtk_widget_hide( (GtkWidget *)boxCOM);
  gtk_widget_hide( (GtkWidget *)ExIO_COMerrMode);
  gtk_widget_hide( (GtkWidget *)ExIO_COMerrState);
  gtk_widget_hide( (GtkWidget *)ExIO_COMeMode);
  gtk_widget_hide( (GtkWidget *)ExIO_COMeState);
  gtk_widget_hide( (GtkWidget *)handshakeMode);
  gtk_widget_hide( (GtkWidget *)checkCOMerr);
  gtk_widget_hide( (GtkWidget *)frameCOM);
}

void PopulateMsgTrans ()
{
  GtkWidget *vbox1, *vbox2,  *vbox,   *vboxx, *vbbox, *main_vbox;
  GtkWidget *hbox,  *hboxx,  *hboxxy, *cyclic, *getMsg, *putMsg;
  GtkWidget *frame, *frame1, *label,  *AutoNr;

  main_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add (GTK_CONTAINER (MsgTransPage), main_vbox);
  hboxxy = gtk_hbox_new(TRUE, 6);
  gtk_container_add (GTK_CONTAINER (main_vbox), hboxxy);

  vboxx = gtk_vbox_new(TRUE, 5);
  vbox  = gtk_vbox_new(FALSE, 0);
  vbox1 = gtk_vbox_new(FALSE, 0);
  vbox2 = gtk_vbox_new(FALSE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  hboxx = gtk_hbox_new(FALSE,  4);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  frame = gtk_frame_new("Output Message");
  gtk_box_pack_start (GTK_BOX (hboxx), vbox,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hboxx), vbox1, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hboxx), vbox2, FALSE, FALSE, 0);
  gtk_container_add( GTK_CONTAINER(frame),  vboxx);
  gtk_container_add( GTK_CONTAINER(vboxx),  hboxx);
  label = gtk_label_new("RX");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetRX = gtk_entry_new_with_max_length (4);
  gtk_widget_set_usize(eMsgGetRX, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetRX), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetRX, FALSE, FALSE, 0);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("TX");
  gtk_widget_set_usize(label,24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetTX = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgGetTX, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetTX), "0");
  gtk_entry_set_editable(GTK_ENTRY( eMsgGetTX), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetTX, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("NR");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetNR = gtk_entry_new_with_max_length (3);
  gtk_widget_set_usize(eMsgGetNR, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetNR), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetNR, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("LN");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetLN = gtk_entry_new_with_max_length (3);
  gtk_widget_set_usize(eMsgGetLN, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetLN), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetLN, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("A");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetA = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgGetA, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetA), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetA, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("F");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetF = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgGetF, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetF), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetF, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("B");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetB = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgGetB, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetB), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetB, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("E");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgGetE = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgGetE, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgGetE), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgGetE, FALSE, FALSE, 0);

  vbbox = gtk_vbutton_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(vbbox), 24);
  gtk_box_pack_start (GTK_BOX(vbox2), vbbox, FALSE, FALSE, 0);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (vbbox),   0);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (vbbox), 24, 16);

  cyclic = gtk_toggle_button_new_with_label("cyclic");
  gtk_container_add( GTK_CONTAINER(vbbox), cyclic);
  gtk_signal_connect (GTK_OBJECT (cyclic), "toggled", 
		      GTK_SIGNAL_FUNC (GetCyclic),NULL);

  getMsg = gtk_button_new_with_label("get msg");
  gtk_container_add( GTK_CONTAINER(vbbox), getMsg);
  gtk_signal_connect (GTK_OBJECT (getMsg), "released", 
		      GTK_SIGNAL_FUNC (GetMsg),NULL);

  MsgGetData = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(MsgGetData), FALSE);
  gtk_container_add (GTK_CONTAINER (vboxx), MsgGetData);
  gtk_container_add (GTK_CONTAINER (hboxxy), frame);

  vboxx = gtk_vbox_new(TRUE, 5);
  vbox  = gtk_vbox_new(FALSE, 0);
  vbox1 = gtk_vbox_new(FALSE, 0);
  vbox2 = gtk_vbox_new(FALSE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  hboxx = gtk_hbox_new(FALSE, 4);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  frame1 = gtk_frame_new("Input Message");
  gtk_box_pack_start (GTK_BOX (hboxx), vbox,  FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hboxx), vbox1, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hboxx), vbox2, FALSE, FALSE, 0);
  gtk_container_add( GTK_CONTAINER(frame1), vboxx);
  gtk_container_add( GTK_CONTAINER(vboxx),  hboxx);

  label = gtk_label_new("RX");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutRX = gtk_entry_new_with_max_length (4);
  gtk_widget_set_usize(eMsgPutRX, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutRX), "1");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutRX, FALSE, FALSE, 0);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("TX");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutTX = gtk_entry_new_with_max_length (4);
  gtk_widget_set_usize(eMsgPutTX, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutTX), "16");
  gtk_entry_set_editable(GTK_ENTRY( eMsgPutTX), TRUE);//25.01.05 FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutTX, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("NR");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutNR = gtk_entry_new_with_max_length (4);
  gtk_widget_set_usize(eMsgPutNR, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutNR), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutNR, FALSE, FALSE, 0);
  AutoNr = gtk_toggle_button_new_with_label(" Auto ");
  gtk_box_pack_start (GTK_BOX (hbox), AutoNr, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT (AutoNr), "toggled", 
		      GTK_SIGNAL_FUNC(AutoNum),NULL);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("LN");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutLN = gtk_entry_new_with_max_length (4);
  gtk_widget_set_usize(eMsgPutLN, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutLN), "0");
  gtk_entry_set_editable(GTK_ENTRY( eMsgPutLN), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutLN, FALSE, FALSE, 0);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new(" A");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutA = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgPutA, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutA), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutA, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new(" F");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutF = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgPutF, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutF), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutF, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new(" B");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutB = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgPutB, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutB), "10");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutB, FALSE, FALSE, 0);
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);
  label = gtk_label_new(" E");
  gtk_widget_set_usize(label, 24, 20);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  eMsgPutE = gtk_entry_new_with_max_length (2);
  gtk_widget_set_usize(eMsgPutE, 40, 20);
  gtk_entry_set_text (GTK_ENTRY ( eMsgPutE), "0");
  gtk_box_pack_start (GTK_BOX (hbox), eMsgPutE, FALSE, FALSE, 0);

  vbbox = gtk_vbutton_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(vbbox), 24);
  gtk_box_pack_start (GTK_BOX(vbox2), vbbox, FALSE, FALSE, 0);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (vbbox),   0);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (vbbox), 24, 16);

  cyclic = gtk_toggle_button_new_with_label("cyclic");
  gtk_container_add( GTK_CONTAINER(vbbox), cyclic);
  gtk_signal_connect (GTK_OBJECT (cyclic), "toggled", 
		      GTK_SIGNAL_FUNC(PutCyclic),NULL);

  putMsg = gtk_button_new_with_label("put msg");
  gtk_container_add( GTK_CONTAINER(vbbox), putMsg);
  gtk_signal_connect (GTK_OBJECT (putMsg), "released", 
		      GTK_SIGNAL_FUNC(PutMsg),NULL);

  MsgPutData = gtk_text_new (NULL, NULL);
  gtk_text_set_editable(GTK_TEXT(MsgPutData), TRUE);
  gtk_container_add (GTK_CONTAINER (vboxx), MsgPutData);
  gtk_container_add (GTK_CONTAINER (hboxxy), frame1);

  status_bar_Msg = gtk_statusbar_new ();      
  gtk_box_pack_start (GTK_BOX (main_vbox), status_bar_Msg, FALSE, FALSE, 0);
  context_id_Msg = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar_Msg), "MsgTransfer");

  gtk_widget_show_all( MsgTransPage);
}

GtkWidget *CreateEditField (char *label_str)
{
  GtkWidget *hbox;
  GtkWidget *label;
  
  hbox = gtk_vbox_new (TRUE, 0);
  label = gtk_label_new (label_str);
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);
  gtk_widget_show (hbox);
  return (hbox);
}

/*{{=========  DrvInfoPage components  ==================*/
void UpdateDrvInfo (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abData[512];                /* Buffer for various information */
  short           sRet;           
  char            buff[96];
  DRIVERINFO     *pDriverInf = NULL;

  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo1), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo3), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo4), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo5), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo6), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo7), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo8), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo9), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo10), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo11), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo12), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo13), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo14), "");
  gtk_entry_set_text(GTK_ENTRY( entryDrvInfo15), "");

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetInfo( usBoard,
			     GET_DRIVER_INFO,
			     sizeof(DRIVERINFO),
			     &abData[0])) != DRV_NO_ERROR) {// some error occured, cure it!
      g_snprintf(buff, 72, "Error by driver function DevGetInfo(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo, buff);
      goto leave;
    } else {
      // Load  structure pointer
      pDriverInf = (DRIVERINFO *)&abData[0];
      
      g_snprintf(buff, 48, "%ld", pDriverInf->ulOpenCnt);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo1), buff);
      g_snprintf(buff, 48, "%ld", pDriverInf->ulReadCnt);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo3), buff);
      g_snprintf(buff, 48, "%ld", pDriverInf->ulWriteCnt);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo4), buff);
      g_snprintf(buff, 48, "%ld", pDriverInf->ulIRQCnt);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo5), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bInitMsgFlag);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo6), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bReadMsgFlag);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo7), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bWriteMsgFlag);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo8), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bLastFunction);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo9), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bWriteState);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo10), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bReadState);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo11), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bHostFlags);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo12), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bMyDevFlags);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo13), buff);
      g_snprintf(buff, 48, "0x%.2X", pDriverInf->bExIOFlag);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo14), buff);
      g_snprintf(buff, 48, "%ld", pDriverInf->ulExIOCnt);
      gtk_entry_set_text(GTK_ENTRY( entryDrvInfo15), buff);
      
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_DrvInfo), context_id_DrvInfo, "         Done");
    }
  }
 leave: ;
}

void PopulateDrvInfo ()
{
  GtkWidget *button, *separator;
  GtkWidget *vboxx, *vboxx1, *hboxx, *hbox, *main_vbox;
  
  main_vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add  (GTK_CONTAINER ( DrvInfoPage), main_vbox);
  
  hboxx  = gtk_hbox_new (FALSE, 0);
  vboxx  = gtk_vbox_new (FALSE, 0);
  vboxx1 = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_vbox), hboxx, FALSE, FALSE, 0); 
  gtk_box_pack_start (GTK_BOX (hboxx), vboxx, FALSE, FALSE, 0); 
  gtk_box_pack_start (GTK_BOX (hboxx), vboxx1, FALSE, FALSE, 0); 
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("open counter:");
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, TRUE, TRUE, 4);
  entryDrvInfo1 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo1), "");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo1), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo1, TRUE, TRUE, 4);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("read counter:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo3 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo3), "");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo3), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo3, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("write counter:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo4 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo4), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo4), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo4, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, FALSE, 0);
  labDrvInfo = gtk_label_new ("interrupt counter:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo5 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo5), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo5), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo5, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("init state:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo6 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo6), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo6), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo6, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("HostMBXState:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo7 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo7), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo7), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo7, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("DeviceMBXState:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo8 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo8), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo8), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo8, TRUE, TRUE, 4);
  
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("last function:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo9 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo9), "");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo9), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo9, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("write state:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo10 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo10), "");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo10), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo10, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("read state:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo11 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo11), "");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo11), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo11, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("HOST flags:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo12 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo12), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo12), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo12, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, FALSE, 0);
  labDrvInfo = gtk_label_new ("Device flags:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo13 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo13), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo13), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo13, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("IO flags:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo14 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo14), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo14), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo14, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("IO Exchange:");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_misc_set_alignment(GTK_MISC(labDrvInfo), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  entryDrvInfo15 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryDrvInfo15), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryDrvInfo15), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryDrvInfo15, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);
  labDrvInfo = gtk_label_new ("");
  gtk_widget_set_usize( labDrvInfo, 112, 20);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx1), hbox, FALSE, TRUE, 0);

  labDrvInfo = gtk_label_new ("");
  gtk_widget_set_usize( labDrvInfo, 112, 10);
  gtk_box_pack_start (GTK_BOX (hbox), labDrvInfo, FALSE, FALSE, 4);
  button = gtk_button_new_with_label ("update");
  gtk_container_add  (GTK_CONTAINER (hbox), button);
  gtk_signal_connect (GTK_OBJECT (button), "released", 
		      GTK_SIGNAL_FUNC(UpdateDrvInfo),NULL);
  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (main_vbox), separator, FALSE, FALSE, 0);

  status_bar_DrvInfo = gtk_statusbar_new ();      
  gtk_box_pack_start (GTK_BOX (main_vbox), status_bar_DrvInfo, TRUE, TRUE, 0);
  context_id_DrvInfo = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar_DrvInfo), "DrvInfo");
  gtk_widget_show_all (DrvInfoPage);
}
/*}}=========  DrvInfoPage components  ==================*/
/*{{========= BoardInfoPage components ==================*/
void GetHardwareInfo (GtkWidget *widget, gpointer *data)
{
  char     buff[96];
  unsigned short  usBoard = 0xFFFF;

  gtk_label_set_text( GTK_LABEL( labBoardInfo1),  "Device type: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo2),  "Device model: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo3),  "Device identifier: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo4),  "DPM address: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo5),  "DPM size: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo6),  "IRQ number: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo7),  "Operation mode: ");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "");

       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;

  if( usBoard > tHardwareInfo.tBoardInfo.usBoards_detected) {
    gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
    gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, " No Board detected");
    goto leave;
  }
  if( tHardwareInfo.fInfo) {
    g_snprintf(buff, 16, "%lX", tHardwareInfo.tBoardInfo.tBoard[usBoard].ulPhysicalAddress);
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), buff);
    if( tHardwareInfo.fIrq)
      g_snprintf(buff, 16, "%d", tHardwareInfo.tBoardInfo.tBoard[usBoard].usIrq);
    else
      g_snprintf(buff, 16, "%d (desabled!)", tHardwareInfo.tBoardInfo.tBoard[usBoard].usIrq);
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), buff);
  }
  else {
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "INFO NOT AVAILABLE");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "INFO NOT AVAILABLE");
  }
  if( tHardwareInfo.fInit) {
    g_snprintf(buff, 4, "%c", tHardwareInfo.bDevType);
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), buff);
    g_snprintf(buff, 4, "%c", tHardwareInfo.bDevModel);
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), buff);
    g_snprintf(buff, 4,"%c%c%c", tHardwareInfo.abDevIdentifier[0],
	                         tHardwareInfo.abDevIdentifier[1], 
                                 tHardwareInfo.abDevIdentifier[2]);  
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), buff);
    g_snprintf(buff, 24, "%d (x 1024 byte)", tHardwareInfo.bDpmSize); 
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), buff);
    if( tHardwareInfo.fIrq && tHardwareInfo.tBoardInfo.tBoard[usBoard].usIrq != 0)
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "interrupt mode active");
    else
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "polling mode active");
  }
  else {
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "INFO NOT AVAILABLE");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "INFO NOT AVAILABLE");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "INFO NOT AVAILABLE");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "INFO NOT AVAILABLE");
    gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "INFO NOT AVAILABLE");
  }
  gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
  gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "HARDWARE_INFO");
 leave: ;
}

void GetVersionInfo (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abData[512];    /* Buffer for various information */
  short           sRet;           
  char            buff[96];
  unsigned char  *pabBuffer;
  VERSIONINFO    *pVersionInf = NULL;

  gtk_label_set_text( GTK_LABEL( labBoardInfo1), "Date: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo2), "DevNumber: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo3), "SerialNumber: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo4), "OS Identifier 0: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo5), "OS Identifier 1: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo6), "OS Identifier 2: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo7), "OEM Identifier:");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "");
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetInfo( usBoard,
			     GET_VERSION_INFO,
			     sizeof(VERSIONINFO),
			     &abData[0])) != DRV_NO_ERROR) {// some error occured, cure it!
      g_snprintf(buff, 72, "Error by driver function DevGetInfo(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
    } else {
      // Load  structure pointer
      pVersionInf = (VERSIONINFO *)&abData[0];
      pabBuffer = (unsigned char *)&pVersionInf->ulDate;
      g_snprintf(buff, 48, "%.2X.%.2X.%.2X%.2X", pabBuffer[0],
		 pabBuffer[1],  
		 pabBuffer[2],
		 pabBuffer[3]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), buff);
      
      pabBuffer = (unsigned char *)&pVersionInf->ulDeviceNo;
      g_snprintf(buff, 48, "%.2X%.2X%.2X%.2X", pabBuffer[0],
		 pabBuffer[1],
		 pabBuffer[2],
		 pabBuffer[3]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), buff);
      
      
      pabBuffer = (unsigned char *)&pVersionInf->ulSerialNo;
      g_snprintf(buff, 48, "%.2X%.2X%.2X%.2X", pabBuffer[0],
		 pabBuffer[1],
		 pabBuffer[2],
		 pabBuffer[3]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), buff);
      
      g_snprintf(buff, 8, "%.4s", (char *)&pVersionInf->abPcOsName0[0]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), buff);
      
      g_snprintf(buff, 8, "%.4s", (char *)&pVersionInf->abPcOsName1[0]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), buff);
      
      g_snprintf(buff, 8, "%.4s", (char *)&pVersionInf->abPcOsName2[0]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), buff);
      
      g_snprintf(buff, 8, "%.4s", (char *)&pVersionInf->abOemIdentifier[0]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), buff);
      
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "         Done");
    }
  }
 leave: ;
}

void GetTaskInfo (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abData[512];     /* Buffer for various information */
  short           sRet;            
  char            buff[96];
  TASKINFO       *pTask = NULL;
  gtk_label_set_text( GTK_LABEL( labBoardInfo1), "Task1: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo2), "Task2: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo3), "Task3: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo4), "Task4: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo5), "Task5: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo6), "Task6: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo7), "Task7: ");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "");
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
  }
  else {
      if ( (sRet = DevGetInfo( usBoard,
                               GET_TASK_INFO,
                               sizeof(TASKINFO),
                               &abData[0])) != DRV_NO_ERROR) {// some error occured, cure it!
	g_snprintf(buff, 72, "Error by driver function DevGetInfo(). (Error: %d)", sRet);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
	goto leave;
      } else {
        // Load  structure pointer
        pTask = (TASKINFO *)&abData[0];
	g_snprintf(buff, 48, "Version: %.4d State: 0x%.2X", pTask->tTaskInfo[0].usTaskVersion,
		                                              pTask->tTaskInfo[0].bTaskCondition);
	gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), buff);
	g_snprintf(buff, 48, "Version: %.4d State: 0x%.2X", pTask->tTaskInfo[1].usTaskVersion,
		                                              pTask->tTaskInfo[1].bTaskCondition);
	gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), buff);
	g_snprintf(buff, 48, "Version: %.4d State: 0x%.2X", pTask->tTaskInfo[2].usTaskVersion,
		                                              pTask->tTaskInfo[2].bTaskCondition);
	gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), buff);
	g_snprintf(buff, 48, "Version: %.4d State: 0x%.2X", pTask->tTaskInfo[3].usTaskVersion,
		                                              pTask->tTaskInfo[3].bTaskCondition);
	gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), buff);
	g_snprintf(buff, 48, "Version: %.4d State: 0x%.2X", pTask->tTaskInfo[4].usTaskVersion,
		                                              pTask->tTaskInfo[4].bTaskCondition);
	gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), buff);
	g_snprintf(buff, 48, "Version: %.4d State: 0x%.2X", pTask->tTaskInfo[5].usTaskVersion,
		                                              pTask->tTaskInfo[5].bTaskCondition);
	gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), buff);
	g_snprintf(buff, 48, "Version: %.4d State: 0x%.2X", pTask->tTaskInfo[6].usTaskVersion,
		                                              pTask->tTaskInfo[6].bTaskCondition);
	gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), buff);
	gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
	gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "         Done");
      }
  }
 leave: ;
}

void GetRcsInfo (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abData[512];    /* Buffer for various information */
  short           sRet;           
  char            buff[96];
  RCSINFO        *pRCS = NULL;
  gtk_label_set_text( GTK_LABEL( labBoardInfo1), "RCS Vers.: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo2), "RCS Error: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo3), "HostWatchdog: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo4), "DeviceWatchdog: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo5), "Segment count: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo6), "Device Address: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo7), "Driver Type: ");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "");
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetInfo( usBoard,
			     GET_RCS_INFO,
			     sizeof(RCSINFO),
			     &abData[0])) != DRV_NO_ERROR) {// some error occured, cure it!
      g_snprintf(buff, 72, "Error by driver function DevGetInfo(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
    } else {
      // Load  structure pointer
      pRCS = (RCSINFO *)&abData[0];
      g_snprintf(buff, 8, "%.4d",    pRCS->usRcsVersion);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), buff);
      g_snprintf(buff, 8, "%d",      pRCS->bRcsError);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), buff);
      g_snprintf(buff, 8, "0x%.2X",  pRCS->bHostWatchDog);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), buff);
      g_snprintf(buff, 8,  "0x%.2X", pRCS->bDevWatchDog);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), buff);
      g_snprintf(buff, 8,  "%d",     pRCS->bSegmentCount);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), buff);
      g_snprintf(buff, 8,  "%d",     pRCS->bDeviceAdress);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), buff);
      g_snprintf(buff, 8,  "0x%.2X", pRCS->bDriverType);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), buff);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "         Done");
    }
  }
 leave: ;
}

void GetIOInfo (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abData[512];    /* Buffer for various information */
  short           sRet;           
  char            buff[96];
  IOINFO         *pIO = NULL;
  gtk_label_set_text( GTK_LABEL( labBoardInfo1), "COM-Bit: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo2), "IO exchange mode: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo3), "IO exchange count: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo4), " ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo5), " ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo6), " ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo7), " ");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "");
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetInfo( usBoard,
			     GET_IO_INFO,
			     sizeof(IOINFO),
			     &abData[0])) != DRV_NO_ERROR) {// some error occured, cure it!
      g_snprintf(buff, 72, "Error by driver function DevGetInfo(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
    } else {
      // Load  structure pointer
      pIO = (IOINFO *)&abData[0];
      g_snprintf(buff, 8, "0x%.2X", pIO->bComBit);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), buff);
      g_snprintf(buff, 8, "%d", pIO->bIOExchangeMode);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), buff);
      g_snprintf(buff, 18, "%ld", pIO->ulIOExchangeCnt);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), buff);
      
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "         Done");
    }
  }
 leave: ;
}

void GetIOSndData (GtkWidget *widget, gpointer *data)
{
  unsigned short  usBoard;
  unsigned char   abData[512];   /* Buffer for various information */
  short           sRet;          
  char            buff[96];

  gtk_label_set_text( GTK_LABEL( labBoardInfo1), "Offset  0: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo2), "Offset 16: ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo3), " ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo4), " ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo5), " ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo6), " ");
  gtk_label_set_text( GTK_LABEL( labBoardInfo7), " ");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo3), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo4), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo5), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo6), "");
  gtk_entry_set_text(GTK_ENTRY( entryBoardInfo7), "");
       if( DEV_INFO_FLAGS & BOARD_0)
	 usBoard = 0;
  else if( DEV_INFO_FLAGS & BOARD_1)
         usBoard = 1;
  else if( DEV_INFO_FLAGS & BOARD_2)
         usBoard = 2;
  else if( DEV_INFO_FLAGS & BOARD_3)
         usBoard = 3;
  else
         usBoard = 0xFFFF;
  if( usBoard == 0xFFFF) {
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "No Board selected");
      goto leave;
  }
  else if ( !(DEV_INFO_FLAGS & DEV_INIT) ) {// Board not init, go away!
      g_snprintf(buff, 40, "Board-%d not initialized!\n", usBoard);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
  }
  else {
    if ( (sRet = DevGetInfo( usBoard,
			     GET_IO_SEND_DATA,
			     32,
			     &abData[0])) != DRV_NO_ERROR) {// some error occured, cure it!
      g_snprintf(buff, 72, "Error by driver function DevGetInfo(). (Error: %d)", sRet);
      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, buff);
      goto leave;
    } else {
      // Load  structure pointer
      g_snprintf(buff,96,"%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X",
		 abData[0],
		 abData[1],
		 abData[2],
		 abData[3],
		 abData[4],
		 abData[5],
		 abData[6],
		 abData[7],
		 abData[8],
		 abData[9],
		 abData[10],
		 abData[11],
		 abData[12],
		 abData[13],
		 abData[14],
		 abData[15]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo1), buff);
      g_snprintf(buff,96,"%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X",
		 abData[16],
		 abData[17],
		 abData[18],
		 abData[19],
		 abData[20],
		 abData[21],
		 abData[22],
		 abData[23],
		 abData[24],
		 abData[25],
		 abData[26],
		 abData[27],
		 abData[28],
		 abData[29],
		 abData[30],
		 abData[31]);
      gtk_entry_set_text(GTK_ENTRY( entryBoardInfo2), buff);

      gtk_statusbar_pop ( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo );
      gtk_statusbar_push( GTK_STATUSBAR(status_bar_BoardInfo), context_id_BoardInfo, "         Done");
    }
  }
 leave: ;
}

void SetStyleRecursively (GtkWidget *widget, gpointer data)
{
  GtkStyle *style;
  
  style = (GtkStyle *) data;
  gtk_widget_set_style (widget, style);
  if (GTK_IS_CONTAINER (widget)) {
    gtk_container_foreach (GTK_CONTAINER (widget), 
                           SetStyleRecursively, style);
  }
}

GtkWidget *create_bbox( gint horizontal, char *title, gint spacing, gint child_w, gint child_h, gint layout )
{
  GtkWidget *frame;
  GtkWidget *bbox;
  GtkWidget *button;
  GtkStyle *style;
  GtkStyle *defstyle;
  GdkFont *font;

  defstyle = gtk_widget_get_default_style ();
  style = gtk_style_copy (defstyle);
  if ( (font = gdk_font_load ( "-Schumacher-Clean-Medium-R-Normal--12-120-75-75-C-60-ISO646.1991-IRV")) == NULL)
    if( (font = gdk_font_load ( "-Adobe-Courier-Medium-R-Normal--11-110-75-75-M-70-ISO8859-1")) == NULL)
      font = gdk_font_load ( "-adobe-times-medium-r-normal--11-110-75-75-p-59-iso8859-1");
  if( font) style->font = font;

  frame = gtk_frame_new (title);
  if (horizontal)
    bbox = gtk_hbutton_box_new ();
  else
    bbox = gtk_vbutton_box_new ();

  gtk_container_set_border_width (GTK_CONTAINER (bbox), 5);
  gtk_container_add (GTK_CONTAINER (frame), bbox);

  gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), layout);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), spacing);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (bbox), child_w, child_h);

  button = gtk_button_new_with_label ("TASK_INFO");
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_signal_connect (GTK_OBJECT (button), "released", 
		      GTK_SIGNAL_FUNC(GetTaskInfo),NULL);
  button = gtk_button_new_with_label ("RCS_INFO");
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_signal_connect (GTK_OBJECT (button), "released", 
		      GTK_SIGNAL_FUNC(GetRcsInfo),NULL);
  button = gtk_button_new_with_label ("IO_INFO");
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_signal_connect (GTK_OBJECT (button), "released", 
		      GTK_SIGNAL_FUNC(GetIOInfo),NULL);
  button = gtk_button_new_with_label ("IO_SEND_DATA");
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_signal_connect (GTK_OBJECT (button), "released", 
		      GTK_SIGNAL_FUNC(GetIOSndData),NULL);
  button = gtk_button_new_with_label ("VERSION_INFO");
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_signal_connect (GTK_OBJECT (button), "released", 
		      GTK_SIGNAL_FUNC(GetVersionInfo),NULL);
  button = gtk_button_new_with_label ("HARDWARE");
  gtk_container_add (GTK_CONTAINER (bbox), button);
  gtk_signal_connect (GTK_OBJECT (button), "released", 
		      GTK_SIGNAL_FUNC(GetHardwareInfo),NULL);
  SetStyleRecursively (bbox, (gpointer) style);

  return(frame);
}

void PopulateBoardInfo ()
{
  GtkWidget *frame, *vboxx, *hbox, *main_vbox;

  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add  (GTK_CONTAINER ( BoardInfoPage), main_vbox);
  gtk_box_pack_start (GTK_BOX (main_vbox), 
		      create_bbox (TRUE, "Select Info Area", 40, 65, 20, GTK_BUTTONBOX_SPREAD),
		      FALSE, FALSE, 0);
  vboxx = gtk_vbox_new (FALSE, 0);
  frame = gtk_frame_new("results");
  gtk_container_add (GTK_CONTAINER (frame), vboxx);
  gtk_box_pack_start (GTK_BOX (main_vbox), frame, FALSE, FALSE, 0); 
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labBoardInfo1 = gtk_label_new ("---");
  gtk_widget_set_usize( labBoardInfo1, 116, 10);
  gtk_misc_set_alignment(GTK_MISC(labBoardInfo1), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labBoardInfo1, FALSE, FALSE, 4);
  entryBoardInfo1 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryBoardInfo1), "");
  gtk_entry_set_editable(GTK_ENTRY( entryBoardInfo1), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryBoardInfo1, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labBoardInfo2 = gtk_label_new ("---");
  gtk_widget_set_usize( labBoardInfo2, 116, 10);
  gtk_misc_set_alignment(GTK_MISC(labBoardInfo2), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labBoardInfo2, FALSE, FALSE, 4);
  entryBoardInfo2 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryBoardInfo2), "");
  gtk_entry_set_editable(GTK_ENTRY( entryBoardInfo2), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryBoardInfo2, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labBoardInfo3 = gtk_label_new ("---");
  gtk_widget_set_usize( labBoardInfo3, 116, 10);
  gtk_misc_set_alignment(GTK_MISC(labBoardInfo3), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labBoardInfo3, FALSE, FALSE, 4);
  entryBoardInfo3 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryBoardInfo3), "");
  gtk_entry_set_editable(GTK_ENTRY( entryBoardInfo3), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryBoardInfo3, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labBoardInfo4 = gtk_label_new ("---");
  gtk_widget_set_usize( labBoardInfo4, 116, 10);
  gtk_misc_set_alignment(GTK_MISC(labBoardInfo4), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labBoardInfo4, FALSE, FALSE, 4);
  entryBoardInfo4 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryBoardInfo4), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryBoardInfo4), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryBoardInfo4, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, FALSE, 0);
  labBoardInfo5 = gtk_label_new ("---");
  gtk_widget_set_usize( labBoardInfo5, 116, 10);
  gtk_misc_set_alignment(GTK_MISC(labBoardInfo5), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labBoardInfo5, FALSE, FALSE, 4);
  entryBoardInfo5 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryBoardInfo5), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryBoardInfo5), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryBoardInfo5, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labBoardInfo6 = gtk_label_new ("---");
  gtk_widget_set_usize( labBoardInfo6, 116, 10);
  gtk_misc_set_alignment(GTK_MISC(labBoardInfo6), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labBoardInfo6, FALSE, FALSE, 4);
  entryBoardInfo6 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryBoardInfo6), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryBoardInfo6), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryBoardInfo6, TRUE, TRUE, 4);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vboxx), hbox, FALSE, TRUE, 0);
  labBoardInfo7 = gtk_label_new ("---");
  gtk_widget_set_usize( labBoardInfo7, 116, 10);
  gtk_misc_set_alignment(GTK_MISC(labBoardInfo7), 4, 0);
  gtk_box_pack_start (GTK_BOX (hbox), labBoardInfo7, FALSE, FALSE, 4);
  entryBoardInfo7 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY ( entryBoardInfo7), " ");
  gtk_entry_set_editable(GTK_ENTRY( entryBoardInfo7), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), entryBoardInfo7, TRUE, TRUE, 4);

  status_bar_BoardInfo = gtk_statusbar_new ();      
  gtk_box_pack_start (GTK_BOX (main_vbox), status_bar_BoardInfo, TRUE, TRUE, 3);
  context_id_BoardInfo = gtk_statusbar_get_context_id( GTK_STATUSBAR(status_bar_BoardInfo), "BoardInfo");
  gtk_widget_show_all (BoardInfoPage);
}
/*}}========= BoardInfoPage components ==================*/

BOOL StringToHex( char           *pcsInText,
		  unsigned short usUserBufferLen,
		  unsigned char  *pabUserBuffer,
		  unsigned short *pusOutputLen,
		  GtkStatusbar   *pStatusbar,
		  gint           context_id)
{
  unsigned char   bChar, bLowByte, bHighByte, szLineBuffer[512];
  unsigned short  usTextLen, usDataLen, usIdx, usOutIdx;
  BOOL  bState;
  BOOL  bRet = FALSE;  

  usTextLen = strlen(pcsInText);
  if ( usTextLen == 0 ) {
    // No entry
    *pusOutputLen = 0;
    gtk_statusbar_pop (GTK_STATUSBAR(pStatusbar),context_id);
    gtk_statusbar_push(GTK_STATUSBAR(pStatusbar),context_id," No input data found !");
  } else if ( usTextLen > usUserBufferLen) {///sizeof(szLineBuffer)) {
    sprintf( szLineBuffer, "String longer then internal buffer (%d Bytes)", usUserBufferLen);
    gtk_statusbar_pop (GTK_STATUSBAR(pStatusbar),context_id);
    gtk_statusbar_push(GTK_STATUSBAR(pStatusbar),context_id, szLineBuffer);
  } else {
    bState    = TRUE;
    usDataLen = 0;
    // Remove all none ASCII characters
    for ( usIdx = 0; usIdx < usTextLen; usIdx++) {
      bChar = pcsInText[usIdx];
      if ( isxdigit( bChar)) {
        // This is a digit 0..9 A..F
	if(!isdigit( bChar)) bChar = (unsigned char)toupper( bChar);
        szLineBuffer[usDataLen] = bChar;
        usDataLen++;
      } else if (!isspace(bChar)) {
        // not a space, signal error
        bState = FALSE;
        // String too long
	gtk_statusbar_pop (GTK_STATUSBAR(pStatusbar), context_id);
	gtk_statusbar_push(GTK_STATUSBAR(pStatusbar),context_id,"Invalid character found");
      } 
    }
    if ( bState == TRUE) {
      // There are bytes to send
      if ( usDataLen %2) {
	// Length mut be even
	gtk_statusbar_pop (GTK_STATUSBAR(pStatusbar), context_id);
	gtk_statusbar_push(GTK_STATUSBAR(pStatusbar),context_id,"String must have even number of characters");
      } else {
	// Convert to hexadecimal area
	memset( pabUserBuffer, 0, usUserBufferLen); // clear user buffer
	for ( usIdx = 0, usOutIdx = 0; usIdx < usDataLen; usIdx += 2, usOutIdx++) {
	  bLowByte=(char)(isdigit((int)szLineBuffer[usIdx])?szLineBuffer[usIdx]-0x30:szLineBuffer[usIdx]-0x37);
	  bHighByte=(char)(isdigit((int)szLineBuffer[usIdx+1])?szLineBuffer[usIdx+1]-0x30:szLineBuffer[usIdx+1] - 0x37);  
	  pabUserBuffer[usOutIdx] = (char)(bHighByte | (bLowByte << 4));
        }
        *pusOutputLen = usOutIdx;    // return data length
        bRet = TRUE;
      }
    }
  }
  return bRet;
}

void HexToTextBox( GtkText       *pwEdit, 
		   gshort         usBuffLen,
		   unsigned char *pabUserBuffer)
{
  char    chData[3*1024+1]; // = (char *)malloc(3*usBuffLen+1);
  char    bByteHigh, bByteLow;
  gshort  i = 0;
  GdkFont *fixed_font;
  // Clear contents
  fixed_font = gdk_font_load ("-misc-fixed-medium-r-*-*-*-110-*-*-*-*-*-*");
  gtk_text_backward_delete ( pwEdit, gtk_text_get_length( pwEdit));
  for ( i=0; i < usBuffLen; i++) {
    bByteHigh = pabUserBuffer[i] >> 4;
    bByteLow  = pabUserBuffer[i] & 0x0F;
    if ( bByteHigh > 9) {
      // A..F
      bByteHigh += 0x37;
    } else {
      // 0..9
      bByteHigh += 0x30;
    }
    if ( bByteLow > 9) {
      // A..F
      bByteLow += 0x37;
    } else {
      // 0..9
      bByteLow += 0x30;
    }
    //cDataText +=cTemp;
    chData[3*i]     = bByteHigh;
    chData[3*i + 1] = bByteLow;
    chData[3*i + 2] = ' ';
    if ( (i == (usBuffLen - 1))){///???  ||
	 ///???         ((i & 0x0F) == 15)   ) {
      chData[3*i + 3] = '\0';
      gtk_text_insert( pwEdit,
		       fixed_font,
		       NULL,
		       NULL,
		       &chData[0],
		       -1);
      break;
    }
  }
}

BOOL CheckEditEntry( GtkWidget      *pEntry,
		     unsigned short *usUserBuffer,
		     long           lMaxSize,
		     char           *chErr) 
{
  char   *pszText, *chValue;
  long    lValue;
  BOOL    fRet = FALSE;

  chValue = gtk_entry_get_text (GTK_ENTRY ( pEntry));
  lValue = strtol( chValue, &pszText, 10);

  if ( *pszText != '\0') {
    memcpy(chErr, "Can't convert an entry !", 25);
  } else if ( lValue > 255) {
    memcpy(chErr,"Value too big !", 16);
  } else {
    *usUserBuffer = (unsigned short)lValue;
    fRet = TRUE;
  }
  return fRet;
}

BOOL CheckEditEntryC( GtkWidget     *pEntry,
		      unsigned char *bUserBuffer,
		      long          lMaxSize,
		      char          *chErr) 
{
  char            *pszText, *chValue;
  long            lValue;
  BOOL            fRet = FALSE;

  chValue = gtk_entry_get_text (GTK_ENTRY ( pEntry));
  lValue = strtol( chValue, &pszText, 10);

  if ( *pszText != '\0') {
    memcpy(chErr, "Can't convert an entry !", 25);
  } else if ( lValue > 255) {
    memcpy(chErr,"Value too big !", 16);
  } else {
    *bUserBuffer = (unsigned char)lValue;
    fRet = TRUE;
  }
  return fRet;
}
/*}}========= DrvFunPage Components ==================*/
