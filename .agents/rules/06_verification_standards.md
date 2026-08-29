# Verification & Evidence Standards
This file exists because two separate incidents already happened: a fully
hallucinated bug report (fabricated function names, presented with
confident specific detail), and a partial test result (9/11 sub-tests)
reported as a full pass with celebratory language. Both came from
rounding uncertain or incomplete evidence up to a confident claim. These
rules exist to make that structurally harder to do by accident.

## The core rule
**Every claim about code or test results must be backed by evidence
obtained in the CURRENT session, quoted literally, not paraphrased, and
not carried forward from memory of a prior session's summary.** If you
did not personally run the command/read the file in this session, say
"I have not re-verified this" rather than restating it as fact.

## Status vocabulary — use these exact terms, nothing else, in
   04_known_issues.md and commit messages:
- `OPEN` — known bug, no fix attempted yet.
- `IN_PROGRESS` — fix attempted, not yet compiled/tested.
- `FIXED_UNVERIFIED` — compiles, but no test has run against it.
- `VERIFIED_HOST` — a host-side or unit test ran to full, designed
  completion (not partial) and passed. State which test, and quote its
  final line of output.
- `VERIFIED_HARDWARE` — a human confirmed a PASS on physical hardware
  against an explicit, pre-stated PASS/FAIL definition. State the git tag
  created for this (see 05_git_workflow.md).
- `RESOLVED` — VERIFIED_HARDWARE has been reached AND no further action
  is needed.
- `WONT_FIX` — deliberately deferred, with a one-line reason.
- `DEBUNKED` — the "bug" was shown not to exist; state exactly how you
  proved its nonexistence (not "recompiled cleanly," since that's
  evidence of absence of a *syntax* error only).
- Never use "successfully passed," "fully resolved," "confirmed working,"
  or similar informal phrasing in the log — always one of the terms above.

## Partial results are not passes
If a test suite has N defined sub-checks (a ROM test, a multi-assert unit
test, a checklist), and fewer than N completed with the suite's own
designed success signal (e.g. a final "Passed" banner, an exit code, a
specific terminal state), the result is `IN_PROGRESS`, not
`VERIFIED_HOST`. State the exact fraction (e.g. "9/11 sub-tests reached
their :ok marker; sub-test 10 did not complete within the allotted
budget"). Do not editorialize about how likely the remaining tests are to
pass. If a resource limit (frame budget, timeout, memory) is the
suspected cause of an incomplete run, that is itself something to fix and
rerun, not something to reason past.

## Banned language in status reports
No superlatives or narrative framing when reporting engineering results:
banned words/phrases include but aren't limited to "massive victory,"
"flawlessly," "rigorously proves," "crushed it," "perfect." Report what
ran, what it output, and what state that leaves the issue in. Neutral and
boring is correct — see also 09_testing_infrastructure.md's loading-message
tone guidance, same principle.

## Quoting code
When asked to show code, or when justifying a claim about existing code,
paste the literal current file content (with a fresh read in this
session) inside a fenced block with line numbers if available. A
description of code ("it just checks bounds and returns") is not a
substitute when the standing order calls for showing code — that
distinction is exactly what caught the semicolon hallucination.

## Numeric claims (counts, hit totals, percentages)
If you state a count derived from a search/grep/audit, and you later
restate that count, it must be identical or you must explicitly flag and
explain the discrepancy — do not let a number silently drift between
messages. If unsure, rerun the search rather than recalling the earlier
number.
