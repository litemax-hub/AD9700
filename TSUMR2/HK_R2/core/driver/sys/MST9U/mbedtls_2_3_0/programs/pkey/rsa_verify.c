/*
 *  RSA/SHA-256 signature verification program
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

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#define mbedtls_snprintf   snprintf
#endif

#if 0
    !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_RSA_C) ||  \
    !defined(MBEDTLS_SHA256_C) || !defined(MBEDTLS_MD_C) || \
    !defined(MBEDTLS_FS_IO)
int main( void )
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_RSA_C and/or "
            "MBEDTLS_MD_C and/or "
            "MBEDTLS_SHA256_C and/or MBEDTLS_FS_IO not defined.\n");
    return( 0 );
}
#else

#include "mbedtls/rsa.h"
#include "mbedtls/md.h"
#include "mbedtls/rsa_verify.h"

#include <stdio.h>
#include <string.h>

unsigned char *mbedtls_rsa_verify_content_ptr;
unsigned long mbedtls_rsa_verify_content_size;
unsigned long mbedtls_rsa_verify_content_size_1st;
unsigned char *mbedtls_rsa_verify_sig_ptr;
unsigned char *mbedtls_rsa_verify_key_ptr;

#define R2_DQMEM_RSA_VERIFY_BASE 0xB0006000
#define R2_DQMEM_RSA_VERIFY_SIZE 0x6000

int mbedtls_rsa_verify(void)
{
    int ret = 0;
    mbedtls_rsa_context rsa;
    unsigned char hash[32];
    char rsa_N[0x200+1];
    char rsa_E[0x06+1];

    MemoryAllocatorInit((unsigned char *)R2_DQMEM_RSA_VERIFY_BASE, R2_DQMEM_RSA_VERIFY_SIZE);
	
    // IO mapping
    mbedtls_rsa_verify_content_ptr = (void*)((unsigned long)mbedtls_rsa_verify_content_ptr);
    mbedtls_rsa_verify_sig_ptr = (void*)((unsigned long)mbedtls_rsa_verify_sig_ptr);
    mbedtls_rsa_verify_key_ptr = (void*)((unsigned long)mbedtls_rsa_verify_key_ptr);

    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

    // read rsa key
    mbedtls_memcpy( rsa_N, (mbedtls_rsa_verify_key_ptr + 0x04), 0x200 );
    rsa_N[0x200] = '\0';
    mbedtls_memcpy( rsa_E, (mbedtls_rsa_verify_key_ptr + 0x20A), 0x06 );
    rsa_E[0x06] = '\0';
    ret = mbedtls_mpi_read_string( &rsa.N, 16, rsa_N );
    mbedtls_mpi_read_string( &rsa.E, 16, rsa_E );
    rsa.len = ( mbedtls_mpi_bitlen( &rsa.N ) + 7 ) >> 3;

    /*
     * Compute the SHA-256 hash of the input file and
     * verify the signature
     */
    mbedtls_md( mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 ),
                    mbedtls_rsa_verify_content_ptr, mbedtls_rsa_verify_content_size, hash );

    if( (ret = mbedtls_rsa_pkcs1_verify( &rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                                  MBEDTLS_MD_SHA256, 20, hash, mbedtls_rsa_verify_sig_ptr )) != 0 )
    {
        mbedtls_printf( "FAIL (the signature is invalid) - 0x%0x\r\n", -ret );
    }
    else
    {
        mbedtls_printf( "OK (the signature is valid)\r\n" );
    }

    mbedtls_rsa_free( &rsa );

    return( ret );
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_RSA_C && MBEDTLS_SHA256_C &&
          MBEDTLS_FS_IO */
