#ifndef _DRVHDR_H_
#define _DRVHDR_H_
#if ENABLE_DeltaE
#include "drvDeltaE.h"
#endif
//#if ENABLE_HDR
#if (DRAM_TYPE == DRAM_NOUSE) || !ENABLE_AUTOLOAD
#define HDR_AUTODOWNLOAD 1
//#define XVYCC_AUTODOWNLOAD 1
#else
#define HDR_AUTODOWNLOAD 1
//#define XVYCC_AUTODOWNLOAD 1
#endif

#if CHIP_ID == CHIP_MT9700
typedef enum
{
    HDR_OFF = 0,
    HDR_LOW = 1, //lengency setting, same as HDR10 ON
    HDR_MID = 2, //lengency setting, same as HDR10 ON
    HDR_HIGH = 3, //lengency setting, same as HDR10 ON
    HDR_RESUME = 4,//resume last low/mid/high hdr mode.
    HDR_DEMO = 5, //set demo metadata to HDR driver.
    HDR_DEMO_HDR10 = 5,
    HDR_UPDATETMO = 6,
} HDR_Level;
#else
typedef enum
{
    HDR_OFF = 0,
    HDR_HDR10 = 1,
    HDR_DEMO_HDR10 = 5,
    HDR_FREESYNC_PRO =12, //AMD HDR FreeSync Premium Pro
} HDR_Level;

#endif
typedef struct
{
    //BYTE TypeCode; //HD0, header byte 0
    BYTE Version;    //HD1, header byte 1
    BYTE Length;     //HD2, header byte 2
    BYTE Checksum;
    BYTE EOTF;
    BYTE MetadataDescID;
    WORD Rx;
    WORD Ry;
    WORD Gx;
    WORD Gy;
    WORD Bx;
    WORD By;
    WORD Wx;
    WORD Wy;
    //max display mastering luminance
    WORD MaxDispLum;
    //min display mastering luminance
    WORD MinDispLum;
    //max content light level
    WORD MaxCLL;
    //max frame-average light level
    WORD MaxFALL;
}SMPTE2086Metadata;

//Gamut Descriptor
typedef struct
{
    float sx;
    float sy;
} CIExy;

typedef struct
{
    BYTE IsValid;
    CIExy R;
    CIExy G;
    CIExy B;
    CIExy W;
} GamutDesc;

typedef struct
{
    BYTE IsValid;
    BYTE lumaCurve[16];
} LumaCurveDesc;

/*
typedef struct
{
    BYTE IsValid;
    WORD smin;
    WORD smed;
    WORD smax;
    WORD tmin;
    WORD tmed;
    WORD tmax;
} ToneMapping3PDesc;

typedef struct
{
    BYTE IsValid;
    BYTE IsAuto;
    WORD srcMax;
    WORD trgMax;
    WORD srcNits[12];
    WORD trgNits[12];
} ToneMapping12PDesc;
*/

typedef struct
{
    BYTE IsValid;
    WORD  saturation;
    DWORD highTh;
} UVCDesc;

typedef struct
{
    BYTE IsValid;
    BYTE  IsEnClampEOTF;
    BYTE clampEOTFType;
    WORD clampNits;
} EOTFDesc;

typedef struct
{
    BYTE IsValid;
    BYTE IsTraceNativeWhitePoint;
    BYTE IsDisableGamutMapping;
} GamutMappingDesc;

typedef struct
{
    BYTE IsValid;
    BYTE SharpOrBlur;
    BYTE Level;
} SharpnessDesc;

typedef struct
{
   BYTE IsValid;
   BYTE adjValue; //Bit7 is sign bit and value range is -63~63
} SkinToneDesc;

typedef struct
{
   BYTE IsValid;
   BYTE IHC[6];
} IHCDesc;

typedef struct
{
   BYTE IsValid;
   BYTE IBC[6];
} IBCDesc;

typedef struct
{
   BYTE IsValid;
   BYTE ICC[6];
} ICCDesc;
#if (CHIP_ID==CHIP_MT9700)
typedef struct
{
    BYTE IsValid;
    BYTE mPostGamma[3][64];
} __attribute__((packed))PostGammaDesc;
#else
typedef struct
{
    BYTE IsValid;
    BYTE mPostGamma[3][320];
} __attribute__((packed))PostGammaDesc;
#endif
typedef struct
{
   BYTE IsValid;
   BYTE RGBGain[3];
}HDRColorTempDesc;


typedef struct
{
//------------------------
     //ToneMapping3PDesc tmd;
     //ToneMapping12PDesc tmd12P;
     UVCDesc uvcd;
//------Common Desc------------
     LumaCurveDesc lcd;
     GamutDesc pnlgmd;
     GamutDesc srcgmd;
     EOTFDesc eotfd;
     GamutMappingDesc gmd;
     SharpnessDesc sharpd;
     SkinToneDesc skinToned;
     IHCDesc IHCd;
     IBCDesc IBCd;
     ICCDesc ICCd;
     HDRColorTempDesc hdrColorTemp;
}HDRUserPrefSettings;

#if !ENABLE_DeltaE
typedef struct{
    DWORD minLum;
    DWORD maxLum;
    DWORD Rx;
    DWORD Ry;
    DWORD Gx;
    DWORD Gy;
    DWORD Bx;
    DWORD By;
    DWORD Wx;
    DWORD Wy;
} HDRPanelInfo;

typedef struct{
    HDRPanelInfo mHDRPanelInfo;
	BYTE CheckSum;
}StoredHDRPanelInfo;

#ifndef GammaTableSize
#define GammaTableSize    64
#endif

typedef struct{
    BYTE rgbCont[3];
} HDRColorTemp;

typedef struct{
    HDRColorTemp mHDRColorTemp;
	BYTE CheckSum;
}StoredFormatOfHDRColorTemp;

typedef struct{
    WORD mHDRLumMap[11];
	BYTE CheckSum;
}StoredHDRLumMap;
#endif
//If set IsRefColorimetry = FALSE, then source gamut from HDR infoframe will be referenced.
extern void msSetGamutMappingType(BOOL IsRefColorimetry);

//Setup panel gamut and luminace settings.
extern void msSetHDRPanelInfo(double maxLum, double minLum, double Rx, double Ry, double Gx, double Gy, double Bx, double By, double Wx, double Wy);
extern SMPTE2086Metadata GetDemoMetadata(void);
//Setup panel gamma
extern void msSetHDRPanelGamma(BYTE *pGammadata);

//if setup hdr function successed then return applied HDR_Level.
//if setup fail then return HDR_OFF.
extern HDR_Level msHDRHandler(BYTE u8WinIdx, HDR_Level lvl);

//Get HDMI Infoframe HDR metadata By PortNum
extern SMPTE2086Metadata msGetST2086MetadataByWin(BYTE u8WinIdx);

//Get current HDR status.
extern HDR_Level msGetHDRStatus(BYTE u8WinIdx);

//Set HDR user pref settings
extern void msSetHDRUserPref(HDRUserPrefSettings *pref);
//------------------------------------------

extern void msSetHDRDebugMsg(BOOL IsEnDebugMsg);
void HDR_PrintData( char *str, WORD value );
void HDR_PrintMsg( char *str);

//extern BOOL IsSupportDolbyHDR(void);
extern BOOL IsSupportOpenHDR(void);
//extern void msSetHDRPortIPMux(BYTE u8WinIdx, BYTE portIPMux);
extern void msSetHDRWindowSize(BYTE u8WinIdx, WORD width, WORD height);
BYTE msGetHDRPortIPMuxByWin(BYTE u8WinIdx);
extern BOOL IsDiffMetadata(SMPTE2086Metadata m1, SMPTE2086Metadata m2);
extern void msSetHDRSrcIsDisplayPort(BYTE u8WinIdx, BOOL IsDP);
#if ENABLE_ACHDR_FUNCTION
extern BOOL msLoadACHDRPanelInfo(StoredHDRPanelInfo *pHDRPanelInfo);
extern BOOL msLoadACHDRLumMap(StoredHDRLumMap *pHDRLumMap);
extern void msSetHDRPostGamma(BYTE *pGammadata);
extern void msSetHDRPanelColorTemp(BYTE *pHDRColorTempGain);
#endif
extern void LoadHDRPanelColorTempRGBGain(void);
extern void LoadHDRPostGammaTable(BYTE u8WinIdx);

extern void msSetHDREOTFGain(BYTE u8WinIdx, float gain);
#endif
//#endif

