#include "freedeck_serial.hpp"
#include "GFX.hpp"
#include "bsp/board.h"
#include "fd_usb.hpp"
#include "freedeck.hpp"
#include "init.hpp"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "settings.hpp"
#include "stdint.h"
#include "stdlib.h"
#include "tusb.h"
#include "util.hpp"
////////////////////////7
// #include "f_util.h"
// #include "ff.h"
// #include "hw_config.h"
void write_serial_line(const char *line) {
  const uint32_t len = strlen(line) + 3;
  char lineCRN[len];
  strcpy(lineCRN, line);
  lineCRN[len - 3] = '\r';
  lineCRN[len - 2] = '\n';
  lineCRN[len - 1] = '\0';
  tud_cdc_write_str(lineCRN);
  tud_cdc_write_flush();
}

void write_serial_char(const char chr) {
  tud_cdc_write_char(chr);
  tud_cdc_write_flush();
}

void write_serial(const char *line) {
  tud_cdc_write_str(line);
  tud_cdc_write_flush();
}

uint32_t combine_bytes(char *numbers, uint8_t len) {
  unsigned long int number = 0;
  for (char i = 0; i < len; i++) {
    if (numbers[i] == 13)
      break;
    number |= numbers[i] << (i * 8);
  }
  return number;
}

uint32_t read_serial_binary() {
  uint32_t available = tud_cdc_available();
  while (!available) {
    available = tud_cdc_available();
    process_usb();
  }
  char buf[available];
  for (int i = 0; i < available; i++) {
    buf[i] = tud_cdc_read_char();
    if (buf[i] == '\n' || buf[i] == '\r') {
      buf[i] = '\0';
      return combine_bytes(buf, available);
    }
  }
  return INT32_MAX;
}

char *read_serial_string(char *serial_string, uint32_t max_len) {
  uint32_t available = tud_cdc_available();
  while (!available) {
    available = tud_cdc_available();
    process_usb();
  }
  char buf[available];
  if (available == 0)
    return serial_string;
  for (int i = 0; i < available; i++) {

    buf[i] = tud_cdc_read_char();
    if (buf[i] == '\n' || buf[i] == '\r') {
      buf[i] = '\0';
      strncpy(serial_string, buf, max_len);
      return serial_string;
    }
  }
  return serial_string;
}

void sdcard() {

  process_usb();
  load_page(0);
  // if (f_printf(&fil, "Hello, world!\n") < 0) {
  //   printf("f_printf failed\n");
  // }
  // fr = f_close(&fil);
  // if (FR_OK != fr) {
  //   tud_cdc_write_str("CLOSE KAPUTT\n");
  //   tud_cdc_write_flush();
  // }
}

void oled_command(uint32_t input) {
  uint8_t display = input % 0x10;
  input = input / 0x10;
  uint8_t arg = input % 0x10;
  input = input / 0x10;
  uint8_t command = input % 0x10;
  if (command == 0) {
    if (!arg) {
      set_mux_address(display);
      oled[display]->initOled();
    } else {
      for (int i = 0; i < BD_COUNT; i++) {
        set_mux_address(i);
        oled[display]->initOled();
      }
    }
  }
  if (command == 1) {
    if (arg < 2) {
      set_mux_address(display);
      oled[display]->displayON(arg);
    } else {
      for (int i = 0; i < BD_COUNT; i++) {
        set_mux_address(i);
        oled[i]->displayON(arg - 2);
      }
    }
  }
}

void oled_write_line(uint32_t command) {
  uint8_t row = command % 0x10;
  command = command / 0x10;
  uint8_t display = command % 0x10;
  command = command / 0x10;
  uint8_t font_size = command % 0x10;
  font_size > 1 ? font_size : 1;
  uint8_t max_len = 10;
  char input[max_len + 1];
  memset(input, '\0', sizeof(input));

  read_serial_string(input, max_len);
  if (input) {
    set_mux_address(display);
    oled[display]->drawString(0, 8 * row, input, font_size);
    oled[display]->display();
  }
}

void serial_api(uint32_t command) {
  if (command == 0x10) {
    write_serial_line("3.0.0P");
  }
  if (command == 0x11) {
    sdcard();
  }
  if (command >= 0x1000 && command < 0x2000) {
    oled_command(command);
  }
  if (command >= 0x2000 && command < 0x3000) {
    oled_write_line(command);
  }
}

void cdc_task(void) {
  if (!tud_cdc_connected())
    return;
  if (!tud_cdc_available())
    return;
  uint32_t command = read_serial_binary();
  serial_api(command);
}