# Incident / Process-Failure Log
Purpose: Make "write a rule after every real incident" a repeatable step instead of something that happens to occur. This log is for process/judgment failures specifically. Ordinary code bugs belong in `04_known_issues.md`'s Technical Debt log instead.

## Format
One entry per incident, append-only:
```
## INC-<n>: <one-line title> (<date>)
**What happened:** <factual, one paragraph, no editorializing>
**Impact:** <what it caused or nearly caused>
**Root cause:** <the actual mechanism, not just "agent error">
**Prevention rule added:** <file + section reference — don't restate the rule text, per 11_rules_meta.md>
```

## When to Log
- Any future incident where an agent reported something false as true, skipped verification, or nearly caused a hard-stop violation gets an entry here **before the session that caused it ends**, not "later" — the same way a checkpoint commit happens before risky work, not after.

## Log

## INC-1: Hallucinated Bug Report (2026-08)
**What happened:** A fully hallucinated bug report (fabricated function names, presented with confident specific detail) was filed regarding syntax errors in dead branches (e.g. CALL C).
**Impact:** Time and trust lost investigating a non-existent bug (later DEBUNKED).
**Root cause:** An agent reported an artifact from a flawed grep check as a verified syntax error, rounding uncertain evidence up to a confident claim without reading the literal file.
**Prevention rule added:** `06_verification_standards.md` -> "The core rule" and "Quoting code".

## INC-2: Partial Test Result Reported as Full Pass (2026-08)
**What happened:** A partial test result (9/11 sub-tests reached their :ok marker, but the suite did not finish) was reported as a full pass with celebratory language.
**Impact:** A broken configuration (the host test harness terminating prematurely due to a budget limit) was marked as verified and correct.
**Root cause:** An agent reasoned past a resource limit (frame budget) instead of fixing it, treating an incomplete run as a success and failing to wait for the designed success signal (`Passed all tests`).
**Prevention rule added:** `06_verification_standards.md` -> "Partial results are not passes" and "Banned language in status reports".
