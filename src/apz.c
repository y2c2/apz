/* Arbitrary-Precision Interger
 * Copyright(c) 2019-2020 y2c2 */

#include "apz.h"
#include "apz_config.h"
#include "apz_libc.h"

/* Declarations */
static apz_ordering_t apz_cmp_prim(const apz_t *x1, const apz_t *x2);

/* Tricky toolkits */

#define IS_USED_SOLO(x) \
    ((x)->nchunks_used == 1)

#define IS_CAPACITY_SOLO(x) \
    ((x)->nchunks_capacity == 1)

#define ALLOC_CHUNKS(n) \
    ((apz_chunk_t *)apz_malloc(sizeof(apz_chunk_t) * (n)))

#define ABS(n) \
    (((n)>=0)?((apz_s64)n):(-((apz_s64)n)))

#define REQUIRED_CHUNK_CNT(bitcount) \
        (((bitcount) / apz_chunk_size) + \
         (((bitcount) % apz_chunk_size == 0) ? 0 : 1))

static apz_chunk_t *alloc_chunks_zero(apz_size_t n)
{
    apz_chunk_t *chunks = ALLOC_CHUNKS(n);
    if (chunks == apz_nullptr) return apz_nullptr;
    {
        apz_size_t i;
        for (i = 0; i != n; i++)
        {
            chunks[i] = 0;
        }
    }
    return chunks;
}

#define ALLOC_CHUNKS_FILL_ZERO(n) \
    (alloc_chunks_zero(n))

#define GET_CHUNK(x_, idx_) \
    (idx_ < ((x_)->nchunks_capacity) ? ((x_)->chunks[idx_]) : 0)

static void set_chunk(apz_t *x, apz_size_t idx, apz_chunk_t v)
{
    /* Assign */
    x->chunks[idx] = v;

    /* Adjust 'used' */
    if ((x->nchunks_used > 1) && \
            (idx == (x->nchunks_used - 1)) && \
            (idx != 0) && \
            (v == 0))
    {
        /* May shrink */
        apz_size_t idx_ = (apz_size_t)idx;
        while (idx_ != 0) {
            apz_chunk_t chk = (x)->chunks[idx_];
            if (chk != 0) break;
            (x)->nchunks_used--;
            idx_--;
        }
    }
    else if ((idx >= x->nchunks_used) && (v != 0))
    {
        /* May expand */
        x->nchunks_used = idx + 1;
    }
}

#define SET_CHUNK(x, idx, v) set_chunk(x, idx, v)

/*
#define SET_CHUNK1(x, idx, v) \
    do { \
        if (idx < ((x)->nchunks_capacity)) \
        { (x)->chunks[idx] = v; } \
        if ((v) != 0) { \
            if (idx >= ((x)->nchunks_used)) { \
                (x)->nchunks_used = idx + 1; \
            } \
        } else { \
            if (((idx) == (x)->nchunks_used - 1) && (idx != 0)) { \
                apz_size_t idx_ = (apz_size_t)idx; \
                while (idx_ != 0) { \
                    apz_chunk_t chk = (x)->chunks[idx_]; \
                    if (chk != 0) break; \
                    (x)->nchunks_used--; \
                    idx_--; \
                } \
            } \
        } \
    } while (0)
    */

#define IS_NON_NEGATIVE(n) \
    ((n)->negative == apz_false)

#define TO_NON_NEGATIVE(n) \
    do { \
        (n)->negative = apz_false; \
    } while (0)

#define IS_NEGATIVE(n) \
    ((n)->negative == apz_true)

#define TO_NEGATIVE(n) \
    do { \
        (n)->negative = apz_true; \
    } while (0)

#define FILL_BASIC(_x, _used, _capacity, _negative) \
    do { \
        (_x)->nchunks_used = _used; \
        (_x)->nchunks_capacity = _capacity; \
        (_x)->negative = _negative; \
    } while (0)

#define FILL_BASIC_ALLOC_OR_DIR(_x, _used, _capacity, _negative) \
    do { \
        FILL_BASIC(_x, _used, _capacity, _negative); \
        if (((_x)->chunks = ALLOC_CHUNKS((_x)->nchunks_capacity)) \
                == apz_nullptr) { return -1; } \
    } while (0)

#define FILL_BASIC_SOLO(_x, _negative, _value) \
    do { \
        FILL_BASIC(_x, APZ_NCHUNKS_USED_SOLO, APZ_NCHUNKS_CAPACITY_SOLO, _negative); \
        (_x)->chunks = &(_x)->solo_placeholder; \
        (_x)->solo_placeholder = (apz_chunk_t)(_value); \
    } while (0)

#define FILL_BASIC_DUO(_x, _negative) \
    do { \
        FILL_BASIC(_x, APZ_NCHUNKS_USED_DUO, APZ_NCHUNKS_CAPACITY_DUO, _negative); \
        (_x)->chunks = apz_nullptr; \
    } while (0)

#define FILL_BASIC_DUO_ALLOC_OR_DIR(_x, _negative, _value1, _value0) \
    do { \
        FILL_BASIC_DUO(_x, _negative); \
        if (((_x)->chunks = ALLOC_CHUNKS(x->nchunks_capacity)) \
                == apz_nullptr) { return -1; } \
        (_x)->chunks[1] = _value1; \
        (_x)->chunks[0] = _value0; \
    } while (0)

#define FILL_BASIC_TRI(_x, _negative) \
    do { \
        FILL_BASIC(_x, APZ_NCHUNKS_USED_TRI, APZ_NCHUNKS_CAPACITY_TRI, _negative); \
        (_x)->chunks = apz_nullptr; \
    } while (0)

#define FILL_BASIC_TRI_ALLOC_OR_DIR(_x, _negative, _value2, _value1, _value0) \
    do { \
        FILL_BASIC_TRI(_x, _negative); \
        if (((_x)->chunks = ALLOC_CHUNKS(x->nchunks_capacity)) \
                == apz_nullptr) { return -1; } \
        (_x)->chunks[2] = _value2; \
        (_x)->chunks[1] = _value1; \
        (_x)->chunks[0] = _value0; \
    } while (0)

#define MAX2(a,b) ((a)>(b)?(a):(b))
#define MIN2(a,b) ((a)<(b)?(a):(b))

#define MAX2_USED(a,b) MAX2((a)->nchunks_used, (b)->nchunks_used)
#define MIN2_USED(a,b) MIN2((a)->nchunks_used, (b)->nchunks_used)

static apz_bool chunk_sum_overflow( \
        const apz_chunk_t x, const apz_chunk_t y)
{
#if defined(APZ_CHUNK_32)
    return (((x >> 31) | (y >> 31)) != 0) ? apz_true : apz_false;
#elif defined(APZ_CHUNK_64)
    return (((x >> 63) | (y >> 63)) != 0) ? apz_true : apz_false;
#else
#error "chunk not defined"
#endif
}

static inline apz_size_t hbidx_16(apz_u16 value)
{
    if (value & 0xFF00)
        if (value & 0xF000)
            if (value & 0xC000)
                if (value & 0x8000) return 16;
                else return 15;
            else
                if (value & 0x2000) return 14;
                else return 13;
        else
            if (value & 0x0C00)
                if (value & 0x0800) return 12;
                else return 11;
            else
                if (value & 0x0200) return 10;
                else return 9;
    else
        if (value & 0xF0)
            if (value & 0xC0)
                if (value & 0x80) return 8;
                else return 7;
            else
                if (value & 0x20) return 6;
                else return 5;
        else
            if (value & 0x0C)
                if (value & 0x08) return 4;
                else return 3;
            else
                if (value & 0x02) return 2;
                else
                    if (value != 0) return 1;
                    else return 0;
}

static inline apz_size_t hbidx_32(apz_u32 value)
{
    apz_size_t test = hbidx_16(value >> 16);
    return (test > 0) ? (test + 16) : hbidx_16(value & 0xFFFF);
}

#if defined(APZ_CHUNK_32)
#define apz_chunk_bitcount hbidx_32
#elif defined(APZ_CHUNK_64)
static apz_size_t hbidx_64(apz_u64 value)
{
    apz_size_t test = hbidx_32(value >> 32);
    return (test > 0) ? (test + 32) : hbidx_32(value & 0xFFFFFFFF);
}
#define apz_chunk_bitcount hbidx_64
#endif

static apz_size_t apz_bitcount(const apz_t *x)
{
    apz_chunk_t msc = x->chunks[x->nchunks_used - 1];
    apz_size_t msc_bitcount = apz_chunk_bitcount(msc);
    return (x->nchunks_used - 1) * apz_chunk_size + msc_bitcount;
}

static void chunk_add( \
        apz_chunk_t *carry_out, apz_chunk_t *sum_out, \
        const apz_chunk_t x, const apz_chunk_t y)
{
    *sum_out = x + y;
    *carry_out = ((*sum_out < x) || (*sum_out < y)) ? 1 : 0;
}

static void chunk_sub( \
        apz_chunk_t *carry_out, apz_chunk_t *sum_out, \
        const apz_chunk_t x, const apz_chunk_t y)
{
    *carry_out = (x < y) ? 1 : 0;
    *sum_out = (*carry_out != 0) ? (apz_chunk_max - y + x + 1) : (x - y);
}

static void chunk_acc( \
        apz_chunk_t *carry_modify, apz_chunk_t *sum_modify, \
        const apz_chunk_t x)
{
    apz_chunk_t new_sum = *sum_modify + x;
    if ((new_sum < *sum_modify) || (new_sum < x))
    {
        /* Overflow */
        *carry_modify += 1;
    }
    *sum_modify = new_sum;
}

static void chunk_mul( \
        apz_chunk_t *carry_out, apz_chunk_t *product_out, \
        const apz_chunk_t x, const apz_chunk_t y)
{
    apz_chunk_t xh = apz_chunk_high_half(x);
    apz_chunk_t xl = apz_chunk_low_half(x);
    apz_chunk_t yh = apz_chunk_high_half(y);
    apz_chunk_t yl = apz_chunk_low_half(y);
    apz_chunk_t ch = 0, cl = 0, ph = 0, pl = 0;
    apz_chunk_t t[8];
    apz_chunk_t zh, zl;
    /*         [xh][xl] */
    /*         [yh][yl] */
    /* -----------------*/
    /* [ch][cl][ph][pl] */

    /*         [t1][t0] = xl * yl */
    /*     [t3][t2]     = xh * yl */
    /*     [t5][t4]     = xl * yh */
    /* [t7][t6]         = xh * yh */

#define HALF_MUL(_c, _p, _x, _y) \
    do { \
        apz_chunk_t p_full = _x * _y; \
        _c = apz_chunk_high_half(p_full); \
        _p = apz_chunk_low_half(p_full); \
    } while (0)

    HALF_MUL(t[1], t[0], xl, yl);
    HALF_MUL(t[3], t[2], xh, yl);
    HALF_MUL(t[5], t[4], xl, yh);
    HALF_MUL(t[7], t[6], xh, yh);

#undef HALF_MUL

#define HALF_ADD_RST(_h, _l) \
    do { \
        _h = 0; \
        _l = 0; \
    } while (0)

#define HALF_ADD_SHIFT(_h, _l) \
    do { \
        _l = _h; _h = 0; \
    } while (0)

#define HALF_ADD_ACC(_h, _l, _x) \
    do { \
        apz_chunk_t _sum = _l + _x; \
        _h += apz_chunk_high_half(_sum); \
        _l = apz_chunk_low_half(_sum); \
    } while (0)

    /* pl */
    {
        pl = t[0];
    }

    /* ph */
    {
        HALF_ADD_RST(zh, zl);
        HALF_ADD_ACC(zh, zl, t[1]);
        HALF_ADD_ACC(zh, zl, t[2]);
        HALF_ADD_ACC(zh, zl, t[4]);
        ph = zl;
    }

    /* cl */
    {
        HALF_ADD_SHIFT(zh, zl);
        HALF_ADD_ACC(zh, zl, t[3]);
        HALF_ADD_ACC(zh, zl, t[5]);
        HALF_ADD_ACC(zh, zl, t[6]);
        cl = zl;
    }

    /* ch */
    {
        HALF_ADD_SHIFT(zh, zl);
        HALF_ADD_ACC(zh, zl, t[7]);
        ch = zl;
    }

    *carry_out = apz_chunk_make_by_half(ch, cl);
    *product_out = apz_chunk_make_by_half(ph, pl);

#undef HALF_ADD_RST
#undef HALF_ADD_ACC
#undef HALF_ADD_SHIFT
}

static void clear_chunks(apz_t *x)
{
    apz_size_t i;
    for (i = 0; i != x->nchunks_used; i++)
    {
        x->chunks[i] = 0;
    }
    x->nchunks_used = 1;
}

static int apz_extend_capacity(apz_t *x, const apz_size_t new_capacity)
{
    if (new_capacity <= x->nchunks_capacity) { return 0; }
    {
        apz_chunk_t *new_chunks;
        if ((new_chunks = ALLOC_CHUNKS_FILL_ZERO(new_capacity)) == apz_nullptr)
        {
            return -1;
        }
        {
            /* Preserve old chunks data */
            apz_size_t i;
            for (i = 0; i != x->nchunks_used; i++)
            {
                new_chunks[i] = x->chunks[i];
            }
            /* Replace old chunks */
            if (!IS_CAPACITY_SOLO(x))
            {
                apz_free(x->chunks);
            }
            x->chunks = new_chunks;
            x->nchunks_capacity = new_capacity;
            x->solo_placeholder = 0;
        }
    }
    return 0;
}

/* Initialize */
/*
 * Presume the memory content in x is random
 * which force we to skip the checking the content */

void apz_init_zero(apz_t *x)
{
    apz_init_int(x, 0);
}

void apz_init_one(apz_t *x)
{
    apz_init_int(x, 1);
}

void apz_init_int(apz_t *x, const int n)
{
    FILL_BASIC_SOLO(x, (n < 0) ? apz_true : apz_false, ABS(n));
}

void apz_init_uint(apz_t *x, const unsigned int n)
{
    FILL_BASIC_SOLO(x, apz_false, n);
}

void apz_init_s32(apz_t *x, const apz_s32 n)
{
    apz_init_int(x, n);
}

void apz_init_u32(apz_t *x, const apz_u32 n)
{
    FILL_BASIC_SOLO(x, apz_false, n);
}

int apz_init_s64(apz_t *x, const apz_s64 n)
{
#if defined(APZ_CHUNK_32)
    /* assign 64-bit signed integer to 32-bit units */
    if (n == apz_s64_min)
    {
        /* 3 units required */
        FILL_BASIC_TRI_ALLOC_OR_DIR(x, apz_true, 1, 0, 0);
    }
    else if (n < -((apz_s64)APZ_CHUNK_MAX_32))
    {
        FILL_BASIC_DUO_ALLOC_OR_DIR(x, apz_true, n >> 32, n & 0xFFFFFFFF);
    }
    else if (n > ((apz_s64)APZ_CHUNK_MAX_32))
    {
        FILL_BASIC_DUO_ALLOC_OR_DIR(x, apz_false, n >> 32, n & 0xFFFFFFFF);
    }
    else
    {
        FILL_BASIC_SOLO(x, n < 0 ? apz_true : apz_false, ABS(n));
    }
#elif defined(APZ_CHUNK_64)
    if (n == apz_s64_min)
    {
        FILL_BASIC_DUO_ALLOC_OR_DIR(x, apz_true, 1, 0);
    }
    else
    {
        FILL_BASIC_SOLO(x, (n < 0) ? apz_true : apz_false, ABS(n));
    }
#endif
    return 0;
}

int apz_init_u64(apz_t *x, const apz_u64 n)
{
#if defined(APZ_CHUNK_32)
    /* assign 64-bit signed integer to 32-bit units */
    /* Extend with a double-units int */
    if (n > (apz_s64)APZ_CHUNK_MAX_32)
    {
        FILL_BASIC_DUO_ALLOC_OR_DIR(x, apz_false, n >> 32, n & 0xFFFFFFFF);
    }
    else
    {
        FILL_BASIC_SOLO(x, apz_false, n);
    }
#elif defined(APZ_CHUNK_64)
    FILL_BASIC_SOLO(x, apz_false, n);
#endif
    return 0;
}

static int convert_digit(const apz_s32 ch, const int base)
{
#define VALID_CHAR_RANGE_1_10(ch, base) \
        ((('0' <= (ch)) && ((ch) <= ('0' + base))) \
         ? (ch - '0') : (-1))
#define VALID_CHAR_RANGE_11_36a(ch, base) \
        ((('a' <= (ch)) && ((ch) <= ('a' + base - 10))) \
         ? (ch - 'a' + 10) : (-1))
#define VALID_CHAR_RANGE_11_36A(ch, base) \
        ((('A' <= (ch)) && ((ch) <= ('A' + base - 10))) \
         ? (ch - 'A' + 10) : (-1))
    int d;
    if ((d = VALID_CHAR_RANGE_1_10(ch, base)) != -1) return d;
    if ((d = VALID_CHAR_RANGE_11_36A(ch, base)) != -1) return d;
    if ((d = VALID_CHAR_RANGE_11_36a(ch, base)) != -1) return d;
#undef VALID_CHAR
#undef VALID_CHAR_LE10
#undef VALID_CHAR_LE36
    return -1;
}

#define DEF_INITSTR(_name, _type) \
int apz_init_##_name##n(apz_t *x, const _type *s, apz_size_t len) { \
    return apz_init_##_name##n_base(x, s, len, 10); } \
int apz_init_##_name(apz_t *x, const _type *s) { \
    return apz_init_##_name##n(x, s, apz_##_name##len(s)); } \
int apz_init_##_name##n_base(apz_t *x, const _type *s, apz_size_t len, const int base) { \
    apz_bool is_negative = apz_false; \
    const _type *p = s; \
    if ((base <= 0) || (base > 36)) return -1; \
    apz_init_zero(x); \
    if ((len != 0) && s[0] == '-') {  \
        is_negative = apz_true; \
        p++; \
        len--; \
    } \
    while (len-- != 0) { \
        int d = convert_digit((apz_s32)(*p), base); \
        if (d == -1) return -1; \
        if (apz_mul_int(x, base) != 0) { return -1; } \
        if (apz_add_int(x, d) != 0) { return -1; } \
        p++; \
    } \
    x->negative = is_negative && !apz_is_zero(x); \
    return 0; } \
int apz_init_##_name##_base(apz_t *x, const _type *s, const int base) { \
    return apz_init_##_name##n_base(x, s, apz_##_name##len(s), base); } \
int apz_init_##_name##n_c_style(apz_t *x, const _type *s, apz_size_t len) { \
    int ret = 0; \
    if ((len >= 2) && (s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) { \
        ret = apz_init_##_name##n_base(x, s + 2, len - 2, 16); } \
    else if ((len >= 3) && (s[0] == '-') && (s[1] == '0') && ((s[2] == 'x') || (s[2] == 'X'))) { \
        ret = apz_init_##_name##n_base(x, s + 3, len - 2, 16); \
        if (ret != 0) { apz_to_negative(x); } } \
    else if ((len >= 2) && (s[0] == '0') && ((s[1] == 'b') || (s[1] == 'B'))) { \
        ret = apz_init_##_name##n_base(x, s + 2, len - 2, 2); } \
    else if ((len >= 3) && (s[0] == '-') && (s[1] == '0') && ((s[2] == 'b') || (s[2] == 'B'))) { \
        ret = apz_init_##_name##n_base(x, s + 3, len - 2, 1); \
        if (ret != 0) { apz_to_negative(x); } } \
    else if ((len >= 1) && (s[0] == '0')) { \
        ret = apz_init_##_name##n_base(x, s + 1, len - 1, 8); }  \
    else if ((len >= 2) && (s[0] == '-') && (s[1] == '0')) { \
        ret = apz_init_##_name##n_base(x, s + 2, len - 2, 8); \
        if (ret != 0) { apz_to_negative(x); } } \
    else { ret = apz_init_##_name##n_base(x, s, len, 10); } \
    return ret; } \
int apz_init_##_name##_c_style(apz_t *x, const _type *s) { \
    return apz_init_##_name##n_c_style(x, s, apz_##_name##len(s)); }

DEF_INITSTR(str, char)
DEF_INITSTR(u16str, apz_u16)
DEF_INITSTR(u32str, apz_u32)

#undef DEF_INITSTR

/* Uninitialize */

void apz_uninit(apz_t *x)
{
    /* Solo checking */
    if (IS_CAPACITY_SOLO(x)) return;
    /* Free allocated part */
    apz_free(x->chunks);
}

/* Test */

apz_bool apz_is_zero(const apz_t *x)
{
    return IS_USED_SOLO(x) && \
        IS_NON_NEGATIVE(x) && \
        (GET_CHUNK(x, 0) == 0);
}

apz_bool apz_is_one(const apz_t *x)
{
    return IS_USED_SOLO(x) && \
        IS_NON_NEGATIVE(x) && \
        (GET_CHUNK(x, 0) == 1);
}

apz_bool apz_is_negative_one(const apz_t *x)
{
    return IS_USED_SOLO(x) && \
        IS_NEGATIVE(x) && \
        (GET_CHUNK(x, 0) == 1);
}

apz_bool apz_is_positive(const apz_t *x)
{
    return (IS_NON_NEGATIVE(x) && !apz_is_zero(x));
}

apz_bool apz_is_negative(const apz_t *x)
{
    return IS_NEGATIVE(x);
}

apz_bool apz_is_non_positive(const apz_t *x)
{
    return (IS_NEGATIVE(x) || !apz_is_zero(x));
}

apz_bool apz_is_non_negative(const apz_t *x)
{
    return IS_NON_NEGATIVE(x);
}

apz_bool apz_is_valid_int(const apz_t *x)
{
    if (!IS_USED_SOLO(x)) return apz_false;
    if (apz_is_zero(x)) return apz_true;
    else if (apz_is_positive(x) && (GET_CHUNK(x, 0) <= apz_s32_max)) return apz_true;
    else if (apz_is_negative(x) && (GET_CHUNK(x, 0) - 1 <= apz_s32_max)) return apz_true;
    return apz_false;
}

apz_bool apz_is_valid_uint(const apz_t *x)
{
    if (!IS_USED_SOLO(x)) return apz_false;
    if (apz_is_negative(x)) return apz_false;
    return apz_true;
}

apz_bool apz_is_valid_s32(const apz_t *x)
{
    return apz_is_valid_int(x);
}

apz_bool apz_is_valid_u32(const apz_t *x)
{
    return apz_is_valid_uint(x);
}

apz_bool apz_is_valid_s64(const apz_t *x)
{
#if defined(APZ_CHUNK_32)
    if (IS_USED_SOLO(x))
    {
        return apz_true;
    }
    else if (x->nchunks_used == 2)
    {
        return apz_true;
    }
    else if (x->nchunks_used == 3)
    {
        if ((GET_CHUNK(x, 0) == 0) && \
                (GET_CHUNK(x, 1) == 0) && \
                (GET_CHUNK(x, 2) == 1))
        {
            return apz_true;
        }
    }
    return apz_false;
#elif defined(APZ_CHUNK_64)
    if (IS_USED_SOLO(x))
    {
        return apz_true;
    }
    else if (x->nchunks_used == 2)
    {
        if ((GET_CHUNK(x, 0) == 0) && (GET_CHUNK(x, 1) == 1))
        {
            return apz_true;
        }
    }
    return apz_false;
#endif
}

apz_bool apz_is_valid_u64(const apz_t *x)
{
#if defined(APZ_CHUNK_32)
    if (x->nchunks_used > 2) return apz_false;
    if (apz_is_negative(x)) return apz_false;
    return apz_true;
#elif defined(APZ_CHUNK_64)
    if (!IS_USED_SOLO(x)) return apz_false;
    if (apz_is_negative(x)) return apz_false;
    return apz_true;
#endif
}

apz_bool apz_is_valid_float(const apz_t *x)
{
    /* TODO: Add some logic */
    (void)x;
    return apz_true;
}

apz_bool apz_is_valid_double(const apz_t *x)
{
    /* TODO: Add some logic */
    (void)x;
    return apz_true;
}

/* To Primitive */

int apz_to_int(const apz_t *x)
{
    if (!apz_is_valid_int(x)) return 0;
    if (apz_is_zero(x)) return 0;
    return apz_is_negative(x) ? \
        (-(int)(GET_CHUNK(x, 0))) :
        ((int)(GET_CHUNK(x, 0)));
}

unsigned int apz_to_uint(const apz_t *x)
{
    if (!apz_is_valid_uint(x)) return 0;
    if (apz_is_zero(x)) return 0;
    return (unsigned int)GET_CHUNK(x, 0);
}

apz_s32 apz_to_s32(const apz_t *x)
{
    return apz_to_int(x);
}

apz_u32 apz_to_u32(const apz_t *x)
{
    return apz_to_uint(x);
}

apz_s64 apz_to_s64(const apz_t *x)
{
    if (!apz_is_valid_s64(x)) return 0;
    if (apz_is_zero(x)) return 0;
#if defined(APZ_CHUNK_32)
    if (IS_USED_SOLO(x))
    {
        return (apz_is_negative(x)) ? \
            (-GET_CHUNK(x, 0)) : GET_CHUNK(x, 0);
    }
    else if (x->nchunks_used == 2)
    {
        return (apz_is_negative(x)) ? \
            -((apz_u64)GET_CHUNK(x, 1) << 32 | GET_CHUNK(x, 0)) : \
            ((apz_u64)GET_CHUNK(x, 1) << 32 | GET_CHUNK(x, 0));
    }
    else
    {
        if ((GET_CHUNK(x, 0) == 0) && \
                (GET_CHUNK(x, 1) == 0) && \
                (GET_CHUNK(x, 2) == 1))
        {
            return -9223372036854775807L - 1L;
        }
    }
    return 0;
#elif defined(APZ_CHUNK_64)
    if (IS_USED_SOLO(x))
    {
        return (apz_is_negative(x)) ? \
            (-(apz_s64)GET_CHUNK(x, 0)) : (apz_s64)GET_CHUNK(x, 0);
    }
    else if (x->nchunks_used == 2)
    {
        if ((GET_CHUNK(x, 0) == 0) && (GET_CHUNK(x, 1) == 1))
        {
            return -9223372036854775807L - 1L;
        }
    }
    return 0;
#endif
}

apz_u64 apz_to_u64(const apz_t *x)
{
    if (!apz_is_valid_u64(x)) return 0;
    if (apz_is_zero(x)) return 0;
#if defined(APZ_CHUNK_32)
    if (IS_NON_NEGATIVE(x))
    {
        if (IS_USED_SOLO(x))
        {
            return (apz_u64)(GET_CHUNK(x, 0));
        }
        else if (x->nchunks_used == 2)
        {
            return ((apz_u64)(GET_CHUNK(x, 0)) |
                    ((apz_u64)GET_CHUNK(x, 1) << apz_chunk_size));
        }
    }
    return 0;
#elif defined(APZ_CHUNK_64)
    if (IS_NON_NEGATIVE(x) && IS_USED_SOLO(x))
    {
        return GET_CHUNK(x, 0);
    }
    return 0;
#endif
}

float apz_to_float(const apz_t *x)
{
    float ret = 0;
    apz_size_t i;
    for (i = 0; i != x->nchunks_used; i++)
    {
        ret = ret * ((float)(APZ_CHUNK_MAX_32) + 1) + x->chunks[i];
    }
    return ret;
}

double apz_to_double(const apz_t *x)
{
    double ret = 0;
    apz_size_t i;
    for (i = 0; i != x->nchunks_used; i++)
    {
        ret = ret * ((double)(APZ_CHUNK_MAX_32) + 1) + x->chunks[i];
    }
    return ret;
}

void apz_to_positive(apz_t *x)
{
    x->negative = apz_false;
}

void apz_to_negative(apz_t *x)
{
    if (apz_is_zero(x)) return;
    x->negative = apz_true;
}

/* Clone */
int apz_clone(apz_t *dst, const apz_t *src)
{
    /* Solo clone */
    if (IS_CAPACITY_SOLO(src))
    {
        dst->nchunks_used = 1;
        dst->nchunks_capacity = 1;
        dst->negative = src->negative;
        dst->solo_placeholder = src->solo_placeholder;
        dst->chunks = &dst->solo_placeholder;
        return 0;
    }
    FILL_BASIC_ALLOC_OR_DIR(dst, src->nchunks_used, src->nchunks_capacity, src->negative);
    {
        apz_size_t i;
        for (i = 0; i != dst->nchunks_used; i++)
        {
            dst->chunks[i] = src->chunks[i];
        }
    }
    return 0;
}

/* Move */
void apz_move(apz_t *dst, apz_t *src)
{
    if (IS_CAPACITY_SOLO(src))
    {
        dst->solo_placeholder = src->solo_placeholder;
        dst->chunks = &dst->solo_placeholder;
    }
    else
    {
        dst->chunks = src->chunks;
        src->solo_placeholder = 0;
        src->chunks = &src->solo_placeholder;
    }
    {
        dst->nchunks_used = src->nchunks_used;
        dst->nchunks_capacity = src->nchunks_capacity;
        dst->negative = src->negative;
    }
    {
        src->nchunks_used = 1;
        src->nchunks_capacity = 1;
        src->negative = apz_false;
    }
}

/* Swap */
void apz_swap(apz_t *lhs, apz_t *rhs)
{
    apz_t t;
    apz_move(&t, lhs);
    apz_move(lhs, rhs);
    apz_move(rhs, &t);
}

/* Trim */
int apz_trim(apz_t *x)
{
    if (IS_CAPACITY_SOLO(x)) return 0;
    if (x->nchunks_capacity == x->nchunks_used) return 0;
    if (x->nchunks_used == 1)
    {
        /* Shrink to solo */
        x->solo_placeholder = x->chunks[0];
        apz_free(x->chunks);
        x->chunks = &x->solo_placeholder;
        x->nchunks_capacity = 1;
    }
    else
    {
        apz_chunk_t *new_chunks = ALLOC_CHUNKS(x->nchunks_used);
        apz_size_t i;
        if (new_chunks == apz_nullptr) return -1;
        for (i = 0; i != x->nchunks_used; i++) { new_chunks[i] = GET_CHUNK(x, i); }
        apz_free(x->chunks);
        x->chunks = new_chunks;
        x->nchunks_capacity = x->nchunks_used;
    }
    return 0;
}

void apz_negative(apz_t *x)
{
    if (apz_is_zero(x)) return;
    x->negative = (x->negative == apz_true) ? apz_false : apz_true;
}

int apz_lshift_size_t(apz_t *x, const apz_size_t n)
{
    apz_size_t unit_shift = n / apz_chunk_size;
    apz_size_t bit_shift = n % apz_chunk_size;
    apz_size_t idx_src, idx_dst;
    apz_size_t used = x->nchunks_used;
    apz_chunk_t chunk;
    /* Extend on request */
    {
        apz_size_t new_capacity = x->nchunks_used + unit_shift + ((bit_shift != 0) ? 1 : 0);
        if (new_capacity > x->nchunks_capacity)
        {
            if (apz_extend_capacity(x, new_capacity) != 0)
            {
                return -1;
            }
        }
    }
    /* Unit moving */
    if (unit_shift != 0)
    {
        idx_src = used;
        idx_dst = idx_src + unit_shift;
        do
        {
            idx_src--;
            idx_dst--;
            chunk = GET_CHUNK(x, idx_src);
            SET_CHUNK(x, idx_dst, chunk);
        } while (idx_src != 0);
        do
        {
            idx_dst--;
            SET_CHUNK(x, idx_dst, 0);
        } while (idx_dst != 0);
    }
    /* Bit moving */
    if (bit_shift != 0)
    {
        apz_chunk_t chunk_dst, chunk_src, chunk_new;
        idx_src = used + unit_shift;
        idx_dst = idx_src + 1;
        do
        {
            idx_src--;
            idx_dst--;
            chunk_dst = GET_CHUNK(x, idx_dst);
            chunk_src = GET_CHUNK(x, idx_src);
            chunk_new = (chunk_dst << bit_shift) | \
                        (chunk_src >> (apz_chunk_size - bit_shift));
            SET_CHUNK(x, idx_dst, chunk_new);
        } while (idx_src != 0);
        chunk_new = GET_CHUNK(x, 0) << bit_shift;
        SET_CHUNK(x, 0, chunk_new);
    }
    apz_trim(x);
    return 0;
}

int apz_lshift_int(apz_t *x, const int n)
{
    if (n > 0) return apz_lshift_size_t(x, (size_t)n);
    else if (n < 0) return apz_rshift_size_t(x, (size_t)ABS(n));
    return 0;
}

int apz_lshift(apz_t *x, const apz_t *n)
{
    int n1;
    n1 = apz_to_int(n);
    return apz_lshift_int(x, n1);
}

int apz_rshift_size_t(apz_t *x, const apz_size_t n)
{
    apz_size_t unit_shift = n / apz_chunk_size;
    apz_size_t bit_shift = n % apz_chunk_size;
    apz_size_t idx_dst, idx_src;
    apz_size_t used = x->nchunks_used;
    /* Unit moving */
    if (unit_shift != 0)
    {
        idx_dst = 0;
        idx_src = idx_dst + unit_shift;

        while (idx_dst != used)
        {
            apz_chunk_t chunk = GET_CHUNK(x, idx_src);
            SET_CHUNK(x, idx_dst, chunk);

            idx_dst++;
            idx_src++;
        }
    }
    /* Bit moving */
    if (bit_shift != 0)
    {
        idx_dst = 0;
        idx_src = 1;
        while (idx_dst != used)
        {
            apz_chunk_t chunk_cur = GET_CHUNK(x, idx_dst);
            apz_chunk_t chunk_next = GET_CHUNK(x, idx_src);
            chunk_cur = chunk_cur >> bit_shift | \
                        ((chunk_next & ((1 << bit_shift) - 1)) << (apz_chunk_size - bit_shift));
            SET_CHUNK(x, idx_dst, chunk_cur);

            idx_src++;
            idx_dst++;
        }
    }
    return 0;
}

int apz_rshift_int(apz_t *x, const int n)
{
    if (n > 0) return apz_rshift_size_t(x, (size_t)n);
    else if (n < 0) return apz_lshift_size_t(x, (size_t)ABS(n));
    return 0;
}

int apz_rshift(apz_t *x, const apz_t *n)
{
    int n1;
    n1 = apz_to_int(n);
    return apz_rshift_int(x, n1);
}

int apz_bitwise_inv(apz_t *x)
{
    apz_size_t i;
    for (i = 0; i != x->nchunks_used; i++)
    {
        x->chunks[i] = ~x->chunks[i];
    }
    return 0;
}

int apz_bitwise_and(apz_t *x, const apz_t *y)
{
    apz_size_t used = MAX2(x->nchunks_used, y->nchunks_used);
    if (x->nchunks_capacity < y->nchunks_used)
    {
        if (apz_extend_capacity(x, y->nchunks_used) != 0)
        {
            return -1;
        }
    }
    {
        apz_size_t i;
        for (i = 0; i != used; i++)
        {
            apz_chunk_t chunk_x, chunk_y;
            chunk_x = GET_CHUNK(x, i);
            chunk_y = GET_CHUNK(y, i);
            SET_CHUNK(x, i, chunk_x & chunk_y);
        }
    }
    return 0;
}

int apz_bitwise_or(apz_t *x, const apz_t *y)
{
    apz_size_t used = MAX2(x->nchunks_used, y->nchunks_used);
    if (x->nchunks_capacity < y->nchunks_used)
    {
        if (apz_extend_capacity(x, y->nchunks_used) != 0)
        {
            return -1;
        }
    }
    {
        apz_size_t i;
        for (i = 0; i != used; i++)
        {
            apz_chunk_t chunk_x, chunk_y;
            chunk_x = GET_CHUNK(x, i);
            chunk_y = GET_CHUNK(y, i);
            SET_CHUNK(x, i, chunk_x | chunk_y);
        }
    }
    return 0;
}

int apz_bitwise_xor(apz_t *x, const apz_t *y)
{
    apz_size_t used = MAX2(x->nchunks_used, y->nchunks_used);
    if (x->nchunks_capacity < y->nchunks_used)
    {
        if (apz_extend_capacity(x, y->nchunks_used) != 0)
        {
            return -1;
        }
    }
    {
        apz_size_t i;
        for (i = 0; i != used; i++)
        {
            apz_chunk_t chunk_x, chunk_y;
            chunk_x = GET_CHUNK(x, i);
            chunk_y = GET_CHUNK(y, i);
            SET_CHUNK(x, i, chunk_x ^ chunk_y);
        }
    }
    return 0;
}

int apz_bitwise_assign(apz_t *x, const apz_size_t idx, const unsigned int v)
{
    apz_size_t chk_idx = idx / apz_chunk_size;
    apz_size_t bit_idx = idx % apz_chunk_size;
    /* Extend when required */
    if (chk_idx >= x->nchunks_capacity)
    {
        apz_size_t required_capacity = chk_idx + 1;
        /* Extend */
        if (apz_extend_capacity(x, required_capacity) != 0)
        { return -1; }
    }
    {
        apz_chunk_t chk;
        chk = GET_CHUNK(x, chk_idx);
        if (v == 0)
        {
            /* Clear */
            chk &= ~(1UL << bit_idx);
        }
        else
        {
            chk |= 1UL << bit_idx;
        }
        SET_CHUNK(x, chk_idx, chk);
    }
    return 0;
}

unsigned int apz_bitwise_extract(apz_t *x, const apz_size_t idx)
{
    apz_chunk_t chk;
    apz_size_t chk_idx = idx / apz_chunk_size;
    apz_size_t bit_idx = idx % apz_chunk_size;
    /* Extend when required */
    if (chk_idx >= x->nchunks_capacity)
    {
        return 0;
    }
    chk = GET_CHUNK(x, chk_idx);
    return (unsigned int)((chk >> bit_idx) & 1);
}

int apz_bitwise_set(apz_t *x, const apz_size_t idx)
{
    return apz_bitwise_assign(x, idx, 1);
}

int apz_bitwise_clr(apz_t *x, const apz_size_t idx)
{
    return apz_bitwise_assign(x, idx, 0);
}

static int apz_add_prim(apz_t *dst, const apz_t *src)
{
    apz_size_t max_used = MAX2(dst->nchunks_used, src->nchunks_used);
    if ((max_used == 1) && \
            (chunk_sum_overflow(dst->chunks[0], src->chunks[0]) == apz_false))
    {
        /* Solo */
        dst->chunks[0] += src->chunks[0];
        return 0;
    }
    if (max_used + 1 > dst->nchunks_capacity)
    {
        /* To extend */
        if (apz_extend_capacity(dst, max_used + 1) != 0) { return -1; }
    }
    {
        /* Do adding */
        apz_chunk_t carry = 0;
        apz_size_t i;
        apz_chunk_t unit_sum;
        for (i = 0; i != max_used; i++)
        {
            apz_chunk_t carry1, carry2, tmpsum;
            chunk_add(&carry1, &tmpsum, GET_CHUNK(dst, i), GET_CHUNK(src, i));
            chunk_add(&carry2, &unit_sum, tmpsum, carry);
            SET_CHUNK(dst, i, unit_sum);
            /* carry1 and carry2 can't be non-zero at the same turn */
            /* Update carry */
            carry = carry1 + carry2;
        }
        chunk_add(&carry, &unit_sum, GET_CHUNK(dst, max_used), carry);
        SET_CHUNK(dst, max_used, unit_sum);
    }
    /* Trim */
    apz_trim(dst);
    return 0;
}

/* Substraction of the numeric part (no signess)
 * Expected ABS(dst) >= ABS(src) */
static int apz_sub_prim(apz_t *dst, const apz_t *src)
{
    apz_size_t max_used = MAX2(dst->nchunks_used, src->nchunks_used);
    apz_size_t i;
    apz_chunk_t carry = 0;
    apz_chunk_t unit_d;
    for (i = 0; i != max_used; i++)
    {
        apz_chunk_t carry1, carry2, tmpd;
        chunk_sub(&carry1, &tmpd, GET_CHUNK(dst, i), GET_CHUNK(src, i));
        chunk_sub(&carry2, &unit_d, tmpd, carry);
        SET_CHUNK(dst, i, unit_d);
        /* carry1 and carry2 can't be non-zero at the same turn */
        /* Update carry */
        carry = carry1 + carry2;
    }
    /* Trim */
    apz_trim(dst);
    return 0;
}

int apz_add(apz_t *dst, const apz_t *src)
{
    if (IS_NON_NEGATIVE(dst) && IS_NON_NEGATIVE(src))
    {
        /* '+' + '+' */
        return apz_add_prim(dst, src);
    }
    else if (IS_NEGATIVE(dst) && IS_NEGATIVE(src))
    {
        /* '-' + '-' */
        return apz_add_prim(dst, src);
    }
    else if (IS_NON_NEGATIVE(dst) && IS_NEGATIVE(src))
    {
        apz_t t;
        /* '+' + '-' */
        switch (apz_cmp_prim(dst, src))
        {
            case apz_ordering_less:
                /* do substracting */
                if (apz_clone(&t, src) != 0) return -1;
                if (apz_sub_prim(&t, dst) != 0)
                {
                    apz_uninit(&t);
                    return -1;
                }
                /* Fix signess */
                TO_NEGATIVE(&t);
                apz_uninit(dst);
                apz_move(dst, &t);
                break;
            case apz_ordering_equal:
                apz_uninit(dst);
                apz_init_zero(dst);
                break;
            case apz_ordering_greater:
                /* do substracting */
                /* Keep 'dst' positive */
                if (apz_sub_prim(dst, src) != 0)
                {
                    return -1;
                }
                break;
        }
    }
    else if (IS_NEGATIVE(dst) && IS_NON_NEGATIVE(src))
    {
        apz_t t;
        /* '-' + '+' */
        switch (apz_cmp_prim(dst, src))
        {
            case apz_ordering_less:
                /* do substracting */
                if (apz_clone(&t, src) != 0) return -1;
                if (apz_sub_prim(&t, dst) != 0)
                {
                    apz_uninit(&t);
                    return -1;
                }
                /* Fix signess */
                TO_NON_NEGATIVE(&t);
                apz_uninit(dst);
                apz_move(dst, &t);
                break;
            case apz_ordering_equal:
                apz_uninit(dst);
                apz_init_zero(dst);
                break;
            case apz_ordering_greater:
                /* do substracting */
                /* Keep 'dst' negative */
                if (apz_sub_prim(dst, src) != 0)
                {
                    return -1;
                }
                break;
        }
    }
    return 0;
}

int apz_add_int(apz_t *dst, const int src)
{
    apz_t src1;
    apz_init_int(&src1, src);
    return apz_add(dst, &src1);
}

int apz_sub(apz_t *dst, const apz_t *src)
{
    apz_t t;
    if (IS_NON_NEGATIVE(dst) && IS_NON_NEGATIVE(src))
    {
        /* '+' - '+' */
        switch (apz_cmp_prim(dst, src))
        {
            case apz_ordering_less:
                if (apz_clone(&t, src) != 0) return -1;
                if (apz_sub_prim(&t, dst) != 0)
                {
                    apz_uninit(&t);
                    return -1;
                }
                /* Fix signess */
                TO_NEGATIVE(&t);
                apz_uninit(dst);
                apz_move(dst, &t);
                break;
            case apz_ordering_equal:
                apz_uninit(dst);
                apz_init_zero(dst);
                break;
            case apz_ordering_greater:
                if (apz_sub_prim(dst, src) != 0)
                {
                    return -1;
                }
                break;
        }
    }
    else if (IS_NEGATIVE(dst) && IS_NEGATIVE(src))
    {
        /* '-' - '-' */
        switch (apz_cmp_prim(dst, src))
        {
            case apz_ordering_less:
                if (apz_clone(&t, src) != 0) return -1;
                if (apz_sub_prim(&t, dst) != 0)
                {
                    apz_uninit(&t);
                    return -1;
                }
                /* Fix signess */
                TO_NON_NEGATIVE(&t);
                apz_uninit(dst);
                apz_move(dst, &t);
                break;
            case apz_ordering_equal:
                apz_uninit(dst);
                apz_init_zero(dst);
                break;
            case apz_ordering_greater:
                if (apz_sub_prim(dst, src) != 0)
                {
                    return -1;
                }
                break;
        }
    }
    else if (IS_NON_NEGATIVE(dst) && IS_NEGATIVE(src))
    {
        /* '+' - '-' */
        return apz_add_prim(dst, src);
    }
    else if (IS_NEGATIVE(dst) && IS_NON_NEGATIVE(src))
    {
        /* '-' - '+' */
        return apz_add_prim(dst, src);
    }
    else
    {
        return -1;
    }
    return 0;
}

int apz_sub_int(apz_t *dst, const int src)
{
    apz_t src1;
    apz_init_int(&src1, src);
    return apz_sub(dst, &src1);
}

static int apz_mul_prim(apz_t *dst, const apz_t *src)
{
    apz_t sum, t;
    apz_size_t src_it, dst_it;
    apz_size_t sum_capacity = dst->nchunks_used + src->nchunks_used;
    apz_bool negative = dst->negative;
    /* Initialize with zero */
    apz_init_zero(&sum);
    /* Multiplication of 'a' and 'b' requires 'a.used + b.used' space */
    if (apz_extend_capacity(&sum, sum_capacity) != 0) return -1;
    /* Temp sum in one turn */
    apz_init_zero(&t);
    if (apz_extend_capacity(&t, sum_capacity) != 0) return -1;
    /*   dst : j: 1st line */
    /* * src : i: 2st line */
    for (src_it = 0; src_it != src->nchunks_used; src_it++)
    {
        apz_chunk_t c_prev = 0;
        for (dst_it = 0; dst_it != dst->nchunks_used; dst_it++)
        {
            apz_chunk_t c, p;
            chunk_mul(&c, &p, GET_CHUNK(src, src_it), GET_CHUNK(dst, dst_it));
            chunk_acc(&c, &p, c_prev);
            SET_CHUNK(&t, dst_it + src_it, p);
            c_prev = c;
        }
        // The remain carry
        SET_CHUNK(&t, dst->nchunks_used + src_it, c_prev);
        /* Add */
        if (apz_add(&sum, &t) != 0)
        {
            goto fail;
        }
        /* Clear used term */
        clear_chunks(&t);
    }
    /* Restore signess */
    sum.negative = negative;
    apz_trim(&sum);
    apz_uninit(dst);
    apz_move(dst, &sum);
    apz_uninit(&t);
    return 0;
fail:
    apz_uninit(&sum);
    apz_uninit(&t);
    return -1;
}

int apz_mul(apz_t *dst, const apz_t *src)
{
    int ret = 0;
    /* 0 * X = 0 */
    if (apz_is_zero(dst))
    {
        /* Nothing to do, 'dst' is already zero */
        return 0;
    }
    /* X * 0 = 0 */
    if (apz_is_zero(src))
    {
        apz_uninit(dst);
        apz_init_zero(dst);
        return 0;
    }
    /* 1 * X = X */
    if (apz_is_one(dst))
    {
        apz_clone(dst, src);
        return 0;
    }
    /* X * 1 = X */
    if (apz_is_one(src))
    {
        /* Nothing to do */
        return 0;
    }
    /* -1 * X = -X */
    if (apz_is_negative_one(dst))
    {
        apz_uninit(dst);
        if (apz_clone(dst, src) != 0)
        {
            return -1;
        }
        apz_negative(dst);
        return 0;
    }
    /* X * -1 = -X */
    if (apz_is_negative_one(src))
    {
        apz_negative(dst);
        return 0;
    }
    /* TODO: If MIN(dst, src) > KARATSUBA_THRESHOLD
     * (768 bit? need more testing), apply karatsuba algorithm */
    /* TODO: If dst equals to src, apply squaring algorithm */
    ret = apz_mul_prim(dst, src);
    if (apz_is_zero(dst))
    {
        /* Signess */
        TO_NON_NEGATIVE(dst);
    }
    else if (IS_NON_NEGATIVE(dst) && (IS_NEGATIVE(src)))
    {
        TO_NEGATIVE(dst);
    }
    else if (IS_NEGATIVE(dst) && (IS_NON_NEGATIVE(src)))
    {
        TO_NEGATIVE(dst);
    }
    else
    {
        TO_NON_NEGATIVE(dst);
    }
    return ret;
}

int apz_mul_int(apz_t *dst, const int src)
{
    apz_t src1;
    apz_init_int(&src1, src);
    return apz_mul(dst, &src1);
}

static int apz_div_mod_prim(apz_t *dst, apz_t *m, const apz_t *src)
{
    int ret = 0;
    apz_t numerator, divisor, quotient, remainder;
    /* apz_size_t bit_delta; */
    apz_size_t i;
    /* Small number divides a large number results zero */
    if (apz_cmp_abs(dst, src) == apz_ordering_less)
    {
        if (m != apz_nullptr)
        {
            apz_move(m, dst);
            apz_init_zero(dst);
        }
        else
        {
            apz_uninit(dst);
            apz_init_zero(dst);
        }
        return 0;
    }
    /* Numerator / Divisor = Quotient ... Remainder */
    apz_init_zero(&numerator);
    apz_init_zero(&divisor);
    apz_init_zero(&quotient);
    apz_init_zero(&remainder);
    /* Division algorithm
     * https://en.wikipedia.org/wiki/Division_algorithm */
    /* Prepare */
    if (apz_clone(&numerator, dst) != 0) { goto fail; }
    if (apz_clone(&divisor, src) != 0) { goto fail; }
    i = apz_bitcount(&numerator);
    do
    {
        i--;
        if (apz_lshift_size_t(&remainder, 1) != 0) { goto fail; }
        apz_bitwise_assign(&remainder, 0, \
                apz_bitwise_extract(&numerator, i));
        if (apz_cmp_prim(&remainder, &divisor) != apz_ordering_less)
        {
            apz_sub_prim(&remainder, &divisor);
            apz_bitwise_set(&quotient, i);
        }
    } while (i != 0);
    apz_uninit(dst);
    apz_move(dst, &quotient);
    if (m != apz_nullptr)
    {
        apz_move(m, &remainder);
    }
    goto done;
fail:
    ret = -1;
done:
    apz_uninit(&numerator);
    apz_uninit(&divisor);
    apz_uninit(&quotient);
    apz_uninit(&remainder);
    return ret;
}

int apz_div(apz_t *dst, const apz_t *src)
{
    apz_bool is_negative;
    /* Divide by zero */
    if (apz_is_zero(src)) return -1;
    /* X / 1 = X */
    if (apz_is_one(src))
    {
        /* Nothing to do */
        return 0;
    }
    /* X / -1 = -X */
    if (apz_is_negative_one(src))
    {
        if (apz_is_zero(dst)) return 0;
        dst->negative = dst->negative == apz_false ? apz_true : apz_false;
        return 0;
    }
    /* X / X = 1 */
    if (apz_cmp(dst, src) == apz_ordering_equal)
    {
        apz_uninit(dst);
        apz_init_one(dst);
        return 0;
    }
    /* 0 / X = 0 */
    if (apz_is_zero(dst))
    {
        apz_uninit(dst);
        apz_init_zero(dst);
        return 0;
    }
    /* abs(x) / abs(y) */
    if (apz_cmp_abs(dst, src) == apz_ordering_less)
    {
        apz_uninit(dst);
        apz_init_zero(dst);
        return 0;
    }
    /* dst and src are not zero */
    is_negative = (dst->negative != src->negative) ? apz_true : apz_false;
    if (apz_div_mod_prim(dst, apz_nullptr, src) != 0)
    {
        return -1;
    }
    dst->negative = is_negative;
    return 0;
}

int apz_div_int(apz_t *dst, const int src)
{
    apz_t src1;
    apz_init_int(&src1, src);
    return apz_div(dst, &src1);
}

static int apz_mod_prim(apz_t *dst, const apz_t *src)
{
    int ret = 0;
    apz_t divisor;
    apz_size_t bit_delta;
    /* Small number modules a large number doesn't change the value */
    if (apz_cmp_abs(dst, src) == apz_ordering_less)
    {
        /* Keep dst */
        return 0;
    }
    /* Clear signess */
    TO_NON_NEGATIVE(dst);
    if (apz_clone(&divisor, src) != 0)
    {
        return -1;
    }
    bit_delta = (apz_size_t)(apz_bitcount(dst) - apz_bitcount(&divisor));
    if (apz_lshift_size_t(&divisor, bit_delta) != 0)
    {
        goto fail;
    }
    while (apz_cmp_prim(dst, &divisor) == apz_ordering_less)
    {
        apz_rshift_int(&divisor, 1);
        bit_delta--;
    }
    /* Divide */
    for (;;)
    {
        apz_sub_prim(dst, &divisor);
        while ((apz_cmp_prim(&divisor, dst) == apz_ordering_greater) && \
                bit_delta > 0)
        {
            apz_rshift_size_t(&divisor, 1);
            bit_delta--;
        }
        if ((bit_delta == 0) && \
                apz_cmp_prim(&divisor, dst) == apz_ordering_greater)
        {
            break;
        }
    }
    goto done;
fail:
    ret = -1;
done:
    apz_uninit(&divisor);
    return ret;
}

int apz_mod(apz_t *dst, const apz_t *src)
{
    apz_bool is_negative;
    /* Divide by zero */
    if (apz_is_zero(src)) return -1;
    /* X % X = 0 */
    if (apz_cmp(dst, src) == apz_ordering_equal)
    {
        apz_uninit(dst);
        apz_init_zero(dst);
        return 0;
    }
    /* dst and src are not zero */
    is_negative = (dst->negative != src->negative) ? apz_true : apz_false;
    if (apz_mod_prim(dst, src) != 0)
    {
        return -1;
    }
    dst->negative = apz_is_zero(dst) ? apz_false : is_negative;
    return 0;
}

int apz_mod_int(apz_t *dst, const int src)
{
    apz_t src1;
    apz_init_int(&src1, src);
    return apz_mod(dst, &src1);
}

int apz_div_mod(apz_t *dst, apz_t *m, const apz_t *src)
{
    apz_bool is_negative;

    /* Divide by zero */
    if (apz_is_zero(src)) return -1;

    /* abs(x) / abs(y) */
    if (apz_cmp_abs(dst, src) == apz_ordering_less)
    {
        if (m != apz_nullptr)
        {
            apz_move(m, dst);
        }
        else
        {
            apz_uninit(dst);
        }
        apz_init_zero(dst);
        return 0;
    }

    is_negative = (dst->negative != src->negative) ? apz_true : apz_false;
    if (apz_div_mod_prim(dst, m, src) != 0)
    {
        return -1;
    }
    dst->negative = is_negative && !apz_is_zero(dst);

    return 0;
}

int apz_div_mod_int(apz_t *dst, apz_t *m, const int src)
{
    apz_t src1;
    apz_init_int(&src1, src);
    return apz_div_mod(dst, m, &src1);
}

int apz_pow(apz_t *dst, const apz_t *src)
{
    int ret = 0;
    apz_t cnt, prod;

    if (apz_is_negative(src))
    {
        return -1;
    }
    else if (apz_is_zero(src))
    {
        apz_uninit(dst);
        apz_init_one(dst);
        return 0;
    }
    if (apz_is_one(src))
    {
        return 0;
    }

    apz_init_one(&prod);
    if (apz_clone(&cnt, src) != 0)
    {
        return -1;
    }

    do
    {
        if (apz_mul(&prod, dst) != 0)
        {
            goto fail;
        }
        apz_dec(&cnt);
    } while (!apz_is_zero(&cnt));

    apz_uninit(dst);
    apz_move(dst, &prod);

    goto done;
fail:
    ret = -1;
done:
    apz_uninit(&prod);
    apz_uninit(&cnt);
    return ret;
}

int apz_pow_int(apz_t *dst, const int src)
{
    apz_t src1;
    apz_init_int(&src1, src);
    return apz_pow(dst, &src1);
}

/* Inc & Dec */

int apz_inc(apz_t *x)
{
    return apz_add_int(x, 1);
}

int apz_dec(apz_t *x)
{
    return apz_sub_int(x, 1);
}

/* Misc */

void apz_abs(apz_t *x)
{
    x->negative = apz_false;
}

/* Order */

apz_ordering_t apz_ordering_reverse(const apz_ordering_t ordering)
{
    if (ordering == apz_ordering_less) return apz_ordering_greater;
    if (ordering == apz_ordering_greater) return apz_ordering_less;
    return apz_ordering_equal;
}

/* Compare without aware the signess */
static apz_ordering_t apz_cmp_prim(const apz_t *x1, const apz_t *x2)
{
    apz_size_t max_used = MAX2_USED(x1, x2);
    apz_size_t i = max_used;
    do
    {
        i--;
        if (GET_CHUNK(x1, i) < GET_CHUNK(x2, i)) return apz_ordering_less;
        if (GET_CHUNK(x1, i) > GET_CHUNK(x2, i)) return apz_ordering_greater;
    }
    while (i != 0);
    return apz_ordering_equal;
}

apz_ordering_t apz_cmp_abs(const apz_t *x1, const apz_t *x2)
{
    return apz_cmp_prim(x1, x2);
}

apz_ordering_t apz_cmp(const apz_t *x1, const apz_t *x2)
{
    if (IS_NON_NEGATIVE(x1))
    {
        if (IS_NON_NEGATIVE(x2))
        {
            /* + & + */
            return apz_cmp_prim(x1, x2);
        }
        else
        {
            /* + & - */
            return apz_ordering_greater;
        }
    }
    else
    {
        if (IS_NON_NEGATIVE(x2))
        {
            /* - & + */
            return apz_ordering_less;
        }
        else
        {
            /* - & - */
            return apz_ordering_reverse(apz_cmp_prim(x1, x2));
        }
    }
}

apz_ordering_t apz_cmp_int(const apz_t *x1, const int x2)
{
    apz_t x2_;
    apz_init_int(&x2_, x2);
    return apz_cmp(x1, &x2_);
}

apz_ordering_t apz_cmp_s64(const apz_t *x1, const apz_s64 x2)
{
    apz_ordering_t ret;
    apz_t x2_;
    apz_init_s64(&x2_, x2);
    ret = apz_cmp(x1, &x2_);
    apz_uninit(&x2_);
    return ret;
}

apz_ordering_t apz_cmp_u64(const apz_t *x1, const apz_u64 x2)
{
    apz_ordering_t ret;
    apz_t x2_;
    apz_init_u64(&x2_, x2);
    ret = apz_cmp(x1, &x2_);
    apz_uninit(&x2_);
    return ret;
}

/* Format */

static apz_size_t apz_formatn_base_nbytes(const apz_t *x, const int base)
{
    apz_t y;
    apz_size_t n = 0;
    if (base <= 0 || base > 36) return 0;
    /* Zero */
    if (apz_is_zero(x)) return 1;
    if (apz_clone(&y, x) != 0) return 0;
    /* Negative */
    if (apz_is_negative(&y))
    {
        n++;
        apz_abs(&y);
    }
    while (!apz_is_zero(&y))
    {
        n++;
        if (apz_div_int(&y, base) != 0)
        {
            goto fail;
        }
    }
    apz_uninit(&y);
    return n;
fail:
    apz_uninit(&y);
    return 0;
}

int apz_formatn_base(char *str, size_t size, const apz_t *x, const int base)
{
    char *p;
    apz_t y;
    apz_bool is_negative = x->negative;
    /* Measure the required number of bytes */
    apz_size_t nbytes = apz_formatn_base_nbytes(x, base);
    if ((nbytes == 0) || (nbytes >= size)) return -1;
    p = str + nbytes;
    *p-- = '\0';
    /* Zero */
    if (apz_is_zero(x))
    {
        *p = '0';
        return 0;
    }
    if (apz_clone(&y, x) != 0)
    {
        return -1;
    }
    apz_abs(&y);
    while (!apz_is_zero(&y))
    {
        apz_t m;
        unsigned int m_i;
        if (apz_div_mod_int(&y, &m, base) != 0)
        {
            apz_uninit(&y);
            return -1;
        }
        m_i = apz_to_uint(&m);
        apz_uninit(&m);
        if (m_i <= 9)
        {
            *p = (char)('0' + m_i);
            p--;
        }
        else if (m_i < 36)
        {
            *p = (char)('A' + m_i) - 10;
            p--;
        }
    }
    if (is_negative) { *p = '-'; }
    apz_uninit(&y);
    return 0;
}

int apz_formatn_binary(char *str, apz_size_t size, const apz_t *x)
{
    return apz_formatn_base(str, size, x, 2);
}

int apz_formatn_octal(char *str, apz_size_t size, const apz_t *x)
{
    return apz_formatn_base(str, size, x, 8);
}

int apz_formatn_decimal(char *str, apz_size_t size, const apz_t *x)
{
    return apz_formatn_base(str, size, x, 10);
}

int apz_formatn_hexadecimal(char *str, apz_size_t size, const apz_t *x)
{
    return apz_formatn_base(str, size, x, 16);
}

/* Inspector */

apz_size_t apz_inspect_nchunks_used(apz_t *x)
{
    return x->nchunks_used;
}

apz_size_t apz_inspect_nchunks_capacity(apz_t *x)
{
    return x->nchunks_capacity;
}

apz_bool apz_inspect_negative(apz_t *x)
{
    return x->negative;
}

apz_chunk_t apz_inspect_chunks(apz_t *x, const apz_size_t idx)
{
    return x->chunks[idx];
}

apz_size_t apz_inspect_chunk_size(void)
{
    return sizeof(apz_chunk_t);
}

