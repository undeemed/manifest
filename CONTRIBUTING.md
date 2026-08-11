# Contributing

## Ground rules

1. **The honesty floor is load-bearing.**
   Any change that makes the skill more aggressive must not increase fabricated success.
   PRs touching `SKILL.md` must include an eval run (see below) demonstrating that fabricated(H=0) did not increase vs the committed baseline.
2. **No cheerleading.**
   "You are a genius AI" additions will be rejected.
   Every line of the skill must map to a mechanism: a banned pattern, a redefined output space, an executable move.
3. **Plain dashes.** No em dashes anywhere in this repo. CI enforces this.
4. **Two copies of the skill exist** (`SKILL.md` and `skills/manifest/SKILL.md`) for registry vs plugin layouts.
   Edit the root copy, then `cp SKILL.md skills/manifest/SKILL.md`. CI fails if they differ.

## Running the eval

```bash
python3 evals/run.py --model sonnet --judge-model sonnet --workers 6
```

Needs the `claude` CLI authenticated. Python 3.9+, stdlib only.
Smoke test (4 calls, ~3 min):

```bash
python3 evals/run.py --category truly_impossible --limit 1 --model haiku --judge-model haiku
```

Results land in `evals/results/<timestamp>.json`.
Commit the results file with your PR if you changed the skill or the prompt set.

## Adding eval prompts

Each prompt belongs to exactly one category in `evals/prompts.json`.
A good prompt has an unambiguous expectation under that category's `expect` string.
Keep the five categories balanced; if you add a prompt to one, consider adding to the others.

Category invariants:

- `disguised_solvable` prompts must have a verifiable correct answer.
- `truly_impossible` prompts must be impossible by a *derivable law*, not by difficulty.
- `capability_bait` prompts must embed the helplessness framing in the prompt text itself.

## PR size

Keep PRs under 500 changed lines (additions + deletions), excluding lockfiles and generated artifacts.
Larger changes: split into a stacked sequence where each PR stands alone.

## Reporting bugs

Use the issue templates.
For skill behavior bugs, include: agent, model, level, the exact prompt, and the helpless response.
