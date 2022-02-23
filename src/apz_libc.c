/* Arbitrary-Precision Interger : LibC
 * Copyright(c) 2019-2020 y2c2 */

#include "apz_dt.h"
#include "apz_alloc.h"
#include "apz_libc.h"

apz_size_t apz_strlen(const char *s)
{
    apz_size_t len = 0;
    const char *p = s;
    while (*p++ != '\0') len++;
    return len;
}

apz_size_t apz_u16strlen(const apz_u16 *s)
{
    apz_size_t len = 0;
    const apz_u16 *p = s;
    while (*p++ != '\0') len++;
    return len;
}

apz_size_t apz_u32strlen(const apz_u32 *s)
{
    apz_size_t len = 0;
    const apz_u32 *p = s;
    while (*p++ != '\0') len++;
    return len;
}

char *apz_strdup(const char *s)
{
    apz_size_t len = apz_strlen(s);
    char *p;
    if ((p = apz_malloc(sizeof(char) * (len + 1))) == NULL)
    { return NULL; }
    apz_memcpy(p, s, len + 1);
    return p;
}

int apz_strcmp(const char *s1, const char *s2)
{
    const char *p1 = s1, *p2 = s2;
    if ((p1 == NULL) || (p2 == NULL)) return -1;
    for (;;)
    {
        if ((*p1 != *p2) || (*p1 == '\0')) break;
        p1++; p2++;
    }
    return *p1 - *p2;
}

int apz_strncmp(const char *s1, const char *s2, apz_size_t n)
{
    const char *p1 = s1, *p2 = s2;
    if (n != 0)
    {
        do
        {
            if (*p1 != *p2++) break;
            if (*p1++ == '\0') return 0;
        }
        while (--n != 0);
        if (n > 0)
        {
            if (*p1 == '\0') return -1;
            if (*--p2 == '\0') return 1;
            return (unsigned char)*p1 - (unsigned char)*p2;
        }
    }
    return 0;
}

