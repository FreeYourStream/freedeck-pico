#pragma once
#define DEBUG 1

#define BD_COUNT 6
#define CONFIG_NAME "config.bin"
#define CONFIG_NAME_TEMP "config.tmp"
#define CONFIG_NAME_OLD "config.bin.old"
#define LONG_PRESS_DURATION 250
#define TIMEOUT_TIME 5L * 60L // timeout in seconds
#define WAKE_ON_SERIAL true
// #define CUSTOM_ORDER

#define BUTTON_PIN 19

#define S0_PIN 20
#define S1_PIN 21
#define S2_PIN 22
// #define S3_PIN 5

#define OLED_SDA 2
#define OLED_SCL 3
#define OLED_ADDRESS 0x3C

// for faster displays
#define OLED_SPEED 1200000
// for slower displays
// #define OLED_SPEED 500000

// for faster sdcards
#define SD_MHZ 16
// for slower sdcards
// #define SD_MHZ 5
#define OK "ok"
#define ERROR "error"

// for ryan aukes 5x3 pcb layout or
// if your screens are not in 1..n order
// #define CUSTOM_ORDER
#ifdef CUSTOM_ORDER
#define ADDRESS_TO_SCREEN                                                                          \
  { 13, 11, 8, 5, 2, 14, 10, 7, 4, 1, 12, 9, 6, 3, 0 }
#define ADDRESS_TO_BUTTON                                                                          \
  { 12, 14, 6, 5, 2, 11, 13, 7, 4, 1, 10, 9, 8, 3, 0 }
#endif