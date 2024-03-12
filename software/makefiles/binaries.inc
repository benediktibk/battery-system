BOARDSSOURCE := $(shell find boards -type f)
ZEPHYRSOURCE := $(shell find zephyr -type f -iname *.[ch]) $(shell find zephyr -type f -iname *.dts) $(shell find zephyr -type f -iname *.dtsi) $(shell find zephyr -type f -iname *.yaml)
COMMONAPPDEPENDENCIES := $(ALLMAKEFILES) build/guard zephyr/VERSION build/zephyrRevisionCheck $(ZEPHYRSOURCE) $(BOARDSSOURCE)
BATTERYMANAGEMENTSYSTEMSOURCE := $(shell find battery_management_system -type f)
BATTERYMANAGEMENTSYSTEMDEPENDENCIES := $(COMMONAPPDEPENDENCIES) $(BATTERYMANAGEMENTSYSTEMSOURCE)

zephyr/VERSION:
	west update

build/zephyrRevisionCheck: $(ALLMAKEFILES) build/guard $(ZEPHYRSOURCE) zephyr/VERSION scripts/zephyrVersionCheck.py
	rm -f $@
	./scripts/zephyrVersionCheck.py --manifest=manifest/manifest.yml --zephyr=zephyr
	touch $@


build/%/zephyr/zephyr.elf: build/%/build.ninja
	ninja -j `nproc` -C build/$*

build/%/battery_management_system/build.ninja: $(BATTERYMANAGEMENTSYSTEMDEPENDENCIES)
	CONF_FILE="prj.base.conf prj.$*.conf" west build --cmake-only -p auto --board $(BATTERYMANAGEMENTSYSTEMBOARD) -d build/$*/battery_management_system battery_management_system

build/%/battery_management_system.elf: build/%/battery_management_system/zephyr/zephyr.elf
	cp $< $@