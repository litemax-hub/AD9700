#include "Global.h"
//#if ENABLE_HDR
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "ComboApp.h"
#include "drvHDR.h"
#include "apiHDR.h"
#include "drvHDRLUT.h"
//For panel gamut info
#include "drvHDRPanelInfo.h"
#include "AutoGamma.h"
#include "msflash.h"
#include "msACE.h"
#include "NVRam.h"
#undef MST9U4 // TSUMR2 TBD
#define MST9U4 CHIP_ID // TSUMR2 TBD
//#define MST9U3 0
#ifndef UNUSED //to avoid compile warnings...
#define UNUSED(var) (void)((var)=(var))
#endif
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a ,b) (((a) < (b)) ? (a) : (b))
//#define bool BOOL
#define true TRUE
#define false FALSE

typedef enum
{
    _R = 0,
    _G = 1,
    _B = 2,
    _W = 3,
} RGBW;

#if (CHIP_ID == MST9U4)
#define  ENABLE_HDR_CFD 0
#else
#define  ENABLE_HDR_CFD 0
#endif

#if ENABLE_HDR_CFD
extern void setPanelMaxLum(MS_U16 u16MaxLumNits);
extern void setHdrCustomTMO(MS_BOOL IsUseCustomHDR, MS_U16 *pSrcNits, MS_U16 *pTrgNits);
#include "drvCFD.h"
#endif

xdata ST_COMBO_COLOR_FORMAT _cf;
xdata SMPTE2086Metadata _metadata;
xdata HDRUserPrefSettings _pref;
xdata HDRUserPrefSettings* _pPref = NULL;

#if ENABLE_ACHDR_FUNCTION
xdata StoredHDRPanelInfo g_HDRPanelInfo;
xdata StoredFormatOfHDRColorTemp g_HDRColorTemp;
xdata StoredHDRLumMap g_HDRLumMap;
#endif

#if (CHIP_ID == MST9U4)
//xdata HDR_ToneMappingData _hdrTmd;
//BOOL _IsUseRGBFakeY = FALSE;
#endif

xdata BOOL _IsDebugMsg = FALSE;

//PANEL GAMUT
xdata BOOL  _IsEnEotfDarkEnhance = TRUE;
xdata BOOL _IsPanelInfoChanged = FALSE;
xdata BOOL _IsUserPrefChanged = FALSE;
xdata BOOL _IsRefColorimetry = TRUE;
xdata double _panelMaxLum = PANEL_MAX_LUMINANCE;
xdata double _panelMinLum = PANEL_MIN_LUMINANCE;
xdata GamutDesc _panelGamut = {TRUE, {PANEL_GAMUT_Rx, PANEL_GAMUT_Ry}, //R
                                               {PANEL_GAMUT_Gx, PANEL_GAMUT_Gy}, //G
                                               {PANEL_GAMUT_Bx, PANEL_GAMUT_By}, //B
                                               {PANEL_GAMUT_Wx, PANEL_GAMUT_Wy} //W
                                               };//Valid
xdata HDRColorTempDesc _HDRColorTemp = {FALSE, {0, 0, 0}};

//Tone Mapping Descriptor (nits)
typedef struct
{
    double srcMinLum;
    double srcMaxLum;
    double trgMinLum;
    double trgMaxLum;
    double MaxCLL;
    double MaxFALL;
    BYTE IsValid;
}ToneMappingDesc;

//------------------------------------------
//HLG OETF/EOTF coefficant
xdata double _sysGma = 1.25;
//------------------------------------------
//SMPTE 2084 OETF/EOTF coefficant
code double _maxLumOfST2084 = 10000;
double _factor = 0.01; //PQ100 : 0.01, PQ 1K : 0.1, PQ 10K : 1
#if (CHIP_ID == MST9U3)
const double _m1 = 0.159301758;
const double _m2 = 78.84375;
const double _c1 = 0.8359375;
const double _c2 = 18.8515625;
const double _c3 = 18.6875;
#endif

//------------------------------------------------------------------
//Params for multi-window and IPMux configration by custom coding environment
//------------------------------------------------------------------
#define _maxWinNum 8
xdata BOOL _IsInitParams = FALSE;
xdata SMPTE2086Metadata _lastMetadata[_maxWinNum];
xdata HDR_Level _lastHDRLvl[_maxWinNum];
xdata BOOL _IsUseExtPortConfig[_maxWinNum];
xdata BOOL _IsDP[_maxWinNum];
xdata BOOL _IsUseExtPortIPMux[_maxWinNum];
xdata BYTE _extPortIPMux[_maxWinNum];
//------------------------------------------
xdata double _minLimRatio = 1.0;

//gaumt define
code const GamutDesc _DCIP3Gamut = {TRUE,
                                                         {0.6800, 0.3200}, //R
                                                         {0.2560, 0.6900}, //G
                                                         {0.1500, 0.0600}, //B
                                                         {0.3127, 0.3290}};//W

code const GamutDesc _sRGBGamut = {TRUE,
                                                        {0.6400, 0.3300}, //R
                                                        {0.3000, 0.6000}, //G
                                                        {0.1500, 0.0600}, //B
                                                        {0.3127, 0.3290}};//W

code const GamutDesc _BT2020Gamut = {TRUE,
                                                        {0.7080, 0.2920}, //R
                                                        {0.1700, 0.7970}, //G
                                                        {0.1310, 0.0460}, //B
                                                        {0.3127, 0.3290}};//W

code const GamutDesc _AdobeRGBGamut = {TRUE,
                                                            {0.6400, 0.3300}, //R
                                                            {0.2100, 0.7100}, //G
                                                            {0.1500, 0.0060}, //B
                                                            {0.3127, 0.3290}};//W
//------------------------------------------

//Mastering Display Gamut
xdata GamutDesc _masteringDispGamut = {FALSE, {0, 0}, {0, 0},{0, 0},{0, 0}};
//------------------------------------------

//Tone Mapping
xdata ToneMappingDesc _tmDesc = {FALSE, 0, 0 ,0, 0, 0, 0};
//------------------------------------------

#if (CHIP_ID == MST9U4)
#if 0
BYTE _tblGamma_sRGB_N[] =
{
    0x00,0x00,0x3B,0x03,0x6A,0x05,0x0F,0x07,0x67,0x08,0x90,0x09,0x98,0x0A,0x88,0x0B,
    0x64,0x0C,0x31,0x0D,0xF0,0x0D,0xA5,0x0E,0x51,0x0F,0xF4,0x0F,0x90,0x10,0x26,0x11,
    0xB6,0x11,0x41,0x12,0xC8,0x12,0x4A,0x13,0xC8,0x13,0x42,0x14,0xB9,0x14,0x2D,0x15,
    0x9E,0x15,0x0D,0x16,0x78,0x16,0xE2,0x16,0x49,0x17,0xAE,0x17,0x11,0x18,0x72,0x18,
    0xD1,0x18,0x2F,0x19,0x8B,0x19,0xE5,0x19,0x3E,0x1A,0x95,0x1A,0xEC,0x1A,0x40,0x1B,
    0x94,0x1B,0xE6,0x1B,0x37,0x1C,0x87,0x1C,0xD6,0x1C,0x24,0x1D,0x71,0x1D,0xBD,0x1D,
    0x08,0x1E,0x52,0x1E,0x9B,0x1E,0xE4,0x1E,0x2B,0x1F,0x72,0x1F,0xB8,0x1F,0xFD,0x1F,
    0x41,0x20,0x85,0x20,0xC8,0x20,0x0B,0x21,0x4C,0x21,0x8E,0x21,0xCE,0x21,0x0E,0x22,
    0x4D,0x22,0x8C,0x22,0xCA,0x22,0x08,0x23,0x45,0x23,0x81,0x23,0xBD,0x23,0xF9,0x23,
    0x34,0x24,0x6F,0x24,0xA9,0x24,0xE2,0x24,0x1C,0x25,0x54,0x25,0x8D,0x25,0xC5,0x25,
    0xFC,0x25,0x33,0x26,0x6A,0x26,0xA0,0x26,0xD6,0x26,0x0C,0x27,0x41,0x27,0x76,0x27,
    0xAB,0x27,0xDF,0x27,0x13,0x28,0x46,0x28,0x79,0x28,0xAC,0x28,0xDF,0x28,0x11,0x29,
    0x43,0x29,0x74,0x29,0xA6,0x29,0xD7,0x29,0x07,0x2A,0x38,0x2A,0x68,0x2A,0x98,0x2A,
    0xC7,0x2A,0xF7,0x2A,0x26,0x2B,0x55,0x2B,0x83,0x2B,0xB1,0x2B,0xDF,0x2B,0x0D,0x2C,
    0x3B,0x2C,0x68,0x2C,0x95,0x2C,0xC2,0x2C,0xEF,0x2C,0x1B,0x2D,0x48,0x2D,0x74,0x2D,
    0x9F,0x2D,0xCB,0x2D,0xF6,0x2D,0x21,0x2E,0x4C,0x2E,0x77,0x2E,0xA2,0x2E,0xCC,0x2E,
    0xF6,0x2E,0x20,0x2F,0x4A,0x2F,0x73,0x2F,0x9D,0x2F,0xC6,0x2F,0xEF,0x2F,0x18,0x30,
    0x41,0x30,0x69,0x30,0x91,0x30,0xBA,0x30,0xE2,0x30,0x09,0x31,0x31,0x31,0x59,0x31,
    0x80,0x31,0xA7,0x31,0xCE,0x31,0xF5,0x31,0x1C,0x32,0x42,0x32,0x69,0x32,0x8F,0x32,
    0xB5,0x32,0xDB,0x32,0x01,0x33,0x27,0x33,0x4C,0x33,0x72,0x33,0x97,0x33,0xBC,0x33,
    0xE1,0x33,0x06,0x34,0x2A,0x34,0x4F,0x34,0x74,0x34,0x98,0x34,0xBC,0x34,0xE0,0x34,
    0x04,0x35,0x28,0x35,0x4C,0x35,0x6F,0x35,0x93,0x35,0xB6,0x35,0xD9,0x35,0xFC,0x35,
    0x1F,0x36,0x42,0x36,0x65,0x36,0x88,0x36,0xAA,0x36,0xCD,0x36,0xEF,0x36,0x11,0x37,
    0x33,0x37,0x55,0x37,0x77,0x37,0x99,0x37,0xBA,0x37,0xDC,0x37,0xFD,0x37,0x1F,0x38,
    0x40,0x38,0x61,0x38,0x82,0x38,0xA3,0x38,0xC4,0x38,0xE5,0x38,0x05,0x39,0x26,0x39,
    0x46,0x39,0x67,0x39,0x87,0x39,0xA7,0x39,0xC7,0x39,0xE7,0x39,0x07,0x3A,0x27,0x3A,
    0x47,0x3A,0x66,0x3A,0x86,0x3A,0xA5,0x3A,0xC5,0x3A,0xE4,0x3A,0x03,0x3B,0x22,0x3B,
    0x41,0x3B,0x60,0x3B,0x7F,0x3B,0x9E,0x3B,0xBD,0x3B,0xDB,0x3B,0xFA,0x3B,0x18,0x3C,
    0x37,0x3C,0x55,0x3C,0x73,0x3C,0x91,0x3C,0xB0,0x3C,0xCE,0x3C,0xEB,0x3C,0x09,0x3D,
    0x27,0x3D,0x45,0x3D,0x62,0x3D,0x80,0x3D,0x9D,0x3D,0xBB,0x3D,0xD8,0x3D,0xF5,0x3D,
    0x13,0x3E,0x30,0x3E,0x4D,0x3E,0x6A,0x3E,0x87,0x3E,0xA3,0x3E,0xC0,0x3E,0xDD,0x3E,
    0xF9,0x3E,0x16,0x3F,0x33,0x3F,0x4F,0x3F,0x6B,0x3F,0x88,0x3F,0xA4,0x3F,0xC0,0x3F,
};
#endif
#endif

#if(CHIP_ID == MST9U3)
//Fix-Gamma Table
const code BYTE _tblGamma_inv2d2[] =
{
    0x00, 0x51, 0x10, 0xCC, 0x28, 0x21,
    0x74, 0xB7, 0x22, 0xF2, 0x29, 0x32,
    0x5B, 0x89, 0x33, 0xB5, 0xDF, 0x33,
    0x07, 0x2D, 0x44, 0x51, 0x74, 0x44,
    0x95, 0xB6, 0x44, 0xD5, 0xF4, 0x44,
    0x11, 0x2E, 0x55, 0x4A, 0x65, 0x55,
    0x80, 0x9A, 0x55, 0xB4, 0xCD, 0x55,
    0xE5, 0xFD, 0x55, 0x15, 0x2C, 0x66,
    0x43, 0x59, 0x66, 0x6F, 0x85, 0x66,
    0x9A, 0xAF, 0x66, 0xC4, 0xD8, 0x66,
    0xEC, 0x00, 0x76, 0x14, 0x27, 0x77,
    0x3A, 0x4D, 0x77, 0x5F, 0x72, 0x77,
    0x84, 0x96, 0x77, 0xA7, 0xB9, 0x77,
    0xCA, 0xDC, 0x77, 0xED, 0xFD, 0x77,
    0x0E, 0x1F, 0x88, 0x2F, 0x3F, 0x88,
    0x4F, 0x5F, 0x88, 0x6F, 0x7E, 0x88,
    0x8E, 0x9D, 0x88, 0xAC, 0xBB, 0x88,
    0xCA, 0xD9, 0x88, 0xE8, 0xF7, 0x88,
    0x05, 0x13, 0x99, 0x22, 0x30, 0x99,
    0x3E, 0x4C, 0x99, 0x5A, 0x67, 0x99,
    0x75, 0x83, 0x99, 0x90, 0x9E, 0x99,
    0xAB, 0xB8, 0x99, 0xC5, 0xD2, 0x99,
    0xDF, 0xEC, 0x99, 0xF9, 0x06, 0xA9,
    0x12, 0x1F, 0xAA, 0x2B, 0x38, 0xAA,
    0x44, 0x50, 0xAA, 0x5D, 0x69, 0xAA,
    0x75, 0x81, 0xAA, 0x8D, 0x99, 0xAA,
    0xA5, 0xB0, 0xAA, 0xBC, 0xC8, 0xAA,
    0xD3, 0xDF, 0xAA, 0xEA, 0xF6, 0xAA,
    0x01, 0x0C, 0xBB, 0x18, 0x23, 0xBB,
    0x2E, 0x39, 0xBB, 0x44, 0x4F, 0xBB,
    0x5A, 0x65, 0xBB, 0x70, 0x7A, 0xBB,
    0x85, 0x90, 0xBB, 0x9A, 0xA5, 0xBB,
    0xB0, 0xBA, 0xBB, 0xC4, 0xCF, 0xBB,
    0xD9, 0xE4, 0xBB, 0xEE, 0xF8, 0xBB,
    0x02, 0x0C, 0xCC, 0x17, 0x21, 0xCC,
    0x2B, 0x35, 0xCC, 0x3F, 0x48, 0xCC,
    0x52, 0x5C, 0xCC, 0x66, 0x70, 0xCC,
    0x79, 0x83, 0xCC, 0x8D, 0x96, 0xCC,
    0xA0, 0xAA, 0xCC, 0xB3, 0xBD, 0xCC,
    0xC6, 0xCF, 0xCC, 0xD9, 0xE2, 0xCC,
    0xEC, 0xF5, 0xCC, 0xFE, 0x07, 0xDC,
    0x11, 0x1A, 0xDD, 0x23, 0x2C, 0xDD,
    0x35, 0x3E, 0xDD, 0x47, 0x50, 0xDD,
    0x59, 0x62, 0xDD, 0x6B, 0x74, 0xDD,
    0x7D, 0x85, 0xDD, 0x8E, 0x97, 0xDD,
    0xA0, 0xA8, 0xDD, 0xB1, 0xBA, 0xDD,
    0xC2, 0xCB, 0xDD, 0xD3, 0xDC, 0xDD,
    0xE5, 0xED, 0xDD, 0xF6, 0xFE, 0xDD,
    0x06, 0x0F, 0xEE, 0x17, 0x20, 0xEE,
    0x28, 0x30, 0xEE, 0x38, 0x41, 0xEE,
    0x49, 0x51, 0xEE, 0x59, 0x62, 0xEE,
    0x6A, 0x72, 0xEE, 0x7A, 0x82, 0xEE,
    0x8A, 0x92, 0xEE, 0x9A, 0xA2, 0xEE,
    0xAA, 0xB2, 0xEE, 0xBA, 0xC2, 0xEE,
    0xCA, 0xD2, 0xEE, 0xDA, 0xE1, 0xEE,
    0xE9, 0xF1, 0xEE, 0xF9, 0x01, 0xFE,
    0x08, 0x10, 0xFF, 0x18, 0x1F, 0xFF,
    0x27, 0x2F, 0xFF, 0x36, 0x3E, 0xFF,
    0x46, 0x4D, 0xFF, 0x55, 0x5C, 0xFF,
    0x64, 0x6B, 0xFF, 0x73, 0x7A, 0xFF,
    0x82, 0x89, 0xFF, 0x91, 0x98, 0xFF,
    0x9F, 0xA7, 0xFF, 0xAE, 0xB5, 0xFF,
    0xBD, 0xC4, 0xFF, 0xCB, 0xD3, 0xFF,
    0xDA, 0xE1, 0xFF, 0xE8, 0xF0, 0xFF,
};

const code BYTE _tblGamma_BT1886[] =
{
    0x00, 0xCC, 0x00, 0x5C, 0xC4, 0x11,
    0x1C, 0x66, 0x22, 0xA8, 0xE4, 0x22,
    0x1C, 0x4E, 0x33, 0x7E, 0xAC, 0x33,
    0xD8, 0x00, 0x43, 0x28, 0x4E, 0x44,
    0x72, 0x94, 0x44, 0xB6, 0xD6, 0x44,
    0xF6, 0x14, 0x54, 0x32, 0x50, 0x55,
    0x6C, 0x88, 0x55, 0xA2, 0xBE, 0x55,
    0xD8, 0xF0, 0x55, 0x0A, 0x22, 0x66,
    0x3A, 0x52, 0x66, 0x68, 0x7E, 0x66,
    0x96, 0xAC, 0x66, 0xC0, 0xD6, 0x66,
    0xEA, 0x00, 0x76, 0x14, 0x28, 0x77,
    0x3C, 0x50, 0x77, 0x62, 0x76, 0x77,
    0x88, 0x9C, 0x77, 0xAE, 0xC0, 0x77,
    0xD2, 0xE4, 0x77, 0xF4, 0x06, 0x87,
    0x18, 0x28, 0x88, 0x3A, 0x4A, 0x88,
    0x5A, 0x6A, 0x88, 0x7A, 0x8A, 0x88,
    0x9A, 0xAA, 0x88, 0xBA, 0xCA, 0x88,
    0xDA, 0xE8, 0x88, 0xF8, 0x06, 0x98,
    0x16, 0x24, 0x99, 0x32, 0x40, 0x99,
    0x50, 0x5E, 0x99, 0x6C, 0x7A, 0x99,
    0x88, 0x96, 0x99, 0xA4, 0xB0, 0x99,
    0xBE, 0xCC, 0x99, 0xDA, 0xE6, 0x99,
    0xF4, 0x00, 0xA9, 0x0E, 0x1A, 0xAA,
    0x28, 0x34, 0xAA, 0x40, 0x4E, 0xAA,
    0x5A, 0x66, 0xAA, 0x72, 0x7E, 0xAA,
    0x8C, 0x98, 0xAA, 0xA4, 0xB0, 0xAA,
    0xBC, 0xC8, 0xAA, 0xD4, 0xDE, 0xAA,
    0xEA, 0xF6, 0xAA, 0x02, 0x0E, 0xBB,
    0x18, 0x24, 0xBB, 0x30, 0x3A, 0xBB,
    0x46, 0x50, 0xBB, 0x5C, 0x68, 0xBB,
    0x72, 0x7C, 0xBB, 0x88, 0x92, 0xBB,
    0x9E, 0xA8, 0xBB, 0xB2, 0xBE, 0xBB,
    0xC8, 0xD2, 0xBB, 0xDE, 0xE8, 0xBB,
    0xF2, 0xFC, 0xBB, 0x06, 0x10, 0xCC,
    0x1C, 0x26, 0xCC, 0x30, 0x3A, 0xCC,
    0x44, 0x4E, 0xCC, 0x58, 0x62, 0xCC,
    0x6C, 0x74, 0xCC, 0x7E, 0x88, 0xCC,
    0x92, 0x9C, 0xCC, 0xA6, 0xB0, 0xCC,
    0xB8, 0xC2, 0xCC, 0xCC, 0xD6, 0xCC,
    0xDE, 0xE8, 0xCC, 0xF2, 0xFA, 0xCC,
    0x04, 0x0E, 0xDD, 0x16, 0x20, 0xDD,
    0x28, 0x32, 0xDD, 0x3A, 0x44, 0xDD,
    0x4E, 0x56, 0xDD, 0x5E, 0x68, 0xDD,
    0x70, 0x7A, 0xDD, 0x82, 0x8C, 0xDD,
    0x94, 0x9C, 0xDD, 0xA6, 0xAE, 0xDD,
    0xB6, 0xC0, 0xDD, 0xC8, 0xD0, 0xDD,
    0xDA, 0xE2, 0xDD, 0xEA, 0xF2, 0xDD,
    0xFC, 0x04, 0xED, 0x0C, 0x14, 0xEE,
    0x1C, 0x26, 0xEE, 0x2E, 0x36, 0xEE,
    0x3E, 0x46, 0xEE, 0x4E, 0x56, 0xEE,
    0x5E, 0x66, 0xEE, 0x6E, 0x76, 0xEE,
    0x7E, 0x86, 0xEE, 0x8E, 0x96, 0xEE,
    0x9E, 0xA6, 0xEE, 0xAE, 0xB6, 0xEE,
    0xBE, 0xC6, 0xEE, 0xCE, 0xD6, 0xEE,
    0xDE, 0xE6, 0xEE, 0xEE, 0xF6, 0xEE,
    0xFC, 0x04, 0xFE, 0x0C, 0x14, 0xFF,
    0x1C, 0x22, 0xFF, 0x2A, 0x32, 0xFF,
    0x3A, 0x42, 0xFF, 0x48, 0x50, 0xFF,
    0x58, 0x60, 0xFF, 0x66, 0x6E, 0xFF,
    0x76, 0x7C, 0xFF, 0x84, 0x8C, 0xFF,
    0x92, 0x9A, 0xFF, 0xA2, 0xA8, 0xFF,
    0xB0, 0xB8, 0xFF, 0xBE, 0xC6, 0xFF,
    0xCC, 0xD4, 0xFF, 0xDC, 0xE2, 0xFF,
    0xEA, 0xF0, 0xFF, 0xF8, 0xFE, 0xFF,
};
//------------------------------------------

const code BYTE _tblGamma_DCIP3[] =
{
    0x00, 0xE5, 0x10, 0x7A, 0xE4, 0x22,
    0x3B, 0x85, 0x33, 0xC6, 0x01, 0x43,
    0x37, 0x69, 0x44, 0x98, 0xC4, 0x44,
    0xED, 0x15, 0x54, 0x3A, 0x5E, 0x55,
    0x81, 0xA2, 0x55, 0xC2, 0xE1, 0x55,
    0xFF, 0x1C, 0x65, 0x38, 0x53, 0x66,
    0x6E, 0x88, 0x66, 0xA1, 0xBA, 0x66,
    0xD2, 0xEA, 0x66, 0x01, 0x18, 0x77,
    0x2E, 0x44, 0x77, 0x5A, 0x6F, 0x77,
    0x83, 0x98, 0x77, 0xAC, 0xBF, 0x77,
    0xD3, 0xE6, 0x77, 0xF9, 0x0B, 0x87,
    0x1D, 0x2F, 0x88, 0x41, 0x53, 0x88,
    0x64, 0x75, 0x88, 0x86, 0x97, 0x88,
    0xA7, 0xB7, 0x88, 0xC7, 0xD7, 0x88,
    0xE7, 0xF7, 0x88, 0x06, 0x15, 0x99,
    0x24, 0x33, 0x99, 0x42, 0x51, 0x99,
    0x5F, 0x6D, 0x99, 0x7C, 0x8A, 0x99,
    0x98, 0xA5, 0x99, 0xB3, 0xC1, 0x99,
    0xCE, 0xDB, 0x99, 0xE9, 0xF6, 0x99,
    0x03, 0x10, 0xAA, 0x1C, 0x29, 0xAA,
    0x36, 0x42, 0xAA, 0x4F, 0x5B, 0xAA,
    0x67, 0x73, 0xAA, 0x7F, 0x8B, 0xAA,
    0x97, 0xA3, 0xAA, 0xAF, 0xBA, 0xAA,
    0xC6, 0xD1, 0xAA, 0xDD, 0xE8, 0xAA,
    0xF3, 0xFE, 0xAA, 0x0A, 0x15, 0xBB,
    0x20, 0x2A, 0xBB, 0x35, 0x40, 0xBB,
    0x4B, 0x55, 0xBB, 0x60, 0x6B, 0xBB,
    0x75, 0x7F, 0xBB, 0x8A, 0x94, 0xBB,
    0x9E, 0xA8, 0xBB, 0xB3, 0xBD, 0xBB,
    0xC7, 0xD1, 0xBB, 0xDA, 0xE4, 0xBB,
    0xEE, 0xF8, 0xBB, 0x02, 0x0B, 0xCC,
    0x15, 0x1E, 0xCC, 0x28, 0x31, 0xCC,
    0x3B, 0x44, 0xCC, 0x4D, 0x57, 0xCC,
    0x60, 0x69, 0xCC, 0x72, 0x7B, 0xCC,
    0x84, 0x8E, 0xCC, 0x97, 0x9F, 0xCC,
    0xA8, 0xB1, 0xCC, 0xBA, 0xC3, 0xCC,
    0xCC, 0xD4, 0xCC, 0xDD, 0xE6, 0xCC,
    0xEE, 0xF7, 0xCC, 0xFF, 0x08, 0xDC,
    0x10, 0x19, 0xDD, 0x21, 0x2A, 0xDD,
    0x32, 0x3A, 0xDD, 0x42, 0x4B, 0xDD,
    0x53, 0x5B, 0xDD, 0x63, 0x6B, 0xDD,
    0x73, 0x7B, 0xDD, 0x83, 0x8B, 0xDD,
    0x93, 0x9B, 0xDD, 0xA3, 0xAB, 0xDD,
    0xB3, 0xBB, 0xDD, 0xC3, 0xCA, 0xDD,
    0xD2, 0xDA, 0xDD, 0xE1, 0xE9, 0xDD,
    0xF1, 0xF8, 0xDD, 0x00, 0x07, 0xEE,
    0x0F, 0x16, 0xEE, 0x1E, 0x25, 0xEE,
    0x2D, 0x34, 0xEE, 0x3C, 0x43, 0xEE,
    0x4A, 0x52, 0xEE, 0x59, 0x60, 0xEE,
    0x67, 0x6F, 0xEE, 0x76, 0x7D, 0xEE,
    0x84, 0x8B, 0xEE, 0x92, 0x99, 0xEE,
    0xA0, 0xA7, 0xEE, 0xAF, 0xB6, 0xEE,
    0xBC, 0xC3, 0xEE, 0xCA, 0xD1, 0xEE,
    0xD8, 0xDF, 0xEE, 0xE6, 0xED, 0xEE,
    0xF4, 0xFA, 0xEE, 0x01, 0x08, 0xFF,
    0x0F, 0x15, 0xFF, 0x1C, 0x23, 0xFF,
    0x29, 0x30, 0xFF, 0x37, 0x3D, 0xFF,
    0x44, 0x4A, 0xFF, 0x51, 0x58, 0xFF,
    0x5E, 0x65, 0xFF, 0x6B, 0x72, 0xFF,
    0x78, 0x7E, 0xFF, 0x85, 0x8B, 0xFF,
    0x92, 0x98, 0xFF, 0x9E, 0xA5, 0xFF,
    0xAB, 0xB1, 0xFF, 0xB8, 0xBE, 0xFF,
    0xC4, 0xCA, 0xFF, 0xD1, 0xD7, 0xFF,
    0xDD, 0xE3, 0xFF, 0xE9, 0xF0, 0xFF,
};
#endif

//DLC
code BYTE _defaultLumaCurve[16] = {0x08, 0x18, 0x28, 0x38,0x48,0x58,0x68,0x78,0x88,0x98,0xA8,0xB8,0xC8,0xD8,0xE8,0xF8};

#if 0
BOOL msGetHDRSrcIsDisplayPort(BYTE u8WinIdx, BYTE portNum)
{
    if (_IsUseExtPortConfig[u8WinIdx])
    {
        return _IsDP[u8WinIdx];
    }else{
        return INPUT_IS_DISPLAYPORT(portNum);
    }
}

void msSetHDRSrcIsDisplayPort(BYTE u8WinIdx, BOOL IsDP)
{
    _IsUseExtPortConfig[u8WinIdx] = TRUE;
    _IsDP[u8WinIdx] = IsDP;
}
#endif

void SetHDRColorFormat(ST_COMBO_COLOR_FORMAT cf)
{
    _cf = cf;
    //HDR_PrintData("Pixel Format(RGB/422/444/420) = %d", cf.ucColorType);
    //HDR_PrintData("Color Range(Dft/Limit/Full) = %d", cf.ucColorRange);
    //HDR_PrintData("Yuv Colorimetry(601/709/Nodata/EC) = %d", cf.ucYuvColorimetry);
    //HDR_PrintData("Extend Colorimetry(601/709/s601/adobe601/adobeRGB/2020CL/2020NCL) = %d", cf.ucColorimetry);
}

ST_COMBO_COLOR_FORMAT GetHDRColorFormat(void)
{
    return _cf;
}

#if 0
void msSetHDRPortIPMux(BYTE u8WinIdx, BYTE portIPMux)
{
    _IsUseExtPortIPMux[u8WinIdx] = TRUE;
    _extPortIPMux[u8WinIdx] = portIPMux;
}

BYTE msGetHDRPortIPMuxByWin(BYTE u8WinIdx)
{
    if (_IsUseExtPortIPMux[u8WinIdx])
    {
        return _extPortIPMux[u8WinIdx];
    }else{
        //Greg : Combo API
        //return (EN_COMBO_IP_SELECT)(g_InputPort[MapWin2Port(u8WinIdx)].eIPMux);
        return 0;
    }
}

BYTE msGetHDRPortIPMuxByPort(BYTE u8WinIdx, BYTE portNum)
{
    if (_IsUseExtPortIPMux[u8WinIdx])
    {
        return _extPortIPMux[u8WinIdx];
    }else{
        //Greg : Combo API
        //return (EN_COMBO_IP_SELECT)(g_InputPort[portNum].eIPMux);
        UNUSED(portNum);
        return 0;
    }
}
#endif

void msSetHDRDebugMsg(BOOL IsEnDebugMsg)
{
    _IsDebugMsg = IsEnDebugMsg;
}

void HDR_PrintData( char *str, WORD value )
{
    if (_IsDebugMsg == FALSE)
	return;
    printf(str, value);
    //printMsg("\n");

}

void HDR_PrintMsg( char *str)
{
    if (_IsDebugMsg == FALSE)
	return;
    printf(str);
    //printMsg("\n");
}

int CIExyToRGBW(float x, float y)
{
    //const float offset = 0.05;

    if ((_sRGBGamut.G.sx >= x) && (_sRGBGamut.G.sy <= y)){
        return _G;
    }else if ((_sRGBGamut.R.sx <= x) && (_sRGBGamut.R.sy >= y)){
        return _R;
    }else if (((_sRGBGamut.B.sx + 0.05) >= x) && ((_sRGBGamut.B.sy + 0.05) >= y)){
        return _B;
    }else{
        return _W;
    }
}

BOOL IsGamutValid(GamutDesc gmt)
{
    if ((gmt.R.sx < 0.00001) ||
        (gmt.R.sy < 0.00001) ||
        (gmt.G.sx < 0.00001) ||
        (gmt.G.sy < 0.00001) ||
        (gmt.B.sx < 0.00001) ||
        (gmt.B.sy < 0.00001) ||
        (gmt.W.sx < 0.00001) ||
        (gmt.W.sy < 0.00001)){
        return FALSE;
    }
    return TRUE;
}

GamutDesc InitGamutDesc(void)
{
     xdata GamutDesc gmt = {FALSE,
                                           {0,0}, //R
                                           {0,0}, //G
                                           {0,0}, //B
                                           {0,0} //W
                                           };//Valid;
     return gmt;
}



//Only gamut is large then sRGB gamut is available.
GamutDesc ToGamutDesc(BYTE u8WinIdx, SMPTE2086Metadata metadata, BOOL IsRefColorimetry)
{
    xdata GamutDesc gmtDesc;
    xdata float x = 0, y = 0;
    xdata const float units = 0.00002;
    xdata const WORD maxVal = 0xC350;
    gmtDesc = InitGamutDesc();
    if(IsRefColorimetry == TRUE)
    {
        //Greg : Color Format API
        ST_COMBO_COLOR_FORMAT cf;
        cf = GetHDRColorFormat();
        //HDR_PrintData("Pixel Format(RGB/422/444/420) = %d", cf.ucColorType);
        //HDR_PrintData("Color Range(Dft/Limit/Full) = %d", cf.ucColorRange);
        //HDR_PrintData("Yuv Colorimetry(601/709/Nodata/EC) = %d", cf.ucYuvColorimetry);
        //HDR_PrintData("Extend Colorimetry(601/709/s601/adobe601/adobeRGB/2020CL/2020NCL) = %d", cf.ucColorimetr
        //sRGB/709
        memcpy(&gmtDesc, &_sRGBGamut, sizeof(gmtDesc));
        //if (cf.ucYuvColorimetry == COMBO_YUV_COLORIMETRY_EC)
        {
            if (cf.ucColorType == COMBO_COLOR_FORMAT_RGB)
            {
                if (cf.ucColorimetry ==COMBO_COLORIMETRY_ADOBERBG)
                {
                    memcpy(&gmtDesc, &_AdobeRGBGamut, sizeof(gmtDesc));
                }else if(cf.ucColorimetry == COMBO_COLORIMETRY_BT2020RGBYCbCr)
                {
                    memcpy(&gmtDesc, &_BT2020Gamut, sizeof(gmtDesc));
                }
            }else{
                if (cf.ucColorimetry == COMBO_COLORIMETRY_BT2020RGBYCbCr)
                {
                    memcpy(&gmtDesc, &_BT2020Gamut, sizeof(gmtDesc));
                }
            }
        }

    }else{
        UNUSED(u8WinIdx);
        //range check;
        if (metadata.Rx > maxVal)
            metadata.Rx = maxVal;

        if (metadata.Ry > maxVal)
            metadata.Ry = maxVal;

        if (metadata.Gx > maxVal)
            metadata.Gx = maxVal;

        if (metadata.Gy > maxVal)
            metadata.Gy = maxVal;

        if (metadata.Bx > maxVal)
            metadata.Bx = maxVal;

        if (metadata.By > maxVal)
            metadata.By = maxVal;

        if (metadata.Wx > maxVal)
            metadata.Wx = maxVal;

        if (metadata.Wy > maxVal)
            metadata.Wy = maxVal;

        x = units * metadata.Rx;
        y = units * metadata.Ry;
        switch(CIExyToRGBW(x, y))
        {
            case _R:{gmtDesc.R.sx = x; gmtDesc.R.sy = y;}break;
            case _G:{gmtDesc.G.sx = x; gmtDesc.G.sy = y;}break;
            case _B:{gmtDesc.B.sx = x; gmtDesc.B.sy = y;}break;
            case _W:{gmtDesc.W.sx = x; gmtDesc.W.sy = y;}break;
        }

        x = units * metadata.Gx;
        y = units * metadata.Gy;
        switch(CIExyToRGBW(x, y))
        {
            case _R:{gmtDesc.R.sx = x; gmtDesc.R.sy = y;}break;
            case _G:{gmtDesc.G.sx = x; gmtDesc.G.sy = y;}break;
            case _B:{gmtDesc.B.sx = x; gmtDesc.B.sy = y;}break;
            case _W:{gmtDesc.W.sx = x; gmtDesc.W.sy = y;}break;
        }

        x = units * metadata.Bx;
        y = units * metadata.By;
        switch(CIExyToRGBW(x, y))
        {
            case _R:{gmtDesc.R.sx = x; gmtDesc.R.sy = y;}break;
            case _G:{gmtDesc.G.sx = x; gmtDesc.G.sy = y;}break;
            case _B:{gmtDesc.B.sx = x; gmtDesc.B.sy = y;}break;
            case _W:{gmtDesc.W.sx = x; gmtDesc.W.sy = y;}break;
        }

        x = units * metadata.Wx;
        y = units * metadata.Wy;
        switch(CIExyToRGBW(x, y))
        {
            case _R:{gmtDesc.R.sx = x; gmtDesc.R.sy = y;}break;
            case _G:{gmtDesc.G.sx = x; gmtDesc.G.sy = y;}break;
            case _B:{gmtDesc.B.sx = x; gmtDesc.B.sy = y;}break;
            case _W:{gmtDesc.W.sx = x; gmtDesc.W.sy = y;}break;
        }
    }
    gmtDesc.IsValid = IsGamutValid(gmtDesc);

    HDR_PrintMsg("HDR : Mastering Display gamut (to int)");
    //HDR_PrintData("Rx = %d", gmtDesc.R.sx * 10000);
    //HDR_PrintData("Ry = %d", gmtDesc.R.sy * 10000);
    //HDR_PrintData("Gx = %d", gmtDesc.G.sx * 10000);
    //HDR_PrintData("Gy = %d", gmtDesc.G.sy * 10000);
    //HDR_PrintData("Bx = %d", gmtDesc.B.sx * 10000);
    //HDR_PrintData("By = %d", gmtDesc.B.sy * 10000);
    //HDR_PrintData("Wx = %d", gmtDesc.W.sx * 10000);
    //HDR_PrintData("Wy = %d", gmtDesc.W.sy * 10000);
    HDR_PrintData("IsValid = %d", gmtDesc.IsValid);
    return gmtDesc;
}

ToneMappingDesc ToToneMappingDesc(SMPTE2086Metadata metadata)
{
    xdata const float units = 0.0001;
    xdata const float maxVal = 6.5535;
    xdata ToneMappingDesc tmDesc;

    tmDesc.srcMaxLum = metadata.MaxDispLum;
    tmDesc.srcMinLum = metadata.MinDispLum * units * _minLimRatio;;
    tmDesc.MaxCLL = metadata.MaxCLL;
    tmDesc.MaxFALL = metadata.MaxFALL;
    tmDesc.trgMaxLum = _panelMaxLum;
    tmDesc.trgMinLum = _panelMinLum * _minLimRatio;

    //range check;
    if (tmDesc.srcMinLum > maxVal)
    {
        tmDesc.srcMinLum = maxVal;
    }

    if (tmDesc.srcMaxLum > _maxLumOfST2084)
        tmDesc.srcMaxLum = _maxLumOfST2084;

    if (tmDesc.trgMaxLum > _maxLumOfST2084)
        tmDesc.trgMaxLum = _maxLumOfST2084;

    if (tmDesc.MaxCLL > _maxLumOfST2084)
        tmDesc.MaxCLL = _maxLumOfST2084;

    if (tmDesc.MaxFALL > _maxLumOfST2084)
        tmDesc.MaxFALL = _maxLumOfST2084;

    if (tmDesc.srcMaxLum < tmDesc.trgMaxLum)
        tmDesc.srcMaxLum  = tmDesc.trgMaxLum;

    if (tmDesc.MaxCLL < tmDesc.srcMaxLum)
        tmDesc.MaxCLL  = tmDesc.srcMaxLum;

    //The MaxCLL and MaxFALL in most of video content is zero.
    //For avoiding calcuation error in tone mapping algorithem, so applied default values.
    tmDesc.MaxFALL = 0;


    //#if (CHIP_ID == MST9U4)
         //tmDesc.IsValid = true;
    //#else

    //for avoiding some content send incorrent min luminance.
    if (tmDesc.srcMinLum >= 0.5)
    {
        tmDesc.srcMinLum = 0.5;
    }

    //Here is check max luminance is reasonable or not.
    if ( (_panelMaxLum >= tmDesc.MaxCLL) && (_panelMaxLum >= tmDesc.srcMaxLum))
    {
        tmDesc.IsValid = false;
    }else{
        tmDesc.IsValid = true;
    }
    //#endif

    HDR_PrintMsg("HDR :  Luminance Parms (nits)");
    HDR_PrintData("srcMaxLum = %d", tmDesc.srcMaxLum);
    HDR_PrintData("srcMinLum(*10000) = %d", tmDesc.srcMinLum* 10000);
    HDR_PrintData("MaxCLL = %d", tmDesc.MaxCLL);
    HDR_PrintData("MaxFALL = %d", tmDesc.MaxFALL);
    HDR_PrintData("trgMaxLum = %d", tmDesc.trgMaxLum);
    HDR_PrintData("trgMinLum(*10000) = %d", tmDesc.trgMinLum* 10000);

    return tmDesc;
}

void SetHDRPanelMaxLum(WORD maxLum)
{
    //HDR_PrintData("Set Max Lum = %d", maxLum);
    if (maxLum >= 100)
        _panelMaxLum = maxLum;

    _factor = _panelMaxLum/ _maxLumOfST2084;
}

void SetHDRPanelMinLum(WORD minLum)
{
    _panelMinLum = minLum* 0.0001;
}

Matrix3x3  ToMatrix(GamutDesc gmd)
{
    xdata float Rx = gmd.R.sx;
    xdata float Ry = gmd.R.sy;
    xdata float Gx = gmd.G.sx;
    xdata float Gy = gmd.G.sy;
    xdata float Bx = gmd.B.sx;
    xdata float By = gmd.B.sy;
    xdata float Wx = gmd.W.sx;
    xdata float Wy = gmd.W.sy;
    xdata Matrix3x3 m;

    xdata float a = 0;
    xdata float b = 0;
    xdata float c = 0;
    xdata float d = 0;
    xdata float e = 0;
    xdata float f = 0;
    xdata float g = 0;
    xdata float h = 0;
    xdata float i = 0;

    a = Wy*(-By+Gx*By-Bx*Gy+Gy)/(-Gx*Wy+Bx*Wy+Gx*By-By*Wx+Gy*Wx-Bx*Gy);
    b = Wy*(Gx*By-Gx+Bx-Bx*Gy)/(-Gx*Wy+Bx*Wy+Gx*By-By*Wx+Gy*Wx-Bx*Gy);
    c = (Gx*By-Bx*Gy)*Wy/(- Gx*Wy+Bx*Wy+Gx*By-By*Wx+Gy*Wx-Bx*Gy);
    d = (Ry-Ry*Bx-By+Rx*By)*Wy/(Ry*Wx-Ry*Bx-Rx*Wy+Rx*By-By*Wx+Bx*Wy);
    e = (Rx*By-Rx+Bx-Ry*Bx)*Wy/(Ry*Wx-Ry*Bx-Rx*Wy+Rx*By-By*Wx+Bx*Wy);
    f = (Rx*By-Ry*Bx)*Wy/(Ry*Wx-Ry*Bx-Rx*Wy+Rx*By-By*Wx+Bx*Wy);
    g = Wy*(Rx*Gy-Gx*Ry+Ry-Gy)/(-Gx*Ry+Ry*Wx-Gy*Wx-Rx*Wy+Rx*Gy+Gx*Wy);
    h = Wy*(Rx*Gy-Rx+Gx-Gx*Ry)/(-Gx*Ry+Ry*Wx-Gy*Wx-Rx*Wy+Rx*Gy+Gx*Wy);
    i = ((Rx*Gy-Gx*Ry)*Wy)/(-Gx*Ry+Ry*Wx-Gy*Wx-Rx*Wy+Rx*Gy+Gx*Wy);
    m.matrix[0][0] = a;
    m.matrix[0][1] = b;
    m.matrix[0][2] = c;
    m.matrix[1][0] = d;
    m.matrix[1][1] = e;
    m.matrix[1][2] = f;
    m.matrix[2][0] = g;
    m.matrix[2][1] = h;
    m.matrix[2][2] = i;

    m.IsValid = TRUE;

    return m;
}

Matrix3x3 InverseMatrix(Matrix3x3 m)
{
    xdata Matrix3x3 inv;
    xdata float tmp1, tmp2, tmp3, det;

    tmp1=m.matrix[0][0]*(m.matrix[1][1]*m.matrix[2][2]-m.matrix[1][2]*m.matrix[2][1]);
    tmp2=m.matrix[0][1]*(m.matrix[1][0]*m.matrix[2][2]-m.matrix[1][2]*m.matrix[2][0]);
    tmp3=m.matrix[0][2]*(m.matrix[1][0]*m.matrix[2][1]-m.matrix[1][1]*m.matrix[2][0]);

    det=(tmp1-tmp2+tmp3);
    inv.matrix[0][0]= (m.matrix[1][1]*m.matrix[2][2]-m.matrix[1][2]*m.matrix[2][1])/det;
    inv.matrix[0][1]=-(m.matrix[0][1]*m.matrix[2][2]-m.matrix[0][2]*m.matrix[2][1])/det;
    inv.matrix[0][2]= (m.matrix[0][1]*m.matrix[1][2]-m.matrix[0][2]*m.matrix[1][1])/det;
    inv.matrix[1][0]=-(m.matrix[1][0]*m.matrix[2][2]-m.matrix[1][2]*m.matrix[2][0])/det;
    inv.matrix[1][1]= (m.matrix[0][0]*m.matrix[2][2]-m.matrix[0][2]*m.matrix[2][0])/det;
    inv.matrix[1][2]=-(m.matrix[0][0]*m.matrix[1][2]-m.matrix[0][2]*m.matrix[1][0])/det;
    inv.matrix[2][0]= (m.matrix[1][0]*m.matrix[2][1]-m.matrix[1][1]*m.matrix[2][0])/det;
    inv.matrix[2][1]=-(m.matrix[0][0]*m.matrix[2][1]-m.matrix[0][1]*m.matrix[2][0])/det;
    inv.matrix[2][2]= (m.matrix[0][0]*m.matrix[1][1]-m.matrix[0][1]*m.matrix[1][0])/det;
    inv.IsValid = TRUE;
    return inv;
}

Matrix3x3 MatrixMultiply(Matrix3x3 m0, Matrix3x3 m1)
{
    xdata Matrix3x3 result;
    xdata int row = 0, col = 0;
    for (row = 0; row < 3; row++ )
    {
        for (col = 0; col < 3; col++ )
        {
            result.matrix[row][col] = ((m0.matrix[row][0] * m1.matrix[0][col]) +
                                                        (m0.matrix[row][1] * m1.matrix[1][col]) +
                                                        (m0.matrix[row][2] * m1.matrix[2][col]));
        }
    }
    result.IsValid = TRUE;
    return result;
}

//Ex. Big gamut(native, src) mapping to small gamut(sRGB, trg).
void GamutMapping(BYTE u8WinIdx, GamutDesc src, GamutDesc trg)
{
    xdata float sum[3] = {0};
    xdata float maxVal = 0;
    xdata int row = 0, col = 0;
    xdata Matrix3x3 srcMat;
    xdata Matrix3x3 trgMat;
    xdata Matrix3x3 sRGB2dRGB;
    srcMat = ToMatrix(src);
    trgMat = ToMatrix(trg);
    trgMat = InverseMatrix(trgMat);
    sRGB2dRGB = MatrixMultiply(srcMat, trgMat);

    for (row = 0; row < 3; row++)
    {
        sum[row] = 0;
        for (col = 0; col < 3; col++)
        {
            sum[row] += sRGB2dRGB.matrix[row][col];
        }
    }

    maxVal = MAX(sum[0], sum[1]);
    maxVal = MAX(maxVal, sum[2]);

    if (maxVal > 1)
    {
        for (row = 0; row < 3; row++)
        {
            for (col = 0; col < 3; col++)
            {
                sRGB2dRGB.matrix[row][col] = sRGB2dRGB.matrix[row][col] / maxVal;
            }
        }
    }
    API_LoadColorMatrix(u8WinIdx, sRGB2dRGB);
    UNUSED(u8WinIdx);
}


bool IsDiffMetadata(SMPTE2086Metadata m1, SMPTE2086Metadata m2)
{
    if ((m1.Checksum!= m2.Checksum)||
        (m1.Version  != m2.Version)||
        (m1.Length != m2.Length)||
        (m1.EOTF != m2.EOTF)||
        (m1.MetadataDescID != m2.MetadataDescID)||
        (m1.Rx != m2.Rx)||
        (m1.Ry != m2.Ry)||
        (m1.Gx != m2.Gx)||
        (m1.Gy != m2.Gy)||
        (m1.Bx != m2.Bx)||
        (m1.By != m2.By)||
        (m1.Wx != m2.Wx)||
        (m1.Wy != m2.Wy)||
        (m1.MaxDispLum != m2.MaxDispLum)||
        (m1.MinDispLum != m2.MinDispLum)||
        (m1.MaxCLL!= m2.MaxCLL)||
        (m1.MaxFALL != m2.MaxFALL))
    {
        return true;
    }
    return false;
}

void ResetMetadata(SMPTE2086Metadata *pMd)
{
    memset(pMd, 0x00, sizeof(SMPTE2086Metadata));
}

void LoadDeGammaTable(BYTE u8WinIdx)
{
#if HDR_AUTODOWNLOAD
    API_LoadDeGammaTabel(u8WinIdx, GetHDRDeGammaTable());
#endif

}

void LoadPanelGammaTable(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
    /* no panel gamma in mt9701
    #if ENABLE_LUT_AUTODOWNLOAD
    //API_LoadPanelGammaTable(BYTE u8WinIdx, BYTE **pu8Table)
    LoadPanelGammaTbl_ADL(u8WinIdx, 0);//LoadHDRPostGammaTbl();
    #else
     LoadPanelGammaTbl(u8WinIdx, 0);//LoadHDRPostGammaTbl();
    #endif
    */
}

WORD GetMaxCLL(SMPTE2086Metadata *metadata)
{
    //If MaxCLL is present, use MaxCLL to set tone mapping limits.
    WORD maxCLL = metadata->MaxCLL;
    //If MaxCLL is not present, use MasteringDisplayLuminance to set tone mapping limits.
    //Exception to Rule 1: If MaxCLL is greater than MasteringDisplayLuminance, use MasteringDisplayLuminance to set tone mapping limits.
    if ((maxCLL == 0) || (maxCLL > metadata->MaxDispLum))
    {
        maxCLL = metadata->MaxDispLum;
    }

    if (maxCLL == 0)
    {
        maxCLL = _panelMaxLum;
    }

    if (maxCLL < _panelMaxLum)
    {
        maxCLL = _panelMaxLum;
    }
    return maxCLL;
}


void LoadGammaTable(BYTE u8WinIdx,SMPTE2086Metadata *metadata,ToneMappingDesc* tmd, double panelMaxLum)
{
    WORD MaxCLL = GetMaxCLL(metadata);
    API_LoadGammaTable(u8WinIdx, GetHDRGammaTable(panelMaxLum,MaxCLL,tmd->IsValid));
}

#if 0
bool CheckPktCheckSum(BYTE *pkt, int pktLen, BYTE pktCheckSum )
{
    UNUSED(pkt);
    UNUSED(pktLen);
    UNUSED(pktCheckSum);
    return true;
}
#endif

SMPTE2086Metadata msGetST2086MetadataByPort(BYTE u8WinIdx, BYTE portNum)
{
    xdata SMPTE2086Metadata metadata;
    xdata BYTE header[4];
    xdata BYTE packet[32];
    xdata BOOL IsPktReady = FALSE;
    bool bIsPktCheckSumError = FALSE;
    memset(packet, 0, sizeof(SMPTE2086Metadata));
    ResetMetadata(&metadata);
    UNUSED(u8WinIdx);
    UNUSED(portNum);
    IsPktReady = msAPI_combo_IPGetHDRPacket(SrcInputType, header, packet);
#if (COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
    bIsPktCheckSumError = (bool)(msAPI_combo_HDMI_GetErrorStatus_Partial(SrcInputType) & _BIT2);
#endif
    
    if ((INPUT_IS_DISPLAYPORT(SrcInputType) && IsPktReady)||(INPUT_IS_USBTYPEC(SrcInputType) && IsPktReady))
    {
       //HDR_PrintMsg("DP HDR Packet");
        metadata.Version = packet[0];
        metadata.Length = packet[1];
        //metadata.Checksum = packet[2];
        metadata.EOTF = packet[2];
        metadata.MetadataDescID = packet[3];
        metadata.Rx = (((WORD)packet[13]) << 8) + packet[12];
        metadata.Ry = (((WORD)packet[15]) << 8 ) + packet[14];
        metadata.Gx = (((WORD)packet[5])<< 8 ) + packet[4];
        metadata.Gy = (((WORD)packet[7]) << 8 ) + packet[6];
        metadata.Bx = (((WORD)packet[9]) << 8 ) + packet[8];
        metadata.By = (((WORD)packet[11]) << 8 ) + packet[10];
        metadata.Wx = (((WORD)packet[17]) << 8 ) + packet[16];
        metadata.Wy =(((WORD)packet[19]) << 8 ) + packet[18];
        metadata.MaxDispLum = (((WORD)packet[21]) << 8 ) + packet[20];
        metadata.MinDispLum = (((WORD)packet[23]) << 8 ) + packet[22];
        metadata.MaxCLL = (((WORD)packet[25]) << 8 ) + packet[24];
        metadata.MaxFALL = (((WORD)packet[27]) << 8 ) + packet[26];
    }
    else if(INPUT_IS_HDMI(SrcInputType) && IsPktReady && !bIsPktCheckSumError){
        //HDMI
        metadata.Version = packet[0];
        metadata.Length = packet[1];
        metadata.Checksum = packet[2];
        metadata.EOTF = packet[3];
        metadata.MetadataDescID = packet[4];
        metadata.Rx = (((WORD)packet[6]) << 8) + packet[5];
        metadata.Ry = (((WORD)packet[8]) << 8 ) + packet[7];
        metadata.Gx = (((WORD)packet[10])<< 8 ) + packet[9];
        metadata.Gy = (((WORD)packet[12]) << 8 ) + packet[11];
        metadata.Bx = (((WORD)packet[14]) << 8 ) + packet[13];
        metadata.By = (((WORD)packet[16]) << 8 ) + packet[15];
        metadata.Wx = (((WORD)packet[18]) << 8 ) + packet[17];
        metadata.Wy =(((WORD)packet[20]) << 8 ) + packet[19];
        metadata.MaxDispLum = (((WORD)packet[22]) << 8 ) + packet[21];
        metadata.MinDispLum = (((WORD)packet[24]) << 8 ) + packet[23];
        metadata.MaxCLL = (((WORD)packet[26]) << 8 ) + packet[25];
        metadata.MaxFALL = (((WORD)packet[28]) << 8 ) + packet[27];
    }
    return metadata;
}

void msSetGamutMappingType(BOOL IsRefColorimetry)
{
    _IsRefColorimetry = IsRefColorimetry;
}


void msSetHDRUserPref(HDRUserPrefSettings *pref)
{
    memcpy(&_pref, pref, sizeof(_pref));
    _pPref = pref;
    _IsUserPrefChanged = TRUE;
}

void msClearHDRUserPref(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
    //reset sharpness
    if (_pPref != NULL)
    {
        if(_pref.skinToned.IsValid)
        {
            //Greg : Color API
            //msAPI_AdjustSkinTone(u8WinIdx, FALSE,  0);
        }

        #if (CHIP_ID == MST9U4)
        {
            if (_pref.IHCd.IsValid)
            {
                //Greg : Color API
                //msAPI_VIPHueEnable(u8WinIdx, FALSE);
            }

            if (_pref.IBCd.IsValid)
            {
                //Greg : Color API
                //msDrvCustomBrightnessEnable(u8WinIdx, FALSE);
            }

            if (_pref.ICCd.IsValid)
            {
                //Greg : Color API
                //msDrvCustomSaturationEnable(u8WinIdx, FALSE);
            }
        }
        #else
        {
            if (_pref.IHCd.IsValid)
            {
                //Greg : Color API
                //msAPI_CustomHueEnable(u8WinIdx, FALSE);
            }

            if (_pref.IBCd.IsValid)
            {
                //Greg : Color API
                //msAPI_CustomBriEnableRGBMode(u8WinIdx, FALSE);
            }

            if (_pref.ICCd.IsValid)
            {
                //Greg : Color API
                //msAPI_CustomSatEnableRGBMode(u8WinIdx, FALSE);
            }
        }
        #endif
    }
    memset(&_pref, 0x00, sizeof(_pref));
}

void msExeHDRUserPref(BYTE u8WinIdx)
{
    //TBD, for six axis user prefer.
    //============================

    if (_pPref == NULL)
    {
        msClearHDRUserPref(u8WinIdx);
        return;
    }

    if(_pref.pnlgmd.IsValid)
    {
        HDR_PrintMsg("Panel gamut settings from User Pref is referenced.");
        _panelGamut = _pref.pnlgmd;
    }

    //Reserved feature
    if(_pref.srcgmd.IsValid)
    {
    }

    if(_pref.skinToned.IsValid)
    {
        //Greg : Color API
        //msAPI_AdjustSkinTone(u8WinIdx, TRUE,  _pref.skinToned.adjValue);
    }

}

void DisableSramGmcCenCtrl(void)
{
    msWriteByteMask( SC79_61, BIT5, BIT5);
}

void SetupHDRMode(BYTE u8WinIdx, HDR_Level lvl, SMPTE2086Metadata metadata)
{
    if ((HDR_OFF == lvl) ||(HDR_FREESYNC_PRO == lvl))
    {
        //mStar_WaitForDataBlanking();
        API_CloseHDRMode(u8WinIdx);
        msClearHDRUserPref(u8WinIdx);
        _lastHDRLvl[u8WinIdx] = lvl;
        _lastMetadata[u8WinIdx] = metadata;
        _IsUseExtPortIPMux[u8WinIdx] = FALSE;
        _IsUseExtPortConfig[u8WinIdx] = FALSE;
        _IsUserPrefChanged = FALSE;
        return;
    }else{
        //disabled sram power saving function for avoiding write table fail in AC OFF/ON condtiion.
        DisableSramGmcCenCtrl();
    }

    // For Update TMO Table
    if (IsDiffMetadata(metadata,  _lastMetadata[u8WinIdx]) && (lvl == _lastHDRLvl[u8WinIdx]) && (_IsPanelInfoChanged == FALSE))
    {
        _lastMetadata[u8WinIdx] = metadata;
        _lastHDRLvl[u8WinIdx] = lvl;

        _tmDesc = ToToneMappingDesc(metadata);
        LoadGammaTable(u8WinIdx,&metadata, &_tmDesc,_panelMaxLum);


        HDR_PrintData(">>> Update TMO Table(w/o TMO = 0) = %d\n", _tmDesc.IsValid);

        //mStar_WaitForDataBlanking();
#if HDR_AUTODOWNLOAD
        FireAutoDownload(u8WinIdx);
#endif
    }


    if ((lvl != _lastHDRLvl[u8WinIdx]) || (_IsPanelInfoChanged == TRUE))
    {
        //---------------------------------
        _lastMetadata[u8WinIdx] = metadata;
        _lastHDRLvl[u8WinIdx] = lvl;
        _IsPanelInfoChanged = FALSE;
        _IsUserPrefChanged = FALSE;
        msExeHDRUserPref(u8WinIdx);

        API_InitHDR(u8WinIdx);
        //XVYCC Degamma/Fixgamma/Color Matrix
        if(_pref.srcgmd.IsValid)
        {
            HDR_PrintMsg("Source gamut settings from User Pref is referenced.");
            _masteringDispGamut = _pref.srcgmd;
        }else{
            HDR_PrintMsg("Source gamut settings from AVI colorimetry is referenced.");
            _masteringDispGamut = ToGamutDesc(u8WinIdx, metadata, _IsRefColorimetry);
        }

        if ((_pref.gmd.IsValid)&&(_pref.gmd.IsTraceNativeWhitePoint))
        {
            HDR_PrintMsg("The traget white point is changed as panel native white point..");
            _masteringDispGamut.W.sx = _panelGamut.W.sx;
            _masteringDispGamut.W.sy = _panelGamut.W.sy;
        }

        if ((_pref.gmd.IsValid)&&(_pref.gmd.IsDisableGamutMapping))
        {
            Matrix3x3 m;
            m.IsValid = TRUE;
            m.matrix[0][0] = 1;
            m.matrix[0][1] = 0;
            m.matrix[0][2] = 0;
            m.matrix[1][0] = 0;
            m.matrix[1][1] = 1;
            m.matrix[1][2] = 0;
            m.matrix[2][0] = 0;
            m.matrix[2][1] = 0;
            m.matrix[2][2] = 1;
            API_LoadColorMatrix(u8WinIdx, m);
            HDR_PrintMsg("Gamut mapping is bypassed.");
        }
        else
        {
            if (_masteringDispGamut.IsValid)
            {
                GamutMapping(u8WinIdx, _panelGamut , _masteringDispGamut);
            }
        }
        //xvycc
        LoadXVyccLUT(u8WinIdx);
        //EOTF
        SetHDRPanelMaxLum(_panelMaxLum);
        LoadDeGammaTable(u8WinIdx);
        //Gamma
        _tmDesc = ToToneMappingDesc(metadata);
         LoadGammaTable(u8WinIdx,&metadata, &_tmDesc,_panelMaxLum);
        API_LoadOOTFTable(u8WinIdx, GetHDROOTFTable(_panelMaxLum));

        HDR_PrintData(">>> Load Gamma Table(w/o TMO = 0) = %d\n", _tmDesc.IsValid);
        //tone mapping
        #if 0
        _tmDesc = ToToneMappingDesc(metadata);
        if(_tmDesc.IsValid)
        {
            UpdateToneMappingCurve(u8WinIdx, _tmDesc);
        }
        #endif

        //RGBLimitedRgnToFullRgn(MAIN_WINDOW);
        //gamut mapping: mastering display gamut mapping (big gamut) to traget display gamut(small gamut)
        msSetHDRCSCType_Bypass(MAIN_WINDOW);
        API_CSC(u8WinIdx, HDR_Y2R);

        ST_COMBO_COLOR_FORMAT cf = GetHDRColorFormat();
        if(cf.ucColorType != COMBO_COLOR_FORMAT_RGB)
        {
          API_CSC(u8WinIdx, HDR_R2Y);
        }

#if HDR_AUTODOWNLOAD
        FireAutoDownload(u8WinIdx);
#endif

        API_OpenHDRMode(u8WinIdx);
        //Load Post Gamma
        //msAPI_GammaLoadTbl_1024E_12B_N(MAIN_WINDOW );
    }
}

void msSetHDRPanelInfo(double maxLum, double minLum, double Rx, double Ry, double Gx, double Gy, double Bx, double By, double Wx, double Wy)
{
    xdata const WORD unit = 10000;
    SetHDRPanelMaxLum((WORD)maxLum);
    SetHDRPanelMinLum((WORD)(minLum*unit));
    _panelGamut.R.sx = Rx;
    _panelGamut.G.sx = Gx;
    _panelGamut.B.sx = Bx;
    _panelGamut.W.sx = Wx;
    _panelGamut.R.sy = Ry;
    _panelGamut.G.sy = Gy;
    _panelGamut.B.sy = By;
    _panelGamut.W.sy =Wy;
    _IsPanelInfoChanged = TRUE;
    #if ENABLE_HDR_CFD
    drvCfdSetPanelNative(Wx, Wy, Rx, Ry, Gx, Gy, Bx, By);
    #endif
    HDR_PrintMsg(">>>HDR panel Info<<< ");
    HDR_PrintData("Trg Max Lum = %d", maxLum);
    HDR_PrintData("Trg Min Lum = %d",  minLum * unit);
    //HDR_PrintData("Rx = %d", Rx * unit);
    //HDR_PrintData("Ry = %d", Ry * unit);
    //HDR_PrintData("Gx = %d", Gx * unit);
    //HDR_PrintData("Gy = %d", Gy * unit);
    //HDR_PrintData("Bx = %d", Bx * unit);
    //HDR_PrintData("By = %d", By * unit);
    //HDR_PrintData("Wx = %d", Wx * unit);
    //HDR_PrintData("Wy = %d", Wy * unit);

}

SMPTE2086Metadata GetDemoMetadata(void)
{
    //for demo DCI-P3 Life of Pi
    /*
     display_primaries_x:                                   13250 (0x33C2)
     display_primaries_y:                                   34500 (0x86C4)
     display_primaries_x:                                   7500 (0x1D4C)
     display_primaries_y:                                   3000 (0x0BB8)
     display_primaries_x:                                   34000 (0x84D0)
     display_primaries_y:                                   16000 (0x3E80)
     white_point_x:                                   15635 (0x3D13)
     white_point_y:                                   16450 (0x4042)
     max_display_mastering_luminance:                                   12000000 (0x00B71B00)
     min_display_mastering_luminance:                                   200 (0x000000C8)
    */

    xdata SMPTE2086Metadata metadata;
    metadata.MaxCLL = 10000;
    metadata.MaxFALL = 0;
    metadata.MaxDispLum = 1200;
    metadata.MinDispLum = 200;
    metadata.Rx = 0x84D0;
    metadata.Ry = 0x3E80;
    metadata.Gx = 0x33C2;
    metadata.Gy = 0x86C4;
    metadata.Bx = 0x1D4C;
    metadata.By = 0x0BB8;
    metadata.Wx = 0x3D13;
    metadata.Wy = 0x4042;
    metadata.EOTF = 2;
    metadata.MetadataDescID = 0;
    metadata.Version = 1;
    //metadata.TypeCode = 0x87;
    //below are unused.
    //metadata.Reserved = 0;
    metadata.Length = 0x1A;
    metadata.Checksum = 0;
    return metadata;
}

bool IsST2084EOTF(SMPTE2086Metadata* md)
{
    xdata bool result = false;
    HDR_PrintMsg(">>>HDR metadata<<< ");
    //HDR_PrintData("EOTF = %d", md->EOTF);
    HDR_PrintData("Src Max Lum = 0x%x", md->MaxDispLum);
    HDR_PrintData("Src Min Lum = 0x%x", md->MinDispLum);
    HDR_PrintData("MaxCLL = 0x%x", md->MaxCLL);
    HDR_PrintData("MaxFALL = 0x%x", md->MaxFALL);
    //HDR_PrintData("Rx = 0x%x", md->Rx);
    //HDR_PrintData("Ry = 0x%x", md->Ry);
    //HDR_PrintData("Gx = 0x%x", md->Gx);
    //HDR_PrintData("Gy = 0x%x", md->Gy);
    //HDR_PrintData("Bx = 0x%x", md->Bx);
    //HDR_PrintData("By = 0x%x", md->By);
    //HDR_PrintData("Wx = 0x%x", md->Wx);
    //HDR_PrintData("Wy = 0x%x", md->Wy);
    //check rule is type code is 0x07 (HDR type), eotf type is 0x02 (Smpte2084) and desc ID is 0x00.
    if ((md->EOTF & 0x02) == 0x02)
    {
         if ((md->MaxDispLum < 50) ||
              (md->MaxCLL < md->MaxFALL) )
         {
             HDR_PrintMsg(">>>HDR metadata check error,so metadata will applied demo settings<<< ");
             *md = GetDemoMetadata();
         }
        result  = true;
    }

    //patch, for avoiding some video content is eotf stream but eotf type is wrong.
    /*_tmDesc = ToToneMappingDesc(*md);
    _masteringDispGamut = ToGamutDesc(*md);
    if (_tmDesc.IsValid && _masteringDispGamut.IsValid)
    {
        result = true;
    }else{
        result = false;
    }
    */
    return result;
}

SMPTE2086Metadata msGetST2086MetadataByWin(BYTE u8WinIdx)
{
    //Greg : Combo API
    if (_lastHDRLvl[u8WinIdx] == (int)HDR_DEMO_HDR10)
    {
        return GetDemoMetadata();
    }
    else
    {
        return msGetST2086MetadataByPort(u8WinIdx, 0);
    }
}

bool GetHDRMetadata(BYTE u8WinIdx, SMPTE2086Metadata* metadata)
{
    *metadata = msGetST2086MetadataByWin(u8WinIdx);
    return IsST2084EOTF(metadata);
}


BOOL IsSupportOpenHDR(void)
{
    //Greg : TBD
    #if 0
    static BOOL IsSupport = FALSE;
    #if(CHIP_ID == MST9U3)
    static BOOL IsReadEfuse = FALSE;

    if(IsReadEfuse == FALSE)
    {
        BYTE efuseVal = msEread_GetReseredValue();
        IsReadEfuse = TRUE;
        if (efuseVal & BIT1)
        {
            IsSupport = TRUE;
        }

         if (efuseVal & BIT7)
        {
            IsSupport = FALSE;
        }
    }
    #endif

    #if (CHIP_ID == MST9U4)
    static BOOL IsReadEfuse = FALSE;
    if(IsReadEfuse == FALSE)
    {
        BYTE efuseVal = msReadByte(REG_101EFA); //value : 0x000F (support HDR)
        IsReadEfuse = TRUE;
        if (efuseVal & (BIT5|BIT6))
        {
            IsSupport = FALSE;
        }else{
            IsSupport = TRUE;
        }
    }
    #endif

    return IsSupport;
    #else
    return TRUE;
    #endif
}
#if 0
BOOL IsSupportDolbyHDR(void)
{
    #if 0
    static BOOL IsSupport = FALSE;
    #if (CHIP_ID == MST9U4)
    static BOOL IsReadEfuse = FALSE;
    if(IsReadEfuse == FALSE)
    {
        BYTE efuseVal = msReadByte(REG_101EFB);
        IsReadEfuse = TRUE;
	    efuseVal &= (BIT0|BIT1);
        if ((efuseVal == 0x00) || (efuseVal  == 0x03))
        {
            IsSupport = TRUE;
        }else{
            IsSupport = FALSE;
        }
    }
    #endif
    return IsSupport;
    #else
    return FALSE;
    #endif
}
#endif

void InitParams(void)
{
    xdata int winIdx = 0;
    if (_IsInitParams == FALSE)
    {
        _IsInitParams = TRUE;
        for(winIdx = 0; winIdx < _maxWinNum; winIdx++)
        {
            ResetMetadata(&_lastMetadata[winIdx]);
            _lastHDRLvl[winIdx] = HDR_OFF;
            _IsUseExtPortConfig[winIdx] = FALSE;
            _IsDP[winIdx] = FALSE;
            _IsUseExtPortIPMux[winIdx] = FALSE;;
            _extPortIPMux[winIdx] = 0;
        }
    }
}

HDR_Level msHDRHandler(BYTE u8WinIdx, HDR_Level lvl)
{
	//int nDS_Start = MsOS_GetSystemTime();
    InitParams();
    ResetMetadata(&_metadata);

    if (!IsSupportOpenHDR())
    {
         HDR_PrintMsg("HDR is not support.");
        return _lastHDRLvl[u8WinIdx];
    }

    if (!API_CheckWindowSize(u8WinIdx))
    {
        return _lastHDRLvl[u8WinIdx];
    }

    //mdrv_HDR_DS_Start();
    switch((int)lvl)
    {
        case HDR_FREESYNC_PRO:
        {
            SetupHDRMode(u8WinIdx, HDR_FREESYNC_PRO, _metadata);
        }break;
        case HDR_OFF:
        {
            SetupHDRMode(u8WinIdx, HDR_OFF, _metadata);
        }break;

        case HDR_DEMO_HDR10:
        {
            HDR_PrintMsg("HDR DEMO MODE");
            SetupHDRMode(u8WinIdx, HDR_DEMO_HDR10, GetDemoMetadata());
        }break;

        case HDR_HDR10:
        {
            //Get HDR metadata and then setup HDR mode.
            if (GetHDRMetadata(u8WinIdx, &_metadata))
            {
                SetupHDRMode(u8WinIdx, HDR_HDR10, _metadata);
            }
        }break;

    }

    //mdrv_HDR_DS_End();
    //int nDS_END = MsOS_GetSystemTime();
    //HDR_PrintData("msHDRHandler  take: = %d ms", nDS_END - nDS_Start);
    return _lastHDRLvl[u8WinIdx];
}

HDR_Level msGetHDRStatus(BYTE u8WinIdx)
{
    return _lastHDRLvl[u8WinIdx];
}

void msSetHDREOTFGain(BYTE u8WinIdx, float gain)
{
    API_SetHDREOTFGain(u8WinIdx, gain);
}

#if ENABLE_ACHDR_FUNCTION && !ENABLE_DeltaE
DWORD ByteReverse(DWORD i)
{
    return((i&0xff)<<24)+((i&0xff00)<<8)+((i&0xff0000)>>8)+((i>>24)&0xff);
}

short GetCheckSum(BYTE * pData, int size)
{
	XDATA short sum = 0;
	XDATA int idx = 0;

	for(idx = 0; idx < size; idx++){
		sum ^=	pData[idx] & 0xFF;;
		//printData("--%x", pData[idx]);
	}
	return sum;
}

BOOL IsStructEmpty(void * pData, int size)
{
    XDATA short idx = 0;
    XDATA BYTE* pStruct = pData;
    XDATA BOOL bIsAllData_FF = TRUE;
    XDATA BOOL bIsAllData_Zero = TRUE;

	for(idx = 0; idx < size; idx++)
    {
	    if (0x00 != pStruct[idx])
        {
            bIsAllData_Zero = FALSE;
        }
        if (0xFF != pStruct[idx])
        {
            bIsAllData_FF = FALSE;
        }

        if(!bIsAllData_Zero && !bIsAllData_FF)
        {
            return FALSE;
        }
	}

    if(bIsAllData_Zero || bIsAllData_FF)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL msLoadACHDRPanelInfo(StoredHDRPanelInfo *pHDRPanelInfo)
{
    XDATA DWORD BaseAddr = HDRPanelInfoAddr;//ACHDRGammaTblAddr + (ACHDRGammaTblSize*3);
    XDATA short checkSum = 0;

    #if USEFLASH
        Flash_ReadTbl(BaseAddr, (BYTE*)&(g_HDRPanelInfo), sizeof(g_HDRPanelInfo));
    #else
        NVRam_ReadTbl(BaseAddr, (BYTE*)&(g_HDRPanelInfo), sizeof(g_HDRPanelInfo));
    #endif
    checkSum = GetCheckSum((BYTE*)(&g_HDRPanelInfo.mHDRPanelInfo), sizeof(g_HDRPanelInfo.mHDRPanelInfo));

    //HDR_PrintData("msLoadACHDRPanelInfo addr = %x", BaseAddr);
    //HDR_PrintData("Stored CheckSum of AutoCalibrationHDR : %x", g_HDRPanelInfo.CheckSum);
    //HDR_PrintData("Calculated CheckSum of AutoCalibrationHDR : %x", checkSum);

    if ((DWORD)checkSum == g_HDRPanelInfo.CheckSum && !IsStructEmpty(&g_HDRPanelInfo.mHDRPanelInfo, sizeof(g_HDRPanelInfo.mHDRPanelInfo)))
    {
        pHDRPanelInfo->mHDRPanelInfo.minLum   = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.minLum);
        pHDRPanelInfo->mHDRPanelInfo.maxLum   = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.maxLum);
        pHDRPanelInfo->mHDRPanelInfo.Rx       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.Rx);
        pHDRPanelInfo->mHDRPanelInfo.Ry       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.Ry);
        pHDRPanelInfo->mHDRPanelInfo.Gx       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.Gx);
        pHDRPanelInfo->mHDRPanelInfo.Gy       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.Gy);
        pHDRPanelInfo->mHDRPanelInfo.Bx       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.Bx);
        pHDRPanelInfo->mHDRPanelInfo.By       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.By);
        pHDRPanelInfo->mHDRPanelInfo.Wx       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.Wx);
        pHDRPanelInfo->mHDRPanelInfo.Wy       = ByteReverse(g_HDRPanelInfo.mHDRPanelInfo.Wy);
        pHDRPanelInfo->CheckSum               = g_HDRPanelInfo.CheckSum;

        return TRUE;
    }
    return FALSE;
}

#if ENABLE_HDR_DYNAMIC_FUNCTION
BOOL msLoadACHDRLumMap(StoredHDRLumMap *pHDRLumMap)
{
    XDATA DWORD BaseAddr = HDRLumMapAddr;
    XDATA short checkSum = 0;
    XDATA BYTE idx = 0;

    #if USEFLASH
        Flash_ReadTbl(BaseAddr, (BYTE*)&(g_HDRLumMap), sizeof(g_HDRLumMap));
    #else
        NVRam_ReadTbl(BaseAddr, (BYTE*)&(g_HDRLumMap), sizeof(g_HDRLumMap));
    #endif
    checkSum = GetCheckSum((BYTE*)(&g_HDRLumMap.mHDRLumMap), sizeof(g_HDRLumMap.mHDRLumMap));

    if ((DWORD)checkSum == g_HDRLumMap.CheckSum && !IsStructEmpty(&g_HDRLumMap.mHDRLumMap, sizeof(g_HDRLumMap.mHDRLumMap)))
    {
        for(idx = 0 ; idx < sizeof(g_HDRLumMap.mHDRLumMap); idx++)
        {
            pHDRLumMap->mHDRLumMap[idx] = (g_HDRLumMap.mHDRLumMap[idx]>>8) | (g_HDRLumMap.mHDRLumMap[idx]<<8);
        }
        pHDRLumMap->CheckSum = g_HDRLumMap.CheckSum;

        return TRUE;
    }
    return FALSE;
}
#endif

/*
void msLoadACHDRGamma(StoredFormatOfPostGamma *pStoredPostGamma)
{
    XDATA DWORD BaseAddr = ACHDRGammaTblAddr;
    XDATA short checkSum = 0;
    XDATA BYTE ch = 0;
    XDATA WORD idx = 0;

    Flash_ReadTbl(BaseAddr, (BYTE*)(&_postGamma.Data), sizeof(_postGamma.Data));

    BaseAddr = ACHDRGammaCheckSumStartAddr;
    _postGamma.CheckSum = FlashReadByte(BaseAddr);
    checkSum = GetCheckSum((BYTE*)(&_postGamma.Data), sizeof(_postGamma.Data));

    //HDR_PrintData("msLoadACHDRGamma addr = %x", BaseAddr);
	//HDR_PrintData("Stored CheckSum of Post Gamma : %x", _postGamma.CheckSum);
	//HDR_PrintData("Calculated CheckSum of Post Gamma : %x", checkSum);

    if ((_postGamma.CheckSum != checkSum) || IsStructEmpty(_postGamma.Data, sizeof(_postGamma.Data)))
	{
		//ResetPostGammaData();
		HDR_PrintMsg("ResetPostGammaData");
 	}

    //msLoadPostGammaData(GetAddrOfHDRPostGamma());
    memcpy(pStoredPostGamma, &_postGamma, sizeof(_postGamma));
}
*/

#if 0
void msLoadACHDRColorTemp(StoredFormatOfHDRColorTemp *pStoredHDRColorTemp)
{
    XDATA DWORD BaseAddr = HDRColorTempAddr;
    XDATA short checkSum = 0;
	Flash_ReadTbl(BaseAddr, (BYTE*)(&g_HDRColorTemp), sizeof(g_HDRColorTemp));
	checkSum = GetCheckSum((BYTE*)(&g_HDRColorTemp.mHDRColorTemp), sizeof(g_HDRColorTemp.mHDRColorTemp));

    //HDR_PrintData("msLoadACHDRColorTemp addr = %x", BaseAddr);
	//HDR_PrintData("Stored CheckSum of HDR Color Temp : %x", g_HDRColorTemp.CheckSum);
	//HDR_PrintData("Calculated CheckSum of HDR Color Temp : %x", checkSum);

	if ((g_HDRColorTemp.CheckSum != checkSum) || IsStructEmpty(&g_HDRColorTemp.mHDRColorTemp, sizeof(g_HDRColorTemp.mHDRColorTemp)))
	{
	    g_HDRColorTemp.mHDRColorTemp.rgbCont[0] = 128;
        g_HDRColorTemp.mHDRColorTemp.rgbCont[1] = 128;
        g_HDRColorTemp.mHDRColorTemp.rgbCont[2] = 128;
        HDR_PrintMsg("Reset HDR Color Temp Data");
	}
    //HDR_PrintData("Stored HDR Color Temp R Gain : %x", g_HDRColorTemp.mHDRColorTemp.rgbCont[0]);
    //HDR_PrintData("Stored HDR Color Temp G Gain : %x", g_HDRColorTemp.mHDRColorTemp.rgbCont[1]);
    //HDR_PrintData("Stored HDR Color Temp B Gain : %x", g_HDRColorTemp.mHDRColorTemp.rgbCont[2]);

    memcpy(pStoredHDRColorTemp, &g_HDRColorTemp, sizeof(g_HDRColorTemp));
}

void msSetHDRPanelColorTemp(BYTE *pHDRColorTempGain)
{
    if (NULL != pHDRColorTempGain)
    {
        memcpy(_HDRColorTemp.RGBGain, pHDRColorTempGain, sizeof(_HDRColorTemp.RGBGain));
        _HDRColorTemp.IsValid = true;
    }
}
#endif

#endif

//#endif

