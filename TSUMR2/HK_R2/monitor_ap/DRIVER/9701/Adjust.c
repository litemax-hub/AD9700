#include <math.h>
#include "types.h"
#include "Global.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Debug.h"
#include "Common.h"
#include "msOSD.h"
#include "board.h"
#include "menudef.h"
#include "MsDLC.h"
#include "msACE.h"
#include "misc.h"
#include "drvOSD.h"
#include "drvGPIO.h"
#include "Adjust.h"
#include "drvDAC.h"
#include "OsdDefault.h"
#include "AutoGamma.h"
#include "drvDeltaE.h"
#include "msflash.h"
#include "string.h"

////////////////////////////////////////
//#include "drvAdjust.h"
//#include "halRwreg.h"
///////////////////////////////////////
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
//===================================================
// Local function declaration
#if UsesRGB
void mStar_AdjustRedColor( BYTE rColor, BYTE gColor, BYTE bColor );
void mStar_AdjustGreenColor( BYTE rColor, BYTE gColor, BYTE bColor );
void mStar_AdjustBlueColor( BYTE rColor, BYTE gColor, BYTE bColor );
#else
void mStar_AdjustRedColor( BYTE color, BYTE contrast );
void mStar_AdjustGreenColor( BYTE color, BYTE contrast );
void mStar_AdjustBlueColor( BYTE color, BYTE contrast );
#endif

extern void GetColorTempRGB(ColorType *pstColor);
extern Bool IsColorspaceRGB(void);

void mStar_AdjustBlackLevel( BYTE BlackLevel );
#define min( a , b )     ( (a) < (b) ? (a) : (b) )
#define RealMaxBrightnessValue  0xFF
#define RealMinBrightnessValue  0x00//0x7F
#define RealAbsoluteMinBrightnessValue  0x30

#define LowBoundDCRRealBriRatio          30     // CCFL: 30%, LED:50%
#define RealMinDcrBrightnessValue    ((((RealMaxBrightnessValue - RealMinBrightnessValue) * LowBoundDCRRealBriRatio) / 100) + RealMinBrightnessValue)

//****************************************************
#define MaxBacklightNowValue    240  // HongYi suggest 100712 Rex
#define MinBacklightNowValue      20//17//29  //39  //19//17

#define AbsMostDarkest1  ((CURRENT_INPUT_IS_VGA())?(16):(16))  // 16  //20081201
#define AbsNearDarkest1  ((CURRENT_INPUT_IS_VGA())?(19):(17))  // 18
#define AbsMostDarkest2  ((CURRENT_INPUT_IS_VGA())?(16):(16))  // 17
#define AbsNearDarkest2  ((CURRENT_INPUT_IS_VGA())?(21):(19))  // 20
//****************************************************
//Define-----------------------------------------------------
#define RGB_GAIN_OSD_MAX     255//0xDB//255//255
#define RGB_GAIN_OSD_MID     128//0xDB/2//128//0xDA/2//128
#define RGB_GAIN_OSD_MIN     0

#if (COLOR_ACCURACY == COLOR_8_BIT)
#define RGB_GAIN_REAL_MAX    128
#define RGB_GAIN_REAL_MID    64
#define RGB_GAIN_REAL_MIN    0
#elif (COLOR_ACCURACY == COLOR_10_BIT)
#define RGB_GAIN_REAL_MAX    512
#define RGB_GAIN_REAL_MID    256
#define RGB_GAIN_REAL_MIN    0
#endif
BYTE code msAdjustNullData[] = {0};
void msAdjustDummy(void)
{
    BYTE xdata i = msAdjustNullData[0];
    i = i;
}
void mStar_AdjustDcrBrightness( void )
{
    static BYTE xdata brightness;
    static BYTE xdata DcrCutoffFlag = 0;    //DCR new rule 081128

#if ENABLE_DEBUG
static BYTE xdata PreValue;
#endif

    if( BacklightNow < MinBacklightNowValue )
        BacklightNow = MinBacklightNowValue;

    if( BacklightNow > MaxBacklightNowValue )
    {
        brightness = RealMaxBrightnessValue;
        DcrCutoffFlag = 0;    //20081201
    }
    else if( BacklightNow <= MinBacklightNowValue )
    {
        if( SetDarkestFlag ) // the black pattern
        {
            SetDarkestFlag = 0;
            PreBrightData = 0; // to make sure if bright data exist, it will update brightness.
            //brightness = RealAbsoluteMinBrightnessValue;  // set logical absolute dark value, // RealMinBrightnessValue;
            if( DcrCutoffFlag == 0 ) // DcrCutoffFlag nitial value is 0
            {
                if(( DarkData <= AbsMostDarkest1 ) && ( BrightData <= AbsNearDarkest1 ) )   // >>>  logical absolute dark condition  <<< 20081128
                    // set logical absolute dark value, // RealMinBrightnessValue;
                {
                    //brightness = RealMinDcrBrightnessValue;  // set logical absolute dark value, // RealMinBrightnessValue;
                    DcrCutoffFlag = 1;
                }
                brightness = RealMinDcrBrightnessValue;  // set logical absolute dark value, // RealMinBrightnessValue;
            }
            else
            {
                if(( DarkData <= AbsMostDarkest2 ) && ( BrightData <= AbsNearDarkest2 ) )   // set logical absolute dark value,
                {
                    if( brightness > RealAbsoluteMinBrightnessValue )
                        brightness--;  // set logical absolute dark value, // RealMinBrightnessValue;
#if 0// defined(DCRClearBacklight)
                     if(OSD_MENU_EXIST() && ( brightness < RealMinBrightnessValue ) )
                        brightness = RealMinBrightnessValue;
                     ForceDelay1ms( 100 );
#endif
                }
                else
                {
                    DcrCutoffFlag = 0;
                    brightness = RealMinDcrBrightnessValue;
                }
            }
        }    // if(SetDarkestFlag)
        else // RealMinDcrBrightnessValue=98 -> brightness >=98 (min)
        {
            DcrCutoffFlag = 0;
            brightness = RealMinDcrBrightnessValue;
        }
    }
    else  // RealMinDcrBrightnessValue=98 -> brightness >=98 (min)
    {
        DcrCutoffFlag = 0;
        brightness = ((( DWORD )( BacklightNow - MinBacklightNowValue ) * ( RealMaxBrightnessValue - RealMinDcrBrightnessValue ) ) / ( MaxBacklightNowValue - MinBacklightNowValue ) ) + RealMinDcrBrightnessValue;
    }

    #if 0//DEBUG_PRINT_ENABLE
    printData( "---BacklightNow =%d", BacklightNow );
    printData( "---brightness =%d", brightness );
    #endif

    #if ENABLE_DEBUG
    if(abs(PreValue-BacklightNow) > 3)
    {
        PreValue = BacklightNow;
    printData( "---BacklightNow =%d", BacklightNow );
    }
    #endif

    #if BRIGHTNESS_INVERSE  //+Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-brightness);
    #else                           //-Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, brightness);
    #endif

    #if defined(DCRClearBacklight)
    if( brightness == RealAbsoluteMinBrightnessValue )
    {

        if( BackLightActiveFlag )
            hw_ClrBlacklit();
    }
    else
    {
        if( !BackLightActiveFlag )
            hw_SetBlacklit();
    }
    #endif
}

void mStar_AdjustBrightness( BYTE brightness )
{
    brightness = ((( DWORD )brightness * ( RealMaxBrightnessValue - RealMinBrightnessValue ) ) / 100 ) + RealMinBrightnessValue;

    BacklightNow = BacklightPrev = brightness;

    #if BRIGHTNESS_INVERSE  //+Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-brightness);
    //drvGPIO_SetBacklightDuty(BrightnessPWM, 0);
    #else                           //-Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, brightness);
    //drvGPIO_SetBacklightDuty(BrightnessPWM, 0xff);
    #endif
}

BYTE mAPI_appAdjust_IDXColorMode_Load(BYTE u8DispWin, WORD IDX)
{
    return mdrv_DetalE_IDXColorMode_Load(u8DispWin, IDX);
}

WORD PWMBoundaryClamp(WORD PWMvalue)
{
    //printData("Before Clamp BRIGHTNESS PWM value: %x\n", PWMvalue);

    WORD retPWMvalue;
    WORD maxVal, minVal;
    maxVal = BRIGHTNESS_REAL_MAX;
    minVal = BRIGHTNESS_REAL_MIN;

    #if ENABLE_HDR
    BOOL IsHDR10Input = FALSE;
    //IsHDR10Input =  msIsDetectedHDR10Input(MAIN_WINDOW);
    if(IsHDR10Input)
    {
        maxVal = BRIGHTNESS_HDR_MAX;
        minVal = BRIGHTNESS_HDR_MIN;
    }
    #endif

    if(PWMvalue > maxVal)
        retPWMvalue = maxVal;
    else if(PWMvalue < minVal)
        retPWMvalue = minVal;
    else
        retPWMvalue = PWMvalue;

    //printData("After Clamp BRIGHTNESS PWM value: %x\n", retPWMvalue);
    return retPWMvalue;
}

void msAPI_SetGlobalBrightnessPWM(WORD u16Brightness)
{
    //printData ( "[msAPI_SetGlobalBrightnessPWM]brightness ==> %x\n", u16Brightness );
    #if 0//LD_ENABLE
        //MApi_LD_Set_Globaldimming_Strength(TRUE, (BYTE)(u16Brightness) );
    #else
        #if BRIGHTNESS_INVERSE  //+Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-u16Brightness);
        //drvGPIO_SetBacklightDuty(BrightnessPWM, 0);
        #else                           //-Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, u16Brightness);
        //drvGPIO_SetBacklightDuty(BrightnessPWM, 0xff);
        #endif
    #endif
}

#if AudioFunc
WORD GetRealVaueFunc(WORD wOsdValue, WORD wOsdMin, WORD wOsdMid, WORD wOsdMax, WORD wRealMin, WORD wRealMid, WORD wRealMax)
{
    WORD wResult=0;

    if(wOsdValue == wOsdMid)
    {
        wResult = wRealMid;
    }
    else if(wOsdValue > wOsdMid)
    {
        wResult = LINEAR_MAPPING_VALUE(wOsdValue, \
            wOsdMid, wOsdMax, \
            wRealMid, wRealMax);
    }
    else
    {
        wResult = LINEAR_MAPPING_VALUE(wOsdValue, \
            wOsdMin, wOsdMid, \
            wRealMin, wRealMid);
    }
    return wResult;
}
WORD GetRealRgbGain(WORD u16OsdValue)
{
    WORD u16Result=0;

    u16Result = GetRealVaueFunc(u16OsdValue, RGB_GAIN_OSD_MIN, RGB_GAIN_OSD_MID, RGB_GAIN_OSD_MAX,\
        RGB_GAIN_REAL_MIN, RGB_GAIN_REAL_MID, RGB_GAIN_REAL_MAX);

    return u16Result;
}
WORD GetRealVolume(WORD u16OsdValue)
{
    WORD u16Result=0;

    u16Result = GetRealVaueFunc(u16OsdValue, VOLUME_OSD_MIN, VOLUME_OSD_MID, VOLUME_OSD_MAX, VOLUME_REAL_MIN, VOLUME_REAL_MID, VOLUME_REAL_MAX);

    return VOLUME_REAL_MAX-u16Result;
}
void msAPI_AdjustVolume(BYTE u8Volume)
{
    signed char s8Volume;

    if(u8Volume == 0)
    {
    #if ENABLE_DAC
        msAPI_AudioSetDpgaMute(TRUE);
    #endif
    }
    else
    {
        s8Volume = (-1) * GetRealVolume(u8Volume);
    #if ENABLE_DAC
        msAPI_AudioSetVolume(E_AUDIO_LEFT_RIGHT_CH, (short)s8Volume, VOLUME_FRACTION_MINUS_dot0dB);
    #endif
    }
}
#endif


void mStar_AdjustRGBColor(BYTE u8Contrast, BYTE u8Red, BYTE u8Green, BYTE u8Blue)
{
    msAdjustVideoContrastRGB(MAIN_WINDOW,u8Contrast, u8Red, u8Green, u8Blue);

}

#define IsUseMatrix 0
// 0 ~ 255 (default 128)
void mStar_AdjustContrast( BYTE contrast )
{
    xdata ColorType CT;
    xdata ST_COMBO_COLOR_FORMAT cf;

    GetColorTempRGB(&CT);

 if (UserPrefInputColorRange == ColorRangeMenuItems_Full)
	cf =IsOSDCSCControl(TRUE);
 else  if (UserPrefInputColorRange == ColorRangeMenuItems_Limit)	
	cf =IsOSDCSCControl(TRUE);
 else  if (UserPrefInputColorRange == ColorRangeMenuItems_Auto)	
	cf =IsOSDCSCControl(FALSE);

 //msACESetRGBColorRange(MAIN_WINDOW,_ENABLE, cf.ucColorRange== COMBO_COLOR_RANGE_LIMIT);

#if IsUseMatrix
    mStar_AdjustRGBColor(contrast, CT.u8Red, CT.u8Green, CT.u8Blue);
#else
    msAPI_AdjustRGBColor(MAIN_WINDOW, contrast, CT.u8Red, CT.u8Green, CT.u8Blue);
#endif
}
// 0 ~ 255 (default 128)
void mStar_AdjustRedColor( BYTE color, BYTE contrast )
{
    xdata ColorType CT;
    GetColorTempRGB(&CT);

#if IsUseMatrix
    mStar_AdjustRGBColor(contrast, color, CT.u8Green, CT.u8Blue);
#else
	msAPI_AdjustRGBColor(MAIN_WINDOW, contrast, color, CT.u8Green, CT.u8Blue);
#endif
}
// 0 ~ 255 (default 128)
void mStar_AdjustGreenColor( BYTE color, BYTE contrast )
{
    xdata ColorType CT;

    GetColorTempRGB(&CT);
#if IsUseMatrix
    mStar_AdjustRGBColor(contrast, CT.u8Red, color, CT.u8Blue);
#else
	msAPI_AdjustRGBColor(MAIN_WINDOW, contrast, CT.u8Red, color, CT.u8Blue);
#endif
}
// 0 ~ 255 (default 128)
void mStar_AdjustBlueColor( BYTE color, BYTE contrast )
{
    xdata ColorType CT;

    GetColorTempRGB(&CT);
#if IsUseMatrix
    mStar_AdjustRGBColor(contrast, CT.u8Red, CT.u8Green, color);
#else
	msAPI_AdjustRGBColor(MAIN_WINDOW, contrast, CT.u8Red, CT.u8Green, color);
#endif
}

void mStar_AdjustHPosition( WORD position )
{
    SC0_SET_IMAGE_START_H(position);//old_msWrite2Byte(SC0_07, position);
}

void mStar_AdjustVPosition( WORD position )
{
#if 1
#if DECREASE_V_SCALING
    position = (position>VSTART_OFFSET)?(position-VSTART_OFFSET):(0);
#endif
    SC0_SET_IMAGE_START_V(position);//old_msWrite2Byte( SC0_05, position );
#else
    if( DecVScaleFlag )
    {
        if( position <= ( VScaleDecValue / 2 ) )
            position = VScaleDecValue / 2 + 1;
        old_msWrite2Byte( SC0_05, position - ( VScaleDecValue / 2 ) );
    }
    else
    {
        if( position == 0 )
            position = 1;
        old_msWrite2Byte( SC0_05, position );
    }
#endif
    if((g_SetupPathInfo.ucFrameLockMode == FLM_FB_FPLL) || (g_SetupPathInfo.ucFrameLockMode == FLM_FBL_FRAMEPLL))
    	msDrvSetIVS();

}

// 100702 coding, modify to wait complete blanking
void mStar_WaitForDataBlanking( void )
{
    WORD retry = 0xffff;

    if( SyncLossState() || !bPanelOnFlag )
        return;

    while( retry-- && ( msReadByte(SC00_90)&BIT7 ) && !InputTimingChangeFlag );

    retry = 0xffff;

    while( retry-- && !( msReadByte(SC00_90)&BIT7 ) && !InputTimingChangeFlag );
}

void mStar_AdjustBackgoundColor( BYTE pattern )
{
    WORD rColor = 0;
    WORD gColor = 0;
    WORD bColor = 0;
    if( pattern == 0 )
    {
        rColor = 0xFFF;
    }
    else if( pattern == 1 )
    {
        gColor = 0xFFF;
    }
    else if( pattern == 2 )
    {
        bColor = 0xFFF;
    }
    else if( pattern == 4 )
    {
        rColor = 0xFFF;
        gColor = 0xFFF;
        bColor = 0xFFF;
    }
    mStar_WaitForDataBlanking();
    drvOSD_FrameColor_Set(MAIN_WINDOW, ENABLE, rColor, gColor, bColor);
}


void msAPI_CustomHueEnable(BYTE u8WinIndex, Bool bEnable)
{
    msDrvCustomHueEnable(u8WinIndex, bEnable);
}

void msAPI_CustomSatEnable(BYTE u8WinIndex, Bool bEnable)
{
    msDrvCustomSatEnable(u8WinIndex, bEnable);
}

void msAPI_CustomBriEnable(BYTE u8WinIndex, Bool bEnable)
{
    msDrvCustomBriEnable(u8WinIndex, bEnable);
}

void msAPI_FixGammaLoadTbl_256E_12B_Extend(BYTE u8WinIndex, BYTE **pu8TableIndex, BOOL bExtendMode)
{
    msDrvFixGammaLoadTbl_256E_12B_Extend(u8WinIndex,pu8TableIndex, bExtendMode);
}

void msAPI_DeGammaLoadTbl_1024E_16B_N(BYTE u8WinIndex, BYTE **pu8Table)
{
 BYTE i;
#if ENABLE_LUT_AUTODOWNLOAD
    msDrvClearDMADeGammaDataBlock();
#else
    msDrvDeGammaEnable(u8WinIndex,FALSE);
#endif

    for (i = 0; i < 3; i++)
    	{
#if ENABLE_LUT_AUTODOWNLOAD
        msDrvDeGammaWriteTbl_1024E_16B_ByDMA(i, pu8Table);
#else
        msDrvDeGammaLoadTbl_1024E_16B_N(u8WinIndex, i, pu8Table);
#endif
}

#if ENABLE_LUT_AUTODOWNLOAD
  msDrvDeGamma_DMA_Reader_Trigger(u8WinIndex);
#endif


    msDrvDeGammaEnable(u8WinIndex,_ENABLE);
    //for (i = 0; i < DISPLAY_MAX_NUMS; i++)
    //    if (msAPIWinGetScDwEnable(i) && (ucDeGammaFunc[i] == _ENABLE))
    //        msDrvDeGammaEnable(i, TRUE);

}

void msAPI_GammaEnable(BYTE u8WinIndex, BYTE bEnable)
{
    msDrvGammaEnable(u8WinIndex, bEnable);
}

void msAPI_DeGammaEnable(BYTE u8WinIndex, BYTE bEnable)
{
    msDrvDeGammaEnable(u8WinIndex, bEnable);
    //ucDeGammaFunc[u8WinIndex] = bEnable;
}

void msAPI_FixGammaEnable(BYTE u8WinIndex, BYTE bEnable)
{
    msDrvFixGammaEnable(u8WinIndex, bEnable);
    //ucFixGammaFunc[u8WinIndex] = bEnable;
}

void msAPI_ColorMatrixEnable(BYTE u8WinIndex, BYTE bEnable, BYTE bCarryEn)
{
    msDrvColorMatrixEnable(u8WinIndex, bEnable, bCarryEn);
}

void msAPI_GammaLoadTbl_1024E_12B(BYTE u8WinIndex, BYTE **pu8TableIndex)
{
    #if !(ENABLE_LUT_AUTODOWNLOAD)
                msDrvGammaEnable(u8WinIndex, FALSE);
    #endif

    msDrvGammaLoadTbl_1024E_12B(u8WinIndex, pu8TableIndex);

    msDrvGammaEnable(u8WinIndex, _ENABLE);
}

void msAPI_GammaLoadTbl_1024E_12B_N(BYTE u8WinIndex, BYTE **pu8TableIndex)
{
    #if !(ENABLE_LUT_AUTODOWNLOAD)
                msDrvGammaEnable(u8WinIndex, FALSE);
    #endif

    msDrvGammaLoadTbl_1024E_12B_N(u8WinIndex, pu8TableIndex);
}

// bDirection: 0 for left, 1 for right
void msAPI_AdjustOutputHPosition( WORD u16ShiftAmount , BOOL bDirection )
{
    WORD u16HStart, u16HEnd, u16HEndPre;
    WORD u16PnlHDEEnd = (g_sPnlInfo.sPnlTiming.u16HStart + g_sPnlInfo.sPnlTiming.u16Width)/PANEL_H_DIV - 1;
#if ENABLE_MENULOAD
    BYTE bMLEn = u8MenuLoadFlag[ML_DEV_1];
    if(!bMLEn)
        MENU_LOAD_START();
#endif
    u16HStart = msRead2Byte(SC00_18);
    u16HEnd = msRead2Byte(SC00_1C);
    
    u16HStart = (bDirection)?(u16HStart + u16ShiftAmount):(u16HStart - u16ShiftAmount);
    u16HEnd = u16HEndPre = (bDirection)?(u16HEnd + u16ShiftAmount):(u16HEnd - u16ShiftAmount);
    
    if((u16HEndPre>u16PnlHDEEnd) && (u16HEnd<=u16PnlHDEEnd))
        u16HEnd -=1;
    else if((u16HEndPre<u16PnlHDEEnd) && (u16HEnd>=u16PnlHDEEnd))
        u16HEnd +=1;

    DB_W2BMask(SC00_18, u16HStart, 0x1FFF); //image H. start
    DB_W2BMask(SC00_1C, u16HEnd, 0x1FFF); //image H. end
#if ENABLE_MENULOAD
    if(!bMLEn)
        MENU_LOAD_END();
#endif
}

// bDirection: 0 for up, 1 for down
void msAPI_AdjustOutputVPosition( WORD u16ShiftAmount , BOOL bDirection )
{
    WORD u16VStart, u16VEnd;
    WORD u16SCVStart, u16SCVEnd;
#if ENABLE_MENULOAD
    BYTE bMLEn = u8MenuLoadFlag[ML_DEV_1];
    if(!bMLEn)
        MENU_LOAD_START();
#endif
    u16VStart = msRead2Byte(SC00_5D);
    u16VEnd = msRead2Byte(SC00_1A);
    
    u16VStart = (bDirection)?(u16VStart + u16ShiftAmount):(u16VStart - u16ShiftAmount);
    u16VEnd = (bDirection)?(u16VEnd + u16ShiftAmount):(u16VEnd - u16ShiftAmount);
    
    u16SCVStart = msRead2Byte(SC00_10);
    u16SCVEnd = msRead2Byte(SC00_14);
    if(ENABLE_SUPER_RESOLUTION)
    {
        u16SCVStart -= PANEL_SHIFT_DE_V;
        u16SCVEnd -= PANEL_SHIFT_DE_V;
    }
    if(u16VEnd>u16SCVEnd || u16VStart>=u16SCVStart)
    {
        u16VEnd = u16SCVEnd;
    }
    DB_W2BMask(SC00_5D, u16VStart, 0x1FFF); //image V. start
    DB_W2BMask(SC00_1A, u16VEnd, 0x1FFF); //image V. end
#if ENABLE_MENULOAD
    if(!bMLEn)
        MENU_LOAD_END();
#endif
}

static void _msWinMWEConfig(WORD u16X, WORD u16Y, WORD u16Width, WORD u16Height)
{
    WORD u16PanelX = msRead2Byte(SC00_12);
    WORD u16PanelY = msRead2Byte(SC00_10);
    WORD u16MWEHStart = u16PanelX + u16X;
    WORD u16MWEHEnd = u16PanelX + u16X + u16Width - 1;
    WORD u16MWEVStart = u16PanelY + u16Y;
    WORD u16MWEVEnd = u16PanelY + u16Y + u16Height - 1;

    DB_W2B(SC03_18, u16MWEHStart);
    DB_W2B(SC03_1C, u16MWEHEnd);

    DB_W2B(SC03_1E,u16MWEVStart);
    DB_W2B(SC03_1A,u16MWEVEnd);
}

static void _msWinBorderWidth(BYTE u8Left, BYTE u8Right, BYTE u8Top, BYTE u8Bottom, BOOL bIsInner)
{
    static BYTE _u8Left = 0;
    static BYTE _u8Right = 0;
    static BYTE _u8Top = 0;
    static BYTE _u8Bottom = 0;

    u8Left = (u8Left & 0x0F) << (4*(!bIsInner));
    u8Right = (u8Right & 0x0F) << (4*(!!bIsInner));
    u8Top = (u8Top & 0x0F) << (4*(!bIsInner));
    u8Bottom = (u8Bottom & 0x0F) << (4*(!!bIsInner));

    _u8Left = (_u8Left & (bIsInner?0xF0:0x0F)) | u8Left;
    _u8Right = (_u8Right & (bIsInner?0x0F:0xF0)) | u8Right;
    _u8Top = (_u8Top & (bIsInner?0xF0:0x0F)) | u8Top;
    _u8Bottom = (_u8Bottom & (bIsInner?0x0F:0xF0)) | u8Bottom; 

    DB_WB(SC03_05, _u8Left);
    DB_WB(SC03_07, _u8Right);

    DB_WB(SC03_09, _u8Top);
    DB_WB(SC03_0B, _u8Bottom);
}

static void _msWinBorderColor(BYTE u8BorderColor)
{
    // cannot seperate control when 2D peaking is on
    DB_WB(SC65_2E, u8BorderColor);
    DB_WB(SC65_49, u8BorderColor);
}

static void _msWinMWEBorderEnable(BOOL bEnable)
{
    DB_WBit(SC00_5C, bEnable, BIT3);
    DB_WBit(SC03_02, bEnable, BIT2);
}

void msAPI_MWEBorderEnable(BOOL bEnable, WORD u16X, WORD u16Y, WORD u16Width, WORD u16Height, BYTE u8BorderWidth, BYTE u8BorderColor, BOOL bIsInnerBorder)
{
#if ENABLE_MENULOAD
    BYTE bMLEn = u8MenuLoadFlag[ML_DEV_1];
    if(!bMLEn)
        MENU_LOAD_START();
#endif
    _msWinMWEConfig(u16X, u16Y, u16Width, u16Height);
    _msWinBorderWidth(u8BorderWidth, u8BorderWidth, u8BorderWidth, u8BorderWidth, bIsInnerBorder);
    _msWinBorderColor(u8BorderColor);
    _msWinMWEBorderEnable(bEnable);

#if ENABLE_MENULOAD
    if(!bMLEn)
        MENU_LOAD_END();
#endif
}

void msAPI_ForceUseBothContrast_Get( BYTE u8DispWin, BOOL bForceUseBothContrast )
{
    msForceUseBothContrast(u8DispWin, bForceUseBothContrast);
}

void msAPI_AdjustRGBColor( BYTE u8DispWin, WORD u16Contrast, WORD u16Red, WORD u16Green, WORD u16Blue)
{
    //printf("[msAPI_AdjustRGBColor]u16Red:0x%x, u16Green:0x%x, u16Blue:0x%x\n", u16Red, u16Green, u16Blue);
    u16Red  = GetRealRgbGain(u16Red);
    u16Green= GetRealRgbGain(u16Green);
    u16Blue = GetRealRgbGain(u16Blue);

    int shift = (COLOR_ACCURACY == COLOR_8_BIT)?4:2;

    if (msForceUseBothContrast_Get(u8DispWin))
    {
        msSetContrast(u8DispWin, u16Contrast);
        msSetUserRGBGain(u8DispWin, u16Red<<shift, u16Green<<shift, u16Blue<<shift);
    }
    else
    {
        msSetRGBContrast(u8DispWin, u16Contrast, u16Red, u16Green, u16Blue);
    }
    msAdjustRGBContrast(u8DispWin);
}

void msAPI_WinReportEnable(Bool bEnable)
{
    msDrvWinReportEnable(bEnable);
}
void msAPI_DisplayWindowEnable(Bool bEnable)
{
    msDrvDisplayWindowEnable(bEnable);
}
void msAPI_WinReportSetRange(WORD u16HStart, WORD u16HEnd, WORD u16VStart, WORD u16VEnd)
{
    msDrvWinReportSetRange(u16HStart, u16HEnd, u16VStart, u16VEnd);
}

void msAPI_WinReportGetWeightingPixelCountBySetRange(WORD *Rweight, WORD *Gweight, WORD *Bweight)
{
    msDrvWinReportGetWeightingPixelCountBySetRange(Rweight, Gweight, Bweight);
}
void mapi_Adjust_EnableNonStdCSC_Set(BOOL u8Enable)
{
    mdrv_Adjust_EnableNonStdCSC_Set(u8Enable);
}

BOOL mapi_Adjust_EnableNonStdCSC_Get(void)
{
    return mdrv_Adjust_EnableNonStdCSC_Get();
}
#if (USEFLASH && COLORDATA_USEFLASH)
#warning "!! Please implement customized functions below for FlashWriter(Byte/Table) !!"
#warning "!! The function should contain FLASH erase sector inside !!"

static Bool _bDoWP = TRUE;
static MemoryType _enMemoryType = enMemoryType_FLASH;
static DWORD _addrOfAutoColorBlock = ADDR_DELTAE_BLOCK;
#define FLASH_SECTOR_SIZE 0x1000

void msAPI_appAdjust_FlashWriteByte(Bool bDoWP, DWORD u32Addr, BYTE u8Value)
{
    BYTE new_buffer[FLASH_SECTOR_SIZE];

    DWORD SectorAddr = u32Addr/FLASH_SECTOR_SIZE*FLASH_SECTOR_SIZE; 
    Flash_ReadTable(SectorAddr, new_buffer, FLASH_SECTOR_SIZE);
    new_buffer[u32Addr%FLASH_SECTOR_SIZE] = u8Value; 
    Flash_EraseSector(bDoWP, SectorAddr);
    Flash_WriteTable(bDoWP, SectorAddr, new_buffer, FLASH_SECTOR_SIZE);        
}

void msAPI_appAdjust_FlashWriteTbl(Bool bDoWP, DWORD u32Addr, BYTE *buffer, DWORD count)
{   
    BYTE new_buffer[FLASH_SECTOR_SIZE];
    DWORD currentAddress = u32Addr;
    DWORD endAddress = u32Addr+count;
    DWORD pageAddress, slidingSize, writingSize, remainingSize, writtenSize = 0;
    printf("currentAddress:0x%x, endAddress:0x%x\n", currentAddress, endAddress);
    while (currentAddress < endAddress) {
        // Calculate the start address of the flash sector containing the current address
        pageAddress = currentAddress & ~(FLASH_SECTOR_SIZE - 1);
        //Backup the data in flash
        Flash_ReadTable(pageAddress, new_buffer, FLASH_SECTOR_SIZE); 

        // Erase the sector if it hasn't been erased yet
        Flash_EraseSector(bDoWP, pageAddress);
        printf("Erase pageAddress:0x%x\n", pageAddress);
        slidingSize = currentAddress - pageAddress;
        remainingSize = count-writtenSize;
        writingSize = min(remainingSize, FLASH_SECTOR_SIZE-slidingSize);
        printf("slidingSize:0x%x, remainingSize:0x%x, writingSize:0x%x\n", slidingSize, remainingSize, writingSize);
        memcpy(new_buffer+slidingSize, buffer+writtenSize, writingSize); //copy data to be written to buffer
        Flash_WriteTable(bDoWP, pageAddress, new_buffer, FLASH_SECTOR_SIZE);
        printf("Write pageAddress:0x%x\n", pageAddress);
        writtenSize += writingSize;
        currentAddress += writingSize;
        printf("currentAddress:0x%x\n", currentAddress);
    }
}

void msAPI_appAdjust_FlashInit(void)
{
    mdrv_DeltaE_FlashWriteProtect_Set(_bDoWP);   
    mdrv_DeltaE_FlashByteWriter_Set(msAPI_appAdjust_FlashWriteByte);
    mdrv_DeltaE_FlashTblWriter_Set(msAPI_appAdjust_FlashWriteTbl);
    mdrv_DeltaE_Memory_Init(_addrOfAutoColorBlock, _enMemoryType);
}
#endif

#if ENABLE_SUPER_RESOLUTION
void msInitalSRSetting(void)
{
    msDrv_InitalSRSetting();
}

void msSuperResolutionOnOff(BOOL bOn)
{
    msDrv_SuperResolutionOnOff(bOn);
}

void mStar_SetupSuperResolution( BYTE SuperResolutionMode )
{
    msDrv_SetupSuperResolution(SuperResolutionMode);
}
#endif

Bool SetColorModeDemo(void)
{
    msDrv_SetColorModeDemo();
}
