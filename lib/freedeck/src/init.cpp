#include "init.hpp"
#include "f_util.h"
#include "freedeck.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "logo.hpp"
#include "settings.hpp"
#include "util.hpp"
#include <hw_config.h>
#include <sd_card.h>
////////////////////////7
// #include "ff.h"
// #include "hw_config.h"

void init_oleds() {
  i2c_init(i2c1, OLED_SPEED);                 // make this afap
  gpio_set_function(OLED_SDA, GPIO_FUNC_I2C); // Use GPIO2 as I2C
  gpio_set_function(OLED_SCL, GPIO_FUNC_I2C); // Use GPIO3 as I2C
  gpio_pull_up(OLED_SDA);                     // Pull up GPIO2
  gpio_pull_up(OLED_SCL);                     // Pull up GPIO3

  for (int i = 0; i < BD_COUNT; i++) {
    oled[i] = new GFX(OLED_ADDRESS, size::W128xH64, i2c1); // Declare oled instance
    set_mux_address(i);
    oled[i]->init();
    i % 2 ? oled[i]->display(logo) : oled[i]->display(fdlogo);
  }
}

void init_sdcard() {
  pSD = sd_get_by_num(0);
  FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
  if (FR_OK != fr) {
    o_debug("sdcard error 1");
    o_debug(FRESULT_str(fr), 1);
  }
  const char *const filename = CONFIG_NAME;
  fr = f_open(&fil, filename, FA_READ);
  if (FR_OK != fr && FR_EXIST != fr) {
    o_debug("sdcard error 2");
    o_debug(FRESULT_str(fr), 1);
  }
  f_lseek(&fil, 2);
  UINT read;
  char buffer[4];
  f_read(&fil, &buffer, 4, NULL);
  uint16_t num = (uint8_t)buffer[0] | (uint8_t)buffer[1] << 8;
  image_data_offset = num * 16;
}

void init_button() {
  // button pin
  gpio_set_input_enabled(BUTTON_PIN, true);
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN);

  for (int i = 0; i < BD_COUNT; i++) {
    buttons[i].index = i;
    buttons[i].onPressCallback = on_button_press; // to do: only do this initially
    buttons[i].onReleaseCallback = on_button_release;
  }
}

void init_mux() {
  // mux pin 1
  gpio_init(S0_PIN);
  gpio_set_dir(S0_PIN, GPIO_OUT);
  gpio_pull_down(S0_PIN);
  gpio_put(S0_PIN, 0);
#if BD_COUNT > 2
  // mux pin 2
  gpio_init(S1_PIN);
  gpio_set_dir(S1_PIN, GPIO_OUT);
  gpio_pull_down(S1_PIN);
  gpio_put(S1_PIN, 0);
#endif
#if BD_COUNT > 4
  // mux pin 3
  gpio_init(S2_PIN);
  gpio_set_dir(S2_PIN, GPIO_OUT);
  gpio_pull_down(S2_PIN);
  gpio_put(S2_PIN, 0);
#endif
#if BD_COUNT > 8
  // mux pin 4
  gpio_init(S3_PIN);
  gpio_set_dir(S3_PIN, GPIO_OUT);
  gpio_pull_down(S3_PIN);
  gpio_put(S3_PIN, 0);
#endif
}