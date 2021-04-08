#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pio_square.hpp"

void setup() {
  stdio_init_all();
  uart_init(uart0, 115200);
  // internal led
  gpio_init(27);
  gpio_set_dir(27, GPIO_OUT);
  gpio_put(27, 1);

  Pio_square freq(pio0, 27);
  freq.set_khz(123);
}

void loop() {}

int main() {
  setup();
  while (true) {
    loop();
  }
  return 0;
}
