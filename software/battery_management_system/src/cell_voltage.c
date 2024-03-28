#include "cell_voltage.h"

#include <autoconf.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

struct cell_voltage_config {
    const struct device *adc;
    uint8_t adc_channel;
    uint8_t voltage_divider_upper;
    uint8_t voltage_divider_lower;
};

struct cell_voltage_data {
};

bool cell_voltage_measure(
    const struct device *device,
    uint32_t *value)
{
    const struct cell_voltage_config *config = device->config;
    int32_t buffer;
    struct adc_channel_cfg channel_cfg = {
        .gain = ADC_GAIN_1,
        .reference = ADC_REF_INTERNAL,
        .acquisition_time = ADC_ACQ_TIME_DEFAULT,
        .channel_id = config->adc_channel,
        .differential = 0
    };
    struct adc_sequence sequence = {
        .channels = BIT(config->adc_channel),
        .buffer = &buffer,
        .buffer_size = sizeof(buffer),
        .resolution = 12,
        .oversampling = 0,
        .calibrate = false,
    };

    int result = adc_channel_setup(
        config->adc,
        &channel_cfg);

    if (result != 0) {
        LOG_ERR("unable to setup ADC channel");
        return false;
    }

    result = adc_read(
        config->adc,
        &sequence);

    if (result != 0) {
        LOG_ERR("unable to read from ADC");
        return false;
    }

    result = adc_raw_to_millivolts(
        adc_ref_internal(config->adc),
        channel_cfg.gain,
        sequence.resolution,
        &buffer);

    *value = buffer * (config->voltage_divider_upper + config->voltage_divider_lower) / config->voltage_divider_lower;

    return true;
}

static int cell_voltage_init(const struct device *device)
{
    const struct cell_voltage_config *config = device->config;

    LOG_DBG("initializing cell voltage");

    if (!device_is_ready(config->adc)) {
        LOG_ERR("ADC %s is not yet ready", config->adc->name);
        return -1;
    }

    LOG_DBG("successfully configured cell voltage");
    return 0;
}

#define DT_DRV_COMPAT benediktibk_cell_voltage

#define CELL_VOLTAGE_DEFINE(index)                                                    \
    static struct cell_voltage_data cell_voltage_data_##index;                        \
                                                                                      \
    static struct cell_voltage_config cell_voltage_config_##index = {                 \
        .adc = DEVICE_DT_GET(DT_INST_PROP(index, adc)),                               \
        .adc_channel = DT_INST_PROP(index, adc_channel),                              \
        .voltage_divider_upper = DT_INST_PROP(index, voltage_divider_upper),          \
        .voltage_divider_lower = DT_INST_PROP(index, voltage_divider_lower),          \
    };                                                                                \
                                                                                      \
    DEVICE_DT_INST_DEFINE(index, cell_voltage_init, NULL,                             \
                          &cell_voltage_data_##index,                                 \
                          &cell_voltage_config_##index,                               \
                          POST_KERNEL, CONFIG_CELL_VOLTAGE_POST_KERNEL_INIT_PRIORITY, \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(CELL_VOLTAGE_DEFINE)