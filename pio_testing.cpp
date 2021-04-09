#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pio_square.hpp"

double duty_cycle = 0.01;
double modifier = 0.01;

void setup() {
  stdio_init_all();
  uart_init(uart0, 115200);
  sleep_ms(2000);
  Pio_square freq = Pio_square(pio0, 25);
  freq.set_khz(5);
  while (true) {
    if (duty_cycle > 1 || duty_cycle < 0)
      modifier *= -1;
    freq.set_duty_cycle_percent(duty_cycle);
    duty_cycle += modifier;
    sleep_ms(10);
  }
}

void loop() {}

int main() {
  setup();
  while (true) {
    loop();
  }
  return 0;
}
