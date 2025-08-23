/*
 * Copyright (c) gaiaochos.com
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h> // for the service
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/uuid.h> //for device service identification should be 128bit not 16bit
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

// Enable logs
LOG_MODULE_REGISTER(custom_service_log);

// global state
volatile bool ble_ready = false;

// variable or buffer to hold temprature read
double temparaure_value = 0;
float temp_float;

// call back function that reads the value we need to expose
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   // 1. Service offered
                                   uint16_t len, uint16_t offset);

/* Defining the advertising packets
 * An advertising packet has
 * 1. The name of the service
 * 2. Connection paramters such as intervals
 */
static const struct bt_data advert[] = {
    // enable ble and stop classic bluetooth
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    // our device name
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

/* Define a BLE service
 * takes name, primary service and secondary service
 * you can have multiple primary services
 * assign permissions
 * point to function to read
 */
BT_GATT_SERVICE_DEFINE(
    enviromental_sensing, BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x181A)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A6E),
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ, read_custom_characteristic, NULL,
                           &temp_float));

/* function to read characteristic
 * needs to be type defined as follows
 * this function is read in the rx thread and is blocking so use it like an ISR
 */
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   uint16_t len, uint16_t offset) 
{
	float *value = attr->user_data;
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

/* Error checking function to make sure ble runs well.
 * Enables us to report on Bluetooth
 */
void bt_ready(int err) {
  if (err) {
    LOG_ERR("bt enable return %d", err);
  }
  LOG_INF("bt ready!");
  ble_ready = true;
}

/* Define a Routine to collect sensor readings */
static double fetch_temp(const struct device *sensor) {
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

int main(void) {
  int err;

  /*Initialize BLE and wait until it is done*/
  err = bt_enable(bt_ready);
  while (!ble_ready) {
    LOG_WRN("bt not ready!");
    k_msleep(100);
  }

  /*When ready register callbacks to handle connections and notifications*/
  err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, advert, ARRAY_SIZE(advert), NULL,
                        0);
  if (err) {
    LOG_ERR("advertising failed to start 0x%02x", err);
  }

  /*Bluetooth setup was a success do sensor now*/
  const struct device *const temp_sensor = DEVICE_DT_GET_ANY(nordic_nrf_temp);

  if (temp_sensor == NULL) {
    LOG_ERR("Temprature sensor not found");
    return 1;
  }

  if (!device_is_ready(temp_sensor)) {
    LOG_ERR("Device (%s) is not ready", temp_sensor->name);
  }

  LOG_WRN("Updating temp every 2000 MSEC");

  while (1) {
    temparaure_value = fetch_temp(temp_sensor);
    temp_float = (float)temparaure_value;
    k_sleep(K_SECONDS(2));
  }
}
