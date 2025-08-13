///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mdrv_eDPTx.h
/// @author MStar Semiconductor Inc.
/// @brief  DP driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_EDPTX_H_
#define _MDRV_EDPTX_H_
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define eDPTXAdjustSwingLevel   0     // 0(auto swing); 1(fix weak swing)~10(fix max swing)


#define AuxWaitReplyLpCntNum    2000     // 2000 nop delay
#define AuxWaitRdPluseLpCntNum  250//200  // 200 nop delay
#define AuxWaitWtPluseLpCntNum  10    // 10 nop delay
#define WaitHDCPR0VaildCountNum 500 // 300 nop delay

#define eDPTXMODPhyOffSet 0
//#define eDPTXDETECTHPD 1

// #############   eDPTXAutoTest  Define  ########################

#if eDPTXAutoTest
#define TEST_LINK_TRAINING      1
#define TEST_PATTERN            1
#define TEST_EDID_READ          0
#define PHY_TEST_PATTERN        1
    #if PHY_TEST_PATTERN
    #define TEST_D102                   1
    #define TEST_SYMBERR                1
    #define TEST_PRBS7                  1
    #define TEST_PHY80B                 1
    #define TEST_HBR2EYE                1
    #endif
#endif
// #############################################################

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void mdrv_eDPTx_PowerDown(BYTE PortNum);
void mdrv_eDPTx_Init1Time(void);
void mdrv_eDPTx_Init(BYTE ucInputPort);
void mdrv_eDPTx_SetMSA(BYTE PortNum);
void mdrv_eDPTx_FineTuneTU_Disable(BYTE ucInputPort);
void mdrv_eDPTx_FineTuneTU(BYTE ucInputPort, DWORD eDPTX_OUTBL_PixRateKhz);
Bool mdrv_eDPTx_AuxDPCDReadByte( BYTE PortNum, DWORD  DPCDADDR , BYTE *pRdData);
Bool mdrv_eDPTx_AuxDPCDReadBytes( BYTE PortNum, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf);
Bool mdrv_eDPTx_AuxDPCDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData);
Bool mdrv_eDPTx_AuxDPCDWriteBytes(BYTE PortNum, DWORD  DPCDADDR, BYTE Length ,BYTE *pTxBuf);
Bool mdrv_eDPTx_AuxEDIDReadNLCMD( BYTE PortNum, DWORD  DPCDADDR );
Bool mdrv_eDPTx_AuxEDIDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData);
Bool mdrv_eDPTx_AuxEDIDReadBytes( BYTE PortNum, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf);
Bool mdrv_eDPTx_ReadEDID(BYTE PortNum);
Bool mdrv_eDPTx_AuxMCCSReadBytes( BYTE PortNum, BYTE Length, BYTE EOF, BYTE *pRxBuf);
Bool mdrv_eDPTx_AuxMCCSWriteBytes(BYTE PortNum, BYTE Length, BYTE EOF ,BYTE *pTxBuf);
Bool mdrv_eDPTx_ReadDPCDLinkStatus(BYTE PortNum);
void mdrv_eDPTx_ConfigLinkRate(BYTE LinkRate, BYTE ucInputPort);
void mdrv_eDPTx_MLSignalEn(BYTE ucInputPort, bool bEnable);
Bool mdrv_eDPTx_Training(BYTE PortNum, BYTE LinkRate, BYTE LaneCount);
void mdrv_eDPTx_HBR2DRVMAINTune(BYTE LaneNum, BYTE SetValu, BYTE ucInputPorte);
void mdrv_eDPTx_HBR2DRVPRETune(BYTE LaneNum, BYTE SetValue, BYTE ucInputPort);
void mdrv_eDPTx_HBR2PREDRVMAINTune(BYTE LaneNum, BYTE SetValue, BYTE ucInputPort);
void mdrv_eDPTx_HBR2PREDRVPRETune(BYTE LaneNum, BYTE SetValue, BYTE ucInputPort);
void mdrv_eDPTx_HBR2SetSwPre(BYTE LinkRate, BYTE LaneNum, BYTE SwLv, BYTE PreLv, BYTE ucInputPort);
void mdrv_eDPTx_MODSetSwPre(BYTE LinkRate, BYTE LaneNum, BYTE SwLv, BYTE PreLv, BYTE ucInputPort);
void mdrv_eDPTx_AdjustSwingLevel(void);
void mdrv_eDPTx_SSCEnable(BYTE bEnable, BYTE LinkRate, BYTE ucInputPort);
void mdrv_eDPTx_HPDHandle01(BYTE ucInputPort);
void mdrv_eDPTx_HPDHandle02(BYTE ucInputPort);
Bool mdrv_eDPTx_CheckHPD(BYTE HPDPortNum);
Bool mdrv_eDPTx_CheckHPDIRQ(BYTE HPDPortNum);
void mdrv_eDPTx_PGSyncRst(void);
void mdrv_eDPTx_VDClkRst(void);
void mdrv_eDPTx_HPDIRQRst(BYTE ucInputPort);
void mdrv_eDPTx_ATSTPrintErrCNT(BYTE PortNum);
void mdrv_eDPTx_Handle(void);
void mdrv_eDPTx_HDCP13(BYTE ucInputPort);
void mdrv_eDPTx_ATSTReadSwPreReq(BYTE ucInputPort,BYTE LinkRate,BYTE LaneCount);
Bool mdrv_eDPTx_AutoTest(BYTE ucInputPort);


#endif


