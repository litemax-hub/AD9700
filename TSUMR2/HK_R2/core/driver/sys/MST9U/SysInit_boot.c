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
// (“MStar Confidential Information”) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
///@file drvsys.h
///@brief System functions: Initialize, interrupt
///@author MStarSemi Inc.
///
///////////////////////////////////////////////////////////////////////////////

#define  _DRV_SYS_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hwreg.h"
#include "board.h"
#include "MsTypes.h"
#include "drvUartDebug.h"
#include "drvUART.h"
#include "drvMMIO.h"
#include "SysInit.h"
#include "drvGlobal.h"
#include "drvISR.h"
#include "drvCPU.h"
#include "drvBDMA.h"

#if defined(__aeon__)
#include "risc32_spr.h"
#endif

#if ENABLE_USB_HOST_DUAL_IMAGE_SIGN
#include "./mbedtls_2_3_0/include/mbedtls/rsa_verify.h"
#endif

/******************************************************************************/
/*                     Temporary Defines                                                                             */
/******************************************************************************/
//#define MST9U4  3
//#define MST9U5  4
//#undef CHIP_ID
//#define CHIP_ID         MST9U4 //Leo-temp for InitialSystemPowerUp()

//#define TSUMR2_FPGA0

#define ENABLE_PRINT_MSG        1


#ifdef TSUMR2_FPGA
#define R2_CLK       60000000//71590000 //71400000 //60000000
U32 g_FPGA_CPU_CLOCK = 60000000; //71400000; //60000000; //36000000
#else
#define R2_CLK       (320000000)
#define R2_CLK_SET   (0x1<<2)
#endif


static U8 u8OrigWdtTimer = 0x0A;    //Default 10 sec

/******************************************************************************/
/*                     Local Defines                                          */
/******************************************************************************/
#define PM_LOCK_SUPPORT             0// 1   // 1: Enable 0: Disable
#if (PM_LOCK_SUPPORT == 1)
#include "drvSEM.h"
#endif

// set 1 to enable support for download image to dram and run from MSTV_Tool
#define DOWNLOAD_RUN_SERVER     0
#define DBG_SYS_PNT(x)   //x

#define SYSINIT_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && SYSINIT_DEBUG
#define SYSINIT_PRINT(format, ...)     printf_boot(format, ##__VA_ARGS__)
#else
#define SYSINIT_PRINT(format, ...)
#endif

#if 0//ENABLE_COMPILE_INFO
MS_CMP_Info code g_cmpInfo __attribute__((__section__ (".cmp_img_info"))) =
{
	__DATE__,
	__TIME__
};
#endif

#if ENABLE_MSTV_UART_DEBUG && SYSINIT_DEBUG
extern int vsnprintf_boot(char *str, size_t size, const char *fmt, va_list ap) __attribute__ ((far));
int printf_boot(const char *fmt, ...) __attribute__ ((far));
int printf_boot(const char *fmt, ...)
{
    int i, iret;
    char szPrint[PRINTF_CHAR_MAX];
    va_list ap;

	if (_bIsUartDebugEnable == 0x00)
	{
		return 0;
	}
	else
	{
		va_start(ap, fmt);
		iret = vsnprintf_boot(szPrint, PRINTF_CHAR_MAX, fmt, ap);
		va_end(ap);

		for (i = 0; i < iret; i++)
		{
		    putchar(szPrint[i]);
		}

		return iret;
	}
}
#endif

/******************************************************************************/
/*                     Global Variables                                       */
/******************************************************************************/

// memory map for pre-built libraries
//U32 u32SecbufAddr = ((SECBUF_START_MEMORY_TYPE & MIU1) ? (SECBUF_START_ADR | MIU_INTERVAL) : (SECBUF_START_ADR));
//U32 u32SecbufSize = SECBUF_START_LEN;

U32 u32CRCbufAddr = NULL;
U32 u32NewSPIBase = 0;

unsigned long RIU_MAP;

#define u8ChipIdMajor   REG8(REG_CHIP_ID_MAJOR)
#define u8ChipIdMinor   REG8(REG_CHIP_ID_MINOR)
#define u8ChipVer       REG8(REG_CHIP_VERSION)
#define u8ChipRev       REG8(REG_CHIP_REVISION)

/******************************************************************************/
void MDrv_Sys_InitUartForXtalClk( char cOutputChar )
{
    UNUSED(cOutputChar);
    MDrv_WriteRegBit(0x1E03, ENABLE, BIT2);          // enable UART RX
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_Sys_Get_BinInfo()
/// @brief \b Function \b Description : Get Specific Bin information
/// @param <IN>        \b None        :
/// @param <OUT>       \b pBinInfo    : Get Bin Information
/// @param <RET>       \b BOOLEAN     : Success or Fail
/// @param <GLOBAL>    \b None        :
////////////////////////////////////////////////////////////////////////////////
#if 0
void MDrv_Sys_Get_BinInfo(BININFO *pBinInfo, BOOLEAN *pbResult)
{
    BININFO NewBinInfo;

    // Added by coverity_0505
    memset(&NewBinInfo, 0, sizeof(BININFO));

    NewBinInfo.B_ID = pBinInfo->B_ID;
    *pbResult = Get_BinInfo(&NewBinInfo);
    pBinInfo->B_FAddr = NewBinInfo.B_FAddr;
    pBinInfo->B_Len   = NewBinInfo.B_Len;
    pBinInfo->B_IsComp = NewBinInfo.B_IsComp;
}
#endif

/******************************************************************************/
/// DO NOT MODIFY THIS FUNCTION, IT IS VERY DANGEROUS! (called by vectors.S)
/******************************************************************************/

#define STACK_SIZE      131072

//---------------------------------------------------------
//---------------------------------------------------------
#if ENABLE_MSTV_UART_DEBUG
static void console_init(void)
{
#ifdef TSUMR2_FPGA
    MDrv_UART_Init(DEFAULT_UART_DEV, 38400);
    mdrv_uart_connect(E_UART_PORT_GPIO40_41/*E_UART_PORT_GPIOX03_04*/,DEFAULT_UART_DEV);
#else
    //before querying CPU_clock, need to set base address for driver
    MDrv_UART_Init(DEFAULT_UART_DEV, 115200);
    mdrv_uart_connect(DEFAULT_UART_PORT,DEFAULT_UART_DEV);
#endif

    SYSINIT_PRINT("\nDefault port(0x%x) connecting to 0x%x\n", DEFAULT_UART_PORT, mdrv_uart_get_connection(DEFAULT_UART_PORT));
    SYSINIT_PRINT("\n=== console_init ok (%s %s) ===\n\n", __TIME__, __DATE__);
}
#endif

void MDrv_Timer_Init(void) // Now support mode 1
{
    gTimerCount0 = 0;
    gSystemTimeCount = 0;
    gTimerDownCount0 = 0;
    gTimerDownCount1 = 0;
    gu8100msTH = 0;
    g_u16ElapsedTimeSecond = 0;
    g_u8TimeInfo_Flag = 0;
}

extern unsigned char __data_start_s[];
extern unsigned char __data_end_s[];
extern unsigned char __data_start_d[];

void Init_Data(void);
void Init_Data(void)
{
    U8 *spt = __data_start_s; // source
    U8 *dpt = __data_start_d; // dest
//    *((U32 *)__data_start_d) = 0xaabbccdd;
//    *((U32 *)__data_start_d+1) = 0xdeaddead;

    while(spt<__data_end_s)
    {
        *dpt++ = *spt++;
    }
}

#ifdef TSUMR2_FPGA0  //test code for the very begining
#define UART0_BAUDRATE              115200
#define UART0_BAUDRATE_FPGA         38400
#ifdef TSUMR2_FPGA
#define DEFAULT_UART_DEV            0 //UART_ENGINE_R2_HK
#else
#define DEFAULT_UART_DEV            UART_ENGINE_R2_HK
#endif
#if(CHIP_ID == MST9U4)
#define DEFAULT_UART_PORT           UART_GPIOX33_34
#else
#define DEFAULT_UART_PORT           UART_GPIOX33_34 //UART_GPIO40_41
#endif

#define __USE__AEON_UART__

#ifdef __USE__AEON_UART__
#undef UART_REG8
#define UART_BASE       0xE0000000 //0x90000000
#define UART_REG8(_x_)   ((volatile unsigned char *)UART_BASE)[_x_]
#else
#define UART_BASE       (0xA0200000+0x1300)
#define UART_REG8(_x_)  ((volatile unsigned char *)(UART_BASE))[((_x_) * 4) - ((_x_) & 1)]
#endif

#if 1
#ifdef __USE__AEON_UART__
#define UART_RX     0    // In:  Receive buffer (DLAB=0)    (16-byte FIFO)
#define UART_TX     0    // Out: Transmit buffer (DLAB=0)   (16-byte FIFO)
#define UART_DLL    0    // Out: Divisor Latch Low (DLAB=1)
#define UART_DLM    1    // Out: Divisor Latch High (DLAB=1)
#define UART_IER    1    // Out: Interrupt Enable Register
#define UART_IIR    2    // In:  Interrupt ID Register
#define UART_FCR    2    // Out: FIFO Control Register
#define UART_LCR    3    // Out: Line Control Register
#define UART_MCR    4    // Out: Modem Control Register
#define UART_LSR    5    // In:  Line Status Register
#define UART_MSR    6    // In:  Modem Status Register
#define UART_SCR    7    // I/O: Scratch Register
#else
#define UART_RX        (0 * 2)  // In:  Receive buffer (DLAB=0)
#define UART_TX        (0 * 2)  // Out: Transmit buffer (DLAB=0)
#define UART_DLL       (0 * 2)  // Out: Divisor Latch Low (DLAB=1)
#define UART_DLM       (1 * 2)  // Out: Divisor Latch High (DLAB=1)
#define UART_IER       (1 * 2)  // Out: Interrupt Enable Register
#define UART_IIR       (2 * 2)  // In:  Interrupt ID Register
#define UART_FCR       (2 * 2)  // Out: FIFO Control Register
#define UART_LCR       (3 * 2)  // Out: Line Control Register
#define UART_LSR       (5 * 2)  // In:  Line Status Register
#define UART_MSR       (6 * 2)  // In:  Modem Status Register
#endif

// UART_FCR(2)
// FIFO Control Register (16650 only)
//
#define UART_FCR_ENABLE_FIFO        0x01        // Enable the FIFO
#define UART_FCR_CLEAR_RCVR         0x02        // Clear the RCVR FIFO
#define UART_FCR_CLEAR_XMIT         0x04        // Clear the XMIT FIFO
#define UART_FCR_DMA_SELECT         0x08        // For DMA applications
#define UART_FCR_TRIGGER_MASK       0xC0        // Mask for the FIFO trigger range
//#define UART_FCR_TRIGGER_1          0x00        // Mask for trigger set at 1
//#define UART_FCR_TRIGGER_4          0x40        // Mask for trigger set at 4
#define UART_FCR_TRIGGER_8          0x80        // Mask for trigger set at 8
#define UART_FCR_TRIGGER_14         0xC0        // Mask for trigger set at 14

//
// UART_LCR(3)
// Line Control Register
// Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
// UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
//
#define UART_LCR_WLEN5              0x00        // Wordlength: 5 bits
#define UART_LCR_WLEN6              0x01        // Wordlength: 6 bits
#define UART_LCR_WLEN7              0x02        // Wordlength: 7 bits
#define UART_LCR_WLEN8              0x03        // Wordlength: 8 bits
#define UART_LCR_STOP1              0x00        // Stop bits: 0=1 stop bit, 1= 2 stop bits
#define UART_LCR_STOP2              0x04        // Stop bits: 0=1 stop bit, 1= 2 stop bits
#define UART_LCR_PARITY             0x08        // Parity Enable
#define UART_LCR_EPAR               0x10        // Even parity select
#define UART_LCR_SPAR               0x20        // Stick parity (?)
#define UART_LCR_SBC                0x40        // Set break control
#define UART_LCR_DLAB               0x80        // Divisor latch access bit

// UART_FCR(4)
// FIFO Control Register (16650 only)
#define UART_FCR_MASK               0x00FF
#define UART_FCR_RXFIFO_CLR         0x0001
#define UART_FCR_TXFIFO_CLR         0x0002
#define UART_FCR_TRIGGER_0          0x0000
#define UART_FCR_TRIGGER_1          0x0010
#define UART_FCR_TRIGGER_2          0x0020
#define UART_FCR_TRIGGER_3          0x0030
#define UART_FCR_TRIGGER_4          0x0040
#define UART_FCR_TRIGGER_5          0x0050
#define UART_FCR_TRIGGER_6          0x0060
#define UART_FCR_TRIGGER_7          0x0070

//
// UART_LSR(5)
// Line Status Register
//
#define UART_LSR_DR                 0x01          // Receiver data ready
#define UART_LSR_OE                 0x02          // Overrun error indicator
#define UART_LSR_PE                 0x04          // Parity error indicator
#define UART_LSR_FE                 0x08          // Frame error indicator
#define UART_LSR_BI                 0x10          // Break interrupt indicator
#define UART_LSR_THRE               0x20          // Transmit-hold-register empty
#define UART_LSR_TEMT               0x40          // Transmitter empty
#endif

typedef enum // UART GPIO
{
    UART_GPIO02_03,
    UART_GPIO40_41,  // VGA ?
    UART_GPIO50_51,
    UART_GPIOX03_04, // combo 0
    UART_GPIOX13_14, // combo 1
    UART_GPIOX23_24, // combo 2
    UART_GPIOX33_34, // combo 3
    UART_GPIOX43_44, // combo 4
    UART_GPIOX53_54 // combo 5
}UART_GPIO;

typedef enum // UART Engine
{
    UART_ENGINE_51_UART0,
    UART_ENGINE_51_UART1,
    UART_ENGINE_R2_HK,
    UART_ENGINE_R2_NONHK,
    UART_ENGINE_DW_uart,
    RESERVED
}UART_ENGINE;

#if ENABLE_PRINT_MSG
void mcuSetUartMux2( UART_ENGINE ucUartEngine, UART_GPIO ucUartGPIO );
void mcuSetUartMux2( UART_ENGINE ucUartEngine, UART_GPIO ucUartGPIO )
{
    switch(ucUartGPIO)
    {
        case UART_GPIO02_03:
            MDrv_WriteByteMask(0x000414, ucUartEngine, 0x07);
            break;
        case UART_GPIO40_41:
            MDrv_WriteByteMask(0x000415, ucUartEngine, 0x07);
            break;
        case UART_GPIO50_51:
            MDrv_WriteByteMask(0x000414, ucUartEngine<<4, 0x70);
            break;
        case UART_GPIOX03_04:
            MDrv_WriteByteMask(0x000410, ucUartEngine, 0x07);
            break;
        case UART_GPIOX13_14:
            MDrv_WriteByteMask(0x000410, ucUartEngine<<4, 0x70);
            break;
        case UART_GPIOX23_24:
            MDrv_WriteByteMask(0x000411, ucUartEngine, 0x07);
            break;
        case UART_GPIOX33_34:
            MDrv_WriteByteMask(0x000411, ucUartEngine<<4, 0x70);
            break;
        case UART_GPIOX43_44:
            MDrv_WriteByteMask(0x000412, ucUartEngine, 0x07);
            break;
        case UART_GPIOX53_54:
            MDrv_WriteByteMask(0x000412, ucUartEngine<<4, 0x70);
            break;
        default:
            //DRVMCU_PRINT("******UART MUX ERROR*****\n");
            break;
    }
}
#endif

void uart_init(U32 aeon_clock, U32 uart_clock)
{
    int divisor;
#if 1
#ifdef __USE__AEON_UART__
    // Reset receiver and transmiter
    UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1;

    // Set 8 bit char, 1 stop bit, no parity
    UART_REG8(UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY);
    divisor = ((aeon_clock) / (16 * uart_clock));

    UART_REG8(UART_LCR) |= UART_LCR_DLAB;
    UART_REG8(UART_DLL) = divisor & 0xFF;
    UART_REG8(UART_DLM) = (divisor >> 8) & 0xFF;
    UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);
#else
    // UART mode
    *(volatile U32*)(0xA0200000+(0x0F6E*4)) &= ~0x0F00;

    UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_14;
    /* Interrupt Enable Register */
    UART_REG8(UART_IER) = UART_IER_RDI;     // Receive data available
    /* Set 8 bit char, 1 stop bit, no parity */
    UART_REG8(UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY);
    /* Set baud rate */
    divisor = aeon_clock/(16 * uart_clock);
    UART_REG8(UART_LCR) |= UART_LCR_DLAB;
    UART_REG8(UART_DLL) = divisor & 0x000000ff;
    UART_REG8(UART_DLM) = (divisor >> 8) & 0x000000ff;
    UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);
#endif
#endif
}

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

#define WAIT_FOR_XMITR \
        do { \
                lsr = UART_REG8(UART_LSR); \
        } while ((lsr & BOTH_EMPTY) != BOTH_EMPTY)

#define WAIT_FOR_THRE \
        do { \
                lsr = UART_REG8(UART_LSR); \
        } while ((lsr & UART_LSR_THRE) != UART_LSR_THRE)

void uart_putc(char c)
{
#if ENABLE_PRINT_MSG
#ifdef __USE__AEON_UART__
    unsigned char lsr;

    WAIT_FOR_THRE;
    UART_REG8(UART_TX) = c;
    WAIT_FOR_XMITR;
#else
    while (!(UART_REG8(UART_LSR) & UART_LSR_THRE));
    UART_REG8(UART_TX) = c;
#endif
#else
    c = c;
#endif
}

void print_hex(unsigned long num)
{
    int i;
    U32 digit;

    uart_putc('0');
    uart_putc('x');
    for (i=7; i>=0; i--)
    {
        digit = (num>>(4*i))&0xF;
        if (digit >9)
            uart_putc(digit-10+'A');
        else
            uart_putc(digit+'0');
    }
    uart_putc('\r');
    uart_putc('\n');
}
static void MDrv_InitConsole( void )
{
    #ifdef TSUMR2_FPGA
    {
    #if ENABLE_PRINT_MSG
        mcuSetUartMux2(DEFAULT_UART_DEV, UART_GPIO40_41);
        uart_init(R2_CLK/*12000000*/, UART0_BAUDRATE_FPGA);
    #endif
        mtspr( SPR_TTMR, 0x40000000 | (R2_CLK/1000)/*12000*/ );
    }
    #else
    {
    #if ENABLE_PRINT_MSG
        mcuSetUartMux2(DEFAULT_UART_DEV, DEFAULT_UART_PORT);
        uart_init(R2_CLK, UART0_BAUDRATE);
    #endif
        mtspr( SPR_TTMR, 0x40000000 | (R2_CLK/1000) );
    }
    #endif
}
#endif

void MDrv_SysDelayUs(U32 u32InputTime)
{
    U32 u32DelayTime;
    U16 u16TMRStatus;

    u32DelayTime = 12 * u32InputTime;

    MDrv_Write2Byte(0x002C24, u32DelayTime & 0x0000FFFF);
    MDrv_Write2Byte(0x002C26, u32DelayTime >>16);
    MDrv_Write2Byte(0x002C20, (MDrv_Read2Byte(0x002C20) | BIT1));

    do{
        u16TMRStatus = MDrv_Read2Byte(0x002C22);
        u16TMRStatus &= BIT0;
    }while(u16TMRStatus == 0);
}

void SetSPI_Quad_Enable(void);
void SetSPI_Quad_Enable(void)
{
    //    WREG(0x09C0) = 0x06;   //SPI Flash WREN Command
    //    WREG(0x09C1) = 0x01;   //SPI Flash Write Command
    MDrv_Write2Byte(0x09C0, 0x0106);

#if 1 //Flash MX25L1636E .. 1 Bytes SR, QE in SR-1[6]
    //    WREG(0x09C2) = 0x40;
    //    WREG(0x09C3) = 0x05;   //SPI Flash RDSR Command
    MDrv_Write2Byte(0x09C2, 0x0540);
    //    WREG(0x09D4) = 0x21;   //Write length: 1 Bytes / 2 Bytes / 1 Bytes
    //    WREG(0x09D5) = 0x01;
    MDrv_Write2Byte(0x09D4, 0x0121);
#else //Flash W25Q128 .. 2 Bytes SR, QE in SR-2[1]
    //    WREG(0x09C2) = 0x00;
    //    WREG(0x09C3) = 0x02;
    MDrv_Write2Byte(0x09C2, 0x0200);
    //    WREG(0x09C4) = 0x05;   //SPI Flash RDSR Command
    MDrv_Write2Byte(0x09C4, 0x0005);
    //    WREG(0x09D4) = 0x31;   //Write length: 1 Bytes / 3 Bytes / 1 Bytes
    //    WREG(0x09D5) = 0x01;
    MDrv_Write2Byte(0x09D4, 0x0131);
#endif
    //    WREG(0x09D6) = 0x00;   //Read length: 0 Bytes / 0 Bytes / 1 Bytes
    //    WREG(0x09D7) = 0x01;
    MDrv_Write2Byte(0x09D6, 0x0100);

    //    WREG(0x09D9) = 0xf0;   //enable second/third commands and auto check status
    //    WREG(0x09D8) = 0x07;  //fsp enable and interrupt enable
    MDrv_Write2Byte(0x09D8, 0xf007);

    //    WREG(0x09DA) = 0x01;        //Trigger
    MDrv_Write2Byte(0x09DA, 0x0001);

    //Done = RREG(0x09DC, Bit(0))  //Check FSP done flag
    //WREG(0x09DE, Bit(0)) = 1     //Clear FSP done flag
    //    while(!(WREG(0x09DC)&BIT0))
    while(!(MDrv_ReadByte(0x09DC)&BIT0))
    {
    }
    MDrv_Write2Byte(0x09DE, (MDrv_ReadByte(0x09DE)|BIT0));     //Clear FSP done flag
}

void SetSPI_Mode(void);
void SetSPI_Mode(void)
{
#define REG_SPI_BANK           (0xA00)
#define REG_SEL_MODE           (REG_SPI_BANK + 0x72*2)
#define REG_PADTOP_BANK        (0x400)
#define REG_SPI_QUAD_EN        (REG_PADTOP_BANK + 0x2*2)
typedef enum spi_mode
{
    spi_normal,
    spi_fast,
    spi_single_dual,
    spi_dual_dual,
    spi_single_quad = 0xa,
    spi_quad_quad = 0xb
} spi_mode_t;

#if ENABLE_USB_HOST_DUAL_IMAGE_SIGN
    U16  u16SpiMode = spi_single_dual; // need sync with PM code setting for AC_ON consistency
#else
    U16  u16SpiMode = spi_fast; // need sync with PM code setting for AC_ON consistency
#endif

    if (u16SpiMode >= 0xa)
    {
        MDrv_Write2Byte(REG_SEL_MODE, spi_fast); // for FSP (SetSPI_Quad_En()) not support quad mode
        SetSPI_Quad_Enable();
        MDrv_Write2Byte(REG_SPI_QUAD_EN, MDrv_Read2Byte(REG_SPI_QUAD_EN)|BIT11);
    }
    MDrv_Write2Byte(REG_SEL_MODE, u16SpiMode);
}

void SetSPI_Speed( BYTE ucIndex );
void SetSPI_Speed( BYTE ucIndex )
{
    if( ucIndex == IDX_SPI_CLK_XTAL)
    {
        MDrv_WriteByteMask( 0x0AE0, 0, _BIT2 );
    }
    else
    {
        MDrv_WriteByteMask(REG_000AE0, 0, BIT2); // switch to xtal first for glitch free

    #if (CHIP_ID == CHIP_MT9701)
        if(ucIndex == IDX_SPI_CLK_SSC)
        {
            MDrv_WriteByteMask(REG_000AB4, SPI_SSC_DIVIDER_A, 0x0F);
            MDrv_WriteByteMask(REG_000AB4, SPI_SSC_DIVIDER_B, 0xF0);
            MDrv_WriteByteMask(REG_000AB5, SPI_SSC_DIVIDER_C, 0x0F);
            MDrv_WriteByteMask(REG_000AB5, SPI_SSC_DIVIDER_D, 0xF0);
            MDrv_WriteByteMask(REG_000AB6, BIT0, BIT0);
        }
    #endif

        MDrv_WriteByteMask( 0x0AE0, ucIndex << 3, _BIT5 | _BIT4 | _BIT3);
        MDrv_WriteByteMask( 0x0AE0, _BIT2, _BIT2 );
    }
}


#if (CHIP_ID == CHIP_MT9701)
void MDrv_SyninpllR2Init(void) // syninpll 500M for HKR2 and PQR2
{
    #define SynthCLK 125

    BYTE u8LoopDivSecond, u8OutputDiv;
    DWORD u32MPLL_MHZ = 432ul; //unit:MHz
    DWORD u32SynthSet = 0;

    u8LoopDivSecond = 4;
    u8OutputDiv = 0;

    // AUPLL init
    MDrv_WriteByte(REG_101E65, 0x00);
    MDrv_WriteByte(REG_101E66, 0xA1);

    //set SYNTHESIZER's frequency
    u32SynthSet = ((float)u32MPLL_MHZ * 524288) / SynthCLK;
    MDrv_Write2Byte(REG_101E50, (u32SynthSet&0xFFFF));  //reg_synth_set
    MDrv_WriteByteMask(REG_101E52, (u32SynthSet>>16)&0xFF, 0xFF);   //reg_synth_set
    MDrv_WriteByteMask(REG_101E60, BIT0, BIT0);                     //reg_synth_sld
    MDrv_WriteByteMask(REG_101E69, u8LoopDivSecond, 0xFF);          //reg_syninpll_loopdiv_second
    MDrv_WriteByteMask(REG_101E68, u8OutputDiv, 0x07);              //reg_syninpll_output_div
    MDrv_WriteByteMask(REG_101E66, 0, BIT4);                        //reg_syninpll_pd
}
#endif

static int InitialSystemPowerUp( void )
{
#ifdef TSUMR2_FPGA

    return 0;

#else
//-----------------------------------------------------------------
// MPLL setting
//-----------------------------------------------------------------
    MDrv_WriteByte(0x101ED0, 0x92);
    MDrv_WriteByte(0x101ED1, 0x54);
    MDrv_WriteByte(0x101EDC, 0x00);
    MDrv_WriteByte(0x101EDD, 0x00);

#if (CHIP_ID == CHIP_MT9701)
    MDrv_WriteByteMask(REG_120F06, BIT2, BIT2); // set miu clk to 216 TBD!!

    MDrv_SyninpllR2Init();
#endif

#if 0
#if (CHIP_ID==MST9U4)
//-----------------------------------------------------------------
//  UPLL  setting
//-----------------------------------------------------------------
    MDrv_Write2Byte(0x112380, 0x01c0);  // upll power on

//---------------------------
//enable uhc mac
//---------------------------
    MDrv_Write2Byte(0x112308, 0x040f);
    MDrv_Write2Byte(0x112300, 0x0001);
    MDrv_WriteByte(0x112200, 0x0a);
    MDrv_WriteByte(0x112200, 0x28);
#endif
#endif

//-----------------------------------------------------------------
//  UTMI  setting
//-----------------------------------------------------------------
#if 0
#if (CHIP_ID==MST9U4)
   MDrv_Write2Byte(0x112322, 0x2088);
   MDrv_Write2Byte(0x112320, 0x8051);
   MDrv_Write2Byte(0x112302, 0x2084);
   MDrv_Write2Byte(0x112308, 0x0426);
   MDrv_Write2Byte(0x112300, 0x6bc3);
   MDrv_SysDelayUs(1000);
   MDrv_Write2Byte(0x112300, 0x69c3);
   MDrv_SysDelayUs(2000);
   MDrv_Write2Byte(0x112300, 0x0001);
#else
    MDrv_Write2Byte(0x150308, 0x040f);
    MDrv_Write2Byte(0x150320, 0x80a1);
    MDrv_Write2Byte(0x150322, 0x0088);
    MDrv_Write2Byte(0x150300, 0x0001);
    MDrv_Write2Byte(0x150338, 0x2801);
#endif
#endif
//-------------------------------------------------------------------
//  turn on all clocks
//-------------------------------------------------------------------

    MDrv_WriteByte(0x120F1F, 0x00);   // reg_ckg_pll_idclk
    MDrv_WriteByte(0x120F23, 0x04);   // reg_ckg_imi
    MDrv_WriteByte(0x120F30, 0x00);   // reg_ckg_sosd_ft

//-------------------------------------------------------------------
//  clk_mcu ("switch nonPM clk_mcu to 216MHz");
//-------------------------------------------------------------------

    MDrv_WriteByte(0x0003BB, 0x02);
    MDrv_WriteByte(0x0003BC, 0x11);

//-------------------------------------------------------------------
//  clk_mcu ("switch PM clk_mcu to <MCU_SPEED_INDEX> in PM code");
//-------------------------------------------------------------------
    //////////////////////////////////////////////////
    // need sync with PM code setting for AC_ON consistency //
#if 0 //Leo-temp
    if (!IS_WAKEUP_FROM_PM) // AC ON
    {
#if ( CHIP_ID == MST9U3 )
        MDrv_WriteByte(0x0003bb, IDX_MCU_CLK_MPLL_DIV); // need sync with PM code setting for AC_ON consistency
#else
        MDrv_WriteByte(0x0003bb, IDX_MCU_CLK_216MHZ); // need sync with PM code setting for AC_ON consistency
#endif
        MDrv_WriteByte(0x0003bc, 0x01);
    }
    #endif
    // need sync with PM code setting for AC_ON consistency //
    //////////////////////////////////////////////////

//-------------------------------------------------------------------
//  clk_r2 ("switch clk_r2 to 345MHz");
//-------------------------------------------------------------------

    MDrv_WriteByte(0x0003B3, 0x80);

//-------------------------------------------------------------------
//  reg_ckg_r2_secure ("switch reg_ckg_r2_secure to 345MHz");
//-------------------------------------------------------------------

    MDrv_WriteByte(0x120F0C, 0x20);

//-------------------------------------------------------------------
//  SPI_clk = 54Mhz, [4:2]:spi clk sel 000~100: xtal, 101: 54, 110: 86, 111: 108
//-------------------------------------------------------------------

    MDrv_WriteByteMask(0x120F08, 0x14, 0x1F);
    SetSPI_Speed(SPI_SPEED_INDEX);
    SetSPI_Mode();

#endif //#ifdef TSUMR2_FPGA

    return 1;
}

#define REG_IO1_BASE      0x28AA

void putc2(char ch)
{
    volatile int i = 0;
    U32 IO1_BASE = MDrv_Read2Byte(REG_IO1_BASE);

    IO1_BASE = 0xE0000000; //<<= 16;
    *(volatile char*)IO1_BASE= ch;
    while(i++ < 0x2000);
}

static void SysInit_UART_Test1(void)
{
#ifdef TSUMR2_FPGA0
    U32 spr1 = 0;
    spr1 = mfspr(SPR_SR);

    MDrv_InitConsole();

    MDrv_COPRO_GetBase();
    U32 cpu_clock = MDrv_CPU_QueryClock();

    print_hex(0xaabbccdd);
    uart_putc('S');
    uart_putc('P');
    uart_putc('R');
    uart_putc('_');
    uart_putc('S');
    uart_putc('R');
    uart_putc('=');
    print_hex(spr1);
    uart_putc('C');
    uart_putc('P');
    uart_putc('U');
    uart_putc('_');
    uart_putc('C');
    uart_putc('L');
    uart_putc('K');
    uart_putc('=');
    print_hex(cpu_clock);

    RIU_MAP = 0xA0000000;
    U32 IO1_BASE = MDrv_Read2Byte(REG_IO1_BASE);
    print_hex(UART_BASE);
    print_hex(IO1_BASE);

    int i = 0, i2 = 0;
    while(1)
    {
        if ((i++ % 0x2000000) == 0)
        {
            uart_putc('h');
            uart_putc('e');
            uart_putc('l');
            uart_putc('l');
            uart_putc('o');
            print_hex(i2++);
            i = 1;
            if (i2 >= 5)
                break;
        }
    }
    uart_putc('e');
    uart_putc('n');
    uart_putc('d');
    uart_putc('\r');
    uart_putc('\n');
#endif

}

static void SysInit_UART_Test2(void)
{
    U32 IO1_BASE1 = MDrv_Read2Byte(REG_IO1_BASE);
    SYSINIT_PRINT("REG_IO1_BASE=0x%x\n", REG_IO1_BASE);
    SYSINIT_PRINT("IO1_BASE1=0x%x\n", IO1_BASE1);
    //IO1_BASE1 = msRead2Byte(REG_IO1_BASE);
    SYSINIT_PRINT("IO1_BASE1_1=0x%x\n", IO1_BASE1);

#if 0
    int j = 0, k = 0;
    while(1)
    {
        if ((j++ % 0x2000000) == 0)
        {
            printf("hello.. %d\n", k++);
            j = 1;
            if (k>=5)
                break;
        }
    }
#endif
}

static void SysInit_TICK_Test(void)
{
    int tick_ms = 0;
    while(1)
    {
        tick_ms = MsOS_GetSystemTime() ;
        if ((tick_ms % 1000) == 0)
        {
             SYSINIT_PRINT("tick_ms=%d\n", tick_ms);
        }

//        if (tick_ms >= 10000)
//            break;
    }
}

#if 1
//extern int printf(const char *fmt, ...) __attribute__ ((far));
extern int puts(const char *s) __attribute__ ((far));

#define MEM32(add) *((volatile unsigned long *)(add))
#if (CHIP_ID == CHIP_MT9701)
void SysInit_ResetSPIBase(void) __attribute__ ((section(".IQMEM"))) __attribute__((far)) ;

#define R2_DQMEM_TEST_BASE 0xB0000000  //
#else
//__attribute__ ((far)) void FuncOnIMI(void) __attribute__ ((section(".IMI")));
void FuncOnIMI1(void) __attribute__ ((section(".IMI"))) __attribute__((far)) ;

void FuncOnIMI2(void) __attribute__ ((section(".IMI"))) __attribute__((far)) ;

void SysInit_ResetSPIBase(void) __attribute__ ((section(".IMI"))) __attribute__((far)) ;

#define R2_DQMEM_TEST_BASE 0xB0008000  //
#if 0 // test code
void FuncOnIMI1(void)
{
//    volatile int i;

    puts("\nFuncOnIMI_1 is running on IMI..");
    printf("address 0x%x=0x%x\n\n", R2_DQMEM_TEST_BASE, MEM32(R2_DQMEM_TEST_BASE));
#if 0
    puts("\n====================================================\n");
    for(i=0; i<0x80; i+=4)
    {
        MEM32(R2_DQMEM_TEST_BASE+i) = MEM32(0x00000100+i);
    }

    for(i=0; i<0x80; )
    {
        if ((i % 0x10) == 0)
            printf("\naddr=0x%08x ", R2_DQMEM_TEST_BASE+i);
        printf(" %08x", MEM32(R2_DQMEM_TEST_BASE+i));
        i += 4;
    }
    puts("\n====================================================\n");
#endif
}


void FuncOnIMI2(void)
{
    volatile int i;

    puts("\n====================================================\n");
    puts("\nFuncOnIMI2 is running on IMI..\n");

    for(i=0; i<0x80; i+=4)
    {
        MEM32(R2_DQMEM_TEST_BASE+i) = MEM32(0x00000100+i);
    }

    for(i=0; i<0x80; )
    {
        if ((i % 0x10) == 0)
            printf("\naddr=0x%08x ", R2_DQMEM_TEST_BASE+i);
        printf(" %08x", MEM32(R2_DQMEM_TEST_BASE+i));
        i += 4;
    }
    puts("\n====================================================\n");
}
#endif

#endif
#endif

static inline void local_irq_disable(void)
{
	unsigned long sr;
	asm volatile("l.mfspr	%0,r0,%1" : "=r"(sr) : "i"(SPR_SR));
	sr &= ~(SPR_SR_TEE | SPR_SR_IEE);
	asm volatile("l.mtspr	r0,%0,%1" :: "r"(sr), "i"(SPR_SR));
}

static void JumpToResetVector(U32 u32Addr)
{
#if 0
    //set up reset vector base
    MDrv_Write2Byte(0x28B4, (U16)(u32Addr >> 16));
    //R2 boot from spi
    MDrv_Write2Byte(0x2880, 0x001F);
#endif

    __asm__ __volatile__ (
        "\tl.syncwritebuffer\n"
        "\tl.jr     %0\n"
        "\tl.syncwritebuffer\n"
        : : "r" (u32Addr));
}

void SysInit_ResetSPIBase(void)
{
/*
PM_R2第1組RIU bank在0x0028
PM_R2第2組RIU bank在0x002d

RIU1
h5d	h5d	5	5	reg_r2_stalled
h5d	h5d	6	6	reg_r2_reseted
h5d	h5d	7	7	reg_has_rstctrl
h5d	h5d	8	8	reg_safe_reset
h5d	h5d	9	9	reg_safe_reboot

RIU2
h1c	h1c	15	10	reg_spi_offset_lo
h1d	h1d	15	0	reg_spi_offset_hi
h1e	h1e	15	10	reg_io2_offset_lo
h1f	h1f	15	0	reg_io2_offset_hi


*/
#define REG_R2_RIU1_BASE  0x2800
#define REG_R2_RIU2_BASE  0x2D00

#define REG_SPI_OFFSET_LO_ADDR (REG_R2_RIU2_BASE+0x38)
#define REG_SPI_OFFSET_HI_ADDR (REG_R2_RIU2_BASE+0x3A)
#define REG_IO2_OFFSET_LO_ADDR (REG_R2_RIU2_BASE+0x3C)
#define REG_IO2_OFFSET_HI_ADDR (REG_R2_RIU2_BASE+0x3E)

#define REG_R2_RESET_CTRL (REG_R2_RIU1_BASE+0x5D*2)
#define R2_HAS_RSTCTRL_BIT  BIT7
#define R2_SAFE_REBOOT_BIT    BIT9

#define NEW_SPI_BASE 0x6000
#define RESET_BY_SAFE_REBOOT  1

#if ENABLE_USB_HOST_DUAL_IMAGE_SIGN
    U32 u32SizeImg;
    u32SizeImg = ((*(U32 *)((U32)u32NewSPIBase+ENABLE_USB_HOST_DUAL_IMAGE_SBOOT+0x20))+0x08);
    mbedtls_rsa_verify_content_size = u32SizeImg;
    mbedtls_rsa_verify_content_ptr = (unsigned char*)u32NewSPIBase;
    mbedtls_rsa_verify_sig_ptr = (unsigned char*)(u32NewSPIBase+u32SizeImg);
    mbedtls_rsa_verify_key_ptr = (unsigned char*)(mbedtls_rsa_verify_sig_ptr + 0x100);

    if(mbedtls_rsa_verify())
    {
        SYSINIT_PRINT("RSA Signature Fail on SPI Base 0x%x\r\n", u32NewSPIBase);
        if( u32NewSPIBase == ENABLE_USB_HOST_DUAL_IMAGE_OFFSET )
            u32NewSPIBase = 0;
        else
            u32NewSPIBase = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;

        SYSINIT_PRINT("Change SPI Base to 0x%x\r\n", u32NewSPIBase);
        u32SizeImg = ((*(U32 *)((U32)u32NewSPIBase+ENABLE_USB_HOST_DUAL_IMAGE_SBOOT+0x20))+0x08);
        mbedtls_rsa_verify_content_size = u32SizeImg;
        mbedtls_rsa_verify_content_ptr = (unsigned char*)u32NewSPIBase;
        mbedtls_rsa_verify_sig_ptr = (unsigned char*)(u32NewSPIBase+u32SizeImg);
        mbedtls_rsa_verify_key_ptr = (unsigned char*)(mbedtls_rsa_verify_sig_ptr + 0x100);
        if(mbedtls_rsa_verify())
        {
            SYSINIT_PRINT("RSA Signature Fail on SPI Base 0x%x\r\n", u32NewSPIBase);
            while(1);
        }
        else
        {
            SYSINIT_PRINT("RSA Signature Pass on SPI Base 0x%x\r\n", u32NewSPIBase);
        }
    }
    else
    {
        SYSINIT_PRINT("RSA Signature Pass on SPI Base 0x%x\r\n", u32NewSPIBase);
    }
#endif

    //disable ICache
    mtspr(SPR_SR, mfspr(SPR_SR) & ~SPR_SR_ICE);

    U32 u32SPI_BASE1  = 0;
    U32 u32SPI_BASE2  = 0;
    U32 u32Retry = 10;
    BOOL bReturn = 0;

    u32NewSPIBase += ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;

    SYSINIT_PRINT("REG_R2_RESET_CTRL=0x%x\n", MDrv_Read2Byte(REG_R2_RESET_CTRL));
    SYSINIT_PRINT("SysInit_ResetSPIBase\n");
    SYSINIT_PRINT("u32NewSPIBase=0x%x\n", u32NewSPIBase);

    MDrv_Write2Byte(REG_SPI_OFFSET_LO_ADDR, u32NewSPIBase&0xFFFF);
    MDrv_Write2Byte(REG_SPI_OFFSET_HI_ADDR, (u32NewSPIBase>>16)&0xFFFF);
    MDrv_Write2Byte(REG_IO2_OFFSET_LO_ADDR, u32NewSPIBase&0xFFFF);
    MDrv_Write2Byte(REG_IO2_OFFSET_HI_ADDR, (u32NewSPIBase>>16)&0xFFFF);


    while(u32Retry--)
    {
        u32SPI_BASE1 = (MDrv_Read2Byte(REG_SPI_OFFSET_LO_ADDR)&0xFC00) | MDrv_Read2Byte(REG_SPI_OFFSET_HI_ADDR)<<16 ;
        u32SPI_BASE2 = (MDrv_Read2Byte(REG_IO2_OFFSET_LO_ADDR)&0xFC00) | MDrv_Read2Byte(REG_IO2_OFFSET_HI_ADDR)<<16 ;
        if ((u32SPI_BASE1 == u32NewSPIBase) && (u32SPI_BASE2 == u32NewSPIBase))
        {
            bReturn = 1;
            break;
        }
    }

    //printf("SysInit_ResetSPIBase bReturn=%d\n", bReturn);

    //if (bReturn)
    {
#if 0//RESET_BY_SAFE_REBOOT
    //printf("REG_R2_RESET_CTRL=0x%x\n", MDrv_Read2Byte(REG_R2_RESET_CTRL));
    //if (MDrv_Read2Byte(REG_R2_RESET_CTRL) & R2_HAS_RSTCTRL_BIT)
    {
        MDrv_WriteWordRegBit(REG_R2_RESET_CTRL, TRUE, R2_SAFE_REBOOT_BIT);
    }
#else
        //Jump to Reset Vector directly
        JumpToResetVector(0x100);
#endif
    }

    // it should never return
    while(1);
}

#if (CHIP_ID == CHIP_MT9701)
#define REG_R2_IO1_BASE 0x28AA
#define R2_IQMEM_BASE 0xFFC00000
extern unsigned char _IQMEM_lma[];
extern unsigned char _IQMEM_start[];
extern unsigned char _IQMEM_end[];

void SysInit_CopyIQbyR2(void)
{
    U8 *spt = _IQMEM_lma; // source
    volatile U8 *dpt = _IQMEM_start; // dest
    volatile U8 *end = _IQMEM_end; // dest

    MDrv_WriteByteMask(0x28B0,BIT4,BIT4);

    //Access IQMEM from D-side IO1 = access (IO1_BASE + 0x200000)
    dpt = dpt - R2_IQMEM_BASE + ((MDrv_Read2Byte(REG_R2_IO1_BASE)&0xFFFF)<<16) + 0x200000;
    end = end - R2_IQMEM_BASE + ((MDrv_Read2Byte(REG_R2_IO1_BASE)&0xFFFF)<<16) + 0x200000;
    SYSINIT_PRINT("src=0x%x, dst=0x%x, end=0x%x\n ", spt, dpt, end);

    MDrv_WriteByteMask(0x28C8,BIT4,BIT4); // imem_sel[4] 1 DMA, 0 ICU
    MDrv_WriteByteMask(0x28D8,BIT5,BIT5); // reg_iqm_owner_dio

    //move data from flash to IQMEM
    while(dpt<end)
    {
        *dpt++ = *spt++;
    }

    MDrv_WriteByteMask(0x28C8,0,BIT4); // imem_sel[4] 1 DMA, 0 ICU
    MDrv_WriteByteMask(0x28D8,0,BIT5); // reg_iqm_owner_dio

}

void Init_IMI(void)
{
    MDrv_Write2Byte(0x2902, 0x8400); // set IO space of IMI from 0x40000000~
}
#else
extern unsigned char _IMI_lma[];
extern unsigned char _IMI_start[];
extern unsigned char _IMI_end[];

void Init_IMI(void)
{
    U8 *spt = _IMI_lma; // source
    volatile U8 *dpt = _IMI_start; // dest
//    *((U32 *)__data_start_d) = 0xaabbccdd;

    SYSINIT_PRINT("_IMI_lma=0x%x, _MI_start=0x%x, _IMI_end=0x%x\n ", _IMI_lma, _IMI_start, _IMI_end);

    *dpt = 0xAA;
    *(dpt+1) = 0xBB;
    SYSINIT_PRINT("IMI0(0x%x)=0x%x, IMI1(0x%x)=0x%x", dpt, *dpt, dpt+1, *(dpt+1));
    SYSINIT_PRINT("Init IMI 2\n");

#if 1//def TSUMR2_FPGA
    while(dpt<_IMI_end)
    {
        *dpt++ = *spt++;
    }
#endif
}
#endif
BYTE FlashdwAddrReadByte_boot( DWORD dwAddr )
{
    return ((unsigned char  volatile *) (0x00000000ul))[dwAddr];
}

BYTE MonthMapping(BYTE* buffer)
{
    switch (*buffer)
    {
        case 'D': return 12; break;
        case 'N': return 11; break;
        case 'O': return 10; break;
        case 'S': return 9; break;
        case 'F': return 2; break;
        case 'A':
            if(*(buffer+1) == 'p')
                return 4;
            else
                return 8;
        break;
        case 'M':
            if(*(buffer+2) == 'r')
                return 3;
            else
                return 5;
        break;
        case 'J':
            if(*(buffer+1) == 'a')
                return 1;
            else if(*(buffer+2) == 'n')
                return 6;
            else
                return 7;
        break;
        default: return 0; break;
    }
}


#define ASCII2NUM(Value)    ((Value <= 0x30) ? 0 : (Value - 0x30) )

BYTE Compare_Img(void)
{
    BYTE DateCode[2][20];
    DWORD i;

#if ENABLE_COMPILE_INFO
    DWORD Tmp1, Tmp2;
#endif

    for(i=0; i<20; i++)
    {
        DateCode[0][i] = FlashdwAddrReadByte_boot(ENABLE_USB_HOST_DUAL_IMAGE_SBOOT+0x1200+i);
        DateCode[1][i] = FlashdwAddrReadByte_boot(ENABLE_USB_HOST_DUAL_IMAGE_OFFSET+ENABLE_USB_HOST_DUAL_IMAGE_SBOOT+0x1200+i);
    }

    if(DateCode[1][0] == 0xFF || DateCode[1][19] == 0xFF)
    {
        SYSINIT_PRINT("Img2 invalid!\r\n");
        return 0;
    }

#if ENABLE_COMPILE_INFO

    Tmp1 = (DateCode[0][9]<<8) | DateCode[0][10];
    Tmp2 = (DateCode[1][9]<<8) | DateCode[1][10];

//year
    if(Tmp1 > Tmp2)
        return 0;
    else if (Tmp2 > Tmp1)
        return 1;

    Tmp1 = MonthMapping(DateCode[0]);
    Tmp2 = MonthMapping(DateCode[1]);
    SYSINIT_PRINT("M1:%d\r\n", Tmp1);
    SYSINIT_PRINT("M2:%d\r\n", Tmp2);

//month
    if(Tmp1 > Tmp2)
        return 0;
    else if (Tmp2 > Tmp1)
        return 1;

    Tmp1 = (ASCII2NUM(DateCode[0][4])*10) + ASCII2NUM(DateCode[0][5]);
    Tmp2 = (ASCII2NUM(DateCode[1][4])*10) + ASCII2NUM(DateCode[1][5]);
    SYSINIT_PRINT("D1:%d\r\n",Tmp1);
    SYSINIT_PRINT("D2:%d\r\n",Tmp2);

//date
    if(Tmp1 > Tmp2)
        return 0;
    else if (Tmp2 > Tmp1)
        return 1;

    Tmp1 = (ASCII2NUM(DateCode[0][12])*10) + ASCII2NUM(DateCode[0][13]);
    Tmp2 = (ASCII2NUM(DateCode[1][12])*10) + ASCII2NUM(DateCode[1][13]);
    SYSINIT_PRINT("H1:%d\r\n",Tmp1);
    SYSINIT_PRINT("H2:%d\r\n",Tmp2);
//hour
    if(Tmp1 > Tmp2)
        return 0;
    else if (Tmp2 > Tmp1)
        return 1;

    Tmp1 = (ASCII2NUM(DateCode[0][15])*10) + ASCII2NUM(DateCode[0][16]);
    Tmp2 = (ASCII2NUM(DateCode[1][15])*10) + ASCII2NUM(DateCode[1][16]);
    SYSINIT_PRINT("m1:%d\r\n",Tmp1);
    SYSINIT_PRINT("m2:%d\r\n",Tmp2);
 //minute
    if(Tmp1 > Tmp2)
        return 0;
    else if (Tmp2 > Tmp1)
        return 1;

    Tmp1 = (ASCII2NUM(DateCode[0][18])*10) + ASCII2NUM(DateCode[0][19]);
    Tmp2 = (ASCII2NUM(DateCode[1][18])*10) + ASCII2NUM(DateCode[1][19]);
    SYSINIT_PRINT("s1:%d\r\n",Tmp1);
    SYSINIT_PRINT("s2:%d\r\n",Tmp2);
 //second
    if(Tmp1 > Tmp2)
        return 0;
    else if (Tmp2 > Tmp1)
        return 1;

#else

    for(i=0; i<6; i++)
    {
        if(DateCode[0][i] > DateCode[1][i])
            return 0;
        else if(DateCode[1][i] > DateCode[0][i])
            return 1;
    }

    for(i=0; i<6; i++)
    {
        if(DateCode[0][i+12] > DateCode[1][i+12])
            return 0;
        else if(DateCode[1][i+12] > DateCode[0][i+12])
            return 1;
    }

#endif

    return 0;
}

DWORD BDMA_OperationCRC(BYTE ch, BYTE SourceId, BYTE DestinId, DWORD dwSourceAddr, DWORD dwDestinAddr, DWORD dwByteCount, DWORD dwPattern, DWORD dwExternPattern)
{
    BYTE uctemp, i;

    if(ch == CHANNEL_AUTO)
    {
        if(!(MDrv_ReadByte(REG_100902)&_BIT1))
            uctemp = 0; // channel 0
        else if(!(MDrv_ReadByte(REG_100902+0x20)&_BIT1))
            uctemp = 0x20; // channel 1
        else
    	{
            SYSINIT_PRINT("Both Ch0 and Ch1 are busy\r\n");
            return 0; // both busy
        }
    }
    else if(ch == CHANNEL_0)
    {
        if(MDrv_ReadByte(REG_100902)&_BIT1)
	    {
            SYSINIT_PRINT("Ch0 is busy\r\n");
            return 0;
	    }
        uctemp = 0;
    }
    else
    {
        if(MDrv_ReadByte(REG_100902+0x20)&_BIT1)
	    {
            SYSINIT_PRINT("Ch1 is busy\r\n");
            return 0;
        }
        uctemp = 0x20;
    }

    switch(SourceId&0x0F)
    {
        case SOURCE_MIU0:
        case SOURCE_MIU1:
        case SOURCE_SPI:
        	SourceId |= DW_16BYTE;
        	break;
#if (CHIP_ID == CHIP_MT9701)
        case SOURCE_HKR2_IQM:
        case SOURCE_HKR2_DQM:
        case SOURCE_SECR2_IQM:
        case SOURCE_SECR2_DQM:
            SourceId |= DW_4BYTE;
            break;
#else
        case SOURCE_R2_DQM:
        case SOURCE_R2_IQM:
        	SourceId |= DW_4BYTE;
        	break;
#endif
        default:
        	break;
    }

    switch(DestinId&0x0F)
    {
        case DEST_MIU0:
        case DEST_MIU1:
        	DestinId |= DW_16BYTE;
        	break;
#if (CHIP_ID == CHIP_MT9701)
        case DEST_HKR2_IQMEM:
        case DEST_HKR2_DQMEM:
#endif
        case DEST_SECR2_DQMEM:
        case DEST_SECR2_IQMEM:
        	DestinId |= DW_4BYTE;
        	break;
        case DEST_PD51_PSRAM:
        case DEST_PATTERN_SEARCH:
        case DEST_CRC32:
#if (CHIP_ID == CHIP_MT9700)
        case DEST_HK51_PSRAM:
        case DEST_VDMCU:
#endif
        	DestinId |= DW_1BYTE;
        	break;
        default:
        	break;
    }

    MDrv_WriteByte(REG_100904+uctemp, SourceId);
    MDrv_WriteByte(REG_100905+uctemp, DestinId);

    MDrv_WriteByteMask(REG_100902+uctemp, _BIT4|_BIT3, _BIT4|_BIT3); // clear  bdma_done and pattern search result flag
    MDrv_WriteByteMask(REG_100900, _BIT4, _BIT4);
    MDrv_SysDelayUs(1000);
    MDrv_WriteByteMask(REG_100900, 0x00, _BIT4);

    MDrv_Write4Byte(REG_100908+uctemp, dwSourceAddr);
    MDrv_Write4Byte(REG_10090C+uctemp, dwDestinAddr);
    MDrv_Write4Byte(REG_100910+uctemp, dwByteCount);
    MDrv_Write4Byte(REG_100914+uctemp, dwPattern);
    MDrv_Write4Byte(REG_100918+uctemp, dwExternPattern);

    MDrv_WriteByteMask(REG_100900+uctemp, 1, _BIT0);    //trigger

    if((DestinId&0x0F) == DEST_CRC32)
    {
        i = 0;
        while((!(MDrv_ReadByte(REG_100902+uctemp)&_BIT3)))
        {
            MDrv_SysDelayUs(10000);
            if( (++i) > 200 )
                break;
        }

        dwDestinAddr = MDrv_Read4Byte(REG_100918+uctemp);
        //SYSINIT_PRINT("BDMA CRC:%x\r\n", dwDestinAddr);

        return dwDestinAddr;
    }

    return 0;
}

void SysInit_ImageCheck(void)
{
    U32 u32SizeImg, u32Tmp;

    if(!Compare_Img())
        u32NewSPIBase = 0;
    else
        u32NewSPIBase = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;

    // two crc are appended, first crc excludes sboot, 2nd crc includes sboot
    u32SizeImg = ((*(U32 *)((U32)u32NewSPIBase+ENABLE_USB_HOST_DUAL_IMAGE_SBOOT+0x20))+0x08);

    // skip sboot checksum checking
    if( u32NewSPIBase == 0 )
    {
        u32SizeImg -= (ENABLE_USB_HOST_DUAL_IMAGE_SBOOT+0x04);  // skip sboot + crc of MERGE.bin
        u32Tmp = ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;
    }
    else
    {
        u32Tmp = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;
    }

#if !ENABLE_USB_HOST_DUAL_IMAGE_SIGN
    if( !BDMA_OperationCRC(CHANNEL_AUTO, SOURCE_SPI, DEST_CRC32, u32Tmp, u32Tmp, u32SizeImg, BDMA_CRC32_POLY, BDMA_CRC_SEED_0) )
    {
        SYSINIT_PRINT("CRC OK, Base : %x\r\n", u32NewSPIBase);
    }
    else
    {
        SYSINIT_PRINT("CRC Error, Base : %x\r\n", u32NewSPIBase);

        if( u32NewSPIBase == ENABLE_USB_HOST_DUAL_IMAGE_OFFSET )
            u32NewSPIBase = 0;
        else
            u32NewSPIBase = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;
    }
#endif
}

void start(void)
{
    extern U32 __heap;
    extern U32 __heap_end;
    extern U8  _readonly_start[];
    extern U8  _readonly_end[];

    extern int  main(void);
    //extern void MDrv_Pad_Init(void);
    //extern void MDrv_ISR_Init(void);
    //extern void mhal_stack_init(void *stack);
    //extern void MDrv_Timer_ISR_Register(void);

    char *stack=0;

    RIU_MAP = 0xA0000000;

#if ENABLE_WATCH_DOG
    MDrv_WriteRegBit(0x2C00, TRUE, _BIT0); // WDT clear
    MDrv_Write4Byte(0x2C08, WATCH_DOG_TIME_RESET * CRYSTAL_CLOCK); // need set from H-byte to L-byte if default is not maximum
#else
    //(1) disable watch dog
    MDrv_Write2Byte(0x2C08/*0x3008*/, 0x0000);
    MDrv_Write2Byte(0x2C0A/*0x300A*/, 0x0000);
#endif
    //(2) boot initialization
    Init_Data(); // init .data which are placed in DQMEM but initial data in flash

    InitialSystemPowerUp();

    SysInit_UART_Test1();

#if 0
    // Setup stack
    stack = (char *)malloc(STACK_SIZE);
    if (stack)
    {
        stack += STACK_SIZE;
        mhal_stack_init(stack);
    }
    else
    {
        ASSERT(0);
    }
#endif

    U32 u32Tmp;
    MDrv_MMIO_GetBASE((MS_U32 *) &RIU_MAP, &u32Tmp, MS_MODULE_PM);

#ifdef TSUMR2_FPGA0
    print_hex(RIU_MAP);
    //while(1);
#endif

    MDrv_COPRO_GetBase();

#if ENABLE_MSTV_UART_DEBUG
    console_init();
#endif

    SYSINIT_PRINT("Hello from SysInit_boot : (RIU_MAP=%lx), (STACK:%lx)\n",RIU_MAP, ((DWORD)stack));

    SysInit_UART_Test2();

#if 0
    Init_IMI();
    FuncOnIMI1();
#else
    Init_IMI();
#if (CHIP_ID == CHIP_MT9701)
    SysInit_CopyIQbyR2();
#endif
    SysInit_ImageCheck();

    local_irq_disable();

    SysInit_ResetSPIBase();
    while(1);
#endif

    //printf("stack_top=0x%x, stack_size=0x%x\n", stack, STACK_SIZE);
    //printf("read only: u32Start=0x%x, u32End=0x%x\n", u32Start, u32End);
    //printf("heap => %x, => %x \n",&__heap, &__heap_end);
    if ((&__heap_end) <= (&__heap))
    {
        SYSINIT_PRINT("## ERROR!! MMAP lyout for CPU code is not enough!!!\n");
    }
    else if (((U32)&__heap_end - (U32)&__heap) < 0x4B000)
    {
        U32 u32FreeSize;
        u32FreeSize = (U32)&__heap_end - (U32)&__heap;
        SYSINIT_PRINT("## heap size is %dK under 300K!!\n", (u32FreeSize>>10));
    }

    if (((U32)_readonly_start & 0xFFF) || ((U32)_readonly_end & 0xFFF))
    {
        SYSINIT_PRINT("\r\n## ERROR!! ReadOnly area is not alignment 4K\n");
    }

    //MDrv_IIC_Init();

    //if ( ENABLE_POWER_GOOD_DETECT )
    //{
    //    MDrv_WriteRegBit(0x1EE4, ENABLE, 0x40);
    //    MDrv_WriteRegBit(0x1EE4, ENABLE, 0x1F);
    //}

#if 0
    if (DOWNLOAD_RUN_SERVER)
    {
        //mhal_interrupt_disable();
        MsOS_CPU_DisableInterrupt();
        putchar( 'F' );
        putchar( 'W' );
        while (1)
        {
            MDrv_Sys_ClearWatchDog();
        }
    }
    else
    {
        if (ENABLE_WATCH_DOG == ENABLE)
        {
            MDrv_Sys_ClearWatchDog();
        }
        else
        {
            MDrv_Sys_DisableWatchDog();
        }
    }
#endif
    {
        extern void ProcessSysTrap( MHAL_SavedRegisters *pHalReg, U32 vector );
        MsOS_CPU_AttachException( E_EXCEPTION_TRAP, ProcessSysTrap, E_EXCEPTION_TRAP );

        MDrv_MMIO_Init(); // I/O remap
        MsOS_Init();
//        MDrv_ISR_Init();
    }
//    MDrv_Timer_ISR_Register();
    MDrv_Timer_Init();
    MsOS_CPU_EnableInterrupt();
#ifdef __aeon__
    MsOS_CPU_UnMaskInterrupt(E_INTERRUPT_FIQ); //unmask FIQ
    MsOS_CPU_UnMaskInterrupt(E_INTERRUPT_IRQ); //unmask IRQ
#endif

#if 1//#ifdef TSUMR2_FPGA
    SysInit_TICK_Test();
    while(1);
#endif
    // jump to main() should never return
    main();

    while(1);
}


/******************************************************************************/
//
//   128T +------+          +------+ 128T
//     |  |      |          |      |  |
//     +--+ 8051 |          | Aeon +--+
//        |      |          |      |
//        +--+---+          +---+--+
//       +---+----+        +----+---+
//       |        |        |        |
//   boot_aeon   sw_rst   sw_rst  boot_8051
//
// When boot from 8051
//      reboot:   use reg_reset_cpu0 with password
//      run aeon: set reg_sw_reset_cpu1 to 1
// When boot from Aeon
//      reboot:   use reg_reset_cpu1 with password
//      run 8051: set reg_sw_reset_cpu0 to 1
/******************************************************************************/
void MDrv_Sys_WholeChipReset( void )
{
  #if 1
    MDrv_WriteByte( RIUBASE_PM_MISC+0x5C, 0xFF );
    MDrv_Write4Byte(0xEA0, 0x51685168);
    MDrv_WriteRegBit(RIUBASE_PM_MISC+0x52, 1, BIT7);
    MDrv_WriteRegBit(RIUBASE_PM_MISC+0x52, 1, BIT6);
    MDrv_WriteByte( RIUBASE_PM_MISC+0x5C, 0x79 );
  #else
    MDrv_WDT_SetTimer(0, 1);
  #endif

    while(1);
}

/******************************************************************************/
///Reset 8051 to reboot
/******************************************************************************/
void MDrv_Sys_Reboot( void )
{
  #if 1
    MDrv_Write2Byte( REG_RESET_CPU_8051, 0x029F );
    MDrv_Write2Byte( REG_RESET_CPU_8051, 0x829F ); // can't ignore second 0x9F
  #else
    MDrv_WDT_SetTimer(0, 1);
  #endif
    while(1);
}


void MDrv_Sys_SetWatchDogTimer( U8 sec )
{
     if( sec == 0 )
     {
        u8OrigWdtTimer = 1;
     }
     else
     {
        u8OrigWdtTimer = sec;
     }
}

void  MDrv_Sys_EnableWatchDog(void)
{
#if (ENABLE_WATCH_DOG == ENABLE )
    MDrv_Write4Byte(WDT_TIMER_0, ( MDrv_Sys_WatchDogCycles( u8OrigWdtTimer ) ));
#endif
}

//----------------------------------------------------------------
void MDrv_Sys_DisableWatchDog( void )
{
    MDrv_Write4Byte( WDT_TIMER_0, 0x0000UL );
}

