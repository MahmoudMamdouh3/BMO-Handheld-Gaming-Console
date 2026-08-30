# .agents/rules/ Index
**RULESET_VERSION: 3**

Read in this order. 00-01 are mandatory for every task, no exceptions.
For a fast 90-second primer on the entire codebase, start with `31_quick_start_primer.md`.

| File | Purpose | Mandatory? |
|---|---|---|
| 00_hard_stops.md | Irreversible/boot-bricking mistakes to never make | Always |
| 01_hardware.md | Physically wired ground truth + pin map | Always |
| 02_architecture.md | Toolchain, build config, performance patterns | Task-dependent |
| 03_conventions.md | Repo structure, testing rule, licensing, doc maintenance | Task-dependent |
| 04_known_issues.md | Technical debt log + changelog | Always skim |
| 05_git_workflow.md | Commit granularity, message format, tagging, branching | Always |
| 06_verification_standards.md | Evidence rules, status vocabulary, banned language | Always |
| 07_task_protocol.md | Step-by-step task checklist + report format | Always |
| 08_ui_style_guide.md | Theme constants, layout, typography, motion rules | UI tasks |
| 09_testing_infrastructure.md | host_test harness, ROM ledger, pass/fail definitions | Core/CPU tasks |
| 10_symbol_reference.md | Ground-truth symbol table (prevents invented names) | Always check before code claims |
| 11_rules_meta.md | Rules about the rules directory itself | Rare |
| 12_extensibility_contract.md | New emulator core contract, teardown rule, no-per-frame-alloc rule | New emulator/core-touching tasks |
| 13_code_style.md | Code style, conventions, naming | Code edits |
| 14_error_handling_and_fault_isolation.md | Failure paths and panics | Core tasks |
| 15_performance_budgets.md | Hot path tracking, zero-allocation rules | Core tasks |
| 16_logging_and_diagnostics.md | Log levels, visibility, boot logs | Core tasks |
| 17_release_and_versioning.md | Version definitions and release checklist | Releases |
| 18_dependency_and_vendor_sync.md | Patch handling and upstream syncing | Vendor/Library updates |
| 19_security_and_data_integrity.md | Untrusted input constraints | I/O tasks |
| 20_multi_agent_protocol.md | Agent-to-agent crediting and handoffs | Always |
| 21_documentation_standards.md | In-code and out-of-code documentation rules | Docs/Architecture tasks |
| 22_review_checklist.md | Lightweight everyday self-review | Before reporting done |
| 23_incident_postmortem_log.md | Record of process failures | Process revisions |
| 24_vendor_flag_safety.md | Mandatory read-before-enable protocol for vendor #define flags | **Always before touching src/engine/ or src/vendor/ flags** |
| 25_game_compatibility_ledger.md | Per-game hardware test ledger; WORKS/PARTIAL/BROKEN status | Always skim before emulator changes |
| 26_emulator_exit_contract.md | destroy() teardown requirements; documents active 4-core dispatch | Emulator or menu tasks |
| 27_codebase_map.md | Full architecture map: directories, state machine, routing, PSRAM, SPI, partitions | **Always** (replaces needing to re-read 5 files from scratch) |
| 28_display_and_spi_contract.md | Pixel format, startFrame/endFrame protocol, SPI sharing rules | Display/rendering tasks |
| 29_adding_a_baked_rom.md | Step-by-step checklist for baking a new ROM into flash | Adding ROM tasks |
| 30_common_agent_mistakes.md | Anti-pattern catalogue (M-1 through M-16) with INC cross-references | **Always skim** — prevents repeated mistakes |
| 31_quick_start_primer.md | Zero-context 90-second agent on-ramp & decision matrix | **New agents / Quick start** |

