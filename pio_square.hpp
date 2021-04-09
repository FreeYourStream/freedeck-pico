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
  double duty_cycle;

  Pio_square(PIO p, uint8_t pin) {
    duty_cycle = 0.5;
    pio = p;
    sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &jump_program);
    jump_program_init(pio, sm, offset, pin);
    set_hz(100);
  }
  // -13 is for calibrating
  uint32_t cleanDivider() { return (divider * 2) - 13; }

  void set_hz(uint32_t hz) {
    divider = SYS_CLOCK / (hz * 2);
    write_to_sm();
  }
  void set_khz(uint16_t khz) { set_hz(khz * 1000); }

  void set_interval_us(uint32_t interval) {
    divider = (SYS_CLOCK / 1000000) * interval / 2;
    write_to_sm();
  }
  void set_interval_ms(uint32_t interval) { set_interval_us(interval * 1e3); }

  void set_duty_cycle_percent(double dc) {
    if (dc > 1)
      duty_cycle = 1;
    else if (dc < 0)
      duty_cycle = 0;
    else
      duty_cycle = dc;
    write_to_sm();
  }
  void set_duty_cycle_us(uint32_t us) {
    uint32_t onTime = (SYS_CLOCK / 1000000) * us / 2;
    duty_cycle = ((double)onTime / (double)divider);
    // printf("divider:%d, us: %d, onTime: %d, dutyCycle: %f\n", divider, us, onTime, duty_cycle);
    write_to_sm();
  }
  void set_duty_cycle_ms(uint32_t ms) { set_duty_cycle_us(ms * 1e3); }

private:
  void write_to_sm() {
    // printf("1:%d\n", (uint32_t)(cleanDivider() * duty_cycle));
    // printf("2:%d\n\n", (uint32_t)(cleanDivider() * (1.0 - duty_cycle)));
    pio_sm_put_blocking(pio, sm, (uint32_t)(cleanDivider() * duty_cycle));
    pio_sm_put_blocking(pio, sm, (uint32_t)(cleanDivider() * (1.0 - duty_cycle)));
  }
};
