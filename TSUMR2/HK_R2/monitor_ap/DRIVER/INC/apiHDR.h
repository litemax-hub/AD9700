

#ifndef _APIHDR_H_
#define _APIHDR_H_

//#if ENABLE_HDR
#define HDR_Uncalled_Segment 0

typedef struct
{
    BOOL IsValid;
    float matrix[3][3];
} Matrix3x3;

typedef enum
{
    HDR_Y2R,
    HDR_R2Y,
    VOP_Y2R,
}HDR_CSC;

void API_InitHDR(BYTE u8WinIdx);
void API_CSC(BYTE u8WinIdx, HDR_CSC cscType);
void API_LoadDeGammaTabel(BYTE u8WinIdx, const void* pTable);
void API_LoadGammaTable(BYTE u8WinIdx, const void* pTable);
#if HDR_Uncalled_Segment
void API_LoadPanelGammaTable(BYTE u8WinIdx, BYTE **pu8Table);
void API_LoadTMOCurve(BYTE u8WinIdx, void* pTable);
#endif
void API_LoadColorMatrix( BYTE u8WinIdx, Matrix3x3 m);
void API_OpenHDRMode(BYTE u8WinIdx);
void API_CloseHDRMode(BYTE u8WinIdx);
BOOL API_CheckWindowSize(BYTE u8WinIdx);
void API_ConfigB04(BYTE u8WinIdx, WORD gain, DWORD highTh);
void FireAutoDownload(BYTE u8WinIdx);
void API_SetHDREOTFGain(BYTE u8WinIdx, float gain);

void WriteXVYCCTable(BYTE u8WinIdx);
#if (CHIP_ID == CHIP_MT9700)
void WritePostGammaTable(BYTE u8WinIdx);
#endif
void LoadXVyccLUT( BYTE u8WinIdx);
void msSetHDRCSCType_Bypass(BYTE u8WinIdx);
#if (CHIP_ID == CHIP_MT9701)
void API_LoadOOTFTable(BYTE u8WinIdx, const void* pTable);
#endif
#if (ENABLE_DOLBY_HDR) 
void API_Dolby_Addr_Init(void);
#endif
#endif

