#pragma once
#include <Arduino.h>

#include "USB.h"
#include "USBHID.h"

#include "../NSC.h"

#if !CONFIG_TINYUSB_HID_ENABLED
#error "TinyUSB HID is disabled. Enable CONFIG_TINYUSB_HID_ENABLED for ESP32-S3."
#endif

namespace NSC {

// This transport embeds esp32beans/switch_ESP32 (MIT) HID descriptor & report layout.
// Report: 14 buttons + dpad + 2 sticks (4 axes).
class UsbSwitchHid {
public:
  UsbSwitchHid();
  bool begin();
  void end();

  void sendState(const State& st);

private:
  // dpad values expected by descriptor
  static constexpr uint8_t DPAD_CENTERED = 0x0F;

  // Matches esp32beans/switch_ESP32 report struct
  typedef struct __attribute__((packed)) {
    uint16_t buttons;
    uint8_t  dPad;
    uint8_t  leftXAxis;
    uint8_t  leftYAxis;
    uint8_t  rightXAxis;
    uint8_t  rightYAxis;
    uint8_t  filler;
  } HID_NSGamepadReport_Data_t;

  class NSGamepad : public USBHIDDevice {
  public:
    NSGamepad();
    void begin();
    void end();
    bool write();
    bool write(void* report, size_t len);

    HID_NSGamepadReport_Data_t report{};
    uint16_t _onGetDescriptor(uint8_t* dst) override;
  protected:
    USBHID hid;
  };

  NSGamepad pad_;

  static uint8_t mapHat(Hat h);
  static uint16_t mapButtons(const State& st);
};

} // namespace NSC
