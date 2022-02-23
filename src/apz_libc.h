/* Arbitrary-Precision Interger : LibC
 * Copyright(c) 2019-2020 y2c2 */

#ifndef APZ_LIBC_H
#define APZ_LIBC_H

#include "apz_dt.h"

apz_size_t apz_strlen(const char *s);
apz_size_t apz_u16strlen(const apz_u16 *s);
apz_size_t apz_u32strlen(const apz_u32 *s);

char *apz_strdup(const char *s);

int apz_strcmp(const char *s1, const char *s2);
int apz_strncmp(const char *s1, const char *s2, apz_size_t n);

#endif
