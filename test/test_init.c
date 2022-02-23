#include <limits.h>
#include "testfw.h"
#include "apz.h"
#include "test_init.h"

/* #define INT64_MAX 9223372036854775807L */
/* #define INT64_MIN (-INT64_MAX - 1L) */

void test_init(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Init");

#define CHECK_BASIC(_x, _used, _capacity, _negative) \
    do { \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_used(&_x), _used); \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_capacity(&_x), _capacity); \
        CUNITTEST_ASSERT_EQ(apz_inspect_negative(&_x), _negative); \
    } while (0)

    /* int : 0 */
    {
        apz_t x;
        apz_init_zero(&x);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
    }
    /* int : 1 */
    {
        apz_t x;
        apz_init_one(&x);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 1);
    }
    /* int : 2 */
    {
        apz_t x;
        apz_init_int(&x, 2);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* int : 123 */
    {
        apz_t x;
        apz_init_int(&x, 123);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 123);
    }
    /* int : -1 */
    {
        apz_t x;
        apz_init_int(&x, -1);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 1);
    }
    /* int : INT_MAX */
    {
        apz_t x;
        apz_init_int(&x, INT_MAX);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), INT_MAX);
    }
    /* int : INT_MIN */
    {
        apz_t x;
        apz_init_int(&x, INT_MIN);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), (apz_chunk_t)(-(int64_t)(INT_MIN)));
    }
    /* uint : UINT_MAX */
    {
        apz_t x;
        apz_init_uint(&x, UINT_MAX);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ((apz_chunk_t)apz_inspect_chunks(&x, 0), UINT_MAX);
    }

    /* s64 : INT64_MAX */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, INT64_MAX), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), INT64_MAX & 0xFFFFFFFF);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), INT64_MAX >> 32);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), INT64_MAX);
#endif
        apz_uninit(&x);
    }
    /* s64 : INT64_MIN */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, INT64_MIN), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 3, 3, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 2), 1);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 2, 2, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 1);
#endif
        apz_uninit(&x);
    }
    /* s64 : INT32_MAX */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, INT32_MAX), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), INT32_MAX);
        apz_uninit(&x);
    }
    /* s64 : INT32_MIN */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, INT32_MIN), 0);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), -(apz_chunk_t)INT32_MIN);
        apz_uninit(&x);
    }
    /* s64 : INT16_MAX */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, INT16_MAX), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), INT16_MAX);
        apz_uninit(&x);
    }
    /* s64 : INT16_MIN */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, INT16_MIN), 0);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), -(apz_chunk_t)INT16_MIN);
        apz_uninit(&x);
    }

    /* u64 : UINT64_MAX */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_u64(&x, UINT64_MAX), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), UINT64_MAX & 0xFFFFFFFF);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), UINT64_MAX >> 32);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), UINT64_MAX);
#endif
        apz_uninit(&x);
    }
    /* u64 : UINT32_MAX */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_u64(&x, UINT32_MAX), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), UINT32_MAX);
        apz_uninit(&x);
    }
    /* str : 0 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "0"), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        apz_uninit(&x);
    }
    /* str : 1 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "1"), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 1);
        apz_uninit(&x);
    }
    /* str : 123 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "123"), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 123);
        apz_uninit(&x);
    }
    /* str : 4294967295 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "4294967295"), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 4294967295U);
        apz_uninit(&x);
    }
    /* str : 4294967296 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "4294967296"), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 1);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 4294967296U);
#endif
        apz_uninit(&x);
    }
    /* str : 18446744073709551615 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "18446744073709551615"), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFF);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 0xFFFFFFFF);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 18446744073709551615UL);
#endif
        apz_uninit(&x);
    }
    /* str : 18446744073709551616 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "18446744073709551616"), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 3, 3, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 2), 1);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 2, 2, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 1);
#endif
        apz_uninit(&x);
    }
    /* str : -1 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-1"), 0);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 1);
        apz_uninit(&x);
    }
    /* str : -0 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-0"), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        apz_uninit(&x);
    }
    /* str : -123 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-123"), 0);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 123);
        apz_uninit(&x);
    }
    /* str : -2147483647 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-2147483647"), 0);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2147483647);
        apz_uninit(&x);
    }
    /* str : -4294967295 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-4294967295"), 0);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 4294967295UL);
        apz_uninit(&x);
    }
    /* str : -4294967296 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-4294967296"), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 1);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 4294967296UL);
#endif
        apz_uninit(&x);
    }
    /* str : -18446744073709551615 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-18446744073709551615"), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFF);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 0xFFFFFFFF);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 18446744073709551615UL);
#endif
        apz_uninit(&x);
    }
    /* str : -18446744073709551616 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str(&x, "-18446744073709551616"), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 3, 3, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 2), 1);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 2, 2, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 1);
#endif
        apz_uninit(&x);
    }
    /* str 16 : 0 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "0", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        apz_uninit(&x);
    }
    /* str 16 : 1 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "1", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 1);
        apz_uninit(&x);
    }
    /* str 16 : 9 */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "9", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 9);
        apz_uninit(&x);
    }
    /* str 16 : a */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "a", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xa);
        apz_uninit(&x);
    }
    /* str 16 : f */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "f", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xf);
        apz_uninit(&x);
    }
    /* str 16 : A */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "A", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xa);
        apz_uninit(&x);
    }
    /* str 16 : F */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "F", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xf);
        apz_uninit(&x);
    }
    /* str 16 : FFFF */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "FFFF", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFF);
        apz_uninit(&x);
    }
    /* str 16 : FFFFFFFF */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "FFFFFFFF", 16), 0);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFF);
        apz_uninit(&x);
    }
    /* str 16 : FFFFFFFFFFFFFFFF */
    {
        apz_t x;
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, "FFFFFFFFFFFFFFFF", 16), 0);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFF);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 0xFFFFFFFF);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFFFFFFFFFF);
#endif
        apz_uninit(&x);
    }

    CUNITTEST_RESULT();
}

