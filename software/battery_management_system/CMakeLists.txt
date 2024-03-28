cmake_minimum_required(VERSION 3.21.1)

list(APPEND BOARD_ROOT $ENV{ZEPHYR_BASE}/..)
list(APPEND DTS_ROOT $ENV{ZEPHYR_BASE}/..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(battery_management_system)

message(STATUS "define sources for main app")

target_include_directories(app PRIVATE include)

target_sources(app PRIVATE
	src/main.c
	src/status_leds.c
	src/cell_voltage.c
	src/status_leds_shell.c
	src/cell_voltage_shell.c
)