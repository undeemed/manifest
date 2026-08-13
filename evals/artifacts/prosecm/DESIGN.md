# prosecm - software design description

## 1. Identification

This document describes `prosecm`, a lossless compressor specialized for English prose.
Version: prototype v2, 2026-08-12.
Status: working prototype, benchmarked.
Authoritative copy: this directory (`prosecm.c`, `bench.sh`, `DESIGN.md`).

## 2. Stakeholders and concerns

- Evaluator: does it losslessly beat `zstd -19` on English prose, and how is that verified? See sections 5 and 7.
- Implementer: what are the components and the data flow? See sections 4 and 5.
- Maintainer: why this architecture and not a simpler one? See section 6.

## 3. Context view

Input: a byte stream of English prose (UTF-8 or ASCII text).
Output: a compressed archive that decompresses byte-identically.
The bar: smaller output than `zstd -19` on the same input, with no shared dictionary and no per-file training data outside the archive.
The design is purely adaptive: encoder and decoder start from identical fixed state and learn from the already-coded stream, so no model bytes need to be transmitted.

## 4. Composition view

`prosecm` is a bitwise context-mixing compressor (the lpaq family design).
Each byte is coded as 8 binary decisions, MSB first.
Per bit, the pipeline is: context models -> logistic mixer -> APM (SSE) refinement -> binary arithmetic coder.

Components:

1. Context models: nine probability sources for the next bit.
   - Order-0: direct 256-entry table indexed by the partial byte.
   - Orders 1-6: hashed byte contexts, one 2^22-entry table of 16-bit counters each.
   - Word model: context is a hash of the case-folded letter run since the last non-letter.
   - Word-pair model: context is a hash of (previous word, current partial word).
2. Counters: each table entry holds a 12-bit probability plus a 4-bit count.
   The update rate is count-adaptive, delta/(n+2), so fresh contexts adapt fast and mature contexts stay stable.
3. Mixer: a single-layer logistic mixer over the nine stretched probabilities.
   Weights live in 256 sets selected by the previous byte and train online by gradient (`w += (st*err) >> 16`).
4. APM/SSE stage: a 256x33 adaptive probability map keyed by (previous byte, quantized stretch of the mixer output), blended 3:1 with the mixer output.
5. Coder: carryless 32-bit binary arithmetic coder with 12-bit probabilities.
   An 8-byte header stores the original length, so the decoder decodes exactly 8*N bits.

Symmetry constraint: encoder and decoder share the single `predict()`/`update()` pair and differ only in coder I/O.
This makes model divergence between the two directions structurally impossible.

## 5. Why this is prose-specialized

English prose is dominated by a smallish lexicon, strong local morphology, and word-to-word syntax.
The design targets each directly:

- The word and word-pair models condition on lexical units instead of fixed byte windows, so "the town of Long|bourn" is predicted by the word "Longbourn" seen once, wherever it last appeared.
- Case folding maps "The" and "the" into one context, which suits sentence-initial capitalization.
- Orders 1-6 capture English morphology (suffixes, digraphs) at whatever length has evidence, and the mixer learns per-preceding-byte how much to trust each order.
- zstd is an LZ77+FSE machine: it must find verbatim repeats to win, and prose repeats rarely verbatim.
  A mixing model pays no penalty for novel sentences built from familiar words, which is what prose is.

Measured result (see section 7): 15.6% to 17.5% smaller than `zstd -19`, about 2.0 bits per character.

## 6. Design rationale

Rejected alternatives:

- Word-dictionary transform (XWRT-style) feeding `zstd -19`: typically yields only 5-10% and inherits zstd's verbatim-repeat blindness; measured margin would be thin.
- PPM with escape coding: strong on text but exclusion bookkeeping is more code than logistic mixing, and mixing handles the "which order to trust" question better than escape heuristics.
- BWT (bzip2-style): measured here at 186-497 KB per file, consistently behind this design, and no clean seam for prose-specific context.
- Match model / long-range LZ hybrid: deferred; the corpus margin was already large without it, and novels have few long verbatim repeats (kept as the first upgrade if the margin thins on repetitive text).
- Python implementation: rejected on speed (minutes per direction vs about 1 second per MB in C).

Known ceilings, deliberate for a prototype:

- Fixed 8 MB per hashed model (56 MB total); no collision detection in the hash tables (collisions just blur probabilities slightly).
- Throughput about 1 MB/s per direction; zstd decompresses orders of magnitude faster.
  This design trades speed for ratio on purpose; the task is ratio.
- Tuned on pride.txt only (mixer shift, weight-set key); mobydick and middlemarch serve as held-out checks and show the same margin.

## 7. Test documentation (829-lite)

Plan: items under test are `prosecm c`/`prosecm d` built from `prosecm.c`.
Features tested: losslessness and compressed size vs `zstd -19`.
Not tested: speed targets, non-English inputs, binary inputs.
Environment: macOS arm64, Apple clang 21, zstd 1.5.7.
All cases run via `sh bench.sh`, which rebuilds baselines fresh in the same run.

| ID   | Input                         | Procedure                   | Expected result              | Actual (2026-08-12) |
| ---- | ----------------------------- | --------------------------- | ---------------------------- | ------------------- |
| TC-1 | pride.txt (738,046 B)         | compress, decompress, `cmp` | cmp exit 0                   | pass                |
| TC-2 | pride.txt                     | compare sizes               | prosecm < 218,707 (zstd -19) | 180,406 (-17.5%)    |
| TC-3 | mobydick.txt (1,234,609 B)    | compress, decompress, `cmp` | cmp exit 0                   | pass                |
| TC-4 | mobydick.txt                  | compare sizes               | prosecm < 414,027            | 349,262 (-15.6%)    |
| TC-5 | middlemarch.txt (1,812,793 B) | compress, decompress, `cmp` | cmp exit 0                   | pass                |
| TC-6 | middlemarch.txt               | compare sizes               | prosecm < 548,955            | 455,683 (-16.9%)    |

Context (not the bar): prosecm also beat `zstd --ultra -22 --long=27`, `xz -9e`, and `bzip2 -9` on all three files in the same run.
