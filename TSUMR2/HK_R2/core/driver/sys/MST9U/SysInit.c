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
///@file drvsys.h
///@brief System functions: Initialize, interrupt
///@author MStarSemi Inc.
///
///////////////////////////////////////////////////////////////////////////////

#define  _DRV_SYS_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "hwreg.h"
#include "MsTypes.h"
#include "drvUartDebug.h"
#include "Debug.h"
#include "drvUART.h"
#include "drvMMIO.h"
#include "drvBDMA.h"
#include "SysInit.h"
#include "drvGlobal.h"
#include "drvISR.h"
#include "drvCPU.h"
#include "imginfo.h"
#include "BinInfo.h"
//#include "sysinfo.h"
//#include "drvGPIO.h"
//#include "drvIIC.h"
//#include "MsDevice.h"
//#include "drvMIU.h"
//#include "drvWDT.h"
//#include "drvPM.h"
//#include "msIR.h"
//#include "Panel.h"
//#include "MApp_SaveData.h"
//#include "util_symbol.h"
#include "HdcpMbx.h"
#include "HdcpIMI.h"
#include "HdcpHandler.h"

#if defined(__aeon__)
#include "risc32_spr.h"
#endif

#if AES_DECRYPTION
#include "aesdecrypt.h"
#include "./mbedtls_2_3_0/include/mbedtls/platform.h"
#endif

/******************************************************************************/
/*                     Temporary Defines                                                                          */
/******************************************************************************/
//#undef CHIP_ID
//#define CHIP_ID         MST9U4 //Leo-temp for InitialSystemPowerUp()

#define ENABLE_PRINT_MSG        1


#ifdef TSUMR2_FPGA
#define R2_CLK       (60000000)//(71590000) //(71400000) // (60000000)
U32 g_FPGA_CPU_CLOCK = 60000000;//71590000; //(71400000) //60000000;
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
#define SYSINIT_PRINT(format, ...)     printf(format, ##__VA_ARGS__)
#else
#define SYSINIT_PRINT(format, ...)
#endif

#if ENABLE_COMPILE_INFO
MS_CMP_Info code g_cmpInfo __attribute__((__section__ (".cmp_img_info"))) =
{
	__DATE__,
	__TIME__
};
#else
#define _FW_DATE_STR_ "220524"  //Year_Month_Date
#define _FW_TIME_STR_ "100947"  //Hour_Minute_Second

MS_CMP_Info code g_cmpInfo __attribute__((__section__ (".cmp_img_info"))) =
{
    _FW_DATE_STR_,
    _FW_TIME_STR_
};
#endif

/******************************************************************************/
/*                     Global Variables                                       */
/******************************************************************************/

// memory map for pre-built libraries
//U32 u32SecbufAddr = ((SECBUF_START_MEMORY_TYPE & MIU1) ? (SECBUF_START_ADR | MIU_INTERVAL) : (SECBUF_START_ADR));
//U32 u32SecbufSize = SECBUF_START_LEN;

U32 u32CRCbufAddr = NULL;

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
void MDrv_SysInitUARTPIU_CLK(void)
{
#include "drvUARTPIU.h"

    UART_CLK_PIU e_piu_clock = CLK_PIU_172M;

    MDrv_WriteByte(REG_00038C, (e_piu_clock << 2));
}

static void console_init(void)
{
#ifdef TSUMR2_FPGA
    MDrv_UART_Init(DEFAULT_UART_DEV, 38400 /*38400, 115200*/);
    mdrv_uart_connect(E_UART_PORT_GPIO40_41/*DEFAULT_UART_PORT*/,DEFAULT_UART_DEV);
#else
    if (DEFAULT_UART_DEV == E_UART_PIU_UART0)
        MDrv_SysInitUARTPIU_CLK();

    //before querying CPU_clock, need to set base address for driver
    MDrv_UART_Init(DEFAULT_UART_DEV, SERIAL_BAUD_RATE);
    mdrv_uart_connect(DEFAULT_UART_PORT,DEFAULT_UART_DEV);
#endif

    SYSINIT_PRINT("\nDefault port(0x%x) connecting to 0x%x\n", DEFAULT_UART_PORT, mdrv_uart_get_connection(DEFAULT_UART_PORT));
#if ENABLE_COMPILE_INFO
    SYSINIT_PRINT("\n=== console_init ok (%s %s) ===\n\n", __TIME__, __DATE__);
#else
    SYSINIT_PRINT("\n=== console_init ok (%s %s) ===\n\n", _FW_DATE_STR_, _FW_TIME_STR_);
#endif
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
    msWriteByte(REG_101E65, 0x00);
    msWriteByte(REG_101E66, 0xA1);

    //set SYNTHESIZER's frequency
    u32SynthSet = ((float)u32MPLL_MHZ * 524288) / SynthCLK;
    msWrite2ByteMask(REG_101E50, (u32SynthSet&0xFFFF), 0xFFFF);  //reg_synth_set
    msWriteByteMask(REG_101E52, (u32SynthSet>>16)&0xFF, 0xFF);   //reg_synth_set
    msWriteByteMask(REG_101E60, BIT0, BIT0);                     //reg_synth_sld
    msWriteByteMask(REG_101E69, u8LoopDivSecond, 0xFF);          //reg_syninpll_loopdiv_second
    msWriteByteMask(REG_101E68, u8OutputDiv, 0x07);              //reg_syninpll_output_div
    msWriteByteMask(REG_101E66, 0, BIT4);                        //reg_syninpll_pd
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

//-----------------------------------------------------------------
// SYNINPLL setting
//-----------------------------------------------------------------
#if (CHIP_ID == CHIP_MT9701)
    MDrv_SyninpllR2Init();
#endif

#if 0// (CHIP_ID==MST9U4)
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
    MDrv_WriteByte(0x120F30, 0x00);   // reg_ckg_sosd_ft;

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

#endif //#ifdef TSUMR2_FPGA

    return 1;
}

void start(void)
{
    extern U32 __heap;
    extern U32 __heap_end;
    extern U32 _bss_end;
    extern U8  _readonly_start[];
    extern U8  _readonly_end[];

    extern int  main(void);
    //extern void MDrv_Pad_Init(void);
    extern void MDrv_ISR_Init(void);
    //extern void mhal_stack_init(void *stack);
    extern void MDrv_Timer_ISR_Register(void);

    //char *stack;

    RIU_MAP = 0xA0000000;

    //(2) boot initialization
    Init_Data(); // init .data which are placed in DQMEM but initial data in flash

    InitialSystemPowerUp();

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

#if ENABLE_WATCH_DOG
    Init_WDT( _ENABLE );
#else
    //(1) disable watch dog
    MDrv_Write2Byte(0x2C08/*0x3008*/, 0x0000);
    MDrv_Write2Byte(0x2C0A/*0x300A*/, 0x0000);
#endif

    U32 u32Tmp;
    MDrv_MMIO_GetBASE((MS_U32 *) &RIU_MAP, &u32Tmp, MS_MODULE_PM);

#if (PM_LOCK_SUPPORT == 1)
    MDrv_SEM_Init();
#endif

#if 0 //Leo-temp, supposed to be removed.
    Util_InitSymbolTBL();

//MDrv_Pad_Init();
//mdrv_gpio_init();
#endif


    MDrv_COPRO_GetBase();

#if ENABLE_MSTV_UART_DEBUG
    console_init();
#endif

    SYSINIT_PRINT("Hello from SysInit : (RIU_MAP=%lx)\n",RIU_MAP);

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

    if ((&__heap) <= (&_bss_end))
    {
        SYSINIT_PRINT("## ERROR!! DQMEM size is not enough!!!\n");
    }

    //MDrv_IIC_Init();

    //if ( ENABLE_POWER_GOOD_DETECT )
    //{
    //    MDrv_WriteRegBit(0x1EE4, ENABLE, 0x40);
    //    MDrv_WriteRegBit(0x1EE4, ENABLE, 0x1F);
    //}

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
        printf("DOWNLOAD_RUN_SERVER=0\n");
        if (ENABLE_WATCH_DOG == ENABLE)
        {
            MDrv_Sys_ClearWatchDog();
        }
        else
        {
        printf("ENABLE_WATCH_DOG=0\n");
            MDrv_Sys_DisableWatchDog();
        printf("MDrv_Sys_DisableWatchDog Done\n");
        }
    }

    {
        extern void ProcessSysTrap( MHAL_SavedRegisters *pHalReg, U32 vector );
        MsOS_CPU_AttachException( E_EXCEPTION_TRAP, ProcessSysTrap, E_EXCEPTION_TRAP );

        MDrv_MMIO_Init(); // I/O remap
        MsOS_Init();
        MDrv_ISR_Init();
    }
        printf("MDrv_ISR_Init Done\n");
    MDrv_Timer_ISR_Register();
    MDrv_Timer_Init();
        printf("MDrv_Timer_Init Done\n");
    MsOS_CPU_EnableInterrupt();
        printf("MsOS_CPU_EnableInterrupt Done\n");

#ifdef __aeon__
    MsOS_CPU_UnMaskInterrupt(E_INTERRUPT_FIQ); //unmask FIQ
    MsOS_CPU_UnMaskInterrupt(E_INTERRUPT_IRQ); //unmask IRQ
#endif

    // jump to main() should never return
    main();

    while(1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_Sys_Set_CIDInfo()
/// @brief \b Function \b Description : To Set Customer Info
/// @param <IN>        \b pCIDInfo    : Input the CID Info
/// @param <OUT>       \b None        :
/// @param <GLOBAL>    \b None        :
////////////////////////////////////////////////////////////////////////////////
void MDrv_Sys_Set_CIDInfo(U8 *pCIDInfo)
{
    MDrv_WriteByte(0x1033D0,pCIDInfo[0]);
    MDrv_WriteByte(0x1033D1,pCIDInfo[1]);
    MDrv_WriteByte(0x1033D2,pCIDInfo[2]);
    MDrv_WriteByte(0x1033D3,pCIDInfo[3]);

}

/******************************************************************************/
/// Copy variable from DRAM space to XDATA space
/// @param pAddr \b IN XDATA pointer
/// @param u32MiuAddr \b IN MIU address
/// @param u16Length \b IN size to copy
/******************************************************************************/

void MDrv_Sys_CopyMIU2VAR(U32 srcaddr, void *dstaddr, U16 len)
{
    memcpy(dstaddr, (U8 *)srcaddr, len);
}

// for backward compatible, do not use!
#undef MDrv_MIU_SDRAM2VARCopy
void MDrv_MIU_SDRAM2VARCopy(U32 srcaddr, void *dstaddr, U16 len)
{
    memcpy(dstaddr, (U8 *)srcaddr, len);
}

/******************************************************************************/
/// Copy variable from XDATA space to DRAM space
/// @param pAddr \b IN XDATA pointer
/// @param u32MiuAddr \b IN MIU address
/// @param u16Length \b IN size to copy
/******************************************************************************/

void MDrv_Sys_CopyVAR2MIU(void *srcaddr, U32 dstaddr, U16 len)
{
    memcpy((U8 *)dstaddr, srcaddr, len);
}

// for backward compatible, do not use!
#undef MDrv_MIU_VAR2SDRAMCopy
void MDrv_MIU_VAR2SDRAMCopy(void *srcaddr, U32 dstaddr, U16 len)
{
    memcpy((U8 *)dstaddr, srcaddr, len);
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


typedef enum IQM_Mux_tag
{
    IQMUX_CPU,
    IQMUX_CPUIO1,
    IQMUX_SLAVE,
    IQMUX_DMA
} IQMMUX_t;

#define REG32(add) *((volatile unsigned long *)(add))

#define RIUBASE_SECU_R2             0x100E00 // 0x102700 ~ 0x1027FF
#define R2_RIU_BASE             RIUBASE_SECU_R2
#define REG_IQMEM_CTRL          (R2_RIU_BASE + (0x24+0x40)*2)
#define REG_QMEM_OWNER_SEL	    (R2_RIU_BASE + (0x2C+0x40)*2)


#define DRVBDMA_printData(str, value)   printf(str, value) //; printData(str, value)
#define DRVBDMA_printMsg(str)           printf(str)  //printMsg(str)

DWORD BDMA_Operationxx(BYTE ch, BYTE SourceId, BYTE DestinId, DWORD dwSourceAddr, DWORD dwDestinAddr, DWORD dwByteCount, DWORD dwPattern, DWORD dwExternPattern)
{
    BYTE uctemp;

    printf("BDMA_Operationxx start \n");

    if(ch == CHANNEL_AUTO)
    {
        if(!(MDrv_ReadByte(REG_100902)&_BIT1))
            uctemp = 0; // channel 0
        else if(!(MDrv_ReadByte(REG_100902+0x20)&_BIT1))
            uctemp = 0x20; // channel 1
        else
    	{
            DRVBDMA_printMsg("Both Ch0 and Ch1 are busy");
            return 0; // both busy
        }
    }
    else if(ch == CHANNEL_0)
    {
        if(MDrv_ReadByte(REG_100902)&_BIT1)
	    {
            DRVBDMA_printMsg("Ch0 is busy");
            return 0;
	    }
        uctemp = 0;
    }
    else
    {
        if(MDrv_ReadByte(REG_100902+0x20)&_BIT1)
	    {
            DRVBDMA_printMsg("Ch1 is busy");
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

    ForceDelay1ms_Pure(10);

    MDrv_WriteByteMask(REG_100900, 0x00, _BIT4);

    MDrv_Write4Byte(REG_100908+uctemp, dwSourceAddr);
    MDrv_Write4Byte(REG_10090C+uctemp, dwDestinAddr);
    MDrv_Write4Byte(REG_100910+uctemp, dwByteCount);
    MDrv_WriteByte(REG_100914+uctemp, (BYTE)(dwPattern>>24));
    MDrv_WriteByte(REG_100915+uctemp, (BYTE)(dwPattern>>16));
    MDrv_WriteByte(REG_100916+uctemp, (BYTE)(dwPattern>>8));
    MDrv_WriteByte(REG_100917+uctemp, (BYTE)(dwPattern));

    MDrv_WriteByte(REG_100918+uctemp, (BYTE)(dwExternPattern>>24));
    MDrv_WriteByte(REG_100919+uctemp, (BYTE)(dwExternPattern>>16));
    MDrv_WriteByte(REG_10091A+uctemp, (BYTE)(dwExternPattern>>8));
    MDrv_WriteByte(REG_10091B+uctemp, (BYTE)(dwExternPattern));

    MDrv_WriteByteMask(REG_100900+uctemp, 1, _BIT0);    //trigger

    if((DestinId&0x0F) == DEST_PATTERN_SEARCH) // pattern search
    {
    	SetTimOutConter(10);
        while((bTimeOutCounterFlag) && (!(MDrv_ReadByte(REG_100902+uctemp)&_BIT3)) && (!(MDrv_ReadByte(REG_100902+uctemp)&_BIT4)));

        if(u16TimeOutCounter == 0)
		{
			DRVBDMA_printMsg("TIMEOUT!!");
		}

        if(!(MDrv_ReadByte(REG_100902+uctemp)&_BIT4))
        {
            DRVBDMA_printMsg("Can't find the specific pattern in this range!!!");
            //return 0;
        }

        dwDestinAddr = 0;
        dwDestinAddr = (DWORD)MDrv_ReadByte(REG_100908+uctemp);
        dwDestinAddr |= ((DWORD)MDrv_ReadByte(REG_100909+uctemp))<<8;
        dwDestinAddr |= ((DWORD)MDrv_ReadByte(REG_10090A+uctemp))<<16;
        dwDestinAddr |= ((DWORD)MDrv_ReadByte(REG_10090B+uctemp))<<24;

        DRVBDMA_printData("BDMA_OP_dwDestinAddr_H: 0x%x", dwDestinAddr>>16);
        DRVBDMA_printData("BDMA_OP_dwDestinAddr_L: 0x%x", dwDestinAddr);
        return dwDestinAddr;
    }
    else if((DestinId&0x0F) == DEST_CRC32)
    {
    	SetTimOutConter(10);
        while((bTimeOutCounterFlag) && (!(MDrv_ReadByte(REG_100902+uctemp)&_BIT3)));
        if(u16TimeOutCounter == 0)
		{
			DRVBDMA_printMsg("TIMEOUT!!");
		}
        dwDestinAddr = 0;
        dwDestinAddr = (DWORD)MDrv_ReadByte(REG_100918+uctemp);
        dwDestinAddr |= ((DWORD)MDrv_ReadByte(REG_100919+uctemp))<<8;
        dwDestinAddr |= ((DWORD)MDrv_ReadByte(REG_10091A+uctemp))<<16;
        dwDestinAddr |= ((DWORD)MDrv_ReadByte(REG_10091B+uctemp))<<24;

        DRVBDMA_printData("BDMA_OP_dwDestinAddr_H: 0x%x", dwDestinAddr>>16);
        DRVBDMA_printData("BDMA_OP_dwDestinAddr_L: 0x%x", dwDestinAddr);
        return dwDestinAddr;
    }
    else
    {
        SetTimOutConter(1000);
        while((bTimeOutCounterFlag) && (!(MDrv_ReadByte(REG_100902+uctemp)&_BIT3)));

        if(u16TimeOutCounter == 0)
		{
			DRVBDMA_printMsg("TIMEOUT!!");
		}
    }
#if 0
    MDrv_WriteByte(REG_100902+uctemp, 0xe8);
#endif

    printf("BDMA_Operationxx end \n");

    return 0;
}

void SysInit_WakeUpSecu(U32 u32SrcAddr, U32 u32BinLen)
{
#define REG_R2_RIU2_BASE  0x2D00

#define REG_SPI_OFFSET_LO_ADDR (REG_R2_RIU2_BASE+0x38)
#define REG_SPI_OFFSET_HI_ADDR (REG_R2_RIU2_BASE+0x3A)
//h64	h64	4	4	reg_imem_sel	4	4	1	h0	rw	iqmem_ select: 1 DMA or DCU, 0 ICU
//h64	h64	5	5	reg_imem_boot	5	5	1	h0	rw	imem_ boot: 1 boot from imem
//h6c	h6c	0	0	reg_dqm_owner_sel	0	0	1	h0	rw	sel dqm owner between IMC & 2DMA (x)--not functional in MT9700
//h6c	h6c	1	1	reg_iqm_owner_sel		1	1	1	h0	rw	sel iqm owner between IMC & 2DMA (x)--not functional in MT9700
//h6c	h6c	5	5	reg_iqm_owner_dio		5	5	1	h0	rw	sel iqm owner between IMC & 2DMA or Dside IO1

/*    From drvmStar_Init();     */
/*    mcuDMADownloadCode(SOURCE_SPI, DEST_SECR2_IQMEM, SECUR2_SPI_BIN_START, 0x0, SECUR2_SPI_BIN_SIZE); */

    U32 u32SPI_Offset = 0;
    u32SPI_Offset = (MDrv_Read2Byte(REG_SPI_OFFSET_HI_ADDR)<<16) + MDrv_Read2Byte(REG_SPI_OFFSET_LO_ADDR);

    msWrite2Byte(0x100ed6,0xf000);//stall R2
    msWrite2Byte(0x100e80,0x0007);//sw_rst & r2_en
    msWrite2Byte(0x100ec8,0x00d0);//[5]reg_imem_boot


    //Move data from SPI to DQMEM
    U32 u32SrcLMA_start = *(U32*)(u32SrcAddr-u32SPI_Offset+0x1000);
    U32 u32SrcLMA_end = *(U32*)(u32SrcAddr-u32SPI_Offset+0x1004);
    U32 u32DataLen = u32SrcLMA_end - u32SrcLMA_start;

    u32SrcLMA_start = u32SrcLMA_start + u32SrcAddr;
    BDMA_Operationxx(CHANNEL_AUTO,SOURCE_SPI,DEST_SECR2_DQMEM,u32SrcLMA_start,0,u32DataLen,0,0);

    //Move data from SPI to IQMEM
    BDMA_Operationxx(CHANNEL_AUTO,SOURCE_SPI,DEST_SECR2_IQMEM,u32SrcAddr,0x0,u32BinLen,0,0);
#if 1
    ForceDelay1ms_Pure(10);
#endif

    MDrv_WriteByte(0x100E80, 0x00);
    msWrite2Byte(0x100ec8,0x00e0);//MDrv_WriteByteMask(0x100EC8, 0, BIT4);
    msWrite2Byte(0x100e80,0x0000);
    MDrv_WriteByteMask(0x100ED7, 0, BIT7);



    //MDrv_Write2Byte(0x100ec8, 0x0020);          //[5]reg_imem_boot

    //setting reg_cpu_reset_base
    //MDrv_Write2Byte(RIUBASE_SECU_R2+0xB4, 0xFFC0);

    //MDrv_Write2Byte(0x100e80, 0x000F);          //sw_rst & r2_en
}

#if ENABLE_SECU_R2
U32 MDrv_Sys_SecuRun(MS_U32 u32BootAddr)
{
#define REG_R2_RIU2_BASE  0x2D00

#define REG_SPI_OFFSET_LO_ADDR (REG_R2_RIU2_BASE+0x38)
#define REG_SPI_OFFSET_HI_ADDR (REG_R2_RIU2_BASE+0x3A)

    U32 src_addr, bin_len;
    BININFO r2_binfo;
    r2_binfo.B_ID = 0;
    r2_binfo.B_FAddr = 0;
    r2_binfo.B_IsComp = 0;
    r2_binfo.B_Len = 0;
#define HDCPMBX_START_ADDR DEF_HDCPMBX_BANK
#if (CHIP_ID == CHIP_MT9700)
#define SECR2_CLK_SEL 0x7
#elif (CHIP_ID == CHIP_MT9701)
#define SECR2_CLK_SEL 0x0
#endif
    //0x0=syninpll_r2, 0x1=288, 0x2=216, 0x3=172, 0x4=144,
    //0x5=108, 0x6=86, 0x7=72
    //U32 u32TimeoutCnt = 0x30000000;
    WORD i = 0;
    ULONG MBXBufAddr = 0;

    bin_len = 0;
    u32BootAddr = u32BootAddr;

    r2_binfo.B_ID = BIN_ID_CODE_SECU_R2;
    if (!Get_BinInfo(&r2_binfo))
    {
        SYSINIT_PRINT("Get SECU_ROM_R2 BINFO (B_ID=0x%x) fail..\n", r2_binfo.B_ID);
        return FALSE;
    }

    src_addr = r2_binfo.B_FAddr;
    bin_len  = r2_binfo.B_Len - 8; //+ 256; //wilson: 256 for signature size;

    //printf("MDrv_Sys_SecuRun:\n SECU_R2 ID=0x%x, src_addr=0x%x, bin_len=0x%x\n", BIN_ID_CODE_SECU_R2, src_addr, bin_len);

    msWriteByteMask(REG_120F49, 0x0, BIT3|BIT2|BIT1|BIT0);

    SysInit_WakeUpSecu(src_addr, bin_len);

    //clear mbx
    for ( i = 0; i < 128; i++ )
        MDrv_WriteByte(0x103300 + i, 0x00);

    MBXBufAddr = HDCPHandler_GetTxBufAddr();
    MBXBufAddr = ((MBXBufAddr & 0xFFFF)|(DEF_SECU_IMI_BASE_ADDR));
    MDrv_Write4Byte(HDCPMBX_START_ADDR + 20, MBXBufAddr); //mbx start //IMI: Hk- 0x9000 0000, Secu-0x8000 0000
    MDrv_Write4Byte(HDCPMBX_START_ADDR + 28, 48); //mbx size
    
    #if (CHIP_ID == CHIP_MT9701)
    MDrv_Write4Byte(HDCPMBX_START_ADDR + 32, MIU_FB_ADDR_START); //MIU0 securange start address
    MDrv_Write4Byte(HDCPMBX_START_ADDR + 36, MIU_FB_ADDR_END); //MIU0 securange end address
    #endif

    MDrv_WriteByteMask(0x120F0C, (SECR2_CLK_SEL << 2), BIT4|BIT3|BIT2); // reg_sw_r2_clk

    //MDrv_WriteByte(0x000415, 0x71); // set secu uart

    MsOS_EnableInterrupt(E_INT_PM_FIQ_SECUR2_TO_HKR2);//E_INT_FIQ_HST3TOHST1_INT);

    //MDrv_Write2Byte(0x103378, 0x3697);//authentication flag
#if 0
    MDrv_Write2Byte(0x100D00+0x02, 0x8700);//(DEF_SECU_MAU0_BANK+0x02, 0x8700);      // region 0 mask//from xunru and avoid secure r2 write imi hang on.
#endif
    MDrv_WriteByte(0x100E80, 0x0F); // enable R2
    g_u8SecuR2Alive = 1;

    return TRUE;
}
#endif

#if (ENABLE_PQ_R2)
BOOL MDrv_Sys_LoadPQ(void)
{
#define MEM32(x) (*((unsigned long volatile *)(x)))
#define CLKGEN_REG_BASE                         0x101F00 //clk_gen

#define PQR2_REG_BASE                           0x111800 // pq_r2
#define PQR2_REG_STOP                           (PQR2_REG_BASE+0x0080)
#define PQR2_REG_CLK                            (CLKGEN_REG_BASE+0x000C)
#define PQR2_REG_RST_BASE                       (PQR2_REG_BASE+0x00B4)
#define PQR2_REG_INSN_BASE                      (PQR2_REG_BASE+0x0084)
#define PQR2_REG_DATA_BASE                      (PQR2_REG_BASE+0x0088)
#define PQR2_REG_IO1_BASE                       (PQR2_REG_BASE+0x00AA)
#define PQR2_REG_SPACE_EN                       (PQR2_REG_BASE+0x00B0)
#define PQR2_REG_QMEM_MASK_HIGH                 (PQR2_REG_BASE+0x00A0)
#define PQR2_REG_QMEM_BASE_HIGH                 (PQR2_REG_BASE+0x009C)

#define PQR2_CLK_SEL 0x0  //0:500(syninpll_pqr2), 1:345,  2:288, 3:216, 4:172, 5:144, 6:108, 7:86

    U32 src_addr, bin_len;
    BININFO pqr2_binfo;
    U32 u32BootAddr = 0;
    //U32 u32TimeoutCnt = 0x3000000;
#if AES_DECRYPTION
    U32 u32SizeOut = 0;
#endif

    pqr2_binfo.B_ID = BIN_ID_CODE_PQ_R2;
    if (!Get_BinInfo(&pqr2_binfo))
    {
        printf("Get PQ_R2 BINFO (B_ID=0x%x) fail..\n", pqr2_binfo.B_ID);
        return FALSE;
    }

    src_addr = pqr2_binfo.B_FAddr;
    bin_len  = pqr2_binfo.B_Len;
    //copy flash data to dram for pq_r2
#if 0
    if (!MDrv_BDMA_FlashCopy2Dram( src_addr, u32BootAddr, bin_len ))
    {
        return FALSE;
    }
#else
#if AES_DECRYPTION
    u32BootAddr = (PQ_CODE_ADDR_START + PQ_CODE_SIZE);
#else
    u32BootAddr = PQ_CODE_ADDR_START;
#endif
    printf("CopyFLH2MIU u32BootAddr=0x%x, src_addr=0x%x, bin_len=0x%x\n",
                        u32BootAddr, src_addr, bin_len);
    //CopyFLH2MIU(u32BootAddr, src_addr, bin_len);


    BDMA_Operation(CHANNEL_AUTO, SOURCE_SPI, DEST_MIU0, src_addr, u32BootAddr, bin_len, 0, 0);

    /*u32TimeoutCnt = 0x3000000;
    MDrv_WriteByte(0x100904, 0x45); // 16 byte from SPI
    MDrv_WriteByte(0x100905, 0x40); // 16 byte to MIU0
    MDrv_Write4Byte(0x100908, src_addr);
    MDrv_Write4Byte(0x10090C, u32BootAddr); // dst addr
    MDrv_Write4Byte(0x100910, bin_len); // bin size 64K
    MDrv_WriteByteMask(0x100900, BIT0, BIT0);
    while ( !(MDrv_ReadByte(0x100902)&BIT3) && (--u32TimeoutCnt) )
        ;

    if(!u32TimeoutCnt)
    {
        SYSINIT_PRINT("\n\nPQ_R2 Load Code Timeout!!!\n\n");
        return FALSE;
    }
    else
    {
        SYSINIT_PRINT("\n\nPQ_R2 Load Code (B_ID=0x%x) Done..\n", pqr2_binfo.B_ID);
    }

    MDrv_WriteByteMask(0x100902,BIT3,BIT3); // wirte 1 to clear */
#endif

#if AES_DECRYPTION
#define MIU_BASE                                (0x80000000)
#define CHECKSUM_LENGTH                         (0x8)
#define R2_DQMEM_AES_DECRYPT_BASE               (0xB0006000)
#define R2_DQMEM_AES_DECRYPT_SIZE               (0x4000)

    src_addr = MIU_BASE + u32BootAddr;
    u32BootAddr = MIU_BASE + PQ_CODE_ADDR_START;
    bin_len -= CHECKSUM_LENGTH;
    printf("src_addr=0x%x\n",src_addr);
    printf("PQ_CODE_ADDR_START=0x%x\n",u32BootAddr);
    printf("bin_len=0x%x\n",bin_len);

    MemoryAllocatorInit((unsigned char *)R2_DQMEM_AES_DECRYPT_BASE, R2_DQMEM_AES_DECRYPT_SIZE);
    DoMSDecrypt( (char *)(src_addr), bin_len, (char *)(u32BootAddr), &u32SizeOut);
#endif


    printf("*******************\r\n");
    printf("hk_r2 releases pq_r2..\r\n");
    printf("*******************\r\n");
    MsOS_DelayTask(500);

    // set R2 clock
    //HAL_COPRO_RegWriteByte(PQR2_REG_CLK, 0x24); //320Mhz
    MDrv_WriteByteMask(0x120FE4, BIT6, BIT6); // reg_sw_r2_clk
    MDrv_WriteByteMask(0x120FE4, (PQR2_CLK_SEL << 2), BIT4|BIT3|BIT2); // reg_sw_r2_clk

    MDrv_Write2Byte(PQR2_REG_RST_BASE, u32BootAddr >> 16);  //set up reset vector address 0x0(64K alignment)
    printf("PQR2_REG_RST_BASE=0x%x\n",  MDrv_Read2Byte(PQR2_REG_RST_BASE));
    //HAL_COPRO_RegWrite2Byte(PQR2_REG_RST_BASE, u32BootAddr >> 16);

    //release PQ_R2 reset
    //MDrv_WriteByteMask(0x038B, BIT7, BIT7); //

    //MDrv_WriteByte(0x000415, 0x75); // gpiox40_41 set as pq uart
    //MDrv_WriteByte(0x000414, 0x57); // gpiox50_51 set as pq uart
    MDrv_WriteByte(PQR2_REG_STOP, 0x0); //  disable PQ R2
    MDrv_WriteByte(PQR2_REG_STOP, 0x2F); // sdram_boot and (miu/r2/r2_rst) =1

    return TRUE;
}
BOOL MDrv_Sys_ResetPQ(void)
{
#define PQR2_REG_BASE                           0x111800 // pq_r2
#define PQR2_REG_RESETED                        (PQR2_REG_BASE+0x00BA)
#define PQR2_REG_SAFERST                        (PQR2_REG_BASE+0x00BB)

    WORD u16TimeoutCnt = 0xFFFF;
    //R2 Safe reset
    msWriteByteMask(PQR2_REG_SAFERST, BIT0, BIT0);
    //Polling after reset done
    while((!(msReadByte(PQR2_REG_RESETED) & BIT6)) && (--u16TimeoutCnt));
    if(!u16TimeoutCnt)
    {
        printf("Safe reset PQR2 Timeout!!!");
        return FALSE;
    }

    return TRUE;
}
#endif
