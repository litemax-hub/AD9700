
#include <math.h>
#include "types.h"
#include "board.h"
#include "Panel.h"
//#include "msreg.h"
#include "Ms_rwreg.h"
#include "NVRam.h"
#include "Mode.h"
#include "UserPrefDef.h"
#include "Global.h"
#include "misc.h"
#include "menudef.h"
#include "menufunc.h"
#include "Debug.h"
#include "UserPref.h"
#include "msOSD.h"
#include "msDPS_Handler.h"
#include "msDPS_Setting.h"
#include "msDPS_RegSetting.h"
#include "msACE.h"
#include "Adjust.h"
#include "MsDLC.h"
//#include "LedControl.h"
//#include "debugmsg.h"
#include "Common.h"


//===================================================================================================================================================================:
//#include "msDPS.h"
//===================================================================================================================================================================:
#if ENABLE_DPS
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///// Y-mapping Table ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
BYTE code DPS_Lumi_Table[17][11] =
{
/*
 {255,235,215,195,176,156,136,116, 96, 76, 57,},
 {208,192,176,160,144,127,111, 95, 79, 62, 46,},
 {173,159,145,132,119,106, 92, 79, 65, 51, 39,},
 {143,131,120,109, 98, 87, 76, 65, 54, 43, 32,},
 {118,108, 99, 90, 81, 72, 63, 54, 44, 35, 26,},
 { 96, 88, 81, 74, 66, 59, 51, 44, 36, 29, 22,},
 { 76, 70, 64, 58, 52, 47, 41, 35, 29, 23, 17,},
 { 60, 55, 50, 46, 41, 36, 32, 27, 23, 18, 13,},
 { 45, 42, 38, 35, 31, 28, 24, 21, 17, 14, 10,},
 { 34, 31, 29, 26, 23, 21, 18, 16, 13, 10,  8,},
 { 25, 23, 21, 19, 17, 15, 13, 11,  9,  7,  6,},
 { 17, 15, 14, 13, 11, 10,  9,  8,  6,  5,  4,},
 { 10,  9,  9,  8,  7,  6,  5,  5,  4,  3,  2,},
 {  6,  5,  5,  4,  4,  3,  3,  3,  2,  2,  1,},
 {  2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,},
 {  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,},
 {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,},
*/
 {255,229,205,179,155,129,104, 78, 53, 26,  1,},
 {223,201,179,157,135,113, 91, 68, 46, 23,  1,},
 {191,172,154,134,116, 97, 78, 58, 40, 20,  1,},
 {164,147,132,115, 99, 83, 67, 50, 34, 16,  1,},
 {138,124,111, 97, 83, 70, 56, 42, 28, 14,  1,},
 {113,102, 91, 79, 68, 57, 46, 34, 23, 12,  0,},
 { 93, 84, 75, 66, 57, 47, 38, 28, 19, 10,  0,},
 { 74, 67, 60, 52, 45, 37, 30, 23, 15,  8,  0,},
 { 58, 52, 47, 41, 35, 29, 24, 18, 12,  6,  0,},
 { 43, 39, 35, 30, 26, 22, 17, 13,  9,  4,  0,},
 { 31, 28, 25, 22, 19, 15, 12,  9,  6,  3,  0,},
 { 22, 20, 17, 15, 13, 11,  9,  7,  4,  2,  0,},
 { 14, 13, 11, 10,  9,  7,  6,  4,  3,  1,  0,},
 {  8,  7,  6,  5,  5,  4,  3,  2,  2,  1,  0,},
 {  3,  3,  2,  2,  2,  2,  1,  1,  1,  0,  0,},
 {  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,},
 {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,},
};

//===================================================================================================================================================================:
// XDATA variable Share with DLC (back up)
//===================================================================================================================================================================:
#if !SHARE_DLC_XDATA
///// ISR //////////////
xdata WORD  ISR_LumaHistogram32H[32]; // >>6
xdata WORD  ISR_HistogramTotalPix; // >>6
xdata WORD  ISR_HistogramTotalLuma; // >>14
xdata BYTE  ISR_HistogramMax;
xdata BYTE  ISR_HistogramMin;
#endif
xdata WORD  REG_DPS_UserBrightContlGain;// 0~100: 0~100%
//DPS_AggrPowerSaveMode_EN
xdata WORD  REG_DPS_UserBrightLimitGain;
xdata WORD  REG_DPS_AggressiveYgainGain;//  256   // 0~256: 0~100%
xdata WORD  REG_DPS_AggressivePdutyGain;//  256   // 0~256: 0~100%
xdata WORD  REG_DPS_AggressiveGainRange;//  240   // apply AggressiveGain when ImgYmax < Range

xdata BYTE  REG_DPS_YOUT_ENABLE; // Color step from 0~255 to 16~235
xdata BYTE  REG_DPS_BW_MODE_EN;
xdata BYTE  REG_DPS_BW_MODE_Gain;
xdata BOOL  REG_DPS_RUN_Flag =0;
//#define ucWin DLCStatus

///// Global Variables ////////////////////////////////////////////////////////////
///// ISR /////////////////////////////////////////////////////////////////////////
xdata BYTE  ISR_preLoopCount = 0;
xdata BYTE  ISR_curLoopCount = 0;
///// DPS /////////////////////////////////////////////////////////////////////////
xdata BYTE  ucWin = 0;
xdata BYTE  Flag_DPS_Latch_Busy = 0;
xdata WORD  CurYgain = 0x400;
xdata BYTE  CurBacklight = 0xFF;
#define DPS_UserContrastControl_EN 0
#if DPS_YminForWindowSave_EN
xdata BYTE  REG_DPS_PowerLevel;  //0~FF
#endif
#if DPS_HistPseuMax_MOD1_EN
xdata BYTE  REG_MouseCount;
#endif
xdata WORD u16DPSContrast;
#if 0
void msAdjustPCContrastXvycc ( BYTE bScalerWin, WORD uwContrast )
{
    bScalerWin = bScalerWin;
    #ifdef _ML_ENABLE_
    msML_WaitReady();
    msML_WriteByte(ML_MODE_NORMAL, REG_ADDR_SC_BANK, 0x08 ); //alpha source  // sc8_82
    msML_WriteByte(ML_MODE_NORMAL, REG_ADDR_SC_COLOR_MATRIX_MAIN, uwContrast&0xFF ); //alpha source
    msML_WriteByte(ML_MODE_NORMAL, REG_ADDR_SC_COLOR_MATRIX_MAIN+1, uwContrast>>8 ); //alpha source
    msML_WriteByte(ML_MODE_NORMAL, REG_ADDR_SC_COLOR_MATRIX_MAIN+8, uwContrast&0xFF ); //alpha source
    msML_WriteByte(ML_MODE_NORMAL, REG_ADDR_SC_COLOR_MATRIX_MAIN+9, uwContrast>>8 ); //alpha source
    msML_WriteByte(ML_MODE_NORMAL, REG_ADDR_SC_COLOR_MATRIX_MAIN+16, uwContrast&0xFF ); //alpha source
    msML_WriteByte(ML_MODE_NORMAL, REG_ADDR_SC_COLOR_MATRIX_MAIN+17, uwContrast>>8 ); //alpha source /
    msML_Trigger(ML_TRIG_OUT_VDE_END);
    #else
        {
    BYTE xdata ucBank;
    WORD xdata uwAddr;
    ucBank = old_msReadByte ( SC0_00 );
#if CHIP_ID == CHIP_TSUMY || CHIP_ID == CHIP_TSUMB
    old_scWriteByte(SC0_00, 0x03);
#elif CHIP_ID == CHIP_TSUMV || CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUM2 || \
	CHIP_ID == CHIP_TSUMC || CHIP_ID == CHIP_TSUMD || CHIP_ID == CHIP_TSUMJ || CHIP_ID == CHIP_TSUM9
    old_scWriteByte(SC0_00, 0x08);
#endif
    if ( bScalerWin == MAIN_WINDOW )
    {
        uwAddr = REG_ADDR_COLOR_MATRIX_Main;//SCE_05;   //--post3x3
    }
    else
    {
        uwAddr = REG_ADDR_COLOR_MATRIX_Sub;//SCE_25;
    }
    old_msWrite2Byte ( uwAddr, uwContrast ); //CM_11
    old_msWrite2Byte ( uwAddr + 8, uwContrast ); //CM_22
    old_msWrite2Byte ( uwAddr + 16, uwContrast ); //CM_33
    old_msWriteByte ( SC0_00, ucBank );
        }
    #endif

}
#endif
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
///// Gen Output Interface ///////////////////////////////////////////////////////////
///// 1. Backlight adjust /////////////////////////////////////////////////////////////
///// 2. Y-gain adjust ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msDPS_AdjustBacklight ( BYTE BLValue ) // 0~255 <--> 0%~100%
{
    //BYTE xdata ucBank;
    //ucBank = msReadByte ( SC0_00 );

    if (CurBacklight != BLValue)
    {
        drvGPIO_SetBacklightDuty(BrightnessPWM,BLValue);
    }

    //mStar_AdjustBrightness(BLValue);//Set_Real_Brightness(BLValue);

#if DEBUG_PWM
if (CurBacklight != BLValue)
    printData("\r\n PWM %x", BLValue);
#endif
    CurBacklight = BLValue;
    //old_msWriteByte ( SC0_00, ucBank );
}

void msDPS_AdjustYgain ( WORD YgainValue ) // 2.10 format
{
BYTE xdata i;
for ( i = 0; i < 32; i++ )
    uwArray[i] = ISR_LumaHistogram32H[i] ;
#if DEBUG_HISTOGRAM
    //BYTE xdata i;
    printMsg("\r\n ===== Histogram Start ======");
    for ( i = 0; i < 32; i++ )
        printData("\r\n  %x", ISR_LumaHistogram32H[i]);
    printMsg("\r\n ===== Histogram End ======");
#endif
#if DEBUG_HISTOGRAM_PAM
printData("\r\n TotalPix %x", ISR_HistogramTotalPix);
printData("\r\n TotalLuma %x", ISR_HistogramTotalLuma);
printData("\r\n Max %x", ISR_HistogramMax);
printData("\r\n Min %x", ISR_HistogramMin);
#endif

#if DPS_UserContrastControl_EN
    YgainValue = (DWORD)(YgainValue * UserPrefContrast * 256 /75) >> 8
#endif

if (CurYgain != YgainValue)
{
    u16DPSContrast = (DWORD)YgainValue* UserPrefContrast / 1024;
    mStar_AdjustContrast( u16DPSContrast ); //msAdjustPCContrastXvycc ( ucWin, ( YgainValue & 0x0FFF ) ); // ratio: 0~3.99x
}

#if DEBUG_Ygain
if (CurYgain != YgainValue)
    printData("\r\n Ygain %x", YgainValue);
#endif
    CurYgain = YgainValue;
}

//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///// Function Declaration //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msDPS_Interrupt ( void )
{
    //BYTE xdata ucBank;
    BYTE xdata ucTmp;
    if ( ( msReadByte ( REG_ADDR_DLC_HANDSHAKE ) &BIT3 ) && ( !Flag_DPS_Latch_Busy ))
    {
        //msWriteByte(SC0_00, 0x08);
        for ( ucTmp = 0; ucTmp < 32; ucTmp++ )
        {
            ISR_LumaHistogram32H[ucTmp] = msReadByte ( REG_ADDR_HISTOGRAM_DATA_32 + ( 2 * ucTmp ) + 1 );
            ISR_LumaHistogram32H[ucTmp] <<= 8;
            ISR_LumaHistogram32H[ucTmp] |= msReadByte ( REG_ADDR_HISTOGRAM_DATA_32 + ( 2 * ucTmp ) );
        }
        //msWriteByte(SC0_00, 0x07);
        ISR_HistogramTotalPix = ((DWORD)g_sPnlInfo.sPnlTiming.u16Width * (DWORD)g_sPnlInfo.sPnlTiming.u16Height) >> 6 ;
        ISR_HistogramTotalLuma = ( ( ( WORD ) msReadByte ( REG_ADDR_HISTOGRAM_TOTAL_SUM_H ) ) << 8 ) + msReadByte ( REG_ADDR_HISTOGRAM_TOTAL_SUM_L ); //(((WORD)scRegs[0x37]) << 8) + scRegs[0x36];
        ISR_HistogramMax = msReadByte ( REG_ADDR_MAIN_MAX_VALUE );
        ISR_HistogramMin = msReadByte ( REG_ADDR_MAIN_MIN_VALUE );
        //ISR_HistogramMax = ISR_HistogramMax * 250 / 255;
        //Request Hist
        msWriteByte ( REG_ADDR_DLC_HANDSHAKE, msReadByte ( REG_ADDR_DLC_HANDSHAKE ) & 0xF0 );
        if ( MAIN_WINDOW == ucWin )
            msWriteByte ( REG_ADDR_DLC_HANDSHAKE, msReadByte ( REG_ADDR_DLC_HANDSHAKE ) | BIT2 | BIT1 );
        else
            msWriteByte ( REG_ADDR_DLC_HANDSHAKE, msReadByte ( REG_ADDR_DLC_HANDSHAKE ) | BIT2 | BIT0 );
    }
    ISR_curLoopCount++;
    //msWriteByte ( SC0_00, ucBank );

}

//===================================================================================================================================================================:
/*
static void SetHistogramRange ( WORD wVStart, WORD wVEnd, WORD wHStart, WORD wHEnd, BYTE Enable)
{
    BYTE xdata ucBank;
    ucBank = msReadByte ( SC0_00 );

    msWriteByte(SC0_00, 0x07);

    if ( REG_ADDR_HISTOGRAM_RANGE_M_HST !=0xFF)
    {
        // Set histogram range - H
        // Main Window
        msWriteByte ( REG_ADDR_HISTOGRAM_RANGE_M_HST, wHStart );
        msWriteByte ( REG_ADDR_HISTOGRAM_RANGE_M_HST + 1, ( msReadByte( REG_ADDR_HISTOGRAM_RANGE_M_HST + 1 ) & 0xF0 ) | ( wHStart >> 8 ) );
        msWriteByte ( REG_ADDR_HISTOGRAM_RANGE_M_HEN, wHEnd );
        msWriteByte ( REG_ADDR_HISTOGRAM_RANGE_M_HEN + 1, ( msReadByte( REG_ADDR_HISTOGRAM_RANGE_M_HEN + 1 ) & 0xF0 ) | ( wHEnd >> 8 ) );
    }
    // Set histogram range - V
    // Main Window
	msWriteByte ( REG_ADDR_HISTOGRAM_RANGE_M_VST, wVStart / 8 );
	msWriteByte ( REG_ADDR_HISTOGRAM_RANGE_M_VEN, wVEnd / 8 );
	msWriteByte(SC0_00, 0x03);
	msWriteByte ( REG_ADDR_HSLU_AP, ( msReadByte( REG_ADDR_HSLU_AP ) & 0xFE ) | ( ( wVStart / 8 ) >> 8 ) );
	msWriteByte ( REG_ADDR_HSLU_AP, ( msReadByte( REG_ADDR_HSLU_AP ) & 0xFB ) | ( ( wVEnd / 8 ) >> 8 ) );

    // Enable range for Histogram
    msWriteByte(SC0_00, 0x07);
    if(Enable)
    {
        msWriteByteMask ( REG_ADDR_HISTOGRAM_RANGE_H_ENABLE, BIT6 | BIT7, BIT6 | BIT7 );
        msWriteByteMask ( REG_ADDR_HISTOGRAM_RANGE_V_ENABLE, BIT0 | BIT1, BIT0 | BIT1 );
    }
    else
    {
        scWriteByteMask ( REG_ADDR_HISTOGRAM_RANGE_H_ENABLE, 0, BIT6 | BIT7 );
        scWriteByteMask ( REG_ADDR_HISTOGRAM_RANGE_V_ENABLE, 0, BIT0 | BIT1 );
    }
    msWriteByte ( SC0_00, ucBank );
}
*/
//===================================================================================================================================================================:
//////////////////////////////////////////////////////////////////////////////////
///// DPS initail ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msDPS_Init( void )
{
    //Default brightness 100
    CurYgain = 0x400;
    CurBacklight = 0xFF;
#if DPS_UserBrightControl_EN
    REG_DPS_UserBrightContlGain = UserPrefBrightness;//0~100//(WORD)USER_PREF_BRIGHTNESS * 256 / 100;//256;   // 0~256: 0~100%
#else
    REG_DPS_UserBrightContlGain = 100;
#endif

#if DPS_AggrPowerSaveMode_EN
    #if DPS_UserBrightControl_EN
        REG_DPS_UserBrightLimitGain = 256;//USER_PREF_BRIGHTNESS * 256 / 100;//
    #else
        REG_DPS_UserBrightLimitGain = 256;//"AggressivePdutyGain" it depend on "OSD bri", when OSD bri it sets lower than "AggressivePdutyGain", it need to follow "OSD bri".
    #endif
    REG_DPS_AggressiveYgainGain = 256;// 0~256: 0~100%
    REG_DPS_AggressivePdutyGain = 210;//180;// 0~256: 0~100%
    REG_DPS_AggressiveGainRange = 255;// apply AggressiveGain when ImgYmax < Range
#else
    REG_DPS_UserBrightLimitGain = 256;
    REG_DPS_AggressiveYgainGain = 256;
    REG_DPS_AggressivePdutyGain = 256;
    REG_DPS_AggressiveGainRange = 128;
#endif
    REG_DPS_BW_MODE_EN = 1;

    if(REG_DPS_BW_MODE_EN)
        REG_DPS_BW_MODE_Gain = 230;

    REG_DPS_YOUT_ENABLE = 0; //(Rosen)sc7_40[2]:reference Yout enable //(solar) sc0_5E[3]

    REG_DPS_RUN_Flag = 1;

#if DPS_YminForWindowSave_EN
    REG_DPS_PowerLevel = 20;  //0~5
#endif
#if DPS_HistPseuMax_MOD1_EN
    REG_MouseCount = 1;
#endif
}
//////////////////////////////////////////////////////////////////////////////////
///// DPS On <--> Off Settings ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void msDPS_On_Setting( void )
{
    msDrvHistogramReportEnable(MAIN_WINDOW,1);
    //BYTE ucBank = msReadByte ( SC0_00 );

    //msDlcOnOff (_DISABLE);
    msDPS_Init();
    // Request HW to do histogram
    //msWriteByteMask(REG_ADDR_HISTOGRAM_RGB_ENABLE, BIT6, BIT6 );// [6]: Histogram RGB Mode Enable
    //SetHistogramRange ( 0, ( PANEL_HEIGHT - 1 ), 0, ( PANEL_WIDTH - 1 ), 0 ); //SetHistogramRange(PanelHeight*1/8, PanelHeight*7/8, 0, (PanelWidth-1));

    ucWin = ( msReadByte ( REG_ADDR_MWE_ENABLE ) &BIT3 ) ? SUB_WINDOW : MAIN_WINDOW;

     //msWriteByte ( SC0_00, 0x07 );

    //msWriteByteMask ( REG_ADDR_HISTOGRAM_Acceler_ENABLE, BIT1, BIT1 ); //  histogram accelerate mode on

    //msWriteByteMask ( REG_ADDR_DLC_HANDSHAKE, 0, 0x0F );
    if ( MAIN_WINDOW == ucWin ) // Main window
        msWriteByteMask ( REG_ADDR_DLC_HANDSHAKE, BIT2 | BIT1, BIT2 | BIT1 | BIT0 );
    else // Sub window
        msWriteByteMask ( REG_ADDR_DLC_HANDSHAKE, BIT2 | BIT0, BIT2 | BIT1 | BIT0 );

    mStar_AdjustBrightness(100);
    u16DPSContrast = (DWORD)CurYgain* UserPrefContrast / 1024;
    mStar_AdjustContrast( u16DPSContrast );      //msAdjustPCContrastXvycc ( MAIN_WINDOW, CurYgain );

    //UserPrefContrast=128; // as range is 0~255, if range is 0~100, the value is 50

    //if (USER_PREF_INPUT_COLOR_FORMAT==INPUT_COLOR_RGB)
	//if(IsColorspaceRGB())
	{
		msWriteByteMask ( REG_ADDR_MAIN_COLOR_MATRIX_ENABLE, BIT7, BIT7 ); // 3x3 disable
		mStar_AdjustContrast(UserPrefContrast);
	}
	//else
	{
		//msWriteByteMask ( REG_ADDR_MAIN_COLOR_MATRIX_ENABLE, BIT7, BIT7 ); // 3x3 disable
		//msAdjustVideoContrast(MAIN_WINDOW,UserPrefContrast);
	}
    ///////Reference Yout/////////
    if(REG_DPS_YOUT_ENABLE)
    {
        //msWriteByte ( SC0_00, 0x07);
        //msWriteBit ( SC7_40, 1, BIT2 );
    }
    //////////////////////////////
    //msWriteByte ( SC0_00, ucBank );
}
//===================================================================================================================================================================:
void msDPS_Off_Setting ( void )
{
    
    REG_DPS_RUN_Flag = 0;

    //Default brightness 100
    mStar_AdjustBrightness(100);
    mStar_AdjustContrast( UserPrefContrast ); //msAdjustPCContrastXvycc ( MAIN_WINDOW, 0x400 );


    ISR_curLoopCount = 0;
    ISR_preLoopCount = 0;

    msDPS_AdjustYgain ( 1024 );
    mStar_AdjustBrightness(UserPrefBrightness);//msDPS_AdjustBacklight (USER_PREF_BRIGHTNESS);
    ///////Reference Yout/////////
    if(REG_DPS_YOUT_ENABLE)
    {
        //msWriteByte ( SC0_00, 0x07);
        //msWriteBit ( SC7_40, 0, BIT2 );
    }
    //////////////////////////////
    //msWriteByte ( SC0_00, ucBank );
}

void msDPSHandler(void)
{
        if(!REG_DPS_RUN_Flag)
        return;

        msDPS_Interrupt();
        msDPS_Handler();
}

//===================================================================================================================================================================:
#else
#endif // #if EnableDPS
//===================================================================================================================================================================:


