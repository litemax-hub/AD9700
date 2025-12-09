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
//-----------------------------------------------------------
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
	#if (LiteMAX_UI == 1)||(LiteMAX_Baron_UI == 1)
//LiteMAX_Brightness_Mapping
    if(brightness==0)
        brightness = FUserPrefBrightness_0;
    else if(brightness<=25)
        brightness = LINEAR_MAPPING_VALUE(brightness,0,25,FUserPrefBrightness_0,FUserPrefBrightness_25);//FUserPrefBrightness_25;
    else if(brightness<=50)
        brightness = LINEAR_MAPPING_VALUE(brightness,25,50,FUserPrefBrightness_25,FUserPrefBrightness_50);//FUserPrefBrightness_50;
    else if(brightness<=75)
        brightness = LINEAR_MAPPING_VALUE(brightness,50,75,FUserPrefBrightness_50,FUserPrefBrightness_75);//FUserPrefBrightness_75;
    else
        brightness = LINEAR_MAPPING_VALUE(brightness,75,MaxBrightnessValue,FUserPrefBrightness_75,FUserPrefBrightness_100);//FUserPrefBrightness_100;
//LiteMAX_Brightness_Mapping
    #endif

    brightness = ((( DWORD )brightness * ( RealMaxBrightnessValue - RealMinBrightnessValue ) ) / MaxBrightnessValue ) + RealMinBrightnessValue;

    BacklightNow = BacklightPrev = brightness;

    #if BRIGHTNESS_INVERSE  //+Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-brightness);
    //drvGPIO_SetBacklightDuty(BrightnessPWM, 0);
    #else                           //-Duty power
    drvGPIO_SetBacklightDuty(BrightnessPWM, brightness);
    //drvGPIO_SetBacklightDuty(BrightnessPWM, 0xff);
    #endif
}

#if AudioFunc
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

#if ENABLE_FULL_RGB_COLOR_PATH
    msAdjustVideoContrastRGB(MAIN_WINDOW,u8Contrast, u8Red, u8Green, u8Blue);
#else
    if (IsColorspaceRGB() && (UserPrefInputColorFormat == INPUTCOLOR_RGB))
    {
        msAdjustPCContrastRGB(MAIN_WINDOW,u8Contrast, u8Red, u8Green, u8Blue);
    }
    else
    {
    msAdjustVideoContrastRGB(MAIN_WINDOW,u8Contrast, u8Red, u8Green, u8Blue);
    }

#endif
}

#define IsUseMatrix 0
// 0 ~ 255 (default 128)
void mStar_AdjustContrast( BYTE contrast )
{
    printf("[mStar_AdjustContrast]\n");
    xdata ColorType CT;
    xdata ST_COMBO_COLOR_FORMAT cf;

    GetColorTempRGB(&CT);

   cf =IsOSDCSCControl(OSD_CONTROL_CSC);
#if IsUseMatrix
    mStar_AdjustRGBColor(contrast, CT.u8Red, CT.u8Green, CT.u8Blue);
#else
	msAPI_AdjustRGBColor(MAIN_WINDOW, contrast, CT.u8Red, CT.u8Green, CT.u8Blue);
#endif
}
// 0 ~ 255 (default 128)
void mStar_AdjustRedColor( BYTE color, BYTE contrast )
{
    printf("[mStar_AdjustRedColor]\n");
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
    printf("[mStar_AdjustRedColor]\n");
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
    printf("[mStar_AdjustRedColor]\n");
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
    BYTE rColor = 0;
    BYTE gColor = 0;
    BYTE bColor = 0;
    if( pattern == 0 )
    {
        rColor = 0xFF;
    }
    else if( pattern == 1 )
    {
        gColor = 0xFF;
    }
    else if( pattern == 2 )
    {
        bColor = 0xFF;
    }
    else if( pattern == 3 )
    {
        rColor = 0xFF;
        gColor = 0xFF;
        bColor = 0xFF;
    }
    else if( pattern == 4 )
    {
        rColor = 0;
        gColor = 0;
        bColor = 0;
    }
    else if( pattern == 5 )
    {
        rColor = 0x33;
        gColor = 0x33;
        bColor = 0x33;
    }
    else if( pattern == 6 )
    {
        rColor = 0x66;
        gColor = 0x66;
        bColor = 0x66;
    }
    else if( pattern == 7 )
    {
        rColor = 0x99;
        gColor = 0x99;
        bColor = 0x99;
    }
    else if( pattern == 8 )
    {
        rColor = 0xCC;
        gColor = 0xCC;
        bColor = 0xCC;
    }
    mStar_WaitForDataBlanking();
    drvOSD_FrameColorRGB(rColor, gColor, bColor);
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

void msAPI_DeGammaLoadTbl_256E_16B_N(BYTE u8WinIndex, BYTE **pu8Table)
{
    BYTE i;
//#if ENABLE_LUT_AUTODOWNLOAD
//    msDrvClearDMAGammaDataBlock();
//#endif

    for (i = 0; i < 3; i++)
    {
#if ENABLE_LUT_AUTODOWNLOAD
        msDrvDeGammaWriteTbl(u8WinIndex, i, pu8Table); //msDrvDeGammaWriteTbl_600E_16B_ByDMA(u8WinIndex, i, pu8Table);
#else
        msDrvDeGammaLoadTbl_256E_16B_N(u8WinIndex, i, pu8Table);
#endif
    }
	
#if ENABLE_LUT_AUTODOWNLOAD
	WriteTriggerADLxvYCCGamma(DEGAMMA_ADL_ADDR, TRUE);
#endif
}

void msAPI_FixGammaLoadTbl_256E_12B(BYTE u8WinIndex, BYTE **pu8TableIndex)
{
	UNUSED(u8WinIndex);
/*    BYTE i;

//    for (i = 0; i < 3; i++)
	{
#if ENABLE_LUT_AUTODOWNLOAD
        msDrvFixGammaWriteTbl(u8WinIndex, i, pu8TableIndex);
#else
        msDrvFixGammaLoadTbl_256E_10B(u8WinIndex, i, pu8TableIndex);
#endif
	}
*/
#if ENABLE_LUT_AUTODOWNLOAD
BYTE u8TgtChannel=0;
BYTE* pu8Tab1;
//extern code BYTE tblFixGamma10_N[];
//extern code BYTE tblFixGamma22_N[];

    for (u8TgtChannel=0; u8TgtChannel<3; u8TgtChannel++)
    {
        pu8Tab1 = pu8TableIndex[0];
        //pu8Tab1 = (BYTE*)((void*)tblFixGamma22_N);
        #if XVYCC_GAMMA_EXT
        WriteFixGamma2Dram(u8TgtChannel, pu8Tab1, TRUE);
        #else
        WriteFixGamma2Dram(u8TgtChannel, pu8Tab1, FALSE);
        #endif
    }
    WriteTriggerADLxvYCCGamma(DEGAMMA_ADL_ADDR, TRUE);
    #else //RIU TBD
    UNUSED(pu8TableIndex);
    #endif
}

void msAPI_GammaLoadTbl_256E_14B_DICOM(BYTE u8WinIndex, BYTE **pu8TableIndex)
{
//#if ENABLE_LUT_AUTODOWNLOAD
//    msDrvClearDMAGammaDataBlock();
//#endif
    msDrvGammaLoadTbl_256E_14B_76Bytes(u8WinIndex, pu8TableIndex);
}
void msAPI_GammaLoadTbl_256E_14B(BYTE u8WinIndex, BYTE **pu8TableIndex)
{
//#if ENABLE_LUT_AUTODOWNLOAD
//    msDrvClearDMAGammaDataBlock();
//#endif
    msDrvGammaLoadTbl_256E_14B(u8WinIndex, pu8TableIndex);
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

void msAPI_ForceUseBothContrast_Get( BYTE u8DispWin, BOOL bForceUseBothContrast )
{
    msForceUseBothContrast(u8DispWin, bForceUseBothContrast);
}

void msAPI_AdjustRGBColor( BYTE u8DispWin, WORD u16Contrast, WORD u16Red, WORD u16Green, WORD u16Blue)
{
    //printf("[msAPI_AdjustRGBColor]u16Contrast:%x, u16Red:0x%x, u16Green:0x%x, u16Blue:0x%x\n",u16Contrast, u16Red, u16Green, u16Blue);
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

void msAPI_IP2Pattern_SetPureColorPattern(BYTE R,BYTE G,BYTE B)
{
    IP2PATTERNColor color ={R, G, B};//{0xEF, 0XAB, 0xCD};
    mdrv_IP2Pattern_SetPureColorPattern(&color);
}

void mapi_Adjust_EnableNonStdCSC_Set(BOOL u8Enable)
{
    mdrv_Adjust_EnableNonStdCSC_Set(u8Enable);
}

BOOL mapi_Adjust_EnableNonStdCSC_Get(void)
{
    return mdrv_Adjust_EnableNonStdCSC_Get();
}

#if (USEFLASH && COLORDATA_USEFLASH && ENABLE_DeltaE)
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
void msAPI_VideoHueSaturation(void)
{
    msDrv_VideoHueSaturation();
}
Bool SetColorModeDemo(void)
{
    return msDrv_SetColorModeDemo();
}

#if (LiteMAX_Baron_UI == 1)
//======== FACTORY MENU FUNCTION ===============//
void mStar_FAdjustBrightness( BYTE brightness )
{
    brightness = (((DWORD)brightness * (RealMaxBrightnessValue - RealMinBrightnessValue)) / MaxBrightnessValue) + RealMinBrightnessValue;
    BacklightNow = BacklightPrev = brightness;

    #if BRIGHTNESS_INVERSE  //+Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-brightness);
    #else                           //-Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, brightness);
    #endif
}
#endif

