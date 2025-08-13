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
///@file drvuart.h
///@brief System functions:UART Module
/// This file is vaild when "ENABLE_MSTV_UART_DEBUG" is enable
/// The brief function is get the register content by UART without halt the whole
/// system.
//
///////////////////////////////////////////////////////////////////////////////
#define _DRVUART_C_
#include "board.h"
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
//#include "msAPI_Timer.h"

// Common Definition
#include "Debug.h"
#include "hwreg.h"
#include "Ms_rwreg.h"
#include "drvUartDebug.h"
//#include "sysinfo.h"
#include "SysInit.h"
#include "drvGlobal.h"
#include "drvUART.h"
//#include "msDaisyChain.h"
#if XMODEM_DWNLD_ENABLE
#include "xmodem.h"
#endif
/* original stuff in MDrv_UART_Init() is moved to drvsys.c */
unsigned char _bIsUartDebugEnable = TRUE;

#if (ENABLE_MSTV_UART_DEBUG)

// panel
////#include "Panel.h"
////#include "apiPNL.h"

#define UARTMSG(x)    //x

/******************************************************************************/
/// UART device: putchar and ISR
/******************************************************************************/
static MS_U32 uart_debug;

void MDrv_UART_DebugEnable(unsigned char _bEnUartDebug)
{
	_bIsUartDebugEnable = _bEnUartDebug;
}

void putcharb(MS_U8 byte)
{
    if (uart_debug && _bIsUartDebugEnable)
    {
        mdrv_uart_write(uart_debug, (MS_U8 *)&byte, 1);
    }
}

#define ENABLE_UART_CHECKSUM    1

#if( ENABLE_UART_CHECKSUM )
// Check uart protocal
bit IsUartCmdValid(void)
{
    BYTE i, ucLen;
    BYTE ucCS = 0;

    if( UART_EXT )
    {
        ucLen = UART_CMD_EXT_LENGTH;
    }
    else
    {
        ucLen = UART_CMD_LENGTH;
    }

    ucCS = 0;
    for( i = 0; i < ucLen; ++ i )
    {
        ucCS += g_UartCommand.Buffer[i];
    }

    if( ucCS == 0xFF )
        return 1;

    return 0;
}
#endif

 void MDrv_UART_RecvHandler(int c)
{
#if XMODEM_DWNLD_ENABLE
    if (xmodem_mode != XMODEM_NON_ACTIVE)
    {
        xmodem_packet_receiver(c);
        goto out;
    }
    else if (g_UartCommand.Index == 0)
    {
        switch (c)
        {
            /* For those terminal software that cannot type HEX codes (implies it
               cannot generate test commands), type 'X' to start XMODEM file download. */
            case 'X':
            {
                xmodem_mode = XMODEM;

                /* Fake the Extended Command of TestCommand_XMODEM. */
                g_bUart0Detected = TRUE;
                g_UartCommand.Index = 0;
                g_Uart0CheckTick = 0;
                g_UartCommand.Buffer[0] = 0xE0;
                g_UartCommand.Buffer[1] = 0x04;
                g_UartCommand.Buffer[2] = 0x12;
                g_UartCommand.Buffer[3] = 0x09;
            }
                break;
            default:
                xmodem_mode = XMODEM_NON_ACTIVE;
                break;
        }

        if (xmodem_mode != XMODEM_NON_ACTIVE)   /* No need to go through MS_DEBUG path */
            goto out;
    }
#endif
    g_Uart0CheckTick = 50;// time-out control ms

    if (g_UartCommand.Index >= _UART_CMD_LENGTH_)
    {
        g_UartCommand.Index = 0;
    }

        g_UartCommand.Buffer[g_UartCommand.Index] = c;

    // check command buffer index
    if (UART_EXT)
    {
        if (g_UartCommand.Index == 0) // check 1st data
        {
            g_UartCommand.Index++; // for get UART_CMD_EXT_LENGTH
        }
        else
        {
            if (g_UartCommand.Index < UART_CMD_EXT_LENGTH) // still read command
            {
                g_UartCommand.Index++; // next index of command buffer
            }
            if (g_UartCommand.Index >= UART_CMD_EXT_LENGTH) // read command ok
            {
                g_bUart0Detected = TRUE; // command  buffer recieve ok
                g_UartCommand.Index = 0; // reset index of command buffer
                g_Uart0CheckTick = 0;
            }
        }
    }
    else
    {
        if (g_UartCommand.Index < UART_CMD_LENGTH) // still read command
        {
            g_UartCommand.Index++; // next index of command buffer
        }
        if (g_UartCommand.Index >= UART_CMD_LENGTH) // read command ok
        {
            // check if Enter Key. (0x0D)
            if (g_UartCommand.Buffer[_UART_CMD_CODE_] == 0x0D) {
            }

        #if( ENABLE_UART_CHECKSUM )
            if( IsUartCmdValid() )
        #endif
            g_bUart0Detected = TRUE; // command  buffer recieve ok
            g_UartCommand.Index = 0; // reset index of command buffer
            g_Uart0CheckTick = 0;
        }
    }
#if XMODEM_DWNLD_ENABLE
out:
#endif
    #if defined(MIPS_CHAKRA) || defined(__AEONR2__)
        MsOS_EnableInterrupt(E_INT_PM_IRQ_PM_UART0);
    #endif
}


/******************************************************************************/
///Initial UART relative variable and flag.
/******************************************************************************/
void MDrv_UART_DebugInit(UART_DEVICE_TYPE uart_dev)
{
    // reset Uart variables

    g_UartCommand.Index = 0;

    g_bDebugASCIICommandFlag = 0;
    g_bDebugProgStopFlag = FALSE;

    g_Uart0CheckTick = 0;

    uart_debug = mdrv_uart_open(uart_dev);

    if (uart_debug)
    {
        mdrv_uart_set_rx_callback(uart_debug, MDrv_UART_RecvHandler);
    }

/*#if (CHANNEL_SCAN_AUTO_TEST)
    g_ScanAutoTestData.u12ChNum = 0;
#endif*/
}


U8  gucDebugData0   = 0x00;
U8  gucDebugData1   = 0x00;
U16 gu16DebugData2  = 0x0000;
U16 gu16DebugData3  = 0x0000;


void MDrv_UART_ExecTestCommand(void)
{
    switch( g_UartCommand.Buffer[_UART_CMD_INDEX1_] )
    {
        case 0x50:
            g_bDebugProgStopFlag = !g_bDebugProgStopFlag;
            if( g_bDebugProgStopFlag )
            {
                printf("<Debug>\r\n");
            }
            else
            {
                printf("<Free>\r\n");
            }
            break;
        default:
            break;
    }
}
/******************************************************************************/
/// Decode the Command gets from UART port for main chip.
/// Command Buffer [0]:
///  -bit7~bit5 :Command length:1 ~ 6
///  -bit4~bit0 :UART command
/// Command Buffer [1]:
///  -Control address high.
/// Command Buffer [2]:
///  -Control address low.
/// if write command:
///   -Command buffer [3]: control data
/******************************************************************************/
void MDrv_UART_DecodeNormalCommand(void)
{
    //printf("MDrv_UART_DecodeNormalCommand %x\n", (U16)UART_CMD );
    U32 u32Tmp;

    switch ( UART_CMD )
    {
        case uartWrite_MST_Bank_n:
            u32Tmp = 0x00902F00;
            u32Tmp |= ((U8)UART_CMD_MCU_IDX_H2 << 24) |
                      (U8)UART_CMD_MCU_IDX_L;
            msWriteByte(u32Tmp, UART_CMD_MCU_DATA);
            putcharb( 0xF1 );
            break;

        case uartRead_MST_Bank_n:
            u32Tmp = 0x00902F00;
            u32Tmp |= ((U8)UART_CMD_MCU_IDX_H2 << 24) |
                      (U8)UART_CMD_MCU_IDX_L;
            putcharb( 0xF2 );
            putcharb(msReadByte(u32Tmp));
            break;

        case uartWrite_MCU_XDATA:
           {
             MDrv_WriteByte(((U8)UART_CMD_MCU_IDX_H1 << 16) |
                           ((U8)UART_CMD_MCU_IDX_H2 << 8)  |
                            (U8)UART_CMD_MCU_IDX_L,
                                UART_CMD_MCU_DATA
                          );
            }
            putcharb( 0xF1 );
            // MDrv_WriteByte(((U8)UART_CMD_MCU_IDX_H<<8) | (U8)UART_CMD_MCU_IDX_L, UART_CMD_MCU_DATA);
            //putcharb(0xF1);
            break;
        case uartRead_MCU_XDATA:
       	   {
            putcharb( 0xF2 );
            putcharb(MDrv_ReadByte(((U8)UART_CMD_MCU_IDX_H1 << 16) |
                                   ((U8)UART_CMD_MCU_IDX_H2 << 8)  |
                                   (U8)UART_CMD_MCU_IDX_L
                                  )
                    );
       	    }
            break;
	 case uartWrite_DC_XDATA:
	 {
		//msDCWriteByte(MAKEWORD( UART_CMD_MCU_IDX_H2, UART_CMD_MCU_IDX_L ), UART_CMD_MCU_DATA);
	 	putcharb( 0xF1 );
	 }
	 break;
	 case uartRead_DC_XDATA:
	 	putcharb( 0xF2 );
		//msDCReadByte(MAKEWORD( UART_CMD_MCU_IDX_H2, UART_CMD_MCU_IDX_L ), &UART_CMD_MCU_DATA);
		putcharb(UART_CMD_MCU_DATA);
	 break;
        case uartTest_Command:
            MDrv_UART_ExecTestCommand();
            break;
        default:
            // printf("Unsupport Normal Command: 0x%x\n", (U16)UART_CMD );
            break;
    }
}


#else
// Fixed compiling warning : No one refer to u8Dummy.
//static U8 code u8Dummy;
static MS_U32 uart_debug;
static void MDrv_Empty_UART_RecvHandler(int c)
{
    UNUSED(c);
}
void MDrv_UART_Register_Empty_RecvHandler(void)
{
    uart_debug = mdrv_uart_open(E_UART_AEON);
    mdrv_uart_set_rx_callback(uart_debug, MDrv_Empty_UART_RecvHandler);
}
#endif /* ENABLE_MSTV_UART_DEBUG */


BOOLEAN UART_GetChar(BYTE *u8Char)
{
    BOOLEAN bOK;

    //ES1 = 0;                      // disable uart interrupt
    //_SetInt_SERIAL1(DISABLE);
    if( uartRXByteCount > 0 )                 /* is there any char in buffer   */
    {
        bOK = TRUE;
        *u8Char = uartRXBuf[uartRxQueueTail];        /* fetch the character           */

        uartRxQueueTail++;                    /* adjust it to the next         */

        if( uartRxQueueTail == UART_RXBUF_SIZE)
        {
            uartRxQueueTail = 0x00;                   /* going round                   */
        }
        uartRXByteCount--;                     /* decrement character counter   */
    }
    else
    {
        bOK = FALSE;
    }
    //ES1 = 1;                      // enable uart interrupt
    //_SetInt_SERIAL1(ENABLE);

    return (bOK);

} /* UART_GetChar() */

void UART_Clear(void)
{
    uartRXByteCount   = 0x00;                  /* reset pointers and counter    */
     uartRxQueueHeader = 0x00;
    uartRxQueueTail = 0x00;
}

#undef _DRVUART_C_
