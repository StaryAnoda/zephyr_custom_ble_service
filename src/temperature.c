#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include "app.h"
#include "temperature.h"

LOG_MODULE_REGISTER(sensor_module);

static const struct device *temp_sensor;

void sensor_temp_sensor_init() { 
	 temp_sensor = DEVICE_DT_GET_ANY(nordic_nrf_temp);

  if (!temp_sensor || !device_is_ready(temp_sensor)) {
    LOG_ERR("Device (%s) is not ready", temp_sensor->name);
  }

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

/*define a thread to read temps every 2s*/
void temp_sensor_thread(void *arg1 , void *arg2, void *arg3) 
{
	while(1) {
		struct current_temp_msg curr_message;

		float curr_temp = (float)fetch_temp(temp_sensor); 
		curr_message.value  = curr_temp;

		/*Push the work  into the queue*/
		if ((k_msgq_put(&tempmsgq, &curr_message, K_NO_WAIT) != 0)) {
			//Queue is full here  drop the oldest 
			k_msgq_purge(&tempmsgq);
			k_msgq_put(&tempmsgq, &curr_message, K_NO_WAIT); 
		}

		/*Wait 2 seconds and run again*/
		k_sleep(K_SECONDS(2));	
	}
}

