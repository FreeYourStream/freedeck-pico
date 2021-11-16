#include "pico/stdlib.h"
#define BUTTON_DOWN 0
#define BUTTON_UP 1
typedef void (*CallbackType)(uint8_t index, uint8_t secondary);

class Button {
public:
  uint8_t index = 0;
  bool state = BUTTON_UP;
  uint32_t pressedSince = 0;
  bool longPressExecuted = false;
  bool hasSecondary = 0;

  CallbackType onPressCallback;
  CallbackType onReleaseCallback;

  void update(bool new_state);

private:
  void callShortPress();
  void callShortRelease();
  void callLongPress();
  void callLongRelease();
};