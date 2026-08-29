# 6. Known Issues / Technical Debt Log
These are verified latent bugs existing in the current codebase:
1. **Walnut-CGB Macro Typos (VERIFIED_HOST)**: The massive `_OPS_OPS` and `_DISABLED` macro typos in `walnut_cgb.h` have been programmatically fixed. The 16-bit fast paths are active, compile cleanly, and have successfully passed CPU execution validation in the new `host_test` harness using Blargg's `cpu_instrs.gb` (final output: "Passed all tests"). On-hardware confirmation (`VERIFIED_HARDWARE`) is still outstanding and pending manual flash-and-report.
2. **Missing Semicolons in Dead Branches**: **DEBUNKED**. We previously believed there were missing semicolons in the dead branches (e.g. CALL C). The recent un-dead-coding and successful compile proves these syntax errors did not actually exist; they were a hallucinated artifact from a flawed grep check in an earlier session.
3. **Unaligned Pointer Casts (VERIFIED_HOST)**: `gb_rom_read16` and `gb_rom_read32` in `emu_walnut.cpp` previously used raw pointer casts that were unsafe on Xtensa architectures. This has been resolved by implementing byte-wise, explicitly little-endian reconstruction. The remaining 54 raw pointer casts in the codebase operate on internal SRAM/PSRAM (where unaligned access is supported) and are deferred.

---

# 12. Changelog
- **2026-08-29**: Discovered and documented the `Buttons::update()` double-polling bug in Doom.
- **2026-08-29**: Split singular `project-rules.md` into multiple `.agents/rules/` files to evade the 12,000 character context truncation limit.
- **2026-08-29**: Completely rewrote rules file to enforce strict structure. Reconciled dormant module discrepancies, consolidated pin map, and formally acquired tooling to verify ESP32 core version (3.3.11) and libraries. (Agent Antigravity)
