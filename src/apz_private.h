/* Arbitrary-Precision Interger : Private
 * Copyright(c) 2019-2020 y2c2 */

#ifndef APZ_PRIVATE_H
#define APZ_PRIVATE_H

#include "apz_dt.h"
#include "apz_config.h"

#define APZ_CHUNK_MAX_32 4294967295U
#define APZ_CHUNK_MAX_64 18446744073709551615UL

/* Chunk unit */
#if defined(APZ_CHUNK_32)
typedef apz_u32 apz_chunk_t;
typedef apz_u16 apz_chunk_half_t;
#define apz_chunk_size 32
#define apz_chunk_max APZ_CHUNK_MAX_32
#define apz_chunk_high_half(x) (((x) >> 16) & 0xFFFF)
#define apz_chunk_low_half(x) ((x) & 0xFFFF)
#define apz_chunk_make_by_half(_h, _l) (((_h) << 16) | (_l))
#elif defined(APZ_CHUNK_64)
typedef apz_u64 apz_chunk_t;
typedef apz_u32 apz_chunk_half_t;
#define apz_chunk_size 64
#define apz_chunk_max APZ_CHUNK_MAX_64
#define apz_chunk_high_half(x) (((x) >> 32) & 0xFFFFFFFF)
#define apz_chunk_low_half(x) ((x) & 0xFFFFFFFF)
#define apz_chunk_make_by_half(_h, _l) (((_h) << 32) | (_l))
#endif

/* Fundamental structure to keep the data
 * of an Integer (Z) */

struct apz_
{
    /* How many chunks used */
    apz_size_t nchunks_used;
    apz_size_t nchunks_capacity;
    /* Signed */
    apz_bool negative;
    /* Chunks data */
    apz_chunk_t solo_placeholder;
    /* Point to solo_placeholder if solo */
    apz_chunk_t *chunks;
};
typedef struct apz_ apz_t;

#endif
