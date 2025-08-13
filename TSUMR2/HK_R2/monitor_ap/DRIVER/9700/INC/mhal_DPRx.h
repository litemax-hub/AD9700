///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mhal_DPRx.h
/// @author MStar Semiconductor Inc.
/// @brief  DP Rx driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_DPRX_H_
#define _MHAL_DPRX_H_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "dpCommon.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#ifndef BIT
#define BIT(_bit_)                          (1 << (_bit_))
#endif

#ifndef BMASK
#define BMASK(_bits_)                       (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))
#endif

#define DP_DEBUG_MESSAGE					1

#define DP_EDID_SIZE_512					512

#define DP_SHORTHPD_TIMEOUT_CNT				100

#define DP_AUX_TIMEOUT_CNT					20000
#define DPRX_AUX_COMMAND_DDC_LENGTH			3
#define DPRX_AUX_COMMAND_MAX_LENGTH			16
#define DP_AUX_DELAY_NORMAL					0x284 // 53us delay reply
#define DP_AUX_DELAY_TRAINING				0x284 // 53us delay reply

#define DPRX_FASTTRAING_SW					0

#define DP_AUTO_EQ_ENABLE					1
#define DPRX_AUTOEQ_Lane0DoneOnly			1 // 1: only lane 0,  0: lane 0 ~ lane 3

#define DP_INVALID_M_VALUE					0xA

#define DPRX_AUDIO_CS_STABLE				0x174
#define DPRX_AUDIO_CS_STABLE1				0x74
#define DPRX_AUDIO_PHASE_DIFF_RANGE			0xF
#define DPRX_AUDIO_PHASE_DIFF_RESET			0x70


#define DP_AUX_TIMEOUTCNT					2000 // (times)
//#define DP_CLEAR_COUNT						10
#define DP_HBR23_EQ_INIT					0x19
#define DP_HBR23_EQ_MAX						0x22
#define DP_HBRRBR_EQ_INIT					0x1
#define DP_HBRRBR_EQ_MAX					0x1
#define DP_AUX_COMMAND_MAX_LENGTH			16
#define DP_AUX_COMMAND_DDC_LENGTH			3
#define DPRX_PROGRAM_DPCD_OFFSET			0x4
#define DPRX_XDataQueue						(2*DP_AUX_COMMAND_MAX_LENGTH)

#define DPRX_PROGRAM_DPCD0_ENABLE			1
#define DPRX_PROGRAM_DPCD1_ENABLE			0
#define DPRX_PROGRAM_DPCD2_ENABLE			0
#define DPRX_PROGRAM_DPCD3_ENABLE			0
#define DPRX_PROGRAM_DPCD4_ENABLE			DP_VESA_ADAPTIVE_SYNC_SDP_SUPPORT
#define DPRX_PROGRAM_DPCD5_ENABLE			0
#define DPRX_PROGRAM_DPCD6_ENABLE			0
#define DPRX_PROGRAM_DPCD7_ENABLE			0
#define DPRX_PROGRAM_DPCD8_ENABLE			1
#define DPRX_PROGRAM_DPCD9_ENABLE			0
#define DPRX_PROGRAM_DPCD10_ENABLE			0
#define DPRX_PROGRAM_DPCD11_ENABLE			0
#define DPRX_XDATA_PROGRAM_DPCD0_ENABLE		1
#define DPRX_XDATA_PROGRAM_DPCD1_ENABLE		(0 && (DP_XDATA_PROGRAMMABLE_DPCD_MCCS == 0))

#define DPRX_PROGRAM_DPCD0_ADDERSS			0x00030 // I2C 0x30
#define DPRX_PROGRAM_DPCD1_ADDERSS			0xFFFFF //0x68029
#define DPRX_PROGRAM_DPCD2_ADDERSS			0xFFFFF
#define DPRX_PROGRAM_DPCD3_ADDERSS			0xFFFFF
#define DPRX_PROGRAM_DPCD4_ADDERSS			0x02210
#define DPRX_PROGRAM_DPCD5_ADDERSS			0xFFFFF
#define DPRX_PROGRAM_DPCD6_ADDERSS			0xFFFFF
#define DPRX_PROGRAM_DPCD7_ADDERSS			0xFFFFF
#define DPRX_PROGRAM_DPCD8_ADDERSS			0x00100
#define DPRX_PROGRAM_DPCD9_ADDERSS			0xFFFFF //0x00700
#define DPRX_PROGRAM_DPCD10_ADDERSS			0xFFFFF
#define DPRX_PROGRAM_DPCD11_ADDERSS			0xFFFFF
#define DPRX_XDATA_PROGRAM_DPCD0_ADDERSS	0x00030
#define DPRX_XDATA_PROGRAM_DPCD1_ADDERSS	0xFFFFF

//====================================================================
//		Sink DEVICE SPecific Field
//====================================================================
#define IEEE_OUI_FIRST						0x00
#define IEEE_OUI_SECOND						0x0C
#define IEEE_OUI_THIRD						0xE7

#define DEVICE_STRING_0             		0x00
#define DEVICE_STRING_1             		0x00
#define DEVICE_STRING_2             		0x00
#define DEVICE_STRING_3             		0x00
#define DEVICE_STRING_4             		0x00
#define DEVICE_STRING_5             		0x00

#if (DPRX_HDCP2_ENABLE == 0x1)
//=============================================================================
//		HDCP2
//=============================================================================
#define DP_HDCP2_AKE_RTX_SIZE						8
#define DP_HDCP2_AKE_TXCAPS_SIZE					3
#define DP_HDCP2_AKE_CERTRX_SIZE					522
#define DP_HDCP2_AKE_RRX_SIZE						8
#define DP_HDCP2_AKE_RXCAPS_SIZE					3
#define DP_HDCP2_AKE_EKPUB_KM_SIZE					128
#define DP_HDCP2_AKE_EKH_KM_WR_SIZE					16
#define DP_HDCP2_AKE_M_SIZE							16
#define DP_HDCP2_AKE_H_PRIME_SIZE					32
#define DP_HDCP2_AKE_EKH_KM_RD_SIZE					16
#define DP_HDCP2_LC_RN_SIZE							8
#define DP_HDCP2_LC_L_PRIME_SIZE					32
#define DP_HDCP2_SKE_EDKEY_KS_SIZE					16
#define DP_HDCP2_SKE_RIV_SIZE						8
#define DP_HDCP2_ACK_SEND_ACK_V_SIZE				16
#define DP_HDCP2_STREAM_SEQ_NUM_M_SIZE				3
#define DP_HDCP2_STREAM_k_SIZE						2
#define DP_HDCP2_STREAM_STREAM_ID_TYPE_SIZE			2
#define DP_HDCP2_RECVLIST_RXINFO_SIZE				2
#define DP_HDCP2_RECVLIST_SEQ_NUM_V_SIZE			3
#define DP_HDCP2_RECVLIST_VPRIME_SIZE				16
#define DP_HDCP2_RECVLIST_RECEIVER_ID_LIST_SIZE		155
#define DP_HDCP2_STREAM_MPRIME_SIZE					32
#define DP_HDCP2_RECEIVER_ID_SIZE					5
#endif

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum _DPRx_XDATAPROGRAM_DPCD_TYPE
{
	DPRx_XDATAPROGRAM_DPCD_0 = 0,
	DPRx_XDATAPROGRAM_DPCD_1 = 1,
	DPRx_XDATAPROGRAM_DPCD_MAX
}DPRx_XDATAPROGRAM_DPCD_TYPE;

typedef enum _DPRx_Command_TYPE
{
	DPRx_NATIVE_AUX = 0,
	DPRx_I2C 		= 1
}DPRx_Command_TYPE;

typedef enum _DPRx_AudioStream
{
	DPRx_AUD_STREAM_0 = 0,
    DPRx_AUD_STREAM_1,
    DPRx_AUD_STREAM_MAX
}DPRx_AudioStream;

typedef enum _DPRx_AudioFreqType
{
    DPRx_AUD_FREQ_32K 	= 32,
    DPRx_AUD_FREQ_44K 	= 44,
    DPRx_AUD_FREQ_48K 	= 48,
    DPRx_AUD_FREQ_88K 	= 88,
    DPRx_AUD_FREQ_96K 	= 96,
    DPRx_AUD_FREQ_176K  = 176,
    DPRx_AUD_FREQ_192K  = 192,
    DPRx_AUD_FREQ_ERROR = 0
}DPRx_AudioFreqType;

typedef enum _DPRx_COLOR_FORMAT_TYPE
{
    DPRx_COLOR_FORMAT_RGB      = 0,     				// HDMI RGB 444 color format
    DPRx_COLOR_FORMAT_YUV_422  = 1,     				// HDMI YUV 422 color format
    DPRx_COLOR_FORMAT_YUV_444  = 2,     				// HDMI YUV 444 color format
    DPRx_COLOR_FORMAT_YUV_420  = 3,     				// HDMI YUV 420 color format
    DPRx_COLOR_FORMAT_RESERVED = 4,     				// Reserve
    DPRx_COLOR_FORMAT_DEFAULT  = DPRx_COLOR_FORMAT_RGB, // Default setting
    DPRx_COLOR_FORMAT_UNKNOWN  = 15    					// Unknow color format
}DPRx_COLOR_FORMAT_TYPE;

typedef enum _DPRx_PROGRAM_DPCD_TYPE
{
	DPRx_PROGRAM_DPCD_0  = 0,
	DPRx_PROGRAM_DPCD_1  = 1,
	DPRx_PROGRAM_DPCD_2  = 2,
	DPRx_PROGRAM_DPCD_3  = 3,
	DPRx_PROGRAM_DPCD_4  = 4,
	DPRx_PROGRAM_DPCD_5  = 5,
	DPRx_PROGRAM_DPCD_6  = 6,
	DPRx_PROGRAM_DPCD_7  = 7,
	DPRx_PROGRAM_DPCD_8  = 8,
	DPRx_PROGRAM_DPCD_9  = 9,
	DPRx_PROGRAM_DPCD_10 = 10,
	DPRx_PROGRAM_DPCD_11 = 11,
	DPRx_PROGRAM_DPCD_MAX
}DPRx_PROGRAM_DPCD_TYPE;

typedef enum _DPRx_SDP_TYPE
{
	DPRx_SDP_SEL_AUDIO_TIMESTAMP = 0x0,
	DPRx_SDP_SEL_AUDIO_STREAM 	 = 0x1,
	DPRx_SDP_SEL_EXT 			 = 0x2,
	DPRx_SDP_SEL_ACM 			 = 0x3,
	DPRx_SDP_SEL_ISRC 			 = 0x4,
	DPRx_SDP_SEL_VSC 			 = 0x5,
	DPRx_SDP_SEL_VSC_EXT 		 = 0x6,
	DPRx_SDP_SEL_VSP 			 = 0x7,
	DPRx_SDP_SEL_AVI 			 = 0x8,
	DPRx_SDP_SEL_SPD 			 = 0x9,
	DPRx_SDP_SEL_AUI 			 = 0xA,
	DPRx_SDP_SEL_MPEG 			 = 0xB,
	DPRx_SDP_SEL_NTSC 			 = 0xC,
	DPRx_SDP_VSC_EXT_VESA 		 = 0x0D,
	DPRx_SDP_VSC_EXT_CEA 		 = 0x0E,
	DPRx_SDP_DRM 				 = 0x0F, // HDR
	DPRx_SDP_SEL_MAX 			 = 0x10
}DPRx_SDP_TYPE;

typedef enum _DPRx_3D_INPUT_MODE
{
    // Range [0000 ~ 1111] reserved for DP 3D spec
    DPRx_3D_INPUT_NO_STEREO         = 0x00, // 0000
    DPRx_3D_INPUT_FRAME_SEQUENTIAL  = 0x01, // 0001, /* page flipping */
    DPRx_3D_INPUT_STACK_FRAME       = 0x02, // 0002, // Frame Packing
    DPRx_3D_INPUT_PIXEL_INTERLEAVED = 0x03, // 0003, /* line interleave */
    DPRx_3D_INPUT_SIDE_BY_SIDE_FULL = 0x04, // 0004, /* side by side full */
    DPRx_3D_RESERVED                = 0x05  // 0005, 101 ~ 1111
}DPRx_3D_INPUT_MODE;

typedef enum _DPRx_3D_DATA_MODE
{
    DPRx_3D_PIXELINTERLEAVE_LLRR = 0x00, // 0000
    DPRx_3D_PIXELINTERLEAVE_RRLL = 0x01, // 0001
    DPRx_3D_PIXELINTERLEAVE_LRRL = 0x02, // 0002
    DPRx_3D_PIXELINTERLEAVE_LRLR = 0x03, // 0003
    DPRx_3D_PIXELINTERLEAVE_RLRL = 0x04, // 0004
    DPRx_3D_SIDEBYSIDE_LR        = 0x05, // 0005
    DPRx_3D_SIDEBYSIDE_RL        = 0x06, // 0006
    DPRx_3D_DATA_RESERVED
}DPRx_3D_DATA_MODE;

typedef struct
{
    BYTE ucKmValue;
    BYTE ucKpValue;
    WORD usDivValue;
    DWORD ulMCodeValue;
    DWORD ulNCodeValue;
}DPRx_stDPAudioInfo;

typedef struct
{
    DWORD ulMCodeValue;
    DWORD ulNCodeValue;
}DPRx_stDPVideoInfo;

typedef enum _DP_DECODER_OVERWRITE_SEL
{
    DP_DECODER_OVERWRITE_NONE       = 0,
    DP_DECODER_OVERWRITE_HSP_HSW    = BIT0,
    DP_DECODER_OVERWRITE_VSP_VSW    = BIT1,
    DP_DECODER_OVERWRITE_HWIDTH     = BIT2,
    DP_DECODER_OVERWRITE_HTOTAL     = BIT3,
    DP_DECODER_OVERWRITE_VHEIGHT    = BIT4,
    DP_DECODER_OVERWRITE_VTOTAL     = BIT5,
    DP_DECODER_OVERWRITE_MISC0      = BIT6,
    DP_DECODER_OVERWRITE_MISC1      = BIT7,
    DP_DECODER_OVERWRITE_VBID       = BIT8,
    DP_DECODER_OVERWRITE_LANE_COUNT = BIT9
}DP_DECODER_OVERWRITE_SEL;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void mhal_DPRx_DELAY_NOP(DWORD msNums);
void mhal_DPRx_DELAY_1ms(void);

void ________INIT________(void);
void mhal_DPRx_Initial(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DPRx_PHY_ID dprx_phy_id);
void mhal_DPRx_Initial_Decoder(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_InitialSetting_3(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_DecodeInitialSetting_3(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_AuxInitialSetting(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_ReceiverInitialSetting(DPRx_ID dprx_id);
void mhal_DPRx_FastTrainingInitialSetting(DPRx_ID dprx_id);
void mhal_DPRx_TopGpInitialSetting(void);
void mhal_DPRx_OUIInitialSetting(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_PLLInitialSetting(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_DecodeInitialSetting(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_VersionSetting(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BYTE ucVersion);
void mhal_DPRx_SetupInputPort(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
BOOL mhal_DPRx_CableGND_Level(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_AUX_N_Level(DPRx_AUX_ID dprx_aux_id);

void ________VIDEO________(void);
void mhal_DPRx_EDIDOffsetSetting(void);
void mhal_DPRx_Load_EDID(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id,const BYTE *EdidData);
void mhal_DPRx_SetInternalDPVersion(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BYTE ucValue, BYTE ucDPVersion);
BYTE mhal_DPRx_GetDPVersion(DPRx_AUX_ID dprx_aux_id);
WORD mhal_DPRx_GetLinkRate(DPRx_ID dprx_id);
BYTE mhal_DPRx_GetDPCDLinkRate(DPRx_AUX_ID dprx_aux_id);
BYTE mhal_DPRx_GetDPLaneCnt(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_MCUWriteOUIDPCD(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucDATA);
void mhal_DPRx_EnableGUIDWrite(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_GetGUIDValue(DPRx_AUX_ID dprx_aux_id, BYTE *pGUID);
BOOL mhal_DPRx_CheckSquelch(DPRx_ID dprx_id);
void mhal_DPRx_ResetAsyncFIFO(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_SetReferenceClock(DPRx_DECODER_ID dprx_decoder_id, WORD uwReferClock);
void mhal_DPRx_UpdateMVIDValue(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_UpdateMAUDValue(DPRx_DECODER_ID dprx_decoder_id);
DWORD mhal_DPRx_GetHTT(DPRx_DECODER_ID dprx_decoder_id);
DWORD mhal_DPRx_GetVTT(DPRx_DECODER_ID dprx_decoder_id);
DWORD mhal_DPRx_GetHWidth(DPRx_DECODER_ID dprx_decoder_id);
DWORD mhal_DPRx_GetVWidth(DPRx_DECODER_ID dprx_decoder_id);
WORD mhal_DPRx_GetTimingPixelClock(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id);
WORD mhal_DPRx_GetTimingPixelClock10K(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id);
BOOL mhal_DPRx_GetTimingInformation(DPRx_DECODER_ID dprx_decoder_id, WORD *usTimingInfo);
void mhal_DPRx_EnableAutoInterlace(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
BOOL mhal_DPRx_GetInterlaceFlag(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_GetMISC01(DPRx_DECODER_ID dprx_decoder_id, BYTE *usMISCInfo);
void mhal_DPRx_EnableDRRFunction(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
BOOL mhal_DPRx_GetDRRFlag(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_EnableRegenSync(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetRegenSync(DPRx_ID dprx_id, BYTE Value);
void mhal_DPRx_RegenTimingInformation(DPRx_DECODER_ID dprx_decoder_id, WORD *usTimingInfo);
BOOL mhal_DPRx_GetHwHtotalLearningStable(DPRx_AUX_ID dprx_aux_id, DPRx_DECODER_ID dprx_decoder_id);
BOOL mhal_DPRx_IsInputStreamValid(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_OverwriteVBlankingData(DPRx_DECODER_ID dprx_decoder_id, BYTE ucDPColorFormat, BYTE ucDPRangeFormat);
WORD mhal_DPRx_GetPacketStatus(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_ClearPacketStatus(DPRx_DECODER_ID dprx_decoder_id, DP_SDP_SEL PacketType);
BOOL mhal_DPRx_GetPacketData(DPRx_DECODER_ID dprx_decoder_id, DP_SDP_SEL PacketType, BYTE *pHeader, BYTE *pPacketData, BYTE ubDataLength);
void mhal_DPRx_GetVSCSDP(DPRx_ID dprx_id, BYTE *usMISCInfo);
BOOL mhal_DPRx_GetVSC_EXT(DPRx_ID dprx_id, DP_VSC_EXT_TYPE Type, BYTE *VSC_EXT);
BOOL mhal_DPRx_GetTrainingPatternFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_CheckReceiveIdlePattern(DPRx_DECODER_ID dprx_decoder_id);
BOOL mhal_DPRx_CheckDPCDPowerState(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_CheckCDRLock(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DPRx_PHY_ID dprx_phy_id, DPRx_ID OnlinePort);
BOOL mhal_DPRx_CheckLossCDRStatus(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_CheckInterlaneSkewStatus(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_CheckLossAlignment(DPRx_ID dprx_id);
BOOL mhal_DPRx_CheckVPLLBigChange(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_ClrVideoBigChgFlag(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_EnableVspHwRegen(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_DecoderOverwrite_Set(DPRx_DECODER_ID dprx_decoder_id, BOOL bOverwriteEnable, WORD usTargetValue);
DWORD mhal_DPRx_LongDIV(DWORD M_Value, WORD LinkRate, DWORD N_Value);
void mhal_DPRx_EnableSEReplaceByBE(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);

void ________AUDIO________(void);
void mhal_DPRx_AUPLLSetting(void);
BOOL mhal_DPRx_CheckAUPLLBigChange(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_SetAUPLLBigChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetAudioMuteInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetAudioChanneStatusChgInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetMISC0ChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetMISC1ChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_UpdateAudioMNCode(DPRx_DECODER_ID dprx_decoder_id, DWORD ulAudioMCode, DWORD ulAudioNCode);
BYTE mhal_DPRx_AudioChannelCnt(DPRx_DECODER_ID dprx_decoder_id, BYTE *pPacketData);
void mhal_DPRx_SetAudioMaskCount(DPRx_ID dprx_id, BYTE ucMaskCount);
BOOL mhal_DPRx_CheckAudioAbsent(DPRx_DECODER_ID dprx_decoder_id, BYTE times);
BYTE mhal_DPRx_GetAudioSampleFreq(DPRx_DECODER_ID dprx_decoder_id);
BYTE mhal_DPRx_CalAudioFreqByIVS(DPRx_ID dprx_id);
BYTE mhal_DPRx_CalAudioFreq(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id);
BOOL mhal_DPRx_AudioCheckStable(DPRx_ID dprx_id, BYTE Times);
void mhal_DPRx_AudioCheckFIFOStatus(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetAudioBigChgFlag(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_ClrAudioBigChgFlag(DPRx_DECODER_ID dprx_decoder_id);
BYTE mhal_DPRx_SetAudioPath(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id, DPRx_AudioStream dprx_audio_stream, BOOL ubEnable);
BOOL mhal_DPRx_GetAudioGlobeMute(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetNoAudioFlag(DPRx_DECODER_ID dprx_decoder_id);
DPRx_AudioStream mhal_DPRx_GetAudioStream(DPRx_ID dprx_id);
DP_AUDIO_FORMAT mhal_DPRx_IsAudioFmtPCM(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_AudioGlobalMute(DPRx_AudioStream dprx_audio_select, BOOL bEnable);
BYTE mhal_DPRx_AudioStreamHB3(DPRx_DECODER_ID dprx_decoder_id);
void mhal_DPRx_AudioAlockReset(DPRx_AudioStream dprx_audio_select);
void mhal_DPRx_AudioFPLLReset(DPRx_AudioStream dprx_audio_select);

void ________INTERRUPT________(void);
void mhal_DPRx_EnableAuxInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_EnableMCCSInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_EnableAuxInterruptSetting(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_EnableDPAUXRecvInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
BOOL mhal_DPRx_AuxTimeoutInterruptFlag_Get(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_AuxTimeoutInterruptFlag_Clear(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_EnableReceiverInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetCDRLossLockInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetMainLinkLossLockInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetInterlaneSkewLoseInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetInterlaneSkewDoneInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetClkLsBigChangeInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableFTInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetTrainingPattern1Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetTrainingPattern2Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetTrainingPattern3Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetTrainingPattern4Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableDPCDTrainingInterrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableDPCD102Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableDPCD103Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableDPCD202Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableDPCD270Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetVPLLBigChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetSDCInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetVideoBufferOverflowInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SetMSAChgInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_MSAChgInterruptEvent_Enable(DPRx_DECODER_ID dprx_decoder_id, WORD usMSAChgInterruptEvent);
void mhal_DPRx_SetVideoMuteInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_DecoderIsrDetectEnable(DPRx_DECODER_ID dprx_decoder_id, Bool bEnable);
void mhal_DPRx_SetVHeightUnstableInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);

void ________HPD_CONTROL________(void);
void mhal_DPRx_HPDControl(DPRx_AUX_ID dprx_aux_id, BOOL bSetHPD);
BOOL mhal_DPRx_IsHwHPDControlEnable(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_SetHwHPDControlEnable(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
BOOL mhal_DPRx_GetShortHPDIsrFlag(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_InternalOverWriteHPD(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_HwShortHPDTrigger(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_CheckHPDBusy(DPRx_AUX_ID dprx_aux_id);

void ________AUX________(void);
void mhal_DPRx_AuxClockEnable(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_SetGPIOforAUX(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_SetAuxClockGating(BOOL bEnable);
BYTE mhal_DPRx_GetAuxPMClock(void);
void mhal_DPRx_SetAuxPMClock(BOOL bIsXtalClk);
BOOL mhal_DPRx_GetAuxValid(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_SetAuxDelayReply(DPRx_AUX_ID dprx_aux_id, WORD usDelayTime);
WORD mhal_DPRx_GetAuxDelayReply(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_SetAuxIsel(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_SetOffLine(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
BOOL mhal_DPRx_IsAuxAtOffLine(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_CheckAuxIdle(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_CheckAuxPhyIdle(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_PMAux_Reset(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_CableDisconectResetDPCD(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id);

void ________DPCD________(void);
void mhal_DPRx_MCUWritePMDPCD(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucDATA);
BYTE mhal_DPRx_MCUReadPMDPCD(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress);
void mhal_DPRx_MCUWriteNonPMDPCD(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucDATA);
BYTE mhal_DPRx_MCUReadNonPMDPCD(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress);
BYTE mhal_DPRx_GetDPCDValueByRIU(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress);
void mhal_DPRx_SetDPCDValueByRIU(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ubValue);
void mhal_DPRx_OverWriteDPCD202_203(DPRx_ID dprx_id, BOOL bEnable, WORD ucValue);
BYTE mhal_DPRx_GetFakeTrainingDPCD20x(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress);

void ________PROG_DPCD________(void);
void mhal_DPRx_PutAuxDDCData(DPRx_AUX_ID dprx_aux_id, BYTE ucDDCData);
BYTE mhal_DPRx_GetAuxDDCData(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_ProgramDPCDEnable(DPRx_AUX_ID dprx_aux_id, DPRx_PROGRAM_DPCD_TYPE dp_pDPCD_id, BOOL bEnable);
void mhal_DPRx_SetProgramDPCD(BOOL bEnable, DPRx_AUX_ID dprx_aux_id, DPRx_PROGRAM_DPCD_TYPE dp_pDPCD_id, DWORD pDPCD_Addr, BOOL bAddrGroup);
void mhal_DPRx_SetProgramDPCDInterrupt(DPRx_AUX_ID dprx_aux_id, DPRx_PROGRAM_DPCD_TYPE dp_pDPCD_id, BOOL bEnable);
void mhal_DPRx_SetXDATAProgramDPCD(BOOL bEnable, DPRx_AUX_ID dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_TYPE dp_pDPCD_id, DWORD pDPCD_Addr, DPRx_Command_TYPE CommandType, BOOL bAddrGroup);
void mhal_DPRx_SetXDATAProgramDPCDInterrupt(DPRx_AUX_ID dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_TYPE dp_pDPCD_id, BOOL bEnable);
BOOL mhal_DPRx_ProgramDPCD0Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
BOOL mhal_DPRx_ProgramDPCD1Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue, BYTE *ucAUXCMD, BYTE *ucAUXLEN);
BOOL mhal_DPRx_ProgramDPCD2Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
BOOL mhal_DPRx_ProgramDPCD3Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
BOOL mhal_DPRx_ProgramDPCD4Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
BOOL mhal_DPRx_ProgramDPCD5Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue, BYTE *ucDataLength);
BOOL mhal_DPRx_ProgramDPCD6Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
BOOL mhal_DPRx_ProgramDPCD7Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
void mhal_DPRx_ProgramDPCD8Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
void mhal_DPRx_ProgramDPCD9Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
void mhal_DPRx_ProgramDPCD10Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
void mhal_DPRx_ProgramDPCD11Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue);
BOOL mhal_DPRx_XDATAProgramDPCD0Proc(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_XDATAProgramDPCD1Proc(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD0Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD1Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD2Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD3Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD4Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD5Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD6Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD7Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD8Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD9Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD10Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetProgramDPCD11Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetXDATAProgramDPCD0Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_GetXDATAProgramDPCD1Flag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_SetProgrammableDPCDEnable(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_EnableMCCSXDATAProgrammableDPCD(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_CheckMCCSXDATAProgrammableDPCD(DPRx_AUX_ID dprx_aux_id);
DPRx_MCCS_WAKEUP mhal_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(DPRx_AUX_ID dprx_aux_id);
#if (DPRx_MCCS_SUPPORT == 0x1)
BOOL mhal_DPRx_GetMCCSReceiveFlag(DPRx_AUX_ID dprx_aux_id, BYTE ucInputPort);
void mhal_DPRx_MCCSReceiveProc(DPRx_AUX_ID dprx_aux_id, BYTE ucInputPort);
BOOL mhal_DPRx_SetMCCSReplyEnable(DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
#endif

void ________TRAINING________(void);
void mhal_DPRx_HWFastTrainingEnable(DPRx_ID dprx_id, BOOL bEnable);
BOOL mhal_DPRx_HWFastTrainingPeriod(DPRx_ID dprx_id);
void mhal_DPRx_OfflinePortLinkTrainingSetting(DPRx_AUX_ID dprx_aux_id, BYTE ucSwing, BYTE ucPreEmphasis);
void mhal_DPRx_SetFastTrainingTime(DPRx_ID dprx_id, BYTE ubTime);
BOOL mhal_DPRx_CheckHWFastTrainingLock(DPRx_ID dprx_id);
BOOL mhal_DPRx_CheckHWNormalTrainingDone(DPRx_ID dprx_id);
BOOL mhal_DPRx_TrainingBusy_Check(DPRx_ID dprx_id);

void ________HDCP________(void);
void mhal_DPRx_combo_LoadHDCPKey(BYTE *pHDCPKey, BYTE *pBKSV);
void mhal_DPRx_HDCP2SetRxCaps(DPRx_ID dprx_id, BOOL bEnable);
#if(DPRX_HDCP2_ENABLE == 0x1)
void mhal_DPRx_HDCP2InitialSetting(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableHDCP2Interrupt(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableHDCP2EventMask(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_EnableHDCP2ReadDownInterrupt(DPRx_ID dprx_id, BOOL bEnable);
BOOL mhal_DPRx_GetHDCP2IrqFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2AKEInitFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2NoStoredKmFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2StoredKmFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2LCInitFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2SKEFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2HprimeReadDoneFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2RepAuthSendAckFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetHDCP2RepAuthStreamManageFlag(DPRx_ID dprx_id);
BOOL mhal_DPRx_HDCP2GetRxData(DPRx_ID dprx_id, BYTE ucDataInfo, BYTE *pHDCPData);
void mhal_DPRx_HDCP22ReadRxinfo(DPRx_ID dprx_id, BYTE *pRxInfo_Hbyte, BYTE *pRxInfo_Lbyte);
BOOL mhal_DPRx_HDCP22GetRepeaterCapbility(DPRx_ID dprx_id);
void mhal_DPRx_HDCP22RepeaterEnable(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_HDCP2SetRxStatus(DPRx_ID dprx_id, BYTE ucMaskIndex, BOOL bEnable);
BOOL mhal_DPRx_HDCP2SetTxData(DPRx_ID dprx_id, BYTE ucDataInfo, BYTE *pHDCPData);
//void mhal_DPRx_ForceHDCP22IntegrityFail(DPRx_ID dprx_id, BOOL bForceEnable);
#endif
#if (DPRX_HDCP14_Repeater_ENABLE == 0x1)
void mhal_DPRx_HDCP14WriteV(DPRx_AUX_ID dprx_aux_id, BYTE *VData);
void mhal_DPRx_HDCP14WriteBinfo(DPRx_AUX_ID dprx_aux_id, BYTE Binfo_Depth, BYTE Binfo_Devs);
void mhal_DPRx_HDCP14SetRDY(DPRx_ID dprx_id, BOOL Enable);
void mhal_DPRx_HDCP14SetReAuthBit(DPRx_ID dprx_id, BOOL Enable);
void mhal_DPRx_HDCP14RepeaterEnable(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BOOL bEnable);
void mhal_DPRx_HDCP14ReadR0(DPRx_ID dprx_id, BYTE *R0_Data);
void mhal_DPRx_HDCP14WriteKSVFIFO(DPRx_ID dprx_id, WORD u16length, BYTE *BKSVData);
#endif
void mhal_DPRx_EnableAKSVInterrupt(DPRx_ID dprx_id, BOOL bEnable);
BOOL mhal_DPRx_GetAKSVReceivedFlag(DPRx_ID dprx_id);
DP_HDCP_STATE mhal_DPRx_CheckHDCPState(DPRx_ID dprx_id);
DP_HDCP_STATE mhal_DPRx_CheckHDCPEncryption(DPRx_ID dprx_id);
void mhal_DPRx_ForceHDCP13IntegrityFail(DPRx_ID dprx_id, BOOL bForceEnable);
void mhal_DPRx_ForceHDCP22IntegrityFail(DPRx_ID dprx_id, BOOL bForceEnable);
BOOL mhal_DPRx_GetHDCPIntegrity(DPRx_ID dprx_id);
void mhal_DPRx_ClearHDCPIntegrity(DPRx_ID dprx_id);
void mhal_DPRx_SetCPIRQ(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_HDCPLinkFailReset(DPRx_ID dprx_id);
void mhal_DPRx_HDCP14SoftReset(DPRx_ID dprx_id);
void mhal_DPRx_HDCP22SoftReset(DPRx_ID dprx_id);
BOOL mhal_DPRx_GetAKSVFlag(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_ClearAKSVFlag(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_HDCP14CheckAKSVStatus(DPRx_AUX_ID dprx_aux_id);
BOOL mhal_DPRx_HDCP14CheckAnStatus(DPRx_AUX_ID dprx_aux_id);

void ________TYPE_C________(void);
BOOL mhal_DPRx_TypeC_AUX_N_Level(DPRx_ID dprx_id, BYTE ubPinAssign, BYTE ubCCpinInfo);
BOOL mhal_DPRx_TypeC_CheckSquelch(DPRx_ID dprx_id, BYTE ubPinAssign, BYTE ubCCpinInfo);
BOOL mhal_DPRx_SetLaneSwapEnable(DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id, BOOL bEnable, BYTE ubTargetLaneNumber);
BOOL mhal_DPRx_SetLanePNSwapEnable(DPRx_ID dprx_id, BOOL bEnable, BYTE ubLanePNSwapSelect);
BOOL mhal_DPRx_SetAuxPNSwapEnable(DPRx_AUX_ID dprx_aux_id, BOOL bAuxPNSwap);
void mhal_DPRx_MSCHIP_TypeC_PRBS7PNswap(DPRx_ID dprx_id, BYTE ubTypeC_PinAssign);
void mhal_DPRx_MSCHIP_TypeC_PRBS7PNswapforFT(DPRx_ID dprx_id, BYTE ubTypeC_PinAssign);

void ________POWER________(void);
BOOL mhal_DPRx_Polling600h(DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_PLLOven(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable);
void mhal_DPRx_SymbolEQResetEnable(DPRx_ID dprx_id, BOOL bEnable);
void mhal_DPRx_SetPowerDownControl(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DP_ePM_Mode dp_pm_Mode);

#endif //_MHAL_DPRX_H_

