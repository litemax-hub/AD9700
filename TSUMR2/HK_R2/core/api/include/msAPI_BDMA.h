////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
/// @file  drvBDMA.h
/// @brief DMA interface header file
/// @author MStar Semiconductor Inc.
///
////////////////////////////////////////////////////////////////////////////////
#ifndef _APIBDMA_
#define _APIBDMA_

////////////////////////////////////////////////////////////////////////////////
// Compiler Directive
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Include List
////////////////////////////////////////////////////////////////////////////////
#include "datatype.h"
#include "MsTypes.h"
#include "drvBDMA.h"
#include "retcode.h"
#include "BinInfo.h"
////////////////////////////////////////////////////////////////////////////////
// Constant & Macro Definition
////////////////////////////////////////////////////////////////////////////////
#define LZSS_COMPRESS	1
#define SLZ_COMPRESS 	2
#define MS_COMPRESS 	4
#define MS_COMPRESS7	8

#define BUF_FOR_DECOMPRESS_OFFSET1       (0x300000UL)
#define BUF_FOR_DECOMPRESS_OFFSET2       (0x600000UL)

#if defined(__mips__)
	#define NON_CACHEABLE_TO_CACHEABLE_MASK (0xDFFFFFFF)
#elif defined(__arm__)
    #define NON_CACHEABLE_TO_CACHEABLE_MASK (0xDFFFFFFF)
#else
	#define NON_CACHEABLE_TO_CACHEABLE_MASK (0x7FFFFFFF)
#endif

//20100106EL
#if(CHIP_FAMILY_TYPE==CHIP_FAMILY_A6)
#define BUF_OFFSET_FOR_BITMAP_DECOMPRESS_SRC (0x300000UL)  // 3MB
#define BUF_OFFSET_FOR_BITMAP_DECOMPRESS_DST (0x100000UL)  // 1MB
#define BUF_OFFSET_FOR_BITMAP_DECOMPRESS_TMP (0x600000UL)  // 6MB
#else
#define BUF_OFFSET_FOR_BITMAP_DECOMPRESS_SRC (0xD00000UL)  // 13MB
#define BUF_OFFSET_FOR_BITMAP_DECOMPRESS_DST (0x100000UL)  // 1MB
#define BUF_OFFSET_FOR_BITMAP_DECOMPRESS_TMP (0x1000000UL)  // 16MB
#endif
#define BUF_FOR_VDEC_DECOMPRESS_OFFSET1         (0x100000UL)
#define BUF_FOR_VDEC_DECOMPRESS_OFFSET2         (0x200000UL)

////////////////////////////////////////////////////////////////////////////////
// Type & Structure Declaration
////////////////////////////////////////////////////////////////////////////////
typedef enum MEMTYPE_t
{
    MIU_FLASH = 0x00,   ///<    0: Flash
    MIU_SRAM  = 0x01,   ///<    1: SRAM
    MIU_SDRAM = 0x02,   ///<    2: SDRAM
    MIU_FILE  = 0x09,   ///<    9: FILE
} MEMTYPE;

typedef enum
{
    MCPY_LOADMVDFW,
    MCPY_LOADVDMCUFW,
    MCPY_LOADFONT,
    MCPY_LOADBITMAP,
    MCPY_LOADLOGO,
    MCPY_CCS,
} MCPY_TYPE;

// Memory copy type enumerate
typedef enum MEMCOPYTYPE_t
{
    MIU_FLASH2SDRAM,   ///< 0x02:Flash0 to SDRAM0
    MIU_SRAM2SDRAM,   ///< 0x12:SRAM to SDRAM
    MIU_SDRAM2SRAM,   ///< 0x21:SDRAM to SRAM
    MIU_SDRAM2SDRAM,   ///< 0x22:SDRAM to SDRAM
    MIU_SDRAM02SDRAM1, ///< 0x34:SDRAM0 to SDRAM1
    MIU_SDRAM12SDRAM0, ///< 0x43:SDRAM1 to SDRAM0
    MIU_SDRAM02SDRAM0, ///< 0x33:SDRAM0 to SDRAM0
    MIU_SDRAM12SDRAM1, ///< 0x44:SDRAM1 to SDRAM1
    MIU_SDRAM2SDRAM_I, ///< 0x55:SDRAM to SDRAM Inverse BitBlt
    MIU_FLASH2VDMCU,     ///< 0x60:Flash to VD MCU
    MIU_FLASH2DRAM_AEON, ///< 0x70:Flash to DRAM Aeon
	MIU_SDRAM02SDRAM2, // SDRAM0(MIU0) to SDRAM2(MIU2)
	MIU_SDRAM12SDRAM2, // SDRAM1(MIU1) to SDRAM2(MIU2)
	MIU_SDRAM22SDRAM2, // SDRAM2(MIU2) to SDRAM2(MIU2)
	MIU_SDRAM22SDRAM1, // SDRAM2(MIU2) to SDRAM1(MIU1)
	MIU_SDRAM22SDRAM0, // SDRAM2(MIU2) to SDRAM0(MIU0)
	MIU_SDRAM02FLASH,  // SDRAM0(MIU0) to FLASH(SPI0)
    MIU_SDRAM12FLASH,  // SDRAM1(MIU1) to FLASH(SPI0)
    MIU_SDRAM22FLASH,  // SDRAM2(MIU2) to FLASH(SPI0)
    MIU_SDRAM02FLASH1, // SDRAM0(MIU0) to FLASH1(SPI1)
    MIU_SDRAM12FLASH1, // SDRAM1(MIU1) to FLASH1(SPI1)
    MIU_SDRAM22FLASH1, // SDRAM2(MIU2) to FLASH1(SPI1)
	MIU_FLASH2SDRAM1,  // FLASH(SPI0) to SDRAM1(MIU1)
    MIU_FLASH2SDRAM2,  // FLASH(SPI0) to SDRAM2(MIU2)
    MIU_FLASH12SDRAM0, // FLASH1(SPI1) to SDRAM0(MIU0)
    MIU_FLASH12SDRAM1, // FLASH1(SPI1) to SDRAM1(MIU1)
    MIU_FLASH12SDRAM2, // FLASH1(SPI1) to SDRAM2(MIU2)
    
} MEMCOPYTYPE;

////////////////////////////////////////////////////////////////////////////////
// External Variable Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Prototype Declaration
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MApi_BDMA_Spi2MIU(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len, MEMCOPYTYPE eType);
BDMA_Result MApi_BDMA_MIU2Spi(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len, MEMCOPYTYPE eType);
BDMA_Result MApi_BDMA_XCopySetFWStatus(BOOLEAN bFWStatus);
BDMA_Result MApi_BDMA_Copy(U32 u32Srcaddr, U32 u32Dstaddr, U32 u32Len, MEMCOPYTYPE eType);
BDMA_Result MApi_BDMA_XCopy(MCPY_TYPE eType, U32 u32Srcaddr, U32 u32Dstaddr, U32 u32Len);
eRETCODE MDrv_DMA_LoadBin(BININFO *pBinInfo, U32 u32DstVA, U32 u32DecVA, U32 u32TmpVA);
eRETCODE MDrv_DMA_Copy(U32 u32Srcaddr, U32 u32Dstaddr, U32 u32Len, MEMCOPYTYPE eType);
eRETCODE MApi_BDMA_CopyFromResource(U32 offset,U32 destVA,U32 len);
BOOL MApi_BDMA_XCopyGetFWStatus(void);
#endif  //_DRV_DMA_IF_H
