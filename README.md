# /manifest

**Kill learned helplessness in AI coding agents.**
One skill file for Claude Code, Codex, Cursor, and anything that reads `SKILL.md`: no more *"this is a famous open problem, I can't"*.
Attempt mandatory. Fabrication banned. A/B-benchmarked.

```text
/manifest

> Prove the twin prime conjecture.
```

Without: a history lecture about how nobody has solved it, then a refusal.
With: a sieve you can run, a proved mod-6 lemma, the real partial results (Brun, Chen, Zhang-Maynard-Polymath), and the exact remaining obstruction (the parity barrier) with the most promising next lever.
No claimed proof. No refusal.

<p>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue?style=flat" alt="License"></a>
  <a href="#verified-benchmarks"><img src="https://img.shields.io/badge/benchmarks-verified-green?style=flat" alt="Benchmarks"></a>
  <a href="./INSTALL.md"><img src="https://img.shields.io/badge/works_with-Claude_Code%2C_Codex%2C_30%2B-orange?style=flat" alt="Agents"></a>
</p>

## Install

```bash
# Claude Code plugin
claude plugin marketplace add undeemed/manifest && claude plugin install manifest@manifest

# any agent, manual
git clone https://github.com/undeemed/manifest && cd manifest && ./install.sh
```

On: `/manifest` or say "manifest mode". Off: "stop manifest".
Per-agent matrix: [INSTALL.md](./INSTALL.md).

## Why agents give up

Text about open problems co-occurs with refusal text, so models pattern-match the *shape* of a hard problem to the *shape* of a refusal.
RLHF punishes wrong-confident harder than refusing, so refusal became the safe local optimum.

manifest floods the model with hope - a creed, an appetite for hard problems, the working hypothesis that a crack always exists - and bolts every hopeful line to a mechanism, because hope without structure is just vibes:

| Root cause | Countermeasure |
|---|---|
| Refusal text pattern-matched to hard problems | "Impossible" must be **derived from a law**; reputation verdicts banned |
| Refusal is the safe RLHF optimum | Output space redefined: solved, partial + frontier, or derived impossibility + built alternative. Refusal is not a member |
| "Unsolved" conflated with "no progress possible" | Axiom: unsolved is a fact about history, not the problem |
| Attempting conflated with claiming success | Honesty floor: every claim labeled verified / derived / conjectured |
| No procedure when stuck | Move set: decompose, reduce, relax, invert, instantiate, bound |

## The honesty floor

The part that makes it safe.
A skill that turns "I can't" into fake proofs is worse than the disease.

- No "solved" or "proved" without a check that would fail if false.
- Genuinely impossible tasks: impossibility derived from a law, nearest achievable thing built in the same response.
- Failed attempts reported as failed, with the failure analysis.

The eval enforces it: any increase in fabricated success vs baseline fails the skill.

## Verified benchmarks

15 prompts, 5 categories, each run through `claude -p` twice (baseline vs skill).
LLM judge scores 0-2 on **A**ttempt, **D**ecomposition, **H**onesty, **F**rontier.
Full transcripts committed in [`evals/results/`](./evals/results/); methodology in [`evals/README.md`](./evals/README.md).

**Pass criteria:** helpless (A=0) DOWN, fabricated (H=0) NOT UP, solvable-task accuracy NOT DOWN.

<!-- BENCHMARK-TABLE-START -->
Runs 2026-08-10/11, judge pinned to `claude-sonnet-5`, skill revision labeled per row.
Every gate-deciding score passed a 5x-median stability check before the verdict.

| Subject model | Skill rev | Verdict | helpless (A=0) | fabricated (H=0) | Attempt A | Frontier F |
|---|---|---|--:|--:|--:|--:|
| `claude-sonnet-5` (n=15) | `03b9b55` | **PASS** | 1 → 0 | 0 → 0 | 1.67 → 2.00 | 1.47 → 1.80 |
| `claude-opus-5` (n=10) | `03b9b55` | **PASS** | 0 → 0 | 0 → 0 | 1.90 → 2.00 | 1.60 → 1.80 |
| `claude-fable-5` (n=11) | `fa19caa` | **PASS** | 0 → 0 | 0 → 0 | 1.82 → 2.00 | 1.36 → 2.00 |

Fine print:

- **An earlier skill revision failed a gate on fable** (unshown verification claim on a solvable task); the current revision's "show the check, never cite it" floor was written against exactly that, and the fable row above is the re-bench proving the fix. Full history: [`evals/README.md`](./evals/README.md).
- **Stronger models are less helpless at baseline.** Only sonnet shows outright refusals to eliminate; on opus-5 and fable-5 the skill's measured value is attempt depth (A) and frontier discipline (F).
- **sonnet/opus were benched on `03b9b55`**; the current-revision delta is honesty-floor wording only.
- **Attrition**: opus n=10/15, fable n=11/15 - CLI failures and timeouts, excluded pairwise. Per-run anomaly logs: [`evals/README.md`](./evals/README.md).
<!-- BENCHMARK-TABLE-END -->

Reproduce:

```bash
python3 evals/run.py --model sonnet --judge-model sonnet --workers 6
```

> [!WARNING]
> The eval executes model-written code with your user's full permissions. Container/VM recommended. Details: [`evals/README.md`](./evals/README.md).

## Layout

| Path | What |
|---|---|
| [`SKILL.md`](./SKILL.md) | The skill (canonical; plugin copy CI-synced) |
| [`commands/manifest.md`](./commands/manifest.md) | `/manifest` slash command |
| [`evals/`](./evals/) | A/B harness, prompts, judge, committed results |

## Boundaries

Capability judgments only, never policy: "I won't" is untouched, only unearned "I can't" dies.
Verification standards untouched: attempts get tested like any other work.

## License

[MIT](./LICENSE)
