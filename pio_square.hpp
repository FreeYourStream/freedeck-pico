#include "hardware/gpio.h"
#include "jump.pio.h"
#include "pico/stdlib.h"
#include "stdio.h"

#ifndef SYS_CLOCK
#define SYS_CLOCK 125000000
#endif

class Pio_square {
public:
  PIO pio;
  uint8_t sm;
  uint32_t divider;
  Pio_square(PIO p, uint8_t pin) {
    pio = p;
    sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &jump_program);
    jump_program_init(pio, sm, offset, pin);
    set_hz(100);
  }
  // we have 4 cycles that have nothing to do with waiting
  // so we subtract them
  uint32_t cleanDivider() { return divider - 4; }

  void set_hz(uint32_t hz) {
    divider = SYS_CLOCK / (hz * 2);
    pio_sm_put_blocking(pio, sm, cleanDivider());
  }
  void set_khz(uint16_t khz) { set_hz(khz * 1000); }

  void set_interval_us(uint32_t interval) {
    divider = (SYS_CLOCK / 1000000) * interval / 2;
    pio_sm_put_blocking(pio, sm, cleanDivider());
  }
  void set_interval_ms(uint32_t interval) { set_interval_us(interval * 1e3); }
};
