/*
 * Copyright (c) gaiaochos.com
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h> // for the service
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/uuid.h> //for device service identification should be 128bit not 16bit
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* naive implementation for random number for sensor simulation
 * TODO since the NRF52840 has a temperature sensor, lets use that
 */
unsigned long long seed = 1;
#define MODULUS 0x100000000ULL // 2^32
#define MULTIPLIER 6364136223846793005ULL
#define INCREMENT 1ULL

static unsigned long generate_rnd_number(unsigned long long s) {
  seed = s;
  seed = (MULTIPLIER * seed + INCREMENT) % MODULUS;
  return (unsigned long)(seed >> 16);
}

/*End Sensor simulation*/

// Enable logs
LOG_MODULE_REGISTER(custom_service_log);

// global state
volatile bool ble_ready = false;
// variable or buffer to hold info
uint32_t custom_value = 0;

// call back function that reads the value we need to expose
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   uint16_t len, uint16_t offset);

// Defining the advertising packets
// An advertising packet has
// 1. Service offered
// 2. The name of the service
// 3. Connection paramters such as intervals
static const struct bt_data advert[] = {
    // enable ble and stop classic bluetooth
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    // our services
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_OUR_CUSTOM_SERVICE_VAL),
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
                           &custom_value));

// function to read characteristic
// needs to be type defined as follows
// this function is read in the rx thread and is blocking so use it like an ISR
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   uint16_t len, uint16_t offset) {
  LOG_WRN("We got a read!");
  return bt_gatt_attr_read(conn, attr, buf, len, offset, &custom_value,
                           sizeof(custom_value));
}

// Error checking function to make sure ble runs well.
// Enables us to report on Bluetooth
void bt_ready(int err) {
  if (err) {
    LOG_ERR("bt enable return %d", err);
  }
  LOG_INF("bt ready!");
  ble_ready = true;
}

int main(void) {
  int err;
  LOG_INF("initializing bt");
  // Initialize BLE
  err = bt_enable(bt_ready);
  while (!ble_ready) {
    LOG_INF("bt not ready!");
    k_msleep(100);
  }

  // IF BLE ready advertise device and services
  // You can register callbacks to handle connections and notifications
  err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, advert, ARRAY_SIZE(advert), NULL,
                        0);
  if (err) {
    LOG_ERR("advertising failed to start 0x%02x",
            err); // bt_hci_err_to_str(err));
  }
  seed = 34449;
  while (1) {
    custom_value = generate_rnd_number(seed);
    k_sleep(K_SECONDS(2));
  }
}
