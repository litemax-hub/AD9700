#include "datatype.h" //roger: should I use it
#include "ComboApp.h"

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

#define TMDS_HDCP_WINDOW_END_VALUE          0x3U
#define TMDS_HDCP2_SOURCE_READ_OFFSET       130U
#define HDMI_HDCP_HDCP2_VERSION             0x50U
#define DPRx_AUX_OFFSET300(a)               ((a) * 0x300)

//#define INPUT_PORT_DVI_END              INPUT_PORT_DVI3
#ifndef BIT
#define BIT(_bit_)                          (1 << (_bit_))
#endif

#ifndef BMASK
#define BMASK(_bits_)                       (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))
#endif

#ifndef ABS_MINUS
#define ABS_MINUS(a, b)                     (((a) > (b))? ((a) -(b)): ((b) -(a)))
#endif


#define HDMI_EDID_BLOCK_SIZE               128
#define HDMI_VSDB_PACKET_SIZE 14
#define HDMI_HDR_PACKET_SIZE HDMI_VSDB_PACKET_SIZE

//==============================================================================
//==============================================================================

#define IRQ_DVI_CK_CHG                      BIT(0)      //#[0]
#define IRQ_HDMI_MODE_CHG                   BIT(1)    //#[1]
#define IRQ_SCART_ID0_CHG                   BIT(2)     //#[2]
#define IRQ_SCART_ID1_CHG                   BIT(3)     //#[3]
#define IRQ_SAR_DET_UPD                     BIT(4)        //#[4]
#define IRQ_RESERVE                         (BIT(7)|BIT(6)|BIT(5))                 //#[5:7]
#define IRQ_ALL_BIT                         (BIT(4)|BIT(3)|BIT(2)|BIT(1)|BIT(0))

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
#define TMDS_FINE_TUNE_UNDER_20_THRESHOLD   0x1FFU
#define TMDS_FINE_TUNE_UNDER_20_THRESHOLD1  0x9FFU
#define TMDS_CONTINUE_START                 0xFU
#define TMDS_CONTINUOUS_NUMBER              0x350U

#define HDMI_UNDER_150M_EQ_SETTING_VALUE    3U
#define HDMI_OVER_150M_EQ_SETTING_VALUE     7U
#define HDMI_EQ_14_SETTING_VALUE            20U
#define HDMI_EQ_14_LOW_SETTING_VALUE        10U
#define HDMI_EQ_20_SETTING_VALUE            0x19U

#define HDMI_MHL_EQ_SETTING_VALUE           4U

#define TMDS_SCDC_ACCESS_ADDERSS_A8         BIT(6)

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
#define TMDS_14_CHECK_ERROR_TIMES           0x80U
#define TMDS_20_CHECK_ERROR_INTERVAL        0x1U
#define TMDS_20_CHECK_ERROR_TIMES           0x80U
#define TMDS_20_ERROR_HAPPEN_COUNT          3U
#define TMDS_20_ERROR_HAPPEN_DCDR_COUNT     10U
#define TMDS_20_MODE_CHANGE_COUNT           8U
#define TMDS_20_RETRIGGER_TIMES             5U
#define TMDS_AUTO_EQ_PROCESS_INTERVAL       500U
#define TMDS_AUTO_EQ_BGPROCESS_INTERVAL     100U
#define TMDS_AUTO_EQ_VERIFY_COUNT           10U
#define TMDS_FORCE_ACDR_CHECK_INTERVAL      2U
#define HDMI_VDAC_DLEV_COUNT_H20            6U  //24mv
#define HDMI_VDAC_DLEV_COUNT_PORT20         0x20U
#define TMDS_CLEAR_ERROR_STATUS_COUNT       2U

#define HDMI_DECODE_ERROR_THRESHOLD14       0x8001U
#define HDMI_DECODE_ERROR_THRESHOLD20       0x8000U
#define HDMI_AUTO_EQ_ENABLE_THRESHOLD       140U
#define HDMI_AUTO_EQ_MEDIUM_THRESHOLD       (200U)
#define HDMI_AUTO_EQ_LOW_THRESHOLD          (150U)
#define HDMI_EQ_FIXED_LOW_THRESHOLD         75U
#define HDMI_CLOCK_UNSTABLE_OFFSET          2U
#define HDMI_SCAN_PHASE_THRESHOLD           (32U)
#define HDMI_SCAN_PHASE_THRESHOLD_14        37U
#define HDMI_SCAN_PHASE_THRESHOLD_20        30U

#define HDMI_CHECK_SCRAMBLE_INTERVAL        20U
#define HDMI_POWER_SAVING_CHECK_INTERVAL    500U
#define HDMI_POWER_SAVING_CHECK_COUNT       200U
#define HDMI_HDCP_AUTH_CHECK_INTERVAL       2000U
#define HDMI_LANE_LENGTH                    4U

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
#define HDMI_14_RETRIG_UNDER_75M_TIMES      2
#define HDMI_CLOCK_RATE_FRL_12G_MIDDLE      0x1788U
#define HDMI_CLOCK_RATE_FRL_OFFSET          0x3U

#define HDMI_XTAL_CLOCK_MHZ                 12U
#define HDMI_XTAL_CLOCK_10kHZ               1200U
#define HDMI_XTAL_CLOCK_HZ                  12000000U
#define HDMI_XTAL_DIVIDER                   128U

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
#define HBMASK    0xFF00
#define LBMASK    0x00FF

#define HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_SW_READ        0x140U
#define HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_SW_READ        0x15EU
#define HDMI_EFUSE_ADDRESS_HRX_P2_RTERM_SW_READ        0x15EU

#define HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_45             0x13CU
#define HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_45             0x15BU
#define HDMI_EFUSE_ADDRESS_HRX_P2_RTERM_45             0x15DU

#define HDMI_EFUSE_ADDRESS_HRX_P0_RTERM_50             0x13DU
#define HDMI_EFUSE_ADDRESS_HRX_P1_RTERM_50             0x15CU
#define HDMI_EFUSE_ADDRESS_HRX_P2_RTERM_50             0x15DU

#define HDMI_EFUSE_P0_ADDRESS_OFFSET    0x0U
#define HDMI_EFUSE_P1_ADDRESS_OFFSET    0x40U
#define HDMI_EFUSE_P2_ADDRESS_OFFSET    0x80U

#define HDMI_MPEG_PACKET_LENGTH             5U
#define HDMI_AUDIO_PACKET_LENGTH            5U
#define HDMI_SPD_PACKET_LENGTH             25U
#define HDMI_AVI_PACKET_LENGTH             15U
#define HDMI_GCP_PACKET_LENGTH              2U
#define HDMI_VS_PACKET_LENGTH             (32U)
#define HDMI_ISRC1_PACKET_LENGTH           16U
#define HDMI_ISRC2_PACKET_LENGTH          (32U)
#define HDMI_ACP_PACKET_LENGTH             16U
#define HDMI_HDR_PACKET_LENGTH             30U
#define HDMI_AUDIO_CHANNEL_STATUS_LENGTH    5U
#define HDMI_MULTI_VS_PACKET_LENGTH      (120U)

typedef enum
{
    HDMI_INPUT_PORT0 = 0,
    HDMI_INPUT_PORT1,
    HDMI_INPUT_PORT2,
    HDMI_INPUT_PORT_END,
    HDMI_INPUT_PORT3,
    HDMI_INPUT_PORT_NONE,
}EN_HDMI_INPUT_PORT;

typedef struct
{
    Bool bNoInputFlag;
    Bool bHDMI20Flag;
    Bool bYUV420Flag;
    Bool bFreeSyncModeEnable;
    Bool bHDMIModeFlag;
    Bool bAutoEQEnable;
    Bool bAutoEQRetrigger;
    Bool bClockStableFlag;
    Bool bDEStableFlag;
    Bool bDEChangeFlag;
    Bool bTimingStableFlag;
    Bool bPowerOnLane;
    Bool bIsRepeater;
    Bool bPowerSavingFlag;
    Bool bAutoEQIgnoreFlag;
    Bool bHDREnableFlag;
    Bool bDVIDualLinkFlag;
    Bool usClkCountChangeFlag;
    Bool bDChangeFlag;
    //Bool bHDMI14SettingFlag;
    BYTE ucCurrentPort;
    BYTE ucAutoEQState;
    //BYTE ucSourceVersion;
    BYTE ucCheckErrorInterval;
    BYTE ucAutoEQ14Mode;
    BYTE ucHDCPState;
    BYTE ucHDCP14IntStatus;
    BYTE ucAutoEQErrorHappenCount;
    BYTE ucDVIDualLinkCount;
    BYTE ucDVISingleLinkCount;
    WORD usClockCount;
    WORD usCheckErrorTimes;
    WORD usAutoEQProcCounter;
    WORD ulPacketStatusInfo_Low;
    WORD ulPacketStatusInfo_High;
    BYTE u8FRLRate;//======= below are new added parameters===========
    BYTE ucSourceVersion;
    Bool bVersionChangeFlag;
    Bool bActiveCableFlag;
    Bool bResetDCFifoFlag ;
    BYTE u8RatioDetect;
    BYTE u8ActiveCableState;
    Bool bPowerControlOn;
    Bool bForcePowerDown;
    BYTE u8Colordepth;
    BYTE u8ChangePHYCase;
    WORD u16ActiveCableCheckErrorCount;
    BYTE u8FastTrainingState;
    WORD u16CheckErrorCount;
    Bool bEQError0[HDMI_LANE_LENGTH];
    Bool bEQError1[HDMI_LANE_LENGTH];
    Bool bEQError2[HDMI_LANE_LENGTH];
    BYTE u8EQValue0[HDMI_LANE_LENGTH];
    BYTE u8EQValue1[HDMI_LANE_LENGTH];
    BYTE u8EQValue2[HDMI_LANE_LENGTH];
    BYTE u8RetriggerCount20;
    BYTE u8ActiveCableClearErrorStatusCount;
    BYTE u8ClearErrorStatusCount;
    BYTE u8TryNegativeC;
    BYTE u8TryPhaseDecI;
    WORD u16EMPacketInfo;
    DWORD ulPacketStatusInfo;
    BYTE u8RetriggerH14Under75M;
    WORD u16GC_PktLossCnt;
    WORD u16CheckBCHMuteCnt;
}ST_HDMI_RX_POLLING_INFO;


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

enum HDMI_COLORDEPTH_TYPE
{
    HDMI_COLOR_DEPTH_8BIT = 0,
    HDMI_COLOR_DEPTH_10BIT,
    HDMI_COLOR_DEPTH_12BIT,
    HDMI_COLOR_DEPTH_16BIT,
    HDMI_COLOR_DEPTH_NONE,
};

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
    HDMI_TMDS_CLOCK_NUMOF_BAND,
}EN_HDMI_TMDS_CLOCK_DETECT_BAND;


typedef enum
{
    HDMI_TIMING_DETECT_4_0 = 0, // (4) - 0, for legacy
    HDMI_TIMING_DETECT_4_1, // (4) - 1, for legacy
    HDMI_TIMING_DETECT_4_2, // (4) - 2, for FRL
    HDMI_TIMING_DETECT_4_3, // (4) - 3, for FRL
    HDMI_TIMING_DETECT_5_0, // (5) - 0
}EN_HDMI_TIMING_DETECT_PATH;


typedef struct
{
    Bool bWordBoundaryErrorL0;
    Bool bWordBoundaryErrorL1;
    Bool bWordBoundaryErrorL2;
    Bool bWordBoundaryErrorL3;
    WORD u16DecodeErrorL0;
    WORD u16DecodeErrorL1;
    WORD u16DecodeErrorL2;
    WORD u16DecodeErrorL3;
}stHDMI_ERROR_STATUS;


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
    HDMI_FAST_TRAINING_TRY_PHASEDEC_I_H14,
    HDMI_FAST_TRAINING_CHANGE_PHY_H14,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H14_75M_TO_135M,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H14_UNDER_75M,
    HDMI_FAST_TRAINING_CHECK_STABLE_H14_UNDER_75M,
    HDMI_FAST_TRAINING_CHECK_STABLE_H14_75M_TO_135M,
    HDMI_FAST_TRAINING_CHANGE_PHY_H14_75M_TO_135M,
    HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H20,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H20,
    HDMI_FAST_TRAINING_CHECK_STABLE_H20,
    HDMI_FAST_TRAINING_CHANGE_PHY_H20,
    HDMI_FAST_TRAINING_WAIT_EQ_FLOW_DONE_H21,
    HDMI_FAST_TRAINING_CLEAR_DE_UNSTABLE_H21,
    HDMI_FAST_TRAINING_CHECK_DE_STABLE_H21,
    HDMI_FAST_TRAINING_CLEAR_ERROR_H21,
    HDMI_FAST_TRAINING_CHECK_STABLE_H21,
    HDMI_FAST_TRAINING_STOP,
    HDMI_FAST_TRAINING_FINISH,
};


typedef enum
{
    HDMI_14_TMDS_CLOCK_NONE = 0,
    HDMI_14_TMDS_CLOCK_UNDER_75M,
    HDMI_14_TMDS_CLOCK_75M_TO_135M,
    HDMI_14_TMDS_CLOCK_OVER_135M,
}EN_HDMI_14_TMDS_CLOCK_RANGE;

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
    HDMI_CHANGE_PHY_H20_CASE0,
    HDMI_CHANGE_PHY_H20_CASE1,
    HDMI_CHANGE_PHY_H20_CASE2,
    HDMI_CHANGE_PHY_FINISH,
};

enum HDMI_SOURCE_VERSION_TYPE
{
    HDMI_SOURCE_VERSION_NOT_SURE = 0,
    HDMI_SOURCE_VERSION_HDMI14,
    HDMI_SOURCE_VERSION_HDMI20,
};

enum HDMI_SIGNAL_DETECT_TYPE
{
    HDMI_SIGNAL_DETECT_NONE = 0,
    HDMI_SIGNAL_DETECT_SQUELCH,
    HDMI_SIGNAL_DETECT_CLOCK_STABLE,
    HDMI_SIGNAL_DETECT_DATA_ENABLE,
    HDMI_SIGNAL_DETECT_HDMI_MODE,
    HDMI_SIGNAL_DETECT_HDCP_STATUS,
    HDMI_SIGNAL_DETECT_AUDIO_MUTE_EVENT,
    HDMI_SIGNAL_DETECT_YUV420,
    HDMI_SIGNAL_DETECT_EMP,
};

/// HDMI Packet nauber
typedef enum
{
    PKT_MPEG  = BIT(0),    ///< Pactet[0]
    PKT_AUI  = BIT(1),    ///< Pactet[1]
    PKT_SPD  = BIT(2),    ///< Pactet[2]
    PKT_AVI  = BIT(3),    ///< Pactet[3]
    PKT_GC  = BIT(4),    ///< Pactet[4]
    PKT_ASAMPLE  = BIT(5),    ///< Pactet[5]
    PKT_ACR  = BIT(6),    ///< Pactet[6]
    PKT_VS  = BIT(7),    ///< Pactet[7]
    PKT_NULL  = BIT(8),    ///< Pactet[8]
    PKT_ISRC2  = BIT(9),    ///< Pactet[9]
    PKT_ISRC1  = BIT(10),    ///< Pactet[10]
    PKT_ACP  = BIT(11),    ///< Pactet[11]
    PKT_ONEBIT_AUD  = BIT(12),    ///< Pactet[12]
    PKT_GM =  BIT(13),    ///< Pactet[13]
    PKT_HBR =  BIT(14),    ///< Pactet[14]
    PKT_VBI =  BIT(15),    ///< Pactet[15]
    PKT_HDR  = BIT(16),    ///< Pactet[16]
    PKT_RSV  = BIT(17),    ///< Pactet[17]
    PKT_EDR  = BIT(18),	///< Pactet[18]
    PKT_CHANNEL_STATUS = BIT(19),
    PKT_MULTI_VS = BIT(20),
}MS_HDMI_PACKET_STATE_t;

/// HDMI reset type
typedef enum
{
    REST_AVMUTE = BIT(8),    ///<HDMI Reset bit[8]
    REST_Y_COLOR = BIT(9),    ///<HDMI Reset bit[9]
    REST_PIXEL_REPETITION  = BIT(10),    ///<HDMI Reset bit[10]
    REST_FRAME_REPETITION  = BIT(11),    ///<HDMI Reset bit[11]
    REST_GCP_PKT  = BIT(12),    ///<HDMI Reset bit[12]
    REST_DEEP_COLOR_FIFO  = BIT(13),    ///<HDMI Reset bit[13]
    REST_RESERVE = BIT(14),    ///<HDMI Reset bit[14]
    REST_HDMI_STATUS = BIT(15),    ///<HDMI Reset bit[15]
}HDMI_REST_t;

//HDMI IRQ Mask
typedef enum
{
    IRQ_GeneralConrolPacket               = BIT(0),
    IRQ_AVIInfoFramePacket                = BIT(1),
    IRQ_AudioInfoFramePacket              = BIT(2),
    IRQ_MPEGInfoFramePacket               = BIT(3),
    IRQ_ACPPacket                         = BIT(4),
    IRQ_ISRCPacket                        = BIT(5),
    IRQ_BCHError                          = BIT(6),
    IRQ_GamutMetadataPacket               = BIT(7),
    IRQ_ReceiveANewMessage                = BIT(8),
    IRQ_TransmitAMessageSuccessfully      = BIT(9),
    IRQ_RetryFailForTx                    = BIT(10),
    IRQ_LostArbitration2TeSecondInitiator = BIT(11),
    IRQ_CTSNOverRange                     = BIT(12),
    IRQ_HDMIVideoClockFrequencyBigChange  = BIT(13),
    IRQ_NoVideoClockInputFromDVI          = BIT(14),
    IRQ_CEC1ReceivesANewMessage           = BIT(15),
}HDMI_IRQ_Mask;

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
} E_HDMI_GET_DATA_INFO;

typedef enum
{
    HDMI_PM_POWERON,
    HDMI_PM_STANDBY,
    HDMI_PM_POWEROFF,
    HDMI_PM_INVAILD
}HDMI_PM_MODE_TYPE;

typedef enum
{
    HDMI_MUX_0,
    HDMI_MUX_1,
    HDMI_MUX_NONE,
}EN_HDMI_MUX_TYPE;

typedef enum
{
    HDMI_SIGNAL_PIX_MHZ   = 0,
    HDMI_SIGNAL_PIX_10KHZ,
    HDMI_SIGNAL_PIX_HZ   ,
}EN_HDMI_PIX_CLK_TYPE;

typedef enum
{
    MSCombo_TMDS_COLOR_DEPTH_6_BIT  = 0,
    MSCombo_TMDS_COLOR_DEPTH_8_BIT  = 1,
    MSCombo_TMDS_COLOR_DEPTH_10_BIT = 2,
    MSCombo_TMDS_COLOR_DEPTH_12_BIT = 3,
    MSCombo_TMDS_COLOR_DEPTH_16_BIT = 4,
    MSCombo_TMDS_COLOR_DEPTH_UNKNOWN = 5,
    MSCombo_TMDS_COLOR_DEPTH_14_BIT = 6,
} MSCombo_TMDS_COLOR_DEPTH_FORMAT;

typedef enum
{
    MSCombo_TMDS_N0_PIX_REP = 0,
    MSCombo_TMDS_2x_PIX_REP = 1,
    MSCombo_TMDS_3x_PIX_REP = 2,
    MSCombo_TMDS_4x_PIX_REP = 3,
    MSCombo_TMDS_5x_PIX_REP = 4,
    MSCombo_TMDS_6x_PIX_REP = 5,
    MSCombo_TMDS_7x_PIX_REP = 6,
    MSCombo_TMDS_8x_PIX_REP = 7,
    MSCombo_TMDS_9x_PIX_REP = 8,
    MSCombo_TMDS_10x_PIX_REP = 9,
    MSCombo_TMDS_RESERVED_PIX_REP = 10,
} MSCombo_TMDS_PIXEL_REPETITION;


typedef enum
{
    HDMI_AUDIO_CONTENT_CTS = 0,
    HDMI_AUDIO_CONTENT_N,
}EN_HDMI_AUDIO_CONTENT_INFO;

typedef enum
{
    HDMI_VIDEO_CONTENT_HDE = 0,
    HDMI_VIDEO_CONTENT_VDE,
    HDMI_VIDEO_CONTENT_HTT,
    HDMI_VIDEO_CONTENT_VTT,
    HDMI_VIDEO_CLK_STABLE,
    HDMI_VIDEO_CLK_COUNT,
    HDMI_VIDEO_DE_STABLE,
    HDMI_VIDEO_COLOR_FROMAT,
    HDMI_VIDEO_COLOR_DEPTH,
    HDMI_VIDEO_COLOR_RANGE,
    HDMI_VIDEO_HDR_FLAG,
    HDMI_VIDEO_FREE_SYNC_FLAG,
    HDMI_VIDEO_AVMUTE_FLAG,
    HDMI_VIDEO_VRR_FLAG,
    HDMI_VIDEO_ALLM_FLAG,
    HDMI_VIDEO_HDMI_MODE_FLAG,
}EN_HDMI_VIDEO_CONTENT_INFO;

typedef struct
{
    EN_YPBPR_COLORIMETRY_TYPE                    ucColorimetry;
    EN_COLORIMETRY_TYPE                          ucExtColorimetry;
	EN_EXTENDED_ADDITIONAL_COLORIMETRY_TYPE      ucExtAddColorimetry;
}ST_HDMI_RX_COLOR_FORMAT;

typedef struct
{
    Bool bAudioFreqChangeFlag;
    WORD usAudioFrequency;
    DWORD ulAudioCTSNValue;
}ST_HDMI_RX_AUDIO_INFO;

typedef struct
{
    Bool bHDREnableFlag;
    Bool bYUV422To444EnableFlag;
    Bool bYUV420To444EnableFlag;
    BYTE ucColorFormat;
    EN_HDMI_INPUT_PORT enInputPortSelect;
}ST_HDMI_RX_HDR_INFO;

typedef struct
{
    //Bool bInitialFlag_R;
    //Bool bDEStableFlag_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    BYTE ucColorFormat_R; // COMBO_RX_TO_HDMITX_EVENT_GET_COLOR_INFO
    BYTE ucColorDepth_R; // COMBO_RX_TO_HDMITX_EVENT_GET_COLOR_INFO
    BYTE ucPixedRepetition_R; // COMBO_RX_TO_HDMITX_EVENT_GET_COLOR_INFO
    WORD usHDEValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usHTTValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVDEValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVTTValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usHSYNCValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVSYNCValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    //WORD usHFrontValue_R;   // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    //WORD usVFrontValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    //WORD usHBackValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    //WORD usVBackValue_R;     // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    //BYTE ucIP_mode_R;         // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    //WORD usAudioFrequency_R; // COMBO_RX_TO_HDMITX_EVENT_GET_AUDIO_INFO

    DWORD ulTMDSClockCount_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    //DWORD ulValidEventGroup_R;
    //DWORD ulRequestEventGroup_T;
    //DWORD ulAckEventGroup_T;
}ST_HDMI_RX_TIMING_INFO, *pST_RX_TIMING_INFO;

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

enum HDMI_EM_PACKET_STATUS_TYPE
{
    HDMI_EM_PACKET_DSC_ENABLE = BIT(0),
    HDMI_EM_PACKET_VRR_ENABLE = BIT(1),
    HDMI_EM_PACKET_VENDOR_ENABLE = BIT(2),
    HDMI_EM_PACKET_FVA_ENABLE = BIT(3),
    HDMI_EM_PACKET_HDR_ENABLE = BIT(4),
};

enum HDMI_FREE_SYNC_TYPE
{
    HDMI_FREE_SYNC_SUPPORTED = BIT(0),
    HDMI_FREE_SYNC_ENABLED = BIT(1),
    HDMI_FREE_SYNC_ACTIVE = BIT(2),
};

enum HDMI_SW_RESET_TYPE
{
    HDMI_SW_RESET_DVI = BIT(4),
    HDMI_SW_RESET_HDCP = BIT(5),
    HDMI_SW_RESET_HDMI = BIT(6),
    HDMI_SW_RESET_DVI2MIU = BIT(10),
    HDMI_SW_RESET_AUDIO_FIFO = BIT(15),
};

enum HDMI_INFO_SOURCE_TYPE
{
    HDMI_INFO_SOURCE0 = 0,
    HDMI_INFO_SOURCE1,
    HDMI_INFO_SOURCE2,
    HDMI_INFO_SOURCE3,
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

enum HDMI_HDCP_STATE
{
    HDMI_HDCP_NO_ENCRYPTION = 0,
    HDMI_HDCP_1_4,
    HDMI_HDCP_2_2,
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
    HDMI_AUTO_EQ_STABLE_DONE,
};

enum HDMI_AUDIO_FREQUENCY_TYPE
{
    HDMI_AUDIO_FREQUENCY_UNVALID = 0,
    HDMI_AUDIO_FREQUENCY_32K = 32,
    HDMI_AUDIO_FREQUENCY_44K = 44,
    HDMI_AUDIO_FREQUENCY_48K = 48,
    HDMI_AUDIO_FREQUENCY_88K = 88,
    HDMI_AUDIO_FREQUENCY_96K = 96,
    HDMI_AUDIO_FREQUENCY_176K = 176,
    HDMI_AUDIO_FREQUENCY_192K = 192,
};

enum TMDS_HDCP2_MESSAGE_ID_TYPE
{
    TMDS_HDCP2_MSG_ID_NONE = 0,
    TMDS_HDCP2_MSG_ID_AKE_INIT = 2,
    TMDS_HDCP2_MSG_ID_AKE_SEND_CERT = 3,
    TMDS_HDCP2_MSG_ID_AKE_NO_STORED_KM = 4,
    TMDS_HDCP2_MSG_ID_AKE_STORED_KM = 5,
    TMDS_HDCP2_MSG_ID_AKE_SEND_H_PRIME = 7,
    TMDS_HDCP2_MSG_ID_AKE_SEND_PAIRING_INFO = 8,
    TMDS_HDCP2_MSG_ID_LC_INIT = 9,
    TMDS_HDCP2_MSG_ID_LC_SEND_L_PRIME = 10,
    TMDS_HDCP2_MSG_ID_SKE_SEND_EKS = 11,
};

typedef enum
{
    TMDS_HDCP2_INFO_NONE = 0,
    TMDS_HDCP2_INFO_AKE_RTX,
    TMDS_HDCP2_INFO_AKE_TXCAPS,
    TMDS_HDCP2_INFO_AKE_CERTRX,
    TMDS_HDCP2_INFO_AKE_RRX,
    TMDS_HDCP2_INFO_AKE_RXCAPS,
    TMDS_HDCP2_INFO_AKE_EKPUB_KM,
    TMDS_HDCP2_INFO_AKE_EKH_KM_WR,
    TMDS_HDCP2_INFO_AKE_M,
    TMDS_HDCP2_INFO_AKE_H_PRIME,
    TMDS_HDCP2_INFO_AKE_EKH_KM_RD,
    TMDS_HDCP2_INFO_LC_RN,
    TMDS_HDCP2_INFO_LC_L_PRIME,
    TMDS_HDCP2_INFO_SKE_EDKEY_KS,
    TMDS_HDCP2_INFO_SKE_RIV,
} TMDS_HDCP2_INFORMATION_TYPE;

typedef enum
{
    MSCombo_TMDS_PORT_0      = 0,
    MSCombo_TMDS_PORT_1      = 1,
    MSCombo_TMDS_PORT_MAX      = 1,
} MSCombo_TMDS_PORT_INDEX;

enum COMBO_RX_EVENT_TYPE
{
    COMBO_RX_EVENT_NONE 				= 0,
    COMBO_RX_EVENT_GET_TIMING_INFO 	= 0,
    COMBO_RX_EVENT_GET_COLOR_INFO,
    COMBO_RX_EVENT_GET_SPD_INFO,
    COMBO_RX_EVENT_GET_AUDIO_INFO,
    COMBO_RX_EVENT_AUTO_UPDATE 		= COMBO_RX_EVENT_GET_AUDIO_INFO,
    COMBO_RX_EVENT_MAX,
};

typedef enum
{
    mhal_ePM_POWERON,
    mhal_ePM_STANDBY,
    mhal_ePM_POWEROFF,
    mhal_ePM_INVAILD
}mhal_HDMI_PM_Mode;

typedef enum
{
    HDMI_SWAP_TYPE_NONE = 0,
    HDMI_SWAP_TYPE_PN,
    HDMI_SAWP_TYPE_RB,
}HDMI_SWAP_TYPE;

Bool _Hal_tmds_CheckActiveCableProc(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo);
Bool _Hal_tmds_GetErrorCountStatus(BYTE enInputPortType, Bool bHDMI20Flag, stHDMI_ERROR_STATUS *pstErrorStatus);
Bool _Hal_tmds_ActiveCableCheckDLEV(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo);
Bool _Hal_tmds_DIGLockCheck(BYTE enInputPortType);
Bool Hal_HDMI_get_packet_value(BYTE enInputPortSelect, MS_HDMI_PACKET_STATE_t u8state, BYTE u8byte_idx, BYTE *pu8Value);
void Hal_HDMI_StablePolling(BYTE enInputPortSelect, DWORD *ulPacketStatus, ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo);
void Hal_tmds_BusyPolling(BYTE enInputPortType, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo);
void Hal_HDMI_Software_Reset(BYTE enInputPortSelect, WORD u16Reset);
void Hal_HDMI_pkt_reset(BYTE enInputPortSelect, HDMI_REST_t enResetType);
void Hal_HDCP_ClearStatus(BYTE enInputPortSelect, WORD usInt);
void Hal_HDMI_Audio_Status_Clear(ST_HDMI_RX_POLLING_INFO *stHDMIPollingInfo);
#if ENABLE_HPD_REPLACE_MODE
Bool Hal_HDMI_SCDC_status(BYTE enInputPortSelect);
void Hal_HDMI_SCDC_config(BYTE enInputPortSelect, BYTE bDDC);
void Hal_HDMI_SCDC_Clr(BYTE enInputPortSelect);
void Hal_HDMI_MAC_HDCP_Enable(Bool bEnable);
#endif
void Hal_HDMI_init(BYTE enInputPortSelect);
void Hal_HDMI_ClockRtermControl(BYTE enInputPortSelect, Bool bPullHighFlag);
void Hal_HDMI_DataRtermControl(BYTE enInputPortSelect, Bool bPullHighFlag);
void Hal_HDMI_HPDControl(BYTE enInputPortSelect, Bool bPullHighFlag);
void Hal_HDMI_WriteInputPortEDID(BYTE enInputPortSelect, BYTE ucEDIDSize, BYTE *pEDID);
void Hal_HDMI_AudioPollingProc(ST_HDMI_RX_AUDIO_INFO *pstHDMIRxAudioInfo);
void Hal_tmds_Switch2HDMI(BYTE hdmirx_id);
void Hal_HDMI_Audio_MUTE_Enable(DWORD u16MuteEvent, DWORD u16MuteMask);
void Hal_HDMI_SetPowerDown(BYTE enInputPortType, Bool bPowerDown);
BYTE Hal_HDMI_ComboPortMapping2DDCGPIOConfig(BYTE enInputPortType);
void Hal_HDMI_DDCControl(BYTE enInputPortType, Bool bEnable);
#if ENABLE_HDMI_BCHErrorIRQ
void Hal_HDMI_EnableHDMIRx_PKT_ParsrIRQ(Bool bEnable);
void Hal_HDMI_IRQMaskEnable(HDMI_IRQ_Mask enIRQType, Bool bEnable);
Bool Hal_HDMI_CheckIRQMaskEnable(HDMI_IRQ_Mask enIRQType);
void Hal_HDMI_IRQClearStatus(HDMI_IRQ_Mask enIRQType);
Bool Hal_HDMI_IRQCheckStatus(HDMI_IRQ_Mask enIRQType);
#endif
DWORD Hal_tmds_GetClockRatePort(MS_U8 enInputPortType, MS_U8 u8SourceVersion, EN_HDMI_PIX_CLK_TYPE enType);
void Hal_HDMI_PMEnableDVIDetect(BYTE enInputPortSelect, Bool bEnable, HDMI_PM_MODE_TYPE enPMModeType);
void Hal_HDMI_PM_SwitchHDMIPort(BYTE hdmirx_id);
Bool Hal_HDMI_GetDEStableStatus(BYTE enInputPortSelect);
Bool Hal_HDMI_GetAVMuteEnableFlag(BYTE enInputPortSelect);
Bool Hal_HDMI_GetVRREnableFlag(BYTE enInputPortSelect);
Bool Hal_HDMI_Get_InfoFrame(BYTE enInputPortSelect, MS_HDMI_PACKET_STATE_t u8state, void *pData, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo);
BYTE Hal_HDMI_GetFreeSyncInfo(BYTE enInputPortSelect, DWORD u32PacketStatus, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo __attribute__ ((unused)));
BYTE Hal_HDMI_avi_infoframe_info(BYTE enInputPortSelect, BYTE u8byte, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo);
MSCombo_TMDS_PIXEL_REPETITION KHal_HDMIRx_GetPixelRepetitionInfo(MS_U8 enInputPortType __attribute__ ((unused)));
BYTE Hal_HDMI_GetGCPColorDepth(BYTE enInputPortSelect, DWORD *u32PacketStatus);
BYTE Hal_HDMI_GetColorFormat(BYTE enInputPortSelect);
BYTE Hal_HDMI_GetColorRange(BYTE enInputPortSelect);
WORD Hal_HDCP_getstatus(BYTE enInputPortSelect);
WORD Hal_HDMI_gcontrol_info(HDMI_GControl_INFO_t gcontrol, BYTE enInputPortSelect);
WORD Hal_HDMI_GetDataInfo(E_HDMI_GET_DATA_INFO enInfo, BYTE enInputPortSelect);
DWORD Hal_HDMI_GetAudioContentInfo(BYTE enInputPortSelect, EN_HDMI_AUDIO_CONTENT_INFO enAudioContentInfo);
DWORD Hal_HDMI_packet_info(BYTE enInputPortSelect, ST_HDMI_RX_POLLING_INFO *pstHDMIPollingInfo);
ST_HDMI_RX_COLOR_FORMAT Hal_HDMI_GetColorimetry(BYTE enInputPortSelect);
BOOL mhal_tmds_HDCP2CheckWriteDone(MSCombo_TMDS_PORT_INDEX enInputPort);
BOOL mhal_tmds_HDCP2ReadDone(MSCombo_TMDS_PORT_INDEX enInputPort);
void mhal_tmds_HDCP2GetRxData(MSCombo_TMDS_PORT_INDEX enInputPort, BYTE *pHDCPData);
BOOL mhal_tmds_HDCP2CheckWriteStart(MSCombo_TMDS_PORT_INDEX enInputPort);
void mhal_tmds_HDCP2SetTxData(MSCombo_TMDS_PORT_INDEX enInputPort, BYTE MessageID, WORD DataSize, BYTE *pHDCPTxData, BYTE *pHDCPTxData_CertRx);
void mhal_tmds_HDCP2Initial(MSCombo_TMDS_PORT_INDEX enInputPort);
void mhal_tmds_HDCP2InterruptEnable(MSCombo_TMDS_PORT_INDEX enInputPort, Bool bEnableIRQ);
void Hal_HDMI_PHYPowerModeSetting(BYTE enInputPortSelect ,BYTE ucPMMode);
Bool mhal_tmds_HDCP2CheckRomCodeResult(void);
Bool mhal_tmds_HDMIGetErrorStatus(BYTE enInputPortType);
Bool mhal_tmds_HDMIGetBCHErrorStatus(BYTE enInputPortType);
BYTE mhal_hdmiRx_CheckHDCPState(void);
void _Hal_HDMI_set_PortSel_byGPIOConfig(BYTE u8HDMI_GPIO);
void _Hal_HDMI_set_DDC_Engine_byGPIOConfig(BYTE u8HDMI_GPIO);
BYTE Hal_HDMI_audio_channel_status(BYTE ucPortIndex, BYTE ucByte);
EN_TMDS_AUDIO_FORMAT Hal_HDMI_IsAudioFmtPCM(BYTE ucPortIndex);
MS_BOOL Hal_HDMIRx_GetSCDC_Tx5V_PwrDetectFlag(MS_U8 enInputPortType);
Bool Hal_HDMIRx_Set_RB_PN_Swap(BYTE ucPortIndex, HDMI_SWAP_TYPE enHDMI_SWAP_TYPE, Bool bSwapEnable);
MS_U8 KHal_HDMIRx_GetSCDCValue(MS_U8 enInputPortSelect, MS_U8 u8Offset);
void KHal_HDMIRx_SetSCDCValue(MS_U8 enInputPortSelect, MS_U8 u8Offset, MS_U8 u8SCDCValue);
void KHal_HDMIRx_CEDCheck(BYTE enInputPortType);


