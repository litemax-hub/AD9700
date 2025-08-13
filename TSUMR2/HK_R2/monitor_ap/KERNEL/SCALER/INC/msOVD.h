#ifndef _MSOVD_H_
#define _MSOVD_H_

#include "drvmsOVD.h"

#ifdef _MSOVD_G_
  #define _MSOVDDEC_
#else
  #define _MSOVDDEC_ extern
#endif


typedef enum
{
   _OD_Disable,
   _OD_Enable

}ODStatusType;

typedef enum
{
   _MIU_SSC_OFF,
   _MIU_SSC_40K_ZERO_POINT_FIVE_PERCENT,    //40k, 0.5%
   _MIU_SSC_40K_ONE_PERCENT,                //40K,1%
   _MIU_SSC_40K_TWO_PERCENT,                //40K,2%
   _MIU_SSC_40K_THREE_PERCENT               //40K,3%

}MIUSSCType;

//*************************************************************************************************
//Description: OD look-up table loader.
//
//           ex: msLoadODT(tOverDrive)
//
//           It provides two kinds of format (normal and compressed) for OD look-up table.
//           The look-up table and OD lib should be located in the same bank!!

_MSOVDDEC_ code BYTE tOverDrive[];
#if ENABLE_OD_MTG
_MSOVDDEC_ code BYTE tOverDrive_MultiGain[];
#endif
#if ENABLE_OD_VRR
_MSOVDDEC_ code BYTE tOverDrive_1[]; //fill 0x00 
_MSOVDDEC_ code BYTE tOverDrive_2[]; //fill 0xFF
_MSOVDDEC_ code BYTE tAlphaTBL[];
#endif
#ifdef _MULTI_OD_
_MSOVDDEC_ code BYTE* tOverDrive_AllTbl[];
#endif
_MSOVDDEC_ void msAPI_OverDriveInit(void);
#if (CHIP_ID == CHIP_MT9701)
_MSOVDDEC_ void msAPI_OverDriveTable(const BYTE *pODTbl, ODTBL_INDEX Tblindex);
#else
_MSOVDDEC_ void msAPI_OverDriveTable(void);
#endif
_MSOVDDEC_ void msAPI_OverDriveMultiTable(void);
_MSOVDDEC_ void msAPI_OverDriveEnable(BOOL bEnable);
_MSOVDDEC_ void msAPI_OverDriveAdjustWeighting(BYTE u8Weighting);
_MSOVDDEC_ void msAPI_OverDriveSetActiveWindow(BYTE enable, WORD u16HST,WORD u16HEND,  WORD u16VST, WORD u16VEND);

#if ENABLE_OD_VRR
_MSOVDDEC_ void msAPI_OverDriveVRRAlphaTable_Load(const BYTE *pAlphaTbl);
_MSOVDDEC_ void msAPI_OverDriveVRRForceTable_Enable(BOOL bEnable);
_MSOVDDEC_ void msAPI_OverDriveVRRTable_Select(VRR_TBLSEC_INDEX Tblindex);
_MSOVDDEC_ void msAPI_OverDriveVRRNode_Set(WORD u16VRRNode0, WORD u16VRRNode1, WORD u16VRRNode2);
_MSOVDDEC_ WORD msAPI_OverDriveVRRFrameRate_Get(void);
#if ENABLE_OD_AutoDownTBL
_MSOVDDEC_ void msAPI_OverDriveTable2DRAM_Load(const BYTE *pODTbl, ODTBL_INDEX Tblindex, DWORD OD_ADL_ADDR_START);
_MSOVDDEC_ void msAPI_OverDriveTableADL_Trig(DWORD OD_ADL_ADDR_START);
_MSOVDDEC_ void msAPI_OverDriveVRRAlphaTable2DRAM_Load(const BYTE *pAlphaTbl, DWORD OD_alpha_ADL_ADDR_START);
_MSOVDDEC_ void msAPI_OverDriveVRRAlphaTableADL_Trig(DWORD OD_alpha_ADL_ADDR_START);
#endif
#endif
#if ENABLE_OD_MTG
_MSOVDDEC_ void msAPI_OverDriveMultiGain(WORD u16Vstart, BYTE u8MTGinterval, BYTE *pODMTGTbl);
_MSOVDDEC_ void msAPI_OverDriveMultiGainEnable(Bool bEnable);
_MSOVDDEC_ void msAPI_AdjustOverDriveMultiODGain(BYTE u8MultiGainIdx, BYTE u8ODGain);
#endif

#endif
