//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
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
////////////////////////////////////////////////////////////////////////////////
#define _INFOBLOCK_C_
///////////////////////////////////////////////////////////////////////////////
///@file InfoBlock.c
///@version rev.00 00000
///@author MStarSemi Inc.
///@brief System Information block
///
///@endcode
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "datatype.h"
#include "hwreg.h"
#include "sysinfo.h"
#include "InfoBlock.h"
//#include "msAPI_MMap.h"
#include "MsTypes.h"
//#include "drvBDMA.h"
//#include "msIR.h"
#include "BinInfo.h"
#include "Debug.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "msflash.h"
#include "mw_usbdownload.h"

//------------------------------------------------------------------------------
// Local Defines
//------------------------------------------------------------------------------
#define INFOBLOCK_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && INFOBLOCK_DEBUG
#define INFOERROR_DBG(x)   x //MS_DEBUG_MSG(x)
#define INFOBLOCK_DBG(x)   x
#else
#define INFOERROR_DBG(x)
#define INFOBLOCK_DBG(x)
#endif

#define NULL_DATA   0xCCCCCCCC
//jing fixme plz
#define BinItem(id) PlaceU16_(id), PlaceU32_(NULL_DATA), PlaceU32_(NULL_DATA), PlaceU8_(0xCC)
// Special bin item for bootloader
#define BL_BinItemStart(id, adr) PlaceU16_(id), PlaceU32_(adr), PlaceU32_(NULL_DATA), PlaceU8_(0xCC)

//------------------------------------------------------------------------------
// Table
//------------------------------------------------------------------------------
#ifndef FLASH_SIZE
    #define FLASH_SIZE_8MB                  0x800000
    #define FLASH_SIZE                  FLASH_SIZE_8MB  // the extern flash use for power on music
#endif


volatile U8 const BinaryInfoBlock[] __attribute__((__section__ (".img_info"))) =
#if (BLOADER) //Bootloader
#else //(!BLOADER) main AP
{
    //--------------------------------------------------------------------------
    // Binary Package
    //--------------------------------------------------------------------------
    // Header ID Code
    PlaceU32_(BIN_PACKAGE_HEADER_ID0), PlaceU32_(BIN_PACKAGE_HEADER_ID1),


    // Check reserved FLASH SIZE
#if (EEPROM_DB_STORAGE == EEPROM_SAVE_ALL)
    PlaceU32_(FLASH_SIZE - 0/*CM_DATABASE_FLASH_SIZE*/),
#elif (EEPROM_DB_STORAGE == EEPROM_SAVE_NONE)
    PlaceU32_(FLASH_SIZE - CM_DATABASE_FLASH_SIZE - 1*FLASH_BLOCK_SIZE), //CM + OAD
#elif (EEPROM_DB_STORAGE == EEPROM_SAVE_WITHOUT_CH_DB)
    PlaceU32_(FLASH_SIZE - CM_DATABASE_FLASH_SIZE),
#else
    #error "EEPROM_DB_STORAGE is undefined!\n"
#endif


    // Binary Number: Filled by external tool
    PlaceU16_(0),

    ////////////////////////////////////////////////////////////////////////////
    // Bin List
    ////////////////////////////////////////////////////////////////////////////

#if 1
#if ENABLE_SECU_R2
    BinItem(BIN_ID_CODE_SECU_R2),
#endif

#if ENABLE_USB_TYPEC
//Code: PD51
#if ENABLE_INTERNAL_CC
        BinItem(BIN_ID_CODE_PD51_ICC),
#else
        BinItem(BIN_ID_CODE_PD51),
#endif
#endif

#if ENABLE_PQ_R2
    BinItem(BIN_ID_CODE_PQ_R2),
#endif

#else
#if (ENABLE_CP_R2)
    BinItem(BIN_ID_CODE_CP_R2),
#endif

#if (ENABLE_DAISY_CHAIN)
    BinItem(BIN_ID_DAISY_CHAIN),
#endif

#if (ENABLE_SECU_R2)
#if (CHIP_ID == MST9U4)
        BinItem(BIN_ID_CODE_MST9U4_SECU_R2),
#else
        BinItem(BIN_ID_CODE_SECU_R2),
#endif
#endif

#if (ENABLE_VBY1_TO_EDP)
        BinItem(BIN_ID_VBY1_TO_EDP),
#endif

    // MVF Font, ID, Address, Size
#if VECTOR_FONT_ENABLE
    BinItem(BIN_ID_FONT_MSTAR_MVF_0),
    BinItem(BIN_ID_FONT_MSTAR_MVF_1),
#else
    BinItem(BIN_ID_FONT_BMPFONT_0_0),
    BinItem(BIN_ID_FONT_BMPFONT_0_1),
    BinItem(BIN_ID_FONT_BMPFONT_0_2),
    BinItem(BIN_ID_FONT_BMPFONT_1_0),
    BinItem(BIN_ID_FONT_BMPFONT_1_1),
    BinItem(BIN_ID_FONT_BMPFONT_1_2),
#endif // #if VECTOR_FONT_ENABLE

    // BMP, ID, Address, Size
    BinItem(BIN_ID_OSDCP_BMP),
#if defined(BIN_ID_PAL_ZUI) && defined(BIN_ID_PALALPHA_ZUI)
    BinItem(BIN_ID_PAL_ZUI),
    BinItem(BIN_ID_PALALPHA_ZUI),
#endif

    // TEXT, ID, Address, Size
    BinItem(BIN_ID_OSDCP_TEXT),

#if ENABLE_FB_BMP_DRAW
    BinItem(BIN_ID_BMP_TEST_FILE),
#endif

#if ENABLE_FB_JPG_DRAW
    BinItem(BIN_ID_JPG_TEST_FILE),
#endif
#endif

    // It will be verified by bootloader for integrity check.
    BinItem(BIN_ID_MERGE_END ),

    //=====================================================================
    // Put the following ID 0xF000 at the end of this file.
    // It will be verified by bootloader for integrity check.
    BinItem(BIN_ID_MAGIC_55AAABCD)
};
#endif //(!BLOADER)

BOOLEAN Get_BinInfo(BININFO *pBinInfo)
{
    BININFO Fmt;
    U16 i, Count;
    U8 const *p;
    U32 u32DualImgOffset;

    if(g_dwSpiDuelImageOffset < ENABLE_USB_HOST_DUAL_IMAGE_OFFSET)
        u32DualImgOffset = 0;
    else
        u32DualImgOffset = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;

    Count = (U16)(BinaryInfoBlock[BIN_PACKAGE_NUMBER_ADDR]<<8) + BinaryInfoBlock[BIN_PACKAGE_NUMBER_ADDR+1];
    INFOBLOCK_DBG(printf("Get_BinInfo: Count=%u\n", Count));

    // get the whole header content
    for ( i = 0; i < Count; i++ )
    {
    #ifdef SUPPORT_AP_BIN_IN_FLASH_2
        p = (U8 *)&BinaryInfoBlock[BIN_PACKAGE_BINFO_ADDR+i*BIN_PACKAGE_BINFO_SIZE];
    #else
        p = (U8 const *)&BinaryInfoBlock[BIN_PACKAGE_BINFO_ADDR+i*BIN_PACKAGE_BINFO_SIZE];
    #endif

        Fmt.B_ID = ReadU16BE(p + 0);
        Fmt.B_FAddr = ReadU32BE(p + 2) + u32DualImgOffset;
        Fmt.B_Len = ReadU32BE(p + 6);
        Fmt.B_IsComp = p[10];

        //printf("ID = 0x%04X : IsComp = 0x%02X \n", Fmt.B_ID, Fmt.B_IsComp);
        //printf("Search ID:%04X,Addr_orig:0x%08x,  Addr:0x%08X,  Len:0x%08X, Fmt.B_IsComp:%02x\n", Fmt.B_ID, ReadU32BE(p + 2), Fmt.B_FAddr, Fmt.B_Len,Fmt.B_IsComp);
        INFOBLOCK_DBG(printf("Search ID:%04X:Addr:0x%08X, Len:0x%08X\n", Fmt.B_ID, Fmt.B_FAddr, Fmt.B_Len));
        if ( Fmt.B_ID == pBinInfo->B_ID )
        {
            pBinInfo->B_FAddr  = Fmt.B_FAddr + 0;//MDrv_MIU_SPI_GetOffset();
            pBinInfo->B_Len    = (Fmt.B_Len + 7) & ~7;
            pBinInfo->B_IsComp = Fmt.B_IsComp;
            INFOBLOCK_DBG(printf("Success ID:%04X:Addr:0x%08X, Len:0x%08X\n", pBinInfo->B_ID, pBinInfo->B_FAddr, pBinInfo->B_Len));
            return TRUE;
        }
    }

    INFOERROR_DBG(printf("Get_BinInfo::Fail ID:%04X:Addr:0x%08lX, Len:0x%08lX\n", pBinInfo->B_ID, pBinInfo->B_FAddr, pBinInfo->B_Len));

    return FALSE;
}


#undef _INFOBLOCK_C_

