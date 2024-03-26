#include <autoconf.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "status_leds.h"

LOG_MODULE_REGISTER(battery_management_system, CONFIG_BATTERY_MANAGEMENT_SYSTEM_LOG_LEVEL);

const struct device *status_leds_device = DEVICE_DT_GET(DT_NODELABEL(status_leds));

int main(void)
{
    LOG_INF("starting battery management system");
    status_leds_set_system_active(status_leds_device, true);
}
