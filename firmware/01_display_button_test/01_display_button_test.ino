// -----------------------------------------------------------------------
// Milestone 1: Display + Button bring-up test
//
// Verifies the ST7789 display and all 8 buttons (Up/Down/Left/Right,
// A/B, Start/Select) are wired correctly, with live visual feedback on
// the screen itself (no Serial Monitor required, though it's mirrored
// there too). No SD card, no emulator logic - isolated hardware test.
//
// Libraries needed (Arduino Library Manager):
//   "Adafruit ST7789"
//   "Adafruit GFX Library"
//
// See config.h for pin wiring and docs/wiring/01-display-and-buttons.md
// for the full wiring reference.
// -----------------------------------------------------------------------

#include "config.h"
#include "buttons.h"
#include "display_ui.h"

void setup() {
  Serial.begin(115200);
  delay(300);

  Buttons::begin();
  DisplayUI::begin();
  DisplayUI::drawTitle("Display+Button Test");
  DisplayUI::drawButtonGrid();

  Serial.println("Milestone 1 ready. Press buttons to test.");
}

void loop() {
  Buttons::update();
  DisplayUI::updateButtonGrid();

  for (int i = 0; i < Buttons::count(); i++) {
    const ButtonState &b = Buttons::get(i);
    if (b.changed) {
      Serial.printf("%s -> %s\n", b.name, b.pressed ? "PRESSED" : "released");
    }
  }

  delay(20);
}
