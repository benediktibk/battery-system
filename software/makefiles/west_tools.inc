flash-battery-system-%-internal: build/%/battery_system.elf
	west flash --build-dir build/$*/battery_system

guiconfig-battery-system-%-internal:
	west build -d build/$*/battery_system -t guiconfig -s battery_system

initlevels-battery-system-%-internal:
	CONF_FILE="prj.base.conf prj.$*.conf" west build -d build/$*/battery_system -t initlevels --board $(BATTERYSYSTEMBOARD) -s battery_system