# 6. Known Issues / Technical Debt Log
These are verified latent bugs existing in the current codebase:
1. **Walnut-CGB Macro Typos (FIXED BUT UNVERIFIED)**: The massive `_OPS_OPS` and `_DISABLED` macro typos in `walnut_cgb.h` have been programmatically fixed, and the 16-bit fast paths are now active and compile cleanly without syntax errors. **HOWEVER**, functional correctness of the reactivated 3,000-line 16-bit path has NOT been verified at runtime on real hardware yet. There is no automated CPU-behavior test covering this. Flash and test this manually before trusting it.
2. **Missing Semicolons in Dead Branches**: **DEBUNKED**. We previously believed there were missing semicolons in the dead branches (e.g. CALL C). The recent un-dead-coding and successful compile proves these syntax errors did not actually exist; they were a hallucinated artifact from a flawed grep check in an earlier session.
3. **Unaligned Pointer Casts**: In `emu_walnut.cpp`, `gb_rom_read16` and `gb_rom_read32` use raw `uint16_t*` and `uint32_t*` pointer casts on a byte buffer (`current_rom_data`) at arbitrary offsets. This risks an unaligned-access fault or silent corruption on the Xtensa core.

---

# 12. Changelog
- **2026-08-29**: Discovered and documented the `Buttons::update()` double-polling bug in Doom.
- **2026-08-29**: Split singular `project-rules.md` into multiple `.agents/rules/` files to evade the 12,000 character context truncation limit.
- **2026-08-29**: Completely rewrote rules file to enforce strict structure. Reconciled dormant module discrepancies, consolidated pin map, and formally acquired tooling to verify ESP32 core version (3.3.11) and libraries. (Agent Antigravity)
