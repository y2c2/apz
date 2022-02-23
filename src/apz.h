/* Arbitrary-Precision Interger
 * Copyright(c) 2019-2020 y2c2 */

#ifndef APZ_H
#define APZ_H

#include "apz_dt.h"
#include "apz_config.h"
#include "apz_private.h"
#include "apz_alloc.h"

/* Memory Management */
void apz_allocator_set_malloc(apz_malloc_cb_t cb);
void apz_allocator_set_calloc(apz_calloc_cb_t cb);
void apz_allocator_set_free(apz_free_cb_t cb);
void apz_allocator_set_memcpy(apz_memcpy_cb_t cb);
void apz_allocator_set_memset(apz_memset_cb_t cb);

/* Initialize */
void apz_init_zero(apz_t *x);
void apz_init_one(apz_t *x);
void apz_init_int(apz_t *x, const int n);
void apz_init_uint(apz_t *x, const unsigned int n);
void apz_init_s32(apz_t *x, const apz_s32 n);
void apz_init_u32(apz_t *x, const apz_u32 n);
int apz_init_s64(apz_t *x, const apz_s64 n);
int apz_init_u64(apz_t *x, const apz_u64 n);

int apz_init_strn(apz_t *x, const char *s, apz_size_t len);
int apz_init_str(apz_t *x, const char *s);
int apz_init_strn_base(apz_t *x, const char *s, apz_size_t len, const int base);
int apz_init_str_base(apz_t *x, const char *s, const int base);
int apz_init_strn_c_style(apz_t *x, const char *s, apz_size_t len);
int apz_init_str_c_style(apz_t *x, const char *s);

int apz_init_u16strn(apz_t *x, const apz_u16 *s, apz_size_t len);
int apz_init_u16str(apz_t *x, const apz_u16 *s);
int apz_init_u16strn_base(apz_t *x, const apz_u16 *s, apz_size_t len, const int base);
int apz_init_u16str_base(apz_t *x, const apz_u16 *s, const int base);
int apz_init_u16strn_c_style(apz_t *x, const apz_u16 *s, apz_size_t len);
int apz_init_u16str_c_style(apz_t *x, const apz_u16 *s);

int apz_init_u32strn(apz_t *x, const apz_u32 *s, apz_size_t len);
int apz_init_u32str(apz_t *x, const apz_u32 *s);
int apz_init_u32strn_base(apz_t *x, const apz_u32 *s, apz_size_t len, const int base);
int apz_init_u32str_base(apz_t *x, const apz_u32 *s, const int base);
int apz_init_u32strn_c_style(apz_t *x, const apz_u32 *s, apz_size_t len);
int apz_init_u32str_c_style(apz_t *x, const apz_u32 *s);

/* Uninitialize */
void apz_uninit(apz_t *x);

/* Test */
apz_bool apz_is_zero(const apz_t *x);
apz_bool apz_is_one(const apz_t *x);
apz_bool apz_is_negative_one(const apz_t *x);
apz_bool apz_is_positive(const apz_t *x);
apz_bool apz_is_negative(const apz_t *x);
apz_bool apz_is_non_positive(const apz_t *x);
apz_bool apz_is_non_negative(const apz_t *x);
apz_bool apz_is_valid_int(const apz_t *x);
apz_bool apz_is_valid_uint(const apz_t *x);
apz_bool apz_is_valid_s32(const apz_t *x);
apz_bool apz_is_valid_u32(const apz_t *x);
apz_bool apz_is_valid_s64(const apz_t *x);
apz_bool apz_is_valid_u64(const apz_t *x);
apz_bool apz_is_valid_float(const apz_t *x);
apz_bool apz_is_valid_double(const apz_t *x);

/* To Primitive */
int apz_to_int(const apz_t *x);
unsigned int apz_to_uint(const apz_t *x);
apz_s32 apz_to_s32(const apz_t *x);
apz_u32 apz_to_u32(const apz_t *x);
apz_s64 apz_to_s64(const apz_t *x);
apz_u64 apz_to_u64(const apz_t *x);
float apz_to_float(const apz_t *x);
double apz_to_double(const apz_t *x);
void apz_to_positive(apz_t *x);
void apz_to_negative(apz_t *x);

/* Clone */
int apz_clone(apz_t *dst, const apz_t *src);

/* Move */
void apz_move(apz_t *dst, apz_t *src);

/* Swap */
void apz_swap(apz_t *lhs, apz_t *rhs);

/* Trim */
int apz_trim(apz_t *x);

/* Negative */
void apz_negative(apz_t *x);

/* Bitwise */
int apz_lshift(apz_t *x,const apz_t *n);
int apz_lshift_size_t(apz_t *x, const apz_size_t n);
int apz_lshift_int(apz_t *x, const int n);
int apz_rshift(apz_t *x,const apz_t *n);
int apz_rshift_size_t(apz_t *x, const apz_size_t n);
int apz_rshift_int(apz_t *x, const int n);
int apz_bitwise_inv(apz_t *x);
int apz_bitwise_and(apz_t *x, const apz_t *y);
int apz_bitwise_or(apz_t *x, const apz_t *y);
int apz_bitwise_xor(apz_t *x, const apz_t *y);
int apz_bitwise_assign(apz_t *x, const apz_size_t idx, \
        const unsigned int v);
unsigned int apz_bitwise_extract(apz_t *x, const apz_size_t idx);
int apz_bitwise_set(apz_t *x, const apz_size_t idx);
int apz_bitwise_clr(apz_t *x, const apz_size_t idx);

/* Arithemetic */
int apz_add(apz_t *dst, const apz_t *src);
int apz_add_int(apz_t *dst, const int src);
int apz_sub(apz_t *dst, const apz_t *src);
int apz_sub_int(apz_t *dst, const int src);
int apz_mul(apz_t *dst, const apz_t *src);
int apz_mul_int(apz_t *dst, const int src);
int apz_div(apz_t *dst, const apz_t *src);
int apz_div_int(apz_t *dst, const int src);
int apz_mod(apz_t *dst, const apz_t *src);
int apz_mod_int(apz_t *dst, const int src);
int apz_div_mod(apz_t *dst, apz_t *m, const apz_t *src);
int apz_div_mod_int(apz_t *dst, apz_t *m, const int src);
int apz_pow(apz_t *dst, const apz_t *src);
int apz_pow_int(apz_t *dst, const int src);

/* Inc & Dec */
int apz_inc(apz_t *x);
int apz_dec(apz_t *x);

/* Misc */
void apz_abs(apz_t *x);

/* Order */
typedef enum
{
    apz_ordering_less = -1,
    apz_ordering_equal = 0,
    apz_ordering_greater = 1,
} apz_ordering_t;
apz_ordering_t apz_ordering_reverse(const apz_ordering_t ordering);
apz_ordering_t apz_cmp_abs(const apz_t *x1, const apz_t *x2);
apz_ordering_t apz_cmp(const apz_t *x1, const apz_t *x2);
apz_ordering_t apz_cmp_int(const apz_t *x1, const int x2);
apz_ordering_t apz_cmp_s64(const apz_t *x1, const apz_s64 x2);
apz_ordering_t apz_cmp_u64(const apz_t *x1, const apz_u64 x2);

/* Format */
int apz_formatn_base(char *str, apz_size_t size, const apz_t *x, const int base);
int apz_formatn_binary(char *str, apz_size_t size, const apz_t *x);
int apz_formatn_octal(char *str, apz_size_t size, const apz_t *x);
int apz_formatn_decimal(char *str, apz_size_t size, const apz_t *x);
int apz_formatn_hexadecimal(char *str, apz_size_t size, const apz_t *x);

/* Inspector */
apz_size_t apz_inspect_nchunks_used(apz_t *x);
apz_size_t apz_inspect_nchunks_capacity(apz_t *x);
apz_bool apz_inspect_negative(apz_t *x);
apz_chunk_t apz_inspect_chunks(apz_t *x, const apz_size_t idx);
apz_size_t apz_inspect_chunk_size(void);

#endif

