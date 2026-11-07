#ifndef BLE_KEYBOARD_H
#define BLE_KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

#define HID_KEY_RIGHT_ARROW  0x4F
#define HID_KEY_LEFT_ARROW   0x50

void ble_keyboard_init(void);
bool ble_keyboard_is_connected(void);
bool ble_keyboard_send_key(uint8_t keycode);

#endif