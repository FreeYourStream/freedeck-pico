#pragma once
#include "pico/stdlib.h"
void led_blinking_task(void);
void init_usb(void);
void process_usb(void);
void set_keycode(uint8_t newKeycode[7]);
void set_special_code(uint16_t newKeycode);