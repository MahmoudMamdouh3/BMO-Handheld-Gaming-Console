# Wiring - Milestone 2: SD Card (added to Display + Buttons)

Board: **ESP32-S3-N16R8** (16MB flash, 8MB octal PSRAM)

## What's new in this milestone

The SD card module is added to the existing display + button wiring
from milestone 1. The SD card **shares the SPI bus** with the display
(same SCK and MOSI lines), plus gets its own MISO and CS.

## SD Card Module (6-pin breakout)

| SD Module Pin | ESP32-S3 | Notes |
|---|---|---|
| VCC | **5V** (not 3V3!) | Module has onboard regulator needing 5V |
| GND | GND | Shared ground rail |
| SCK | GPIO12 | **Shared** with display SCK |
| MOSI | GPIO11 | **Shared** with display MOSI |
| MISO | GPIO15 | SD-only (display doesn't send data back) |
| CS | GPIO13 | SD-only chip select |

## Shared SPI bus - critical note

Both the display and SD card use the same physical SPI lines (SCK=12,
MOSI=11). The firmware **must** call `SPI.begin(12, 15, 11)` with
explicit pin mapping before initializing either device. Without this,
`SD.begin()` will try to claim default SPI pins and the display will
go black.

## Display (unchanged from milestone 1)

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

## Buttons (unchanged from milestone 1)

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
