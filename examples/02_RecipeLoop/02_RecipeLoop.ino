#include <Arduino.h>
#include "NSC.h"
#include "NSC_Recipe.h"

NSC::Controller nsc;
NSC::RecipeRunner runner(nsc);

// Your recipe:
// 1) LT(ZL) hold 200ms, next gap 0ms
// 2) B hold 100ms, default gap
// 3) LT(ZL) hold 200ms, next gap 0ms
// 4) X hold 100ms, default gap
static constexpr NSC::Step recipe[] = {
  { NSC::Button::ZL, 200, 0  },
  { NSC::Button::B,  100, -1 },
  { NSC::Button::ZL, 200, 0  },
  { NSC::Button::X,  100, -1 },
};

void taskRecipe(void*) {
  nsc.begin();
  runner.setDefaultGap(50);
  runner.runLoop(recipe, sizeof(recipe)/sizeof(recipe[0]));
}

void setup() {
  xTaskCreatePinnedToCore(taskRecipe, "recipe", 4096, nullptr, 1, nullptr, tskNO_AFFINITY);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
