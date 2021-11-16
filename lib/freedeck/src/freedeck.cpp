#include "init.hpp"
#include "pico/stdlib.h"
#include "settings.hpp"
#include "util.hpp"
#include <GFX.hpp>
#include <sd_card.h>

GFX *oled[BD_COUNT];
sd_card_t *pSD = 0;
FIL fil;
uint32_t image_data_offset = 0;
uint32_t current_page = 0;

void displayImage(int16_t imageNumber) {
  uint8_t display_number = imageNumber % BD_COUNT;
  f_lseek(&fil, image_data_offset + imageNumber * 1024L);
  unsigned char buffer[1024];
  f_read(&fil, &buffer, 1024, NULL);
  set_mux_address(display_number, TYPE_DISPLAY);
  oled[display_number]->display(buffer);
}

void load_page(int16_t pageIndex) {
  current_page = pageIndex;
  for (uint8_t buttonIndex = 0; buttonIndex < BD_COUNT; buttonIndex++) {
    // uint8_t command = getCommand(buttonIndex, false);
    // buttons[buttonIndex].hasSecondary = command > 15;
    // buttons[buttonIndex].onPressCallback = onButtonPress; // to do: only do this initially
    // buttons[buttonIndex].onReleaseCallback = onButtonRelease;

    displayImage(pageIndex * BD_COUNT + buttonIndex);
  }
}
