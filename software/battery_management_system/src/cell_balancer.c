#include "cell_balancer.h"

#include <autoconf.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

struct cell_balancer_config {
    const struct gpio_dt_spec enable_gpio;
    const struct gpio_dt_spec select_a0_gpio;
    const struct gpio_dt_spec select_a1_gpio;
};

struct cell_balancer_data {
};

bool cell_balancer_disable(
    const struct device *device)
{
    const struct cell_balancer_config *config = device->config;

    int result = gpio_pin_set_dt(&config->enable_gpio, 0);

    if (result != 0) {
        LOG_ERR("unable to disable cell balancer");
        return false;
    }

    return true;
}

bool cell_balancer_enable(
    const struct device *device,
    uint8_t index)
{
    const struct cell_balancer_config *config = device->config;

    if (index > 3) {
        LOG_ERR("invalid cell %i selected", index);
        return false;
    }

    int result = gpio_pin_set_dt(&config->select_a0_gpio, index & BIT(0));

    if (result != 0) {
        LOG_ERR("unable to set A0 for cell balancer");
        return false;
    }

    result = gpio_pin_set_dt(&config->select_a1_gpio, index & BIT(1));

    if (result != 0) {
        LOG_ERR("unable to set A1 for cell balancer");
        return false;
    }

    result = gpio_pin_set_dt(&config->enable_gpio, 1);

    if (result != 0) {
        LOG_ERR("unable to enable cell balancer");
        return false;
    }

    return true;
}

static int cell_balancer_init(const struct device *device)
{
    const struct cell_balancer_config *config = device->config;

    LOG_DBG("initializing cell balancer");

    if (!device_is_ready(config->enable_gpio.port)) {
        LOG_ERR("enable GPIO %s is not yet ready", config->enable_gpio.port->name);
        return -2;
    }

    if (!device_is_ready(config->select_a0_gpio.port)) {
        LOG_ERR("select A0 GPIO %s is not yet ready", config->select_a0_gpio.port->name);
        return -2;
    }

    if (!device_is_ready(config->select_a1_gpio.port)) {
        LOG_ERR("select A1 GPIO %s is not yet ready", config->select_a1_gpio.port->name);
        return -2;
    }

    int result = gpio_pin_configure_dt(&config->enable_gpio, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("configuring enable GPIO failed");
        return -3;
    }

    result = gpio_pin_configure_dt(&config->select_a0_gpio, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("configuring select A0 GPIO failed");
        return -3;
    }

    result = gpio_pin_configure_dt(&config->select_a1_gpio, GPIO_OUTPUT_INACTIVE);

    if (result != 0) {
        LOG_ERR("configuring select A1 GPIO failed");
        return -3;
    }

    LOG_DBG("successfully configured cell balancer");
    return 0;
}

#define DT_DRV_COMPAT benediktibk_cell_balancer

#define CELL_BALANCER_DEFINE(index)                                                             \
    static struct cell_balancer_data cell_balancer_data_##index;                                \
                                                                                                \
    static struct cell_balancer_config cell_balancer_config_##index = {                         \
        .enable_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), enable_gpios, 0),            \
        .select_a0_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), cell_select_a0_gpios, 0), \
        .select_a1_gpio = GPIO_DT_SPEC_GET_BY_IDX(DT_DRV_INST(index), cell_select_a1_gpios, 0), \
    };                                                                                          \
                                                                                                \
    DEVICE_DT_INST_DEFINE(index, cell_balancer_init, NULL,                                      \
                          &cell_balancer_data_##index,                                          \
                          &cell_balancer_config_##index,                                        \
                          POST_KERNEL, CONFIG_CELL_BALANCER_POST_KERNEL_INIT_PRIORITY,          \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(CELL_BALANCER_DEFINE)