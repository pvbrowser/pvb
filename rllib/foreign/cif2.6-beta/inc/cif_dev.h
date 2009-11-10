/* <St> *******************************************************************

  cif_dev.h

  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION

    Include file for CIF device driver, DPM layout                 .

  =========================================================================

  CHANGES

  version name      date        Discription
                 March 2001
    		 Juli  2004     Redesigned for the 2.6 Kernel
 		                Copyright changed to GNU Lesser GPL
-------------------------------------------------------------------------

  V2.600

  NOTE: as groundwork for this header served Windows version of 
        the CIF device driver

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
#ifndef  CIF_DEV_H
# define CIF_DEV_H

#ifndef STRICT             /* Check Typdefinition */
   #define STRICT
#endif

#ifndef	_GNUC_
#    define  _GNUC_
#endif	/* _GNUC_ */

/* ---------------------------------------------------------------------------------- */
/* DEV definitions                                                                    */
/* ---------------------------------------------------------------------------------- */

#define DRV_ACTIV_STATE1      2           /* DEV board is in the registery            */
#define DRV_ACTIV_STATE2      TRUE        /* DEV board is tested and ready            */

#define NUM_OF_DRV_NAMES      3           /* Number of possible driver names          */
#define DEVNAMELEN            3           /* Size of DEV name in DPM                  */

#define HW_INTERRUPT_DISABLE  0x00
#define HW_INTERRUPT_ENABLE   0x01

#define BUS_TYPE_PCI          0x01
#define BUS_TYPE_ISA          0x02
/* ------------------------------------------------------------------------------------ */
/* PCI card definitions                                                                 */
/* ------------------------------------------------------------------------------------ */

#define VENDOR_ID      0x10B5          /* PLX technology                                */
#define SUBVENDOR_ID   0x10B5          /* PLX-PCI chip                                  */
#define SUBSYSTEM_ID   0x1080          /* Defines a HILSCHER CIF 50 PCI board           */ 
#define DEVICE_ID      0x9050          /* PLX-PCI chip                                  */

/* ------------------------------------------------------------------------------------ */
/* Compact-PCI card definitions                                                         */
#define CPCI_SUBSYSTEM_ID   0x2695     /* Defines a HILSCHER CIF 50 PCI board           */ 
#define CPCI_DEVICE_ID      0x9030     /* PLX-PCI chip                                  */
/* ------------------------------------------------------------------------------------ */

/* Local Control Register Offsets */
#define PCI_INTCTRLSTS_REG  0x4C

#ifndef CIF_MAJOR
#define CIF_MAJOR 0   /* dynamic major by default */
#endif

#ifndef CIF_MAX_BOARDS
#define CIF_MAX_BOARDS   4  /* Board-0 through Board-3 */
#endif

/* ------------------------------------------------------------------------------------ */
/* DPM structure                                                                        */
/* ------------------------------------------------------------------------------------ */
#ifdef		_GNUC_
#  ifndef	PACKED
#    define	PACKED	__attribute__((packed, aligned(1)))
#  endif	/* PACKED */
#else
#  define	PACKED
#endif

typedef struct tagDPM_MEMORY{
  /* DPM size is flexible, now using pointers      */
  /* for addressing the send and receive area      */
  MSG_STRUC     tDevMbx     PACKED;                /* Mailbox PC  --> DEV                (288 Bytes)*/
  VERSIONINFO   tDevVersion PACKED;                /* DEV version information            ( 32 Bytes)*/
  MSG_STRUC     tPcMbx      PACKED;                /* Mailbox DEV --> PC                 (288 Bytes)*/
  FIRMWAREINFO  tFiwInfo    PACKED;                /* Firmware info                      ( 32 Bytes)*/
  TASKPARAM     tKpt1Param  PACKED;                /* Task 1 interface parameter         ( 64 Bytes)*/
  TASKPARAM     tKpt2Param  PACKED;                /* Task 2 interface parameter         ( 64 Bytes)*/
  TASKSTATE     tKpt1State  PACKED;                /* Task 1 state                       ( 64 Bytes)*/
  TASKSTATE     tKpt2State  PACKED;                /* Task 2 state                       ( 64 Bytes)*/
  TASKINFO      tTaskInfo   PACKED;                /* Task 1 to 7 info field             (112 Bytes)*/
  RCSINFO       tRcsInfo    PACKED;                /* RCS information                    (  8 Bytes)*/
  DEVINFO       tDevInfo    PACKED;                /* DEV information                    (  6 Bytes)*/
  unsigned char HostFlags   PACKED;                /* DPM communication DEV->PC(PcFlags) (  1 Byte )*/
  unsigned char DevFlags    PACKED;                /* DPM communication PC->DEV(CifFlags)(  1 Byte )*/
} DPM_MEMORY;

/* ------------------------------------------------------------------------------------ */
/*   DPM flag definition                                                                */
/* ------------------------------------------------------------------------------------ */

// Host flags (HOST_FLAGS) written by the device
#define HOSTCOM_FLAG            0x01
#define DEVACK_FLAG             0x02
#define PDACK_FLAG              0x04
#define STATECOM_FLAG           0x08
#define SPCACK_FLAG             0x10
#define COM_FLAG                0x20
#define RUN_FLAG                0x40
#define READY_FLAG              0x80

// Device flags (DEV_FLAGS) written by the host 
#define HOSTACK_FLAG            0x01
#define DEVCOM_FLAG             0x02
#define PDCOM_FLAG              0x04
#define STATEACK_FLAG           0x08
#define SPCCOM_FLAG             0x10
#define NOTREADY_FLAG           0x20
#define INIT_FLAG               0x40
#define RESET_FLAG              0x80

/* ------------------------------------------------------------------------------------ */
/*   driver state definitions                                                           */
/* ------------------------------------------------------------------------------------ */

#define INI_MSG_WAIT            0x00
#define INI_MSG_RUN             0x40
#define INI_MSG_RDY             0x80
#define INI_MSG_RDYRUN          0xC0

#define COM_FLAG_RDY            0x20

#define RESET_MSG_NON           0x00
#define RESET_MSG_RUN           0x01

#define EXCHGIO_NON             0x00
#define EXCHGIO_EQUAL           0x01
#define EXCHGIO_NOT_EQUAL       0x02

#ifndef CIF_NR_DEVS
#define CIF_NR_DEVS   4 
#endif

/*
 * Split minors in two parts
 */
#define TYPE(dev)  (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)   (MINOR(dev) & 0xf) /* low  nibble */
/* ------------------------------------------------------------------------------------ */
/* DEV instance structure                                                               */
/* ------------------------------------------------------------------------------------ */
typedef struct tagDEV_INSTANCE {
  struct tagDEV_INSTANCE  *next;               /* linked list of Devices, see CIF_MAX_BOARDS = 4 */ 
  unsigned short        usBoard;               // Internal board number 
  void *                pvIntSynch;            // Interrupt synchronization
  unsigned char         ucPCIBusNumber;        // PCI bus number 
  unsigned char         ucBusType;             // PCI, ISA
  unsigned long         ulBoardAddress;        // physical memory address
  unsigned long         ulDPMSize;             // DPM size of the DEV
  unsigned long         ulDPMByteSize;         // DPM size in bytes
  unsigned short        usBoardIrq;            // Interrupt number of the DEV
  unsigned short        usBoardIrq_scanned;    // Interrupt number of the DEV
  unsigned char *       IrqCtrlRegAddr;        // 
  void *                pvVirtualIrq;          // Virtual Interrupt
  unsigned short        usRcsError;            // DEV-Error during startup
  short                 sDrvInitError;         // DRV-Error during startup
  unsigned char         bActive;               // Board is active
  unsigned char *       pDpmBase;              // Virtual DPM start address
  DPM_MEMORY *          ptDpmAddress;          // Virtual DPM address last kbyte
  unsigned char *       pbDpmSendArea;         // Virtual DPM IO address send area
  unsigned char *       pbDpmReceiveArea;      // Virtual DPM IO address receive area
  unsigned char *       pbHostFlags;           // Pointer to HostFlags (PcFlags)
  unsigned char *       pbDevFlags;            // Pointer to DevFlags  (CifFlags)
  unsigned short        usOpenCounter;         // Number of Applikations
  spinlock_t            mutex;                 // SMP synchronization 
  wait_queue_head_t     pInitSemaphore;
  wait_queue_head_t     pReadSemaphore;
  wait_queue_head_t     pWriteSemaphore;
  wait_queue_head_t     pExIOSemaphore;
  unsigned char         bInitState;            // Driver init state
  unsigned char         bReadState;            // Driver read state
  unsigned char         bWriteState;           // Driver write state
  unsigned char         bExIOEqState;          // Driver exchange IO EQUAL state
  unsigned char         bExIONeqState;         // Driver exchange IO NOT EQUAL state
  unsigned char         bCOMEqState;           // Driver COM state wait on EQUAL
  unsigned char         bCOMNeqState;          // Driver COM state wait on NOT EQUAL
  unsigned char         bInitMsgFlag;          // Init  message activ flag
  unsigned char         bReadMsgFlag;          // Read  message available flag
  unsigned char         bWriteMsgFlag;         // Write message done flag
  unsigned char         bStateFlag;            // State flag
  unsigned char         bExIOFlag;             // Exchange IO flag
  unsigned char         bHostFlags;
  unsigned char         bMyDevFlags;
  unsigned short        usSpecialRcsError;     // DEV-Error during startup
  DRIVERINFO            tStateInfo;
} DEV_INSTANCE;

#endif /* CIF_DEV_H */
