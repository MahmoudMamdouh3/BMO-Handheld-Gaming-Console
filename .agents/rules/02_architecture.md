# 4. Toolchain & Build Configuration
- **Board Package:** esp32:esp32 version `3.3.11`.
- **Key Libraries:** `Adafruit ST7735 and ST7789 Library` v1.11.0, `SD` v1.3.0.
- **Board Model:** ESP32-S3-N16R8 (16MB Flash, 8MB PSRAM).
- **Flash Mode:** OPI PSRAM, 80MHz flash speed. (Crucial for performance and stability).
- **Partition Scheme:** Custom `partitions.csv` prioritizing `app0` space for baked ROMs.

---

# 5. Architecture & Performance Patterns
- **Memory & Cache:** Place hot structures (like emulator `gb_s` contexts) in internal DRAM and align them to the ESP32-S3 D-cache line size (`__attribute__((aligned(32)))`) to prevent cache straddling penalties. Large idle buffers (like save RAM) should go to PSRAM.
- **IRAM Placement:** Critical inner-loop rendering functions (e.g., `lcd_draw_line`, `gb_rom_read`) MUST use `IRAM_ATTR` to prevent instruction cache misses.
- **Input Polling:** `Buttons::update()` manages a single global bitmask (`gb_joypad_state`) and enum indexes. It MUST be called exactly **once per `loop()` iteration** (or once per emulator frame, e.g., in `DoomEmu::runFrame()`). Do not poll hardware multiple times per frame.
- **Vendor Core Inclusion:** The `peanut_gb.h` and `walnut_cgb.h` header-only libraries follow the single-translation-unit pattern. Their implementations must be compiled into exactly one `.cpp` file (`emu_peanut.cpp` and `emu_walnut.cpp` respectively) using a namespace wrap to avoid ODR violations.
