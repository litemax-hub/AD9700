#ifndef __PANEL_AUO_ULTRA_HD_eDP__
#define __PANEL_AUO_ULTRA_HD_eDP__
#define PanelNumber             1800
#define PanelName               "PanelAuo UltraHD_eDP"

#define PanelDither         8
#define DITHER_COEFF        0x2D
#define DITHER_METHOD       0x42

#define PanelTTL            0
#define PanelTCON           0
#define PanelLVDS           0xff
#define PanelRSDS           0
#define PanelminiLVDS       0

#define ShortLineCheck      0
#define LVDS_TIMode         0//0xff
#define PanelDualPort       0xff
//////////////Select Panel Type/////////////
#define PANEL_TTL               0
#define PANEL_LVDS_1CH          0
#define PANEL_LVDS_2CH          0
#define PANEL_LVDS_4CH          0
#define PANEL_LVDS_8CH          0 // 1
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
#define PANEL_EDP               1 // 1

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
////////////////////////////////////////////////////////////
// ####  Fix Parameter for eDPTX (Do Not Change this Parameter) ####
////////////////////////////////////////////////////////////
#define eDPTX_HBR2 0x14
#define eDPTX_HBR   0x0A
#define eDPTX_RBR   0x06
#define eDPTX_4Lanes  4
#define eDPTX_2Lanes  2
#define eDPTX_1Lanes  1
////////////////////////////////////////////////////////////
// #######   Configure for different eDP Panel  ####################
////////////////////////////////////////////////////////////
#if PANEL_EDP
#define eDPTX_FineTune_TU_En    0  //set 1 to auto fine tune TU value when scaler clk change
#define eDPTXColorDepth         3  //1:12bits ; 2:10bits; 3:8bits; 4:6bits
#define eDPTXAutoTest           0  //Auto Test for PHY CTS
#define eDPTXMSADelayLine       1  //Delay 1~8 Line after BE for TCON
#define eDPTX_total_ports 1 //numbers of use total ports : 1,2,4
#define eDPTXMODOnePort 0 // if MOD play in Only One eDPTX Port , Set 1 to support  it.
#define eDPTXMODUsePort 2 // 0 : port0, 1: port1, 2: port0&1
#define eDPTXHBR2PortEn 0 // set 1 when use MT9701 chip && eDPTX ouput in HBR2 path


#define eDPTXSWAP 0
#define eDPTXP0PNSWAP eDPTXSWAP  // Port#0  (ch8~ch11) PN swap
#define eDPTXP1PNSWAP eDPTXSWAP  // Port#1 (ch12~ch15) PN swap
#define eDPTXP0LaneSWAP eDPTXSWAP // Port#0 (ch8~ch11) lane swap
#define eDPTXP1LaneSWAP eDPTXSWAP // Port#1 (ch11~ch15) lane swap
#define eDPTXPortSWAP eDPTXSWAP // eDP Port#0 <-> Port#1 swap == LR SWAP
#define eDPTXAuxP0PNSWAP 0  // AuxPort#0  PN swap
#define eDPTXAuxP1PNSWAP 0  // AuxPort#1 PN swap
#define eDPTXAuxP2PNSWAP 0  // AuxPort#2  PN swap
#define eDPTXAuxP3PNSWAP 0  // AuxPort#3 PN swap
// //////// config ML<->AUX<->HPD Port Mapping ///////////////
#define eDP_SWING_DefaultValue                  0x2E   // 0x00~0x3F
#define eDP_PREMPHASIS_DefaultValue      0x00   // 0x00~0x07


#define eDPTXP0UseAUXPortNum 0 // 0~3
#define eDPTXP0UseHPDPortNum 0 // 0~1
#define eDPTXP1UseAUXPortNum 1 // 0~3
#define eDPTXP1UseHPDPortNum 1 // 0~1

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
#define eDPTXLinkRate               eDPTX_HBR       // eDPTX_RBR
#define eDPTXLaneCount            eDPTX_4Lanes   // eDPTX_2Lanes  //eDPTX_1Lanes
#define eDPTXDETECTHPD       1 // 1=enable TX detect HPD
#define eDPTXHPDUseGPIOGroupNum   eDPTXHPDUseGPIOGroup0 // 0=GPIO50~53 ; 1=GPIO54~57
#define eDPTXHPDPort0             eDPTXHPD_USE_GPIO56
#define eDPTXHPDPort1             eDPTXHPD_USE_GPIO57
////////////////////////////////////////////////////////////
#define eDPTX10Bits 0
////// choose either method#3a or method#3b ////////////////////
#define eDPTXEnASSR                        1     // method  #3a
#define eDPTXEnAFR                           1    // method  #3b
////// choose support No Aux HandShake Link Training  ///////////
#define eDPTX_NO_AUX_HANDSHAKE_LINK_TRAINING  0  //
////////////////////////////////////////////////////////////
#define eDPTx_Training_Swap 0
#define eDPTXEnPG 0
#define TG_PANAL_DEF 1                              //Timing Gen from panel define
////// choose support MCCS Cmd ////////////////////////////
#define eDPTxAuxMCCSEn 0
#define eDP_HDCP13    0
#endif

////////Select Panel Output Format/////////
#define PANEL_OUTPUT_FMT_LR     0
#define PANEL_OUTPUT_FMT_4B     0 // 1

#define PanelSwapPort       0//0xff
#define PanelSwapOddML      0
#define PanelSwapEvenML     0
#define PanelSwapOddRB      0
#define PanelSwapEvenRB     0
#define PanelSwapMLSB       0
#define PanelDClkDelay      8
#define PanelInvDE          0
#define PanelInvDClk        0
#define PanelInvHSync       0
#define PanelInvVSync       0

#define PANEL_SWAP_PN      0xFF
//==================================================================
#define LVDS_CH_A_SWAP              LVDS_CH_A
#define LVDS_CH_B_SWAP              LVDS_CH_E
#define LVDS_CH_C_SWAP              LVDS_CH_C
#define LVDS_CH_D_SWAP              LVDS_CH_G
#define LVDS_CH_E_SWAP              LVDS_CH_B
#define LVDS_CH_F_SWAP              LVDS_CH_F
#define LVDS_CH_G_SWAP              LVDS_CH_D
#define LVDS_CH_H_SWAP              LVDS_CH_H
//==================================================================


#define _DISABLE_AUTO_OUTPUT_VSYNC_

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent          1 // DE signal current
#define PanelOddDataCurrent     1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current
#define PanelOnTiming0          2 // time between 3.3V - 12V
#define PanelOnTiming1          25  // time between panel & data while turn on power
#define PanelOnTiming2          550 // time between data & back light while turn on power  //20050701
#define PanelOffTiming1         100 // time between back light & data while turn off power
#define PanelOffTiming2         10//25 // time between data & panel while turn off power
#define PanelOffTiming0         0 // time between 12V - 3.3V

#if 1 // 2048x2180@60 268.7M AUO
#define PanelHSyncWidth        32 //19 //38//22//24
#define PanelHSyncBackPorch     40 // 41
#define PanelVSyncBackPorch     11  //21 // 46
#define PanelHStart         (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart         (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth          1920//1920//2560//3440
#define PanelHeight         1080//1080//1440
#define PanelHTotal         2250//2200//2800//3600
#define PanelVTotal         1125//1100//1460
#define PanelVSyncWidth       PanelVTotal - PanelHeight - PanelVSyncBackPorch - 10   // 3

#elif 0 // 2000x2222@60 (533/2)M LG
#define PanelHSyncWidth        20 //19 //38//22//24
#define PanelHSyncBackPorch     30 // 41
#define PanelVSyncWidth         6 // 3
#define PanelVSyncBackPorch     20 //11 //21 // 46
#define PanelHStart         (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart         (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth          (1920*2)//3840
#define PanelHeight         2160
#define PanelHTotal         (2000*2)//(2080*2) //(2200*2) //4400
#define PanelVTotal         2222 //2200 //2250
#endif
#define PanelMaxHTotal      2400 //2600//3000//4000
#define PanelMinHTotal      2100 //2000//2200//3200
#define PanelMaxVTotal      1300//1120//1480
#define PanelMinVTotal      1100//1040//1420
#define PanelDCLK           148//(61*4) //(70*8) // (74*8)
#define PanelMaxDCLK        196//(66*4) //(72*8) // (76*8)
#define PanelMinDCLK        117//(59*4) //(66*8) // (69*8)
#define HV_OUTPUT_TYPE      HSRM_B

#define PanelVfreq          (600)
#define PanelMaxVfreq       (600+20)
#define PanelMinVfreq       (600-20) // 50Hz

#if PANEL_EDP
// ##### eDPTiming index ###############################
#define eDP480P 0
#define eDP720P 1
#define eDP1080P 2
#define eDP2K2K 3
#define eDP2K2KR 4
#define eDPanelDefine 5
// ##### eDPTiming define ###############################
#define eDPTiming eDPanelDefine // eDP2K2KR // eDP2K2K  // eDP1080P //eDP720P //eDP480P
// ##### eDPTiming MSA ###############################
#if (eDPTiming==eDPanelDefine)
#define eDPTXMSA_Htotal      g_sPnlInfo.sPnlTiming.u16Htt // Htotal
#define eDPTXMSA_Hwidth    g_sPnlInfo.sPnlTiming.u16Width // Hactive
#define eDPTXMSA_HStart     g_sPnlInfo.sPnlTiming.u16HSyncWidth+g_sPnlInfo.sPnlTiming.u16HSyncBP // HStart  == HSW+HBACKPORCH
#define eDPTXMSA_HSP         1           // HSP   [15]=Polarity  set 0 or 1
#define eDPTXMSA_HSW        g_sPnlInfo.sPnlTiming.u16HSyncWidth // Hsw 20  [15]=Polarity
#define eDPTXMSA_Vtotal      g_sPnlInfo.sPnlTiming.u16Vtt // Vtotal
#define eDPTXMSA_VStart     g_sPnlInfo.sPnlTiming.u16VSyncWidth+g_sPnlInfo.sPnlTiming.u16VSyncBP // Vstart == VSW+VBACKPORCH
#define eDPTXMSA_VSP         0           // VSP   [15]=Polarity  set 0 or 1
#define eDPTXMSA_VSW       g_sPnlInfo.sPnlTiming.u16VSyncWidth // Vsw   [15]=Polarity
#define eDPTXMSA_Vheight   g_sPnlInfo.sPnlTiming.u16Height // Vactive
#elif (eDPTiming==eDP2K2KR)
#define eDPTXMSA_Htotal      0x0800 // Htotal 2048
#define eDPTXMSA_HStart     0x0028 // HStart 40  == HSW+HBACKPORCH
#define eDPTXMSA_HSP         1           // HSP   [15]=Polarity
#define eDPTXMSA_HSW        0x0014 // Hsw 20  [15]=Polarity
#define eDPTXMSA_Vtotal      0x0898 // Vtotal  2200
#define eDPTXMSA_VStart     0x0008 // Vstart  8 == VSW+VBACKPORCH
#define eDPTXMSA_VSP         0           // VSP   [15]=Polarity
#define eDPTXMSA_VSW        0x0005 // Vsw   [15]=Polarity
#define eDPTXMSA_Hwidth    0x0780 // Hactive 1920
#define eDPTXMSA_Vheight  0x0870 // Vactive 2160
#elif  (eDPTiming==eDP2K2K)
#define eDPTXMSA_Htotal      0x0820 // Htotal 2080
#define eDPTXMSA_HStart     0x0058 // HStart 88 == HSW+HBACKPORCH
#define eDPTXMSA_HSP         1           // HSP   [15]=Polarity
#define eDPTXMSA_HSW        0x002C // Hsw 44  [15]=Polarity
#define eDPTXMSA_Vtotal      0x08CA // Vtotal  2250
#define eDPTXMSA_VStart     0x0008 // Vstart  8 == VSW+VBACKPORCH
#define eDPTXMSA_VSP         0           // VSP   [15]=Polarity
#define eDPTXMSA_VSW        0x0005 // Vsw   [15]=Polarity
#define eDPTXMSA_Hwidth    0x0780 // Hactive 1920
#define eDPTXMSA_Vheight  0x0870 // Vactive 2160
#elif (eDPTiming==eDP1080P)
#define eDPTXMSA_Htotal      0x0898 // Htotal 2220
#define eDPTXMSA_HStart     0x0058 // HStart 88 == HSW+HBACKPORCH
#define eDPTXMSA_HSP         1           // HSP   [15]=Polarity
#define eDPTXMSA_HSW        0x002C // Hsw 44  [15]=Polarity
#define eDPTXMSA_Vtotal      0x0465 // Vtotal  1125
#define eDPTXMSA_VStart     0x0010 // Vstart  16 == VSW+VBACKPORCH
#define eDPTXMSA_VSP         0           // VSP   [15]=Polarity
#define eDPTXMSA_VSW        0x0005 // Vsw   [15]=Polarity
#define eDPTXMSA_Hwidth    0x0780 // Hactive 1920
#define eDPTXMSA_Vheight  0x0438 // Vactive 1080
#elif (eDPTiming==eDP720P)
#define eDPTXMSA_Htotal      0x0672 // Htotal 1650
#define eDPTXMSA_HStart     0x006E // HStart 110 == HSW+HBACKPORCH
#define eDPTXMSA_HSP         1           // HSP   [15]=Polarity
#define eDPTXMSA_HSW        0x0028 // Hsw 40  [15]=Polarity
#define eDPTXMSA_Vtotal      0x02EE // Vtotal  750
#define eDPTXMSA_VStart     0x0008 // Vstart  8 == VSW+VBACKPORCH
#define eDPTXMSA_VSP         0           // VSP   [15]=Polarity
#define eDPTXMSA_VSW        0x0005 // Vsw   [15]=Polarity
#define eDPTXMSA_Hwidth    0x0500 // Hactive 1280
#define eDPTXMSA_Vheight  0x02D0 // Vactive  720
#elif (eDPTiming==eDP480P)
#define eDPTXMSA_Htotal      0x035A // Htotal 858
#define eDPTXMSA_HStart     0x0010 // HStart 16 == HSW+HBACKPORCH
#define eDPTXMSA_HSP         1           // HSP   [15]=Polarity
#define eDPTXMSA_HSW        0x003E // Hsw 62  [15]=Polarity
#define eDPTXMSA_Vtotal      0x020D // Vtotal  525
#define eDPTXMSA_VStart     0x0009 // Vstart  9 == VSW+VBACKPORCH
#define eDPTXMSA_VSP         1           // VSP   [15]=Polarity
#define eDPTXMSA_VSW        0x0006 // Vsw   [15]=Polarity
#define eDPTXMSA_Hwidth    0x01E0 // Hactive 480
#define eDPTXMSA_Vheight  0x02D0 // Vactive  720
#endif
#define eDPTXMSA_HB         (eDPTXMSA_Htotal)-(eDPTXMSA_Hwidth)  // HB   Blanking
#define eDPTXMSA_VB         (eDPTXMSA_Vtotal)-(eDPTXMSA_Vheight) // VB   Blanking
#define eDPTXMSA_HFP         (eDPTXMSA_HB)-(g_sPnlInfo.sPnlTiming.u16HStart)  // H Front porch
#define eDPTXMSA_VFP         (eDPTXMSA_VB)-(g_sPnlInfo.sPnlTiming.u16VStart) // VFront porch
#define eDPTXMSA_HBP         g_sPnlInfo.sPnlTiming.u16HSyncBP  // H  Back porch
#define eDPTXMSA_VBP         g_sPnlInfo.sPnlTiming.u16VSyncBP // V Back porch
#if eDPTX10Bits
#define eDPTXMSAColorMISC BIT6  // [7:5]=  000=6bits  / 001=8bits / 010 = 10bits  / 011= 12bits  /  100=16bits
#else
#define eDPTXMSAColorMISC BIT5  // [7:5]=  000=6bits  / 001=8bits / 010 = 10bits  / 011= 12bits  /  100=16bits
#endif
#endif
//=================================================================
// TCON  setting for RSDS
// TCON  setting for RSDS
#define TCON_Format1    0x1 // OFC1: control polarity & inversion
#define ESP_AfterData   0 // Even Start Pulse after Data
#define ESP_Offset      1 // Even start pulse position
#define OSP_AfterData   0 // Odd Start Pulse after Data
#define OSP_Offset      1 // Odd start pulse position
// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelOESPCurrent  2 // OSP/ESP drive current
#define PanelOEGCurrent   2 // OPOL/EPOL/GPOL drive current
#define PanelEINVCurrent  0 // EINV drive current
#define PanelOINVCurrent  0 // OINV drive current
#define OuputFormat   (BIT3|BIT5)//Enable Auto Toggle GPO0 & data invert
// POL
#define GPO0_VStart     0x000
#define GPO0_VEnd       0x000
#define GPO0_HStart     0x186 //0x068
#define GPO0_HEnd       0x186 //0x222
#define GPO0_Control    0x02 //0x02 //0x82
//LP
#define GPO1_VStart     0
#define GPO1_VEnd       0
#define GPO1_HStart     0x406 //0x407
#define GPO1_HEnd       0x41b //0x428
#define GPO1_Control    0x00
// STV
#define GPO2_VStart     0x000
#define GPO2_VEnd       0x001
#define GPO2_HStart     0x166 //x170
#define GPO2_HEnd       0x166 //x170
#define GPO2_Control    0x00
// CLKV
#define GPO3_VStart     0
#define GPO3_VEnd       0x00 //0x300
#define GPO3_HStart     0x166 //0x173
#define GPO3_HEnd       0x406 //0x407
#define GPO3_Control    0x01
// OE
#define GPO4_VStart     0
#define GPO4_VEnd       0
#define GPO4_HStart     0x384 //0x357
#define GPO4_HEnd       0x442 //0x427
#define GPO4_Control    0x00
//PVS / FSYNC
#define GPO9_VStart     0 //0x002
#define GPO9_VEnd       0 //0x306
#define GPO9_HStart     0 //0x00A
#define GPO9_HEnd       0 //0x005
#define GPO9_Control    0 //0x08
//HSYNC
#define GPOA_VStart     0
#define GPOA_VEnd       0
#define GPOA_HStart     0
#define GPOA_HEnd       0
#define GPOA_Control    0

//=================================================================
// Color Temp
#define DefCool_RedColor            108
#define DefCool_GreenColor          128
#define DefCool_BlueColor           128
#define DefNormal_RedColor          118
#define DefNormal_GreenColor        128
#define DefNormal_BlueColor         128
#define DefWarm_RedColor            128
#define DefWarm_GreenColor      128
#define DefWarm_BlueColor           128
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor
//==================================================================
//LED Backlight Control
#if 0 // power measurement temply
#define LED_CH_0    1
#define LED_CH_1    1
#define LED_CH_2    1
#define LED_CH_3    1
#define LEDBL_PanelDefaultCurrent           110//50//5o for LG set , 110            //need set by panel spec
#define PanelLightBarDefaultVoltage         40          //need set by panel spec
#define PanelLightBarMaximumVoltage     52//52          //need set by panel spec
#else // product panel
#define LED_CH_0    1
#define LED_CH_1    1
#define LED_CH_2    1
#define LED_CH_3    1
#define LEDBL_PanelDefaultCurrent           50//110         //need set by panel spec
#define PanelLightBarDefaultVoltage         44          //need set by panel spec
#define PanelLightBarMaximumVoltage     52          //need set by panel spec
#endif

#endif


