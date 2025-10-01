////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MDRV_HDMIRX_H
#define MDRV_HDMIRX_H

//#include "IpCommon.h"
#include "mhal_hdmiRx.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#ifndef BIT
#define BIT(_bit_)                          (1 << (_bit_))
#endif

#define GET_HDMIRX_FLAG(a, b)               (Bool)((a &b) ?TRUE :FALSE)
#define SET_HDMIRX_FLAG(a, b)               (a |= b)
#define CLR_HDMIRX_FLAG(a, b)               (a &= ~b)

#define HDMI_POLLING_INTERVAL               10U
#define HDMI_PACKET_RECEIVE_COUNT           5U   // 10ms

#define HDMI_HDCP2_RECEIVE_QUEUE_SIZE       129U
#define HDMI_HDCP2_SEND_QUEUE_SIZE          33U
#define HDMI_HDCP2_AKE_CERTRX_SIZE          522U

#define DFT_HPDCLK_DIFF_PERIOD 100ul // 100 as suggested
// HDCP 2.2
#define TMDS_HDCP2_FUNCTION_SUPPORT     COMBO_HDCP2_FUNCTION_SUPPORT
// HPD ISR
#define HDMI_HPD_ISR_MODE   ENABLE
typedef BOOL (*TMDS_HDCP2_CALL_BACK_FUNC)(BYTE, BYTE*);
typedef void (*COMBO_HDCP2_CALL_BACK_FUNC)(BYTE, BYTE, WORD, BYTE*); // 3rd argument should be WORD for hdcp2.2 tx (AKE_Send_No_Stored_Km : 129 bytes)
typedef BOOL (*COMBO_HDCPTX_IO_CBFUNC)(BYTE, BYTE, BYTE, BYTE, WORD, BYTE*); //portIdx, Offset, cmdID, operation, length, data pointer; return TRUE: access successfully
typedef void (*COMBO_WRITEX74_FUNC)(BYTE, BYTE, BYTE, BYTE*); //portIdx, Addr, Length, Data; return TRUE: access successfully
typedef BYTE (*COMBO_READX74_FUNC)(BYTE, BYTE); //portIdx, Addr; return Data
typedef void (*COMBO_KSVFIFO_FUNC)(BYTE, WORD, BYTE*); //portIdx, KSV Length, KSV Data; return TRUE: access successfully
typedef BOOL (*COMBO_HDCPTX_COMPARE_RI_FUNC)(BYTE, WORD*, WORD*); //portIdx, Tx_Ri, Rx_Ri; return TRUE: access successfully
typedef BOOL (*COMBO_HDCPTX_GET_DPR0_FUNC)(BYTE); //portIdx; return TRUE: access successfully

#define COMBO_EDID_SIZE_PORT0           2
#define COMBO_EDID_SIZE_PORT1           2
#define COMBO_EDID_SIZE_PORT2           2
#define COMBO_EDID_SIZE_PORT3           2

#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
#define AVI_PKT_LOSS_BOUND      2   //unit:vsync
#define VS_PKT_LOSS_BOUND       2
#define HDR_PKT_LOSS_BOUND      2
#define GCP_PKT_LOSS_BOUND      4
#else
#define AVI_PKT_LOSS_BOUND      500   //unit:1ms
#define VS_PKT_LOSS_BOUND       500
#define HDR_PKT_LOSS_BOUND      500
#define GCP_PKT_LOSS_BOUND      500
#endif

enum HDMI_HDCP2_MESSAGE_ID_TYPE
{
    HDMI_HDCP2_MSG_ID_NONE = 0,
    HDMI_HDCP2_MSG_ID_AKE_INIT = 2,
    HDMI_HDCP2_MSG_ID_AKE_SEND_CERT = 3,
    HDMI_HDCP2_MSG_ID_AKE_NO_STORED_KM = 4,
    HDMI_HDCP2_MSG_ID_AKE_STORED_KM = 5,
    HDMI_HDCP2_MSG_ID_AKE_SEND_H_PRIME = 7,
    HDMI_HDCP2_MSG_ID_AKE_SEND_PAIRING_INFO = 8,
    HDMI_HDCP2_MSG_ID_LC_INIT = 9,
    HDMI_HDCP2_MSG_ID_LC_SEND_L_PRIME = 10,
    HDMI_HDCP2_MSG_ID_SKE_SEND_EKS = 11,
};

enum HDMI_GC_INFO_CD_TYPE
{
    HDMI_GC_INFO_CD_NOT_INDICATED = 0,  // 0
    HDMI_GC_INFO_CD_RESERVED,           // 1/2/3
    HDMI_GC_INFO_CD_24BITS = 4,         // 4
    HDMI_GC_INFO_CD_30BITS,             // 5
    HDMI_GC_INFO_CD_36BITS,             // 6
    HDMI_GC_INFO_CD_48BITS,             // 7
};

enum HDMI_COLOR_DEPTH_TYPE
{
    HDMI_COLOR_DEPTH_6_BIT = 0,
    HDMI_COLOR_DEPTH_8_BIT = 1,
    HDMI_COLOR_DEPTH_10_BIT = 2,
    HDMI_COLOR_DEPTH_12_BIT = 3,
    HDMI_COLOR_DEPTH_16_BIT = 4,
    HDMI_COLOR_DEPTH_UNKNOWN = 5,
};

enum HDMI_PIXEL_REPETITION_TYPE
{
    HDMI_PIXEL_REPETITION_1X = 0,
    HDMI_PIXEL_REPETITION_2X,
    HDMI_PIXEL_REPETITION_3X,
    HDMI_PIXEL_REPETITION_4X,
    HDMI_PIXEL_REPETITION_5X,
    HDMI_PIXEL_REPETITION_6X,
    HDMI_PIXEL_REPETITION_7X,
    HDMI_PIXEL_REPETITION_8X,
    HDMI_PIXEL_REPETITION_9X,
    HDMI_PIXEL_REPETITION_10X,
    HDMI_PIXEL_REPETITION_RESERVED,
};

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef void        (*HDMI_HDCP2_CALL_BACK_FUNC)                (BYTE, BYTE*);
typedef void        (*IOCTL_HDMIRX_INITIAL)                      (WORD, WORD, WORD);
typedef void        (*IOCTL_HDMIRX_HANDLER)                      (void);
typedef void        (*IOCTL_HDMIRX_TIMER_HANDLER)                (void);
typedef void        (*IOCTL_HDMIRX_CLOCK_RTERM_CONTROL)         (EN_HDMI_INPUT_PORT, Bool, Bool);
typedef void        (*IOCTL_HDMIRX_HPD_CONTROL)                  (EN_HDMI_INPUT_PORT, Bool, Bool, Bool);
typedef Bool        (*IOCTL_HDMIRX_GET_HPD_STATUS)              (EN_HDMI_INPUT_PORT, Bool);
typedef Bool        (*IOCTL_HDMIRX_GET_CABLE_DETECT_FLAG)       (EN_HDMI_INPUT_PORT, Bool);
typedef BYTE        (*IOCTL_HDMIRX_GET_SOURCE_VERSION)          (EN_HDMI_INPUT_PORT, Bool);
typedef void        (*IOCTL_HDMIRX_HDCP2_HANDER)                (void);
typedef BYTE        (*IOCTL_HDMIRX_GET_COLOR_DEPTH)             (EN_HDMI_INPUT_PORT, Bool, Bool);
typedef BYTE        (*IOCTL_HDMIRX_GET_COLOR_FORMAT)             (EN_HDMI_INPUT_PORT, Bool);
typedef BYTE        (*IOCTL_HDMIRX_GET_PIXEL_REPETITION)        (EN_HDMI_INPUT_PORT, Bool);
typedef void        (*IOCTL_HDMIRX_FIXED_EQ_SETTING)            (EN_HDMI_INPUT_PORT, BYTE, Bool, Bool);
typedef void        (*IOCTL_HDMIRX_ARC_PAD_ENABLE)              (Bool);

typedef struct
{
    Bool b10msTimerFlag;
    BYTE uc10msCounter;
    BYTE ucPacketReceiveCount;
    BYTE ucHPDControlIndex;
    BYTE ucHDCPWriteDoneIndex;
    BYTE ucHDCPReadDoneIndex;
    BYTE ucHDCPSendMessageIndex;
    BYTE *pucHDCPCertRx;
    BYTE usInputPortEnableIndex;
    BYTE usInputPortTypeDualDVIIndex;
    BYTE usInputPortTypeHDMIIndex;
    ST_COMBO_COLOR_FORMAT          input_color;
    ST_HDMI_RX_POLLING_INFO stPollingInfo;
    ST_HDMI_RX_AUDIO_INFO stAudioInfo;
    ST_HDMI_RX_HDR_INFO stHDRInfo;//[HDMI_HDR_BLOCK_END];
    ST_HDMI_RX_TIMING_INFO stRxTimingInfo;
    HDMI_HDCP2_CALL_BACK_FUNC pmdrv_tmds_HDCP2ReceiveEventProc;
}ST_HDMI_RX_SYSTEM_PARAMETER;

typedef struct _HDMI_RX_INSTANT_PRIVATE
{
    DWORD ulDeviceID;
    IOCTL_HDMIRX_INITIAL                    fpHDMIRxInitial;
    IOCTL_HDMIRX_HANDLER                    fpHDMIRxHandler;
    IOCTL_HDMIRX_TIMER_HANDLER              fpHDMIRxTimerHandler;
    IOCTL_HDMIRX_CLOCK_RTERM_CONTROL        fpHDMIRxClockRtermControl;
    IOCTL_HDMIRX_HPD_CONTROL                fpHDMIRxHPDControl;
    IOCTL_HDMIRX_GET_HPD_STATUS             fpHDMIRxGetHPDStatus;
    IOCTL_HDMIRX_GET_CABLE_DETECT_FLAG      fpHDMIRxGetCableDetectFlag;
    IOCTL_HDMIRX_GET_SOURCE_VERSION         fpHDMIRxGetSourceVersion;
    IOCTL_HDMIRX_HDCP2_HANDER               fpHDMIRxHDCP2Hander;
    IOCTL_HDMIRX_GET_COLOR_DEPTH            fpHDMIRxGetColorDepth;
    IOCTL_HDMIRX_GET_COLOR_FORMAT           fpHDMIRxGetColorFormat;
    IOCTL_HDMIRX_GET_PIXEL_REPETITION       fpHDMIRxGetPixedRepetition;
    IOCTL_HDMIRX_FIXED_EQ_SETTING           fpHDMIRxFixedEQSetting;
    IOCTL_HDMIRX_ARC_PAD_ENABLE             fpHDMIRxARCPadEnable;
}ST_HDMI_RX_INSTANT_PRIVATE;

typedef struct
{
    WORD    u16HPDCnt[2];   //1:u16HPDCnt[0]: use to set HPD low; u16HPDCnt[1]:HPD low duration counter
    WORD    u16HPDClkLow;   //2:CLK r-term low thd
    WORD    u16HPDClkHigh;  //3:CLK r-term high thd
    BOOL    bIsHPDProcessDone;
} ST_HPD_CONTROL;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
Bool mdrv_hdmiRx_Initial(BYTE ucMode);
void mdrv_hdmiRx_HPD_Resume(void);
void mdrv_hdmiRx_Handler(void);
void mdrv_hdmiRx_TimerInterrupt(void);
void mdrv_hdmi_SetupInputPort(BYTE ucPortSelect);
//void mdrv_hdmiRx_TimerHandler(void);
ST_HDMI_RX_COLOR_FORMAT mdrv_hdmiRx_GetColorimetry(BYTE ucPortSelect);
EN_TMDS_AUDIO_FORMAT mdrv_hdmiRx_IsAudioFmtPCM(BYTE ucPortIndex);
DWORD mdrv_tmds_GetPixelClockHz(BYTE enInputPort, EN_HDMI_PIX_CLK_TYPE enType);
BOOL mdrv_hdmiRx_RxInfo_Get(BYTE enInputPort, ST_COMBO_RX_INFO_UNION *pRxInfoUnion, EN_COMBO_RX_INFO_SELECT enInfo);
WORD mdrv_hdmiRx_GetVideoContentInfo(EN_HDMI_VIDEO_CONTENT_INFO enVideoContentInfo, BYTE ucPortSelect);
BOOL mdrv_hdmiRx_GetPacketContent(MS_HDMI_PACKET_STATE_t ucPacketType, BYTE ucPacketLength, BYTE *pPacketData, BYTE ucPortSelect);
#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
BYTE mdrv_tmds_HDMIGetErrorStatus_Partial(BYTE enInputPort);
void mdrv_tmds_HDMI_GetPktReceived_Partial(BYTE ucInputPort);
#endif
void mdrv_hdmiRx_HPDControl(BYTE enInputPortSelect, Bool bPullHighFlag);
void mdrv_hdmiRx_HPDControl_By_Duration(BYTE enInputPortSelect, WORD uwDurationMs);
void mdrv_hdmiRx_ClockRtermControl(EN_HDMI_INPUT_PORT enInputPortSelect, Bool bPullHighFlag);
void mdrv_hdmiRx_DataRtermControl(EN_HDMI_INPUT_PORT enInputPortSelect, Bool bPullHighFlag);
void mdrv_hdmiRx_DDCControl(BYTE enInputPortType, Bool bEnable);
void mdrv_HDMIRx_LoadEDID(BYTE ucPortSelect, BYTE *EdidData);
#if ENABLE_HPD_REPLACE_MODE
void mdrv_hdmiRx_SCDC_Clr(BYTE enInputPortType);
void mdrv_hdmiRx_SCDC_config(BYTE enInputPortSelect, BYTE bDDC);
void mdrv_hdmiRx_MAC_HDCP_Enable(Bool bEnable);
#endif
void mdrv_hdmiRx_SetSCDCValue(BYTE enInputPort,BYTE u8Offset,BYTE u8Size,BYTE *u8SetValue);
void mdrv_tmds_SCDC_ISRHandler(void);
void mdrv_tmds_PHY_ISRHandler(void);
void mdrv_hdmiRx_IRQ_Enable(EN_KDRV_HDMIRX_INT e_int, BYTE enInputPortSelect, MS_U16 bit_msk, Bool bEnableIRQ);
void mdrv_tmds_TimingChgISR(void);
void mdrv_tmds_SetTimingChgCallBackFunction(TimingChg_CALL_BACK_FUNC pFunc);
void mdrv_hdmiRx_PMEnableDVIDetect(HDMI_PM_MODE_TYPE PMMode);
void mdrv_hdmiRx_PMSwitchDVIDetect (BYTE ucPortSelect);
void mdrv_hdmiRx_PMPowerDownLane(EN_HDMI_INPUT_PORT enInputPortSelect, Bool Enable);
void mdrv_hdmiRx_SetPowerDown(BYTE enInputPortType, Bool bPowerDown);
void mdrv_hdmiRx_Software_Reset(BYTE enInputPortSelect, WORD u16Reset);
EN_HDMI_INPUT_PORT mdrv_hdmiRx_InputPortMapping(BYTE enInputPortSelect);
Bool mdrv_combo_HDCP2RxEventProc(BYTE ucPortSelect, BYTE *pMessage);
void mdrv_tmds_HDCP2Handler(void);
void mdrv_combo_HDCP2Initial(void);
void mdrv_tmds_SetHDCP2CallBackFunction(TMDS_HDCP2_CALL_BACK_FUNC pHDCP2RxFunc, BYTE *pHDCP2CertRx);
Bool mdrv_combo_HDCP2KeyDecodeDone(void);
void mdrv_tmds_HDCP2TxEventProc(BYTE enInputPort, BYTE ucMessageID, WORD wDataSize, BYTE *pHDCPTxData);
void mdrv_combo_SetHDCP2CallBackFunction(BYTE ucPortSelect, COMBO_HDCP2_CALL_BACK_FUNC pHDCP2TxFunc);
void mdrv_combo_HDCP2ResetRomCodeflag(void);
Bool mdrv_combo_HDCP2RomCodeDone(void);
void mdrv_combo_HDCP2Handler(void);
Bool mdrv_tmds_HDMIGetErrorStatus(BYTE enInputPortType);
BYTE mdrv_hdmiRx_CheckHDCPState(void);
void mdrv_hdmiRx_DTOPDEC_IRQ_Mask(void);
void mdrv_hdmiRx_DTOPDEC_IRQ_Unmask(void);
TMDS_AUDIO_FREQUENCY_INDEX mdrv_hdmiRx_GetAudioFrequency(BYTE enInputPortType);
MS_BOOL mdrv_hdmiRx_Cable_5V_Detect(BYTE ucInputPort);
Bool mdrv_hdmiRx_Set_RB_PN_Swap(BYTE ucPortIndex, HDMI_SWAP_TYPE enHDMI_SWAP_TYPE, Bool bSwapEnable);
void mdrv_HDMIRx_SetFreeSyncEDID(BYTE ucPortSelect,BOOL bEnableFreeSync);
#endif // MDRV_HDMIRX_H

