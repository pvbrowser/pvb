/* <St> *******************************************************************

  FILENAME    : RcsDef.h

  -------------------------------------------------------------------------
  CREATED BY  :	R.Mayer,  Hilscher GmbH
  CREATED AT  :	10.03.97
  PROJECT     :	
  =========================================================================

  FUNCTION / CLASSDESCRIPTION:
  
  =========================================================================

  CHANGES OF REVISIONS :

  Version	Name		Date		Change
  -------------------------------------------------------------------------

  1.000   RM     	10.03.97				Created

  ******************************************************************** <En> */
//	Prevent multiple inclusion
#ifndef __RCSDEF_H__
#define __RCSDEF_H__

#ifdef _cplusplus
  extern "C" {
#endif  /* _cplusplus */

  #define FIRMWARE_DOWNLOAD         1
  #define CONFIGURATION_DOWNLOAD    2

  /* ======================================================================== */
  /* Message definitions                                                      */
  /* ======================================================================== */

  #define RCS_TASK              0x00   	  /* Number of RCS	*/
  #define PLC_TASK              0x02  	  /* Number for PLC_TASK	*/

  #define MSG_SYSTEM_TX         0xFF  	  /* Transmitter for system functions number	*/
  
  #define RCS_B_TASK_STATE     4      // Task state  

  /************************************************************************** */
  /* Genaral RCS commands                                                     */
  /************************************************************************** */

  #define RCS_B_SYSFKT          1
  #define RCS_B_TASKFKT         2
  #define RCS_B_DIAGNOSE        3
  #define RCS_B_STRUCTFNC       4
  #define RCS_B_TRACE           5
  #define RCS_B_LOADFKT         6
  #define RCS_B_DBMFKT          10

  /* ----------------------- */
  /* Message extension masks */
  /* ----------------------- */

  #define RCS_FIFO_MSK                           0
  #define RCS_LIFO_MSK                           1
  #define RCS_NAK_MSK                            2

  #define RCS_NORM_MSK                           0
  #define RCS_FIRST_MSK                          4
  #define RCS_CONT_MSK                           8
  #define RCS_LAST_MSK                         0x0C
  #define RCS_SEQ_MSK                          0x0C

  /* ------------------ */
  /* Mode definitions   */
  /* ------------------ */

  #define MODE_NEUSTART               0     /* Command: B_SYSFKT */
  #define MODE_KALTSTART              1     /* Command: B_SYSFKT */
  #define MODE_WARMSTART              2     /* Command: B_SYSFKT */
  #define MODE_ZYKL_STATUS_STOP       3     /* Command: B_SYSFKT */
  #define MODE_FWVERSION              4     /* Command: B_SYSFKT */
  #define MODE_GET_PROJ_WERTE_HW      5     /* Command: B_SYSFKT */
  #define MODE_GET_PROJ_WERTE_SW      6     /* Command: B_SYSFKT */
  #define MODE_SHOW_DYN_SYSSTAT       7     /* Command: B_SYSFKT */
  #define MODE_SETTIME                9     /* not implemented yet */
  #define MODE_SET_DEVICE_DATA        9     /* not implemented yet */
  #define MODE_MODUL_RCS              11    /* Command: B_SYSFKT */
  #define MODE_MODUL_LIB              12    /* Command: B_SYSFKT */
  #define MODE_MODUL_MCL              13    /* Command: B_SYSFKT */
  #define MODE_MODUL_LIB              12    /* Command: B_SYSFKT */
  #define MODE_MODUL_MCL              13    /* Command: B_SYSFKT */
  #define MODE_DISTRIBUTOR_DRIVER     14    /* Command: B_SYSFKT ->function 1 = insert */
  #define MODE_PRINT_DRIVER           15    /* Command: B_SYSFKT */
  #define MODE_GET_RCS_ERROR          16    /* Command: B_SYSFKT */

  #define MODE_PRINT_DEVICE           1     /* NO mode, only for MENU.H */
  #define MODE_PRINT_DEVICE_DRIVER    2     /* NO mode, only for MENU.H */

  #define MODE_START_STOP_STAT        0     /* Command: B_TASKFKT */
  #define MODE_START_STOP             1     /* Command: B_TASKFKT */
  #define MODE_TASK_VERSION           2     /* Command: B_TASKFKT */
  #define MODE_ZYKL_STATUS            3     /* Command: B_TASKFKT */
  #define MODE_SHOW_FUELLST           4     /* Command: B_TASKFKT */
  #define MODE_SHOW_TIMER             5     /* Command: B_TASKFKT */

  #define MODE_DIAG_MEM_READ_SINGLE   0x0   /* Command: B_DIAGNOSE */
  #define MODE_DIAG_MEM_WRITE_SINGLE  0x1   /* Command: B_DIAGNOSE */
  #define MODE_DIAG_IO_READ_SINGLE    0x2   /* Command: B_STRUKTFKT */
  #define MODE_DIAG_IO_WRITE_SINGLE   0x3   /* Command: B_STRUKTFKT */
  #define MODE_DIAG_ZYKL              0x10  /* Command: B_STRUKTFKT */
  #define MODE_DIAG_MEM_READ_ZYKL     0x10  /* Command: B_STRUKTFKT */
  #define MODE_DIAG_MEM_WRITE_ZYKL    0x11  /* Command: B_STRUKTFKT */
  #define MODE_DIAG_IO_READ_ZYKL      0x12  /* Command: B_STRUKTFKT */
  #define MODE_DIAG_IO_WRITE_ZYKL     0x13  /* Command: B_STRUKTFKT */

  #define MODE_ZYKL_TASK_STRUK        0     /* Command: B_STRUKTFKT */
  #define MODE_INIT_INFO_STRUK        1     /* Command: B_STRUKTFKT */
  #define MODE_WRITE_STRUK            2     /* Command: B_STRUKTFKT */

  #define MODE_START_TRACE            0     /* Command: B_TRACE */
  #define MODE_DEL_TRACE_PUF          1     /* Command: B_TRACE */

  #define MODE_UPLOAD_BINAER          0     /* Command: B_LOADFKT */
  #define MODE_DOWNLOAD_BINAER        1     /* Command: B_LOADFKT */
  #define MODE_UPLOAD_DBM             2     /* Command: B_LOADFKT */
  #define MODE_DOWNLOAD_DBM           3     /* Command: B_LOADFKT */
  #define MODE_DEL_FLASH              4     /* Command: B_LOADFKT */
  #define MODE_GET_FLASH_DIR          5     /* Command: B_LOADFKT */
  #define MODE_URLADEN                6     /* Command: B_LOADFKT */
  #define MODE_LONG_BINLOAD           7     /* Command: B_LOADFKT */
  #define MODE_FREE_DRIVER            8     /* Command: B_LOADFKT */
  #define MODE_RESET_DEVICE           10    /* Command: B_LOADFKT */

/* ------------------------------------------------------------------------------------ */
#ifdef _cplusplus
  }
#endif

#endif
//	========  eof 'RcsDef.h'   ========
                   