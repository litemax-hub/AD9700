#ifndef __PANEL__
#define __PANEL__
//#include "board.h"
#include "Panel_define.h"
#include "types.h"
#include "ms_reg.h"

#if ( PanelType == PanelM156B1L01 )
#include "PanelM156B1L01.h"

#elif ( PanelType   == PanelM190A1L02)
#include "PanelCMOM190A1L02.h"

#elif ( PanelType == PanelM185B1L01)
#include "PanelM185B1L01.h"

#elif ( PanelType == PanelLM185WH1TLA1)
#include "PanelLM185WH1TLA1.h"

#elif ( PanelType == PanelM216H1L01)
#include "PanelM216H1L01.h"

#elif (PanelType==PanelLM215WF1)
#include "PanelLM215WF1.h"

#elif(PanelType==PanelLM220WE1)
#include "PanelLM220WE1.h"

#elif (PanelType==PanelLTM200KT01)
#include "PanelLTM200KT01.h"

#elif ( PanelType == PanelLM230WF1 )
#include "PanelLM230WF1.h"

#elif ( PanelType   == PanelTPM200O1L02)
#include "PanelTPM200O1L02.h"

#elif ( PanelType   == PanelCLAA185W)
#include "PanelCLAA185W.h"

#elif ( PanelType   == PanelLM200WD3)
#include "PanelLM200WD3.h"

#elif ( PanelType   == PanelM270HW01)
#include "PanelM270HW01.h"
#elif ( PanelType   == PanelLTM230HT03)
#include "PanelLTM230HT03.h"

#elif ( PanelType   == PanelLTM200KT07)
#include "PanelLTM200KT07.h"

#elif (PanelType == PanelTPM215HW01_HGE)      //2011.9.16 cc add
#include "PanelTPM215HW01_HGE.h"

#elif (PanelType == PanelLM230WF5TLD1)
#include "PanelLM230WF5TLD1.h"

#elif (PanelType==PanelAUOM215HW01VB)
#include "PanelAUOM215HW01VB.h"

#elif (PanelType==PanelCMIM236HGJ_L21)
#include "PanelCMIM236HGJ_L21.h"

#elif (PanelType==PanelCMIM236HGJ_L21_FPGA)
#include "PanelCMIM236HGJ_L21_FPGA.h"

#elif (PanelType==PanelLTM220M1L09) //111012 Modify
#include "PanelLTM220M1L09.h"

#elif (PanelType==PanelLM215WF3_S2A2)
#include "PanelLM215WF3_S2A2.h"


#elif (PanelType==PanelBM230WF3_SJC2)
#include "PanelBM230WF3_SJC2.h"

#elif (PanelType==PanelAUOM240UW01)
#include "PanelAUOM240UW01.h"

#elif (PanelType==PanelAU20SVGA)
#include "PanelAU20SVGA.h"

#elif (PanelType==PanelCMO190)
#include "PanelCMO190.h"

#elif (PanelType==PanelAUOM185XW01)
#include "PanelAUOM185XW01.h"

#elif (PanelType==PanelCMI28UltraHD)
#include "PanelCMI28UltraHD.h"

#elif (PanelType==PanelM250HTN01)
#include "PanelM250HTN01.h"

#elif (PanelType==PanelSAM_LTM27_2560)
#include "PanelLTM27_2560.h"

#elif (PanelType==PanelAUOM238HVN02)
#include "PanelAUOM238HVN02.h"

#elif (PanelType == PanelCMIM238HHJ)
#include "PanelCMIM238HHJ.h"

#elif (PanelType==PanelLM270WR6_eDP)
#include "PanelLM270WR6_eDP.h"

#elif (PanelType==PanelLP156WF6_eDP)
#include "PanelLP156WF6_eDP.h"

#elif (PanelType==PanelLM270WQ5_eDP)
#include "PanelLM270WQ5_eDP.h"

#elif (PanelType==PanelLM315DP01_eDP)
#include "PanelLM315DP01_eDP.h"

#elif (PanelType==PanelLM340UW6_SSB1_eDP)
#include "PanelLM340UW6_SSB1_eDP.h"

#elif (PanelType==PanelM270HAN03_0_eDP)
#include "PanelM270HAN03_0_eDP.h"

#endif


#ifndef PanelVfreq
#define PanelVfreq          (600)
#endif

#ifndef PanelMaxVfreq
#define PanelMaxVfreq   (750+12) // 75Hz
#endif

#ifndef PanelMinVfreq
#define PanelMinVfreq   (500-12) // 50Hz
#endif

#ifndef PanelMiniLVDS3
#define PanelMiniLVDS3	0
#endif

#ifndef PANEL_VCOM_ADJUST
#define PANEL_VCOM_ADJUST    0
#endif

#ifndef DefCool_RedColor
#define DefCool_RedColor            99//103//95//126
#define DefCool_GreenColor          111//115//105//126
#define DefCool_BlueColor           128//130//126//143
#define DefNormal_RedColor          116//117//115//105//140
#define DefNormal_GreenColor        122//125//123//109//135
#define DefNormal_BlueColor         128//130//143
#define DefWarm_RedColor            126//128//123//115//143
#define DefWarm_GreenColor      128//133//129//114//137
#define DefWarm_BlueColor           128//130//132
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
//#message "please measure panel color temp setting"
#endif

#if (PanelType==PanelLTM190M2)
#define DITHER_COEFF    0xE4
#define DITHER_METHOD    0x12
#elif PanelType==PanelCLAA220WA02
#define DITHER_COEFF    0xE4
#define DITHER_METHOD    0x42
#elif PanelType==PanelTPM200O1L02
#define DITHER_COEFF    0xE4
#define DITHER_METHOD    0x40
#else
#define DITHER_COEFF    0x2D
#define DITHER_METHOD    0x42
#endif

#ifndef Enable_PanelHandler
#define Enable_PanelHandler                         1
#endif

#ifndef Enable_ValidTimingStablePanelTurnOn
#define Enable_ValidTimingStablePanelTurnOn         (1&&Enable_PanelHandler)
#endif

#ifndef Enable_ReducePanelPowerOnTime
#define Enable_ReducePanelPowerOnTime               (1&&!Enable_PanelHandler)
#endif

#ifndef PANEL_SSC_MODULATION_DEF_EDP
#define PANEL_SSC_MODULATION_DEF_EDP    32 // unit: 1KHz, range 0~33 means 0~33KHz
#endif

#ifndef PANEL_SSC_MODULATION_DEF_VX1
#define PANEL_SSC_MODULATION_DEF_VX1    30 // unit: 1KHz, range 0~30 means 0~30KHz
#endif

#ifndef PANEL_SSC_MODULATION_DEF
#define PANEL_SSC_MODULATION_DEF        40 // unit: 1KHz, range 0~50 means 0~50KHz
#endif

#ifndef PANEL_SSC_PERCENTAGE_DEF_EDP
#define PANEL_SSC_PERCENTAGE_DEF_EDP    2   // unit: 0.1%, range 0~3  means 0~0.2%
#endif 

#ifndef PANEL_SSC_PERCENTAGE_DEF_VX1
#define PANEL_SSC_PERCENTAGE_DEF_VX1    2   // unit: 0.1%, range 0~5 means 0~0.5%
#endif

#ifndef PANEL_SSC_PERCENTAGE_DEF
#define PANEL_SSC_PERCENTAGE_DEF        20  // unit: 0.1%, range 0~30 means 0~3%
#endif

#ifndef PANEL_SSC_MODULATION_MAX_EDP
#define PANEL_SSC_MODULATION_MAX_EDP    33 // unit: 1KHz, range 0~33 means 0~33KHz
#endif

#ifndef PANEL_SSC_MODULATION_MAX_VX1
#define PANEL_SSC_MODULATION_MAX_VX1    30 // unit: 1KHz, range 0~30 means 0~30KHz
#endif

#ifndef PANEL_SSC_MODULATION_MAX
#if(CHIP_ID == CHIP_MT9701)
#define PANEL_SSC_MODULATION_MAX        50 // unit: 1KHz, range 0~50 means 0~50KHz
#else
#define PANEL_SSC_MODULATION_MAX        100 // unit: 1KHz, range 0~100 means 0~100KHz
#endif
#endif

#ifndef PANEL_SSC_PERCENTAGE_MAX_EDP
#define PANEL_SSC_PERCENTAGE_MAX_EDP    3 // unit: 0.1%, range 0~3 means 0~0.3%
#endif

#ifndef PANEL_SSC_PERCENTAGE_MAX_VX1
#define PANEL_SSC_PERCENTAGE_MAX_VX1    5 // unit: 0.1%, range 0~30 means 0~0.5%
#endif

#ifndef PANEL_SSC_PERCENTAGE_MAX
#define PANEL_SSC_PERCENTAGE_MAX        30 // unit: 0.1%, range 0~30 means 0~3%
#endif

#if PANEL_EDP
#define eDPTXEnSSC                          0
#define eDPTX_SSC_PERCENTAGE_DEF            2	 //1: 0.1%, 2 : 0.2%, 3: 0.3%, 4: 0.4%, 5: 0.5%
#define eDP_SwTBL_ShiftLv                   0
#endif

#ifndef DISABLE_SHORT_LINE_TUNE
#define DISABLE_SHORT_LINE_TUNE    0
#endif

#ifndef DISABLE_SHORT_FRAME
#define DISABLE_SHORT_FRAME    0
#endif

#ifndef PANEL_SWAP_PN
#define PANEL_SWAP_PN       0
#endif


#ifndef PANEL_3D_PASSIVE
#define PANEL_3D_PASSIVE            0
#endif

#ifndef PANEL_3D_PASSIVE_4M
#define PANEL_3D_PASSIVE_4M         0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_LR
#define PANEL_OUTPUT_FMT_3D_LR      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_CB
#define PANEL_OUTPUT_FMT_3D_CB      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_FS
#define PANEL_OUTPUT_FMT_3D_FS      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_TB
#define PANEL_OUTPUT_FMT_3D_TB      0
#endif

#ifndef PANEL_OUTPUT_FMT_3D_SBS
#define PANEL_OUTPUT_FMT_3D_SBS      0
#endif

#ifndef LVDS_CH_A
#define LVDS_CH_A           0
#endif
#ifndef LVDS_CH_B
#define LVDS_CH_B           1
#endif
#ifndef LVDS_CH_C
#define LVDS_CH_C           2
#endif
#ifndef LVDS_CH_D
#define LVDS_CH_D           3
#endif
#ifndef LVDS_CH_E
#define LVDS_CH_E           4
#endif
#ifndef LVDS_CH_F
#define LVDS_CH_F           5
#endif
#ifndef LVDS_CH_G
#define LVDS_CH_G           6
#endif
#ifndef LVDS_CH_H
#define LVDS_CH_H           7
#endif

#ifndef LVDS_CH_A_SWAP
#define LVDS_CH_A_SWAP              LVDS_CH_A
#endif
#ifndef LVDS_CH_B_SWAP
#define LVDS_CH_B_SWAP              LVDS_CH_B
#endif
#ifndef LVDS_CH_C_SWAP
#define LVDS_CH_C_SWAP              LVDS_CH_C
#endif
#ifndef LVDS_CH_D_SWAP
#define LVDS_CH_D_SWAP              LVDS_CH_D
#endif
#ifndef LVDS_CH_E_SWAP
#define LVDS_CH_E_SWAP              LVDS_CH_E
#endif
#ifndef LVDS_CH_F_SWAP
#define LVDS_CH_F_SWAP              LVDS_CH_F
#endif
#ifndef LVDS_CH_G_SWAP
#define LVDS_CH_G_SWAP              LVDS_CH_G
#endif
#ifndef LVDS_CH_H_SWAP
#define LVDS_CH_H_SWAP              LVDS_CH_H
#endif

 // 111227 coding addition, if V(80pin) with dual LVDS output, enable the setting.
//#define DOUBLE_LVDS_CLK_SWING   (PanelDualPort && MainBoardType == BD_5270_M0A)
 // 120920 coding modified, remove the setting of double LVDS clock swing to main board
 #ifndef DOUBLE_LVDS_CLK_SWING
 #define DOUBLE_LVDS_CLK_SWING      0
 #endif

// 120119 coding addition for Y / B type LED light bar control
// "00= 87mV 01= 127mV 10=160mV 11= 212mV"
#ifndef BOOST_CURRENT_LIMIT_VALUE
#define BOOST_CURRENT_LIMIT_VALUE   2
#endif

#ifndef PANEL_TTL
#define PANEL_TTL      0
#endif

#ifndef PANEL_LVDS_1CH
#define PANEL_LVDS_1CH      0
#endif
#ifndef PANEL_LVDS_2CH
#define PANEL_LVDS_2CH      0
#endif
#ifndef PANEL_LVDS_4CH
#define PANEL_LVDS_4CH      0
#endif

#ifndef PANEL_VBY1_1CH_8Bit
#define PANEL_VBY1_1CH_8Bit      0
#endif
#ifndef PANEL_VBY1_1CH_10Bit
#define PANEL_VBY1_1CH_10Bit      0
#endif
#ifndef PANEL_VBY1_2CH_8Bit
#define PANEL_VBY1_2CH_8Bit      0
#endif
#ifndef PANEL_VBY1_2CH_10Bit
#define PANEL_VBY1_2CH_10Bit      0
#endif
#ifndef PANEL_VBY1_4CH_8Bit
#define PANEL_VBY1_4CH_8Bit      0
#endif
#ifndef PANEL_VBY1_4CH_10Bit
#define PANEL_VBY1_4CH_10Bit      0
#endif
#ifndef PANEL_VBY1_8CH_8Bit
#define PANEL_VBY1_8CH_8Bit      0
#endif
#ifndef PANEL_VBY1_8CH_10Bit
#define PANEL_VBY1_8CH_10Bit      0
#endif

////////Select Panel Output Format/////////
#ifndef PANEL_OUTPUT_FMT_OE
#define PANEL_OUTPUT_FMT_OE      0
#endif
#ifndef PANEL_OUTPUT_FMT_LR
#define PANEL_OUTPUT_FMT_LR      0
#endif
#ifndef PANEL_OUTPUT_FMT_4B
#define PANEL_OUTPUT_FMT_4B      0
#endif
#ifndef PANEL_OUTPUT_FMT_8B
#define PANEL_OUTPUT_FMT_8B      0
#endif
#ifndef PANEL_OUTPUT_FMT_5B
#define PANEL_OUTPUT_FMT_5B      0
#endif
#ifndef PANEL_OUTPUT_FMT_10B
#define PANEL_OUTPUT_FMT_10B      0
#endif

#ifndef PANEL_EDP
#define PANEL_EDP       0
#endif

#if !PANEL_EDP
#define eDPTX_HBR2                  0x14
#define eDPTX_HBR                   0x0A
#define eDPTX_RBR                   0x06
#define eDPTX_4Lanes                4
#define eDPTX_2Lanes                2
#define eDPTX_1Lanes                1

#define eDPTX_Port0                 BIT0
#define eDPTX_Port1                 BIT1
#define eDPTX_Port2                 BIT2
#define eDPTX_Port3                 BIT3

#define eDPTX_FineTune_TU_En        0
#define eDPTXDETECTHPD              1
#define eDPTXAutoTest               0
#define eDPTXP0PNSWAP               0
#define eDPTXP1PNSWAP               0
#define eDPTXAuxP0PNSWAP            0
#define eDPTXAuxP1PNSWAP            0
#define eDPTX_total_ports           1 //numbers of use total ports : 1,2,4
#define eDPTXMODOnePort             1   // if MOD play in Only One eDPTX Port , Set 1 to support  it.
#define eDPTXMODUsePort             0   // 0 : port0, 1: port1, 2: port0&1
#define eDPTXHBR2PortEn             0   // set 1 when use MT9701 chip && eDPTX ouput in HBR2 path
#define eDPTXColorDepth             1   // 1:12 bits ; 2: 10bits; 3 : 8bits; 4 : 6bits
#define eDPTXEnPG                   0
#define eDPTXHPDPort0               eDPTXHPD_USE_GPIO56
#define eDPTXHPDPort1               eDPTXHPD_USE_GPIO57
#define eDP_SWING_DefaultValue      0x2E   // 0x00~0x3F
#define eDP_PREMPHASIS_DefaultValue 0x00   // 0x00~0x07
#define eDPTXP0LaneSWAP             0 // HBR2Port# (ch0~ch3) lane swap
#define eDPTXP1LaneSWAP             0 // HBR2Port# (ch0~ch3) lane swap
#define eDPTXEnSSC                  0
#define eDPTXEnASSR                 0     // method  #3a
#define eDPTXEnAFR                  0    // method  #3b
#define eDPTX_NO_AUX_HANDSHAKE_LINK_TRAINING    0  //

#define eDPTXMSA_Htotal             g_sPnlInfo.sPnlTiming.u16Htt // Htotal
#define eDPTXMSA_Hwidth             g_sPnlInfo.sPnlTiming.u16Width // Hactive
#define eDPTXMSA_HStart             g_sPnlInfo.sPnlTiming.u16HSyncWidth+g_sPnlInfo.sPnlTiming.u16HSyncBP // HStart  == HSW+HBACKPORCH
#define eDPTXMSA_HSP                1           // HSP   [15]=Polarity  set 0 or 1
#define eDPTXMSA_HSW                g_sPnlInfo.sPnlTiming.u16HSyncWidth // Hsw 20  [15]=Polarity
#define eDPTXMSA_Vtotal             g_sPnlInfo.sPnlTiming.u16Vtt // Vtotal
#define eDPTXMSA_VStart             g_sPnlInfo.sPnlTiming.u16VSyncWidth+g_sPnlInfo.sPnlTiming.u16VSyncBP // Vstart == VSW+VBACKPORCH
#define eDPTXMSA_VSP                0           // VSP   [15]=Polarity  set 0 or 1
#define eDPTXMSA_VSW                g_sPnlInfo.sPnlTiming.u16VSyncWidth // Vsw   [15]=Polarity
#define eDPTXMSA_Vheight            g_sPnlInfo.sPnlTiming.u16Height // Vactive
#define eDPTXMSA_HB                 (eDPTXMSA_Htotal)-(eDPTXMSA_Hwidth)  // HB   Blanking
#define eDPTXMSA_VB                 (eDPTXMSA_Vtotal)-(eDPTXMSA_Vheight) // VB   Blanking
#define eDPTXMSA_HFP                (eDPTXMSA_HB)-(g_sPnlInfo.sPnlTiming.u16HStart)  // H Front porch
#define eDPTXMSA_VFP                (eDPTXMSA_VB)-(g_sPnlInfo.sPnlTiming.u16VStart) // VFront porch
#define eDPTXMSA_HBP                g_sPnlInfo.sPnlTiming.u16HSyncBP  // H  Back porch
#define eDPTXMSA_VBP                g_sPnlInfo.sPnlTiming.u16VSyncBP // V Back porch

typedef enum
{
    eDPTXHPD_USE_GPIO50=0,
    eDPTXHPD_USE_GPIO51,
    eDPTXHPD_USE_GPIO52,
    eDPTXHPD_USE_GPIO53,
}eDPTXHPD_USE_GPIO_GP0_TYP;

typedef enum
{
    eDPTXHPD_USE_GPIO56=0,
    eDPTXHPD_USE_GPIO57,
}eDPTXHPD_USE_GPIO_GP1_TYP;

typedef enum
{
    eDPTXHPDUseGPIOGroup0=0,
    eDPTXHPDUseGPIOGroup1,
}eDPTXHPD_USE_GPIO_Group_TYP;

#endif

#ifndef LVDS_TI_BitModeSel
#define LVDS_TI_BitModeSel          2 // TI_10BIT:1 ,TI_8BIT:2,TI_6BIT:3
#endif

#ifndef LVDS_CH_OUT
#define LVDS_CH_OUT                 0x555
#endif

#ifndef LVDS_2CH_A_OUT
#define LVDS_2CH_A_OUT          LVDS_CH_OUT
#endif

#ifndef LVDS_2CH_B_OUT
#define LVDS_2CH_B_OUT          LVDS_CH_OUT
#endif

#ifndef LVDS_2CH_C_OUT
#define LVDS_2CH_C_OUT          0
#endif

#ifndef LVDS_2CH_D_OUT
#define LVDS_2CH_D_OUT          0
#endif

#ifndef LVDS_2CH_E_OUT
#define LVDS_2CH_E_OUT          0
#endif

#ifndef LVDS_2CH_F_OUT
#define LVDS_2CH_F_OUT          0
#endif

#ifndef LVDS_2CH_G_OUT
#define LVDS_2CH_G_OUT          0
#endif

#ifndef LVDS_2CH_H_OUT
#define LVDS_2CH_H_OUT          0
#endif
#ifndef LVDS_SWING_TARGET
#define LVDS_SWING_TARGET  250 //unit:mV  for EFUSE calculation
#endif

#ifndef PANEL_SWAP_PN_LOCKN
#define PANEL_SWAP_PN_LOCKN       0
#endif

#if(CHIP_ID == CHIP_MT9701)
#define VBY1_SWING_448 0
#define VBY1_SWING_436 1
#define VBY1_SWING_424 2
#define VBY1_SWING_412 3
#define VBY1_SWING_400 4 // default: 400mV
#define VBY1_SWING_388 5
#define VBY1_SWING_376 6
#define VBY1_SWING_364 7
#define VBY1_SWING_351 8
#define VBY1_SWING_339 9
#define VBY1_SWING_326 10
#define VBY1_SWING_313 11
#define VBY1_SWING_300 12
#define VBY1_SWING_286 13
#define VBY1_SWING_273 14
#define VBY1_SWING_243 15
#else
#define VBY1_SWING_452 0
#define VBY1_SWING_440 1
#define VBY1_SWING_428 2
#define VBY1_SWING_416 3
#define VBY1_SWING_404 4
#define VBY1_SWING_392 5
#define VBY1_SWING_380 6
#define VBY1_SWING_370 7
#endif

#if CHIP_ID == CHIP_MT9701
    #if (PanelVTotal>SC_MASK_V)
        #error "--------------------PanelVTotal Overs Scaler Limit---------------------------"
    #endif
#if (PanelMaxVTotal>SC_MASK_V)
    #ifndef _HW_AUTO_NO_SIGNAL_
    #define _HW_AUTO_NO_SIGNAL_//use HW auto no signal
    #endif
#endif
#else
    #if (PanelMaxVTotal>SC_MASK_V)
        #error "--------------------PanelMaxVTotal Overs Scaler Limit---------------------------"
    #endif
#endif

#if CHIP_ID == CHIP_MT9701 // User can remove to prevent build code error if PanelMaxDCLK is float
    #if PanelMaxDCLK > 441
    #error "--------------------PanelMaxDCLK Overs Scaler Limit---------------------------"
    #endif
#endif

#if 0//PANEL_EDP
    #if(((PanelHTotal-PanelWidth-PanelHSyncBackPorch-PanelHSyncWidth)<20)||(PanelHSyncBackPorch<20))
        #error "--------------------eDP Panel FrontPorch/BackPorch for each port need > 20---------------------------"
    #endif

#endif

#if ((PanelMinHTotal - PanelHStart - PanelWidth) <= 4)
    #error "--------------------Panel mini Hfront porch should be larger than 4---------------------------"
#endif

#define PANEL_VBY1_1CH()         ((g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_1_8BIT) || (g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_1_10BIT) )
#define PANEL_VBY1_2CH()         ((g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_8BIT) || (g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_10BIT) )
#define PANEL_VBY1_4CH()         ((g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_8BIT) || (g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_10BIT) )
#define PANEL_VBY1_8CH()         ((g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_8BIT) || (g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_10BIT) )

typedef enum
{
    EN_LVDS_CH_A = LVDS_CH_A,
    EN_LVDS_CH_B = LVDS_CH_B,
    EN_LVDS_CH_C = LVDS_CH_C,
    EN_LVDS_CH_D = LVDS_CH_D,
    EN_LVDS_CH_E = LVDS_CH_E,
    EN_LVDS_CH_F = LVDS_CH_F,
    EN_LVDS_CH_G = LVDS_CH_G,
    EN_LVDS_CH_H = LVDS_CH_H,
} EN_PNL_LVDS_CH;

typedef enum
{
    EN_PNL_LVDS_NONE,
    EN_PNL_LVDS_CH_1,
    EN_PNL_LVDS_CH_2,
    EN_PNL_LVDS_CH_4,
} EN_PNL_TYPE_LVDS;

typedef enum
{
    EN_PNL_VBY1_NONE,
    EN_PNL_VBY1_CH_1_8BIT,
    EN_PNL_VBY1_CH_1_10BIT,
    EN_PNL_VBY1_CH_2_8BIT,
    EN_PNL_VBY1_CH_2_10BIT,
    EN_PNL_VBY1_CH_4_8BIT,
    EN_PNL_VBY1_CH_4_10BIT,
    EN_PNL_VBY1_CH_8_8BIT,
    EN_PNL_VBY1_CH_8_10BIT,
} EN_PNL_TYPE_VBY1;

typedef enum
{
    EN_PNL_EDP_NONE,
    EN_PNL_EDP_ENABLE,
} EN_PNL_TYPE_EDP;

typedef enum
{
    EN_PNL_EDP_RBR,
    EN_PNL_EDP_HBR,
    EN_PNL_EDP_HBR2,
} EN_PNL_EDP_LINKRATE;

typedef enum
{
    EN_PNL_EDP_LANE_CNT_1,
    EN_PNL_EDP_LANE_CNT_2,
    EN_PNL_EDP_LANE_CNT_4,
} EN_PNL_EDP_LANE_CNT;

typedef enum
{
    EN_PNL_OUT_FMT_NONE,
    EN_PNL_OUT_FMT_LR,
    EN_PNL_OUT_FMT_4B,
    EN_PNL_OUT_FMT_8B,
} EN_PNL_OUT_FMT;
    
typedef struct
{
    // H info
    U16     u16HSyncWidth;
    U16     u16HSyncBP;
    U16     u16HStart;
    U16     u16Htt;
    U16     u16HttMax;
    U16     u16HttMin;
    U16     u16Width;

    // V info
    U16     u16VSyncWidth;
    U16     u16VSyncBP;
    U16     u16VStart;
    U16     u16Vtt;
    U16     u16VttMax;
    U16     u16VttMin;
    U16     u16Height;
    U16     u16VFreq;
    U16     u16VFreqMax;
    U16     u16VFreqMin;

    // Pixel Clock
    float   u16DClk;
    float   u16DClkMax;
    float   u16DClkMin;

} ST_PANELTIMING_INFO;

typedef struct
{
    // Type LVDS
    EN_PNL_TYPE_LVDS    ePnlTypeLvds;
    EN_PNL_LVDS_CH      ePnlLvdsChA;
    EN_PNL_LVDS_CH      ePnlLvdsChB;
    EN_PNL_LVDS_CH      ePnlLvdsChC;
    EN_PNL_LVDS_CH      ePnlLvdsChD;
    EN_PNL_LVDS_CH      ePnlLvdsChE;
    EN_PNL_LVDS_CH      ePnlLvdsChF;
    EN_PNL_LVDS_CH      ePnlLvdsChG;
    EN_PNL_LVDS_CH      ePnlLvdsChH;
    U8                  u8PnlLvdsTiEn;
    U8                  u8PnlLvdsTiBitMode;
    U8                  u8PnlSwapPn;
    U8                  u8PnlSwapMlsb;
    U8                  u8PnlInvDE;
    U8                  u8PnlInvVSync;
    U8                  u8PnlInvHSync;

    // Type Vby1
    EN_PNL_TYPE_VBY1    ePnlTypeVby1;
    U8                  u8PnlVby1SwpPnLockn;

    // Type Edp
    EN_PNL_TYPE_EDP     ePnlTypeEdp;
    U8                  u8PnlEdpLinkRate;
    U8                  u8PnlEdpLaneCnt;
    BOOL                bPnlEdpFineTuneTUEn;
    BOOL                bPnlEdpDETECTHPD;
    BOOL                bPnlEdpP0PNSWAP;
    BOOL                bPnlEdpP1PNSWAP;
    BOOL                bPnlEdpAuxP0PNSwap;
    BOOL                bPnlEdpAuxP1PNSwap;
    BOOL                u8PnlEdpTotalPorts;
    BOOL                u8PnlEdpMODOnePort;
    BOOL                u8PnlEdpMODUsePort;
    BOOL                u8PnlEdpHBR2PortEn;
    BOOL                u8PnlEdpColorDepth;
    BOOL                bPnlEdpEnPG;
    U8                  u8PnlEdpHPDPort0;
    U8                  u8PnlEdpHPDPort1;
    BOOL                u8PnlEdpSwTBLShiftLv;
    U8                  u8PnlEdpSwDftValue;
    U8                  u8PnlEdpPreemphasisDftValue;
    BOOL                bPnlEdpP0LaneSwap;
    BOOL                bPnlEdpP1LaneSwap;
    BOOL                bPnlEdpEnSSC;
    BOOL                bPnlEdpEnASSR;
    BOOL                bPnlEdpEnAFR;
    BOOL                bPnlEdpNoAuxHandshakeLinkTrain;

    // Output Format
    EN_PNL_OUT_FMT      ePnlOutFmt;

    // Output Timing
    ST_PANELTIMING_INFO sPnlTiming;
} ST_PANEL_INFO;

#endif
