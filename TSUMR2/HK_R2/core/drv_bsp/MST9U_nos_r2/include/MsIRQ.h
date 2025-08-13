////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file   MsIRQ.h
/// @brief  MStar IRQ
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MS_IRQ_H_
#define _MS_IRQ_H_



#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
#define MS_IRQ_MAX          (128) //64 IRQs + 64 FIQs
#define ENABLE_USB_PORT0
#define E_IRQ_FIQ_INVALID   0xFFFF

// Interrupt related
typedef enum
{
    // PM FIQ
    E_INT_PM_FIQL_START                = 0,
    E_INT_PM_FIQ_EXTIMER0              = E_INT_PM_FIQL_START +  0,
    E_INT_PM_FIQ_WDT                   = E_INT_PM_FIQL_START +  1,
    E_INT_PM_FIQ_PM_XIU_TIMEOUT        = E_INT_PM_FIQL_START +  2,
    E_INT_PM_FIQ_MENULOAD              = E_INT_PM_FIQL_START +  4,
    E_INT_PM_FIQ_PM                    = E_INT_PM_FIQL_START +  5,
    E_INT_PM_FIQ_PM_MCCS_WAKEUP        = E_INT_PM_FIQL_START +  6,
    E_INT_PM_FIQ_DW_UART0_DET          = E_INT_PM_FIQL_START +  9,
    E_INT_PM_FIQ_HKR2_UART0_DET        = E_INT_PM_FIQL_START + 10,
    E_INT_PM_FIQ_HKR2_TO_PD51          = E_INT_PM_FIQL_START + 13,
    E_INT_PM_FIQ_HKR2_TO_HST2          = E_INT_PM_FIQL_START + 14,
    E_INT_PM_FIQ_HKR2_TO_SECUR2        = E_INT_PM_FIQL_START + 15,
    E_INT_PM_FIQ_DVDD_PWRGD            = E_INT_PM_FIQL_START + 16,
    E_INT_PM_FIQ_PD51_TO_HKR2          = E_INT_PM_FIQL_START + 18,
    E_INT_PM_FIQ_PD51_TO_HST2          = E_INT_PM_FIQL_START + 19,
    E_INT_PM_FIQ_PD51_TO_SECUR2        = E_INT_PM_FIQL_START + 20,
    E_INT_PM_FIQ_HST2_TO_HKR2          = E_INT_PM_FIQL_START + 21,
    E_INT_PM_FIQ_HST2_TO_PD51          = E_INT_PM_FIQL_START + 22,
    E_INT_PM_FIQ_HST2_TO_SECUR2        = E_INT_PM_FIQL_START + 23,
    E_INT_PM_FIQ_SECUR2_TO_HKR2        = E_INT_PM_FIQL_START + 24,
    E_INT_PM_FIQ_SECUR2_TO_PD51        = E_INT_PM_FIQL_START + 25,
    E_INT_PM_FIQ_SECUR2_TO_HST2        = E_INT_PM_FIQL_START + 26,
    E_INT_PM_FIQH_END                  = 31,

    // FIQEXP
    E_INT_PM_FIQEXPL_START             = 32,
    E_FIQ_HKR2_TO_SECUR2               = E_INT_PM_FIQEXPL_START + 4,
    E_INT_PM_FIQEXPH_END               = 63,


    // PM IRQ
    E_INT_PM_IRQL_START                = 64,
    E_INT_PM_IRQ_NPM_IRQ_TO_SECUR2     = E_INT_PM_IRQL_START  +  0,
    E_INT_PM_IRQ_PM_FIQ_OUT            = E_INT_PM_IRQL_START  +  1,
    E_INT_PM_IRQ_D2B                   = E_INT_PM_IRQL_START  +  2,
    E_INT_PM_IRQ_GPIO_C_WKUP0          = E_INT_PM_IRQL_START  +  3,
    E_INT_PM_IRQ_GPIO_C_WKUP1          = E_INT_PM_IRQL_START  +  4,
    E_INT_PM_IRQ_FSP                   = E_INT_PM_IRQL_START  +  5,
    E_INT_PM_IRQ_GPIO_C_WKUP2          = E_INT_PM_IRQL_START  +  6,
    E_INT_PM_IRQ_GPIO_C_WKUP3          = E_INT_PM_IRQL_START  +  7,
    E_INT_PM_IRQ_CEC0_OUT              = E_INT_PM_IRQL_START  +  8,
    E_INT_PM_IRQ_SCDC                  = E_INT_PM_IRQL_START  +  9,
    E_INT_PM_IRQ_PM_UART0              = E_INT_PM_IRQL_START  + 10,
    E_INT_PM_IRQ_PM_MIIC0              = E_INT_PM_IRQL_START  + 11,
    E_INT_PM_IRQ_NPM_FIQ_TO_SECUR2     = E_INT_PM_IRQL_START  + 12,
    E_INT_PM_IRQ_DP_AUX_ALL            = E_INT_PM_IRQL_START  + 13,
    E_INT_PM_IRQ_PM_IN                 = E_INT_PM_IRQL_START  + 14,
    E_INT_PM_IRQ_KEYPAD                = E_INT_PM_IRQL_START  + 15,
    E_INT_PM_IRQ_GPIO_C_WKUP4          = E_INT_PM_IRQL_START  + 16,
    E_INT_PM_IRQ_GPIO_C_WKUP5          = E_INT_PM_IRQL_START  + 17,
    E_INT_PM_IRQ_GPIO_C_WKUP6          = E_INT_PM_IRQL_START  + 18,
    E_INT_PM_IRQ_GPIO_C_WKUP7          = E_INT_PM_IRQL_START  + 19,
    E_INT_PM_IRQ_GPIO_C_WKUP8          = E_INT_PM_IRQL_START  + 20,
    E_INT_PM_IRQ_GPIO_C_WKUP9          = E_INT_PM_IRQL_START  + 21,
    E_INT_PM_IRQ_DDC_ID                = E_INT_PM_IRQL_START  + 22,
    E_INT_PM_IRQ_PM_ERROR_RESP         = E_INT_PM_IRQL_START  + 23,
    E_INT_PM_IRQ_PM_MIIC1              = E_INT_PM_IRQL_START  + 24,
#if defined(CHIP_ID) && (CHIP_ID == CHIP_MT9701)
    E_INT_PM_IRQ_NPM_IRQ_TO_HKR2       = E_INT_PM_IRQL_START  + 25,
    E_INT_PM_IRQ_U3_WAKEUP             = E_INT_PM_IRQL_START  + 26,
    E_INT_PM_IRQ_U3_RD                 = E_INT_PM_IRQL_START  + 27,
    E_INT_PM_IRQ_U3_RT                 = E_INT_PM_IRQL_START  + 28,
    E_INT_PM_IRQ_NPM_IRQ_TO_PD51       = E_INT_PM_IRQL_START  + 29,
    E_INT_PM_IRQ_NPM_FIQ_TO_PD51       = E_INT_PM_IRQL_START  + 30,
    E_INT_PM_IRQ_NPM_FIQ_TO_HKR2       = E_INT_PM_IRQL_START  + 31,
#else
    E_INT_PM_IRQ_NPM_IRQ_TO_HKR2       = E_INT_PM_IRQL_START  + 29,
    E_INT_PM_IRQ_NPM_FIQ_TO_HKR2       = E_INT_PM_IRQL_START  + 30,
#endif
    E_INT_PM_IRQH_END                  = 95,

    //IRQEXP
    E_INT_PM_IRQEXPL_START             = 96,
    E_INT_PM_IRQEXPH_END               = 127,

    E_INT_PM_IRQ_FIQ_NONE              = 0xFE,
    E_INT_PM_IRQ_FIQ_ALL               = 0xFF,
    E_INT_PM_FIQ_0xF0_END              = 0xFF,
} InterruptNum;

#ifdef __cplusplus
}
#endif

#endif // _MS_IRQ_H_
