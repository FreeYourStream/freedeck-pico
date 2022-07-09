#include "freedeck.hpp"
#include "button.hpp"
#include "freedeck_serial.hpp"

#include "GFX.hpp"
#include "init.hpp"
#include "settings.hpp"
#include "util.hpp"
#include <bsp/board.h>
#include <fd_usb.hpp>
#include <pico/stdlib.h>
uint32_t last_action = 0;
Button buttons[BD_COUNT];
GFX *oled[BD_COUNT];
sd_card_t *pSD = 0;
FIL fil;
uint16_t image_data_offset = 0;
uint16_t current_page = 0;
uint16_t page_count = 0;
uint8_t contrast = 0;
uint16_t timeout_sec = TIMEOUT_TIME;

void send_text() {

  char i = 0;
  char key_index = 0;
  uint8_t key;
  uint8_t keycode[7] = {HID_KEY_NONE};
  uint32_t ms = board_millis();
  do {
    f_read(&fil, &key, 1, NULL);
    keycode[key_index] = key;
    set_keycode(keycode);
    sleep_ms(10);
    key_index++;
    if (key < 224) {
      for (char j = 0; j <= key_index; j++) {
        keycode[j] = HID_KEY_NONE;
      }
      set_keycode(keycode);
      key_index = 0;
      sleep_ms(10);
    }
  } while (key != 0 && i++ < 15);
  o_debug(board_millis() - ms);
  sleep_ms(11);
}
void press_keys() {
  char i = 0;
  uint8_t key;
  uint8_t keycode[7] = {HID_KEY_NONE};
  do {
    f_read(&fil, &key, 1, NULL);
    keycode[i] = key;
  } while (key != 0 && i++ < 7);
  set_keycode(keycode);
  sleep_ms(11);
}

void press_special_key() {
  char i = 0;
  uint8_t buffer[2];
  uint16_t key;
  f_read(&fil, &buffer, 2, NULL);
  key = buffer[0] | buffer[1] << 8;
  set_special_code(key);
}

void change_page() {
  uint16_t pageIndex;
  f_read(&fil, &pageIndex, 2, NULL);
  load_page(pageIndex);
}
void set_global_contrast(unsigned short c) {
  if (c == 0)
    c = 1;
  contrast = c;
  for (uint8_t buttonIndex = 0; buttonIndex < BD_COUNT; buttonIndex++) {
    set_mux_address(buttonIndex, TYPE_DISPLAY);
    oled[buttonIndex]->setContrast(c);
  }
}

bool wake_display_if_needed() {
  if (timeout_sec == 0) {
    last_action = board_millis();
    return false;
  }
  if (board_millis() - last_action > (timeout_sec * 1000L)) {
    for (uint8_t i = 0; i < BD_COUNT; i++) {
      set_mux_address(i);
      oled[i]->displayON(true);
    }
    last_action = board_millis();
    return true;
  }
  last_action = board_millis();
  return false;
}

void set_setting() {
  uint8_t settingCommand;
  f_read(&fil, &settingCommand, 1, NULL);
  if (settingCommand == 1) { // decrease brightness
    contrast = std::max(contrast - 20, 1);
    set_global_contrast(contrast);
  } else if (settingCommand == 2) { // increase brightness
    contrast = std::min(contrast + 20, 255);
    set_global_contrast(contrast);
  } else if (settingCommand == 3) { // set brightness
    f_read(&fil, &contrast, 1, NULL);
    set_global_contrast(contrast);
  }
}

uint8_t get_command(uint8_t button, uint8_t secondary) {
  f_lseek(&fil, (BD_COUNT * current_page + button + 1) * 16 + 8 * secondary);
  uint8_t command;
  f_read(&fil, &command, 1, NULL);
  return command;
}

void on_button_press(uint8_t buttonIndex, uint8_t secondary, bool leave) {
  if (wake_display_if_needed())
    return;
  uint8_t command = get_command(buttonIndex, secondary) & 0xf;
  if (command == 1) {
    change_page();
  } else if (command == 0) {
    press_keys();
  } else if (command == 3) {
    press_special_key();
  } else if (command == 5) {
    set_setting();
  } else if (command == 4) {
    send_text();
  }
}

void on_button_release(uint8_t buttonIndex, uint8_t secondary, bool leave) {
  uint8_t command = get_command(buttonIndex, secondary) & 0xf;
  if (command == 0) {
    uint8_t keycode[6] = {HID_KEY_NONE};
    set_keycode(keycode);
  } else if (command == 3) {
    set_special_code(HID_KEY_NONE);
  }
  if (leave) {
    f_lseek(&fil, (BD_COUNT * current_page + buttonIndex + 1) * 16 + 8);
    change_page();
  }
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
    buttons[buttonIndex].mode = command >> 4;
    display_image(pageIndex * BD_COUNT + buttonIndex);
  }
}

void check_button_state(uint8_t buttonIndex) {
  set_mux_address(buttonIndex, TYPE_BUTTON);
  sleep_ms(1);
  uint8_t state = gpio_get(BUTTON_PIN);

  buttons[buttonIndex].update(state);
}

void load_header_info() {
  f_lseek(&fil, 2);
  UINT read;
  char buffer[4];
  f_read(&fil, &buffer, 2, NULL);
  uint16_t num = (uint8_t)buffer[0] | (uint8_t)buffer[1] << 8;
  image_data_offset = num * 16;
  page_count = (num - 1) / BD_COUNT;
  char contrast_buf;
  f_read(&fil, &contrast_buf, 1, NULL);
  set_global_contrast(contrast_buf);

  char timeout_buf[2];
  f_read(&fil, &timeout_buf, 2, NULL);
  timeout_sec = timeout_buf[0] | timeout_buf[1] << 8;
}

void post_setup() {
  load_header_info();
  load_page(0);
}

void sleep_task() {
  if (timeout_sec == 0)
    return;
  if (board_millis() - last_action > (timeout_sec * 1000L)) {
    for (uint8_t i = 0; i < BD_COUNT; i++) {
      set_mux_address(i);
      oled[i]->displayON(false);
    }
  }
}