/* Arbitrary-Precision Interger : Data Types
 * Copyright(c) 2019-2020 y2c2 */

#ifndef APZ_DT_H
#define APZ_DT_H

#ifndef __cplusplus
typedef enum
{
    apz_false = 0,
    apz_true = 1,
} apz_bool;
#else
typedef enum
{
    apz_false = false,
    apz_true = true,
} apz_bool;
#endif

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdint.h>
#include <stdio.h>
typedef uint64_t apz_u64;
typedef uint32_t apz_u32;
typedef int32_t apz_s32;
typedef int64_t apz_s64;
typedef uint16_t apz_u16;
typedef int16_t apz_s16;
typedef uint8_t apz_u8;
typedef int8_t apz_s8;
typedef size_t apz_size_t;
#else
typedef unsigned long long apz_u64;
typedef unsigned int apz_u32;
typedef unsigned short int apz_u16;
typedef unsigned char apz_u8;
typedef signed long long apz_s64;
typedef signed int apz_s32;
typedef signed short int apz_s16;
typedef char apz_s8;
typedef unsigned int apz_size_t;
#endif

typedef apz_u32 apz_char_t;
typedef apz_size_t apz_offset_t;
typedef apz_u8 apz_byte_t;

#define apz_nullptr ((void *)0)

/* 32-bit gap */
#define apz_s32_min (-2147483648)
#define apz_s32_max (2147483647)

/* 64-bit gap */
#define apz_s64_max (9223372036854775807L)
#define apz_s64_min (-apz_s64_max - 1L)

/* Chunks */
#define APZ_NCHUNKS_USED_SOLO 1
#define APZ_NCHUNKS_CAPACITY_SOLO 1
#define APZ_NCHUNKS_USED_DUO 2
#define APZ_NCHUNKS_CAPACITY_DUO 2
#define APZ_NCHUNKS_USED_TRI 3
#define APZ_NCHUNKS_CAPACITY_TRI 3

#endif

