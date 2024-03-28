#include "power_control.h"

#include <autoconf.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

struct power_control_config {
    const struct gpio_dt_spec charge_gpio;
    const struct gpio_dt_spec discharge_gpio;
    const struct gpio_dt_spec charge_available_gpio;
};

struct power_control_data {
};

bool power_control_set_charge(
    const struct device *device,
    bool value)
{
    const struct power_control_config *config = device->config;

    int result = gpio_pin_set_dt(&config->charge_gpio, value ? 1 : 0);

    if (result != 0) {
        LOG_ERR("unable to set charge GPIO");
        return false;
    }

    return true;
}

bool power_control_set_discharge(
    const struct device *device,
    bool value)
{
    const struct power_control_config *config = device->config;

    int result = gpio_pin_set_dt(&config->discharge_gpio, value ? 1 : 0);

    if (result != 0) {
        LOG_ERR("unable to set discharge GPIO");
        return false;
    }

    return true;
}

bool power_control_get_charge_available(
    const struct device *device,
    bool *value)
{
    const struct power_control_config *config = device->config;

    int result = gpio_pin_get_dt(&config->charge_available_gpio);

    if (result < 0) {
        LOG_ERR("failed to read charge available GPIO");
        return false;
    }

    *value = result == 1;

    return true;
}

static int power_control_init(const struct device *device)
{
    const struct power_control_config *config = device->config;

    LOG_DBG("initializing power control");

    if (!device_is_ready(config->charge_gpio.port)) {
        LOG_ERR("GPIO %s is not yet ready", config->charge_gpio.port->name);
        return -1;
    }

    if (!device_is_ready(config->discharge_gpio.port)) {
        LOG_ERR("GPIO %s is not yet ready", config->discharge_gpio.port->name);
        return -1;
    }

    if (!device_is_ready(config->charge_available_gpio.port)) {
        LOG_ERR("GPIO %s is not yet ready", config->charge_available_gpio.port->name);
        return -1;
    }

    int result = gpio_pin_configure_dt(&config->charge_gpio, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("failed to configure charge GPIO");
        return -2;
    }

    result = gpio_pin_configure_dt(&config->discharge_gpio, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("failed to configure discharge GPIO");
        return -2;
    }

    result = gpio_pin_configure_dt(&config->charge_available_gpio, GPIO_INPUT);

    if (result != 0) {
        LOG_ERR("failed to configure charge available GPIO");
        return -2;
    }

    LOG_DBG("successfully configured power control");
    return 0;
}

#define DT_DRV_COMPAT benediktibk_power_control

#define POWER_CONTROL_DEFINE(index)                                                                      \
    static struct power_control_data power_control_data_##index;                                         \
                                                                                                         \
    static struct power_control_config power_control_config_##index = {                                  \
        .charge_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), charge_gpios, 0),                     \
        .discharge_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), discharge_gpios, 0),               \
        .charge_available_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), charge_available_gpios, 0), \
    };                                                                                                   \
                                                                                                         \
    DEVICE_DT_INST_DEFINE(index, power_control_init, NULL,                                               \
                          &power_control_data_##index,                                                   \
                          &power_control_config_##index,                                                 \
                          POST_KERNEL, CONFIG_POWER_CONTROL_POST_KERNEL_INIT_PRIORITY,                   \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(POWER_CONTROL_DEFINE)