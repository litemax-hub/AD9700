////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
///////////////////////////////////////////////////////////////////////////////

#ifndef MHAL_DPRXPHY_C
#define MHAL_DPRXPHY_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "dpCommon.h"
#include "mhal_DPRx.h"
#include "mhal_DPRx_phy.h"
#include "Ms_rwreg.h"
#include "msEread.h"

#if (ENABLE_DP_INPUT == 0x1)
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define DP_HAL_PHY_DEBUG_MESSAGE	0

#if(DP_DEBUG_MESSAGE && DP_HAL_PHY_DEBUG_MESSAGE)
#define DP_HAL_PHY_DPUTSTR(str)		printMsg(str)
#define DP_HAL_PHY_DPRINTF(str, x)	printData(str, x)
#else
#define DP_HAL_PHY_DPUTSTR(str)
#define DP_HAL_PHY_DPRINTF(str, x)
#endif
BYTE XDATA Current_rate[DPRx_PHY_ID_MAX] = {0};
static BOOL record_current_eq = false;
#define HALF_RATE_FLAG 1

static WORD usTemp1 = 0;
static BYTE ubTemp2 = 0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
//**************************************************************************
//  [Function Name]:
//                  mhal_Switch_NormalTraining_Setting()
//  [Description]
//					mhal_Switch_NormalTraining_Setting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_Switch_Training_Setting(DPRx_PHY_ID dprx_phy_id, BYTE link_rate, BOOL bAutoTest)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffsetPHY1ByID = DP_REG_OFFSET400(dprx_phy_id);
    //WORD usRegOffsetPHY2ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffsetPHY3ByID = DP_REG_OFFSET400(dprx_phy_id);
    if(Current_rate[dprx_phy_id]==0x14) //current rate 14h:5.4 Gbps per lane
    {
        if(link_rate==0x0A) // link_rate from reg_dpcd_00100h 0Ah: 2.7Gbps per lane
        {
            msWriteByteMask(REG_DPRX_PHY0_23_L + usRegOffsetPHY0ByID, DP_HBRRBR_EQ_MAX, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
#ifdef HALF_RATE_FLAG
            // if(dprx_phy_id == 0 )
            // {
            msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x8888 ); // Initial script by ryan
            //reg_test_lane_l0/1/2/3
            msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, BIT3, BIT3 );  // by Ryan
            msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x00); // Initial script by Ryan

            if(bAutoTest == FALSE)
            {
                msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x07); // Initial script by Ryan
            }

            // }
            /* else if(dprx_phy_id == 1 )
             {
                 msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x9999);// Initial script by Ryan
                 // Bank 1623 -> PHY3
                 //reg_test_lane_l0/1/2/3
                 msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, 0, BIT3 );  // by Ryan
                 msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x02); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x00); // Initial script by Ryan
             }*/
#endif
            //reg_test_dfe_msb_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_12_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_13_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            //reg_dlpf_kp_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_21_L + usRegOffsetPHY3ByID, 0x4444); // Initial script by OG
            //PGA negative C current
            msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0X11); //  by Ava
            msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x11); //  by Ava
            msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x11); //  by Ava
            msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x11); //  by Ava
            msWrite2Byte(REG_DPRX_PHY1_6B_L + usRegOffsetPHY1ByID, 0xAAAA);//default: 0x82CC Only HBR2 Setting,Initial script by OG
            //RBR only 0x80
            msWriteByte(REG_DPRX_PHY3_60_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_61_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_62_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_63_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
            // Bank 1623 -> PHY3
            // Bank 1621 -> PHY1
            //reg_test_eq_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            // Bank 1623 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_22_L + usRegOffsetPHY3ByID, 0x7777); // Initial script by OG
            //reg_gc_eqrs_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_32_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_33_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_34_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_35_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x0110); // Initial script by OG
            ////pd_vdac_ov hbr/rbr:1
            msWriteByte(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, 0x03); // Initial script by OG
            ////HBR/RBR bypass dfe and agc autoscan2
            msWriteByte(REG_DPRX_PHY0_35_L + usRegOffsetPHY0ByID, 0x33); // Initial script by OG
            //msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x0000 ); // Initial script by ryan
            Current_rate[dprx_phy_id] = 0x0A;//0Ah: 2.7Gbps per lane
        }
        else if(link_rate==0x06) // link_rate from reg_dpcd_00100h 06h: 1.62Gbps per lane
        {
            msWriteByteMask(REG_DPRX_PHY0_23_L + usRegOffsetPHY0ByID, DP_HBRRBR_EQ_MAX, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
#ifdef HALF_RATE_FLAG
            msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x9999);// Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_lane_l0/1/2/3
            msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, 0, BIT3 );  // by Ryan
#endif
            //reg_test_dfe_msb_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_12_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_13_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            //reg_dlpf_kp_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_21_L + usRegOffsetPHY3ByID, 0x4444); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG

            msWrite2Byte(REG_DPRX_PHY1_6B_L + usRegOffsetPHY1ByID, 0xAAAA);//default: 0x82CC Only HBR2 Setting,Initial script by OG
            msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x02); // Initial script by OG
            //RBR only
            msWriteByte(REG_DPRX_PHY3_60_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_61_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_62_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_63_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG

            // Bank 1621 -> PHY1
            //reg_test_eq_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_ch_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x0101); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x0101); // Initial script by OG
            //reg_test_active_sw_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_40_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_41_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_42_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_43_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            // Bank 1621 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            // Bank 1623 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_22_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            //reg_gc_eqrs_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_32_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_33_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_34_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_35_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x0310); // Initial script by OG
            ////pd_vdac_ov hbr/rbr:1
            msWriteByte(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, 0x03); // Initial script by OG
            ////HBR/RBR bypass dfe and agc autoscan2
            msWriteByte(REG_DPRX_PHY0_35_L + usRegOffsetPHY0ByID, 0x33); // Initial script by OG
            Current_rate[dprx_phy_id] = 0x06;//06h: 1.62Gbps per lane
        }
    }
    else if(Current_rate[dprx_phy_id]==0x0A) //current rate 0Ah:2.7 Gbps per lane
    {
        if(link_rate==0x14) // link_rate from reg_dpcd_00100h 14h: 5.4Gbps per lane
        {
            msWriteByteMask(REG_DPRX_PHY0_23_L + usRegOffsetPHY0ByID, DP_HBR23_EQ_MAX, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
#ifdef HALF_RATE_FLAG
            msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x8888);// Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_lane_l0/1/2/3
            msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, 0, BIT3 );  // by Ryan
#endif
            //reg_test_dfe_msb_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_12_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_13_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            //reg_dlpf_kp_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_21_L + usRegOffsetPHY3ByID, 0x6666); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x01); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_6B_L + usRegOffsetPHY1ByID, 0x8888);//default: 0x82CC Only HBR2 Setting,Initial script by OG

            //RBR only 0x80
            msWriteByte(REG_DPRX_PHY3_60_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_61_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_62_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_63_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG

            msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG

            //msWrite2ByteMask(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
            //msWrite2ByteMask(REG_DPRX_PHY3_14_H + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
            //msWrite2ByteMask(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
            //msWrite2ByteMask(REG_DPRX_PHY3_15_H + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava

            // Bank 1621 -> PHY1
            //reg_test_eq_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG

            // Bank 1623 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_22_L + usRegOffsetPHY3ByID, 0xEEEE); // Initial script by OG
            //reg_gc_eqrs_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_32_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_33_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_34_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_35_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            //=================================================================================
            //		pd_vdac_ov hbr/rbr:1
            //=================================================================================
            //PGA negative C current
            msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0X01); //  by Ava
            msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x01); //  by Ava
            msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x01); //  by Ava
            msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x01); //  by Ava
            msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x1110); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
            //reg_lane_sel          b[0:1]
            //reg_en_dfe_state      b[2]
            //reg_en_agc_state      b[3]
            //reg_auto_scan_thrd    b[4:7]
            msWriteByte(REG_DPRX_PHY0_35_L + usRegOffsetPHY0ByID, 0x3F); // default value
            //msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x0000);// Initial script by OG
            Current_rate[dprx_phy_id] = 0x14;//14h: 5.4Gbps per lane
        }
        else if(link_rate==0x06) // link_rate from reg_dpcd_00100h 06h: 1.62Gbps per lane
        {
            msWriteByteMask(REG_DPRX_PHY0_23_L + usRegOffsetPHY0ByID, DP_HBRRBR_EQ_MAX, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
#ifdef HALF_RATE_FLAG
            msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x9999);// Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_lane_l0/1/2/3
            msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, 0, BIT3 );  // by Ryan
#endif
            //reg_test_dfe_msb_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_12_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_13_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            //reg_dlpf_kp_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_21_L + usRegOffsetPHY3ByID, 0x4444); // Initial script by OG
            //reg_test_eq_cs_l0/1/2/3
            msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x02); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_60_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_61_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_62_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_63_L + usRegOffsetPHY3ByID, 0x80); // Initial script by OG

            msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            // Bank 1621 -> PHY1
            msWrite2Byte(REG_DPRX_PHY1_6B_L + usRegOffsetPHY1ByID, 0xAAAA);
            //reg_test_eq_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x0800); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x6001); // Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_ch_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x0101); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x0101); // Initial script by OG
            //reg_test_active_sw_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_40_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_41_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_42_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_43_L + usRegOffsetPHY3ByID, 0x8038); // Initial script by OG
            // Bank 1621 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            // Bank 1623 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_22_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            //reg_gc_eqrs_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_32_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_33_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_34_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_35_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x0310); // Initial script by OG
            ////HBR/RBR bypass dfe and agc autoscan2
            Current_rate[dprx_phy_id] = 0x06;//06h: 1.62Gbps per lane
        }
    }
    else if(Current_rate[dprx_phy_id]==0x06) //current rate 06h:1.62 Gbps per lane
    {
        if(link_rate==0x14) // link_rate from reg_dpcd_00100h 14h: 5.4Gbps per lane
        {
            msWriteByteMask(REG_DPRX_PHY0_23_L + usRegOffsetPHY0ByID, DP_HBR23_EQ_MAX, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
#ifdef HALF_RATE_FLAG
            msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x8888);// Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_lane_l0/1/2/3
            msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
            msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, 0, BIT3 );  // by Ryan
#endif
            //reg_test_dfe_msb_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_12_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_13_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
            //reg_dlpf_kp_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_21_L + usRegOffsetPHY3ByID, 0x6666); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x01); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_6B_L + usRegOffsetPHY1ByID, 0x8888);//default: 0x82CC Only HBR2 Setting,Initial script by OG
            msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
            //reg_test_eq_cs_l0/1/2/3
            msWriteByte(REG_DPRX_PHY3_60_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_61_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_62_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_63_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            // Bank 1621 -> PHY1
            //reg_test_eq_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_ch_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
            //msWriteByteMask(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
            //msWriteByteMask(REG_DPRX_PHY3_14_H + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
            //msWriteByteMask(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
            //msWriteByteMask(REG_DPRX_PHY3_15_H + usRegOffsetPHY3ByID, BIT2|BIT3|BIT4|BIT5 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava

            //reg_test_active_sw_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_40_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_41_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_42_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_43_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            // Bank 1621 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
            // Bank 1623 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_22_L + usRegOffsetPHY3ByID, 0xEEEE); // Initial script by OG
            //reg_gc_eqrs_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_32_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_33_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_34_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_35_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x1110); // Initial script by OG
            ////pd_vdac_ov hbr/rbr:1
            msWriteByte(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
            //reg_lane_sel          b[0:1]
            //reg_en_dfe_state      b[2]
            //reg_en_agc_state      b[3]
            //reg_auto_scan_thrd    b[4:7]
            msWriteByte(REG_DPRX_PHY0_35_L + usRegOffsetPHY0ByID, 0x3F); // default value
            //msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x0000);// Initial script by OG
            Current_rate[dprx_phy_id] = 0x14;//14h: 5.4Gbps per lane
        }
        else if(link_rate==0x0A) // link_rate from reg_dpcd_00100h 10h: 2.7Gbps per lane
        {
            msWriteByteMask(REG_DPRX_PHY0_23_L + usRegOffsetPHY0ByID, DP_HBRRBR_EQ_MAX, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
#ifdef HALF_RATE_FLAG
            // if(dprx_phy_id == 0 )
            // {
            msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x8888 ); // Initial script by ryan
            //reg_test_lane_l0/1/2/3
            msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0xC0); // Initial script by Ryan
            msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, BIT3, BIT3 );  // by Ryan
            msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x00); // Initial script by Ryan

            if(bAutoTest == FALSE)
            {
                msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x07); // Initial script by Ryan
            }
            // }
            /* else if(dprx_phy_id == 1 )
             {
                 msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x9999);// Initial script by Ryan
                 // Bank 1623 -> PHY3
                 //reg_test_lane_l0/1/2/3
                 msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0x60); // Initial script by Ryan
                 msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, 0, BIT3 );  // by Ryan
                 msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x02); // Initial script by Ryan
                 msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x00); // Initial script by Ryan
             }*/
#endif
            //reg_test_dfe_msb_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_12_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_13_L + usRegOffsetPHY3ByID, 0x2020); // Initial script by OG
            //reg_dlpf_kp_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_21_L + usRegOffsetPHY3ByID, 0x4444); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_60_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_61_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_62_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            msWriteByte(REG_DPRX_PHY3_63_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
            // Bank 1621 -> PHY1
            msWrite2Byte(REG_DPRX_PHY1_6B_L + usRegOffsetPHY1ByID, 0xAAAA);
            //reg_test_eq_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x281E); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
            // Bank 1623 -> PHY3
            //reg_test_ch_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
            //reg_test_active_sw_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_40_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_41_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_42_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_43_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
            // Bank 1621 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
            msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
            // Bank 1623 -> PHY1
            //reg_test_pga_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_22_L + usRegOffsetPHY3ByID, 0x7777); // Initial script by OG
            //reg_gc_eqrs_l0/1/2/3
            msWrite2Byte(REG_DPRX_PHY3_32_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_33_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_34_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_35_L + usRegOffsetPHY3ByID, 0x2300); // Initial script by OG
            msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x0110); // Initial script by OG
            //msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by ryan
            Current_rate[dprx_phy_id] = 0x0A;//10h: 2.7Gbps per lane
        }
    }
    if (dprx_phy_id == 1)
    {
        mhal_DPRx_SetEQ_Current( dprx_phy_id, record_current_eq);
    }
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_Switch_Training_Setting_FT()
//  [Description]
//					mhal_Switch_Training_Setting_FT
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_Switch_Training_Setting_FT(DPRx_PHY_ID dprx_phy_id, BOOL bAutoTest)
{
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_phy_id);

    // For RBR/HBR JTOL PRBS7 setting
    // HBR3 / HBR2.5 / HBR2 for fast training
    if(((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == BIT6) ||
            ((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == (BIT4|BIT5)) ||
            ((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == BIT5))
    {
        mhal_DPRx_Switch_Training_Setting(dprx_phy_id, 0x14, bAutoTest); // DPCD100 HBR2
    }
    // HBR / RBR for fast training
    else if((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == BIT4)
    {
        mhal_DPRx_Switch_Training_Setting(dprx_phy_id, 0x0A, bAutoTest); // DPCD100 HBR
    }// RBR for fast training
    else if((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == 0)
    {
        mhal_DPRx_Switch_Training_Setting(dprx_phy_id, 0x06, bAutoTest); // DPCD100 RBR
    }
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_Select_ACDR_Lane()
//  [Description]
//					mhal_DPRx_Select_ACDR_Lane
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_Select_ACDR_Lane(DPRx_PHY_ID dprx_phy_id, BYTE lane_no)
{
    //PHY3 OFFSET
    WORD usRegOffsetPHY3ByID = DP_REG_OFFSET400(dprx_phy_id);
    //lane_no: <0 lane0> , <1 lane1> , <2 lane2> , <3 lane3>
    msWriteByteMask(REG_DPRX_PHY3_44_L+usRegOffsetPHY3ByID, lane_no, BIT0|BIT1);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MuxSelect()
//  [Description]
//					mhal_DPRx_MuxSelect
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MuxSelect(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id)
{
    //HDMI:0 DP:1
    msWriteByteMask(REG_DPRX_PHY_PM_65_L, BIT0, BIT0);

    if(dprx_id == DPRx_ID_3)
    {
        //DPC MUX Sel
        msWriteByteMask(REG_DPRX_PHY_PM_65_L, BIT1, BIT1);
    }
    else
    {
        msWriteByteMask(REG_DPRX_PHY_PM_65_L, 0, BIT1);
    }
    //Port 0:00 Port1:01 Port2:10
    msWriteByteMask(REG_DPRX_PHY_PM_63_L, dprx_aux_id, BIT0|BIT1); //SETTING3.07 , Aux select to Phy/MAC

	if(ubTemp2 > 1) //SETTING3.07, combo port select any Aux
	{
		msWriteByteMask(REG_DPRX_PHY_PM_64_L, dprx_id, BIT1|BIT0); //Port select to MAC,  0~2: Combo0~2, 3: Type C

                if(dprx_aux_id == DPRx_AUX_ID_0)
                {
                     msWriteByteMask(REG_DPRX_PHY_PM_62_L, dprx_id, BIT0|BIT1);
                }
                else if(dprx_aux_id == DPRx_AUX_ID_1)
                {
                     msWriteByteMask(REG_DPRX_PHY_PM_62_L, (dprx_id << 2), BIT2|BIT3);
                }
                else if(dprx_aux_id == DPRx_AUX_ID_2)
                {
                     msWriteByteMask(REG_DPRX_PHY_PM_62_L, (dprx_id << 4), BIT4|BIT5);
                }
	}


	if(ubTemp2 > 1) //SETTING3.08 auto scan2 SHOULD BYPASS DLPF , enable in HDMI
	{
        msWriteByteMask(REG_DPRX_PHY3_37_H  , BIT0 , BIT0);  // Disable DLPF
    }

	//HDMI:0 DP:1
    msWriteByteMask(REG_DPRX_HDMI_30_L, BIT0, BIT0);
    //HDMI:0 DP:1
    msWriteByteMask(REG_DPRX_HDMI_30_L, BIT4, BIT4);
    //HDMI change 0x1F
    msWriteByte(REG_DPRX_TRANS_CTRL_1D_H, 0x0F);

    // fro HDMi 750M issue
    msWrite2ByteMask(REG_DPRX_PHY1_1B_L,0x0000, BMASK(11:8));
    msWriteByte(REG_DPRX_PHY3_52_L, 0x03);  // Initial script by OG

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PHYInitialSetting()
//  [Description]
//					mhal_DPRx_PHYInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PHYInitialSetting(DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id)
{
    //dprx_phy_id = 1 ;//fix typeC
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffsetPHY1ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffsetPHY2ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffsetPHY3ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffset40ByID = DP_REG_OFFSET040(dprx_id);

    msEread_Init(&usTemp1, &ubTemp2);

    // Chip top
    //msWriteByte(REG_CHIP_TOP_68_L, BIT4|BIT6); // [4] reg_mpll_prdt_en // [6] reg_mpll_xtal_en
    //msWriteByte(REG_CHIP_TOP_68_H, BIT5); // [12:10] reg_mpll_output_div
    //msWriteByteMask(REG_CHIP_TOP_69_L, BIT2, BIT2); // [3:2] reg_mpll_output_div_first

    msWrite2Byte(REG_DPRX_TOP_GP_00_L, 0xFFFF); // DP clkgen enable
    msWrite2Byte(REG_DPRX_TOP_GP_01_L, 0xFFFF); // DP clkgen enable
    msWrite2Byte(REG_DPRX_TOP_GP_02_L, 0xFFFF); // DP clkgen enable
    msWrite2Byte(REG_DPRX_TOP_GP_03_L, 0xFFFF); // DP clkgen enable

    msWriteByteMask(REG_DPRX_PHY0_00_H + usRegOffsetPHY0ByID, 0x00, 0x0F); // For PHY CTS

    msWrite2Byte(REG_DPRX_TOP_GP_0C_L, 0xFFFF); // DP clkgen enable
    msWrite2Byte(REG_DPRX_TOP_GP_0D_L, 0xFFFF); // DP clkgen enable
    msWrite2Byte(REG_DPRX_TOP_GP_0E_L, 0xFFFF); // DP clkgen enable
    msWrite2Byte(REG_DPRX_TOP_GP_0F_L, 0xFFFF); // DP clkgen enable

    //msWriteByteMask(REG_DPRX_TOP_GP_05_L, 0, BMASK(7:0)); // Diable clock enable // Peter request
    //msWriteByteMask(REG_DPRX_TOP_GP_05_H, 0, BMASK(3:0)); // Diable clock enable // Peter request

    // Bank 1628 -> DP_HDMI_PHY
    msWriteByte(REG_DPRX_HDMI_22_L, 0x03); //reg_div_m_n_lane_p_phy[0:7] Initial script by OG
    // Bank 1621 -> PHY1
    //reg_divsel_pll_l0/1/2/3_ov_en b[2],b[6],b[10],b[14]
    msWrite2Byte(REG_DPRX_PHY1_01_L + usRegOffsetPHY1ByID, 0x4444);// Initial script by OG
    //feedback divider of PLL of lane 0/1/2/3 override value 0:b[0:3] 1:b[4:7] 2:b[8:11] 3:b[12:15]
    msWrite2Byte(REG_DPRX_PHY1_02_L + usRegOffsetPHY1ByID, 0x4444);// Initial script by OG
    //post divider of lane 0/1/2/3 override value  b[0:2],b[4:5],b[8:9],b[12:13]
    //post divider of lane 0/1/2/3 override enable b[3],b[7],b[11],b[15]
    msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x8888);// Initial script by OG
    //reg_dfe_mode_l0/1/2/3_ov_en b[3],b[7],b[11],b[15]
    msWrite2Byte(REG_DPRX_PHY1_6B_L + usRegOffsetPHY1ByID, 0x8888);//default: 0x82CC Only HBR2 Setting,Initial script by OG
    // Bank 1623 -> PHY3
    //reg_sel_link_clk_lane b[0:1] O-> LANE0
    //reg_dp_fifo_10to40_data_order b[4:5] future to ask
    msWriteByte(REG_DPRX_PHY3_44_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_eq_bw_ov b[8:10]
    //reg_eq_bw_ov_en b[11]
    //reg_pd_ctrl_ov b[14]
    msWriteByte(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    //reg_en_idac_tap b[0:3] reg_pd_bg b[6] reg_pd_ldo b[7]
    msWriteByte(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_en_phd_cal_bypass_l0/1/2/3 b[0] b[1] b[2] b[3]
    //reg_gc_phdac_slewrate_ov_en b[4]
    //reg_gc_phdac_slewrate_ov b[8:9]
    //reg_pd_updn_dmx b[12] : 1
    msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x1000); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_pd_phdac_ov_en b[4]
    msWriteByte(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x10); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_pd_phdac_q_ov_en b[4]
    msWriteByte(REG_DPRX_PHY3_1B_L + usRegOffsetPHY3ByID, 0x10); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_sel_clkin_l0_ov 1:MPLLS
    //reg_sel_clkin_l0_ov_en b[3] 1:enable
    //msWriteByte(REG_DPRX_PHY1_13_L + usRegOffsetPHY1ByID, 0x09); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_gc_en_dp_mode_ov_en b[0]
    //reg_gc_en_dp_mode_ov b[1]
    msWriteByte(REG_DPRX_PHY3_52_L + usRegOffsetPHY3ByID, 0x03); // Initial script by OG
    //reg_en_acdr_mode b[0]
    msWriteByte(REG_DPRX_PHY3_03_L + usRegOffsetPHY3ByID, 0x01); // Initial script by OG
    //reg_test_ch_l0/1/2/3 b[0:7] b[8:15]
    msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
    //reg_test_ch_l2/3 b[0:7] b[8:15]
    msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_test_dfe_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY1_20_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_21_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_22_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_23_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_24_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_25_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_26_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_27_L + usRegOffsetPHY1ByID, 0x0000); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_test_lane_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_16_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_17_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_18_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_19_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    //reg_test_dlpf_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_0A_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_0B_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_0C_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_0D_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_0E_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_0F_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_10_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_11_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_eq_bw_hbr2p5/hbr2/hbr/rbr b[0:2] b[4:6] b[8:10] b[12:14]
    //debug by temp
    msWrite2Byte(REG_DPRX_PHY1_63_L + usRegOffsetPHY1ByID, 0x6430); // Initial script by OG
    // reg_dfe_bw_hbr2p5/hbr2/hbr/rbr b[0:1] b[2:3] b[4:5] b[6:7]
    //reg_divsel_scrpll_hbr2p5/hbr2/hbr/rbr b[8:9] b[10:11] b[12:13] b[14:15]
    msWrite2Byte(REG_DPRX_PHY1_64_L + usRegOffsetPHY1ByID, 0x0600); // Initial script by OG

    // Bank 1623 -> PHY3
    //reg_pd_cal_saff_l0/1/2/3 b[8] b[9] b[10] b[11]
    msWriteByte(REG_DPRX_PHY3_01_H + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_test_pll_l0 b[0:31]
    msWriteByte(REG_DPRX_PHY1_38_H + usRegOffsetPHY1ByID, 0x04); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_ictrl_pfd b[4:8]
    msWrite2Byte(REG_DPRX_PHY3_06_L + usRegOffsetPHY3ByID, 0x0020); // Initial script by OG
    //reg_Rctrl_pll b[4:8]
    msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    //AABA
    msWriteByteMask(REG_DPRX_PHY0_22_L + usRegOffsetPHY0ByID, BIT4|BIT1, BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    msWriteByteMask(REG_DPRX_PHY0_49_H + usRegOffsetPHY0ByID, BIT7, BIT7);
    //msWriteByteMask(REG_DPRX_PHY0_4E_H + usRegOffsetPHY3ByID, BIT8 , BIT8);
    //msWriteByteMask(REG_DPRX_PHY0_28_L + usRegOffsetPHY3ByID, BIT7 , BIT7);

    msWrite2Byte(REG_DPRX_PHY0_2B_L + usRegOffsetPHY0ByID, 0x0007);
    msWrite2Byte(REG_DPRX_PHY0_2C_L + usRegOffsetPHY0ByID, 0x0002);
    //reg_ictrl_pd_ov_en b[0:3]
    msWriteByte(REG_DPRX_PHY1_51_L + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_ictrl_pd_ov b[0:4]
    msWriteByte(REG_DPRX_PHY3_50_L + usRegOffsetPHY3ByID, 0x01); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_gc_rctrl_pll_l0/1 b[0:2] b[4:6]
    msWriteByte(REG_DPRX_PHY1_6E_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    //reg_pga_bw_hbr2p5/hbr2/HBR/RBR b[0:2] b[4:6] b[8:10] b[12:14]
    msWrite2Byte(REG_DPRX_PHY1_74_L + usRegOffsetPHY1ByID, 0x6430); // Initial script by OG
    //reg_eq_bw_hbr2p5/hbr2/hbr/rbr  b[0:2] b[4:6] b[8:10] b[12:14]
    //debug by temp
    msWrite2Byte(REG_DPRX_PHY1_63_L + usRegOffsetPHY1ByID, 0x6430); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_test_lane_l0/1/2/3
    msWriteByte(REG_DPRX_PHY3_16_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
    msWriteByte(REG_DPRX_PHY3_17_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
    msWriteByte(REG_DPRX_PHY3_18_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
    msWriteByte(REG_DPRX_PHY3_19_H + usRegOffsetPHY3ByID, 0x60); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_test_eq_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x4016); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x0001); // Initial script by OG
    // Bank 1623 -> PHY3
    //reg_test_ch_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0x8080); // Initial script by OG
    // msWriteByteMask(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID, 0 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
    // msWriteByteMask(REG_DPRX_PHY3_14_H + usRegOffsetPHY3ByID, 0 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
    // msWriteByteMask(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID, 0 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
    // msWriteByteMask(REG_DPRX_PHY3_15_H + usRegOffsetPHY3ByID, 0 , BIT2|BIT3|BIT4|BIT5|BIT6); // by Ava
    //reg_test_active_sw_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_40_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_41_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_42_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_43_L + usRegOffsetPHY3ByID, 0x8A39); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_test_pga_l0/1/2/3
    msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x11); // Initial script by OG
    // Bank 1623 -> PHY1
    //reg_test_pga_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_22_L + usRegOffsetPHY3ByID, 0xEEEE); // Initial script by OG
    //reg_gc_eqrs_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_32_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_33_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_34_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_35_L + usRegOffsetPHY3ByID, 0x2980); // Initial script by OG
    //reg_test_dfe_msb_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_12_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY3_13_L + usRegOffsetPHY3ByID, 0x0000); // Initial script by OG
    //reg_en_phd_cal_bypass_l0/1/2/3 b[0] b[1] b[2] b[3]
    //reg_gc_phdac_slewrate_ov_en b[4]
    //reg_gc_phdac_slewrate_ov b[8:9]
    //reg_pd_updn_dmx b[12]
    msWrite2Byte(REG_DPRX_PHY3_30_L + usRegOffsetPHY3ByID, 0x1110); // Initial script by OG
    //reg_dlpf_kp_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_21_L + usRegOffsetPHY3ByID, 0x6666); // Initial script by OG
    //reg_dlpf_ki_l0/1/2/3
    msWrite2Byte(REG_DPRX_PHY3_20_L + usRegOffsetPHY3ByID, 0x6666); // Initial script by OG
    if(Input_Displayport_C1 != Input_Nothing)
	{
        // Bank 0020 -> PM
        //reg_dp_hdmi_pm_port_sel b[0] DP:1
        //msWriteByte(REG_DPRX_PHY_PM_65_L, 0x03);  // Initial script by OG
        //reg_dp_pm_port_sel b[0:1] Port:00 Port:01 Port:10
        //msWriteByte(REG_DPRX_PHY_PM_63_L, 0x01);  // Initial script by OG
        //reg_nodie_sq_vth_0  b[0:2]//sqh vth=110 vth=250mV vdpp ,sqh vth=101 vth=200mV vdpp
        //reg_nodie_bypass_sq_ac_0 b[3]
        //reg_nodie_en_sq_0 b[4:7]
        msWriteByte(REG_DPRX_PHY_PM_00_L, 0xF5);   // Initial script by OG

        // The order of R-term setting need to enable R-term first, then power up R-term for each lane
        msWriteByteMask(REG_DPRX_PHY_PM_02_L, BIT4, BIT4); // reg_nodie_en_dp_rt
        msWriteByte(REG_DPRX_PHY_PM_00_H, 0); // reg_nodie_pd_rt_0[8:11]
    }
    if(Input_Displayport_C2 != Input_Nothing)
    {
        //reg_nodie_sq_vth_1        b[0:2]//sqh vth=110 vth=250mV vdpp ,sqh vth=101 vth=200mV vdpp
        //reg_nodie_bypass_sq_ac_1  b[3]
        //reg_nodie_en_sq_1         b[4:7]
        msWriteByte(REG_DPRX_PHY_PM_20_L, 0xF5);  // Initial script by OG

        // The order of R-term setting need to enable R-term first, then power up R-term for each lane
        msWriteByteMask(REG_DPRX_PHY_PM_02_L, BIT5, BIT5); // reg_nodie_en_dp_rt
        msWriteByte(REG_DPRX_PHY_PM_20_H, 0); // reg_nodie_pd_rt_1[8:11]
    }
    if(Input_Displayport_C3 != Input_Nothing)
    {
        //reg_nodie_sq_vth_2        b[0:2]//sqh vth=110 vth=250mV vdpp ,sqh vth=101 vth=200mV vdpp
        //reg_nodie_bypass_sq_ac_2  b[3]
        //reg_nodie_en_sq_2         b[4:7]
        msWriteByte(REG_DPRX_PHY_PM_40_L, 0xF5);  // Initial script by OG

        // The order of R-term setting need to enable R-term first, then power up R-term for each lane
        msWriteByteMask(REG_DPRX_PHY_PM_02_L, BIT6, BIT6); // reg_nodie_en_dp_rt
        msWriteByte(REG_DPRX_PHY_PM_40_H, 0); // reg_nodie_pd_rt_2[8:11]
    }

    //reg_nodie_en_sqh_ov_0     b[8]
    //reg_lane_swap0_0          b[10:11]
    //reg_lane_swap1_0          b[12:13]
    //reg_lane_swap2_0          b[14:15]
    msWriteByte(REG_DPRX_PHY_PM_01_H, 0x90);  // Initial script by OG
    //reg_ps_sw_rst             b[8]
    //reg_ps_enable             b[9]
    //reg_sqh_ps_sel_0/1/2      b[10] b[11] b[12]
    //reg_rt_ps_sel_0/1/2       b[13] b[14] b[15]
    msWriteByte(REG_DPRX_PHY_PM_6B_H, 0x02);  // Initial script by OG

    msWriteByte(REG_DPRX_PHY_PM_66_L, 0x00);  // Initial script by OG

    msWriteByte(REG_DPRX_PHY_PM_67_L, 0xff);  // Initial script by OG
    // Bank 1603
    //reg_fast_train_lock_too_fast_to_sw_lock        b[8]
    //reg_fast_train_reset_when_rw_dpcd_100h_101h    b[9]
    //reg_only_consider_dpcd_diff_103h_456h          b[10]
    //reg_reset_auto_eq_only_tps234                  b[11]
    msWriteByte(REG_DPRX_TRANS_CTRL_1D_H, 0x0F); //Initial script by OG
    //reg_clk_icg_ls0/1/2/3_en b[0] b[1] b[2] b[3]
    msWriteByte(REG_DPRX_TRANS_CTRL_01_L, 0x0F); //Initial script by OG

    // Bank 1620 -> PHY0
    //reg_tap1/2/3/4_stable_thrd
    msWriteByte(REG_DPRX_PHY0_34_L + usRegOffsetPHY0ByID, 0x22); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_34_H + usRegOffsetPHY0ByID, 0x22); // Initial script by OG
    //Edward debug only , for clock inverse
    //msWriteByte(REG_DPRX_PHY0_57_H + usRegOffsetPHY0ByID, 0xF); // Initial script by OG
    //msWriteByteMask(REG_DPRX_PHY1_57_H + usRegOffsetPHY0ByID, BIT4|BIT5|BIT6|BIT7,BIT4|BIT5|BIT6|BIT7);
    //h64	h64	1	0	reg_hdmi_pm_port_sel	1
    //msWriteByteMask(REG_DPRX_DPCD1_28_L , BIT5 , BIT5);
    // Bank 1621 -> PHY1
    //reg_cr_lock_ov      b[0:3]
    //reg_cr_lock_ov_en   b[6]
    //reg_dfe_bw_ov       b[4:5]
    //reg_dfe_bw_ov_en    b[7]
    //reg_eq_bw_ov        b[8]
    //reg_eq_bw_ov_en     b[10:11]
    msWriteByte(REG_DPRX_PHY1_00_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, 0x40); // Initial script by OG
    //reg_en_idac_tap     b[0:3] 1111
    //reg_pd_bg           b[6]
    //reg_pd_ldo          b[7]
    //reg_pd_cml_dff      b[8:11]
    //reg_pd_dfe          b[12:15]
    msWriteByte(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0x0F); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    //reg_pd_eq           b[0:3]
    //reg_pd_pga          b[4:7]
    //reg_pd_pll          b[8:11]
    //reg_pd_vdac         b[12:15]
    msWriteByte(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    //reg_pd_phdac_ov       b[0:3]
    //reg_pd_phdac_ov_en    b[4] 1
    //reg_pd_vco_vpump      b[5]
    //reg_disable_vco_vpump b[6] 1
    //reg_en_rbr_updn_swap  b[7]
    //reg_en_rbr_ov         b[8]
    //reg_en_rbr_ov_en      b[9]
    //reg_en_hbr_ov         b[10]
    //reg_en_hbr_ov_en      b[11]
    msWriteByte(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x50); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_1B_H + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    // Bank 1620 -> PHY0
    //reg_clkgen_en         b[0:29] all bit on
    msWrite2Byte(REG_DPRX_PHY0_7B_L + usRegOffsetPHY0ByID, 0xFFFF); // Initial script by OG
    msWrite2Byte(REG_DPRX_PHY0_7C_L + usRegOffsetPHY0ByID, 0x3FFF); // Initial script by OG
    //reg_clkgen_inv        b[0:15]
    msWrite2Byte(REG_DPRX_PHY0_7D_L + usRegOffsetPHY0ByID, 0x0000); // Initial script by OG
    //reg_sel_pll_clk_det
    //reg_10b_sel_inv
    //reg_clk_div10_sel
    //reg_link_rate_ov
    //reg_link_rate_ov_en
    //reg_auto_br_mode_sel
    msWrite2Byte(REG_DPRX_PHY0_7F_L + usRegOffsetPHY0ByID, 0x0000); // Initial script by OG
    //reg_freq_sel                b[0:2]
    //reg_freq_det_dis            b[3]
    //reg_freq_th                 b[4:7]
    //reg_lane0/1/2/3_af_ls_ove   b[8]  b[9]  b[10] b[11]
    //reg_lane0/1/2/3_af_ls_ov    b[12] b[13] b[14] b[15]
    msWrite2Byte(REG_DPRX_PHY0_4F_L + usRegOffsetPHY0ByID, 0x0023); // Initial script by OG
    // Bank 1621 -> PHY1
    //reg_en_la_ov                b[0:3]
    //reg_en_sa_oscal_ov          b[4:7]
    //reg_en_scan_ov              b[8:11]
    //reg_en_la_ov_en             b[12]
    //reg_en_sa_oscal_ov_en       b[13]
    //reg_en_scan_ov_en           b[14]
    //reg_clk_432_sel             b[15]
    msWrite2Byte(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0x100F); // Initial script by OG

    //msWriteByteMask(REG_DPRX_HDMI_30_L, BIT0, BIT0);
    //msWriteByteMask(REG_DPRX_HDMI_30_L, BIT4, BIT4);
    //msWriteByteMask(REG_DPRX_PHY_PM_63_L, 0, BIT0|BIT1);
    // Bank 1620 -> PHY0
    //=================================================================================
    //		UP TYPEC LDO Voltage % 00: 0.95V | 01: 1.0V | 10: 1.05V | 11: 1.1V
    //=================================================================================
    //if(dprx_phy_id==1)
    //    msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY3ByID, BIT0 , BIT0 ); // By Mansi
    //=================================================================================
    //		DP mode enable
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_0C_H + usRegOffsetPHY0ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_4C_L + usRegOffsetPHY0ByID, 0x80); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_4C_H + usRegOffsetPHY0ByID, 0x00); // Initial script by OG

    msWriteByte(REG_DPRX_PHY0_28_L + usRegOffsetPHY0ByID, 0xA0); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_28_H + usRegOffsetPHY0ByID, 0x3F); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_49_L + usRegOffsetPHY0ByID, 0x46); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_49_H + usRegOffsetPHY0ByID, 0xC0); // Initial script by OG

    //=================================================================================
    //		Auto EQ enable
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_25_L + usRegOffsetPHY0ByID, 0x5C); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_25_H + usRegOffsetPHY0ByID, 0x0A); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_4E_L + usRegOffsetPHY0ByID, 0x04); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_4E_H + usRegOffsetPHY0ByID, 0x01); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_5C_H + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    //=================================================================================
    //		bypass_cal_saff_sum
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_60_H + usRegOffsetPHY1ByID, 0xE8); // Initial script by OG
    //msWriteByteMask(REG_DPRX_PHY1_61_L + usRegOffsetPHY1ByID, BIT2 , BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); // Initial script by OG
    //=================================================================================
    //		Turn off reset vco
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_12_H + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
    //=================================================================================
    //		Fasting Training setting
    //=================================================================================
    //msWriteByte(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, 0x24);
    msWriteByte(REG_DPRX_PHY0_75_L + usRegOffsetPHY0ByID, 0x04);

    msWrite2Byte(REG_DPRX_PHY2_24_L + usRegOffsetPHY2ByID, 0x9999);
    msWriteByte(REG_DPRX_PHY2_25_L + usRegOffsetPHY2ByID, 0x19);
    //if(dprx_phy_id==0)
    //{
    msWrite2Byte(REG_DPRX_PHY2_1E_L + usRegOffsetPHY2ByID, 0x3333);
    msWriteByte(REG_DPRX_PHY2_1F_L + usRegOffsetPHY2ByID, 0x33);
    //}
    /* else if(dprx_phy_id==1)
     {
         msWrite2Byte(REG_DPRX_PHY2_1E_L + usRegOffsetPHY2ByID, 0x9999);
         msWriteByte(REG_DPRX_PHY2_1F_L + usRegOffsetPHY2ByID, 0x19);
     }*/
    msWrite2Byte(REG_DPRX_PHY2_18_L + usRegOffsetPHY2ByID, 0xAAAA);
    msWriteByte(REG_DPRX_PHY2_19_L + usRegOffsetPHY2ByID, 0x2A);
//wriu     0x162248             0x99
//wriu     0x162249             0x99
//wriu     0x16224a             0x19
//wriu     0x16223c             0x99
//wriu     0x16223d             0x99
//wriu     0x16223e             0x19
//wriu     0x162230             0xaa
//wriu     0x162231             0xaa
//wriu     0x162232             0x2a
    //=================================================================================
    //		ECO setting
    //=================================================================================
    msWrite2Byte(REG_DPRX_PHY0_4D_L + usRegOffsetPHY0ByID, 0x03FF); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_4E_L + usRegOffsetPHY0ByID, 0x06); // Initial script by OG

    //msWriteByteMask(REG_DPRX_PHY0_22_L + usRegOffsetPHY0ByID, BIT4|BIT3|BIT0 , BIT5|BIT4|BIT3|BIT2|BIT1|BIT0); // Initial script by OG

    msWriteByte(REG_DPRX_PHY0_32_L + usRegOffsetPHY0ByID, 0x60); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_5D_L + usRegOffsetPHY1ByID, 0xA0); // Initial script by OG
    ////hdmi_dp_sel
    msWriteByte(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, 0x00); // Initial script by OG
    //=================================================================================
    //		dlev_quality_err_cnt_thrd
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_0E_L + usRegOffsetPHY0ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_0E_H + usRegOffsetPHY0ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_0F_L + usRegOffsetPHY0ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_0F_H + usRegOffsetPHY0ByID, 0x00); // Initial script by OG
    //=================================================================================
    //		update_dlev_time_out_thrd
    ///================================================================================
    msWriteByte(REG_DPRX_PHY0_12_H + usRegOffsetPHY0ByID, 0x80); // Initial script by OG
    //=================================================================================
    //		update_dac_time_out_thrd
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_1A_H + usRegOffsetPHY0ByID, 0x82); // Initial script by OG

    ////=================================================================================
    ////eq_strength_max
    ////=================================================================================
    msWriteByte(REG_DPRX_PHY2_68_L + usRegOffsetPHY2ByID, 0x0F); // Initial script by OG
    msWriteByte(REG_DPRX_PHY2_68_H + usRegOffsetPHY2ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY2_69_L + usRegOffsetPHY2ByID, 0x07); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_23_L + usRegOffsetPHY0ByID, 0x22); // Initial script by OG
    //=================================================================================
    //		Auto EQ direction
    //=================================================================================
    msWriteByteMask(REG_DPRX_PHY0_24_H + usRegOffsetPHY0ByID, BIT0, BIT0 );  //by Ava
    //=================================================================================
    //		Auto EQ coarse tune/fine tune
    //=================================================================================
    msWriteByteMask(REG_DPRX_PHY0_36_L + usRegOffsetPHY0ByID, BIT0|BIT1, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); //by Ava
    msWriteByteMask(REG_DPRX_PHY0_37_L + usRegOffsetPHY0ByID, BIT0|BIT1, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); //by Ava
    //=================================================================================
    //		pga_gain_sel_hbr_rbr
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_24_L + usRegOffsetPHY0ByID, 0xC0); // Initial script by OG
    ////--------------------------------------
    ////eq_accu_threshold
    ////--------------------------------------
    msWriteByte(REG_DPRX_PHY0_28_L + usRegOffsetPHY0ByID, 0xA0); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_28_H + usRegOffsetPHY0ByID, 0x3A); // Initial script by OG
    //=================================================================================
    //		filter_depth_dlev
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_30_H + usRegOffsetPHY0ByID, 0x33); // Initial script by OG
    //=================================================================================
    //		Rterm efuse
    //=================================================================================
    if(dprx_phy_id==0)
    {
        msWriteByteMask(REG_DPRX_PHY_PM_03_H + usRegOffset40ByID, 0, BIT4|BIT5 );  // by ryan

        if(msEread_GetDataFromEfuse(0x140)&BIT(4))
        {
            msWriteByteMask(REG_DPRX_PHY_PM_03_L, (msEread_GetDataFromEfuse(0x13C)&0x0f), 0x0F );   // by ryan
        }

        if(msEread_GetDataFromEfuse(0x15E)&BIT(0))
        {
            msWriteByteMask(REG_DPRX_PHY_PM_23_L, (msEread_GetDataFromEfuse(0x15B)&0x0f), 0x0F );   // by ryan
        }
        if(msEread_GetDataFromEfuse(0x15E)&BIT(3))
        {
            msWriteByteMask(REG_DPRX_PHY_PM_43_L, (msEread_GetDataFromEfuse(0x15C)&0xf0)>>4, 0x0F );  // by ryan
        }
    }
    //=================================================================================
    //		dlev_target efuse
    //=================================================================================
    if(dprx_phy_id==0)
    {
        if(msEread_GetDataFromEfuse(0x185)&BIT(4))
        {
            msWriteByte(REG_DPRX_PHY0_3D_L, 0x3F);  //by Ryan
        }
        else
        {
            msWriteByte(REG_DPRX_PHY0_3D_L, 0x38);  //by Ryan
        }
    }
    else if(dprx_phy_id==1)
    {
        if(msEread_GetDataFromEfuse(0x185)&BIT(5))
        {
            msWriteByte(REG_DPRX_PHY0_3D_L + usRegOffsetPHY0ByID, 0x3F); //by Ryan
        }
        else
        {
            msWriteByte(REG_DPRX_PHY0_3D_L + usRegOffsetPHY0ByID, 0x38); //by Ryan
        }
    }
    //=================================================================================
    //		Phase quality enough
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_49_L + usRegOffsetPHY0ByID, 0x46); // Initial script by OG
    //=================================================================================
    //		autoeq_crlose_en_synth
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_62_L + usRegOffsetPHY0ByID, 0x80); // Initial script by OG
    //JL suggest turn off this bit when timer_a = 0 to avoid CR lose irq when unplug cable, only turn on this bit during normal training
    #if (DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL == 0x0)
    msWriteByteMask(REG_DPRX_PHY0_62_L + usRegOffsetPHY0ByID, 0, BIT7);
    #endif
    //=================================================================================
    //		pd_dlev_saff hbr/rbr:f
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    ////--------------------------------------
    ////en_idac_tap
    ////--------------------------------------
    msWriteByte(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0x0F); // Initial script by OG
    //=================================================================================
    //		pd_vdac hbr/rbr:f
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG
    //=================================================================================
    //		pd_vdac_ov hbr/rbr:1
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, 0x00); // Initial script by OG

    //=================================================================================
    //		autoscan_partial_mode_en
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_5C_H + usRegOffsetPHY1ByID, 0x00); // Initial script by OG

    //=================================================================================
    //		dfe_agc_freerun
    //=================================================================================
    //b[15] keep dfe value , not to be reseted
    msWriteByte(REG_DPRX_PHY1_5F_H + usRegOffsetPHY1ByID, 0xB0); // Initial script by OG
    //=================================================================================
    //		bypass_cal_saff_sum
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_60_H + usRegOffsetPHY1ByID, 0xE8); // Initial script by OG
    //=================================================================================
    //		dlev_good
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_65_L + usRegOffsetPHY1ByID, 0x30); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_65_H + usRegOffsetPHY1ByID, 0xB0); // Initial script by OG
    //=================================================================================
    //		dlev_enough
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_66_L + usRegOffsetPHY1ByID, 0x20); // Initial script by OG
    msWriteByte(REG_DPRX_PHY1_66_H + usRegOffsetPHY1ByID, 0xF0); // Initial script by OG
    //=================================================================================
    //		phase_2ui_en
    //=================================================================================
    msWriteByte(REG_DPRX_PHY1_7E_L + usRegOffsetPHY1ByID, 0xE3); // Initial script by OG

    msWriteByteMask(REG_DPRX_PHY1_7E_H + usRegOffsetPHY1ByID, BIT1, BIT1); // Patch by Leona2 setting ,eq big than/small than gray to bin

    msWriteByteMask(REG_DPRX_PHY2_64_L + usRegOffsetPHY0ByID, 0x0C, BMASK(5:0));
    msWriteByteMask(REG_DPRX_PHY2_64_H + usRegOffsetPHY0ByID, 0x06, BMASK(5:0));
    msWriteByteMask(REG_DPRX_PHY0_2E_L + usRegOffsetPHY0ByID, BIT0, BIT0);
    msWriteByteMask(REG_DPRX_PHY0_44_H + usRegOffsetPHY0ByID, 0, BIT6);
	msWrite2Byte(REG_DPRX_PHY0_29_L + usRegOffsetPHY0ByID, 0x0028); // 201806, by analog RD's requests

    //=================================================================================
    //		Set EQ lower bound
    //=================================================================================
    msWriteByte(REG_DPRX_TRANS_CTRL_32_L + usRegOffsetTransCTRLByID, 0x05); // Initial script by OG
    msWriteByte(REG_DPRX_TRANS_CTRL_12_L + usRegOffsetTransCTRLByID, 0x95); // Initial script by OG
    //=================================================================================
    //		HBR2/HBR3 bypass autoscan1
    //=================================================================================
    msWriteByte(REG_DPRX_PHY0_10_L + usRegOffsetPHY0ByID, 0x88); // Initial script by OG
    msWriteByte(REG_DPRX_PHY0_10_H + usRegOffsetPHY0ByID, 0x00); // Initial script by OG

    //===========
    //      No description in SCript
    //===========
    //reg_enable_dlev_bit8_check   b[0]: 1
    msWriteByte(REG_DPRX_PHY2_61_L + usRegOffsetPHY2ByID, 0x01); // Initial script by OG
    //reg_dummy_dtop_7f            b[0]: 1 , latch dlev and release after eye height over
    msWriteByte(REG_DPRX_PHY1_7F_L + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
    //reg_dummy_73                 b[2:7] For timer use, 1us
    msWriteByte(REG_DPRX_PHY1_73_L + usRegOffsetPHY1ByID, 0x30); // Initial script by OG
    //only two tap, no tap3/4
    msWriteByte(REG_DPRX_PHY0_30_L + usRegOffsetPHY0ByID, 0x37); // Initial script by OG
    //disable pn swap of scan
    msWriteByte(REG_DPRX_PHY1_5F_L + usRegOffsetPHY1ByID, 0x01); // Initial script by OG
    //(1: if dfe tap1 is negative ,tap1=0)
    //(0: if dfe tap1 is negative ,tap1=0 still negative)
    msWriteByte(REG_DPRX_PHY0_3C_L + usRegOffsetPHY0ByID, 0x01); // Initial script by OG
    // up 0.95V to 1.05V
    msWriteByteMask(REG_DPRX_PHY3_0A_L + usRegOffsetPHY3ByID, BIT0|BIT3, BIT0|BIT3 );  // by Ryan
    msWriteByteMask(REG_DPRX_PHY3_0C_L + usRegOffsetPHY3ByID, BIT0|BIT3, BIT0|BIT3 );  // by Ryan
    msWriteByteMask(REG_DPRX_PHY3_0E_L + usRegOffsetPHY3ByID, BIT0|BIT3, BIT0|BIT3 );  // by Ryan
    msWriteByteMask(REG_DPRX_PHY3_10_L + usRegOffsetPHY3ByID, BIT0|BIT3, BIT0|BIT3 );  // by Ryan

    //PGA negative C current
    msWriteByte(REG_DPRX_PHY1_31_H + usRegOffsetPHY1ByID, 0X01); //  by Ava
    msWriteByte(REG_DPRX_PHY1_33_H + usRegOffsetPHY1ByID, 0x01); //  by Ava
    msWriteByte(REG_DPRX_PHY1_35_H + usRegOffsetPHY1ByID, 0x01); //  by Ava
    msWriteByte(REG_DPRX_PHY1_37_H + usRegOffsetPHY1ByID, 0x01); //  by Ava

    msWriteByte(REG_DPRX_PHY3_60_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY3_61_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY3_62_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
    msWriteByte(REG_DPRX_PHY3_63_L + usRegOffsetPHY3ByID, 0x00); // Initial script by OG
//reverse setting for HDMI power saving mode .
    msWrite2Byte(REG_DPRX_PHY1_03_L + usRegOffsetPHY1ByID, 0x0000);// Initial script by OG
    //bypass for typeC
    if(dprx_phy_id==1)//SETTING1,SETTING2
    {
        msWriteByteMask(REG_DPRX_PHY0_10_L + usRegOffsetPHY3ByID, BIT7, BIT7); //by Yuhsiang
        msWriteByteMask(REG_DPRX_PHY0_10_H + usRegOffsetPHY3ByID, BIT0, BIT0); //by Yuhsiang
        msWrite2ByteMask(REG_DPRX_PHY0_4D_L + usRegOffsetPHY3ByID, BMASK(10:0), BMASK(10:0)); //by Yuhsiang
    }
    msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, BIT0, BIT0);// force Sqh mode

#if (SET_DPC_PORT_AS_DP_PORT == 0x1)//DP TO DPC
    if(ubTemp2==0)//Note. this is Version1
    {
        msWriteByte(REG_DPRX_PHY0_00_L, 0xE1);
        msWriteByteMask(REG_DPRX_PHY2_02_H, BIT7, BIT7);
        msWriteByteMask(REG_DPRX_PHY3_44_L, BIT0, BIT0|BIT1);
    }
    // only setting U31 in 19072
    msWriteByteMask(REG_152816, 0, BIT4);//Reg_u31_redrv_en
    msWriteByte(REG_152678, 0x88);//Reg_rx_sqh_deglitch [0]=0, to use fro_12m
    msWriteByte(REG_15267A, 0x88);//Reg_rx_sqh_deglitch [0]=0, to use fro_12m
#endif
    Current_rate[dprx_phy_id] = 0x14;
	if(ubTemp2 > 1) //SETTING3.08 auto scan2 SHOULD BYPASS DLPF
	{
        if(dprx_phy_id==1)//SETTING3.08 enable dpc auto scan
        {
            msWriteByteMask(REG_DPRX_PHY0_10_H + usRegOffsetPHY0ByID, 0, BIT0); // enable auto scan2--> 1:bypass 0:enable
            msWriteByteMask(REG_DPRX_PHY3_39_H + usRegOffsetPHY3ByID , BIT7 , BIT7); // Disable DLPF
        }
    }
    //=================================================================================
    //		0811 CDR lock CTS issue
    //=================================================================================
    msWriteByteMask(REG_DPRX_PHY0_60_H + usRegOffsetPHY0ByID, (0x1F << 1), BMASK(5:1)); // For DP 1.4 CTS 5.3.1.x items

    //=================================================================================
	//		CTS patch
	//=================================================================================
	msWrite2Byte(REG_DPRX_PHY0_2D_L + usRegOffsetPHY0ByID, 0x300); // For DP1.2 CTS 5.3.1.4 and 5.3.1.5 patch

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnablePHYInterrupt()
//  [Description]
//					mhal_DPRx_EnablePHYInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnablePHYInterrupt(DPRx_PHY_ID dprx_phy_id, BOOL bEnable)
{
    WORD usRegOffsetPHY2ByID = DP_REG_OFFSET400(dprx_phy_id);

#if (DPRX_AUTOEQ_Lane0DoneOnly == 0x1)
    //CR Done Interrupt registration
    msWriteByte(REG_DPRX_PHY2_43_H + usRegOffsetPHY2ByID, BIT7);
    msWriteByte(REG_DPRX_PHY2_43_H + usRegOffsetPHY2ByID, 0);
    msWriteByteMask(REG_DPRX_PHY2_41_H + usRegOffsetPHY2ByID, bEnable? 0 : BIT7, BIT7);
    //EQ Lane0 Done Interrupt registration
    msWriteByte(REG_DPRX_PHY2_47_H + usRegOffsetPHY2ByID, BIT7);
    msWriteByte(REG_DPRX_PHY2_47_H + usRegOffsetPHY2ByID, 0);
    msWriteByteMask(REG_DPRX_PHY2_45_H + usRegOffsetPHY2ByID, bEnable? 0 : BIT7, BIT7);
#else // Lane 0 / 1 / 2 /3 auto EQ done on
    msWriteByte(REG_DPRX_PHY2_47_H + usRegOffsetPHY2ByID, BIT7);
    msWriteByte(REG_DPRX_PHY2_47_H + usRegOffsetPHY2ByID, 0);
    msWriteByteMask(REG_DPRX_PHY2_45_H + usRegOffsetPHY2ByID, bEnable? 0 : BIT7, BIT7);

    msWriteByte(REG_DPRX_PHY2_4B_H + usRegOffsetPHY2ByID, BIT7);
    msWriteByte(REG_DPRX_PHY2_4B_H + usRegOffsetPHY2ByID, 0);
    msWriteByteMask(REG_DPRX_PHY2_49_H + usRegOffsetPHY2ByID, bEnable? 0 : BIT7, BIT7);

    msWriteByte(REG_DPRX_PHY2_4F_H + usRegOffsetPHY2ByID, BIT7);
    msWriteByte(REG_DPRX_PHY2_4F_H + usRegOffsetPHY2ByID, 0);
    msWriteByteMask(REG_DPRX_PHY2_4D_H + usRegOffsetPHY2ByID, bEnable? 0 : BIT7, BIT7);

    msWriteByte(REG_DPRX_PHY2_53_H + usRegOffsetPHY2ByID, BIT7);
    msWriteByte(REG_DPRX_PHY2_53_H + usRegOffsetPHY2ByID, 0);
    msWriteByteMask(REG_DPRX_PHY2_51_H + usRegOffsetPHY2ByID, bEnable? 0 : BIT7, BIT7);
#endif

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PHYCDRDetectEnable()
//  [Description]
//					mhal_DPRx_PHYCDRDetectEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PHYCDRDetectEnable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);

    if(bEnable)
    {
        msWriteByteMask(REG_DPRX_PHY0_64_L + usRegOffsetPHY0ByID, BIT4|BIT0, BIT4|BIT0);
        msWriteByteMask(REG_DPRX_PHY0_64_H + usRegOffsetPHY0ByID, BIT4|BIT0, BIT4|BIT0);
    }
    else
    {
        msWriteByteMask(REG_DPRX_PHY0_64_L + usRegOffsetPHY0ByID, 0, BIT4|BIT0);
        msWriteByteMask(REG_DPRX_PHY0_64_H + usRegOffsetPHY0ByID, 0, BIT4|BIT0);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PowerDownEQEnable()
//  [Description]
//					mhal_DPRx_PowerDownEQEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PowerDownEQEnable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable)
{
    WORD usRegOffsetPHY1ByID = DP_REG_OFFSET400(dprx_phy_id);

    if(bEnable)
    {
        msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0xF0, 0xF0);//OG check RTL code from 1A TO 19
    }
    else
    {
        msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0, 0xF0);//OG check RTL code from 1A TO 19
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetPHYLanePNSwapEnable()
//  [Description]
//					mhal_DPRx_SetPHYLanePNSwapEnable
//  [Arguments]:
//					ubLanePNSwapSelect: [0] for Lane0,
//                                      [1] for Lane1,
//                                      [2] for Lane2,
//                                      [3] for Lane3,
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetPHYLanePNSwapEnable(DPRx_PHY_ID dprx_phy_id,  BOOL bEnable, BYTE ubLanePNSwapSelect)
{

    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);
    if(bEnable == TRUE)
    {
        msWriteByte(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, ubLanePNSwapSelect);
    }
    else
    {
        //default:
        msWriteByteMask(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, 0, BIT0|BIT1|BIT2|BIT3 );
    }
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetPHYLaneSwapEnable()
//  [Description]
//					mhal_DPRx_SetPHYLaneSwapEnable
//  [Arguments]:
//					ubTargetLaneNumber: [1:0] for lane0
//										[3:2] for lane1
//										[5:4] for lane2
//										[7:6] for lane3
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetPHYLaneSwapEnable(DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id, BOOL bEnable, BYTE ubTargetLaneNumber)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffsetPHY2ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffset40ByID = DP_REG_OFFSET040(dprx_id);
    BYTE ucLane0Swap = 0xFF;

    if(bEnable == TRUE)
    {
        msWriteByte(REG_DPRX_PHY0_00_L + usRegOffsetPHY0ByID, ubTargetLaneNumber);
        msWriteByteMask(REG_DPRX_PHY2_02_H + usRegOffsetPHY2ByID, BIT7, BIT7);
    }
    else
    {
        //default:
        msWriteByte(REG_DPRX_PHY0_00_L + usRegOffsetPHY0ByID, 0xE4);
        msWriteByteMask(REG_DPRX_PHY2_02_H + usRegOffsetPHY2ByID, 0x0, BIT7);
    }

    ucLane0Swap = msReadByte(REG_DPRX_PHY0_00_L + usRegOffsetPHY0ByID) & (BIT1|BIT0);
    msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, ucLane0Swap << 2, BIT3|BIT2);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetPHYLanePNSwapEnable()
//  [Description]
//					mhal_DPRx_SetPHYLanePNSwapEnable
//  [Arguments]:
//					ubLanePNSwapSelect: [0] for Lane0,
//                                      [1] for Lane1,
//                                      [2] for Lane2,
//                                      [3] for Lane3,
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetPHYPRBS7PNSwapEnable(DPRx_PHY_ID dprx_phy_id, BYTE ubLanePNSwapSelect )
{

    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);

    msWriteByteMask(REG_DPRX_PHY0_5E_H + usRegOffsetPHY0ByID, ubLanePNSwapSelect << 4, BIT4|BIT5|BIT6|BIT7 );

    return;
}

void ______RX_PHY_AUTO_EQ________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AutoEQModeSetting()
//  [Description]
//					mhal_DPRx_AutoEQModeSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AutoEQModeSetting(DPRx_PHY_ID dprx_phy_id, BYTE EQ_mode)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);

    switch(EQ_mode)
    {
        case DPRx_EQ_UNDER_OVER_MODE: // Use ABAA both under/over pattern to chane EQ value
            msWriteByteMask(REG_DPRX_PHY0_4E_H + usRegOffsetPHY0ByID, 0, BIT0);
            msWriteByteMask(REG_DPRX_PHY0_49_H + usRegOffsetPHY0ByID, 0, BIT7);
            break;

        case DPRx_EQ_UNDER_ONLY_MODE: // Use ABAA only one pattern to change EQ value
            msWriteByteMask(REG_DPRX_PHY0_4E_H + usRegOffsetPHY0ByID, BIT0, BIT0);
            msWriteByteMask(REG_DPRX_PHY0_49_H + usRegOffsetPHY0ByID, BIT7, BIT7);
            break;

        default : // Use under-only
            msWriteByteMask(REG_DPRX_PHY0_4E_H + usRegOffsetPHY0ByID, BIT0, BIT0);
            msWriteByteMask(REG_DPRX_PHY0_49_H + usRegOffsetPHY0ByID, BIT7, BIT7);
            break;
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AutoEQStartSetting()
//  [Description]
//					mhal_DPRx_AutoEQStartSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AutoEQStartSetting(DPRx_PHY_ID dprx_phy_id, BYTE EQ_Start)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);

    msWriteByteMask(REG_DPRX_PHY0_22_L + usRegOffsetPHY0ByID, EQ_Start, 0x1F); // EQ start value

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AutoEQRangeSetting()
//  [Description]
//					mhal_DPRx_AutoEQRangeSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AutoEQRangeSetting(DPRx_PHY_ID dprx_phy_id, BYTE EQ_Max, BYTE EQ_Min)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);

    msWriteByte(REG_DPRX_PHY0_36_L + usRegOffsetPHY0ByID, EQ_Min); // L0/L1 EQ range: minimum
    msWriteByte(REG_DPRX_PHY0_36_H + usRegOffsetPHY0ByID, EQ_Max); // L0/L1 EQ range: maximum
    msWriteByte(REG_DPRX_PHY0_37_L + usRegOffsetPHY0ByID, EQ_Min); // L2/L3 EQ range: minimum
    msWriteByte(REG_DPRX_PHY0_37_H + usRegOffsetPHY0ByID, EQ_Max); // L2/L3 EQ range: maximum

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AutoEQABAASetting()
//  [Description]
//					mhal_DPRx_AutoEQABAASetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AutoEQABAASetting(DPRx_PHY_ID dprx_phy_id, BYTE ABAA_mode)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);

    switch(ABAA_mode)
    {
        case DPRx_EQ_ABAA_DENOMIATOR_10:
            msWrite2Byte(REG_DPRX_PHY0_4C_L + usRegOffsetPHY0ByID, 0x10);
            msWriteByteMask(REG_DPRX_PHY0_28_L + usRegOffsetPHY0ByID, 0x04, 0x7F);
            break;

        case DPRx_EQ_ABAA_DENOMIATOR_20:
            msWrite2Byte(REG_DPRX_PHY0_4C_L + usRegOffsetPHY0ByID, 0x20);
            msWriteByteMask(REG_DPRX_PHY0_28_L + usRegOffsetPHY0ByID, 0x08, 0x7F);
            break;

        case DPRx_EQ_ABAA_DENOMIATOR_40:
            msWrite2Byte(REG_DPRX_PHY0_4C_L + usRegOffsetPHY0ByID, 0x40);
            msWriteByteMask(REG_DPRX_PHY0_28_L + usRegOffsetPHY0ByID, 0x10, 0x7F);
            break;

        case DPRx_EQ_ABAA_DENOMIATOR_80:
            msWrite2Byte(REG_DPRX_PHY0_4C_L + usRegOffsetPHY0ByID, 0x80);
            msWriteByteMask(REG_DPRX_PHY0_28_L + usRegOffsetPHY0ByID, 0x20, 0x7F);
            break;

        default:
            msWrite2Byte(REG_DPRX_PHY0_4C_L + usRegOffsetPHY0ByID, 0x80);
            msWriteByteMask(REG_DPRX_PHY0_28_L + usRegOffsetPHY0ByID, 0x20, 0x7F);
            break;
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AutoEQictrlSetting()
//  [Description]
//					mhal_DPRx_AutoEQictrlSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AutoEQictrlSetting(DPRx_PHY_ID dprx_phy_id, BYTE ICTRL_mode)
{
    UNUSED(dprx_phy_id);
    UNUSED(ICTRL_mode);
#if 0
    WORD usRegOffsetPHY1ByID = DP_REG_OFFSET400(dprx_phy_id);

    if(ICTRL_mode == DPRx_EQ_ICTRL_NONE)
    {
        msWriteByteMask(REG_DPRX_PHY1_0C_L + usRegOffsetPHY1ByID, 0x03, 0x1F); // Force ictrl value
        msWriteByteMask(REG_DPRX_PHY1_0C_L + usRegOffsetPHY1ByID, 0, BIT7); // ictrl manual enable
    }
    else
    {
        msWriteByteMask(REG_DPRX_PHY1_0C_L + usRegOffsetPHY1ByID, ICTRL_mode, 0x1F); // Force ictrl value
        msWriteByteMask(REG_DPRX_PHY1_0C_L + usRegOffsetPHY1ByID, BIT7, BIT7); // ictrl manual enable
    }
#endif
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PhaseThresholdSetting()
//  [Description]
//					mhal_DPRx_PhaseThresholdSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PhaseThresholdSetting(DPRx_PHY_ID dprx_phy_id, BYTE Phase_Threshold)
{
    WORD usRegOffsetPHY2ByID = DP_REG_OFFSET400(dprx_phy_id);
    //reg_phase_quality_good
    msWriteByteMask(REG_DPRX_PHY2_64_L + usRegOffsetPHY2ByID, Phase_Threshold, 0x3F); // (08/40) = 0.2UI: phase good and report symbol lock

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetEQ_Current()
//  [Description]
//					mhal_DPRx_SetEQ_Current
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetEQ_Current(DPRx_PHY_ID dprx_phy_id,BOOL eq_current)
{
    WORD usRegOffsetPHY1ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffsetPHY3ByID = DP_REG_OFFSET400(dprx_phy_id);

    record_current_eq = eq_current;
    if(Current_rate[dprx_phy_id]==0x14)
    {
        if(eq_current==true)
        {
            //revese 4 lane eq pga inegc increase
            //============================================================
            msWriteByte(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x01);
            msWriteByte(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x01);
            msWriteByte(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x01);
            msWriteByte(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x01);

            msWriteByte(REG_DPRX_PHY1_28_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2A_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2C_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2E_H + usRegOffsetPHY1ByID, 0x20);
            //============================================================
            //eq pga main current increase
            //l0/l1/l2/l3 ov en
            msWriteByteMask(REG_DPRX_PHY1_28_H + usRegOffsetPHY1ByID, 0x04, 0x04);
            msWriteByteMask(REG_DPRX_PHY1_2A_H + usRegOffsetPHY1ByID, 0x04, 0x04);
            msWriteByteMask(REG_DPRX_PHY1_2C_H + usRegOffsetPHY1ByID, 0x04, 0x04);
            msWriteByteMask(REG_DPRX_PHY1_2E_H + usRegOffsetPHY1ByID, 0x04, 0x04);
            //l0 eq imain
            msWriteByteMask(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0xE0, 0xE0);
            msWriteByteMask(REG_DPRX_PHY1_28_H + usRegOffsetPHY1ByID, 0x01, 0x03);
            //l1 eq imain
            msWriteByteMask(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0xE0, 0xE0);
            msWriteByteMask(REG_DPRX_PHY1_2A_H + usRegOffsetPHY1ByID, 0x01, 0x03);
            //l2 eq imain
            msWriteByteMask(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0xE0, 0xE0);
            msWriteByteMask(REG_DPRX_PHY1_2C_H + usRegOffsetPHY1ByID, 0x01, 0x03);
            //l3 eq imain
            msWriteByteMask(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0xE0, 0xE0);
            msWriteByteMask(REG_DPRX_PHY1_2E_H + usRegOffsetPHY1ByID, 0x01, 0x03);
            //l0 pga imain
            msWriteByte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID,  0xBC);
            //l1 pga imain
            msWriteByte(REG_DPRX_PHY3_14_H + usRegOffsetPHY3ByID,  0xBC);
            //l2 pga imain
            msWriteByte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID,  0xBC);
            //l3 pga imain
            msWriteByte(REG_DPRX_PHY3_15_H + usRegOffsetPHY3ByID,  0xBC);
            //AVDDL_LDO 1.0V
            msWriteByteMask(REG_DPRX_PHY3_38_H + usRegOffsetPHY1ByID, 0x01, 0x03);

        }
        else
        {
            ////reverse 2by2 eq pga main current increase
            msWriteByte(REG_DPRX_PHY1_28_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2A_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2C_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2E_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_28_L + usRegOffsetPHY1ByID, 0x16);
            msWriteByte(REG_DPRX_PHY1_28_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2A_L + usRegOffsetPHY1ByID, 0x16);
            msWriteByte(REG_DPRX_PHY1_2A_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2C_L + usRegOffsetPHY1ByID, 0x16);
            msWriteByte(REG_DPRX_PHY1_2C_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY1_2E_L + usRegOffsetPHY1ByID, 0x16);
            msWriteByte(REG_DPRX_PHY1_2E_H + usRegOffsetPHY1ByID, 0x20);
            msWriteByte(REG_DPRX_PHY3_14_L + usRegOffsetPHY3ByID,0x80);
            msWriteByte(REG_DPRX_PHY3_14_H + usRegOffsetPHY3ByID,0x80);
            msWriteByte(REG_DPRX_PHY3_15_L + usRegOffsetPHY3ByID,0x80);
            msWriteByte(REG_DPRX_PHY3_15_H + usRegOffsetPHY3ByID,0x80);
            msWriteByte(REG_DPRX_PHY3_38_H + usRegOffsetPHY1ByID,0x00);
            //eq pga inegc increase 4 lane
            //l0/l1/l2/l3 ov en
            msWriteByteMask(REG_DPRX_PHY1_29_L + usRegOffsetPHY1ByID, 0x01, 0x01);
            msWriteByteMask(REG_DPRX_PHY1_2B_L + usRegOffsetPHY1ByID, 0x01, 0x01);
            msWriteByteMask(REG_DPRX_PHY1_2D_L + usRegOffsetPHY1ByID, 0x01, 0x01);
            msWriteByteMask(REG_DPRX_PHY1_2F_L + usRegOffsetPHY1ByID, 0x01, 0x01);
            //l0/l1/l2/l3 eq inegc
            msWriteByteMask(REG_DPRX_PHY1_28_H + usRegOffsetPHY1ByID, 0x40, 0xF8);
            msWriteByteMask(REG_DPRX_PHY1_2A_H + usRegOffsetPHY1ByID, 0x40, 0xF8);
            msWriteByteMask(REG_DPRX_PHY1_2C_H + usRegOffsetPHY1ByID, 0x40, 0xF8);
            msWriteByteMask(REG_DPRX_PHY1_2E_H + usRegOffsetPHY1ByID, 0x40, 0xF8);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_Check_AGC_Reset()
//  [Description]
//					mhal_DPRx_Check_AGC_Reset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_Check_AGC_Reset(DPRx_PHY_ID dprx_phy_id)
{
    WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);
    BYTE i, j;

    if(dprx_phy_id==0)
    {
        for(i=1 ; i<4 ; i++)//reset lane1~3 for check agc state
        {
            j = i << 6;
            msWriteByteMask(REG_DPRX_PHY0_72_H + usRegOffsetPHY0ByID, j, BIT6|BIT7 );
            if(msReadByte(REG_DPRX_PHY0_72_L + usRegOffsetPHY0ByID)==0x08)
            {
                msWriteByteMask(REG_DPRX_PHY0_7E_L + usRegOffsetPHY0ByID, BIT6, BIT6 );
                msWriteByteMask(REG_DPRX_PHY0_7E_L + usRegOffsetPHY0ByID, 0, BIT6 );
            }
            mhal_DPRx_DELAY_NOP(10);
        }
    }
    if(dprx_phy_id==1)
    {
        for(i=3 ; i>0 ; i--)//reset lane1~3 for check agc state
        {
            j = i << 6;
            msWriteByteMask(REG_DPRX_PHY0_72_H + usRegOffsetPHY0ByID, j, BIT6|BIT7 );
            if(msReadByte(REG_DPRX_PHY0_72_L + usRegOffsetPHY0ByID)==0x08)
            {
                msWriteByteMask(REG_DPRX_PHY0_7E_L + usRegOffsetPHY0ByID, BIT6, BIT6 );
                msWriteByteMask(REG_DPRX_PHY0_7E_L + usRegOffsetPHY0ByID, 0, BIT6 );
            }
            mhal_DPRx_DELAY_NOP(10);
        }
    }


    return;
}
#endif // ENABLE_DP_INPUT

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PHYPowerModeSetting()
//  [Description]
//					mhal_DPRx_PHYPowerModeSetting
//  [Arguments]:
//                  ucPMMode: ePM_POWERON/ePM_STANDBY/ePM_POWEROFF
//                  dprx_id: DP Rx Port ID, DPRx_ID_0~2: Combo, DPRx_ID_3: DPC
//                  dprx_phy_id: DPRx_PHY_ID_0: Combo, DPRx_PHY_ID_1: DPC
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PHYPowerModeSetting(DP_ePM_Mode dp_pm_Mode, DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id)
{
    WORD usRegOffsetPHY1ByID = DP_REG_OFFSET400(dprx_phy_id);
    WORD usRegOffset40ByID = DP_REG_OFFSET040(dprx_id);

#if (SET_DPC_PORT_AS_DP_PORT == 0x1)
    if(dprx_phy_id == 1) // TypeC to DP SQH setting sequence
    {
        switch(dp_pm_Mode)
        {
            case DP_ePM_POWERON:
                msWriteByteMask(REG_DPRX_PHY_PM_08_H + usRegOffset40ByID, 0x00, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_09_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_08_L + usRegOffset40ByID, 0xF0, 0xF0); // reg_nodie_en_sqh

                msWriteByteMask(REG_DPRX_PHY3_47_L + usRegOffsetPHY1ByID, BIT1, BIT1); // en_avddl
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0, BIT6); // reg_pd_bg
                msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0, 0x0F); // reg_pd_cal_saff_l3/2/1/0
                msWriteByteMask(REG_DPRX_PHY3_01_L + usRegOffsetPHY1ByID, 0, 0x0F); // reg_pd_dlpf
                msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0, 0xFF); // [7:4]: reg_pd_eq, [3:0]: reg_pd_dfe
                msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0, 0xF0); // reg_pd_pga
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0, BIT7); // reg_pd_ldo
                msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x10, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
                msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x10, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
                msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, 0, BIT0); // reg_pd_pll
                msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
                msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
                msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0, 0x0F); // reg_pd_dlev_saff_ov
                break;

            case DP_ePM_STANDBY:
                msWriteByteMask(REG_DPRX_PHY_PM_08_H + usRegOffset40ByID, 0x00, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_09_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_08_L + usRegOffset40ByID, 0xF0, 0xF0); // reg_nodie_en_sqh

                msWriteByteMask(REG_DPRX_PHY3_47_L + usRegOffsetPHY1ByID, 0, BIT1); // en_avddl
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_bg
                msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_cal_saff_l3/2/1/0
                msWriteByteMask(REG_DPRX_PHY3_01_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlpf
                msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0xFF, 0xFF); // [7:4]: reg_pd_eq, [3:0]: reg_pd_dfe
                msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0xF0, 0xF0); // reg_pd_pga
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT7, BIT7); // reg_pd_ldo
                msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
                msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
                msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, BIT0, BIT0); // reg_pd_pll
                msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
                msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
                msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlev_saff_ov
                break;

            case DP_ePM_POWEROFF:
                msWriteByteMask(REG_DPRX_PHY_PM_08_H + usRegOffset40ByID, 0x00, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_09_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_08_L + usRegOffset40ByID, 0xF0, 0xF0); // reg_nodie_en_sqh

                msWriteByteMask(REG_DPRX_PHY3_47_L + usRegOffsetPHY1ByID, 0, BIT1); // en_avddl
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_bg
                msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_cal_saff_l3/2/1/0
                msWriteByteMask(REG_DPRX_PHY3_01_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlpf
                msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0xFF, 0xFF); // [7:4]: reg_pd_eq, [3:0]: reg_pd_dfe
                msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0xF0, 0xF0); // reg_pd_pga
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT7, BIT7); // reg_pd_ldo
                msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
                msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
                msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, BIT0, BIT0); // reg_pd_pll
                msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
                msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
                msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlev_saff_ov
                break;

            default:
                break;
        }
    }
    else if(dprx_phy_id == 0)
    {
        switch(dp_pm_Mode)
        {
            case DP_ePM_POWERON:
                if(dprx_id < DPRx_ID_3) // Except DPC, DPC's R-term and SQH control is in USB register banks
                {
                    msWriteByteMask(REG_DPRX_PHY_PM_00_H + usRegOffset40ByID, 0x00, 0x0F); // reg_nodie_pd_rt
                    msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                    msWriteByteMask(REG_DPRX_PHY_PM_00_L + usRegOffset40ByID, 0xF0, 0xF0); // reg_nodie_en_sqh
                }

                msWriteByteMask(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_ctrl_ov for reg_pd_cml_diff, reg_pd_dfe, reg_pd_pga, reg_pd_eq, reg_pd_pll, and reg_pd_vco_vpump
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0, BIT6); // reg_pd_bg
                msWriteByteMask(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, BIT1, BIT1); // reg_pd_dlev_saff_ov_en
                msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0, 0x0F); // reg_pd_dlev_saff_ov
                msWriteByteMask(REG_DPRX_PHY3_51_H + usRegOffsetPHY1ByID, 0x01, 0xF1); // REG_PD_DLPF, [15:12]: reg_pd_dlpf_ov, [8]: reg_pd_dlpf_ov_en
                msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0, 0xF0); // reg_pd_dfe
                msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0, 0xFF); // REG_PD_LANE: [7:4]: reg_pd_pga, [3:0]: reg_pd_eq
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0, BIT7); // reg_pd_ldo
                msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x10, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
                msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x10, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
                msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, 0, BIT0); // reg_pd_pll
                msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
                msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
                msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0, 0x0F); // reg_pd_cal_saff_l3/2/1/0
                break;

            case DP_ePM_STANDBY:
                if(dprx_id < DPRx_ID_3) // Except DPC, DPC's R-term and SQH control is in USB register banks
                {
                    msWriteByteMask(REG_DPRX_PHY_PM_00_H + usRegOffset40ByID, 0x00, 0x0F); // reg_nodie_pd_rt
                    msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                    msWriteByteMask(REG_DPRX_PHY_PM_00_L + usRegOffset40ByID, 0xF0, 0xF0); // reg_nodie_en_sqh
                }

                msWriteByteMask(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_ctrl_ov for reg_pd_cml_diff, reg_pd_dfe, reg_pd_pga, reg_pd_eq, reg_pd_pll, and reg_pd_vco_vpump
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_bg
                msWriteByteMask(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, BIT1, BIT1); // reg_pd_dlev_saff_ov_en
                msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlev_saff_ov
                msWriteByteMask(REG_DPRX_PHY3_51_H + usRegOffsetPHY1ByID, 0xF1, 0xF1); // REG_PD_DLPF, [15:12]: reg_pd_dlpf_ov, [8]: reg_pd_dlpf_ov_en
                msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0xF0, 0xF0); // reg_pd_dfe
                msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0xFF, 0xFF); // REG_PD_LANE: [7:4]: reg_pd_pga, [3:0]: reg_pd_eq
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT7, BIT7); // reg_pd_ldo
                msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
                msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
                msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, BIT0, BIT0); // reg_pd_pll
                msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
                msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
                msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_cal_saff_l3/2/1/0
                break;

            case DP_ePM_POWEROFF:
                if(dprx_id < DPRx_ID_3) // Except DPC, DPC's R-term and SQH control is in USB register banks
                {
                    msWriteByteMask(REG_DPRX_PHY_PM_00_H + usRegOffset40ByID, 0x0F, 0x0F); // reg_nodie_pd_rt
                    msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                    msWriteByteMask(REG_DPRX_PHY_PM_00_L + usRegOffset40ByID, 0, 0xF0); // reg_nodie_en_sqh
                }

                msWriteByteMask(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_ctrl_ov for reg_pd_cml_diff, reg_pd_dfe, reg_pd_pga, reg_pd_eq, reg_pd_pll, and reg_pd_vco_vpump
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_bg
                msWriteByteMask(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, BIT1, BIT1); // reg_pd_dlev_saff_ov_en
                msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlev_saff_ov
                msWriteByteMask(REG_DPRX_PHY3_51_H + usRegOffsetPHY1ByID, 0xF1, 0xF1); // REG_PD_DLPF, [15:12]: reg_pd_dlpf_ov, [8]: reg_pd_dlpf_ov_en
                msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0xF0, 0xF0); // reg_pd_dfe
                msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0xFF, 0xFF); // REG_PD_LANE: [7:4]: reg_pd_pga, [3:0]: reg_pd_eq
                msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT7, BIT7); // reg_pd_ldo
                msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
                msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
                msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, BIT0, BIT0); // reg_pd_pll
                msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
                msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
                msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_cal_saff_l3/2/1/0
                break;

            default:
                break;
        }
    }
#else
    switch(dp_pm_Mode)
    {
        case DP_ePM_POWERON:
            if(dprx_id < DPRx_ID_3) // Except DPC, DPC's R-term and SQH control is in USB register banks
            {
                msWriteByteMask(REG_DPRX_PHY_PM_00_H + usRegOffset40ByID, 0x00, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + usRegOffset40ByID, 0xF0, 0xF0); // reg_nodie_en_sqh
            }

            msWriteByteMask(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_ctrl_ov for reg_pd_cml_diff, reg_pd_dfe, reg_pd_pga, reg_pd_eq, reg_pd_pll, and reg_pd_vco_vpump
            msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0, BIT6); // reg_pd_bg
            msWriteByteMask(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, BIT1, BIT1); // reg_pd_dlev_saff_ov_en
            msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0, 0x0F); // reg_pd_dlev_saff_ov
            msWriteByteMask(REG_DPRX_PHY3_51_H + usRegOffsetPHY1ByID, 0x01, 0xF1); // REG_PD_DLPF, [15:12]: reg_pd_dlpf_ov, [8]: reg_pd_dlpf_ov_en
            msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0, 0xF0); // reg_pd_dfe
            msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0, 0xFF); // REG_PD_LANE: [7:4]: reg_pd_pga, [3:0]: reg_pd_eq
            msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, 0, BIT7); // reg_pd_ldo
            msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x10, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
            msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x10, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
            msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, 0, BIT0); // reg_pd_pll
            msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
            msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
            msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0, 0x0F); // reg_pd_cal_saff_l3/2/1/0
            break;

        case DP_ePM_STANDBY:
            if(dprx_id < DPRx_ID_3) // Except DPC, DPC's R-term and SQH control is in USB register banks
            {
                msWriteByteMask(REG_DPRX_PHY_PM_00_H + usRegOffset40ByID, 0x00, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + usRegOffset40ByID, 0xF0, 0xF0); // reg_nodie_en_sqh
            }

            msWriteByteMask(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_ctrl_ov for reg_pd_cml_diff, reg_pd_dfe, reg_pd_pga, reg_pd_eq, reg_pd_pll, and reg_pd_vco_vpump
            msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_bg
            msWriteByteMask(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, BIT1, BIT1); // reg_pd_dlev_saff_ov_en
            msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlev_saff_ov
            msWriteByteMask(REG_DPRX_PHY3_51_H + usRegOffsetPHY1ByID, 0xF1, 0xF1); // REG_PD_DLPF, [15:12]: reg_pd_dlpf_ov, [8]: reg_pd_dlpf_ov_en
            msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0xF0, 0xF0); // reg_pd_dfe
            msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0xFF, 0xFF); // REG_PD_LANE: [7:4]: reg_pd_pga, [3:0]: reg_pd_eq
            msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT7, BIT7); // reg_pd_ldo
            msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
            msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
            msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, BIT0, BIT0); // reg_pd_pll
            msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
            msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
            msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_cal_saff_l3/2/1/0
            break;

        case DP_ePM_POWEROFF:
            if(dprx_id < DPRx_ID_3) // Except DPC, DPC's R-term and SQH control is in USB register banks
            {
                msWriteByteMask(REG_DPRX_PHY_PM_00_H + usRegOffset40ByID, 0x0F, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID, BIT0, BIT0); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + usRegOffset40ByID, 0, 0xF0); // reg_nodie_en_sqh
            }

            msWriteByteMask(REG_DPRX_PHY1_00_H + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_ctrl_ov for reg_pd_cml_diff, reg_pd_dfe, reg_pd_pga, reg_pd_eq, reg_pd_pll, and reg_pd_vco_vpump
            msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT6, BIT6); // reg_pd_bg
            msWriteByteMask(REG_DPRX_PHY1_57_L + usRegOffsetPHY1ByID, BIT1, BIT1); // reg_pd_dlev_saff_ov_en
            msWriteByteMask(REG_DPRX_PHY1_12_L + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_dlev_saff_ov
            msWriteByteMask(REG_DPRX_PHY3_51_H + usRegOffsetPHY1ByID, 0xF1, 0xF1); // REG_PD_DLPF, [15:12]: reg_pd_dlpf_ov, [8]: reg_pd_dlpf_ov_en
            msWriteByteMask(REG_DPRX_PHY1_19_H + usRegOffsetPHY1ByID, 0xF0, 0xF0); // reg_pd_dfe
            msWriteByteMask(REG_DPRX_PHY1_1A_L + usRegOffsetPHY1ByID, 0xFF, 0xFF); // REG_PD_LANE: [7:4]: reg_pd_pga, [3:0]: reg_pd_eq
            msWriteByteMask(REG_DPRX_PHY1_19_L + usRegOffsetPHY1ByID, BIT7, BIT7); // reg_pd_ldo
            msWriteByteMask(REG_DPRX_PHY1_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_I, [4]: reg_pd_phdac_ov_en, [3:0]: reg_pd_phdac_ov
            msWriteByteMask(REG_DPRX_PHY3_1B_L + usRegOffsetPHY1ByID, 0x1F, 0x1F); // reg_pd_phdac_Q, [4]: reg_pd_phdac_q_ov_en, [3:0]: reg_pd_phdac_q_ov
            msWriteByteMask(REG_DPRX_PHY1_1A_H + usRegOffsetPHY1ByID, BIT0, BIT0); // reg_pd_pll
            msWriteByteMask(REG_DPRX_PHY1_1F_H + usRegOffsetPHY1ByID, BIT4, BIT4); // REG_EN_LA_OV [12]: reg_en_la_ov_en
            msWriteByteMask(REG_DPRX_PHY1_1F_L + usRegOffsetPHY1ByID, 0, 0x0F); // REG_EN_LA [3:0]: reg_en_la_ov
            msWriteByteMask(REG_DPRX_PHY3_01_H + usRegOffsetPHY1ByID, 0x0F, 0x0F); // reg_pd_cal_saff_l3/2/1/0
            break;

        default:
            break;
    }
#endif

    return;
}
#endif // MHAL_DPRXPHY_C

