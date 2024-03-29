
#ifndef BATTERY_MANAGEMENT_SYSTEM_CELLS_MEASUREMENT_H
#define BATTERY_MANAGEMENT_SYSTEM_CELLS_MEASUREMENT_H

#include <stdint.h>
#include <zephyr/device.h>

#define CELL_COUNT 4

struct cell_voltages {
    uint16_t value[CELL_COUNT]; // in mV
};

/*!
 * @brief read out cell voltages
 * 
 * @param device instance of cells measurement
 * @param values cell voltages
 * @return success
 */
bool cells_measurement_measure(
    const struct device *device,
    struct cell_voltages *cell_voltages);

#endif