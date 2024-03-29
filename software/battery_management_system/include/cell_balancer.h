
#ifndef BATTERY_MANAGEMENT_SYSTEM_CELL_BALANCER_H
#define BATTERY_MANAGEMENT_SYSTEM_CELL_BALANCER_H

#include <stdint.h>
#include <zephyr/device.h>

/*!
 * @brief disable cell balancer
 * 
 * @param device instance of cell balancer
 * @return success
 */
bool cell_balancer_disable(
    const struct device *device);
/*!
 * @brief enable cell balancer
 * 
 * @param device instance of cell balancer
 * @param index index of cell where energy should be taken from
 * @return success
 */
bool cell_balancer_enable(
    const struct device *device,
    uint8_t index);

#endif