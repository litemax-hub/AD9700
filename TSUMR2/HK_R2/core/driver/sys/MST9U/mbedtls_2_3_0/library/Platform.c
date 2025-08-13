/*
 *  Portable interface to the CPU cycle counter
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#endif

#include <string.h>

void * _m_memset(void *s, int c, unsigned int count);
void * _m_memset(void *s, int c, unsigned int count)
{
    char *xs=(char *)s;
    while(count--)
        *xs++=c;
    return s;
}

void *_m_memcpy(void *dst, const void *src, size_t n);
void *_m_memcpy(void *dst, const void *src, size_t n)
{
    char *pdst = (char *)dst;
    const char *psrc = (const char *)src;
    for(; n > 0; ++pdst, ++psrc, --n)
    {
        *pdst = *psrc;
    }

    return (dst);
}

int _m_memcmp(const void *s1, const void *s2, size_t n);
int _m_memcmp(const void *s1, const void *s2, size_t n)
{
    unsigned char c1, c2;

    for ( ; n-- ; s1++, s2++)
    {
        c1 = *(unsigned char *)s1;
        c2 = *(unsigned char *)s2;
        if ( c1 != c2)
            return (c1-c2);
    }
    return 0;
}

size_t _m_strlen(const char *pStr);
size_t _m_strlen(const char *pStr)
{
    size_t i=0;

    while(pStr[i])
    {
        i++;
    }
    return i;
}

static unsigned char *pDecompressionBuf = NULL;
static unsigned char *pEndofDecompressionBuf = NULL;
static unsigned char *pCurrentBufPtr = NULL;

void MemoryAllocatorInit(unsigned char *ptr, unsigned long nSize);
void MemoryAllocatorInit(unsigned char *ptr, unsigned long nSize)
{
   pDecompressionBuf = ptr;
   pEndofDecompressionBuf = pDecompressionBuf + nSize*sizeof(unsigned char);
   pCurrentBufPtr = pDecompressionBuf;
}

void *m_zalloc(void *ptr, unsigned long nNum, unsigned long nSize);
void *m_zalloc(void *ptr, unsigned long nNum, unsigned long nSize)
{
// Because C language use call by value
// so if we modify ptr value, it will not afftect the caller value
// Considering decompression performance, we don't do memory pool
// region check here
#if 0
    nSize = ( nSize*nNum + 3 ) & ~3;

    ptr = (void *)pCurrentBufPtr;
    pCurrentBufPtr += nSize;

    if (pCurrentBufPtr > pEndofDecompressionBuf)
    {
        printf("Memory Allocate Fail\n");
        ptr = NULL;
    }
#else
    ptr = (void *)pCurrentBufPtr;
    // 4' alignment
    pCurrentBufPtr += ( nSize*nNum + 3 ) & ~3;
#endif
    return ptr;
}


void *m_zcalloc(unsigned long num, unsigned long size);
void *m_zcalloc(unsigned long num, unsigned long size)
{
    void *p = NULL;
    unsigned long total = num * size;

    p = m_zalloc(p, 1, total);
    _m_memset(p, 0, total);

    return p;
}

void m_zfree(void *x, void *address, unsigned nbytes);
void m_zfree(void *x, void *address, unsigned nbytes)
{
    // Considering decompression performance, we don't
    // do memory free operation here
    // This will cause a lot of memory usage, but I think we can
    // afford this, because our target files are not so big
    x = x;
    address = address;
    nbytes = nbytes;
}



