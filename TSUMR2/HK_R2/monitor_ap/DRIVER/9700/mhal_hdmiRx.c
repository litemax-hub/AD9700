#include "mhal_hdmiRx.h"
#include "Ms_rwreg.h"
#include "ms_reg_TSUMR2.h"
#include "Common.h" //for printf
#include "msEread.h"
#include "board.h"

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#else
#include <string.h>
#endif


#define HDMI_PHY_OLD_PATCH                  0

#define HDMI_DEBUG_MESSAGE_HAL      0
#if(HDMI_DEBUG_MESSAGE_HAL )
#define HDMI_HAL_DPUTSTR(str)            printMsg(str)
#define HDMI_HAL_DPRINTF(str, x)         printData(str, x)
#else
#define HDMI_HAL_DPUTSTR(str)
#define HDMI_HAL_DPRINTF(str, x)
#endif

#define H14_3GFULLRATE_PATCH  1
#define DLPF_DIV2  0    //U01 H14 = 1 & 0.95V;  H20 = 0 & 1.05V

#define PHY_SW_VERSION 0x0208
static WORD usTemp1 = 0;
static BYTE ubTemp2= 0;

#if (!ENABLE_HDMI_SCDC)
#warning "If HDMI need to support over 3.4G(HDMI_v2.0>3.4G, HDMI_v1.x<3.4G), please enable SCDC."
#elif(ENABLE_HPD_REPLACE_MODE)
#error "Not support this case, cannot enable SCDC and HPD_REPLACE in the same time!"
#endif

#define HDMI1P4_ANALOG_SETTING_OV_EN

#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
#define OV_EN_PATTERN_1 (BIT(3)|BIT(7)|BIT(11)|BIT(15))
#define OV_EN_PATTERN_2 (BIT(7)|BIT(15))
#define OV_EN_PATTERN_3 (BIT(3)|BIT(11)|BIT(15))

//#define OV_ELEMENTS_H2P1_PORT_N (sizeof(ov_table_h2p1_port)/sizeof(s_ov_element))
#define OV_ELEMENTS_H2P0_PORT_N (sizeof(ov_table_h2p0_port)/sizeof(s_ov_element))

typedef struct{
    DWORD u32_reg;
    WORD u16_msk;
    WORD u16_ov_en_msk;
    WORD u16_val[5];
    WORD u16_rsv;
}s_ov_element;

typedef struct{
    DWORD u32_n;
    s_ov_element* ptr_ov_table;
}s_ov_table;

typedef struct{
    BYTE u8_analog_ov_en;
    BYTE rev[3];
    s_ov_table s_ov_table_a;
}stHDMI_phy_parm;
#endif


#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
inline static void phy_param_init(BYTE enInputPortType,MS_BOOL b_h2p1_port);
inline static MS_BOOL _Hal_HDMI_Fix_Analog_Setting(BYTE enInputPortType,MS_BOOL b_ov_en);
#endif
#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
/*
band0, 25~50
band1, 50~90
band2, 90~180
band3, 180~300
band4, 300~360
*/

/*
static s_ov_element ov_table_h2p1_port[]={
//                                                band0 ,band1 ,band2 ,band3 ,band4
    {REG_PHY2P1_2_P0_00_L,0xffff,OV_EN_PATTERN_1,{0x5555,0x5555,0x5555,0x4444,0x4444},0}, // 1.dfe_bw
    {REG_PHY2P1_2_P0_01_L,0xffff,OV_EN_PATTERN_1,{0x2222,0x2222,0x2222,0x2222,0x2222},0}, // 2.dfe_mode
    {REG_PHY2P1_2_P0_1C_L,0xbbbb,OV_EN_PATTERN_1,{0x0000,0x0000,0x0000,0x1111,0x1111},0}, // 3.input_div_sel
    {REG_PHY2P1_2_P0_1D_L,0xbbbb,OV_EN_PATTERN_1,{0x0000,0x0000,0x0000,0x0000,0x0000},0}, // 4.feedback_div_mhal_mode
    {REG_PHY2P1_2_P0_1E_L,0xffff,OV_EN_PATTERN_1,{0x1111,0x0000,0x0000,0x0000,0x0000},0}, // 5.feedback_div_pll
    {REG_PHY2P1_2_P0_1F_L,0xffff,OV_EN_PATTERN_1,{0x2222,0x1111,0x1111,0x0000,0x0000},0}, // 6.post_div_vco_phdac
    {REG_PHY2P1_2_P0_26_L,0x00f0,BIT(7)         ,{0xc077,0xc077,0xc077,0xc057,0xc047},0}, // 7.eq_bias_current_mode
    {REG_PHY2P1_2_P0_27_L,0xffff,OV_EN_PATTERN_1,{0x6666,0x6666,0x6666,0x6666,0x6666},0}, // 8.pll lpf resistor
    {REG_PHY2P1_2_P0_28_L,0x9f9f,OV_EN_PATTERN_2,{0x0202,0x0202,0x0202,0x0202,0x0202},0}, // 9.pll charge dump bias for pd mode
    {REG_PHY2P1_2_P0_29_L,0x9f9f,OV_EN_PATTERN_2,{0x0202,0x0202,0x0202,0x0202,0x0202},0}, // 9.pll charge dump bias for pd mode
    {REG_PHY2P1_2_P0_2A_L,0x9f9f,OV_EN_PATTERN_2,{0x0303,0x0303,0x0202,0x0202,0x0202},0}, // 10.pll charge dump bias for pfd mode
    {REG_PHY2P1_2_P0_2B_L,0x9f9f,OV_EN_PATTERN_2,{0x0303,0x0303,0x0202,0x0202,0x0202},0}, // 10.pll charge dump bias for pfd mode
    {REG_PHY2P1_2_P0_2C_L,0x0070,BIT(6)         ,{0x0010,0x0010,0x0010,0x0010,0x0110},0}, // 11.output of clk_vcodiv_sel
    {REG_PHY2P1_2_P0_6C_L,0x000f,BIT(1)|BIT(3)  ,{0x000e,0x000e,0x000e,0x000e,0x000e},0}, // 12.datarate_div36_double, LA/15 for nyquist rate LTP3
    {REG_PHY2P1_3_P0_0B_L,0xbbbb,OV_EN_PATTERN_1,{0x2222,0x2222,0x1111,0x1111,0x1111},0}, // 13.phdac ldo voltage sel
};
*/


static s_ov_element ov_table_h2p0_port[]={
    {REG_PHY2P1_2_P0_00_L,0xffff,OV_EN_PATTERN_1,{0x0000,0x0000,0x0000,0x0000,0x0000},0}, // 1.dfe_bw
    {REG_PHY2P1_2_P0_01_L,0xbbbb,OV_EN_PATTERN_1,{0xaaaa,0xaaaa,0xaaaa,0xaaaa,0x8888},0}, // 2.dfe_mode
    {REG_PHY2P1_2_P0_1A_L,0xff0b,OV_EN_PATTERN_3,{0x3600,0x2500,0x1400,0x1501,0x0502},0}, // 3.input_div_sel, feedback_div_pll, post_div_vco_phdac
    {REG_PHY2P1_2_P0_26_L,0x00f0,BIT(7)         ,{0x0170,0x0170,0x0170,0x0150,0x0140},0}, // 4.eq_bias_current_mode
    {REG_PHY2P1_2_P0_27_L,0x09ff,BIT(3)|BIT(11) ,{0x00c7,0x0007,0x0000,0x0007,0x0000},0}, // 5.pll lpf resistor,charge dump pd
    {REG_PHY2P1_2_P0_28_L,0x009f,BIT(7)         ,{0x0011,0x0011,0x0001,0x0011,0x0011},0}, // 6.charge dump pfd
    {REG_PHY2P1_2_P0_2C_L,0x0070,BIT(6)         ,{0x0010,0x0010,0x0010,0x0010,0x0110},0}, // 7.output of clk_vcodiv_sel
    {REG_PHY2P1_2_P0_6C_L,0x000f,BIT(1)|BIT(3)  ,{0x010e,0x010e,0x010e,0x010e,0x010e},0}, // 8.datarate_div36_double, LA/15 for nyquist rate LTP3
    {REG_PHY2P1_3_P0_0A_L,0x0300,BIT(9)         ,{0x0100,0x0100,0x0100,0x0100,0x0100},0}, // 9.datarate_div36 to div20
};

static stHDMI_phy_parm s_hdmi_phy_parm;
#endif

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_DeCodeGray()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _Hal_tmds_DeCodeGray(BYTE u8GrayValue)
{
    BYTE u8BinaryValue = 0;

    while(u8GrayValue != 0x00)
    {
        u8BinaryValue = u8BinaryValue ^ u8GrayValue;
        u8GrayValue = u8GrayValue >> 1;
    }

    return u8BinaryValue;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_EnCodeGray()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _Hal_tmds_EnCodeGray(BYTE u8BinaryValue)
{
    BYTE u8GrayValue = u8BinaryValue ^ (u8BinaryValue >> 1);

    return u8GrayValue;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_ActiveCableCheckPhase()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_ActiveCableCheckPhase(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    Bool bActiveCableFlag = FALSE;
    BYTE u8temp = 0;
    BYTE u8PhaseCount[3] = {0};
    volatile WORD u16temp = 1000;
    WORD u16RegValue = 0;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
    {
        msWrite2ByteMask(REG_PHY2P1_0_P0_51_L +u32PHY2P1BankOffset, 0x0100, BMASK(10:8)|BIT(3)); // phy0_51[3]: reg_en_dlev_sw_mode; [10:8]: reg_filter_depth_dlev
    }


    // Overwrite VDAC code = 0
    msWrite2ByteMask(REG_PHY2P1_2_P0_14_L +u32PHY2P1BankOffset, 0x8000, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_15_L +u32PHY2P1BankOffset, 0x8000, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_16_L +u32PHY2P1BankOffset, 0x8000, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_17_L +u32PHY2P1BankOffset, 0x8000, BIT(15)|BMASK(8:0));

    // Overwrite FSM
    msWrite2Byte(REG_PHY2P1_0_P0_10_L +u32PHY2P1BankOffset, 0x0080);
    msWrite2Byte(REG_PHY2P1_0_P0_11_L +u32PHY2P1BankOffset, 0x0080);
    msWrite2Byte(REG_PHY2P1_0_P0_12_L +u32PHY2P1BankOffset, 0x0080);
    msWrite2Byte(REG_PHY2P1_0_P0_13_L +u32PHY2P1BankOffset, 0x0080);

    while(u16temp--);

    msWrite2Byte(REG_PHY2P1_0_P0_3E_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_42_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_46_L +u32PHY2P1BankOffset, 0);

    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_0B_L +u32DTOPBankOffset, 0x0100, BMASK(15:8));

    // Re-trigger auto scan
    msWrite2ByteMask(REG_PHY2P1_0_P0_34_L +u32PHY2P1BankOffset, 0xF000, 0xF00C);

    // Re-trigger auto EQ
    //_Hal_tmds_NewPHYAutoEQTrigger(enInputPortType);

    u16temp = 60000;

    while(u16temp > 0)
    {
        // Wait scan phase done
        if(msRead2Byte(REG_PHY2P1_0_P0_3F_L +u32PHY2P1BankOffset) &BIT(2))
        {
            if(msRead2Byte(REG_PHY2P1_0_P0_43_L +u32PHY2P1BankOffset) &BIT(2))
            {
                if(msRead2Byte(REG_PHY2P1_0_P0_47_L +u32PHY2P1BankOffset) &BIT(2))
                {
                    HDMI_HAL_DPRINTF("** HDMI auto scan done %d\r\n", u16temp);

                    break;
                }
            }
        }

        u16temp--;
    }

    if(u16temp > 0)
    {
        // Count phase count
        for(u8temp = 0; u8temp < 3; u8temp++)
        {
            msWrite2ByteMask(REG_PHY2P1_0_P0_2D_L +u32PHY2P1BankOffset, (u8temp << 10), BMASK(11:10));

            u16RegValue = msRead2Byte(REG_PHY2P1_0_P0_31_L +u32PHY2P1BankOffset);
            u8PhaseCount[u8temp] = (u16RegValue >> 8) & 0x7F;
        }

        HDMI_HAL_DPRINTF("** HDMI active cable phase scan done value0 %d \r\n", u8PhaseCount[0]);
        HDMI_HAL_DPRINTF("** HDMI active cable phase scan done value1 %d \r\n", u8PhaseCount[1]);
        HDMI_HAL_DPRINTF("** HDMI active cable phase scan done value2 %d \r\n", u8PhaseCount[2]);


        if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
        {
            bActiveCableFlag = (u8PhaseCount[0] > HDMI_SCAN_PHASE_THRESHOLD_14) & (u8PhaseCount[1] > HDMI_SCAN_PHASE_THRESHOLD_14) & (u8PhaseCount[2] > HDMI_SCAN_PHASE_THRESHOLD_14);
        }

        if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20)
        {
            bActiveCableFlag = (u8PhaseCount[0] > HDMI_SCAN_PHASE_THRESHOLD_20) & (u8PhaseCount[1] > HDMI_SCAN_PHASE_THRESHOLD_20) & (u8PhaseCount[2] > HDMI_SCAN_PHASE_THRESHOLD_20);
        }
    }
    else
    {
        HDMI_HAL_DPUTSTR("** HDMI auto scan not done \r\n");
    }

    return bActiveCableFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_InitInternalInfo()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_InitInternalInfo(ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    pstHDMIPollingInfo->u8FRLRate = HDMI_FRL_MODE_LEGACY;
    pstHDMIPollingInfo->bClockStableFlag = FALSE;
    pstHDMIPollingInfo->bVersionChangeFlag = FALSE;
    pstHDMIPollingInfo->bActiveCableFlag = FALSE;
    pstHDMIPollingInfo->bResetDCFifoFlag = FALSE;
    pstHDMIPollingInfo->u8RatioDetect = HDMI_FRL_MODE_LEGACY;
    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_NONE;
    pstHDMIPollingInfo->bPowerControlOn = FALSE;
    pstHDMIPollingInfo->bForcePowerDown = FALSE;
    pstHDMIPollingInfo->u8Colordepth = HDMI_COLOR_DEPTH_NONE;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetNewPHYEQValue()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_GetNewPHYEQValue(BYTE enInputPortType, BYTE *pu8EQValue)
{
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    // gray code
    pu8EQValue[3] = msRead2Byte(REG_PHY2P1_2_P0_24_L +u32PHY2P1BankOffset);
    pu8EQValue[0] = msRead2Byte(REG_PHY2P1_2_P0_24_L +u32PHY2P1BankOffset) >> 8;
    pu8EQValue[1] = msRead2Byte(REG_PHY2P1_2_P0_25_L +u32PHY2P1BankOffset);
    pu8EQValue[2] = msRead2Byte(REG_PHY2P1_2_P0_25_L +u32PHY2P1BankOffset) >> 8;

    // binary code
    pu8EQValue[4] = _Hal_tmds_DeCodeGray(pu8EQValue[0] & BMASK(6:0));
    pu8EQValue[5] = _Hal_tmds_DeCodeGray(pu8EQValue[1] & BMASK(6:0));
    pu8EQValue[6] = _Hal_tmds_DeCodeGray(pu8EQValue[2] & BMASK(6:0));
    pu8EQValue[7] = _Hal_tmds_DeCodeGray(pu8EQValue[3] & BMASK(6:0));
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_SetMacPowerDown()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_SetMacPowerDown(BYTE enInputPortType, Bool bPowerDown)
{

    enInputPortType = enInputPortType;
    msWrite2Byte(REG_COMBO_GP_TOP_49_L, BIT(4)); // combo_gp_top_49[4]: reg_flat_asmp_rcvd

    msWrite2Byte(REG_COMBO_GP_TOP_10_L, bPowerDown? BMASK(15:1) : 0); // combo_gp_top_10[15:0]: reg_dtop_clk_pd_p0
    msWrite2Byte(REG_COMBO_GP_TOP_11_L, bPowerDown? BMASK(15:0) : 0); // combo_gp_top_11[15:0]: reg_dtop_clk_pd_p0
    msWrite2ByteMask(REG_COMBO_GP_TOP_13_L, bPowerDown? BMASK(7:0) : 0, BMASK(7:0)); // combo_gp_top_13[7:0]: reg_ve_clk_pd_p0

    /*switch(enInputPortType)
    {
        case INPUT_PORT_DVI0:   // 1739_10
            msWrite2Byte(REG_COMBO_GP_TOP_10_L, bPowerDown? BMASK(15:1) : 0); // combo_gp_top_10[15:0]: reg_dtop_clk_pd_p0
            msWrite2Byte(REG_COMBO_GP_TOP_11_L, bPowerDown? BMASK(15:0) : 0); // combo_gp_top_11[15:0]: reg_dtop_clk_pd_p0
            msWrite2ByteMask(REG_COMBO_GP_TOP_13_L, bPowerDown? BMASK(7:0) : 0, BMASK(7:0)); // combo_gp_top_13[7:0]: reg_ve_clk_pd_p0
            break;

        case INPUT_PORT_DVI1:   // 1739_18
            msWrite2Byte(REG_COMBO_GP_TOP_18_L, bPowerDown? BMASK(15:1) : 0); // combo_gp_top_18[15:0]: reg_dtop_clk_pd_p1
            msWrite2Byte(REG_COMBO_GP_TOP_19_L, bPowerDown? BMASK(15:0) : 0); // combo_gp_top_19[15:0]: reg_dtop_clk_pd_p1
            msWrite2ByteMask(REG_COMBO_GP_TOP_1B_L, bPowerDown? BMASK(7:0) : 0, BMASK(7:0)); // combo_gp_top_1B[7:0]: reg_ve_clk_pd_p1
            break;

        case INPUT_PORT_DVI2:   // 1739_20
            msWrite2Byte(REG_COMBO_GP_TOP_20_L, bPowerDown? BMASK(15:1) : 0); // combo_gp_top_20[15:0]: reg_dtop_clk_pd_p2
            msWrite2Byte(REG_COMBO_GP_TOP_21_L, bPowerDown? BMASK(15:0) : 0); // combo_gp_top_21[15:0]: reg_dtop_clk_pd_p2
            msWrite2ByteMask(REG_COMBO_GP_TOP_23_L, bPowerDown? BMASK(7:0) : 0, BMASK(7:0)); // combo_gp_top_23[7:0]: reg_ve_clk_pd_p2
            break;

        case INPUT_PORT_DVI3:   // 1739_28
            msWrite2Byte(REG_COMBO_GP_TOP_28_L, bPowerDown? BMASK(15:1) : 0); // combo_gp_top_28[15:0]: reg_dtop_clk_pd_p3
            msWrite2Byte(REG_COMBO_GP_TOP_29_L, bPowerDown? BMASK(15:0) : 0); // combo_gp_top_29[15:0]: reg_dtop_clk_pd_p3
            msWrite2ByteMask(REG_COMBO_GP_TOP_2B_L, bPowerDown? BMASK(7:0) : 0, BMASK(7:0)); // combo_gp_top_2B[7:0]: reg_ve_clk_pd_p3
            break;

        default:
            break;
    }*/
    HDMI_HAL_DPRINTF("** HDMI SetMacPower, port: %d\r\n", enInputPortType);
    HDMI_HAL_DPRINTF("** HDMI SetMacPower, bPowerDown: %d.\r\n", bPowerDown);


}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_SetPowerDown()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_SetPowerDown(BYTE enInputPortType, Bool bPowerDown)
{
    volatile WORD u16temp = 400;
    DWORD u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    //DWORD u32PHY2P1PMBankOffset = 0; //_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
    enInputPortType = enInputPortType;

    // REG_PD_BG
    msWrite2ByteMask(REG_PHY2P1_3_P0_0A_L +u32PHY2P1BankOffset, 0x0000, BIT(1)); // phy3_0a[1] reg_atop_pd_hw_ctrl_en
    //msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset, bPowerDown? 0x4000: 0x0000, BIT(14)); // phy2_2e[14] reg_pd_bg=1'b1

    // REG_PD_LDO
    //msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, bPowerDown? 0x0040: 0x0000, BIT(6)); // phy2_6e[6] reg_pd_ldo

    // REG_PD_PLL
    //msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, bPowerDown? 0x00C0: 0x0080, BMASK(7:6)); // phy2_2f[6] reg_pd_pll_ov, phy2_2f[7] reg_pd_pll_ov_en

    while(u16temp--); // 200count : 1us

    // REG_PD_CAL_CAL_SAFF_L0, REG_PD_CAL_CAL_SAFF_L1, REG_PD_CAL_CAL_SAFF_L2, REG_PD_CAL_CAL_SAFF_L3
    msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset, bPowerDown? 0x00FF: 0x000F, BMASK(7:0)); // phy2_1f[3:0] reg_pd_cal_saff_l0-3_ov_en, phy2_1f[7:4] reg_pd_cal_saff_l0-3_ov

    // REG_PD_CLKIN
    //msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset, bPowerDown? 0x000C: 0x0004, BMASK(3:2)); // phy2_2e[2] reg_pd_clkin_ov_en, phy2_2e[3] reg_pd_clkin_ov

    // REG_PD_PHD_CAL_L0, REG_PD_PHD_CAL_L1, REG_PD_PHD_CAL_L2, REG_PD_PHD_CAL_L3
    msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset, bPowerDown? 0xFF00: 0x0F00, BMASK(15:8)); // phy2_1f[11:8] reg_pd_phd_cal_l0-3_ov_en, phy2_1f[15:12] reg_pd_phd_cal_l0-3_ov

    // REG_PD_UPDN_DMX
    msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, bPowerDown? 0x0180: 0x0080, BMASK(8:7)); // phy2_6e[7] reg_pd_updn_dmx_ov_en, phy2_6e[8] reg_pd_updn_dmx_ov

    // REG_PD_LANE
    //msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, bPowerDown? 0x001F: 0x0010, BMASK(4:0)); // phy2_2f[4] reg_pd_lane_ov_en, phy2_2f[3:0] reg_pd_lane_ov

    // REG_PD_DLPF
    //msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset, bPowerDown? 0x00F1: 0x0081, BMASK(7:4)|BIT(0)); // phy2_05[0] reg_pd_dlpf_ov_en, phy2_05[7:4] reg_pd_dlpf_ov

    // REG_PD_PHDAC_I
    msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset, 0x0080, BIT(7)); // phy2_2e[7] reg_pd_phdac_i_ov_en
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, bPowerDown? 0x0F00: 0x0000, BMASK(11:8)); // phy2_2f[11:8] reg_pd_phdac_i_ov

    // REG_PD_PHDAC_Q
    msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset, 0x2000, BIT(13)); // phy2_2e[13] reg_pd_phdac_q_ov_en
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, bPowerDown? 0xF000: 0x0000, BMASK(15:12)); // phy2_2f[15:12] reg_pd_phdac_q_ov

#if 0 //TBD check bank 0x20
    // NODIE_PD_RT
    msWrite2ByteMask(REG_PHY2P1_PM_P0_42_L + u32PHY2P1PMBankOffset, bPowerDown? 0x8F00: 0x8000, BIT(15)|BMASK(11:8)); // phy2p1_pm_42[11:8] reg_nodie_pd_rt_ov, phy2p1_pm_42[15] reg_nodie_pd_rt_ov_en

    // NODIE_EN_SQH
    msWrite2ByteMask(REG_PHY2P1_PM_P0_40_L + u32PHY2P1PMBankOffset, bPowerDown? 0x0008: 0x000C, BMASK(3:2)); // phy2p1_pm_40[3] reg_nodie_en_sqh_ov_en, phy2p1_pm_40[2] reg_nodie_en_sqh_ov

    // NODIE_PD_CLKIN
    msWrite2ByteMask(REG_PHY2P1_PM_P0_42_L + u32PHY2P1PMBankOffset, bPowerDown? 0x0030: 0x0020, BMASK(5:4)); // phy2p1_42[4] reg_nodie_pd_clkin_ov, phy2p1_42[5] reg_nodie_pd_clkin_ov_en
#endif
    /*
    phy2_04[7:4] reg_pd_dfe_ov
    phy2_04[11:8] reg_pd_dfe_vdac_ov
    phy2_04[15:12] reg_pd_dlev_saff_ov
    */
    msWrite2ByteMask(REG_PHY2P1_2_P0_04_L + u32PHY2P1BankOffset, bPowerDown? 0xFFF0: 0x0000, BMASK(11:4));

    // phy2_05[11:8] reg_pd_phd_dec_ov
    msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset, bPowerDown? 0x0700: 0x0000, BMASK(10:8));

    // phy2_2e[4] reg_en_clko_pix_ov
    msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset, bPowerDown? 0x0000: 0x0010, BIT(4));

    // phy2_6e[5] reg_en_clko_vcodiv
    msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, bPowerDown? 0x0000: 0x0020, BIT(5));

    /*
    phy3_60[9:8] reg_ckg_xtal
    phy3_60[11:10] reg_ckg_mpll
    phy3_60[13:12] reg_ckg_dvi_raw
    */
    msWrite2ByteMask(REG_PHY2P1_3_P0_60_L + u32PHY2P1BankOffset, bPowerDown? 0x1500: 0x0000, BMASK(13:8));

    /*
    phy3_61[1:0] reg_ckg_clko_data_l0_1st
    phy3_61[3:2] reg_ckg_clko_data_l1_1st
    phy3_61[5:4] reg_ckg_clko_data_l2_1st
    phy3_61[7:6] reg_ckg_clko_data_l3_1st
    phy3_61[9:8] reg_ckg_clko_edata_l0_1st
    phy3_61[11:10] reg_ckg_clko_edata_l1_1st
    phy3_61[13:12] reg_ckg_clko_edata_l2_1st
    phy3_61[15:14] reg_ckg_clko_edata_l3_1st
    */
    msWrite2Byte(REG_PHY2P1_3_P0_61_L +u32PHY2P1BankOffset, bPowerDown? 0x5555: 0x00AA);//for MT9700 clk inv

    /*
    phy3_62[1:0] reg_ckg_clko_data_l0_swap
    phy3_62[3:2] reg_ckg_clko_data_l1_swap
    phy3_62[5:4] reg_ckg_clko_data_l2_swap
    phy3_62[7:6] reg_ckg_clko_data_l3_swap
    phy3_62[9:8] reg_ckg_clko_edata_l0_swap
    phy3_62[11:10] reg_ckg_clko_edata_l1_swap
    phy3_62[13:12] reg_ckg_clko_edata_l2_swap
    phy3_62[15:14] reg_ckg_clko_edata_l3_swap
    */
    msWrite2Byte(REG_PHY2P1_3_P0_62_L +u32PHY2P1BankOffset, bPowerDown? 0x5555: 0x00AA);//for MT9700 clk inv

    /*
    phy3_63[1:0] reg_ckg_lckdet_swap
    phy3_63[3:2] reg_ckg_pll_fb_swap
    */
    msWrite2ByteMask(REG_PHY2P1_3_P0_63_L +u32PHY2P1BankOffset, bPowerDown? 0x0005: 0x0000, BMASK(3:0));

    /*
    phy3_64[1:0] reg_ckg_clko_data_af_ls_40
    phy3_64[3:2] reg_ckg_clko_data_af_ls_36
    phy3_64[15:14] reg_ckg_clk_af_ls_phy_hd20
    */
    msWrite2ByteMask(REG_PHY2P1_3_P0_64_L +u32PHY2P1BankOffset, bPowerDown? 0x4005: 0x0000, 0xC00F);

    /*
    phy3_68[1:0] reg_ckg_br_det_l0_swap
    phy3_68[3:2] reg_ckg_br_det_l1_swap
    phy3_68[5:4] reg_ckg_br_det_l2_swap
    phy3_68[7:6] reg_ckg_br_det_l3_swap
    phy3_68[9:8] reg_ckg_tmds_fg
    */
    msWrite2ByteMask(REG_PHY2P1_3_P0_68_L +u32PHY2P1BankOffset, bPowerDown? 0x0155: 0x0000, BMASK(9:0));

    //--------------HDMI MAC------------
    _Hal_tmds_SetMacPowerDown(enInputPortType, bPowerDown);

}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_ComboPortMapping2DDCGPIOConfig()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE Hal_HDMI_ComboPortMapping2DDCGPIOConfig(BYTE enInputPortType)
{
    BYTE u8DDCPortIndex = HDMI_DDC_None;

    switch(enInputPortType)
    {
        case HDMI_INPUT_PORT0:
                u8DDCPortIndex = HDMI_Rx0;
            break;

        case HDMI_INPUT_PORT1:
                u8DDCPortIndex = HDMI_Rx1;
            break;

        case HDMI_INPUT_PORT2:
                u8DDCPortIndex = HDMI_Rx2;
            break;

        default:
            break;
    }

    return u8DDCPortIndex;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_DDCControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_DDCControl(BYTE enInputPortType, Bool bEnable)
{
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortType);
    Bool bFlag = 0;
#if (ENABLE_HDMI_SCDC || ENABLE_HPD_REPLACE_MODE)
    bFlag = bEnable;
#endif

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            msWrite2ByteMask(REG_SCDC_P0_00_L, bFlag, BIT(0)); //scdc_p1_00[0]: SCDC enable
            break;

        case HDMI_DDC_1:
            msWrite2ByteMask(REG_SCDC_P1_00_L, bFlag, BIT(0)); //scdc_p1_00[0]: SCDC enable
            break;

        case HDMI_DDC_2:
            msWrite2ByteMask(REG_SCDC_P2_00_L, bFlag, BIT(0)); //scdc_p2_00[0]: SCDC enable
            break;

        default:
            break;
    }

#if ENABLE_HPD_REPLACE_MODE
    //do nothing
#else
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L, bEnable?BIT(10):0x0, BIT(10)); //HDCP enalbe for DDC
#endif
}

#if ENABLE_HPD_REPLACE_MODE
//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_MAC_HDCP_Enable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_MAC_HDCP_Enable(Bool bEnable)
{
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L, bEnable?BIT(10):0x0, BIT(10)); //HDCP enalbe for DDC
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_SCDC_status()
//  [Description]
//
//  [Arguments]:
//              bDDC : 0 (0xA8/0xA9), 1 (0x74/0x75)
//  [Return]:
//
//**************************************************************************
BOOL Hal_HDMI_SCDC_status(BYTE enInputPortSelect)
{
    BYTE u8HDMI_DDC_Mapping = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    BOOL bFlag = FALSE;
    switch(u8HDMI_DDC_Mapping)
    {
        case HDMI_DDC_0:
            if(msRead2Byte(REG_SCDC_P0_07_L) & BIT6)
            {
                msWrite2ByteMask(REG_SCDC_P0_07_L, BIT14, BIT14); //IRQ CLR
                msWrite2ByteMask(REG_SCDC_P0_07_L, 0, BIT14);
                bFlag = TRUE;
            }
            break;

        case HDMI_DDC_1:
            if(msRead2Byte(REG_SCDC_P1_07_L) & BIT6)
            {
                msWrite2ByteMask(REG_SCDC_P1_07_L, BIT14, BIT14); //IRQ CLR
                msWrite2ByteMask(REG_SCDC_P1_07_L, 0, BIT14);
                bFlag = TRUE;
            }
            break;

        case HDMI_DDC_2:
            if(msRead2Byte(REG_SCDC_P2_07_L) & BIT6)
            {
                msWrite2ByteMask(REG_SCDC_P2_07_L, BIT14, BIT14); //IRQ CLR
                msWrite2ByteMask(REG_SCDC_P2_07_L, 0, BIT14);
                bFlag = TRUE;
            }
            break;

        default:
            break;
    }
    return bFlag;
}


//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_SCDC_config()
//  [Description]
//
//  [Arguments]:
//              bDDC : 0 (0xA8/0xA9), 1 (0x74/0x75)
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_SCDC_config(BYTE enInputPortSelect, BYTE bDDC)
{
    BYTE u8HDMI_DDC_Mapping = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    switch(u8HDMI_DDC_Mapping)
    {
        case HDMI_DDC_0:
            msWrite2ByteMask(REG_SCDC_P0_00_L, bDDC ? 0x7401 : 0xA800, 0xFF01); //scdc_p0_00[15:8]: SCDC address
            msWrite2ByteMask(REG_SCDC_P0_01_L, bDDC ? 0x7574 : 0xA9A8, 0xFFFF); //scdc_p0_01[15:8]: ID1, [7:0]: ID0
            break;

        case HDMI_DDC_1:
            msWrite2ByteMask(REG_SCDC_P1_00_L, bDDC ? 0x7401 : 0xA800, 0xFF01); //scdc_p1_00[15:8]: SCDC address
            msWrite2ByteMask(REG_SCDC_P1_01_L, bDDC ? 0x7574 : 0xA9A8, 0xFFFF); //scdc_p1_01[15:8]: ID1, [7:0]: ID0
            break;

        case HDMI_DDC_2:
            msWrite2ByteMask(REG_SCDC_P2_00_L, bDDC ? 0x7401 : 0xA800, 0xFF01); //scdc_p2_00[15:8]: SCDC address
            msWrite2ByteMask(REG_SCDC_P2_01_L, bDDC ? 0x7574 : 0xA9A8, 0xFFFF); //scdc_p2_01[15:8]: ID1, [7:0]: ID0
            break;

        default:
            break;
    }
}


//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_SCDC_Clr()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_SCDC_Clr(BYTE enInputPortSelect)
{
    BYTE u8HDMI_DDC_Mapping = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);
    WORD i;
    switch(u8HDMI_DDC_Mapping)
    {
        case HDMI_DDC_0:
            msWrite2ByteMask(REG_SCDC_P0_02_L, 0x2000, 0x23FF); //[13]:enable write, [9:0]:Address
            msWrite2ByteMask(REG_SCDC_P0_02_L, BIT10, BIT10); //[10]:address pulse
            for(i = 0; i < 256; i++)
            {
                if(i == 1)
                    msWrite2ByteMask(REG_SCDC_P0_03_L, 0x0001, 0x00FF);//[7:0]: write data, SCDC version
                else
                    msWrite2ByteMask(REG_SCDC_P0_03_L, 0, 0x00FF);//[7:0]: write data, clear SCDC content
                msWrite2ByteMask(REG_SCDC_P0_03_L, BIT8, BIT8);//[8]: write pulse
                msRead2Byte(REG_SCDC_P0_05_L); //dummy read for delay
            }
            msWrite2ByteMask(REG_SCDC_P0_02_L, 0x0000, 0x23FF); //[13]:disable write, [9:0]:Address
            msWrite2ByteMask(REG_SCDC_P0_07_L, 0xFF00, 0xFF00); //IRQ CLR
            msWrite2ByteMask(REG_SCDC_P0_07_L, 0, 0xFF00);
            if(msRead2Byte(REG_SCDC_P0_0C_L) & BIT3)
            {
                msWrite2ByteMask(REG_SCDC_P0_0C_L, BIT2, BIT2); //5V_Reset_OV_value
                msWrite2ByteMask(REG_SCDC_P0_0C_L, 0, BIT2);
            }
            else
            {
                msWrite2ByteMask(REG_SCDC_P0_0C_L, BIT3, BIT3); //5V_Reset_OV_en
                msWrite2ByteMask(REG_SCDC_P0_0C_L, BIT2, BIT2); ///5V_Reset_OV_value
                msWrite2ByteMask(REG_SCDC_P0_0C_L, 0, BIT2);
                msWrite2ByteMask(REG_SCDC_P0_0C_L, 0, BIT3);
            }
            break;

        case HDMI_DDC_1:
            msWrite2ByteMask(REG_SCDC_P1_02_L, 0x2000, 0x23FF); //[13]:enable write, [9:0]:Address
            msWrite2ByteMask(REG_SCDC_P1_02_L, BIT10, BIT10); //[10]:address pulse
            for(i = 0; i < 256; i++)
            {
                if(i == 1)
                    msWrite2ByteMask(REG_SCDC_P1_03_L, 0x0001, 0x00FF);//[7:0]: write data, SCDC version
                else
                    msWrite2ByteMask(REG_SCDC_P1_03_L, 0, 0x00FF);//[7:0]: write data, clear SCDC content
                msWrite2ByteMask(REG_SCDC_P1_03_L, BIT8, BIT8);//[8]: write pulse
                msRead2Byte(REG_SCDC_P1_05_L); //dummy read for delay
            }
            msWrite2ByteMask(REG_SCDC_P1_02_L, 0x0000, 0x23FF); //[13]:disable write, [9:0]:Address
            msWrite2ByteMask(REG_SCDC_P1_07_L, 0xFF00, 0xFF00); //IRQ CLR
            msWrite2ByteMask(REG_SCDC_P1_07_L, 0, 0xFF00);
            if(msRead2Byte(REG_SCDC_P1_0C_L) & BIT3)
            {
                msWrite2ByteMask(REG_SCDC_P1_0C_L, BIT2, BIT2); //5V_Reset_OV_value
                msWrite2ByteMask(REG_SCDC_P1_0C_L, 0, BIT2);
            }
            else
            {
                msWrite2ByteMask(REG_SCDC_P1_0C_L, BIT3, BIT3); //5V_Reset_OV_en
                msWrite2ByteMask(REG_SCDC_P1_0C_L, BIT2, BIT2); ///5V_Reset_OV_value
                msWrite2ByteMask(REG_SCDC_P1_0C_L, 0, BIT2);
                msWrite2ByteMask(REG_SCDC_P1_0C_L, 0, BIT3);
            }
            break;

        case HDMI_DDC_2:
            msWrite2ByteMask(REG_SCDC_P2_02_L, 0x2000, 0x23FF); //[13]:enable write, [9:0]:Address
            msWrite2ByteMask(REG_SCDC_P2_02_L, BIT10, BIT10); //[10]:address pulse
            for(i = 0; i < 256; i++)
            {
                if(i == 1)
                    msWrite2ByteMask(REG_SCDC_P2_03_L, 0x0001, 0x00FF);//[7:0]: write data, SCDC version
                else
                    msWrite2ByteMask(REG_SCDC_P2_03_L, 0, 0x00FF);//[7:0]: write data, clear SCDC content
                msWrite2ByteMask(REG_SCDC_P2_03_L, BIT8, BIT8);//[8]: write pulse
                msRead2Byte(REG_SCDC_P2_05_L); //dummy read for delay
            }
            msWrite2ByteMask(REG_SCDC_P0_02_L, 0x0000, 0x23FF); //[13]:disable write, [9:0]:Address
            msWrite2ByteMask(REG_SCDC_P2_07_L, 0xFF00, 0xFF00); //IRQ CLR
            msWrite2ByteMask(REG_SCDC_P2_07_L, 0, 0xFF00);
            if(msRead2Byte(REG_SCDC_P2_0C_L) & BIT3)
            {
                msWrite2ByteMask(REG_SCDC_P2_0C_L, BIT2, BIT2); //5V_Reset_OV_value
                msWrite2ByteMask(REG_SCDC_P2_0C_L, 0, BIT2);
            }
            else
            {
                msWrite2ByteMask(REG_SCDC_P2_0C_L, BIT3, BIT3); //5V_Reset_OV_en
                msWrite2ByteMask(REG_SCDC_P2_0C_L, BIT2, BIT2); ///5V_Reset_OV_value
                msWrite2ByteMask(REG_SCDC_P2_0C_L, 0, BIT2);
                msWrite2ByteMask(REG_SCDC_P2_0C_L, 0, BIT3);
            }
            break;

        default:
            break;
    }
}
#endif

#if ENABLE_HDMI_BCHErrorIRQ
//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_EnableHDMIRx_PKT_ParsrIRQ()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_EnableHDMIRx_PKT_ParsrIRQ(Bool bEnable)
{
    msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, bEnable?BIT(2) : 0, BIT(2)); //combo_gp_top_38: reg_combo_rx_gp_irq_enable; [2]:hdmirx_pkt_parser_top (hdmirx_packet_parser 1-2)
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_IRQMaskEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_IRQMaskEnable(HDMI_IRQ_Mask enIRQType, Bool bEnable)
{
    msWrite2ByteMask(REG_HDMI_U0_60_L, bEnable? 0x0 : enIRQType, enIRQType); //hdmi_u0_60: req_int_mask
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_CheckIRQMaskEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool Hal_HDMI_CheckIRQMaskEnable(HDMI_IRQ_Mask enIRQType)
{
    Bool bIRQMaskEnable = FALSE;

    if(!(msRead2Byte(REG_HDMI_U0_60_L) & enIRQType))//hdmi_u0_60: req_int_mask
    {
        bIRQMaskEnable = TRUE;
    }

    return bIRQMaskEnable;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_IRQClearStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_IRQClearStatus(HDMI_IRQ_Mask enIRQType)
{
    msWrite2ByteMask(REG_HDMI_U0_63_L, enIRQType, enIRQType); //hdmi_u0_63: req_int_clr
    msWrite2ByteMask(REG_HDMI_U0_63_L, 0, enIRQType); //hdmi_u0_63: req_int_clr
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_IRQCheckStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool Hal_HDMI_IRQCheckStatus(HDMI_IRQ_Mask enIRQType)
{
    Bool bIRQCheckStatus = FALSE;

    if(msRead2Byte(REG_HDMI_U0_61_L) & enIRQType)
    {
        bIRQCheckStatus = TRUE;
    }

    return bIRQCheckStatus;
}

#endif

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_PMEnableDVIDetect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_PMEnableDVIDetect(BYTE enInputPortSelect __attribute__ ((unused)), Bool bEnable, HDMI_PM_MODE_TYPE enPMModeType)
{
    if(enPMModeType == HDMI_PM_POWEROFF)
    {
        //clear DVI_RAW_CLK (reg_dvi_clk_det_0) to avoid DC case DVI_RAW_CLK event happend
        msWrite2ByteMask(REG_PM_4F_L, BIT(8), BIT(8));//[8]:Enable or disable the wakeup function of DVI clock active
        msWrite2ByteMask(REG_PM_47_L, 0x00, BMASK(3:0));//[3]:DVI CLK Gating[1:0]:DVI CLK Select
        ForceDelay1ms(5);
    }

    // connect to PM DVI0 only, polling by REG_0020C8 mux switching
    msWrite2ByteMask(REG_PM_47_L, bEnable ? 0x00:BMASK(3:0), BMASK(3:0));//[3]:DVI CLK Gating[1:0]:DVI CLK Select
    msWrite2ByteMask(REG_PM_47_L, bEnable ? 0x00:BMASK(11:8), BMASK(11:8));//[11]: DVI RAW CLK Gating[10]:DVI RAW CLK Invert.[9:8]:DVI RAW CLK Select
    msWrite2ByteMask(REG_PM_4F_L, bEnable ? BIT(8):0x00, BIT(8));//[8]: Enable or disable the wakeup function of DVI clock active
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetFRLRate()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _Hal_tmds_GetFRLRate(BYTE enInputPortType)
{
    DWORD u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    BYTE u8FRLRate = msRead2Byte(REG_PHY2P1_1_P0_57_L +u32PHY2P1BankOffset) &BMASK(3:0);

    enInputPortType = enInputPortType;

    if(u8FRLRate > HDMI_FRL_MODE_NONE)
    {
        u8FRLRate = HDMI_FRL_MODE_NONE;
    }
    else if(u8FRLRate == HDMI_FRL_MODE_LEGACY)
    {
        if(msRead2Byte(REG_PHY2P1_1_P0_57_L +u32PHY2P1BankOffset) &BIT(8))
        {
            u8FRLRate = HDMI_FRL_MODE_LEGACY_20;
        }
        else
        {
            u8FRLRate = HDMI_FRL_MODE_LEGACY_14;
        }
    }

    return u8FRLRate;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetClockStableFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_GetClockStableFlag(BYTE enInputPortType)
{
    Bool bStatusFlag = FALSE;
    DWORD u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(msRead2Byte(REG_PHY2P1_4_P0_5F_L +u32PHY2P1BankOffset) & BIT(3)) // phy2p1_4_5F[3]: reg_clk_stable_out
    {
        bStatusFlag = TRUE;
    }

    return bStatusFlag;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_tmds_GetClockRatePort()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DWORD Hal_tmds_GetClockRatePort(BYTE enInputPortType, BYTE u8SourceVersion, EN_HDMI_PIX_CLK_TYPE enType)
{
    BYTE u8_Count = 0;
    DWORD u32ClockCount = 0;
    DWORD u32ClockRateCal[10] = {0};
    DWORD u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;
    u8SourceVersion = u8SourceVersion;

    for(u8_Count = 0; u8_Count<10; u8_Count++)
    {
        u32ClockRateCal[u8_Count] = msRead2Byte(REG_PHY2P1_4_P0_74_L +u32PHY2P1BankOffset); // phy2p1_4_74[15:0]: reg_cr_done_cnt_hold
    }

    u32ClockCount = GetSortMiddleNumber(&u32ClockRateCal[0],10);
    if(enType == HDMI_SIGNAL_PIX_MHZ)
        u32ClockCount = u32ClockCount * HDMI_XTAL_CLOCK_MHZ / HDMI_XTAL_DIVIDER;
    else if(enType == HDMI_SIGNAL_PIX_10KHZ)
        u32ClockCount = u32ClockCount * HDMI_XTAL_CLOCK_10kHZ / HDMI_XTAL_DIVIDER;
    else // HDMI_SIGNAL_PIX_HZ not support
        u32ClockCount = 0;

    return u32ClockCount;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetRatioDetect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _Hal_tmds_GetRatioDetect(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    BYTE u8RatioDetect = HDMI_FRL_MODE_NONE;
    DWORD u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    DWORD ulClockCount = Hal_tmds_GetClockRatePort(enInputPortType, pstHDMIPollingInfo->ucSourceVersion, HDMI_SIGNAL_PIX_MHZ);

    enInputPortType = enInputPortType;

    if((ulClockCount > 150) || (ulClockCount < 85))
    {
        u8RatioDetect = HDMI_FRL_MODE_LEGACY_14;
    }
    //else if(msRead2Byte(REG_PHY2P1_1_P0_4A_L +u32PHY2P1BankOffset) &BIT(12))
    else if(msRead2Byte(REG_PHY2P1_1_P0_57_L +u32PHY2P1BankOffset) &BIT(8))
    {
        u8RatioDetect = HDMI_FRL_MODE_LEGACY_20;
    }

    return u8RatioDetect;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_GetSCDC5vDetectFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool Hal_HDMI_GetSCDC5vDetectFlag(BYTE enInputPortType)
{
    Bool bCableDetectFlag = FALSE;
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortType);

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
        #if HDMI_Cable5v_0
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P0_0C_L) & BIT(6)) ?TRUE: FALSE); // scdc_0C[6]: reg_tx_5v_reset_c
        #else
                bCableDetectFlag = TRUE;
        #endif
            break;

        case HDMI_DDC_1:
        #if HDMI_Cable5v_1
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P1_0C_L) & BIT(6)) ?TRUE: FALSE); // scdc_0C[6]: reg_tx_5v_reset_c
        #else
                bCableDetectFlag = TRUE;
        #endif
            break;

        case HDMI_DDC_2:
        #if HDMI_Cable5v_2
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P2_0C_L) & BIT(6)) ?TRUE: FALSE); // scdc_0C[6]: reg_tx_5v_reset_c
        #else
                bCableDetectFlag = TRUE;
        #endif
            break;

        default:
            break;
    }

    return bCableDetectFlag;
}

//**************************************************************************
//  [Function Name]:
//                  KHal_HDMIRx_GetSCDCValue()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_U8 KHal_HDMIRx_GetSCDCValue(MS_U8 enInputPortSelect, MS_U8 u8Offset)
{
    //TODO: PHY part
    MS_U8 u8XA8Data = 0;
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);
    MS_U32 u32PMSCDCBankOffset = 0;

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            u32PMSCDCBankOffset = 0;
        break;
        case HDMI_DDC_1:
            u32PMSCDCBankOffset = 0x100;
        break;
        case HDMI_DDC_2:
            u32PMSCDCBankOffset = 0x200;
        break;
        default:
            break;
    }

    // scdc_02[13]: Enable CPU write (not enable)
    msWriteByteMask(REG_SCDC_P0_02_H + u32PMSCDCBankOffset, 0x00, BIT5);

    // scdc_02[9:0]: SCDC CPU r/w address
    msWrite2ByteMask(REG_SCDC_P0_02_L + u32PMSCDCBankOffset, u8Offset, 0x3FF);

    // scdc_02[10]: SCDC address load pulse generate
    msWriteByteMask(REG_SCDC_P0_02_H + u32PMSCDCBankOffset, BIT2, BIT2);

    // wait read ready
    // scdc_04[9]: CPU read busy status
    while(msReadByte(REG_SCDC_P0_04_H + u32PMSCDCBankOffset) & BIT1);

    // scdc_04[7:0]: CPU read data port (for scdc)
    u8XA8Data = (MS_U8)msReadByte(REG_SCDC_P0_04_L + u32PMSCDCBankOffset);

    return u8XA8Data;
}

//**************************************************************************
//  [Function Name]:
//                  KHal_HDMIRx_SetSCDCValue()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void KHal_HDMIRx_SetSCDCValue(MS_U8 enInputPortSelect, MS_U8 u8Offset, MS_U8 u8SCDCValue)
{
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);
    MS_U32 u32PMSCDCBankOffset = 0;

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            u32PMSCDCBankOffset = 0;
        break;
        case HDMI_DDC_1:
            u32PMSCDCBankOffset = 0x100;
        break;
        case HDMI_DDC_2:
            u32PMSCDCBankOffset = 0x200;
        break;
        default:
            break;
    }

    // scdc_02[13]: Enable CPU write (not enable)
    msWriteByteMask(REG_SCDC_P0_02_H + u32PMSCDCBankOffset, BIT5, BIT5);

    // scdc_02[9:0]: SCDC CPU r/w address
    msWrite2ByteMask(REG_SCDC_P0_02_L + u32PMSCDCBankOffset, u8Offset, 0x3FF);

    // scdc_02[10]: SCDC address load pulse generate
    msWriteByteMask(REG_SCDC_P0_02_H + u32PMSCDCBankOffset, BIT2, BIT2);

    // scdc_03[7:0]: CPU write data port (for scdc)
    msWriteByte(REG_SCDC_P0_03_L + u32PMSCDCBankOffset, u8SCDCValue);

    // scdc_03[8]: SCDC data write port pulse generate
    msWriteByteMask(REG_SCDC_P0_03_H + u32PMSCDCBankOffset, BIT0, BIT0);

    // wait write ready
    // scdc_03[9]: CPU write busy status
    while(msReadByte(REG_SCDC_P0_03_H + u32PMSCDCBankOffset) & BIT1);

    // scdc_02[13]: Enable CPU write (not enable)
    msWriteByteMask(REG_SCDC_P0_02_H + u32PMSCDCBankOffset, 0x00, BIT5);
}

//**************************************************************************
//  [Function Name]:
//                  KHal_HDMIRx_CEDCheck()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void KHal_HDMIRx_CEDCheck(BYTE enInputPortType)
{
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortType);
    MS_U32 u32PMSCDCBankOffset = 0;

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            u32PMSCDCBankOffset = 0;
        break;
        case HDMI_DDC_1:
            u32PMSCDCBankOffset = 0x100;
        break;
        case HDMI_DDC_2:
            u32PMSCDCBankOffset = 0x200;
        break;
        default:
            break;
    }

    if(Hal_HDMI_GetSCDC5vDetectFlag(enInputPortType)) // 5V
    {
        if(_Hal_tmds_GetClockStableFlag(enInputPortType))
        {
            if((msReadByte(REG_SCDC_P0_0D_L + u32PMSCDCBankOffset) & BIT1)
                && (msReadByte(REG_SCDC_P0_0D_L + u32PMSCDCBankOffset) & BIT2))
            {
                msWriteByteMask(REG_SCDC_P0_0D_L + u32PMSCDCBankOffset, 0, (BIT1|BIT2)); //  scdc_0d[2:1]: error_cnt_ov
            }
        }
    }
    else // 5v drop
    {
        if(((msReadByte(REG_SCDC_P0_0D_L + u32PMSCDCBankOffset) & BIT1) == 0x00)
            && ((msReadByte(REG_SCDC_P0_0D_L + u32PMSCDCBankOffset) & BIT2) == 0x00))
        {
            msWriteByteMask(REG_SCDC_P0_0D_L + u32PMSCDCBankOffset, (BIT1|BIT2), (BIT1|BIT2)); //  scdc_05[0]: error_cnt_ov

            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x02, 0x00);
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x03, 0x00);
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x10, 0x00);
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x50, 0x00); // 0x50
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x51, 0x00); // 0x51
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x52, 0x00); // 0x52
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x53, 0x00); // 0x53
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x54, 0x00); // 0x54
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x55, 0x00); // 0x55
            KHal_HDMIRx_SetSCDCValue(enInputPortType, 0x56, 0x00); // 0x56
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_CheckTxDcBalanceEn()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_CheckTxDcBalanceEn(BYTE enInputPortType, Bool bEnable)
{
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(bEnable == TRUE) //turn on check TxDcBalance scheme
    {
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_52_L +u32DTOPBankOffset, 0 , BIT(7)); // hd21_dvi_dtop_dual_52[7]: reg_dis_dc_balance_count
    }
    else
    {
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_52_L +u32DTOPBankOffset, BIT(7) , BIT(7)); // hd21_dvi_dtop_dual_52[7]: reg_dis_dc_balance_count
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_SetPGAGainValue()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_SetPGAGainValue(BYTE enInputPortType, Bool bOvenFlag, BYTE *pu8PGAvalue)
{
    DWORD u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(bOvenFlag)
    {
        if(pu8PGAvalue != NULL)
        {
            msWrite2ByteMask(REG_PHY2P1_2_P0_12_L +u32PHY2P1BankOffset, BMASK(3:0), BMASK(3:0)); // phy2p1_2_12[0][1][2][3]: reg_pga_gain_l0/1/2/3_ov_en
            msWrite2Byte(REG_PHY2P1_2_P0_10_L +u32PHY2P1BankOffset, (pu8PGAvalue[1] << 8)|pu8PGAvalue[0]); // phy2p1_2_10[7:0]: reg_pga_gain_l0_ov; [15:8]: reg_pga_gain_l1_ov
            msWrite2Byte(REG_PHY2P1_2_P0_11_L +u32PHY2P1BankOffset, (pu8PGAvalue[3] << 8)|pu8PGAvalue[2]); // phy2p1_2_11[7:0]: reg_pga_gain_l2_ov; [15:8]: reg_pga_gain_l3_ov
        }
    }
    else
    {
        msWrite2ByteMask(REG_PHY2P1_2_P0_12_L +u32PHY2P1BankOffset, 0, BMASK(3:0)); // phy2p1_2_12[0][1][2][3]: reg_pga_gain_l0/1/2/3_ov_en
    }
}


//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_Toggle2XMode()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_Toggle2XMode(BYTE enInputPortType __attribute__ ((unused)))
{
    UINT u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    msWrite2ByteMask(REG_PHY2P1_4_P0_0A_L +u32PHY2P1BankOffset, 0x0500, BMASK(10:8)); // reg_hd14_gc_divsel_pll_l0_band_0
    msWrite2ByteMask(REG_PHY2P1_4_P0_0E_L +u32PHY2P1BankOffset, 0x0200, BMASK(10:8)); // reg_hd14_gc_divsel_post_l0_band_0

    ForceDelay1ms(1);

    msWrite2ByteMask(REG_PHY2P1_4_P0_0A_L +u32PHY2P1BankOffset, 0x0600, BMASK(10:8));
    msWrite2ByteMask(REG_PHY2P1_4_P0_0E_L +u32PHY2P1BankOffset, 0x0300, BMASK(10:8));
}
//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_BandDependSetting()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_BandDependSetting(BYTE enInputPortType)
{
    WORD u16BandInd, i;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    u16BandInd = (msRead2Byte(REG_PHY2P1_1_P0_37_L +u32PHY2P1BankOffset)& BMASK(10:0));
    for(i=0 ; i<HDMI_TMDS_CLOCK_NUMOF_BAND ; i++)
    {
        if((u16BandInd & (0x1<<i)) != 0)
        {
            break;
        }
    }

    switch(i)
    {
        case HDMI_TMDS_CLOCK_H14_BAND0:
            // 45+6.25%~25MHz
            // phy2_1a[3][1:0]=[1'b0][2'h0], phy2_1a[11][10:8]=[1'b0][3'h6], phy2_1a[15][14:12]=[1'b0][3'h3]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0600, 0x0F0B);

                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'hc], phy2_27[3][2:0]=[1'b0][3'h7]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x00c7);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b111]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000f, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b111]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0070, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h2], phy2_1[7][5:4]=[1'b0][2'h2], phy2_1[11][9:8]=[1'b0][2'h2], phy2_1[15][13:12]=[1'b0][2'h2]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0xAAAA);
            // phy2_6e[8][7]=[1'b1][1'b0], phy2_6e[3][2:0]=[1'b1][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0189, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h1], phy2_7f[7][6:4]=[1'b1][3'h1], phy2_7f[11][10:8]=[1'b1][3'h1], phy2_7f[15][14:12]=[1'b1][3'h1]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0x9999);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);
            //new add -- _TEST_EQ_CS
            msWrite2Byte(REG_DPRX_HDMI_60_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_61_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_62_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_63_L + u32PHY2P1BankOffset, 0x0080);

            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8038);


            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        case HDMI_TMDS_CLOCK_H14_BAND1:
            // 90+6.25%~45MHz
            // phy2_1a[3][1:0]=[1'b0][2'h0], phy2_1a[11][10:8]=[1'b0][3'h5], phy2_1a[15][14:12]=[1'b0][3'h2]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0500, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'h0], phy2_27[3][2:0]=[1'b0][3'h7]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x0007);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b111]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000f, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b111]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0070, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h2], phy2_1[7][5:4]=[1'b0][2'h2], phy2_1[11][9:8]=[1'b0][2'h2], phy2_1[15][13:12]=[1'b0][2'h2]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0xAAAA);
            // phy2_6e[8][7]=[1'b1][1'b0], phy2_6e[3][2:0]=[1'b1][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0189, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h1], phy2_7f[7][6:4]=[1'b1][3'h1], phy2_7f[11][10:8]=[1'b1][3'h1], phy2_7f[15][14:12]=[1'b1][3'h1]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0x9999);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            //new add -- _TEST_EQ_CS
            msWrite2Byte(REG_DPRX_HDMI_60_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_61_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_62_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_63_L + u32PHY2P1BankOffset, 0x0080);


            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8038);


            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));


            break;
        case HDMI_TMDS_CLOCK_H14_BAND2:
            // 140+6.25%~90MHz
            // phy2_1a[3][1:0]=[1'b0][2'h0], phy2_1a[11][10:8]=[1'b0][3'h4], phy2_1a[15][14:12]=[1'b0][3'h1]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0400, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'h0], phy2_27[3][2:0]=[1'b0][3'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b111]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000f, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b111]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0070, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h2], phy2_1[7][5:4]=[1'b0][2'h2], phy2_1[11][9:8]=[1'b0][2'h2], phy2_1[15][13:12]=[1'b0][2'h2]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0xAAAA);
            // phy2_6e[8][7]=[1'b1][1'b0], phy2_6e[3][2:0]=[1'b1][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0189, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h2], phy2_7f[7][6:4]=[1'b1][3'h2], phy2_7f[11][10:8]=[1'b1][3'h2], phy2_7f[15][14:12]=[1'b1][3'h2]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0xAAAA);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);


            //new add -- _TEST_EQ_CS
            msWrite2Byte(REG_DPRX_HDMI_60_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_61_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_62_L + u32PHY2P1BankOffset, 0x0080);
            msWrite2Byte(REG_DPRX_HDMI_63_L + u32PHY2P1BankOffset, 0x0080);


            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8038);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8038);


            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        case HDMI_TMDS_CLOCK_H14_BAND3:
             //3//.1Gbps ~ 1.4Gbps
            // phy2_1a[3][1:0]=[1'b0][2'h1], phy2_1a[11][10:8]=[1'b0][3'h5], phy2_1a[15][14:12]=[1'b0][3'h1]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0501, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'h0], phy2_27[3][2:0]=[1'b0][3'h7]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x0007);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b101]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000d, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b101]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0050, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h2], phy2_1[7][5:4]=[1'b0][2'h2], phy2_1[11][9:8]=[1'b0][2'h2], phy2_1[15][13:12]=[1'b0][2'h2]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0xAAAA);
            // phy2_6e[8][7]=[1'b0][1'b0], phy2_6e[3][2:0]=[1'b1][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0089, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h2], phy2_7f[7][6:4]=[1'b1][3'h2], phy2_7f[11][10:8]=[1'b1][3'h2], phy2_7f[15][14:12]=[1'b1][3'h2]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0xAAAA);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            //new add REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));


            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8A39);

            //new add -- _TEST_EQ_CS
            //U02 bringup add
            msWrite2Byte(REG_DPRX_HDMI_60_L + u32PHY2P1BankOffset, 0x0000);//back to default value
            msWrite2Byte(REG_DPRX_HDMI_61_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_62_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_63_L + u32PHY2P1BankOffset, 0x0000);


            break;
        case HDMI_TMDS_CLOCK_H14_BAND4:
            // 340+6.25%~310MHz
            // phy2_1a[3][1:0]=[1'b0][2'h1], phy2_1a[11][10:8]=[1'b0][3'h5], phy2_1a[15][14:12]=[1'b0][3'h1]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0501, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'h0], phy2_27[3][2:0]=[1'b0][3'h7]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x0007);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b100]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000c, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b100]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0040, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h1->2'h0], phy2_1[7][5:4]=[1'b0][2'h1->2'h0], phy2_1[11][9:8]=[1'b0][2'h1->2'h0], phy2_1[15][13:12]=[1'b0][2'h1->2'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x9999);
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x8888);
            // phy2_6e[8][7]=[1'b0][1'b0], phy2_6e[3][2:0]=[1'b1][2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0188, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h3], phy2_7f[7][6:4]=[1'b1][3'h3], phy2_7f[11][10:8]=[1'b1][3'h3], phy2_7f[15][14:12]=[1'b1][3'h3]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0x9999);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8A39);

            //U02 bringup add
            //new add -- _TEST_EQ_CS
            msWrite2Byte(REG_DPRX_HDMI_60_L + u32PHY2P1BankOffset, 0x0000);//back to default value
            msWrite2Byte(REG_DPRX_HDMI_61_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_62_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_63_L + u32PHY2P1BankOffset, 0x0000);


            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        case HDMI_TMDS_CLOCK_H14_BAND5:
            // 450+6.25%~340MHz
            // phy2_1a[3][1:0]=[1'b0][2'h2], phy2_1a[11][10:8]=[1'b0][3'h5], phy2_1a[15][14:12]=[1'b0][3'h0]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0502, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'hc], phy2_27[3][2:0]=[1'b0][3'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x00c0);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b100]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000c, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b100]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0040, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h1->2'h0], phy2_1[7][5:4]=[1'b0][2'h1->2'h0], phy2_1[11][9:8]=[1'b0][2'h1->2'h0], phy2_1[15][13:12]=[1'b0][2'h1->2'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x9999);
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x8888);
            // phy2_6e[8][7]=[1'b1][1'b0], phy2_6e[3][2:0]=[1'b1][2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0188, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h3], phy2_7f[7][6:4]=[1'b1][3'h3], phy2_7f[11][10:8]=[1'b1][3'h3], phy2_7f[15][14:12]=[1'b1][3'h3]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0xaaaa);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8A39);

            //U02 bringup add
            //new add -- _TEST_EQ_CS
            msWrite2Byte(REG_DPRX_HDMI_60_L + u32PHY2P1BankOffset, 0x0000);//back to default value
            msWrite2Byte(REG_DPRX_HDMI_61_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_62_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_63_L + u32PHY2P1BankOffset, 0x0000);


            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        case HDMI_TMDS_CLOCK_H14_BAND6:
            // 600~450MHz
            // phy2_1a[3][1:0]=[1'b0][2'h2], phy2_1a[11][10:8]=[1'b0][3'h5], phy2_1a[15][14:12]=[1'b0][3'h0]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0502, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'hc], phy2_27[3][2:0]=[1'b0][3'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x00c0);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b011]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000b, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b011]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0030, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h1->2'h0], phy2_1[7][5:4]=[1'b0][2'h1->2'h0], phy2_1[11][9:8]=[1'b0][2'h1->2'h0], phy2_1[15][13:12]=[1'b0][]2'h1->2'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x9999);
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x8888);
            // phy2_6e[8][7]=[1'b1][1'b0], phy2_6e[3][2:0]=[1'b1][2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0188, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[]2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h3], phy2_7f[7][6:4]=[1'b1][3'h3], phy2_7f[11][10:8]=[1'b1][3'h3], phy2_7f[15][14:12]=[1'b1][3'h3]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0xaaaa);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8A39);

            //U02 bringup add
            //new add -- _TEST_EQ_CS
            msWrite2Byte(REG_DPRX_HDMI_60_L + u32PHY2P1BankOffset, 0x0000);//back to default value
            msWrite2Byte(REG_DPRX_HDMI_61_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_62_L + u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_DPRX_HDMI_63_L + u32PHY2P1BankOffset, 0x0000);


            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        case HDMI_TMDS_CLOCK_H20_BAND0:
        case HDMI_TMDS_CLOCK_H20_BAND1: // 112.5+6.25%~90MHz
            // 90+6.25%~75MHz
            // phy2_1a[3][1:0]=[1'b0][2'h0], phy2_1a[11][10:8]=[1'b0][3'h6], phy2_1a[15][14:12]=[1'b0][3'h1]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0600, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'h3], phy2_27[3][2:0]=[1'b0][3'h7]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x0037);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b100]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000c, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b100]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0040, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h1->2'h0], phy2_1[7][5:4]=[1'b0][2'h1->2'h0], phy2_1[11][9:8]=[1'b0][2'h1->2'h0], phy2_1[15][13:12]=[1'b0][2'h1->2'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x9999);
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x8888);
            // phy2_6e[8][7]=[1'b0][1'b0], phy2_6e[3][2:0]=[1'b1][2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0188, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h3], phy2_7f[7][6:4]=[1'b1][3'h3], phy2_7f[11][10:8]=[1'b1][3'h3], phy2_7f[15][14:12]=[1'b1][3'h3]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0xaaaa);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8A39);

            //U02 bringup add
             //2020.Feb.4 need added form ANA team --START
             msWrite2ByteMask(REG_DPRX_HDMI_60_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:0));//BACK TO DEFAULT VALUE
             msWrite2ByteMask(REG_DPRX_HDMI_61_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:0));//BACK TO DEFAULT VALUE
             msWrite2ByteMask(REG_DPRX_HDMI_62_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:0));//BACK TO DEFAULT VALUE

             msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, 0x069C, BMASK(10:0));
             msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, 0x069C, BMASK(10:0));
             msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, 0x069C, BMASK(10:0));
            //end

            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        //case HDMI_TMDS_CLOCK_H20_BAND1: // 112.5+6.25%~90MHz
        case HDMI_TMDS_CLOCK_H20_BAND2: // 152.5+3.125%~112.5MHz
            // phy2_1a[3][1:0]=[1'b0][2'h0], phy2_1a[11][10:8]=[1'b0][3'h5], phy2_1a[15][14:12]=[1'b0][3'h0]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0500, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'hc], phy2_27[3][2:0]=[1'b0][3'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x00c0);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b011]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000b, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b011]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0030, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h1->2'h0], phy2_1[7][5:4]=[1'b0][2'h1->2'h0], phy2_1[11][9:8]=[1'b0][2'h1->2'h0], phy2_1[15][13:12]=[1'b0][2'h1->2'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x9999);
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x8888);
            // phy2_6e[8][7]=[1'b1][1'b0], phy2_6e[3][2:0]=[1'b1][2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0188, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h3], phy2_7f[7][6:4]=[1'b1][3'h3], phy2_7f[11][10:8]=[1'b1][3'h3], phy2_7f[15][14:12]=[1'b1][3'h3]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0xaaaa);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8A39);

            //U02 bringup add
            //2020.Feb.4 need added form ANA team --START
            msWrite2ByteMask(REG_DPRX_HDMI_60_L +u32PHY2P1BankOffset, 0x0080, BMASK(7:0));
            msWrite2ByteMask(REG_DPRX_HDMI_61_L +u32PHY2P1BankOffset, 0x00FF, BMASK(7:0));
            msWrite2ByteMask(REG_DPRX_HDMI_62_L +u32PHY2P1BankOffset, 0x00FF, BMASK(7:0));

            msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, 0x069C, BMASK(10:0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, 0x0597, BMASK(10:0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, 0x0597, BMASK(10:0));
           //end

           //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
           msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
           msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
           msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
           msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
           msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
           msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
           msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
           msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        case HDMI_TMDS_CLOCK_H20_BAND3:
            // 170~152.5MHz
            // phy2_1a[3][1:0]=[1'b0][2'h0], phy2_1a[11][10:8]=[1'b0][3'h5], phy2_1a[15][14:12]=[1'b0][3'h0]
            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset, 0x0500, 0x0F0B);
                // phy2_28[7][4:0]=[1'b0][5'h0]
                msWrite2Byte(REG_PHY2P1_2_P0_28_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_27[11][8:4]=[1'b0][5'h0], phy2_27[3][2:0]=[1'b0][3'h7]
            msWrite2Byte(REG_PHY2P1_2_P0_27_L + u32PHY2P1BankOffset, 0x0007);
            // phy2_2c[6][5:4]=[1'b0][2'b01]
            msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0010, BMASK(6:4));
            // phy2_6[3][2:0]=[1'b1][3'b010]
            msWrite2ByteMask(REG_PHY2P1_2_P0_06_L +u32PHY2P1BankOffset, 0x000a, BMASK(3:0));
            // phy2_26[7][6:4]=[1'b0][3'b010]
            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0x0020, BMASK(7:4));
            // phy2_0[3][2:0]=[1'b0][3'b0], phy2_0[7][6:4]=[1'b0][3'b0], phy2_0[11][10:8]=[1'b0][3'b0], phy2_0[15][14:12]=[1'b0][3'b0]
            msWrite2Byte(REG_PHY2P1_2_P0_00_L + u32PHY2P1BankOffset, 0x0000);
            // phy2_1[3][1:0]=[1'b0][2'h1->2'h0], phy2_1[7][5:4]=[1'b0][2'h1->2'h0], phy2_1[11][9:8]=[1'b0][2'h1->2'h0], phy2_1[15][13:12]=[1'b0][2'h1->2'h0]
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x9999);
            msWrite2Byte(REG_PHY2P1_2_P0_01_L + u32PHY2P1BankOffset, 0x8888);
            // phy2_6e[8][7]=[1'b1][1'b0], phy2_6e[3][2:0]=[1'b1][2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset, 0x0188, BMASK(8:7)|BMASK(3:0));
            // phy2_2[9:8]=[]2'b00]
            msWrite2ByteMask(REG_PHY2P1_2_P0_02_L +u32PHY2P1BankOffset, 0x0000, BMASK(9:8));
            // phy2_7f[3][2:0]=[1'b1][3'h3], phy2_7f[7][6:4]=[1'b1][3'h3], phy2_7f[11][10:8]=[1'b1][3'h3], phy2_7f[15][14:12]=[1'b1][3'h3]
            msWrite2Byte(REG_PHY2P1_2_P0_7F_L + u32PHY2P1BankOffset, 0xaaaa);
            // phy2_7e[3][2:0]=[1'b1][3'h5], phy2_7e[7][6:4]=[1'b1][3'h5], phy2_7e[11][10:8]=[1'b1][3'h5], phy2_7e[15][14:12]=[1'b1][3'h5]
            msWrite2Byte(REG_PHY2P1_2_P0_7E_L + u32PHY2P1BankOffset, 0xeeee);

            msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8A39);
            msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8A39);

            //U02 bringup add
             //2020.Feb.4 need added form ANA team --START
             msWrite2ByteMask(REG_DPRX_HDMI_60_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:0));//BACK TO DEFAULT VALUE
             msWrite2ByteMask(REG_DPRX_HDMI_61_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:0));//BACK TO DEFAULT VALUE
             msWrite2ByteMask(REG_DPRX_HDMI_62_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:0));//BACK TO DEFAULT VALUE

             msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, 0x069C, BMASK(10:0));
             msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, 0x069C, BMASK(10:0));
             msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, 0x069C, BMASK(10:0));
            //end

            //new add to reset REG_TEST_CH_L0/L1/L2/L3<39:0>
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0, BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(14));
            msWrite2ByteMask(REG_PHY2P1_2_P0_6B_L +u32PHY2P1BankOffset, 0, BIT(0));

            break;
        default:
            break;
    }


}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_EnableDfeTap()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_EnableDfeTap(BYTE enInputPortType, Bool bEnTap1to4, Bool bEnTap5to12)
{
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if (bEnTap1to4)
    {
        // release override tap1~tap4
        msWrite2Byte(REG_PHY2P1_2_P0_07_L +u32PHY2P1BankOffset, 0); // phy2p1_2_07[3:0]: reg_gc_dfe_idac_tap1_l3/2/10_ov_en; [7:4]: reg_gc_dfe_idac_tap2_l3/2/1/0_ov_en; [11:8]: reg_gc_dfe_idac_tap3_l3/2/1/0_ov_en; [15:12]: reg_gc_dfe_idac_tap4_l3/2/1/0_ov_en
    }
    else
    {
        // override dfe tap1~tap4 to 0
        msWrite2Byte(REG_PHY2P1_2_P0_07_L +u32PHY2P1BankOffset, 0xFFFF); // phy2p1_2_07[3:0]: reg_gc_dfe_idac_tap1_l3/2/10_ov_en; [7:4]: reg_gc_dfe_idac_tap2_l3/2/1/0_ov_en; [11:8]: reg_gc_dfe_idac_tap3_l3/2/1/0_ov_en; [15:12]: reg_gc_dfe_idac_tap4_l3/2/1/0_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_08_L +u32PHY2P1BankOffset, 0); // phy2p1_2_08[7:0]: reg_gc_dfe_idac_tap1_l0_ov; [15:8]: reg_gc_dfe_idac_tap1_l1_ov
        msWrite2Byte(REG_PHY2P1_2_P0_09_L +u32PHY2P1BankOffset, 0); // phy2p1_2_09[7:0]: reg_gc_dfe_idac_tap1_l2_ov; [15:8]: reg_gc_dfe_idac_tap1_l3_ov
        msWrite2Byte(REG_PHY2P1_2_P0_0A_L +u32PHY2P1BankOffset, 0); // phy2p1_2_0A[7:0]: reg_gc_dfe_idac_tap2_l0_ov; [15:8]: reg_gc_dfe_idac_tap2_l1_ov
        msWrite2Byte(REG_PHY2P1_2_P0_0B_L +u32PHY2P1BankOffset, 0); // phy2p1_2_0B[7:0]: reg_gc_dfe_idac_tap2_l2_ov; [15:8]: reg_gc_dfe_idac_tap2_l3_ov
        msWrite2Byte(REG_PHY2P1_2_P0_0C_L +u32PHY2P1BankOffset, 0); // phy2p1_2_0C[7:0]: reg_gc_dfe_idac_tap3_l0_ov; [15:8]: reg_gc_dfe_idac_tap3_l1_ov
        msWrite2Byte(REG_PHY2P1_2_P0_0D_L +u32PHY2P1BankOffset, 0); // phy2p1_2_0D[7:0]: reg_gc_dfe_idac_tap3_l2_ov; [15:8]: reg_gc_dfe_idac_tap3_l3_ov
        msWrite2Byte(REG_PHY2P1_2_P0_0E_L +u32PHY2P1BankOffset, 0); // phy2p1_2_0E[7:0]: reg_gc_dfe_idac_tap4_l0_ov; [15:8]: reg_gc_dfe_idac_tap4_l1_ov
        msWrite2Byte(REG_PHY2P1_2_P0_0F_L +u32PHY2P1BankOffset, 0); // phy2p1_2_0F[7:0]: reg_gc_dfe_idac_tap4_l2_ov; [15:8]: reg_gc_dfe_idac_tap4_l3_ov
    }

    if(bEnTap5to12)
    {
        // release override tap5~tap12
        msWrite2Byte(REG_PHY2P1_2_P0_30_L +u32PHY2P1BankOffset, 0); // phy2p1_2_30[5:0]: reg_gc_dfe_idac_tap5_l0_ov; [7]: reg_gc_dfe_idac_tap5_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap5_l1_ov; [15]: reg_gc_dfe_idac_tap5_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_31_L +u32PHY2P1BankOffset, 0); // phy2p1_2_31[5:0]: reg_gc_dfe_idac_tap5_l2_ov; [7]: reg_gc_dfe_idac_tap5_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap5_l3_ov; [15]: reg_gc_dfe_idac_tap5_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_32_L +u32PHY2P1BankOffset, 0); // phy2p1_2_32[5:0]: reg_gc_dfe_idac_tap6_l0_ov; [7]: reg_gc_dfe_idac_tap6_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap6_l1_ov; [15]: reg_gc_dfe_idac_tap6_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_33_L +u32PHY2P1BankOffset, 0); // phy2p1_2_33[5:0]: reg_gc_dfe_idac_tap6_l2_ov; [7]: reg_gc_dfe_idac_tap6_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap6_l3_ov; [15]: reg_gc_dfe_idac_tap6_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_34_L +u32PHY2P1BankOffset, 0); // phy2p1_2_34[5:0]: reg_gc_dfe_idac_tap7_l0_ov; [7]: reg_gc_dfe_idac_tap7_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap7_l1_ov; [15]: reg_gc_dfe_idac_tap7_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_35_L +u32PHY2P1BankOffset, 0); // phy2p1_2_35[5:0]: reg_gc_dfe_idac_tap7_l2_ov; [7]: reg_gc_dfe_idac_tap7_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap7_l3_ov; [15]: reg_gc_dfe_idac_tap7_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_36_L +u32PHY2P1BankOffset, 0); // phy2p1_2_36[5:0]: reg_gc_dfe_idac_tap8_l0_ov; [7]: reg_gc_dfe_idac_tap8_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap8_l1_ov; [15]: reg_gc_dfe_idac_tap8_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_37_L +u32PHY2P1BankOffset, 0); // phy2p1_2_37[5:0]: reg_gc_dfe_idac_tap8_l2_ov; [7]: reg_gc_dfe_idac_tap8_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap8_l3_ov; [15]: reg_gc_dfe_idac_tap8_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_38_L +u32PHY2P1BankOffset, 0); // phy2p1_2_38[5:0]: reg_gc_dfe_idac_tapf1_l0_ov; [7]: reg_gc_dfe_idac_tapf1_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf1_l1_ov; [15]: reg_gc_dfe_idac_tapf1_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_39_L +u32PHY2P1BankOffset, 0); // phy2p1_2_39[5:0]: reg_gc_dfe_idac_tapf1_l2_ov; [7]: reg_gc_dfe_idac_tapf1_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf1_l3_ov; [15]: reg_gc_dfe_idac_tapf1_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3A_L +u32PHY2P1BankOffset, 0); // phy2p1_2_3A[5:0]: reg_gc_dfe_idac_tapf2_l0_ov; [7]: reg_gc_dfe_idac_tapf2_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf2_l1_ov; [15]: reg_gc_dfe_idac_tapf2_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3B_L +u32PHY2P1BankOffset, 0); // phy2p1_2_3B[5:0]: reg_gc_dfe_idac_tapf2_l2_ov; [7]: reg_gc_dfe_idac_tapf2_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf2_l3_ov; [15]: reg_gc_dfe_idac_tapf2_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3C_L +u32PHY2P1BankOffset, 0); // phy2p1_2_3C[5:0]: reg_gc_dfe_idac_tapf3_l0_ov; [7]: reg_gc_dfe_idac_tapf3_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf3_l1_ov; [15]: reg_gc_dfe_idac_tapf3_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3D_L +u32PHY2P1BankOffset, 0); // phy2p1_2_3D[5:0]: reg_gc_dfe_idac_tapf3_l2_ov; [7]: reg_gc_dfe_idac_tapf3_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf3_l3_ov; [15]: reg_gc_dfe_idac_tapf3_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3E_L +u32PHY2P1BankOffset, 0); // phy2p1_2_3E[5:0]: reg_gc_dfe_idac_tapf4_l0_ov; [7]: reg_gc_dfe_idac_tapf4_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf4_l1_ov; [15]: reg_gc_dfe_idac_tapf4_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3F_L +u32PHY2P1BankOffset, 0); // phy2p1_2_3F[5:0]: reg_gc_dfe_idac_tapf4_l2_ov; [7]: reg_gc_dfe_idac_tapf4_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf4_l3_ov; [15]: reg_gc_dfe_idac_tapf4_l3_ov_en
    }
    else
    {
        // override dfe tap5~tap12 to 0
        msWrite2Byte(REG_PHY2P1_2_P0_30_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_30[5:0]: reg_gc_dfe_idac_tap5_l0_ov; [7]: reg_gc_dfe_idac_tap5_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap5_l1_ov; [15]: reg_gc_dfe_idac_tap5_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_31_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_31[5:0]: reg_gc_dfe_idac_tap5_l2_ov; [7]: reg_gc_dfe_idac_tap5_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap5_l3_ov; [15]: reg_gc_dfe_idac_tap5_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_32_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_32[5:0]: reg_gc_dfe_idac_tap6_l0_ov; [7]: reg_gc_dfe_idac_tap6_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap6_l1_ov; [15]: reg_gc_dfe_idac_tap6_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_33_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_33[5:0]: reg_gc_dfe_idac_tap6_l2_ov; [7]: reg_gc_dfe_idac_tap6_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap6_l3_ov; [15]: reg_gc_dfe_idac_tap6_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_34_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_34[5:0]: reg_gc_dfe_idac_tap7_l0_ov; [7]: reg_gc_dfe_idac_tap7_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap7_l1_ov; [15]: reg_gc_dfe_idac_tap7_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_35_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_35[5:0]: reg_gc_dfe_idac_tap7_l2_ov; [7]: reg_gc_dfe_idac_tap7_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap7_l3_ov; [15]: reg_gc_dfe_idac_tap7_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_36_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_36[5:0]: reg_gc_dfe_idac_tap8_l0_ov; [7]: reg_gc_dfe_idac_tap8_l0_ov_en; [13:8]: reg_gc_dfe_idac_tap8_l1_ov; [15]: reg_gc_dfe_idac_tap8_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_37_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_37[5:0]: reg_gc_dfe_idac_tap8_l2_ov; [7]: reg_gc_dfe_idac_tap8_l2_ov_en; [13:8]: reg_gc_dfe_idac_tap8_l3_ov; [15]: reg_gc_dfe_idac_tap8_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_38_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_38[5:0]: reg_gc_dfe_idac_tapf1_l0_ov; [7]: reg_gc_dfe_idac_tapf1_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf1_l1_ov; [15]: reg_gc_dfe_idac_tapf1_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_39_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_39[5:0]: reg_gc_dfe_idac_tapf1_l2_ov; [7]: reg_gc_dfe_idac_tapf1_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf1_l3_ov; [15]: reg_gc_dfe_idac_tapf1_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3A_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_3A[5:0]: reg_gc_dfe_idac_tapf2_l0_ov; [7]: reg_gc_dfe_idac_tapf2_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf2_l1_ov; [15]: reg_gc_dfe_idac_tapf2_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3B_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_3B[5:0]: reg_gc_dfe_idac_tapf2_l2_ov; [7]: reg_gc_dfe_idac_tapf2_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf2_l3_ov; [15]: reg_gc_dfe_idac_tapf2_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3C_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_3C[5:0]: reg_gc_dfe_idac_tapf3_l0_ov; [7]: reg_gc_dfe_idac_tapf3_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf3_l1_ov; [15]: reg_gc_dfe_idac_tapf3_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3D_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_3D[5:0]: reg_gc_dfe_idac_tapf3_l2_ov; [7]: reg_gc_dfe_idac_tapf3_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf3_l3_ov; [15]: reg_gc_dfe_idac_tapf3_l3_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3E_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_3E[5:0]: reg_gc_dfe_idac_tapf4_l0_ov; [7]: reg_gc_dfe_idac_tapf4_l0_ov_en; [13:8]: reg_gc_dfe_idac_tapf4_l1_ov; [15]: reg_gc_dfe_idac_tapf4_l1_ov_en
        msWrite2Byte(REG_PHY2P1_2_P0_3F_L +u32PHY2P1BankOffset, 0x8080); // phy2p1_2_3F[5:0]: reg_gc_dfe_idac_tapf4_l2_ov; [7]: reg_gc_dfe_idac_tapf4_l2_ov_en; [13:8]: reg_gc_dfe_idac_tapf4_l3_ov; [15]: reg_gc_dfe_idac_tapf4_l3_ov_en
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_SACalibration()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_SACalibration(BYTE enInputPortType)
{
    DWORD u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    volatile WORD u16temp = 0;

    enInputPortType = enInputPortType;

    // (A). Power down lane to reset previous SA calibration values
    //REG_PD_LANE_OV[3:0] = 0xf
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, BMASK(3:0), BMASK(3:0));
    //REG_PD_LANE_OV_EN = 1'b1
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, BIT(4), BIT(4));
    //REG_PD_CAL_SAFF_Lx=1'b1, set 1 to clear SA calibration
    msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset, 0x00FF, BMASK(7:0));

    //wait 1us
    u16temp = 300; // 200:1us
    while(u16temp--);

    //REG_PD_LANE_OV[3:0] = 0x0
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0, BMASK(3:0));
    //REG_PD_LANE_OV_EN= 1'b0
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0, BIT(4));
    //REG_PD_CAL_SAFF_Lx=1'b0
    msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset, 0x000F, BMASK(7:0));

    // (B). Start SA calibration
    //GC_CAL_START_SAFF_Lx_OV = 1'b1
    //GC_CAL_START_SAFF_Lx_OVE = 1'b1
    msWrite2ByteMask(REG_PHY2P1_2_P0_12_L +u32PHY2P1BankOffset, 0xFF00, BMASK(15:8));

    //wait  >10us, SA cal. pulse width >10u
    u16temp = 3000; // 2000:10us
    while(u16temp--);

    //GC_CAL_START_SAFF_Lx_OV = 1'b0
    //GC_CAL_START_SAFF_Lx_OVE = 1'b0
    msWrite2ByteMask(REG_PHY2P1_2_P0_12_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:8));
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_SetNewPHYEQValue()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_SetNewPHYEQValue(BYTE enInputPortType, Bool bOvenFlag, BYTE *pu8EQvalue)
{
    DWORD u32PHY2P1BankOffset = 0;  // _Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(bOvenFlag)
    {
        if(pu8EQvalue != NULL)
        {
            msWrite2ByteMask(REG_PHY2P1_2_P0_24_L +u32PHY2P1BankOffset, BIT(15)|(_Hal_tmds_EnCodeGray(pu8EQvalue[1]) << 8)|BIT(7)|_Hal_tmds_EnCodeGray(pu8EQvalue[0]), BIT(15)|BMASK(13:8)|BIT(7)|BMASK(5:0)); // phy2p1_2_24[5:0]: reg_gc_eq_l0_ov; [7]: reg_gc_eq_l0_ov_en; [13:8]: reg_gc_eq_l1_ov; [15]: reg_gc_eq_l1_ov_en
            msWrite2ByteMask(REG_PHY2P1_2_P0_25_L +u32PHY2P1BankOffset, BIT(15)|(_Hal_tmds_EnCodeGray(pu8EQvalue[3]) << 8)|BIT(7)|_Hal_tmds_EnCodeGray(pu8EQvalue[2]), BIT(15)|BMASK(13:8)|BIT(7)|BMASK(5:0)); // phy2p1_2_25[5:0]: reg_gc_eq_l2_ov; [7]: reg_gc_eq_l2_ov_en; [13:8]: reg_gc_eq_l3_ov; [15]: reg_gc_eq_l3_ov_en
        }
    }
    else
    {
        msWrite2ByteMask(REG_PHY2P1_2_P0_24_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(7)); // phy2p1_2_24[7]: reg_gc_eq_l0_ov_en; [15]: reg_gc_eq_l1_ov_en
        msWrite2ByteMask(REG_PHY2P1_2_P0_25_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(7)); // phy2p1_2_25[7]: reg_gc_eq_l2_ov_en; [15]: reg_gc_eq_l3_ov_en
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_NewPHYSwitch()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_NewPHYSwitch(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    BYTE u8PGAValue[4] = {0x0F, 0x0F, 0x0F, 0x0F};
    DWORD u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    msEread_Init(&usTemp1, &ubTemp2);
    _Hal_tmds_CheckTxDcBalanceEn(enInputPortType, FALSE);

    // release DFE overwrite
    msWrite2Byte(REG_PHY2P1_2_P0_01_L +u32PHY2P1BankOffset, 0); // phy2p1_2_01[15:0]: reg_gc_dfe_mode_l3/2/1/0_ov_en; reg_gc_dfe_mode_l3/2/1/0_ov

    _Hal_tmds_SetPGAGainValue(enInputPortType, FALSE, NULL);

    _Hal_tmds_BandDependSetting(enInputPortType);

    // Release VDAC
    msWrite2ByteMask(REG_PHY2P1_2_P0_14_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_15_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_16_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_17_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));

    // Release FSM
    msWrite2Byte(REG_PHY2P1_0_P0_10_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_11_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_12_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_13_L +u32PHY2P1BankOffset, 0);

    msWrite2ByteMask(REG_PHY2P1_0_P0_51_L +u32PHY2P1BankOffset, 0x0100, BMASK(10:8)|BIT(3)); // phy0_51[3]: reg_en_dlev_sw_mode; [10:8]: reg_filter_depth_dlev

    msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L +u32PHY2P1BankOffset, 0x0000, BIT(11)); // PHY2_2c[11]=1'h0 -> disable pixcko md overwrite

    switch(pstHDMIPollingInfo->ucSourceVersion)
    {
        case HDMI_SOURCE_VERSION_HDMI14:
            if((ubTemp2&0x03)==0)//Note. this is V1
            {
                msWrite2ByteMask(REG_PHY2P1_2_P0_74_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_76_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_78_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_7A_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
            }
            else
            {
                msWrite2ByteMask(REG_PHY2P1_2_P0_74_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_76_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_78_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_7A_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));
            }

            // disable dfe tap1~tap4 & disable dfe tap5~tap12
            _Hal_tmds_EnableDfeTap(enInputPortType, FALSE, FALSE);
            msWrite2ByteMask(REG_PHY2P1_0_P0_57_L +u32PHY2P1BankOffset, 0x0000, BMASK(11:0));

            msWrite2Byte(REG_PHY2P1_2_P0_1B_L +u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_PHY2P1_2_P0_1C_L +u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_PHY2P1_2_P0_1D_L +u32PHY2P1BankOffset, 0x0000);
            msWrite2Byte(REG_PHY2P1_2_P0_1E_L +u32PHY2P1BankOffset, 0x0000);

            msWrite2ByteMask(REG_PHY2P1_0_P0_5A_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:12));

            // [mt5871] restore - increase main current
            msWrite2Byte(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, 0x0000);
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0x0000, BMASK(7:1));
            msWrite2Byte(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, 0x0000);
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0x0000, BMASK(7:0));
            msWrite2Byte(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, 0x0000);
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0x0000, BMASK(7:0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_4C_L +u32PHY2P1BankOffset, 0x0000, BIT(11));
            msWrite2ByteMask(REG_PHY2P1_2_P0_4D_L +u32PHY2P1BankOffset, 0x0000, BIT(11));
            msWrite2ByteMask(REG_PHY2P1_2_P0_4E_L +u32PHY2P1BankOffset, 0x0000, BIT(11));

            // restore - increase DFE main current
            msWrite2ByteMask(REG_PHY2P1_2_P0_50_L +u32PHY2P1BankOffset, 0x0000, BIT(15));
            msWrite2ByteMask(REG_PHY2P1_2_P0_51_L +u32PHY2P1BankOffset, 0x0000, 0xE00F);
            msWrite2ByteMask(REG_PHY2P1_2_P0_54_L +u32PHY2P1BankOffset, 0x0000, BIT(15));
            msWrite2ByteMask(REG_PHY2P1_2_P0_55_L +u32PHY2P1BankOffset, 0x0000, 0xE00F);
            msWrite2ByteMask(REG_PHY2P1_2_P0_58_L +u32PHY2P1BankOffset, 0x0000, BIT(15));
            msWrite2ByteMask(REG_PHY2P1_2_P0_59_L +u32PHY2P1BankOffset, 0x0000, 0xE00F);

            // restore - decrease DFE step
            msWrite2ByteMask(REG_PHY2P1_0_P0_53_L + u32PHY2P1BankOffset, 0x8000, BIT(15));

            // set agc dLev target = 200mV
            //msWrite2ByteMask(REG_PHY2P1_0_P0_5D_L +u32PHY2P1BankOffset, 0x3200, BMASK(15:8));

            // set EQ code to N-type EQ
            msWrite2Byte(REG_PHY2P1_2_P0_49_L +u32PHY2P1BankOffset, 0x9999);

            //U02 modify
            // power down EQ calibration for MIK
            //msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0x1800, BMASK(12:11));
            //msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0x1800, BMASK(12:11));
            //msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0x1800, BMASK(12:11));

            // [mt5871] restore - reduce PGA-RL
            msWrite2ByteMask(REG_PHY2P1_2_P0_05_L + u32PHY2P1BankOffset, 0x0000, BMASK(15:12));
            msWrite2ByteMask(REG_PHY2P1_2_P0_13_L + u32PHY2P1BankOffset, 0x0000, BMASK(11:0));

            msWrite2ByteMask(REG_PHY2P1_2_P0_73_L + u32PHY2P1BankOffset, 0x0000, BIT(5)); // -> restore - increase vring cmp threshold

            // under threshold = 0x1/0x3FFF
            msWrite2ByteMask(REG_PHY2P1_0_P0_23_L +u32PHY2P1BankOffset, 0x1, BMASK(9:0)); // phy2p1_0_23[9:0]: reg_eq_sth_chg_th
            msWrite2Byte(REG_PHY2P1_0_P0_24_L +u32PHY2P1BankOffset, 0x3FFF); // phy2p1_0_24[15:0]: reg_pat_pass2_num

            // short EQ-RS override disable, EQ-RS1 override disable
            msWrite2ByteMask(REG_PHY2P1_3_P0_0C_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0C[3]: short EQ-RS_L0 OVE; [8]: EQ-RS1_L0 OVE; [15]: reg_gc_eqrs_l0_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0D_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0D[3]: short EQ-RS_L1 OVE; [8]: EQ-RS1_L1 OVE; [15]: reg_gc_eqrs_l1_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0E_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0E[3]: short EQ-RS_L2 OVE; [8]: EQ-RS1_L2 OVE; [15]: reg_gc_eqrs_l2_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0F_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0F[3]: short EQ-RS_L3 OVE; [8]: EQ-RS1_L3 OVE; [15]: reg_gc_eqrs_l3_ov_en

            // search each aaba timeout
            msWrite2Byte(REG_PHY2P1_0_P0_1E_L +u32PHY2P1BankOffset, 0xFFFF); // phy2p1_0_1E[15:0]: reg_eq_sth_det_duration

            msWrite2Byte(REG_PHY2P1_0_P0_1D_L +u32PHY2P1BankOffset, 0x0028); // phy0_1d[15:0] reg_eq_chg_sattle_time

            //decrease EQ/PGA INEGC (short cable setting)
#if 1 // R3 fine EQ
            msWrite2ByteMask(REG_PHY2P1_5_P0_51_L +u32PHY2P1BankOffset, 0x0100, BMASK(15:8)); // phy5_51[9:8] reg_aaba_parse_num_l0=1(R3+), phy5_51[11:10] reg_aaba_parse_num_l1=0, phy5_51[13:12] reg_aaba_parse_num_l2=0, phy5_51[15:14] reg_aaba_parse_num_l3=0
            msWrite2Byte(REG_PHY2P1_5_P0_4F_L +u32PHY2P1BankOffset, 0x0A1E);// phy5_4f[5:0] reg_coa_eq_up_bond, phy5_4f[13:8] reg_coa_eq_lw_bond

            msWrite2ByteMask(REG_PHY2P1_1_P0_20_L + u32PHY2P1BankOffset, 15, BMASK(9:0)); // phy1_20[9:0] reg_eq_sth_chg_th_a_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_21_L + u32PHY2P1BankOffset, 15, BMASK(9:0)); // phy1_21[9:0] reg_eq_sth_chg_th_b_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_22_L + u32PHY2P1BankOffset, 15, BMASK(9:0)); // phy1_22[9:0] reg_eq_sth_chg_th_c_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_23_L + u32PHY2P1BankOffset, 15, BMASK(9:0)); // phy1_23[9:0] reg_eq_sth_chg_th_d_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_24_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_24[9:0] reg_eq_sth_chg_th_a_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_25_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_25[9:0] reg_eq_sth_chg_th_b_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_26_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_26[9:0] reg_eq_sth_chg_th_c_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_27_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_27[9:0] reg_eq_sth_chg_th_d_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_28_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_28[9:0] reg_eq_sth_chg_th_a_l2
            msWrite2ByteMask(REG_PHY2P1_1_P0_29_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_29[9:0] reg_eq_sth_chg_th_b_l2
            msWrite2ByteMask(REG_PHY2P1_1_P0_2A_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_2a[9:0] reg_eq_sth_chg_th_c_l2
            msWrite2ByteMask(REG_PHY2P1_1_P0_2B_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_2b[9:0] reg_eq_sth_chg_th_d_l2

            msWrite2Byte(REG_PHY2P1_1_P0_08_L + u32PHY2P1BankOffset, 300); // phy1_08[15:0] reg_pat_pass2_num_a_l0
            msWrite2Byte(REG_PHY2P1_1_P0_09_L + u32PHY2P1BankOffset, 300); // phy1_09[15:0] reg_pat_pass2_num_b_l0
            msWrite2Byte(REG_PHY2P1_1_P0_0A_L + u32PHY2P1BankOffset, 300); // phy1_0a[15:0] reg_pat_pass2_num_c_l0
            msWrite2Byte(REG_PHY2P1_1_P0_0B_L + u32PHY2P1BankOffset, 300); // phy1_0b[15:0] reg_pat_pass2_num_d_l0
            msWrite2Byte(REG_PHY2P1_1_P0_0C_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0c[15:0] reg_pat_pass2_num_a_l1
            msWrite2Byte(REG_PHY2P1_1_P0_0D_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0d[15:0] reg_pat_pass2_num_b_l1
            msWrite2Byte(REG_PHY2P1_1_P0_0E_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0e[15:0] reg_pat_pass2_num_c_l1
            msWrite2Byte(REG_PHY2P1_1_P0_0F_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0f[15:0] reg_pat_pass2_num_d_l1
            msWrite2Byte(REG_PHY2P1_1_P0_10_L + u32PHY2P1BankOffset, 0x03FF); // phy1_10[15:0] reg_pat_pass2_num_a_l2
            msWrite2Byte(REG_PHY2P1_1_P0_11_L + u32PHY2P1BankOffset, 0x03FF); // phy1_11[15:0] reg_pat_pass2_num_b_l2
            msWrite2Byte(REG_PHY2P1_1_P0_12_L + u32PHY2P1BankOffset, 0x03FF); // phy1_12[15:0] reg_pat_pass2_num_c_l2
            msWrite2Byte(REG_PHY2P1_1_P0_13_L + u32PHY2P1BankOffset, 0x03FF); // phy1_13[15:0] reg_pat_pass2_num_d_l2

            msWrite2ByteMask(REG_PHY2P1_0_P0_25_L +u32PHY2P1BankOffset, 0x1000, BMASK(13:8)); // phy0_25[13:8] reg_sth_eq_seq_vld

            msWrite2ByteMask(REG_PHY2P1_0_P0_2A_L + u32PHY2P1BankOffset, 0x003F, BMASK(7:0)); // phy0_2a[7:0] reg_under_over_max_time
            msWrite2ByteMask(REG_PHY2P1_0_P0_26_L + u32PHY2P1BankOffset, 0x0002, BMASK(5:0)); // phy0_26[5:0] reg_sth_eq_diff_th

            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L0 Ineg ove
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L1 Ineg ove
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L2 Ineg ove
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L3 Ineg ove
#endif

            // under thershold = 3, upper thershold = 30
            msWrite2ByteMask(REG_PHY2P1_0_P0_21_L +u32PHY2P1BankOffset, 0x1E03, BMASK(15:8)| BMASK(5:0)); // phy2p1_0_21[13:8]: reg_eq_up_bond; [5:0]: reg_eq_lw_bond

            // aaba reference to blanking
            msWrite2Byte(REG_PHY2P1_0_P0_27_L +u32PHY2P1BankOffset, 0x9300); // phy0_27[9:8]=3 -> both blanking and video, default=1

            // EQ fine tune symbol continue time value in hdmi1.4 mode
            msWrite2Byte(REG_PHY2P1_3_P0_7C_L +u32PHY2P1BankOffset, 0x500);

            // select eq output type: average
            msWrite2ByteMask(REG_PHY2P1_0_P0_5A_L +u32PHY2P1BankOffset, BIT(10), BMASK(11:10)); // phy2p1_0_5A[11:10]: reg_eq_output_sel

            _Hal_tmds_SetPGAGainValue(enInputPortType, TRUE, u8PGAValue);

            _Hal_tmds_SACalibration(enInputPortType);

            break;

        case HDMI_SOURCE_VERSION_HDMI20:
            msWrite2ByteMask(REG_PHY2P1_2_P0_74_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_76_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_78_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_7A_L +u32PHY2P1BankOffset, 0x0000, BIT(3)|BIT(0));


            _Hal_tmds_EnableDfeTap(enInputPortType, TRUE, TRUE);

            if ((msRead2Byte(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset) & 0x7000) == 0x0000) // half rate
            {
                msWrite2ByteMask(REG_PHY2P1_0_P0_57_L +u32PHY2P1BankOffset, 0x0003, BMASK(11:0));//DFE 2 taps
                msWrite2Byte(REG_PHY2P1_2_P0_1B_L +u32PHY2P1BankOffset, 0x1FFF);
                msWrite2Byte(REG_PHY2P1_2_P0_1C_L +u32PHY2P1BankOffset, 0x1FFF);
                msWrite2Byte(REG_PHY2P1_2_P0_1D_L +u32PHY2P1BankOffset, 0x1FFF);
                msWrite2Byte(REG_PHY2P1_2_P0_1E_L +u32PHY2P1BankOffset, 0x1FFF);
                msWrite2ByteMask(REG_PHY2P1_0_P0_5A_L +u32PHY2P1BankOffset, 0x3000, BMASK(15:12));
            }
            else
            {
                msWrite2ByteMask(REG_PHY2P1_0_P0_57_L +u32PHY2P1BankOffset, 0x0000, BMASK(11:0));
                msWrite2Byte(REG_PHY2P1_2_P0_1B_L +u32PHY2P1BankOffset, 0x0000);
                msWrite2Byte(REG_PHY2P1_2_P0_1C_L +u32PHY2P1BankOffset, 0x0000);
                msWrite2Byte(REG_PHY2P1_2_P0_1D_L +u32PHY2P1BankOffset, 0x0000);
                msWrite2Byte(REG_PHY2P1_2_P0_1E_L +u32PHY2P1BankOffset, 0x0000);
                msWrite2ByteMask(REG_PHY2P1_0_P0_5A_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:12));
            }
            //U02 bringup add
            // [mt5871] increase EQ-Imain
            //msWrite2Byte(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, 0x069C);//transfer to _Hal_tmds_BandDependSetting
            msWrite2ByteMask(REG_PHY2P1_2_P0_62_L +u32PHY2P1BankOffset, 0x0030, BMASK(7:0));
            //msWrite2Byte(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, 0x069C);//transfer to _Hal_tmds_BandDependSetting
            msWrite2ByteMask(REG_PHY2P1_2_P0_65_L +u32PHY2P1BankOffset, 0x0030, BMASK(7:0));
            //msWrite2Byte(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, 0x069C);//transfer to _Hal_tmds_BandDependSetting
            msWrite2ByteMask(REG_PHY2P1_2_P0_68_L +u32PHY2P1BankOffset, 0x0030, BMASK(7:0));
            msWrite2ByteMask(REG_PHY2P1_2_P0_4C_L +u32PHY2P1BankOffset, 0x0800, BIT(11));
            msWrite2ByteMask(REG_PHY2P1_2_P0_4D_L +u32PHY2P1BankOffset, 0x0800, BIT(11));
            msWrite2ByteMask(REG_PHY2P1_2_P0_4E_L +u32PHY2P1BankOffset, 0x0800, BIT(11));

            // increase DFE main current
            msWrite2ByteMask(REG_PHY2P1_2_P0_50_L +u32PHY2P1BankOffset, 0x8000, BIT(15));
            msWrite2ByteMask(REG_PHY2P1_2_P0_51_L +u32PHY2P1BankOffset, 0xE00F, 0xE00F);
            msWrite2ByteMask(REG_PHY2P1_2_P0_54_L +u32PHY2P1BankOffset, 0x8000, BIT(15));
            msWrite2ByteMask(REG_PHY2P1_2_P0_55_L +u32PHY2P1BankOffset, 0xE00F, 0xE00F);
            msWrite2ByteMask(REG_PHY2P1_2_P0_58_L +u32PHY2P1BankOffset, 0x8000, BIT(15));
            msWrite2ByteMask(REG_PHY2P1_2_P0_59_L +u32PHY2P1BankOffset, 0xE00F, 0xE00F);

            // decrease DFE step
            msWrite2ByteMask(REG_PHY2P1_0_P0_53_L + u32PHY2P1BankOffset, 0x0000, BIT(15));

            // [mt5871] set agc dLev target = 156mV
            //msWrite2ByteMask(REG_PHY2P1_0_P0_5D_L +u32PHY2P1BankOffset, 0x2700, BMASK(15:8));

            // restore - set EQ code to N-type EQ
            msWrite2Byte(REG_PHY2P1_2_P0_49_L +u32PHY2P1BankOffset, 0x0000);

            // restore - power down EQ calibration for MIK
            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0x0000, BMASK(12:11));
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0x0000, BMASK(12:11));
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0x0000, BMASK(12:11));

            // [mt5871] reduce PGA-RL
            msWrite2ByteMask(REG_PHY2P1_2_P0_05_L + u32PHY2P1BankOffset, 0xF000, BMASK(15:12));
            msWrite2ByteMask(REG_PHY2P1_2_P0_13_L + u32PHY2P1BankOffset, 0x0DDD, BMASK(11:0));

            /*if (MDrv_SYS_GetChipRev() == 0)
            {
                msWrite2ByteMask(REG_PHY2P1_2_P0_73_L + u32PHY2P1BankOffset, 0x0020, BIT(5)); // -> increase vring cmp threshold
            }
            else
            {
                msWrite2ByteMask(REG_PHY2P1_2_P0_73_L + u32PHY2P1BankOffset, 0x0000, BIT(5)); // keep default vring cmp threshold
            }*/
            msWrite2ByteMask(REG_PHY2P1_2_P0_73_L + u32PHY2P1BankOffset, 0x0000, BIT(5)); // keep default vring cmp threshold


            // under threshold = 0xFF/0x3FF
            msWrite2ByteMask(REG_PHY2P1_0_P0_23_L +u32PHY2P1BankOffset, 0xFF, BMASK(9:0)); // phy2p1_0_23[9:0]: reg_eq_sth_chg_th
            msWrite2Byte(REG_PHY2P1_0_P0_24_L +u32PHY2P1BankOffset, 0x03FF); // phy2p1_0_24[15:0]: reg_pat_pass2_num

            // short EQ-RS override disable, EQ-RS1 override disable
            msWrite2ByteMask(REG_PHY2P1_3_P0_0C_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0C[3]: short EQ-RS_L0 OVE; [8]: EQ-RS1_L0 OVE; [15]: reg_gc_eqrs_l0_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0D_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0D[3]: short EQ-RS_L1 OVE; [8]: EQ-RS1_L1 OVE; [15]: reg_gc_eqrs_l1_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0E_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0E[3]: short EQ-RS_L2 OVE; [8]: EQ-RS1_L2 OVE; [15]: reg_gc_eqrs_l2_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0F_L +u32PHY2P1BankOffset, 0, BIT(15)|BIT(8)|BMASK(7:4)|BIT(3)|BMASK(2:0)); // phy2p1_3_0F[3]: short EQ-RS_L3 OVE; [8]: EQ-RS1_L3 OVE; [15]: reg_gc_eqrs_l3_ov_en

            // search each aaba timeout
            msWrite2Byte(REG_PHY2P1_0_P0_1E_L +u32PHY2P1BankOffset, 0xA); // phy2p1_0_1E[15:0]: reg_eq_sth_det_duration

            msWrite2Byte(REG_PHY2P1_0_P0_1D_L +u32PHY2P1BankOffset, 0x0008); // phy0_1d[15:0] reg_eq_chg_sattle_time

            msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(0)); // L0 Ineg ove
            msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(0)); // L1 Ineg ove
            msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(0)); // L2 Ineg ove
            msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(0)); // L3 Ineg ove

            // under thershold = 5, upper thershold = 36
            msWrite2ByteMask(REG_PHY2P1_0_P0_21_L +u32PHY2P1BankOffset, 0x2405, BMASK(15:8)| BMASK(5:0)); // phy2p1_0_21[13:8]: reg_eq_up_bond; [5:0]: reg_eq_lw_bond

            // aaba reference to full screen
            msWrite2Byte(REG_PHY2P1_0_P0_27_L +u32PHY2P1BankOffset, 0x9300); // phy2p1_0_27[15:0]: reg_eq_top_condi

            // EQ fine tune symbol continue time value in hdmi1.4 mode
            msWrite2Byte(REG_PHY2P1_3_P0_7C_L +u32PHY2P1BankOffset, 0x350);

            // select eq output type: average
            msWrite2ByteMask(REG_PHY2P1_0_P0_5A_L +u32PHY2P1BankOffset, BIT(10), BMASK(11:10)); // phy2p1_0_5A[11:10]: reg_eq_output_sel


            _Hal_tmds_SetPGAGainValue(enInputPortType, FALSE, NULL);


            _Hal_tmds_SACalibration(enInputPortType);

            msWrite2ByteMask(REG_PHY2P1_5_P0_51_L +u32PHY2P1BankOffset, 0x0000, BMASK(15:8));  // phy5_51[9:8] reg_aaba_parse_num_l0=0, phy5_51[11:10] reg_aaba_parse_num_l1=0, phy5_51[13:12] reg_aaba_parse_num_l2=0, phy5_51[15:14] reg_aaba_parse_num_l3=0
            msWrite2Byte(REG_PHY2P1_5_P0_4F_L +u32PHY2P1BankOffset, 0x0024);// phy5_4f[5:0] reg_coa_eq_up_bond, phy5_4f[13:8] reg_coa_eq_lw_bond

            msWrite2ByteMask(REG_PHY2P1_1_P0_20_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_20[9:0] reg_eq_sth_chg_th_a_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_21_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_21[9:0] reg_eq_sth_chg_th_b_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_22_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_22[9:0] reg_eq_sth_chg_th_c_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_23_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_23[9:0] reg_eq_sth_chg_th_d_l0
            msWrite2ByteMask(REG_PHY2P1_1_P0_24_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_24[9:0] reg_eq_sth_chg_th_a_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_25_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_25[9:0] reg_eq_sth_chg_th_b_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_26_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_26[9:0] reg_eq_sth_chg_th_c_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_27_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_27[9:0] reg_eq_sth_chg_th_d_l1
            msWrite2ByteMask(REG_PHY2P1_1_P0_28_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_28[9:0] reg_eq_sth_chg_th_a_l2
            msWrite2ByteMask(REG_PHY2P1_1_P0_29_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_29[9:0] reg_eq_sth_chg_th_b_l2
            msWrite2ByteMask(REG_PHY2P1_1_P0_2A_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_2a[9:0] reg_eq_sth_chg_th_c_l2
            msWrite2ByteMask(REG_PHY2P1_1_P0_2B_L + u32PHY2P1BankOffset, 0x00FF, BMASK(9:0)); // phy1_2b[9:0] reg_eq_sth_chg_th_d_l2

            msWrite2Byte(REG_PHY2P1_1_P0_08_L + u32PHY2P1BankOffset, 0x03FF); // phy1_08[15:0] reg_pat_pass2_num_a_l0
            msWrite2Byte(REG_PHY2P1_1_P0_09_L + u32PHY2P1BankOffset, 0x03FF); // phy1_09[15:0] reg_pat_pass2_num_b_l0
            msWrite2Byte(REG_PHY2P1_1_P0_0A_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0a[15:0] reg_pat_pass2_num_c_l0
            msWrite2Byte(REG_PHY2P1_1_P0_0B_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0b[15:0] reg_pat_pass2_num_d_l0
            msWrite2Byte(REG_PHY2P1_1_P0_0C_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0c[15:0] reg_pat_pass2_num_a_l1
            msWrite2Byte(REG_PHY2P1_1_P0_0D_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0d[15:0] reg_pat_pass2_num_b_l1
            msWrite2Byte(REG_PHY2P1_1_P0_0E_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0e[15:0] reg_pat_pass2_num_c_l1
            msWrite2Byte(REG_PHY2P1_1_P0_0F_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0f[15:0] reg_pat_pass2_num_d_l1
            msWrite2Byte(REG_PHY2P1_1_P0_10_L + u32PHY2P1BankOffset, 0x03FF); // phy1_10[15:0] reg_pat_pass2_num_a_l2
            msWrite2Byte(REG_PHY2P1_1_P0_11_L + u32PHY2P1BankOffset, 0x03FF); // phy1_11[15:0] reg_pat_pass2_num_b_l2
            msWrite2Byte(REG_PHY2P1_1_P0_12_L + u32PHY2P1BankOffset, 0x03FF); // phy1_12[15:0] reg_pat_pass2_num_c_l2
            msWrite2Byte(REG_PHY2P1_1_P0_13_L + u32PHY2P1BankOffset, 0x03FF); // phy1_13[15:0] reg_pat_pass2_num_d_l2

            msWrite2ByteMask(REG_PHY2P1_0_P0_25_L +u32PHY2P1BankOffset, 0x0A00, BMASK(13:8)); // phy0_25[13:8] reg_sth_eq_seq_vld

            msWrite2ByteMask(REG_PHY2P1_0_P0_2A_L + u32PHY2P1BankOffset, 0x002F, BMASK(7:0)); // phy0_2a[7:0] reg_under_over_max_time
            msWrite2ByteMask(REG_PHY2P1_0_P0_26_L + u32PHY2P1BankOffset, 0x0003, BMASK(5:0)); // phy0_26[5:0] reg_sth_eq_diff_th

            break;

        default:

            break;
    };

    _Hal_tmds_SetNewPHYEQValue(enInputPortType, FALSE, NULL);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2SetTxData()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_tmds_HDCP2SetDataInfo(MSCombo_TMDS_PORT_INDEX enInputPort, BYTE MessageID, WORD DataSize, BYTE *pHDCPTxData, BYTE *pHDCPTxData_CertRx)
{
#define DEF_MSG_ID_CHK 1
#define DEF_DDC_BUSY_TIMEOUT_CNT 20000

    WORD i = 0;
    WORD u16RdOffset = 130;
    WORD wOffset;
    DWORD u32NopCnt = 0;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);

    if (mhal_tmds_HDCP2CheckWriteStart(enInputPort) == TRUE)
    {
        switch (enInputPort)
        {
            case MSCombo_TMDS_PORT_0:
            case MSCombo_TMDS_PORT_1:
            {
                while ((msReadByte(REG_HDMIRX_HDCP_P0_66_L) & BIT2) == 0); //wait until write done; to avoid data corruption
            }
            break;
            default:
            {
            }
            break;
        }

    }

    msWriteByteMask(REG_COMBO_GP_TOP_40_L, 0x05, 0x07); //enable xiu write

    switch(enInputPort)
    {
        case MSCombo_TMDS_PORT_0:

            RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;  // xiu to memory access address: 04

            if(MessageID == TMDS_HDCP2_MSG_ID_AKE_SEND_CERT)
            {
                msWriteByte(REG_HDCPKEY_05_L, MessageID);  // xiu to memory wirte data: 05

#if (DEF_MSG_ID_CHK == 1)
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;

                    while (msReadByte(REG_HDCPKEY_0E_L) != MessageID)
                    {
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                        msWriteByte(REG_HDCPKEY_05_L, MessageID);
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                    }
                }
#endif

                for ( i = 0; i < (522 + DataSize); i++ )
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                    if (i < 522)
                    {
                        msWriteByte(REG_HDCPKEY_05_L, msMemReadByte((ULONG)pHDCPTxData_CertRx + i));
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                        while (msReadByte(REG_HDCPKEY_0E_L) != msMemReadByte((ULONG)pHDCPTxData_CertRx + i))
                        {
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                            msWriteByte(REG_HDCPKEY_05_L, msMemReadByte((ULONG)pHDCPTxData_CertRx + i));
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                        }
                    }
                    else
                    {
                        msWriteByte(REG_HDCPKEY_05_L, *(BYTE*)(pHDCPTxData + i - 522));
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                        while (msReadByte(REG_HDCPKEY_0E_L) != *(BYTE*)(pHDCPTxData + i - 522))
                        {
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                            msWriteByte(REG_HDCPKEY_05_L, *(BYTE*)(pHDCPTxData + i - 522));
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                        }
                    }
                }

                msWrite2Byte(REG_HDMIRX_HDCP_P0_63_L + wOffset, 0x216);
                do
                {
                    //MAsm_CPU_Nop();
                    _nop_();
                    u32NopCnt++;
                }
                while ((u32NopCnt < DEF_DDC_BUSY_TIMEOUT_CNT) && (msReadByte(REG_HDMIRX_HDCP_P0_19_H + wOffset) & BIT2));   // hdcp_dual_19[10]: ddc busy
                msWriteByteMask(REG_HDMIRX_HDCP_P0_64_H + wOffset, 0x80, 0x80); // mdssage data length update pulse (sink)
            }
            else
            {
                msWriteByte(REG_HDCPKEY_05_L, MessageID);  // xiu to memory wirte data: 09

#if (DEF_MSG_ID_CHK == 1)
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;

                    while (msReadByte(REG_HDCPKEY_0E_L) != MessageID)
                    {
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                        msWriteByte(REG_HDCPKEY_05_L, MessageID);
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                    }
                }
#endif

                for(i = 0; i< DataSize; i++)
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address
                    msWriteByte(REG_HDCPKEY_05_L, *(BYTE*)(pHDCPTxData + i));
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                    while (msReadByte(REG_HDCPKEY_0E_L) != *(BYTE*)(pHDCPTxData + i))
                    {
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                        msWriteByte(REG_HDCPKEY_05_L, *(BYTE*)(pHDCPTxData + i));
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                    }
                }

                msWrite2Byte(REG_HDMIRX_HDCP_P0_63_L + wOffset, 1 + DataSize);  //  message datalength wrote by sink: 63 (1 + DataSize)
                msWriteByteMask(REG_HDMIRX_HDCP_P0_64_H + wOffset, 0x80, 0x80); // mdssage data length update pulse (sink)

            }

            break;

        default:
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2SetTxData()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_tmds_HDCP2SetTxData(MSCombo_TMDS_PORT_INDEX enInputPort, BYTE MessageID, WORD DataSize, BYTE *pHDCPTxData, BYTE *pHDCPTxData_CertRx)
{
    mhal_tmds_HDCP2SetDataInfo(enInputPort, MessageID, DataSize, pHDCPTxData, pHDCPTxData_CertRx);
}

#if(COMBO_HDCP2_FUNCTION_SUPPORT)
#if 0
//**************************************************************************
//  [Function Name]:
//                  mhal_combo_HDCP2Initial()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_combo_HDCP2Initial(void)
{
    BYTE i = 0;
    #if ((defined(DEF_COMBO_HDCP2RX_ISR_MODE) && (DEF_COMBO_HDCP2RX_ISR_MODE == 1)))
    WORD u16BankOffset[3] = {0x000, 0x500, 0xC00};
    #endif

	msWriteByteMask(0x163062, 0x00, 0xFF); //enable clk
	msWriteByteMask(0x163080, 0x05, 0x07); //enable xiu

    #if (COMBO_HDCP2_HDMI20PORT_EN == 1)
    	//tmp: wilson test; write 74ram for HDCP22 version
    	msWriteByteMask(REG_16102F, BIT2, BIT2); //HDCP enable for DDC
    	msWriteByteMask(REG_161033, BIT7, MASKBIT(7:6)); //write enable


    	msWrite2ByteMask(REG_16102E, 0x50, MASKBIT(9:0)); //CPU r/w address (for hdcp_key_sram/74reg)
    	msWriteByteMask(REG_161032, BIT5, BIT5); //HDCP address load pulse generate
    	msWriteByte(REG_161030, 0x04); //bit2: HDCP22 version
    	msWriteByteMask(REG_161032, BIT4, BIT4); // REG_COMBO_DVI_HDCP_DUAL_19[4]: HDCP data write port pulse generate

    	//clear RxStatus wirte msg length
    	RIU[REG_1610C6] = 0x00;
    	msWriteByteMask(REG_1610C9, BIT7, BIT7);
    #endif

    #if (COMBO_HDCP2_HDMI14PORT_EN == 1)
        //for 1.4 port
        for ( i = 0; i < 2; i++ )
        {
            //write 74ram for HDCP22 version
        	msWriteByteMask(REG_16042F + i*0x500, BIT2, BIT2); //HDCP enable for DDC
        	msWriteByteMask(REG_160433 + i*0x500, BIT7, MASKBIT(7:6)); //write enable

        	msWrite2ByteMask(REG_16042E + i*0x500, 0x50, MASKBIT(9:0)); //CPU r/w address (for hdcp_key_sram/74reg)
        	msWriteByteMask(REG_160432 + i*0x500, BIT5, BIT5); //HDCP address load pulse generate
        	msWriteByte(REG_160430 + i*0x500, 0x04); //bit2: HDCP22 version
        	msWriteByteMask(REG_160432 + i*0x500, BIT4, BIT4); // REG_COMBO_DVI_HDCP_DUAL_19[4]: HDCP data write port pulse generate

        	//clear RxStatus wirte msg length
        	RIU[REG_1604C6 + i*0x500] = 0x00;
            msWriteByteMask(REG_1604C9 + i*0x500, BIT7, BIT7);
        }
    #endif

    #if ((defined(DEF_COMBO_HDCP2RX_ISR_MODE) && (DEF_COMBO_HDCP2RX_ISR_MODE == 1)))
    //turn on mask
    for ( i = 0; i < 3; i++)
    {
        msWriteByte(REG_1604CE + u16BankOffset[i], msReadByte(REG_1604CE + u16BankOffset[i]) & (~BIT2));
        msWriteByteMask(REG_160277, 0x00, BIT6);
    }
    #if (COMBO_HDCP2_HDMI14PORT_EN == 1)
	msWriteByteMask(REG_163070, (msReadByte(REG_163070) | (BIT0 | BIT2 | BIT4)), (BIT0 | BIT2 | BIT4));
    #else
    msWriteByteMask(REG_163070, (msReadByte(REG_163070) | BIT4), BIT4);
    #endif
    #endif

}
#endif

//**************************************************************************
//  [Function Name]:
//                  mhal_combo_HDCP2GetReadDownFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_combo_HDCP2GetReadDownFlag(BYTE ucPortSelect)
{
    Bool bReadDownFlag = FALSE;

	ucPortSelect = 0;
	bReadDownFlag = FALSE;
	return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2Initial()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_tmds_HDCP2Initial(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    //WORD usDTOPBankOffset = 0;//_mhal_tmds_phy_offset(enInputPort);
    UNUSED(enInputPort);
    msWrite2ByteMask(REG_COMBO_GP_TOP_40_L, 0x05, 0x07); // [2]: Enable CPU write; [1]: disable SRAM read; [0]: enable xiu2hdcpkey

    //wirte 74Ram for HDCP22 version
    //msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L , BIT(10), BIT(10)); //HDCP enalbe for DDC
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L , BIT(15), BMASK(15:14)); //write enable

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L , HDMI_HDCP_HDCP2_VERSION, BMASK(9:0)); //CPU r/w address
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L , BIT(5), BIT(5)); //HDCP load address pulse

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_18_L , BIT(2), BMASK(7:0)); //bit2: HDCP22 version
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L , BIT(4), BIT(4)); //HDCP data write port pulse generate

    //clear RxStatus
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_63_L , 0x00, BMASK(9:0));
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_64_L , BIT(15), BIT(15));

    //clear hdcp_enable cipher bit
    //W2BYTEMSK(REG_HDCP_DUAL_P0_4E_L + dwBKOffset, 0, BIT(0));

    //clear xiu status
    msWrite2Byte(REG_HDMIRX_HDCP_P0_66_L , 0x3D);
    msWrite2Byte(REG_HDMIRX_HDCP_P0_66_L , 0x00);

    //Enable write offset
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_68_L , BIT(15)| TMDS_HDCP2_SOURCE_READ_OFFSET, BIT(15)| BMASK(9:0)); //b'[0..9]offset; b'[15]: enable bit
    //Enable MSG length fully writen in
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_5A_L , BIT(2)|BIT(4), BIT(2)|BIT(4)); //bit2:timeout_mask  bit4:msg_length_update_sel

}

//**************************************************************************
//  [Function Name]:
//                  mhal_hdmiRx_HDCP2InterruptEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_tmds_HDCP2InterruptEnable(MSCombo_TMDS_PORT_INDEX enInputPort, Bool bEnableIRQ)
{
    WORD usDTOPBankOffset = 0;//[MT9700]_mhal_tmds_phy_offset(enInputPort);
    UNUSED(enInputPort);
    msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, 1, BIT(0));//[MT9700] msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, 0, BIT(0));
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_3B_L +usDTOPBankOffset, bEnableIRQ? 0: BIT(14), BIT(14));
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_15_L +usDTOPBankOffset, bEnableIRQ? 0: BIT(15), BIT(15));
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_67_L +usDTOPBankOffset, bEnableIRQ? 0: BIT(3)| BIT(2), BIT(3)| BIT(2));
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2CheckRomCodeResult()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_tmds_HDCP2CheckRomCodeResult(void)
{
    #define HDCP22_MBX_START_ADDR 0x103300
    BYTE i = 0;

    for(i = 0 ; i < 10 ; i++)
    {
        if((msRead2Byte(HDCP22_MBX_START_ADDR + 0x0) == 0xE018) &&
        (msRead2Byte(HDCP22_MBX_START_ADDR + 0x74) == 0x4F4B))
        {
            return TRUE;
        }
        else
        {
            ForceDelay1ms(1);
        }
    }
    return false;
}

#endif

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP_IRQ_Clear()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_tmds_HDCP_IRQ_Clear(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    UNUSED(enInputPort);
    WORD wTemp;

    if(msReadByte(REG_HDMIRX_DTOP_P0_3B_L) & BIT6)
    {
        wTemp = msRead2Byte(REG_HDMIRX_DTOP_P0_3B_L);
        msWriteByte(REG_HDMIRX_DTOP_P0_3D_L, 0x01); //[0] irq clr select
        msWrite2Byte(REG_HDMIRX_DTOP_P0_3B_L, wTemp); //clear hdcp irq
        msWriteByte(REG_HDMIRX_DTOP_P0_3D_L, 0x00); //[0] irq clr select
    }
}


//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2WriteDone()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_tmds_HDCP2CheckWriteDone(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    WORD wOffset;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);
    enInputPort=enInputPort;
    //if((enInputPort == MSCombo_TMDS_PORT_0) || (enInputPort == MSCombo_TMDS_PORT_1))
    {
        if(msReadByte(REG_HDMIRX_HDCP_P0_66_L) & BIT2)     // IRQ status: 66
        {
            msWriteByte(REG_HDMIRX_HDCP_P0_66_L + wOffset, 0x04); //clear wirte done status
            msWriteByte(REG_HDMIRX_HDCP_P0_66_L + wOffset, 0x10); //clear write start status

            msWrite2Byte(REG_HDMIRX_DTOP_P0_3C_L + wOffset, 0x4000); //clear dtop w/r done status
            msWrite2Byte(REG_HDMIRX_DTOP_P0_3C_L + wOffset, 0x0000); //clear dtop w/r done status

            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    //else
        //return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2ReadDone()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_tmds_HDCP2ReadDone(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    WORD wOffset;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);

    if((enInputPort == MSCombo_TMDS_PORT_0) || (enInputPort == MSCombo_TMDS_PORT_1))
    {
        if(msReadByte(REG_HDMIRX_HDCP_P0_66_L) & BIT3)     // IRQ status: 66
        {
            msWriteByte(REG_HDMIRX_HDCP_P0_66_L, 0x08); //clear read done status

            msWrite2Byte(REG_HDMIRX_DTOP_P0_3C_L + wOffset, 0x4000); //clear dtop w/r done status
            msWrite2Byte(REG_HDMIRX_DTOP_P0_3C_L + wOffset, 0x0000); //clear dtop w/r done status

            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
        return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2GetDataInfo()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_tmds_HDCP2GetDataInfo(MSCombo_TMDS_PORT_INDEX enInputPort, WORD ucDataSize, BYTE *pHDCPRxData)
{
    WORD i = 0;

    msWriteByteMask(REG_COMBO_GP_TOP_40_L, 0x03, 0x07); //enable XIU read
    switch(enInputPort)
    {
        case MSCombo_TMDS_PORT_0:
            RIU[REG_HDCPKEY_04_L] = 0x0000; // xiu to memory access address: 04
            #if 1
            for ( i = 0; i < ucDataSize; i++ )
            {
                pHDCPRxData[i] = msReadByte(REG_HDCPKEY_0E_L); // memory to xiu read data (port0): 0E
            }
            #endif
            break;
        default:
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2GetRxData()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_tmds_HDCP2GetRxData(MSCombo_TMDS_PORT_INDEX enInputPort, BYTE *pHDCPRxData)
{
    WORD wOffset;
    wOffset = 0;//[MT9700]_mhal_tmds_phy_offset(enInputPort);

    //if ((enInputPort == MSCombo_TMDS_PORT_0) || (enInputPort == MSCombo_TMDS_PORT_1))
        mhal_tmds_HDCP2GetDataInfo(enInputPort, msRead2Byte(REG_HDMIRX_HDCP_P0_64_L + wOffset) & 0x03FF, pHDCPRxData);  // message length receive: 64
    //else
    //{
    //    TMDS_PRINT("Port: 0x%x, no need\r\n", enInputPort);
    //}
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDCP2WriteStart()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_tmds_HDCP2CheckWriteStart(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    WORD wOffset;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);
    UNUSED(enInputPort);
    UNUSED(wOffset);
    // write IRQ start
    //if((enInputPort == MSCombo_TMDS_PORT_0) || (enInputPort == MSCombo_TMDS_PORT_1))
    {
        if(msReadByte(REG_HDMIRX_HDCP_P0_66_L) & BIT4)     // IRQ status: 66
        {
            msWriteByte(REG_HDMIRX_HDCP_P0_66_L, 0x10);    // IRQ write start clear: 66
            msWriteByte(REG_HDMIRX_HDCP_P0_66_L, 0x00);

            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    //else
    //    return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDMIGetErrorStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
// [8]: Checksum Error occurred; packet discarded
// [9]: Unsupported Packet received
// [13]: BCH parity Error occurred; packet discarded
// [15]: Single bit BCH parity Error occurred and Corrected
//**************************************************************************
Bool mhal_tmds_HDMIGetErrorStatus(BYTE enInputPortType)
{
    Bool bErrorStatus = FALSE;
    enInputPortType = enInputPortType;
    if(msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_10_L) & BIT(13) )
    {
        msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_10_L, BIT(13), BIT(13));
        bErrorStatus = true;
    }

    return bErrorStatus;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDMIGetErrorStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//[13]: BCH parity Error occurred; packet discarded
//[14]: bch_error delayed
//[15]: Single bit BCH parity Error occurred and Corrected

//**************************************************************************
Bool mhal_tmds_HDMIGetBCHErrorStatus(BYTE enInputPortType)
{

    enInputPortType = enInputPortType;
    if(msReadByte(REG_HDMIRX_DTOP_PKT_P0_10_H) & (BIT5|BIT6|BIT7) ){

        msWriteByteMask(REG_HDMIRX_DTOP_PKT_P0_10_H, (BIT5|BIT6|BIT7), (BIT5|BIT6|BIT7));
        return true;
    }
    return false;
}

BYTE mhal_hdmiRx_CheckHDCPState(void)
{
    enum HDMI_HDCP_STATE enHDCPState = HDMI_HDCP_NO_ENCRYPTION;

    if(msRead2Byte(REG_HDMIRX_HDCP_P0_1B_L) & BIT(9)) // encryption enable
    {
        if(msReadByte(REG_HDMIRX_HDCP_P0_4F_L) & 0x01) // SKE successful
        {
            enHDCPState = HDMI_HDCP_2_2;
        }
        else
        {
            enHDCPState = HDMI_HDCP_1_4;
        }
    }
    return enHDCPState;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetClockChangeFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_GetClockChangeFlag(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    Bool bClockBigChangeFlag = FALSE;
    WORD u16ClockCount = 0;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    u16ClockCount = Hal_tmds_GetClockRatePort(enInputPortType, pstHDMIPollingInfo->ucSourceVersion, HDMI_SIGNAL_PIX_MHZ);

    if(msRead2Byte(REG_PHY2P1_4_P0_5F_L +u32PHY2P1BankOffset) & BIT(2)) // phy2p1_4_5F[2]: reg_clk_big_chg_sts
    {
        msWrite2ByteMask(REG_PHY2P1_4_P0_5F_L +u32PHY2P1BankOffset, BIT(2), BIT(2)); // phy2p1_4_5F[2]: reg_clk_big_chg_sts

        bClockBigChangeFlag = TRUE;
    }

    if((BYTE)(ABS_MINUS(pstHDMIPollingInfo->usClockCount, u16ClockCount)&0xff) > HDMI_CLOCK_UNSTABLE_OFFSET)
    {
        pstHDMIPollingInfo->usClockCount = u16ClockCount;

        bClockBigChangeFlag = TRUE;
    }

    return bClockBigChangeFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetSquelchFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_GetSquelchFlag(BYTE enInputPortType)
{
    Bool bStatusFlag = FALSE;
    //DWORD u32PHY2P1PMBankOffset = 0;//_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
    WORD temp =0;

    msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L , (enInputPortType&0x03) , BMASK(15:14));//reg_data_debounce_out_l_sel = 2'h0
    temp = msRead2Byte(REG_DPRX_PHY_PM_6D_L);

    if( enInputPortType == 0  && ((temp&0x000F)!= 0))
        bStatusFlag = TRUE;
    else if (enInputPortType == 1  && ((temp&0x00F0)!= 0))
        bStatusFlag = TRUE;
    else if (enInputPortType == 2  && ((temp&0x0F00)!= 0))
        bStatusFlag = TRUE;

    return bStatusFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_ResetDLPF()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_ResetDLPF(BYTE enInputPortType __attribute__ ((unused)))
{
    UINT u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    volatile WORD dly;

    msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset, 0x00F1, BMASK(6:4)|BIT(0)); // pd_dlpf
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0F00, BMASK(11:8)); // pd_phdac_i
    for (dly=0 ; dly<200 ; dly++); // 1us delay
    msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset, 0x0001, BMASK(6:4)|BIT(0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0000, BMASK(11:8));
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetH14AutoEQEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
EN_HDMI_14_TMDS_CLOCK_RANGE _Hal_tmds_GetH14AutoEQEnable(BYTE enInputPortType, DWORD ulClockRate)
{
    EN_HDMI_14_TMDS_CLOCK_RANGE enClockRange = HDMI_14_TMDS_CLOCK_NONE;
    DWORD u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(ulClockRate < 75)
    {
        BYTE u8EQValue[4] = {10, 10, 10, 10};

        // aaba reference to full screen
        msWrite2Byte(REG_PHY2P1_0_P0_27_L +u32PHY2P1BankOffset, 0x9300); // phy2p1_0_27[15:0]: reg_eq_top_condi
        _Hal_tmds_ResetDLPF(enInputPortType);

        _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

        enClockRange = HDMI_14_TMDS_CLOCK_UNDER_75M;
    }
    else if(ulClockRate < 135)
    {
        BYTE u8EQValue[4] = {15, 15, 15, 15};

        // aaba reference to full screen
        msWrite2Byte(REG_PHY2P1_0_P0_27_L +u32PHY2P1BankOffset, 0x9300); // phy2p1_0_27[15:0]: reg_eq_top_condi
        _Hal_tmds_ResetDLPF(enInputPortType);

        _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

        enClockRange = HDMI_14_TMDS_CLOCK_75M_TO_135M;
    }
    else // Auto EQ
    {
        enClockRange = HDMI_14_TMDS_CLOCK_OVER_135M;
    }

    return enClockRange;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_ActiveCablePatch()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_ActiveCablePatch(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    BYTE u8EQValue[4] = {2, 2, 2, 2};
    BYTE u8PGAValue[4] = {0x0F, 0x0F, 0x0F, 0x0F}; // eg_pga_gain_lx_ov[7:6] = 2'b11 for H20 patch
    WORD u16temp = 1000;
    DWORD u32PHY2P1BankOffset =0;// _Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, 0, BIT(7));//2     // 26 bit[7] reg_gc_eq_bw_ov_en

    _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue); // Set EQ = 2

    _Hal_tmds_SetPGAGainValue(enInputPortType, TRUE, u8PGAValue); // Set PGA gain = 0xF

    // Clear auto scan done flag
    msWrite2Byte(REG_PHY2P1_0_P0_3E_L +u32PHY2P1BankOffset, BIT(2));
    msWrite2Byte(REG_PHY2P1_0_P0_42_L +u32PHY2P1BankOffset, BIT(2));
    msWrite2Byte(REG_PHY2P1_0_P0_46_L +u32PHY2P1BankOffset, BIT(2));

    // Overwrite IDAC = 0 (disable dfe tap1~tap4 & disable dfe tap5~tap12)
    _Hal_tmds_EnableDfeTap(enInputPortType, FALSE, FALSE);

    // Release VDAC
    msWrite2ByteMask(REG_PHY2P1_2_P0_14_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_15_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_16_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_17_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));

    if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
    {
        msWrite2ByteMask(REG_PHY2P1_0_P0_51_L +u32PHY2P1BankOffset, BIT(9)|BIT(3), BMASK(10:8)|BIT(3)); // phy0_51[3]: reg_en_dlev_sw_mode; [10:8]: reg_filter_depth_dlev

        while(u16temp--);
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_ReleaseActiveCablePatch()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_ReleaseActiveCablePatch(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    BYTE u8PGAValue[4] = {0xCF, 0xCF, 0xCF, 0xCF}; // eg_pga_gain_lx_ov[7:6] = 2'b11 for H20 patch

    enInputPortType = enInputPortType;

    // Normal cable
    _Hal_tmds_SetNewPHYEQValue(enInputPortType, FALSE, NULL);  // Auto EQ

    if((pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20) && ((msRead2Byte(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset) & 0x7000) == 0x0000))
    {
        // Release IDAC (Enable DFE Taps)
        _Hal_tmds_EnableDfeTap(enInputPortType, TRUE, TRUE);


        _Hal_tmds_SetPGAGainValue(enInputPortType, FALSE, NULL); // PGA gain not enable

    }
    else
    {
        _Hal_tmds_SetPGAGainValue(enInputPortType, TRUE, u8PGAValue); // Set PGA gain = 0xF
    }

    // Release VDAC
    msWrite2ByteMask(REG_PHY2P1_2_P0_14_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_15_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_16_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_17_L +u32PHY2P1BankOffset, 0, BIT(15)|BMASK(8:0));

    // Release FSM
    msWrite2Byte(REG_PHY2P1_0_P0_10_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_11_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_12_L +u32PHY2P1BankOffset, 0);
    msWrite2Byte(REG_PHY2P1_0_P0_13_L +u32PHY2P1BankOffset, 0);

    HDMI_HAL_DPUTSTR("** HDMI change to normal cable \r\n");
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_CheckActiveCableProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_CheckActiveCableProc(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    Bool bCheckDoneFlag = FALSE;
    Bool bNextStateFlag = FALSE;
    WORD u16temp = 2000;
    stHDMI_ERROR_STATUS stErrorStatus = {0,0,0,0,0,0,0,0};
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    do
    {
        switch(pstHDMIPollingInfo->u8ActiveCableState)
        {
            case HDMI_ACTIVE_CABLE_START:
                _Hal_tmds_ActiveCablePatch(enInputPortType, pstHDMIPollingInfo);

                if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
                {
                    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_CHECK_DLEV;
                    bNextStateFlag = TRUE;
                }
                else if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20)
                {
                    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_CHECK_DLEV;
                    bNextStateFlag = TRUE;
                }
                else
                {
                    bNextStateFlag = FALSE;
                }

                break;

            case HDMI_ACTIVE_CABLE_CHECK_DLEV:
                if(_Hal_tmds_ActiveCableCheckDLEV(enInputPortType, pstHDMIPollingInfo))
                {
                    //since the SQUELCH will not disable when signal re-enter
                    //do the PDLF reset
                    _Hal_tmds_ResetDLPF(enInputPortType);

                    //msWrite2ByteMask(REG_PHY2P1_2_P0_05_L + u32PHY2P1BankOffset, BMASK(7:4), BMASK(7:4));

                    //msWrite2ByteMask(REG_PHY2P1_2_P0_05_L + u32PHY2P1BankOffset, 0x0000, BMASK(7:4));

                    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_CHECK_PHASE;
                    bNextStateFlag = TRUE;
                }
                else
                {
                    bNextStateFlag = FALSE;
                }
                break;

            case HDMI_ACTIVE_CABLE_CHECK_PHASE:
                if(_Hal_tmds_ActiveCableCheckPhase(enInputPortType, pstHDMIPollingInfo))
                {
                    if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
                    {
                        pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_EQ_FLOW_H14;
                    }
                    else if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20)
                    {
                        pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_EQ_FLOW_H20;
                    }

                    pstHDMIPollingInfo->u16ActiveCableCheckErrorCount = 0;
                    pstHDMIPollingInfo->u8ActiveCableClearErrorStatusCount = 0;
                    bNextStateFlag = FALSE;
                }
                else
                {
                    pstHDMIPollingInfo->bActiveCableFlag = FALSE;
                    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_FINISH;
                    bNextStateFlag = TRUE;
                }
                break;

            case HDMI_ACTIVE_CABLE_EQ_FLOW_H14:
                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                HDMI_HAL_DPRINTF("** HDMI 14 Active cable clear error status port %d\r\n", enInputPortType);


                pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_CHECK_STABLE_H14;
                bNextStateFlag = TRUE;

                break;

            case HDMI_ACTIVE_CABLE_CHECK_STABLE_H14:
                if(Hal_HDMI_GetDEStableStatus(enInputPortType) &&
                    (!_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus)))
                {
                    if(pstHDMIPollingInfo->u16ActiveCableCheckErrorCount < TMDS_14_CHECK_ERROR_TIMES)
                    {
                        pstHDMIPollingInfo->u16ActiveCableCheckErrorCount++;
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                        pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_FINISH;
                        pstHDMIPollingInfo->bActiveCableFlag = TRUE;
                        bNextStateFlag = TRUE;
                    }
                }
                else // DE unstable or BCH error
                {
                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, decode error0: 0x%x \r\n", stErrorStatus.u16DecodeErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, decode error1: 0x%x \r\n", stErrorStatus.u16DecodeErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, decode error2: 0x%x \r\n", stErrorStatus.u16DecodeErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, port %d\r\n",enInputPortType);


                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, WB error0: %d\r\n", stErrorStatus.bWordBoundaryErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, WB error1: %d\r\n", stErrorStatus.bWordBoundaryErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, WB error2: %d\r\n", stErrorStatus.bWordBoundaryErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 14 Active cable error happened, port %d\r\n",  enInputPortType);


                    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_FINISH;
                    pstHDMIPollingInfo->bActiveCableFlag = FALSE;
                    bNextStateFlag = TRUE;
                }
                break;

            case HDMI_ACTIVE_CABLE_EQ_FLOW_H20:
                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, TRUE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                if(pstHDMIPollingInfo->u8ActiveCableClearErrorStatusCount < TMDS_CLEAR_ERROR_STATUS_COUNT)
                {
                    pstHDMIPollingInfo->u8ActiveCableClearErrorStatusCount++;
                    bNextStateFlag = FALSE;
                }
                else
                {
                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable clear error status port %d\r\n", enInputPortType);

                    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_CHECK_STABLE_H20;
                    bNextStateFlag = TRUE;
                }
                break;

            case HDMI_ACTIVE_CABLE_CHECK_STABLE_H20:
                if(Hal_HDMI_GetDEStableStatus(enInputPortType) &&
                    (!_Hal_tmds_GetErrorCountStatus(enInputPortType, TRUE, &stErrorStatus)))
                {
                    if(pstHDMIPollingInfo->u16ActiveCableCheckErrorCount < TMDS_20_CHECK_ERROR_TIMES)
                    {
                        pstHDMIPollingInfo->u16ActiveCableCheckErrorCount++;
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                        pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_FINISH;
                        pstHDMIPollingInfo->bActiveCableFlag = TRUE;
                        bNextStateFlag = TRUE;
                    }
                }
                else // DE unstable or BCH error
                {
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 20 Active cable error happened, decode error: 0x%x, 0x%x, 0x%x port %d\r\n", stErrorStatus.u16DecodeErrorL0, stErrorStatus.u16DecodeErrorL1, stErrorStatus.u16DecodeErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 20 Active cable error happened, WB error: %d, %d, %d port %d\r\n", stErrorStatus.bWordBoundaryErrorL0, stErrorStatus.bWordBoundaryErrorL1, stErrorStatus.bWordBoundaryErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));

                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, decode error0: 0x%x \r\n", stErrorStatus.u16DecodeErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, decode error1: 0x%x \r\n", stErrorStatus.u16DecodeErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, decode error2: 0x%x \r\n", stErrorStatus.u16DecodeErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, port %d\r\n",enInputPortType);


                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, WB error0: %d\r\n", stErrorStatus.bWordBoundaryErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, WB error1: %d\r\n", stErrorStatus.bWordBoundaryErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, WB error2: %d\r\n", stErrorStatus.bWordBoundaryErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 20 Active cable error happened, port %d\r\n",  enInputPortType);

                    pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_FINISH;
                    pstHDMIPollingInfo->bActiveCableFlag = FALSE;
                    bNextStateFlag = TRUE;
                }
                break;

            case HDMI_ACTIVE_CABLE_FINISH:
                pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_NONE;
                msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0008, BIT(3));

                if(!pstHDMIPollingInfo->bActiveCableFlag)
                {
                    _Hal_tmds_ReleaseActiveCablePatch(enInputPortType, pstHDMIPollingInfo);
                }
                else
                {
                    msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0004, BIT(2));
                }

                bNextStateFlag = TRUE;
                break;

            default:
                bNextStateFlag = FALSE;
                bCheckDoneFlag = TRUE;
                break;

        }
    }while(bNextStateFlag);

    return bCheckDoneFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_NewPHYAutoEQTrigger()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_NewPHYAutoEQTrigger(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)), WORD LanSel)
{
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    msWrite2ByteMask(REG_PHY2P1_0_P0_20_L +u32PHY2P1BankOffset, 0x14, BMASK(5:0)); // phy2p1_0_20[5:0]: reg_eq_strength_ini

    // trigger autoEQ
    msWrite2ByteMask(REG_PHY2P1_0_P0_05_L +u32PHY2P1BankOffset, LanSel, BMASK(11:8));
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetEQFlowDoneFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_GetEQFlowDoneFlag(BYTE enInputPortType, BYTE opt)
{
    BYTE bEQFlowDoneFlag = 0;
    DWORD u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(msRead2Byte(REG_PHY2P1_0_P0_10_L +u32PHY2P1BankOffset) &BIT(7))
        bEQFlowDoneFlag |= 0x01;

    if(msRead2Byte(REG_PHY2P1_0_P0_11_L +u32PHY2P1BankOffset) &BIT(7))
        bEQFlowDoneFlag |= 0x02;

    if(msRead2Byte(REG_PHY2P1_0_P0_12_L +u32PHY2P1BankOffset) &BIT(7))
        bEQFlowDoneFlag |= 0x04;

    if(msRead2Byte(REG_PHY2P1_0_P0_13_L +u32PHY2P1BankOffset) &BIT(7))
        bEQFlowDoneFlag |= 0x08;

    bEQFlowDoneFlag &= opt;
    return (bEQFlowDoneFlag == opt);
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetErrorCountStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_GetErrorCountStatus(BYTE enInputPortType, Bool bHDMI20Flag, stHDMI_ERROR_STATUS *pstErrorStatus)
{
    Bool bWordBoundaryUnlock = FALSE;
    Bool bDecodeError = FALSE;
    WORD u16DecodeErrorThr = 0;
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(msRead2Byte(REG_HDMIRX_DTOP_P0_4F_L +u32DTOPBankOffset) & BIT(3)) // hdmirx_dtop_4F[15:0]: reg_dbg_rpt - ch0
    {
        bWordBoundaryUnlock = TRUE;
        pstErrorStatus->bWordBoundaryErrorL0 = TRUE;
    }
    if(msRead2Byte(REG_HDMIRX_DTOP_P0_4F_L +u32DTOPBankOffset) & BIT(7)) // hdmirx_dtop_4F[15:0]: reg_dbg_rpt - ch1
    {
        bWordBoundaryUnlock = TRUE;
        pstErrorStatus->bWordBoundaryErrorL1 = TRUE;
    }
    if(msRead2Byte(REG_HDMIRX_DTOP_P0_4F_L +u32DTOPBankOffset) & BIT(11)) // hdmirx_dtop_4F[15:0]: reg_dbg_rpt - ch2
    {
        bWordBoundaryUnlock = TRUE;
        pstErrorStatus->bWordBoundaryErrorL2 = TRUE;
    }

    if(bWordBoundaryUnlock)
    {
        // clear WB unlock: hdmirx_dtop_4F[3][7][11]
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_4D_L + u32DTOPBankOffset, BIT(0), BIT(0)); // hdmirx_dtop_4D[0]: reg_clr_err_chk
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_4D_L + u32DTOPBankOffset, 0, BIT(0)); // hdmirx_dtop_4D[0]: reg_clr_err_chk
    }

    u16DecodeErrorThr = bHDMI20Flag ? HDMI_DECODE_ERROR_THRESHOLD20 : HDMI_DECODE_ERROR_THRESHOLD14;

    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_58_L +u32DTOPBankOffset, 0xFE00, BMASK(15:8)); // hdmirx_dtop_58[15:8]: reg_dec20_tst_bus_sel - ch0

    pstErrorStatus->u16DecodeErrorL0 = msRead2Byte(REG_HDMIRX_DTOP_P0_59_L +u32DTOPBankOffset); // hdmirx_dtop_59[15:0]: reg_tst_bus_rpt0

    if(pstErrorStatus->u16DecodeErrorL0 > u16DecodeErrorThr)
    {
        bDecodeError = TRUE;
    }

    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_58_L +u32DTOPBankOffset, 0xFD00, BMASK(15:8)); // hdmirx_dtop_58[15:8]: reg_dec20_tst_bus_sel - ch1

    pstErrorStatus->u16DecodeErrorL1 = msRead2Byte(REG_HDMIRX_DTOP_P0_59_L +u32DTOPBankOffset); // hdmirx_dtop_59[15:0]: reg_tst_bus_rpt0

    if(pstErrorStatus->u16DecodeErrorL1 > u16DecodeErrorThr)
    {
        bDecodeError = TRUE;
    }

    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_58_L +u32DTOPBankOffset, 0xFC00, BMASK(15:8)); // hdmirx_dtop_58[15:8]: reg_dec20_tst_bus_sel - ch2

    pstErrorStatus->u16DecodeErrorL2 = msRead2Byte(REG_HDMIRX_DTOP_P0_59_L +u32DTOPBankOffset); // hdmirx_dtop_59[15:0]: reg_tst_bus_rpt0

    if(pstErrorStatus->u16DecodeErrorL2 > u16DecodeErrorThr)
    {
        bDecodeError = TRUE;
    }

    if(bDecodeError)
    {
        // clear decode error: hdmirx_dtop_59[15:0]
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_40_L + u32DTOPBankOffset, BIT(15), BIT(15)); // hdmirx_dtop_40[15]: reg_sw_clr_err_cnt
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_40_L + u32DTOPBankOffset, 0, BIT(15)); // hdmirx_dtop_40[15]: reg_sw_clr_err_cnt
    }

    return (bWordBoundaryUnlock | bDecodeError);
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_ChangePHYCase()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_ChangePHYCase(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    Bool bFinishFlag = FALSE;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if (pstHDMIPollingInfo->u8ChangePHYCase != HDMI_CHANGE_PHY_FINISH)
    {
        _Hal_tmds_ResetDLPF(enInputPortType);
    }

    switch(pstHDMIPollingInfo->u8ChangePHYCase)
    {
        case HDMI_CHANGE_PHY_H14_CASE0:
            {
                BYTE u8EQValue[4] = {33, 33, 33, 33};

                HDMI_HAL_DPRINTF("** HDMI change PHY H14 case0 port %d\r\n", enInputPortType);
                pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H14_CASE1;

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(0));
            }
            break;

        case HDMI_CHANGE_PHY_H14_CASE1:
            {
                BYTE u8EQValue[4] = {8, 8, 8, 8};

                HDMI_HAL_DPRINTF("** HDMI change PHY H14 case1 port %d\r\n", enInputPortType);
                pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H14_CASE2;

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(0));
            }
            break;

        case HDMI_CHANGE_PHY_H14_CASE2:
            {
                BYTE u8EQValue[4] = {16, 16, 16, 16};

                HDMI_HAL_DPRINTF("** HDMI change PHY H14 case2 port %d\r\n", enInputPortType);
                pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_FINISH;

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(0));
                msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(0));
            }
            break;

        case HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE0:
            {
                BYTE u8EQValue[4] = {15, 15, 15, 15};

                HDMI_HAL_DPRINTF("** HDMI change PHY H14 75M_TO_135M case0 port %d\r\n", enInputPortType);
                pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE1;

                msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_60[3:0]: EQ_RL override value - L0; [4]: EQ/PGA-RL override enable - L0
                msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_63[3:0]: EQ_RL override value - L1; [4]: EQ/PGA-RL override enable - L1
                msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_66[3:0]: EQ_RL override value - L2; [4]: EQ/PGA-RL override enable - L2
                msWrite2ByteMask(REG_PHY2P1_2_P0_69_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_69[3:0]: EQ_RL override value - L3; [4]: EQ/PGA-RL override enable - L3

                msWrite2ByteMask(REG_PHY2P1_2_P0_28_L +u32PHY2P1BankOffset, 0x0081, BIT(7)|BMASK(4:0)); // phy2_28[4:0] reg_gc_ictrl_pfd_ov, phy2_28[7] reg_gc_ictrl_pfd_ov_en

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);
            }
            break;

        case HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE1:
            {
                BYTE u8EQValue[4] = {25, 25, 25, 25};

                HDMI_HAL_DPRINTF("** HDMI change PHY H14 75M_TO_135M case1 port %d\r\n", enInputPortType);
                pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_FINISH;

                msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_60[3:0]: EQ_RL override value - L0; [4]: EQ/PGA-RL override enable - L0
                msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_63[3:0]: EQ_RL override value - L1; [4]: EQ/PGA-RL override enable - L1
                msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_66[3:0]: EQ_RL override value - L2; [4]: EQ/PGA-RL override enable - L2
                msWrite2ByteMask(REG_PHY2P1_2_P0_69_L +u32PHY2P1BankOffset, BIT(4)| BMASK(3:0), BIT(4)| BMASK(3:0)); // phy2p1_2_69[3:0]: EQ_RL override value - L3; [4]: EQ/PGA-RL override enable - L3

                msWrite2ByteMask(REG_PHY2P1_2_P0_28_L +u32PHY2P1BankOffset, 0x0081, BIT(7)|BMASK(4:0)); // phy2_28[4:0] reg_gc_ictrl_pfd_ov, phy2_28[7] reg_gc_ictrl_pfd_ov_en

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);
            }
            break;

        case HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE2:
            {
                BYTE u8EQValue[4] = {15, 15, 15, 15};

                HDMI_HAL_DPRINTF("** HDMI change PHY H14 75M_TO_135M case2 port %d\r\n", enInputPortType);
                pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE0;

                msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_60[3:0]: EQ_RL override value - L0; [4]: EQ/PGA-RL override enable - L0
                msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_63[3:0]: EQ_RL override value - L1; [4]: EQ/PGA-RL override enable - L1
                msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_66[3:0]: EQ_RL override value - L2; [4]: EQ/PGA-RL override enable - L2
                msWrite2ByteMask(REG_PHY2P1_2_P0_69_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_69[3:0]: EQ_RL override value - L3; [4]: EQ/PGA-RL override enable - L3

                msWrite2Byte(REG_PHY2P1_3_P0_18_L +u32PHY2P1BankOffset, 0x6666); // phy2p1_3_18[15:12][11:8][7:4][3:0] reg_gc_pga_rl_l0,l1,l2,l3

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);
            }
            break;

        case HDMI_CHANGE_PHY_H20_CASE0:
            HDMI_HAL_DPRINTF("** HDMI change PHY H20 case0 port %d\r\n", enInputPortType);
            pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H20_CASE1;

            msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, BIT(7)| BIT(5), BIT(7)| BMASK(6:4));

            // case0 EQ setting
            msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, BIT(4)| 0x5, BIT(4)| BMASK(3:0)); // phy2p1_2_60[3:0]: EQ_RL override value - L0; [4]: EQ/PGA-RL override enable - L0
            msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, BIT(4)| 0x5, BIT(4)| BMASK(3:0)); // phy2p1_2_63[3:0]: EQ_RL override value - L1; [4]: EQ/PGA-RL override enable - L1
            msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, BIT(4)| 0x5, BIT(4)| BMASK(3:0)); // phy2p1_2_66[3:0]: EQ_RL override value - L2; [4]: EQ/PGA-RL override enable - L2
            msWrite2ByteMask(REG_PHY2P1_2_P0_69_L +u32PHY2P1BankOffset, BIT(4)| 0x5, BIT(4)| BMASK(3:0)); // phy2p1_2_69[3:0]: EQ_RL override value - L3; [4]: EQ/PGA-RL override enable - L3

            msWrite2ByteMask(REG_PHY2P1_3_P0_0C_L +u32PHY2P1BankOffset, BIT(15)| BIT(8), BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0C[0:2]: short EQ-RS_L0 OV; [3]: short EQ-RS_L0 OVE; [4:7]: EQ-RS1_L0 OV; [8]: EQ-RS1_L0 OVE; [15]: reg_gc_eqrs_l0_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0D_L +u32PHY2P1BankOffset, BIT(15)| BIT(8), BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0D[0:2]: short EQ-RS_L1 OV; [3]: short EQ-RS_L1 OVE; [4:7]: EQ-RS1_L1 OV; [8]: EQ-RS1_L1 OVE; [15]: reg_gc_eqrs_l1_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0E_L +u32PHY2P1BankOffset, BIT(15)| BIT(8), BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0E[0:2]: short EQ-RS_L2 OV; [3]: short EQ-RS_L2 OVE; [4:7]: EQ-RS1_L2 OV; [8]: EQ-RS1_L2 OVE; [15]: reg_gc_eqrs_l2_ov_en
            msWrite2ByteMask(REG_PHY2P1_3_P0_0F_L +u32PHY2P1BankOffset, BIT(15)| BIT(8), BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0F[0:2]: short EQ-RS_L3 OV; [3]: short EQ-RS_L3 OVE; [4:7]: EQ-RS1_L3 OV; [8]: EQ-RS1_L3 OVE; [15]: reg_gc_eqrs_l3_ov_en

            break;

        case HDMI_CHANGE_PHY_H20_CASE1:
            {
                BYTE u8EQValue[4] = {8, 8, 8, 8};

                HDMI_HAL_DPRINTF("** HDMI change PHY H20 case1 port %d\r\n", enInputPortType);
                pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_FINISH;

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                msWrite2ByteMask(REG_PHY2P1_2_P0_26_L +u32PHY2P1BankOffset, BMASK(5:4), BIT(7)| BMASK(6:4));

                // disable case0 EQ setting
                msWrite2ByteMask(REG_PHY2P1_2_P0_60_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_60[3:0]: EQ_RL override value - L0; [4]: EQ/PGA-RL override enable - L0
                msWrite2ByteMask(REG_PHY2P1_2_P0_63_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_63[3:0]: EQ_RL override value - L1; [4]: EQ/PGA-RL override enable - L1
                msWrite2ByteMask(REG_PHY2P1_2_P0_66_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_66[3:0]: EQ_RL override value - L2; [4]: EQ/PGA-RL override enable - L2
                msWrite2ByteMask(REG_PHY2P1_2_P0_69_L +u32PHY2P1BankOffset, 0, BIT(4)| BMASK(3:0)); // phy2p1_2_69[3:0]: EQ_RL override value - L3; [4]: EQ/PGA-RL override enable - L3

                msWrite2ByteMask(REG_PHY2P1_3_P0_0C_L +u32PHY2P1BankOffset, 0, BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0C[0:2]: short EQ-RS_L0 OV; [3]: short EQ-RS_L0 OVE; [4:7]: EQ-RS1_L0 OV; [8]: EQ-RS1_L0 OVE; [15]: reg_gc_eqrs_l0_ov_en
                msWrite2ByteMask(REG_PHY2P1_3_P0_0D_L +u32PHY2P1BankOffset, 0, BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0D[0:2]: short EQ-RS_L0 OV; [3]: short EQ-RS_L0 OVE; [4:7]: EQ-RS1_L0 OV; [8]: EQ-RS1_L0 OVE; [15]: reg_gc_eqrs_l0_ov_en
                msWrite2ByteMask(REG_PHY2P1_3_P0_0E_L +u32PHY2P1BankOffset, 0, BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0E[0:2]: short EQ-RS_L0 OV; [3]: short EQ-RS_L0 OVE; [4:7]: EQ-RS1_L0 OV; [8]: EQ-RS1_L0 OVE; [15]: reg_gc_eqrs_l0_ov_en
                msWrite2ByteMask(REG_PHY2P1_3_P0_0F_L +u32PHY2P1BankOffset, 0, BIT(15)| BIT(8)| BMASK(7:4)); // phy2p1_3_0F[0:2]: short EQ-RS_L0 OV; [3]: short EQ-RS_L0 OVE; [4:7]: EQ-RS1_L0 OV; [8]: EQ-RS1_L0 OVE; [15]: reg_gc_eqrs_l0_ov_en
            }
            break;

        case HDMI_CHANGE_PHY_FINISH:
            pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_FINISH;
            bFinishFlag = TRUE;
            break;

        default:
            break;
    };

    return bFinishFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_ActiveCableCheckDLEV()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_ActiveCableCheckDLEV(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    Bool bDLEVFlag = FALSE;
    BYTE u8DLEVCnt = 0;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
    {
        u8DLEVCnt = HDMI_VDAC_DLEV_COUNT_PORT20;
    }
    else
    {
        u8DLEVCnt = HDMI_VDAC_DLEV_COUNT_H20;
    }

    if(_Hal_tmds_DIGLockCheck(enInputPortType))
    {
        if(msRead2Byte(REG_PHY2P1_0_P0_64_L +u32PHY2P1BankOffset) > u8DLEVCnt) // phy0_64[9:0]: reg_vdac_in_dlev_bin_0
        {
            if(msRead2Byte(REG_PHY2P1_0_P0_65_L +u32PHY2P1BankOffset) > u8DLEVCnt) // phy0_65[9:0]: reg_vdac_in_dlev_bin_1
            {
                if(msRead2Byte(REG_PHY2P1_0_P0_66_L +u32PHY2P1BankOffset) > u8DLEVCnt) // phy0_66[9:0]: reg_vdac_in_dlev_bin_2
                {
                    bDLEVFlag = TRUE;
                }
            }
        }
    }

    return bDLEVFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_DIGLockCheck()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_DIGLockCheck(BYTE enInputPortType)
{
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if( (msRead2Byte(REG_PHY2P1_4_P0_5E_L + u32PHY2P1BankOffset)&BIT(12))==BIT(12))  //dig lock
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_PHYInit()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_PHYInit(BYTE enInputPortType)
{
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    WORD u16DPRx_AUX_OFFSET = 0;
    BYTE u8Index = 0;
    //DWORD u32PHY2P1PMBankOffset = 0;//_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
    enInputPortType = enInputPortType;
    msWriteByteMask(REG_DPRX_PHY_PM_66_L , 0x00, BIT(0)); //NODIE_PD_CLKIN

    for(u8Index = 0; u8Index <3; u8Index ++)
    {
        u16DPRx_AUX_OFFSET = DPRx_AUX_OFFSET300(u8Index);
        msWriteByteMask(REG_DPRX_AUX_57_L + u16DPRx_AUX_OFFSET, 0, BIT0|BIT1|BIT2); // Disble DP wake up event to PM
    }


    //==New added==
    msWrite2Byte(REG_PHY2P1_2_P0_72_L + u32PHY2P1BankOffset, 0x0400);
    msWrite2Byte(REG_PHY2P1_4_P0_0B_L + u32PHY2P1BankOffset, 0x4433);
    msWrite2Byte(REG_PHY2P1_4_P0_25_L + u32PHY2P1BankOffset, 0x4011);
    msWrite2Byte(REG_PHY2P1_4_P0_24_L + u32PHY2P1BankOffset, 0x1101);
    msWrite2Byte(REG_PHY2P1_4_P0_23_L + u32PHY2P1BankOffset, 0x1111);
    msWrite2Byte(REG_PHY2P1_4_P0_22_L + u32PHY2P1BankOffset, 0x1111);
    msWrite2Byte(REG_PHY2P1_4_P0_21_L + u32PHY2P1BankOffset, 0x1111);
    msWrite2Byte(REG_PHY2P1_4_P0_20_L + u32PHY2P1BankOffset, 0x1111);
    msWrite2Byte(REG_PHY2P1_4_P0_27_L + u32PHY2P1BankOffset, 0x4040);
    msWrite2Byte(REG_PHY2P1_4_P0_2F_L + u32PHY2P1BankOffset, 0x0606);
    msWrite2Byte(REG_PHY2P1_4_P0_47_L + u32PHY2P1BankOffset, 0x0606);
    msWrite2Byte(REG_PHY2P1_4_P0_16_L + u32PHY2P1BankOffset, 0x7770);
    msWrite2Byte(REG_PHY2P1_4_P0_15_L + u32PHY2P1BankOffset, 0x7000);
    msWrite2Byte(REG_PHY2P1_4_P0_14_L + u32PHY2P1BankOffset, 0x0007);
    msWrite2Byte(REG_PHY2P1_4_P0_17_L + u32PHY2P1BankOffset, 0x3777);


    //==New added==
    msWrite2ByteMask(REG_PHY2P1_1_P0_57_L + u32PHY2P1BankOffset, 0, BIT(11)); // phy2p1_57[11]: enable of saff cal start hardware mode
    //msWrite2Byte(REG_PHY2P1_3_P0_06_L + u32PHY2P1BankOffset, 0x00f0); // phy2p1_3_06[3:0]: reg_hdmi2p1_atop_gate; [7:4]: reg_hdmi2p1_atop_inv

    // autoEQ parameter
    msWrite2Byte(REG_PHY2P1_1_P0_01_L + u32PHY2P1BankOffset, 0xFFFF);

    msWrite2Byte(REG_PHY2P1_1_P0_08_L + u32PHY2P1BankOffset, 0x03FF); // phy1_08[15:0] reg_pat_pass2_num_a_l0
    msWrite2Byte(REG_PHY2P1_1_P0_09_L + u32PHY2P1BankOffset, 0x03FF); // phy1_09[15:0] reg_pat_pass2_num_b_l0
    msWrite2Byte(REG_PHY2P1_1_P0_0A_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0a[15:0] reg_pat_pass2_num_c_l0
    msWrite2Byte(REG_PHY2P1_1_P0_0B_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0b[15:0] reg_pat_pass2_num_d_l0
    msWrite2Byte(REG_PHY2P1_1_P0_0C_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0c[15:0] reg_pat_pass2_num_a_l1
    msWrite2Byte(REG_PHY2P1_1_P0_0D_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0d[15:0] reg_pat_pass2_num_b_l1
    msWrite2Byte(REG_PHY2P1_1_P0_0E_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0e[15:0] reg_pat_pass2_num_c_l1
    msWrite2Byte(REG_PHY2P1_1_P0_0F_L + u32PHY2P1BankOffset, 0x03FF); // phy1_0f[15:0] reg_pat_pass2_num_d_l1
    msWrite2Byte(REG_PHY2P1_1_P0_10_L + u32PHY2P1BankOffset, 0x03FF); // phy1_10[15:0] reg_pat_pass2_num_a_l2
    msWrite2Byte(REG_PHY2P1_1_P0_11_L + u32PHY2P1BankOffset, 0x03FF); // phy1_11[15:0] reg_pat_pass2_num_b_l2
    msWrite2Byte(REG_PHY2P1_1_P0_12_L + u32PHY2P1BankOffset, 0x03FF); // phy1_12[15:0] reg_pat_pass2_num_c_l2
    msWrite2Byte(REG_PHY2P1_1_P0_13_L + u32PHY2P1BankOffset, 0x03FF); // phy1_13[15:0] reg_pat_pass2_num_d_l2

    msWrite2Byte(REG_PHY2P1_1_P0_14_L + u32PHY2P1BankOffset, 0x03F0); // phy1_14[9:0] reg_eq_sth_acc_th_a_l0
    msWrite2Byte(REG_PHY2P1_1_P0_15_L + u32PHY2P1BankOffset, 0x03F0); // phy1_15[9:0] reg_eq_sth_acc_th_b_l0
    msWrite2Byte(REG_PHY2P1_1_P0_16_L + u32PHY2P1BankOffset, 0x03F0); // phy1_16[9:0] reg_eq_sth_acc_th_c_l0
    msWrite2Byte(REG_PHY2P1_1_P0_17_L + u32PHY2P1BankOffset, 0x03F0); // phy1_17[9:0] reg_eq_sth_acc_th_d_l0
    msWrite2Byte(REG_PHY2P1_1_P0_18_L + u32PHY2P1BankOffset, 0x03F0); // phy1_18[9:0] reg_eq_sth_acc_th_a_l1
    msWrite2Byte(REG_PHY2P1_1_P0_19_L + u32PHY2P1BankOffset, 0x03F0); // phy1_19[9:0] reg_eq_sth_acc_th_b_l1
    msWrite2Byte(REG_PHY2P1_1_P0_1A_L + u32PHY2P1BankOffset, 0x03F0); // phy1_1a[9:0] reg_eq_sth_acc_th_c_l1
    msWrite2Byte(REG_PHY2P1_1_P0_1B_L + u32PHY2P1BankOffset, 0x03F0); // phy1_1b[9:0] reg_eq_sth_acc_th_d_l1
    msWrite2Byte(REG_PHY2P1_1_P0_1C_L + u32PHY2P1BankOffset, 0x03F0); // phy1_1c[9:0] reg_eq_sth_acc_th_a_l2
    msWrite2Byte(REG_PHY2P1_1_P0_1D_L + u32PHY2P1BankOffset, 0x03F0); // phy1_1d[9:0] reg_eq_sth_acc_th_b_l2
    msWrite2Byte(REG_PHY2P1_1_P0_1E_L + u32PHY2P1BankOffset, 0x03F0); // phy1_1e[9:0] reg_eq_sth_acc_th_c_l2
    msWrite2Byte(REG_PHY2P1_1_P0_1F_L + u32PHY2P1BankOffset, 0x03F0); // phy1_1f[9:0] reg_eq_sth_acc_th_d_l2

    msWrite2Byte(REG_PHY2P1_1_P0_20_L + u32PHY2P1BankOffset, 0x00FF); // phy1_20[9:0] reg_eq_sth_chg_th_a_l0
    msWrite2Byte(REG_PHY2P1_1_P0_21_L + u32PHY2P1BankOffset, 0x00FF); // phy1_21[9:0] reg_eq_sth_chg_th_b_l0
    msWrite2Byte(REG_PHY2P1_1_P0_22_L + u32PHY2P1BankOffset, 0x00FF); // phy1_22[9:0] reg_eq_sth_chg_th_c_l0
    msWrite2Byte(REG_PHY2P1_1_P0_23_L + u32PHY2P1BankOffset, 0x00FF); // phy1_23[9:0] reg_eq_sth_chg_th_d_l0
    msWrite2Byte(REG_PHY2P1_1_P0_24_L + u32PHY2P1BankOffset, 0x00FF); // phy1_24[9:0] reg_eq_sth_chg_th_a_l1
    msWrite2Byte(REG_PHY2P1_1_P0_25_L + u32PHY2P1BankOffset, 0x00FF); // phy1_25[9:0] reg_eq_sth_chg_th_b_l1
    msWrite2Byte(REG_PHY2P1_1_P0_26_L + u32PHY2P1BankOffset, 0x00FF); // phy1_26[9:0] reg_eq_sth_chg_th_c_l1
    msWrite2Byte(REG_PHY2P1_1_P0_27_L + u32PHY2P1BankOffset, 0x00FF); // phy1_27[9:0] reg_eq_sth_chg_th_d_l1
    msWrite2Byte(REG_PHY2P1_1_P0_28_L + u32PHY2P1BankOffset, 0x00FF); // phy1_28[9:0] reg_eq_sth_chg_th_a_l2
    msWrite2Byte(REG_PHY2P1_1_P0_29_L + u32PHY2P1BankOffset, 0x00FF); // phy1_29[9:0] reg_eq_sth_chg_th_b_l2
    msWrite2Byte(REG_PHY2P1_1_P0_2A_L + u32PHY2P1BankOffset, 0x00FF); // phy1_2a[9:0] reg_eq_sth_chg_th_c_l2
    msWrite2Byte(REG_PHY2P1_1_P0_2B_L + u32PHY2P1BankOffset, 0x00FF); // phy1_2b[9:0] reg_eq_sth_chg_th_d_l2

    //msWrite2ByteMask(REG_PHY2P1_1_P0_30_L + u32PHY2P1BankOffset, BMASK(2:1), BMASK(2:1)); // phy2p1_1_30[2:1]: enable DE for symbol detect
    msWrite2Byte(REG_PHY2P1_1_P0_30_L + u32PHY2P1BankOffset, PHY_SW_VERSION);

    msWrite2ByteMask(REG_PHY2P1_0_P0_2C_L + u32PHY2P1BankOffset, 0x0C00, BMASK(15:10)); // phy2p1_0_2C[10]: reg_eq_sth_acc_sel; [11]: reg_eq_mode_add_ov_en; [15:12]: reg_eq_mode_add_ov
    msWrite2ByteMask(REG_PHY2P1_3_P0_38_L + u32PHY2P1BankOffset, 0x0009, BIT(3)|BMASK(1:0)); // phy2p1_3_38[3]: reg_agc_mode_select_ov_en; [1:0]: reg_agc_mode_select

    msWrite2ByteMask(REG_PHY2P1_1_P0_48_L + u32PHY2P1BankOffset, BIT(9), BIT(9)); // phy2p1_1_48[9]: reg_hdmi_14_20_freq_det_only
    msWrite2ByteMask(REG_PHY2P1_1_P0_4A_L + u32PHY2P1BankOffset, 0x40, BMASK(7:4)); // phy2p1_1_4A[7:4]: reg_s3_cmp_c_sel
    msWrite2Byte(REG_PHY2P1_1_P0_5C_L + u32PHY2P1BankOffset, 0xFFFF); // IRQ mask

    msWrite2ByteMask(REG_PHY2P1_3_P0_0A_L + u32PHY2P1BankOffset, 0x0000, BMASK(9:8)|BMASK(1:0)); // phy2p1_3_0A[0]: reg_atop_pd_hw_ctrl_sel; [1]: reg_atop_pd_hw_ctrl_en; [9]: reg_en_clko_link_div20_ov_en
    msWrite2ByteMask(REG_PHY2P1_3_P0_07_L + u32PHY2P1BankOffset, BIT(12), BIT(12)); // phy2p1_3_07[12]: reg_en_clko_pix_ov_en
    msWrite2ByteMask(REG_PHY2P1_3_P0_20_L + u32PHY2P1BankOffset, 6, BMASK(3:0));

    msWrite2Byte(REG_PHY2P1_2_P0_02_L + u32PHY2P1BankOffset, 0x0000); // phy2p1_2_02[3:0]: reg_gcr_iedata_load_sync_ov; [7]: reg_gcr_iedata_load_sync_ov_en; phy2_02[9:8] reg_gc_phdac_int_wei; phy2_02[12] reg_en_acdr_mode
    msWrite2Byte(REG_PHY2P1_2_P0_04_L + u32PHY2P1BankOffset, 0x8007);
    msWrite2Byte(REG_PHY2P1_2_P0_05_L + u32PHY2P1BankOffset, 0x0883);
    //msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L + u32PHY2P1BankOffset, 0, BIT(4)|BIT(7)); // phy2p1_2_2f[4]: reg_pd_lane_ov_en; [7]:  reg_pd_pll_ov_en
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x0018, BMASK(4:0));
    msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(7), BMASK(7:6));


    msWrite2Byte(REG_PHY2P1_2_P0_2E_L + u32PHY2P1BankOffset, 0x0094); // phy2_2e[3] reg_pd_clkin_ov, phy2_2e[4] reg_en_clko_pix_ov, phy2_2e[7] reg_pd_phdac_i_ov_en, phy2_2e[13] reg_pd_phdac_q_ov_en
    msWrite2Byte(REG_PHY2P1_2_P0_6E_L + u32PHY2P1BankOffset, 0x0020); // phy2p1_2_6E[2:0]: reg_gc_phdac_slewrate_ov; [3]: reg_gc_phdac_slewrate_ov_en; [4]: reg_en_clko_tmds2x_ov; [5]: reg_en_clko_vcodiv; [6]: reg_pd_ldo

    msWrite2ByteMask(REG_PHY2P1_2_P0_6C_L + u32PHY2P1BankOffset, BIT(1), BMASK(1:0)); // phy2p1_2_6C[1]: reg_en_clko_link_datarate_div36_2x_ov_en; [0]: reg_en_clko_link_datarate_div36_2x_ov
    //msWrite2ByteMask(REG_PHY2P1_PM_P0_40_L + u32PHY2P1PMBankOffset, 0x000C, BMASK(3:0));


    //msWrite2Byte(REG_PHY2P1_PM_P0_41_L + u32PHY2P1PMBankOffset, 0x0000); // phy2p1_pm_41[5:0]: reg_nodie_sq_cal_ov_ov; [7]: reg_nodie_sq_cal_ov_ov_en; [14:12]: reg_nodie_sqh_vth_ov; [15]: reg_nodie_sqh_vth_ov
    //msWrite2Byte(REG_PHY2P1_PM_P0_42_L + u32PHY2P1PMBankOffset, 0x8F20); // phy2p1_pm_42[2]: reg_nodie_en_hdmi2p1_ov; [3]: reg_nodie_en_hdmi2p1_ov_en; [4]: reg_nodie_pd_clkin_ov; [5]: reg_nodie_pd_clkin_ov_en; [7]: reg_nodie_en_pad_short; [11:8]: reg_nodie_pd_rt_ov; [15]: reg_nodie_pd_rt_ov_en
    if(Input_HDMI_C1 != Input_Nothing)
    {
        msWrite2ByteMask(REG_DPRX_PHY_PM_00_L , 0x0000, BIT(3));
        msWrite2ByteMask(REG_DPRX_PHY_PM_00_L , 0x00F0, BMASK(7:4));

        msWrite2ByteMask(REG_DPRX_PHY_PM_01_L , 0x0100, BIT(8));

        msWrite2ByteMask(REG_DPRX_PHY_PM_01_L , 0x0000, BMASK(7:0));
        msWrite2ByteMask(REG_DPRX_PHY_PM_00_L , 0x0000, BMASK(2:0));
    }
    if(Input_HDMI_C2 != Input_Nothing)
    {
        msWrite2ByteMask(REG_DPRX_PHY_PM_20_L , 0x0000, BIT(3));
        msWrite2ByteMask(REG_DPRX_PHY_PM_20_L , 0x00F0, BMASK(7:4));

        msWrite2ByteMask(REG_DPRX_PHY_PM_21_L , 0x0100, BIT(8));

        msWrite2ByteMask(REG_DPRX_PHY_PM_21_L , 0x0000, BMASK(7:0));
        msWrite2ByteMask(REG_DPRX_PHY_PM_20_L , 0x0000, BMASK(2:0));
    }
    if(Input_HDMI_C3 != Input_Nothing)
    {
        msWrite2ByteMask(REG_DPRX_PHY_PM_40_L , 0x0000, BIT(3));
        msWrite2ByteMask(REG_DPRX_PHY_PM_40_L , 0x00F0, BMASK(7:4));

        msWrite2ByteMask(REG_DPRX_PHY_PM_41_L , 0x0100, BIT(8));

        msWrite2ByteMask(REG_DPRX_PHY_PM_41_L , 0x0000, BMASK(7:0));
        msWrite2ByteMask(REG_DPRX_PHY_PM_40_L , 0x0000, BMASK(2:0));
    }

    //by port setting --20200902
    msWriteByteMask(REG_DPRX_PHY_PM_66_L , 0x00, BIT(0)); //NODIE_PD_CLKIN


    msWrite2ByteMask(REG_DPRX_PHY1_01_L , 0x0000, BIT(4));


    msWrite2ByteMask(REG_PHY2P1_3_P0_75_L + u32PHY2P1BankOffset, 0x0000, BMASK(15:8)); // phy2p1_3_75[8]: reg_acdr_ov_phdac_en
    msWrite2Byte(REG_PHY2P1_3_P0_60_L + u32PHY2P1BankOffset, 0x00E4); // phy2p1_3_60[7:0]: reg_lane_swap0/1/2/3; [9:8]: reg_ckg_xtal; [11:10]: reg_ckg_mpll; [13:12]: reg_ckg_dvi_raw; [15]: reg_lane_swap_ov_en
    msWrite2Byte(REG_PHY2P1_3_P0_61_L + u32PHY2P1BankOffset, 0x0000); // phy2p1_3_61[7:0]: reg_ckg_clko_data_l0/1/2/3_1st; [15:8]: reg_ckg_clko_edata_l0/1/2/3_1st
    msWrite2Byte(REG_PHY2P1_3_P0_62_L + u32PHY2P1BankOffset, 0x0000); // phy2p1_3_62[7:0]: reg_ckg_clko_data_l0/1/2/3_swap; [15:8]: reg_ckg_clko_edata_l0/1/2/3_swap

    msWrite2ByteMask(REG_PHY2P1_3_P0_63_L + u32PHY2P1BankOffset, 0x0000, BMASK(3:0)); // phy3_63[1:0] reg_ckg_lckdet_swap; phy3_63[3:2] reg_ckg_pll_fb_swap

    msWrite2Byte(REG_PHY2P1_3_P0_64_L + u32PHY2P1BankOffset, 0x0000); // phy2p1_3_64[1:0]: reg_ckg_clko_data_af_ls_40; [3:2]: reg_ckg_clko_data_af_ls_36; [6:4]: reg_ckg_tstbus_sel; [7]: reg_phy2mac_ctrl_select; [8]: reg_clk_xtal_div2_en; [10]: reg_select_hdmi20_20b_source (hdmi21 40bit to 20 bit)
    msWrite2ByteMask(REG_PHY2P1_3_P0_65_L + u32PHY2P1BankOffset, 0x7C << 8, BMASK(15:8)); // phy2p1_3_65[15:8]: reg_lanesync_fifo_wr_ptr_rst
    msWrite2ByteMask(REG_PHY2P1_3_P0_67_L + u32PHY2P1BankOffset, 0, BMASK(14:12)); // phy2p1_3_64[1:0]: reg_ckg_clko_data_af_ls_40; [3:2]: reg_ckg_clko_data_af_ls_36; [6:4]: reg_ckg_tstbus_sel; [7]: reg_phy2mac_ctrl_select; [8]: reg_clk_xtal_div2_en; [10]: reg_select_hdmi20_20b_source (hdmi21 40bit to 20 bit)
    msWrite2Byte(REG_PHY2P1_3_P0_68_L + u32PHY2P1BankOffset, 0x0000); // phy2p1_3_68[7:0]: reg_ckg_br_det_l0/1/2/3_swap; [9:8]: reg_ckg_tmds_fg; [15:12]: reg_lane_pn_swape

    msWrite2Byte(REG_PHY2P1_0_P0_1C_L + u32PHY2P1BankOffset, 0x01FF); // phy0_1c[15:0]: reg_i_smbl_lk_conti

    msWrite2Byte(REG_PHY2P1_0_P0_38_L + u32PHY2P1BankOffset, 0x000F); // phy2p1_0_38[9:0]: reg_dlev_at_agc_eq

    msWrite2ByteMask(REG_PHY2P1_0_P0_4E_L + u32PHY2P1BankOffset, 0x0F, BMASK(9:0)); // phy2p1_0_4E[9:0]: reg_dlev_at_eq; [10]: reg_dlev_at_eq_en; [14]: eg_en_idac_tap_clr; [15]: reg_en_idac_tap_set
    msWrite2Byte(REG_PHY2P1_0_P0_4F_L + u32PHY2P1BankOffset, 0x0202); // phy2p1_0_4F[6:0]: reg_phase_good_thr; [14:8]: reg_phase_enough_thr

    msWrite2ByteMask(REG_PHY2P1_3_P0_66_L + u32PHY2P1BankOffset, 0x6000, BMASK(15:8)); // phy2p1_3_66[11:8]: reg_lane_reverse36; [12]: reg_hd20_hd21_sel; [13]: reg_clk_20bit_out; [14]: reg_20bit_out; [15]: reg_36bit_out

    // same as old phy (combo_phy1_10~1F)
    msWrite2Byte(REG_PHY2P1_3_P0_50_L + u32PHY2P1BankOffset, 0x0008); // phy2p1_3_50[4]: reg_swap_dcdr_updn_polity; [3]: reg_enable_phase_step
    msWrite2Byte(REG_PHY2P1_3_P0_58_L + u32PHY2P1BankOffset, 0x009A); // phy2p1_3_58[6:0]: reg_csync_interval0; [7]: reg_csync_slow0; [8]: reg_phase_hump0
    msWrite2Byte(REG_PHY2P1_3_P0_59_L + u32PHY2P1BankOffset, 0x009A); // phy2p1_3_59[6:0]: reg_csync_interval1; [7]: reg_csync_slow1; [8]: reg_phase_hump1
    msWrite2Byte(REG_PHY2P1_3_P0_5A_L + u32PHY2P1BankOffset, 0x009A); // phy2p1_3_5A[6:0]: reg_csync_interval2; [7]: reg_csync_slow2; [8]: reg_phase_hump2
    msWrite2Byte(REG_PHY2P1_3_P0_5B_L + u32PHY2P1BankOffset, 0x009A); // phy2p1_3_5B[6:0]: reg_csync_interval3; [7]: reg_csync_slow3; [8]: reg_phase_hump3
    msWrite2Byte(REG_PHY2P1_3_P0_53_L + u32PHY2P1BankOffset, 0x0100); // phy2p1_3_53[7:0]: reg_dvi_phase_thd; [8]: reg_auto_clr_enz; [10]: reg_delocken

    msWrite2Byte(REG_PHY2P1_0_P0_06_L + u32PHY2P1BankOffset, 0x0661); // phy0_06[3]: enable link train change FRL rate to reset eq fsm, phy0_06[4]: enable FFE change to reset eq fsm, phy2p1_0_06[9]: time out enable (2048us per state)
    msWrite2Byte(REG_PHY2P1_0_P0_08_L + u32PHY2P1BankOffset, 0x5555); // phy2p1_0_08[3:0]: reg_tap1_stable_thrd; [7:4]: reg_tap2_stable_thrd; [11:8]: reg_tap3_stable_thrd; [15:12]: reg_tap4_stable_thrd
    msWrite2Byte(REG_PHY2P1_0_P0_09_L + u32PHY2P1BankOffset, 0x5555); // phy2p1_0_09[3:0]: reg_tap5_stable_thrd; [7:4]: reg_tap6_stable_thrd; [11:8]: reg_tap7_stable_thrd; [15:12]: reg_tap8_stable_thrd
    msWrite2Byte(REG_PHY2P1_0_P0_0A_L + u32PHY2P1BankOffset, 0x5555); // phy2p1_0_0A[3:0]: reg_tapf1_stable_thrd; [7:4]: reg_tapf2_stable_thrd; [11:8]: reg_tapf3_stable_thrd; [15:12]: reg_tapf4_stable_thrd
    msWrite2Byte(REG_PHY2P1_0_P0_18_L + u32PHY2P1BankOffset, 0x0518); // phy0_18[3]reg_sweep_bypass, phy0_18[4] reg_dynamic_bypass, phy0_18[10:8] reg_aaba_ext, phy0_18[14] reg_dis_sqh_condi
    msWrite2Byte(REG_PHY2P1_0_P0_19_L + u32PHY2P1BankOffset, 0);
    // search each aaba timeout
    msWrite2Byte(REG_PHY2P1_0_P0_1E_L + u32PHY2P1BankOffset, 0xFFFF); // phy2p1_0_1E[15:0]: reg_eq_sth_det_duration
    msWrite2ByteMask(REG_PHY2P1_0_P0_20_L + u32PHY2P1BankOffset, 0x0300, BMASK(15:8)); // phy2p1_0_20[15]: reg_eq_coarse_direc; [10:8]:
    // under thershold = 3, upper thershold = 30
    msWrite2ByteMask(REG_PHY2P1_0_P0_21_L + u32PHY2P1BankOffset, 0x1E03, BMASK(15:8)| BMASK(5:0)); // phy2p1_0_21[13:8]: reg_eq_up_bond; [5:0]: reg_eq_lw_bond
    msWrite2Byte(REG_PHY2P1_0_P0_23_L + u32PHY2P1BankOffset, 0x80FF); // phy2p1_0_23[9:0]: reg_eq_sth_chg_th, phy0_23[15] reg_enable_time_out
    msWrite2ByteMask(REG_PHY2P1_0_P0_25_L + u32PHY2P1BankOffset, 0x0A00, BMASK(13:8)|BMASK(5:0)); // phy2p1_0_25[5:0]: reg_eq_strength_active_cable; [13:8]: reg_sth_eq_seq_vld;
    msWrite2Byte(REG_PHY2P1_0_P0_26_L + u32PHY2P1BankOffset, 0x0003); // phy2p1_0_26[15]: reg_enhan_aeq_enable; [12:8]: reg_redo_enhan_aeq; [5:0]: reg_sth_eq_diff_th
    // aaba reference to blanking
    msWrite2Byte(REG_PHY2P1_0_P0_27_L + u32PHY2P1BankOffset, 0x9100); // phy2p1_0_27[15:0]: reg_eq_top_condi
    msWrite2Byte(REG_PHY2P1_0_P0_2A_L + u32PHY2P1BankOffset, 0x012F); // phy2p1_0_2A[7:0]: reg_under_over_max_time; [15:8]: reg_err_cnt_th
    msWrite2Byte(REG_PHY2P1_0_P0_2B_L + u32PHY2P1BankOffset, 0x0000); // phy2p1_0_2B[5:0]: reg_eq_result_offset; [10:8]: reg_redo_aeq

    msWrite2Byte(REG_PHY2P1_0_P0_2E_L + u32PHY2P1BankOffset, 0x8A80); // phy0_2e[7:0]reg_agc_min, phy2p1_0_2E[15:8]: reg_agc_max
    msWrite2Byte(REG_PHY2P1_0_P0_3B_L + u32PHY2P1BankOffset, 0x57D2); // phy2p1_0_3B
    msWrite2Byte(REG_PHY2P1_0_P0_5D_L + u32PHY2P1BankOffset, 0x008A); // phy2p1_0_5D[7:0]: reg_agc_initial; [15:8]: reg_agc_dlev_target


    if( (msEread_GetDataFromEfuse(0x185)&BIT(6)) == BIT(6))
    {
        msWrite2ByteMask(REG_PHY2P1_0_P0_5D_L + u32PHY2P1BankOffset, 0x2D00, BMASK(15:8));//Vpp=180mV
    }
    else
    {
        msWrite2ByteMask(REG_PHY2P1_0_P0_5D_L + u32PHY2P1BankOffset, 0x2500, BMASK(15:8));//Vpp=150mV
    }

    msWrite2ByteMask(REG_PHY2P1_0_P0_2F_L + u32PHY2P1BankOffset, 0x0100, BMASK(15:8)); // phy0_2f[9] reg_en_ffe3_to_ffe0_clr_cr_lock

    msWrite2ByteMask(REG_PHY2P1_0_P0_34_L + u32PHY2P1BankOffset, 0x0000, BMASK(7:0)); // phy2p1_0_34[1:0]: reg_scan_src_sel; [3:2]: reg_scan_rpt_sel; [5]: reg_scan_timer_sel; [6]: reg_scan_timer_sel; [7]: reg_bps_af_eq_scan
    msWrite2ByteMask(REG_PHY2P1_0_P0_35_L + u32PHY2P1BankOffset, 0x0030, BMASK(7:4)); // phy0_35[7:4] reg_autoscan_accum_time
    msWrite2ByteMask(REG_PHY2P1_0_P0_3A_L + u32PHY2P1BankOffset, 0x0010, BMASK(7:0)); // phy2p1_0_3A[1]: reg_enable_coarse2done; [2]: reg_enable_coarse2done_bypass_enh_eq

    msWrite2ByteMask(REG_PHY2P1_0_P0_50_L + u32PHY2P1BankOffset, 0xC200, BMASK(15:8)); // phy2p1_0_50[11:8]: reg_dlev_settle_iteration; [14]: reg_update_dlev_time_out_enable; [15]: reg_dfe_time_out_en
    msWrite2ByteMask(REG_PHY2P1_0_P0_51_L + u32PHY2P1BankOffset, 0x4117, BMASK(14:0)); // phy2p1_0_51[7]: reg_en_pga_max; [10:8]: reg_filter_depth_dlev; [14:12]: reg_filter_depth_tap
    msWrite2ByteMask(REG_PHY2P1_0_P0_52_L + u32PHY2P1BankOffset, 0x1000, BMASK(15:8)); // phy2p1_0_52[13:12]: reg_dlev_step
    msWrite2ByteMask(REG_PHY2P1_0_P0_53_L + u32PHY2P1BankOffset, 0xFC00, BMASK(15:8)); // phy2p1_0_53[11:10]: reg_dlev_settle_pulse_en
    msWrite2Byte(REG_PHY2P1_0_P0_54_L + u32PHY2P1BankOffset, 0x0A26); // phy2p1_0_54[15:8]: reg_dlev_stable_thrd; [7:4]: reg_dfe_settle_iteration
    msWrite2ByteMask(REG_PHY2P1_0_P0_59_L + u32PHY2P1BankOffset, 0x0000, BMASK(15:8)); // phy2p1_0_59[15:14]: reg_agc_output_sel
    msWrite2ByteMask(REG_PHY2P1_0_P0_5A_L + u32PHY2P1BankOffset, 0x3400, BMASK(15:8)); // phy2p1_0_5A[11:10]: reg_eq_output_sel; [15:12]: reg_dfe_agc_freerun
    msWrite2Byte(REG_PHY2P1_0_P0_5C_L + u32PHY2P1BankOffset, 0x1020); // phy2p1_0_5C[7:0]: reg_dlev_good; [15:8]: reg_dlev_enough
    msWrite2Byte(REG_PHY2P1_0_P0_5E_L + u32PHY2P1BankOffset, 0x0823); // phy2p1_0_5E[15:8]: reg_dlev_stable_thrd_dfe; [7:4]: reg_agc_settle_iteration
    msWrite2Byte(REG_PHY2P1_2_P0_10_L + u32PHY2P1BankOffset, 0x0F0F); // phy2p1_2_10[7:0]: reg_pga_gain_l0_ov; [15:8]: reg_pga_gain_l1_ov
    msWrite2Byte(REG_PHY2P1_2_P0_11_L + u32PHY2P1BankOffset, 0x0F0F); // phy2p1_2_11[7:0]: reg_pga_gain_l2_ov; [15:8]: reg_pga_gain_l3_ov
    msWrite2ByteMask(REG_PHY2P1_2_P0_12_L + u32PHY2P1BankOffset, 0x0000, BMASK(3:0)); // phy2p1_2_12[3:0]: reg_pga_gain_l3/2/1/0_ov_en

    msWrite2Byte(REG_PHY2P1_3_P0_30_L + u32PHY2P1BankOffset, 0x1414); // phy2p1_3_30[6:0]: reg_enhan_aeq_eye_target; [15:8]: reg_dfe_eh_target
    msWrite2Byte(REG_PHY2P1_3_P0_31_L + u32PHY2P1BankOffset, 0x2094); // phy2p1_3_31[6:0]: reg_sweep_eye_target; [7]: reg_wait_for_agc_dfe_stable; [15:8]: reg_sweep_eye_high_target
    msWrite2ByteMask(REG_PHY2P1_3_P0_32_L + u32PHY2P1BankOffset, 0x0000, BMASK(6:4)|BMASK(2:0)); // phy2p1_3_32[2:0]: reg_agc_dlev_eh_sel; [6:4]: reg_agc_dfe_eh_sel
    msWrite2Byte(REG_PHY2P1_3_P0_33_L + u32PHY2P1BankOffset, 0x4180); // phy2p1_3_33[2:0]: reg_dfe_ew_check_time; [3]: reg_dfe_ew_check_en; [4]: reg_sweep_ew_disable; [5]: reg_sweep_eh_disable; [6]: reg_sweep_mode_direction; [7]: reg_auto_eq_gray_enable; [10:8]: reg_sweep_check_time
    msWrite2Byte(REG_PHY2P1_3_P0_37_L + u32PHY2P1BankOffset, 0x001F); // phy2p1_3_37[6:0]: reg_active_cable_eye_target
    //msWrite2Byte(REG_PHY2P1_3_P0_3D_L + u32PHY2P1BankOffset, 0x0190); // phy2p1_3_3D[15:0]: reg_data_div40_clk_thr1
    msWrite2ByteMask(REG_PHY2P1_3_P0_3F_L + u32PHY2P1BankOffset, 0x00E9, BMASK(11:0)); // phy2p1_3_3F[11:0]: reg_data_div40_clk_thr_tolrnt
    // z-type
    msWrite2Byte(REG_PHY2P1_3_P0_4D_L + u32PHY2P1BankOffset, 0x0000); // phy2p1_3_4D[2:0][6:4][10:8][14:12]: reg_eq_code_mode_a/b/c/d
    msWrite2Byte(REG_PHY2P1_4_P0_5A_L + u32PHY2P1BankOffset, 0x6C63); // phy2p1_4_5A[5]: reg_auto_vcorst_en, phy4_5a[15:12] reg_dig_lck_cnt_power
    msWrite2Byte(REG_PHY2P1_4_P0_5B_L + u32PHY2P1BankOffset, 0x1033); // phy4_5b[3:0] reg_dig_lck_range, phy4_5b[9:4] reg_dig_lose_range
    msWrite2Byte(REG_PHY2P1_4_P0_5E_L + u32PHY2P1BankOffset, 0x0351); // phy4_5e[3:0] reg_dig_lck_range_power, phy4_5e[7:4] reg_dig_lose_range_power, phy4_5e[8] reg_lck_range_sel, phy4_5e[9] reg_lose_range_sel
    msWrite2Byte(REG_PHY2P1_4_P0_60_L + u32PHY2P1BankOffset, 0x090A); // phy4_60[11:0] reg_clk_valid_l
    /*if (MDrv_SYS_GetChipRev() == 0)
    {
        msWrite2Byte(REG_PHY2P1_4_P0_08_L + u32PHY2P1BankOffset, 0x6655); // phy4_08[2:0] reg_hd20_gc_divsel_pll_l0_band_3
        msWrite2ByteMask(REG_PHY2P1_4_P0_0C_L +u32PHY2P1BankOffset, 0x1100, BMASK(15:8)); //phy4_0x0c[14:12]= 0x1, h20_dvisel_post_band0
    }
    else
    {
        msWrite2Byte(REG_PHY2P1_4_P0_08_L + u32PHY2P1BankOffset, 0x6555); // phy4_08[2:0] reg_hd20_gc_divsel_pll_l0_band_3
        msWrite2ByteMask(REG_PHY2P1_4_P0_0C_L +u32PHY2P1BankOffset, 0x1000, BMASK(15:8)); //phy4_0x0c[14:12]= 0x1, h20_dvisel_post_band0
    }*/



    //msWrite2ByteMask(REG_PHY2P1_4_P0_0C_L +u32PHY2P1BankOffset, 0x1000, BMASK(15:8));


    msWrite2ByteMask(REG_PHY2P1_4_P0_62_L + u32PHY2P1BankOffset, 0x9A00, BMASK(15:8));
    msWrite2ByteMask(REG_PHY2P1_4_P0_1F_L + u32PHY2P1BankOffset, 0x0700, BMASK(15:8));
    msWrite2Byte(REG_PHY2P1_4_P0_71_L + u32PHY2P1BankOffset, 0); // phy2p1_4_71
    msWrite2Byte(REG_PHY2P1_4_P0_72_L + u32PHY2P1BankOffset, 0); // phy2p1_4_72

    msWrite2ByteMask(REG_PHY2P1_3_P0_15_L + u32PHY2P1BankOffset, BIT(15), BIT(15)); // phy2p1_3_15[15]: reg_en_clk_rx2tx

    // 2.0 port setting >>>>>>>>>
    msWrite2Byte(REG_PHY2P1_4_P0_40_L + u32PHY2P1BankOffset, 0x0202); // phy2p1_4_40[5:0]: reg_hd20_gc_ictrl_pd_wide_l0_band_3; [13:8]: reg_hd20_gc_ictrl_pd_wide_l0_band_2
    msWrite2Byte(REG_PHY2P1_4_P0_41_L + u32PHY2P1BankOffset, 0x0202); // phy2p1_4_41[5:0]: reg_hd20_gc_ictrl_pd_wide_l0_band_1; [13:8]: reg_hd20_gc_ictrl_pd_wide_l0_band_0
    msWrite2Byte(REG_PHY2P1_4_P0_42_L + u32PHY2P1BankOffset, 0x0202); // phy2p1_4_42[5:0]: reg_hd14_gc_ictrl_pd_wide_l0_band_6; [13:8]: reg_hd14_gc_ictrl_pd_wide_l0_band_5
    msWrite2Byte(REG_PHY2P1_4_P0_43_L + u32PHY2P1BankOffset, 0x0808); // phy2p1_4_43[5:0]: reg_hd14_gc_ictrl_pd_wide_l0_band_4; [13:8]: reg_hd14_gc_ictrl_pd_wide_l0_band_3
    msWrite2Byte(REG_PHY2P1_4_P0_44_L + u32PHY2P1BankOffset, 0x0808); // phy2p1_4_44[5:0]: reg_hd14_gc_ictrl_pd_wide_l0_band_2; [13:8]: reg_hd14_gc_ictrl_pd_wide_l0_band_1
    msWrite2Byte(REG_PHY2P1_4_P0_45_L + u32PHY2P1BankOffset, 0x0208); // phy2p1_4_45[5:0]: reg_hd14_gc_ictrl_pd_wide_l0_band_0; [13:8]: reg_hd21_gc_ictrl_pd_wide_l0_12g
    msWrite2Byte(REG_PHY2P1_4_P0_46_L + u32PHY2P1BankOffset, 0x0202); // phy2p1_4_46[5:0]: reg_hd21_gc_ictrl_pd_wide_l0_10g; [13:8]: reg_hd21_gc_ictrl_pd_wide_l0_8g
    //msWrite2Byte(REG_PHY2P1_4_P0_47_L + u32PHY2P1BankOffset, 0x0202); // phy2p1_4_47[5:0]: reg_hd21_gc_ictrl_pd_wide_l0_6g; [13:8]: reg_hd21_gc_ictrl_pd_wide_l0_3g
    msWrite2Byte(REG_PHY2P1_4_P0_57_L + u32PHY2P1BankOffset, 0xC65A);

    msWrite2ByteMask(REG_PHY2P1_3_P0_50_L + u32PHY2P1BankOffset, 0, BIT(4)); // phy2p1_3_50[4]: reg_swap_dcdr_updn_polity
    msWrite2ByteMask(REG_PHY2P1_3_P0_6A_L + u32PHY2P1BankOffset, 0, BIT(0)); // phy2p1_3_6A[0]: reg_en_dvi_clk_div

    msWrite2Byte(REG_PHY2P1_3_P0_7E_L + u32PHY2P1BankOffset, 0x0081); // phy2p1_3_7E[9:0]: reg_dlev_max_table
    msWrite2ByteMask(REG_PHY2P1_3_P0_77_L + u32PHY2P1BankOffset, 0x0002, BIT(1));
    msWrite2ByteMask(REG_PHY2P1_3_P0_7D_L + u32PHY2P1BankOffset, 0x0001, BIT(0));

    msWrite2Byte(REG_PHY2P1_5_P0_52_L + u32PHY2P1BankOffset, 0x1111); // phy5_52[2:0] reg_under_bit_select_l0, phy5_52[6:4] reg_under_bit_select_l1, phy5_52[10:8] reg_under_bit_select_l2, phy5_52[14:12] reg_under_bit_select_l3
    msWrite2Byte(REG_PHY2P1_5_P0_53_L +u32PHY2P1BankOffset, 0x5d5d); // phy5_53[6:0] reg_aaba_pattern_l0, phy5_53[14:8] reg_aaba_pattern_l1
    msWrite2Byte(REG_PHY2P1_5_P0_54_L +u32PHY2P1BankOffset, 0x5d5d); // phy5_54[6:0] reg_aaba_pattern_l2, phy5_54[14:8] reg_aaba_pattern_l3

    msWrite2Byte(REG_PHY2P1_4_P0_58_L +u32PHY2P1BankOffset, 0x042A);
    msWrite2Byte(REG_PHY2P1_4_P0_59_L +u32PHY2P1BankOffset, 0x0320);
    msWriteByte(REG_PHY2P1_4_P0_5A_H +u32PHY2P1BankOffset, 0x6c);
    msWrite2Byte(REG_PHY2P1_4_P0_5B_L +u32PHY2P1BankOffset, 0x1033);
    msWrite2Byte(REG_PHY2P1_4_P0_5E_L +u32PHY2P1BankOffset, 0x0351);
    msWrite2Byte(REG_PHY2P1_4_P0_60_L +u32PHY2P1BankOffset, 0x090a);

    // Enable DFE Taps
    // enable dfe tap1~tap4 & disable dfe tap5~tap12
    _Hal_tmds_EnableDfeTap(enInputPortType, TRUE, FALSE);

    msWrite2ByteMask(REG_PHY2P1_2_P0_52_L +u32PHY2P1BankOffset, BMASK(6:5), BMASK(6:5));
    msWrite2ByteMask(REG_PHY2P1_2_P0_56_L +u32PHY2P1BankOffset, BMASK(6:5), BMASK(6:5));
    msWrite2ByteMask(REG_PHY2P1_2_P0_5A_L +u32PHY2P1BankOffset, BMASK(6:5), BMASK(6:5));
    msWrite2ByteMask(REG_PHY2P1_2_P0_5E_L +u32PHY2P1BankOffset, BMASK(6:5), BMASK(6:5));
    // <<<<<<<<<< 2.0 port setting

    msWrite2Byte(REG_PHY2P1_2_P0_01_L +u32PHY2P1BankOffset, 0xAAAA); // phy2p1_2_01[15:0]: reg_gc_dfe_mode_l3/2/1/0_ov_en; reg_gc_dfe_mode_l3/2/1/0_ov

   // bring up new added
   //msWrite2ByteMask(REG_PHY2P1_3_P0_06_L +u32PHY2P1BankOffset, 0x00F0, BMASK(7:4));//PHY A top : 1st fifo no need inv
   msWrite2ByteMask(REG_PHY2P1_3_P0_61_L +u32PHY2P1BankOffset, 0x00AA, BMASK(7:0));//PHY D top : 1st fifo  need inv
   msWrite2ByteMask(REG_PHY2P1_3_P0_62_L +u32PHY2P1BankOffset, 0x00AA, BMASK(7:0));//PHY D top : lane a fifo  need inv

   msWrite2ByteMask(REG_PHY2P1_3_P0_66_L +u32PHY2P1BankOffset, 0x000F, BMASK(3:0));//reg_lane_pn_swap[3:0]
   msWrite2ByteMask(REG_PHY2P1_3_P0_68_L +u32PHY2P1BankOffset, 0xF000, BMASK(15:12));

      //FOR CTS test H20, no need these settings, but 1.4 need, DLPF div2
#if(DLPF_DIV2)
   msWrite2ByteMask(REG_PHY2P1_2_P0_74_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
   msWrite2ByteMask(REG_PHY2P1_2_P0_76_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
   msWrite2ByteMask(REG_PHY2P1_2_P0_78_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
   msWrite2ByteMask(REG_PHY2P1_2_P0_7A_L +u32PHY2P1BankOffset, 0x0009, BIT(3)|BIT(0));
#endif

   msWrite2Byte(REG_PHY2P1_3_P0_20_L +u32PHY2P1BankOffset, 0x8038);
   msWrite2Byte(REG_PHY2P1_3_P0_21_L +u32PHY2P1BankOffset, 0x8038);
   msWrite2Byte(REG_PHY2P1_3_P0_22_L +u32PHY2P1BankOffset, 0x8038);
   msWrite2Byte(REG_PHY2P1_3_P0_23_L +u32PHY2P1BankOffset, 0x8038);


   msWrite2ByteMask(REG_PHY2P1_0_P0_06_L + u32PHY2P1BankOffset, BIT(10),BIT(10)); // bit 10 new added for IE

   //for H14 3.5G and 0.25G, disable upper and lower bound
   msWrite2ByteMask(REG_PHY2P1_4_P0_60_L + u32PHY2P1BankOffset, 0x0000,BMASK(11:0) );
   msWrite2ByteMask(REG_PHY2P1_4_P0_61_L + u32PHY2P1BankOffset, 0x0FFF,BMASK(12:0) );

#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
phy_param_init(enInputPortType,FALSE);
#endif

}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_MACInit()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_MACInit(BYTE enInputPortSelect __attribute__ ((unused)))
{
    //DWORD u32PMSCDCBankOffset = 0; //_Hal_tmds_GetPMSCDCBankOffset(enInputPortSelect);
    DWORD u32DTOPBankOffset = 0; //_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);
    //DWORD u32VEBankOffset = 0; //_Hal_tmds_GetVEBankOffset(enInputPortSelect);
    DWORD u32HDMIBankOffset = 0; //_Hal_tmds_GetHDMIBankOffset(enInputPortSelect);

    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_30_L +u32DTOPBankOffset, 0, BIT(15)); //hdmirx_dtop_30[15]: reg_vs_rpt_sel: 0: by watchdog timer; 1: by VSYNC
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_30_L +u32DTOPBankOffset, 0x4, BMASK(3:0)); //hdmirx_dtop_30[3:0]: reg_timer_v_div: timer v divider for div report
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_31_L +u32DTOPBankOffset, BIT(9), BMASK(15:8)); //hdmirx_dtop_31[15:8]: reg_dep_filter_len
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L +u32DTOPBankOffset, BIT(2)|BIT(1), BMASK(7:0)); //hdmirx_dtop_47[2]: reg_dis_sscp_only; [1]: reg_dis_ch_info
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L +u32DTOPBankOffset, BIT(15)|BIT(10), BMASK(15:8)); //hdmirx_dtop_47[15]: reg_scmb_flg_err; [10]: reg_wb_timer_th

    msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_00_L +u32DTOPBankOffset, BIT(1)|BIT(0), BMASK(15:0)); //hdmirx_dtop_pkt_00[1]: reg_auto_rst_dc_fifo; [0]: reg_en_deep_color
    msWrite2ByteMask(REG_HDMI_U0_07_L +u32HDMIBankOffset,0, BMASK(15:8)); //hdmi_u0_00[15:8]
    msWrite2ByteMask(REG_HDMI_U0_0D_L +u32HDMIBankOffset,BIT(9)|BIT(4)|BIT(2)|BIT(0), BMASK(15:0)); //hdmi_u0_0D[15:0]
    msWrite2ByteMask(REG_HDMI_U0_0E_L +u32HDMIBankOffset,BIT(10)|BIT(5)|BIT(3)|BIT(1), BMASK(15:0)); //hdmi_u0_0E[15:0]
    msWrite2ByteMask(REG_HDMI_U0_0F_L +u32HDMIBankOffset,BIT(11)|BIT(6)|BIT(4)|BIT(2)|BIT(0), BMASK(15:0)); //hdmi_u0_0F[15:0]
    msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_13_L +u32DTOPBankOffset,BIT(2), BMASK(7:0)); //hdmi_u0_13[2]: reg_n_0
    msWrite2ByteMask(REG_HDMI_U0_05_L +u32HDMIBankOffset,BIT(7), BMASK(7:0)); //hdmi_u0_0F[15:0]
    msWrite2ByteMask(REG_HDMI_EM_U0_05_L +u32HDMIBankOffset,0, BMASK(7:0));
    msWrite2ByteMask(REG_HDMI_EM_U0_06_L +u32HDMIBankOffset,0xEF, BMASK(7:0));
    msWrite2ByteMask(REG_HDMIRX_VE_U0_05_L +u32HDMIBankOffset,0, BMASK(7:0));

    // Check decode error setting
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_4D_L +u32DTOPBankOffset, BIT(10), BIT(10)); //hdmirx_dtop_4D[10]: reg_dvt_ph
    //macbook HDCP re-auth issue
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_1F_L +u32DTOPBankOffset, BIT(14)|BIT(15), BMASK(15:14));
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_HDMIInit()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_HDMIInit(BYTE enInputPortSelect  __attribute__ ((unused)))
{
#warning "to support hdmi2.0, please make sure: (1)support 5v detect (2) enable scdc: ENABLE_HDMI_SCDC = 1 (3) disable replace mode: ENABLE_HPD_REPLACE_MODE = 0"

    DWORD u32HDMIBankOffset = 0;//_Hal_tmds_GetHDMIBankOffset(u8HDMIInfoSource);

    msWrite2ByteMask(REG_HDMI2_U0_24_L +u32HDMIBankOffset, 0, BIT(10)); // hdmi2_u0_24[10]: reg_pkt_8p_en

    msWrite2ByteMask(REG_HDMI3_U0_71_L +u32HDMIBankOffset, BIT(13), BIT(13)|BIT(12)); // hdmi3_u0_71[12]: reg_aud_mask_en; [13]: reg_aud_mask_ove

    msWrite2ByteMask(REG_HDMI_U0_06_L +u32HDMIBankOffset, BIT(6), BIT(6)); // hdmi_u0_06[6]: reg_en_hbr

    msWrite2ByteMask(REG_HDMI_U0_64_L +u32HDMIBankOffset, BIT(1), BMASK(3:0)); // hdmi_u0_64[1]: Auto detect non-PCM mode; [3]: Auto detect DSD mode

    msWrite2ByteMask(REG_HDMI3_U0_3F_L +u32HDMIBankOffset, 0, BIT(11)); // hdmi3_u0_3F[11]: reg_vsif_ful_shift_en
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_InsertEfuseValue()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_InsertEfuseValue(BYTE enInputPortSelect)
{

    BYTE enSWReadEfuseValue = 0;
    BYTE enEfuseValue = 0;
    BYTE enAddressOffset = 0;

    //get rterm 450hm from efuse for reg_nodie_rt_ctrl1
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            enSWReadEfuseValue = (msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_SW_READ) & BIT(5));
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_45) & 0xF0;
            enAddressOffset = HDMI_EFUSE_P0_ADDRESS_OFFSET;
       	break;

        case HDMI_INPUT_PORT1:
            enSWReadEfuseValue = ( msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_SW_READ) & BIT(1));
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_45) & 0xF0;
            enAddressOffset = HDMI_EFUSE_P1_ADDRESS_OFFSET;
        break;

        case HDMI_INPUT_PORT2:
            enSWReadEfuseValue = (msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P2_RTERM_SW_READ) & BIT(4));
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P2_RTERM_45) & 0x0F;
            enAddressOffset = HDMI_EFUSE_P2_ADDRESS_OFFSET;
        break;

    }

    if(enSWReadEfuseValue)
    {
        msWriteByteMask(REG_DPRX_PHY_PM_03_L + enAddressOffset, enEfuseValue , 0xF0 );
    }

    //get rterm 500hm from efuse for reg_nodie_rt_ctrl2
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            enSWReadEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_SW_READ) & BIT(6);
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_50) & 0x0F;
            enAddressOffset = HDMI_EFUSE_P0_ADDRESS_OFFSET;
        break;

        case HDMI_INPUT_PORT1:
            enSWReadEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_SW_READ) & BIT(2);
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_50) & 0x0F;
            enAddressOffset = HDMI_EFUSE_P1_ADDRESS_OFFSET;
        break;

        case HDMI_INPUT_PORT2:
            enSWReadEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P2_RTERM_SW_READ) & BIT(5);
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P2_RTERM_50) & 0xF0;
            enAddressOffset = HDMI_EFUSE_P2_ADDRESS_OFFSET;
        break;

    }

    if(enSWReadEfuseValue)
    {
        msWriteByteMask(REG_DPRX_PHY_PM_03_H + enAddressOffset, enEfuseValue , 0x0F );
    }

//efuse value select
#if ENABLE_HDMIRX_RTERM_45OHM
    msWriteByteMask(REG_DPRX_PHY_PM_03_H+ enAddressOffset, BIT4, BIT4|BIT5 );  // rterm sel to reg_nodie_rt_ctrl1
#else //efuse value: 500hm
    msWriteByteMask(REG_DPRX_PHY_PM_03_H+ enAddressOffset, BIT5, BIT4|BIT5 );  // rterm sel to reg_nodie_rt_ctrl2
#endif

}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_BusyPolling()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_tmds_BusyPolling(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    BYTE u8SourceVersion = HDMI_SOURCE_VERSION_NOT_SURE;
    BYTE u8FRLRate = 0;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    //DWORD u32PHY2P1PMBankOffset = 0;//_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
    enInputPortType = enInputPortType;

#if 1 // TBD
    if(!pstHDMIPollingInfo->bPowerOnLane)
    {
        _Hal_tmds_InitInternalInfo(pstHDMIPollingInfo);
        //Hal_HDMI_SetPowerDown(enInputPortType, TRUE);  // power down once when AC on
        pstHDMIPollingInfo->bPowerOnLane = TRUE;
    }
#endif

    u8FRLRate =  _Hal_tmds_GetFRLRate(enInputPortType);

    if(pstHDMIPollingInfo->u8FRLRate != u8FRLRate)
    {
        pstHDMIPollingInfo->u8FRLRate = u8FRLRate;
    }
    else if(pstHDMIPollingInfo->u8FRLRate < HDMI_FRL_MODE_NONE)
    {
        HDMI_HAL_DPRINTF("** HDMI FRL detect error: %d \r\n", pstHDMIPollingInfo->u8FRLRate);
        HDMI_HAL_DPRINTF("** port= %d\r\n", enInputPortType);
    }
    else if(_Hal_tmds_GetClockStableFlag(enInputPortType))
    {
        pstHDMIPollingInfo->u8RatioDetect = _Hal_tmds_GetRatioDetect(enInputPortType, pstHDMIPollingInfo);

        if(pstHDMIPollingInfo->u8RatioDetect == HDMI_FRL_MODE_LEGACY_14)
        {
            u8SourceVersion = HDMI_SOURCE_VERSION_HDMI14;
        }
        else if(pstHDMIPollingInfo->u8FRLRate == HDMI_FRL_MODE_LEGACY_14)
        {
            u8SourceVersion = HDMI_SOURCE_VERSION_HDMI14;
        }
        else if(pstHDMIPollingInfo->u8FRLRate == HDMI_FRL_MODE_LEGACY_20)
        {
            u8SourceVersion = HDMI_SOURCE_VERSION_HDMI20;
        }

        pstHDMIPollingInfo->bClockStableFlag = TRUE;
    }
    else if(pstHDMIPollingInfo->bClockStableFlag)
    {
        pstHDMIPollingInfo->bClockStableFlag = FALSE;

        if(pstHDMIPollingInfo->ucSourceVersion != HDMI_SOURCE_VERSION_NOT_SURE)
        {
            pstHDMIPollingInfo->ucSourceVersion = HDMI_SOURCE_VERSION_NOT_SURE;
        }

        // Release FSM when clock unstable for CTS issue
        msWrite2Byte(REG_PHY2P1_0_P0_10_L +u32PHY2P1BankOffset, 0);
        msWrite2Byte(REG_PHY2P1_0_P0_11_L +u32PHY2P1BankOffset, 0);
        msWrite2Byte(REG_PHY2P1_0_P0_12_L +u32PHY2P1BankOffset, 0);
        msWrite2Byte(REG_PHY2P1_0_P0_13_L +u32PHY2P1BankOffset, 0);
        // clear ENC state
        msWrite2ByteMask(REG_HDMIRX_HDCP_P0_1B_L, BIT(9), BIT(9)); // dvi_hdcp_p0_1b[9]: reg_enc_en_status
    }

    if (pstHDMIPollingInfo->bPowerControlOn)
    {
        if (!Hal_HDMI_GetSCDC5vDetectFlag(enInputPortType))
        {
           // Hal_HDMI_SetPowerDown(enInputPortType, TRUE);
            pstHDMIPollingInfo->bPowerControlOn = FALSE;
        }
    }
    else
    {
        if (Hal_HDMI_GetSCDC5vDetectFlag(enInputPortType)&& !(pstHDMIPollingInfo->bForcePowerDown))
        {
            Hal_HDMI_SetPowerDown(enInputPortType, FALSE);
            pstHDMIPollingInfo->bPowerControlOn = TRUE;
        }
        else
        {
            // mark this temperaily: TBD
            //if (R2BYTEMSK(REG_PHY2P1_PM_P0_42_L +u32PHY2P1PMBankOffset, BMASK(11:8)) != 0x0F00)//TBD R2BYTEMSK
            //{
                //msWrite2ByteMask(REG_PHY2P1_PM_P0_42_L +u32PHY2P1PMBankOffset, 0x0F00, BMASK(11:8));
            //}
        }
    }

    if(pstHDMIPollingInfo->ucSourceVersion != u8SourceVersion)
    {
        pstHDMIPollingInfo->ucSourceVersion = u8SourceVersion;
        pstHDMIPollingInfo->bVersionChangeFlag = TRUE;

        if(pstHDMIPollingInfo->ucSourceVersion != HDMI_SOURCE_VERSION_NOT_SURE)
        {
            if((pstHDMIPollingInfo->u8FRLRate == HDMI_FRL_MODE_LEGACY_14) || (pstHDMIPollingInfo->u8FRLRate == HDMI_FRL_MODE_LEGACY_20))
            {
                HDMI_HAL_DPRINTF("** HDMI FRL Rate = 0, port %d\r\n", enInputPortType);
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI FRL Rate = %d, port %d\r\n", pstHDMIPollingInfo->u8FRLRate, HDMI_GET_PORT_SELECT(enInputPortType));
                HDMI_HAL_DPRINTF("** HDMI FRL Rate = %d \r\n", pstHDMIPollingInfo->u8FRLRate);
                HDMI_HAL_DPRINTF("** port= %d\r\n", enInputPortType);

            }

            _Hal_tmds_NewPHYSwitch(enInputPortType, pstHDMIPollingInfo);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_FastTrainingProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_FastTrainingProc(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    Bool bNextStateFlag = FALSE;
    WORD u16temp = 2000;
    BYTE u8EQValue[8] = {0};
    stHDMI_ERROR_STATUS stErrorStatus = {0,0,0,0,0,0,0,0};
    BYTE u8Lane = 0;
    DWORD u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    enInputPortType = enInputPortType;
    MS_U16 de_status;

    do
    {
        switch(pstHDMIPollingInfo->u8FastTrainingState)
        {
            case HDMI_FAST_TRAINING_START:
                pstHDMIPollingInfo->ucSourceVersion = HDMI_SOURCE_VERSION_NOT_SURE;
                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;

                bNextStateFlag = FALSE;
                break;

            case HDMI_FAST_TRAINING_SQUELCH: // HW detect
                if(_Hal_tmds_GetSquelchFlag(enInputPortType))
                {
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;

                    HDMI_HAL_DPRINTF("** HDMI get squelch port %d\r\n", enInputPortType);
                }

                bNextStateFlag = FALSE;
                break;

            case HDMI_FAST_TRAINING_CHECK_AUTO_EQ:
                {
                    BYTE u8ClockRange = _Hal_tmds_GetH14AutoEQEnable(enInputPortType, Hal_tmds_GetClockRatePort(enInputPortType, pstHDMIPollingInfo->ucSourceVersion, HDMI_SIGNAL_PIX_MHZ));

                    if(u8ClockRange == HDMI_14_TMDS_CLOCK_OVER_135M)
                    {
                        HDMI_HAL_DPRINTF("** HDMI 14 OVER_135M enable auto EQ port %d\r\n", enInputPortType);

                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_ACTIVE_CABLE;
                    }
                    else if(u8ClockRange == HDMI_14_TMDS_CLOCK_75M_TO_135M)
                    {
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CLEAR_ERROR_H14_75M_TO_135M;
                        pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE0;
                        pstHDMIPollingInfo->u16CheckErrorCount = 0;

                        HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M disable auto EQ port %d\r\n", enInputPortType);
                    }
                    else if (u8ClockRange == HDMI_14_TMDS_CLOCK_UNDER_75M)
                    {
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                        HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M disable auto EQ port %d\r\n", enInputPortType);
                    }

                    bNextStateFlag = TRUE;
                }

                break;

            case HDMI_FAST_TRAINING_CHECK_ACTIVE_CABLE:
                if(_Hal_tmds_CheckActiveCableProc(enInputPortType, pstHDMIPollingInfo))
                {
                    if(!pstHDMIPollingInfo->bActiveCableFlag)
                    {
                        if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
                        {
                            pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H14;
                            pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H14_CASE0;

                            for(u8Lane = 0; u8Lane < 4; u8Lane++)
                            {
                                pstHDMIPollingInfo->bEQError0[u8Lane] = FALSE;
                                pstHDMIPollingInfo->bEQError1[u8Lane] = FALSE;
                                pstHDMIPollingInfo->bEQError2[u8Lane] = FALSE;
                            }
                        }
                        else if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20)
                        {
                            pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20;
                            pstHDMIPollingInfo->u8ChangePHYCase = HDMI_CHANGE_PHY_H20_CASE0;
                        }

                        pstHDMIPollingInfo->u16CheckErrorCount = 0;
                        bNextStateFlag = FALSE;
                        if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
                        {
                            // Overwrite FSM
                            msWrite2Byte(REG_PHY2P1_0_P0_11_L +u32PHY2P1BankOffset, 0x0080);
                            msWrite2Byte(REG_PHY2P1_0_P0_12_L +u32PHY2P1BankOffset, 0x0080);
                            msWrite2Byte(REG_PHY2P1_0_P0_13_L +u32PHY2P1BankOffset, 0x0080);

                            msWrite2ByteMask(REG_PHY2P1_0_P0_1B_L +u32PHY2P1BankOffset, 0x100, BMASK(11:8)); // reg_eq_trg_sw_0
                        }
                        else
                        {
                            _Hal_tmds_ResetDLPF(enInputPortType); // reset DLPF before auto EQ re-triger
                            _Hal_tmds_NewPHYAutoEQTrigger(enInputPortType, pstHDMIPollingInfo, BMASK(11:8));
                        }
                        msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0010, BMASK(7:4));

                        HDMI_HAL_DPRINTF("** HDMI not active cable port %d\r\n", enInputPortType);
                    }
                    else
                    {
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                        bNextStateFlag = TRUE;

                        HDMI_HAL_DPRINTF("** HDMI active cable port %d\r\n", enInputPortType);
                    }
                }
                else
                {
                    bNextStateFlag = FALSE;
                }

                break;

            case HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H14:

#if 1 // trigger EQ only
                if((msRead2Byte(REG_PHY2P1_0_P0_14_L +u32PHY2P1BankOffset) & 0xFF) == 0x80)
#else
                if(_Hal_tmds_GetEQFlowDoneFlag(enInputPortType, 0x1))
#endif
                {
                    _Hal_tmds_GetNewPHYEQValue(enInputPortType, u8EQValue);
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L0 Gray = 0x%x\r\n",u8EQValue[0] );
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L0(Binary) =(%d)\r\n",u8EQValue[4] );
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L1 Gray = 0x%x\r\n",u8EQValue[1] );
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L1(Binary) = (%d)\r\n", u8EQValue[5] );
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L2 Gray = 0x%x\r\n",u8EQValue[2]);
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L2(Binary) = (%d)\r\n", u8EQValue[6]);
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L3 Gray = 0x%x\r\n", u8EQValue[3]);
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: L3 (Binary) = (%d)\r\n", u8EQValue[7]);
                    HDMI_HAL_DPRINTF("** HDMI 14 EQ done value: port %d\r\n", enInputPortType);

                    pstHDMIPollingInfo->u8EQValue0[0] = u8EQValue[4];
                    pstHDMIPollingInfo->u8EQValue0[1] = u8EQValue[4];
                    pstHDMIPollingInfo->u8EQValue0[2] = u8EQValue[4];
                    pstHDMIPollingInfo->u8EQValue0[3] = u8EQValue[4];

                    pstHDMIPollingInfo->u8EQValue1[0] = ((pstHDMIPollingInfo->u8EQValue0[0] + 5) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[0] + 5);
                    pstHDMIPollingInfo->u8EQValue1[1] = ((pstHDMIPollingInfo->u8EQValue0[1] + 5) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[1] + 5);
                    pstHDMIPollingInfo->u8EQValue1[2] = ((pstHDMIPollingInfo->u8EQValue0[2] + 5) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[2] + 5);
                    pstHDMIPollingInfo->u8EQValue1[3] = ((pstHDMIPollingInfo->u8EQValue0[3] + 5) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[3] + 5);

                    pstHDMIPollingInfo->u8EQValue2[0] = ((pstHDMIPollingInfo->u8EQValue0[0] + 10) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[0] + 10);
                    pstHDMIPollingInfo->u8EQValue2[1] = ((pstHDMIPollingInfo->u8EQValue0[1] + 10) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[1] + 10);
                    pstHDMIPollingInfo->u8EQValue2[2] = ((pstHDMIPollingInfo->u8EQValue0[2] + 10) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[2] + 10);
                    pstHDMIPollingInfo->u8EQValue2[3] = ((pstHDMIPollingInfo->u8EQValue0[3] + 10) > 30)? 30: (pstHDMIPollingInfo->u8EQValue0[3] + 10);

                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CLEAR_ERROR_H14;
                    bNextStateFlag = TRUE;
                }
                else
                {
                    bNextStateFlag = FALSE;
                }
                break;

            case HDMI_FAST_TRAINING_CLEAR_ERROR_H14:
                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ0 error status. EQ0(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue0[0]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ0 error status. EQ1(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue0[1]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ0 error status. EQ2(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue0[2]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ0 error status. EQ3(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue0[3]);
                HDMI_HAL_DPRINTF(" port %d\r\n",enInputPortType);

                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_EQ0_H14;
                bNextStateFlag = FALSE;

                break;

            case HDMI_FAST_TRAINING_CHECK_EQ0_H14:
                if(!Hal_HDMI_GetDEStableStatus(enInputPortType))
                {
                    pstHDMIPollingInfo->bEQError0[0] = TRUE;
                    pstHDMIPollingInfo->bEQError0[1] = TRUE;
                    pstHDMIPollingInfo->bEQError0[2] = TRUE;
                }
                else if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    if((stErrorStatus.u16DecodeErrorL0 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL0)
                    {
                        pstHDMIPollingInfo->bEQError0[0] = TRUE;
                    }
                    if((stErrorStatus.u16DecodeErrorL1 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL1)
                    {
                        pstHDMIPollingInfo->bEQError0[1] = TRUE;
                    }
                    if((stErrorStatus.u16DecodeErrorL2 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL2)
                    {
                        pstHDMIPollingInfo->bEQError0[2] = TRUE;
                    }
                }

#if(!HDMI_PHY_OLD_PATCH)
                for(u8Lane = 0; u8Lane < 4; u8Lane++)
                {
                    // set EQ0 (autoEQ value)
                    u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue0[u8Lane];
                }

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                pstHDMIPollingInfo->u8ClearErrorStatusCount = 1;

                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_STABLE_H14;
#else
                // set EQ1
                for(u8Lane = 0; u8Lane < 4; u8Lane++)
                {
                    u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue1[u8Lane];
                }

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }
                //MHAL_HDMIRX_MSG_INFO("** HDMI 14 clear EQ1 error status. EQ1(Binary) = %d/%d/%d/%d port %d\r\n", pstHDMIPollingInfo->u8EQValue1[0], pstHDMIPollingInfo->u8EQValue1[1], pstHDMIPollingInfo->u8EQValue1[2], pstHDMIPollingInfo->u8EQValue1[3], HDMI_GET_PORT_SELECT(enInputPortType));

                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ1 error status. EQ0(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue1[0]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ1 error status. EQ1(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue1[1]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ1 error status. EQ2(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue1[2]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ1 error status. EQ3(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue1[3]);
                HDMI_HAL_DPRINTF(" port %d\r\n",enInputPortType);

                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_EQ1_H14;
#endif
                bNextStateFlag = FALSE;

                break;

            case HDMI_FAST_TRAINING_CHECK_EQ1_H14:
                if(!Hal_HDMI_GetDEStableStatus(enInputPortType))
                {
                    pstHDMIPollingInfo->bEQError1[0] = TRUE;
                    pstHDMIPollingInfo->bEQError1[1] = TRUE;
                    pstHDMIPollingInfo->bEQError1[2] = TRUE;
                }
                else if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    if((stErrorStatus.u16DecodeErrorL0 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL0)
                    {
                        pstHDMIPollingInfo->bEQError1[0] = TRUE;
                    }
                    if((stErrorStatus.u16DecodeErrorL1 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL1)
                    {
                        pstHDMIPollingInfo->bEQError1[1] = TRUE;
                    }
                    if((stErrorStatus.u16DecodeErrorL2 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL2)
                    {
                        pstHDMIPollingInfo->bEQError1[2] = TRUE;
                    }
                }

                // set EQ2
                for(u8Lane = 0; u8Lane < 4; u8Lane++)
                {
                    u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue2[u8Lane];
                }

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                //MHAL_HDMIRX_MSG_INFO("** HDMI 14 clear EQ2 error status. EQ2(Binary) = %d/%d/%d/%d port %d\r\n", pstHDMIPollingInfo->u8EQValue2[0], pstHDMIPollingInfo->u8EQValue2[1], pstHDMIPollingInfo->u8EQValue2[2], pstHDMIPollingInfo->u8EQValue2[3], HDMI_GET_PORT_SELECT(enInputPortType));
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ2 error status. EQ0(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue2[0]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ2 error status. EQ1(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue2[1]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ2 error status. EQ2(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue2[2]);
                HDMI_HAL_DPRINTF("** HDMI 14 clear EQ2 error status. EQ3(Binary) = %d\r\n", pstHDMIPollingInfo->u8EQValue2[3]);
                HDMI_HAL_DPRINTF(" port %d\r\n",enInputPortType);

                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_EQ2_H14;
                bNextStateFlag = FALSE;

                break;

            case HDMI_FAST_TRAINING_CHECK_EQ2_H14:
                if(!Hal_HDMI_GetDEStableStatus(enInputPortType))
                {
                    pstHDMIPollingInfo->bEQError2[0] = TRUE;
                    pstHDMIPollingInfo->bEQError2[1] = TRUE;
                    pstHDMIPollingInfo->bEQError2[2] = TRUE;
                }
                else if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    if((stErrorStatus.u16DecodeErrorL0 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL0)
                    {
                        pstHDMIPollingInfo->bEQError2[0] = TRUE;
                    }
                    if((stErrorStatus.u16DecodeErrorL1 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL1)
                    {
                        pstHDMIPollingInfo->bEQError2[1] = TRUE;
                    }
                    if((stErrorStatus.u16DecodeErrorL2 > HDMI_DECODE_ERROR_THRESHOLD14) || stErrorStatus.bWordBoundaryErrorL2)
                    {
                        pstHDMIPollingInfo->bEQError2[2] = TRUE;
                    }
                }
                for(u8Lane = 0; u8Lane < 4; u8Lane++)
                {
                    if((!pstHDMIPollingInfo->bEQError0[u8Lane]) && (!pstHDMIPollingInfo->bEQError1[u8Lane]) && (!pstHDMIPollingInfo->bEQError2[u8Lane]))
                    {
                        // set EQ1 = EQ0 (autoEQ value) + 5
                        u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue1[u8Lane];
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 EQ0/1/2 no error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);
                    }
                    else if((!pstHDMIPollingInfo->bEQError0[u8Lane]) && (!pstHDMIPollingInfo->bEQError1[u8Lane]) && (pstHDMIPollingInfo->bEQError2[u8Lane]))
                    {
                        // set (EQ0 (autoEQ value) + EQ1) / 2
                        u8EQValue[u8Lane] = (pstHDMIPollingInfo->u8EQValue0[u8Lane] + pstHDMIPollingInfo->u8EQValue1[u8Lane]) / 2;

                        //MHAL_HDMIRX_MSG_INFO("** HDMI 14 Lane%d EQ0/1 no error, select EQ(Binary) = %d port %d\r\n", u8Lane, u8EQValue[u8Lane], HDMI_GET_PORT_SELECT(enInputPortType));
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 EQ0/1 no error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);

                    }
                    else if((pstHDMIPollingInfo->bEQError0[u8Lane]) && (!pstHDMIPollingInfo->bEQError1[u8Lane]) && (!pstHDMIPollingInfo->bEQError2[u8Lane]))
                    {
                        // set (EQ1 + EQ2) / 2
                        u8EQValue[u8Lane] = (pstHDMIPollingInfo->u8EQValue1[u8Lane] + pstHDMIPollingInfo->u8EQValue2[u8Lane]) / 2;

                        //MHAL_HDMIRX_MSG_INFO("** HDMI 14 Lane%d EQ1/2 no error, select EQ(Binary) = %d port %d\r\n", u8Lane, u8EQValue[u8Lane], HDMI_GET_PORT_SELECT(enInputPortType));
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 EQ 1/2 no error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);

                    }
                    else if((!pstHDMIPollingInfo->bEQError0[u8Lane]) && (pstHDMIPollingInfo->bEQError1[u8Lane]) && (!pstHDMIPollingInfo->bEQError2[u8Lane]))
                    {
                        // set EQ0 (autoEQ value)
                        u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue0[u8Lane];

                        //MHAL_HDMIRX_MSG_INFO("** HDMI 14 Lane%d EQ0/2 no error, select EQ(Binary) = %d port %d\r\n", u8Lane, u8EQValue[u8Lane], HDMI_GET_PORT_SELECT(enInputPortType));
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 EQ 0/2 no error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);

                    }
                    else if((!pstHDMIPollingInfo->bEQError0[u8Lane]) && (pstHDMIPollingInfo->bEQError1[u8Lane]) && (pstHDMIPollingInfo->bEQError2[u8Lane]))
                    {
                        // set EQ0 (autoEQ value)
                        u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue0[u8Lane];

                        //MHAL_HDMIRX_MSG_INFO("** HDMI 14 Lane%d EQ0 no error, select EQ(Binary) = %d port %d\r\n", u8Lane, u8EQValue[u8Lane], HDMI_GET_PORT_SELECT(enInputPortType));
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 EQ 0 no error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);

                    }
                    else if((pstHDMIPollingInfo->bEQError0[u8Lane]) && (!pstHDMIPollingInfo->bEQError1[u8Lane]) && (pstHDMIPollingInfo->bEQError2[u8Lane]))
                    {
                        // set EQ1
                        u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue1[u8Lane];

                        //MHAL_HDMIRX_MSG_INFO("** HDMI 14 Lane%d EQ1 no error, select EQ(Binary) = %d port %d\r\n", u8Lane, u8EQValue[u8Lane], HDMI_GET_PORT_SELECT(enInputPortType));
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 EQ 1 no error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);

                    }
                    else if((pstHDMIPollingInfo->bEQError0[u8Lane]) && (pstHDMIPollingInfo->bEQError1[u8Lane]) && (!pstHDMIPollingInfo->bEQError2[u8Lane]))
                    {
                        // set EQ2
                        u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue2[u8Lane];

                        //MHAL_HDMIRX_MSG_INFO("** HDMI 14 Lane%d EQ2 no error, select EQ(Binary) = %d port %d\r\n", u8Lane, u8EQValue[u8Lane], HDMI_GET_PORT_SELECT(enInputPortType));
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 EQ 2 no error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);
                    }
                    else
                    {
                        // set EQ0 (autoEQ value)
                        u8EQValue[u8Lane] = pstHDMIPollingInfo->u8EQValue0[u8Lane];

                        //MHAL_HDMIRX_MSG_INFO("** HDMI 14 Lane%d error, select EQ(Binary) = %d port %d\r\n", u8Lane, u8EQValue[u8Lane], HDMI_GET_PORT_SELECT(enInputPortType));
                        HDMI_HAL_DPRINTF("** HDMI 14 Lane%d \r\n", u8Lane);
                        HDMI_HAL_DPRINTF("** HDMI 14 error, select EQ(Binary) = %d \r\n",u8EQValue[u8Lane]);
                        HDMI_HAL_DPRINTF("** HDMI 14 port %d\r\n", enInputPortType);

                    }
                }

                _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_STABLE_H14;
                bNextStateFlag = FALSE;

                break;

            case HDMI_FAST_TRAINING_CHECK_STABLE_H14:
                if(pstHDMIPollingInfo->u8ClearErrorStatusCount > 0)
                {
                    if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                    {
                        while(u16temp--);
                    }
                    pstHDMIPollingInfo->u8ClearErrorStatusCount--;
                    bNextStateFlag = FALSE;
                    break;
                }

                de_status = Hal_HDMI_GetDEStableStatus(enInputPortType);
                if(de_status && !_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))

                {
                    if(pstHDMIPollingInfo->u16CheckErrorCount < TMDS_14_CHECK_ERROR_TIMES)
                    {
                        pstHDMIPollingInfo->u16CheckErrorCount++;
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                        msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0000, BIT(14));
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                        bNextStateFlag = TRUE;
                    }
                }
                else // DE unstable or BCH error
                {
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 14 error happened, decode error: 0x%x, 0x%x, 0x%x port %d\r\n", stErrorStatus.u16DecodeErrorL0, stErrorStatus.u16DecodeErrorL1, stErrorStatus.u16DecodeErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 14 error happened, WB error: %d, %d, %d port %d\r\n", stErrorStatus.bWordBoundaryErrorL0, stErrorStatus.bWordBoundaryErrorL1, stErrorStatus.bWordBoundaryErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened,de_status= %d \r\n", de_status);
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, decode error0: 0x%x \r\n", stErrorStatus.u16DecodeErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, decode error1: 0x%x \r\n", stErrorStatus.u16DecodeErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, decode error2: 0x%x \r\n", stErrorStatus.u16DecodeErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, port %d\r\n",enInputPortType);

                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, WB error0: %d\r\n", stErrorStatus.bWordBoundaryErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, WB error1: %d\r\n", stErrorStatus.bWordBoundaryErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, WB error2: %d\r\n", stErrorStatus.bWordBoundaryErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 14  error happened, port %d\r\n",  enInputPortType);

                    msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x4000, BIT(14));

                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_TRY_PHASEDEC_I_H14;//HDMI_FAST_TRAINING_TRY_NEGATIVE_C_H14;
                    bNextStateFlag = TRUE;
                }
                break;
             //U02 modified             //U02 bringup add
             case HDMI_FAST_TRAINING_TRY_PHASEDEC_I_H14:
                 if(pstHDMIPollingInfo->u8TryPhaseDecI == 0)
                 {
                      HDMI_HAL_DPRINTF("1!!! try phdac i %d\r\n", pstHDMIPollingInfo->u8RetriggerCount20);
                     pstHDMIPollingInfo->u8TryPhaseDecI++;

                     //roger cts lab test
                      msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(0)); // L0 Ineg ove
                      msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(0)); // L1 Ineg ove
                      msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(0)); // L2 Ineg ove
                      msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(0)); // L3 Ineg ove

                     ForceDelay1ms(2);
                     msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0700, BMASK(10:8)); // pd_phdac_i
                     ForceDelay1ms(3);
                     msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0000, BMASK(10:8)); // pd_phdac_i

                     pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H14;
                     pstHDMIPollingInfo->u16CheckErrorCount = 0;
                     bNextStateFlag = FALSE;

                 }
                 else
                 {
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_TRY_NEGATIVE_C_H14;
                    bNextStateFlag = TRUE;
                }
                break;

            case HDMI_FAST_TRAINING_TRY_NEGATIVE_C_H14:
                if (pstHDMIPollingInfo->u8TryNegativeC == 0)
                {
                    pstHDMIPollingInfo->u8TryNegativeC++;
                    msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(0));
                    msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(0));
                    msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(0));
                    msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(0));

                    bNextStateFlag = FALSE; // wait 10ms
                }
                else if (pstHDMIPollingInfo->u8TryNegativeC == 1)
                {
                    pstHDMIPollingInfo->u8TryNegativeC++;
                    _Hal_tmds_SetNewPHYEQValue(enInputPortType, FALSE, NULL);

                    for(u8Lane = 0; u8Lane < 4; u8Lane++)
                    {
                        pstHDMIPollingInfo->bEQError0[u8Lane] = FALSE;
                        pstHDMIPollingInfo->bEQError1[u8Lane] = FALSE;
                        pstHDMIPollingInfo->bEQError2[u8Lane] = FALSE;
                    }

                    pstHDMIPollingInfo->u16CheckErrorCount = 0;
                    _Hal_tmds_ResetDLPF(enInputPortType); // reset DLPF before auto EQ re-triger

                    // Overwrite FSM
                    msWrite2Byte(REG_PHY2P1_0_P0_11_L +u32PHY2P1BankOffset, 0x0080);
                    msWrite2Byte(REG_PHY2P1_0_P0_12_L +u32PHY2P1BankOffset, 0x0080);
                    msWrite2Byte(REG_PHY2P1_0_P0_13_L +u32PHY2P1BankOffset, 0x0080);

                    msWrite2ByteMask(REG_PHY2P1_0_P0_1B_L +u32PHY2P1BankOffset, 0x100, BMASK(11:8)); // reg_eq_trg_sw_0
                    msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0020, BMASK(7:4));

                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H14;
                    bNextStateFlag = FALSE;
                }
                else
                { // Try Negative C still fail
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHANGE_PHY_H14;
                    bNextStateFlag = TRUE;
                }
                break;
#if 0

            case HDMI_FAST_TRAINING_CLEAR_ERROR_H14_UNDER_75M:
                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_STABLE_H14_UNDER_75M;
                pstHDMIPollingInfo->u16CheckErrorCount = 0;
                bNextStateFlag = FALSE;

                break;

            case HDMI_FAST_TRAINING_CHECK_STABLE_H14_UNDER_75M:

                de_status = Hal_HDMI_GetDEStableStatus(enInputPortType);
                if(de_status && !_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    if(pstHDMIPollingInfo->u16CheckErrorCount < TMDS_14_CHECK_ERROR_TIMES)
                    {
                        pstHDMIPollingInfo->u16CheckErrorCount++;
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                        HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M check finish. retrigger %d\r\n", pstHDMIPollingInfo->u8RetriggerH14Under75M);
                        msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0000, BIT(14));
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                        bNextStateFlag = TRUE;
                    }
                }
                else // DE unstable or BCH error
                {
                    msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x4000, BIT(14));



                    if (pstHDMIPollingInfo->u8RetriggerH14Under75M < HDMI_14_RETRIG_UNDER_75M_TIMES  )
                    {
                        pstHDMIPollingInfo->u8RetriggerH14Under75M++;
                        //_Hal_tmds_Toggle2XMode(enInputPortType);
                        //phy2_1a[4]
                        u16temp1 = msRead2Byte(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset);
                        if( (u16temp1 & BIT(4)) == BIT(4))
                    {
                            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset,0x0000,BIT(4));
                    }
                    else
                    {
                            msWrite2ByteMask(REG_PHY2P1_2_P0_1A_L +u32PHY2P1BankOffset,BIT(4),BIT(4));
                    }


                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CLEAR_ERROR_H14_UNDER_75M;
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, decode error0: 0x%x \r\n", stErrorStatus.u16DecodeErrorL0);
                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, decode error1: 0x%x \r\n", stErrorStatus.u16DecodeErrorL1);
                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, decode error2: 0x%x \r\n", stErrorStatus.u16DecodeErrorL2);
                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, port %d\r\n",enInputPortType);

                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, WB error0: %d\r\n", stErrorStatus.bWordBoundaryErrorL0);
                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, WB error1: %d\r\n", stErrorStatus.bWordBoundaryErrorL1);
                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, WB error2: %d\r\n", stErrorStatus.bWordBoundaryErrorL2);
                       HDMI_HAL_DPRINTF("** HDMI 14 UNDER_75M  error happened, port %d\r\n",  enInputPortType);
                       pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                       bNextStateFlag = TRUE;
                    }
                }


                break;
#endif


            case HDMI_FAST_TRAINING_CHANGE_PHY_H14:
                if(_Hal_tmds_ChangePHYCase(enInputPortType, pstHDMIPollingInfo))
                {
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                    bNextStateFlag = TRUE;
                }
                else
                {
                    if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                    {
                        while(u16temp--);
                    }

                    pstHDMIPollingInfo->u16CheckErrorCount = 0;
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_STABLE_H14;

                    bNextStateFlag = FALSE;
                }

                break;

            case HDMI_FAST_TRAINING_CLEAR_ERROR_H14_75M_TO_135M:
                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M clear error status port %d\r\n", enInputPortType);

                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_STABLE_H14_75M_TO_135M;
                bNextStateFlag = FALSE;

                break;

            case HDMI_FAST_TRAINING_CHECK_STABLE_H14_75M_TO_135M:
                if(Hal_HDMI_GetDEStableStatus(enInputPortType) &&
                    (!_Hal_tmds_GetErrorCountStatus(enInputPortType, FALSE, &stErrorStatus)))
                {
                    if(pstHDMIPollingInfo->u16CheckErrorCount < TMDS_14_CHECK_ERROR_TIMES)
                    {
                        pstHDMIPollingInfo->u16CheckErrorCount++;
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                        msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0000, BIT(14));
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                        bNextStateFlag = TRUE;
                    }
                }
                else // DE unstable or BCH error
                {
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 14 75M_TO_135M error happened, decode error: 0x%x, 0x%x, 0x%x port %d\r\n", stErrorStatus.u16DecodeErrorL0, stErrorStatus.u16DecodeErrorL1, stErrorStatus.u16DecodeErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 14 75M_TO_135M error happened, WB error: %d, %d, %d port %d\r\n", stErrorStatus.bWordBoundaryErrorL0, stErrorStatus.bWordBoundaryErrorL1, stErrorStatus.bWordBoundaryErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));
                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, decode error0: 0x%x \r\n", stErrorStatus.u16DecodeErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, decode error1: 0x%x \r\n", stErrorStatus.u16DecodeErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, decode error2: 0x%x \r\n", stErrorStatus.u16DecodeErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, port %d\r\n",enInputPortType);

                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, WB error0: %d\r\n", stErrorStatus.bWordBoundaryErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, WB error1: %d\r\n", stErrorStatus.bWordBoundaryErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, WB error2: %d\r\n", stErrorStatus.bWordBoundaryErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 14 75M_TO_135M  error happened, port %d\r\n",  enInputPortType);

                    msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x4000, BIT(14));

#if(!HDMI_PHY_OLD_PATCH)
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
#else
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHANGE_PHY_H14_75M_TO_135M;
#endif
                    bNextStateFlag = TRUE;
                }

                break;

            case HDMI_FAST_TRAINING_CHANGE_PHY_H14_75M_TO_135M:
                if(_Hal_tmds_ChangePHYCase(enInputPortType, pstHDMIPollingInfo))
                {
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                    bNextStateFlag = TRUE;
                }
                else
                {
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CLEAR_ERROR_H14_75M_TO_135M;

                    bNextStateFlag = FALSE;
                }

                break;

            case HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20:
                if(_Hal_tmds_GetEQFlowDoneFlag(enInputPortType, 0x7))
                {
                    _Hal_tmds_GetNewPHYEQValue(enInputPortType, u8EQValue);
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 20 EQ done value: L0/L1/L2/L3 Gray(Binary) = 0x%x(%d)/0x%x(%d)/0x%x(%d)/0x%x(%d), port %d\r\n", u8EQValue[0], u8EQValue[4], u8EQValue[1], u8EQValue[5], u8EQValue[2], u8EQValue[6], u8EQValue[3], u8EQValue[7], HDMI_GET_PORT_SELECT(enInputPortType));
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L0 Gray = 0x%x\r\n",u8EQValue[0] );
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L0(Binary) =(%d)\r\n",u8EQValue[4] );
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L1 Gray = 0x%x\r\n",u8EQValue[1] );
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L1(Binary) = (%d)\r\n", u8EQValue[5] );
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L2 Gray = 0x%x\r\n",u8EQValue[2]);
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L2(Binary) = (%d)\r\n", u8EQValue[6]);
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L3 Gray = 0x%x\r\n", u8EQValue[3]);
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: L3 (Binary) = (%d)\r\n", u8EQValue[7]);
                    HDMI_HAL_DPRINTF("** HDMI 20 EQ done value: port %d\r\n", enInputPortType);


                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CLEAR_ERROR_H20;
                    pstHDMIPollingInfo->u8ClearErrorStatusCount = 0;

                    bNextStateFlag = TRUE;
                }
                else
                {
                    bNextStateFlag = FALSE;
                }
                break;


            case HDMI_FAST_TRAINING_CLEAR_ERROR_H20:
                if(_Hal_tmds_GetErrorCountStatus(enInputPortType, TRUE, &stErrorStatus))
                {
                    while(u16temp--);
                }

                if(pstHDMIPollingInfo->u8ClearErrorStatusCount < TMDS_CLEAR_ERROR_STATUS_COUNT)
                {
                    pstHDMIPollingInfo->u8ClearErrorStatusCount++;
                    bNextStateFlag = FALSE;
                }
                else
                {
                    HDMI_HAL_DPRINTF("** HDMI 20 clear error status port %d\r\n", enInputPortType);

                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_STABLE_H20;
                    bNextStateFlag = TRUE;
                }
                break;

            case HDMI_FAST_TRAINING_CHECK_STABLE_H20:
                de_status = Hal_HDMI_GetDEStableStatus(enInputPortType);
                if(de_status && !_Hal_tmds_GetErrorCountStatus(enInputPortType, TRUE, &stErrorStatus))

                {
                    if(pstHDMIPollingInfo->u16CheckErrorCount < TMDS_20_CHECK_ERROR_TIMES)
                    {
                        pstHDMIPollingInfo->u16CheckErrorCount++;
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                        msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x0000, BIT(14));
                        bNextStateFlag = TRUE;
                    }
                }
                else // DE unstable or BCH error
                {
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 20 error happened, decode error: 0x%x, 0x%x, 0x%x port %d\r\n", stErrorStatus.u16DecodeErrorL0, stErrorStatus.u16DecodeErrorL1, stErrorStatus.u16DecodeErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));
                    //MHAL_HDMIRX_MSG_INFO("** HDMI 20 error happened, WB error: %d, %d, %d port %d\r\n", stErrorStatus.bWordBoundaryErrorL0, stErrorStatus.bWordBoundaryErrorL1, stErrorStatus.bWordBoundaryErrorL2, HDMI_GET_PORT_SELECT(enInputPortType));
                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, de_status = %d \r\n", de_status);

                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, decode error0: 0x%x \r\n", stErrorStatus.u16DecodeErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, decode error1: 0x%x \r\n", stErrorStatus.u16DecodeErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, decode error2: 0x%x \r\n", stErrorStatus.u16DecodeErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, port %d\r\n",enInputPortType);


                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, WB error0: %d\r\n", stErrorStatus.bWordBoundaryErrorL0);
                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, WB error1: %d\r\n", stErrorStatus.bWordBoundaryErrorL1);
                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, WB error2: %d\r\n", stErrorStatus.bWordBoundaryErrorL2);
                    HDMI_HAL_DPRINTF("** HDMI 20 error happened, port %d\r\n",  enInputPortType);

                    msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x4000, BIT(14));

#if(!HDMI_PHY_OLD_PATCH)
                    if(pstHDMIPollingInfo->u8RetriggerCount20 < TMDS_20_RETRIGGER_TIMES)
                    {
                    	  //HDMI_HAL_DPRINTF("0!!! %d\r\n", pstHDMIPollingInfo->u8RetriggerCount20);
                        pstHDMIPollingInfo->u8RetriggerCount20++;
                        _Hal_tmds_ResetDLPF(enInputPortType); // reset DLPF before auto EQ re-triger
                        _Hal_tmds_NewPHYAutoEQTrigger(enInputPortType, pstHDMIPollingInfo, BMASK(11:8));

                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20;
                        pstHDMIPollingInfo->u16CheckErrorCount = 0;
                        bNextStateFlag = FALSE;
                    }
                    else if(pstHDMIPollingInfo->u8RetriggerCount20 < (TMDS_20_RETRIGGER_TIMES+10) && (((pstHDMIPollingInfo->u8RetriggerCount20)&0x01)==0x0))
                    {
                    	 HDMI_HAL_DPRINTF("1!!! try phdac i %d\r\n", pstHDMIPollingInfo->u8RetriggerCount20);

                        //roger cts lab test
                         msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, 0, BIT(0)); // L0 Ineg ove
                         msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, 0, BIT(0)); // L1 Ineg ove
                         msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, 0, BIT(0)); // L2 Ineg ove
                         msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, 0, BIT(0)); // L3 Ineg ove

                        pstHDMIPollingInfo->u8RetriggerCount20++;
                        msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0700, BMASK(10:8)); // pd_phdac_i
                        ForceDelay1ms(2);
                        msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0000, BMASK(10:8)); // pd_phdac_i

                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20;
                        pstHDMIPollingInfo->u16CheckErrorCount = 0;
                        bNextStateFlag = FALSE;

                    }
                    else if(pstHDMIPollingInfo->u8RetriggerCount20 < (TMDS_20_RETRIGGER_TIMES+10) && (((pstHDMIPollingInfo->u8RetriggerCount20)&0x01)==0x1))
                    {

                        HDMI_HAL_DPRINTF("!!! try NegC %d\r\n", pstHDMIPollingInfo->u8RetriggerCount20);

                       pstHDMIPollingInfo->u8RetriggerCount20++;

                       //roger cts lab test
                        msWrite2ByteMask(REG_PHY2P1_2_P0_61_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L0 Ineg ove
                        msWrite2ByteMask(REG_PHY2P1_2_P0_64_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L1 Ineg ove
                        msWrite2ByteMask(REG_PHY2P1_2_P0_67_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L2 Ineg ove
                        msWrite2ByteMask(REG_PHY2P1_2_P0_6A_L +u32PHY2P1BankOffset, BIT(0), BIT(0)); // L3 Ineg ove

                       pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20;
                       pstHDMIPollingInfo->u16CheckErrorCount = 0;
                       bNextStateFlag = FALSE;

                    }
                    else
                    {
       	                HDMI_HAL_DPRINTF("2!!! %d\r\n", pstHDMIPollingInfo->u8RetriggerCount20);
                        ForceDelay1ms(4);
                        //MT9700 H20 one lane decode error patch
                        msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0700, BMASK(10:8)); // pd_phdac_i
                        ForceDelay1ms(4);
                        msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset, 0x0000, BMASK(10:8)); // pd_phdac_i
                        pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_FINISH;
                        bNextStateFlag = TRUE;
                    }
#else
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHANGE_PHY_H20;
                    bNextStateFlag = TRUE;
#endif
                }
                break;

            case HDMI_FAST_TRAINING_CHANGE_PHY_H20:
                if(_Hal_tmds_ChangePHYCase(enInputPortType, pstHDMIPollingInfo))
                {
                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_NONE;
                    bNextStateFlag = FALSE;
                }
                else
                {
                    _Hal_tmds_NewPHYAutoEQTrigger(enInputPortType, pstHDMIPollingInfo, BMASK(11:8));

                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20;
                    bNextStateFlag = FALSE;
                }

                break;

            case HDMI_FAST_TRAINING_FINISH:
                _Hal_tmds_GetNewPHYEQValue(enInputPortType, u8EQValue);
                //MHAL_HDMIRX_MSG_INFO("** HDMI SW patch finish. EQ value: L0/L1/L2/L3 Gray(Binary) = 0x%x(%d)/0x%x(%d)/0x%x(%d)/0x%x(%d), port %d\r\n", u8EQValue[0], u8EQValue[4], u8EQValue[1], u8EQValue[5], u8EQValue[2], u8EQValue[6], u8EQValue[3], u8EQValue[7], HDMI_GET_PORT_SELECT(enInputPortType));
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L0 Gray = 0x%x\r\n",u8EQValue[0] );
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L0(Binary) =(%d)\r\n",u8EQValue[4] );
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L1 Gray = 0x%x\r\n",u8EQValue[1] );
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L1(Binary) = (%d)\r\n", u8EQValue[5] );
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L2 Gray = 0x%x\r\n",u8EQValue[2]);
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L2(Binary) = (%d)\r\n", u8EQValue[6]);
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L3 Gray = 0x%x\r\n", u8EQValue[3]);
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: L3 (Binary) = (%d)\r\n", u8EQValue[7]);
                HDMI_HAL_DPRINTF("**  HDMI SW patch finish. EQ done value: port %d\r\n", enInputPortType);

                msWrite2ByteMask(REG_PHY2P1_1_P0_35_L +u32PHY2P1BankOffset, 0x8000, BIT(15));

                _Hal_tmds_CheckTxDcBalanceEn(enInputPortType, TRUE);
                pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_STOP;
                bNextStateFlag = FALSE;

                break;

            case HDMI_FAST_TRAINING_NONE:
                bNextStateFlag = FALSE;

                break;

            default:
                bNextStateFlag = FALSE;
                break;
        }
    }while(bNextStateFlag);
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_GetFreeSyncInfo()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE Hal_HDMI_GetFreeSyncInfo(BYTE enInputPortSelect, DWORD u32PacketStatus, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)))
{
	BYTE u8temp = 0;
	BYTE u8PacketData = 0;
	BYTE u8PacketContent[6] = {0};
	BYTE u8FreeSyncInfo = 0;

	if(u32PacketStatus & HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG)
	{
		for(u8temp = 0; u8temp < 6; u8temp++)
		{
			if(Hal_HDMI_get_packet_value(enInputPortSelect, PKT_SPD, u8temp, &u8PacketData))
			{
				u8PacketContent[u8temp] = u8PacketData;
			}
		}

		if((u8PacketContent[0] == 0x1A) && (u8PacketContent[1] == 0x00) && (u8PacketContent[2] == 0x00))
		{
			if(u8PacketContent[5] & BIT(0))
			{
				u8FreeSyncInfo = u8FreeSyncInfo| HDMI_FREE_SYNC_SUPPORTED;
			}

			if(u8PacketContent[5] & BIT(1))
			{
				u8FreeSyncInfo = u8FreeSyncInfo| HDMI_FREE_SYNC_ENABLED;
			}

			if(u8PacketContent[5] & BIT(2))
			{
				u8FreeSyncInfo = u8FreeSyncInfo| HDMI_FREE_SYNC_ACTIVE;
			}
		}
	}

	return u8FreeSyncInfo;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_SetDEStableFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_SetDEStableFlag(BYTE enInputPortSelect, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo, DWORD u32PacketStatus, Bool bStable)
{
	Bool bVRRFlag = FALSE;
	Bool bFreeSyncFlag = FALSE;
	DWORD u32DTOPBankOffset = 0; //_Hal_tmds_GetDTOPBankOffset(enInputPortType);

	if((u32PacketStatus & HDMI_STATUS_EM_PACKET_RECEIVE_FLAG) && (pstHDMIPollingInfo->u16EMPacketInfo & HDMI_EM_PACKET_VRR_ENABLE))
	{
		bVRRFlag = TRUE;
	}

	if(Hal_HDMI_GetFreeSyncInfo(enInputPortSelect, u32PacketStatus, pstHDMIPollingInfo))
	{
		bFreeSyncFlag = TRUE;
	}

	if(bStable && (bVRRFlag || bFreeSyncFlag))
	{
		msWrite2ByteMask(REG_HDMIRX_DTOP_P0_31_L +u32DTOPBankOffset, BIT(2), BIT(2)); // hdmirx_dtop_31[2]: reg_dep_fsm_pause

		if(!(msRead2Byte(REG_HDMIRX_DTOP_P0_30_L + u32DTOPBankOffset) & BIT(5))) // hdmirx_dtop_30[5]: reg_de_stable
		{
			msWrite2ByteMask(REG_HDMIRX_DTOP_P0_31_L +u32DTOPBankOffset, 0, BIT(2)); // hdmirx_dtop_31[2]: reg_dep_fsm_pause
		}
	}
	else
	{
		msWrite2ByteMask(REG_HDMIRX_DTOP_P0_31_L +u32DTOPBankOffset, 0, BIT(2)); // hdmirx_dtop_31[2]: reg_dep_fsm_pause
	}
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_UpdateErrorStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_UpdateErrorStatus(BYTE enInputPortSelect  __attribute__ ((unused)), ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

    // DE change
    if(msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_10_L + u32DTOPBankOffset) & BIT(12)) // hdmirx_dtop_pkt_10[12]: reg_depack_dec_irq_status_0 -The number of DE pixel changed
    {
        pstHDMIPollingInfo->bDEChangeFlag = TRUE;

        msWrite2Byte(REG_HDMIRX_DTOP_PKT_P0_10_L +u32DTOPBankOffset, BIT(12));  // hdmirx_dtop_pkt_10[12]: reg_depack_dec_irq_status_0 -The number of DE pixel changed
    }
    else
    {
        pstHDMIPollingInfo->bDEChangeFlag = FALSE;
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetHDMIModeFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_GetHDMIModeFlag(BYTE enInputPortSelect  __attribute__ ((unused)))
{
    Bool bHDMIModeFlag = FALSE;
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

    if(msRead2Byte(REG_HDMIRX_HDCP_P0_01_L + u32DTOPBankOffset) & BIT(0)) // hdmirx_hdcp_01[0]: reg_status_hdmi_mode_rpt
    {
        bHDMIModeFlag = TRUE;
    }

    return bHDMIModeFlag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetHDCP22IntStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD _Hal_tmds_GetHDCP22IntStatus(BYTE enInputPortSelect  __attribute__ ((unused)))
{
    WORD u16HDCP22Status = 0;
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

    u16HDCP22Status = msRead2Byte(REG_HDMIRX_HDCP_P0_66_L +u32DTOPBankOffset);

    if(u16HDCP22Status & BIT(1))
    {
        msWrite2Byte(REG_HDMIRX_HDCP_P0_66_L +u32DTOPBankOffset, BIT(1));
    }

    return u16HDCP22Status;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetYUV420Flag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool _Hal_tmds_GetYUV420Flag(BYTE enInputPortSelect, DWORD *u32PacketStatus)
{
    Bool bYUV420Flag = FALSE;
    DWORD u32PKTStatus = *u32PacketStatus;

    if(u32PKTStatus & HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG) // AVI packet receive
    {
        if((Hal_HDMI_avi_infoframe_info(enInputPortSelect, _BYTE_1, NULL) & BMASK(7:5)) == 0x60)
        {
            bYUV420Flag = TRUE;
        }
    }

    return bYUV420Flag;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_YUV420PHYSetting()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_YUV420PHYSetting(BYTE enInputPortSelect  __attribute__ ((unused)), Bool bYUV420Flag, BYTE u8SourceVersion  __attribute__ ((unused)), BYTE u8Colordepth  __attribute__ ((unused)))
{
	DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

	if(bYUV420Flag)
	{
	    msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L + u32PHY2P1BankOffset, 0, BIT(11)|BMASK(9:8)); //PHY2_2c[11]: disable pixcko md overwrite;PHY2_2c[9:8]:pixcko md set to 8bit mode
		msWrite2ByteMask(REG_PHY2P1_3_P0_07_L + u32PHY2P1BankOffset, BMASK(13:12), BMASK(13:12)); // phy2p1_3_07[12]: reg_en_clko_pix_ov_en; [13]: reg_en_clko_pix_2x_ov_en
		msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L + u32PHY2P1BankOffset, BMASK(5:4), BMASK(5:4)); // phy2p1_2_2E[4]: reg_en_clko_pix_ov; [5]: reg_en_clko_pix_2x_ov
		msWrite2ByteMask(REG_COMBO_GP_TOP_15_L, BIT(4), BIT(4)); // combo_gp_top_15[4]: reg_pix_clk_div2_en
	}
	else
	{
		msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L + u32PHY2P1BankOffset, 0, BIT(11)|BMASK(9:8)); //PHY2_2c[11]: disable pixcko md overwrite;PHY2_2c[9:8]:pixcko md set to 8bit mode
		msWrite2ByteMask(REG_PHY2P1_3_P0_07_L + u32PHY2P1BankOffset, BIT(12), BMASK(13:12)); // phy2p1_3_07[12]: reg_en_clko_pix_ov_en; [13]: reg_en_clko_pix_2x_ov_en
		msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L + u32PHY2P1BankOffset, BIT(4), BMASK(5:4)); // phy2p1_2_2E[4]: reg_en_clko_pix_ov; [5]: reg_en_clko_pix_2x_ov
		msWrite2ByteMask(REG_COMBO_GP_TOP_15_L, 0, BIT(4)); // combo_gp_top_15[4]: reg_pix_clk_div2_en
	}

}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_GetEMPacketInfo()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD _Hal_tmds_GetEMPacketInfo(BYTE enInputPortSelect  __attribute__ ((unused)), DWORD *u32PacketStatus)
{
    WORD u16EMPacketStatus = 0;
    DWORD u32HDMIBankOffset = 0;//_Hal_tmds_GetHDMIBankOffset(u8HDMIInfoSource);
    DWORD u32PKTStatus = *u32PacketStatus;

    if(u32PKTStatus & HDMI_STATUS_EM_PACKET_RECEIVE_FLAG) // AVI packet receive
    {
        u16EMPacketStatus = msRead2Byte(REG_HDMI_EM_U0_02_L +u32HDMIBankOffset) & BMASK(3:0); //hdmirx_emp_02[3:0]

        if(msRead2Byte(REG_HDMI_EM_U0_01_L +u32HDMIBankOffset) & BIT(15)) //hdmirx_emp_01[15]
        {
            u16EMPacketStatus = u16EMPacketStatus | HDMI_EM_PACKET_HDR_ENABLE;
        }
    }

    return u16EMPacketStatus;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_tmds_SignalDetectProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_tmds_SignalDetectProc(BYTE enInputPortSelect, DWORD *u32PacketStatus, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
	Bool bNextStateFlag = FALSE;
	BYTE u8SignalDetectState = HDMI_SIGNAL_DETECT_SQUELCH;
	//DWORD u32PHY2P1BankOffset = 0; // _Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
	BYTE u8Colordepth;

#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
	stHDMI_phy_parm* ptr_phy_parm = &s_hdmi_phy_parm;
#endif

	do
	{
		switch(u8SignalDetectState)
		{
			case HDMI_SIGNAL_DETECT_SQUELCH:
				if(pstHDMIPollingInfo->bClockStableFlag && (!pstHDMIPollingInfo->bVersionChangeFlag))
				{
					if(pstHDMIPollingInfo->bNoInputFlag) //Unstable -> stable
					{
						pstHDMIPollingInfo->bNoInputFlag = FALSE;
						pstHDMIPollingInfo->bTimingStableFlag = FALSE;
						pstHDMIPollingInfo->bActiveCableFlag = FALSE;
						pstHDMIPollingInfo->bResetDCFifoFlag = FALSE;
						pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_START;
                                          pstHDMIPollingInfo->ucCheckErrorInterval = 0;

						HDMI_HAL_DPRINTF("** HDMI clock stable port %d\r\n", enInputPortSelect);

						if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20)
						{
							HDMI_HAL_DPRINTF("** HDMI get version 2.0 port %d\r\n", enInputPortSelect);

							pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_ACTIVE_CABLE;
							pstHDMIPollingInfo->u8RetriggerCount20 = 0;
							pstHDMIPollingInfo->bHDMI20Flag = TRUE;
						}
						else if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
						{
							HDMI_HAL_DPRINTF("** HDMI get version 1.4 port %d\r\n", enInputPortSelect);

							pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_AUTO_EQ;
							pstHDMIPollingInfo->u8TryNegativeC = 0;
                            pstHDMIPollingInfo->u8TryPhaseDecI = 0;
							pstHDMIPollingInfo->bHDMI20Flag = FALSE;
						}

						if(_Hal_tmds_GetClockChangeFlag(enInputPortSelect, pstHDMIPollingInfo))
						{
							// clear clock big change
						}
#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
                        if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
                        {
                            // ASSERT(ptr_phy_parm->u8_analog_ov_en==0);
                            if (ptr_phy_parm->u8_analog_ov_en) {
                                HDMI_HAL_DPRINTF("*** Error, ov_en_fail_3, u8_analog_ov_en shall be 0, might cause ill lock,%x\n",ptr_phy_parm->u8_analog_ov_en);
                            }
                            if(_Hal_HDMI_Fix_Analog_Setting(enInputPortSelect,TRUE)){
                                ptr_phy_parm->u8_analog_ov_en = 1;
                            }
                        }
#endif

					}

					bNextStateFlag = TRUE;
				}
				else
				{
					if(!pstHDMIPollingInfo->bNoInputFlag) //Stable to unstable
					{
						pstHDMIPollingInfo->bNoInputFlag = TRUE;
						pstHDMIPollingInfo->bDEChangeFlag = TRUE;
						//_Hal_tmds_ClearEDRVaildFlag(pstHDMIPollingInfo->ucHDMIInfoSource);

						// hdmirx_dtop_35[15] = 1 reg_dep_de_det_vsync_en (default setting, fix)
						//_Hal_tmds_SetDEStableFlag(enInputPortSelect, pstHDMIPollingInfo, &u32PacketStatus, FALSE);
						//Hal_HDCP_GetAuthVersion(enInputPortType, FALSE); // Clear HDCP22 interrupt status
						Hal_HDMI_Software_Reset(enInputPortSelect, HDMI_SW_RESET_HDMI|HDMI_SW_RESET_HDCP); // reset packet status

						HDMI_HAL_DPRINTF("** HDMI clock unstable port %d\r\n", enInputPortSelect);
		                        /*
		                        CTS hdcp14 2c-01 to 2c-02 fail due to bstatus hdmi mode trigger.
		                        [0]: dvi sw reset
		                        [1]: mhl sw reset
		                        [2]: hdcp sw reset
		                        [3]: hdmi sw reset
		                        [4]: power saving sw reset
		                        */
		                        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_3E_L, 0x0005, 0xFFFF); //dvi_dtop_p0_3E: dtop module sw reset
		                        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_3E_L, 0, 0xFFFF); // dvi_dtop_p0_3E: dtop module sw reset

#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
		                        if (ptr_phy_parm->u8_analog_ov_en)
		                        {
		                            if(_Hal_HDMI_Fix_Analog_Setting(enInputPortSelect,FALSE)){
		                                ptr_phy_parm->u8_analog_ov_en = 0;
		                            }
		                        }
#endif

					}

					if(pstHDMIPollingInfo->bVersionChangeFlag)
					{
						pstHDMIPollingInfo->bVersionChangeFlag = FALSE;
					}

					if(pstHDMIPollingInfo->u8FastTrainingState != HDMI_FAST_TRAINING_STOP)
					{
						pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_STOP;
					}

					bNextStateFlag = FALSE;
				}
				break;

			case HDMI_SIGNAL_DETECT_CLOCK_STABLE:
				if(_Hal_tmds_GetClockChangeFlag(enInputPortSelect, pstHDMIPollingInfo))
				{
					pstHDMIPollingInfo->bNoInputFlag = TRUE;
					pstHDMIPollingInfo->bDEChangeFlag = TRUE;

					// hdmirx_dtop_35[15] = 1 reg_dep_de_det_vsync_en (default setting, fix)
					//_Hal_tmds_SetDEStableFlag(enInputPortType, pstHDMIPollingInfo, u32PacketStatus, FALSE);

					HDMI_HAL_DPRINTF("** HDMI clock changed, switch to no input port %d\r\n", enInputPortSelect);

					pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_START;
					HDMI_HAL_DPRINTF("** HDMI clock change trigger FT port %d\r\n", enInputPortSelect);

#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
		                    if (ptr_phy_parm->u8_analog_ov_en)
		                    {
		                        if(_Hal_HDMI_Fix_Analog_Setting(enInputPortSelect,FALSE)){
		                            ptr_phy_parm->u8_analog_ov_en = 0;
		                        }
		                    }
#endif

					bNextStateFlag = FALSE;
				}
				else
				{
					bNextStateFlag = TRUE;
				}

				break;

			case HDMI_SIGNAL_DETECT_DATA_ENABLE:
				if(Hal_HDMI_GetDEStableStatus(enInputPortSelect))
				{
				   //Add de stable flag
				    pstHDMIPollingInfo->bDEStableFlag = TRUE;
					if(!pstHDMIPollingInfo->bResetDCFifoFlag)
					{
						Hal_HDMI_pkt_reset(enInputPortSelect, REST_DEEP_COLOR_FIFO);
						pstHDMIPollingInfo->bResetDCFifoFlag = TRUE;
					}
					// hdmirx_dtop_35[15] = 1 reg_dep_de_det_vsync_en (default setting, fix)
#if 0
					if(!pstHDMIPollingInfo->bDEChangeFlag)
					{
						_Hal_tmds_SetDEStableFlag(enInputPortSelect, pstHDMIPollingInfo, u32PacketStatus, TRUE);
					}
					else
					{
						_Hal_tmds_SetDEStableFlag(enInputPortSelect, pstHDMIPollingInfo, u32PacketStatus, FALSE);
					}
#endif

                    if(!pstHDMIPollingInfo->bTimingStableFlag)
                    {
                        Hal_HDMI_pkt_reset(enInputPortSelect, REST_AVMUTE);
                        pstHDMIPollingInfo->bTimingStableFlag = TRUE;
                    }

					_Hal_tmds_UpdateErrorStatus(enInputPortSelect, pstHDMIPollingInfo);

					bNextStateFlag = TRUE;
				}
				else // DE unstable
				{
					// hdmirx_dtop_35[15] = 1 reg_dep_de_det_vsync_en (default setting, fix)
					//_Hal_tmds_SetDEStableFlag(enInputPortType, pstHDMIPollingInfo, u32PacketStatus, FALSE);
				    pstHDMIPollingInfo->bDEStableFlag = FALSE;

					if(pstHDMIPollingInfo->bYUV420Flag)
					{
						pstHDMIPollingInfo->bYUV420Flag = FALSE;

						//_Hal_tmds_YUV420PHYSetting(enInputPortSelect, pstHDMIPollingInfo->bYUV420Flag, pstHDMIPollingInfo->ucSourceVersion, pstHDMIPollingInfo->u8Colordepth);

						HDMI_HAL_DPRINTF("** HDMI back to HDMI 2.0 port %d\r\n", enInputPortSelect);
					}

					bNextStateFlag = FALSE;
				}
				break;

			case HDMI_SIGNAL_DETECT_HDMI_MODE:
				if(_Hal_tmds_GetHDMIModeFlag(enInputPortSelect))
				{
					if(!pstHDMIPollingInfo->bHDMIModeFlag)
					{
						pstHDMIPollingInfo->bHDMIModeFlag = TRUE;
					}
				}
				else if(pstHDMIPollingInfo->bHDMIModeFlag)
				{
					pstHDMIPollingInfo->bHDMIModeFlag = FALSE;
					Hal_HDMI_pkt_reset(enInputPortSelect, REST_HDMI_STATUS);
				}

				bNextStateFlag = TRUE;

				break;

			case HDMI_SIGNAL_DETECT_HDCP_STATUS:
				if(Hal_HDCP_getstatus(enInputPortSelect) & BIT(8)) // AKSV interrupt status: HDCP1.4
				{
					Hal_HDCP_ClearStatus(enInputPortSelect, BIT(0));

					pstHDMIPollingInfo->ucHDCPState = HDMI_HDCP_1_4;

					//MHAL_HDMIRX_MSG_INFO("** HDMI HDCP state 1.4, port %d\r\n", (enInputPortType -INPUT_PORT_DVI0));
				}
				else if(_Hal_tmds_GetHDCP22IntStatus(enInputPortSelect) & BIT(1)) // AKE_Init interrupt status: HDCP2.2
				{
					pstHDMIPollingInfo->ucHDCPState = HDMI_HDCP_2_2;

					//MHAL_HDMIRX_MSG_INFO("** HDMI HDCP state 2.2, port %d\r\n", (enInputPortType -INPUT_PORT_DVI0));
				}

				bNextStateFlag = TRUE;

				break;

			case HDMI_SIGNAL_DETECT_AUDIO_MUTE_EVENT:  //change to audio status
				Hal_HDMI_Audio_Status_Clear(NULL);

				bNextStateFlag = TRUE;

				break;

			case HDMI_SIGNAL_DETECT_YUV420:
				if(_Hal_tmds_GetYUV420Flag(enInputPortSelect, u32PacketStatus))
				{
					u8Colordepth = Hal_HDMI_GetGCPColorDepth(enInputPortSelect, u32PacketStatus);
					if((pstHDMIPollingInfo->u8Colordepth != u8Colordepth) || (!pstHDMIPollingInfo->bYUV420Flag))
					{
						pstHDMIPollingInfo->bYUV420Flag = TRUE;
						pstHDMIPollingInfo->u8Colordepth = u8Colordepth;
						//_Hal_tmds_YUV420PHYSetting(enInputPortSelect, pstHDMIPollingInfo->bYUV420Flag, pstHDMIPollingInfo->ucSourceVersion, pstHDMIPollingInfo->u8Colordepth);
						HDMI_HAL_DPRINTF("** HDMI get YUV420 port %d\r\n", enInputPortSelect);
					}
				}
				else if(pstHDMIPollingInfo->bYUV420Flag)
				{
					pstHDMIPollingInfo->bYUV420Flag = FALSE;

					//_Hal_tmds_YUV420PHYSetting(enInputPortSelect, pstHDMIPollingInfo->bYUV420Flag, pstHDMIPollingInfo->ucSourceVersion, pstHDMIPollingInfo->u8Colordepth);

					HDMI_HAL_DPRINTF("** HDMI disable YUV420 port %d\r\n", enInputPortSelect);
				}

				bNextStateFlag = TRUE;

				break;

			case HDMI_SIGNAL_DETECT_EMP:
				pstHDMIPollingInfo->u16EMPacketInfo = _Hal_tmds_GetEMPacketInfo(enInputPortSelect, u32PacketStatus);

				bNextStateFlag = FALSE;

				break;

			default:
				bNextStateFlag = FALSE;
				break;
		};

		u8SignalDetectState++;
	}while(bNextStateFlag);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_hdmiRx_WriteInputPortEDID()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_HDMI_InsertEDIDData(BYTE ucAddress, BYTE ucData)
{
    DWORD u32Timeout = 400; // ~ 550us

    while(DP_EDID_BUSY() && (u32Timeout > 0))
    {
        u32Timeout--;
    }

            msWrite2ByteMask(REG_PM_DDC_21_L,0, BIT(9));
            msWrite2ByteMask(REG_PM_DDC_25_L,(ucAddress << 8), BMASK(14:8));//DDC address port for CPU read/write for ADC_0
            msWrite2ByteMask(REG_PM_DDC_26_L,ucData, BMASK(7:0));//DDC Data Port for cpu write for ADC_0.
            msWrite2ByteMask(REG_PM_DDC_21_L, BIT(13), BIT(13));

            while(msRead2Byte(REG_PM_DDC_3D_L) & BIT(13));//ADC_0 cpu wr busy status (wr for ADC sram)

}

//**************************************************************************
//  [Function Name]:
//                  _mhal_HDMIRx_InternalEDIDEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_HDMI_InternalEDIDEnable(BYTE enInputPortSelect, Bool bDDCEnable)
{
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            msWrite2ByteMask(REG_PM_DDC_22_L, bDDCEnable? BIT15: 0, BIT15); // REG_DDC_BANK_22[15]: D0 DDC enable
            break;

        case HDMI_DDC_1:
            msWrite2ByteMask(REG_PM_DDC_26_L, bDDCEnable? BIT15: 0, BIT15); // REG_DDC_BANK_26[15]: D1 DDC enable
            break;

        case HDMI_DDC_2:
            msWrite2ByteMask(REG_PM_DDC_2C_L, bDDCEnable? BIT7: 0, BIT7); // REG_DDC_BANK_2C[7]: D2 DDC enable
            break;

        default:
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_HDMIRx_EDIDWriteProtectEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_HDMI_EDIDWriteProtectEnable(BYTE enInputPortSelect, Bool bWriteProtect)
{
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            msWrite2ByteMask(REG_PM_DDC_22_L, bWriteProtect? BIT13: 0, BIT13); // REG_DDC_BANK_22[13]: D0 I2C write protect
            break;

        case HDMI_DDC_1:
            msWrite2ByteMask(REG_PM_DDC_26_L, bWriteProtect? BIT13: 0, BIT13); // REG_DDC_BANK_26[13]: D1 I2C write protect
            break;

        case HDMI_DDC_2:
            msWrite2ByteMask(REG_PM_DDC_2C_L, bWriteProtect? BIT5: 0, BIT5); // REG_DDC_BANK_2C[5]: D2 I2C write protect
            break;

        default:
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_HDMI_GetAudioFrequency()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _Hal_HDMI_GetAudioFrequency(ST_HDMI_RX_POLLING_INFO *pstHDMIRxPollingInfo,ST_HDMI_RX_AUDIO_INFO *pstHDMIRxAudioInfo, BYTE enInputPortSelect)
{
    //WORD usAudioFrequency = 0;
    BYTE usAudioFrequency = 0;
    //DWORD ulTMDSClockCount = 0;
    DWORD ulTMDSClockCount = 0;
    DWORD ulAudioCTSVaule = 0;
    DWORD ulAudioNValue = 0;
    //DWORD ulAudioFrequencyValue = 0;
    BYTE ulAudioFrequencyValue = 0;
    //DWORD ulAudioCTSNValue = 1769472; // 216x8192
    DWORD ulAudioCTSNValue;

    if(!pstHDMIRxPollingInfo->bHDMIModeFlag)
    {
        usAudioFrequency = HDMI_AUDIO_FREQUENCY_UNVALID;
    }
    else if(pstHDMIRxPollingInfo->bClockStableFlag )
    {
        // Get PHY rate clock count
        ulTMDSClockCount = Hal_tmds_GetClockRatePort(enInputPortSelect, pstHDMIRxPollingInfo->ucSourceVersion, HDMI_SIGNAL_PIX_MHZ);
        ulTMDSClockCount = 12000/128*ulTMDSClockCount;

        if(pstHDMIRxPollingInfo->ulPacketStatusInfo_Low &HDMI_STATUS_ACR_PACKET_RECEIVE_FLAG)
        {
            // Get audio CTS value
            ulAudioCTSVaule = Hal_HDMI_GetAudioContentInfo(enInputPortSelect, HDMI_AUDIO_CONTENT_CTS);

            //ulAudioCTSVaule = ulAudioCTSVaule *2;

            // Get audio N value
            ulAudioNValue = Hal_HDMI_GetAudioContentInfo(enInputPortSelect, HDMI_AUDIO_CONTENT_N);
        }

        if((ulAudioCTSVaule < 0x10) || (ulAudioNValue < 0x10))
        {
            usAudioFrequency = HDMI_AUDIO_FREQUENCY_UNVALID;
        }
        else
        {
            ulAudioFrequencyValue = (DWORD)((ulTMDSClockCount /128) *ulAudioNValue) /ulAudioCTSVaule;

           #define _1DIV128FS ulAudioCTSVaule/((ulTMDSClockCount+500)/1000*ulAudioNValue)
           ulAudioCTSNValue = (DWORD)65536*_1DIV128FS*216;

            if(diff(32,ulAudioFrequencyValue) < 3)
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_32K;
            }
            else if(diff(44,ulAudioFrequencyValue) < 3)
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_44K;
            }
            else if(diff(48,ulAudioFrequencyValue) < 3)
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_48K;
            }
            else if(diff(88,ulAudioFrequencyValue) < 4)
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_88K;
            }
            else if(diff(96,ulAudioFrequencyValue) < 4)
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_96K;
            }
            else if(diff(176,ulAudioFrequencyValue) < 5)
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_176K;
            }
            else if(diff(192,ulAudioFrequencyValue) < 5)
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_192K;
            }
            else
            {
                usAudioFrequency = HDMI_AUDIO_FREQUENCY_UNVALID;
            }

            if(diff(pstHDMIRxAudioInfo->ulAudioCTSNValue,ulAudioCTSNValue) > 1000)
            {
                //_mhal_hdmiRx_InsertAudioCTSNValue(ulAudioCTSNValue);
                pstHDMIRxAudioInfo->ulAudioCTSNValue = ulAudioCTSNValue;
            }

            if(pstHDMIRxAudioInfo->usAudioFrequency == usAudioFrequency)
            {
                return usAudioFrequency;
            }
        }
    }

    return usAudioFrequency;
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_HDMI_SetAudioFrequency()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_HDMI_SetAudioFrequency(WORD usAudioFrequency)
{
    BYTE ucAPLLDDiv = 0;
    BYTE ucAPLLFBDiv = 6;
    BYTE ucAPLLKP = 0;
    BYTE ucAPLLKM = 1;
    BYTE ucAPLLKN = 1;

    switch(usAudioFrequency)
    {
        case HDMI_AUDIO_FREQUENCY_32K:
            ucAPLLDDiv = 1;
            ucAPLLFBDiv = 6;
            ucAPLLKP = 4;
            ucAPLLKM = 3;
            ucAPLLKN = 0;
            break;

        case HDMI_AUDIO_FREQUENCY_44K:
            ucAPLLDDiv = 1;
            ucAPLLFBDiv = 6;
            ucAPLLKP = 3;
            ucAPLLKM = 2;
            ucAPLLKN = 0;
            break;

        case HDMI_AUDIO_FREQUENCY_48K:
            ucAPLLDDiv = 1;
            ucAPLLFBDiv = 6;
            ucAPLLKP = 3;
            ucAPLLKM = 2;
            ucAPLLKN = 0;
            break;

        case HDMI_AUDIO_FREQUENCY_88K:
            ucAPLLDDiv = 1;
            ucAPLLFBDiv = 6;
            ucAPLLKP = 2;
            ucAPLLKM = 1;
            ucAPLLKN = 0;
            break;

        case HDMI_AUDIO_FREQUENCY_96K:
            ucAPLLDDiv = 1;
            ucAPLLFBDiv = 6;
            ucAPLLKP = 2;
            ucAPLLKM = 1;
            ucAPLLKN = 0;
            break;

        case HDMI_AUDIO_FREQUENCY_176K:
            ucAPLLDDiv = 1;
            ucAPLLFBDiv = 6;
            ucAPLLKP = 1;
            ucAPLLKM = 1;
            ucAPLLKN = 1;
            break;

        case HDMI_AUDIO_FREQUENCY_192K:
            ucAPLLDDiv = 1;
            ucAPLLFBDiv = 6;
            ucAPLLKP = 1;
            ucAPLLKM = 1;
            ucAPLLKN = 1;
            break;

        default:

            break;
    };
             HDMI_HAL_DPRINTF("** TMDS SetAudioFrequency %d\r\n", usAudioFrequency);

            //msWrite2ByteMask(REG_HDMI_COMBO_AUD_CTR_30_L, (ucAPLLFBDiv << 12)| (ucAPLLDDiv << 8), BMASK(15:8));
            //msWrite2ByteMask(REG_HDMI_COMBO_AUD_CTR_31_L, (ucAPLLKN << 8)| (ucAPLLKM << 4)| ucAPLLKP, BMASK(10:0));
}

//**************************************************************************
//  [Function Name]:
//                  _Hal_HDMI_DDCOffset()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD _Hal_HDMI_P1P2DDCOffset(BYTE enInputPort)
{
    WORD wOffset = 0;

    switch(enInputPort)
    {
        default:
        case HDMI_DDC_1:
            wOffset = 0x0000;
            break;

        case HDMI_DDC_2:
            wOffset = 0x300;
            break;
    }

    return wOffset;

}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_Software_Reset()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_Software_Reset(BYTE enInputPortSelect __attribute__ ((unused)), WORD u16Reset)
{
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);
    HDMI_HAL_DPRINTF("** Hal_HDMI_Software_Reset, reset type: %d\r\n", u16Reset);

    if(u16Reset &HDMI_SW_RESET_DVI)//Bit4
    {
        msWrite2Byte(REG_HDMIRX_DTOP_P0_3E_L +u32DTOPBankOffset, BIT(0));//hdmirx_dtop_3E[0]: dvi sw reset (reset HDMI2.0/1.4 decoder)
        msWrite2Byte(REG_HDMIRX_DTOP_P0_3E_L +u32DTOPBankOffset, 0);
    }

    if(u16Reset &HDMI_SW_RESET_HDCP)//Bit5
    {
        msWrite2Byte(REG_HDMIRX_DTOP_P0_3E_L +u32DTOPBankOffset, BIT(2));//hdmirx_dtop_3E[2]: hdcp sw reset (reset HDCP (1p & 8p))
        msWrite2Byte(REG_HDMIRX_DTOP_P0_3E_L +u32DTOPBankOffset, 0);
    }

    if(u16Reset &HDMI_SW_RESET_HDMI)//Bit6
    {
        msWrite2Byte(REG_HDMIRX_DTOP_P0_3E_L +u32DTOPBankOffset, BIT(3));//hdmirx_dtop_3E[3]: hdmi sw reset (reset depacket ( include packet parser ) (1p & 8p))
        msWrite2Byte(REG_HDMIRX_DTOP_P0_3E_L +u32DTOPBankOffset, 0);
    }

    if(u16Reset &HDMI_SW_RESET_DVI2MIU)//Bit10
    {
        msWrite2Byte(REG_COMBO_GP_TOP_34_L, BIT(1));//combo_gp_top_34[1]: reset dvi2miu
        msWrite2Byte(REG_COMBO_GP_TOP_34_L, 0);
    }

    if(u16Reset &HDMI_SW_RESET_AUDIO_FIFO)//Bit15
    {
        msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, BIT(12), BIT(12));//combo_gp_top_49[12]: reg_afifo_rst
        msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, 0, BIT(12));
    }
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_pkt_reset()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_pkt_reset(BYTE enInputPortSelect __attribute__ ((unused)), HDMI_REST_t enResetType)
{
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);
    DWORD u32HDMIBankOffset = 0;//_Hal_tmds_GetHDMIBankOffset(enInputPortSelect);
    HDMI_HAL_DPRINTF("** Hal_HDMI_pkt_reset, reset type: %d\r\n", enResetType);

    if(enResetType &REST_DEEP_COLOR_FIFO)
    {
        msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_00_L +u32DTOPBankOffset, BIT(13), BIT(13));//hdmirx_dtop_pkt_00[13]: reg_reset_dc_fifo
        ForceDelay1ms(1);
        msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_00_L +u32DTOPBankOffset, 0, BIT(13));//hdmirx_dtop_pkt_00[13]: reg_reset_dc_fifo
    }
    else if(enResetType &REST_HDMI_STATUS)
    {
        //reset AVI/GCP/SPD/RSV register status
        msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_16_L +u32DTOPBankOffset, BIT(15), BIT(15));//hdmirx_dtop_pkt_16[15:8]: reg_pkt_rst
        ForceDelay1ms(1);
        msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_16_L +u32DTOPBankOffset, 0, BIT(15));//hdmirx_dtop_pkt_16[15:8]: reg_pkt_rst

        //reset ACP/AUDIO info/audio cs/GMP/HDR/ISRC/MEPG/VBI/VSIF/VSP register status
        msWrite2ByteMask(REG_HDMI_U0_5F_L +u32HDMIBankOffset, BIT(15), BIT(15));//hdmi_u0_5F[15]: reg_pkt_rst
        ForceDelay1ms(1);
        msWrite2ByteMask(REG_HDMI_U0_5F_L +u32HDMIBankOffset, 0, BIT(15));//hdmi_u0_5F[15]: reg_pkt_rst
    }
    else
    {
        msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_16_L +u32DTOPBankOffset, enResetType, BMASK(14:8));//hdmirx_dtop_pkt_16[15:8]: reg_pkt_rst
        ForceDelay1ms(1);
        msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_16_L +u32DTOPBankOffset, 0, BMASK(14:8));//hdmirx_dtop_pkt_16[15:8]: reg_pkt_rst
    }
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDCP_ClearStatus()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
void Hal_HDCP_ClearStatus(BYTE enInputPortSelect __attribute__ ((unused)), WORD usInt)
{
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

	msWrite2Byte(REG_HDMIRX_HDCP_P0_01_L +u32DTOPBankOffset, usInt << 8); // hdmirx_hdcp_01[15:8]
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_Audio_Status_Clear()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_Audio_Status_Clear(ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo __attribute__ ((unused)))
{
	BYTE u8HDMIInfoSource = HDMI_INFO_SOURCE0;
	BYTE ucMuteEvent = msRead2Byte(REG_COMBO_GP_TOP_4A_L) & BMASK(7:0);
	BYTE ucAudioMux = msRead2Byte(REG_COMBO_GP_TOP_4B_L) & BMASK(1:0);
	DWORD u32HDMIBankOffset = 0;

	switch(ucAudioMux)
	{
		case HDMI_AUDIO_PATH0:
			u8HDMIInfoSource = HDMI_INFO_SOURCE0;
			break;

		case HDMI_AUDIO_PATH1:
			u8HDMIInfoSource = HDMI_INFO_SOURCE1;
			break;

		case HDMI_AUDIO_PATH2:
			u8HDMIInfoSource = HDMI_INFO_SOURCE2;
			break;

		case HDMI_AUDIO_PATH3:
			u8HDMIInfoSource = HDMI_INFO_SOURCE3;
			break;
	};

	u32HDMIBankOffset = 0;//_Hal_tmds_GetHDMIBankOffset(u8HDMIInfoSource);

	if(ucMuteEvent & HDMI_CTSN_OVERRANGE)
	{
		if(msRead2Byte(REG_HDMI_U0_02_L +u32HDMIBankOffset) & BIT(6)) // hdmi_u0_02[6]: reg_ctsn_ov_range
		{
			//load current CTS/N value first
			msWrite2Byte(REG_HDMI_U0_12_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_12[15:0]: CTS[15:0] from ACR packet
			msWrite2Byte(REG_HDMI_U0_13_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_13[15:0]: N[15:0] from ACR packet
			msWrite2ByteMask(REG_HDMI_U0_14_L +u32HDMIBankOffset, 0xFF, 0xFF); // hdmi_u0_14[7:4]: N[19:16]; [3:0]: CTS[19:16] from ACR packet

			//update CTS value
			msWrite2Byte(REG_HDMI_U0_0A_L +u32HDMIBankOffset, (msRead2Byte(REG_HDMI_U0_12_L +u32HDMIBankOffset))); // hdmi_u0_0A[15:0]: Limited CTS[15:0]
			msWrite2ByteMask(REG_HDMI_U0_09_L +u32HDMIBankOffset, ((msRead2Byte(REG_HDMI_U0_14_L +u32HDMIBankOffset) & 0x0F) << 8), 0x0F00); // hdmi_u0_09[11:8]: Limited CTS[19:16]

			//update N value
			msWrite2Byte(REG_HDMI_U0_0B_L +u32HDMIBankOffset, (msRead2Byte(REG_HDMI_U0_13_L +u32HDMIBankOffset))); // hdmi_u0_0B[15:0]: Limited N[15:0]
			msWrite2ByteMask(REG_HDMI_U0_09_L +u32HDMIBankOffset, ((msRead2Byte(REG_HDMI_U0_14_L +u32HDMIBankOffset) & 0xF0) << 8), 0xF000); // hdmi_u0_09[15:12]: Limited N[19:16]

			//setup CTS/N range
			msWrite2Byte(REG_HDMI_U0_0C_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_0C[15:8]: N range; [7:0]: CTS range

			//upload new CTS/N value
			msWrite2ByteMask(REG_HDMI_U0_07_L +u32HDMIBankOffset, BIT(15), BIT(15)); // hdmi_u0_07[15]: reg_update_lmt_ctsn

			//clear received CTS/N over range status
			msWrite2ByteMask(REG_HDMI_U0_02_L +u32HDMIBankOffset, BIT(6), BIT(6)); // hdmi_u0_02[6]: reg_ctsn_ov_range (write 1 clear)
		}
	}

	if(ucMuteEvent & HDMI_CLOCK_BIG_CHANGE)
	{
		if(msRead2Byte(REG_HDMI_U0_02_L +u32HDMIBankOffset) & BIT(7)) // hdmi_u0_02[7]: reg_vclk_big_chg
		{
			//clear HDMI video clock freqency big change
			msWrite2ByteMask(REG_HDMI_U0_02_L +u32HDMIBankOffset, BIT(7), BIT(7)); // hdmi_u0_02[7]: reg_vclk_big_chg (write 1 clear)
		}
	}

	if(ucMuteEvent & HDMI_AVMUTE)
	{

	}

	if(ucMuteEvent & HDMI_NO_INPUT_CLOCK)
	{

	}

	if(ucMuteEvent & HDMI_AUDIO_SAMPLE_FLAT_BIT)
	{
		if(msRead2Byte(REG_COMBO_GP_TOP_49_L) & BIT(4)) // combo_gp_top_49[4]: reg_flat_asmp_rcvd
		{
			//clear flat audio sample
			msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, BIT(4), BIT(4)); // combo_gp_top_49[4]: reg_flat_asmp_rcvd (write 1 clear)
		}
	}

	if(ucMuteEvent &  HDMI_AUDIO_PRESENT_BIT_ERROR)
	{
		if(msRead2Byte(REG_HDMI_U0_04_L +u32HDMIBankOffset) & BIT(10)) // hdmi_u0_04[10]: reg_asp_prsnt_bit_err
		{
			//clear audio sample packet present bit error
			msWrite2ByteMask(REG_HDMI_U0_04_L +u32HDMIBankOffset, BIT(10), BIT(10)); // hdmi_u0_04[10]: reg_asp_prsnt_bit_err (write 1 clear)
		}
	}

	if(ucMuteEvent & HDMI_AUDIO_FIFO_OVERFLOW_UNDERFLOW)
	{
		if(msRead2Byte(REG_COMBO_GP_TOP_49_L) & (BIT(9)|BIT(8))) // combo_gp_top_49[9]: reg_afifo12_over; [8]: reg_afifo12_under
		{
			//clear audio fifo overflow/underflow
			msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, BIT(9)|BIT(8), BIT(9)|BIT(8)); // combo_gp_top_49[9]: reg_afifo12_over; [8]: reg_afifo12_under (write 1 clear)
		}
	}

	if(ucMuteEvent & HDMI_AUDIO_SAMPLE_PARITY_ERROR)
	{
		if(msRead2Byte(REG_HDMI_U0_04_L +u32HDMIBankOffset) & (BIT(5)|BIT(4))) // hdmi_dual_04[5]: reg_as_pbit_err; [4]: Audio Sample packet receive Error occurred; sample repeated
		{
			//clear audio sample parity error
			msWrite2ByteMask(REG_HDMI_U0_04_L +u32HDMIBankOffset, BIT(5)|BIT(4), BIT(5)|BIT(4)); // hdmi_dual_04[5]: reg_as_pbit_err; [4]: Audio Sample packet receive Error occurred; sample repeated (write 1 clear)
		}
	}
}


//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_get_packet_value()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool Hal_HDMI_get_packet_value(BYTE enInputPortSelect  __attribute__ ((unused)), MS_HDMI_PACKET_STATE_t u8state, BYTE u8byte_idx, BYTE *pu8Value)
{
	DWORD u32Base_add = 0;
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset((E_MUX_INPUTPORT)(INPUT_PORT_DVI0 +enInputPortSelect));
	DWORD u32HDMIBankOffset = 0;//_Hal_tmds_GetHDMIBankOffset(enInputPortSelect);

	switch(u8state)
	{
		case PKT_MPEG:
			// 1~5
			if(u8byte_idx > 5)
				return FALSE;

			u32Base_add = REG_HDMI_U0_57_L +u32HDMIBankOffset; // hdmi_u0_57[7:0]: reg_mpeg_if1; 59[7:0]: reg_mpeg_if5
			break;

		case PKT_AUI:
			// 1~10
			if(u8byte_idx > 10)
				return FALSE;
			u32Base_add = REG_HDMI3_U0_52_L +u32HDMIBankOffset; // hdmi3_u0_52[7:0]: reg_audio_if01; hdmi3_u0_56[15:8]: reg_audio_if10
			break;

		case PKT_SPD:
			// 1~25
			if(u8byte_idx > 25)
				return FALSE;

			u32Base_add = REG_HDMIRX_DTOP_PKT_P0_30_L +u32DTOPBankOffset; // hdmirx_dtop_pkt_30[7:0]: reg_spd_if1; 3C[7:0]: reg_spd_if25
			break;

		case PKT_AVI:
			// 1~14
			if(u8byte_idx > 13)
				return FALSE;

			u32Base_add = REG_HDMIRX_DTOP_PKT_P0_23_L +u32DTOPBankOffset; // hdmirx_dtop_pkt_23[7:0]: reg_avi_if1; 29[15:8]: reg_avi_if14
			break;

		case PKT_GC:
			// 2
			if(u8byte_idx > 2)
				return FALSE;

			u32Base_add = REG_HDMIRX_DTOP_PKT_P0_19_L +u32DTOPBankOffset; // hdmirx_dtop_pkt_19[15:0]: reg_gcontrol
			break;

		case PKT_ASAMPLE:
			break;

		case PKT_ACR:
			break;

		case PKT_VS:
			// 0~2:header, 4~31:payload
			if(u8byte_idx > 31)
				return FALSE;

			u32Base_add = REG_HDMI_U0_30_L +u32HDMIBankOffset; // hdmi_u0_30[7:0]: reg_vs_hdr0; 31[7:0]: reg_vs_hdr2; 32[7:0]: reg_vs_if1; 3F[15:8]: reg_vs_if28
			break;

		case PKT_NULL:
			break;

		case PKT_ISRC1:
			// 0~15
			u8byte_idx ++;
			if(u8byte_idx > 16)
				return FALSE;

			msWrite2ByteMask(REG_HDMI_U0_07_L, 0, BIT(13)); // hdmi_u0_07[13]: Orignial ISRC packet
			u32Base_add = REG_HDMI_U0_20_L +u32HDMIBankOffset; // hdmi_u0_20[7:0]: reg_isrc_pb0; 27[15:8]: reg_isrc_pb15
			break;

		case PKT_ISRC2:
			// 0~15
			u8byte_idx ++;
			if(u8byte_idx > 16)
				return FALSE;

			u32Base_add = REG_HDMI_U0_28_L +u32HDMIBankOffset; // hdmi_u0_28[7:0]: reg_isrc_pb16; 2F[15:8]: reg_isrc_pb31
			break;

		case PKT_ACP:
			// 0~15
			u8byte_idx ++;
			if(u8byte_idx > 16)
				return FALSE;

			u32Base_add = REG_HDMI_U0_17_L +u32HDMIBankOffset; // hdmi_u0_17[7:0]: reg_acp_pb0; 1E[15:8]: reg_acp_pb15
			break;

		case PKT_ONEBIT_AUD:
			break;

		case PKT_HDR:
			// 0~1: header, 2~29: payload
			if(u8byte_idx > 29)
				return FALSE;

			u32Base_add = REG_HDMI2_U0_60_L +u32HDMIBankOffset; // hdmi2_u0_60[7:0]: reg_hdr_hdr2; [15:8]: reg_hdr_hdr1; 61[7:0]: reg_hdr_pb01; [15:8]: reg_hdr_pb00; 6E[7:0]: reg_hdr_pb27
			break;

		case PKT_MULTI_VS:
			if(u8byte_idx < 120)
			{
				if(u8byte_idx < 30)
				{
					u32Base_add = REG_HDMI3_U0_00_L +u32HDMIBankOffset; // hdmi3_u0_00[7:0]: reg_vsif_hdr2_0; [15:8]: reg_vsif_hdr1_0; 01[7:0]: reg_vsif_pb01_0; [15:8]: reg_vsif_pb00_0; 0E[7:0]: reg_vsif_pb27_0
				}
				else if((u8byte_idx >= 30) && (u8byte_idx < 60))
				{
					u32Base_add = REG_HDMI3_U0_10_L +u32HDMIBankOffset; // hdmi3_u0_10[7:0]: reg_vsif_hdr2_1; [15:8]: reg_vsif_hdr1_1; 11[7:0]: reg_vsif_pb01_1; [15:8]: reg_vsif_pb00_1; 1E[7:0]: reg_vsif_pb27_1
					u8byte_idx = u8byte_idx - 30;
				}
				else if((u8byte_idx >= 60) && (u8byte_idx < 90))
				{
					u32Base_add = REG_HDMI3_U0_20_L +u32HDMIBankOffset; // hdmi3_u0_20[7:0]: reg_vsif_hdr2_2; [15:8]: reg_vsif_hdr1_2; 21[7:0]: reg_vsif_pb01_2; [15:8]: reg_vsif_pb00_2; 2E[7:0]: reg_vsif_pb27_2
					u8byte_idx = u8byte_idx - 60;
				}
				else if((u8byte_idx >= 90) && (u8byte_idx < 120))
				{
					u32Base_add = REG_HDMI3_U0_30_L +u32HDMIBankOffset; // hdmi3_u0_30[7:0]: reg_vsif_hdr2_3; [15:8]: reg_vsif_hdr1_3; 31[7:0]: reg_vsif_pb01_3; [15:8]: reg_vsif_pb00_3; 3E[7:0]: reg_vsif_pb27_3
					u8byte_idx = u8byte_idx - 90;
				}
			}
			break;

		default:
			break;
	}

	*pu8Value = msReadByte(u32Base_add +u8byte_idx);

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_StablePolling()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_StablePolling(BYTE enInputPortSelect, DWORD *ulPacketStatus, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo)
{
    //BYTE enPortIndex = 0;
    //for(enPortIndex = 0; enPortIndex <= 2; enPortIndex++)
    {
        //Hal_tmds_BusyPolling(enPortIndex, &stHDMIPollingInfo[enPortIndex]);
        _Hal_tmds_SignalDetectProc(enInputPortSelect, ulPacketStatus, stHDMIPollingInfo);

        _Hal_tmds_FastTrainingProc(enInputPortSelect, stHDMIPollingInfo);
	}
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_init()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_init(BYTE enInputPortSelect)
{
	msEread_Init(&usTemp1, &ubTemp2);
    //Video initial setting
	msWrite2Byte(REG_COMBO_GP_TOP_10_L, 0); // combo_gp_top_10[15:0]: reg_dtop_clk_pd_p0
	msWrite2Byte(REG_COMBO_GP_TOP_11_L, 0); // combo_gp_top_11[15:0]: reg_dtop_clk_pd_p0
	msWrite2ByteMask(REG_COMBO_GP_TOP_13_L, 0, BMASK(7:0)); // combo_gp_top_13[7:0]: reg_ve_clk_pd_p0
	msWrite2Byte(REG_COMBO_GP_TOP_02_L, 0); //  combo_gp_top_02[15:0]: reg_misc_clk_pd
	msWrite2ByteMask(REG_COMBO_GP_TOP_03_L, 0, BMASK(7:0)); //  combo_gp_top_03[7:0]: reg_misc_clk_pd
	msWrite2Byte(REG_COMBO_GP_TOP_15_L, 0); //  combo_gp_top_15[15:0]

	//Audio initial setting
    msWrite2ByteMask(REG_COMBO_GP_TOP_54_L, 0x0A00, BMASK(11:8)); // combo_gp_top_54[9:8]: reg_aud_fifo_empty_diff; [11:10]: reg_aud_fifo_full_diff
    msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, 0, BIT(6)); // combo_gp_top_49[6]: reg_aud_8p_en

    msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, BIT(12), BIT(12)); // combo_gp_top_49[12]: reg_afifo_rst
    msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, 0, BIT(12)); // combo_gp_top_49[12]: reg_afifo_rst

    // 48[14] = 1 audio fifo underflow don't do HW reset; 48[15] = 1 audio fifo overflow don't do HW reset
    msWrite2ByteMask(REG_COMBO_GP_TOP_48_L, BMASK(15:14), BMASK(15:14)); // combo_gp_top_48[14]: audio fifo underflow don't do hw reset; [15]: audio fifo overflow don't do hw reset
    msWrite2ByteMask(REG_COMBO_GP_TOP_4B_L, BMASK(5:4), BMASK(5:4)); // combo_gp_top_4B[4]: reg_af_rctrl_rstz_roll_back; [5]: reg_read_rdy_dis


    msWrite2ByteMask(REG_COMBO_GP_TOP_44_L, BIT(7)|BIT(0), BIT(7)|BIT(0)); // combo_gp_top_44[0]: reg_synth_sel_cts_ref; [7]: reg_en_cts_n_synth
    //[MT9700]msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, BIT(0), BIT(0)); // combo_gp_top_38[0] : hdmi_hdcp dtop module disable mask
    msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, 1, BIT(0));//[MT9700] msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, 0, BIT(0));

#if(!HDMI_Cable5v_0)
    msWrite2ByteMask(REG_SCDC_P0_0C_L, BIT(3), BMASK(3:2)); //scdc_p0_0c[3:2]: [3]:hpd 5v reset overwrite enable; [2]: hpd 5v reset overwrite value
#endif
#if(!HDMI_Cable5v_1)
    msWrite2ByteMask(REG_SCDC_P1_0C_L, BIT(3), BMASK(3:2)); //scdc_p1_0c[3:2]: [3]:hpd 5v reset overwrite enable; [2]: hpd 5v reset overwrite value
#endif
#if(!HDMI_Cable5v_2)
    msWrite2ByteMask(REG_SCDC_P2_0C_L, BIT(3), BMASK(3:2)); //scdc_p2_0c[3:2]: [3]:hpd 5v reset overwrite enable; [2]: hpd 5v reset overwrite value
#endif

#if 0
	msWrite2Byte(REG_COMBO_GP_TOP_18_L, 0); // combo_gp_top_18[15:0]: reg_dtop_clk_pd_p1
	msWrite2Byte(REG_COMBO_GP_TOP_19_L, 0); // combo_gp_top_19[15:0]: reg_dtop_clk_pd_p1
	msWrite2ByteMask(REG_COMBO_GP_TOP_1B_L, 0, BMASK(7:0)); // combo_gp_top_1B[7:0]: reg_ve_clk_pd_p1
	msWrite2Byte(REG_COMBO_GP_TOP_20_L, 0); // combo_gp_top_20[15:0]: reg_dtop_clk_pd_p2
	msWrite2Byte(REG_COMBO_GP_TOP_21_L, 0); // combo_gp_top_21[15:0]: reg_dtop_clk_pd_p2
	msWrite2ByteMask(REG_COMBO_GP_TOP_23_L, 0, BMASK(7:0)); // combo_gp_top_23[7:0]: reg_ve_clk_pd_p2
	msWrite2Byte(REG_COMBO_GP_TOP_28_L, 0); // combo_gp_top_28[15:0]: reg_dtop_clk_pd_p3
	msWrite2Byte(REG_COMBO_GP_TOP_29_L, 0); // combo_gp_top_29[15:0]: reg_dtop_clk_pd_p3
	msWrite2ByteMask(REG_COMBO_GP_TOP_2B_L, 0, BMASK(7:0)); // combo_gp_top_2B[7:0]: reg_ve_clk_pd_p3

	msWrite2ByteMask(REG_COMBO_GP_TOP_54_L, 0x0A00, BMASK(11:8)); // combo_gp_top_54[9:8]: reg_aud_fifo_empty_diff; [11:10]: reg_aud_fifo_full_diff

	msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, 0, BIT(6)); // combo_gp_top_49[6]: reg_aud_8p_en

	msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, BIT(12), BIT(12)); // combo_gp_top_49[12]: reg_afifo_rst
	msWrite2ByteMask(REG_COMBO_GP_TOP_49_L, 0, BIT(12)); // combo_gp_top_49[12]: reg_afifo_rst

	// 48[14] = 1 audio fifo underflow don't do HW reset; 48[15] = 1 audio fifo overflow don't do HW reset
	msWrite2ByteMask(REG_COMBO_GP_TOP_48_L, BMASK(15:14), BMASK(15:14)); // combo_gp_top_48[14]: audio fifo underflow don't do hw reset; [15]: audio fifo overflow don't do hw reset
	msWrite2ByteMask(REG_COMBO_GP_TOP_4B_L, BMASK(5:4), BMASK(5:4)); // combo_gp_top_4B[4]: reg_af_rctrl_rstz_roll_back; [5]: reg_read_rdy_dis

	msWrite2ByteMask(REG_COMBO_GP_TOP_44_L, BIT(7)|BIT(0), BIT(7)|BIT(0)); // combo_gp_top_44[0]: reg_synth_sel_cts_ref; [7]: reg_en_cts_n_synth
	msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, BIT(0), BIT(0)); // combo_gp_top_38[0] : hdmi_hdcp dtop module disable mask
#endif

	//for(enInputPortSelect = 0; enInputPortSelect <= 2; enInputPortSelect++)
	{
		_Hal_tmds_PHYInit(enInputPortSelect);
		_Hal_tmds_MACInit(enInputPortSelect);

        _Hal_tmds_HDMIInit(enInputPortSelect);
	}

    Hal_HDMI_Audio_MUTE_Enable(0xBF, 0xBF); //enable aud_flat mute event for hdmi2.0 CTS;
    msWrite2ByteMask(REG_COMBO_GP_TOP_4D_L, 0, BIT(6)); // combo_gp_top_4D[6]: reg_hdmi_audio_status_en:audio fifo overflow/underflow for QD980 cts 8-21/23 audio cutting

    msWrite2ByteMask(REG_COMBO_GP_TOP_01_L, BIT(6), BIT(6)); // combo_gp_top_01[6]: DVI2MIU enable
    msWrite2ByteMask(REG_COMBO_GP_TOP_01_L, 0x3000, BMASK(15:12)); // combo_gp_top_01[15:12]:0x3 Enable DVI2MIU select Clock source CLKO_TMDS
#if ENABLE_HDMI_BCHErrorIRQ
    msWrite2ByteMask(REG_HDMI_U0_60_L, BMASK(15:0), BMASK(15:0)); //hdmi_u0_60: req_int_mask (depacket_irq 2-7)
    msWrite2ByteMask(REG_HDMI2_U0_26_L, BMASK(15:8), BMASK(15:8)); //hdmi2_u0_26[15:8]: req_hdmi2_irq_mask(depacket_irq 2-7)
    msWrite2ByteMask(REG_HDMI_EM_U0_08_L, BMASK(15:0), BMASK(15:0)); //hdmi_em_u0_08: req_irq_mask(depacket_irq 2-8)
#endif
    _Hal_tmds_InsertEfuseValue(enInputPortSelect);

    msWrite2ByteMask(REG_HDMI_EM_U0_26_L, BIT(7),BIT(7));

}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_ClockRtermControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_ClockRtermControl(BYTE enInputPortSelect, Bool bPullHighFlag)
{
    switch(enInputPortSelect)
	{
	    case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_DPRX_PHY_PM_00_L, bPullHighFlag? 0: BIT(8), BIT(8));
		break;

		case HDMI_INPUT_PORT1:
			msWrite2ByteMask(REG_DPRX_PHY_PM_20_L, bPullHighFlag? 0: BIT(8), BIT(8));
		break;

		case HDMI_INPUT_PORT2:
			msWrite2ByteMask(REG_DPRX_PHY_PM_40_L, bPullHighFlag? 0: BIT(8), BIT(8));
		break;

		default:
			bPullHighFlag = FALSE;
		break;
	}
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_DataRtermControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_DataRtermControl(BYTE enInputPortSelect, Bool bPullHighFlag)
{
    switch(enInputPortSelect)
	{
	    case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_DPRX_PHY_PM_00_L, bPullHighFlag? 0: BMASK(11:9), BMASK(11:9));
		break;

		case HDMI_INPUT_PORT1:
			msWrite2ByteMask(REG_DPRX_PHY_PM_20_L, bPullHighFlag? 0: BMASK(11:9), BMASK(11:9));
		break;

		case HDMI_INPUT_PORT2:
			msWrite2ByteMask(REG_DPRX_PHY_PM_40_L, bPullHighFlag? 0: BMASK(11:9), BMASK(11:9));
		break;

		default:
			bPullHighFlag = FALSE;
		break;
	}

}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_PHYPowerModeSetting()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_PHYPowerModeSetting(BYTE enInputPortSelect ,BYTE ucPMMode)
{

    MS_U32 u32PHY2P1BankOffset=0;

    switch(ucPMMode)
    {
            case mhal_ePM_POWERON:
                msWriteByteMask(REG_DPRX_PHY_PM_00_H + (0x40*enInputPortSelect) , 0x00, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_01_H + (0x40*enInputPortSelect) , BIT(0), BIT(0)); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + (0x40*enInputPortSelect) , 0xE0, 0xF0); // reg_nodie_en_sqh

                msWriteByteMask(REG_DPRX_PHY_PM_66_L , 0x00, BIT(0)); //NODIE_PD_CLKIN

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(2), BMASK(3:2)); // REG_PD_CLKIN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,0x0, BIT(14)); // REG_PD_BG
                msWrite2ByteMask(REG_PHY2P1_2_P0_04_L +u32PHY2P1BankOffset,BIT(2), BIT(2)); // REG_PD_DLEV_SAFF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_04_L +u32PHY2P1BankOffset,0x8000, BMASK(15:12)); // REG_PD_DLEV_SAFF[3:0]
                msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset,BIT(0), BIT(0)); // REG_PD_DLPF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset,0x0080, BMASK(7:4)); // REG_PD_DLPF[3:0]


                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(4), BIT(4)); // REG_PD_LANE[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x0008, BMASK(3:0)); // REG_PD_LANE[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset,0x0000, BIT(6)); // REG_PD_LDO

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(7), BIT(7)); // REG_PD_PHDAC_I[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x0800, BMASK(11:8)); // REG_PD_PHDAC_I[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(13), BIT(13)); // REG_PD_PHDAC_Q[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x8000, BMASK(15:12)); // REG_PD_PHDAC_Q[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(7), BIT(7)); // REG_PD_PLL OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x0000, BIT(6)); // REG_PD_PLL

                msWrite2ByteMask(REG_PHY2P1_2_P0_6C_L +u32PHY2P1BankOffset,BIT(8), BIT(8)); // REG_EN_LA[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_6C_L +u32PHY2P1BankOffset,0x0000, BMASK(7:4)); // REG_EN_LA[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset,0x000F, BMASK(3:0)); // REG_PD_CAL_SAFF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset,0x0080, BMASK(7:4)); // REG_PD_CAL_SAFF[3:0]



                break;

            case mhal_ePM_STANDBY:

                msWriteByteMask(REG_DPRX_PHY_PM_00_H + (0x40*enInputPortSelect) , 0x0E, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_01_H + (0x40*enInputPortSelect) , BIT(0), BIT(0)); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + (0x40*enInputPortSelect) , 0x00, 0xF0); // reg_nodie_en_sqh


                msWriteByteMask(REG_DPRX_PHY_PM_66_L , 0x00, BIT(0)); //NODIE_PD_CLKIN

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(3)|BIT(2), BMASK(3:2)); // REG_PD_CLKIN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(14), BIT(14)); // REG_PD_BG
                msWrite2ByteMask(REG_PHY2P1_2_P0_04_L +u32PHY2P1BankOffset,BIT(2), BIT(2)); // REG_PD_DLEV_SAFF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_04_L +u32PHY2P1BankOffset,0xF000, BMASK(15:12)); // REG_PD_DLEV_SAFF[3:0]
                msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset,BIT(0), BIT(0)); // REG_PD_DLPF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset,0x00F0, BMASK(7:4)); // REG_PD_DLPF[3:0]


                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(4), BIT(4)); // REG_PD_LANE[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x000F, BMASK(3:0)); // REG_PD_LANE[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset,BIT(6), BIT(6)); // REG_PD_LDO

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(7), BIT(7)); // REG_PD_PHDAC_I[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x0F00, BMASK(11:8)); // REG_PD_PHDAC_I[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(13), BIT(13)); // REG_PD_PHDAC_Q[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0xF000, BMASK(15:12)); // REG_PD_PHDAC_Q[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(7), BIT(7)); // REG_PD_PLL OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(6), BIT(6)); // REG_PD_PLL

                msWrite2ByteMask(REG_PHY2P1_2_P0_6C_L +u32PHY2P1BankOffset,BIT(8), BIT(8)); // REG_EN_LA[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_6C_L +u32PHY2P1BankOffset,0x0000, BMASK(7:4)); // REG_EN_LA[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset,0x000F, BMASK(3:0)); // REG_PD_CAL_SAFF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset,0x00F0, BMASK(7:4)); // REG_PD_CAL_SAFF[3:0]

                break;

            case mhal_ePM_POWEROFF:

                msWriteByteMask(REG_DPRX_PHY_PM_00_H + (0x40*enInputPortSelect) , 0x0F, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_01_H + (0x40*enInputPortSelect) , BIT(0), BIT(0)); // reg_nodie_en_sqh_ov
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + (0x40*enInputPortSelect) , 0x00, 0xF0); // reg_nodie_en_sqh


                msWriteByteMask(REG_DPRX_PHY_PM_66_L , 0, BIT(0)); //NODIE_PD_CLKIN

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(3)|BIT(2), BMASK(3:2)); // REG_PD_CLKIN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(14), BIT(14)); // REG_PD_BG
                msWrite2ByteMask(REG_PHY2P1_2_P0_04_L +u32PHY2P1BankOffset,BIT(2), BIT(2)); // REG_PD_DLEV_SAFF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_04_L +u32PHY2P1BankOffset,0xF000, BMASK(15:12)); // REG_PD_DLEV_SAFF[3:0]
                msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset,BIT(0), BIT(0)); // REG_PD_DLPF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_05_L +u32PHY2P1BankOffset,0x00F0, BMASK(7:4)); // REG_PD_DLPF[3:0]


                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(4), BIT(4)); // REG_PD_LANE[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x000F, BMASK(3:0)); // REG_PD_LANE[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_6E_L +u32PHY2P1BankOffset,BIT(6), BIT(6)); // REG_PD_LDO

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(7), BIT(7)); // REG_PD_PHDAC_I[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0x0F00, BMASK(11:8)); // REG_PD_PHDAC_I[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L +u32PHY2P1BankOffset,BIT(13), BIT(13)); // REG_PD_PHDAC_Q[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,0xF000, BMASK(15:12)); // REG_PD_PHDAC_Q[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(7), BIT(7)); // REG_PD_PLL OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_2F_L +u32PHY2P1BankOffset,BIT(6), BIT(6)); // REG_PD_PLL

                msWrite2ByteMask(REG_PHY2P1_2_P0_6C_L +u32PHY2P1BankOffset,BIT(8), BIT(8)); // REG_EN_LA[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_6C_L +u32PHY2P1BankOffset,0x0000, BMASK(7:4)); // REG_EN_LA[3:0]

                msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset,0x000F, BMASK(3:0)); // REG_PD_CAL_SAFF[3:0] OEN
                msWrite2ByteMask(REG_PHY2P1_2_P0_1F_L +u32PHY2P1BankOffset,0x00F0, BMASK(7:4)); // REG_PD_CAL_SAFF[3:0]

                break;
            default:
                break;
        }


}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_HPDControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_HPDControl(BYTE enInputPortSelect, Bool bPullHighFlag)
{
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            if(bPullHighFlag)
            {
                hw_Set_HdcpHpd();
            }
            else
            {
                hw_Clr_HdcpHpd();
            }
        break;

        case HDMI_INPUT_PORT1:
            if(bPullHighFlag)
            {
                hw_Set_HdcpHpd2();
            }
            else
            {
                hw_Clr_HdcpHpd2();
            }
        break;

        case HDMI_INPUT_PORT2:
            if(bPullHighFlag)
            {
                hw_Set_HdcpHpd3();
            }
            else
            {
                hw_Clr_HdcpHpd3();
            }
        break;

        default:
            bPullHighFlag = FALSE;
        break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_WriteInputPortEDID()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_WriteInputPortEDID(BYTE enInputPortSelect, BYTE ucEDIDSize, BYTE *pEDID)
{
    BYTE uctemp = 0;
    WORD u16Base = 0;
    BYTE ucSizeCount = 0;
    BYTE u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            msWrite2ByteMask(REG_PM_DDC_45_L, (ucEDIDSize > 0x2)? 0x01:0x00, 0x01); // REG_DDC_BANK_45[0]: Enhance DDC for D0
            msWrite2ByteMask(REG_PM_DDC_75_L, 0x00, MASKBIT(13:8)); // REG_DDC_BANK_75[12:8]: Select EDID sram base address for cpu read/write
            msWrite2ByteMask(REG_PM_DDC_76_L, 0x00, MASKBIT(5:0)); // REG_DDC_BANK_76[4:0]: Select D0 EDID sram base address
            break;

        case HDMI_DDC_1:
            msWrite2ByteMask(REG_PM_DDC_45_L, (ucEDIDSize > 0x2)? 0x02:0x00, 0x02); // REG_DDC_BANK_45[1]: Enhance DDC for D1
            msWrite2ByteMask(REG_PM_DDC_75_L, 0x0400, MASKBIT(13:8)); // REG_DDC_BANK_75[12:8]: Select EDID sram base address for cpu read/write
            msWrite2ByteMask(REG_PM_DDC_76_L, 0x0400, MASKBIT(13:8)); // REG_DDC_BANK_76[12:8]: Select D1 EDID sram base address
            break;

        case HDMI_DDC_2:
            msWrite2ByteMask(REG_PM_DDC_45_L, (ucEDIDSize > 0x2)? 0x04:0x00, 0x04); // REG_DDC_BANK_45[2]: Enhance DDC for D2
            msWrite2ByteMask(REG_PM_DDC_75_L, 0x0800, MASKBIT(13:8)); // REG_DDC_BANK_75[12:8]: Select EDID sram base address for cpu read/write
            msWrite2ByteMask(REG_PM_DDC_7B_L, 0x08, MASKBIT(5:0)); // REG_DDC_BANK_7B[4:0]: Select D2 EDID sram base address
            break;

            default:
                break;
    }

    u16Base = msRead2Byte(REG_PM_DDC_75_L);
     for(ucSizeCount = 0; ucSizeCount < ucEDIDSize; ucSizeCount++)
    {
        msWrite2ByteMask(REG_PM_DDC_75_L, u16Base + (ucSizeCount<<8), MASKBIT(13:8));

        for(uctemp = 0; uctemp < HDMI_EDID_BLOCK_SIZE; uctemp++)
        {
            _Hal_HDMI_InsertEDIDData(uctemp, pEDID[uctemp +ucSizeCount *HDMI_EDID_BLOCK_SIZE]);
        }
    }

     _Hal_HDMI_InternalEDIDEnable(enInputPortSelect, TRUE);
    _Hal_HDMI_EDIDWriteProtectEnable(enInputPortSelect, TRUE);
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_AudioPollingProc()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
void Hal_HDMI_AudioPollingProc(ST_HDMI_RX_AUDIO_INFO *pstHDMIRxAudioInfo)
{
	if(pstHDMIRxAudioInfo->bAudioFreqChangeFlag)
	{
		_Hal_HDMI_SetAudioFrequency(pstHDMIRxAudioInfo->usAudioFrequency);

		pstHDMIRxAudioInfo->bAudioFreqChangeFlag = FALSE;

		HDMI_HAL_DPRINTF("** TMDS change audio frequency %d stream %d \r\n", pstHDMIRxAudioInfo->usAudioFrequency);
	}

	//_mhal_hdmiRx_ClearAudioStatus(MAIN_IPMUX);
	Hal_HDMI_Audio_Status_Clear(NULL);

}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_GetDEStableStatus()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
Bool Hal_HDMI_GetDEStableStatus(BYTE enInputPortSelect __attribute__ ((unused)))
{
	Bool bDEStableFlag = FALSE;
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

	if((msRead2Byte(REG_HDMIRX_DTOP_P0_30_L + u32DTOPBankOffset) & BIT(5))) // hdmirx_dtop_30[5]: reg_de_stable
	{
		if(_Hal_tmds_GetHDMIModeFlag(enInputPortSelect))
        {
            if(mhal_tmds_HDMIGetBCHErrorStatus(enInputPortSelect))
                bDEStableFlag = FALSE;
            else
                bDEStableFlag = TRUE;
        }
        else
            bDEStableFlag = TRUE;
    }

	return bDEStableFlag;
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_GetAVMuteEnableFlag()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
Bool Hal_HDMI_GetAVMuteEnableFlag(BYTE enInputPortSelect __attribute__ ((unused)))
{
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);
	Bool bAVMuteEnableFlag = (msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_19_L + u32DTOPBankOffset) &BIT(0))? TRUE: FALSE;

	return bAVMuteEnableFlag;
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_GetVRREnableFlag()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
Bool Hal_HDMI_GetVRREnableFlag(BYTE enInputPortSelect __attribute__ ((unused)))
{
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);
	Bool bVRREnableFlag = (msRead2Byte(REG_HDMI_EM_U0_02_L + u32DTOPBankOffset) &BIT(1))? TRUE: FALSE;

	return bVRREnableFlag;
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_Get_InfoFrame()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
Bool Hal_HDMI_Get_InfoFrame(BYTE enInputPortSelect  __attribute__ ((unused)), MS_HDMI_PACKET_STATE_t u8state, void *pData, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo  __attribute__ ((unused)))
{
	Bool bRet = FALSE;
	BYTE uctemp = 0;
	WORD usPacketContent = 0;
	DWORD u32HDMIBankOffset = 0;//_Hal_tmds_GetHDMIBankOffset(ucHDMIInfoSource);

	switch(u8state)
	{
		case PKT_HDR:
			{
				WORD *pInfFrame = (WORD*)pData;

				for(uctemp = 0; uctemp < HDMI_HDR_PACKET_SIZE; uctemp++)
				{
					usPacketContent = msRead2Byte(REG_HDMI2_U0_61_L +u32HDMIBankOffset +uctemp *2); // hdmi2_u0_61[7:0]: reg_hdr_pb01

					pInfFrame[uctemp] = (usPacketContent << 8) | (usPacketContent >> 8);
				}

				bRet = TRUE;
			}
			break;

		default:
			break;
	};

	return bRet;
}


//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_avi_infoframe_info()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE Hal_HDMI_avi_infoframe_info(BYTE enInputPortSelect __attribute__ ((unused)), BYTE u8byte, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)))
{
	BYTE u8ReturnValue = 0;
	WORD u16Reg = 0;
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset((E_MUX_INPUTPORT)(INPUT_PORT_DVI0 +ucHDMIInfoSource));

	if(u8byte > 0)
	{
		u16Reg = msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_23_L +u32DTOPBankOffset +((u8byte - 1)/2)*2); // hdmirx_dtop_pkt_23[7:0]: reg_avi_if1; 29[15:8]: reg_avi_if14;
		u8ReturnValue = (BYTE)(((u8byte-1)&0x01) ? (u16Reg>>8) : (u16Reg&0xFF));
	}

	return u8ReturnValue;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_GetGCPColorDepth()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE Hal_HDMI_GetGCPColorDepth(BYTE enInputPortSelect, DWORD *u32PacketStatus)
{
    BYTE u8ReValue = HDMI_COLOR_DEPTH_8BIT;
    WORD u16ColorDepth = 0;
    DWORD u32PKTStatus = *u32PacketStatus;

    if(u32PKTStatus & HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG) // GCP packet received
    {
        u16ColorDepth = Hal_HDMI_gcontrol_info(G_Ctrl_CD_VAL, enInputPortSelect);

        switch(u16ColorDepth) // Color depth
        {
            case 0x500:
                u8ReValue = HDMI_COLOR_DEPTH_10BIT;
                break;

            case 0x600:
                u8ReValue = HDMI_COLOR_DEPTH_12BIT;
                break;

            case 0x700:
                u8ReValue = HDMI_COLOR_DEPTH_16BIT;
                break;

            default:
                break;
        };
    }
    return u8ReValue;
}

//**************************************************************************
//  [Function Name]:
//                  KHal_HDMIRx_GetPixelRepetitionInfo()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MSCombo_TMDS_PIXEL_REPETITION KHal_HDMIRx_GetPixelRepetitionInfo(MS_U8 enInputPortType __attribute__ ((unused)))
{
    BYTE ucTmpValue = 0;
    DWORD u32DTOPBankOffset = 0;
    //MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPKTDECBankOffset(0);

    // [3:0]: Pixel repetition
    //ucTmpValue = (msReadByte(REG_173084 + wOffset_hdmi) & 0x0F); // hdmi_dual_42[3:0]: pixel repetition
    //ucTmpValue = u4IO32ReadFld_1(REG_008C_P0_HDMIRX_PKT_DEC + u32PortBankOffset,REG_008C_P0_HDMIRX_PKT_DEC_REG_AVI_PB05) & BMASK(3:0);//AVI InfoFrame byte 5.[3:0]: Pixel repetition; pixel sent (PR+1) times.
    ucTmpValue = (msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_25_L +u32DTOPBankOffset) & 0x0F);

    switch (ucTmpValue)
    {
        case 0:
            return MSCombo_TMDS_N0_PIX_REP;
        break;
        case 1:
            return MSCombo_TMDS_2x_PIX_REP;
        break;
        case 2:
            return MSCombo_TMDS_3x_PIX_REP;
        break;
        case 3:
            return MSCombo_TMDS_4x_PIX_REP;
        break;
        case 4:
            return MSCombo_TMDS_5x_PIX_REP;
        break;
        case 5:
            return MSCombo_TMDS_6x_PIX_REP;
        break;
        case 6:
            return MSCombo_TMDS_7x_PIX_REP;
        break;
        case 7:
            return MSCombo_TMDS_8x_PIX_REP;
        break;
        case 8:
            return MSCombo_TMDS_9x_PIX_REP;
        break;
        case 9:
            return MSCombo_TMDS_10x_PIX_REP;
        break;
        default:
            return MSCombo_TMDS_RESERVED_PIX_REP;
        break;
    }

}


//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_GetColorFormat()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE Hal_HDMI_GetColorFormat(BYTE enInputPortSelect __attribute__ ((unused)))
{
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

    BYTE u8ColorFormat = (msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_23_L +u32DTOPBankOffset) &BMASK(6:5)) >>5;

    switch(u8ColorFormat)
	{
	    case 0:
			u8ColorFormat = COMBO_COLOR_FORMAT_RGB;
		break;

		case 1:
			u8ColorFormat = COMBO_COLOR_FORMAT_YUV_422;
		break;

	    case 2:
			u8ColorFormat = COMBO_COLOR_FORMAT_YUV_444;
		break;

		case 3:
			u8ColorFormat = COMBO_COLOR_FORMAT_YUV_420;
		break;

	    default:
			u8ColorFormat = COMBO_COLOR_FORMAT_RGB;
		break;

	}
	return u8ColorFormat;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_GetColorRange()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE Hal_HDMI_GetColorRange(BYTE enInputPortSelect)
{
    DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);
    BYTE u8YCCValue = Hal_HDMI_GetColorFormat(enInputPortSelect);
    BYTE u8ColorRange = COMBO_COLOR_RANGE_DEFAULT;
    BYTE u8YccColorRangeReg = ((msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_25_L +u32DTOPBankOffset) &BMASK(7:6)) >>6);
    BYTE u8RgbColorRangeReg = ((msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_24_L +u32DTOPBankOffset) &BMASK(3:2)) >>2);
    BYTE u8VIC_Value = (msReadByte(REG_HDMIRX_DTOP_PKT_P0_24_H +u32DTOPBankOffset) & 0xFF);

    if((u8YCCValue == 1) ||(u8YCCValue == 2) || (u8YCCValue == 3)) //YCbCr
    {
        switch(u8YccColorRangeReg)
        {
            case 0:
                u8ColorRange = COMBO_COLOR_RANGE_LIMIT;
            break;
            case 1:
                u8ColorRange = COMBO_COLOR_RANGE_FULL;
            break;
            case 2:
                u8ColorRange = COMBO_COLOR_RANGE_RESERVED;
            break;
            default:
                u8ColorRange = COMBO_COLOR_RANGE_RESERVED;
            break;
        }
    }
    else  //RGB
    {
        switch (u8RgbColorRangeReg)
        {
            case 0:
                {
                    if(u8VIC_Value > 1) // CE timing , color range need limit
                        u8ColorRange = COMBO_COLOR_RANGE_LIMIT;
                    else    // IT timing , color range need FULL
                        u8ColorRange = COMBO_COLOR_RANGE_FULL;
                }
            break;
            case 1:
                u8ColorRange = COMBO_COLOR_RANGE_LIMIT;
            break;
            case 2:
                u8ColorRange = COMBO_COLOR_RANGE_FULL;
            break;
            default:
                u8ColorRange = COMBO_COLOR_RANGE_RESERVED;
            break;
        }
    }
    return u8ColorRange;
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDCP_getstatus()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
WORD Hal_HDCP_getstatus(BYTE enInputPortSelect __attribute__ ((unused)))
{
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

	return msRead2Byte(REG_HDMIRX_HDCP_P0_01_L +u32DTOPBankOffset); // hdmirx_hdcp_01
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_gcontrol_info()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
WORD Hal_HDMI_gcontrol_info(HDMI_GControl_INFO_t gcontrol, BYTE enInputPortSelect __attribute__ ((unused)))
{
	WORD u16regvalue;
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset((E_MUX_INPUTPORT)(INPUT_PORT_DVI0 +enInputPortSelect));

	u16regvalue = msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_19_L +u32DTOPBankOffset); // hdmirx_dtop_pkt_19[15:0]: reg_gcontrol

	switch(gcontrol)
	{
		default:
		case G_CTRL_AVMUTE://#[0]: AVMUTE
			u16regvalue &= BIT(0);
			break;
		case G_Ctrl_DEFAULT_PHASE://#[1]: Default_Phase
			u16regvalue &= BIT(1);
			break;
		case G_Ctrl_LAST_PP://#[4:2]: LAST_PP[2:0]
			u16regvalue &= (BIT(4)|BIT(3)|BIT(2));
			break;
		case G_Ctrl_PRE_LPP://#[7:5]: PRE_LPP[2:0]
			u16regvalue &= (BIT(7)|BIT(6)|BIT(5));
			break;
		case G_Ctrl_CD_VAL://#[11:8]: CD_VAL[3:0]
			u16regvalue &= (BIT(11)|BIT(10)|BIT(9)|BIT(8));
			break;
		case G_Ctrl_PP_VAL://#[15:12]: PP_VAL[3:0]
			u16regvalue &= (BIT(15)|BIT(14)|BIT(13)|BIT(12));
			break;
		case G_Ctrl_ALL:
			u16regvalue &=(LBMASK|HBMASK);
			break;
	}

	return u16regvalue;
}



//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_set_PortSel_byGPIOConfig()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_HDMI_set_PortSel_byGPIOConfig(BYTE u8HDMI_GPIO)
{
    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            msWrite2ByteMask(REG_DPRX_PHY_PM_64_L, 0, BMASK(1:0));//dp_phy_pm_64[1:0]: reg_hdmi_pm_port_sel: means port number of EVB
            break;

        case HDMI_DDC_1:
            msWrite2ByteMask(REG_DPRX_PHY_PM_64_L, BIT0, BMASK(1:0));//dp_phy_pm_64[1:0]: reg_hdmi_pm_port_sel: means port number of EVB
            break;

        case HDMI_DDC_2:
            msWrite2ByteMask(REG_DPRX_PHY_PM_64_L, BIT1, BMASK(1:0));//dp_phy_pm_64[1:0]: reg_hdmi_pm_port_sel: means port number of EVB
            break;

        default:
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_set_DDC_byGPIOConfig()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_HDMI_set_DDC_Engine_byGPIOConfig(BYTE u8HDMI_GPIO)
{
    WORD ucP1P2DDCIndex = _Hal_HDMI_P1P2DDCOffset(u8HDMI_GPIO);

    switch(u8HDMI_GPIO)
    {
        case HDMI_DDC_0:
            //default setting
            break;

        case HDMI_DDC_1:
            //for DDC1 engine enable
            msWrite2ByteMask(REG_DPRX_AUX_54_L  + ucP1P2DDCIndex, 0, BIT2); //dp_aux_pm0_p1_54[2]:reg_en_aux
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_61_L  + ucP1P2DDCIndex, BIT2, BIT2); //dp_aux_pm2_p1_61[2]: reg_en_gpio
            msWrite2ByteMask(REG_DPRX_AUX_54_L  + ucP1P2DDCIndex, BIT13, BIT13); //dp_aux_pm0_p1_54[13]: reg_pd_aux_rterm
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_61_L  + ucP1P2DDCIndex, BMASK(7:5), BMASK(7:5)); //dp_aux_pm2_p1_61[7]: reg_pd_vcm_op; [6]: reg_pd_ngate_oven; [5]: reg_pd_ngate_ov
            msWrite2ByteMask(REG_DPRX_AUX_7D_L + ucP1P2DDCIndex, BIT3|BIT2, BMASK(3:0)); //dp_aux_pm0_p1_7D[3]: reg_aux_tx_oen_set_ov_mode; [2]: reg_aux_rx_oen_set_ov_mode; [1]: reg_aux_tx_oen_set; [0]: reg_aux_rx_oen_set
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_61_L + ucP1P2DDCIndex, BIT4, BIT4|BIT3|BIT1); //dp_aux_pm2_p1_61[4]:reg_en_hbr3_ov_en; [3]: reg_en_hbr3_ov; [1]: reg_en_ft_mux
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_0F_L + ucP1P2DDCIndex, 0, BIT0); //dp_aux_pm2_p1_0F[0]: reg_en_rxcm_boost
            break;

        case HDMI_DDC_2:
            //for DDC2 engine enable
            msWrite2ByteMask(REG_DPRX_AUX_54_L  + ucP1P2DDCIndex, 0, BIT2); //dp_aux_pm0_p1_54[2]:reg_en_aux
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_61_L  + ucP1P2DDCIndex, BIT2, BIT2); //dp_aux_pm2_p1_61[2]: reg_en_gpio
            msWrite2ByteMask(REG_DPRX_AUX_54_L  + ucP1P2DDCIndex, BIT13, BIT13); //dp_aux_pm0_p1_54[13]: reg_pd_aux_rterm
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_61_L  + ucP1P2DDCIndex, BMASK(7:5), BMASK(7:5)); //dp_aux_pm2_p1_61[7]: reg_pd_vcm_op; [6]: reg_pd_ngate_oven; [5]: reg_pd_ngate_ov
            msWrite2ByteMask(REG_DPRX_AUX_7D_L + ucP1P2DDCIndex, BIT3|BIT2, BMASK(3:0)); //dp_aux_pm0_p1_7D[3]: reg_aux_tx_oen_set_ov_mode; [2]: reg_aux_rx_oen_set_ov_mode; [1]: reg_aux_tx_oen_set; [0]: reg_aux_rx_oen_set
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_61_L + ucP1P2DDCIndex, BIT4, BIT4|BIT3|BIT1); //dp_aux_pm2_p1_61[4]:reg_en_hbr3_ov_en; [3]: reg_en_hbr3_ov; [1]: reg_en_ft_mux
            msWrite2ByteMask(REG_DPRX_AUX_DPCD_0F_L + ucP1P2DDCIndex, 0, BIT0); //dp_aux_pm2_p1_0F[0]: reg_en_rxcm_boost
            break;

        default:
            break;
    }
}


//**************************************************************************
//  [Function Name]:
//                  Hal_tmds_Switch2HDMI()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_tmds_Switch2HDMI(BYTE hdmirx_id)
{
    msWriteByteMask(REG_DPRX_PHY_PM_65_L, 0x0, BIT0); //dp_phy_pm_65[0]: reg_dp_hdmi_pm_port_sel   0:HDMI   1:DP
    msWriteByteMask(REG_DPRX_HDMI_30_L, 0x0, BIT0);//dp_hdmi_phy_30[0]: reg_dp_select, 0: mux select to hdmi function; 1: mux select to dp function
    msWriteByteMask(REG_DPRX_HDMI_30_L, 0x0, BIT4); //dp_hdmi_phy_30[4]: reg_force_dp_select, 0: dp_select from  PM domain; 1: dp_select from reg_dp_select

    // fro HDMi 750M issue
    msWrite2ByteMask(REG_DPRX_PHY1_1B_L,0x0A00, BMASK(11:8));
    msWrite2ByteMask(REG_DPRX_PHY3_52_L,0x0, BIT(0));

    msWriteByteMask(REG_DPRX_PHY_PM_66_L , 0x00, BIT(0)); //NODIE_PD_CLKIN


    switch(hdmirx_id)
    {
        case HDMI_INPUT_PORT0:
#if ENABLE_HPD_REPLACE_MODE
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT0, 0);
            Hal_HDMI_SCDC_Clr(HDMI_INPUT_PORT0);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT1, 1);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT2, 1);
#endif
            _Hal_HDMI_set_PortSel_byGPIOConfig(HDMI_Rx0);

            //U03 use main link setting config with hw real port
            msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, 0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

            msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, 0, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
            msWrite2ByteMask(REG_DPRX_PHY_PM_62_L, 0, BMASK(1:0));
            break;

      case HDMI_INPUT_PORT1:
#if ENABLE_HPD_REPLACE_MODE
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT0, 1);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT1, 0);
            Hal_HDMI_SCDC_Clr(HDMI_INPUT_PORT1);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT2, 1);
#endif

            _Hal_HDMI_set_PortSel_byGPIOConfig(HDMI_Rx1);
            _Hal_HDMI_set_DDC_Engine_byGPIOConfig(HDMI_Rx1);

            //U03 use main link setting config with hw real port
            msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, BIT0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

            msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, BIT14, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
            msWrite2ByteMask(REG_DPRX_PHY_PM_62_L, BIT2, BMASK(3:2));
            break;

      case HDMI_INPUT_PORT2:
#if ENABLE_HPD_REPLACE_MODE
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT0, 1);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT1, 1);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT2, 0);
            Hal_HDMI_SCDC_Clr(HDMI_INPUT_PORT2);
#endif

            _Hal_HDMI_set_PortSel_byGPIOConfig(HDMI_Rx2);
            _Hal_HDMI_set_DDC_Engine_byGPIOConfig(HDMI_Rx2);

            //U03 use main link setting config with hw real port
            msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, BIT1, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

            msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, BIT15, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
            msWrite2ByteMask(REG_DPRX_PHY_PM_62_L, BIT5, BMASK(5:4));
            break;

            default:
                break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_PM_SwitchHDMIPort()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_PM_SwitchHDMIPort(BYTE hdmirx_id)
{
    msWriteByteMask(REG_DPRX_PHY_PM_65_L, 0x0, BIT0); //dp_phy_pm_65[0]: reg_dp_hdmi_pm_port_sel   0:HDMI	1:DP
    //msWriteByteMask(REG_DPRX_HDMI_30_L, 0x0, BIT0);//dp_hdmi_phy_30[0]: reg_dp_select, 0: mux select to hdmi function; 1: mux select to dp function
    //msWriteByteMask(REG_DPRX_HDMI_30_L, 0x0, BIT4); //dp_hdmi_phy_30[4]: reg_force_dp_select, 0: dp_select from  PM domain; 1: dp_select from reg_dp_select

    switch(hdmirx_id)
    {
    case HDMI_INPUT_PORT0:
        _Hal_HDMI_set_PortSel_byGPIOConfig(HDMI_Rx0);

        //U03 use main link setting config with hw real port
        msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, 0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

        //msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, 0, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
        break;

    case HDMI_INPUT_PORT1:
        _Hal_HDMI_set_PortSel_byGPIOConfig(HDMI_Rx1);

        //U03 use main link setting config with hw real port
        msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, BIT0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

        //msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, BIT14, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
        break;

    case HDMI_INPUT_PORT2:
        _Hal_HDMI_set_PortSel_byGPIOConfig(HDMI_Rx2);

        //U03 use main link setting config with hw real port
        msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, BIT1, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

        //msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, BIT15, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
        break;

        default:
            break;
    }
}


//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_Audio_MUTE_Enable()
//  [Description]:
//                  use to enable hdmi audio mute event.
//  [Arguments]:
//                  u16MuteEvent: hdmi audio mute event.
//
//                  [0]: HDMI CTS/N over limited range.
//                  [1]: HDMI video clock frequency big change.
//                  [2]: AVMute.
//                  [3]: hdmi no input
//                  [4]: audio sample with flat bit
//                  [5]: audio present bit error
//                  [6]: audio fifo overflow/underflow
//                  [7]: audio sample error/audio sample priority error
//  [Return]:
//
//**************************************************************************
void Hal_HDMI_Audio_MUTE_Enable(DWORD u16MuteEvent, DWORD u16MuteMask)
{
    msWriteByteMask(REG_COMBO_GP_TOP_4A_L, u16MuteEvent, u16MuteMask); // combo_gp_top_4A[7:0]: audio mute event to vivaldi
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_GetDataInfo()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
WORD Hal_HDMI_GetDataInfo(E_HDMI_GET_DATA_INFO enInfo, BYTE enInputPortSelect __attribute__ ((unused)))
{
	WORD u16HDMIDataInfo = 0;
	DWORD u32VEBankOffset = 0;//_Hal_tmds_GetVEBankOffset((E_MUX_INPUTPORT)(INPUT_PORT_DVI0 +ucHDMIInfoSource));
	DWORD u32HDMIBankOffset =  0;//_Hal_tmds_GetHDMIBankOffset(ucHDMIInfoSource);

	switch(enInfo)
	{
		case E_HDMI_GET_HDE:
			u16HDMIDataInfo = msRead2Byte(REG_HDMIRX_VE_U0_14_L +u32VEBankOffset)& BMASK(13:0); // hdmirx_ve_14[15:0]: reg_timing_det_hde
			break;

		case E_HDMI_GET_VDE:
			u16HDMIDataInfo = msRead2Byte(REG_HDMIRX_VE_U0_15_L +u32VEBankOffset)& BMASK(13:0); // hdmirx_ve_15[15:0]: reg_timing_det_vde
			break;

		case E_HDMI_GET_HTT:
			u16HDMIDataInfo = msRead2Byte(REG_HDMIRX_VE_U0_12_L +u32VEBankOffset)& BMASK(13:0); // hdmirx_ve_12[15:0]: reg_timing_det_htt
			break;

		case E_HDMI_GET_VTT:
			u16HDMIDataInfo = msRead2Byte(REG_HDMIRX_VE_U0_13_L +u32VEBankOffset)& BMASK(13:0); // hdmirx_ve_13[15:0]: reg_timing_det_vtt
			break;

		case E_HDMI_GET_SUPPORT_EDID_SIZE:
			u16HDMIDataInfo = HDMI_EDID_SUPPORT_SIZE;
			break;

		case E_HDMI_GET_MULTIVS_COUNT:
			u16HDMIDataInfo = msRead2Byte(REG_HDMI3_U0_2F_L +u32HDMIBankOffset)& BMASK(3:0); // hdmi3_u0_2F[3:0]: reg_vsif_cnt
			break;

		case E_HDMI_GET_H_POLARITY:
			u16HDMIDataInfo = (msRead2Byte(REG_HDMIRX_VE_U0_11_L +u32VEBankOffset)& BIT(13)) >> 13; // hdmirx_ve_11[13]: reg_hs_pol
			break;

		case E_HDMI_GET_V_POLARITY:
			u16HDMIDataInfo = (msRead2Byte(REG_HDMIRX_VE_U0_11_L +u32VEBankOffset)& BIT(12)) >> 12; // hdmirx_ve_11[12]: reg_vs_pol
			break;

		case E_HDMI_GET_INTERLACE:
			u16HDMIDataInfo = (msRead2Byte(REG_HDMIRX_VE_U0_11_L +u32VEBankOffset)& BIT(15)) >> 15; // hdmirx_ve_11[15]: reg_src_det_i_md_en
			break;

		case E_HDMI_GET_CHIP_HDCP_CAPABILITY:
			u16HDMIDataInfo = HDMI_HDCP_2_2;
			break;

		default:

			break;
	};

	return u16HDMIDataInfo;
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_GetAudioContentInfos()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
DWORD Hal_HDMI_GetAudioContentInfo(BYTE enInputPortSelect   __attribute__ ((unused)), EN_HDMI_AUDIO_CONTENT_INFO enAudioContentInfo)
{
	DWORD ulAudioContent = 0;
	DWORD u32HDMIBankOffset = 0;

	//load current CTS/N value first
	msWrite2Byte(REG_HDMI_U0_12_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_12[15:0]: CTS[15:0] from ACR packet
	msWrite2Byte(REG_HDMI_U0_13_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_13[15:0]: N[15:0] from ACR packet
	msWrite2ByteMask(REG_HDMI_U0_14_L +u32HDMIBankOffset, 0xFF, 0xFF); // hdmi_u0_14[7:4]: N[19:16]; [3:0]: CTS[19:16] from ACR packet

	switch(enAudioContentInfo)
	{
		case HDMI_AUDIO_CONTENT_CTS:
			ulAudioContent = (DWORD)(((msRead2Byte(REG_HDMI_U0_14_L +u32HDMIBankOffset) & BMASK(3:0))*0x10000)|(msRead2Byte(REG_HDMI_U0_12_L +u32HDMIBankOffset)));

			break;

		case HDMI_AUDIO_CONTENT_N:
			ulAudioContent = (DWORD)((((msRead2Byte(REG_HDMI_U0_14_L +u32HDMIBankOffset) &BMASK(7:4)) >> 4)*0x10000)|msRead2Byte(REG_HDMI_U0_13_L +u32HDMIBankOffset));
			break;

		default:

			break;
	}

	return ulAudioContent;
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_packet_info()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
DWORD Hal_HDMI_packet_info(BYTE enInputPortSelect __attribute__ ((unused)), ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)))
{
	Bool bCFDPlusFlag = FALSE;
	WORD usPacketStatus = 0;
	DWORD ulPacketStatus = 0;
	DWORD u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset((E_MUX_INPUTPORT)(INPUT_PORT_DVI0 +ucHDMIInfoSource));
	DWORD u32HDMIBankOffset = 0;//_Hal_tmds_GetHDMIBankOffset(ucHDMIInfoSource);

	// Dummy reg for CFD+
	usPacketStatus = msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_50_L +u32DTOPBankOffset); // hdmirx_dtop_pkt_50[15:0]: reg_pkt_reserved_0

	if(usPacketStatus > 0)
	{
		if(usPacketStatus & BIT(0))
		{
			ulPacketStatus = ulPacketStatus | HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG;
		}

		msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_50_L +u32DTOPBankOffset, 0, BIT(0)); // hdmirx_dtop_pkt_0B[15:0]: reg_pkt_reserved_0

		//if(enInputPortSelect == _Hal_HDMI_GetPacketHdmiPortSelect()) // check HDMI source is same as reg_xc2hd_port_sel
		{
			if(usPacketStatus & BIT(1))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG;
			}

			if(usPacketStatus & BIT(2))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_VS_PACKET_RECEIVE_FLAG;
			}

			msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_50_L +u32DTOPBankOffset, 0, BMASK(2:1)); // hdmirx_dtop_pkt_0B[15:0]: reg_pkt_reserved_0
		}

		if(usPacketStatus & BIT(15))
		{
			bCFDPlusFlag = TRUE;
		}

	}

	usPacketStatus = msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_10_L +u32DTOPBankOffset) & BMASK(6:3); // hdmirx_dtop_pkt_10[3]: GCP; [4]: AVI; [5]: SPD; [6]: RSV packet received

	if(usPacketStatus & BIT(3))
	{
		ulPacketStatus = ulPacketStatus | HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG;
	}
	if(usPacketStatus & BIT(4))
	{
		ulPacketStatus = ulPacketStatus | HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG;
	}
	if(usPacketStatus & BIT(5))
	{
		ulPacketStatus = ulPacketStatus | HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG;
	}
	if(usPacketStatus & BIT(6))
	{
		ulPacketStatus = ulPacketStatus | HDMI_STATUS_RESERVED_PACKET_RECEIVE_FLAG;
	}

	if(usPacketStatus > 0)
	{
		if(bCFDPlusFlag)
		{
			usPacketStatus = usPacketStatus& (~(BIT(4))); // HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG
		}

		// Clear status
		msWrite2Byte(REG_HDMIRX_DTOP_PKT_P0_10_L +u32DTOPBankOffset, usPacketStatus); // hdmirx_dtop_pkt_10[3]: GCP; [4]: AVI; [5]: SPD; [6]: RSV packet received
	}

	//if(ucHDMIInfoSource == _Hal_HDMI_GetPacketHdmiPortSelect())//check HDMI source is same as reg_xc2hd_port_sel
	{
		usPacketStatus = msRead2Byte(REG_HDMI_U0_01_L +u32HDMIBankOffset);

		if(usPacketStatus > 0)
		{
			ulPacketStatus |= usPacketStatus;

			if(bCFDPlusFlag)
			{
				usPacketStatus = usPacketStatus& (~HDMI_STATUS_VS_PACKET_RECEIVE_FLAG);
			}

			msWrite2Byte(REG_HDMI_U0_01_L +u32HDMIBankOffset, usPacketStatus); // hdmi_u0_01[15:0]
		}

		usPacketStatus = msRead2Byte(REG_HDMI_U0_02_L +u32HDMIBankOffset) & BMASK(15:14);

		if(usPacketStatus > 0)
		{
			if(usPacketStatus &BIT(14))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG;
			}

			if(bCFDPlusFlag)
			{
				usPacketStatus = 0;
			}

			// Clear status
			msWrite2Byte(REG_HDMI_U0_02_L +u32HDMIBankOffset, usPacketStatus);
		}

		usPacketStatus = msRead2Byte(REG_HDMI2_U0_26_L +u32HDMIBankOffset) &BIT(3);

		if(usPacketStatus &BIT(3))
		{
			ulPacketStatus = ulPacketStatus | HDMI_STATUS_EDR_VALID_FLAG;

			// Clear status
			//_Hal_tmds_ClearEDRVaildFlag(ucHDMIInfoSource);
		}

		usPacketStatus = msRead2Byte(REG_HDMI_U0_03_L +u32HDMIBankOffset) & BMASK(6:0);

		if(usPacketStatus > 0)
		{
			if(usPacketStatus &BIT(0))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_DST_RECEIVE_FLAG;
			}

			if(usPacketStatus &BIT(1))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_3D_ASP_RECEIVE_FLAG;
				ulPacketStatus = ulPacketStatus & (~HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG);
			}

			if(usPacketStatus &BIT(2))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_3D_DSD_RECEIVE_FLAG;
				ulPacketStatus = ulPacketStatus & (~HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG);
			}

			if(usPacketStatus &BIT(4))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_MULTI_ASP_RECEIVE_FLAG;
				ulPacketStatus = ulPacketStatus & (~HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG);
			}

			if(usPacketStatus &BIT(5))
			{
				ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_MULTI_DSD_RECEIVE_FLAG;
				ulPacketStatus = ulPacketStatus & (~HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG);
			}

			// Clear status
			msWrite2Byte(REG_HDMI_U0_03_L +u32HDMIBankOffset, usPacketStatus);
		}

		if(msRead2Byte(REG_HDMI_EM_U0_07_L +u32HDMIBankOffset) & BIT(7))
		{
			ulPacketStatus = ulPacketStatus| HDMI_STATUS_EM_PACKET_RECEIVE_FLAG;

			msWrite2Byte(REG_HDMI_EM_U0_0D_L +u32HDMIBankOffset, BIT(3));
		}
	}

	return ulPacketStatus;
}

//**************************************************************************
//	[Function Name]:
//					Hal_HDMI_GetColorimetry()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
//
//**************************************************************************
ST_HDMI_RX_COLOR_FORMAT Hal_HDMI_GetColorimetry(BYTE enInputPortSelect  __attribute__ ((unused)))
{
    ST_HDMI_RX_COLOR_FORMAT stColorimetry = {COMBO_YUV_COLORIMETRY_NoData, COMBO_COLORIMETRY_NONE, COMBO_COLORIMETRY_Additional_NoData};
    BYTE u8ColorimetryValue = (msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_23_L) &BMASK(15:14)) >>14;
    BYTE u8ExtenedColorimetryValue = (msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_24_L) &BMASK(6:4)) >>4;
    BYTE u8ExtendedAddiColorimetryVal = (msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_29_L) &BMASK(15:12)) >>12;

    switch(u8ExtendedAddiColorimetryVal)
    {
        case 0:
            stColorimetry.ucExtAddColorimetry = COMBO_COLORIMETRY_Additional_DCI_P3_RGB_D65;
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_DCI_P3_RGB_D65;
        break;

        case 1:
            stColorimetry.ucExtAddColorimetry = COMBO_COLORIMETRY_Additional_DCI_P3_RGB_Theater;
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_DCI_P3_RGB_Theater;
        break;
    }

    switch(u8ExtenedColorimetryValue)
    {
        case 0:
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_xvYCC601;
        break;

        case 1:
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_xvYCC709;
        break;

        case 2:
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_sYCC601;
        break;

        case 3:
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_ADOBEYCC601;
        break;

        case 4:
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_ADOBERBG;
        break;

        case 5:
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_BT2020YcCbcCrc;
        break;

        case 6:
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_BT2020RGBYCbCr;
        break;
    }

    switch(u8ColorimetryValue)
    {
        case 0:
            stColorimetry.ucColorimetry = COMBO_YUV_COLORIMETRY_NoData;
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_NONE;
            break;

        case 1:
            stColorimetry.ucColorimetry = COMBO_YUV_COLORIMETRY_ITU601;
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_ITU601;
            break;

        case 2:
            stColorimetry.ucColorimetry = COMBO_YUV_COLORIMETRY_ITU709;
            stColorimetry.ucExtColorimetry = COMBO_COLORIMETRY_ITU709;
            break;

        case 3:
            stColorimetry.ucColorimetry = COMBO_YUV_COLORIMETRY_EC;
            break;
    }

    return stColorimetry;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_audio_channel_status()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE Hal_HDMI_audio_channel_status(BYTE ucPortIndex __attribute__ ((unused)), BYTE ucByte)
{
    DWORD ulHDMIBankiOffset = 0;

    //192 bit
    if(ucByte % 2)
    {
        return ( (msRead2Byte(REG_HDMI_U0_70_L + ulHDMIBankiOffset + (ucByte/2)*2))&BMASK(15:8) >> 8); // hdmi_70[7:0]: reg_hdmi_cs
    }
    else
    {
        return ( (msRead2Byte(REG_HDMI_U0_70_L + ulHDMIBankiOffset + (ucByte/2)*2)) &BMASK(7:0));// hdmi_70[7:0]: reg_hdmi_cs
    }
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_IsAudioFmtPCM()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
EN_TMDS_AUDIO_FORMAT Hal_HDMI_IsAudioFmtPCM(BYTE ucPortIndex  __attribute__ ((unused)))
{
    EN_TMDS_AUDIO_FORMAT enAudioFMT = COMBO_AUDIO_FMT_UNKNOWN;

    if((msRead2Byte(REG_HDMI_U0_70_L) & BIT(1)) || (msRead2Byte(REG_HDMI3_U0_52_L) & BMASK(7:5))) //Channel Status: PCM/nPCM || AudioInfo Fraome none PCM Format
    {
        enAudioFMT = COMBO_AUDIO_FMT_NON_PCM;
    }
    else
    {
        enAudioFMT = COMBO_AUDIO_FMT_PCM;
    }

    return enAudioFMT;
}


#ifdef HDMI1P4_ANALOG_SETTING_OV_EN
inline static void phy_param_init(BYTE enInputPortType,MS_BOOL b_h2p1_port)
{
    stHDMI_phy_parm* ptr_phy_param = &s_hdmi_phy_parm;
    UNUSED(b_h2p1_port);
    UNUSED(enInputPortType);

    memset((void*)ptr_phy_param,0,sizeof(stHDMI_phy_parm));
    ptr_phy_param->s_ov_table_a.u32_n        =OV_ELEMENTS_H2P0_PORT_N;
    ptr_phy_param->s_ov_table_a.ptr_ov_table = ov_table_h2p0_port;
}

inline static MS_BOOL _Hal_HDMI_Fix_Analog_Setting(BYTE enInputPortType,MS_BOOL b_ov_en)
{
    DWORD u32PHY2P1BankOffset = 0;
    stHDMI_phy_parm* ptr_phy_param = &s_hdmi_phy_parm;
    WORD u16_tmp = msRead2Byte(REG_PHY2P1_1_P0_37_L+u32PHY2P1BankOffset);
    // MS_U16 u16_tmp_1,u16_tmp_2;
    MS_BOOL ret = TRUE;

    UNUSED(enInputPortType);
    //HDMI_HAL_DPRINTF("[%s] ** port/b_ov_en:%d/%d/0x%x \n",
        //__FUNCTION__, enInputPortType,b_ov_en,u16_tmp);

    // u16_tmp_1 = R2BYTE(REG_PHY2P1_2_P0_26_L+u32PHY2P1BankOffset);


    if(b_ov_en)
    {
        WORD u16_reg_band = u16_tmp&0x7f;
        BYTE u8_bit = 0xff;
        DWORD u32_indx = 0;
        while(u16_reg_band) {
            u16_reg_band >>= 1;
            u8_bit++;
        };
        if (u8_bit <5)
        {
            s_ov_element* ptr_ov_tb = ptr_phy_param->s_ov_table_a.ptr_ov_table;
            for(u32_indx=0;u32_indx<ptr_phy_param->s_ov_table_a.u32_n;u32_indx++)
            {
                DWORD u32_reg = (ptr_ov_tb+u32_indx)->u32_reg;
                WORD u16_msk = (ptr_ov_tb+u32_indx)->u16_msk;
                WORD u16_ov_en_msk = (ptr_ov_tb+u32_indx)->u16_ov_en_msk;
                WORD u16_val = (ptr_ov_tb+u32_indx)->u16_val[u8_bit];

                if (!(msRead2Byte(u32_reg+u32PHY2P1BankOffset)&u16_ov_en_msk))
                {
                    msWrite2ByteMask(u32_reg+u32PHY2P1BankOffset,u16_val|u16_ov_en_msk,u16_msk);
                    /*
                    MHAL_HDMIRX_MSG_INFO("** HDMI REG_W1, %x,%x|%x,%x,%x\n",
                        ptr_phy_param->s_ov_table_a.u32_n,u32_indx,
                        u32_reg+u32PHY2P1BankOffset,u16_val|u16_ov_en_msk,u16_msk);
                        */
                }
            }
        }
        else if (u8_bit<7)
        {
            s_ov_element* ptr_ov_tb = ptr_phy_param->s_ov_table_a.ptr_ov_table;
            for(u32_indx=0;u32_indx<ptr_phy_param->s_ov_table_a.u32_n;u32_indx++)
            {
                DWORD u32_reg = (ptr_ov_tb+u32_indx)->u32_reg;
                // MS_U16 u16_msk = (ptr_ov_tb+u32_indx)->u16_msk;
                WORD u16_ov_en_msk = (ptr_ov_tb+u32_indx)->u16_ov_en_msk;
                // MS_U16 u16_val = (ptr_ov_tb+u32_indx)->u16_val[u8_bit];
                // for band 5,6, enable ov directly.
                msWrite2ByteMask(u32_reg+u32PHY2P1BankOffset,u16_ov_en_msk,u16_ov_en_msk);
                /*
                MHAL_HDMIRX_MSG_INFO("** HDMI REG_W2, %x,%x|%x,%x\n",
                    ptr_phy_param->s_ov_table_a.u32_n,u32_indx,
                    u32_reg+u32PHY2P1BankOffset,u16_ov_en_msk);
                    */
            }
        }
        else
        {
            //HDMI_HAL_DPRINTF("[%s]%d ** HDMI Error, %x,%x\n",
           // __FUNCTION__, __LINE__,u16_reg_band,u8_bit);
            ret = FALSE;
        }
    }
    else
    {
        DWORD u32_indx = 0;
        s_ov_element* ptr_ov_tb = ptr_phy_param->s_ov_table_a.ptr_ov_table;
        for(u32_indx=0;u32_indx<ptr_phy_param->s_ov_table_a.u32_n;u32_indx++)
        {
            DWORD u32_reg = (ptr_ov_tb+u32_indx)->u32_reg;
            // MS_U16 u16_msk = (ptr_ov_tb+u32_indx)->u16_msk;
            WORD u16_ov_en_msk = (ptr_ov_tb+u32_indx)->u16_ov_en_msk;
            // MS_U16 u16_val = (ptr_ov_tb+u32_indx)->u16_val[u8_bit];

            msWrite2ByteMask(u32_reg+u32PHY2P1BankOffset,0,u16_ov_en_msk);
        }
    }
/*
    u16_tmp_2 = R2BYTE(REG_PHY2P1_2_P0_26_L+u32PHY2P1BankOffset);
    MHAL_HDMIRX_MSG_INFO("[MJ]%x,%x\n",u16_tmp_1,u16_tmp_2);
  */
    return ret;
}
#endif

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMIRx_GetSCDC_Tx5V_PwrDetectFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_BOOL Hal_HDMIRx_GetSCDC_Tx5V_PwrDetectFlag(MS_U8 enInputPortType)
{
    MS_BOOL bCableDetectFlag = FALSE;
    BYTE u8HDMI_DDC_Mapping = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortType);

    switch(u8HDMI_DDC_Mapping)
    {
        case HDMI_DDC_0:
        #if HDMI_Cable5v_0
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P0_0C_L) & BIT(14)) ?TRUE: FALSE); // scdc_0C[14]: reg_tx_5v_pwr_c
        #else
                bCableDetectFlag = TRUE;
        #endif
            break;

        case HDMI_DDC_1:
        #if HDMI_Cable5v_1
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P1_0C_L) & BIT(14)) ?TRUE: FALSE); // scdc_0C[14]: reg_tx_5v_pwr_c
        #else
                bCableDetectFlag = TRUE;
        #endif
            break;

        case HDMI_DDC_2:
        #if HDMI_Cable5v_2
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P2_0C_L) & BIT(14)) ?TRUE: FALSE); // scdc_0C[14]: reg_tx_5v_pwr_c
        #else
                bCableDetectFlag = TRUE;
        #endif
            break;

        default:
            break;
    }

    //printf("@@@@@bCableDetectFlag = %d  \n",bCableDetectFlag);

    return bCableDetectFlag;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMIRx_Set_RB_PN_Swap()
//  [Description]
//                  This function is used for HDMI RB Swap, or PN Swap;
//                  In further, it can be added more swap setting.
//  [Arguments]:
//                  HDMI_SWAP_TYPE:
//                  HDMI_SWAP_TYPE_NONE: nothing swap.
//                  HDMI_SWAP_TYPE_PN: need PN swap.
//                  HDMI_SAWP_TYPE_RB: need RB swap.
//  [Return]:
//                  Bool: return the swap result.
//**************************************************************************
Bool Hal_HDMIRx_Set_RB_PN_Swap(BYTE ucPortIndex __attribute__ ((unused)), HDMI_SWAP_TYPE enHDMI_SWAP_TYPE, Bool bSwapEnable)
{
    Bool bSwapDone = FALSE;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    DWORD u32DTOPBankOffset = 0; //_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

    switch(enHDMI_SWAP_TYPE)
    {
        case HDMI_SWAP_TYPE_PN:
            msWrite2ByteMask(REG_PHY2P1_3_P0_66_L +u32PHY2P1BankOffset, bSwapEnable?0:0x000F, BMASK(3:0));//reg_lane_pn_swap[3:0]
            bSwapDone = TRUE;
            break;

        case HDMI_SAWP_TYPE_RB:
            msWrite2ByteMask(REG_HDMIRX_DTOP_P0_40_L +u32DTOPBankOffset, bSwapEnable?BIT(7):0, BIT(7)); //hdmirx_dtop_30[7]: switch channel order: 1'b1:={ch0,ch1,ch2}; 1'b0:={ch2,ch1,ch0}
            bSwapDone = TRUE;
            break;

        case HDMI_SWAP_TYPE_NONE:
        default:
            break;
    }

    return bSwapDone;
}

