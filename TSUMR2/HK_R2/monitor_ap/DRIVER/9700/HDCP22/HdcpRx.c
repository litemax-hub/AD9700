#ifndef HDCP_RX_C
#define HDCP_RX_C
//----------------------------------------------------------------------//
// includes
//----------------------------------------------------------------------//
#include <string.h>
#include "datatype.h"
#include "Global.h"
#include "Common.h"
#include "MsTypes.h"

#include "HdcpCommon.h"
#include "HdcpHandler.h"
#include "HdcpMbx.h"
#include "HdcpLog.h"
#include "HdcpError.h"
#include "HdcpMsgPool.h"
//#include "mhal_combo.h"
#include "drvIMI.h"
#include "drvGlobal.h"
#include "HdcpRx.h"
#include "mhal_hdmiRx.h"
//#include "timer.h"
#include "asmCPU.h"

//#if (ENABLE_SECU_R2)

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_TX_PORT_OFFSET              9
#define TX_PORT_OFFSET              COMBO_IP_SUPPORT_TYPE
#define GET_RECOVERY_TX_OFFSET(x)   (x-TX_PORT_OFFSET)
//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//
extern volatile ST_HDCP_HANDLER g_stHdcpHandler[DEF_HDCP_PORT_NUM];
extern volatile BYTE*           g_u8HdcpRecvBuf;//bit 7 of byte 0 is processing bit
extern volatile BYTE*           g_u8HdcpTransBuf;
extern const WORD g_u16Hdcp2MsgArgLen[DEF_HDCP2_CMD_NUM];
extern BYTE  g_u8RxTxPortPair[COMBO_IP_SUPPORT_TYPE];
static MS_U32 g_u32HDCPRxPreTime[INPUT_PORT_END] = {0x0};
static MS_U32 g_u32HDCPRxCurTime[INPUT_PORT_END] = {0x0};
extern BYTE XDATA tCOMBO_HDCP14_BKSV[DEF_HDCP1X_KSV_SIZE] ;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void __________PROTOTYPE__________(void);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKEInit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendCert(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKENoStoredKm(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKEStoredKm(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2HandlePairingInfo(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendHPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendPairingInfo(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2LCInit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2LCSendLPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2SKESendEks(BYTE u8PortIdx);
void HDCPRx_Hdcp2RepSetRdyBit(BYTE u8PortIdx, BOOL bEnable);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2Process_RecvIDList(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendRepeaerRecvListMsg(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2TriggerReAuthBit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2RepAuthRecvAck(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2RepAuthStreamManage(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2RepAuthStreamReady(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_SetKSVFIFO(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_WriteVPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_SetBstatus(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_SetReadyBit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_ComputeVPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCP14Rx_Handler(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCP14Rx_RepeaterHandler(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRx_Handler(BYTE u8PortIdx);
MS_U32 _HDCPRx_GetTimeDiff(MS_U32 dwPreTime, MS_U32 dwPostTime);
BOOL HDCPRx_Hdcp2MsgParser(BYTE ucPortIdx, BYTE *pucMsg);
BOOL HDCPRx_GetRi(BYTE ucInputPort, BYTE *ucRi);

void __________FUNCTION__________(void);

//----------------------------------------------------------------------//
// function
//----------------------------------------------------------------------//
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKEInit(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        memcpy((void*)((g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX + DEF_HDCP2_RTX_SIZE + DEF_HDCP2_TXCAPS_SIZE)), (void*)&g_stHdcpHandler[u8PortIdx].bAsRepeater, 1);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_AKE_INIT, EN_HDCPMBX_CMD_HOST_HKR2, \
            g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_INIT] + 1, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendCert(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_AKE_SEND_CERT;
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc(u8PortIdx, EN_HDCP2_MSG_AKE_SEND_CERT, (DEF_HDCP2_RRX_SIZE + DEF_HDCP2_RXCAPS_SIZE), (BYTE*)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE))));
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKENoStoredKm(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_AKE_NO_STORED_KM, EN_HDCPMBX_CMD_HOST_HKR2, \
            DEF_HDCP2_EKPUBKM_SIZE, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKEStoredKm(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_AKE_STORED_KM, EN_HDCPMBX_CMD_HOST_HKR2, \
            DEF_HDCP2_EKHKM_SIZE, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2HandlePairingInfo(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO, EN_HDCPMBX_CMD_HOST_HKR2, \
            0, 0) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));
        //HDCPMsgPool_SetActiveBit(u8PortIdx, FALSE);

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendHPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc(u8PortIdx, EN_HDCP2_MSG_AKE_SEND_H_PRIME, DEF_HDCP2_HPRIME_SIZE, (BYTE*)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE))));
    } while(FALSE);
    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendPairingInfo(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc(u8PortIdx, EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO, DEF_HDCP2_EKHKM_SIZE, (BYTE*)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE)+DEF_HDCP2_HPRIME_SIZE)));
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE)+DEF_HDCP2_HPRIME_SIZE)), DEF_HDCP2_EKHKM_SIZE);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2LCInit(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_LC_INIT, EN_HDCPMBX_CMD_HOST_HKR2, \
            DEF_HDCP2_RN_SIZE, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2LCSendLPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_LC_SEND_L_PRIME;
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc(u8PortIdx, EN_HDCP2_MSG_LC_SEND_L_PRIME, DEF_HDCP2_LPRIME_SIZE, (BYTE*)((ULONG)(g_u8HdcpTransBuf+ (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE)) | 0x80000000));

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2SKESendEks(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_SKE_SEND_EKS, EN_HDCPMBX_CMD_HOST_HKR2, \
            (DEF_HDCP2_EDKEYKS_SIZE + DEF_HDCP2_RIV_SIZE), (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}
void HDCPRx_Hdcp2RepSetRdyBit(BYTE u8PortIdx, BOOL bEnable)
{
	bEnable = bEnable; //T.B.D fix warning
    switch(u8PortIdx)
    {
/*
        case COMBO_INPUT_SUPPORT_5PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P0 + 0xC4, 0x0080 , 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P0 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P0 + 0xC4, 0x0000, 0x0080);
            break;

        case COMBO_INPUT_SUPPORT_6PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P1 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P1 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P1 + 0xC4, 0x0000, 0x0080);
            break;

        case COMBO_INPUT_SUPPORT_7PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P2 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P2 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P2 + 0xC4, 0x0000, 0x0080);
            break;

        case COMBO_INPUT_SUPPORT_8PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P3 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P3 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P3 + 0xC4, 0x0000, 0x0080);
            break;

        case COMBO_INPUT_SUPPORT_9PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P4 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P4 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P4 + 0xC4, 0x0000, 0x0080);
            break;

        case COMBO_INPUT_SUPPORT_10PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P5 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P5 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P5 + 0xC4, 0x0000, 0x0080);
            break;
        case COMBO_INPUT_SUPPORT_11PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P6 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P6 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P6 + 0xC4, 0x0000, 0x0080);
            break;
        case COMBO_INPUT_SUPPORT_12PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P7 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P7 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P7 + 0xC4, 0x0000, 0x0080);
            break;
        case COMBO_INPUT_SUPPORT_13PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P8 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P8 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P8 + 0xC4, 0x0000, 0x0080);
            break;
        case COMBO_INPUT_SUPPORT_14PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P9 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P9 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P9 + 0xC4, 0x0000, 0x0080);
            break;
        case COMBO_INPUT_SUPPORT_15PORT:
            msWrite2ByteMask(REG_HDCP_DUAL_P10 + 0xC4, 0x0080, 0x0080);
            msWrite2ByteMask(REG_HDCP_DUAL_P10 + 0xC4, bEnable ? 0x0000 : 0x0040, 0x0040);
            msWrite2ByteMask(REG_HDCP_DUAL_P10 + 0xC4, 0x0000, 0x0080);
            break;
*/

        default:

            break;
    }

}
ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2Process_RecvIDList(BYTE u8PortIdx)
{

    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8DeviceCnt;
    BYTE u8Depth;
    BYTE u8SeqNumV[DEF_HDCP2_SEQ_NUM_V_SIZE] = {0};
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        #if 0 //DEF_LG_USE
        u8DeviceCnt = 1;
        u8Depth = 1;
        g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt = u8DeviceCnt;
        g_stHdcpHandler[u8PortIdx].u8RxInfo[1] = g_stHdcpHandler[u8PortIdx].u8RxInfo[1] | ((u8DeviceCnt << 4) & 0xF0);
        g_stHdcpHandler[u8PortIdx].u8RxInfo[0] = g_stHdcpHandler[u8PortIdx].u8RxInfo[0] | ((u8DeviceCnt >> 4) & 0x01);
        g_stHdcpHandler[u8PortIdx].u8RxInfo[0] = g_stHdcpHandler[u8PortIdx].u8RxInfo[0] | ((u8Depth << 1) & 0x0E);
        #else
        u8DeviceCnt = g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8DownStreamDevCnt + 1;
        u8Depth = ((g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8RxInfo[0] & 0x0E) >> 1) + 1;
        memset((void*)g_stHdcpHandler[u8PortIdx].u8RxInfo, 0x00, DEF_HDCP2_RXINFO_SIZE);
        g_stHdcpHandler[u8PortIdx].u8RxInfo[1] = g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8RxInfo[1] & 0x0F;
        memcpy((void*)g_stHdcpHandler[u8PortIdx].u8RecvIDList, (void*)g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8RecvID, DEF_HDCP2_RECV_ID_SIZE);

        if(u8DeviceCnt > DEF_HDCP2_MAX_DEVICE_COUNT)
        {
            g_stHdcpHandler[u8PortIdx].u8RxInfo[1] = g_stHdcpHandler[u8PortIdx].u8RxInfo[1] | DEF_HDCP2_MAX_DEV_EXCEEDED;
            g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt = g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8DownStreamDevCnt;
            g_stHdcpHandler[u8PortIdx].u8RxInfo[1] = g_stHdcpHandler[u8PortIdx].u8RxInfo[1] | ((g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8DownStreamDevCnt << 4) & 0xF0);
            g_stHdcpHandler[u8PortIdx].u8RxInfo[0] = g_stHdcpHandler[u8PortIdx].u8RxInfo[0] | ((g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8DownStreamDevCnt >> 4) & 0x01);
        }
        else
        {
            g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt = u8DeviceCnt;
            g_stHdcpHandler[u8PortIdx].u8RxInfo[1] = g_stHdcpHandler[u8PortIdx].u8RxInfo[1] | ((u8DeviceCnt << 4) & 0xF0);
            g_stHdcpHandler[u8PortIdx].u8RxInfo[0] = g_stHdcpHandler[u8PortIdx].u8RxInfo[0] | ((u8DeviceCnt >> 4) & 0x01);
            memcpy((void*)(g_stHdcpHandler[u8PortIdx].u8RecvIDList + DEF_HDCP2_RECV_ID_SIZE), (void*)g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8RecvIDList, g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE);
        }
        if(u8Depth > DEF_HDCP2_MAX_DEPTH_LEVEL)
        {
            g_stHdcpHandler[u8PortIdx].u8RxInfo[1] = g_stHdcpHandler[u8PortIdx].u8RxInfo[1] | DEF_HDCP2_MAX_CASCADE_EXCEEDED;
            g_stHdcpHandler[u8PortIdx].u8RxInfo[0] = g_stHdcpHandler[u8PortIdx].u8RxInfo[0] | (((u8Depth - 1) << 1) & 0x0E);
        }
        else
        {
            g_stHdcpHandler[u8PortIdx].u8RxInfo[0] = g_stHdcpHandler[u8PortIdx].u8RxInfo[0] | ((u8Depth << 1) & 0x0E);
        }
        #endif
        u8SeqNumV[0] = (BYTE)((g_stHdcpHandler[u8PortIdx].u32RxSeqNumV & 0xFF0000)>>16);
        u8SeqNumV[1] = (BYTE)((g_stHdcpHandler[u8PortIdx].u32RxSeqNumV & 0x00FF00)>>8);
        u8SeqNumV[2] = (BYTE)((g_stHdcpHandler[u8PortIdx].u32RxSeqNumV & 0x0000FF));
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), (void*)(g_stHdcpHandler[u8PortIdx].u8RecvIDList), g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE);
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX + g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE), (void*)g_stHdcpHandler[u8PortIdx].u8RxInfo, DEF_HDCP2_RXINFO_SIZE);
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX + g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE + DEF_HDCP2_RXINFO_SIZE), (void*)u8SeqNumV, DEF_HDCP2_SEQ_NUM_V_SIZE);

        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST, EN_HDCPMBX_CMD_HOST_HKR2, \
            DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE  + g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));
    } while(FALSE);

    //HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2AKESendRepeaerRecvListMsg(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    //BYTE u8RxStatusHB;
    BYTE u8RetData[DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE>>1) + (g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt * DEF_HDCP2_RECV_ID_SIZE)];
    BYTE u8SeqNumV[DEF_HDCP2_SEQ_NUM_V_SIZE] = {0};
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

        u8SeqNumV[0] = (BYTE)((g_stHdcpHandler[u8PortIdx].u32RxSeqNumV & 0xFF0000)>>16);
        u8SeqNumV[1] = (BYTE)((g_stHdcpHandler[u8PortIdx].u32RxSeqNumV & 0x00FF00)>>8);
        u8SeqNumV[2] = (BYTE)((g_stHdcpHandler[u8PortIdx].u32RxSeqNumV & 0x0000FF));

        memcpy((void*)u8RetData, (void*)g_stHdcpHandler[u8PortIdx].u8RxInfo, DEF_HDCP2_RXINFO_SIZE);
        memcpy((void*)(u8RetData + DEF_HDCP2_RXINFO_SIZE), (void*)u8SeqNumV, DEF_HDCP2_SEQ_NUM_V_SIZE);
        memcpy((void*)(u8RetData + DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE), (void*)(ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE)), (DEF_HDCP2_VPRIME_SIZE>>1));
        memcpy((void*)(u8RetData + DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE>>1)), (void*)g_stHdcpHandler[u8PortIdx].u8RecvIDList, g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt * DEF_HDCP2_RECV_ID_SIZE);
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc(u8PortIdx, EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST, (DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE>>1) + (g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt * DEF_HDCP2_RECV_ID_SIZE)), (BYTE*)u8RetData);
        HDCPRx_Hdcp2RepSetRdyBit(u8PortIdx, TRUE);
        //printf("RecvList : \r\n");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u8RetData, DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE>>1) + (g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt * DEF_HDCP2_RECV_ID_SIZE));
        //u8RxStatusHB = g_stHdcpHandler[u8PortIdx].pReadX74Func(u8PortIdx, (EN_HDCP2X_X74_OFFSET_RXSTATUS+1));
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2TriggerReAuthBit(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8DelayCnt = 0;
    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        switch(u8PortIdx)
        {
            case INPUT_PORT3:
            {
                msWrite2ByteMask(REG_HDCP_DUAL_P0 + 0xCA, 0x0400, 0x0400);  // enable re-auth
                msWrite2ByteMask(REG_HDCP_DUAL_P0 + 0xCA, 0x0800, 0x0800);  // trigger re-auth
                while(u8DelayCnt < 50)
                {
                    u8DelayCnt++;
                }
                msWrite2ByteMask(REG_HDCP_DUAL_P0 + 0xCA, 0x0000, 0x0800);
                msWrite2ByteMask(REG_HDCP_DUAL_P0 + 0xCA, 0x0000, 0x0400);
            }
            break;

            case INPUT_PORT4:
            {
                msWrite2ByteMask(REG_HDCP_DUAL_P1 + 0xCA, 0x0400, 0x0400);
                msWrite2ByteMask(REG_HDCP_DUAL_P1 + 0xCA, 0x0800, 0x0800);
                while(u8DelayCnt < 50)
                {
                    u8DelayCnt++;
                }
                msWrite2ByteMask(REG_HDCP_DUAL_P1 + 0xCA, 0x0000, 0x0800);
                msWrite2ByteMask(REG_HDCP_DUAL_P1 + 0xCA, 0x0000, 0x0400);
            }
            break;

            case INPUT_PORT5:
            {
                msWrite2ByteMask(REG_HDCP_DUAL_P2 + 0xCA, 0x0400, 0x0400);
                msWrite2ByteMask(REG_HDCP_DUAL_P2 + 0xCA, 0x0800, 0x0800);
                while(u8DelayCnt < 50)
                {
                    u8DelayCnt++;
                }
                msWrite2ByteMask(REG_HDCP_DUAL_P2 + 0xCA, 0x0000, 0x0800);
                msWrite2ByteMask(REG_HDCP_DUAL_P2 + 0xCA, 0x0000, 0x0400);
            }
            break;

            case INPUT_PORT6:
            {
                msWrite2ByteMask(REG_HDCP_DUAL_P3 + 0xCA, 0x0400, 0x0400);
                msWrite2ByteMask(REG_HDCP_DUAL_P3 + 0xCA, 0x0800, 0x0800);
                while(u8DelayCnt < 50)
                {
                    u8DelayCnt++;
                }
                msWrite2ByteMask(REG_HDCP_DUAL_P3 + 0xCA, 0x0000, 0x0800);
                msWrite2ByteMask(REG_HDCP_DUAL_P3 + 0xCA, 0x0000, 0x0400);
            }
            break;

            case INPUT_PORT7:
            {
                msWrite2ByteMask(REG_HDCP_DUAL_P4 + 0xCA, 0x0400, 0x0400);
                msWrite2ByteMask(REG_HDCP_DUAL_P4 + 0xCA, 0x0800, 0x0800);
                while(u8DelayCnt < 50)
                {
                    u8DelayCnt++;
                }
                msWrite2ByteMask(REG_HDCP_DUAL_P4 + 0xCA, 0x0000, 0x0800);
                msWrite2ByteMask(REG_HDCP_DUAL_P4 + 0xCA, 0x0000, 0x0400);
            }
            break;

            case INPUT_PORT8:
            {
                msWrite2ByteMask(REG_HDCP_DUAL_P5 + 0xCA, 0x0400, 0x0400);
                msWrite2ByteMask(REG_HDCP_DUAL_P5 + 0xCA, 0x0800, 0x0800);
                while(u8DelayCnt < 50)
                {
                    u8DelayCnt++;
                }
                msWrite2ByteMask(REG_HDCP_DUAL_P5 + 0xCA, 0x0000, 0x0800);
                msWrite2ByteMask(REG_HDCP_DUAL_P5 + 0xCA, 0x0000, 0x0400);
            }
            break;

            default:
                break;
        }
    }while(FALSE);
    HTRACEL(EN_HLOG_HDCP2X);
    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2RepAuthRecvAck(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        HDCPRx_Hdcp2RepSetRdyBit(u8PortIdx, FALSE);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_REPAUTH_SEND_ACK, EN_HDCPMBX_CMD_HOST_HKR2, \
            DEF_HDCP2_V_SIZE>>1, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2RepAuthStreamManage(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SINK, EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE, EN_HDCPMBX_CMD_HOST_HKR2, \
            (DEF_HDCP2_SEQ_NUM_M_SIZE + (2 * g_stHdcpHandler[u8PortIdx].u8StreamManage_k[1]) + DEF_HDCP2_K_SIZE), (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }
        memcpy((void*)g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8StreamIDType, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX + DEF_HDCP2_SEQ_NUM_M_SIZE + DEF_HDCP2_K_SIZE), DEF_HDCP2_STREAMID_TYPE_SIZE);
        g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].bRx2TxStreamType = TRUE;
        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
        HDCPMsgPool_SetPriority(u8PortIdx, (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)) + DEF_HDCP_PORT_NUM));

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Hdcp2RepAuthStreamReady(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc(u8PortIdx, EN_HDCP2_MSG_REPAUTH_STREAM_READY, DEF_HDCP2_REP_M_SIZE, (BYTE*)(ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE)));
		g_stHdcpHandler[u8PortIdx].bRecvStreamManage = FALSE;
	} while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_SetKSVFIFO(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8DeviceKVCnt = g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt;
    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (g_stHdcpHandler[u8PortIdx].pKSVFIFOFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].pKSVFIFOFunc(u8PortIdx, u8DeviceKVCnt*DEF_HDCP1X_KSV_SIZE, (BYTE*)g_stHdcpHandler[u8PortIdx].u8KSVFIFO);
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, (u8DeviceKVCnt)*DEF_HDCP1X_KSV_SIZE);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_WriteVPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (g_stHdcpHandler[u8PortIdx].pWriteX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        memcpy((void*)g_stHdcpHandler[u8PortIdx].u8VPrime, (BYTE*)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)), DEF_HDCP1X_V_SIZE);
        g_stHdcpHandler[u8PortIdx].pWriteX74Func(u8PortIdx, 0x20, DEF_HDCP1X_V_SIZE, (BYTE*)g_stHdcpHandler[u8PortIdx].u8VPrime);
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)g_stHdcpHandler[u8PortIdx].u8VPrime, DEF_HDCP1X_V_SIZE);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_SetBstatus(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);
     do
    {
        if (g_stHdcpHandler[u8PortIdx].pWriteX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

		//for HDMI HDCP14 repeater
		#if 0
        if (g_stHdcpHandler[u8PortIdx].pReadX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].u8BStatus[1] = (g_stHdcpHandler[u8PortIdx].pReadX74Func(GET_RECOVERY_TX_OFFSET(g_u8RxTxPortPair[u8PortIdx]), (EN_HDCP1X_X74_OFFSET_BSTATUS + 1)) & 0xF0) | g_stHdcpHandler[u8PortIdx].u8BStatus[1];
		#endif

        g_stHdcpHandler[u8PortIdx].pWriteX74Func(u8PortIdx, EN_HDCP1X_X74_OFFSET_BSTATUS, DEF_HDCP1X_BSTATUS_SIZE, (BYTE*)g_stHdcpHandler[u8PortIdx].u8BStatus);
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)g_stHdcpHandler[u8PortIdx].u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_SetReadyBit(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pWriteX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        if (g_stHdcpHandler[u8PortIdx].pReadX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].bRdyBitSet = TRUE;
        g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit = FALSE;
		g_stHdcpHandler[u8PortIdx].u8BCaps = g_stHdcpHandler[u8PortIdx].pReadX74Func(GET_RECOVERY_TX_OFFSET(g_u8RxTxPortPair[u8PortIdx]), EN_HDCP1X_X74_OFFSET_BCAPS)|0x20;
        g_stHdcpHandler[u8PortIdx].pWriteX74Func(u8PortIdx, EN_HDCP1X_X74_OFFSET_BCAPS, DEF_HDCP1X_BCAPS_SIZE, (BYTE*)&g_stHdcpHandler[u8PortIdx].u8BCaps);
        g_stHdcpHandler[u8PortIdx].pWriteX74Func(u8PortIdx, EN_HDCP1X_X74_OFFSET_BSTATUS, DEF_HDCP1X_BSTATUS_SIZE, (BYTE*)g_stHdcpHandler[u8PortIdx].u8BStatus);
        //printf("SetRxRdyBit %02X \r\n", g_stHdcpHandler[u8PortIdx].u8BCaps);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_ComputeVPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8DeviceKVCnt = g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), (void*)&u8DeviceKVCnt, 1);
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1), (void*)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, (u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE));
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1 + (u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE)), (void*)g_stHdcpHandler[u8PortIdx].u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);

        //printf("SetVprimeKSVFIFO : ");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, (u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE));
        //printf("SetVPrimeBstatus : ");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)g_stHdcpHandler[u8PortIdx].u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_COMPUTE_H1XRX_VPRIME, EN_HDCPMBX_CMD_HOST_HKR2,\
            ((u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE) + DEF_HDCP1X_KSV_FIFO_SIZE + DEF_HDCP1X_BSTATUS_SIZE) , (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;

}

BOOL HDCPRx_GetHDCP14_Info(BYTE u8PortIdx, ST_HDCP14_LG_INFO* stHDCPInfo)
{
    BOOL bRet = TRUE;
    BOOL bEnc = FALSE;
    BYTE u8Status;
    BYTE u8AN[DEF_HDCP1X_AN_SIZE] = {0};
    BYTE u8AKSV[DEF_HDCP1X_KSV_SIZE] = {0};
    BYTE u8BKSV[DEF_HDCP1X_KSV_SIZE] = {0};
    BYTE u8Ri[DEF_HDCP1X_RI_SIZE] = {0};
    BYTE u8Bcaps = 0;
    BYTE u8BStatus[DEF_HDCP1X_BSTATUS_SIZE] = {0};
    BYTE u8Cnt;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pReadX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            bRet = FALSE;
            break;
        }

        bEnc = HDCPRx_GetEncryptionEnc(u8PortIdx);
        memcpy((void*)&stHDCPInfo->bEnc14, (void*)&bEnc, 1);   // byte 0 Enc

        u8Status = HDCPRx_GetAuthStatus(u8PortIdx);
        memcpy((void*)&stHDCPInfo->u8Status, (void*)&u8Status, 1);   // byte 1 Status

        for(u8Cnt=0 ; u8Cnt < DEF_HDCP1X_AN_SIZE ; u8Cnt++)
        {
            u8AN[u8Cnt] = g_stHdcpHandler[u8PortIdx].pReadX74Func(u8PortIdx, (EN_HDCP1X_X74_OFFSET_AN + u8Cnt));
        }
        memcpy((void*)(stHDCPInfo->u8AN), (void*)u8AN, DEF_HDCP1X_AN_SIZE);   // byte 2 ~ 9 AN


        for(u8Cnt=0 ; u8Cnt < DEF_HDCP1X_KSV_SIZE ; u8Cnt++)
        {
            u8AKSV[u8Cnt] = g_stHdcpHandler[u8PortIdx].pReadX74Func(u8PortIdx, (EN_HDCP1X_X74_OFFSET_AKSV + u8Cnt));
        }
        memcpy((void*)(stHDCPInfo->u8AKSV), (void*)u8AKSV, DEF_HDCP1X_KSV_SIZE);   // byte 10 ~ 14 AKSV

        for(u8Cnt=0 ; u8Cnt < DEF_HDCP1X_KSV_SIZE ; u8Cnt++)
        {
            u8BKSV[u8Cnt] = g_stHdcpHandler[u8PortIdx].pReadX74Func(u8PortIdx, (EN_HDCP1X_X74_OFFSET_BKSV + u8Cnt));
        }
        memcpy((void*)(stHDCPInfo->u8BKSV), (void*)u8BKSV, DEF_HDCP1X_KSV_SIZE);   // byte 15 ~ 19 BKSV

        HDCPRx_GetRi(u8PortIdx, u8Ri);
        memcpy((void*)(stHDCPInfo->u8Ri), (void*)u8Ri, DEF_HDCP1X_RI_SIZE);   // byte 20 ~ 21 Ri

        u8Bcaps = g_stHdcpHandler[u8PortIdx].pReadX74Func(u8PortIdx, EN_HDCP1X_X74_OFFSET_BCAPS);
        memcpy((void*)&stHDCPInfo->u8Bcaps, (void*)&u8Bcaps, DEF_HDCP1X_BCAPS_SIZE);   // byte 22 Bcaps

        for(u8Cnt=0 ; u8Cnt < DEF_HDCP1X_BSTATUS_SIZE ; u8Cnt++)
        {
            u8BStatus[u8Cnt] = g_stHdcpHandler[u8PortIdx].pReadX74Func(u8PortIdx, (EN_HDCP1X_X74_OFFSET_BSTATUS + u8Cnt));
        }
        memcpy((void*)(stHDCPInfo->u8BStatus), (void*)u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);   // byte 23~ 24 BStatus

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;

}

ENUM_HDCP_ERR_CODE HDCP14Rx_Handler(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    ENUM_HDCPMBX_STATE_LIST enState = EN_HDCPMBX_STATE_IDLE;
    BYTE u8DeviceKVCnt;
    BYTE u8NewBStatus[2];
    BYTE XDATA u8KSVList[DEF_HDCP1X_MAX_KSV_SIZE * DEF_HDCP1X_KSV_SIZE];
    do
    {
        if(g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit)
        {
            if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
            {
				HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED,"NoTxConnectComputeVprime %d", u8PortIdx);

				g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt	= 1;
				u8DeviceKVCnt = 1;

				u8NewBStatus[0] = 1;
				u8NewBStatus[1] = 1;

				memcpy((void*)u8KSVList, tCOMBO_HDCP14_BKSV, DEF_HDCP1X_KSV_SIZE);

				memcpy((void*)g_stHdcpHandler[u8PortIdx].u8BStatus, u8NewBStatus, DEF_HDCP1X_BSTATUS_SIZE);
				memcpy((void*)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, u8KSVList, u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE);
				if(HDCPRx_SetBstatus(u8PortIdx) != EN_HDCP_ERR_NONE)
				{
					break;
				}
				if(HDCPRx_ComputeVPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
				{
					break;
				}

            }
            else
            {
                if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                {
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Get State Fail!");
                    break;
                }

                if (enState == EN_HDCPMBX_STATE_HALT)
                {
                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                    break;
                }

                if (enState != EN_HDCPMBX_STATE_DONE)
                {
                    HLOGV(EN_HLOG_HDCP, "Command NOT Done");
                    break;
                }

                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                if(HDCPRx_WriteVPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
                {
                    break;
                }
                if(HDCPRx_SetKSVFIFO(u8PortIdx) != EN_HDCP_ERR_NONE)
                {
                    break;
                }
				g_stHdcpHandler[u8PortIdx].u8BCaps = g_stHdcpHandler[u8PortIdx].u8BCaps | 0x20;
				g_stHdcpHandler[u8PortIdx].pWriteX74Func(u8PortIdx, EN_HDCP1X_X74_OFFSET_BCAPS, DEF_HDCP1X_BCAPS_SIZE, (BYTE*)&g_stHdcpHandler[u8PortIdx].u8BCaps);
				g_stHdcpHandler[u8PortIdx].pWriteX74Func(u8PortIdx, EN_HDCP1X_X74_OFFSET_BSTATUS, DEF_HDCP1X_BSTATUS_SIZE, (BYTE*)g_stHdcpHandler[u8PortIdx].u8BStatus);

                g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit = FALSE;
            }
        }
    }while(FALSE);
    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCP14Rx_RepeaterHandler(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    ENUM_HDCPMBX_STATE_LIST enState = EN_HDCPMBX_STATE_IDLE;
    do
    {
        if(g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit && g_stHdcpHandler[u8PortIdx].bTx2RxInfo)
        {
            if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
            {
                if(HDCPRx_SetBstatus(u8PortIdx) != EN_HDCP_ERR_NONE)
                {
                    break;
                }
                if(HDCPRx_ComputeVPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
                {
                    break;
                }
            }
            else
            {
                if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                {
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Get State Fail!");
                    break;
                }

                if (enState == EN_HDCPMBX_STATE_HALT)
                {
                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                    break;
                }

                if (enState != EN_HDCPMBX_STATE_DONE)
                {
                    HLOGV(EN_HLOG_HDCP, "Command NOT Done");
                    break;
                }

                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                if(HDCPRx_WriteVPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
                {
                    break;
                }
                if(HDCPRx_SetKSVFIFO(u8PortIdx) != EN_HDCP_ERR_NONE)
                {
                    break;
                }
                if(HDCPRx_SetReadyBit(u8PortIdx)!= EN_HDCP_ERR_NONE)
                {
                    break;
                }
                g_stHdcpHandler[u8PortIdx].bTx2RxInfo = FALSE;
                g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit = FALSE;
            }
        }
    }while(FALSE);
    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRx_Handler(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    ENUM_HDCPMBX_STATE_LIST enState;

    switch (g_stHdcpHandler[u8PortIdx].u8MainState)
    {
        case EN_HDCP2RX_MS_B0:
        {
            switch (g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                    //waiting msg
                break;

                case EN_HDCP2_MSG_AKE_INIT:
                {
					if(CURRENT_INPUT_IS_DISPLAYPORT())
	                {
	                	HDCPHandler_SECU_SET_HDMI_DP(FALSE);//DP mode
					}
	                else
	                {
	                	HDCPHandler_SECU_SET_HDMI_DP(TRUE);//HDMI mode
	                }
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Port#%d Get State Fail!", u8PortIdx);
                        break;
                    }

					//check if security r2 process is halt or done;
                    #if 0
					if((enState != EN_HDCPMBX_STATE_IDLE) && (enState != EN_HDCPMBX_STATE_DONE) && (enState != EN_HDCPMBX_STATE_HALT))
					{
						HLOGI(EN_HLOG_HDCP2X, "Port #%d: AKE_Init Return!State = %d", u8PortIdx, enState);
						break;
					}
                    #endif
                    // Clear Rdy bit
                    if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                    {
                        HDCPRx_Hdcp2RepSetRdyBit(u8PortIdx, FALSE);
                    }
                    //update state
                    g_stHdcpHandler[u8PortIdx].bAuthDone = FALSE;
                    #if 0//DEF_LG_USE  // if receiver ID was gotten, Tx2RxInfo will be always set after Authdone status was cleared.
                    if(g_stHdcpHandler[u8PortIdx].bRecvIDSet)
                    {
                        g_stHdcpHandler[u8PortIdx].bTx2RxInfo = TRUE;
                    }
                    #endif
                    g_stHdcpHandler[u8PortIdx].bSendPair = FALSE;
                    g_stHdcpHandler[u8PortIdx].u32RxSeqNumV = 0;
                    g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2RX_MS_B1;
                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                    /*g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8MainState = EN_HDCP2TX_MS_A0F0;
                    g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].u8SubState = EN_HDCP2_MSG_ZERO;
                    g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].bSendAKEInit = FALSE;*/
                    memset((void*)g_stHdcpHandler[u8PortIdx].u8RxInfo, 0x00, DEF_HDCP2_RXINFO_SIZE);
                    g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                    g_stHdcpHandler[u8PortIdx].bRecvStreamManage = FALSE;
                    g_stHdcpHandler[u8PortIdx].bRecvAck = FALSE;
                    g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].bRxHDCPStart = TRUE;
                    HDCPRx_Hdcp2AKEInit(u8PortIdx);
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State AKEINIT!");
                break;
            }
        }
        break;

        case EN_HDCP2RX_MS_B1:
        {
            switch (g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }

                    if ((enState != EN_HDCPMBX_STATE_DONE)&&(enState != EN_HDCPMBX_STATE_INIT))
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command Not Done");
                        break;
                    }

                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                    if (HDCPRx_Hdcp2AKESendCert(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {

                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "HDCPRx_Hdcp2AKESendCert() Fail!");
                        break;
                    }
                }
                break;

                case EN_HDCP2_MSG_AKE_NO_STORED_KM:
                {
                    g_stHdcpHandler[u8PortIdx].bStoredKm = FALSE;
                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_AKE_SEND_H_PRIME;
                    HDCPRx_Hdcp2AKENoStoredKm(u8PortIdx);
                }
                break;

                case EN_HDCP2_MSG_AKE_STORED_KM:
                {

                    g_stHdcpHandler[u8PortIdx].bStoredKm = TRUE;
                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_AKE_SEND_H_PRIME;
                    HDCPRx_Hdcp2AKEStoredKm(u8PortIdx);
                }
                break;

                case EN_HDCP2_MSG_AKE_SEND_H_PRIME:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }

                    if (enState != EN_HDCPMBX_STATE_DONE)
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command Not Done");
                        break;
                    }

                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                    if (g_stHdcpHandler[u8PortIdx].bStoredKm == FALSE)
                    {
                        g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO;
                        HDCPRx_Hdcp2AKESendHPrime(u8PortIdx);

                    }
                    else
                    {
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPRx_Hdcp2AKESendHPrime(u8PortIdx);
                    }
                }
                break;

                case EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO:
                {
                    #if 0
                    {
                        if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                        {
                            HDCPRx_Hdcp2HandlePairingInfo(u8PortIdx);
                        }
                        else
                        {
                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;
                            //while(!(mhal_combo_HDCP2GetReadDownFlag(u8PortIdx)));

                            if (HDCPRx_Hdcp2AKESendPairingInfo(u8PortIdx) != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "HDCPRx_Hdcp2AKESendPairingInfo() Fail!");
                                break;
                            }
                        }
                    }
                    #else
                    {
                        if(g_stHdcpHandler[u8PortIdx].bReadDone && (g_stHdcpHandler[u8PortIdx].bSendPair == FALSE))
                        {
                            g_stHdcpHandler[u8PortIdx].bReadDone = FALSE;
                            g_stHdcpHandler[u8PortIdx].bSendPair = TRUE;
                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_GREEN, "port:%d", u8PortIdx);
                            HDCPRx_Hdcp2AKESendPairingInfo(u8PortIdx);
                        }
                    }
                    #endif
                }
                break;

                case EN_HDCP2_MSG_LC_INIT:
                {

                    g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2RX_MS_B2;
                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                    HDCPRx_Hdcp2LCInit(u8PortIdx);
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State! B1");
                break;
            }
        }
        break;

        case EN_HDCP2RX_MS_B2:
        {
            switch (g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }

                    if (enState != EN_HDCPMBX_STATE_DONE)
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command Not Done");
                        break;
                    }

                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                    HDCPRx_Hdcp2LCSendLPrime(u8PortIdx);
                }
                break;

                case EN_HDCP2_MSG_LC_INIT:
                {

                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                    HDCPRx_Hdcp2LCInit(u8PortIdx);
                }
                break;

                case EN_HDCP2_MSG_SKE_SEND_EKS:
                {

                    g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2RX_MS_B3;
                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                    HDCPRx_Hdcp2SKESendEks(u8PortIdx);
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State! B2");
                break;
            }
        }
        break;

        case EN_HDCP2RX_MS_B3:
        {
            if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
            {
                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                break;
            }

            if (enState != EN_HDCPMBX_STATE_DONE)
            {
                HLOGV(EN_HLOG_HDCP2X, "Command Not Done");
                break;
            }

            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
            g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;
            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_GREEN, "Hdcp22 RX Auth Done@Port #%d", u8PortIdx);
            g_stHdcpHandler[u8PortIdx].bAuthDone = TRUE;
			g_stHdcpHandler[u8PortIdx].bIsSendRecvList = FALSE;
            g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2RX_MS_B4;
            g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
			g_u32HDCPRxPreTime[u8PortIdx] = MAsm_GetSystemTime();
        }
        break;

        case EN_HDCP2RX_MS_B4:
        {
            switch (g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                {
                    if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                    {
                        if(g_stHdcpHandler[u8PortIdx].bTx2RxInfo)
                        {
                        	//avoid retry StreamManage NG
                        	if(g_stHdcpHandler[u8PortIdx].bRecvStreamManage)
                    		{
								#if (DEF_HDCP2RX_ISR_MODE)
								if (g_stHdcpHandler[u8PortIdx].bRecvMsg)
								{
									//fetch message
									memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX),(void*)g_stHdcpHandler[u8PortIdx].u8RecvManageStream, (DEF_HDCP2_SEQ_NUM_M_SIZE + (2 * g_stHdcpHandler[u8PortIdx].u8StreamManage_k[1]) + DEF_HDCP2_K_SIZE + 1));

									//assign priority
									HDCPMsgPool_AssignPriority(u8PortIdx);

									HDCPRx_Hdcp2RepAuthStreamManage(u8PortIdx);
									g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
									g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_REPAUTH_STREAM_READY;

								}
								#endif

                    			break;
                    		}

							g_u32HDCPRxCurTime[u8PortIdx] = MAsm_GetSystemTime();
							if(_HDCPRx_GetTimeDiff(g_u32HDCPRxPreTime[u8PortIdx], g_u32HDCPRxCurTime[u8PortIdx]) > 200) //for HDCP22 CTS 3C 01 test case 2 stream management parallel
							{
	                            g_stHdcpHandler[u8PortIdx].bIsSendRecvList = TRUE;  // set if send RcvList
	                            if (HDCPRx_Hdcp2Process_RecvIDList(u8PortIdx) != EN_HDCP_ERR_NONE)
	                            {
	                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "HDCPRx_Hdcp2Process_RecvIDList() Fail!");
	                                break;
	                            }
	                            g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST;
							}
                        }
                    }
                    else
                    {
                        g_stHdcpHandler[u8PortIdx].bAuthDone = TRUE;
                        HDCPMBX_SetState(u8PortIdx, EN_HDCPMBX_STATE_IDLE);
                    }
                }
                break;

                case EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }

                    if ((enState != EN_HDCPMBX_STATE_DONE)&&(enState != EN_HDCPMBX_STATE_INIT))
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command Not Done");
                        break;
                    }

                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                    if (HDCPRx_Hdcp2AKESendRepeaerRecvListMsg(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {

                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "HDCPRx_Hdcp2AKESendRepeaerRecvListMsg() Fail!");
                        break;
                    }
                    g_stHdcpHandler[u8PortIdx].u32RxSeqNumV++;
                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_REPAUTH_SEND_ACK;
                    g_u32HDCPRxPreTime[u8PortIdx] = MAsm_GetSystemTime();
                }
                break;

                case EN_HDCP2_MSG_REPAUTH_SEND_ACK:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    {
                        g_u32HDCPRxCurTime[u8PortIdx] = MAsm_GetSystemTime();
                        if(_HDCPRx_GetTimeDiff(g_u32HDCPRxPreTime[u8PortIdx], g_u32HDCPRxCurTime[u8PortIdx]) > DEF_HDCP2_REP_SEND_ACK)
                        {
                            enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Rep Send Ack Timeout!");
                            g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2RX_MS_B0;
                            g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                            HDCPRx_Hdcp2RepSetRdyBit(u8PortIdx, FALSE);
                            HDCPRx_Hdcp2TriggerReAuthBit(u8PortIdx);
                            break;
                        }
                        if (g_stHdcpHandler[u8PortIdx].bRecvAck)
                        {
                            HDCPRx_Hdcp2RepAuthRecvAck(u8PortIdx);
                            g_stHdcpHandler[u8PortIdx].bRecvAck = FALSE;
                        }
                    }
                    else
                    {
                        if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                        {
                            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                            break;
                        }

                        if (enState == EN_HDCPMBX_STATE_HALT)
                        {
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Verify Least V Fail!");
                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2RX_MS_B0;
                            g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                            HDCPRx_Hdcp2RepSetRdyBit(u8PortIdx, FALSE);
                            HDCPRx_Hdcp2TriggerReAuthBit(u8PortIdx);
                            break;
                        }

                        if (enState != EN_HDCPMBX_STATE_DONE)
                        {
                            HLOGV(EN_HLOG_HDCP2X, "Command Not Done");
                            break;
                        }

                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;
                        g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE;
                        #if (DEF_HDCP2RX_ISR_MODE)
						if (g_stHdcpHandler[u8PortIdx].bRecvMsg)
						{
							//fetch message
							memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX),(void*)g_stHdcpHandler[u8PortIdx].u8RecvManageStream, (DEF_HDCP2_SEQ_NUM_M_SIZE + (2 * g_stHdcpHandler[u8PortIdx].u8StreamManage_k[1]) + DEF_HDCP2_K_SIZE + 1));

							//assign priority
							HDCPMsgPool_AssignPriority(u8PortIdx);

							HDCPRx_Hdcp2RepAuthStreamManage(u8PortIdx);
							g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
							g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_REPAUTH_STREAM_READY;

						}
                        #endif
                    }
                }
                break;

                case EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE:
                {

                    if (g_stHdcpHandler[u8PortIdx].bRecvMsg)
                    {
                        //fetch message
                        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX),(void*)g_stHdcpHandler[u8PortIdx].u8RecvManageStream, (DEF_HDCP2_SEQ_NUM_M_SIZE + (2 * g_stHdcpHandler[u8PortIdx].u8StreamManage_k[1]) + DEF_HDCP2_K_SIZE + 1));

                        //assign priority
                        HDCPMsgPool_AssignPriority(u8PortIdx);

                        HDCPRx_Hdcp2RepAuthStreamManage(u8PortIdx);
                        g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                        g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_REPAUTH_STREAM_READY;

                    }
                    else
                    {
                        break;
                    }

                }
                break;

                case EN_HDCP2_MSG_REPAUTH_STREAM_READY:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }

                    if (enState != EN_HDCPMBX_STATE_DONE)
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command Not Done");
                        break;
                    }
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPRx_Hdcp2RepAuthStreamReady(u8PortIdx);
                    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State! B4");
                break;
            }
        }
        break;

        default:
            enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State! Main");
        break;
    }

    return enErrCode;
}

BOOL HDCPRx_Hdcp2MsgParser(BYTE ucPortIdx, BYTE *pucMsg)
{
    BOOL bEnterHandlerFlag = TRUE;
    ULONG u32ArgAddr = 0;
    HTRACEE(EN_HLOG_HDCP2X);
    u32ArgAddr = (ULONG)(g_u8HdcpRecvBuf + (ucPortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX);

    switch (*pucMsg)
    {
        case EN_HDCP2_MSG_ZERO:
        {
            if((g_stHdcpHandler[ucPortIdx].u8SubState == EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO) && !g_stHdcpHandler[ucPortIdx].bSendPair)
            {
                g_stHdcpHandler[ucPortIdx].bReadDone = TRUE;
                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv ReadDone");
            }
            else
            {
                bEnterHandlerFlag = FALSE;
            }
            break;
        }
        case EN_HDCP2_MSG_AKE_INIT:
        {

            if (g_stHdcpHandler[ucPortIdx].bPolling == TRUE)
            {
                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_GREEN, "Receive AKE_Init, Halt Process!");
                HDCPMBX_SetState(ucPortIdx, EN_HDCPMBX_STATE_INIT);
                HDCPMBX_SetActiveBit(ucPortIdx, FALSE);
                g_stHdcpHandler[ucPortIdx].bPolling = FALSE;
            }
            else
            {
                HDCPMBX_SetState(ucPortIdx, EN_HDCPMBX_STATE_IDLE);
                HDCPMBX_ClearCmd(ucPortIdx);
            }

            //update FSM
            g_stHdcpHandler[ucPortIdx].u8MainState = (BYTE)EN_HDCP2RX_MS_B0;
            g_stHdcpHandler[ucPortIdx].u8SubState = (BYTE)EN_HDCP2_MSG_AKE_INIT;
            //update RxAuthVersion
            g_stHdcpHandler[ucPortIdx].u8HDCPRxVer = EN_HDCP_VER_22;
            //init buff
            memset((void*)u32ArgAddr, 0x00, DEF_HDCP_RECV_BUF_SIZE);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_INIT]);

            //assign priority
            HDCPMsgPool_SetPriority(ucPortIdx, DEF_HDCPMSG_MAX_PRIORITY);
            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv AKE_Init");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_INIT]);
        }
        break;

        case EN_HDCP2_MSG_AKE_NO_STORED_KM:
        {

            if (*(BYTE*)(g_u8HdcpRecvBuf + (ucPortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGWC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Already Got Message!");
                break;
            }

            //update FSM
            g_stHdcpHandler[ucPortIdx].u8SubState = (BYTE)EN_HDCP2_MSG_AKE_NO_STORED_KM;

            //init buff
            memset((void*)u32ArgAddr, 0x00, DEF_HDCP_RECV_BUF_SIZE);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_NO_STORED_KM]);

            //assign priority
            HDCPMsgPool_AssignPriority(ucPortIdx);

            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv AKE_No_Stored_Km");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_NO_STORED_KM]);
        }
        break;

        case EN_HDCP2_MSG_AKE_STORED_KM:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (ucPortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGWC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Already Got Message!");
                break;
            }

            //update FSM
            g_stHdcpHandler[ucPortIdx].u8SubState = (BYTE)EN_HDCP2_MSG_AKE_STORED_KM;

            //init buff
            memset((void*)u32ArgAddr, 0x00, DEF_HDCP_RECV_BUF_SIZE);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_STORED_KM]);
            //assign priority
            HDCPMsgPool_AssignPriority(ucPortIdx);

            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv AKE_Stored_Km");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_STORED_KM]);
        }
        break;

        case EN_HDCP2_MSG_LC_INIT:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (ucPortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGWC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Already Got Message!");
                break;
            }

            //update FSM
            g_stHdcpHandler[ucPortIdx].u8SubState = (BYTE)EN_HDCP2_MSG_LC_INIT;

            //init buff
            memset((void*)u32ArgAddr, 0x00, DEF_HDCP_RECV_BUF_SIZE);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_LC_INIT]);

            //assign priority
            HDCPMsgPool_SetPriority(ucPortIdx, (DEF_HDCPMSG_MAX_PRIORITY - 1));

            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv LC_Init");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_LC_INIT]);
        }
        break;

        case EN_HDCP2_MSG_SKE_SEND_EKS:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (ucPortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGWC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Already Got Message!");
                break;
            }

            //update FSM
            g_stHdcpHandler[ucPortIdx].u8SubState = (BYTE)EN_HDCP2_MSG_SKE_SEND_EKS;

            //init buff
            memset((void*)u32ArgAddr, 0x00, DEF_HDCP_RECV_BUF_SIZE);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_SKE_SEND_EKS]);

            //assign priority
            HDCPMsgPool_AssignPriority(ucPortIdx);

            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv SKE_Send_Eks");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_SKE_SEND_EKS]);
        }
        break;

        case EN_HDCP2_MSG_REPAUTH_SEND_ACK:
        {

            if (*(BYTE*)(g_u8HdcpRecvBuf + (ucPortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGWC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Already Got Message!");
                break;
            }
            //update FSM
            g_stHdcpHandler[ucPortIdx].u8SubState = (BYTE)EN_HDCP2_MSG_REPAUTH_SEND_ACK;

            //init buff
            memset((void*)u32ArgAddr, 0x00, DEF_HDCP_RECV_BUF_SIZE);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_REPAUTH_SEND_ACK]);

            //assign priority
            HDCPMsgPool_AssignPriority(ucPortIdx);
            g_stHdcpHandler[ucPortIdx].bRecvAck= TRUE;
            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv RepAuth_Send_Ack");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_REPAUTH_SEND_ACK]);

        }
        break;

        case EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE:
        {
#if 0
            if (*(BYTE*)(g_u8HdcpRecvBuf + (ucPortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                bEnterHandlerFlag = FALSE;
                HLOGWC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Already Got Message!");
                break;
            }
#endif
            //update FSM
            //g_stHdcpHandler[ucPortIdx].u8SubState = (BYTE)EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE;

            //init buff
            //memset((void*)u32ArgAddr, 0x00, DEF_HDCP_RECV_BUF_SIZE);

            //fetch message to buffer avoid tx send ManageStream too soon

			memcpy((void*)g_stHdcpHandler[ucPortIdx].u8StreamManage_k, pucMsg + (DEF_HDCP2_SEQ_NUM_M_SIZE + 1), DEF_HDCP2_K_SIZE);
            memcpy((void*)g_stHdcpHandler[ucPortIdx].u8RecvManageStream, pucMsg, (DEF_HDCP2_SEQ_NUM_M_SIZE + (2 * g_stHdcpHandler[ucPortIdx].u8StreamManage_k[1]) + DEF_HDCP2_K_SIZE + 1));
            //memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE]);

            g_stHdcpHandler[ucPortIdx].bRecvMsg = TRUE;
			g_stHdcpHandler[ucPortIdx].bRecvStreamManage = TRUE;

            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Recv RepAuth_Stream_Manage");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE]);
        }
        break;

        default:
        {
            HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown Message ID!!");
        }
        break;
    }

    HTRACEL(EN_HLOG_HDCP2X);
    return bEnterHandlerFlag;
}

void HDCPRx_ResetHdcp22AuthStatus(BYTE ucInputPort)
{
    g_stHdcpHandler[ucInputPort].bAuthDone = FALSE;
    g_stHdcpHandler[ucInputPort].u8MainState = 0;
    g_stHdcpHandler[ucInputPort].u8SubState  = 0;
}

BOOL HDCPRx_GetHdcp22CipherEnable(BYTE ucInputPort)
{
    BOOL u8Ret = FALSE;

    switch(ucInputPort)
    {
/*
        case COMBO_INPUT_SUPPORT_0PORT:
            u8Ret = (msReadByte(REG_DPRX_RECEIVER_70_H + (ucInputPort*0xA00) ) & 0x10) ? TRUE : FALSE;  //Encryption: reg_hdcp_link_verify_state[10:8]=b'001

            break;

        case COMBO_INPUT_SUPPORT_1PORT:
            u8Ret = (msReadByte(REG_DPRX_RECEIVER_70_H + (ucInputPort*0xA00) ) & 0x10) ? TRUE : FALSE; //Encryption: reg_hdcp_link_verify_state[10:8]=b'001

            break;

        case COMBO_INPUT_SUPPORT_2PORT:
            u8Ret = (msReadByte(REG_DPRX_RECEIVER_70_H + (ucInputPort*0xA00) ) & 0x10) ? TRUE : FALSE; //Encryption: reg_hdcp_link_verify_state[10:8]=b'001

            break;

        case COMBO_INPUT_SUPPORT_5PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P0 + 0x9C) & 0x01) ? TRUE : FALSE; // 0x1710, 0x1711, 0x1712
            break;

        case COMBO_INPUT_SUPPORT_6PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P1 + 0x9C) & 0x01) ? TRUE : FALSE; // 0x1713, 0x1714, 0x1715
            break;

        case COMBO_INPUT_SUPPORT_7PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P2 + 0x9C) & 0x01) ? TRUE : FALSE;; // 0x1718, 0x1719, 0x171A
            break;

        case COMBO_INPUT_SUPPORT_8PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P3 + 0x9C) & 0x01) ? TRUE : FALSE;; // 0x171B, 0x171C, 0x171D
            break;

        case COMBO_INPUT_SUPPORT_9PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P4 + 0x9E) & 0x01) ? TRUE : FALSE;; // 0x1726, 0x1727, 0x1728
            break;

        case COMBO_INPUT_SUPPORT_10PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P5 + 0x9E) & 0x01) ? TRUE : FALSE;; // 0x1729, 0x172A, 0x172B
            break;

        case COMBO_INPUT_SUPPORT_11PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P6 + 0x9E) & 0x01) ? TRUE : FALSE;; // 0x3013
            break;

        case COMBO_INPUT_SUPPORT_12PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P7 + 0x9E) & 0x01) ? TRUE : FALSE;; // 0x3016
            break;

        case COMBO_INPUT_SUPPORT_13PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P8 + 0x9E) & 0x01) ? TRUE : FALSE;; // 0x3019
            break;

        case COMBO_INPUT_SUPPORT_14PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P9 + 0x9E) & 0x01) ? TRUE : FALSE;; // 0x301C
            break;

        case COMBO_INPUT_SUPPORT_15PORT:
            u8Ret = (msRead2Byte(REG_HDCP_DUAL_P10 + 0x9E) & 0x01) ? TRUE : FALSE;; // 0x301F
            break;
*/
        default:

            break;
    }

    return u8Ret;

}


BOOL HDCPRx_GetEncryptionEnc(BYTE ucInputPort)
{
    WORD usHDCPEncStatus = 0;
    BOOL u8Ret = FALSE;
    UNUSED(ucInputPort);
    UNUSED(usHDCPEncStatus);
/*
    switch(ucInputPort)
    {

        case COMBO_INPUT_SUPPORT_0PORT:
            usHDCPEncStatus = msReadByte(REG_DPRX_RECEIVER_73_H + (ucInputPort*0xA00) ) & 0x07;  //Encryption: reg_hdcp_link_verify_state[10:8]=b'001

            if(usHDCPEncStatus == 0x1)
            {
                usHDCPEncStatus = BIT4;
            }
            break;

        case COMBO_INPUT_SUPPORT_1PORT:
            usHDCPEncStatus = msReadByte(REG_DPRX_RECEIVER_73_H + (ucInputPort*0xA00) ) & 0x07; //Encryption: reg_hdcp_link_verify_state[10:8]=b'001

            if(usHDCPEncStatus == 0x1)
            {
                usHDCPEncStatus = BIT4;
            }
            break;

        case COMBO_INPUT_SUPPORT_2PORT:
            usHDCPEncStatus = msReadByte(REG_DPRX_RECEIVER_73_H + (ucInputPort*0xA00) ) & 0x07; //Encryption: reg_hdcp_link_verify_state[10:8]=b'001

            if(usHDCPEncStatus == 0x1)
            {
                usHDCPEncStatus = BIT4;
            }
            break;

        case COMBO_INPUT_SUPPORT_5PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P0 + 0x02); // 0x1710, 0x1711, 0x1712
            break;

        case COMBO_INPUT_SUPPORT_6PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P1 + 0x02); // 0x1713, 0x1714, 0x1715
            break;

        case COMBO_INPUT_SUPPORT_7PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P2 + 0x02); // 0x1718, 0x1719, 0x171A
            break;

        case COMBO_INPUT_SUPPORT_8PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P3 + 0x02); // 0x171B, 0x171C, 0x171D
            break;

        case COMBO_INPUT_SUPPORT_9PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P4 + 0x02); // 0x1726, 0x1727, 0x1728
            break;

        case COMBO_INPUT_SUPPORT_10PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P5 + 0x02); // 0x1729, 0x172A, 0x172B
            break;

        case COMBO_INPUT_SUPPORT_11PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P6 + 0x02); // 0x3013
            break;

        case COMBO_INPUT_SUPPORT_12PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P7 + 0x02); // 0x3016
            break;

        case COMBO_INPUT_SUPPORT_13PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P8 + 0x02); // 0x3019
            break;

        case COMBO_INPUT_SUPPORT_14PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P9 + 0x02); // 0x301C
            break;

        case COMBO_INPUT_SUPPORT_15PORT:
            usHDCPEncStatus = msRead2Byte(REG_HDCP_DUAL_P10 + 0x02); // 0x301F
            break;

        default:

            break;
    }

    if(usHDCPEncStatus & BIT(4))
    {
        u8Ret = TRUE;
    }
*/
    return u8Ret;

}

BOOL HDCPRx_GetRi(BYTE ucInputPort, BYTE *ucRi)
{
    WORD usHDCPRi = 0;
    BOOL bRet = TRUE;

    switch(ucInputPort)
    {

/*
        case COMBO_INPUT_SUPPORT_0PORT:
            usHDCPRi = msReadByte(REG_DPRX_RECEIVER_73_H + (ucInputPort*0xA00) ) & 0x07;  //Encryption: reg_hdcp_link_verify_state[10:8]=b'001
            break;

        case COMBO_INPUT_SUPPORT_1PORT:
            usHDCPRi = msReadByte(REG_DPRX_RECEIVER_73_H + (ucInputPort*0xA00) ) & 0x07; //Encryption: reg_hdcp_link_verify_state[10:8]=b'001
            break;

        case COMBO_INPUT_SUPPORT_2PORT:
            usHDCPRi = msReadByte(REG_DPRX_RECEIVER_73_H + (ucInputPort*0xA00) ) & 0x07; //Encryption: reg_hdcp_link_verify_state[10:8]=b'001
            break;

        case COMBO_INPUT_SUPPORT_5PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P0 + 0x26); // 0x1710, 0x1711, 0x1712
            break;

        case COMBO_INPUT_SUPPORT_6PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P1 + 0x26); // 0x1713, 0x1714, 0x1715
            break;

        case COMBO_INPUT_SUPPORT_7PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P2 + 0x26); // 0x1718, 0x1719, 0x171A
            break;

        case COMBO_INPUT_SUPPORT_8PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P3 + 0x26); // 0x171B, 0x171C, 0x171D
            break;

        case COMBO_INPUT_SUPPORT_9PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P4 + 0x26); // 0x1726, 0x1727, 0x1728
            break;

        case COMBO_INPUT_SUPPORT_10PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P5 + 0x26); // 0x1729, 0x172A, 0x172B
            break;

        case COMBO_INPUT_SUPPORT_11PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P6 + 0x26); // 0x3013
            break;

        case COMBO_INPUT_SUPPORT_12PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P7 + 0x26); // 0x3016
            break;

        case COMBO_INPUT_SUPPORT_13PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P8 + 0x26); // 0x3019
            break;

        case COMBO_INPUT_SUPPORT_14PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P9 + 0x26); // 0x301C
            break;

        case COMBO_INPUT_SUPPORT_15PORT:
            usHDCPRi = msRead2Byte(REG_HDCP_DUAL_P10 + 0x26); // 0x301F
            break;
*/
        default:

            break;
    }

    memcpy((void*)ucRi, (void*) &usHDCPRi, DEF_HDCP1X_RI_SIZE);

    return bRet;

}

BYTE HDCPRx_GetAuthStatus(BYTE ucInputPort)
{
    BYTE u8Ret = EN_HDCP_STATUS_NO_AUTH;

    if(HDCPRx_GetEncryptionEnc(ucInputPort))
    {
        u8Ret = EN_HDCP_STATUS_PASS;
    }
    else
    {
        u8Ret = EN_HDCP_STATUS_NO_AUTH;
        if(g_stHdcpHandler[ucInputPort].u8HDCPRxVer == EN_HDCP_VER_22)
        {
            if((g_stHdcpHandler[ucInputPort].u8MainState == 0) && (g_stHdcpHandler[ucInputPort].u8SubState == 0))
            {
                u8Ret = EN_HDCP_STATUS_NO_AUTH;
            }
            else
            {
                if(g_stHdcpHandler[ucInputPort].bAuthDone)
                {
                    u8Ret = EN_HDCP_STATUS_NO_AUTH;
                }
                else
                {
                    u8Ret = EN_HDCP_STATUS_PROCESSING;
                }
            }
        }
    }

    //printf("Port : %02X  Auth : %02X  RxVer : %02X\r\n", ucInputPort, u8Ret, g_stHdcpHandler[ucInputPort].u8HDCPRxVer);
    //printf("usHDCPEncStatus : %02X\r\n", HDCPRx_GetEncryptionEnc(ucInputPort));
    return u8Ret;

}

BOOL HDCPRx_GetHDCPRxInfo(BYTE ucInputPort, void* pDataContent)
{
    BYTE ucBCaps = 0;
    WORD usBStatus = 0;
    BOOL bRet = FALSE;
    pST_HDCP_RX_INFO pstHDCPRxArgs = (pST_HDCP_RX_INFO)pDataContent;
    if(!g_stHdcpHandler[ucInputPort].bAsRepeater)
    {
        pstHDCPRxArgs->ucDeviceCount = 0x00;
        pstHDCPRxArgs->ucDeviceExceeded = 0x00;
        pstHDCPRxArgs->ucDepth = 0x00;
        pstHDCPRxArgs->ucDepthExceed = 0x00;
        pstHDCPRxArgs->ucHDMIMode = 0x01;
        pstHDCPRxArgs->ucRepeaterCapability = g_stHdcpHandler[ucInputPort].bAsRepeater;
        bRet = TRUE;
        return bRet;
    }

    if(g_stHdcpHandler[ucInputPort].u8HDCPRxVer == EN_HDCP_VER_22)
    {
        pstHDCPRxArgs->ucDeviceCount = (g_stHdcpHandler[ucInputPort].u8RxInfo[1] & DEF_HDCP2_MAX_DEV_EXCEEDED) ? 0x00 : g_stHdcpHandler[ucInputPort].u8DownStreamDevCnt;
        pstHDCPRxArgs->ucDeviceExceeded = (g_stHdcpHandler[ucInputPort].u8RxInfo[1] & DEF_HDCP2_MAX_DEV_EXCEEDED);
        pstHDCPRxArgs->ucDepth = (g_stHdcpHandler[ucInputPort].u8RxInfo[1] & DEF_HDCP2_MAX_DEV_EXCEEDED) ? 0x00 : ((g_stHdcpHandler[ucInputPort].u8RxInfo[0] & 0x0E) >> 1);
        pstHDCPRxArgs->ucDepthExceed = (g_stHdcpHandler[ucInputPort].u8RxInfo[1] & DEF_HDCP2_MAX_CASCADE_EXCEEDED);
        pstHDCPRxArgs->ucHDMIMode = 0x01;
        pstHDCPRxArgs->ucRepeaterCapability = g_stHdcpHandler[ucInputPort].bAsRepeater;
    }
    else if(g_stHdcpHandler[ucInputPort].u8HDCPRxVer == EN_HDCP_VER_14)
    {

        if (g_stHdcpHandler[ucInputPort].pReadX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            return bRet;
        }

        ucBCaps = g_stHdcpHandler[ucInputPort].pReadX74Func(ucInputPort, EN_HDCP1X_X74_OFFSET_BCAPS);
        usBStatus = (g_stHdcpHandler[ucInputPort].pReadX74Func(ucInputPort, (EN_HDCP1X_X74_OFFSET_BSTATUS + 1)) << 8) |g_stHdcpHandler[ucInputPort].pReadX74Func(ucInputPort, EN_HDCP1X_X74_OFFSET_BSTATUS);

        pstHDCPRxArgs->ucDeviceCount = usBStatus &BMASK(6:0);
        pstHDCPRxArgs->ucDeviceExceeded = (usBStatus &BIT(7)) >> 7;
        pstHDCPRxArgs->ucDepth = (usBStatus &BMASK(10:8)) >> 8;
        pstHDCPRxArgs->ucDepthExceed = (usBStatus &BIT(11)) >> 11;
        pstHDCPRxArgs->ucHDMIMode = (usBStatus &BIT(12)) >> 12;
        pstHDCPRxArgs->ucRepeaterCapability = (ucBCaps &BIT(6)) >> 6;
    }
    bRet = TRUE;
    return bRet;

}

BOOL HDCPRx_GetHDCP22Enc(BYTE ucInputPort)
{
    BYTE bRet = FALSE;

    if (!(HDCPRx_GetHdcp22CipherEnable(ucInputPort)) && HDCPRx_GetEncryptionEnc(ucInputPort))
    {
        g_stHdcpHandler[ucInputPort].u8HDCPRxVer = EN_HDCP_VER_14;
    }

    if(HDCPRx_GetEncryptionEnc(ucInputPort) && (g_stHdcpHandler[ucInputPort].u8HDCPRxVer == EN_HDCP_VER_22))
    {
        bRet = TRUE;
    }

    return bRet;
}

BOOL HDCPRx_RecvIDList_Set(BYTE ucInputPort, BYTE *ucRecvIDList, BYTE ucLength)
{
    BYTE bRet = FALSE;
    BYTE u8Depth = 0;
    do
    {
        if(ucRecvIDList != NULL)
        {
            // for HDCP Rx 14 Repeater
            memcpy((void*)g_stHdcpHandler[ucInputPort].u8KSVFIFO, (void*)ucRecvIDList, ucLength);
            g_stHdcpHandler[ucInputPort].u8SinkDeviceCnt = 1;
            u8Depth = 1;
            g_stHdcpHandler[ucInputPort].u8BStatus[0] = g_stHdcpHandler[ucInputPort].u8SinkDeviceCnt & 0xFF;
            g_stHdcpHandler[ucInputPort].u8BStatus[1] = u8Depth & 0x0F;

            // for HDCP Rx 22 Repeater
            memcpy((void*)g_stHdcpHandler[ucInputPort].u8RecvIDList, (void*)ucRecvIDList, ucLength);
            g_stHdcpHandler[ucInputPort].bTx2RxInfo = TRUE;
            g_stHdcpHandler[ucInputPort].bRecvIDSet = TRUE;
            bRet = TRUE;
        }

    }while(FALSE);

    return bRet;
}

MS_U32 _HDCPRx_GetTimeDiff(MS_U32 dwPreTime, MS_U32 dwPostTime)
{
	if (dwPreTime > dwPostTime)
	{
		return ((0xFFFFFFFF-dwPreTime) + dwPostTime); // patch for timer overflow case.
	}
	else
	{
		return (dwPostTime - dwPreTime);
	}
}
//#endif //#if (ENABLE_SECU_R2 )
#endif
