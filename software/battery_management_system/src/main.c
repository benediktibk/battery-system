#include <autoconf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "power_control.h"
#include "status_leds.h"

LOG_MODULE_REGISTER(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

const struct device *status_leds_device = DEVICE_DT_GET(DT_NODELABEL(status_leds));
const struct device *power_control_device = DEVICE_DT_GET(DT_NODELABEL(power_control));
const struct device *bms_temp_device = DEVICE_DT_GET(DT_NODELABEL(bms_temp));

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

    return true;
}

static bool check_bms_pcb_temperature(void)
{
    struct sensor_value sensor_value;

    int result = sensor_sample_fetch(bms_temp_device);

    if (result != 0) {
        LOG_ERR("unable to check PCB temperature of BMS");
        return false;
    }

    result = sensor_channel_get(bms_temp_device, SENSOR_CHAN_AMBIENT_TEMP, &sensor_value);

    if (result != 0) {
        LOG_ERR("unable to get PCB temperature of BMS");
        return false;
    }

    LOG_DBG("BMS PCB temperature: %i °C", sensor_value.val1);

    if (sensor_value.val1 >= CONFIG_MAXIMUM_BMS_PCB_TEMPERATURE_CELSIUS) {
        LOG_ERR("BMS PCB temperature is too high: %i °C", sensor_value.val1);
        return false;
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

int main(void)
{
    if (!check_ready()) {
        emergency_shutdown();
        return -1;
    }

    LOG_INF("starting battery management system");
    status_leds_set_system_active(status_leds_device, true);

    while (true) {
        bool error_detected = false;

        if (!check_bms_pcb_temperature()) {
            emergency_shutdown();
            return -1;
        }

        status_leds_set_error(status_leds_device, error_detected);
        k_sleep(K_MSEC(CONFIG_BMS_LOOP_TIME_MS));
    }
}
