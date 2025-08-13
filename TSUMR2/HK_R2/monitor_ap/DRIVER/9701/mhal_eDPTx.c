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
#if 1//PANEL_EDP
#include "datatype.h"
#include "Global.h"
#include "ms_reg.h"
#include "ms_reg_TSUMR2.h"
#include "mhal_eDPTx.h"
#include "msEread.h"
#include "drvmStar.h"
#include "asmCPU.h"



//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#if (eDPTX_DEBUG&&ENABLE_MSTV_UART_DEBUG)
#define eDPTX_printData(str, value)   printData(str, value)
#define eDPTX_printMsg(str)               printMsg(str)
#define eDPTX_printf(str, value)               printf(str, value)
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
ST_EDPTX_INFO g_eDPTxInfo;
eDP_SYSINFO gDPSYSInfo;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

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
    WORD u16DevID;
    msEread_Init(&u16DevID, &gDPSYSInfo.ucCV);
    gDPSYSInfo.bCID = ( u16DevID == 0xFF ? TRUE : FALSE);
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
void mhal_eDPTx_SetHPDOV(eDPTx_ID edptx_id)
{
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_H + eDPTX_AUX_Offset*edptx_id, 0x00, BIT3|BIT2); // [1]Ov Value [0] HPD Soft Ov En
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
void mhal_eDPTx_SetHPDOVHigh(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_AUX_TX_P0_13_H + eDPTX_AUX_Offset*edptx_id, BIT3|BIT2, BIT3|BIT2); // [1]Ov Value [0] HPD Soft Ov En

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
void mhal_eDPTx_SetAtop(eDPTx_ID edptx_id)
{
    msWriteByte(REG_EDP_AUX_TX_P0_1C_H + eDPTX_AUX_Offset*edptx_id, 0x00 ); //wriu 0x112639    0x00
    msWriteByte(REG_EDP_AUX_TX_P0_1F_H + eDPTX_AUX_Offset*edptx_id, 0x14); //wriu 0x11263F  0x14
    msWriteByte(REG_EDP_AUX_TX_P0_0D_H + eDPTX_AUX_Offset*edptx_id, 0x0B);//wriu 0x11261B  0x0B    //  Sample Rate for 12M
    msWriteByteMask(REG_EDP_AUX_TX_P0_05_L + eDPTX_AUX_Offset*edptx_id, 0x07, 0x7F);//wriu 0x11260A  [6:0] 0x07  // UI for 12M
    msWriteByteMask(REG_EDP_AUX_TX_P0_16_L + eDPTX_AUX_Offset*edptx_id, 0, BIT0); // Enable AUXTX  // MST9U3 ONLY

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
void mhal_eDPTx_AuxPNSwap(eDPTx_ID edptx_id)
{
    //## AUXTX P/N swap
    //msWriteByte(REG_DPRX_AUX_TX0_03_H, 0xC0); //[7]:swap //wriu 0x112607  0xC0
    // ## timeout+P/N swap setting

    if(g_eDPTxInfo.u8AuxPNSwap&(BIT0<<edptx_id))
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_03_H + edptx_id*eDPTX_Port_Offset_100, BIT7, BIT7); //AUXRX PN Swap/no Swap
        msWriteByteMask(REG_EDP_AUX_TX_P0_20_L+ edptx_id*eDPTX_Port_Offset_100, BIT0, BIT0); //AUXTX PN Swap/no Swap
    }
    else
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_03_H + edptx_id*eDPTX_Port_Offset_100, 0, BIT7); //AUXRX PN Swap/no Swap
        msWriteByteMask(REG_EDP_AUX_TX_P0_20_L+ edptx_id*eDPTX_Port_Offset_100, 0, BIT0); //AUXTX PN Swap/no Swap
    }
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
void mhal_eDPTx_AuxClkDebunce(eDPTx_ID edptx_id)//keep setting as MST9U4
{
    // ## aux debounce clock
    msWriteByte(REG_EDP_AUX_TX_P0_17_H + eDPTX_AUX_Offset*edptx_id, 0x0E);
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
void mhal_eDPTx_PHYInit(eDPTx_ID edptx_id)
{
/*    msWriteByte(REG_111A1E, 0x08);
    msWriteByte(REG_111A1F, 0x02);
    msWriteByte(REG_111B1E, 0x08);
    msWriteByte(REG_111B1F, 0x02);
    msWriteByte(REG_111C1E, 0x08);
    msWriteByte(REG_111C1F, 0x02);
    msWriteByte(REG_111D1E, 0x08);
    msWriteByte(REG_111D1F, 0x02);
    msWriteByte(REG_111E1E, 0x08);
    msWriteByte(REG_111E1F, 0x02);
    msWriteByte(REG_111F1E, 0x08);
    msWriteByte(REG_111F1F, 0x02);
    msWriteByte(REG_11201E, 0x08);
    msWriteByte(REG_11201F, 0x02);
    msWriteByte(REG_11211E, 0x08);
    msWriteByte(REG_11211F, 0x02);
    msWriteByte(REG_111AAE, 0x02); */
    
    //--------------------------------------
    //Disable QEC/DCC
    //--------------------------------------
    if(g_sPnlInfo.u8PnlEdpHBR2PortEn)
    {
        msWriteByte(REG_EDP_TX_PHY_P0_6C_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_6C_H + eDPTX_PHY_Offset*edptx_id,  0x55);
        msWrite2Byte(REG_EDP_TX_PHY_P0_70_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_71_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_72_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_73_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_74_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_75_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_76_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_77_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_78_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_79_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_7A_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_7B_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_7C_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_7D_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_7E_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
        msWrite2Byte(REG_EDP_TX_PHY_P0_7F_L + eDPTX_PHY_Offset*edptx_id,  0xDF7F);
    
        //--------------------------------------
        //dptx1p3_phy_0/1/2 initial setting
        //--------------------------------------
        //msWriteByte(REG_EDP_TX_PHY_P0_0A_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        //msWriteByte(REG_EDP_TX_PHY_P0_0A_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_10_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_10_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_11_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_11_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_12_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_12_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_14_L + eDPTX_PHY_Offset*edptx_id,  0x02);
        msWriteByte(REG_EDP_TX_PHY_P0_14_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_3B_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_3B_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_3C_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_3C_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_3D_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_3D_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_63_L + eDPTX_PHY_Offset*edptx_id,  0x30);
        msWriteByte(REG_EDP_TX_PHY_P0_63_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_64_L + eDPTX_PHY_Offset*edptx_id,  0x30);
        msWriteByte(REG_EDP_TX_PHY_P0_64_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_65_L + eDPTX_PHY_Offset*edptx_id,  0x30);
        msWriteByte(REG_EDP_TX_PHY_P0_65_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_66_L + eDPTX_PHY_Offset*edptx_id,  0x30);
        msWriteByte(REG_EDP_TX_PHY_P0_66_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_07_L + eDPTX_PHY_Offset*edptx_id,  0x28);
        msWriteByte(REG_EDP_TX_PHY_P0_07_H + eDPTX_PHY_Offset*edptx_id,  0x03);
        //===============================
        // setting by linkrate
        //===============================
        msWriteByte(REG_EDP_TX_PHY_P0_4D_L + eDPTX_PHY_Offset*edptx_id,  0x01);
        msWriteByte(REG_EDP_TX_PHY_P0_4D_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_0F_L + eDPTX_PHY_Offset*edptx_id,  0x08);
        msWriteByte(REG_EDP_TX_PHY_P0_0F_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_13_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_13_H + eDPTX_PHY_Offset*edptx_id,  0x21);
        msWriteByte(REG_EDP_TX_PHY_P0_1D_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_1D_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_67_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_67_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_68_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_68_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_40_L + eDPTX_PHY_Offset*edptx_id,  0x82);
        msWriteByte(REG_EDP_TX_PHY_P0_40_H + eDPTX_PHY_Offset*edptx_id,  0x82);
        msWriteByte(REG_EDP_TX_PHY_P0_41_L + eDPTX_PHY_Offset*edptx_id,  0x82);
        msWriteByte(REG_EDP_TX_PHY_P0_41_H + eDPTX_PHY_Offset*edptx_id,  0x82);
        msWriteByte(REG_EDP_TX_PHY_P0_42_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_42_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_43_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_43_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_44_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_44_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_45_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_45_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_46_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_46_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_47_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_47_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_48_L + eDPTX_PHY_Offset*edptx_id,  0xCC);
        msWriteByte(REG_EDP_TX_PHY_P0_48_H + eDPTX_PHY_Offset*edptx_id,  0xCC);
    
        //--------------------------------------
        //dptx1p3_phy_0/1/2 power down disable
        //--------------------------------------
        msWriteByte(REG_EDP_TX_PHY_P0_0B_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_0B_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_0C_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_0C_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_0D_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_0D_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_0E_L + eDPTX_PHY_Offset*edptx_id,  0x0F);
        msWriteByte(REG_EDP_TX_PHY_P0_0E_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_4A_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_4A_H + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_57_L + eDPTX_PHY_Offset*edptx_id,  0x02);
        msWriteByte(REG_EDP_TX_PHY_P0_08_L + eDPTX_PHY_Offset*edptx_id,  0x20);
    
        //--------------------------------------
        //synth_clkgen_en
        //--------------------------------------
        msWriteByte(REG_EDP_TX_PHY_P0_20_L + eDPTX_PHY_Offset*edptx_id,  0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_20_H + eDPTX_PHY_Offset*edptx_id,  0x10);
        //--------------------------------------
        //CLKI_MPLL setting
        //--------------------------------------
        msWriteByte(REG_LPLL_D8,  0x21);
        msWriteByte(REG_LPLL_D9,  0xA2);
    
        msWriteByte(REG_EDP_TX_PHY_P0_2D_L + eDPTX_PHY_Offset*edptx_id, 0x01);
        msWriteByte(REG_EDP_TX_PHY_P0_2D_H + eDPTX_PHY_Offset*edptx_id, 0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_29_L + eDPTX_PHY_Offset*edptx_id, 0x51);
        msWriteByte(REG_EDP_TX_PHY_P0_29_H + eDPTX_PHY_Offset*edptx_id, 0xB8);
        msWriteByte(REG_EDP_TX_PHY_P0_2A_L + eDPTX_PHY_Offset*edptx_id, 0x1E);
        msWriteByte(REG_EDP_TX_PHY_P0_2A_H + eDPTX_PHY_Offset*edptx_id, 0x00);
        msWriteByte(REG_EDP_TX_PHY_P0_2B_L + eDPTX_PHY_Offset*edptx_id, 0x01);
        msWriteByte(REG_EDP_TX_PHY_P0_2B_H + eDPTX_PHY_Offset*edptx_id, 0x00);
    }
    
    msWriteByte(REG_142B72, 0x2F);
    msWriteByte(REG_142C72, 0x2F);

    if(g_sPnlInfo.u8PnlEdpHBR2PortEn)
    {
        mhal_eDPTx_EfuseRtermTrimSetting(edptx_id);
        mhal_eDPTx_EfuseIbiasTrimSetting(edptx_id);
    }

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
void mhal_eDPTx_MACInit(eDPTx_ID edptx_id)
{
    // edp_encoder0_p0       Bank 1410
    // edp_encoder1_p0       Bank 1411
    // edp_encoder0_p1       Bank 1412
    // edp_encoder1_p1       Bank 1413
    // edp_encoder0_p2       Bank 1414
    // edp_encoder1_p2       Bank 1415
    // edp_encoder0_p3       Bank 1416
    // edp_encoder1_p3       Bank 1417
    // edp_trans_p0          Bank 1418
    // edp_trans_p1          Bank 1419
    // edp_trans_p2          Bank 141A
    // edp_trans_p3          Bank 141B
    // edp_tx_top_gp         Bank 1420
    // edp_tx_dig_top        Bank 1421

    // disp_misc             Bank 1112
    // mft                   Bank 1404
    // mod1                  Bank 1406
    // mod2                  Bank 1407

    // disp misc
    if(g_sPnlInfo.u8PnlEdpHBR2PortEn)
    {
        msWriteByte(REG_EDP_TX_EDP_PHY_DIG_13_L, 0x02);
        msWriteByte(REG_EDP_TX_EDP_PHY_DIG_15_L, 0x44);
    }
    msWriteByte(REG_EDP_TX_MOD_PHY_DIG_71_L, 0x01);

    // clkgen
    msWriteByte(REG_EDP_TX_TOP_GP_00_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_00_H, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_01_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_01_H, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_02_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_02_H, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_03_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_03_H, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_04_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_04_H, 0xff);

    // clkicg
    msWriteByte(REG_EDP_TX_TOP_GP_20_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_20_H, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_21_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_21_H, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_22_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_22_H, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_23_L, 0xff);
    msWriteByte(REG_EDP_TX_TOP_GP_23_H, 0xff);
    if(g_sPnlInfo.u8PnlEdpHBR2PortEn)
    {
        msWriteByte(REG_EDP_TX_TOP_GP_15_L, 0x40);
        msWriteByte(REG_EDP_TX_TOP_GP_15_H, 0x00);
    }
    else
    {
        msWriteByte(REG_EDP_TX_TOP_GP_15_L, 0x44);
        msWriteByte(REG_EDP_TX_TOP_GP_15_H, 0x00);
    }
    msWriteByteMask(REG_EDP_TX_DIG_TOP_41_L, 0, BIT4);//reg_4pllrr_en

    mhal_eDPTx_MuxSwitch(edptx_id);

    msWriteByte(REG_EDP_TX_DIG_TOP_41_H, 0x00);
    msWriteByteMask(REG_EDP_TX_DIG_TOP_34_L, 0x00,BIT7|BIT6);

    msWriteByte(REG_EDP_TRANS_P0_03_H + eDPTX_TRANS_Offset*edptx_id, 0x20);               // reset transmitter for unknown status
    msWriteByte(REG_EDP_TRANS_P0_03_H + eDPTX_TRANS_Offset*edptx_id, 0x00);
    msWriteByte(REG_EDP_TRANS_P1_03_H + eDPTX_TRANS_Offset*edptx_id, 0x20);               // reset transmitter for unknown status
    msWriteByte(REG_EDP_TRANS_P1_03_H + eDPTX_TRANS_Offset*edptx_id, 0x00);

    msWriteByte(REG_EDP_TRANS_P0_40_L + eDPTX_TRANS_Offset*edptx_id, 0x04);               // [1] reg_fec_support
    msWriteByte(REG_EDP_TRANS_P0_40_H + eDPTX_TRANS_Offset*edptx_id, 0x03);
    msWriteByte(REG_EDP_TRANS_P0_06_L + eDPTX_TRANS_Offset*edptx_id, 0x00);               // [12:9] reg_lane3/2/1/0_reset_sw
    msWriteByte(REG_EDP_TRANS_P0_06_H + eDPTX_TRANS_Offset*edptx_id, 0x1e);
    msWriteByte(REG_EDP_TRANS_P0_06_L + eDPTX_TRANS_Offset*edptx_id, 0x00);               // [12:9] reg_lane3/2/1/0_reset_sw
    msWriteByte(REG_EDP_TRANS_P0_06_H + eDPTX_TRANS_Offset*edptx_id, 0x00);

    msWriteByte(REG_EDP_ENCODER0_P0_0F_L + eDPTX_ENCODER_Offset*edptx_id, 0x08);               // [5:0] reg_sram_start_read_thrd
    //////// SDP setting    //////////
    msWriteByte(REG_EDP_ENCODER0_P0_10_L + eDPTX_ENCODER_Offset*edptx_id, 0x20);               // [11:0] reg_sdp_down_cnt_init
    msWriteByte(REG_EDP_ENCODER1_P0_59_L + eDPTX_ENCODER_Offset*edptx_id, 0x20);               // [11:0] reg_sdp_down_cnt_init_in_blank
    msWriteByte(REG_EDP_ENCODER1_P0_2C_L + eDPTX_ENCODER_Offset*edptx_id, 0xef);               // [12:0] reg_sdp_down_asp_cnt_init
    msWriteByte(REG_EDP_ENCODER1_P0_5D_H + eDPTX_ENCODER_Offset*edptx_id, 0x01);

    //////// video stable setting ////
    msWriteByte(REG_EDP_ENCODER1_P0_5A_L + eDPTX_ENCODER_Offset*edptx_id, 0x11);               // [7:4] reg_video_stable_cnt_thrd [2] reg_video_stable_en [1:0] reg_video_sram_fifo_cnt_reset_sel

    //////// DP TX reset    //////////
    msWriteByte(REG_EDP_ENCODER0_P0_01_H + eDPTX_ENCODER_Offset*edptx_id, 0x42);               // [14] reg_dp_tx_mux [9] reg_dp_tx_encoder_4p_reset_sw

    msWriteByte(REG_EDP_ENCODER0_P0_01_H + eDPTX_ENCODER_Offset*edptx_id, 0x00);               // [14] reg_dp_tx_mux [9] reg_dp_tx_encoder_4p_reset_sw



    //////// enhance frame mode //////
    msWriteByte(REG_EDP_ENCODER0_P0_00_L + eDPTX_ENCODER_Offset*edptx_id, 0x1a );              // [4] reg_enhance_frame_en
    msWriteByte(REG_EDP_TRANS_P0_09_H + eDPTX_TRANS_Offset*edptx_id, 0x00 );              // [8] reg_idle_pattern_en

    msWriteByte(REG_EDP_ENCODER0_P0_13_L + eDPTX_ENCODER_Offset*edptx_id, 0x42 );
    msWriteByte(REG_EDP_ENCODER0_P0_0C_L + eDPTX_ENCODER_Offset*edptx_id, 0xFF );
    msWriteByte(REG_EDP_ENCODER0_P0_0C_H + eDPTX_ENCODER_Offset*edptx_id, 0x93 );



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
void mhal_eDPTx_PowerDown(eDPTx_ID edptx_id)
{
    if(g_sPnlInfo.u8PnlEdpHBR2PortEn)
    {
        msWrite2Byte(REG_EDP_TX_PHY_P0_0B_L + eDPTX_PHY_Offset*edptx_id, 0x000C);// [2] : TX PLL POWER DOWN ;  [3]: TX POWER DOWN
        msWrite2Byte(REG_EDP_TX_PHY_P0_0C_L + eDPTX_PHY_Offset*edptx_id, 0x0F0F);// [11:8] eDPTX L0~3 power down
        msWrite2Byte(REG_EDP_TX_PHY_P0_0D_L + eDPTX_PHY_Offset*edptx_id, 0x00F0);// [7:4] eDPTX L0~3 RTerm PD
        msWrite2Byte(REG_EDP_TX_PHY_P0_0E_L + eDPTX_PHY_Offset*edptx_id, 0x0000);
        msWrite2Byte(REG_EDP_TX_PHY_P0_4A_L + eDPTX_PHY_Offset*edptx_id, 0x0040);
        msWrite2Byte(REG_EDP_TX_PHY_P0_48_L + eDPTX_PHY_Offset*edptx_id, 0xFFFF);
        msWrite2Byte(REG_EDP_TX_PHY_P0_57_L + eDPTX_PHY_Offset*edptx_id, 0x3E);
        msWrite2Byte(REG_EDP_TX_PHY_P0_08_L + eDPTX_PHY_Offset*edptx_id, 0x20);
        msWrite2Byte(REG_EDP_TX_PHY_P0_68_L + eDPTX_PHY_Offset*edptx_id, 0x01);
    }
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
    return;
}

//**************************************************************************
//  [Function Name] :  mhal_eDPTx_MuxSwitch(void)
//
//  [Description] : Set Mux switch for clk and MAC
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_MuxSwitch(eDPTx_ID edptx_id)
{
    BOOL b2pto4p_2p_en = FALSE, b2pto4p_4p_en = FALSE, b4pLLRRen = FALSE;
    BYTE u8VideoDataMux0_1 = 0x00;
    BYTE u8VideoDataMux2_3 = 0x00;
    BYTE u8VideoDataMux4_5 = 0x00;
    BYTE u8VideoDataMux6_7 = 0x00;
    Bool b8PLR24POE = 0; //msDrvISSCDualPort()&&(g_eDPTxInfo.u8TotalPorts==0x01);

    if(b8PLR24POE)
    {
        msWriteByteMask(REG_101F28, BIT4, BIT4|BIT5);
        msWriteByte(REG_DISPMISC_20, 0x00);               // [4] reg_ckg_top_odclk_tcon120 [3:0] reg_ckg_vby1_vmode
        msWriteByte(REG_DISPMISC_21, 0x00);
        msWriteByte(REG_DISPMISC_22, 0x04);
        msWriteByte(REG_DISPMISC_23, 0x00);
        msWriteByte(REG_DISPMISC_24, 0x00);               // [9] reg_ckg_mod_odclk_mft_sram [11:8] reg_ckg_mod_sr_wclk [5;4] reg_ckg_mod_sr_rclk [3:0] reg_ckg_mod_odclk_a
        msWriteByte(REG_DISPMISC_25, 0x08);
    }
    else
    {
        msWriteByteMask(REG_101F28, 0x00, BIT4|BIT5);
        // DISP_MISC
        // clock
        msWriteByte(REG_DISPMISC_20, 0x00);               // [4] reg_ckg_top_odclk_tcon120 [3:0] reg_ckg_vby1_vmode
        msWriteByte(REG_DISPMISC_21, 0x00);
        msWriteByte(REG_DISPMISC_22, 0x00);
        msWriteByte(REG_DISPMISC_23, 0x00);
        msWriteByte(REG_DISPMISC_24, 0x00);               // [9] reg_ckg_mod_odclk_mft_sram [11:8] reg_ckg_mod_sr_wclk [5;4] reg_ckg_mod_sr_rclk [3:0] reg_ckg_mod_odclk_a
        msWriteByte(REG_DISPMISC_25, 0x80);
    }
    if(0)//(msDrvISSCDualPort()) // scaler 8P out
    {
        if((g_eDPTxInfo.u8TotalPorts==0x02)||(b8PLR24POE))
        {
            if(g_eDPTxInfo.u8PortSelect==(eDPTX_Port1|eDPTX_Port3))
            {
                if(g_eDPTxInfo.u8PortSWAP)
                {
                    u8VideoDataMux0_1 = 0x76;
                    u8VideoDataMux2_3 = 0x54;
                    u8VideoDataMux4_5 = 0x32;
                    u8VideoDataMux6_7 = 0x10;
                }
                else
                {
                    u8VideoDataMux0_1 = 0x32;
                    u8VideoDataMux2_3 = 0x10;
                    u8VideoDataMux4_5 = 0x76;
                    u8VideoDataMux6_7 = 0x54;
                }
            }
            else if(g_eDPTxInfo.u8PortSelect==(eDPTX_Port1|eDPTX_Port2))
            {
                if(g_eDPTxInfo.u8PortSWAP)
                {
                    u8VideoDataMux0_1 = 0x54;
                    u8VideoDataMux2_3 = 0x76;
                    u8VideoDataMux4_5 = 0x32;
                    u8VideoDataMux6_7 = 0x10;
                }
                else
                {
                    u8VideoDataMux0_1 = 0x32;
                    u8VideoDataMux2_3 = 0x10;
                    u8VideoDataMux4_5 = 0x54;
                    u8VideoDataMux6_7 = 0x76;
                }
            }
            else if(g_eDPTxInfo.u8PortSelect==(eDPTX_Port0|eDPTX_Port3))
            {
                if(g_eDPTxInfo.u8PortSWAP)
                {
                    u8VideoDataMux0_1 = 0x54;
                    u8VideoDataMux2_3 = 0x76;
                    u8VideoDataMux4_5 = 0x32;
                    u8VideoDataMux6_7 = 0x10;
                }
                else
                {
                    u8VideoDataMux0_1 = 0x10;
                    u8VideoDataMux2_3 = 0x32;
                    u8VideoDataMux4_5 = 0x76;
                    u8VideoDataMux6_7 = 0x54;
                }
            }
            else // Port 0/1, 2/3, 0/2
            {
                if(g_eDPTxInfo.u8PortSWAP)
                {
                    u8VideoDataMux0_1 = 0x54;
                    u8VideoDataMux2_3 = 0x76;
                    u8VideoDataMux4_5 = 0x10;
                    u8VideoDataMux6_7 = 0x32;
                }
                else
                {
                    u8VideoDataMux0_1 = 0x10;
                    u8VideoDataMux2_3 = 0x32;
                    u8VideoDataMux4_5 = 0x54;
                    u8VideoDataMux6_7 = 0x76;
                }
                if(g_eDPTxInfo.u8PortSelect==(eDPTX_Port0|eDPTX_Port1)||(g_eDPTxInfo.u8PortSelect==(eDPTX_Port2|eDPTX_Port3)))
                    b4pLLRRen = TRUE;
            }
            b2pto4p_2p_en = FALSE;
            b2pto4p_4p_en = FALSE;
        }
        else if(g_eDPTxInfo.u8TotalPorts==0x04)
        {
            if(g_eDPTxInfo.u8PortSWAP)
            {
                u8VideoDataMux0_1 = 0x76;
                u8VideoDataMux2_3 = 0x54;
                u8VideoDataMux4_5 = 0x32;
                u8VideoDataMux6_7 = 0x10;
            }
            else
            {
                u8VideoDataMux0_1 = 0x10;
                u8VideoDataMux2_3 = 0x32;
                u8VideoDataMux4_5 = 0x54;
                u8VideoDataMux6_7 = 0x76;
            }
            b2pto4p_2p_en = FALSE;
            b2pto4p_4p_en = TRUE;
        }
        else
        {
            u8VideoDataMux0_1 = 0x00;
            u8VideoDataMux2_3 = 0x00;
            u8VideoDataMux4_5 = 0x00;
            u8VideoDataMux6_7 = 0x00;
            b2pto4p_2p_en = FALSE;
            b2pto4p_4p_en = FALSE;
        }
    }
    else
    {
        if(g_eDPTxInfo.u8TotalPorts==0x01)
        {
            b2pto4p_2p_en = FALSE;
            b2pto4p_4p_en = FALSE;

            if((g_eDPTxInfo.u8PortSelect==eDPTX_Port0)||(g_eDPTxInfo.u8PortSelect==eDPTX_Port2))
            {
                u8VideoDataMux0_1 = 0x10;
                u8VideoDataMux2_3 = 0x32;
                u8VideoDataMux4_5 = 0x10;
                u8VideoDataMux6_7 = 0x32;
            }
            else // Port1 / Port3
            {
                u8VideoDataMux0_1 = 0x32;
                u8VideoDataMux2_3 = 0x10;
                u8VideoDataMux4_5 = 0x32;
                u8VideoDataMux6_7 = 0x10;
            }
        }
        else if(g_eDPTxInfo.u8TotalPorts==0x02)
        {
            if((g_eDPTxInfo.u8PortSelect==(eDPTX_Port1|eDPTX_Port2))||(g_eDPTxInfo.u8PortSelect==(eDPTX_Port0|eDPTX_Port3)))
            {
                if(g_eDPTxInfo.u8PortSWAP)
                {
                    u8VideoDataMux0_1 = 0x32;
                    u8VideoDataMux2_3 = 0x32;
                    u8VideoDataMux4_5 = 0x10;
                    u8VideoDataMux6_7 = 0x10;
                }
                else
                {
                    u8VideoDataMux0_1 = 0x10;
                    u8VideoDataMux2_3 = 0x10;
                    u8VideoDataMux4_5 = 0x32;
                    u8VideoDataMux6_7 = 0x32;
                }
            }
            else if((g_eDPTxInfo.u8PortSelect==(eDPTX_Port0|eDPTX_Port1))||(g_eDPTxInfo.u8PortSelect==(eDPTX_Port2|eDPTX_Port3)))//Port 0/1, 2/3
            {
                if(g_eDPTxInfo.u8PortSWAP)
                {
                    u8VideoDataMux0_1 = 0x32;
                    u8VideoDataMux2_3 = 0x10;
                    u8VideoDataMux4_5 = 0x32;
                    u8VideoDataMux6_7 = 0x10;
                }
                else
                {
                    u8VideoDataMux0_1 = 0x10;
                    u8VideoDataMux2_3 = 0x32;
                    u8VideoDataMux4_5 = 0x10;
                    u8VideoDataMux6_7 = 0x32;
                }
            }
            else //Port 0/2, 1/3
            {
                if(g_eDPTxInfo.u8PortSWAP)
                {
                    u8VideoDataMux0_1 = 0x32;
                    u8VideoDataMux2_3 = 0x32;
                    u8VideoDataMux4_5 = 0x10;
                    u8VideoDataMux6_7 = 0x10;
                }
                else
                {
                    u8VideoDataMux0_1 = 0x10;
                    u8VideoDataMux2_3 = 0x10;
                    u8VideoDataMux4_5 = 0x32;
                    u8VideoDataMux6_7 = 0x32;
                }
            }
            b2pto4p_2p_en = FALSE;
            b2pto4p_4p_en = TRUE;
        }
        else if(g_eDPTxInfo.u8TotalPorts==0x04)
        {
            if(g_eDPTxInfo.u8PortSWAP)
            {
                u8VideoDataMux0_1 = 0x23;
                u8VideoDataMux2_3 = 0x00;
                u8VideoDataMux4_5 = 0x01;
                u8VideoDataMux6_7 = 0x00;
            }
            else
            {
                u8VideoDataMux0_1 = 0x10;
                u8VideoDataMux2_3 = 0x00;
                u8VideoDataMux4_5 = 0x32;
                u8VideoDataMux6_7 = 0x00;
            }
            b2pto4p_2p_en = TRUE;
            b2pto4p_4p_en = TRUE;
        }
        else
        {
            u8VideoDataMux0_1 = 0x00;
            u8VideoDataMux2_3 = 0x00;
            u8VideoDataMux4_5 = 0x00;
            u8VideoDataMux6_7 = 0x00;
            b2pto4p_2p_en = FALSE;
            b2pto4p_4p_en = FALSE;
        }
    }
    msWriteByte(REG_EDP_TX_DIG_TOP_42_L, u8VideoDataMux0_1);               // [14:12] [10:8] [6:4] [2:0] video_encode_mux 3 2 1 0
    msWriteByte(REG_EDP_TX_DIG_TOP_42_H, u8VideoDataMux2_3);
    msWriteByte(REG_EDP_TX_DIG_TOP_43_L, u8VideoDataMux4_5);               // [14:12] [10:8] [6:4] [2:0] video_encode_mux 7 6 5 4
    msWriteByte(REG_EDP_TX_DIG_TOP_43_H, u8VideoDataMux6_7);
    msWriteBit(REG_EDP_TX_DIG_TOP_41_L, b4pLLRRen, BIT4);//reg_4pllrr_en

    if(b2pto4p_2p_en&&b2pto4p_4p_en)
    {
        // clk_pix clkmux
        // 4'd6     clk_odclk_div1
        // 4'd7     clk_odclk_div2
        // 4'd8     clk_odclk_div4
        // 4'd9     clk_odclk_div8
        // 4'd10    clk_odclk_div16
        // clk_pix clkmux select
        msWriteByte(REG_EDP_TX_TOP_GP_0C_L, 0x88);
        msWriteByte(REG_EDP_TX_TOP_GP_0C_H, 0x88);

        // clk_pix_mn clkmux select
        msWriteByte(REG_EDP_TX_TOP_GP_18_L, 0x88);
        msWriteByte(REG_EDP_TX_TOP_GP_18_H, 0x88);
        // clk_mft(clk_pix_main) clkmux
        // 3'd0 clk_odclk_div1
        // 3'd1 clk_odclk_div2
        // 3'd2 clk_odclk_div4
        // 3'd3 clk_odclk_div8
        // 3'd4 clk_odclk_div16
        msWriteByteMask(REG_EDP_TX_TOP_GP_13_L ,BIT4, BIT6|BIT5|BIT4|BIT2|BIT1|BIT0 );    // clk_pix_main_div2       , clk_pix_main
        msWriteByteMask(REG_EDP_TX_TOP_GP_13_H ,BIT1, BIT2|BIT1|BIT0);    // reg_tx_mainlink_clk_sel , clk_pix_main_div4
        msWriteByteMask(REG_EDP_TX_DIG_TOP_41_L, BIT2|BIT0, BIT2|BIT0);               // [4] reg_2pto4p_L1_R0_en [3] reg_2pto4p_4p_sel_datain [2]reg_2pto4p_4p_en [1] reg_2pto4p_2p_sel_datain [0]reg_2pto4p_2p_en
    }
    else if(!b2pto4p_2p_en&&b2pto4p_4p_en)
    {
        msWriteByte(REG_MOD1_01, 0x02);

        // clk_pix clkmux select
        msWriteByte(REG_EDP_TX_TOP_GP_0C_L, 0x66);
        msWriteByte(REG_EDP_TX_TOP_GP_0C_H, 0x66);

        // clk_pix_mn clkmux select
        msWriteByte(REG_EDP_TX_TOP_GP_18_L, 0x66);
        msWriteByte(REG_EDP_TX_TOP_GP_18_H, 0x66);
        msWriteByteMask(REG_EDP_TX_TOP_GP_13_L ,0x22, BIT6|BIT5|BIT4|BIT2|BIT1|BIT0 );    // clk_pix_main_div2       , clk_pix_main
        msWriteByteMask(REG_EDP_TX_TOP_GP_13_H ,0x03, BIT2|BIT1|BIT0);    // reg_tx_mainlink_clk_sel , clk_pix_main_div4
        msWriteByteMask(REG_EDP_TX_DIG_TOP_41_L, BIT2, BIT2|BIT0);               // [4] reg_2pto4p_L1_R0_en [3] reg_2pto4p_4p_sel_datain [2]reg_2pto4p_4p_en [1] reg_2pto4p_2p_sel_datain [0]reg_2pto4p_2p_en
    }
    else
    {
        msWriteByte(REG_MOD1_01, 0x02);
        
        // clk_pix clkmux select
        msWriteByte(REG_EDP_TX_TOP_GP_0C_L, 0x66);
        msWriteByte(REG_EDP_TX_TOP_GP_0C_H, 0x66);

        // clk_pix_mn clkmux select
        msWriteByte(REG_EDP_TX_TOP_GP_18_L, 0x66);
        msWriteByte(REG_EDP_TX_TOP_GP_18_H, 0x66);
        msWriteByteMask(REG_EDP_TX_TOP_GP_13_L ,0x22, BIT6|BIT5|BIT4|BIT2|BIT1|BIT0 );    // clk_pix_main_div2       , clk_pix_main
        msWriteByteMask(REG_EDP_TX_TOP_GP_13_H ,0x02, BIT2|BIT1|BIT0);    // reg_tx_mainlink_clk_sel , clk_pix_main_div4
        msWriteByteMask(REG_EDP_TX_DIG_TOP_41_L, 0x00, BIT2|BIT0);               // [4] reg_2pto4p_L1_R0_en [3] reg_2pto4p_4p_sel_datain [2]reg_2pto4p_4p_en [1] reg_2pto4p_2p_sel_datain [0]reg_2pto4p_2p_en

    }

    if(b2pto4p_2p_en||b2pto4p_4p_en)
    {
        msWriteByteMask(REG_EDP_ENCODER1_P0_5A_H + eDPTX_ENCODER_Offset*edptx_id, BIT2, BIT2|BIT1);
    }
    else
    {
        msWriteByteMask(REG_EDP_ENCODER1_P0_5A_H + eDPTX_ENCODER_Offset*edptx_id, 0x00, BIT2|BIT1);
    }
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
void mhal_eDPTx_LanePNSwap(eDPTx_ID edptx_id)
{

    if(g_eDPTxInfo.u8LanePNSwap)
        msWriteByteMask(REG_EDP_TRANS_P0_0A_L + eDPTX_TRANS_Offset*edptx_id, 0xF0, 0xF0);
    else
        msWriteByteMask(REG_EDP_TRANS_P0_0A_L + eDPTX_TRANS_Offset*edptx_id, 0x00, 0xF0);

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
void mhal_eDPTx_LaneSwap(eDPTx_ID edptx_id, BOOL bDataLaneSwapEn, BOOL bSkewSwapEn)
{

    if(bDataLaneSwapEn == TRUE)
    {
        msWriteByte(REG_EDP_TRANS_P0_02_H + eDPTX_TRANS_Offset*edptx_id, 0x1B);
    }
    else
    {
        msWriteByte(REG_EDP_TRANS_P0_02_H + eDPTX_TRANS_Offset*edptx_id, 0xE4); //[15:8] Lane swap [7:0] Skew SWAP
    }

    if(bSkewSwapEn == TRUE)
    {
        msWriteByte(REG_EDP_TRANS_P0_02_L + eDPTX_TRANS_Offset*edptx_id, 0x1B); //for normal panel case
    }
    else
    {
        msWriteByte(REG_EDP_TRANS_P0_02_L + eDPTX_TRANS_Offset*edptx_id, 0xE4); //Skew  no swap for SEC Panel
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
        bENABLE = bENABLE;
        return;

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
void mhal_eDPTx_ConfigLinkRate(BYTE LinkRate, eDPTx_ID edptx_id)
{
    switch(LinkRate)
    {
        case eDPTXHBR3:
            msWriteByteMask(REG_EDP_TX_PHY_P0_2D_L + eDPTX_PHY_Offset*edptx_id, BIT4, BIT0|BIT1|BIT2|BIT3|BIT4);
            msWriteByte(REG_EDP_TX_PHY_P0_13_H + eDPTX_PHY_Offset*edptx_id,  0x21);
            msWriteByte(REG_EDP_TX_PHY_P0_07_L + eDPTX_PHY_Offset*edptx_id,  0x1C);
            msWriteByte(REG_EDP_TX_PHY_P0_07_H + eDPTX_PHY_Offset*edptx_id,  0x02); // [0]HBR2, [1]HBR, [2]RBR, [3]HBR25, [4]HBR3
            msWriteByte(REG_EDP_TX_PHY_P0_48_L + eDPTX_PHY_Offset*edptx_id,  0x88);
            msWriteByte(REG_EDP_TX_PHY_P0_48_H + eDPTX_PHY_Offset*edptx_id,  0x88);
            msWriteByte(REG_EDP_TX_PHY_P0_29_L + eDPTX_PHY_Offset*edptx_id, 0x81);
            msWriteByte(REG_EDP_TX_PHY_P0_29_H + eDPTX_PHY_Offset*edptx_id, 0x4E);
            msWriteByte(REG_EDP_TX_PHY_P0_2A_L + eDPTX_PHY_Offset*edptx_id, 0x1B);

            break;

        case eDPTXHBR25://TBD
            // ANA HBR25 mode
            msWriteByteMask(REG_EDP_TX_PHY_P0_2D_L + eDPTX_PHY_Offset*edptx_id, BIT3, BIT0|BIT1|BIT2|BIT3|BIT4);  // [0]HBR2, [1]HBR, [2]RBR, [3]HBR25, [4]HBR3
            msWriteByteMask(REG_EDP_TX_PHY_P0_2B_L + eDPTX_PHY_Offset*edptx_id, 0, BIT0); // step 1 : Synthesizer overwrite mode : 20160706 set 0 by Jumbo
            //msWrite2Byte(REG_EDP_TX_PHY_P0_29_L + eDPTX_Port_Offset_100*edptx_id, 0x9EBE); // step 2: syth value
            //msWrite2Byte(REG_EDP_TX_PHY_P0_2A_L + eDPTX_Port_Offset_100*edptx_id, 0x001A); // step 2: syth value
            break;

        case eDPTXHBR2:
            msWriteByteMask(REG_EDP_TX_PHY_P0_2D_L + eDPTX_Port_Offset_100*edptx_id, BIT0, BIT0|BIT1|BIT2|BIT3|BIT4);  // [0]HBR2, [1]HBR, [2]RBR, [3]HBR25, [4]HBR3
            msWriteByte(REG_EDP_TX_PHY_P0_13_H + eDPTX_PHY_Offset*edptx_id,  0x21);
            msWriteByte(REG_EDP_TX_PHY_P0_07_L + eDPTX_PHY_Offset*edptx_id,  0x28);
            msWriteByte(REG_EDP_TX_PHY_P0_07_H + eDPTX_PHY_Offset*edptx_id,  0x03);
            msWriteByte(REG_EDP_TX_PHY_P0_48_L + eDPTX_PHY_Offset*edptx_id,  0xCC);
            msWriteByte(REG_EDP_TX_PHY_P0_48_H + eDPTX_PHY_Offset*edptx_id,  0xCC);
            msWriteByte(REG_EDP_TX_PHY_P0_29_L + eDPTX_PHY_Offset*edptx_id, 0x51);
            msWriteByte(REG_EDP_TX_PHY_P0_29_H + eDPTX_PHY_Offset*edptx_id, 0xB8);
            msWriteByte(REG_EDP_TX_PHY_P0_2A_L + eDPTX_PHY_Offset*edptx_id, 0x1E);

            break;

        case eDPTXHBR:
            msWriteByteMask(REG_EDP_TX_PHY_P0_2D_L + eDPTX_Port_Offset_100*edptx_id, BIT1, BIT0|BIT1|BIT2|BIT3|BIT4);  // [0]HBR2, [1]HBR, [2]RBR, [3]HBR25, [4]HBR3
            msWriteByte(REG_EDP_TX_PHY_P0_13_H + eDPTX_PHY_Offset*edptx_id,  0x41);
            msWriteByte(REG_EDP_TX_PHY_P0_07_L + eDPTX_PHY_Offset*edptx_id,  0x28);
            msWriteByte(REG_EDP_TX_PHY_P0_07_H + eDPTX_PHY_Offset*edptx_id,  0x03);
            msWriteByte(REG_EDP_TX_PHY_P0_48_L + eDPTX_PHY_Offset*edptx_id,  0xCC);
            msWriteByte(REG_EDP_TX_PHY_P0_48_H + eDPTX_PHY_Offset*edptx_id,  0xCC);
            msWriteByte(REG_EDP_TX_PHY_P0_29_L + eDPTX_PHY_Offset*edptx_id, 0x51);
            msWriteByte(REG_EDP_TX_PHY_P0_29_H + eDPTX_PHY_Offset*edptx_id, 0xB8);
            msWriteByte(REG_EDP_TX_PHY_P0_2A_L + eDPTX_PHY_Offset*edptx_id, 0x1E);

            break;

        case eDPTXRBR:
            msWriteByteMask(REG_EDP_TX_PHY_P0_2D_L + eDPTX_Port_Offset_100*edptx_id, BIT2, BIT0|BIT1|BIT2|BIT3|BIT4);  // [0]HBR2, [1]HBR, [2]RBR, [3]HBR25, [4]HBR3
            msWriteByte(REG_EDP_TX_PHY_P0_13_H + eDPTX_PHY_Offset*edptx_id,  0x01);
            msWriteByte(REG_EDP_TX_PHY_P0_07_L + eDPTX_PHY_Offset*edptx_id,  0x28);
            msWriteByte(REG_EDP_TX_PHY_P0_07_H + eDPTX_PHY_Offset*edptx_id,  0x03);
            msWriteByte(REG_EDP_TX_PHY_P0_48_L + eDPTX_PHY_Offset*edptx_id,  0xCC);
            msWriteByte(REG_EDP_TX_PHY_P0_48_H + eDPTX_PHY_Offset*edptx_id,  0xCC);
            msWriteByte(REG_EDP_TX_PHY_P0_29_L + eDPTX_PHY_Offset*edptx_id, 0x99);
            msWriteByte(REG_EDP_TX_PHY_P0_29_H + eDPTX_PHY_Offset*edptx_id, 0x99);
            msWriteByte(REG_EDP_TX_PHY_P0_2A_L + eDPTX_PHY_Offset*edptx_id, 0x19);

            break;

        default:
            break;
    }

    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_SSCEnable(BYTE LinkRate, eDPTx_ID edptx_id)
//
//  [Description] : SSC table for 0.1%, 0.2%, 0.3%, 0.4, 0.5%
//
//  [Arguments] : LinkRate, edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SSCEnable(eDPTx_ID edptx_id, BYTE LinkRate, BYTE bEnable)
{
    DWORD dwStep;
    WORD wSpan;
    DWORD dwFactor;
    BYTE freqMod = g_eDPTxInfo.u8SSCModulation;
    BYTE range = g_eDPTxInfo.u8SSCPercentage;


    if( g_sPnlInfo.u8PnlEdpHBR2PortEn )
    {
        if( LinkRate == eDPTXHBR2 )
            dwFactor = 0x1EB851;
        else if( LinkRate == eDPTXHBR )
            dwFactor = 0x1EB851;
        else //RBR
            dwFactor = 0x199999;
    
        if( (freqMod == 0) || (range == 0) )
        {
            eDPTX_printMsg("eDPTX SSC Disable\r\n");
            msWriteByteMask(REG_EDP_TX_PHY_P0_2B_L+ eDPTX_PHY_Offset*edptx_id,0,BIT1); // Down Spread
            msWriteByteMask(REG_EDP_TX_PHY_P0_25_L+ eDPTX_PHY_Offset*edptx_id,0,BIT0); // SSC DISEN
            msWrite2Byte(REG_EDP_TX_PHY_P0_23_L+ eDPTX_PHY_Offset*edptx_id,0x0000); // [11:0] Step   +/- 0.20%
            msWrite2Byte(REG_EDP_TX_PHY_P0_24_L+ eDPTX_PHY_Offset*edptx_id,0x0000); // [13:0] Span   +/- 0.20%
            return;
        }
    
        // freqMod: 0~40 means 0~40K
        if( freqMod > PANEL_SSC_MODULATION_MAX )
            freqMod = PANEL_SSC_MODULATION_MAX;
        else
            freqMod = g_eDPTxInfo.u8SSCModulation;
    
        // range: 0~30 means 0~3%
        if( range > PANEL_SSC_PERCENTAGE_MAX )
            range = PANEL_SSC_PERCENTAGE_MAX;
        else
            range = g_eDPTxInfo.u8SSCPercentage;
    
        wSpan = (((float)432000/freqMod)*131072ul)/dwFactor;
        dwStep = (float)(dwFactor*range/10)/wSpan/100;
    
        if( bEnable )
        {
            eDPTX_printMsg("eDPTX SSC Enable\r\n");
            msWrite2Byte(REG_EDP_TX_PHY_P0_23_L+ eDPTX_PHY_Offset*edptx_id,dwStep); // [11:0] Step   0.1%~0.5%
            msWrite2Byte(REG_EDP_TX_PHY_P0_24_L+ eDPTX_PHY_Offset*edptx_id,wSpan); // [13:0] Span   0.1%~0.5%
            msWriteByteMask(REG_EDP_TX_PHY_P0_2B_L+ eDPTX_PHY_Offset*edptx_id,BIT1,BIT1);  //// Down Spread
            msWriteByteMask(REG_EDP_TX_PHY_P0_25_L+ eDPTX_PHY_Offset*edptx_id,BIT0,BIT0); // SSC EN
        }
        else
        {
            eDPTX_printMsg("eDPTX SSC Disable\r\n");
            msWriteByteMask(REG_EDP_TX_PHY_P0_2B_L+ eDPTX_PHY_Offset*edptx_id,0,BIT1); // Down Spread
            msWriteByteMask(REG_EDP_TX_PHY_P0_25_L+ eDPTX_PHY_Offset*edptx_id,0,BIT0); // SSC DISEN
            msWrite2Byte(REG_EDP_TX_PHY_P0_23_L+ eDPTX_PHY_Offset*edptx_id,0x0000); // [11:0] Step   +/- 0.20%
            msWrite2Byte(REG_EDP_TX_PHY_P0_24_L+ eDPTX_PHY_Offset*edptx_id,0x0000); // [13:0] Span   +/- 0.20%
        }
    }
    else
    {
        DWORD xdata u32MPLL_MHZ = 864ul;

        if( LinkRate == eDPTX_RBR )
        {
            dwFactor = (float)((float)(u32MPLL_MHZ)*1000ull*16/msDrvGetLinkRateLoopDiv(LinkRate))*((float)524288UL*(float)msDrvGetLinkRateLoopGain(LinkRate)/162000);
        }
        else
        {
            dwFactor = (float)((float)(u32MPLL_MHZ)*1000ull*16/msDrvGetLinkRateLoopDiv(LinkRate))*((float)524288UL*(float)msDrvGetLinkRateLoopGain(LinkRate)/270000);
        }

        if( (freqMod == 0) || (range == 0) )
        {
            eDPTX_printMsg("eDPTX SSC Disable\r\n");
           
            msWrite4Byte(REG_LPLL_90, dwFactor);            
            msWrite2Byte(REG_LPLL_2E, 0);
            msWrite2ByteMask(REG_LPLL_30, 0, 0x3FFF);
            msWriteBit(REG_LPLL_1B, 0, BIT3); // ssc enable
            return;
        }
    
        // freqMod: 0~40 means 0~40K
        if( freqMod > PANEL_SSC_MODULATION_MAX )
            freqMod = PANEL_SSC_MODULATION_MAX;
    
        // range: 0~30 means 0~3%
        if( range > PANEL_SSC_PERCENTAGE_MAX )
            range = PANEL_SSC_PERCENTAGE_MAX;

        if( bEnable ) 
            dwFactor += dwFactor*range/1000;
        
        wSpan = (((float)(u32MPLL_MHZ)*1000ull*16/freqMod)*131072ull)/dwFactor;
        dwStep = ((float)(dwFactor)*range/10)/wSpan/100; // 120320 coding modified
    
        if( dwStep > 0xFFFF )
            dwStep = 0xFFFF;
    
        if( wSpan > 0x3FFF )
            wSpan = 0x3FFF;

        msWrite4Byte(REG_LPLL_90, dwFactor);
        msWrite2Byte(REG_LPLL_2E, dwStep);
        msWrite2ByteMask(REG_LPLL_30, wSpan, 0x3FFF);
        msWriteBit(REG_LPLL_1B, (bEnable?BIT3:0), BIT3); // ssc enable
    }
    
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
void mhal_eDPTx_ColorMISC(eDPTx_ID edptx_id)
{
    msWriteByteMask(REG_EDP_TX0_P0_17_H + eDPTX_Port_Offset_300*edptx_id, BIT3, BIT3); // set color by MISC register
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
    //mhal_eDPTx_EfuseAuxTrimSetting(edptx_id);
    msWriteByteMask(REG_EDP_AUX_TX_P0_03_H + eDPTX_AUX_Offset*edptx_id, 0x0E, 0x1F);    // modify timeout threshold [12:8]
    msWriteByte(REG_EDP_AUX_TX_P0_03_L + eDPTX_AUX_Offset*edptx_id, 0xA6);    // modify timeout threshold [7:0]

    //for Aux Init
    msWriteByte(REG_EDP_AUX_TX_P0_24_L + eDPTX_AUX_Offset*edptx_id, 0x04);
    msWriteByte(REG_EDP_AUX_TX_P0_0D_H + eDPTX_AUX_Offset*edptx_id, 0x0B);
    msWriteByte(REG_EDP_AUX_TX_P0_05_L + eDPTX_AUX_Offset*edptx_id, 0x07);

    //For Aux Analog
    msWriteByte(REG_EDP_AUX_TX_P0_1F_H + eDPTX_AUX_Offset*edptx_id, 0x14);
    msWriteByte(REG_EDP_AUX_TX_P0_1C_H + eDPTX_AUX_Offset*edptx_id, 0x00);
    msWriteByte(REG_EDP_AUX_TX_P0_16_L + eDPTX_AUX_Offset*edptx_id, 0x80);
    //msWriteByte(REG_EDP_AUX_TX_P0_03_H + eDPTX_AUX_Offset*edptx_id, 0x60);

    msWriteByte(REG_EDP_TX_TOP_GP_09_L, 0x55);
    msWriteByte(REG_EDP_TX_TOP_GP_09_H, 0x55);
    msWriteByte(REG_EDP_TX_TOP_GP_0A_L, 0x55);
    //msWriteByte(REG_EDP_TX_TOP_GP_0A_H, 0x55);
    msWriteByte(REG_EDP_TX_TOP_GP_22_L, 0xFF);
    msWriteByte(REG_EDP_TX_TOP_GP_22_H, 0x0F);
    msWriteByte(REG_EDP_TX_TOP_GP_00_L, 0x0f);

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
void mhal_eDPTx_ColorDepth(eDPTx_ID edptx_id)
{
    if(g_eDPTxInfo.u8ColorDepth==4) //6bits
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_0D_L + eDPTX_ENCODER_Offset*edptx_id, (0x00 << 0x5), 0xE0); //MISC0
        msWriteByteMask(REG_EDP_ENCODER0_P0_0F_H + eDPTX_ENCODER_Offset*edptx_id, (4), 0x07);
    }
    else if(g_eDPTxInfo.u8ColorDepth == 3)//8bits
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_0D_L + eDPTX_ENCODER_Offset*edptx_id, (0x01 << 0x5), 0xE0); //MISC0
        msWriteByteMask(REG_EDP_ENCODER0_P0_0F_H + eDPTX_ENCODER_Offset*edptx_id, (3), 0x07);
    }
    else if(g_eDPTxInfo.u8ColorDepth == 2)//10bits
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_0D_L + eDPTX_ENCODER_Offset*edptx_id, (0x02 << 0x5), 0xE0); //MISC0
        msWriteByteMask(REG_EDP_ENCODER0_P0_0F_H + eDPTX_ENCODER_Offset*edptx_id, (2), 0x07);
    }
    else//12bits
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_0D_L + eDPTX_ENCODER_Offset*edptx_id, (0x03 << 0x5), 0xE0); //MISC0
        msWriteByteMask(REG_EDP_ENCODER0_P0_0F_H + eDPTX_ENCODER_Offset*edptx_id, (1), 0x07);
    }
    return;
}

//**************************************************************************
//  [Function Name] : mhal_eDPTx_SetFreeSyncOn(BYTE PortNum
//                  )
//  [Description] : Set TU and DE only mode
//
//  [Arguments] : edptx_id
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_SetFreeSyncOn(eDPTx_ID edptx_id)
{
    msWriteByteMask(REG_EDP_ENCODER1_P0_5A_H + eDPTX_ENCODER_Offset*edptx_id, BIT5|BIT4, BIT5|BIT4); //20180529 : set for freesync always on, BS align to Hsync
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
    edptx_id = edptx_id;
    return;
    //msWriteByteMask(REG_EDP_ENCODER1_P0_56_L + eDPTX_ENCODER_Offset*edptx_id, 0, BIT7); // TU Oven disable
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
void mhal_eDPTx_FineTuneTU(eDPTx_ID edptx_id, DWORD eDPTX_OUTBL_PixRateKhz)  //MST9U6 use HW TU only.
{
    edptx_id = edptx_id;
    eDPTX_OUTBL_PixRateKhz = eDPTX_OUTBL_PixRateKhz;
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
void mhal_eDPTx_TimingSet(eDPTx_ID edptx_id)
{
    Bool b8PLR24POE = 0; //msDrvISSCDualPort()&&(g_eDPTxInfo.u8TotalPorts==0x01);

    if(b8PLR24POE)
    {
        msWrite2Byte(REG_EDP_ENCODER0_P0_04_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16Htt);  // Htotal
        msWrite2Byte(REG_EDP_ENCODER0_P0_05_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16Vtt); // Vtotal
        msWrite2Byte(REG_EDP_ENCODER0_P0_06_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16HStart); // HStart
        msWrite2Byte(REG_EDP_ENCODER0_P0_07_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16VStart); // Vstart
        msWrite2Byte(REG_EDP_ENCODER0_P0_08_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16Width); //Hactive
        msWrite2Byte(REG_EDP_ENCODER0_P0_19_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16Width); //Hactive
        msWrite2Byte(REG_EDP_ENCODER0_P0_09_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16Height);    // Vactive 2160
        msWrite2Byte(REG_EDP_ENCODER0_P0_0A_L + eDPTX_ENCODER_Offset*edptx_id,((eDPTXMSA_HSP*0x0001)+(g_sPnlInfo.sPnlTiming.u16HSyncWidth << 1)));    // Hsp|Hsw 20  //20171227 : [0]polar, need <<1bit to match RX received
        msWrite2Byte(REG_EDP_ENCODER0_P0_0B_L + eDPTX_ENCODER_Offset*edptx_id,((eDPTXMSA_VSP*0x0001)+(g_sPnlInfo.sPnlTiming.u16VSyncWidth << 1))); // Vsp|Vsw  5  //20171227 : [0]polar, need <<1bit to match RX received
    }
    else
    {
        msWrite2Byte(REG_EDP_ENCODER0_P0_04_L + eDPTX_ENCODER_Offset*edptx_id,g_sPnlInfo.sPnlTiming.u16Htt/g_eDPTxInfo.u8TotalPorts);  // Htotal
        msWrite2Byte(REG_EDP_ENCODER0_P0_05_L + eDPTX_ENCODER_Offset*edptx_id,g_sPnlInfo.sPnlTiming.u16Vtt); // Vtotal
        msWrite2Byte(REG_EDP_ENCODER0_P0_06_L + eDPTX_ENCODER_Offset*edptx_id,g_sPnlInfo.sPnlTiming.u16HStart/g_eDPTxInfo.u8TotalPorts); // HStart
        msWrite2Byte(REG_EDP_ENCODER0_P0_07_L + eDPTX_ENCODER_Offset*edptx_id,g_sPnlInfo.sPnlTiming.u16VStart); // Vstart
        msWrite2Byte(REG_EDP_ENCODER0_P0_08_L + eDPTX_ENCODER_Offset*edptx_id,g_sPnlInfo.sPnlTiming.u16Width/g_eDPTxInfo.u8TotalPorts); //Hactive
        msWrite2Byte(REG_EDP_ENCODER0_P0_19_L + eDPTX_ENCODER_Offset*edptx_id,g_sPnlInfo.sPnlTiming.u16Width/g_eDPTxInfo.u8TotalPorts); //Hactive
        msWrite2Byte(REG_EDP_ENCODER0_P0_09_L + eDPTX_ENCODER_Offset*edptx_id,g_sPnlInfo.sPnlTiming.u16Height);    // Vactive 2160
        msWrite2Byte(REG_EDP_ENCODER0_P0_0A_L + eDPTX_ENCODER_Offset*edptx_id,eDPTXMSA_HSP*0x0001+(g_sPnlInfo.sPnlTiming.u16HSyncWidth << 1)/g_eDPTxInfo.u8TotalPorts);    // Hsp|Hsw 20  //20171227 : [0]polar, need <<1bit to match RX received
        msWrite2Byte(REG_EDP_ENCODER0_P0_0B_L + eDPTX_ENCODER_Offset*edptx_id,eDPTXMSA_VSP*0x0001+(g_sPnlInfo.sPnlTiming.u16VSyncWidth << 1)); // Vsp|Vsw  5  //20171227 : [0]polar, need <<1bit to match RX received
    }
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
void mhal_eDPTx_VideoPG(eDPTx_ID edptx_id)
{
    // ################ For PG Setting ###############################
    msWrite2Byte(REG_EDP_ENCODER0_P0_55_L + eDPTX_ENCODER_Offset*edptx_id, g_sPnlInfo.sPnlTiming.u16Htt); // HTT
    msWrite2Byte(REG_EDP_ENCODER0_P0_56_L + eDPTX_ENCODER_Offset*edptx_id, (g_sPnlInfo.sPnlTiming.u16Htt-g_sPnlInfo.sPnlTiming.u16Width-g_sPnlInfo.sPnlTiming.u16HStart) ); // H FP
    msWrite2Byte(REG_EDP_ENCODER0_P0_57_L + eDPTX_ENCODER_Offset*edptx_id,  g_sPnlInfo.sPnlTiming.u16HSyncWidth); // H sync width
    msWrite2Byte(REG_EDP_ENCODER0_P0_58_L + eDPTX_ENCODER_Offset*edptx_id,  (g_sPnlInfo.sPnlTiming.u16Htt-g_sPnlInfo.sPnlTiming.u16Width)); // H Blank
    msWrite2Byte(REG_EDP_ENCODER0_P0_59_L + eDPTX_ENCODER_Offset*edptx_id,  g_sPnlInfo.sPnlTiming.u16Width); // HDE

    msWrite2Byte(REG_EDP_ENCODER0_P0_5A_L + eDPTX_ENCODER_Offset*edptx_id,  g_sPnlInfo.sPnlTiming.u16Vtt); // VTT
    msWrite2Byte(REG_EDP_ENCODER0_P0_5B_L + eDPTX_ENCODER_Offset*edptx_id,  (g_sPnlInfo.sPnlTiming.u16Vtt-g_sPnlInfo.sPnlTiming.u16Height-g_sPnlInfo.sPnlTiming.u16VStart)); // V FP
    msWrite2Byte(REG_EDP_ENCODER0_P0_5C_L + eDPTX_ENCODER_Offset*edptx_id,  g_sPnlInfo.sPnlTiming.u16VSyncWidth); // V Sync Width
    msWrite2Byte(REG_EDP_ENCODER0_P0_5D_L + eDPTX_ENCODER_Offset*edptx_id,  (g_sPnlInfo.sPnlTiming.u16Vtt-g_sPnlInfo.sPnlTiming.u16Height)); // V Blank
    msWrite2Byte(REG_EDP_ENCODER0_P0_5E_L + eDPTX_ENCODER_Offset*edptx_id,  g_sPnlInfo.sPnlTiming.u16Height); // VDE
    msWriteByteMask(REG_EDP_ENCODER0_P0_50_H + eDPTX_ENCODER_Offset*edptx_id,  BIT5,BIT5); // reg_pgen_tg_sel

    return;
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
void mhal_eDPTx_PGEnable( eDPTx_ID edptx_id, BOOL bEnable, BYTE u8Pattern)
{
    msWriteByteMask(REG_EDP_ENCODER0_P0_6C_L + eDPTX_ENCODER_Offset*edptx_id, u8Pattern, BIT6|BIT5|BIT4);//reg_pgen_pattern_sel
    if(bEnable)
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_0E_H + eDPTX_ENCODER_Offset*edptx_id, BIT3, BIT3);    //  enable TX pattern gen
    }
    else
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_0E_H + eDPTX_ENCODER_Offset*edptx_id, 0, BIT3);//  disable TX pattern gen
    }

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
    return;
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
void mhal_eDPTx_AuxClearIRQ(eDPTx_ID edptx_id)
{
    msWriteByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id, 0x7F); // [6:0] Clear irq
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
void mhal_eDPTx_AuxReadCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR)
{
    msWriteByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id, 0x7F); // [6:0] Clear irq
    //## AUXTX CMD
    MAsm_CPU_DelayMs(1);
    msWriteByteMask(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] Complete CMD ->Clear Buf
    msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x09, 0x0F); // [3:0] CMD
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x00); // [7:4] Req Len[3:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id, 0x00, BIT0); // [0] No Length Command
    msWriteByteMask(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08, BIT3); // [3] AUX TX Fire CMD
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
void mhal_eDPTx_AuxReadCMDBytes(eDPTx_ID edptx_id, BYTE Length, DWORD DPCDADDR)
{
    //## AUXTX CMD
    msWriteByteMask(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] Complete CMD ->Clear Buf
    msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x09, 0x0F); // [3:0] CMD
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id, 0x00, BIT0); // [0] No Length Command
    msWriteByteMask(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08, BIT3); // [3] AUX TX Fire CMD
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
Bool mhal_eDPTx_CheckWritePoint(eDPTx_ID edptx_id)
{
    return (msReadByte(REG_EDP_AUX_TX_P0_06_L + eDPTX_AUX_Offset*edptx_id)&0x0F);
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
Bool mhal_eDPTx_CheckAuxRxFull(eDPTx_ID edptx_id)
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
Bool mhal_eDPTx_AuxRxReplyCMD(eDPTx_ID edptx_id)
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
void mhal_eDPTx_TxComplete(eDPTx_ID edptx_id)
{
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01);
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
void mhal_eDPTx_TrigerReadPulse0(eDPTx_ID edptx_id)
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
void mhal_eDPTx_TrigerReadPulse1(eDPTx_ID edptx_id)
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
BYTE mhal_eDPTx_AuxReadData(eDPTx_ID edptx_id)
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
void mhal_eDPTx_AuxWriteCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData)
{
#if AUX_NEW_MODE_EN
    msWriteByteMask(REG_EDP_AUX_TX_P0_41_L + eDPTX_AUX_Offset*edptx_id, BIT2, BIT2); // Set AUX new mode
#endif
    msWriteByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id, 0x7F); // [6:0] Clear irq
    //## AUXTX CMD
    MAsm_CPU_DelayMs(1);
    msWriteByteMask(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] Complete CMD ->Clear Buf
    msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x08, 0x0F); // [3:0] CMD
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
#if AUX_NEW_MODE_EN
    msWriteByte(REG_EDP_AUX_TX_P0_42_L + (eDPTX_AUX_Offset*edptx_id),wData);
#else
    msWriteByte(REG_EDP_AUX_TX_P0_0D_L + eDPTX_AUX_Offset*edptx_id,wData); // [7:0] AUX TX Write DATA BUF
#endif
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x00); // [7:4] Req Len[3:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id, 0x00, BIT0); // [0] No Length Command
#if AUX_NEW_MODE_EN
    msWriteByteMask(REG_EDP_AUX_TX_P0_41_L + eDPTX_AUX_Offset*edptx_id, BIT1, BIT1); // Set new mode toggle load data to buffer
#endif
    msWriteByteMask(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08, BIT3); // [3] AUX TX Fire CMD
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
void mhal_eDPTx_AuxWriteCMDBytes(eDPTx_ID edptx_id, DWORD DPCDADDR)
{
    //## AUXTX CMD
#if AUX_NEW_MODE_EN
    msWriteByteMask(REG_EDP_AUX_TX_P0_41_L + eDPTX_AUX_Offset*edptx_id, BIT2, BIT2); // Set AUX new mode
#endif
    msWriteByteMask(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] Complete CMD ->Clear Buf
    msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x08, 0x0F); // [3:0] CMD
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
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
Bool mhal_eDPTx_AuxRXCompleteInterrupt(eDPTx_ID edptx_id)
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
    return (msReadByte(REG_EDP_AUX_TX_P0_10_L + eDPTX_AUX_Offset*edptx_id)&BIT6)? TRUE : FALSE;
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
void mhal_eTPTx_AUXTXWriteBuffer(eDPTx_ID edptx_id, BYTE LengthCnt ,BYTE *pTxBuf)
{
#if AUX_NEW_MODE_EN
    msWriteByte(REG_EDP_AUX_TX_P0_42_L + (eDPTX_AUX_Offset*edptx_id) + LengthCnt,*(pTxBuf+LengthCnt));
#else
    msWriteByte(REG_EDP_AUX_TX_P0_0D_L + eDPTX_AUX_Offset*edptx_id,*(pTxBuf+LengthCnt)); // [7:0] AUX TX Write DATA BUF
#endif
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
void mhal_eTPTx_AUXTXWriteData(eDPTx_ID edptx_id, BYTE Length)
{
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByte(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id, 0x00); // [0] No Length Command
#if AUX_NEW_MODE_EN
    msWriteByteMask(REG_EDP_AUX_TX_P0_41_L + eDPTX_AUX_Offset*edptx_id, BIT1, BIT1); // Set new mode toggle load data to buffer
#endif
    msWriteByte(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08); // [3] AUX TX Fire CMD
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
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByte(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,EOF&0x01); // [0] No Length Command
#if AUX_NEW_MODE_EN
    msWriteByteMask(REG_EDP_AUX_TX_P0_41_L + eDPTX_AUX_Offset*edptx_id, BIT1, BIT1); // Set new mode toggle load data to buffer
#endif
    msWriteByte(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08); // [3] AUX TX Fire CMD
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
void mhal_eDPTx_AuxEDIDReadNLCMD(eDPTx_ID edptx_id, DWORD DPCDADDR)
{
    //## AUXTX CMD
    msWriteByteMask(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] Complete CMD ->Clear Buf
    msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x01, 0x0F); // [3:0] CMD , MOT=0
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x00); // [7:4] Req Len[3:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] No Length Command
    msWriteByteMask(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08, BIT3); // [3] AUX TX Fire CMD
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
void mhal_eDPTx_AuxEDIDWriteByteCMD(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData)
{
    //## AUXTX CMD
    msWriteByteMask(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] Complete CMD ->Clear Buf
    msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x04, 0x0F); // [3:0] CMD ,MOT=1
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
    msWriteByte(REG_EDP_AUX_TX_P0_0D_L + eDPTX_AUX_Offset*edptx_id,wData); // [7:0] AUX TX Write DATA BUF
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x00); // [7:4] Req Len[3:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id, 0x00, BIT0); // [0] No Length Command
    msWriteByteMask(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08, BIT3); // [3] AUX TX Fire CMD
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
void mhal_eDPTx_AuxEDIDReadBytesCMD(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE Length)
{
    //## AUXTX CMD
    msWriteByteMask(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id, 0x01, BIT0); // [0] Complete CMD ->Clear Buf
    msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x05, 0x0F); // [3:0] CMD , MOT=1
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,DPCDADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(DPCDADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id, 0x00, BIT0); // [0] No Length Command
    msWriteByteMask(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08, BIT3); // [3] AUX TX Fire CMD
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
    if (EOF)
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x01, 0x0F);// [3:0] CMD , MOT=0
    }
    else
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x05, 0x0F);// [3:0] CMD , MOT=1
    }
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,eDPMCCSADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(eDPMCCSADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(eDPMCCSADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
    msWriteByte(REG_EDP_AUX_TX_P0_14_H + eDPTX_AUX_Offset*edptx_id,(Length-1)<<4); // [7:4] Req Len[3:0]
    msWriteByteMask(REG_EDP_AUX_TX_P0_0B_L + eDPTX_AUX_Offset*edptx_id,EOF&0x01, BIT0); // [0] No Length Command
    msWriteByteMask(REG_EDP_AUX_TX_P0_0C_L + eDPTX_AUX_Offset*edptx_id, 0x08, BIT3); // [3] AUX TX Fire CMD
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
    if(EOF)
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x00, 0x0F);// [3:0] CMD  MOT= 0
    }

    else
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_11_L + eDPTX_AUX_Offset*edptx_id, 0x04, 0x0F);// [3:0] CMD  MOT=1
    }
    msWriteByte(REG_EDP_AUX_TX_P0_12_L + eDPTX_AUX_Offset*edptx_id,eDPMCCSADDR&0x000000FF); // [7:0] ADR[7:0]
    msWriteByte(REG_EDP_AUX_TX_P0_12_H + eDPTX_AUX_Offset*edptx_id,(eDPMCCSADDR>>8)&0x000000FF); // [7:0] ADR[15:8]
    msWriteByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id,(eDPMCCSADDR>>16)&0x0000000F, 0x0F); // [3:0] ADR[19:16]
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
void mhal_eDPTx_HPDInitSet(eDPTx_ID edptx_id)
{
    if( g_sPnlInfo.u8PnlEdpHBR2PortEn )
    {
        msWriteByte(REG_MOD1_50, 0x01);
        msWriteByte(REG_MOD1_51, 0x00);
        msWriteByte(REG_MOD1_84, 0x00);
        msWriteByte(REG_MOD1_85, 0x00);
    }
    else
    {
        msWriteByte(REG_MOD1_4E, 0x6C);
        msWriteByte(REG_MOD1_4F, 0x00);
        msWriteByte(REG_MOD1_50, 0x00);
        msWriteByte(REG_MOD1_51, 0x00);
        msWriteByte(REG_MOD1_84, 0x00);
        msWriteByte(REG_MOD1_85, 0x00);
    }
    
    msWriteByte(REG_MOD1_30, 0x00);               // reg_ext_en
    msWriteByte(REG_MOD1_31, 0x00);               // reg_ext_en
    msWriteByte(REG_MOD1_32, 0x00);               // reg_ext_en
    msWriteByte(REG_MOD1_33, 0x00);               // reg_ext_en
    msWriteByte(REG_MOD1_28, 0xff);               // reg_gpo_oez
    msWriteByte(REG_MOD1_29, 0xff);               // reg_gpo_oez
    msWriteByte(REG_MOD1_2A, 0xff);               // reg_gpo_oez
    msWriteByte(REG_MOD1_2B, 0xff);               // reg_gpo_oez
    
    //HPD pin from GPIO to HPD mode
    msWriteByteMask(REG_000417, BIT0<<(edptx_id+4), BIT0<<(edptx_id+4)); //HPD pin from GPIO to HPD mode
    //Choose 10M Xtal
    msWriteByteMask(REG_EDP_AUX_TX_P0_1B_H + eDPTX_AUX_Offset*edptx_id, 0x30, 0xFF);
    
    // TBD Adjust Tx detect Rx HPD up/lower bound
    //msWriteByteMask(REG_DPTX_TRANS_04_L + eDPTX_Port_Offset_100*edptx_id , 0x8, 0x0F);
    
    //Adjust Tx reg_hpd_disc_thd to 2ms, it is because of the spec. "HPD pulse" description
    //Low Bound: 3'b010 ~ 500us
    //Up Bound: 3'b110 ~1.9ms
    msWrite2ByteMask(REG_EDP_AUX_TX_P0_13_L + eDPTX_AUX_Offset*edptx_id, 0x0320  , 0x03F0);

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
void mhal_eDPTx_MLSignalDisable(eDPTx_ID edptx_id)
{
    mhal_eDPTx_SetPHY_IdlePattern(edptx_id, TRUE);
    //eDPTX_printMsg("eDPTX ML Signal Off ##\r\n");
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
void mhal_eDPTx_MLSignalEnable(eDPTx_ID edptx_id)
{
    msWriteByteMask(REG_EDP_TRANS_DUAL_P0_60_H + eDPTX_TRANS_Offset*edptx_id, 0x00, BIT7|BIT6|BIT5|BIT4); // [7:4]  L3~0 Ch Data Ov Dis
    msWriteByte(REG_EDP_CLKGEN_13_L, 0x90);
    //eDPTX_printMsg("eDPTX ML Signal On ##\r\n");
    return;
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
void mhal_eDPTx_ASSR(eDPTx_ID edptx_id,BOOL bENABLE)
{
    if (bENABLE==1)
    {
        msWriteByteMask(REG_EDP_TRANS_P0_01_L + eDPTX_TRANS_Offset*edptx_id, BIT1, BIT1);
    }
    else
    {
        msWriteByteMask(REG_EDP_TRANS_P0_01_L + eDPTX_TRANS_Offset*edptx_id, 0, BIT1);
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
void mhal_eDPTx_AFR(eDPTx_ID edptx_id,BOOL bENABLE)
{
    edptx_id = edptx_id;
    bENABLE = bENABLE;

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
void mhal_eDPTx_EnhanceFrameMode(eDPTx_ID edptx_id,BOOL bENABLE)
{

    if (bENABLE==1)
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_00_L + eDPTX_ENCODER_Offset*edptx_id, BIT4, BIT4);
    }
    else
    {
        msWriteByteMask(REG_EDP_ENCODER0_P0_00_L + eDPTX_ENCODER_Offset*edptx_id, 0, BIT4);
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
void mhal_eDPTx_SetLaneCnt(eDPTx_ID edptx_id, BYTE LaneCount)
{

    msWriteByteMask(REG_EDP_ENCODER0_P0_00_L + eDPTX_ENCODER_Offset*edptx_id,LaneCount/2, BIT1|BIT0);
    msWriteByteMask(REG_EDP_TRANS_P0_29_L + eDPTX_TRANS_Offset*edptx_id,((LaneCount/2) << 2), BIT3|BIT2);

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
    if(bENABLE)
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_60_H + eDPTX_Port_Offset_300*edptx_id, BIT7|BIT6|BIT5|BIT4, BIT7|BIT6|BIT5|BIT4);
    }
    else
    {
        msWriteByteMask(REG_EDP_TRANS_DUAL_P0_60_H + eDPTX_Port_Offset_300*edptx_id, 0, BIT7|BIT6|BIT5|BIT4);
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
    //  0x163B_4A[2]  // 0:disable PHY idle data
    if(bENABLE) // if Set TPS1
    {
        msWriteByteMask(REG_EDP_TX_PHY_P0_4A_L + eDPTX_PHY_Offset*edptx_id, BIT2, BIT2);  // enable PHY idle data
    }  // enable PHY idle data
    else
    {
        msWriteByteMask(REG_EDP_TX_PHY_P0_4A_L + eDPTX_PHY_Offset*edptx_id, 0, BIT2);  // disable PHY idle data
    }
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
void mhal_eDPTx_TrainingPattern_Select(BYTE TP_value, eDPTx_ID edptx_id)
{

    msWriteByte(REG_EDP_TRANS_P0_10_L + eDPTX_TRANS_Offset*edptx_id, 0x00);
    msWriteByte(REG_EDP_TRANS_P0_10_H + eDPTX_TRANS_Offset*edptx_id, 0x00);
    msWriteByte(REG_EDP_TRANS_P0_11_L + eDPTX_TRANS_Offset*edptx_id, 0x00);
    msWriteByte(REG_EDP_TRANS_P0_11_H + eDPTX_TRANS_Offset*edptx_id, 0x00);

    switch (TP_value)
    {
        case(0) :
            msWriteByteMask(REG_EDP_TRANS_P0_00_H + eDPTX_TRANS_Offset*edptx_id, 0, BIT7|BIT6|BIT5|BIT4);

            break;

        case(1) :
            msWriteByteMask(REG_EDP_TRANS_P0_00_H + eDPTX_TRANS_Offset*edptx_id, BIT4, BIT7|BIT6|BIT5|BIT4);

            break;

        case(2) :
            msWriteByteMask(REG_EDP_TRANS_P0_00_H + eDPTX_TRANS_Offset*edptx_id, BIT5, BIT7|BIT6|BIT5|BIT4);

            break;

        case(3) :
            msWriteByteMask(REG_EDP_TRANS_P0_00_H + eDPTX_TRANS_Offset*edptx_id, BIT6, BIT7|BIT6|BIT5|BIT4);

            break;

        case(4) :
            msWriteByteMask(REG_EDP_TRANS_P0_00_H + eDPTX_TRANS_Offset*edptx_id, BIT7, BIT7|BIT6|BIT5|BIT4);

            break;

        default :
            break;

    }

    return;
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
    return;
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
void mhal_eDPTx_FMTOutputEnable(void)
{
    msWrite2Byte(REG_EDP_TX0_P0_52_L, 0x7030); // (including clock enable and MN gen)
    msWrite2ByteMask(REG_EDP_TX0_P0_29_L, 0x8000, BIT15); // Block 1<->2 swap
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
WORD mhal_eDPTx_HPDStatus(WORD HPDStatus, eDPTx_ID edptx_id)
{
    WORD u16HPDStatus;

    u16HPDStatus = (msReadByte(REG_EDP_AUX_TX_P0_02_L + eDPTX_AUX_Offset*edptx_id))&(BIT0|BIT2);

    u16HPDStatus |= (((msReadByte(REG_EDP_AUX_TX_P0_02_H + eDPTX_AUX_Offset*edptx_id))&BIT2)>>1);

    HPDStatus = HPDStatus;
    return(u16HPDStatus); //[5]interrup [6]conn [7]

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
    edptx_id = edptx_id;

    Bool bHPD = (((msReadByte(REG_EDP_AUX_TX_P0_13_H + eDPTX_AUX_Offset*edptx_id))&BIT7)>>7);

    return bHPD;
}

#if eDPTXDETECTHPD

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
void mhal_eDPTx_HPDClearIRQBitAll(eDPTx_ID edptx_id)
{


    msWrite2Byte(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id ,0xFFFF);  // clear IRQ
    msWrite2Byte(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id ,0x00);


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
void mhal_eDPTx_HPDClearIRQConnect(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id , BIT0,BIT0);  // clear IRQ
    msWriteByteMask(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id , 0x00,BIT0);

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
void mhal_eDPTx_HPDClearIRQDisconnect(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_AUX_TX_P0_1A_H + eDPTX_AUX_Offset*edptx_id , BIT2,BIT2);  // clear IRQ
    msWriteByteMask(REG_EDP_AUX_TX_P0_1A_H + eDPTX_AUX_Offset*edptx_id , 0x00,BIT2);

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
void mhal_eDPTx_HPDClearIRQEvent(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id , BIT2,BIT2);  // clear IRQ
    msWriteByteMask(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id , 0x00,BIT2);

    return;

}

#endif
//**************************************************************************
//  [Function Name] :
//                 void mhal_eDPTx_HBR2DRVMAINTune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HBR2DRVMAINTune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
{
    switch(LaneNum)
    {
        case 0x00:
            msWriteByte(REG_EDP_TX_PHY_P0_40_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L0
            break;

        case 0x01:
            msWriteByte(REG_EDP_TX_PHY_P0_40_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L1
            break;

        case 0x02:
            msWriteByte(REG_EDP_TX_PHY_P0_41_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L2
            break;

        case 0x03:
            msWriteByte(REG_EDP_TX_PHY_P0_41_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;

        default: // all Lane config
            msWriteByte(REG_EDP_TX_PHY_P0_40_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L0
            msWriteByte(REG_EDP_TX_PHY_P0_40_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L1
            msWriteByte(REG_EDP_TX_PHY_P0_41_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L2
            msWriteByte(REG_EDP_TX_PHY_P0_41_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;
    }
    return;
}

//**************************************************************************
//  [Function Name] :
//                 void mhal_eDPTx_HBR2DRVPRETune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HBR2DRVPRETune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
{
    switch(LaneNum)
    {
        case 0x00:
            msWriteByte(REG_EDP_TX_PHY_P0_42_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L0
            break;

        case 0x01:
            msWriteByte(REG_EDP_TX_PHY_P0_42_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L1
            break;

        case 0x02:
            msWriteByte(REG_EDP_TX_PHY_P0_43_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L2
            break;

        case 0x03:
            msWriteByte(REG_EDP_TX_PHY_P0_43_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;

        default: // all Lane config
            msWriteByte(REG_EDP_TX_PHY_P0_42_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L0
            msWriteByte(REG_EDP_TX_PHY_P0_42_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L1
            msWriteByte(REG_EDP_TX_PHY_P0_43_L + eDPTX_PHY_Offset*edptx_id,SetValue); // L2
            msWriteByte(REG_EDP_TX_PHY_P0_43_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;
    }
    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_HBR2PREDRVMAINTune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HBR2PREDRVMAINTune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
{
    switch(LaneNum)
    {
        case 0x00:
            msWriteByte(REG_EDP_TX_PHY_P0_44_L + eDPTX_PHY_Offset*edptx_id,SetValue);           // L0
            break;

        case 0x01:
            msWriteByte(REG_EDP_TX_PHY_P0_44_H + eDPTX_PHY_Offset*edptx_id,SetValue);  // L1
            break;

        case 0x02:
            msWriteByte(REG_EDP_TX_PHY_P0_45_L + eDPTX_PHY_Offset*edptx_id,SetValue);          // L2
            break;

        case 0x03:
            msWriteByte(REG_EDP_TX_PHY_P0_45_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;

        default: // all Lane config
            msWriteByte(REG_EDP_TX_PHY_P0_44_L + eDPTX_PHY_Offset*edptx_id,SetValue);           // L0
            msWriteByte(REG_EDP_TX_PHY_P0_44_H + eDPTX_PHY_Offset*edptx_id,SetValue);  // L1
            msWriteByte(REG_EDP_TX_PHY_P0_45_L + eDPTX_PHY_Offset*edptx_id,SetValue);          // L2
            msWriteByte(REG_EDP_TX_PHY_P0_45_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;
    }
    return;
}

//**************************************************************************
//  [Function Name] :
//                 void mdrv_eDPTx_HBR2PREDRVPRETune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_HBR2PREDRVPRETune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id)
{
    switch(LaneNum)
    {
        case 0x00:
            msWriteByte(REG_EDP_TX_PHY_P0_46_L + eDPTX_PHY_Offset*edptx_id,SetValue);          // L0
            break;

        case 0x01:
            msWriteByte(REG_EDP_TX_PHY_P0_46_H + eDPTX_PHY_Offset*edptx_id,SetValue);  // L1
            break;

        case 0x02:
            msWriteByte(REG_EDP_TX_PHY_P0_47_L + eDPTX_PHY_Offset*edptx_id,SetValue);         // L2
            break;

        case 0x03:
            msWriteByte(REG_EDP_TX_PHY_P0_47_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;

        default: // all Lane config
            msWriteByte(REG_EDP_TX_PHY_P0_46_L + eDPTX_PHY_Offset*edptx_id,SetValue);         // L0
            msWriteByte(REG_EDP_TX_PHY_P0_46_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L1
            msWriteByte(REG_EDP_TX_PHY_P0_47_L + eDPTX_PHY_Offset*edptx_id,SetValue);          // L2
            msWriteByte(REG_EDP_TX_PHY_P0_47_H + eDPTX_PHY_Offset*edptx_id,SetValue); // L3
            break;
    }
    return;
}

void mhal_eDPTx_MODSwingTune(BYTE LaneNum, BYTE SwingValue, BYTE edptx_id)
{
    WORD SwingTune;
    SwingTune=SwingValue&0xFF;
    BYTE u8Offset=4;

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
    switch(LaneNum)
    {
        case 0x00:
            msWriteByte(REG_MOD2_A7+u8Offset*edptx_id,SwingTune);  // lane0 : Swing ch1  8bits : ch1=[15:8]
            break;
        case 0x01:
            msWriteByte(REG_MOD2_A8+u8Offset*edptx_id,SwingTune);  // lane1 : Swing ch2  8bits : ch2=[7:0] 
            break;
        case 0x02:
            msWriteByte(REG_MOD2_A9+u8Offset*edptx_id,SwingTune);  // lane2 : Swing ch3  8bits : ch3=[15:8]
            break;
        case 0x03:
            msWriteByte(REG_MOD2_AA+u8Offset*edptx_id,SwingTune);  // lane3 : Swing ch4  8bits : ch4=[7:0]
            break;
        default: // all Lane config
            msWriteByte(REG_MOD2_A7+u8Offset*edptx_id,SwingTune);  // lane0 : Swing ch1  8bits : ch1=[15:8]
            msWriteByte(REG_MOD2_A8+u8Offset*edptx_id,SwingTune);  // lane1 : Swing ch2  8bits : ch2=[7:0] 
            msWriteByte(REG_MOD2_A9+u8Offset*edptx_id,SwingTune);  // lane2 : Swing ch3  8bits : ch3=[15:8]
            msWriteByte(REG_MOD2_AA+u8Offset*edptx_id,SwingTune);  // lane3 : Swing ch4  8bits : ch4=[7:0]
            break;
    }
}

void mhal_eDPTx_MODPreEmphasisTune(BYTE LaneNum, BYTE PremphasisValue, BYTE edptx_id)
{
    WORD PreTune;
    PreTune=PremphasisValue&0x1F;
    BYTE u8Offset=4;

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
                msWriteByte(REG_MOD2_47+u8Offset*edptx_id,PreTune); // lane0 : PreEmphasis ch1  8bits : ch1=[15:8]
                //msWriteByteMask(REG_MOD2_42,PreTune?0x1E:0x00, BIT1); // lane 0 : enable pre-emphasis electric effect
                break;
            case 0x01:
                msWriteByte(REG_MOD2_48+u8Offset*edptx_id,PreTune); // lane1 : PreEmphasis ch2  8bits : ch2=[7:0]
                //msWriteByteMask(REG_MOD2_42,PreTune?0x1E:0x00, BIT2); // lane 1 : enable pre-emphasis electric effect
                break;
            case 0x02:
                msWriteByte(REG_MOD2_49+u8Offset*edptx_id,PreTune); // lane2 : PreEmphasis ch3  8bits : ch3=[15:8]
                //msWriteByteMask(REG_MOD2_42,PreTune?0x1E:0x00, BIT3); // lane 2 : enable pre-emphasis electric effect
                break;
            case 0x03:
                msWriteByte(REG_MOD2_4A+u8Offset*edptx_id,PreTune); // lane4 : PreEmphasis ch4  8bits : ch4=[7:0]
                //msWriteByteMask(REG_MOD2_42,PreTune?0x1E:0x00, BIT4); // lane 3 : enable pre-emphasis electric effect
                break;
            default: // all Lane config
                msWriteByte(REG_MOD2_47+u8Offset*edptx_id,PreTune); // lane0 : PreEmphasis ch1  8bits : ch1=[15:8]
                msWriteByte(REG_MOD2_48+u8Offset*edptx_id,PreTune); // lane1 : PreEmphasis ch2  8bits : ch2=[7:0]
                msWriteByte(REG_MOD2_49+u8Offset*edptx_id,PreTune); // lane2 : PreEmphasis ch3  8bits : ch3=[15:8]
                msWriteByte(REG_MOD2_4A+u8Offset*edptx_id,PreTune); // lane4 : PreEmphasis ch4  8bits : ch4=[7:0]
                //msWriteByteMask(REG_MOD2_42,PreTune?0x1E:0x00, 0x1E); // lane 0~3 : enable pre-emphasis electric effect
                break;
        }
    }
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_PRBS7Disable(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_PRBS7Disable(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_TRANS_P0_11_L + eDPTX_TRANS_Offset*edptx_id, 0, BIT3);

    return;
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
void mhal_eDPTx_PRBS7Enable(eDPTx_ID edptx_id)
{


    msWriteByteMask(REG_EDP_TRANS_P0_11_L + eDPTX_TRANS_Offset*edptx_id, BIT3|BIT2, BIT3|BIT2);
    msWrite2Byte(REG_EDP_TRANS_P0_10_L + eDPTX_TRANS_Offset*edptx_id, 0x444F);

    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_EyePatternDisable(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************
void mhal_eDPTx_EyePatternDisable(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_TRANS_P0_1E_L + eDPTX_TRANS_Offset*edptx_id, 0, BIT0);

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
void mhal_eDPTx_ProgramPatternEnable(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_TRANS_P0_10_L + eDPTX_TRANS_Offset*edptx_id, 0x0F, BIT3|BIT2|BIT1|BIT0);

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
void mhal_eDPTx_EyePatternEnable(eDPTx_ID edptx_id)
{

    msWriteByteMask(REG_EDP_TRANS_P0_1E_L + eDPTX_TRANS_Offset*edptx_id, BIT0, BIT0);

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
DWORD  mhal_eDPTx_CheckHPD(eDPTx_ID edptx_id)
{
    return(msReadByte(REG_EDP_AUX_TX_P0_13_H + eDPTX_AUX_Offset*edptx_id));
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
WORD mhal_eDPTx_CheckHPDIRQ(eDPTx_ID edptx_id)
{
    WORD u16HPDStatus;

    u16HPDStatus = msReadByte(REG_EDP_AUX_TX_P0_02_L + eDPTX_AUX_Offset*edptx_id)&(BIT0|BIT2);
    u16HPDStatus = (u16HPDStatus|((msReadByte(REG_EDP_AUX_TX_P0_02_H + eDPTX_AUX_Offset*edptx_id)&BIT2)>>1));

    return(u16HPDStatus); //[9]interrup [8]conn [7]Disconnect
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
void  mhal_eDPTx_PGSyncRst(void)
{
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
void mhal_eDPTx_VDClkRst(void)
{

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
    msWrite2ByteMask(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id, 0xFFFF, 0xFFFF);  // clear  HPD0~3 IRQ
    msWrite2ByteMask(REG_EDP_AUX_TX_P0_1A_L + eDPTX_AUX_Offset*edptx_id, 0x0000, 0xFFFF);  // clear  HPD0~3 IRQ
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

void mhal_eDPTx_EfuseAuxTrimSetting(eDPTx_ID edptx_id)
{
    BYTE u8TrimData = 0; //msEread_GetDataFromEfuse(EFUSE_0, 0x13A+edptx_id);
    BOOL bTrimFlag = 0; //msEread_GetDataFromEfuse(EFUSE_0, 0x13A+edptx_id)&BIT7;

    if(bTrimFlag)
    {
        msWriteByteMask(REG_EDP_AUX_TX_P0_1D_L + eDPTX_AUX_Offset*edptx_id,u8TrimData, 0x1F);
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
    BYTE u8TrimData = msEread_GetDataFromEfuse(0x135)&0x0F;
    BOOL bTrimFlag = msEread_GetDataFromEfuse(0x134)&BIT5;

    edptx_id = edptx_id;
    
    if(bTrimFlag)
    {
        msWriteByteMask(REG_EDP_TX_PHY_P0_0E_L, u8TrimData, 0x0F);
    }
    return;
}

//**************************************************************************
//  [Function Name] :
//                 mhal_eDPTx_EfuseIbiasTrimSetting(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] : None
//
//**************************************************************************

void mhal_eDPTx_EfuseIbiasTrimSetting(eDPTx_ID edptx_id)
{
    BYTE u8TrimData = 0;
    BOOL bTrimFlag = 0;

    edptx_id = edptx_id;
    u8TrimData = msEread_GetDataFromEfuse(0x135)&0xF0;
    bTrimFlag = msEread_GetDataFromEfuse(0x134)&BIT6;
    if(bTrimFlag)
        msWriteByte(REG_EDP_TX_PHY_P0_61_L, (u8TrimData>>4));

    u8TrimData = msEread_GetDataFromEfuse(0x15D)&0x0F;
    bTrimFlag = msEread_GetDataFromEfuse(0x15E)&BIT4;
    if(bTrimFlag)
        msWriteByte(REG_EDP_TX_PHY_P0_61_H, u8TrimData);

    u8TrimData = msEread_GetDataFromEfuse(0x15D)&0xF0;
    bTrimFlag = msEread_GetDataFromEfuse(0x15E)&BIT5;
    if(bTrimFlag)
        msWriteByte(REG_EDP_TX_PHY_P0_62_L, (u8TrimData>>4));

    u8TrimData = msEread_GetDataFromEfuse(0x15E)&0x0F;
    bTrimFlag = msEread_GetDataFromEfuse(0x15E)&BIT6;
    if(bTrimFlag)
        msWriteByte(REG_EDP_TX_PHY_P0_62_H, u8TrimData);

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


#if 0//(eDP_HDCP13 == 0x1)

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPAnCode(eDPTx_ID edptx_id, BYTE i, BYTE ubAnValue)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
void  mhal_eDPTx_HDCPAnCode(eDPTx_ID edptx_id, BYTE i, BYTE ubAnValue)
{
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPInCode(eDPTx_ID edptx_id, BYTE i, BYTE Lm)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
void  mhal_eDPTx_HDCPInCode(eDPTx_ID edptx_id, BYTE i, BYTE Lm)
{
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPInSeed(eDPTx_ID edptx_id, BYTE temp)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
void  mhal_eDPTx_HDCPInSeed(eDPTx_ID edptx_id, BYTE temp)
{
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPCipherAuth(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
void  mhal_eDPTx_HDCPCipherAuth(eDPTx_ID edptx_id)
{
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPr0Available(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
void  mhal_eDPTx_HDCPr0Available(eDPTx_ID edptx_id)
{
    return;
}

//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPr0L(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
BYTE  mhal_eDPTx_HDCPr0L(eDPTx_ID edptx_id)
{
    return 0;
}


//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPr0H(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
BYTE  mhal_eDPTx_HDCPr0H(eDPTx_ID edptx_id)
{
    return 0;
}
//**************************************************************************
//  [Function Name] :
//                  mhal_eDPTx_HDCPFrameEn(eDPTx_ID edptx_id)
//  [Description]
//
//  [Arguments] :
//
//  [Return] :
//
//**************************************************************************
void  mhal_eDPTx_HDCPFrameEn(eDPTx_ID edptx_id)
{
    return;
}

#endif //eDP_HDCP13


#endif //PANEL_EDP

#endif //_MHAL_EDPTX_C_
