#pragma once
#include "GFX.hpp"
#include "button.hpp"
#include "sd_card.h"
extern GFX *oled[];
extern sd_card_t *pSD;
extern FIL fil;
extern uint16_t image_data_offset;
extern uint16_t current_page;
extern uint16_t page_count;
extern Button buttons[];
extern uint32_t last_action;
extern uint32_t last_human_action;
extern uint16_t timeout_sec;
void displayImage(int16_t imageNumber);
void load_images(int16_t pageIndex);
void load_buttons(int16_t pageIndex);
void load_page(int16_t pageIndex);
void on_button_press(uint8_t buttonIndex, uint8_t secondary, bool leave);
void on_button_release(uint8_t buttonIndex, uint8_t secondary, bool leave);
void check_button_state(uint8_t buttonIndex);
void post_setup();
void sleep_task();
bool wake_display_if_needed();