#include "testfw.h"
#include "apz.h"
#include "test_add.h"

void test_add(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Add");

#define CHECK_BASIC(_x, _used, _capacity, _negative) \
    do { \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_used(&_x), _used); \
        CUNITTEST_ASSERT_EQ(apz_inspect_nchunks_capacity(&_x), _capacity); \
        CUNITTEST_ASSERT_EQ(apz_inspect_negative(&_x), _negative); \
    } while (0)

    /* '+' + '+'*/
    {
        apz_t x, y;
        apz_init_int(&x, 1);
        apz_init_int(&y, 2);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 3);
    }
    /* '-' + '-'*/
    {
        apz_t x, y;
        apz_init_int(&x, -1);
        apz_init_int(&y, -2);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 3);
    }
    /* '+' + '-' #1 */
    {
        apz_t x, y;
        apz_init_int(&x, 1);
        apz_init_int(&y, -3);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* '+' + '-' #2 */
    {
        apz_t x, y;
        apz_init_int(&x, 3);
        apz_init_int(&y, -3);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
    }
    /* '+' + '-' #3 */
    {
        apz_t x, y;
        apz_init_int(&x, 3);
        apz_init_int(&y, -1);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* '-' + '+' #1 */
    {
        apz_t x, y;
        apz_init_int(&x, -1);
        apz_init_int(&y, 3);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* '-' + '+' #2 */
    {
        apz_t x, y;
        apz_init_int(&x, -3);
        apz_init_int(&y, 3);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
    }
    /* '-' + '+' #3 */
    {
        apz_t x, y;
        apz_init_int(&x, -3);
        apz_init_int(&y, 1);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 2);
    }
    /* -0x10000000000000000 + 1 */
    {
        apz_t x, y;
        apz_init_str_base(&x, "-10000000000000000", 16);
        apz_init_int(&y, 1);
        apz_add(&x, &y);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFF);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 0xFFFFFFFF);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFFFFFFFFFFUL);
#endif
        apz_uninit(&x);
        apz_uninit(&y);
    }
    /* -0x100000000 + 1 */
    {
        apz_t x, y;
        apz_init_str_base(&x, "-100000000", 16);
        apz_init_int(&y, 1);
        apz_add(&x, &y);
        CHECK_BASIC(x, 1, 1, apz_true);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0xFFFFFFFF);
        apz_uninit(&x);
        apz_uninit(&y);
    }
    /* 0xFFFFFFFF + 1 */
    {
        apz_t x, y;
        apz_init_u32(&x, 0xFFFFFFFF);
        apz_init_int(&y, 1);
        apz_add(&x, &y);
#if defined(APZ_CHUNK_32)
        CHECK_BASIC(x, 2, 2, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 1), 1);
#elif defined(APZ_CHUNK_64)
        CHECK_BASIC(x, 1, 1, apz_false);
        CUNITTEST_ASSERT_EQ(apz_inspect_chunks(&x, 0), 0x100000000UL);
#endif
        apz_uninit(&x);
        apz_uninit(&y);
    }
    /* 0xFFFFFFFFFFFFFFFF + 1 */
    {
        apz_t x, y;
        apz_init_u64(&x, 0xFFFFFFFFFFFFFFFF);
        apz_init_int(&y, 1);
        apz_add(&x, &y);
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
        apz_uninit(&y);
    }

    CUNITTEST_RESULT();
}
