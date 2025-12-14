#include "NSC_UsbSwitchHid.h"
#include <cstring>

namespace NSC {

// --- esp32beans/switch_ESP32 report descriptor (MIT) ---
static const uint8_t report_descriptor[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x05,        // Usage (Game Pad)
  0xA1, 0x01,        // Collection (Application)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x35, 0x00,        //   Physical Minimum (0)
  0x45, 0x01,        //   Physical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x0E,        //   Report Count (14)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x0E,        //   Usage Maximum (0x0E)
  0x81, 0x02,        //   Input (Data,Var,Abs)
  0x95, 0x02,        //   Report Count (2)
  0x81, 0x01,        //   Input (Const,Array,Abs)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x25, 0x07,        //   Logical Maximum (7)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
  0x09, 0x39,        //   Usage (Hat switch)
  0x81, 0x42,        //   Input (Data,Var,Abs,Null State)
  0x65, 0x00,        //   Unit (None)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x01,        //   Input (Const,Array,Abs)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x46, 0xFF, 0x00,  //   Physical Maximum (255)
  0x09, 0x30,        //   Usage (X)
  0x09, 0x31,        //   Usage (Y)
  0x09, 0x32,        //   Usage (Z)
  0x09, 0x35,        //   Usage (Rz)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x04,        //   Report Count (4)
  0x81, 0x02,        //   Input (Data,Var,Abs)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x01,        //   Input (Const,Array,Abs)
  0xC0              // End Collection
};

// esp32beans button index order (bit positions):
// 0:Y 1:B 2:A 3:X 4:L 5:R 6:ZL 7:ZR 8:- 9:+ 10:L3 11:R3 12:Home 13:Capture
enum NSButtons : uint8_t {
  NSButton_Y = 0,
  NSButton_B,
  NSButton_A,
  NSButton_X,
  NSButton_LeftTrigger,   // L
  NSButton_RightTrigger,  // R
  NSButton_LeftThrottle,  // ZL
  NSButton_RightThrottle, // ZR
  NSButton_Minus,
  NSButton_Plus,
  NSButton_LeftStick,
  NSButton_RightStick,
  NSButton_Home,
  NSButton_Capture,
};

UsbSwitchHid::NSGamepad::NSGamepad() : hid() {
  static bool initialized = false;

  // HORI Wired Controller for Nintendo Switch
  USB.VID(0x0f0d);
  USB.PID(0x00c1);
  USB.usbClass(0);
  USB.usbSubClass(0);
  USB.usbProtocol(0);

  end();
  if (!initialized) {
    initialized = true;
    hid.addDevice(this, sizeof(report_descriptor));
  }
}

uint16_t UsbSwitchHid::NSGamepad::_onGetDescriptor(uint8_t* dst) {
  memcpy(dst, report_descriptor, sizeof(report_descriptor));
  return sizeof(report_descriptor);
}

void UsbSwitchHid::NSGamepad::begin() {
  hid.begin();
  end();
}

void UsbSwitchHid::NSGamepad::end() {
  memset(&report, 0x00, sizeof(report));
  report.leftXAxis  = 0x80;
  report.leftYAxis  = 0x80;
  report.rightXAxis = 0x80;
  report.rightYAxis = 0x80;
  report.dPad       = DPAD_CENTERED;
}

bool UsbSwitchHid::NSGamepad::write() {
  return hid.SendReport(0, &report, sizeof(report));
}

bool UsbSwitchHid::NSGamepad::write(void* r, size_t len) {
  memcpy(&report, r, std::min(len, sizeof(report)));
  return write();
}

// --- UsbSwitchHid (NSC transport) ---
UsbSwitchHid::UsbSwitchHid() : pad_() {}

bool UsbSwitchHid::begin() {
  USB.begin();     // required on ESP32-S3 USB device
  pad_.begin();
  return true;
}

void UsbSwitchHid::end() {
  pad_.end();
}

uint8_t UsbSwitchHid::mapHat(Hat h) {
  switch (h) {
    case Hat::Up:        return 0;
    case Hat::UpRight:   return 1;
    case Hat::Right:     return 2;
    case Hat::DownRight: return 3;
    case Hat::Down:      return 4;
    case Hat::DownLeft:  return 5;
    case Hat::Left:      return 6;
    case Hat::UpLeft:    return 7;
    case Hat::Center:    return DPAD_CENTERED;
  }
  return DPAD_CENTERED;
}

uint16_t UsbSwitchHid::mapButtons(const State& st) {
  uint16_t bits = 0;

  auto setBit = [&](uint8_t idx, bool on) {
    if (on) bits |= (1u << idx);
  };

  // NSC Button -> esp32beans bit order
  setBit(NSButton_A,       st.btn[idx(Button::A)]);
  setBit(NSButton_B,       st.btn[idx(Button::B)]);
  setBit(NSButton_X,       st.btn[idx(Button::X)]);
  setBit(NSButton_Y,       st.btn[idx(Button::Y)]);

  setBit(NSButton_LeftTrigger,  st.btn[idx(Button::L)]);
  setBit(NSButton_RightTrigger, st.btn[idx(Button::R)]);
  setBit(NSButton_LeftThrottle, st.btn[idx(Button::ZL)]);
  setBit(NSButton_RightThrottle,st.btn[idx(Button::ZR)]);

  setBit(NSButton_LeftStick,  st.btn[idx(Button::L3)]);
  setBit(NSButton_RightStick, st.btn[idx(Button::R3)]);

  setBit(NSButton_Plus,   st.btn[idx(Button::PLUS)]);
  setBit(NSButton_Minus,  st.btn[idx(Button::MINUS)]);
  setBit(NSButton_Home,   st.btn[idx(Button::HOME)]);
  setBit(NSButton_Capture,st.btn[idx(Button::CAPTURE)]);

  return bits;
}

void UsbSwitchHid::sendState(const State& st) {
  HID_NSGamepadReport_Data_t r{};
  r.buttons    = mapButtons(st);
  r.dPad       = mapHat(st.hat);
  r.leftXAxis  = st.lx;
  r.leftYAxis  = st.ly;
  r.rightXAxis = st.rx;
  r.rightYAxis = st.ry;
  r.filler     = 0x00;

  pad_.write(&r, sizeof(r));
}

} // namespace NSC
