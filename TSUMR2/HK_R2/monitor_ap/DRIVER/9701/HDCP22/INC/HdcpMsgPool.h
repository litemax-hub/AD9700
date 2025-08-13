#ifndef HDCP_MSGPOOL_H
#define HDCP_MSGPOOL_H

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HDCPMSG_BUF_BASE_IDX        0
#define DEF_HDCPMSG_BUF_CMD_ID_IDX      1
#define DEF_HDCPMSG_BUF_CONTENT_IDX     2
//----------------------------------------------------------------------//
// function proto-type
//----------------------------------------------------------------------//
BYTE HDCPMsgPool_GetMaxPriority(void);
BYTE HDCPMsgPool_GetMinPriority(void);
void HDCPMsgPool_AssignPriority(BYTE u8PortIdx);
void HDCPMsgPool_SetPriority(BYTE u8PortIdx, BYTE u8Priority);
BYTE HDCPMsgPool_FetchMsg(void);
#endif //#ifndef HDCP_MSGPOOL_H
