#pragma once
#include <Arduino.h>
#include "NSC.h"

namespace NSC {

struct Step {
  Button button;
  uint16_t hold_ms;
  int16_t gap_ms = -1; // -1: use default gap, 0: no gap
};

class RecipeRunner {
public:
  explicit RecipeRunner(Controller& c) : c_(c) {}

  void setDefaultGap(uint16_t ms) { default_gap_ms_ = ms; }

  void runOnce(const Step* steps, size_t n);
  void runLoop(const Step* steps, size_t n);

private:
  Controller& c_;
  uint16_t default_gap_ms_ = 50;
};

} // namespace NSC
