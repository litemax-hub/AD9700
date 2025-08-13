#ifndef __PANEL_AU170N5E_
#define __PANEL_AU170N5E_
#define PanelName   "AU170N5E"

#define PanelDither         6
#define DITHER_COEFF        0x2D
#define DITHER_METHOD       0x42

#define PanelTTL            0
#define PanelTCON           0
#define PanelLVDS           0xFF
#define PanelRSDS           0
#define PanelMiniLVDS3      0

#define ShortLineCheck      0
#define LVDS_TIMode         0xFF
#define PanelDualPort       0
#define PANEL_TTL      0
#define PANEL_LVDS_1CH      0
#define PANEL_LVDS_2CH      0// 0
#define PANEL_LVDS_4CH      0
#define PANEL_LVDS_8CH      0
#define PANEL_VBY1_1CH_8Bit      0
#define PANEL_VBY1_1CH_10Bit      0
#define PANEL_VBY1_2CH_8Bit      0
#define PANEL_VBY1_2CH_10Bit      0
#define PANEL_VBY1_4CH_8Bit      0
#define PANEL_VBY1_4CH_10Bit      0
#define PANEL_VBY1_8CH_8Bit      0
#define PANEL_VBY1_8CH_10Bit      0
#define PANEL_VBY1()       ((PANEL_VBY1_1CH_8Bit)  ||  \
                            (PANEL_VBY1_1CH_10Bit) ||  \
                            (PANEL_VBY1_2CH_8Bit)  ||  \
                            (PANEL_VBY1_2CH_10Bit) ||  \
                            (PANEL_VBY1_4CH_8Bit)  ||  \
                            (PANEL_VBY1_4CH_10Bit) ||  \
                            (PANEL_VBY1_8CH_8Bit)  ||  \
                            (PANEL_VBY1_8CH_10Bit) )
#define PANEL_EDP      1
#if 1//PANEL_EDP
#define eDPTX_HBR3    0x1E
#define eDPTX_HBR25    0x19
#define eDPTX_HBR2 0x14
#define eDPTX_HBR   0x0A
#define eDPTX_RBR   0x06
#define eDPTX_4Lanes  4
#define eDPTX_2Lanes  2
#define eDPTX_1Lanes  1
#define eDPTX_Port0   BIT0
#define eDPTX_Port1   BIT1
#define eDPTX_Port2   BIT2
#define eDPTX_Port3   BIT3

typedef enum
{
    eDPTXHPD_USE_GPIO54=0,
    eDPTXHPD_USE_GPIO55,
    eDPTXHPD_USE_GPIO56,
    eDPTXHPD_USE_GPIO57,
}eDPTXHPD_USE_GPIO_GP1_TYP;

////////////////////////////////////////////////////////////
#define eDPTX_port_select eDPTX_Port0//|eDPTX_Port1|eDPTX_Port2|eDPTX_Port3
#define eDPTX_total_ports 1 //numbers of use total ports : 1,2,4
#define eDPTX_one_port_select 0 //use port 0/1/2/3
#define eDPTX_two_port_select 0 //0: use port0&1; else:use port 2&3
#define eDPTX_four_port_select 0 //0: use port0&1&2&3
#define eDPTx_MFT_mode 11//7 //CASE1~10
#define eDPTx_use_4P_MAC_count 1 //2:use 4P MAC0 and MAC1


#if (eDPTX_total_ports==1)
#define eDPTX_port_num_select eDPTX_one_port_select
#elif (eDPTX_total_ports==2)
#define eDPTX_port_num_select eDPTX_two_port_select
#elif (eDPTX_total_ports==4)
#define eDPTX_port_num_select eDPTX_four_port_select
#endif
#define eDPTX_FineTune_TU_En    0 //set 1 to auto fine tune TU value when scaler clk change
#define eDPTX_4PEngine_FreeSyncEN           0 //set 1 for 4P engine Freesync panel
////////////////////////////////////////////////////////////
#define eDP_HDCP13          0
#define eDP_SWING_DefaultValue                  0x2E   // 0x00~0x3F
#define eDP_PREMPHASIS_DefaultValue      0x00   // 0x00~0x07
////////////////////////////////////////////////////////////
#define eDPTXLinkRate               eDPTX_HBR2       // eDPTX_RBR
#define eDPTXLaneCount            eDPTX_4Lanes   // eDPTX_2Lanes  //eDPTX_1Lanes
#define eDPTXDETECTHPD       1 // 1=enable TX detect HPD
////////////////////////////////////////////////////////////
#define eDPTXColorDepth 2 //1:12bits ; 2:10bits; 3:8bits; 4:6bits
////// choose either method#3a or method#3b ////////////////////
#define eDPTXEnASSR                        0     // method  #3a
#define eDPTXEnAFR                           1    // method  #3b
////// choose support No Aux HandShake Link Training  ///////////
#define eDPTX_NO_AUX_HANDSHAKE_LINK_TRAINING  0  //
///////////////////////////////////////////////////////////
#define eDPTXEnPG 0
#define eDPTXMODPhyCalEn 0
////// choose support MCCS Cmd ////////////////////////////
#define eDPTxAuxMCCSEn 0
#define eDPTXMODOnePort 0
#define eDPTXMODUsePort 2 // 0 : port0, 1: port1, 2: port0&1
#define eDPTXHBR2PortEn 0 // set 1 when use MT9701 chip && eDPTX ouput in HBR2 path

#define eDPTX_Print_En 1

#define eDPTXSWAP 0
#define eDPTXP0PNSWAP eDPTXSWAP  // Port#0  (ch8~ch11) PN swap
#define eDPTXP1PNSWAP eDPTXSWAP  // Port#1 (ch12~ch15) PN swap
#define eDPTXP0LaneSWAP eDPTXSWAP // Port#0 (ch8~ch11) lane swap
#define eDPTXP1LaneSWAP eDPTXSWAP // Port#1 (ch11~ch15) lane swap

#define eDPTXHBR2PNSWAP 0    // HBR2Port#  (ch0~ch3) PN swap
#define eDPTXHBR2LaneSWAP 0 // HBR2Port# (ch0~ch3) lane swap
#define eDPTXLaneSkewSWAP 0 // (ch0~ch3) lane skew swap
#define eDPTXAuxP0PNSWAP 0  // AuxPort#0  PN swap
#define eDPTXAuxP1PNSWAP 0  // AuxPort#1 PN swap
#define eDPTXAuxP2PNSWAP 0  // AuxPort#2  PN swap
#define eDPTXAuxP3PNSWAP 0  // AuxPort#3 PN swap
#define eDPTXHPDUseGPIOGroupNum   eDPTXHPDUseGPIOGroup1 // 0=GPIO50~53 ; 1=GPIO54~57
#define eDPTXPortSWAP 0
//#define eDPTXHPDUseGPIOGroupNum   eDPTXHPDUseGPIOGroup0 // 0=GPIO50~53 ; 1=GPIO54~57
#define eDPTXHPDPort0             eDPTXHPD_USE_GPIO54
#define eDPTXHPDPort1             eDPTXHPD_USE_GPIO55  // if  project only use One HPD port => please define HPDPort1 = HPDPort0
#define eDPTXHPDPort2             eDPTXHPD_USE_GPIO56
#define eDPTXHPDPort3             eDPTXHPD_USE_GPIO57

#define eDPTXMSA_Htotal      PanelHTotal // Htotal
#define eDPTXMSA_Hwidth    PanelWidth // Hactive
#define eDPTXMSA_HStart     PanelHSyncWidth+PanelHSyncBackPorch // HStart  == HSW+HBACKPORCH
#define eDPTXMSA_HSP         1           // HSP   [15]=Polarity  set 0 or 1
#define eDPTXMSA_HSW        PanelHSyncWidth // Hsw 20  [15]=Polarity
#define eDPTXMSA_Vtotal      PanelVTotal // Vtotal
#define eDPTXMSA_VStart     PanelVSyncWidth+PanelVSyncBackPorch // Vstart == VSW+VBACKPORCH
#define eDPTXMSA_VSP         0           // VSP   [15]=Polarity  set 0 or 1
#define eDPTXMSA_VSW       PanelVSyncWidth // Vsw   [15]=Polarity
#define eDPTXMSA_Vheight   PanelHeight // Vactive
#define eDPTXMSA_HB         (eDPTXMSA_Htotal)-(eDPTXMSA_Hwidth)  // HB   Blanking
#define eDPTXMSA_VB         (eDPTXMSA_Vtotal)-(eDPTXMSA_Vheight) // VB   Blanking
#define eDPTXMSA_HFP         (eDPTXMSA_HB)-(PanelHStart)  // H Front porch
#define eDPTXMSA_VFP         (eDPTXMSA_VB)-(PanelVStart) // VFront porch
#define eDPTXMSA_HBP         PanelHSyncBackPorch  // H  Back porch
#define eDPTXMSA_VBP         PanelVSyncBackPorch // V Back porch
#endif
////////Select Panel Output Format/////////
#define PANEL_OUTPUT_FMT_LR     0
#define PANEL_OUTPUT_FMT_4B     0

#define PanelSwapPort     0//0xff
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

#define PANEL_SWAP_PN       0xFF
#define eDPTXAutoTest   0
#define _DISABLE_AUTO_OUTPUT_VSYNC_  //for FPGA 440 environment

//==================================================================
#define LVDS_CH_A_SWAP              LVDS_CH_A
#define LVDS_CH_B_SWAP              LVDS_CH_C
#define LVDS_CH_C_SWAP              LVDS_CH_B
#define LVDS_CH_D_SWAP              LVDS_CH_D
#define LVDS_CH_E_SWAP              LVDS_CH_E
#define LVDS_CH_F_SWAP              LVDS_CH_G
#define LVDS_CH_G_SWAP              LVDS_CH_F
#define LVDS_CH_H_SWAP              LVDS_CH_H
//==================================================================

//==================================================================

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent        1 // Dclk current
#define PanelDECurrent          1 // DE signal current
#define PanelOddDataCurrent     1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current


#define PanelOnTiming1          30  // time between panel & data while turn on power
#define PanelOnTiming2          100// time between data & back light while turn on power
#define PanelOffTiming1         20 // time between back light & data while turn off power
#define PanelOffTiming2         20//16  // time between data & panel while turn off power

#if 0 // 2048x2180@60 268.7M AUO
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
#else
#define PanelHSyncWidth   64//22//24
#define PanelHSyncBackPorch 68
#define PanelVSyncWidth         3
#define PanelVSyncBackPorch  33
#define PanelHStart   (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart   (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth    720//1920
#define PanelHeight   576//1080
#define PanelHTotal   864
#define PanelVTotal   625
#define PanelMaxHTotal     1000
#define PanelMinHTotal     864
#define PanelMaxVTotal     920
#define PanelMinVTotal    625
#define PanelDCLK            27 // 74.25
#define PanelMaxDCLK      40 // 98
#define PanelMinDCLK        13// 58.54
#define HV_OUTPUT_TYPE          HSRM_B

// FRC coding test panel
#define PanelVfreq          (500)
#define PanelMaxVfreq   610//(750+20) // 75Hz
#define PanelMinVfreq   400//(500-20) // 50Hz

#endif


//=================================================================
// TCON  setting for LVDS
//PVS / FSYNC
#define GPO9_VStart     0x7
#define GPO9_VEnd       0x705
#define GPO9_HStart     0x0
#define GPO9_HEnd       0x0
#define GPO9_Control    0x7

//HSYNC
#define GPOA_VStart     0x006
#define GPOA_VEnd       0x408
#define GPOA_HStart     0
#define GPOA_HEnd       0
#define GPOA_Control    0x4

//#define PanelVfreq      (600)

//Color Temp
#define DefCool_RedColor        117//114//111
#define DefCool_GreenColor      110//114
#define DefCool_BlueColor       127//130//117
#define DefNormal_RedColor      126//127
#define DefNormal_GreenColor    121//125
#define DefNormal_BlueColor     128//127
#define DefWarm_RedColor        130
#define DefWarm_GreenColor      126//132
#define DefWarm_BlueColor       128//130
#define DefsRGB_RedColor        DefWarm_RedColor
#define DefsRGB_GreenColor      DefWarm_GreenColor
#define DefsRGB_BlueColor       DefWarm_BlueColor
#endif
