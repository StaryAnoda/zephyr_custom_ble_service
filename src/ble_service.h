#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H
#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

void ble_service_init(void);

extern int16_t temp_celsius_x100;

// call back function that reads the value we need to expose
ssize_t read_custom_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   // 1. Service offered
                                   uint16_t len, uint16_t offset);

void ble_temp_read_thread(void *arg1, void *arg2, void *arg3); 
#endif
