// $Change: 1185839 $
//*************************************************************************************************
//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2014 MStar Semiconductor, Inc.
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
// rights to any and all damages, losses, costs and expenses resulting
// therefrom.
//*************************************************************************************************

#ifndef DPRXCOMMON_H
#define DPRXCOMMON_H

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "Global.h"
#include "types.h"
#include "HdcpCommon.h"
#include "HdcpHandler.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#ifdef TSUMR2_FPGA
#define FPGA_Verification					1
#define ASIC_Verification					0
#else
#define FPGA_Verification					0
#define ASIC_Verification					1
#endif

#define CTS_SINKSTATUS						0

#define XDATA_PROGRAMDPCD					1

#define DP_NORMAL_PORT0						BIT2
#define DP_NORMAL_PORT1						BIT3
#define DP_NORMAL_PORT2						BIT4
#define DP_TYPEC_PORT0      				BIT5

#define GET_DPRx_FUNCTION_ENABLE_PORT(a)	(Bool)((ucDPFunctionEnableIndex & (BIT0 << (a)))? TRUE : FALSE)
#define GET_DPRx_FUNCTION_ENABLE_INDEX()	(ucDPFunctionEnableIndex)

#define DPRX_M_DETECT_RANGE_PIXEL_CLK		600 // Unit: MHz
#define DPRX_AUIDO_UNMUTE_NORMAL			1 // 1: Unmute normal, 0: Can unmute fast (8 pin sound for CTS 5.4.4.2)

#define DP_VESA_ADAPTIVE_SYNC_SDP_SUPPORT				0x0

//*****************************************************************************
//		DP AUX
//*****************************************************************************
#define AUX_MCCS_SLAVE_ADDR		0x37

//*****************************************************************************
//		DP RX
//*****************************************************************************
#define DPRX_SUPPORT_TP3					1
#define DPRX_SUPPORT_TP4					0
#define DPRX_SUPPORT_DPCD2200				1

#define DPRX_DP14_HDR						ENABLE_HDR
#define DPRX_DP14_YUV420    		    	ENABLE_DP_YUV420
#define DPRX_SUPPORT_VSCSDP					(1 & (DPRX_DP14_HDR | DPRX_DP14_YUV420))
#define DPRX_M_RANGE_NEW_MODE               1
#define DPRX_M_RANGE_NEW_MODE_VALUE         6 //Mrange = M*0.6%
#define DP_REG_OFFSETAUX(a)					((a) == 0? 000 : (a) == 1? 0x300 : (a) == 2? 0xE00 : (a) == 3? 0x1100 : 0)
#define DP_REG_OFFSET000(a)					((a) * 0x000)
#define DP_REG_OFFSET002(a)					((a) * 0x002)
#define DP_REG_OFFSET010(a)					((a) * 0x010)
#define DP_REG_OFFSET020(a)					((a) * 0x020)
#define DP_REG_OFFSET028(a)					((a) * 0x028)
#define DP_REG_OFFSET040(a)					((a) * 0x040)
#define DP_REG_OFFSET100(a)					((a) * 0x100)
#define DP_REG_OFFSET200(a)					((a) * 0x200)
#define DP_REG_OFFSET300(a)					((a) * 0x300)
#define DP_REG_OFFSET400(a)					((a) * 0x400)
#define DP_REG_OFFSET600(a)					((a) * 0x600)
#define DP_REG_OFFSETA00(a)					((a) * 0xA00)

#define DP_GUID_SIZE						0x10
#define DPRX_AUX_REPLY_DELAY				0x2D2 // Normal case, unit is 1/xtal, it about 60us
#define DPRX_STATE_STABLE_DEBOUNCE_TIME		0x20

#define DPRx_MCCS_SUPPORT					1
#define DP_XDATA_PROGRAMMABLE_DPCD_MCCS		0

#define DPRX_OFFLINE_TO_ONLINE_KEEP_LOCK    0
#define ENABLE_DP_DCOFF_HPD_HIGH            1

//*****************************************************************************
//      Fast Training
//*****************************************************************************
#define DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL 	0x1
#define DP_RX_TRAINING_DEBOUNCE_TIME        20

//*****************************************************************************
//      Fake Training
//*****************************************************************************
#define Faketrain_swing                 DP_SWING2
#define Faketrain_pre                   DP_PREEMPHASIS0


//*****************************************************************************
//		TypeC RX
//*****************************************************************************
#define DP_TYPE_C_EN  					ENABLE_USB_TYPEC
#define DP_TYPE_C_PORT_NUM	 			1

//*****************************************************************************
//		Decoder
//*****************************************************************************
#define DP_SDP_PACKET_MAX_DATA_LENGTH		32
#define DP_SDP_PACKET_MAX_HEAD_LENGTH		4
#define DP_DSC_SDP_PACKET_MAX_DATA_LENGTH	128

//*****************************************************************************
//		DPCD
//*****************************************************************************
#define DPCD_00000		0x00000
#define DPCD_00001		0x00001
#define DPCD_00002		0x00002
#define DPCD_00003		0x00003
#define DPCD_00004		0x00004
#define DPCD_00005		0x00005
#define DPCD_00006		0x00006
#define DPCD_00007		0x00007
#define DPCD_00008		0x00008
#define DPCD_00009		0x00009
#define DPCD_0000A		0x0000A
#define DPCD_0000B		0x0000B
#define DPCD_0000C		0x0000C
#define DPCD_0000D		0x0000D
#define DPCD_0000E		0x0000E

#define DPCD_00021		0x00021
#define DPCD_00030		0x00030
#define DPCD_00080		0x00080

#define DPCD_00100		0x00100
#define DPCD_00101		0x00101
#define DPCD_00102		0x00102
#define DPCD_00103		0x00103
#define DPCD_00104		0x00104
#define DPCD_00105		0x00105
#define DPCD_00106		0x00106
#define DPCD_00107		0x00107
#define DPCD_00108		0x00108
#define DPCD_0010A		0x0010A
#define DPCD_0010B		0x0010B
#define DPCD_0010C		0x0010C
#define DPCD_0010D		0x0010D
#define DPCD_0010E		0x0010E

#define DPCD_00111		0x00111
#define DPCD_00112		0x00112
#define DPCD_00113		0x00113
#define DPCD_00114		0x00114
#define DPCD_00115		0x00115
#define DPCD_00118		0x00118
#define DPCD_00119		0x00119
#define DPCD_00120		0x00120
#define DPCD_00154		0x00154
#define DPCD_00155		0x00155
#define DPCD_00156		0x00156
#define DPCD_00157		0x00157
#define DPCD_00158		0x00158
#define DPCD_00159		0x00159
#define DPCD_0015A		0x0015A
#define DPCD_0015B		0x0015B
#define DPCD_00160		0x00160

#define DPCD_001A0		0x001A0
#define DPCD_001A1		0x001A1
#define DPCD_001C0		0x001C0
#define DPCD_001C1		0x001C1
#define DPCD_001C2		0x001C2

#define DPCD_00200		0x00200
#define DPCD_00201		0x00201
#define DPCD_00202		0x00202
#define DPCD_00203		0x00203
#define DPCD_00204		0x00204
#define DPCD_00205		0x00205
#define DPCD_00206		0x00206
#define DPCD_00207		0x00207

#define DPCD_00210		0x00210
#define DPCD_00218		0x00218
#define DPCD_00219		0x00219
#define DPCD_00220		0x00220

#define DPCD_00240		0x00240
#define DPCD_00241		0x00241
#define DPCD_00242		0x00242
#define DPCD_00243		0x00243
#define DPCD_00244		0x00244
#define DPCD_00245		0x00245

#define DPCD_00250		0x00250
#define DPCD_00260		0x00260
#define DPCD_00261		0x00261
#define DPCD_00270		0x00270
#define DPCD_002C0		0x002C0
#define DPCD_00300		0x00300
#define DPCD_00310		0x00310
#define DPCD_00311		0x00311

#define DPCD_00600		0x00600

#define DPCD_01000		0x01000
#define DPCD_01200		0x01200
#define DPCD_01400		0x01400
#define DPCD_01600		0x01600

#define DPCD_02002		0x02002
#define DPCD_02003		0x02003
#define DPCD_0200C		0x0200C
#define DPCD_0200D		0x0200D
#define DPCD_0200E		0x0200E
#define DPCD_0200F		0x0200F

#define DPCD_02200		0x02200
#define DPCD_02201		0x02201
#define DPCD_02202		0x02202
#define DPCD_02203		0x02203
#define DPCD_02204		0x02204
#define DPCD_02205		0x02205
#define DPCD_02206		0x02206
#define DPCD_02207		0x02207
#define DPCD_02208		0x02208
#define DPCD_02209		0x02209
#define DPCD_0220A		0x0220A
#define DPCD_0220B		0x0220B
#define DPCD_0220C		0x0220C
#define DPCD_0220D		0x0220D
#define DPCD_0220E		0x0220E
#define DPCD_0220F		0x0220F
#define DPCD_02210		0x02210
#define DPCD_02211		0x02211

#define DPCD_03051		0x03051
#define DPCD_03052		0x03052
#define DPCD_03054		0x03054
#define DPCD_03055		0x03055
#define DPCD_03056		0x03056
#define DPCD_03057		0x03057
#define DPCD_03058		0x03058
#define DPCD_03059		0x03059

#define DPCD_68000		0x68000
#define DPCD_68028		0x68028

//*****************************************************************************
//		HDCP
//*****************************************************************************
#define DPRX_HDCP14_Repeater_ENABLE		(1 & ENABLE_SECU_R2 & ENABLE_DP_OUTPUT)
#define DPRX_HDCP14_LONGHPD_WDT			650

#define DPRX_HDCP2_ENABLE				ENABLE_HDCP22
#define DPRx_HDCP2_ISR_MODE				(1 & DEF_HDCP2RX_ISR_MODE)

#define DP_EDID_SIZE_512				512
#define COMBO_HDCP_KEY_SIZE				284
#define COMBO_HDCP_BKSV_SIZE			5

#define DP_HDCP_ADDR_Bksv				0x68000
#define DP_HDCP_ADDR_R0_				0x68005 // R0'
#define DP_HDCP_ADDR_Aksv				0x68007
#define DP_HDCP_ADDR_An					0x6800C
#define DP_HDCP_ADDR_V_H0				0x68014	// V'H0
#define DP_HDCP_ADDR_V_H1				0x68018 // V'H1
#define DP_HDCP_ADDR_V_H2				0x6801C // V'H2
#define DP_HDCP_ADDR_V_H3				0x68020 // V'H3
#define DP_HDCP_ADDR_V_H4				0x68024 // V'H4
#define DP_HDCP_ADDR_Bcaps				0x68028
#define DP_HDCP_ADDR_Bstatus			0x68029
#define DP_HDCP_ADDR_Binfo				0x6802A
#define DP_HDCP_ADDR_KSV_FIFO			0x6802C
#define DP_HDCP_ADDR_Ainfo				0x6803B

#define DP_HDCP2_DPCD_HprimeReadDone	0x692DF
#define DP_HDCP2_DPCD_PairingReadDone	0x692EF
#define DP_HDCP2_DPCD_Rxinfo			0x69330

#define DPRX_STATE_CHECK_ENCRYPT_CNT	0x8 // Unit: 20 ms

#define diff(a, b)				(((a) > (b))? ((a) - (b)) : ((b) - (a)))
#define ABS_MINUS_DP(a, b)		(((a) > (b))? ((a) - (b)) : ((b) - (a)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum _DPRx_ID
{
	DPRx_ID_0 = 0x0,
	DPRx_ID_1 = 0x1,
	DPRx_ID_2 = 0x2,
	DPRx_ID_3 = 0x3,
	DPRx_ID_MAX
}DPRx_ID;

typedef enum _DPRx_AUX_ID
{
	DPRx_AUX_ID_0 = 0x0,
	DPRx_AUX_ID_1 = 0x1,
	DPRx_AUX_ID_2 = 0x2,
	DPRx_AUX_ID_MAX
}DPRx_AUX_ID;

typedef enum _DPRx_PHY_ID
{
	DPRx_PHY_ID_0 = 0x0,
	DPRx_PHY_ID_1 = 0x1,
	DPRx_PHY_ID_MAX
}DPRx_PHY_ID;

typedef enum _DPRx_DECODER_ID
{
	DPRx_DECODER_ID_0 = 0x0,
	DPRx_DECODER_ID_MAX
}DPRx_DECODER_ID;

typedef enum _DP_CHIP_MUX
{
	DP_CHIP_MUX_DECODE0 	   = 0x0,
	DP_CHIP_MUX_DECODE1 	   = 0x1,
	DP_CHIP_MUX_TX_ENCODE0 	   = 0x2,
	DP_CHIP_MUX_TX_TRANSMITTER = 0x3,
	DP_CHIP_MUX_MAX
}DP_CHIP_MUX;

typedef enum _DP_CHIP_MUX_DATA_PATH
{
	DP_CHIP_MUX_PATH_RECEIVER0 	   = 0x0,
	DP_CHIP_MUX_PATH_MST_TO_DECODE = 0x1,
	DP_CHIP_MUX_PATH_DECODE0 	   = 0x2, // Only used for DP_CHIP_MUX_TX_ENCODE0
	DP_CHIP_MUX_PATH_DECODE1 	   = 0x3, // Only used for DP_CHIP_MUX_TX_ENCODE0
	DP_CHIP_MUX_PATH_ENCODE0 	   = 0x4,
	DP_CHIP_MUX_PATH_TX_OUTPUT_MST = 0x5,
	DP_CHIP_MUX_PATH_MAX
}DP_CHIP_MUX_DATA_PATH;

typedef enum _DP_VERSION
{
	DP_VERSION_11 = 0x11,
	DP_VERSION_12 = 0x12,
	DP_VERSION_14 = 0x14,
	DP_VERSION_12_14 = 0x16,
	DP_VERSION_14_14 = 0x17,
	DP_VERSION_MAX
}DP_VERSION;

typedef enum _DP_LINKRATE
{
	DP_LINKRATE_RBR = 0x6,
	DP_LINKRATE_HBR = 0xA,
	DP_LINKRATE_HBR2 = 0x14,
	DP_LINKRATE_HBR25 = 0x19,
	DP_LINKRATE_HBR3 = 0x1E
}DP_LINKRATE;

typedef enum _DP_LANECOUNT
{
	DP_LANECOUNT_1 = 0x1,
	DP_LANECOUNT_2 = 0x2,
	DP_LANECOUNT_4 = 0x4,
}DP_LANECOUNT;

typedef enum _DP_SWING_NUM
{
    DP_SWING0 = 0x00,
    DP_SWING1 = 0x01,
    DP_SWING2 = 0x02,
    DP_SWING3 = 0x03,
}DP_SWING_NUM;

typedef enum _DP_PREEMPHASIS_NUM
{
    DP_PREEMPHASIS0 = 0x00,
    DP_PREEMPHASIS1 = 0x01,
    DP_PREEMPHASIS2 = 0x02,
    DP_PREEMPHASIS3 = 0x03,
}DP_PREEMPHASIS_NUM;

typedef enum _DPRx_HANDLER_STATE_TYPE
{
    DPRx_STATE_INITIAL,   // 0
    DPRx_STATE_RETRAIN,   // 1
    DPRx_STATE_STARTUP,   // 2
    DPRx_STATE_CKECKLOCK, // 3
    DPRx_STATE_MEASURE,   // 4
    DPRx_STATE_NORMAL,    // 5
    DPRx_STATE_POWERSAVE, // 6
    DPRx_STATE_DPIDLE     // 7
}DPRx_HANDLER_STATE_TYPE;

typedef enum _DP_HDCP_STATE
{
    DP_HDCP_NO_ENCRYPTION = 0,
    DP_HDCP_1_4 = 1,
    DP_HDCP_2_2 = 2
}DP_HDCP_STATE;

typedef enum
{
    DP_AUDIO_FMT_NON_PCM = 0,
    DP_AUDIO_FMT_PCM     = 1,
    DP_AUDIO_FMT_UNKNOWN = 2
}DP_AUDIO_FORMAT;

typedef enum
{
    DP_AVI_PACKET = 0, //(HB1 packet type = 0x82)
    DP_SPD_PACKET, //(HB1 packet type = 0x83)
    DP_HDR_PACKET, // HDR metadata, (HB1 packet type = 0x87)
    DP_AUDIO_PACKET, //(HB1 packet type = 0x84)
    DP_ACR_PACKET, //ACM (HB1 packet type = 0x5)
    DP_VSC_EXT_CEA_PACKET, //(HB1 packet type = 0x20)
    DP_VSC_EXT_VESEA_PACKET, //(HB1 packet type = 0x21)
    DP_AUDIO_TIMESTAMP, // (HB1 packet type = 0x1)
    DP_AUDIO_TIMESREAM // (HB1 packet type = 0x2)
}DP_PACKET_TYPE;

typedef enum _DP_SDP_SEL
{
	DP_SDP_SEL_AUDIO_TIMESTAMP = 0x0,
	DP_SDP_SEL_AUDIO_STREAM    = 0x1,
	DP_SDP_SEL_EXT 			   = 0x2,
	DP_SDP_SEL_ACM 			   = 0x3,
	DP_SDP_SEL_ISRC 		   = 0x4,
	DP_SDP_SEL_VSC			   = 0x5,
	DP_SDP_SEL_PPS			   = 0x6,
	DP_SDP_SEL_VSP 			   = 0x7,
	DP_SDP_SEL_AVI 			   = 0x8,
	DP_SDP_SEL_SPD 			   = 0x9,
	DP_SDP_SEL_AUI 			   = 0xA,
	DP_SDP_SEL_MPEG 		   = 0xB,
	DP_SDP_SEL_NTSC 		   = 0xC,
	DP_SDP_SEL_VSC_EXT_VESA    = 0x0D,
	DP_SDP_SEL_VSC_EXT_CEA 	   = 0x0E,
	DP_SDP_SEL_DRM 			   = 0x0F, // HDR metadata
	DP_SDP_SEL_MAX
}DP_SDP_SEL;

typedef enum _DP_VSC_EXT_TYPE
{
    VESA = 0x20,
    CEA = 0x21
}DP_VSC_EXT_TYPE;

typedef enum _DP_COLOR_FORMAT_TYPE
{
	DP_COLOR_FORMAT_RGB      = 0,     				// HDMI RGB 444 Color Format
	DP_COLOR_FORMAT_YUV_422  = 1,     				// HDMI YUV 422 Color Format
	DP_COLOR_FORMAT_YUV_444  = 2,     				// HDMI YUV 444 Color Format
	DP_COLOR_FORMAT_YUV_420  = 3,     				// HDMI YUV 420 Color Format
	DP_COLOR_FORMAT_YONLY    = 4,     				// DP Y Only Color Format
	DP_COLOR_FORMAT_RAW      = 5,     				// DP RAW color format
	DP_COLOR_FORMAT_RESERVED = 6,    	 			// Reserve
	DP_COLOR_FORMAT_DEFAULT  = DP_COLOR_FORMAT_RGB, // Default setting
	DP_COLOR_FORMAT_UNKNOWN  = 15  					// Unknow color format
}DP_COLOR_FORMAT_TYPE;

typedef enum _DP_COLORIMETRY_TYPE
{
    DP_COLORIMETRY_xvYCC601       = 0, // HDMI xvYCC601 colorimetry
    DP_COLORIMETRY_xvYCC709       = 1, // HDMI xvYCC709 colorimetry
    DP_COLORIMETRY_sYCC601        = 2, // HDMI sYCC601 colorimetry
    DP_COLORIMETRY_ADOBEYCC601    = 3, // HDMI Adobe YCC 601 colorimetry
    DP_COLORIMETRY_ADOBERBG       = 4, // HDMI Adobe RGB colorimetry
    DP_COLORIMETRY_BT2020YcCbcCrc = 5, // HDMI BT2020 YcCbcCrc colorimetry
    DP_COLORIMETRY_BT2020RGBYCbCr = 6, // HDMI BT2020 RGB or YCbCr colorimetry
    DP_COLORIMETRY_ITU601         = 7,
    DP_COLORIMETRY_ITU709         = 8,
    DP_COLORIMETRY_NONE        	  = 15
}DP_COLORIMETRY_TYPE;

typedef enum _DP_COLOR_RANGE_TYPE
{
    DP_COLOR_RANGE_DEFAULT  = 0, // Depend on video format
    DP_COLOR_RANGE_LIMIT    = 1,
    DP_COLOR_RANGE_FULL     = 2,
    DP_COLOR_RANGE_RESERVED = 3
}DP_COLOR_RANGE_TYPE;

typedef enum _DP_COLOR_DEPTH_TYPE
{
    DP_COLOR_DEPTH_6BIT = 0,
    DP_COLOR_DEPTH_8BIT,
    DP_COLOR_DEPTH_10BIT,
    DP_COLOR_DEPTH_12BIT,
    DP_COLOR_DEPTH_16BIT,
    DP_COLOR_DEPTH_NONE,
}DP_COLOR_DEPTH_TYPE;

typedef enum _DP_YPBPR_COLORIMETRY_TYPE
{
    DP_YUV_COLORIMETRY_ITU601 = 0,
    DP_YUV_COLORIMETRY_ITU709,
    DP_YUV_COLORIMETRY_NoData,
    DP_YUV_COLORIMETRY_EC,
    DP_YUV_COLORIMETRY_MAX
}DP_YPBPR_COLORIMETRY_TYPE;

typedef enum
{
    DP_ePM_POWERON = 0x0,
    DP_ePM_STANDBY,
    DP_ePM_POWEROFF,
    DP_ePM_POWEROFF_NoUsedPort,
    DP_ePM_INVAILD,
}DP_ePM_Mode;

typedef enum _DP_HDCP2_MESSAGE_ID_TYPE
{
    DP_HDCP2_MSG_ID_NONE 					 = 0,
	DP_HDCP2_MSG_ID_AUTHDONE 				 = 1,
    DP_HDCP2_MSG_ID_AKE_INIT 				 = 2,
    DP_HDCP2_MSG_ID_AKE_SEND_CERT 			 = 3,
    DP_HDCP2_MSG_ID_AKE_NO_STORED_KM 		 = 4,
    DP_HDCP2_MSG_ID_AKE_STORED_KM 			 = 5,
    DP_HDCP2_MSG_ID_AKE_H_PRIME_READONE 	 = 6,
    DP_HDCP2_MSG_ID_AKE_SEND_H_PRIME 		 = 7,
    DP_HDCP2_MSG_ID_AKE_SEND_PAIRING_INFO 	 = 8,
    DP_HDCP2_MSG_ID_LC_INIT 				 = 9,
    DP_HDCP2_MSG_ID_LC_SEND_L_PRIME 		 = 10,
    DP_HDCP2_MSG_ID_SKE_SEND_EKS 			 = 11,
    DP_HDCP2_MSG_ID_REPAUTH_SEND_RECVID_LIST = 12,
    DP_HDCP2_MSG_ID_RTT_READY                = 13,
    DP_HDCP2_MSG_ID_RTT_CHALLENGE            = 14,
    DP_HDCP2_MSG_ID_REPAUTH_SEND_ACK         = 15,
    DP_HDCP2_MSG_ID_REPAUTH_STREAM_MANAGE    = 16,
    DP_HDCP2_MSG_ID_REPAUTH_STREAM_READY     = 17,
    DP_HDCP2_MSG_ID_RECEIVER_AUTH_STATUS     = 18,
    DP_HDCP2_MSG_ID_AKE_TRANSMITTER_INFO     = 19,
    DP_HDCP2_MSG_ID_AKE_RECEIVER_INFO        = 20
}DP_HDCP2_MESSAGE_ID_TYPE;

typedef enum _DP_HDCP2_INFORMATION_TYPE
{
    DP_HDCP2_INFO_NONE = 0,
    DP_HDCP2_INFO_AKE_RTX,
    DP_HDCP2_INFO_AKE_TXCAPS,
    DP_HDCP2_INFO_AKE_CERTRX,
    DP_HDCP2_INFO_AKE_RRX,
    DP_HDCP2_INFO_AKE_RXCAPS,
    DP_HDCP2_INFO_AKE_EKPUB_KM,
    DP_HDCP2_INFO_AKE_EKH_KM_WR,
    DP_HDCP2_INFO_AKE_M,
    DP_HDCP2_INFO_AKE_H_PRIME,
    DP_HDCP2_INFO_AKE_EKH_KM_RD,
    DP_HDCP2_INFO_LC_RN,
    DP_HDCP2_INFO_LC_L_PRIME,
    DP_HDCP2_INFO_SKE_EDKEY_KS,
    DP_HDCP2_INFO_SKE_RIV,
    DP_HDCP2_INFO_RECVLIST_RXINFO,
    DP_HDCP2_INFO_RECVLIST_SEQ_NUM_V,
    DP_HDCP2_INFO_RECVLIST_VPRIME,
    DP_HDCP2_INFO_RECVLIST_RECEIVER_ID_LIST,
    DP_HDCP2_INFO_ACK_SEND_ACK_V,
    DP_HDCP2_INFO_STREAM_SEQ_NUM_M,
    DP_HDCP2_INFO_STREAM_k,
    DP_HDCP2_INFO_STREAM_STREAM_ID_TYPE,
    DP_HDCP2_INFO_STREAM_MPRIME
}DP_HDCP2_INFORMATION_TYPE;

typedef enum _DP_HDCP2_RXSTATUS_TYPE
{
    DP_HDCP2_RXSTATUS_READY 				 = BIT0,
    DP_HDCP2_RXSTATUS_H_PRIME_AVAILABLE 	 = BIT1,
    DP_HDCP2_RXSTATUS_PAIRING_AVAILABLE 	 = BIT2,
    DP_HDCP2_RXSTATUS_REAUTH_REQ 			 = BIT3,
    DP_HDCP2_RXSTATUS_LINK_INTEGRITY_FAILURE = BIT4
}DP_HDCP2_RXSTATUS_TYPE;

typedef enum _DPRx_MCCS_WAKEUP
{
    DPRx_MCCS_D1 = 0x0,
    DPRx_MCCS_D4 = 0x1,
    DPRx_MCCS_D5 = 0x2,
    DPRx_MCCS_MAX
}DPRx_MCCS_WAKEUP;

typedef BOOL(*DPRx_HDCP2_CB_FUNC)(BYTE, BYTE*); // Need sync mapi_dprx.h "DPRx_HDCP2_CALL_BACK_FUNC"
typedef void(*DPRX_HDCP2_Config_Repeater_CB_FUNC)(BYTE, BYTE, BOOL);
typedef BOOL(*DPRx_HDCP14_CHECK_VPRIME_CB_FUNC)(BYTE);
typedef void(*DPRx_HDCP14AKSVReceived_CB_FUNC)(BYTE);
#endif

