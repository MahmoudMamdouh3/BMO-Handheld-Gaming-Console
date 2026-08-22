# Wiring - Milestone 1: Display + Buttons

Board: **ESP32-S3-N16R8** (16MB flash, 8MB octal PSRAM)

## Display (ST7789, 2.4", SPI, 240x320)

| Display pin | ESP32-S3 GPIO |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SCL / SCK | GPIO12 |
| SDA / MOSI | GPIO11 |
| RES | GPIO9 |
| DC | GPIO8 |
| CS | GPIO10 |
| BLK | 3V3 |

## Buttons (8x, Game Boy layout)

One leg to the GPIO, other leg to GND. Internal pull-ups used in
firmware (`INPUT_PULLUP`) - no external resistors needed. A press reads
as `LOW`.

| Button | ESP32-S3 GPIO |
|---|---|
| Up | GPIO4 |
| Down | GPIO5 |
| Left | GPIO6 |
| Right | GPIO7 |
| A | GPIO16 |
| B | GPIO17 |
| Start | GPIO18 |
| Select | GPIO21 |

### 4-leg tactile switches (OMRON 12x12mm)

These switches have 4 legs but only 2 electrical nodes: legs on the same
side of the switch body are permanently connected to each other. Wire
**one leg from each side** - one to the GPIO, one to GND. The other two
legs are redundant and can be left unconnected.

## Reserved / avoid on this board

- **GPIO 33-37**: internally used for octal PSRAM, not usable as GPIO.
- **GPIO 0, 3, 45, 46**: strapping pins (affect boot mode at power-on).
  Avoided for buttons to prevent accidentally forcing the wrong boot
  mode if held down at power-up.

## Not yet wired in this milestone

- microSD module (planned for milestone 2) - MISO=GPIO15, CS=GPIO13,
  shares SCK/MOSI with the display, VCC needs 5V (see hardware notes).
