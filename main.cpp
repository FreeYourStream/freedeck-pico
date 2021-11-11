#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include <GFX.hpp>
#include <keyboard.h>
#include <logo.hpp>
// check this
// https://github.com/hathach/tinyusb/blob/master/examples/device/webusb_serial/src/main.c
GFX *oled = 0;

void setup() {
  stdio_init_all();
  // gpio_init(25);
  // gpio_set_dir(25, GPIO_OUT);
  // uart_init(uart0, 115200);
  init_hid();
  // i2c_init(i2c1, 400000);                     // make this afap
  // gpio_set_function(2, GPIO_FUNC_I2C);        // Use GPIO2 as I2C
  // gpio_set_function(3, GPIO_FUNC_I2C);        // Use GPIO3 as I2C
  // gpio_pull_up(2);                            // Pull up GPIO2
  // gpio_pull_up(3);                            // Pull up GPIO3
  // oled = new GFX(0x3C, size::W128xH64, i2c1); // Declare oled instance
  // oled->display(logo);
  // sleep_ms(2000);
}

void loop() {
  process_hid();
  static bool last = 0;
  uint32_t const btn = board_button_read();
  if (btn == 1 && btn != last) {
    uint8_t keycode[6] = {HID_KEY_CONTROL_LEFT, HID_KEY_A};
    setKeycode(keycode);
    last = btn;
  } else if (btn == 0 && btn != last) {
    uint8_t keycode[6] = {HID_KEY_NONE};
    setKeycode(keycode);
    last = btn;
  }
  // sleep_ms(1000);
  // oled->clear(); // Clear buffer
  // oled->drawString(0, 0, "Raspberry Pico");
  // oled->drawString(0, 10, "Oled Example");
  // oled->drawString(0, 20, "Have fun!");
  // oled->drawProgressBar(0, oled->getHeight() - 5, oled->getWidth(), 5, rand() % 100 + 1);

  // oled->display(); // Send buffer to the screen
}

int main() {
  setup();
  while (true) {
    loop();
  }
  return 0;
}
