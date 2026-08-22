#include "display_ui.h"
#include "config.h"
#include "buttons.h"
#include <SPI.h>

namespace {
  // Use hardware SPI on the FSPI bus for ~10x speed over bit-banged SW SPI.
  // SCK=12, MOSI=11, CS=10 are all valid FSPI-capable pins on ESP32-S3.
  SPIClass hspi(FSPI);
  Adafruit_ST7789 tft = Adafruit_ST7789(&hspi, TFT_CS, TFT_DC, TFT_RST);

  const int START_Y     = 60;
  const int ROW_HEIGHT  = 24;
  const int STATUS_X    = 90;
}

void DisplayUI::begin() {
  hspi.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);  // MISO unused (display is write-only)
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
}

void DisplayUI::drawTitle(const char *text) {
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 10);
  tft.println(text);
}

void DisplayUI::drawButtonGrid() {
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  for (int i = 0; i < Buttons::count(); i++) {
    int y = START_Y + i * ROW_HEIGHT;
    tft.setCursor(10, y);
    tft.print(Buttons::get(i).name);

    // initial "released" state so the screen isn't blank before any
    // button has been touched yet
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

    int y = START_Y + i * ROW_HEIGHT;
    uint16_t color = b.pressed ? ST77XX_GREEN : ST77XX_RED;

    tft.fillRect(STATUS_X, y - 2, 120, 14, ST77XX_BLACK);
    tft.setCursor(STATUS_X, y);
    tft.setTextColor(color);
    tft.print(b.pressed ? "PRESSED" : "released");
  }
}
