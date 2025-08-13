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
#ifndef IPCOMMON_H
#define IPCOMMON_H
#include "Global.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define ENABLE_HDMITX_OUTPUT                            				1

#define DPTX_HDCP13_REP_SUPPORT_MAXDEVs                		3

#define DPTX_TO_DPRX_VEC_TOTAL_BYTES 			        	1
#define DPTX_TO_DPRX_VEC0_HDCP13_KSV_Valid 		        	BIT0
#define DPTX_TO_DPRX_VEC0_HDCP13_Set_READY 		        	BIT1
#define DPTX_TO_DPRX_VEC0_HDCP13_Set_IRQHPD 	        	BIT2
#define DPTX_TO_DPRX_VEC_TOTAL_BITES 			        	3

#define DPRX_TO_DPTX_VEC_TOTAL_BYTES 			        	1
#define DPRX_TO_DPTX_VEC0_Timing_Valid 			        	BIT0
#define DPRX_TO_DPTX_VEC_TOTAL_BITES 			        		1
#define DPRX_TO_DPTX_REQUEST_MST_DPCD_RW				BIT0
#define DPRX_TO_DPTX_REQUEST_MST_I2C_RW					BIT1
#define DPRX_TO_DPTX_REQUEST_MST_TRIG_ACT				BIT2

#define DPTX_TO_DPRX_REQUEST_MST_SBM_READY				BIT0

#define DPRX_TO_DPTX_REQUEST_MAX_INDEX               			0x1
#define DPTX_TO_DPRX_REQUEST_MAX_INDEX               			0x1

#define DPRX_TO_DPTX_MAX_I2C_W_TRANS		0x2


enum COMBO_RX_TO_HDMITX_EVENT_TYPE
{
    COMBO_RX_TO_HDMITX_EVENT_NONE 				= 0,
    COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO 	= 0,
    COMBO_RX_TO_HDMITX_EVENT_GET_COLOR_INFO,
    COMBO_RX_TO_HDMITX_EVENT_GET_AUDIO_INFO,
    COMBO_RX_TO_HDMITX_EVENT_AUTO_UPDATE 		= COMBO_RX_TO_HDMITX_EVENT_GET_AUDIO_INFO,
    COMBO_RX_TO_HDMITX_EVENT_MAX,
};

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    COMBO_HDMITX_VIDEO_MODE_NONE 				= 0,
    COMBO_HDMITX_VIDEO_MODE_INTERLACE,
    COMBO_HDMITX_VIDEO_MODE_PROGRESSIVE,
    COMBO_HDMITX_VIDEO_MODE_MAX,
} COMBO_HDMITX_VIDEO_MODE;

typedef enum
{
    AUDIO_2_CH  						= 2, // 2 channels
    AUDIO_8_CH  						= 8, // 8 channels
} EN_AUDIO_CHANNEL_COUNT;

typedef enum
{
    AUDIO_FORMAT_PCM   				= 0,
    AUDIO_FORMAT_DSD   				= 1,
    AUDIO_FORMAT_HBR   				= 2,
    AUDIO_FORMAT_DST   				= 3,
    AUDIO_FORMAT_NA    				= 4,
} EN_AUDIO_SOURCE_FORMAT;

typedef enum
{
    COMMON_TMDS_AUD_FREQ_32K		= 32,
    COMMON_TMDS_AUD_FREQ_44K		= 44,
    COMMON_TMDS_AUD_FREQ_48K		= 48,
    COMMON_TMDS_AUD_FREQ_88K		= 88,
    COMMON_TMDS_AUD_FREQ_96K		= 96,
    COMMON_TMDS_AUD_FREQ_176K	= 176,
    COMMON_TMDS_AUD_FREQ_192K	= 192,
    COMMON_TMDS_AUD_FREQ_384K	= 384,
    COMMON_TMDS_AUD_FREQ_ERROR	= 0,
} EN_TMDS_AUD_FREQ_INDEX;

typedef struct
{
    EN_TMDS_AUD_FREQ_INDEX		freq;  // Sample rate
    EN_AUDIO_CHANNEL_COUNT		ch;    // Channel counts
    EN_AUDIO_SOURCE_FORMAT		sfmt;  // Source Coding format
    EN_TMDS_AUDIO_FORMAT		fmt;   // Audio format
}COMMON_HDMITX_AUDIO_PARAMETER;

typedef struct
{
    BYTE                        ubTimingValid;      // TimingValid
    COMBO_HDMITX_VIDEO_MODE     ip_mode;            // interlace or progressive
    DWORD                       hfp;                // H front porch
    DWORD                       hsync;              // H sync width
    DWORD                       hsp;                // H sync polarity
    DWORD                       hbp;                // H back porch
    DWORD                       hde;                // H de width
    DWORD                       htotal;             // H Total
    DWORD                       vfp;                // V front porch
    DWORD                       vsync;              // V sync width
    DWORD                       vsp;                // V sync polarity
    DWORD                       vbp;                // V back porch
    DWORD                       vde;                // V de width
    DWORD                       vtotal;             // V Total
    DWORD                       prate;              // Pixel rate
} COMBO_HDMITX_TIMING_PARAMETER;

typedef struct
{
    BYTE                            ubRxStable;
    BYTE                            ubInputPortIndex;
    COMBO_HDMITX_TIMING_PARAMETER   stTimingInform;
}COMBO_HDMITX_HANDLER_INFO;

//------------------------------------------
//  DP Rx/Tx Data Handshake Structure
//------------------------------------------
typedef struct
{
	//"_R" stands for Rx could R/W,  For Tx, it's just read only
	//"_RT" stands for Rx/Tx could R/W
	//"_T" stands for Tx could R/W,  For Rx, it's just read only

	BOOL  TimingChange_RT;

	WORD uwDPHtotal_R;
	WORD uwDPVtotal_R;
	WORD uwDPHWidth_R;
	WORD uwDPVWidth_R;
	WORD uwDPHPWS_R;
	WORD uwDPVPWS_R;
	WORD uwDPHBporch_R;
	WORD uwDPVBporch_R;
	WORD uwDPHStart_R;
	WORD uwDPVStart_R;
	WORD uwDPPixel_R;
	DWORD udwDPPixelMhz_R;
	BYTE DPMISC0_R;
	BYTE DPMISC1_R;
	BYTE ucState_R;
	BOOL bDecodeStable_R;
	BOOL bDecodeTimingChg_R;
	BYTE uwLinkRate_R;
	BOOL  bInterlace_R;
	BYTE  ubAudio_Channel_R;
	BOOL bDRREnable_R;
    BYTE  bDPPacket_R[32];
    BYTE  bDPHeader_R[4];
    BYTE  bDPPacketSeq_R;
       WORD udwDPpacketType_RT;
	BOOL bDPpacket100msUpdate_RT;

	DWORD ulDPRX_TO_DPTX_REQUEST_R[DPRX_TO_DPTX_REQUEST_MAX_INDEX];
	DWORD ulDPRX_TO_DPTX_REQUEST_ACK_R[DPTX_TO_DPRX_REQUEST_MAX_INDEX];


	BYTE  ubTxID_R;
	//MST Remote DPCD R/W,  Using Request method !
	BYTE  ubAuxCmd_R;
	BYTE  ubAuxLength_R;
	DWORD ulAuxAddress_R;
	BYTE*  pubRx2TxAuxData_RT;

	//MST Remote DPCD R/W,  Using Request method !
	BYTE  ubI2CWriteDeviceID_R[DPRX_TO_DPTX_MAX_I2C_W_TRANS];
	BYTE  ubI2CNumOfByteToWrite_R[DPRX_TO_DPTX_MAX_I2C_W_TRANS];
	BYTE*  pubI2CDataToWrite_R[DPRX_TO_DPTX_MAX_I2C_W_TRANS];
	//-----------
	BYTE  ubI2CDataSize_R;
	BYTE  ulI2CDeviceID_R;
	BYTE  ubI2CCmd_R;
	BYTE  ubNumOfI2cWriteTrans_R;
	BYTE*  pubRx2TxI2CData_RT;

	//MST SBM forward,  Using Request method !
	BYTE  ubMST_DOWNREQ_TxPortNum_R;
	BYTE  ubMST_DOWNREQ_Length_R;
	BYTE*  pMST_DOWNREQ_Rx2TxSBM_Data_R;

	BYTE  ubMST_UPREP_TxPortNum_R;
	BYTE  ubMST_UPREP_Length_R;
	BYTE*  pMST_UPREP_Rx2TxSBM_Data_R;

	//MST to SST, SST to MST change
	BOOL bDPMST_SST2MSTChangeFlag_RT:1;
	BOOL bDPMST_MST2SSTChangeFlag_RT:1;

	BOOL bReceiveAKSV_RT;

	BYTE  ubDPCD_1C0_R[0x3];  //For Tx trig ACT used

	BYTE ucHDCP14RepTxIDPair_R;
	BYTE ucHDCP22RepTxIDPair_R;
	BYTE bHDCPEncryptionState_R;

	BYTE *pDP_SystemMode_R;
	BYTE *pDPVersion_R;
	BOOL  bDPVersionChg_RT;
	BYTE   ucDPCD600h_R;
	BOOL  ucDPCD600hChg_RT;
	BYTE   ucOnlineID_R;
	BYTE* pucState_R;

}DPRX_TO_DPTX_INFO, *PDPRX_TO_DPTX_INFO;

typedef struct
{
	//"_R" stands for Rx could R/W,  For Tx, it's just read only
	//"_RT" stands for Rx/Tx could R/W
	//"_T" stands for Tx could R/W,  For Rx, it's just read only

	BYTE ucDPRx_ID_T;

	//Pointer link to existed Tx info
	BYTE *pLinkRate_T;
	BYTE *pLinkLaneCount_T;
	BYTE *pDWN_STRM_PORT_TYPE_T;
	BYTE *pDPCD_REV_T;
	BYTE *pSinkCountNum_T;
	Bool *pDPMstCAP_T;
	Bool *pDWN_STRM_PORT_PRESENT_T;
	Bool *pHDCPReapter_T;
	Bool *pHDCPCap_T;
	Bool *pCablePlugIn_T;
	Bool *pDPMstBranch_T;
	BYTE *pDPCD_0x68000_T;
	BYTE *pDPCD_0xE_T;

	BYTE ubTx2RxMstEvent_T;
	BYTE *pMSTUpReqBuffer_T;
	BYTE *pMSTDownRepBuffer_T;

#ifdef  DPTX_TO_DPRX_VEC0_HDCP13_KSV_Valid
	// -------------------------------------------------
	BYTE ucHDCP13_Binfo_T[2]; //  fill to DPRX DPCD 6802A/2B
	// -------------------------------------------------
	Bool bHDCP13_Binfo_MAX_DEVS_T; // // DPCD 6802A[7]
	Bool bHDCP13_Binfo_MAX_CASCADE_T; // DPCD 6802B[3]
	BYTE ucHDCP13_Binfo_DEVICE_COUNT_T; // fill to DPRX DPCD 6802A[6:0] Binfo max = 127
	BYTE ucHDCP13_Binfo_DEPTH_T; //  fill to DPRX DPCD 6802B[2:0] Binfo max = 7
	// -------------------------------------------------
	WORD uwHDCP13_KSVidx_T; // Dev*5 => ex 5:10:15
	BYTE ucHDCP13_KSV_T[DPTX_HDCP13_REP_SUPPORT_MAXDEVs*5]; // DPTX_HDCP13_REP_SUPPORT_MAXDEVs
	BYTE ucHDCP13_V_T[20];
#endif

	//MST SBM forward,  Using Request method !
	BYTE  ubMST_DOWNREQ_Rx2TxSBM_TriggerFlag_RT;  //When flag as 0, Rx could set 1,  When flag as 1, Tx could set 0
	BYTE  ubMST_DOWNREQ_Length_R;
	BYTE*  pMST_DOWNREQ_Rx2TxSBM_Data_R;

	BYTE  ubMST_UPREP_Rx2TxSBM_TriggerFlag_RT;  //When flag as 0, Rx could set 1,  When flag as 1, Tx could set 0
	BYTE  ubMST_UPREP_Length_R;
	BYTE*  pMST_UPREP_Rx2TxSBM_Data_R;

       BOOL bDPMST_NotifyCountDownFlag_RT:1;
       BOOL bDPMST_DownStreamAuthPass_RT:1;

       BOOL bDPMST_TxMSTAllocatePayload_RT:1;

       Bool  bDownStreamInfoUpdateFlag_RT:1;
	Bool  bCablePlugStatusChangeFlag_RT:1;
	Bool  bHDCP14DownStream_ReAuthHPD_Request_T:1;
	Bool  bDPTxDisconnetOccur_RT:1;

	DWORD ulDPTX_TO_DPRX_REQUEST_T[DPTX_TO_DPRX_REQUEST_MAX_INDEX];
	DWORD ulDPTX_TO_DPRX_REQUEST_ACK_T[DPRX_TO_DPTX_REQUEST_MAX_INDEX];
    BYTE bDPMST_DownStreamSinkCountValue_T;
    Bool bDPMST_DownStreamSinkCountUpdateReady_RT;
    BOOL b_DPTXVPLLRequest_RT;
    BOOL b_DPTXVPLLClear_RT;
    BYTE ubDPTXVPLLID_T;
    WORD uwDPTXVPLLxM_T;
    WORD uwDPTXVPLLxN_T;

}DPTX_TO_DPRX_INFO, *PDPTX_TO_DPRX_INFO;

//------------------------------------------
//  DP Rx/ HDMI Tx Data Handshake Structure
//------------------------------------------

typedef struct
{
	//"_R" stands for Rx could R/W,  For Tx, it's just read only
	//"_RT" stands for Rx/Tx could R/W
	//"_T" stands for Tx could R/W,  For Rx, it's just read only

    BYTE  ubPortNum_R;

}DPRX_TO_HDMITX_INFO, *PDPRX_TO_HDMITX_INFO;

typedef struct
{
	//"_R" stands for Rx could R/W,  For Tx, it's just read only
	//"_RT" stands for Rx/Tx could R/W
	//"_T" stands for Tx could R/W,  For Rx, it's just read only

    BYTE  ubPortNum_T;

}HDMITX_TO_DPRX_INFO, *PHDMITX_TO_DPRX_INFO;

//------------------------------------------
//  HDMI Rx/ HDMI Tx Data Handshake Structure
//------------------------------------------
typedef struct
{
	//"_R" stands for Rx could R/W,  For Tx, it's just read only
	//"_RT" stands for Rx/Tx could R/W
	//"_T" stands for Tx could R/W,  For Rx, it's just read only

    BYTE  ubPortNum_R;

}HDMIRX_TO_HDMITX_INFO, *PHDMIRX_TO_HDMITX_INFO;

typedef struct
{
	//"_R" stands for Rx could R/W,  For Tx, it's just read only
	//"_RT" stands for Rx/Tx could R/W
	//"_T" stands for Tx could R/W,  For Rx, it's just read only

    BYTE  ubPortNum_T;

}HDMITX_TO_HDMIRX_INFO, *PHDMITX_TO_HDMIRX_INFO;

typedef struct
{
    Bool bInitialFlag_R;
    Bool bDEStableFlag_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    BYTE ucColorFormat_R; // COMBO_RX_TO_HDMITX_EVENT_GET_COLOR_INFO
    BYTE ucColorDepth_R; // COMBO_RX_TO_HDMITX_EVENT_GET_COLOR_INFO
    BYTE ucPixedRepetition_R; // COMBO_RX_TO_HDMITX_EVENT_GET_COLOR_INFO
    WORD usHDEValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usHTTValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVDEValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVTTValue_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usHSYNCValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVSYNCValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usHFrontValue_R;   // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVFrontValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usHBackValue_R;    // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usVBackValue_R;     // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    BYTE ucIP_mode_R;         // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    WORD usAudioFrequency_R; // COMBO_RX_TO_HDMITX_EVENT_GET_AUDIO_INFO

    DWORD ulTMDSClockCount_R; // COMBO_RX_TO_HDMITX_EVENT_GET_TIMING_INFO
    DWORD ulValidEventGroup_R;
    DWORD ulRequestEventGroup_T;
    DWORD ulAckEventGroup_T;
}ST_RX_TO_HDMITX_INFO, *pST_RX_TO_HDMITX_INFO;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------

#endif
