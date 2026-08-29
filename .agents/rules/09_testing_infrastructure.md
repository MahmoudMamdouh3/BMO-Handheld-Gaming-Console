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
