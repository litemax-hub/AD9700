
#ifndef _DRVMSOVD_H_
#define _DRVMSOVD_H_

#ifdef _MSOVD_G_
  #define _MSOVD_G_
#else
  #define _MSOVD_G_ extern
#endif


#define OD_MIU_BusLength   256
#define pg_length   16
#define pg_table_width   16
#define extra_size_factor 1.5
#define pg_entry_per_req 128
#define OD_BusLength 32

#if ENABLE_OD_AutoDownTBL
typedef enum
{
    ADL_MODE_TRIG,//download table one time unless enable bit is set.
    ADL_MODE_ENABLE//download table continuously.
}ADL_MODE;
#endif


typedef enum
{
    OD_TABLE_0,
    OD_TABLE_1,
    OD_TABLE_2
}ODTBL_INDEX;

#if ENABLE_OD_VRR
typedef enum
{
    OD_TABLE0_0 = 1,
    OD_TABLE1_1,
    OD_TABLE0_1,
    OD_TABLE2_2,
    OD_TABLE1_2 = 6
}VRR_TBLSEC_INDEX;
#endif

//extern DWORD g_dwFactory;

//*************************************************************************************************
// Description: OD look-up table loader.
//
// ex: msLoadODT(tOverDrive)
//
// It provides two kinds of format (normal and compressed) for OD look-up table.
// The look-up table and OD lib should be located in the same bank!!


_MSOVD_G_ void drvmsOverDriverInit(void);
_MSOVD_G_ void drvmsLoadODTable( const BYTE *pODTbl, ODTBL_INDEX Tblindex);
_MSOVD_G_ void drvmsOverDriveOnOff(Bool bEnable);
_MSOVD_G_ void drvmsOverDriveAdjustWeighting(BYTE u8Weighting);
_MSOVD_G_ void drvmsOverDriveWindow(BYTE enable, WORD u16HST,WORD u16HEND,  WORD u16VST, WORD u16VEND);
#if ENABLE_OD_AutoDownTBL
_MSOVD_G_ void drvmsSetODAutoDownload(BOOL bEnable, ADL_MODE eMode, DWORD u32Addr, WORD u16Length);
_MSOVD_G_ void drvmsLoadODTable2DRAM(const BYTE *pODTbl, ODTBL_INDEX Tblindex, DWORD OD_ADL_ADDR_START);
_MSOVD_G_ void drvmsTrigODADL(DWORD OD_ADL_ADDR_START);
#if ENABLE_OD_VRR
_MSOVD_G_ void drvmsLoadODAlphaTable2DRAM(const BYTE *pAlphaTbl, DWORD OD_alpha_ADL_ADDR_START);
_MSOVD_G_ void drvmsTrigODAlphaADL(DWORD OD_alpha_ADL_ADDR_START);
#endif
#endif
#if ENABLE_OD_VRR
_MSOVD_G_ void drvmsLoadAlphaTable(const BYTE *pAlphaTbl);
_MSOVD_G_ void drvmsOverDriverVRRInit(void);
_MSOVD_G_ void drvmsOverDriveForceTableEnable(BOOL bEnable);
_MSOVD_G_ void drvmsOverDriveVRRTableSelect(VRR_TBLSEC_INDEX Tblindex);
_MSOVD_G_ void drvmsOverDriveVRRFrameRateNodeSet(WORD u16VRRNode0, WORD u16VRRNode1, WORD u16VRRNode2);
_MSOVD_G_ WORD drvmsOverDriveVRRFrameRateGet(void);
_MSOVD_G_ void drvmsOverDriveVRRFrameRateFLTSet(void);
#endif
#if ENABLE_OD_MTG
_MSOVD_G_ void msLoad_MultiODGain(WORD wVstart, BYTE u8MTGinterval, BYTE *pODMTGTbl);
_MSOVD_G_ void mdrv_SC_MultiODGain_Enable(Bool bEnable);
_MSOVD_G_ void mdrv_SC_MultiODGain_Adjust(BYTE u8MultiGainIdx, BYTE u8ODGain);
#endif
#undef _MSOVD_G_
#endif

