
#ifndef BATTERY_MANAGEMENT_SYSTEM_STATUS_LEDS_H
#define BATTERY_MANAGEMENT_SYSTEM_STATUS_LEDS_H

#include <stdbool.h>
#include <zephyr/device.h>

/*!
 * @brief set state of system active LED
 * 
 * @param device instance of status leds
 * @param value value for LED
 * @return success
 */
bool status_leds_set_system_active(
    const struct device *device,
    bool value);
/*!
 * @brief set state of error LED
 * 
 * @param device instance of status leds
 * @param value value for LED
 * @return success
 */
bool status_leds_set_error(
    const struct device *device,
    bool value);
/*!
 * @brief set state of debug LED
 * 
 * @param device instance of status leds
 * @param value value for LED
 * @return success
 */
bool status_leds_set_debug(
    const struct device *device,
    bool value);

#endif