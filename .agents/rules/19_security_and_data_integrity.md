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
