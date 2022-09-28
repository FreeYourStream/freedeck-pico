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
extern uint32_t last_data_received;
extern uint16_t timeout_sec;
extern uint8_t contrast;
extern bool has_json;
void displayImage(uint16_t imageNumber);
void load_images(uint16_t pageIndex);
void load_buttons(uint16_t pageIndex);
void load_page(uint16_t pageIndex);
void on_button_press(uint8_t buttonIndex, uint8_t secondary);
void on_button_release(uint8_t buttonIndex, uint8_t secondary);
void check_button_state(uint8_t buttonIndex);
void post_setup();
void sleep_task();
void set_global_contrast(unsigned short c);
bool wake_display_if_needed();