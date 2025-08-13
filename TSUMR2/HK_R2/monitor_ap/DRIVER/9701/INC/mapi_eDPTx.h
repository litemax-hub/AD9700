#ifndef _MAPI_EDPTx_H_
#define _MAPI_EDPTx_H_

////////////////////////////////////////////////////////////
// ####  Fix Parameter for eDPTX (Do Not Change this Parameter) ####
////////////////////////////////////////////////////////////

// ******************************************************

// ######  external function call ###########################


extern void mapi_eDPTx_PowerDown(BYTE PortNum);
extern void mapi_eDPTx_Init(BYTE PortNum);
extern void mapi_eDPTx_Handle(void);
extern void mapi_eDPTx_SetMSA(BYTE PortNum);
extern void mapi_eDPTx_FineTuneTU_Disable( BYTE PortNum);
extern void mapi_eDPTx_FineTuneTU( BYTE PortNum, DWORD eDPTX_OUTBL_PixRateKhz);
extern Bool mapi_eDPTx_AuxDPCDReadByte( BYTE PortNum, DWORD  DPCDADDR , BYTE *pRdData);
extern Bool mapi_eDPTx_AuxDPCDReadBytes( BYTE PortNum, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf);
extern Bool mapi_eDPTx_AuxDPCDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData);
extern Bool mapi_eDPTx_AuxDPCDWriteBytes (BYTE PortNum, DWORD  DPCDADDR, BYTE Length ,BYTE *pTxBuf);
extern Bool mapi_eDPTx_AuxEDIDReadNLCMD( BYTE PortNum, DWORD  DPCDADDR );
extern Bool mapi_eDPTx_AuxEDIDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData);
extern Bool mapi_eDPTx_AuxEDIDReadBytes( BYTE PortNum, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf);
extern Bool mapi_eDPTx_ReadEDID(BYTE PortNum);
extern Bool mapi_eDPTx_AuxMCCSReadBytes( BYTE PortNum, BYTE Length, BYTE EOF, BYTE *pRxBuf);
extern Bool mapi_eDPTx_AuxMCCSWriteBytes(BYTE PortNum, BYTE Length, BYTE EOF ,BYTE *pTxBuf);
extern Bool mapi_eDPTx_CheckHPD(BYTE HPDPortNum);
extern Bool mapi_eDPTx_CheckHPDIRQ(BYTE HPDPortNum);
extern Bool mapi_eDPTx_Training(BYTE PortNum, BYTE LinkRate, BYTE LaneCount);
extern void mapi_eDPTx_HBR2DRVMAINTune(BYTE LaneNum,BYTE SetValue, BYTE PortNum);
extern void mapi_eDPTx_HBR2DRVPRETune(BYTE LaneNum,BYTE SetValue, BYTE PortNum);
extern void mapi_eDPTx_HBR2PREDRVMAINTune(BYTE LaneNum,BYTE SetValue, BYTE PortNum);
extern void mapi_eDPTx_HBR2PREDRVPRETune(BYTE LaneNum,BYTE SetValue, BYTE PortNum);
extern void mapi_eDPTx_HBR2SetSwPre(BYTE LinkRate,BYTE LaneNum,BYTE SwLv, BYTE PreLv, BYTE PortNum);
extern void mapi_eDPTx_SSCEnable(BYTE bEnable, BYTE LinkRate, BYTE PortNum);
extern void mapi_eDPTx_PGSyncRst(void);
extern void mapi_eDPTx_HPDIRQRst(BYTE PortNum);
extern void mapi_eDPTx_ATSTPrintErrCNT(BYTE PortNum);
extern void mapi_eDPTx_VariableInit(void);
// ######  internal function call ###########################
void mapi_eDPTx_ConfigLinkRate(BYTE LinkRate, BYTE PortNum);
void mapi_eDPTx_VDClkRst(void);
void mapi_eDPTx_MLSignalEn(BYTE ucInputPort, BYTE bEnable);


#endif
