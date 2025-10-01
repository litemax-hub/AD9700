///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mdrv_DPRx.h
/// @author MStar Semiconductor Inc.
/// @brief  DP Rx driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_DPRX_H_
#define _MDRV_DPRX_H_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "dpCommon.h"
#include "mhal_DPRx.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DP_IP_SUPPORT_0PORT             		0
#define DP_IP_SUPPORT_1PORT             		1
#define DP_IP_SUPPORT_2PORT             		2
#define DP_IP_SUPPORT_3PORT             		3
#define DP_IP_SUPPORT_4PORT             		4
#define DP_IP_SUPPORT_5PORT             		5
#define DP_IP_SUPPORT_6PORT             		6
#define DP_IP_SUPPORT_7PORT             		7
#define DP_IP_SUPPORT_TYPE              		DP_IP_SUPPORT_3PORT

#define DPRX_FAST_TRAINING_CHECK_TIME          	150 // Unit: ms
#define DPRX_FAST_TRAINING_LOCK_COUNT          	5
#define DPRX_FAST_TRAINING_SQ_DEBOUNCE_TIME    	120 // Unit: ms

#define DPRX_HBR3_REFERCLOCK				   	0x0CA
#define DPRX_HBR25_REFERCLOCK				   	0x0A8
#define DPRX_HBR2_REFERCLOCK				   	0x10E
#define DPRX_HBR_REFERCLOCK				       	0x10E
#define DPRX_RBR_REFERCLOCK				       	0x0A2

#define DPRX_MAX_VALID_HDE                     	5000 // HBR3: 9000, HBR2: 5000
#define DPRX_MIN_VALID_HDE                     	200  // HBR3: 9000, HBR2: 5000
#define DPRX_MAX_VALID_VDE                     	5000 // HBR3: 9000, HBR2: 5000
#define DPRX_MIN_VALID_VDE                     	200  // HBR3: 9000, HBR2: 5000
#define DPRX_MAX_PIX_ClOCK                     	1000 // HBR3: 3000, HBR2: 1000
#define DPRX_MIN_PIX_ClOCK                     	10

#define DPRX_MEASURE_TIMING_COUNTER            	5
#define DPRX_CHECK_TIMING_COUNTER              	16
#define DPRX_CHECK_TIMING_QUEUE_SIZE           	10
#define DPRX_STABLE_POLLING_QUEUE_SIZE         	4

#define DPRX_AUDIO_STABLE_CNT                  	20
#define DPRX_AUDIO_RECOVER_CNT             	    100 // Unit: ms
#define DPRX_AUDIO_CALAUDIOFREQ_CNT            	5
#define DPRX_AUDIO_CheckAudioAbsent_CNT        	200
#define DPRX_AUDIO_MASK_OFFSET                  1

#define DPRX_PACKET_HEADER_SIZE          	   	0x4
#define DPRX_VSC_PACKET_DATA_SIZE          	   	0x1C

#define DPRX_HDCP14_REP_SUPPORT_MAXDEVs			16
#define DPRX_HDCP14_REP_KSV_SIZE				5

#define DP_HDCP2_RX_QUEUE_SIZE					129
#define DP_HDCP2_TX_QUEUE_SIZE					176 // RX info + seq num v + v' + receiver ID List

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum _DPRx_FAST_TRAINING_RATE_TYPE
{
    DPRx_FAST_TRAINING_RATE_HBR3  = 0,
    DPRx_FAST_TRAINING_RATE_HBR2  = 1,
    DPRx_FAST_TRAINING_RATE_HBR25 = 2,
    DPRx_FAST_TRAINING_RATE_HBR   = 3,
    DPRx_FAST_TRAINING_RATE_RBR   = 4,
    DPRx_FAST_TRAINING_RATE_MASK
}DPRx_FAST_TRAINING_RATE_TYPE;

typedef enum _DPRx_FAST_TRAINING_LANE_TYPE
{
    DPRx_FAST_TRAINING_4LANE = 0,
    DPRx_FAST_TRAINING_2LANE = 1,
    DPRx_FAST_TRAINING_1LANE = 2,
    DPRx_FAST_TRAINING_LANE_MASK
}DPRx_FAST_TRAINING_LANE_TYPE;

typedef enum _DPRx_MSA_Change_Mode
{
    DPRx_HTT = BIT0,
    DPRx_VTT = BIT1,
    DPRx_HSW = BIT2,
    DPRx_HSTART = BIT3,
    DPRx_VSTART = BIT4,
    DPRx_VSW = BIT5,
    DPRx_HDE = BIT6,
    DPRx_VDE = BIT7,
    DPRx_NVID = BIT8,
    DPRx_MISC0 = BIT9,
    DPRx_MISC1 = BIT10,
    DPRx_MASK
}DPRx_MSA_Change_Mode;

enum DP_INPUT_SELECT_TYPE
{
    DP_INPUT_SELECT_PORT0 = 0,
	#if(DP_IP_SUPPORT_TYPE == DP_IP_SUPPORT_1PORT)
    DP_INPUT_SELECT_PORT_END,
	#endif
    DP_INPUT_SELECT_PORT1 =1,
	#if(DP_IP_SUPPORT_TYPE == DP_IP_SUPPORT_2PORT)
    DP_INPUT_SELECT_PORT_END,
	#endif
    DP_INPUT_SELECT_PORT2 =2,
	#if(DP_IP_SUPPORT_TYPE == DP_IP_SUPPORT_3PORT)
    DP_INPUT_SELECT_PORT_END,
	#endif
    DP_INPUT_SELECT_PORT3 =3,
	#if(DP_IP_SUPPORT_TYPE == DP_IP_SUPPORT_4PORT)
    DP_INPUT_SELECT_PORT_END,
	#endif
    DP_INPUT_SELECT_PORT4,
	#if(DP_IP_SUPPORT_TYPE == DP_IP_SUPPORT_5PORT)
    DP_INPUT_SELECT_PORT_END,
	#endif
    DP_INPUT_SELECT_PORT5,
	#if(DP_IP_SUPPORT_TYPE == DP_IP_SUPPORT_6PORT)
    DP_INPUT_SELECT_PORT_END,
	#endif
    DP_INPUT_SELECT_PORT6,
	#if(DP_IP_SUPPORT_TYPE == DP_IP_SUPPORT_7PORT)
	DP_INPUT_SELECT_PORT_END,
	#endif
    DP_INPUT_SELECT_MASK = DP_INPUT_SELECT_PORT_END,
    DP_INPUT_SELECT_NONE = DP_INPUT_SELECT_MASK
};

typedef struct
{
    DPRx_ID OnlinePort;
	BYTE decoderID_Info; // Bit mask, BIT0 stand for using decoder 0 (for scalar used)

	BOOL bDPcableConnent; // DP Rx cable status
	BOOL bDPHpdHwModeEn;
    BOOL bDPLoseCDRLockIRQ;
    BOOL bDPInterlaneSkewLoseIRQ;
	BOOL bDPInterlaneSkewDoneIRQ;
	BOOL bDPHDCPIRQ;
	BOOL bDPHpdState;
    BOOL bDPAutoTestEn;

	BYTE DPVersion;
	WORD uwDPRxStableTimeout;
	WORD bCableConnectCnt;
	WORD bCableDisConnectCnt;
	WORD uwHDCPCnt;
	WORD uwCDRHPDCnt;
    WORD uwSkewHPDCnt;
    WORD uwSinkStatusCnt;
	BYTE bAudioRecoverCnt;
	BOOL bChkEncryptionState;
	WORD uwChkEncryptionTime;
	BYTE ucInterlaceCount;
	BYTE DPCheckModeCnt;
	WORD uwDPStableCount;
	BYTE ubContDownTimer;
    BYTE ubContDownTimer2;
    BYTE ucState;
	BYTE ucPreState;
	BYTE ubSQStableTimer;
	BOOL bDPTrainingFlag;
	BYTE bDPTrainingP1T;
    BYTE bDPTrainingP234T;
	BOOL bRxDecodeStable; // Input timing stable
	BOOL bSDPSplitEnable;

	BYTE ucHDCP14_KSVList[DPRX_HDCP14_REP_SUPPORT_MAXDEVs*DPRX_HDCP14_REP_KSV_SIZE];
	WORD u16HDCP14_KSVListLength;
    BOOL bHDCPLongHPDTrigger:1;
    WORD ucHDCPLongHPDTimer;
    BOOL bHDCP14TriggerShortHPD:1;
	BOOL bHDCP14R0beRead;
    BOOL bHDCP14VprimebeRead:1;
    BOOL bHDCP22TriggerShortHPD:1;
    BYTE u8HDCP22MsgID;
    BOOL bForceIntegrityFail:1;
    BOOL bOverWriteLockStatus:1;
    BOOL bHdcpStartAuth:1;

	BYTE ubTypeC_CC;
	BYTE ubTypeC_PinAssign;
}DPRx_INFO, *PDPRx_INFO;

typedef struct
{
	BYTE ubComboPhyID;

	BOOL bDPVPLLBIGChange;
    BOOL bDPAUPLLBIGChange;
	BOOL bDPMSAChange; // For system used on 800x600@60Hz switch to 800x600@75Hz, system read/clear this flag
	BYTE ucAudioCTS_UCD400;
	BOOL bAudioEnable;
	BOOL bAudioMute;
	BYTE DPAudioFreq;
    BYTE ucAudioFreqByIVS;

	WORD uwDPHtotal;
    WORD uwDPVtotal;
    WORD uwDPHWidth;
    WORD uwDPVWidth;
	WORD uwDPHPWS;
    WORD uwDPVPWS;
    WORD uwDPHBporch;
    WORD uwDPVBporch;
    WORD uwDPHStart;
    WORD uwDPVStart;
    BOOL bDPHPol;
    BOOL bDPVPol;
	WORD uwDPPixel;
	DWORD ulFrameTime;
	BYTE ucPacketTimer;
	WORD uwInputPacketStatus;
	BOOL bDPInterlace;
    BYTE ucDPColorFormat;
    BYTE ucDPRangeFormat;
	BYTE ucDPColorDepth;
    BYTE ucColorType;
    BYTE ucYuvColorimetry;
    BYTE DPMISC0;
    BYTE DPMISC1;
}DPRx_DecoderINFO, *PDPRx_DecoderINFO;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void ________INIT________(void);
void mdrv_DPRx_Initial(void);
void mdrv_DPRx_InitValue(void);
void mdrv_DPRX_PortConfigSetting(BYTE *ucDPFunctionPtr, BYTE *ucDPTPCFunctionPtr);
void mdrv_DPRx_VersionSetting(BYTE ucInputPort, BYTE ucVersion);
void mdrv_DPRx_SetupInputPort_DisplayPort(BYTE ucInputPort);
void mdrv_DPRx_Not_DisplayPort(BYTE ucInputPort);
void mdrv_DPRx_Switch_Port_Check(void);
BOOL mdrv_DPRx_AUX_N_Level(BYTE ucInputPort);

void ________VIDEO________(void);
void mdrv_DPRx_SetLaneCount(BYTE ucInputPort, BYTE ubLaneCount);
BYTE mdrv_DPRx_GetDPVersion(BYTE ucInputPort);
BYTE mdrv_DPRx_GetDPCDLinkRate(BYTE ucInputPort);
BYTE mdrv_DPRx_GetDPLaneCnt(BYTE ucInputPort);
void mdrv_DPRx_GetHVInformation(BYTE ucInputPort, WORD *usHTotalValue, WORD *usVTotalValue);
void mdrv_DPRx_GetHVDEInformation(BYTE ucInputPort, WORD *usHDE, WORD *usVDE);
WORD mdrv_DPRx_GetPixelClock(BYTE ucInputPort);
WORD mdrv_DPRx_GetPixelClock10K(BYTE ucInputPort);
BOOL mdrv_DPRx_GetMSAChgFlag(BYTE ucInputPort);
void mdrv_DPRx_ClrMSAChgFlag(BYTE ucInputPort);
void mdrv_DPRx_PollingColorimetry(DPRx_ID dprx_id);
BYTE mdrv_DPRx_GetYuvColorimetry(BYTE ucInputPort);
BYTE mdrv_DPRx_GetColorSpace(BYTE ucInputPort);
BYTE mdrv_DPRx_GetColorRange(BYTE ucInputPort);
BYTE mdrv_DPRx_GetColorimetry(BYTE ucInputPort);
BYTE mdrv_DPRx_GetColorDepthInfo(BYTE ucInputPort);
BOOL mdrv_DPRx_GetInterlaceFlag(BYTE ucInputPort);
BOOL mdrv_DPRx_Get_Misc(BYTE ucInputPort, BYTE *pPacketData);
void mdrv_DPRx_EnableDRRFunction(BYTE ucInputPort, BOOL bEnable);
BOOL mdrv_DPRx_GetDRRFlag(BYTE ucInputPort);
BOOL mdrv_DPRx_GetPacketData(BYTE ucInputPort, WORD PacketReceiveType, BYTE *pucPacketData, BYTE ucPacketLength);
BOOL mdrv_DPRx_CheckDPLock(BYTE ucInputPort);
BOOL mdrv_DPRx_CheckSourceLock(BYTE ucInputPort);
BOOL mdrv_DPRx_CheckDPTimingStable(BYTE ucInputPort);
BOOL mdrv_DPRx_CheckIgnoreDPMS(BYTE ucInputPort);
BOOL  mdrv_DPRx_GetDPStable( BYTE ucInputPort);
BOOL mdrv_DPRx_GetVideoStreamCRC(BYTE ucInputPort, BYTE* pCRC_Data);

void ________AUDIO________(void);
WORD mdrv_DPRx_GetAudioFreq(BYTE ucInputPort);
BOOL mdrv_DPRx_CheckAudioStatus(BYTE ucInputPort);
void mdrv_DPRx_ConfigAudioPort(BYTE ucInputPort, BOOL bEnable);
BYTE mdrv_DPRx_AudioChannelCnt(BYTE ucInputPort, BYTE *pPacketData);
BOOL mdrv_DPRx_CheckAudioAbsent(BYTE ucInputPort);
BOOL mdrv_DPRx_GetAudioGlobeMute(BYTE ucInputPort);
BOOL mdrv_DPRx_GetNoAudioFlag(BYTE ucInputPort);
DP_AUDIO_FORMAT mdrv_DPRx_IsAudioFmtPCM(BYTE ucInputPort);

void ________AUX________(void);
void mdrv_DPRx_SetOffLine(BYTE ucInputPort, BOOL bEnable);
BOOL mdrv_DPRx_CheckAuxPhyIdle(BYTE ucInputPort);
BOOL mdrv_DPRx_CheckAuxIdle(BYTE ucInputPort);
void mdrv_DPRx_SetAuxPMClock(BOOL bIsXtalClk);
void mdrv_DPRx_ClearFreeSyncFlag(BYTE ucInputPort);
void mdrv_DPRx_SetVESA_AdaptiveSyncSDPEanble(BYTE ucInputPort, BOOL bEnable);
void mdrv_DPRx_SetSDPSplitEnable(BYTE ucInputPort, BOOL bEnable);
void mdrv_DPRx_Load_EDID(BYTE ucInputPort, BYTE *EdidData);
BOOL mdrv_DPRx_SetMCCSReplyEnable(BYTE ucInputPort, BOOL bEnable);

void ________TRAINING________(void);
void mdrv_DPRx_SetFastTrainingTime(BYTE ucInputPort, BYTE ubTime);
BYTE mdrv_DPRx_MaxLinkRate_Set(BYTE ucInputPort, BYTE ucMaxLinkRate);

void ________HPD_CONTROL________(void);
BOOL mdrv_DPRx_HPDControl(BYTE ucInputPort, BOOL bSetHPD);

void ________HANDLER________(void);
void mdrv_DPRx_Handler(void);
void mdrv_DPRx_RX_IRQ_Handler(void);
void mdrv_DPRx_RX2_IRQ_Handler(void);
void mdrv_DPRx_IsrAuxHandler(void);
void mdrv_DPRx_TimerHandler(void);

void ________HDCP________(void);
#if (DPRX_HDCP14_Repeater_ENABLE == 0x1)
void mdrv_DPRx_HDCP14MapDPCDWrite(BYTE ucInputPort, BYTE ucAddress, BYTE ucLength, BYTE *pucData);
void mdrv_DPRx_HDCP14WriteKSVFIFO(BYTE ucInputPort,  WORD u16length, BYTE *BKSVData);
void mdrv_DPRx_HDCP14ConfigRepeaterPair(BYTE ucInputPort, BYTE ucOutputPort, BOOL bEnable);
#endif

void mdrv_DPRx_ClearAKSVFlag(BYTE ucInputPort);
DP_HDCP_STATE mdrv_DPRx_CheckHDCPState(BYTE ucInputPort);

#if (DPRX_HDCP2_ENABLE == 0x1)
void mdrv_DPRx_SetHDCP2CallBackFunction(DPRx_HDCP2_CB_FUNC pHDCP2RxFunc);
void mdrv_DPRx_SetHDCP2ConfigRepeaterCallBackFunction(DPRX_HDCP2_Config_Repeater_CB_FUNC pHDCP2RxConfig_RepeaterFunc);
void mdrv_DPRx_HDCP2TxEventProc(BYTE ucInputPort, BYTE ucMessageID, BYTE *pMessageData);
void mdrv_DPRx_HDCP2SetCertRx(BYTE *pHDCP2CertRx);
void mdrv_DPRx_HDCP2SetRxCaps(BYTE ucInputPort, BOOL bEnable);
void mdrv_DPRx_HDCP22RepeaterGetRxinfo(BYTE ucInputPort, BYTE *pRxInfo_Hbyte, BYTE *pRxInfo_Lbyte);
#if (DPRx_HDCP2_ISR_MODE == 0x1)
void mdrv_DPRx_HDCP2FetchMsg(BYTE ucInputPort);
#endif
#endif

void ________TYPE_C________(void);
BOOL mdrv_DPRx_SetLaneSwapEnable(BYTE ucInputPort, BOOL bEnable, BYTE ubTargetLaneNumber);
BOOL mdrv_DPRx_SetLanePNSwapEnable(BYTE ucInputPort, BOOL bEnable, BOOL bAuxPNSwap, BOOL b4LanePNSwap, BYTE ubLanePNSwapSelect);
BOOL mdrv_DPRx_SetTypeCInfo(BYTE ucInputPort, BYTE ubCCpinInfo, BYTE ubPinAssingInfo);
BOOL mdrv_DPRx_SetEQ_Current(BYTE ucInputPort,BOOL eq_current);
BOOL mdrv_DPRx_GetTypeCHPDEvent(BYTE ucInputPort, BYTE *ucHPDValue);
void mdrv_DPRx_ClrTypeCHPDEvent(BYTE ucInputPort);
BOOL mdrv_DPRx_GetTypeCShortHPDEvent(BYTE ucInputPort);
void mdrv_DPRx_ClrTypeCShortHPDEvent(BYTE ucInputPort);
void mdrv_DPRx_TypeC_CableDisconect(BYTE ucInputPort);

void ________POWER________(void);
void mdrv_DPRx_SetPowerDownControl(BYTE ucInputPort, DP_ePM_Mode dp_pm_Mode);
void mdrv_DPRx_SetPhyPowerDownControl(BYTE ucInputPort, DP_ePM_Mode dp_pm_Mode);
BOOL mdrv_DPRx_IsSystemGoRealSleep(BYTE ucInputPort);

void ________MSCHIP________(void);
BOOL mdrv_DPRx_MSCHIP_CableDetect(BYTE ucInputPort);
void mdrv_DPRx_MSCHIP_EnableDPDetect(BYTE ucPMMode, BOOL bEnable);
void mdrv_DPRx_MSCHIP_DP_PMHandler(void);
DPRx_MCCS_WAKEUP mdrv_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(BYTE ucInputPort);
BOOL mdrv_DPRx_PortInfo_Get(BYTE ucInputPort, ST_COMBO_RX_INFO_UNION *pRxInfoUnion, EN_COMBO_RX_INFO_SELECT RxInfoSelect);
BOOL mdrv_DPRx_PortInfo_Set(BYTE ucInputPort, ST_COMBO_RX_INFO_UNION *pRxInfoUnion, EN_COMBO_RX_INFO_SELECT RxInfoSelect);


#endif //_MDRV_DPRX_H_

