#include "testfw.h"
#include "apz.h"
#include "test_div.h"

void test_div(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Div");

#define CHECK_BASIC(_x, _used, _capacity, _negative) \
    do { \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_used(&_x), _used); \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_capacity(&_x), _capacity); \
        CUNITTEST_ASSERT_EQ(apz_inspect_negative(&_x), _negative); \
    } while (0)

#define TEST_CASE_INT(lhs, rhs, expected) \
    do { \
        apz_t x, y, z; \
        apz_init_int(&x, lhs); \
        apz_init_int(&y, rhs); \
        apz_init_int(&z, expected); \
        CUNITTEST_ASSERT_EQ(apz_div(&x, &y), 0); \
        CUNITTEST_ASSERT_EQ( \
                apz_cmp(&x, &z), apz_ordering_equal); \
        apz_uninit(&x); \
        apz_uninit(&y); \
        apz_uninit(&z); \
    } while (0)

#define TEST_CASE_STR_HEX(lhs, rhs, expected) \
    do { \
        apz_t x, y, z; \
        apz_init_str_base(&x, lhs, 16); \
        apz_init_str_base(&y, rhs, 16); \
        apz_init_str_base(&z, expected, 16); \
        CUNITTEST_ASSERT_EQ(apz_div(&x, &y), 0); \
        CUNITTEST_ASSERT_EQ(apz_cmp(&x, &z), apz_ordering_equal); \
        apz_uninit(&x); \
        apz_uninit(&y); \
        apz_uninit(&z); \
    } while (0)

    TEST_CASE_INT(0, 2, 0);
    TEST_CASE_INT(123, 1, 123);
    TEST_CASE_INT(123, 123, 1);
    TEST_CASE_INT(123, -1, -123);
    TEST_CASE_INT(1, 2, 0);
    TEST_CASE_INT(1, 3, 0);

    TEST_CASE_INT(6, 2, 3);
    TEST_CASE_INT(-6, -2, 3);
    TEST_CASE_INT(-6, 2, -3);
    TEST_CASE_INT(6, -2, -3);

    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "10", "FFFFFFFFFFFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "100", "FFFFFFFFFFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "1000", "FFFFFFFFFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "10000", "FFFFFFFFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "100000", "FFFFFFFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "1000000", "FFFFFFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "10000000", "FFFFFFFFF");

    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "100000000", "FFFFFFFF");

    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "1000000000", "FFFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "10000000000", "FFFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "100000000000", "FFFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "1000000000000", "FFFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "10000000000000", "FFF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "100000000000000", "FF");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "1000000000000000", "F");
    TEST_CASE_STR_HEX("FFFFFFFFFFFFFFFF", "10000000000000000", "0");

#undef TEST_CASE_INT
#undef TEST_CASE_STR_HEX

    CUNITTEST_RESULT();
}
