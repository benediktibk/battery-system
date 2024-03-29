
#ifndef BATTERY_MANAGEMENT_SYSTEM_CELL_VOLTAGE_H
#define BATTERY_MANAGEMENT_SYSTEM_CELL_VOLTAGE_H

#include <stdint.h>
#include <zephyr/device.h>

/*!
 * @brief read out cell voltage
 * 
 * @param device instance of cell voltage
 * @param value cell voltage in mV
 * @return success
 */
bool cell_voltage_measure(
    const struct device *device,
    uint16_t *value);

#endif