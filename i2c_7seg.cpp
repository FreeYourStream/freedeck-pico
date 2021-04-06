#include <stdio.h>
#include <string.h>

#include "lib/tm1637.hpp"
#include "pico/stdlib.h"

void setup() {
	// internal led
	gpio_init(25);
	gpio_set_dir(25, GPIO_OUT);
	gpio_put(25, 1);

	// bitbang 7 segment
	TM1637 display;
	display.setNumber(1637);

	for (uint8_t b = 0; b != 8; ++b) {
		display.setBrightness(b);
		sleep_ms(500);
	}
	for (int i = 0; i <= 9999; i++) {
		display.setNumberPad(i, 3, 4, 0);
		sleep_ms(100);
	}
}

void loop() {}

int main() {
	stdio_init_all();
	setup();
	while (true) {
		loop();
	}
	return 0;
}
