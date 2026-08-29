# .agents/rules/ Index
Read in this order. 00-01 are mandatory for every task, no exceptions.

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
