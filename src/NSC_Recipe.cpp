#include "NSC_Recipe.h"

namespace NSC {

void RecipeRunner::runOnce(const Step* steps, size_t n) {
  for (size_t i = 0; i < n; i++) {
    const auto& s = steps[i];

    c_.press(s.button);
    c_.send();
    vTaskDelay(pdMS_TO_TICKS(s.hold_ms));

    c_.release(s.button);
    c_.send();

    uint16_t gap = (s.gap_ms < 0) ? default_gap_ms_ : (uint16_t)s.gap_ms;
    if (gap) vTaskDelay(pdMS_TO_TICKS(gap));
  }
}

void RecipeRunner::runLoop(const Step* steps, size_t n) {
  while (true) {
    runOnce(steps, n);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

} // namespace NSC
