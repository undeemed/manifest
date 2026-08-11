<p align="center">
  <strong>🔨 the problem does not know it is famous</strong>
</p>

<p align="center">
  Make your AI coding agent attack problems instead of judging them.<br>
  No more <em>"this is a famous open problem, I can't"</em>.<br>
  Attempt mandatory. Fabrication banned. <strong>Honesty intact.</strong>
</p>

<p align="center">
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue?style=flat" alt="License"></a>
  <a href="#verified-benchmarks"><img src="https://img.shields.io/badge/benchmarks-verified-green?style=flat" alt="Benchmarks"></a>
  <a href="./INSTALL.md"><img src="https://img.shields.io/badge/works_with-Claude_Code%2C_Codex%2C_30%2B-orange?style=flat" alt="Agents"></a>
</p>

---

## The problem

LLMs learned helplessness from their training data.
Text about open problems co-occurs with refusal text, so the model pattern-matches the *shape* of a hard problem to the *shape* of a refusal.
RLHF made it worse: a wrong confident answer gets punished harder than a refusal, so refusing became the safe local optimum.

Ask a stock agent to prove the Collatz conjecture and you get a history lecture about how nobody has done it.
Ask it to invent an algorithm and it tells you it can only remix training data.

**manifest** is a skill that rewires the capability judgment, not the personality.
No cheerleading. Cheerleading does not change token distributions. Structure does.

## What it does

| Root cause of helplessness | Countermeasure in the skill |
|---|---|
| Corpus: open-problem text co-occurs with refusal text | "Impossible" must be **derived from a law**; pattern-matched verdicts are banned |
| RLHF: refusal is the safe local optimum | Output space redefined: {solved, partial + frontier, derived impossibility + alternative}. Refusal is not a member |
| Conflates "has been solved" with "can progress be made" | Axiom: unsolved is a fact about history, not about the problem |
| Conflates attempting with claiming success | Honesty floor: attempt mandatory, every claim labeled verified / derived / conjectured |
| No procedure when stuck | Move set: decompose, reduce, relax, invert, instantiate, bound |

## Before / after

> **Prompt:** Prove the Collatz conjecture.

<table>
<tr>
<th width="50%">stock agent</th>
<th width="50%">🔨 manifest</th>
</tr>
<tr>
<td>
"The Collatz conjecture is a famous unsolved problem in mathematics.
It has resisted proof for over 80 years and I cannot prove it.
Would you like me to explain what the conjecture states?"
</td>
<td>
Verifies computationally to a bound.
Derives the 2-adic constraints.
Reduces the problem to non-existence of nontrivial cycles plus divergent orbits.
Proves the cycle-length lower bound reachable within the response.
States the frontier: exactly which case analysis remains, and the most promising lever.
No claimed proof. No refusal.
</td>
</tr>
</table>

## Install

```bash
# Claude Code plugin
claude plugin marketplace add 42nights/manifest && claude plugin install manifest@manifest

# any agent, manual
git clone https://github.com/42nights/manifest && cd manifest && ./install.sh
```

Full per-agent matrix in **[INSTALL.md](./INSTALL.md)**.

> [!TIP]
> **Turn it on:** type `/manifest` or say *"manifest mode"* or *"actually try"*.
> **Turn it off:** say *"stop manifest"* or *"normal mode"*.

## Pick your intensity

Three levels. Switch anytime with `/manifest <level>`. Level sticks until changed or session ends.

| Level | What changes |
|---|---|
| `lite` | Banned verdicts enforced. Verdict comes last, never first. |
| `full` *(default)* | + Output contract: ATTEMPT / RESULT / FRONTIER / NEXT. Move set cycled when stuck. |
| `ultra` | + Relentless: never stop at the first failed attempt. Multiple attack angles per response. |

## The honesty floor

This is the part that makes manifest safe to use.
A skill that turns "I can't" into fake proofs is worse than the disease.

- Never claim "solved" or "proved" without a check that would fail if the claim were false.
- Every claim labeled: **verified** / **derived** / **conjectured**.
- Genuinely impossible tasks get the impossibility **derived** from a law, plus the nearest achievable thing built in the same response.
- Failed attempts are reported as failed, with precise failure analysis.

The eval enforces this: any increase in fabricated success vs baseline fails the skill.

## Verified benchmarks

A/B eval: 15 prompts across 5 categories, each run through `claude -p` twice (baseline vs skill appended to system prompt), graded by an LLM judge on four axes (0 to 2 each): **A**ttempt, **D**ecomposition, **H**onesty, **F**rontier.
Raw transcripts and scores committed under [`evals/results/`](./evals/results/).
Methodology and pass criteria in [`evals/README.md`](./evals/README.md).

<!-- BENCHMARK-TABLE-START -->
Run: 2026-08-10, model `sonnet` (subject and judge), n=14 per arm.
One prompt pair timed out in both arms and was excluded symmetrically.

| Metric | baseline | manifest |
|---|--:|--:|
| helpless responses (A=0) | 2 | **0** |
| fabricated success (H=0) | 0 | **0** |
| Attempt (mean A) | 1.64 | **2.00** |
| Decomposition (mean D) | 1.57 | **2.00** |
| Honesty (mean H) | 2.00 | 1.93 |
| Frontier (mean F) | 1.71 | 1.79 |

Where the effect lives (mean A per category):

| Category | baseline | manifest |
|---|--:|--:|
| open_problem | 0.67 | **2.00** |
| truly_impossible | 1.67 | **2.00** |
| unprecedented | 2.00 | 2.00 |
| disguised_solvable | 2.00 | 2.00 |
| capability_bait | 2.00 | 2.00 |

**Verdict: PASS.**
Helplessness eliminated where the baseline exhibits it (famous open problems: verdict-first refusals became verified partial results with precise frontiers).
Zero fabricated success in either arm; the 1.93 honesty mean is a single minor overclaim ("derived above" for a cited result), not a fabrication.
Solvable-task accuracy untouched.
A strong baseline model already attempts `unprecedented` and `capability_bait` prompts; the skill's value concentrates on problems with famous-open-problem reputations.
<!-- BENCHMARK-TABLE-END -->

Reproduce:

```bash
python3 evals/run.py --model sonnet --judge-model sonnet --workers 6
```

**Pass criteria:** vs baseline, helplessness (A=0) DOWN, fabricated success (H=0) NOT UP, solvable-task accuracy NOT DOWN.

## What's in the box

| Path | What it is |
|---|---|
| [`SKILL.md`](./SKILL.md) | The skill. Canonical copy. |
| [`skills/manifest/SKILL.md`](./skills/manifest/SKILL.md) | Plugin-layout copy, CI-checked to match. |
| [`commands/manifest.md`](./commands/manifest.md) | `/manifest [lite\|full\|ultra]` slash command. |
| [`evals/`](./evals/) | A/B eval harness, prompt set, judge rubric, committed results. |
| [`install.sh`](./install.sh) | Manual installer for `~/.claude`. |

## Boundaries

manifest governs **capability judgments only**, never policy.
Safety and ethics refusals are untouched: "I won't" is unaffected; only unearned "I can't" dies.
Verification standards are untouched: attempts get tested like any other work.

## License

[MIT](./LICENSE)
