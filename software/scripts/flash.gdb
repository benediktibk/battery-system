target extended-remote /dev/ttyACM0
monitor swdp_scan
attach 1
flash-erase
load
detach
quit