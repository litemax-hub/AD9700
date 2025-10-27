/*------------------------------------------------------------------------------
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 *----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   drvXC_HDMI_if.h
/// @brief  HDMI Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \defgroup MISC HDMI modules

 * \defgroup HDMI_IF HDMI interface (drvXC_HDMI_if.h)
 * \ingroup MISC

<b> Features </b>
  - HDMI Ethernet Channel
   Support Ethernet communication through HDMI
  - Audio Return Channel
  - 3D support
  - Frame packing, Side-by-Side half, Top-Buttom and etc.
  - 4Kx2K support
   3840x2160@24/25/30Hz, 4096x2160@24Hz
  - Additional Color Spaces
   sYCC601, AdobeRGB, AdobeYCC601
  - HDMI Micro Connector
   19-pin/1080p

   \image html api_HDMI_if_pic1.png

<b> Operation Flow </b>
 - Initialized and enable HDMI
     - MApi_HDMITx_SetHPDGpioPin(_st_msAPI_XC_InitData.u16HdmitxHpdPin);
     - MApi_HDMITx_Init();
     - MApi_HDMITx_TurnOnOff(TRUE);
     - MApi_HDMITx_SetVideoOnOff(TRUE)
     - MApi_HDMITx_SetAudioConfiguration(HDMITX_AUDIO_48K, HDMITX_AUDIO_CH_2, HDMITX_AUDIO_PCM);
*/
#ifndef DRVXC_HDMI_IF_H
#define DRVXC_HDMI_IF_H
//#if  1//willy add
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define _BYTE_0  0x00
#define _BYTE_1  0x01
#define _BYTE_2  0x02
#define _BYTE_3  0x03
#define _BYTE_4  0x04
#define _BYTE_5  0x05
#define _BYTE_6  0x06
#define _BYTE_7  0x07
#define _BYTE_8  0x08
#define _BYTE_9  0x09
#define _BYTE_10  0x0A
#define _BYTE_11  0x0B
#define _BYTE_12  0x0C
#define _BYTE_13  0x0D
#define _BYTE_14  0x0E
#ifndef _BIT
#define _BIT(_bit_)                  (1 << (_bit_))
#endif
#ifndef ENABLE
#define ENABLE 1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif

// library information
#define HDMI_HDCP_KEY_LENGTH                289U
#define HDMI_HDCP_BKSV_LENGTH               5U
#define HDMI_HDCP22_RIV_LENGTH              8U
#define HDMI_HDCP22_SESSION_KEY_LENGTH      16U
#define HDMI_HDCP22_MESSAGE_LENGTH          129U
#define HDMI_HDCP22_SEND_MESSAGE_LENGTH     (535U)
#define HDMI_POLLING_STACK_SIZE             1U
#define HDMI_HDCP_INTERRUPT_STACK_SIZE      1U
#define HDMI_PACKET_RECEIVE_COUNT           (5U)    // 5 x 10ms = 50ms
#define HDMI_POLLING_INTERVAL               (10U)
#define HDMI_FRAME_RATE_OFFSET              (2U)
#define HDMI_CONTROL_FUNCTION_BUFFER_SIZE   (30U)
#define HDMI_HDCP14_RI_COUNT                550U  // 550 x 10ms = 5.5s
#define HDMI_HDCP_KSV_LIST_LENGTH           635U  // max: 127x5
#define HDMI_HDCP_VPRIME_LENGTH             20U
#define HDMI_HDCP_EVENT_POLLING_STATUS      0x100U
#define HDMI_HDCP_EVENT_RECEIVE_AKE_INIT    0x200U
#define HDMI_HDCP_EVENT_WAIT_MESSAGE        0x300U
#define HDMI_HDCP_EVENT_WRITE_DONE          0x400U
#define HDMI_HDCP_EVENT_STR_SUSPEND         0x500U
#define HDMI_HDCP_EVENT_STR_RESUME          0x600U
#define HDMI_HDCP_EVENT_INDEX_MASK          0xFFFFFF00U
#define HDMI_HDCP_EVENT_STATUS_MASK         0x000000FFU
#define HDMI_HDCP_ALL_EVENT                 0xFFFFFFFFU
#define MSIF_HDMI_LIB_CODE               {'H','D','M','I'}
#define MSIF_HDMI_LIBVER                 {'0','1'}
#define MSIF_HDMI_BUILDNUM               {'0','7'}
#define MSIF_HDMI_CHANGELIST             {'0','0','3','5','1','6','6','3'}
#define HDMI_DRV_VERSION                  /* Character String for DRV/API version             */  \
    MSIF_TAG,                           /* 'MSIF'                                           */  \
    MSIF_CLASS,                         /* '00'                                             */  \
    MSIF_CUS,                           /* 0x0000                                           */  \
    MSIF_MOD,                           /* 0x0000                                           */  \
    MSIF_CHIP,                                                                                  \
    MSIF_CPU,                                                                                   \
    MSIF_HDMI_LIB_CODE  ,                  /* IP__                                             */  \
    MSIF_HDMI_LIBVER    ,                          /* 0.0 ~ Z.Z                                        */  \
    MSIF_HDMI_BUILDNUM  ,                          /* 00 ~ 99                                          */  \
    MSIF_HDMI_CHANGELIST,  /* CL#                                              */  \
    MSIF_OS

#define HDMI_VSDB_PACKET_SIZE 14
#define HDMI_HDR_PACKET_SIZE HDMI_VSDB_PACKET_SIZE

/// Extend packet receive structure version
#define HDMI_EXTEND_PACKET_RECEIVE_VERSION          4U
#define HDMI_EXTEND_PACKET_RECEIVE_VERSION_1_SIZE   30U

/// HDR metadata structure version
#define HDMI_HDR_METADATA_VERSION                   2U
#define HDMI_HDR_METADATA_VERSION_1_SIZE            32U

/// AVI parsing info structure version
#define HDMI_AVI_PARSING_INFO_VERSION               4U
#define HDMI_AVI_PARSING_INFO_VERSION_1_SIZE        25U

/// VS parsing info structure version
#define HDMI_VS_PARSING_INFO_VERSION                2U
#define HDMI_VS_PARSING_INFO_VERSION_1_SIZE         14U

/// GC parsing info structure version
#define HDMI_GC_PARSING_INFO_VERSION                1U
#define HDMI_GC_PARSING_INFO_VERSION_1_SIZE         11U

#define HDMI_GET_PACKET_LENGTH              (32U)
#define HDMI_GET_MULTIVS_PACKET_LENGTH      (121U)

//==============================================================================
//==============================================================================
/* DDC SRAM SEL (After T3) */
#define DDC_RAM_SRAM_DVI                0U
#define DDC_RAM_SRAM_ADC                1U
#define DDC_RAM_SRAM_DVI0               0U
#define DDC_RAM_SRAM_DVI1               1U
#define DDC_RAM_SRAM_DVI2               2U
#define DDC_RAM_SRAM_DVI3               3U

#define DDC_OFFSET_SRAM_DVI0            0U
#define DDC_OFFSET_SRAM_DVI1            4U
#define DDC_OFFSET_SRAM_DVI2            8U
#define DDC_OFFSET_SRAM_DVI3            12U
#define DDC_OFFSET_SRAM_ADC             16U

#define INPUT_PORT_DVI_END              INPUT_PORT_DVI2
#define INPUT_PORT_DVI_MAX              INPUT_PORT_DVI3


//#define ABS_MINUS(a, b)                     ((a > b)? (a -b): (b -a))

#define HDMI_GET_PORT_SELECT(a)             ((MS_U8)(((a >= INPUT_PORT_DVI0) && (a <= INPUT_PORT_DVI2))? (a - INPUT_PORT_DVI0): 0))

//==============================================================================
//==============================================================================

#define SUPPORT_HDMI_PHY_CHANGE19            (FALSE)
#define SUPPORT_HDMI_IMMESWITCH_POWER_SAVING (FALSE)
#define YUV420_NEED_2X_CLOCK                 (TRUE)
#define PHY_H20_CTS_PATCH_FOR_DEMOD_BOARD    1
#define SUPPORT_HDMI_SRAM_POWER_DOWN (TRUE)

#define CED_RSCC_ENHS_HW_MODE
#ifdef CED_RSCC_ENHS_HW_MODE
#define HDMI_CTS_SW_PATCH_CED               0
#define HDMI_CTS_SW_PATCH_RS                0
#else
#define HDMI_CTS_SW_PATCH_CED               0
#define HDMI_CTS_SW_PATCH_RS                0
#endif

#define HDCP_TEE_RIU_WRITE_PROTECT          1

#define HBMASK                              0xFF00
#define LBMASK                              0x00FF

#define TMDS_DIGITAL_LOCK_CNT_POWER         5U
#define TMDS_DIGITAL_LOSE_RANGE             3U

#define TMDS_POWER_SWITCH_IRQ_ENABLE        0U

#define TMDS_CLOCK_CMP_VALUE0               0x06U
#define TMDS_CLOCK_CMP_VALUE1               0x65U

#define TMDS_COARSE_TUNE_14_MIN             0x11U
#define TMDS_COARSE_TUNE_20_MIN             0x00U
#define TMDS_COARSE_TUNE_20_MAX             0x1FU
#define TMDS_COARSE_TUNE_20_START           0x12U
#define TMDS_COARSE_TUNE_14_DETECT_TIME     0xFFU
#define TMDS_COARSE_TUNE_20_DETECT_TIME     0x50U
#define TMDS_COARSE_TUNE_14_AABA_NUMBER     0x50U
#define TMDS_COARSE_TUNE_20_AABA_NUMBER     0x19U
#define TMDS_FINE_TUNE_AABA_14_NUMBER       0x3FFFU
#define TMDS_FINE_TUNE_AABA_20_NUMBER       0x1FFFU
#define TMDS_FINE_TUNE_UNDER_14_THRESHOLD   0x1U
//#define TMDS_FINE_TUNE_UNDER_20_THRESHOLD   0x1FFU
#define TMDS_FINE_TUNE_UNDER_20_THRESHOLD1  0x9FFU
#define TMDS_CONTINUE_START                 0xFU
#define TMDS_CONTINUOUS_NUMBER              0x350U

#define HDMI_UNDER_150M_EQ_SETTING_VALUE    3U
#define HDMI_OVER_150M_EQ_SETTING_VALUE     7U
//#define HDMI_EQ_14_SETTING_VALUE            20U
//#define HDMI_EQ_14_LOW_SETTING_VALUE        10U
#define HDMI_EQ_20_SETTING_VALUE            0x19U

#define HDMI_MHL_EQ_SETTING_VALUE           4U

//#define TMDS_SCDC_ACCESS_ADDERSS_A8         BIT(6)

#define HDMI_AUTO_EQ_CHECK_INTERVAL         30U

#define HDMI_EQ_CAL_TRIGGER_COUNT           1000U
#define HDMI_EQ_CAL_MEASURE_COUNT           250U

#define TMDS_5V_DETECT_GPIO_ENABLE          1
#define TMDS_POST_EQ_FUNCTION_ENABLE        0
#define TMDS_ACDR_AUTO_FUNCTION_ENABLE      0
#define TMDS_POWER_SAVING_ENABLE            0
#define TMDS_SW_DETECT_SCDC_SCRAMBLE        0
#define TMDS_HDMI21_LANE3_POWER_DOWN        0

#define TMDS_DE_FILTER_HDMI14_VALUE         0xCU
#define TMDS_DE_FILTER_HDMI20_VALUE         0x5U

#define TMDS_HDCP_WINDOW_END_VALUE          0x3U
#define TMDS_HDCP2_SOURCE_READ_OFFSET       130U

#define TMDS_14_CHECK_ERROR_INTERVAL        0x1U
#define TMDS_14_CHECK_ERROR_TIMES           4500//ms//0x80U
#define TMDS_14_RETRIG_UNDER_75M_TIMES      5
#define TMDS_14_RETRIG_75_TO_135M_TIMES     5
#define TMDS_14_RETRIG_OVER_135M_TIMES      5
#define TMDS_20_CHECK_ERROR_INTERVAL        0x1U
#define TMDS_20_CHECK_ERROR_TIMES           1700//ms//0x80U
#define TMDS_20_CHECK_ERROR_TIMES_CTS_PATCH 0x100U
#define TMDS_20_ERROR_HAPPEN_COUNT          3U
#define TMDS_20_ERROR_HAPPEN_DCDR_COUNT     10U
#define TMDS_20_MODE_CHANGE_COUNT           8U
#define TMDS_20_RETRIGGER_TIMES_2P1PORT     6U
#define TMDS_20_RETRIGGER_TIMES             5U
#define TMDS_21_RETRIGGER_TIMES             5U
#define TMDS_21_DE_STABLE_COUNT             5U
#define TMDS_21_CHECK_ERROR_TIMES           1000//ms//10U //0x80U
#define TMDS_AUTO_EQ_PROCESS_INTERVAL       500U
#define TMDS_AUTO_EQ_BGPROCESS_INTERVAL     100U
#define TMDS_AUTO_EQ_VERIFY_COUNT           10U
#define TMDS_FORCE_ACDR_CHECK_INTERVAL      2U
#define HDMI_VDAC_DLEV_COUNT_H20            6U  //24mv
#define HDMI_VDAC_DLEV_COUNT_PORT20         0x20U
#define HDMI_VDAC_DLEV_COUNT_PORT21         0x10U
#define TMDS_CLEAR_ERROR_STATUS_COUNT       2U

#define HDMI_DECODE_ERROR_THRESHOLD14       0x8001U
#define HDMI_DECODE_ERROR_THRESHOLD20       0x8000U
#define HDMI_AUTO_EQ_ENABLE_THRESHOLD       140U
#define HDMI_AUTO_EQ_MEDIUM_THRESHOLD       (200U)
#define HDMI_AUTO_EQ_LOW_THRESHOLD          (150U)
#define HDMI_EQ_FIXED_LOW_THRESHOLD         75U
#define HDMI_CLOCK_UNSTABLE_OFFSET          4U
#define HDMI_SCAN_PHASE_THRESHOLD           (32U)
#define HDMI_SCAN_PHASE_THRESHOLD_14        37U
#define HDMI_SCAN_PHASE_THRESHOLD_20        30U
#define HDMI_SCAN_PHASE_THRESHOLD_14_2P1PORT 50U
#define HDMI_SCAN_PHASE_THRESHOLD_20_2P1PORT 32U

#define HDMI_CHECK_SCRAMBLE_INTERVAL        20U
#define HDMI_POWER_SAVING_CHECK_INTERVAL    500U
#define HDMI_POWER_SAVING_CHECK_COUNT       200U
#define HDMI_HDCP_AUTH_CHECK_INTERVAL       2000U
#define HDMI_LANE_LENGTH                    4U
#define HDMI_SCDC_CED_LENGTH                9U
#define HDMI_RED_RMD_RECORD_LENGTH          10U
#define HDMI_RSCC_RECORD_LENGTH             10U

#define HDMI_EDID_SUPPORT_128               128U
#define HDMI_EDID_SUPPORT_256               256U
#define HDMI_EDID_SUPPORT_384               384U
#define HDMI_EDID_SUPPORT_512               512U

#define HDMI_EDID_SUPPORT_SIZE              HDMI_EDID_SUPPORT_512

#define HDMI_EDID_0BLOCK                    (0)
#define HDMI_EDID_1BLOCK                    (1)
#define HDMI_EDID_2BLOCK                    (2)
#define HDMI_EDID_3BLOCK                    (3)
#define HDMI_EDID_4BLOCK                    (4)
#define HDMI_EDID_5BLOCK                    (5)

#define HDMI_EDID_EXTENSION_FLAG_ADDRESS    (0x7EU)

#define HDMI_VCO_DVI40_CLK_THR7_21          0x02FAU
#define HDMI_VCO_DVI40_CLK_THR6_21          0x02BDU
#define HDMI_VCO_DVI40_CLK_THR5_21          0x025DU
#define HDMI_VCO_DVI40_CLK_THR4_21          0x01FAU
#define HDMI_VCO_DVI40_CLK_THR3_21          0x01A0U
#define HDMI_VCO_DVI40_CLK_THR2_21          0x013AU
#define HDMI_VCO_DVI40_CLK_THR1_21          0x00E8U

#define HDMI_VCO_DVI40_CLK_THR7_20          0x05F5U
#define HDMI_VCO_DVI40_CLK_THR6_20          0x057AU
#define HDMI_VCO_DVI40_CLK_THR5_20          0x04BAU
#define HDMI_VCO_DVI40_CLK_THR4_20          0x03F5U
#define HDMI_VCO_DVI40_CLK_THR3_20          0x0340U
#define HDMI_VCO_DVI40_CLK_THR2_20          0x0275U
#define HDMI_VCO_DVI40_CLK_THR1_20          0x01D0U

#define HDMI_VCO_DVI40_CLK_THR7_14_HALF     0x1FFAU
#define HDMI_VCO_DVI40_CLK_THR6_14_HALF     0x1FFAU
#define HDMI_VCO_DVI40_CLK_THR5_14_HALF     0x1FFAU
#define HDMI_VCO_DVI40_CLK_THR4_14_HALF     0x0FD5U
#define HDMI_VCO_DVI40_CLK_THR3_14_HALF     0x0D00U
#define HDMI_VCO_DVI40_CLK_THR2_14_HALF     0x09D5U
#define HDMI_VCO_DVI40_CLK_THR1_14_HALF     0x0740U

#define HDMI_VCO_DVI40_CLK_THR7_14_FULL     0x0BEAU
#define HDMI_VCO_DVI40_CLK_THR6_14_FULL     0x0AF5U
#define HDMI_VCO_DVI40_CLK_THR5_14_FULL     0x0975U
#define HDMI_VCO_DVI40_CLK_THR4_14_FULL     0x07EAU
#define HDMI_VCO_DVI40_CLK_THR3_14_FULL     0x0680U
#define HDMI_VCO_DVI40_CLK_THR2_14_FULL     0x04EAU
#define HDMI_VCO_DVI40_CLK_THR1_14_FULL     0x03A0U

#define HDMI_VCO_DVI40_CLK_THR7_14_2X       0x05F5U
#define HDMI_VCO_DVI40_CLK_THR6_14_2X       0x057AU
#define HDMI_VCO_DVI40_CLK_THR5_14_2X       0x04BAU
#define HDMI_VCO_DVI40_CLK_THR4_14_2X       0x03F5U
#define HDMI_VCO_DVI40_CLK_THR3_14_2X       0x0340U
#define HDMI_VCO_DVI40_CLK_THR2_14_2X       0x0275U
#define HDMI_VCO_DVI40_CLK_THR1_14_2X       0x01D0U

#define HDMI_VCO_DVI40_CLK_THR7_14_4X       0x02FAU
#define HDMI_VCO_DVI40_CLK_THR6_14_4X       0x02BDU
#define HDMI_VCO_DVI40_CLK_THR5_14_4X       0x025DU
#define HDMI_VCO_DVI40_CLK_THR4_14_4X       0x01FAU
#define HDMI_VCO_DVI40_CLK_THR3_14_4X       0x01A0U
#define HDMI_VCO_DVI40_CLK_THR2_14_4X       0x013AU
#define HDMI_VCO_DVI40_CLK_THR1_14_4X       0x00E8U

#define HDMI_CLOCK_RATE_FRL_3G_MIDDLE       0x0C22U
#define HDMI_CLOCK_RATE_FRL_6G_MIDDLE       0x1844U
#define HDMI_CLOCK_RATE_FRL_8G_MIDDLE       0x0FB0U
#define HDMI_CLOCK_RATE_FRL_10G_MIDDLE      0x139CU
#define HDMI_CLOCK_RATE_FRL_12G_MIDDLE      0x1788U
#define HDMI_CLOCK_RATE_FRL_OFFSET          0x3U

#define GET_HDMI_FLAG(a, b)                 (MS_BOOL)((a &b) ?TRUE :FALSE)
#define SET_HDMI_FLAG(a, b)                 (a |= b)
#define CLR_HDMI_FLAG(a, b)                 (a &= ~b)

#define HDMI_SCDC_ADDRESS_10                0x10U
#define HDMI_SCDC_ADDRESS_20                0x20U
#define HDMI_SCDC_ADDRESS_21                0x21U
#define HDMI_SCDC_ADDRESS_30                0x30U
#define HDMI_SCDC_ADDRESS_31                0x31U
#define HDMI_SCDC_ADDRESS_40                0x40U
#define HDMI_SCDC_ADDRESS_41                0x41U
#define HDMI_SCDC_ADDRESS_42                0x42U
#define HDMI_SCDC_ADDRESS_50                0x50U
#define HDMI_SCDC_ADDRESS_51                0x51U
#define HDMI_SCDC_ADDRESS_52                0x52U
#define HDMI_SCDC_ADDRESS_53                0x53U
#define HDMI_SCDC_ADDRESS_54                0x54U
#define HDMI_SCDC_ADDRESS_55                0x55U
#define HDMI_SCDC_ADDRESS_56                0x56U
#define HDMI_SCDC_ADDRESS_57                0x57U
#define HDMI_SCDC_ADDRESS_58                0x58U
#define HDMI_SCDC_ADDRESS_59                0x59U
#define HDMI_SCDC_ADDRESS_5A                0x5AU

typedef struct {
    MS_U32 u32RsccTimerRecord[HDMI_RSCC_RECORD_LENGTH];
    MS_U16 u16RsccCountRecord[HDMI_RSCC_RECORD_LENGTH];
    MS_U8 u8RsccRecordCnt;
    MS_U8 u8RsccRecord_valid_n;
}s_rsed_update;

typedef struct
{
    MS_BOOL bNoInputFlag;
    MS_BOOL bHDMI20Flag;
    MS_BOOL bYUV420Flag;
    MS_BOOL bHDMIModeFlag;
    MS_BOOL bAutoEQEnable;
    MS_BOOL bAutoEQRetrigger;
    MS_BOOL bTimingStableFlag;
    MS_BOOL bPowerOnLane;
    MS_BOOL bPowerControlOn;
    MS_BOOL bForcePowerDown;
    MS_BOOL bIsRepeater;
    MS_BOOL bPowerSavingFlag;
    MS_BOOL bDCDRDoneFlag;
    MS_BOOL bFRLTrainingFlag;
    MS_BOOL bDDCEnableFlag;
    MS_BOOL bDEChangeFlag;
    MS_BOOL bClockStableFlag;
    MS_BOOL bVersionChangeFlag;
    MS_BOOL bActiveCableFlag;
    MS_BOOL bEQError0[HDMI_LANE_LENGTH];
    MS_BOOL bEQError1[HDMI_LANE_LENGTH];
    MS_BOOL bEQError2[HDMI_LANE_LENGTH];
    MS_BOOL bFRLTrainDoneFlag;
    MS_BOOL bResetDCFifoFlag;
    MS_BOOL bClearRedRmdErrorFlag;
    MS_BOOL bClearRSCCFlag;
    MS_BOOL bFLTNoRetrainFlag;
    MS_BOOL bFrameRateCHGFlag;
    MS_BOOL bPreInterlace;
    MS_BOOL bPreCableDetected;
    // MS_BOOL bTogglePinCtrlTriggered;
    MS_U8 ucSourceVersion;
    MS_U8 ucHDCPState;
    MS_U8 ucHDCPInt;
    MS_U8 ucHDMIInfoSource;
    MS_U8 u8FRLRate;
    MS_U8 u8LaneCnt;
    MS_U8 u8SCDCCEDCnt;
    MS_U8 u8FastTrainingState;
    MS_U8 u8AutoEQState;
    MS_U8 u8AutoEQ14Mode;
    MS_U8 u8CheckErrorInterval;
    MS_U8 u8RatioDetect;
    MS_U8 u8ActiveCableState;
    MS_U8 u8TryNegativeC;
    MS_U8 u8ActiveCableClearErrorStatusCount;
    MS_U8 u8ChangePHYCase;
    MS_U8 u8ClearErrorStatusCount;
    MS_U8 u8EQValue0[HDMI_LANE_LENGTH];
    MS_U8 u8EQValue1[HDMI_LANE_LENGTH];
    MS_U8 u8EQValue2[HDMI_LANE_LENGTH];
    MS_U8 u8RetriggerCount14;
    MS_U8 u8RetriggerCount20;
    MS_U8 u8RetriggerCount21;
    MS_U8 u8EQDoneCount21;
    MS_U8 u8RetriggerH14Under75M;
    MS_U8 u8H21CheckDEStableCount;
    MS_U8 u8RedRmdRecordCnt[HDMI_LANE_LENGTH];
    MS_U8 u8SCDCCEDChecksum;
    MS_U8 u8AccessSCDC50Flag;
    MS_U8 u8SCDC50ReadCnt;
    MS_U8 u8SCDCIRQAddr;
    MS_U8 u8ErrorStatus;
    // MS_U8 u8HDMILogLevel;
    MS_U8 u8AccessSCDC59Flag;
    MS_U8 u8FrameRateCnt;
    MS_U8 u8ActivePort;
    MS_U8 u8RegenCount;
    MS_U16 u16PreHTT;
    MS_U16 u16PreVTT;
    MS_U16 u16PreVDE;
    MS_U16 u16PreVSB;
    MS_U16 usClockCount;
    MS_U16 u16CheckErrorCount;
    MS_U32 u32CheckErrorTime;
    //MS_U16 u16H20CheckErrorTimes;
    MS_U16 u16AutoEQProcCounter;
    MS_U16 u16ActiveCableCheckErrorCount;
    MS_U16 u16EMPacketInfo;
    MS_U16 u16CEDTimer;
    MS_U32 u32RedRmdTimerRecord[HDMI_LANE_LENGTH][HDMI_RED_RMD_RECORD_LENGTH];
    MS_U16 u16RedRmdCountRecord[HDMI_LANE_LENGTH][HDMI_RED_RMD_RECORD_LENGTH];
    MS_U16 u16RedRmdCountPre[HDMI_LANE_LENGTH];
    MS_U16 u16RedRmdCountReport[HDMI_LANE_LENGTH];
    MS_U16 u16FecDecCountPre;
    MS_U16 u16FecDecCountReport;
    MS_U32 u32TMDSPLLPixelFrequency;
    MS_U8 u8Colordepth;
    MS_U8 u8DVIMuxSelect;
    MS_U8 u8RedRmdRecord_valid_n;
    MS_U8 u8SCDC59ReadCnt;
    MS_U16 u16TriggerPinControlCounter;
    MS_U16 u16RedRmdLastReg[HDMI_LANE_LENGTH];
    MS_U16 u16FecLastReg;
    s_rsed_update s_rsed_update_a;
    MS_U32 u32_isr_evt;
    MS_U8 u8Dsc;
    MS_U32 u8RegenReDoDebounceCnt;
    MS_U8 u8RegenFsm;
    MS_U8 u8dsc_engine;
    MS_U8 u8dvi2miu_select;
    MS_U8 u8DSCFifoErrorRegenCnt;
    MS_U8 u8HDMISetHPD;
#ifdef AUTO_RATIO_EXP
    MS_U8 u8WaitCnt;
    MS_BOOL bNormalFinish;
    MS_U8 u8ClockStableCnt;
    MS_U8 u8Ratio_ScrambleDet;
#endif
    MS_U8 u8_prev_repetition;

    MS_U32 u32AudioPreRstTime;
    MS_U16 u16DEStableCnt;

} stHDMI_POLLING_INFO;

typedef struct
{
    MS_U8 ucPowerSavingState;
    MS_U16 usCase1FailCount;
    MS_U16 usCase2FailCount;
    MS_U16 usCase3FailCount;
    MS_U16 usPowerSavingTime;
    MS_U16 usPowerSavingCount;
} stHDMI_POWER_SAVING_INFO;

typedef struct
{
    MS_BOOL bWordBoundaryErrorL0;
    MS_BOOL bWordBoundaryErrorL1;
    MS_BOOL bWordBoundaryErrorL2;
    MS_BOOL bWordBoundaryErrorL3;
    MS_U16 u16DecodeErrorL0;
    MS_U16 u16DecodeErrorL1;
    MS_U16 u16DecodeErrorL2;
    MS_U16 u16DecodeErrorL3;
}stHDMI_ERROR_STATUS;

typedef struct
{
    MS_U8 u8AudioMux;
    MS_U8 u8AudioStatusEnable;
    MS_U8 u8AudioMuteEnable;
    MS_U32 u32AudioInfo_CTS;
    MS_U32 u32AudioInfo_N;
    MS_U32 u32ClockRates;
    MS_U32 u32AudioInfo_FS;
} stHDMI_AUDIO_INFO;

typedef struct
{
    MS_U16 u16HDCP22HwControlStatus;
    MS_U16 u16HDCP14IrqStatus;
} stHDMI_HDCP_INFO;

typedef enum
{
    HDMI_SIGNAL_PIX_MHZ   = 0,
    HDMI_SIGNAL_PIX_10KHZ,
    HDMI_SIGNAL_PIX_HZ   ,
}HDMI_PIX_CLK_TYPE;

enum HDMI_ACTIVE_CABLE_TYPE
{
    HDMI_ACTIVE_CABLE_NONE = 0,
    HDMI_ACTIVE_CABLE_START,
    HDMI_ACTIVE_CABLE_CHECK_DLEV,
    HDMI_ACTIVE_CABLE_CHECK_PHASE,
    HDMI_ACTIVE_CABLE_EQ_FLOW_H14,
    HDMI_ACTIVE_CABLE_WAIT_EQ_FLOW_DONE_H14,
    HDMI_ACTIVE_CABLE_CHECK_STABLE_H14,
    HDMI_ACTIVE_CABLE_EQ_FLOW_H20,
    HDMI_ACTIVE_CABLE_WAIT_EQ_FLOW_DONE_H20,
    HDMI_ACTIVE_CABLE_CHECK_STABLE_H20,
    HDMI_ACTIVE_CABLE_FINISH,
};

enum HDMI_CHANGE_PHY_TYPE
{
    HDMI_CHANGE_PHY_NONE = 0,
    HDMI_CHANGE_PHY_H14_CASE0,
    HDMI_CHANGE_PHY_H14_CASE1,
    HDMI_CHANGE_PHY_H14_CASE2,
    HDMI_CHANGE_PHY_H14_CASE3,
    HDMI_CHANGE_PHY_H14_CASE4,
    HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE0,
    HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE1,
    HDMI_CHANGE_PHY_H14_75M_TO_135M_CASE2,
    HDMI_CHANGE_PHY_H14_UNDER_75M_CASE0,
    HDMI_CHANGE_PHY_H20_CASE0,
    HDMI_CHANGE_PHY_H20_CASE1,
    HDMI_CHANGE_PHY_H20_CASE2,
    HDMI_CHANGE_PHY_H21_CASE0,
    HDMI_CHANGE_PHY_FINISH,
};

enum HDMI_FAST_TRAINING_TYPE
{
    HDMI_FAST_TRAINING_NONE = 0,
    HDMI_FAST_TRAINING_START,
    HDMI_FAST_TRAINING_SQUELCH,
    HDMI_FAST_TRAINING_CHECK_AUTO_EQ,
    HDMI_FAST_TRAINING_CHECK_ACTIVE_CABLE,
    HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H14,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H14,
    HDMI_FAST_TRAINING_CHECK_EQ0_H14,
    HDMI_FAST_TRAINING_CHECK_EQ1_H14,
    HDMI_FAST_TRAINING_CHECK_EQ2_H14,
    HDMI_FAST_TRAINING_CHECK_STABLE_H14,
    HDMI_FAST_TRAINING_TRY_NEGATIVE_C_H14,
    HDMI_FAST_TRAINING_CHANGE_PHY_H14,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H14_75M_TO_135M,
    HDMI_FAST_TRAINING_CHECK_STABLE_H14_75M_TO_135M,
    HDMI_FAST_TRAINING_CHANGE_PHY_H14_75M_TO_135M,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H14_UNDER_75M,
    HDMI_FAST_TRAINING_CHECK_STABLE_H14_UNDER_75M,
    HDMI_FAST_TRAINING_CHANGE_PHY_H14_UNDER_75M,
    HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20,
    HDMI_FAST_TRAINING_FORCE_PGA_PATCH_H20,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H20,
    HDMI_FAST_TRAINING_CHECK_STABLE_H20,
    HDMI_FAST_TRAINING_CHANGE_PHY_H20,
    HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H21,
    HDMI_FAST_TRAINING_CLEAR_DE_UNSTABLE_H21,
    HDMI_FAST_TRAINING_CHECK_DE_STABLE_H21,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H21,
    HDMI_FAST_TRAINING_CHECK_STABLE_H21,
    HDMI_FAST_TRAINING_CHANGE_PHY_H21,
    HDMI_FAST_TRAINING_STOP,
    HDMI_FAST_TRAINING_FINISH,
#ifdef AUTO_RATIO_EXP
    HDMI_FAST_TRAINING_AUTO_RATIO_DET,
    HDMI_FAST_TRAINING_AUTO_RATIO_CLOCK_STABLE,
#endif
};

typedef enum
{
    HDMI_14_TMDS_CLOCK_NONE = 0,
    HDMI_14_TMDS_CLOCK_UNDER_75M,
    HDMI_14_TMDS_CLOCK_75M_TO_135M,
    HDMI_14_TMDS_CLOCK_OVER_135M,
}EN_HDMI_14_TMDS_CLOCK_RANGE;

typedef enum
{
    HDMI_TMDS_CLOCK_H14_BAND0 = 0,
    HDMI_TMDS_CLOCK_H14_BAND1,
    HDMI_TMDS_CLOCK_H14_BAND2,
    HDMI_TMDS_CLOCK_H14_BAND3,
    HDMI_TMDS_CLOCK_H14_BAND4,
    HDMI_TMDS_CLOCK_H14_BAND5,
    HDMI_TMDS_CLOCK_H14_BAND6,
    HDMI_TMDS_CLOCK_H20_BAND0,
    HDMI_TMDS_CLOCK_H20_BAND1,
    HDMI_TMDS_CLOCK_H20_BAND2,
    HDMI_TMDS_CLOCK_H20_BAND3,
    HDMI_TMDS_CLOCK_H21_BAND3G,
    HDMI_TMDS_CLOCK_H21_BAND6G,
    HDMI_TMDS_CLOCK_H21_BAND8G,
    HDMI_TMDS_CLOCK_H21_BAND10G,
    HDMI_TMDS_CLOCK_H21_BAND12G,
    HDMI_TMDS_CLOCK_NUMOF_BAND,
}EN_HDMI_TMDS_CLOCK_DETECT_BAND;

enum HDMI_FAST_TRAINING_RESULT_TYPE
{
    HDMI_FAST_TRAINING_RESULT_NONE = 0,
    HDMI_FAST_TRAINING_RESULT_MULTI_TRIGGER,
    HDMI_FAST_TRAINING_RESULT_IN_FRL,
    HDMI_FAST_TRAINING_RESULT_STABLE_READY,
    HDMI_FAST_TRAINING_RESULT_ALL_CASE_CHECK,
    HDMI_FAST_TRAINING_RESULT_PASS,
};

enum HDMI_FRL_TRAINING_STATE_TYPE
{
    HDMI_FRL_TRAINING_STATE_IDLE = 0,
    HDMI_FRL_TRAINING_STATE_CR_DONE,
    HDMI_FRL_TRAINING_STATE_EQ_DONE,
    HDMI_FRL_TRAINING_STATE_NORMAL_DONE,
    HDMI_FRL_TRAINING_STATE_NONE,
};

enum HDMI_FRL_MODE_TYPE
{
    HDMI_FRL_MODE_LEGACY = 0,
    HDMI_FRL_MODE_FRL_3G_3CH,
    HDMI_FRL_MODE_FRL_6G_3CH,
    HDMI_FRL_MODE_FRL_6G_4CH,
    HDMI_FRL_MODE_FRL_8G,
    HDMI_FRL_MODE_FRL_10G,
    HDMI_FRL_MODE_FRL_12G,
    HDMI_FRL_MODE_NONE,
    HDMI_FRL_MODE_LEGACY_14,
    HDMI_FRL_MODE_LEGACY_20,
};

enum HDMI_AUTO_EQ_STATE_TYPE
{
    HDMI_AUTO_EQ_START = 0,
    HDMI_AUTO_EQ_WAIT_DONE,
    HDMI_AUTO_EQ_FINISH_PROCESS,
    HDMI_AUTO_EQ_CHECK_STABLE,
    HDMI_AUTO_EQ_CHECK_DONE,
    HDMI_AUTO_EQ_14_CLEAR_STATUS,
    HDMI_AUTO_EQ_14_CHECK_STATUS,
    HDMI_AUTO_EQ_14_CHANGE_SETTING,
    HDMI_AUTO_EQ_20_CLEAR_STATUS,
    HDMI_AUTO_EQ_20_CHECK_STATUS,
    HDMI_AUTO_EQ_20_CHANGE_SETTING,
    HDMI_AUTO_EQ_DCDR_POLLING,
    HDMI_AUTO_EQ_STABLE_DONE,
    HDMI_AUTO_EQ_NONE,
    HDMI_AUTO_EQ_20_CHANGE_ACDR,
    HDMI_AUTO_EQ_FORCE_INTO_ACDR,
    HDMI_AUTO_EQ_20_ACDR_WAIT_DONE,
    HDMI_AUTO_EQ_20_DCDR_WAIT_DONE,
    HDMI_AUTO_EQ_20_DCDR_WAIT_STABLE,
};

//enum HDMI_SOURCE_VERSION_TYPE
//{
//    HDMI_SOURCE_VERSION_NOT_SURE = 0,
//    HDMI_SOURCE_VERSION_HDMI14,
//    HDMI_SOURCE_VERSION_HDMI20,
//    HDMI_SOURCE_VERSION_HDMI21,
//};

enum HDMI_HDCP_STATE
{
    HDMI_HDCP_NO_ENCRYPTION = 0,
    HDMI_HDCP_1_4,
    HDMI_HDCP_2_2,
};

enum HDMI_HDCP_ENCRYPTION_STATE
{
    HDMI_HDCP_NOT_ENCRYPTION = 0,
    HDMI_HDCP_1_4_ENCRYPTION,
    HDMI_HDCP_2_2_ENCRYPTION,
};

enum HDMI_5V_DETECT_GPIO_TYPE
{
    HDMI_5V_DETECT_GPIO_NONE = 0,
    HDMI_5V_DETECT_GPIO_INDEX00,
    HDMI_5V_DETECT_GPIO_INDEX01,
    HDMI_5V_DETECT_GPIO_INDEX02,
    HDMI_5V_DETECT_GPIO_INDEX03,
    HDMI_5V_DETECT_GPIO_INDEX04,
    HDMI_5V_DETECT_GPIO_INDEX05,
    HDMI_5V_DETECT_GPIO_INDEX06,
    HDMI_5V_DETECT_GPIO_INDEX07,
    HDMI_5V_DETECT_GPIO_INDEX08,
    HDMI_5V_DETECT_GPIO_INDEX09,
    HDMI_5V_DETECT_GPIO_INDEX10,
    HDMI_5V_DETECT_GPIO_INDEX11,
    HDMI_5V_DETECT_GPIO_INDEX12,
    HDMI_5V_DETECT_GPIO_INDEX13,
    HDMI_5V_DETECT_GPIO_INDEX14,
    HDMI_5V_DETECT_GPIO_INDEX15,
    HDMI_5V_DETECT_GPIO_INDEX16,
    HDMI_5V_DETECT_GPIO_INDEX17,
    HDMI_5V_DETECT_GPIO_INDEX18,
    HDMI_5V_DETECT_GPIO_INDEX19,
    HDMI_5V_DETECT_GPIO_INDEX20,
    HDMI_5V_DETECT_GPIO_INDEX21,
    HDMI_5V_DETECT_GPIO_INDEX22,
    HDMI_5V_DETECT_GPIO_INDEX23,
    HDMI_5V_DETECT_GPIO_INDEX24,
    HDMI_5V_DETECT_GPIO_INDEX25,
};

enum HDMI_AUTO_EQ_14_MODE_TYPE
{
    HDMI_AUTO_EQ_14_MODE0 = 0,
    HDMI_AUTO_EQ_14_MODE1,
    HDMI_AUTO_EQ_14_MODE2,
    HDMI_AUTO_EQ_14_MODE3,
    HDMI_AUTO_EQ_14_MODE_MASK,
};

enum HDMI_AUTO_EQ_20_MODE_TYPE
{
    HDMI_AUTO_EQ_20_MODE0 = 0,
    HDMI_AUTO_EQ_20_MODE1,
    HDMI_AUTO_EQ_20_MODE2,
    HDMI_AUTO_EQ_20_MODE_MASK,
};

enum HDMI_STATUS_FLAG_TYPE
{
    HDMI_STATUS_MPEG_PACKET_RECEIVE_FLAG = BIT(0),
    HDMI_STATUS_AUDIO_PACKET_RECEIVE_FLAG = BIT(1),
    HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG = BIT(2),
    HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG = BIT(3),
    HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG = BIT(4),
    HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG = BIT(5),
    HDMI_STATUS_ACR_PACKET_RECEIVE_FLAG = BIT(6),
    HDMI_STATUS_VS_PACKET_RECEIVE_FLAG = BIT(7),
    HDMI_STATUS_NULL_PACKET_RECEIVE_FLAG = BIT(8),
    HDMI_STATUS_ISRC2_PACKET_RECEIVE_FLAG = BIT(9),
    HDMI_STATUS_ISRC1_PACKET_RECEIVE_FLAG = BIT(10),
    HDMI_STATUS_ACP_PACKET_RECEIVE_FLAG = BIT(11),
    HDMI_STATUS_DSD_PACKET_RECEIVE_FLAG = BIT(12),
    HDMI_STATUS_GM_PACKET_RECEIVE_FLAG = BIT(13),
    HDMI_STATUS_HBR_PACKET_RECEIVE_FLAG = BIT(14),
    HDMI_STATUS_VBI_PACKET_RECEIVE_FLAG = BIT(15),
    HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG = BIT(16),
    HDMI_STATUS_RESERVED_PACKET_RECEIVE_FLAG = BIT(17),
    HDMI_STATUS_EDR_VALID_FLAG = BIT(18),
    HDMI_STATUS_AUDIO_DST_RECEIVE_FLAG = BIT(19),
    HDMI_STATUS_AUDIO_3D_ASP_RECEIVE_FLAG = BIT(20),
    HDMI_STATUS_AUDIO_3D_DSD_RECEIVE_FLAG = BIT(21),
    HDMI_STATUS_AUDIO_METADATA_RECEIVE_FLAG = BIT(22),
    HDMI_STATUS_AUDIO_MULTI_ASP_RECEIVE_FLAG = BIT(23),
    HDMI_STATUS_AUDIO_MULTI_DSD_RECEIVE_FLAG = BIT(24),
    HDMI_STATUS_EM_PACKET_RECEIVE_FLAG = BIT(25),
};

enum HDMI_INFO_SOURCE_TYPE
{
    HDMI_INFO_SOURCE0 = 0,
    HDMI_INFO_SOURCE1,
    HDMI_INFO_SOURCE2,
   // HDMI_INFO_SOURCE3, //Odinson2 only 3 port
    HDMI_INFO_SOURCE_MAX,
    HDMI_INFO_SOURCE_NONE,
};

enum HDMI_AUDIO_PATH_TYPE
{
    HDMI_AUDIO_PATH0 = 0,
    HDMI_AUDIO_PATH1,
    HDMI_AUDIO_PATH2,
    HDMI_AUDIO_PATH3,
    HDMI_AUDIO_PATH_MAX,
    HDMI_AUDIO_PATH_NONE,
};

enum HDMI_CHECK_PACKET_TYPE
{
    HDMI_CHECK_PACKET_CLEAR_RECEIVE_STATUS = 0,
    HDMI_CHECK_PACKET_WAIT_AVI_PACKET,
    HDMI_CHECK_PACKET_MEASURE_PACKET_INTERVAL,
    HDMI_CHECK_PACKET_UPDATE_RECEIVE_STATUS,
};

enum HDMI_POWER_SAVING_TEST_TYPE
{
    HDMI_POWER_SAVING_START = 0,
    HDMI_POWER_SAVING_ENABLE,
    HDMI_POWER_SAVING_WAIT_FAIL1,
    HDMI_POWER_SAVING_HPD_LOW,
    HDMI_POWER_SAVING_HPD_HIGH,
    HDMI_POWER_SAVING_WAIT_STATUS,
    HDMI_POWER_SAVING_WAIT_FAIL2,
    HDMI_POWER_SAVING_DISABLE,
    HDMI_POWER_SAVING_WAIT_FAIL3,
    HDMI_POWER_SAVING_FINISH,
};

enum HDMI_AUDIO_MUTE_EVENT
{
    HDMI_CTSN_OVERRANGE = BIT(0),
    HDMI_CLOCK_BIG_CHANGE = BIT(1),
    HDMI_AVMUTE = BIT(2),
    HDMI_NO_INPUT_CLOCK= BIT(3),
    HDMI_AUDIO_SAMPLE_FLAT_BIT = BIT(4),
    HDMI_AUDIO_PRESENT_BIT_ERROR = BIT(5),
    HDMI_AUDIO_FIFO_OVERFLOW_UNDERFLOW = BIT(6),
    HDMI_AUDIO_SAMPLE_PARITY_ERROR = BIT(7),
};

enum HDMI_ACDR_CHANNEL_TYPE
{
    HDMI_ACDR_CHANNEL_B = 0,
    HDMI_ACDR_CHANNEL_G,
    HDMI_ACDR_CHANNEL_R,
    HDMI_ACDR_CHANNEL_NONE,
};

enum HDMI_SW_RESET_TYPE
{
    HDMI_SW_RESET_DVI = BIT(4),
    HDMI_SW_RESET_HDCP = BIT(5),
    HDMI_SW_RESET_HDMI = BIT(6),
    HDMI_SW_RESET_DVI2MIU = BIT(10),
    HDMI_SW_RESET_AUDIO_FIFO = BIT(14),
};

enum HDMI_RESET_PHY_DTOP
{
    HDMI_RESET_PHY_DTOP_SYNTH = BIT(0),
    HDMI_RESET_PHY_DTOP_IRQ = BIT(1),
    HDMI_RESET_PHY_DTOP_1ST_FIFO = BIT(2),
    HDMI_RESET_PHY_DTOP_AFIFO = BIT(3),
    HDMI_RESET_PHY_DTOP_40to36_FIFO = BIT(4),
    HDMI_RESET_PHY_DTOP_SQH = BIT(5),
    HDMI_RESET_PHY_DTOP_LANE0 = BIT(6),
    HDMI_RESET_PHY_DTOP_LANE1 = BIT(7),
    HDMI_RESET_PHY_DTOP_LANE2 = BIT(8),
    HDMI_RESET_PHY_DTOP_LANE3 = BIT(9),
    HDMI_RESET_PHY_DTOP_FREQ_DET = BIT(10),
    HDMI_RESET_PHY_DTOP_MISC = BIT(11),
    HDMI_RESET_PHY_DTOP_CLOCK_DET = BIT(12),
    HDMI_RESET_PHY_DTOP_LONG_ZERO_ONE = BIT(13),
};

enum HDMI_FREE_SYNC_TYPE
{
    HDMI_FREE_SYNC_SUPPORTED = BIT(0),
    HDMI_FREE_SYNC_ENABLED = BIT(1),
    HDMI_FREE_SYNC_ACTIVE = BIT(2),
};

enum HDMI_EM_PACKET_STATUS_TYPE
{
    HDMI_EM_PACKET_DSC_ENABLE = BIT(0),
    HDMI_EM_PACKET_VRR_ENABLE = BIT(1),
    HDMI_EM_PACKET_VENDOR_ENABLE = BIT(2),
    HDMI_EM_PACKET_FVA_ENABLE = BIT(3),
    HDMI_EM_PACKET_HDR_ENABLE = BIT(4),
};

enum HDMI_SCDC_INTERRUPT_STATUS
{
    HDMI_SCDC_INT_STATUS_UPDATE =         BIT(0),
    HDMI_SCDC_INT_CED_UPDATE =            BIT(1),
    HDMI_SCDC_INT_RR_TEST =               BIT(2),
    HDMI_SCDC_INT_RSVD =                  BIT(3),
    HDMI_SCDC_INT_CLOCK_STRETCH_OFFSET =  BIT(4),
    HDMI_SCDC_INT_HPD_5V =                BIT(5),
    HDMI_SCDC_INT_ACCESS_A8 =             BIT(6),
    HDMI_SCDC_INT_ACCESS_A9 =             BIT(7),
    HDMI_SCDC_INT_TMDS_CONFIG_R =         BIT(8),
    HDMI_SCDC_INT_FLT_UPDATE =            BIT(9),
    HDMI_SCDC_INT_RSED_UPDATE =           BIT(10),
    HDMI_SCDC_INT_FLT_NO_RETRAIN =        BIT(11),
    HDMI_SCDC_INT_35H_CHG =               BIT(12),
    HDMI_SCDC_INT_FRL_RATE =              BIT(13),
    HDMI_SCDC_INT_31H_W =                 BIT(14),
    HDMI_SCDC_INT_FFE_LEVEL_CHG =         BIT(15),
    HDMI_SCDC_INT_READ_L0 =               BIT(16+1),
    HDMI_SCDC_INT_READ_L1 =               BIT(16+2),
    HDMI_SCDC_INT_READ_L2 =               BIT(16+3),
    HDMI_SCDC_INT_READ_L3 =               BIT(16+4),
    HDMI_SCDC_INT_READ_RS =               BIT(16+5),
};


enum HDMI_COLORDEPTH_TYPE
{
    HDMI_COLOR_DEPTH_6BIT = 0,
    HDMI_COLOR_DEPTH_8BIT,
    HDMI_COLOR_DEPTH_10BIT,
    HDMI_COLOR_DEPTH_12BIT,
    HDMI_COLOR_DEPTH_16BIT,
    HDMI_COLOR_DEPTH_NONE,
};

typedef enum
{
    HDMI_SCDC_INTERNEL_SCRAMBLE = 0,
    HDMI_SCDC_INTERNEL_RATIO,
    HDMI_SCDC_INTERNEL_READ_REQUEST,
    HDMI_SCDC_INTERNEL_FRL_RATE,
    HDMI_SCDC_INTERNEL_FFE_LEVELS,
}EN_HDMI_SCDC_INTERNEL_TYPE;

typedef enum
{
    HDMI_SCDC_SW_MODE_STATUS_UPDATE = BIT(0),  // 0x10[0]
    HDMI_SCDC_SW_MODE_CED_UPDATE = BIT(1),  // 0x10[1] & 0x50~0x58
    HDMI_SCDC_SW_MODE_RR_TEST = BIT(2),  // 0x10[2]
    HDMI_SCDC_SW_MODE_SOURCE_TEST_UPDATE = BIT(3),  // 0x10[3]
    HDMI_SCDC_SW_MODE_FRL_START = BIT(4),  // 0x10[4]
    HDMI_SCDC_SW_MODE_FLT_UPDATE = BIT(5),  // 0x10[5]
    HDMI_SCDC_SW_MODE_RSED_UPDATE = BIT(6),  // 0x10[6]
    HDMI_SCDC_SW_MODE_RSVD = BIT(7),  // 0x10[7]
    HDMI_SCDC_SW_MODE_RSCC = BIT(8), // 0x59, 0x5A
}EN_HDMI_SCDC_SW_MODE;

#define HDMI_PACKET_RECEIVE_INTERVAL_MIN    4
#define HDMI_PACKET_RECEIVE_INTERVAL_MAX    11

typedef struct
{
    MS_U16 u16HTTValue;
    MS_U16 u16HSyncValue;
    MS_U16 u16HSyncBackValue;
    MS_U16 u16HDEValue;
    MS_U16 u16VTTValue;
    MS_U16 u16VSyncValue;
    MS_U16 u16VSyncBackValue;
    MS_U16 u16VDEValue;
    MS_U32 u32FrameRate;
}ST_HDMI_REGEN_TIMING_INFO;

typedef enum
{
    HDMI_DSC_NONE = 0,
    HDMI_DSC_TG0 = 1,
    HDMI_DSC_TG1 = 2,
}HDMI_DSC_ENGINE_SELECT_TYPE;

enum HDMI_DIV_SELECT_TYPE
{
    HDMI_DIV_SELECT_BYPASS = 0,
    HDMI_DIV_SELECT_2,
    HDMI_DIV_SELECT_4,
    HDMI_DIV_SELECT_8,
    HDMI_DIV_SELECT_16,
};

enum
{
    HDMI21_DATA_LANE_0 = 0,
    HDMI21_DATA_LANE_1,
    HDMI21_DATA_LANE_2,
    HDMI21_DATA_LANE_3,
    HDMI21_DATA_LANE_MAX
};

enum
{
    E_H21_SCDC_RD_INT_MODE_MUX_NONE =           0x00,
    E_H21_SCDC_RD_INT_MODE_MUX_ADDR_LSB =       0x01,
    E_H21_SCDC_RD_INT_MODE_MUX_ADDR_MSB =       0x02,
    E_H21_SCDC_RD_INT_MODE_MUX_ADDR_USER_DEF =  0x03
};


enum
{
    E_H21_SCDC_18B16B_RED_CNT_FLD = 0,
    E_H21_SCDC_18B16B_RMD_CNT_FLD = 1,
    E_H21_SCDC_18B16B_SYMBOL_ERR_FLD = 2
};

//CTS:RS
#define DEF_HDMI21_SCDC_RS_ERR_TYPE_NUM         5

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// for HDCP 22 callback function
typedef void (*HDCP22_Recv_CBF)(MS_U8, MS_U8, MS_U8*, MS_U32, void*); //type, portIdx, msg, msg length, context
// for HDCP 14 STR resume callback function
typedef void (*HDCP14_Resume_CBF)(int, void*); //parateter: int: Rx or Tx, context
// for HDCP 22 STR resume callback function
typedef void (*HDCP22_Resume_CBF)(int, void*); //parateter: int: Rx or Tx, context
// for HDCP 14 STR suspend callback function
typedef void (*HDCP14_Suspend_CBF)(int, void*); //parateter: int: Rx or Tx, context
// for HDCP 22 STR suspend callback function
typedef void (*HDCP22_Suspend_CBF)(int, void*); //parateter: int: Rx or Tx, context
// for HDCP 14 SVP Lock callback function
typedef void (*HDCP14_SVPLock_CBF)(int, MS_U8, void*); //parateter: int: Rx or Tx, MS_U8 : PortIdx, context

typedef enum
{
    E_HDMI_INTERFACE_CMD_GET_HDCP_STATE = 0x500, // HDMIRX resource
    /*************************** HDCP Related ***************************/
    E_HDMI_INTERFACE_CMD_WRITE_X74,
    E_HDMI_INTERFACE_CMD_READ_X74,
    E_HDMI_INTERFACE_CMD_SET_REPEATER,
    E_HDMI_INTERFACE_CMD_SET_BSTATUS,
    E_HDMI_INTERFACE_CMD_SET_HDMI_MODE,
    E_HDMI_INTERFACE_CMD_GET_INTERRUPT_STATUS,
    E_HDMI_INTERFACE_CMD_WRITE_KSV_LIST,
    E_HDMI_INTERFACE_CMD_SET_VPRIME,
    /*************************** HDCP Related ***************************/
    E_HDMI_INTERFACE_CMD_DATA_RTERM_CONTROL,
    E_HDMI_INTERFACE_CMD_GET_SCDC_VALUE,
    E_HDMI_INTERFACE_CMD_GET_TMDS_RATES_KHZ,
    E_HDMI_INTERFACE_CMD_GET_SCDC_CABLE_DETECT,
    E_HDMI_INTERFACE_CMD_GET_PACKET_STATUS,
    E_HDMI_INTERFACE_CMD_GET_PACKET_CONTENT,
    E_HDMI_INTERFACE_CMD_GET_HDR_METADATA,
    E_HDMI_INTERFACE_CMD_GET_AVI_PARSING_INFO,
    E_HDMI_INTERFACE_CMD_GET_VS_PARSING_INFO,
    E_HDMI_INTERFACE_CMD_GET_GC_PARSING_INFO,
    E_HDMI_INTERFACE_CMD_GET_TIMING_INFO,
    E_HDMI_INTERFACE_CMD_GET_HDCP_AUTHVERSION,
    E_HDMI_INTERFACE_CMD_FORCE_POWER_DOWN_PORT,
} E_HDMI_CTRL_ID;

/// HDMI reset type
typedef enum
{
    REST_AVMUTE = _BIT(8),    ///<HDMI Reset bit[8]
    REST_Y_COLOR = _BIT(9),    ///<HDMI Reset bit[9]
    REST_PIXEL_REPETITION  = _BIT(10),    ///<HDMI Reset bit[10]
    REST_FRAME_REPETITION  = _BIT(11),    ///<HDMI Reset bit[11]
    REST_GCP_PKT  = _BIT(12),    ///<HDMI Reset bit[12]
    REST_DEEP_COLOR_FIFO  = _BIT(13),    ///<HDMI Reset bit[13]
    REST_RESERVE = _BIT(14),    ///<HDMI Reset bit[14]
    REST_HDMI_STATUS = _BIT(15),    ///<HDMI Reset bit[15]
}HDMI_REST_t;

/*!
    HDMI Aspect Ratio TYPE
*/
typedef enum
{
    // Active Format Aspect Ratio - AFAR
//    HDMI_AF_AR_Not_Present   = -1,    ///< IF0[11..8] AFD not present, or AFD not yet found
    HDMI_AF_AR_Reserve_0  = 0x00,    ///< IF0[11..8] 0000, Reserved
    HDMI_AF_AR_Reserve_1  = 0x01,    ///< IF0[11..8] 0001, Reserved
    HDMI_AF_AR_16x9_Top = 0x02,    ///< IF0[11..8] 0010, box 16:9 (top).
    HDMI_AF_AR_14x9_Top = 0x03,    ///< IF0[11..8] 0011, box 14:9 (top).
    HDMI_AF_AR_GT_16x9 = 0x04,    ///< IF0[11..8] 0100, box >16:9 (centre)
    HDMI_AF_AR_Reserve_5  = 0x05,    ///< IF0[11..8] 0101, Reserved
    HDMI_AF_AR_Reserve_6  = 0x06,    ///< IF0[11..8] 0110, Reserved
    HDMI_AF_AR_Reserve_7  = 0x07,    ///< IF0[11..8] 0111, Reserved
    HDMI_AF_AR_SAME   = 0x08,    ///< IF0[11..8] 1000, same as picture
    HDMI_AF_AR_4x3_C  = 0x09,    ///< IF0[11..8] 1001, 4:3 Center
    HDMI_AF_AR_16x9_C = 0x0A,    ///< IF0[11..8] 1010, 16:9 Center
    HDMI_AF_AR_14x9_C = 0x0B,    ///< IF0[11..8] 1011, 14:9 Center
    HDMI_AF_AR_Reserve_12   = 0x0C,    ///< IF0[11..8] 1100, Reserved.
    HDMI_AF_AR_4x3_with_14x9_C  = 0x0D,    ///< IF0[11..8] 1101, 4:3 with shoot and protect 14:9 centre.
    HDMI_AF_AR_16x9_with_14x9_C = 0x0E,    ///< IF0[11..8] 1110, 16:9 with shoot and protect 14:9 centre.
    HDMI_AF_AR_16x9_with_4x3_C = 0x0F,    ///< IF0[11..8] 1111, 16:9 with shoot and protect 4:3 centre.

    // Picture Aspect Ratio - PAR
    HDMI_Pic_AR_NODATA = 0x00,     ///< IF0[13..12] 00
    HDMI_Pic_AR_4x3    = 0x10,     ///< IF0[13..12] 01, 4:3
    HDMI_Pic_AR_16x9   = 0x20,     ///< IF0[13..12] 10, 16:9
    HDMI_Pic_AR_RSV    = 0x30,     ///< IF0[13..12] 11, reserved
} MS_HDMI_AR_TYPE;

//-------------------------------------------------------------------------------------------------
/// AVI infomation frame version
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_AVI_INFOFRAME_VERSION_NON,    //invalid version
    E_AVI_INFOFRAME_VERSION1,       //AVI infomation frame version 1
    E_AVI_INFOFRAME_VERSION2,       //AVI infomation frame version 2
    E_AVI_INFOFRAME_VERSION3        //AVI infomation frame version 3
}EN_AVI_INFOFRAME_VERSION;

#define HDMI_AR_INIT_VALUE          (HDMI_Pic_AR_RSV)
#define HDMI_AR_REG_MASK            (0x3F)
#define HDMI_AFAR_MASK              (0x0F)      // mask to get the AFAR
#define HDMI_AR_MASK                (0x30)      // mask to get the PAR

/// HDMI GControl package type.
typedef enum
{
    G_CTRL_AVMUTE = 0,    ///< bit[0]: AVMUTE
    G_Ctrl_DEFAULT_PHASE,    ///< bit[1]: Default_Phase
    G_Ctrl_LAST_PP,    ///< bit[4:2]: LAST_PP[2:0]
    G_Ctrl_PRE_LPP,    ///< bit[7:5]: PRE_LPP[2:0]
    G_Ctrl_CD_VAL,    ///< bit[11:8]: CD_VAL[3:0]
    G_Ctrl_PP_VAL,    ///< bit[15:12]: PP_VAL[3:0]
    G_Ctrl_ALL
}HDMI_GControl_INFO_t;

/// HDMI PLL control type
typedef enum
{
    PLL_CTRL_L=0,    ///< HDMI_PLL_CTRL1_L  7:0
    PLL_CTRL_MANUAL_DIV,    ///< MANUAL_PLL_DIV    7
    PLL_CTRL_PORST,    ///< PLL_PORST 6
    PLL_CTRL_RESET_TP,    ///< RESET_TP 5
    PLL_CTRL_RESET_TF,    ///< RESET_TF 4
    PLL_CTRL_RESET_TI,    ///< RESET_TI 3
    PLL_CTRL_VCO_OFFSET,    ///< VCO_OFFSET    2
    PLL_CTRL_RESET,    ///< PLL_RESET 1
    PLL_CTRL_PWRDOWN,    ///< PWRDOWN  0

    PLL_CTRL_H,    ///< HDMI_PLL_CTRL1_H 7:0
    PLL_CTRL_KN,    ///< KN[1:0]    7:6
    PLL_CTRL_RCTRL,    ///< RCTRL 5:3
    PLL_CTRL_ICTRL,    ///< ICTRL 2:0
}HDMI_PLL_CTRL_t;

/// HDMI PLL control type
typedef enum
{
    PLL_CTRL2_L,    ///< HDMI_PLL_CTRL2_L 7:0
    PLL_CTRL2_KP,    ///< KP[3:0]   7:4
    PLL_CTRL2_KM,    ///< KM[3:0]   3:0

    PLL_CTRL2_H,    ///< HDMI_PLL_CTRL2_H  7:0
    PLL_CTRL2_DDIV,    ///< DDIV[3:0] 7:4
    PLL_CTRL2_FBDIV,    ///< FBDIV[3:0]    3:0
}HDMI_PLL_CTRL2_t;

////===========================>>HDMI
typedef struct
{
    MS_BOOL bIsHDMIMode;
    MS_BOOL bMuteHDMIVideo;

    MS_BOOL bMuteHDMIAudio;
    MS_BOOL bEnableNonPCM;
    MS_U8      u8AudioStbCnt;      // If stable more than HDMI_AUDIO_STABLE_CNT, enable audio
    // In 199M case, audio on->off->on->off->on, so, we wait 4 * HDMI_AUDIO_STABLE_CNT, if unstable,
    // enable audio still to prevent mute audio always because of un-stable
    MS_U8      u8AudioForceEnableCnt;

    MS_U8      u8ColorFormat;
    MS_U8      u8LostHDMICounter;
    MS_BOOL bColorFMTChange;
    MS_U8      u8PreClolrFormat;
    MS_U8      u8AspectRatio;
} HDMI_POLLING_STATUS_t;

typedef enum
{
    MS_DVI_CHANNEL_R,
    MS_DVI_CHANNEL_G,
    MS_DVI_CHANNEL_B,
} MS_DVI_CHANNEL_TYPE;

/// HDMI COLOR FORMAT
typedef enum
{
    MS_HDMI_COLOR_RGB,    ///< HDMI RGB 444 Color Format
    MS_HDMI_COLOR_YUV_422,    ///< HDMI YUV 422 Color Format
    MS_HDMI_COLOR_YUV_444,    ///< HDMI YUV 444 Color Format
    MS_HDMI_COLOR_YUV_420,    ///< HDMI YUV 420 Color Format
    MS_HDMI_COLOR_RESERVED,    ///< Reserve
    MS_HDMI_COLOR_DEFAULT = MS_HDMI_COLOR_RGB,    ///< Default setting
    MS_HDMI_COLOR_UNKNOWN = 15,    ///< Unknow Color Format
} MS_HDMI_COLOR_FORMAT;

/// HDMI COLORIMETRY
typedef enum
{
    MS_HDMI_COLORIMETRY_NO_DATA,
    MS_HDMI_COLORIMETRY_SMPTE_170M,
    MS_HDMI_COLORIMETRY_ITU_R_BT_709,
    MS_HDMI_COLORIMETRY_DEFAULT = MS_HDMI_COLORIMETRY_NO_DATA,    ///< Default setting
    MS_HDMI_COLORIMETRY_UNKNOWN = 3,    ///< Unknow
} MS_HDMI_COLORIMETRY_FORMAT;

/// HDMI Extended COLORIMETRY
typedef enum
{
    MS_HDMI_EXT_COLOR_XVYCC601,        ///< xvycc 601
    MS_HDMI_EXT_COLOR_XVYCC709,        ///< xvycc 709
    MS_HDMI_EXT_COLOR_SYCC601,         ///< sYCC 601
    MS_HDMI_EXT_COLOR_ADOBEYCC601,     ///< Adobe YCC 601
    MS_HDMI_EXT_COLOR_ADOBERGB,        ///< Adobe RGB
    MS_HDMI_EXT_COLOR_BT2020YcCbcCrc,  /// ITU-F BT.2020 YcCbcCrc
    MS_HDMI_EXT_COLOR_BT2020RGBYCbCr,  /// ITU-R BT.2020 RGB or YCbCr
    MS_HDMI_EXT_COLOR_ADDITIONAL_DCI_P3_RGB_D65,
    MS_HDMI_EXT_COLOR_ADDITIONAL_DCI_P3_RGB_THEATER,
    MS_HDMI_EXT_COLORIMETRY_SMPTE_170M,
    MS_HDMI_EXT_COLORIMETRY_ITU_R_BT_709,
    MS_HDMI_EXT_COLOR_DEFAULT = MS_HDMI_EXT_COLOR_XVYCC601,    ///< Default setting
    MS_HDMI_EXT_COLOR_UNKNOWN = 9,    ///< Unknow
} MS_HDMI_EXT_COLORIMETRY_FORMAT;

/// HDMI Content Type
typedef enum
{
    MS_HDMI_CONTENT_NoData = 0,    // No Data.
    MS_HDMI_CONTENT_Graphics,  // Graphics(text).
    MS_HDMI_CONTENT_Photo,     // Photo type.
    MS_HDMI_CONTENT_Cinema,    // Cinema type.
    MS_HDMI_CONTENT_Game,      // Game type.
} MS_HDMI_CONTENT_TYPE;

///HDMI COLOR RANGE
typedef enum
{
    E_HDMI_COLOR_RANGE_DEFAULT, //
    E_HDMI_COLOR_RANGE_LIMIT,   //HDMI RGB Limited Range (16-235)
    E_HDMI_COLOR_RANGE_FULL,    //HDMI Full Range (0-255)
    E_HDMI_COLOR_RANGE_RESERVED
} EN_HDMI_COLOR_RANGE;

typedef enum
{
    MS_HDMI_COLOR_DEPTH_6_BIT = 0,
    MS_HDMI_COLOR_DEPTH_8_BIT = 1,
    MS_HDMI_COLOR_DEPTH_10_BIT = 2,
    MS_HDMI_COLOR_DEPTH_12_BIT = 3,
    MS_HDMI_COLOR_DEPTH_16_BIT = 4,
    MS_HDMI_COLOR_DEPTH_UNKNOWN = 5,
}MS_HDMI_COLOR_DEPTH;

typedef enum
{
    MS_HDMI_PIXEL_REPETITION_1X = 0,
    MS_HDMI_PIXEL_REPETITION_2X,
    MS_HDMI_PIXEL_REPETITION_3X,
    MS_HDMI_PIXEL_REPETITION_4X,
    MS_HDMI_PIXEL_REPETITION_5X,
    MS_HDMI_PIXEL_REPETITION_6X,
    MS_HDMI_PIXEL_REPETITION_7X,
    MS_HDMI_PIXEL_REPETITION_8X,
    MS_HDMI_PIXEL_REPETITION_9X,
    MS_HDMI_PIXEL_REPETITION_10X,
    MS_HDMI_PIXEL_REPETITION_RESERVED,
}MS_HDMI_PIXEL_REPETITION;

/// HDMI Equalize Level
typedef enum
{
    MS_HDMI_EQ_2M,    ///< HDMI EQ 2M
    MS_HDMI_EQ_10M,    ///< HDMI EQ 10M
    MS_HDMI_EQ_15M,    ///< HDMI EQ 15M
    MS_HDMI_EQ_MAX,    ///< HDMI EQ MAX
    MS_HDMI_EQ_USER_DEFINE,    ///< HDMI EQ User define
} MS_HDMI_EQ;

/// HDMI Packet nauber
typedef enum
{
    PKT_MPEG  = _BIT(0),    ///< Pactet[0]
    PKT_AUI  = _BIT(1),    ///< Pactet[1]
    PKT_SPD  = _BIT(2),    ///< Pactet[2]
    PKT_AVI  = _BIT(3),    ///< Pactet[3]
    PKT_GC  = _BIT(4),    ///< Pactet[4]
    PKT_ASAMPLE  = _BIT(5),    ///< Pactet[5]
    PKT_ACR  = _BIT(6),    ///< Pactet[6]
    PKT_VS  = _BIT(7),    ///< Pactet[7]
    PKT_NULL  = _BIT(8),    ///< Pactet[8]
    PKT_ISRC2  = _BIT(9),    ///< Pactet[9]
    PKT_ISRC1  = _BIT(10),    ///< Pactet[10]
    PKT_ACP  = _BIT(11),    ///< Pactet[11]
    PKT_ONEBIT_AUD  = _BIT(12),    ///< Pactet[12]
    PKT_GM =  _BIT(13),    ///< Pactet[13]
    PKT_HBR =  _BIT(14),    ///< Pactet[14]
    PKT_VBI =  _BIT(15),    ///< Pactet[15]
    PKT_HDR  = _BIT(16),    ///< Pactet[16]
    PKT_RSV  = _BIT(17),    ///< Pactet[17]
    PKT_EDR  = _BIT(18),	///< Pactet[18]
    PKT_CHANNEL_STATUS = _BIT(19),
    PKT_MULTI_VS = _BIT(20),
}MS_HDMI_PACKET_STATE_t;

typedef enum
{
    CAPS_DVI_ADJ_70M = _BIT(0),
    CAPS_VSYNC_END_EN = _BIT(1),
}MS_HDMI_FUNC_CAPS;

typedef enum
{
    E_HDMI_MODE_UNKNOWN ,
    E_HDMI_MODE_HDMI,
    E_HDMI_MODE_DVI,
}EN_HDMI_MODE;


/// HDMI PACKET VALUE structure
typedef struct __attribute__((packed))
{
    MS_U16 PKT_MPEG_VALUE;    ///< MPEG packet content
    MS_U16 PKT_AUI_VALUE;    ///< AUI packet content
    MS_U16 PKT_SPD_VALUE;    ///< SPD packet content
    MS_U16 PKT_AVI_VALUE;    ///< AVI packet content
    MS_U16 PKT_GC_VALUE;    ///< GC packet content
    MS_U16 PKT_ASAMPLE_VALUE;    ///< ASAMPLE packet content
    MS_U16 PKT_ACR_VALUE;    ///< ACR packet content
    MS_U16 PKT_VS_VALUE;    ///< VS packet content
    MS_U16 PKT_NULL_VALUE;    ///< NULL packet content
    MS_U16 PKT_ISRC2_VALUE;    ///< ISRC2 packet content
    MS_U16 PKT_ISRC1_VALUE;    ///< ISRC1 packet content
    MS_U16 PKT_ACP_VALUE;    ///< ACP packet content
    MS_U16 PKT_ONEBIT_AUD_VALUE;    ///< ONE bit AUD packet content
}MS_HDMI_PACKET_VALUE_t;

/// Received HDMI packet flag.
typedef struct __attribute__((packed))
{
    MS_U16 u16Version;                     /// Version.
    MS_U16 u16Size;                        /// Structure size.
    MS_BOOL bPKT_MPEG_RECEIVE;       ///< MPEG packet received
    MS_BOOL bPKT_AUI_RECEIVE;         ///< AUI packet received
    MS_BOOL bPKT_SPD_RECEIVE;         ///< SPD packet received
    MS_BOOL bPKT_AVI_RECEIVE;         ///< AVI packet received
    MS_BOOL bPKT_GC_RECEIVE;         ///< GC packet received
    MS_BOOL bPKT_ASAMPLE_RECEIVE;    ///< ASAMPLE packet received
    MS_BOOL bPKT_ACR_RECEIVE;        ///< ACR packet received
    MS_BOOL bPKT_VS_RECEIVE;          ///< VS packet received
    MS_BOOL bPKT_NULL_RECEIVE;       ///< NULL packet received
    MS_BOOL bPKT_ISRC2_RECEIVE;      ///< ISRC2 packet received
    MS_BOOL bPKT_ISRC1_RECEIVE;      ///< ISRC1 packet received
    MS_BOOL bPKT_ACP_RECEIVE;         ///< ACP packet received
    MS_BOOL bPKT_ONEBIT_AUD_RECEIVE;    ///< ONE bit AUD packet received
    MS_BOOL bPKT_GM_RECEIVE;     /// <GM packet received
    MS_BOOL bPKT_HBR_RECEIVE;    /// <HBR packet received
    MS_BOOL bPKT_VBI_RECEIVE;     /// <VBIpacket received
    MS_BOOL bPKT_HDR_RECEIVE;    ///< HDR packet received
    MS_BOOL bPKT_RSV_RECEIVE;    ///< RSV packet received
    MS_BOOL bPKT_EDR_VALID;      ///< Dolby EDR packet valid
    MS_BOOL bPKT_AUDIO_DST_RECEIVE; // DST
    MS_BOOL bPKT_AUDIO_3D_ASP_RECEIVE; // 3D L-PCM
    MS_BOOL bPKT_AUDIO_3D_DSD_RECEIVE; // One bit 3D audio
    MS_BOOL bPKT_AUDIO_METADATA_RECEIVE; // Audio metadata
    MS_BOOL bPKT_AUDIO_MULTI_ASP_RECEIVE; // Multi-stream
    MS_BOOL bPKT_AUDIO_MULTI_DSD_RECEIVE; // Multi-stream one bit audio
    MS_BOOL bPKT_EM_RECEIVE;    // EM packet received
}MS_HDMI_EXTEND_PACKET_RECEIVE_t;



typedef struct
{
    MS_U8 AVMuteStatus;
    MS_U8 AudioNotPCM;
    MS_U8 PreAudiostatus;
    MS_U8 ChecksumErrOrBCHParityErr;
    MS_U8 PacketColorFormat;
    MS_U8 AspectRatio;
    MS_U8 AudioPacketErr;
    MS_U8 ACPType;
} MS_HDMI_PACKET_STATUS_t;


typedef struct
{
    MS_HDMI_PACKET_VALUE_t enPKT_Value;
    MS_HDMI_PACKET_STATUS_t enPKT_Status;
} HDMI_PACKET_INFO_t, *P_HDMI_PACKET_INFO_t;

/// HDCP STATUS INFO structure
typedef struct __attribute__((packed))
{
    MS_BOOL b_St_HDMI_Mode;     ///> HDCP_STATUS[0] & BIT0
    MS_BOOL b_St_Reserve_1;     ///> (HDCP_STATUS[0] & BIT1)>>1
    MS_BOOL b_St_HDCP_Ver;      ///> (HDCP_STATUS[0] & BIT2)>>2
    MS_BOOL b_St_AVMute;        ///> (HDCP_STATUS[0] & BIT3)>>3
    MS_BOOL b_St_ENC_En;        ///> (HDCP_STATUS[0] & BIT4)>>4
    MS_BOOL b_St_AKsv_Rcv;      ///> (HDCP_STATUS[0] & BIT5)>>5
    MS_BOOL b_St_ENC_Dis;       ///> (HDCP_STATUS[0] & BIT6)>>6
    MS_BOOL b_St_Reserve_7;     ///> (HDCP_STATUS[0] & BIT7)>>7
}MS_HDCP_STATUS_INFO_t;

/// HDMI Black Level
typedef enum
{
    E_BLACK_LEVEL_LOW = 0,    ///> HDMI black level low
    E_BLACK_LEVEL_HIGH,    ///> HDMI black level high
} BLACK_LEVEL;

/// VGA/DVI/HDMI EDID
typedef enum
{
    E_XC_PROG_DVI0_EDID = 0x00,
    E_XC_PROG_DVI1_EDID = 0x01,
    E_XC_PROG_DVI2_EDID = 0x02,
    E_XC_PROG_DVI3_EDID = 0x03,
    E_XC_PROG_VGA_EDID  = 0x10,
} E_XC_DDCRAM_PROG_TYPE;

typedef struct DLL_PACKED
{
    MS_U8 *EDID;                            ///< customer EDID
    MS_U16 u16EDIDSize;                       ///< 128 for VGA/DVI , 256 for HDMI
    E_XC_DDCRAM_PROG_TYPE eDDCProgType;     ///< what kind of DDCRam to program

    MS_U16 u16CECPhyAddr;                   ///< if only 1 Digital DDCRam (get this information from MApi_XC_GetInfo()), you need to specify CEC physical address here
    MS_U8  u8CECPhyAddrIdxL;                ///< start point of CEC physical address (low byte) in EDID
    MS_U8  u8CECPhyAddrIdxH;                ///< start point of CEC physical address (high byte) in EDID
} XC_DDCRAM_PROG_INFO;

////===========================>>HDCP

///HDCP polling status
typedef struct
{
    MS_BOOL bHPD_OK;    ///< HPD OK
    MS_U16     u16HPD_Counter;    ///< HPD times counter
    MS_U16     u16HDCP_KeyCounter;    /// > HDCP key number
    MS_U16     u16HDCP_KeyChkSum;    ///> HDCP key check sum
    MS_BOOL bIsPullHighHPD;    ///> HPD is high or not
    MS_U8      u8BKSV[5];    ///> BKSV

} HDCP_POLLING_STATUS_t;

/**
 *  The return value for HDMI fuction.
 */
typedef enum
{
    /// fail
    E_HDMI_RET_FAIL = 0,
    /// success
    E_HDMI_RET_OK,
} HDMI_Ret_Value;


// HDMI v1.4 new feature - T8 and T9
typedef enum
{
    E_HDMI_NO_ADDITIONAL_FORMAT, // 000
    E_HDMI_4Kx2K_FORMAT,                 // 001
    E_HDMI_3D_FORMAT,                      // 010
    E_HDMI_RESERVED,                         // 011 ~ 111
    E_HDMI_NA                                     // Not available
} E_HDMI_ADDITIONAL_VIDEO_FORMAT;

// 3D info
typedef enum
{
    //range [0000 ~ 1111] reserved for hdmi 3D spec
    E_XC_3D_INPUT_FRAME_PACKING                     = 0x00, //0000
    E_XC_3D_INPUT_FIELD_ALTERNATIVE                 = 0x01, //0001
    E_XC_3D_INPUT_LINE_ALTERNATIVE                  = 0x02, //0010
    E_XC_3D_INPUT_SIDE_BY_SIDE_FULL                 = 0x03, //0011
    E_XC_3D_INPUT_L_DEPTH                           = 0x04, //0100
    E_XC_3D_INPUT_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH   = 0x05, //0101
    E_XC_3D_INPUT_TOP_BOTTOM                        = 0x06, //0110
    E_XC_3D_INPUT_SIDE_BY_SIDE_HALF                 = 0x08, //1000

    E_XC_3D_INPUT_CHECK_BORAD                       = 0x09, //1001

    //user defined
    E_XC_3D_INPUT_MODE_USER                         = 0x10,
    E_XC_3D_INPUT_MODE_NONE                         = E_XC_3D_INPUT_MODE_USER,
    E_XC_3D_INPUT_FRAME_ALTERNATIVE,
    E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE,
    // optimize for format: in:E_XC_3D_INPUT_FRAME_PACKING, out:E_XC_3D_OUTPUT_TOP_BOTTOM,E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF
    E_XC_3D_INPUT_FRAME_PACKING_OPT,
    // optimize for format: in:E_XC_3D_INPUT_TOP_BOTTOM, out:E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF
    E_XC_3D_INPUT_TOP_BOTTOM_OPT,
    E_XC_3D_INPUT_NORMAL_2D,
    E_XC_3D_INPUT_NORMAL_2D_INTERLACE,
    E_XC_3D_INPUT_NORMAL_2D_INTERLACE_PTP,
    E_XC_3D_INPUT_SIDE_BY_SIDE_HALF_INTERLACE_OPT,
    E_XC_3D_INPUT_NORMAL_2D_HW,  //for hw 2D to 3D use
    E_XC_3D_INPUT_PIXEL_ALTERNATIVE,
} E_XC_3D_INPUT_MODE;

typedef enum
{
    E_VIC_4Kx2K_30Hz = 0x01, // 1920(x2)x1080(x2) @ 29.97/30Hz
    E_VIC_4Kx2K_25Hz = 0x02, // 1920(x2)x1080(x2) @ 25Hz
    E_VIC_4Kx2K_24Hz = 0x03, // 1920(x2)x1080(x2) @ 23.976/24Hz
    E_VIC_4Kx2K_24Hz_SMPTE = 0x04, // 2048(x2)x1080(x2) @ 24Hz
    E_VIC_RESERVED,                // 0x00, 0x05 ~ 0xFF
    E_VIC_NA                             // Not available
} E_HDMI_VIC_4Kx2K_CODE;

typedef enum
{
    E_3D_EXT_DATA_HOR_SUB_SAMPL_0,      //0000
    E_3D_EXT_DATA_HOR_SUB_SAMPL_1,      //0001
    E_3D_EXT_DATA_HOR_SUB_SAMPL_2,      //0010
    E_3D_EXT_DATA_HOR_SUB_SAMPL_3,      //0011
    E_3D_EXT_DATA_QUINCUNX_MATRIX_0,    //0100
    E_3D_EXT_DATA_QUINCUNX_MATRIX_1,    //0101
    E_3D_EXT_DATA_QUINCUNX_MATRIX_2,    //0110
    E_3D_EXT_DATA_QUINCUNX_MATRIX_3,    //0111
    E_3D_EXT_DATA_RESERVE,              //1000 ~ 1111
    E_3D_EXT_DATA_MODE_MAX,
} E_HDMI_3D_EXT_DATA_T;

typedef enum
{
    E_3D_META_DATA_PARALLAX_ISO23002_3,      //0000
    E_3D_META_DATA_RESERVE,              //001 ~ 1111
    E_3D_META_DATA_MAX,
} E_HDMI_3D_META_DATA_TYPE;

typedef struct
{
    MS_BOOL bImmeswitchSupport;
    MS_U8 ucMHLSupportPath;
} stHDMI_INITIAL_TABLE;

typedef struct __attribute__ ((packed))
{
    MS_BOOL b3D_Meta_Present;
    E_HDMI_3D_META_DATA_TYPE t3D_Metadata_Type;
    MS_U8 u83D_Metadata_Length;
    MS_U8 u83D_Metadata[23]; // Maximum length of VS is 31 - 8 = 23
} sHDMI_3D_META_FIELD;

/////////// HDCP22 /////////////////
typedef enum
{
    E_HDCP22_IF_HDMI,   //0x00
    E_HDCP22_IF_MIRACAST,   //0x01
    //TBD
} E_HDCP22_PORT_TYPE;

typedef enum
{
    E_HDCP_NO_ENCRYPTION = 0, //0x00
    E_HDCP_1_4,               //0x01
    E_HDCP_2_2,               //0x02
}E_HDMI_HDCP_STATE;

typedef enum
{
    E_HDCP_NOT_ENCRYPTION = 0, //0x00
    E_HDCP_1_4_ENCRYPTION,     //0x01
    E_HDCP_2_2_ENCRYPTION,     //0x02
}E_HDMI_HDCP_ENCRYPTION_STATE;

typedef enum
{
    E_HDMI_NO_DITHER,
    E_HDMI_1BIT_DITHER,
    E_HDMI_2BIT_DITHER,
} E_DITHER_TYPE;

/////////// HDR metadata /////////////////
typedef struct __attribute__ ((packed))
{
    MS_U16      u16Version;                     /// Version.
    MS_U16      u16Size;                        /// Structure size.
    MS_U8       u8EOTF; // 0:SDR gamma,  1:HDR gamma,  2:SMPTE ST2084,  3:Future EOTF,  4-7:Reserved
    MS_U8       u8Static_Metadata_ID; // 0:Static Metadata Type 1,  1-7:Reserved for future use
    MS_U16      u16Display_Primaries_X[3]; // display_primaries_x
    MS_U16      u16Display_Primaries_Y[3]; // display_primaries_x
    MS_U16      u16White_Point_X; // display primaries Wx
    MS_U16      u16White_Point_Y; // display primaries Wy
    MS_U16      u16Max_Display_Mastering_Luminance; // max display mastering luminance
    MS_U16      u16Min_Display_Mastering_Luminance; // min display mastering luminance
    MS_U16      u16Maximum_Content_Light_Level; // maximum content light level
    MS_U16      u16Maximum_Frame_Average_Light_Level; // maximum frame-average light level
    MS_U8       u8Version;
    MS_U8       u8Length;
} sHDR_METADATA;

typedef struct __attribute__ ((packed))
{
    MS_U16 u16StructVersion;     // Version.
    MS_U16 u16StructSize;        // Structure size.
    MS_U8 u8Version;
    MS_U8 u8Length;
    MS_U8 u8S10Value;
    MS_U8 u8B10Value;
    MS_U8 u8A0Value;
    MS_U8 u8Y210Value;
    MS_U8 u8R3210Value;
    MS_U8 u8M10Value;
    MS_U8 u8C10Value;
    MS_U8 u8SC10Value;
    MS_U8 u8Q10Value;
    MS_U8 u8EC210Value;
    MS_U8 u8ITCValue;
    MS_U8 u8VICValue;
    MS_U8 u8PR3210Value;
    MS_U8 u8CN10Value;
    MS_U8 u8YQ10Value;
    MS_U8 u8ACE3210Value;
    MS_HDMI_COLOR_FORMAT enColorForamt;
    EN_HDMI_COLOR_RANGE enColorRange;
    MS_HDMI_EXT_COLORIMETRY_FORMAT enColormetry;
    MS_HDMI_PIXEL_REPETITION enPixelRepetition;
    MS_HDMI_AR_TYPE enAspectRatio;
    MS_HDMI_AR_TYPE enActiveFormatAspectRatio;
}stAVI_PARSING_INFO;

typedef struct __attribute__ ((packed))
{
    MS_U16 u16StructVersion;     // Version.
    MS_U16 u16StructSize;        // Structure size.
    MS_U8 u8ControlAVMute;
    MS_U8 u8DefaultPhase;
    MS_U8 u8CDValue;
    MS_U8 u8PPValue;
    MS_U8 u8LastPPValue;
    MS_U8 u8PreLastPPValue;
    MS_HDMI_COLOR_DEPTH enColorDepth;
}stGC_PARSING_INFO;


typedef enum
{
    E_HDMI_GET_HDE,
    E_HDMI_GET_VDE,
    E_HDMI_GET_HTT,
    E_HDMI_GET_VTT,
    E_HDMI_GET_AUDIO_PROTECT_INFO,
    E_HDMI_GET_SUPPORT_EDID_SIZE,
    E_HDMI_GET_ERROR_STATUS,
    E_HDMI_GET_MULTIVS_COUNT,
    E_HDMI_GET_ISRC1_HEADER_INFO,
    E_HDMI_GET_H_POLARITY,
    E_HDMI_GET_V_POLARITY,
    E_HDMI_GET_INTERLACE,
    E_HDMI_GET_CHIP_HDCP_CAPABILITY,
    E_HDMI_GET_VFRONT,
    E_HDMI_GET_VSYNC,
    E_HDMI_GET_VBACK,
    E_HDMI_GET_HFRONT,
    E_HDMI_GET_HSYNC,
    E_HDMI_GET_HBACK,
    E_HDMI_GET_PIXEL_CLOCK,
    E_HDMI_GET_FS,  // audio?
    E_HDMI_GET_FRAMERATE,
    E_HDMI_GET_CLOCKSTABLE_STATUS,
    E_HDMI_GET_PIPELINE_MODE,
    E_HDMI_GET_EMP_ENABLE_STATUS,
    E_HDMI_GET_DSC_STATUS,
    E_HDMI_GET_VRR_STATUS,
    E_HDMI_GET_FreeSync_STATUS,
    E_HDMI_GET_TG_HDE,
    E_HDMI_GET_TG_VDE,
    E_HDMI_GET_TG_HTT,
    E_HDMI_GET_TG_VTT,
    E_HDMI_GET_TG_VFRONT,
    E_HDMI_GET_TG_VSYNC,
    E_HDMI_GET_TG_VBACK,
    E_HDMI_GET_TG_HFRONT,
    E_HDMI_GET_TG_HSYNC,
    E_HDMI_GET_TG_HBACK,
} E_HDMI_GET_DATA_INFO;

typedef enum
{
    E_HDMI_GET_TMDS_BIT_RATES,
    E_HDMI_GET_TMDS_CHARACTER_RATES,
    E_HDMI_GET_TMDS_CLOCK_RATES,
    E_HDMI_GET_PIXEL_CLOCK_RATES,
} E_HDMI_GET_TMDS_RATES;

typedef enum
{
    E_HDMI_GET_HDMI_SIGNAL_STABLE
} E_HDMI_GET_HDMI_STATUS_IDX;

typedef enum
{
    E_HDMI_GET_FRL_ABILITY = BIT(0),
    E_HDMI_GET_ALL_ABILITY = BIT(1),
} E_HDMI_GET_WHICH_HW_ABILITY;


typedef enum
{
    E_HDMI_EMP_DOLBY_HDR,
    E_HDMI_EMP_DSC,
    E_HDMI_EMP_DYNAMIC_HDR,
    E_HDMI_EMP_VRR,
    E_HDMI_EMP_QFT,
    E_HDMI_EMP_QMS,

    //MAX
    E_HDMI_EMP_MAX
} E_HDMI_EMPACKET_TYPE;


typedef enum
{
    MS_HDMI_PIPELINE_UNKNOWN = 0,
    MS_HDMI_PIPELINE_1P,
    MS_HDMI_PIPELINE_2P,
} E_HDMI_PIPELINE_MODE;

//------------------------------------------------------------------------------------------------
// for v4l2
//------------------------------------------------------------------------------------------------
typedef enum
{
    E_KDRV_HDMI_EMP_DOLBY_HDR,
    E_KDRV_HDMI_EMP_DSC,
    E_KDRV_HDMI_EMP_DYNAMIC_HDR,
    E_KDRV_HDMI_EMP_VRR,
    E_KDRV_HDMI_EMP_QFT,
    E_KDRV_HDMI_EMP_QMS,

    //MAX
    E_KDRV_HDMI_EMP_MAX
} EN_KDRV_HDMI_EMP_TYPE;

typedef enum {
    HDMI_IRQ_PHY,
    HDMI_IRQ_MAC,
    HDMI_IRQ_PKT_QUE,
    HDMI_IRQ_PM_SQH_ALL_WK,
    HDMI_IRQ_PM_SCDC,
    HDMI_FIQ_CLK_DET_0,
    HDMI_FIQ_CLK_DET_1,
    HDMI_FIQ_CLK_DET_2,
    HDMI_FIQ_CLK_DET_3,
    HDMI_INT_N,
}EN_KDRV_HDMIRX_INT;

typedef enum
{
    E_MUX_DVI2MIU_S0 = 0,
    E_MUX_DVI2MIU_S1,
    E_MUX_DVI2MIU_S2,
    E_MUX_MAX,
}EN_HDMI_DVI2MIU_MUX_SEL;

typedef enum
{
    HDMI_PATGEN_RGB_V_BAR = 0,
    HDMI_PATGEN_RGB_H_BAR,
    HDMI_PATGEN_RGB_USERDEF,
    HDMI_PATGEN_YUV_V_BAR,
    HDMI_PATGEN_YUV_H_BAR,
}EN_HDMI_PATGEN_MODE;

#ifdef HDMI_CPU_SUPPORT
typedef enum {
  CMD_OP_PORT_RELATED_WRITE_PARAM,
  CMD_OP_PORT_RELATED_READ_PARAM,
  CMD_OP_PORT_RELATED_READ_PAGE,
} CMD_ID_DEFINE;

enum HDMI_MBX_SET_PARAM_CMD_ID {
  HDMI_MBX_SET_PARAM_ID_NOT_DEFINED = 0,
};

enum HDMI_MBX_GET_PARAM_CMD_ID {
  HDMI_MBX_GET_PARAM_ID_NOT_DEFEIND = 0,
  HDMI_MBX_GET_PARAM_ID_CLK_CHG,
};

enum HDMI_MBX_INSTANCE_FLAG {
  HDMI_MBX_INST_FLAG_CABLE_DET = 0,
  HDMI_MBX_INST_FLAG_CLOCK_STABLE,
  HDMI_MBX_INST_FLAG_CLOCK_CHANGE,
  HDMI_MBX_INST_FLAG_TRAINING_DONE,
  HDMI_MBX_INST_FLAG_NO_RETRAIN_FLAG,
  HDMI_MBX_INST_FLAG_PHY_DONE,
  HDMI_MBX_INST_FLAG_DE_STABLE,
  HDMI_MBX_INST_FLAG_IMMESWITCHSTATUS,
  HDMI_MBX_INST_FLAG_FRL_RATE,
  HDMI_MBX_INST_FLAG_VERSION,
};

enum HDMI_MBX_HK_INSTANCE_FLAG {
  HDMI_MBX_HK_INST_FLAG_ACTIVE_PORT = 0,
  HDMI_MBX_HK_INST_FLAG_IMMESWITCH,
  HDMI_MBX_HK_INST_FLAG_POWEROFF,
  HDMI_MBX_HK_INST_FLAG_RTERMOFF,
  HDMI_MBX_HK_INST_FLAG_DATA_RTERMOFF,
  HDMI_MBX_HK_INST_FLAG_HPD_SET,
};
#endif


//#endif// willy add


#undef INTERFACE


#ifdef __cplusplus
}
#endif

#endif /* MDRV_HDMI_H */

