///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mhal_eDPTx.c
/// @author MStar Semiconductor Inc.
/// @brief  DP Rx driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_EDPTX_C_
#define _MHAL_EDPTX_C_



//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "board.h"
//#include "DataType.h"
#include "Global.h"
//#include "Ms_reg.h"
//#include "ms_reg_tsumg.h"
#include "mhal_eDPTx.h"
#include "ms_reg.h"
#include "ms_reg_TSUMR2.h"
#include "msEread.h"

#if 1//PANEL_EDP

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#if (eDPTX_DEBUG&&ENABLE_MSTV_UART_DEBUG)
#define eDPTX_printData(str, value)   printData(str, value)
#define eDPTX_printMsg(str)               printMsg(str)
#else
#define eDPTX_printData(str, value)
#define eDPTX_printMsg(str)
#endif
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//eDP_SYSINFO gDPSYSInfo;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//**************************************************************************
//  [Function Name] :
//                  _mhal_eDPTx_PortNumber2eDPTxID(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : edptx_id
//
//**************************************************************************
BYTE _mhal_eDPTx_PortNumber2eDPTxID(eDPTx_ID edptx_id)
{
    switch(edptx_id)
    {
        case 0 :
            return eDPTx_ID_0;
            break;

        case 1 :
            return eDPTx_ID_1;
            break;

        case 2 :
            return eDPTx_ID_2;
            break;

        case 3 :
            return eDPTx_ID_3;
            break;
        default :
            eDPTX_printMsg("ERROR !: eDPTx ID Error\r\n");
            break;
    }


    return eDPTx_ID_MAX;     //Error ID


}



//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//**************************************************************************
//  [Function Name]:
//                  _mhal_DPRx_NewInitialSetting()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void  mhal_eDPTx_InitialSetting_2(eDPTx_ID edptx_id)
{
    edptx_id= edptx_id;
    //===================================================
    //  Item2.03 : Cannot delay MSA update in vblank, if MSA transfer in first line(vblank), EDPRX have fail
    //===================================================
#if 0//TBD
    #if (eDPTXMSADelayLine > 8)
    msWriteByteMask(REG_EDP_TX0_P0_78_L , BIT7|BIT6|BIT5, BIT7|BIT6|BIT5|BIT4);
    msWriteByteMask(REG_EDP_TX0_P0_78_L , BIT1, BIT1);
    #else if(eDPTXMSADelayLine > 1)
    msWriteByteMask(REG_EDP_TX0_P0_78_L , ((eDPTXMSADelayLine-1)*2)<<4, BIT7|BIT6|BIT5|BIT4);
    msWriteByteMask(REG_EDP_TX0_P0_78_L , BIT1, BIT1);
    #endif
#endif
    return;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_CheckIDInfo(void)
//
//  [Description] : check chip ID history
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_CheckIDInfo(void)
{
    return;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_SetHPDOV(eDPTx_ID edptx_id)
//
//  [Description] : Set HPD overwrite values to 0x00 --> detect HPD low
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SetHPDOV(eDPTx_ID edptx_id) //poll from Raptor
{
    edptx_id= edptx_id;
    msWriteByte(REG_EDP_TX0_P0_51_L , 0x00); // [3:0] HPD Soft Ov En  [7:4] Ov Value
    return;
}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_SetHPDOVHigh(eDPTx_ID edptx_id)
//
//  [Description] : Set HPD overwrite values to 0xFF --> detect HPD high
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SetHPDOVHigh(eDPTx_ID edptx_id) //poll from Raptor
{
    edptx_id= edptx_id;
    msWriteByte(REG_EDP_TX0_P0_51_L , 0xFF); // [3:0] HPD Soft Ov En  [7:4] Ov Value
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_SetAtop(void)
//
//  [Description] : set Atop initial
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SetAtop(eDPTx_ID edptx_id) //poll from Raptor
{
    // ## P0 AUXTX atop setting
    msWriteByte( REG_EDP_AUX_TX_P0_1C_H + eDPTX_AUX_Offset*edptx_id, 0x00); //wriu 0x112639  0x00
    msWriteByte( REG_EDP_AUX_TX_P0_1F_H + eDPTX_AUX_Offset*edptx_id,0x14); //wriu 0x11263F  0x14
    msWriteByte( REG_EDP_AUX_TX_P0_0D_H + eDPTX_AUX_Offset*edptx_id,0x0B);//wriu 0x11261B  0x0B  //  Sample Rate for 12M
    //msWriteByteMask( REG_EDP_AUX_TX_P0_05_L + eDPTX_Port_Offset_100*edptx_id,0x07,0x7F);//wriu 0x11260A  [6:0] 0x07  // UI for 12M
    msWriteByteMask( REG_EDP_AUX_TX_P0_05_L + eDPTX_AUX_Offset*edptx_id,0x07,0x7F);//20170911 : MengKun revised
    msWriteByteMask( REG_EDP_AUX_TX_P0_16_L + eDPTX_AUX_Offset*edptx_id,0,BIT0); // Enable AUXTX  // MST9U3 ONLY
    return;
}

//**************************************************************************
//  [Function Name] :  mhal_eDPTx_AuxPNSwap(void)
//
//  [Description] : edptx_id
//
//  [Arguments] : set Aux for P/N swap
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxPNSwap(eDPTx_ID edptx_id) //poll from Raptor
{
    //## AUXTX P/N swap
    //msWriteByte(REG_DPRX_AUX_TX0_03_H, 0xC0); //[7]:swap //wriu 0x112607  0xC0
    // ## timeout+P/N swap setting
    Bool bAuxSwap;

    bAuxSwap = edptx_id?g_sPnlInfo.bPnlEdpAuxP1PNSwap:g_sPnlInfo.bPnlEdpAuxP0PNSwap;

    msWriteByte(REG_EDP_AUX_TX_P0_03_H + edptx_id*eDPTX_AUX_Offset, bAuxSwap?0xC8:0x48);//[15] Swap AUX input activity polarity.
    msWriteByteMask(REG_EDP_AUX_TX_P0_20_L + edptx_id*eDPTX_AUX_Offset, bAuxSwap?BIT0:0, BIT0);//[0] Swap AUX output activity polarity.

    return;
}

//**************************************************************************
//  [Function Name] :  mhal_eDPTx_HPDOverwriteSet(void)
//
//  [Description] : make Aux clock debunce
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxClkDebunce(eDPTx_ID edptx_id) //poll from Raptor
{
    // ## aux debounce clock
    msWriteByte(REG_EDP_AUX_TX_P0_17_H + edptx_id*eDPTX_AUX_Offset, 0x0E);//0x11262E, 8w0E
    return;
}

//**************************************************************************
//  [Function Name] :  mhal_eDPTx_PHYInit(eDPTx_ID edptx_id)
//
//  [Description] : PHY initial with disable power down
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_PHYInit(eDPTx_ID edptx_id)//poll from Raptor
{
    edptx_id = edptx_id;
#if 0
    msWriteByteMask(REG_EDP_SC3_0D_H, 0, BIT3); //MOD SSC off
    msWrite2Byte(REG_EDP_TX_MOD2_0A_L ,0x0007);//clk en  //ok
    msWrite2Byte(REG_EDP_TX_CLKGEN_25_L ,0x0000);//clk en
    msWrite2Byte(REG_EDP_TX_CLKGEN_45_L ,0x000C);//clk en
    msWrite2Byte(REG_EDP_TX_CLKGEN_40_L ,0x05FF);//clk en  //ok
    msWrite2Byte(REG_EDP_TX_CLKGEN_39_L ,0x5511);//clk en  //ok
    msWrite2Byte(REG_EDP_SC1_00_L ,0x00);//clk en
    msWriteByte(REG_EDP_SC1_46_L ,0x51);//clk en
    msWrite2Byte(REG_EDP_TX_CLKGEN_17_L ,0x0040);//clk en  //ok
    msWriteByte(REG_EDP_TX0_P0_01_L + eDPTX_TX0_Offset*edptx_id ,0xFF);//clk en  //ok
    msWriteByte(REG_EDP_TX0_P0_77_H + eDPTX_TX0_Offset*edptx_id ,0xB5);//clk en  //ok
    mhal_eDPTx_EfuseRtermTrimSetting(edptx_id);
#endif


    return;
}


//**************************************************************************
//  [Function Name] :  mhal_eDPTx_MACInit(eDPTx_ID edptx_id)
//
//  [Description] : MAC Init
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_MACInit(eDPTx_ID edptx_id) //poll from Raptor
{

    edptx_id = edptx_id;
    msWrite2Byte(REG_EDP_TX_MOD1_27_L, 0x007E);
    msWrite2Byte(REG_EDP_TX_MOD1_28_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_21_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_22_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_29_L, 0xFFFF);
    msWrite2Byte(REG_EDP_TX_MOD1_2A_L, 0x00FF);
    msWriteByte(REG_EDP_TX_MOD2_06_L, 0x40);
    msWrite2Byte(REG_EDP_TX_MOD2_20_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD2_21_L, 0x0000);
    msWriteByteMask(REG_EDP_TX_MOD2_48_H,0,BIT7);
    msWriteByteMask(REG_EDP_TX_MOD1_2E_H,0,BIT2);
    msWrite2Byte(REG_EDP_TX_MOD2_50_L, 0x5858);
    msWrite2Byte(REG_EDP_TX_MOD2_51_L, 0x5858);
    msWrite2Byte(REG_EDP_TX_MOD2_52_L, 0x5858);
    msWrite2Byte(REG_EDP_TX_MOD2_53_L, 0x5858);
    msWrite2Byte(REG_EDP_TX_MOD2_54_L, 0x5858);


    msWrite2Byte(REG_EDP_SC3_7F_L, 0x0000);
    msWriteByte(REG_EDP_SC3_15_H, 0x00);
    msWriteByte(REG_EDP_SC3_02_L, 0x00);
    msWrite2Byte(REG_EDP_SC3_04_L, 0x0000);
    msWriteByte(REG_EDP_SC3_30_H, 0x00);
    msWriteByte(REG_EDP_SC3_32_H, 0x00);
    msWriteByte(REG_EDP_SC3_33_H, 0x00);
    msWrite2Byte(REG_EDP_SC3_34_L, 0x0000);
    msWriteByte(REG_EDP_SC3_37_H, 0x00);
    msWrite2Byte(REG_EDP_SC3_38_L, 0x0000);
    msWriteByte(REG_EDP_SC3_39_L, 0x00);
    msWrite2Byte(REG_EDP_SC3_60_L, 0x0000);
    msWrite2Byte(REG_EDP_SC3_61_L, 0x0000);
    msWrite2Byte(REG_EDP_SC3_62_L, 0x0000);
    msWrite2Byte(REG_EDP_SC3_63_L, 0x0000);
    msWrite2Byte(REG_EDP_SC3_64_L, 0x0000);
    msWrite2Byte(REG_EDP_SC3_65_L, 0x0000);
    msWrite2Byte(REG_EDP_SC3_66_L, 0x0000);
    msWrite2Byte(REG_EDP_SC3_71_L, 0x0000);

    msWrite2Byte(REG_EDP_TX_MOD1_41_L, 0xC002);
    msWrite2Byte(REG_EDP_TX_MOD1_42_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_43_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_51_L, 0x0000);
    msWriteByte(REG_EDP_TX_MOD1_3C_L, 0x00);
    msWriteByte(REG_EDP_TX_MOD2_0A_L, 0x03);
    msWrite2Byte(REG_EDP_TX_MOD1_65_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_66_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_67_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_68_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_18_L, 0x0001);
    msWrite2Byte(REG_EDP_TX_MOD1_19_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_1A_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_1B_L, 0x0000);
    //msWrite2Byte(REG_EDP_TX_MOD1_14_L, 0xFFFF);
    //msWrite2Byte(REG_EDP_TX_MOD1_15_L, 0xFFFF);
    //msWrite2Byte(REG_EDP_TX_MOD1_16_L, 0xFFFF);
    msWrite2Byte(REG_EDP_TX_MFT_53_L, 0x4000);
    msWrite2Byte(REG_EDP_TX_MFT_20_L, 0x4000);
    msWrite2Byte(REG_EDP_TX_MFT_1B_L, 0x00A0);
    msWrite2Byte(REG_EDP_TX_MFT_22_L, 0x0080);
    msWrite2Byte(REG_EDP_TX_MFT_26_L, 0x0080);
    msWrite2Byte(REG_EDP_TX_MFT_27_L, 0x0300);
    msWrite2Byte(REG_EDP_TX_MOD2_01_L, 0x3120);
    msWrite2Byte(REG_EDP_TX_MOD2_02_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD2_10_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD2_11_L, 0x0201);
    msWrite2Byte(REG_EDP_TX_MOD2_12_L, 0x0003);
    msWrite2Byte(REG_EDP_TX_MOD1_00_L, 0x0201);
    msWrite2Byte(REG_EDP_TX_MOD1_05_L, 0x0200);
    msWrite2Byte(REG_EDP_TX_MOD1_0F_L, 0xE400);
    msWrite2Byte(REG_EDP_TX_MOD1_30_L, 0x5000);
    msWrite2Byte(REG_EDP_TX_MOD1_31_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_64_L, 0x1003);
    msWrite2Byte(REG_EDP_TX_MOD1_61_L, 0x8F3F);
    msWrite2Byte(REG_EDP_TX_MOD1_68_L, 0x0080);
    msWrite2Byte(REG_EDP_TX_MOD1_62_L, 0x2000);
    msWrite2Byte(REG_EDP_TX_MOD1_60_L, 0x0AAE);
    msWrite2Byte(REG_EDP_TX_MOD2_40_L, 0x00FF);
    msWriteByte(REG_EDP_TX_MOD1_79_H, 0x40);
    msWriteByte(REG_EDP_TX_MOD1_79_H, 0x00);
    msWrite2Byte(REG_EDP_TX_MOD1_11_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_12_L, 0x0000);
    msWrite2Byte(REG_EDP_TX_MOD1_13_L, 0x0000);
    msWriteByte(REG_EDP_TX_MOD1_0D_L, 0x08);


    msWriteByteMask(REG_EDP_TX_CLKGEN_36_H,BIT0, BIT0);
    msWrite2ByteMask(REG_EDP_TX_CLKGEN_41_L,BIT1|BIT14|BIT15, BIT1|BIT14|BIT15);
    msWriteByteMask(REG_EDP_TX_CLKGEN_45_L,BIT3, BIT3);
    msWriteByte( REG_EDP_TX0_P0_19_L , 0x32);
    // SW reset
    msWrite2Byte( REG_EDP_TX0_P0_00_L , 0xFFFF);
    msWrite2Byte( REG_EDP_TX0_P0_52_L , 0xFFFF);
    msWrite2Byte( REG_EDP_TX0_P0_00_L , 0x0000);
    msWrite2Byte( REG_EDP_TX0_P0_52_L , 0x0000);
    // eDP Tx initial
    msWrite2Byte(REG_EDP_TX0_P0_65_L + eDPTX_TX0_Offset*edptx_id,0x020A); //TBC FIFO read start value
    msWriteByteMask( REG_EDP_TX0_P0_76_L + eDPTX_TX0_Offset*edptx_id,1,BIT0); //[0] Set Training New Mode
    msWriteByteMask(REG_EDP_TRANS_DUAL_P0_18_H + eDPTX_DUAL_Offset*edptx_id,0,BIT7); // disable 2p mode (Transmitter)
    msWrite2Byte(REG_EDP_TX0_P0_34_L + eDPTX_TX0_Offset*edptx_id,0x00f1); // [7:4]Scramble [0]enable dptx
    msWrite2Byte(REG_EDP_TX0_P0_52_L + eDPTX_TX0_Offset*edptx_id,0x6210); //form APN page.10
    msWrite2Byte(REG_EDP_TX_MOD2_0A_L,0x0003);  //MOD2  [0] edp en [1] edp clk en  //ok
    return;



}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_VideoInputSwap(void)
//
//  [Description] : MFT table and setting before display
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_VideoInputSwap(void) //poll from Raptor
{
    msWrite2ByteMask(REG_EDP_TX0_P0_5B_L, BIT0, BIT0);    // ex. L0/L1/L2  -> L0/R0/L2 ; R0/R1/R2 -> L1/R1/L3
    return;
}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_MFTEnable(void)
//
//  [Description] : MFT table and setting before display
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_MFTEnable(void) //poll from Raptor
{

    if(g_sPnlInfo.u8PnlEdpMODOnePort == 2) // disable MFT
    {
        msWriteByte(REG_EDP_TX_CLKGEN_43_H ,0x40); // CLK Gen for 2P LR

    }else
    {
        msWriteByte(REG_EDP_TX_CLKGEN_43_H ,0x00);  // CLK Gen for 1P
        msWriteByteMask(REG_EDP_TX_MFT_01_L, BIT4, BIT4);  // Odd to Full
    }

    return;
}



//**************************************************************************
//  [Function Name] :  mhal_eDPTx_PowerDown(void)
//
//  [Description] : Power down for PHY
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_PowerDown(BOOL bPowerDown) //poll from Raptor
{
    msWriteBit(REG_00210C, !bPowerDown, BIT0);
    msWriteBit(REG_EDP_AUX_TX_P0_52_L, bPowerDown, BIT6);
    msWriteBit(REG_EDP_AUX_TX_P0_1C_H, bPowerDown, BIT2);
    return;
}
//**************************************************************************
//  [Function Name] :  mhal_eDPTx_OVLaneFunction(void)
//
//  [Description] : OverWrite Lane Function to solve TPS3 decode error
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_OVLaneFunction(void)
{
    //set for solve training unstable issue (K/D code error)
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_61_L, 0x5555);
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_62_L, 0x5555);
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_63_L, 0x5555);
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_64_L, 0x5555);
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_65_L, 0x5555);
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_66_L, 0x5555);
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_67_L, 0x5555);
    msWrite2Byte(REG_EDP_TRANS_DUAL_P0_68_L, 0x5555);


    return;
}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_HBR2PNSwap(eDPTx_ID edptx_id)
//
//  [Description] : Set PN swap for each data lane
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_LanePNSwap(eDPTx_ID edptx_id)//poll from Raptor
{
    edptx_id = edptx_id;
//    g_sPnlInfo.bPnlEdpP1PNSWAP = g_sPnlInfo.bPnlEdpP1PNSWAP;

    if( g_sPnlInfo.bPnlEdpP0PNSWAP )
    {
        msWrite2Byte(REG_EDP_TX_MOD1_23_L,0x001E);
        msWrite2Byte(REG_EDP_TX_MOD1_24_L,0x0000);
    }
    else
    {
        msWrite2Byte(REG_EDP_TX_MOD1_23_L,0x0000);
        msWrite2Byte(REG_EDP_TX_MOD1_24_L,0x0000);
    }

    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_HBR2LaneSwap(eDPTx_ID edptx_id)
//
//  [Description] : Set data lane swap : 0123 <--> 3210
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_LaneSwap(eDPTx_ID edptx_id, BOOL bENABLE) //poll from Raptor
{
    edptx_id = edptx_id;

    if(bENABLE)
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_60_H,0x0F,0x0F); // [3:0] Lane SWAP : L0123 --> L3210
    }
    else
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_60_H,0x00,0x0F); // [3:0] Lane SWAP : L0123 --> L0123
    }
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_PatternSkew(eDPTx_ID edptx_id)
//
//  [Description] : Set lane skew for pattern
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_PatternSkew(eDPTx_ID edptx_id, BOOL bENABLE)
{
    edptx_id = edptx_id;

    if(bENABLE)
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_1F_L, BIT1, BIT1);  //ok
    }
    else
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_1F_L, 0, BIT1);  //ok
    }

    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_ConfigLinkRate(BYTE LinkRate, eDPTx_ID edptx_id)
//
//  [Description] : Config PHY link rate and set synthesizer
//
//  [Arguments] : LinkRate, edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_ConfigLinkRate(BYTE LinkRate, eDPTx_ID edptx_id)//poll form Raptor
{
    edptx_id = edptx_id;
    switch(LinkRate)
    {
        case eDPTXHBR:
            msWriteByteMask(REG_EDP_SC3_2F_L, 0, BIT1);
            msWriteByteMask(REG_EDP_SC3_2B_L, BIT5, BIT5);
            msWriteByteMask(REG_EDP_SC3_2D_H, BIT2|BIT1, BIT3|BIT2|BIT1|BIT0);
            msWriteByteMask(REG_EDP_TX_MOD1_28_H, BIT1, BIT1);
            break;

        case eDPTXRBR:
            msWriteByteMask(REG_EDP_SC3_2F_L, 0, BIT1);
            msWriteByteMask(REG_EDP_SC3_2B_L, 0, BIT5);
            msWriteByteMask(REG_EDP_SC3_2D_H, BIT3, BIT3|BIT2|BIT1|BIT0);
            msWriteByteMask(REG_EDP_TX_MOD1_28_H, 0, BIT1);
            break;

        default:
            break;
    }

    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_ConfigLinkRate(BYTE LinkRate, eDPTx_ID edptx_id)
//
//  [Description] : Config PHY link rate and set synthesizer
//
//  [Arguments] : LinkRate, edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_ConfigLaneCount(eDPTx_ID edptx_id, ConfigLaneCount LaneCount)//poll form Raptor
{
    edptx_id = edptx_id;
    switch(LaneCount)
    {

        case Config_4Lane:
            // 4lane mode
            msWriteByteMask(REG_EDP_TX0_P0_10_L+ edptx_id*eDPTX_TX0_Offset, BIT5, BIT5|BIT4);  // [2] 4lane [1] 2lane [0] 1lane
            msWrite2Byte(REG_EDP_TX0_P0_01_L+ edptx_id*eDPTX_TX0_Offset, 0x00FF);  // 2P mode 4Lane  ###
            //msWriteByteMask(REG_EDP_TX0_P0_5C_L, 0, BIT2|BIT1|BIT0); //m : original
            break;

        case Config_2Lane:
            // 2lane mode
            msWriteByteMask(REG_EDP_TX0_P0_10_L+ edptx_id*eDPTX_TX0_Offset, BIT4, BIT5|BIT4);  // [2] 4lane [1] 2lane [0] 1lane
            if(g_sPnlInfo.u8PnlEdpMODUsePort == 2)
            {
                msWrite2Byte(REG_EDP_TX0_P0_01_L+ edptx_id*eDPTX_TX0_Offset, 0x00FF);  // 2P mode 4Lane   ###
            }else
            {
                msWrite2Byte(REG_EDP_TX0_P0_01_L+ edptx_id*eDPTX_TX0_Offset, 0x0033);  // 2P mode 2Lane   ###
            }
            //msWriteByteMask(REG_EDP_TX0_P0_5C_L, BIT2|BIT0, BIT2|BIT1|BIT0); //m : x2
            break;

        case Config_1Lane:
            // 4lane mode
            msWriteByteMask(REG_EDP_TX0_P0_10_L+ edptx_id*eDPTX_TX0_Offset, 0, BIT5|BIT4);  // [2] 4lane [1] 2lane [0] 1lane
            if(g_sPnlInfo.u8PnlEdpMODUsePort == 2)
            {
                 msWrite2Byte(REG_EDP_TX0_P0_01_L+ edptx_id*eDPTX_TX0_Offset, 0x0033);  // 2P mode 2Lane
            }else
            {
                msWrite2Byte(REG_EDP_TX0_P0_01_L+ edptx_id*eDPTX_TX0_Offset, 0x0011);  // 2P mode 1Lane
            }
            //msWriteByteMask(REG_EDP_TX0_P0_5C_L, BIT2|BIT1, BIT2|BIT1|BIT0); //m : x4
            break;

        default:
            break;
    }

    return;
}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_SSCEnable(eDPTx_ID edptx_id, BYTE LinkRate, BYTE bEnable)
//
//  [Description] : SSC table for 0.1%, 0.2%, 0.3%, 0.4, 0.5%
//
//  [Arguments] : LinkRate, edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SSCEnable(eDPTx_ID edptx_id, BYTE LinkRate, BYTE bEnable) //poll from Raptor
{
    // step and span setting set in mStar_SetPanelSSCFactor
    edptx_id = edptx_id;
    LinkRate = LinkRate;
    msWriteBit(REG_LPLL_1B, bEnable, BIT3); // ssc enable

    return;

}





//**************************************************************************
//  [Function Name] :  mhal_eDPTx_ColorMISC(eDPTx_ID edptx_id)
//
//  [Description] : Set color MISC
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_ColorMISC(eDPTx_ID edptx_id)//poll from Raptor
{
    //eDPTXColorDepth  form panel define ~ 1:12 bits ; 2: 10bits; 3 : 8bits; 4 : 6bits;
    eDPTX_printData("eDPTXColorDepth1 = %x", g_sPnlInfo.u8PnlEdpColorDepth);
    if(g_sPnlInfo.u8PnlEdpColorDepth==1) //12 bit
        msWriteByteMask(REG_EDP_TX0_P0_32_L + eDPTX_TX0_Offset*edptx_id, BIT6|BIT5, BIT7|BIT6|BIT5); // [7:5] : 000 6bit, 001 8bit, 010 10bit, 011 12bit, 100 16bit  ###
    else if(g_sPnlInfo.u8PnlEdpColorDepth==2) // 10bit
        msWriteByteMask(REG_EDP_TX0_P0_32_L + eDPTX_TX0_Offset*edptx_id, BIT6, BIT7|BIT6|BIT5); // [7:5] : 000 6bit, 001 8bit, 010 10bit, 011 12bit, 100 16bit ###
    else if(g_sPnlInfo.u8PnlEdpColorDepth==3) //8bit
        msWriteByteMask(REG_EDP_TX0_P0_32_L + eDPTX_TX0_Offset*edptx_id, BIT5, BIT7|BIT6|BIT5); // [7:5] : 000 6bit, 001 8bit, 010 10bit, 011 12bit, 100 16bit ###
    else //6bit
      msWriteByteMask(REG_EDP_TX0_P0_32_L + eDPTX_TX0_Offset*edptx_id, 0x00, BIT7|BIT6|BIT5); // [7:5] : 000 6bit, 001 8bit, 010 10bit, 011 12bit, 100 16bit ###
    return;
}


//**************************************************************************
//  [Function Name] :  mhal_eDPTx_AuxInit(void)
//
//  [Description] : initial AUX setting
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxInit(eDPTx_ID edptx_id)
{
    edptx_id = edptx_id;
    msWriteByteMask(REG_EDP_TX_PADTOP_0B_H, BIT5|BIT4, BIT5|BIT4);//enable pad top for aux hpd GPIO

    msWriteByteMask(REG_EDP_AUX_TX_P0_1F_H + eDPTX_AUX_Offset*edptx_id, BIT4, BIT5|BIT4);
    msWriteByteMask(REG_EDP_AUX_TX_P0_1C_H + eDPTX_AUX_Offset*edptx_id, 0, BIT2);
    msWriteByteMask(REG_EDP_AUX_TX_P0_03_H + eDPTX_AUX_Offset*edptx_id, BIT5, BIT5);//mask bit > disable timeout
    msWriteByteMask(REG_EDP_AUX_TX_P0_03_H + eDPTX_AUX_Offset*edptx_id, 0x09, BIT4|BIT3|BIT2|BIT1|BIT0);    // modify timeout threshold [12:8]
    msWriteByte(REG_EDP_AUX_TX_P0_03_L + eDPTX_AUX_Offset*edptx_id, 0x6A);    // modify timeout threshold [7:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_16_L + eDPTX_AUX_Offset*edptx_id, 0, BIT5);

    msWriteByteMask(REG_EDP_AUX_TX_P0_17_H + eDPTX_AUX_Offset*edptx_id, BIT3|BIT2|BIT1, BIT3|BIT2|BIT1);
    msWriteByteMask(REG_EDP_AUX_TX_P0_16_L + eDPTX_AUX_Offset*edptx_id, 0, BIT0);
    msWriteByte(REG_EDP_AUX_TX_P0_0D_H + eDPTX_AUX_Offset*edptx_id, 0x0b);
    msWriteByteMask(REG_EDP_AUX_TX_P0_05_L + eDPTX_AUX_Offset*edptx_id, BIT2|BIT1|BIT0, BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(REG_EDP_AUX_TX_P0_51_H + eDPTX_AUX_Offset*edptx_id, BIT0,BIT0);  // 20170419 : aux ATOP , [8:7]=  deglitch + 0.6us(83ns(1/12M)*8T)
    msWriteByteMask(REG_EDP_AUX_TX_P0_53_H + eDPTX_AUX_Offset*edptx_id, BIT4|BIT0,BIT4|BIT0);  //aux mux
    msWriteByte(REG_EDP_AUX_TX_P0_24_L + eDPTX_AUX_Offset*edptx_id, 0x09);  //aux mux
    msWriteByteMask(REG_00210C, BIT0, BIT0);
    mhal_eDPTx_EfuseAuxTrimSetting(edptx_id, 0);

    return;
}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_ColorDepth(BYTE PortNum)
//
//  [Description] : Color depth for 6/8/10/12 bits
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_ColorDepth(eDPTx_ID edptx_id)//poll from Raptor
{
    //eDPTXColorDepth  form panel define ~ 1:12 bits ; 2: 10bits; 3 : 8bits; 3 : 12bits; 4 : 6 bits

    msWriteByteMask(REG_EDP_TX0_P0_17_H + eDPTX_TX0_Offset*edptx_id, g_sPnlInfo.u8PnlEdpColorDepth, BIT0|BIT1|BIT2); // [2:0] : 001 12bit, 002 10bit, 003 8bit, 004 6bit  ###
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_SetTU(BYTE PortNum
//                  )
//  [Description] : Set TU and DE only mode
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SetTU(eDPTx_ID edptx_id)
{
    edptx_id = edptx_id;

    if( g_sPnlInfo.bPnlEdpFineTuneTUEn )
        msWrite2Byte(REG_EDP_TX0_P0_60_L,0x1F00);// TU == 1F for overwrite function
    else
        msWrite2Byte(REG_EDP_TX0_P0_60_L,0x3f00);// TU == 3F

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_eDPTx_FineTuneTU_Disable(eDPTx_ID edptx_id)
//  [Description]
//                  mhal_eDPTx_FineTuneTU_Disable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_eDPTx_FineTuneTU_Disable(eDPTx_ID edptx_id)
{
    msWriteByteMask(REG_EDP_TX0_P0_61_H + eDPTX_TX0_Offset*edptx_id, 0, BIT0); // Dsiable force TU
}

//**************************************************************************
//  [Function Name]:
//                  mhal_eDPTx_FineTuneTU(eDPTx_ID edptx_id, DWORD eDPTX_OUTBL_PixRateKhz)
//  [Description]
//                  mhal_eDPTx_FineTuneTU
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_eDPTx_FineTuneTU(eDPTx_ID edptx_id, DWORD eDPTX_OUTBL_PixRateKhz)
{
    if( g_sPnlInfo.bPnlEdpFineTuneTUEn )
    {
        DWORD TU_size = 0;
        DWORD ColorDepth = 24;
        BYTE LaneCount = 4;
        WORD LinkRate = 540;

        edptx_id = edptx_id;
        eDPTX_OUTBL_PixRateKhz = eDPTX_OUTBL_PixRateKhz;

        switch(g_sPnlInfo.u8PnlEdpColorDepth)//0:6 bits ; 1: 8bits; 2 : 10bits; 3 : 12bits; 4 : 16 bits
        {
            case 1:
                ColorDepth = 36;
                break;
            case 2:
                ColorDepth = 30;
                break;
            case 3:
                ColorDepth = 24;
                break;
            case 4:
                ColorDepth = 18;
                break;
             default:
                ColorDepth = 24;
                break;
        }
        switch(g_sPnlInfo.u8PnlEdpLaneCnt)
        {
            case 4:
                LaneCount = eDPTX_4Lanes;
                break;
            case 2:
                LaneCount = eDPTX_2Lanes;
                break;
            case 1:
                LaneCount = eDPTX_1Lanes;
                break;
             default:
                LaneCount = eDPTX_4Lanes;
                break;
        }
        switch(g_sPnlInfo.u8PnlEdpLinkRate)
        {
            case eDPTX_HBR2:
                LinkRate = 540;
                break;
            case eDPTX_HBR:
                LinkRate = 270;
                break;
            case eDPTX_RBR:
                LinkRate = 162;
                break;
             default:
                LinkRate = 540;
                break;
        }

        TU_size = (32*eDPTX_OUTBL_PixRateKhz*ColorDepth)/(LinkRate*LaneCount*8)/100;
        eDPTX_printData("eDPTX_OUTBL_PixRateKhz = %x", eDPTX_OUTBL_PixRateKhz);
        eDPTX_printData("eDPTX_OUTBL_PixRateKhz>>16 = %x", eDPTX_OUTBL_PixRateKhz>>16);
        eDPTX_printData("[eDPTX] TU_size = %d", (TU_size/10));
        msWriteByteMask(REG_EDP_TX0_P0_61_L,(TU_size/10), 0x3F); // TU(integer)
        msWrite2ByteMask(REG_EDP_TX0_P0_62_L, 0x0A, 0x3FFF);
        msWrite2ByteMask(REG_EDP_TX0_P0_63_L,(TU_size%10), 0x3FFF); // TU(fraction)
        msWriteByteMask(REG_EDP_TX0_P0_61_H, BIT0, BIT0); // enable force TU
    }
    else
    {
        edptx_id = edptx_id;
        eDPTX_OUTBL_PixRateKhz = eDPTX_OUTBL_PixRateKhz;
    }

    return;
}




//**************************************************************************
//  [Function Name] : mhal_eDPTx_TimingSet(eDPTx_ID edptx_id)
//
//  [Description] : Set timing
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_TimingSet(eDPTx_ID edptx_id) //poll form Raptor
{
    WORD eDPHTOTAL = 0;
    WORD eDPVTOTAL = 0;
    WORD eDPHwidth  = 0;
    WORD eDPVwidth  = 0;
    BYTE HStart=0;
    BYTE VStart=0;

    eDPHTOTAL =  eDPTXMSA_Htotal;
    eDPVTOTAL =  eDPTXMSA_Vtotal;
    eDPHwidth  = eDPTXMSA_Hwidth;
    eDPVwidth  = eDPTXMSA_Vheight;
    HStart= eDPTXMSA_HStart;
    VStart= eDPTXMSA_VStart;
    eDPTX_printMsg("  MSA Data from Panel define \r\n");


    eDPTX_printData("MSA Htotal[%x]\r\n",eDPTXMSA_Htotal);
    eDPTX_printData("MSA eDPTXMSA_HStart[%x]\r\n",eDPTXMSA_HStart);
    eDPTX_printData("MSA eDPTXMSA_HSP*0x8000+eDPTXMSA_HSW[%x]\r\n",eDPTXMSA_HSP*0x8000+eDPTXMSA_HSW);
    eDPTX_printData("MSA Vtotal[%x]\r\n",eDPTXMSA_Vtotal);
    eDPTX_printData("MSA eDPTXMSA_VStart[%x]\r\n",eDPTXMSA_VStart);

    eDPTX_printData("MSA eDPTXMSA_VSP*0x8000+eDPTXMSA_VSW[%x]\r\n",eDPTXMSA_VSP*0x8000+eDPTXMSA_VSW);
    eDPTX_printData("MSA Hwidth[%x]\r\n",eDPTXMSA_Hwidth);
    eDPTX_printData("MSA Vheight[%x]\r\n",eDPTXMSA_Vheight);

    // MSA 2160P    VB=40 HB=128
    msWrite2Byte(REG_EDP_TX0_P0_2A_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_Htotal/g_sPnlInfo.u8PnlEdpTotalPorts);  // Htotal 2048
    msWrite2Byte(REG_EDP_TX0_P0_2B_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_HStart/g_sPnlInfo.u8PnlEdpTotalPorts); // HStart 40
    msWrite2Byte(REG_EDP_TX0_P0_2C_L + eDPTX_TX0_Offset*edptx_id,(eDPTXMSA_HSP*0x8000+eDPTXMSA_HSW)/g_sPnlInfo.u8PnlEdpTotalPorts);    // Hsp|Hsw 20
    msWrite2Byte(REG_EDP_TX0_P0_2D_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_Vtotal); // Vtotal  2200
    msWrite2Byte(REG_EDP_TX0_P0_2E_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_VStart); // Vstart  4
    msWrite2Byte(REG_EDP_TX0_P0_2F_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_VSP*0x8000+eDPTXMSA_VSW); // Vsp|Vsw  5
    msWrite2Byte(REG_EDP_TX0_P0_30_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_Hwidth/g_sPnlInfo.u8PnlEdpTotalPorts); //Hactive 1920
    msWrite2Byte(REG_EDP_TX0_P0_31_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_Vheight);    // Vactive 2160

    return;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_VideoPG(eDPTx_ID edptx_id)
//
//  [Description] : Set timing for pattern generate
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_VideoPG(eDPTx_ID edptx_id) //poll form Raptor
{
    WORD eDPHTOTAL = 0;
    WORD eDPVTOTAL = 0;
    WORD eDPHwidth  = 0;
    WORD eDPVwidth  = 0;
    BYTE   HStart=0;
    BYTE   VStart=0;


    eDPHTOTAL =  eDPTXMSA_Htotal;
    eDPVTOTAL =  eDPTXMSA_Vtotal;
    eDPHwidth  = eDPTXMSA_Hwidth;
    eDPVwidth  = eDPTXMSA_Vheight;
    HStart= eDPTXMSA_HStart;
    VStart= eDPTXMSA_VStart;
        eDPTX_printMsg("  MSA Data from Panel define \r\n");

    msWrite2Byte(REG_EDP_TX0_P0_1B_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_VFP); // V front porch
    msWrite2Byte(REG_EDP_TX0_P0_1C_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_VSW); // Vsp
    msWrite2Byte(REG_EDP_TX0_P0_1D_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_VBP); // Vback porch 30
    msWrite2Byte(REG_EDP_TX0_P0_1E_L + eDPTX_TX0_Offset*edptx_id,eDPTXMSA_Vheight); //V active

    msWrite2Byte(REG_EDP_TX0_P0_22_L + eDPTX_TX0_Offset*edptx_id,(eDPTXMSA_HFP)); // H front porch 28
    msWrite2Byte(REG_EDP_TX0_P0_23_L + eDPTX_TX0_Offset*edptx_id,(eDPTXMSA_HSW)); // Hsp 40
    msWrite2Byte(REG_EDP_TX0_P0_24_L + eDPTX_TX0_Offset*edptx_id,(eDPTXMSA_HBP)); //  H back proch 60
    msWrite2Byte(REG_EDP_TX0_P0_25_L + eDPTX_TX0_Offset*edptx_id,(eDPTXMSA_Hwidth)); // H active
    msWrite2Byte(REG_EDP_TX0_P0_1F_L + eDPTX_TX0_Offset*edptx_id,0x0000);  // [11:0]V pix step [12]pix repeat
    msWrite2Byte(REG_EDP_TX0_P0_26_L + eDPTX_TX0_Offset*edptx_id,0x1002); // [11:0]H pix step [12]pix repeat
    msWrite2Byte(REG_EDP_TX0_P0_27_L + eDPTX_TX0_Offset*edptx_id,0x0000); // H pix inc

    msWrite2Byte(REG_EDP_TX0_P0_21_L + eDPTX_TX0_Offset*edptx_id,0x0040); // H pix inc
    msWrite2Byte(REG_EDP_TX0_P0_1F_L + eDPTX_TX0_Offset*edptx_id,0x1001); // H pix inc
    mhal_eDPTx_PGEnable(edptx_id);
}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_PGEnable( eDPTx_ID edptx_id, BOOL ENABLE)
//
//  [Description] : Set pattern enable/disable
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_PGEnable( eDPTx_ID edptx_id)
{
    //msWrite2ByteMask(REG_EDP_TX0_P0_1A_L + eDPTX_Port_Offset_100*edptx_id, BIT1, BIT1); // SW reset PG
    msWrite2ByteMask(REG_EDP_TX0_P0_1A_H + eDPTX_TX0_Offset*edptx_id, BIT2|BIT1|BIT0, BIT2|BIT1|BIT0); //PG [8]R [9]G [10]B
    msWrite2ByteMask(REG_EDP_TX0_P0_1A_L + eDPTX_TX0_Offset*edptx_id, g_sPnlInfo.bPnlEdpEnPG, BIT0); // PG EN
    return;

}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_FsyncSet(void)
//
//  [Description] : Set Frame Sync Generate
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_FsyncSet(void)
{

}




//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxClearIRQ(eDPTx_ID edptx_id)
//
//  [Description] : Clear Aux IRQ
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxClearIRQ(eDPTx_ID edptx_id) //Poll from Raptor
{
    msWriteByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id, 0x7F); // [6:0] Clear irq
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_InputVTotal(void)
//
//  [Description] : Get Input Vtotal from scaler
//
//  [Arguments] : None
//
//  [Return] : 102F_01_1F[15:0]
//  [12:0] Input Vertical Total, count by HSYNC.
//  [1:0] Vsync Pulse Width Read Enable . The Report is shown in Current Bank 22
//
//**************************************************************************
WORD mhal_eDPTx_InputVTotal(void)
{
    return 0; // Vtotal
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_InputHTotal(void)
//
//  [Description] : Get Input Htotal from scaler
//
//  [Arguments] : None
//
//  [Return] : 102F_01_28[15:0]
//  [12:0]HTT by idclk
//
//**************************************************************************
WORD mhal_eDPTx_InputHTotal(void)
{
    return 0; // Htotal
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_OutputHTotal(void)
//
//  [Description] : Get Output Htotal from scaler
//
//  [Arguments] : None
//
//  [Return] : 102F_10_0C[11:0], [15:12]reserved
//  Output Horizontal Total.
//  53fh: Recommended value for XGA output (power on default value is 3).
//  697h: Recommended value for SXGA output
//
//**************************************************************************
WORD mhal_eDPTx_OutputHTotal(void)
{
    return 0; // Htotal
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_OutputVTotal(void)
//
//  [Description] : Get Output Vtotal from scaler
//
//  [Arguments] : None
//
//  [Return] : 102F_10_0D[11:0], [15:12]reserved
//  Output Vertical Total.
//  326h: Recommended value for XGA output (power on default value is 3).
//  42Ah: Recommended value for SXGA output
//
//**************************************************************************
WORD mhal_eDPTx_OutputVTotal(void)
{
    return 0; // Vtotal
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_ClkDividerSet(DWORD SCLPLL_InDiv, DWORD SCLPLL_OutDiv)
//
//  [Description] :
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_ClkDividerSet(DWORD SCLPLL_InDiv, DWORD SCLPLL_OutDiv)
{
    SCLPLL_InDiv = SCLPLL_InDiv;
    SCLPLL_OutDiv = SCLPLL_OutDiv;
    return;
}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxReadCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR)
//
//  [Description] : Set read 1 byte command for Aux
//
//  [Arguments] : edptx_id, DPCDADDR
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxReadCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR) //Poll from Raptor
{
    msWriteByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id,0x7F); // [6:0] Clear irq
    //## AUXTX CMD
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x01); // [0] Complete CMD ->Clear Buf
    msWriteByte( REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id,0x09); // [3:0] CMD
    msWriteByte( REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte( REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByte( REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F); // [3:0] ADR[19:16]
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x00); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,0x00); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxReadCMDBytes(eDPTx_ID edptx_id, BYTE Length, DWORD DPCDADDR)
//
//  [Description] : Set read bytes command for Aux
//
//  [Arguments] : edptx_id, Length, DPCDADDR
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxReadCMDBytes(eDPTx_ID edptx_id, BYTE Length, DWORD DPCDADDR)//poll from Raptor
{
    //## AUXTX CMD
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x01); // [0] Complete CMD ->Clear Buf
    msWriteByte( REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id,0x09); // [3:0] CMD
    msWriteByte( REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte( REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByte( REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F); // [3:0] ADR[19:16]
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,0x00); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD

    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_CheckWritePoint(eDPTx_ID edptx_id)
//
//  [Description] : Check read/write done
//
//  [Arguments] : edptx_id
//
//  [Return] : 1126_06[7:0]
//  1126[3:0] AUX RX FIFO write pointer.
//  1126[7:4] AUX RX FIFO read pointer.
//
//**************************************************************************
Bool mhal_eDPTx_CheckWritePoint(eDPTx_ID edptx_id)//Poll from Raptor
{
    return (msReadByte(REG_EDP_AUX_TX_P0_06_L + eDPTX_AUX_Offset*edptx_id)&0x0F); // [3:0]: Write Point (RX LENG) [7:4] Read Point
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_CheckAuxRxFull(eDPTx_ID edptx_id)
//
//  [Description] : Check Aux fifo full?
//
//  [Arguments] : edptx_id
//
//  [Return] : 1126_06[9]
//  DP AUX RX FIFO full.
//  0: FIFO full not act.
//  1: FIFO full assert.
//
//**************************************************************************
Bool mhal_eDPTx_CheckAuxRxFull(eDPTx_ID edptx_id)//Poll from Raptor
{
    return (msReadByte(REG_EDP_AUX_TX_P0_06_H + eDPTX_AUX_Offset*edptx_id)&0x02);
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxRxReplyCMD(eDPTx_ID edptx_id)
//
//  [Description] : Check Aux RX reply command
//
//  [Arguments] : edptx_id
//
//  [Return] : 1126_09[7:0]
//  1126[3:0] DP AUX RX FIFO receive command.
//

//**************************************************************************
Bool mhal_eDPTx_AuxRxReplyCMD(eDPTx_ID edptx_id)//Poll from Raptor
{
    return (msReadByte(REG_EDP_AUX_TX_P0_09_L + eDPTX_AUX_Offset*edptx_id)&0x0F);
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_TxComplete(eDPTx_ID edptx_id)
//
//  [Description] : write 1126_14[8]
//              AUX Request/Response transaction complete.
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_TxComplete(eDPTx_ID edptx_id)//Poll from Raptor
{
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01);// [0] Complete CMD ->Clear RX Buf
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_TrigerReadPulse0(eDPTx_ID edptx_id)
//
//  [Description] : 1126_08[15:8] = 0x00
//              [9] reg_aux_rd_mode = 0 : AUX RX FIFO read pointer increase by a read pulse
//              [8] reg_aux_rx_fifo_read_pulse = 0 : Write 1 to increment AUX RX FIFO read pointer.
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_TrigerReadPulse0(eDPTx_ID edptx_id)//Poll from Raptor
{
    msWriteByte(REG_EDP_AUX_TX_P0_08_H + eDPTX_AUX_Offset*edptx_id, 0x00); // [1]: set 0
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_TrigerReadPulse1(eDPTx_ID edptx_id)
//
//  [Description] : 1126_08[15:8] = 0x00
//              [9] reg_aux_rd_mode = 0 : AUX RX FIFO read pointer increase by a read pulse
//              [8] reg_aux_rx_fifo_read_pulse = 1 : Write 1 to increment AUX RX FIFO read pointer.
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_TrigerReadPulse1(eDPTx_ID edptx_id)//Poll from Raptor
{
    msWriteByte(REG_EDP_AUX_TX_P0_08_H + eDPTX_AUX_Offset*edptx_id, 0x01); // [0]: triger read pulse
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxReadData(eDPTx_ID edptx_id)
//
//  [Description] : Read data from Aux RX
//
//  [Arguments] : edptx_id
//
//  [Return] : 1126_08[7:0]
//  reg_aux_rx_fifo_read_data : DP AUX RX FIFO receive data.
//
//**************************************************************************
BYTE mhal_eDPTx_AuxReadData(eDPTx_ID edptx_id)//Poll from Raptor
{
    return msReadByte(REG_EDP_AUX_TX_P0_08_L + eDPTX_AUX_Offset*edptx_id);
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxWriteCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData)
//
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxWriteCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData) //poll from Raptor
{
    //msWriteByte( REG_EDP_AUX_TX_P0_10_L + eDPTX_Port_Offset_100*edptx_id,0x7F); // [6:0] Clear irq
    //## AUXTX CMD
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x01); // [0] Complete CMD ->Clear Buf
    msWriteByte( REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3:0] CMD
    msWriteByte( REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte( REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByte( REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F); // [3:0] ADR[19:16]
    msWriteByte( REG_EDP_AUX_TX_P0_0D_L + eDPTX_AUX_Offset*edptx_id,wData); // [7:0] AUX TX Write DATA BUF
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x00); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,0x00); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD
    return;
}


//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_AuxWriteCMDBytes(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxWriteCMDBytes(eDPTx_ID edptx_id, DWORD DPCDADDR)//poll from Raptor
{
    //## AUXTX CMD
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x01); // [0] Complete CMD ->Clear Buf
    msWriteByte( REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3:0] CMD
    msWriteByte( REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte( REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByte( REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F); // [3:0] ADR[19:16]
    return;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxRXCompleteInterrupt(eDPTx_ID edptx_id)
//
//  [Description] : Check Aux RX reply interrupt
//
//  [Arguments] : edptx_id
//
//  [Return] : reg_aux_rx_cmd_recv_irq
//  AUX RX Command field receive complete interrupt.
//  1: Interrupt event appeared.
//  0: Interrupt event no action.
//  Write 1 to clear.
//**************************************************************************
Bool mhal_eDPTx_AuxRXCompleteInterrupt(eDPTx_ID edptx_id) //poll from Raptor
{
    return (msReadByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id)&BIT3); // [3] Rx Cmd irq
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_AuxWriteCheckComplete(eDPTx_ID edptx_id)
//
//  [Description] : Check Aux RX reply complete
//
//  [Arguments] : edptx_id
//
//  [Return] : reg_aux_rx_aux_recv_complete_irq
//  AUX RX Native AUX CH transaction complete interrupt.
//  1: Interrupt event appeared.
//  0: Interrupt event no action.
//  Write 1 to clear.
//**************************************************************************
Bool mhal_eDPTx_AuxWriteCheckComplete(eDPTx_ID edptx_id)
{
    return(msReadByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id)&BIT6);
}



//**************************************************************************
//  [Function Name] : mhal_eTPTx_AUXTXWriteData(eDPTx_ID edptx_id, BYTE LengthCnt ,BYTE *pTxBuf)
//
//  [Description] :
//
//  [Arguments] : edptx_id, LengthCnt, pTxBuf
//
//  [Return] : None
//
//**************************************************************************
void mhal_eTPTx_AUXTXWriteBuffer(eDPTx_ID edptx_id, BYTE LengthCnt ,BYTE *pTxBuf) //poll from Raptor
{
    msWriteByte(REG_EDP_AUX_TX_P0_0D_L + eDPTX_AUX_Offset*edptx_id,*(pTxBuf+LengthCnt)); // [7:0] AUX TX Write DATA BUF
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eTPTx_AUXTXWriteData(eDPTx_ID edptx_id, BYTE LengthCnt ,BYTE *pTxBuf)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eTPTx_AUXTXWriteData(eDPTx_ID edptx_id, BYTE Length)//poll from Raptor
{
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,0x00); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eTPTx_AUXTXWriteDataBytes(eDPTx_ID edptx_id, BYTE Length, BYTE EOF)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eTPTx_AUXTXWriteDataBytes(eDPTx_ID edptx_id, BYTE Length, BYTE EOF)
{
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,EOF&0x01); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD

    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_AuxEDIDReadNLCMD(eDPTx_ID edptx_id, DWORD DPCDADDR)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxEDIDReadNLCMD(eDPTx_ID edptx_id, DWORD DPCDADDR) //poll form Raptor
{
    //## AUXTX CMD
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x01); // [0] Complete CMD ->Clear Buf
    msWriteByte( REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id,0x01); // [3:0] CMD , MOT=0
    msWriteByte( REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte( REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByte( REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F); // [3:0] ADR[19:16]
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x00); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,0x01); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD
    return;

}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_AuxEDIDReadNLCMD(eDPTx_ID edptx_id, DWORD DPCDADDR)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxEDIDWriteByteCMD(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData) //poll form Raptor
{
    //## AUXTX CMD
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x01); // [0] Complete CMD ->Clear Buf
    msWriteByte( REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id,0x04); // [3:0] CMD ,MOT=1
    msWriteByte( REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte( REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByte( REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F); // [3:0] ADR[19:16]
    msWriteByte( REG_EDP_AUX_TX_P0_0D_L + eDPTX_AUX_Offset*edptx_id,wData); // [7:0] AUX TX Write DATA BUF
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x00); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,0x00); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD
    return;

}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_AuxEDIDeadBytesCMD(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE Length)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxEDIDReadBytesCMD(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE Length)//poll from Raptor
{
    //## AUXTX CMD
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,0x01); // [0] Complete CMD ->Clear Buf
    msWriteByte( REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id,0x05); // [3:0] CMD , MOT=1
    msWriteByte( REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte( REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByte( REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F); // [3:0] ADR[19:16]
    msWriteByte( REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByte( REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,0x00); // [0] No Length Command
    msWriteByte( REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id,0x08); // [3] AUX TX Fire CMD
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_AuxMCCSReadBytesCMD( eDPTx_ID edptx_id, BYTE Length, BYTE EOF)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxMCCSReadBytesCMD( eDPTx_ID edptx_id, BYTE Length, BYTE EOF)
{
    edptx_id = edptx_id;
    Length = Length;
    EOF = EOF;
    return;
}


//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_AuxMCCSWriteBytesCMD( eDPTx_ID edptx_id, BYTE Length, BYTE EOF)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AuxMCCSWriteBytesCMD( eDPTx_ID edptx_id, BYTE EOF)
{
    edptx_id = edptx_id;
    EOF = EOF;
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_HPDInitSet(eDPTx_ID edptx_id)
//
//  [Description] : Set HPD initail
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HPDInitSet(eDPTx_ID edptx_id)//Poll from Raptor
{
    //msWriteByte( REG_EDP_TX0_P0_A3,0xFF); // [3:0] HPD GPIO Oen   [7:4] GPIO Output Value
    msWriteByteMask( REG_EDP_TX0_P0_51_H + eDPTX_TX0_Offset*edptx_id, g_sPnlInfo.u8PnlEdpHPDPort0|g_sPnlInfo.u8PnlEdpHPDPort1,g_sPnlInfo.u8PnlEdpHPDPort0|g_sPnlInfo.u8PnlEdpHPDPort1); // [3:0] HPD GPIO Oen   [7:4] GPIO Output Value
    msWriteByteMask( REG_EDP_TX0_P0_52_H + eDPTX_TX0_Offset*edptx_id,BIT6|BIT5,BIT6|BIT5); //[6] En Xtal Clock [5]mn gen
    msWriteByte( REG_EDP_TX0_P0_57_L + eDPTX_TX0_Offset*edptx_id,0x30); //12M Xtal Count 1us [7:2] integer [1:0]Fractional
    msWriteByte( REG_EDP_TX0_P0_50_L + eDPTX_TX0_Offset*edptx_id,0xE8); // [3:0] debounce Xtal*8   [7:4] HPD int event [5:4] lower 100us [7:6] upper 700us  // 200us per step
    msWriteByte( REG_EDP_TX0_P0_50_H + eDPTX_TX0_Offset*edptx_id,0x00); // [3:0] disconnect 1.5ms+N*100us  [7:4] connect 1.5ms+N*100us
    msWrite2Byte(REG_EDP_TX0_P0_56_L ,0x0FFF);  // clear IRQ
    msWrite2Byte(REG_EDP_TX0_P0_56_L ,0x0000);  // clear IRQ;

    if(g_sPnlInfo.u8PnlEdpTotalPorts == 2)
    {
        msWrite2Byte(REG_EDP_TX0_P0_54_L + eDPTX_TX0_Offset*edptx_id,0xFFC0); // HPD IRQ mask port0&1
    }
    else if(g_sPnlInfo.u8PnlEdpTotalPorts == 1)
    {
        msWrite2Byte(REG_EDP_TX0_P0_54_L + eDPTX_TX0_Offset*edptx_id,0xFFF8); // HPD IRQ mask port1
    }
    else
    {
        msWrite2Byte(REG_EDP_TX0_P0_54_L + eDPTX_TX0_Offset*edptx_id,0xFFC7); // HPD IRQ mask port0
    }


    msWrite2Byte(REG_EDP_TX0_P0_55_L + eDPTX_TX0_Offset*edptx_id,0x0000); // HPD IRQ force
    msWrite2Byte(REG_EDP_TX0_P0_56_L ,0x0FFF);  // clear IRQ
    msWrite2Byte(REG_EDP_TX0_P0_56_L ,0x0000);  // clear IRQ;

    //msWriteByteMask( REG_10196C , 0, BIT0); //HPD INT MASK
    //msWriteByteMask( REG_10196B , 0, BIT6); //AUXP0 INT MASK
    msWriteByteMask(REG_EDP_TX0_P0_72_L + eDPTX_TX0_Offset*edptx_id, 0x32, 0x3F);  // HPD interrupt threshould 0.9m~1.3ms
    msWrite2Byte(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id,0xFFFF); // clear AUX IRQ
    msWrite2Byte(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id,0x0000); // clear AUX IRQ

    msWrite2Byte(REG_EDP_AUX_TX_P1_1A_L + eDPTX_AUX_Offset*edptx_id,0xFFFF); // clear AUX IRQ
    msWrite2Byte(REG_EDP_AUX_TX_P1_1A_L + eDPTX_AUX_Offset*edptx_id,0x0000); // clear AUX IRQ


    msWrite2Byte(REG_EDP_AUX_TX_P0_18_L + eDPTX_AUX_Offset*edptx_id,0xFDFF); // AUX IRQ mask port0
    msWrite2Byte(REG_EDP_AUX_TX_P0_18_L + eDPTX_AUX_Offset*edptx_id,0xFDFF); // AUX IRQ mask port0
    msWrite2Byte(REG_EDP_AUX_TX_P1_18_L + eDPTX_AUX_Offset*edptx_id,0xFDFF); // AUX IRQ mask port0
    msWrite2Byte(REG_EDP_AUX_TX_P1_18_L + eDPTX_AUX_Offset*edptx_id,0xFDFF); // AUX IRQ mask port0
    msWriteByteMask(REG_EDP_TX_PADTOP_44_L, BIT1, BIT1);


    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_MLSignalDisable(eDPTx_ID edptx_id)
//
//  [Description] : Disable main link signal output
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_MLSignalDisable(eDPTx_ID edptx_id) //Poll from Raptor
{
    edptx_id = edptx_id;

    if(edptx_id==0)
    {
        msWrite2Byte(REG_EDP_TX_MOD1_10_L,0x0000); // ch1~4 LVDS mode
        //msWriteByteMask(REG_EDP_TX_MOD1_53,0x00,BIT3|BIT2|BIT1|BIT0);  // MOD1  Ch8-Ch11 Rterm off
    }
    else
    {
        msWriteByte(REG_EDP_TX_MOD1_11_H,0x00); // ch12~15  TTL/standy mode
        //msWriteByteMask(REG_EDP_TX_MOD1_53,0x00,BIT7|BIT6|BIT5|BIT4);  // MOD1  Ch12-Ch15 Rterm off
    }
        eDPTX_printData("## eDPTX MOD PORT#[%x] ML Signal Off ##\r\n",edptx_id);

    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_MLSignalEnable(eDPTx_ID edptx_id)
//
//  [Description] : Enable main link signal output
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_MLSignalEnable(eDPTx_ID edptx_id)//Poll from Raptor
{
    if(edptx_id == 0)
    {
        msWrite2Byte(REG_EDP_TX_MOD1_10_L,0x0154); // ch1~4 LVDS mode
    }
    else
    {
        msWriteByteMask(REG_EDP_TX_MOD1_11_H,0x55,0x55); // ch12~15  LVDS mode
    }
        eDPTX_printData("## eDPTX MOD PORT#[%x] ML Signal On ##\r\n",edptx_id);

    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDStatus(WORD HPDStatus, eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//  IRQ status
//  [0]: Link 0 HPD connect status.
//  [1]: Link 0 HPD dis-connect status.
//  [2]: Link 0 HPD interrupt envent status.
//  [3]: Link 1 HPD connect status.
//  [4]: Link 1 HPD dis-connect status.
//  [5]: Link 1 HPD interrupt envent status.
//  [6]: Link 2 HPD connect status.
//  [7]: Link 2 HPD dis-connect status.
//  [8]: Link 2 HPD interrupt envent status.
//  [9]: Link 3 HPD connect status.
//  [10]: Link 3 HPD dis-connect status.
//  [11]: Link 3 HPD interrupt envent status.
//  [15:12]: Reserved.
//
//**************************************************************************
WORD mhal_eDPTx_HPDStatus(eDPTx_ID edptx_id)
{
    WORD u16HPDStatus;

    u16HPDStatus = (msReadByte(REG_EDP_TX0_P0_53_L)&(0x07<<edptx_id));
    msWriteByte(REG_EDP_TX0_P0_56_L, u16HPDStatus);
    msWriteByte(REG_EDP_TX0_P0_56_L, 0);
    return u16HPDStatus;

}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_ASSREnable(eDPTx_ID edptx_id)
//
//  [Description] : Enable/disable ASSR
//
//  [Arguments] : edptx_id, bENABLE
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_ASSR(eDPTx_ID edptx_id,BOOL bENABLE)//poll form Raptor
{
    edptx_id = edptx_id;
    if (bENABLE==1)
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_19_L, BIT0, BIT0);//enable ASSR for eDP panel
    }
    else
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_19_L, 0, BIT0);//disable ASSR for eDP panel
    }
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_ASSREnable(eDPTx_ID edptx_id)
//
//  [Description] : Enable/disable AFR
//
//  [Arguments] : edptx_id, bENABLE
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_AFR(eDPTx_ID edptx_id,BOOL bENABLE)//poll form Raptor
{
    edptx_id = edptx_id;
    if (bENABLE==1)
    {
        msWriteByteMask(REG_EDP_TX0_P0_19_L,BIT1,BIT1);  // Alternate Framing Mode enable
    }
    else
    {
        msWriteByteMask(REG_EDP_TX0_P0_19_L,0,BIT1);  // Alternate Framing Mode enable
    }
    return;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_EnFrameModeEnable(eDPTx_ID edptx_id)
//
//  [Description] : Enable/disable Enhance Frame Mode
//
//  [Arguments] : edptx_id, bENABLE
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_EnhanceFrameMode(eDPTx_ID edptx_id,BOOL bENABLE)//poll from Raptor
{
    edptx_id = edptx_id;
    if(bENABLE == 1)
    {
        msWriteByteMask(REG_EDP_TX0_P0_15_L, BIT7, BIT7);  // enhanceFrame Mode enable
    }else
    {
        msWriteByteMask(REG_EDP_TX0_P0_15_L, 0, BIT7);  // enhanceFrame Mode disable
    }
    return;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_SetLaneCnt(eDPTx_ID edptx_id, BYTE LaneCount)
//
//  [Description] : Set lane count : 1/2/4 Lanes
//
//  [Arguments] : edptx_id, LaneCount
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SetLaneCnt(eDPTx_ID edptx_id, BYTE LaneCount) //poll from Raptor
{

    msWriteByteMask(REG_EDP_TX0_P0_10_L + eDPTX_TX0_Offset*edptx_id,(LaneCount>>1)&0x03, BIT1|BIT0);// [3:2]: pattern [1:0] Lane Count 0:1L /1:2L /2:4L
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_LaneDataOverWriteEnable(eDPTx_ID edptx_id, BOOL bENABLE)
//
//  [Description] : Set lane data overwrite enable
//
//  [Arguments] : edptx_id, bENABLE
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_LaneDataOverWriteEnable(eDPTx_ID edptx_id, BOOL bENABLE)
{
    edptx_id = edptx_id;

    if(bENABLE)
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_60_H + eDPTX_DUAL_Offset*edptx_id, BIT7|BIT6|BIT5|BIT4, BIT7|BIT6|BIT5|BIT4);
    }
    else
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_60_H + eDPTX_DUAL_Offset*edptx_id, 0, BIT7|BIT6|BIT5|BIT4);
    }
    return;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_SetPHY_IdlePattern( eDPTx_ID edptx_id, BOOL  ENABLE)
//
//  [Description] : Set idle pattern with analog/PHY
//
//  [Arguments] : edptx_id, bENABLE
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SetPHY_IdlePattern(eDPTx_ID edptx_id, BOOL bENABLE)
{
    edptx_id = edptx_id;
    bENABLE = bENABLE;
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_TrainingPattern_Select(BYTE TP_value, eDPTx_ID edptx_id)
//
//  [Description] : Enable training pattern :0/1/2/3/4
//
//  [Arguments] : TP_value, edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_TrainingPattern_Select(BYTE TP_value, eDPTx_ID edptx_id) //poll from Raptor
{
    if(edptx_id)
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_0E_L ,(BIT4<<TP_value), 0xF0);
    else
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_0E_L ,(BIT0<<TP_value), 0x0F);
    msWriteByteMask(REG_EDP_TRANS_DUAL_P0_0E_H ,BIT0,BIT0);// enable training pattern
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_TBCFIFO_Overwrite
//
//  [Description] : calculate and overwrite TU
//
//  [Arguments] : us100TimesTU
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_TBCFIFO_Overwrite(void)
{
    msWrite2Byte(REG_EDP_TX0_P0_65_L,0x0204); //TBC FIFO read start value
    return;

}



//**************************************************************************
//  [Function Name] : mhal_eDPTx_TU_Overwrite
//
//  [Description] : calculate and overwrite TU
//
//  [Arguments] : us100TimesTU
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_TU_Overwrite(WORD us100TimesTU)
{
    us100TimesTU = us100TimesTU;
}


//**************************************************************************
//  [Function Name] : mhal_eDPTx_FMTOutputEnable(eDPTx_ID edptx_id)
//
//  [Description] :
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_FMTOutputEnable(void) //Poll from Raptor
{
    msWrite2Byte(REG_EDP_TX0_P0_52_L,0x7030); // (including clock enable and MN gen) APN : 0x6210
    msWrite2ByteMask(REG_EDP_TX0_P0_29_L,0x8000,BIT15); // Block 1<->2 swap
    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDP0Values(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : HPD GPIO input value compare with port0
//
//**************************************************************************
Bool mhal_eDPTx_HPDValues(eDPTx_ID edptx_id)
{
    return(msReadByte(REG_EDP_TX0_P0_52_L)&(BIT0<<edptx_id));
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDP0ClearIRQBit012(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HPDClearIRQBitAll(eDPTx_ID edptx_id) //poll from Raptor
{
    if(edptx_id == 0)
    {
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent,eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent);  // clear IRQ
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent);
    }else
    {
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP1_IRQConnect|eDPTXHPDP1_IRQDisConnect|eDPTXHPDP1_IRQEvent,eDPTXHPDP1_IRQConnect|eDPTXHPDP1_IRQDisConnect|eDPTXHPDP1_IRQEvent);  // clear IRQ
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP1_IRQConnect|eDPTXHPDP1_IRQDisConnect|eDPTXHPDP1_IRQEvent);

    }

    return;
}




//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDP0ClearIRQBit0(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HPDClearIRQConnect(eDPTx_ID edptx_id) //poll from Raptor
{
    if(edptx_id == 0)
    {
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP0_IRQConnect,eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent);  // clear IRQ
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent);
    }else
    {
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP1_IRQConnect,eDPTXHPDP1_IRQConnect|eDPTXHPDP1_IRQDisConnect|eDPTXHPDP1_IRQEvent);    // clear IRQ
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0,eDPTXHPDP1_IRQConnect|eDPTXHPDP1_IRQDisConnect|eDPTXHPDP1_IRQEvent);
    }

    return;
}


//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDP0ClearIRQBit1(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HPDClearIRQDisconnect(eDPTx_ID edptx_id) //poll from Raptor
{
    if(edptx_id == 0)
    {
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP0_IRQDisConnect,eDPTXHPDP0_IRQDisConnect);  // clear IRQ
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP0_IRQDisConnect);
    }else
    {
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP1_IRQDisConnect,eDPTXHPDP1_IRQDisConnect);  // clear IRQ
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP1_IRQDisConnect);

    }
    return;
}



//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDP0ClearIRQBit2(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HPDClearIRQEvent(eDPTx_ID edptx_id) //poll from Raptor
{
    if(edptx_id == 0)
    {
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP0_IRQEvent,eDPTXHPDP0_IRQEvent);  // clear IRQ
        //msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP0_IRQEvent);
    }else
    {
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP1_IRQEvent,eDPTXHPDP1_IRQEvent);  // clear IRQ
        msWrite2ByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP1_IRQEvent);
    }
    return;
}


void mhal_eDPTx_SwingTune(BYTE LaneNum, BYTE SwingValue, BYTE edptx_id)
{
    WORD SwingTune;
    SwingTune=SwingValue&0xFF;

//Control swing of channel
//6'h0: 40mV +/- 15%
//6'h1: 50mV +/- 15%
//6'h2: 60mV +/- 15%
//6'h3f: 670mV +/- 15%
//Output Swing Formula:
//Vout_swing= 40 + 10 * ICON[5:0] mV +/- 15%
// #### DP spec swing Level ##########
// Lv0= Vpp:400mv . set single pair 200mv = 0x0E/0x10/0x12
// Lv1= Vpp:600mv . set single pair 300mv = 0x18/0x1A/0x1D
// Lv2= Vpp:800mv . set single pair 400mv = 0x22/0x24/0x26
// Lv3= Vpp:1200mv . set single pair 600mv = 0x36/0x38/0x3A

    // eDP TX Port#0  L0(ch1) / L1(ch2) / L2(ch3) / L3(ch4)
    if( (edptx_id==0)||(edptx_id==0x80))
    {
        switch(LaneNum)
        {
            case 0x00:
                msWriteByte(REG_EDP_TX_MOD2_50_H,SwingTune);  // lane0 : Swing ch1  8bits : ch1=[15:8]
                break;
            case 0x01:
                msWriteByte(REG_EDP_TX_MOD2_51_L,SwingTune);  // lane1 : Swing ch2  8bits : ch2=[7:0] 
                break;
            case 0x02:
                msWriteByte(REG_EDP_TX_MOD2_51_H,SwingTune);  // lane2 : Swing ch3  8bits : ch3=[15:8]
                break;
            case 0x03:
                msWriteByte(REG_EDP_TX_MOD2_52_L,SwingTune);  // lane3 : Swing ch4  8bits : ch4=[7:0]
                break;
            default: // all Lane config
                msWriteByte(REG_EDP_TX_MOD2_50_H,SwingTune);  // lane0 : Swing ch1  8bits : ch1=[15:8]
                msWriteByte(REG_EDP_TX_MOD2_51_L,SwingTune);  // lane1 : Swing ch2  8bits : ch2=[7:0] 
                msWriteByte(REG_EDP_TX_MOD2_51_H,SwingTune);  // lane2 : Swing ch3  8bits : ch3=[15:8]
                msWriteByte(REG_EDP_TX_MOD2_52_L,SwingTune);  // lane3 : Swing ch4  8bits : ch4=[7:0]
                break;
        }
    }
}

void mhal_eDPTx_PreEmphasisTune(BYTE LaneNum, BYTE PremphasisValue, BYTE edptx_id)
{
    WORD PreTune;
    PreTune=PremphasisValue&0x1F;

// preemphasis adjust
//Differential output data/clock pre-emphasis level adjust of channel0
//3'b000: 0mV +/- 15%
//3'b001: 20mV +/- 15 %
//3'b010: 40mV +/- 15%
//¡K
//3'b111: 140mV +/- 15%
//Pre-emphasis Voltage Formula:
//Vpem = 20* ADJ[2:0] mV +/- 15%

// #### DP spec pre Level ##########
// Lv0= 0.0dB(1x):   support swing Lv 0:1:2:3 => all set Value=0
// Lv1= 3.5dB(1.5x):support swing Lv 0:1:2 => swLv0 set 100mv / swLv1 set 150mv / swLv2 set 200mv
// Lv2= 6.0dB(2x):): support swing Lv 0:1 => swLv0 set 200mv / swLv1 set 300mv
// Lv3= 9.5dB(3x):   support swing Lv 0 => swLv0 set 400mv
// 0mv=0x00,100mv=0x05

    // eDP TX Port#0  L0(ch1) / L1(ch2) / L2(ch3) / L3(ch4)
    if( (edptx_id==0)||(edptx_id==0x80))
    {
        switch(LaneNum)
        {
            case 0x00:
                msWriteByte(REG_EDP_TX_MOD2_20_H,PreTune); // lane0 : PreEmphasis ch1  8bits : ch1=[15:8]
                msWriteByteMask(REG_EDP_TX_MOD1_21_L,PreTune?0x1E:0x00, BIT1); // lane 0 : enable pre-emphasis electric effect
                break;
            case 0x01:
                msWriteByte(REG_EDP_TX_MOD2_21_L,PreTune); // lane1 : PreEmphasis ch2  8bits : ch2=[7:0]
                msWriteByteMask(REG_EDP_TX_MOD1_21_L,PreTune?0x1E:0x00, BIT2); // lane 1 : enable pre-emphasis electric effect
                break;
            case 0x02:
                msWriteByte(REG_EDP_TX_MOD2_21_H,PreTune); // lane2 : PreEmphasis ch3  8bits : ch3=[15:8]
                msWriteByteMask(REG_EDP_TX_MOD1_21_L,PreTune?0x1E:0x00, BIT3); // lane 2 : enable pre-emphasis electric effect
                break;
            case 0x03:
                msWriteByte(REG_EDP_TX_MOD2_22_L,PreTune); // lane4 : PreEmphasis ch4  8bits : ch4=[7:0]
                msWriteByteMask(REG_EDP_TX_MOD1_21_L,PreTune?0x1E:0x00, BIT4); // lane 3 : enable pre-emphasis electric effect
                break;
            default: // all Lane config
                msWriteByte(REG_EDP_TX_MOD2_20_H,PreTune); // lane0 : PreEmphasis ch1  8bits : ch1=[15:8]
                msWriteByte(REG_EDP_TX_MOD2_21_L,PreTune); // lane1 : PreEmphasis ch2  8bits : ch2=[7:0]
                msWriteByte(REG_EDP_TX_MOD2_21_H,PreTune); // lane2 : PreEmphasis ch3  8bits : ch3=[15:8]
                msWriteByte(REG_EDP_TX_MOD2_22_L,PreTune); // lane4 : PreEmphasis ch4  8bits : ch4=[7:0]
                msWriteByteMask(REG_EDP_TX_MOD1_21_L,PreTune?0x1E:0x00, 0x1E); // lane 0~3 : enable pre-emphasis electric effect
                break;
        }
    }
}



//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_PRBS7Enable(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_PRBS7Enable(eDPTx_ID edptx_id, BYTE bEnable)
{

    if(bEnable == 1)
    {
        msWriteByte(REG_EDP_TRANS_DUAL_P0_1F_L  + eDPTX_DUAL_Offset*edptx_id,0xF3);
    }
    else
    {
        msWriteByte(REG_EDP_TRANS_DUAL_P0_1F_L  + eDPTX_DUAL_Offset*edptx_id, 0);           // PRBS7 pat+skew
    }
    return;
}


//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_ProgramPatternEnable(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_ProgramPatternEnable(eDPTx_ID edptx_id, BYTE bEnable)
{
    edptx_id = edptx_id;
    
    if(bEnable == 1)
    {
         msWriteByte(REG_EDP_TRANS_DUAL_P0_0A_L, 0x0F);  //ok         // PLTPAT pat
    }else
    {
        msWriteByte(REG_EDP_TRANS_DUAL_P0_0A_L, 0x00);  //ok         // PLTPAT pat
    }
    return;
}
//**************************************************************************
//  [Function Name]:
//                  mhal_DPTx_ICP_Setting()
//  [Description]
//             mhal_DPTx_ICP_Setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_eDPTx_ICP_Setting(eDPTx_ID edptx_id, BOOL bENABLE)
{
    edptx_id = edptx_id;
    bENABLE = bENABLE;

    return;
}
//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_EyePatternEnable(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_EyePatternEnable(eDPTx_ID edptx_id, BYTE bEnable)
{
    if(bEnable == 1)
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_19_L  + eDPTX_DUAL_Offset*edptx_id, BIT3, BIT3);           // CP2520 pat
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_19_H  + eDPTX_DUAL_Offset*edptx_id, BIT7, BIT7);           // CP2520 skew

    }else
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_19_L  + eDPTX_DUAL_Offset*edptx_id, 0, BIT3);           // CP2520 pat
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_19_H  + eDPTX_DUAL_Offset*edptx_id, 0, BIT7);           // CP2520 skew

    }
    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_CheckHPD(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
DWORD  mhal_eDPTx_CheckHPD(eDPTx_ID edptx_id) //Poll from Raptor
{
    Bool HPDST0=FALSE;
    Bool HPDST1=FALSE;
    Bool Status=FALSE;

    HPDST0=(msReadByte(REG_EDP_TX0_P0_52_L)&(BIT0<<edptx_id))?TRUE:FALSE;
    HPDST1=(msReadByte(REG_EDP_TX0_P0_52_L)&(BIT0<<edptx_id))?TRUE:FALSE;
    Status=(HPDST0|HPDST1)?TRUE:FALSE;  // twice low status return false

    return Status;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_CheckHPDIRQ(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
WORD mhal_eDPTx_CheckHPDIRQ(eDPTx_ID edptx_id) //Poll from Raptor
{
    edptx_id = edptx_id;


    return 0;
/*
  Bool Status=FALSE;


  switch(edptx_id)
  {
    case 0x00:
        Status=(msRead2Byte(REG_EDP_TX0_P0_53_L + eDPTX_Port_Offset_100*edptx_id)&(eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQEvent|eDPTXHPDP0_IRQDisConnect))?TRUE:FALSE;
        break;

    case 0x01:
        Status=(msRead2Byte(REG_EDP_TX0_P0_53_L + eDPTX_Port_Offset_100*edptx_id)&(eDPTXHPDP1_IRQConnect|eDPTXHPDP1_IRQEvent|eDPTXHPDP1_IRQDisConnect))?TRUE:FALSE;
        break;

    default:
        break;
    }
  eDPTX_printData("mhal_eDPTx_CheckHPDIRQ@Status %x",(msRead2Byte(REG_EDP_TX0_P0_53_L + eDPTX_Port_Offset_100*edptx_id)&(eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQEvent|eDPTXHPDP0_IRQDisConnect)));
    return Status;
    */
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_PGSyncRst(void)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void  mhal_eDPTx_PGSyncRst(void) //poll from Raptor
{
    msWrite2Byte(REG_EDP_TX0_P0_1A_L,0x0003); // PG EN +Rst
    msWrite2Byte(REG_EDP_TX1_P0_1A_L,0x0003); // PG EN +Rst
    msWriteByteMask(REG_EDP_TX0_P0_52_H,0x00, BIT5); // Video clk disable
    msWrite2Byte(REG_EDP_TX0_P0_1A_L,0x0001); // PG EN
    msWrite2Byte(REG_EDP_TX1_P0_1A_L,0x0001); // PG EN
    msWriteByteMask(REG_EDP_TX0_P0_52_H,BIT5, BIT5); // Video clk en  able
    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_VDClkRst(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_VDClkRst(void)//poll from Raptor
{
    msWriteByteMask(REG_EDP_TX0_P0_52_H, 0x00, BIT5); // Video clk disable
    msWriteByteMask(REG_EDP_TX0_P0_52_H, BIT5, BIT5); // Video clk en  able
    eDPTX_printMsg("mhal_eDPTx_VDClkRst");
    return;
}


//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDIRQRst(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HPDIRQRst(eDPTx_ID edptx_id)
{
    msWrite2Byte(REG_EDP_TX0_P0_56_L , (0x07<<(edptx_id*3)));  // clear  HPD0~3 IRQ
    msWrite2Byte(REG_EDP_TX0_P0_56_L , 0x0000);  // clear IRQ

    return;
}
//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HPDIRQRst(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HPDIRQ_Handler(eDPTx_ID edptx_id)//poll from Raptor
{
    edptx_id = edptx_id;
    msWrite2Byte(REG_EDP_TX0_P0_56_L, 0x0FFF);  // clear  HPD0~3 IRQ
    msWrite2Byte(REG_EDP_TX0_P0_56_L, 0x0000);  // clear IRQ
    return;
}


//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_EfuseAuxTrimSetting(eDPTx_ID edptx_id, BYTE LinkRate)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************

void mhal_eDPTx_EfuseAuxTrimSetting(eDPTx_ID edptx_id, BYTE LinkRate)
{
    BYTE u8TrimData;

    LinkRate = LinkRate;
    u8TrimData = msEread_GetDataFromEfuse(0x13A);

    if(u8TrimData & BIT7)
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_1D_L + eDPTX_AUX_Offset*edptx_id, (u8TrimData &0x1F),0x1F);  //aux mux
        msWriteByteMask(REG_EDP_AUX_TX_P0_52_L + eDPTX_AUX_Offset*edptx_id, BIT3, BIT3); // enable ov en
    }
    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_EfuseRtermTrimSetting(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************

void mhal_eDPTx_EfuseRtermTrimSetting(eDPTx_ID edptx_id)
{
    edptx_id = edptx_id;
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_ReadEFuseFlow()
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
void  mhal_eDPTx_ReadEFuseFlow(void)
{
    return;
}

#endif

#endif //__MHAL_EDPTX_C_
