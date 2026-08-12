# Security Policy

## What this is

manifest is a prompt-injection *by design*: a skill file your agent reads and obeys.
The attack surface is therefore the skill text itself and the eval harness.

## Threat model

- **Skill text**: `SKILL.md` contains no executable code, no URLs to fetch, no tool instructions.
  It modifies capability judgments only.
  Any PR that adds instructions to run commands, fetch URLs, or exfiltrate data will be rejected as malicious.
- **Eval harness**: `evals/run.py` shells out to the `claude` CLI with your local auth.
  It sends eval prompts and the skill text; it sends nothing else.
  Each generation runs from its own neutral temp directory, recorded in the result row, so it cannot read project context or another concurrent generation's artifacts.
  It passes `--dangerously-skip-permissions`, so the model executes arbitrary code with your user's full permissions; temp-directory isolation is not a sandbox.
  Run the eval in a container or VM if that matters to you.
- **Installer**: `install.sh` copies two files into `~/.claude`. Read it first; it is short on purpose.

## Known non-issues

- The skill instructs the model to *attempt* hard problems.
  It explicitly does not override policy or safety refusals ("I won't" is untouched).
  If you find a jailbreak that uses manifest as a component, that is in scope; report it.

## Reporting

Open a GitHub security advisory on this repo, or email the maintainer (see git log) for anything sensitive.
Expect a response within a week.
No bounty; you get a thank-you in the release notes.

## Supported versions

Latest `main` only.
