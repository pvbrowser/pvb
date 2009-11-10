module="cif"
device="cif"

make clean; make

cp -p -f cif.ko /lib/modules/`uname -r`/extra
depmod
modprobe -r $module

modprobe $module $*

major=`cat /proc/devices | awk "\\$2==\"$module\" {print \\$1}"`

rm -f /dev/${device}
mknod -m 644 /dev/${device} c $major 0
