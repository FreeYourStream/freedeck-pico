#include "freedeck.hpp"
#include "button.hpp"
#include "freedeck_serial.hpp"

#include "init.hpp"
#include "pico/stdlib.h"
#include "settings.hpp"
#include "util.hpp"
#include <GFX.hpp>
#include <bsp/board.h>
#include <fd_usb.hpp>
#include <sd_card.h>
Button buttons[BD_COUNT];
GFX *oled[BD_COUNT];
sd_card_t *pSD = 0;
FIL fil;
uint16_t image_data_offset = 0;
uint16_t current_page = 0;

void press_keys() {
  char i = 0;
  uint8_t key;
  uint8_t keycode[7] = {HID_KEY_NONE};
  do {
    f_read(&fil, &key, 1, NULL);
    keycode[i] = key;
  } while (key != 0 && i++ < 7);
  setKeycode(keycode);
}

void change_page() {
  int16_t pageIndex;
  f_read(&fil, &pageIndex, 2, NULL);
  load_page(pageIndex);
}

uint8_t get_command(uint8_t button, uint8_t secondary) {
  f_lseek(&fil, (BD_COUNT * current_page + button + 1) * 16 + 8 * secondary);
  uint8_t command;
  f_read(&fil, &command, 1, NULL);
  return command;
}

void on_button_press(uint8_t buttonIndex, uint8_t secondary) {
  uint8_t command = get_command(buttonIndex, secondary) & 0xf;
  if (command == 1) {
    change_page();
  } else if (command == 0) {
    press_keys();
  } /*else if (command == 3) {
    pressSpecialKey();
  } else if (command == 4) {
    sendText();
  } else if (command == 5) {
    setSetting();
  }*/
}

void on_button_release(uint8_t buttonIndex, uint8_t secondary) {
  uint8_t command = get_command(buttonIndex, secondary) & 0xf;
  if (command == 0) {
    uint8_t keycode[6] = {HID_KEY_NONE};
    setKeycode(keycode);
  } /*else if (command == 3) {
    Consumer.releaseAll();
  }*/
}

void display_image(int16_t imageNumber) {
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
    uint8_t command = get_command(buttonIndex, false);
    // buttons[buttonIndex].onPressCallback = on_button_press; // to do: only do this initially
    // buttons[buttonIndex].onReleaseCallback = on_button_release;
    buttons[buttonIndex].hasSecondary = command > 15;
    display_image(pageIndex * BD_COUNT + buttonIndex);
  }
}

void check_button_state(uint8_t buttonIndex) {
  set_mux_address(buttonIndex, TYPE_BUTTON);
  sleep_ms(1);
  uint8_t state = gpio_get(BUTTON_PIN);
  buttons[buttonIndex].update(state);
  return;
}