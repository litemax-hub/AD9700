#ifndef __PANEL_BOEDV195FBB_N10__
#define __PANEL_BOEDV195FBB_N10__
#define PanelName               "BOE_DV195FBB"

//////////////Select Panel Type/////////////
#define PANEL_TTL               0
#define PANEL_LVDS_1CH          1
#define PANEL_LVDS_2CH          0
#define PANEL_LVDS_4CH          0
#define PANEL_VBY1_1CH_8Bit     0
#define PANEL_VBY1_1CH_10Bit    0
#define PANEL_VBY1_2CH_8Bit     0
#define PANEL_VBY1_2CH_10Bit    0
#define PANEL_VBY1_4CH_8Bit     0
#define PANEL_VBY1_4CH_10Bit    0
#define PANEL_VBY1_8CH_8Bit     0
#define PANEL_VBY1_8CH_10Bit    0
#define PANEL_VBY1()       ((PANEL_VBY1_1CH_8Bit)  ||  \
                            (PANEL_VBY1_1CH_10Bit) ||  \
                            (PANEL_VBY1_2CH_8Bit)  ||  \
                            (PANEL_VBY1_2CH_10Bit) ||  \
                            (PANEL_VBY1_4CH_8Bit)  ||  \
                            (PANEL_VBY1_4CH_10Bit) ||  \
                            (PANEL_VBY1_8CH_8Bit)  ||  \
                            (PANEL_VBY1_8CH_10Bit) )
#define PANEL_EDP               0

#define LVDS_CH_A_SWAP      LVDS_CH_A
#define LVDS_CH_B_SWAP      LVDS_CH_B
#define LVDS_CH_C_SWAP      LVDS_CH_C
#define LVDS_CH_D_SWAP      LVDS_CH_D

#define PanelDither   8
#define PanelTTL    0
#define PanelTCON   0
#define PanelLVDS   0xff
#define PanelRSDS   0
#define PanelminiLVDS      0
#define ShortLineCheck    0
#define LVDS_TIMode   0xff
#define PanelDualPort   0
#define PanelSwapPort   0
#define PanelSwapOddML    0
#define PanelSwapEvenML   0
#define PanelSwapOddRB    0
#define PanelSwapEvenRB   0
#define PanelSwapMLSB   0
#define PanelDClkDelay    8
#define PanelInvDE      0
#define PanelInvDClk    0
#define PanelInvHSync   0
#define PanelInvVSync   0
#define PANEL_SWAP_PN      0
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent  1 // Dclk current
#define PanelDECurrent    1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent  1 // Even data current
#define PanelOnTiming1          30//huimin 20080921*changemode huayi 40// time between panel & data while turn on power
#define PanelOnTiming2          550// time between data & back light while turn on power
#define PanelOffTiming1         250// time between back light & data while turn off power
#define PanelOffTiming2         20//20//25 // 2005/5/3//10 // time between data & panel while turn off power
#define PanelOffOnDelay         1100

#define PanelHSyncWidth         32
#define PanelHSyncBackPorch     128

#define PanelVSyncWidth         3
#define PanelVSyncBackPorch     13

#define PanelHStart             (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart             (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth              1920
#define PanelHeight             536
#define PanelHTotal             2300//(2200-52)
#define PanelVTotal             581//(1125-5)


#define PanelMaxHTotal          2400    //4094
#define PanelMinHTotal          2200//2112//2080    // 2040
#define PanelMaxVTotal          596
#define PanelMinVTotal          566
#define PanelDCLK               80
#define PanelMaxDCLK            93
#define PanelMinDCLK            68

#define HV_OUTPUT_TYPE          HSRM_B

// FRC coding test panel
#define PanelVfreq          (600)
#define PanelMaxVfreq   (650+20) // 75Hz
#define PanelMinVfreq   (550-20) // 50Hz

#define DOUBLE_LVDS_CLK_SWING	1//0
#define LVDS_SWING_TARGET  350//250
//=================================================================
// TCON  setting for RSDS
// TCON  setting for RSDS
#define TCON_Format1  0x1 // OFC1: control polarity & inversion
#define ESP_AfterData   0 // Even Start Pulse after Data
#define ESP_Offset  1 // Even start pulse position
#define OSP_AfterData   0 // Odd Start Pulse after Data
#define OSP_Offset  1 // Odd start pulse position
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelOESPCurrent  2 // OSP/ESP drive current
#define PanelOEGCurrent   2 // OPOL/EPOL/GPOL drive current
#define PanelEINVCurrent  0 // EINV drive current
#define PanelOINVCurrent  0 // OINV drive current
#define OuputFormat   (BIT3|BIT5)//Enable Auto Toggle GPO0 & data invert
// POL
#define GPO0_VStart   0x000
#define GPO0_VEnd   0x000
#define GPO0_HStart   0x186 //0x068
#define GPO0_HEnd   0x186 //0x222
#define GPO0_Control    0x02 //0x02 //0x82
//LP
#define GPO1_VStart   0
#define GPO1_VEnd   0
#define GPO1_HStart   0x406 //0x407
#define GPO1_HEnd   0x41b //0x428
#define GPO1_Control  0x00
// STV
#define GPO2_VStart   0x000
#define GPO2_VEnd   0x001
#define GPO2_HStart   0x166 //x170
#define GPO2_HEnd   0x166 //x170
#define GPO2_Control  0x00
// CLKV
#define GPO3_VStart   0
#define GPO3_VEnd   0x00 //0x300
#define GPO3_HStart   0x166 //0x173
#define GPO3_HEnd   0x406 //0x407
#define GPO3_Control  0x01
// OE
#define GPO4_VStart   0
#define GPO4_VEnd   0
#define GPO4_HStart   0x384 //0x357
#define GPO4_HEnd   0x442 //0x427
#define GPO4_Control  0x00
//PVS / FSYNC
#define GPO9_VStart   0 //0x002
#define GPO9_VEnd   0 //0x306
#define GPO9_HStart   0 //0x00A
#define GPO9_HEnd   0 //0x005
#define GPO9_Control  0 //0x08
//HSYNC
#define GPOA_VStart   0
#define GPOA_VEnd   0
#define GPOA_HStart   0
#define GPOA_HEnd   0
#define GPOA_Control  0

//=================================================================
// Color Temp
#define DefCool_RedColor            128 //108
#define DefCool_GreenColor          120 //128
#define DefCool_BlueColor           123 //128

#define DefNormal_RedColor          128
#define DefNormal_GreenColor        128
#define DefNormal_BlueColor         128
#if (StandardBoardPPS == P2506168)
#define DefWarm_RedColor          128
#define DefWarm_GreenColor        117
#define DefWarm_BlueColor         110
#else
#define DefWarm_RedColor          128
#define DefWarm_GreenColor        117
#define DefWarm_BlueColor         110
#endif
#define DefsRGB_RedColor            DefWarm_RedColor
#define DefsRGB_GreenColor          DefWarm_GreenColor
#define DefsRGB_BlueColor           DefWarm_BlueColor
//=================================================================
// Backlight Curve
#if (StandardBoardPPS == P2506168)
#define DEF_FAC_BRIGHTNESS_0        12
#define DEF_FAC_BRIGHTNESS_25       21
#define DEF_FAC_BRIGHTNESS_50       29
#define DEF_FAC_BRIGHTNESS_75       44 // 44 = 333 nits
#define DEF_FAC_BRIGHTNESS_99       55
#else
#define DEF_FAC_BRIGHTNESS_0        10
#define DEF_FAC_BRIGHTNESS_25       25
#define DEF_FAC_BRIGHTNESS_50       50
#define DEF_FAC_BRIGHTNESS_75       75
#define DEF_FAC_BRIGHTNESS_99       100
#endif

#endif

