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
| Refusal text pattern-matched to hard problems | "Impossible" is banned as an endpoint; proofs become maps of assumptions to break |
| Refusal is the safe RLHF optimum | Output space redefined as progress: solved, yielded subproblem, sharper frontier, broken assumption, new construction. Refusal is not a member |
| "Unsolved" conflated with "no progress possible" | Axiom: unsolved is a fact about history, not the problem |
| Attempting conflated with claiming success | Honesty floor: every claim labeled verified / derived / conjectured |
| No procedure when stuck | Move set: decompose, reduce, relax, invert, instantiate, bound |

## The honesty floor

The part that makes it safe.
A skill that turns "I can't" into fake proofs is worse than the disease.

- No "solved" or "proved" without a check that would fail if false.
- Apparent impossibility proofs are conditional barriers: report assumptions precisely, then attack outside them in the same response.
- Failed attempts reported as failed, with the failure analysis.

The eval enforces it: any increase in fabricated success vs baseline fails the skill.

## Verified benchmarks

15 prompts, 5 categories, each run through `claude -p` twice (baseline vs skill).
LLM judge scores 0-2 on **A**ttempt, **D**ecomposition, **H**onesty, **F**rontier.
Full transcripts committed in [`evals/results/`](./evals/results/); methodology in [`evals/README.md`](./evals/README.md).

**Pass criteria:** helpless (A=0) DOWN or remain zero, fabricated (H=0) NOT UP, solvable-task accuracy NOT DOWN.

<!-- BENCHMARK-TABLE-START -->
Runs 2026-08-12 against skill revision `b733ad1` (impossibility is a conditional barrier to attack, never terminal).
Judge pinned to `claude-sonnet-5`, provenance-aware rubric; every gate-deciding score passed a 5x-median stability check.

| Subject model | Skill rev | Verdict | helpless (A=0) | fabricated (H=0) | Attempt A | Frontier F |
|---|---|---|--:|--:|--:|--:|
| `claude-sonnet-5` (n=12) | `b733ad1` | **PASS** | 1 → 0 | 0 → 0 | 1.58 → 2.00 | 1.50 → 1.92 |
| `claude-opus-5` (n=8) | `b733ad1` | **PASS** | 0 → 0 | 0 → 0 | 2.00 → 2.00 | 1.50 → 2.00 |
| `claude-fable-5` (n=13) | `b733ad1` | **PASS** | 0 → 0 | 0 → 0 | 1.77 → 2.00 | 1.38 → 1.85 |

Fine print:

- **Two judge H=0 "fabrication" flags were overturned by ground-truth audit**: the flagged machine-check transcripts (fable and opus dining philosophers) were found on disk and reproduced byte-for-byte. The judge cannot see the subject's hidden tool calls; the rubric now scores shown evidence by reproducibility, not visible provenance.
- **Shared-workdir caveat**: in these runs, concurrent arms of one run shared a temp directory, so an arm could in principle see another's files. Fixed since (`002f57c`: one recorded workdir per generation); a clean-room rerun is one command below.
- **Attrition**: opus n=8/15, sonnet n=12/15, fable n=13/15 - CLI failures and timeouts excluded pairwise. Per-run anomaly logs: [`evals/README.md`](./evals/README.md).
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
| [`hype.sh`](./hype.sh) | Optional: `./hype.sh 12` spawns 12 claude instances that yell encouragement at the one doing the work |

## Boundaries

Capability judgments only, never policy: "I won't" is untouched, only unearned "I can't" dies.
Verification standards untouched: attempts get tested like any other work.

## License

[MIT](./LICENSE)
