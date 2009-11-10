#!/bin/bash
# cp -p -f cif.ko /lib/modules/`uname -r`/kernel/drivers/pcmcia/
cp -p -f cif_cs.ko /lib/modules/`uname -r`/extra/cif_cs.ko
cp ../etc/pcmcia/cif60pb.conf /etc/pcmcia/
depmod
rmmod cif_cs
modprobe cif_cs
