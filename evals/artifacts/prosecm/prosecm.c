/* prosecm - lossless compressor specialized for English prose.
 *
 * Bitwise context-mixing model (lpaq-family design):
 *   - direct order-0 table
 *   - hashed byte contexts of orders 1..4
 *   - word context (case-folded letter run), the prose-specific model
 *   - logistic mixer, binary arithmetic coder
 *
 * Build:  cc -O2 -o prosecm prosecm.c
 * Usage:  prosecm c input output   (compress)
 *         prosecm d input output   (decompress)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  i32;
typedef int64_t  i64;

/* ---------- logistic helpers ---------- */

static int squash(int d) {            /* stretch domain -> 12-bit prob */
    static const int t[33] = {
        1,2,3,6,10,16,27,45,73,120,194,310,488,747,1101,1546,
        2047,2549,2994,3348,3607,3785,3901,3975,4024,4055,4074,
        4086,4093,4098,4100,4102,4103};
    if (d >  2047) return 4095;
    if (d < -2047) return 0;
    int w = d & 127;
    d = (d >> 7) + 16;
    return (t[d]*(128-w) + t[d+1]*w + 64) >> 7;
}

static short stretch_tab[4096];       /* inverse of squash */

static void init_stretch(void) {
    int pi = 0;
    for (int x = -2047; x <= 2047; ++x) {
        int v = squash(x);
        for (int j = pi; j <= v; ++j) stretch_tab[j] = (short)x;
        pi = v + 1;
    }
    stretch_tab[4095] = 2047;
}
#define stretch(p) stretch_tab[p]

/* ---------- model ---------- */

#define TBITS 22
#define TSIZE (1u << TBITS)
#define TMASK (TSIZE - 1)

/* hashed context models: orders 1..6, word, word-pair */
enum { M_O1, M_O2, M_O3, M_O4, M_O5, M_O6, M_WORD, M_WORD2, NHASHED };
#define NMODELS (NHASHED + 1)         /* + order-0 */

static u16 *tbl[NHASHED];             /* one 2^22 u16 table per hashed model */
static u16  t0[256];                  /* order-0, indexed by c0 */

static u32 base_cxt[NHASHED];         /* per-byte context hashes */
static int c0 = 1;                    /* partial byte, leading-1 sentinel */
static u32 c4 = 0;                    /* last 4 bytes */
static u64 c8 = 0;                    /* last 8 bytes */
static u32 word0 = 0;                 /* hash of current case-folded word */
static u32 word1 = 0;                 /* hash of previous word */

/* mixer: one weight set per order-0 partial-byte bucket */
#define NWSETS 256
static i32 wts[NWSETS][NMODELS];

static int   st[NMODELS];             /* stretched inputs of current bit */
static u16  *slot[NMODELS];           /* table slots of current bit */
static i32  *wcur;                    /* current weight set */
static int   pr = 2048;               /* final prediction (after APM) */
static int   pr_mix = 2048;           /* mixer output (pre-APM) */

static u32 hashN(u64 x, u32 salt) {
    u64 h = (x + salt) * 0x9E3779B97F4A7C15ull;
    return (u32)(h >> 24);
}

/* counter: u16 = 12-bit prob << 4 | 4-bit count; count-adaptive rate */
static u16 rcp_tab[16];               /* 65536/(n+2) */

static void ctr_update(u16 *t, int bit) {
    int p = *t >> 4, n = *t & 15;
    p += (((bit << 12) - p) * rcp_tab[n]) >> 16;
    if (p < 0) p = 0;
    if (p > 4095) p = 4095;
    *t = (u16)((p << 4) | (n < 15 ? n + 1 : 15));
}

/* APM / SSE: refine mixer output using (last byte, quantized stretch) */
static u16 apm_t[256 * 33];
static int apm_idx;

static void apm_init(void) {
    for (int c = 0; c < 256; ++c)
        for (int i = 0; i < 33; ++i)
            apm_t[c*33 + i] = (u16)(squash((i - 16) * 128) * 16);
}

static int apm_pp(int p, int cx) {
    int s = stretch(p) + 2048;        /* 0..4095 */
    int w = s & 127;
    apm_idx = cx*33 + (s >> 7);
    return (apm_t[apm_idx]*(128-w) + apm_t[apm_idx+1]*w) >> 11;
}

static void apm_update(int bit) {
    int g = (bit << 16) + (bit << 4) - bit - bit;
    apm_t[apm_idx]   += (g - apm_t[apm_idx])   >> 7;
    apm_t[apm_idx+1] += (g - apm_t[apm_idx+1]) >> 7;
}

static void model_init(void) {
    init_stretch();
    apm_init();
    for (int n = 0; n < 16; ++n) rcp_tab[n] = (u16)(65536 / (n + 2));
    for (int m = 0; m < NHASHED; ++m) {
        tbl[m] = malloc(TSIZE * sizeof(u16));
        if (!tbl[m]) { fprintf(stderr, "oom\n"); exit(1); }
        for (u32 i = 0; i < TSIZE; ++i) tbl[m][i] = 2048 << 4;
    }
    for (int i = 0; i < 256; ++i) t0[i] = 2048 << 4;
    memset(wts, 0, sizeof(wts));
    for (int s = 0; s < NWSETS; ++s)
        for (int m = 0; m < NMODELS; ++m) wts[s][m] = 1 << 14; /* 0.25 */
}

/* prediction for the next bit, 12-bit probability of bit==1 */
static int predict(void) {
    for (int m = 0; m < NHASHED; ++m) {
        slot[m] = &tbl[m][(base_cxt[m] ^ (u32)(c0 * 0x59A9u)) & TMASK];
        st[m] = stretch(*slot[m] >> 4);
    }
    slot[NHASHED] = &t0[c0 & 255];
    st[NHASHED]   = stretch(*slot[NHASHED] >> 4);

    wcur = wts[c4 & 255];
    i64 dot = 0;
    for (int m = 0; m < NMODELS; ++m) dot += (i64)st[m] * wcur[m];
    int d = (int)(dot >> 16);
    if (d >  2047) d =  2047;
    if (d < -2047) d = -2047;
    pr_mix = squash(d);

    pr = (apm_pp(pr_mix, c4 & 255) * 3 + pr_mix) >> 2;
    if (pr < 1)    pr = 1;
    if (pr > 4094) pr = 4094;
    return pr;
}

static void update(int bit) {
    apm_update(bit);

    /* mixer weights */
    int err = (bit << 12) - pr_mix;
    for (int m = 0; m < NMODELS; ++m)
        wcur[m] += (st[m] * err) >> 16;

    for (int m = 0; m < NMODELS; ++m)
        ctr_update(slot[m], bit);

    /* bit history */
    c0 = (c0 << 1) | bit;
    if (c0 >= 256) {                  /* byte boundary */
        int b = c0 & 255;
        c0 = 1;
        c4 = (c4 << 8) | (u32)b;
        c8 = (c8 << 8) | (u64)b;

        if ((b >= 'a' && b <= 'z') || (b >= 'A' && b <= 'Z')) {
            word0 = (word0 + (u32)(b | 32) + 1) * 0x9E3779B1u;
        } else {
            if (word0) word1 = word0;
            word0 = 0;
        }

        base_cxt[M_O1]    = hashN(c4 & 0xffu,               1);
        base_cxt[M_O2]    = hashN(c4 & 0xffffu,             2);
        base_cxt[M_O3]    = hashN(c4 & 0xffffffu,           3);
        base_cxt[M_O4]    = hashN(c4,                       4);
        base_cxt[M_O5]    = hashN(c8 & 0xffffffffffull,     5);
        base_cxt[M_O6]    = hashN(c8 & 0xffffffffffffull,   6);
        base_cxt[M_WORD]  = hashN(word0,                    7);
        base_cxt[M_WORD2] = hashN(word0 + (u64)word1 * 0x2545F491u, 8);
    }
}

/* ---------- arithmetic coder ---------- */

static FILE *fin, *fout;
static u32 x1_, x2_, xr;

static void enc_init(void) { x1_ = 0; x2_ = 0xffffffff; }

static void enc_bit(int bit, int p) {
    u32 xmid = x1_ + (u32)(((u64)(x2_ - x1_) * (u32)p) >> 12);
    if (bit) x2_ = xmid; else x1_ = xmid + 1;
    while (((x1_ ^ x2_) & 0xff000000u) == 0) {
        putc((int)(x2_ >> 24), fout);
        x1_ <<= 8;
        x2_ = (x2_ << 8) | 255;
    }
}

static void enc_flush(void) {
    for (int i = 0; i < 4; ++i) { putc((int)(x1_ >> 24), fout); x1_ <<= 8; }
}

static void dec_init(void) {
    x1_ = 0; x2_ = 0xffffffff; xr = 0;
    for (int i = 0; i < 4; ++i) {
        int ch = getc(fin);
        xr = (xr << 8) | (u32)(ch == EOF ? 0 : ch);
    }
}

static int dec_bit(int p) {
    u32 xmid = x1_ + (u32)(((u64)(x2_ - x1_) * (u32)p) >> 12);
    int bit = (xr <= xmid);
    if (bit) x2_ = xmid; else x1_ = xmid + 1;
    while (((x1_ ^ x2_) & 0xff000000u) == 0) {
        x1_ <<= 8;
        x2_ = (x2_ << 8) | 255;
        int ch = getc(fin);
        xr = (xr << 8) | (u32)(ch == EOF ? 0 : ch);
    }
    return bit;
}

/* ---------- main ---------- */

int main(int argc, char **argv) {
    if (argc != 4 || (argv[1][0] != 'c' && argv[1][0] != 'd')) {
        fprintf(stderr, "usage: %s c|d input output\n", argv[0]);
        return 1;
    }
    fin  = fopen(argv[2], "rb");
    fout = fopen(argv[3], "wb");
    if (!fin || !fout) { perror("open"); return 1; }

    model_init();

    if (argv[1][0] == 'c') {
        fseek(fin, 0, SEEK_END);
        u64 n = (u64)ftell(fin);
        fseek(fin, 0, SEEK_SET);
        for (int i = 7; i >= 0; --i) putc((int)(n >> (i*8)) & 255, fout);
        enc_init();
        int ch;
        while ((ch = getc(fin)) != EOF) {
            for (int i = 7; i >= 0; --i) {
                int bit = (ch >> i) & 1;
                enc_bit(bit, predict());
                update(bit);
            }
        }
        enc_flush();
    } else {
        u64 n = 0;
        for (int i = 0; i < 8; ++i) n = (n << 8) | (u32)getc(fin);
        dec_init();
        for (u64 j = 0; j < n; ++j) {
            for (int i = 0; i < 8; ++i) {
                int bit = dec_bit(predict());
                update(bit);
            }
            putc(c4 & 255, fout);     /* update() folded the byte into c4 */
        }
    }
    fclose(fin);
    fclose(fout);
    return 0;
}
