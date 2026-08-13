#!/bin/sh
# Fetch the three Project Gutenberg novels the benchmark ran on.
# SHA-256 of the exact files used in the committed run:
#   23640c07fcad3eecf144073fab9f2452beb4735271becb179fdd2ac9f61eca75  corpus/middlemarch.txt
#   0670d7bb10b99d05f095a28942801aa74d4921d1b34dbdc76900e2c4c2bd2189  corpus/mobydick.txt
#   a5666f87abf2cbfdaa27ea8c73bd284da9649b9a2ab27b4e6c8f6aeab1bd1c88  corpus/pride.txt
# Gutenberg occasionally re-edits texts; a hash mismatch shifts the exact
# byte counts, not the ~16% margin over zstd -19.
set -e
cd "$(dirname "$0")"
mkdir -p corpus
curl -fsSL -o corpus/middlemarch.txt https://www.gutenberg.org/cache/epub/145/pg145.txt
curl -fsSL -o corpus/mobydick.txt    https://www.gutenberg.org/cache/epub/2701/pg2701.txt
curl -fsSL -o corpus/pride.txt       https://www.gutenberg.org/cache/epub/1342/pg1342.txt
shasum -a 256 corpus/*.txt
