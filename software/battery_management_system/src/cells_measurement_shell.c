#include <autoconf.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "cells_measurement.h"

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

#define CELLS_MEASUREMENT_LIST_ENTRY(node_id) DEVICE_DT_GET(node_id),

static const struct device *cells_measurement_shell_devices[] = {
    DT_FOREACH_STATUS_OKAY(benediktibk_cells_measurement, CELLS_MEASUREMENT_LIST_ENTRY)
};

static int cells_measurement_shell_measure(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    uint32_t value;

    if (argc != 3) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    char *device_name = argv[1];
    char *cell_index = argv[2];

    const struct device *device = device_get_binding(device_name);

    if (device == NULL) {
        shell_print(sh, "unable to find device %s", device_name);
        return -2;
    }

    uint8_t cell_index_casted = atoi(cell_index);

    bool success = cells_measurement_measure(device, cell_index_casted, &value);

    if (!success) {
        shell_print(sh, "measuring cell voltage failed");
        return -3;
    }

    shell_print(sh, "cell %i voltage: %i mV", cell_index_casted, value);

    return 0;
}

static int cells_measurement_shell_list(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 1) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    shell_print(sh, "available devices:");

    for (size_t i = 0; i < ARRAY_SIZE(cells_measurement_shell_devices); ++i) {
        shell_print(sh, "\t%s", cells_measurement_shell_devices[i]->name);
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(cells_measurement_command,
                               SHELL_CMD_ARG(measure, NULL, "measure cell voltage", cells_measurement_shell_measure, 3, 0),
                               SHELL_CMD_ARG(list, NULL, "list available instances", cells_measurement_shell_list, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(cells_measurement, &cells_measurement_command, "cell voltage", NULL);