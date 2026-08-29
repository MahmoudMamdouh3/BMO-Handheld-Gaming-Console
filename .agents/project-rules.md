# Project Ground-Truth & Strict Agent Rules

update the entirety of docuemntation with what we are doing as soon as you finish any tasks 

This file contains the strict rules and ground truth for the GB-Emu-ESP32S3 project. **All LLM agents must read and adhere to these rules strictly to avoid hallucination or breaking the build.**

## 1. Hardware Ground Truth
* **Power:** USB-C power ONLY. NO battery, NO TP4056 module, NO voltage divider on GPIO1.
* **Storage:** Blue MicroSD card module physically wired using the 5V VBUS workaround to bypass its onboard logic-level issues. SD Card works.
* **Audio:** NO MAX98357A DAC. NO I2S wiring at all.
* **Display & Input:** ST7789 display and 8 buttons ONLY. Wired on the shared FSPI bus (`SCK=12, MOSI=11, CS=10, DC=8, RES=9, BLK=3V3`).
* **Feature Gating:** Code for Audio and Battery *exists* but is disabled via `FEATURE_AUDIO 0` and `FEATURE_BATTERY_MONITOR 0` in `config.h`. `FEATURE_SD_CARD` is enabled (`1`).

## 2. Pin Constraints (ESP32-S3-N16R8)
* **GPIO 33-37:** RESERVED for internal Octal PSRAM. **DO NOT USE.**
* **GPIO 0, 3, 45, 46:** STRAPPING PINS. Do not use for inputs/buttons.
* **GPIO 19, 20:** USB D-/D+. **DO NOT USE.**
* **GPIO 1 (Battery ADC):** Currently FLOATING. Do not read this pin and trigger deep sleep, or the board will enter a fatal boot-loop.

## 3. Firmware Compilation Rules
* **Flash Mode (CRITICAL):** Because this is an N16R8 board with Octal Flash AND Octal PSRAM, **Flash Mode MUST be set to `OPI 80MHz`** in the Arduino IDE. If left on the default `QIO`, the ESP32-S3 will instantly panic (`esp_core_dump_flash: No core dump partition found!`) on boot due to a flash cache mismatch.
* **PSRAM:** MUST be set to `OPI PSRAM`.
* **Partition Scheme:** `Custom` or `Huge APP (3MB No OTA/1MB SPIFFS)`.

## 3. Architecture & Performance Rules
* **Shared SPI Initialization:** The display and (future) SD card share the FSPI bus. You MUST explicitly call `SPI.begin(SCK, MISO, MOSI, -1)` before initializing the TFT or SD, otherwise initialization will collide and hang.
* **Display Rendering:** DO NOT use `Adafruit_GFX` pixel-by-pixel pushes (`drawPixel`). Use `SPI.writeBytes()` or DMA to stream scanlines. Apply the hardware bug fix for ST7789 MADCTL (`0xA0 | 0x08`) at boot.
* **Memory Management:** Emulator ROMs and framebuffers exceed DRAM. You MUST allocate large buffers in PSRAM using `heap_caps_malloc(size, MALLOC_CAP_SPIRAM)`.
* **Cache Alignment:** Emulator state structs (e.g., `cpu_reg`) MUST be aligned to the 32-byte D-cache boundaries using `__attribute__((aligned(32)))` to prevent cache thrashing.
* **Execution (IRAM):** Performance-critical hot loops (rendering, cartridge read/write callbacks) MUST be forced into internal RAM using `IRAM_ATTR`.

## 4. Agent Behavior
* **No Assumptions:** Do not assume a peripheral exists just because a driver file (like `battery.cpp` or `sd_card.cpp`) exists. Trust *only* Section 1 above.
* **Graceful Degradation:** Never write code that hangs, blocks, or deep-sleeps if a hardware peripheral (like the SD card or I2S DAC) fails to initialize. Fall back silently or log an error.
* **Physical Reality:** Everything is soldered to a perfboard. Mistakes are permanent and costly. Isolate new peripheral tests in standalone `.ino` sketches before integrating them into the main emulator firmware.
