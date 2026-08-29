# Git & Commit Discipline

## Commit granularity
- One logical change per commit. "Logical" means: if you'd describe it
  with an "and" that isn't a shared cause (e.g. "fixed the pointer cast
  AND renumbered the docs"), it's two commits.
- Always checkpoint-commit BEFORE starting risky/multi-step work, so any
  session is revertable to a known-good point. This is not optional and
  is not the same commit as the work itself.
- Never combine a hardware-behavior change with a docs-only change in one
  commit — they have different review/verification needs.

## Commit message format (Conventional Commits)
`type(scope): summary`, where type is one of:
`fix | feat | docs | test | refactor | chore | perf`
- scope is the module touched (e.g. `emu_walnut`, `rules`, `sd_card`).
- Body (optional, below the summary line) must state verification status
  using the vocabulary in 06_verification_standards.md — e.g. "Verified:
  host-test suite full pass (11/11). Hardware: unverified."
- If the commit closes or changes a 04_known_issues.md entry, reference it:
  `Refs: KI-1`.

## Tags for hardware-verified states
- After a human confirms a PASS on real hardware per a handoff checklist,
  tag that commit: `git tag hw-verified-YYYYMMDD-<short-feature>`. This
  gives every future agent a guaranteed-good rollback point that isn't
  just "compiles" but "was seen working on the actual board." List active
  tags at the top of 04_known_issues.md so agents don't have to `git tag
  -l` and guess which one is relevant.

## Branching
- Default to committing directly to `main` for docs-only, test-only, or
  low-risk changes.
- For anything that changes CPU/memory-safety-critical code (emulator
  cores, pointer arithmetic, ISR/timer code) or touches a HARD STOP
  category from 00_hard_stops.md, create a short-lived branch
  (`fix/<topic>` or `feat/<topic>`), do the work, and only merge to `main`
  after the verification steps in 06_verification_standards.md pass. Say
  explicitly whether you branched and why if you didn't.

## What never gets committed
- Anything already excluded by .gitignore (ROMs, generated ROM headers).
- Downloaded toolchains (e.g. a Zig binary fetched for host_test) — commit
  a documented, pinned VERSION reference in 02_architecture.md instead, not
  the binary itself.
- Build output directories, host_test binaries, `.o`/`.elf`/`.bin` files.
- Anything from Part A of a prompt that Part A told you to correct — i.e.
  never commit an overclaimed result before it's actually true.

## Review gate before committing actual work
The existing checkpoint-commit-before-starting rule stays automatic — no
review needed for that, it's a safety net.
For the commit that represents COMPLETED work, before running `git
commit`: show me the exact `git diff --stat` (file list) and the proposed
commit message, and wait for an explicit yes before committing, IF any of
these apply:
- Any new file over 1MB is being added.
- Any new binary, executable, or downloaded-toolchain file is being added
  (this is exactly how the Zig binary ended up in history last time — a
  review step here would have caught it before the commit, not after).
- The change touches anything in a 00_hard_stops.md category.
- The change involves rewriting git history (filter-repo, rebase -i,
  amend of an already-old commit).
Routine docs-only or single-file low-risk commits don't need this gate —
use judgment, but the binary/size/hard-stop triggers above are not
judgment calls, they always require the review step.
