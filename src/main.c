/*
 * Copyright (c) gaiaochos.com
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ble_service.h"
#include "temperature.h"

// Enable logs
LOG_MODULE_REGISTER(custom_service_log);

// global state
float temp_float;

// variable or buffer to hold temprature read
double temparature_value = 0;

int main(void) {
  /*Blocking bluetooth init*/
  ble_service_init();

  /*Bluetooth setup was a success do sensor now*/
  const struct device *temp_sensor = sensor_temp_sensor_init();
  if (temp_sensor == NULL) {
    LOG_ERR("Sensor initialization failed");
    return 1;
  }

  LOG_WRN("Updating temp every 2000 MSEC");

  while (1) {
    temparature_value = fetch_temp(temp_sensor);
    temp_float = (float)temparature_value;
    k_sleep(K_SECONDS(2));
  }
}
