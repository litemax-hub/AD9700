///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mapi_DPRx.c
/// @author MStar Semiconductor Inc.
/// @brief  DP Rx driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MAPI_DPRX_C_
#define _MAPI_DPRX_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "Global.h"
#include "ComboApp.h"
#include "dpCommon.h"
#include "mapi_DPRx.h"
#include "mdrv_DPRx.h"

#if (ENABLE_DP_INPUT == 0x1)
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define DP_API_DEBUG_MESSAGE		0

#if (DP_DEBUG_MESSAGE && DP_API_DEBUG_MESSAGE)
#define DP_API_DPUTSTR(str)			printMsg(str)
#define DP_API_DPRINTF(str, x)		printData(str, x)
#else
#define DP_API_DPUTSTR(str)
#define DP_API_DPRINTF(str, x)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void ________INIT________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_Initial()
//  [Description]
//					mapi_DPRx_Initial
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_Initial(void)
{
	mdrv_DPRx_Initial();

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_VersionSetting()
//  [Description]
//					ucVersion = 0x11/0x12/0x14 stands for DP1.1/1.2/1.4
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_VersionSetting(BYTE ucInputPort, BYTE ucVersion)
{
	mdrv_DPRx_VersionSetting(ucInputPort, ucVersion);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetupInputPort_DisplayPort()
//  [Description]
//					mapi_DPRx_SetupInputPort_DisplayPort
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetupInputPort_DisplayPort(BYTE ucInputPort)
{
    mdrv_DPRx_SetupInputPort_DisplayPort(ucInputPort);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_Not_DisplayPort()
//  [Description]
//					mapi_DPRx_Not_DisplayPort
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_Not_DisplayPort(BYTE ucInputPort)
{
    mdrv_DPRx_Not_DisplayPort(ucInputPort);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_Switch_Port_Check()
//  [Description]
//					mapi_DPRx_Switch_Port_Check
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_Switch_Port_Check(void)
{
    mdrv_DPRx_Switch_Port_Check();

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_AUX_N_Level()
//  [Description]
//					mapi_DPRx_AUX_N_Level
//  [Arguments]:
//
//  [Return]:   	1: Cable connect
//					0: Cable disconnect
//
//**************************************************************************
BOOL mapi_DPRx_AUX_N_Level(BYTE ucInputPort)
{
	return mdrv_DPRx_AUX_N_Level(ucInputPort);
}

void ________VIDEO________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetLaneCount()
//  [Description]
//					mapi_DPRx_SetLaneCount
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetLaneCount(BYTE ucInputPort, BYTE ubLaneCount)
{
	mdrv_DPRx_SetLaneCount(ucInputPort, ubLaneCount);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetDPVersion()
//  [Description]
//					mapi_DPRx_GetDPVersion
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetDPVersion(BYTE ucInputPort)
{
    return mdrv_DPRx_GetDPVersion(ucInputPort);
}
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetDPCDLinkRate()
//  [Description]
//					mapi_DPRx_GetDPCDLinkRate
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetDPCDLinkRate(BYTE ucInputPort)
{
    return mdrv_DPRx_GetDPCDLinkRate(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetDPLaneCnt()
//  [Description]
//					mapi_DPRx_GetDPLaneCnt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetDPLaneCnt(BYTE ucInputPort)
{
    return mdrv_DPRx_GetDPLaneCnt(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetHVInformation()
//  [Description]
//					mapi_DPRx_GetHVInformation
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_GetHVInformation(BYTE ucInputPort, WORD *usHTotalValue, WORD *usVTotalValue)
{
    mdrv_DPRx_GetHVInformation(ucInputPort, usHTotalValue, usVTotalValue);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetHVDEInformation()
//  [Description]
//					mapi_DPRx_GetHVDEInformation
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_GetHVDEInformation(BYTE ucInputPort, WORD *usHDE, WORD *usVDE)
{
    mdrv_DPRx_GetHVDEInformation(ucInputPort, usHDE, usVDE);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetPixelClock()
//  [Description]
//					mapi_DPRx_GetPixelClock
//  [Arguments]:
//
//  [Return]:
//                  Pixel clock = xxx MHz
//**************************************************************************
WORD mapi_DPRx_GetPixelClock(BYTE ucInputPort)
{
    return mdrv_DPRx_GetPixelClock(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetMSAChgFlag()
//  [Description]
//                  mapi_DPRx_GetMSAChgFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetMSAChgFlag(BYTE ucInputPort)
{
    return mdrv_DPRx_GetMSAChgFlag(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_ClrMSAChgFlag()
//  [Description]
//                  mapi_DPRx_ClrMSAChgFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_ClrMSAChgFlag(BYTE ucInputPort)
{
	mdrv_DPRx_ClrMSAChgFlag(ucInputPort);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetColorFormate()
//  [Description]
//					mapi_DPRx_GetColorFormate
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
ST_DP_COLOR_FORMAT mapi_DPRx_GetColorFormate(BYTE ucInputPort)
{
    ST_DP_COLOR_FORMAT stColorInfo = {COMBO_COLOR_FORMAT_DEFAULT, COMBO_COLOR_RANGE_DEFAULT, COMBO_COLORIMETRY_NONE, COMBO_YUV_COLORIMETRY_ITU601};

    stColorInfo.ucColorType = mdrv_DPRx_GetColorSpace(ucInputPort);
    stColorInfo.ucColorRange = mdrv_DPRx_GetColorRange(ucInputPort);
    stColorInfo.ucColorimetry = mdrv_DPRx_GetColorimetry(ucInputPort);
    stColorInfo.ucYuvColorimetry =  mdrv_DPRx_GetYuvColorimetry(ucInputPort);

    return stColorInfo;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetColorDepthInfo()
//  [Description]
//					mapi_DPRx_GetColorDepthInfo
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetColorDepthInfo(BYTE ucInputPort)
{
    BYTE ubColorDepth = mdrv_DPRx_GetColorDepthInfo(ucInputPort);

    switch(ubColorDepth)
    {
        case DP_COLOR_DEPTH_6BIT:
            return COMBO_COLOR_DEPTH_6BIT;
        case DP_COLOR_DEPTH_8BIT:
            return COMBO_COLOR_DEPTH_8BIT;
        case DP_COLOR_DEPTH_10BIT:
            return COMBO_COLOR_DEPTH_10BIT;
        case DP_COLOR_DEPTH_12BIT:
            return COMBO_COLOR_DEPTH_12BIT;
        case DP_COLOR_DEPTH_16BIT:
            return COMBO_COLOR_DEPTH_16BIT;
        default:
            return COMBO_COLOR_DEPTH_NONE;
    }

    return COMBO_COLOR_DEPTH_NONE;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetYuvColorimetry()
//  [Description]
//					mapi_DPRx_GetYuvColorimetry
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetYuvColorimetry(BYTE ucInputPort)
{
    return mdrv_DPRx_GetYuvColorimetry(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetColorSpace()
//  [Description]
//					mapi_DPRx_GetColorSpace
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetColorSpace(BYTE ucInputPort)
{
    return mdrv_DPRx_GetColorSpace(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetColorRange()
//  [Description]
//					mapi_DPRx_GetColorRange
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetColorRange(BYTE ucInputPort)
{
    return mdrv_DPRx_GetColorRange(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetColorimetry()
//  [Description]
//					mapi_DPRx_GetColorimetry
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_GetColorimetry(BYTE ucInputPort)
{
    return mdrv_DPRx_GetColorimetry(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetInterlaceFlag()
//  [Description]
//					mapi_DPRx_GetInterlaceFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetInterlaceFlag(BYTE ucInputPort)
{
    return mdrv_DPRx_GetInterlaceFlag(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_Get_Misc()
//  [Description]
//					mapi_DPRx_Get_Misc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_Get_Misc(BYTE ucInputPort, BYTE *pPacketData)
{
    return mdrv_DPRx_Get_Misc(ucInputPort, pPacketData);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_EnableDRRFunction() // cyc: need mailbox
//  [Description]
//					mapi_DPRx_EnableDRRFunction
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_EnableDRRFunction(BYTE ucInputPort, BOOL bEnable)
{
    mdrv_DPRx_EnableDRRFunction(ucInputPort, bEnable);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetDRRFlag()
//  [Description]
//					mapi_DPRx_GetDRRFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetDRRFlag(BYTE ucInputPort)
{
	return mdrv_DPRx_GetDRRFlag(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetPacketData()
//  [Description]
//					mapi_DPRx_GetPacketData
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetPacketData(BYTE ucInputPort, WORD PacketReceiveType, BYTE *pucPacketData, BYTE ucPacketLength)
{
    return mdrv_DPRx_GetPacketData(ucInputPort, PacketReceiveType, pucPacketData, ucPacketLength);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckDPLock()
//  [Description]
//					mapi_DPRx_CheckDPLock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_CheckDPLock(BYTE ucInputPort)
{
    return mdrv_DPRx_CheckDPLock(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckSourceLock()
//  [Description]
//					mapi_DPRx_CheckSourceLock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_CheckSourceLock(BYTE ucInputPort)
{
    return mdrv_DPRx_CheckSourceLock(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckDPTimingStable()
//  [Description]
//					mapi_DPRx_CheckDPTimingStable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_CheckDPTimingStable(BYTE ucInputPort)
{
    return mdrv_DPRx_CheckDPTimingStable(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckIgnoreDPMS()
//  [Description]
//					mapi_DPRx_CheckIgnoreDPMS
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_CheckIgnoreDPMS(BYTE ucInputPort)
{
    return mdrv_DPRx_CheckIgnoreDPMS(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetDPStable()
//  [Description]
//					mapi_DPRx_GetDPStable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetDPStable(BYTE ucInputPort)
{
	return mdrv_DPRx_GetDPStable(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetVideoStreamCRC() , for SST stream only !!!
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetVideoStreamCRC(BYTE ucInputPort, BYTE* pCRC_Data)
{

    return mdrv_DPRx_GetVideoStreamCRC(ucInputPort, pCRC_Data);
}

void ________AUDIO________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetAudioFreq()
//  [Description]
//					mapi_DPRx_GetAudioFreq
//  [Arguments]:
//
//  [Return]:
//  				001: 8 bit
//  				010: 10 bit
//  				011: 12 bit
//  				100: 16 bit
//
//**************************************************************************
WORD mapi_DPRx_GetAudioFreq(BYTE ucInputPort)
{
    return mdrv_DPRx_GetAudioFreq(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckAudioStatus()
//  [Description]
//					mapi_DPRx_CheckAudioStatus
//  [Arguments]:
//
//  [Return]:   	1: Audio Exit
//                  0: No Audio
//
//**************************************************************************
BOOL mapi_DPRx_CheckAudioStatus(BYTE ucInputPort)
{
    return mdrv_DPRx_CheckAudioStatus(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_AudioChannelCnt()
//  [Description]
//                  mapi_DPRx_AudioChannelCnt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_AudioChannelCnt(BYTE ucInputPort, BYTE *pPacketData)
{
    return mdrv_DPRx_AudioChannelCnt(ucInputPort, pPacketData);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckAudioAbsent()
//  [Description]
//					mapi_DPRx_CheckAudioAbsent
//  [Arguments]:
//
// 	[Return]:  		1 : No audio
//                 	0 : Has audio
//
//**************************************************************************
BOOL mapi_DPRx_CheckAudioAbsent(BYTE ucInputPort)
{
	return mdrv_DPRx_CheckAudioAbsent(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_ConfigAudioPort()
//  [Description]
//					mapi_DPRx_ConfigAudioPort
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_ConfigAudioPort(BYTE ucInputPort, BOOL bEnable)
{
    mdrv_DPRx_ConfigAudioPort(ucInputPort, bEnable);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetAudioGlobeMute()
//  [Description]
//                  mapi_DPRx_GetAudioGlobeMute
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetAudioGlobeMute(BYTE ucInputPort)
{
    return mdrv_DPRx_GetAudioGlobeMute(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetNoAudioFlag()
//  [Description]
//                  mapi_DPRx_GetNoAudioFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetNoAudioFlag(BYTE ucInputPort)
{
    return mdrv_DPRx_GetNoAudioFlag(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_IsAudioFmtPCM()
//  [Description]
//					mapi_DPRx_IsAudioFmtPCM
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DPRx_AUDIO_FORMAT mapi_DPRx_IsAudioFmtPCM(BYTE ucInputPort)
{
	DP_AUDIO_FORMAT dpAudioFormat = mdrv_DPRx_IsAudioFmtPCM(ucInputPort);

	switch(dpAudioFormat)
	{
		case DP_AUDIO_FMT_NON_PCM:
			return DPRx_AUDIO_FMT_NON_PCM;
			break;

		case DP_AUDIO_FMT_PCM:
			return DPRx_AUDIO_FMT_PCM;
			break;

		case DP_AUDIO_FMT_UNKNOWN:
			return DPRx_AUDIO_FMT_UNKNOWN;
			break;

		default:
			break;
	}

    return DPRx_AUDIO_FMT_UNKNOWN;
}

void ________AUX________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetOffLine()
//  [Description]
//					mapi_DPRx_SetOffLine
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetOffLine(BYTE ucInputPort, BOOL bEnable)
{
    mdrv_DPRx_SetOffLine(ucInputPort, bEnable);

    return;
}

//**************************************************************************
//  [Function Name]:
//					mapi_DPRx_CheckAuxPhyIdle()
//  [Description]
//					mapi_DPRx_CheckAuxPhyIdle
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_CheckAuxPhyIdle(BYTE ucInputPort)
{
    return mdrv_DPRx_CheckAuxPhyIdle(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//					mapi_DPRx_CheckAuxIdle()
//  [Description]
//					mapi_DPRx_CheckAuxIdle
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_CheckAuxIdle(BYTE ucInputPort)
{
    return mdrv_DPRx_CheckAuxIdle(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//					mapi_DPRx_SetAuxPMClock()
//  [Description]
//					mapi_DPRx_SetAuxPMClock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetAuxPMClock(BOOL bIsXtalClk)
{
    mdrv_DPRx_SetAuxPMClock(bIsXtalClk);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_ClearFreeSyncFlag()
//  [Description]
//					mapi_DPRx_ClearFreeSyncFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_ClearFreeSyncFlag(BYTE ucInputPort)
{
    mdrv_DPRx_ClearFreeSyncFlag(ucInputPort);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetVESA_AdaptiveSyncSDPEanble()
//  [Description]
//					mapi_DPRx_SetVESA_AdaptiveSyncSDPEanble
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetVESA_AdaptiveSyncSDPEanble(BYTE ucInputPort, BOOL bEnable)
{
    mdrv_DPRx_SetVESA_AdaptiveSyncSDPEanble(ucInputPort, bEnable);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_Load_EDID()
//  [Description]
//					mapi_DPRx_Load_EDID
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_Load_EDID(BYTE ucInputPort, BYTE *EdidData)
{
	mdrv_DPRx_Load_EDID(ucInputPort, EdidData);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetMCCSReplyEnable()
//  [Description]
//                  mapi_DPRx_SetMCCSReplyEnable
//  [Arguments]:
//
//  [Return]:
//                  TRUE: Reply MCCS normally
//
//**************************************************************************
BOOL mapi_DPRx_SetMCCSReplyEnable(BYTE ucInputPort, BOOL bEnable)
{
    return mdrv_DPRx_SetMCCSReplyEnable(ucInputPort, bEnable);
}

void ________TRAINING________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetFastTrainingTime()
//  [Description]
//					mapi_DPRx_SetFastTrainingTime
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetFastTrainingTime(BYTE ucInputPort, BYTE ubTime)
{
	mdrv_DPRx_SetFastTrainingTime(ucInputPort, ubTime);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_MaxLinkRate_Set()
//  [Description]
//					mapi_DPRx_MaxLinkRate_Set
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mapi_DPRx_MaxLinkRate_Set(BYTE ucInputPort, BYTE ucMaxLinkRate)
{
    return mdrv_DPRx_MaxLinkRate_Set(ucInputPort, ucMaxLinkRate);
}

void ________HPD_CONTROL________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_HPDControl()
//  [Description]
//					mapi_DPRx_HPDControl
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_HPDControl(BYTE ucInputPort, BOOL bSetHPD)
{
	return mdrv_DPRx_HPDControl(ucInputPort, bSetHPD);
}

void ________HANDLER________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_Handler()
//  [Description]
//					mapi_DPRx_Handler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_Handler(void)
{
    mdrv_DPRx_Handler();

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_RX_IRQ_Handler()
//  [Description]
//					mapi_DPRx_RX_IRQ_Handler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_RX_IRQ_Handler(void)
{
    mdrv_DPRx_RX_IRQ_Handler();

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_RX2_IRQ_Handler()
//  [Description]
//					mapi_DPRx_RX2_IRQ_Handler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_RX2_IRQ_Handler(void)
{
    mdrv_DPRx_RX2_IRQ_Handler();

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_PMIsrHandler()
//  [Description]
//					mapi_DPRx_PMIsrHandler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_PMIsrHandler(void)
{
    mdrv_DPRx_IsrAuxHandler();

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_TimerHandler()
//  [Description]
//					mapi_DPRx_TimerHandler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_TimerHandler(void)
{
    mdrv_DPRx_TimerHandler();

    return;
}

void ________HDCP________(void);
#if (DPRX_HDCP14_Repeater_ENABLE == 0x1)
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_HDCP14MapDPCDWrite()
//  [Description]
//					mapi_DPRx_HDCP14MapDPCDWrite
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_HDCP14MapDPCDWrite(BYTE ucInputPort, BYTE ucAddress, BYTE ucLength, BYTE *pucData)
{
    mdrv_DPRx_HDCP14MapDPCDWrite(ucInputPort, ucAddress, ucLength, pucData);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_HDCP14MapDPCDKSVList()
//  [Description]
//					mapi_DPRx_HDCP14MapDPCDKSVList
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_HDCP14MapDPCDKSVList(BYTE ucInputPort, WORD usKSVLength, BYTE *pucKSVData)
{
    mdrv_DPRx_HDCP14WriteKSVFIFO(ucInputPort, usKSVLength, pucKSVData);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_HDCP14ConfigRepeaterPair()
//  [Description]
//					mapi_DPRx_HDCP14ConfigRepeaterPair
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_HDCP14ConfigRepeaterPair(BYTE ucInputPort, BYTE ucOutputPort, BOOL bEnable)
{
	mdrv_DPRx_HDCP14ConfigRepeaterPair(ucInputPort, ucOutputPort, bEnable);

	return;
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetHDCP2CallBackFunction()
//  [Description]
//					mapi_DPRx_SetHDCP2CallBackFunction
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetHDCP2CallBackFunction(DPRx_HDCP2_CALL_BACK_FUNC pHDCP2RxFunc, BYTE *pHDCP2CertRx)
{
	#if (DPRX_HDCP2_ENABLE == 0x1)
    mdrv_DPRx_SetHDCP2CallBackFunction(pHDCP2RxFunc);

    if(pHDCP2CertRx != NULL)
    {
        mdrv_DPRx_HDCP2SetCertRx(pHDCP2CertRx);
    }
	#else
    pHDCP2RxFunc = NULL;
    pHDCP2CertRx = NULL;
	#endif

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetHDCP2ConfigRepeaterCallBackFunction()
//  [Description]
//					mapi_DPRx_SetHDCP2ConfigRepeaterCallBackFunction
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetHDCP2ConfigRepeaterCallBackFunction(DPRX_HDCP2_CONFIG_REPEATER_CALL_BACK_FUNC pHDCP2RxConfig_RepeaterFunc)
{
	#if(DPRX_HDCP2_ENABLE == 1)
    mdrv_DPRx_SetHDCP2ConfigRepeaterCallBackFunction(pHDCP2RxConfig_RepeaterFunc);
	#else
    pHDCP2RxConfig_RepeaterFunc = NULL;
	#endif

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_HDCP2TxEventProc()
//  [Description]
//					mapi_DPRx_HDCP2TxEventProc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_HDCP2TxEventProc(BYTE ucInputPort, BYTE ucMessageID, WORD wDataSize, BYTE *pMessageData)
{
	#if (DPRX_HDCP2_ENABLE == 0x1)
    if(wDataSize <= DP_HDCP2_TX_QUEUE_SIZE)
    {
        mdrv_DPRx_HDCP2TxEventProc(ucInputPort, ucMessageID, pMessageData);
    }
    else
    {
        printf("** DP HDCP2 Tx queue size too small port %d", ucInputPort); // TBD
    }
	#else
    ucInputPort = 0;
    ucMessageID = 0;
    wDataSize = 0;
    pMessageData = NULL;
	#endif
}

#if (DPRx_HDCP2_ISR_MODE == 0x1) && (DPRX_HDCP2_ENABLE == 0x1)
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_HDCP2FetchMsg()
//  [Description]
//					mapi_DPRx_HDCP2FetchMsg
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_HDCP2FetchMsg(BYTE ucInputPort)
{
    mdrv_DPRx_HDCP2FetchMsg(ucInputPort);

    return;
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_ClearAKSVFlag()
//  [Description]
//					mapi_DPRx_ClearAKSVFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_ClearAKSVFlag(BYTE ucInputPort)
{
    mdrv_DPRx_ClearAKSVFlag(ucInputPort);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckHDCPState()
//  [Description]
//                  Check HDCP is 1.4 or 2.2 or no encryption
//  [Arguments]:
//
//  [Return]:
//                  HDCP1.4 or HDCP2.2 or no encryption
//
//**************************************************************************
DP_HDCP_STATE mapi_DPRx_CheckHDCPState(BYTE ucInputPort)
{
	DP_HDCP_STATE dpRxHDPCSTate = mdrv_DPRx_CheckHDCPState(ucInputPort);

	if(dpRxHDPCSTate == DP_HDCP_NO_ENCRYPTION)
	{
		return DP_HDCP_NO_ENCRYPTION;
	}
	else if(dpRxHDPCSTate == DP_HDCP_1_4)
	{
		return DP_HDCP_1_4;
	}
	else
	{
		return DP_HDCP_2_2;
	}
}

void ________TYPE_C________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetLaneSwapEnable()
//  [Description]
//                  mapi_DPRx_SetLaneSwapEnable
//  [Arguments]:
//					ubTargetLaneNumber: [1:0] for Lane0,
//										[3:2] for Lane1,
//										[5:4] for Lane2,
//										[7:6] for Lane3
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_SetLaneSwapEnable(BYTE ucInputPort, BOOL bEnable, BYTE ubTargetLaneNumber)
{
    return mdrv_DPRx_SetLaneSwapEnable(ucInputPort, bEnable, ubTargetLaneNumber);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetLanePNSwapEnable()
//  [Description]
//					mapi_DPRx_SetLanePNSwapEnable
//  [Arguments]:
//					ubLanePNSwapSelect: BIT0 for Lane0, BIT1 for Lane1, BIT2 for Lane2, BIT3 for Lane3, only work when b4LanePNSwap = TRUE
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_SetLanePNSwapEnable(BYTE ucInputPort, BOOL bEnable, BOOL bAuxPNSwap, BOOL b4LanePNSwap, BYTE ubLanePNSwapSelect)
{
	return mdrv_DPRx_SetLanePNSwapEnable(ucInputPort, bEnable, bAuxPNSwap, b4LanePNSwap, ubLanePNSwapSelect);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetTypeCInfo()
//  [Description]
//					mapi_DPRx_SetTypeCInfo
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_SetTypeCInfo(BYTE ucInputPort, BYTE ubCCpinInfo, BYTE ubPinAssingInfo)
{
	return mdrv_DPRx_SetTypeCInfo(ucInputPort, ubCCpinInfo, ubPinAssingInfo);
}
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetEQ_Current()
//  [Description]
//					mapi_DPRx_SetEQ_Current
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_SetEQ_Current(BYTE ucInputPort,BOOL eq_current)
{
	return mdrv_DPRx_SetEQ_Current(ucInputPort , eq_current);
}
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetTypeCHPDEvent()
//  [Description]
//					mapi_DPRx_GetTypeCHPDEvent
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetTypeCHPDEvent(BYTE ucInputPort, BYTE *ucHPDValue)
{
    return mdrv_DPRx_GetTypeCHPDEvent(ucInputPort, ucHPDValue);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetTypeCHPDEvent()
//  [Description]
//					mapi_DPRx_ClrTypeCHPDEvent
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_ClrTypeCHPDEvent(BYTE ucInputPort)
{
	mdrv_DPRx_ClrTypeCHPDEvent(ucInputPort);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetTypeCShortHPDEvent()
//  [Description]
//					mapi_DPRx_GetTypeCShortHPDEvent
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetTypeCShortHPDEvent(BYTE ucInputPort)
{
    return mdrv_DPRx_GetTypeCShortHPDEvent(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_ClrTypeCShortHPDEvent()
//  [Description]
//					mapi_DPRx_ClrTypeCShortHPDEvent
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_ClrTypeCShortHPDEvent(BYTE ucInputPort)
{
    mdrv_DPRx_ClrTypeCShortHPDEvent(ucInputPort);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_TypeC_CableDisconect()
//  [Description]
//					mapi_DPRx_TypeC_CableDisconect
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_TypeC_CableDisconect(BYTE ucInputPort)
{
    mdrv_DPRx_TypeC_CableDisconect(ucInputPort);

	return;
}

void ________POWER________(void);
//**************************************************************************
//  [Function Name]:
//					mapi_DPRx_IsSystemGoRealSleep()
//  [Description]
//					mapi_DPRx_IsSystemGoRealSleep
//  [Arguments]:
//
//  [Return]:
//					TRUE: Real PM mode, FASE: Fake sleep mode
//**************************************************************************
BOOL mapi_DPRx_IsSystemGoRealSleep(BYTE ucInputPort)
{
	return mdrv_DPRx_IsSystemGoRealSleep(ucInputPort);
}

void ________MSCHIP________(void);
//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetCableDetectPort()
//  [Description]
//					mapi_DPRx_GetCableDetectPort
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mapi_DPRx_GetCableDetectPort(BYTE ucInputPort)
{
    return mdrv_DPRx_MSCHIP_CableDetect(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_MSCHIP_EnableDPDetect()
//  [Description]
//					mapi_DPRx_MSCHIP_EnableDPDetect
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_MSCHIP_EnableDPDetect(BYTE ucPMMode, BOOL bEnable)
{
	mdrv_DPRx_MSCHIP_EnableDPDetect(ucPMMode, bEnable);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_MSCHIP_DP_PMHandler()
//  [Description]
//					mapi_DPRx_MSCHIP_DP_PMHandler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_MSCHIP_DP_PMHandler(void)
{
	mdrv_DPRx_MSCHIP_DP_PMHandler();

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD()
//  [Description]
//					mapi_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DPRx_MCCS_WAKEUP mapi_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(BYTE ucInputPort)
{
	return mdrv_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(ucInputPort);
}
#endif // ENABLE_DP_INPUT

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_GetPixelClock10K()
//  [Description]
//					mapi_DPRx_GetPixelClock10K
//  [Arguments]:
//
//  [Return]:
//                  Pixel clock = xxx MHz
//**************************************************************************
WORD mapi_DPRx_GetPixelClock10K(BYTE ucInputPort)
{
    return mdrv_DPRx_GetPixelClock10K(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetPowerDownControl()
//  [Description]
//					mapi_DPRx_SetPowerDownControl
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetPowerDownControl(BYTE ucInputPort, BYTE ucPMMode)
{
	DP_ePM_Mode dp_pm_Mode = DP_ePM_INVAILD;

	switch(ucPMMode)
	{
		case ePM_POWERON:
			dp_pm_Mode = DP_ePM_POWERON;
			break;

		case ePM_STANDBY:
			dp_pm_Mode = DP_ePM_STANDBY;
			break;

		case ePM_POWEROFF:
		case ePM_POWEROFF_0W:
			dp_pm_Mode = DP_ePM_POWEROFF;
			break;

		default:
			break;
	}

    mdrv_DPRx_SetPowerDownControl(ucInputPort, dp_pm_Mode);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mapi_DPRx_SetPowerDownControl()
//  [Description]
//					mapi_DPRx_SetPowerDownControl
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_DPRx_SetPhyPowerDownControl(BYTE ucInputPort, BYTE ucPMMode)
{
	DP_ePM_Mode dp_pm_Mode = DP_ePM_INVAILD;

	switch(ucPMMode)
	{
		case ePM_POWERON:
			dp_pm_Mode = DP_ePM_POWERON;
			break;

		case ePM_STANDBY:
			dp_pm_Mode = DP_ePM_STANDBY;
			break;

		case ePM_POWEROFF:
		case ePM_POWEROFF_0W:
			dp_pm_Mode = DP_ePM_POWEROFF;
			break;

		default:
			break;
	}

    mdrv_DPRx_SetPhyPowerDownControl(ucInputPort, dp_pm_Mode);

    return;
}
#endif // _MAPI_DPRX_C_

