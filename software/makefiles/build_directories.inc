BUILDDIRECTORIES = build
BUILDDIRECTORIES += build/debug
BUILDDIRECTORIES += build/debug/battery_system
BUILDDIRECTORIES += build/release
BUILDDIRECTORIES += build/release/battery_system

build/guard: $(ALLMAKEFILES)
    rm -f $@
	mkdir -p $(BUILDDIRECTORIES)
	touch $@