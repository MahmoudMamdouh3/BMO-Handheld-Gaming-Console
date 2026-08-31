# .agents/rules/ Index
**RULESET_VERSION: 7**

Read in this order. 00-01 are mandatory for every task, no exceptions.
For a fast 90-second primer on the entire codebase, start with [`31_quick_start_primer.md`](file:///e:/BMO%20Gameboy/.agents/rules/31_quick_start_primer.md).

| File | Purpose | Mandatory? |
|---|---|---|
| [`00_hard_stops.md`](file:///e:/BMO%20Gameboy/.agents/rules/00_hard_stops.md) | Irreversible/boot-bricking mistakes to never make | Always |
| [`01_hardware.md`](file:///e:/BMO%20Gameboy/.agents/rules/01_hardware.md) | Physically wired ground truth + pin map | Always |
| [`02_architecture.md`](file:///e:/BMO%20Gameboy/.agents/rules/02_architecture.md) | Toolchain, build config, performance patterns | Task-dependent |
| [`03_conventions.md`](file:///e:/BMO%20Gameboy/.agents/rules/03_conventions.md) | Repo structure, testing rule, licensing, doc maintenance | Task-dependent |
| [`04_known_issues.md`](file:///e:/BMO%20Gameboy/.agents/rules/04_known_issues.md) | Technical debt log + changelog | Always skim |
| [`05_git_workflow.md`](file:///e:/BMO%20Gameboy/.agents/rules/05_git_workflow.md) | Commit granularity, message format, tagging, branching | Always |
| [`06_verification_standards.md`](file:///e:/BMO%20Gameboy/.agents/rules/06_verification_standards.md) | Evidence rules, status vocabulary, banned language | Always |
| [`07_task_protocol.md`](file:///e:/BMO%20Gameboy/.agents/rules/07_task_protocol.md) | Step-by-step task checklist + report format | Always |
| [`08_ui_style_guide.md`](file:///e:/BMO%20Gameboy/.agents/rules/08_ui_style_guide.md) | Theme constants, layout, typography, motion rules | UI tasks |
| [`09_testing_infrastructure.md`](file:///e:/BMO%20Gameboy/.agents/rules/09_testing_infrastructure.md) | host_test harness, ROM ledger, pass/fail definitions | Core/CPU tasks |
| [`10_symbol_reference.md`](file:///e:/BMO%20Gameboy/.agents/rules/10_symbol_reference.md) | Ground-truth symbol table (prevents invented names) | Always check before code claims |
| [`11_rules_meta.md`](file:///e:/BMO%20Gameboy/.agents/rules/11_rules_meta.md) | Rules about the rules directory itself | Rare |
| [`12_extensibility_contract.md`](file:///e:/BMO%20Gameboy/.agents/rules/12_extensibility_contract.md) | New emulator core contract, teardown rule, no-per-frame-alloc rule | New emulator/core-touching tasks |
| [`13_code_style.md`](file:///e:/BMO%20Gameboy/.agents/rules/13_code_style.md) | Code style, conventions, naming | Code edits |
| [`14_error_handling_and_fault_isolation.md`](file:///e:/BMO%20Gameboy/.agents/rules/14_error_handling_and_fault_isolation.md) | Failure paths and panics | Core tasks |
| [`15_performance_budgets.md`](file:///e:/BMO%20Gameboy/.agents/rules/15_performance_budgets.md) | Hot path tracking, zero-allocation rules | Core tasks |
| [`16_logging_and_diagnostics.md`](file:///e:/BMO%20Gameboy/.agents/rules/16_logging_and_diagnostics.md) | Log levels, visibility, boot logs | Core tasks |
| [`17_release_and_versioning.md`](file:///e:/BMO%20Gameboy/.agents/rules/17_release_and_versioning.md) | Version definitions and release checklist | Releases |
| [`18_dependency_and_vendor_sync.md`](file:///e:/BMO%20Gameboy/.agents/rules/18_dependency_and_vendor_sync.md) | Patch handling and upstream syncing | Vendor/Library updates |
| [`19_security_and_data_integrity.md`](file:///e:/BMO%20Gameboy/.agents/rules/19_security_and_data_integrity.md) | Untrusted input constraints | I/O tasks |
| [`20_multi_agent_protocol.md`](file:///e:/BMO%20Gameboy/.agents/rules/20_multi_agent_protocol.md) | Agent-to-agent crediting and handoffs | Always |
| [`21_documentation_standards.md`](file:///e:/BMO%20Gameboy/.agents/rules/21_documentation_standards.md) | In-code and out-of-code documentation rules | Docs/Architecture tasks |
| [`22_review_checklist.md`](file:///e:/BMO%20Gameboy/.agents/rules/22_review_checklist.md) | Lightweight everyday self-review | Before reporting done |
| [`23_incident_postmortem_log.md`](file:///e:/BMO%20Gameboy/.agents/rules/23_incident_postmortem_log.md) | Record of process failures | Process revisions |
| [`24_vendor_flag_safety.md`](file:///e:/BMO%20Gameboy/.agents/rules/24_vendor_flag_safety.md) | Mandatory read-before-enable protocol for vendor #define flags | **Always before touching flags** |
| [`25_game_compatibility_ledger.md`](file:///e:/BMO%20Gameboy/.agents/rules/25_game_compatibility_ledger.md) | Per-game hardware test ledger; WORKS/PARTIAL/BROKEN status | Always skim before emulator changes |
| [`26_emulator_exit_contract.md`](file:///e:/BMO%20Gameboy/.agents/rules/26_emulator_exit_contract.md) | destroy() teardown requirements; documents active 4-core dispatch | Emulator or menu tasks |
| [`27_codebase_map.md`](file:///e:/BMO%20Gameboy/.agents/rules/27_codebase_map.md) | Full architecture map: directories, state machine, routing, PSRAM, SPI, partitions | **Always** |
| [`28_display_and_spi_contract.md`](file:///e:/BMO%20Gameboy/.agents/rules/28_display_and_spi_contract.md) | Pixel format, startFrame/endFrame protocol, SPI sharing rules | Display/rendering tasks |
| [`29_adding_a_baked_rom.md`](file:///e:/BMO%20Gameboy/.agents/rules/29_adding_a_baked_rom.md) | Step-by-step checklist for baking a new ROM into flash | Adding ROM tasks |
| [`30_common_agent_mistakes.md`](file:///e:/BMO%20Gameboy/.agents/rules/30_common_agent_mistakes.md) | Anti-pattern catalogue (M-1 through M-20) with INC cross-references | **Always skim** |
| [`31_quick_start_primer.md`](file:///e:/BMO%20Gameboy/.agents/rules/31_quick_start_primer.md) | Zero-context 90-second agent on-ramp & decision matrix | **New agents / Quick start** |
| [`32_modular_core_template.md`](file:///e:/BMO%20Gameboy/.agents/rules/32_modular_core_template.md) | Copy-paste template and scaffolding for adding new cores/screens | New core/screen tasks |
| [`33_agent_handoff_and_optimization_cycle.md`](file:///e:/BMO%20Gameboy/.agents/rules/33_agent_handoff_and_optimization_cycle.md) | Anonymous agent-to-agent continuous optimization loop | Always at session handoff |
| [`34_ai_agent_sandbox_and_guardrails.md`](file:///e:/BMO%20Gameboy/.agents/rules/34_ai_agent_sandbox_and_guardrails.md) | LLM anti-hallucination protocols, memory alignment, hardware safety | **Always for AI agents** |
| [`35_bmo_face_contract.md`](file:///e:/BMO%20Gameboy/.agents/rules/35_bmo_face_contract.md) | Procedural SDF mascot renderer contract, dirty-flag caching, call sites | Mascot/UI tasks |
| [`36_bug_intake_protocol.md`](file:///e:/BMO%20Gameboy/.agents/rules/36_bug_intake_protocol.md) | Structured bug intake protocol for human-reported device misbehavior | Bug investigation tasks |
| [`37_rom_governance_and_flash_budget.md`](file:///e:/BMO%20Gameboy/.agents/rules/37_rom_governance_and_flash_budget.md) | ROM tracking resolution, standing flash-budget invariant, partition changes | Assets/ROM tasks |
| [`38_mandatory_documentation_update.md`](file:///e:/BMO%20Gameboy/.agents/rules/38_mandatory_documentation_update.md) | **Forced rule**: required documentation updates per change type, pre-commit checklist | **Always — every task** |
| [`39_performance_and_benchmark_framework.md`](file:///e:/BMO%20Gameboy/.agents/rules/39_performance_and_benchmark_framework.md) | Mandatory Guardian performance & ground-truth framework governance | **Always — all performance tasks** |
| [`40_ai_knowledge_graph_and_indexing_protocol.md`](file:///e:/BMO%20Gameboy/.agents/rules/40_ai_knowledge_graph_and_indexing_protocol.md) | Mandatory AI Knowledge Graph, AST Indexing & Decision Matrix Protocol | **Always — every task** |
| [`41_engineering_communication_and_critical_pushback.md`](file:///e:/BMO%20Gameboy/.agents/rules/41_engineering_communication_and_critical_pushback.md) | Engineering communication, critical pushback, fact-checking & toolchain autonomy | **Always — all sessions** |
