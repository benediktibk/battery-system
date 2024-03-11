#!/usr/bin/make -f

BINARIESTARGETSSHORT := battery_system
BINARIESTARGETSDEBUG := $(foreach binary,$(BINARIESTARGETSSHORT),build/debug/$(binary).elf)
BINARIESTARGETSRELEASE := $(foreach binary,$(BINARIESTARGETSSHORT),build/release/$(binary).elf)
BINARIESTARGETS := $(BINARIESTARGETSDEBUG) $(BINARIESTARGETSRELEASE)
BATTERYSYSTEMBOARD = battery_system@R0
CLANGFORMAT = clang-format
MAKEFILESINCLUDED := $(shell find makefiles -type f)
ALLMAKEFILES = Makefile $(MAKEFILESINCLUDED)

all: $(BINARIESTARGETS) build/clang-format

build/guard: $(ALLMAKEFILES)
	mkdir -p build
	touch $@

clean:
	rm -fR build

flash-battery-system-debug: flash-battery-system-debug-internal

flash-battery-system-release: flash-battery-system-release-internal

guiconfig-battery-system-debug: guiconfig-battery-system-debug-internal

guiconfig-battery-system-release: guiconfig-battery-system-release-internal

initlevels-battery-system-debug: initlevels-battery-system-debug-internal

initlevels-battery-system-release: initlevels-battery-system-release-internal

include makefiles/build_directories.inc
include makefiles/binaries.inc
include makefiles/west_tools.inc
include makefiles/code_style.inc

.PHONY: all clean format

.SECONDARY:

.DELETE_ON_ERROR: