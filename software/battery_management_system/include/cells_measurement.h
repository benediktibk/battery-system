
#ifndef BATTERY_MANAGEMENT_SYSTEM_CELLS_MEASUREMENT_H
#define BATTERY_MANAGEMENT_SYSTEM_CELLS_MEASUREMENT_H

#include <stdint.h>
#include <zephyr/device.h>

/*!
 * @brief read out cell voltage
 * 
 * @param device instance of cells measurement
 * @param index cell index
 * @param value cell voltage in mV
 * @return success
 */
bool cells_measurement_measure(
    const struct device *device,
    uint8_t index,
    uint32_t *value);

#endif