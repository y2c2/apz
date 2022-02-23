#include "testfw.h"
#include "apz.h"
#include "test_mod.h"

void test_mod(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Mod");

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
        CUNITTEST_ASSERT_EQ(apz_mod(&x, &y), 0); \
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
        CUNITTEST_ASSERT_EQ(apz_mod(&x, &y), 0); \
        CUNITTEST_ASSERT_EQ(apz_cmp(&x, &z), apz_ordering_equal); \
        apz_uninit(&x); \
        apz_uninit(&y); \
        apz_uninit(&z); \
    } while (0)

    TEST_CASE_INT(0, 2, 0);
    TEST_CASE_INT(123, 123, 0);
    TEST_CASE_INT(1, 2, 1);
    TEST_CASE_INT(1, 3, 1);

    TEST_CASE_INT(6, 2, 0);
    TEST_CASE_INT(-6, -2, 0);
    TEST_CASE_INT(-6, 2, 0);
    TEST_CASE_INT(6, -2, 0);

#undef TEST_CASE_INT
#undef TEST_CASE_STR_HEX

    CUNITTEST_RESULT();
}

