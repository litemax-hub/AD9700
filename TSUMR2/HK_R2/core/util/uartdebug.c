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
#define _UARTDEBUG_C_


#include "board.h"
//#include "DemoFineTune.h"
#include "Debug.h"

#if (ENABLE_MSTV_UART_DEBUG)
#include "drvUartDebug.h"


#include <stdio.h>
#include <string.h>

// Common Definition
#include "MsCommon.h"

#include "drvGlobal.h"
//#include "drvMIU.h"

#include "SysInit.h"

//#include "MDebug.h"
//#include "drvWDT.h"

#if 1//(CHIP_FAMILY_TYPE==CHIP_FAMILY_MST9U) || (CHIP_FAMILY_TYPE==CHIP_FAMILY_M12)
    #define ENABLE_CUS_UART_TEST_COMMAND    1
#else
    #define ENABLE_CUS_UART_TEST_COMMAND    0
#endif

#if( ENABLE_CUS_UART_TEST_COMMAND )
static U8 s_u8UartDebug_McuPauseFlag = 0;

#define XC_RIU_BASE     0x102F00
U8 XC_BK_ReadByte(U8 u8Bank, U8 u8Addr )
{
    MDrv_WriteByte(XC_RIU_BASE, u8Bank);
    return MDrv_ReadByte(XC_RIU_BASE+u8Addr);
}
U16 XC_BK_Read2Byte(U8 u8Bank, U8 u8Addr )
{
    MDrv_WriteByte(XC_RIU_BASE, u8Bank);
    return MDrv_Read2Byte(XC_RIU_BASE+u8Addr);
}
U32 XC_BK_Read3Byte(U8 u8Bank, U8 u8Addr )
{
    MDrv_WriteByte(XC_RIU_BASE, u8Bank);
    return MDrv_Read3Byte(XC_RIU_BASE+u8Addr);
}
U32 XC_BK_Read4Byte(U8 u8Bank, U8 u8Addr )
{
    MDrv_WriteByte(XC_RIU_BASE, u8Bank);
    return MDrv_Read4Byte(XC_RIU_BASE+u8Addr);
}




#define DEBUG_SSC   0
#define DEBUG_PWM   0


BOOL Cus_UART_ExecTestCommand(void)
{
    BOOL bRtn = TRUE;
    U8 u8XCBank;
    U8 ucPara1;
    U8 ucPara2;
    U8 ucPara3;

    ucPara1 = g_UartCommand.Buffer[_UART_CMD_INDEX2_];
    ucPara2 = g_UartCommand.Buffer[_UART_CMD_INDEX3_];
    ucPara3 = g_UartCommand.Buffer[_UART_CMD_INDEX4_];

    u8XCBank = MDrv_ReadByte(XC_RIU_BASE);

    switch( g_UartCommand.Buffer[_UART_CMD_INDEX1_] )
    {
        case 0x00:
            printf("\nTest Command List:\n");
            printf("01: Sclaer\n");
            printf("02: PQ\n");
            printf("03: Miu\n");
            printf("05: GOP\n");
            printf("06: Audio\n");
            printf("11: Set Mcu pause flag\n");
            break;

        case 0x11:
            s_u8UartDebug_McuPauseFlag = ucPara1;
            printf("\ns_u8UartDebug_McuPauseFlag=%x\n", s_u8UartDebug_McuPauseFlag);
            break;


        default:
            bRtn = FALSE; // Use system default test command
            break;
    }

    MDrv_WriteByte(XC_RIU_BASE, u8XCBank);
    return bRtn;
}
#endif

extern void putcharb(MS_U8 byte);

#if 1//(CHIP_FAMILY_TYPE == CHIP_FAMILY_MST9U) || (CHIP_FAMILY_TYPE==CHIP_FAMILY_M12)
    #define ENABLE_CUS_UART_CMD_RW_BANK_N   1
    #define ENABLE_DEBUG_RW_MEMORY          1
#else
    #define ENABLE_CUS_UART_CMD_RW_BANK_N   0
    #define ENABLE_DEBUG_RW_MEMORY          0
#endif

#if( ENABLE_DEBUG_RW_MEMORY )
static U32 g_u32DebugRWMemBaseAddress = 0;
static U8 DrvMemReadByte(U32 u32MemAddr )
{
    U8 *pu8;
    pu8 = (U8*)_PA2VA(u32MemAddr);
    return *pu8;
}

static void DrvMemWriteByte(U32 u32MemAddr, U8 u8Data )
{
    U8 *pu8;
    pu8 = (U8*)_PA2VA(u32MemAddr);
    *pu8 = u8Data;
}

static U8 NormalCmd_ReadXData1001xx(U8 u8Offset)
{
    U8 uc;

    switch(u8Offset)
    {
        case 0xF0:
            uc = g_u32DebugRWMemBaseAddress;
            break;
        case 0xF1:
            uc = g_u32DebugRWMemBaseAddress>>8;
            break;
        case 0xF2:
            uc = g_u32DebugRWMemBaseAddress>>16;
            break;
        case 0xF3:
            uc = g_u32DebugRWMemBaseAddress>>24;
            break;
        default:
            uc = DrvMemReadByte(g_u32DebugRWMemBaseAddress+u8Offset);
            break;
    }
    return uc;
}

static void NormalCmd_WriteXData1001xx(U8 u8Offset, BYTE u8Data)
{
    switch(u8Offset)
    {
        case 0xF0:
            g_u32DebugRWMemBaseAddress = (DWORD)u8Data|(g_u32DebugRWMemBaseAddress&0xFFFFFF00);
            printf("memAddr=%lX\n", g_u32DebugRWMemBaseAddress);
            break;
        case 0xF1:
            g_u32DebugRWMemBaseAddress = (DWORD)u8Data<<8|(g_u32DebugRWMemBaseAddress&0xFFFF00FF);
            printf("memAddr=%lX\n", g_u32DebugRWMemBaseAddress);
            break;
        case 0xF2:
            g_u32DebugRWMemBaseAddress = (DWORD)u8Data<<16|(g_u32DebugRWMemBaseAddress&0xFF00FFFF);
            printf("memAddr=%lX\n", g_u32DebugRWMemBaseAddress);
            break;
        case 0xF3:
            g_u32DebugRWMemBaseAddress = (DWORD)u8Data<<24|(g_u32DebugRWMemBaseAddress&0x00FFFFFF);
            printf("memAddr=%lX\n", g_u32DebugRWMemBaseAddress);
            break;
        default:
            DrvMemWriteByte( g_u32DebugRWMemBaseAddress+u8Offset, u8Data );
            printf("mem[%lX]=%X\n", g_u32DebugRWMemBaseAddress+u8Offset, u8Data);
            break;
    }
}
#endif // #if( ENABLE_DEBUG_RW_MEMORY )


BOOL Cus_UART_DecodeCommand(void)
{
    BOOL bRtn = TRUE;
  #if( ENABLE_CUS_UART_CMD_RW_BANK_N )
    static U8 u8XCBank = 0;
  #endif
    U32 u32Tmp;

    switch(UART_CMD)
    {
    #if( ENABLE_CUS_UART_CMD_RW_BANK_N )
        case uartWrite_MST_Bank_n:
            u32Tmp = ((U8)UART_CMD_MS_BANK_H << 16) |
                           ((U8)UART_CMD_MS_BANK_L << 8)  |
                            (U8)UART_CMD_MS_REGINDEX;

            if( u32Tmp == 0x102F00 ) // Scaler bank sel
                u8XCBank = UART_CMD_MS_REGDATA;
            else if( (u32Tmp&0xFFFF00) == 0x102F00 ) // Scaler bank
            {
                MDrv_WriteByte( 0x102F00, u8XCBank ); // Sel bank
            }

            MDrv_WriteByte( u32Tmp, UART_CMD_MS_REGDATA );

            putcharb( 0xF1 );
            break;
        case uartRead_MST_Bank_n:
            u32Tmp = ((U8)UART_CMD_MS_BANK_H << 16) |
                           ((U8)UART_CMD_MS_BANK_L << 8)  |
                            (U8)UART_CMD_MS_REGINDEX;

            /*if( u32Tmp == 0x102F00 ) // Scaler bank sel
                {}
            else*/ if( (u32Tmp&0xFFFF00) == 0x102F00 ) // Scaler bank
            {
                MDrv_WriteByte( 0x102F00, u8XCBank ); // Sel bank
            }

            putcharb( 0xF2 );
            putcharb( MDrv_ReadByte(u32Tmp) );
            //printf("\nRn[%X],", u32Tmp );
            break;
    #endif

    #if( ENABLE_DEBUG_RW_MEMORY )
        case uartWrite_MCU_XDATA:
            u32Tmp = ((U8)UART_CMD_MS_BANK_H << 16) |
               ((U8)UART_CMD_MS_BANK_L << 8)  |
                (U8)UART_CMD_MS_REGINDEX;

            if( (u32Tmp&0xFFFF00) == 0x100100 ) // R/W memory port
            {
                NormalCmd_WriteXData1001xx(u32Tmp&0xFF, UART_CMD_MCU_DATA);
                putcharb( 0xF1 );
            }
            else
            {
                bRtn = FALSE;
            }

            break;
        case uartRead_MCU_XDATA:
            u32Tmp = ((U8)UART_CMD_MS_BANK_H << 16) |
               ((U8)UART_CMD_MS_BANK_L << 8)  |
                (U8)UART_CMD_MS_REGINDEX;

            if( (u32Tmp&0xFFFF00) == 0x100100 ) // R/W memory port
            {
                putcharb( 0xF2 );
                putcharb( NormalCmd_ReadXData1001xx(u32Tmp&0xFF) );
            }
            else
            {
                bRtn = FALSE;
            }
            break;
    #endif

    #if( ENABLE_CUS_UART_TEST_COMMAND )
        case uartTest_Command:
            bRtn = Cus_UART_ExecTestCommand();
            break;
    #endif

        default:
            u32Tmp = 0; // For warning
            bRtn = FALSE;
            break;
    }

    return bRtn;
}

extern void (* CfgLevel0Action[])(void);
extern void (* DbgLevel0Action[])(void);
extern void (* CatLevel0Action[])(void);
//extern void (* UntLevel0Action[])(void);


//$
extern int DbgMenuMode;
//$
extern int DbgMenuLevel;
extern int CatMenuLevel;
extern int UntMenuLevel;
extern int CfgMenuLevel;
extern void (* CfgLevel0Action[])(void);



void UART_DecodeCommand(void)
{
    if (!g_bDisableUartDebug && g_bUart0Detected) // check command flag
    {
        g_bUart0Detected = FALSE;
        // dbg msg.
        // printf("g_UartCommand.Buffer[_UART_CMD_CODE_] = %x\n",
        // g_UartCommand.Buffer[_UART_CMD_CODE_]);
        // printf("UART_CMD_LENGTH = %x\n", UART_CMD_LENGTH);
        // printf("UART_EXT = %x\n", UART_EXT);
        { //command normal mode
            if (UART_EXT)
            {
                ProcessCommandExt();
            }
            else
            {
                if( ProcessCommand() == FALSE ) // 51 debug handler
                {
                    //if( Cus_UART_DecodeCommand() == FALSE )
                    {
                        MDrv_UART_DecodeNormalCommand();
                    }
                }
            }
        }
        //g_bUart0Detected = FALSE;
    }
}

U8 msAPI_UART_DecodeCommand( void )
{
    UART_DecodeCommand();

    // stop main loop for debug
    if( g_bDebugProgStopFlag )
        return 1;
    else
        return 0;
}


void MCUPause(char* pcStr, int i)
{
    bool bMiuProtectDetected = FALSE;
    //BYTE ucSC1_04 = XC_BK_ReadByte(1, 4);

    //if(s_u8UartDebug_McuPauseFlag&1)
    {
        if( pcStr )
            printf("%s:", pcStr);
        printf("%d.Mcu Pause:\n", i);

        //printf(" sc1_04=%X,", ucSC1_04);

        g_bDebugProgStopFlag = 1;
        while(g_bDebugProgStopFlag)
        {
            UART_DecodeCommand();
          #if ( ENABLE_WATCH_DOG == ENABLE )
            MDrv_Sys_ClearWatchDog();
          #endif

            if( bMiuProtectDetected == FALSE )
            {
                // Check MIU protect hit log
            #if(CHIP_FAMILY_TYPE==CHIP_FAMILY_MST9U) || (CHIP_FAMILY_TYPE==CHIP_FAMILY_M12)
                if( MDrv_ReadByte(0x1012DE)&0x10 )
                {
                    bMiuProtectDetected = TRUE;
                    printf("\nhit_protect_flag=%x\n", (MDrv_ReadByte(0x1012DE)&0x10)>>4 );
                    printf(" hit_protect_no=%d, hit_protect_id=%x\n", (MDrv_ReadByte(0x1012DE)&0xE0)>>5, (MDrv_ReadByte(0x1012DF)&0x3F) );
                }
            #endif
            }

        }
        printf("==> Mcu Run~\n");
    }
}


void dbgVersionMessage(void)
{
#define BOARD_NAME                  "BD_TSUMR2"
#define MIU_DRAM_LEN                0x0008000000
#define FLASH_SIZE                     0x0002000000
    printf("\n====================================");
    printf("\n=========== MONITOR_MST9U ===========\n");

    printf("\r\n[Board]: %s", BOARD_NAME);

    printf("\r\n[DRAM SIZE]: %d MB", MIU_DRAM_LEN/1024/1024);
    printf("\r\n[FLASH SIZE]: %d MB", FLASH_SIZE/1024/1024);
    printf("\n====================================\n");
}

#endif

