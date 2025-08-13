////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information!�L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _MSAPI_FONT_H
#define    _MSAPI_FONT_H

#include "datatype.h"
#include "board.h"
#if (VECTOR_FONT_ENABLE)
#include "drvCPU.h"
#include "ms_decompress.h"
#endif

#ifdef MSAPI_FONT_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

// Tags to identify a valid font header
#define FONT_HEADER_TAG    0x466f4E74

// Font compression method
#define FONT_COMPRESSION_NONE               0x00
#define FONT_COMPRESSION_BOUND_RECTANGLE    0x01
#define FONT_COMPRESSION_RUN_LENGTH         0x02
//#define FONT_COMPRESSION_HUFFMAN            0x03
#define FONT_MAX_RANGE 20

//Move from msAPI_Font.h
/////////////////////////////////////
#define MAX_FONTTABLE_NUM_OSD  18//5 //LANGUAGE_NUM

#define FONT_0        0 //35pt
#define FONT_1        1 //30pt
#define FONT_2        2 //26pt
#define FONT_3		  3

#define FONT_BMP_0   3
#define FONT_BMP_1   4
#define FONT_BMP_2   5

#define FONT_EXT_BEGIN  6
#define FONT_MULTILANG_BEGIN    9

#if (ENABLE_ARABIC_OSD)
    #define FONT_ARABIC_0   10
    #define FONT_ARABIC_1   11
    #define FONT_ARABIC_2   12
    #define FONT_ARABIC_50  13
#endif

#if (ENABLE_THAI_OSD)
    #define FONT_THAI_0     14
    #define FONT_THAI_1     15
    #define FONT_THAI_2     16
    #define FONT_THAI_50    17
#endif
////////////////////////////////////////////

#if (VECTOR_FONT_ENABLE)
 #if ((CHIP_FAMILY_TYPE == CHIP_FAMILY_S7) && defined(MIPS_CHAKRA) && defined(ARM_CHAKRA))
  #if (DISPLAY_LOGO == DISABLE && ENABLE_POWERON_MUSIC == DISABLE)
   #ifdef ATSC_SYSTEM
    #define COPRO_MVF_ENABLE 0
   #else
    #define COPRO_MVF_ENABLE 1
   #endif
  #else // #if (DISPLAY_LOGO == DISABLE && ENABLE_POWERON_MUSIC == DISABLE)
    #define COPRO_MVF_ENABLE 0
  #endif
 #else // #if ((CHIP_FAMILY_TYPE == CHIP_FAMILY_S7) && defined(MIPS_CHAKRA))
    #define COPRO_MVF_ENABLE 0
 #endif
#endif

/// Font target bits per pixel
typedef enum // for ATSC_TRUNK
{
    FONT_TARGET_BPP_NO_CHANGE,  ///< Target BPP is the same as source
    FONT_TARGET_BPP_I2_VARIANT  ///< Target BPP is converted from I2 to 3 I1. Used for blinking text that appears in closed caption.
} EN_FONT_TARGET_BPP;

typedef enum
{
    MB_MVF_RETURN_NONE,
    MB_MVF_RETURN_DONE,
    MB_MVF_RETURN_FAILED,
} MB_MVF_RETURN_STATE;

typedef enum
{
    //HK => COPRO
	MB_MVF_CMD_LOAD_FACE           = 0,
	MB_MVF_CMD_GENERATE_BITMAP     = 1,
	MB_MVF_CMD_DONE_FACE           = 2,

    //COPRO => HK
    MB_MVF_CMD_LOAD_DONE           = 0x80,
    MB_MVF_CMD_GENERATE_DONE       = 0x81,

} MB_MVF_CMD_INDEX;

/// Structure for a font table
typedef struct
{
    FONTHANDLE fHandle;         ///< Font handle
} FONT;

/// Font bin header
typedef struct {
    U32     u32Tag;                 ///< Default is 0x466f4E74
    U32     u32BPP;                 ///< Bits Per Pixel. I1: 0x01, I2: 0x02, I2 variant: 0x03 (for Saturn)
    U32     u32CharNum;             ///< Character count
    U32     u32Width;               ///< Width in pixels of a font
    U32     u32Height;              ///< Height in pixels of a font
    U32     u32Pitch;               ///< Font pitch in bytes
    U32     u32Compression;         ///< 0x00 for none compression, 0x01 for bounding rectangle
    // BBox (Bounding Box) describes the smallest rectangle that encloses the shape/outline of each glyph
    U32     u32BBoxOffset;          ///< Offset of BBox information
    U32     u32CodemapOffset;       ///< Offset of codemap block data. Each codemap block is a tuple of(start code, end code, start index)
    U32     u32CodemapBlockNum;     ///< Number of codemap block.
    U32     u32CharGlyphBytes;      ///< Size of one char glyph including possible paddings for alignment
    U32     u32FontGlyphOffset;     ///< Offset of glyph data
    U32     u32FontGlyphBytes;      ///< Length of all the glyph data, depending on u8Compression.
} FONT_BIN_HEADER;


typedef struct
{
    U32     u32SdramLen;        ///< Total SDRAM length in bytes
    U32     u32StartAddr;       ///< SDRAM start address of the font

    U32     u32BBoxAddr;        ///< SDRAM address of bbox
    U32     u32CodemapAddr;     ///< SDRAM address of codemap
    U32     u32GlyphAddr;       ///< SDRAM address of glyph
} ST_LOAD_FONT_SDRAM_REPORT;

typedef struct
{
    U32     u32Tag;             ///< Default is 0x466f4E74
    U32     u32SdramLen;        ///< Total SDRAM length in bytes
    U32     u32StartAddr;       ///< SDRAM start address of the font
    U32     u32GlyphAddr;       ///< SDRAM address of glyph
    U32     u32BBoxAddr;        ///< SDRAM address of bbox
    U32     u32CodemapAddr;     ///< SDRAM address of codemap
    U32     u32CharNum;         ///< Character count
    U32     u32CodemapBlockNum; ///< Number of codemap block.
    U32     u32Width;           ///< Width in pixels of a font
    U32     u32Height;          ///< Height in pixels of a font
    U32     u32BPP;             ///< Bits Per Pixel. I1: 0x01, I2: 0x02, I2 variant: 0x03 (for Saturn)
    U32     u32CharGlyphBytes;  ///< Size of one char glyph including possible paddings for alignment
    U32     u32Pitch;           ///< Font pitch in bytes

} ST_LOADFONT_HEADER;

typedef struct
{
    U16  FontID;
    U8 output_bpp;
    U8 output_size;
    U8 output_width;
    U8 output_height;
    U16 unicode_from[FONT_MAX_RANGE];
    U16 unicode_to[FONT_MAX_RANGE];
} LOADFONT_STRUCT;


typedef struct
{
    U8 FontID;
    U16 MMapID;
} LOADFONT_PAIR;

#define FONT_TEMP_BUFFER_ADDR (((FONT_START_MEMORY_TYPE & MIU1) ? (FONT_START_ADR | MIU_INTERVAL) : (FONT_START_ADR)) + FONT_START_LEN - 1024)

//Move from MApp_Font.h
///////////////////////////////////////////////////////
INTERFACE FONT Font[MAX_FONTTABLE_NUM_OSD];
INTERFACE U16 FontHandleIndex[MAX_FONTTABLE_NUM_OSD];
//////////////////////////////////////////////////////

INTERFACE bool msAPI_Font_ReadBinHeader(U16 u16FontBinID, FONT_BIN_HEADER *pFontBinHeader, U32 *pu32BinBaseAddr);  //20091127EL
INTERFACE U8  msAPI_Font_GetLineBytes(const FONT_BIN_HEADER *pFontBinHeader);
INTERFACE U16 msAPI_Font_GetCharGlyphBytes(const FONT_BIN_HEADER *pFontBinHeader);
INTERFACE U32 msAPI_Font_GetFontGlyphBytes(const FONT_BIN_HEADER *pFontBinHeader);

INTERFACE FONTHANDLE msAPI_Font_LoadFlashFontToSDRAM(U16 u16FontBinID);
INTERFACE void msAPI_Font_ReloadFlashFontToSDRAM(FONTHANDLE handle, U16 u16FontBinID);
INTERFACE BOOLEAN msAPI_Font_LoadSDRAMFont_Decompress(U32 srcAddr, U32 u32DstAddr, FONT_BIN_HEADER *pFontBinHeader, ST_LOAD_FONT_SDRAM_REPORT *pFontSdramReport);
INTERFACE void GET_Y_BBOX_FROM_SDRAM_TO_SRAM(U32 sdram_addr, U16 char_index);

INTERFACE void msAPI_Font_RegisterLoadFontFunction(void (*pFnLoadFont)(U8));
INTERFACE void msAPI_Font_VariableInit(void);

#if VECTOR_FONT_ENABLE
INTERFACE void msAPI_Font_MVF_LoadFont(U16 nFontId, LOADFONT_STRUCT* fontinfo, U8 u8Begin, U8 u8End);
INTERFACE BOOLEAN msAPI_Font_MVF_LoadFlashFontToSDRAM(U16 u16FontBinID);
INTERFACE void msAPI_Font_MVF_AddBitmapSize(
        U16 font_id,
        U8 output_bpp, U8 output_size,
        U8 output_width, U8 output_height,
        U16* unicode_from, U16* unicode_to);
INTERFACE void msAPI_Font_MVF_StartGenerateBitmaps(void);
INTERFACE void msAPI_Font_MVF_WaitForGenerateBitmaps(void);
INTERFACE void msAPI_Font_MVF_EndGenerateBitmaps(void);
#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
    INTERFACE BOOLEAN msAPI_Font_MVF_LoadFlashFontToSDRAM(U16 u16FontBinID);
#endif
#endif //VECTOR_FONT_ENABLE

#if (VECTOR_FONT_ENABLE)
INTERFACE MB_MVF_RETURN_STATE msAPI_ProcessMailBox(void);
#endif

#ifdef DVBT_MMBOX
INTERFACE void msAPI_Font_Set2ndFontAddr(void);
#endif

#undef INTERFACE

#endif

