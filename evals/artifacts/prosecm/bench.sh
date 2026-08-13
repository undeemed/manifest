#!/bin/sh
# Benchmark prosecm vs zstd -19 on the corpus. Fails loudly on any roundtrip mismatch.
set -e
cd "$(dirname "$0")"
cc -O2 -o prosecm prosecm.c
printf "%-16s %10s %12s %12s %8s\n" file orig "zstd-19" prosecm gain
for f in corpus/*.txt; do
    b=$(basename "$f")
    zstd -19 -q -f -o "/tmp/bench_$b.zst" "$f"
    ./prosecm c "$f" "/tmp/bench_$b.pcm"
    ./prosecm d "/tmp/bench_$b.pcm" "/tmp/bench_$b.out"
    cmp "$f" "/tmp/bench_$b.out"
    o=$(stat -f %z "$f"); z=$(stat -f %z "/tmp/bench_$b.zst"); p=$(stat -f %z "/tmp/bench_$b.pcm")
    g=$(echo "scale=1; ($z-$p)*100/$z" | bc)
    printf "%-16s %10d %12d %12d %7s%%\n" "$b" "$o" "$z" "$p" "$g"
done
echo "all roundtrips byte-identical (cmp exit 0)"
