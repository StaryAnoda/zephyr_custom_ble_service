#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include <zephyr/device.h>

const struct device *sensor_temp_sensor_init();
double fetch_temp(const struct device *sensor);
#endif
