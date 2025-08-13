#ifndef _MS_DELTAE_H_
#define _MS_DELTAE_H_

#include "Adjust.h"

#define offset(A, b) ((BYTE)(&(((A *)0)->b)))



#define ENABLE_ColorMode_Preload_Function 0
#if ENABLE_ColorMode_Preload_Function
#define ENABLE_AllColorMode_Preload 0
#endif

#define ENABLE_ReadColorFUNCTION_FROM_FW 1
#if ENABLE_ReadColorFUNCTION_FROM_FW
#define sRGB_Enable                  (1&& ENABLE_DeltaE)
#define AdobeRGB_Enable         (0&& ENABLE_DeltaE)
#define BT709_Enable                (0&& ENABLE_DeltaE)
#define BT2020_Enable              (0&& ENABLE_DeltaE)
#define DCIP3_Enable                (0&& ENABLE_DeltaE)
#define EBU_Enable                    (0&& ENABLE_DeltaE)
#define SMPTEC_Enable              (0&& ENABLE_DeltaE)
#endif

//==========MSTAR COMMAND====================================
#define MS_Set_PostGammaQK          0x49
#define MS_Set_PostGamma      	    0x32
#define MS_WR_PostGamma      	    0x33
#define MS_SetPostGammaFromMem      0x39
#define MS_ColorEngine_OnOff   	    0x40
#define MS_Set_RGBGain              0x41
#define MS_LoadColorMode            0x42
#define MS_LoadPostGamma            0x44
#define MS_PanelNative_Data         0x46
#define MS_PanelNative_Data_Save    0x47
#define MS_ApplyColorMode           0x48
#define MS_Set_3DLUTQK              0X62
#define MS_Set_3DLUT          	    0x50
#define MS_WR_3DLUT           	    0x51
#define MS_IsUse_3DLUT           	0x52
#define MS_GetBrightnessPWMFormat   0x53
#define MS_Set_LDStrength           0x54
#define MS_WR_LD_CalibrateData      0x55
#define MS_WR_HDR_Calibrate_Y       0x56
#define MS_Set_FS2_Brightness       0x57
#define MS_WR_FS2_Brightness        0x58
#define MS_Set_HDR_10StepLumMap     0x59
#define MS_WR_HDRPanelInfo          0x5A
#define MS_WR_HDRColorTempGain      0x5B
#define MS_LoadHDRColorMode         0x5C
#define MS_GetHDRPanelMaxLum        0x5D
#define MS_HDR_BacklightToMax       0x5E
#define MS_WR_HDR_10StepLumMap      0x5F
#define MS_Set_DICOM_Mode           0x60
#define MS_Set_Max_PWMvalue         0x61
#define MS_Set_GammaCurve           0x71
#define MS_Set_ColorTrackMode       0x80
#define MS_WR_ColorMatrix      	    0x84
#define MS_SAVE_ColorMatrix         0x86
#define MS_GET_ColorMode_VerifiedDeltaE 0x90
#define MS_GET_ColorMode_VerifiedColorTemp 0x91
#define MS_GET_ColorMode_VerifiedGamma 0x92
#define MS_GET_ColorMode_VerifiedColorTrack 0x93
#define MS_SAVE_ColorMode_VerifiedDeltaE 0x94
#define MS_SAVE_ColorMode_VerifiedColorTemp 0x95
#define MS_SAVE_ColorMode_VerifiedGamma 0x96
#define MS_SAVE_ColorMode_VerifiedColorTrack 0x97
#define MS_WR_ColorTemp             0x98
#define MS_Set_ColorTemp            0x99
#define MS_GetDeltaEVersion         0x9A
#define MS_GetEEPROMData			0x9F
#if ENABLE_ReadColorFUNCTION_FROM_FW
#define MS_ReadFirmwareSetting   0x9B
#define MS_ReadColorTempValue   0x9C
#endif
#define MS_Save_ColorMode_Adjust_PWM              0x9D
#define MS_Get_ColorMode_Adjust_PWM              0x9E
#define MS_LoadLDTable              0xD4
#define MS_Set_LD_Pattern           0xD5
#define MS_Set_Tool_Info          0xD6
//=========================================================
#define COLORDATA_USEFLASH              1
#define NUM_OF_COLORMODE                1
#define NUM_OF_COLORMODE_GAMMA          (NUM_OF_COLORMODE)
#define NUM_OF_COLOR_TEMP               1
#define NUM_OF_DICOM_GAMMA              1
#define NUM_OF_COLORTRACK_GAMMA         1
#if ENABLE_GAMMA_FUNCTION
#define NUM_OF_ADJUST_PWM				(NUM_OF_COLORMODE_GAMMA+NUM_OF_COLOR_TEMP+NUM_OF_DICOM_GAMMA+NUM_OF_COLORTRACK_GAMMA+1)
#else
#define NUM_OF_ADJUST_PWM				(NUM_OF_COLORMODE_GAMMA+NUM_OF_COLOR_TEMP+NUM_OF_DICOM_GAMMA+NUM_OF_COLORTRACK_GAMMA)
#endif

#define BRIGHTNESS_PWM_FORMAT 0
#if (BRIGHTNESS_PWM_FORMAT == 0) // 0: 8bits 1: 10bits
    #define BRIGHTNESS_REAL_MAX 0xFF //Need to modify by project
    #define BRIGHTNESS_REAL_MID 0x80
    #define BRIGHTNESS_REAL_MIN 0x00 //Need to modify by project
#else
    #define BRIGHTNESS_REAL_MAX 0xFFFF //Need to modify by project
    #define BRIGHTNESS_REAL_MID 0x8000
    #define BRIGHTNESS_REAL_MIN 0x0000 //Need to modify by project
#endif

#if ENABLE_ReadColorFUNCTION_FROM_FW
typedef enum
{
 //color function enable bit offset
        ECFEnBitOffset_Is_Use_3DLut_Table = 0,
        ECFEnBitOffset_sRGB_Enable = 1,
        ECFEnBitOffset_AdobeRGB_Enable = 2,
        ECFEnBitOffset_BT709_Enable = 3,
        ECFEnBitOffset_BT2020_Enable = 4,
        ECFEnBitOffset_DCIP3_Enable = 5,
        ECFEnBitOffset_EBU_Enable = 6,
        ECFEnBitOffset_SMPTEC_Enable = 7,
        ECFEnBitOffset_ColorTemp_Enable = 8,
        ECFEnBitOffset_DICOM_Enable = 9,
        ECFEnBitOffset_GammaCurve_Enable = 10,
        ECFEnBitOffset_ColorTrack_Enable = 11,
        ECFEnBitOffset_HDR_Enable = 12
        //13    reserved
        //...
        //31    reserved
}ECFEnBitOffset;
#endif


typedef enum
{
    DELTAE_COLORMODE_SRGB,
    DELTAE_COLORMODE_ADOBERGB,
    DELTAE_COLORMODE_BT709,
    DELTAE_COLORMODE_BT2020,
    DELTAE_COLORMODE_DCIP3,
    DELTAE_COLORMODE_EBU,
    DELTAE_COLORMODE_SMPTEC,
    DELTAE_COLORMODE_UHDADCIP3,
    DELTAE_NUM_OF_COLORMODE
} EnumDeltaEColorModes;

typedef enum
{
	GammaValue16 = 16,
	GammaValue18 = 18,
	GammaValue20 = 20,
    GammaValue22 = 22,
    GammaValue24 = 24,
    GammaValue26 = 26,
    GammaValuesRGB = 66,
    GammaValue1886 = 86
} EnumGammaValue;

typedef enum
{
	AutoColorFunction_ColorMode = 0,
	AutoColorFunction_DICOMMode = 1,
	AutoColorFunction_GammaCurve = 2,
	AutoColorFunction_ColorTemp = 3,
	AutoColorFunction_ColorTrack = 4,
	AutoColorFunction_UHDA = 5,
	AutoColorFunction_HDR = 6,
	AutoColorFunction_FreeSync2 = 7,
	AutoColorFunction_ColorModeStatus = 8,
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
	AutoColorFunction_SRGB = 9,
	AutoColorFunction_ADOBERGB = 10,
	AutoColorFunction_BT709 = 11,
	AutoColorFunction_BT2020 = 12,
	AutoColorFunction_DCIP3 = 13,
	AutoColorFunction_EBU = 14,
	AutoColorFunction_SMPTEC = 15,
	AutoColorFunction_Default = 0xFF,
#endif

}AutoColorFunction;

typedef enum
{
    LoadStatus_SUCCESS = 0,
    LoadStatus_NOT_FOUND = 0,
    LoadStatus_RESET_GAMMA = BIT0,
    LoadStatus_RESET_GAMUT = BIT1,
    LoadStatus_RESET_RGBGAIN = BIT2,
    LoadStatus_RESET_PWM = BIT3,
    LoadStatus_RESET_ALL = 0x0F,
} LoadStatus;

typedef enum
{
    enPreLoadType_Not_PreLoaded = 0,
    enPreLoadType_BypassDefault = 1,
    enPreLoadType_MTK_ACT = 2,
    enPreLoadType_Customized = 3,
}PreLoadType;

typedef enum{
    ColorToolCCTBase_128 = 1,
    ColorToolCCTBase_255 = 2,
    ColorToolCCTBase_1024 = 3
}ColorToolCCTBase;

typedef enum{
    ColorToolStatus_Init = 1,
    ColorToolStatus_End = 0xFF,
}ColorToolStatus;


#define GammaTableSize    320
typedef struct{
	BYTE ColorMode;
	BYTE funcIdx;
    BYTE GammaValue;
	BYTE CT_idx;
	BYTE PWM_idx;
	BYTE UC_flag;
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
	WORD IDX;
#endif
	BYTE Data[3][GammaTableSize];
	short CheckSum;
} __attribute__((packed))StoredFormatOfPostGamma;

#ifndef DEF_DELTAE_TABLE
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
#define DEF_DELTAE_TABLE    AutoColorFunction_SRGB
#else
#define DEF_DELTAE_TABLE    DELTAE_COLORMODE_SRGB
#endif
#endif

#if ENABLE_ColorMode_FUNCTION
        #define _3DLUTTableSize    125
        typedef struct{
            BYTE ColorMode;
			BYTE funcIdx;
        	BYTE Data[3][_3DLUTTableSize];
        	short CheckSum;
        } __attribute__((packed))StoredFormatOf3DLUT;
        
        typedef struct{
            BYTE ColorMode;
			BYTE funcIdx;
        	short Data[9];
        	short CheckSum;
        }  __attribute__((packed))StoredFormatOfColorMatirx;
        
    #if ENABLE_AC3DLut_FUNCTION
        #define sizeOfColorModeBlock (sizeof(StoredFormatOfPostGamma)*NUM_OF_COLORMODE_GAMMA + sizeof(StoredFormatOf3DLUT)*NUM_OF_COLORMODE)
    #else
        #define sizeOfColorModeBlock (sizeof(StoredFormatOfPostGamma)*NUM_OF_COLORMODE_GAMMA + sizeof(StoredFormatOfColorMatirx)*NUM_OF_COLORMODE)
    #endif
#else
#define sizeOfColorModeBlock 0
#endif


#if ENABLE_DICOM_FUNCTION
#define sizeOfDICOMBlock (sizeof(StoredFormatOfPostGamma)*NUM_OF_DICOM_GAMMA)
#else
#define sizeOfDICOMBlock 0
#endif


#if ENABLE_GAMMA_FUNCTION
#define sizeOfGammaBlock sizeof(StoredFormatOfPostGamma)
#else
#define sizeOfGammaBlock 0
#endif

#if ENABLE_COlORMODE_ADJUST_PWM
typedef struct{
     #if ENABLE_IDX_LOADCOLORMODE_FUNCTION
    	WORD ADJ_PWM;
		DWORD nits;
	 #else
		BYTE ADJ_PWM;
	 #endif
    } __attribute__((packed))BrightnessPWMInfo;
#endif

#if ENABLE_COLORTEMP_WORD_FORMAT
typedef WORD ColorTempRGB; 
#else
typedef BYTE ColorTempRGB;
#endif


typedef struct{
#if ENABLE_COLORTEMP_WORD_FORMAT
        WORD rgbCont[3];
#else
    	BYTE rgbCont[3];
#endif
#if ENABLE_COlORMODE_ADJUST_PWM
		BYTE PWM_idx;
#endif
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
		WORD IDX;
#endif
    } __attribute__((packed))ColorTempInfo;
    typedef struct{
		ColorTempInfo mColorTempInfo;
    	short CheckSum;
    }  __attribute__((packed))StoredFormatOfColorTemp;
    StoredFormatOfColorTemp _colorTemp;
#if ENABLE_COLORTEMP_FUNCTION
#define sizeOfColorTempBlock (sizeof(StoredFormatOfPostGamma) + sizeof(StoredFormatOfColorTemp) * NUM_OF_COLOR_TEMP)
#else
#define sizeOfColorTempBlock 0
#endif

#if ENABLE_COLORTRACK_FUNCTION
#define sizeOfColorTrackBlock (sizeof(StoredFormatOfPostGamma) * NUM_OF_COLORTRACK_GAMMA)
#else
#define sizeOfColorTrackBlock 0
#endif

#if ENABLE_ACHDR_FUNCTION
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
    	DWORD CheckSum;
    }StoredHDRPanelInfo;
    typedef struct{
        BYTE rgbCont[3];
    } HDRColorTemp;
    typedef struct{
        HDRColorTemp mHDRColorTemp;
    	short CheckSum;
    }StoredFormatOfHDRColorTemp;
    typedef struct{
        WORD mHDRLumMap[11];
    	short CheckSum;
    }StoredHDRLumMap;
    //BYTE StoredHDRBrightness;
    typedef struct{
        WORD PWM;
        DWORD maxLum;
    }__attribute__((packed))HDRCalibrateY;
    typedef struct{
        HDRCalibrateY mHDRCalibrateY;
        short CheckSum;
    }__attribute__((packed))StoredHDRCalibrateY;

    //----- LD Online Calibration
    typedef struct{
        BYTE compTbl[512];
        BYTE iMiniPt[2];
        BYTE valStrength;
        BYTE valUseV2;
        BYTE gammaOutTbl[256];
        BYTE gammaInTbl[256];
    }__attribute__((packed))LDCalibrateData;
    typedef struct{
        LDCalibrateData mLDCalibrateData;
    	short CheckSum;
    }__attribute__((packed))StoredLDCalibrateData;

    typedef struct{
        BYTE pwmRatio[10]; //FS2 /HDR PTN10 /HDR PTN100
    } HDRPWMRatio;
    typedef struct{
        HDRPWMRatio mHDRPWMRatio;
    	short CheckSum;
    }StoredFormatOfHDRPWMRatio;
#define sizeOfHDRBlock (sizeof(StoredFormatOfPostGamma) + sizeof(StoredHDRPanelInfo) + sizeof(StoredFormatOfHDRColorTemp) + sizeof(StoredHDRLumMap)+ sizeof(BYTE) + sizeof(StoredHDRCalibrateY) + sizeof(StoredLDCalibrateData) + sizeof(StoredFormatOfHDRPWMRatio))
#else
#define sizeOfHDRBlock 0
#endif

#if ENABLE_COlORMODE_ADJUST_PWM
    typedef struct{
    #if ENABLE_IDX_LOADCOLORMODE_FUNCTION
    	BrightnessPWMInfo mBrightnessPWMInfo[NUM_OF_ADJUST_PWM];
    	DWORD CheckSum[NUM_OF_ADJUST_PWM];
	#else
		BrightnessPWMInfo mBrightnessPWMInfo[30];
    	BYTE CheckSum[30];
	#endif
    }__attribute__((packed))StoredFormatOfAdjustPWM;

StoredFormatOfAdjustPWM _AdjustPWM;

#define sizeAdjustPWMblock (sizeof(StoredFormatOfAdjustPWM))
#else
#define sizeAdjustPWMblock 0
#endif

#if ENABLE_SAVE_COlORMODE_VERIFIED_VALUES

    typedef struct{
    	WORD Vrf_DeltaE[7];
		WORD Vrf_ColorTemp[7];
		WORD Vrf_Gamma[6]; // 1.6, 1.8, 2.0, 2.2, 2.4, 2.6
		WORD Vrf_ColorTrack[10];
    	WORD CheckSum[30];
    }__attribute__((packed))StoredFormatOfVerifiedValues;

StoredFormatOfVerifiedValues _VerifiedValues;

#define sizeVerifiedValuesblock (sizeof(StoredFormatOfVerifiedValues))
#else
#define sizeVerifiedValuesblock 0
#endif


#define sizeOfDeltaEBlock (sizeOfColorModeBlock + sizeOfDICOMBlock + sizeOfGammaBlock + sizeOfColorTempBlock +\
                           sizeOfColorTrackBlock + sizeOfHDRBlock + sizeAdjustPWMblock + sizeVerifiedValuesblock)

static const int _channelNum = 3;     //Number of channels
static const int _maxEntries = 1024;  //Number of entries in native de-gamma table
static const int _segPntNum = 3;    //Number of segements
static const int _maxDelta = 0x20;
static const int _comMaxEntries = 384;//Number of entries in compressed de-gamma talbe;

typedef enum
{
    enMemoryType_EEPROM,
    enMemoryType_FLASH,
} MemoryType;

typedef enum
{
    enColorMode3DLutType_5x5x5,
    enColorMode3DLutType_17x17x17,
} ColorMode3DLutType;

typedef enum
{
    enPostGammaType_Compressed,
    enPostGammaType_NonCompressed,
} PostGammaType;

typedef struct{
    WORD u16MaxValR;
    WORD u16MaxValG;
    WORD u16MaxValB;
}GammaMaxValue;


#if (ENABLE_ColorMode_Preload_Function)
typedef struct{
WORD IDX;
BOOL bPreloaded;
BYTE** ColorMode3DLut;
short* ColorMatrix;
BYTE* DeGamma;
BYTE* FixGamma;
BYTE** PostGamma;
ColorTempRGB* PostGain;
BrightnessPWMInfo* PWMInfo;
BOOL* UCflag;
}CustomizedColorMode;

typedef struct{
    WORD IDX;
    PreLoadType enPreloadType;
    BYTE** PostGamma;
    BYTE** ColorMode3DLut;
    short* ColorMatrix;
    BYTE* DeGamma;
    BYTE* FixGamma;
    ColorTempRGB* PostGain;
#if ENABLE_COlORMODE_ADJUST_PWM
    BrightnessPWMInfo PWMInfo;
#endif
    BOOL* UCflag;
}IDXColorMode;

enum
{
    enDeltaECBIDXUserPrefAdjust,
    // etc ..
    enDeltaECBMaxNum,
};

typedef void ( *fpCallback )              (void);
typedef void ( *fpIDXUserPrefAdjust )    (IDXColorMode* pstIDXColorMode);
void mdrv_DeltaE_CbAttach(BYTE CbType, void *CbFunc);

void mdrv_DetalE_CustomizedDefaultColorMode_Set(CustomizedColorMode stCustomizedColorMode);
void mdrv_DetalE_AllIDXColorMode_PreLoad(void);
void mdrv_DeltaE_Init(DWORD addr, MemoryType enType);
#endif
LoadStatus mdrv_DetalE_IDXColorMode_Load(BYTE u8DispWin, WORD IDX);
LoadStatus mdrv_DetalE_IDXGammaCurveMode_Load(BYTE u8DispWin, WORD IDX, BYTE GammaIndex);
BYTE* mdrv_DeltaE_DegammaTbl_Get(BYTE GammaValue);
BYTE* mdrv_DeltaE_FixgammaTbl_Get(BYTE GammaValue);
BYTE mdrv_DeltaE_AutoColorToolStatus_Get(void);
void mdrv_DeltaE_AutoColorToolStatus_Set(BYTE value);
//-------------------------------------------------------------------------------
// Specify the Delta-E color mode. sRGB/AdobeRGB
//-------------------------------------------------------------------------------
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
extern LoadStatus mdrv_DetalE_IDXColorMode_Load(BYTE u8DispWin, WORD IDX);
extern LoadStatus IDXLoadDeltaEColorMode(BYTE u8DispWin, WORD IDX);
extern LoadStatus IDXLoadGammaCurveMode(BYTE u8DispWin, WORD IDX, BYTE GammaIndex);
extern LoadStatus msNewLoadColorMode(BYTE u8DispWin, BYTE ucColorMode, BYTE funcIdx);
#else
extern void msLoadDeltaEColorMode(BYTE u8DispWin, BYTE ucColorMode);
#endif
//-------------------------------------------------------------------------------
// Apply DICOM mode
//-------------------------------------------------------------------------------
extern void msSetDICOMMode(BYTE u8DispWin, BYTE DICOMModeIdx);
extern DWORD GetAddrOfDICOM(BYTE idx);
//-------------------------------------------------------------------------------
// Fix gamma + Post gamma
//-------------------------------------------------------------------------------
extern LoadStatus msSetGammaCurve(BYTE u8DispWin, BYTE GammaIndex);
extern DWORD GetAddrOfPostGamma(void);
//-------------------------------------------------------------------------------
// Output Test Pattern For Measurement. sRGB/AdobeRGB
//-------------------------------------------------------------------------------
extern void msSetTestPattern(BOOL IsOn, BYTE R, BYTE G , BYTE B);
//-------------------------------------------------------------------------------
// Output Color Engine Test Pattern For Verify Color Function ( sRGB , AdobeRGB , BT709 , BT2020 ... )
//-------------------------------------------------------------------------------
extern void msSetColorEngineTestPattern(BYTE u8WinIdx, Bool bEnable, WORD u16Red, WORD u16Green, WORD u16Blue);
extern void msSetHDRColorEngineTestPattern(BYTE u8WinIdx, Bool bEnable, BYTE R, BYTE G, BYTE B, int stepAddr);
//extern void msInitTestPattern_WithoutSignal(void); //modified name
//extern void msExitTestPattern_WithoutSignal(void); //modified name
extern void msInitIP2TestPattern_WithoutSignal(void);
extern void msExitIP2TestPattern_WithoutSignal(void);

//-------------------------------------------------------------------------------
// Control Color Engine to On/Off, it includes de-gamma/fix-gamma/post-gamma & color matrix.
//-------------------------------------------------------------------------------
extern void msSetColorEngine(BYTE u8DispWin, BOOL IsOn);
extern void msRecoverColorEngine(BYTE *ColorEngineStatus);
extern void msByPassVOPColorEngine(BOOL bEnable);
extern void msByPassVIPColorEngine(BOOL bEnable);
//-------------------------------------------------------------------------------
// Assign color matrix data.
//-------------------------------------------------------------------------------
extern void msSetColorMatrixData(int idx, short data);
//-------------------------------------------------------------------------------
// Save color matrix data to EEPROM.
//-------------------------------------------------------------------------------
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
extern void msSaveColorMatrixData(BYTE ucColorMode, BYTE ucfuncIdx, BYTE idx);
#else
extern void msSaveColorMatrixData(BYTE ucDeltaEColorMode, BYTE idx);
#endif
//-------------------------------------------------------------------------------
// Assign 3D LUT data.
//-------------------------------------------------------------------------------
extern void msSet3DLUTData(int ch, int idx, BYTE data);
//-------------------------------------------------------------------------------
// Save 3D LUT data to EEPROM.
//-------------------------------------------------------------------------------
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
extern void msSave3DLUTData(BYTE ucColorMode, BYTE funcIdx, BYTE idx);
#else
extern void msSave3DLUTData(BYTE ucColorMode, BYTE idx);
#endif
//-------------------------------------------------------------------------------
// Assign post gamma data.
//-------------------------------------------------------------------------------
extern void msSetPostGammaData(int ch, int idx, BYTE data);
//-------------------------------------------------------------------------------
// Save post gamma data to EEPROM.
//-------------------------------------------------------------------------------
extern DWORD GetAddrOfColorModePostGamma(BYTE idx);
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
extern void  msSavePostGammaData(DWORD addr, BYTE ColorMode, BYTE funcIdx, BYTE GammaValue, BYTE CT_idx, BYTE PWM_idx, BYTE UC_flag, WORD IDX);
extern void  msSavePostGammaTable(BYTE mode, BYTE idx, BYTE funcIdx,BYTE GammaValue, BYTE CT_idx, BYTE PWM_idx, BYTE UC_flagX, WORD IDX);
#else
extern void  msSavePostGammaData(DWORD addr, BYTE GammaValue);
extern void  msSavePostGammaTable(BYTE mode, BYTE idx, BYTE GammaValue);
#endif
//-------------------------------------------------------------------------------
// Assign color temp data.
//-------------------------------------------------------------------------------
#if ENABLE_COLORTEMP_FUNCTION
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
extern void msSetColorTempData(int idx, WORD u16Red, WORD u16Green, WORD u16Blue, BYTE PWM_idx, WORD IDX);
extern BOOL IDXGetColorTempInfo(WORD u16IDX, ColorTempInfo *mColorTempInfo);
extern BOOL msGetColorTempData ( BYTE idx , ColorTempInfo *mColorTempInfo);
#else
extern void msSetColorTempData(int idx, WORD u16Red, WORD u16Green, WORD u16Blue);
#endif
#endif
extern void msByPassGainOffset(BYTE u8DispWin);
extern void mdrv_DeltaE_RGBGain_Get(WORD *u16Red, WORD *u16Green, WORD *u16Blue);
//-------------------------------------------------------------------------------
// Save color temp data to EEPROM.
//-------------------------------------------------------------------------------
extern void msSaveColorTempData(int idx);
extern DWORD GetAddrOfColorTempRGBGain(BYTE idx);
extern DWORD GetAddrOfColorTempPostGamma(void);
//-------------------------------------------------------------------------------
// Load color temp data from EEPROM.
//-------------------------------------------------------------------------------
extern void msLoadRGBGain(BYTE idx);
extern LoadStatus msLoadColorTempMode(BYTE u8DispWin, BYTE idx);
#if ENABLE_COLORTEMP_FUNCTION
extern void msLoadColorTempMode2(BYTE idx, ColorType * pstColor);
#endif
//-------------------------------------------------------------------------------
// Set color track mode
//-------------------------------------------------------------------------------
#if ENABLE_COLORTRACK_FUNCTION
extern void msSetColorTrackMode(BYTE u8DispWin, BYTE GammaIndex);
extern DWORD GetAddrOfColorTrackPostGamma(BYTE idx);
#endif
//-------------------------------------------------------------------------------
// Set UHDA color mode
//-------------------------------------------------------------------------------
extern void LoadUHDAColorMode(void);

//-------------------------------------------------------------------------------
// Adjust Luminance PWM
//-------------------------------------------------------------------------------
#if ENABLE_COlORMODE_ADJUST_PWM 
#if ENABLE_IDX_LOADCOLORMODE_FUNCTION
extern BOOL IDXGetColorModeBrightnessPWMInfo(WORD u16IDX, BrightnessPWMInfo *mBrightnessPWMInfo);
extern void msSetColorModeAdjustBrightnessPWMInfo(int idx, BYTE u8PWMlow, BYTE u8PWMhigh, DWORD u32nits );
extern void msSaveColorModeAdjustPWM(void);
extern BOOL msGetColorModeBrightnessPWMInfo ( BYTE idx , BrightnessPWMInfo* mBrightnessPWMInfo); 
extern void ResetAdjustPWM(void);
#else
extern void msSetColorModeAdjustPWM(int idx, BYTE u8PWMValue );
extern void msSaveColorModeAdjustPWM(void);
extern BYTE msGetColorModeAdjustPWM ( BYTE idx ); 
#endif
#endif

//-------------------------------------------------------------------------------
// Save Verified Values to EEPROM
//-------------------------------------------------------------------------------
#if ENABLE_SAVE_COlORMODE_VERIFIED_VALUES 
extern void msSetColorModeVerfiedDeltaE(int idx, WORD u16DeltaEValue ); 
extern void msSetColorModeVerfiedColorTemp(int idx, WORD u16ColorTempValue ); 
extern void msSetColorModeVerfiedGamma(int idx, WORD u16GammaValue ); 
extern void msSetColorModeVerfiedColorTrack(int idx, WORD u16ColorTrackValue ); 
extern void msSaveColorModeVerifiedValues(void);
extern WORD msGetColorModeVerfiedValues ( BYTE type, BYTE idx );
#endif
//-------------------------------------------------------------------------------
// Set Auto Calibration HDR color mode
//-------------------------------------------------------------------------------
#if ENABLE_ACHDR_FUNCTION
extern void msSetACHDRPanelInfo(HDRPanelInfo *pHDRInfo);
extern void msSaveHDRPanelInfo(void);
extern BOOL msLoadACHDRPanelInfo(StoredHDRPanelInfo *pHDRPanelInfo);
extern void msLoadACHDRGamma(StoredFormatOfPostGamma *pStoredPostGamma);

//-------------------------------------------------------------------------------
// Set Auto Calibration HDR Color Temp
//-------------------------------------------------------------------------------
extern DWORD GetAddrOfHDRColorTempRGBGain(void);
extern void msSaveHDRColorTempRGBGain(void);
extern void msSetHDRColorTempRGBGain(HDRColorTemp *pHDRColorTemp);
extern void msLoadHDRColorTempMode(void);
extern void msLoadACHDRColorTemp(StoredFormatOfHDRColorTemp *pStoredHDRColorTemp);
extern void msLoadACHDRColorTempMode(BYTE u8DispWin);

//-------------------------------------------------------------------------------
// Auto Calibration Verify HDR Mode
//-------------------------------------------------------------------------------
extern void msVerifyHDRMode(BYTE u8WinIdx, BYTE enHDR);
//-------------------------------------------------------------------------------
// Set Auto Calibration HDR 10 step luminance map
//-------------------------------------------------------------------------------
extern DWORD GetAddrOfHDRLumMap(void);
extern void msSetHDRLumMap(StoredHDRLumMap *pHDRLumMap);
extern void msSaveHDRLumMap(void);
extern BOOL msLoadACHDRLumMap(StoredHDRLumMap *pHDRLumMap);
//-------------------------------------------------------------------------------
// Access Adjust HDR Brightness
//-------------------------------------------------------------------------------
extern DWORD GetAddrOfHDRBrightness(void);
extern DWORD GetAddrOfHDRCalibrateY(void);
extern void msSaveHDRCalibrateY(void);
extern void msSetACHDRCalibrateY(HDRCalibrateY *pHDRCalibrateY);
extern BOOL msLoadACHDRCalibrateY(StoredHDRCalibrateY *pHDRCalibrateY);
//-------------------------------------------------------------------------------
// Set LD Online Calibration Data
//-------------------------------------------------------------------------------
extern DWORD GetAddrOfLDCalibrateData(void);
extern void msSaveLDCalibrateData(void);
extern void msSetLDCalibrateData(int idx, BYTE data);
extern BOOL msLoadLDCalibrateData(StoredLDCalibrateData *pStoredLDCalibrateData);
//-------------------------------------------------------------------------------
// Set HDR PWM Ratio Data
//-------------------------------------------------------------------------------
extern DWORD GetAddrOfHDRPWMRatioData(void);
extern void msSaveHDRPWMRatioData(void);
extern void msSetHDRPWMRatioData(int idx, BYTE data);
extern BOOL msLoadHDRPWMRatioData(StoredFormatOfHDRPWMRatio *pStoredFormatOfHDRPWMRatio);
#endif
//-------------------------------------------------------------------------------
// degamma Algorithm
//-------------------------------------------------------------------------------

extern void msCompressGamma(float nativeGma[_channelNum][_maxEntries],
							 unsigned char compressedGma[_channelNum][_comMaxEntries],
							 unsigned char segOft[_segPntNum],
							 unsigned int segPnt[_channelNum][_segPntNum]);
extern void msCompressGammaWithSegValue(float nativeGma[_channelNum][_maxEntries],
							unsigned char compressedGma[_channelNum][_comMaxEntries],
							unsigned char segOftVal[_channelNum][_segPntNum + 1],
							unsigned int segPnt[_channelNum][_segPntNum]);
extern void msInitSegSettings(float nativeGma[_channelNum][_maxEntries],
								unsigned char segOft[_segPntNum],
								unsigned char segOftVal[_channelNum][_segPntNum + 1],
								unsigned int segPnt[_channelNum][_segPntNum]);

extern int msGetSegOffset(unsigned int segPnt[_channelNum][_segPntNum],
							unsigned char segOftVal[_channelNum][_segPntNum + 1],
							int ch,
							int entry);

extern void msFindMaxDelta(float nativeGma[_channelNum][_maxEntries], int maxDelta[_channelNum], unsigned char point[_channelNum]);

extern void msResetWholeColorCalibrationData(void);

BYTE AutoColorCalibrationHandler(BYTE u8WinIdx, WORD *_checkSum);

extern BOOL IsAutoColorCommand(BYTE CommandCode);

extern void msLoadHDRColorMode(short* p3x3);
extern LoadStatus msReloadGammaData(BYTE u8DispWin, DWORD addr);
extern DWORD GetAddrOfHDRPostGamma(void);

//-------------------------------------------------------------------------------
// Output Test Pattern For verfiy HDR
//-------------------------------------------------------------------------------
extern void msSetIP2TestPattern_Off(void);
extern void msSetIP2TestPattern_ImageSize(BYTE u8WinIdx, WORD hSize, WORD vSize);
void msSetIP2TestPattern_ScalerMask(WORD u16ScMask);
extern void msSetIP2TestPattern_WithoutSignal(BOOL IsOn);
extern BOOL msGetIP2TestPattern_WithoutSignal(void);
extern void msSetIP2TestPattern_PureColor(BYTE u8WinIdx, WORD u16R, WORD u16G, WORD u16B);
extern void msSetIP2TestPattern_GreyScale(BYTE u8WinIdx, WORD numOfScale, BOOL bIsHorizontal);
extern void msSetIP2TestPattern_XPercentPIP(BYTE u8WinIdx, double percentOfCentralArea, WORD u16R, WORD u16G, WORD u16B);
extern void msSetIP2TestPattern_CheckBoardBox(BYTE u8WinIdx, WORD u16GregCode, WORD numOfBlock_h, WORD numOfBlock_v);

extern void SetStopLDUpdateStatus(BOOL bStopLDUpdate);
extern void mdrv_DetalE_XPercentPIP_SetSize(BYTE u8WinIdx, double percentOfCentralArea);
//-------------------------------------------------------------------------------
// Flash Read Write
//-------------------------------------------------------------------------------
extern void mdrv_DeltaE_FlashWriteProtect_Set(Bool bDoWP);
extern BOOL mdrv_DeltaE_FlashByteWriter_Set(void (*fpFlashWriteByte)(Bool bDoWP, DWORD u32Addr, BYTE u8Value));
extern BOOL mdrv_DeltaE_FlashTblWriter_Set(void (*fpFlashWriteTbl)(Bool bDoWP, DWORD u32Addr, BYTE *buffer, DWORD count));
extern void mdrv_DeltaE_Memory_Init(DWORD addr, MemoryType enMemoryType);

#if ENABLE_ACHDR_FUNCTION
enum
{
    e100percent = 0,
    e10percent = 1,
    eFS2 = 2,
};

typedef struct{
    WORD ADJ_PWM[10];  //HDR100%/HDR10%/FS2
    DWORD nits;
} __attribute__((packed))HDRBrightnessPWMInfo;
extern BOOL IDXGetHDRBrightnessPWMInfo( WORD u16IDX, HDRBrightnessPWMInfo* mBrightnessPWMInfo); 
extern DWORD GetAddrOfAutoColorFunctionPostGamma(BYTE mode, BYTE idx);

#endif
#endif
