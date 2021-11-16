#include "Button.hpp"
#include "pico/stdlib.h"
#include "settings.hpp"
#include <bsp/board.h>

void Button::update(bool new_state) {
  // if the button is being held down and we are waiting for
  // the secondary function to fire
  if (state == BUTTON_DOWN && state == new_state) { // still being pressed down
    if (!hasSecondary)
      return;
    if (longPressExecuted == true)
      return;

    uint32_t now = board_millis();
    uint32_t passedTime = now - pressedSince;

    if (passedTime < LONG_PRESS_DURATION)
      return;
    longPressExecuted = true;
    callLongPress();

  } else if (state == BUTTON_UP && new_state == BUTTON_DOWN) { // getting pressed down
    state = new_state;
    if (hasSecondary) {
      // to decide if we need to execute long or short press
      // start couting for how long we press the button
      pressedSince = board_millis();
    } else {
      callShortPress();
    }
  } else if (state == BUTTON_DOWN && new_state == BUTTON_UP) { // getting released
    state = new_state;
    if (hasSecondary) {
      uint32_t now = board_millis();
      uint32_t passedTime = now - pressedSince;
      pressedSince = 0;
      if (passedTime < LONG_PRESS_DURATION) {
        callShortPress();
        sleep_ms(100);
        callShortRelease();
      } else {
        longPressExecuted = false;
        callLongRelease();
      }
    } else {
      callShortRelease();
    }
  }
}

void Button::callShortPress() {
  if (onPressCallback != NULL)
    onPressCallback(index, false);
}
void Button::callShortRelease() {
  if (onReleaseCallback != NULL)
    onReleaseCallback(index, false);
}
void Button::callLongPress() {
  if (onPressCallback != NULL)
    onPressCallback(index, true);
}
void Button::callLongRelease() {
  if (onReleaseCallback != NULL)
    onReleaseCallback(index, true);
}