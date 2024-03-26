#include <autoconf.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "status_leds.h"

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

#define STATUS_LEDS_LIST_ENTRY(node_id) DEVICE_DT_GET(node_id),

static const struct device *status_leds_shell_devices[] = {
    DT_FOREACH_STATUS_OKAY(benediktibk_status_leds, STATUS_LEDS_LIST_ENTRY)
};

static int status_leds_shell_system_active(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 3) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    char *device_name = argv[1];
    char *state = argv[2];

    const struct device *device = device_get_binding(device_name);

    if (device == NULL) {
        shell_print(sh, "unable to find device %s", device_name);
        return -2;
    }

    int state_converted = atoi(state);
    bool state_boolean = state_converted;

    shell_print(sh, "setting system active to %s", state_boolean ? "ACTIVE" : "INACTIVE");

    bool success = status_leds_set_system_active(device, state_boolean);

    if (!success) {
        shell_print(sh, "setting system active led failed");
        return -1;
    }

    return 0;
}

static int status_leds_shell_error(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 3) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    char *device_name = argv[1];
    char *state = argv[2];

    const struct device *device = device_get_binding(device_name);

    if (device == NULL) {
        shell_print(sh, "unable to find device %s", device_name);
        return -2;
    }

    int state_converted = atoi(state);
    bool state_boolean = state_converted;

    shell_print(sh, "setting error to %s", state_boolean ? "ACTIVE" : "INACTIVE");

    bool success = status_leds_set_error(device, state_boolean);

    if (!success) {
        shell_print(sh, "setting error led failed");
        return -1;
    }

    return 0;
}

static int status_leds_shell_debug(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 3) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    char *device_name = argv[1];
    char *state = argv[2];

    const struct device *device = device_get_binding(device_name);

    if (device == NULL) {
        shell_print(sh, "unable to find device %s", device_name);
        return -2;
    }

    int state_converted = atoi(state);
    bool state_boolean = state_converted;

    shell_print(sh, "setting debug to %s", state_boolean ? "ACTIVE" : "INACTIVE");

    bool success = status_leds_set_debug(device, state_boolean);

    if (!success) {
        shell_print(sh, "setting debug led failed");
        return -1;
    }

    return 0;
}

static int status_leds_shell_list(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 1) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    shell_print(sh, "available devices:");

    for (size_t i = 0; i < ARRAY_SIZE(status_leds_shell_devices); ++i) {
        shell_print(sh, "\t%s", status_leds_shell_devices[i]->name);
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(status_leds_command,
                               SHELL_CMD_ARG(system_active, NULL, "set system active LED", status_leds_shell_system_active, 3, 0),
                               SHELL_CMD_ARG(error, NULL, "set error LED", status_leds_shell_error, 3, 0),
                               SHELL_CMD_ARG(debug, NULL, "set debug LED", status_leds_shell_debug, 3, 0),
                               SHELL_CMD_ARG(list, NULL, "list available instances", status_leds_shell_list, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(status_leds, &status_leds_command, "status leds", NULL);