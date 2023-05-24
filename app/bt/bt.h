#ifndef BT_H
#define BT_H
#include "../inc/primary_deps.h"
// bluetooth specifications

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>

int bluetooth_init(void);
void on_connected(struct bt_conn *conn, uint8_t err);
void on_disconnect(struct bt_conn *conn, uint8_t reason);
// void update_phy(struct bt_conn* conn);
// void on_le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param);

#define BT_UUID BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x18649ede, 0x817c, 0x48ed, 0x9c00, 0xb6e7e6ebe168))
#define BT_LED BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x4b015bbb, 0x5e83, 0x4d1f, 0xbc84, 0x4d613edcc99a))
#define BT_BUTTON BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x1232d82c, 0x9395, 0x42bb, 0x89e0, 0x573615cb4f80))




 int send_button_state_indicate(bool button_state);
#endif