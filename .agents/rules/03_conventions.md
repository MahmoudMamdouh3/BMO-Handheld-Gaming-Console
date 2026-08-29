# 7. Repository & File Structure
The project uses a **single, clean repository structure**. No numbered milestone folders (`01_foo`, `02_bar`) are permitted. Past states are tracked via git history.

```
repo-root/
├── README.md
├── .gitignore
├── docs/                          ← human-readable documentation stubs
├── .agents/                       ← ground-truth LLM agent rules
│   └── rules/                     ← topic-specific modular rule files
├── tools/                         ← build/asset tooling scripts
└── firmware/
    └── BmoGameboy/                ← Arduino sketch folder (MUST match .ino)
        ├── BmoGameboy.ino         ← ONLY setup(), loop(), and state dispatch
        ├── partitions.csv
        └── src/
            ├── core/              ← custom hardware drivers (config.h, display, buttons)
            ├── emulators/         ← glue code per console (emu_peanut, emu_doom)
            ├── vendor/            ← pristine third-party libraries (peanut_gb, doom)
            ├── assets/            ← generated binary-as-C-array data (bmo_face SDFs)
            │   └── roms/          ← commercial baked ROM headers
            └── tests/             ← unit_tests.cpp
```
- **Rule:** Third-party libraries go under `src/vendor/<name>/` and must remain as close to upstream as possible.
- **Rule:** Generated assets (sprites, ROM arrays) go under `src/assets/`.

---

# 8. Testing & Verification Workflow
- The file `src/tests/unit_tests.cpp` contains critical validation tests (e.g., PSRAM speed checks, module validation). 
- **Rule:** No task should be reported complete without at minimum a successful compile.
- **Rule:** If touching memory architecture or core drivers, the unit tests should be executed by calling `UnitTests::runAll()` in `setup()` before integration.

---

# 9. IP & Licensing Notes
This project currently bakes multiple commercial ROMs directly into the firmware flash as fallback games (`mario_deluxe.h`, `zelda_ages.h`, `aladdin.h`, `lego_racers.h` under `src/assets/roms/`). 
- This is a known, deliberate personal/non-commercial choice.
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
