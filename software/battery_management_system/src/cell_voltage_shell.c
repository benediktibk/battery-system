#include <autoconf.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "cell_voltage.h"

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

#define STATUS_LEDS_LIST_ENTRY(node_id) DEVICE_DT_GET(node_id),

static const struct device *cell_voltage_shell_devices[] = {
    DT_FOREACH_STATUS_OKAY(benediktibk_cell_voltage, STATUS_LEDS_LIST_ENTRY)
};

static int cell_voltage_shell_measure(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    uint32_t value;

    if (argc != 2) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    char *device_name = argv[1];

    const struct device *device = device_get_binding(device_name);

    if (device == NULL) {
        shell_print(sh, "unable to find device %s", device_name);
        return -2;
    }

    bool success = cell_voltage_measure(device, &value);

    if (!success) {
        shell_print(sh, "measuring cell voltage failed");
        return -3;
    }

    shell_print(sh, "cell voltage: %i mV", value);

    return 0;
}

static int cell_voltage_shell_list(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 1) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    shell_print(sh, "available devices:");

    for (size_t i = 0; i < ARRAY_SIZE(cell_voltage_shell_devices); ++i) {
        shell_print(sh, "\t%s", cell_voltage_shell_devices[i]->name);
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(cell_voltage_command,
                               SHELL_CMD_ARG(measure, NULL, "measure cell voltage", cell_voltage_shell_measure, 2, 0),
                               SHELL_CMD_ARG(list, NULL, "list available instances", cell_voltage_shell_list, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(cell_voltage, &cell_voltage_command, "cell voltage", NULL);