///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   Mapi_DPRx.h
/// @author MStar Semiconductor Inc.
/// @brief  DP Rx driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPI_DPRX_H_
#define _MAPI_DPRX_H_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
extern BOOL glbSetAuxClktoXtal;
extern BOOL glbSetAuxClktoFRO;

typedef BOOL(*DPRx_HDCP2_CALL_BACK_FUNC)(BYTE, BYTE*); // Need sync dpcommon.h "DPRx_HDCP2_CB_FUNC"
typedef void(*DPRX_HDCP2_CONFIG_REPEATER_CALL_BACK_FUNC)(BYTE, BYTE, BOOL);

typedef struct
{
    DP_COLOR_FORMAT_TYPE      ucColorType;
    DP_COLOR_RANGE_TYPE       ucColorRange;
    DP_COLORIMETRY_TYPE       ucColorimetry;
    DP_YPBPR_COLORIMETRY_TYPE ucYuvColorimetry;
}ST_DP_COLOR_FORMAT;

typedef enum _DPRx_AUDIO_FORMAT
{
    DPRx_AUDIO_FMT_NON_PCM = 0,
    DPRx_AUDIO_FMT_PCM     = 1,
    DPRx_AUDIO_FMT_UNKNOWN = 2
}DPRx_AUDIO_FORMAT;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void ________INIT________(void);
void mapi_DPRx_Initial(void);
void mapi_DPRx_VersionSetting(BYTE ucInputPort, BYTE ucVersion);
void mapi_DPRx_SetupInputPort_DisplayPort(BYTE ucInputPort);
void mapi_DPRx_Switch_Port_Check(void);
void mapi_DPRx_Not_DisplayPort(BYTE ucInputPort);
BOOL mapi_DPRx_AUX_N_Level(BYTE ucInputPort);

void ________VIDEO________(void);
void mapi_DPRx_SetLaneCount(BYTE ucInputPort, BYTE ubLaneCount);
BYTE mapi_DPRx_GetDPVersion(BYTE ucInputPort);
BYTE mapi_DPRx_GetDPCDLinkRate(BYTE ucInputPort);
BYTE mapi_DPRx_GetDPLaneCnt(BYTE ucInputPort);
void mapi_DPRx_GetHVInformation(BYTE ucInputPort, WORD *usHTotalValue, WORD *usVTotalValue);
void mapi_DPRx_GetHVDEInformation(BYTE ucInputPort, WORD *usHDE, WORD *usVDE);
WORD mapi_DPRx_GetPixelClock(BYTE ucInputPort);
WORD mapi_DPRx_GetPixelClock10K(BYTE ucInputPort);
BOOL mapi_DPRx_GetMSAChgFlag(BYTE ucInputPort);
void mapi_DPRx_ClrMSAChgFlag(BYTE ucInputPort);
ST_DP_COLOR_FORMAT mapi_DPRx_GetColorFormate(BYTE ucInputPort);
BYTE mapi_DPRx_GetColorDepthInfo(BYTE ucInputPort);
BYTE mapi_DPRx_GetYuvColorimetry(BYTE ucInputPort);
BYTE mapi_DPRx_GetColorSpace(BYTE ucInputPort);
BYTE mapi_DPRx_GetColorRange(BYTE ucInputPort);
BYTE mapi_DPRx_GetColorimetry(BYTE ucInputPort);
BOOL mapi_DPRx_GetInterlaceFlag(BYTE ucInputPort);
BOOL mapi_DPRx_Get_Misc(BYTE ucInputPort, BYTE *pPacketData);
void mapi_DPRx_EnableDRRFunction(BYTE ucInputPort, BOOL bEnable);
BOOL mapi_DPRx_GetDRRFlag(BYTE ucInputPort);
BOOL mapi_DPRx_GetPacketData(BYTE ucInputPort, WORD PacketReceiveType, BYTE *pucPacketData, BYTE ucPacketLength);
BOOL mapi_DPRx_GetPacketReceived_Partial(BYTE ucInputPort);
BOOL mapi_DPRx_CheckDPLock(BYTE ucInputPort);
BOOL mapi_DPRx_CheckDPTimingStable(BYTE ucInputPort);
BOOL mapi_DPRx_CheckIgnoreDPMS(BYTE ucInputPort);
BOOL mapi_DPRx_GetDPStable(BYTE ucInputPort);
BOOL mapi_DPRx_GetVideoStreamCRC(BYTE ucInputPort, BYTE* pCRC_Data);

void ________AUDIO________(void);
WORD mapi_DPRx_GetAudioFreq(BYTE ucInputPort);
BOOL mapi_DPRx_CheckAudioStatus(BYTE ucInputPort);
BYTE mapi_DPRx_AudioChannelCnt(BYTE ucInputPort, BYTE *pPacketData);
BOOL mapi_DPRx_CheckAudioAbsent(BYTE ucInputPort);
void mapi_DPRx_ConfigAudioPort(BYTE ucInputPort, BOOL bEnable);
BOOL mapi_DPRx_GetAudioGlobeMute(BYTE ucInputPort);
BOOL mapi_DPRx_GetNoAudioFlag(BYTE ucInputPort);

void ________AUX________(void);
void mapi_DPRx_SetOffLine(BYTE ucInputPort, BOOL bEnable);
BOOL mapi_DPRx_CheckAuxPhyIdle(BYTE ucInputPort);
BOOL mapi_DPRx_CheckAuxIdle(BYTE ucInputPort);
void mapi_DPRx_SetAuxPMClock(BOOL bIsXtalClk);
void mapi_DPRx_ClearFreeSyncFlag(BYTE ucInputPort);
void mapi_DPRx_Load_EDID(BYTE ucInputPort, BYTE *EdidData);
BOOL mapi_DPRx_SetMCCSReplyEnable(BYTE ucInputPort, BOOL bEnable);

void ________TRAINING________(void);
void mapi_DPRx_SetFastTrainingTime(BYTE ucInputPort, BYTE ubTime);
BYTE mapi_DPRx_MaxLinkRate_Set(BYTE ucInputPort, BYTE ucMaxLinkRate);

void ________HPD_CONTROL________(void);
BOOL mapi_DPRx_HPDControl(BYTE ucInputPort, BOOL bSetHPD);

void ________HANDLER________(void);
void mapi_DPRx_Handler(void);
void mapi_DPRx_RX_IRQ_Handler(void);
void mapi_DPRx_RX2_IRQ_Handler(void);
void mapi_DPRx_PMIsrHandler(void);
void mapi_DPRx_TimerHandler(void);

void ________HDCP________(void);
#if (DPRX_HDCP14_Repeater_ENABLE == 0x1)
void mapi_DPRx_HDCP14MapDPCDWrite(BYTE ucInputPort, BYTE ucAddress, BYTE ucLength, BYTE *pucData);
void mapi_DPRx_HDCP14MapDPCDKSVList(BYTE ucInputPort, WORD usKSVLength, BYTE *pucKSVData);
void mapi_DPRx_HDCP14ConfigRepeaterPair(BYTE ucInputPort, BYTE ucOutputPort, BOOL bEnable);
#endif

void mapi_DPRx_SetHDCP2CallBackFunction(DPRx_HDCP2_CALL_BACK_FUNC pHDCP2RxFunc, BYTE *pHDCP2CertRx);
void mapi_DPRx_SetHDCP2ConfigRepeaterCallBackFunction(DPRX_HDCP2_CONFIG_REPEATER_CALL_BACK_FUNC pHDCP2RxConfig_RepeaterFunc);
void mapi_DPRx_HDCP2TxEventProc(BYTE ucInputPort, BYTE ucMessageID, WORD wDataSize, BYTE *pMessageData);

#if (DPRx_HDCP2_ISR_MODE == 0x1) && (DPRX_HDCP2_ENABLE == 0x1)
void mapi_DPRx_HDCP2FetchMsg(BYTE ucInputPort);
#endif
void mapi_DPRx_ClearAKSVFlag(BYTE ucInputPort);
BYTE mapi_DPRx_CheckHDCPState(BYTE ucInputPort);

void ________TYPE_C________(void);
BOOL mapi_DPRx_SetPinAssignSettings(BYTE ucInputPort, BOOL bEnable, BYTE ubTargetLaneNumber);
BOOL mapi_DPRx_SetLanePNSwapEnable(BYTE ucInputPort, BOOL bEnable, BOOL bAuxPNSwap, BOOL b4LanePNSwap, BYTE ubLanePNSwapSelect);
BOOL mapi_DPRx_SetTypeCInfo(BYTE ucInputPort, BYTE ubCCpinInfo, BYTE ubPinAssingInfo);
BOOL mapi_DPRx_SetEQ_Current(BYTE ucInputPort,BOOL eq_current);
BOOL mapi_DPRx_GetTypeCHPDEvent(BYTE ucInputPort, BYTE *ucHPDValue);
void mapi_DPRx_ClrTypeCHPDEvent(BYTE ucInputPort);
BOOL mapi_DPRx_GetTypeCShortHPDEvent(BYTE ucInputPort);
void mapi_DPRx_ClrTypeCShortHPDEvent(BYTE ucInputPort);
void mapi_DPRx_TypeC_CableDisconect(BYTE ucInputPort);

void ________POWER________(void);
void mapi_DPRx_SetPowerDownControl(BYTE ucInputPort, BYTE ucPMMode);
void mapi_DPRx_SetPhyPowerDownControl(BYTE ucInputPort, BYTE ucPMMode);
BOOL mapi_DPRx_IsSystemGoRealSleep(BYTE ucInputPort);

void ________MSCHIP________(void);
BOOL mapi_DPRx_GetCableDetectPort(BYTE ucInputPort);
void mapi_DPRx_MSCHIP_EnableDPDetect(BYTE ucPMMode, BOOL bEnable);
void mapi_DPRx_MSCHIP_DP_PMHandler(void);
BYTE mapi_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(BYTE ucInputPort);

#endif //_MAPI_DPRX_H_

