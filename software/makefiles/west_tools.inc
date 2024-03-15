flash-battery-management-system-%-internal: build/%/battery_management_system.elf
	west flash --build-dir build/$*/battery_management_system

guiconfig-battery-management-system-%-internal:
	west build -d build/$*/battery_management_system -t guiconfig -s battery_management_system

initlevels-battery-management-system-%-internal:
	CONF_FILE="prj.base.conf prj.$*.conf" west build -d build/$*/battery_management_system -t initlevels --board $(BATTERYSYSTEMBOARD) -s battery_management_system

rom-battery-management-system-%-internal:
	west build -d build/$*/battery_management_system -t rom_report -s battery_management_system

ram-battery-management-system-%-internal:
	west build -d build/$*/battery_management_system -t ram_report -s battery_management_system