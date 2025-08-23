#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include "temperature.h"

LOG_MODULE_REGISTER(sensor_module);

const struct device *sensor_temp_sensor_init() {

  const struct device *temp_sensor = DEVICE_DT_GET_ANY(nordic_nrf_temp);

  if (temp_sensor == NULL) {
    LOG_ERR("Temprature sensor not found");
    return NULL;
  }

  if (!device_is_ready(temp_sensor)) {
    LOG_ERR("Device (%s) is not ready", temp_sensor->name);
    return NULL;
  }

  return temp_sensor;
}

/* Define a Routine to collect sensor readings */
double fetch_temp(const struct device *sensor) {
  struct sensor_value temp;
  int rc = sensor_sample_fetch(sensor);

  if (rc == -EBADMSG) {
    /*In polling we don't care*/
    rc = 0;
  }

  if (rc == 0U) {
    rc = sensor_channel_get(sensor, SENSOR_CHAN_DIE_TEMP, &temp);
  }

  if (rc != 0U) {
    LOG_ERR("Error: Getting Temp channel fromsensor (%d)", rc);
  }

  return sensor_value_to_double(&temp);
}
