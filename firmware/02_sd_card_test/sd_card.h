#pragma once
// -----------------------------------------------------------------------
// sd_card.h - SD card detection and file listing module.
//
// Handles SD card initialization on the shared SPI bus and provides
// card info (type, size) and root directory listing.
// -----------------------------------------------------------------------

#include <Arduino.h>

namespace SDCard {
  // Call once in setup(), AFTER the shared SPI bus is initialized.
  // Returns true if the card was detected and mounted.
  bool begin();

  // Re-scan the card (e.g. after inserting a new one).
  // Returns true if the card is now detected.
  bool rescan();

  // True if a card is currently mounted.
  bool isMounted();

  // Card info (only valid if isMounted() is true).
  const char *cardType();
  float cardSizeGB();

  // Count of files in the root directory.
  int fileCount();

  // Get the name of a file in the root directory by index.
  // Returns empty string if index is out of range.
  const char *fileName(int index);
}
