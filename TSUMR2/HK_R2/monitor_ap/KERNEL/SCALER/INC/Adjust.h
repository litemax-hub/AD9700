#ifndef _ADJSUT_H_
#define _ADJSUT_H_

#ifdef _ADJSUT_C_
  #define _ADJSUTDEC_
#else
  #define _ADJSUTDEC_ extern
#endif

typedef struct
{
  BYTE u8Red;
  BYTE u8Green;
  BYTE u8Blue;
} ColorType;
//HDR Backlight Range Control
#define BRIGHTNESS_HDR_MAX 0xFF //Need to modify by project
#define BRIGHTNESS_HDR_MIN 0x00 //Need to modify by project

_ADJSUTDEC_ WORD GetRealRgbGain(WORD u16OsdValue);
_ADJSUTDEC_ void mStar_AdjustBrightness( BYTE brightness );
_ADJSUTDEC_ void mStar_AdjustDcrBrightness( void );
_ADJSUTDEC_ void mStar_AdjustBlackLevel( BYTE BlackLevel );
_ADJSUTDEC_ void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel );
_ADJSUTDEC_ void mStar_AdjustContrast( BYTE contrast );

_ADJSUTDEC_ void mStar_AdjustHPosition( WORD position );
_ADJSUTDEC_ void mStar_AdjustVPosition( WORD position );
#if UsesRGB
_ADJSUTDEC_ void mStar_AdjustRedColor( BYTE rColor, BYTE gColor, BYTE bColor );
_ADJSUTDEC_ void mStar_AdjustGreenColor( BYTE rColor, BYTE gColor, BYTE bColor );
_ADJSUTDEC_ void mStar_AdjustBlueColor( BYTE rColor, BYTE gColor, BYTE bColor );
#else
_ADJSUTDEC_ void mStar_AdjustRedColor( BYTE color, BYTE contrast );
_ADJSUTDEC_ void mStar_AdjustGreenColor( BYTE color, BYTE contrast );
_ADJSUTDEC_ void mStar_AdjustBlueColor( BYTE color, BYTE contrast );
#endif
_ADJSUTDEC_ void msAPI_AdjustVolume( BYTE u8Volume );

_ADJSUTDEC_ void mStar_WaitForDataBlanking( void );
_ADJSUTDEC_ void mStar_AdjustBackgoundColor( BYTE pattern );

_ADJSUTDEC_ void msAPI_CustomHueEnable(BYTE u8WinIndex, Bool bEnable);
_ADJSUTDEC_ void msAPI_CustomSatEnable(BYTE u8WinIndex, Bool bEnable);
_ADJSUTDEC_ void msAPI_CustomBriEnable(BYTE u8WinIndex, Bool bEnable);

_ADJSUTDEC_ void msAPI_DeGammaLoadTbl_256E_16B_N(BYTE u8WinIndex, BYTE **pu8Table);
_ADJSUTDEC_ void msAPI_FixGammaLoadTbl_256E_12B(BYTE u8WinIndex, BYTE **pu8TableIndex);
_ADJSUTDEC_ void msAPI_GammaLoadTbl_256E_14B(BYTE u8WinIndex, BYTE **pu8TableIndex);
_ADJSUTDEC_ void msAPI_GammaLoadTbl_256E_14B_DICOM(BYTE u8WinIndex, BYTE **pu8TableIndex);
_ADJSUTDEC_ void msAPI_GammaEnable(BYTE u8WinIndex, BYTE bEnable);
_ADJSUTDEC_ void msAPI_DeGammaEnable(BYTE u8WinIndex, BYTE bEnable);
_ADJSUTDEC_ void msAPI_FixGammaEnable(BYTE u8WinIndex, BYTE bEnable);
_ADJSUTDEC_ void msAPI_ColorMatrixEnable(BYTE u8WinIndex, BYTE bEnable, BYTE bCarryEn);
_ADJSUTDEC_ void msAPI_AdjustRGBColor( BYTE u8DispWin, WORD u16Contrast, WORD u16Red, WORD u16Green, WORD u16Blue);

_ADJSUTDEC_ void msAPI_WinReportEnable(Bool bEnable);
_ADJSUTDEC_ void msAPI_DisplayWindowEnable(Bool bEnable);
_ADJSUTDEC_ void msAPI_WinReportSetRange(WORD u16HStart, WORD u16HEnd, WORD u16VStart, WORD u16VEnd);
_ADJSUTDEC_ void msAPI_WinReportGetWeightingPixelCountBySetRange(WORD *Rweight, WORD *Gweight, WORD *Bweight);
_ADJSUTDEC_ void msAPI_IP2Pattern_SetPureColorPattern(BYTE R,BYTE G,BYTE B);
_ADJSUTDEC_ void mapi_Adjust_EnableNonStdCSC_Set(BOOL u8Enable);
_ADJSUTDEC_ BOOL mapi_Adjust_EnableNonStdCSC_Get(void);

_ADJSUTDEC_ WORD PWMBoundaryClamp(WORD PWMvalue);
_ADJSUTDEC_ void msAPI_SetGlobalBrightnessPWM(WORD u16Brightness);
_ADJSUTDEC_ void msAPI_appAdjust_FlashInit(void);
_ADJSUTDEC_ void msAPI_appAdjust_FlashWriteByte(Bool bDoWP, DWORD u32Addr, BYTE u8Value);
_ADJSUTDEC_ void msAPI_appAdjust_FlashWriteTbl(Bool bDoWP, DWORD u32Addr, BYTE *buffer, DWORD count);
#endif
#if ENABLE_SUPER_RESOLUTION
_ADJSUTDEC_ void mStar_SetupSuperResolution( BYTE SuperResolutionMode );
_ADJSUTDEC_ void msInitalSRSetting(void);
_ADJSUTDEC_ void msSuperResolutionOnOff(BOOL bOn);
#endif
_ADJSUTDEC_ void msAPI_VideoHueSaturation(void);
_ADJSUTDEC_ Bool SetColorModeDemo(void);
_ADJSUTDEC_ void mStar_FAdjustBrightness( BYTE brightness );

