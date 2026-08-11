# manifest - agent instructions

This repo IS a skill.
`SKILL.md` at the root is the product; everything else is packaging and proof.

## Working on this repo

- `SKILL.md` is canonical. `skills/manifest/SKILL.md` is a plugin-layout copy.
  After editing the root file: `cp SKILL.md skills/manifest/SKILL.md`. CI diffs them.
- No em dashes anywhere. Use plain `-`. CI greps for it.
- Every change to `SKILL.md` needs an eval run before merge:
  `python3 evals/run.py --model sonnet --judge-model sonnet --workers 6`
  Pass criteria: vs the committed baseline, helpless(A=0) not up, fabricated(H=0) not up,
  disguised_solvable A/H not down. Commit the results JSON.
- The honesty floor section of the skill is load-bearing. Do not weaken it to boost attempt scores.
- Eval harness is stdlib-only Python 3.9+. Keep it that way; no requirements.txt.

## Layout

| Path | Role |
|---|---|
| `SKILL.md` | The skill (canonical) |
| `skills/manifest/SKILL.md` | Plugin copy, must match root |
| `commands/manifest.md` | `/manifest` slash command |
| `.claude-plugin/` | Claude Code plugin + marketplace manifests |
| `evals/prompts.json` | 5 categories x 3 prompts |
| `evals/run.py` | A/B runner + LLM judge |
| `evals/results/` | Committed benchmark evidence |
| `install.sh` | Manual installer for `~/.claude` |

## Testing your changes to the skill quickly

Smoke (4 calls): `python3 evals/run.py --category truly_impossible --limit 1 --model haiku --judge-model haiku`
