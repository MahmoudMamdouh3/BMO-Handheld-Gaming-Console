#pragma once
// -----------------------------------------------------------------------
// display_ui.h - Owns the ST7789 display instance and all screen drawing
// for this test milestone. Later milestones (SD test, emulator) get
// their own UI modules rather than growing this one indefinitely.
// -----------------------------------------------------------------------

#include <Adafruit_ST7789.h>

namespace DisplayUI {
  // Call once in setup(). Initializes the panel and clears the screen.
  void begin();

  // Draws the static title text once.
  void drawTitle(const char *text);

  // Draws the static button name labels once.
  void drawButtonGrid();

  // Call once per loop(). Redraws only buttons whose state changed
  // this frame (avoids full-screen redraw flicker/cost).
  void updateButtonGrid();
}
