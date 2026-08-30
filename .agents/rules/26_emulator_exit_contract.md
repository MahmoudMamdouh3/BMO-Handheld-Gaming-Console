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
