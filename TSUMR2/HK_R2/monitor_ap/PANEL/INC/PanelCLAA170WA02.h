// for CPT RSDS 17"
#define PanelName   "CLAA170WA02"
#define PanelDither     6

#define PanelTTL        0
#define PanelTCON       0
#define PanelLVDS       0
#define PanelRSDS       0xFF

#define LVDS_TIMode     0x00
#define PanelDualPort       0//0xFF
#define PanelSwapPort       0//0xFF
#define PanelSwapOddML      0//0xFF
#define PanelSwapEvenML       0
#define PanelSwapOddRB      0xFF
#define PanelSwapEvenRB       0xFF
#define PanelSwapMLSB       0
#define PanelSwapLCPS            0

#define PanelDClkDelay      8
#define PanelInvDClk        0
#define PanelInvDE      0
#define PanelInvHSync       0
#define PanelInvVSync       0

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA
#define PanelDCLKCurrent    1 // Dclk current
#define PanelDECurrent      1 // DE signal current
#define PanelOddDataCurrent 1 // Odd data current
#define PanelEvenDataCurrent    1 // Even data current

// 070125 modify period of PanelOnTiming1 and PanelOnTiming2
#define PanelOnTiming0      15//20 // time between 3.3v & 12v while turn on power
#define PanelOnTiming1      25//50//20//15//20//Delay1ms * 2 already. 60//10 //80 // time between panel & data while turn on power
#define PanelOnTiming2      200//180 time between data & back light while turn on power 
//#define PanelOnTiming3        100
#define PanelOffTiming0     10//20 // time between 3.3v & 12v while turn on power
#define PanelOffTiming1     100//10//80//Delay1ms * 2 already. 60//200 // time between back light & data while turn off power 
#define PanelOffTiming2     25//10//80//Delay1ms * 2 already. 60//10 // time between data & panel while turn off power 
//#define PanelOffTiming3       10

#define PanelHSyncWidth        32
#define PanelHSyncBackPorch 40

#define PanelVSyncWidth        2
#define PanelVSyncBackPorch 34

#define PanelHStart     (PanelHSyncWidth+PanelHSyncBackPorch)
#define PanelVStart     (PanelVSyncWidth+PanelVSyncBackPorch)
#define PanelWidth      1280
#define PanelHeight     720
#define PanelHTotal     1440
#define PanelVTotal     758

#define PanelMaxHTotal      1600
#define PanelMinHTotal      1440//1520//1400//1340
#define PanelMaxVTotal      770
#define PanelMinVTotal      730
#define PanelDCLK           65
#define PanelMaxDCLK        91
#define PanelMinDCLK        54

//=================================================================
#define TCON_Format1    0x1 // OFC1: control polarity & inversion

#define ESP_AfterData   0 // Even Start Pulse after Data
#define ESP_Offset  1 // Even start pulse position
#define OSP_AfterData   0 // Odd Start Pulse after Data
#define OSP_Offset  1 // Odd start pulse position

// driving current setting 0==>4mA, 1==>6mA, 2==>8mA ,3==>12mA

#define OuputFormat     (DATI_B|G0AT_B)//Enable Auto Toggle GPO0 & data invert

#define PanelOESPCurrent    2 // OSP/ESP drive current
#define PanelOEGCurrent     2 // OPOL/EPOL/GPOL drive current
#define PanelEINVCurrent    0 // EINV drive current
#define PanelOINVCurrent    0 // OINV drive current

// POL
#define GPO0_VStart     0x000
#define GPO0_VEnd       0x2D5
#define GPO0_HStart     0x500
#define GPO0_HEnd       0x500
#define GPO0_Control        0x12
//LP
#define GPO1_VStart     0
#define GPO1_VEnd       0x2D1
#define GPO1_HStart     0x501//407
#define GPO1_HEnd       0x513//0x428 j
#define GPO1_Control        0x00//0x19//0x00 j

// STV
#define GPO2_VStart     0x000
#define GPO2_VEnd       0x01 //0x002 j
#define GPO2_HStart     0x282//x170
#define GPO2_HEnd       0x282//x170
#define GPO2_Control        0x00

// CLKV
#define GPO3_VStart     0
#define GPO3_VEnd       0//x300
#define GPO3_HStart     0x281//173
#define GPO3_HEnd       0x501//407
#define GPO3_Control        0x01

// OE
#define GPO4_VStart     0
#define GPO4_VEnd       0x2D1
#define GPO4_HStart     0x541//357
#define GPO4_HEnd       0x47F//427
#define GPO4_Control        0x01

//PVS / FSYNC
#define GPO9_VStart     0//x002
#define GPO9_VEnd       0//x306
#define GPO9_HStart     0//x00A
#define GPO9_HEnd       0//x005
#define GPO9_Control        1//0x08
//HSYNC
#define GPOA_VStart     0
#define GPOA_VEnd       0
#define GPOA_HStart     0
#define GPOA_HEnd       0
#define GPOA_Control        1

#define HV_OUTPUT_TYPE          HSRM_B

//=================================================================
//Color Temp
#define DefCool_RedColor            125
#define DefCool_GreenColor          134
#define DefCool_BlueColor           154
#define DefNormal_RedColor          132
#define DefNormal_GreenColor        144
#define DefNormal_BlueColor         154
#define DefWarm_RedColor            136
#define DefWarm_GreenColor      154
#define DefWarm_BlueColor           141
#define DefsRGB_RedColor      DefWarm_RedColor
#define DefsRGB_GreenColor  DefWarm_GreenColor
#define DefsRGB_BlueColor     DefWarm_BlueColor