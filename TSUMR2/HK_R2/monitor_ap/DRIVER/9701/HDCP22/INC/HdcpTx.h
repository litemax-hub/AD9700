#ifndef HDCP_TX_H
#define HDCP_TX_H

//----------------------------------------------------------------------//
// function proto-type
//----------------------------------------------------------------------//
ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2Init(BYTE u8PortIdx);
ENUM_HDCP_ERR_CODE HDCPTx_Handler(BYTE u8PortIdx);
//ENUM_HDCP_ERR_CODE HDCPTx_Hdcp1xSetENC_EN(BYTE u8PortIdx, BOOL bEnable);
ENUM_HDCP_ERR_CODE HDCPTx_HdcpEnable_ENC(BYTE u8PortIdx, BOOL bEnable, BYTE u8Version);
//ENUM_HDCP_ERR_CODE HDCPTx_Hdcp2EnableAuth(BYTE u8PortIdx, BOOL bEnable);
//void HDCPTx_SetTxMode(BYTE u8OutputPort, BYTE u8Mode);
void HDCPRepeater_ReStart14TxAuth(BYTE u8PortIdx);
BOOL HDCPRepeater_TxSetRxRdyBit(BYTE u8PortIdx);
void HDCPRepeater_ConfigRepeaterPair(BYTE u8InputPort, BYTE u8OutputPort, Bool bEnableFlag);
BOOL HDCPTx_Hdcp2CheckSinkCapability(BYTE u8PortIdx);

#endif //#ifndef HDCP_TX_H
