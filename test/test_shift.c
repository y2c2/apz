#include "testfw.h"
#include "apz.h"
#include "test_shift.h"

void test_shift(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Shift");

#define CHECK_BASIC(_x, _used, _capacity, _negative) \
    do { \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_used(&_x), _used); \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_capacity(&_x), _capacity); \
        CUNITTEST_ASSERT_EQ(apz_inspect_negative(&_x), _negative); \
    } while (0)

#define TEST_CASE_INT(lhs, shift_func, rhs, expected) \
    do { \
        apz_t x; \
        apz_init_int(&x, lhs); \
        apz_##shift_func##_int(&x, rhs); \
        CHECK_BASIC(x, 1, 1, apz_false); \
        CUNITTEST_ASSERT_EQ(apz_to_int(&x), expected); \
        apz_uninit(&x); \
    } while (0)

#define TEST_CASE_STR_HEX(lhs, shift_func, rhs, expected) \
    do { \
        apz_t x, y; \
        apz_init_str_base(&x, lhs, 16); \
        apz_init_str_base(&y, expected, 16); \
        apz_##shift_func##_int(&x, rhs); \
        CUNITTEST_ASSERT_EQ(apz_cmp(&x, &y), apz_ordering_equal); \
        apz_uninit(&x); \
        apz_uninit(&y); \
    } while (0)

    TEST_CASE_INT(0, rshift, 0, 0);
    TEST_CASE_INT(0, rshift, 1, 0);
    TEST_CASE_INT(0, rshift, 32, 0);
    TEST_CASE_INT(0, rshift, 64, 0);

    TEST_CASE_INT(1, rshift, 0, 1);
    TEST_CASE_INT(1, rshift, 1, 0);
    TEST_CASE_INT(1, rshift, 32, 0);
    TEST_CASE_INT(1, rshift, 64, 0);

    TEST_CASE_INT(2, rshift, 0, 2);
    TEST_CASE_INT(2, rshift, 1, 1);
    TEST_CASE_INT(2, rshift, 2, 0);
    TEST_CASE_INT(2, rshift, 32, 0);
    TEST_CASE_INT(2, rshift, 64, 0);

    TEST_CASE_STR_HEX("10000000000000000", rshift, 1, "8000000000000000");
    TEST_CASE_STR_HEX("10000000000000000", rshift, 2, "4000000000000000");
    TEST_CASE_STR_HEX("10000000000000000", rshift, 3, "2000000000000000");
    TEST_CASE_STR_HEX("10000000000000001", rshift, 63, "2");
    TEST_CASE_STR_HEX("10000000000000001", rshift, 64, "1");
    TEST_CASE_STR_HEX("10000000000000001", rshift, 65, "0");
    TEST_CASE_STR_HEX("10000000000000001", rshift, 66, "0");
    TEST_CASE_STR_HEX("100000000", rshift, 1, "80000000");

    TEST_CASE_INT(0, lshift, 0, 0);
    TEST_CASE_INT(0, lshift, 1, 0);
    TEST_CASE_INT(0, lshift, 32, 0);
    TEST_CASE_INT(0, lshift, 64, 0);

    TEST_CASE_STR_HEX("1", lshift, 0, "1");
    TEST_CASE_STR_HEX("1", lshift, 1, "2");
    TEST_CASE_STR_HEX("1", lshift, 30, "40000000");
    TEST_CASE_STR_HEX("1", lshift, 31, "80000000");
    TEST_CASE_STR_HEX("1", lshift, 32, "100000000");
    TEST_CASE_STR_HEX("1", lshift, 33, "200000000");
    TEST_CASE_STR_HEX("1", lshift, 62, "4000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 63, "8000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 64, "10000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 65, "20000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 66, "40000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 95, "800000000000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 96, "1000000000000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 97, "2000000000000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 127, "80000000000000000000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 128, "100000000000000000000000000000000");
    TEST_CASE_STR_HEX("1", lshift, 129, "200000000000000000000000000000000");

#undef TEST_CASE_INT
#undef TEST_CASE_STR
#undef CHECK_BASIC

    CUNITTEST_RESULT();
}

