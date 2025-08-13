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
// (MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
// File Name: SW_Config_Trunk_MST9U.h
// Description: Customization and Specialization for default board!
// Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _SW_CONFIG_TRUNK_MST9U_H_
#define _SW_CONFIG_TRUNK_MST9U_H_

#ifndef ENABLE
#define ENABLE                      1
#endif

#ifndef DISABLE
#define DISABLE                     0
#endif

//-------------------------------------------------------------------------------
// Bootloader System
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// HW IP configuration for SW part
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Common Monitors System
//-------------------------------------------------------------------------------

//--------- ZUI, related with Panel resolution ----------------------------------
#define UI_SKIN_BM_800X600X1555                 0
#define UI_SKIN_NON_BM_800X600X1555        1
#if (ZUI_BM)
#define UI_SKIN_SEL                 UI_SKIN_BM_800X600X1555
#else
#define UI_SKIN_SEL                 UI_SKIN_NON_BM_800X600X1555
#endif

// ---- Mirror ---------------------------------------------------------------------

#if (ENABLE_MIRROR)
#define MirrorEnable                            ENABLE
#else
#define MirrorEnable                    DISABLE
#endif

#if (ENABLE_FORCE_MM_HD_FB)
#define ENABLE_MM_HD_FB                            ENABLE
#else
#define ENABLE_MM_HD_FB                            DISABLE
#endif

//----Control Backlight Function-------------------------------------------------
//#define ENABLE_DLC                      ENABLE
#define MWE_FUNCTION                    DISABLE//ENABLE

//------ DataBase in NAND -----------------------------------------------------
#define DB_IN_NAND                      DISABLE

//------ DataBase in EEPROM -----------------------------------------------------
#define EEPROM_SAVE_NONE                0
#define EEPROM_SAVE_WITHOUT_CH_DB       1
#define EEPROM_SAVE_ALL                 3

#if (DB_IN_NAND)
#define EEPROM_DB_STORAGE               EEPROM_SAVE_NONE//EEPROM_SAVE_WITHOUT_CH_DB
#else
#define EEPROM_DB_STORAGE               EEPROM_SAVE_NONE//EEPROM_SAVE_WITHOUT_CH_DB
#endif

#if (EEPROM_DB_STORAGE==EEPROM_SAVE_NONE)
#define STANDBY_EEPROM_ACCESS           DISABLE
#else
#define STANDBY_EEPROM_ACCESS           ENABLE
#endif


//------ FLASH DataBase size ------------------------------------------------------
#if (EEPROM_DB_STORAGE == EEPROM_SAVE_NONE)
#define  CM_DATABASE_FLASH_SIZE    FLASH_BLOCK_SIZE * 4
#elif (EEPROM_DB_STORAGE == EEPROM_SAVE_WITHOUT_CH_DB)
#define  CM_DATABASE_FLASH_SIZE    FLASH_BLOCK_SIZE * 2
#elif (EEPROM_DB_STORAGE == EEPROM_SAVE_ALL)
#define  CM_DATABASE_FLASH_SIZE    0
#endif

//------------------------------VECTOR FONT-------------------------------------
#define FONT_SUPPORT_VECTOR_AND_BITMAP  DISABLE
#define VECTOR_FONT_ENABLE              DISABLE

#if (VECTOR_FONT_ENABLE)
    #define ENABLE_ARABIC_OSD           DISABLE
    #define ENABLE_THAI_OSD             DISABLE
#endif

#ifndef ENABLE_ARABIC_OSD
    #define ENABLE_ARABIC_OSD           DISABLE
    #define ENABLE_THAI_OSD             DISABLE
#endif

#define CHINESE_SIMP_FONT_ENABLE        DISABLE
#define CHINESE_BIG5_FONT_ENABLE        DISABLE
#if (CHINESE_SIMP_FONT_ENABLE && CHINESE_BIG5_FONT_ENABLE)
#error "CHINESE_SIMP_FONT_ENABLE and CHINESE_BIG5_FONT_ENABLE can not both enabled!!"
#endif


//#define SUPPORT_AP_BIN_IN_FLASH_2
#ifdef SUPPORT_AP_BIN_IN_FLASH_2
//=== About SUPPORT_AP_BIN_IN_FLASH_2 Flag
//** Feature:
//   -- You can put some bin file to secondary Flash
//   -- If you only modify some bin file in flash_2, you can only update the flash_2
//   -- First bin file name = MERGE.bin Secondary = MERGE2.bin
//
//** How to enable:
//   --Step1: Enable this flag "SUPPORT_AP_BIN_IN_FLASH_2"
//   --Step2: choice the Secondary flash name "#define FLASH_2_NUM       FLASH_ID2" in the board define
//   --Step3: To set the Bin file to Flash-2 , add "-2" to BinInfo.h
//     Example: #define BIN_ID_MSB1210                  0x837   //~24KB
//              //@..\..\..\core\bin\demodulator\demod_msb1210.bin -2
//   --Step4: To add function call before Bin copy "InfoBlock_Flash_2_Checking_Start(&BinInfo);"
//   --Step5: To add function call after Bin copy " InfoBlock_Flash_2_Checking_End(&BinInfo);"

#define FLASH_2_NUM       FLASH_ID2
#endif

//-------------------------------------------------------------------------------
// miscellany
//-------------------------------------------------------------------------------
#define KEEP_UNUSED_FUNC            DISABLE

#endif // _SW_CONFIG_TRUNK_MST9U_H_

