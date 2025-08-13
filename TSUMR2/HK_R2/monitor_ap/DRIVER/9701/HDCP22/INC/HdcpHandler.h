#ifndef HDCP_HANDLER_H
#define HDCP_HANDLER_H

#include "HdcpError.h"
//#include "mhal_hdmiRx.h"
#include "HdcpCommon.h"
//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HDCP2RX_ISR_MODE        1//0//HDMI_HDCP2_IRQ_MODE_ENABLE
#define DEF_HDCP_RECV_BUF_SIZE 544//535 // priority byte + AKE_Send_Cert
#define DEF_HDCP_TRANS_BUF_SIZE 192//178 //priority byte + RepeaterAuth_Send_ReceiverID_List (22 + 5*device cnt) then align 16
#define HDCP14_TX_KEY_IMI_ADDRESS       0x3D60
#define HDCP22_CERTRX_IMI_ADDRESS       0x3700

#define DEF_CUSTOMIZE_KEY_SIZE  16
#define COMBO_HDCP2_ENCODE_KEY_CHECK_SIZE   32

//----------------------------------------------------------------------//
// struct
//----------------------------------------------------------------------//
typedef void (*HDCPRX_IO_FUNC)(BYTE, BYTE, WORD, BYTE*); //PortIdx, MsgID, size, data pointer
typedef void (*HDCPRX_FETCH_MSG_FUNC)(BYTE); //portIdx
typedef BOOL (*HDCPTX_IO_FUNC)(BYTE, BYTE, BYTE, BYTE, WORD, BYTE*); //portIdx, Offset, cmdID, operation, length, data pointer; return TRUE: access successfully
typedef BOOL (*HDCPTX_COMPARE_RI_FUNC)(BYTE, WORD*, WORD*); //portIdx, Tx_Ri, Rx_Ri; return TRUE: access successfully
typedef BOOL (*HDCPTX_GET_DPTXR0_FUNC)(BYTE);////portIdx; return TRUE: access successfully
typedef void (*HDCPRX_WRITEX74_FUNC)(BYTE, BYTE, BYTE, BYTE*); //portIdx, Addr, Length, Write Data; return TRUE: access successfully
typedef BYTE (*HDCPRX_READX74_FUNC)(BYTE, BYTE); //portIdx, Read Addr; return TRUE: access successfully
typedef void (*HDCPREP_KSVFIFO_FUNC)(BYTE, WORD, BYTE*); //portIdx, KSV Length, KSV Data; return TRUE: access successfully
typedef BOOL (*HDCPREP_CHECKENC_FUNC)(BYTE); //portIdx; return TRUE: Encryption status
typedef void (*HDCPREP_RX_HPDCTRL_FUNC)(BYTE, BOOL, BOOL); // portIdx, HPD high_low, auto high_low

typedef struct
{
    BYTE                    enHdcpType;
    BYTE                    enHdcpVerCtrl; //force hdcp2x, 1x or free run
    BYTE                    enHdcpVerInfo; //indicate sink capability
    BYTE                    u8RetryCnt;
    BYTE                    u8MainState;
    BYTE                    u8SubState;
    BYTE                    u8DownStreamDevCnt;
    BOOL                    bEnableHdcp;
    BOOL                    bAsRepeater;
    BYTE                    u8HDCPRxVer;    //Rx auth version
    BOOL                    bRxHDCPStart;   // Rx Receive HDCP start command
    BOOL                    bRxStartEnc;    // Rx Receive HDCP ENC
    //hdcp1.x relative
    BYTE                    u8AKSV[DEF_HDCP1X_KSV_SIZE];
    BYTE                    u8BKSV[DEF_HDCP1X_KSV_SIZE];
    BYTE                    u8BCaps;
    BYTE                    u8BStatus[DEF_HDCP1X_BSTATUS_SIZE];
    BYTE                    u8SinkDeviceCnt;
    BYTE                    u8KSVFIFO[DEF_HDCP1X_MAX_KSV_SIZE*DEF_HDCP1X_KSV_SIZE];
    BYTE                    u8VPrime[DEF_HDCP1X_V_SIZE];
    BYTE                    u8M0[DEF_HDCP1X_M0_SIZE];    //DP used
    BOOL                    bPrepare2SetRdyBit;
    BOOL                    bRdyBitSet;
    BOOL                    bTx2RxInfo;
    BOOL                    bRecvIDSet;
    BOOL                    bIsSendRecvList;
    //hdcp2.x relative
    BOOL                    bSendAKEInit;
    BYTE                    u8RecvID[DEF_HDCP2_RECV_ID_SIZE];
    BOOL                    bRetryAKEInit;
    BOOL                    bRetryStreamManage;
    BOOL                    bGetRecvIDList;
    BOOL                    bStoredKm;
    BOOL                    bSendLCInit;
    BOOL                    bPolling;
    BOOL                    bWaitRxResponse;
    BOOL                    bAuthDone;
    BOOL                    bSinkIsRepeater;
    BOOL                    bRecvMsg;
    BOOL                    bRecvStreamManage;
    BOOL                    bTopologyErr;
    BOOL                    bReadDone;
    BOOL                    bSendPair;
    BOOL                    bRecvAck;
    ULONG                   u32SeqNumVCnt;
    BYTE                    u8RxInfo[DEF_HDCP2_RXINFO_SIZE];
    BYTE                    u8RecvIDList[DEF_HDCP2_MAX_DEVICE_COUNT*DEF_HDCP2_RECV_ID_SIZE];
    ULONG                   u32RxSeqNumV;
    BYTE                    u8StreamIDType[DEF_HDCP2_STREAMID_TYPE_SIZE];
    BOOL                    bRx2TxStreamType;
    BYTE                    u8RecvManageStream[(DEF_HDCP2_SEQ_NUM_M_SIZE + DEF_HDCP2_STREAMID_TYPE_SIZE + DEF_HDCP2_K_SIZE + 1)];
	BYTE					u8StreamManage_k[DEF_HDCP2_K_SIZE];

    union
    {
        HDCPRX_IO_FUNC      pRxSendFunc; //pSendFunc;
        HDCPTX_IO_FUNC      pTxIOCbFunc;
    } pHdcpIOCbFunc;
    HDCPRX_FETCH_MSG_FUNC   pFetchMsgFunc;
    HDCPTX_COMPARE_RI_FUNC  pCompareRiFunc;
	HDCPTX_GET_DPTXR0_FUNC  pGetDPTxR0Func;
    HDCPRX_WRITEX74_FUNC    pWriteX74Func;
    HDCPRX_READX74_FUNC     pReadX74Func;
    HDCPREP_KSVFIFO_FUNC    pKSVFIFOFunc;
    HDCPREP_RX_HPDCTRL_FUNC  pRxHPDCTRLFunc;
} ST_HDCP_HANDLER;
//----------------------------------------------------------------------//
// Function prototype
//----------------------------------------------------------------------//
BOOL HDCPHandler_AttachCBFunc(BYTE u8PortIdx, HDCPRX_IO_FUNC pFunc);
BOOL HDCPHandler_AttachTxIOCBFunc(BYTE u8PortIdx, HDCPTX_IO_FUNC pFunc);
BOOL HDCPHandler_AttachTxCompareRiFunc(BYTE u8PortIdx, HDCPTX_COMPARE_RI_FUNC pFunc);
BOOL HDCPHandler_AttachTxGetDPTXR0Func(BYTE u8PortIdx, HDCPTX_GET_DPTXR0_FUNC pFunc);
BOOL HDCPHandler_AttachWriteX74Func(BYTE u8PortIdx, HDCPRX_WRITEX74_FUNC pFunc);
BOOL HDCPHandler_AttachReadX74Func(BYTE u8PortIdx, HDCPRX_READX74_FUNC pFunc);
BOOL HDCPHandler_AttachRepKSVFIFOFunc(BYTE u8PortIdx, HDCPREP_KSVFIFO_FUNC pFunc);
BOOL HDCPHandler_AttachRxHDCPCtrlFunc(BYTE u8PortIdx, HDCPREP_RX_HPDCTRL_FUNC pFunc);
void HDCPHandler_SetHdcpRetryCount(U8 u8RetryCnt);
ENUM_HDCP_ERR_CODE HDCPHandler_SECU_PowerDown(BOOL bSetPowerDown);
ENUM_HDCP_ERR_CODE HDCPHandler_SECU_SET_HDMI_DP(BOOL HDMI);
ENUM_HDCP_ERR_CODE HDCPHandler_DecodeHdcp1TxKey(BYTE* pu8Hdcp2TxKey);
ENUM_HDCP_ERR_CODE HDCPHandler_DecodeHdcp2RxKey(BYTE* pu8Hdcp2RxKey, BOOL bUseCustomizeKey);
ENUM_HDCP_ERR_CODE HDCPHandler_DecodeHdcp2TxKey(BYTE* pu8Hdcp2TxKey, BOOL bUseCustomizeKey);
void HDCPHandler_EnableHdcpTxOnHDMI(BYTE u8PortIdx, BYTE enHdcpVerCtrl, BOOL bEnable);
//void HDCPHandler_EnableHdcpTxOnDP(BYTE u8PortIdx, BYTE enHdcpVerCtrl, BOOL bEnable);
BYTE HDCPHandler_TxVerInfo(BYTE u8RxPortIdx);
void HDCPHandler_HdcpTxMsgParser(BYTE ucPortIdx, BYTE *pucMsg);
void HDCPHandler_HdcpRxMsgParser(BYTE ucPortIdx, BYTE *pucMsg);
void HDCPHandler_SetR2EventFlag(void);
void HDCPHandler_SetSecR2EventFlag(void);
BOOL HDCPHandler_GetR2EventFlag(void);
ULONG HDCPHandler_GetTxBufAddr(void);
BOOL HDCPHandler_Hdcp2MsgDispatcher(BYTE ucPortIdx, BYTE *pucMsg);
#if (DEF_HDCP2RX_ISR_MODE == 1)
BOOL HDCPHandler_AttachFetchMsgCBFunc(BYTE u8PortIdx, HDCPRX_FETCH_MSG_FUNC pFunc);
void HDCPHandler_Hdcp2RxISR(void);
#endif
void HDCP_Handler(void);
void HDCP_NonISRModeHandler(void);
void HDCPHandler_RebootInitValue(void);
void HDCP_ISRModeHandler(void);
void HDCPHandler_Init(WORD u16InputPortTypeMap, WORD u16OutputPortTypeMap, BOOL bIsRepeater);
void HDCPHandler_Hdcp14BKSVRead(BYTE ucPortIdx);
BOOL HDCPHandler_Hdcp14CheckVPrimePrepared(BYTE ucPortIdx);
ENUM_HDCP_ERR_CODE HDCPHandler_EncryptHdcp2RxKey(BYTE* pu8Hdcp2TxKey);
void HDCPHandler_HDCPLoadKeyViaEfuse(BYTE ucKeyType, BYTE* pucHDCPKeyTable, BYTE* pucHDCPBksvTable);
void HDCPHandler_LoadKeyViaEfuse(void);
void HDCPHandler_LoadHDCP14RxBksv(BYTE *pu8BKSV, BYTE *pu8Hdcp1RxKey, BOOL bExternalKey);
void HDCPHandler_LoadHDCP14RxKey(BYTE *pu8BKSV, BYTE *pu8Hdcp1RxKey, BOOL bExternalKey);
BOOL HDCPHandler_UpdateRomR2FirstEventFlag(void);
BOOL HDCPHandler_GetRomR2FirstEventFlag(void);
void HDCPHandler_ClrRomR2FirstEventFlag(void);
void HDCPHandler_SetR2EventFlag(void);
#endif //#ifndef HDCP_HANDLER_H
