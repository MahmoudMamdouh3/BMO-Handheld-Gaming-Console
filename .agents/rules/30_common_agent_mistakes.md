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
do not move those calls inside the emulator branch.

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

## M-11: Forgetting that aladdin.h and lego_racers.h are ~6MB each
**What happens:** Agent registers them as baked ROMs. Binary exceeds 8MB.
Flash fails. Device may require reflash with different partition table.
**Prevention:** See 29_adding_a_baked_rom.md step 1. Check binary size
before committing any new baked ROM registration.

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
**Prevention:** See 26_emulator_exit_contract.md. This is a known OPEN bug.
When fixing it, follow the exact dispatch pattern in that file.
