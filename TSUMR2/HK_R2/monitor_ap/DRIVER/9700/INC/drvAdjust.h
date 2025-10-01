
#ifndef _DRVADJUST_H_
#define _DRVADJUST_H_

#define MinBlackLevelValue  0
#define MaxBlackLevelValue  255

typedef struct{
    BYTE R;
    BYTE G;
    BYTE B;
} IP2PATTERNColor;

typedef struct{
    WORD HStart;
    WORD HEnd;
    WORD VStart;
    WORD VEnd;
} IP2PATTERNPIPWIN;

typedef enum
{
    Pat_Horizontal=0,
    Pat_Vertical,
    Pat_Horizontal_Flipped,
    Pat_Vertical_Flipped
} PATTERN_DIRECTION;

extern void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel );

extern void msDrvCustomHueEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvAdjustCustomHue(BYTE u8Value, BYTE u8ColorIndex);
extern void msDrvCustomSatEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvAdjustCustomSat(BYTE u8Value, BYTE u8ColorIndex);
extern void msDrvCustomBriEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvAdjustCustomBri(BYTE u8Value, BYTE u8ColorIndex);

extern void msDrvDeCompressFMT46Gamma(BYTE *pu8GammaTblIdx, DWORD* dwDecompressGamma);

extern void msDrvDeGammaLoadTbl_256E_16B_N(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8DeGammaTbl);
extern void msDrvFixGammaLoadTbl_256E_10B(BYTE u8WinIdx, BYTE u8Color,  BYTE **pu8FixGammaTblIdx);
#if ENABLE_LUT_AUTODOWNLOAD
extern void msDrvDeGammaWriteTbl(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx );
extern void msDrvFixGammaWriteTbl(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx );
extern void WritePostComprssGamma2Dram ( BYTE u8Color, BYTE *pu8GammaTblIdx);
extern void msDrvGammaWriteTbl_256E_14B_ByDMA(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx );
#endif
extern void msDrvGammaLoadTbl_256E_14B(BYTE u8WinIdx, BYTE **GammaTblIdx);
extern void msDrvGammaLoadTbl_256E_14B_76Bytes(BYTE u8WinIdx, BYTE **GammaTblIdx);
extern void WritePostComprssGamma2Dramby76Bytes( BYTE u8Color, BYTE *pu8GammaTblIdx);
extern void msDrvDeGammaEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvFixGammaEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvGammaEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvColorMatrixEnable(BYTE u8WinIdx, Bool bEnable, Bool bCarryEn);

/* Get Window Color Report*/
extern void msDrvWinReportEnable(Bool bEnable);
extern void msDrvDisplayWindowEnable(Bool bEnable);
extern void msDrvWinReportSetRange(WORD u16HStart, WORD u16HEnd, WORD u16VStart, WORD u16VEnd);
extern void msDrvWinReportGetWeightingPixelCountBySetRange(WORD *Rweight, WORD *Gweight, WORD *Bweight);
extern void mdrv_IP2Pattern_Enable(bool bEnable);
extern void mdrv_IP2Pattern_Reset(void);
extern void mdrv_IP2Pattern_SetPureColorPattern(IP2PATTERNColor *stColor);

#endif

#if ENABLE_SUPER_RESOLUTION
extern void msDrv_SetupSuperResolution( BYTE SuperResolutionMode );
extern void msDrv_InitalSRSetting(void);
extern void msDrv_SuperResolutionOnOff(BOOL bOn);
#endif

extern void msDrv_VideoHueSaturation(void);

extern Bool msDrv_SetColorModeDemo(void);


void mdrv_Adjust_EnableNonStdCSC_Set(BOOL u8Enable);
BOOL mdrv_Adjust_EnableNonStdCSC_Get(void);
