#include "NSC.h"
#include "transport/NSC_UsbSwitchHid.h"

namespace NSC {

static UsbSwitchHid g_usb;

bool Controller::begin() {
  centerSticks();
  releaseAll();
  setHat(Hat::Center);
  return g_usb.begin();
}

void Controller::end() { g_usb.end(); }

void Controller::press(Button b)   { st_.btn[idx(b)] = true; }
void Controller::release(Button b) { st_.btn[idx(b)] = false; }
void Controller::set(Button b, bool on) { st_.btn[idx(b)] = on; }

void Controller::releaseAll() {
  for (int i = 0; i < buttonCount(); i++) st_.btn[i] = false;
}

void Controller::setHat(Hat h) { st_.hat = h; }

void Controller::setLeftStick(uint8_t x, uint8_t y)  { st_.lx = x; st_.ly = y; }
void Controller::setRightStick(uint8_t x, uint8_t y) { st_.rx = x; st_.ry = y; }

void Controller::centerSticks() {
  st_.lx = st_.ly = st_.rx = st_.ry = 128;
}

void Controller::send() { g_usb.sendState(st_); }

void Controller::tap(Button b, uint16_t hold_ms, uint16_t gap_ms) {
  press(b);  send();
  vTaskDelay(pdMS_TO_TICKS(hold_ms));
  release(b); send();
  if (gap_ms) vTaskDelay(pdMS_TO_TICKS(gap_ms));
}

void Controller::chordHoldTap(Button hold, Button tap, uint16_t tap_ms, uint16_t after_ms) {
  press(hold); send();
  press(tap);  send();
  vTaskDelay(pdMS_TO_TICKS(tap_ms));
  release(tap); send();
  if (after_ms) vTaskDelay(pdMS_TO_TICKS(after_ms));
  release(hold); send();
}

} // namespace NSC
