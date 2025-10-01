
#ifndef _DRVADJUST_H_
#define _DRVADJUST_H_

#define MinBlackLevelValue  0
#define MaxBlackLevelValue  255

extern void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel );

extern void msDrvCustomHueEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvAdjustCustomHue(BYTE u8Value, BYTE u8ColorIndex);
extern void msDrvCustomSatEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvAdjustCustomSat(BYTE u8Value, BYTE u8ColorIndex);
extern void msDrvCustomBriEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvAdjustCustomBri(BYTE u8Value, BYTE u8ColorIndex);

extern void msDrvDeCompressFMT46Gamma(BYTE *pu8GammaTblIdx, DWORD* dwDecompressGamma);

extern void msDrvDeGammaLoadTbl_256E_16B_N(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8DeGammaTbl);

extern void msDrvFixGammaLoadTbl_256E_12B_Extend(BYTE u8WinIndex, BYTE **pu8TableIndex, BOOL bExtendMode);
#if ENABLE_LUT_AUTODOWNLOAD
extern void msDrvDeGammaWriteTbl(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx );
extern void msDrvFixGammaWriteTbl(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx );
extern void msDrvClearDMADeGammaDataBlock(void);
extern void msDrvDeGammaWriteTbl_1024E_16B_ByDMA(BYTE u8Color, BYTE **pu8GammaTblIdx);
extern void msDrvDeGamma_DMA_Reader_Trigger(BYTE u8WinIdx);
#endif
extern void msDrvDeGammaLoadTbl_1024E_16B_N(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8DeGammaTbl);
extern void msDrvGammaLoadTbl_256E_14B(BYTE u8WinIdx, BYTE **GammaTblIdx);
extern void msDrvGammaWriteTbl_256E_14B_ByDMA(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx );
void msDrvGammaWriteTbl_1024E_12B_N(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx );

extern void msDrvDeGammaEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvFixGammaEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvGammaEnable(BYTE u8WinIdx, Bool bEnable);
extern void msDrvColorMatrixEnable(BYTE u8WinIdx, Bool bEnable, Bool bCarryEn);
extern void msDrvGammaLoadTbl_1024E_12B(BYTE u8WinIdx, BYTE **pu8GammaTblIdx);
extern void msDrvGammaLoadTbl_1024E_12B_N(BYTE u8WinIdx, BYTE **pu8GammaTblIdx);

/* Get Window Color Report*/
extern void msDrvWinReportEnable(Bool bEnable);
extern void msDrvDisplayWindowEnable(Bool bEnable);
extern void msDrvWinReportSetRange(WORD u16HStart, WORD u16HEnd, WORD u16VStart, WORD u16VEnd);
extern void msDrvWinReportGetWeightingPixelCountBySetRange(WORD *Rweight, WORD *Gweight, WORD *Bweight);

extern void msSetVIPBypass(BOOL bIsBypass);
extern void msSetVOP2Bypass(BOOL bIsBypass);
extern void msSetPQGABypass(BOOL bIsBypass);

void mdrv_Adjust_DS_Start(void);
void mdrv_Adjust_DS_End(void);
void mdrv_Adjust_DS_W2BMask(DWORD u32Address, WORD u16Data, WORD u16Mask);
void mdrv_Adjust_DS_WBMask(DWORD u32Address, BYTE u8Data, BYTE u8Mask);
void mdrv_Adjust_DS_W4B(DWORD u32Address, DWORD u32Value);
void mdrv_Adjust_DS_W2B(DWORD u32Address, WORD u16Data);
void mdrv_Adjust_DS_WB(DWORD u32Address, BYTE u8Data);
void mdrv_Adjust_DS_WBit(DWORD u32Address, Bool bBit, BYTE u8BitPos);

#endif

#if ENABLE_SUPER_RESOLUTION
extern void msDrv_SetupSuperResolution( BYTE SuperResolutionMode );
extern void msDrv_InitalSRSetting(void);
extern void msDrv_SuperResolutionOnOff(BOOL bOn);
#endif


extern Bool msDrv_SetColorModeDemo(void);
extern void mdrv_Adjust_EnableNonStdCSC_Set(BOOL u8Enable);
extern BOOL mdrv_Adjust_EnableNonStdCSC_Get(void);
