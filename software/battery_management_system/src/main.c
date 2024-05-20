#include <autoconf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "cells_measurement.h"
#include "power_control.h"
#include "status_leds.h"

LOG_MODULE_REGISTER(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

struct battery_state {
    int8_t cell_temperatures[CELL_COUNT]; // in °C
    uint16_t cell_voltages[CELL_COUNT]; // in mV
    int8_t bms_pcb_temperature; // in °C
};

const struct device *status_leds_device = DEVICE_DT_GET(DT_NODELABEL(status_leds));
const struct device *power_control_device = DEVICE_DT_GET(DT_NODELABEL(power_control));
const struct device *bms_temp_device = DEVICE_DT_GET(DT_NODELABEL(bms_temp));
const struct device *cells_measurement_device = DEVICE_DT_GET(DT_NODELABEL(cells_measurement));
const struct device *cell_temperature_devices[] = {
    DEVICE_DT_GET(DT_NODELABEL(cell_temp1)),
    DEVICE_DT_GET(DT_NODELABEL(cell_temp2)),
    DEVICE_DT_GET(DT_NODELABEL(cell_temp3)),
    DEVICE_DT_GET(DT_NODELABEL(cell_temp4)),
};
static struct battery_state battery_state = { 0 };

static bool check_device_ready(const struct device *device)
{
    if (!device_is_ready(device)) {
        LOG_ERR("device %s is not ready", device->name);
        return false;
    }

    return true;
}

static bool check_ready(void)
{
    if (!check_device_ready(status_leds_device)) {
        return false;
    }

    if (!check_device_ready(power_control_device)) {
        return false;
    }

    if (!check_device_ready(bms_temp_device)) {
        return false;
    }

    if (!check_device_ready(cells_measurement_device)) {
        return false;
    }

    for (size_t i = 0; i < CELL_COUNT; ++i) {
        if (!check_device_ready(cell_temperature_devices[i])) {
            return false;
        }
    }

    return true;
}

static bool fetch_bms_pcb_temperature(void)
{
    struct sensor_value sensor_value;

    int result = sensor_sample_fetch(bms_temp_device);

    if (result != 0) {
        LOG_ERR("unable to fetch PCB temperature of BMS");
        return false;
    }

    result = sensor_channel_get(bms_temp_device, SENSOR_CHAN_AMBIENT_TEMP, &sensor_value);

    if (result != 0) {
        LOG_ERR("unable to get PCB temperature of BMS");
        return false;
    }

    battery_state.bms_pcb_temperature = sensor_value.val1;

    return true;
}

static bool fetch_cell_voltages(void)
{
    struct cell_voltages cell_voltages;

    bool success = cells_measurement_measure(cells_measurement_device, &cell_voltages);

    if (!success) {
        return false;
    }

    for (size_t i = 0; i < CELL_COUNT; ++i) {
        battery_state.cell_voltages[i] = cell_voltages.value[i];
    }

    return true;
}

static bool fetch_cell_temperatures(void)
{
    struct sensor_value sensor_value;

    for (size_t i = 0; i < ARRAY_SIZE(cell_temperature_devices); ++i) {
        int result = sensor_sample_fetch(cell_temperature_devices[i]);

        if (result != 0) {
            LOG_ERR("unable to fetch cell %i temperature", i);
            return false;
        }

        result = sensor_channel_get(cell_temperature_devices[i], SENSOR_CHAN_AMBIENT_TEMP, &sensor_value);

        if (result != 0) {
            LOG_ERR("unable to get cell %i temperature", i);
            return false;
        }

        battery_state.cell_temperatures[i] = sensor_value.val1;
    }

    return true;
}

static void emergency_shutdown(void)
{
    LOG_ERR("executing emergency shutdown");
    status_leds_set_error(status_leds_device, true);
    power_control_set_charge(power_control_device, false);
    power_control_set_discharge(power_control_device, false);

    // This won't return if battery powered
}

static bool execute(void)
{
    if (!fetch_bms_pcb_temperature()) {
        return false;
    }

    if (!fetch_cell_voltages()) {
        return false;
    }

    if (!fetch_cell_temperatures()) {
        return false;
    }

    if (battery_state.bms_pcb_temperature >= CONFIG_MAXIMUM_PCB_TEMPERATURE_CELSIUS) {
        LOG_ERR("BMS temperature is too high: %i °C", battery_state.bms_pcb_temperature);
        return false;
    }

    for (size_t i = 0; i < CELL_COUNT; ++i) {
        if (battery_state.cell_temperatures[i] < CONFIG_MINIMUM_CELL_TEMPERATURE_CELSIUS) {
            LOG_ERR("cell %i temperature is too low: %i °C", i, battery_state.cell_temperatures[i]);
            return false;
        }

        if (battery_state.cell_temperatures[i] > CONFIG_MAXIMUM_CELL_TEMPERATURE_CELSIUS) {
            LOG_ERR("cell %i temperature is too high: %i °C", i, battery_state.cell_temperatures[i]);
            return false;
        }
    }

    // TODO: due to a design error all other voltages are incorrect
    for (size_t i = 0; i < 1; ++i) {
        if (battery_state.cell_voltages[i] < CONFIG_MINIMUM_CELL_VOLTAGE_MV) {
            LOG_ERR("cell %i voltage is too low: %i mV", i, battery_state.cell_voltages[i]);
            return false;
        }

        if (battery_state.cell_voltages[i] > CONFIG_MAXIMUM_CELL_VOLTAGE_MV) {
            LOG_ERR("cell %i voltage is too high: %i mV", i, battery_state.cell_voltages[i]);
            return false;
        }
    }

    return true;
}

int main(void)
{
    int64_t error_start;
    bool error_detected = false;

    if (!check_ready()) {
        emergency_shutdown();
        return -1;
    }

    power_control_set_charge(power_control_device, true);
    power_control_set_discharge(power_control_device, true);

    LOG_INF("starting battery management system");
    status_leds_set_system_active(status_leds_device, true);

    while (true) {
        if (!execute()) {
            if (!error_detected) {
                error_start = k_uptime_get();
                error_detected = true;
            }
        } else {
            error_detected = false;
        }

        int64_t now = k_uptime_get();

        if (error_detected) {
            int64_t error_length = now - error_start;

            if (error_length > CONFIG_MAXIMUM_ERROR_TIME_MS) {
                LOG_ERR("error persisted too long, for %lli ms", error_length);
                emergency_shutdown();
                return -1;
            }
        }

        status_leds_set_error(status_leds_device, error_detected);
        k_sleep(K_MSEC(CONFIG_BMS_LOOP_TIME_MS));
    }
}

static int battery_state_shell_get(
    const struct shell *sh,
    size_t argc,
    char **argv)
{
    shell_print(sh, "BMS PCB temperature: %i °C", battery_state.bms_pcb_temperature);

    for (size_t i = 0; i < CELL_COUNT; ++i) {
        shell_print(sh, "cell %i voltage: %i mV", i, battery_state.cell_voltages[i]);
    }

    for (size_t i = 0; i < CELL_COUNT; ++i) {
        shell_print(sh, "cell %i temperature: %i °C", i, battery_state.cell_temperatures[i]);
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(battery_state_command,
                               SHELL_CMD_ARG(get, NULL, "get battery state", battery_state_shell_get, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(battery_state, &battery_state_command, "battery state", NULL);