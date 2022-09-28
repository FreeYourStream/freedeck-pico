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
uint32_t last_data_received = 0;
uint32_t last_action = 0;
uint32_t last_human_action = 0;
Button buttons[BD_COUNT];
GFX *oled[BD_COUNT];
sd_card_t *pSD = 0;
FIL fil;
uint16_t image_data_offset = 0;
uint16_t current_page = 0;
uint16_t next_page = 0;
uint16_t page_count = 0;
uint8_t contrast = 0;
uint16_t timeout_sec = TIMEOUT_TIME;
uint8_t oled_speed = OLED_SPEED;
uint8_t pre_charge_period = PRE_CHARGE_PERIOD;
uint8_t refresh_frequency = REFRESH_FREQUENCY;
bool has_json = true;

bool woke_display = 0;

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
  char j = -1;
  uint8_t key;
  uint8_t keycode[61] = {HID_KEY_NONE};
  do {
    f_read(&fil, &key, 1, NULL);
    if (key > 0x80 && key < 0xe0) {
      sleep_ms(15);
      keycode[j] = HID_KEY_NONE;
      set_keycode(keycode);
      sleep_ms(15);
    } else {
      keycode[++j] = key;
      set_keycode(keycode);
    }

  } while (key != 0 && i++ < 61);
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

void emit_button_press(uint8_t button_index, bool secondary) {
  write_serial(0x3);
  write_serial_line("");
  write_serial(0x10);
  write_serial_line("");
  char f_size_str[10];
  sprintf(f_size_str, "%d\t%d\t%d", current_page, button_index, secondary);
  write_serial_line(f_size_str);
}

void emit_page_change(uint16_t page_index) {
  write_serial(0x3);
  write_serial_line("");
  write_serial(0x20);
  write_serial_line("");
  write_serial_number(page_index);
}

uint16_t get_target_page(uint8_t buttonIndex, uint8_t secondary) {
  f_lseek(&fil,
          (BD_COUNT * current_page + buttonIndex + 1) * ROW_SIZE + (ROW_SIZE / 2 * secondary) + 1);
  uint16_t pageIndex;
  f_read(&fil, &pageIndex, 2, NULL);
  return pageIndex;
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
  f_lseek(&fil, (BD_COUNT * current_page + button + 1) * ROW_SIZE + (ROW_SIZE / 2) * secondary);
  uint8_t command;
  f_read(&fil, &command, 1, NULL);
  return command;
}

void on_button_press(uint8_t button_index, uint8_t secondary) {
  last_human_action = board_millis();
  woke_display = wake_display_if_needed();
  if (woke_display)
    return;
  uint8_t command = get_command(button_index, secondary) & 0xf;
  if (command == 0) {
    press_keys();
  } else if (command == 1) {
    next_page = get_target_page(button_index, secondary);
    load_images(next_page);
  } else if (command == 3) {
    press_special_key();
  } else if (command == 4) {
    send_text();
  } else if (command == 5) {
    set_setting();
  } else if (command == 6) {
    emit_button_press(button_index, secondary);
  }
}

void on_button_release(uint8_t buttonIndex, uint8_t secondary) {
  last_human_action = board_millis();
  if (woke_display) {
    woke_display = false;
    return;
  }
  uint8_t command = get_command(buttonIndex, secondary) & 0xf;
  if (command == 0) {
    uint8_t keycode[6] = {HID_KEY_NONE};
    set_keycode(keycode);
  } else if (command == 1) {
    current_page = next_page;
    load_buttons(current_page);
  } else if (command == 3) {
    set_special_code(HID_KEY_NONE);
  }
  f_lseek(&fil, (BD_COUNT * current_page + buttonIndex + 1) * ROW_SIZE +
                    (ROW_SIZE / (2 - secondary)) - 2);
  uint16_t page_index;
  f_read(&fil, &page_index, 2, NULL);
  if (page_index > 0) {
    load_page(page_index - 1);
  }
}

void display_image(uint16_t imageNumber) {
  uint8_t display_number = imageNumber % BD_COUNT;
  f_lseek(&fil, image_data_offset + imageNumber * 1025L);
  uint8_t has_live_data;
  f_read(&fil, &has_live_data, 1, NULL);
  if (has_live_data == 1 && (board_millis() - last_data_received) < 2000)
    return;
  unsigned char buffer[1024];
  f_read(&fil, &buffer, 1024, NULL);
  set_mux_address(display_number, TYPE_DISPLAY);
  oled[display_number]->display(buffer);
}

void load_images(uint16_t page_index) {
  emit_page_change(page_index);
  // current_page = page_index;
  for (uint8_t buttonIndex = 0; buttonIndex < BD_COUNT; buttonIndex++) {
    display_image(page_index * BD_COUNT + buttonIndex);
  }
}

void load_buttons(uint16_t page_index) {
  for (uint8_t buttonIndex = 0; buttonIndex < BD_COUNT; buttonIndex++) {
    uint8_t command = get_command(buttonIndex, false);
    uint8_t second_command = get_command(buttonIndex, true);
    buttons[buttonIndex].has_secondary = second_command != 2;
  }
}

void load_page(uint16_t page_index) {
  current_page = page_index;
  load_images(page_index);
  load_buttons(page_index);
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
  image_data_offset = num * ROW_SIZE;
  page_count = (num - 1) / BD_COUNT;

  f_read(&fil, &contrast, 1, NULL);

  char timeout_buf[2];
  f_read(&fil, &timeout_buf, 2, NULL);
  timeout_sec = timeout_buf[0] | timeout_buf[1] << 8;

  f_read(&fil, &oled_speed, 1, NULL);
  f_read(&fil, NULL, 1, NULL);
  f_read(&fil, &pre_charge_period, 1, NULL);
  f_read(&fil, &refresh_frequency, 1, NULL);
  f_read(&fil, &has_json, 1, NULL);

  if (oled_speed == 0)
    oled_speed = OLED_SPEED / 10000;
  if (pre_charge_period == 0)
    pre_charge_period = PRE_CHARGE_PERIOD;
  if (refresh_frequency == 0)
    refresh_frequency = REFRESH_FREQUENCY;
}

void post_setup() {
  load_header_info();
  init_oleds(oled_speed, pre_charge_period, refresh_frequency);
  set_global_contrast(contrast);
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