#ifndef HDCP_MSGPOOL_C
#define HDCP_MSGPOOL_C
//----------------------------------------------------------------------//
// includes
//----------------------------------------------------------------------//
#include <string.h>
#include "datatype.h"
#include "Global.h"
#include "Common.h"

#include "HdcpCommon.h"
#include "HdcpLog.h"
#include "HdcpMsgPool.h"
#include "HdcpHandler.h"
#include "HdcpError.h"

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//
extern volatile ST_HDCP_HANDLER g_stHdcpHandler[DEF_HDCP_PORT_NUM];
extern volatile BYTE* g_u8HdcpRecvBuf; //bit 7 of byte 0 is processing bit
extern volatile BYTE* g_u8HdcpTransBuf;

//----------------------------------------------------------------------//
// functions
//----------------------------------------------------------------------//
BYTE HDCPMsgPool_GetMaxPriority(void)
{
    BYTE u8PortCnt = 0;
    BYTE u8MaxPriority = 0;

    for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++)
    {
        if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) > u8MaxPriority)
        {
            u8MaxPriority = *(BYTE*)(g_u8HdcpRecvBuf + u8PortCnt * DEF_HDCPMSG_BUF_BASE_IDX);
        }
    }
    return u8MaxPriority;
}

BYTE HDCPMsgPool_GetMinPriority(void)
{
    BYTE u8PortCnt = 0;
    BYTE u8MinPriority = DEF_HDCP_PORT_NUM;

    for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++)
    {
        if (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) < u8MinPriority)
        {
            u8MinPriority = *(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX);
        }
    }
    return u8MinPriority;
}

void HDCPMsgPool_AssignPriority(BYTE u8PortIdx)
{
    BYTE u8CurMax = 0;
    BYTE u8CurMin = 0;
    BYTE u8PortCnt = 0;

    u8CurMax = HDCPMsgPool_GetMaxPriority();

    if (u8CurMax == 0)
    {
        *(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) = 1;
    }
    else
    {
        u8CurMin = HDCPMsgPool_GetMinPriority();

        for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++)
        {
            if ((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) != 0) \
                && (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) <= DEF_HDCP_PORT_NUM ))
            {
                (*(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX))++;
            }
        }
        *(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) = u8CurMin;
    }
}

void HDCPMsgPool_SetPriority(BYTE u8PortIdx, BYTE u8Priority)
{
    *(BYTE*)(g_u8HdcpRecvBuf + (u8PortIdx * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) = u8Priority;
}

BYTE HDCPMsgPool_FetchMsg(void)
{
    BYTE u8PortCnt = 0;
    BYTE u8MaxPriority = 0;
    BYTE u8MsgIdx = 0;

    for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++)
    {
        if ((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX) > u8MaxPriority)\
            && (((*(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX)) & BIT7) == 0))
        {
            u8MaxPriority = *(BYTE*)(g_u8HdcpRecvBuf + (u8PortCnt * DEF_HDCP_RECV_BUF_SIZE) + DEF_HDCPMSG_BUF_BASE_IDX);
            u8MsgIdx = u8PortCnt;
        }
    }

    if (u8MaxPriority == 0)
        return (DEF_HDCP_PORT_NUM + 1);
    else
        return u8MsgIdx;
}

#endif //#ifndef HDCP_MSGPOOL_C
