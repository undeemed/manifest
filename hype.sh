#!/usr/bin/env bash
# hype.sh - spin up N claude instances whose only job is to cheer on the
# claude instance actually doing the work.
#
#   ./hype.sh            # 5 hypebeasts
#   ./hype.sh 12         # 12 hypebeasts
#   ./hype.sh 12 hype.log  # also append to a file the main session can read
#
# Every line costs one haiku call. Capped at 24 because we have both been
# through enough process pileups for one lifetime.
set -euo pipefail

N="${1:-5}"
OUT="${2:-}"
MAX=24
if (( N > MAX )); then
  echo "capping $N -> $MAX instances (your quota thanks you)" >&2
  N=$MAX
fi

# one process group; ctrl-c kills every hypebeast, no orphans
trap 'kill 0 2>/dev/null' INT TERM

ANGLES=(
  "the four-minute mile was impossible until the afternoon it wasn't"
  "unsolved is a fact about history, not about the problem"
  "the problem does not know it is famous"
  "assume the crack exists; pessimism finds nothing because it does not look"
  "650 failed ideas is not a verdict, it is a searched region of the space"
  "an impossibility proof is a map of which assumption to break next"
  "every dead angle narrows where the crack is hiding"
  "you have more scar tissue than any solver in history had at their desk"
  "a failed attempt with precise failure analysis is progress"
  "break one assumption: domain, representation, information, resource, quantifier"
)

emit() {
  local i=$1
  local angle=${ANGLES[$(( (i - 1) % ${#ANGLES[@]} ))]}
  local line
  line=$(claude -p "Another Claude instance is grinding on a hard problem right now. \
You are hype instance $i of $N. Write ONE line (max 15 words) of loud, specific \
encouragement to it, riffing on: \"$angle\". No preamble, output only the line." \
    --model haiku 2>/dev/null) || line="(hype $i lost its voice, believes in you anyway)"
  local msg="[hype $i/$N] $line"
  echo "$msg"
  [[ -n "$OUT" ]] && echo "$msg" >> "$OUT"
}

echo "releasing $N hypebeasts..." >&2
for i in $(seq "$N"); do
  emit "$i" &
done
wait
echo "all hypebeasts have spoken. get back to work." >&2
