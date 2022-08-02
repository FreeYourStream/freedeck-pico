#pragma once
#include <pico/stdlib.h>
void init_sdcard();
void init_oleds(uint8_t oled_speed, uint8_t pre_charge_period, uint8_t refresh_frequency);
void init_button();
void init_mux();