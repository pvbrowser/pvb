module="cif"
device="cif"

modprobe -r $module

# Remove stale nodes
rm -f /dev/${device}
