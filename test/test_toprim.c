#include <limits.h>
#include <float.h>
#include <stdint.h>
#include "testfw.h"
#include "apz.h"
#include "test_toprim.h"

void test_toprim(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("To Prim");

#define TEST_CASE(case_type, rtype, from) \
    do { \
        apz_t x; \
        rtype to_value; \
        apz_init_##case_type(&x, from); \
        CUNITTEST_ASSERT_EQ(apz_is_valid_##case_type(&x), apz_true); \
        to_value = apz_to_##case_type(&x); \
        CUNITTEST_ASSERT_EQ(to_value, from); \
        apz_uninit(&x); \
    } while (0)

#define TEST_CASE_FLOAT(from) \
    do { \
        apz_t x; \
        float f; \
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, (apz_u64)(from)), 0); \
        CUNITTEST_ASSERT_EQ(apz_is_valid_float(&x), apz_true); \
        f = apz_to_float(&x); \
        CUNITTEST_ASSERT_EQ((apz_u64)f, (apz_u64)(from)); \
    } while (0)

#define TEST_CASE_DOUBLE(from) \
    do { \
        apz_t x; \
        double f; \
        CUNITTEST_ASSERT_EQ(apz_init_s64(&x, (apz_u64)(from)), 0); \
        CUNITTEST_ASSERT_EQ(apz_is_valid_float(&x), apz_true); \
        f = apz_to_double(&x); \
        CUNITTEST_ASSERT_EQ((apz_u64)f, (apz_u64)(from)); \
    } while (0)

    /* int */
    {
        TEST_CASE(int, int, 0);
        TEST_CASE(int, int, 1);
        TEST_CASE(int, int, 2);
        TEST_CASE(int, int, -1);
        TEST_CASE(int, int, -2);
        TEST_CASE(int, int, INT_MIN);
        TEST_CASE(int, int, INT_MAX);
    }

    /* uint */
    {
        TEST_CASE(uint, unsigned int, 0);
        TEST_CASE(uint, unsigned int, INT_MAX);
        TEST_CASE(uint, unsigned int, UINT_MAX);
    }

    /* u64 */
    {
        TEST_CASE(u64, uint64_t, 0);
        TEST_CASE(u64, uint64_t, 18446744073709551615UL);
    }

    /* s64 */
    {
        TEST_CASE(s64, int64_t, 0);
        TEST_CASE(s64, int64_t, 9223372036854775807L);
        TEST_CASE(s64, int64_t, -9223372036854775807L - 1L);
    }

    /* float */
    {
        TEST_CASE_FLOAT(0.0);
        TEST_CASE_FLOAT(1.0);
        TEST_CASE_FLOAT(-1.0);
        TEST_CASE_FLOAT(123.0);
        TEST_CASE_FLOAT(-123.0);
    }

    /* double */
    {
        TEST_CASE_DOUBLE(0.0);
        TEST_CASE_DOUBLE(1.0);
        TEST_CASE_DOUBLE(-1.0);
        TEST_CASE_DOUBLE(123.0);
        TEST_CASE_DOUBLE(-123.0);
    }

    CUNITTEST_RESULT();
}

