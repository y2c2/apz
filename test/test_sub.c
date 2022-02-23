#include "testfw.h"
#include "apz.h"
#include "test_sub.h"

void test_sub(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Sub");

#define CHECK_BASIC(_x, _used, _capacity, _negative) \
    do { \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_used(&_x), _used); \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_capacity(&_x), _capacity); \
        CUNITTEST_ASSERT_EQ(apz_inspect_negative(&_x), _negative); \
    } while (0)

    /* '0' - '0' */
    {
        apz_t x, y;
        apz_init_int(&x, 0);
        apz_init_int(&y, 0);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
    }
    /* '0' - '1' */
    {
        apz_t x, y;
        apz_init_int(&x, 0);
        apz_init_int(&y, 1);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 1);
    }
    /* '+' - '+' #1 */
    {
        apz_t x, y;
        apz_init_int(&x, 1);
        apz_init_int(&y, 3);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* '+' - '+' #2 */
    {
        apz_t x, y;
        apz_init_int(&x, 3);
        apz_init_int(&y, 3);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
    }
    /* '+' - '+' #3 */
    {
        apz_t x, y;
        apz_init_int(&x, 3);
        apz_init_int(&y, 1);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* '-' - '-' #1 */
    {
        apz_t x, y;
        apz_init_int(&x, -1);
        apz_init_int(&y, -3);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* '-' - '-' #2 */
    {
        apz_t x, y;
        apz_init_int(&x, -3);
        apz_init_int(&y, -3);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
    }
    /* '-' - '-' #3 */
    {
        apz_t x, y;
        apz_init_int(&x, -3);
        apz_init_int(&y, -1);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* '+' - '-' */
    {
        apz_t x, y;
        apz_init_int(&x, 1);
        apz_init_int(&y, -2);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 3);
    }
    /* '-' - '+' */
    {
        apz_t x, y;
        apz_init_int(&x, -1);
        apz_init_int(&y, 2);
        apz_sub(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 3);
    }

    CUNITTEST_RESULT();
}
