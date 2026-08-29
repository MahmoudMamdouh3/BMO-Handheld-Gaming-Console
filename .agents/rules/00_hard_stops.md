# 0. Hard Stops
**CRITICAL:** These are irreversible, boot-bricking, or severely damaging mistakes. Do NOT violate these constraints under any circumstances.
- **GPIO1 Floating ADC Boot-Loop:** Do NOT use GPIO1 (Battery ADC) in any code without confirming the voltage divider is physically soldered. Reading a floating GPIO1 on the ESP32-S3 can trigger unstable behavior or boot loops.
- **OPI Flash Mode:** The board is an ESP32-S3-N16R8. It REQUIRES OPI flash mode (80MHz). Using QPI or other modes will result in a non-booting or heavily degraded system.
- **No Fatal Deadlocks on Missing Hardware:** Any code path that runs when a hardware feature flag is disabled (`0`) MUST NOT hang, sleep, or crash waiting for hardware that isn't there. For example, `Battery::update()` must not trigger deep sleep loops, and `SD.begin()` must not be called if `FEATURE_SD_CARD == 0`.
- **Perfboard Permanence:** Everything is soldered onto a permanent perfboard. Software must adapt to the wiring, not the other way around. Always explicitly test code in isolated `.ino` sketches before integrating hardware changes.
