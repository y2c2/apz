#include "testfw.h"
#include "apz.h"
#include "test_pow.h"

void test_pow(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Pow");

#define TEST_CASE_INT(lhs, rhs, expected) \
    do { \
        apz_t x, y, z; \
        apz_init_int(&x, lhs); \
        apz_init_int(&y, rhs); \
        apz_init_int(&z, expected); \
        CUNITTEST_ASSERT_EQ(apz_pow(&x, &y), 0); \
        CUNITTEST_ASSERT_EQ( \
                apz_cmp(&x, &z), apz_ordering_equal); \
        apz_uninit(&x); \
        apz_uninit(&y); \
        apz_uninit(&z); \
    } while (0)

    {
        TEST_CASE_INT(0, 0, 1);
        TEST_CASE_INT(0, 1, 0);
        TEST_CASE_INT(1, 1, 1);
        TEST_CASE_INT(1, 2, 1);
        TEST_CASE_INT(1, 3, 1);
        TEST_CASE_INT(1, 128, 1);
        TEST_CASE_INT(2, 0, 1);
        TEST_CASE_INT(2, 1, 2);
        TEST_CASE_INT(2, 2, 4);
        TEST_CASE_INT(2, 3, 8);
    }
    CUNITTEST_RESULT();
}
