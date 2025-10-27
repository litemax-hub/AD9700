#ifndef HDCP_HANDLER_C
#define HDCP_HANDLER_C
//----------------------------------------------------------------------//
// includes
//----------------------------------------------------------------------//
#include <string.h>
#include "datatype.h"
#include "Global.h"
//[MT9700]#include "mapi_combo.h"

#include "HdcpCommon.h"
#include "HdcpHandler.h"
#include "HdcpMbx.h"
#include "HdcpLog.h"
#include "HdcpMsgPool.h"
#include "HdcpError.h"
#include "HdcpRx.h"
#include "HdcpTx.h"
#include "drv_Hdcp_IMI.h"//[MT9700]
#include "drvIMI.h"
#include "drvGlobal.h"
#include "mdrv_hdmiRx.h"
#include "msEread.h"

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HDCP_TX_ON_DP_IDX       COMBO_IP_SUPPORT_TYPE
#define DEF_HDCP_TX_TASK_PERIOD     1

#define DEF_HDCP_HANDLER_INTERVAL   5
#define DPTX_OUTPUT_PORT_OFFSET     DEF_HDCP_TX_ON_DP_IDX
#define TX_OFFSET                   DEF_HDCP_TX_ON_DP_IDX

#define DEF_HDCP_RX_SUPPORT_EESS    FALSE


#define GET_TX_OUTPUT_OFFSET(x)     (x+TX_OFFSET)
#define GET_DPTX_PORTIDX(x)         (x+DPTX_OUTPUT_PORT_OFFSET) //DP HDCP internal port
//#define DEF_HDCPMSG_MAX_PRIORITY (2*DEF_HDCP_PORT_NUM + 1)
//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//
static BOOL                 g_bHdcpRxInitDoneFlag = FALSE;
static DWORD                g_u32IntervalCnt = 0;
BYTE                        g_u8HdcpTxAuthRetryCnt = 0;
//TBD: for MST9U4 project, there is only 2 Tx port; we will treat it as optional function and using fix port index to implement;
//static BOOL                 g_bHdcpTxTaskTimerStart = FALSE;
const BYTE                  g_u8DpTxPortIdx = DEF_HDCP_TX_ON_DP_IDX;
BYTE                        g_u8SecR2EventFlag = 0;
BYTE                        g_u8SecR2RomFlag=0;
WORD                        g_u16HdcpTxTaskTimerCnt = 0;
#if (COMBO_HDCP2_FUNCTION_SUPPORT)
volatile ST_HDCP_HANDLER    g_stHdcpHandler[DEF_HDCP_PORT_NUM];
#endif
//[MT9700]extern BYTE               g_u8TxRxPortPair[COMBO_OP_SUPPORT_TYPE];
//[MT9700]extern BYTE               g_u8RxTxPortPair[COMBO_IP_SUPPORT_TYPE];
volatile U8*              g_u8HdcpRecvBuf = (U8*)(IMIGetAddr(DEF_IMI_RECVBUFF_ADDR));//bit 7 of byte 0 is processing bit
volatile U8*              g_u8HdcpTransBuf = (U8*)(IMIGetAddr(DEF_IMI_TRANSBUFF_ADDR));

#if (ENABLE_LOAD_KEY_VIA_EFUSE == 0)
#warning "<<< ENABLE_LOAD_KEY_VIA_EFUSE - load internal hdcp14 Key>>>"
#endif

BYTE XDATA tCOMBO_HDCP14_BKSV[DEF_HDCP1X_KSV_SIZE] =
{
    0
};

BYTE XDATA pDPHDCP14Key[DEF_HDCP1X_RX_KEY_SIZE] =
{
    0
};

const WORD g_u16Hdcp2MsgArgLen[DEF_HDCP2_CMD_NUM] =
{
    0,      //na
    0,      //NULL_MESSAGE = 1,
    12,     //AKE_INIT = 2,
    534,    //AKE_SEND_CERT = 3,
    129,    //AKE_NO_STORED_KM = 4,
    33,     //AKE_STORED_KM = 5,
    0,      //AKE_SEND_RRX = 6,
    33,     //AKE_SEND_H_PRIME = 7,
    17,     //AKE_SEND_PAIRING_INFO = 8,
    9,      //LC_INIT = 9,
    33,     //LC_SEND_L_PRIME =10,
    25,     //SKE_SEND_EKS = 11,
    3,      // 3 + (19+DeviceCount*5); REPEATERAUTH_SEND_RECEIVERID_LIST = 12,
    0,      //RTT_READY = 13,
    0,     //RTT_CHALLENGE = 14,
    17,     //REPEATERAUTH_SEND_ACK = 15,
    8,      //6+2*k, REPEATERAUTH_STREAM_MANAGE = 16,
    33,     //REPEATERAUTH_STREAM_READY = 17,
    0,      //RECEIVER_AUTHSTATUS = 18,
    0,      //AKE_TRANSMITTER_INFO = 19,
    0,      //AKE_RECEIVER_INFO = 20,
};
#if (COMBO_HDCP2_FUNCTION_SUPPORT)
//----------------------------------------------------------------------//
// functions
//----------------------------------------------------------------------//
BOOL HDCPHandler_AttachCBFunc(BYTE u8PortIdx, HDCPRX_IO_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        if (u8PortIdx >= DEF_HDCP_PORT_NUM)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        g_stHdcpHandler[u8PortIdx].pHdcpIOCbFunc.pRxSendFunc = pFunc;

        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return bRet;
}

BOOL HDCPHandler_AttachTxIOCBFunc(BYTE u8PortIdx, HDCPTX_IO_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        if ((GET_TX_OUTPUT_OFFSET(u8PortIdx) >= DEF_HDCP_PORT_NUM) || (GET_TX_OUTPUT_OFFSET(u8PortIdx) < DEF_HDCP_TX_ON_DP_IDX))
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "I/O Callback Function Initialized!");

        g_stHdcpHandler[GET_TX_OUTPUT_OFFSET(u8PortIdx)].pHdcpIOCbFunc.pTxIOCbFunc = pFunc;

        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return bRet;
}

BOOL HDCPHandler_AttachTxCompareRiFunc(BYTE u8PortIdx, HDCPTX_COMPARE_RI_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        if ((GET_TX_OUTPUT_OFFSET(u8PortIdx) >= DEF_HDCP_PORT_NUM) || (GET_TX_OUTPUT_OFFSET(u8PortIdx) < DEF_HDCP_TX_ON_DP_IDX))
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "CompareRi Callback Function Initialized!");

        g_stHdcpHandler[GET_TX_OUTPUT_OFFSET(u8PortIdx)].pCompareRiFunc = pFunc;

        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return bRet;
}

BOOL HDCPHandler_AttachTxGetDPTXR0Func(BYTE u8PortIdx, HDCPTX_GET_DPTXR0_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        if ((GET_TX_OUTPUT_OFFSET(u8PortIdx) >= DEF_HDCP_PORT_NUM) || (GET_TX_OUTPUT_OFFSET(u8PortIdx) < DEF_HDCP_TX_ON_DP_IDX))
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "CompareRi Callback Function Initialized!");

        g_stHdcpHandler[GET_TX_OUTPUT_OFFSET(u8PortIdx)].pGetDPTxR0Func = pFunc;

        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return bRet;
}



BOOL HDCPHandler_AttachWriteX74Func(BYTE u8PortIdx, HDCPRX_WRITEX74_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (u8PortIdx >= DEF_HDCP_PORT_NUM)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "WriteX74 Callback Function Initialized!");

        g_stHdcpHandler[u8PortIdx].pWriteX74Func = pFunc;

        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}

BOOL HDCPHandler_AttachReadX74Func(BYTE u8PortIdx, HDCPRX_READX74_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (u8PortIdx >= DEF_HDCP_PORT_NUM)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "ReadX74 Callback Function Initialized!");

        g_stHdcpHandler[u8PortIdx].pReadX74Func = pFunc;

        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}


BOOL HDCPHandler_AttachRepKSVFIFOFunc(BYTE u8PortIdx, HDCPREP_KSVFIFO_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (u8PortIdx >= DEF_HDCP_PORT_NUM)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "RepKSVFIFO Callback Function Initialized!");

        g_stHdcpHandler[u8PortIdx].pKSVFIFOFunc = pFunc;
        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}

BOOL HDCPHandler_AttachRxHDCPCtrlFunc(BYTE u8PortIdx, HDCPREP_RX_HPDCTRL_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP);
    do
    {
        if (u8PortIdx >= DEF_HDCP_PORT_NUM)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        HLOGDC(EN_HLOG_HDCP2X, DEF_COLOR_YELLOW, "RxHDCPCtrl Callback Function Initialized!");

        g_stHdcpHandler[u8PortIdx].pRxHPDCTRLFunc = pFunc;

        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP);

    return bRet;
}
#endif
void HDCPHandler_SetHdcpRetryCount(U8 u8RetryCnt)
{
    g_u8HdcpTxAuthRetryCnt = u8RetryCnt;
}

ENUM_HDCP_ERR_CODE HDCPHandler_SECU_PowerDown(BOOL bSetPowerDown)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

	memcpy((void*)((g_u8HdcpRecvBuf +  DEF_IMI_SECU_POWERDOWN_ADDR)),(void*)&bSetPowerDown, 1);

    do
    {

		HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "HDCPHandler_SECU_PowerDown");

        if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_SECU_POWER_DOWN, sizeof(BYTE), (ULONG)(g_u8HdcpRecvBuf + DEF_IMI_SECU_POWERDOWN_ADDR) )== FALSE)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Send HDCPHandler_SECU_PowerDown FAIL");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
        }
    } while (FALSE);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPHandler_SECU_SET_HDMI_DP(BOOL HDMI)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

	memcpy((void*)((g_u8HdcpRecvBuf +  DEF_IMI_HDMI_DP_ADDR)),(void*)&HDMI, 1);

    do
    {

		HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "HDCPHandler_SECU_SET_HDMI_DP");

        if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_SECU_IS_HDMI, sizeof(BYTE), (ULONG)(g_u8HdcpRecvBuf + DEF_IMI_HDMI_DP_ADDR) )== FALSE)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Send HDCPHandler_SECU_SET_HDMI_DP FAIL");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
        }
    } while (FALSE);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPHandler_DecodeHdcp1TxKey(BYTE* pu8Hdcp1TxKey)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    do
    {
        if (pu8Hdcp1TxKey == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL KEY data!");
            enErrCode = EN_HDCP_ERR_INVALID_ARG;
        }

        if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_LOAD_HDCP1X_KEY, DEF_SIZE_OF_HDCP1X_KEY+1, (ULONG)pu8Hdcp1TxKey)!= TRUE)
        {
            HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "Send Decode Key Command FAIL!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
        }
    } while (FALSE);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPHandler_DecodeHdcp2RxKey(BYTE* pu8Hdcp2RxKey, BOOL bUseCustomizeKey)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;
    do
    {
        if (pu8Hdcp2RxKey == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL KEY data!");
            enErrCode = EN_HDCP_ERR_INVALID_ARG;
        }

        if (bUseCustomizeKey == TRUE)
        {
            if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_LOAD_HDCP2X_RX_KEY_WITH_CUSTOMIZE_KEY, DEF_SEED_KEY_SIZE + DEF_HDCP2_CERT_SIZE + DEF_HDCP2_RX_ENCODE_KEY_SIZE+1, (ULONG)pu8Hdcp2RxKey)!= TRUE)
            {
                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Send [EN_HDCPMBX_CC_LOAD_HDCP2X_RX_KEY_WITH_CUSTOMIZE_KEY] Command FAIL!");
                enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            }
        }
        else
        {
            if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_LOAD_HDCP2X_RX_KEY, DEF_HDCP2_CERT_SIZE + DEF_HDCP2_RX_ENCODE_KEY_SIZE+1, (ULONG)pu8Hdcp2RxKey)!= TRUE)
            {
                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Send Decode Key Command FAIL!");
                enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            }
        }
    } while (FALSE);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPHandler_DecodeHdcp2TxKey(BYTE* pu8Hdcp2TxKey, BOOL bUseCustomizeKey)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    do
    {
        if (pu8Hdcp2TxKey == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL KEY data!");
            enErrCode = EN_HDCP_ERR_INVALID_ARG;
        }

        if (bUseCustomizeKey == TRUE)
        {
            if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_LOAD_HDCP2X_TX_KEY_WITH_CUSTOMIZE_KEY, DEF_SEED_KEY_SIZE + DEF_HDCP2_TX_ENCODE_KEY_SIZE+1, (ULONG)pu8Hdcp2TxKey)!= TRUE)
            {
                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Send [EN_HDCPMBX_CC_LOAD_HDCP2X_TX_KEY_WITH_CUSTOMIZE_KEY] Command FAIL!");
                enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            }
        }
        else
        {
            if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_LOAD_HDCP2X_TX_KEY, DEF_HDCP2_TX_ENCODE_KEY_SIZE+1, (ULONG)pu8Hdcp2TxKey)!= TRUE)
            {
                HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Send Decode Key Command FAIL!");
                enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
            }
        }
    } while (FALSE);

    return enErrCode;
}

ENUM_HDCP_ERR_CODE HDCPHandler_EncryptHdcp2RxKey(BYTE* pu8Hdcp2TxKey)
{
    ENUM_HDCP_ERR_CODE enErrCode = EN_HDCP_ERR_NONE;

    do
    {
        if (pu8Hdcp2TxKey == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL RAWKEY data!");
            enErrCode = EN_HDCP_ERR_INVALID_ARG;
        }

        if (HDCPMBX_SetCommonCmd(EN_HDCPMBX_CC_LOAD_HDCP2X_RX_RAWKEY, DEF_HDCP2_RX_KEY_NON_PADDING_SIZE, (ULONG)pu8Hdcp2TxKey)!= TRUE)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Send [EN_HDCPMBX_CC_LOAD_HDCP2X_RX_RAWKEY] Command FAIL!");
            enErrCode = EN_HDCP_ERR_SET_MBX_CMD_FAIL;
        }
    } while (FALSE);

    return enErrCode;
}

#if 0
void HDCPHandler_EnableHdcpTxOnHDMI(BYTE u8PortIdx, BYTE enHdcpVerCtrl, BOOL bEnable)
{
    HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_GREEN, "%s HdcpTx on HDMI Tx Port %d", bEnable ? "Enable" : "Disable", GET_HDMITX_PORTIDX(u8PortIdx));
    g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].bEnableHdcp = bEnable;
    //g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].bAsRepeater = bEnable ? TRUE : FALSE;
    //g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].pWriteX74Func(GET_HDMITX_PORTIDX(u8PortIdx), EN_HDCP1X_X74_OFFSET_BCAPS, DEF_HDCP1X_BCAPS_SIZE, bEnable ? 0x40 : 0x00);
    if (bEnable == FALSE)
    {
        HDCPMsgPool_SetPriority(GET_HDMITX_PORTIDX(u8PortIdx), 0); //clear priority;
        HDCPMBX_SetActiveBit(GET_HDMITX_PORTIDX(u8PortIdx), FALSE); //clear active bit;
        g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].bPolling = FALSE; //clear polling flag;
        //HDCPTx_HdcpEnable_ENC(GET_HDMITX_PORTIDX(u8PortIdx), FALSE, enHdcpVerCtrl);
        #if 1
        if(g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].enHdcpVerInfo == EN_HDCP_VER_14)
        {
            HDCPTx_Hdcp1xSetENC_EN(GET_HDMITX_PORTIDX(u8PortIdx), FALSE);
        }
        else if(g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].enHdcpVerInfo == EN_HDCP_VER_22)
        {
            HDCPTx_Hdcp2EnableAuth(GET_HDMITX_PORTIDX(u8PortIdx), FALSE);
        }
        #endif
        HDCPTx_Hdcp2Init(GET_HDMITX_PORTIDX(u8PortIdx));
    }
    else
    {
        if(enHdcpVerCtrl == EN_HDCP_VER_22)
        {
            g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].u8MainState = EN_HDCP2TX_MS_A0F0;
            g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].u8SubState= EN_HDCP2_MSG_ZERO;
        }
        g_stHdcpHandler[GET_HDMITX_PORTIDX(u8PortIdx)].enHdcpVerInfo = enHdcpVerCtrl;
    }
}


void HDCPHandler_EnableHdcpTxOnDP(BYTE u8PortIdx, BYTE enHdcpVerCtrl, BOOL bEnable)
{
    HLOGIC(EN_HLOG_HDCP2X, DEF_COLOR_LIGHT_GREEN, "%s HdcpTx on DP Tx Port %d", bEnable ? "Enable" : "Disable", GET_DPTX_PORTIDX(u8PortIdx));
    g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].bEnableHdcp = bEnable;
    //g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].enHdcpVerInfo = enHdcpVerCtrl;

    if (bEnable == FALSE)
    {
        HDCPMsgPool_SetPriority(GET_DPTX_PORTIDX(u8PortIdx), 0); //clear priority;
        HDCPMBX_SetActiveBit(GET_DPTX_PORTIDX(u8PortIdx), FALSE); //clear active bit;
        g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].bPolling = FALSE; //clear polling flag;
        //HDCPTx_HdcpEnable_ENC(GET_HDMITX_PORTIDX(u8PortIdx), FALSE, enHdcpVerCtrl);
        #if 1
        if(g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].enHdcpVerInfo == EN_HDCP_VER_14)
        {
            HDCPTx_Hdcp1xSetENC_EN(GET_DPTX_PORTIDX(u8PortIdx), FALSE);
        }
        else if(g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].enHdcpVerInfo == EN_HDCP_VER_22)
        {
            HDCPTx_Hdcp2EnableAuth(GET_DPTX_PORTIDX(u8PortIdx), FALSE);
        }
        #endif
        HDCPTx_Hdcp2Init(GET_DPTX_PORTIDX(u8PortIdx));
    }
    else
    {
        if(enHdcpVerCtrl == EN_HDCP_VER_22)
        {
            g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].u8MainState = EN_HDCP2TX_MS_A0F0;
            g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].u8SubState = EN_HDCP2_MSG_ZERO;
        }
        g_stHdcpHandler[GET_DPTX_PORTIDX(u8PortIdx)].enHdcpVerInfo = enHdcpVerCtrl;
    }
}
#endif

#if 0//[MT9700]
BYTE HDCPHandler_TxVerInfo(BYTE u8RxPortIdx)
{
    BYTE bRet = EN_HDCP_VER_FREERUN;
    if(g_stHdcpHandler[g_u8RxTxPortPair[u8RxPortIdx]].bAsRepeater)
    {
        bRet = g_stHdcpHandler[g_u8RxTxPortPair[u8RxPortIdx]].enHdcpVerInfo;
    }
    return bRet;
}
#endif
void HDCPHandler_SetR2EventFlag(void)
{
    //printf("====1g_bHdcpRxInitDoneFlag is %d===\n",g_bHdcpRxInitDoneFlag);

    if (g_bHdcpRxInitDoneFlag == FALSE)
    {
        //printf("====g_u8SecR2EventFlag is %d===\n",g_u8SecR2EventFlag);
        g_u8SecR2EventFlag = 1;
    }
    else
    {
        #if (COMBO_HDCP2_FUNCTION_SUPPORT)
        HDCP_Handler();
        #endif
        g_u8SecR2EventFlag = 1;
    }
    //printf("====2g_u8SecR2EventFlag is %d===\n",g_u8SecR2EventFlag);
}
void HDCPHandler_SetSecR2EventFlag(void)
{
    g_u8SecR2EventFlag=TRUE;
}

BOOL HDCPHandler_GetR2EventFlag(void)
{

    //printf("====3g_u8SecR2EventFlag is %d===\n",g_u8SecR2EventFlag);
    if (g_u8SecR2EventFlag == 0)
    {
        return FALSE;
    }
    else
    {
        g_u8SecR2EventFlag = 0;
        printf("====3g_u8SecR2EventFlag is %d===\n",g_u8SecR2EventFlag);
        return TRUE;
    }

}

BOOL HDCPHandler_UpdateRomR2FirstEventFlag(void)
{
    if (g_u8SecR2RomFlag == 0)
    {
        if(msReadByte(0x10054C) & BIT1)
        {
            g_u8SecR2RomFlag=1;
            return TRUE;
        }
    }else{

            return FALSE;
    }
    return FALSE;
}
BOOL HDCPHandler_GetRomR2FirstEventFlag(void)
{
    return g_u8SecR2RomFlag;
}

void HDCPHandler_ClrRomR2FirstEventFlag(void)
{
    printf("g_u8SecR2RomFlag=0\n");
    g_u8SecR2RomFlag=0;
}


ULONG HDCPHandler_GetTxBufAddr(void)
{
    return (ULONG)g_u8HdcpTransBuf;
}
#if (COMBO_HDCP2_FUNCTION_SUPPORT)
BOOL HDCPHandler_Hdcp2MsgDispatcher(BYTE ucPortIdx, BYTE *pucMsg)
{
    BOOL bMsgParser = FALSE;

    HLOGVC(EN_HLOG_HDCP2X,  DEF_COLOR_YELLOW, "Port %d, MsgID: %d", ucPortIdx, *pucMsg);

    #if 0
    if (g_stHdcpHandler[ucPortIdx].enHdcpType == EN_HDCPMBX_PORT_TYPE_SOURCE)
    {
        HDCPHandler_HdcpTxMsgParser(ucPortIdx, pucMsg);
    }
    else
    #else
    if (g_stHdcpHandler[ucPortIdx].enHdcpType == EN_HDCPMBX_PORT_TYPE_SINK)
    #endif
    {
        if(HDCPRx_Hdcp2MsgParser(ucPortIdx, pucMsg))
        {
            HDCPRx_Handler(ucPortIdx);
            bMsgParser = TRUE;
        }
        //HDCPHandler_HdcpRxMsgParser(ucPortIdx, pucMsg);
    }

    return bMsgParser;
}
#endif
void HDCPHandler_Hdcp14BKSVRead(BYTE ucPortIdx) //Rx redo 14 need to re-trigger Tx auth
{
    HLOGVC(EN_HLOG_HDCP,  DEF_COLOR_YELLOW, "BKSV Read Port %d", ucPortIdx);
    HDCPRepeater_ReStart14TxAuth(ucPortIdx);
}

BOOL HDCPHandler_Hdcp14CheckVPrimePrepared(BYTE ucPortIdx)  // Rx is prepared to Set Rdy Bit R0' has been read
{
    HLOGVC(EN_HLOG_HDCP,  DEF_COLOR_YELLOW, "Rdy Bit Check Port %d", ucPortIdx);
    return HDCPRepeater_TxSetRxRdyBit(ucPortIdx);
}

#if 0
BOOL HDCPHandler_AttachFetchMsgCBFunc(BYTE u8PortIdx, HDCPRX_FETCH_MSG_FUNC pFunc)
{
    BOOL bRet = FALSE;

    HTRACEE(EN_HLOG_HDCP2X);
    do
    {
        if (u8PortIdx >= DEF_HDCP_PORT_NUM)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "Invalid Port Index!");
            break;
        }

        if (pFunc == NULL)
        {
            HLOGEC(EN_HLOG_HDCP2X, DEF_COLOR_RED, "NULL function pointer");
            break;
        }

        g_stHdcpHandler[u8PortIdx].pFetchMsgFunc = pFunc;
        bRet = TRUE;
    } while(FALSE);

    HTRACEL(EN_HLOG_HDCP2X);

    return bRet;
}

void HDCPHandler_Hdcp2RxISR(void)
{
    BYTE u8PortCnt = 0;
    WORD u16BankOffset[4] = {0x000, 0x300, 0x800, 0xb00}; //hdcp dual bank : 1712, 1715, 171A, 171D
    WORD u16DPBankOffset[3] = {0x000, 0x900, 0x1200};     //DP hdcp bank : 160A, 1613, 161C

    for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++ )
    {
        if ((u8PortCnt == g_u8HdmiTxPortIdx) || (u8PortCnt == g_u8DpTxPortIdx))
        {
            // do nothing
        }
        else if (u8PortCnt < 4)
        {
            if ((msReadByte(0x1712CE + u16BankOffset[u8PortCnt]) & 0x08) == 0x00) // read_done mask is open
            {
                if (msReadByte(REG_1712CC + u16BankOffset[u8PortCnt]) & BIT3)
                {
                    //turn off mask
                    msWriteByte(REG_170D61, msReadByte(REG_170D61) & (~BIT4)); //dvi_hdmi_hdcp_int;
                    msWriteByte(REG_1712CE + u16BankOffset[u8PortCnt], msReadByte(0x1712CE + u16BankOffset[u8PortCnt])|(BIT3 | BIT2));

                    //clear status;
                    msWriteByte(REG_1712CC + u16BankOffset[u8PortCnt], 0x1C);
                    msWriteByteMask(REG_171079 + u16BankOffset[u8PortCnt], 0x40, 0x40);
                    msWriteByteMask(REG_171079 + u16BankOffset[u8PortCnt], 0x00, 0x40);

                    HDCPRx_Handler(u8PortCnt); //get read done status; try to send pairing info;

                    //turn on mask
                    msWriteByte(REG_170D61, msReadByte(0x170D61) | BIT4); //dvi_hdmi_hdcp_int;
                    msWriteByte(REG_1712CE + u16BankOffset[u8PortCnt], msReadByte(0x1712CE + u16BankOffset[u8PortCnt])&(~BIT2));
                }
                else if (msReadByte(REG_1712CC + u16BankOffset[u8PortCnt]) & BIT2)
                {
                    //turn off mask
                    msWriteByte(REG_170D61, msReadByte(REG_170D61) & (~BIT4)); //dvi_hdmi_hdcp_int;
                    msWriteByte(REG_1712CE + u16BankOffset[u8PortCnt], msReadByte(0x1712CE + u16BankOffset[u8PortCnt])|(BIT3 | BIT2));

                    //clear status
                    msWriteByte(REG_1712CC + u16BankOffset[u8PortCnt], 0x1C); // hdcp_dual_66[2]: clear b'2 wirte done status; b'3 read one; b'4 write start
                    msWriteByteMask(REG_171079 + u16BankOffset[u8PortCnt], 0x40, 0x40);
                    msWriteByteMask(REG_171079 + u16BankOffset[u8PortCnt], 0x00, 0x40);

                    g_bHdcpRxInitDoneFlag = TRUE;

                    if (g_stHdcpHandler[u8PortCnt].pFetchMsgFunc != NULL)
                    {
                        g_stHdcpHandler[u8PortCnt].pFetchMsgFunc(u8PortCnt);
                    }

                    //turn on mask
                    msWriteByte(REG_170D61, msReadByte(REG_170D61) | BIT4); //dvi_hdmi_hdcp_int;
                    msWriteByte(REG_1712CE + u16BankOffset[u8PortCnt], msReadByte(0x1712CE + u16BankOffset[u8PortCnt])&(~BIT2));
                }
                else
                {
                    //do nothing
                }
                break;
            }
            else
            {
                if (msReadByte(REG_1712CC + u16BankOffset[u8PortCnt]) & BIT2)
                {
                    //turn off mask
                    msWriteByte(REG_170D61, msReadByte(REG_170D61) & (~BIT4)); //dvi_hdmi_hdcp_int;
                    msWriteByte(REG_1712CE + u16BankOffset[u8PortCnt], msReadByte(0x1712CE + u16BankOffset[u8PortCnt])|BIT2);

                    //clear status
                    msWriteByte(REG_1712CC + u16BankOffset[u8PortCnt], 0x1C); // hdcp_dual_66[2]: clear b'2 wirte done status; b'3 read one; b'4 write start
                    msWriteByteMask(REG_171079 + u16BankOffset[u8PortCnt], 0x40, 0x40);
                    msWriteByteMask(REG_171079 + u16BankOffset[u8PortCnt], 0x00, 0x40);

                    g_bHdcpRxInitDoneFlag = TRUE;

                    if (g_stHdcpHandler[u8PortCnt].pFetchMsgFunc != NULL)
                    {
                        g_stHdcpHandler[u8PortCnt].pFetchMsgFunc(u8PortCnt);
                    }

                    //turn on mask
                    msWriteByte(REG_170D61, msReadByte(REG_170D61) | BIT4); //dvi_hdmi_hdcp_int;
                    msWriteByte(REG_1712CE + u16BankOffset[u8PortCnt], msReadByte(0x1712CE + u16BankOffset[u8PortCnt])&(~BIT2));
                    break;
                }
            }
        }
		else
		{
            if ((u8PortCnt == g_u8HdmiTxPortIdx) || (u8PortCnt == g_u8DpTxPortIdx))
            {
                // do nothing
            }
            else if(msReadByte(REG_DPRX_HDCP22_4_78_H + u16DPBankOffset[u8PortCnt-4]) &BIT0) // REG_DPCD_HDCP22_4_78[8]: Read down flag
			{
				HDCPRx_Handler(u8PortCnt); //get read done status; try to send pairing info;

				if(msReadByte(REG_DPRX_TOP_GOP_30_L) & (BIT0 << (u8PortCnt-4)))
				{
					msWriteByteMask(REG_DPRX_TOP_GOP_38_L , (BIT0 << (u8PortCnt-4)), (BIT0 << (u8PortCnt-4))); // REG_DPRX_TOP_GOP_38_L[2:0]: RX AUX 0~2 R2 IRQ
					msWriteByteMask(REG_DPRX_TOP_GOP_38_L , 0, (BIT0 << (u8PortCnt-4))); // REG_DPRX_TOP_GOP_38_L[2:0]:  RX AUX 0~2 R2 IRQ
					msWriteByteMask(REG_DPRX_DPCD1_31_H + u16DPBankOffset[u8PortCnt-4], BIT5, BIT5); // REG_DPRX_DPCD1_31_H[5]: clear (HDCP22) INT
					msWriteByteMask(REG_DPRX_DPCD1_31_H + u16DPBankOffset[u8PortCnt-4], 0, BIT5);	 // REG_DPRX_DPCD1_31_H[5]: enable (HDCP22) INT
				    msWriteByteMask(REG_DPRX_HDCP22_4_78_H + u16DPBankOffset[u8PortCnt-4], BIT3, BIT3); // REG_DPCD_HDCP22_4_78[11]: clear	H' read Down flag INT
				    msWriteByteMask(REG_DPRX_HDCP22_4_78_H + u16DPBankOffset[u8PortCnt-4], 0, BIT3); // REG_DPCD_HDCP22_4_78[11]: clear  H' read Down flag INT
				}
			}
			else
			{
				if(msReadByte(REG_DPRX_TOP_GOP_30_L) & (BIT2|BIT1|BIT0))// REG_DPRX_TOP_GOP_30_L[2:0]:  RX AUX 0~2 R2 IRQ
				{
					if(msReadByte(REG_DPRX_DPCD1_2D_H + u16DPBankOffset[u8PortCnt-4]) &BIT5) //[13] dpcd_hdcp22_irq
					{
						if(msReadByte(REG_DPRX_HDCP22_4_70_L + u16DPBankOffset[u8PortCnt-4]) &BIT1) // REG_DPCD_HDCP22_4_70[1], reg_ake_init_irq
						{
							msWriteByteMask(REG_DP_MST1_00_H + (2*(u8PortCnt-4)), 2, 0x1F);
							msWriteByteMask(REG_DPRX_HDCP22_4_4F_L + u16DPBankOffset[u8PortCnt-4], 0, (BIT4|BIT3)); // REG_DPCD_HDCP22_4_4F[7:0]: reg_rx_status
						}

						if(msReadByte(REG_DPRX_HDCP22_4_70_L + u16DPBankOffset[u8PortCnt-4]) &BIT2) // REG_DPCD_HDCP22_4_73[2], reg_ake_no_stored_km_irq
						{
							msWriteByteMask(REG_DP_MST1_00_H + (2*(u8PortCnt-4)), 4, 0x1F);
						}

						if(msReadByte(REG_DPRX_HDCP22_4_70_L + u16DPBankOffset[u8PortCnt-4]) &BIT3) // REG_DPCD_HDCP22_4_73[3], reg_ake_stored_km_irq
						{
							msWriteByteMask(REG_DP_MST1_00_H + (2*(u8PortCnt-4)), 5, 0x1F);
						}

						if(msReadByte(REG_DPRX_HDCP22_4_70_L + u16DPBankOffset[u8PortCnt-4]) &BIT4) // REG_DPCD_HDCP22_4_73[4], reg_locality_check_irq
						{
							msWriteByteMask(REG_DP_MST1_00_H + (2*(u8PortCnt-4)), 9, 0x1F);
						}

						if(msReadByte(REG_DPRX_HDCP22_4_70_L + u16DPBankOffset[u8PortCnt-4]) &BIT5) // REG_DPCD_HDCP22_4_73[5], reg_ske_irq
						{
							msWriteByteMask(REG_DP_MST1_00_H + (2*(u8PortCnt-4)), 11, 0x1F);
						}

						g_bHdcpRxInitDoneFlag = TRUE;

						if (g_stHdcpHandler[u8PortCnt].pFetchMsgFunc != NULL)
						{
							g_stHdcpHandler[u8PortCnt].pFetchMsgFunc(u8PortCnt);
						}

						if(msReadByte(REG_DPRX_TOP_GOP_30_L) & (BIT0 << (u8PortCnt-4)))
						{
							msWriteByteMask(REG_DPRX_TOP_GOP_38_L , (BIT0 << (u8PortCnt-4)), (BIT0 << (u8PortCnt-4))); // REG_DPRX_TOP_GOP_38_L[2:0]: RX AUX 0~2 R2 IRQ
							msWriteByteMask(REG_DPRX_TOP_GOP_38_L , 0, (BIT0 << (u8PortCnt-4))); // REG_DPRX_TOP_GOP_38_L[2:0]:  RX AUX 0~2 R2 IRQ
							msWriteByteMask(REG_DPRX_DPCD1_31_H + u16DPBankOffset[u8PortCnt-4], BIT5, BIT5); // REG_DPRX_DPCD1_31_H[5]: clear (HDCP22) INT
							msWriteByteMask(REG_DPRX_DPCD1_31_H + u16DPBankOffset[u8PortCnt-4], 0, BIT5);	 // REG_DPRX_DPCD1_31_H[5]: enable (HDCP22) INT
							msWriteByteMask(REG_DPRX_HDCP22_4_73_L + u16DPBankOffset[u8PortCnt-4], (BIT5|BIT4|BIT3|BIT2|BIT1), (BIT5|BIT4|BIT3|BIT2|BIT1)); // REG_DPCD_HDCP22_4_73[1]: reg_ake_init_irq_clr
																																							// REG_DPCD_HDCP22_4_73[2], reg_ake_no_stored_km_irq
																																							// REG_DPCD_HDCP22_4_73[3], reg_ake_stored_km_irq
																																							// REG_DPCD_HDCP22_4_73[4], reg_locality_check_irq
																																							// REG_DPCD_HDCP22_4_73[5], reg_ske_irq
							msWriteByteMask(REG_DPRX_HDCP22_4_73_L + u16DPBankOffset[u8PortCnt-4], 0, (BIT5|BIT4|BIT3|BIT2|BIT1)); // REG_DPCD_HDCP22_4_73[1]: reg_ake_init_irq_clr
						}
					}
				}
		    }
		}
    }
}
#endif
#if (COMBO_HDCP2_FUNCTION_SUPPORT)
void HDCP_Handler(void)
{
    BYTE u8PortCnt = 0;
    //for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++ )
    {
        if (g_stHdcpHandler[u8PortCnt].enHdcpType == EN_HDCPMBX_PORT_TYPE_SINK)
        {
            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE)) != 0x00) //TBD: fetch message by priority
            {
                HDCPRx_Handler(u8PortCnt);
            }
        }
    }
}

void HDCP_NonISRModeHandler(void)
{
    BYTE u8PortCnt = 0;

    for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++ )
    {
        if (g_stHdcpHandler[u8PortCnt].enHdcpType == EN_HDCPMBX_PORT_TYPE_SINK)
        {
#if (DEF_HDCP2RX_ISR_MODE)
            if(g_stHdcpHandler[u8PortCnt].bAuthDone && g_stHdcpHandler[u8PortCnt].bTx2RxInfo)
            {
                g_stHdcpHandler[u8PortCnt].u8MainState = EN_HDCP2RX_MS_B4;
                g_stHdcpHandler[u8PortCnt].u8SubState = EN_HDCP2_MSG_ZERO;
                HDCPMsgPool_SetPriority(u8PortCnt, 0); //clear priority;
                HDCPMBX_SetActiveBit(u8PortCnt, FALSE); //clear active bit;
                g_stHdcpHandler[u8PortCnt].bPolling = FALSE; //clear polling flag;
                HDCPRx_Handler(u8PortCnt);
            }
#else
            if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE)) != 0x00) //TBD: fetch message by priority
            {
                HDCPRx_Handler(u8PortCnt);
            }
            else if(g_stHdcpHandler[u8PortCnt].bAuthDone && (g_stHdcpHandler[u8PortCnt].u8HDCPRxVer == EN_HDCP_VER_22))
            {
                HDCPRx_Handler(u8PortCnt);
            }
#endif
            if(g_stHdcpHandler[u8PortCnt].bAsRepeater)
            {
                HDCP14Rx_Handler(u8PortCnt);
            }
        }
        #if 0//[MT9700]
        else
        {
            if (g_u32IntervalCnt % DEF_HDCP_HANDLER_INTERVAL == 0)
            {
                if (g_stHdcpHandler[u8PortCnt].bEnableHdcp == TRUE) //check if hdcp enabled;
                {
                    if ((g_u8HdcpTxAuthRetryCnt == 0) || (g_u8HdcpTxAuthRetryCnt >= g_stHdcpHandler[u8PortCnt].u8RetryCnt))
                    {
                        HDCPTx_Handler(u8PortCnt);
                    }
                }
                g_u32IntervalCnt = 0;
            }
        }
        #endif
    }
    g_u32IntervalCnt++;
}
#endif
void HDCPHandler_RebootInitValue(void)
{
    g_bHdcpRxInitDoneFlag = FALSE;
    g_u8SecR2EventFlag = FALSE;
    g_u32IntervalCnt=0;

    return;
}
#if (COMBO_HDCP2_FUNCTION_SUPPORT)
void HDCP_ISRModeHandler(void)
{
	BYTE u8PortCnt = 0;


	for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++ )
	{
		if (g_stHdcpHandler[u8PortCnt].enHdcpType == EN_HDCPMBX_PORT_TYPE_SINK)
		{
			if(g_stHdcpHandler[u8PortCnt].bAuthDone && g_stHdcpHandler[u8PortCnt].bTx2RxInfo && !g_stHdcpHandler[u8PortCnt].bIsSendRecvList && !g_stHdcpHandler[u8PortCnt].bRecvStreamManage)
			{
				g_stHdcpHandler[u8PortCnt].u8MainState = EN_HDCP2RX_MS_B4;
				g_stHdcpHandler[u8PortCnt].u8SubState = EN_HDCP2_MSG_ZERO;
				HDCPMsgPool_SetPriority(u8PortCnt, 0); //clear priority;
				HDCPMBX_SetActiveBit(u8PortCnt, FALSE); //clear active bit;
				g_stHdcpHandler[u8PortCnt].bPolling = FALSE; //clear polling flag;
				HDCPRx_Handler(u8PortCnt);
			}
			else if(g_stHdcpHandler[u8PortCnt].bRecvMsg == TRUE && !g_stHdcpHandler[u8PortCnt].bIsSendRecvList && g_stHdcpHandler[u8PortCnt].bRecvStreamManage == TRUE) //For HDCP22 CTS 3C01
			{
				g_stHdcpHandler[u8PortCnt].u8MainState = EN_HDCP2RX_MS_B4;
				g_stHdcpHandler[u8PortCnt].u8SubState = EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE;
				HDCPMsgPool_SetPriority(u8PortCnt, 0); //clear priority;
				HDCPMBX_SetActiveBit(u8PortCnt, FALSE); //clear active bit;
				g_stHdcpHandler[u8PortCnt].bPolling = FALSE; //clear polling flag;
				HDCPRx_Handler(u8PortCnt);
			}
			if(g_stHdcpHandler[u8PortCnt].bAsRepeater)
			{
				HDCP14Rx_RepeaterHandler(u8PortCnt);
			}
			else
			{
				//patch for Rx alwsys declare Repeater but Tx not connect
				HDCP14Rx_Handler(u8PortCnt);
			}
		}
#if 0
		else
		{
			if (g_u32IntervalCnt % DEF_HDCP_HANDLER_INTERVAL == 0)
			{
				if (g_stHdcpHandler[u8PortCnt].bEnableHdcp == TRUE) //check if hdcp enabled;
				{
                    if ((g_u8HdcpTxAuthRetryCnt == 0) || (g_u8HdcpTxAuthRetryCnt >= g_stHdcpHandler[u8PortCnt].u8RetryCnt))
                    {
					    HDCPTx_Handler(u8PortCnt);
                    }
				}
				g_u32IntervalCnt = 0;
			}
		}
#endif
	}
	g_u32IntervalCnt++;
}

void HDCPHandler_Init(WORD u16InputPortTypeMap, WORD u16OutputPortTypeMap, BOOL bIsRepeater __attribute__ ((unused)))
{
    BYTE u8PortCnt = 0;
    BYTE u8BitCnt = 0;
    for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM ; u8PortCnt++ )
    {
        //init callback;
        g_stHdcpHandler[u8PortCnt].pHdcpIOCbFunc.pRxSendFunc = NULL;
        g_stHdcpHandler[u8PortCnt].pHdcpIOCbFunc.pTxIOCbFunc = NULL;

        //reset state;
        g_stHdcpHandler[u8PortCnt].u8MainState = 0;
        g_stHdcpHandler[u8PortCnt].u8SubState = 0;

        //init flag
        g_stHdcpHandler[u8PortCnt].bEnableHdcp = FALSE;
        g_stHdcpHandler[u8PortCnt].bSendAKEInit = FALSE;
        g_stHdcpHandler[u8PortCnt].bRetryStreamManage = FALSE;
        g_stHdcpHandler[u8PortCnt].bGetRecvIDList = FALSE;
        g_stHdcpHandler[u8PortCnt].bStoredKm = FALSE;
        g_stHdcpHandler[u8PortCnt].bSendLCInit = FALSE;
        g_stHdcpHandler[u8PortCnt].bPolling = FALSE;
        g_stHdcpHandler[u8PortCnt].bAuthDone = FALSE;
        g_stHdcpHandler[u8PortCnt].bSinkIsRepeater = FALSE;
        g_stHdcpHandler[u8PortCnt].bRecvMsg = FALSE;
		g_stHdcpHandler[u8PortCnt].bRecvStreamManage = FALSE;
        g_stHdcpHandler[u8PortCnt].bRecvAck = FALSE;
        g_stHdcpHandler[u8PortCnt].bReadDone = FALSE;
        g_stHdcpHandler[u8PortCnt].bSendPair = FALSE;
        g_stHdcpHandler[u8PortCnt].bPrepare2SetRdyBit = FALSE;
        g_stHdcpHandler[u8PortCnt].bTx2RxInfo= FALSE;
        g_stHdcpHandler[u8PortCnt].bIsSendRecvList = FALSE;
        g_stHdcpHandler[u8PortCnt].bRecvIDSet = FALSE;  // for  LG use
        g_stHdcpHandler[u8PortCnt].bRx2TxStreamType= FALSE;
        //g_stHdcpHandler[u8PortCnt].bAsRepeater = bIsRepeater;
        g_stHdcpHandler[u8PortCnt].bRxHDCPStart= FALSE;
        g_stHdcpHandler[u8PortCnt].bRxStartEnc= FALSE;
    }
    #if DEF_HDCP2RX_ISR_MODE
    g_bHdcpRxInitDoneFlag = TRUE;
    #endif
    //assign port type;
    //[MT9700]for (u8BitCnt = 0; u8BitCnt < COMBO_IP_SUPPORT_TYPE; u8BitCnt++)
    for (u8BitCnt = 0; u8BitCnt < 1; u8BitCnt++)
    {
        //reset RxVersion
        g_stHdcpHandler[u8BitCnt].u8HDCPRxVer = 0;
        g_stHdcpHandler[u8BitCnt].enHdcpType = ((u16InputPortTypeMap >> u8BitCnt) & 0x01) ? EN_HDCPMBX_PORT_TYPE_SINK : EN_HDCPMBX_PORT_TYPE_SOURCE;
        //[MT9700]g_u8RxTxPortPair[u8BitCnt] = 0;
    }
#if 0   // no hdcp tx, and g_stHdcpHandler[DEF_HDCP_PORT_NUM] , DEF_HDCP_PORT_NUM = 1
    //[MT9700]for (u8BitCnt = COMBO_IP_SUPPORT_TYPE; u8BitCnt < DEF_HDCP_PORT_NUM; u8BitCnt++)
    for (u8BitCnt = 0; u8BitCnt < 1; u8BitCnt++)
    {
        g_stHdcpHandler[u8BitCnt].enHdcpType = ((u16OutputPortTypeMap >> (u8BitCnt)) & 0x01) ? EN_HDCPMBX_PORT_TYPE_SOURCE : EN_HDCPMBX_PORT_TYPE_SINK;
        //[MT9700]g_u8TxRxPortPair[u8BitCnt - COMBO_IP_SUPPORT_TYPE] = u8BitCnt - COMBO_IP_SUPPORT_TYPE;
    }
#else
    UNUSED(u16OutputPortTypeMap);
#endif
}
#endif
void HDCPHandler_LoadHDCP14RxBksv(BYTE *pu8BKSV, BYTE *pu8Hdcp1RxKey, BOOL bExternalKey)
{
    UNUSED(pu8BKSV);
    UNUSED(pu8Hdcp1RxKey);
    UNUSED(bExternalKey);

    WORD ustemp = 0;
    WORD usRegOffsetAuxDPCDByID = 0;

	msWrite2ByteMask(REG_COMBO_GP_TOP_02_L, 0,BIT3); //  combo_gp_top_02[15:0]: reg_misc_clk_pd
    msWrite2ByteMask(REG_PM_48_L,0x0000,BIT4|BIT5);//reg_clk_ddc_selDDC clock selection, 00: XTAL/01: 12M
    msWrite2ByteMask(REG_COMBO_GP_TOP_5A_L, BIT15, BIT15);//reg_combo_top_reserved_0[15:0]
    msWrite2ByteMask(REG_PM_57_L,BIT11,BIT10|BIT11);//[10]reg_hdcp_sram_cen[11]reg_sram_handshake

    //msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L,BIT10,BIT10);//[10]HDCP enable for DDC
    msWrite2ByteMask(REG_PM_57_L,BIT14|BIT15,BIT14|BIT15);

    //msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L,BIT10,BIT10);//[10]HDCP enable for DDC
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L,BIT15,BIT14|BIT15);//[15]reg_enwrite_hdcp[14]reg_hdcp_sram_access0: access HDCP 74reg/1: access HDCP sram

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L,0x0000,0x03ff);//74 address
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L,BIT5,BIT5);//trig latch address reg_load_adr_p,HDCP address load pulse generate [5]0: not gen pulse/1: gen pulse

    for(ustemp = 0; ustemp < 0x50; ustemp++)   {
        msWrite2ByteMask(REG_HDMIRX_HDCP_P0_18_L, 0, BMASK(7:0));
        msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L, BIT(4), BIT(4));
        // wait write ready
        while(msRead2Byte(REG_HDMIRX_HDCP_P0_19_L) & BIT(7));
    }
    //avoid overwrite offset 0x50 (hdcp22 capability)
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L, 0x51, BMASK(9:0));
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L, BIT(5), BIT(5));

    for(ustemp = 0x51; ustemp <= 0xff; ustemp++)   {
        msWrite2ByteMask(REG_HDMIRX_HDCP_P0_18_L, 0, BMASK(7:0));
        msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L, BIT(4), BIT(4));
        // wait write ready
        while(msRead2Byte(REG_HDMIRX_HDCP_P0_19_L) & BIT(7));
    }

    //restore address back to offset 0x00
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L, 0, BMASK(9:0)); // CPU r/w address (for hdcp_key_sram/74reg)
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L, BIT(5), BIT(5)); // HDCP address load pulse generate

    //bksv
    for(ustemp = 0; ustemp < DEF_HDCP1X_BKSV_SIZE; ustemp++)
    {
      msWrite2ByteMask(REG_HDMIRX_HDCP_P0_18_L, tCOMBO_HDCP14_BKSV[ustemp], 0xFF); // the data for CPU write into HDCP KEY SRAM through XIU
      //msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L,BIT4,BIT4);//reg_hdcp_data_wr_p,HDCP data write port pulse generate[4]0: not gen pulse/1: gen pulse
      //msWrite2ByteMask(0x170530,tCOMBO_HDCP14_BKSV[ustemp],0xFF);//addrH is0x18 LowByte
      msWrite2ByteMask(0x170532,0x10,0x10);//addrH is0x19 LowByte
    }

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_17_L,0x0040,0x03FF);//74 address
    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L,BIT5,BIT5);//trig latch address reg_load_adr_p,HDCP address load pulse generate [5]0: not gen pulse/1: gen pulse

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_18_L, DEF_HDCP_RX_SUPPORT_EESS? 0x82: 0x80, 0xFF); //bcaps the data for CPU write into HDCP KEY SRAM through XIU

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L,BIT4,BIT4);//reg_hdcp_data_wr_p,HDCP data write port pulse generate[4]0: not gen pulse/1: gen pulse

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_19_L,0x0000,BIT14|BIT15);//[15]reg_enwrite_hdcp[14]reg_hdcp_sram_access

    msWrite2ByteMask(REG_HDMIRX_HDCP_P0_00_L,0x0721,0xFFFF);


    //======================= DP port load BKSV=======================//

    for(ustemp = 0; ustemp < AUX_None; ustemp++)
    {
        if((ustemp == DPRx_C1_AUX) || (ustemp == DPRx_C2_AUX) || (ustemp == DPRx_C3_AUX) || (ustemp == DPRx_C4_AUX))
        {
            usRegOffsetAuxDPCDByID = ustemp * 0x300;
        }
        else
        {
            continue;
        }
        msWriteByte(REG_DPRX_AUX_DPCD_2A_L + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[0]); // BKSV // Move to R2
        msWriteByte(REG_DPRX_AUX_DPCD_2A_H + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[1]); // BKSV
        msWriteByte(REG_DPRX_AUX_DPCD_2B_L + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[2]); // BKSV
        msWriteByte(REG_DPRX_AUX_DPCD_2B_H + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[3]); // BKSV
        msWriteByte(REG_DPRX_AUX_DPCD_2C_L + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[4]); // BKSV
    }

    msWriteByte(REG_DPRX_DPCD1_22_L, tCOMBO_HDCP14_BKSV[0]); // BKSV // Move to R2
    msWriteByte(REG_DPRX_DPCD1_22_H, tCOMBO_HDCP14_BKSV[1]); // BKSV
    msWriteByte(REG_DPRX_DPCD1_23_L, tCOMBO_HDCP14_BKSV[2]); // BKSV
    msWriteByte(REG_DPRX_DPCD1_23_H, tCOMBO_HDCP14_BKSV[3]); // BKSV
    msWriteByte(REG_DPRX_DPCD1_24_L, tCOMBO_HDCP14_BKSV[4]); // BKSV

    //============================================================//


    HDCP_printMsg("** HDCPHandler_LoadHDCP14RxBksv");

}

void HDCPHandler_LoadHDCP14RxKey(BYTE *pu8BKSV, BYTE *pu8Hdcp1RxKey, BOOL bExternalKey)
{
    UNUSED(pu8BKSV);
    UNUSED(pu8Hdcp1RxKey);
    UNUSED(bExternalKey);

    WORD ustemp = 0;

    msWrite2ByteMask(REG_HDCPKEY_02_L,BIT8,BIT8);//reg_hdcpkey_reserved_02
    msWrite2ByteMask(REG_COMBO_GP_TOP_40_L,0x0D,0x0D);//[0]reg_xiu2hdcpkey_en[2] CPU write, [3] HDCP ram

    msWrite2ByteMask(REG_HDCPKEY_00_L,0x0005,0x03FF);//reg_xiu2hdcpkey_adr

    for(ustemp = 0; ustemp < DEF_HDCP1X_RX_KEY_SIZE; ustemp++)
    {
      msWrite2ByteMask(REG_HDCPKEY_01_L, pDPHDCP14Key[ustemp], 0xFF); // the data for CPU write into HDCP KEY SRAM through XIU
    }

    msWrite2ByteMask(REG_COMBO_GP_TOP_40_L,0x00,0x0D);//[0]reg_xiu2hdcpkey_en[2] CPU write, [3] HDCP ram
    HDCP_printMsg("** Combo check XIU HDCP14 Rx key decode done");


}

void HDCPHandler_HDCPLoadKeyViaEfuse(BYTE ucKeyType, BYTE* pucHDCPKeyTable, BYTE* pucHDCPBksvTable)
{
    WORD u16Temp;
    if(ucKeyType == COMBO_KEY_RX)
    {
        if(pucHDCPKeyTable != NULL)
        {
            if(pucHDCPBksvTable != NULL)
            {
                for(u16Temp = 0x0; u16Temp < (COMBO_HDCP_BKSV_SIZE + COMBO_HDCP_KEY_SIZE); u16Temp++)
                {
                    if(u16Temp < 5)
                    {
                        pucHDCPBksvTable[u16Temp] = msEread_GetDataFromEfuse(u16Temp);
                        //printf("0x%02x ", pucHDCPBksvTable[i]);
                    }
                    else
                    {
                        pucHDCPKeyTable[u16Temp - 5] = msEread_GetDataFromEfuse(u16Temp);
                        //printf("0x%02x ", pucHDCPKeyTable[u16Temp - 5]);
                    }
                }
            }
        }
    }
    else
    {
        // do nothing
    }
    return;
}

void HDCPHandler_LoadKeyViaEfuse(void)
{
    HDCPHandler_HDCPLoadKeyViaEfuse(COMBO_KEY_RX, pDPHDCP14Key, tCOMBO_HDCP14_BKSV);
}


#endif //#ifndef HDCP_HANDLER_C
