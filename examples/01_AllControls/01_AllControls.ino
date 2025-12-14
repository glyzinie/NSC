#include <Arduino.h>
#include "NSC.h"

NSC::Controller nsc;

void setup() {
  Serial.begin(115200);
  nsc.begin();

  // simple test: press A
  nsc.tap(NSC::Button::A, 80, 200);

  // chord: hold ZL, tap A, release ZL
  nsc.chordHoldTap(NSC::Button::ZL, NSC::Button::A, 200, 200);

  // move sticks & hat
  nsc.setHat(NSC::Hat::Right);
  nsc.setLeftStick(255, 128);
  nsc.send();
  vTaskDelay(pdMS_TO_TICKS(300));
  nsc.setHat(NSC::Hat::Center);
  nsc.centerSticks();
  nsc.send();
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
