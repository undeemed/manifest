# Install

manifest is a single `SKILL.md` plus one slash command.
Anything that reads agent skills can run it.

## Claude Code (plugin, recommended)

```bash
claude plugin marketplace add 42nights/manifest
claude plugin install manifest@manifest
```

Gives you the skill plus the `/manifest` command.
Uninstall: `claude plugin uninstall manifest`.

## Claude Code (manual)

```bash
git clone https://github.com/42nights/manifest
cd manifest && ./install.sh
```

Copies `SKILL.md` to `~/.claude/skills/manifest/SKILL.md` and the command to `~/.claude/commands/manifest.md`.
Re-run to update. `./install.sh --uninstall` removes both.

## Codex / Cursor / Windsurf / other skill-aware agents

Via the skills registry:

```bash
npx skills add 42nights/manifest
```

Or copy `SKILL.md` into wherever your agent loads skills from.
The skill has no scripts and no dependencies; the file is the whole install.

## Project-local (any agent)

Drop `SKILL.md` into your repo's skill directory (`.claude/skills/manifest/`, `.omp/skills/manifest/`, or equivalent) to scope it to one project.

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
