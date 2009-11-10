module="cif"
device="cif"

modprobe $module $*

major=`cat /proc/devices | awk "\\$2==\"$module\" {print \\$1}"`

rm -f /dev/${device}
mknod -m 644 /dev/${device} c $major 0
