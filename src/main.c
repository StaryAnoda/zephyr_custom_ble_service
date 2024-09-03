/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
unsigned long long seed = 1;
#define MODULUS    0x100000000ULL // 2^32
#define MULTIPLIER 6364136223846793005ULL
#define INCREMENT  1ULL

static unsigned long generate_rnd_number(unsigned long long s) {
  seed = s;
  seed = (MULTIPLIER * seed + INCREMENT) % MODULUS;
  return (unsigned long)(seed >> 16);
}
LOG_MODULE_REGISTER(custom_service_log);
// using any online UUID generator service we generate UUIDs
#define BT_UUID_OUR_CUSTOM_SERVICE_VAL                                         \
  BT_UUID_128_ENCODE(0x49696277, 0xf2f0, 0x47c6, 0x8854, 0xe2dc31396481)

#define BT_UUID_OUR_CUSTOM_SERVICE                                             \
  BT_UUID_DECLARE_128(BT_UUID_OUR_CUSTOM_SERVICE_VAL)

#define BT_UUID_OUR_CUSTOM_CHARACTERISTIC_VAL                                  \
  BT_UUID_128_ENCODE(0x49696277, 0xf2f0, 0x47c6, 0x8854, 0xe2dc31396482)

#define BT_UUID_OUR_CUSTOM_CHARACTERISTIC                                      \
  BT_UUID_DECLARE_128(BT_UUID_OUR_CUSTOM_CHARACTERISTIC_VAL)
volatile bool ble_ready = false;
static ATOMIC_DEFINE(connection_state, 2U);

#define CONNECTED 1U
#define DISCONNECTED 2U
uint32_t custom_value = 0;
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   uint16_t len, uint16_t offset);

// Defining the advertising packets
static const struct bt_data advert[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_OUR_CUSTOM_SERVICE_VAL),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};
BT_GATT_SERVICE_DEFINE(
    custom_service, BT_GATT_PRIMARY_SERVICE(BT_UUID_OUR_CUSTOM_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_OUR_CUSTOM_CHARACTERISTIC,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ, read_custom_characteristic, NULL,
                           NULL));
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   uint16_t len, uint16_t offset) {
  return bt_gatt_attr_read(conn, attr, buf, len, offset, &custom_value,
                           sizeof(custom_value));
}
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
  err = bt_enable(bt_ready);
  if (!ble_ready) {
    LOG_INF("bt not ready!");
    k_msleep(100);
  }
  err = bt_le_adv_start(BT_LE_ADV_CONN, advert, ARRAY_SIZE(advert), NULL, 0);
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