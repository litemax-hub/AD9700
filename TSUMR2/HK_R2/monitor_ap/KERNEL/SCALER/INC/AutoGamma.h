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
#include "drvIMI.h"
#define FULL_DEGAMMA_TABLE_ENTRIES      600
#define DEGAMMA_TABLE_ENTRIES           256
#define FIXGAMMA_TABLE_ENTRIES          256
#define FIXGAMMA_EX_TABLE_ENTRIES       96 // Extend entries = 88, allocate to 96
#define POSTGAMMA_TABLE_ENTRIES         256
#define POSTGAMMA_TABLE_HALF_ENTRIES    128
#define PANELGAMMA_TABLE_ENTRIES        1024

#define DEGAMMA_ADL_SIZE        0x1C20 //600E * 4byte * 3 (2byte data alignment to 4byte)
#define FIXGAMMA_ADL_SIZE       0x840 //(256E + 96E) * 2byte * 3
#define POSTGAMMA_ADL_SIZE      0x600 //256E * 2byte * 3
#define PANELGAMMA_ADL_SIZE     0x1800 //1024E * 2byte * 3

#define DEGAMMA_ADL_ADDR        DEF_IMI_ADL_START_ADDR
#define FIXGAMMA_ADL_ADDR       (DEGAMMA_ADL_ADDR + DEGAMMA_ADL_SIZE)
#define POSTGAMMA_ADL_ADDR      (FIXGAMMA_ADL_ADDR + FIXGAMMA_ADL_SIZE)
#define PANELGAMMA_ADL_ADDR     (POSTGAMMA_ADL_ADDR+PANELGAMMA_ADL_SIZE)
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

