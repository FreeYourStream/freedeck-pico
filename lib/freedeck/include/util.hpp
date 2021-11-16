#pragma once

#include <pico/stdlib.h>
#define BUTTON_DOWN 0
#define TYPE_DISPLAY 0
#define TYPE_BUTTON 1

void o_debug(const char *message, uint display = 0);
int getBitValue(int number, int place);
void set_mux_address(int address);
void set_mux_address(int address, uint8_t type);
