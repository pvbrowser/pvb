/* <St> *******************************************************************

   cif_main.c 
  
  -------------------------------------------------------------------------
  CREATETED     : D. Tsaava,  Hilscher GmbH
  DATE          : 18.07.2000
  PROJEKT       : CIF device driver
  =========================================================================

  DISCRIPTION
    initialization routines               .
  =========================================================================

  CHANGES
  version name      date        Discription
                 March 2001
   		 Juli  2004     Redesigned for the 2.6 Kernel
		                Copyright changed to GNU Lesser GPL
-------------------------------------------------------------------------
  V2.600

  NOTE: as groundwork for this source code served Windows version of 
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

#define MODNAME		"cif: "
#define VERSION		"2.600"


#include <linux/module.h>
#include <linux/moduleparam.h> /* K - 2.6 */

#include <linux/kernel.h>     /* printk() */
#include <linux/slab.h>       /* kmalloc() */
#include <linux/errno.h>      /* error codes */
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>

#include <asm/system.h>       /* cli(), *_flags */

#include "../inc/cif_types.h"
#include "../inc/cif_dev_i.h"  /* local definitions */
#include "../inc/cif_dev.h"   /* local definitions */

/* ---------------------------------------------------------------------- */
/*  External prototypes                                                   */
/* ---------------------------------------------------------------------- */
extern  int              cif_ioctl         ( struct inode *, struct file *, unsigned int , unsigned long );
extern  irqreturn_t      cif_interrupt     ( int , void *, struct pt_regs *);
extern  void             cif_set_irq_state ( unsigned short, DEV_INSTANCE *);
extern  void             cif_test_hardware ( DEV_INSTANCE *);
extern  DEV_INSTANCE    *cif_dev_get_at    ( int n);


DEV_INSTANCE *cif_devices;

// Driver version       1234567890123456
char * pabDrvVersion = "CIFDriver V2.100";

//unsigned int   cif50_region = 0x50;
unsigned char  bData        = 0x00;
unsigned short cif_base     = 0x00;

int cif_major   = CIF_MAJOR;
int cif_nr_devs = 0;// max: CIF_MAX_BOARDS;    /* number of cif devices */

unsigned long           ulRegisterSave   = 0x0UL;
unsigned long           ulIrqCtrlRegSave = 0x0UL;
volatile unsigned char *IrqCtrlRegAddr;

#define PCI_SUB_VENDOR_ID 0x2C
#define PCI_SUB_SYSTEM_ID 0x2E

static u32 addresses[] = {
    PCI_BASE_ADDRESS_0,
    PCI_BASE_ADDRESS_1,
    PCI_BASE_ADDRESS_2,
    PCI_BASE_ADDRESS_3,
    PCI_BASE_ADDRESS_4,
    PCI_BASE_ADDRESS_5,
    0
};
/////////////////////////////////////////////////////////////////////////////
// Global Variables to hold parameters for ISA boards specified at load time
//
#define NUM_PARAMS 4
static int num_params = NUM_PARAMS;
static int dpm_add[NUM_PARAMS]= {0, 0, 0, 0};
static int dpm_len[NUM_PARAMS] = {0, 0, 0, 0};
static short   irq[NUM_PARAMS] = {0, 0, 0, 0};
static char *pci_irq = "no ";

/*
 * "module_param" supports several data types:
 * byte, short, ushort, int, uint, long, ulong, charp,
 * bool, invbool, or XYZ, where you define XYZ and its
 * helper functions.
 */
//module_param_string(pci_irq, string, 3, 0);
module_param(pci_irq, charp, 0);
MODULE_PARM_DESC(pci_irq, "Activate Interrupt for PCI Cards (possible values are: y, n, yes, no. default=no)");
module_param_array(dpm_add, int, num_params, 0);
MODULE_PARM_DESC(dpm_add, "Up to four DPM-Addresses (integer)");
module_param_array(dpm_len, int, num_params, 0);
MODULE_PARM_DESC(dpm_len, "Up to four DPM-Sizes (integer)");
module_param_array(irq, short, num_params, 0);
MODULE_PARM_DESC(irq, "Up to four irq-numbers (short)");
/* K-2.6  
   old way
MODULE_PARM( pci_irq, "s");
MODULE_PARM_DESC( pci_irq, "Activate Interrupt for PCI Cards");
MODULE_PARM( dpm_add, "1-4i");
MODULE_PARM_DESC( dpm_add, "Up to four DPM-Addresses (integer)");
MODULE_PARM( dpm_len, "1-4i");
MODULE_PARM_DESC( dpm_len, "Up to four DPM-Sizes (integer)");
MODULE_PARM(     irq, "1-4h");
MODULE_PARM_DESC( irq, "Up to four irq-numbers (short)");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Tsaava");
MODULE_DESCRIPTION("CIF Device Driver for Hilscher Communication Boards, V2.100");
MODULE_SUPPORTED_DEVICE("cif");
*/
int cif_open (struct inode *inode, struct file *filp)
{
  //int type = TYPE(inode->i_rdev);
    int num  =  NUM(inode->i_rdev);
    DEV_INSTANCE  *dev; /* device information */

    DBG_PRN("CIFdevdrv opened!\n\n");
    /* type 0, check the device number */
    if (num >= cif_nr_devs) return -ENODEV;
    dev = &cif_devices[num];

    /* and use filp->private_data to point to the device data */
    filp->private_data = dev;

    return 0;          /* success */
}

int cif_release (struct inode *inode, struct file *filp)
{
    DBG_PRN("CIFdevdrv closed!\n\n");
    return (0);
}

// Try ISA boards if there was parameters for them passed while loading driver
int cif_init_dev_isa ( int iSa)
{
  DEV_INSTANCE  *dev;

  if ( (dpm_add[iSa] < 0xA0000)  ||
       (dpm_add[iSa] > (0xFFFFF - dpm_len[iSa] * 1024)) ) {
    DBG_PRN("not suitable dpm-address or -length: %d, %d\n", dpm_add[iSa], dpm_len[iSa]);
    return -1;
  } else {
    if( cif_nr_devs == 0) {
      cif_devices = (DEV_INSTANCE *)kmalloc( sizeof (DEV_INSTANCE), GFP_KERNEL);
      if (!cif_devices) 
        return -ENOMEM;
      else {
	memset(cif_devices, 0, sizeof (DEV_INSTANCE));
	cif_nr_devs++;
	dev = (DEV_INSTANCE *)cif_devices;
	dev->next = NULL;  
	DBG_PRN("nr.%d, dev.%p, dev->next %p\n", cif_nr_devs, dev, dev->next);
      }
    }
    else {
      dev = (DEV_INSTANCE *)cif_devices;
      DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
      while ( dev->next) {
	DBG_PRN("nr. %d, dev. %p, dev->next %p\n",  cif_nr_devs, dev, dev->next);
	dev = (DEV_INSTANCE *)dev->next;
	DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
      }
      DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
      dev->next = (DEV_INSTANCE *)kmalloc(sizeof(DEV_INSTANCE), GFP_KERNEL);
      if (!dev->next) 
	return -ENOMEM;
      else {
	memset(dev->next, 0, sizeof(DEV_INSTANCE));
	dev = (DEV_INSTANCE *)dev->next;
	dev->next = NULL;  
	cif_nr_devs++;
      }
    }
    DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
    
    dev->usBoard            = cif_nr_devs <= 0 ? 0 : cif_nr_devs - 1;
    dev->usBoardIrq         = irq[iSa];
    dev->usBoardIrq_scanned = dev->usBoardIrq;  // save it, needed while switching between irq and polling modes
    dev->ulBoardAddress     = dpm_add[iSa];
    dev->ulDPMByteSize      = dpm_len[iSa] * 1024;
    dev->ulDPMSize          = dpm_len[iSa];
    dev->bActive            = TRUE;
    dev->ucBusType          = BUS_TYPE_ISA;
    return 0;
  }
}

int cif_init_dev_pci (struct pci_dev *pDev, unsigned short index, u32 ulDPMByteSize)
{
  DEV_INSTANCE *dev;

  DBG_PRN("ulDPMByteSize = %d\n", ulDPMByteSize);
  if( cif_nr_devs == 0) {
  DBG_PRN("cif_nr_devs = %d\n", cif_nr_devs);
    cif_devices = (DEV_INSTANCE *)kmalloc( sizeof (DEV_INSTANCE), GFP_KERNEL);
    if (!cif_devices)
      return -ENOMEM;
    else {
      memset(cif_devices, 0, sizeof (DEV_INSTANCE));
      cif_nr_devs++;
      dev = (DEV_INSTANCE *)cif_devices;
      dev->next = NULL;
      DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
    }
  }
  else {
  DBG_PRN("cif_nr_devs = %d\n", cif_nr_devs);
    dev = (DEV_INSTANCE *)cif_devices;
    DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
    while ( dev->next) {
      DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
      dev = (DEV_INSTANCE *)dev->next;
      DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
    }
    DBG_PRN("nr. %d, dev. %p, dev->next %p\n", cif_nr_devs, dev, dev->next);
    dev->next = (DEV_INSTANCE *)kmalloc(sizeof(DEV_INSTANCE), GFP_KERNEL);
    if (!dev->next)
      return -ENOMEM;
    else {
      memset(dev->next, 0, sizeof(DEV_INSTANCE));
      dev = (DEV_INSTANCE *)dev->next;
      dev->next = NULL;
      cif_nr_devs++;
    }
  }

  DBG_PRN("nr. %d, dev. %p, dev->next %p, irq %X\n", cif_nr_devs, dev, dev->next, dev->usBoardIrq);
  dev->usBoard            = index;
  dev->ucPCIBusNumber     = pDev->bus->number;
  dev->usBoardIrq         = pDev->irq;
  dev->IrqCtrlRegAddr     = (unsigned char *)IrqCtrlRegAddr;
  dev->usBoardIrq_scanned = dev->usBoardIrq;  // save it, needed while switching between irq and polling modes
  dev->ulBoardAddress     = ulRegisterSave & PCI_BASE_ADDRESS_IO_MASK;
  dev->ulDPMByteSize      = ulDPMByteSize;
  dev->ulDPMSize          = ulDPMByteSize / 1024;
  dev->bActive            = TRUE;
  dev->ucBusType          = BUS_TYPE_PCI;
  DBG_PRN("!!!: regsave: %lX, boardAdd: %lX\n", ulRegisterSave, dev->ulBoardAddress);
  return 0;
}

/*
 * Scan PCI-bus / detect CIF50 boards
 */
int cif_scan_pci (void)
{
  unsigned short usSubVendor, usSubSystem;

#ifdef CONFIG_PCI
    int i = 0;
    u32  curr, mask, ulDPMByteSize = 0x0UL;
    unsigned short index = 0;
    struct pci_dev *pDev = NULL;
      DBG_PRN ("scanning for cif50 Boards . . .\n");
    while ( (pDev = pci_find_device(VENDOR_ID, DEVICE_ID, pDev)) != NULL ) {
      // PLX-Chip found, look for Hilscher one!
      pci_read_config_word(pDev, PCI_SUB_VENDOR_ID, &usSubVendor);
      pci_read_config_word(pDev, PCI_SUB_SYSTEM_ID, &usSubSystem);
      DBG_PRN("subSystem = %X, subVendor = %X\n", usSubSystem, usSubVendor); 

      if( SUBVENDOR_ID == usSubVendor && SUBSYSTEM_ID == usSubSystem && index < 4) {
 	ulDPMByteSize = 0x0UL;

   	DBG_PRN("BAR[]=%x,%x,%x,%x IRQ=%x\n",
		(unsigned short)pci_resource_start(pDev, 0), (unsigned short)pci_resource_start(pDev, 1),
		(unsigned short)pci_resource_start(pDev, 2), (unsigned short)pci_resource_start(pDev, 3), pDev->irq);
 	for(i=0; addresses[i]; i++) {
 	  pci_read_config_dword(pDev, addresses[i],&curr);
	  DBG_PRN("addresses[%d] = %d, curr = %d\n", i, addresses[i], curr);
 	  local_irq_disable(); // cli();
 	  if(i == 2) ulRegisterSave = pci_resource_start (pDev, 2);
 	  else if(i == 1) cif_base = (unsigned short)(pci_resource_start (pDev, 1) & 0xFFFFFFFE);
 	  else if(i == 0) ulIrqCtrlRegSave = pci_resource_start (pDev, 0);
 	  pci_write_config_dword(pDev, addresses[i], ~0);
 	  pci_read_config_dword (pDev, addresses[i], &mask);
 	  pci_write_config_dword(pDev, addresses[i], curr);
 	  local_irq_enable(); // sti();
 	  if(i == 2) ulDPMByteSize = ~(mask & PCI_BASE_ADDRESS_IO_MASK) +1;
 	}
 	DBG_PRN ("PLX-CHIP local Reg. Add. : %x\n", cif_base);
 	if(( IrqCtrlRegAddr = (unsigned char *)ioremap(ulIrqCtrlRegSave, 0x50)) != NULL) {
 	  bData = readb( IrqCtrlRegAddr + PCI_INTCTRLSTS_REG);
 	}
 	DBG_PRN ("Interrupt-ctrl-state read: %x\n", bData);
 	bData = (unsigned char)(bData | HW_INTERRUPT_ENABLE);//(bData & ~HW_INTERRUPT_ENABLE); // Desable Interrupt
 	if( cif_init_dev_pci (pDev, index, ulDPMByteSize) != 0) {
 	  DBG_PRN ("Cannot alloc memory for Board : %x\n", index);
 	  break; // -ENOMEM
 	}
 	else
 	  index++;
       }
    }
    if(index == 0) {
      DBG_PRN ("cif50: no Boards found:\n");
      return -ENODEV;
    }
#endif
    return -ENODEV;
}

/*
 * Scan Compact-PCI-bus / detect CIF80 boards 
 */
int cif_scan_cpci (void)
{
  unsigned short usSubVendor, usSubSystem;

#ifdef CONFIG_PCI
    int i = 0;
    u32  curr, mask, ulDPMByteSize = 0x0UL;
    unsigned short index = 0;
    struct pci_dev *pDev = NULL;
      DBG_PRN ("scanning for cif80 Boards . . .\n");
    while ( (pDev = pci_find_device(VENDOR_ID, CPCI_DEVICE_ID, pDev)) != NULL ) {
      // PLX-Chip found, look for Hilscher one!
      pci_read_config_word(pDev, PCI_SUB_VENDOR_ID, &usSubVendor);
      pci_read_config_word(pDev, PCI_SUB_SYSTEM_ID, &usSubSystem);
      DBG_PRN("subSystem = %X, subVendor = %X\n", usSubSystem, usSubVendor); 

      if( SUBVENDOR_ID == usSubVendor && CPCI_SUBSYSTEM_ID == usSubSystem && index < 4) {
 	ulDPMByteSize = 0x0UL;

   	DBG_PRN("BAR[]=%x,%x,%x,%x IRQ=%x\n",
		(unsigned short)pci_resource_start(pDev, 0), (unsigned short)pci_resource_start(pDev, 1),
		(unsigned short)pci_resource_start(pDev, 2), (unsigned short)pci_resource_start(pDev, 3), pDev->irq);
 	for(i=0; addresses[i]; i++) {
 	  pci_read_config_dword(pDev, addresses[i],&curr);
	  DBG_PRN("addresses[%d] = %d, curr = %d\n", i, addresses[i], curr);
 	  local_irq_disable(); // cli();
 	  if(i == 2) ulRegisterSave = pci_resource_start (pDev, 2);
 	  else if(i == 1) cif_base = (unsigned short)(pci_resource_start (pDev, 1) & 0xFFFFFFFE);
 	  else if(i == 0) ulIrqCtrlRegSave = pci_resource_start (pDev, 0);
 	  pci_write_config_dword(pDev, addresses[i], ~0);
 	  pci_read_config_dword (pDev, addresses[i], &mask);
 	  pci_write_config_dword(pDev, addresses[i], curr);
 	  local_irq_enable(); // sti();
 	  if(i == 2) ulDPMByteSize = ~(mask & PCI_BASE_ADDRESS_IO_MASK) +1;
 	}
 	DBG_PRN ("PLX-CHIP local Reg. Add. : %x\n", cif_base);
 	if(( IrqCtrlRegAddr = (unsigned char *)ioremap(ulIrqCtrlRegSave, 0x50)) != NULL) { 
 	  bData = readb( IrqCtrlRegAddr + PCI_INTCTRLSTS_REG);
 	}
 	DBG_PRN ("Interrupt-ctrl-state read: %x\n", bData);
 	bData = (unsigned char)(bData | HW_INTERRUPT_ENABLE);//(bData & ~HW_INTERRUPT_ENABLE); // Desable Interrupt
 	if( cif_init_dev_pci (pDev, index, ulDPMByteSize) != 0) {
 	  DBG_PRN ("Cannot alloc memory for Board : %x\n", index);
 	  break; // -ENOMEM
 	}
 	else
 	  index++;
       }
    }
    if(index == 0) {
      DBG_PRN ("cif80: no Boards found:\n");
      return -ENODEV;
    }
#endif
    return -ENODEV;
}

/* <ST> =================================================================================
  Function: cif_drv_load
            Device Driver Load
  ---------------------------------------------------------------------------------------
  Input   :
  Output  :
  Return  : bSuccess  TRUE   - at least one board is initialized
                      FALSE  - no board is initialized
  ================================================================================= <En> */

BOOL cif_drv_load ( void)
{
  int            result = 0;
  BOOL           bSuccess = FALSE; // no board available
  unsigned short usIdx = 0;
  DEV_INSTANCE * dev = NULL;
  // Driver is loaded for the very first time
  DBG_PRN("Boards altogether: %X\n", cif_nr_devs);

  // get memory for global driver data, delete content
  if ( cif_nr_devs > 0) {
      for ( usIdx = 0; usIdx < cif_nr_devs/*CIF_MAX_BOARDS*/; usIdx++) {
        DBG_PRN("Install instance Number = %d\n", usIdx);
	dev = cif_dev_get_at( usIdx); 
	DBG_PRN("Board %d, dev: %p\n", usIdx, dev);

        if ( dev->bActive == TRUE) {
              DBG_PRN("DPM Adresses AVAILABLE,  Board - %X, DPMsize: %lX\n", usIdx, dev->ulDPMByteSize);
              DBG_PRN("Physical-> DPMBase: %lX, DPMAdd: %lX, SndAdd: %lX, RcvAdd: %lX\n", 
		      dev->ulBoardAddress, 
		      dev->ulBoardAddress + dev->ulDPMByteSize - 1024, 
		      dev->ulBoardAddress, 
		      dev->ulBoardAddress + (dev->ulDPMByteSize - 1024)/2);

          if ( (dev->pDpmBase = (unsigned char *)bus_to_virt(dev->ulBoardAddress)) == NULL) { 
             DBG_PRN("No virtual DPM base address\n");
             // no virtual board address is allocated
             dev->bActive = FALSE;

          // get handle to message area, last Kbyte of the DPM
          } else if ( (dev->ptDpmAddress = (DPM_MEMORY *)ioremap(dev->ulBoardAddress + 
							         dev->ulDPMByteSize - 1024, 1024)) == NULL) { 
             DBG_PRN("No virtual DPM address for last DPM Kbyte\n");
             // no virtual board address is allocated
              dev->bActive = FALSE;

          // get handle to send data area (start of DPM)
          } else if ( (dev->pbDpmSendArea = (unsigned char *)ioremap(dev->ulBoardAddress,
								     (dev->ulDPMByteSize - 1024) / 2)) == NULL) { 
            DBG_PRN("No virtual DPM address for send data area\n");
            // no virtual board address is allocated
            dev->bActive = FALSE;

          // get handle to receive data area (middle of the DPM)
          } else if ( (dev->pbDpmReceiveArea = (unsigned char *)ioremap(dev->ulBoardAddress + 
									(dev->ulDPMByteSize - 1024) / 2 , 
									(dev->ulDPMByteSize - 1024) / 2)) == NULL) { 
            DBG_PRN("No virtual DPM address for receive data area\n");
            // no virtual board address is allocated
            dev->bActive = FALSE;

          // install interrupt while IRQ number not 0
          } else {
              DBG_PRN("DPM Adresses AVAILABLE\n");
              DBG_PRN("Remapped-> DPMBase: %lX, DPMAdd: %lX, SndAdd: %lX, RcvAdd: %lX\n", 
		      (unsigned long)dev->pDpmBase, 
		      (unsigned long)dev->ptDpmAddress, 
		      (unsigned long)dev->pbDpmSendArea, 
		      (unsigned long)dev->pbDpmReceiveArea);

            // all parameters successfully read. Initialize instance No. usIdx
            dev->usRcsError    = 0;  // RCS-Error during startup
            dev->sDrvInitError = 0;  // DRV-Error during startup
            // pointers to the DEV dpm cells
            dev->pbHostFlags = &(dev->ptDpmAddress->HostFlags);
            dev->pbDevFlags  = &(dev->ptDpmAddress->DevFlags);
            // initialize start states
            dev->usOpenCounter  = 0;
            dev->bInitMsgFlag   = INI_MSG_WAIT;       // Init state
            dev->bReadMsgFlag   = HOST_MBX_EMPTY;     // Read state
            dev->bWriteMsgFlag  = DEVICE_MBX_EMPTY;   // write state
            dev->bExIOFlag      = EXCHGIO_NON;        // IO state
            dev->bStateFlag     = STATE_ERR_NON;      // State field

	    init_waitqueue_head( &dev->pInitSemaphore);
	    init_waitqueue_head( &dev->pReadSemaphore);
	    init_waitqueue_head( &dev->pWriteSemaphore);
	    init_waitqueue_head( &dev->pExIOSemaphore);
	    spin_lock_init     (&dev->mutex);

	    // request IRQ, register interrupt handler and BHs
	    if (dev->usBoardIrq != 0) {
	      //free_irq(dev->usBoardIrq, dev);
	      if( dev->ucBusType == BUS_TYPE_PCI) {
		  if(strncmp(pci_irq, "y", 1) == 0) {
		    DBG_PRN("Board %d, activating irq %d\n", usIdx, dev->usBoardIrq);
		    result = request_irq( (unsigned int)(dev->usBoardIrq), cif_interrupt, SA_SHIRQ, "cif", dev);
		    cif_set_irq_state( HW_INTERRUPT_ENABLE, dev);
		  }
		  else {/* poll */
		    DBG_PRN("Board %d, disabling irq %d\n", usIdx, dev->usBoardIrq);
		    cif_set_irq_state( HW_INTERRUPT_DISABLE, dev);
		    result = -EBUSY;
		  }
	      }
	      else // ISA bus supports no irq sharing
		result = request_irq( (unsigned int)(dev->usBoardIrq), cif_interrupt, 0, "cif", dev);
	      if (result) {
		DBG_PRN("can't get assigned irq %i\n", dev->usBoardIrq);
		dev->usBoardIrq = 0;
	      }
	      else {
		DBG_PRN("interrupt handler registration OK. Board = %d, dev = 0x%p, request_irq() = %d\n", usIdx, dev, result);
		if( dev->ucBusType == BUS_TYPE_PCI)
		  cif_set_irq_state( HW_INTERRUPT_ENABLE, dev);
	      }
	    }
            // board is configured, test hardware
            cif_test_hardware( dev);

            // virtual board address is allocated
            DBG_PRN("V-IRQ                     = %p \n", dev->pvVirtualIrq );
            DBG_PRN("V-DPM base address        = %p \n", dev->pDpmBase );
            DBG_PRN("V-DPM send area address   = %p \n", dev->pbDpmSendArea);
            DBG_PRN("V-DPM recive area address = %p \n", dev->pbDpmReceiveArea);
            DBG_PRN("V-DPM last kByte          = %p \n", dev->ptDpmAddress);

            DBG_PRN("Initialisation OK\n");

            // one board is initialized
            bSuccess = TRUE;
        }
      }  // end for
    }
  }

  DBG_PRN("Leave %d \n", bSuccess);

  if( !bSuccess) {
    iounmap( dev->ptDpmAddress);
    iounmap( dev->pbDpmReceiveArea);
    iounmap( dev->pbDpmReceiveArea);
  }
  return bSuccess;
}

/* <ST> =================================================================================

  Function: cif_drv_unload
            Device Driver Unload
  ---------------------------------------------------------------------------------------

  Input   :

  Output  :

  Return  :

  ================================================================================= <En> */

BOOL cif_drv_unload (void)
{
  DEV_INSTANCE *  dev = cif_devices;

  DBG_PRN("unloading driver\n");
  // OS wants to unload driver
  while (dev) {               // deallocate memory, free irq
    if ( dev->usBoardIrq) {
      DBG_PRN("free Irq %d for board %d\n", dev->usBoardIrq, dev->usBoard);
      // Disable physical interrupt on the hardware
      cif_set_irq_state( HW_INTERRUPT_DISABLE, dev);
      free_irq( (unsigned int)(dev->usBoardIrq), dev);
    }
    DBG_PRN("unmap  pDPMBase\n");
    if ( ( dev->bActive  == TRUE) &&
	 ( dev->pDpmBase != NULL)    ) {
      iounmap( dev->ptDpmAddress);
      iounmap( dev->pbDpmReceiveArea);
      iounmap( dev->pbDpmSendArea);
    }
    dev = (DEV_INSTANCE *)dev->next;
  }

  DBG_PRN("Cleanup done!\n");

  return (TRUE); // always TRUE if driver is static
}


ssize_t cif_read (struct file *filp, char *buf, size_t count,
                loff_t *f_pos)
{
  //__copy_to_user(...);  memcpa_tofs(...)
    return (0);
}

ssize_t cif_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    return (-ENOMEM);
}

/*
 * The "extended" operations -- only seek
 */

loff_t cif_llseek (struct file *filp, loff_t off, int whence)
{
  //    DEV_INSTANCE *dev = filp->private_data;
  loff_t newpos;

  switch(whence) {
  case 0: /* SEEK_SET */
    newpos = off;
    break;

  case 1: /* SEEK_CUR */
    newpos = filp->f_pos + off;
    break;


  default: /* can't happen */
    return -EINVAL;
  }
  if (newpos<0) return -EINVAL;
  filp->f_pos = newpos;
  return newpos;
}

struct file_operations cif_fops = {
    llseek:     cif_llseek,
    read:       cif_read,
    write:      cif_write,
    ioctl:      cif_ioctl,
    open:       cif_open,
    flush:      NULL,
    release:    cif_release,
};

static int __init cif_init(void)
{
  int res = 0;

  printk (MODNAME "loaded: {v." VERSION "}\n");

  /* scan compact-pci bus for Hilscher boards */
  res = cif_scan_cpci();
  if( res != -ENODEV)
    DBG_PRN("%x CIF80-Board(s) detected\n", res);

  /* scan pci bus for Hilscher boards */
  res = cif_scan_pci();
  if( res != -ENODEV)
    DBG_PRN("%x CIF50-Board(s) detected\n", res);

  // try for Hilscher ISA boards, if there was parameter passed
  for(res = 0; res < MAX_DEV_BOARDS; res++) {
    if( cif_nr_devs < MAX_DEV_BOARDS && dpm_add[res] != 0)
      cif_init_dev_isa( res);
    else
      break;
  }

  res = 0;

  if(cif_nr_devs == 0) {
    return -ENODEV;
  }
  else {
    /* Register major, and accept a dynamic number */
    res = register_chrdev(cif_major, "cif", &cif_fops);
    if (res < 0) {
      DBG_PRN("can't get major %d\n",cif_major);
      return res;
    }
    if (cif_major == 0) cif_major = res; /* dynamic */
    if( cif_drv_load())
      return 0; /* succeed */
    else {
      unregister_chrdev(cif_major, "cif");
      return -ENOMEM;
    }
  }
}

static void __exit cif_cleanup(void)
{
  DEV_INSTANCE  *dev = NULL, *next = NULL;

  if(cif_nr_devs != 0) {
    cif_drv_unload();
    for  ( dev = cif_devices; dev; dev = next) { /* all items of the linked list */
      if ( dev) {
	DBG_PRN("free mem. dev %p, dev->next %p\n", dev, dev->next);
	next = (DEV_INSTANCE *)dev->next;
	kfree( dev);
	dev = NULL;
      }
    } 
  }   

  unregister_chrdev(cif_major, "cif");

  DBG_PRN("clean up finished!\n\n");
}

module_init(cif_init);
module_exit(cif_cleanup);

MODULE_AUTHOR("David Tsaava");
MODULE_ALIAS ("cif");
MODULE_LICENSE("GPL and additional rights");
MODULE_DESCRIPTION("CIF Device Driver for Hilscher Communication Boards");
MODULE_VERSION (VERSION);
MODULE_SUPPORTED_DEVICE("/dev/cif");

