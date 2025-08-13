
#ifndef _MAPI_EDPTX_C_
#define _MAPI_EDPTX_C_
#include <string.h>
#include <math.h>
#include "board.h"
//#include "datatype.h"
#include "mapi_eDPTx.h"
#include "mdrv_eDPTx.h"
#include "drvmStar.h"

#if PANEL_EDP

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

    //static Bool b1TimeInit = FALSE;
    if(1)//b1TimeInit == FALSE)
    {
        mdrv_eDPTx_Init1Time();
        //b1TimeInit = TRUE;
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
//      mapi_eDPTx_SetSCFastLPLL(BOOL IsVGA, WORD VGAHtt, WORD VGAVtt, WORD InVFreq)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void mapi_eDPTx_SetSCFastLPLL(BOOL IsVGA, WORD VGAHtt, WORD VGAVtt, WORD InVFreq)
{
    mdrv_eDPTx_SetSCFastLPLL(IsVGA, VGAHtt, VGAVtt, InVFreq);
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
    LaneNum = LaneNum;
    SetValue = SetValue;
    PortNum = PortNum;

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
    LaneNum = LaneNum;
    SetValue = SetValue;
    PortNum = PortNum;
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
    LaneNum = LaneNum;
    SetValue = SetValue;
    PortNum = PortNum;
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
    LaneNum = LaneNum;
    SetValue = SetValue;
    PortNum = PortNum;
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
    LinkRate = LinkRate;
    LaneNum = LaneNum;
    SwLv = SwLv;
    PreLv = PreLv;
    PortNum = PortNum;
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
void mapi_eDPTx_HPDIRQ_Handler(BYTE PortNum)
{
     mdrv_eDPTx_HPDIRQ_Handler(PortNum);
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
    #if eDPTXAutoTest
        mdrv_eDPTx_ATSTReadSwPreReq(PortNum, LinkRate, LaneCount);
    #else
        PortNum = PortNum;
        LinkRate = LinkRate;
        LaneCount = LaneCount;
    #endif
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
void mapi_eDPTx_AutoTest(BYTE PortNum)
{
    #if eDPTXAutoTest
        mdrv_eDPTx_AutoTest(PortNum);
    #else
        PortNum = PortNum;

    #endif

    return;

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
//**************************************************************************
//  [Function Name] :  mapi_eDPTx_PowerDown(void)
//
//  [Description] : Power down for PHY
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mapi_eDPTx_PowerDown(BOOL bPowerDown) //poll from Raptor
{
    mdrv_eDPTx_PowerDown(bPowerDown);
    return;
}

#endif

#endif //#ifndef _MAPI_EDPTX_C_


