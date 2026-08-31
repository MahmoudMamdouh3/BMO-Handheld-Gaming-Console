# 40. AI Knowledge Graph, AST Indexing & Decision Matrix Protocol

**Purpose:** Autonomous AI agents and LLMs operating in this repository require deterministic, ground-truth navigation to prevent hallucinations, duplicate work, and linear scans across 1.35 million lines of code. This rule establishes the protocol for maintaining the machine-readable **AI Knowledge Graph (`AGENT_KNOWLEDGE_GRAPH.json`)** and **Zero-Shot Decision Matrix (`AGENT_DECISION_TREE.json`)**.

---

## 1. Machine-Readable Knowledge Architecture

The repository maintains an automated, self-indexing intelligence layer designed to enable any AI model (from compact local LLMs to massive frontier models) to reason about the firmware with zero latency:

```
+-----------------------------------------------------------------------------------+
| BMO Machine-Readable Intelligence Topology                                        |
|                                                                                   |
| [Root Entry Points]                                                               |
|   ├── AGENT_KNOWLEDGE_GRAPH.json  <- Complete 786-file AST index, symbols, pins    |
|   ├── AGENT_DECISION_TREE.json    <- Zero-shot task intent routing matrix          |
|   ├── AGENT_MANIFEST.json         <- Platform matrix, flash budget, build health   |
|   └── .agents/rules/CONTEXT_INDEX.json <- Task-to-rule fast lookup index          |
|                                                                                   |
| [Compilation Engine]                                                              |
|   └── scripts/generate_ai_knowledge_base.py (triggered via `python -m tools.guardian index`) |
+-----------------------------------------------------------------------------------+
```

---

## 2. Mandatory Rules for All AI Agents

### Rule 40.1: Mandatory Index Synchronization
Whenever an agent:
1. Adds, renames, or deletes any source file in `firmware/`, `tools/`, `scripts/`, or `tests/`.
2. Modifies public function signatures, classes, or macro definitions.
3. Alters hardware pin assignments or memory allocation schemes.
4. Completes performance optimizations or benchmark evaluations.

The agent **MUST** execute the knowledge base compilation pipeline before concluding:
```powershell
python -m tools.guardian index
```

### Rule 40.2: Zero-Shot Intent Routing
Before performing broad directory searches or file scans, an agent **MUST** query `AGENT_DECISION_TREE.json` or `CONTEXT_INDEX.json`.
- Lookup the active task intent (e.g. `modify_display_rendering_or_spi`, `add_or_modify_emulator_core`, `bake_rom_into_flash`).
- Read the exact `mandatory_rules` specified.
- Target the exact `primary_files` listed.
- Execute the specified `verification_command`.

### Rule 40.3: Ground-Truth Telemetry Derivation
All symbol signatures, memory section allocations (`IRAM`, `DRAM`, `PSRAM`, `FLASH`), line-by-line metrics, and latency budgets in generated artifacts **MUST** be derived directly from the Guardian engine (`tools/guardian/`). Never fabricate or estimate performance numbers.

---

## 3. Pre-Handoff Verification Checklist

```
[ ] Ran `python -m tools.guardian index` -> regenerated AGENT_KNOWLEDGE_GRAPH.json and AGENT_DECISION_TREE.json.
[ ] Verified AGENT_MANIFEST.json and CONTEXT_INDEX.json are 100% synchronized.
[ ] Ran `python scripts/validate_repo.py` -> 0 errors across all phases.
[ ] Ran `python -m unittest discover tests` -> all unit tests passing.
```
