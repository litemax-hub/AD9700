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
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "hwreg.h"
#include "drvGlobal.h"
#include "MsTypes.h"

#define GPIO_NONE               0       // Not GPIO pin (default)
#define GPIO_IN                 1       // GPI
#define GPIO_OUT_LOW            2       // GPO output low
#define GPIO_OUT_HIGH           3       // GPO output high

#define _MapBase_nonPM_M12 0xA0200000
#define _MapBase_PM_M12    0xA0000000

#define _MEMMAP_nonPM_  _RVM1(0x0000, 0x10, 0xFF)
#define _MEMMAP_PM_     _RVM1(0x0000, 0x00, 0xFF)

static U32 u32MapBase;

static void MDrv_PAD_WriteByte(U32 u32RegAddr, U8 u8Val)
{
    ((volatile U8*)(u32MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)] = u8Val;
}

static U8 MDrv_PAD_ReadByte(U32 u32RegAddr)
{
    return ((volatile U8*)(u32MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)];
}

static void MDrv_PAD_WriteByteMask(U32 u32Reg_addr, U8 u8Value, U8 u8Mask)
{
    U8 u8Val = MDrv_PAD_ReadByte(u32Reg_addr);

    u8Val = (u8Val & ~(u8Mask)) | (u8Value & u8Mask);
    MDrv_PAD_WriteByte(u32Reg_addr, u8Val);
}

//static code U8 padInitTbl[] =
volatile U8 code padInitTbl[]= //__attribute__((__section__ (".isp_info"))) =
{
#if 0
    0x39, 0xB6, 0x5B, 0x53,     // magic code for ISP_Tool

    // programable device number
    // spi flash count
    2,
    //1 + (PIN_SPI_CZ1 != 0) + (PIN_SPI_CZ2 != 0) + (PIN_SPI_CZ3 != 0),
    0x00,                       // nor
    0x00,                       // nand
    0x00,                       // reserved
    0x00,                       // reserved
    0x00,                       // reserved

//---------------------------------------------------------------------
// GPIO Configuartion
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// Pad Configuartion
//---------------------------------------------------------------------
#endif
};

void MDrv_Pad_Init( void )
{
    U8 code *pPadConfig;
    U8  u8Length;
    U16 u16Index; // register index

    u32MapBase = _MapBase_nonPM_M12;

    pPadConfig = (U8 *)padInitTbl + 10;

    while (1)
    {
        u16Index = (pPadConfig[0] << 8) + (pPadConfig[1]);

        if (u16Index == 0xFFFF) // check end of table
            break;

        pPadConfig += 2;

        if(u16Index == 0x0000) // switch base address
        {
            if((pPadConfig[0]==0x10)||(pPadConfig[0]==0x11))
            {
                u32MapBase = _MapBase_nonPM_M12;
            }
            else if(pPadConfig[0]==0x00)
            {
                u32MapBase = _MapBase_PM_M12;
            }
            pPadConfig += 2;
            continue;
        }

        u8Length = HIGHBYTE(u16Index) >> 6;
        u16Index &= 0x3FFF;

        while (1)
        {
            //printf("%x = %bx & %bx\n", u16Index, pPadConfig[0], pPadConfig[1]);
            MDrv_PAD_WriteByteMask((U32)u16Index, pPadConfig[0], pPadConfig[1]);
            pPadConfig += 2;

            if (u8Length == 0)
                break;

            u8Length--;
            u16Index++;
        }
    }

}

//-----------------------------------------------------------------
// Sanity check for PAD_TOP setting (TODO)
//-----------------------------------------------------------------
#if 0
#if (UART0_SRC_SEL == UART1_SRC_SEL) || \
    (UART0_SRC_SEL == UART2_SRC_SEL) || \
    (UART1_SRC_SEL == UART2_SRC_SEL)
    #error "UART src selection conflict"
#endif
#endif


