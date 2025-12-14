#pragma once
#include <Arduino.h>

namespace NSC {

enum class Button : uint16_t {
  // Face
  A, B, X, Y,
  // Shoulder
  L, R, ZL, ZR,
  // Stick press
  L3, R3,
  // System
  PLUS, MINUS, HOME, CAPTURE,
};

enum class Hat : uint8_t {
  Center = 0,
  Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft,
};

struct State {
  bool btn[14] = {false};  // aligned with idx(Button)
  Hat  hat     = Hat::Center;

  // 0..255 (128 = center)
  uint8_t lx = 128, ly = 128;
  uint8_t rx = 128, ry = 128;
};

inline constexpr int buttonCount() { return 14; }

inline constexpr int idx(Button b) {
  switch (b) {
    case Button::A: return 0;
    case Button::B: return 1;
    case Button::X: return 2;
    case Button::Y: return 3;
    case Button::L: return 4;
    case Button::R: return 5;
    case Button::ZL: return 6;
    case Button::ZR: return 7;
    case Button::L3: return 8;
    case Button::R3: return 9;
    case Button::PLUS: return 10;
    case Button::MINUS: return 11;
    case Button::HOME: return 12;
    case Button::CAPTURE: return 13;
  }
  return 0;
}

class Controller {
public:
  // NOTE: Requires Arduino-ESP32 with TinyUSB HID enabled on ESP32-S3.
  // begin() calls USB.begin() internally.
  bool begin();
  void end();

  void press(Button b);
  void release(Button b);
  void set(Button b, bool on);
  void releaseAll();

  void setHat(Hat h);
  void setLeftStick(uint8_t x, uint8_t y);
  void setRightStick(uint8_t x, uint8_t y);
  void centerSticks();

  void send();

  // Helpers
  void tap(Button b, uint16_t hold_ms = 50, uint16_t gap_ms = 50);

  // hold (press & keep) + tap + release hold
  void chordHoldTap(Button hold, Button tap, uint16_t tap_ms = 50, uint16_t after_ms = 0);

  const State& state() const { return st_; }

private:
  State st_;
};

} // namespace NSC
