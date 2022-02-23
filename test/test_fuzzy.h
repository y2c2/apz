#ifndef TEST_FUZZY_H
#define TEST_FUZZY_H


typedef enum
{
    fuzzy_binop_add,
    fuzzy_binop_sub,
    fuzzy_binop_mul,
    fuzzy_binop_div,
    fuzzy_binop_mod,
    fuzzy_binop_lshift,
    fuzzy_binop_rshift,
} fuzzy_binop_t;

void test_fuzzy_binop(const fuzzy_binop_t op, const char *lhs, const char *rhs, const char *exptected);

#endif

