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

#ifndef MSAPI_FLASH_H
#define    MSAPI_FLASH_H

#include "datatype.h"
#include "msflash.h"
#include "drvSERFLASH.h"

#ifdef MSAPI_FLASH_C
#define INTERFACE
#else
#define INTERFACE extern
#endif


typedef enum
{
    E_FLASH_CHIP_SELECT_0 = 0,
    E_FLASH_CHIP_SELECT_1 = 1,
    E_FLASH_CHIP_SELECT_2 = 2,
    E_FLASH_CHIP_SELECT_3 = 3,
}FLASH_CHIP_SELECT;

INTERFACE U16 msAPI_CalCheckSum(U32 u32FlashSize);
INTERFACE BOOLEAN msAPI_Flash_ChipSelect(FLASH_CHIP_SELECT eFlashChipSel);
INTERFACE BOOLEAN msAPI_Flash_WriteProtect(BOOL bEnable);
INTERFACE BOOLEAN msAPI_Flash_AddressErase(U32 u32StartAddr,U32 u32EraseSize,BOOL bWait);
INTERFACE BOOLEAN msAPI_Flash_BlockErase(U32 u32StartBlock, U32 u32EndBlock, BOOL bWait);
INTERFACE BOOLEAN msAPI_Flash_Write(U32 u32StartAddr, U32 u32WriteSize, U8 * user_buffer);
INTERFACE BOOLEAN msAPI_Flash_Read(U32 u32StartAddr, U32 u32ReadSize, U8 * user_buffer);
INTERFACE BOOLEAN msAPI_Flash_DetectType(void);
INTERFACE BOOLEAN msAPI_Flash_AddressToBlock(U32 u32FlashAddr, U32 * pu32BlockIndex);
INTERFACE BOOLEAN msAPI_Flash_BlockToAddress(U32 u32BlockIndex, U32 *pu32FlashAddr);
INTERFACE BOOLEAN msAPI_Flash_CheckWriteDone(void);
INTERFACE BOOLEAN msAPI_Flash_IsIndividBlockWPSupport(void);
#undef INTERFACE

#endif

