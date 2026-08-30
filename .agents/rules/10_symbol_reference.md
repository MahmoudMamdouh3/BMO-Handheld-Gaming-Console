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
- `src/emulators/emu_walnut.cpp`
- `src/emulators/emu_peanut.cpp`
- `src/engine/walnut_cgb/walnut_cgb.h`
- `src/core/config.h`

Run: `git log --oneline -1 -- <file>` for each to check. If any show a
commit newer than the date above, grep live instead of trusting this table.

## Format
One table per file, columns: `Symbol | Kind (fn/macro/struct) | Signature
| Notes`. Keep entries terse — this is a lookup table, not documentation
prose.

## Standing rule for all agents
Before writing a claim that references a specific function, macro, or
struct field by name — especially in a bug report, a diff justification,
or a "here's what's wrong" explanation — check it against this file (or
grep live if this file might be stale) BEFORE presenting the claim as
fact. If the symbol isn't found in either, say "I could not find a symbol
called X in the codebase" instead of describing what it supposedly does.

## src/emulators/emu_walnut.cpp
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `gb_rom_read16` | fn | `uint16_t gb_rom_read16(struct gb_s *gb, const uint_fast32_t addr)` | Reads 2 bytes from external flash |
| `gb_rom_read32` | fn | `uint32_t gb_rom_read32(struct gb_s *gb, const uint_fast32_t addr)` | Reads 4 bytes from external flash |
| `gb_rom_read` | fn | `uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr)` | Reads 1 byte from external flash |
| `gb_error` | fn | `void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val)` | Handles emulator panics |
| `lcd_draw_line` | fn | `void lcd_draw_line(struct gb_s *gb, const uint8_t *pixels, const uint_fast8_t line)` | TFT raster callback |

## src/engine/walnut_cgb/walnut_cgb.h
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `gb_init` | fn | `enum gb_init_error_e gb_init(struct gb_s* gb, uint8_t(*gb_rom_read)(struct gb_s*, const uint_fast32_t), uint16_t(*gb_rom_read16)(struct gb_s*, const uint_fast32_t), uint32_t(*gb_rom_read32)(struct gb_s*, const uint_fast32_t), uint8_t(*gb_cart_ram_read)(struct gb_s*, const uint_fast32_t), void (*gb_cart_ram_write)(struct gb_s*, const uint_fast32_t, const uint8_t), void (*gb_error)(struct gb_s*, const enum gb_error_e, const uint16_t), void* priv)` | 8 arguments including 16/32 read callbacks |
| `gb_run_frame` | fn | `void gb_run_frame(struct gb_s *gb)` | Executes one frame via `__gb_step_cpu_x` |
| `gb_run_frame_dualfetch` | fn | `void gb_run_frame_dualfetch(struct gb_s *gb)` | Executes one frame via `__gb_step_cpu` (dual-fetch path); THIS is what `WalnutEmu::runFrame()` calls |
| `gb_init_lcd` | fn | `void gb_init_lcd(struct gb_s *gb, void(*lcd_draw_line)(...))` | Registers scanline callback |
| `gb_s` | struct | `struct gb_s` | Core emulator context, align(32) |
| `gb_init_error_e` | enum | `enum gb_init_error_e` | Initialization error codes |

## src/core/config.h
| Symbol | Kind | Signature | Notes |
|---|---|---|---|
| `FEATURE_SD_CARD` | macro | `#define FEATURE_SD_CARD 1` | Gates SD logic |
| `FEATURE_BATTERY_MONITOR` | macro | `#define FEATURE_BATTERY_MONITOR 0` | Dormant battery logic |
| `FEATURE_AUDIO` | macro | `#define FEATURE_AUDIO 0` | Dormant I2S audio logic |
| `BTN_UP`... | macro | `#define BTN_UP 4` (etc) | GPIO pin assignments |
