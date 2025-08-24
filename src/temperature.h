#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include <zephyr/device.h>

void sensor_temp_sensor_init();
double fetch_temp(const struct device *sensor);
void temp_sensor_thread(void *arg1 , void *arg2, void *arg3); 
#endif
