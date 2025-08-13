#ifndef _MS_OPEN_HDR_H_
#define _MS_OPEN_HDR_H_
#include "drvHDR.h"
//#if ENABLE_HDR
#if 0
typedef struct
{
    unsigned short smin; // 0.10
    unsigned short smed; // 0.10
    unsigned short smax; // 0.10
    unsigned short tmin; // 0.10
    unsigned short tmed; // 0.10
    unsigned short tmax; // 0.10
    unsigned short slope; // 4.12
    unsigned short rolloff; // 4.12
} HDR_ToneMappingData;
#endif
#define Enable_Dynamic_TMO_Table 1
const DWORD* GetHDRGammaTable(double panelMax,double maxCLL,BOOL bEnableTMO);
const DWORD* GetHDRDeGammaTable(void);

#if Enable_Dynamic_TMO_Table
void HDR_ToneMappingGen_12pt_int(WORD panelMaxLum, WORD clipMaxLum, DWORD *pTbl, BOOL IsUserCustomTMO12P);
#endif

#if (CHIP_ID == CHIP_MT9701)
const DWORD* GetHDROOTFTable(double panelMax);
const WORD* mdrv_HDRLUT_DeGamma_Get(void);
const BYTE* mdrv_HDRLUT_FixGamma_Get(BOOL u8ExtendMode);
const WORD* mdrv_HDRLUT_PostGamma_Get(void);
void mdrv_HDRLUT_PostGamma_Set(WORD *u8Table);
#endif

#endif

