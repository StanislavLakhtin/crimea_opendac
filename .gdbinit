set target-async on
set confirm off
set history save
set mem inaccessible-by-default off
target extended-remote /dev/ttyACM0
mon swdp_scan
att 1
load ./build/src/crimea_opendac.elf
file ./build/src/crimea_opendac.elf
start
