#include <stdio.h>

#include "freedeck.hpp"
#include "freedeck_serial.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <bsp/board.h>
#include <fd_usb.hpp>
#include <init.hpp>
#include <logo.hpp>
#include <settings.hpp>
#include <util.hpp>
void setup() {
  stdio_init_all();
  init_mux();
  init_button();
  init_oleds();
  init_sdcard();
  init_usb();
  load_page(0);
}

void loop() {
  process_usb();
  cdc_task();
  // if (board_millis() % 3000 == 0) {
  //   fillScreen(0);
  //   fillScreen(1);
  // }
  // if (board_millis() % 3000 == 1000) {
  //   set_mux_address(0);
  //   oled->display(logo);
  //   set_mux_address(1);
  //   oled->display(logo);
  // }
  // if (board_millis() % 3000 == 2000) {
  //   set_mux_address(0);
  //   oled->invertColors(true);
  //   set_mux_address(1);
  //   oled->invertColors(true);
  // }
  for (uint8_t i = 0; i < BD_COUNT; i++) {
    check_button_state(i);
  }
  // static bool last = 0;
  // const uint32_t btn = board_button_read() || !gpio_get(19);
  // if (btn == 1 && btn != last) {
  //   uint8_t keycode[6] = {HID_KEY_CONTROL_LEFT, HID_KEY_A};
  //   setKeycode(keycode);
  //   last = btn;
  // } else if (btn == 0 && btn != last) {
  //   uint8_t keycode[6] = {HID_KEY_NONE};
  //   setKeycode(keycode);
  //   last = btn;
  // }
}

int main() {
  setup();
  while (true) {
    loop();
  }
  return 0;
}
