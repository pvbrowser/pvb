/*======================================================================

    A Hilscher CIF driver for fieldbus cards

    This driver supports the Hilscher PCMCIA CIF60 Card.
    Written by R. Lehrig, lehrig@t-online.de

    Written 2005.

    This code is based on pcnet_cs.c from David Hinds.
    
    V1.0.0 March 2005 - R. Lehrig lehrig@t-online.de

    This code now only contains code required for the Card Services.
    All we do here is set the card up enough so that the real cif_main.c
    driver can find it and work with it properly.

    i.e. We set up the io port, irq, mmio memory and shared ram
    memory.  This enables cif_init in cif_main.c to find the card and
    configure it as though it was a normal ISA and/or PnP card.

    CHANGES

    v1.0.0 March 2005 R. Lehrig (lehrig@t-online.de)
    First version and testing of driver using http://pvbrowser.org
    
======================================================================*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/trdevice.h>

#include <pcmcia/version.h>
#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>

#define PCMCIA
#include "cif_main.c"

#define PCMCIA_DEBUG 100
#ifdef PCMCIA_DEBUG
static int pc_debug = PCMCIA_DEBUG;
//MODULE_PARM(pc_debug, "i");
#define DEBUG(n, args...) if (pc_debug>(n)) printk(KERN_DEBUG args)
#else
#define DEBUG(n, args...)
#endif

/*====================================================================*/

/* Parameters that can be set with 'insmod' */

/* Bit map of interrupts to choose from */
//static u_int irq_mask = 0x00;
//static int irq_list[4] = { -1 };

/* SRAM base address */
static u_long srambase = 0xd0000;

/* SRAM size 8,16,32,64 */
static u_long sramsize = 8;

//MODULE_PARM(irq_mask, "i");
//MODULE_PARM(irq_list, "1-4i");
//MODULE_PARM(srambase, "i");
//MODULE_PARM(sramsize, "i");
MODULE_LICENSE("GPL");

/*====================================================================*/
typedef struct
{
  u_long  base_addr;
  u_long  sram_base;
  u_long  sram_virt;
  void   *mmio;
  int     irq;
  char    name[32];
} CARD_DEVICE;

static void cif60_config(dev_link_t *link);
static void cif60_release(dev_link_t *link);
static int  cif60_event(event_t event, int priority,  event_callback_args_t *args);

static dev_info_t dev_info = "cif_cs";

static dev_link_t *cif60_attach(void);
static void cif60_detach(dev_link_t *);

static dev_link_t *dev_list;

extern irqreturn_t cif_interrupt(int irq, void *dev_id, struct pt_regs *regs);

/*====================================================================*/

typedef struct cif60_dev_t {
    dev_link_t          link;
    CARD_DEVICE        *dev;
    dev_node_t          node;
    window_handle_t     sram_win_handle;
} cif60_dev_t;

/*======================================================================

    cif60_attach() creates an "instance" of the driver, allocating
    local data structures for one device.  The device is registered
    with Card Services.

======================================================================*/

static dev_link_t *cif60_attach(void)
{
    cif60_dev_t *info;
    dev_link_t *link;
    CARD_DEVICE *dev;
    client_reg_t client_reg;
    int ret;
    
    DEBUG(0, "cif60_attach()\n");

    /* Create new cif device */
    info = kmalloc(sizeof(*info), GFP_KERNEL); 
    if (!info) return NULL;
    memset(info,0,sizeof(*info));
    // dev = alloc_trdev(sizeof(struct tok_info));
    dev = (CARD_DEVICE *)kmalloc( sizeof(CARD_DEVICE), GFP_KERNEL);
    if (!dev) { 
        kfree(info); 
        return NULL;
    } 

    link = &info->link;
    link->priv = info;
    //link->io.Attributes1 = IO_DATA_PATH_WIDTH_8;
    //link->io.NumPorts1 = 4;
    //link->io.IOAddrLines = 16;
    //link->irq.Attributes = IRQ_TYPE_EXCLUSIVE | IRQ_HANDLE_PRESENT;
    //link->irq.IRQInfo1 = IRQ_INFO2_VALID|IRQ_LEVEL_ID;
    //if (irq_list[0] == -1)
    //    link->irq.IRQInfo2 = irq_mask;
    //else
    //    for (i = 0; i < 4; i++)
    //        link->irq.IRQInfo2 |= 1 << irq_list[i];
    //link->irq.Handler = &cif_interrupt;
    link->conf.Attributes = CONF_VALID_CLIENT;
    link->conf.Vcc = 50;
    link->conf.IntType = INT_MEMORY;
    link->conf.Present = PRESENT_OPTION;

    //link->irq.Instance = info->dev = dev;
    
    /* Register with Card Services */
    link->next = dev_list;
    dev_list = link;
    client_reg.dev_info = &dev_info;
    client_reg.Attributes = INFO_MEM_CLIENT | INFO_CARD_SHARE;
    client_reg.EventMask =
        CS_EVENT_CARD_INSERTION | CS_EVENT_CARD_REMOVAL |
        CS_EVENT_RESET_PHYSICAL | CS_EVENT_CARD_RESET |
        CS_EVENT_PM_SUSPEND | CS_EVENT_PM_RESUME;
    client_reg.event_handler = &cif60_event;
    client_reg.Version = 0x0210;
    client_reg.event_callback_args.client_data = link;
    ret = pcmcia_register_client(&link->handle, &client_reg);
    if (ret != 0) {
        cs_error(link->handle, RegisterClient, ret);
        goto out_detach;
    }

out:
    DEBUG(0,"cif60_attach:out:open=%d state=%d\n",link->open,link->state);
    link->open = 1;
    DEBUG(0,"cif60_attach:out:link=0x%p handle=0x%p\n",link,link->handle);
    return link;

out_detach:
    DEBUG(0,"cif60_attach:out_detach:link=%p\n",link->handle);
    cif60_detach(link);
    link = NULL;
    goto out;
} /* cif60_attach */

/*======================================================================

    This deletes a driver "instance".  The device is de-registered
    with Card Services.  If it has been released, all local data
    structures are freed.  Otherwise, the structures will be freed
    when the device is released.

======================================================================*/

static void cif60_detach(dev_link_t *link)
{
    struct cif60_dev_t *info = link->priv;
    dev_link_t **linkp;
    CARD_DEVICE *dev;

    DEBUG(0, "cif60_detach(0x%p)\n", link);

    /* Locate device structure */
    for (linkp = &dev_list; *linkp; linkp = &(*linkp)->next)
        if (*linkp == link) break;
    if (*linkp == NULL)
        return;

    dev = info->dev;

    DEBUG(0, "cif60_detach2\n");
    if(link->state & DEV_CONFIG)
    {
      DEBUG(0, "cif60_detach2:cif60_release\n");
      cif60_release(link);
    }  

    if(link->handle)
    {
      DEBUG(0, "cif60_detach2:deregister_client\n");
      pcmcia_deregister_client(link->handle);
    }
    
    /* Unlink device structure, free bits */
    *linkp = link->next;
    kfree(dev);
    kfree(info); 
    DEBUG(0, "cif60_detach2:return\n");
} /* cif60_detach */

/*======================================================================

    cif60_config() is scheduled to run after a CARD_INSERTION event
    is received, to configure the PCMCIA socket, and to make the
    token-ring device available to the system.

======================================================================*/

#define CS_CHECK(fn, ret) \
do { last_fn = (fn); if ((last_ret = (ret)) != 0) goto cs_failed; } while (0)

static void cif60_config(dev_link_t *link)
{
    client_handle_t handle = link->handle;
    cif60_dev_t *info = link->priv;
    //CARD_DEVICE *dev = info->dev;
    tuple_t tuple;
    cisparse_t parse;
    win_req_t req;
    memreq_t mem;
    int last_ret, last_fn;
    u_char buf[64];

    DEBUG(0, "cif60_config(0x%p)\n", link);

    tuple.Attributes = 0;
    tuple.TupleData = buf;
    tuple.TupleDataMax = 64;
    tuple.TupleOffset = 0;
    tuple.DesiredTuple = CISTPL_CONFIG;
    CS_CHECK(GetFirstTuple, pcmcia_get_first_tuple(handle, &tuple));
    DEBUG(0, "cif60_config:GetFirstTuple\n");
    CS_CHECK(GetTupleData, pcmcia_get_tuple_data(handle, &tuple));
    DEBUG(0, "cif60_config:GetTupleData\n");
    CS_CHECK(ParseTuple, pcmcia_parse_tuple(handle, &tuple, &parse));
    DEBUG(0, "cif60_config:ParseTuple\n");
    link->conf.ConfigBase = parse.config.base;
    DEBUG(0, "cif60_config:link->conf.ConfigBase=%d\n", link->conf.ConfigBase);

    /* Configure card */
    link->state |= DEV_CONFIG;

    //link->conf.ConfigIndex = 0x61;

    /* Determine if this is PRIMARY or ALTERNATE. */

    /* Try PRIMARY card at 0xA20-0xA23 */
    /*
    link->io.BasePort1 = 0xd0000;
    i = pcmcia_request_io(link->handle, &link->io);
    DEBUG(0, "cif60_config:request_io ret=%d\n", i);
    if (i != CS_SUCCESS) {
        / * Couldn't get 0xA20-0xA23.  Try ALTERNATE at 0xA24-0xA27. * /
        link->io.BasePort1 = 0xA24;
        CS_CHECK(RequestIO, pcmcia_request_io(link->handle, &link->io));
        DEBUG(0, "cif60_config:retry request_io\n");
    }
    dev->base_addr = link->io.BasePort1;
    DEBUG(0, "cif60_config:base_addr=%lx\n", dev->base_addr);
    */
    
    /*
    CS_CHECK(RequestIRQ, pcmcia_request_irq(link->handle, &link->irq));
    dev->irq = link->irq.AssignedIRQ;
    DEBUG(0, "cif60_config:irq=%d\n", dev->irq);
    */
    /* Allocate the MMIO memory window */
    /*
    req.Attributes = WIN_DATA_WIDTH_16|WIN_MEMORY_TYPE_CM|WIN_ENABLE;
    req.Attributes |= WIN_USE_WAIT;
    req.Base = 0; 
    req.Size = 0x2000;
    req.AccessSpeed = 250;
    CS_CHECK(RequestWindow, pcmcia_request_window(&link->handle, &req, &link->win));
    DEBUG(0, "cif60_config:RequestWindow\n");

    mem.CardOffset = mmiobase;
    mem.Page = 0;
    CS_CHECK(MapMemPage, pcmcia_map_mem_page(link->win, &mem));
    dev->mmio = ioremap(req.Base, req.Size);
    */
    
    /* Allocate the SRAM memory window */
    req.Attributes = WIN_DATA_WIDTH_16|WIN_MEMORY_TYPE_CM|WIN_ENABLE;
    req.Attributes |= WIN_USE_WAIT;
    req.Base = 0;
    req.Size = sramsize * 1024;
    req.AccessSpeed = 250;
    CS_CHECK(RequestWindow, pcmcia_request_window(&link->handle, &req, &info->sram_win_handle));
    DEBUG(0, "cif60_config:RequestWindow2\n");

    mem.CardOffset = srambase;
    mem.Page = 0;
    CS_CHECK(MapMemPage, pcmcia_map_mem_page(info->sram_win_handle, &mem));
    DEBUG(0, "cif60_config:MapMemPage\n");

    //dev->sram_base = mem.CardOffset >> 12;
    //dev->sram_virt = (u_long)ioremap(req.Base, req.Size);

    //CS_CHECK(RequestConfiguration, pcmcia_request_configuration(link->handle, &link->conf));
    //DEBUG(0, "cif60_config:RequestConfiguration\n");

    DEBUG(0, "cif60_config:return\n");
    return;
/*    
    link->dev = &info->node;
    link->state &= ~DEV_CONFIG_PENDING;

    i = cif_scan_pci();   
    // i = cif60_probe_card(dev);
    if(i != -ENODEV) {
        printk(KERN_NOTICE "cif_cs: register_cif() failed\n");
        link->dev = NULL;
        goto failed;
    }

    strcpy(info->node.dev_name, dev->name);

    printk(KERN_INFO "%s: port %#3lx, irq %d,",
           dev->name, dev->base_addr, dev->irq);
    printk("\n");
    
    return;
*/
cs_failed:
    DEBUG(0, "cif60_config:cs_failed\n");
    cs_error(link->handle, last_fn, last_ret);
/*
failed:
    cif60_release(link);
*/
} /* cif60_config */

/*======================================================================

    After a card is removed, cif60_release() will unregister the net
    device, and release the PCMCIA configuration.  If the device is
    still open, this will be postponed until it is closed.

======================================================================*/

static void cif60_release(dev_link_t *link)
{
    cif60_dev_t *info = link->priv;
    //CARD_DEVICE *dev = info->dev;

    DEBUG(0, "cif60_release(0x%p)\n", link);

    pcmcia_release_configuration(link->handle);
    //pcmcia_release_io(link->handle, &link->io);
    //pcmcia_release_irq(link->handle, &link->irq);
    if (link->win) {
        //iounmap((void *)dev->mmio);
        //pcmcia_release_window(link->win);
        pcmcia_release_window(info->sram_win_handle);
    }

    link->state &= ~DEV_CONFIG;
}

/*======================================================================

    The card status event handler.  Mostly, this schedules other
    stuff to run after an event is received.  A CARD_REMOVAL event
    also sets some flags to discourage the net drivers from trying
    to talk to the card any more.

======================================================================*/

static int cif60_event(event_t event, int priority,
                       event_callback_args_t *args)
{
    dev_link_t *link = args->client_data;
    cif60_dev_t *info = link->priv;
    CARD_DEVICE *dev = info->dev;
    int ret;
    
    DEBUG(1, "cif60_event(0x%06x)\n", event);

    switch (event) {
    case CS_EVENT_CARD_REMOVAL:
        DEBUG(1, "cif60_event:CS_EVENT_CARD_REMOVAL\n");
        link->state &= ~DEV_PRESENT;
        if (link->state & DEV_CONFIG) {
            /* set flag to bypass normal interrupt code */
            dev->sram_virt |= 1;
            cif_cleanup();
        }
        break;
    case CS_EVENT_CARD_INSERTION:
        DEBUG(1, "cif60_event:CS_EVENT_CARD_INSERTION\n");
        link->state |= DEV_PRESENT;
        cif60_config(link);
        DEBUG(1, "cif60_event:before cif_init\n");
        cif_init();
        DEBUG(1, "cif60_event:after cif_init\n");
        /*
        DEBUG(1, "cif60_event:CS_EVENT_CARD_INSERTION2\n");
        ret = pcmcia_request_configuration(link->handle, &link->conf);
        DEBUG(1, "cif60_event:request_configuration ret=0x%x\n",ret);
        cif_init(); 
        DEBUG(1, "cif60_event:really\n");
        */
        break;
    case CS_EVENT_PM_SUSPEND:
        DEBUG(1, "cif60_event:CS_EVENT_PM_SUSPEND\n");
        link->state |= DEV_SUSPEND;
        /* Fall through... */
    case CS_EVENT_RESET_PHYSICAL:
        DEBUG(1, "cif60_event:CS_EVENT_RESET_PHYSICAL\n");
        if (link->state & DEV_CONFIG) {
            cif_cleanup();
            pcmcia_release_configuration(link->handle);
        }
        break;
    case CS_EVENT_PM_RESUME:
        DEBUG(1, "cif60_event:CS_EVENT_PM_RESUME\n");
        link->state &= ~DEV_SUSPEND;
        /* Fall through... */
    case CS_EVENT_CARD_RESET:
        DEBUG(1, "cif60_event:CS_EVENT_CARD_RESET\n");
        if (link->state & DEV_CONFIG) {
            ret = pcmcia_request_configuration(link->handle, &link->conf);
            DEBUG(1, "cif60_event:request_configuration ret=0x%x\n",ret);
            if (link->open) {  
                DEBUG(1, "cif60_event:really\n");
                cif_init();       /* really? */
            }
        }
        break;
    }
    return 0;
} /* cif60_event */

/*====================================================================*/

static struct pcmcia_driver cif60_cs_driver = {
        .owner          = THIS_MODULE,
        .drv            = {
                .name   = "cif_cs",
        },
        .attach         = cif60_attach,
        .detach         = cif60_detach,
};

static int __init init_cif60_cs(void)
{
        int ret;
        ret = pcmcia_register_driver(&cif60_cs_driver);
        DEBUG(0, "cif_cs:init ret=%d\n", ret);
        return ret;
}

static void __exit exit_cif60_cs(void)
{
        pcmcia_unregister_driver(&cif60_cs_driver);
        while (dev_list != NULL)
                cif60_detach(dev_list);
        DEBUG(0, "cif_cs:exit\n");
}

module_init(init_cif60_cs);
module_exit(exit_cif60_cs);
