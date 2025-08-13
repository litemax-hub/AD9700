////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MSAPI_OCP
#define MSAPI_OCP

#include "MsCommon.h"
#include "apiGFX.h"


#ifdef MSAPI_OCP_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#define INFO_SIZE 64


#define ENCODING_UTF16_BIG_ENDIAN      0
#define ENCODING_UTF8                  1
#define ENCODING_FLAG_FOR_ONE_OR_TWO   2
#define ENCODING_UTF16_LITTLE_ENDIAN   3

INTERFACE BOOLEAN _bOCPFromMem;  //20091124EL

INTERFACE void msAPI_OCP_LoadBitmap(BMPHANDLE* pbmpHandle);
INTERFACE void msAPI_OCP_PrepareBitmapBinary(void);
INTERFACE void msAPI_OCP_Init(void);
INTERFACE BMPHANDLE msAPI_OCP_GenerateBitmapHandle_SDRAM(U16 u16width, U16 u16height, GFX_Buffer_Format fmt);
INTERFACE BMPHANDLE msAPI_OCP_GenerateBitmapHandle_SDRAMEx(U16 u16width, U16 u16height, GFX_Buffer_Format fmt);
INTERFACE U32 msAPI_OCP_GetBmpAddr(void);
INTERFACE BOOLEAN msAPI_OCP_IsBitmapBinaryPrepared(void);
INTERFACE void msAPI_OCP_PrepareStringBinary(void);//Prepare the string binary in memory
INTERFACE void msAPI_OCP_ReadBinaryString(U8 language, U16 id, U16 *pu16stringbuffer);
INTERFACE U32 msAPI_OCP_GetStringAddr(void);
INTERFACE void msAPI_OCP_LoadAllStringToMem(void);

#if 0
INTERFACE FONTHANDLE msAPI_OCP_LoadSDRAMFont(U32 fontIndex, EN_FONT_TARGET_BPP u8TargetBPP);
INTERFACE U32 msAPI_OCP_PrepareFontPackBinary(void);//return the font count
INTERFACE void msAPI_OCP_ReadFontBoundary(U32 *pFontDataBoundary);
#endif
INTERFACE U16 msAPI_OCP_MappinUTF8ToUCS2(U8 *pu8Str, U16 *pu16Str, U16 srcByteLen, U16 dstWideCharLen);
#ifdef OBA_COPROCESSOR_LZSS_DECOMPRESSED
void msAPI_OCP_WaitForBitmapPrepared(void);
#endif

#ifdef DVBT_MMBOX
INTERFACE void msAPI_OCP_Load2ndBmpGroup(void);
INTERFACE void msAPI_OCP_ReLoad2ndBmpGroup(void);
#endif
INTERFACE void msAPI_OCP_DecompressHandle(BMPHANDLE handle);

#undef INTERFACE
#endif

