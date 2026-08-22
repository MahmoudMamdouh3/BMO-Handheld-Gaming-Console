// -----------------------------------------------------------------------
// Milestone 2: SD Card + Display + Button test
//
// Verifies the SD card module works on the shared SPI bus alongside the
// ST7789 display (the #1 gotcha: SD.begin() can kill the display if the
// SPI bus isn't explicitly initialized with our pin mapping first).
// Also keeps button testing live so we confirm nothing broke.
//
// Libraries needed (Arduino Library Manager):
//   "Adafruit ST7789"
//   "Adafruit GFX Library"
//   "SD" (built-in with ESP32 board package)
//
// See config.h for pin wiring and docs/hardware-notes.md for the shared
// SPI bus lesson learned.
// -----------------------------------------------------------------------

#include <SPI.h>
#include "config.h"
#include "buttons.h"
#include "sd_card.h"
#include "display_ui.h"

void doSDScan() {
  DisplayUI::drawStatusMessage("Scanning SD card...");
  bool ok = SDCard::isMounted() ? SDCard::rescan() : SDCard::begin();
  DisplayUI::drawSDStatus(ok, SDCard::cardType(), SDCard::cardSizeGB(),
                          SDCard::fileCount());
  if (ok) {
    DisplayUI::drawFileList(SDCard::fileCount());
    DisplayUI::drawStatusMessage("[A] rescan  |  SD OK");
    Serial.println("SD card detected.");
    Serial.printf("  Type: %s\n", SDCard::cardType());
    Serial.printf("  Size: %.1f GB\n", SDCard::cardSizeGB());
    Serial.printf("  Files in root: %d\n", SDCard::fileCount());
    for (int i = 0; i < SDCard::fileCount(); i++) {
      Serial.printf("    %s\n", SDCard::fileName(i));
    }
  } else {
    DisplayUI::drawFileList(0);
    DisplayUI::drawStatusMessage("[A] rescan  |  No card");
    Serial.println("SD card not found. Check wiring / insert card.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  // CRITICAL: Initialize the shared SPI bus with our real pin mapping
  // BEFORE any device tries to use it. This prevents SD.begin() from
  // claiming default pins and killing the display.
  SPI.begin(TFT_SCK, SD_MISO, TFT_MOSI);

  Buttons::begin();
  DisplayUI::begin();
  DisplayUI::drawTitle("SD+Button Test");

  // Try to mount the SD card
  doSDScan();

  // Draw the button grid below
  DisplayUI::drawButtonGrid();

  Serial.println("Milestone 2 ready. Press [A] to rescan SD card.");
}

void loop() {
  Buttons::update();
  DisplayUI::updateButtonGrid();

  // Press A to rescan SD card (for hot-insert testing)
  const ButtonState &btnA = Buttons::get(4);  // index 4 = A button
  if (btnA.changed && btnA.pressed) {
    doSDScan();
    // Redraw button grid since SD scan overwrites some screen area
    DisplayUI::drawButtonGrid();
  }

  // Mirror button changes to Serial
  for (int i = 0; i < Buttons::count(); i++) {
    const ButtonState &b = Buttons::get(i);
    if (b.changed) {
      Serial.printf("%s -> %s\n", b.name, b.pressed ? "PRESSED" : "released");
    }
  }

  delay(20);
}
