# 6. Known Issues / Technical Debt Log

## Active Hardware-Verified Tags
*(none yet)*

These are verified latent bugs existing in the current codebase:
1. **Walnut-CGB Macro Typos (VERIFIED_HOST)**: The `_OPS_OPS` and `_DISABLED` macro typos in `walnut_cgb.h` are fixed. The 16-bit fast paths are active and compile. The host-side test harness ran `cpu_instrs.gb` to full completion and printed `Passed all tests`. Hardware behavior remains unverified.
2. **Missing Semicolons in Dead Branches (DEBUNKED)**: The reported syntax errors in dead branches (e.g. CALL C) did not exist. Un-dead-coding the branches resulted in a clean compile, demonstrating the report was an artifact from a flawed grep check.
3. **Unaligned Pointer Casts (VERIFIED_HOST)**: `gb_rom_read16` and `gb_rom_read32` in `emu_walnut.cpp` used raw pointer casts that were unsafe on Xtensa architectures. This is fixed by implementing byte-wise, explicitly little-endian reconstruction. The host-side CPU test harness completed `cpu_instrs.gb` and printed `Passed all tests`. The remaining 54 raw pointer casts in the codebase operate on internal SRAM/PSRAM (where unaligned access is supported) and are deferred.
4. **Serial.print in Hot Loops (FIXED)**: Widespread usage of `Serial.print` (appears in 9 files, 41 total call sites) violated `15_performance_budgets.md`. Replaced with gated `LOG_LEVEL` macros defined in `config.h`.
5. **Vendor Versions Missing (OPEN)**: Vendor libraries `peanut_gb`, `walnut_cgb`, and `doomgeneric` have no recorded upstream version in their headers/source files. Cannot pin retroactively without external research.
6. **Walnut-CGB 16-bit Fast Paths Incompatible with GBC ROMs (FIXED_UNVERIFIED)**: `WALNUT_GB_16_BIT_OPS_DUALFETCH=1` and `WALNUT_GB_16_BIT_OPS=1` were enabled by a previous agent session. The upstream author's own comment warns "currently breaks compatibility with some games." Super Mario Bros. Deluxe (GBC) froze at the title screen when attempting to start/load a game. Both flags reverted to `0` in `walnut_cgb.h`. Status: `FIXED_UNVERIFIED` — compiled, no hardware test run this session.

---

# 12. Changelog
- **2026-08-29**: Discovered and documented the `Buttons::update()` double-polling bug in Doom.
- **2026-08-29**: Split singular `project-rules.md` into multiple `.agents/rules/` files to evade the 12,000 character context truncation limit.
- **2026-08-29**: Completely rewrote rules file to enforce strict structure. Reconciled dormant module discrepancies, consolidated pin map, and formally acquired tooling to verify ESP32 core version (3.3.11) and libraries. (Agent Antigravity)
- **2026-08-29**: Purged Zig compiler binary from git history using `git filter-repo` and ignored `tools/zig/`.
- **2026-08-29**: Relocated `walnut_cgb.h` to the canonical `src/vendor/walnut_cgb/` directory.
- **2026-08-29**: Fixed `host_test.cpp` string-matching logic to require full `Passed all tests` instead of short-circuiting at `Passed`.
- **2026-08-29**: Appended peripheral cross-reference to `12_extensibility_contract.md`. (Agent Antigravity)
- **2026-08-29**: Added `13_code_style.md` to standardize styling and language use. (Agent Antigravity)
- **2026-08-29**: Added `14_error_handling_and_fault_isolation.md` to define fault paths and `gb_error` panic handling. (Agent Antigravity)
- **2026-08-29**: Added `15_performance_budgets.md` to standardize hot path limits and memory tracking. (Agent Antigravity)
- **2026-08-29**: Added `16_logging_and_diagnostics.md` to govern log levels and visibility. (Agent Antigravity)
- **2026-08-29**: Added `17_release_and_versioning.md` to govern version constants and tagging. (Agent Antigravity)
- **2026-08-29**: Added `18_dependency_and_vendor_sync.md` to govern `BMO-PATCH` tags and upstream syncing. (Agent Antigravity)
- **2026-08-29**: Added `19_security_and_data_integrity.md` to cover untrusted input handling and FRAM validation. (Agent Antigravity)
- **2026-08-29**: Added `20_multi_agent_protocol.md` to make multi-session/multi-model handoffs explicit. (Agent Antigravity)
- **2026-08-29**: Added `21_documentation_standards.md` to define `docs/` vs `.agents/rules/` hierarchy. (Agent Antigravity)
- **2026-08-29**: Added `22_review_checklist.md` as an everyday self-review counterpart to git workflow gates. (Agent Antigravity)
- **2026-08-29**: Added `23_incident_postmortem_log.md` with retroactive INC-1 and INC-2 logs.
- **2026-08-30**: Reverted `WALNUT_GB_16_BIT_OPS_DUALFETCH` and `WALNUT_GB_16_BIT_OPS` to `0` in `walnut_cgb.h`. Previous agent enabled them; upstream docs warn they break some games. Root cause of Super Mario Bros. Deluxe GBC freezing on new game/load. Status: FIXED_UNVERIFIED. (Agent Antigravity)
- **2026-08-30**: Ruleset v2 — full repo audit. Added 27_codebase_map.md (architecture map), 28_display_and_spi_contract.md (pixel format + SPI rules), 29_adding_a_baked_rom.md (ROM baking checklist), 30_common_agent_mistakes.md (anti-pattern catalogue M1-M15). Updated 07_task_protocol.md to lead with codebase map and mistakes catalogue. (Agent Antigravity)
