// for CMO M236H5-L05
#ifndef __LG27_2560_H
#define __LG27_2560_H

#define PanelName   "LTM27"
#define PanelDither         6

#define PanelTTL            0
#define PanelTCON           0
#define PanelLVDS           0xFF
#define PanelRSDS   0

#define PANEL_SWAP_AB_PORT  0 //0: non-swap, 0xFF: AB swap
#define PANEL_SWAP_CD_PORT  0//0: non-swap ,0xFF: CD swap

#define LVDS_CH_A_SWAP      LVDS_CH_C
#define LVDS_CH_B_SWAP      LVDS_CH_A
#define LVDS_CH_C_SWAP      LVDS_CH_D
#define LVDS_CH_D_SWAP      LVDS_CH_B


//////////////Select Panel Type/////////////
#define PANEL_TTL                   0
#define PANEL_LVDS_1CH              0
#define PANEL_LVDS_2CH              0
#define PANEL_LVDS_4CH              1
#define PANEL_LVDS_8CH              0
#define PANEL_VBY1_1CH_8Bit         0
#define PANEL_VBY1_1CH_10Bit        0
#define PANEL_VBY1_2CH_8Bit         0
#define PANEL_VBY1_2CH_10Bit        0
#define PANEL_VBY1_4CH_8Bit         0
#define PANEL_VBY1_4CH_10Bit        0
#define PANEL_VBY1_8CH_8Bit         0
#define PANEL_VBY1_8CH_10Bit        0
#define PANEL_VBY1()       ((PANEL_VBY1_1CH_8Bit)  ||  \
                            (PANEL_VBY1_1CH_10Bit) ||  \
                            (PANEL_VBY1_2CH_8Bit)  ||  \
                            (PANEL_VBY1_2CH_10Bit) ||  \
                            (PANEL_VBY1_4CH_8Bit)  ||  \
                            (PANEL_VBY1_4CH_10Bit) ||  \
                            (PANEL_VBY1_8CH_8Bit)  ||  \
                            (PANEL_VBY1_8CH_10Bit) )
#define PANEL_EDP                   0
////////Select Panel Output Format/////////
#define PANEL_OUTPUT_FMT_LR         0
#define PANEL_OUTPUT_FMT_4B         0

#define PAIR_SWAP_B         0

#define ShortLineCheck      0xFF
#define LVDS_TIMode         0xFF//  //Thine mode:disable TI mode

#define LVDS_TI_BitModeSel  2

#define PanelDualPort       0xFF
#define PanelQuadPort       0
#define PanelSwapPort       0
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0
#define PanelSwapEvenRB     0
#define PanelSwapMLSB       0
#define PANEL_SWAP_PN       0

//    panel output type select
#define PANEL_OUTPUT_FMT_3D_LR      1
#define PANEL_OUTPUT_FMT_3D_CB      0

#define PanelDClkDelay      0
#define PanelInvDE          0
#define PanelInvDClk        0
#define PanelInvHSync       0
#define PanelInvVSync       0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent    1 // Dclk current
#define PanelDECurrent      1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

#define PanelOnTiming1      10 // time between panel & data while turn on power
#define PanelOnTiming2      500 // time between data & back light while turn on power
#define PanelOffTiming1     10 // time between back light & data while turn off power
#define PanelOffTiming2     10 // time between data & panel while turn off power

#define PanelHSyncWidth         10
#define PanelHSyncBackPorch    48

#define PanelVSyncWidth         5
#define PanelVSyncBackPorch    26

#define PanelHStart         (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart         (PanelVSyncWidth+PanelVSyncBackPorch)

#define PanelWidth              2560
#define PanelHeight             1440
#define PanelHTotal             2720
#define PanelVTotal             1481
#define PanelVsyncFreq      60

#define PanelMaxHTotal        2728
#define PanelMinHTotal         2712
#define PanelMaxVTotal         1483
#define PanelMinVTotal         1479
#define PanelDCLK               (((DWORD)PanelHTotal*PanelVTotal*PanelVsyncFreq)/1000000)
#define PanelMaxDCLK           243
#define PanelMinDCLK           240
#define Panel_VTT_3D_120    1588
#define Panel_VTT_3D_110    1732
#define Panel_VTT_3D_100    1905
#define Panel_VTT_3D        Panel_VTT_3D_120

//=================================================================
// Not Used
#if 0
// backlight voltage
#define PNL_PWM_INVERT          1

#define USE_FRAME_PLL               1

#define PANEL_DITHER                0 // 8/6 bits panel
#define PANEL_MONO                  0
#define PANEL_MONO_MONO_MODE        0 //_PNL_FUNC_EN_

#define PANEL_RSDS                  0
#define PANEL_MINI_LVDS_5PAIR       0
#define PANEL_MINI_LVDS_6PAIR       0
#define PANEL_MINI_LVDS_6PAIR_TV    0

#endif // Not Used

//=================================================================
// TCON  setting for LVDS
//PVS / FSYNC
#define GPO9_VStart     0x7
#define GPO9_VEnd       0x705
#define GPO9_HStart     0x0
#define GPO9_HEnd       0x0
#define GPO9_Control        0x7

//HSYNC
#define GPOA_VStart     0x006
#define GPOA_VEnd       0x408
#define GPOA_HStart     0
#define GPOA_HEnd       0
#define GPOA_Control        0x4

//I_Gen_Tuning CH Source Select
#define I_Gen_CH  CH3_CLK

//I_Gen_Tuning Target Select
#define Target_Select Target_250mv

#endif
