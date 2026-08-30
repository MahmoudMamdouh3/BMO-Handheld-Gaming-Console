# 32. Modular Core Template & Scaffolding Guide

**Purpose:** Any AI agent or developer adding a new console/emulator core (e.g. Chip-8, Sega Master System, Atari 2600, Pico-8) or major firmware subsystem can follow this exact copy-paste blueprint and step-by-step checklist to achieve zero friction, complete modularity, and zero memory leaks.

---

## 1. The 6-Step Integration Checklist

When adding a new emulator core (e.g. `MyCore`):

1. [ ] **Place Core Engine:** Put vendor engine source in `src/vendor/my_core/` (if pristine) or `src/engine/my_core/` (if customized). Add `BMO-PATCH` tags to any edits.
2. [ ] **Create C++ Wrapper:** Create `src/emulators/emu_mycore.h` and `src/emulators/emu_mycore.cpp` implementing `EmulatorCoreContract`.
3. [ ] **Register `RomType`:** In `src/core/sd_card.h`, add `ROM_MYCORE` to `enum RomType` and update extension matching in `sd_card.cpp`.
4. [ ] **Register Console Carousel:** In `BmoGameboy.ino`, add `ROM_MYCORE` to `CONSOLES[]` array and update `CONSOLE_COUNT`.
5. [ ] **Integrate Teardown in `SELECT + UP`:** In `BmoGameboy.ino` under `STATE_EMULATOR`, add `MyCoreEmu::destroy()` to the teardown branch.
6. [ ] **Verify Build & Run CI Validator:** Run `python scripts/validate_repo.py` and compile with `arduino-cli.exe`.

---

## 2. Standard Header Template (`src/emulators/emu_mycore.h`)

```cpp
#pragma once

#include <stdint.h>
#include <stddef.h>

namespace MyCoreEmu {
  // Initializes the emulator core with ROM data.
  // Returns true on success, false on invalid header / allocation failure.
  bool begin(const uint8_t* romData, size_t romSize);

  // Synchronizes physical button bitmask to emulator input registers.
  void updateJoypad();

  // Executes one frame of emulation and streams scanlines to ST7789 display.
  void runFrame();

  // Releases all allocated PSRAM/DRAM buffers (prevents memory leaks).
  void destroy();
}
```

---

## 3. Standard Implementation Template (`src/emulators/emu_mycore.cpp`)

```cpp
#include "emu_mycore.h"
#include "../core/config.h"
#include "../core/display_emu.h"
#include "../core/buttons.h"
#include <esp_heap_caps.h>

namespace {
  // Pointer to working state allocated in PSRAM/DRAM
  uint8_t* s_cartRam = nullptr;
  bool s_running = false;

  // Scanline output buffer (4-byte aligned for fast 32-bit transfers)
  uint16_t s_rowBuffer[320] __attribute__((aligned(4)));
}

namespace MyCoreEmu {

bool begin(const uint8_t* romData, size_t romSize) {
  if (!romData || romSize == 0) {
    LOG_ERROR_STR("MyCore: Invalid ROM pointer or size.");
    return false;
  }

  // 1. Allocate working memory in Octal PSRAM
  s_cartRam = (uint8_t*)heap_caps_malloc(64 * 1024, MALLOC_CAP_SPIRAM);
  if (!s_cartRam) {
    LOG_ERROR_STR("MyCore: Failed to allocate PSRAM.");
    return false;
  }
  memset(s_cartRam, 0, 64 * 1024);

  // 2. Initialize vendor core structures here...

  s_running = true;
  LOG_INFO("MyCore: Core started successfully (%u bytes ROM)", (unsigned)romSize);
  return true;
}

void updateJoypad() {
  if (!s_running) return;
  // Read Buttons::gb_joypad_state (0 = pressed, active-low)
  // or poll Buttons::get(Buttons::A).pressed directly
}

void runFrame() {
  if (!s_running) return;

  // DisplayEmu N3 Streaming Protocol:
  DisplayEmu::startFrame();

  // Step emulator frame and stream scanlines:
  for (int line = 0; line < 240; ++line) {
    // Generate line into s_rowBuffer...
    // Note: Colors must be pre-swapped BGR565 (Big-Endian on wire)
    DisplayEmu::streamPixelRow(s_rowBuffer, 320);
  }

  DisplayEmu::endFrame();
}

void destroy() {
  s_running = false;
  if (s_cartRam) {
    heap_caps_free(s_cartRam);
    s_cartRam = nullptr;
  }
  LOG_INFO_STR("MyCore: Teardown complete. PSRAM freed.");
}

} // namespace MyCoreEmu
```

---

## 4. Invariants to Check Before Committing
- [ ] No `malloc()` or `heap_caps_malloc()` called during `runFrame()`.
- [ ] Wire pixel format is **BGR565 byte-swapped**.
- [ ] Little-endian byte reconstruction used for any ROM data access.
- [ ] All diagnostic prints use `LOG_INFO` / `LOG_ERROR`, never bare `Serial.print`.
- [ ] Teardown `destroy()` safely handles being called multiple times (`nullptr` checks).
