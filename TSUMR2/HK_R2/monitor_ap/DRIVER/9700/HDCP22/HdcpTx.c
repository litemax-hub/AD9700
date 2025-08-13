#ifndef HDCP_TX_C
#define HDCP_TX_C
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
#include "HdcpMsgPool.h"
#include "drv_Hdcp_IMI.h"//[MT9700]
//[MT9700]#include "mhal_combo.h"
#include "drvIMI.h"
//#include "timer.h"
#include "HdcpTx.h"
#include "asmCPU.h"

#if 1
//#if (ENABLE_SECU_R2 == 1) && (ENABLE_HDCP22 == 1)
//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HDCP2_TX_RETRY_CNT      32
#define DEF_HDCP2_TX_LC_RETRY_CNT   32
#define TX_PORT_OFFSET              COMBO_IP_SUPPORT_TYPE
#define GET_RECOVERY_TX_OFFSET(x)   (x-TX_PORT_OFFSET)
#define GET_RX_OFFSET(x)            (x-TX_PORT_OFFSET)
#define DEF_TX_PORT_INDEX(x)        (x+TX_PORT_OFFSET)
//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//
WORD                                g_u16TimeDiff = 0;
extern BYTE                         g_u8DpTxPortIdx;
extern WORD volatile                g_u16TimeMeasurementCounter;
extern volatile ST_HDCP_HANDLER     g_stHdcpHandler[DEF_HDCP_PORT_NUM];
extern const WORD                   g_u16Hdcp2MsgArgLen[DEF_HDCP2_CMD_NUM];
static ULONG                        g_u32AuthRetryCnt[DEF_HDCP_PORT_NUM] = {0};
static ULONG                        g_u32LCRetryCnt[DEF_HDCP_PORT_NUM] = {0};
static ST_HDCP2_SEQ_NUM_HANDLER     g_stSeqNumHandler[DEF_HDCP_PORT_NUM];
static ST_HDCP2_PAIRING_INFO        g_stStoredPairingInfo[DEF_HDCP_STORED_KM_CAPACITY];
extern volatile BYTE*               g_u8HdcpRecvBuf; //bit 7 of byte 0 is processing bit
extern volatile BYTE*               g_u8HdcpTransBuf;
static MS_U32                       g_u32HDCPPreTime[COMBO_OP_SUPPORT_TYPE];
static MS_U32                       g_u32HDCPCurTime[COMBO_OP_SUPPORT_TYPE];
BOOL                                g_u8H1xFSMReset = FALSE;
BYTE                                g_u8TxRxPortPair[COMBO_OP_SUPPORT_TYPE];
BYTE                                g_u8RxTxPortPair[COMBO_IP_SUPPORT_TYPE];
extern BOOL HDCPRx_GetEncryptionEnc(BYTE ucInputPort);

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void __________PROTOTYPE__________(void);
void __HDCP1X_Relative__(void);
BOOL HDCPTx_Hdcp1xCheckRxValid(BYTE u8PortIdx);
void HDCPTx_Hdcp1xSetState(BYTE u8PortIdx, BYTE u8MainState, BYTE u8SubState);
BOOL HDCPTx_Hdcp1xPollingHdmiMode(BYTE u8PortIdx);
//ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xSetENC_EN(BYTE u8PortIdx, BOOL bEnable);
BOOL HDCPTx_Hdcp1xCheckRevokeKey(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xReadOffset(BYTE u8PortIdx, ENUM_HDCP1X_X74_OFFSET enOffset);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xWriteOffset(BYTE u8PortIdx, ENUM_HDCP1X_X74_OFFSET enOffset);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xWriteOffset(BYTE u8PortIdx, ENUM_HDCP1X_X74_OFFSET enOffset);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xCheckBKSVAndLn(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xSetEncryptMode(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xProcessAN(BYTE u8PortIdx, BOOL bUseInternalAN, BYTE *pu8AN);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xProcessAKSV(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xGenerateCipher(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xProcessR0(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xGetR0(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xSetAuthPass(BYTE u8PortIdx);
BOOL HDCPTx_Hdcp1xRepeaterChkRdyBit(BYTE u8PortIdx);
BOOL HDCPTx_Hdcp1xRepeaterChkTopologyError(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xRepeaterChkVPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xRepeaterComputeVPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRepeater_SetKSVFIFO(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRepeater_SetReadyBit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRepeater_SetBstatus(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPRepeater_WriteVPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_CopyInfo2Rx(BYTE u8PortIdx);
void HDCPRepeater_ReStart14TxAuth(BYTE u8PortIdx);
BOOL HDCPRepeater_TxSetRxRdyBit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xFSM(BYTE u8PortIdx);
void __HDCP2X_Relative__(void);
void HDCPTx_Hdcp2SetState(BYTE u8PortIdx, BYTE u8MainState, BYTE u8SubState);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2Init(BYTE u8PortIdx);
BOOL HDCPTx_Hdcp2CheckSinkCapability(BYTE u8PortIdx);
//ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2EnableAuth(BYTE u8PortIdx, BOOL bEnable);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2SendMsg(BYTE u8PortIdx, ENUM_HDCP2_MSG_LIST enMsg);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessAKEInit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessLCInit(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessSKESendEks(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessRepAuthStreamManage(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvAKESendCert(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvAKESendHPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvAKESendPairing(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvLCSendLPrime(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvRepAuthSendRecvIDList(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvRepAuthStreamReady(BYTE u8PortIdx);
ENUM_HDCP2_SEQNUM_V_STATUS HDCPTx_Hdcp2CheckSeqNumV(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2FSM(BYTE u8PortIdx);
void HDCPTx_Hdcp2MsgParser(BYTE u8PortIdx, BYTE *pucMsg);
ENUM_HDCP_ERR_CODE HDCPTx_Handler(BYTE u8PortIdx);
MS_U32 _HDCPTx_GetTimeDiff(MS_U32 dwPreTime, MS_U32 dwPostTime);
ENUM_HDCP_ERR_CODE HDCPTx_HdcpEnable_ENC(BYTE u8PortIdx, BOOL bEnable, BYTE u8Version);
void __________FUNCTION__________(void);

//----------------------------------------------------------------------//
// function
//----------------------------------------------------------------------//

void __HDCP1X_Relative__(void)
{
    //pesudo function
}

void HDCPTx_Hdcp1xSetState(BYTE u8PortIdx, BYTE u8MainState, BYTE u8SubState)
{
    g_stHdcpHandler[u8PortIdx].u8MainState = u8MainState;
    g_stHdcpHandler[u8PortIdx].u8SubState = u8SubState;
}

BOOL HDCPTx_Hdcp1xPollingHdmiMode(BYTE u8PortIdx)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (u8PortIdx < g_u8DpTxPortIdx)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Invalid Port Index!");
            break;
        }

        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), (BYTE)EN_HDCP1X_X74_OFFSET_BSTATUS, 0x00, (BYTE)EN_HDCP_OPCODE_READ, DEF_HDCP1X_BSTATUS_SIZE, (BYTE*)g_stHdcpHandler[u8PortIdx].u8BStatus) == FALSE)
        {
            bRet = FALSE;
            break;
        }
        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}
#if 0
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xSetENC_EN(BYTE u8PortIdx, BOOL bEnable)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE* u8SendData = (BYTE*)IMIGetAddr(DEF_IMI_ENAUTH_ADDR + 0x2);
    HTRACEE(EN_HLOG_HDCP);
    do
    {
        *(BYTE*)(u8SendData) = u8PortIdx;
        *(BYTE*)(u8SendData + 1) = bEnable;

        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_ENABLE_ENCEN, EN_HDCPMBX_CMD_HOST_HKR2, 2, (ULONG)u8SendData) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }
    } while(FALSE);

    g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}
#endif
BOOL HDCPTx_Hdcp1xCheckRevokeKey(BYTE u8PortIdx)
{
    BOOL bRet = FALSE;
    BYTE u8ByteCnt = 0;
    BYTE u8ByteCntOuter = 0;
    BYTE u8Revokedksv[2][DEF_HDCP1X_KSV_SIZE] = {{0x23, 0xDE, 0x5C, 0x43, 0x93}, {0x0B, 0x37, 0x21, 0xB4, 0x7D}};
    // The revoked key from QD882: Bksv=0x23, 0xde, 0x5c, 0x43, 0x93
    // The revoked key from SL8800: Bksv=0x0b, 0x37, 0x21, 0xb4, 0x7d

    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (u8PortIdx < g_u8DpTxPortIdx)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Invalid Port Index!");
            break;
        }
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)(g_stHdcpHandler[u8PortIdx].u8BKSV), 5);

        //temp solution: if we got SRM list, we should check whole list instead of only 1 set ksv;
        for( u8ByteCntOuter = 0; u8ByteCntOuter < 2; u8ByteCntOuter++)
        {
            bRet = TRUE;
            for ( u8ByteCnt = 0; u8ByteCnt < DEF_HDCP1X_KSV_SIZE; u8ByteCnt++ )
            {
                if (g_stHdcpHandler[u8PortIdx].u8BKSV[u8ByteCnt] != u8Revokedksv[u8ByteCntOuter][u8ByteCnt])
                {
                    bRet = FALSE;
                    break;
                }
            }
            if(u8ByteCnt == DEF_HDCP1X_KSV_SIZE)
            {
                HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "BKSV Revoked Keys");
                break;
            }
        }

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);
    return bRet;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xReadOffset(BYTE u8PortIdx, ENUM_HDCP1X_X74_OFFSET enOffset)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

        HLOGDC(EN_HLOG_HDCP, DEF_COLOR_YELLOW, "--> Read Offset 0x%X", enOffset);

        switch (enOffset)
        {
            case EN_HDCP1X_X74_OFFSET_BKSV:
            {
                if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), EN_HDCP1X_X74_OFFSET_BKSV, 0x00, EN_HDCP_OPCODE_READ,\
                    DEF_HDCP1X_KSV_SIZE, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
                {
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Access BKSV Failed!");
                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                }
            }
            break;

            case EN_HDCP1X_X74_OFFSET_RIPRIME:
            {
                if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), EN_HDCP1X_X74_OFFSET_RIPRIME, 0x00, EN_HDCP_OPCODE_READ,\
                    DEF_HDCP1X_RI_SIZE, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
                {
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Access Ri Failed!");
                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                    break;
                }
            }
            break;

            case EN_HDCP1X_X74_OFFSET_PJPRIME:
            {
            }
            break;

            case EN_HDCP1X_X74_OFFSET_VPRIME_H0:
            {
                BYTE u8StepCnt = 0;

                for ( u8StepCnt = 0; u8StepCnt < 5; u8StepCnt++ )
                {
                    if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), EN_HDCP1X_X74_OFFSET_VPRIME_H0 + 4*u8StepCnt, 0x00, EN_HDCP_OPCODE_READ,\
                        (DEF_HDCP1X_V_SIZE/5), (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 4*u8StepCnt)) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Access V'.H %d Failed!", u8StepCnt);
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }
                }

                if(enErrCode != EN_HDCP_ERR_PROCESS_FAIL)
                {
                    memcpy((void*)g_stHdcpHandler[u8PortIdx].u8VPrime,(void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), DEF_HDCP1X_V_SIZE);
                }

            }
            break;

            case EN_HDCP1X_X74_OFFSET_BCAPS:
            {
                if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), EN_HDCP1X_X74_OFFSET_BCAPS, 0x00, EN_HDCP_OPCODE_READ,\
                    DEF_HDCP1X_BCAPS_SIZE, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
                {
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Access BCaps Failed!");
                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                }
                else
                {
                    memcpy((void*)&g_stHdcpHandler[u8PortIdx].u8BCaps, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), DEF_HDCP1X_BCAPS_SIZE);
                }
            }
            break;

            case EN_HDCP1X_X74_OFFSET_BSTATUS:
            {
                if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), EN_HDCP1X_X74_OFFSET_BSTATUS, 0x00, EN_HDCP_OPCODE_READ,\
                    DEF_HDCP1X_BSTATUS_SIZE, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
                {
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Access BStatus Failed!");
                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                }
                else
                {
                    memcpy((void*)g_stHdcpHandler[u8PortIdx].u8BStatus, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), DEF_HDCP1X_BSTATUS_SIZE);
                }
            }
            break;

            case EN_HDCP1X_X74_OFFSET_KSVFIFO:
            {
                {
                    if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), EN_HDCP1X_X74_OFFSET_KSVFIFO, 0x00, EN_HDCP_OPCODE_READ,\
                        g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt*5, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Access KSVFIFO Failed!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                    }
                    else
                    {
                        memcpy((void*)(g_stHdcpHandler[u8PortIdx].u8KSVFIFO), (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt*5);
                    }
                }

            }
            break;

            case EN_HDCP1X_X74_OFFSET_DBG:
            {
            }
            break;

            default:
            {
                enErrCode = EN_HDCP_ERR_INVALID_ARG;
                HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Invalid HDCP1.x Offset Address!");
            }
            break;

        }

        if (enErrCode != EN_HDCP_ERR_NONE)
        {
            break;
        }

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xWriteOffset(BYTE u8PortIdx, ENUM_HDCP1X_X74_OFFSET enOffset)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8Offset = 0;
    WORD u16DataLen = 0;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

        HLOGDC(EN_HLOG_HDCP, DEF_COLOR_YELLOW, "--> Write Offset 0x%X", enOffset);

        switch (enOffset)
        {
            case EN_HDCP1X_X74_OFFSET_AKSV:
            {
                u8Offset = (BYTE)EN_HDCP1X_X74_OFFSET_AKSV;
                u16DataLen = DEF_HDCP1X_KSV_SIZE;
            }
            break;

            case EN_HDCP1X_X74_OFFSET_AINFO:
            {
                u8Offset = (BYTE)EN_HDCP1X_X74_OFFSET_AINFO;
                u16DataLen = DEF_HDCP1X_AINFO_SIZE;
            }
            break;

            case EN_HDCP1X_X74_OFFSET_AN:
            {
                u8Offset = (BYTE)EN_HDCP1X_X74_OFFSET_AN;
                u16DataLen = DEF_HDCP1X_AN_SIZE;
                HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1), u16DataLen);
                g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), u8Offset, 0x00, EN_HDCP_OPCODE_WRITE, u16DataLen, (BYTE*)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1));
            }
            return enErrCode;

            case EN_HDCP1X_X74_OFFSET_DBG:
            {
                u8Offset = (BYTE)EN_HDCP1X_X74_OFFSET_AN;
                u16DataLen = DEF_HDCP1X_DBG_SIZE;
            }
            break;

            default:
            {
                u8Offset = 0;
                u16DataLen = 0;
                enErrCode = EN_HDCP_ERR_INVALID_ARG;
                HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Invalid HDCP1.x Offset Address!");
            }
            break;

        }

        if (enErrCode != EN_HDCP_ERR_NONE)
        {
            break;
        }


        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), u16DataLen);
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), u8Offset, 0x00, EN_HDCP_OPCODE_WRITE, u16DataLen, (BYTE*)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX));
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

BOOL HDCPTx_Hdcp1xCheckRxValid(BYTE u8PortIdx)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (u8PortIdx < g_u8DpTxPortIdx)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Invalid Port Index!");
            break;
        }

        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_BCAPS) != EN_HDCP_ERR_NONE)
        {
            bRet = FALSE;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "HDCPTx_Hdcp1xCheckRxValid Fail!");
            break;
        }

        memcpy((void*)&g_stHdcpHandler[u8PortIdx].u8BCaps, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), DEF_HDCP1X_BCAPS_SIZE);
        bRet = (g_stHdcpHandler[u8PortIdx].u8BCaps == 0xFF)? FALSE : TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xCheckBKSVAndLn(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8ByteCnt = 0x00;
    BYTE u8BitCnt = 0x00;
    BYTE u8KsvOneCnt = 0x00;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_BKSV) != EN_HDCP_ERR_NONE)
        {
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read BKSV Fail!");
            break;
        }

        memcpy((void*)&g_stHdcpHandler[u8PortIdx].u8BKSV, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), DEF_HDCP1X_KSV_SIZE);

        for ( u8ByteCnt = 0; u8ByteCnt < DEF_HDCP1X_KSV_SIZE; u8ByteCnt++ )
        {
            for ( u8BitCnt = 0; u8BitCnt < 0x08; u8BitCnt++ )
            {
                u8KsvOneCnt += ((g_stHdcpHandler[u8PortIdx].u8BKSV[u8ByteCnt] >> u8BitCnt) & 0x01);
            }
        }
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)g_stHdcpHandler[u8PortIdx].u8BKSV, DEF_HDCP1X_KSV_SIZE);

        if (u8KsvOneCnt != ((8 * DEF_HDCP1X_KSV_SIZE) >> 1))
        {
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Invalid BKSV!");
            break;
        }

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xSetEncryptMode(BYTE u8PortIdx)
{
#define DEF_HDCP1XTX_SINK_IS_REPEATER   (0x01 << 1)
#define DEF_HDCP1XTX_EESS_MODE          (0x01 << 2)
#define DEF_HDCP1XTX_ADV_CIPHER         (0x01 << 3)

    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8EncMode = 0;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_BCAPS) != EN_HDCP_ERR_NONE)
        {
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read BCaps Fail!");
            break;
        }

        memcpy((void*)&g_stHdcpHandler[u8PortIdx].u8BCaps, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), DEF_HDCP1X_BCAPS_SIZE);

        //TBD: HDMI mode and DP
        //if HDMI mode
        #if 0
        if (u8PortIdx >= g_u8HdmiTxPortIdx)
        {
            u8EncMode = (DEF_HDCP1XTX_ADV_CIPHER | DEF_HDCP1XTX_EESS_MODE);
        }
		#endif

        if (g_stHdcpHandler[u8PortIdx].u8BCaps & (0x01 << 1))
        {
            u8EncMode = (DEF_HDCP1XTX_ADV_CIPHER | DEF_HDCP1XTX_EESS_MODE);

            //write AInfo
            *(BYTE*)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) = 0x02; //ENABLE_1.1_FEATURE
            if (HDCPTx_Hdcp1xWriteOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_AINFO) != EN_HDCP_ERR_NONE)
            {
                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Write AINFO Fail!");
                break;
            }
        }

        //check repeater
        g_stHdcpHandler[u8PortIdx].bSinkIsRepeater = (g_stHdcpHandler[u8PortIdx].u8BCaps & (1<<6)) ? TRUE : FALSE;

        if (g_stHdcpHandler[u8PortIdx].bSinkIsRepeater == TRUE)
        {
            u8EncMode |= DEF_HDCP1XTX_SINK_IS_REPEATER;

            if(u8PortIdx >= g_u8DpTxPortIdx)
            {
                *(BYTE*)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) = 0x01; //REAUTHENTICATION_ENABLE_IRQ_HPD for DP repeater

                if (HDCPTx_Hdcp1xWriteOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_AINFO) != EN_HDCP_ERR_NONE)
                {
                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Write AINFO Fail!");
                    break;
                }
            }
        }
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), (void*)&u8EncMode, 1);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_CONFIG_MODE, EN_HDCPMBX_CMD_HOST_HKR2,\
            1, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

#undef DEF_HDCP1XTX_SINK_IS_REPEATER
#undef DEF_HDCP1XTX_EESS_MODE
#undef DEF_HDCP1XTX_ADV_CIPHER

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xProcessAN(BYTE u8PortIdx, BOOL bUseInternalAN, BYTE *pu8AN)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        *(BYTE*)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) = bUseInternalAN;

        if (bUseInternalAN == TRUE)
        {
            if (pu8AN == NULL)
            {
                HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL external AN!");
                break;
            }
            else
            {
                memcpy((void*)((g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) + 1), pu8AN, DEF_HDCP1X_AN_SIZE);
            }
        }

        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_PROCESS_AN, EN_HDCPMBX_CMD_HOST_HKR2,\
            (bUseInternalAN ? (1 + DEF_HDCP1X_AN_SIZE) : 1), (ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX))== FALSE)
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

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xProcessAKSV(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        //if (HDCPMBX_SetCommonCmd(EN_MBXCMD_HDCP1_TX_GET_AKSV, 0, g_stHdcpHandler[u8PortIdx].u8AKSV) == FALSE)
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_GET_AKSV, EN_HDCPMBX_CMD_HOST_HKR2,\
            DEF_HDCP1X_KSV_SIZE, (ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
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

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xGenerateCipher(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_GENERATE_CIPHER, EN_HDCPMBX_CMD_HOST_HKR2,\
            DEF_HDCP1X_KSV_SIZE, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
        {
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xProcessR0(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_PROCESS_R0, EN_HDCPMBX_CMD_HOST_HKR2,\
            0, (ULONG)NULL) == FALSE)
        {
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xGetR0(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_GET_R0, EN_HDCPMBX_CMD_HOST_HKR2,\
            0, (ULONG)NULL) == FALSE)
        {
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}


ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xSetAuthPass(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {

        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_SET_AUTH_PASS, EN_HDCPMBX_CMD_HOST_HKR2,\
            0, (ULONG)NULL) == FALSE)
        {
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

BOOL HDCPTx_Hdcp1xRepeaterChkRdyBit(BYTE u8PortIdx)
{
    BOOL bRet = FALSE;
    static DWORD u8Cnt = 0;
    HTRACEE(EN_HLOG_HDCP);
    do
    {
        u8Cnt++;
        if(u8Cnt %50 != 0 ) //CTS for SL8800 Access slower
        {
            break;
        }
        u8Cnt = 1;

        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_BCAPS) != EN_HDCP_ERR_NONE)
        {
            bRet = FALSE;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read BCaps Fail!");
            break;
        }
        //printf("BCaps : %02X\r\n",g_stHdcpHandler[u8PortIdx].u8BCaps);
        bRet =(g_stHdcpHandler[u8PortIdx].u8BCaps & (1 << 5)) ? TRUE : FALSE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}

BOOL HDCPTx_Hdcp1xRepeaterChkTopologyError(BYTE u8PortIdx)
{
    BOOL bRet = FALSE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {

        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_BSTATUS) != EN_HDCP_ERR_NONE)
        {
            bRet = FALSE;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read BStauts Fail!");
            break;
        }
        //printf("BStatus : 0x%02X  0x%02X\r\n",g_stHdcpHandler[u8PortIdx].u8BStatus[0],g_stHdcpHandler[u8PortIdx].u8BStatus[1]);

        if ((g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0x80) || (g_stHdcpHandler[u8PortIdx].u8BStatus[1] & 0x08)) // "MAX_DECS_EXCEEDED" or "MAX_CASCADE_EXCEEDED"
        {
            bRet = FALSE;
            g_stHdcpHandler[u8PortIdx].bTopologyErr = TRUE;
            HDCPRepeater_SetBstatus(u8PortIdx);
            HDCPRepeater_SetReadyBit(u8PortIdx);
            //HDCPTx_Hdcp1xSetENC_EN(u8PortIdx,FALSE);
            HLOGIC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_CYAN, "MAX_DECS_EXCEEDED or MAX_CASCADE_EXCEEDED!!");
            break;
        }
#if 0
        if(g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0x7F)  // "MAX_DECS_EXCEEDED"
        {
            bRet = FALSE;
            g_stHdcpHandler[u8PortIdx].bTopologyErr = TRUE;
            HDCPRepeater_SetBstatus(u8PortIdx);
            HDCPTx_Hdcp1xSetENC_EN(u8PortIdx,FALSE);
            HLOGIC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_CYAN, "MAX_DECS_EXCEEDED!!");
            break;
        }

        if(g_stHdcpHandler[u8PortIdx].u8BStatus[1] & 0x07)  // "MAX_CASCADE_EXCEEDED"
        {
            bRet = FALSE;
            g_stHdcpHandler[u8PortIdx].bTopologyErr = TRUE;
            HDCPRepeater_SetBstatus(u8PortIdx);
            HDCPTx_Hdcp1xSetENC_EN(u8PortIdx,FALSE);
            HLOGIC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_CYAN, "MAX_CASCADE_EXCEEDED!!");
            break;
        }
#endif
        g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt = (g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0x7F);

        if (g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt == 0)
        {
            HLOGIC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_CYAN, "Device is 0!!");
            break;
        }
        bRet = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xRepeaterChkVPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), (void*)&g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt, 1);
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1), (void*)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, (g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt * 5));
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1 + (g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt * 5)), (void*)g_stHdcpHandler[u8PortIdx].u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1 + (g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt * 5) + DEF_HDCP1X_BSTATUS_SIZE), (void*)g_stHdcpHandler[u8PortIdx].u8VPrime, DEF_HDCP1X_V_SIZE);
        //printf("V Prime : \r\n");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)g_stHdcpHandler[u8PortIdx].u8VPrime, DEF_HDCP1X_V_SIZE);
        //printf("KSVLIST : \r\n");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, (g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt * 5));
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_CHECK_VPRIME, EN_HDCPMBX_CMD_HOST_HKR2,\
            ((g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt * 5) + DEF_HDCP1X_KSV_FIFO_SIZE + DEF_HDCP1X_BSTATUS_SIZE + DEF_HDCP1X_V_SIZE) , (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
        {
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;

}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xRepeaterComputeVPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8DeviceKVCnt = g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt+1;
    BYTE u8KSVList[u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE];

    HTRACEE(EN_HLOG_HDCP);

    do
    {
        memcpy((void*)u8KSVList, (void*)g_stHdcpHandler[u8PortIdx].u8BKSV, DEF_HDCP1X_KSV_SIZE);
        memcpy((void*)(u8KSVList + DEF_HDCP1X_KSV_SIZE), (void*)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt*DEF_HDCP1X_KSV_SIZE);
        memcpy((void*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8KSVFIFO, u8KSVList, u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE);
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX), (void*)&u8DeviceKVCnt, 1);
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1), u8KSVList, (u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE));
        memcpy((void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX + 1 + (u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE)), (void*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8KSVFIFO, (u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE));
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP1_TX_COMPUTE_H1XRX_VPRIME, EN_HDCPMBX_CMD_HOST_HKR2,\
            ((u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE) + DEF_HDCP1X_KSV_FIFO_SIZE + DEF_HDCP1X_BSTATUS_SIZE) , (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) == FALSE)
        {
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;

}

ENUM_HDCP_ERR_CODE HDCPRepeater_SetKSVFIFO(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8DeviceKVCnt = g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt+1;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pKSVFIFOFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pKSVFIFOFunc(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], u8DeviceKVCnt*DEF_HDCP1X_KSV_SIZE, (BYTE*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8KSVFIFO);
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8KSVFIFO, (u8DeviceKVCnt)*DEF_HDCP1X_KSV_SIZE);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRepeater_WriteVPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pWriteX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        memcpy((void*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8VPrime, (BYTE*)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)), DEF_HDCP1X_V_SIZE);
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pWriteX74Func(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], 0x20, DEF_HDCP1X_V_SIZE, (BYTE*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8VPrime);
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8VPrime, DEF_HDCP1X_V_SIZE);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRepeater_SetBstatus(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8NewBStatus[DEF_HDCP1X_BSTATUS_SIZE];
    HTRACEE(EN_HLOG_HDCP);
     do
    {
        if (g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pWriteX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_BSTATUS) != EN_HDCP_ERR_NONE)
        {
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read BStauts Fail!");
            break;
        }
        g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt = (g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0x7F);

        u8NewBStatus[0] = ((g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0xFF) == 0xFF) ? 0xFF : ((g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0xFF) + 1);
        u8NewBStatus[1] = ((g_stHdcpHandler[u8PortIdx].u8BStatus[1] & 0x0F) == 0x0F) ? 0x0F : ((g_stHdcpHandler[u8PortIdx].u8BStatus[1] & 0x0F) + 1);

		#if 0
        if(u8PortIdx >= HDMITX_PORT_OFFSET)
        {
            u8NewBStatus[1] = u8NewBStatus[1]|(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pReadX74Func(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], 0x42)&0xF0);
        }
		#endif

        memcpy((void*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BStatus, u8NewBStatus, DEF_HDCP1X_BSTATUS_SIZE);
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pWriteX74Func(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], EN_HDCP1X_X74_OFFSET_BSTATUS, DEF_HDCP1X_BSTATUS_SIZE, (BYTE*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BStatus);
        //printf("TxSetRxBStatus\r\n");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPRepeater_SetReadyBit(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP);

    do
    {
        if (g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pWriteX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        if (g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pReadX74Func == NULL)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }
        g_stHdcpHandler[u8PortIdx].bRdyBitSet = TRUE;
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].bPrepare2SetRdyBit = FALSE;
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BCaps = g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pReadX74Func(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], EN_HDCP1X_X74_OFFSET_BCAPS)|0x20;
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pWriteX74Func(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], EN_HDCP1X_X74_OFFSET_BCAPS, DEF_HDCP1X_BCAPS_SIZE, (BYTE*)&g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BCaps);
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pWriteX74Func(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], EN_HDCP1X_X74_OFFSET_BSTATUS, DEF_HDCP1X_BSTATUS_SIZE, (BYTE*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BStatus);
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_CopyInfo2Rx(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE u8NewBStatus[DEF_HDCP1X_BSTATUS_SIZE];
    BYTE u8DeviceKVCnt = 0;
    BYTE u8Depth;
    BYTE u8KSVList[DEF_HDCP1X_MAX_KSV_SIZE * DEF_HDCP1X_KSV_SIZE];
    HTRACEE(EN_HLOG_HDCP);
     do
    {
        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_BSTATUS) != EN_HDCP_ERR_NONE)
        {
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read BStauts Fail!");
            break;
        }
        if(g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_14)  // Tx HDCP 14
        {
            g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt = (g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0x7F);
            g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8SinkDeviceCnt  = g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt+2;
            u8DeviceKVCnt = g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8SinkDeviceCnt;

            u8NewBStatus[0] = ((g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0xFF) == 0xFF) ? 0xFF : ((g_stHdcpHandler[u8PortIdx].u8BStatus[0] & 0xFF) + 2);
            u8NewBStatus[1] = ((g_stHdcpHandler[u8PortIdx].u8BStatus[1] & 0x0F) == 0x0F) ? 0x0F : ((g_stHdcpHandler[u8PortIdx].u8BStatus[1] & 0x0F) + 1);
            memcpy((void*)u8KSVList, (void*)g_stHdcpHandler[u8PortIdx].u8BKSV, DEF_HDCP1X_KSV_SIZE);
            memcpy((void*)(u8KSVList + DEF_HDCP1X_KSV_SIZE), g_stHdcpHandler[u8PortIdx].u8BKSV, DEF_HDCP1X_KSV_SIZE);
            memcpy((void*)(u8KSVList + DEF_HDCP1X_KSV_SIZE + DEF_HDCP1X_KSV_SIZE), (void*)g_stHdcpHandler[u8PortIdx].u8KSVFIFO, g_stHdcpHandler[u8PortIdx].u8SinkDeviceCnt*DEF_HDCP1X_KSV_SIZE);

        }
        else if(g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_22) // Tx HDCP 22
        {
            u8DeviceKVCnt = g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt + 1;
            u8Depth = ((g_stHdcpHandler[u8PortIdx].u8RxInfo[0] & 0x0E) >> 1) + 1;
            if(u8DeviceKVCnt > DEF_HDCP2_MAX_DEVICE_COUNT)
            {
                u8NewBStatus[0] = (g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt & 0x7F) | 0x80;
            }
            else
            {
                u8NewBStatus[0] = u8DeviceKVCnt;
            }
            g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8SinkDeviceCnt = u8DeviceKVCnt;
            if(u8Depth > DEF_HDCP2_MAX_DEPTH_LEVEL)
            {
                u8NewBStatus[1] = ((u8Depth - 1) & 0x07) | 0x08;
            }
            else
            {
                u8NewBStatus[1] = u8Depth;
            }
            memcpy((void*)u8KSVList, (void*)g_stHdcpHandler[u8PortIdx].u8RecvID, DEF_HDCP1X_KSV_SIZE);
            memcpy((void*)(u8KSVList + DEF_HDCP1X_KSV_SIZE), (void*)g_stHdcpHandler[u8PortIdx].u8RecvIDList, g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP1X_KSV_SIZE);
        }

		#if 0
        if(u8PortIdx >= HDMITX_PORT_OFFSET)
        {
            u8NewBStatus[1] = u8NewBStatus[1]|(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pReadX74Func(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], 0x42)&0xF0);
        }
		#endif

        memcpy((void*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BStatus, u8NewBStatus, DEF_HDCP1X_BSTATUS_SIZE);
        memcpy((void*)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8KSVFIFO, u8KSVList, u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE);
        //printf("SetRxBStatus : \r\n");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8BStatus, DEF_HDCP1X_BSTATUS_SIZE);
        //printf("SetRxKSVFIFO : \r\n");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP, (unsigned char *)(ULONG)g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8KSVFIFO, (u8DeviceKVCnt * DEF_HDCP1X_KSV_SIZE));

        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].bTx2RxInfo = TRUE;
        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].bPolling = FALSE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

void HDCPRepeater_ReStart14TxAuth(BYTE u8PortIdx)
{
    HTRACEE(EN_HLOG_HDCP);
    g_stHdcpHandler[u8PortIdx].u8HDCPRxVer = EN_HDCP_VER_14;
    g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2RX_MS_B0;
    g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
    if(g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].bAsRepeater)
    {
        g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].bRxHDCPStart = TRUE;
        HDCPTx_Hdcp2SetState(g_u8RxTxPortPair[u8PortIdx], EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
        HDCPTx_Hdcp2FSM(g_u8RxTxPortPair[u8PortIdx]);   // reset 22 cause bRdyBitSet only set in 14 repeater
        if(g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].bRdyBitSet)
        {
            if(g_stHdcpHandler[g_u8RxTxPortPair[u8PortIdx]].enHdcpVerInfo == EN_HDCP_VER_14)
            {
                HDCPTx_Hdcp1xSetState(g_u8RxTxPortPair[u8PortIdx], EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                HDCPTx_Hdcp1xFSM(g_u8RxTxPortPair[u8PortIdx]);
            }
        }
        g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit = FALSE;
    }
    g_stHdcpHandler[u8PortIdx].bAuthDone = FALSE;
    HTRACEL(EN_HLOG_HDCP);
}

BOOL HDCPRepeater_TxSetRxRdyBit(BYTE u8PortIdx)
{
    HTRACEE(EN_HLOG_HDCP);
    g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit = TRUE;
    HTRACEL(EN_HLOG_HDCP);
    return TRUE;
}
#if 0
void HDCPTx_SetTxMode(BYTE u8OutputPort, BYTE u8Mode)
{
    g_stHdcpHandler[DEF_TX_PORT_INDEX(u8OutputPort)].enHdcpVerCtrl = u8Mode;
    if(u8Mode == EN_HDCP_VER_14)
    {
        HDCPTx_Hdcp1xSetState(DEF_TX_PORT_INDEX(u8OutputPort), EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
        HDCPTx_Hdcp2EnableAuth(DEF_TX_PORT_INDEX(u8OutputPort), FALSE);
    }
    else if(u8Mode == EN_HDCP_REFLECT_22)
    {
        if(g_stHdcpHandler[DEF_TX_PORT_INDEX(u8OutputPort)].bAsRepeater)
        {
            if(g_stHdcpHandler[g_u8TxRxPortPair[u8OutputPort]].u8HDCPRxVer == EN_HDCP_VER_22)
            {
                HDCPTx_Hdcp2SetState(DEF_TX_PORT_INDEX(u8OutputPort), EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //reset HDCP22 State
                g_stHdcpHandler[DEF_TX_PORT_INDEX(u8OutputPort)].bSendAKEInit = FALSE;
                HDCPTx_Hdcp1xSetENC_EN(DEF_TX_PORT_INDEX(u8OutputPort), FALSE);
            }
        }
    }
    else if(u8Mode == EN_HDCP_VER_FREERUN)
    {
        if(g_stHdcpHandler[DEF_TX_PORT_INDEX(u8OutputPort)].enHdcpVerInfo == EN_HDCP_VER_22)
        {
            HDCPTx_Hdcp2SetState(DEF_TX_PORT_INDEX(u8OutputPort), EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //reset HDCP22 State
            HDCPTx_Hdcp1xSetENC_EN(DEF_TX_PORT_INDEX(u8OutputPort), FALSE);
            g_stHdcpHandler[DEF_TX_PORT_INDEX(u8OutputPort)].bSendAKEInit = FALSE;
        }
        else if(g_stHdcpHandler[DEF_TX_PORT_INDEX(u8OutputPort)].enHdcpVerInfo == EN_HDCP_VER_14)
        {
            HDCPTx_Hdcp1xSetState(DEF_TX_PORT_INDEX(u8OutputPort), EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE); //reset HDCP14 State
            HDCPTx_Hdcp2EnableAuth(DEF_TX_PORT_INDEX(u8OutputPort), FALSE);
        }
    }
}
#endif
void HDCPRepeater_ConfigRepeaterPair(BYTE u8InputPort, BYTE u8OutputPort, Bool bEnableFlag)
{
    g_stHdcpHandler[DEF_TX_PORT_INDEX(u8OutputPort)].bAsRepeater = bEnableFlag;
    g_stHdcpHandler[u8InputPort].bAsRepeater = bEnableFlag;
    g_u8TxRxPortPair[u8OutputPort] = u8InputPort;
    g_u8RxTxPortPair[u8InputPort] = DEF_TX_PORT_INDEX(u8OutputPort);
    //printf("Set Repeater Pair Repeater : %02X, Pair InputPort %02X OutputPort %02X\r\n", bEnableFlag, g_u8TxRxPortPair[u8OutputPort], DEF_TX_PORT_INDEX(u8OutputPort));
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xFSM(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    ENUM_HDCPMBX_STATE_LIST enState = EN_HDCPMBX_STATE_IDLE;
    MS_U16 u16SinkRi,u16SourceRi;
    static MS_U8 u8DelayCnt = 0;
    //HTRACEE(EN_HLOG_HDCP);

    if (g_u8H1xFSMReset == TRUE)
    {
        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_AUTH_START);
    }

    switch (g_stHdcpHandler[u8PortIdx].u8MainState)
	{
        case EN_HDCP14TX_MS_A0:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP14TX_SS_IDLE:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == TRUE)
                    {
                        if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                        {
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Get State Fail!");
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

                    }
                    g_u8H1xFSMReset = TRUE;
                }
                break;
                case EN_HDCP14TX_SS_AUTH_START:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    {
                        g_u8H1xFSMReset = FALSE;
                        //reset variable;
                        g_stHdcpHandler[u8PortIdx].bTopologyErr = FALSE;
                        g_stHdcpHandler[u8PortIdx].u8BCaps =  0x00;
                        memset((void*)g_stHdcpHandler[u8PortIdx].u8AKSV, 0x00, DEF_HDCP1X_KSV_SIZE);
                        memset((void*)g_stHdcpHandler[u8PortIdx].u8BKSV, 0x00, DEF_HDCP1X_KSV_SIZE);
                        memset((void*)g_stHdcpHandler[u8PortIdx].u8BStatus, 0x00, DEF_HDCP1X_BSTATUS_SIZE);
                        //check if hdmi mode
                        //disable ENC_EN
                        //HDCPTx_Hdcp1xSetENC_EN(u8PortIdx, FALSE);
                    }
                    else
                    {
                        if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                        {
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Get State Fail!");
                            break;
                        }

                        if (enState != EN_HDCPMBX_STATE_DONE)
                        {
                            HLOGV(EN_HLOG_HDCP, "Command NOT Done");
                            break;
                        }

                        g_stHdcpHandler[u8PortIdx].bRdyBitSet = FALSE;  //clear RdyBitFlag
                        g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].bTx2RxInfo = FALSE;

                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                        if (HDCPTx_Hdcp1xPollingHdmiMode(u8PortIdx) == FALSE)
                        {
                            HLOGIC(EN_HLOG_HDCP, DEF_COLOR_YELLOW, "Polling HDMI Mode Failed");
                            break;
                        }

                        HLOGDC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_CYAN, "HDMI Mode Set");
                        if(u8DelayCnt<25)   // avoid encrypted frame to make TE misunderstand
                        {
                            u8DelayCnt++;
                            break;
                        }
                        u8DelayCnt = 0;

                        if (HDCPTx_Hdcp1xCheckRxValid(u8PortIdx) == TRUE)
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_PROCESS_AN);
                            g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        }
                        else
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        }
                    }

                }
                break;

                case EN_HDCP14TX_SS_PROCESS_AN:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    {
                        //process AN
                        if (HDCPTx_Hdcp1xProcessAN(u8PortIdx, FALSE, NULL) != EN_HDCP_ERR_NONE)
                        {
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Process AN Failed!");
                            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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

                        if (HDCPTx_Hdcp1xWriteOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_AN) != EN_HDCP_ERR_NONE)
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Write AN Fail!");
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }

                        if (HDCPTx_Hdcp1xProcessAKSV(u8PortIdx) != EN_HDCP_ERR_NONE)
                        {
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Process AKSV Failed!");
                            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            break;
                        }
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_EXCHANGE_KSV);
                    }
                }
                break;

                case EN_HDCP14TX_SS_EXCHANGE_KSV:
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
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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

                    if (HDCPTx_Hdcp1xWriteOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_AKSV) != EN_HDCP_ERR_NONE)
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Write AKSV Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    if (HDCPTx_Hdcp1xSetEncryptMode(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A1A2, EN_HDCP14TX_SS_EXCHANGE_KSV);
                }
                break;

                default:
                {
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                }
                break;
            }
        }
        break;

        case EN_HDCP14TX_MS_A1A2:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP14TX_SS_EXCHANGE_KSV:
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
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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

                    if (HDCPTx_Hdcp1xCheckBKSVAndLn(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "HDCPTx_Hdcp1xCheckBKSVAndLn Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    //check revoke list
                    if (HDCPTx_Hdcp1xCheckRevokeKey(u8PortIdx) == TRUE)
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "BKSV is Revoked!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    if (HDCPTx_Hdcp1xGenerateCipher(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A3, EN_HDCP14TX_SS_IDLE);
                }
                break;

                default:
                {
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                }
                break;
            }
        }
        break;

        case EN_HDCP14TX_MS_A3:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP14TX_SS_IDLE:
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
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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

                    //process R0
                    if (HDCPTx_Hdcp1xProcessR0(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

					if(u8PortIdx >= COMBO_IP_SUPPORT_TYPE) //for DPTX Get R0
                    {
                    	HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A3, EN_HDCP14TX_SS_DP_GET_R0);
					}
					else
					{
						HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A3, EN_HDCP14TX_SS_CHECK_R0);
					}
                }
                break;
				case EN_HDCP14TX_SS_DP_GET_R0:
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
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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



                    //Check R0 available
                    if (g_stHdcpHandler[u8PortIdx].pGetDPTxR0Func == NULL)
                        {
                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Compare R0 Callback Function is NULL!");
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }

                        if (g_stHdcpHandler[u8PortIdx].pGetDPTxR0Func(GET_RECOVERY_TX_OFFSET(u8PortIdx)) == FALSE)
                        {
                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                            //HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Get Tx R0 Fail!");
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }

						//Get R0
                    if (HDCPTx_Hdcp1xGetR0(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }


                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A3, EN_HDCP14TX_SS_CHECK_R0);
                }
                break;
                case EN_HDCP14TX_SS_CHECK_R0:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == TRUE)
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
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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
                        g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A3, EN_HDCP14TX_SS_CHECK_R0);
                    }
                    else
                    {
                        //validate receiver
                        /**************************************************************************/
                        // [HDCP] 1A and 1B: AKSV -> Ro should be large than 100msec
                        // to pass Quantumdata 882 HDCP test, we delay 150ms
                        //
                        // 2013/11/07, in SEC Compatibility test, we meet an Repeater timeout error on PEPEATER YAMAHA RX-V2700
                        // patch AKSV -> Ro large than 250ms
                        /***************************************************************************/

                        //wait 100ms(at least) before check R0
                        g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                        #if DEF_DELAY_R0_COMPARE
                        if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) < DEF_HDCP1_CHK_R0_WDT)
                        {
                            HLOGDC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "CHECK_R0 Delay!");
                            break;
                        }
                        #endif
                        g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        if (g_stHdcpHandler[u8PortIdx].pCompareRiFunc == NULL)
                        {
                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Compare R0 Callback Function is NULL!");
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }
                    #if 0
                        //DP only get Tx R0
                        if(u8PortIdx >= COMBO_IP_SUPPORT_TYPE) //for DPTX Get R0
                        {
                        //IMI_ReadBytes(DEF_IMI_DP_GET_R0_ADDR, (BYTE*)&u16SourceRi , DEF_HDCP1X_RI_SIZE);
                        }
                    #endif
                        if (g_stHdcpHandler[u8PortIdx].pCompareRiFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), &u16SourceRi, &u16SinkRi) == FALSE)
                        {
                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Compare R0 Fail!");
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_IDLE);

                        //HDCPTx_Hdcp1xSetState(u8PortIdx, (g_stHdcpHandler[u8PortIdx].bSinkIsRepeater == TRUE) ? EN_HDCP14TX_MS_A8 : EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_IDLE);
                    }
                }
                break;

                default:
                {
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                }
                break;
            }
        }
        break;

        case EN_HDCP14TX_MS_A4:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP14TX_SS_IDLE:
                {
                    if (HDCPTx_Hdcp1xSetAuthPass(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }
                    g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                    // 1B-05/06/ & 3B-04/05 R0 pass-> encryption and disencryption after read R0
                    HDCPTx_Hdcp1xSetState(u8PortIdx, (g_stHdcpHandler[u8PortIdx].bSinkIsRepeater == TRUE) ? EN_HDCP14TX_MS_A8 : EN_HDCP14TX_MS_A4, (g_stHdcpHandler[u8PortIdx].bSinkIsRepeater == TRUE) ? EN_HDCP14TX_SS_IDLE : EN_HDCP14TX_SS_AUTH_DONE);
                    //HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_AUTH_DONE);
                }
                break;

                case EN_HDCP14TX_SS_AUTH_DONE:
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
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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
                    g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                    HLOGFC(EN_HLOG_HDCP, DEF_COLOR_YELLOW, "Auth Done, Start Compare Ri!");
                    #if 0
                    if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                    {
                        if(!g_stHdcpHandler[u8PortIdx].bPrepare2SetRdyBit)
                        {
                            break;
                        }

                        if(HDCPRepeater_SetBstatus(u8PortIdx) != EN_HDCP_ERR_NONE)
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            break;
                        }
                        if(HDCPTx_Hdcp1xRepeaterComputeVPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            break;
                        }
                        else
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_SET_KSVFIFO);
                            break;
                        }
                    }
                    #endif
                    if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                    {
                        if(HDCPTx_CopyInfo2Rx(u8PortIdx) != EN_HDCP_ERR_NONE)  //Copy info for Rx14Handler
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            break;
                        }
                    }
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_CHECK_RI);
                }
                break;

                case EN_HDCP14TX_SS_CHECK_RI:
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
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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
                    g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                    if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) < DEF_HDCP1_CHK_RI_PERIOD_WDT)
                    {
                        //HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Check Ri Timer!");
                        break;
                    }
                    g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                    if (g_stHdcpHandler[u8PortIdx].pCompareRiFunc == NULL)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Compare Ri Callback Function is NULL!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    if (g_stHdcpHandler[u8PortIdx].pCompareRiFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), &u16SourceRi, &u16SinkRi) == FALSE)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        #if 1
                        if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                        {
                            g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].pRxHPDCTRLFunc(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)], FALSE, TRUE);
                        }
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        #endif
                        HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Compare Ri Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }
                }
                break;
#if 0
                case EN_HDCP14TX_SS_SET_KSVFIFO:
                {
                    g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                    if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP1_RPT_RDY_BIT_WDT)
                    {
                        HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "SetKSVFIFO Timeout!");
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        break;
                    }

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
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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

                    if(HDCPRepeater_WriteVPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        break;
                    }
                    if(HDCPRepeater_SetKSVFIFO(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        break;
                    }
                    if(HDCPRepeater_SetReadyBit(u8PortIdx)!= EN_HDCP_ERR_NONE)
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        break;
                    }

                    g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_CHECK_RI);
                }
                break;
#endif
                default:
                {
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                }
                break;
            }
        }
        break;

        case EN_HDCP14TX_MS_A5:
        {
            enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
        }
        break;

        case EN_HDCP14TX_MS_A6:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP14TX_SS_IDLE:
                {
                    if (g_stHdcpHandler[u8PortIdx].bSinkIsRepeater == TRUE)
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A8, EN_HDCP14TX_SS_IDLE);
                    }
                    else
                    {
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_AUTH_DONE);
                    }
                }
                break;

                default:
                {
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                }
                break;
            }
        }
        break;

        case EN_HDCP14TX_MS_A7:
        {
            enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
        }
        break;

        case EN_HDCP14TX_MS_A8:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP14TX_SS_IDLE:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    {
                        //setup 5 sec WDT and polling RX ready bit;
                        g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A8, EN_HDCP14TX_SS_POLLING_RDY);
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
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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
                    }
                }
                break;

                case EN_HDCP14TX_SS_POLLING_RDY:
                {
                    g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                    if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) < DEF_HDCP1_RPT_RDY_BIT_WDT)
                    {
                        if (HDCPTx_Hdcp1xRepeaterChkRdyBit(u8PortIdx) == TRUE)
                        {
                            if ((HDCPTx_Hdcp1xRepeaterChkTopologyError(u8PortIdx) == FALSE) || (g_stHdcpHandler[u8PortIdx].bTopologyErr == TRUE))
                            {
                                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                                HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Topology Error!");
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                break;
                            }
                            else
                            {
                                HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A9, EN_HDCP14TX_SS_AUTH_REPEATER);
                            }
                        }
                    }
                    else
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        //HDCPTx_Hdcp1xSetENC_EN(u8PortIdx, FALSE);
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                        HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Polling Ready Bit Timeout!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                    }
                }
                break;

                default:
                {
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                }
                break;
            }
        }
        break;

        case EN_HDCP14TX_MS_A9:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP14TX_SS_IDLE:
                {
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A9, EN_HDCP14TX_SS_POLLING_RDY);
                }
                break;

                case EN_HDCP14TX_SS_AUTH_REPEATER:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    {
                        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_KSVFIFO) != EN_HDCP_ERR_NONE)
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read KSVList Fail!");
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }

                        if (HDCPTx_Hdcp1xReadOffset(u8PortIdx, EN_HDCP1X_X74_OFFSET_VPRIME_H0) != EN_HDCP_ERR_NONE)
                        {
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_RED, "Read V Prime Fail!");
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }

                        if (HDCPTx_Hdcp1xRepeaterChkVPrime(u8PortIdx) !=  EN_HDCP_ERR_NONE)
                        {
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Process V' Failed!");
                            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
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
                            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                            HLOGFC(EN_HLOG_HDCP, DEF_COLOR_LIGHT_PURPLE, "Check V' Fail!");
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
                        g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        HLOGFC(EN_HLOG_HDCP, DEF_COLOR_YELLOW, "V Prime Pass");
                        HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A4, EN_HDCP14TX_SS_AUTH_DONE);

                    }
                }
                break;

                default:
                {
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
                }
                break;
            }
        }
        break;

        default:
        {
            enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Unknown State!");
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
        }
        break;
    }

    //HTRACEL(EN_HLOG_HDCP);

    return enErrCode;
}

void __HDCP2X_Relative__(void)
{
    //pesudo function
}

void HDCPTx_Hdcp2SetState(BYTE u8PortIdx, BYTE u8MainState, BYTE u8SubState)
{
    g_stHdcpHandler[u8PortIdx].u8MainState = u8MainState;
    g_stHdcpHandler[u8PortIdx].u8SubState = u8SubState;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2Init(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE i = 0;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        //init pairing info list
        for (i = 0; i < DEF_HDCP_STORED_KM_CAPACITY; i++)
        {
            g_stStoredPairingInfo[i].bStored = FALSE;
            memset(g_stStoredPairingInfo[i].u8RecvID, 0x00, DEF_HDCP2_RECV_ID_SIZE);
            memset(g_stStoredPairingInfo[i].u8M, 0x00, DEF_HDCP2_M_SIZE);
            memset(g_stStoredPairingInfo[i].u8KM, 0x00, DEF_HDCP2_KM_SIZE);
            memset(g_stStoredPairingInfo[i].u8EkhKM, 0x00, DEF_HDCP2_EKHKM_SIZE);
        }


        if (g_stHdcpHandler[u8PortIdx].enHdcpType == EN_HDCPMBX_PORT_TYPE_SOURCE)
        {
            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_H1P1, EN_HDCP2_MSG_ZERO);
            HDCPTx_Hdcp1xSetState(u8PortIdx, EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_AUTH_START);
            g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_FREERUN;
            g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
            g_stHdcpHandler[u8PortIdx].bRetryStreamManage = FALSE;
            g_stHdcpHandler[u8PortIdx].bGetRecvIDList = FALSE;
            g_stHdcpHandler[u8PortIdx].bStoredKm = FALSE;
            g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
            g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
            g_stHdcpHandler[u8PortIdx].bAuthDone = FALSE;
            g_stHdcpHandler[u8PortIdx].bSinkIsRepeater = FALSE;
            g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
            g_stHdcpHandler[u8PortIdx].bTopologyErr = FALSE;
            g_stHdcpHandler[u8PortIdx].u32SeqNumVCnt = 0x00;
            g_u32AuthRetryCnt[u8PortIdx] = 0;
            g_u32LCRetryCnt[u8PortIdx] = 0;

            memset(g_stSeqNumHandler[u8PortIdx].u8SeqNumV, 0x00, DEF_HDCP2_SEQ_NUM_V_SIZE);
            g_stSeqNumHandler[u8PortIdx].u8RetryCnt = 0;

            //clear mailbox
            HDCPMBX_SetActiveBit(u8PortIdx, FALSE);
            HDCPMBX_SetState(u8PortIdx, EN_HDCPMBX_STATE_IDLE);
        }

    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

BOOL HDCPTx_Hdcp2CheckSinkCapability(BYTE u8PortIdx)
{
    BYTE u8Hdcp2Verion = 0;
    BOOL bRet = FALSE;

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc == NULL)
        {
            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "NULL Function Pointer!!");
            break;
        }

        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), (BYTE)EN_HDCP2X_X74_OFFSET_VERSION, EN_HDCP2_MSG_ZERO, (BYTE)EN_HDCP_OPCODE_READ, DEF_HDCP2_VER_NFO_SIZE, &u8Hdcp2Verion) == FALSE)
        {
            HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Access I/O FAILED!");
            break;
        }
        bRet = (u8Hdcp2Verion & (1<<2)) ? TRUE : FALSE;

    } while(FALSE);

    return bRet;
}
#if 0
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2EnableAuth(BYTE u8PortIdx, BOOL bEnable)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE* u8SendData = (BYTE*)IMIGetAddr(DEF_IMI_ENAUTH_ADDR);

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        *(BYTE*)(u8SendData) = u8PortIdx;
        *(BYTE*)(u8SendData + 1) = bEnable;

        if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_HDCP2X_ENABLE_AUTH, sizeof(BYTE), (ULONG)u8SendData) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        //g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;

}
#endif
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2SendMsg(BYTE u8PortIdx, ENUM_HDCP2_MSG_LIST enMsg)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    WORD u16MsgLen = 0;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL Function Pointer!");
            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

        switch (enMsg)
        {
            case EN_HDCP2_MSG_AKE_INIT:
                u16MsgLen = DEF_HDCP2_RRX_SIZE + DEF_HDCP2_RXCAPS_SIZE;
            break;

            case EN_HDCP2_MSG_AKE_NO_STORED_KM:
                u16MsgLen = DEF_HDCP2_EKPUBKM_SIZE;
            break;

            case EN_HDCP2_MSG_AKE_STORED_KM:
                u16MsgLen = DEF_HDCP2_EKHKM_SIZE + DEF_HDCP2_KM_SIZE;
            break;

            case EN_HDCP2_MSG_LC_INIT:
                u16MsgLen = DEF_HDCP2_RN_SIZE;
            break;

            case EN_HDCP2_MSG_SKE_SEND_EKS:
                u16MsgLen = DEF_HDCP2_EDKEYKS_SIZE + DEF_HDCP2_RIV_SIZE;
            break;

            case EN_HDCP2_MSG_REPAUTH_SEND_ACK:
                u16MsgLen = (DEF_HDCP2_V_SIZE >> 1);
            break;

            case EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE:
                u16MsgLen = DEF_HDCP2_SEQ_NUM_M_SIZE + DEF_HDCP2_K_SIZE + 2;
            break;

            default:
                enErrCode = EN_HDCP_ERR_INVALID_ARG;
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "--> Send MsgID %d", enMsg);
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)), u16MsgLen + 1);
        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), EN_HDCP2X_X74_OFFSET_WRITEMSG, enMsg, EN_HDCP_OPCODE_WRITE, u16MsgLen + 1, (BYTE*)((ULONG)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)));

    } while(FALSE);
    if (enMsg == EN_HDCP2_MSG_LC_INIT)
    {
        if (enErrCode == EN_HDCP_ERR_NONE)
        {
            g_stHdcpHandler[u8PortIdx].bSendLCInit = TRUE;
        }
        else
        {
            g_stHdcpHandler[u8PortIdx].bSendLCInit = FALSE;
        }
    }

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessAKEInit(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "process AKE_Init");
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_AKE_INIT, EN_HDCPMBX_CMD_HOST_HKR2,\
            0, 0) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessLCInit(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "process LC_Init");
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_LC_INIT, EN_HDCPMBX_CMD_HOST_HKR2,\
            0, 0) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessSKESendEks(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "process SKE_Send_Eks");
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_SKE_SEND_EKS, EN_HDCPMBX_CMD_HOST_HKR2,\
            0, 0) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2ProcessRepAuthStreamManage(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        volatile BYTE u8RetryFlag = 0x00;

        u8RetryFlag = g_stHdcpHandler[u8PortIdx].bRetryStreamManage;
        *(BYTE*)((ULONG)&u8RetryFlag | 0x80000000) = g_stHdcpHandler[u8PortIdx].bRetryStreamManage;
        #if 0
        memcpy((g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), &g_stHdcpHandler[u8PortIdx].bRetryStreamManage, 1);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_REP_STREAM_MANAGE, EN_HDCPMBX_CMD_HOST_HKR2,\
            3, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX) == FALSE)
        #endif
        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "process RepeaterAuth_Stream_Manage");
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_REP_STREAM_MANAGE, EN_HDCPMBX_CMD_HOST_HKR2,\
            1, (ULONG)&u8RetryFlag) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvAKESendCert(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        memcpy((void*)g_stHdcpHandler[u8PortIdx].u8RecvID, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), DEF_HDCP2_RECV_ID_SIZE);
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "HDCPTx_Hdcp2RecvAKESendCert");
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_AKE_SEND_CERT, EN_HDCPMBX_CMD_HOST_HKR2,\
            g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_CERT], (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvAKESendHPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    //ULONG u8Hprime = IMIGetAddr(0x3D30);
    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        //memcpy((void*)u8Hprime, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_H_PRIME]);
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "HDCPTx_Hdcp2RecvAKESendHPrime");
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_AKE_SEND_H_PRIME, EN_HDCPMBX_CMD_HOST_HKR2,\
            g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_H_PRIME], (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}


ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvAKESendPairing(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "HDCPTx_Hdcp2RecvAKESendPairing");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO]);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_AKE_SEND_PARING_INFO, EN_HDCPMBX_CMD_HOST_HKR2,\
            g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO], (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);
    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvLCSendLPrime(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "HDCPTx_Hdcp2RecvLCSendLPrime");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_LC_SEND_L_PRIME]);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_LC_SEND_L_PRIME, EN_HDCPMBX_CMD_HOST_HKR2,\
            g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_LC_SEND_L_PRIME], (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvRepAuthSendRecvIDList(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        WORD u16MsgLen = 0x00;

        u16MsgLen = 1 + DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE>>1) + g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE;
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "HDCPTx_Hdcp2RecvRepAuthSendRecvIDList");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), u16MsgLen);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_MBXCMD_HDCP2_REP_SEND_RECVID_LIST, EN_HDCPMBX_CMD_HOST_HKR2,\
            u16MsgLen, (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2RecvRepAuthStreamReady(BYTE u8PortIdx)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "HDCPTx_Hdcp2RecvRepAuthStreamReady");
        HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX), g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_REPAUTH_STREAM_READY]);
        if (HDCPMBX_SetCmd(u8PortIdx, EN_HDCPMBX_PORT_TYPE_SOURCE, EN_HDCP2_MSG_REPAUTH_STREAM_READY, EN_HDCPMBX_CMD_HOST_HKR2,\
            g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_REPAUTH_STREAM_READY], (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX)) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;
}

ENUM_HDCP2_SEQNUM_V_STATUS HDCPTx_Hdcp2CheckSeqNumV(BYTE u8PortIdx)
{
    ENUM_HDCP2_SEQNUM_V_STATUS enRet = EN_HDCP2_SEQNUMV_FAIL;

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        if (((g_stSeqNumHandler[u8PortIdx].u8SeqNumV[0] == 0xFF)&&(g_stSeqNumHandler[u8PortIdx].u8SeqNumV[1] == 0xFF)&&(g_stSeqNumHandler[u8PortIdx].u8SeqNumV[2] == 0xFF)) ||
            (g_stHdcpHandler[u8PortIdx].u32SeqNumVCnt > 0xFFFFFF))
        {
            //rollover
            HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_BLUE, "SeqNumV Full!");

			g_stSeqNumHandler[u8PortIdx].bSeqNumVFull = TRUE;
            //ghdcp22TxContnet.bSeqNumVRollover = TRUE;
            enRet = EN_HDCP2_SEQNUMV_PASS;
            break;
        }
		if (((g_stSeqNumHandler[u8PortIdx].u8SeqNumV[0] == 0x00)&&(g_stSeqNumHandler[u8PortIdx].u8SeqNumV[1] == 0x00)&&(g_stSeqNumHandler[u8PortIdx].u8SeqNumV[2] == 0x00)) &&
			(g_stSeqNumHandler[u8PortIdx].bSeqNumVFull == TRUE))
		{
			//rollover
			HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_BLUE, "SeqNumV Rollover!");

			g_stSeqNumHandler[u8PortIdx].bSeqNumVFull = FALSE;
			enRet = EN_HDCP2_SEQNUMV_ROLLOVER;
			break;
		}
        if ((g_stSeqNumHandler[u8PortIdx].u8SeqNumV[0] != (MS_U8)((g_stHdcpHandler[u8PortIdx].u32SeqNumVCnt & 0xFF0000)>>16)) ||
            (g_stSeqNumHandler[u8PortIdx].u8SeqNumV[1] != (MS_U8)((g_stHdcpHandler[u8PortIdx].u32SeqNumVCnt & 0x00FF00)>>8)) ||
            (g_stSeqNumHandler[u8PortIdx].u8SeqNumV[2] != (MS_U8)((g_stHdcpHandler[u8PortIdx].u32SeqNumVCnt & 0x0000FF))))
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_BLUE, "Invalid Seq_num_V!");

            enRet = EN_HDCP2_SEQNUMV_FAIL;
            break;
        }

        enRet = EN_HDCP2_SEQNUMV_PASS;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enRet;
}

ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2FSM(BYTE u8PortIdx)
{
    static WORD u16TimeoutValue = 0;
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    ENUM_HDCPMBX_STATE_LIST enState = EN_HDCPMBX_STATE_IDLE;
#if 0
    static BYTE u8PreMain = 0;
    static BYTE u8PreSub = 0;

    if ((u8PreMain != g_stHdcpHandler[u8PortIdx].u8MainState) || (g_stHdcpHandler[u8PortIdx].u8SubState != u8PreSub))
    {
        HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, "Port[%d](M : S)= (%d, %d)", u8PortIdx, g_stHdcpHandler[u8PortIdx].u8MainState, g_stHdcpHandler[u8PortIdx].u8SubState);
        u8PreMain = g_stHdcpHandler[u8PortIdx].u8MainState;
        u8PreSub = g_stHdcpHandler[u8PortIdx].u8SubState;
    }
#endif

    switch (g_stHdcpHandler[u8PortIdx].u8MainState)
	{
	    case EN_HDCP2TX_MS_H1P1:
        break;

        case EN_HDCP2TX_MS_A0F0:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                {
                    if (g_stHdcpHandler[u8PortIdx].bPolling == TRUE)
                    {
                        if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                        {
                            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Get State Fail!");
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
                    }
                    g_stHdcpHandler[u8PortIdx].u8RetryCnt++;
                    if (HDCPTx_Hdcp2CheckSinkCapability(u8PortIdx) == TRUE)
                    {
                        g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_22;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A1F1, EN_HDCP2_MSG_ZERO);
                        //HDCPTx_Hdcp2EnableAuth(u8PortIdx, FALSE);
                        HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_GREEN, "Sink Support Hdcp22!");
                    }
                    else
                    {
                        if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_VER_22)
                        {
                            enErrCode = EN_HDCP_ERR_NONE;
                            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Sink Doesn't Support Hdcp22!");
                            return enErrCode;
                        }
                        else
                        {
                            if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                            {
                                if(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8HDCPRxVer == EN_HDCP_VER_22)
                                {
                                    enErrCode = EN_HDCP_ERR_NONE;
                                    HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Sink Doesn't Support Hdcp22! But UpStream Do Hdcp22!");
                                    return enErrCode;
                                }
                            }
                        }
                        g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_14;
                        enErrCode = EN_HDCP_ERR_NONE;
                        HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Sink Doesn't Support Hdcp22!");
                        return enErrCode;
                    }
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                break;
            }
        }
        break;

        case EN_HDCP2TX_MS_A1F1:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                {
                    g_stHdcpHandler[u8PortIdx].bSendLCInit = FALSE;
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = FALSE;
                    g_stHdcpHandler[u8PortIdx].bAuthDone = FALSE;
                    g_stHdcpHandler[u8PortIdx].bRetryStreamManage = FALSE;
                    g_stHdcpHandler[u8PortIdx].bGetRecvIDList = FALSE;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                    g_stHdcpHandler[u8PortIdx].bSinkIsRepeater = FALSE;
                    g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                    g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                    g_stHdcpHandler[u8PortIdx].u32SeqNumVCnt = 0;
                    g_stHdcpHandler[u8PortIdx].bRx2TxStreamType = FALSE;
                    memset((void*)g_stHdcpHandler[u8PortIdx].u8RxInfo, 0x00, DEF_HDCP2_RXINFO_SIZE);
                    //HDCPTx_Hdcp2EnableAuth(u8PortIdx, FALSE);
                    g_u8H1xFSMReset = TRUE;
                    if (g_u32AuthRetryCnt[u8PortIdx] < DEF_HDCP2_TX_RETRY_CNT)
                    {
                        g_u32AuthRetryCnt[u8PortIdx]++;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;

                        if (HDCPTx_Hdcp2ProcessAKEInit(u8PortIdx) != EN_HDCP_ERR_NONE)
                        {
                            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Mailbox Fail!");
                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                            enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }

                        g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_AKE_INIT;

                    }
                }
                break;

                case EN_HDCP2_MSG_AKE_INIT:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }
                    if (enState == EN_HDCPMBX_STATE_HALT)
                    {
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Process Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    if (enState != EN_HDCPMBX_STATE_DONE)
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                        break;
                    }

                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;
                    g_stHdcpHandler[u8PortIdx].bSendPair = FALSE; //clear pairing info flag;
                    g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].bTx2RxInfo= FALSE; //clear CopyInfo flag;

                    if (HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_AKE_INIT) != EN_HDCP_ERR_NONE)
                    {
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = TRUE;
                    g_stHdcpHandler[u8PortIdx].bWaitRxResponse = TRUE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A1F1, EN_HDCP2_MSG_AKE_SEND_CERT);
                    g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                }
                break;

                case EN_HDCP2_MSG_AKE_SEND_CERT:
                {
                    if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                    {
                        if (g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE)
                        {
                            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_AKESENDCERT_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);

                                break;
                            }
                        }
                    }
                    else
                    {
                        if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                        {
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_AKESENDCERT_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);

                                break;
                            }

                            if (HDCPTx_Hdcp2RecvAKESendCert(u8PortIdx) != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Mailbox Fail!");
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                break;
                            }

                            //process AKE_Send_Cert
                            g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                            g_stHdcpHandler[u8PortIdx].bSinkIsRepeater = (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX + g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_CERT] - 1) & BIT0)? TRUE : FALSE;
                            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_CYAN, g_stHdcpHandler[u8PortIdx].bSinkIsRepeater ? "Sink is Repeater!" : "Sink is pure Rx!");

                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A1F1, EN_HDCP2_MSG_AKE_SEND_CERT);
                        }
                        else //waiting for SECU R2 finish
                        {
                            if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                                break;
                            }

                            if (enState == EN_HDCPMBX_STATE_HALT)
                            {
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Verify Cert Fail!");
                                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                break;
                            }

                            if (enState != EN_HDCPMBX_STATE_DONE)
                            {
                                HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                                break;
                            }

                            g_stHdcpHandler[u8PortIdx].bStoredKm = (*(BYTE*)(g_u8HdcpTransBuf + (u8PortIdx * DEF_HDCP_TRANS_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) == EN_HDCP2_MSG_AKE_STORED_KM) ? TRUE : FALSE;

                            if (g_stHdcpHandler[u8PortIdx].bStoredKm == TRUE)
                                enErrCode = HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_AKE_STORED_KM);
                            else
                                enErrCode = HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_AKE_NO_STORED_KM);

                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                            if (enErrCode != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                break;
                            }

                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A1F1, EN_HDCP2_MSG_AKE_SEND_H_PRIME);
                            u16TimeoutValue = (g_stHdcpHandler[u8PortIdx].bStoredKm) ? DEF_HDCP2_AKESENDHPRIME_STORED_WDT : DEF_HDCP2_AKESENDHPRIME_NO_STORED_WDT;
                            g_stHdcpHandler[u8PortIdx].bWaitRxResponse = TRUE;
                            g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                            g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        }//if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    }//if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                }
                break;

                case EN_HDCP2_MSG_AKE_SEND_H_PRIME:
                {
                    if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                    {
                        if (g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE)
                        {
                            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > u16TimeoutValue)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!!Time :%d", u16TimeoutValue);
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);

                                break;
                            }
                        }
                    }
                    else
                    {
                        if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                        {
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > u16TimeoutValue)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);

                                break;
                            }
                            g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                            if (HDCPTx_Hdcp2RecvAKESendHPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                break;
                            }

                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A1F1, EN_HDCP2_MSG_AKE_SEND_H_PRIME);
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
                                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Verify H' Fail!");
                                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                break;
                            }

                            if (enState != EN_HDCPMBX_STATE_DONE)
                            {
                                HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                                break;
                            }

                            HLOGVC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "H' is PASS!!");

                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;
                            if(!g_stHdcpHandler[u8PortIdx].bSendPair)
                            {
                                g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                            }

                            if (g_stHdcpHandler[u8PortIdx].bStoredKm == TRUE)
                            {
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                enErrCode = HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_LC_INIT);
                                if (enErrCode != EN_HDCP_ERR_NONE)
                                {
                                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                    enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                    break;
                                }
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = TRUE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A2F2, EN_HDCP2_MSG_LC_SEND_L_PRIME);
                                //g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                            }
                            else
                            {
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = TRUE;
                                HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Prepare to Receive Pairing Info!");
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A1F1, EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO);
                                //g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                            }
                        }
                    }//if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                }
                break;

                case EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO:
                {
                    if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                    {
                        if (g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE)
                        {
                            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_AKESENDPAIRINGINFO_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);

                                break;
                            }
                        }
                    }
                    else
                    {
                        if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                        {
                            //printf("port : %d , Pair Time : %X\r\n", GET_RECOVERY_TX_OFFSET(u8PortIdx), (unsigned int)_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]));
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_AKESENDPAIRINGINFO_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);

                                break;
                            }
                            g_stHdcpHandler[u8PortIdx].bSendPair = TRUE;

                            if (HDCPTx_Hdcp2RecvAKESendPairing(u8PortIdx) != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                break;
                            }
                            g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A1F1, EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO);
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
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Prepare LC_Init Failed!");
                                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                break;
                            }

                            if (enState != EN_HDCPMBX_STATE_DONE)
                            {
                                HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                                break;
                            }

                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                            g_stHdcpHandler[u8PortIdx].bWaitRxResponse = TRUE;

                            enErrCode = HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_LC_INIT);
                            if (enErrCode != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                break;
                            }
                            g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A2F2, EN_HDCP2_MSG_LC_SEND_L_PRIME);
                            g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        }//if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    }//if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                break;
            }
        }
        break;

        case EN_HDCP2TX_MS_A2F2: //locality check
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO: //retry LC_Init
                {
                    if (HDCPTx_Hdcp2ProcessLCInit(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                        g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                        break;
                    }
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A2F2, EN_HDCP2_MSG_LC_INIT);
                }
                break;

                case EN_HDCP2_MSG_LC_INIT:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }

                    if (enState == EN_HDCPMBX_STATE_HALT)
                    {
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Prepare LC_Init Fail!");
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                        g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                        break;
                    }

                    if (enState != EN_HDCPMBX_STATE_DONE)
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                        break;
                    }

                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                    if (HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_LC_INIT) != EN_HDCP_ERR_NONE)
                    {
                        g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }
                    g_stHdcpHandler[u8PortIdx].bSendLCInit = TRUE;

                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A2F2, EN_HDCP2_MSG_LC_SEND_L_PRIME);
                    g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                }
                break;

                case EN_HDCP2_MSG_LC_SEND_L_PRIME:
                {
                    if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                    {
                        if (g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE)
                        {
                            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_LCSENDLPRIME_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                if (g_u32LCRetryCnt[u8PortIdx] > DEF_HDCP2_TX_LC_RETRY_CNT)
                                {
                                    g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                    g_u32LCRetryCnt[u8PortIdx] = 0;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                }
                                else
                                {
                                    g_u32LCRetryCnt[u8PortIdx]++;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A2F2, EN_HDCP2_MSG_ZERO);
                                }

                                break;
                            }
                        }
                    }
                    else
                    {
                        if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                        {
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_LCSENDLPRIME_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");

                                if (g_u32LCRetryCnt[u8PortIdx] > DEF_HDCP2_TX_LC_RETRY_CNT)
                                {
                                    g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                    g_u32LCRetryCnt[u8PortIdx] = 0;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                }
                                else
                                {
                                    g_u32LCRetryCnt[u8PortIdx]++;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A2F2, EN_HDCP2_MSG_ZERO);
                                }
                                break;
                            }

                            if (HDCPTx_Hdcp2RecvLCSendLPrime(u8PortIdx) != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Mailbox Fail!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                break;
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
                                HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Verify L' Fail!");
                                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                break;
                            }

                            if (enState != EN_HDCPMBX_STATE_DONE)
                            {
                                HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                                break;
                            }

                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;

                            if (HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_SKE_SEND_EKS) != EN_HDCP_ERR_NONE)
                            {
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                break;
                            }

                            g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                            if (g_stHdcpHandler[u8PortIdx].bSinkIsRepeater == FALSE)
                            {
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A3F3, EN_HDCP2_MSG_SKE_SEND_EKS);
                                g_stHdcpHandler[u8PortIdx].bAuthDone = TRUE;
                                g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                            }
                            else
                            {
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A6F6, EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST);
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = TRUE;
                                g_stHdcpHandler[u8PortIdx].bAuthDone = FALSE;
                            }

                            g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                        }
                    }
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                break;
            }
        }
        break;

        case EN_HDCP2TX_MS_A3F3:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                {
                }
                break;

                case EN_HDCP2_MSG_SKE_SEND_EKS:
                {
                    g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                    if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) >= DEF_HDCP2_ENC_EN_TIMER)
                    {
                        HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_GREEN, "Hdcp22 TX Auth Done@Port #%d", u8PortIdx);
                        //HDCPTx_Hdcp2EnableAuth(u8PortIdx, TRUE);
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A5F5, EN_HDCP2_MSG_ZERO);
                        g_stHdcpHandler[u8PortIdx].u8RetryCnt = 0; //clear retry count;
                        if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                        {
                            if(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8HDCPRxVer == EN_HDCP_VER_14)
                            {
                                HDCPTx_CopyInfo2Rx(u8PortIdx);
                            }
                            else
                            {
                                g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].bTx2RxInfo = TRUE;
                            }
                        }
                    }
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                break;
            }
        }
        break;

        case EN_HDCP2TX_MS_A4F4:
        {
        }
        break;

        case EN_HDCP2TX_MS_A5F5:
        {
        }
        break;

        case EN_HDCP2TX_MS_A6F6:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST:
                {
                    if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                    {
                        if (g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE)
                        {
                            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_REPAUTHSENDRECVID_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_u32LCRetryCnt[u8PortIdx] = 0;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                        {
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_REPAUTHSENDRECVID_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_u32LCRetryCnt[u8PortIdx] = 0;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                break;
                            }

                            //check seqNumV here;
                            g_stSeqNumHandler[u8PortIdx].enSeqNumVStatus = HDCPTx_Hdcp2CheckSeqNumV(u8PortIdx);
                            if (g_stSeqNumHandler[u8PortIdx].enSeqNumVStatus == EN_HDCP2_SEQNUMV_FAIL)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "illegal SeqNumV!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                break;
                            }

                            if (HDCPTx_Hdcp2RecvRepAuthSendRecvIDList(u8PortIdx) != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Mailbox Fail!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                break;
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
                                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Verify V' Fail!");
                                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                break;
                            }

                            if (enState != EN_HDCPMBX_STATE_DONE)
                            {
                                HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                                break;
                            }

                            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "V' is PASS!!");

                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                            //g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                            // force FSM to send stream management

                            g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                            enErrCode = HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_REPAUTH_SEND_ACK);
                            if (enErrCode != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                break;
                            }

                            if (g_stSeqNumHandler[u8PortIdx].enSeqNumVStatus == EN_HDCP2_SEQNUMV_ROLLOVER)
                            {
                                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_BLUE, "SeqNumV Roll-over, Re-Auth!");
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                                break;
                            }

                            g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A8F8, EN_HDCP2_MSG_ZERO);

                        } //if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                    }
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                break;
            }
        }
        break;

        case EN_HDCP2TX_MS_A7F7:
        {
        }
        break;

        case EN_HDCP2TX_MS_A8F8:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_ZERO:
                {
                    #if 0
                    if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                    {
                        if(g_stHdcpHandler[u8PortIdx].bRx2TxStreamType)
                        {
                            memcpy((g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX + 1), &g_stHdcpHandler[u8PortIdx].u8StreamIDType, 2);
                            g_stHdcpHandler[u8PortIdx].bRx2TxStreamType = FALSE;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        g_stHdcpHandler[u8PortIdx].u8StreamIDType[0] = 0x00;
                        g_stHdcpHandler[u8PortIdx].u8StreamIDType[1] = 0x01;
                        memcpy((g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX + 1), &g_stHdcpHandler[u8PortIdx].u8StreamIDType, 2);
                    }
                    #endif
                    if (HDCPTx_Hdcp2ProcessRepAuthStreamManage(u8PortIdx) != EN_HDCP_ERR_NONE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Mailbox Fail!");
                        g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A8F8, EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE);
                }
                break;

                case EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE:
                {
                    if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Get State Fail!");
                        break;
                    }

                    if (enState == EN_HDCPMBX_STATE_HALT)
                    {
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Stream Manage Fail!");
                        HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                        HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                        g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                        g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                        break;
                    }

                    if (enState != EN_HDCPMBX_STATE_DONE)
                    {
                        HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                        break;
                    }

                    HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                    HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE;

                    g_stHdcpHandler[u8PortIdx].bWaitRxResponse = TRUE;
                    enErrCode = HDCPTx_Hdcp2SendMsg(u8PortIdx, EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE);
                    if (enErrCode != EN_HDCP_ERR_NONE)
                    {
                        HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Msg Fail!");
                        g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                        enErrCode = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A9F9, EN_HDCP2_MSG_REPAUTH_STREAM_READY);
                    g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                break;
            }
        }
        break;

        case EN_HDCP2TX_MS_A9F9:
        {
            switch(g_stHdcpHandler[u8PortIdx].u8SubState)
            {
                case EN_HDCP2_MSG_REPAUTH_STREAM_READY:
                {
                    if (g_stHdcpHandler[u8PortIdx].bRecvMsg == FALSE)
                    {
                        if (g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE)
                        {
                            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();

                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_REPAUTHSTREAMRDY_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                if (g_stHdcpHandler[u8PortIdx].bRetryStreamManage == FALSE)
                                {
                                    if (g_stSeqNumHandler[u8PortIdx].u8RetryCnt >= DEF_HDCP2_STREAM_MANAGE_RETRY_CNT)
                                    {
                                        g_stHdcpHandler[u8PortIdx].bRetryStreamManage = TRUE;
                                        g_stHdcpHandler[u8PortIdx].bPolling = TRUE; //make FSM availiable
                                    }
                                    else
                                    {
                                        g_stSeqNumHandler[u8PortIdx].u8RetryCnt++;
                                    }
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A8F8, EN_HDCP2_MSG_ZERO);
                                }
                                else
                                {
                                    g_stHdcpHandler[u8PortIdx].bRetryStreamManage = FALSE;
                                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (g_stHdcpHandler[u8PortIdx].bPolling == FALSE)
                        {
                            if (_HDCPTx_GetTimeDiff(g_u32HDCPPreTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)], g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)]) > DEF_HDCP2_REPAUTHSTREAMRDY_WDT)
                            {
                                enErrCode = EN_HDCP_ERR_RESPONSE_TIMEROUT;
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Timeout!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                if (g_stHdcpHandler[u8PortIdx].bRetryStreamManage == FALSE)
                                {
                                    if (g_stSeqNumHandler[u8PortIdx].u8RetryCnt >= DEF_HDCP2_STREAM_MANAGE_RETRY_CNT)
                                    {
                                        g_stSeqNumHandler[u8PortIdx].u8RetryCnt = 0;
                                        g_stHdcpHandler[u8PortIdx].bRetryStreamManage = TRUE;
                                        g_stHdcpHandler[u8PortIdx].bPolling = TRUE; //make FSM availiable
                                    }
                                    else
                                    {
                                        g_stSeqNumHandler[u8PortIdx].u8RetryCnt++;
                                    }
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A8F8, EN_HDCP2_MSG_ZERO);
                                }
                                else
                                {
                                    g_stHdcpHandler[u8PortIdx].bRetryStreamManage = FALSE;
                                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                }
                                break;
                            }

                            if (HDCPTx_Hdcp2RecvRepAuthStreamReady(u8PortIdx) != EN_HDCP_ERR_NONE)
                            {
                                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Send Mailbox Fail!");
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                break;
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
                                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Verify M' Fail!");
                                HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                                HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
                                g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                                g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;

                                if (g_stHdcpHandler[u8PortIdx].bRetryStreamManage == FALSE)
                                {
                                    if (g_stSeqNumHandler[u8PortIdx].u8RetryCnt >= DEF_HDCP2_STREAM_MANAGE_RETRY_CNT)
                                    {
                                        g_stSeqNumHandler[u8PortIdx].u8RetryCnt = 0;
                                        g_stHdcpHandler[u8PortIdx].bRetryStreamManage = TRUE;
                                    }
                                    else
                                    {
                                        g_stSeqNumHandler[u8PortIdx].u8RetryCnt++;
                                    }
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A8F8, EN_HDCP2_MSG_ZERO);
                                }
                                else
                                {
                                    g_stHdcpHandler[u8PortIdx].bRetryStreamManage = FALSE;
                                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                                    g_stHdcpHandler[u8PortIdx].bPolling = FALSE;
                                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO); //do re-authentication
                                }
                                break;
                            }

                            if (enState != EN_HDCPMBX_STATE_DONE)
                            {
                                HLOGV(EN_HLOG_HDCP2X, "Command NOT Done");
                                break;
                            }

                            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
                            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;

                            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "M' is PASS!!");
                            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A3F3, EN_HDCP2_MSG_SKE_SEND_EKS);
                            g_stHdcpHandler[u8PortIdx].bAuthDone = TRUE;
                            g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
                            g_stHdcpHandler[u8PortIdx].bWaitRxResponse = FALSE;
                            g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                        }
                    }
                }
                break;

                default:
                    enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
                    HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                break;
            }
        }
        break;

        default:
            enErrCode = EN_HDCP_ERR_UNKNOWN_STATE;
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Unknown State!");
            g_stHdcpHandler[u8PortIdx].bRetryAKEInit = TRUE;
            g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
        break;
    }

    return enErrCode;
}

void HDCPTx_Hdcp2MsgParser(BYTE u8PortIdx, BYTE *pucMsg)
{
    ULONG u32ArgAddr = 0;
    HTRACEE(EN_HLOG_HDCP2X);

    u32ArgAddr = (ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX);
    u32ArgAddr |= 0x80000000;

    switch (*pucMsg)
    {
        case EN_HDCP2_MSG_AKE_SEND_CERT:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGE(EN_HLOG_HDCP2X, "Msg Slot is NOT empty!");
                return;
            }

            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
            g_stHdcpHandler[u8PortIdx].bRecvMsg = TRUE;

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_CERT]);

            HLOGD(EN_HLOG_HDCP2X, "EN_HDCP2_MSG_AKE_SEND_CERT::");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_CERT]);
        }
        break;

        case EN_HDCP2_MSG_AKE_SEND_H_PRIME:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGE(EN_HLOG_HDCP2X, "Msg Slot is NOT empty!");
                return;
            }

            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
            g_stHdcpHandler[u8PortIdx].bRecvMsg = TRUE;

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_H_PRIME]);

            HLOGV(EN_HLOG_HDCP2X, "EN_HDCP2_MSG_AKE_SEND_H_PRIME::");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_H_PRIME]);
        }
        break;

        case EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGE(EN_HLOG_HDCP2X, "Msg Slot is NOT empty!");
                return;
            }

            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
            g_stHdcpHandler[u8PortIdx].bRecvMsg = TRUE;
            if(g_stHdcpHandler[u8PortIdx].bPolling)
            {
                //printf("HPrime is not done\r\n");
                g_stHdcpHandler[u8PortIdx].bSendPair = TRUE;
            }
            else
            {
                HDCPTx_Hdcp2SetState(u8PortIdx, (BYTE)EN_HDCP2TX_MS_A1F1, (BYTE)EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO);
            }
     //paul mask    //HDCPTx_Hdcp2SetState(u8PortIdx, (BYTE)EN_HDCP2TX_MS_A1F1, (BYTE)EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO]);

            HLOGV(EN_HLOG_HDCP2X, "EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO::");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO]);
        }
        break;

        case EN_HDCP2_MSG_LC_SEND_L_PRIME:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGE(EN_HLOG_HDCP2X, "Msg Slot is NOT empty!");
                return;
            }

            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
            g_stHdcpHandler[u8PortIdx].bRecvMsg = TRUE;
            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A2F2, EN_HDCP2_MSG_LC_SEND_L_PRIME);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_LC_SEND_L_PRIME]);

            HLOGV(EN_HLOG_HDCP2X, "EN_HDCP2_MSG_LC_SEND_L_PRIME::");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_LC_SEND_L_PRIME]);
			/*for(idx=0;idx<33;idx++)
				{//printf("%x",(unsigned char *)(u32ArgAddr++));
			printf("%x ",*(pucMsg+idx));}*/
            //HTIMER_DIFF(u8PortIdx, g_u16TimeDiff);
        }
        break;

        case EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST:
        {
            WORD u16MsgLen = 0;

            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGE(EN_HLOG_HDCP2X, "Msg Slot is NOT empty!");
                return;
            }

            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
            g_stHdcpHandler[u8PortIdx].bRecvMsg = TRUE;
            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A6F6, EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST);

            //cpy seqNumV
            memcpy(g_stSeqNumHandler[u8PortIdx].u8SeqNumV, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CONTENT_IDX + DEF_HDCP2_RXINFO_SIZE), DEF_HDCP2_SEQ_NUM_V_SIZE);

            //fetch message
            u16MsgLen = 1 + DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE>>1) + g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE;
            memcpy((void*)u32ArgAddr, pucMsg, u16MsgLen);

            HLOGV(EN_HLOG_HDCP2X, "EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST::");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, u16MsgLen);
        }
        break;

        case EN_HDCP2_MSG_REPAUTH_STREAM_READY:
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_BASE_IDX) != 0) //already got message
            {
                HLOGE(EN_HLOG_HDCP2X, "Msg Slot is NOT empty!");
                return;
            }

            g_u32HDCPCurTime[GET_RECOVERY_TX_OFFSET(u8PortIdx)] = MAsm_GetSystemTime();
            g_stHdcpHandler[u8PortIdx].bRecvMsg = TRUE;
            HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A9F9, EN_HDCP2_MSG_REPAUTH_STREAM_READY);

            //fetch message
            memcpy((void*)u32ArgAddr, pucMsg, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_REPAUTH_STREAM_READY]);

            HLOGV(EN_HLOG_HDCP2X, "EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST::");
            HDCPLOG_DumpHexValue(EN_HLOG_HDCP2X, (unsigned char *)u32ArgAddr, g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO]);
        }
        break;

        default:
        break;
    }
    HTRACEL(EN_HLOG_HDCP2X);
}

#if 0
ENUM_HDCP_ERR_CODE HDCP1xTx_Handler(BYTE u8PortIdx)
{
}

ENUM_HDCP_ERR_CODE HDCP2xTx_Handler(BYTE u8PortIdx)
{
    do
    {
    } while(FALSE);
}
#endif

ENUM_HDCP_ERR_CODE HDCPTx_Handler(BYTE u8PortIdx)
{
    #define DEF_DIS_ENC_CNT 20
    ENUM_HDCP_ERR_CODE bRet = EN_HDCP_ERR_NONE;
    WORD u16RxStatus = 0;
    WORD u16MsgLen = 0;
    BYTE u16TotalLen = 0;
    //static BYTE u8EncDisCnt = 0;

//printf("HANDLER");
    do
    {
	//printf("\n%d\n",u8PortIdx);
	//printf("1");
        #if 0
        if(!g_stHdcpHandler[u8PortIdx].bRxHDCPStart)
        {
            HDCPMsgPool_SetPriority(u8PortIdx, 0); //clear priority;
            HDCPMBX_SetActiveBit(u8PortIdx, FALSE); //clear active bit;
            g_stHdcpHandler[u8PortIdx].bPolling = FALSE; //clear polling flag;
            if(g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_14)
            {
                HDCPTx_Hdcp1xSetENC_EN(u8PortIdx, FALSE);
                HDCPTx_Hdcp1xSetState(g_u8RxTxPortPair[u8PortIdx], EN_HDCP14TX_MS_A0, EN_HDCP14TX_SS_IDLE);
            }
            else if(g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_22)
            {
                HDCPMBX_SetCommonCmdState(EN_HDCPMBX_STATE_INIT);
                HDCPTx_Hdcp2EnableAuth(u8PortIdx, FALSE);
                g_stHdcpHandler[u8PortIdx].u8MainState = EN_HDCP2TX_MS_A0F0;
                g_stHdcpHandler[u8PortIdx].u8SubState = EN_HDCP2_MSG_ZERO;
                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
            }
            HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Pair Rx Doesn't Do HDCP!!");
            break;
        }
        else if(HDCPRx_GetEncryptionEnc(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]))
        {
            g_stHdcpHandler[u8PortIdx].bRxStartEnc = TRUE; // Set Rx Enc Receive
        }

        if( g_stHdcpHandler[u8PortIdx].bRxStartEnc)
        {
            if(!HDCPRx_GetEncryptionEnc(g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]))
            {
                if(u8EncDisCnt == DEF_DIS_ENC_CNT) // avoid opp window no enc, consecutive 20 times not receive ENC
                {
                    g_stHdcpHandler[u8PortIdx].bRxStartEnc = FALSE; // Set Rx Enc Receive
                    g_stHdcpHandler[u8PortIdx].bRxHDCPStart = FALSE;
                    HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Rx Start Not to Receive ENC!!");
                }
                u8EncDisCnt++;
                break;
            }
            u8EncDisCnt = 0;
        }
        #endif
		//printf("portid=%d//",u8PortIdx);
        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc == NULL)
        {
            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "NULL Function Pointer!!");
            bRet = EN_HDCP_ERR_PROCESS_FAIL;
            break;
        }

        if (g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_22)
        {//printf("enHdcpVerInfo == EN_HDCP_VER_22\n");
            if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_VER_14)
            {
                if(!g_stHdcpHandler[u8PortIdx].bAsRepeater) //Not Repeater
                {
                    g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_14;
                    break;
                }
                else
                {
                    if(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8HDCPRxVer != EN_HDCP_VER_22)  //Repeater but upstream not 22
                    {
                        g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_14;
                        break;
                    }
                }
            }
            if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), (BYTE)EN_HDCP2X_X74_OFFSET_RXSTATUS, EN_HDCP2_MSG_ZERO, (BYTE)EN_HDCP_OPCODE_READ, DEF_HDCP2_RXSTATUS_SIZE, (BYTE*)&u16RxStatus) == FALSE)
            {
                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Access I/O FAILED! PORT :%02X", u8PortIdx);
                bRet = EN_HDCP_ERR_PROCESS_FAIL;
                break;
            }
            if ((u16RxStatus & 0x0800) && g_stHdcpHandler[u8PortIdx].bSendAKEInit)//re-auth
            {
                HDCPTx_Hdcp2Init(u8PortIdx);
                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "Sink Assert Re-Auth Bit!");
                bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                break;
            }

            u16MsgLen = ((u16RxStatus & 0x3FF) > g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_CERT]) ? g_u16Hdcp2MsgArgLen[EN_HDCP2_MSG_AKE_SEND_CERT] : (u16RxStatus & 0x3FF);

            //repeater part
            if (u16RxStatus & 0x0400)
            {
                if (u16MsgLen == 0x00)
                {
                    if ((g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE) || (g_stHdcpHandler[u8PortIdx].bRetryAKEInit == TRUE)\
                        || (g_stHdcpHandler[u8PortIdx].bPolling == TRUE))
                    {
                        bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                        break;
                    }
                }
                else
                {
                    BYTE u8CmdID = 0;

                    if (((u16MsgLen - 1 - DEF_HDCP2_RXINFO_SIZE - (DEF_HDCP2_VPRIME_SIZE >> 1) - DEF_HDCP2_SEQ_NUM_V_SIZE) % DEF_HDCP2_RECV_ID_SIZE) == 0)
                    {
                        u8CmdID = EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST;
                    }
                    else
                    {
                        u8CmdID = EN_HDCP2_MSG_REPAUTH_STREAM_READY;
                    }

                    //fetch message
                    if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), (BYTE)EN_HDCP2X_X74_OFFSET_READMSG, u8CmdID, (BYTE)EN_HDCP_OPCODE_READ, \
                        u16MsgLen, (BYTE*)((ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_CMD_ID_IDX))) == FALSE)
                    {
                        HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Access x74 FAILED!");
                        bRet = EN_HDCP_ERR_PROCESS_FAIL;
                        break;
                    }

                    HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_BLUE, "<-- [REP]Recv MsgID %d, Len = 0x%X", (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+ DEF_HDCPMSG_BUF_CMD_ID_IDX)), u16MsgLen);

                    if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX) == EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST)
                    {
                        if (u16MsgLen >= 0x03)
                        {
                        	if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                            {
                                memcpy((void*)g_stHdcpHandler[u8PortIdx].u8RxInfo, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+DEF_HDCPMSG_BUF_CMD_ID_IDX + 1), DEF_HDCP2_RXINFO_SIZE);
                                memcpy((void*)g_stHdcpHandler[u8PortIdx].u8RecvIDList, (void*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+DEF_HDCPMSG_BUF_CMD_ID_IDX + 1 + DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE >> 1)), g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE);
                            }
                            //check topology error
                            if ((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX + 2) & (BYTE)(DEF_HDCP2_MAX_DEV_EXCEEDED)) ||\
                                (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX + 2) & (BYTE)(DEF_HDCP2_MAX_CASCADE_EXCEEDED)))
                            {
                                HDCPTx_Hdcp2Init(u8PortIdx);
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Topology ERROR!");
								g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].bTx2RxInfo = TRUE;
                                bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                                break;
                            }

                            g_stHdcpHandler[u8PortIdx].bTopologyErr = FALSE;
                            g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt = (BYTE)((((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+DEF_HDCPMSG_BUF_CMD_ID_IDX + 1)) & 0x01) << 4) | \
                                (((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX + 2)) & 0xF0) >> 4));

                            u16TotalLen = 1 + DEF_HDCP2_RXINFO_SIZE + DEF_HDCP2_SEQ_NUM_V_SIZE + (DEF_HDCP2_VPRIME_SIZE>>1) + g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt*DEF_HDCP2_RECV_ID_SIZE;
                            HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_GREEN, "u8DownStreamDevCnt = %d, u16TotalLen = 0x%X", g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt, u16TotalLen);

                            if (u16TotalLen != u16MsgLen)
                            {
                                HDCPTx_Hdcp2Init(u8PortIdx);
                                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                                HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Incorrect Msg length!");
                                bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                                break;
                            }

                            HDCPTx_Hdcp2MsgParser(u8PortIdx, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX));

                            //process FSM
                            bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                            break;
                        }
                    }

                    if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX) == EN_HDCP2_MSG_REPAUTH_STREAM_READY)
                    {
                        //check if length is valid
                        if ((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX) >= DEF_HDCP2_CMD_NUM) || (g_u16Hdcp2MsgArgLen[*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX)] != u16MsgLen))
                        {
                            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Unknown Message ID!");
                            bRet = EN_HDCP_ERR_INVALID_ARG;
                            break;
                        }

                        HDCPTx_Hdcp2MsgParser(u8PortIdx, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX));

                        //process FSM
                        bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                        break;
                    }
                }
            }
            else
            {
                if(g_stHdcpHandler[u8PortIdx].bAsRepeater && !g_stHdcpHandler[u8PortIdx].bSinkIsRepeater)
                {
                    memset((void*)g_stHdcpHandler[u8PortIdx].u8RxInfo, 0x00, DEF_HDCP2_RXINFO_SIZE);
                    memset((void*)g_stHdcpHandler[u8PortIdx].u8RecvIDList, 0x00, DEF_HDCP2_MAX_DEVICE_COUNT*DEF_HDCP2_RECV_ID_SIZE);
                    g_stHdcpHandler[u8PortIdx].u8DownStreamDevCnt = 0;
                }
                if (g_stHdcpHandler[u8PortIdx].bSendAKEInit == TRUE)
                {
                    if (u16MsgLen == 0x00)
                    {
                        //g_stHdcpHandler[u8PortIdx].bRecvMsg = FALSE;
                        if ((g_stHdcpHandler[u8PortIdx].bWaitRxResponse == TRUE) || (g_stHdcpHandler[u8PortIdx].bRetryAKEInit == TRUE)\
                            || (g_stHdcpHandler[u8PortIdx].bPolling == TRUE))
                        {
                            bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                            break;
                        }
                    }
                    else
                    {
                        BYTE u8ListCnt = 0;

                        for (u8ListCnt = 0; u8ListCnt < DEF_HDCP2_CMD_NUM; u8ListCnt++)
                        {
                            if (g_u16Hdcp2MsgArgLen[u8ListCnt] == u16MsgLen)
                            {
                                break;
                            }
                        }

                        //fix coverity: avoid out-of-bound access
                        u8ListCnt = (u8ListCnt >=DEF_HDCP2_CMD_NUM) ? (DEF_HDCP2_CMD_NUM - 1) : u8ListCnt;

                        if (g_u16Hdcp2MsgArgLen[u8ListCnt] == 33) //H' or L'
                        {
                            u8ListCnt = (g_stHdcpHandler[u8PortIdx].bSendLCInit == FALSE) ? EN_HDCP2_MSG_AKE_SEND_H_PRIME : EN_HDCP2_MSG_LC_SEND_L_PRIME;
                        }

                        //fetch message
                        if (g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pTxIOCbFunc(GET_RECOVERY_TX_OFFSET(u8PortIdx), (BYTE)EN_HDCP2X_X74_OFFSET_READMSG, u8ListCnt, (BYTE)EN_HDCP_OPCODE_READ, \
                            u16MsgLen, (BYTE*)((ULONG)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX))) == FALSE)
                        {
                            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Access x74 FAILED!");
                            bRet = EN_HDCP_ERR_PROCESS_FAIL;
                            break;
                        }
                        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_BLUE, "<-- Recv MsgID %d, Len = 0x%X", *(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE)+DEF_HDCPMSG_BUF_CMD_ID_IDX), u16MsgLen);

                        //check if length is valid
                        if ((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX) >= DEF_HDCP2_CMD_NUM) || (g_u16Hdcp2MsgArgLen[*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX)] != u16MsgLen))
                        {
                            HLOGFC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_RED, "Unknown Message ID!");
                            bRet = EN_HDCP_ERR_INVALID_ARG;
                            break;
                        }

                        HDCPTx_Hdcp2MsgParser(u8PortIdx, (BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_CMD_ID_IDX));

                        //process FSM
                        bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                        break;
                    }//if (u16MsgLen == 0x00)
                }
                else
                {
                    bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                    break;
                }
            }
        }
        else
        {
            if (g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_FREERUN) //unknown capability
            {
                if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_VER_22)
                {
                    g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_22;

                }
                else if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_REFLECT_22)
                {
                    if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                    {
                        if(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8HDCPRxVer == EN_HDCP_VER_22)
                        {
                            g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_22;
                        }
                    }
                }
                else if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_VER_14)
                {
                    g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_14;
                    bRet = HDCPTx_Hdcp1xFSM(u8PortIdx);
                    break;
                }
                else if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_VER_FREERUN)
                {
                    if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                    {
                        if(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8HDCPRxVer == EN_HDCP_VER_22)
                        {
                            g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_22;
                        }
                    }
                }
                if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_VER_22)
                {
                    g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_22;

                }
                //get hdcp2.2 capability
                g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                g_stHdcpHandler[u8PortIdx].bRetryAKEInit = FALSE;
                HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);

                bRet = HDCPTx_Hdcp2FSM(u8PortIdx);
                break;
            }
            else if (g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_14) //Now 14
            {
                if(g_stHdcpHandler[u8PortIdx].enHdcpVerCtrl == EN_HDCP_VER_22)
                {
                    g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_22;
                    g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                    g_stHdcpHandler[u8PortIdx].bRetryAKEInit = FALSE;
                    HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                    break;
                }
                if(g_stHdcpHandler[u8PortIdx].bAsRepeater)
                {
                    if(g_stHdcpHandler[g_u8TxRxPortPair[GET_RECOVERY_TX_OFFSET(u8PortIdx)]].u8HDCPRxVer == EN_HDCP_VER_22)
                    {
                        g_stHdcpHandler[u8PortIdx].enHdcpVerInfo = EN_HDCP_VER_22;
                        g_stHdcpHandler[u8PortIdx].bSendAKEInit = FALSE;
                        g_stHdcpHandler[u8PortIdx].bRetryAKEInit = FALSE;
                        HDCPTx_Hdcp2SetState(u8PortIdx, EN_HDCP2TX_MS_A0F0, EN_HDCP2_MSG_ZERO);
                        break;
                    }
                }
                bRet = HDCPTx_Hdcp1xFSM(u8PortIdx);
            }
        } //if (g_stHdcpHandler[u8PortIdx].enHdcpVerInfo == EN_HDCP_VER_22)
    } while(FALSE);

    return bRet;
}

MS_U32 _HDCPTx_GetTimeDiff(MS_U32 dwPreTime, MS_U32 dwPostTime)
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
#if 0
ENUM_HDCP_ERR_CODE HDCPTx_HdcpEnable_ENC(BYTE u8PortIdx, BOOL bEnable, BYTE u8Version)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    BYTE* u8SendData = (BYTE*)IMIGetAddr(DEF_IMI_ENAUTH_ADDR);

    HTRACEE(EN_HLOG_HDCP2X);

    do
    {
        *(BYTE*)(u8SendData) = u8PortIdx;
        *(BYTE*)(u8SendData + 1) = bEnable;
        *(BYTE*)(u8SendData + 2) = u8Version;

        if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_HDCP_ENABLE_ENC, sizeof(BYTE), (ULONG)u8SendData) == FALSE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Set Mailbox Command Fail!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            break;
        }

        //g_stHdcpHandler[u8PortIdx].bPolling = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return enErrCode;

}
#endif
#endif //#if (ENABLE_SECU_R2 == 1) && (ENABLE_HDCP22 == 1)
#endif //#ifndef HDCP_TX_C
