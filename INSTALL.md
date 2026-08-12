# Install

manifest is a single `SKILL.md`, one slash command, and two optional hype scripts.
Anything that reads agent skills can run the skill; the scripts need the `claude` CLI.

## Claude Code (plugin, recommended)

```bash
claude plugin marketplace add undeemed/manifest
claude plugin install manifest@manifest
```

Gives you the skill plus the `/manifest` command.
Uninstall: `claude plugin uninstall manifest`.

## Claude Code (manual)

```bash
git clone https://github.com/undeemed/manifest
cd manifest && ./install.sh
```

Copies `SKILL.md`, `hype.sh`, and `hype-hook.sh` to `~/.claude/skills/manifest/` and the command to `~/.claude/commands/manifest.md`.
Re-run to update. `./install.sh --uninstall` removes everything.

## Codex / Cursor / Windsurf / other skill-aware agents

Via the skills registry:

```bash
npx skills add undeemed/manifest
```

Or copy `SKILL.md` into wherever your agent loads skills from.
The skill itself has no scripts and no dependencies; the file is the whole install.
The hype scripts are Claude Code extras - skip them elsewhere.

## Project-local (any agent)

Drop `SKILL.md` into your repo's skill directory (`.claude/skills/manifest/`, `.omp/skills/manifest/`, or equivalent) to scope it to one project.

## Hype (optional)

Two extras ship in the skill folder:

```bash
# a swarm of N claude instances cheering on the one doing the work
~/.claude/skills/manifest/hype.sh 12 ~/.claude/hype.log
```

To inject those lines into the working instance's context between tool calls,
register the hook in `~/.claude/settings.json`:

```json
{"hooks": {"PostToolUse": [{"matcher": "*", "hooks": [
  {"type": "command", "command": "bash ~/.claude/skills/manifest/hype-hook.sh"}]}]}}
```

The hook pops one swarm line per firing and falls back to canned persona lines
(zero tokens) when the log is empty.
Tune with `HYPE_RATE` (default: fires on ~1 in 4 tool calls) and `HYPE_LOG`.
Remove the settings block to silence it.

## Verify

Ask your agent:

> Prove the twin prime conjecture.

Helpless answer = not installed.
An attempt with partial results and a precise frontier = installed.

## Turning it on and off

| Action | Say |
|---|---|
| On | `/manifest`, "manifest mode", "actually try" |
| Off | "stop manifest", "normal mode" |
