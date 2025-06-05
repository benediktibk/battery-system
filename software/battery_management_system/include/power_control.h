
#ifndef BATTERY_MANAGEMENT_SYSTEM_POWER_CONTROL_H
#define BATTERY_MANAGEMENT_SYSTEM_POWER_CONTROL_H

#include <stdbool.h>
#include <zephyr/device.h>

/*!
 * @brief set charge
 * 
 * @param device instance of cell voltage
 * @param value value for charge
 * @return success
 */
bool power_control_set_charge(
    const struct device *device,
    bool value);
/*!
 * @brief set discharge
 * 
 * @param device instance of cell voltage
 * @param value value for discharge
 * @return success
 */
bool power_control_set_discharge(
    const struct device *device,
    bool value);

#endif