
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
/*!
 * @brief check if charge is available
 * 
 * @param device instance of cell voltage
 * @param value true if charge is available
 * @return success
 */
bool power_control_get_charge_available(
    const struct device *device,
    bool *value);

#endif