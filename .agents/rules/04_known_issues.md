# 6. Known Issues / Technical Debt Log
These are verified latent bugs existing in the current codebase:
1. **Walnut Macro Name Typo**: In `walnut_cgb.h`, the dual-fetch 16-bit operation feature is governed by a typo. The codebase defines `#define WALNUT_GB_16_BIT_OPS_DUALFETCH`, but several `#if` guards check for `#if WALNUT_GB_16_BIT_OPS_OPS_DUALFETCH` (or `_DISABLED`), silently dead-coding those optimization paths.
2. **Missing Semicolons in Dead Branches**: Inside those same dead `#if WALNUT_GB_16_BIT_OPS_OPS_...` branches in `walnut_cgb.h`, there are latent syntax errors (missing semicolons). Fixing the macro typo will immediately result in compile errors until the semicolons are also fixed.
3. **Unaligned Pointer Casts**: In `emu_walnut.cpp`, `gb_rom_read16` and `gb_rom_read32` use raw `uint16_t*` and `uint32_t*` pointer casts on a byte buffer (`current_rom_data`) at arbitrary offsets. This risks an unaligned-access fault or silent corruption on the Xtensa core.
4. **Doom Button Double-Polling**: `Buttons::update()` is called unconditionally at the top of the emulator frame in `BmoGameboy.ino:loop()`. However, `DoomEmu::runFrame()` also calls `Buttons::update()` internally. This violates the single-polling rule and causes buttons to be polled twice per frame when the Doom emulator is active.

---

# 12. Changelog
- **2026-08-29**: Discovered and documented the `Buttons::update()` double-polling bug in Doom.
- **2026-08-29**: Split singular `project-rules.md` into multiple `.agents/rules/` files to evade the 12,000 character context truncation limit.
- **2026-08-29**: Completely rewrote rules file to enforce strict structure. Reconciled dormant module discrepancies, consolidated pin map, and formally acquired tooling to verify ESP32 core version (3.3.11) and libraries. (Agent Antigravity)
