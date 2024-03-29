#include "cells_measurement.h"

#include <autoconf.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "cell_voltage.h"

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

struct cells_measurement_config {
    const struct device **cell_voltage_devices;
    size_t cell_voltage_devices_count;
    const struct gpio_dt_spec enable_gpio;
};

struct cells_measurement_data {
};

bool cells_measurement_measure(
    const struct device *device,
    struct cell_voltages *cell_voltages)
{
    const struct cells_measurement_config *config = device->config;
    bool success = true;
    struct cell_voltages cell_voltages_raw;

    gpio_pin_set_dt(&config->enable_gpio, 1);

    for (size_t i = 0; i < CELL_COUNT; ++i) {
        success &= cell_voltage_measure(config->cell_voltage_devices[i], &cell_voltages_raw.value[i]);
    }

    gpio_pin_set_dt(&config->enable_gpio, 0);

    if (!success) {
        LOG_ERR("cell voltage measurement failed");
        return false;
    }

    cell_voltages->value[0] = cell_voltages_raw.value[0];

    for (size_t i = 1; i < CELL_COUNT; ++i) {
        cell_voltages->value[i] = cell_voltages_raw.value[i] - cell_voltages_raw.value[i - 1];
    }

    return true;
}

static int cells_measurement_init(const struct device *device)
{
    const struct cells_measurement_config *config = device->config;

    LOG_DBG("initializing cells measurement");

    for (size_t i = 0; i < config->cell_voltage_devices_count; ++i) {
        if (!device_is_ready(config->cell_voltage_devices[i])) {
            LOG_ERR("cell voltage %s is not yet ready", config->cell_voltage_devices[i]->name);
            return -1;
        }
    }

    if (!device_is_ready(config->enable_gpio.port)) {
        LOG_ERR("enable GPIO %s is not yet ready", config->enable_gpio.port->name);
        return -2;
    }

    int result = gpio_pin_configure_dt(&config->enable_gpio, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("configuring enable GPIO failed");
        return -3;
    }

    LOG_DBG("successfully configured cells measurement");
    return 0;
}

#define DT_DRV_COMPAT benediktibk_cells_measurement

#define DEVICE_ARRAY_ENTRY(node_id, prop, idx) \
    DEVICE_DT_GET(DT_PROP_BY_IDX(node_id, prop, idx)),

#define CELLS_MEASUREMENT_DEFINE(index)                                                    \
    static const struct device *cells_measurement_cell_voltages_##index[] = {              \
        DT_FOREACH_PROP_ELEM(DT_DRV_INST(index), cell_voltages, DEVICE_ARRAY_ENTRY)        \
    };                                                                                     \
                                                                                           \
    static struct cells_measurement_data cells_measurement_data_##index;                   \
                                                                                           \
    static struct cells_measurement_config cells_measurement_config_##index = {            \
        .cell_voltage_devices = cells_measurement_cell_voltages_##index,                   \
        .cell_voltage_devices_count = ARRAY_SIZE(cells_measurement_cell_voltages_##index), \
        .enable_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), enable_gpios, 0),       \
    };                                                                                     \
                                                                                           \
    DEVICE_DT_INST_DEFINE(index, cells_measurement_init, NULL,                             \
                          &cells_measurement_data_##index,                                 \
                          &cells_measurement_config_##index,                               \
                          POST_KERNEL, CONFIG_CELLS_MEASUREMENT_POST_KERNEL_INIT_PRIORITY, \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(CELLS_MEASUREMENT_DEFINE)