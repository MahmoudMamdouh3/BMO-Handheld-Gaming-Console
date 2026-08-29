# 1. Physically Wired Hardware (Verified Ground Truth)
Only the following hardware is *currently, physically soldered and wired* to the ESP32-S3:
- **Power:** USB-C power only. (No battery, no TP4056 module, no voltage divider yet).
- **Display:** ST7789 display (2.4", 240x320) wired to the shared FSPI bus.
- **Input:** 8 standard Game Boy-layout tactile buttons wired with internal pull-ups (`INPUT_PULLUP`).
- **Storage:** SD card module wired to the shared FSPI bus.
- **Audio:** None. (No MAX98357A DAC, no I2S wiring).

---

# 2. Pin Map
This is the single source of truth for all pin assignments, synchronized with `config.h`. No other file should hardcode a pin number.

| Pin | Function / Macro | Module / Owner | Constraints / Notes |
| :--- | :--- | :--- | :--- |
| **0, 3, 45, 46** | *Strapping Pins* | System | **DO NOT USE** for buttons. Affects boot mode. |
| **33-37** | *Internal PSRAM* | System | **DO NOT USE**. Reserved internally for octal PSRAM. |
| **4** | `BTN_UP` | `buttons.cpp` | `INPUT_PULLUP` (reads LOW when pressed). |
| **5** | `BTN_DOWN` | `buttons.cpp` | `INPUT_PULLUP` |
| **6** | `BTN_LEFT` | `buttons.cpp` | `INPUT_PULLUP` |
| **7** | `BTN_RIGHT` | `buttons.cpp` | `INPUT_PULLUP` |
| **16** | `BTN_A` | `buttons.cpp` | `INPUT_PULLUP` |
| **17** | `BTN_B` | `buttons.cpp` | `INPUT_PULLUP` |
| **18** | `BTN_START` | `buttons.cpp` | `INPUT_PULLUP` |
| **21** | `BTN_SELECT` | `buttons.cpp` | `INPUT_PULLUP` |
| **8** | `TFT_DC` | `display_emu.cpp` | Data/Command pin for ST7789. |
| **9** | `TFT_RST` | `display_emu.cpp` | Reset pin for ST7789. |
| **10** | `TFT_CS` | `display_emu.cpp` | Chip Select for ST7789 (FSPI). |
| **11** | `TFT_MOSI` | `display_emu.cpp` / SD | Shared FSPI MOSI. |
| **12** | `TFT_SCK` | `display_emu.cpp` / SD | Shared FSPI SCK. |
| **13** | `SD_CS` | `sd_card.cpp` | Chip Select for SD Card. |
| **15** | `SD_MISO` | `sd_card.cpp` | Shared FSPI MISO. |
| **1** | `BATTERY_ADC_PIN` | *Dormant (battery.cpp)* | Reserved for future voltage divider. |
| **38** | `I2S_BCLK` | *Dormant (audio_i2s.cpp)* | Reserved for future I2S DAC. |
| **39** | `I2S_LRC` | *Dormant (audio_i2s.cpp)*| Reserved for future I2S DAC. |
| **40** | `I2S_DIN` | *Dormant (audio_i2s.cpp)*| Reserved for future I2S DAC. |
| **43** | `I2C_SDA` | *Planned (fram_save.cpp)*| Reserved for future I2C FRAM. |
| **44** | `I2C_SCL` | *Planned (fram_save.cpp)*| Reserved for future I2C FRAM. |

---

# 3. Software Feature Flags & Dormant Modules
All peripheral modules must be gated by `FEATURE_*` flags in `config.h`. A disabled feature must compile to a safe no-op.

- **`FEATURE_SD_CARD` (Current: `1`)**: Gates `sd_card.cpp`. The SD card is physically present and enabled.
- **`FEATURE_BATTERY_MONITOR` (Current: `0`)**: Gates `battery.cpp`. **Note:** A complete, realistic driver implementation exists in `battery.cpp`. It is dormant. It is safe to read/modify, but MUST NEVER be enabled until the physical voltage divider is confirmed soldered in Section 1.
- **`FEATURE_AUDIO` (Current: `0`)**: Gates `audio_i2s.cpp` and emulator APU callbacks. **Note:** A complete driver implementation exists and is dormant. Do NOT enable until physical I2S hardware is verified in Section 1.
