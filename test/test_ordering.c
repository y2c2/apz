#include "testfw.h"
#include "apz.h"
#include "test_ordering.h"

void test_ordering(void)
{
    apz_t zero;
    apz_t one;
    apz_t two;
    apz_t neg_one;
    apz_t neg_two;
    apz_t arr[5];
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Ordering");

    apz_init_zero(&zero);
    apz_init_one(&one);
    apz_init_int(&two, 2);
    apz_init_int(&neg_one, -1);
    apz_init_int(&neg_two, -2);
    {
        int i;
        for (i = 0; i != 5; i++)
        {
            int x = i - 2;
            apz_init_int(&arr[i], (int)x);
        }
    }

    {
        CUNITTEST_ASSERT_EQ(apz_ordering_reverse(apz_ordering_less), apz_ordering_greater);
        CUNITTEST_ASSERT_EQ(apz_ordering_reverse(apz_ordering_greater), apz_ordering_less);
        CUNITTEST_ASSERT_EQ(apz_ordering_reverse(apz_ordering_equal), apz_ordering_equal);
    }

    {
        CUNITTEST_ASSERT_EQ(apz_cmp(&zero, &zero), apz_ordering_equal);
        CUNITTEST_ASSERT_EQ(apz_cmp(&one, &one), apz_ordering_equal);
        CUNITTEST_ASSERT_EQ(apz_cmp(&two, &two), apz_ordering_equal);
        CUNITTEST_ASSERT_EQ(apz_cmp(&neg_one, &neg_one), apz_ordering_equal);
        CUNITTEST_ASSERT_EQ(apz_cmp(&neg_two, &neg_two), apz_ordering_equal);
    }

    {
        int i;
        for (i = 0; i != 5 - 3; i++)
        {
            CUNITTEST_ASSERT_EQ(apz_cmp(&arr[i], &arr[i + 1]), apz_ordering_less);
            CUNITTEST_ASSERT_EQ(apz_cmp(&arr[i + 1], &arr[i + 2]), apz_ordering_less);
            CUNITTEST_ASSERT_EQ(apz_cmp(&arr[i + 1], &arr[i]), apz_ordering_greater);
            CUNITTEST_ASSERT_EQ(apz_cmp(&arr[i + 2], &arr[i + 1]), apz_ordering_greater);

            CUNITTEST_ASSERT_EQ(apz_cmp(&arr[i + 1], &arr[i + 1]), apz_ordering_equal);
        }
    }

    {
        apz_t x[4];
        int i;
        apz_init_str_base(&x[0], "FFFFFFFE", 16);
        apz_init_str_base(&x[1], "FFFFFFFF", 16);
        apz_init_str_base(&x[2], "100000000", 16);
        apz_init_str_base(&x[3], "100000001", 16);
        for (i = 0; i != 2; i++)
        {
            CUNITTEST_ASSERT_EQ(apz_cmp(&x[i], &x[i + 1]), apz_ordering_less);
            CUNITTEST_ASSERT_EQ(apz_cmp(&x[i + 1], &x[i + 2]), apz_ordering_less);
            CUNITTEST_ASSERT_EQ(apz_cmp(&x[i + 1], &x[i]), apz_ordering_greater);
            CUNITTEST_ASSERT_EQ(apz_cmp(&x[i + 2], &x[i + 1]), apz_ordering_greater);

            CUNITTEST_ASSERT_EQ(apz_cmp(&x[i + 1], &x[i + 1]), apz_ordering_equal);
        }
        apz_uninit(&x[0]);
        apz_uninit(&x[1]);
        apz_uninit(&x[2]);
        apz_uninit(&x[3]);
    }
    {
        int i;
        for (i = -3; i != 3; i++)
        {
            apz_t x;
            apz_init_int(&x, i);
            CUNITTEST_ASSERT_EQ(apz_cmp_int(&x, i - 1), \
                    apz_ordering_greater);
            CUNITTEST_ASSERT_EQ(apz_cmp_int(&x, i), \
                    apz_ordering_equal);
            CUNITTEST_ASSERT_EQ(apz_cmp_int(&x, i + 1), \
                    apz_ordering_less);
        }
    }

    CUNITTEST_RESULT();
}
