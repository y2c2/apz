#include "testfw.h"
#include "apz.h"
#include "test_format.h"

void test_format(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Format");

#define TEST_CASE_S(s, base, expected, expected_base) \
    do { \
        char buf[256]; \
        apz_t x; \
        CUNITTEST_ASSERT_EQ(apz_init_str_base(&x, s, base), 0); \
        CUNITTEST_ASSERT_EQ(apz_formatn_base(buf, 256, &x, expected_base), 0); \
        CUNITTEST_ASSERT_STREQ(buf, expected); \
        apz_uninit(&x); \
    } while (0)

    /* Hexadecimal */
    TEST_CASE_S("0", 16, "0", 16);
    TEST_CASE_S("1", 16, "1", 16);
    TEST_CASE_S("9", 16, "9", 16);
    TEST_CASE_S("A", 16, "A", 16);
    TEST_CASE_S("F", 16, "F", 16);
    TEST_CASE_S("10", 16, "10", 16);
    TEST_CASE_S("FF", 16, "FF", 16);
    TEST_CASE_S("FFFF", 16, "FFFF", 16);
    TEST_CASE_S("FFFFFFFF", 16, "FFFFFFFF", 16);
    TEST_CASE_S("FFFFFFFFFFFFFFFF", 16, "FFFFFFFFFFFFFFFF", 16);
    TEST_CASE_S("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 16, \
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 16);
    /* Binary */
    TEST_CASE_S("0", 16, "0", 2);
    TEST_CASE_S("1", 16, "1", 2);
    TEST_CASE_S("2", 16, "10", 2);
    TEST_CASE_S("3", 16, "11", 2);
    TEST_CASE_S("4", 16, "100", 2);
    TEST_CASE_S("8", 16, "1000", 2);

    /* Octal */
    TEST_CASE_S("0", 16, "0", 8);
    TEST_CASE_S("1", 16, "1", 8);
    TEST_CASE_S("2", 16, "2", 8);
    TEST_CASE_S("3", 16, "3", 8);
    TEST_CASE_S("4", 16, "4", 8);
    TEST_CASE_S("7", 16, "7", 8);
    TEST_CASE_S("8", 16, "10", 8);

    /* Decimal */
    TEST_CASE_S("0", 16, "0", 10);
    TEST_CASE_S("1", 16, "1", 10);
    TEST_CASE_S("2", 16, "2", 10);
    TEST_CASE_S("3", 16, "3", 10);
    TEST_CASE_S("4", 16, "4", 10);
    TEST_CASE_S("9", 16, "9", 10);
    TEST_CASE_S("A", 16, "10", 10);
    TEST_CASE_S("B", 16, "11", 10);
    TEST_CASE_S("F", 16, "15", 10);
    TEST_CASE_S("10", 16, "16", 10);

    /* Negative */
    TEST_CASE_S("-0", 10, "0", 10);
    TEST_CASE_S("-1", 10, "-1", 10);
    TEST_CASE_S("-123", 10, "-123", 10);

    CUNITTEST_RESULT();
}

