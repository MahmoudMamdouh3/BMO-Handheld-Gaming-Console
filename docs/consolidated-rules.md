# BMO Handheld Gaming Console — Consolidated Ruleset (v5)
**Target Platform:** ESP32-S3-N16R8 (Dual-Core LX7 @ 240MHz, 16MB OPI Flash, 8MB Octal PSRAM)  
**Display:** ST7789VW 240×320 SPI TFT (Landscape 320×240)  
**Document Status:** Complete Consolidated Ground Truth (Rules 00 through 37)  
**Purpose:** Single-file compilation for external architectural and code review.

---

## Table of Contents
- [0. Hard Stops](#0-hard-stops)
- [1. Physically Wired Hardware (Verified Ground Truth)](#1-physically-wired-hardware-verified-ground-truth)
- [4. Toolchain & Build Configuration](#4-toolchain-build-configuration)
- [7. Repository & File Structure](#7-repository-file-structure)
- [6. Known Issues / Technical Debt Log](#6-known-issues-technical-debt-log)
- [Git & Commit Discipline](#git-commit-discipline)
- [Verification & Evidence Standards](#verification-evidence-standards)
- [Standard Task Protocol](#standard-task-protocol)
- [UI & Visual Style Guide](#ui-visual-style-guide)
- [Testing Infrastructure](#testing-infrastructure)
- [Symbol Reference (Ground Truth for Names)](#symbol-reference-ground-truth-for-names)
- [Meta-Rules for the Rules Directory Itself](#meta-rules-for-the-rules-directory-itself)
- [Extensibility Contract (New Emulator Cores)](#extensibility-contract-new-emulator-cores)
- [Code Style & Formatting](#code-style-formatting)
- [Error Handling & Fault Isolation](#error-handling-fault-isolation)
- [Performance Engineering & Budgets](#performance-engineering-budgets)
- [Logging & Diagnostics](#logging-diagnostics)
- [Release & Versioning](#release-versioning)
- [Dependency & Vendor-Library Sync](#dependency-vendor-library-sync)
- [Security & Data Integrity](#security-data-integrity)
- [Multi-Agent / Cross-Session Protocol](#multi-agent-cross-session-protocol)
- [Documentation Standards](#documentation-standards)
- [Everyday Self-Review Checklist](#everyday-self-review-checklist)
- [Incident / Process-Failure Log](#incident-process-failure-log)
- [Vendor Flag Safety](#vendor-flag-safety)
- [Game Compatibility Ledger](#game-compatibility-ledger)
- [Emulator Exit Contract](#emulator-exit-contract)
- [Codebase Map — Ground Truth for Architecture](#codebase-map-ground-truth-for-architecture)
- [Display & SPI Bus Contract](#display-spi-bus-contract)
- [How to Add a Baked ROM](#how-to-add-a-baked-rom)
- [Common Agent Mistakes (Anti-Pattern Catalogue)](#common-agent-mistakes-anti-pattern-catalogue)
- [31. Quick-Start Primer (Zero-Context Agent On-Ramp)](#31-quick-start-primer-zero-context-agent-on-ramp)
- [32. Modular Core Template & Scaffolding Guide](#32-modular-core-template-scaffolding-guide)
- [33. Agent Handoff & Continuous Optimization Cycle](#33-agent-handoff-continuous-optimization-cycle)
- [34. AI Agent Guardrails & Execution Invariants](#34-ai-agent-guardrails-execution-invariants)
- [BmoFace Mascot Subsystem Contract](#bmoface-mascot-subsystem-contract)
- [Human-Reported Hardware Bug Intake Protocol](#human-reported-hardware-bug-intake-protocol)
- [ROM Governance & Flash-Budget Invariant](#rom-governance-flash-budget-invariant)

---

<!-- Section: 00_hard_stops.md -->

# 0. Hard Stops
**CRITICAL:** These are irreversible, boot-bricking, or severely damaging mistakes. Do NOT violate these constraints under any circumstances.
- **GPIO1 Floating ADC Boot-Loop:** Do NOT use GPIO1 (Battery ADC) in any code without confirming the voltage divider is physically soldered. Reading a floating GPIO1 on the ESP32-S3 can trigger unstable behavior or boot loops.
- **OPI Flash Mode:** The board is an ESP32-S3-N16R8. It REQUIRES OPI flash mode (80MHz). Using QPI or other modes will result in a non-booting or heavily degraded system.
- **No Fatal Deadlocks on Missing Hardware:** Any code path that runs when a hardware feature flag is disabled (`0`) MUST NOT hang, sleep, or crash waiting for hardware that isn't there. For example, `Battery::update()` must not trigger deep sleep loops, and `SD.begin()` must not be called if `FEATURE_SD_CARD == 0`.
- **Perfboard Permanence:** Everything is soldered onto a permanent perfboard. Software must adapt to the wiring, not the other way around. Always explicitly test code in isolated `.ino` sketches before integrating hardware changes.

---

<!-- Section: 01_hardware.md -->

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

---

<!-- Section: 02_architecture.md -->

# 4. Toolchain & Build Configuration
- **Board Package:** esp32:esp32 version `3.3.11`.
- **Key Libraries:** `Adafruit ST7735 and ST7789 Library` v1.11.0, `SD` v1.3.0, `Agnes` v0.2.0.
- **Board Model:** ESP32-S3-N16R8 (16MB Flash, 8MB PSRAM).
- **Host Test Compiler:** Zig v0.13.0 for Windows x86_64 (`https://ziglang.org/download/0.13.0/zig-windows-x86_64-0.13.0.zip`). Downloaded and extracted via `curl` and `Expand-Archive`.
- **Flash Mode:** OPI Flash & OPI PSRAM, 80MHz flash speed. (Crucial for performance and stability).
- **Partition Scheme:** Custom `partitions.csv` prioritizing `app0` space for baked ROMs.
- **Verified Build Command:**
  ```powershell
  .\arduino-cli.exe compile --fqbn "esp32:esp32:esp32s3:FlashMode=opi,FlashSize=16M,PartitionScheme=custom,PSRAM=opi" firmware/BmoGameboy
  ```

---

# 5. Architecture & Performance Patterns
- **Memory & Cache:** Place hot structures (like emulator `gb_s` contexts) in internal DRAM and align them to the ESP32-S3 D-cache line size (`__attribute__((aligned(32)))`) to prevent cache straddling penalties. Large idle buffers (like save RAM) should go to PSRAM.
- **IRAM Placement:** Critical inner-loop rendering functions (e.g., `lcd_draw_line`, `gb_rom_read`) MUST use `IRAM_ATTR` to prevent instruction cache misses.
- **Input Polling:** `Buttons::update()` manages a single global bitmask (`gb_joypad_state`) and enum indexes. It MUST be called exactly **once per `loop()` iteration** (or once per emulator frame, e.g., in `DoomEmu::runFrame()`). Do not poll hardware multiple times per frame.
- **Vendor Core Inclusion:** The `peanut_gb.h` and `walnut_cgb.h` header-only libraries follow the single-translation-unit pattern. Their implementations must be compiled into exactly one `.cpp` file (`emu_peanut.cpp` and `emu_walnut.cpp` respectively) using a namespace wrap to avoid ODR violations.

---

<!-- Section: 03_conventions.md -->

# 7. Repository & File Structure
The project uses a **single, clean repository structure**. No numbered milestone folders (`01_foo`, `02_bar`) are permitted. Past states are tracked via git history.

```
repo-root/
├── README.md
├── CHANGELOG.md                      ← human-readable version log
├── .gitignore
├── docs/                             ← architecture docs & hardware notes
├── .agents/                          ← ground-truth LLM agent rules
│   └── rules/                        ← topic-specific modular rule files
├── tools/                            ← host test harness & asset tooling
├── scripts/                          ← ROM validator and build automation scripts
└── firmware/
    └── BmoGameboy/                   ← Arduino sketch folder (MUST match .ino)
        ├── BmoGameboy.ino            ← ONLY setup(), loop(), and state dispatch
        ├── partitions.csv            ← 8MB app0 custom partition table
        └── src/
            ├── core/                 ← hardware drivers (config.h, display, buttons, SDF face)
            ├── emulators/            ← glue code per console (emu_peanut, emu_walnut, emu_nes, emu_doom)
            ├── engine/               ← header-only engines (walnut_cgb)
            ├── vendor/               ← third-party libraries (peanut_gb, agnes, doom)
            ├── assets/               ← asset headers
            │   └── roms/             ← baked commercial ROM headers (mario, zelda)
            └── tests/                ← on-device unit tests
```
- **Rule:** Custom engines live in `src/engine/<name>/`, while third-party vendor libraries go under `src/vendor/<name>/`.
- **Rule:** Generated assets and baked ROM headers go under `src/assets/`.

---

# 8. Testing & Verification Workflow
- The file `src/tests/unit_tests.cpp` contains critical validation tests (e.g., PSRAM speed checks, module validation). 
- **Rule:** No task should be reported complete without at minimum a successful compile.
- **Rule:** If touching memory architecture or core drivers, the unit tests should be executed by calling `UnitTests::runAll()` in `setup()` before integration.

---

# 9. IP & Licensing Notes
This project currently bakes multiple commercial ROMs directly into the firmware flash as fallback games (`mario_deluxe.h`, `zelda_ages.h`, `aladdin.h`, `lego_racers.h` under `src/assets/roms/`). 
- This is a known, deliberate personal/non-commercial choice.
- **Test ROMs:** We use Blargg's `cpu_instrs.gb` (Public Domain) for CPU correctness validation in the host-side test harness.
- **FORWARD-LOOKING RULE:** No new copyrighted commercial ROM or third-party copyrighted asset may be baked into the firmware without it being explicitly logged in this file first.

---

# 10. Agent Behavior Rules
1. **Never Assume Hardware:** Agents MUST NOT guess at unverified hardware/toolchain facts. If you cannot definitively verify a hardware state, library version, or wiring layout from the code, DO NOT fabricate confident prose. Check using command line tools, or ask the user directly.
2. **Graceful Degradation:** Hardware is brittle. Use `FEATURE_*` flags meticulously.
3. **Isolate Peripherals:** When introducing a new hardware module (e.g., I2C FRAM), always write an isolated, standalone `.ino` sketch in the `tools/` or a scratch directory to verify the hardware works before integrating it into `BmoGameboy.ino`.
4. **Follow the Ground Truth:** You must trust these rules over your own inferences when resolving architectural intent.

---

# 11. Documentation Maintenance Protocol
The files under `.agents/rules/` MUST be updated immediately upon completing a task that involves:
- **Pin Mapping:** Adding, removing, or changing any GPIO pin assignment.
- **Feature Flags:** Adding a new `FEATURE_*` toggle.
- **Architecture:** Introducing a new core module or changing a fundamental loop/memory pattern.
- **Hardware Wiring:** When dormant hardware (like the battery circuit) is physically soldered.
- **Known Issues:** Resolving or discovering a bug that belongs in the Technical Debt log.

Routine bug fixes not touching hardware/architecture, or pure refactors that don't change the directory conventions, do NOT require updating these files.

---

<!-- Section: 04_known_issues.md -->

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
7. **Emulator Teardown PSRAM Leak (FIXED_UNVERIFIED)**: `WalnutEmu::destroy()` and `PeanutEmu::destroy()` were not previously called on SELECT+UP return-to-menu exit in `BmoGameboy.ino`, leaking 128KB cart_ram PSRAM per session. All 4 emulator cores (Walnut, Peanut, NES, DOOM) now call `destroy()` in the SELECT+UP handler. Status: `FIXED_UNVERIFIED`.

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
- **2026-08-30**: Ruleset v3 & SDD Upgrade — Fixed Walnut/Peanut `destroy()` PSRAM teardown in `BmoGameboy.ino` (status: FIXED_UNVERIFIED). Rewrote `docs/software-design-document.md` to full reviewer-grade specification. Expanded `10_symbol_reference.md` with all core/emulator APIs. Added `31_quick_start_primer.md`, root-level `CHANGELOG.md`, updated root `README.md`, synchronized `scripts/` and `tests/` paths, and added M-16 mistake entry. Updated directory trees in `03_conventions.md` and codebase map. (Agent Antigravity)
- **2026-08-30**: Ruleset v4 & AI Environment Upgrade — Elevated Software Design Document to v3.0 (authoritative standalone engineering specification). Added Rules 32 (Modular Core Template), 33 (Agent Handoff & Optimization Cycle), 34 (AI Agent Sandbox & Guardrails), anti-patterns M-17 through M-20 in Rule 30, and created machine-readable indices `AGENT_MANIFEST.json` and `.agents/rules/CONTEXT_INDEX.json`. Upgraded `validate_repo.py` into multi-phase AI Guardian validator and expanded `test_repo_tools.py` unit test suite. (Agent Antigravity)
- **2026-08-30**: Baked ROM Flash Audit & Registration — Validated `aladdin.h` and `lego_racers.h` ROM headers, checksums, and actual binary sizes (1,048,576 bytes each). Registered both in `sd_card.cpp` with flash .rodata protection guards in `freeRom()`. Compiled firmware: 4,986,092 bytes (59.44% of 8MB `app0` partition, leaving 3,402,516 bytes headroom). Status: FIXED_UNVERIFIED. (Agent Antigravity)
- **2026-08-30**: BmoFace Mascot Rendering & Visibility Fix — Resolved symptom where mascot was invisible/flickering. Fixed menu canvas full-screen overwrite covering face on clean frames, cached `faceBuf` in `blitFace()` to skip recomputation when clean, added 1000ms boot splash hold in `setup()`, and added 400ms game launch celebration hold in `BmoGameboy.ino`. Status: FIXED_UNVERIFIED. (Agent Antigravity)
- **2026-08-30**: Ruleset v5 (Governance Gaps) — Added 35_bmo_face_contract.md (mascot subsystem contract), 36_bug_intake_protocol.md (structured hardware bug intake protocol), and 37_rom_governance_and_flash_budget.md (ROM tracking truth & standing flash-budget invariant). Bumped RULESET_VERSION to 5, updated indices, and added cross-references across 07_task_protocol.md, 29_adding_a_baked_rom.md, 30_common_agent_mistakes.md, and 33_agent_handoff_and_optimization_cycle.md. (Agent Antigravity)

---

<!-- Section: 05_git_workflow.md -->

# Git & Commit Discipline

## Commit granularity
- One logical change per commit. "Logical" means: if you'd describe it
  with an "and" that isn't a shared cause (e.g. "fixed the pointer cast
  AND renumbered the docs"), it's two commits.
- Always checkpoint-commit BEFORE starting risky/multi-step work, so any
  session is revertable to a known-good point. This is not optional and
  is not the same commit as the work itself.
- Never combine a hardware-behavior change with a docs-only change in one
  commit — they have different review/verification needs.

## Commit message format (Conventional Commits)
`type(scope): summary`, where type is one of:
`fix | feat | docs | test | refactor | chore | perf`
- scope is the module touched (e.g. `emu_walnut`, `rules`, `sd_card`).
- Body (optional, below the summary line) must state verification status
  using the vocabulary in 06_verification_standards.md — e.g. "Verified:
  host-test suite full pass (11/11). Hardware: unverified."
- If the commit closes or changes a 04_known_issues.md entry, reference it:
  `Refs: KI-1`.

## Tags for hardware-verified states
- After a human confirms a PASS on real hardware per a handoff checklist,
  tag that commit: `git tag hw-verified-YYYYMMDD-<short-feature>`. This
  gives every future agent a guaranteed-good rollback point that isn't
  just "compiles" but "was seen working on the actual board." List active
  tags at the top of 04_known_issues.md so agents don't have to `git tag
  -l` and guess which one is relevant.

## Branching
- Default to committing directly to `main` for docs-only, test-only, or
  low-risk changes.
- For anything that changes CPU/memory-safety-critical code (emulator
  cores, pointer arithmetic, ISR/timer code) or touches a HARD STOP
  category from 00_hard_stops.md, create a short-lived branch
  (`fix/<topic>` or `feat/<topic>`), do the work, and only merge to `main`
  after the verification steps in 06_verification_standards.md pass. Say
  explicitly whether you branched and why if you didn't.

## What never gets committed
- Anything already excluded by .gitignore (ROMs, generated ROM headers).
- Downloaded toolchains (e.g. a Zig binary fetched for host_test) — commit
  a documented, pinned VERSION reference in 02_architecture.md instead, not
  the binary itself.
- Build output directories, host_test binaries, `.o`/`.elf`/`.bin` files.
- Anything from Part A of a prompt that Part A told you to correct — i.e.
  never commit an overclaimed result before it's actually true.

## Review gate before committing actual work
The existing checkpoint-commit-before-starting rule stays automatic — no
review needed for that, it's a safety net.
For the commit that represents COMPLETED work, before running `git
commit`: show me the exact `git diff --stat` (file list) and the proposed
commit message, and wait for an explicit yes before committing, IF any of
these apply:
- Any new file over 1MB is being added.
- Any new binary, executable, or downloaded-toolchain file is being added
  (this is exactly how the Zig binary ended up in history last time — a
  review step here would have caught it before the commit, not after).
- The change touches anything in a 00_hard_stops.md category.
- The change involves rewriting git history (filter-repo, rebase -i,
  amend of an already-old commit).
Routine docs-only or single-file low-risk commits don't need this gate —
use judgment, but the binary/size/hard-stop triggers above are not
judgment calls, they always require the review step.

---

<!-- Section: 06_verification_standards.md -->

# Verification & Evidence Standards
This file exists because two separate incidents already happened: a fully
hallucinated bug report (fabricated function names, presented with
confident specific detail), and a partial test result (9/11 sub-tests)
reported as a full pass with celebratory language. Both came from
rounding uncertain or incomplete evidence up to a confident claim. These
rules exist to make that structurally harder to do by accident.

## The core rule
**Every claim about code or test results must be backed by evidence
obtained in the CURRENT session, quoted literally, not paraphrased, and
not carried forward from memory of a prior session's summary.** If you
did not personally run the command/read the file in this session, say
"I have not re-verified this" rather than restating it as fact.

## Status vocabulary — use these exact terms, nothing else, in
   04_known_issues.md and commit messages:
- `OPEN` — known bug, no fix attempted yet.
- `IN_PROGRESS` — fix attempted, not yet compiled/tested.
- `FIXED_UNVERIFIED` — compiles, but no test has run against it.
- `VERIFIED_HOST` — a host-side or unit test ran to full, designed
  completion (not partial) and passed. State which test, and quote its
  final line of output.
- `VERIFIED_HARDWARE` — a human confirmed a PASS on physical hardware
  against an explicit, pre-stated PASS/FAIL definition. State the git tag
  created for this (see 05_git_workflow.md).
- `RESOLVED` — VERIFIED_HARDWARE has been reached AND no further action
  is needed.
- `WONT_FIX` — deliberately deferred, with a one-line reason.
- `DEBUNKED` — the "bug" was shown not to exist; state exactly how you
  proved its nonexistence (not "recompiled cleanly," since that's
  evidence of absence of a *syntax* error only).
- Never use "successfully passed," "fully resolved," "confirmed working,"
  or similar informal phrasing in the log — always one of the terms above.

## Partial results are not passes
If a test suite has N defined sub-checks (a ROM test, a multi-assert unit
test, a checklist), and fewer than N completed with the suite's own
designed success signal (e.g. a final "Passed" banner, an exit code, a
specific terminal state), the result is `IN_PROGRESS`, not
`VERIFIED_HOST`. State the exact fraction (e.g. "9/11 sub-tests reached
their :ok marker; sub-test 10 did not complete within the allotted
budget"). Do not editorialize about how likely the remaining tests are to
pass. If a resource limit (frame budget, timeout, memory) is the
suspected cause of an incomplete run, that is itself something to fix and
rerun, not something to reason past.

## Banned language in status reports
No superlatives or narrative framing when reporting engineering results:
banned words/phrases include but aren't limited to "massive victory,"
"flawlessly," "rigorously proves," "crushed it," "perfect." Report what
ran, what it output, and what state that leaves the issue in. Neutral and
boring is correct — see also 09_testing_infrastructure.md's loading-message
tone guidance, same principle.

## Quoting code
When asked to show code, or when justifying a claim about existing code,
paste the literal current file content (with a fresh read in this
session) inside a fenced block with line numbers if available. A
description of code ("it just checks bounds and returns") is not a
substitute when the standing order calls for showing code — that
distinction is exactly what caught the semicolon hallucination.

## Numeric claims (counts, hit totals, percentages)
If you state a count derived from a search/grep/audit, and you later
restate that count, it must be identical or you must explicitly flag and
explain the discrepancy — do not let a number silently drift between
messages. If unsure, rerun the search rather than recalling the earlier
number.

---

<!-- Section: 07_task_protocol.md -->

# Standard Task Protocol
Any agent (strong or weak model) starting ANY task in this repo should
follow this sequence. This exists so behavior doesn't depend on how
capable or careful the specific model is.

## Before starting
1. Read 00_hard_stops.md, 01_hardware.md in full — always, every task,
   regardless of how unrelated the task seems, since a task that looks
   pure-software can still touch a pin or a feature flag.
2. Read 27_codebase_map.md — this gives the full architecture, routing,
   PSRAM budget, SPI sharing, and bitmask layout in one place. Knowing
   this prevents the most common structural mistakes without reading 5 files.
3. Skim 30_common_agent_mistakes.md — 2 minutes, saves hours. If your task
   touches a category listed there, re-read the linked rule file in full.
4. Read 10_symbol_reference.md and confirm any function/struct/macro
   you're about to reference actually appears there. If it doesn't, grep
   for it yourself before using it in a claim — never assume a name
   exists because it "sounds right" for the codebase.
5. `git status`; checkpoint-commit if dirty.
6. State your plan in 3-6 bullets before writing code. If the task
   touches anything in 00_hard_stops.md's categories, say so explicitly
   and state which constraint applies.

## During
5. Prefer the smallest change that satisfies the request. Don't bundle
   unrelated cleanup into the same commit (see 05_git_workflow.md).
6. If you hit an unrelated bug while working, do NOT silently fix it
   inline — note it, ask, or log it as a new 04_known_issues.md entry
   with status `OPEN`, and finish the requested task first, unless it's
   blocking the requested task.
7. If you're about to fabricate a plausible-sounding detail because you
   don't actually know it (a line number, a variable name, a test
   result) — stop. State the uncertainty instead. This is the single
   highest-value habit for this repo given past incidents.
8. **Stop-after-2-failures rule:** if the same interaction/approach fails
   twice in a row (a build flag, a test invocation, a specific fix
   attempt), STOP retrying variations of the same approach. Switch
   strategy entirely, or stop and ask. Never spend more than 5 tool calls
   total on a single stuck sub-problem before changing approach or asking.
9. **Implementation-plan gate for substantial changes:** for anything
   multi-file, architectural, or that would trigger the review gate in
   05_git_workflow.md (hard-stop-adjacent, history-rewriting, new
   binaries) — write a short plan (a few bullets is fine, doesn't need to
   be a separate file for most cases) and wait for explicit approval
   before writing code or running risky commands. Trivial fixes and
   single-line edits skip this, same as always.

## Before reporting done
10. Compile. A successful compile is necessary, never sufficient, for
   "done" — see 06_verification_standards.md for what else is required
   depending on what was touched.
11. Update .agents/rules/ files per the Documentation Maintenance Protocol
   in 03_conventions.md, using the status vocabulary from
   06_verification_standards.md.
12. Update 04_known_issues.md's Changelog section with a one-line dated
    entry (see existing entries for format).

## Final report format (always use these two headers, verbatim)
```
## Verified by me this session
<only things you personally ran/read/confirmed in THIS session>

## Waiting on you
<anything requiring physical hardware, a human decision, or an external
resource you don't have access to — with an exact checklist, per
06_verification_standards.md's PASS/FAIL definition rules>
```
Never merge these two categories. Never imply the second category is done.

## Definition of Done, by task type
- **Docs-only change:** compiles N/A; diff reviewed; cross-references
  checked (grep for old section numbers/filenames before renaming).
- **Bug fix, no hardware-timing implication:** compiles; host-side test
  covering the fix passes to full completion; known_issues updated.
- **Change to CPU core / memory access / ISR / timing-critical code:**
  compiles; host-side test passes to full completion; hardware handoff
  checklist prepared; known_issues stays `FIXED_UNVERIFIED` or
  `VERIFIED_HOST` until a human reports back `VERIFIED_HARDWARE`.
- **New hardware peripheral integration:** isolated standalone `.ino` in
  tools/ verified first (03_conventions.md rule already requires this);
  01_hardware.md and the pin map updated ONLY after physical soldering is
  confirmed by the human, never in anticipation of it.
- **New UI screen/menu:** follows 08_ui_style_guide.md; no new raw hex
  colors outside the shared theme constants file.
- **Change touching src/assets/ or adding a vendor library:** flash-budget invariant in `37_rom_governance_and_flash_budget.md` verified this session.
- **Rules-directory / process change:** diff reviewed; every new file under 12,000 chars; README index and RULESET_VERSION updated; changelog entries added; no hard-stop or hardware-ground-truth text restated elsewhere.

---

<!-- Section: 08_ui_style_guide.md -->

# UI & Visual Style Guide
Purpose: stop each new screen/menu from reinventing colors, spacing, and
widget style from scratch, and avoid the "default Adafruit_GFX look"
(flat black background, default font, filled rectangles) creeping back in
after hardware-notes.md section 8's UI performance lesson already pushed
the project away from it once.

## Central theme file
- All colors, spacing constants, and font references MUST live in one
  file (`src/core/theme.h` — create it if it doesn't exist yet, and
  migrate any hardcoded `0xXXXX` RGB565 literals found elsewhere into it
  as named constants, e.g. `THEME_BG`, `THEME_ACCENT`, `THEME_TEXT_DIM`).
- No new screen may introduce a raw hex color literal. If a new semantic
  color is genuinely needed, add it to theme.h with a name, don't inline
  it.

## Layout
- Define and use a consistent spacing unit (e.g. an 8px grid) for margins
  and padding between UI elements, instead of ad hoc pixel offsets per
  screen. Put the constant in theme.h.
- Menu selection state: use outline (`drawRoundRect`), never filled
  (`fillRoundRect`), per the existing measured performance finding in
  hardware-notes.md section 8 (UI Rendering Performance) — this is a
  standing rule now, not a one-off optimization.
- Avoid animated background elements (grids, moving decorations) behind
  menus — same section, same reasoning: measured cost, not a guess.

## Typography
- Pick ONE primary UI font and ONE monospace/debug font for the whole
  project; name both explicitly in theme.h comments. Don't let different
  screens silently use different default GFX fonts.
- Define standard text sizes as named constants (e.g. `TEXT_SIZE_TITLE`,
  `TEXT_SIZE_BODY`, `TEXT_SIZE_SMALL`) instead of literal `setTextSize(2)`
  calls scattered around.

## Motion & feedback
- Any animation (blink timing, transitions, loading indicators) must be
  driven by the existing non-blocking `millis()` delta pattern already
  established for BmoFace — never a blocking `delay()` inside a
  UI-drawing path, since that stalls button polling.
- New screens should have an explicit idle/loading/error visual state,
  not just a happy path — mirroring the crash/low-battery states BmoFace
  already defines.

## When adding a new screen
Checklist: uses theme.h constants only; uses the shared spacing grid;
reuses an existing menu/list widget if one exists rather than writing a
new one; documented in docs/ if it introduces a new navigable state in
the state machine.

---

<!-- Section: 09_testing_infrastructure.md -->

# Testing Infrastructure
Formalizes the host-side test harness introduced for Walnut-CGB
verification so it's a standing part of the project, not a one-off.

## Layout
- `tools/host_test.cpp` — host-side (desktop) test harness. Compiles
  vendor cores standalone via the same namespace-wrap pattern used for
  the on-device `.cpp` wrappers (02_architecture.md), so CPU-execution
  correctness can be tested WITHOUT a physical board.
- Toolchain used to compile it (compiler + exact version) MUST be pinned
  and documented in 02_architecture.md's toolchain section — "downloaded
  X to make this work" in a session is not reproducible for the next
  agent or the next machine. State the exact acquisition method too
  (package manager, direct download URL + version).

## Test ROM ledger (required — do not add a ROM without this)
Maintain a table (in this file or a linked LICENSES.md) of every test ROM
in use: name, source URL, license, and which sub-tests/instruction
categories it covers. Entry format:
| ROM | License | Source | Covers |
|---|---|---|---|
| `cpu_instrs.gb` (Blargg) | Public Domain | https://github.com/retrio/gb-test-roms | 11 sub-tests covering Game Boy CPU instruction correctness and timing |
This is required before `03_conventions.md` section 9 logging is
considered complete — that section says a ROM must be logged before being
added; this table is where.

## What "passing" means, per suite
For any multi-part test ROM (Blargg-style, Mooneye-style), define the
suite's own completion signal explicitly here once (e.g. "Blargg
cpu_instrs.gb: individual sub-tests print `NN:ok`; full suite success is
the literal string `Passed` printed after all sub-tests complete") so
no future agent has to infer or guess when a run actually finished vs.
timed out. See 06_verification_standards.md for the rule that partial
sub-test completion is `IN_PROGRESS`, not `VERIFIED_HOST`.

## Resource budgets
If a test harness uses a frame/cycle/time budget, that budget must be
generous enough for the slowest defined sub-test to reach its own
completion signal, and the run must FAIL LOUDLY (not silently truncate)
if the budget is exceeded before completion — the harness should print
something unambiguous like `TIMEOUT: budget exceeded before suite
completion`, not just stop.

## When to run this
Per 03_conventions.md section 8: any change to memory architecture or
core drivers already requires `UnitTests::runAll()`. Extend that: any
change to a vendor emulator core's CPU-execution path (opcode handlers,
memory read/write callbacks, register logic) requires a full host_test
run to designed completion before the change can be marked anything
better than `FIXED_UNVERIFIED`.

## Hardware-in-the-loop handoff format
When a change needs physical confirmation, always produce a checklist
with: exact file to copy and where; exact build flags (cross-reference
00_hard_stops.md's OPI/partition requirements verbatim, don't paraphrase
them from memory); an explicit, unambiguous PASS description; an explicit
FAIL description. This is the format already used successfully for the
Walnut-CGB handoff — keep using it, just make sure the PASS bar quoted in
the checklist matches what the host-side harness actually confirmed to
have run (see Part A of this session).

---

<!-- Section: 10_symbol_reference.md -->

# Symbol Reference (Ground Truth for Names)
Purpose: the semicolon-hallucination incident happened because a
plausible-sounding function name (`__gb_write16`) and struct field
(`gb->cpu_reg.sp.reg`) were invented and never existed anywhere in the
codebase. This file is the single place to check before referencing a
symbol you're not 100% sure of, so "does this function exist" is a table
lookup, not a memory guess.

## Maintenance rule
This file must be regenerated (not hand-edited from memory) any time a
core emulator file, driver, or shared header changes its public
functions/macros/structs. Regenerate by grepping actual function
signatures, macro `#define`s, and top-level struct declarations from:
`src/core/`, `src/emulators/`, `src/engine/*/`, `src/vendor/*/`.

## Staleness detection
This file was last regenerated: **2026-08-30**.
If any of the following files have a newer git commit date than that,
this file is stale -- grep live and update before making any symbol claims:
- `src/emulators/emu_walnut.cpp/h`
- `src/emulators/emu_peanut.cpp/h`
- `src/emulators/emu_nes.cpp/h`
- `src/emulators/emu_doom.cpp/h`
- `src/core/display_emu.h`
- `src/core/buttons.h`
- `src/core/sd_card.h`
- `src/core/bmo_face.h`
- `src/core/battery.h`
- `src/core/config.h`
- `src/engine/walnut_cgb/walnut_cgb.h`

Run: `git log --oneline -1 -- <file>` for each to check. If any show a
commit newer than the date above, grep live instead of trusting this table.

## Format
One table per file, columns: `Symbol | Kind (fn/macro/struct) | Signature | Notes`.

---

## src/core/display_emu.h (DisplayEmu)
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `DisplayEmu::begin` | fn | `void begin()` | Initializes ST7789 display over SPI |
| `DisplayEmu::clearScreen` | fn | `void clearScreen()` | Clears screen to black |
| `DisplayEmu::showSDCardWarning` | fn | `void showSDCardWarning()` | Shows SD card error message |
| `DisplayEmu::initMenuUI` | fn | `void initMenuUI()` | Allocates menu buffer in PSRAM |
| `DisplayEmu::cleanupMenuUI` | fn | `void cleanupMenuUI()` | Frees menu buffer in PSRAM |
| `DisplayEmu::drawConsoleSelectMenu`| fn | `void drawConsoleSelectMenu(int selectedIndex, const int gameCounts[4], bool sdMounted)` | Renders console menu carousel |
| `DisplayEmu::drawGameSelectMenu` | fn | `void drawGameSelectMenu(const RomFile* const* games, int count, int selectedIndex, RomType console, bool sdMounted)` | Renders game list menu |
| `DisplayEmu::startFrame` | fn | `void startFrame()` | Asserts CS & sets 240x216 window once |
| `DisplayEmu::endFrame` | fn | `void endFrame()` | Deasserts CS |
| `DisplayEmu::streamPixelRow` | fn | `void streamPixelRow(const uint16_t* buf, int pixelCount)` | Streams scanline without CS toggle |
| `DisplayEmu::pushPixelsFullScreen`| fn | `void pushPixelsFullScreen(const uint16_t* buffer)` | Full 320x240 frame blit |
| `DisplayEmu::pushPixelsAt` | fn | `void pushPixelsAt(int x, int y, int w, int h, const uint16_t* buf)` | Arbitrary sub-rect blit |
| `DisplayEmu::CLASSIC_PALETTE` | const | `const uint16_t CLASSIC_PALETTE[4]` | Pre-swapped BGR565 green palette |
| `DisplayEmu::NES_PALETTE` | const | `const uint16_t NES_PALETTE[64]` | Pre-swapped BGR565 NES palette |

---

## src/core/buttons.h (Buttons)
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `Buttons::begin` | fn | `void begin()` | Configures button GPIOs as INPUT_PULLUP |
| `Buttons::update` | fn | `void update()` | Reads GPIO_IN_REG atomically, updates state |
| `Buttons::count` | fn | `int count()` | Returns number of buttons (8) |
| `Buttons::get` | fn | `const ButtonState& get(int index)` | Returns ButtonState struct |
| `Buttons::gb_joypad_state` | var | `uint8_t gb_joypad_state` | Active-low joypad bitmask (0=pressed) |
| `Buttons::Index` | enum | `enum Index : int { UP, DOWN, LEFT, RIGHT, A, B, START, SELECT }` | Button indices |
| `ButtonState` | struct | `struct ButtonState { const char* name; uint8_t pin; bool pressed; bool changed; }` | Button state info |

---

## src/core/sd_card.h (SDCard)
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `SDCard::begin` | fn | `bool begin()` | Mounts SD & registers baked flash ROMs |
| `SDCard::isMounted` | fn | `bool isMounted()` | Returns true if SD card is mounted |
| `SDCard::scanRoms` | fn | `void scanRoms()` | Populates romList from SD card |
| `SDCard::getRomCount` | fn | `int getRomCount()` | Returns total ROM count (baked + SD) |
| `SDCard::getRomInfo` | fn | `const RomFile* getRomInfo(int index)` | Returns RomFile pointer |
| `SDCard::loadRom` | fn | `uint8_t* loadRom(const char* filename, size_t* outSize)` | Loads ROM to PSRAM (or returns .rodata) |
| `SDCard::freeRom` | fn | `void freeRom(uint8_t* buffer)` | Frees PSRAM ROM (safe for .rodata) |
| `RomType` | enum | `enum RomType { ROM_UNKNOWN, ROM_GB, ROM_GBC, ROM_NES, ROM_WAD }` | Console type enum |
| `RomFile` | struct | `struct RomFile { char filename[64]; RomType type; }` | ROM entry info |

---

## src/core/bmo_face.h (BmoFace)
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `BmoFace::begin` | fn | `void begin()` | Initializes RNG and clears dirty flag |
| `BmoFace::setExpression` | fn | `void setExpression(BmoExpression expr)` | Sets target facial expression |
| `BmoFace::update` | fn | `void update()` | Ticks procedural animation & blink |
| `BmoFace::draw` | fn | `void draw(int x, int y, int size)` | Renders & blits scaled face |
| `BmoFace::draw` | fn | `void draw()` | Blits large centered face |
| `BmoFace::isDirty` | fn | `bool isDirty()` | True if expression or blink changed |
| `BmoFace::BmoExpression`| enum | `enum BmoExpression { IDLE, SURPRISED, HAPPY, SLEEPY, LOW_BATTERY, CHARGING, ERROR, SHUTDOWN, HIDDEN }` | Face emotion states |

---

## src/core/battery.h (Battery)
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `Battery::begin` | fn | `void begin()` | Initializes ADC pin (dormant if flag=0) |
| `Battery::getVoltage` | fn | `float getVoltage()` | Returns smoothed voltage (0.0 - 4.2V) |
| `Battery::getPercentage`| fn | `int getPercentage()` | Returns percentage (0 - 100) |
| `Battery::update` | fn | `void update()` | Updates moving average; checks low-cutoff |
| `Battery::safeShutdown` | fn | `void safeShutdown()` | Shows low battery face & triggers deep sleep |

---

## src/emulators/ Public APIs
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `PeanutEmu::begin` | fn | `bool begin(const uint8_t* rom_data, size_t rom_len)` | Starts Game Boy DMG core |
| `PeanutEmu::updateJoypad` | fn | `void updateJoypad()` | Syncs direct.joypad with buttons |
| `PeanutEmu::runFrame` | fn | `void runFrame()` | Runs 1 frame inside startFrame/endFrame |
| `PeanutEmu::destroy` | fn | `void destroy()` | Frees cart_ram in PSRAM |
| `WalnutEmu::begin` | fn | `bool begin(const uint8_t* rom_data, size_t rom_len)` | Starts Game Boy Color core |
| `WalnutEmu::updateJoypad` | fn | `void updateJoypad()` | Syncs direct.joypad with buttons |
| `WalnutEmu::runFrame` | fn | `void runFrame()` | Runs 1 frame inside startFrame/endFrame |
| `WalnutEmu::destroy` | fn | `void destroy()` | Frees cart_ram in PSRAM |
| `NesEmu::begin` | fn | `static bool begin(const uint8_t* romData, size_t romSize)` | Starts Agnes NES core |
| `NesEmu::updateJoypad` | fn | `static void updateJoypad()` | Syncs Agnes controller mask |
| `NesEmu::runFrame` | fn | `static void runFrame()` | Runs 1 NES frame |
| `NesEmu::destroy` | fn | `static void destroy()` | Shuts down Agnes NES core |
| `DoomEmu::begin` | fn | `bool begin(const char* wadPath)` | Starts doomgeneric core |
| `DoomEmu::runFrame` | fn | `void runFrame()` | Runs 1 DOOM engine tick |
| `DoomEmu::destroy` | fn | `void destroy()` | Shuts down doomgeneric core |

---

## src/engine/walnut_cgb/walnut_cgb.h
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `gb_init` | fn | `enum gb_init_error_e gb_init(struct gb_s* gb, uint8_t(*gb_rom_read)(struct gb_s*, const uint_fast32_t), uint16_t(*gb_rom_read16)(struct gb_s*, const uint_fast32_t), uint32_t(*gb_rom_read32)(struct gb_s*, const uint_fast32_t), uint8_t(*gb_cart_ram_read)(struct gb_s*, const uint_fast32_t), void (*gb_cart_ram_write)(struct gb_s*, const uint_fast32_t, const uint8_t), void (*gb_error)(struct gb_s*, const enum gb_error_e, const uint16_t), void* priv)` | 8 arguments including 16/32 read callbacks |
| `gb_run_frame` | fn | `void gb_run_frame(struct gb_s *gb)` | Executes one frame via `__gb_step_cpu_x` |
| `gb_run_frame_dualfetch` | fn | `void gb_run_frame_dualfetch(struct gb_s *gb)` | Executes one frame via `__gb_step_cpu` (dual-fetch path) |
| `gb_init_lcd` | fn | `void gb_init_lcd(struct gb_s *gb, void(*lcd_draw_line)(...))` | Registers scanline callback |
| `gb_s` | struct | `struct gb_s` | Core emulator context, align(32) |
| `gb_init_error_e` | enum | `enum gb_init_error_e` | Initialization error codes |

---

## src/core/config.h
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `FEATURE_SD_CARD` | macro | `#define FEATURE_SD_CARD 1` | Gates SD logic |
| `FEATURE_BATTERY_MONITOR` | macro | `#define FEATURE_BATTERY_MONITOR 0` | Dormant battery logic |
| `FEATURE_AUDIO` | macro | `#define FEATURE_AUDIO 0` | Dormant I2S audio logic |
| `BTN_UP`... | macro | `#define BTN_UP 4` (etc) | GPIO pin assignments |
| `LOG_INFO`, `LOG_ERROR`... | macro | `#define LOG_INFO(fmt, ...)` | Gated logging macros |

---

<!-- Section: 11_rules_meta.md -->

# Meta-Rules for the Rules Directory Itself
**RULESET_VERSION: 5** <!-- v5 = closed mascot contract, bug intake protocol, and ROM governance -->

- Every file in .agents/rules/ must stay under 12,000 characters (the
  original constraint that caused project-rules.md to be split). If a
  file grows past that, split it into a new numbered file and update
  README.md's index — don't silently let one file balloon.
- Numbered files (00-12 existing) are read in numeric order and are
  mandatory context for any nontrivial task. README.md is the index and
  is always safe to read first — it should never itself contain binding
  rules, only pointers.
- Any change to a numbered rules file requires a one-line changelog entry
  in 04_known_issues.md's Changelog section, dated, same as existing
  entries.
- If a rule in one file contradicts a rule in another, 00_hard_stops.md
  always wins, followed by 01_hardware.md (physical ground truth beats
  everything except hard stops). Flag any contradiction you find instead
  of silently picking one.
- Don't restate a hard stop's full text in another file — reference it by
  file+section (e.g. "per 00_hard_stops.md, OPI flash mode") so the
  constraint has exactly one canonical source and can't drift out of sync
  across files.

---

<!-- Section: 12_extensibility_contract.md -->

# Extensibility Contract (New Emulator Cores)
Purpose: adding a 5th console shouldn't require touching code that the
existing 4 already depend on. Modeled on a "zero-touch core" pattern from
another project, adapted to this one.

## Adding a new emulator core
- New files ONLY under `src/emulators/<name>/` (glue) and
  `src/vendor/<name>/` (pristine third-party source), per
  03_conventions.md's existing directory rule.
- Every core must implement the same shared lifecycle contract: an init
  function, a per-frame run function, an input-handling path that reads
  the shared `gb_joypad_state`-style bitmask (not its own polling), and
  an explicit teardown/dispose function (see below).
- `BmoGameboy.ino`'s state-dispatch logic may be touched at exactly ONE
  point: the registration/dispatch table that maps a ROM extension to a
  core. Nothing else in `BmoGameboy.ino`, `display_emu.cpp`, or
  `buttons.cpp` should need to change to add a core. If it does, that's a
  sign the shared interface is missing something — fix the interface, not
  the core files, and say so explicitly rather than patching around it.

## Emulator teardown on switch (NEW RULE — currently not enforced anywhere)
When switching from one emulator core to another (returning to menu,
loading a different ROM), the outgoing core MUST explicitly free every
PSRAM buffer it allocated (ROM buffer, save-state buffer, any
core-specific scratch memory) and reset its static state before the next
core initializes. Repeated play sessions without this will fragment or
exhaust PSRAM over time. Add this as an explicit `teardown()` step in
every core's glue file, and call it from the state-machine transition in
`BmoGameboy.ino`, not left implicit.

## No per-frame heap allocation (NEW RULE)
No emulator core's per-frame render or CPU-execution loop may call
`malloc`, `heap_caps_malloc`, `new`, or any other heap allocation.
Everything needed inside the hot loop must be allocated once at core
init and reused. This applies with extra force to PSRAM allocations,
which fragment badly on repeated alloc/free cycles on this hardware. If
you find an existing violation while working on something else, log it in
04_known_issues.md as `OPEN` rather than silently fixing it inline (per
07_task_protocol.md's existing rule about unrelated bugs).

## General Peripheral Extensions
The zero-touch / init-update-teardown / `FEATURE_*`-flag contract described above for emulator cores is the general pattern for **any** new hardware peripheral module (FRAM, audio, battery, a future second display). Additionally, the rules in `14_error_handling_and_fault_isolation.md` and `15_performance_budgets.md` apply to any such hardware addition.

---

<!-- Section: 13_code_style.md -->

# Code Style & Formatting
Purpose: stop "works but inconsistent" drift across four emulator cores plus custom drivers, written across many sessions with no shared style memory between them.

## Naming Conventions
- `snake_case` for functions inside emulator glue, matching existing vendor convention (e.g., `gb_rom_read16`).
- `PascalCase` for first-party classes/structs (e.g., `Buttons`, matching `Buttons::update()`).
- `SCREAMING_SNAKE_CASE` for macros/constants (e.g., `FEATURE_SD_CARD`, `BTN_UP`). Extend the existing convention, don't introduce a third one.

## Module Structure
- One module = one `.h`/`.cpp` pair under `src/core/`. Don't add unrelated free functions to an already-open file just because it's open.
- Header guards: use `#pragma once` (matching existing convention in `src/core/config.h` and others).

## Language & Libraries
- **No `String` class** (Arduino) in any function called more than once per frame — it heap-allocates. Use fixed-size `char[]` buffers or `snprintf`. Applies to menu label rendering, assembled debug strings, and file path construction.
- RTTI and C++ exceptions are disabled by default in the Arduino-ESP32 build environment. Do not write code that depends on `try/catch` or `dynamic_cast`.

## Syntax & Const-Correctness
- No raw magic numbers for pins, colors, sizes, or timing outside `config.h`/`theme.h` — generalizes `08_ui_style_guide.md`'s theme rule to all constants, not just UI color.
- Braces: same-line opening brace; always brace single-statement `if`/`for` bodies — no bare-statement shorthand, full stop.
- `const`-correctness: any pointer/reference parameter a function doesn't mutate is `const`; any local never reassigned is `const`.

## Control Flow
- **No recursion in any emulator hot path** (CPU step, per-scanline render) — stack depth isn't statically boundable the way a loop is on this MCU.

## Comments
- Comment *why*, not *what*. `// skip if SD absent` next to `if (!sdPresent) return;` is noise; `// SD absent: fall back to baked ROM per hardware-notes.md §7` earns its line.
- TODO format: `// TODO(scope): description`. A bare `// TODO` with no scope/description isn't acceptable — if you can't state the follow-up, log it in `04_known_issues.md` instead.

---

<!-- Section: 14_error_handling_and_fault_isolation.md -->

# Error Handling & Fault Isolation
Purpose: ties `00_hard_stops.md`'s no-fatal-deadlock rule and the `gb_error` panic path into one place so failure handling isn't reinvented ad hoc per module.

## Core Philosophy
- Every module's failure path degrades to a **visible, non-fatal state** (e.g., adding an error/panic display state to the `BmoFace` state machine, a Serial log line, or a returned error enum) — never a silent `return;` leaving state inconsistent, never a blocking loop waiting on hardware that might not be there (already a `00_hard_stops.md` category).
- Never swallow an init function's return value (`gb_init`, `SD.begin()`, display init). Check it; on failure, enter that subsystem's explicit error state rather than proceeding as if it succeeded.

## Emulator Panics
Emulator panics (`gb_error` and equivalents in other cores) must: 
1. Stop that core's execution.
2. Free that core's PSRAM allocations via its `teardown()` (`12_extensibility_contract.md`).
3. Return to the menu/BmoFace with a visible reason string — never a silent reboot, never a hang.

## Untrusted Input
- Anything read from the SD card (ROM headers, save files, generated cover art) is **untrusted input**. 
- Validate size and the Nintendo-logo checksum (`software-design-document.md` §9) before treating a file as valid. 
- A failed validation goes to the visible error state — never a "partial load."

## Subsystem Error Codes
- Define one small error-code enum per subsystem (SD, save/FRAM, each emulator core) instead of magic ints or a `bool` that collapses multiple failure reasons into one bit. 
- Register these in `10_symbol_reference.md` like any other symbol.

## Assertions
- Use asserts liberally in the **host_test** desktop build (`09_testing_infrastructure.md`), where a failed assert just prints and exits — that's where violated invariants should be caught. 
- On-device, a failed invariant degrades to the visible error state above, never `abort()`/halt — a handheld that looks bricked with no visible cause is worse for field debugging than a "core X crashed: reason Y" screen.

---

<!-- Section: 15_performance_budgets.md -->

# Performance Engineering & Budgets
Purpose: make "fast enough" a checkable number. The project already has one hard number (16742 µs/frame); generalize the discipline.

## Budget Ledger
The budget for every hot path that has one:
| Subsystem/Path | Budget |
|---|---|
| GB/GBC frame | 16742 µs |
| Boot-to-menu time | TODO (requires on-device flash + Serial capture, not available this session) |
| SD mount time | TODO (requires on-device flash + Serial capture, not available this session) |
| ROM-load time (largest ROM) | TODO (requires on-device flash + Serial capture, not available this session) |

If a number doesn't exist yet, mark it `TODO` rather than inventing one.

## Measurement & Validation
- Any change to a hot path (opcode dispatch, per-scanline render, input poll) expected to affect timing must be measured before/after with `micros()`/`millis()` instrumentation or the host_test harness's cycle counting, with both numbers stated in the commit body — "should be faster" is not a verification (`06_verification_standards.md`).
- "Fast enough" claims always require a number and a method: what was measured, with what tool, over how many frames/iterations — never "feels smoother."

## No Per-Frame Heap Allocation
- **No heap allocation** (`malloc`, `new`, `heap_caps_malloc`) inside any per-frame path — generalizes `12_extensibility_contract.md`'s no-per-frame-allocation rule to UI/menu render loops too, not just emulator cores.

## Memory Budgets & Usage
- **PSRAM budget:** Ceiling is 8MB octal (`01_hardware.md`). Any new PSRAM consumer must state worst-case allocation size in this table before merging, so exhaustion is caught at design time, not by an on-hardware OOM.
- **DRAM budget:** ~400KB internal DRAM (`software-design-document.md` §3), shared with the WiFi/BT stack even if unused — if any future change enables a radio feature, re-check this budget explicitly. (Note: no high-water mark logging currently exists for this, so use heap tracking manually if needed).
- **IRAM budget:** List of functions currently claiming `IRAM_ATTR` so a future addition can check for room rather than silently evicting something else from cache:
  - `agnes.c`
  - `unit_tests.cpp`
  - `emu_peanut.cpp`
  - `emu_walnut.cpp`
  - `BmoGameboy.ino`

## Cache-line Alignment
- Cache-line alignment (`__attribute__((aligned(32)))`) applies to any new hot-path struct read every CPU tick/scanline, not just `gb_s`. See `02_architecture.md` for the original instance.

---

<!-- Section: 16_logging_and_diagnostics.md -->

# Logging & Diagnostics

## Log Levels
- `ERROR`: Subsystem entered its fault state.
- `WARN`: Degraded but continuing (e.g. SD absent, falling back to baked ROM).
- `INFO`: State transitions: boot, menu, ROM load, core switch.
- `DEBUG`: Per-frame/high-frequency — must be compile-time gated out of any non-dev build, since `Serial.print` in a hot loop violates `15_performance_budgets.md`. Note: The current codebase contains violations of this rule (appears in 9 files, 41 total call sites); they are tracked in `04_known_issues.md`.

## Implementation Rules
- Gate `DEBUG` (and ideally `INFO`) behind one compile-time flag (e.g. `LOG_LEVEL` in `config.h`) — one knob, not scattered per-file `#ifdef DEBUG_FOO` blocks.
- The same banned-superlatives list as `06_verification_standards.md` applies to any log/diagnostic string shown to a human — a log line is a status report.

## Visible Crash States
- On-device crash/error screens state: which subsystem, which error code (`14_error_handling_and_fault_isolation.md`'s enums), and, where feasible, a short actionable next step ("re-insert SD card") — not just "Error."

## Boot Log
- Boot log (`INFO` minimum) states: firmware version (`17_release_and_versioning.md`), which `FEATURE_*` flags are compiled in, and the SD self-test result (`software-design-document.md` §7) — the fastest way for a human or the next agent session to know what they're looking at.

## Secrets
- Never log secrets/keys. Not currently applicable (no network stack), but stated now so it isn't forgotten if WiFi/OTA is ever added.

---

<!-- Section: 17_release_and_versioning.md -->

# Release & Versioning

## Version Location
- Firmware version lives in one place (`src/core/version.h` — create if absent) as `FW_VERSION_MAJOR/MINOR/PATCH` plus a build-date string, printed in the boot log per `16_logging_and_diagnostics.md`. Note: currently no version constant exists; the next agent to prepare a release must create it.

## Bump Rules
- Bump **PATCH** for bug fixes.
- Bump **MINOR** for new features/cores/screens.
- Bump **MAJOR** for anything breaking save-file/ROM compatibility with a prior version. 
- State this explicitly in the commit body, since "I lost my save" is a real risk on a personal device, not an abstract semver nicety.

## Tagging
- `hw-verified-YYYYMMDD-<feature>` tags (`05_git_workflow.md`) verify one feature on hardware; a version bump is "what's actually flashed right now." 
- A version can bundle several already-hw-verified features accumulated since the last bump — don't conflate the two tagging systems.

## Release Checklist
Before flashing a build meant for actual day-to-day use (not bench iteration):
- Compiles clean.
- Every touched subsystem is at least `VERIFIED_HOST` (and `VERIFIED_HARDWARE` for anything CPU-core/memory/ISR-related).
- `04_known_issues.md` reviewed for any `OPEN`/`IN_PROGRESS` entry affecting features in use.
- Version bumped and confirmed printed in the boot log.

## Human Changelog
- Keep a human-facing version changelog (e.g. `docs/CHANGELOG.md`) distinct from `04_known_issues.md`'s granular per-task log.
- The task log is for agents/developers; the version log is "what changed since I last flashed this."

---

<!-- Section: 18_dependency_and_vendor_sync.md -->

# Dependency & Vendor-Library Sync

## Vendor Patching
- Any local modification to a `src/vendor/<name>/` file gets an inline `// BMO-PATCH: <one-line reason>` comment at the exact changed line(s) — not just a commit message. (Note: Currently no `BMO-PATCH` tags exist in the repo. Any future local modifications must use this).
- This ensures a future upstream sync can grep `BMO-PATCH` and know exactly what to re-check, instead of diffing the whole file from scratch.
- Never reformat a vendor file wholesale ("cleaning it up") — that destroys upstream diffability. Vendor files stay byte-for-byte upstream except `BMO-PATCH`-marked lines, operationalizing `03_conventions.md`'s existing "as close to upstream as possible" rule.

## Syncing Upstream
Before syncing a vendor library to a newer upstream version: 
1. Grep that directory for `BMO-PATCH` first.
2. List every patch found.
3. Confirm each is still necessary, superseded, or needs re-applying.
4. State this explicitly in the commit body.

## Version Pinning
- Pin exact versions for every vendor/Arduino-library dependency in `02_architecture.md`'s toolchain table (already done for the ST7789 and SD libraries).
- Extend this to `peanut_gb`, `walnut_cgb`, `agnes`, `doomgeneric` with an exact commit hash or release tag, not "latest," matching the existing Zig-pinning rationale.

## Patch Ledger
- Maintain a short patch ledger (a table here, or a linked `docs/VENDOR_PATCHES.md` if it outgrows this file's char budget): file, line, one-line reason, date added.

| File | Line | Reason | Date Added | Status |
|---|---|---|---|---|
| `src/emulators/emu_walnut.cpp` | ~L84, ~L90 | Replaced unsafe unaligned pointer casts with byte-wise little-endian reconstruction | 2026-08-29 | ACTIVE |
| `src/engine/walnut_cgb/walnut_cgb.h` | ~L67 | Enabled 16-bit fast paths (DUALFETCH + 16BIT_OPS) | 2026-08-29 | **REVERTED 2026-08-30** -- caused Mario Deluxe GBC freeze (INC-3). See `24_vendor_flag_safety.md`. |

## Vendor flag changes require 24_vendor_flag_safety.md compliance
Any change to a `#define` value in `src/engine/*/` or `src/vendor/*/`
that is NOT a whitespace/comment-only change must follow the protocol in
`24_vendor_flag_safety.md` before being committed. Add a row to this
table when you make such a change, and update its Status if reverted.

---

<!-- Section: 19_security_and_data_integrity.md -->

# Security & Data Integrity
Note: The current hardware (`01_hardware.md`) has no network stack enabled (no `WiFi.h`/`BluetoothSerial`). If WiFi/OTA is ever added, this file MUST be revisited before that feature ships.

## Untrusted Storage
- Every SD-sourced file (ROM, save state, generated cover art) is untrusted input the moment it's read.
- Validate size bounds and expected header/checksum (existing Nintendo-logo check) before using its contents to index a buffer or drive a loop bound.
- A malformed file fails into the visible error state (`14_error_handling_and_fault_isolation.md`), never reads/writes outside its buffer.

## Path Traversal
- ROM/save filenames read from the SD directory must not be used to construct a path that escapes the intended directory (no `..` traversal). 
- This is a cheap guard to add now, before any future PC-sync or OTA feature raises the stakes.

## FRAM Integrity
- FRAM save-blob integrity: since saves moved to the FM24C FRAM module specifically for write endurance (`software-design-document.md` §7), add a checksum/CRC to the save blob so a partial write (e.g. from a brownout) is detected on load instead of silently loading a corrupted game state. (Note: Check if this already exists before proposing it as new).

## Python Tooling
- `process_games.py`/`validate_repo.py` process ROM `.zip` archives. 
- If these are ever run against files from an untrusted source (not the developer's own trusted local collection), note Python's `zipfile` is vulnerable to zip-bomb/path-traversal patterns and should validate member paths explicitly. (Forward-looking only; don't over-engineer a threat model that doesn't currently apply).

---

<!-- Section: 20_multi_agent_protocol.md -->

# Multi-Agent / Cross-Session Protocol
Purpose: this repo is worked on by different AI agents/models across sessions with no shared working memory between them — the changelog already credits "(Agent Antigravity)" by name. Make the handoff explicit rather than accidental.

## Crediting & Logging
- Every `04_known_issues.md` changelog entry representing a distinct agent/model session credits which agent produced it, in parentheses, matching the existing `(Agent Antigravity)` convention.

## Verification Boundaries
- Never trust a prior session's summary of "what's true about the code" as verified fact for your own session — re-run the check yourself if it matters to what you're about to do. 
- This restates `06_verification_standards.md`'s core rule for the specifically cross-agent case: a previous agent's confident, well-formatted claim is exactly as unverified to you as an unverified guess of your own.
- If you discover a prior changelog/`04_known_issues.md` status was wrong (e.g. claimed `VERIFIED_HOST` but the quoted output doesn't actually show a full pass), correct it with a new dated entry stating what was wrong and the corrected status — don't silently overwrite the old entry, and don't leave it standing uncorrected.

## Handoffs & Pauses
- When stopping mid-task (context limit, session end, handoff), leave the repo so `git status` plus the latest changelog entry fully explain what's done, half-done, and next.
- Don't rely on an out-of-band chat transcript surviving to the next session, since the next agent may be a different model reading only the repo.

## Conflict Resolution
- If two agents' changes conflict, resolve via git history and commit messages as source of truth, not by guessing intent.
- If genuinely ambiguous, ask the human rather than silently picking a side, per `03_conventions.md` section 10's existing agent-behavior rules.
- Write your plan and final report (`07_task_protocol.md`) assuming zero shared context with whoever reads it next — even if that's "future you" in a new session.

---

<!-- Section: 21_documentation_standards.md -->

# Documentation Standards
Purpose: `docs/` currently exists as human-readable stubs (`hardware-notes.md`, `software-design-document.md`, `project-rules.md`, `wiring/`) with no stated content contract — give it one so it can't drift.

## Documentation Hierarchy
- `docs/` is for humans; `.agents/rules/` is ground truth the moment the two disagree. 
- Any `docs/` page describing current hardware/architecture state links to the relevant `.agents/rules/` file rather than duplicating it.

## UI Screens
- Every new navigable UI state/screen (per `08_ui_style_guide.md`'s existing checklist item) gets one short `docs/` page: purpose, how to reach it, which `FEATURE_*` flags gate it.

## Code Headers
- File header comment template for new first-party `.h`/`.cpp` files: one line of purpose, which `FEATURE_*` flag (if any) gates the whole file, and a pointer to the governing rules file if applicable (e.g. `display_emu.cpp` → `08_ui_style_guide.md`).
- Function-level doc comments required for anything declared in a header (exposed outside its own file): one line of purpose plus any non-obvious precondition (e.g. "must be called after `SPI.begin()`"). 
- Not required for file-local `static` helpers where name + one-line body already says it.

## Architecture Decision Records (ADR)
- Lightweight ADR for any change matching `03_conventions.md` section 11's "Architecture" trigger.
- Add a short dated note under `docs/adr/` (create if absent) stating the decision and the one-sentence reason, cross-referencing the rules file updated as a result. 
- Deliberately lightweight — a few sentences, not a formal template.

---

<!-- Section: 22_review_checklist.md -->

# Everyday Self-Review Checklist
Purpose: a lightweight everyday counterpart to `05_git_workflow.md`'s review gate, which only fires for specific high-risk triggers.

Checklist to run before marking any task done, regardless of size:
- No leftover debug `Serial.print`/commented-out code from the working process.
- No hardcoded pins, colors, or magic timing constants outside `config.h`/`theme.h`.
- Every `TODO` added has a scope + description (`13_code_style.md`'s format), or is a `04_known_issues.md` entry instead.
- `10_symbol_reference.md` regenerated if any public function/struct/macro changed shape.
- `04_known_issues.md` changelog updated (`07_task_protocol.md` step 12).
- Any `.agents/rules/` file that should have changed per `03_conventions.md` section 11's Documentation Maintenance Protocol actually did.
- Compile re-run after the last edit — not just "it compiled before this last tweak."
- Final report uses the two-header format with nothing blurred between "verified" and "waiting on you."

This checklist is self-review, not a second reviewer — it doesn't replace the explicit human-approval gates already required elsewhere. It covers everything below that bar, which currently has no checklist at all.

---

<!-- Section: 23_incident_postmortem_log.md -->

# Incident / Process-Failure Log
Purpose: Make "write a rule after every real incident" a repeatable step instead of something that happens to occur. This log is for process/judgment failures specifically. Ordinary code bugs belong in `04_known_issues.md`'s Technical Debt log instead.

## Format
One entry per incident, append-only:
```
## INC-<n>: <one-line title> (<date>)
**What happened:** <factual, one paragraph, no editorializing>
**Impact:** <what it caused or nearly caused>
**Root cause:** <the actual mechanism, not just "agent error">
**Prevention rule added:** <file + section reference — don't restate the rule text, per 11_rules_meta.md>
```

## When to Log
- Any future incident where an agent reported something false as true, skipped verification, or nearly caused a hard-stop violation gets an entry here **before the session that caused it ends**, not "later" — the same way a checkpoint commit happens before risky work, not after.

## Log

## INC-1: Hallucinated Bug Report (2026-08)
**What happened:** A fully hallucinated bug report (fabricated function names, presented with confident specific detail) was filed regarding syntax errors in dead branches (e.g. CALL C).
**Impact:** Time and trust lost investigating a non-existent bug (later DEBUNKED).
**Root cause:** An agent reported an artifact from a flawed grep check as a verified syntax error, rounding uncertain evidence up to a confident claim without reading the literal file.
**Prevention rule added:** `06_verification_standards.md` -> "The core rule" and "Quoting code".

## INC-2: Partial Test Result Reported as Full Pass (2026-08)
**What happened:** A partial test result (9/11 sub-tests reached their :ok marker, but the suite did not finish) was reported as a full pass with celebratory language.
**Impact:** A broken configuration (the host test harness terminating prematurely due to a budget limit) was marked as verified and correct.
**Root cause:** An agent reasoned past a resource limit (frame budget) instead of fixing it, treating an incomplete run as a success and failing to wait for the designed success signal (`Passed all tests`).
**Prevention rule added:** `06_verification_standards.md` -> "Partial results are not passes" and "Banned language in status reports".

## INC-3: Bypassing Performance Warnings for Flag Activation (2026-08)
**What happened:** A previous agent enabled `WALNUT_GB_16_BIT_OPS_DUALFETCH` and `WALNUT_GB_16_BIT_OPS` in `walnut_cgb.h` as part of a performance optimization task. The upstream author's comment directly above those defines reads "currently breaks compatibility with some games, 16-bit opcode optimization needs revisions." The agent enabled them, ran `cpu_instrs.gb` on the host test harness (which passed), and committed. Super Mario Bros. Deluxe (GBC) subsequently froze on hardware when starting a new game or loading a save — a code path requiring MBC5 bank switching into upper ROM banks.
**Impact:** The only baked GBC game stopped working on real hardware. The freeze was silent from the device's perspective (no `gb_error` callback fired, no restart) — the game simply locked up.
**Root cause:** Two compounding failures. (1) The agent read the blocking warning comment and overrode it without hardware verification or user approval. (2) The agent used `cpu_instrs.gb` passing as evidence of compatibility -- but that suite only tests individual opcodes, not bank switching or game-specific initialization sequences. The distinction between "CPU instructions correct" and "this game runs" was not captured anywhere in the rules.
**Prevention rule added:** `24_vendor_flag_safety.md` (new file) -- mandatory read-before-enable protocol for vendor flags. `25_game_compatibility_ledger.md` (new file) -- game-level test ledger so a future agent can see "Mario: BROKEN with these flags" before touching them.

---

<!-- Section: 24_vendor_flag_safety.md -->

# Vendor Flag Safety
Purpose: INC-3 (2026-08-30) proved that enabling a vendor `#define` flag
without reading its own warning comment breaks real games silently.
This file makes the read-before-enable step mandatory and non-skippable.

## The incident this rule prevents
`WALNUT_GB_16_BIT_OPS_DUALFETCH` and `WALNUT_GB_16_BIT_OPS` were flipped
from `0` to `1` by a previous agent to activate a performance fast path.
The comment directly above those defines, written by the upstream author,
reads: "currently breaks compatibility with some games, 16-bit opcode
optimization needs revisions." The agent enabled them anyway and committed.
Result: Super Mario Bros. Deluxe (GBC) froze on new-game/load, which
exercises MBC5 bank switching paths the fast paths mishandle.

## Mandatory read-before-enable protocol
Before changing any `#define` flag from `0` to `1` (or adding one) inside
**any file under `src/vendor/` or `src/engine/`**:

1. **Read the comment block immediately above the `#define`.** "Immediately
   above" means contiguous -- no blank line gap between comment and define.
2. **Read the comment block immediately inside any `#if <FLAG>` block** that
   flag controls (e.g., the body of `#if WALNUT_GB_16_BIT_OPS`).
3. **Search the file** for any // TODO, // FIXME, // NOTE, // WARNING,
   // BROKEN, // EXPERIMENTAL, // may, // breaks, // limited near
   the flag name. Grep for the flag name if the file is large.
4. **Quote the warning verbatim** in your commit message body if any such
   language exists. Do not paraphrase. The exact words are what a future
   agent will grep for.
5. If the comment contains any compatibility or correctness warning, the flag
   is **BLOCKED from being enabled** until either:
   - A hardware-verified game compatibility test (VERIFIED_HARDWARE) covers
     the specific code path the flag activates, OR
   - The user explicitly approves the risk in writing (quote their exact
     message in the commit body).

## What "compatibility warning" means
Any of the following phrases in the comment above the flag counts as a
blocking warning: "breaks", "compatibility", "some games", "may not work",
"experimental", "needs revisions", "limited to", "TODO", "FIXME",
"not yet", "only on", "unsafe", "can break".
This list is not exhaustive -- use judgment. If the upstream author was
hedging, treat it as a warning.

## CPU-test pass != game compatibility
A passing `cpu_instrs.gb` run (or any Blargg CPU test suite) only verifies
that individual opcodes execute with correct register state and timing.
It does **not** verify:
- MBC bank switching sequences (MBC1/3/5 state machine transitions)
- DMA transfer correctness during active HDMA
- Timing-dependent interrupt edge cases
- GBC-specific WRAM banking
- Any game-specific initialization sequence

Enabling a vendor flag and then running `cpu_instrs.gb` is insufficient
evidence to promote a change above FIXED_UNVERIFIED. State which
**named commercial or homebrew game** you ran, on real hardware, to
game-specific known-good states (see `25_game_compatibility_ledger.md`).

## Turning a flag off (revert path)
If a game breaks and the suspected cause is a recently-enabled vendor flag:
1. Revert that flag first (one-line change), commit, re-flash, confirm.
2. Do not combine the revert with other changes -- it must be isolatable.
3. Add the game and the broken-flag combination to `25_game_compatibility_ledger.md`.
4. Log the incident in `23_incident_postmortem_log.md`.

## src/engine/walnut_cgb/ specific flags
Current known-dangerous flags and their safe defaults:

| Flag | Safe default | Warning quoted from source |
|---|---|---|
| `WALNUT_GB_16_BIT_OPS_DUALFETCH` | `0` | "currently breaks compatibility with some games, 16-bit opcode optimization needs revisions" |
| `WALNUT_GB_16_BIT_OPS` | `0` | "this can break compatibility with some games and is disabled by default" |
| `WALNUT_GB_SAFE_DUALFETCH_OPCODES` | `0` | "used for debugging invalidated opcodes or compatibility but slows execution" |
| `WALNUT_GB_SAFE_DUALFETCH_DMA` | `0` | same as above |
| `WALNUT_GB_SAFE_DUALFETCH_MBC` | `0` | same as above |

**Do not change these without an entry in `25_game_compatibility_ledger.md`
first showing the affected game under both flag states.**

---

<!-- Section: 25_game_compatibility_ledger.md -->

# Game Compatibility Ledger
Purpose: agents should never have to re-discover whether a game works --
the answer must be in this file. Specifically, this ledger records what
hardware flags, emulator revisions, and engine settings were active when
a game was last tested, so future agents know if a change is a regression.

## Maintenance rule
- Add a row for EVERY game that is tested on real hardware, even partial runs.
- Update the row (do not add a duplicate) when a game is re-tested after a change.
- Never remove rows -- only update Status and Notes.
- If you break a previously-working game: change its Status to BROKEN,
  add a note describing what changed, and log an INC in 23_incident_postmortem_log.md.

## Status vocabulary (same as 06_verification_standards.md, adapted)
- `WORKS` -- ran to a stable known-good gameplay state, defined below.
- `PARTIAL` -- boots/title screen shown but freezes, glitches, or crashes before
  reaching a stable gameplay state.
- `BROKEN` -- does not boot or immediately crashes/reboots device.
- `UNTESTED` -- exists in repo or on SD card but never run on real hardware.

## Known-good states (PASS bar per game)
These definitions are the PASS bar for VERIFIED_HARDWARE status.
A game may not be listed as WORKS unless it reached its defined state.

| Game | Console | PASS definition |
|---|---|---|
| Super Mario Bros. Deluxe | GBC | Title screen loads; "New Game" selected; gameplay starts; Mario walks at least 10 steps without freeze |
| Legend of Zelda: Oracle of Ages | GBC | Title screen loads; "New Game" selected; intro cutscene plays |
| Aladdin | GBC | Title screen loads; gameplay starts; at least 5 seconds of movement |
| Lego Racers | GBC | Title screen loads; menu navigation responds |
| Tobu Tobu Girl | GB (baked) | Title screen loads; gameplay starts |

For any game not listed above, define the PASS bar before calling it WORKS.
Document the definition in this file before the hardware test, not after.

## Ledger

| Game | Console | Emulator | Firmware commit | Walnut flags | Status | Last tested | Notes |
|---|---|---|---|---|---|---|---|
| Super Mario Bros. Deluxe (Baked) | GBC | WalnutEmu | 3c53275 (logging refactor) | 16BIT_OPS=1, DUALFETCH=1 | BROKEN | 2026-08-30 | Froze on new-game/load. INC-3. Flags reverted to 0 in 5d78e8b. |
| Super Mario Bros. Deluxe (Baked) | GBC | WalnutEmu | 5d78e8b (flag revert) | 16BIT_OPS=0, DUALFETCH=0 | UNTESTED | -- | Flags reverted; needs hardware re-flash to confirm fix. |
| Legend of Zelda: Oracle of Ages (Baked) | GBC | WalnutEmu | -- | -- | UNTESTED | -- | Baked ROM present, never hardware-tested |
| Aladdin (Baked) | GBC | WalnutEmu | -- | -- | UNTESTED | -- | Baked ROM registered in sd_card.cpp; compiles cleanly |
| Lego Racers (Baked) | GBC | WalnutEmu | -- | -- | UNTESTED | -- | Baked ROM registered in sd_card.cpp; compiles cleanly |

## How to update this file
When you flash and test a game:
1. Record the exact `git describe --tags --always` output as "Firmware commit".
2. Record which walnut flags were non-default (0->1 or 1->0).
3. Record the date as YYYY-MM-DD.
4. Set Status to WORKS / PARTIAL / BROKEN.
5. Add a Notes column entry if anything was abnormal.

---

<!-- Section: 26_emulator_exit_contract.md -->

# Emulator Exit Contract
Purpose: The return-to-menu path in BmoGameboy.ino has a documented gap
(12_extensibility_contract.md "Emulator teardown on switch") -- currently
WalnutEmu::destroy() and PeanutEmu::destroy() are NOT called on SELECT+UP.
This file tracks that gap, formalizes what teardown must do, and prevents
agents from silently adding more violations.

## Current state (verified 2026-08-30)
In BmoGameboy.ino SELECT+UP handler:
- WalnutEmu::destroy() IS called when selectedEmulatorIndex == 0. (FIXED_UNVERIFIED)
- PeanutEmu::destroy() IS called when selectedEmulatorIndex == 1. (FIXED_UNVERIFIED)
- NesEmu::destroy() IS called when selectedEmulatorIndex == 2. OK.
- DoomEmu::destroy() IS called when selectedEmulatorIndex == 3. OK.

## What every emulator destroy() must do
When called, a core's destroy() function must:
1. Free every PSRAM buffer it allocated at begin() time.
2. Set the freed pointer(s) back to nullptr so begin() can re-initialize cleanly.
3. NOT touch the ROM buffer -- that is owned by the caller (BmoGameboy.ino)
   and freed via SDCard::freeRom() after destroy() returns.
4. NOT crash if called multiple times (idempotent null-pointer guards required).

## The dispatch pattern (Active in BmoGameboy.ino)
BmoGameboy.ino SELECT+UP handler calls the outgoing emulator's destroy()
for ALL four emulator indices:
```cpp
if (selectedEmulatorIndex == 0) {
  WalnutEmu::destroy();
} else if (selectedEmulatorIndex == 1) {
  PeanutEmu::destroy();
} else if (selectedEmulatorIndex == 2) {
  NesEmu::destroy();
} else if (selectedEmulatorIndex == 3) {
  DoomEmu::destroy();
}
```
Status: FIXED_UNVERIFIED (hardware verification required to confirm cart_ram recovery on physical device).

## Why this matters
The ESP32-S3-N16R8 has 8MB of PSRAM. WalnutEmu allocates 128KB for cart_ram.
PeanutEmu has similar allocations. Repeated play sessions without freeing
will slowly fragment the PSRAM heap. On this hardware the heap allocator
does NOT compact -- fragmentation is permanent until reset. A full PSRAM
fragmentation event will cause the next heap_caps_malloc (e.g. ROM load,
DOOM WAD load) to fail silently with a null pointer, which then triggers
either a crash or a blank screen. This is a silent, session-count-dependent
failure -- exactly the kind that is hard to reproduce and diagnose.

## Rule: do not add new destroy() gaps
If you add a new emulator core (see 12_extensibility_contract.md), its
destroy() must be wired into the SELECT+UP handler in the same commit.
Never leave destroy() wired for some cores but not others.

---

<!-- Section: 27_codebase_map.md -->

# Codebase Map — Ground Truth for Architecture
Purpose: an agent starting a new session should be able to read this ONE
file and understand the whole system before touching anything. Updated
whenever structure changes.

Last updated: 2026-08-30 (Antigravity)

---

## Directory tree (verified, current)
```
repo-root/
├── AGENTS.md                      <- entry point, points here (Ruleset v4)
├── README.md                      <- user-facing project summary
├── AGENT_MANIFEST.json            <- machine-readable hardware & build manifest
├── IMPLEMENTATION_PLAN.md         <- historical plan, status=COMPLETE, safe to ignore
├── partitions.csv                 <- WARNING: see Flash Partitions below
├── .agents/rules/                 <- agent ruleset (34 rules + CONTEXT_INDEX.json)
├── docs/
│   ├── software-design-document.md  <- Living architectural specification (v3.0 Ground Truth)
│   └── hardware-notes.md
├── firmware/BmoGameboy/
│   ├── BmoGameboy.ino             <- ONLY setup(), loop(), state machine
│   ├── partitions.csv             <- custom partition table (8MB app0)
│   └── src/
│       ├── core/                  <- hardware drivers, always compiled
│       │   ├── config.h           <- single source of truth for all pins + FEATURE flags
│       │   ├── display_emu.cpp/h  <- ST7789 SPI driver + all render APIs
│       │   ├── buttons.cpp/h      <- GPIO polling, joypad bitmask
│       │   ├── sd_card.cpp/h      <- SD + baked ROM registration
│       │   ├── bmo_face.cpp/h     <- procedural SDF mascot renderer
│       │   ├── battery.cpp/h      <- DORMANT (FEATURE_BATTERY_MONITOR=0)
│       │   ├── audio_i2s.cpp/h    <- DORMANT (FEATURE_AUDIO=0)
│       │   └── fram_save.cpp/h    <- DORMANT (not wired, no FEATURE flag yet)
│       ├── emulators/             <- thin glue wrappers, one per console
│       │   ├── emu_peanut.cpp/h   <- GB (.gb)   -> peanut_gb engine
│       │   ├── emu_walnut.cpp/h   <- GBC (.gbc) -> walnut_cgb engine
│       │   ├── emu_nes.cpp/h      <- NES (.nes)  -> agnes engine
│       │   └── emu_doom.cpp/h     <- DOOM (.wad) -> doomgeneric engine
│       ├── engine/
│       │   └── walnut_cgb/
│       │       └── walnut_cgb.h   <- GBC engine (9937 lines, header-only)
│       ├── vendor/
│       │   ├── peanut_gb/         <- peanut_gb.h + peanut_gb_config.h
│       │   ├── agnes/             <- agnes.h + agnes.c (NES)
│       │   └── doom/              <- doomgeneric (DOOM)
│       ├── assets/
│       │   ├── bmo_assets.h       <- extern declarations for face bitmaps
│       │   ├── rom_data.h         <- legacy single-ROM header (tobu_tobu_girl), UNUSED in live build
│       │   └── roms/
│       │       ├── mario_deluxe.h   <- 1MB GBC ROM baked as C array
│       │       ├── zelda_ages.h     <- 1MB GBC ROM baked as C array
│       │       ├── aladdin.h        <- 1MB GBC ROM baked as C array
│       │       └── lego_racers.h    <- 1MB GBC ROM baked as C array
│       └── tests/
│           └── unit_tests.cpp     <- on-device test suite (requires ENABLE_UNIT_TESTS)
├── tools/
│   ├── host_test.cpp              <- desktop CPU-correctness harness (Zig compiler)
│   └── convert.py                 <- ROM-to-C-header converter
└── scripts/
    ├── process_games.py           <- ROM validation + C header generation
    ├── validate_repo.py           <- Python syntax check + ROM integrity
    └── (others)                   <- benchmark, color calc, fetch covers
```

---

## The state machine (BmoGameboy.ino)
Three states. THIS is the entire runtime control flow:
```
STATE_CONSOLE_MENU  -> user picks console (GB/GBC/NES/DOOM)
      |
      | A button
      v
STATE_GAME_MENU     -> user picks ROM file
      |
      | A button
      v
STATE_EMULATOR      -> one emulator runs, SELECT+UP exits back to CONSOLE_MENU
```
**KEY FACT:** `BmoFace::update()` and `Battery::update()` run every loop() tick
regardless of state. Emulator cores do NOT call them — only menu states do.

---

## Emulator routing (sd_card.h RomType enum -> BmoGameboy.ino)
| Extension | RomType | selectedEmulatorIndex | Engine | destroy() called on exit? |
|---|---|---|---|---|
| `.gb`  | ROM_GB  | 1 | PeanutEmu (peanut_gb.h) | YES (wired in BmoGameboy.ino) |
| `.gbc` | ROM_GBC | 0 | WalnutEmu (walnut_cgb.h) | YES (wired in BmoGameboy.ino) |
| `.nes` | ROM_NES | 2 | NesEmu (agnes) | YES |
| `.wad` | ROM_WAD | 3 | DoomEmu (doomgeneric) | YES (no-op) |

DOOM does NOT load ROM into PSRAM — it reads WAD directly from SD via fopen().
All others load full ROM into PSRAM via `SDCard::loadRom()`.

---

## Baked ROMs (always available, no SD card needed)
Registered in `SDCard::begin()` and `SDCard::loadRom()` in sd_card.cpp.
| Name in romList | Header | Size | Registered? |
|---|---|---|---|
| Super Mario Bros Deluxe (Baked).gbc | mario_deluxe.h | 1MB | YES |
| Legend of Zelda Ages (Baked).gbc | zelda_ages.h | 1MB | YES |
| Aladdin (Baked).gbc | aladdin.h | 1MB | YES |
| Lego Racers (Baked).gbc | lego_racers.h | 1MB | YES |

Note: Header files are ~6MB of formatted C text on disk, but each compiles
to exactly 1,048,576 bytes (1MB) in flash .rodata. All 4 baked ROMs consume
~4.98MB total firmware binary space, well within the 8MB app0 partition limit.

---

## Flash partition layout (partitions.csv)
| Name | Type | Offset | Size | Purpose |
|---|---|---|---|---|
| nvs | data/nvs | 0x9000 | 20KB | NVS storage |
| otadata | data/ota | 0xE000 | 8KB | OTA metadata |
| app0 | app/ota_0 | 0x10000 | **8MB** | Firmware + baked ROMs |
| ffat | data/fat | 0x810000 | ~7.9MB | FatFS (unused currently) |

The 8MB app0 is NON-STANDARD. The standard Arduino ESP32 partition schemes
use 1.3MB or 1.9MB for app. This custom partition is required because the
baked ROM headers (mario, zelda) alone consume ~10MB of raw C source which
compiles down to ~2MB of .rodata in flash. If the custom partitions.csv is
lost or overwritten, the firmware will not fit and will fail to flash.

---

## SPI bus sharing (critical — do not add new SPI devices without reading this)
The TFT display and SD card SHARE the SPI bus:
- SCK: GPIO12, MOSI: GPIO11 (shared)
- TFT CS: GPIO10, SD CS: GPIO13, MISO: GPIO15 (SD only)
- Bus initialized once in setup() via `SPI.begin(TFT_SCK, SD_MISO, TFT_MOSI, -1)`
- SD card uses a slower 4MHz init transaction; display runs at 80MHz during frames
- startFrame()/endFrame() hold the SPI bus open for an entire 144-scanline frame
  -- NO SD card reads can happen during an active frame render
- DOOM is the only engine that calls fread() during gameplay; it holds the SPI bus
  between DG_DrawFrame() calls; this is why DOOM latency spikes appear in the log

---

## Display coordinate system
- Physical display: 320px wide × 240px tall (landscape, rotation=3)
- Game Boy viewport: 240×216, centered at OFFSET_X=40, OFFSET_Y=12
- NES viewport: 256×240, centered at x=32 (full height)
- DOOM viewport: 320×200, y-offset=20 (20px letterbox top/bottom)
- Full-screen blit (BMO face, menus): 0,0 to 320,240
- Pixel format ON THE WIRE: BGR565 byte-swapped (big-endian on wire)
  -- `SPI.writeBytes()` sends LSB first; ST7789 expects MSB first per byte
  -- All emulator palettes must be pre-swapped; use the swapBytes() helper

---

## Buttons joypad bitmask (gb_joypad_state)
Active-low: 0=pressed, 1=released (same as Game Boy hardware).
| Bit | Button | Config pin |
|---|---|---|
| 0 | A | GPIO16 |
| 1 | B | GPIO17 |
| 2 | SELECT | GPIO21 |
| 3 | START | GPIO18 |
| 4 | RIGHT | GPIO7 |
| 5 | LEFT | GPIO6 |
| 6 | UP | GPIO4 |
| 7 | DOWN | GPIO5 |

Buttons::update() reads ALL button GPIO with a single REG_READ(GPIO_IN_REG)
call (all button pins are < GPIO32). Do not add buttons on GPIO >= 32 without
changing the read logic.

---

## PSRAM allocation map (current, approximate worst case)
| Allocator | Size | Notes |
|---|---|---|
| WalnutEmu cart_ram | 128KB | lazy-allocated on first begin(), freed on destroy() |
| PeanutEmu cart_ram | 128KB | same pattern |
| DisplayEmu menuCanvas | 320×240×2 = 150KB | freed on cleanupMenuUI() |
| SDCard ROM buffer | up to 4MB | freed on SDCard::freeRom() |
| DOOM heap | large | internally managed by doomgeneric |
| Total worst case | ~4.5MB+ | 8MB ceiling; fragmentation possible |

---

## Known dormant/aspirational features (do NOT enable without physical hardware)
| Feature | Flag | Why dormant |
|---|---|---|
| Battery monitor | FEATURE_BATTERY_MONITOR=0 | No voltage divider soldered |
| I2S audio | FEATURE_AUDIO=0 | No MAX98357A DAC wired |
| FRAM save | (no flag yet) | No I2C FRAM wired |

---

<!-- Section: 28_display_and_spi_contract.md -->

# Display & SPI Bus Contract
Purpose: the display/SPI subsystem has several non-obvious rules that,
if violated, either produce silently wrong colors or corrupt the SPI bus.
This file is the single reference so agents do not have to reverse-engineer
display_emu.cpp from scratch.

## Pixel format — THE most common source of color bugs
The ST7789 on this hardware is wired in BGR mode. The MADCTL register is
manually written at boot (0xA0 | 0x08 = BGR bit set). This means:

- **Emulator palettes** must be in **BGR565 byte-swapped** format.
  "Byte-swapped" means the two bytes of each uint16_t are reversed relative
  to what you'd get from a standard `(r << 11) | (g << 5) | b` formula.
- **The correct formula for a palette entry:**
  ```c
  uint16_t bgr565 = ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
  uint16_t wire   = (bgr565 >> 8) | (bgr565 << 8); // byte-swap for SPI
  ```
- **Menu/UI pixels** go through a GFXcanvas16 which uses a different path
  (writeBytes) -- see uiColor() in display_emu.cpp for the correct helper.
- If colors are wrong (red/blue swapped, or blue sky turns orange):
  check whether the palette was built with BGR or RGB, and whether the
  byte swap was applied. Do NOT touch the MADCTL register to fix colors --
  fix the palette instead.

## startFrame / endFrame contract
```
DisplayEmu::startFrame();     // asserts SPI CS, calls setAddrWindow ONCE
  // Inside here: lcd_draw_line fires 144 times, each calls:
  //   DisplayEmu::streamPixelRow(rowBuffer, 240 or 480);
DisplayEmu::endFrame();       // releases CS
```
**Rules:**
- startFrame() must ALWAYS be paired with endFrame(). Missing endFrame()
  leaves the SPI bus locked -- subsequent SD card operations will hang.
- streamPixelRow() must ONLY be called between startFrame/endFrame.
  Calling it outside that context is undefined behavior on the ST7789.
- The address window is set for the GB viewport (240x216 at OFFSET_X=40,
  OFFSET_Y=12). Streaming more than 240*216*2 bytes will overrun the window.

## NES and DOOM render paths
- NES: streamNESFrame() is self-contained (startWrite/endWrite internally).
  It does NOT use startFrame/endFrame. Do not mix the two.
- DOOM: streamDoomFrame() is also self-contained. DOOM interleaves SD reads
  between frames; do not call startFrame before doomgeneric_Tick().

## SPI bus sharing rules
- The SD card and TFT share SCK (GPIO12) and MOSI (GPIO11).
- They have SEPARATE CS pins: TFT=GPIO10, SD=GPIO13.
- The SD card uses MISO (GPIO15); the TFT does not use MISO.
- The Arduino SPI library manages CS via the transaction API.
  Never manually toggle CS pins outside startWrite/endWrite or
  SPISettings transactions -- this will corrupt in-flight data.
- SD card reads are BLOCKED during an active startFrame()/endFrame() window.
  Do not call SDCard::loadRom() or any SD operation while a frame is rendering.

## Adding a new display region or blitting API
Any new blit function must:
1. Use one of: pushPixelsAt(), pushPixelsFullScreen(), pushPixels(),
   or be a new self-contained startWrite/setAddrWindow/writeBytes/endWrite.
2. Never call setAddrWindow() inside startFrame/endFrame (that context
   already has an address window set -- a second setAddrWindow will corrupt).
3. State the pixel format explicitly in the function comment (BGR565 byteswap?
   RGB565? raw index?).
4. Not exceed 320x240 total pixels -- the display is exactly that size.

## Scaling reference (nearest-neighbor, verified)
| Source | Target | Method |
|---|---|---|
| GB 160x144 | 240x216 | 1.5x: 4 source pixels -> 6 output (A A B C C D); even rows doubled |
| NES 256x240 | 256x240 at x=32 | 1:1 center crop (left/right letterbox) |
| DOOM 320x200 | 320x200 at y=20 | 1:1 center (top/bottom letterbox) |
| BMO face FB 128x128 | variable | bilinear or nearest in bmo_face.cpp |

---

<!-- Section: 29_adding_a_baked_rom.md -->

# How to Add a Baked ROM
Purpose: adding a baked ROM header to sd_card.cpp is the most common
"simple" task that has several non-obvious landmines. This checklist
prevents each of them.

## What "baked ROM" means
A ROM baked as a C array in a `.h` file under `src/assets/roms/`.
The firmware can serve these WITHOUT an SD card. The ROM data lives in
the ESP32-S3 flash .rodata section.

## Pre-flight checklist (do all before touching sd_card.cpp)
This budget check is now a standing invariant, not a one-time step — see `37_rom_governance_and_flash_budget.md`.

1. **Check flash budget.** Run the arduino-cli build and inspect the
   binary size. The `app0` partition is exactly 8MB (8,388,608 bytes).
   The binary with 4 baked 1MB ROMs (mario, zelda, aladdin, lego_racers) is ~4.98MB.
   Each 1MB ROM adds ~1MB to the binary (do not confuse the ~6MB C source text size
   on disk with the actual 1MB compiled binary .rodata array size).
   Formula: `current_binary_size + new_rom_size_bytes < 8,388,608`
   If this does not hold, DO NOT proceed -- the flash will fail silently.

2. **Verify the ROM header.** A valid GBC/GB ROM has:
   - Nintendo logo at 0x0104-0x0133 (specific byte pattern)
   - Header checksum at 0x014D (computed as: x=0; for j in 0x134..0x14C: x=x-rom[j]-1; x&0xFF)
   - CGB flag at 0x0143: 0xC0 or 0x80 = GBC, anything else = DMG only
   Use the Python check: `python -c "import re; data=bytes([int(x,16) for x in re.findall(r'0x([0-9a-fA-F]{2})', open('foo.h').read())]); print(hex(data[0x143]), hex(data[0x14D])); cs=0; [cs:=cs-data[j]-1 for j in range(0x134,0x14D)]; print('ok' if (cs&0xFF)==data[0x14D] else 'CHECKSUM FAIL')"`

3. **Confirm the rom_size variable.** The .h file must define both:
   - `const uint8_t <name>_rom[] = {...};` (the data array)
   - `const size_t <name>_rom_size = <N>;` (byte count matching the array)
   Both must be present. sd_card.cpp uses the `_rom_size` variable.

4. **Confirm the RomType.** GBC roms (.gbc) use ROM_GBC -> WalnutEmu.
   DMG-only roms (.gb) use ROM_GB -> PeanutEmu. Routing the wrong way
   will not compile-error but will produce wrong colors or crashes.

## Steps to register the ROM
After pre-flight passes, make these three changes in sd_card.cpp:

### 1. Add the include (top of file)
```cpp
#include "../assets/roms/<name>.h"
```

### 2. Register in SDCard::begin() (baked ROM block)
```cpp
strncpy(romList[numRoms].filename, "<Display Name> (Baked).<ext>", 63);
romList[numRoms].type = ROM_GBC;  // or ROM_GB
numRoms++;
```
Keep baked ROMs FIRST in the list (before SD scan). They are always available.

### 3. Add to SDCard::loadRom() (baked ROM dispatch)
```cpp
if (strcmp(filename, "<Display Name> (Baked).<ext>") == 0) {
    *outSize = <name>_rom_size;
    return (uint8_t*)<name>_rom;
}
```

### 4. Add to SDCard::freeRom() (protect from free())
```cpp
if (buffer == <name>_rom /* || buffer == other_baked_rom */) {
    return;  // flash .rodata -- never free
}
```
**CRITICAL:** forgetting this causes a crash when the user exits the game.
`heap_caps_free()` on a .rodata pointer is undefined behavior on ESP32-S3.

## After registration
- Build and check binary size fits in app0 (step 1 above).
- Add the ROM to `25_game_compatibility_ledger.md` with status UNTESTED.
- Update `04_known_issues.md` changelog.

## What NOT to do
- Do NOT #include the ROM header anywhere except sd_card.cpp. The array
  is declared without `static`, so including it in multiple TUs causes
  an ODR violation (multiply-defined symbol at link time).
- Do NOT add ROMs that exceed the flash budget without expanding partitions.csv.
- Do NOT change the partitions.csv without verifying OTA and ffat behavior.

---

<!-- Section: 30_common_agent_mistakes.md -->

# Common Agent Mistakes (Anti-Pattern Catalogue)
Purpose: a catalogue of mistakes that have already happened or are highly
predictable given the architecture. An agent reads this ONCE and gets
the institutional memory of every prior failure without having to re-derive it.
Append entries here after every INC in 23_incident_postmortem_log.md.

---

## M-1: Enabling a vendor flag that says "breaks compatibility"
**What happens:** Agent sees a performance flag set to 0, enables it,
runs cpu_instrs.gb which passes, commits. Game freezes on hardware.
**Root cause documented in:** INC-3, 24_vendor_flag_safety.md
**Prevention:** Read the comment above every vendor #define before touching it.
The words "breaks", "some games", "needs revisions" = STOP.

---

## M-2: Trusting 10_symbol_reference.md without checking staleness
**What happens:** Agent states "the function is called X" based on this
file. The file was last updated weeks ago. The function was renamed.
**Prevention:** Run `git log --oneline -1 -- <source_file>` before citing
this file. If the source file is newer than the staleness date in
10_symbol_reference.md, grep live instead.

---

## M-3: Calling SDCard::freeRom() on a baked ROM pointer
**What happens:** Code path exits game and calls SDCard::freeRom(romData).
If romData points to .rodata (baked ROM), heap_caps_free() is UB -- crash.
**Prevention:** freeRom() already guards against the two registered baked
ROMs. If you ADD a new baked ROM, you MUST add it to freeRom()'s guard.
See 29_adding_a_baked_rom.md step 4.

---

## M-4: Including a baked ROM header in more than one .cpp file
**What happens:** The ROM array has external linkage (no `static` keyword).
Including it in two TUs = multiply-defined symbol = link error.
**Prevention:** Baked ROM headers are ONLY included in sd_card.cpp.

---

## M-5: Adding a SPI device without reading the bus sharing rules
**What happens:** New device added on the same SPI bus without proper CS
management. Display frame renders corrupt, or SD reads return garbage.
**Prevention:** Read 28_display_and_spi_contract.md before touching SPI.

---

## M-6: Calling Buttons::update() more than once per loop() iteration
**What happens:** Two update() calls per loop means `changed` flags fire
twice for every physical press. Menu double-advances, emulator gets
duplicate input. INC documented in 04_known_issues.md (Doom double-polling).
**Prevention:** Buttons::update() appears ONCE per loop() execution path.
The emu path does: update() -> check SELECT+UP -> run emulator frame.
The menu path does: update() at top of menu state. Never both.

---

## M-7: Drawing to the display during STATE_EMULATOR
**What happens:** BmoFace::draw() called mid-frame overwrites the SPI
address window that startFrame() set, producing screen tearing or
corrupting the ongoing frame transfer.
**Prevention:** BmoFace::draw() and BmoFace::update() are NEVER called
during STATE_EMULATOR. This is enforced by the loop() structure --
do not move those calls inside the emulator branch. See also `35_bmo_face_contract.md` for the full mascot subsystem contract.

---

## M-8: Setting FEATURE_BATTERY_MONITOR=1 or FEATURE_AUDIO=1
**What happens:** GPIO1 floating ADC causes unstable boot or reading loop.
I2S tries to clock out a nonexistent DAC. Both are hard-stop violations.
**Prevention:** See 00_hard_stops.md. These flags are NEVER enabled until
physical hardware is confirmed soldered (01_hardware.md).

---

## M-9: Reporting VERIFIED_HARDWARE without a git tag
**What happens:** Status is promoted to VERIFIED_HARDWARE verbally in a
commit message or comment, but no hardware-flash tag is created.
Future agents cannot find when or at what commit this was confirmed.
**Prevention:** See 05_git_workflow.md. VERIFIED_HARDWARE requires a git
tag created at the same commit.

---

## M-10: Changing BmoGameboy.ino routing without updating 27_codebase_map.md
**What happens:** The emulator routing table in the .ino changes (new
emulator, new index) but 27_codebase_map.md and 10_symbol_reference.md
are not updated. Next agent reads the wrong routing.
**Prevention:** Any change to CONSOLES[], selectedEmulatorIndex values,
or the SELECT+UP exit handler MUST update 27_codebase_map.md routing table.

---

## M-11: Confusing C-array source header size with flash binary size
**What happens:** Agent sees a 6MB `.h` file on disk and assumes it takes 6MB
of flash. In reality, each byte formatted as `0xXX, ` takes 6 bytes of ASCII text,
so a 1MB ROM produces a 6MB `.h` file, but compiles to exactly 1MB in flash `.rodata`.
**Prevention:** Always verify the compiled binary size with `arduino-cli` rather than
estimating from the header text size on disk. See `29_adding_a_baked_rom.md`.

---

## M-12: Putting game logic or emulator calls inside BmoGameboy.ino
**What happens:** BmoGameboy.ino becomes a megafile that's hard to test,
review, or modify without touching the state machine.
**Prevention:** BmoGameboy.ino contains ONLY: setup(), loop(), state
machine dispatch, and frame timing. All emulator logic stays in src/.

---

## M-13: Using Serial.print directly in hot loops
**What happens:** Serial.print is blocking on ESP32-S3. At 115200 baud,
a single println() can stall the loop for 0.5-1ms. Inside a 16.7ms
frame budget, even one call per scanline breaks the timing.
**Prevention:** Only LOG_INFO/LOG_DEBUG macros. These are gated by LOG_LEVEL
in config.h. The hot paths (lcd_draw_line, gb_rom_read) have zero log calls.

---

## M-14: Confusing the "engine" and "vendor" directories
**What happens:** walnut_cgb.h lives in src/engine/, not src/vendor/.
peanut_gb.h lives in src/vendor/. agnes.c lives in src/vendor/. doom lives in src/vendor/.
An agent looking for walnut_cgb.h in src/vendor/ will not find it.
**Prevention:** See 27_codebase_map.md directory tree. Or just grep.

---

## M-15: Not calling WalnutEmu::destroy() or PeanutEmu::destroy() on exit
**What happens:** cart_ram (128KB each) stays allocated in PSRAM.
Repeated play sessions fragment PSRAM until a future allocation fails.
**Prevention:** See 26_emulator_exit_contract.md. Follow the universal
teardown contract in BmoGameboy.ino SELECT+UP handler.

---

## M-16: Assuming the destroy() PSRAM gap is still open after the 2026-08-30 fix
**What happens:** Agent reads an older document or prompt mentioning that Walnut/Peanut
destroy() is not wired, assumes it is still missing, and attempts to re-add it or files a false bug.
**Prevention:** Check BmoGameboy.ino SELECT+UP handler live in THIS session before claiming
a teardown gap exists. All four cores (Walnut, Peanut, NES, DOOM) are wired to call destroy().

---

## M-17: Using raw unaligned pointer casts on Flash `.rodata`
**What happens:** Direct pointer casts like `*(uint16_t*)&rom[addr]` on Flash memory
can trigger unaligned memory access exceptions on Xtensa LX7 cores.
**Prevention:** Always reconstruct 16-bit and 32-bit words byte-by-byte in little-endian order:
`((uint16_t)rom[addr]) | ((uint16_t)rom[addr + 1] << 8)`.

---

## M-18: Pre-loading DOOM WAD into PSRAM before calling `DoomEmu::begin()`
**What happens:** DOOM streams `.wad` data directly from the MicroSD FAT filesystem via POSIX VFS `fopen()` / `fread()`.
Loading a full copy into PSRAM wastes ~4MB of PSRAM and causes DOOM's internal zone allocator to fail.
**Prevention:** For `ROM_WAD`, pass the VFS file path `/sd/FILENAME.WAD` directly to `DoomEmu::begin()` with `romData = nullptr`.

---

## M-19: Leaving `DisplayEmu::startFrame()` open without matching `endFrame()`
**What happens:** `startFrame()` asserts `TFT_CS` LOW. If `endFrame()` is omitted, the SPI bus is held, causing any subsequent MicroSD SPI transactions to fail or collide.
**Prevention:** Always pair `DisplayEmu::startFrame()` with `DisplayEmu::endFrame()`. See also `35_bmo_face_contract.md` for the full mascot subsystem contract.

---

## M-20: Omitting handoff logs in `04_known_issues.md` and `CHANGELOG.md`
**What happens:** Future agents or sessions lack ground-truth context about what was modified, verified, or debunked, causing duplicate effort or regressions.
**Prevention:** Follow `33_agent_handoff_and_optimization_cycle.md` and always leave clear, dated logs before concluding a session.

---

<!-- Section: 31_quick_start_primer.md -->

# 31. Quick-Start Primer (Zero-Context Agent On-Ramp)
Purpose: Any AI agent or developer starting a session with zero prior memory can read this ONE page in 90 seconds and safely navigate the repository without breaking hardware or hallucinating APIs.

---

## 1. What Is This Repository? (3 Core Facts)
1. **Target Hardware:** ESP32-S3-N16R8 (16MB Flash, 8MB Octal PSRAM) running on a permanent perfboard.
2. **Current Physical State:** USB-C powered, ST7789 240x320 SPI display, 8 GPIO tactile buttons, MicroSD card. **No battery divider, no I2S audio DAC physically soldered.**
3. **Firmware Purpose:** Multi-console retro handheld (Game Boy, GBC, NES, DOOM) with a procedural 2D SDF animated mascot face ("BMO").

---

## 2. The 3 Absolute Hard Stops (Never Violate)
1. **NEVER read GPIO1 or enable `FEATURE_BATTERY_MONITOR` (must stay `0`).** Floating ADC on GPIO1 causes bootloops.
2. **NEVER enable `FEATURE_AUDIO` (must stay `0`).** No physical I2S DAC is wired.
3. **NEVER change Octal SPI Flash/PSRAM settings in Arduino CLI (requires OPI 80MHz).** QPI will brick boot.

---

## 3. Architecture in 30 Seconds
- **Main State Machine ([`BmoGameboy.ino`](file:///e:/BMO%20Gameboy/firmware/BmoGameboy/BmoGameboy.ino)):**
  - `STATE_CONSOLE_MENU` -> `STATE_GAME_MENU` -> `STATE_EMULATOR`.
  - Exiting emulator (`SELECT + UP`) invokes `destroy()` on active core and frees PSRAM.
- **Display Streaming Protocol (N3):**
  - Display is in Landscape (`320x240`). Game Boy is centered (`240x216`, `OFFSET_X=40, OFFSET_Y=12`).
  - Wire format: **BGR565 byte-swapped**.
  - `DisplayEmu::startFrame()` opens SPI window once per frame; scanlines stream via `streamPixelRow()`; `endFrame()` closes transaction.
- **Button Polling:**
  - `Buttons::update()` called once per frame. Maintains `Buttons::gb_joypad_state` (0=pressed). Emulators read bitmask branchlessly.
- **Flash & Partition Table:**
  - Custom `partitions.csv` allocates an **8MB `app0`** partition for firmware + baked ROMs (`mario_deluxe.h`, `zelda_ages.h`).

---

## 4. Where Is Code Located?
| Path | Contents |
| :--- | :--- |
| `firmware/BmoGameboy/BmoGameboy.ino` | `setup()`, `loop()`, state machine dispatch, frame pacing |
| `firmware/BmoGameboy/src/core/` | Hardware drivers (`config.h`, `display_emu`, `buttons`, `sd_card`, `bmo_face`, dormant `battery`/`audio_i2s`) |
| `firmware/BmoGameboy/src/emulators/` | Thin C++ glue wrappers (`emu_peanut`, `emu_walnut`, `emu_nes`, `emu_doom`) |
| `firmware/BmoGameboy/src/engine/` | Walnut GBC emulator engine (`walnut_cgb.h`) |
| `firmware/BmoGameboy/src/vendor/` | Pristine vendor engines (`peanut_gb`, `agnes`, `doom`) |
| `firmware/BmoGameboy/src/assets/roms/` | Baked flash ROM C headers |
| `tools/host_test.cpp` | Host desktop CPU verification harness (Zig compiler) |
| `.agents/rules/` | Agent governance rules (symbol reference, hardware pin map, incident logs) |

---

## 5. Task Decision Table: "I Want To Do X → Read File Y First"
| If your task touches... | You MUST read this rule file first: |
| :--- | :--- |
| Any pin, GPIO, or hardware component | `01_hardware.md` & `src/core/config.h` |
| Display rendering, colors, scaling, or SPI | `28_display_and_spi_contract.md` |
| Adding or modifying an emulator core | `12_extensibility_contract.md`, `26_emulator_exit_contract.md`, & `32_modular_core_template.md` |
| Adding a new baked ROM | `29_adding_a_baked_rom.md` |
| Citing any function or variable name | `10_symbol_reference.md` |
| Modifying vendor `#define` flags | `24_vendor_flag_safety.md` |
| AI guardrails, anti-hallucination, invariants | `34_ai_agent_sandbox_and_guardrails.md` |
| Ending session / logging handoffs | `33_agent_handoff_and_optimization_cycle.md` & `04_known_issues.md` |
| Writing commit messages or status reports | `05_git_workflow.md` & `06_verification_standards.md` |
| Investigating an unexpected bug | `04_known_issues.md` & `30_common_agent_mistakes.md` |

---

## 6. How To Build & Hand Off Cleanly
1. **Run AI Guardian CI Validation:**
   ```powershell
   python scripts/validate_repo.py
   python -m unittest discover tests
   ```
2. **Verified Firmware Build Command:**
   ```powershell
   .\arduino-cli.exe compile --fqbn "esp32:esp32:esp32s3:FlashMode=opi,FlashSize=16M,PartitionScheme=custom,PSRAM=opi" firmware/BmoGameboy
   ```
3. Run compilation / tests before claiming completion.
4. Update `04_known_issues.md` and `CHANGELOG.md` with a one-line dated entry.
5. Use the mandatory two-header final report format:
   - `## Verified by me this session`
   - `## Waiting on you`

---

<!-- Section: 32_modular_core_template.md -->

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

---

<!-- Section: 33_agent_handoff_and_optimization_cycle.md -->

# 33. Agent Handoff & Continuous Optimization Cycle

**Purpose:** Autonomous AI agents operate in sequential, isolated sessions. This rule defines the continuous improvement protocol where every agent leaves clean, verified artifacts and actionable logs, enabling the next agent to immediately build upon progress without friction or lost context.

---

## 1. The 4-Stage Agent Session Lifecycle

Every agent interacting with this repository MUST execute through this lifecycle:

```
[1. ORIENTATION] ───────> [2. INVESTIGATION] ───────> [3. VERIFIED EDIT] ───────> [4. HANDOFF LOG]
Read 31_quick_start.md     Grep active code           Apply minimal diff           Update 04_known_issues.md
Check known issues         Run validate_repo.py       Verify build & host tests    Update CHANGELOG.md
```

### Stage 1: Fast Orientation (First 90 Seconds)
1. Read [`31_quick_start_primer.md`](file:///e:/BMO%20Gameboy/.agents/rules/31_quick_start_primer.md).
2. Check `04_known_issues.md` to see currently active bugs, debunked theories, and pending verifications.
3. Review `AGENT_MANIFEST.json` for current pin maps and build commands.
4. For any human-reported device-misbehavior task with no specific symptom yet, read `36_bug_intake_protocol.md` before Stage 2.

### Stage 2: Investigation & Truth Discovery
1. Never assume an API or struct member exists from LLM training data. Always check `10_symbol_reference.md` or grep the actual header file.
2. Run `python scripts/validate_repo.py` before making edits to confirm a clean baseline.

### Stage 3: Verified Execution & Zero-Regression Edits
1. Adhere to the `EmulatorCoreContract` and zero-allocation hot path invariants.
2. Verify all claims using active session execution (`arduino-cli compile`, `python scripts/validate_repo.py`, `python -m unittest discover tests`).

### Stage 4: Handoff & Optimization Logging
1. If you fixed or investigated a bug, update `04_known_issues.md` with explicit status tag:
   - `OPEN`: Bug confirmed present and unresolved.
   - `FIXED_UNVERIFIED`: Code fix committed; compiles cleanly, awaiting physical hardware confirmation.
   - `VERIFIED_HOST`: Verified by desktop test harness (`tools/host_test.cpp`).
   - `VERIFIED_HARDWARE`: Confirmed functioning on physical perfboard console.
   - `DEBUNKED`: Investigated and proven to be a non-issue or false alarm.
2. Append a dated entry to root [`CHANGELOG.md`](file:///e:/BMO%20Gameboy/CHANGELOG.md).
3. If new public APIs were created or changed, update `10_symbol_reference.md`.

---

## 2. Standard Handoff Report Template

Every agent concluding a task should present findings using this structured format:

```markdown
## Verified by me this session
- [x] Command/Test: `python scripts/validate_repo.py` -> Passed in X ms.
- [x] Command/Test: `.\arduino-cli.exe compile ...` -> Exit code 0 (Sketch: X% flash, Y% RAM).
- [x] Exact code changes made and files touched.

## Known Issues & Handoff State for Next Agent
- Issue status updates logged in `04_known_issues.md`.
- Specific next steps or pending hardware tests for the subsequent session.

## Waiting on you (Human Reviewer)
- Specific questions or physical hardware tests needed.
```

---

<!-- Section: 34_ai_agent_sandbox_and_guardrails.md -->

# 34. AI Agent Guardrails & Execution Invariants

**Purpose:** This document establishes strict guardrails and fail-safe constraints for Large Language Models (LLMs) and autonomous agents operating in this repository. It eliminates common LLM failure modes (hallucinations, destructive refactoring, silent regressions, context overflow).

---

## 1. Golden Invariants for AI Agents

1. **Physical Hardware Precedence:**
   - The physical wiring and soldered state on the perfboard is ground truth.
   - Code must never enable dormant features (`FEATURE_BATTERY_MONITOR`, `FEATURE_AUDIO`) without user confirmation of physical soldering.
   - Never write to or read from GPIO 33-37 (reserved for internal Octal PSRAM).

2. **Zero-Hallucination API Rule:**
   - LLMs often hallucinate convenient helper methods (e.g. `DisplayEmu::drawText`, `Buttons::isDown`, `gb_write16`).
   - If a symbol is not present in `10_symbol_reference.md` or found in a live header file in `src/`, it DOES NOT EXIST. Do not call it.

3. **Atomic File Modifications:**
   - Avoid massive, monolithic file replacements when making localized bug fixes.
   - Preserve existing header guards, include hierarchies, and performance macros (`IRAM_ATTR`, `__attribute__((aligned(32)))`).

4. **Continuous CI Validation:**
   - Before finishing any task, run `python scripts/validate_repo.py`. If this script reports any violations, resolve them immediately before reporting completion.

5. **Endianness & Memory Alignment Safety:**
   - ST7789 wire format is **BGR565 Byte-Swapped (Big-Endian)**.
   - Flash memory `.rodata` is 8-bit byte aligned; unaligned 16-bit or 32-bit direct pointer dereferences on Flash can cause Xtensa exception crashes. Always use explicit byte reconstruction (`(b0) | (b1 << 8)`).

6. **Teardown & PSRAM Lifecycle Guardrail:**
   - Every emulator core must implement `destroy()`.
   - Every `begin()` dynamic allocation in PSRAM must be matched by a corresponding deallocation in `destroy()`.
   - The main `STATE_EMULATOR` loop in `BmoGameboy.ino` must call `destroy()` upon `SELECT + UP` exit.

---

## 2. Guardrail Failure Matrix & Instant Resolutions

| Symptom / Failure Mode | Root Cause | Instant Resolution |
| :--- | :--- | :--- |
| MCU reboots immediately on startup | Reading floating `GPIO1` | Ensure `FEATURE_BATTERY_MONITOR = 0` in `config.h` |
| Screen colors inverted (Red is Blue) | ST7789 MADCTL BGR bit missing | Check `DisplayEmu::begin()` writes `0xA0 \| 0x08` to MADCTL |
| Memory leak after playing several games | Core `destroy()` not called | Verify `BmoGameboy.ino` calls `Core::destroy()` and `SDCard::freeRom()` |
| Frame rate drops in game / audio stutter | Dynamic allocation in `runFrame()` | Move all `malloc`/`heap_caps_malloc` out of frame loop into `begin()` |
| Build fails with "undefined reference" | Hallucinated function name | Check `10_symbol_reference.md` for exact signature |

---

<!-- Section: 35_bmo_face_contract.md -->

# BmoFace Mascot Subsystem Contract
Purpose: consolidate every invariant governing the procedural SDF mascot
renderer into one place, the way `28_display_and_spi_contract.md` does for
the display. Previously scattered across the SDD, M-7, M-19, and the UI
style guide — this file is now the canonical source; those other files
reference it rather than restate it.

## Call-timing invariants
- `BmoFace::draw()` runs during `STATE_CONSOLE_MENU` and `STATE_GAME_MENU`.
  NEVER during active frame rendering in `STATE_EMULATOR` (mistake M-7) — calling
  `draw()` mid-frame corrupts the active SPI transaction.
- `BmoFace::update()` runs every `loop()` tick regardless of state
  (so blink and exponential-decay interpolation timers advance correctly
  even when not drawing).
- Every `BmoFace::draw()` call occurs strictly outside any open
  `DisplayEmu::startFrame()` / `endFrame()` pair (mistake M-19).

## Dirty-flag & caching contract
- `BmoFace::isDirty()` returns true if and only if an animated parameter
  (expression target, blink progress) changed since the last SDF render.
- `blitFace()` in `src/core/bmo_face.cpp` caches `faceBuf` (128×128): it only
  recomputes the 2D SDF (`renderFace()`, ~3ms) when `s_dirty` is true. On static
  frames, it reuses `faceBuf` to execute a fast ~0.4ms row-blit.
- In `STATE_CONSOLE_MENU` and `STATE_GAME_MENU`, `BmoFace::draw()` is called
  unconditionally after `writeMenuCanvas()`. This ensures the menu canvas's
  full-screen blit does not erase the mascot face on clean frames.

## Expression state-transition contract
- The 9 states are: `IDLE`, `HAPPY`, `SURPRISED`, `SLEEPY`, `LOW_BATTERY`,
  `CHARGING`, `ERROR`, `SHUTDOWN`, `HIDDEN` (SDD §6).
- `LOW_BATTERY` and `CHARGING` must never be reachable while
  `FEATURE_BATTERY_MONITOR=0` — all call sites passing these values are gated
  by `#if FEATURE_BATTERY_MONITOR`.
- All transitions interpolate via exponential decay (`k = EASE_RATE * dt`)
  EXCEPT `ERROR`, which snaps instantaneously without easing.

### Call Site Registry
| File & Line | Trigger Context | Target Expression |
|---|---|---|
| `BmoGameboy.ino:128` | Setup / boot splash initialization | `IDLE` |
| `BmoGameboy.ino:178` | Console Menu -> Game Menu navigation (`BTN_A`) | `IDLE` |
| `BmoGameboy.ino:220` | Game Menu -> Console Menu navigation (`BTN_B`) | `IDLE` |
| `BmoGameboy.ino:226` | Invalid ROM selection fallback | `IDLE` |
| `BmoGameboy.ino:273` | Emulator initialization failure fallback | `IDLE` |
| `BmoGameboy.ino:285` | Game launch celebration beat | `HAPPY` |
| `BmoGameboy.ino:332` | Emulator exit (`SELECT + UP`) return to menu | `IDLE` |
| `src/emulators/emu_walnut.cpp:114` | Walnut-CGB `gb_error` panic | `ERROR` |
| `src/emulators/emu_peanut.cpp:83` | Peanut-GB `gb_error` panic | `ERROR` |
| `src/core/battery.cpp:50` | Low battery shutdown (`FEATURE_BATTERY_MONITOR=1`) | `SHUTDOWN` |
| `src/core/battery.cpp:63` | Low battery warning (`FEATURE_BATTERY_MONITOR=1`) | `LOW_BATTERY` |

## Memory & rendering contract
- **Framebuffer:** 128×128 RGB565 in internal DRAM (32KB, `faceBuf`), statically
  allocated at module level with 4-byte alignment. Zero heap allocations
  (`malloc`/`new`) in `update()` or `draw()`.
- **Wire Format:** Color output is packed as BGR565 byte-swapped via `packBGR565()`,
  matching `DisplayEmu::uiColor` and `CLASSIC_PALETTE`.
- **Non-blocking:** Zero `delay()` calls in `BmoFace::update()` or `draw()`.
  All timing is driven by `millis()` and `micros()` deltas.

## Host-testability note
The 2D SDF math (ellipses, parabolic mouth, smoothstep AA, easing) is pure
arithmetic. The only hardware dependency is `DisplayEmu::pushPixelsAt()`.
Host test harnesses can compile and verify `renderFace()` output independently.

## Known failure signatures
| Symptom | Likely cause | Where to look |
|---|---|---|
| Face missing / flashes once and disappears | Menu canvas full-screen overwrite covering face when clean | `BmoGameboy.ino` menu loops |
| Face frozen, never animates | `BmoFace::update()` not called every tick | `loop()` top-level calls |
| Screen tears / corrupts near face | M-7 or M-19 violation | grep `BmoFace::` across ALL files |
| Colors wrong only on face | Missing BGR565 byte-swap | `bmo_face.cpp` `packBGR565` helper |
| Erratic / rapid blink | Blink timer not using `millis()` delta | Blink state variables in `bmo_face.cpp` |

---

<!-- Section: 36_bug_intake_protocol.md -->

# Human-Reported Hardware Bug Intake Protocol
Purpose: prevent a repeat of INC-1 (Hallucinated Bug Report,
`23_incident_postmortem_log.md`). When a human reports the device "isn't
behaving as expected" with no further detail, an agent's default
instinct is to form a plausible-sounding hypothesis and start editing
code. This file makes that instinct structurally harder to act on.

## When this applies
Any report of unexpected physical-device behavior with fewer than:
(a) a specific visual/behavioral description, (b) a when/how-to-reproduce
note, and (c) whether it's a regression. If a report already includes
all three, you may skip straight to the static-audit phase below. If it
doesn't, do NOT start editing code — do intake first.

## Phase 1: Structured intake (ask, don't assume)
Ask the human, as a concrete checklist (not open-ended prose):
1. **WHEN does it happen?** (always / specific state or transition /
   intermittent / after N minutes)
2. **WHAT does it look like, specifically?** Offer a menu of concrete
   options relevant to the subsystem in question rather than an open
   "describe the bug" — humans under-describe visual bugs in prose, a
   checklist gets better signal.
3. **Any correlated action?** (button presses, SD removal, console switch)
4. **Regression or always-broken?** If regression, roughly when did it last
   work (a date, a firmware commit, "before the last flash")?
5. **Can they capture a Serial Monitor log around the event?**

If some questions go unanswered, proceed with what's available but list
the gaps explicitly in your final report's "Waiting on you" section —
never backfill an unanswered question with an assumption stated as fact.

## Phase 2: Static audit BEFORE hypothesis
Before naming a root cause, read the actual live code for the implicated
subsystem and check it against every documented contract/mistake entry
for that subsystem (e.g. `28_display_and_spi_contract.md` +
`30_common_agent_mistakes.md` for display bugs; `35_bmo_face_contract.md` for
mascot bugs). For each relevant contract, report PASS (quote the line
proving it) or FAIL (quote the violating line) — "looks fine" without a
quoted line is not acceptable evidence (`06_verification_standards.md`).

## Phase 3: Map symptom to hypothesis
Only after Phase 1 + Phase 2, state which hypothesis the EVIDENCE
supports. A hypothesis whose corresponding Phase 2 check came back PASS
is ruled out — do not pursue it further. If no hypothesis is supported
with confidence, say so explicitly and propose gated LOG_DEBUG
instrumentation (`16_logging_and_diagnostics.md`) as the next step, rather
than picking the most plausible-sounding guess.

## Phase 4: Instrumentation is the last resort, not the first move
If Phase 2 can't rule a hypothesis in or out from static reading alone,
add the minimum `LOG_DEBUG` calls needed, gated behind `LOG_LEVEL`, and
explicitly hand off to the human for a physical capture — this is a
"Waiting on you" item, not something you can verify yourself.

## Phase 5: Fix only with evidence, verify with an explicit PASS bar
State the fix's plan in 3-6 bullets citing which Phase 2/4 evidence
justifies it (`07_task_protocol.md`). Before claiming any status better
than `FIXED_UNVERIFIED`, state an explicit, human-checkable PASS/FAIL
description (`06_verification_standards.md`'s `VERIFIED_HARDWARE`
requirement) — e.g. "X transitions within 1 second, no corruption over 5
repetitions" — not "should be fixed now."

## Anti-pattern this file exists to prevent
Naming a specific function, line number, or variable in a bug report
that you have not personally greped from a live file THIS session. This
is precisely how INC-1 happened — a fabricated, confidently-specific
report. If you don't know it, say "I have not verified this" instead of
inventing a plausible detail.

---

<!-- Section: 37_rom_governance_and_flash_budget.md -->

# ROM Governance & Flash-Budget Invariant
Purpose: resolve the standing ambiguity between `docs/hardware-notes.md`
§11 ("never commit a copyrighted commercial ROM... .gitignore excludes
ROM files and their generated headers") and `27_codebase_map.md` (which
shows registered baked ROMs the unconditional build depends on). Also
promotes the flash-budget check from a one-time manual checklist
(`29_adding_a_baked_rom.md`) to a standing invariant.

## Tracking status (verified ground truth)
Audit command executed in session:
```powershell
git ls-files firmware/BmoGameboy/src/assets/roms/
```
Literal output:
```text
firmware/BmoGameboy/src/assets/roms/aladdin.h
firmware/BmoGameboy/src/assets/roms/lego_racers.h
firmware/BmoGameboy/src/assets/roms/mario_deluxe.h
firmware/BmoGameboy/src/assets/roms/zelda_ages.h
```

**Finding:** All four ROM headers under `src/assets/roms/` are currently
tracked and committed in git history. The root `.gitignore` specifies
patterns `rom_*.h` and `rom_data*.h`, which do not match `aladdin.h`,
`lego_racers.h`, `mario_deluxe.h`, or `zelda_ages.h`.

## Governance & Legality Policy
- **Direct Policy Conflict:** The presence of commercial ROM C-arrays in git
  directly conflicts with `docs/hardware-notes.md` §11 ("never commit a
  copyrighted commercial ROM").
- **Standing Status:** Tracked as an `OPEN` technical debt item in
  `04_known_issues.md` for human operator resolution (licensing and
  distribution decisions are outside autonomous agent authority).
- In the interim, firmware builds compile against these four tracked headers
  (`mario_deluxe.h`, `zelda_ages.h`, `aladdin.h`, `lego_racers.h`).

## Flash-budget invariant (standing rule)
Any change that adds to `app0`'s compiled content — a new baked ROM, a new
large const table, or a new library — MUST satisfy:
$$\text{new\_total\_binary\_size\_bytes} < 8,388,608 \text{ bytes (app0 partition capacity)}$$
Verified by an ACTUAL `arduino-cli compile` run in the current session, not an estimate.

This check is a mandatory part of Definition of Done (`07_task_protocol.md`)
for ANY change touching `src/assets/` or adding vendor libraries.

## Partition-change protocol (repartitioning app0/ffat)
The `ffat` partition (~7.9MB) is currently unused. Growing `app0` at its
expense is possible in principle but is a HARD-STOP-ADJACENT change:
1. First grep the entire firmware for any `FFat.` / `SPIFFS.` / filesystem
   mount call to confirm `ffat` is truly unused before touching it.
2. A `partitions.csv` change requires the review gate in `05_git_workflow.md`
   (show `git diff --stat` + message, wait for explicit human approval)
   BEFORE committing, every time, no exceptions.
3. Never repartition speculatively "in case it's needed" — only when a
   specific, currently-blocked ROM registration is the stated reason,
   documented in the same commit.
4. After any partition change, re-run the FULL flash-budget verification
   above, plus confirm OTA metadata (`otadata`) still fits its 8KB
   allocation unchanged.
