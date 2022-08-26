#pragma once
#include <pico/stdlib.h>
#define BUTTON_DOWN 0
#define BUTTON_UP 1

#define PRIMARY 0
#define PRIMARY_SECONDARY 1
#define PRIMARY_LEAVE 2

typedef void (*CallbackType)(uint8_t index, uint8_t secondary);

class Button {
public:
  uint8_t index = 0;
  bool state = BUTTON_UP;
  uint32_t pressedSince = 0;
  bool pressExecuted = false;
  bool has_secondary = false;

  CallbackType onPressCallback;
  CallbackType onReleaseCallback;

  void update(bool new_state);

private:
  void callShortPress();
  void callShortRelease();
  void callLongPress();
  void callLongRelease();
};