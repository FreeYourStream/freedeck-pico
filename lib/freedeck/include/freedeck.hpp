#pragma once
#include "button.hpp"
#include <GFX.hpp>
#include <sd_card.h>
extern GFX *oled[];
extern sd_card_t *pSD;
extern FIL fil;
extern uint16_t image_data_offset;
extern uint16_t current_page;
extern Button buttons[];
void displayImage(int16_t imageNumber);
void load_page(int16_t pageIndex);
void on_button_press(uint8_t buttonIndex, uint8_t secondary);
void on_button_release(uint8_t buttonIndex, uint8_t secondary);
void check_button_state(uint8_t buttonIndex);