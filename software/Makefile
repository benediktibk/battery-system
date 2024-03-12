#!/usr/bin/make -f

BINARIESTARGETSSHORT := battery_management_system
BINARIESTARGETSDEBUG := $(foreach binary,$(BINARIESTARGETSSHORT),build/debug/$(binary).elf)
BINARIESTARGETSRELEASE := $(foreach binary,$(BINARIESTARGETSSHORT),build/release/$(binary).elf)
BINARIESTARGETS := $(BINARIESTARGETSDEBUG) $(BINARIESTARGETSRELEASE)
BATTERYMANAGEMENTSYSTEMBOARD = battery_management_system@0
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

include makefiles/build_directories.inc
include makefiles/binaries.inc
include makefiles/west_tools.inc
include makefiles/code_style.inc

.PHONY: all clean format

.SECONDARY:

.DELETE_ON_ERROR: