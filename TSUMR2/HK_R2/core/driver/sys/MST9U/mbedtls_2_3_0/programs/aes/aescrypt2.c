/*
 *  AES-256 file encryption program
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
#define mbedtls_fprintf    fprintf
#define mbedtls_printf     printf
#endif

#include "mbedtls/aes.h"
#include "mbedtls/md.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include "mbedtls/aescrypt2.h"

#if 0
    !defined(MBEDTLS_AES_C) || !defined(MBEDTLS_SHA256_C) || \
    !defined(MBEDTLS_FS_IO) || !defined(MBEDTLS_MD_C)
int main( void )
{
    mbedtls_printf("MBEDTLS_AES_C and/or MBEDTLS_SHA256_C "
                    "and/or MBEDTLS_FS_IO and/or MBEDTLS_MD_C "
                    "not defined.\n");
    return( 0 );
}
#else
int aes_dec( int mode, char *buff_out, unsigned long *size_out, char *buff_in, unsigned long filesize, unsigned char *key, size_t keylen )
{
    int ret = 1;

    int i, n;
    int lastn;

    unsigned char IV[16];
    unsigned char digest[32];
    unsigned char buffer[1024];

    mbedtls_aes_context aes_ctx;
    mbedtls_md_context_t sha_ctx;

    unsigned long offset;

    mbedtls_aes_init( &aes_ctx );
    mbedtls_md_init( &sha_ctx );

    ret = mbedtls_md_setup( &sha_ctx, mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 ), 1 );
    if( ret != 0 )
    {
        mbedtls_printf( "  ! mbedtls_md_setup() returned -0x%04x\n", -ret );
        goto exit;
    }

    *size_out = 0;
    mbedtls_memset(IV, 0, sizeof(IV));
    mbedtls_memset(digest, 0, sizeof(digest));
    mbedtls_memset(buffer, 0, sizeof(buffer));

    if( mode != MODE_DECRYPT )
    {
        mbedtls_printf( stderr, "invalide operation mode\n" );
        goto exit;
    }

    if( mode == MODE_DECRYPT )
    {
        unsigned char tmp[16];

        /*
         *  The encrypted file must be structured as follows:
         *
         *        00 .. 15              Initialization Vector
         *        16 .. 31              AES Encrypted Block #1
         *           ..
         *      N*16 .. (N+1)*16 - 1    AES Encrypted Block #N
         *  (N+1)*16 .. (N+1)*16 + 32   HMAC-SHA-256(ciphertext)
         */
        if( filesize < 48 )
        {
            mbedtls_printf( stderr, "File too short to be encrypted.\n" );
            goto exit;
        }

        if( ( filesize & 0x0F ) != 0 )
        {
            mbedtls_printf( stderr, "File size not a multiple of 16.\n" );
            goto exit;
        }

        /*
         * Subtract the IV + HMAC length.
         */
        filesize -= ( 16 + 32 );

        /*
         * Read the IV and original filesize modulo 16.
         */
        mbedtls_memcpy( buffer, buff_in, 16 );
        buff_in += 16;

        mbedtls_memcpy( IV, buffer, 16 );
        lastn = IV[15] & 0x0F;

        /*
         * Hash the IV and the secret key together 8192 times
         * using the result to setup the AES context and HMAC.
         */
        mbedtls_memset( digest, 0,  32 );
        mbedtls_memcpy( digest, IV, 16 );

        for( i = 0; i < 8192; i++ )
        {
            mbedtls_md_starts( &sha_ctx );
            mbedtls_md_update( &sha_ctx, digest, 32 );
            mbedtls_md_update( &sha_ctx, key, keylen );
            mbedtls_md_finish( &sha_ctx, digest );
        }

        mbedtls_memset( key, 0, sizeof( key ) );
        mbedtls_aes_setkey_dec( &aes_ctx, digest, 256 );
        mbedtls_md_hmac_starts( &sha_ctx, digest, 32 );

        /*
         * Decrypt and write the plaintext.
         */
        for( offset = 0; offset < filesize; offset += 16 )
        {
            mbedtls_memcpy( buffer, buff_in, 16 );
            buff_in += 16;

            mbedtls_memcpy( tmp, buffer, 16 );

            mbedtls_md_hmac_update( &sha_ctx, buffer, 16 );
            mbedtls_aes_crypt_ecb( &aes_ctx, MBEDTLS_AES_DECRYPT, buffer, buffer );

            for( i = 0; i < 16; i++ )
                buffer[i] = (unsigned char)( buffer[i] ^ IV[i] );

            mbedtls_memcpy( IV, tmp, 16 );

            n = ( lastn > 0 && offset == filesize - 16 )
                ? lastn : 16;

            mbedtls_memcpy( buff_out, buffer, n );
            buff_out += n;
            *size_out += n;
        }
    }

    ret = 0;

exit:
    mbedtls_memset( buffer, 0, sizeof( buffer ) );
    mbedtls_memset( digest, 0, sizeof( digest ) );

    mbedtls_aes_free( &aes_ctx );
    mbedtls_md_free( &sha_ctx );

    return( ret );
}
#endif /* MBEDTLS_AES_C && MBEDTLS_SHA256_C && MBEDTLS_FS_IO */
