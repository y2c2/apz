/* Arbitrary-Precision Interger : Allocation
 * Copyright(c) 2019-2020 y2c2 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcast-align"
#endif

#include "apz_dt.h"
#include "apz_alloc.h"

/* Global stuff */
static apz_malloc_cb_t g_apz_malloc = NULL;
static apz_calloc_cb_t g_apz_calloc = NULL;
static apz_free_cb_t g_apz_free = NULL;
static apz_memcpy_cb_t g_apz_memcpy = NULL;
static apz_memset_cb_t g_apz_memset = NULL;

void apz_allocator_set_malloc(apz_malloc_cb_t cb);
void apz_allocator_set_calloc(apz_calloc_cb_t cb);
void apz_allocator_set_free(apz_free_cb_t cb);
void apz_allocator_set_memcpy(apz_memcpy_cb_t cb);
void apz_allocator_set_memset(apz_memset_cb_t cb);

void apz_allocator_set_malloc(apz_malloc_cb_t cb) { g_apz_malloc = cb; }
void apz_allocator_set_calloc(apz_calloc_cb_t cb) { g_apz_calloc = cb; }
void apz_allocator_set_free(apz_free_cb_t cb) { g_apz_free = cb; }
void apz_allocator_set_memcpy(apz_memcpy_cb_t cb) { g_apz_memcpy = cb; }
void apz_allocator_set_memset(apz_memset_cb_t cb) { g_apz_memset = cb; }

void *apz_malloc(apz_size_t size) { return g_apz_malloc(size); }
void *apz_calloc(apz_size_t nmemb, apz_size_t size) { return g_apz_calloc(nmemb, size); }
void apz_free(void *ptr) { g_apz_free(ptr); }
void *apz_memcpy(void *dest, const void *src, apz_size_t n) { return g_apz_memcpy(dest, src, n); }
void *apz_memset(void *s, int c, apz_size_t n) { return g_apz_memset(s, c, n); }

/* <dtor> <item_size> <item_count> <body> */

#define PTR_SIZE (sizeof(void *))
#define ITEM_SIZE_SIZE (sizeof(apz_size_t))
#define COUNT_SIZE (sizeof(apz_size_t))
typedef void (*__apz_dtor)(void *ptr);

void *__apz_new_raw( \
        apz_size_t size, \
        apz_size_t count, \
        void ctor(void *ptr), \
        void dtor(void *ptr))
{
    __apz_dtor *_dtor;
    apz_size_t *_item_size;
    apz_size_t *_item_count;
    apz_size_t idx;
    char *ptr_with_dtor = NULL;
    char *ptr, *ptr2;
    ptr_with_dtor = apz_malloc( \
            (PTR_SIZE + ITEM_SIZE_SIZE + COUNT_SIZE) + size * count);
    if (ptr_with_dtor == NULL) return NULL;
    _dtor = (__apz_dtor *)ptr_with_dtor;
    _item_size = (apz_size_t *)(ptr_with_dtor + PTR_SIZE);
    _item_count = (apz_size_t *)(ptr_with_dtor + PTR_SIZE + ITEM_SIZE_SIZE);
    ptr = (char *)(ptr_with_dtor + PTR_SIZE + ITEM_SIZE_SIZE + COUNT_SIZE);
    /* Set block information */
    *_item_size = size;
    *_item_count = count;
    *_dtor = dtor;
    /* Initialize */
    if (ctor != NULL)
    {
        ptr2 = ptr;
        for (idx = 0; idx != count; idx++)
        {
            ctor(ptr2);
            ptr2 += size;
        }
    }
    return ptr;
}

void apz_delete(void *ptr)
{
    __apz_dtor _dtor;
    apz_size_t *_item_size;
    apz_size_t *_item_count;
    apz_size_t idx;
    char *ptr_with_dtor;
    apz_size_t size;
    apz_size_t count;
    char *ptrb;
    if (ptr == NULL) return;
    ptr_with_dtor = ((char *)ptr) - (PTR_SIZE + ITEM_SIZE_SIZE + COUNT_SIZE);
    ptrb = ptr;
    _dtor = *((__apz_dtor *)ptr_with_dtor);
    if (_dtor != NULL)
    {
        _item_size = (apz_size_t *)(ptr_with_dtor + PTR_SIZE);
        _item_count = (apz_size_t *)(ptr_with_dtor + PTR_SIZE + ITEM_SIZE_SIZE);
        size = *_item_size;
        count = *_item_count;

        for (idx = 0; idx != count; idx++)
        {
            (*_dtor)(ptrb);
            ptrb += size;
        }
    }
    apz_free(ptr_with_dtor);
}

