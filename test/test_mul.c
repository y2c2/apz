#include "testfw.h"
#include "apz.h"
#include "test_mul.h"

void test_mul(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Mul");

#define CHECK_BASIC(_x, _used, _capacity, _negative) \
    do { \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_used(&_x), _used); \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_capacity(&_x), _capacity); \
        CUNITTEST_ASSERT_EQ(apz_inspect_negative(&_x), _negative); \
    } while (0)

#define TEST_CASE_INT(lhs, rhs, expected) \
    do { \
        apz_t x, y; \
        apz_init_int(&x, lhs); \
        apz_init_int(&y, rhs); \
        CUNITTEST_ASSERT_EQ(apz_mul(&x, &y), 0); \
        CUNITTEST_ASSERT_EQ(apz_to_int(&x), expected); \
        apz_uninit(&x); \
        apz_uninit(&y); \
    } while (0)

#define TEST_CASE_STR_HEX(lhs, rhs, expected) \
    do { \
        apz_t x, y, z; \
        apz_init_str_base(&x, lhs, 16); \
        apz_init_str_base(&y, rhs, 16); \
        apz_init_str_base(&z, expected, 16); \
        CUNITTEST_ASSERT_EQ(apz_mul(&x, &y), 0); \
        CUNITTEST_ASSERT_EQ(apz_cmp(&x, &z), apz_ordering_equal); \
        apz_uninit(&x); \
        apz_uninit(&y); \
        apz_uninit(&z); \
    } while (0)

    TEST_CASE_INT(0, 0, 0);
    TEST_CASE_INT(0, 123, 0);
    TEST_CASE_INT(123, 0, 0);
    TEST_CASE_INT(1, 123, 123);
    TEST_CASE_INT(123, 1, 123);
    TEST_CASE_INT(2, 3, 6);
    TEST_CASE_INT(-2, -3, 6);
    TEST_CASE_INT(2, -3, -6);
    TEST_CASE_INT(-2, 3, -6);
    TEST_CASE_INT(0, 0, 0);

    TEST_CASE_STR_HEX("80000000", "2", "100000000");
    TEST_CASE_STR_HEX("80000000", "4", "200000000");
    TEST_CASE_STR_HEX("80000000", "8", "400000000");
    TEST_CASE_STR_HEX("80000000", "10", "800000000");
    TEST_CASE_STR_HEX("8000000000000000", "2", "10000000000000000");

#undef TEST_CASE_INT
#undef TEST_CASE_STR_HEX

    CUNITTEST_RESULT();
}
