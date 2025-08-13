////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MS_DECOMPRESS_H__
#define __MS_DECOMPRESS_H__

#ifdef OBA_COPROCESSOR_LZSS_DECOMPRESSED
//-----------------------------------------------
// Class :: LZSS
//-----------------------------------------------
typedef enum
{
    //HK to CO_PRO
	MB_LZSS_ADDR_PART1             = 0,
	MB_LZSS_ADDR_PART2             = 1,

    //CO_PRO to HK
    MB_LZSS_DCOMPRESS_DONE           = 0x80,

} MB_LZSS_CLASS;
#endif

/// Mail box return state
typedef enum
{
    MB_RETURN_LZSS_OK,
    MB_RETURN_LZSS_COMPRESS_DONE,
    MB_RETURN_LZSS_COMPRESS_FAILED,
} MB_RETURN_LZSS_STATE;


BOOLEAN ms_DecompressInit(U8 *pOutStream);
int ms_Decompress(const U8 *pInStream, U32 u32DataLen);
void ms_DecompressDeInit(void);

BOOLEAN DoMsDecompression(U8 *pSrc, U8 *pDst, U8 *pTmp, U32 srclen);
BOOLEAN DoMsDecompression7(U8 *pSrc, U8 *pDst, U8 *pTmp, U32 srclen);

#endif /* __MS_DECOMPRESS_H__ */
