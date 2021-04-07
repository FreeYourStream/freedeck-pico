#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/regs/addressmap.h"
#include "hardware/regs/rosc.h"
#include "lib/tm1637.hpp"
#include "lib/tm1637font.h"
#include "pico/stdlib.h"
#include "tm1637.pio.h"

#define ADDRESS 0xc0 << 16
#define ADDR_FIX 0x44 << 8
#define CONTROL 0x88 << 0
PIO pio = pio0;
uint8_t brightness = 8;
char *get_digits(uint32_t number) {
  char *digits = (char *)malloc(sizeof(char) * 4); // create array to store the digits
  memset(digits, TM_DIGITS[36], sizeof(char) * 4); // initializes this array
                                                   // with the empty 7-segment char
  int i = 0;
  while (number) {
    digits[i++] = number % 10;
    number /= 10;
  }
  return digits;
}

uint32_t rnd(void) {
  int k, random = 0;
  volatile uint32_t *rnd_reg = (uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

  for (k = 0; k < 32; k++) {

    random = random << 1;
    random = random + (0x00000001 & (*rnd_reg));
  }
  return random;
}

void display_digit(uint8_t digit, uint8_t address) {
  uint32_t data = ADDRESS | ADDR_FIX | CONTROL;
  data |= TM_DIGITS[digit] << 24; // set character to display
  data |= (address << 16);        // set address
  data |= brightness;
  pio_sm_put_blocking(pio0, 0, data);
}

void display_number(uint16_t number) {
  char is_first_digit = true;
  char *places = get_digits(number);
  char arr_len = sizeof(places) / sizeof(places[0]);
  printf("%d\n", arr_len);

  free(places);

  for (char i = 0; i < 4; i++) {

    if (places[3 - i] == 0 && is_first_digit) {
      pio_sm_put_blocking(pio0, 0, 0xc0448f | (i << 16) | 0x00 << 24);
    } else {
      pio_sm_put_blocking(pio0, 0, 0xc0448f | (i << 16) | TM_DIGITS[places[3 - i]] << 24);
      is_first_digit = false;
    }
  }
}

void display_random_number() {
  uint32_t random = rnd();
  srand(random);
  char rnd_char = rand() % (220 + 1);
  display_number(rnd_char);
}

void setup() {
  // internal led
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  gpio_put(25, 1);

  uint offset = pio_add_program(pio, &tm1637_program);
  tm1637_program_init(pio, 0, offset, 3, 2);

  stdio_init_all();

  // // Light up all segments:
  // pio_sm_put_blocking(pio0, 0, 0xffc0448a);
  // pio_sm_put_blocking(pio0, 0, 0xffc1448a);
  // pio_sm_put_blocking(pio0, 0, 0xffc2448a);
  // pio_sm_put_blocking(pio0, 0, 0xffc3448a);

  sleep_ms(3000);

  // // Wait 3 seconds, then print 123 with max brightness:
  // pio_sm_put_blocking(pio0, 0, 0x3fc0448f);
  // pio_sm_put_blocking(pio0, 0, 0x06c1448f);
  // pio_sm_put_blocking(pio0, 0, 0x5bc2448f);
  // pio_sm_put_blocking(pio0, 0, 0x4fc3448f);
}

void loop() {
  // display_random_number();
  // sleep_ms(250);
}

int main() {
  setup();
  while (true) {
    loop();
  }
  return 0;
}
