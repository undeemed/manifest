#!/usr/bin/env bash
# manifest manual installer: copies the skill + command into ~/.claude.
# Usage: ./install.sh [--uninstall]
set -euo pipefail

SRC="$(cd "$(dirname "$0")" && pwd)"
SKILL_DST="$HOME/.claude/skills/manifest"
CMD_DST="$HOME/.claude/commands"

if [[ "${1:-}" == "--uninstall" ]]; then
  rm -rf "$SKILL_DST"
  rm -f "$CMD_DST/manifest.md"
  echo "manifest removed."
  exit 0
fi

mkdir -p "$SKILL_DST" "$CMD_DST"
cp "$SRC/SKILL.md" "$SKILL_DST/SKILL.md"
cp "$SRC/commands/manifest.md" "$CMD_DST/manifest.md"

echo "manifest installed:"
echo "  $SKILL_DST/SKILL.md"
echo "  $CMD_DST/manifest.md"
echo "Turn on: /manifest  |  Off: 'stop manifest'"
