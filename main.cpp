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
  init_oleds();
  init_sdcard();
  init_usb();
  post_setup();
}

void loop2() {
  while (true) {
    cdc_task();
    sleep_task();
    for (uint8_t i = 0; i < BD_COUNT; i++) {
      check_button_state(i);
    }
  }
}

void loop() { process_usb(); }

int main() {
  setup();
  multicore_launch_core1(loop2);
  while (true) {
    loop();
  }
  return 0;
}
