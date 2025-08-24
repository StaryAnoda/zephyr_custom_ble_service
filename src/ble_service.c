#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ble_module);

#include "app.h"
#include "ble_service.h"


volatile bool ble_ready = false;
float temp_float;

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
                                   uint16_t len, uint16_t offset) {
  float *value = attr->user_data;
  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

void ble_service_init(void) {
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
}


/*BLE Threads*/

/*Read Current Temp Thread*/
void ble_temp_read_thread(void *arg1, void *arg2, void *arg3) {
	struct current_temp_msg curr_temp; 

	while(1) {
		/*Try to read the queue until it has something*/
		if (k_msgq_get(&tempmsgq, &curr_temp, K_FOREVER) == 0 ) {
			temp_float = curr_temp.value;
		}
	}
}
