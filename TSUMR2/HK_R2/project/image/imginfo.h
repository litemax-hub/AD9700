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
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _IMGINFO_H_
#define _IMGINFO_H_

#include "datatype.h"

#ifdef IMGINFO_C
#define INTERFACE
#else
#define INTERFACE extern
#endif


#define PMCodeStart				msRead4Byte(0x0010F6)
#define PM_version_address     (PMCodeStart + 0x10000 - 0x20)
#define Chuck_header           (PMCodeStart - 0x80)
#define AP_version_address     (FlashRead4Byte(Chuck_header) + 0x1200)
#define Sboot_size             (FlashRead4Byte(Chuck_header + 0x24))
#define Sboot_version_address  (Sboot_size - 0x100) // Always run the first block's sboot although using dual image mode

#define IMG_TYPE_NONE   0x00
#define IMG_TYPE_BOOT   0x01
#define IMG_TYPE_APP    0x02
#define IMG_TYPE_MERGE  (IMG_TYPE_BOOT | IMG_TYPE_APP)

// Magic numbers. Don't change them! The numbers are also used externally.
#define MAGIC_BOOTLOADER        0x55AA1234  // Used by: (1) ISP tool. (2) CRC32 tool
#define MAGIC_APP               0x55AA5678  // Used by: (1) Bootloader for integrity check. (2) ISP tool. (3) CRC32 tool
#define MAGIC_IMAGE_END         0x55AAABCD  // At the end of a image (see packlist.txt and the file MagicNum55AAABCD.bin). Mainly for detection of programming failure.

#define IMG_MODEL_NAME_LEN      19
#define IMG_PANEL_NAME_LEN      19

// OAD info
#define CUSTOMER_OUI            0x0014B9    // OUI
#define CUSTOMER_SELBYTE        "UNUSED"
#define CUSTOMER_SELBYTE_LEN    4 // It's customized
#define HW_MODEL                0x01        // HW Model
#define HW_VERSION              0x01        // HW Version
#define BL_SW_MODEL             0x01        // SW Model of Bootloader
#define BL_SW_VERSION           0x01        // SW Version of of Bootloader
#define AP_SW_MODEL             0x0001        // SW Model of Application
#define AP_SW_VERSION           0xFEDC        // SW Version of of Application

typedef struct
{
    U32     u32SourceSize;              // Source size in bytes
    U32     u32FlashOffset;             // Flash offset (relative to a image) of the source.
    U32     u32DramAdr;                 // DRAM start address to be loaded at.
} MS_MEMORY_INFO;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Any change of MS_IMG_INFO may not be backward compatible with external tool and previously released bootloader
// External tools include ISP and CreateImage
typedef struct
{
    U32     u32Magic;                   // Magic number

    U16     u16PackInfoFlashBankNo;     // Flash bank No of pack information
    U16     u16PackInfoFlashOffset;     // Flash offset of pack information

    //MS_MEMORY_INFO MCU8051Code;         // Memory information of MCU8051 code
    //MS_MEMORY_INFO AEONCode;            // Memory information of AEON code
    //MS_MEMORY_INFO Database;            // Memory information of database

    U32     u32Length;
    U32     u32OUI;                     // OUI or manufacturer id
    U8      au8Selector_Byte[CUSTOMER_SELBYTE_LEN]; // Selector Byte

    U16     u16HW_Model;                // HW Model
    U16     u16HW_Version;              // HW Version

    U16     u16SW_Model;                // SW Model
    U16     u16SW_Version;              // SW Version

    U16     u16BoardType;                // Board type
    U8      u8ModelName[IMG_MODEL_NAME_LEN+1];            // Model name
    U8      u8PanelName[IMG_PANEL_NAME_LEN+1];            // Panel name
    U8      u8PanelType;
    U8      CompressMagicNum[3];
    U32     u32CompressedLength;

    //U8      u8Reserved[32];             // Reserved for future use
} MS_IMG_INFO;

// the info struct should sync with Sboot code, and the size should smaller than oxC0
typedef struct  _SBOOT_Info
{
    U8      chipid[8];
    U8      packge[4];
    U8      speed[8];
    U8      bus[4];
    U8      misc[8];
    U8      reserve[0xA0];
    U8      u8Date[12];
    U8      u8Time[9];
} SBOOT_Info;

// the info struct should sync with PM code
typedef struct  _PM_Info
{
    U8      u8Date[12];
    U8      u8Time[9];
} PM_Info;

typedef struct _CMP_Info
{
    U8      u8Date[12];
    U8      u8Time[9];
} MS_CMP_Info;

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
INTERFACE BOOLEAN MApp_ImgInfo_IsBootImage(MS_IMG_INFO *pImgInfo);
INTERFACE BOOLEAN MApp_ImgInfo_IsAppImage(MS_IMG_INFO *pImgInfo);

#if 0 // TSUMR2 TBD
INTERFACE void MApp_ImgInfo_GetBootInfo(MS_IMG_INFO *pImgInfo);
INTERFACE void MApp_ImgInfo_GetAppInfo(MS_IMG_INFO *pImgInfo);
#endif
INTERFACE void MApp_ImgInfo_GetCurModel(U8 *pModel);
INTERFACE BOOLEAN MApp_ImgInfo_IsSameModelAndPanelType(MS_IMG_INFO *pImgInfo, U8 *pModel);

INTERFACE void MApp_Imginfo_GetSboot(SBOOT_Info *pSbootInfo);
INTERFACE void MApp_Imginfo_GetPM(PM_Info *pPMInfo);
INTERFACE void MApp_Imginfo_GetAP(MS_CMP_Info *pAPInfo);
#undef INTERFACE

#endif  // _IMGINFO_H_

