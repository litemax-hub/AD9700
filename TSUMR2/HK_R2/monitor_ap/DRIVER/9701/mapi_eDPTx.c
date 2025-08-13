#define _MAPI_EDPTX_C_
#include <string.h>
#include <math.h>
#include "board.h"
#include "datatype.h"
#include "mapi_eDPTx.h"
#include "mdrv_eDPTx.h"
#include "mhal_eDPTx.h"


#if 1//PANEL_EDP
//**************************************************************************
//  [Function Name]:
//                  mapi_eDPTx_VariableInit( PortNum )
//  [Description] : IInitial eDP related variables
//
//  [Arguments]:
//
//  [Return]: None
//
//**************************************************************************

void mapi_eDPTx_VariableInit(void)
{
#if PANEL_EDP
    BYTE u8PortNum = 0x00;
    
    g_eDPTxInfo.u8PortSelect = eDPTX_port_select;
    g_eDPTxInfo.u8TotalPorts=0x00;
    for(u8PortNum=0x00;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
            g_eDPTxInfo.u8TotalPorts++;
    }

    g_eDPTxInfo.u8LinkRate = eDPTXLinkRate;
    g_eDPTxInfo.u8LaneCount = eDPTXLaneCount;
    g_eDPTxInfo.u8AuxPNSwap = (eDPTXAuxP0PNSWAP?BIT0:0x00)|(eDPTXAuxP1PNSWAP?BIT1:0x00)|(eDPTXAuxP2PNSWAP?BIT2:0x00)|(eDPTXAuxP3PNSWAP?BIT3:0x00);
    g_eDPTxInfo.u8LanePNSwap = eDPTXHBR2PNSWAP;
    g_eDPTxInfo.u8LaneSwap = eDPTXHBR2LaneSWAP;
    g_eDPTxInfo.u8LaneSkewSwap = eDPTXLaneSkewSWAP;
    g_eDPTxInfo.u8PortSWAP = eDPTXPortSWAP;
    g_eDPTxInfo.u8SSCModulation = PANEL_SSC_MODULATION_DEF_EDP;
    g_eDPTxInfo.u8SSCPercentage = PANEL_SSC_PERCENTAGE_DEF_EDP;
    g_eDPTxInfo.u8ColorDepth = eDPTXColorDepth;
    g_eDPTxInfo.bEnhanceFrameMode = TRUE;
    g_eDPTxInfo.bASSR = eDPTXEnASSR;
#endif
    return;
}
//**************************************************************************
//  [Function Name]:
//                  mapi_eDPTx_PowerDown( PortNum )
//  [Description] : Power Down all ports
//
//  [Arguments]:
//
//  [Return]: None
//
//**************************************************************************
void mapi_eDPTx_PowerDown(BYTE PortNum)
{
    mdrv_eDPTx_PowerDown(PortNum);
    return;
}
//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_Init(BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_Init(BYTE PortNum)
{
    static Bool b1TimeInit = FALSE;

    if(b1TimeInit == FALSE)
    {
        mdrv_eDPTx_Init1Time();
        b1TimeInit = TRUE;
    }

    mdrv_eDPTx_Init(PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_SetMSA(BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_SetMSA(BYTE PortNum)
{
    mdrv_eDPTx_SetMSA(PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_FineTuneTU(BYTE PortNum, DWORD eDPTX_OUTBL_PixRateKhz)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_FineTuneTU_Disable(BYTE PortNum)
{
    mdrv_eDPTx_FineTuneTU_Disable(PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_FineTuneTU(BYTE PortNum, DWORD eDPTX_OUTBL_PixRateKhz)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_FineTuneTU(BYTE PortNum, DWORD eDPTX_OUTBL_PixRateKhz)
{
    mdrv_eDPTx_FineTuneTU(PortNum, eDPTX_OUTBL_PixRateKhz);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxDPCDReadByte( BYTE PortNum, DWORD  DPCDADDR , BYTE *pRdData)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxDPCDReadByte( BYTE PortNum, DWORD  DPCDADDR , BYTE *pRdData)
{
    return mdrv_eDPTx_AuxDPCDReadByte(PortNum, DPCDADDR, pRdData);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxDPCDReadBytes( BYTE PortNum, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxDPCDReadBytes( BYTE PortNum, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf)
{
    return mdrv_eDPTx_AuxDPCDReadBytes(PortNum, DPCDADDR, Length, pRxBuf);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxDPCDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxDPCDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData)
{
    return mdrv_eDPTx_AuxDPCDWriteByte(PortNum, DPCDADDR, wData);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxDPCDWriteBytes(BYTE PortNum, DWORD    DPCDADDR, BYTE Length ,BYTE *pTxBuf)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxDPCDWriteBytes(BYTE PortNum, DWORD   DPCDADDR, BYTE Length ,BYTE *pTxBuf)
{
    return mdrv_eDPTx_AuxDPCDWriteBytes(PortNum, DPCDADDR, Length, pTxBuf);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxEDIDReadNLCMD( BYTE PortNum, DWORD  DPCDADDR )
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxEDIDReadNLCMD( BYTE PortNum, DWORD  DPCDADDR )
{
    return mdrv_eDPTx_AuxEDIDReadNLCMD(PortNum, DPCDADDR );

}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxEDIDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxEDIDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData)
{
    return mdrv_eDPTx_AuxEDIDWriteByte(PortNum, DPCDADDR , wData);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxEDIDReadBytes( BYTE PortNum, DWORD    DPCDADDR , BYTE Length , BYTE *pRxBuf)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxEDIDReadBytes( BYTE PortNum, DWORD   DPCDADDR , BYTE Length , BYTE *pRxBuf)
{
    return mdrv_eDPTx_AuxEDIDReadBytes(PortNum, DPCDADDR , Length , pRxBuf);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_ReadEDID(BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_ReadEDID(BYTE PortNum)
{
    return mdrv_eDPTx_ReadEDID(PortNum);
}


//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxMCCSReadBytes( BYTE PortNum, BYTE Length, BYTE EOF, BYTE *pRxBuf)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxMCCSReadBytes( BYTE PortNum, BYTE Length, BYTE EOF, BYTE *pRxBuf)
{
    return mdrv_eDPTx_AuxMCCSReadBytes(PortNum, Length, EOF, pRxBuf);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_AuxMCCSWriteBytes(BYTE PortNum, BYTE Length, BYTE EOF ,BYTE *pTxBuf)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AuxMCCSWriteBytes(BYTE PortNum, BYTE Length, BYTE EOF ,BYTE *pTxBuf)
{
    return mdrv_eDPTx_AuxMCCSWriteBytes(PortNum, Length, EOF, pTxBuf);
}



//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_ReadDPCDLinkStatus(BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_ReadDPCDLinkStatus(BYTE PortNum)
{
    return mdrv_eDPTx_ReadDPCDLinkStatus(PortNum);
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_ConfigLinkRate(BYTE LinkRate, BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_ConfigLinkRate(BYTE LinkRate, BYTE PortNum)
{
    mdrv_eDPTx_ConfigLinkRate(LinkRate, PortNum);
    return;
}


//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_SSCEnable(BYTE bEnable, BYTE LinkRate, BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_SSCEnable(BYTE bEnable, BYTE LinkRate, BYTE PortNum)
{
    mdrv_eDPTx_SSCEnable(bEnable, LinkRate, PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//      mapi_eDPTx_MLSignalEn(BYTE PortNum, BYTE bEnable)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_MLSignalEn(BYTE PortNum, BYTE bEnable)
{
    mdrv_eDPTx_MLSignalEn(PortNum,bEnable);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_Training(BYTE PortNum, BYTE LinkRate, BYTE LaneCount)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_Training(BYTE PortNum, BYTE LinkRate, BYTE LaneCount)
{
    return mdrv_eDPTx_Training(PortNum, LinkRate, LaneCount);
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_HBR2DRVMAINTune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_HBR2DRVMAINTune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
{
    mdrv_eDPTx_HBR2DRVMAINTune(LaneNum, SetValue, PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_HBR2DRVPRETune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_HBR2DRVPRETune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
{
    mdrv_eDPTx_HBR2DRVPRETune(LaneNum, SetValue, PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_HBR2PREDRVMAINTune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_HBR2PREDRVMAINTune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
{
    mdrv_eDPTx_HBR2PREDRVMAINTune(LaneNum, SetValue, PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_HBR2PREDRVPRETune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_HBR2PREDRVPRETune(BYTE LaneNum, BYTE SetValue, BYTE PortNum)
{
    mdrv_eDPTx_HBR2PREDRVPRETune(LaneNum, SetValue, PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_HBR2SetSwPre(BYTE LinkRate, BYTE LaneNum, BYTE SwLv, BYTE PreLv, BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_HBR2SetSwPre(BYTE LinkRate, BYTE LaneNum, BYTE SwLv, BYTE PreLv, BYTE PortNum)
{
    mdrv_eDPTx_HBR2SetSwPre(LinkRate, LaneNum, SwLv, PreLv, PortNum);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_PGSyncRst(void)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_PGSyncRst(void)
{
    mdrv_eDPTx_PGSyncRst();
    return;

}
//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_VDClkRst(void)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_VDClkRst(void)
{
    mdrv_eDPTx_VDClkRst();
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_HPDIRQRst(BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_HPDIRQRst(BYTE PortNum)
{
     mdrv_eDPTx_HPDIRQRst(PortNum);
     return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_ATSTPrintErrCNT(BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_ATSTPrintErrCNT(BYTE PortNum)
{
    mdrv_eDPTx_ATSTPrintErrCNT(PortNum);
    return;
}


//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_ATSTReadSwPreReq(BYTE PortNum,BYTE LinkRate,BYTE LaneCount)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_ATSTReadSwPreReq(BYTE PortNum,BYTE LinkRate,BYTE LaneCount)
{
    mdrv_eDPTx_ATSTReadSwPreReq(PortNum, LinkRate, LaneCount);
    return;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_AutoTest(BYTE PortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_AutoTest(BYTE PortNum)
{
    return mdrv_eDPTx_AutoTest(PortNum);
}



//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_CheckHPD(BYTE HPDPortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_CheckHPD(BYTE HPDPortNum)
{
    Bool Status=FALSE;
    Status = mdrv_eDPTx_CheckHPD(HPDPortNum);
    return Status;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_CheckHPDIRQ(BYTE HPDPortNum)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
Bool mapi_eDPTx_CheckHPDIRQ(BYTE HPDPortNum)
{
    Bool Status=FALSE;
    mdrv_eDPTx_CheckHPDIRQ(HPDPortNum);
    return Status;
}

//******************************************************************************
//
//  [Function Name]:
//     mapi_eDPTx_Handle(void)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_Handle(void)
{
    mdrv_eDPTx_Handle();
    return;
}


#endif



