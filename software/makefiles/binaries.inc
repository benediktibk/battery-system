BOARDSSOURCE := $(shell find boards -type f)
ZEPHYRSOURCE := $(shell find zephyr -type f -iname *.[ch]) $(shell find zephyr -type f -iname *.dts) $(shell find zephyr -type f -iname *.dtsi) $(shell find zephyr -type f -iname *.yaml)
COMMONAPPDEPENDENCIES := $(ALLMAKEFILES) build/guard zephyr/VERSION build/zephyrRevisionCheck $(ZEPHYRSOURCE) $(BOARDSSOURCE)
BATTERYSYSTEMSOURCE := $(shell find battery_system -type f)
BATTERYSYSTEMDEPENDENCIES := $(COMMONAPPDEPENDENCIES) $(BATTERYSYSTEMSOURCE)

zephyr/VERSION:
	west update

build/zephyrRevisionCheck: $(ALLMAKEFILES) build/guard $(ZEPHYRSOURCE) zephyr/VERSION scripts/zephyrVersionCheck.py
	rm -f $@
	./scripts/zephyrVersionCheck.py --manifest=manifest/manifest.yml --zephyr=zephyr
	touch $@


build/%/zephyr/zephyr.elf: build/%/build.ninja
	ninja -j `nproc` -C build/$*

build/%/battery_system/build.ninja: $(BATTERYSYSTEMDEPENDENCIES)
	CONF_FILE="prj.base.conf prj.$*.conf" west build --cmake-only -p auto --board $(BATTERYSYSTEMBOARD) -d build/$*/battery_system battery_system

build/%/battery_system.elf: build/%/battery_system/zephyr/zephyr.elf
	cp $< $@