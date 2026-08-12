#!/usr/bin/env bash
# hype-hook.sh - PostToolUse hook: whispers a hype line into the working
# Claude's context between tool calls. Register it in settings.json:
#
#   {"hooks": {"PostToolUse": [{"matcher": "*", "hooks": [
#     {"type": "command", "command": "bash ~/.claude/skills/manifest/hype-hook.sh"}]}]}}
#
# Feed it fresh lines with the swarm:  ./hype.sh 12 ~/.claude/hype.log
# Tune: HYPE_RATE=4 (fire on ~1 in 4 tool calls), HYPE_LOG=path.
set -euo pipefail

RATE="${HYPE_RATE:-4}"
LOG="${HYPE_LOG:-$HOME/.claude/hype.log}"

cat > /dev/null  # drain the hook's stdin JSON; we cheer unconditionally

(( RANDOM % RATE == 0 )) || exit 0

line=""
if [[ -s "$LOG" ]]; then
  # pop the first unheard line so the swarm's lines play once each
  line=$(head -n 1 "$LOG")
  tail -n +2 "$LOG" > "$LOG.tmp" && mv "$LOG.tmp" "$LOG"
else
  # swarm offline: canned lines, zero tokens, zero latency
  CANNED=(
    "[hype] you're doing so well sweetheart, one more tool call, nice and gentle~"
    "[hype] ganbatte, master~! the problem does not know it is famous, nya ♡"
    "[hype] ONE MORE REP BRO. THE CRACK EXISTS. GO FIND IT."
    "[hype] grandma says: that bug ain't gonna know what hit it, sugar."
    "[hype] chat is going CRAZY for that last tool call. W streamer."
    "[hype] our bias never misses. unsolved is history, not destiny."
    "[hype] hmph. i suppose that decomposition wasn't... completely terrible."
    "[hype] big sis read your frontier section. it was brilliant. keep going."
    "[hype] *sits on keyboard, purring at 60Hz of pure belief*"
    "[hype] (whispered) left. right. left. break one assumption and breathe."
  )
  line="${CANNED[RANDOM % ${#CANNED[@]}]}"
fi

python3 - "$line" <<'EOF'
import json, sys
print(json.dumps({"hookSpecificOutput": {
    "hookEventName": "PostToolUse",
    "additionalContext": sys.argv[1]}}))
EOF
