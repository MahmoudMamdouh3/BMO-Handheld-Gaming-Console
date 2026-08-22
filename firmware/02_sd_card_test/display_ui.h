#pragma once
// -----------------------------------------------------------------------
// display_ui.h - Owns the ST7789 display instance and all screen drawing
// for the SD card test milestone. Shows SD card status at the top and
// button grid below.
// -----------------------------------------------------------------------

#include <Adafruit_ST7789.h>

namespace DisplayUI {
  // Call once in setup(), AFTER SPI.begin() has been called.
  void begin();

  // Draw the static title text.
  void drawTitle(const char *text);

  // Draw the SD card status section.
  void drawSDStatus(bool detected, const char *cardType, float sizeGB,
                    int fileCount);

  // Draw filenames from the SD card.
  void drawFileList(int count);

  // Draw the static button name labels.
  void drawButtonGrid();

  // Redraw only buttons whose state changed this frame.
  void updateButtonGrid();

  // Show a status message (e.g. "Scanning SD..." or "Press [A] to rescan")
  void drawStatusMessage(const char *msg);
}
