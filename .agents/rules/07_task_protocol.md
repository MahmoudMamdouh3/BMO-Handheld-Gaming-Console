# Standard Task Protocol
Any agent (strong or weak model) starting ANY task in this repo should
follow this sequence. This exists so behavior doesn't depend on how
capable or careful the specific model is.

## Before starting
1. Read 00_hard_stops.md, 01_hardware.md in full — always, every task,
   regardless of how unrelated the task seems, since a task that looks
   pure-software can still touch a pin or a feature flag.
2. Read 10_symbol_reference.md and confirm any function/struct/macro
   you're about to reference actually appears there. If it doesn't, grep
   for it yourself before using it in a claim — never assume a name
   exists because it "sounds right" for the codebase.
3. `git status`; checkpoint-commit if dirty.
4. State your plan in 3-6 bullets before writing code. If the task
   touches anything in 00_hard_stops.md's categories, say so explicitly
   and state which constraint applies.

## During
5. Prefer the smallest change that satisfies the request. Don't bundle
   unrelated cleanup into the same commit (see 05_git_workflow.md).
6. If you hit an unrelated bug while working, do NOT silently fix it
   inline — note it, ask, or log it as a new 04_known_issues.md entry
   with status `OPEN`, and finish the requested task first, unless it's
   blocking the requested task.
7. If you're about to fabricate a plausible-sounding detail because you
   don't actually know it (a line number, a variable name, a test
   result) — stop. State the uncertainty instead. This is the single
   highest-value habit for this repo given past incidents.
8. **Stop-after-2-failures rule:** if the same interaction/approach fails
   twice in a row (a build flag, a test invocation, a specific fix
   attempt), STOP retrying variations of the same approach. Switch
   strategy entirely, or stop and ask. Never spend more than 5 tool calls
   total on a single stuck sub-problem before changing approach or asking.
9. **Implementation-plan gate for substantial changes:** for anything
   multi-file, architectural, or that would trigger the review gate in
   05_git_workflow.md (hard-stop-adjacent, history-rewriting, new
   binaries) — write a short plan (a few bullets is fine, doesn't need to
   be a separate file for most cases) and wait for explicit approval
   before writing code or running risky commands. Trivial fixes and
   single-line edits skip this, same as always.

## Before reporting done
10. Compile. A successful compile is necessary, never sufficient, for
   "done" — see 06_verification_standards.md for what else is required
   depending on what was touched.
11. Update .agents/rules/ files per the Documentation Maintenance Protocol
   in 03_conventions.md, using the status vocabulary from
   06_verification_standards.md.
12. Update 04_known_issues.md's Changelog section with a one-line dated
    entry (see existing entries for format).

## Final report format (always use these two headers, verbatim)
```
## Verified by me this session
<only things you personally ran/read/confirmed in THIS session>

## Waiting on you
<anything requiring physical hardware, a human decision, or an external
resource you don't have access to — with an exact checklist, per
06_verification_standards.md's PASS/FAIL definition rules>
```
Never merge these two categories. Never imply the second category is done.

## Definition of Done, by task type
- **Docs-only change:** compiles N/A; diff reviewed; cross-references
  checked (grep for old section numbers/filenames before renaming).
- **Bug fix, no hardware-timing implication:** compiles; host-side test
  covering the fix passes to full completion; known_issues updated.
- **Change to CPU core / memory access / ISR / timing-critical code:**
  compiles; host-side test passes to full completion; hardware handoff
  checklist prepared; known_issues stays `FIXED_UNVERIFIED` or
  `VERIFIED_HOST` until a human reports back `VERIFIED_HARDWARE`.
- **New hardware peripheral integration:** isolated standalone `.ino` in
  tools/ verified first (03_conventions.md rule already requires this);
  01_hardware.md and the pin map updated ONLY after physical soldering is
  confirmed by the human, never in anticipation of it.
- **New UI screen/menu:** follows 08_ui_style_guide.md; no new raw hex
  colors outside the shared theme constants file.
- **Rules-directory / process change:** diff reviewed; every new file under 12,000 chars; README index and RULESET_VERSION updated; changelog entries added; no hard-stop or hardware-ground-truth text restated elsewhere.
