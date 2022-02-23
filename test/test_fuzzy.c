#include "testfw.h"
#include "apz.h"
#include "test_fuzzy.h"

static const char *op_names[] = 
{
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "lshift",
    "rshift",
};

void test_fuzzy_binop(const fuzzy_binop_t op, const char *lhs, const char *rhs, const char *expected)
{
    apz_t z_lhs;
    apz_t z_rhs;
    apz_t z_expected;
    char buf_lhs[1024], buf_rhs[1024];

    apz_init_zero(&z_lhs);
    apz_init_zero(&z_rhs);
    apz_init_zero(&z_expected);

    if (apz_init_str_c_style(&z_lhs, lhs) != 0)
    {
        goto fail;
    }
    if (apz_init_str_c_style(&z_rhs, rhs) != 0)
    {
        goto fail;
    }
    if (apz_init_str_c_style(&z_expected, expected) != 0)
    {
        goto fail;
    }

    apz_formatn_decimal(buf_lhs, 1024, &z_lhs);
    apz_formatn_decimal(buf_rhs, 1024, &z_rhs);

    switch (op)
    {
        case fuzzy_binop_add:
            if (apz_add(&z_lhs, &z_rhs) != 0)
            { goto fail; }
            break;
        case fuzzy_binop_sub:
            if (apz_sub(&z_lhs, &z_rhs) != 0)
            { goto fail; }
            break;
        case fuzzy_binop_mul:
            if (apz_mul(&z_lhs, &z_rhs) != 0)
            { goto fail; }
            break;
        case fuzzy_binop_div:
            if (apz_div(&z_lhs, &z_rhs) != 0)
            { goto fail; }
            break;
        case fuzzy_binop_mod:
            if (apz_mod(&z_lhs, &z_rhs) != 0)
            { goto fail; }
            break;
        case fuzzy_binop_lshift:
            if (apz_lshift(&z_lhs, &z_rhs) != 0)
            { goto fail; }
            break;
        case fuzzy_binop_rshift:
            if (apz_rshift(&z_lhs, &z_rhs) != 0)
            { goto fail; }
            break;
    }

    if (apz_cmp(&z_lhs, &z_expected) != apz_ordering_equal)
    { goto fail_result; }

    goto done;
fail:
    printf("failed on op=%s, lhs=%s, rhs=%s, expected=%s\n", op_names[(int)(op)], lhs, rhs, expected);
    goto done;
fail_result:
    {
        char buf[1024], buf2[1024];
        apz_formatn_decimal(buf, 1024, &z_lhs);
        apz_formatn_hexadecimal(buf2, 1024, &z_lhs);
        printf("failed on op=%s, lhs=%s, rhs=%s, expected=%s, " \
                "result=%s (0x%s)\n", \
                op_names[(int)(op)], buf_lhs, buf_rhs, expected, \
                buf, buf2);
    }
done:
    apz_uninit(&z_lhs);
    apz_uninit(&z_rhs);
    apz_uninit(&z_expected);
}
