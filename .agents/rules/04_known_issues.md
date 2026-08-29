# 6. Known Issues / Technical Debt Log

## Active Hardware-Verified Tags
*(none yet)*

These are verified latent bugs existing in the current codebase:
1. **Walnut-CGB Macro Typos (VERIFIED_HOST)**: The `_OPS_OPS` and `_DISABLED` macro typos in `walnut_cgb.h` are fixed. The 16-bit fast paths are active and compile. The host-side test harness ran `cpu_instrs.gb` to full completion and printed `Passed all tests`. Hardware behavior remains unverified.
2. **Missing Semicolons in Dead Branches (DEBUNKED)**: The reported syntax errors in dead branches (e.g. CALL C) did not exist. Un-dead-coding the branches resulted in a clean compile, demonstrating the report was an artifact from a flawed grep check.
3. **Unaligned Pointer Casts (VERIFIED_HOST)**: `gb_rom_read16` and `gb_rom_read32` in `emu_walnut.cpp` used raw pointer casts that were unsafe on Xtensa architectures. This is fixed by implementing byte-wise, explicitly little-endian reconstruction. The host-side CPU test harness completed `cpu_instrs.gb` and printed `Passed all tests`. The remaining 54 raw pointer casts in the codebase operate on internal SRAM/PSRAM (where unaligned access is supported) and are deferred.

---

# 12. Changelog
- **2026-08-29**: Discovered and documented the `Buttons::update()` double-polling bug in Doom.
- **2026-08-29**: Split singular `project-rules.md` into multiple `.agents/rules/` files to evade the 12,000 character context truncation limit.
- **2026-08-29**: Completely rewrote rules file to enforce strict structure. Reconciled dormant module discrepancies, consolidated pin map, and formally acquired tooling to verify ESP32 core version (3.3.11) and libraries. (Agent Antigravity)
