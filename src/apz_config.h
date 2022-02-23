/* Arbitrary-Precision Interger : Config
 * Copyright(c) 2019-2020 y2c2 */

#ifndef APZ_CONFIG_H
#define APZ_CONFIG_H

/* Uncomment one of the following line to preset the chunk size to
 * 32-bit or 64-bit */

/*
#define APZ_CHUNK_32
*/
/*
#define APZ_CHUNK_64
*/

#ifndef APZ_CHUNK_32
#ifndef APZ_CHUNK_64
#  if (defined(_MSV_VER) && (defined(_M_X64) || defined(_M_AMD64))) || \
    (defined(__GNUC__) && (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)))
#    define APZ_CHUNK_64
#  elif (defined(_MSV_VER) && (defined(_M_IX86)) || \
        (defined(__GNUC__) && (defined(i386)) || (defined(__i386)) || (defined(__i386__))))
#    define APZ_CHUNK_32
#  elif (defined(__GNUC__) && defined(__aarch64__))
#    define APZ_CHUNK_64
#  elif (defined(__GNUC__) && defined(__arm__))
#    define APZ_CHUNK_32
#  elif (defined(__GNUC__) && (defined(__mips__) || defined(mips)))
#    define APZ_CHUNK_32
#  else
#    define APZ_CHUNK_32
#  endif
#endif
#endif

#endif
