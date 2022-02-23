/* Arbitrary-Precision Interger : Allocation
 * Copyright(c) 2019-2020 y2c2 */

#ifndef APZ_ALLOC_H
#define APZ_ALLOC_H

#include "apz_dt.h"

typedef void *(*apz_malloc_cb_t)(apz_size_t size);
typedef void *(*apz_calloc_cb_t)(apz_size_t nmemb, apz_size_t size);
typedef void (*apz_free_cb_t)(void *ptr);
typedef void *(*apz_memcpy_cb_t)(void *dest, const void *src, apz_size_t n);
typedef void *(*apz_memset_cb_t)(void *s, int c, apz_size_t n);

void apz_allocator_set_malloc(apz_malloc_cb_t cb);
void apz_allocator_set_calloc(apz_calloc_cb_t cb);
void apz_allocator_set_free(apz_free_cb_t cb);
void apz_allocator_set_memcpy(apz_memcpy_cb_t cb);
void apz_allocator_set_memset(apz_memset_cb_t cb);

void *apz_malloc(apz_size_t size);
void *apz_calloc(apz_size_t nmemb, apz_size_t size);
void apz_free(void *ptr);
void *apz_memcpy(void *dest, const void *src, apz_size_t n);
void *apz_memset(void *s, int c, apz_size_t n);

typedef void (*apz_ctor_t)(void *ptr);
typedef void (*apz_dtor_t)(void *ptr);

void *__apz_new_raw( \
        apz_size_t size, \
        apz_size_t count, \
        apz_ctor_t ctor, \
        apz_dtor_t dtor);

/* new : 1 with nothing */
#define apz_new(_type) \
    ((_type *)__apz_new_raw(sizeof(_type), 1, NULL, NULL))

/* new : 1 with ctor */
#define apz_newc(_type, _ctor) \
    ((_type *)__apz_new_raw(sizeof(_type), 1, _ctor, NULL))

/* new : 1 with dtor */
#define apz_newd(_type, _dtor) \
    ((_type *)__apz_new_raw(sizeof(_type), 1, NULL, _dtor))

/* new : 1 with ctor and dtor */
#define apz_newcd(_type, _ctor, _dtor) \
    ((_type *)__apz_new_raw(sizeof(_type), 1, (apz_ctor_t)_ctor, (apz_dtor_t)_dtor))

/* new : n without ctor and dtor */
#define apz_newn(_type, _count) \
    ((_type *)__apz_new_raw(sizeof(_type), (_count), NULL, NULL))

/* new : n with ctor */
#define apz_newnc(_type, _count, _ctor) \
    ((_type *)__apz_new_raw(sizeof(_type), (_count), _ctor, NULL))

/* new : n with dtor */
#define apz_newnd(_type, _count, _ctor) \
    ((_type *)__apz_new_raw(sizeof(_type), (_count), NULL, _dtor))

/* new : n with ctor and dtor */
#define apz_newncd(_type, _count, _ctor) \
    ((_type *)__apz_new_raw(sizeof(_type), (_count), _ctor, _dtor))

/* delete */
void apz_delete(void *ptr);

#endif
