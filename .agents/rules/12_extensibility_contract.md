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
