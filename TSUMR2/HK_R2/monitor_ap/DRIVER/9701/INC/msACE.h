
/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: msACE.h
 [Date]:        01-Dec-2003
 [Comment]:
   Color adjust header file.
 [Reversion History]:
*******************************************************************************/

#ifndef _MSACE_H_
#define _MSACE_H_

#ifdef _MSACE_C_
  #define _MSACEDEC_
#else
  #define _MSACEDEC_ extern
#endif

#include "ComboApp.h"
//#include "Global.h"

#define MAIN_WINDOW 0
#define SUB_WINDOW  1

#define REG_ADDR_SRGBCM_MAIN    (SC25_04)
#define REG_ADDR_SRGBCM_SUB     (SC25_24)

typedef enum{
    
    E_INPUT_LIMIT_RGB,
    E_INPUT_LIMIT_YCC601,
    E_INPUT_LIMIT_YCC709,
    E_INPUT_LIMIT_2020NCL,
    E_INPUT_FULL_RGB,
    E_INPUT_FULL_YCC601,
    E_INPUT_FULL_YCC709,
    E_INPUT_FULL_2020NCL,
    E_MAX_INPUT_SOURCE_TYPE,
    

} E_INPUT_SOURCE_TYPE;

typedef enum {
    R_CR,
    G_Y,
    B_CB,
} E_CSC_CH;

typedef enum 
{
    INPUT_COLOR_AUTO,
    INPUT_COLOR_RGB,
    INPUT_COLOR_YUV,
    INPUT_COLOR_MAX,
}E_INPUT_COLOR_FORMAT;

typedef enum
{
    INPUT_RANGE_AUTO,
    INPUT_RANGE_FULL,
    INPUT_RANGE_LIMIT,
    INPUT_RANGE_MAX,

}E_INPUT_COLOR_RANGE;

typedef enum
{
    //WIN_COLOR_AUTO,
    WIN_COLOR_RGB,
    WIN_COLOR_YUV,
    WIN_COLOR_MAX,

}E_WIN_COLOR_FORMAT;

typedef enum
{
    //the same index as OSD menu
    WIN_COLOR_RANGE_FULL,
    WIN_COLOR_RANGE_LIMIT,
    WIN_COLOR_RANGE_AUTO,
    WIN_COLOR_RNAGE_MAX,

}E_WIN_COLOR_RANGE;

// sRGB matrix for PC
_MSACEDEC_ xdata short *tSrgbMatrix;

// Color correction matrix for Video
_MSACEDEC_ xdata const short *tColorCorrectionMatrix;
//for tool
//extern xdata short g_sCommandMatrix[3][3];


extern BYTE xdata g_bAccOnOff;

extern code short tStandardColorCorrectionMatrix[][3];
extern code short tMultimediaMatrix[][3];
extern code short tGameMatrix[][3];

extern code short tVideoStandardColorCorrectionMatrix[][3];
extern code short tMovieColorCorrectionMatrix[][3];
extern code short tVideoSportsColorCorrectionMatrix[][3];
extern code short tVideoNatureColorCorrectionMatrix[][3];
///////////////////////////////////////////////////////////////////////////////
// Basic color control function(Library)
///////////////////////////////////////////////////////////////////////////////


// Must call this function before using other function
void InitACEVar(void);

#define NO_CHANGE_SHARPNESS 0xFF
#define NO_CHANGE_STEP      0x80
void InitialPeaking(void);
void msACESetRGBMode(BOOL bEnable);
//void msAdjustSharpness( BYTE ucSharpness, BYTE ucStep);// 2004/12/10
void msAdjustSharpness(BYTE bScalerWin ,BYTE ucSharpness, BYTE ucStep );

void msAdjustSubBrightness(BYTE bScalerWin,  BYTE u8brightnessR, BYTE u8brightnessG, BYTE u8brightnessB );
void msAdjustVideoContrast(BYTE bScalerWin, BYTE ucContrast );// 2004/12/10
void msAdjustVideoSaturation(BYTE bScalerWin, BYTE ucSaturation );// 2004/12/10
void msAdjustVideoHue(BYTE bScalerWin, BYTE ucHue );// 2004/12/10
//void msAdjustHSC(BYTE bScalerWin,BYTE ucHue, BYTE ucSaturation, BYTE ucContrast);// 2004/12/10


///////////////////////////////////////////////////////////////////////////////
// Special color control function
///////////////////////////////////////////////////////////////////////////////
void msAdjustVideoRGB(BYTE bScalerWin, BYTE ucRCon, BYTE ucGCon, BYTE ucBCon);
void msAdjustVideoContrastRGB(BYTE bScalerWin, BYTE ucContrast, BYTE ucRCon, BYTE ucGCon, BYTE ucBCon);
extern void msAdjustHSC(BYTE bScalerWin,BYTE ucHue, BYTE ucSaturation, BYTE ucContrast);
void msAdjustPCContrast( BYTE bScalerWin, BYTE ucContrast );
void msAdjustPCRGB( BYTE bScalerWin, BYTE ucRed, BYTE ucGreen, BYTE ucBlue );
void msAdjustPCContrastRGB( BYTE bScalerWin, BYTE ucContrast, BYTE ucRed, BYTE ucGreen, BYTE ucBlue );

//#define ADJUST_PC_CONTRAST_RGB(bScalerWin)  msAdjustPCContrastRGB( bScalerWin, g_ColorSetting.Contrast, g_ColorTempSetting.RedColor, g_ColorTempSetting.GreenColor, g_ColorTempSetting.BlueColor )

//extern void msACEPCsRGBCtl( BYTE bEnable );
//extern void msACEPCYUV2RGBCtl( BYTE bEnable );


///////////////////////////////////////////////////////////////////////////////
// ACC
///////////////////////////////////////////////////////////////////////////////
void msAccSetup( WORD hstart_point, WORD hend_point, WORD vstart_point, WORD vend_point);
void msAccOnOff( BYTE on );

void LoadACETable(BYTE bScalerWin, BYTE *pColorTable,BYTE ucHue,BYTE ucSaturation,BYTE ucSubContrast);// 2004/11/2
void msACESetHDTVMode(BYTE isHDTV);

///////////////////////////////////////////////////////////////////////////////
// XVYcc Global Hue/Sat
///////////////////////////////////////////////////////////////////////////////
void msAdjustXVYccVideoSaturation(BYTE bScalerWin, BYTE ucSaturation );
void msAdjustXVYccVideoHue(BYTE bScalerWin, BYTE ucHue );
void msAdjustXVYCCVideoContrastRGB(BYTE bScalerWin, BYTE ucContrast, BYTE ucRCon, BYTE ucGCon, BYTE ucBCon);


void msFormatMatrixToS2D10(short* psMatrix);
void msWritexvYccColorMatrix( BYTE ucWinIndex, short* psMatrix);

//---------------------------------------------------------------------------
// Specify the formula of YUV to RGB.
//---------------------------------------------------------------------------
#define ACE_YUV_TO_RGB_MATRIX_SDTV  0   // Use Std CSCM for SDTV
#define ACE_YUV_TO_RGB_MATRIX_HDTV  1   // Use Std CSCM for HDTV
#define ACE_YUV_TO_RGB_MATRIX_USER  2   // Use user define CSCM, specified by tUserYVUtoRGBMatrix
#define ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT  3   // Use Std CSCM for SDTV
#define ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT  4   // Use Std CSCM for HDTV
#define ACE_YUV_TO_RGB_MATRIX_SDTV_10Bit  5   // Use Std CSCM for SDTV
#define ACE_YUV_TO_RGB_MATRIX_HDTV_10Bit  6   // Use Std CSCM for HDT
#define ACE_YUV_TO_RGB_MATRIX_HDTV_YUV_LMITE 7  // Use Std CSCM for HDTV with YUV
#define ACE_YUV_TO_RGB_MATRIX_SDTV_YUV_LMITE   8// Use Std CSCM for SDTV with YUV
#define ACE_YUV_TO_RGB_MATRIX_BT2020  9  // Use Std BT2020
#define ACE_YUV_TO_RGB_MATRIX_BT2020_LIMIT  10  // Use Std BT2020

void msACESetRGBColorRange(BYTE bScalerWin, Bool En, Bool bLimitRange);
void msSetVideoColorMatrix_Contrast(BYTE bScalerWin, BYTE ucContrast);


// ---------- PreGain /PostGain from MST9U ----------

void msEnablePreContrast(BYTE ucWinIndex, BOOL bEnable);
void msEnablePreOffset(BYTE ucWinIndex, BOOL bEnable);
void msEnablePostContrast( BYTE ucWinIndex,BOOL bEnable);
void msEnablePostOffset( BYTE ucWinIndex,BOOL bEnable);
void msAdjustPreRGBContrast(BYTE ucWinIndex);
void msAdjustPreRGBOffset(BYTE ucWinIndex);
void msAdjustPostRGBContrast(BYTE ucWinIndex);
void msAdjustPostRGBOffset(BYTE ucWinIndex);
void msAdjustRGBGainContrast( BYTE u8DispWin, WORD u16Contrast, WORD u16Red, WORD u16Green, WORD u16Blue);
void msSetRGBContrast(BYTE ucWinIndex, WORD ucContrast, WORD ucRCon, WORD ucGCon, WORD ucBCon);
void msSetUserRGBGain(BYTE ucWinIndex, WORD u16RCon, WORD u16GCon, WORD u16BCon);
void msSetContrast( BYTE ucWinIndex, WORD u16Contrast);
void msSetRGBGain(BYTE ucWinIndex, WORD u16RCon, WORD u16GCon, WORD u16BCon);
void msSetRGBOffset(BYTE ucWinIndex, BYTE ucRoff, BYTE ucG0ff, BYTE ucBoff);
BOOL msForceUseBothContrast_Get( BYTE ucWinIndex);
void msForceUseBothContrast( BYTE ucWinIndex, BOOL bForceUseBothContrast);
void msAdjustRGBContrast(BYTE ucWinIndex );
void msACECSCControl(void);
void msACEVIPRGBdeltaMode_Enable(BYTE ucWinIndex, Bool bEnable);
BOOL msACEGetEnableGlobalHueSat(void);
extern ST_COMBO_COLOR_FORMAT IsOSDCSCControl(BOOL bEnable);
extern void msACESetEnableY2RMatrix(BYTE bScalerWin, BOOL bEnable);
void msACESetVIPAllColor(void);
void msACESetVOPAllColor(void);

void msACESetWinColor(BOOL bMain, E_WIN_COLOR_FORMAT cf);
void msACESetWinColorRange(BOOL bMain, E_WIN_COLOR_RANGE cr);

E_WIN_COLOR_FORMAT msACEGetWinColor(void);
E_WIN_COLOR_RANGE msACEGetWinColorRange(void);
BOOL mdrv_ACE_MweEnable_Get(void);
void mdrv_ACE_MweEnable_Set(BOOL u8IsEnabledMEW);

void msACESetIPR2YEnable(Bool bEn);
void msACESetIPR2YSub16Enable(BYTE ch, Bool bEn);
#endif
