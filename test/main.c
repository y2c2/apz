#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apz.h"
#include "test_basic.h"
#include "test_init.h"
#include "test_toprim.h"
#include "test_ordering.h"
#include "test_shift.h"
#include "test_add.h"
#include "test_sub.h"
#include "test_mul.h"
#include "test_div.h"
#include "test_mod.h"
#include "test_divmod.h"
#include "test_pow.h"
#include "test_format.h"
#include "test_fuzzy.h"

static void show_info(void)
{
    static const char *info = ""
        "test                        -- Predefined Test"
        "test <action> [parameters]  -- Fuzzy Test\n"
        "  action: add, sub, mul, div, mod\n"
        "";
    puts(info);
}

int main(int argc, char *argv[])
{
    apz_allocator_set_malloc(malloc);
    apz_allocator_set_calloc(calloc);
    apz_allocator_set_free(free);
    apz_allocator_set_memcpy(memcpy);
    apz_allocator_set_memset(memset);

    if (argc == 1)
    {
        test_basic();
        test_init();
        test_toprim();
        test_ordering();
        test_shift();
        test_add();
        test_sub();
        test_mul();
        test_div();
        test_mod();
        test_divmod();
        test_pow();
        test_format();
    }
    else if (argc == 5)
    {
        if (strcmp(argv[1], "add") == 0)
        {
            test_fuzzy_binop(fuzzy_binop_add, argv[2], argv[3], argv[4]);
        }
        else if (strcmp(argv[1], "sub") == 0)
        {
            test_fuzzy_binop(fuzzy_binop_sub, argv[2], argv[3], argv[4]);
        }
        else if (strcmp(argv[1], "mul") == 0)
        {
            test_fuzzy_binop(fuzzy_binop_mul, argv[2], argv[3], argv[4]);
        }
        else if (strcmp(argv[1], "div") == 0)
        {
            test_fuzzy_binop(fuzzy_binop_div, argv[2], argv[3], argv[4]);
        }
        else if (strcmp(argv[1], "mod") == 0)
        {
            test_fuzzy_binop(fuzzy_binop_mod, argv[2], argv[3], argv[4]);
        }
        else
        {
            show_info();
        }
    }
    else
    {
        show_info();
    }

    return 0;
}
