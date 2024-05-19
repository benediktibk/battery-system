#!/usr/bin/make -f

BINARIESTARGETSSHORT := battery_management_system
BINARIESTARGETSDEBUG := $(foreach binary,$(BINARIESTARGETSSHORT),build/debug/$(binary).elf)
BINARIESTARGETSRELEASE := $(foreach binary,$(BINARIESTARGETSSHORT),build/release/$(binary).elf)
BINARIESTARGETS := $(BINARIESTARGETSDEBUG) $(BINARIESTARGETSRELEASE)
BATTERYMANAGEMENTSYSTEMBOARD = battery_management_system@2
CLANGFORMAT = clang-format
MAKEFILESINCLUDED := $(shell find makefiles -type f)
ALLMAKEFILES = Makefile $(MAKEFILESINCLUDED)

all: $(BINARIESTARGETS) build/clang-format

clean:
	rm -fR build

flash-battery-management-system-debug: flash-battery-management-system-debug-internal

flash-battery-management-system-release: flash-battery-management-system-release-internal

guiconfig-battery-management-system-debug: guiconfig-battery-management-system-debug-internal

guiconfig-battery-management-system-release: guiconfig-battery-management-system-release-internal

initlevels-battery-management-system-debug: initlevels-battery-management-system-debug-internal

initlevels-battery-management-system-release: initlevels-battery-management-system-release-internal

rom-battery-management-system-debug: rom-battery-management-system-debug-internal

rom-battery-management-system-release: rom-battery-management-system-release-internal

ram-battery-management-system-debug: ram-battery-management-system-debug-internal

ram-battery-management-system-release: ram-battery-management-system-release-internal

debug-battery-management-system-debug: debug-battery-management-system-debug-internal

debug-battery-management-system-release: debug-battery-management-system-release-internal

include makefiles/build_directories.inc
include makefiles/binaries.inc
include makefiles/west_tools.inc
include makefiles/code_style.inc

.PHONY: all clean format

.SECONDARY:

.DELETE_ON_ERROR: