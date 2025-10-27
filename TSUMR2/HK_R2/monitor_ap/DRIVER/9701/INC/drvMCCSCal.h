#ifndef _DRVMCCSCAL_H
#define _DRVMCCSCAL_H

#include "Global.h"

//==========DELL MCCS COMMAND====================================
#define MCCS_COLORAP_COMMAND_READ       0xF1
#define MCCS_COLORAP_COMMAND_WRITE      0xF0
#define MCCS_COLORAP_SaveAdjustment     0x02
#define MCCS_COLORAP_RestoreFactory     0x04
#define MCCS_COLORAP_Bypass             0x08
#define MCCS_COLORAP_Brightness         0x10
#define MCCS_COLORAP_Contrast           0x12
#define MCCS_COLORAP_Color_Preset       0x14
#define MCCS_COLORAP_RGB_Gain           0x16
#define MCCS_COLORAP_RGB_Offset         0x17
#define MCCS_COLORAP_Color_Matrix       0x18
#define MCCS_COLORAP_Post_Gamma         0x30
#define MCCS_COLORAP_Fix_Gamma          0x31
#define MCCS_COLORAP_Pre_Gamma          0x32
#define MCCS_COLORAP_Panel_Gamma         0x33
#define MCCS_COLORAP_Gamma_CheckSum     0x34
#define MCCS_COLORAP_ResetPostGamma     0x35
#define MCCS_COLORAP_ResetPreGamma      0x36
#define MCCS_COLORAP_ResetFixGamma      0x37
#define MCCS_COLORAP_ResetPanelGamma    0x38
#define MCCS_COLORAP_AdjustSpecificLUT  0x39
#define MCCS_COLORAP_3DLUT              0x3A
#define MCCS_COLORAP_3DLUT_Custom   0x42
#define MCCS_COLORAP_3DLUTGain          0x3B
#define MCCS_COLORAP_3DLUTGrayGuard     0x3C
#define MCCS_COLORAP_MeasurePtternInfo  0x3E
#define MCCS_COLORAP_MeasurePtternInfo_Save  0x3F
#define MCCS_COLORAP_UCStart            0x70
#define MCCS_COLORAP_UCGridSize         0x71
#define MCCS_COLORAP_UCData             0x72
#define MCCS_COLORAP_UCSave             0x73
#define MCCS_COLORAP_Uniformity              0x74
#define MCCS_COLORAP_Uniformity_Custom  0x44
#define MCCS_COLORAP_MntDisplayTime     0x75
#define MCCS_COLORAP_UCEnable_CAL1      0x79
#define MCCS_COLORAP_UCEnable_CAL2      0x7A
#define MCCS_COLORAP_UCEnable_CAL3      0x7B
#define MCCS_COLORAP_Uniformity_Preset 0x7C
#define MCCS_COLORAP_Uniformity_Save 0x7D
#define MCCS_COLORAP_Saturation         0x8A
#define MCCS_COLORAP_Hue                0x90
#define MCCS_COLORAP_6axis_Saturation   0x9A
#define MCCS_COLORAP_6axis_Hue          0x9B
#define MCCS_COLORAP_6axis_Brightness   0x9C
#define MCCS_COLORAP_DisableOSD         0xB0
#define MCCS_COLORAP_EnableOSD          0xB1
#define MCCS_COLORAP_ToggleInternalPattern   0xB2
#define MCCS_COLORAP_InternalPattern         0xB3
#define MCCS_COLORAP_ToggleXVYCCPATTERN      0xB4
#define MCCS_COLORAP_XVYCCPATTERN            0xB5

#define FORMAT3RD3DLUT_FMTS7 0
#define FORMAT3RD3DLUT_FMTS9 1
#define FORMATUCDATAWITHLAYERGAIN  1 //2022 new format(MCCS_COLORAP_Uniformity, old format(MCCS_COLORAP_UCStart/MCCS_COLORAP_UCData/MCCS_COLORAP_UCSave/MCCS_COLORAP_UCGridSize)
#define FORMAT_SEL_3RD3DLUT FORMAT3RD3DLUT_FMTS7

#define OFFSETOF(TYPE,ELEMENT) ((size_t)&(((TYPE*)0)->ELEMENT))

static const int _maxEntrieBytes= 2048;
#if FORMAT_SEL_3RD3DLUT == FORMAT3RD3DLUT_FMTS9
static const int _3DLUTEntrieBytes= 9826;
#else
static const int _3DLUTEntrieBytes= 4913;
#endif

#define NUM_OF_USER_CALIBRATION_BLOCK   1
#define NUM_OF_USER_CALIBRATION_DEMURA_BLOCK   2

#define MOVE_MST9U 0 //move to MST9U incomplete

struct MCCS_PARAMETER{
#if (CHIP_ID == MST9U4) ||(CHIP_ID == MST9U5) //|| (CHIP_ID == MST9U6)
    BYTE  bPanelGammaModified;
#endif
    BYTE  bPostGammaModified;
    BYTE  bPreGammaModified;
    BYTE  bFixGammaModified;
    BYTE  b3DLUTModified;
    BYTE  bSixAxisHueModified;
    BYTE  bSixAxisSatModified;
    BYTE  bSixAxisBriModified;
    BYTE  bRGBGainModified;
    BYTE  bRGBOffsetModified;
    BYTE  bColorMatrixModified;
    BYTE  bBrightnessModified;
    BYTE  bContrastModified;
    BYTE  bSaturationModified;
    BYTE  bHueModified;
    BYTE  u8ModifiedFlagCheckSum;
#if (CHIP_ID == MST9U4) ||(CHIP_ID == MST9U5) //|| (CHIP_ID == MST9U6)
    BYTE  u8PanelGamma[3][2048];
#endif
    BYTE  u8PreGamma[3][2048];
    BYTE  u8FixGamma[3][512];
    BYTE  u8PostGamma[3][2048];
    #if ENABLE_3DLUT
    #if FORMAT_SEL_3RD3DLUT == FORMAT3RD3DLUT_FMTS9
        WORD u163DLUT[3][4913];
    #else
        BYTE  u163DLUT[3][4913];
    #endif
    #endif
    BYTE  u83DLUTGain;
    BYTE  u8SixAxisHue[6];
    BYTE  u8SixAxisSat[6];
    BYTE  u8SixAxisBri[6];
    BYTE  u8RGBGain[3];
    BYTE  u8RGBOffset[3];
    short  ColorMatrix[9];
    BYTE  u8Brightness;
    BYTE  u8Contrast;
    BYTE  u8Saturation;
    BYTE  u8Hue;
    BYTE  u8PresetNumber;
    BYTE  u83DLUTGrayGuard;
};

#define MEASURE_COUNT 33

typedef struct
{
    BYTE X[3];
    BYTE Y[3];
    BYTE Z[3];
}  __attribute__((packed))MeasureDataElement;//MeasureDataElement;//

typedef struct
{
    MeasureDataElement R[MEASURE_COUNT];
    MeasureDataElement G[MEASURE_COUNT];
    MeasureDataElement B[MEASURE_COUNT];
    MeasureDataElement W[MEASURE_COUNT];
} MCCS_MeasurePatternInfo;

#if MOVE_MST9U//ENABLE_UC_CALIBRATION_FOR_THIRD_PARTY
#if FORMATUCDATAWITHLAYERGAIN
typedef struct
{
    U8 LayerGains[8]; //L255(2bytes)/L192(2bytes)/L128(2bytes)/L63(2bytes)
    U16 CheckSumLayerGain;
    BYTE DemuraTbl[MAX_NODES*4*3]; //node(L0(RGB)~L3(RGB)...nodeN(L0(RGB)~L3(RGB))
    U16 CheckSumTbl;
}__attribute__((packed))MCCS_Demura_Data;

typedef struct
{
    MCCS_Demura_Data data;
    BYTE bDemuraTblModified;
}__attribute__((packed))MCCS_Demura;

#else
    BYTE  u8DemuraTbl[sizeOfDeMuraBlock];
#endif
#endif

#define NumOf3rdPartyColorEngine OFFSETOF(struct MCCS_PARAMETER,u8ModifiedFlagCheckSum)

extern void msBypassMCCSColorEngine(BYTE u8WinIdx);
extern BYTE MCCSCalibrationHandler(BYTE u8WinIdx, BYTE u8Source, BYTE CommandCode);

#if MOVE_MST9U//ENABLE_DEMURA_FUNCTION&& ExtremeDataAlgorithm && ENABLE_UC_CALIBRATION_FOR_THIRD_PARTY && FORMATUCDATAWITHLAYERGAIN
void Wrap3rdUCToStdUC(void);
#endif

#if ENABLE_SaveMCCSData
#if MOVE_MST9U//ENABLE_UC_CALIBRATION_FOR_THIRD_PARTY && FORMATUCDATAWITHLAYERGAIN
#define sizeOfMCCSDataBlock ((sizeof(struct MCCS_PARAMETER)*NUM_OF_USER_CALIBRATION_BLOCK)+sizeof(MCCS_MeasurePatternInfo))+sizeof(MCCS_Demura)*NUM_OF_USER_CALIBRATION_DEMURA_BLOCK
BOOL msLoadMCCSDemuraData(BYTE CSIndex);
#else
#define sizeOfMCCSDataBlock ((sizeof(struct MCCS_PARAMETER)*NUM_OF_USER_CALIBRATION_BLOCK)+sizeof(MCCS_MeasurePatternInfo))
#endif
extern DWORD GetAddrOfMCCSStruct(BYTE CSIndex);
extern DWORD GetAddrOfNativeData(void);
extern BOOL msLoadMCCSColorData(BYTE CSIndex);
extern BOOL msLoadNativeData(void);
extern void  msSaveMCCSColorData(BYTE CSIndex);
extern void  msSaveNativeData(void);
void mdrv_MCCS_PatternInfo_Set(MCCS_MeasurePatternInfo* ptnInfo);
extern void  msDiscardMCCSColorData(BYTE CSIndex);
extern void msApplyMCCSColorData(BYTE u8WinIdx);
#if MOVE_MST9U//ENABLE_DEMURA_FUNCTION&& ExtremeDataAlgorithm && ENABLE_UC_CALIBRATION_FOR_THIRD_PARTY && (!FORMATUCDATAWITHLAYERGAIN)
void UCSaveSettingFor3rdParty(void);
DWORD GetAddOfMCCSStruct_DemuraTbl(BYTE CSIndex);
#endif

#if MOVE_MST9U//ENABLE_DEMURA_FUNCTION && ENABLE_UC_CALIBRATION_FOR_THIRD_PARTY
BOOL msSaveMCCSDemuraData(BYTE CSIndex);
DWORD GetAddrOfMCCSDemuraStruct(BYTE CSIndex);
#endif

#endif

#if MOVE_MST9U//ENABLE_DEMURA_FUNCTION && ENABLE_UC_CALIBRATION_FOR_THIRD_PARTY
void msApplyMCCSDemuraData(void);
extern void RestoreToFactoryUCTable(void);
extern Bool IsMCCSUniformityDataExist(void);
#endif

#endif

