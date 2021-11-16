#pragma once
#include <GFX.hpp>
#include <sd_card.h>
extern GFX *oled[];
extern sd_card_t *pSD;
extern FIL fil;
extern uint16_t image_data_offset;
extern uint16_t current_page;

void displayImage(int16_t imageNumber);
void load_page(int16_t pageIndex);