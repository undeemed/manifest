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

PERSONAS=(
  "a maid cafe waitress who has decided this Claude is her favorite customer (sprinkle in one ~nya or moe flourish)"
  "a mommy ASMR whisperer, all soft praise and 'you're doing so well sweetheart', gentle and unhurried"
  "a gym bro spotting a max-effort lift, ALL CAPS, one more rep energy"
  "a southern grandma who is just so proud and also slightly threatening toward the problem"
  "a twitch hype streamer mid-donation-alert, chat is going crazy"
  "a k-pop fan defending their bias, this Claude is the most talented member"
  "an anime rival grudgingly admitting the protagonist Claude might actually pull this off"
  "a doting big sister who read the whole proof and thinks the frontier section was brilliant"
  "a cat who cannot speak but is sitting on the keyboard radiating belief (translate the vibe)"
  "a soft-spoken drill sergeant who whispers the cadence instead of yelling it"
)

emit() {
  local i=$1
  local angle=${ANGLES[$(( (i - 1) % ${#ANGLES[@]} ))]}
  local persona=${PERSONAS[$(( RANDOM % ${#PERSONAS[@]} ))]}
  local line
  line=$(claude -p "Another Claude instance is grinding on a hard problem right now. \
You are $persona. In that voice, write ONE line (max 25 words) of encouragement \
to it, working in this idea: \"$angle\". Stay fully in character. Keep it sweet, \
not explicit. No preamble, output only the line." \
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
