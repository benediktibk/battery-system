#include <autoconf.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "power_control.h"

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

#define POWER_CONTROL_LIST_ENTRY(node_id) DEVICE_DT_GET(node_id),

static const struct device *power_control_shell_devices[] = {
    DT_FOREACH_STATUS_OKAY(benediktibk_power_control, POWER_CONTROL_LIST_ENTRY)
};

static int power_control_shell_charge(
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

    bool success = power_control_set_charge(device, true);

    if (!success) {
        shell_print(sh, "power control failed");
        return -3;
    }

    return 0;
}

static int power_control_shell_not_charge(
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

    bool success = power_control_set_charge(device, false);

    if (!success) {
        shell_print(sh, "power control failed");
        return -3;
    }

    return 0;
}

static int power_control_shell_discharge(
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

    bool success = power_control_set_discharge(device, true);

    if (!success) {
        shell_print(sh, "power control failed");
        return -3;
    }

    return 0;
}

static int power_control_shell_not_discharge(
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

    bool success = power_control_set_discharge(device, false);

    if (!success) {
        shell_print(sh, "power control failed");
        return -3;
    }

    return 0;
}

static int power_control_shell_get_charge_available(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    bool value;

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

    bool success = power_control_get_charge_available(device, &value);

    if (!success) {
        shell_print(sh, "power control failed");
        return -3;
    }

    shell_print(sh, "charge is available: %s", value ? "YES" : "NO");

    return 0;
}

static int power_control_shell_list(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    if (argc != 1) {
        shell_print(sh, "invalid number of arguments\n");
        return -1;
    }

    shell_print(sh, "available devices:");

    for (size_t i = 0; i < ARRAY_SIZE(power_control_shell_devices); ++i) {
        shell_print(sh, "\t%s", power_control_shell_devices[i]->name);
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(power_control_command,
                               SHELL_CMD_ARG(charge, NULL, "enable charging", power_control_shell_charge, 2, 0),
                               SHELL_CMD_ARG(notcharge, NULL, "disable charging", power_control_shell_not_charge, 2, 0),
                               SHELL_CMD_ARG(discharge, NULL, "enable discharging", power_control_shell_discharge, 2, 0),
                               SHELL_CMD_ARG(notdischarge, NULL, "disable discharging", power_control_shell_not_discharge, 2, 0),
                               SHELL_CMD_ARG(get_charge_available, NULL, "get charge available", power_control_shell_get_charge_available, 2, 0),
                               SHELL_CMD_ARG(list, NULL, "list available instances", power_control_shell_list, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(power_control, &power_control_command, "power control", NULL);