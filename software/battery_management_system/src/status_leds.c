#include "status_leds.h"

#include <autoconf.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

struct status_leds_config {
    const struct gpio_dt_spec gpio_system_active;
    const struct gpio_dt_spec gpio_error;
    const struct gpio_dt_spec gpio_debug;
};

struct status_leds_data {
};

bool status_leds_set_system_active(
    const struct device *device,
    bool value)
{
    const struct status_leds_config *config = device->config;

    int result = gpio_pin_set_dt(&config->gpio_system_active, value ? 1 : 0);

    if (result != 0) {
        LOG_ERR("setting system active LED failed with error %i", result);
        return false;
    }

    return true;
}

bool status_leds_set_debug(
    const struct device *device,
    bool value)
{
    const struct status_leds_config *config = device->config;

    int result = gpio_pin_set_dt(&config->gpio_debug, value ? 1 : 0);

    if (result != 0) {
        LOG_ERR("setting debug LED failed with error %i", result);
        return false;
    }

    return true;
}

bool status_leds_set_error(
    const struct device *device,
    bool value)
{
    const struct status_leds_config *config = device->config;

    int result = gpio_pin_set_dt(&config->gpio_error, value ? 1 : 0);

    if (result != 0) {
        LOG_ERR("setting error LED failed with error %i", result);
        return false;
    }

    return true;
}

static int status_leds_init(const struct device *device)
{
    const struct status_leds_config *config = device->config;

    LOG_DBG("initializing status leds");

    if (!device_is_ready(config->gpio_system_active.port)) {
        LOG_ERR("power system active GPIO %s is not yet ready", config->gpio_system_active.port->name);
        return -1;
    }

    if (!device_is_ready(config->gpio_error.port)) {
        LOG_ERR("power error GPIO %s is not yet ready", config->gpio_error.port->name);
        return -1;
    }

    if (!device_is_ready(config->gpio_debug.port)) {
        LOG_ERR("power debug GPIO %s is not yet ready", config->gpio_debug.port->name);
        return -1;
    }

    int result = gpio_pin_configure_dt(&config->gpio_system_active, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("pin configuration for system active GPIO failed");
        return -2;
    }

    result = gpio_pin_configure_dt(&config->gpio_error, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("pin configuration for error GPIO failed");
        return -2;
    }

    result = gpio_pin_configure_dt(&config->gpio_debug, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("pin configuration for debug GPIO failed");
        return -2;
    }

    LOG_DBG("successfully configured status leds");
    return 0;
}

#define DT_DRV_COMPAT benediktibk_status_leds

#define STATUS_LEDS_DEFINE(index)                                                                  \
    static struct status_leds_data status_leds_data_##index;                                       \
                                                                                                   \
    static struct status_leds_config status_leds_config_##index = {                                \
        .gpio_system_active = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), system_active_gpios, 0), \
        .gpio_error = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), error_gpios, 0),                 \
        .gpio_debug = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), debug_gpios, 0),                 \
    };                                                                                             \
                                                                                                   \
    DEVICE_DT_INST_DEFINE(index, status_leds_init, NULL,                                           \
                          &status_leds_data_##index,                                               \
                          &status_leds_config_##index,                                             \
                          POST_KERNEL, CONFIG_STATUS_LEDS_POST_KERNEL_INIT_PRIORITY,               \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(STATUS_LEDS_DEFINE)