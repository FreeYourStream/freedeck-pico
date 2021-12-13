/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "../../freedeck_serial/freedeck_serial.hpp"
#include "bsp/board.h"
#include "fd_usb.hpp"
#include "freedeck.hpp"
#include "init.hpp"
#include "pico/bootrom.h"
#include "settings.hpp"
#include "tusb.h"
#include "usb_descriptors.hpp"
#include "util.hpp"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1,
  BLINK_SUSPENDED = 0,
};

uint8_t keycode[7] = {0};
uint16_t special_code = 0;

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void set_keycode(uint8_t newKeycode[7]) {
#ifdef DEBUG
  for (uint8_t i = 0; i < 6; i++) {
    char buffer[4];
    sprintf(buffer, "%d", newKeycode[i]);
  }
#endif
  for (int i = 0; i < 6; i++) {
    keycode[i] = newKeycode[i];
  }
}

void set_special_code(uint16_t newKeycode) {
#ifdef DEBUG
  char buffer[4];
  sprintf(buffer, "%d", newKeycode);
#endif
  special_code = newKeycode;
}

/*------------- MAIN -------------*/
void init_usb(void) {
  board_init();
  tusb_init();
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) { blink_interval_ms = BLINK_MOUNTED; }

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void)itf;
  if (!dtr) {
    cdc_line_coding_t coding;
    tud_cdc_get_line_coding(&coding);

    if (coding.bit_rate == 1200) {
      reset_usb_boot(0, 0);
    }
  }
}

// Invoked when device is unmounted
void tud_umount_cb(void) { blink_interval_ms = BLINK_NOT_MOUNTED; }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
  for (int i = 0; i < BD_COUNT; i++) {
    set_mux_address(i);
    oled[i]->displayON(0);
  }
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {

  blink_interval_ms = BLINK_MOUNTED;
  for (int i = 0; i < BD_COUNT; i++) {
    set_mux_address(i);
    oled[i]->displayON(1);
  }
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report() {
  // skip if hid is not ready yet
  if (!tud_hid_ready())
    return;

  // use to avoid send multiple consecutive zero report for keyboard
  static bool has_keyboard_key = false;

  if (keycode[0] != HID_KEY_NONE) {
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
    has_keyboard_key = true;
  } else {
    // send empty key report if previously has key pressed
    if (has_keyboard_key) {
      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
    }
    has_keyboard_key = false;
  }

  static bool has_consumer_key = false;
  if (special_code != 0) {
    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &special_code, 2);
    has_consumer_key = true;
  } else {
    // send empty key report (release key) if previously has key pressed
    if (has_consumer_key)
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &special_code, 2);
    has_consumer_key = false;
  }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void process_usb(void) {
  tud_task(); // tinyusb device task
  led_blinking_task();
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms)
    return; // not enough time
  start_ms += interval_ms;

  // Remote wakeup
  if (tud_suspended()) {
    bool const btn = gpio_get(BUTTON_PIN);
    if (btn == BUTTON_DOWN)
      // Wake up host if we are in suspend mode
      // and REMOTE_WAKEUP feature is enabled by host
      tud_remote_wakeup();
  } else {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report();
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                               uint8_t *buffer, uint16_t reqlen) {
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize) {
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void) {
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) {
    board_led_write(false);
    return;
  }

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms)
    return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}