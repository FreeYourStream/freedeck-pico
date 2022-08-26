#include <stdio.h>

#include "freedeck.hpp"
#include "freedeck_serial.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
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
  init_oleds(OLED_SPEED, PRE_CHARGE_PERIOD, REFRESH_FREQUENCY);
  init_sdcard();
  init_usb();
  post_setup();
}

void main_loop() {
  while (true) {
    cdc_task();
    sleep_task();
    for (uint8_t i = 0; i < BD_COUNT; i++) {
      check_button_state(i);
    }
  }
}

void loop_usb() {
  while (true) {
    process_usb();
  }
}

int main() {
  setup();
  multicore_launch_core1(main_loop);
  loop_usb();
  return 0;
}
