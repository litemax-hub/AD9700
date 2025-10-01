///////////////////////////////////////
// DRVMSTAR_V VERSION: V02
////////////////////////////////////////
#include <string.h>
#include "Global.h"
#include "msEread.h"
#include "menufunc.h"
#include "Detect.h"
#include "drvBDMA.h"
#include "LPLL_Tbl.h"
#if ENABLE_DP_INPUT
#include "mapi_DPRx.h"
#endif
#include "mapi_eDPTx.h"
#include "system_eDPTx.h"

#include "drvOSD.h"
#include "ComboApp.h"
#include "HdcpHandler.h"
#include "msflash.h"
#include "SysInit.h"
#include "Panel.h"
#include "asmCPU.h"

#ifndef _DRVMSTAR_C_
#define _DRVMSTAR_C_

#define drvmStar_DEBUG    1
#if ENABLE_DEBUG&&drvmStar_DEBUG
    #define drvmStar_printData(str, value)   printData(str, value)
    #define drvmStar_printMsg(str)           printMsg(str)
#else
    #define drvmStar_printData(str, value)
    #define drvmStar_printMsg(str)
#endif

ST_PANEL_INFO g_sPnlInfo;
ST_PANEL_INFO g_sPnlInfoInit;

#if ENABLE_AUTOEQ
    Bool xdata AUTOEQ_FLAG = 0;
#endif

#if ENABLE_SUPER_RESOLUTION
#define PANEL_SHIFT_DE_V    2
#else
#define PANEL_SHIFT_DE_V    0
#endif

BYTE g_u8KeyUserID[COMBO_HDCP2_USER_ID_SIZE] = {0};

BYTE  u8LPLL_LOOP_GAIN = 0;
float fLPLL_LOOP_DIV = 1.0;

#define ENABLE_NOISE_DITHER_PAFRC    1

void APPInput_HdcpKeyProcess(void);

code RegUnitType2 tblInitMODLvdsCh1[]=
{
///////////MOD Initial Settings///////////
    //PANEL_LVDS_1CH
    //------- MOD ATOP setting for LVDS, st --------// ,},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD2_0C ,0x0000},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD1_5C ,0x0000},
    {REG_MOD2_10 ,0x0000},
    {REG_MOD2_12 ,0x0000},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP setting for LVDS, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_82 ,0xC000},
    {REG_CLKGEN_86 ,0x0000},
    {REG_CLKGEN_88 ,0x0000},
    {REG_CLKGEN_8A ,0x0000},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- MFT seetting, st --------// ,},
    {REG_MFT_40  ,0x0000},
    //------- MFT setting, ed --------// ,},
    //------- Free Swap, st --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x0000},
    {REG_MOD2_20 ,0x0100},
    {REG_MOD2_22 ,0x0302},
    {REG_MOD2_24 ,0x0004},
    {REG_MOD2_26 ,0x0201},
    {REG_MOD2_28 ,0x0403},
    //------- Free Swap, ed --------// ,},
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0001},
    {REG_MOD1_0A ,0x6008},
    {REG_MOD1_1E ,0xE800},
    {REG_MOD1_60 ,0x0000},
    {REG_MOD1_62 ,0x0000},
    {REG_MOD1_78 ,0x0000},
    //------- MOD_D settingng, ed --------// ,},
    //------- Output channel Setting, st --------// ,},
    {REG_MOD2_80 ,0x00FF},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODLvdsCh2[]=
{
    //PANEL_LVDS_2CH
    //------- MOD ATOP setting for LVDS, st --------// ,},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD2_0C ,0x0000},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD1_5C ,0x0000},
    {REG_MOD2_10 ,0x0000},
    {REG_MOD2_12 ,0x0000},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP setting for LVDS, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_82 ,0xC000},
    {REG_CLKGEN_86 ,0x0000},
    {REG_CLKGEN_88 ,0x0001},
    {REG_CLKGEN_8A ,0x0000},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- MFT setting, st --------// ,},
    {REG_MFT_40  ,0x0000},
    //------- MFT setting, ed --------// ,},
    //------- Free Swap, ST --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x0000},
    {REG_MOD2_20 ,0x0100},
    {REG_MOD2_22 ,0x0302},
    {REG_MOD2_24 ,0x0604},
    {REG_MOD2_26 ,0x0807},
    {REG_MOD2_28 ,0x0A09},
    //------- Free Swap, Ed --------// ,},
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0001},
    {REG_MOD1_0A ,0x6008},
    {REG_MOD1_1E ,0xE400},
    {REG_MOD1_60 ,0x0000},
    {REG_MOD1_62 ,0x0000},
    //------- MOD_D setting, ed --------// ,},
    //-------Output channel setting, st --------// ,},
    {REG_MOD2_80 ,0x00ff},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODVby1Ch2Bit10[]=
{
    //PANEL_VBY1_2CH_10Bit
    //------- MOD ATOP Setting for VBY1, st --------// ,4byte_1blk},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD2_10 ,0x3FFF},
    {REG_MOD2_82 ,0x4444},
    {REG_MOD2_84 ,0x4444},
    {REG_MOD2_86 ,0x0044},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP Setting for VBY1, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_6C ,0x0000},
    {REG_CLKGEN_82 ,0xC000},
    {REG_CLKGEN_86 ,0x4000},
    {REG_CLKGEN_88 ,0x0000},
    {REG_CLKGEN_8A ,0x0010},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- HPD/LCK GPI, st --------// ,},
    {REG_MOD1_CA ,0x0000},
    {REG_MOD1_CC ,0x0000},
    {REG_MOD1_CE ,0x0000},
    {REG_MOD1_70 ,0x0000},
    {REG_MOD1_30 ,0x0000},
    {REG_MOD1_32 ,0x0000},
    {REG_MOD1_34 ,0x0000},
    {REG_MOD1_36 ,0x0000},
    {REG_MOD1_28 ,0xFFFF},
    {REG_MOD1_2A ,0xFFFF},
    {REG_MOD1_2C ,0xFFFF},
    {REG_MOD1_1A ,0x0000},
    //------- HPD/LCK GPI, ed --------// ,},
    //-------  MFT setting, st --------// ,},
    {REG_MFT_40 ,0x0005}, // DIV1
    {REG_MFT_36 ,0x00A0},
    {REG_MFT_4E ,0x0100},
    {REG_MFT_56 ,0x0002},
    {REG_MFT_58 ,0x0045},
    //-------  MFT setting, ed --------// ,},
    //------- Free Swap, ST --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x0000},
    //------- Free Swap, Ed --------// ,},
    //------- Pair free swap, st ---// ,}
    {REG_MOD2_20 ,0x0000},
    {REG_MOD2_22 ,0x0001},
    {REG_MOD2_24 ,0x0000},
    {REG_MOD2_26 ,0x0000},
    {REG_MOD2_28 ,0x0000},
    //------- Pair free swap, ed ---// ,}
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0001},
    {REG_MOD1_0A ,0x2000},
    {REG_MOD1_1E ,0xE400},
    {REG_MOD1_60 ,0x5000},
    {REG_MOD1_62 ,0x0000},
    //------- MOD_D setting, ed --------// ,},
    //------- VBY1 DIG setting, st --------// ,},
    {REG_MOD1_C8 ,0x1003},
    {REG_MOD1_C2 ,0x8F3F},
    {REG_MOD1_D0 ,0x0080},
    {REG_MOD1_C4 ,0x2000},
    {REG_MOD1_C0 ,0x0AAE},
    //------- VBY1 DIG setting, ed --------// ,},
    //------- Output channel setting, st --------// ,},
    {REG_MOD2_80 ,0x00FF},
    {REG_MOD1_20 ,0x0014},
    {REG_MOD1_22 ,0x0000},
    {REG_MOD1_24 ,0x0000},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODVby1Ch4Bit10[]=
{
    //PANEL_VBY1_4CH_10Bit
    //------- MOD ATOP Setting for VBY1, st --------// ,4byte_1blk},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD2_10 ,0x3FFF},
    {REG_MOD2_82 ,0x4444},
    {REG_MOD2_84 ,0x4444},
    {REG_MOD2_86 ,0x0044},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP Setting for VBY1, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_6C ,0x0000},
    {REG_CLKGEN_82 ,0xC000},
    {REG_CLKGEN_86 ,0x0000},
    {REG_CLKGEN_88 ,0x0000},
    {REG_CLKGEN_8A ,0x0000},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- HPD/LCK GPI, st --------// ,},
    {REG_MOD1_CA ,0x0000},
    {REG_MOD1_CC ,0x0000},
    {REG_MOD1_CE ,0x0000},
    {REG_MOD1_70 ,0x0000},
    {REG_MOD1_30 ,0x0000},
    {REG_MOD1_32 ,0x0000},
    {REG_MOD1_34 ,0x0000},
    {REG_MOD1_36 ,0x0000},
    {REG_MOD1_28 ,0xFFFF},
    {REG_MOD1_2A ,0xFFFF},
    {REG_MOD1_2C ,0xFFFF},
    {REG_MOD1_1A ,0x0000},
    //------- HPD/LCK GPI, ed --------// ,},
    //-------  MFT setting, st --------// ,},
    {REG_MFT_40 ,0x0005}, // DIV1
    {REG_MFT_36 ,0x00A0},
    {REG_MFT_4E ,0x0300},
    {REG_MFT_56 ,0x0002},
    {REG_MFT_58 ,0x0022},
    //-------  MFT setting, ed --------// ,},
    //------- Free Swap, ST --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x0000},
    //------- Free Swap, Ed --------// ,},
    //------- Pair free swap, st ---// ,}
    {REG_MOD2_20 ,0x0000},
    {REG_MOD2_22 ,0x0201},
    {REG_MOD2_24 ,0x0003},
    {REG_MOD2_26 ,0x0000},
    {REG_MOD2_28 ,0x0000},
    //------- Pair free swap, ed ---// ,}
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0201},
    {REG_MOD1_0A ,0x2000},
    {REG_MOD1_1E ,0xE400},
    {REG_MOD1_60 ,0x5000},
    {REG_MOD1_62 ,0x0000},
    //------- MOD_D setting, ed --------// ,},
    //------- VBY1 DIG setting, st --------// ,},
    {REG_MOD1_C8 ,0x1003},
    {REG_MOD1_C2 ,0x8F3F},
    {REG_MOD1_D0 ,0x0080},
    {REG_MOD1_C4 ,0x2000},
    {REG_MOD1_C0 ,0x0AAE},
    //------- VBY1 DIG setting, ed --------// ,},
    //------- Output channel setting, st --------// ,},
    {REG_MOD2_80 ,0x00FF},
    {REG_MOD1_20 ,0x0154},
    {REG_MOD1_22 ,0x0000},
    {REG_MOD1_24 ,0x0000},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODVby1Ch8Bit10[]=
{
    //PANEL_VBY1_8CH_10Bit
    //------- MOD ATOP Setting for VBY1, st --------// ,4byte_1blk},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD2_10 ,0x3FFF},
    {REG_MOD2_82 ,0x4444},
    {REG_MOD2_84 ,0x4444},
    {REG_MOD2_86 ,0x0044},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP Setting for VBY1, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_6C ,0x0000},
    {REG_CLKGEN_86 ,0x0000},
    {REG_CLKGEN_88 ,0x0001},
    {REG_CLKGEN_8A ,0x0000},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- HPD/LCK GPI, st --------// ,},
    {REG_MOD1_CA ,0x0000},
    {REG_MOD1_CC ,0x0000},
    {REG_MOD1_CE ,0x0000},
    {REG_MOD1_70 ,0x0000},
    {REG_MOD1_30 ,0x0000},
    {REG_MOD1_32 ,0x0000},
    {REG_MOD1_34 ,0x0000},
    {REG_MOD1_36 ,0x0000},
    {REG_MOD1_28 ,0xFFFF},
    {REG_MOD1_2A ,0xFFFF},
    {REG_MOD1_2C ,0xFFFF},
    {REG_MOD1_1A ,0x0000},
    //------- HPD/LCK GPI, ed --------// ,},
    //-------  MFT setting, st --------// ,},
    {REG_MFT_40 ,0x0005}, // DIV1
    {REG_MFT_36 ,0x00A0},
    {REG_MFT_4E ,0x0700},
    {REG_MFT_56 ,0x0002},
    {REG_MFT_58 ,0x0010},
    //-------  MFT setting, ed --------// ,},
    //------- Free Swap, ST --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x7564},
    //------- Free Swap, Ed --------// ,},
    //------- Pair free swap, st ---// ,}
    {REG_MOD2_20 ,0x0000},
    {REG_MOD2_22 ,0x0201},
    {REG_MOD2_24 ,0x0403},
    {REG_MOD2_26 ,0x0605},
    {REG_MOD2_28 ,0x0700},
    //------- Pair free swap, ed ---// ,}
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0601},
    {REG_MOD1_0A ,0x2000},
    {REG_MOD1_1E ,0xE400},
    {REG_MOD1_60 ,0x5000},
    {REG_MOD1_62 ,0x0000},
    //------- MOD_D setting, ed --------// ,},
    //------- VBY1 DIG setting, st --------// ,},
    {REG_MOD1_C8 ,0x1003},
    {REG_MOD1_C2 ,0x8F3F},
    {REG_MOD1_D0 ,0x0080},
    {REG_MOD1_C4 ,0x2000},
    {REG_MOD1_C0 ,0x0AAE},
    //------- VBY1 DIG setting, ed --------// ,},
    //------- Output channel setting, st --------// ,},
    {REG_MOD2_80 ,0x00FF},
    {REG_MOD1_20 ,0x5554},
    {REG_MOD1_22 ,0x0004},
    {REG_MOD1_24 ,0x0000},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODVby1Ch2Bit8[]=
{
    // PANEL_VBY1_2CH_8Bit
    //------- MOD ATOP Setting for VBY1, st --------// ,4byte_1blk},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD2_10 ,0x3FFF},
    {REG_MOD2_82 ,0x4444},
    {REG_MOD2_84 ,0x4444},
    {REG_MOD2_86 ,0x0044},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP Setting for VBY1, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_6C ,0x0000},
    {REG_CLKGEN_82 ,0xC000},
    {REG_CLKGEN_86 ,0x4000},
    {REG_CLKGEN_88 ,0x0203},
    {REG_CLKGEN_8A ,0x0010},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- HPD/LCK GPI, st --------// ,},
    {REG_MOD1_CA ,0x0000},
    {REG_MOD1_CC ,0x0000},
    {REG_MOD1_CE ,0x0000},
    {REG_MOD1_70 ,0x0000},
    {REG_MOD1_30 ,0x0000},
    {REG_MOD1_32 ,0x0000},
    {REG_MOD1_34 ,0x0000},
    {REG_MOD1_36 ,0x0000},
    {REG_MOD1_28 ,0xFFFF},
    {REG_MOD1_2A ,0xFFFF},
    {REG_MOD1_2C ,0xFFFF},
    {REG_MOD1_1A ,0x0000},
    //------- HPD/LCK GPI, ed --------// ,},
    //-------  MFT setting, st --------// ,},
    {REG_MFT_40 ,0x0005}, // DIV1
    {REG_MFT_36 ,0x00A0},
    {REG_MFT_4E ,0x0100},
    {REG_MFT_56 ,0x0002},
    {REG_MFT_58 ,0x0045},
    //-------  MFT setting, ed --------// ,},
    //------- Free Swap, ST --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x0000},
    //------- Free Swap, Ed --------// ,},
    //------- Pair free swap, st ---// ,}
    {REG_MOD2_20 ,0x0000},
    {REG_MOD2_22 ,0x0001},
    {REG_MOD2_24 ,0x0000},
    {REG_MOD2_26 ,0x0000},
    {REG_MOD2_28 ,0x0000},
    //------- Pair free swap, ed ---// ,}
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0001},
    {REG_MOD1_0A ,0x2000},
    {REG_MOD1_1E ,0xE400},
    {REG_MOD1_60 ,0x5000},
    {REG_MOD1_62 ,0x0000},
    //------- MOD_D setting, ed --------// ,},
    //------- VBY1 DIG setting, st --------// ,},
    {REG_MOD1_C8 ,0x1003},
    {REG_MOD1_C2 ,0x8F3F},
    {REG_MOD1_D0 ,0x0080},
    {REG_MOD1_C4 ,0x3000},
    {REG_MOD1_C0 ,0x0AAE},
    //------- VBY1 DIG setting, ed --------// ,},
    //------- Output channel setting, st --------// ,},
    {REG_MOD2_80 ,0x00FF},
    {REG_MOD1_20 ,0x0014},
    {REG_MOD1_22 ,0x0000},
    {REG_MOD1_24 ,0x0000},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODVby1Ch4Bit8[]=
{
    //PANEL_VBY1_4CH_8Bit
    //------- MOD ATOP Setting for VBY1, st --------// ,4byte_1blk},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD2_10 ,0x3FFF},
    {REG_MOD2_82 ,0x4444},
    {REG_MOD2_84 ,0x4444},
    {REG_MOD2_86 ,0x0044},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP Setting for VBY1, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_6C ,0x0000},
    {REG_CLKGEN_82 ,0xC000},
    {REG_CLKGEN_86 ,0x0000},
    {REG_CLKGEN_88 ,0x0203},
    {REG_CLKGEN_8A ,0x0000},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- HPD/LCK GPI, st --------// ,},
    {REG_MOD1_CA ,0x0000},
    {REG_MOD1_CC ,0x0000},
    {REG_MOD1_CE ,0x0000},
    {REG_MOD1_70 ,0x0000},
    {REG_MOD1_30 ,0x0000},
    {REG_MOD1_32 ,0x0000},
    {REG_MOD1_34 ,0x0000},
    {REG_MOD1_36 ,0x0000},
    {REG_MOD1_28 ,0xFFFF},
    {REG_MOD1_2A ,0xFFFF},
    {REG_MOD1_2C ,0xFFFF},
    {REG_MOD1_1A ,0x0000},
    //------- HPD/LCK GPI, ed --------// ,},
    //-------  MFT setting, st --------// ,},
    {REG_MFT_40 ,0x0005}, // DIV1
    {REG_MFT_36 ,0x00A0},
    {REG_MFT_4E ,0x0300},
    {REG_MFT_56 ,0x0002},
    {REG_MFT_58 ,0x0022},
    //-------  MFT setting, ed --------// ,},
    //------- Free Swap, ST --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x0000},
    //------- Free Swap, Ed --------// ,},
    //------- Pair free swap, st ---// ,}
    {REG_MOD2_20 ,0x0000},
    {REG_MOD2_22 ,0x0201},
    {REG_MOD2_24 ,0x0003},
    {REG_MOD2_26 ,0x0000},
    {REG_MOD2_28 ,0x0000},
    //------- Pair free swap, ed ---// ,}
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0201},
    {REG_MOD1_0A ,0x2000},
    {REG_MOD1_1E ,0xE400},
    {REG_MOD1_60 ,0x5000},
    {REG_MOD1_62 ,0x0000},
    //------- MOD_D setting, ed --------// ,},
    //------- VBY1 DIG setting, st --------// ,},
    {REG_MOD1_C8 ,0x1003},
    {REG_MOD1_C2 ,0x8F3F},
    {REG_MOD1_D0 ,0x0080},
    {REG_MOD1_C4 ,0x3000},
    {REG_MOD1_C0 ,0x0AAE},
    //------- VBY1 DIG setting, ed --------// ,},
    //------- Output channel setting, st --------// ,},
    {REG_MOD2_80 ,0x00FF},
    {REG_MOD1_20 ,0x0154},
    {REG_MOD1_22 ,0x0000},
    {REG_MOD1_24 ,0x0000},
    {REG_MOD1_46 ,0xFFFE},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODVby1Ch8Bit8[]=
{
    //PANEL_VBY1_8CH_8Bit
    //------- MOD ATOP Setting for VBY1, st --------// ,4byte_1blk},
    {REG_MOD1_50 ,0x0000},
    {REG_MOD1_4E ,0x007E},
    {REG_MOD1_52 ,0x0000},
    {REG_MOD1_54 ,0x0000},
    {REG_MOD2_10 ,0x3FFF},
    {REG_MOD2_82 ,0x4444},
    {REG_MOD2_84 ,0x4444},
    {REG_MOD2_86 ,0x0044},
    {REG_MOD1_84 ,0x0000},
    {REG_MOD1_86 ,0x0000},
    //------- MOD ATOP Setting for VBY1, ed --------// ,},
    //------- Clock tree setting, st --------// ,},
    {REG_CLKGEN_46 ,0x0100},
    {REG_CLKGEN_6C ,0x0000},
    {REG_CLKGEN_86 ,0x0000},
    {REG_CLKGEN_88 ,0x0207},
    {REG_CLKGEN_8A ,0x0000},
    {REG_MOD1_A2 ,0x0000},
    //------- Clock tree setting, ed --------// ,},
    //------- HPD/LCK GPI, st --------// ,},
    {REG_MOD1_CA ,0x0000},
    {REG_MOD1_CC ,0x0000},
    {REG_MOD1_CE ,0x0000},
    {REG_MOD1_70 ,0x0000},
    {REG_MOD1_30 ,0x0000},
    {REG_MOD1_32 ,0x0000},
    {REG_MOD1_34 ,0x0000},
    {REG_MOD1_36 ,0x0000},
    {REG_MOD1_28 ,0xFFFF},
    {REG_MOD1_2A ,0xFFFF},
    {REG_MOD1_2C ,0xFFFF},
    {REG_MOD1_1A ,0x0000},
    //------- HPD/LCK GPI, ed --------// ,},
    //-------  MFT setting, st --------// ,},
    {REG_MFT_40 ,0x0005}, // DIV1
    {REG_MFT_36 ,0x00A0},
    {REG_MFT_4E ,0x0700},
    {REG_MFT_56 ,0x0002},
    {REG_MFT_58 ,0x0010},
    //-------  MFT setting, ed --------// ,},
    //------- Free Swap, ST --------// ,},
    {REG_MOD2_02 ,0x3120},
    {REG_MOD2_04 ,0x7564},
    //------- Free Swap, Ed --------// ,},
    //------- Pair free swap, st ---// ,}
    {REG_MOD2_20 ,0x0000},
    {REG_MOD2_22 ,0x0201},
    {REG_MOD2_24 ,0x0403},
    {REG_MOD2_26 ,0x0605},
    {REG_MOD2_28 ,0x0700},
    //------- Pair free swap, ed ---// ,}
    //------- MOD_D setting, st --------// ,},
    {REG_MOD1_00 ,0x0601},
    {REG_MOD1_0A ,0x2000},
    {REG_MOD1_1E ,0xE400},
    {REG_MOD1_60 ,0x5000},
    {REG_MOD1_62 ,0x0000},
    //------- MOD_D setting, ed --------// ,},
    //------- VBY1 DIG setting, st --------// ,},
    {REG_MOD1_C8 ,0x1003},
    {REG_MOD1_C2 ,0x8F3F},
    {REG_MOD1_D0 ,0x0080},
    {REG_MOD1_C4 ,0x3000},
    {REG_MOD1_C0 ,0x0AAE},
    //------- VBY1 DIG setting, ed --------// ,},
    //------- Output channel setting, st --------// ,},
    {REG_MOD2_80 ,0x00FF},
    {REG_MOD1_20 ,0x5554},
    {REG_MOD1_22 ,0x0004},
    {REG_MOD1_24 ,0x0000},
    {REG_MOD1_46 ,0xFFFE},
    //------- Output channel setting, ed --------// ,},
};

code RegUnitType2 tblInitMODEdp[]=
{
    //PANEL_EDP
    {REG_120F0E, 0x0800},
    /*
    {REG_MOD1_20, 0x0000},
    //{REG_MOD1_22, 0x0000},
    {REG_MOD1_24, 0x0000},
    {REG_MOD1_26, 0x0000},
    {REG_MOD1_4E, 0x00F8},
    {REG_MOD1_50, 0x8001},
    {REG_MOD1_52, 0xFF00},
    {REG_MOD1_F2, 0x8142},
    {REG_MOD1_B2, 0x000F},
    {REG_MOD1_78, 0x0000},
    {REG_MOD1_60, 0xD000},
    {REG_MOD2_14, 0x0003},
    {REG_MOD2_80, 0x01FF},
    {REG_MOD2_82, 0x0000},
    {REG_MOD2_84, 0x0000},
    {REG_MOD2_86, 0x0000},
    {REG_MOD1_A0, 0x0000},
    {REG_MOD1_A2, 0x0000},
    */
};

code RegTbl2Type tblInitMOD2[]=
{
    //MOD1
    {REG_MOD1_F3, 0 , BIT6}, // toggle reg_sw_force_fix_trig
    {REG_MOD1_F3, BIT6 , BIT6},
    {REG_MOD1_F3, 0 , BIT6},
};

code RegTblType tPowerOnOffTbl[]=
{
    {REG_1ED1, 0x00, BIT0},
    {REG_1EDC, 0x00, BIT5|BIT4},
    {REG_3041, 0x00, 0xFF},
    {REG_30F0, 0x00, BIT0},
    {REG_30FA, 0x00, BIT7},
    {REG_30EE, 0x07, 0x07},
    {REG_1100, 0x00, BIT5|BIT4|BIT3},
    {REG_1101, 0x00, BIT7},
    {REG_1133, 0x00, BIT7},
    {REG_3806, 0x00, BIT5},
    {REG_3A60, 0x00, BIT6},
    //{REG_05F0, 0x00, BIT3},// [3]: AULL power down
#if 1    //  clock gating
    {REG_1E29, 0x00, BIT0},
    {REG_1E35, 0x00, BIT0},
    {REG_1E37, 0x00, BIT0},
    {REG_1E3E, 0x00, BIT0},
    {REG_1E3F, 0x00, BIT0},
    {REG_1E2E, BIT6|BIT4|BIT2|BIT0, BIT6|BIT4|BIT2|BIT0},
    {REG_1E30, 0x00, BIT4|BIT0},
    {REG_1E25, 0x00, BIT2},
    {REG_1E28, 0x00, BIT0},
    {REG_1E24, 0x00, BIT4|BIT0},
    {REG_1E2C, 0x00, BIT4|BIT0},
    {REG_1E2A, BIT0, BIT0},
    {REG_1E47, 0x00, BIT0},
#endif
    {REG_01C7, 0x00, 0xFF},  //PHDAC on

};

#if 0 // no use
code RegTbl2Type tComboTMDSMuxTbl[]=
{
#if ENABLE_AUDIO_AUTO_MUTE
    {REG_1428, 0xAF00, 0xFF00},
#endif
    {REG_1600, 0x000F, 0xFFFF},
    {REG_1602, 0x020A, 0xFFFF},
    {REG_1606, BIT11 |(WORD)(HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV2)), 0xFFFF},
    {REG_1608, BIT11 |(WORD)(HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV4)), 0xFFFF},
    {REG_160A, BIT11 |(WORD)(HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV8)), 0xFFFF},
    {REG_1612, 0x0D11, 0xFFFF},
    {REG_161C, 0xFF00, 0xFFFF},
    {REG_1620, 0xFF00, 0xFFFF},
    {REG_1624, 0xFF00, 0xFFFF},
    {REG_1628, 0xFF00, 0xFFFF},
    {REG_1646, 0x081F, 0xFFFF},
    {REG_165C, 0x000D, 0xFFFF},

    {REG_1700, 0x7045, 0xFFFF},
    {REG_1706, 0x1001, 0xFFFF},
    {REG_170F, 0x0000, 0x0008},
    {REG_1710, 0x8000, 0xFFFF},
    {REG_1714, 0x3130, 0xFFFF},
    {REG_1730, 0x0081, 0xFFFF},
    {REG_1752, 0x0000, 0xFFFF},
    {REG_175A, 0x0000, 0xFFFF},
    {REG_175C, 0xDFFF, 0xFFFF},
    {REG_17A2, 0x800E, 0xFFFF},
    //{REG_17AE, 0x0080, 0xFFFF},
    {REG_17AE, 0x0F80, 0xFFFF}, // 20130201 - follow RD new IP driver table
    {REG_17B0, ((HDMI_B_CHANNEL_EQ <<8) |HDMI_CLK_CHANNEL_EQ), 0xFFFF},
    {REG_17B2, ((HDMI_R_CHANNEL_EQ <<8) |HDMI_G_CHANNEL_EQ), 0xFFFF},
    {REG_17BC, 0x0040 |HDMI_PLL_LDO_VALUE, 0xFFFF},   // [6]: for all mode,   VCO switch cap always on, add PLL LDO 5% setting
    {REG_1782, 0x8101, 0xFFFF},
    {REG_17A8, 0x001B, 0xFFFF},
    {REG_17AA, 0x0000, 0x6000},   // Disable overwrite [D]: Phase DAC DIV select, [E]: PLL DIV select
    //{REG_17AC, 0x8FE0, 0xFFFF},
    {REG_17AC, 0x87E0, 0xFFFF}, // 20130201 - follow RD new IP driver table
    {REG_1740, 0x0001, 0xFFFF},
    {REG_17B4, 0x0010, 0xFFFF},   // [4]: for all mode,  phase dac vring LDO +5%
    {REG_174C, 0xFFFF, 0xFFFF},
    {REG_174E, 0x3FFF, 0xFFFF},
    {REG_17BE, 0x0000, 0xFFFF},

    {REG_01C2, 0x0104, 0xFF1F},    // port mux couldn't be set in the initial table
    {REG_01C6, 0x111F, 0xFFFF},
    {REG_01CA, 0x10F8, 0xFFFF},
    {REG_01CC, 0x0000, 0xFFFF},
    {REG_01CE, 0x0020, 0xFFF8},
    {REG_0C01, 0x0000, 0x0300},
    {REG_1506, 0x0000, 0x0004},
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
    {REG_1508, 0x000C, 0x840C}, //km INT
    {REG_153C, 0x0002, 0x0002}, //km INT
#endif // #if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
#if ENABLE_DP_INPUT
    {REG_18E0, 0x0000, 0x0006}, //disable FT
#endif

    {REG_1542, 0, 0x1FFF}, // REG_COMBO_HDCP_21[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch0 phase release
    {REG_1544, 0, 0x1FFF}, // REG_COMBO_HDCP_22[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch1 phase release
    {REG_1546, 0, 0x1FFF}, // REG_COMBO_HDCP_23[12]: phase overwrite bit, [11:0]: TMDS ch0 phase code, TMDS ch2 phase release
    {REG_1604, 0, BIT3| BIT2| BIT1}, // REG_COMBO_DECODER_02[3:1]: swap for MHL v1.2, [2]: 0:SW mode, [1]: packed-pixel mode enable
    {REG_17AA, 0, BIT15| BIT1| BIT0}, // REG_COMBO_ATOP_55[15]: MHL mode DIV selection overwrite, [1:0]: MHL mode DIV select for override control.
};
#endif

void msWritePowerOnTble(void)
{
    BYTE i;
    for(i=0;i<sizeof(tPowerOnOffTbl)/sizeof(RegTblType);i++)
        old_msWriteByteMask(tPowerOnOffTbl[i].wReg,tPowerOnOffTbl[i].ucValue,tPowerOnOffTbl[i].ucMask );
}



code RegUnitType tblInit[]=
{
#ifdef TSUMR2_FPGA
    {SC00_04, 00}, // [4]: Input HSYNC reference edge select, [3]:Input VSYNC reference edge select
#endif
    //=========================================================================
#if 1
    // display timing OP2-1
    // output DE size // 0x10~0x1D
    {SC00_10, 0 + PANEL_SHIFT_DE_V}, // Vert. DE start
    {SC00_11, 0}, // Vert. DE start

    {SC00_27, BIT6 /*| BIT5 */| BIT1}, // enable auto-htotal

    //{SC0_28, BIT5}, // setup clock mode
    {SC00_49, 0xAF},
    {SC05_3E, 0x80},
    {SC05_3F, 0x0B}, // Line buffer size set to 9.2 lines (9.2->0xB80)
#endif

#if CHIP_ID == CHIP_MT9700
    {SC15_E7, 0x40}, //[7]=0, default set OD to old mode w/o tgen
#endif

#if PanelDither==6
	#if ENABLE_NOISE_DITHER_PAFRC
   		{SC24_7E, BIT4|BIT2|BIT0},
   	#else
		{SC24_7E, BIT4|BIT3|BIT2|BIT0},
	#endif
#elif PanelDither==8
	#if ENABLE_NOISE_DITHER_PAFRC
   		{SC24_7E, BIT4|BIT0},
   	#else
		{SC24_7E, BIT4|BIT3|BIT0},
	#endif
#else
   {SC24_7E,  0x00},	
#endif
    {SC24_7F, 0x00},
    {SC24_80, 0x23},
    {SC24_81, 0x20},
    {SC24_82, 0xf3},
    {SC24_83, 0x3C},
    {SC24_84, 0xC9},
    {SC24_85, 0x9C},
    {SC24_86, 0xC9},
    {SC24_87, 0x9C},
    {SC24_88, 0xAA},
    {SC24_89, 0xAA},
    {SC24_8A, 0x50},
    {SC24_8B, 0x22},
    {SC24_8C, 0xD8},
    {SC24_8D, 0xD8},
    {SC24_8E, 0x72},
    {SC24_8F, 0x72},
    {SC24_90, 0x8D},
    {SC24_91, 0x8D},
    {SC24_92, 0x27},
    {SC24_93, 0xD8},
    {SC24_94, 0x72},
    {SC24_95, 0x8D},
    
#if 0 //TBD
    {REG_3041, 0x00},
    {REG_3080, (PanelSwapMLSB&BIT6)|(BIT5&PANEL_SWAP_PN)|(BIT2&LVDS_TIMode)|BIT3},
    {REG_308A, 0x3F},
    {REG_3093, (PanelSwapOddRB&BIT3)|(PanelSwapEvenRB&BIT5)|(PanelSwapOddML&BIT4)|(PanelSwapEvenML&BIT6)},

#if (PanelDither==6 && (PanelSwapEvenML||PanelSwapOddML))
    {REG_3092, 0x80},       //[7:6]=2'b10 for 6bit panel   need set this for PanelSwapEvenML(REG_3093[6][4])
#elif (PanelDither==8 && (PanelSwapEvenML||PanelSwapOddML))
    {REG_3092, 0x40},       //[7:6]=2'b01 for 8bit panel
#elif (PanelDither==10 && (PanelSwapEvenML||PanelSwapOddML))
    {REG_3092, 0xC0},       //[7:6]=others for 10bit panel
#else
    {REG_3092, 0x00},       //[7:6]=others for 10bit panel
#endif

    {REG_3081, (PanelDClkDelay<<4)}, // DE delay, clock delay for TTL output
    {REG_3094, (PanelInvDClk&BIT4)|(PanelInvDE&BIT2)|(PanelInvVSync&BIT3)
               |(PanelDualPort&BIT1)|(PanelSwapPort&BIT0)},

    //{REG_3089, PanelDualPort&BIT4},
    {REG_3095, (PanelInvHSync&BIT4)},
    {REG_3096, LVDS_TI_BitModeSel},
    //{REG_3092, 0x00},
    //{REG_30DA, 0x55},
    //{REG_30DB, 0x55},
    //{REG_30DC, 0x05},
    {REG_30EE, 0x07},
    {REG_30F0, BIT0},


#endif

    {REG_LPLL_20, 0x2A},
    {REG_LPLL_0A, 0xFF},
    {REG_LPLL_0B, 0xFF},
    {REG_LPLL_0E, 0x02},
    {REG_LPLL_0D, 0x00},
    {REG_LPLL_12, 0x02},
    {REG_LPLL_13, 0x00},
    {REG_LPLL_14, 0x00},
    {REG_LPLL_15, 0xF0},
    {REG_LPLL_16, 0x43},
    {REG_LPLL_17, 0x43},

    {SC00_A3, 0x80}, //BK0_A3[7] prevent input v cnt overflow
    {SC00_6C, 0x90}, // short line tune coef. // 120928 coding, modified default value 0x70 to 0x90
    {SC00_5F, 0x03},
    // Clock generator control 0xD0~0xDB
    //{SC0_D0, 0x00},
    //{REG_30AA, 0x00}, // BK0_D0[0] Rose <--> REG_30AA[0] CHIP_TSUML
    //{REG_1ED1, BIT7}, // setup output pll & master pll  //// 20071219 RD Anson suggest for B1/B2/Goya enable Bit7 fixed line clock in 172Mhz
    //{REG_1ED1, BIT2}, //Jison110311 move to mcuSetSystemSpeed()
    // Master PLL
    //{MPLL_M, 0x6F},//0x72:12MHz // set master pll as 215 MHz & drive current
    //{REG_1ED2, 0x0F}, //Jison110311 move to mcuSetSystemSpeed()
#if 0 //TBD
    {REG_1ED3, 0x40},
    #if PanelLVDS
    {REG_1ED4, 0x03},
    #else
    {REG_1ED4, 0x03},
    #endif

    // Spectrum control
#if !defined(_SSC_BY_OUTPUT_DCLK_)  // the LPLL related settings are located in mStar_SetPanelTiming().
    {REG_382E, LOBYTE(SSC_STEP)},
    {REG_382F, HIBYTE(SSC_STEP)}, //bit8~bit4 have data, TSUMT should use _SSC_BY_OUTPUT_DCLK_
    {REG_3830, LOBYTE(SSC_SPAN)},
    {REG_3831, HIBYTE(SSC_SPAN)},
    {REG_381B, LPLL_SSC|0x03},
#endif
#endif
	//repeat gamma table max value for interpolation setting
	{SC0F_AF, 0x10},

     //Histogram V initial setting
    {SC03_B5, 0x01},

     //MRW Init Settings
    {SC06_01, 0x00}, //can be deleted
    {SC06_02, 0x14}, //[2]IPM CLK [4]DB_EN
    {SC06_03, 0x00},

    {SC06_1A, 0x02}, //OPM CLK
    {SC06_1B, 0x00}, //can be deleted


    {SC06_06 ,0x40},    //[7:0]write  MIU unit of N-data per time ,initial only.
    {SC06_07 ,0x40},    //[7:0]IPM Write high priority thrd
    {SC06_08 ,0x40},    //[7:0]Read MIU unit of N-data per time ,initial only.
    {SC06_09 ,0x40},    //[7:0]IPM Read high priority thrd
    {SC06_0A ,0x40},
    {SC06_0B ,0x40},    //reg_ipr_rreq_hpri

    {SC06_0C ,0x28},    //reg_ipm_arb_lvl_thrd
    {SC06_0D ,0x28},    //reg_ipm_arb_req_length
    {SC06_0E ,0xC8},    //reg_opm_arb_lvl_thrd
    {SC06_0F ,0x28},    //reg_opm_arb_req_length

    {SC06_10 ,0x28},    //reg_ipr_arb_lvl_thrd
    {SC06_11 ,0x28},    //reg_ipr_arb_req_length
    {SC06_12 ,0x40},

    {SC06_14 ,0x00},    //[2:0] memory config:3'000: YG, CB, CR 3'b001: Y, C 3'b010: YG only 3'b100: YG, CB, CR, M 'b101: Y, C, M 3'b110: Y, M
    {SC06_15 ,0x00},    //IPM channel
    {SC06_16 ,0x00},    //OPM channel
    {SC06_17 ,0x00},    //IPR channel
    {SC06_26 ,0x10},    //[5:0]reg_vsync_length_ipm

    {SC06_C0 ,0x60},    //REG_MAX_ADDR
    {SC06_C1 ,0x3d},    //REG_MAX_ADDR
    {SC06_C2 ,0x08},    //REG_MAX_ADDR
    {SC06_C3 ,0x00},    //REG_MAX_ADDR
    {SC06_C4 ,0x00},    //REG_MIN_ADDR
    {SC06_C5 ,0x00},    //REG_MIN_ADDR
    {SC06_C6 ,0x00},    //REG_MIN_ADDR
    {SC06_C7 ,0x00},    //REG_MIN_ADDR
    {SC06_C8 ,0x80},    //REG_MAX_ADDR_M
    {SC06_C9 ,0xfc},    //REG_MAX_ADDR_M
    {SC06_CA ,0x0a},    //REG_MAX_ADDR_M
    {SC06_CB ,0x00},    //REG_MAX_ADDR_M
    {SC06_CC ,0x60},    //REG_MIN_ADDR_M
    {SC06_CD ,0x3d},    //REG_MIN_ADDR_M
    {SC06_CE ,0x08},    //REG_MIN_ADDR_M
    {SC06_CF,0x00},     //REG_MIN_ADDR_M

#if 0
    // Video Compression Settings
    {SC1_00,0x01},
    {SC1_01,0x00},
    {SC1_02,0x00},
    {SC1_03,0x15},
    {SC1_04,0x40},
    {SC1_05,0x06},
    {SC1_06,0x12},
    {SC1_07,0x00},
    {SC1_08,0x11},
    {SC1_09,0x02},
    {SC1_0A,0x11},
    {SC1_0B,0x02},
    {SC1_0C,0x09},
    {SC1_0D,0x07},
    {SC1_0E,0x13},
    {SC1_0F,0x05},
    {SC1_10,0x03},
    {SC1_11,0x00},
    {SC1_12,0x1E},
    {SC1_13,0x44},
    {SC1_20,0x0F},
    {SC1_21,0x00},
    {SC1_22,0x00},
    {SC1_23,0x15},
    {SC1_24,0x40},
    {SC1_25,0x06},
    {SC1_26,0x12},
    {SC1_27,0x00},
    {SC1_28,0x11},
    {SC1_29,0x02},
    {SC1_2A,0x11},
    {SC1_2B,0x02},
    {SC1_2C,0x09},
    {SC1_2D,0x07},
    {SC1_2E,0x13},
    {SC1_2F,0x05},
    {SC1_30,0x03},
    {SC1_31,0x00},
    {SC1_32,0x1E},
    {SC1_33,0x44},
    {SC1_A0,0x07},
    {SC1_A1,0x07},
    {SC1_A4,0x07},
    {SC1_A5,0x00},
    {SC1_A6,0x57},
    {SC1_A7,0x55},
    {SC1_A8,0x07},
    {SC1_A9,0x07},
    {SC1_AA,0x07},
    {SC1_AB,0x07},
    {SC1_AC,0xE5},
    {SC1_AD,0xE5},
    {SC1_AE,0xE5},
    {SC1_AF,0x00},
    {SC1_44,0xA0},
    {SC1_C6,0x51},
    {SC1_C8,0x01},
    {SC1_C9,0x07},
#endif
    //{SCC_42,0x00},  //disable DNR before IPR setting is ready
#if 0
    {SC1_CC,0xE5},
#endif
    // R2Y Sel
    {SC07_41, 0x33}, //Main & Sub window RGB to YCbCr equation selection 0: (BT601 range 0~255), 1 : (BT601range 16~235), 2: (BT709 range 0~255), 3 : (BT709range 16~235),

    //{SC8_30, 0xFF}, //reg_gamma_r_max_base0
    //{SC8_31, 0x03}, //
    //{SC8_34, 0xFF}, //reg_gamma_g_max_base0
    //{SC8_35, 0x03}, //
    //{SC8_38, 0xFF}, //reg_gamma_b_max_base0
    //{SC8_39, 0x03}, //

    // Sub window init setting
    //{SCB_28, 0x02}, // SCB_28[1], sub window init setting, better to set/clr according to sub win enable/disable (SC0_5C[3])


    // Dither control //0x36~0x38

    {SC00_BC, BIT2 | BIT5 | BIT7}, // 071026 RD suggest, New added for auto position error
    // display port

    // brightness 0x2A~0x2D
    //{SC0_2A, BIT0}, // disable birghtness control
    // contrast 0x2E~0x31
    {SC00_2E, 0x60}, // enable contrast function and set 0 as center

    // border color 0x32~0x35
    {SC00_32, 0},// panel background color

    {SC00_A4, 0xC0},// [7]:idclk use genshot CDC
    {SC22_1E, 0x24},// Force disable IP2 422 pack

    // Auto Adjustment 0x78~
    {SC00_7B, 0x1}, // enable auto position
    {SC00_7C, 0x40},//Albert 070118
    {SC00_8B, 0x19}, // enable auto phase
    {SC00_78, 0},//x11}, // enable auto Gain
    {SC00_8A, 0x1C}, // enable auto phase

    // VSync Status
    {SC00_9B, 0x30},
    {SC00_9E, 0x20},

    // H period detect with 8 line mode
    {SC00_DF, 0x80},
    {SC00_E5, 0x80},

#if 0//USE_HDMI_DVI_INT
    //{REG_29E1, 0xFF}, //Disable HDMI INT_MASK
    //{REG_05C0, 0xFF}, //Disable HDMI INT_MASK
    //{REG_05C1, 0xFF}, //Disable HDMI INT_MASK
#endif

	//VIP RGBdelta mode(RGB in and RGB out)
	{SC0B_B0, 0x01},
	{SC0B_B2, 0x14},
	{SC0B_B4, 0x14},

    // interrupt control
    {SC00_CA, 0x0F}, // BIT0 for level trigger, BIT1 for priority
    {SC00_CE, 0},
    {SC00_CF, 0},

    {SC00_E6, 3},//1},//10}, // RD suggest 20081008
    //{SC0_E7, 1}, //default=1
    //{SC0_E5, 0},
    //{SC0_EB, 0x1E},
    // interlace judgement
    //{SC0_E8, 0}, //0x01} //1:non-interlace default=0
    //{SC0_E9, BIT7}, // 20071219 RD Anson suggest for B1/B2/Goya, bk0_80~81 don't need to -0x10
   // {0x5E, 0x20},// Coring threshold

    // combo
    {SC00_FA, BIT2}, // Need set for combo hv mode ([2] always 1)

#if 1
    // SAR
    {REG_003A60, 0x20},
    {REG_003A61, 0x20},
#endif
    {SC00_00, 0x00},
};

//-----------------------------------------
// @param  bEnable     TRUE: Clock Gating
//-----------------------------------------
void msInitClockGating(BOOL bEnable)
{
    msWriteByteMask(REG_120F0C, bEnable?BIT0:0x20, BIT5|BIT0); // reg_ckg_r2_secure
    msWriteByteMask(REG_120F12, bEnable?BIT0:0x20, BIT5|BIT0); // reg_ckg_fclk
    msWriteByteMask(REG_120F1F, bEnable?BIT0:0x00, BIT0); // reg_ckg_pll_idclk
    msWriteByteMask(REG_120F23, bEnable?BIT0:0x04, BIT2|BIT0); // reg_ckg_imi
    msWriteByteMask(REG_120F30, bEnable?BIT0:0x10, BIT4|BIT0); // reg_ckg_sosd_ft
    msWriteByteMask(REG_120F49, bEnable?BIT0:0x00, BIT0); // reg_ckg_aesdma
    msWriteByteMask(REG_120F83, bEnable?BIT4:0x00, BIT4); // reg_ckg_odclk
    msWriteByteMask(REG_120F85, bEnable?BIT0:0x00, BIT0); // reg_ckg_idclk
    msWriteByteMask(REG_140506, bEnable?BIT5:0x00, BIT5); // reg_lpll1_pd
}

/*
///////////////////////////////////////////////////////////////////////////////
// <Function>: msTMDSInit
//
// <Description>: HDMI and HDMI audio initial function
//
// <Parameter>: None
//
///////////////////////////////////////////////////////////////////////////////
void msTMDSInit(void)
{
    //patch for HDCP issue, reset HDCP FSM when no input clock
    old_msWriteByteMask(REG_150D, BIT5, BIT5);

    // DVI power enable
    //old_msWriteByteMask(REG_PM_B2, 0, BIT2);         // power on DVI CKIN. (port 0 and port 1) (reg_b2[2])
    // 1200301 coding, set EQ to 1
    old_msWriteByte(REG_01C7,0x00);                 // DVI phase dac power on for CHIP_TSUML2

    // Audio
    old_msWriteByteMask(REG_1712, 0x22, 0xFF); //[3:0]:KP, [7:4]:KM
    old_msWriteByteMask(REG_1713, 0x00, 0x60); //[6:5]:KN
    old_msWriteByteMask(REG_1715, 0x31, 0xFF); //[B:8]:DDIV, [F:C]:FBDIV

    // Word Boundary setting
    old_msWriteByteMask(REG_161D, 0xFF, 0xFF);
    old_msWriteByteMask(REG_1621, 0xFF, 0xFF);
    old_msWriteByteMask(REG_1625, 0xFF, 0xFF);
    old_msWriteByteMask(REG_1629, 0xFF, 0xFF);

    // SPDIF Out Enable
    old_msWriteByteMask(REG_1426, 0x02, 0x02);


    old_msWriteByteMask(REG_1569, 0x00, BIT0);

    old_msWrite2Byte(REG_1604, 0x00C0);
    old_msWrite2Byte(REG_1438, 0x0010);
    old_msWrite2Byte(REG_142A, 0x0017);

    //CTS
    old_msWriteByteMask(REG_1711, BIT7, BIT7);
    old_msWriteByteMask(REG_1707, BIT4, BIT4);

    //old_msWriteByteMask(REG_0200, 0x02, 0x02);
    old_msWriteByteMask(REG_1439, 0xC0, 0xFF);  //Audio volume attenuation control
    old_msWriteByteMask(REG_1427, 0x10, 0x10);  //Reset Audio FIFO
    old_msWriteByteMask(REG_1427, 0x00, 0x10);

    // Lane 0 delay control
    old_msWriteByteMask(REG_1613, 0x0C, 0x0E);

    old_msWrite2Byte(REG_16B0, 0x0859); //clock stable IRQ boumdary
    old_msWrite2Byte(REG_16B2, 0x145D);  //clock stable IRQ boumdary

    // PLL + Digital CDR (For U01 Demo)
    //old_msWriteByteMask(REG_17AD, 0x03, 0x03);
    //old_msWriteByteMask(REG_17A8, 0x1B, 0x2F);
    old_msWriteByteMask(REG_17CF, 0x80, 0x80);

    old_msWriteByteMask(REG_163D, 0x1F, 0x1F);
    old_msWrite2Byte(REG_175C, 0xFFFF);  //IRQ Mask
    old_msWrite2Byte(REG_175A, 0xFFFF);  //IRQ CLR
    old_msWrite2Byte(REG_175A, 0x0000);  //IRQ CLR
    old_msWrite2Byte(REG_1418, 0xFFFF);  //IRQ Mask Error Status
    old_msWrite2Byte(REG_1508, 0xFF00);  //IRQ Mask HDCP
    old_msWrite2Byte(REG_153C, 0x0001);  //Clr IRQ
    old_msWriteByteMask(REG_2B19, 0x00, 0x80);  //IRQ bank Mask off

    // Power up Xtal clocks and DVI detection clock for DVI clock detection
    //msWriteWordMask(REG_290D, 0, 0x601);      // HDCP related power
    // DVI PLL power control
    //msWriteWordMask(REG_29C1, 0, 0xFFFF);     // enable DVI0 PLL power
    // DVI slowly update
    //old_msWriteByteMask(REG_2853, 0xE3, 0xFF);      // [15]:update Bch slowly; [14:8]: 0x63(100 lines)
    //old_msWrite2ByteMask(REG_2854, 0xE3E3, 0xFFFF); // [15:8]: update Rch slowly, [7:0]:update Gch slowly
    // Enable DVI phase accumulator extension and clock stable gate
    //old_msWriteByteMask(REG_2801, BIT6|BIT5, BIT6|BIT5); // [6]:Enable of acc8lsb extension ; [5]:Enable of acc8lsb extension select
    //old_msWriteByteMask(REG_2841, BIT2, BIT2);      // [2]:Enable the function that clock gen gate signal controlled by TMDS clock stable
    // enable Vsync glitch filter
    //old_msWriteByteMask(REG_283C, BIT4, BIT4);      // enable DVI port0 Vsync glitch filter
    // Enable DVI clock tolerance: 4*0.7 = 2.8 MHz
    //old_msWriteByteMask(REG_2827, 0x20, 0x70);      //the comp value is reg_comp_tol * 4
    //old_msWriteByteMask(REG_2825, 0xF0, 0xF0);      //0x110A25=0xB0, DVI clock detection counter tolerance set to 7
    //old_msWriteByte(REG_2852,0x10);                 // [4]: enable auto DVIPLL adjust
}
*/
#if ENABLE_AUTOEQ
void drv_TMDS_AUTOEQ_initial(void)
{
    old_msWriteByteMask(REG_17AB, 0x04, 0x04); //ECO bit
    old_msWriteByteMask(REG_1650, 0x80, 0x80); //Enable speed up EQ tune
    old_msWriteByteMask(REG_164A, 0x00, 0x13); //under over parse mode
    old_msWrite2Byte(REG_1658, 0x0002); //fine tune time value
    old_msWriteByteMask(REG_164A, 0x08, 0x68);
    old_msWrite2Byte(REG_1652, 0x0042); //symbol detect time

    old_msWriteByteMask(REG_164A, 0x00, 0x80);
    old_msWriteByteMask(REG_164C, 0x0A, 0x3F); //phase check times
    old_msWriteByteMask(REG_164E, 0x03, 0x3F); //phase stable threshold value
    old_msWrite2Byte(REG_1654, 0x0080); //phase check times

    old_msWriteByteMask(REG_1650, 0x03, 0x7F); //accumulator threshold
    old_msWrite2Byte(REG_1656, 0x0040); //under over calcu time

    old_msWriteByteMask(REG_164D, 0x0A, 0x3F);
    old_msWriteByteMask(REG_164F, 0x03, 0x3F);

    old_msWriteByteMask(REG_1651, 0x01, 0x03);
    old_msWriteByteMask(REG_1651, 0x04, 0x04);
    old_msWrite2Byte(REG_165A, 0x0028);
}

void drv_TMDS_AUTOEQ_TRIG(void)
{
    AUTOEQ_FLAG = 0;
    old_msWriteByteMask(REG_164A, 0x00, 0x04); //TRIG  off
    old_msWriteByteMask(REG_17AC, 0x00, 0xE0); //disable EQ overwrite
    old_msWrite2Byte(REG_1752, 0x0008); //RESET
    old_msWrite2Byte(REG_1752, 0x0000);
    old_msWriteByteMask(REG_164A, 0x04, 0x04); //TRIG
}
#endif

#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18

void Delay1us(void)
{
    BYTE xdata i; // Add by Jonson 20100917

    if(g_bMcuPMClock)
        return;
    i=5; // 315K // 20: 135K
    while(i--)
    {
        _nop_();
    }
}
#endif // #if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
#if 0
void TMDSISR(void)
{
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
    BYTE xdata u8AksvVal = 0;
    BYTE xdata u8HDCPFSM = 0;
    static Bool xdata bflag = FALSE;
    BYTE xdata i_loop;
#endif

    if(!INPUT_IS_TMDS())
    {
        _MEM_MSWRITE_BYTE_MASK(REG_175D, 0x20, 0x20);
        _MEM_MSWRITE_BYTE_MASK(REG_175B, 0x20, 0x20);
        _MEM_MSWRITE_BYTE_MASK(REG_175B, 0x00, 0x20);
        return;
    }

    if(_bit5_(msRegs[REG_1757])) // CLK back to stable IRQ
    {
        WORD clk_cnts =0;
        _MEM_MSWRITE_BYTE_MASK(REG_175D, 0x20, 0x20);//Mask
        _MEM_MSWRITE_BYTE_MASK(REG_1543, 0x00, 0x10);//disable overwrite
        _MEM_MSWRITE_BYTE_MASK(REG_1569, 0x01, 0x01);//auto_CLR
        _MEM_MSWRITE_BYTE_MASK(REG_1569, 0x00, 0x01);
        clk_cnts = ((((WORD)_MEM_MSREAD_BYTE((REG_16B6)+1))<<8)|_MEM_MSREAD_BYTE((REG_16B6)));
        //if (!(_bit2_(msRegs[REG_0C01]))) // not MHL
        {
            if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV2) ) //div 1
            {
                _MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x00, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x00, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x10, 0xF0); //ICtrl value
            }
            else if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV4) ) //div 2
            {
                _MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x04, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x04, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x20, 0xF0); //ICtrl value

            }
            else if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV8) ) //div 4
            {
                _MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x08, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x08, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x40, 0xF0); //ICtrl value
            }
            else //div 8
            {
                _MEM_MSWRITE_BYTE_MASK(REG_17AB, 0x60, 0x60);
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x0C, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17A5, 0x0C, 0x0C);
                _MEM_MSWRITE_BYTE_MASK(REG_17AC, 0x01, 0x01); //ICtrl overwrite
                _MEM_MSWRITE_BYTE_MASK(REG_17AA, 0x40, 0xF0); //ICtrl value
            }
#if ENABLE_AUTOEQ
            if( clk_cnts > HDMI_VCO_CODE_CALCULATE(HDMI_VCO_LOOP_DIV2) )
            {
                AUTOEQ_FLAG = 1;
                _MEM_MSWRITE_BYTE_MASK(REG_1752, 0x00, 0x80);
            }
            else
            {
                AUTOEQ_FLAG = 0;
                _MEM_MSWRITE_BYTE_MASK(REG_164A,0x00,0x04);
                _MEM_MSWRITE_BYTE_MASK(REG_17AC,0xE0,0xE0);
                _MEM_MSWRITE_BYTE(REG_17B0, HDMI_CLK_CHANNEL_EQ);
                _MEM_MSWRITE_BYTE(REG_17B1, HDMI_B_CHANNEL_EQ);
                _MEM_MSWRITE_BYTE(REG_17B2, HDMI_G_CHANNEL_EQ);
                _MEM_MSWRITE_BYTE(REG_17B3, HDMI_R_CHANNEL_EQ);
            }
#endif
        }

        _MEM_MSWRITE_BYTE_MASK(REG_175B, 0x20, 0x20);
        _MEM_MSWRITE_BYTE_MASK(REG_175B, 0x00, 0x20);
        _MEM_MSWRITE_BYTE_MASK(REG_175D, 0x00, 0x20);
    }
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
    if(_bit2_(msRegs[REG_1539]))
    {

        _MEM_MSWRITE_BYTE_MASK(REG_1509, 0x84, 0x84);
        _MEM_MSWRITE_BYTE(REG_1539, 0xFF);

        u8HDCPFSM = MEM_MSREAD_BYTE(REG_1531) & 0x1F;

        if(((u8HDCPFSM !=  0x02) && !((u8HDCPFSM ==  0x03) && (MEM_MSREAD_BYTE(REG_1518) & BIT7))) && !bflag)
        //if((MEM_MSREAD_BYTE(REG_1518) & BIT7) && !bflag)
        {
            _MEM_MSWRITE_BYTE_MASK(REG_150F, 0x00, 0xC0);
            _MEM_MSWRITE_BYTE_MASK(REG_150C, 0x14, 0xFF);
            _MEM_MSWRITE_BYTE_MASK(REG_150D, 0, 0x03);
            _MEM_MSWRITE_BYTE(REG_153A, 0x10);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            _MEM_MSWRITE_BYTE(REG_153A, 0x04);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            u8AksvVal = MEM_MSREAD_BYTE(REG_1525);
            _MEM_MSWRITE_BYTE_MASK(REG_150F, 0x80, 0xC0);
            _MEM_MSWRITE_BYTE(REG_153A, 0x10);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            _MEM_MSWRITE_BYTE(REG_150E, u8AksvVal);
            _MEM_MSWRITE_BYTE(REG_153A, 0x08);
            for(i_loop=0; i_loop<2;i_loop++) {Delay1us();}
            _MEM_MSWRITE_BYTE_MASK(REG_150F, 0x00, 0xC0);
            bflag = TRUE;
        }
        else
        {
            bflag = FALSE;
        }
        _MEM_MSWRITE_BYTE_MASK(REG_1509, 0x00, 0x84);
    }
#endif // #if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
}
#endif

void Init_FRORefTrim(void)
{
    BYTE u8EfuseData;
    u8EfuseData = msEread_GetDataFromEfuse(0x131);

    if( u8EfuseData & _BIT7 )
    {
        drvmStar_printData("CALIBARYION RCOSC_12M BY EFUSE DATA",0);
        msWriteByteMask(REG_00038A, u8EfuseData, 0x7F);
    }
    else
    {
        if (!msPM_StartRCOSCCal())
        {
            drvmStar_printData("CALIBARYION RCOSC_12M FAIL!",0);
        }
        else
        {
            drvmStar_printData("CALIBARYION RCOSC_12M Success!",0);
        }
    }
}

void Init_USBXCVRRefTrim(void)
{
    BYTE u8EfuseValue;

    // USB_rterm
    u8EfuseValue = msEread_GetDataFromEfuse(0x147);

    if(u8EfuseValue & BIT4)
    {
        msWrite2ByteMask(REG_150328, (u8EfuseValue&0x0F)<<5, 0x01E0);
    }

    // USB_volt_0
    if(u8EfuseValue & BIT7)
    {
        msWrite2ByteMask(REG_15032A, (u8EfuseValue&0x60)<<3, 0x0300);
    }
}

void msDrvSyninpllInit(void)
{
    #define SynthCLK 120

    BYTE u8LoopDivFirst ,u8LoopDivSecond ,u8OutputDiv;
    DWORD u32MPLL_MHZ = 432ul; //unit:MHz
    DWORD u32SynthSet = 0;

    u8LoopDivFirst = 2;
    u8LoopDivSecond = 4;
    u8OutputDiv = 0;

    //set SYNTHESIZER's frequency
    u32SynthSet = ((float)u32MPLL_MHZ * 524288) / SynthCLK;
    msWrite2ByteMask(REG_101E40, (u32SynthSet&0xFFFF)   ,0xFFFF);  //reg_synth_set
    msWriteByteMask(REG_101E42, (u32SynthSet>>16)&0xFF   ,0xFF);   //reg_synth_set
    msWriteByteMask(REG_101E44, BIT0   ,BIT0);                     //reg_synth_sld
    msWriteByteMask(REG_101E4D, u8LoopDivSecond, 0xFF);            //reg_syninpll_loopdiv_second
    msWriteByteMask(REG_101E4C, u8OutputDiv, 0x07);                //reg_syninpll_output_div
    msWriteByteMask(REG_101E4A, 0, BIT4);                          //reg_syninpll_pd
}

void drvmStar_InitPnl( void )
{
    memset(&g_sPnlInfo, 0, sizeof(ST_PANEL_INFO));
    memset(&g_sPnlInfoInit, 0, sizeof(ST_PANEL_INFO));
    
    // Type LVDS
#if PANEL_LVDS_1CH
    g_sPnlInfo.ePnlTypeLvds = EN_PNL_LVDS_CH_1;
#elif PANEL_LVDS_2CH
    g_sPnlInfo.ePnlTypeLvds = EN_PNL_LVDS_CH_2;
#elif PANEL_LVDS_4CH
    g_sPnlInfo.ePnlTypeLvds = EN_PNL_LVDS_CH_4;
#endif

    g_sPnlInfo.ePnlLvdsChA = LVDS_CH_A_SWAP;
    g_sPnlInfo.ePnlLvdsChB = LVDS_CH_B_SWAP;
    g_sPnlInfo.ePnlLvdsChC = LVDS_CH_C_SWAP;
    g_sPnlInfo.ePnlLvdsChD = LVDS_CH_D_SWAP;
    g_sPnlInfo.ePnlLvdsChE = LVDS_CH_E_SWAP;
    g_sPnlInfo.ePnlLvdsChF = LVDS_CH_F_SWAP;
    g_sPnlInfo.ePnlLvdsChG = LVDS_CH_G_SWAP;
    g_sPnlInfo.ePnlLvdsChH = LVDS_CH_H_SWAP;

    g_sPnlInfo.u8PnlLvdsTiEn = LVDS_TIMode;
    g_sPnlInfo.u8PnlLvdsTiBitMode = LVDS_TI_BitModeSel;
    g_sPnlInfo.u8PnlSwapPn = PANEL_SWAP_PN;
    g_sPnlInfo.u8PnlSwapMlsb = PanelSwapMLSB;
    g_sPnlInfo.u8PnlInvDE = PanelInvDE;
    g_sPnlInfo.u8PnlInvVSync = PanelInvVSync;
    g_sPnlInfo.u8PnlInvHSync = PanelInvHSync;

    // Type Vby1
#if PANEL_VBY1_1CH_8Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_1_8BIT;
#elif PANEL_VBY1_1CH_10Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_1_10BIT;
#elif PANEL_VBY1_2CH_8Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_2_8BIT;
#elif PANEL_VBY1_2CH_10Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_2_10BIT;
#elif PANEL_VBY1_4CH_8Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_4_8BIT;
#elif PANEL_VBY1_4CH_10Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_4_10BIT;
#elif PANEL_VBY1_8CH_8Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_8_8BIT;
#elif PANEL_VBY1_8CH_10Bit
    g_sPnlInfo.ePnlTypeVby1 = EN_PNL_VBY1_CH_8_10BIT;
#endif

    g_sPnlInfo.u8PnlVby1SwpPnLockn = PANEL_SWAP_PN_LOCKN;

    // Type Edp
#if PANEL_EDP
    g_sPnlInfo.ePnlTypeEdp = EN_PNL_EDP_ENABLE;
    g_sPnlInfo.u8PnlEdpLinkRate                     = eDPTXLinkRate;
    g_sPnlInfo.u8PnlEdpLaneCnt                      = eDPTXLaneCount;
    g_sPnlInfo.bPnlEdpFineTuneTUEn                  = eDPTX_FineTune_TU_En;
    g_sPnlInfo.bPnlEdpDETECTHPD                     = eDPTXDETECTHPD;
    g_sPnlInfo.bPnlEdpP0PNSWAP                      = eDPTXP0PNSWAP;
    g_sPnlInfo.bPnlEdpP1PNSWAP                      = eDPTXP1PNSWAP;
    g_sPnlInfo.bPnlEdpAuxP0PNSwap                   = eDPTXAuxP0PNSWAP;
    g_sPnlInfo.bPnlEdpAuxP1PNSwap                   = eDPTXAuxP1PNSWAP;
    g_sPnlInfo.u8PnlEdpTotalPorts                   = eDPTX_total_ports;
    g_sPnlInfo.u8PnlEdpMODOnePort                   = eDPTXMODOnePort;
    g_sPnlInfo.u8PnlEdpMODUsePort                   = eDPTXMODUsePort;
    g_sPnlInfo.u8PnlEdpHBR2PortEn                   = eDPTXHBR2PortEn;
    g_sPnlInfo.u8PnlEdpColorDepth                   = eDPTXColorDepth;
    g_sPnlInfo.bPnlEdpEnPG                          = eDPTXEnPG;
    g_sPnlInfo.u8PnlEdpHPDPort0                     = eDPTXHPDPort0;
    g_sPnlInfo.u8PnlEdpHPDPort1                     = eDPTXHPDPort1;
    g_sPnlInfo.u8PnlEdpSwTBLShiftLv                 = eDP_SwTBL_ShiftLv;
    g_sPnlInfo.u8PnlEdpSwDftValue                   = eDP_SWING_DefaultValue;
    g_sPnlInfo.u8PnlEdpPreemphasisDftValue          = eDP_PREMPHASIS_DefaultValue;
    g_sPnlInfo.bPnlEdpP0LaneSwap                    = eDPTXP0LaneSWAP;
    g_sPnlInfo.bPnlEdpP1LaneSwap                    = eDPTXP1LaneSWAP;
    g_sPnlInfo.bPnlEdpEnSSC                         = eDPTXEnSSC;
    g_sPnlInfo.bPnlEdpEnASSR                        = eDPTXEnASSR;
    g_sPnlInfo.bPnlEdpEnAFR                         = eDPTXEnAFR;
    g_sPnlInfo.bPnlEdpNoAuxHandshakeLinkTrain       = eDPTX_NO_AUX_HANDSHAKE_LINK_TRAINING;
#endif

#if PANEL_OUTPUT_FMT_LR
    g_sPnlInfo.ePnlOutFmt = EN_PNL_OUT_FMT_LR;
#elif PANEL_OUTPUT_FMT_4B
    g_sPnlInfo.ePnlOutFmt = EN_PNL_OUT_FMT_4B;
#elif PANEL_OUTPUT_FMT_8B
    g_sPnlInfo.ePnlOutFmt = EN_PNL_OUT_FMT_8B;
#endif

    // H info
    g_sPnlInfo.sPnlTiming.u16HSyncWidth = PanelHSyncWidth;
    g_sPnlInfo.sPnlTiming.u16HSyncBP = PanelHSyncBackPorch;
    g_sPnlInfo.sPnlTiming.u16HStart = (PanelHSyncWidth+PanelHSyncBackPorch);
    g_sPnlInfo.sPnlTiming.u16Htt = PanelHTotal;
    g_sPnlInfo.sPnlTiming.u16HttMax = PanelMaxHTotal;
    g_sPnlInfo.sPnlTiming.u16HttMin = PanelMinHTotal;
    g_sPnlInfo.sPnlTiming.u16Width = PanelWidth;

    // V info
    g_sPnlInfo.sPnlTiming.u16VSyncWidth = PanelVSyncWidth;
    g_sPnlInfo.sPnlTiming.u16VSyncBP = PanelVSyncBackPorch;
    g_sPnlInfo.sPnlTiming.u16VStart = (PanelVSyncWidth+PanelVSyncBackPorch);
    g_sPnlInfo.sPnlTiming.u16Vtt = PanelVTotal;
    g_sPnlInfo.sPnlTiming.u16VttMax = PanelMaxVTotal;
    g_sPnlInfo.sPnlTiming.u16VttMin = PanelMinVTotal;
    g_sPnlInfo.sPnlTiming.u16Height = PanelHeight;
    g_sPnlInfo.sPnlTiming.u16VFreq = PanelVfreq;
    g_sPnlInfo.sPnlTiming.u16VFreqMax = PanelMaxVfreq;
    g_sPnlInfo.sPnlTiming.u16VFreqMin = PanelMinVfreq;

    // Pixel Clock
    g_sPnlInfo.sPnlTiming.u16DClk = PanelDCLK;
    g_sPnlInfo.sPnlTiming.u16DClkMax = PanelMaxDCLK;
    g_sPnlInfo.sPnlTiming.u16DClkMin = PanelMinDCLK;

#if Enable_Expansion
    DispalyWidth = g_sPnlInfo.sPnlTiming.u16Width;
#endif

    memcpy(&g_sPnlInfoInit, &g_sPnlInfo, sizeof(ST_PANEL_INFO));
}

void drvmStar_PnlTimingSet(ST_PANELTIMING_INFO *sPnlTimingReSetup)
{
    memcpy(&g_sPnlInfo.sPnlTiming, sPnlTimingReSetup, sizeof(ST_PANELTIMING_INFO));
}

void drvmStar_TimingGenInit(void)
{
    msWriteByte(SC00_14, (( g_sPnlInfo.sPnlTiming.u16Height - 1 ) & 0xFF) + PANEL_SHIFT_DE_V); // Vert. DE end
    msWriteByte(SC00_15, ( g_sPnlInfo.sPnlTiming.u16Height - 1 + PANEL_SHIFT_DE_V ) >> 8); // Vert. DE end
    msWriteByte(SC00_12, ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) & 0xFF); // Hor. DE start
    msWriteByte(SC00_13, ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) >> 8); // Hor. DE start
    msWriteByte(SC00_16, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + (g_sPnlInfo.sPnlTiming.u16Width/PANEL_H_DIV) - 1 ) & 0xFF); // Hor. DE end
    msWriteByte(SC00_17, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + (g_sPnlInfo.sPnlTiming.u16Width/PANEL_H_DIV) - 1 ) >> 8); // Hor. DE end


    // Output Sync Timing //0x1E~0x26
    msWriteByte(SC00_1E, ( g_sPnlInfo.sPnlTiming.u16Vtt - 1 ) & 0xFF); // output vtotal
    msWriteByte(SC00_1F, ( g_sPnlInfo.sPnlTiming.u16Vtt - 1 ) >> 8); // output vtotal
    msWriteByte(SC00_20, ( g_sPnlInfo.sPnlTiming.u16Vtt - g_sPnlInfo.sPnlTiming.u16VStart ) & 0xFF); // vsync start
    msWriteByte(SC00_21, ( g_sPnlInfo.sPnlTiming.u16Vtt - g_sPnlInfo.sPnlTiming.u16VStart)>>8); // vsync start
    msWriteByte(SC00_22, ( g_sPnlInfo.sPnlTiming.u16Vtt - g_sPnlInfo.sPnlTiming.u16VStart ) >> 8); // vsync start
    msWriteByte(SC00_22, ( g_sPnlInfo.sPnlTiming.u16Vtt - g_sPnlInfo.sPnlTiming.u16VSyncBP ) & 0xFF); //vsync end
    msWriteByte(SC00_23, ( g_sPnlInfo.sPnlTiming.u16Vtt - g_sPnlInfo.sPnlTiming.u16VSyncBP ) >> 8); //vsync end
    msWriteByte(SC00_24, ( g_sPnlInfo.sPnlTiming.u16Htt/PANEL_H_DIV - 1 ) & 0xFF); // output htotal
    msWriteByte(SC00_25, ( g_sPnlInfo.sPnlTiming.u16Htt/PANEL_H_DIV - 1 ) >> 8); // output htotal
    msWriteByte(SC00_26, ( g_sPnlInfo.sPnlTiming.u16HSyncWidth/PANEL_H_DIV ) - 1); // output Hsync end

    // Scaling Image window size
    msWriteByte(SC00_18, ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) & 0xFF);
    msWriteByte(SC00_19, ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) >> 8);
    msWriteByte(SC00_1C, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + (g_sPnlInfo.sPnlTiming.u16Width/PANEL_H_DIV) - 1 ) & 0xFF);
    msWriteByte(SC00_1D, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + (g_sPnlInfo.sPnlTiming.u16Width/PANEL_H_DIV) - 1 ) >> 8);
    msWriteByte(SC00_1A, ( g_sPnlInfo.sPnlTiming.u16Height - 1 ) & 0xFF);
    msWriteByte(SC00_1B, ( g_sPnlInfo.sPnlTiming.u16Height - 1 ) >> 8);

    msWriteByte(SC03_B6, (((g_sPnlInfo.sPnlTiming.u16Height +7)>>3)+2));
    msWriteByte(SC03_B7, (((g_sPnlInfo.sPnlTiming.u16Height +7)>>3)+1));
    msDrvOutputAutoHttAlign();
}

WORD msDrvOutputHttAlign(WORD u16Htt)
{
    WORD u16HttAlign;

    if(PANEL_VBY1_2CH()||(EN_PNL_LVDS_CH_2 == g_sPnlInfo.ePnlTypeLvds))
        u16HttAlign = ALIGN_2(u16Htt);
    else if(PANEL_VBY1_4CH()||(EN_PNL_LVDS_CH_4 == g_sPnlInfo.ePnlTypeLvds))
        u16HttAlign = ALIGN_4(u16Htt);
    else if(PANEL_VBY1_8CH())
        u16HttAlign = ALIGN_8(u16Htt);
    else
        u16HttAlign = u16Htt;

    return u16HttAlign; // aligned output Htt of each scaler
}

void msDrvOutputAutoHttAlign(void)
{
    BYTE u8SC00_27 = msReadByte(SC00_27);

    u8SC00_27 &= ~(BIT5|BIT4|BIT3);
    
    if(PANEL_VBY1_2CH()||(EN_PNL_LVDS_CH_2 == g_sPnlInfo.ePnlTypeLvds))
        u8SC00_27 |= 0;
    else if(PANEL_VBY1_4CH()||(EN_PNL_LVDS_CH_4 == g_sPnlInfo.ePnlTypeLvds))
        u8SC00_27 |= BIT5;
    else if(PANEL_VBY1_8CH())
        u8SC00_27 |= BIT4;

    msWriteByte(SC00_27, u8SC00_27);
}

void drvmStar_Init( Bool bACon )
{
    BYTE i;

#if(COMBO_HDCP2_INITPROC_NEW_MODE)
    msAPI_HDCP2_reset_static_variable();
#endif
#if ENABLE_SECU_R2
    MDrv_Sys_SecuRun(0);
#endif
    msDrvSyninpllInit();
    msInitClockGating(FALSE);
    APPInput_HdcpKeyProcess(); // HDCP process key after msDrvSrcConfigInit()

    msAPI_combo_IPInitial(bACon);

    //Do Not Add Any Customize Flow between msAPI_combo_IPInitial and eCB_appmStar_CustomizeIPOptionInit function
    if(appmStar_CbGetInitFlag() && appmStar_CB_FuncList[eCB_appmStar_CustomizeIPOptionInit]!=NULL)
        ((fpappmStarCustomizeIPOptionInitCb)appmStar_CB_FuncList[eCB_appmStar_CustomizeIPOptionInit])();

    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        System_eDPTx_Init_main();
    }

    msWriteByteMask(SC00_02, BIT7, BIT7);
    for( i = 0; i < sizeof( tblInit ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tblInit[i].u16Reg, tblInit[i].u8Value );

#if ENABLE_HPERIOD_16LINE_MODE
    g_16ModePre = FALSE; // Hperiod detect is set to 8 line mode in tblInit
#endif

    // SAR Enable
    msDrvSAREnableSwitch(SARKEY_EN|CABLE_DET_SAR_EN|VBUS_DET_SAR_CH|CABLE_DET_SAR2_EN, 1); // SAR0-7 / HDMI cable det

    drvmStar_TimingGenInit();
    mcuInitXdataMapToDRAM();

#if (ENABLE_HK_CODE_ON_DRAM)
    mcuDMADownloadCode(SOURCE_SPI, DEST_MIU0, DMA_TO_DRAM_SOURCE_ADDR, DMA_TO_DRAM_DESTIN_ADDR, DMA_TO_DRAM_BYTE_COUNT);
#endif

#if (ENABLE_HK_CODE_ON_PSRAM)
    mcuDMADownloadCode(SOURCE_SPI, DEST_HK51_PSRAM, DMA_TO_PSRAM_SOURCE_ADDR, DMA_TO_PSRAM_DESTIN_ADDR, DMA_TO_PSRAM_BYTE_COUNT);
#endif

#if (ENABLE_HK_CODE_ON_DRAM) || (ENABLE_HK_CODE_ON_PSRAM)
    mcuArrangeCodeAddr();
#endif

    //old_msWriteByteMask(SC0_0E, BIT7, BIT7); // H coring disable, for dot pattern certical block issue, Jison 100610
    mStar_SetupPathInit(); //Jison need put here because mStar_SetupFreeRunMode() will use g_SetupPathInfo variable
    old_msWriteByteMask(REG_06AE,0x00,0x07);
    old_msWriteByteMask(REG_07AE,0x00,0x07);

    iGenTuningFinished = 0;
    msDrvMODInit();

}
//Average short line tuning
void mStar_STuneAverageMode(Bool bEnable)
{

    if(bEnable)
    {
        //old_msWriteByteMask(SC0_FA, BIT3,BIT3);
    }
    else
    {
        //old_msWriteByteMask(SC0_FA, 0,BIT3);
    }

}

static void msSetupDelayLine(void)
{
    //BYTE xdata u8VSyncTime=GetVSyncTime();

    WORD xdata Hi_Act=GetImageWidth();   //  OK
    WORD xdata Vi_Act=GetImageHeight();  //  OK
    WORD xdata Hi_Total=mSTar_GetInputHTotal();   //  OK
    WORD xdata Vi_Total=SC0_READ_VTOTAL();           //  OK
    WORD xdata Hi_Blanking=Hi_Total-Hi_Act;   //  OK

    WORD xdata Ho_Act=g_sPnlInfo.sPnlTiming.u16Width;
    WORD xdata Vo_Act=g_sPnlInfo.sPnlTiming.u16Height;
    WORD xdata Ho_Total=g_sPnlInfo.sPnlTiming.u16HttMin;
    WORD xdata Vo_Total;

    if((Vi_Act == 0) || (Vo_Act <=1))
    {
        drvmStar_printMsg("Vi_Act or Vo_Act invalid!!");
        return;
    }
    Vo_Total=( (DWORD)Vo_Act*Vi_Total/Vi_Act);

#if SC_V_SC_NON_MINUS1MODE
    WORD xdata u16FV=(DWORD)Vi_Act*1000/Vo_Act;
#else
    WORD xdata u16FV=((((DWORD)Vi_Act-1)*1000)/(Vo_Act-1));
#endif

    WORD xdata u16Ldly_mid = (6200-(DWORD)u16FV*Ho_Act/Ho_Total-((DWORD)Hi_Blanking*1000)/Ho_Total)/2 + 2000;
    drvmStar_printData(" u16Ldly_mid=%d",u16Ldly_mid);
    msWriteByteMask(SC00_90, (u16Ldly_mid/1000) <<4 ,BIT6|BIT5|BIT4);
    msWriteByteMask(SC00_0F, 0x02, 0x0F); //Ylock line

    u16Ldly_mid%=1000;
    msWriteByteMask(SC00_0C,  (u16Ldly_mid/=125)<<4 ,BIT6|BIT5|BIT4);
    drvmStar_printData(" u16Ldly_mid=%d",u16Ldly_mid);

#if 1
    drvmStar_printData(" == Hi_Act ==%d",Hi_Act);
    drvmStar_printData(" == Vi_Act ==%d",Vi_Act);
    drvmStar_printData(" == Hi_Total ==%d",Hi_Total);
    drvmStar_printData(" == Vi_Total ==%d",Vi_Total);
    drvmStar_printData(" == Hi_Blanking ==%d",Hi_Blanking);
    drvmStar_printData(" == Ho_Act ==%d",Ho_Act);
    drvmStar_printData(" == Vo_Act ==%d",Vo_Act);
    drvmStar_printData(" == Ho_Total ==%d",Ho_Total);
    drvmStar_printData(" == Vo_Total ==%d",Vo_Total);
    drvmStar_printData(" == u16Ldly_mid ==%d",u16Ldly_mid);
    drvmStar_printData(" == u16FV ==%d",u16FV);
#endif
}




void mStar_InterlaceModeSetting( void )
{
    msSetupDelayLine();

    if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
    {
        //msWriteByte(SC0_6F,0x00);
        //old_msWriteByte(SC0_90, (ucDelayLine)<<4);
        //msWriteByteMask(SC0_0F,BIT7,BIT7); //shift line mode enable
        if(CURRENT_INPUT_IS_VGA())//(SrcInputType == Input_Analog1)
        {
            if(msReadByte(SC00_ED)&BIT5)
               msWriteByteMask(SC00_E9,0,BIT0);
            else
               msWriteByteMask(SC00_E9,BIT0,BIT0);
        }
        else            //110921 Rick modified - B011
        {
            if(msReadByte(SC00_04)&BIT6)
            {
                msWriteByteMask(SC00_E9,0,BIT0);//0x01); //Jison 080925 for DVI interlace //0x00 090512
                msWriteByteMask(SC00_05,0,BIT0);
            }
            else
            {
        #if ENABLE_DP_INPUT
                if(CURRENT_INPUT_IS_DISPLAYPORT())      // For DP interlace 20121025
                 {
                        if (mapi_DPRx_GetInterlaceFlag(0)&BIT2) //Jison 090526
                        {
                            msWriteByteMask(SC00_E9,0,BIT0);
                        }
                        else
                        {
                                #if 0
                                if (old_msReadByte(SC0_E8)&(_BIT1|_BIT0)) //Mike //Chroma 2233 Patch
                                old_msWriteByteMask(SC0_E9,0,BIT0);
                                else
                                #endif
                                //msWriteByteMask(SC0_05,BIT0,BIT0);
                        }
                }
                else
        #endif
                {
                   msWriteByteMask(SC00_E9,BIT0,BIT0);
                }
            }

        }

        // mStar_WriteByte(BK0_74, 0x10); //enable Vertical 2-tap and CB0
        // mStar_WriteByte(BK0_90, 0x10); //set delay line=1 for 2-tap scaling
         //delay line number needs to calculate from formula
         mStar_STuneAverageMode(TRUE);
    }
    else
    {
        //old_msWriteByte(SC0_6F,0x00);
        //old_msWriteByte(SC0_90, (ucDelayLine<<4));
        //msWriteByteMask(SC0_0F,0,BIT7); //shift line mode disable
        msWriteByteMask(SC00_E9,0,BIT0);
       // mStar_WriteByte(BK0_74, 0x80); //enable Vertical 3-tap and WinSinc
       // mStar_WriteByte(BK0_90, 0x20); //set delay line=2 for 3-tap scaling
       mStar_STuneAverageMode(FALSE);
    }

}

void mStar_SetPanelSSC( BYTE freqMod, BYTE range )
{
    U64 xdata dwFactor;
    U64 xdata dwStep;
    WORD xdata wSpan;
    DWORD xdata u32MPLL_MHZ = 864ul;
#if PANEL_EDP
    BYTE xdata ucLPLL_Type = 0;
#endif

    if( (freqMod == 0) || (range == 0) )
    {
        msWrite2Byte(REG_LPLL_2E, 0);
        msWrite2ByteMask(REG_LPLL_30, 0, 0x3FFF);
        return;
    }

    // freqMod: 0~100 means 0~100K
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        if(freqMod > PANEL_SSC_MODULATION_MAX_EDP)
            freqMod = PANEL_SSC_MODULATION_MAX_EDP;
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
    {
        if(freqMod > PANEL_SSC_MODULATION_MAX)
            freqMod = PANEL_SSC_MODULATION_MAX;
    }
    else
    {
        if(freqMod > PANEL_SSC_MODULATION_MAX_VX1)
            freqMod = PANEL_SSC_MODULATION_MAX_VX1;
    }

    // range: 0~30 means 0~3%
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        if(range > PANEL_SSC_PERCENTAGE_MAX_EDP)
            range = PANEL_SSC_PERCENTAGE_MAX_EDP;
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
    {
        if(range > PANEL_SSC_PERCENTAGE_MAX)
            range = PANEL_SSC_PERCENTAGE_MAX;
    }
    else
    {
        if(range > PANEL_SSC_PERCENTAGE_MAX_VX1)
            range = PANEL_SSC_PERCENTAGE_MAX_VX1;
    }

#if PANEL_EDP
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        if( g_sPnlInfo.u8PnlEdpLinkRate == eDPTX_RBR )
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_150to400MHz;
            dwFactor = (float)((float)(u32MPLL_MHZ)*1000ull*16/u8LoopDiv[ucLPLL_Type])*((float)524288UL*(float)u8LoopGain[ucLPLL_Type]/162000);
        }
        else
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_150to400MHz;
            dwFactor = (float)((float)(u32MPLL_MHZ)*1000ull*16/u8LoopDiv[ucLPLL_Type])*((float)524288UL*(float)u8LoopGain[ucLPLL_Type]/270000);
        }
        if( g_sPnlInfo.bPnlEdpEnSSC )
            dwFactor += dwFactor*range/1000;
    }
    else
#endif
    {
        dwFactor = msRead4Byte(REG_LPLL_1E);
    }

    wSpan = (((float)(u32MPLL_MHZ)*1000ull*16/freqMod) * 131072ull) / dwFactor;
    dwStep = ((float)(dwFactor)*range/10)/wSpan/100; // 120320 coding modified

    //drvmStar_printData("PanelSSC SPAN[%x]",wSpan);
    //drvmStar_printData("PanelSSC STEP[%x]",dwStep);

    if(dwStep > 0xFFFF)
        dwStep = 0xFFFF;

    if(wSpan > 0x3FFF)
        wSpan = 0x3FFF;

    msWrite2Byte(REG_LPLL_2E, dwStep);
    msWrite2ByteMask(REG_LPLL_30, wSpan, 0x3FFF);

#if PANEL_EDP
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        if( g_sPnlInfo.bPnlEdpEnSSC )
            msWrite3Byte(REG_LPLL_90,dwFactor);//SET Value
    }
    else
#endif
    {
        msWriteBit(REG_LPLL_1B, _ENABLE, BIT3); // ssc enable
    }
}
/*
void ComboClockRtermControl(BYTE ucStatus)
{
    if( ucStatus == COMBO_INPUT_OFF )
    {
        old_msWriteByteMask(REG_01CE, BIT2 | BIT1 |BIT0, BIT2 | BIT1 | BIT0); // Turn off Clock R-term
    }
    else //if( ucStatus == COMBO_INPUT_DIGITAL || ucStatus == COMBO_INPUT_POWERSAVING || ucStatus == COMBO_INPUT_ANALOG)
    {
        old_msWriteByteMask(REG_01CE, 0, BIT2 | BIT1 | BIT0); // Turn on Clock R-term
    }
}
void ComboDataRtermControl(BYTE  ucStatus)
{
#if(ENABLE_MHL)
    if( ucStatus == COMBO_INPUT_OFF || ucStatus == COMBO_INPUT_ANALOG || ucStatus == COMBO_INPUT_POWERSAVING)
    {
        if(!MHL_CABLE_DETECT_PORT0())
            old_msWriteByteMask(REG_01C4, 0x0F, 0x0F); // Turn off Port 0 Date R-term
        if(!MHL_CABLE_DETECT_PORT1())
            old_msWriteByteMask(REG_01C4, 0xF0, 0xF0); // Turn off Port 1 Date R-term
        if(!MHL_CABLE_DETECT_PORT2())
            old_msWriteByteMask(REG_01C5, 0x0F, 0x0F); // Turn off Port 2 Date R-term
    }
    else //if( ucStatus == COMBO_INPUT_DIGITAL || ucStatus == COMBO_INPUT_POWERSAVING )
    {
        if(!MHL_CABLE_DETECT_PORT0())
            old_msWriteByteMask(REG_01C4, 0x00, 0x0F); // Turn on Port 0 Date R-term
        if(!MHL_CABLE_DETECT_PORT1())
            old_msWriteByteMask(REG_01C4, 0x00, 0xF0); // Turn on Port 1 Date R-term
        if(!MHL_CABLE_DETECT_PORT2())
            old_msWriteByteMask(REG_01C5, 0x00, 0x0F); // Turn on Port 2 Date R-term
    }
#else
    if( ucStatus == COMBO_INPUT_OFF || ucStatus == COMBO_INPUT_ANALOG || ucStatus == COMBO_INPUT_POWERSAVING)
    {
        old_msWriteByteMask(REG_01C4, 0x0F, 0x0F); // Turn off Port 0 Date R-term
        old_msWriteByteMask(REG_01C4, 0xF0, 0xF0); // Turn off Port 1 Date R-term
        old_msWriteByteMask(REG_01C5, 0x0F, 0x0F); // Turn off Port 2 Date R-term
    }
    else //if( ucStatus == COMBO_INPUT_DIGITAL || ucStatus == COMBO_INPUT_POWERSAVING)
    {
        old_msWriteByteMask(REG_01C4, 0x00, 0x0F); // Turn on Port 0 Date R-term
        old_msWriteByteMask(REG_01C4, 0x00, 0xF0); // Turn on Port 1 Date R-term
        old_msWriteByteMask(REG_01C5, 0x00, 0x0F); // Turn on Port 2 Date R-term
    }
#endif

}
void ComboPortMuxControl(BYTE  ucStatus)
{
#if(ENABLE_MHL)
    if( ucStatus == COMBO_INPUT_OFF || ucStatus == COMBO_INPUT_ANALOG )
    {
        if(!MHL_CABLE_DETECT_PORT0())
            old_msWriteByteMask(REG_01C2, 0x00, BIT5); // Turn off Port 0 Port Mux
        if(!MHL_CABLE_DETECT_PORT1())
            old_msWriteByteMask(REG_01C2, 0x00, BIT6); // Turn off Port 1 Port Mux
        if(!MHL_CABLE_DETECT_PORT2())
            old_msWriteByteMask(REG_01C2, 0x00, BIT7); // Turn off Port 2 Port Mux
    }
    else if( ucStatus == COMBO_INPUT_DIGITAL)
    {
        old_msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7);    // Turn off (Port 0 Port 1 Port 2) Port Mux

        if(SrcInputType==Input_HDMI || SrcInputType==Input_DVI)
        {
            if(!MHL_CABLE_DETECT_PORT0())
                old_msWriteByteMask(REG_01C2, BIT5, BIT5);  // Turn on Port 0 Port Mux
        }
        else if(SrcInputType==Input_HDMI2 || SrcInputType==Input_DVI2 || SrcInputType==Input_Displayport)
        {
            if(!MHL_CABLE_DETECT_PORT1())
                old_msWriteByteMask(REG_01C2, BIT6, BIT6);  // Turn on Port 1 Port Mux
        }
        else //if(SrcInputType==Input_HDMI3 || SrcInputType==Input_DVI3 || SrcInputType==Input_Displayport3)
        {
            if(!MHL_CABLE_DETECT_PORT2())
                old_msWriteByteMask(REG_01C2, BIT7, BIT7);  // Turn on Port 2 Port Mux
        }
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_A)
    {
        if(!MHL_CABLE_DETECT_PORT0())
            old_msWriteByteMask(REG_01C2, BIT5, BIT5 | BIT6 | BIT7);  // Turn on Port 0 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_B)
    {
        if(!MHL_CABLE_DETECT_PORT1())
            old_msWriteByteMask(REG_01C2, BIT6, BIT5 | BIT6 | BIT7);  // Turn on Port 1 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_C)
    {
        if(!MHL_CABLE_DETECT_PORT2())
            old_msWriteByteMask(REG_01C2, BIT7, BIT5 | BIT6 | BIT7);  // Turn on Port 2 Port Mux
    }
    else // if(ucStatus == COMBO_INPUT_POWERSAVING)
    {
        if(!MHL_CABLE_DETECT_PORT0())
            old_msWriteByteMask(REG_01C2, BIT5, BIT5); // Turn on Port 0 Port Mux
        if(!MHL_CABLE_DETECT_PORT1())
            old_msWriteByteMask(REG_01C2, BIT6, BIT6); // Turn on Port 1 Port Mux
        if(!MHL_CABLE_DETECT_PORT2())
            old_msWriteByteMask(REG_01C2, BIT7, BIT7); // Turn on Port 2 Port Mux
    }
#else
    if( ucStatus == COMBO_INPUT_OFF)
    {
        old_msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7); // (Turn off Port Mux 0~2) BIT 5~7 = Port 0~2
    }
    else if(ucStatus == COMBO_INPUT_ANALOG)
    {
        old_msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7); // (Turn off Port Mux 0~2) BIT 5~7 = Port 0~2
    }
    else if( ucStatus == COMBO_INPUT_DIGITAL)
    {
        old_msWriteByteMask(REG_01C2, 0x00, BIT5 | BIT6 | BIT7); // (Turn off Port Mux 0~2) BIT 5~7 = Port 0~2

        if(SrcInputType==Input_HDMI || SrcInputType==Input_DVI)
        {
            old_msWriteByteMask(REG_01C2, BIT5, BIT5);  // Turn on Port 0 Port Mux
        }
        else if(SrcInputType==Input_HDMI2 || SrcInputType==Input_DVI2 || SrcInputType==Input_Displayport)
        {
            old_msWriteByteMask(REG_01C2, BIT6, BIT6);   // Turn on Port 1 Port Mux
        }
        else //if(SrcInputType==Input_HDMI3 || SrcInputType==Input_DVI3 || SrcInputType==Input_Displayport3)
        {
            old_msWriteByteMask(REG_01C2, BIT7, BIT7);   // Turn on Port 2 Port Mux
        }
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_A)
    {
        old_msWriteByteMask(REG_01C2, BIT5, BIT5 | BIT6 | BIT7);  // Turn on Port 0 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_B)
    {
        old_msWriteByteMask(REG_01C2, BIT6, BIT5 | BIT6 | BIT7);  // Turn on Port 1 Port Mux
    }
    else if(ucStatus == COMBO_INPUT_DIGITAL_C)
    {
        old_msWriteByteMask(REG_01C2, BIT7, BIT5 | BIT6 | BIT7);  // Turn on Port 2 Port Mux
    }
    else //if(ucStatus == COMBO_INPUT_POWERSAVING)
    {
        old_msWriteByteMask(REG_01C2, BIT5 | BIT6 | BIT7, BIT5 | BIT6 | BIT7); // (Turn on Port Mux 0~2) BIT 5~7 = Port 0~2
    }
#endif

}
void ComboDeMuxControl(BYTE  ucStatus)
{
    if( ucStatus == COMBO_INPUT_OFF)
    {
#if(ENABLE_MHL)
        if(!(MHL_CABLE_DETECT_PORT0() || MHL_CABLE_DETECT_PORT1() || MHL_CABLE_DETECT_PORT2()))
            old_msWriteByteMask(REG_01C5, 0xF0, 0xF0); // Turn off DeMux
#else
        old_msWriteByteMask(REG_01C5, 0xF0, 0xF0); // Turn off DeMux
#endif
    }
    else
    {
        old_msWriteByteMask(REG_01C5, 0x00, 0xF0); // Turn on DeMux
    }
}
*/
/*

void ComboInputControl(ComboInputType ctrl)
{
    ComboClockRtermControl(ctrl);
    ComboDataRtermControl(ctrl);
    ComboPortMuxControl(ctrl);
    ComboDeMuxControl(ctrl);


    if(ucStatus == COMBO_INPUT_OFF)
        printMsg("ucStatus == COMBO_INPUT_OFF");
    else if(ucStatus == COMBO_INPUT_POWERSAVING)
        printMsg("ucStatus == COMBO_INPUT_POWERSAVING");
    else if(ucStatus == COMBO_INPUT_ANALOG)
        printMsg("ucStatus == COMBO_INPUT_ANALOG");
    else //if(ucStatus == COMBO_INPUT_DIGITAL)
        printMsg("ucStatus == COMBO_INPUT_DIGITAL");

}
*/

// 111220 coding test
void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl)
{
    if(ctrl == DVI_POWER_STANDBY)
    {
        // DVI power on
        // old_msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        old_msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        old_msWrite2ByteMask(REG_174C, 0x0001, 0xFFFF); // enable main link clock
        old_msWrite2ByteMask(REG_174E, 0x1000, 0x3FFF); // enable Xtal

        old_msWrite2ByteMask(REG_01C2, 0xCE08, 0xFF08);
        old_msWrite2ByteMask(REG_01C6, 0xFFFF, 0xFFFF);
    }
    else if(ctrl == DVI_POWER_DOWN)
    {
        // DVI power down
        // old_msWriteByteMask(REG_01C3, BIT4, BIT4); // power down DVI CKIN.
        old_msWriteByteMask(REG_01CE, BIT4, BIT4); // power down PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        old_msWrite2ByteMask(REG_174C, 0x0000, 0xFFFF);
        old_msWrite2ByteMask(REG_174E, 0x0000, 0x3FFF);

        old_msWrite2ByteMask(REG_01C2, 0xFF08, 0xFF08);
        old_msWrite2ByteMask(REG_01C6, 0xFFFF, 0xFFFF);

    }
    else
    {
        // DVI power on
        //old_msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        old_msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap
        old_msWrite2ByteMask(REG_174C, 0xFFFF, 0xFFFF);
        old_msWrite2ByteMask(REG_174E, 0x3FFF, 0x3FFF);

        old_msWrite2ByteMask(REG_01C2, 0x0000, 0xFF08);
        old_msWrite2ByteMask(REG_01C6, 0x0000, 0xFFFF);
    }
}
void mStar_IPPowerControl(void)
{
    if (CURRENT_INPUT_IS_VGA())//( (SrcInputType == Input_Analog1) ) // ADC
    {
        // ADC power on
        drvADC_PowerCtrl(ADC_POWER_ON);

  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_DOWN);
  #endif
    }
    else if( (CURRENT_INPUT_IS_DVI())//(SrcInputType == Input_Digital)
        #if ENABLE_HDMI
        || (CURRENT_INPUT_IS_HDMI())//(SrcInputType == Input_HDMI)
        #endif // #if Enable_DVI2
    ) // DVI / HDMI
    {
  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_ON);
  #endif
        // ADC power down
        drvADC_PowerCtrl(ADC_POWER_DOWN); //CHIP_TSUMV need power down ADC for DVI good phase
    }
     else if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
  #ifdef _IP_POWER_CTRL_
        drvDVI_PowerCtrl(DVI_POWER_ON);
  #endif
        // ADC power down
        drvADC_PowerCtrl(ADC_POWER_DOWN); //CHIP_TSUMV need power down ADC for DVI good phase
    }
}

#if 0 // no use
void msTMDSSetMux( InputPortType inport )
{
#if ENABLE_AUTOEQ
    AUTOEQ_FLAG = 0;
#endif

    if(INPUT_IS_DVI(inport))//( inport == Input_Digital )
    {
        BYTE i;
        //old_msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        old_msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        old_msWrite2ByteMask(REG_174C, 0xFFFF, 0xFFFF);
        old_msWrite2ByteMask(REG_174E, 0x3FFF, 0x3FFF);
        // DVI PLL power control
        old_msWrite2ByteMask(REG_01C2, 0x4000, 0x4000); // power off DVI PLL power
        old_msWriteByteMask( REG_1740, BIT0, BIT0 ); //HDMI select.
        old_msWriteByteMask( REG_17A5, BIT0, BIT1|BIT0 ); //DCDR demux ratio select.
        //old_msWriteByteMask( REG_17A5, BIT4, BIT5|BIT4 ); //EQ mode setting.
        old_msWriteByteMask( REG_17A5, BIT5|BIT4, BIT5|BIT4 ); //EQ mode setting.

        for(i=0;i<sizeof(tComboTMDSMuxTbl)/sizeof(RegTbl2Type);i++)
            old_msWrite2ByteMask(tComboTMDSMuxTbl[i].dwReg,tComboTMDSMuxTbl[i].wValue,tComboTMDSMuxTbl[i].wMask );
#if ENABLE_AUTOEQ
        drv_TMDS_AUTOEQ_initial();
#endif
        //Port Select
        if(SrcInputType == Input_DVI)
        {
            old_msWriteByteMask( REG_1501, BIT4, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_DVI2)
        {
            old_msWriteByteMask( REG_1501, 0, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_DVI3)
        {
            old_msWriteByteMask( REG_1501, BIT5, BIT5|BIT4 );
        }
        msEread_SetHDMIInitialValue();
    }
  #if ENABLE_HDMI
    else if( INPUT_IS_HDMI(inport) ) // HDMI - Port B
    {
        BYTE i;
        //old_msWriteByteMask(REG_01C3, 0, BIT4); // power on DVI CKIN.
        old_msWriteByteMask(REG_01CE, 0, BIT4); // power on PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        old_msWrite2ByteMask(REG_174C, 0xFFFF, 0xFFFF);
        old_msWrite2ByteMask(REG_174E, 0x3FFF, 0x3FFF);
        // DVI PLL power control
        old_msWrite2ByteMask(REG_01C2, 0x4000, 0x4000); // power off DVI PLL power
        old_msWriteByteMask( REG_1740, BIT0, BIT0 ); //HDMI select.
        old_msWriteByteMask( REG_17A5, BIT0, BIT1|BIT0 ); //DCDR demux ratio select.
        //old_msWriteByteMask( REG_17A5, BIT4, BIT5|BIT4 ); //EQ mode setting.
        old_msWriteByteMask( REG_17A5, BIT5|BIT4, BIT5|BIT4 ); //EQ mode setting.
        old_msWriteByteMask( REG_1427, 0, BIT0 ); //audio source selection

        for(i=0;i<sizeof(tComboTMDSMuxTbl)/sizeof(RegTbl2Type);i++)
            old_msWrite2ByteMask(tComboTMDSMuxTbl[i].dwReg,tComboTMDSMuxTbl[i].wValue,tComboTMDSMuxTbl[i].wMask );
#if ENABLE_AUTOEQ
        drv_TMDS_AUTOEQ_initial();
#endif
        //Port Select
        if(SrcInputType == Input_HDMI)
        {
            old_msWriteByteMask( REG_1501, BIT4, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_HDMI2)
        {
            old_msWriteByteMask( REG_1501, 0, BIT5|BIT4 );
        }
        else if(SrcInputType == Input_HDMI3)
        {
            old_msWriteByteMask( REG_1501, BIT5, BIT5|BIT4 );
        }
        msEread_SetHDMIInitialValue();
    }
  #endif // #if ENABLE_HDMI_INPUT
    else // non-DVI/HDMI ports
    {
        //old_msWriteByteMask(REG_01C3, BIT4, BIT4); // power down DVI CKIN.
        old_msWriteByteMask(REG_01CE, BIT4, BIT4); // power down PLL band-gap.
        // Power up Xtal clocks and DVI detection clock for DVI clock detection
        old_msWrite2ByteMask(REG_174C, 0x0001, 0xFFFF); // enable main link clock
        old_msWrite2ByteMask(REG_174E, 0x2000, 0x3FFF); // enable Xtal
        old_msWrite2ByteMask(REG_01C2, 0xCE08, 0xFF08);
        old_msWrite2ByteMask(REG_01C6, 0xFFFF, 0xFFFF);
    }
}
#endif

#if defined(_ENABLE_LPLL_FINETUNE_)
void mStar_LPLLFineTune(WORD u16DST_HTotal)
{
   DWORD   u32Factor;
   WORD u16HttFraction;
   WORD u16LockHtt, u16OHtt;

   old_msWriteByteMask(REG_1ED3, 0,BIT6);// disable SSC
   Delay1ms(40);// at least 2 frame delay after LPLL update for fraction value stable
   //PRINT_DATA("\r\n u16DST_HTotal=%x",u16DST_HTotal);

   //old_msWriteByteMask(SC0_96, BIT7,BIT7);
   u16HttFraction = old_msRead2Byte(SC0_91)&0x0FFF;// Read Fraction
   //PRINT_DATA(" u16HttFraction :%x",u16HttFraction);

   u32Factor = old_msReadByte(REG_1ED7); // Read output dot clock
   u32Factor=(u32Factor<<16);
   u32Factor+= old_msRead2Byte(REG_1ED5);
   //PRINT_DATA(" REG_1ED7=%x",old_msReadByte(REG_1ED7));
   //PRINT_DATA(" REG_1ED4_5=%x",old_msRead2Byte(REG_1ED5));
   //((float)u16OHtt*(PANEL_HEIGHT-1)+3), +3 to avoid too close to cause fraction value floating
   #if 1 //Jison 110727 use theoretical value u16DST_HTotal to make sure get the normal dclk divider
   u32Factor = (((float)u16DST_HTotal*(g_sPnlInfo.sPnlTiming.u16Height-1)+u16HttFraction)/((float)u16DST_HTotal*(g_sPnlInfo.sPnlTiming.u16Height-1)+3))*u32Factor;
   #else
   u16OHtt = old_msRead2Byte(SC0_24)&0x0FFF;
   u16LockHtt = old_msRead2Byte(SC0_97)&0x0FFF;
   PRINT_DATA("\r\n u16LockHtt :%x",u16LockHtt);
   PRINT_DATA(" u16OHtt :%x",u16OHtt);
   u32Factor = (((float)u16LockHtt*(PANEL_HEIGHT-1)+u16HttFraction)/((float)u16OHtt*(PANEL_HEIGHT-1)+3))*u32Factor;
   #endif
   // program LPLL parameters to generate expected output dclk
   old_msWrite2Byte(REG_1ED5, u32Factor&0xFFFF);
   old_msWriteByte(REG_1ED7, u32Factor>>16); // setup output dot clock
   #if 0//DEBUG_EN
   Delay1ms(40); //at least 2 frame
   PRINT_DATA(" NEW_Fration :%x",old_msRead2Byte(SC0_91)&0x0FFF);
   //PRINT_DATA(" new REG_1ED7=%x",old_msReadByte(REG_1ED7));
   //PRINT_DATA(" new REG_1ED4_5=%x\r\n",old_msRead2Byte(REG_1ED5));
   #endif
   //old_msWriteByteMask(SC0_96, 0,BIT7);
   old_msWriteByteMask(REG_1ED3, BIT6,BIT6);// Enable SSC
}
#endif
#if 0//PANEL_LVDS_4CH
void mStar_CheckHtotalAlign(void)
{
    WORD XDATA u16Htt,u16HttMod;
    WORD XDATA u16HttAlign =4;
    u16Htt = msRead2Byte(SC00_97) + 1 ;
    u16HttMod = u16Htt % u16HttAlign;

    if (u16HttMod == 0)
    {
        msWriteByteMask(SC00_27,BIT1,BIT1); //enable auto Htt
    }
    else
    {
        u16Htt = (u16Htt + (u16HttAlign/2)) & 0xFFFC; //let Htt align 4
        if (u16Htt > g_sPnlInfo.sPnlTiming.u16HttMax)
            u16Htt = u16Htt - u16HttAlign;
        else if (u16Htt < g_sPnlInfo.sPnlTiming.u16HttMin)
            u16Htt = u16Htt + u16HttAlign;

        msWriteByteMask(SC00_27,0,BIT1); //disable auto Htt
        msWrite2ByteMask(SC00_24,u16Htt-1,0x0FFF);
    }
}
#endif
void mStar_CheckFastClock(WORD u16SCLK, WORD u16DCLK)
{
    u16SCLK = u16SCLK;
    u16DCLK = u16DCLK;
    old_msWriteByteMask(REG_1E35, 0, _BIT3|_BIT2); // fix maximum fclk
}

void msDrvMODSwingConfig(void)
{
    BYTE i, j, u8IconBase;
    
    //Calculate icon setting for LVDS Swing
    if(LVDS_SWING_TARGET < 340)
        u8IconBase = 95;
    else if(LVDS_SWING_TARGET < 370)
        u8IconBase = 87;
    else // swing target <=400
        u8IconBase = 82;

    i = (((WORD)LVDS_SWING_TARGET-u8IconBase)*10/75);
    for (j = 0; j < 5; j++)
        msWrite2Byte(REG_MOD2_A0+(j*2), (((WORD)i<<8)&0xFF00)|i); // [7:0]Control swing of channel X (Icon)

#if DOUBLE_LVDS_CLK_SWING
    i = (((WORD)(2*LVDS_SWING_TARGET)-u8IconBase)*10/75);
    msWriteByte(REG_MOD2_A3, i); // [7:0]Control swing of channel 3(clk of portB) 
    msWriteByte(REG_MOD2_A8, i); // [7:0]Control swing of channel 8(clk of portA)
#endif
}

void msDrvMODBiasConCal(void)
{
    BYTE u8BiasCon = 0x7F;

    if(g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_NONE)
    {
        iGenTuningFinished = 1;
        return;
    }

    #define BIAS_OFFSET 8
    
    msWriteByteMask(REG_MOD1_F8, u8BiasCon, 0x7F); //set Bias con to max value for checking whether calibration is successful
    
    //===== Setting MOD related CLKGEN ======//   
    msWriteByteMask(REG_CLKGEN_47, 0, BIT0);

    //===== Disable Auto HW RINT ======// 
    msWriteByteMask(REG_MOD2_91, 0, BIT7);
    
    //===== GCR_VCM_SEL[1:0] = 2'b01 ======//    
    msWriteByteMask(REG_MOD2_0C, BIT6, BIT6);

    //===== TEST_MOD[3] = 1 ======// 
    msWrite4Byte(REG_MOD1_28, 0x000FFFFF);

    //==== Enable LVDS TEST MODE ====// 
    msWrite4Byte(REG_MOD1_20, 0x000FFFFF);

    //== set calibration for CH3 with 350mV ==//
    msWriteByte(REG_MOD2_A3, 0x23);

    //===== GCR_CAL_SRC = 4'h3, CH3  ======//                                                                                                                      
    //===== GCR_CAL_LEVEL = 3'b011 ======//
    msWriteByte(REG_MOD1_7C, 0x33);
    msWriteByte(REG_MOD1_7D, 0x40);

    //===== Start HW calibration =====//
    msWriteByte(REG_MOD1_FC, 0x09);
    msWriteByte(REG_MOD1_FD, 0x08);
    msWrite2Byte(REG_MOD1_FE, 0x0009);
    msWriteByte(REG_MOD1_FA, 0x03);
    msWriteByte(REG_MOD1_E3, 0x05);
    MAsm_CPU_DelayMs(5); // wait 5ms
    msWriteByte(REG_MOD1_E1, 0x80);
    MAsm_CPU_DelayMs(10);// wait 10ms for calibration
    u8BiasCon = msReadByte(REG_MOD1_F8)&0x7F;

    //===== CALIBRATION DONE and turn off calibration ======//   
    msWriteByte(REG_MOD1_FC, 0);
    msWriteByte(REG_MOD1_FD, 0xC0);
    msWrite2Byte(REG_MOD1_FE, 0xFFFF);
    msWriteByte(REG_MOD1_FA, 0);
    msWriteByte(REG_MOD1_7C, 0);
    msWriteByte(REG_MOD1_7D, 0);
    msWriteByte(REG_MOD1_E3, 0x08);
    msWriteByte(REG_MOD1_E1, 0);
    msWriteByteMask(REG_CLKGEN_47, BIT0, BIT0);
    msWriteByteMask(REG_MOD2_91, BIT7, BIT7);
    msWriteByteMask(REG_MOD2_0C, 0, BIT6);
    msWrite4Byte(REG_MOD1_28, 0);
    
    if(u8BiasCon != 0x7F)
    {
        u8BiasCon += BIAS_OFFSET;
        if(u8BiasCon > 98 || u8BiasCon < 55) // check whether Bias con meets IC spec
           u8BiasCon = 82; // restore to default
    
        drvmStar_printData("SW calibration PASS!! %x\n", u8BiasCon);
    }
    else
    {
        u8BiasCon = 82; // restore to default
        drvmStar_printMsg("SW calibration FAIL!!\n");
    }
    msWriteByteMask(REG_MOD1_F8, u8BiasCon, 0x7F); 
    msDrvMODSwingConfig();
    iGenTuningFinished = 1;
}

//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msDrvIGenTuning()
/// @brief \b Function \b Description: GCR_RCON for Vx1 output swing calibration and GCR_BIAS_CON for current mode driver output swing calibration
/// @param <IN>        \b None
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msDrvIGenTuning( void )
{
#ifndef TSUMR2_FPGA
    BYTE XDATA  i, u8Status = FALSE;
    BYTE XDATA  j;

    if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
    {
        BYTE u8EfuseData;
        BYTE u8BoundHigh, u8BoundLow;

        if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
        {
            u8EfuseData = msEread_GetDataFromEfuse(0x14A);
            u8BoundHigh = 97;
            u8BoundLow = 63;
        }
        else // LVDS
        {
            u8EfuseData = msEread_GetDataFromEfuse(0x149);
            u8BoundHigh = 98;
            u8BoundLow = 55;
        }

        if(u8EfuseData&BIT7)  //Trimming flag
        {
            u8EfuseData = (u8EfuseData & 0x7F);
            if ((u8EfuseData>u8BoundHigh)||(u8EfuseData<u8BoundLow))
            {
                drvmStar_printMsg("Efuse value is out of range");
                drvmStar_printMsg("Restore the typical Icon value");
                i = 82; // If Efuse data is not reasonable, set the typical value
                u8Status = FALSE;
            }
            else
            {
                i = u8EfuseData;
                u8Status = TRUE;
            }
        }
        else
        {
            i = 82;
            u8Status = FALSE;
        }
        //Set DC level(Bias con)
        msWriteByteMask(REG_MOD1_F8, i, 0x7F); // [6:0]bias current control(bias current control)
        if(u8Status || (g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE))
        {
            msDrvMODSwingConfig();
        }

    }
    else
    {
        BYTE u8EfuseData = msEread_GetDataFromEfuse(0x148);

        if(u8EfuseData&BIT7)  //Trimming flag
        {
            u8EfuseData = (u8EfuseData & 0x3F);
            if((u8EfuseData>21)||(u8EfuseData<10))
            {
                drvmStar_printMsg("Efuse value is out of range");
                drvmStar_printMsg("Restore the typical Icon value");
                i = 15; // If Efuse data is not reasonable, set the typical value
                u8Status=FALSE;
            }
            else
            {
                i = u8EfuseData;
                u8Status=TRUE;
            }
        }
        else
        {
            i = 15;
            u8Status=FALSE;
        }
        //Set RCON current
        for (j = 0; j < 12; j++)
            msWrite2ByteMask(REG_MOD2_C0+(j*2), (((WORD)i<<8)&0x3F00)|(i&0x3F), 0x3F3F); //REG_1406C0~D7:[5:0]reg_gcr_rcon_chxx(Control swing of channel xx)

        //Set VBY1 SWING
        i=VBY1_SWING_404;
        for (j = 0; j < 5; j++)
            msWriteByteMask(REG_MOD2_82+(j), ((BYTE)(i<<4)|(i<<0)), 0xFF); //REG_140682~8D[7:4][3:0]vby1 vreg channel X
    }


    if(u8Status || (g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_NONE))
    {
        iGenTuningFinished = 1;
        drvmStar_printMsg("HW MOD Cali Pass !!");
    }
    else
    {
        iGenTuningFinished = 0;
        drvmStar_printMsg("HW MOD Cali Fail !!");
    }

    drvmStar_printData("I_gen_tuning = %x",i);
    
#endif
}

void msDrvMODsettingForDivision(void)
{
    if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_LR )
    {
        //PANEL_OUTPUT_FMT_LR
        //------- MFT SETTING, st --------//
        msWrite2ByteMask(REG_MFT_44, g_sPnlInfo.sPnlTiming.u16Width, 0x1FFF);
        msWrite2ByteMask(REG_MFT_4C, g_sPnlInfo.sPnlTiming.u16Width/2, 0x1FFF);
        msWriteByteMask(REG_MFT_40, 0x02, 0x07);

        //------- FREE SWAP, ST --------//
        if( PANEL_VBY1_4CH() )
        {
            msWrite2Byte(REG_MOD2_02, 0x3210);
        }
        else if( PANEL_VBY1_8CH() )
        {
            msWrite2Byte(REG_MOD2_02, 0x5410);
            msWrite2Byte(REG_MOD2_04, 0x6732);
        }
    }
    else if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_4B )
    {
        //PANEL_OUTPUT_FMT_4B
        msWrite2ByteMask(REG_MFT_44, g_sPnlInfo.sPnlTiming.u16Width, 0x1FFF);
        msWrite2ByteMask(REG_MFT_4C, g_sPnlInfo.sPnlTiming.u16Width/4, 0x1FFF);
        msWrite2ByteMask(REG_MFT_46, g_sPnlInfo.sPnlTiming.u16Width/4, 0xFFF);
        msWriteByteMask(REG_MFT_40, 0x03, 0x07);
    }
    else if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_8B )
    {
        //PANEL_OUTPUT_FMT_8B
        msWrite2ByteMask(REG_MFT_44, g_sPnlInfo.sPnlTiming.u16Width, 0x1FFF);
        msWrite2ByteMask(REG_MFT_4C, g_sPnlInfo.sPnlTiming.u16Width/8, 0x1FFF);
        msWrite2ByteMask(REG_MFT_46, g_sPnlInfo.sPnlTiming.u16Width/8, 0xFFF);
        msWrite2ByteMask(REG_MFT_48, g_sPnlInfo.sPnlTiming.u16Width*2/8, 0xFFF);
        msWrite2ByteMask(REG_MFT_4A, g_sPnlInfo.sPnlTiming.u16Width*3/8, 0xFFF);
        msWriteByteMask(REG_MFT_40, 0x04, 0x07);
    }
}
//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msDrvPowerModCtrl()
/// @brief \b Function \b Description: switch on/off MOD channel
/// @param <IN>        \b ucSwitch: enable or disable channels
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msDrvPowerModCtrl(BYTE ucSwitch)
{
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
        msWriteByteMask(REG_MOD1_51,0,BIT2);  // reg_test_mod
    else
        msWriteByteMask(REG_MOD1_51,BIT2,BIT2);  // reg_test_mod

    MAsm_CPU_DelayMs(1);

    if (ucSwitch)
    {
        if( g_sPnlInfo.ePnlTypeEdp != EN_PNL_EDP_ENABLE )
        {
            msWriteByteMask(REG_MOD1_4E,BIT6|BIT5,BIT6|BIT5); //CLK enable
            msWriteByteMask(REG_MOD2_80,0x0F,0x0F); //bank 0~3 CLK enable

            msWriteByteMask(REG_MOD1_50,0,BIT0|BIT1); //disable power down mod atop
            msWriteByteMask(REG_MOD1_4E,0x7F,0x7F);
            msWrite2Byte(REG_MOD1_66, LVDS_XSWING_CH); // ch double swing
        }

        if( g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_1 )
        {
            //PANEL_LVDS_1CH
            #if 1
            msWrite2Byte(REG_MOD1_20, LVDS_2CH_B_OUT); //demo board use B port
            #else
            msWrite2Byte(REG_MOD1_20, (LVDS_2CH_A_OUT&0x0F)<<12);
            msWrite2Byte(REG_MOD1_22, (LVDS_2CH_A_OUT>>4));
            #endif
        }
        else if( g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_2 )
        {
            //PANEL_LVDS_2CH
            msWrite2Byte(REG_MOD1_20, (((LVDS_2CH_A_OUT&0x0F)<<12)|LVDS_2CH_B_OUT));
            msWrite2Byte(REG_MOD1_22, (((LVDS_2CH_C_OUT&0xFF)<<8)|(LVDS_2CH_A_OUT>>4)));
            msWrite2Byte(REG_MOD1_24, ((LVDS_2CH_D_OUT<<4)|(LVDS_2CH_C_OUT>>8)));
        }
        else if( PANEL_VBY1_1CH() )
        {
            //PANEL_VBY1_1CH_8Bit||PANEL_VBY1_1CH_10Bit
            msWrite2Byte(REG_MOD1_20, 0x0001); //CH1
        }
        else if( PANEL_VBY1_2CH() )
        {
            //PANEL_VBY1_2CH_8Bit||PANEL_VBY1_2CH_10Bit
            msWrite2Byte(REG_MOD1_20, 0x0014); //CH1,2
        }
        else if( PANEL_VBY1_4CH() )
        {
            //PANEL_VBY1_4CH_8Bit||PANEL_VBY1_4CH_10Bit
            msWrite2Byte(REG_MOD1_20, 0x0154); //CH1~4
        }
        else if( PANEL_VBY1_8CH() )
        {
            //PANEL_VBY1_8CH_8Bit||PANEL_VBY1_8CH_10Bit
            msWrite2Byte(REG_MOD1_20, 0x5554); //CH1~7
            msWrite2Byte(REG_MOD1_22, 0x0004); //CH9
        }

        msDrvMODsettingForDivision();
    }
    else
    {
        if( g_sPnlInfo.ePnlTypeEdp != EN_PNL_EDP_ENABLE )
        {
            msWrite2Byte(REG_MOD1_20, 0x0000);
            msWrite2Byte(REG_MOD1_22, 0x0000);
            msWrite2Byte(REG_MOD1_24, 0x0000);
            msWriteByteMask(REG_MOD1_50,BIT0|BIT1,BIT0|BIT1);
            msWriteByteMask(REG_MOD1_4E,0x00,0x7F);
            msWriteByteMask(REG_MOD2_80,0x00,0x0F);
            msWriteByteMask(REG_MOD1_4E,0x00,BIT5); //MLoad OP trigger needs REG_MOD1_4E[6] = 1(Enable LPLL)
        }
    }
    msWriteByteMask(REG_MOD1_51,0x00,BIT2);  // reg_test_mod
    MAsm_CPU_DelayMs(1);
}


BYTE mStar_ScalerDoubleBuffer(Bool u8Enable)
{
    u8Enable=0;
    return u8Enable;
}

void msDrvSetDClkPLL(DWORD u32ODCLK_KHZ)
{
    BYTE  uctemp = 0, ucLPLL_Type = 0;
    DWORD u32Factor, u32MPLL_KHZ;
    float u32i_gain;
    float u32p_gain;
    BYTE  u8Div_M = 0;
    DWORD u32ExtFactor = 0;

    u32MPLL_KHZ = 864000UL;  // 864MHz

    //PANEL_LVDS_1CH
    if( g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_1 )
    {
        if( u32ODCLK_KHZ <= 50*1000UL )    // 37.5MHz~50MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to50MHz;
        }
        else if( u32ODCLK_KHZ <= 75*1000UL )   // 50MHz~75MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_50to75MHz;
        }
        else if( u32ODCLK_KHZ <= 100*1000UL )   // 75MHz~100MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to100MHz;
        }
        else    // 100MHz~150MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_100to150MHz;
        }
    }
    else if( g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_2 )
    {
        //PANEL_LVDS_2CH
        if( u32ODCLK_KHZ <= 125*1000UL )    // 75MHz~125MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to125MHz;
        }
        else if( u32ODCLK_KHZ <= 200*1000UL )   // 125MHz~200MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_125to200MHz;
        }
        else     // 200MHz~300MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_200to300MHz;
        }
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_10BIT )
    {
        //PANEL_VBY1_2CH_10Bit
        u32ODCLK_KHZ *= 2;
        ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to300MHz;
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_10BIT )
    {
        //PANEL_VBY1_4CH_10Bit
        if( u32ODCLK_KHZ <= 150*1000UL )    // 75MHz~150MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to150MHz;
        }
        else    // 150MHz~400MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_150to400MHz;
        }
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_10BIT )
    {
        //PANEL_VBY1_8CH_10Bit
        if( u32ODCLK_KHZ <= 150*1000UL )    // 75MHz~150MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to150MHz;
        }
        else if( u32ODCLK_KHZ <= 300*1000UL )   // 150MHz~300MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_150to300MHz;
        }
        else if( u32ODCLK_KHZ <= 400*1000UL )   // 300MHz~400MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_300to400MHz;
        }
        else    // 300MHz~520MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_300to520MHz;
        }
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_8BIT )
    {
        //PANEL_VBY1_2CH_8Bit
        u32ODCLK_KHZ *= 2;
        if( u32ODCLK_KHZ <= 200*1000UL )    // 150MHz~200MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to200MHz;
        }
        else    // 200MHz~300MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_200to300MHz;
        }
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_8BIT )
    {
        //PANEL_VBY1_4CH_8Bit
        if( u32ODCLK_KHZ <= 100*1000UL )    // 75MHz~100MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to100MHz;
        }
        else if( u32ODCLK_KHZ <= 200*1000UL )    // 100MHz~200MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_100to200MHz;
        }
        else    // 200MHz~400MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_200to400MHz;
        }
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_8BIT )
    {
        //PANEL_VBY1_8CH_8Bit
        if( u32ODCLK_KHZ <= 100*1000UL )    // 75MHz~100MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to100MHz;
        }
        else if( u32ODCLK_KHZ <= 200*1000UL )    // 100MHz~200MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_100to200MHz;
        }
        else if( u32ODCLK_KHZ <= 400*1000UL )   // 200MHz~400MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_200to400MHz;
        }
        else    // 200MHz~520MHz
        {
            ucLPLL_Type = E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_200to520MHz;
        }
    }
#if PANEL_EDP
    else if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        //PANEL_EDP // TBD
        if( g_sPnlInfo.u8PnlEdpLinkRate == eDPTX_HBR )
        {
            if( u32ODCLK_KHZ <= 25*1000UL ) // <25MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_12_5to25MHz;
            }
            else if( u32ODCLK_KHZ <= 375*100UL )   // 25MHz~37.5MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_25to37_5MHz;
            }
            else if( u32ODCLK_KHZ <= 75*1000UL )   //37.5MHz~75MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_37_5to75MHz;
            }
            else if( u32ODCLK_KHZ <= 150*1000UL )   //75MHz~150MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_75to150MHz;
            }
            else // 150Mhz~400Mhz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_150to400MHz;
            }
        }
        else
        {
            if( u32ODCLK_KHZ <= 25*1000UL ) // <25MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_12_5to25MHz;
            }
            else if( u32ODCLK_KHZ <= 375*100UL )   // 25MHz~37.5MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_25to37_5MHz;
            }
            else if( u32ODCLK_KHZ <= 75*1000UL )   //37.5MHz~75MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_37_5to75MHz;
            }
            else if( u32ODCLK_KHZ <= 150*1000UL )   //75MHz~150MHz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_75to150MHz;
            }
            else // 150Mhz~400Mhz
            {
                ucLPLL_Type = E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_150to400MHz;
            }
        }

        if( g_sPnlInfo.bPnlEdpFineTuneTUEn )
            mapi_eDPTx_FineTuneTU(0,u32ODCLK_KHZ);
    }
#endif

#if PANEL_EDP
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        u8LPLL_LOOP_GAIN = u8EXT_LoopGain[ucLPLL_Type];
        fLPLL_LOOP_DIV = u8EXT_LoopDiv[ucLPLL_Type];
    }
    else
#endif
    {
        u8LPLL_LOOP_GAIN = u8LoopGain[ucLPLL_Type];
        fLPLL_LOOP_DIV = u8LoopDiv[ucLPLL_Type];
    }

#if 1//FRAME_BFF_SEL==FRAME_BUFFER
    u8Div_M = msDrvSetFrameDivider(V_FREQ_IN);
    msDrvSetIVS();
#endif

#ifdef TSUMR2_FPGA
    if(g_sPnlInfo.sPnlTiming.u16Width==800&&g_sPnlInfo.sPnlTiming.u16Height==600)
        u32Factor = (float)(0x6C0000);//For 800x600 panel
    else
        u32Factor = (float)(0xA10000);//For 640x480 panel
    u8LPLL_LOOP_GAIN = u8LPLL_LOOP_GAIN;
    fLPLL_LOOP_DIV = fLPLL_LOOP_DIV;
    u32ODCLK_KHZ = u32ODCLK_KHZ;
    ucLPLL_Type = ucLPLL_Type;
    uctemp = uctemp;
#else
    //pll_synth_tmp = (864*524288*16*loop_gain)/(odclk * loop_div);
    u32Factor = (float)((float)u32MPLL_KHZ*16/fLPLL_LOOP_DIV)*((float)524288UL*(float)u8LPLL_LOOP_GAIN/u32ODCLK_KHZ);
    for(uctemp = 0; uctemp <LPLL_REG_NUM; uctemp++)
    {
        msWrite2ByteMask((REG_LPLL_BASE+(LPLLSettingTBL[ucLPLL_Type][uctemp].address)*2), LPLLSettingTBL[ucLPLL_Type][uctemp].value, LPLLSettingTBL[ucLPLL_Type][uctemp].mask);
    }
#endif
    msWriteBit(REG_LPLL_18,FALSE,_BIT3); // Frame pll enable


    if( (u32ODCLK_KHZ > 400000) && PANEL_VBY1_8CH() )
    {
        u32Factor = (float)((float)u32MPLL_KHZ*16/fLPLL_LOOP_DIV)*((float)524288UL*(float)u8LPLL_LOOP_GAIN/400000); // set scaler to max clk 400M
        u32ExtFactor = (float)((float)u32MPLL_KHZ*16/u8EXT_LoopDiv[ucLPLL_Type])*((float)524288UL*(float)u8EXT_LoopGain[ucLPLL_Type]/u32ODCLK_KHZ);
        msWrite4Byte(REG_LPLL_1E, u32Factor); //SET Value
        msWrite4Byte(REG_LPLL_90, u32ExtFactor); //SET Value of LPLL_EXT
        msWriteByteMask(REG_CLKGEN_82, BIT1, BIT1);
        msWriteByteMask(REG_LPLL_D4, _BIT0, BIT0);
    }
    else
    {
        if( PANEL_VBY1_8CH() )
        {
            msWriteByteMask(REG_CLKGEN_82, 0x00, BIT1);
            msWriteByteMask(REG_LPLL_D4, 0x00, BIT0);
        }
        msWrite4Byte(REG_LPLL_1E, u32Factor); //SET Value
    }
    msWrite2Byte(SC00_D5, u32Factor>>4); // formula of scaler set value doesn't need to multiply by 16
    msWriteByte(SC00_D7, u32Factor>>20);

#if PANEL_EDP
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE ) //Set eDP link rate
    {
        u8LPLL_LOOP_GAIN = u8LoopGain[ucLPLL_Type];
        fLPLL_LOOP_DIV = u8LoopDiv[ucLPLL_Type];
        if( g_sPnlInfo.u8PnlEdpLinkRate == eDPTX_RBR )
            u32Factor = (float)((float)u32MPLL_KHZ*16/fLPLL_LOOP_DIV)*((float)524288UL*(float)u8LPLL_LOOP_GAIN/162000);
        else
            u32Factor = (float)((float)u32MPLL_KHZ*16/fLPLL_LOOP_DIV)*((float)524288UL*(float)u8LPLL_LOOP_GAIN/270000);
        msWrite4Byte(REG_LPLL_90,u32Factor);//SET Value
        // Restore loop gain/div of pixel clock
        u8LPLL_LOOP_GAIN = u8EXT_LoopGain[ucLPLL_Type];
        fLPLL_LOOP_DIV = u8EXT_LoopDiv[ucLPLL_Type];
    }
#endif

    //Set i,p gain
    if( (u32ODCLK_KHZ > 400000) && PANEL_VBY1_8CH() )
        u32i_gain = ( float )((( float ) u8EXT_LoopGain[ucLPLL_Type]*36*524288)/(( DWORD )g_sPnlInfo.sPnlTiming.u16Htt*g_sPnlInfo.sPnlTiming.u16Vtt*(u8Div_M+1)*8*u8EXT_LoopDiv[ucLPLL_Type]));
    else
        u32i_gain = ( float )((( float ) u8LPLL_LOOP_GAIN*36*524288)/(( DWORD )g_sPnlInfo.sPnlTiming.u16Htt*g_sPnlInfo.sPnlTiming.u16Vtt*(u8Div_M+1) *8 *fLPLL_LOOP_DIV));

    u32i_gain = GetLog(u32i_gain*1024) -5;
    u32p_gain = u32i_gain + 1;
    msWriteByteMask(REG_LPLL_16,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain
    msWriteByteMask(REG_LPLL_16,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain
    msWriteByteMask(REG_LPLL_17,(BYTE)u32i_gain       ,BIT0|BIT1|BIT2|BIT3);//i_gain_phase
    msWriteByteMask(REG_LPLL_17,((BYTE)u32p_gain)<<4  ,BIT4|BIT5|BIT6|BIT7);//p_gain_phase

    if( (g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE) || (g_sPnlInfo.ePnlTypeVby1 != EN_PNL_VBY1_NONE) )
    {
        DWORD u32LimitFreqPhaseCorrection;
        if( (u32ODCLK_KHZ > 400000) && PANEL_VBY1_8CH() )
        {
            u32LimitFreqPhaseCorrection = ((float)u32ExtFactor * FPLL_TUNE_LIMIT_HW_VX1) /10000;
        }
        else
        {
            if( g_sPnlInfo.ePnlTypeVby1 != EN_PNL_VBY1_NONE )
                u32LimitFreqPhaseCorrection = ((float)u32Factor * FPLL_TUNE_LIMIT_HW_VX1) /10000;
            else if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
                u32LimitFreqPhaseCorrection = ((float)u32Factor * FPLL_TUNE_LIMIT_HW_EDP) /10000;
            else
                u32LimitFreqPhaseCorrection = ((float)u32Factor * FPLL_TUNE_LIMIT_HW) /10000;
        }

        msWrite3Byte(REG_LPLL_0C, u32LimitFreqPhaseCorrection);
        msWrite3Byte(REG_LPLL_10, u32LimitFreqPhaseCorrection);
    }
}

void drvmStar_SetupFreeRunMode( void )
{
    //mStar_ScalerDoubleBuffer(FALSE);
    msWriteByteMask( SC00_02, BIT7, BIT3|BIT7 ); // disable output Lock mode to enable free run.
    msWriteByteMask(SC00_27,BIT6,BIT6|BIT1);
    msWrite2ByteMask(SC00_1E, g_sPnlInfo.sPnlTiming.u16Vtt-1,SC_MASK_V); // set output panel vtotal
    msWrite2Byte(SC00_24, g_sPnlInfo.sPnlTiming.u16Htt-1);
    msWriteByteMask(SC00_63,BIT0,BIT1|BIT0);
    msWriteByteMask(SC00_28,BIT6,BIT6);
    msWriteByteMask(SC00_28,BIT3,BIT3);
    msWriteByteMask(SC00_9F,0,BIT0);
    msDrvSetDClkPLL(g_sPnlInfo.sPnlTiming.u16DClk*1000UL); //White101208 must use 1000UL to provent overflow truncate
}

void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun)
{
    WORD uwV_total_OP22; // output vtotal OP2-2
    WORD uwV_total_OP21; // output vtotal OP2-1

    uwV_total_OP22 = g_sPnlInfo.sPnlTiming.u16Vtt;//GetVtotal(); // output V total
    uwV_total_OP21 = (g_sPnlInfo.sPnlTiming.u16Vtt-g_sPnlInfo.sPnlTiming.u16Height+u16VDE_OP21);
    if ( ((g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_1) && (!OP21_FreeRun)) || (!g_SetupPathInfo.bFBMode)) //case 12, FBL
    {
        msWrite2Byte(SC00_10, 0x00 + PANEL_SHIFT_DE_V); //panel V. start
        msWrite2ByteMask(SC00_14, ( (u16VDE_OP21) - 1 ) + PANEL_SHIFT_DE_V, 0x7FF); //panel Vert. DE end
        msWrite2ByteMask(SC00_16, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + ((u16HDE_OP21)/PANEL_H_DIV) - 1 ), 0xFFF); //panel Hor. DE end
        msWrite2ByteMask(SC00_1A, ( (u16VDE_OP21) - 1 ), 0x7FF); //image V. end
        msWrite2ByteMask(SC00_20, ( uwV_total_OP21 - g_sPnlInfo.sPnlTiming.u16VStart ), 0x7FF); // vsync start
        msWrite2ByteMask(SC00_22, ( uwV_total_OP21 - g_sPnlInfo.sPnlTiming.u16VSyncBP ), 0xFFF); //vsync end
#if Enable_Expansion
        if( OverScanSetting.Enable )
        {
            WORD offset = (g_sPnlInfo.sPnlTiming.u16Height - OverScanSetting.OverScanV);
            msWrite2ByteMask(SC00_18, ( ( g_sPnlInfo.sPnlTiming.u16HStart + ( g_sPnlInfo.sPnlTiming.u16Width-OverScanSetting.OverScanH )/2 )/1/*Hdivider*/ ), 0xFFF);
            msWrite2ByteMask(SC00_1C, ( ( ( g_sPnlInfo.sPnlTiming.u16HStart + ( g_sPnlInfo.sPnlTiming.u16Width-OverScanSetting.OverScanH )/2 + OverScanSetting.OverScanH )/1/*Hdivider*/ ) - 1 ), 0xFFF);

            if( OverScanSetting.OverScanV < g_sPnlInfo.sPnlTiming.u16Height )
            {
                msWrite2Byte(SC00_10, (uwV_total_OP22 - (offset/2) - 1 ) + PANEL_SHIFT_DE_V);
                msWrite2Byte(SC00_14, (g_sPnlInfo.sPnlTiming.u16Height - (offset/2) - 1) + PANEL_SHIFT_DE_V);
                msWrite2Byte(SC00_1A, (g_sPnlInfo.sPnlTiming.u16Height - offset - 1));
                msWrite2ByteMask(SC00_20, ( uwV_total_OP22 - (offset/2) - 1 - 3 ), 0x7FF); // vsync start
                msWrite2ByteMask(SC00_22, ( uwV_total_OP22 - (offset/2) - 1 - 2 ), 0xFFF); // vsync end
            }
        }
        else //restore
#endif
        {
            msWrite2ByteMask(SC00_18, ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ), 0xFFF); //image H. start
            msWrite2ByteMask(SC00_1C, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + ((u16HDE_OP21)/PANEL_H_DIV) - 1 ), 0xFFF); //image H. end
        }
    }
    else // Case 3456, FB free run
    {
        msWrite2Byte(SC00_10, 0x00 + PANEL_SHIFT_DE_V);
        msWrite2ByteMask(SC00_14, ( g_sPnlInfo.sPnlTiming.u16Height - 1 )+ PANEL_SHIFT_DE_V, 0x7FF); // Vert. DE end
        msWrite2ByteMask(SC00_1A, ( g_sPnlInfo.sPnlTiming.u16Height - 1 ), 0x7FF);

        msWrite2ByteMask(SC00_16, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + (g_sPnlInfo.sPnlTiming.u16Width/PANEL_H_DIV) - 1 ), 0xFFF); // Hor. DE end
        msWrite2ByteMask(SC00_20, ( uwV_total_OP22 - g_sPnlInfo.sPnlTiming.u16VStart ), 0x7FF); // vsync start
        msWrite2ByteMask(SC00_22, ( uwV_total_OP22 - g_sPnlInfo.sPnlTiming.u16VSyncBP ), 0xFFF); //vsync end

        msWrite2ByteMask(SC00_18, ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ), 0xFFF);
        msWrite2ByteMask(SC00_1C, ( ( g_sPnlInfo.sPnlTiming.u16HStart/PANEL_H_DIV ) + (g_sPnlInfo.sPnlTiming.u16Width/PANEL_H_DIV) - 1 ), 0xFFF);


        if(OverScanSetting.Enable && (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL))
        {
            WORD offset = (g_sPnlInfo.sPnlTiming.u16Height - OverScanSetting.OverScanV);
            msWrite2ByteMask(SC00_18, ( ( g_sPnlInfo.sPnlTiming.u16HStart + ( g_sPnlInfo.sPnlTiming.u16Width-OverScanSetting.OverScanH )/2 )/PANEL_H_DIV ), 0xFFF);
            msWrite2ByteMask(SC00_1C, ( ( ( g_sPnlInfo.sPnlTiming.u16HStart + ( g_sPnlInfo.sPnlTiming.u16Width-OverScanSetting.OverScanH )/2 + OverScanSetting.OverScanH )/PANEL_H_DIV ) - 1 ), 0xFFF);
            if( OverScanSetting.OverScanV < g_sPnlInfo.sPnlTiming.u16Height )
            {
                msWrite2Byte(SC00_10, (uwV_total_OP22 - (offset/2) - 1 ));
                msWrite2Byte(SC00_14, (g_sPnlInfo.sPnlTiming.u16Height - (offset/2) - 1));
                msWrite2Byte(SC00_1A, (g_sPnlInfo.sPnlTiming.u16Height - offset - 1));
                msWrite2ByteMask(SC00_20, ( uwV_total_OP22 - (offset/2) - 1 - 3 ), 0x7FF); // vsync start
                msWrite2ByteMask(SC00_22, ( uwV_total_OP22 - (offset/2) - 1 - 2 ), 0xFFF); // vsync end
            }
        }
    }
}

void msSetFrameSyncMode(FSyncMode ucFrameLockMode, WORD height, WORD u16HTotal, DWORD u32ODCLK_KHZ)
{
    WORD u16Height;

    u16Height = height;
    switch(ucFrameLockMode)
    {
        case FLM_FBL:
        {
            DWORD u16VTotalMax;

            u16VTotalMax = (u32ODCLK_KHZ*1000)/(u16HTotal*(g_sPnlInfo.sPnlTiming.u16VFreqMin/10)) + 1;
            if(u16VTotalMax > g_sPnlInfo.sPnlTiming.u16VttMax)
                u16VTotalMax = g_sPnlInfo.sPnlTiming.u16VttMax;
            msWrite2ByteMask(SC00_1E, ((0xFFF > (u16VTotalMax-1))?(u16VTotalMax-1):0xFFF),SC_MASK_V);
            #if !(DISABLE_SHORT_FRAME && !DISABLE_SHORT_LINE_TUNE) // only enable SLT when SF=disable, SLT=enable
            // disable short line tune
            msWriteByteMask(SC00_63,BIT0,BIT1|BIT0);
            msWriteByteMask(SC00_28,0,BIT6);
            #endif

            #if DISABLE_SHORT_FRAME
            msWriteByteMask(SC00_63,BIT2,BIT3|BIT2);
            #endif
            msWriteByteMask(SC00_28,0,BIT3);
            msWriteByteMask(SC00_27,BIT6|BIT1,BIT6|BIT1);
            msWrite2Byte(SC00_24,u16HTotal -1);
            msWriteByteMask(SC00_9F,0,BIT0);
            break;
        }

        case FLM_FBL_FRAMEPLL:
            msWrite2ByteMask(SC00_1E, ((0xFFF > (g_sPnlInfo.sPnlTiming.u16VttMax-1))?(g_sPnlInfo.sPnlTiming.u16VttMax-1):0xFFF),SC_MASK_V);
            msWriteByteMask(SC00_63,BIT0,BIT1|BIT0);
            msWriteByteMask(SC00_28,0,BIT6);
            msWriteByteMask(SC00_28,BIT3,BIT3);
            msWriteByteMask(SC00_27,BIT6,BIT6|BIT1);
            msWrite2Byte(SC00_24,(g_sPnlInfo.sPnlTiming.u16HttMin + 10) -1);
            msWriteByteMask(SC00_9F,BIT0,BIT0);
            //msWriteByteMask(REG_LPLL_18,BIT3,BIT3);
            break;

        case FLM_FB_FPLL_NOLOCK:
            msWrite2ByteMask(SC00_1E, g_sPnlInfo.sPnlTiming.u16Vtt-1,SC_MASK_V);
            msWriteByteMask(SC00_63,BIT0,BIT1|BIT0);
            msWriteByteMask(SC00_28,0,BIT6);
            msWriteByteMask(SC00_28,BIT3,BIT3);
            msWriteByteMask(SC00_27,BIT6,BIT6|BIT1);
            msWrite2Byte(SC00_24,g_sPnlInfo.sPnlTiming.u16Htt-1);
            msWriteByteMask(SC00_9F,BIT0,BIT0);
            msWriteByteMask(REG_LPLL_18,0,BIT3);
            break;

        case FLM_FB_FPLL:
            msWrite2ByteMask(SC00_1E, g_sPnlInfo.sPnlTiming.u16Vtt-1,SC_MASK_V);
            msWriteByteMask(SC00_63,BIT0,BIT1|BIT0);
            msWriteByteMask(SC00_28,0,BIT6);
            msWriteByteMask(SC00_28,BIT3,BIT3);
            msWriteByteMask(SC00_27,BIT6,BIT6|BIT1);
            msWrite2Byte(SC00_24,g_sPnlInfo.sPnlTiming.u16Htt-1);
            msWriteByteMask(SC00_9F,BIT0,BIT0);
            msWriteByteMask(REG_LPLL_18,BIT3,BIT3);
            //TIME_MEASURE_BEGIN();
            break;

        default:
            break;
    }

}

#if 1//FRAME_BFF_SEL==FRAME_BUFFER
#define MIN_VFREQ_DIFF      2
BYTE msDrvSetFrameDivider( BYTE vfreq_N )
{
    ////////////////////////////////
    //  FRC ratio N/M ==> (N-1), (M-1)//
    ////////////////////////////////
    BYTE u8Index_N;
    BYTE u8Index_M;
    BYTE u8Div_N=1; // input divider
    BYTE u8Div_M=1; // output divider
    BYTE u8VFreq_M; // output clock
    DWORD u32MinDiff; // minimum differencec
    BYTE u8Index_N_step;
    //BOOL bFRC = FALSE;

    u8VFreq_M = GetVfreq();
    if((g_SetupPathInfo.ucFrameLockMode == FLM_FBL_FRAMEPLL)
#if FRAME_BFF_SEL==FRAME_BUFFER
        || (labs(u8VFreq_M - vfreq_N)> MIN_VFREQ_DIFF)
#endif
       )
    {
        //printData( "input VFreq_N = %d", vfreq_N);
        //printData( "output VFreq_M = %d", u8VFreq_M);
        u8Index_N_step = (SrcFlags&bInterlaceMode)?2:1;
        for(u32MinDiff=0xFFFFFFFF,u8Index_N=u8Index_N_step;u8Index_N<=32;u8Index_N+=u8Index_N_step)
        {
            for(u8Index_M=1;u8Index_M<=32;u8Index_M++)
            {
                if((u8Index_N*u8VFreq_M) == (u8Index_M*vfreq_N))
                {
                    u8Div_N   = u8Index_N;
                    u8Div_M   = u8Index_M;
                    u8Index_N = 33; //break 1st loop
                    break;
                }
                else if( labs(((DWORD)(u8Index_N<<16)/u8Index_M) - ((DWORD)(vfreq_N<<16)/u8VFreq_M)) < u32MinDiff)
                {
                    u8Div_N    = u8Index_N;
                    u8Div_M    = u8Index_M;
                    u32MinDiff = labs(((DWORD)(u8Index_N<<16)/u8Index_M) - ((DWORD)(vfreq_N<<16)/u8VFreq_M));
                }
            }
        }
    }
    u8Div_N = u8Div_N-1;
    u8Div_M = u8Div_M-1;

    //if( u8Div_N != u8Div_M )
    //    bFRC = TRUE;

    drvmStar_printData( "Set input V_divier_N = %d", u8Div_N);
    drvmStar_printData( "Set output V_divier_M = %d", u8Div_M);

    msWriteByte(REG_LPLL_19, (u8Div_M<<4)|(u8Div_N&0x0F)); //[3:0] input frame div for frame sync, [7:4] output frame div for frame sync
    msWriteByteMask(REG_LPLL_1C, ((u8Div_M&BIT4)>>3)|(u8Div_N>>4),BIT1|BIT0); //[0] input frame div[4], [1] output frame div [4]

    return u8Div_M;
}
#endif
BYTE GetVfreq( void )
{
    if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF ) // Vout = Vin/2
        return (V_FREQ_IN + 1)/2;
    else if( g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI )// Vout = Vin*2
        return V_FREQ_IN*2;
    else if (g_SetupPathInfo.bFBMode) // FB, Output 60/Vfreq-in double/Vfreq-in triple
    {
        if(IS_INPUT_VFREQ_DOUBLE_IN_PANEL_RANGE())
                return (V_FREQ_IN*2);
            else if(IS_INPUT_VFREQ_TRIPLE_IN_PANEL_RANGE())
                return (V_FREQ_IN*3);
            else
                return 60;
    }
    else // Vout = Vin
    {
        return V_FREQ_IN;
    }
}

void msDrvSetIVS( void )
{
    XDATA WORD ivs = 0, Sample_vst;
    //WORD Input_Vtt = SC0_READ_VTOTAL();

    Sample_vst = (msRead2Byte( SC00_05 ) & SC_MASK_V)/((SrcFlags&bInterlaceMode)?2:1);
    if(g_SetupPathInfo.ucFrameLockMode == FLM_FBL_FRAMEPLL) // adjust input phase
    {
        ivs = Sample_vst + 4; // sample Vst + delay line
    }
#if FRAME_BFF_SEL==FRAME_BUFFER
    else
    {
        if(g_SetupPathInfo.bFBMode)
        {
#if 0
            if( (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_TB)
                     && (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV) )
            {
                ivs = IVS_DEFAULT + g_SetupPathInfo.wImgSizeInV + Sample_vst;
            }
            else
            {
                ivs = IVS_DEFAULT + Sample_vst;
            }
#endif
            ivs = Sample_vst + 10*GetVfreq()/V_FREQ_IN;
        }
        else
            ivs = 4 + Sample_vst;
    }
#endif

    msWrite2ByteMask(SC00_B8, ivs<<4, 0xFFF0);//IVS:{SC0_A6[0],SC0_B9[7:0],SC0_B8[7:4]}
    msWriteByteMask(SC00_A6, ivs>>12, BIT0);
}

void drvmStar_SetupInputPort_VGA(void)
{
    old_msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 );                   //DP Port B/C Off Line
    drvADC_SetInputMux(Input_VGA);
    mStar_SetAnalogInputPort( FALSE );    // switch to RGB port by default

}
#if 0//ENABLE_HDMI
void drvmStar_SetupInputPort_DVI(void)
{
    old_msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 );                     //DP Port B/C Off Line
#if (DVI_RB_SWAP)
    old_msWriteByteMask(REG_1600,_BIT5,_BIT5);
#endif
#if (DVI_PN_SWAP)
    old_msWriteByteMask(REG_1600,0x0F,0x0F);
#endif
    msTMDSSetMux( SrcInputType );
}

void drvmStar_SetupInputPort_HDMI(void)
{
    old_msWriteByteMask( REG_06C2, BIT2|BIT3, BIT2|BIT3 );                   //DP Port B/C Off Line
#if (HDMI_RB_SWAP)
    old_msWriteByteMask(REG_1600,_BIT5,_BIT5);
#endif
#if (HDMI_PN_SWAP)
    old_msWriteByteMask(REG_1600,0x0F,0x0F);
#endif
    msTMDSSetMux( SrcInputType );
}
#endif

BYTE msDrvMapInputToCombo(BYTE u8Input)
{
    BYTE Result;

    // dead code in coverity error can't be eliminated because combo port index used in project may not be from samll to large(ex. only combo2 is used in INPUT_IC case) 
    if(u8Input == Input_Nothing)
        Result = 0xFF;
    else if( (u8Input == Input_DVI) || (u8Input == Input_HDMI) || (u8Input == Input_Displayport) )
        Result =  0;
    else if( (u8Input == Input_DVI2) || (u8Input == Input_HDMI2) || (u8Input == Input_Displayport2))
        Result =  1;
    else if( (u8Input == Input_DVI3) || (u8Input == Input_HDMI3) || (u8Input == Input_Displayport3) || (u8Input == Input_UsbTypeC3))
        Result =  0;
    else if( (u8Input == Input_DVI4) || (u8Input == Input_HDMI4) || (u8Input == Input_Displayport4) || (u8Input == Input_UsbTypeC4))
        Result =  3;
    else
        Result = 0xFF;

    return Result;
}

/*BYTE msDrvMapInputToComboIsr(BYTE u8Input)
{
    if( (u8Input == Input_DVI) || (u8Input == Input_HDMI) || (u8Input == Input_Displayport) )
        return 0;
    else if( (u8Input == Input_DVI2) || (u8Input == Input_HDMI2) || (u8Input == Input_Displayport2))
        return 1;
    else if( (u8Input == Input_DVI3) || (u8Input == Input_HDMI3) || (u8Input == Input_Displayport3) || (u8Input == Input_UsbTypeC3))
        return 2;
    else if( (u8Input == Input_DVI4) || (u8Input == Input_HDMI4) || (u8Input == Input_Displayport4) || (u8Input == Input_UsbTypeC4))
        return 3;
    else if((u8Input == Input_DualDVI) || (u8Input == Input_DualDVI2))
        return 4;

    else
        return 0xFF;
}*/


BYTE msDrvMapComboToPortIndex(BYTE u8Combo)
{
    BYTE XDATA u8PortIdx;

    if( u8Combo == 0 )
        u8PortIdx =  Input_DVI + Input_DualDVI + Input_HDMI + Input_Displayport;
    else if( u8Combo == 1 )
        u8PortIdx = Input_DVI2 + Input_DualDVI2 + Input_HDMI2 + Input_Displayport2;
    else if( u8Combo == 2 )
        u8PortIdx = Input_DVI3 + Input_HDMI3 + Input_Displayport3 + Input_UsbTypeC3;
    else if( u8Combo == 3 )
        u8PortIdx = Input_DVI4 + Input_HDMI4 + Input_Displayport4 + Input_UsbTypeC4;
    else
        return 0xFF; // return 0xFF for invalid combo index

    return (u8PortIdx % Input_Nothing);
}


void msDrvMODInit(void)
{
    BYTE i;

#ifdef TSUMR2_FPGA
    msWrite2Byte(0x101ECE, 0x0020);  //[5:4] phase sel
    msWriteByteMask(0x101ECE, 0x80, 0x80);  //[7] rx reset
    msWriteByteMask(0x101ECE, 0x00, 0x80);  //[7] rx reset
    msWriteByteMask(REG_MOD2_7C, BIT3, BIT3);  //disp_misc_a : [3] PN swap
    msWriteByteMask(REG_MOD2_7C, BIT4, BIT4);  //disp_misc_a : [4] sw reset
    msWriteByteMask(REG_MOD2_7C, 0, BIT4);     //disp_misc_a : [4] sw reset
#endif

    if( !iGenTuningFinished ) // set icon and swing from efuse berore init MOD to avoid overshoot
    {
        msDrvIGenTuning();
    }

    msWriteByteMask(REG_LPLL_06, 0x00, _BIT5);//reg_lpll1_pd

    if( g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_1 )
    {
        for( i = 0; i < sizeof( tblInitMODLvdsCh1 ) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODLvdsCh1[i].u32Reg, tblInitMODLvdsCh1[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_2 )
    {
        for( i = 0; i < sizeof( tblInitMODLvdsCh2 ) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODLvdsCh2[i].u32Reg, tblInitMODLvdsCh2[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_8BIT )
    {
        for( i = 0; i < sizeof( tblInitMODVby1Ch2Bit8) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODVby1Ch2Bit8[i].u32Reg, tblInitMODVby1Ch2Bit8[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_8BIT )
    {
        for( i = 0; i < sizeof( tblInitMODVby1Ch4Bit8) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODVby1Ch4Bit8[i].u32Reg, tblInitMODVby1Ch4Bit8[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_8BIT )
    {
        for( i = 0; i < sizeof( tblInitMODVby1Ch8Bit8) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODVby1Ch8Bit8[i].u32Reg, tblInitMODVby1Ch8Bit8[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_10BIT )
    {
        for( i = 0; i < sizeof( tblInitMODVby1Ch2Bit10) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODVby1Ch2Bit10[i].u32Reg, tblInitMODVby1Ch2Bit10[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_10BIT )
    {
        for( i = 0; i < sizeof( tblInitMODVby1Ch4Bit10) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODVby1Ch4Bit10[i].u32Reg, tblInitMODVby1Ch4Bit10[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_10BIT )
    {
        for( i = 0; i < sizeof( tblInitMODVby1Ch8Bit10) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODVby1Ch8Bit10[i].u32Reg, tblInitMODVby1Ch8Bit10[i].u16Value );
    }
    else if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        for( i = 0; i < sizeof( tblInitMODEdp) / sizeof( RegUnitType2 ); i++ )
            msWrite2Byte( tblInitMODEdp[i].u32Reg, tblInitMODEdp[i].u16Value );
    }

    if( g_sPnlInfo.ePnlTypeVby1 != EN_PNL_VBY1_NONE )
    {
        if( PANEL_VBY1_8CH() )
        {
            if( g_sPnlInfo.sPnlTiming.u16DClkMax > 400 )
                msWrite2Byte(REG_CLKGEN_82 ,0xC002);
            else
                msWrite2Byte(REG_CLKGEN_82 ,0xC000);
        }

        msWrite2Byte(REG_MFT_44 ,g_sPnlInfo.sPnlTiming.u16Width);
        msWrite2Byte(REG_MFT_4C ,g_sPnlInfo.sPnlTiming.u16Width);

        msWrite2Byte(REG_MOD1_46, (g_sPnlInfo.u8PnlSwapPn?(g_sPnlInfo.u8PnlVby1SwpPnLockn ? 0xFFFF : 0xFFFE):(g_sPnlInfo.u8PnlVby1SwpPnLockn ? 0x0001 : 0x0000))); //PN swap for VBY1
    }


    for( i = 0; i < sizeof( tblInitMOD2 ) / sizeof( RegTbl2Type ); i++ )
        msWrite2ByteMask( tblInitMOD2[i].dwReg, tblInitMOD2[i].wValue, tblInitMOD2[i].wMask );

    drvOSD_FrameColorEnable(TRUE);
    drvOSD_FrameColorRGB(0x00,0x00,0x00);
    //MOD2
    msWrite2ByteMask(REG_MOD1_00, (g_sPnlInfo.u8PnlInvDE&BIT7)|(g_sPnlInfo.u8PnlInvVSync&BIT6)|(g_sPnlInfo.u8PnlInvHSync&BIT5)   ,BIT7|BIT6|BIT5);
    msWrite2ByteMask(REG_MOD1_0A, g_sPnlInfo.u8PnlLvdsTiBitMode<<2  ,BIT3|BIT2);   //reg_ti_bitmode
    msWrite2ByteMask(REG_MOD1_0B, ((BIT6&g_sPnlInfo.u8PnlLvdsTiEn)|BIT5|(BIT3&g_sPnlInfo.u8PnlSwapPn)|(g_sPnlInfo.u8PnlSwapMlsb&BIT2))   ,BIT6|BIT5|BIT3|BIT2); //reg_pa_swap=0 &reg_PDP_10bit = 1 -> data0,data1,clk,data3,dat4
    msWrite2ByteMask(REG_MOD2_02, (g_sPnlInfo.ePnlLvdsChD<<12)|(g_sPnlInfo.ePnlLvdsChC<<8)|(g_sPnlInfo.ePnlLvdsChB<<4)|(g_sPnlInfo.ePnlLvdsChA) ,0xFFFF);
    msWrite2ByteMask(REG_MOD2_04, (g_sPnlInfo.ePnlLvdsChH<<12)|(g_sPnlInfo.ePnlLvdsChG<<8)|(g_sPnlInfo.ePnlLvdsChF<<4)|(g_sPnlInfo.ePnlLvdsChE) ,0xFFFF);
}

///////////////////////////////////////////////////////////////////////////////
// <Function>: msDrvCheckVBY1LockN
//
// <Description>: Check LockN value for VBY1 interface
//
// <Parameter>: None
//
///////////////////////////////////////////////////////////////////////////////
void msDrvCheckVBY1LockN(void)
{
    #define VX1_LOCKN_STATUS    (msReadByte(REG_MOD1_C3)&BIT6)
    #define LOCKN_TIMEOUT_COUNTER   500     // about 500ms
    #define LOCKN_STABLE_COUNTER    250     // about 80us
    bit bVby1CheckLockPass = FALSE;
    int chkLock;
    unsigned int u32TimeOutCnt;
    unsigned int u32Cnt;

    drvmStar_printMsg("VBY1 Training (HW start");

    u32TimeOutCnt = LOCKN_TIMEOUT_COUNTER;
    while( u32TimeOutCnt-- )
    {
        msWrite2Byte(REG_MOD1_C0, 0x0000);
        MAsm_CPU_DelayMs(1);
        msWrite2Byte(REG_MOD1_C0, 0x0AAE);

        if(VX1_LOCKN_STATUS)
        {
            u32Cnt = 10000;                             // Polling lockN status maximun 10ms
            while((VX1_LOCKN_STATUS ) && (u32Cnt-- > 0))
                Delay4us();
            bVby1CheckLockPass=TRUE;
        }
        for(chkLock=0 ; chkLock<LOCKN_STABLE_COUNTER ; chkLock++)
        {
            if(VX1_LOCKN_STATUS)
            {
                bVby1CheckLockPass=FALSE;
                drvmStar_printMsg("Check Lock ERROR!");
                drvmStar_printData("chkLock = %d", chkLock);
                break;
            }
        }
        if(bVby1CheckLockPass)
        {
            drvmStar_printData("VBY1 Training PASS!",0);
            break;
        }
        else
        {
            drvmStar_printData("VBY1 Training FAIL!",0);
        }
     }
     if( !u32TimeOutCnt )
     {
        drvmStar_printMsg("******VBY1 Training Timeout(SW)*******");
     }
     else
     {
        drvmStar_printData("******VBY1 LockTotalTime===%d", (LOCKN_TIMEOUT_COUNTER-u32TimeOutCnt));
     }
     #undef  LOCKN_TIMEOUT_COUNTER
     #undef  LOCKN_STABLE_COUNTER
}

#define max(a, b) (((a) > (b)) ? (a) : (b))
void msDrv_forceIP1VsyncPolarityEnable ()
{
    BYTE u8CountTime = 255;
    BYTE u8Posit=0,u8Negat=0;
    BYTE u8Pol=0;
    msWriteBit ( SC00_E8, FALSE, BIT4);

    do
    {
        if (SC0_READ_SYNC_STATUS() & BIT0)
        {
            u8Posit++;
        }
        else
        {
            u8Negat++;
        }
        u8CountTime--;
    } while (u8CountTime > 0);

    u8Pol=max(u8Posit,u8Negat);
    if (u8Pol==u8Posit)
        u8Pol=TRUE;
    else
        u8Pol=FALSE;

    msWriteBit(SC00_E8, u8Pol, BIT5);
    msWriteBit(SC00_E8, TRUE, BIT4);
}

#if (ENABLE_HDCP22)
#define BDMA_CRC32_POLY         (0x04C11DB7)
#define BDMA_CRC_SEED_F         (0xFFFFFFFF)
void APPInputHdcpKeySetValid(void)
{
    BYTE i;
    DWORD u32CRC;

    u32CRC = mcuDMACRC32(CHANNEL_AUTO, SOURCE_SPI, HDCP_KEY_22_RX_2ND_ADDR, COMBO_HDCP2_ENCODE_KEY_SIZE, BDMA_CRC32_POLY, BDMA_CRC_SEED_F);
    drvmStar_printData("CRC: 0x%x", u32CRC);
    drvmStar_printData("CRC: 0x%x", u32CRC>>16);
    for(i=0;i<4;i++)
    {
        FlashWriteAnyByte(TRUE, HDCP_KEY_22_RX_2ND_ADDR+COMBO_HDCP2_ENCODE_KEY_SIZE+i, (BYTE)((u32CRC>>((3-i)*8))&0x000000FF));
    }
}

BOOL APPInputHdcpKeyCheckValid(EN_HDCP_KEY_TYPE eKeyType)
{
    switch(eKeyType)
    {
        case HDCP_KEY_22_RX_2ND:
            return (0x00 == mcuDMACRC32(CHANNEL_AUTO, SOURCE_SPI, HDCP_KEY_22_RX_2ND_ADDR, COMBO_HDCP2_ENCODE_KEY_SIZE+4, BDMA_CRC32_POLY, BDMA_CRC_SEED_F));
        case HDCP_KEY_22_TX_2ND:
            return FALSE;//(0x00 == mcuDMACRC32(CHANNEL_AUTO, SOURCE_SPI, HDCP_KEY_22_RX_2ND_ADDR, COMBO_HDCP2_ENCODE_KEY_SIZE+4, BDMA_CRC32_POLY, BDMA_CRC_SEED_F));
        default:
            return FALSE;
    }
}

void APPInputHdcpKeyRead(EN_HDCP_KEY_TYPE eKeyType, BYTE *pKeyTable)
{
    WORD i;
    switch(eKeyType)
    {
        case HDCP_KEY_22_RX_1ST:
            drvmStar_printMsg("Read Key_RX_1ST\n");
            for(i=0;i<COMBO_HDCP2_ENCODE_KEY_SIZE;i++)
            {
                *(pKeyTable+i) = FlashReadAnyByte(HDCP_KEY_22_RX_1ST_ADDR+i);
            }

            for(i=0;i<COMBO_HDCP2_USER_ID_SIZE;i++)
            {
                *(g_u8KeyUserID+i) = FlashReadAnyByte(HDCP_KEY_22_RX_1ST_ADDR+COMBO_HDCP2_ENCODE_KEY_SIZE+i);
            }
            break;
        case HDCP_KEY_22_RX_2ND:
            drvmStar_printMsg("Read Key_RX_2ND\n");
            for(i=0;i<COMBO_HDCP2_ENCODE_KEY_SIZE+4;i++)
            {
                *(pKeyTable+i) = FlashReadAnyByte(HDCP_KEY_22_RX_2ND_ADDR+i);

            }
            break;
        default:
            drvmStar_printMsg("Read None\n");
            break;
    }
    #if 0
    printf("\n");
    for(i=0;i<COMBO_HDCP2_ENCODE_KEY_SIZE+4;i++)
        printf("0x%x ",*(pKeyTable+i));
    printf("\n");
    #endif
}

void APPInputHdcpKeyWrite(EN_HDCP_KEY_TYPE eKeyType, BYTE *pKeyTable)
{
    WORD i;
    BYTE u8SpiModeBk = mcuGetSpiMode();
    mcuSetSpiMode(SPI_MODE_FR);

    switch(eKeyType)
    {
        case HDCP_KEY_22_RX_2ND:
            drvmStar_printMsg("Write 2nd key \n");
            for(i=0;i<COMBO_HDCP2_ENCODE_KEY_SIZE;i++)
            {
                FlashWriteAnyByte(TRUE,HDCP_KEY_22_RX_2ND_ADDR+i,*(pKeyTable+i));
            }

            for(i=0;i<COMBO_HDCP2_USER_ID_SIZE;i++)
            {
                FlashWriteAnyByte(TRUE,HDCP_KEY_22_RX_2ND_ADDR+COMBO_HDCP2_ENCODE_KEY_SIZE+4+i,*(g_u8KeyUserID+i));
            }
            break;
        default:
            drvmStar_printMsg("Write None\n");
            break;
    }
    mcuSetSpiMode(u8SpiModeBk);
}
void APPInputHdcpKeyErase(EN_HDCP_KEY_TYPE eKeyType)
{
    BYTE u8SpiModeBk = mcuGetSpiMode();
    mcuSetSpiMode(SPI_MODE_FR);
    switch(eKeyType)
    {
        case HDCP_KEY_22_RX_1ST:
            FlashAnySectorErase(TRUE, HDCP_KEY_22_RX_1ST_ADDR);
            break;
        default:
            break;
    }
    mcuSetSpiMode(u8SpiModeBk);
}
BOOL APPInputHdcpKeyCompare(BYTE *pKeyTable, WORD u16Size)
{
    BOOL rtnResult = TRUE;
    BYTE temp;
    WORD comitem;

    for(comitem=0 ; comitem < u16Size ; comitem++)
    {
        temp = FlashReadAnyByte(HDCP_KEY_22_RX_2ND_ADDR+comitem);
        if (temp != *(pKeyTable+comitem))
        {
            drvmStar_printData("1st Failed address is %d ", comitem);
            rtnResult = FALSE;
            break;
        }
    }

    return rtnResult;
}
#define ERASED_CHECK_SIZE   0x10 // check head/tail each with 16 bytes
#define ERASED_DATA         0xFF // Flash erased value
BOOL APPInputHdcpKeyCheckErased(EN_HDCP_KEY_TYPE eKeyType)
{
    XDATA BOOL bRtn = FALSE;
    XDATA BYTE i;
    XDATA BYTE u8KeyPartial[ERASED_CHECK_SIZE] = {0x00};
    XDATA BYTE u8KeyErased[ERASED_CHECK_SIZE];

    for(i=0;i<ERASED_CHECK_SIZE;i++)
        u8KeyErased[i] = ERASED_DATA;

    switch(eKeyType)
    {
        case HDCP_KEY_22_RX_1ST:
            //Flash_ReadTbl(HDCP_KEY_22_RX_1ST_ADDR, u8KeyPartial, ERASED_CHECK_SIZE);
            for(i=0;i<ERASED_CHECK_SIZE;i++)
            {
                *(u8KeyPartial+i) = FlashReadAnyByte(HDCP_KEY_22_RX_1ST_ADDR+i);

            }

            for(i=0;i<ERASED_CHECK_SIZE;i++)
            {
                if(u8KeyPartial[i] != u8KeyErased[i])
                {
                    bRtn = FALSE;
                    break;
                }
                else
                    bRtn = TRUE;
            }

            if(bRtn)//(APPInputHdcpKeyCompare(HDCP_KEY_22_RX_1ST, u8KeyPartial, ERASED_CHECK_SIZE))
            {
                //Flash_ReadTbl(HDCP_KEY_22_RX_1ST_ADDR+COMBO_HDCP2_ENCODE_KEY_SIZE-ERASED_CHECK_SIZE, u8KeyPartial, ERASED_CHECK_SIZE);
                for(i=0;i<ERASED_CHECK_SIZE;i++)
                {
                    *(u8KeyPartial+i) = FlashReadAnyByte(HDCP_KEY_22_RX_1ST_ADDR+COMBO_HDCP2_ENCODE_KEY_SIZE-ERASED_CHECK_SIZE+i);
                }

                for(i=0;i<ERASED_CHECK_SIZE;i++)
                {
                    if(u8KeyPartial[i] != u8KeyErased[i])
                    {
                        bRtn = FALSE;
                        break;
                    }
                    else
                        bRtn = TRUE;
                }
            }
            break;
        default:
            break;
    }
    return bRtn;
}

#endif
void APPInput_HdcpKeyProcess(void)
{
#if (ENABLE_SECU_R2 == 1)
#if (COMBO_HDCP2_INITPROC_NEW_MODE)
    ENUM_HDCP_KEY_INIT_STATUS enInitStatus;
#endif
    BOOL bRtnKeyValid;
    WORD u16KeyRtnSize;
    BYTE u8HdcpKey22[COMBO_HDCP2_ENCODE_KEY_SIZE+CRC_SIZE] = {0};
    //BYTE u8HdcpKey22Bk[COMBO_HDCP2_ENCODE_KEY_SIZE+CRC_SIZE] = {0};


    ///////////////////////////////
    // TX Key Management //
    //////////////////////////////
#if(COMBO_HDCPTX_BLOCK_ENABLE == 1)
    msAPI_combo_InitHDCPkeyTable();  //HDCP key in IMI
    while (!msAPI_combo_ProcessHdcp1TxKey());

    APPInputHdcpKeyRead(HDCP_KEY_22_TX_2ND, u8HdcpKey22);

    if(APPInputHdcpKeyCheckValid(u8HdcpKey22,HDCP_KEY_22_TX_2ND))
    {
    #if (CHIP_ID == MST9U3)
            msAPI_combo_IPInsertHDCP22Key(TRUE, u8HdcpKey22, FALSE, FALSE);
    #elif((CHIP_ID != MST9U) && (CHIP_ID != MST9U2))  //CHIP after MST9U4
            bRtnKeyValid =msAPI_combo_Hdcp2InsertKeyWithCutomizeKey(TRUE, u8HdcpKey22, FALSE, FALSE, NULL);
    #endif

        while (!msAPI_combo_ProcessHdcp2TxKey(FALSE));

        // Could add customized condition/userdata to judge if the erasing is needed to save time.
        if(!APPInputHdcpKeyCheckErased(HDCP_KEY_22_TX_1ST))
            APPInputHdcpKeyErase(HDCP_KEY_22_TX_1ST);
    }
    else
    {
        APPInputHdcpKeyRead(HDCP_KEY_22_TX_1ST, u8HdcpKey22);

    #if (CHIP_ID == MST9U3)
            bRtnKeyValid = msAPI_combo_IPInsertHDCP22Key(TRUE, u8HdcpKey22, FALSE, FALSE);
    #elif((CHIP_ID != MST9U) && (CHIP_ID != MST9U2)) //CHIP after MST9U4
        bRtnKeyValid =msAPI_combo_Hdcp2InsertKeyWithCutomizeKey(TRUE, u8HdcpKey22, FALSE, TRUE, g_u8SecureStormAESKey);
    #endif

        while (!msAPI_combo_ProcessHdcp2TxKey(TRUE));

        if(bRtnKeyValid) // if 1st key is not valid then skip
        {
        if(msAPI_combo_FetchSecureStormResult(FALSE, u8HdcpKey22, COMBO_HDCP2_ENCODE_TX_KEY_SIZE, &u16KeyRtnSize))
            {
                // Add symbo, i.e. CRC, to mark this key's completeness
                APPInputHdcpKeySetValid(u8HdcpKey22,HDCP_KEY_22_TX_2ND);
                // Store 2nd Key to storage
                APPInputHdcpKeyWrite(HDCP_KEY_22_TX_2ND, u8HdcpKey22);
                // Read back from storage to double confirm 2nd Key was written successfully.
                // If 2nd Key Stored Successfully, then Erase 1st Key
                APPInputHdcpKeyRead(HDCP_KEY_22_TX_2ND, u8HdcpKey22Bk);
                if(APPInputHdcpKeyCompare(u8HdcpKey22, u8HdcpKey22Bk, COMBO_HDCP2_ENCODE_TX_KEY_SIZE+CRC_SIZE))
                    APPInputHdcpKeyErase(HDCP_KEY_22_TX_1ST);
            }
        }
    }
#endif
    ///////////////////////////////
    // HDCP 2.2 RX Key Management //
    //////////////////////////////

    APPInputHdcpKeyRead(HDCP_KEY_22_RX_2ND, u8HdcpKey22);
    ForceDelay1ms(20);  // wait secu r2 init done

    if(APPInputHdcpKeyCheckValid(HDCP_KEY_22_RX_2ND))
    {
        bRtnKeyValid = msAPI_combo_Hdcp2InsertKeyWithCutomizeKey(TRUE, u8HdcpKey22, TRUE, FALSE, NULL);

    #if (COMBO_HDCP2_INITPROC_NEW_MODE)
        do
            {
            enInitStatus = msAPI_combo_HDCP2InitHandler(FALSE);
        }while(enInitStatus == EN_HDCP_KEY_INIT_STATUS_NONE); //wait until hdcp22 init done;
    #endif
        // Could add customized condition/userdata to judge if the erasing is needed to save time.
        if(!APPInputHdcpKeyCheckErased(HDCP_KEY_22_RX_1ST))
        {
            APPInputHdcpKeyErase(HDCP_KEY_22_RX_1ST);
        }

    }
    else
    {
        APPInputHdcpKeyRead(HDCP_KEY_22_RX_1ST, u8HdcpKey22);

        bRtnKeyValid = msAPI_combo_Hdcp2InsertKeyWithCutomizeKey(TRUE, u8HdcpKey22, TRUE, TRUE, (BYTE*)g_u8SecureStormAESKey);

        if(bRtnKeyValid) // if 1st key is not valid then skip
        {
            #if (COMBO_HDCP2_INITPROC_NEW_MODE)
                do
                {
                    enInitStatus = msAPI_combo_HDCP2InitHandler(TRUE);
                }while(enInitStatus == EN_HDCP_KEY_INIT_STATUS_NONE); //wait until hdcp22 init done;
            #endif

            if(msAPI_combo_FetchSecureStormResult(TRUE, u8HdcpKey22, COMBO_HDCP2_ENCODE_KEY_SIZE, &u16KeyRtnSize))
            {
                // Store 2nd Key to storage
                APPInputHdcpKeyWrite(HDCP_KEY_22_RX_2ND, u8HdcpKey22);
                // Add symbo, i.e. CRC, to mark this key's completeness
                APPInputHdcpKeySetValid();
                // Read back from storage to double confirm 2nd Key was written successfully.
                // If 2nd Key Stored Successfully, then Erase 1st Key
                APPInputHdcpKeyRead(HDCP_KEY_22_RX_2ND, u8HdcpKey22);
                if(APPInputHdcpKeyCompare(u8HdcpKey22, COMBO_HDCP2_ENCODE_KEY_SIZE+CRC_SIZE))
                {
                    printf("\n compare OK %d\n",__LINE__);
                    //APPInputHdcpKeyErase(HDCP_KEY_22_RX_1ST);
                }
                else
                {
                    printf("\n compare Fail %d\n",__LINE__);
                }

            }
        }
    }
#endif
}
#endif

