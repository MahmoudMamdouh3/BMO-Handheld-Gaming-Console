#include "display_ui.h"
#include "config.h"
#include "buttons.h"
#include "sd_card.h"
#include <SPI.h>

namespace {
  // Use the default SPI bus (shared with SD card).
  // SPI.begin() must be called in the main sketch BEFORE DisplayUI::begin().
  Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

  // Layout constants
  const int TITLE_Y      = 10;
  const int SD_SECTION_Y = 40;
  const int SD_LINE_H    = 16;
  const int FILES_Y      = 110;
  const int FILE_LINE_H  = 12;
  const int BTN_SECTION_Y = 180;
  const int BTN_ROW_H    = 16;
  const int STATUS_X     = 90;
  const int MSG_Y        = 300;
}

void DisplayUI::begin() {
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
}

void DisplayUI::drawTitle(const char *text) {
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(10, TITLE_Y);
  tft.println(text);
}

void DisplayUI::drawSDStatus(bool detected, const char *cardType,
                              float sizeGB, int fileCount) {
  // Clear the SD section area
  tft.fillRect(0, SD_SECTION_Y, TFT_WIDTH, 65, ST77XX_BLACK);
  tft.setTextSize(1);

  int y = SD_SECTION_Y;

  // Line 1: detection status
  tft.setCursor(10, y);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("SD Card: ");
  if (detected) {
    tft.setTextColor(ST77XX_GREEN);
    tft.print("DETECTED");
  } else {
    tft.setTextColor(ST77XX_RED);
    tft.print("NOT FOUND");
  }
  y += SD_LINE_H;

  if (detected) {
    // Line 2: card type
    tft.setCursor(10, y);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Type: ");
    tft.setTextColor(ST77XX_YELLOW);
    tft.print(cardType);
    y += SD_LINE_H;

    // Line 3: card size
    tft.setCursor(10, y);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Size: ");
    tft.setTextColor(ST77XX_YELLOW);
    tft.print(sizeGB, 1);
    tft.print(" GB");
    y += SD_LINE_H;

    // Line 4: file count
    tft.setCursor(10, y);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Files: ");
    tft.setTextColor(ST77XX_YELLOW);
    tft.print(fileCount);
  }
}

void DisplayUI::drawFileList(int count) {
  // Clear file list area
  tft.fillRect(0, FILES_Y, TFT_WIDTH, 60, ST77XX_BLACK);

  if (count == 0) return;

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, FILES_Y);
  tft.print("Root files:");

  int maxShow = (count < 4) ? count : 4;  // Show up to 4 files
  for (int i = 0; i < maxShow; i++) {
    int y = FILES_Y + (i + 1) * FILE_LINE_H;
    tft.setCursor(16, y);
    tft.setTextColor(ST77XX_MAGENTA);
    tft.print(SDCard::fileName(i));
  }
  if (count > 4) {
    int y = FILES_Y + 5 * FILE_LINE_H;
    tft.setCursor(16, y);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("... and ");
    tft.print(count - 4);
    tft.print(" more");
  }
}

void DisplayUI::drawButtonGrid() {
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  for (int i = 0; i < Buttons::count(); i++) {
    int y = BTN_SECTION_Y + i * BTN_ROW_H;
    tft.setCursor(10, y);
    tft.print(Buttons::get(i).name);

    // Initial "released" state
    tft.setCursor(STATUS_X, y);
    tft.setTextColor(ST77XX_RED);
    tft.print("released");
    tft.setTextColor(ST77XX_WHITE);
  }
}

void DisplayUI::updateButtonGrid() {
  for (int i = 0; i < Buttons::count(); i++) {
    const ButtonState &b = Buttons::get(i);
    if (!b.changed) continue;

    int y = BTN_SECTION_Y + i * BTN_ROW_H;
    uint16_t color = b.pressed ? ST77XX_GREEN : ST77XX_RED;

    tft.fillRect(STATUS_X, y - 2, 120, 14, ST77XX_BLACK);
    tft.setCursor(STATUS_X, y);
    tft.setTextColor(color);
    tft.print(b.pressed ? "PRESSED" : "released");
  }
}

void DisplayUI::drawStatusMessage(const char *msg) {
  tft.fillRect(0, MSG_Y, TFT_WIDTH, 16, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(10, MSG_Y);
  tft.print(msg);
}
