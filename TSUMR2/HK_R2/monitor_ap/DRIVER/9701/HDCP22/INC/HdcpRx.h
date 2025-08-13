#ifndef HDCP_RX_H
#define HDCP_RX_H
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// HDCP_DUAL
#define REG_HDCP_DUAL_P0           0x171200UL
#define REG_HDCP_DUAL_P1           0x171500UL
#define REG_HDCP_DUAL_P2           0x171A00UL
#define REG_HDCP_DUAL_P3           0x171D00UL
#define REG_HDCP_DUAL_P4           0x172800UL
#define REG_HDCP_DUAL_P5           0x172B00UL
#define REG_HDCP_DUAL_P6           0x301300UL
#define REG_HDCP_DUAL_P7           0x301600UL
#define REG_HDCP_DUAL_P8           0x301900UL
#define REG_HDCP_DUAL_P9           0x301C00UL
#define REG_HDCP_DUAL_P10          0x301F00UL

typedef enum
{
    INPUT_PORT0 = 0,
    INPUT_PORT1,
    INPUT_PORT2,
    INPUT_PORT3,
    INPUT_PORT4,
    INPUT_PORT5,
    INPUT_PORT6,
    INPUT_PORT7,
    INPUT_PORT8,
    INPUT_PORT_END,
    INPUT_PORT_NONE,
}EN_INPUT_PORT;

typedef enum
{
    EN_HDCP_STATUS_FAIL = 0,
    EN_HDCP_STATUS_PASS,
    EN_HDCP_STATUS_PROCESSING,
    EN_HDCP_STATUS_NO_AUTH,
}ENUM_HDCP_AUTH_STATUS;

typedef struct
{
    BYTE ucDeviceExceeded;
    BYTE ucDeviceCount;
    BYTE ucDepth;
    BYTE ucDepthExceed;
    BYTE ucHDMIMode;
    BYTE ucRepeaterCapability;
    BYTE enInputPortSelect;
}ST_HDCP_RX_INFO, *pST_HDCP_RX_INFO;
//----------------------------------------------------------------------//
// function proto-type
//----------------------------------------------------------------------//
ENUM_HDCP_ERR_CODE HDCPRx_Handler(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCP14Rx_Handler(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCP14Rx_RepeaterHandler(BYTE u8PortIdx);
BOOL HDCPRx_Hdcp2MsgParser(BYTE ucPortIdx, BYTE *pucMsg);
BOOL HDCPRx_GetHdcp22CipherEnable(BYTE ucInputPort);
BOOL HDCPRx_GetEncryptionEnc(BYTE ucInputPort);
BYTE HDCPRx_GetAuthStatus(BYTE ucInputPort);
BOOL HDCPRx_GetHDCPRxInfo(BYTE ucInputPort, void* pDataContent);
BOOL HDCPRx_GetHDCP14_Info(BYTE u8PortIdx, ST_HDCP14_LG_INFO* stHDCPInfo);
BOOL HDCPRx_GetHDCP22Enc(BYTE ucInputPort);
BOOL HDCPRx_RecvIDList_Set(BYTE ucInputPort, BYTE *ucRecvIDList, BYTE ucLength);
void HDCPRx_ResetHdcp22AuthStatus(BYTE ucInputPort);

#endif //#ifndef HDCP_RX_H
