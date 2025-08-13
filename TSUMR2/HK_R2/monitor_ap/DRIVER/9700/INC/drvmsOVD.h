
#ifndef _DRVMSOVD_H_
#define _DRVMSOVD_H_

#ifdef _MSOVD_G_
  #define _MSOVD_G_
#else
  #define _MSOVD_G_ extern
#endif


#define OD_MIU_BusLength   128
#define pg_length   64
#define pg_table_width   16

//extern DWORD g_dwFactory;

//*************************************************************************************************
// Description: OD look-up table loader.
//
// ex: msLoadODT(tOverDrive)
//
// It provides two kinds of format (normal and compressed) for OD look-up table.
// The look-up table and OD lib should be located in the same bank!!


_MSOVD_G_ void drvmsOverDriverInit(void);
_MSOVD_G_ void drvmsLoadODTable( const BYTE *pODTbl);
_MSOVD_G_ void drvmsOverDriveOnOff(Bool bEnable);
_MSOVD_G_ void drvmsOverDriveAdjustWeighting(BYTE u8Weighting);
_MSOVD_G_ void drvmsOverDriveWindow(BYTE enable, WORD u16HST,WORD u16HEND,  WORD u16VST, WORD u16VEND);
#if ENABLE_OD_MTG
_MSOVD_G_ void msLoad_MultiODGain(WORD wVstart, BYTE u8MTGinterval, BYTE *pODMTGTbl);
_MSOVD_G_ void mdrv_SC_MultiODGain_Enable(Bool bEnable);
_MSOVD_G_ void mdrv_SC_MultiODGain_Adjust(BYTE u8MultiGainIdx, BYTE u8ODGain);
#endif

#undef _MSOVD_G_
#endif

