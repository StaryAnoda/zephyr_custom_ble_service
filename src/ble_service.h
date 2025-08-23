#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H
#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

void ble_service_init(void);

extern float temp_float;

// call back function that reads the value we need to expose
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   // 1. Service offered
                                   uint16_t len, uint16_t offset);

#endif
