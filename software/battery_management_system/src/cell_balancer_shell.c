#include <autoconf.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "cell_balancer.h"

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

#define CELL_BALANCER_LIST_ENTRY(node_id) DEVICE_DT_GET(node_id),

static const struct device *cell_balancer_shell_devices[] = {
    DT_FOREACH_STATUS_OKAY(benediktibk_cell_balancer, CELL_BALANCER_LIST_ENTRY)
};

static int cell_balancer_shell_disable(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
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

    bool success = cell_balancer_disable(device);

    if (!success) {
        shell_print(sh, "disabling cell balancer failed");
        return -3;
    }

    return 0;
}

static int cell_balancer_shell_enable(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 3) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    char *device_name = argv[1];
    char *index = argv[2];

    const struct device *device = device_get_binding(device_name);

    if (device == NULL) {
        shell_print(sh, "unable to find device %s", device_name);
        return -2;
    }

    int index_converted = atoi(index);

    bool success = cell_balancer_enable(device, index_converted);

    if (!success) {
        shell_print(sh, "enabling cell balancer failed");
        return -3;
    }

    return 0;
}

static int cell_balancer_shell_list(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 1) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    shell_print(sh, "available devices:");

    for (size_t i = 0; i < ARRAY_SIZE(cell_balancer_shell_devices); ++i) {
        shell_print(sh, "\t%s", cell_balancer_shell_devices[i]->name);
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(cell_balancer_command,
                               SHELL_CMD_ARG(disable, NULL, "disable cell balancer", cell_balancer_shell_disable, 2, 0),
                               SHELL_CMD_ARG(enable, NULL, "enable cell balancer", cell_balancer_shell_enable, 3, 0),
                               SHELL_CMD_ARG(list, NULL, "list available instances", cell_balancer_shell_list, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(cell_balancer, &cell_balancer_command, "cell balancer", NULL);