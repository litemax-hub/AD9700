#ifndef _AUTOGAMMA_H

#define _AUTOGAMMA_H

#include "drvHDR.h"


#ifdef _AUTOGAMMA_C

#define INTERFACE

#else

#define INTERFACE   extern

#endif


#define OPTION_ENABLE_WHITEBAL 0

#define OPTION_ENABLE_CHKSUM 1

#define OPTION_LOADEEPTBLToSRAM 0

#if ENABLE_LUT_AUTODOWNLOAD
#include "msFB.h"

#define DEGAMMA_TABLE_ENTRIES           1024
#define FIXGAMMA_TABLE_ENTRIES          256
#define FIXGAMMA_EX_TABLE_ENTRIES       96 // Extend entries = 88, allocate to 96
#define POSTGAMMA_TABLE_ENTRIES         1024
#define POSTGAMMA_TABLE_HALF_ENTRIES    128
#define PANELGAMMA_TABLE_ENTRIES        1024

#endif

#if !ENABLE_LUT_AUTODOWNLOAD
INTERFACE void LoadPostGammaTbl(BOOL bWindow, BYTE GamIndex);
INTERFACE void msWriteDeGammaTbl(BOOL bWindow, BYTE ModeIndex);
INTERFACE void msWriteFixGammaTbl(BOOL bWindow, BYTE ModeIndex, BOOL bExtendMode);
#endif

#if !HDR_AUTODOWNLOAD
INTERFACE void LoadHDRPostGammaTbl(void);
#endif

INTERFACE void msWriteFGammaCM(BYTE  bWindow,  BYTE ucGammaTableIdx);

INTERFACE void msSetFGammaCMOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void msSetFGammaOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void msSetFixGammaOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void msSetPostGammaOnOff(BOOL bWindow, BOOL bSwitch);

INTERFACE void LoadDeltaEColorMode(BOOL bWindow, BYTE ModeIndex);

INTERFACE void msLoadEEPGmaTBLToSRAM(WORD GmaTblAddr1, BYTE GmaTblSize);

#if 0
INTERFACE void LoadHDRColorMode(BOOL bWindow, short* p3x3Mat);
#endif

#if ENABLE_LUT_AUTODOWNLOAD
INTERFACE void WriteTriggerADLxvYCCGamma(DWORD u16StartAddr, BOOL Enable);
INTERFACE void WriteDeGamma2Dram(BYTE indx, BYTE *pu8GammaTblIdx);
INTERFACE void WriteFixGamma2Dram(BYTE indx, BYTE *pu8GammaTblIdx, BOOL bExtendMode);
INTERFACE void WritePostGamma2Dram(BYTE indx, BYTE *pu8GammaTblIdx);
INTERFACE void WriteTriggerADLPostGamma(DWORD u16StartAddr, BOOL Enable);
INTERFACE void WritePanelGamma2Dram(BYTE indx, BYTE *pu8GammaTblIdx);
INTERFACE void WriteTriggerADLPanelGamma(DWORD u16StartAddr, BOOL Enable);
#endif

INTERFACE void msSetPanelGammaOnOff(BOOL bSwitch);
INTERFACE void LoadPanelGammaTbl(BYTE u8WinIdx, BYTE GamIndex);
INTERFACE void LoadPanelGammaTbl_ADL(BYTE u8WinIdx, BYTE GamIndex);
#undef INTERFACE

#endif

