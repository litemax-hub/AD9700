#include <stdio.h>
#include <string.h>
#include <types.h>
#include "mhal_hdmiRx.h"
//#include "Utl.h"
#include "Ms_rwreg.h"
//#include "ms_reg_TSUMR2.h"
#include "Common.h" //for printf
#include "msEread.h"
#include "board.h"
#include "MsOS.h"
#define HDMI_PHY_OLD_PATCH                  0
#define MT9701_TBD_ENABLE   1
#define AUDIO_FREQENCY_LOG FALSE

//MT9701_TBD_ENABLE TBD
//BYPASS_SPECIAL_REPETITION

#if ENABLE_BRINGUP_LOG
#define HDMI_DEBUG_MESSAGE_HAL      1
#define REGEN_LOG TRUE
#else
#define HDMI_DEBUG_MESSAGE_HAL      0
#define REGEN_LOG FALSE
#endif
#if(HDMI_DEBUG_MESSAGE_HAL )
#define HDMI_HAL_DPUTSTR(str)            printMsg(str)
#define HDMI_HAL_DPRINTF(str, x)         printData(str, x)
#else
#define HDMI_HAL_DPUTSTR(str)
#define HDMI_HAL_DPRINTF(str, x)
#endif

#define H14_3GFULLRATE_PATCH  1
#define DLPF_DIV2  0    //U01 H14 = 1 & 0.95V;  H20 = 0 & 1.05V

#define DC_RST_MINI_INTERVAL_MS (20)
#define HDMI_RESET_PKT_INTERVAL_MS (10)
#define PLLTOP_IVS_OVS_TOLERANCE_REGEN_DONE  1 // <=1 done,
#define PLLTOP_IVS_OVS_TOLERANCE_REGEN_REDO 8 // > 8 redo,
#define PLLTOP_IVS_OVS_TOLERANCE_REGEN_REDO_DEBOUNCE_CNT 3 // > 3 redo,
#define PLLTOP_IVS_OVS_TOLERANCE_REGEN_REDO_DEBOUNCE_CNT_REPITION 10 // > 3 redo,
#define PLLTOP_STABLE_LOCK 0xF4

#define AUPLL_CLK_340M      34000//unit 10k
#define AUPLL_CLK_170M      17000
#define AUPLL_CLK_85M        8500
#define AUPLL_CLK_42500K     4250
#define AUPLL_CLK_21250K     2125

#define MULPLCATION_1     1
#define MULPLCATION_2     2
#define MULPLCATION_4     4
#define MULPLCATION_8     8

//MT9701_TBD_ENABLE TBD check MT9701 ref pll
#define PLL_TOP_REF_CLK 226492416 // 432*pow(2,19)=(ASIC ref clk)*pow(2,19)
#define MULTIPLE_SHIT_FACTOR 4

#define TMDSPLL_LOCK_MAX_CNT 10

#define PHY_SW_VERSION 0x0208

#define HDE_CHANGE_Count        5U

TimingChg_CALL_BACK_FUNC pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc = NULL;

#if (!ENABLE_HDMI_SCDC)
#warning "If HDMI need to support over 3.4G(HDMI_v2.0>3.4G, HDMI_v1.x<3.4G), please enable SCDC."
#endif

//#define HDMI1P4_ANALOG_SETTING_OV_EN  //MT9700 CDC issue fixed,so removed

static MS_U16 usTemp1 = 0;
static MS_U8 ubTemp2= 0;
static MS_U64 U64reset_pkt_timer = 0;

MS_U32 _KHal_HDMIRx_GetPMSCDCBankOffset(MS_U8 enInputPortType)
{
    MS_U32 u32PortBankOffset = 0;

    switch (enInputPortType)
    {
        case HDMI_INPUT_PORT0: // BK 0x31
            u32PortBankOffset = 0;
            break;

        case HDMI_INPUT_PORT1: // BK 0x32
            u32PortBankOffset =  0x100;
            break;

        default:
            break;
    };

    return u32PortBankOffset;
}

MS_BOOL KHAL_HDMIRX_RESET_PKT_PERIOD_IF_VALID(void)
{
    if(MsOS_Timer_DiffTime(MsOS_GetSystemTime(),U64reset_pkt_timer) >= HDMI_RESET_PKT_INTERVAL_MS)
    {
        U64reset_pkt_timer=MsOS_GetSystemTime();
        return TRUE;
    }
    return FALSE;
}

//MT9701_TBD_ENABLE TBD this function very strange
inline static MS_U32 abs_diff(MS_U32 u32_a,MS_U32 u32_b)
{
    return (u32_a>u32_b)?u32_a-u32_b:u32_b-u32_a;
}

static void _KHal_HDMIRx_IRQ_OnOff(EN_KDRV_HDMIRX_INT e_int,MS_U8 enInputPortType,unsigned int bit_msk,MS_BOOL irqOnOff)
{
    unsigned int r_val = 0;

    MHAL_HDMIRX_MSG_INFO("[%x]%s,%x,%x,%x\n",
                          enInputPortType, __FUNCTION__,e_int,bit_msk,irqOnOff);

    switch (e_int)
        {
        case HDMI_IRQ_PHY:
            {
                if(irqOnOff)
                {
                    vIO32WriteFld_1(REG_PHY2P1_1_P0_5E_L, 0x1, FLD_BIT(bit_msk)); //clr phy irq status
                    vIO32WriteFld_1(REG_PHY2P1_1_P0_5E_L, 0, FLD_BIT(bit_msk)); //clr phy irq status
                    vIO32WriteFld_1(REG_PHY2P1_1_P0_5C_L, 0, FLD_BIT(bit_msk)); //unmask
                }
                else
                {
                    vIO32WriteFld_1(REG_PHY2P1_1_P0_5C_L, 1, FLD_BIT(bit_msk)); //mask
                }

                // TBD
            }
            break;
       case HDMI_IRQ_MAC:
            {
                MS_U32 u32_bk_ofs = 0;
                r_val = u4IO32ReadFld_1(REG_0120_HDMIRX_INNER_MISC_0 + u32_bk_ofs,REG_0120_HDMIRX_INNER_MISC_0_REG_INNER_MISC_ARM_IRQ_MASK);
                r_val = irqOnOff?(r_val&(~bit_msk)):(r_val|bit_msk);
                vIO32WriteFld_1(REG_0120_HDMIRX_INNER_MISC_0 + u32_bk_ofs, r_val, FLD_BMSK(10:0));
            }
            break;
       case HDMI_IRQ_PKT_QUE:
            {
                MS_U32 u32_bk_ofs = _KHal_HDMIRx_GetPKTQUEBankOffset(enInputPortType);
                MS_U32 u32_l = u4IO32ReadFld_1(REG_0114_P0_HDMIRX_PKT_QUE_TOP_0 + u32_bk_ofs,REG_0114_P0_HDMIRX_PKT_QUE_TOP_0_REG_HDMIRX_PKT_QUE_IRQ_MASK_0);
                MS_U32 u32_h = u4IO32ReadFld_1(REG_0118_P0_HDMIRX_PKT_QUE_TOP_0 + u32_bk_ofs,REG_0118_P0_HDMIRX_PKT_QUE_TOP_0_REG_HDMIRX_PKT_QUE_IRQ_MASK_1);
                r_val = (u32_h<<16)|u32_l;
                r_val = irqOnOff?(r_val&(~bit_msk)):(r_val|bit_msk);
                vIO32WriteFld_1(REG_0114_P0_HDMIRX_PKT_QUE_TOP_0 + u32_bk_ofs, r_val&0xffff, REG_0114_P0_HDMIRX_PKT_QUE_TOP_0_REG_HDMIRX_PKT_QUE_IRQ_MASK_0);
                vIO32WriteFld_1(REG_0118_P0_HDMIRX_PKT_QUE_TOP_0 + u32_bk_ofs, (r_val>>16)&0xffff, REG_0118_P0_HDMIRX_PKT_QUE_TOP_0_REG_HDMIRX_PKT_QUE_IRQ_MASK_1);
            }
            break;
       case HDMI_IRQ_PM_SQH_ALL_WK:
            {
                // TBD
            }
            break;
       case HDMI_IRQ_PM_SCDC:
            {
 /*
410_2c[3:0], force
410_2c[7:4], mask
410_2c[11:8], status
 */
                MS_U32 u32_bk_ofs = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortType);
                r_val  =  u4IO32ReadFld_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs, REG_00B0_SCDC_P0_REG_SCDC_IRQ_ARM_MASK);
                r_val = irqOnOff?(r_val&(~bit_msk)):(r_val|bit_msk);
                vIO32WriteFld_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs, r_val, REG_00B0_SCDC_P0_REG_SCDC_IRQ_ARM_MASK);
            }
            break;
       case HDMI_FIQ_CLK_DET_0:
       case HDMI_FIQ_CLK_DET_1:
       case HDMI_FIQ_CLK_DET_2:
       case HDMI_FIQ_CLK_DET_3:
            {
                // TBD
            }
            break;
       default:
// unknown, error
            MHAL_HDMIRX_MSG_ERROR("%s,%d,%x,%x,%x,%x\r\n",__FUNCTION__,__LINE__,e_int,bit_msk,enInputPortType,irqOnOff);
        }
}

#if (ENABLE_HDMI || ENABLE_DVI)
static void _mhal_HDCP22_SetReAuth(MS_BOOL bEnReAuth)
{
    //printf("%s[HDMI][%s][%d] reauth:%d %s\n", "\033[0;32;31m", __FUNCTION__, __LINE__,bEnReAuth, "\033[m");
    if (bEnReAuth == TRUE)
    {
        // hdmirx_hdcp_65[10]: reg_reauth_ov_en
        vIO32WriteFld_1(REG_0194_P0_HDCP , 0x1, REG_0194_P0_HDCP_REG_REAUTH_OV_EN);
        // hdmirx_hdcp_65[11]: reg_reauth_ov_value
        vIO32WriteFld_1(REG_0194_P0_HDCP , 0x1, REG_0194_P0_HDCP_REG_REAUTH_OV_VALUE);
        // hdmirx_hdcp_65[10]: reg_reauth_ov_en
        vIO32WriteFld_1(REG_0194_P0_HDCP , 0, REG_0194_P0_HDCP_REG_REAUTH_OV_EN);
    }
    else
    {
        // hdmirx_hdcp_65[12]: reg_reauth_clr
        vIO32WriteFld_1(REG_0194_P0_HDCP , 0x1, REG_0194_P0_HDCP_REG_REAUTH_CLR);
    }
}
#endif
MS_BOOL KHal_HDMIRx_Get_Pkt_Diff(MS_U8 enInputPortType, MS_HDMI_PACKET_STATE_t e_pkt)
{
    UNUSED(enInputPortType);
    MS_U32 u32_pkt_dep_1_ofs = _KHal_HDMIRx_GetPKTBankOffset(enInputPortType);
    MS_U32 u32_pkt_dec_ofs = _KHal_HDMIRx_GetPKTBankOffset(enInputPortType);
    MS_U16 u16_tmp = 0;

    switch (e_pkt)
    {
        case PKT_AVI:
            u16_tmp = u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs);
            u16_tmp &= (1<<E_HDMI_PKT_AVI_DIFF_BIT);
            vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs,u16_tmp);

            break;
        case PKT_GC:
            u16_tmp = u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs);
            u16_tmp &= (1<<E_HDMI_PKT_GCP_DIFF_BIT);
            vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs,u16_tmp);

            break;
        case PKT_SPD:
            u16_tmp = u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs);
            u16_tmp &= (1<<E_HDMI_PKT_SPD_DIFF_BIT);
            vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs,u16_tmp);

            break;
        case PKT_RSV:
            u16_tmp = u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs);
            u16_tmp &= (1<<E_HDMI_PKT_RSV_DIFF_BIT);
            vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32_pkt_dec_ofs,u16_tmp);

            break;
        case PKT_AUI:
            u16_tmp = u2IO32Read2B_1(REG_00AC_P0_HDMIRX_PKT_DEP_1 + u32_pkt_dep_1_ofs);
            u16_tmp &= (1<<E_HDMI_PKT_AUI_DIFF_BIT);
            vIO32Write2B_1(REG_00AC_P0_HDMIRX_PKT_DEP_1 + u32_pkt_dep_1_ofs,u16_tmp);

            break;
        case PKT_ACR:
            u16_tmp = u2IO32Read2B_1(REG_00AC_P0_HDMIRX_PKT_DEP_1 + u32_pkt_dep_1_ofs);
            u16_tmp &= (1<<E_HDMI_PKT_ACR_DIFF_BIT);
            vIO32Write2B_1(REG_00AC_P0_HDMIRX_PKT_DEP_1 + u32_pkt_dep_1_ofs,u16_tmp);

            break;
        case PKT_ACP:
            u16_tmp = u2IO32Read2B_1(REG_00AC_P0_HDMIRX_PKT_DEP_1 + u32_pkt_dep_1_ofs);
            u16_tmp &= (1<<E_HDMI_PKT_ACP_DIFF_BIT);
            vIO32Write2B_1(REG_00AC_P0_HDMIRX_PKT_DEP_1 + u32_pkt_dep_1_ofs,u16_tmp);

            break;
        default:
            break;
    }
    return u16_tmp>0?TRUE:FALSE;
}
//**************************************************************************
//  [Function Name]:
//                  KHal_HDMIRx_GetSCDC_Tx5V_Pwr_DetectFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_BOOL KHal_HDMIRx_GetSCDC_Tx5V_Pwr_DetectFlag(MS_U8 enInputPortType)
{
    MS_BOOL bCableDetectFlag = FALSE;
    MS_U32 u32PMSCDCBankOffset = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortType);

    bCableDetectFlag = ((u2IO32Read2B_SCDC(REG_0030_SCDC_P0 + u32PMSCDCBankOffset) & BIT(14)) ? TRUE : FALSE);//scdc_0C[14]: reg_tx_5v_pwr_c
    //printf("@@@@@bCableDetectFlag = %d  \n",bCableDetectFlag);

    return bCableDetectFlag;
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

    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPKTDECBankOffset(0);

    // [3:0]: Pixel repetition
    //ucTmpValue = (msReadByte(REG_173084 + wOffset_hdmi) & 0x0F); // hdmi_dual_42[3:0]: pixel repetition
    ucTmpValue = u4IO32ReadFld_1(REG_008C_P0_HDMIRX_PKT_DEC + u32PortBankOffset,REG_008C_P0_HDMIRX_PKT_DEC_REG_AVI_PB05) & BMASK(3:0);//AVI InfoFrame byte 5.[3:0]: Pixel repetition; pixel sent (PR+1) times.

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

MS_BOOL _KHAL_HDMIRX_Is_AVIRepetition_Diff(ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo, MS_U8 enInputPortType)
{
    MS_U8 u8_curr_repetition=0;

    u8_curr_repetition = Hal_HDMI_avi_infoframe_info(enInputPortType, _BYTE_5, NULL) & BMASK(3 : 0);

    if(u8_curr_repetition != pstHDMIPollingInfo->u8_prev_repetition)
    {
        pstHDMIPollingInfo->u8_prev_repetition = u8_curr_repetition;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL KHAL_HDMIRx_VDEChange(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo)
{
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U16 u16_vde = GET_DTOP_DEC_VDE(u32_dtop_dec_bkofs);
    if(stHDMIPollingInfo->u16PreVDE!=u16_vde)
    {
#if REGEN_LOG
        MHAL_HDMIRX_MSG_INFO("===KHAL_HDMIRx_VDEChange stHDMIPollingInfo->u16PreVDE 0x[%x] u16_vde[%x]===\n",stHDMIPollingInfo->u16PreVDE,u16_vde);
#endif
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL KHAL_HDMIRx_HTTChange(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo)
{
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U16 u16_htt = GET_DTOP_DEC_HTT(u32_dtop_dec_bkofs);

    if(stHDMIPollingInfo->u16PreHTT!=u16_htt)
    {
#if REGEN_LOG
        MHAL_HDMIRX_MSG_INFO("===KHAL_HDMIRx_HTTChange stHDMIPollingInfo->u16PreHTT 0x[%x] u16_htt[%x]===\n",stHDMIPollingInfo->u16PreHTT,u16_htt);
#endif
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL KHAL_HDMIRx_VTTChange(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo)
{
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U16 u16_vtt = GET_DTOP_DEC_VTT(u32_dtop_dec_bkofs);

    if(stHDMIPollingInfo->u16PreVTT!=u16_vtt)
    {
    //MHAL_HDMIRX_MSG_INFO("===KHAL_HDMIRx_HTTChange stHDMIPollingInfo->u16PreVTT 0x[%x] u16_vtt[%x]===\n",stHDMIPollingInfo->u16PreVTT,u16_vtt);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL KHAL_HDMIRx_VTTDiff(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo)
{
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U16 u16_vtt = GET_DTOP_DEC_VTT(u32_dtop_dec_bkofs);

    if(abs(stHDMIPollingInfo->u16PreVTT-u16_vtt) > VTT_DIFF_THRESHOLD)
    {
    //MHAL_HDMIRX_MSG_INFO("===KHAL_HDMIRx_HTTChange stHDMIPollingInfo->u16PreVTT 0x[%x] u16_vtt[%x]===\n",stHDMIPollingInfo->u16PreVTT,u16_vtt);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL KHAL_HDMIRx_VSBDiff(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo)
{
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U16 u16_vsb = GET_DTOP_DEC_VSYNC(u32_dtop_dec_bkofs)+GET_DTOP_DEC_VBACK(u32_dtop_dec_bkofs);

    if(abs(stHDMIPollingInfo->u16PreVSB-u16_vsb) > VSB_DIFF_THRESHOLD)
    {
    //MHAL_HDMIRX_MSG_INFO("===KHAL_HDMIRx_HTTChange stHDMIPollingInfo->u16PreVTT 0x[%x] u16_vtt[%x]===\n",stHDMIPollingInfo->u16PreVTT,u16_vtt);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void KHAL_HDMIRX_Get_Interlace_Information(MS_U8 enInputPortType,ST_HDMI_INTERLACE_INFO *st_interlace_info)
{
    //MHAL_HDMIRX_MSG_INFO("====%s====,enInputPortType[%d]\n",__FUNCTION__,enInputPortType);
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);

    st_interlace_info->u8Field=u4IO32ReadFld_1(REG_0110_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs,REG_0110_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_SRC_DET_I_MD_ODD_FIELD);
    st_interlace_info->u16VTTValue=GET_DTOP_DEC_VTT(u32_dtop_dec_bkofs);

    SET_DTOP_DEC_OI_MODE(u32_dtop_dec_bkofs);
    st_interlace_info->u8OiMd=u4IO32ReadFld_1(REG_0110_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs,REG_0110_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_SRC_DET_I_MD_VTT_EQ);
}

MS_U16 KHal_HDMIRX_Get_Min_VTT(MS_U8 enInputPortType,ST_HDMI_INTERLACE_INFO *st_interlace_info)
{
#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("====%s====,enInputPortType[%d]\n",__FUNCTION__,enInputPortType);
#endif
    MS_U16 u16VTTValue=0;

    KHAL_HDMIRX_Get_Interlace_Information(enInputPortType,st_interlace_info);

    if(st_interlace_info->u8OiMd)
    {
        u16VTTValue=st_interlace_info->u16VTTValue;
    }
    else
    {
        if(st_interlace_info->u8Field==1)
            u16VTTValue=st_interlace_info->u16VTTValue;
        else
            u16VTTValue=st_interlace_info->u16VTTValue-1;
    }

#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("st_interlace_info->u8OiMd is [%x],st_interlace_info->u8Field[%x],u16VTTValue[%x]\n",st_interlace_info->u8OiMd,st_interlace_info->u8Field,u16VTTValue);
#endif
    return u16VTTValue;
}

MS_BOOL _KHal_HDMIRx_Pll_TOP_Stable(MS_U8 enInputPortType, MS_U32 u32_tol)
{
    UNUSED(enInputPortType);
    MS_U32 u32_plltop_bkofs = _KHal_HDMIRx_GetPLLTOPBankOffset(enInputPortType);
    MS_U16 u16_plltop_lock_status = u4IO32ReadFld_1(REG_00A8_P0_PLL_TOP + u32_plltop_bkofs, FLD_BMSK(8:0));

    MS_U32 u32_ivs =
            (u4IO32ReadFld_1(REG_0084_P0_PLL_TOP + u32_plltop_bkofs,
            REG_0084_P0_PLL_TOP_REG_IVS_PRD_VALUE_0) |
             (u4IO32ReadFld_1(REG_0088_P0_PLL_TOP + u32_plltop_bkofs,
             REG_0088_P0_PLL_TOP_REG_IVS_PRD_VALUE_1) << 16));

    MS_U32 u32_ovs =
            (u4IO32ReadFld_1(REG_008C_P0_PLL_TOP + u32_plltop_bkofs,
            REG_008C_P0_PLL_TOP_REG_OVS_PRD_VALUE_0) |
             (u4IO32ReadFld_1(REG_0090_P0_PLL_TOP + u32_plltop_bkofs,
             REG_0090_P0_PLL_TOP_REG_OVS_PRD_VALUE_1) << 16));

    /*
    if (u32_tol == PLLTOP_IVS_OVS_TOLERANCE_REGEN_DONE)
    {
        MHAL_HDMIRX_MSG_DEBUG("%x,%x,%x,%x\n",u16_plltop_lock_status,u32_ivs,u32_ovs,abs_diff(u32_ivs,u32_ovs) );
    }
    */
    if ((u16_plltop_lock_status&PLLTOP_STABLE_LOCK) != PLLTOP_STABLE_LOCK)
    {
        return FALSE;
    }

    if (abs_diff(u32_ivs,u32_ovs) > u32_tol)
    {
        return FALSE;
    }

    return TRUE;
}

MS_BOOL _KHal_IsVRR(MS_U8 enInputPortType)
{
    UNUSED(enInputPortType);
    MS_U32 u32EMPBankOffset = _KHal_HDMIRx_GetEMPBankOffset(enInputPortType);
    MS_U32 u32_val = u4IO32ReadFld_1(REG_0008_P0_HDMI_EMP + u32EMPBankOffset, REG_0008_P0_HDMI_EMP_REG_EMP_VRR_ENABLE);
    return u32_val?TRUE:FALSE;
}

void CLR_DTOP_DEC_OI_MODE(void)
{
    //MHAL_HDMIRX_MSG_INFO("=====CLR_DTOP_DEC_OI_MODE[%x]=====\n",port_ofs);
   vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC , 0x0,Fld(1, 12, AC_MSKB0));
}

static void _KHal_HDMIRx_RegenTimingProc_Rst(ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo,MS_U8 enInputPortType)
{
    enInputPortType = enInputPortType;
    MS_U32 u32_dscdservice_bkofs = _KHal_HDMIRx_GetDSCDSERVICEPBankOffset(enInputPortType);
    //MS_U32 u32PHY2P1BankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortType);

    vIO32WriteFld_1(REG_00B8_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                    0,
                    REG_00B8_P0_DSCD_SERVICES_REG_RCV_TOP_IVS_LATENCY_VAL);

    vIO32WriteFld_1(REG_0060_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                    0,
                    REG_0060_P0_DSCD_SERVICES_REG_HREF_FORCE);

    pstHDMIPollingInfo->u8RegenFsm = E_REGEN_INIT;
    pstHDMIPollingInfo->u8RegenCount = 0;
    pstHDMIPollingInfo->u8RegenReDoDebounceCnt = 0;
    CLR_DTOP_DEC_OI_MODE();
}

MS_U32 _KHal_HDMIRx_Calc_Rcv_Gen_Pix_Clk_KHz(
    MS_U32 u32_htt,
    MS_U32 u32_ivs_div,
    MS_U32 u32_ivs,
    MS_U8 u8_engine_n,
    MS_U8 u8_pix_rep)
{
// r ivs,
// r ivs_div
// r gen_htt
// gen_pix_clk_mhz = gen_htt*(ivs_div+1)*12/(ivs*u8_engin_n)

// n=f(gen_pix_clk_MHz)
// pll_set = PLL_TOP_REF_CLK/gen_pix_clk_mhz >> n

    MS_U32 u32_tmp = 0x00;

    if ((u32_ivs == 0)||(u8_engine_n == 0) || (u8_pix_rep==0))
    {
        u32_tmp = 0;
    }
    else
    {
        u32_tmp = u32_htt*(u32_ivs_div+1)*12*1000/(u32_ivs*u8_engine_n*u8_pix_rep);
    }

    MHAL_HDMIRX_MSG_DEBUG("%s,%x,%x,%x,%x,%x, %d(KHz)\n",__FUNCTION__,
        u32_htt,u32_ivs_div,u32_ivs,u8_engine_n,u8_pix_rep,
        u32_tmp);

    return u32_tmp;
}

void _KHal_HDMIRx_Clk_Mux_Sel(EN_HDMI_CLK_MUX_SEL mux_sel,
                                     MS_U8 enInputPortType, MS_U32 u32_p0, MS_U32 u32_p1)
{
    MS_U32 u32Portofs =_KHal_HDMIRx_GetCLKGENDRV0BankOffset(enInputPortType);
    MS_U8 u8_clk_mux_port_ofs = 0;//enInputPortType;
    enInputPortType = enInputPortType;
    //MHAL_HDMIRX_MSG_INFO("[%x]_KHal_HDMIRx_Clk_Mux_Sel=%x,%x,%x\n",
    //                      enInputPortType, mux_sel, u32_p0, u32_p1);

    switch (mux_sel)
    {
    case E_CLK_MUX_TIMING_DET_DTOP_DEC:
        /* mux_sel = 1, pi_clk_pix_p_ck
                mux_sel = 2, pi_clk_tmds_p_ck
                mux_sel = 3, pi_clk_link_p_ck
                mux_sel = 4, pi_clk_pix_tmdspll_p_ck */
        if (u32_p1 == HDMI_TIMING_DET_DTOPDEC_DC_AUTO)
        {
            u32_p0 = u32_p0 ? 0x3 : 0x1;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEC_HDCP_AUTO)
        {
            u32_p0 = u32_p0 ? 0x3 : 0x2;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEC_DC_1P)
        {
            u32_p0 = 0x1;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEC_DC_8P)
        {
            u32_p0 = 0x3;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEC_HDCP_1P)
        {
            u32_p0 = 0x2;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEC_HDCP_8P)
        {
            u32_p0 = 0x3;
        }
        vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                        (u32_p0 << 2),
                        REG_0044_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DEC_DRV_OV);
        break;
    case E_CLK_MUX_TIMING_DET_DTOP_DEP:
        /* mux_sel = 1, pi_clk_pix_p_ck
                mux_sel = 2, pi_clk_tmds_p_ck
                mux_sel = 3, pi_clk_link_p_ck
                mux_sel = 4, pi_clk_pix_tmdspll_p_ck */
        if (u32_p1 == HDMI_TIMING_DET_DTOPDEP_AUTO)
        {
            u32_p0 = u32_p0 ? 0x4 : 0x1;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEP_DITHER_1P)
        {
            u32_p0 = 0x1;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEP_RCVTOP)
        {
            u32_p0 = 0x4;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DTOPDEP_SC)
        {
            u32_p0 = 0x4;
        }
        vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                        (u32_p0 << 2),
                        REG_0044_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DEP_DRV_OV);

        break;
    case E_CLK_MUX_TIMING_DET_DTOPMISC:
        /* mux_sel = 0, pi_clk_pix_p_ck
                mux_sel = 1, pi_clk_tmds_p_ck
                mux_sel = 2, pi_clk_link_p_ck
                mux_sel = 3, pi_clk_pix_tmdspll_p_ck */

        if (u32_p1 == HDMI_TIMING_DET_DC_1P)
        {
            u32_p0 = 0x0;
        }
        else if (u32_p1 == HDMI_TIMING_DET_HDCP_1P)
        {
            u32_p0 = 0x1;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DITHER_1P)
        {
            u32_p0 = 0x0;
        }
        else if (u32_p1 == HDMI_TIMING_DET_DC_8P)
        {
            u32_p0 = 0x2;
        }
        else if (u32_p1 == HDMI_TIMING_DET_HDCP_8P)
        {
            u32_p0 = 0x2;
        }
        else if (u32_p1 == HDMI_TIMING_DET_RCV_TOP)
        {
            u32_p0 = 0x3;
        }
        else if (u32_p1 == HDMI_TIMING_DET_SC)
        {
            u32_p0 = 0x3;
        }
        vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                        (u32_p0 << 2),
                        REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DTOP_DRV_OV);

        /* mux_sel = 0, 24mhz
                   mux_sel = 1, p0
                   mux_sel = 2, p1
                   mux_sel = 3, p2
                   mux_sel = 4, p3*/
        vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_1,
                        (u8_clk_mux_port_ofs + 1) << 2,
                        REG_0044_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TIMING_DET_DTOP_DRV_OV);
        break;
    case E_CLK_MUX_AUDIO_S0:
        /*
        [M6]
        mux_sel = 0, p0_clk_tmds_p_ck
        mux_sel = 1, p1_clk_tmds_p_ck
        mux_sel = 2, p2_clk_tmds_p_ck
        mux_sel = 3, p3_clk_tmds_p_ck
        mux_sel = 4, p0_clk_link_p_ck
        mux_sel = 5, p1_clk_link_p_ck
        mux_sel = 6, p2_clk_link_p_ck
        mux_sel = 7, p3_clk_link_p_ck
        [MST9U6]
        mux_sel = 0, p0_clk_tmds/link_p_ck
        mux_sel = 1, p1_clk_tmds/link_p_ck
        mux_sel = 2, p2_clk_tmds/link_p_ck
        mux_sel = 3, p3_clk_tmds/link_p_ck

        0x173e06h[4]-->M6[4]    =1/0:FRL:Legacy,
                       MST9U6[4]become hardware mode(no use),
                                hardware detect FRL/TMDS automatically
                                [3:2]=port select=0 to 3
        */
        u32_p0 =  u8_clk_mux_port_ofs;//u32_p0 = u32_p0 ? (0x4 + u8_clk_mux_port_ofs) : (0x0 + u8_clk_mux_port_ofs);
        vIO32WriteFld_1(REG_0018_HDMIRX_CKGEN_DRIVER_1,
                        (u32_p0 << 2),
                        REG_0018_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S0_0_DRV_OV);
        /*M6/MST9U6 legacy/Frl script both write zero*/
        vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_1,
                        0,
                        REG_0048_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S0_0_DRV_OV);
        break;
    case E_CLK_MUX_AUDIO_S1:
        /*
                mux_sel = 0, p0_clk_tmds_p_ck
                mux_sel = 1, p1_clk_tmds_p_ck
                mux_sel = 2, p2_clk_tmds_p_ck
                mux_sel = 3, p3_clk_tmds_p_ck
                mux_sel = 4, p0_clk_link_p_ck
                mux_sel = 5, p1_clk_link_p_ck
                mux_sel = 6, p2_clk_link_p_ck
                mux_sel = 7, p3_clk_link_p_ck
        */
        /* MST9U6 only has s0
        u32_p0 = u32_p0 ? (0x4 + u8_clk_mux_port_ofs) : (0x0 + u8_clk_mux_port_ofs);
        vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1,
                        (u32_p0 << 2),
                        REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_0_DRV_OV);

        vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_1,
                        (u32_p0 << 2),
                        REG_004C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S1_0_DRV_OV);
        */
        break;
    case E_CLK_MUX_DVI2MIU_S0:
        /*
                mux_sel = 0, p0_clk_tmds_p_ck
                mux_sel = 1, p1_clk_tmds_p_ck
                mux_sel = 2, p2_clk_tmds_p_ck
                mux_sel = 3, p3_clk_tmds_p_ck
                mux_sel = 4, p0_clk_link_p_ck
                mux_sel = 5, p1_clk_link_p_ck
                mux_sel = 6, p2_clk_link_p_ck
                mux_sel = 7, p3_clk_link_p_ck
                */
        u32_p0 = u32_p0 ? (0x4 + u8_clk_mux_port_ofs) : (0x0 + u8_clk_mux_port_ofs);
        vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_1,
                        (u32_p0 << 2),
                        REG_0028_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_0_DRV_OV);
        break;
    case E_CLK_MUX_DVI2MIU_S1:
        /*
                mux_sel = 0, p0_clk_tmds_p_ck
                mux_sel = 1, p1_clk_tmds_p_ck
                mux_sel = 2, p2_clk_tmds_p_ck
                mux_sel = 3, p3_clk_tmds_p_ck
                mux_sel = 4, p0_clk_link_p_ck
                mux_sel = 5, p1_clk_link_p_ck
                mux_sel = 6, p2_clk_link_p_ck
                mux_sel = 7, p3_clk_link_p_ck
                */
        u32_p0 = u32_p0 ? (0x4 + u8_clk_mux_port_ofs) : (0x0 + u8_clk_mux_port_ofs);
        vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1,
                        (u32_p0 << 2),
                        REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_0_DRV_OV);
        break;

    case E_CLK_MUX_DVI2MIU_S2:
        /*
                mux_sel = 0, p0_clk_tmds_p_ck
                mux_sel = 1, p1_clk_tmds_p_ck
                mux_sel = 2, p2_clk_tmds_p_ck
                mux_sel = 3, p3_clk_tmds_p_ck
                mux_sel = 4, p0_clk_link_p_ck
                mux_sel = 5, p1_clk_link_p_ck
                mux_sel = 6, p2_clk_link_p_ck
                mux_sel = 7, p3_clk_link_p_ck
                */
        u32_p0 = u32_p0 ? (0x4 + u8_clk_mux_port_ofs) : (0x0 + u8_clk_mux_port_ofs);
        vIO32WriteFld_1(REG_00B4_HDMIRX_CKGEN_DRIVER_1,
                        (u32_p0 << 2),
                        REG_00B4_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S2_0_DRV_OV);
        break;

    case E_CLK_MUX_TMDS_FRL:
        if(u32_p0 == 0)
        {
            vIO32WriteFld_1(REG_0018_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x1,
                            REG_0018_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_PIX_RCV_3RD_OVE);
            vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x1,
                            REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_TIMING_DET_DEC_2ND_OVE);
            vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x1,
                            REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_TIMING_DET_DEC_3RD_OVE);
            /*
            vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x4,
                            REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_DRV_OV);
                            */
            vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0,
                            REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_OVE);
        }
        else if (u32_p0 == 1)
        {
            vIO32WriteFld_1(REG_0018_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x0,
                            REG_0018_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_PIX_RCV_3RD_OVE);
            vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x0,
                            REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_TIMING_DET_DEC_2ND_OVE);
            vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x0,
                            REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_TIMING_DET_DEC_3RD_OVE);
            /*
            vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0x8,
                            REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_DRV_OV);
                            */
            vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0,
                            REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_OVE);
        }
        else
        {
        // cr unlock 24M xtal
            vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            0,
                            REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_DRV_OV);
            vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs,
                            1,
                            REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_OVE);
        }
        break;
    case E_CLK_MUX_DSC:
        /*
                mux=1, p0
                mux=2, p1
                mux=3, p2
                mux=4, p3
                */
        /*REG_CKG_LINK_DSCD_DRV_OV/PIX_TMDSPLL_DSCD_DRV_OV/CKG_TMDS_DSCD_DRV_OV
          are not used in MST9U6,the path are take off.
          But designer suggest to keep M6 setting
        */
        u32_p0 = (u8_clk_mux_port_ofs+1)<<2;
        vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_1,
                        u32_p0,
                        REG_003C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_LINK_DSCD_DRV_OV);
        vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_1,
                        u32_p0,
                        REG_003C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_PIX_TMDSPLL_DSCD_DRV_OV);
        vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1,
                        u32_p0,
                        REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_DSCD_DRV_OV);
        break;
    default:
        break;
    }
}

void _KHal_HDMIRx_dtop_dec_misc_timing_det_param_chg(
    MS_U8 enInputPortType,
    EN_HDMI_TIMING_DETECT_PATH_DTOPDEC enTimingPath)
{
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U32 u32_tolerance = 0;

    switch (enTimingPath)
    {
        case HDMI_TIMING_DET_DTOPDEC_HDCP_AUTO:
        case HDMI_TIMING_DET_DTOPDEC_HDCP_1P:
        case HDMI_TIMING_DET_DTOPDEC_HDCP_8P:
            {
                u32_tolerance = 2;
            }
            break;
        case HDMI_TIMING_DET_DTOPDEC_DC_AUTO:
        case HDMI_TIMING_DET_DTOPDEC_DC_1P:
        case HDMI_TIMING_DET_DTOPDEC_DC_8P:
            {
                u32_tolerance =0;
            }
            break;
        default:
            MHAL_HDMIRX_MSG_ERROR("%s,%d,%x,%x\r\n",__FUNCTION__,__LINE__,enInputPortType,enTimingPath);
    }

    // hde/htt/vde tolerance
    vIO32WriteFld_1(REG_0138_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, u32_tolerance, REG_0138_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HDE_STABLE_TOL);
    vIO32WriteFld_1(REG_0138_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, u32_tolerance, REG_0138_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HTT_STABLE_TOL);
    vIO32WriteFld_1(REG_013C_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, u32_tolerance, REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_VDE_STABLE_TOL);
}

void _KHal_HDMIRx_GetimingDetectPath(
    MS_U8 enInputPortType,
    EN_HDMI_TIMING_DETECT_PATH_DTOPDEC enTimingPath,
    MS_U8 ucSourceVersion)
{
    // dtopdec
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U32 u32_p_n = (ucSourceVersion == HDMI_SOURCE_VERSION_HDMI21) ? 7 : 0;

    vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs,
                    (MS_U32)enTimingPath,
                    REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_SEL);

    vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs,
                    u32_p_n,
                    REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_SRC_DET_PIX_PER_CYCLE);

    _KHal_HDMIRx_Clk_Mux_Sel(E_CLK_MUX_TIMING_DET_DTOP_DEC,
                             enInputPortType, u32_p_n, (MS_U32)enTimingPath);

    _KHal_HDMIRx_dtop_dec_misc_timing_det_param_chg(enInputPortType,enTimingPath);
}

void _KHal_HDMIRx_GetimingDetectPath_dtopdep(
    MS_U8 enInputPortType,
    EN_HDMI_TIMING_DETECT_PATH_DTOPDEP enTimingPath,
    MS_U8 ucSourceVersion)
{
    MS_U32 u32_dtop_dep_bkofs = _KHal_HDMIRx_GetDTOPDEPBankOffset(enInputPortType);
    MS_U32 u32_p_n = (ucSourceVersion == HDMI_SOURCE_VERSION_HDMI21) ? 7 : 0;

    vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs,
                    (MS_U32)enTimingPath,
                    REG_0100_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_TIMING_DET_SEL);

    vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs,
                    u32_p_n,
                    REG_0100_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_SRC_DET_PIX_PER_CYCLE);

    vIO32WriteFld_1(REG_0090_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs,
                    (ucSourceVersion == HDMI_SOURCE_VERSION_HDMI21)?2:1,
                    REG_0090_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_FEED_RCV_I_MD_SEL);

    _KHal_HDMIRx_Clk_Mux_Sel(E_CLK_MUX_TIMING_DET_DTOP_DEP,
                             enInputPortType, u32_p_n, (MS_U32)enTimingPath);
}

void _KHal_HDMIRx_GetimingDetectPath_dtopmisc(
    MS_U8 enInputPortType,
    EN_HDMI_TIMING_DETECT_PATH enTimingPath,
    MS_U8 ucSourceVersion)
{
    MS_U32 u32_dtopmisc_bkofs = _KHal_HDMIRx_GetDTOPMISCBankOffset(enInputPortType);
    MS_U32 u32_p_n = (ucSourceVersion == HDMI_SOURCE_VERSION_HDMI21) ? 7 : 0;
    enInputPortType = 0;

    vIO32WriteFld_1(REG_0100_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs,
                    u32_p_n,
                    REG_0100_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_SRC_DET_PIX_PER_CYCLE);

    vIO32WriteFld_1(REG_0100_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs,
                    (MS_U32)enTimingPath,
                    REG_0100_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_SEL);

    // port sel
    vIO32WriteFld_1(REG_012C_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs,
                    (MS_U32)(enInputPortType),
                    REG_012C_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_PORT_SEL);

    _KHal_HDMIRx_Clk_Mux_Sel(E_CLK_MUX_TIMING_DET_DTOPMISC,
                             enInputPortType, u32_p_n, (MS_U32)enTimingPath);
}


static void _KHal_HDMIRx_EnableAutoScramble(MS_U8 enInputPortType ,MS_BOOL b_onoff)
{
    enInputPortType = enInputPortType;
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortType);
    // dtop_47[0] reg_scmb_flag_sel
    // dtop_47[1] reg_dis_ch_info
    // dtop_47[2] reg_dis_sscp_only
    // dtop_47[3] reg_scmb_status (r)
    // dtop_47[5] reg_wb_rst
    // dtop_47[6] reg_kw_dly_en
    // dtop_47[7] reg_align_src_sel
    // dtop_47[8] reg_wb_scmb_auto
    // dtop_47[9] reg_scmb_en_both
    // dtop_47[11:10] reg_wb_timer_th
    // dtop_47[14] reg_sscp_flg_err
    // dtop_47[15] reg_scmb_flg_err
    if (b_onoff == TRUE)
    {
        vIO32WriteFld_1(REG_011C_P0_HDMIRX_DTOP + u32PortBankOffset, 0x1, REG_011C_P0_HDMIRX_DTOP_REG_DIS_SSCP_ONLY);
        vIO32WriteFld_1(REG_011C_P0_HDMIRX_DTOP + u32PortBankOffset, 0x1, REG_011C_P0_HDMIRX_DTOP_REG_WB_SCMB_AUTO);
        vIO32WriteFld_1(REG_011C_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_011C_P0_HDMIRX_DTOP_REG_SCMB_EN_BOTH);
        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L, BIT(2), BIT(2));
        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L, BIT(8), BIT(8));
        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L, 0x00, BIT(9));
    }
    else
    {
        vIO32WriteFld_1(REG_011C_P0_HDMIRX_DTOP + u32PortBankOffset, 0x1, REG_011C_P0_HDMIRX_DTOP_REG_DIS_SSCP_ONLY);
        vIO32WriteFld_1(REG_011C_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_011C_P0_HDMIRX_DTOP_REG_WB_SCMB_AUTO);
        vIO32WriteFld_1(REG_011C_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_011C_P0_HDMIRX_DTOP_REG_SCMB_EN_BOTH);
        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L, BIT(2), BIT(2));
        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L, 0x00, BIT(8));
        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L, 0x00, BIT(9));
    }
}


static void _KHal_HDMIRx_Bypass422to444(MS_U8 enInputPortType)
{
    enInputPortType = enInputPortType;
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortType);
    // if(R2BYTEMSK(REG_SC_BK79_3F_L, BIT(14))) // verify trim efuse to disable Dolby HDR
    if (FALSE)   // _KHal_422to444_ByPass_Flag
    {
        // bypass, 422to444 at HDMI
        // dtopdep_21[0] reg_bypass_422to444
        // dtopdep_21[1] reg_force_422to444
        vIO32WriteFld_1(REG_0084_P0_HDMIRX_DTOP_DEP_MISC + u32PortBankOffset, 0x1, REG_0084_P0_HDMIRX_DTOP_DEP_MISC_REG_BYPASS_422TO444);
        //msWrite2ByteMask(REG_HDMIRX_DTOP_DEP_MISC_P0_21_L+u32PortBankOffset, BIT(0), BIT(0));
    }
    else
    {
        // Disable bypass, 422to444 at scaler
        // dtopdep_21[0] reg_bypass_422to444
        // dtopdep_21[1] reg_force_422to444
        vIO32WriteFld_1(REG_0084_P0_HDMIRX_DTOP_DEP_MISC + u32PortBankOffset, 0x0, REG_0084_P0_HDMIRX_DTOP_DEP_MISC_REG_BYPASS_422TO444);
        //msWrite2ByteMask(REG_HDMIRX_DTOP_DEP_MISC_P0_21_L+u32PortBankOffset, 0x00, BIT(0));
    }
}

//**************************************************************************
//  [Function Name]:
//                  _mhal_hdmiRx_GetTimingChangeStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static MS_BOOL _KHal_HDMIRx_GetTimingChangeStatus(EN_HDMI_TIMING_CHANGE_STATUS enTimingStatus ,MS_U8 enInputPortType)
{
    // dtopdec
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_BOOL bFlag = FALSE;
    MS_U32 u32_irq_bits = (MS_U32)enTimingStatus;
    //MS_U32 u32_addr_ofs = (u32_irq_bits >> 4) << 2;
    MS_U32 u32_addr_ofs = (u32_irq_bits >> 4) <<1;
    u32_irq_bits = u32_irq_bits % 16;

    if (u4IO32ReadFld_1(
            REG_0030_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_addr_ofs,
            FLD_BIT(u32_irq_bits)))
    {
        bFlag = TRUE;
        vIO32WriteFld_1(
            REG_0030_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_addr_ofs,
            (1 << u32_irq_bits),
            FLD_BMSK(15 : 0));
    }

    return bFlag;
}

static void _KHal_HDMIRx_Sw_Rst_dtopdec(MS_U8 enInputPortType, EN_HDMI_SWRST_DTOPDEC enRstBit)
{
    // dtopdec
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);

    MHAL_HDMIRX_MSG_ERROR("_KHal_HDMIRx_Sw_Rst_dtopdec,%x,%x\n", enRstBit);

    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs,1, FLD_BIT((MS_U32)enRstBit));
    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs,0, FLD_BIT((MS_U32)enRstBit));
}

static void _KHal_HDMIRx_Sw_Rst_dtopdep(MS_U8 enInputPortType, EN_HDMI_SWRST_DTOPDEP enRstBit)
{
    // dtopdep
    UNUSED(enInputPortType);
    MS_U32 u32_dtop_dep_bkofs = _KHal_HDMIRx_GetDTOPDEPBankOffset(enInputPortType);

    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs,1, FLD_BIT((MS_U32)enRstBit));
    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs,0, FLD_BIT((MS_U32)enRstBit));
}

static void _KHal_HDMIRx_Sw_Rst_dtopmisc(MS_U8 enInputPortType,EN_HDMI_SWRST_DTOPMISC enRstBit)
{
    // dtopmisc
    UNUSED(enInputPortType);

    vIO32WriteFld_1(REG_0000_HDMIRX_DTOP_MISC_0 ,1, FLD_BIT((MS_U32)enRstBit));
    vIO32WriteFld_1(REG_0000_HDMIRX_DTOP_MISC_0 ,0, FLD_BIT((MS_U32)enRstBit));
}

static void _KHal_HDMIRx_Sw_Rst_innermisc( EN_HDMI_SWRST_INNERMISC enRstBit)
{
    vIO32WriteFld_1(REG_0000_HDMIRX_INNER_MISC_0 ,1, FLD_BIT((MS_U32)enRstBit));
    vIO32WriteFld_1(REG_0000_HDMIRX_INNER_MISC_0 ,0, FLD_BIT((MS_U32)enRstBit));
}

static void _KHal_HDMIRx_Sw_Rst_rcvtop(MS_U8 enInputPortType, EN_HDMI_SWRST_RCVTOP enRstBit)
{
    UNUSED(enInputPortType);
    MS_U32 u32_rcvtop_bkofs = _KHal_HDMIRx_GetDSCDSERVICEPBankOffset(enInputPortType);

    vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES+u32_rcvtop_bkofs,1, FLD_BIT((MS_U32)enRstBit));
    vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES+u32_rcvtop_bkofs,0, FLD_BIT((MS_U32)enRstBit));
}

static void _KHal_HDMIRx_MAC_Clkgen0_Init(MS_U8 enInputPortType)
{
    MS_U32 u32Portofs = _KHal_HDMIRx_GetCLKGENDRV0BankOffset(enInputPortType);
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    //P0/P1/P2 are at the same bk, and the offest is 0x20

    //[12]:reg_pi_clk_pix_rep_1p_div_en
    //vIO32WriteFld_1(REG_0008_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0008_HDMIRX_CKGEN_DRIVER_0_REG_P0_CLK_PIX_REP_1P_DIV_EN);
    vIO32WriteFld_1(REG_0008_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0008_HDMIRX_CKGEN_DRIVER_0_REG_P0_CLK_PIX_REP_1P_DIV_EN);//no need in 9U6


    //[15]: reg_en_clk_pix_rcv_3rd_ove
    vIO32WriteFld_1(REG_0018_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0018_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_PIX_RCV_3RD_OVE);

    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_PIX_XC_3RD_OVE);


    //[9] reg_en_clk_timing_det_dep_3rd_ove
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_TIMING_DET_DEC_2ND_OVE);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_TIMING_DET_DEC_3RD_OVE);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_001C_HDMIRX_CKGEN_DRIVER_0_REG_P0_EN_CLK_TIMING_DET_DEP_3RD_OVE);


    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, Fld(8,0,AC_FULLB0));

    //[12] reg_ckg_pi_clk_det_dec_misc_0_0_drv_ove
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0020_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEC_MISC_0_0_DRV_OVE);



    //[4] reg_ckh_pi_clk_det_dec_misc_0_drv_ove
    vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0024_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEC_MISC_0_DRV_OVE);
    vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0024_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEC_MISC_1_0_DRV_OVE);

    //[4] reg_ckg_pi_clk_det_dec_misc_1_drv_ove
    vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0028_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEC_MISC_1_DRV_OVE);
    vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0028_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEP_MISC_0_0_DRV_OVE);

    //[4] reg_ckg_pi_clk_det_dep_misc_0_drv_ove
    vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_002C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEP_MISC_0_DRV_OVE);
    vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_002C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEP_MISC_1_0_DRV_OVE);

    //[4] reg_ckg_pi_clk_det_dep_misc_1_drv_ove
    //[12]reg_ckg_pi_link_drv_ove
    vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0030_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_CLK_DET_DEP_MISC_1_DRV_OVE);
    // pi_clk_link_p_ck
    vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x4, REG_0030_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_LINK_DRV_OV);//mdify for H21
    vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0030_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_LINK_DRV_OVE);

    //[4]reg_ckg_pi_pix_1p_drv_ove
    //[13]reg_ckg_pi_pix_8p_drv_ove
    // pi_clk_pix_p_ck(mux_sel =1)
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x04, REG_0034_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_1P_DRV_OV);
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0034_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_1P_DRV_OVE);
    // pi_clk_link_p_ck(mux_sel =2)
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x08, REG_0034_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_8P_DRV_OV);
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0034_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_8P_DRV_OVE);

    //[5] reg_ckg_pi_pix_dc_mux_drv_ove
    //[12:8] reg_ckg_pi_pix_rcv_mux_drv_ov
    //[13] reg_ckg_pi_pix_rcv_mux_drv_ove
    vIO32WriteFld_1(REG_0038_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0038_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_DC_MUX_DRV_OVE);
    vIO32WriteFld_1(REG_0038_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0038_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_RCV_MUX_DRV_OVE);

    //[4] reg_ckg_pi_pix_rep_1p_drv_ove
    //[12] reg_ckg_pi_pix_tmdspll_drv_ove
    // pi_clk_pix_p_ck(mux_sel =1)
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x04, REG_003C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_REP_1P_DRV_OV);
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_003C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_REP_1P_DRV_OVE);
    // pi_clk_pix_tmdspll_p_ck(mux_sel =1)
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x04, REG_003C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_TMDSPLL_DRV_OV);
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_003C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_TMDSPLL_DRV_OVE);

    //[12] reg_ckg_pi_pix_xc_mux_drv_ove
    // pi_clk_pix_tmdspll_p_ck(mux_sel =1)
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_P0_CLK_LINK_DSCD_P_MUX_DIV);
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_P0_CLK_TMDS_DSCD_P_MUX_DIV);

    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_XC_MUX_DRV_OV);
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_XC_MUX_DRV_OVE);

    //[4:0] reg_ckg_pi_timing_det_dec_drv_ov
    //[5]   reg_ckg_pi_timing_det_dec_drv_ove
    //[12:8] reg_ckg_pi_timing_det_dep_drv_ov
    //[13]   reg_ckg_pi_timing_det_dep_drv_ove
    // pi_clk_link_p_ck(mux_sel=3)
    vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0xc, REG_0044_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DEC_DRV_OV);
    vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0044_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DEC_DRV_OVE);
    // pi_clk_link_p_ck(mux_sel=3)
    vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x10, REG_0044_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DEP_DRV_OV);
    vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0044_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DEP_DRV_OVE);

    //[4] reg_ckg_pi_timing_det_dtop_drv_ove
    //[12] reg_ckg_pi_tmds_frl_8p_drv_ove
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DTOP_DRV_OVE);
    // pi_clk_link_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_FRL_8P_DRV_OV);
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_FRL_8P_DRV_OVE);

    //[4] reg_ckg_pi_tmds_hdcp_1p_drv_ove
    //[12]reg_ckg_pi_tmds_hdcp_4p_drv_ove
    // pi_clk_tmds_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_004C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_1P_DRV_OV);
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_004C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_1P_DRV_OVE);
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_004C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_4P_DRV_OVE);

    //[4] reg_ckg_pi_tmds_hdcp_8p_drv_ove
    //[12]reg_ckg_pi_tmds_hdcp_mux_drv_ove
    // pi_clk_link_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0050_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_8P_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0050_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_8P_DRV_OVE);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0050_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_MUX_DRV_OVE);

    //[4] reg_ckg_pi_tmds_hdmi_1p_drv_ove
    //[12]reg_ckg_pi_tmds_hdmi_8p_drv_ove
    // pi_clk_tmds_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_1P_DRV_OV);
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_1P_DRV_OVE);
    // pi_clk_link_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_8P_DRV_OV);
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_8P_DRV_OVE);

    //[4] reg_ckg_pi_tmds_hdmi_mux_drv_ove
    // pi_clk_link_p_ck(mux_sel=2)
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_DRV_OV);
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_DRV_OVE);
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDSPLL_TEST_DIG_DRV_OV);
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDSPLL_TEST_DIG_DRV_OVE);

    //===== extra this time =======
    //[12] reg_ckg_pi_pix_xc_mux_drv_ove
    // pi_clk_pix_tmdspll_p_ck(mux_sel =1)
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_P0_CLK_LINK_DSCD_P_MUX_DIV);
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_P0_CLK_TMDS_DSCD_P_MUX_DIV);

    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 4, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_XC_MUX_DRV_OV);
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0040_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_PIX_XC_MUX_DRV_OVE);


    //[4] reg_ckg_pi_timing_det_dtop_drv_ove
    //[12] reg_ckg_pi_tmds_frl_8p_drv_ove
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x0c, REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DTOP_DRV_OV);//for 9U6
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TIMING_DET_DTOP_DRV_OVE);
    // pi_clk_link_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 4, REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_FRL_8P_DRV_OV);
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0048_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_FRL_8P_DRV_OVE);

    //[4] reg_ckg_pi_tmds_hdcp_1p_drv_ove
    //[12]reg_ckg_pi_tmds_hdcp_4p_drv_ove
    // pi_clk_tmds_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 4, REG_004C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_1P_DRV_OV);
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_004C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_1P_DRV_OVE);
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_004C_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_4P_DRV_OVE);

    //[4] reg_ckg_pi_tmds_hdcp_8p_drv_ove
    //[12]reg_ckg_pi_tmds_hdcp_mux_drv_ove
    // pi_clk_link_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 4, REG_0050_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_8P_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0050_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_8P_DRV_OVE);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0050_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDCP_MUX_DRV_OVE);

    //[4] reg_ckg_pi_tmds_hdmi_1p_drv_ove
    //[12]reg_ckg_pi_tmds_hdmi_8p_drv_ove
    // pi_clk_tmds_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 4, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_1P_DRV_OV);
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_1P_DRV_OVE);
    // pi_clk_link_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 4, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_8P_DRV_OV);
    vIO32WriteFld_1(REG_0054_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0054_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_8P_DRV_OVE);


    //[4] reg_ckg_pi_tmds_hdmi_mux_drv_ove
    // pi_clk_link_p_ck(mux_sel=2)
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 8, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_DRV_OV);
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 1, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDS_HDMI_MUX_DRV_OVE);
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDSPLL_TEST_DIG_DRV_OV);
    vIO32WriteFld_1(REG_0058_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0, REG_0058_HDMIRX_CKGEN_DRIVER_0_REG_CKG_P0_TMDSPLL_TEST_DIG_DRV_OVE);

    //=============================
    // sw enable
    vIO32WriteFld_1(REG_005C_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0xffff, FLD_BMSK(15 : 0));

    vIO32WriteFld_1(REG_0060_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0xfffc, FLD_BMSK(15 : 0));
    // delay?
    vIO32WriteFld_1(REG_0060_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0xffff, FLD_BMSK(15 : 0));

    vIO32WriteFld_1(REG_0064_HDMIRX_CKGEN_DRIVER_0 + u32Portofs, 0x3fff, FLD_BMSK(15 : 0));
}

static void _KHal_HDMIRx_MAC_Clkgen1_init(MS_U8 enInputPortType)
{
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    //[4] reg_ckg_128fs_fb_s0_drv_ove
    //[12]reg_ckg_128fs_fb_s1_drv_ove
    // clk_256fs_fb_s0_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0000_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0000_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S0_DRV_OV);
    vIO32WriteFld_1(REG_0000_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0000_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S0_DRV_OVE);
    vIO32WriteFld_1(REG_0000_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0000_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S1_DRV_OV);
    vIO32WriteFld_1(REG_0000_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0000_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S1_DRV_OVE);

    //[3:0] reg_ckg_128fs_fb_s0_ff_drv_ov
    //[4]   reg_ckg_128fs_fb_s0_ff_drv_ove
    //[11:8]reg_ckg_128fs_fb_s1_ff_drv_ov
    //[12]  reg_ckg_128fs_fb_s1_ff_drv_ove
    // clk_128fs_fb_s0_ff_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0004_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0004_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S0_FF_DRV_OV);
    vIO32WriteFld_1(REG_0004_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0004_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S0_FF_DRV_OVE);
    vIO32WriteFld_1(REG_0004_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0004_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S1_FF_DRV_OV);
    vIO32WriteFld_1(REG_0004_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0004_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_FB_S1_FF_DRV_OVE);

    //[3:0] reg_ckg_128fs_s0_drv_ov
    //[4]   reg_ckg_128fs_s0_drv_ove
    //[11:8]reg_ckg_128fs_s1_drv_ov
    //[12]  reg_ckg_128fs_s1_drv_ove
    vIO32WriteFld_1(REG_0008_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0008_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_S0_DRV_OV);
    vIO32WriteFld_1(REG_0008_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0008_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_S0_DRV_OVE);
    vIO32WriteFld_1(REG_0008_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0008_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_S1_DRV_OV);
    vIO32WriteFld_1(REG_0008_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0008_HDMIRX_CKGEN_DRIVER_1_REG_CKG_128FS_S1_DRV_OVE);

    //[3:0] reg_ckg_216_drv_ov
    //[4]   reg_ckg_216_drv_ove
    //[11]  reg_ckg_256fs_s0_drv_ove
    //[15]  reg_ckg_256fs_s1_drv_ove
    vIO32WriteFld_1(REG_000C_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_000C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_216_DRV_OV);
    vIO32WriteFld_1(REG_000C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_000C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_216_DRV_OVE);
    vIO32WriteFld_1(REG_000C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_000C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_256FS_S0_DRV_OVE);
    vIO32WriteFld_1(REG_000C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_000C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_256FS_S1_DRV_OVE);

    //[3] reg_ckg_432_drv_ove
    //[7] reg_ckg_864_drv_ove
    //[13]reg_ckg_clk_det_inner_misc_0_0_drv_ove
    //clk_mpll_432_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_432_DRV_OV);
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_432_DRV_OVE);
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_864_DRV_OV);
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_864_DRV_OVE);

    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CLK_DET_INNER_MISC_0_0_DRV_OVE);

    //[5] reg_ckg_clk_det_inner_misc0_drv_ove
    //[13] reg_ckg_clk_det_inner_misc1_drv_ove
    vIO32WriteFld_1(REG_0014_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0014_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CLK_DET_INNER_MISC_0_DRV_OVE);
    vIO32WriteFld_1(REG_0014_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0014_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CLK_DET_INNER_MISC_1_0_DRV_OVE);

    //[4] reg_ckg_clk_det_inner_misc_1_drv_ove
    //p0_clk_tmds_p_ck(mux_sel=0)
    vIO32WriteFld_1(REG_0018_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0018_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CLK_DET_INNER_MISC_1_DRV_OVE);
    vIO32WriteFld_1(REG_0018_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0018_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S0_0_DRV_OV);
    vIO32WriteFld_1(REG_0018_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0018_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S0_0_DRV_OVE);

    //[4] reg_ckg_cts_synth_s0_drv_ove
    //[11:10] reg_ckg_cts_synth_s1_0[3:2]
    //[13] reg_ckg_cts_synth_s1_0_drv_ove
    // clk_cts_synth_s0_0_int_ck (mux_sel=1)
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S0_DRV_OV);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S0_DRV_OVE);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_0_DRV_OV);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_0_DRV_OVE);

    //[4]    reg_ckg_cts_synth_s1_drv_ove
    //[11:8] reg_ckg_dsc_engine_drv_ov
    //[13]   reg_ckg_dsc_engine_drv_ove
    // clk_cts_synth_s1_0_int_ck (mux_sel=1)
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_DRV_OV);
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_DRV_OVE);
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DSC_ENGINE_DRV_OV);
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DSC_ENGINE_DRV_OVE);

    //[4] reg_ckg_dvi2miu_rd_s0_drv_ove
    //[12] reg_ckg_dvi2miu_rd_s1_drv_ove
    // rd_s0/1 : clk_dvi2miu_rd_clkgen_a_s0_p_ck (mux_sel=1)
    //vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S0_DRV_OV);
    //vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S0_DRV_OVE);
    //vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S1_DRV_OV);
    //vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S1_DRV_OVE);

    //[5] reg_ckg_dvi2miu_wr_s0_0_drv_ove
    //[12]reg_ckg_dvi2miu_wr_s0_1_drv_ove
    //vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0028_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_0_DRV_OVE);
    //vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0028_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_1_DRV_OVE);

    //[4] reg_ckg_dvi2miu_wr_s0_drv_ove
    //[13] reg_ckg_dvi2miu_wr_s1_0_drv_ove
    // wr_s0 : clk_dvi2miu_wr_s0_0_int_ck (mux_sel=1)
    //vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_DRV_OV);
    //vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_DRV_OVE);
    //vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_0_DRV_OVE);

    //[4] reg_ckg_dvi2miu_wr_s1_1_drv_ove
    //[12]reg_ckg_dvi2miu_wr_s1_drv_ove
    // wr_s1 : clk_dvi2miu_wr_s1_0_int_ck (mux_sel=1)
    //vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0030_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_1_DRV_OVE);
    //vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0030_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_DRV_OV);

    //vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0030_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_DRV_OVE);

    //[3]earc_audio_ove
    //[7]earc_cm
    //[10] SMI_HDMIRX
    //[14] MCU_BUS_HDMIRX
    // ??? how about smi inverse?
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0034_HDMIRX_CKGEN_DRIVER_1_REG_CKG_EARC_AUDIO_DRV_OVE);
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0034_HDMIRX_CKGEN_DRIVER_1_REG_CKG_EARC_CM_DRV_OVE);
    // just inverse?
    //vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_1, 0x2, REG_0034_HDMIRX_CKGEN_DRIVER_1_REG_CKG_SMI_HDMIRX_DRV_OV);
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0034_HDMIRX_CKGEN_DRIVER_1_REG_CKG_SMI_HDMIRX_DRV_OVE);
    vIO32WriteFld_1(REG_0034_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0034_HDMIRX_CKGEN_DRIVER_1_REG_CKG_MCU_BUS_HDMIRX_DRV_OVE);

    //[2:0] reg_ckg_mcu_hdmi_hdmirx_drv_ov 3h'4
    //[3]   reg_ckg_mcu_hdmi_hdmirx_drv_ove
    //[6]   reg_ckg_mcu_nonpm_hdmirx_drv_ove
    //[10]  reg_ckg_earc_debounce_drv_ove
    //[15]  reg_ckg_earc_dn_prbs_drv_ove
    // mcu_hdmi_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0038_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0038_HDMIRX_CKGEN_DRIVER_1_REG_CKG_MCU_HDMI_HDMIRX_DRV_OV);
    vIO32WriteFld_1(REG_0038_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0038_HDMIRX_CKGEN_DRIVER_1_REG_CKG_MCU_HDMI_HDMIRX_DRV_OVE);
    vIO32WriteFld_1(REG_0038_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0038_HDMIRX_CKGEN_DRIVER_1_REG_CKG_MCU_NONPM_HDMIRX_DRV_OVE);
    vIO32WriteFld_1(REG_0038_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0038_HDMIRX_CKGEN_DRIVER_1_REG_CKG_EARC_DM_PRBS_DRV_OVE);//V

    //[4:0] reg_ckg_link_dscd_drv_ov 5'h4
    //[5]   reg_ckg_link_dscd_drv_ove
    //[12:8] reg_ckg_pix_tmdspll_dscd_drv_ove 5'h4
    //[13]   reg_ckg_pix_tmdspll_dscd_drv_ove
    // p0_clk_link_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_003C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_LINK_DSCD_DRV_OV);
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_003C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_LINK_DSCD_DRV_OVE);
    // p0_clk_pix_tmdspll_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_003C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_PIX_TMDSPLL_DSCD_DRV_OV);
    vIO32WriteFld_1(REG_003C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_003C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_PIX_TMDSPLL_DSCD_DRV_OVE);

    //[5] reg_ckg_pix_xc_s0_drv_ove
    //[13]reg_ckg_pix_xc_s1_drv_ove
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0040_HDMIRX_CKGEN_DRIVER_1_REG_CKG_PIX_XC_S0_DRV_OVE);
    vIO32WriteFld_1(REG_0040_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0040_HDMIRX_CKGEN_DRIVER_1_REG_CKG_PIX_XC_S1_DRV_OVE);

    //[2] reg_ckg_r2_hdmi_drv_ove
    //[13]reg_ckg_timing_det_dtop_drv_ove
    vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0044_HDMIRX_CKGEN_DRIVER_1_REG_CKG_R2_HDMI_DRV_OVE);
    vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_1,0x4,REG_0044_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TIMING_DET_DTOP_DRV_OV);//new added for 9U6[INIT]:h0[MSB]:12.0[LSB]:8.0
    vIO32WriteFld_1(REG_0044_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0044_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TIMING_DET_DTOP_DRV_OVE);

    //[5] reg_ckg_tmds_audio_s0_0_drv_0
    //[12]reg_ckg_tmds_audio_s0_drv_ove
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0048_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S0_0_DRV_OVE);
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0048_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S0_DRV_OV);
    vIO32WriteFld_1(REG_0048_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0048_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S0_DRV_OVE);

    //[5] reg_ckg_tmds_audio_s1_0_drv_0
    //[12]reg_ckg_tmds_audio_s1_drv_ove
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_004C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S1_0_DRV_OVE);
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_004C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S1_DRV_OV);
    vIO32WriteFld_1(REG_004C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_004C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_AUDIO_S1_DRV_OVE);

    //[4:0] reg_ckg_tmds_dscd_drv_ov
    //[5]   reg_ckg_tmds_dscd_drv_ove
    //[10]  reg_ckg_xtal_24_drv_ove
    //[15]  reg_ckg_xtal_hdmi_12_drv_ove
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_DSCD_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_DSCD_DRV_OVE);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_24_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_24_DRV_OVE);
    // xtal_12m_ck (mux_sel=1)
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_HDMI_12_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_HDMI_12_DRV_OVE);//V

    vIO32WriteFld_1(REG_007C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_007C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_128FS_FB_S02MAC_OV);
    vIO32WriteFld_1(REG_007C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_007C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_128FS_FB_S02MAC_OVE);
    vIO32WriteFld_1(REG_007C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_007C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_128FS_FB_S12MAC_OV);
    vIO32WriteFld_1(REG_007C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_007C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_128FS_FB_S12MAC_OVE);

    vIO32WriteFld_1(REG_0080_HDMIRX_CKGEN_DRIVER_1, 0xffff, FLD_BMSK(15 : 0));
    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0xffff, FLD_BMSK(15 : 0));

    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_MPLL_8642PHY_OV);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_MPLL_8642PHY_OVE);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_PIX_TMDSPLL_DSCD2MAC_OV);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_PIX_TMDSPLL_DSCD2MAC_OVE);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_PIX_XC_S02MAC_OV);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_PIX_XC_S02MAC_OVE);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_PIX_XC_S12MAC_OV);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_PIX_XC_S12MAC_OVE);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1,0x1,REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_R2_HDMI2MAC_OV);//new added for 9U6 [INIT]:h0[MSB]:8.0[LSB]:8.0
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_R2_HDMI2MAC_OVE);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TIMING_DET_DTOP2MAC_OV);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TIMING_DET_DTOP2MAC_OVE);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TMDS_AUDIO_S02MAC_OV);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TMDS_AUDIO_S02MAC_OVE);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TMDS_AUDIO_S12MAC_OV);
    vIO32WriteFld_1(REG_0088_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0088_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TMDS_AUDIO_S12MAC_OVE);

    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TMDS_DSCD2MAC_OV);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_TMDS_DSCD2MAC_OVE);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_XTAL_242HDMIRX_MBIST_OV);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_XTAL_242HDMIRX_MBIST_OVE);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_XTAL_242MAC_OV);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_XTAL_242MAC_OVE);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1,0x1,REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_XTAL_HDMI_122MAC_OV);//new added for 9U6 [INIT]:h0[MSB]:6.0[LSB]:6.0
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_XTAL_HDMI_122MAC_OVE);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_AUDIO2EARC_OV);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_AUDIO2EARC_OVE);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_CM2EARC_OV);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_CM2EARC_OVE);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_DEBOUNCE2EARC_OV);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_DEBOUNCE2EARC_OVE);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_DM_PRBS2EARC_OV);
    vIO32WriteFld_1(REG_008C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_008C_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_EARC_DM_PRBS2EARC_OVE);

    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_MCU_BUS2MAC_OV);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_MCU_BUS2MAC_OVE);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_MCU_HDMI2MAC_OV);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_MCU_HDMI2MAC_OVE);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_MCU_NONPM2MAC_OV);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_MCU_NONPM2MAC_OVE);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_RIU_NONPM2RIU_DFT_HDMIRX_OV);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_RIU_NONPM2RIU_DFT_HDMIRX_OVE);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_SMI2MAC_OV);
    vIO32WriteFld_1(REG_0090_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0090_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_SMI2MAC_OVE);


    //[3] reg_ckg_432_drv_ove
    //[7] reg_ckg_864_drv_ove
    //[13]reg_ckg_clk_det_inner_misc_0_0_drv_ove
    //clk_mpll_432_p_ck(mux_sel=1)
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x4,  REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_432_DRV_OV);
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_432_DRV_OVE);
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_864_DRV_OV);
    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_864_DRV_OVE);

    vIO32WriteFld_1(REG_0010_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0010_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CLK_DET_INNER_MISC_0_0_DRV_OVE);


    //[4] reg_ckg_cts_synth_s0_drv_ove
    //[11:10] reg_ckg_cts_synth_s1_0[3:2]
    //[13] reg_ckg_cts_synth_s1_0_drv_ove
    // clk_cts_synth_s0_0_int_ck (mux_sel=1)
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S0_DRV_OV);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S0_DRV_OVE);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_0_DRV_OV);
    vIO32WriteFld_1(REG_001C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_001C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_0_DRV_OVE);


    //[4]    reg_ckg_cts_synth_s1_drv_ove
    //[11:8] reg_ckg_dsc_engine_drv_ov
    //[13]   reg_ckg_dsc_engine_drv_ove
    // clk_cts_synth_s1_0_int_ck (mux_sel=1)
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_DRV_OV);
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_CTS_SYNTH_S1_DRV_OVE);
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DSC_ENGINE_DRV_OV);
    vIO32WriteFld_1(REG_0020_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0020_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DSC_ENGINE_DRV_OVE);



    //[4:0] reg_ckg_tmds_dscd_drv_ov
    //[5]   reg_ckg_tmds_dscd_drv_ove
    //[10]  reg_ckg_xtal_24_drv_ove
    //[15]  reg_ckg_xtal_hdmi_12_drv_ove
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_DSCD_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_TMDS_DSCD_DRV_OVE);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_24_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_24_DRV_OVE);
    // xtal_12m_ck (mux_sel=1)
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_HDMI_12_DRV_OV);
    vIO32WriteFld_1(REG_0050_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0050_HDMIRX_CKGEN_DRIVER_1_REG_CKG_XTAL_HDMI_12_DRV_OVE);

#if 0//DVI2MIU
    vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S0_DRV_OV);
    vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S0_DRV_OVE);
    vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S1_DRV_OV);
    vIO32WriteFld_1(REG_0024_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0024_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S1_DRV_OVE);
    vIO32WriteFld_1(REG_00B4_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_00B4_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S2_DRV_OV);
    vIO32WriteFld_1(REG_00B4_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00B4_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_RD_S2_DRV_OVE);

    vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0028_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_0_DRV_OV);
    vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0028_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_0_DRV_OVE);
    vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_0_DRV_OV);
    vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_0_DRV_OVE);
    vIO32WriteFld_1(REG_00B4_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_00B4_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S2_0_DRV_OV);
    vIO32WriteFld_1(REG_00B4_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00B4_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S2_0_DRV_OVE);

    vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0028_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_1_DRV_OV);
    vIO32WriteFld_1(REG_0028_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0028_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_1_DRV_OVE);
    vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0030_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_1_DRV_OV);
    vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0030_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_1_DRV_OVE);
    vIO32WriteFld_1(REG_00B8_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_00B8_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S2_1_DRV_OV);
    vIO32WriteFld_1(REG_00B8_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00B8_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S2_1_DRV_OVE);

    vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_DRV_OV);
    vIO32WriteFld_1(REG_002C_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_002C_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S0_DRV_OVE);
    vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_0030_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_DRV_OV);
    vIO32WriteFld_1(REG_0030_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0030_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S1_DRV_OVE);
    vIO32WriteFld_1(REG_00B8_HDMIRX_CKGEN_DRIVER_1, 0x4, REG_00B8_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S2_DRV_OV);
    vIO32WriteFld_1(REG_00B8_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00B8_HDMIRX_CKGEN_DRIVER_1_REG_CKG_DVI2MIU_WR_S2_DRV_OVE);

    vIO32WriteFld_1(REG_0078_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0078_HDMIRX_CKGEN_DRIVER_1_REG_EN_CLK_DVI2MIU_WR_S0_1ST_OVE);
    vIO32WriteFld_1(REG_0078_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0078_HDMIRX_CKGEN_DRIVER_1_REG_EN_CLK_DVI2MIU_WR_S0_2ND_OVE);
    vIO32WriteFld_1(REG_0078_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0078_HDMIRX_CKGEN_DRIVER_1_REG_EN_CLK_DVI2MIU_WR_S1_1ST_OVE);
    vIO32WriteFld_1(REG_0078_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0078_HDMIRX_CKGEN_DRIVER_1_REG_EN_CLK_DVI2MIU_WR_S1_2ND_OVE);
    vIO32WriteFld_1(REG_00B8_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_00B8_HDMIRX_CKGEN_DRIVER_1_REG_EN_CLK_DVI2MIU_WR_S2_1ST_OVE);
    vIO32WriteFld_1(REG_00B8_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_00B8_HDMIRX_CKGEN_DRIVER_1_REG_EN_CLK_DVI2MIU_WR_S2_2ND_OVE);

    vIO32WriteFld_1(REG_005C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_005C_HDMIRX_CKGEN_DRIVER_1_REG_CLK_DVI2MIU_RD_S0_DIV);
    vIO32WriteFld_1(REG_005C_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_005C_HDMIRX_CKGEN_DRIVER_1_REG_CLK_DVI2MIU_RD_S1_DIV);
    vIO32WriteFld_1(REG_00BC_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_00BC_HDMIRX_CKGEN_DRIVER_1_REG_CLK_DVI2MIU_RD_S2_DIV);

    vIO32WriteFld_1(REG_0060_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0060_HDMIRX_CKGEN_DRIVER_1_REG_CLK_DVI2MIU_WR_S0_DIV);
    vIO32WriteFld_1(REG_0060_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_0060_HDMIRX_CKGEN_DRIVER_1_REG_CLK_DVI2MIU_WR_S1_DIV);
    vIO32WriteFld_1(REG_00BC_HDMIRX_CKGEN_DRIVER_1, 0x0, REG_00BC_HDMIRX_CKGEN_DRIVER_1_REG_CLK_DVI2MIU_WR_S2_DIV);

    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_RD_S02MAC_OV);
    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_RD_S02MAC_OVE);
    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_RD_S12MAC_OV);
    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_RD_S12MAC_OVE);
    vIO32WriteFld_1(REG_00BC_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00BC_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_RD_S22MAC_OV);
    vIO32WriteFld_1(REG_00BC_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00BC_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_RD_S22MAC_OVE);

    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_WR_S02MAC_OV);
    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_WR_S02MAC_OVE);
    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_WR_S12MAC_OV);
    vIO32WriteFld_1(REG_0084_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_0084_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_WR_S12MAC_OVE);
    vIO32WriteFld_1(REG_00BC_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00BC_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_WR_S22MAC_OV);
    vIO32WriteFld_1(REG_00BC_HDMIRX_CKGEN_DRIVER_1, 0x1, REG_00BC_HDMIRX_CKGEN_DRIVER_1_REG_SW_EN_CLK_DVI2MIU_WR_S22MAC_OVE);
#endif
}

static void _KHal_HDMIRx_MAC_sub_Init(MS_U8 enInputPortType)
{
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    // scdc, dtop, dtop_dec, dtop_dep, dscd_service, frl_top,frl_decoder,

    // MS_U32 u32_dtop_bkofs =_KHal_HDMIRx_GetDTOPBankOffset(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U32 u32_dtop_dep_bkofs = _KHal_HDMIRx_GetDTOPDEPBankOffset(enInputPortType);
    MS_U32 u32_scdc_bkofs = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortType);
    MS_U32 u32_dscd_serv_bkofs = _KHal_HDMIRx_GetDSCDSERVICEPBankOffset(enInputPortType);

    MS_U32 u32_dtop_bkofs = _KHal_HDMIRx_GetDTOPBankOffset(enInputPortType);

    MS_U32 u32_pkt_dec_bkofs = _KHal_HDMIRx_GetPKTBankOffset(enInputPortType);


    MS_U32 u32_i = 0;
    MS_U32 u32_bk_ofs = 0;

    // hotplug
    vIO32WriteFld_SCDC(REG_0030_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0030_SCDC_P0_REG_TX_5V_PWR_I);
    vIO32WriteFld_SCDC(REG_0030_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0030_SCDC_P0_REG_TX_5V_PWR_OEN);
    vIO32WriteFld_SCDC(REG_0030_SCDC_P0 + u32_scdc_bkofs, 1, REG_0030_SCDC_P0_REG_TX_5V_LOW_INT);

    // scdc
    //settime-out value to 0x0a
    vIO32WriteFld_SCDC(REG_0018_SCDC_P0 + u32_scdc_bkofs, 0x64, REG_0018_SCDC_P0_REG_READ_REQUEST_TIMEOUT);

    //enable status_update, ced_update, rr_test_update(mask)
    vIO32WriteFld_SCDC(REG_008C_SCDC_P0 + u32_scdc_bkofs, 0xfff8, REG_008C_SCDC_P0_REG_RR_TRIG_MASK);

    vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32_scdc_bkofs, 0, REG_0080_SCDC_P0_REG_RSC_CNT_OV);
    vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32_scdc_bkofs, 0, REG_0080_SCDC_P0_REG_DIS_FLT_UPDATE_TIMER);
    vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32_scdc_bkofs, 0, REG_0080_SCDC_P0_REG_DIS_FLT_UPDATE_DIFF);
    vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32_scdc_bkofs, 1, REG_0080_SCDC_P0_REG_RR_VLD_RANGE_EN);
    vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32_scdc_bkofs, 1, REG_0080_SCDC_P0_REG_RR_TRIG_SEL);

#ifdef SCDC_CLK_STRETCH_CHK
    // 0xa849
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0000_SCDC_P0_REG_DDC_SCDC_EN);
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0000_SCDC_P0_REG_DDC_FILTER_ON);
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0000_SCDC_P0_REG_FILTER_MSB);
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0000_SCDC_P0_REG_CHK_START);
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0000_SCDC_P0_REG_SLEW_SEL);
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0000_SCDC_P0_REG_MFSR_WRITABLE);
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0000_SCDC_P0_REG_DDC_BUSY);
    vIO32WriteFld_SCDC(REG_0000_SCDC_P0 + u32_scdc_bkofs, 0xa8, REG_0000_SCDC_P0_REG_SCDC_ADDRESS);

    // 0x78de
    vIO32WriteFld_SCDC(REG_0028_SCDC_P0 + u32_scdc_bkofs, 0xde, REG_0028_SCDC_P0_REG_CLK_STRETCH_ADDRESS);
    vIO32WriteFld_SCDC(REG_0028_SCDC_P0 + u32_scdc_bkofs, 0x78, REG_0028_SCDC_P0_REG_CLK_STRETCH_DATA);

    vIO32WriteFld_SCDC(REG_002C_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_002C_SCDC_P0_REG_CLK_STRETCH_EN);
    vIO32WriteFld_SCDC(REG_002C_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_002C_SCDC_P0_REG_STRETCH_AUTO_RELEASE);
    vIO32WriteFld_SCDC(REG_002C_SCDC_P0 + u32_scdc_bkofs, 0x4, REG_002C_SCDC_P0_REG_CLK_STRETCH_CNT);

    // 0x24de = 0010.0100.1101.1110
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0xde, REG_0008_SCDC_P0_REG_CPU_ADR_REG);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0008_SCDC_P0_REG_LOAD_ADR_P);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0008_SCDC_P0_REG_ENWRITE_SCDC);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0008_SCDC_P0_REG_SCDC_SRAM_ACCESS);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0008_SCDC_P0_REG_EN_SRAM_RD);

    mdelay(1);

    // 0x0007
    vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0014_SCDC_P0_REG_SCRAMBLING_STATUS_OV);
    vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0014_SCDC_P0_REG_SCRAMBLING_ONLY);
    vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0014_SCDC_P0_REG_SCRAMBLING_OV_VALUE);
    vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0014_SCDC_P0_REG_SCRAMBLING_OV_EN);
    vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0014_SCDC_P0_REG_TMDS_CLK_RATIO_OV_VALUE);
    vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0014_SCDC_P0_REG_TMDS_CLK_RATIO_OV_EN);

    // 0x01de
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32_scdc_bkofs, 0xde, REG_000C_SCDC_P0_REG_CPU_WDATA_REG);
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_000C_SCDC_P0_REG_SCDC_DATA_WR_P);
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_000C_SCDC_P0_REG_CPU_WR_BUSY_SCDC);

    mdelay(1);

    // 0x24df
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0xdf, REG_0008_SCDC_P0_REG_CPU_ADR_REG);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0008_SCDC_P0_REG_LOAD_ADR_P);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_0008_SCDC_P0_REG_ENWRITE_SCDC);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0008_SCDC_P0_REG_SCDC_SRAM_ACCESS);
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_0008_SCDC_P0_REG_EN_SRAM_RD);

    mdelay(1);

    // 0x01df
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32_scdc_bkofs, 0xdf, REG_000C_SCDC_P0_REG_CPU_WDATA_REG);
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32_scdc_bkofs, 0x1, REG_000C_SCDC_P0_REG_SCDC_DATA_WR_P);
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32_scdc_bkofs, 0x0, REG_000C_SCDC_P0_REG_CPU_WR_BUSY_SCDC);
#endif /*SCDC_CLK_STRETCH_CHK*/

    // dtop_dec & dtop_dep
    // dtop_dec, dtop_dep is very similar, we group same thing and separate different...
    // to prevent from some miss.
    u32_bk_ofs = REG_0000_P0_HDMIRX_DTOP_DEP_MISC - REG_0000_P0_HDMIRX_DTOP_DEC_MISC;
    for (u32_i = 0; u32_i < 2; u32_i++)
    {
        // hde/htt/vde stable cnt enable
        vIO32WriteFld_1(REG_013C_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0x1, REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HDE_STABLE_EN);
        vIO32WriteFld_1(REG_013C_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0x1, REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HTT_STABLE_EN);
        vIO32WriteFld_1(REG_013C_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0x1, REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_VDE_STABLE_EN);

        // hde_cnt, htt_cnt
        vIO32WriteFld_1(REG_0134_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0xf, REG_0134_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HDE_STABLE_CNT);
        vIO32WriteFld_1(REG_0134_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0xf, REG_0134_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HTT_STABLE_CNT);

        // vde_cnt
        vIO32WriteFld_1(REG_0138_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0x2, REG_0138_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_VDE_STABLE_CNT);

        //[06:04]: reg_hdmirx_dec_src_det_pix_per_cycle
        //[12]: reg_hdmirx_dec_i_md_det_cont_en (vtt +-1)
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0x7, REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_SRC_DET_PIX_PER_CYCLE);
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0x2, REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_I_MD_DET_CONDI_EN);

        //[15:0]: reg_hdmirx_dtop_dec_misc_clk_det_en
        vIO32WriteFld_1(REG_0004_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs + u32_i * u32_bk_ofs, 0xffff, REG_0004_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DTOP_DEC_MISC_CLK_DET_EN);
    }

    // dtop_dec
    // [4]: reg_en_deep_color
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x1, REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_EN_DEEP_COLOR);

    // dtop_dec_28[7:0]: reg_de_det_cnt_tol (tolerance for hdcp out hde)
    vIO32WriteFld_1(REG_00A0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x2, REG_00A0_P0_HDMIRX_DTOP_DEC_MISC_REG_DE_DET_CNT_TOL);

    // dtop_dec_28[15]: reg_de_det_timeout_timer_en
    vIO32WriteFld_1(REG_00A0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x0, REG_00A0_P0_HDMIRX_DTOP_DEC_MISC_REG_DE_DET_TIMEOUT_TIMER_EN);
    // hdmirx_dtop_68[14]: reg_dep_de_det_timeout_en
    vIO32WriteFld_1(REG_01A0_P0_HDMIRX_DTOP + u32_dtop_bkofs, 0x0, REG_01A0_P0_HDMIRX_DTOP_REG_DEP_DE_DET_TIMEOUT_EN);

    // dtop_dec_timing_det, dc_auto
    vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, HDMI_TIMING_DET_DTOPDEC_DC_AUTO, REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_SEL);

    // reg_8p_hdmi_mode_timeout_th, (0xf<<1)+1 = 31ms
    vIO32WriteFld_1(REG_00A8_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0xf, REG_00A8_P0_HDMIRX_DTOP_DEC_MISC_REG_8P_HDMI_MODE_TIMEOUT_TH);

    // enable reg_hdmirx2xc_enc_en_ov for xc mute function2 used
    //vIO32WriteFld_1(REG_00A8_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x1, REG_00A8_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX2XC_ENC_EN_OVE);
    //vIO32WriteFld_1(REG_00A8_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x1, REG_00A8_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX2XC_ENC_EN_OV);

    //
    vIO32WriteFld_1(REG_01C0_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 0xFFFF, REG_01C0_P0_HDMIRX_DTOP_DEP_MISC_REG_FASTMUTE_EN_CNT);
    vIO32WriteFld_1(REG_01C4_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 6, REG_01C4_P0_HDMIRX_DTOP_DEP_MISC_REG_FASTMUTE_MODE);
    //vIO32WriteFld_1(REG_01C4_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 0x1, REG_01C4_P0_HDMIRX_DTOP_DEP_MISC_REG_FASTMUTE_ON);

    // dtop_dep
    // [12]: reg_hdmirx_dep_cnt_frame_rate_en
    vIO32WriteFld_1(REG_00D4_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 0x1, REG_00D4_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_CNT_FRAME_RATE_EN);

    // [01:00]: reg_hdmirx_feed_rcv_i_md_sel
    vIO32WriteFld_1(REG_0090_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 0x2, REG_0090_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_FEED_RCV_I_MD_SEL);

    // dtop_dep_timing_det, auto
    vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, HDMI_TIMING_DET_DTOPDEP_AUTO, REG_0100_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_TIMING_DET_SEL);

    // crc enable
    vIO32WriteFld_1(REG_00D0_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 1, REG_00D0_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_CRC_EN);

    // dscd_service
    //[11]: reg_rcv_top_sram_adr_cmp_en
    vIO32WriteFld_1(REG_00B0_P0_DSCD_SERVICES + u32_dscd_serv_bkofs, 1, REG_00B0_P0_DSCD_SERVICES_REG_RCV_TOP_SRAM_ADR_CMP_EN);
    vIO32WriteFld_1(REG_00B0_P0_DSCD_SERVICES + u32_dscd_serv_bkofs, 0x2, REG_00B0_P0_DSCD_SERVICES_REG_RCV_TOP_SRAM_ADR_CMP_PRD);//new add for Od2

    vIO32WriteFld_1(REG_0004_P0_DSCD_SERVICES + u32_dscd_serv_bkofs, 1, REG_0004_P0_DSCD_SERVICES_REG_PATH8P_EN);

    vIO32WriteFld_1(REG_0028_P0_HDMIRX_PKT_DEC + u32_pkt_dec_bkofs, 0xffff, REG_0028_P0_HDMIRX_PKT_DEC_REG_DEPACK_DEC_IRQ_MASK_0);
}

static void _KHAL_HDMIRx_MISC_Init(MS_U8 enInputPortType)
{
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    UNUSED(enInputPortType);
    MS_U32 u32_plltop_bkofs = _KHal_HDMIRx_GetPLLTOPBankOffset(enInputPortType);
    MS_U32 u32_dtopdec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U32 u32_dtopdep_bkofs = _KHal_HDMIRx_GetDTOPDEPBankOffset(enInputPortType);
    MS_U32 u32EMPBankOffset = _KHal_HDMIRx_GetEMPBankOffset(enInputPortType);
    MS_U32 u32PortBankOffset =_KHal_HDMIRx_GetPortBankOffset(enInputPortType);

    //wriu  0x001738e0  0x3f // TBD

    //vIO32WriteFld_1(0x121060, 1, Fld(1,0,AC_MSKB0));//reg_dp_hdmi_sel Selecion for DP/HDMI mux

    vIO32WriteFld_1(REG_00A0_P0_HDMIRX_DTOP_DEP_MISC + u32_dtopdep_bkofs, 1, REG_00A0_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_VMUTE_EN);
    vIO32WriteFld_1(REG_00A4_P0_HDMIRX_DTOP_DEP_MISC + u32_dtopdep_bkofs, 0xD000, REG_00A4_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_AVMUTE_EN);
    vIO32WriteFld_1(REG_0014_P0_HDMI_EMP + u32EMPBankOffset, 0, REG_0014_P0_HDMI_EMP_REG_EMP2MIU_REP_EN);//need sync EMP2MIU_REP_Sel

    //-------------------------------------------------------------------------------
    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEP_MISC + u32_dtopdep_bkofs, 0x40, REG_0000_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DTOP_DEP_SW_RST);
    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEP_MISC + u32_dtopdep_bkofs, 0x00, REG_0000_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DTOP_DEP_SW_RST);


    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdep_bkofs, 0x40, REG_0000_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DTOP_DEC_SW_RST);
    vIO32WriteFld_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdep_bkofs, 0x00, REG_0000_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DTOP_DEC_SW_RST);

    //-------------------------------------------------------------------------------
    //wriu  0x002113dc  0xe1
    //wriu  0x002113dd  0x00
    vIO32WriteFld_1(REG_01B8_P0_PLL_TOP + u32_plltop_bkofs, 0xE1, REG_01B8_P0_PLL_TOP_REG_SW_FREQ_SETTING);

    //=====================================================================================

    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_0030_P0_PLL_TOP_REG_FPLL_MODE);
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_0030_P0_PLL_TOP_REG_FRAME_LPLL_EN);
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_0030_P0_PLL_TOP_REG_I_GAIN_PRD_ZERO);
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_0030_P0_PLL_TOP_REG_P_GAIN_PRD_ZERO);
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_0030_P0_PLL_TOP_REG_I_GAIN_PHASE_ZERO);
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_0030_P0_PLL_TOP_REG_P_GAIN_PHASE_ZERO);

    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x2,REG_0030_P0_PLL_TOP_REG_IVS_FRAME_DIV);
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x2,REG_0030_P0_PLL_TOP_REG_OVS_FRAME_DIV);


    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01A8_P0_PLL_TOP_REG_EN_OV_TMDSPLL_CP_SW);
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x1,REG_01A8_P0_PLL_TOP_REG_EN_TMDSPLL_PRDT);
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01A8_P0_PLL_TOP_REG_EN_TMDSPLL_PRDT2);
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01A8_P0_PLL_TOP_REG_EN_TMDSPLL_TEST);
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01A8_P0_PLL_TOP_REG_GCP_TMDSPLL_EXTAL);
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01A8_P0_PLL_TOP_REG_GCP_TMDSPLL_PORST);
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01A8_P0_PLL_TOP_REG_PD_TMDSPLL);
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x4,REG_01A8_P0_PLL_TOP_REG_GCR_TMDSPLL_LOOPDIV_SECOND);


    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x2,REG_01AC_P0_PLL_TOP_REG_GCR_TMDSPLL_OUTPUT_DIV);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_EN_CLKO_PIX);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_EN_CLKO_TMDS);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x1,REG_01AC_P0_PLL_TOP_REG_PD_AUPLL_KPDIV);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_GCR_AUPLL_KPDIV);


    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x2,REG_01AC_P0_PLL_TOP_REG_GCR_TMDSPLL_OUTPUT_DIV);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x1,REG_01AC_P0_PLL_TOP_REG_EN_CLKO_PIX);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_EN_CLKO_TMDS);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_PD_AUPLL_KPDIV);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_GCR_AUPLL_KPDIV);

    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x2,REG_01AC_P0_PLL_TOP_REG_GCR_TMDSPLL_OUTPUT_DIV);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x1,REG_01AC_P0_PLL_TOP_REG_EN_CLKO_PIX);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_EN_CLKO_TMDS);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_PD_AUPLL_KPDIV);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01AC_P0_PLL_TOP_REG_GCR_AUPLL_KPDIV);


    //wriu  0x002108d2  0x10   //TBD
    vIO32WriteFld_1(REG_01A4_P0_HDMIRX_DTOP + u32PortBankOffset,0x0,REG_01A4_P0_HDMIRX_DTOP_REG_DE_FILTER_EN);//new added for 9U6 [INIT]:h1[MSB]:0[LSB]:0
    //wriu  0x002108d3  0x05   //TBD


    //wriu  0x0000310c  0x0awriu  0x00003146  0xf8wriu  0x00003147  0xffwriu  0x00003141  0x30wriu  0x00003124  0x0awriu  0x00003125  0x00wriu  0x0000310b  0x0c



    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DC_FIFO_RST);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_AUTO_DC_FIFO_RST);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x1,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_DC_DE_UNSTABLE_RST_EN);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x1,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_DC_MODE_CHANGE_RST_EN);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x1,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_EN_DEEP_COLOR);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_EN_DF_ADJUST);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_MANUAL_AP_SEL);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_WP_AUTO_ADJUST);


    vIO32WriteFld_1(REG_0004_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0xFFFF,REG_0004_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DTOP_DEC_MISC_CLK_DET_EN);


    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x1,REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HDE_STABLE_EN);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x1,REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_HTT_STABLE_EN);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x1,REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_VDE_STABLE_EN);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_VTT_STABLE_EN);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_STABLE_OV);
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, 0x0,REG_013C_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_TIMING_DET_STABLE_OVE);
}

static void _KHal_HDMIRx_DTOPMISC_INNER_Init(MS_U8 enInputPortType)
{
    MS_U32 u32_dtopmisc_bkofs = _KHal_HDMIRx_GetDTOPMISCBankOffset(0);
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    //[07:00]: reg_xtal_freq
    //vIO32WriteFld_1(REG_0080_HDMIRX_INNER_MISC_0 + u32_innermisc_bkofs, 4, REG_0080_HDMIRX_INNER_MISC_0_REG_XTAL_FREQ);


    //for timing issue for port2    MT9701_TBD_ENABLE    when ASIC back , need check this setting disable or not
    vIO32WriteFld_1(REG_01C0_HDMIRX_INNER_MISC_0 , 0x1, REG_01C0_HDMIRX_INNER_MISC_0_PORT0_LATCH);
    vIO32WriteFld_1(REG_01C0_HDMIRX_INNER_MISC_0 , 0x1, REG_01C0_HDMIRX_INNER_MISC_0_PORT1_LATCH);

#if 0//DVI2MIU
     vIO32WriteFld_1(REG_00C0_HDMIRX_INNER_MISC_0 , 0, REG_00C0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_0_S0);
     vIO32WriteFld_1(REG_00C0_HDMIRX_INNER_MISC_0 , 0, REG_00C0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_1_S0);
     vIO32WriteFld_1(REG_00C4_HDMIRX_INNER_MISC_0 , 0, REG_00C4_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_2_S0);
     vIO32WriteFld_1(REG_00C4_HDMIRX_INNER_MISC_0 , 0, REG_00C4_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_3_S0);

     vIO32WriteFld_1(REG_00E0_HDMIRX_INNER_MISC_0 , 0, REG_00E0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_0_S1);
     vIO32WriteFld_1(REG_00E0_HDMIRX_INNER_MISC_0 , 0, REG_00E0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_1_S1);
     vIO32WriteFld_1(REG_00E4_HDMIRX_INNER_MISC_0 , 0, REG_00E4_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_2_S1);
     vIO32WriteFld_1(REG_00E4_HDMIRX_INNER_MISC_0 , 0, REG_00E4_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_3_S1);

     vIO32WriteFld_1(REG_0140_HDMIRX_INNER_MISC_0 , 0, REG_0140_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_0_S2);
     vIO32WriteFld_1(REG_0140_HDMIRX_INNER_MISC_0 , 0, REG_0140_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_1_S2);
     vIO32WriteFld_1(REG_0144_HDMIRX_INNER_MISC_0 , 0, REG_0144_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_2_S2);
     vIO32WriteFld_1(REG_0144_HDMIRX_INNER_MISC_0 , 0, REG_0144_HDMIRX_INNER_MISC_0_REG_DVI2MIU_TST_3_S2);

     vIO32WriteFld_1(REG_00CC_HDMIRX_INNER_MISC_0 , 0, REG_00CC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_PORT_SEL_OV_S0);
     vIO32WriteFld_1(REG_00CC_HDMIRX_INNER_MISC_0 , 0, REG_00CC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_PORT_SEL_OVE_S0);
     vIO32WriteFld_1(REG_00EC_HDMIRX_INNER_MISC_0 , 0, REG_00EC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_PORT_SEL_OV_S1);
     vIO32WriteFld_1(REG_00EC_HDMIRX_INNER_MISC_0 , 0, REG_00EC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_PORT_SEL_OVE_S1);
     vIO32WriteFld_1(REG_014C_HDMIRX_INNER_MISC_0 , 0, REG_014C_HDMIRX_INNER_MISC_0_REG_DVI2MIU_PORT_SEL_OV_S2);
     vIO32WriteFld_1(REG_014C_HDMIRX_INNER_MISC_0 , 0, REG_014C_HDMIRX_INNER_MISC_0_REG_DVI2MIU_PORT_SEL_OVE_S2);

     vIO32WriteFld_1(REG_00CC_HDMIRX_INNER_MISC_0 , 0,   REG_00CC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_EMP_SEL_S0);
     vIO32WriteFld_1(REG_00CC_HDMIRX_INNER_MISC_0 , 0x4, REG_00CC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_FIFO_RD_OFFSET_S0);
     vIO32WriteFld_1(REG_00EC_HDMIRX_INNER_MISC_0 , 0,   REG_00EC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_EMP_SEL_S1);
     vIO32WriteFld_1(REG_00EC_HDMIRX_INNER_MISC_0 , 0x4, REG_00EC_HDMIRX_INNER_MISC_0_REG_DVI2MIU_FIFO_RD_OFFSET_S1);
     vIO32WriteFld_1(REG_014C_HDMIRX_INNER_MISC_0 , 0,   REG_014C_HDMIRX_INNER_MISC_0_REG_DVI2MIU_EMP_SEL_S2);
     vIO32WriteFld_1(REG_014C_HDMIRX_INNER_MISC_0 , 0x4, REG_014C_HDMIRX_INNER_MISC_0_REG_DVI2MIU_FIFO_RD_OFFSET_S2);

     vIO32WriteFld_1(REG_00D0_HDMIRX_INNER_MISC_0 , 0, REG_00D0_HDMIRX_INNER_MISC_0_REG_EMP2MIU_REP_MODE_EN_S0);
     vIO32WriteFld_1(REG_00F0_HDMIRX_INNER_MISC_0 , 0, REG_00F0_HDMIRX_INNER_MISC_0_REG_EMP2MIU_REP_MODE_EN_S1);
     vIO32WriteFld_1(REG_0150_HDMIRX_INNER_MISC_0 , 0, REG_0150_HDMIRX_INNER_MISC_0_REG_EMP2MIU_REP_MODE_EN_S2);
     vIO32WriteFld_1(REG_00D0_HDMIRX_INNER_MISC_0 , 0, REG_00D0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_FIFO_PT_RST_S0);
     vIO32WriteFld_1(REG_00F0_HDMIRX_INNER_MISC_0 , 0, REG_00F0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_FIFO_PT_RST_S1);
     vIO32WriteFld_1(REG_0150_HDMIRX_INNER_MISC_0 , 0, REG_0150_HDMIRX_INNER_MISC_0_REG_DVI2MIU_FIFO_PT_RST_S2);
     vIO32WriteFld_1(REG_00D0_HDMIRX_INNER_MISC_0 , 0, REG_00D0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_DIS_CLK_STABLE_RST_S0);
     vIO32WriteFld_1(REG_00F0_HDMIRX_INNER_MISC_0 , 0, REG_00F0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_DIS_CLK_STABLE_RST_S1);
     vIO32WriteFld_1(REG_0150_HDMIRX_INNER_MISC_0 , 0, REG_0150_HDMIRX_INNER_MISC_0_REG_DVI2MIU_DIS_CLK_STABLE_RST_S2);

     vIO32WriteFld_1(REG_00D0_HDMIRX_INNER_MISC_0 , 0, REG_00D0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_EN_S0);
     vIO32WriteFld_1(REG_00F0_HDMIRX_INNER_MISC_0 , 0, REG_00F0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_EN_S1);
     vIO32WriteFld_1(REG_0150_HDMIRX_INNER_MISC_0 , 0, REG_0150_HDMIRX_INNER_MISC_0_REG_DVI2MIU_EN_S2);

     vIO32WriteFld_1(REG_00D0_HDMIRX_INNER_MISC_0 , 0, REG_00D0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_MODE_S0);//1p to 1p
     vIO32WriteFld_1(REG_00F0_HDMIRX_INNER_MISC_0 , 0, REG_00F0_HDMIRX_INNER_MISC_0_REG_DVI2MIU_MODE_S1);
     vIO32WriteFld_1(REG_0150_HDMIRX_INNER_MISC_0 , 0, REG_0150_HDMIRX_INNER_MISC_0_REG_DVI2MIU_MODE_S2);

#endif
    // det path dtop_misc, scaler
    vIO32WriteFld_1(REG_0100_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, HDMI_TIMING_DET_SC, REG_0100_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_SEL);

    // hde/htt/vde stable cnt enable
    vIO32WriteFld_1(REG_013C_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 0x1, REG_013C_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_HDE_STABLE_EN);
    vIO32WriteFld_1(REG_013C_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 0x1, REG_013C_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_HTT_STABLE_EN);
    vIO32WriteFld_1(REG_013C_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 0x1, REG_013C_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_VDE_STABLE_EN);

    // hde_cnt, htt_cnt
    vIO32WriteFld_1(REG_0134_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 0xf, REG_0134_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_HDE_STABLE_CNT);
    vIO32WriteFld_1(REG_0134_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 0xf, REG_0134_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_HTT_STABLE_CNT);

    // vde_cnt
    vIO32WriteFld_1(REG_0138_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 0x2, REG_0138_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_TIMING_DET_VDE_STABLE_CNT);

    //[06:04]: reg_hdmirx_dec_src_det_pix_per_cycle
    //[12]: reg_hdmirx_dec_i_md_det_cont_en (vtt +-1)
    vIO32WriteFld_1(REG_0100_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 7, REG_0100_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_SRC_DET_PIX_PER_CYCLE);
    vIO32WriteFld_1(REG_0100_HDMIRX_DTOP_MISC_0 + u32_dtopmisc_bkofs, 2, REG_0100_HDMIRX_DTOP_MISC_0_REG_HDMIRX_DTOP_I_MD_DET_CONDI_EN);
}

static void _KHal_HDMIRx_Audio_Setting(MS_U8 enInputPortType)
{
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    //[15] reg_n_code_ref_en
    vIO32WriteFld_1(REG_012C_HDMIRX_AUDIO_S0 , 0x0, REG_012C_HDMIRX_AUDIO_S0_REG_N_CODE_REF_EN);

    //[07] reg_en_cts_n_synth
    vIO32WriteFld_1(REG_0110_HDMIRX_AUDIO_S0 , 0x1, REG_0110_HDMIRX_AUDIO_S0_REG_SYNTH_SEL_CTS_REF);
    vIO32WriteFld_1(REG_0110_HDMIRX_AUDIO_S0 , 0x1, REG_0110_HDMIRX_AUDIO_S0_REG_SYNTH_PH_GAIN);
    vIO32WriteFld_1(REG_0110_HDMIRX_AUDIO_S0 , 0x1, REG_0110_HDMIRX_AUDIO_S0_REG_SYNTH_FREQ_GAIN);
    vIO32WriteFld_1(REG_0110_HDMIRX_AUDIO_S0 , 0x0, REG_0110_HDMIRX_AUDIO_S0_REG_SYNTH_PLL_LOCK_FREQ);
    vIO32WriteFld_1(REG_0110_HDMIRX_AUDIO_S0 , 0x0, REG_0110_HDMIRX_AUDIO_S0_REG_SYNTH_256FS_EXPANDER);
    vIO32WriteFld_1(REG_0110_HDMIRX_AUDIO_S0 , 0x0, REG_0110_HDMIRX_AUDIO_S0_REG_SYNTH_PLL_LOCK_SEL);
    vIO32WriteFld_1(REG_0110_HDMIRX_AUDIO_S0 , 0x1, REG_0110_HDMIRX_AUDIO_S0_REG_EN_CTS_N_SYNTH);

    //[15:15] reg_iir_fs_en
    //[07:07] reg_iir_256fs_en
    //[01:00] reg_iir_fs_rate
    vIO32WriteFld_1(REG_013C_HDMIRX_AUDIO_S0 , 0x1, REG_013C_HDMIRX_AUDIO_S0_REG_IIR_FS_EN);
    // vIO32WriteFld_1(REG_013C_HDMIRX_AUDIO_S0+u32_audio_s0_bkofs+u32_i*u32_bk_ofs, 0x0, REG_013C_HDMIRX_AUDIO_S0_REG_IIR_FS_FCE_LK);
    // vIO32WriteFld_1(REG_013C_HDMIRX_AUDIO_S0+u32_audio_s0_bkofs+u32_i*u32_bk_ofs, 0x1, REG_013C_HDMIRX_AUDIO_S0_REG_IIR_FS_BW);
    vIO32WriteFld_1(REG_013C_HDMIRX_AUDIO_S0 , 0x0, REG_013C_HDMIRX_AUDIO_S0_REG_IIR_256FS_EN);
    // vIO32WriteFld_1(REG_013C_HDMIRX_AUDIO_S0+u32_audio_s0_bkofs+u32_i*u32_bk_ofs, 0x1, REG_013C_HDMIRX_AUDIO_S0_REG_IIR_FS_NO_IPT);
    vIO32WriteFld_1(REG_013C_HDMIRX_AUDIO_S0 , 0x1, REG_013C_HDMIRX_AUDIO_S0_REG_IIR_FS_RATE);

    // reg_syn_nf_val[23:0], 0x8ca00
    vIO32WriteFld_1(REG_011C_HDMIRX_AUDIO_S0 , 0xca00, REG_011C_HDMIRX_AUDIO_S0_REG_SYN_NF_VAL_0);
    vIO32WriteFld_1(REG_0120_HDMIRX_AUDIO_S0 , 0x8, REG_0120_HDMIRX_AUDIO_S0_REG_SYN_NF_VAL_1);

    // [15]reg_ld_syn_nf_val
    vIO32WriteFld_1(REG_0118_HDMIRX_AUDIO_S0 , 1, REG_0118_HDMIRX_AUDIO_S0_REG_LD_SYN_NF_VAL);
    // shall delay ???
    vIO32WriteFld_1(REG_0118_HDMIRX_AUDIO_S0 , 0, REG_0118_HDMIRX_AUDIO_S0_REG_LD_SYN_NF_VAL);

    // shall we disable this part to allow sc setting active?
    //[03]:reg_aud_scr_sw_enb
    //[01:00]:reg_aud_src_sel_sw

    // vIO32WriteFld_1(REG_0004_HDMIRX_AUDIO_S0 + u32_audio_s0_bkofs + u32_i * u32_bk_ofs, 0, REG_0004_HDMIRX_AUDIO_S0_REG_AUD_SRC_SEL_SW);
    vIO32WriteFld_1(REG_0004_HDMIRX_AUDIO_S0 , 1, REG_0004_HDMIRX_AUDIO_S0_REG_AUD_SRC_SW_ENB);

    // reg_hdmi_audio_status_en:audio fifo overflow/underflow for QD980 cts 8-21/23 audio cutting
    // aud_top_06[6] Underdlow/Overflow.
    vIO32WriteFld_1(REG_0018_HDMIRX_AUDIO_S0 , 0, Fld(8,0,AC_FULLB0));
    vIO32WriteFld_1(REG_0014_HDMIRX_AUDIO_S0 , 1, FLD_BIT(6));
    // enable reg_aff_ofst_rdy_cfg  trigger by FIFO empty event.
    vIO32WriteFld_1(REG_000C_HDMIRX_AUDIO_S0 , 0x1, REG_000C_HDMIRX_AUDIO_S0_REG_AFF_OFST_RDY_CFG);

    // reg default value
    // aud_top_05[2] gcp avmute or avmute online
    // vIO32WriteFld_1(REG_0014_HDMIRX_AUDIO_S0+u32_audio_s0_bkofs+u32_i*u32_bk_ofs, 1, FLD_BIT(2));
}

void _KHAL_HDMIRx_Reset_TGen(MS_U8 enInputPortType,ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    enInputPortType = enInputPortType;
    vIO32WriteFld_1(REG_0080_P0_DSCD_SERVICES ,0x00, REG_0080_P0_DSCD_SERVICES_REG_TG_HTT);
    //msWrite2ByteMask(REG_HDMIRX_DSCD_SERVICES_20_L, 0x00, BMASK(13:0));
    //pstHDMIPollingInfo->u8DSCFifoErrorRegenCnt = 0;
    pstHDMIPollingInfo->u16PreHTT = 0;
    pstHDMIPollingInfo->u16PreVTT = 0;
    pstHDMIPollingInfo->u16PreVSB = 0;

#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===_KHAL_HDMIRx_Reset_TGen===\n");
#endif
}

void _KHal_HDMIRx_UpdateCTSNValue(void)
{
    MS_U32 u32PortBankOffset;
    u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(0);

    //load current CTS/N value first
    //W2BYTE(REG_HDMI_DUAL_0_12_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_12[15:0]: CTS[15:0] from ACR packet
    vIO32Write2B_1(REG_0034_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xffff);
    //W2BYTE(REG_HDMI_DUAL_0_13_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_13[15:0]: N[15:0] from ACR packet
    vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xffff);
    //W2BYTEMSK(REG_HDMI_DUAL_0_14_L +u32HDMIBankOffset, 0xFF, 0xFF); // hdmi_u0_14[7:4]: N[19:16]; [3:0]: CTS[19:16] from ACR packet
    vIO32WriteFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xf, REG_003C_P0_HDMIRX_PKT_DEP_0_REG_CTS_1);
    vIO32WriteFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xf, REG_003C_P0_HDMIRX_PKT_DEP_0_REG_N_1);

    //update CTS value
    //W2BYTE(REG_HDMI_DUAL_0_0A_L +u32HDMIBankOffset, R2BYTE(REG_HDMI_DUAL_0_12_L +u32HDMIBankOffset)); // hdmi_u0_0A[15:0]: Limited CTS[15:0]
    vIO32Write2B_1(REG_0024_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, u2IO32Read2B_1(REG_0034_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset));
    //W2BYTEMSK(REG_HDMI_DUAL_0_09_L +u32HDMIBankOffset, (R2BYTE(REG_HDMI_DUAL_0_14_L +u32HDMIBankOffset) & 0x0F) << 8, 0x0F00); // hdmi_u0_09[11:8]: Limited CTS[19:16]
    vIO32WriteFld_1(REG_0020_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, u2IO32Read2B_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset) & 0x000f, REG_0020_P0_HDMIRX_PKT_DEP_0_REG_CTS_LMT_1);

    //update N value
    //W2BYTE(REG_HDMI_DUAL_0_0B_L +u32HDMIBankOffset, R2BYTE(REG_HDMI_DUAL_0_13_L +u32HDMIBankOffset)); // hdmi_u0_0B[15:0]: Limited N[15:0]
    vIO32Write2B_1(REG_0028_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset));
    //W2BYTEMSK(REG_HDMI_DUAL_0_09_L +u32HDMIBankOffset, (R2BYTE(REG_HDMI_DUAL_0_14_L +u32HDMIBankOffset) & 0xF0) << 8, 0xF000); // hdmi_u0_09[15:12]: Limited N[19:16]
    vIO32WriteFld_1(REG_0020_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, (u2IO32Read2B_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset) & 0x00f0) >> 4, REG_0020_P0_HDMIRX_PKT_DEP_0_REG_N_LMT_1);

    //setup CTS/N range
    //W2BYTE(REG_HDMI_DUAL_0_0C_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_0C[15:8]: N range; [7:0]: CTS range
    vIO32Write2B_1(REG_002C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xffff);
    vIO32Write2B_1(REG_0030_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xffff);

    //upload new CTS/N value
    //W2BYTEMSK(REG_HDMI_DUAL_0_07_L +u32HDMIBankOffset, BIT(15), BIT(15)); // hdmi_u0_07[15]: reg_update_lmt_ctsn
    vIO32WriteFld_1(REG_001C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_001C_P0_HDMIRX_PKT_DEP_0_REG_UPDATE_LMT_CTSN);
}

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
MS_U8 _Hal_tmds_DeCodeGray(MS_U8 u8GrayValue)
{
    MS_U8 u8BinaryValue = 0;

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
MS_U8 _Hal_tmds_EnCodeGray(MS_U8 u8BinaryValue)
{
    MS_U8 u8GrayValue = u8BinaryValue ^ (u8BinaryValue >> 1);

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
MS_BOOL _Hal_tmds_ActiveCableCheckPhase(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_BOOL bActiveCableFlag = FALSE;
    MS_U8 u8temp = 0;
    MS_U8 u8PhaseCount[3] = {0};
    volatile MS_U16 u16temp = 1000;
    MS_U16 u16RegValue = 0;
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    MS_U32 u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortType);

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
void _Hal_tmds_GetNewPHYEQValue(MS_U8 enInputPortType, MS_U8 *pu8EQValue)
{
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
//                  Hal_tmds_SetMacPowerDown()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void Hal_tmds_SetMacPowerDown(MS_U8 enInputPortType, MS_BOOL bPowerDown)
{
    enInputPortType = enInputPortType;
    MS_U32 u32_plltop_bkofs = _KHal_HDMIRx_GetPLLTOPBankOffset(enInputPortType);
    if(bPowerDown)
    {
        vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x1, REG_01A8_P0_PLL_TOP_REG_PD_TMDSPLL);
        vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x0, REG_01AC_P0_PLL_TOP_REG_EN_CLKO_PIX);
    }
    else
    {
        vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0, REG_01A8_P0_PLL_TOP_REG_PD_TMDSPLL);
        vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x1, REG_01AC_P0_PLL_TOP_REG_EN_CLKO_PIX);
    }

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
void Hal_HDMI_SetPowerDown(MS_U8 enInputPortType, MS_BOOL bPowerDown)
{
    volatile MS_U16 u16temp = 400;
    MS_U32 u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    //MS_U32 u32PHY2P1PMBankOffset = 0; //_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
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
    Hal_tmds_SetMacPowerDown(enInputPortType, bPowerDown);

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
MS_U8 Hal_HDMI_ComboPortMapping2DDCGPIOConfig(MS_U8 enInputPortType)
{
    //MT9701_TBD_ENABLE confirm MT9701 DDC GPIO mapping?

    MS_U8 u8DDCPortIndex = HDMI_DDC_None;

    switch(enInputPortType)
    {
        case HDMI_INPUT_PORT0:
                u8DDCPortIndex = HDMI_Rx0;
            break;

        case HDMI_INPUT_PORT1:
                u8DDCPortIndex = HDMI_Rx1;
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
void Hal_HDMI_DDCControl(MS_U8 enInputPortType, MS_BOOL bEnable)
{
    //MS_U8 u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortType);
    Bool bFlag = 0;
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]Port :%d , bEnable :%d %s\n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortType,bEnable, "\033[m");
#endif
#if (ENABLE_HDMI_SCDC || ENABLE_HPD_REPLACE_MODE)
    bFlag = bEnable;
#endif

    switch(enInputPortType)
    {
        case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_SCDC_P0_00_L, bFlag, BIT(0)); //scdc_p1_00[0]: SCDC enable
            break;

        case HDMI_INPUT_PORT1:
            msWrite2ByteMask(REG_SCDC_P1_00_L, bFlag, BIT(0)); //scdc_p1_00[0]: SCDC enable
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
    MS_U32 u32PMSCDCBankOffset = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortSelect);

    // scdc_02[13]: Enable CPU write (not enable)
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32PMSCDCBankOffset, 0, REG_0008_SCDC_P0_REG_ENWRITE_SCDC);
    // scdc_02[9:0]: SCDC CPU r/w address
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32PMSCDCBankOffset, u8Offset, REG_0008_SCDC_P0_REG_CPU_ADR_REG);
    // scdc_02[10]: SCDC address load pulse generate
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_0008_SCDC_P0_REG_LOAD_ADR_P);

    // wait read ready
    // scdc_04[9]: CPU read busy status
    while(u4IO32ReadFld_SCDC(REG_0010_SCDC_P0 + u32PMSCDCBankOffset,REG_0010_SCDC_P0_REG_CPU_RD_BUSY_SCDC));

    // scdc_04[7:0]: CPU read data port (for scdc)
    u8XA8Data = (MS_U8)u4IO32ReadFld_SCDC(REG_0010_SCDC_P0 + u32PMSCDCBankOffset,REG_0010_SCDC_P0_REG_SCDC_DATA_PORT_RD);

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
    MS_U32 u32PMSCDCBankOffset = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortSelect);

    // scdc_02[13]: Enable CPU write (not enable)
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_0008_SCDC_P0_REG_ENWRITE_SCDC);
    // scdc_02[9:0]: SCDC CPU r/w address
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32PMSCDCBankOffset, u8Offset, REG_0008_SCDC_P0_REG_CPU_ADR_REG);
    // scdc_02[10]: SCDC address load pulse generate
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_0008_SCDC_P0_REG_LOAD_ADR_P);
    // scdc_03[7:0]: CPU write data port (for scdc)
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32PMSCDCBankOffset, u8SCDCValue, REG_000C_SCDC_P0_REG_CPU_WDATA_REG);
    // scdc_03[8]: SCDC data write port pulse generate
    vIO32WriteFld_SCDC(REG_000C_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_000C_SCDC_P0_REG_SCDC_DATA_WR_P);
    // wait write ready
    // scdc_03[9]: CPU write busy status
    while(u4IO32ReadFld_SCDC(REG_000C_SCDC_P0 + u32PMSCDCBankOffset,REG_000C_SCDC_P0_REG_CPU_WR_BUSY_SCDC));

    // scdc_02[13]: Enable CPU write (not enable)
    vIO32WriteFld_SCDC(REG_0008_SCDC_P0 + u32PMSCDCBankOffset, 0, REG_0008_SCDC_P0_REG_ENWRITE_SCDC);
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
void Hal_HDMI_MAC_HDCP_Enable(MS_BOOL bEnable)
{
#if ENABLE_BRINGUP_LOG
    printf("%s [HDMI][%s][%d] bEnable:%d ,time :%d %s \n", "\033[0;32;31m", __FUNCTION__, __LINE__,bEnable,MsOS_GetSystemTime(), "\033[m");
#endif
    vIO32WriteFld_1(REG_005C_P0_HDCP, bEnable? 0x1 : 0, REG_005C_P0_HDCP_REG_DDC_HDCP_EN); //HDCP enalbe for DDC
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
MS_BOOL Hal_HDMI_SCDC_status(MS_U8 enInputPortSelect)
{
    //MS_U8 u8HDMI_DDC_Mapping = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    MS_BOOL bFlag = FALSE;
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            if(msRead2Byte(REG_SCDC_P0_32_L) & (BIT6|BIT4|BIT5)) //bit 6: x74 write, bit 4: x74 read h'70,  bit 5: x74 read
            {
                msWrite2ByteMask(REG_SCDC_P0_32_L, BIT6, BIT6); //IRQ CLR
                bFlag = TRUE;
            }
            break;

        case HDMI_INPUT_PORT1:
            if(msRead2Byte(REG_SCDC_P1_32_L) & (BIT6|BIT4|BIT5)) //bit 6: x74 write, bit 4: x74 read h'70,  bit 5: x74 read
            {
                msWrite2ByteMask(REG_SCDC_P1_32_L, BIT6, BIT6); //IRQ CLR
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
void Hal_HDMI_SCDC_config(MS_U8 enInputPortSelect, MS_U8 bDDC)
{
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]Port :%d , DDC :%d %s\n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortSelect,bDDC, "\033[m");
#endif
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_SCDC_P0_35_L, bDDC ? BIT0 : 0, BIT0); //scdc_p0_35[0]: x74 en
            break;

        case HDMI_INPUT_PORT1:
            msWrite2ByteMask(REG_SCDC_P1_35_L, bDDC ? BIT0 : 0, BIT0); //scdc_p0_35[0]: x74 en
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
void Hal_HDMI_SCDC_Clr(MS_U8 enInputPortSelect)
{
    //MS_U8 u8HDMI_DDC_Mapping = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_SCDC_P0_32_L, 0x00FF, 0x00FF); //IRQ status clr
            //msWrite2ByteMask(REG_SCDC_P0_32_L, 0, 0x00FF);
            break;

        case HDMI_INPUT_PORT1:
            msWrite2ByteMask(REG_SCDC_P1_32_L, 0x00FF, 0x00FF); //IRQ status clr
            //msWrite2ByteMask(REG_SCDC_P1_32_L, 0, 0x00FF);
            break;

        default:
            break;
    }
}

#endif

void KHal_HDMIRx_Set_DE_UNSTABLE_ISR_CallBackFunction(TimingChg_CALL_BACK_FUNC pFunc)
{
    BYTE enInputPort = HDMI_INPUT_PORT0;

    pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc=pFunc;

    if(pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc==NULL)
    {
        for(enInputPort = HDMI_INPUT_PORT0; enInputPort <= HDMI_INPUT_PORT1; enInputPort++)
        {
            _KHal_HDMIRx_IRQ_OnOff(HDMI_IRQ_PM_SCDC,enInputPort,0x03,FALSE);

            _KHal_HDMIRx_IRQ_OnOff(HDMI_IRQ_MAC,enInputPort,0x1,FALSE);
        }
    }
    else
    {
        vIO32WriteFld_1(REG_0020_P0_HDMIRX_DTOP_DEC_MISC,0 , FLD_BIT(0));
    }
}

MS_BOOL KHAL_HDMIRx_SCDC_GetInterruptFlag(void)
{
    return ((msReadByte(REG_000DB9) & BIT1) ?TRUE :FALSE);
}
MS_BOOL KHAL_HDMIRx_PHY_GetInterruptFlag(void)
{
    return ((msRead2Byte(REG_101938) & BIT11) ?TRUE :FALSE);
}
void KHAL_HDMIRx_SCDC_ISRHandler(MS_U8 enInputPortType)
{
    MS_U32 u32_bk_ofs = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortType);
    MS_U8 u8_isr_status = 0;
    if(u2IO32Read2B_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs) & BIT(8)) // ISR status
    {
        vIO32WriteFld_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs, 0x1,FLD_BIT(4));//mask
        vIO32WriteFld_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs, 0x1,FLD_BIT(5));//mask
        u8_isr_status = u4IO32ReadFld_SCDC(REG_001C_SCDC_P0 + u32_bk_ofs, REG_001C_SCDC_P0_REG_SCDC_IRQ_STATUS);
        if(u8_isr_status & BIT5) // 5v int
        {
            vIO32WriteFld_SCDC(REG_0020_SCDC_P0 + u32_bk_ofs, 0x1, FLD_BIT(13));//mask
            vIO32WriteFld_SCDC(REG_001C_SCDC_P0 + u32_bk_ofs, 0x1, FLD_BIT(13));//clr status

            if(pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc!=NULL)
            {
               pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc();
            }

            vIO32WriteFld_SCDC(REG_001C_SCDC_P0 + u32_bk_ofs, 0, FLD_BIT(13));//clr status
            vIO32WriteFld_SCDC(REG_0020_SCDC_P0 + u32_bk_ofs, 0, FLD_BIT(13));//mask
        }
        vIO32WriteFld_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs, 0x1, FLD_BIT(8));//CLR
        vIO32WriteFld_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs, 0, FLD_BIT(4));//mask
        vIO32WriteFld_SCDC(REG_00B0_SCDC_P0 + u32_bk_ofs, 0, FLD_BIT(5));//mask
    }
}

//**************************************************************************
//  [Function Name]:
//                  KHAL_HDMIRx_PHY_IRQ_MASK_CONTROL()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void KHAL_HDMIRx_PHY_ISRHandler(MS_U8 enInputPortType __attribute__ ((unused)))
{

    vIO32WriteFld_1(REG_PHY2P1_1_P0_5C_L, 0x1, FLD_BIT(11));//mask

    if(u2IO32Read2B_1(REG_PHY2P1_1_P0_5F_L) & BIT(11))
    {
        if(pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc!=NULL)
        {
           pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc();
        }

        vIO32WriteFld_1(REG_PHY2P1_1_P0_5E_L, 0x1, FLD_BIT(11));
        vIO32WriteFld_1(REG_PHY2P1_1_P0_5E_L, 0, FLD_BIT(11));
    }

    vIO32WriteFld_1(REG_PHY2P1_1_P0_5C_L, 0, FLD_BIT(11));//unmask
}

//**************************************************************************
//  [Function Name]:
//                  KHal_HDMIRx_TimingChgISR()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void KHal_HDMIRx_TimingChgISR(MS_U8 enInputPortType __attribute__ ((unused)))
{
    if(u2IO32Read2B_1(REG_0030_P0_HDMIRX_DTOP_DEC_MISC) & BIT(0))
    {
        if(pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc!=NULL)
        {
           pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc();
        }

        vIO32WriteFld_1(REG_0030_P0_HDMIRX_DTOP_DEC_MISC, 0x1, FLD_BIT(0));
        vIO32WriteFld_1(REG_0030_P0_HDMIRX_DTOP_DEC_MISC, 0, FLD_BIT(0));
    }
}

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
void Hal_HDMI_PMEnableDVIDetect(MS_U8 enInputPortSelect __attribute__ ((unused)), MS_BOOL bEnable , HDMI_PM_MODE_TYPE enPMModeType)
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
MS_U8 _Hal_tmds_GetFRLRate(MS_U8 enInputPortType)
{
    MS_U32 u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    MS_U8 u8FRLRate = msRead2Byte(REG_PHY2P1_1_P0_57_L +u32PHY2P1BankOffset) &BMASK(3:0);

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
MS_BOOL _Hal_tmds_GetClockStableFlag(MS_U8 enInputPortType)
{
    MS_BOOL bStatusFlag = FALSE;
    MS_U32 u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
MS_U16 Hal_tmds_GetClockRatePort(MS_U8 enInputPortType, MS_U8 u8SourceVersion)
{
    MS_U8 u8_Count = 0;
    MS_U16 u16ClockCount = 0;
    MS_U32 u16ClockRateCal[10] = {0};
    MS_U32 u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;
    u8SourceVersion = u8SourceVersion;

    for(u8_Count = 0; u8_Count<10; u8_Count++)
    {
        u16ClockRateCal[u8_Count] = msRead2Byte(REG_PHY2P1_4_P0_74_L +u32PHY2P1BankOffset); // phy2p1_4_74[15:0]: reg_cr_done_cnt_hold
    }

    u16ClockCount = GetSortMiddleNumber(&u16ClockRateCal[0],10);
    u16ClockCount = u16ClockCount * HDMI_XTAL_CLOCK_MHZ / HDMI_XTAL_DIVIDER;

    return u16ClockCount;
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
MS_U8 _Hal_tmds_GetRatioDetect(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_U8 u8RatioDetect = HDMI_FRL_MODE_NONE;
    MS_U32 u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    MS_U32 ulClockCount = Hal_tmds_GetClockRatePort(enInputPortType, pstHDMIPollingInfo->ucSourceVersion);

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
MS_BOOL Hal_HDMI_GetSCDC5vDetectFlag(MS_U8 enInputPortType)
{
    MS_BOOL bCableDetectFlag = FALSE;
    //MS_U8 u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortType);

    switch(enInputPortType)
    {
        case HDMI_INPUT_PORT0:
        #if HDMI_Cable5v_0
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P0_0C_L) & BIT(14)) ?TRUE: FALSE); // scdc_0C[6]: reg_tx_5v_reset_c
        #else
                bCableDetectFlag = TRUE;
        #endif
            break;

        case HDMI_INPUT_PORT1:
        #if HDMI_Cable5v_1
                bCableDetectFlag = ((msRead2Byte(REG_SCDC_P1_0C_L) & BIT(14)) ?TRUE: FALSE); // scdc_0C[6]: reg_tx_5v_reset_c
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
//                  KHal_HDMIRx_CEDCheck()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void KHal_HDMIRx_CEDCheck(MS_U8 enInputPortSelect)
{
    MS_U32 u32PMSCDCBankOffset = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortSelect);

    if(Hal_HDMI_GetSCDC5vDetectFlag(enInputPortSelect)) // 5V
    {
        if(_Hal_tmds_GetClockStableFlag(enInputPortSelect))
        {
            if(((u2IO32Read2B_SCDC(REG_0034_SCDC_P0 + u32PMSCDCBankOffset) & BIT(1)) == BIT(1))
                && ((u2IO32Read2B_SCDC(REG_0034_SCDC_P0 + u32PMSCDCBankOffset) & BIT(2)) == BIT(2)))
            {
                vIO32WriteFld_SCDC(REG_0034_SCDC_P0 + u32PMSCDCBankOffset, 0, Fld(2,1,AC_MSKB0)); //  scdc_0d[2:1]: error_cnt_ov
            }
        }
    }
    else // 5v drop
    {
        if(((u2IO32Read2B_SCDC(REG_0034_SCDC_P0 + u32PMSCDCBankOffset) & BIT(1)) != BIT(1))
            && ((u2IO32Read2B_SCDC(REG_0034_SCDC_P0 + u32PMSCDCBankOffset) & BIT(2)) != BIT(2)))
        {
            vIO32WriteFld_SCDC(REG_0034_SCDC_P0 + u32PMSCDCBankOffset, 0x3, Fld(2,1,AC_MSKB0)); //  scdc_05[0]: error_cnt_ov
            KHal_HDMIRx_SetSCDCValue(enInputPortSelect, 0x50, 0x00); // 0x50
            KHal_HDMIRx_SetSCDCValue(enInputPortSelect, 0x51, 0x00); // 0x51
            KHal_HDMIRx_SetSCDCValue(enInputPortSelect, 0x52, 0x00); // 0x52
            KHal_HDMIRx_SetSCDCValue(enInputPortSelect, 0x53, 0x00); // 0x53
            KHal_HDMIRx_SetSCDCValue(enInputPortSelect, 0x54, 0x00); // 0x54
            KHal_HDMIRx_SetSCDCValue(enInputPortSelect, 0x55, 0x00); // 0x55
            KHal_HDMIRx_SetSCDCValue(enInputPortSelect, 0x56, 0x00); // 0x56
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
void _Hal_tmds_CheckTxDcBalanceEn(MS_U8 enInputPortType, MS_BOOL bEnable)
{
    UNUSED(enInputPortType);
    MS_U32 u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortType);

    if(bEnable == TRUE) //turn on check TxDcBalance scheme
    {
        vIO32WriteFld_1(REG_0148_P0_HDMIRX_DTOP + u32DTOPBankOffset, 0, REG_0148_P0_HDMIRX_DTOP_REG_DIS_DC_BALANCE_COUNT);
    }
    else
    {
        vIO32WriteFld_1(REG_0148_P0_HDMIRX_DTOP + u32DTOPBankOffset, 1, REG_0148_P0_HDMIRX_DTOP_REG_DIS_DC_BALANCE_COUNT);
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
void _Hal_tmds_SetPGAGainValue(MS_U8 enInputPortType, MS_BOOL bOvenFlag, MS_U8 *pu8PGAvalue)
{
    MS_U32 u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
void _Hal_tmds_Toggle2XMode(MS_U8 enInputPortType __attribute__ ((unused)))
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
void _Hal_tmds_BandDependSetting(MS_U8 enInputPortType)
{
    MS_U16 u16BandInd, i;
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
            //new add -- _TEST_EQ_CS    // MT9701_TBD_ENABLE DP setting
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

            //new add -- _TEST_EQ_CS    // MT9701_TBD_ENABLE DP setting
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


            //new add -- _TEST_EQ_CS    // MT9701_TBD_ENABLE DP setting
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
            //U02 bringup add   // MT9701_TBD_ENABLE DP setting
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
            //new add -- _TEST_EQ_CS    // MT9701_TBD_ENABLE DP setting
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
            //new add -- _TEST_EQ_CS    // MT9701_TBD_ENABLE DP setting
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
            //new add -- _TEST_EQ_CS    // MT9701_TBD_ENABLE DP setting
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
             //2020.Feb.4 need added form ANA team --START  // MT9701_TBD_ENABLE DP setting
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
            //2020.Feb.4 need added form ANA team --START   // MT9701_TBD_ENABLE DP setting
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
             //2020.Feb.4 need added form ANA team --START  // MT9701_TBD_ENABLE DP setting
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
void _Hal_tmds_EnableDfeTap(MS_U8 enInputPortType, MS_BOOL bEnTap1to4, MS_BOOL bEnTap5to12)
{
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
void _Hal_tmds_SACalibration(MS_U8 enInputPortType)
{
    MS_U32 u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    volatile MS_U16 u16temp = 0;

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
void _Hal_tmds_SetNewPHYEQValue(MS_U8 enInputPortType, MS_BOOL bOvenFlag, MS_U8 *pu8EQvalue)
{
    MS_U32 u32PHY2P1BankOffset = 0;  // _Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
void _Hal_tmds_NewPHYSwitch(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_U8 u8PGAValue[4] = {0x0F, 0x0F, 0x0F, 0x0F};
    MS_U32 u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
void mhal_tmds_HDCP2SetDataInfo(MSCombo_TMDS_PORT_INDEX enInputPort, MS_U8 MessageID, MS_U16 DataSize, MS_U8 *pHDCPTxData, MS_U8 *pHDCPTxData_CertRx)
{
#define DEF_MSG_ID_CHK 1
#define DEF_DDC_BUSY_TIMEOUT_CNT 20000

    MS_U16 i = 0;
    MS_U16 u16RdOffset = 130;
    MS_U16 wOffset;
    MS_U32 u32NopCnt = 0;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);

    if (mhal_tmds_HDCP2CheckWriteStart(enInputPort) == TRUE)
    {
        switch (enInputPort)
        {
            case MSCombo_TMDS_PORT_0:
            case MSCombo_TMDS_PORT_1:
            {
                while ((u2IO32Read2B_1(REG_0198_P0_HDCP) & BIT(2)) == 0); //wait until write done; to avoid data corruption
            }
            break;
            default:
            {
            }
            break;
        }

    }

    //enable xiu write
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x01, REG_00C0_HDMIRX_DTOP_MISC_0_REG_XIU2HDCPKEY_EN);
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x00, REG_00C0_HDMIRX_DTOP_MISC_0_REG_EN_HDCP_SRAM_RD);
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x01, REG_00C0_HDMIRX_DTOP_MISC_0_REG_ENWRITE_HDCP);

    switch(enInputPort)
    {
        case MSCombo_TMDS_PORT_0:

            RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;  // xiu to memory access address: 04

            if(MessageID == TMDS_HDCP2_MSG_ID_AKE_SEND_CERT)
            {
                // xiu to memory wirte data: 05
                vIO32WriteFld_1(REG_0014_HDCPKEY, MessageID, REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
#if (DEF_MSG_ID_CHK == 1)
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;

                    while (u4IO32ReadFld_1(REG_0038_HDCPKEY, REG_0038_HDCPKEY_REG_MEM2XIU_RDAT0) != MessageID)
                    {
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                        vIO32WriteFld_1(REG_0014_HDCPKEY, MessageID, REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                    }
                }
#endif

                for ( i = 0; i < (522 + DataSize); i++ )
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                    if (i < 522)
                    {
                        vIO32WriteFld_1(REG_0014_HDCPKEY, msMemReadByte((ULONG)pHDCPTxData_CertRx + i),REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                        while (u4IO32ReadFld_1(REG_0038_HDCPKEY, REG_0038_HDCPKEY_REG_MEM2XIU_RDAT0)  != msMemReadByte((ULONG)pHDCPTxData_CertRx + i))
                        {
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                            vIO32WriteFld_1(REG_0014_HDCPKEY, msMemReadByte((ULONG)pHDCPTxData_CertRx + i),REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                        }
                    }
                    else
                    {
                        vIO32WriteFld_1(REG_0014_HDCPKEY,*(BYTE*)(pHDCPTxData + i - 522),REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                        while (u4IO32ReadFld_1(REG_0038_HDCPKEY, REG_0038_HDCPKEY_REG_MEM2XIU_RDAT0)!= *(BYTE*)(pHDCPTxData + i - 522))
                        {
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                            vIO32WriteFld_1(REG_0014_HDCPKEY,*(BYTE*)(pHDCPTxData + i - 522),REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                            RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                        }
                    }
                }

                vIO32Write2B_1(REG_018C_P0_HDCP, 0x216);
                do
                {
                    //MAsm_CPU_Nop();
                    _nop_();
                    u32NopCnt++;
                }
                while ((u32NopCnt < DEF_DDC_BUSY_TIMEOUT_CNT) && (u2IO32Read2B_1(REG_0064_P0_HDCP) & REG_0064_P0_HDCP_REG_DDC_BUSY));   // hdcp_dual_19[10]: ddc busy
                vIO32WriteFld_1(REG_0190_P0_HDCP, 0x01, REG_0190_P0_HDCP_REG_MESSAGE_LENGTH_UPDATE); // mdssage data length update pulse (sink)
            }
            else
            {
                vIO32WriteFld_1(REG_0014_HDCPKEY, MessageID, REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);  // xiu to memory wirte data: 09

#if (DEF_MSG_ID_CHK == 1)
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;

                    while (u4IO32ReadFld_1(REG_0038_HDCPKEY, REG_0038_HDCPKEY_REG_MEM2XIU_RDAT0) != MessageID)
                    {
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                        vIO32WriteFld_1(REG_0014_HDCPKEY, MessageID, REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + 0x0000;
                    }
                }
#endif

                for(i = 0; i< DataSize; i++)
                {
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address
                    vIO32WriteFld_1(REG_0014_HDCPKEY,*(BYTE*)(pHDCPTxData + i),REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                    RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1; //set address

                    while (u4IO32ReadFld_1(REG_0038_HDCPKEY, REG_0038_HDCPKEY_REG_MEM2XIU_RDAT0)!= *(BYTE*)(pHDCPTxData + i))
                    {
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                        vIO32WriteFld_1(REG_0014_HDCPKEY,*(BYTE*)(pHDCPTxData + i),REG_0014_HDCPKEY_REG_XIU2MEM_WDAT0);
                        RIU[REG_HDCPKEY_04_L] = u16RdOffset + i + 1;
                    }
                }

                vIO32Write2B_1(REG_018C_P0_HDCP, 1 + DataSize);  //  message datalength wrote by sink: 63 (1 + DataSize)
                vIO32WriteFld_1(REG_0190_P0_HDCP, 0x01, REG_0190_P0_HDCP_REG_MESSAGE_LENGTH_UPDATE); // mdssage data length update pulse (sink)

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
void mhal_tmds_HDCP2SetTxData(MSCombo_TMDS_PORT_INDEX enInputPort, MS_U8 MessageID, MS_U16 DataSize, MS_U8 *pHDCPTxData, MS_U8 *pHDCPTxData_CertRx)
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
    MS_U8 i = 0;
    #if ((defined(DEF_COMBO_HDCP2RX_ISR_MODE) && (DEF_COMBO_HDCP2RX_ISR_MODE == 1)))
    MS_U16 u16BankOffset[3] = {0x000, 0x500, 0xC00};
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
MS_BOOL mhal_combo_HDCP2GetReadDownFlag(MS_U8 ucPortSelect)
{
    MS_BOOL bReadDownFlag = FALSE;

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
    //MS_U16 usDTOPBankOffset = 0;//_mhal_tmds_phy_offset(enInputPort);
    UNUSED(enInputPort);

    // [2]: Enable CPU write; [1]: disable SRAM read; [0]: enable xiu2hdcpkey
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x01, REG_00C0_HDMIRX_DTOP_MISC_0_REG_XIU2HDCPKEY_EN); //enable XIU read
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x00, REG_00C0_HDMIRX_DTOP_MISC_0_REG_EN_HDCP_SRAM_RD);
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x01, REG_00C0_HDMIRX_DTOP_MISC_0_REG_ENWRITE_HDCP);

    //wirte 74Ram for HDCP22 version
    //msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L , BIT(10), BIT(10)); //HDCP enalbe for DDC
    //write enable
    vIO32WriteFld_1(REG_0064_P0_HDCP, 0x1, REG_0064_P0_HDCP_REG_ENWRITE_HDCP);
    vIO32WriteFld_1(REG_0064_P0_HDCP, 0x0, REG_0064_P0_HDCP_REG_HDCP_SRAM_ACCESS);

    vIO32WriteFld_1(REG_005C_P0_HDCP, HDMI_HDCP_HDCP2_VERSION, REG_005C_P0_HDCP_REG_CPU_ADR_REG); //CPU r/w address
    vIO32WriteFld_1(REG_0064_P0_HDCP, 0x1, REG_0064_P0_HDCP_REG_LOAD_ADR_P); //HDCP load address pulse

    vIO32WriteFld_1(REG_0060_P0_HDCP, 0x04, REG_0060_P0_HDCP_REG_CPU_WDATA_REG); //bit2: HDCP22 version
    vIO32WriteFld_1(REG_0064_P0_HDCP, 0x1, REG_0064_P0_HDCP_REG_HDCP_DATA_WR_P); //HDCP data write port pulse generate

    //clear RxStatus
    vIO32WriteFld_1(REG_018C_P0_HDCP, 0x00, REG_018C_P0_HDCP_REG_MESSAGE_LENGTH);
    vIO32WriteFld_1(REG_0190_P0_HDCP, 0x01, REG_0190_P0_HDCP_REG_MESSAGE_LENGTH_UPDATE);

    //clear hdcp_enable cipher bit
    //W2BYTEMSK(REG_HDCP_DUAL_P0_4E_L + dwBKOffset, 0, BIT(0));

    //clear xiu status
    vIO32Write2B_1(REG_0198_P0_HDCP, 0x3D);
    vIO32Write2B_1(REG_0198_P0_HDCP, 0x00);

    //Enable write offset
    //b'[0..9]offset; b'[15]: enable bit
    vIO32WriteFld_1(REG_01A0_P0_HDCP, 0x01, REG_01A0_P0_HDCP_REG_IIC2MEM_RADDR_EN);
    vIO32WriteFld_1(REG_01A0_P0_HDCP, TMDS_HDCP2_SOURCE_READ_OFFSET, REG_01A0_P0_HDCP_REG_IIC2MEM_RADDR);

    //Enable MSG length fully writen in
    //bit2:timeout_mask  bit4:msg_length_update_sel
    vIO32WriteFld_1(REG_0168_P0_HDCP, 0x1, FLD_BIT(2));
    vIO32WriteFld_1(REG_0168_P0_HDCP, 0x1, REG_0168_P0_HDCP_REG_MESSAGE_LENGTH_UPDATE_SEL);

    vIO32WriteFld_1(REG_0138_P0_HDCP, 0x1, FLD_BIT(1)); // [10]: reset AES counter when receiving ENC_EN (for HDCP2.2), [2][1]: Enable auto-clear SKE status
    vIO32WriteFld_1(REG_0138_P0_HDCP, 0x1, FLD_BIT(2));
    vIO32WriteFld_1(REG_0138_P0_HDCP, 0x1, FLD_BIT(10));

    //x74 fake HDCP2Version
    msWriteByte(REG_SCDC_P0_35_H, 0x4);
    msWriteByte(REG_SCDC_P1_35_H, 0x4);
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
void mhal_tmds_HDCP2InterruptEnable(MSCombo_TMDS_PORT_INDEX enInputPort, MS_BOOL bEnableIRQ)
{
    UNUSED(enInputPort);
    // MT9701_TBD_ENABLE follow odinson2
    //msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, 1, BIT(0));//[MT9700] msWrite2ByteMask(REG_COMBO_GP_TOP_38_L, 0, BIT(0));
    //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_3B_L +usDTOPBankOffset, bEnableIRQ? 0: BIT(14), BIT(14));
    vIO32WriteFld_1(REG_0054_P0_HDCP, bEnableIRQ? 0: 0x1, REG_0054_P0_HDCP_REG_MASK_IRQ_LEVEL);
    vIO32WriteFld_1(REG_019C_P0_HDCP, bEnableIRQ? 0: 0x3, REG_0054_P0_HDCP_REG_HDCP_X74_INT_FORCE);
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
MS_BOOL mhal_tmds_HDCP2CheckRomCodeResult(void)
{
    #define HDCP22_MBX_START_ADDR 0x103300
    MS_U8 i = 0;

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
//                  mhal_tmds_HDCP2WriteDone()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_BOOL mhal_tmds_HDCP2CheckWriteDone(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    MS_U16 wOffset;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);
    enInputPort=enInputPort;
    //if((enInputPort == MSCombo_TMDS_PORT_0) || (enInputPort == MSCombo_TMDS_PORT_1))
    {
        if(u2IO32Read2B_1(REG_0198_P0_HDCP) & BIT(2)) // IRQ status: 66
        {
            vIO32Write2B_1(REG_0198_P0_HDCP, BIT(4) | BIT(2)); //clear wirte done status, write start status
#if 0
            //msWriteByte(REG_HDCPKEY_3C_H + wOffset, 0x40); //clear dtop w/r done status
            //msWriteByte(REG_HDCPKEY_3C_H + wOffset, 0x00); //clear dtop w/r done status
            // MT9701_TBD_ENABLE need to confirm HDCP2 write done
            if(u2IO32Read2B_1(REG_0130_HDMIRX_INNER_MISC_0) & BIT(0) )
            {
                vIO32Write2B_1(REG_0130_HDMIRX_INNER_MISC_0, BIT(0));
            }
            //msWrite2Byte(REG_HDMIRX_DTOP_P0_3C_L + wOffset, BIT14); //clear wirte done status adcdvi_irq
            //msWrite2Byte(REG_HDMIRX_DTOP_P0_3C_L + wOffset, 0x0000); //clear wirte done status
#endif
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

void _Hal_HDMIRx_DTOP_DEC_MISC_IRQ_OnOff(MS_U8 enInputPortType __attribute__ ((unused)), Bool bEnableIRQ, HDMI_DTOP_DEC_MISC_IRQ_MASK_TYPE type)
{

    MS_U32 u32_dtopdec_bkofs = 0;//_KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    if(type < HDMI_DEC_MISC_HS_POL_CHG_MASK)
    {
        vIO32WriteFld_1(REG_0020_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, bEnableIRQ?0:1, FLD_BIT(type));
    }
    else
    {
        vIO32WriteFld_1(REG_0024_P0_HDMIRX_DTOP_DEC_MISC + u32_dtopdec_bkofs, bEnableIRQ?0:1, FLD_BIT(type-HDMI_DEC_MISC_HS_POL_CHG_MASK));
    }
}

void Hal_HDMI_IRQ_OnOff(EN_KDRV_HDMIRX_INT e_int, MS_U8 enInputPortType, MS_U16 bit_msk, Bool bEnableIRQ)
{
    _KHal_HDMIRx_IRQ_OnOff(e_int, enInputPortType, bit_msk, bEnableIRQ);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_hdmi_DTOPDEC_IRQ_Mask()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_hdmi_DTOPDEC_IRQ_Mask(void)
{
    MSCombo_TMDS_PORT_INDEX enInputPort = 0;

    if(u2IO32Read2B_1(REG_0130_HDMIRX_INNER_MISC_0) & BIT(0) )
    {
        if(pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc!=NULL)
            _Hal_HDMIRx_DTOP_DEC_MISC_IRQ_OnOff(enInputPort, FALSE, HDMI_DEC_MISC_DE_UNSTABLE_MASK);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_hdmi_DTOPDEC_IRQ_Unmask()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_hdmi_DTOPDEC_IRQ_Unmask(void)
{
    MSCombo_TMDS_PORT_INDEX enInputPort = 0;

    if(u2IO32Read2B_1(REG_0130_HDMIRX_INNER_MISC_0) & BIT(0) )
    {
        vIO32Write2B_1(REG_0130_HDMIRX_INNER_MISC_0, BIT(0));

        if(pDEC_MISC_DE_UNSTABLE_ISRCallBackFunc!=NULL)
            _Hal_HDMIRx_DTOP_DEC_MISC_IRQ_OnOff(enInputPort, TRUE, HDMI_DEC_MISC_DE_UNSTABLE_MASK);
    }
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
MS_BOOL mhal_tmds_HDCP2ReadDone(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    MS_U16 wOffset;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);

    if((enInputPort == MSCombo_TMDS_PORT_0) || (enInputPort == MSCombo_TMDS_PORT_1))
    {
        if(u2IO32Read2B_1(REG_0198_P0_HDCP) & BIT(3))     // IRQ status: 66
        {
            vIO32Write2B_1(REG_0198_P0_HDCP, BIT(3)); //clear read done status
#if 0
            // MT9701_TBD_ENABLE need to confirm HDCP2 read done
            if(u2IO32Read2B_1(REG_0130_HDMIRX_INNER_MISC_0) & BIT(0) )
            {
                vIO32Write2B_1(REG_0130_HDMIRX_INNER_MISC_0, BIT(0));
            }

            //msWriteByte(REG_HDMIRX_DTOP_P0_3C_H + wOffset, 0x40); //clear dtop w/r done status
            //msWriteByte(REG_HDMIRX_DTOP_P0_3C_H + wOffset, 0x00); //clear dtop w/r done status
#endif
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
void mhal_tmds_HDCP2GetDataInfo(MSCombo_TMDS_PORT_INDEX enInputPort, MS_U16 ucDataSize, MS_U8 *pHDCPRxData)
{
    MS_U16 i = 0;

    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x01, REG_00C0_HDMIRX_DTOP_MISC_0_REG_XIU2HDCPKEY_EN); //enable XIU read
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x01, REG_00C0_HDMIRX_DTOP_MISC_0_REG_EN_HDCP_SRAM_RD);
    vIO32WriteFld_1(REG_00C0_HDMIRX_DTOP_MISC_0, 0x00, REG_00C0_HDMIRX_DTOP_MISC_0_REG_ENWRITE_HDCP);
    switch(enInputPort)
    {
        case MSCombo_TMDS_PORT_0:
            RIU[REG_HDCPKEY_04_L] = 0x0000; // xiu to memory access address: 04
            #if 1
            for ( i = 0; i < ucDataSize; i++ )
            {
                pHDCPRxData[i] = u4IO32ReadFld_1(REG_0038_HDCPKEY, REG_0038_HDCPKEY_REG_MEM2XIU_RDAT0); // memory to xiu read data (port0): 0E
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
void mhal_tmds_HDCP2GetRxData(MSCombo_TMDS_PORT_INDEX enInputPort, MS_U8 *pHDCPRxData)
{
    MS_U16 wOffset;
    wOffset = 0;
    MS_U16 message_length_receive = 0 ;
    message_length_receive = u4IO32ReadFld_1(REG_0190_P0_HDCP, REG_0190_P0_HDCP_REG_MESSAGE_LENGTH_RECEIVE);

    if(message_length_receive > TMDS_HDCP2_RX_QUEUE_SIZE)
    {
        HDMI_HAL_DPRINTF("** message_length_receive :%d , over 129\r\n", message_length_receive);
    }
    else
    {
        mhal_tmds_HDCP2GetDataInfo(enInputPort, message_length_receive, pHDCPRxData);  // message length receive: 64
    }
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
MS_BOOL mhal_tmds_HDCP2CheckWriteStart(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    MS_U16 wOffset;
    wOffset = 0;//_mhal_tmds_phy_offset(enInputPort);
    UNUSED(enInputPort);
    UNUSED(wOffset);
    // write IRQ start
    //if((enInputPort == MSCombo_TMDS_PORT_0) || (enInputPort == MSCombo_TMDS_PORT_1))
    {
        if(u2IO32Read2B_1(REG_0198_P0_HDCP) & BIT(4))     // IRQ status: 66
        {
            vIO32Write2B_1(REG_0198_P0_HDCP, 0x10);    // IRQ write start clear: 66
            vIO32Write2B_1(REG_0198_P0_HDCP, 0x00);
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
//                  [0] : BCH error (corrected)
//                  [1] : BCH error
//                  [2] : Checksum error
//                  [4] : Audio sample error
//                  [5] : Audio sample parity bit error
//                  [10] : Audio sample packet present bit 2 channel mapping error
//**************************************************************************
MS_U8 mhal_tmds_HDMIGetErrorStatus(MS_U8 ucHDMIInfoSource __attribute__((unused)), MS_U8 u8value, MS_BOOL bRead, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__((unused)))
{
    MS_U8 u8ErrorStatus = 0;
    MS_U16 u16RegValue = 0;

    MS_U32 u32PortBankOffset = 0;//_KHal_HDMIRx_GetPortBankOffset((E_MUX_INPUTPORT)(INPUT_PORT_DVI0 + ucHDMIInfoSource));

    //h0e   h0e 15  0   reg_depack_dec_irq_clr_0    15  0   16  b0  rw
    //"[0]: gcp_pkt_diff[1]: ai_pkt_diff[2]: spd_pkt_diff[3]: rsv_pkt_diff[4]: gcp_rcv[5]: avi_rcv
    //[6]: spd_rcv[7]: rsv[8]: short_island_err_irq[9]: check_sum_err
    //[10]: unsup_packet_err[11]: 0[12]: 0[13]: bch_err[14]: bch_err_delay[15]: bch_corrected"

    if (bRead)
    {
        //u16RegValue = R2BYTE(REG_HDMIRX_DTOP_PKT_P0_10_L +u32DTOPBankOffset);
        // hdmirx_dtop_pkt_10[8]: Checksum Error occurred; packet discarded;
        // [13]: BCH parity Error occurred; packet discarded;
        // [15]: Single bit BCH parity Error occurred and Corrected
        u16RegValue = u2IO32Read2B_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);

        if (u16RegValue & BIT(0))
        {
            u8ErrorStatus = u8ErrorStatus | HDMI_BCH_ERROR_CORRECTED;
        }
        if (u16RegValue & BIT(1))
        {
            u8ErrorStatus = u8ErrorStatus | HDMI_BCH_ERROR;
        }
#if(!COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
        if (u16RegValue & BIT(2))
        {
            u8ErrorStatus = u8ErrorStatus | HDMI_CHECKSUM_ERROR;
        }
#endif
        if (u16RegValue & BIT(4))//Audio sample error
        {
            u8ErrorStatus = u8ErrorStatus | HDMI_SAMPLE_ERROR;
        }
        if (u16RegValue & BIT(5))//Audio sample parity bit error
        {
            u8ErrorStatus = u8ErrorStatus | HDMI_SAMPLE_PARITY_ERROR;
        }

        if (u16RegValue & BIT(10))//Unsupported packet received
        {
            u8ErrorStatus = u8ErrorStatus | HDMI_PRESENT_BIT_ERROR;
        }



        //reg_dtop_dec_misc_irq_status
        //[2]:dc_fifo_udf
        //[3]:dc_fifo_ovf
        u16RegValue = u2IO32Read2B_1(REG_0030_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset);
        //Deep color FIFO overflow (write 1 to clear)
        if (u16RegValue & BIT(3))
        {
            u8ErrorStatus = u8ErrorStatus | BIT(6);
        }

        //Deep color FIFO underflow (write 1 to clear)
        if (u16RegValue & BIT(2))
        {
            u8ErrorStatus = u8ErrorStatus | BIT(7);
        }
    }
    else
    {
        if (u8value & HDMI_BCH_ERROR_CORRECTED)
        {
            u16RegValue = u16RegValue | BIT(0);
        }
        if (u8value & HDMI_BCH_ERROR)
        {
            u16RegValue = u16RegValue | BIT(1);
        }
#if(!COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
        if (u8value & HDMI_CHECKSUM_ERROR)
        {
            u16RegValue = u16RegValue | BIT(2);
        }
#endif
        if (u8value & HDMI_SAMPLE_ERROR)
        {
            u16RegValue = u16RegValue | BIT(4);
        }
        if (u8value & HDMI_SAMPLE_PARITY_ERROR)
        {
            u16RegValue = u16RegValue | BIT(5);
        }
        if (u8value & HDMI_PRESENT_BIT_ERROR)
        {
            u16RegValue = u16RegValue | BIT(10);
        }


        vIO32Write2B_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, u16RegValue);


        u16RegValue = 0;
        if (u8value & BIT(6))
        {
            u16RegValue = u16RegValue | BIT(3);
        }
        if (u8value & BIT(7))
        {
            u16RegValue = u16RegValue | BIT(2);
        }
        vIO32Write2B_1(REG_0030_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset, u16RegValue);

    }
    return u8ErrorStatus;
}

#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
//**************************************************************************
//  [Function Name]:
//                  mhal_tmds_HDMIGetErrorStatus_Partial()
//  [Description]:
//                  use to return HDMI error status
//  [Arguments]:
//                  enInputPort : combo port
//  [Return]:
//                  Checksum error status
//**************************************************************************
MS_U8 mhal_tmds_HDMIGetErrorStatus_Partial(MSCombo_TMDS_PORT_INDEX enInputPort __attribute__((unused)))
{
    BYTE Checksum_error = FALSE;
    MS_U32 u32PortBankOffset;

   u32PortBankOffset = 0;//_KHal_HDMIRx_GetPortBankOffset((E_MUX_INPUTPORT)(INPUT_PORT_DVI0 + enInputPort));
   //temp = u4IO32ReadFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0+ u32PortBankOffset,REG_0010_P0_HDMIRX_PKT_DEP_0_REG_CHECKSUM_ERR));
   if(u4IO32ReadFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0+ u32PortBankOffset,REG_0010_P0_HDMIRX_PKT_DEP_0_REG_CHECKSUM_ERR))
   {
       Checksum_error = BIT2;
       vIO32WriteFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0010_P0_HDMIRX_PKT_DEP_0_REG_CHECKSUM_ERR);//write 1 clear
   }

    return Checksum_error;
}
#endif

MS_U8 mhal_hdmiRx_CheckHDCPState(void)
{
    enum HDMI_HDCP_STATE enHDCPState = HDMI_HDCP_NO_ENCRYPTION;

    if(u4IO32ReadFld_1(REG_006C_P0_HDCP, REG_006C_P0_HDCP_REG_ENC_EN_STATUS) == 0x1) // encryption enable
    {
        if(u4IO32ReadFld_1(REG_013C_P0_HDCP, REG_013C_P0_HDCP_REG_HDCP22_AUTH_STATUS) == 0x01) // SKE successful
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
MS_BOOL _Hal_tmds_GetClockChangeFlag(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_BOOL bClockBigChangeFlag = FALSE;
    MS_U16 u16ClockCount = 0;
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    u16ClockCount = Hal_tmds_GetClockRatePort(enInputPortType, pstHDMIPollingInfo->ucSourceVersion);

    if(msRead2Byte(REG_PHY2P1_4_P0_5F_L +u32PHY2P1BankOffset) & BIT(2)) // phy2p1_4_5F[2]: reg_clk_big_chg_sts
    {
        msWrite2ByteMask(REG_PHY2P1_4_P0_5F_L +u32PHY2P1BankOffset, BIT(2), BIT(2)); // phy2p1_4_5F[2]: reg_clk_big_chg_sts

        bClockBigChangeFlag = TRUE;
    }

    if((MS_U8)(ABS_MINUS(pstHDMIPollingInfo->usClockCount, u16ClockCount)&0xff) > HDMI_CLOCK_UNSTABLE_OFFSET)
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
MS_BOOL _Hal_tmds_GetSquelchFlag(MS_U8 enInputPortType)
{
    MS_BOOL bStatusFlag = FALSE;
    //MS_U32 u32PHY2P1PMBankOffset = 0;//_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
    MS_U16 temp =0;
    // MT9701_TBD_ENABLE, bank offset correct, need to confirm sqh status reg from DP?
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
void _Hal_tmds_ResetDLPF(MS_U8 enInputPortType __attribute__ ((unused)))
{
    UINT u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    volatile MS_U16 dly;

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
EN_HDMI_14_TMDS_CLOCK_RANGE _Hal_tmds_GetH14AutoEQEnable(MS_U8 enInputPortType, MS_U32 ulClockRate)
{
    EN_HDMI_14_TMDS_CLOCK_RANGE enClockRange = HDMI_14_TMDS_CLOCK_NONE;
    MS_U32 u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if(ulClockRate < 75)
    {
        MS_U8 u8EQValue[4] = {10, 10, 10, 10};

        // aaba reference to full screen
        msWrite2Byte(REG_PHY2P1_0_P0_27_L +u32PHY2P1BankOffset, 0x9300); // phy2p1_0_27[15:0]: reg_eq_top_condi
        _Hal_tmds_ResetDLPF(enInputPortType);

        _Hal_tmds_SetNewPHYEQValue(enInputPortType, TRUE, u8EQValue);

        enClockRange = HDMI_14_TMDS_CLOCK_UNDER_75M;
    }
    else if(ulClockRate < 135)
    {
        MS_U8 u8EQValue[4] = {15, 15, 15, 15};

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
void _Hal_tmds_ActiveCablePatch(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_U8 u8EQValue[4] = {2, 2, 2, 2};
    MS_U8 u8PGAValue[4] = {0x0F, 0x0F, 0x0F, 0x0F}; // eg_pga_gain_lx_ov[7:6] = 2'b11 for H20 patch
    MS_U16 u16temp = 1000;
    MS_U32 u32PHY2P1BankOffset =0;// _Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
void _Hal_tmds_ReleaseActiveCablePatch(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    MS_U8 u8PGAValue[4] = {0xCF, 0xCF, 0xCF, 0xCF}; // eg_pga_gain_lx_ov[7:6] = 2'b11 for H20 patch

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
MS_BOOL _Hal_tmds_CheckActiveCableProc(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_BOOL bCheckDoneFlag = FALSE;
    MS_BOOL bNextStateFlag = FALSE;
    MS_U16 u16temp = 2000;
    stHDMI_ERROR_STATUS stErrorStatus = {0,0,0,0,0,0,0,0};
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
void _Hal_tmds_NewPHYAutoEQTrigger(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)), MS_U16 LanSel)
{
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
MS_BOOL _Hal_tmds_GetEQFlowDoneFlag(MS_U8 enInputPortType, MS_U8 opt)
{
    MS_U8 bEQFlowDoneFlag = 0;
    MS_U32 u32PHY2P1BankOffset = 0; //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
MS_BOOL _Hal_tmds_GetErrorCountStatus(MS_U8 enInputPortType, MS_BOOL bHDMI20Flag, stHDMI_ERROR_STATUS *pstErrorStatus)
{
    MS_BOOL bWordBoundaryUnlock = FALSE;
    MS_BOOL bDecodeError = FALSE;
    MS_U16 u16DecodeErrorThr = 0;

    enInputPortType = enInputPortType;

    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortType);
    if (u2IO32Read2B_1(REG_013C_P0_HDMIRX_DTOP + u32PortBankOffset) & BIT(3))
    {
        bWordBoundaryUnlock = TRUE;
        pstErrorStatus->bWordBoundaryErrorL0 = TRUE;
    }
    if (u2IO32Read2B_1(REG_013C_P0_HDMIRX_DTOP + u32PortBankOffset) & BIT(7))
    {
        bWordBoundaryUnlock = TRUE;
        pstErrorStatus->bWordBoundaryErrorL1 = TRUE;
    }
    if (u2IO32Read2B_1(REG_013C_P0_HDMIRX_DTOP + u32PortBankOffset) & BIT(11))
    {
        bWordBoundaryUnlock = TRUE;
        pstErrorStatus->bWordBoundaryErrorL2 = TRUE;
    }

    if (bWordBoundaryUnlock)
    {
        // clear WB unlock: hdmirx_dtop_4F[3][7][11]
        vIO32WriteFld_1(REG_0134_P0_HDMIRX_DTOP + u32PortBankOffset, 0x1, REG_0134_P0_HDMIRX_DTOP_REG_CLR_ERR_CHK);
        vIO32WriteFld_1(REG_0134_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_0134_P0_HDMIRX_DTOP_REG_CLR_ERR_CHK);
    }

    u16DecodeErrorThr = bHDMI20Flag ? HDMI_DECODE_ERROR_THRESHOLD20 : HDMI_DECODE_ERROR_THRESHOLD14;

    vIO32WriteFld_1(REG_0160_P0_HDMIRX_DTOP + u32PortBankOffset, 0xFE, REG_0160_P0_HDMIRX_DTOP_REG_DEC20_TST_BUS_SEL);

    pstErrorStatus->u16DecodeErrorL0 = u2IO32Read2B_1(REG_0164_P0_HDMIRX_DTOP + u32PortBankOffset); // hdmirx_dtop_59[15:0]: reg_tst_bus_rpt0

    if (pstErrorStatus->u16DecodeErrorL0 > u16DecodeErrorThr)
    {
        bDecodeError = TRUE;
    }

    vIO32WriteFld_1(REG_0160_P0_HDMIRX_DTOP + u32PortBankOffset, 0xFD, REG_0160_P0_HDMIRX_DTOP_REG_DEC20_TST_BUS_SEL);

    pstErrorStatus->u16DecodeErrorL1 = u2IO32Read2B_1(REG_0164_P0_HDMIRX_DTOP + u32PortBankOffset); // hdmirx_dtop_59[15:0]: reg_tst_bus_rpt0

    if (pstErrorStatus->u16DecodeErrorL1 > u16DecodeErrorThr)
    {
        bDecodeError = TRUE;
    }

    vIO32WriteFld_1(REG_0160_P0_HDMIRX_DTOP + u32PortBankOffset, 0xFC, REG_0160_P0_HDMIRX_DTOP_REG_DEC20_TST_BUS_SEL);

    pstErrorStatus->u16DecodeErrorL2 = u2IO32Read2B_1(REG_0164_P0_HDMIRX_DTOP + u32PortBankOffset); // hdmirx_dtop_59[15:0]: reg_tst_bus_rpt0

    if (pstErrorStatus->u16DecodeErrorL2 > u16DecodeErrorThr)
    {
        bDecodeError = TRUE;
    }

    if (bDecodeError)
    {
        // clear decode error: hdmirx_dtop_59[15:0]
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP + u32PortBankOffset, 0x1, REG_0100_P0_HDMIRX_DTOP_REG_SW_CLR_ERR_CNT);
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_0100_P0_HDMIRX_DTOP_REG_SW_CLR_ERR_CNT);
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
MS_BOOL _Hal_tmds_ChangePHYCase(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_BOOL bFinishFlag = FALSE;
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

    enInputPortType = enInputPortType;

    if (pstHDMIPollingInfo->u8ChangePHYCase != HDMI_CHANGE_PHY_FINISH)
    {
        _Hal_tmds_ResetDLPF(enInputPortType);
    }

    switch(pstHDMIPollingInfo->u8ChangePHYCase)
    {
        case HDMI_CHANGE_PHY_H14_CASE0:
            {
                MS_U8 u8EQValue[4] = {33, 33, 33, 33};

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
                MS_U8 u8EQValue[4] = {8, 8, 8, 8};

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
                MS_U8 u8EQValue[4] = {16, 16, 16, 16};

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
                MS_U8 u8EQValue[4] = {15, 15, 15, 15};

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
                MS_U8 u8EQValue[4] = {25, 25, 25, 25};

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
                MS_U8 u8EQValue[4] = {15, 15, 15, 15};

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
                MS_U8 u8EQValue[4] = {8, 8, 8, 8};

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
MS_BOOL _Hal_tmds_ActiveCableCheckDLEV(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_BOOL bDLEVFlag = FALSE;
    MS_U8 u8DLEVCnt = 0;
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
MS_BOOL _Hal_tmds_DIGLockCheck(MS_U8 enInputPortType)
{
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);

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
void _Hal_tmds_PHYInit(MS_U8 enInputPortType)
{
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    MS_U16 u16DPRx_AUX_OFFSET = 0;
    MS_U8 u8Index = 0;
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    //MS_U32 u32PHY2P1PMBankOffset = 0;//_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
    enInputPortType = enInputPortType;
    // MT9701_TBD_ENABLE, bank offset correct, need to confirm
    msWriteByteMask(REG_DPRX_PHY_PM_66_L , 0x00, BIT(0)); //NODIE_PD_CLKIN
    // MT9701_TBD_ENABLE, bank offset correct, need to confirm DP AUX
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
    // MT9701_TBD_ENABLE, bank offset correct, need to confirm DP phy pm setting
    if(Input_HDMI_C1 != Input_Nothing)
    {
        msWrite2ByteMask(REG_DPRX_PHY_PM_00_L , 0x0000, BIT(3));
        msWrite2ByteMask(REG_DPRX_PHY_PM_00_L , 0x00F0, BMASK(7:4));
        #if ENABLE_HPD_REPLACE_MODE
        // do nothing
        #else
        msWrite2ByteMask(REG_DPRX_PHY_PM_00_L , 0x0000, BMASK(11:8));//new add
        #endif

        msWrite2ByteMask(REG_DPRX_PHY_PM_01_L , 0x0100, BIT(8));

        msWrite2ByteMask(REG_DPRX_PHY_PM_01_L , 0x0000, BMASK(7:0));
        msWrite2ByteMask(REG_DPRX_PHY_PM_00_L , 0x0000, BMASK(2:0));
    }
    if(Input_HDMI_C2 != Input_Nothing)
    {
        msWrite2ByteMask(REG_DPRX_PHY_PM_20_L , 0x0000, BIT(3));
        msWrite2ByteMask(REG_DPRX_PHY_PM_20_L , 0x00F0, BMASK(7:4));
        #if ENABLE_HPD_REPLACE_MODE
        // do nothing
        #else
        msWrite2ByteMask(REG_DPRX_PHY_PM_20_L , 0x0000, BMASK(11:8));
        #endif

        msWrite2ByteMask(REG_DPRX_PHY_PM_21_L , 0x0100, BIT(8));

        msWrite2ByteMask(REG_DPRX_PHY_PM_21_L , 0x0000, BMASK(7:0));
        msWrite2ByteMask(REG_DPRX_PHY_PM_20_L , 0x0000, BMASK(2:0));
    }
    if(Input_HDMI_C3 != Input_Nothing)
    {
        msWrite2ByteMask(REG_DPRX_PHY_PM_40_L , 0x0000, BIT(3));
        msWrite2ByteMask(REG_DPRX_PHY_PM_40_L , 0x00F0, BMASK(7:4));
        #if ENABLE_HPD_REPLACE_MODE
        // do nothing
        #else
        msWrite2ByteMask(REG_DPRX_PHY_PM_40_L , 0x0000, BMASK(11:8));
        #endif

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


    if( (msEread_GetDataFromEfuse(0x13A)&BIT(7)) == BIT(7))
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
}
#if 0
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
void _Hal_tmds_MACInit(MS_U8 enInputPortSelect __attribute__ ((unused)))
{
#if 0   // mac init setting from MT9700
    //MS_U32 u32PMSCDCBankOffset = 0; //_Hal_tmds_GetPMSCDCBankOffset(enInputPortSelect);
    MS_U32 u32DTOPBankOffset = 0; //_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);
    //MS_U32 u32VEBankOffset = 0; //_Hal_tmds_GetVEBankOffset(enInputPortSelect);
    MS_U32 u32HDMIBankOffset = 0; //_Hal_tmds_GetHDMIBankOffset(enInputPortSelect);

    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_30_L +u32DTOPBankOffset, BIT(15), BMASK(15:8)); //	hdmirx_dtop_30[15]: reg_vs_rpt_sel
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_31_L +u32DTOPBankOffset, BIT(9), BMASK(15:8)); //  hdmirx_dtop_31[15:8]: reg_dep_filter_len
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L +u32DTOPBankOffset, BIT(2)|BIT(1), BMASK(7:0)); //  hdmirx_dtop_47[2]: reg_dis_sscp_only; [1]: reg_dis_ch_info
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_47_L +u32DTOPBankOffset, BIT(15)|BIT(10), BMASK(15:8)); //	hdmirx_dtop_47[15]: reg_scmb_flg_err; [10]: reg_wb_timer_th


    msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_00_L +u32DTOPBankOffset, BIT(1)|BIT(0), BMASK(15:0)); //  hdmirx_dtop_pkt_00[1]: reg_auto_rst_dc_fifo; [0]: reg_en_deep_color
    msWrite2ByteMask(REG_HDMI_U0_07_L +u32HDMIBankOffset,0, BMASK(15:8)); //  hdmi_u0_00[15:8]
    msWrite2ByteMask(REG_HDMI_U0_0D_L +u32HDMIBankOffset,BIT(9)|BIT(4)|BIT(2)|BIT(0), BMASK(15:0)); //	hdmi_u0_0D[15:0]
    msWrite2ByteMask(REG_HDMI_U0_0E_L +u32HDMIBankOffset,BIT(10)|BIT(5)|BIT(3)|BIT(1), BMASK(15:0)); //  hdmi_u0_0E[15:0]
    msWrite2ByteMask(REG_HDMI_U0_0F_L +u32HDMIBankOffset,BIT(11)|BIT(6)|BIT(4)|BIT(2)|BIT(0), BMASK(15:0)); //	hdmi_u0_0F[15:0]
    msWrite2ByteMask(REG_HDMIRX_DTOP_PKT_P0_13_L +u32DTOPBankOffset,BIT(2), BMASK(7:0)); //  hdmi_u0_13[2]: reg_n_0
    msWrite2ByteMask(REG_HDMI_U0_05_L +u32HDMIBankOffset,BIT(7), BMASK(7:0)); //  hdmi_u0_0F[15:0]
    msWrite2ByteMask(REG_HDMI_EM_U0_05_L +u32HDMIBankOffset,0, BMASK(7:0));
    msWrite2ByteMask(REG_HDMI_EM_U0_06_L +u32HDMIBankOffset,0xEF, BMASK(7:0));
    msWrite2ByteMask(REG_HDMIRX_VE_U0_05_L +u32HDMIBankOffset,0, BMASK(7:0));

    // Check decode error setting
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_4D_L +u32DTOPBankOffset, BIT(10), BIT(10)); // hdmirx_dtop_4D[10]: reg_dvt_ph
    //macbook HDCP re-auth issue
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_1F_L +u32DTOPBankOffset, BIT(14)|BIT(15), BMASK(15:14));
#else  // mac init setting from odinson2
    //MS_U32 u32PortBankOffset = 0;
    // RR_Enable
    // scdc_05[15:8]: reg_read_request_retry_period
    //vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32PMSCDCBankOffset,0x68,REG_0014_SCDC_P0_REG_READ_REQUEST_RETRY_PERIOD);//for 9U6[INIT]:h64[MSB]:15.0[LSB]:8.0
    msWriteByte(REG_SCDC_P0_05_H, 0x68);
    msWriteByte(REG_SCDC_P1_05_H, 0x68);
    // scdc_12[15:0]: reg_bus_free_cnt
    //vIO32WriteFld_SCDC(REG_0048_SCDC_P0 + u32PMSCDCBankOffset,0x1600,REG_0048_SCDC_P0_REG_BUS_FREE_CNT);//for 9U6[INIT]:h8000[MSB]:15.0[LSB]:0.0
    msWrite2Byte(REG_SCDC_P0_12_L, 0x1600);
    msWrite2Byte(REG_SCDC_P1_12_L, 0x1600);
    // scdc_20[12]: reg_rr_vld_range_en; [13]: reg_rr_trig_sel
    //vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32PMSCDCBankOffset, 0x1, REG_0080_SCDC_P0_REG_RR_VLD_RANGE_EN);
    //vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32PMSCDCBankOffset, 0x1, REG_0080_SCDC_P0_REG_RR_TRIG_SEL);
    msWrite2ByteMask(REG_SCDC_P0_20_L, BIT(12) , BIT(12));
    msWrite2ByteMask(REG_SCDC_P0_20_L, BIT(13) , BIT(13));
    msWrite2ByteMask(REG_SCDC_P1_20_L, BIT(12) , BIT(12));
    msWrite2ByteMask(REG_SCDC_P1_20_L, BIT(13) , BIT(13));
    // scdc_23[2]: reg_rr_trig_mask - rr_test
    //vIO32WriteFld_SCDC(REG_008C_SCDC_P0 + u32PMSCDCBankOffset, 0x0, Fld(1, 2, AC_MSKB0));
    msWrite2ByteMask(REG_SCDC_P0_23_L, 0 , BIT(2));
    msWrite2ByteMask(REG_SCDC_P1_23_L, 0 , BIT(2));
    // Disable auto detect scramble
    _KHal_HDMIRx_EnableAutoScramble(0 ,FALSE);

    // Check decode error setting
    // hdmirx_dtop_4D[10]: reg_dvt_ph
    //vIO32WriteFld_1(REG_0134_P0_HDMIRX_DTOP + u32PortBankOffset, 0x1, REG_0134_P0_HDMIRX_DTOP_REG_DVT_PH);
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_4D_L, BIT(10), BIT(10));
    // hdmirx_dtop_62[2]: reg_end_c_err_en
    //vIO32WriteFld_1(REG_0188_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_0188_P0_HDMIRX_DTOP_REG_END_C_ERR_EN);
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_62_L, 0x00, BIT(2));
    // hdmirx_dtop_66[0]: reg_wbdry_glitch
    //vIO32WriteFld_1(REG_0198_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_0198_P0_HDMIRX_DTOP_REG_WBDRY_GLITCH);
    msWrite2ByteMask(REG_HDMIRX_DTOP_P0_66_L, 0x00, BIT(0));
    // BCM HDCP re-auth issue
    // hdmirx_hdcp_1F[15:14]: XOR by enc_en
    //vIO32WriteFld_1(REG_007C_P0_HDCP + u32PortBankOffset, 0x3, FLD_BMSK(15 : 14));
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_1F_L, BIT(14)|BIT(15), BMASK(15:14));
    // Enable write offset
    // hdcp_68[9:0] reg_ii2mem_raddr
    // hdcp_68[15:15] reg_iic2mem_addr_en
    //vIO32WriteFld_1(REG_01A0_P0_HDCP + u32PortBankOffset, TMDS_HDCP2_SOURCE_READ_OFFSET, REG_01A0_P0_HDCP_REG_IIC2MEM_RADDR);
    //vIO32WriteFld_1(REG_01A0_P0_HDCP + u32PortBankOffset, 0x1, REG_01A0_P0_HDCP_REG_IIC2MEM_RADDR_EN);
    // MT9701_TBD_ENABLE mhal_tmds_HDCP2Initial do it already , remove this need to confirm?
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_68_L, TMDS_HDCP2_SOURCE_READ_OFFSET, BMASK(9:0));
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_68_L, BIT(15), BIT(15));

    // Enable HDCP MSG length Updated Fully
    // bit2:timeout_mask  bit4:msg_length_update_sel/
    // hdcp_5a[3:2] reg_msg_len_mask_ctrl
    // hdcp_5a[4] reg_msg_len_update_sel
    //vIO32WriteFld_1(REG_0168_P0_HDCP + u32PortBankOffset, 0x1, FLD_BIT(2));
    //vIO32WriteFld_1(REG_0168_P0_HDCP + u32PortBankOffset, 0x1, REG_0168_P0_HDCP_REG_MESSAGE_LENGTH_UPDATE_SEL);
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_5A_L, BIT(2), BIT(2));
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_5A_L, BIT(4), BIT(4));

    // move to _KHal_HDMIRx_MAC_sub_Init
    // dtop_dec[1]: reg_hdmirx_auto_dc_fifo_rst (no use)
    // dtop_dec[4]: reg_en_deep_color
    // vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC+u32_dtop_dec_bkofs,0x1, REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_EN_DEEP_COLOR);

    // hdmirx_pktdec_12[0]: reg_rcv_gcp_sel
    //vIO32WriteFld_1(REG_0048_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x1, REG_0048_P0_HDMIRX_PKT_DEC_REG_RCV_GCP_SEL);
    msWrite2ByteMask(REG_HDMIRX_PKT_DEC_12_L, BIT(0), BIT(0));
    // Frame repetition manual mode
    // hdmirx_ptkdec_15[7:4]: reg_frame_rp_mode
    //vIO32WriteFld_1(REG_0054_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x8, REG_0054_P0_HDMIRX_PKT_DEC_REG_FRAME_RP_MODE);
    msWrite2ByteMask(REG_HDMIRX_PKT_DEC_15_L, BIT(7), BMASK(7:4));
    // where is m6???
    // For xc repetition setting
    // if = 0: repetition times to xc from pkt
    // if = 1: repetition times to xc = 0
    // W2BYTEMSK(REG_HDMIRX_DTOP_PKT_P0_00_L +u32DTOPBankOffset, BIT(14), BIT(14)); // hdmirx_dtop_pkt_00[14]: reg_dc_rep_en

    // For XC AVMUTE_online
    // hdmirx_pkt_dep_2_72[15:0]: reg_vmute_condi (to XC AVMUTE_online)
    // vIO32WriteFld_1(REG_01C8_P0_HDMIRX_PKT_DEP_2 +u32PortBankOffset, BIT(9)|BIT(8)|BIT(6)|BIT(5)|BIT(3)|BIT(1)|BIT(0), FLD_BMSK(9:0));

    // W2BYTEMSK(REG_HDMIRX_VE_U0_04_L +u32VEBankOffset, BMASK(1:0), BMASK(1:0)); // hdmirx_ve_04[0]: reg_vmuteblank; [1]: reg_en_avmute
    // W2BYTEMSK(REG_HDMIRX_VE_U0_07_L +u32VEBankOffset, 0, BIT(6)); // hdmirx_ve_07[6]: reg_video_blank_sel
    // hdmirx_ve_04[0]: reg_vmuteblank ???
    // hdmirx_ve_07[6]: reg_video_blank_sel ???
    //pkt_dep_0_06[11] reg_en_avmute
    //vIO32WriteFld_1(REG_0018_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 1, REG_0018_P0_HDMIRX_PKT_DEP_0_REG_EN_AVMUTE);
    msWrite2ByteMask(REG_HDMIRX_PKT_DEP_0_06_L, BIT(11), BIT(11));
    _KHal_HDMIRx_Bypass422to444(0);

    //Misc setting
    // dtopdec_2b[11:8] reg_8p_de_det_timeout_th
    // to increase the timeout threshold of cable release feature
    // vIO32WriteFld_1(REG_00AC_P0_HDMIRX_DTOP_DEC_MISC +u32PortBankOffset, 1, FLD_BMSK(11:8));

    // reg_dis_video_out_condi ???
    // W2BYTEMSK(REG_HDMIRX_VE_U0_04_L +u32VEBankOffset, BMASK(7:4), BMASK(7:4)); // to avoid unexpected gating

    // unsupported ptk header for tmds
    // pktdec_11[15:8] reg_rsv_pkt_typee
    //vIO32WriteFld_1(REG_0044_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x7f, REG_0044_P0_HDMIRX_PKT_DEC_REG_RSV_PKT_TYPE);
    msWriteByte(REG_HDMIRX_PKT_DEC_11_H, 0x7F);
    // pktdec_16[7:0] reg_support_pkt_header_h
    // pktdec_16[15:8] reg_support_pkt_header_l
    //vIO32WriteFld_1(REG_0058_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x81, REG_0058_P0_HDMIRX_PKT_DEC_REG_SUPPORT_PKT_HEADER_H);
    msWriteByte(REG_HDMIRX_PKT_DEC_16_L, 0x81);
    // vIO32WriteFld_1(REG_0058_P0_HDMIRX_PKT_DEC +u32PortBankOffset, 0x0e, REG_0058_P0_HDMIRX_PKT_DEC_REG_SUPPORT_PKT_HEADER_L);
    // move to _KHal_HDMIRx_MAC_sub_Init
    // already move to dtopdec, dtopdep, dtopmisc
    // i det condition mode, vrr_min=40, i mode false alarm.
    // W2BYTEMSK(REG_HDMIRX_VE_U0_10_L +u32VEBankOffset, BIT(13), BMASK(13:12)); // hdmirx_ve_10[13]: Hsync/Vsync align; [12]: check Vblanking+/-1

    //HDMI don't maintain Dolby function,always keep clr status
    //vIO32WriteFld_1(REG_00BC_P0_HDMIRX_PKT_DEP_2 + u32PortBankOffset, 1, REG_00BC_P0_HDMIRX_PKT_DEP_2_REG_CLR_DOLBY_EDR);
    //vIO32WriteFld_1(REG_0050_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 1, REG_0050_P0_HDMIRX_PKT_DEC_REG_CLR_DOLBY_EDR11);

    msWrite2ByteMask(REG_HDMIRX_PKT_DEP_2_2F_L, BIT(15), BIT(15));
    msWrite2ByteMask(REG_HDMIRX_PKT_DEC_14_L, BIT(14), BIT(14));
#endif
}
#endif
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
void _Hal_tmds_HDMIInit(MS_U8 enInputPortSelect )
{
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortSelect);
    MS_U32 u32PMSCDCBankOffset = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortSelect);
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d]\r\n", __FUNCTION__, __LINE__);

    // RR_Enable
    // scdc_05[15:8]: reg_read_request_retry_period
    vIO32WriteFld_SCDC(REG_0014_SCDC_P0 + u32PMSCDCBankOffset,0x68,REG_0014_SCDC_P0_REG_READ_REQUEST_RETRY_PERIOD);//for 9U6[INIT]:h64[MSB]:15.0[LSB]:8.0

    // scdc_12[15:0]: reg_bus_free_cnt
    vIO32WriteFld_SCDC(REG_0048_SCDC_P0 + u32PMSCDCBankOffset,0x1600,REG_0048_SCDC_P0_REG_BUS_FREE_CNT);//for 9U6[INIT]:h8000[MSB]:15.0[LSB]:0.0

    // scdc_20[12]: reg_rr_vld_range_en; [13]: reg_rr_trig_sel
    vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32PMSCDCBankOffset, 0x1, REG_0080_SCDC_P0_REG_RR_VLD_RANGE_EN);
    vIO32WriteFld_SCDC(REG_0080_SCDC_P0 + u32PMSCDCBankOffset, 0x1, REG_0080_SCDC_P0_REG_RR_TRIG_SEL);

    // scdc_23[2]: reg_rr_trig_mask - rr_test
    vIO32WriteFld_SCDC(REG_008C_SCDC_P0 + u32PMSCDCBankOffset, 0x0, Fld(1, 2, AC_MSKB0));

    // Disable auto detect scramble
    _KHal_HDMIRx_EnableAutoScramble(enInputPortSelect, FALSE);

    // Check decode error setting
    // hdmirx_dtop_4D[10]: reg_dvt_ph
    vIO32WriteFld_1(REG_0134_P0_HDMIRX_DTOP + u32PortBankOffset, 0x1, REG_0134_P0_HDMIRX_DTOP_REG_DVT_PH);

    // hdmirx_dtop_62[2]: reg_end_c_err_en
    vIO32WriteFld_1(REG_0188_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_0188_P0_HDMIRX_DTOP_REG_END_C_ERR_EN);

    // hdmirx_dtop_66[0]: reg_wbdry_glitch
    vIO32WriteFld_1(REG_0198_P0_HDMIRX_DTOP + u32PortBankOffset, 0x0, REG_0198_P0_HDMIRX_DTOP_REG_WBDRY_GLITCH);

    // BCM HDCP re-auth issue
    // hdmirx_hdcp_1F[15:14]: XOR by enc_en
    vIO32WriteFld_1(REG_007C_P0_HDCP + u32PortBankOffset, 0x3, FLD_BMSK(15 : 14));

    // Enable write offset
    // hdcp_68[9:0] reg_ii2mem_raddr
    // hdcp_68[15:15] reg_iic2mem_addr_en
    vIO32WriteFld_1(REG_01A0_P0_HDCP + u32PortBankOffset, TMDS_HDCP2_SOURCE_READ_OFFSET, REG_01A0_P0_HDCP_REG_IIC2MEM_RADDR);
    vIO32WriteFld_1(REG_01A0_P0_HDCP + u32PortBankOffset, 0x1, REG_01A0_P0_HDCP_REG_IIC2MEM_RADDR_EN);

    // Enable HDCP MSG length Updated Fully
    // bit2:timeout_mask  bit4:msg_length_update_sel/
    // hdcp_5a[3:2] reg_msg_len_mask_ctrl
    // hdcp_5a[4] reg_msg_len_update_sel
    vIO32WriteFld_1(REG_0168_P0_HDCP + u32PortBankOffset, 0x1, FLD_BIT(2));
    vIO32WriteFld_1(REG_0168_P0_HDCP + u32PortBankOffset, 0x1, REG_0168_P0_HDCP_REG_MESSAGE_LENGTH_UPDATE_SEL);

    // move to _KHal_HDMIRx_MAC_sub_Init
    // dtop_dec[1]: reg_hdmirx_auto_dc_fifo_rst (no use)
    // dtop_dec[4]: reg_en_deep_color
    // vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC+u32_dtop_dec_bkofs,0x1, REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_EN_DEEP_COLOR);

    // hdmirx_pktdec_12[0]: reg_rcv_gcp_sel
    vIO32WriteFld_1(REG_0048_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x1, REG_0048_P0_HDMIRX_PKT_DEC_REG_RCV_GCP_SEL);

    // Frame repetition manual mode
    // hdmirx_ptkdec_15[7:4]: reg_frame_rp_mode
    vIO32WriteFld_1(REG_0054_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x8, REG_0054_P0_HDMIRX_PKT_DEC_REG_FRAME_RP_MODE);

    // where is m6???
    // For xc repetition setting
    // if = 0: repetition times to xc from pkt
    // if = 1: repetition times to xc = 0
    // W2BYTEMSK(REG_HDMIRX_DTOP_PKT_P0_00_L +u32DTOPBankOffset, BIT(14), BIT(14)); // hdmirx_dtop_pkt_00[14]: reg_dc_rep_en

    // For XC AVMUTE_online
    // hdmirx_pkt_dep_2_72[15:0]: reg_vmute_condi (to XC AVMUTE_online)
    // vIO32WriteFld_1(REG_01C8_P0_HDMIRX_PKT_DEP_2 +u32PortBankOffset, BIT(9)|BIT(8)|BIT(6)|BIT(5)|BIT(3)|BIT(1)|BIT(0), FLD_BMSK(9:0));

    // W2BYTEMSK(REG_HDMIRX_VE_U0_04_L +u32VEBankOffset, BMASK(1:0), BMASK(1:0)); // hdmirx_ve_04[0]: reg_vmuteblank; [1]: reg_en_avmute
    // W2BYTEMSK(REG_HDMIRX_VE_U0_07_L +u32VEBankOffset, 0, BIT(6)); // hdmirx_ve_07[6]: reg_video_blank_sel
    // hdmirx_ve_04[0]: reg_vmuteblank ???
    // hdmirx_ve_07[6]: reg_video_blank_sel ???
    //pkt_dep_0_06[11] reg_en_avmute
    vIO32WriteFld_1(REG_0018_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 1, REG_0018_P0_HDMIRX_PKT_DEP_0_REG_EN_AVMUTE);

    _KHal_HDMIRx_Bypass422to444(enInputPortSelect);

    //Misc setting
    // dtopdec_2b[11:8] reg_8p_de_det_timeout_th
    // to increase the timeout threshold of cable release feature
    // vIO32WriteFld_1(REG_00AC_P0_HDMIRX_DTOP_DEC_MISC +u32PortBankOffset, 1, FLD_BMSK(11:8));

    // reg_dis_video_out_condi ???
    // W2BYTEMSK(REG_HDMIRX_VE_U0_04_L +u32VEBankOffset, BMASK(7:4), BMASK(7:4)); // to avoid unexpected gating

    // unsupported ptk header for tmds
    // pktdec_11[15:8] reg_rsv_pkt_typee
    vIO32WriteFld_1(REG_0044_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x7f, REG_0044_P0_HDMIRX_PKT_DEC_REG_RSV_PKT_TYPE);

    // pktdec_16[7:0] reg_support_pkt_header_h
    // pktdec_16[15:8] reg_support_pkt_header_l
    vIO32WriteFld_1(REG_0058_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x81, REG_0058_P0_HDMIRX_PKT_DEC_REG_SUPPORT_PKT_HEADER_H);
    // vIO32WriteFld_1(REG_0058_P0_HDMIRX_PKT_DEC +u32PortBankOffset, 0x0e, REG_0058_P0_HDMIRX_PKT_DEC_REG_SUPPORT_PKT_HEADER_L);
    // move to _KHal_HDMIRx_MAC_sub_Init
    // already move to dtopdec, dtopdep, dtopmisc
    // i det condition mode, vrr_min=40, i mode false alarm.
    // W2BYTEMSK(REG_HDMIRX_VE_U0_10_L +u32VEBankOffset, BIT(13), BMASK(13:12)); // hdmirx_ve_10[13]: Hsync/Vsync align; [12]: check Vblanking+/-1

    //HDMI don't maintain Dolby function,always keep clr status
    vIO32WriteFld_1(REG_00BC_P0_HDMIRX_PKT_DEP_2 + u32PortBankOffset, 1, REG_00BC_P0_HDMIRX_PKT_DEP_2_REG_CLR_DOLBY_EDR);
    vIO32WriteFld_1(REG_0050_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 1, REG_0050_P0_HDMIRX_PKT_DEC_REG_CLR_DOLBY_EDR11);
// _KHal_HDMIRx_HDMIInit copy from odinson2
    // pkt_dep0_06[6]: reg_en_hbr
    // pkt_dep0_06[10]: blanking when AVmute is active, ??? Is still exist?
    // pkt_dep0_06[11]: enable video mute;
    vIO32WriteFld_1(REG_0018_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0018_P0_HDMIRX_PKT_DEP_0_REG_EN_HBR);
    vIO32WriteFld_1(REG_0018_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0018_P0_HDMIRX_PKT_DEP_0_REG_EN_AVMUTE);

    // ptk_dep_0f[8] manual non-pcm mode
    // ptk_dep_0f[9] auto detect non-pcm mode(*)
    // ptk_dep_0f[10] manual DSD mode
    // ptk_dep_0f[11] auto detect DSD mode
    //W2BYTEMSK(REG_HDMI_DUAL_0_64_L +u32HDMIBankOffset, BIT(1), BMASK(3:0)); // hdmi_u0_64[1]: Auto detect non-PCM mode; [3]: Auto detect DSD mode
    vIO32WriteFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x2, REG_003C_P0_HDMIRX_PKT_DEP_0_REG_AUDIO_MODE);

    //W2BYTEMSK(REG_HDMI3_DUAL_0_3F_L +u32HDMIBankOffset, 0, BIT(11)); // [11] reg_vsif_ful_shift_en
    vIO32WriteFld_1(REG_00FC_P0_HDMIRX_PKT_DEP_2 + u32PortBankOffset, 0x0, REG_00FC_P0_HDMIRX_PKT_DEP_2_REG_VSIF_FUL_SHIFT_EN);

}

void _KHal_HDMIRx_H21MACInit(MS_U8 enInputPortType)
{
    MS_U32 u32PMSCDCBankOffset = _KHal_HDMIRx_GetPMSCDCBankOffset(enInputPortType);
    MS_U32 u32_pkt_bkofs = _KHal_HDMIRx_GetPKTBankOffset(enInputPortType);
    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U32 u32_plltop_bkofs = _KHal_HDMIRx_GetPLLTOPBankOffset(enInputPortType);
    MS_U32 u32_dscdserv_bkofs = _KHal_HDMIRx_GetDSCDSERVICEPBankOffset(enInputPortType);
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortType);
    //DC setting start:
    // dtop_dec_30[1]:reg_auto_rst_dc_fifo
    // dtop_dec_30[4]:reg_en_deep_color,
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x0, REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_AUTO_DC_FIFO_RST);//for 9U6
    vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x1, REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_EN_DEEP_COLOR);

    // move to _KHal_HDMIRx_HDMIInit
    //tk* todo w8 RD confirm
    // pkt_dep0_06[6]: reg_en_hbr
    // pkt_dep0_06[10]: blanking when AVmute is active, ??? Is still exist?
    // pkt_dep0_06[11]: enable video mute;
    // vIO32WriteFld_1(REG_0018_P0_HDMIRX_PKT_DEP_0+u32_pkt_bkofs,0x1, REG_0018_P0_HDMIRX_PKT_DEP_0_REG_EN_HBR);
    // vIO32WriteFld_1(REG_0018_P0_HDMIRX_PKT_DEP_0+u32_pkt_bkofs,0x1, REG_0018_P0_HDMIRX_PKT_DEP_0_REG_EN_AVMUTE);

    //Turn on TMDS_PLL
    // frl_pll_top_6a[6]: reg_pd_tmdspll
    // frl_pll_top_6b[3]: reg_en_clko_pix
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0, REG_01A8_P0_PLL_TOP_REG_PD_TMDSPLL);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x1, REG_01AC_P0_PLL_TOP_REG_EN_CLKO_PIX);

    //tk* todo from script but hawk2
    // frl_dscd_svce_14[0]: reg_tgen_set_ready
    vIO32WriteFld_1(REG_0050_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 0, REG_0050_P0_DSCD_SERVICES_REG_TGEN_SET_READY);
    vIO32WriteFld_1(REG_0050_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 1, REG_0050_P0_DSCD_SERVICES_REG_TGEN_SET_READY);

#ifndef FPGA_ENABLE
    // when fpga_enable, follow setting @ regen
    // Configure sync_pll
    //reg_synpll_ref_hs_en
    //reg_tgen_vrr_en
    // frl_dscd_svce_1C[0]: reg_tgen_de_sft_en
    vIO32WriteFld_1(REG_0068_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 1, REG_0068_P0_DSCD_SERVICES_REG_SYNPLL_REF_HS_EN);
    vIO32WriteFld_1(REG_006C_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 1, REG_006C_P0_DSCD_SERVICES_REG_TGEN_VRR_EN);
    vIO32WriteFld_1(REG_0070_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 1, REG_0070_P0_DSCD_SERVICES_REG_TGEN_DE_SFT_EN);

    //Misc setting
    // dscd_service_1b[0] reg_vrr_en
    // dscd_service_1c[0] reg_de_sft_en
    // dscd_service_1c[11:8] reg_de_sft_prot_opt
    // dscd_service_1e[3:0] reg_dsc_dly
    // dscd_service_1e[14] reg_dsc_hs_sel
    // vIO32WriteFld_1(REG_006C_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 1, REG_006C_P0_DSCD_SERVICES_REG_TGEN_VRR_EN);
    // vIO32WriteFld_1(REG_0070_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 1, REG_0070_P0_DSCD_SERVICES_REG_TGEN_DE_SFT_EN);
    vIO32WriteFld_1(REG_0070_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 1, REG_0070_P0_DSCD_SERVICES_REG_TGEN_DE_SFT_PROT_OPT);
#ifdef  BYPASS_HDMI_LINEBUFFER
    vIO32WriteFld_1(REG_0078_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 0, REG_0078_P0_DSCD_SERVICES_REG_DSC_DLY);
    vIO32WriteFld_1(REG_0078_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 4, REG_0078_P0_DSCD_SERVICES_REG_DSC_ACK_SEL);
    vIO32WriteFld_1(REG_0078_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 0, REG_0078_P0_DSCD_SERVICES_REG_DSC_HS_SEL);
#else
    vIO32WriteFld_1(REG_0078_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 0, REG_0078_P0_DSCD_SERVICES_REG_DSC_DLY);
    vIO32WriteFld_1(REG_0078_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 0, REG_0078_P0_DSCD_SERVICES_REG_DSC_ACK_SEL);
    vIO32WriteFld_1(REG_0078_P0_DSCD_SERVICES + u32_dscdserv_bkofs, 0, REG_0078_P0_DSCD_SERVICES_REG_DSC_HS_SEL);
#endif

    // frl_pll_top_06[15:0]: reg_limit_d5d6d7
    // frl_pll_top_07[7:0]: reg_limit_d5d6d7
    // frl_pll_top_08[15:0]: reg_limit_d5d6d7_rk
    // frl_pll_top_09[7:0]: reg_limit_d5d6d7_rk
    // frl_pll_top_0a[15:0]: reg_limit_lpll_offset
    vIO32WriteFld_1(REG_0018_P0_PLL_TOP + u32_plltop_bkofs, 0x0000, REG_0018_P0_PLL_TOP_REG_LIMIT_D5D6D7_0);
    vIO32WriteFld_1(REG_001C_P0_PLL_TOP + u32_plltop_bkofs, 0x0002, REG_001C_P0_PLL_TOP_REG_LIMIT_D5D6D7_1);
    vIO32WriteFld_1(REG_0020_P0_PLL_TOP + u32_plltop_bkofs, 0x0000, REG_0020_P0_PLL_TOP_REG_LIMIT_D5D6D7_RK_0);
    vIO32WriteFld_1(REG_0024_P0_PLL_TOP + u32_plltop_bkofs, 0x0002, REG_0024_P0_PLL_TOP_REG_LIMIT_D5D6D7_RK_1);
    // init 0x8000 ???
    vIO32WriteFld_1(REG_0028_P0_PLL_TOP + u32_plltop_bkofs, 0x8000, REG_0028_P0_PLL_TOP_REG_LIMIT_LPLL_OFFSET);

    // frl_pll_top_0b[15:0]: 15:12:reg_p_gain_phase, 11:8:reg_i_gain_phase, 7:4:reg_p_gain_prd, 3:0:reg_i_gain_prd
    vIO32WriteFld_1(REG_002C_P0_PLL_TOP + u32_plltop_bkofs, 0xe, REG_002C_P0_PLL_TOP_REG_I_GAIN_PRD);
    vIO32WriteFld_1(REG_002C_P0_PLL_TOP + u32_plltop_bkofs, 0xf, REG_002C_P0_PLL_TOP_REG_P_GAIN_PRD);
    vIO32WriteFld_1(REG_002C_P0_PLL_TOP + u32_plltop_bkofs, 0xe, REG_002C_P0_PLL_TOP_REG_I_GAIN_PHASE);
    vIO32WriteFld_1(REG_002C_P0_PLL_TOP + u32_plltop_bkofs, 0xf, REG_002C_P0_PLL_TOP_REG_P_GAIN_PHASE);

    vIO32WriteFld_1(REG_0034_P0_PLL_TOP + u32_plltop_bkofs, 1, REG_0034_P0_PLL_TOP_REG_FORCE_PRD_STABLE);

    //wriu  0x002113d6  0x0a
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 2, REG_01AC_P0_PLL_TOP_REG_GCR_TMDSPLL_OUTPUT_DIV);
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 1, REG_01AC_P0_PLL_TOP_REG_PD_AUPLL_KPDIV);

    //wriu  0x002113d7  0x00
    vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x00, REG_01AC_P0_PLL_TOP_REG_GCR_AUPLL_KPDIV);

    // frl_pll_top_0c[15:8]: 15:12: reg_ovs_frame_div, 11:8: reg_ivs_frame_div
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x2, REG_0030_P0_PLL_TOP_REG_IVS_FRAME_DIV);
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x2, REG_0030_P0_PLL_TOP_REG_OVS_FRAME_DIV);
    /*
    980 FRL vic 6 1440x480i tmdspll unlock
    hw will automatically adjust kp_div value
    but if sw_freq lower than 22M it will get kp_div value from TMDS_POST_DIV
    TMDS_POST_DIV default initial value is 0x1010[/16] and over tmdspll generating clock ability
    it need to set lowest value of 0x0011[/8]
    */
    vIO32WriteFld_1(REG_01B4_P0_PLL_TOP + u32_plltop_bkofs, 0x3, REG_01B4_P0_PLL_TOP_REG_TMDS_POST_DIV);

#endif

    //adjust phase lock thresh to make phase lock(0F4 -> 1F4)
    // frl_pll_top_05[15:8]: reg_phase_lock_thresh
    // init, 2 -> 1f ???
    vIO32WriteFld_1(REG_0014_P0_PLL_TOP + u32_plltop_bkofs, 0x1f, REG_0014_P0_PLL_TOP_REG_PHASE_LOCK_THRESH);

    // scdc_0D[0] reg_ced_rsed_update_mode (HFR2-22)
    //     1: max condition as pulse
    //     0: max condition as level
    vIO32WriteFld_SCDC(REG_0034_SCDC_P0 + u32PMSCDCBankOffset, 1, FLD_BIT(0));

    //hdmi audio fifo issue
    //reg_cts_fifo_bypass, 1: Bypass
    vIO32WriteFld_1(REG_0014_P0_HDMIRX_PKT_DEP_0 + u32_pkt_bkofs, 0x1, REG_0014_P0_HDMIRX_PKT_DEP_0_REG_CTS_FIFO_BYPASS);

#if (HDMI_CTS_SW_PATCH_CED)
    _KHal_HDMIRx_SetSCDCIRQAddress(enInputPortType, HDMI_SCDC_ADDRESS_50);
#endif

    // _KHal_HDMIRx_SetSCDCInterruptMask(enInputPortType, HDMI_SCDC_INT_FRL_RATE, FALSE);

    // scdc xa8 enable.
    // scdc_31[0]: reg_en_xa8_xiu
    vIO32WriteFld_SCDC(REG_00C4_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_00C4_SCDC_P0_REG_EN_XA8_XIU);

#if(ENABLE_SCDC_INT)
    vIO32WriteFld_1(REG_0020_SCDC_P0 + u32PMSCDCBankOffset, 0, FLD_BIT(13));
    msWriteByteMask(REG_000DA9, 0, _BIT1); // intr_ctrl for HK_R2
#endif
    // move to _KHal_HDMIRx_MACInit
    // unsupported ptk header for frl
    // pkt_dec_11[15:8]: reg_rsv_pkt_type
    // pkt_dec_16[7:0]: reg_support_pkt_header_h
    // vIO32WriteFld_1(REG_0044_P0_HDMIRX_PKT_DEC+u32_pkt_bkofs,0x7f, REG_0044_P0_HDMIRX_PKT_DEC_REG_RSV_PKT_TYPE);
    // vIO32WriteFld_1(REG_0058_P0_HDMIRX_PKT_DEC+u32_pkt_bkofs,0x81, REG_0058_P0_HDMIRX_PKT_DEC_REG_SUPPORT_PKT_HEADER_H);

    // move to MAC_sub_init
    // i det condition mode, vrr_min=40, i mode false alarm.
    // dtop_dec_44[13:12]: reg_src_det_i_md_det_condi_en; Hsync/Vsync align; [12]: check Vblanking+/-1
    // dtop_dep_44[13:12]: reg_src_det_i_md_det_condi_en; Hsync/Vsync align; [12]: check Vblanking+/-1
    // vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0x2, REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_I_MD_DET_CONDI_EN);
    // vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 0x2, REG_0100_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_I_MD_DET_CONDI_EN);

#ifdef CED_RSCC_ENHS_HW_MODE
    // scdc_30[15:12]
    vIO32WriteFld_SCDC(REG_00C0_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_00C0_SCDC_P0_REG_SCDC_ERR_CNT_EN_TIMEOUT);
    vIO32WriteFld_SCDC(REG_00C0_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_00C0_SCDC_P0_REG_SCDC_ERR_READ_PROT_EN);
    vIO32WriteFld_SCDC(REG_00C0_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_00C0_SCDC_P0_REG_SCDC_ERR_CNT_READ_PROT_PRD_SEL);
    vIO32WriteFld_SCDC(REG_00C0_SCDC_P0 + u32PMSCDCBankOffset, 1, REG_00C0_SCDC_P0_REG_SCDC_ERR_CNT_PROT_PRD_56_EN);
#endif

    //bring up new added
    vIO32WriteFld_SCDC(REG_0030_SCDC_P0 + u32PMSCDCBankOffset, 0, REG_0030_SCDC_P0_REG_BIT15);

    //config x74 engine to "without" checking bus free mode
    vIO32WriteFld_SCDC(REG_00D4_SCDC_P0 + u32PMSCDCBankOffset, 0, REG_00D4_SCDC_P0_REG_PM_X74_EN_MODE);
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
void _Hal_tmds_InsertEfuseValue(MS_U8 enInputPortSelect) // MT9701_TBD_ENABLE todo
{
    MS_U8 enSWReadEfuseValue = 0;
    MS_U8 enEfuseValue = 0;
    MS_U8 enAddressOffset = 0;
    MHAL_HDMIRX_MSG_INFO(">>[%s][%d][Port%d]\r\n", __FUNCTION__, __LINE__,enInputPortSelect);
#if 0
    printf("45OHM active bit :0x%x \r\n",(msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_SW_READ) & BIT(5)));
    printf("45OHM trim value :0x%x \r\n",(msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_45) & 0xF0));
    printf("50OHM active bit :0x%x \r\n",(msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_SW_READ) & BIT(6)));
    printf("50OHM trim value :0x%x \r\n",(msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_50) & 0x0F));
    printf(" target threshold select :0x%x \r\n",(msEread_GetDataFromEfuse(0x13A)&BIT(7)));
#endif

    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            enSWReadEfuseValue = (msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_SW_READ) & BIT(5));
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_45) & 0xF0;
            enAddressOffset = HDMI_EFUSE_P0_ADDRESS_OFFSET;
       	break;

        case HDMI_INPUT_PORT1:
            enSWReadEfuseValue = ( msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_SW_READ) & BIT(5));
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_45) & 0xF0;
            enAddressOffset = HDMI_EFUSE_P1_ADDRESS_OFFSET;
        break;

    }

    if(enSWReadEfuseValue)
    {
        msWriteByteMask(REG_DPRX_PHY_PM_03_L + enAddressOffset, enEfuseValue , 0xF0 );
    }

    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            enSWReadEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_SW_READ) & BIT(6);
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_50) & 0x0F;
            enAddressOffset = HDMI_EFUSE_P0_ADDRESS_OFFSET;
        break;

        case HDMI_INPUT_PORT1:
            enSWReadEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_SW_READ) & BIT(6);
            enEfuseValue = msEread_GetDataFromEfuse(HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_50) & 0x0F;
            enAddressOffset = HDMI_EFUSE_P1_ADDRESS_OFFSET;
        break;

    }

    if(enSWReadEfuseValue)
    {
        msWriteByteMask(REG_DPRX_PHY_PM_03_H + enAddressOffset, enEfuseValue , 0x0F );
    }

#if ENABLE_HDMIRX_RTERM_45OHM
    msWriteByteMask(REG_DPRX_PHY_PM_03_H+ enAddressOffset, BIT4, BIT4|BIT5 );  // rterm sel to reg_nodie_rt_ctrl1
#else //50OHM
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
void Hal_tmds_BusyPolling(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_U8 u8SourceVersion = HDMI_SOURCE_VERSION_NOT_SURE;
    MS_U8 u8FRLRate = 0;
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    //MS_U32 u32PHY2P1PMBankOffset = 0;//_Hal_tmds_GetPHYPMBankOffset(enInputPortType);
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
        if (pstHDMIPollingInfo->ucSourceVersion != u8SourceVersion)
        {
            // rst hdmi2.0,1.4 decoder
            Hal_HDMI_Software_Reset(enInputPortType, HDMI_SW_RESET_DVI);
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
        // rst hdmi2.0,1.4 decoder
        Hal_HDMI_Software_Reset(enInputPortType, HDMI_SW_RESET_DVI);
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
void _Hal_tmds_FastTrainingProc(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_BOOL bNextStateFlag = FALSE;
    MS_U16 u16temp = 2000;
    MS_U8 u8EQValue[8] = {0};
    stHDMI_ERROR_STATUS stErrorStatus = {0,0,0,0,0,0,0,0};
    MS_U8 u8Lane = 0;
    MS_U32 u32PHY2P1BankOffset = 0;  //_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
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
                    MS_U8 u8ClockRange = _Hal_tmds_GetH14AutoEQEnable(enInputPortType, Hal_tmds_GetClockRatePort(enInputPortType, pstHDMIPollingInfo->ucSourceVersion));

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
MS_U8 Hal_HDMI_GetFreeSyncInfo(MS_U8 enInputPortSelect, MS_U32 *u32PacketStatus, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)))
{
    MS_U8 u8temp = 0;
    MS_U8 u8PacketData = 0;
    MS_U8 u8PacketContent[6] = {0};
    MS_U8 u8FreeSyncInfo = 0;
    MS_U32 u32PKTStatus = *u32PacketStatus;

    if(u32PKTStatus & HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG)
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

#if 0
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
void _Hal_tmds_SetDEStableFlag(MS_U8 enInputPortSelect, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo, MS_U32 u32PacketStatus, MS_BOOL bStable)
{
    MS_BOOL bVRRFlag = FALSE;
    MS_BOOL bFreeSyncFlag = FALSE;
    MS_U32 u32DTOPBankOffset = 0; //_Hal_tmds_GetDTOPBankOffset(enInputPortType);

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
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_69_L +u32DTOPBankOffset, BIT(2), BIT(2)); // hdmirx_dtop_31[2]: reg_dep_fsm_pause

        if(!(msRead2Byte(REG_HDMIRX_DTOP_DEC_MISC_P0_20_L + u32DTOPBankOffset) & BIT(2))) // hdmirx_dtop_30[5]: reg_de_stable
        {
            msWrite2ByteMask(REG_HDMIRX_DTOP_P0_69_L +u32DTOPBankOffset, 0, BIT(2)); // hdmirx_dtop_31[2]: reg_dep_fsm_pause
        }
    }
    else
    {
        msWrite2ByteMask(REG_HDMIRX_DTOP_P0_69_L +u32DTOPBankOffset, 0, BIT(2)); // hdmirx_dtop_31[2]: reg_dep_fsm_pause
    }
}
#endif
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
void _Hal_tmds_UpdateErrorStatus(MS_U8 enInputPortSelect  __attribute__ ((unused)), ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    //MS_U32 u32DTOPBankOffset = 0;//_Hal_tmds_GetDTOPBankOffset(enInputPortSelect);

    // DE change
    if(_KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_DE_UNSTABLE,enInputPortSelect)) // hdmirx_dtop_pkt_10[12]: reg_depack_dec_irq_status_0 -The number of DE pixel changed
    {
        pstHDMIPollingInfo->bDEChangeFlag = TRUE;

        //msWrite2Byte(REG_HDMIRX_DTOP_PKT_P0_10_L +u32DTOPBankOffset, BIT(12));  // hdmirx_dtop_pkt_10[12]: reg_depack_dec_irq_status_0 -The number of DE pixel changed
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
MS_BOOL _Hal_tmds_GetHDMIModeFlag(MS_U8 enInputPortSelect  __attribute__ ((unused)))
{
    MS_BOOL bHDMIModeFlag = FALSE;
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortType);
    // dvi_hdmi_hdcp status report,
    /*dvi_hdmi_hdcp status report :
    [0] : 1p, 1:HDMI, 0: DVI
    [1] : 8p,  1:HDMI, 0:DVI
    */
    MS_U8 u8_tmp = (MS_U8)u4IO32ReadFld_1(REG_00BC_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset,
        REG_00BC_P0_HDMIRX_DTOP_DEC_MISC_REG_DVI_RPT_STATUS);
    u8_tmp = u8_tmp&BIT(0);//[0]1p:dvi/hdmi[1]8p:dvi/hdmi
    if (u8_tmp)
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
MS_U16 _Hal_tmds_GetHDCP22IntStatus(MS_U8 enInputPortSelect  __attribute__ ((unused)))
{
    MS_U16 u16HDCP22Status = 0;

    u16HDCP22Status = u2IO32Read2B_1(REG_0198_P0_HDCP);

    if(u16HDCP22Status & BIT(1))
    {
        vIO32Write2B_1(REG_0198_P0_HDCP, BIT(1));
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
MS_BOOL _Hal_tmds_GetYUV420Flag(MS_U8 enInputPortSelect, MS_U32 *u32PacketStatus)
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
//MT9701_TBD_ENABLE no used
void _Hal_tmds_YUV420PHYSetting(MS_U8 enInputPortSelect  __attribute__ ((unused)), MS_BOOL bYUV420Flag, MS_U8 u8SourceVersion  __attribute__ ((unused)), MS_U8 u8Colordepth  __attribute__ ((unused)))
{
    MS_U32 u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    if(bYUV420Flag)
    {
        msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L + u32PHY2P1BankOffset, 0, BIT(11)|BMASK(9:8)); //PHY2_2c[11]: disable pixcko md overwrite;PHY2_2c[9:8]:pixcko md set to 8bit mode
        msWrite2ByteMask(REG_PHY2P1_3_P0_07_L + u32PHY2P1BankOffset, BMASK(13:12), BMASK(13:12)); // phy2p1_3_07[12]: reg_en_clko_pix_ov_en; [13]: reg_en_clko_pix_2x_ov_en
        msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L + u32PHY2P1BankOffset, BMASK(5:4), BMASK(5:4)); // phy2p1_2_2E[4]: reg_en_clko_pix_ov; [5]: reg_en_clko_pix_2x_ov
        //msWrite2ByteMask(REG_COMBO_GP_TOP_15_L, BIT(4), BIT(4)); // combo_gp_top_15[4]: reg_pix_clk_div2_en
    }
    else
    {
        msWrite2ByteMask(REG_PHY2P1_2_P0_2C_L + u32PHY2P1BankOffset, 0, BIT(11)|BMASK(9:8)); //PHY2_2c[11]: disable pixcko md overwrite;PHY2_2c[9:8]:pixcko md set to 8bit mode
        msWrite2ByteMask(REG_PHY2P1_3_P0_07_L + u32PHY2P1BankOffset, BIT(12), BMASK(13:12)); // phy2p1_3_07[12]: reg_en_clko_pix_ov_en; [13]: reg_en_clko_pix_2x_ov_en
        msWrite2ByteMask(REG_PHY2P1_2_P0_2E_L + u32PHY2P1BankOffset, BIT(4), BMASK(5:4)); // phy2p1_2_2E[4]: reg_en_clko_pix_ov; [5]: reg_en_clko_pix_2x_ov
        //msWrite2ByteMask(REG_COMBO_GP_TOP_15_L, 0, BIT(4)); // combo_gp_top_15[4]: reg_pix_clk_div2_en
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
MS_U16 _Hal_tmds_GetEMPacketInfo(MS_U8 enInputPortSelect  __attribute__ ((unused)), MS_U32 *u32PacketStatus)
{
    MS_U16 u16EMPacketStatus = 0;
    DWORD u32PKTStatus = *u32PacketStatus;

    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortSelect);
    if (u32PKTStatus & HDMI_STATUS_EM_PACKET_RECEIVE_FLAG) // AVI packet receive
    {
        MS_U16 u16EMPRecValue = u2IO32Read2B_1(REG_0008_P0_HDMI_EMP + u32PortBankOffset) & BMASK(3 : 0);

        if (u2IO32Read2B_1(REG_0004_P0_HDMI_EMP + u32PortBankOffset) & BIT(15))
        {
            u16EMPacketStatus = u16EMPacketStatus | HDMI_EM_PACKET_HDR_ENABLE;
        }

        if (u16EMPRecValue & BIT(0))
        {
            u16EMPacketStatus = u16EMPacketStatus | HDMI_EM_PACKET_DSC_ENABLE;
        }

        if (u16EMPRecValue & BIT(1))
        {
            u16EMPacketStatus = u16EMPacketStatus | HDMI_EM_PACKET_VRR_ENABLE;
        }

        if (u16EMPRecValue & BIT(2))
        {
            u16EMPacketStatus = u16EMPacketStatus | HDMI_EM_PACKET_VENDOR_ENABLE;
        }

        if (u16EMPRecValue & BIT(3))
        {
            u16EMPacketStatus = u16EMPacketStatus | HDMI_EM_PACKET_FVA_ENABLE;
        }
    }

    return u16EMPacketStatus;
}

//**************************************************************************
//  [Function Name]:
//                  _KHal_HDMIRx_RegenTimingProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
static void _KHal_HDMIRx_RegenTimingProc(ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo, MS_U8 enInputPortType , MS_BOOL bIsFreeSync)
{
    UNUSED(enInputPortType);
    ST_HDMI_REGEN_TIMING_INFO stRegenTimingInfo;
    memset(&stRegenTimingInfo, 0, sizeof(ST_HDMI_REGEN_TIMING_INFO));
    ST_HDMI_INTERLACE_INFO st_interlace_info;
    memset(&st_interlace_info, 0, sizeof(ST_HDMI_INTERLACE_INFO));

    MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);
    MS_U32 u32_dtop_dep_bkofs = _KHal_HDMIRx_GetDTOPDEPBankOffset(enInputPortType);
    MS_U32 u32_pkt_dec_bkofs = _KHal_HDMIRx_GetPKTBankOffset(enInputPortType);
    MS_U32 u32_plltop_bkofs = _KHal_HDMIRx_GetPLLTOPBankOffset(enInputPortType);
    MS_U32 u32_dscdservice_bkofs = _KHal_HDMIRx_GetDSCDSERVICEPBankOffset(enInputPortType);
#ifdef FPGA_ENABLE
    MS_U32 u32SynthesizerSet_Mhz = 0;
#else
    MS_U32 u32SynthesizerSet_Mhz = 0;
    MS_U32 u32SynthesizerIn_Mhz_H = 0;
    MS_U32 u32SynthesizerIn_Mhz_L = 0;
#endif
    MS_U32 u32IVSPRDValue = 0;
    MS_U16 u16temp = 0;
    MS_U8 u8_det_i_md = GET_DTOP_DEC_DET_I_MD_EN(u32_dtop_dec_bkofs);
    MS_U8 u8_emp_vrr_en = (_KHal_IsVRR(enInputPortType)==TRUE)?1:0;
    MS_U8 u8_emp_Freesync_en = (bIsFreeSync==TRUE)?1:0;
    MS_U8 u8_pix_rep = (MS_U8)(1 +
                               (u4IO32ReadFld_1(REG_008C_P0_HDMIRX_PKT_DEC + u32_pkt_dec_bkofs,
                                                REG_008C_P0_HDMIRX_PKT_DEC_REG_AVI_PB05) &0xf));
    MS_U16 u16_plltop_lock_status = u4IO32ReadFld_1(REG_00A8_P0_PLL_TOP + u32_plltop_bkofs, FLD_BMSK(8:0));
    MS_U8 u8_timing_shift = 0;
    MS_U32 u32_clk_pix_freq_1p = 0;
    MS_U32 u32_clk_pix_freq = 0;
    MS_U32 digital_div = 1;
    MS_U32 u32_aupll_freq_10k = 0;
    MS_U8 u8_pll_output_div = 0;
    MS_U8 u8_pll_kp_div = 0;
    MS_U32 u32_ivs_div = u4IO32ReadFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, REG_0030_P0_PLL_TOP_REG_IVS_FRAME_DIV);
    MS_U8 ro_tmdspll_lock = 0;
    MS_U8 ro_tmdspll_lock_cnt=0;
    MS_U32 u32_pktdec_bkofs	=_KHal_HDMIRx_GetPKTDECBankOffset(enInputPortType);

#ifdef FPGA_ENABLE
        MS_U16 reg_hdmirx_dec_timing_dep_htt=0;
        MS_U16 clk_pix_freq=0,reg_ivs_prd_value=0,TX_BENCH_PIX_REP_TST=1,input_frame_div_for_frame_sync=0;
#endif
    //MS_U16 reg_hdmirx_dec_timing_dep_hde =GET_DTOP_DEC_HDE(u32_dtop_dec_bkofs);
    //MS_U16 reg_hdmirx_dec_timing_dep_hfront =GET_DTOP_DEC_HFRONT(u32_dtop_dec_bkofs);
#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===_KHal_HDMIRx_RegenTimingProc enInputPortType=%x===\n",enInputPortType);
#endif
    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x1,REG_01A8_P0_PLL_TOP_REG_PD_TMDSPLL);
#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===REG_PD_TMDSPLL is off REG_01A8_P0_PLL_TOP + u32_plltop_bkofs[%x]===\n",REG_01A8_P0_PLL_TOP + u32_plltop_bkofs);
#endif
    MS_U32 u32_ivs_latency_val = 0;
    MS_U32 u32_regen_rptr = 0;
    MS_U32 u32_tmp = 0, u32_tmp2 = 0;

    MS_U16 u16_VIC = 0;


    // Setting rcv_tgen:
    stRegenTimingInfo.u16HTTValue = GET_DTOP_DEC_HTT(u32_dtop_dec_bkofs);
    stRegenTimingInfo.u16HSyncValue = GET_DTOP_DEC_HSYNC(u32_dtop_dec_bkofs);

    stRegenTimingInfo.u16HSyncBackValue =
        GET_DTOP_DEC_HTT(u32_dtop_dec_bkofs) -
        GET_DTOP_DEC_HDE(u32_dtop_dec_bkofs) -
        GET_DTOP_DEC_HFRONT(u32_dtop_dec_bkofs);

    stRegenTimingInfo.u16HDEValue = GET_DTOP_DEC_HDE(u32_dtop_dec_bkofs);

#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===========GET_DTOP_DEC_HTT(u32_dtop_dec_bkofs)=%x\n",GET_DTOP_DEC_HTT(u32_dtop_dec_bkofs));
    MHAL_HDMIRX_MSG_INFO("===========GET_DTOP_DEC_HDE(u32_dtop_dec_bkofs)=%x\n",GET_DTOP_DEC_HDE(u32_dtop_dec_bkofs));
    MHAL_HDMIRX_MSG_INFO("===========GET_DTOP_DEC_HFRONT(u32_dtop_dec_bkofs)=%x\n",GET_DTOP_DEC_HFRONT(u32_dtop_dec_bkofs));
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16HTTValue=%x\n",stRegenTimingInfo.u16HTTValue);
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16HSyncValue=%x\n",stRegenTimingInfo.u16HSyncValue);
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16HSyncBackValue=%x\n",stRegenTimingInfo.u16HSyncBackValue);
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16HDEValue=%x\n",stRegenTimingInfo.u16HDEValue);
#endif
    if(u8_det_i_md)
    {
        stRegenTimingInfo.u16VTTValue = KHal_HDMIRX_Get_Min_VTT(enInputPortType,&st_interlace_info);
    }
    else
        stRegenTimingInfo.u16VTTValue = (MS_U16)GET_DTOP_DEC_VTT(u32_dtop_dec_bkofs);
    stRegenTimingInfo.u16VSyncValue = GET_DTOP_DEC_VSYNC(u32_dtop_dec_bkofs);
    stRegenTimingInfo.u16VSyncBackValue =GET_DTOP_DEC_VSYNC(u32_dtop_dec_bkofs)+GET_DTOP_DEC_VBACK(u32_dtop_dec_bkofs);//(MS_U16)GET_DTOP_DEC_VTT(u32_dtop_dec_bkofs) - GET_DTOP_DEC_VDE(u32_dtop_dec_bkofs) - GET_DTOP_DEC_VFRONT(u32_dtop_dec_bkofs);
    stRegenTimingInfo.u16VDEValue = GET_DTOP_DEC_VDE(u32_dtop_dec_bkofs);
    pstHDMIPollingInfo->u16PreVTT=stRegenTimingInfo.u16VTTValue;
    pstHDMIPollingInfo->u16PreVSB=stRegenTimingInfo.u16VSyncBackValue;



   //AMD VIC39 non-standard issue
     u16_VIC = (MS_U16)u4IO32ReadFld_1(REG_008C_P0_HDMIRX_PKT_DEC + u32_pktdec_bkofs, REG_008C_P0_HDMIRX_PKT_DEC_REG_AVI_PB04);


     if( (u8_det_i_md)  &&  (u16_VIC == 39) && (stRegenTimingInfo.u16VSyncBackValue >= 82) && (stRegenTimingInfo.u16VSyncBackValue <= 83))
     {
         //vIO32WriteFld_1(REG_00BC_P0_DSCD_SERVICES+u32_dscdservice_bkofs , 0x1, REG_00BC_P0_DSCD_SERVICES_REG_RCV_TOP_TMDS_FREQ_MODE);
         stRegenTimingInfo.u16VTTValue = 0x270 ;
         stRegenTimingInfo.u16VSyncBackValue = 0x53;
     }
     else
     {
         vIO32WriteFld_1(REG_00BC_P0_DSCD_SERVICES+u32_dscdservice_bkofs , 0x0, REG_00BC_P0_DSCD_SERVICES_REG_RCV_TOP_TMDS_FREQ_MODE);
     }





#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16VTTValue=%x\n",stRegenTimingInfo.u16VTTValue);
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16VSyncValue=%x\n",stRegenTimingInfo.u16VSyncValue);
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16VSyncBackValue=%x\n",stRegenTimingInfo.u16VSyncBackValue);
    MHAL_HDMIRX_MSG_INFO("===========stRegenTimingInfo.u16VDEValue=%x\n",stRegenTimingInfo.u16VDEValue);
#endif

    if ((stRegenTimingInfo.u16HTTValue == 0) || (stRegenTimingInfo.u16HSyncValue == 0) || (stRegenTimingInfo.u16HSyncBackValue == 0) || (stRegenTimingInfo.u16HDEValue == 0) || (stRegenTimingInfo.u16VTTValue == 0) || (stRegenTimingInfo.u16VSyncValue == 0) || (stRegenTimingInfo.u16VSyncBackValue == 0) || (stRegenTimingInfo.u16VDEValue == 0))
    {
        _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortType);
        MHAL_HDMIRX_MSG_INFO("** HDMI Regen incorrect timing!! \r\n");
        return;
    }

    if ((pstHDMIPollingInfo->u16PreHTT != stRegenTimingInfo.u16HTTValue) || (pstHDMIPollingInfo->u16PreVDE != stRegenTimingInfo.u16VDEValue))
    {
        _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortType);
        pstHDMIPollingInfo->u16PreHTT = stRegenTimingInfo.u16HTTValue;
        pstHDMIPollingInfo->u16PreVDE = stRegenTimingInfo.u16VDEValue;
        MHAL_HDMIRX_MSG_INFO("** HDMI Regen timing change!! \r\n");
        // return;
    }

    //Disable sync_pll:
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0, REG_0030_P0_PLL_TOP_REG_FRAME_LPLL_EN);
    u32IVSPRDValue =
        (u4IO32ReadFld_1(REG_0084_P0_PLL_TOP + u32_plltop_bkofs,
        REG_0084_P0_PLL_TOP_REG_IVS_PRD_VALUE_0) |
         (u4IO32ReadFld_1(REG_0088_P0_PLL_TOP + u32_plltop_bkofs,
         REG_0088_P0_PLL_TOP_REG_IVS_PRD_VALUE_1) << 16));

    if (u32IVSPRDValue == 0)
    {
        pstHDMIPollingInfo->u32TMDSPLLPixelFrequency = 0;
        MHAL_HDMIRX_MSG_ERROR("** HDMI Regen IVS = 0 \r\n");
        return;
    }
    /*
    9U6's PLL_TOP setting is different with M6
    MT9701 follow 9U6
    xtal=12Mhz
    (1/xtal(Mhz))*256(times)=(1/clk_pix_freq_Mhz)*det_cnt
    clk_pix_freq=det_cnt*12/(256) if Legacy(1p)
    clk_pix_freq=det_cnt*12/(256)/8 if FRL(8p)
    */

#define RCV_FIFO_DEPTH 0x400
    u8_timing_shift = 1;
    // ??? init value?
    vIO32WriteFld_1(REG_0058_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 7, REG_0058_P0_DSCD_SERVICES_REG_HREF_SEL);
    // rst tgen here?
   _KHal_HDMIRx_Sw_Rst_rcvtop(enInputPortType, HDMI_RST_RCVTOP_RCV_TOP_SW);


   //if(stRegenTimingInfo.u16VDEValue >= 300)//480i,576i always shhow Scaling issue
   {
        if (1)//(!DBG_IS_SKIP(SKIP_REGEN_FIFO))
        {
            /*
            if(h_p)
            ivs_latency = hfront/2+hsync+hback+hde
            else
            ivs_latency = hfront/2+hback+hde

            fifo_rsv = hfront/2+hsync+hack+hde - RCV_FIFO_MARGIN_KEEP
            */

            u32_tmp = (GET_DTOP_DEC_HFRONT(u32_dtop_dec_bkofs) / 2 + stRegenTimingInfo.u16HDEValue + stRegenTimingInfo.u16HSyncBackValue) / u8_pix_rep;
            u32_tmp2 = (GET_DTOP_DEC_HFRONT(u32_dtop_dec_bkofs) / 2 + stRegenTimingInfo.u16HDEValue + stRegenTimingInfo.u16HSyncBackValue);

            u32_ivs_latency_val = u32_tmp2;
            u32_regen_rptr = (u32_tmp > (RCV_FIFO_DEPTH>>1))?(u32_tmp - (RCV_FIFO_DEPTH>>1)):0;
            //u32_regen_rptr = u32_tmp > RCV_FIFO_MARGIN_KEEP ? (u32_tmp - RCV_FIFO_MARGIN_KEEP) : 0;

            vIO32WriteFld_1(REG_00B8_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                            u32_ivs_latency_val,
                            REG_00B8_P0_DSCD_SERVICES_REG_RCV_TOP_IVS_LATENCY_VAL);

            vIO32WriteFld_1(REG_0060_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                            u32_regen_rptr,
                            REG_0060_P0_DSCD_SERVICES_REG_HREF_FORCE);
        }
    }


#ifdef FPGA_ENABLE
    u32_clk_pix_freq = 144;
#else


    //u32_clk_pix_freq = ((u32_clk_pix_freq_detcnt*10) * 12 / (256 * u8_pix_rep))/10;
    u32_clk_pix_freq =  _KHal_HDMIRx_Calc_Rcv_Gen_Pix_Clk_KHz(
    stRegenTimingInfo.u16HTTValue,
    u32_ivs_div,
    u32IVSPRDValue,
    1,
    u8_pix_rep);

    u32_clk_pix_freq_1p =  _KHal_HDMIRx_Calc_Rcv_Gen_Pix_Clk_KHz(
    stRegenTimingInfo.u16HTTValue,
    u32_ivs_div,
    u32IVSPRDValue,
    1,
    u8_pix_rep);

    u32_clk_pix_freq = (u32_clk_pix_freq+999)/1000;
    u32_clk_pix_freq_1p = (u32_clk_pix_freq_1p+999)/1000;

#endif

    vIO32WriteFld_1(REG_0078_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0, REG_0078_P0_DSCD_SERVICES_REG_DSC_DLY);

    vIO32WriteFld_1(REG_01B8_P0_PLL_TOP + u32_plltop_bkofs, u32_clk_pix_freq_1p, REG_01B8_P0_PLL_TOP_REG_SW_FREQ_SETTING);

#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===========u32_clk_pix_freq=%x\n",u32_clk_pix_freq);
    MHAL_HDMIRX_MSG_INFO("===========u32_clk_pix_freq_1p=%x\n",u32_clk_pix_freq_1p);
#endif
    //MHAL_HDMIRX_MSG_INFO("===========u32_clk_pix_freq_shift_n_100khz=%x\n",u32_clk_pix_freq_shift_n_100khz);
    pstHDMIPollingInfo->u32TMDSPLLPixelFrequency = u32_clk_pix_freq_1p;

    u32_aupll_freq_10k = u32_clk_pix_freq_1p;
    digital_div = 1;
#if REGEN_LOG
    u32_clk_pix_freq_1p = u32_clk_pix_freq_1p;//*1
#endif
    u32_aupll_freq_10k*=100;
#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===========u32_aupll_freq_10k=%x\n",u32_aupll_freq_10k);
#endif
    //kp div, output div
    if (u32_aupll_freq_10k >= AUPLL_CLK_340M)
    {
        u8_pll_output_div = MULPLCATION_2;
        u8_pll_kp_div = MULPLCATION_1;
    }
    else if (u32_aupll_freq_10k >= AUPLL_CLK_170M)
    {
        u8_pll_output_div = MULPLCATION_4;
        u8_pll_kp_div = MULPLCATION_1;
    }
    else if (u32_aupll_freq_10k >= AUPLL_CLK_85M)
    {
        u8_pll_output_div = MULPLCATION_4;
        u8_pll_kp_div = MULPLCATION_2;
    }
    else if (u32_aupll_freq_10k >= AUPLL_CLK_42500K)
    {
        u8_pll_output_div = MULPLCATION_4;
        u8_pll_kp_div = MULPLCATION_4;
    }
    else//if (u32_aupll_freq_10k >= AUPLL_CLK_21250K)
    {
        u8_pll_output_div = MULPLCATION_4;
        u8_pll_kp_div = MULPLCATION_8;
    }

    if(u8_pll_output_div==MULPLCATION_2)
    {
        vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0, REG_01AC_P0_PLL_TOP_REG_GCR_TMDSPLL_OUTPUT_DIV);
    }
    else if(u8_pll_output_div==MULPLCATION_4)
    {
        vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x2, REG_01AC_P0_PLL_TOP_REG_GCR_TMDSPLL_OUTPUT_DIV);
    }
    else//invlaid
    {
        vIO32WriteFld_1(REG_01AC_P0_PLL_TOP + u32_plltop_bkofs, 0x2, REG_01AC_P0_PLL_TOP_REG_GCR_TMDSPLL_OUTPUT_DIV);
    }

    if(u8_pll_kp_div==MULPLCATION_1)
    {
        vIO32WriteFld_1(REG_01BC_P0_PLL_TOP + u32_plltop_bkofs, 0, REG_01BC_P0_PLL_TOP_REG_ATOP_TMDS_POST_DIV_RPT);
    }
    else if(u8_pll_kp_div==MULPLCATION_2)
    {
        vIO32WriteFld_1(REG_01BC_P0_PLL_TOP + u32_plltop_bkofs, 1, REG_01BC_P0_PLL_TOP_REG_ATOP_TMDS_POST_DIV_RPT);
    }
    else if(u8_pll_kp_div==MULPLCATION_4)
    {
        vIO32WriteFld_1(REG_01BC_P0_PLL_TOP + u32_plltop_bkofs, 2, REG_01BC_P0_PLL_TOP_REG_ATOP_TMDS_POST_DIV_RPT);
    }
    else if(u8_pll_kp_div==MULPLCATION_8)
    {
        vIO32WriteFld_1(REG_01BC_P0_PLL_TOP + u32_plltop_bkofs, 3, REG_01BC_P0_PLL_TOP_REG_ATOP_TMDS_POST_DIV_RPT);
    }
    else//invlaid
    {
        vIO32WriteFld_1(REG_01BC_P0_PLL_TOP + u32_plltop_bkofs, 0, REG_01BC_P0_PLL_TOP_REG_ATOP_TMDS_POST_DIV_RPT);
    }


#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===========u8_pll_output_div=%x\n",u8_pll_output_div);
    MHAL_HDMIRX_MSG_INFO("===========u8_pll_kp_div=%x\n",u8_pll_kp_div);
#endif

#ifdef FPGA_ENABLE
    input_frame_div_for_frame_sync=u4IO32ReadFld_1(REG_0030_P0_PLL_TOP, REG_0030_P0_PLL_TOP_REG_IVS_FRAME_DIV);
    reg_hdmirx_dec_timing_dep_htt=u4IO32ReadFld_1(REG_0108_P0_HDMIRX_DTOP_DEP_MISC, REG_0108_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_TIMING_DET_HTT);

    //TX_BENCH_PIX_REP_TST = (self.ReadReg16(self.REG_HDMITX_8P_MISC_P5_BASE, 0x17 << 1) >> 4) & 0xf
    TX_BENCH_PIX_REP_TST=(u2IO32Read2B_1(0x162a2e)>>4)&0xf;//0x162a_17h[7:4],tx
    reg_ivs_prd_value=u4IO32ReadFld_1(REG_0084_P0_PLL_TOP, REG_0084_P0_PLL_TOP_REG_IVS_PRD_VALUE_0);

    //clk_pix_freq = ((input_frame_div_for_frame_sync+1) * 100 *(reg_hdmirx_dec_timing_dep_htt / (TX_BENCH_PIX_REP_TST + 1)) * 12) / reg_ivs_prd_value;
    //synth_set = (60*524288*2 * 100) / (((clk_pix_freq))*5)

    clk_pix_freq = ((input_frame_div_for_frame_sync+1) *(reg_hdmirx_dec_timing_dep_htt / (TX_BENCH_PIX_REP_TST + 1)) * 12) / reg_ivs_prd_value;

    //synth_set = (60*524288*2 * 100) / (((clk_pix_freq))*5)
    //u64SynthesizerSet_Mhz = (FPGA_PLL_TOP_REF_CLK / (clk_pix_freq*5));
    u32SynthesizerSet_Mhz = (FPGA_PLL_TOP_REF_CLK / (clk_pix_freq*5));

    vIO32WriteFld_1(REG_003C_P0_PLL_TOP + u32_plltop_bkofs, (u32SynthesizerSet_Mhz & 0xFFFF), REG_003C_P0_PLL_TOP_REG_PLL_SET_0);
    vIO32WriteFld_1(REG_0040_P0_PLL_TOP + u32_plltop_bkofs, (u32SynthesizerSet_Mhz >> 16), REG_0040_P0_PLL_TOP_REG_PLL_SET_1);
#else
    /*
    [1]syn_in_Mhz = aupll_clk_Mhz * kp_div*output_div /(pfd*cp)*inputdiv
    [2]syn_in_Mhz = aupll_clk_Mhz * kp_div*output_div/8*4
    [3]syn_in_Mhz = aupll_clk_Mhz * kp_div*output_div/8*4=((((u32_aupll_freq_10k>>100)<<(kp_div+output_div))>>3)<<2)
                                                  =u32_aupll_freq_10k>>(100-kp_div-output_div+3-2)
                                                  =u32_aupll_freq_10k>>(101-kp_div-output_div)
    [4]syn_in_100khz = aupll_clk_100khz * kp_div*output_div/8*4=((((u32_aupll_freq_10k>>10)<<(kp_div+output_div))>>3)<<2)
                                                  =u32_aupll_freq_10k>>(CLK_CNT_SHIFT_N-kp_div-output_div+3-2)
                                                  =u32_aupll_freq_10k>>(CLK_CNT_SHIFT_N-kp_div-output_div+1)[CLK_CNT_SHIFT_N=10]

    [5]synset_Mhz=(ref_clk*pow(2,19))/(syn_in_100khz>>10)
            =(432*524288)/(syn_in_100khz>>10)
            =((432*524288)/syn_in_100khz)>>CLK_CNT_SHIFT_N

    */
    u32SynthesizerIn_Mhz_H =  PLL_TOP_REF_CLK<<MULTIPLE_SHIT_FACTOR;
    u32SynthesizerIn_Mhz_L= ((u32_clk_pix_freq/digital_div)<<MULTIPLE_SHIT_FACTOR)*u8_pll_kp_div*u8_pll_output_div/8*1;

    u32SynthesizerSet_Mhz = u32SynthesizerIn_Mhz_H/u32SynthesizerIn_Mhz_L;//synset_Mhz=(ref_clk*pow(2,19))/(syn_in_100khz>>10)

#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===========digital_div=%x\n",digital_div);
    MHAL_HDMIRX_MSG_INFO("===========(u32_clk_pix_freq_1p<<MULTIPLE_SHIT_FACTOR)*u8_pll_kp_div*u8_pll_output_div/8*1=%x\n",(u32_clk_pix_freq_1p<<MULTIPLE_SHIT_FACTOR)*u8_pll_kp_div*u8_pll_output_div/8*1);
    MHAL_HDMIRX_MSG_INFO("===========((u32_clk_pix_freq/digital_div)<<MULTIPLE_SHIT_FACTOR)*u8_pll_kp_div*u8_pll_output_div/8*1=%x\n",((u32_clk_pix_freq/digital_div)<<MULTIPLE_SHIT_FACTOR)*u8_pll_kp_div*u8_pll_output_div/8*1);
    MHAL_HDMIRX_MSG_INFO("===========u32_clk_pix_freq=%x\n",u32_clk_pix_freq);
    MHAL_HDMIRX_MSG_INFO("===========u32SynthesizerIn_Mhz_H=%x\n",u32SynthesizerIn_Mhz_H);
    MHAL_HDMIRX_MSG_INFO("===========u32SynthesizerIn_Mhz_L=%x\n",u32SynthesizerIn_Mhz_L);
    MHAL_HDMIRX_MSG_INFO("===========u32SynthesizerSet_Mhz=%x\n",u32SynthesizerSet_Mhz);
    MHAL_HDMIRX_MSG_INFO("===========u32_plltop_bkofs=%x\n",u32_plltop_bkofs);
#endif
    vIO32WriteFld_1(REG_003C_P0_PLL_TOP + u32_plltop_bkofs, (u32SynthesizerSet_Mhz & 0xFFFF), REG_003C_P0_PLL_TOP_REG_PLL_SET_0);
    vIO32WriteFld_1(REG_0040_P0_PLL_TOP + u32_plltop_bkofs, (u32SynthesizerSet_Mhz >> 16), REG_0040_P0_PLL_TOP_REG_PLL_SET_1);
#endif

    MsOS_DelayTask(1);

#ifdef FPGA_ENABLE
    vIO32WriteFld_1(REG_0070_P0_DSCD_SERVICES + u32_dscdservice_bkofs, u8_vrr_en|u8_det_i_md, REG_0070_P0_DSCD_SERVICES_REG_TGEN_DE_SFT_EN);
    vIO32WriteFld_1(REG_0070_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, REG_0070_P0_DSCD_SERVICES_REG_TGEN_DE_SFT_PROT_OPT);
    vIO32WriteFld_1(REG_0070_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, REG_0070_P0_DSCD_SERVICES_REG_TGEN_DE_SFT_EN);
    vIO32WriteFld_1(REG_0070_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, REG_0070_P0_DSCD_SERVICES_REG_TGEN_DE_SFT_PROT_OPT);
#endif
    vIO32WriteFld_1(REG_0034_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0x1f, REG_0034_P0_DSCD_SERVICES_REG_SN_LEAD_CFG);


    // htt
    vIO32WriteFld_1(REG_0080_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                    stRegenTimingInfo.u16HTTValue / u8_pix_rep - 1, REG_0080_P0_DSCD_SERVICES_REG_TG_HTT);
    // hsync
    vIO32WriteFld_1(REG_0084_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                    stRegenTimingInfo.u16HSyncValue / u8_pix_rep - u8_timing_shift,
                    REG_0084_P0_DSCD_SERVICES_REG_TG_HS_ED);
    // hsync+back
    vIO32WriteFld_1(REG_0088_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                    stRegenTimingInfo.u16HSyncBackValue / u8_pix_rep - 1, REG_0088_P0_DSCD_SERVICES_REG_TG_H_BP_ED);
    // hde
    vIO32WriteFld_1(REG_008C_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                    stRegenTimingInfo.u16HDEValue / u8_pix_rep, REG_008C_P0_DSCD_SERVICES_REG_TG_H_DE);

#if REGEN_LOG
            MHAL_HDMIRX_MSG_INFO("===========REG_0080_P0_DSCD_SERVICES_REG_TG_HTT=%x\n",u4IO32ReadFld_1(REG_0080_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_0080_P0_DSCD_SERVICES_REG_TG_HTT));
            MHAL_HDMIRX_MSG_INFO("===========REG_0084_P0_DSCD_SERVICES_REG_TG_HS_ED=%x\n",u4IO32ReadFld_1(REG_0084_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_0084_P0_DSCD_SERVICES_REG_TG_HS_ED));
            MHAL_HDMIRX_MSG_INFO("===========REG_0088_P0_DSCD_SERVICES_REG_TG_H_BP_ED=%x\n",u4IO32ReadFld_1(REG_0088_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_0088_P0_DSCD_SERVICES_REG_TG_H_BP_ED));
            MHAL_HDMIRX_MSG_INFO("===========REG_008C_P0_DSCD_SERVICES_REG_TG_H_DE=%x\n",u4IO32ReadFld_1(REG_008C_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_008C_P0_DSCD_SERVICES_REG_TG_H_DE));
#endif
    if (u8_emp_vrr_en || u8_emp_Freesync_en)
    {
        //disable i_md det?
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 0, REG_0100_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_I_MD_DET_CONDI_EN);
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 0, REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_I_MD_DET_CONDI_EN);
        // vtt
        vIO32WriteFld_1(REG_0090_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0x1fff, REG_0090_P0_DSCD_SERVICES_REG_TG_VTT);
    }
    else
    {
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEP_MISC + u32_dtop_dep_bkofs, 2, REG_0100_P0_HDMIRX_DTOP_DEP_MISC_REG_HDMIRX_DEP_I_MD_DET_CONDI_EN);
        vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP_DEC_MISC + u32_dtop_dec_bkofs, 2, REG_0100_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DEC_I_MD_DET_CONDI_EN);
        // vtt
#if REGEN_LOG
        MHAL_HDMIRX_MSG_INFO("===u8_det_i_md=%x===\n",u8_det_i_md);
        MHAL_HDMIRX_MSG_INFO("===stRegenTimingInfo.u16VTTValue=%x===\n",stRegenTimingInfo.u16VTTValue);
#endif
        vIO32WriteFld_1(REG_0090_P0_DSCD_SERVICES + u32_dscdservice_bkofs,
                            stRegenTimingInfo.u16VTTValue - 1, REG_0090_P0_DSCD_SERVICES_REG_TG_VTT);
    }

    // vsync
    vIO32WriteFld_1(REG_0094_P0_DSCD_SERVICES + u32_dscdservice_bkofs, stRegenTimingInfo.u16VSyncValue - 1, REG_0094_P0_DSCD_SERVICES_REG_TG_VS_ED);
    // vsync+back
    vIO32WriteFld_1(REG_0098_P0_DSCD_SERVICES + u32_dscdservice_bkofs, stRegenTimingInfo.u16VSyncBackValue-1, REG_0098_P0_DSCD_SERVICES_REG_TG_V_BP_ED);

    // vde
    vIO32WriteFld_1(REG_009C_P0_DSCD_SERVICES + u32_dscdservice_bkofs, stRegenTimingInfo.u16VDEValue, REG_009C_P0_DSCD_SERVICES_REG_TG_V_DE);
#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===========REG_0090_P0_DSCD_SERVICES_REG_TG_VTT=%x\n",u4IO32ReadFld_1(REG_0090_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_0090_P0_DSCD_SERVICES_REG_TG_VTT));
    MHAL_HDMIRX_MSG_INFO("===========REG_0094_P0_DSCD_SERVICES_REG_TG_VS_ED=%x\n",u4IO32ReadFld_1(REG_0094_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_0094_P0_DSCD_SERVICES_REG_TG_VS_ED));
    MHAL_HDMIRX_MSG_INFO("===========REG_0098_P0_DSCD_SERVICES_REG_TG_V_BP_ED=%x\n",u4IO32ReadFld_1(REG_0098_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_0098_P0_DSCD_SERVICES_REG_TG_V_BP_ED));
    MHAL_HDMIRX_MSG_INFO("===========REG_009C_P0_DSCD_SERVICES_REG_TG_V_DE=%x\n",u4IO32ReadFld_1(REG_009C_P0_DSCD_SERVICES + u32_dscdservice_bkofs, REG_009C_P0_DSCD_SERVICES_REG_TG_V_DE));
#endif

    if(stRegenTimingInfo.u16VDEValue < 300)//480i,576i always shhow Scaling issue
    {
       vIO32WriteFld_1(REG_0018_P0_PLL_TOP + u32_plltop_bkofs, 0x0000, REG_0018_P0_PLL_TOP_REG_LIMIT_D5D6D7_0);
       //vIO32WriteFld_1(REG_001C_P0_PLL_TOP + u32_plltop_bkofs, 0x0002, REG_001C_P0_PLL_TOP_REG_LIMIT_D5D6D7_1);
       vIO32WriteFld_1(REG_001C_P0_PLL_TOP + u32_plltop_bkofs, 0x0010, REG_001C_P0_PLL_TOP_REG_LIMIT_D5D6D7_1);
    }
    else//initial script value
    {
        vIO32WriteFld_1(REG_0018_P0_PLL_TOP + u32_plltop_bkofs, 0x0000, REG_0018_P0_PLL_TOP_REG_LIMIT_D5D6D7_0);
        vIO32WriteFld_1(REG_001C_P0_PLL_TOP + u32_plltop_bkofs, 0x0002, REG_001C_P0_PLL_TOP_REG_LIMIT_D5D6D7_1);
        //vIO32WriteFld_1(REG_001C_P0_PLL_TOP + u32_plltop_bkofs, 0x0010, REG_001C_P0_PLL_TOP_REG_LIMIT_D5D6D7_1);
    }



    // set tgen ready
    vIO32WriteFld_1(REG_0050_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0, REG_0050_P0_DSCD_SERVICES_REG_TGEN_SET_READY);
    vIO32WriteFld_1(REG_0050_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, REG_0050_P0_DSCD_SERVICES_REG_TGEN_SET_READY);

    // tgen enable
    vIO32WriteFld_1(REG_004C_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0, REG_004C_P0_DSCD_SERVICES_REG_TGEN_EN);
    vIO32WriteFld_1(REG_004C_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, REG_004C_P0_DSCD_SERVICES_REG_TGEN_EN);

    vIO32WriteFld_1(REG_0034_P0_PLL_TOP + u32_plltop_bkofs, 0x1, REG_0034_P0_PLL_TOP_REG_FORCE_PRD_STABLE);

    //From M6
    // 00[5], reset Tgen
    //vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, FLD_BIT(5)); // reset Tgen
    vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, FLD_BIT(1)); // reset Tgen
    vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 1, FLD_BIT(6)); // reset Tgen

    u16temp = 200;
    while (u16temp--)
        ; // delay 1us
    // 00[5], reset Tgen
    //vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0, FLD_BIT(5)); // reset Tgen
    vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0, FLD_BIT(1)); // reset Tgen
    vIO32WriteFld_1(REG_0000_P0_DSCD_SERVICES + u32_dscdservice_bkofs, 0, FLD_BIT(6)); // reset Tgen

    //mdelay(1);                                                                       // delay 1ms(1);                                                                       // delay 1ms

    vIO32WriteFld_1(REG_01A8_P0_PLL_TOP + u32_plltop_bkofs, 0x0,REG_01A8_P0_PLL_TOP_REG_PD_TMDSPLL);
#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("======REG_PD_TMDSPLL is on===\n");
#endif
    while(ro_tmdspll_lock==0)
    {
        ro_tmdspll_lock=u4IO32ReadFld_1(REG_01B4_P0_PLL_TOP + u32_plltop_bkofs, REG_01B4_P0_PLL_TOP_REG_RO_TMDSPLL_LOCK);
#if REGEN_LOG
        MHAL_HDMIRX_MSG_INFO("===ro_tmdspll_lock_cnt is [%d]===\n",ro_tmdspll_lock_cnt);
#endif
        ro_tmdspll_lock_cnt++;
        if(ro_tmdspll_lock_cnt>=TMDSPLL_LOCK_MAX_CNT)
        {
            ro_tmdspll_lock_cnt=0;
            break;
        }
    }

#if REGEN_LOG
    MHAL_HDMIRX_MSG_INFO("===ro_tmdspll_lock is  %d===\n",ro_tmdspll_lock);
#endif
    vIO32WriteFld_1(REG_0030_P0_PLL_TOP + u32_plltop_bkofs, 0x1,REG_0030_P0_PLL_TOP_REG_FRAME_LPLL_EN);


    pstHDMIPollingInfo->u8RegenFsm = E_REGEN_CONFIG;



    if (_KHal_HDMIRx_Pll_TOP_Stable(enInputPortType, PLLTOP_IVS_OVS_TOLERANCE_REGEN_DONE)
        || pstHDMIPollingInfo->u8RegenCount > 10)
    {
        pstHDMIPollingInfo->u8RegenFsm = E_REGEN_DONE;
        #if 0//MT9701_TBD_ENABLE todo
        _KHal_HDMIRx_IRQ_CLR(HDMI_IRQ_PHY, enInputPortType, BIT0);
        KHAL_HDMIRx_PHY_IRQ_MASK_CONTROL(enInputPortType, TRUE);
        #endif
        MHAL_HDMIRX_MSG_INFO("** [1]HDMI Regen done,(%d),%x,%d(MHz),%d(KHz),%x, HTT/HDE/VTT/VDE is %d/%d/%d/%d\r\n",
            pstHDMIPollingInfo->u8RegenCount,
            u16_plltop_lock_status,
            u32_clk_pix_freq,
            u32_clk_pix_freq,
            u32SynthesizerSet_Mhz,
            stRegenTimingInfo.u16HTTValue,
            stRegenTimingInfo.u16HDEValue,
            stRegenTimingInfo.u16VTTValue,
            stRegenTimingInfo.u16VDEValue
            );
    }
    else
    {
        pstHDMIPollingInfo->u8RegenCount++;

        MHAL_HDMIRX_MSG_INFO("** [0]HDMI Regen,(%d),%x,%d(MHz),%d(KHz),%x, HTT/HDE/VTT/VDE is %d/%d/%d/%d\r\n",
            pstHDMIPollingInfo->u8RegenCount,
            u16_plltop_lock_status,
            u32_clk_pix_freq,
            u32_clk_pix_freq,
            u32SynthesizerSet_Mhz,
            stRegenTimingInfo.u16HTTValue,
            stRegenTimingInfo.u16HDEValue,
            stRegenTimingInfo.u16VTTValue,
            stRegenTimingInfo.u16VDEValue
            );
    }
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
void _Hal_tmds_SignalDetectProc(MS_U8 enInputPortSelect, MS_U32 *u32PacketStatus, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_BOOL bNextStateFlag = FALSE;
    MS_U8 u8SignalDetectState = HDMI_SIGNAL_DETECT_SQUELCH;
    MS_BOOL b_hde_chg = FALSE;
    MS_BOOL b_vde_chg = FALSE;
    MS_BOOL b_htt_chg = FALSE;
    MS_BOOL b_vtt_chg = FALSE;
    MS_BOOL b_vback_chg = FALSE;
    MS_BOOL bTimingChgStatus = FALSE;
    MS_BOOL b_vtt_diff = FALSE;
    MS_BOOL b_vsb_diff = FALSE;
    MS_BOOL b_freesync_chg = FALSE;
    static MS_U8 u8_hde_chg_count = 0;
    //MS_U32 u32PHY2P1BankOffset = 0; // _Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    MS_U8 u8Colordepth;
    //MS_U32 u32_dtop_dec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortSelect);

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
                        pstHDMIPollingInfo->u8FrameRateCnt = 0;
                        pstHDMIPollingInfo->u8ActiveCableState = HDMI_ACTIVE_CABLE_START;
                        _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);

                        HDMI_HAL_DPRINTF("** HDMI clock stable port %d\r\n", enInputPortSelect);

                        if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20)
                        {
                        	HDMI_HAL_DPRINTF("** HDMI get version 2.0 port %d\r\n", enInputPortSelect);

                        	pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_ACTIVE_CABLE;
                        	pstHDMIPollingInfo->u8RetriggerCount20 = 0;
                        	pstHDMIPollingInfo->bHDMI20Flag = TRUE;
                            _KHal_HDMIRx_GetimingDetectPath(enInputPortSelect,HDMI_TIMING_DET_DTOPDEC_DC_AUTO, HDMI_SOURCE_VERSION_HDMI20);
                            _KHal_HDMIRx_GetimingDetectPath_dtopdep(enInputPortSelect, HDMI_TIMING_DET_DTOPDEP_AUTO, HDMI_SOURCE_VERSION_HDMI20);
                            _KHal_HDMIRx_GetimingDetectPath_dtopmisc(enInputPortSelect, HDMI_TIMING_DET_RCV_TOP, HDMI_SOURCE_VERSION_HDMI20);
                        }
                        else if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
                        {
                        	HDMI_HAL_DPRINTF("** HDMI get version 1.4 port %d\r\n", enInputPortSelect);

                        	pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_CHECK_AUTO_EQ;
                        	pstHDMIPollingInfo->u8TryNegativeC = 0;
                            pstHDMIPollingInfo->u8TryPhaseDecI = 0;
                        	pstHDMIPollingInfo->bHDMI20Flag = FALSE;
                            _KHal_HDMIRx_GetimingDetectPath(enInputPortSelect,HDMI_TIMING_DET_DTOPDEC_DC_AUTO, HDMI_SOURCE_VERSION_HDMI14);
                            _KHal_HDMIRx_GetimingDetectPath_dtopdep(enInputPortSelect, HDMI_TIMING_DET_DTOPDEP_AUTO, HDMI_SOURCE_VERSION_HDMI14);
                            _KHal_HDMIRx_GetimingDetectPath_dtopmisc(enInputPortSelect, HDMI_TIMING_DET_RCV_TOP, HDMI_SOURCE_VERSION_HDMI14);
                        }
                        Hal_HDMI_Software_Reset(enInputPortSelect, HDMI_SW_RESET_AUDIO_FIFO);
                        if(_Hal_tmds_GetClockChangeFlag(enInputPortSelect, pstHDMIPollingInfo))
                        {
                        	// clear clock big change
                        }
                    }

                    bNextStateFlag = TRUE;
                }
                else
                {
                    if(!pstHDMIPollingInfo->bNoInputFlag) //Stable to unstable
                    {
                        pstHDMIPollingInfo->bNoInputFlag = TRUE;
                        pstHDMIPollingInfo->bDEChangeFlag = TRUE;
                        pstHDMIPollingInfo->u8_prev_repetition = 0;
                        pstHDMIPollingInfo->u8RegenFsm = E_REGEN_INIT;
                        //_Hal_tmds_ClearEDRVaildFlag(pstHDMIPollingInfo->ucHDMIInfoSource);

                        // hdmirx_dtop_35[15] = 1 reg_dep_de_det_vsync_en (default setting, fix)
                        //_Hal_tmds_SetDEStableFlag(enInputPortSelect, pstHDMIPollingInfo, &u32PacketStatus, FALSE);
                        //Hal_HDCP_GetAuthVersion(enInputPortType, FALSE); // Clear HDCP22 interrupt status
                        _Hal_tmds_GetHDCP22IntStatus(enInputPortSelect);    // Clear HDCP22 interrupt status
                        pstHDMIPollingInfo->ucHDCPState = HDMI_HDCP_NO_ENCRYPTION;
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
                        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_3E_L, 0x0005, 0xFFFF); //dvi_dtop_p0_3E: dtop module sw reset
                        //msWrite2ByteMask(REG_HDMIRX_DTOP_P0_3E_L, 0, 0xFFFF); // dvi_dtop_p0_3E: dtop module sw reset
                        _KHal_HDMIRx_Sw_Rst_dtopdec(enInputPortSelect, HDMI_RST_DTOPDEC_TIMING_DET);
                        _KHal_HDMIRx_Sw_Rst_dtopdep(enInputPortSelect, HDMI_RST_DTOPDEP_TIMING_DET);
                        _KHal_HDMIRx_Sw_Rst_dtopmisc(enInputPortSelect, HDMI_RST_DTOPMISC_TIMING_DET);
                        Hal_HDMI_pkt_reset(enInputPortSelect,REST_HDMI_STATUS, pstHDMIPollingInfo,TRUE);
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
                    // from odinson2
                    Hal_HDMI_Software_Reset(enInputPortSelect, HDMI_SW_RESET_HDCP);
                    _KHal_HDMIRx_Sw_Rst_dtopdec(enInputPortSelect, HDMI_RST_DTOPDEC_TIMING_DET);
                    _KHal_HDMIRx_Sw_Rst_dtopdep(enInputPortSelect, HDMI_RST_DTOPDEP_TIMING_DET);
                    _KHal_HDMIRx_Sw_Rst_dtopmisc(enInputPortSelect, HDMI_RST_DTOPMISC_TIMING_DET);
                    Hal_HDMI_pkt_reset(enInputPortSelect,REST_HDMI_STATUS, pstHDMIPollingInfo,TRUE);
                    _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);
                    // from odinson2 end

                    HDMI_HAL_DPRINTF("** HDMI clock changed, switch to no input port %d\r\n", enInputPortSelect);

                    pstHDMIPollingInfo->u8FastTrainingState = HDMI_FAST_TRAINING_START;
                    HDMI_HAL_DPRINTF("** HDMI clock change trigger FT port %d\r\n", enInputPortSelect);
                    bNextStateFlag = FALSE;
                }
                else
                {
                    bNextStateFlag = TRUE;
                }
                break;

            case HDMI_SIGNAL_DETECT_DATA_ENABLE:
                if(Hal_HDMI_GetDEStableStatus(enInputPortSelect)&&GET_DTOP_DEC_DET_TIMING_STABLE())
                {
                    //Add de stable flag
                    pstHDMIPollingInfo->bDEStableFlag = TRUE;
                    if(!pstHDMIPollingInfo->bResetDCFifoFlag)
                    {
                        pstHDMIPollingInfo->u32_dc_fifo = MsOS_GetSystemTime();
                        Hal_HDMI_pkt_reset(enInputPortSelect, REST_DEEP_COLOR_FIFO,pstHDMIPollingInfo,TRUE);
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
                        Hal_HDMI_pkt_reset(enInputPortSelect, REST_AVMUTE, pstHDMIPollingInfo,TRUE);
                        pstHDMIPollingInfo->bTimingStableFlag = TRUE;
                    }
                    _Hal_tmds_UpdateErrorStatus(enInputPortSelect, pstHDMIPollingInfo);

                    b_hde_chg = _KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_HDE_CHG, enInputPortSelect);
                    b_vde_chg = _KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_VDE_CHG, enInputPortSelect);

                    if ((b_hde_chg || b_vde_chg))
                    {
                        _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);
                        bNextStateFlag = FALSE;
                    }
                    else
                    {
                        bNextStateFlag = TRUE;
                    }
                }
                else // DE unstable
                {
                    // hdmirx_dtop_35[15] = 1 reg_dep_de_det_vsync_en (default setting, fix)
                    //_Hal_tmds_SetDEStableFlag(enInputPortType, pstHDMIPollingInfo, u32PacketStatus, FALSE);
                    pstHDMIPollingInfo->bDEStableFlag = FALSE;
                    pstHDMIPollingInfo->u8_prev_repetition = 0;

                    if(pstHDMIPollingInfo->bYUV420Flag)
                    {
                        pstHDMIPollingInfo->bYUV420Flag = FALSE;

                        //_Hal_tmds_YUV420PHYSetting(enInputPortSelect, pstHDMIPollingInfo->bYUV420Flag, pstHDMIPollingInfo->ucSourceVersion, pstHDMIPollingInfo->u8Colordepth);

                        HDMI_HAL_DPRINTF("** HDMI back to HDMI 2.0 port %d\r\n", enInputPortSelect);
                    }
                    if (pstHDMIPollingInfo->bTimingStableFlag)
                    {
                        pstHDMIPollingInfo->bTimingStableFlag = FALSE;
                        Hal_HDMI_pkt_reset(enInputPortSelect,REST_HDMI_STATUS, pstHDMIPollingInfo,TRUE);
                        _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);

                        MHAL_HDMIRX_MSG_INFO("** HDMI PKT RST REST_HDMI_STATUS port %d\r\n", enInputPortSelect);

                    }
                    bNextStateFlag = FALSE;
                }

                b_hde_chg = _KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_HDE_CHG, enInputPortSelect);

                if(b_hde_chg) //debounce: check 5 times continuously.
                {
                    if(u8_hde_chg_count < HDE_CHANGE_Count) //avoid u8_hde_chg_count becomes too large.
                    {
                        u8_hde_chg_count ++;
                    }
                }
                else
                {
                    u8_hde_chg_count = 0;
                }

                if (u8_hde_chg_count == HDE_CHANGE_Count)
                {
                    if (MsOS_Timer_DiffTime(MsOS_GetSystemTime(),
                                            pstHDMIPollingInfo->u32_dc_fifo) >= DC_RST_MINI_INTERVAL_MS)
                    {
                        pstHDMIPollingInfo->u32_dc_fifo = MsOS_GetSystemTime();
                        Hal_HDMI_pkt_reset(enInputPortSelect,REST_DEEP_COLOR_FIFO, pstHDMIPollingInfo,TRUE);
                    }

                    if (_KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_HDE_CHG, enInputPortSelect))
                    {
                        // do nothing for coverity
                    }
                    _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);
                    u8_hde_chg_count = 0;
                }
                break;

            case HDMI_SIGNAL_DETECT_HDMI_MODE:
                if(_Hal_tmds_GetHDMIModeFlag(enInputPortSelect))
                {
                    if(!pstHDMIPollingInfo->bHDMIModeFlag)
                    {
                        pstHDMIPollingInfo->bHDMIModeFlag = TRUE;
                        MHAL_HDMIRX_MSG_INFO("** HDMI detect HDMI mode, port %d\r\n", enInputPortSelect);
                    }
                }
                else if(pstHDMIPollingInfo->bHDMIModeFlag)
                {
                    pstHDMIPollingInfo->bHDMIModeFlag = FALSE;
                    Hal_HDMI_pkt_reset(enInputPortSelect, REST_AVMUTE|REST_HDMI_STATUS,pstHDMIPollingInfo,TRUE);
                    MHAL_HDMIRX_MSG_INFO("** HDMI detect DVI mode, port %d\r\n", enInputPortSelect);
                }

                bNextStateFlag = TRUE;

                break;

            case HDMI_SIGNAL_DETECT_HDCP_STATUS:
                if(Hal_HDCP_getstatus(enInputPortSelect) & BIT(8)) // AKSV interrupt status: HDCP1.4
                {
                    Hal_HDCP_ClearStatus(enInputPortSelect, BIT(0));

                    pstHDMIPollingInfo->ucHDCPState = HDMI_HDCP_1_4;

                    MHAL_HDMIRX_MSG_INFO("** HDMI HDCP state 1.4, port %d\r\n", enInputPortSelect);
                }
                else if(_Hal_tmds_GetHDCP22IntStatus(enInputPortSelect) & BIT(1)) // AKE_Init interrupt status: HDCP2.2
                {
                    pstHDMIPollingInfo->ucHDCPState = HDMI_HDCP_2_2;

                    MHAL_HDMIRX_MSG_INFO("** HDMI HDCP state 2.2, port %d\r\n", enInputPortSelect);
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
            case HDMI_SIGNAL_DETECT_REGEN_TIMING:
                {
                    //MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortSelect);
                    MS_U32 u32_dtopdec_bkofs = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortSelect);
                    MS_U32 u32_dtopdep_bkofs = _KHal_HDMIRx_GetDTOPDEPBankOffset(enInputPortSelect);
                    MS_U32 u32_pkt_dec_bkofs = _KHal_HDMIRx_GetPKTBankOffset(enInputPortSelect);
                    MS_BOOL b_I_mode = FALSE;
                    MS_BOOL b_dec_I_mode = GET_DTOP_DEC_DET_I_MD_EN(u32_dtopdec_bkofs);
                    MS_BOOL b_dep_I_mode = GET_DTOP_DEP_DET_I_MD_EN(u32_dtopdep_bkofs);
                    ST_HDMI_INTERLACE_INFO st_interlace_info;
                    memset(&st_interlace_info, 0, sizeof(ST_HDMI_INTERLACE_INFO));
                    MS_U8 u8_IsVRR = _KHal_IsVRR(enInputPortSelect);
                    MS_BOOL bIsFreeSync = FALSE;
                    MS_U8 u8_pix_rep = (MS_U8)(1 + (u4IO32ReadFld_1(REG_008C_P0_HDMIRX_PKT_DEC + u32_pkt_dec_bkofs,
                                                REG_008C_P0_HDMIRX_PKT_DEC_REG_AVI_PB05) &0xf));
                    MS_U8 u8_debounce_cnt = u8_pix_rep>1?PLLTOP_IVS_OVS_TOLERANCE_REGEN_REDO_DEBOUNCE_CNT_REPITION:
                        PLLTOP_IVS_OVS_TOLERANCE_REGEN_REDO_DEBOUNCE_CNT;
                    KHAL_HDMIRX_Get_Interlace_Information(enInputPortSelect,&st_interlace_info);
                    //fix for gamming vrr  wrong i mode
                    CLR_DTOP_DEC_OI_MODE();
                    b_vde_chg = _KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_VDE_CHG, enInputPortSelect);
                    b_htt_chg = _KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_HTT_CHG, enInputPortSelect);
                    b_vtt_chg = _KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_VTT_CHG, enInputPortSelect);
                    b_vback_chg = _KHal_HDMIRx_GetTimingChangeStatus(HDMI_TIMING_CHG_DTOPDEC_VBACK_CHG, enInputPortSelect);

                    b_vde_chg|=KHAL_HDMIRx_VDEChange(enInputPortSelect,pstHDMIPollingInfo);
                    b_htt_chg|=KHAL_HDMIRx_HTTChange(enInputPortSelect,pstHDMIPollingInfo);
                    b_vtt_chg|=KHAL_HDMIRx_VTTChange(enInputPortSelect,pstHDMIPollingInfo);
                    b_vtt_diff|=KHAL_HDMIRx_VTTDiff(enInputPortSelect,pstHDMIPollingInfo);
                    b_vsb_diff|=KHAL_HDMIRx_VSBDiff(enInputPortSelect,pstHDMIPollingInfo);

                    if (Hal_HDMI_GetFreeSyncInfo(enInputPortSelect, u32PacketStatus, NULL) & BIT(2))
                    {
                        bIsFreeSync = TRUE;
                    }

                    if (pstHDMIPollingInfo->bPreFreeSync!= bIsFreeSync)
                    {
                        pstHDMIPollingInfo->bPreFreeSync = bIsFreeSync;
                        b_freesync_chg = TRUE;
                    }

                    if(bIsFreeSync || u8_IsVRR)
                    {
                        b_I_mode = b_dep_I_mode;
                    }
                    else
                    {
                        b_I_mode = b_dec_I_mode;
                    }

                    if (pstHDMIPollingInfo->bPreInterlace != b_I_mode)
                    {
                        MHAL_HDMIRX_MSG_INFO("** HDMI Regen Interlace flag change port %d,%d,%d , b_dep_I_mode :%d\r\n",
                                             (enInputPortSelect), pstHDMIPollingInfo->bPreInterlace, b_dec_I_mode,b_dep_I_mode);
                        pstHDMIPollingInfo->bPreInterlace = b_I_mode;
                        _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);
                    }

                    if (!GET_DTOP_DEP_DET_TIMING_STABLE(u32_dtopdep_bkofs))
                    {
                        MHAL_HDMIRX_MSG_INFO("2.do regen, dep=%x\n",GET_DTOP_DEP_DET_TIMING_STABLE(u32_dtopdep_bkofs));
                    }

                    if(pstHDMIPollingInfo->u8RegenFsm==E_REGEN_DONE
                    && !_KHal_HDMIRx_Pll_TOP_Stable(enInputPortSelect, PLLTOP_IVS_OVS_TOLERANCE_REGEN_REDO)
                    )
                    {
                        pstHDMIPollingInfo->u8RegenReDoDebounceCnt++;
                        if (pstHDMIPollingInfo->u8RegenReDoDebounceCnt > u8_debounce_cnt)
                        {
                            _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);
                            MHAL_HDMIRX_MSG_INFO("3.do regen, pll top unstable\n");
                        }
                    }
                    else if (pstHDMIPollingInfo->u8RegenFsm==E_REGEN_CONFIG
                        && _KHal_HDMIRx_Pll_TOP_Stable(enInputPortSelect, PLLTOP_IVS_OVS_TOLERANCE_REGEN_DONE)
                        )
                    {
                        pstHDMIPollingInfo->u8RegenFsm = E_REGEN_DONE;
                        #if 0 //MT9701_TBD_ENABLE TBD , need check below items
                        _KHal_HDMIRx_IRQ_CLR(HDMI_IRQ_PHY, enInputPortSelect, BIT0);
                        KHAL_HDMIRx_PHY_IRQ_MASK_CONTROL(enInputPortSelect, TRUE);

                        MHAL_HDMIRX_MSG_INFO("port[%d] DVI2MIU S%d ,FRL=%d\n",enInputPortSelect,pstHDMIPollingInfo->u8dvi2miu_select,IsFRL(pstHDMIPollingInfo->u8FRLRate));
                        _KHal_HDMIRx_Clk_Mux_Sel(E_CLK_MUX_DVI2MIU_S0+pstHDMIPollingInfo->u8dvi2miu_select, enInputPortSelect,(MS_U32)IsFRL(pstHDMIPollingInfo->u8FRLRate), 0);
                        _KHal_HDMIRx_EMP2MIU_REP_Sel(E_CLK_MUX_DVI2MIU_S0+pstHDMIPollingInfo->u8dvi2miu_select,enInputPortSelect,pstHDMIPollingInfo->ucSourceVersion);
                        #endif
                    }
                    else
                    {
                        pstHDMIPollingInfo->u8RegenReDoDebounceCnt = 0;
                    }

                    if (b_I_mode)
                    {
                        if(st_interlace_info.u8OiMd)
                        {
                            bTimingChgStatus = b_vde_chg || b_htt_chg || b_vtt_chg || b_vsb_diff;
                        }
                        else
                        {
                            // i mode, check vde, htt
                            //bTimingChgStatus = b_vde_chg || b_htt_chg || b_vtt_diff || b_vsb_diff;
                            bTimingChgStatus = b_vde_chg || b_htt_chg;
                        }
                        if(bIsFreeSync || u8_IsVRR)
                        {
                            if (b_I_mode)
                            {
                                MHAL_HDMIRX_MSG_INFO("** HDMI error, I mode false alarm, VRR supported P mode only. %d,I=%d\r\n", (enInputPortSelect), b_I_mode);
                            }
                        }
                    }
                    else if(bIsFreeSync || u8_IsVRR)
                    {
                        // p mode, vrr, vtt, vfront chg, check vde
                        bTimingChgStatus = b_vde_chg||b_htt_chg|| b_vback_chg||b_freesync_chg;
                        pstHDMIPollingInfo->u8FrameRateCnt = 0;


                        if (b_I_mode)
                        {
                            MHAL_HDMIRX_MSG_INFO("** HDMI error, I mode false alarm, VRR supported P mode only. %d,I=%d\r\n", (enInputPortSelect), b_I_mode);
                        }
                    }
                    else
                    {
                        bTimingChgStatus = b_vde_chg||b_htt_chg || b_vback_chg||b_freesync_chg;

                        // p mode, non vrr, check vtt
                        if (pstHDMIPollingInfo->bFrameRateCHGFlag)
                        {
                            pstHDMIPollingInfo->bFrameRateCHGFlag = FALSE;
                            pstHDMIPollingInfo->u8FrameRateCnt = 0;
                        }

                        //wait 10*15ms, if vtt change, do regen.
                        if (pstHDMIPollingInfo->u8FrameRateCnt > 15) // Debounce for FRL VRR CTS toggle VRR_en.
                        {
                            bTimingChgStatus |= b_vtt_chg;
                            pstHDMIPollingInfo->u8FrameRateCnt = 0;
                        }
                        else
                        {
                            pstHDMIPollingInfo->u8FrameRateCnt++;
                        }
                    }
                    if (bTimingChgStatus)
                    {
                        _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);
                    }

                    if (KHal_HDMIRx_Get_Pkt_Diff(enInputPortSelect, PKT_AVI))
                    {
                        if(_KHAL_HDMIRX_Is_AVIRepetition_Diff(pstHDMIPollingInfo, enInputPortSelect))
                            _KHal_HDMIRx_RegenTimingProc_Rst(pstHDMIPollingInfo, enInputPortSelect);
                    }

                    if (bTimingChgStatus || pstHDMIPollingInfo->u8RegenFsm != E_REGEN_DONE)
                    {
                        MHAL_HDMIRX_MSG_INFO("vde chg[%x]/htt chg[%x]/vtt chg[%x]/vback chg[%x]|vtt chg[%x],vtt diff[%x],vsb diff[%x]|TimingChgStatus[%x],RegenFsm[%x]|I_mode[%x],OiMd[%x],FreeSync[%x],pix_rep[%x],VRR[%x]\n",
                          b_vde_chg, b_htt_chg, b_vtt_chg, b_vback_chg,
                          b_vtt_chg,b_vtt_diff,b_vsb_diff,
                          bTimingChgStatus,
                          pstHDMIPollingInfo->u8RegenFsm,
                          b_I_mode,st_interlace_info.u8OiMd,bIsFreeSync, u8_pix_rep,u8_IsVRR);

                        // h14 and h20
                        if(_Hal_tmds_GetClockChangeFlag(enInputPortSelect, pstHDMIPollingInfo))
                        {
                            // clear clock big change
                        }
                        _KHal_HDMIRx_RegenTimingProc(pstHDMIPollingInfo, enInputPortSelect,bIsFreeSync);
                    }
                    bNextStateFlag = TRUE;
                }
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
void _Hal_HDMI_InsertEDIDData(MS_U8 ucAddress, MS_U8 ucData)
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
//                  _mhal_HDMIRx_EDIDWriteProtectEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _Hal_HDMI_EDIDWriteProtectEnable(MS_U8 enInputPortSelect, MS_BOOL bWriteProtect)
{
    //MS_U8 u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_PM_DDC_22_L, bWriteProtect? BIT13: 0, BIT13); // REG_DDC_BANK_22[13]: D0 I2C write protect
            break;

        case HDMI_INPUT_PORT1:
            msWrite2ByteMask(REG_PM_DDC_26_L, bWriteProtect? BIT13: 0, BIT13); // REG_DDC_BANK_26[13]: D1 I2C write protect
            break;

        default:
            break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_GetAudioFrequency()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
TMDS_AUDIO_FREQUENCY_INDEX Hal_HDMI_GetAudioFrequency(MS_U8 enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIRxPollingInfo)
{
    DWORD dwCTSValue = 0, dwNValue = 0, dwTMDSClkValCnt[10] = {0}, dwClockCount, dwGetMidClkCnt;
#if AUDIO_FREQENCY_LOG
    static DWORD dwPreAudFreqValue[3] = {0};
#endif
    DWORD dwAudFreqValue = HDMI_AUDIO_FREQUENCY_UNVALID;
    BYTE ucCount = 0;
    UNUSED(enInputPortType);
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetDTOPDECBankOffset(enInputPortType);

    for(ucCount = 0; ucCount < 10; ucCount++)
    {
        dwTMDSClkValCnt[ucCount] = u4IO32ReadFld_1(REG_0044_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset,REG_0044_P0_HDMIRX_DTOP_DEC_MISC_REG_CLK_TMDS_HDMI_1P_DET_CNT);
    }

    dwGetMidClkCnt = GetSortMiddleNumber(&dwTMDSClkValCnt[0],10);
    dwClockCount = dwGetMidClkCnt * 12 * 1000 / 256; // move *1000 from dwAudFreqValue formula: for avoiding floating point rounding problem.

    //load current CTS/N value first
    //msWrite2ByteMask(REG_173024 + wOffset_hdmi, 0xFFFF, 0xFFFF); // hdmi_dual_12[15:0]: CTS[15:0] from ACR packet
    //msWrite2ByteMask(REG_173026 + wOffset_hdmi, 0xFFFF, 0xFFFF); // hdmi_dual_13[15:0]: N[15:0] from ACR packet
    //msWriteByteMask(REG_173028 + wOffset_hdmi, 0xFF, 0xFF); // hdmi_dual_14[7:4]: N[19:16]; [3:0]: CTS[19:16] from ACR packet
    //load current CTS/N value first
    //W2BYTE(REG_HDMI_DUAL_0_12_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_12[15:0]: CTS[15:0] from ACR packet
    vIO32Write2B_1(REG_0034_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xffff);
    //W2BYTE(REG_HDMI_DUAL_0_13_L +u32HDMIBankOffset, 0xFFFF); // hdmi_u0_13[15:0]: N[15:0] from ACR packet
    vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xffff);
    //W2BYTEMSK(REG_HDMI_DUAL_0_14_L +u32HDMIBankOffset, 0xFF, 0xFF); // hdmi_u0_14[7:4]: N[19:16]; [3:0]: CTS[19:16] from ACR packet
    vIO32WriteFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xf, REG_003C_P0_HDMIRX_PKT_DEP_0_REG_CTS_1);
    vIO32WriteFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0xf, REG_003C_P0_HDMIRX_PKT_DEP_0_REG_N_1);

    //dwNValue = (msRead2Byte(REG_173026 + wOffset_hdmi)) | (((msReadByte(REG_173028 + wOffset_hdmi) & 0xF0) >> 4) << 16);
    dwNValue = (u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset)) | (u4IO32ReadFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset,REG_003C_P0_HDMIRX_PKT_DEP_0_REG_N_1) << 16);
    dwCTSValue = ((u2IO32Read2B_1(REG_0034_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset)) | (u4IO32ReadFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset,REG_003C_P0_HDMIRX_PKT_DEP_0_REG_CTS_1) << 16));

    if ((dwNValue < 0x10) || (dwCTSValue < 0x10))
        return HDMI_AUDIO_FREQUENCY_UNVALID;   //error

    //dwAudFreqValue = ((dwTMDSClkValCnt / 128) * dwNValue) / dwCTSValue;
    //_tmp=__hdmiRxStatusFRL["TMDSCLOCK"]*__hdmiRxStatusFRL["AUDIO_N"]/128/__hdmiRxStatusFRL["AUDIO_CTS"]*1000

    if(pstHDMIRxPollingInfo->ucSourceVersion>=HDMI_SOURCE_VERSION_HDMI20)//>340Mcsc
    {
        dwAudFreqValue = (1*(dwClockCount / 128) * dwNValue) / dwCTSValue;

#if AUDIO_FREQENCY_LOG // debug log
        {
            if((enInputPortType >= INPUT_PORT_DVI0) && (enInputPortType < INPUT_PORT_DVI3))
            {
                if(dwPreAudFreqValue[enInputPortType] != dwAudFreqValue)
                {
                    MHAL_HDMIRX_MSG_INFO("---[KHal_HDMIRx_GetAudioFreq] [[FRL]] dwPreAudFreqValue: %x, dwAudFreqValue: %x,  dwClockCount: %x, dwGetMidClkCnt: %x, dwNValue:%x, dwCTSValue:%x \r\n", dwPreAudFreqValue[enInputPortType], dwAudFreqValue, dwClockCount, dwGetMidClkCnt, dwNValue, dwCTSValue);
                    dwPreAudFreqValue[enInputPortType] = dwAudFreqValue;
                }
            }
            else
            {
                MHAL_HDMIRX_MSG_INFO("---[KHal_HDMIRx_GetAudioFreq] [[FRL]] enInputPortType: %x, this is not a valid port index for this function. \r\n",enInputPortType);
            }
        }
#endif
    }
    else
    {
        dwAudFreqValue = ((dwClockCount / 128) * dwNValue) / dwCTSValue;

#if AUDIO_FREQENCY_LOG // debug log
        {
            if((enInputPortType >= INPUT_PORT_DVI0) && (enInputPortType < INPUT_PORT_DVI3))
            {
                if(dwPreAudFreqValue[enInputPortType] != dwAudFreqValue)
                {
                    MHAL_HDMIRX_MSG_INFO("---[KHal_HDMIRx_GetAudioFreq] [[TMDS]] dwPreAudFreqValue: %x, dwAudFreqValue: %x,  dwClockCount: %x, dwGetMidClkCnt: %x, dwNValue:%x, dwCTSValue:%x \r\n", dwPreAudFreqValue[enInputPortType], dwAudFreqValue, dwClockCount, dwGetMidClkCnt, dwNValue, dwCTSValue);
                    dwPreAudFreqValue[enInputPortType] = dwAudFreqValue;
                }
            }
            else
            {
                MHAL_HDMIRX_MSG_INFO("---[KHal_HDMIRx_GetAudioFreq] [[TMDS]] enInputPortType: %x, this is not a valid port index for this function. \r\n",enInputPortType);
            }
        }
#endif
    }


    if (ABS_MINUS(dwAudFreqValue, 32) < 3)
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_32K;

    else if (ABS_MINUS(dwAudFreqValue, 44) < 3)
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_44K;

    else if (ABS_MINUS(dwAudFreqValue, 48) < 3)
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_48K;

    else if (ABS_MINUS(dwAudFreqValue, 88) < 4)
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_88K;

    else if (ABS_MINUS(dwAudFreqValue, 96) < 4)
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_96K;

    else if (ABS_MINUS(dwAudFreqValue, 176) < 8)
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_176K;

    else if (ABS_MINUS(dwAudFreqValue, 192) < 8)
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_192K;

    else
        dwAudFreqValue = HDMI_AUDIO_FREQUENCY_UNVALID; //error

    return (TMDS_AUDIO_FREQUENCY_INDEX)dwAudFreqValue;

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
void _Hal_HDMI_SetAudioFrequency(MS_U16 usAudioFrequency)
{
    MS_U8 ucAPLLDDiv = 0;
    MS_U8 ucAPLLFBDiv = 6;
    MS_U8 ucAPLLKP = 0;
    MS_U8 ucAPLLKM = 1;
    MS_U8 ucAPLLKN = 1;

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
MS_U16 _Hal_HDMI_P1P2DDCOffset(MS_U8 enInputPort)
{
    MS_U16 wOffset = 0;

    switch(enInputPort)
    {
        default:
        case HDMI_INPUT_PORT1:
            wOffset = 0x0000;
            break;
    }

    return wOffset;

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
void Hal_HDMI_InternalEDIDEnable(MS_U8 enInputPortSelect, MS_BOOL bDDCEnable)
{
    //MS_U8 u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);

    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_PM_DDC_22_L, bDDCEnable? BIT15: 0, BIT15); // REG_DDC_BANK_22[15]: D0 DDC enable
            break;

        case HDMI_INPUT_PORT1:
            msWrite2ByteMask(REG_PM_DDC_26_L, bDDCEnable? BIT15: 0, BIT15); // REG_DDC_BANK_26[15]: D1 DDC enable
            break;

        default:
            break;
    }
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
void Hal_HDMI_Software_Reset(MS_U8 enInputPortSelect __attribute__ ((unused)), MS_U16 u16Reset)
{
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortType);
    MS_U32 u32_hdcp_bk_ofs = _KHal_HDMIRx_GetHDCPBankOffset(enInputPortType);
    MS_U8 u8_tmp = 0;
    /*
original
[0]: reset HDMI2.0/1.4 decoder
[1]: reset HDMI2.1/2.0/1.4 error detect
[2]: reset HDCP (1p & 8p)
[3]: reset depacket ( include packet parser ) (1p & 8p)

new
[0]: dec
[1]: hdcp
[2]: deep color
[3]: pkt
*/
    if (u16Reset & HDMI_SW_RESET_DVI) // Bit0
    {
        vIO32Write2B_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset, BIT(0));
        vIO32Write2B_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset, 0);
    }

    if (u16Reset & HDMI_SW_RESET_HDCP) // Bit1
    {
        u8_tmp = u4IO32ReadFld_1(REG_005C_P0_HDCP + u32_hdcp_bk_ofs, REG_005C_P0_HDCP_REG_DE_UNSTABLE_RST_EN);
        vIO32WriteFld_1(REG_005C_P0_HDCP + u32_hdcp_bk_ofs, 1, REG_005C_P0_HDCP_REG_DE_UNSTABLE_RST_EN);
        vIO32Write2B_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset, BMASK(1 : 0));
        vIO32Write2B_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset, 0);

        vIO32WriteFld_1(REG_005C_P0_HDCP + u32_hdcp_bk_ofs, u8_tmp, REG_005C_P0_HDCP_REG_DE_UNSTABLE_RST_EN);
    }

    if (u16Reset & HDMI_SW_RESET_HDMI) // Bit3
    {
        vIO32Write2B_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset, BIT(3));
        vIO32Write2B_1(REG_0000_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset, 0);
    }

    if (u16Reset & HDMI_SW_RESET_DVI2MIU) // Bit10
    {
        _KHal_HDMIRx_Sw_Rst_innermisc(HDMI_RST_INNERMISC_DVI2MIU_S0);

        //_KHal_HDMIRx_Sw_Rst_innermisc( HDMI_RST_INNERMISC_DVI2MIU_S1);
    }

    if (u16Reset & HDMI_SW_RESET_AUDIO_FIFO) // Bit14
    {
        vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x1, REG_0000_HDMIRX_AUDIO_S0_REG_WRAP_SYN_RST);
        vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x1, REG_0000_HDMIRX_AUDIO_S0_REG_HDRX_AFF_CLR);
        //vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x1, REG_0000_HDMIRX_AUDIO_S0_REG_HDRX_AFFW_RST);
        vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x1, REG_0000_HDMIRX_AUDIO_S0_REG_HDRX_AFFR_RST);

        vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x0, REG_0000_HDMIRX_AUDIO_S0_REG_WRAP_SYN_RST);
        vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x0, REG_0000_HDMIRX_AUDIO_S0_REG_HDRX_AFF_CLR);
        vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x0, REG_0000_HDMIRX_AUDIO_S0_REG_HDRX_AFFR_RST);
        //vIO32WriteFld_1(REG_0000_HDMIRX_AUDIO_S0, 0x0, REG_0000_HDMIRX_AUDIO_S0_REG_HDRX_AFFW_RST);
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
void Hal_HDMI_pkt_reset(MS_U8 enInputPortSelect __attribute__ ((unused)), HDMI_REST_t enResetType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo,MS_BOOL bIsRstTgen)
{
    if(!KHAL_HDMIRX_RESET_PKT_PERIOD_IF_VALID())
        return;

    if (enResetType & REST_AVMUTE)
    {
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x1, FLD_BIT(0));
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x0, FLD_BIT(0));
    }
    if (enResetType & REST_Y_COLOR)
    {
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x1, FLD_BIT(1));
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x0, FLD_BIT(1));
    }
    if (enResetType & REST_PIXEL_REPETITION)
    {
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x1, FLD_BIT(2));
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x0, FLD_BIT(2));
    }
    if (enResetType & REST_FRAME_REPETITION)
    {
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x1, FLD_BIT(3));
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x0, FLD_BIT(3));
    }
    if (enResetType & REST_GCP_PKT)
    {
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x1, FLD_BIT(4));
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC , 0x0, FLD_BIT(4));
    }
    if (enResetType & REST_DEEP_COLOR_FIFO)
    {
        // not sure shall we rst dc_fifo....????
        // dtop_dec_30[0]: reg_hdmirx_dc_fifo_rst
        vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC , 0x1,
                        REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DC_FIFO_RST);
        vIO32WriteFld_1(REG_00C0_P0_HDMIRX_DTOP_DEC_MISC , 0x0,
                        REG_00C0_P0_HDMIRX_DTOP_DEC_MISC_REG_HDMIRX_DC_FIFO_RST);
    }
    if (enResetType & REST_HDMI_STATUS)
    {
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEP_0 , 0x1, FLD_BIT(7));
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEP_0 , 0x0, FLD_BIT(7));
    }

    if(bIsRstTgen)
    {
        _KHAL_HDMIRx_Reset_TGen(enInputPortSelect,pstHDMIPollingInfo);
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
void Hal_HDCP_ClearStatus(MS_U8 enInputPortSelect __attribute__ ((unused)), MS_U16 usInt)
{
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortSelect);
    vIO32Write2B_1(REG_0004_P0_HDCP + u32PortBankOffset, usInt << 8);
}

//**************************************************************************
//  [Function Name]:
//                  KHal_HDMIRx_audio_channel_status()
//  [Description]
//                  return 1 : 8 CH, 0: 2 CH
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_U8 KHal_HDMIRx_audio_channel_number(MS_U8 ucHDMIInfoSource __attribute__ ((unused)))
{
    MS_U8 u8_aud_ch = u4IO32ReadFld_1(REG_0008_P0_HDMIRX_PKT_DEP_0,
        REG_0008_P0_HDMIRX_PKT_DEP_0_REG_ASP_LAYOUT);

    return u8_aud_ch;
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
    MS_U8 ucMuteEvent = u2IO32Read2B_1(REG_0014_HDMIRX_AUDIO_S0)& 0xff ;//combo gptop_4a[7:0]/4a[15:8]h-->HDMIRX_AUDIO_S0_05h/04h
    MS_U8 bMulitCh = KHal_HDMIRx_audio_channel_number(NULL);
    MS_U32 u32PortBankOffset;
    u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(0);
    static MS_U16 u16DataStableChk = 0;
    MS_U8 i = 0;
    MS_U8 u8DataMuteCnts = 0;
    MS_U8 u8AudioDataCheckCnts = 100;
    MS_U8 u8AudioDataMuteTh = 5;
    MS_U16 u16AudioDataStableTh = 5;

    if (ucMuteEvent & HDMI_CTSN_OVERRANGE)
    {
        if (u4IO32ReadFld_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, REG_0008_P0_HDMIRX_PKT_DEP_0_REG_CTSN_OV_RANGE))// hdmi_u0_02[6]: reg_ctsn_ov_range
        {
            _KHal_HDMIRx_UpdateCTSNValue();
            vIO32WriteFld_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0008_P0_HDMIRX_PKT_DEP_0_REG_CTSN_OV_RANGE);
        }
        else
            ucMuteEvent &= (~HDMI_CTSN_OVERRANGE);

    }
    if (ucMuteEvent & HDMI_CLOCK_BIG_CHANGE)
    {
        if (u4IO32ReadFld_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, REG_0008_P0_HDMIRX_PKT_DEP_0_REG_VCLK_BIG_CHG))// hdmi_u0_02[7]: reg_vclk_big_chg
        {
            vIO32WriteFld_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0008_P0_HDMIRX_PKT_DEP_0_REG_VCLK_BIG_CHG);
        }
        else
            ucMuteEvent &= (~HDMI_CLOCK_BIG_CHANGE);
    }
    if (ucMuteEvent & HDMI_AVMUTE)
    {
#if 0   //removed it, follow MT9700
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x1, REG_0000_P0_HDMIRX_PKT_DEC_REG_RESET_AVMUTE);//reg_pkt_rst [0]: Reset AVMUTE register at GCONTROL:AVMUTE.
        vIO32WriteFld_1(REG_0000_P0_HDMIRX_PKT_DEC + u32PortBankOffset, 0x0, REG_0000_P0_HDMIRX_PKT_DEC_REG_RESET_AVMUTE);//reg_pkt_rst [0]: Reset AVMUTE register at GCONTROL:AVMUTE.
        ucMuteEvent &= (~HDMI_AVMUTE);
#endif
        if(Hal_HDMI_GetAVMuteEnableFlag(0) == FALSE)
            ucMuteEvent &= (~HDMI_AVMUTE);
    }
    if (ucMuteEvent & HDMI_NO_INPUT_CLOCK)
    {
        if (!u4IO32ReadFld_1(REG_0114_HDMIRX_AUDIO_S0,REG_0114_HDMIRX_AUDIO_S0_REG_SYN_NO_IPT_LK))
            ucMuteEvent &= (~HDMI_NO_INPUT_CLOCK);
    }
    if (ucMuteEvent & HDMI_AUDIO_SAMPLE_FLAT_BIT)
    {
        if (u4IO32ReadFld_1(REG_0020_HDMIRX_AUDIO_S0,REG_0020_HDMIRX_AUDIO_S0_REG_IRQ_FLAT))
        {
            //clear flat audio sample
            vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x1, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_FLAT_CLR);
            vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x0, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_FLAT_CLR);
        }
        else
            ucMuteEvent &= (~HDMI_AUDIO_SAMPLE_FLAT_BIT);
    }
    if (ucMuteEvent & HDMI_AUDIO_PRESENT_BIT_ERROR)
    {
        if (u4IO32ReadFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, REG_0010_P0_HDMIRX_PKT_DEP_0_REG_ASP_PRSNT_BIT_ERR))
        {
            vIO32WriteFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0010_P0_HDMIRX_PKT_DEP_0_REG_ASP_PRSNT_BIT_ERR);
        }
        else
            ucMuteEvent &= (~HDMI_AUDIO_PRESENT_BIT_ERROR);
    }

    if (u4IO32ReadFld_1(REG_0020_HDMIRX_AUDIO_S0,REG_0020_HDMIRX_AUDIO_S0_REG_IRQ_OVRUN)|u4IO32ReadFld_1(REG_0020_HDMIRX_AUDIO_S0,REG_0020_HDMIRX_AUDIO_S0_REG_IRQ_UDRUN))
    {
        if (ucMuteEvent & HDMI_AUDIO_FIFO_OVERFLOW_UNDERFLOW)
        {
            Hal_HDMI_Software_Reset(0, HDMI_SW_RESET_AUDIO_FIFO);
        }
        //clear audio fifo overflow/underflow
        vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x1, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_UDRUN_CLR);
        vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x1, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_OVRUN_CLR);
        vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x0, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_UDRUN_CLR);
        vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x0, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_OVRUN_CLR);
    }
    else
        ucMuteEvent &= (~HDMI_AUDIO_FIFO_OVERFLOW_UNDERFLOW);


    if (ucMuteEvent & HDMI_AUDIO_SAMPLE_PARITY_ERROR)
    {
        if (u4IO32ReadFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, REG_0010_P0_HDMIRX_PKT_DEP_0_REG_ASAMPLE_ERR)|
            u4IO32ReadFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, REG_0010_P0_HDMIRX_PKT_DEP_0_REG_AS_PBIT_ERR))
        {
            vIO32WriteFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0010_P0_HDMIRX_PKT_DEP_0_REG_ASAMPLE_ERR);
            vIO32WriteFld_1(REG_0010_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, 0x1, REG_0010_P0_HDMIRX_PKT_DEP_0_REG_AS_PBIT_ERR);
        }
        else
            ucMuteEvent &= (~HDMI_AUDIO_SAMPLE_PARITY_ERROR);
    }

    if(bMulitCh == FALSE) // 2ch
    {
        for(i=0; i<u8AudioDataCheckCnts; i++)//temp value
        {
            if((msRead2Byte(0x112D80) == 0x0000) && (msRead2Byte(0x112D82) == 0x0000))//Check Haydn input data = 0
            {
                u8DataMuteCnts++;
            }
        }
    }
    else // 8ch
    {
        u8DataMuteCnts = 0; // temp value for data not mute
    }

    if(ucMuteEvent != 0)
    {
        u16DataStableChk = 0;
        vIO32WriteFld_1(REG_0014_HDMIRX_AUDIO_S0, 0, FLD_BIT(6));//disable audio FIFO event
    }
    else
    {
        if(u8DataMuteCnts < u8AudioDataMuteTh) // audio data is not 0
        {
            if(u16DataStableChk < u16AudioDataStableTh)
            {
                u16DataStableChk++;
            }
            else if(u16DataStableChk == u16AudioDataStableTh)
            {
                u16DataStableChk++;
                vIO32WriteFld_1(REG_0014_HDMIRX_AUDIO_S0, 1, FLD_BIT(6));//enable audio FIFO event
            }
        }
        else // audio data is mute
        {
            if(u4IO32ReadFld_1(REG_0020_HDMIRX_AUDIO_S0,REG_0020_HDMIRX_AUDIO_S0_REG_IRQ_OVRUN)||u4IO32ReadFld_1(REG_0020_HDMIRX_AUDIO_S0,REG_0020_HDMIRX_AUDIO_S0_REG_IRQ_UDRUN))
            {
                Hal_HDMI_Software_Reset(0, HDMI_SW_RESET_AUDIO_FIFO);
                //clear audio fifo overflow/underflow
                vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x1, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_UDRUN_CLR);
                vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x1, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_OVRUN_CLR);
                vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x0, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_UDRUN_CLR);
                vIO32WriteFld_1(REG_0024_HDMIRX_AUDIO_S0, 0x0, REG_0024_HDMIRX_AUDIO_S0_REG_IRQ_OVRUN_CLR);
            }
            vIO32WriteFld_1(REG_0014_HDMIRX_AUDIO_S0, 0, FLD_BIT(6));
            u16DataStableChk = 0;
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
MS_BOOL Hal_HDMI_get_packet_value(MS_U8 enInputPortSelect  __attribute__ ((unused)), MS_HDMI_PACKET_STATE_t u8state, MS_U8 u8byte_idx, MS_U8 *pu8Value)
{
    //CAUTION:multiple 4
    //the byte_idx is count from 0
    MS_U32 u32Base_add = 0;
    MS_U32 u32Reg_Offset_full = 0;
    MS_U32 u32Reg_Offset_partial = 0;
    MS_U16 u16tmp;
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(0);

    switch (u8state)
    {
        case PKT_MPEG:
            {
                // 1~5
                //since the MPEG PB1 is at byte1(start from byte0, so need to offset the MS_U8 idx)
                if (u8byte_idx >= HDMI_MPEG_PACKET_LENGTH)
                {
                    return FALSE;
                }
                u32Reg_Offset_partial = (u8byte_idx + 1) % 2;
                u32Reg_Offset_full = ((u8byte_idx + 1) / 2) * 2;
                u32Base_add = REG_0144_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset; // hdmi_u0_57[7:0]: reg_mpeg_if1; 59[7:0]: reg_mpeg_if5
            }
            break;

        case PKT_AUI:
            {
                // PB1 ~ PB5
                //since the Audio INFO Frame PB1 is at byte1(start from byte0, so need to offset the MS_U8 idx)
                if (u8byte_idx >= HDMI_AUDIO_PACKET_LENGTH)
                {
                    return FALSE;
                }

                u32Reg_Offset_partial = (u8byte_idx + 1) % 2;
                u32Reg_Offset_full = ((u8byte_idx + 1) / 2) * 2;
                u32Base_add = REG_0104_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset;
            }
            break;

        case PKT_SPD:
            {
                // PB1 ~ PB25
                //since the SPD PB1 is at byte1(start from byte0, so need to offset the MS_U8 idx)
                if (u8byte_idx >= HDMI_SPD_PACKET_LENGTH)
                {
                    return FALSE;
                }

                u32Reg_Offset_partial = (u8byte_idx + 1) % 2;
                u32Reg_Offset_full = ((u8byte_idx + 1) / 2) * 2;
                u32Base_add = REG_0104_P0_HDMIRX_PKT_DEC + u32PortBankOffset; // hdmi3_u0_52[7:0]: reg_audio_if01; hdmi3_u0_56[15:8]: reg_audio_if10
            }
            break;

        case PKT_AVI:
            {
                // 0: PB1
                // 1: PB2
                // 2: PB3
                // 3: PB4
                // 4: PB5
                // 5: PB6
                // 12: PB13
                // 13: PB14
                // 14: AVI Version

                // 1~14
                //since the AVI PB1 is at byte1(start from byte0, so need to offset the MS_U8 idx )
                if (u8byte_idx >= HDMI_AVI_PACKET_LENGTH)
                {
                    return FALSE;
                }
                if (u8byte_idx < 14)
                {
                    u32Reg_Offset_partial = (u8byte_idx + 1) % 2;
                    u32Reg_Offset_full = ((u8byte_idx + 1) / 2) * 2;
                    u32Base_add = REG_0084_P0_HDMIRX_PKT_DEC + u32PortBankOffset;
                }
                else
                {
                    u32Reg_Offset_partial = 0;
                    u32Reg_Offset_full = 0;
                    u32Base_add = REG_0080_P0_HDMIRX_PKT_DEC + u32PortBankOffset;
                }
            }
            break;

        case PKT_GC:
            {
                /*
                [0] avmute
                [1]default phase
                [4:2] hw phase
                [7:5] hw phase
                [11:8] dc
                [15:12] phase
                */
                if (u8byte_idx >= HDMI_GCP_PACKET_LENGTH)
                {
                    return FALSE;
                }

                u32Reg_Offset_partial = u8byte_idx;
                u32Reg_Offset_full = 0;
                u32Base_add = REG_004C_P0_HDMIRX_PKT_DEC + u32PortBankOffset; // hdmi3_u0_52[7:0]: reg_audio_if01; hdmi3_u0_56[15:8]: reg_audio_if10
            }
            break;

        case PKT_ASAMPLE:
            //no action here
            return FALSE;
            break;

        case PKT_ACR:
            //no action here
            return FALSE;
            break;

        case PKT_VS:
            {
                // 0: HB0
                // 1: HB1
                // 2: HB2
                // 3: NULL
                // 4: PB0
                // 5: PB1
                // 6: PB2
                // 7: PB3
                // 8: PB4
                // 9: PB5
                // 10: PB6
                if (u8byte_idx >= HDMI_VS_PACKET_LENGTH)
                    return FALSE;

                if (u8byte_idx == 0)
                {
                    u32Reg_Offset_partial = 0;
                    u32Reg_Offset_full = 0;
                    u32Base_add = REG_00FC_P0_HDMIRX_PKT_DEP_0 +u32PortBankOffset;
                }
                else if(u8byte_idx == 3)
                {
                    *pu8Value = 0;
                    return TRUE;
                }
                else if(u8byte_idx < 3)
                {
                    u32Reg_Offset_partial = (u8byte_idx-1) % 2;
                    u32Reg_Offset_full = ((u8byte_idx-1) / 2) * 2;
                    // hdr1
                    u32Base_add = REG_00C0_P0_HDMIRX_PKT_DEP_0 +u32PortBankOffset;
                }
                else
                {
                    u32Reg_Offset_partial = (u8byte_idx-4) % 2;
                    u32Reg_Offset_full = ((u8byte_idx-4) / 2) * 2;
                    // pb0
                    u32Base_add = REG_00C4_P0_HDMIRX_PKT_DEP_0 +u32PortBankOffset;
                }
            }
            break;

        case PKT_NULL:
            return FALSE;
            break;

        case PKT_ISRC1:
            {
                // PB0 ~ PB15
                // u8byte_idx++; //skip pb0, but register access still count from pb0
                if (u8byte_idx >= HDMI_ISRC1_PACKET_LENGTH)
                    return FALSE;

                u32Reg_Offset_partial = (u8byte_idx) % 2;
                u32Reg_Offset_full = ((u8byte_idx) / 2) * 2;

                vIO32WriteFld_1(REG_001C_P0_HDMIRX_PKT_DEP_0, 0, REG_001C_P0_HDMIRX_PKT_DEP_0_REG_GMP_OUT_EN); // hdmi_u0_07[13]: Orignial ISRC packet
                u32Base_add = REG_0080_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset;                                // hdmi_u0_20[7:0]: reg_isrc_pb0; 27[15:8]: reg_isrc_pb15
            }
            break;

        case PKT_ISRC2:
            {
                // isrc1 PB0 ~ PB15
                // isrc2 PB0 ~ PB15
                // u8byte_idx++; //skip pb16, but register access still count from pb16
                if (u8byte_idx >= HDMI_ISRC2_PACKET_LENGTH)
                    return FALSE;

                u32Reg_Offset_partial = (u8byte_idx) % 2;
                u32Reg_Offset_full = ((u8byte_idx) / 2) * 2;

                u32Base_add = REG_0080_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset; // hdmi_u0_28[7:0]: reg_isrc_pb16; 2F[15:8]: reg_isrc_pb31
            }
            break;

        case PKT_ACP:
            {
                // PB0 ~ PB15 for Super Audio DVD
                if (u8byte_idx >= HDMI_ACP_PACKET_LENGTH)
                    return FALSE;

                u32Reg_Offset_partial = (u8byte_idx) % 2;
                u32Reg_Offset_full = ((u8byte_idx) / 2) * 2;

                u32Base_add = REG_0044_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset; // hdmi_u0_17[7:0]: reg_acp_pb0; 1E[15:8]: reg_acp_pb15
            }
            break;

        case PKT_ONEBIT_AUD:
            return FALSE;
            break;

        case PKT_HDR:
            {
            // 0: HB1
            // 1: HB2
            // 2: PB0
            // 3: PB1
            // 4: PB2
            // 5: PB3
            // 6: PB4
            // ...
            // 28: PB26
                //TODO:Need TO confirm the sequence, the sequence is invert between M6 and previous generation IC
                // 0~1: header, 2~28: payload
                if (u8byte_idx >= HDMI_HDR_PACKET_LENGTH)
                    return FALSE;

                u32Reg_Offset_partial = (u8byte_idx) % 2;
                u32Reg_Offset_full = ((u8byte_idx) / 2) * 2;
                // hdr1
                u32Base_add = REG_0100_P0_HDMIRX_PKT_DEP_1 +u32PortBankOffset;
            }
            break;

        case PKT_MULTI_VS:
            {
                // HB1, HB2, PB0, PB1, ... , PB27 (30)
                // HB1, HB2, PB0, PB1, ... , PB27 (30)
                // HB1, HB2, PB0, PB1, ... , PB27 (30)
                // HB1, HB2, PB0, PB1, ... , PB27 (30)

                if (u8byte_idx >= HDMI_MULTI_VS_PACKET_LENGTH)
                    return FALSE;

                if (u8byte_idx < 120)
                {
                    if (u8byte_idx < 30)
                    {
                    //hdr1
                        u32Base_add = REG_0000_P0_HDMIRX_PKT_DEP_2 + u32PortBankOffset;
                    }
                    else if ((u8byte_idx >= 30) && (u8byte_idx < 60))
                    {
                        u32Base_add = REG_0040_P0_HDMIRX_PKT_DEP_2 + u32PortBankOffset;
                        u8byte_idx = u8byte_idx - 30;
                    }
                    else if ((u8byte_idx >= 60) && (u8byte_idx < 90))
                    {
                        u32Base_add = REG_0080_P0_HDMIRX_PKT_DEP_2 + u32PortBankOffset;
                        u8byte_idx = u8byte_idx - 60;
                    }
                    else if ((u8byte_idx >= 90) && (u8byte_idx < 120))
                    {
                        u32Base_add = REG_00C0_P0_HDMIRX_PKT_DEP_2 + u32PortBankOffset;
                        u8byte_idx = u8byte_idx - 90;
                    }

                    u32Reg_Offset_partial = (u8byte_idx) % 2;
                    u32Reg_Offset_full = ((u8byte_idx) / 2) * 2;
                }
            }
            break;


        case PKT_CHANNEL_STATUS:
            {
                if (u8byte_idx >= HDMI_AUDIO_CHANNEL_STATUS_LENGTH)
                    return FALSE;

                u32Reg_Offset_partial = (u8byte_idx) % 2;
                u32Reg_Offset_full = ((u8byte_idx) / 2) * 2;

                u32Base_add = REG_0040_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset;
            }
            break;


        default:
            return FALSE;
            break;
    }

    u16tmp = u2IO32Read2B_1(u32Base_add + u32Reg_Offset_full);

    if (u32Reg_Offset_partial == 0) //extract MS_U8 0
    {
        *pu8Value = (MS_U8)(u16tmp & 0xff);
    }
    else //extract MS_U8 1
    {
        *pu8Value = (MS_U8)((u16tmp & 0xff00) >> 8);
    }
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
void Hal_HDMI_StablePolling(MS_U8 enInputPortSelect, MS_U32 *ulPacketStatus, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo)
{
    _Hal_tmds_SignalDetectProc(enInputPortSelect, ulPacketStatus, stHDMIPollingInfo);

    _Hal_tmds_FastTrainingProc(enInputPortSelect, stHDMIPollingInfo);
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
void Hal_HDMI_init(MS_U8 enInputPortSelect)
{
    msEread_Init(&usTemp1, &ubTemp2);


#if(!HDMI_Cable5v_0)
    msWrite2ByteMask(REG_SCDC_P0_0C_L, BIT(3), BMASK(3:2)); //scdc_p0_0c[3:2]: [3]:hpd 5v reset overwrite enable; [2]: hpd 5v reset overwrite value
#endif
#if(!HDMI_Cable5v_1)
    msWrite2ByteMask(REG_SCDC_P1_0C_L, BIT(3), BMASK(3:2)); //scdc_p1_0c[3:2]: [3]:hpd 5v reset overwrite enable; [2]: hpd 5v reset overwrite value
#endif

    _KHal_HDMIRx_MAC_Clkgen0_Init(enInputPortSelect);
    _KHal_HDMIRx_MAC_Clkgen1_init(enInputPortSelect);

    _KHal_HDMIRx_MAC_sub_Init(enInputPortSelect);
    _KHAL_HDMIRx_MISC_Init(enInputPortSelect);
    _KHal_HDMIRx_DTOPMISC_INNER_Init(enInputPortSelect);
    _KHal_HDMIRx_Audio_Setting(enInputPortSelect);

    _Hal_tmds_PHYInit(enInputPortSelect);
    //_Hal_tmds_MACInit(enInputPortSelect); // removed
    _KHal_HDMIRx_H21MACInit(enInputPortSelect);
    _Hal_tmds_HDMIInit(enInputPortSelect);

    _KHal_HDMIRx_GetimingDetectPath(enInputPortSelect,HDMI_TIMING_DET_DTOPDEC_DC_AUTO, HDMI_SOURCE_VERSION_HDMI14);
    _KHal_HDMIRx_GetimingDetectPath_dtopdep(enInputPortSelect, HDMI_TIMING_DET_DTOPDEP_RCVTOP, HDMI_SOURCE_VERSION_HDMI14);
    _KHal_HDMIRx_GetimingDetectPath_dtopmisc(enInputPortSelect, HDMI_TIMING_DET_RCV_TOP, HDMI_SOURCE_VERSION_HDMI14);
    _KHal_HDMIRx_Clk_Mux_Sel(E_CLK_MUX_TMDS_FRL, enInputPortSelect, 0, 0);
    // turn on dtop_dec, for hdcp
    _KHal_HDMIRx_IRQ_OnOff(HDMI_IRQ_MAC,enInputPortSelect,0x01,TRUE);

    Hal_HDMI_Audio_MUTE_Enable(0xAF, 0xAF);

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
void Hal_HDMI_ClockRtermControl(MS_U8 enInputPortSelect, MS_BOOL bPullHighFlag)
{
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]enInputPortSelect:%d , bPullHighFlag:%d , time :%d %s  \n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortSelect,bPullHighFlag,MsOS_GetSystemTime(), "\033[m");
#endif
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
        {
            msWrite2ByteMask(REG_DPRX_PHY_PM_00_L, bPullHighFlag? 0: BIT(8), BIT(8));
        }
        break;

        case HDMI_INPUT_PORT1:
        {
            msWrite2ByteMask(REG_DPRX_PHY_PM_20_L, bPullHighFlag? 0: BIT(8), BIT(8));
        }
        break;

        default:
        {
            bPullHighFlag = FALSE;
        }
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
void Hal_HDMI_DataRtermControl(MS_U8 enInputPortSelect, MS_BOOL bPullHighFlag)
{
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]enInputPortSelect:%d , bPullHighFlag:%d , time :%d %s  \n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortSelect,bPullHighFlag,MsOS_GetSystemTime(), "\033[m");
#endif

    if(bPullHighFlag && (Hal_HDMIRx_Check_OnLinePort(enInputPortSelect)))
    {
        Hal_HDMI_MAC_HDCP_Enable(TRUE);
    }
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
        {
            msWrite2ByteMask(REG_DPRX_PHY_PM_00_L, bPullHighFlag? 0: BMASK(11:9), BMASK(11:9));
        }
        break;

        case HDMI_INPUT_PORT1:
        {
            msWrite2ByteMask(REG_DPRX_PHY_PM_20_L, bPullHighFlag? 0: BMASK(11:9), BMASK(11:9));
        }
        break;

        default:
        {
            bPullHighFlag = FALSE;
        }
        break;
    }
    if((!bPullHighFlag) && (Hal_HDMIRx_Check_OnLinePort(enInputPortSelect)))
    {
        Hal_HDMI_MAC_HDCP_Enable(FALSE);
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
void Hal_HDMI_PHYPowerModeSetting(MS_U8 enInputPortSelect ,MS_U8 ucPMMode)
{
    MS_U32 u32PHY2P1BankOffset=0;

    switch(ucPMMode)
    {
        case mhal_ePM_POWERON:
            {
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
            }
            break;

        case mhal_ePM_STANDBY:
            {
                msWriteByteMask(REG_DPRX_PHY_PM_00_H + (0x40*enInputPortSelect) , 0x0E, 0x0F); // reg_nodie_pd_rt
                msWriteByteMask(REG_DPRX_PHY_PM_01_H + (0x40*enInputPortSelect) , BIT(0), BIT(0)); // reg_nodie_en_sqh_ov
            #if PM_SUPPORT_WAKEUP_HDMI_SQH
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + (0x40*enInputPortSelect) , 0xE0, 0xF0); // reg_nodie_en_sqh
            #else
                msWriteByteMask(REG_DPRX_PHY_PM_00_L + (0x40*enInputPortSelect) , 0x00, 0xF0); // reg_nodie_en_sqh
            #endif

#if (PM_SUPPORT_WAKEUP_HDMI_SCDC5V)
                MsOS_DisableInterrupt(E_INT_PM_IRQ_SCDC);
                msWriteByteMask(0x003119 + (0x100*enInputPortSelect), 0x04, 0x06); // [10] 5v PAD high Int, [9] 5v PAD low Int.
                msWriteByteMask(0x00310F + (0x100*enInputPortSelect), 0xFF, 0xFF); // [15:8] SCDC irq status clear
                Delay1ms(2);
                msWriteByteMask(0x00310F + (0x100*enInputPortSelect), 0x00, 0xFF); // [15:8] SCDC irq status clear
#endif
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
            }
            break;

        case mhal_ePM_POWEROFF:
            {
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
            }
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
void Hal_HDMI_HPDControl(MS_U8 enInputPortSelect, MS_BOOL bPullHighFlag)
{
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
        {
            if(bPullHighFlag)
            {
                hw_Set_HdcpHpd();
            }
            else
            {
                hw_Clr_HdcpHpd();
            }
        }
        break;

        case HDMI_INPUT_PORT1:
        {
            if(bPullHighFlag)
            {
                hw_Set_HdcpHpd2();
            }
            else
            {
                hw_Clr_HdcpHpd2();
            }
        }
        break;

        default:
        {
            bPullHighFlag = FALSE;
        }
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
void Hal_HDMI_WriteInputPortEDID(MS_U8 enInputPortSelect, MS_U8 ucEDIDSize, MS_U8 *pEDID)
{
    MS_U8 uctemp = 0;
    MS_U8 ucSizeCount = 0;
    //MS_U8 u8HDMI_GPIO = Hal_HDMI_ComboPortMapping2DDCGPIOConfig(enInputPortSelect);
    MS_U8 u8SramBaseAddr = 0;

    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_PM_DDC_45_L, (ucEDIDSize > 0x2)? 0x01:0x00, 0x01); // REG_DDC_BANK_45[0]: Enhance DDC for D0
            msWrite2ByteMask(REG_PM_DDC_75_L, 0x00, MASKBIT(13:8)); // REG_DDC_BANK_75[12:8]: Select EDID sram base address for cpu read/write
            msWrite2ByteMask(REG_PM_DDC_76_L, 0x00, MASKBIT(5:0)); // REG_DDC_BANK_76[4:0]: Select D0 EDID sram base address
            u8SramBaseAddr = 0;
            break;

        case HDMI_INPUT_PORT1:
            msWrite2ByteMask(REG_PM_DDC_45_L, (ucEDIDSize > 0x2)? 0x02:0x00, 0x02); // REG_DDC_BANK_45[1]: Enhance DDC for D1
            msWrite2ByteMask(REG_PM_DDC_75_L, 0x0400, MASKBIT(13:8)); // REG_DDC_BANK_75[12:8]: Select EDID sram base address for cpu read/write
            msWrite2ByteMask(REG_PM_DDC_76_L, 0x0400, MASKBIT(13:8)); // REG_DDC_BANK_76[12:8]: Select D1 EDID sram base address
            u8SramBaseAddr = 4;
            break;

        default:
            break;
    }

     for(ucSizeCount = 0; ucSizeCount < ucEDIDSize; ucSizeCount++)
    {
        msWrite2ByteMask(REG_PM_DDC_75_L, (u8SramBaseAddr + ucSizeCount)<<8, MASKBIT(13:8));

        for(uctemp = 0; uctemp < HDMI_EDID_BLOCK_SIZE; uctemp++)
        {
            _Hal_HDMI_InsertEDIDData(uctemp, pEDID[uctemp +ucSizeCount *HDMI_EDID_BLOCK_SIZE]);
        }
    }

    _Hal_HDMI_EDIDWriteProtectEnable(enInputPortSelect, TRUE);
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
MS_BOOL Hal_HDMI_GetDEStableStatus(MS_U8 enInputPortSelect __attribute__ ((unused)))
{
    MS_BOOL bDEStableFlag = FALSE;
    //chg to DEC_MISC
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortSelect);

    if (u4IO32ReadFld_1(REG_0080_P0_HDMIRX_DTOP_DEC_MISC + u32PortBankOffset,
            REG_0080_P0_HDMIRX_DTOP_DEC_MISC_REG_DE_STABLE))
    {
        bDEStableFlag = TRUE;
    }

//MHAL_HDMIRX_MSG_ERROR("** KHal_HDMIRx_GetDEStableStatus, %x,%d\n",enInputPortType,bDEStableFlag);

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
MS_BOOL Hal_HDMI_GetAVMuteEnableFlag(MS_U8 enInputPortSelect __attribute__ ((unused)))
{
    MS_BOOL bAVMuteEnableFlag = FALSE;
    MS_U16 u16regvalue = 0;
    u16regvalue = u4IO32ReadFld_1(REG_004C_P0_HDMIRX_PKT_DEC, REG_004C_P0_HDMIRX_PKT_DEC_REG_GCONTROL);
    if(u16regvalue & BIT(0))
    {
        bAVMuteEnableFlag = TRUE;
    }

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
MS_BOOL Hal_HDMI_GetVRREnableFlag(MS_U8 enInputPortSelect __attribute__ ((unused)))
{
    MS_U32 u32EMPBankOffset = _KHal_HDMIRx_GetEMPBankOffset(enInputPortSelect);
    MS_U32 u32_val = u4IO32ReadFld_1(REG_0008_P0_HDMI_EMP + u32EMPBankOffset, REG_0008_P0_HDMI_EMP_REG_EMP_VRR_ENABLE);

    return u32_val?TRUE:FALSE;
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
MS_BOOL Hal_HDMI_Get_InfoFrame(MS_U8 enInputPortSelect  __attribute__ ((unused)), MS_HDMI_PACKET_STATE_t u8state, void *pData, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo  __attribute__ ((unused)))
{
    MS_BOOL bRet = FALSE;
    MS_U8 uctemp = 0;
    MS_U16 usPacketContent = 0;

    //CUATION:multiple 4
    //the hi-low MS_U8 sequence is difference from Morucha, M6 is MS_U8-wise sequence
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(0);
    //MS_U32 u32Reg_Offset_partial;
    MS_U32 u32Reg_Offset_full;
    switch (u8state)
    {
        case PKT_HDR:
        {
            MS_U16 *pInfFrame = (MS_U16 *)pData;

            for (uctemp = 0; uctemp < HDMI_HDR_PACKET_SIZE; uctemp++)
            {
                u32Reg_Offset_full = ((uctemp) / 2) * 4;
                usPacketContent = u2IO32Read2B_1(REG_0104_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset + u32Reg_Offset_full);

                pInfFrame[uctemp] = usPacketContent;
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
MS_U8 Hal_HDMI_avi_infoframe_info(MS_U8 enInputPortSelect __attribute__ ((unused)), MS_U8 u8byte, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)))
{
    MS_U8 u8ReturnValue = 0;
    MS_U16 u16Reg = 0;
    //CAUTION:multiple 4
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(0);

    if (u8byte > 0)
    {
        //in m6
        //h21	h21	15	8	reg_avi_pb01	7	0	8		r	"AVI InfoFrame MS_U8 1.[1:0]: Scan info.[3:2]: Ban info.[4]: Active format info present.[6:5]: RGB or YCbCr."
        //change to *4 is the reg read/write unit is 32bit(4byte), and the first meaningful payload is locate at byte1 instead of byte0
        u16Reg = u2IO32Read2B_1(REG_0084_P0_HDMIRX_PKT_DEC + u32PortBankOffset+ ((u8byte) / 2) * 2);
        u8ReturnValue = (MS_U8)(((u8byte)&0x01) ? (u16Reg >> 8) : (u16Reg & 0xFF));
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
MS_U8 Hal_HDMI_GetGCPColorDepth(MS_U8 enInputPortSelect, MS_U32 *u32PacketStatus)
{
    MS_U8 u8ReValue = HDMI_COLOR_DEPTH_8BIT;
    MS_U16 u16ColorDepth = 0;
    DWORD u32PKTStatus = *u32PacketStatus;

    if(u32PKTStatus & HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG) // GCP packet received
    {
        u16ColorDepth = Hal_HDMI_gcontrol_info(G_Ctrl_CD_VAL, enInputPortSelect);
        u16ColorDepth = u16ColorDepth >> 8;

        switch(u16ColorDepth) // Color depth
        {
            case 0x5:
                u8ReValue = HDMI_COLOR_DEPTH_10BIT;
                break;

            case 0x6:
                u8ReValue = HDMI_COLOR_DEPTH_12BIT;
                break;

            case 0x7:
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
//                  Hal_HDMI_GetColorFormat()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_U8 Hal_HDMI_GetColorFormat(MS_U8 enInputPortSelect __attribute__ ((unused)))
{
    MS_U8 u8Reg=0;
    MS_U8 u8ColorFormat = COMBO_COLOR_FORMAT_RGB;
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortType);
    u8Reg = (u4IO32ReadFld_1(REG_0084_P0_HDMIRX_PKT_DEC + u32PortBankOffset,REG_0084_P0_HDMIRX_PKT_DEC_REG_AVI_PB01) & 0x0060)>>5;

    switch(u8Reg)
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
MS_U8 Hal_HDMI_GetColorRange(MS_U8 enInputPortSelect)
{
    MS_U8 u8YCCValue = Hal_HDMI_GetColorFormat(enInputPortSelect);
    MS_U8 u8ColorRange = COMBO_COLOR_RANGE_DEFAULT;

    MS_U8 u8YccColorRangeReg = (Hal_HDMI_avi_infoframe_info(enInputPortSelect, _BYTE_5, NULL) & BMASK(7:6))>> 6;
    MS_U8 u8RgbColorRangeReg = (Hal_HDMI_avi_infoframe_info(enInputPortSelect, _BYTE_3, NULL) & BMASK(3:2))>>2;
    MS_U8 u8VIC_Value = (Hal_HDMI_avi_infoframe_info(enInputPortSelect, _BYTE_4, NULL) & 0xFF);

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
MS_U16 Hal_HDCP_getstatus(MS_U8 enInputPortSelect __attribute__ ((unused)))
{
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(enInputPortSelect);
    return (MS_U16)u2IO32Read2B_1(REG_0004_P0_HDCP + u32PortBankOffset); // hdmirx_hdcp_01
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
MS_U16 Hal_HDMI_gcontrol_info(HDMI_GControl_INFO_t gcontrol, MS_U8 enInputPortSelect __attribute__ ((unused)))
{
    MS_U16 u16regvalue;
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(0);
    u16regvalue = u4IO32ReadFld_1(REG_004C_P0_HDMIRX_PKT_DEC + u32PortBankOffset, REG_004C_P0_HDMIRX_PKT_DEC_REG_GCONTROL);

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


void ________NEED_CONFIRM_START________(void);
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
void _Hal_HDMI_set_PortSel_byGPIOConfig(MS_U8 enInputPortSelect)
{
    switch(enInputPortSelect)
    {
        case HDMI_INPUT_PORT0:
            msWrite2ByteMask(REG_DPRX_PHY_PM_64_L, 0, BMASK(1:0));//dp_phy_pm_64[1:0]: reg_hdmi_pm_port_sel: means port number of EVB
            break;

        case HDMI_INPUT_PORT1:
            msWrite2ByteMask(REG_DPRX_PHY_PM_64_L, BIT0, BMASK(1:0));//dp_phy_pm_64[1:0]: reg_hdmi_pm_port_sel: means port number of EVB
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
void Hal_tmds_Switch2HDMI(MS_U8 hdmirx_id)
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
            if(msRead2Byte(REG_SCDC_P0_32_L) & BIT4)    // check [4] x74 read 0x70
            {
                _mhal_HDCP22_SetReAuth(TRUE);
            }
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT0, 0);
            Hal_HDMI_SCDC_Clr(HDMI_INPUT_PORT0);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT1, 1);
#endif
            _Hal_HDMI_set_PortSel_byGPIOConfig(hdmirx_id);
#if 0
            //U03 use main link setting config with hw real port
            msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, 0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.
#endif
            msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, 0, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
            msWrite2ByteMask(REG_DPRX_PHY_PM_62_L, 0, BMASK(1:0));
            break;

        case HDMI_INPUT_PORT1:
#if ENABLE_HPD_REPLACE_MODE
            if(msRead2Byte(REG_SCDC_P1_32_L) & BIT4)    // check [4] x74 read 0x70
            {
                _mhal_HDCP22_SetReAuth(TRUE);
            }
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT0, 1);
            Hal_HDMI_SCDC_config(HDMI_INPUT_PORT1, 0);
            Hal_HDMI_SCDC_Clr(HDMI_INPUT_PORT1);
#endif

            _Hal_HDMI_set_PortSel_byGPIOConfig(hdmirx_id);
            //_Hal_HDMI_set_DDC_Engine_byGPIOConfig(hdmirx_id); // MT9701 no use
#if 0
            //U03 use main link setting config with hw real port
            msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, BIT0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.
#endif
            msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, BIT14, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
            msWrite2ByteMask(REG_DPRX_PHY_PM_62_L, BIT2, BMASK(3:2));
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
void Hal_HDMI_PM_SwitchHDMIPort(MS_U8 hdmirx_id)
{
    msWriteByteMask(REG_DPRX_PHY_PM_65_L, 0x0, BIT0); //dp_phy_pm_65[0]: reg_dp_hdmi_pm_port_sel   0:HDMI	1:DP
    //msWriteByteMask(REG_DPRX_HDMI_30_L, 0x0, BIT0);//dp_hdmi_phy_30[0]: reg_dp_select, 0: mux select to hdmi function; 1: mux select to dp function
    //msWriteByteMask(REG_DPRX_HDMI_30_L, 0x0, BIT4); //dp_hdmi_phy_30[4]: reg_force_dp_select, 0: dp_select from  PM domain; 1: dp_select from reg_dp_select

    switch(hdmirx_id)
    {
        case HDMI_INPUT_PORT0:
            _Hal_HDMI_set_PortSel_byGPIOConfig(hdmirx_id);

            //U03 use main link setting config with hw real port
            //msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, 0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

            //msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, 0, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
            break;

        case HDMI_INPUT_PORT1:
            _Hal_HDMI_set_PortSel_byGPIOConfig(hdmirx_id);

            //U03 use main link setting config with hw real port
            //msWrite2ByteMask(REG_DPRX_PHY_PM_63_L, BIT0, BMASK(1:0)); //dp_phy_pm_63[1:0]: reg_dp_pm_port_sel for hdmi sqh select.

            //msWrite2ByteMask(REG_DPRX_PHY_PM_6D_L, BIT14, BMASK(15:14)); //dp_phy_pm_6D[15:14]: reg_data_debounce_out_l_sel for hdmi sqh select.
            break;


        default:
            break;
    }
}

void ________NEED_CONFIRM_END________(void);
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
void Hal_HDMI_Audio_MUTE_Enable(MS_U32 u16MuteEvent, MS_U32 u16MuteMask)
{
    msWriteByteMask(REG_0014_HDMIRX_AUDIO_S0, u16MuteEvent, u16MuteMask);
    msWriteByteMask(REG_0010_HDMIRX_AUDIO_S0, u16MuteEvent, u16MuteMask);
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
MS_U16 Hal_HDMI_GetDataInfo(E_HDMI_GET_DATA_INFO enInfo, MS_U8 enInputPortSelect __attribute__ ((unused)),ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo)
{
    MS_U16 u16HDMIDataInfo = 0;

    if(pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI14)
    {
        _KHal_HDMIRx_GetimingDetectPath_dtopmisc(enInputPortSelect, HDMI_TIMING_DET_RCV_TOP,HDMI_SOURCE_VERSION_HDMI14);
    }
    else if((pstHDMIPollingInfo->ucSourceVersion == HDMI_SOURCE_VERSION_HDMI20))
    {
        _KHal_HDMIRx_GetimingDetectPath_dtopmisc(enInputPortSelect, HDMI_TIMING_DET_RCV_TOP,HDMI_SOURCE_VERSION_HDMI20);
    }

    switch(enInfo)
    {
    	case E_HDMI_GET_HDE:
            if (Hal_HDMI_GetDEStableStatus(enInputPortSelect))
            {
                u16HDMIDataInfo = GET_DTOP_MISC_HDE();
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI port %d, E_HDMI_GET_HDE de unstable. \r\n", ucHDMIInfoSource);
            }
            break;

    	case E_HDMI_GET_VDE:
            if (Hal_HDMI_GetDEStableStatus(enInputPortSelect))
            {
                u16HDMIDataInfo = GET_DTOP_MISC_VDE();
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI port %d, E_HDMI_GET_HDE de unstable. \r\n", ucHDMIInfoSource);
            }
            break;

    	case E_HDMI_GET_HTT:
            if (Hal_HDMI_GetDEStableStatus(enInputPortSelect))
            {
                u16HDMIDataInfo = GET_DTOP_MISC_HTT();
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI port %d, E_HDMI_GET_HDE de unstable. \r\n", ucHDMIInfoSource);
            }
            break;

    	case E_HDMI_GET_VTT:
            if (Hal_HDMI_GetDEStableStatus(enInputPortSelect))
            {
                u16HDMIDataInfo = GET_DTOP_MISC_VTT();
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI port %d, E_HDMI_GET_HDE de unstable. \r\n", ucHDMIInfoSource);
            }
            break;

    	case E_HDMI_GET_SUPPORT_EDID_SIZE:
            u16HDMIDataInfo = HDMI_EDID_SUPPORT_SIZE;
            break;

    	case E_HDMI_GET_MULTIVS_COUNT:
            u16HDMIDataInfo = (MS_U16)u4IO32ReadFld_1(REG_00BC_P0_HDMIRX_PKT_DEP_2, REG_00BC_P0_HDMIRX_PKT_DEP_2_REG_VSIF_CNT);
            break;


    	case E_HDMI_GET_H_POLARITY:
            if (Hal_HDMI_GetDEStableStatus(enInputPortSelect))
            {
                u16HDMIDataInfo = GET_DTOP_MISC_DET_HS_POLARITY();
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI port %d, E_HDMI_GET_HDE de unstable. \r\n", ucHDMIInfoSource);
            }
            break;

    	case E_HDMI_GET_V_POLARITY:
            if (Hal_HDMI_GetDEStableStatus(enInputPortSelect))
            {
                u16HDMIDataInfo = GET_DTOP_MISC_DET_VS_POLARITY();
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI port %d, E_HDMI_GET_HDE de unstable. \r\n", ucHDMIInfoSource);
            }
            break;

    	case E_HDMI_GET_INTERLACE:
            if (Hal_HDMI_GetDEStableStatus(enInputPortSelect))
            {
                u16HDMIDataInfo = GET_DTOP_MISC_DET_I_MD_EN();
            }
            else
            {
                //MHAL_HDMIRX_MSG_INFO("** HDMI port %d, E_HDMI_GET_HDE de unstable. \r\n", ucHDMIInfoSource);
            }
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
MS_U32 Hal_HDMI_GetAudioContentInfo(MS_U8 enInputPortSelect   __attribute__ ((unused)), EN_HDMI_AUDIO_CONTENT_INFO enAudioContentInfo)
{
    MS_U32 ulAudioContent = 0;
    MS_U32 u32HDMIBankOffset = 0;

    //load current CTS/N value first
    vIO32Write2B_1(REG_0034_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset, 0xffff);
    vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset, 0xffff);
    vIO32WriteFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset, 0xf, REG_003C_P0_HDMIRX_PKT_DEP_0_REG_CTS_1);
    vIO32WriteFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset, 0xf, REG_003C_P0_HDMIRX_PKT_DEP_0_REG_N_1);

    switch(enAudioContentInfo)
    {
    	case HDMI_AUDIO_CONTENT_CTS:
            ulAudioContent = ((u2IO32Read2B_1(REG_0034_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset)) | (u4IO32ReadFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset,REG_003C_P0_HDMIRX_PKT_DEP_0_REG_CTS_1) << 16));
            break;

    	case HDMI_AUDIO_CONTENT_N:
            ulAudioContent = (u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset)) | (u4IO32ReadFld_1(REG_003C_P0_HDMIRX_PKT_DEP_0 + u32HDMIBankOffset,REG_003C_P0_HDMIRX_PKT_DEP_0_REG_N_1) << 16);
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
MS_U32 Hal_HDMI_packet_info(MS_U8 enInputPortSelect __attribute__ ((unused)), ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)))
{
    MS_U16 usPacketStatus = 0;
    MS_U32 ulPacketStatus = 0;
    MS_U32 u32PortBankOffset = 0;
    MS_U32 u32_emp_bkofs = 0;

#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)

        //packet received without AVI/GC
        usPacketStatus = u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32PortBankOffset);
        if (usPacketStatus)
        {
            if (usPacketStatus & BIT(6))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(7))
            {
               ulPacketStatus = ulPacketStatus | HDMI_STATUS_RESERVED_PACKET_RECEIVE_FLAG;
            }
            usPacketStatus &= (BIT(6)|BIT(7));
            if (usPacketStatus > 0)
            {
                //clear packet received without AVI/GC
                vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32PortBankOffset, usPacketStatus);
            }
        }

        usPacketStatus = u2IO32Read2B_1(REG_0004_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);
        if (usPacketStatus)
        {
            if (usPacketStatus & BIT(0))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_MPEG_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(1))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(5))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(6))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_ACR_PACKET_RECEIVE_FLAG;
            }
            //if (usPacketStatus & BIT(7))packet received without VS.
            //{
            //  ulPacketStatus = ulPacketStatus | HDMI_STATUS_VS_PACKET_RECEIVE_FLAG;
            //}
            if (usPacketStatus & BIT(8))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_NULL_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(9))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_ISRC2_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(10))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_ISRC1_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(11))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_ACP_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(12))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(13))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_GM_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(14))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_HBR_PACKET_RECEIVE_FLAG;
            }
            if (usPacketStatus & BIT(15))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_VBI_PACKET_RECEIVE_FLAG;
            }
            usPacketStatus &= (BIT(0)|BIT(1)|BIT(5)|BIT(6)|BMASK(15:8));
            if (usPacketStatus > 0)
            {
                // Clear status,clear packet received without VS.
                vIO32Write2B_1(REG_0004_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, usPacketStatus);
            }
        }

        usPacketStatus = u2IO32Read2B_1(REG_000C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);
        if (usPacketStatus)
        {
            if(usPacketStatus &BIT(0))
            {
                ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_DST_RECEIVE_FLAG;
            }
            if(usPacketStatus &BIT(1))
            {
                ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_3D_ASP_RECEIVE_FLAG;
                ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG);
            }
            if(usPacketStatus &BIT(2))
            {
                ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_3D_DSD_RECEIVE_FLAG;
                ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG);
            }

            if(usPacketStatus &BIT(4))
            {
                ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_MULTI_ASP_RECEIVE_FLAG;
                ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG);
            }

            if(usPacketStatus &BIT(5))
            {
                ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_MULTI_DSD_RECEIVE_FLAG;
                ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG);
            }

            usPacketStatus &= (BMASK(5:4)|BMASK(2:0));
            if (usPacketStatus > 0)
            {
               // Clear status
               vIO32Write2B_1(REG_000C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, usPacketStatus);
            }
        }

        usPacketStatus = u2IO32Read2B_1(REG_00A8_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset);
        if (usPacketStatus)
        {
            if (usPacketStatus & BIT(11))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_EDR_VALID_FLAG;
            }
            usPacketStatus &= BIT(11);
            if (usPacketStatus > 0)
            {
                // Clear status
                vIO32Write2B_1(REG_00A8_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset, usPacketStatus);
            }
        }

        usPacketStatus = u2IO32Read2B_1(REG_001C_P0_HDMI_EMP + u32_emp_bkofs);
        if (usPacketStatus)
        {
            if (usPacketStatus & BIT(7))
            {
                ulPacketStatus = ulPacketStatus | HDMI_STATUS_EM_PACKET_RECEIVE_FLAG;
                // MHAL_HDMIRX_MSG_INFO("EMP...1,%d\n",_KHal_HDMIRx_GetSystemTime());
            }

            usPacketStatus &= BIT(7);
            if (usPacketStatus > 0)
            {
                // Clear status
                vIO32Write2B_1(REG_001C_P0_HDMI_EMP + u32_emp_bkofs, usPacketStatus);
            }
        }

#else
    usPacketStatus = u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32PortBankOffset);
    if (usPacketStatus)
    {
        if (usPacketStatus & BIT(4))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(5))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(6))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(7))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_RESERVED_PACKET_RECEIVE_FLAG;
        }
        usPacketStatus &= (BIT(4)|BIT(5)|BIT(6)|BIT(7));
        if (usPacketStatus > 0)
        {
            // Clear status
            vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32PortBankOffset, usPacketStatus);
        }
    }

    usPacketStatus = u2IO32Read2B_1(REG_0004_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);
    if (usPacketStatus)
    {
        if (usPacketStatus & BIT(0))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_MPEG_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(1))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(5))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG;
                        //KHal_HDMIRx_Clr_ASP_channel_status();
        }
        if (usPacketStatus & BIT(6))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_ACR_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(7))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_VS_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(8))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_NULL_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(9))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_ISRC2_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(10))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_ISRC1_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(11))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_ACP_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(12))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(13))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_GM_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(14))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_HBR_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(15))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_VBI_PACKET_RECEIVE_FLAG;
        }
        usPacketStatus &= (BIT(0)|BIT(1)|BMASK(15:5));
        if (usPacketStatus > 0)
        {
            // Clear status
            vIO32Write2B_1(REG_0004_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, usPacketStatus);
        }
    }

    usPacketStatus = u2IO32Read2B_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);
    if (usPacketStatus)
    {
        if (usPacketStatus & BIT(14))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG;
        }
        usPacketStatus &= BIT(14);
        if (usPacketStatus > 0)
        {
            // Clear status
            vIO32Write2B_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, usPacketStatus);
        }
    }

    usPacketStatus = u2IO32Read2B_1(REG_000C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);
    if (usPacketStatus)
    {
    	if(usPacketStatus &BIT(0))
    	{
    		ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_DST_RECEIVE_FLAG;
    	}

    	if(usPacketStatus &BIT(1))
    	{
    		ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_3D_ASP_RECEIVE_FLAG;
    		ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG);
    	}

    	if(usPacketStatus &BIT(2))
    	{
    		ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_3D_DSD_RECEIVE_FLAG;
    		ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG);
    	}

    	if(usPacketStatus &BIT(4))
    	{
    		ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_MULTI_ASP_RECEIVE_FLAG;
    		ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG);
    	}

    	if(usPacketStatus &BIT(5))
    	{
    		ulPacketStatus = ulPacketStatus| HDMI_STATUS_AUDIO_MULTI_DSD_RECEIVE_FLAG;
    		ulPacketStatus = ulPacketStatus& (~HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG);
    	}

    	usPacketStatus &= (BMASK(5:4)|BMASK(2:0));
    	if (usPacketStatus > 0)
    	{
    		// Clear status
    		vIO32Write2B_1(REG_000C_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, usPacketStatus);
    	}
    }

    usPacketStatus = u2IO32Read2B_1(REG_00A8_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset);
    if (usPacketStatus)
    {
    	if (usPacketStatus & BIT(11))
    	{
    		ulPacketStatus = ulPacketStatus | HDMI_STATUS_EDR_VALID_FLAG;
    	}
    	usPacketStatus &= BIT(11);
    	if (usPacketStatus > 0)
    	{
    		// Clear status
    		vIO32Write2B_1(REG_00A8_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset, usPacketStatus);
    	}
    }

    usPacketStatus = u2IO32Read2B_1(REG_001C_P0_HDMI_EMP + u32_emp_bkofs);
    if (usPacketStatus)
    {
    	if (usPacketStatus & BIT(7))
    	{
    		ulPacketStatus = ulPacketStatus | HDMI_STATUS_EM_PACKET_RECEIVE_FLAG;
    		// MHAL_HDMIRX_MSG_INFO("EMP...1,%d\n",_KHal_HDMIRx_GetSystemTime());
    	}
    	/*
    	else
    	{
    		MHAL_HDMIRX_MSG_INFO("EMP...0,%d\n",_KHal_HDMIRx_GetSystemTime());
    	}*/

    	usPacketStatus &= BIT(7);
    	if (usPacketStatus > 0)
    	{
    		// Clear status
    		vIO32Write2B_1(REG_001C_P0_HDMI_EMP + u32_emp_bkofs, usPacketStatus);
    	}
        }

    // hw didnt support amp, HDMI_STATUS_AUDIO_METADATA_RECEIVE_FLAG
#endif

    return ulPacketStatus;
}

#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
//**************************************************************************
//  [Function Name]:
//                  Hal_HDMI_packet_info_partial()
//  [Description]:
//                  use to get HDMI received packet status
//  [Arguments]:
//                  enInputPort : combo port
//  [Return]:
//                  [3] : AVI infoframe
//                  [4] : GC packet
//                  [7] : Vendor specific infoframe
//                  [16]: HDR
//
//**************************************************************************
MS_U32 Hal_HDMI_packet_info_partial(MS_U8 ucHDMIInfoSource __attribute__((unused)), ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__((unused)))
{
    MS_U16 usPacketStatus = 0;
    MS_U32 ulPacketStatus = 0;

    MS_U32 u32PortBankOffset = 0;


    usPacketStatus = u2IO32Read2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32PortBankOffset);
    if (usPacketStatus)
    {
        if (usPacketStatus & BIT(4))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG;
        }
        if (usPacketStatus & BIT(5))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG;
        }

        usPacketStatus &= (BIT(4)|BIT(5));
        if (usPacketStatus > 0)
        {
            // Clear status
            vIO32Write2B_1(REG_0038_P0_HDMIRX_PKT_DEC + u32PortBankOffset, usPacketStatus);
        }
    }

    usPacketStatus = u2IO32Read2B_1(REG_0004_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);
    if (usPacketStatus)
    {
        if (usPacketStatus & BIT(7))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_VS_PACKET_RECEIVE_FLAG;
        }

        usPacketStatus &= (BIT(7));
        if (usPacketStatus > 0)
        {
            // Clear status
            vIO32Write2B_1(REG_0004_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, usPacketStatus);
        }
    }
    usPacketStatus = u2IO32Read2B_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset);
    if (usPacketStatus)
    {
        if (usPacketStatus & BIT(14))
        {
            ulPacketStatus = ulPacketStatus | HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG;
        }
        usPacketStatus &= BIT(14);
        if (usPacketStatus > 0)
        {
            // Clear status
            vIO32Write2B_1(REG_0008_P0_HDMIRX_PKT_DEP_0 + u32PortBankOffset, usPacketStatus);
        }
    }

    return ulPacketStatus;

}
#endif

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
ST_HDMI_RX_COLOR_FORMAT Hal_HDMI_GetColorimetry(MS_U8 enInputPortSelect  __attribute__ ((unused)))
{
    ST_HDMI_RX_COLOR_FORMAT stColorimetry = {COMBO_YUV_COLORIMETRY_NoData, COMBO_COLORIMETRY_NONE, COMBO_COLORIMETRY_Additional_NoData};
    MS_U8 u8ColorimetryValue = ((Hal_HDMI_avi_infoframe_info(enInputPortSelect, _BYTE_2,NULL) & BMASK(7:6)) >> 6);
    MS_U8 u8ExtenedColorimetryValue = ((Hal_HDMI_avi_infoframe_info(enInputPortSelect, _BYTE_3,NULL) & BMASK(6:4))>> 4);
#if 0   //  TBD MT9701_TBD_ENABLE
    MS_U8 u8ExtendedAddiColorimetryVal = (msRead2Byte(REG_HDMIRX_DTOP_PKT_P0_29_L) &BMASK(15:12)) >>12;

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
#else
    stColorimetry.ucExtAddColorimetry = COMBO_COLORIMETRY_Additional_DCI_P3_RGB_D65;
#endif
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
MS_U8 Hal_HDMI_audio_channel_status(MS_U8 ucPortIndex __attribute__ ((unused)), MS_U8 ucByte)
{
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(ucPortIndex);

    // 192 bit
    if (ucByte % 2) //high MS_U8
    {
        return (((u2IO32Read2B_1(REG_0040_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset + (ucByte / 2) * 2)) & 0xff00) >> 8);
    }
    else
    {
        return ((u2IO32Read2B_1(REG_0040_P0_HDMIRX_PKT_DEP_1 + u32PortBankOffset + (ucByte / 2) * 2)) & 0x00ff);
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
    MS_U8 temp=0;

    temp = u2IO32Read2B_1(REG_0040_P0_HDMIRX_PKT_DEP_1);

    if(temp & BIT1)
        enAudioFMT = COMBO_AUDIO_FMT_NON_PCM;
    else
        enAudioFMT = COMBO_AUDIO_FMT_PCM;
#if 0   //need check audio infoframe?
    if((msRead2Byte(REG_HDMI_U0_70_L) & BIT(1)) || (msRead2Byte(REG_HDMI3_U0_52_L) & BMASK(7:5))) //Channel Status: PCM/nPCM || AudioInfo Fraome none PCM Format
    {
        enAudioFMT = COMBO_AUDIO_FMT_NON_PCM;
    }
    else
    {
        enAudioFMT = COMBO_AUDIO_FMT_PCM;
    }
#endif
    return enAudioFMT;
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
Bool Hal_HDMIRx_Set_RB_PN_Swap(BYTE ucPortIndex, HDMI_SWAP_TYPE enHDMI_SWAP_TYPE, Bool bSwapEnable)
{
    Bool bSwapDone = FALSE;
    ucPortIndex = ucPortIndex;
    DWORD u32PHY2P1BankOffset = 0;//_Hal_tmds_GetPHY2P1BankOffset(enInputPortType);
    MS_U32 u32PortBankOffset = _KHal_HDMIRx_GetPortBankOffset(ucPortIndex);

    switch(enHDMI_SWAP_TYPE)
    {
        case HDMI_SWAP_TYPE_PN:
            msWrite2ByteMask(REG_PHY2P1_3_P0_66_L +u32PHY2P1BankOffset, bSwapEnable?0:0x000F, BMASK(3:0));//reg_lane_pn_swap[3:0]
            bSwapDone = TRUE;
            break;

        case HDMI_SAWP_TYPE_RB:
            vIO32WriteFld_1(REG_0100_P0_HDMIRX_DTOP + u32PortBankOffset, bSwapEnable?0x1:0, REG_0100_P0_HDMIRX_DTOP_REG_CHANNEL_SWAP); //0x1707_40h[7]: switch channel order: 1'b1:={ch0,ch1,ch2}; 1'b0:={ch2,ch1,ch0}
            bSwapDone = TRUE;
            break;

        case HDMI_SWAP_TYPE_NONE:
        default:
            break;
    }

    return bSwapDone;
}

//**************************************************************************
//  [Function Name]:
//                  Hal_HDMIRx_Check_OnLinePort()
//  [Description]
//                  This function is used for check HDMI on-line port.
//  [Arguments]:
//
//  [Return]:
//                  Bool: return the result.
//**************************************************************************
Bool Hal_HDMIRx_Check_OnLinePort(BYTE ucPortIndex)
{
    if((msReadByte(REG_DPRX_PHY_PM_64_L) & 0x03) == ucPortIndex)
        return TRUE;
    else
        return FALSE;
}

