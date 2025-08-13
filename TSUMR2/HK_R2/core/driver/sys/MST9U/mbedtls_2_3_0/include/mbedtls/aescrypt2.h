

#define MODE_ENCRYPT    0
#define MODE_DECRYPT    1

extern int aes_dec( int mode, char *buff_out, unsigned long *size_out, char *buff_in, unsigned long filesize, unsigned char *key, size_t keylen );
extern void mbedtls_sha256( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 );

