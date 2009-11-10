/* <St> *******************************************************************

  cif_irq.c

  -------------------------------------------------------------------------
  CREATETED     : D.Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION
    interrupt routine, resource management               .
  =========================================================================

  CHANGES
  version name      date        Discription
                 March 2001
  -------------------------------------------------------------------------
  V2.100
  
  NOTE: as groundwork for this source code served Windows version of 
        the CIF device driver

  ======================== Copyright =====================================
  Complete package is copyrighted by Hilscher GmbH and is licensed through 
  the GNU General Public License. 
  You should have received a copy of the GNU Library General Public
  License along with this package; if not, please refer to www.gnu.org
  ========================================================================
******************************************************************** <En> */

/* ------------------------------------------------------------------------------------ */
/*  Includes                                                                            */
/* ------------------------------------------------------------------------------------ */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/kernel.h>   /* printk() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/sched.h>

#include <asm/io.h>

#include "../inc/cif_types.h"
#include "../inc/cif_dev_i.h"
#include "../inc/cif_dev.h"

/* ------------------------------------------------------------------------------------ */
/*  External data                                                                      */
/* ------------------------ ------------------------------------------------------------ */

extern int             cif_nr_devs;
extern DEV_INSTANCE   *cif_devices;
extern unsigned char   bData;

/*
 * pick the device 
 */
DEV_INSTANCE *cif_dev_get_at(unsigned short n)
{
  DEV_INSTANCE *dev = cif_devices;
  if(n >= cif_nr_devs) dev = NULL;
  else {
    while (n--) {
      dev = (DEV_INSTANCE *)dev->next;
    }
  }
  return dev;
}

/* <ST> =================================================================================
  Function: cif_clear_board_irq
            Clear all pending board interrupts
  ---------------------------------------------------------------------------------------
  Input   : -
  Output  : -
  Return  : -
  ================================================================================= <En> */

void  cif_clear_board_irq(void)
{
  unsigned short usIdx;
  unsigned char  bDummy;
  DEV_INSTANCE   * dev;

  for ( usIdx = 0; usIdx < MAX_DEV_BOARDS; usIdx++) {
    dev = cif_dev_get_at( usIdx);

    if ( (dev->pvVirtualIrq != NULL) &&
         (dev->bActive == TRUE) ) {

      DBG_PRN("(%p)\n", dev);
      bDummy = *(dev->pbHostFlags);
    }
  }
}

/* ==================================================================================== */
/* Interrupt handler                                                                    */
/* ==================================================================================== */
static void cif_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
  DEV_INSTANCE   * dev = cif_devices;
  unsigned char    bHostFlags;
  BOOL fIntFound = FALSE;            

  DBG_PRN( "params: irq %d, dev_id %p. cif_devices %p\n", irq, dev_id, dev);

  if (!dev_id) {
    DBG_PRN( "irq with NULL dev_id: %d\n", irq);
    return;
  }
  // Did one of CIF cards generate the interrupt?
  while (dev) {
    if (dev == dev_id)
      break;
    dev = (DEV_INSTANCE *)dev->next;
  }
  // impossible - unless we add the device to our list after both
  // registering the IRQ handler for it and enabling interrupts, AND
  // the card generates an IRQ at startup - should not happen again
  if (!dev) {
    DBG_PRN( "irq for unknown device: %d\n", irq);
    return;
  }
  // do we have memory corruption of dev or kernel?
  if (irq != ((int) dev->usBoardIrq) ) {
    DBG_PRN ("irq mismatch:  %d, irq of CIF card: %d\n", irq, (int) dev->usBoardIrq);
    return;
  }
  
  DBG_PRN("dev: 0x%p, irq: %d\n", dev, irq);

  /* Interrupt context can use plain spin_lock() form because */ 
  /* interrupts are disabled inside an interrupt handler.     */
  spin_lock (&dev->mutex);

  // Clear device interrupt by reading the host flags
  bHostFlags = *(dev->pbHostFlags);
  DBG_PRN("proper irq found! instance address (%p)\n", dev);
  // Interrupt found
  fIntFound = TRUE;
  //--------------------
  // Get state of DEV 
  //--------------------
  dev->bHostFlags = bHostFlags;          // only for information
  dev->tStateInfo.IRQCnt++;              // increment irq counter

  if ( (bHostFlags & INI_MSG_RDY) == 0x00) { /* Reset on DEV is running */
    DBG_PRN("Reset on DEV is running");
    dev->bMyDevFlags    = 0x00;
    dev->bReadMsgFlag   = HOST_MBX_EMPTY;
    dev->bWriteMsgFlag  = DEVICE_MBX_FULL;
    dev->bInitMsgFlag   = INI_MSG_WAIT;
    dev->bExIOFlag      = EXCHGIO_NON;
    dev->bStateFlag     = STATE_ERR_NON;
  } else {
    if ( dev->bInitMsgFlag == INI_MSG_WAIT ) {
      // get actual init state
      dev->bInitMsgFlag = (unsigned char)(bHostFlags & INI_MSG_RDYRUN);
    } /* endif */
    DBG_PRN("bInitMsgFlag  (%d)\n", dev->bInitMsgFlag);
    // check if message is available
    if ( ((bHostFlags ^ dev->bMyDevFlags) & HOSTCOM_FLAG) != 0x00 ) {
      /* ACK-bit DEV != COM-bit PC   --> message inside */
      dev->bReadMsgFlag = HOST_MBX_FULL;
    } /* endif */
    DBG_PRN("bReadMsgFlag  (%d)\n", dev->bReadMsgFlag);
    // check if mailbox to DEV is free
    if ( ((bHostFlags ^ dev->bMyDevFlags) & DEVACK_FLAG) == 0x00 ) {
      /* ACK-bit DEV  ==  COM-bit PC --> MBX emty */
      dev->bWriteMsgFlag = DEVICE_MBX_EMPTY;
    } /* endif */
    DBG_PRN("bWriteMsgFlag (%d)\n", dev->bWriteMsgFlag);
    // check if IO state equal or not equal
    if ( ((bHostFlags ^ dev->bMyDevFlags) & PDACK_FLAG) == 0x00 ) {
      /* ACK-bit DEV  ==  COM-bit HOST */
      dev->bExIOFlag = EXCHGIO_EQUAL;
    } else {
      /* ACK-bit DEV  !=  COM-bit HOST */
      dev->bExIOFlag = EXCHGIO_NOT_EQUAL;
    } /* endif */
    // check for State 
    if ( ((bHostFlags ^ dev->bMyDevFlags) & STATEACK_FLAG) == 0x00 ) {
      /* ACK-bit DEV  ==  COM-bit PC --> No error */
      dev->bStateFlag = STATE_ERR_NON;
    } else {
      /* ACK-bit DEV  ==  COM-bit PC --> No error */
      dev->bStateFlag = STATE_ERR;
    } /* endif */
  } /* endif */
  //--------------------------------
  // Check application 
  //--------------------------------
  // application must be logged in
  if (dev->usOpenCounter > 0) {
    //******************************************************************
    // check for read data
    //******************************************************************
    if ( (dev->bReadState == TRUE)       &&       // application is waiting
	 (dev->bReadMsgFlag != HOST_MBX_EMPTY)  ) {
      // Applikation is waiting on read service, MSG is available
      dev->tStateInfo.ReadState   = STATE_IN_IRQ;
      switch (dev->bReadMsgFlag) {
      case HOST_MBX_EMPTY:           // MBX is empty
	/* Nothing to do */
	DBG_PRN("ReadState = HOST_MBX_EMPTY\n");
	break;
      case HOST_MBX_SYSERR:        // System ERROR
	DBG_PRN("ReadState = HOST_MBX_SYSERR\n");
	// lets run applikation, waiting on semaphore
	dev->bReadState = FALSE;
	wake_up_interruptible( &(dev->pReadSemaphore));
	break;
      case HOST_MBX_FULL:          /* Message available */
	DBG_PRN("ReadState = HOST_MBX_FULL\n");
	// lets run applikation, waiting on semaphore
	dev->bReadState = FALSE;
	wake_up_interruptible( &(dev->pReadSemaphore));
	break;
      } /* endswitch */
    } /* endif */
    //******************************************************************
    // check for write data
    //******************************************************************
    if ( (dev->bWriteState == TRUE)       &&
	 (dev->bWriteMsgFlag == DEVICE_MBX_EMPTY)  ) {
      // write is ready, start waiting applikation
      dev->tStateInfo.WriteState = STATE_IN_IRQ;
      dev->bWriteState = FALSE;
      wake_up_interruptible( &(dev->pWriteSemaphore));
    } /* endif */
  
    //******************************************************************
    // check for EXCHGIO state
    //******************************************************************
  
    if ( dev->bExIOEqState == TRUE ) {
      if (dev->bExIOFlag == EXCHGIO_EQUAL) {
	dev->bExIOEqState = FALSE;
	wake_up_interruptible( &(dev->pExIOSemaphore));
	// DBG_PRN("ExIOEqState = EXIO_EQUAL\n");
      }
    }
    if ( dev->bExIONeqState == TRUE ) {
      if (dev->bExIOFlag == EXCHGIO_NOT_EQUAL) {
	dev->bExIONeqState = FALSE;
	wake_up_interruptible( &(dev->pExIOSemaphore));
	DBG_PRN("ExIOEqState = EXIO_EQUAL\n");
      }
    }
    //******************************************************************
    // check for COM state
    //******************************************************************
  
    if ( dev->bCOMEqState == TRUE ) {
      if ( (dev->bHostFlags & COM_FLAG_RDY) == 0) {
	dev->bCOMEqState = FALSE;
	wake_up_interruptible( &(dev->pInitSemaphore));
	DBG_PRN("INIT InitMsgFlag = (%x)\n",dev->bInitMsgFlag);
      }
    }
    if ( dev->bCOMNeqState == TRUE ) {
      if ( (dev->bHostFlags & COM_FLAG_RDY) != 0) {
	dev->bCOMNeqState = FALSE;
	wake_up_interruptible( &(dev->pInitSemaphore));//up( &(dev->pInitSemaphore));
	DBG_PRN("INIT InitMsgFlag = (%x)\n",dev->bInitMsgFlag);
      }
    }
    //******************************************************************
    // check for init state
    //******************************************************************
    
    if ( dev->bInitState == TRUE ) {
      if ( dev->bInitMsgFlag != INI_MSG_WAIT) {
	// reset is gone, start waiting application
	dev->bInitState = FALSE;
	wake_up_interruptible( &(dev->pInitSemaphore));
	DBG_PRN("INIT InitMsgFlag = (%x)\n",dev->bInitMsgFlag);
      }
    } /* endif */
  }/* endif */
  
  DBG_PRN("H %u ", bHostFlags);
  DBG_PRN("D %u ", *(dev->pbDevFlags));
  DBG_PRN("M %u\n", dev->bMyDevFlags);

  if ( fIntFound == FALSE) {
    // No interrupt found or initialisation is running,
    // clear all board Interrupts by reading the hardware
    DBG_PRN("Phys Int ERROR, instance not found\n");
    // Delete interrupt by reading the hardware
    cif_clear_board_irq();
  }
  spin_unlock (&dev->mutex);
}

/* <ST> =================================================================================
  Function: cif_set_irq_state
            Enabable /Disable interrupt for PCI cards
  ---------------------------------------------------------------------------------------
  Input   : DeviceObject -
  Output  : -
  Return  : -
  ================================================================================= <En> */

void cif_set_irq_state( unsigned short usIntState, DEV_INSTANCE  *dev)
{
  unsigned char  bData;

  if(dev && (dev->ucBusType == BUS_TYPE_PCI) ) {
    // Check if PCI board and Interrupt requested
    if ( dev->usBoardIrq != 0 )  { 
      DBG_PRN("ulIOLocalRegAddress = 0x%p\n", dev->IrqCtrlRegAddr);
      bData = readb( dev->IrqCtrlRegAddr + PCI_INTCTRLSTS_REG);
      DBG_PRN("original bData(0x%X)\n", bData);
      if ( usIntState == HW_INTERRUPT_ENABLE) {
	// Enable Interrupt (LCR-Registers)
	bData = (unsigned char)(bData | HW_INTERRUPT_ENABLE);
	DBG_PRN("bData(0x%X)\n", bData);
	DBG_PRN("INTERRUPT IS ENABLED\n");
      } else {
	bData = (unsigned char)(bData & ~HW_INTERRUPT_ENABLE); // Desable Interrupt
	// Disable PCI Interrupt (LCR-Registers)
	DBG_PRN("bData(0x%X)\n", bData);
	DBG_PRN("INTERRUPT WILL BE DISABLED\n");
      }
      // Write new data to port
      writeb(bData, dev->IrqCtrlRegAddr +  PCI_INTCTRLSTS_REG);
    }
  }
  else
    DBG_PRN("could not get device pointer\n");
}

int cif_register_irq( DEV_INSTANCE *dev)
{
  int result = 0;

  DBG_PRN("dev %p, irq = %X\n", dev, dev->usBoardIrq);
  /* extern int request_irq ( unsigned int,
                              void (*handler)(int, void *, struct pt_regs *),
			      unsigned long, const char *, void *);
     extern void free_irq(unsigned int, void *); */
  result = request_irq( (unsigned int)(dev->usBoardIrq), cif_interrupt, SA_SHIRQ, "cif", dev);
  if (result) {
    dev->usBoardIrq = 0;
    DBG_PRN ( "Cant request interrupt on device cif: IRQ = %d, request_irq() Ret = %d\n",
	      result, dev->usBoardIrq );
  }
  else {
    DBG_PRN ( "request interrupt on device cif: IRQ = %d, Ret = %d\n",
	      result, dev->usBoardIrq );
    cif_set_irq_state( HW_INTERRUPT_ENABLE, dev);
  }
  return result;
}

/* <ST> =================================================================================

  Function: cif_get_msg_status

  ---------------------------------------------------------------------------------------
  Input   : dev  - pointer to DEV_INSTANCE
  Output  : -
  Return  : -
  ================================================================================= <En> */

void  cif_get_msg_status( DEV_INSTANCE * dev, unsigned char InByte)
{
  //--------------------
  // Get state of DEV 
  //--------------------
  dev->bHostFlags = InByte;              // only for information
  dev->tStateInfo.IRQCnt++;              // increment irq counter

  if ( (InByte & INI_MSG_RDY) == 0x00) {
    /* Reset on DEV is running */
    DBG_PRN("Reset on DEV is running");
    dev->bMyDevFlags    = 0x00;
    dev->bReadMsgFlag   = HOST_MBX_EMPTY;
    dev->bWriteMsgFlag  = DEVICE_MBX_FULL;
    dev->bInitMsgFlag   = INI_MSG_WAIT;
    dev->bExIOFlag      = EXCHGIO_NON;
    dev->bStateFlag     = STATE_ERR_NON;
  } else {
    if ( dev->bInitMsgFlag == INI_MSG_WAIT ) {
      // get actual init state
      dev->bInitMsgFlag = (unsigned char)(InByte & INI_MSG_RDYRUN);
    } /* endif */

    DBG_PRN("bInitMsgFlag  (%d)\n", dev->bInitMsgFlag);

    // check if message is available
    if ( ((InByte ^ dev->bMyDevFlags) & HOSTCOM_FLAG) != 0x00 ) {
      /* ACK-bit DEV != COM-bit PC   --> message inside */
      dev->bReadMsgFlag = HOST_MBX_FULL;
    } /* endif */

    DBG_PRN("bReadMsgFlag  (%d)\n", dev->bReadMsgFlag);

    // check if mailbox to DEV is free
    if ( ((InByte ^ dev->bMyDevFlags) & DEVACK_FLAG) == 0x00 ) {
      /* ACK-bit DEV  ==  COM-bit PC --> MBX emty */
      dev->bWriteMsgFlag = DEVICE_MBX_EMPTY;
    } /* endif */

    DBG_PRN("bWriteMsgFlag (%d)\n", dev->bWriteMsgFlag);

    // check if IO state equal or not equal
    if ( ((InByte ^ dev->bMyDevFlags) & PDACK_FLAG) == 0x00 ) {
      /* ACK-bit DEV  ==  COM-bit HOST */
      dev->bExIOFlag = EXCHGIO_EQUAL;
    } else {
      /* ACK-bit DEV  !=  COM-bit HOST */
      dev->bExIOFlag = EXCHGIO_NOT_EQUAL;
    } /* endif */

    // check for State 
    if ( ((InByte ^ dev->bMyDevFlags) & STATEACK_FLAG) == 0x00 ) {
      /* ACK-bit DEV  ==  COM-bit PC --> No error */
      dev->bStateFlag = STATE_ERR_NON;
    } else {
      /* ACK-bit DEV  ==  COM-bit PC --> No error */
      dev->bStateFlag = STATE_ERR;
    } /* endif */
  } /* endif */

  //--------------------------------
  // Check application 
  //--------------------------------
  // application must be logged in
  if (dev->usOpenCounter > 0) {

    //******************************************************************
    // check for read data
    //******************************************************************

    if ( (dev->bReadState == TRUE)       &&       // application is waiting
         (dev->bReadMsgFlag != HOST_MBX_EMPTY)  ) {

      // Applikation is waiting on read service, MSG is available
      dev->tStateInfo.ReadState   = STATE_IN_IRQ;

      switch (dev->bReadMsgFlag) {
      case HOST_MBX_EMPTY:           // MBX is empty
         /* Nothing to do */
         DBG_PRN("ReadState = HOST_MBX_EMPTY\n");
         break;
      case HOST_MBX_SYSERR:        // System ERROR
         DBG_PRN("ReadState = HOST_MBX_SYSERR\n");
         // lets run applikation, waiting on semaphore
         dev->bReadState = FALSE;
	 wake_up_interruptible( &(dev->pReadSemaphore));
         break;
      case HOST_MBX_FULL:          /* Message available */
        DBG_PRN("ReadState = HOST_MBX_FULL\n");
        // lets run applikation, waiting on semaphore
        dev->bReadState = FALSE;
	wake_up_interruptible( &(dev->pReadSemaphore));
        break;
      } /* endswitch */
    } /* endif */

    //******************************************************************
    // check for write data
    //******************************************************************

    if ( (dev->bWriteState == TRUE)       &&
         (dev->bWriteMsgFlag == DEVICE_MBX_EMPTY)  ) {
       // write is ready, start waiting applikation
       dev->tStateInfo.WriteState = STATE_IN_IRQ;
       dev->bWriteState = FALSE;
       wake_up_interruptible( &(dev->pWriteSemaphore));
    } /* endif */

    //******************************************************************
    // check for EXCHGIO state
    //******************************************************************

    if ( dev->bExIOEqState == TRUE ) {
      if (dev->bExIOFlag == EXCHGIO_EQUAL) {
        dev->bExIOEqState = FALSE;
        wake_up_interruptible( &(dev->pExIOSemaphore));
      }
    }
    if ( dev->bExIONeqState == TRUE ) {
      if (dev->bExIOFlag == EXCHGIO_NOT_EQUAL) {
        dev->bExIONeqState = FALSE;
        wake_up_interruptible( &(dev->pExIOSemaphore));
        DBG_PRN("ExIOEqState = EXIO_EQUAL\n");
      }
    }
    //******************************************************************
    // check for COM state
    //******************************************************************

    if ( dev->bCOMEqState == TRUE ) {
      if ( (dev->bHostFlags & COM_FLAG_RDY) == 0) {
          dev->bCOMEqState = FALSE;
          wake_up_interruptible( &(dev->pInitSemaphore));
	  DBG_PRN("INIT InitMsgFlag = (%x)\n",dev->bInitMsgFlag);
      }
    }
    if ( dev->bCOMNeqState == TRUE ) {
      if ( (dev->bHostFlags & COM_FLAG_RDY) != 0) {
          dev->bCOMNeqState = FALSE;
          wake_up_interruptible( &(dev->pInitSemaphore));
        DBG_PRN("INIT InitMsgFlag = (%x)\n",dev->bInitMsgFlag);
      }
    }
    //******************************************************************
    // check for init state
    //******************************************************************

    if ( dev->bInitState == TRUE ) {
      if ( dev->bInitMsgFlag != INI_MSG_WAIT) {
        // reset is gone, start waiting application
        dev->bInitState = FALSE;
        wake_up_interruptible( &(dev->pInitSemaphore));
        DBG_PRN("INIT InitMsgFlag = (%x)\n",dev->bInitMsgFlag);
      }
    } /* endif */
  }/* endif */

  DBG_PRN("H %x ", InByte);
  DBG_PRN("D %x ", *(dev->pbDevFlags));
  DBG_PRN("M %x\n", dev->bMyDevFlags);
}
