///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_eDPTx.c
/// @author MStar Semiconductor Inc.
/// @brief  DP Rx driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_EDPTX_C_
#define _MDRV_EDPTX_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "board.h"
#include "ms_reg.h"
#include "Common.h"
#include "Ms_rwreg.h"
//#include "MDebug.h"
#include "mdrv_eDPTx.h"
#include "mhal_eDPTx.h"

#include "msEread.h"//for EFUSE_0
//#include "Ms_rwreg.h"

//#include "Utl.h"
#include "asmCPU.h"
#include <stdio.h>
#include <string.h>

#if 1//PANEL_EDP

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#if (eDPTX_DEBUG&&ENABLE_MSTV_UART_DEBUG)
#define eDPTX_printData(str, value)   printData(str, value)
#define eDPTX_printMsg(str)               printMsg(str)
#define eDPTX_printf(str, value)               printf(str, value)
#else
#define eDPTX_printData(str, value)
#define eDPTX_printMsg(str)
#define eDPTX_printf(str, value)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
#define DPTX_EDID_SIZE 512

WORD XDATA WaitRdPluseDelayCount;
WORD XDATA WaitReplyCount;

BYTE code (*eDPTxMOD_GC_SWING)[4];
BYTE code (*eDPTxMOD_GC_PREEM)[4];

BYTE code eDPTXMODSetSwLv[5];
BYTE code eDPTXMODSetPreLv[5];
//#if eDP_SwTBL_ShiftLv == 3
BYTE code eDPTxMOD_GC_SWING_Lv3[4][4]={{0x72,0x72,0x72,0x72},{0x72,0x72,0x72,0x72},{0x72,0x72,0x72,0x72},{0x72,0x72,0x72,0x72}};
BYTE code eDPTxMOD_GC_PREEM_Lv3[4][4]={{0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00}};
//#elif eDP_SwTBL_ShiftLv == 2
BYTE code eDPTxMOD_GC_SWING_Lv2[4][4]={{0x4d,0x72,0x72,0x72},{0x72,0x72,0x72,0x72},{0x72,0x72,0x72,0x72},{0x72,0x72,0x72,0x72}};
BYTE code eDPTxMOD_GC_PREEM_Lv2[4][4]={{0x00,0x09,0x09,0x09},{0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00}};
//#elif eDP_SwTBL_ShiftLv == 1
BYTE code eDPTxMOD_GC_SWING_Lv1[4][4]={{0x30,0x45,0x5f,0x5f},{0x4d,0x72,0x72,0x72},{0x72,0x72,0x72,0x72},{0x72,0x72,0x72,0x72}};
BYTE code eDPTxMOD_GC_PREEM_Lv1[4][4]={{0x00,0x06,0x0f,0x0f},{0x00,0x09,0x09,0x09},{0x00,0x00,0x00,0x00},{0x00,0x00,0x00,0x00}};
//#else
BYTE code eDPTxMOD_GC_SWING_Lv0[4][4]={{0x1b,0x25,0x30,0x4d},{0x30,0x45,0x5f,0x5f},{0x4d,0x72,0x72,0x72},{0x72,0x72,0x72,0x72}};
BYTE code eDPTxMOD_GC_PREEM_Lv0[4][4]={{0x00,0x04,0x09,0x17},{0x00,0x06,0x0f,0x0f},{0x00,0x09,0x09,0x09},{0x00,0x00,0x00,0x00}};
//#endif

//BYTE code eDPTX_HDCP_KEY[289];// ={0x0};


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//**************************************************************************
//  [Function Name]:
//                  _mdrv_eDPTx_PortNumber2eDPTxID(BYTE ucInputPort)
//  [Description]
//
//  [Arguments]:
//
//  [Return]: edptx_id
//
//**************************************************************************
eDPTx_ID _mdrv_eDPTx_PortNumber2eDPTxID(BYTE ucInputPort)
{
    switch(ucInputPort)
    {
        case 0 :
            return eDPTx_ID_0;
            break;

        case 1 :
            return eDPTx_ID_1;
            break;

        case 2 :
            return eDPTx_ID_2;
            break;

        case 3 :
            return eDPTx_ID_3;
            break;
        default :
            eDPTX_printMsg("ERROR !: eDPTx ID Error\r\n");
            break;
    }


    return eDPTx_ID_MAX;     //Error ID

}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_Init1Time( void )
//  [Description] : Init HPD, Atop, PNSwap, CLK
//
//  [Arguments]:
//
//  [Return]: None
//
//**************************************************************************
void mdrv_eDPTx_Init1Time(void)
{
    mhal_eDPTx_CheckIDInfo();
    //mhal_eDPTx_PowerDown();

    if ((g_sPnlInfo.sPnlTiming.u16Htt-g_sPnlInfo.sPnlTiming.u16Width)<120)
    {
        mhal_eDPTx_TBCFIFO_Overwrite();
    }

//  memset(eDPTXHBR2SetSwLv, 0x0, sizeof(eDPTXHBR2SetSwLv));
    //memset(eDPTXHBR2SetPreLv, 0x0, sizeof(eDPTXHBR2SetSwLv));

    mhal_eDPTx_ReadEFuseFlow();
    // Set MFT mode
    mhal_eDPTx_MFTEnable(); // MFT   for MST93U

    return;
}
//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_Init( void )
//  [Description] : Init HPD, Atop, PNSwap, CLK
//
//  [Arguments]:
//
//  [Return]: None
//
//**************************************************************************
void mdrv_eDPTx_Init(BYTE ucInputPort)
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    mhal_eDPTx_SetPHY_IdlePattern(edptx_id, TRUE);
    mhal_eDPTx_InitialSetting_2(edptx_id);
    mhal_eDPTx_AuxInit(edptx_id);
    eDPTX_printData("\r\n>> eDP TX Init Port[%x] <<\r\n",edptx_id);

    if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_4B )
    {
        eDPTX_printMsg("PANEL_OUTPUT_FMT_4B\r\n");
    }
    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        // ## Overwrite HPD
        mhal_eDPTx_SetHPDOV(edptx_id);
        // ## HPD initial setting
        mhal_eDPTx_HPDInitSet(edptx_id);
    }
    else
    {
        // ## Overwrite HPD
        mhal_eDPTx_SetHPDOVHigh(edptx_id);
    }
    mhal_eDPTx_SetAtop(edptx_id);
    mhal_eDPTx_AuxPNSwap(edptx_id);
    mhal_eDPTx_AuxClkDebunce(edptx_id);
    mhal_eDPTx_MFTEnable(); // MFT   for MST93U

    eDPTX_printData("eDPTX rate=[%x] Mode Init \r\n",g_sPnlInfo.u8PnlEdpLinkRate);

    // ------------------------------
    //  DP MAC initial script
    // ------------------------------
    mhal_eDPTx_MACInit(edptx_id);


    // initial sw/pre

    mhal_eDPTx_LanePNSwap(edptx_id);
    mdrv_eDPTx_MLSignalEn(edptx_id, FALSE);
    if( g_sPnlInfo.u8PnlEdpSwTBLShiftLv == 3 )
    {
        eDPTxMOD_GC_SWING = eDPTxMOD_GC_SWING_Lv3;
        eDPTxMOD_GC_PREEM = eDPTxMOD_GC_PREEM_Lv3;
    }
    else if( g_sPnlInfo.u8PnlEdpSwTBLShiftLv == 2 )
    {
        eDPTxMOD_GC_SWING = eDPTxMOD_GC_SWING_Lv2;
        eDPTxMOD_GC_PREEM = eDPTxMOD_GC_PREEM_Lv2;
    }
    else if( g_sPnlInfo.u8PnlEdpSwTBLShiftLv == 1 )
    {
        eDPTxMOD_GC_SWING = eDPTxMOD_GC_SWING_Lv1;
        eDPTxMOD_GC_PREEM = eDPTxMOD_GC_PREEM_Lv1;
    }
    else
    {
        eDPTxMOD_GC_SWING = eDPTxMOD_GC_SWING_Lv0;
        eDPTxMOD_GC_PREEM = eDPTxMOD_GC_PREEM_Lv0;
    }

    mdrv_eDPTx_MODSetSwPre(g_sPnlInfo.u8PnlEdpLinkRate, 4, 0, 0, edptx_id);              // swingLv=0 , PreLv=0;
    //mhal_eDPTx_SwingTune(4,g_sPnlInfo.u8PnlEdpSwDftValue,edptx_id);            // swing = 0x2E
    //mhal_eDPTx_PreEmphasisTune(4,g_sPnlInfo.u8PnlEdpPreemphasisDftValue,edptx_id); // pre = 0x00
    return;

}




//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_SetMSA(BYTE ucInputPort)
//  [Description]
//
//  [Arguments]:
//
//  [Return]: None
//
//**************************************************************************
void mdrv_eDPTx_SetMSA(BYTE ucInputPort)
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    eDPTX_printData("eDP TX Set P%x MSA",edptx_id);
    // eDP TX
    mhal_eDPTx_ColorMISC(edptx_id);
    if (g_sPnlInfo.bPnlEdpP0LaneSwap||g_sPnlInfo.bPnlEdpP1LaneSwap)
    {
        mhal_eDPTx_LaneSwap(edptx_id, 1);
    }
    else
    {
        mhal_eDPTx_LaneSwap(edptx_id, 0);
    }

    mhal_eDPTx_ColorDepth(edptx_id);
    //mhal_eDPTx_EnhanceFrameMode(edptx_id,1);
    mhal_eDPTx_SetTU(edptx_id);
    eDPTX_printData("MSA Hwidth[%d]",eDPTXMSA_Hwidth);
    eDPTX_printData("MSA Vheight[%d]",eDPTXMSA_Vheight);
    eDPTX_printData("MSA Htotal[%d]",eDPTXMSA_Htotal);
    eDPTX_printData("MSA Vtotal[%d]",eDPTXMSA_Vtotal);

    mhal_eDPTx_TimingSet(edptx_id);

    //##################  Video PG ############################################
    mhal_eDPTx_VideoPG(edptx_id);
    //mhal_eDPTx_FSyncSet();
    mhal_eDPTx_PGEnable(edptx_id);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_FineTuneTU(BYTE ucInputPort, DWORD eDPTX_OUTBL_PixRateKhz)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_FineTuneTU_Disable(BYTE ucInputPort)
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    mhal_eDPTx_FineTuneTU_Disable(edptx_id);
    return;
}
//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_FineTuneTU(BYTE ucInputPort, DWORD eDPTX_OUTBL_PixRateKhz)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_FineTuneTU(BYTE ucInputPort, DWORD eDPTX_OUTBL_PixRateKhz)
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    mhal_eDPTx_FineTuneTU(edptx_id, eDPTX_OUTBL_PixRateKhz);
    return;
}


//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_SetSCFastLPLL(BOOL IsVGA, WORD VGAHtt, WORD VGAVtt, WORD InVFreq)
//  [Description]
//
//  [Arguments]:
//
//  [Return]: None
//
//**************************************************************************
void mdrv_eDPTx_SetSCFastLPLL(BOOL IsVGA, WORD VGAHtt, WORD VGAVtt, WORD InVFreq)
{
    DWORD SCinHotoal;
    DWORD SCinVotoal;
    DWORD SCOutHotoal;
    DWORD SCOutVotoal;
    DWORD SCLPLL_InDiv;
    DWORD SCLPLL_OutDiv;

    if(IsVGA)
    {
        SCinVotoal=VGAVtt; // Vtotal
        SCinHotoal=VGAHtt; // Htotal
    }
    else
    {
        SCinVotoal = mhal_eDPTx_InputVTotal(); // Vtotal
        SCinHotoal = mhal_eDPTx_InputHTotal(); // Htotal
    }

    SCOutHotoal=mhal_eDPTx_OutputHTotal(); // Htotal
    SCOutVotoal=mhal_eDPTx_OutputVTotal(); // Vtotal
    eDPTX_printData("[eDP] In Ht %d",SCinHotoal);
    eDPTX_printData("[eDP] In Vt %d",SCinVotoal);
    eDPTX_printData("[eDP] In VFeq %d",InVFreq);
    eDPTX_printData("[eDP] Ot Ht %d",SCOutHotoal);
    eDPTX_printData("[eDP] Ot Vt %d",SCOutVotoal);
    SCLPLL_InDiv=SCinHotoal*(SCinVotoal+1);

    if(IsVGA)
    {
        SCLPLL_InDiv=SCLPLL_InDiv+0x10000;
    }
    SCLPLL_OutDiv=(SCOutHotoal+1)*(SCOutVotoal+1)*(600/InVFreq);
    eDPTX_printData("[eDP] In DivH %x",SCLPLL_InDiv>>16);
    eDPTX_printData("[eDP] In DivL %x",SCLPLL_InDiv);
    eDPTX_printData("[eDP] Out DivH %x",SCLPLL_OutDiv>>16);
    eDPTX_printData("[eDP] Out DivL %x",SCLPLL_OutDiv);
    mhal_eDPTx_ClkDividerSet(SCLPLL_InDiv, SCLPLL_OutDiv);
    //mhal_eDPTx_LPLLLockMainWindow();
    return;
}

//**************************************************************************
//  [Function Name]:
//                  DPTxAuxDPCDReadByte( BYTE PortNum, DWORD  DPCDADDR , BYTE *pRdData)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxDPCDReadByte( BYTE ucInputPort, DWORD  DPCDADDR , BYTE *pRdData) //Poll from Raptor
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE HPDPortNum=0;
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum = edptx_id;
        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {

        }
        else {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    for(RetryCnt = 0; RetryCnt < 5;RetryCnt++)
    {
        WaitReplyCount  =AuxWaitReplyLpCntNum;
        mhal_eDPTx_AuxReadCMDByte(edptx_id,DPCDADDR);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            if(mhal_eDPTx_CheckWritePoint(edptx_id) == 1) // [3:0]: Write Point (RX LENG) [7:4] Read Point
            {
                bVaildCmd = TRUE;
                break;
            }
        }
        ReplyCmd = mhal_eDPTx_AuxRxReplyCMD(edptx_id);

        if( (ReplyCmd==0x00)&&(bVaildCmd==TRUE))
        {
            break;
        }
        else{
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id);
        } // [0] Complete CMD ->Clear RX Buf

        if(bVaildCmd==FALSE)
        {
            eDPTX_printData("SINK ACK TimeOut => R DPCD[0x%x] retry",DPCDADDR);
        }
        else
        {
            eDPTX_printData("SINK DEF/NACK => R DPCD[0x%x] retry",DPCDADDR);
        }
    }

    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id);  // [0] Complete CMD ->Clear RX Buf
        return FALSE;
    }

    if(bVaildCmd)
    {
        mhal_eDPTx_TrigerReadPulse0(edptx_id);
        mhal_eDPTx_TrigerReadPulse1(edptx_id);
        WaitRdPluseDelayCount=AuxWaitRdPluseLpCntNum;
        while(WaitRdPluseDelayCount--)
            _nop_();
        *pRdData=mhal_eDPTx_AuxReadData(edptx_id);          // [7:0]: Read DATA
        //eDPTX_printData("WaitReplyCount:%d",WaitReplyCount);
        //eDPTX_printData("R DPCD ADR:%x",DPCDADDR);
        //printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
        eDPTX_printData("D:%d",*pRdData);
    }else
    {
        eDPTX_printMsg("R TimeOut");
    }
    mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    return bVaildCmd;
}

//**************************************************************************
//  [Function Name]:
//                  DPTxAuxDPCDReadBytes( BYTE PortNum, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxDPCDReadBytes( BYTE ucInputPort, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf) //poll from Raptor
{
    BYTE RetryCnt;
    Bool bVaildCmd = FALSE;
    BYTE ReplyCmd = 0xFF;
    BYTE RdCount;
    BYTE HPDPortNum = 0;

    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum = edptx_id;
        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {

        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    //eDPTX_printData("edptx_id=[%x]",edptx_id);
    for(RetryCnt = 0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount = AuxWaitReplyLpCntNum;
        mhal_eDPTx_AuxClearIRQ(edptx_id);// [6:0] Clear irq
        if((Length==0) || (Length>16))
        {
            return bVaildCmd;
        }
        //## AUXTX CMD
        mhal_eDPTx_AuxReadCMDBytes(edptx_id, Length, DPCDADDR);

        //eDPTX_printData("DPCD ADR:%x",DPCDADDR);

        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            if(mhal_eDPTx_CheckWritePoint(edptx_id)) // [3:0]: Write Point (RX LENG) [7:4] Read Point
            {
                bVaildCmd=TRUE;
                break;
            }
            if(mhal_eDPTx_CheckAuxRxFull(edptx_id)) // [1]: Write Point  Buf Full
            {
                bVaildCmd=TRUE;
                break;
            }
        }
        ReplyCmd=mhal_eDPTx_AuxRxReplyCMD(edptx_id);
        if((ReplyCmd == 0x00)&&(bVaildCmd == TRUE))
        {
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
        }

        if(bVaildCmd==FALSE)
        {
            eDPTX_printData("SINK ACK TimeOut => R DPCD[0x%x] retry",DPCDADDR);
        }
        else
        {
            eDPTX_printData("SINK DEF/NACK => R DPCD[0x%x] retry",DPCDADDR);
        }
    }
    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id);  // [0] Complete CMD ->Clear RX Buf
        return FALSE;
    }
    if(bVaildCmd)
    {
        //eDPTX_printData("R DPCD ADR:%x",DPCDADDR);
        //eDPTX_printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
        mhal_eDPTx_TrigerReadPulse0(edptx_id); // [1]: set 0
        for(RdCount=0;RdCount<Length;RdCount++)
        {
            mhal_eDPTx_TrigerReadPulse1(edptx_id); // [0]: triger read pulse
            WaitRdPluseDelayCount=AuxWaitRdPluseLpCntNum;
            while(WaitRdPluseDelayCount--)
            {
                _nop_();
            }

            *(pRxBuf+RdCount) = mhal_eDPTx_AuxReadData(edptx_id);          // [7:0]: Read DATA
        //eDPTX_printData("D:%x",*(pRxBuf+RdCount));
        }
    }else
    {
        eDPTX_printMsg("R TimeOut");
    }
    mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    //mhal_eDPTx_AuxClearIRQ(edptx_id);// [6:0] Clear irq
    return bVaildCmd;
}



//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_AuxDPCDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxDPCDWriteByte( BYTE ucInputPort, DWORD  DPCDADDR , BYTE wData) //poll from Raptor
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE HPDPortNum=0;
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum=edptx_id;

        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {
        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    //mhal_eDPTx_TxComplete(edptx_id);
    //WaitReplyCount=AuxWaitReplyLpCntNum;

    //mhal_eDPTx_AuxClearIRQ(edptx_id);
    //DELAY_NOP(20);
    for(RetryCnt=0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount=AuxWaitReplyLpCntNum;
        mhal_eDPTx_AuxClearIRQ(edptx_id); // [6:0] Clear irq
        mhal_eDPTx_AuxWriteCMDByte(edptx_id, DPCDADDR, wData);
        //eDPTX_printData("DPCD ADR:%x",DPCDADDR);
        //eDPTX_printData("W:%x",wData);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            //if(mhal_eDPTx_AuxRXCompleteInterrupt(edptx_id)// [3] Rx Cmd irq
            if(mhal_eDPTx_AuxWriteCheckComplete(edptx_id)) // [6] Rx Transaction Complete
            {
                bVaildCmd = TRUE;
                break;
            }
        }

        ReplyCmd = mhal_eDPTx_AuxRxReplyCMD(edptx_id);
        if( (ReplyCmd == 0x00)&&(bVaildCmd == TRUE))
        {
            break;
            //mhal_eDPTx_TxComplete(edptx_id);
            //mhal_eDPTx_AuxClearIRQ(edptx_id);
            //DELAY_NOP(20); //Wait ISR clean done because the hardware is run 12MHz
            //return FALSE;
        }
        else
        {   // [0] Complete CMD ->Clear RX Buf
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id);
        }

            //mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf

        if(bVaildCmd==FALSE)
        {
            eDPTX_printData("SINK ACK TimeOut => W DPCD[0x%x] retry",DPCDADDR);
        }
        else
        {
            eDPTX_printData("SINK DEF/NACK => W DPCD[0x%x] retry",DPCDADDR);
        }

    }

    //mhal_eDPTx_AuxClearIRQ(edptx_id);
    //DELAY_NOP(20);


    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id);  // [0] Complete CMD ->Clear RX Buf
        return FALSE;
    }

    if(bVaildCmd)
    {
        //eDPTX_printData("W DPCD ADR:%x",DPCDADDR);
        //eDPTX_printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
    }else
    {
        eDPTX_printMsg("W TimeOut");
    }
    mhal_eDPTx_TxComplete(edptx_id);

    return bVaildCmd;
}
//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_AuxDPCDWriteBytes(BYTE PortNum, DWORD  DPCDADDR, BYTE Length ,BYTE *pTxBuf)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxDPCDWriteBytes(BYTE ucInputPort, DWORD  DPCDADDR, BYTE Length ,BYTE *pTxBuf) //poll from Raptor
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE LengthCnt=0;
    BYTE HPDPortNum=0;
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum=edptx_id;

        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {

        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    for(RetryCnt=0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount = AuxWaitReplyLpCntNum;
        mhal_eDPTx_AuxClearIRQ(edptx_id); // [6:0] Clear irq
        if((Length==0)||(Length>16))
        {
            return bVaildCmd;
        }
        //## AUXTX CMD
        mhal_eDPTx_AuxWriteCMDBytes(edptx_id, DPCDADDR);
        //eDPTX_printData("DPCD ADR:%x",DPCDADDR);
        for(LengthCnt = 0;LengthCnt<Length;LengthCnt++)
        {
            mhal_eTPTx_AUXTXWriteBuffer(edptx_id, LengthCnt ,pTxBuf); // [7:0] AUX TX Write DATA BUF
            WaitRdPluseDelayCount=AuxWaitWtPluseLpCntNum;
            while(WaitRdPluseDelayCount--)
            {
                _nop_();
            }
        }
        mhal_eTPTx_AUXTXWriteData(edptx_id,Length);
        //eDPTX_printData("DPCD ADR:%x",DPCDADDR);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            //if((mhal_eDPTx_AuxRXCompleteInterrupt(edptx_id))// [3] Rx Cmd irq
            if( mhal_eDPTx_AuxWriteCheckComplete(edptx_id)) //  [6] Rx Transaction Complete
            {
                bVaildCmd=TRUE;
                break;
            }
        }

        ReplyCmd=mhal_eDPTx_AuxRxReplyCMD(edptx_id);

        if( (ReplyCmd==0x00)&&(bVaildCmd==TRUE))
        {
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id);
        } // [0] Complete CMD ->Clear RX Buf

        if(bVaildCmd==FALSE)
        {
            eDPTX_printData("SINK ACK TimeOut => W DPCD[0x%x] retry",DPCDADDR);
        }
        else
        {
            eDPTX_printData("SINK DEF/NACK => W DPCD[0x%x] retry",DPCDADDR);
        }
    }

    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id);// [0] Complete CMD ->Clear RX Buf
        return FALSE;
    }

    if(bVaildCmd)
    {
        //eDPTX_printData("W DPCD ADR:%x",DPCDADDR);
        //eDPTX_printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
    }else
    {
        eDPTX_printMsg("W TimeOut");
    }
    mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    return bVaildCmd;
}



//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_AuxEDIDReadNLCMD( BYTE PortNum, DWORD  DPCDADDR )
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxEDIDReadNLCMD( BYTE ucInputPort, DWORD  DPCDADDR ) //poll from Raptor
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE HPDPortNum=0;
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum=edptx_id;

        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {
        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    for(RetryCnt=0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount=AuxWaitReplyLpCntNum;
        mhal_eDPTx_AuxClearIRQ(edptx_id); // [6:0] Clear irq
        //## AUXTX CMD
        mhal_eDPTx_AuxEDIDReadNLCMD(edptx_id, DPCDADDR);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            //if(mhal_eDPTx_CheckWritePoint(edptx_id))// [3:0]: Write Point (RX LENG) [7:4] Read Point
            //{bVaildCmd=TRUE;break;}
            // check reply cmd IRQ
            if(WaitReplyCount==1)
            {
                bVaildCmd=TRUE;
                break;
            }
        }
        ReplyCmd=mhal_eDPTx_AuxRxReplyCMD(edptx_id);
        if((ReplyCmd == 0x00)&&(bVaildCmd == TRUE))
        {
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
        }

        if(bVaildCmd == FALSE)
        {
            eDPTX_printData("SINK ACK TimeOut => R NL EDID[0x%x] retry",DPCDADDR);
        }
        else
        {
            eDPTX_printData("SINK DEF/NACK => R NL EDID[0x%x] retry",DPCDADDR);
        }
    }

    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
        return FALSE;
    }

    if(bVaildCmd)
    {
        //eDPTX_printData("WaitReplyCount:%d",WaitReplyCount);
        //eDPTX_printData("R DPCD ADR:%x",DPCDADDR);
        printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
    }
    else
    {
        eDPTX_printMsg("R TimeOut");
    }

    mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    return bVaildCmd;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_AuxEDIDWriteByte( BYTE PortNum, DWORD  DPCDADDR , BYTE wData)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxEDIDWriteByte( BYTE ucInputPort, DWORD  DPCDADDR , BYTE wData) //poll from Raptor
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE HPDPortNum=0;

    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum=edptx_id;
        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {
        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    for(RetryCnt=0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount=AuxWaitReplyLpCntNum;
        mhal_eDPTx_AuxClearIRQ(edptx_id); // [6:0] Clear irq
        //## AUXTX CMD
        mhal_eDPTx_AuxEDIDWriteByteCMD(edptx_id, DPCDADDR, wData);
        //eDPTX_printData("DPCD ADR:%x",DPCDADDR);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            if((mhal_eDPTx_AuxRXCompleteInterrupt(edptx_id)) // [3] Rx Cmd irq
             &&(mhal_eDPTx_AuxWriteCheckComplete(edptx_id))) //  [6] Rx Transaction Complete
            {
                bVaildCmd=TRUE;
                break;
            }
        }
        ReplyCmd=mhal_eDPTx_AuxRxReplyCMD(edptx_id);
        if((ReplyCmd == 0x00)&&(bVaildCmd == TRUE))
        {
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id);// [0] Complete CMD ->Clear RX Buf
        }

        if(bVaildCmd==FALSE)
        {
            eDPTX_printData("SINK ACK TimeOut => W EDID[0x%x] retry",DPCDADDR);
        }
        else
        {
            eDPTX_printData("SINK DEF/NACK => W EDID[0x%x] retry",DPCDADDR);
        }
    }

    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id);// [0] Complete CMD ->Clear RX Buf
        return FALSE;
    }
    if(bVaildCmd)
    {
        //eDPTX_printData("W DPCD ADR:%x",DPCDADDR);
        //eDPTX_printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
    }else
    {
        eDPTX_printMsg("W TimeOut");
    }
    mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    return bVaildCmd;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_AuxEDIDReadBytes( BYTE ucInputPort, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf)
//  [Description]
//
//  [Arguments]:
//
//  [Return]: bVaildCmd
//
//**************************************************************************
Bool mdrv_eDPTx_AuxEDIDReadBytes( BYTE ucInputPort, DWORD  DPCDADDR , BYTE Length , BYTE *pRxBuf) //poll from Raptor
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE RdCount;
    BYTE HPDPortNum=0;

    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum=edptx_id;
        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {

        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    for(RetryCnt=0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount = AuxWaitReplyLpCntNum+1000;
        mhal_eDPTx_AuxClearIRQ(edptx_id); // [6:0] Clear irq
        if((Length==0)||(Length>16))
        {
            return bVaildCmd;
        }
        //## AUXTX CMD
        mhal_eDPTx_AuxEDIDReadBytesCMD(edptx_id, DPCDADDR, Length);
        //eDPTX_printData("DPCD ADR:%x",DPCDADDR);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            if(mhal_eDPTx_CheckWritePoint(edptx_id)) // [3:0]: Write Point (RX LENG) [7:4] Read Point
            {
                bVaildCmd = TRUE;
                break;
            }

            if(mhal_eDPTx_CheckAuxRxFull(edptx_id)) // [1]: Write Point  Buf Full
            {
                bVaildCmd=TRUE;
                break;
            }
        }

        ReplyCmd=mhal_eDPTx_AuxRxReplyCMD(edptx_id);
        if((ReplyCmd==0x00)&&(bVaildCmd==TRUE))
        {
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id);
        } // [0] Complete CMD ->Clear RX Buf

        if(bVaildCmd == FALSE)
        {
            eDPTX_printData("SINK ACK TimeOut => R EDID[0x%x] retry",DPCDADDR);
        }
        else
        {
            eDPTX_printData("SINK DEF/NACK => R EDID[0x%x] retry",DPCDADDR);
        }
    }

    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id);// [0] Complete CMD ->Clear RX Buf
        return FALSE;
    }

    if(bVaildCmd)
    {
        //eDPTX_printData("R DPCD ADR:%x",DPCDADDR);
        //eDPTX_printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
        mhal_eDPTx_TrigerReadPulse0(edptx_id); // [1]: set 0
        for(RdCount = 0;RdCount<Length;RdCount++)
        {
            mhal_eDPTx_TrigerReadPulse1(edptx_id);// [0]: triger read pulse
            WaitRdPluseDelayCount=AuxWaitRdPluseLpCntNum;
            while(WaitRdPluseDelayCount--)
            {
                _nop_();
            }

            *(pRxBuf+RdCount)=mhal_eDPTx_AuxReadData(edptx_id);          // [7:0]: Read DATA
            //eDPTX_printData("D:%x",*(pRxBuf+RdCount));
        }
    }
    else
    {
        eDPTX_printMsg("R TimeOut");
    }
    mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    return bVaildCmd;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_ReadEDID(BYTE PortNum)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

Bool mdrv_eDPTx_ReadEDID(BYTE ucInputPort)//poll from Raptor
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);
    BYTE pReadBuffer[0x10];
    BYTE i,cp_idx;
    BYTE bTempValue;
    BYTE AuxCmdTotalCnt;
    BYTE ExtendBlocks;
    BYTE ExtBkIdx; // 0: 0~127 , 1: 128~255 , 2:256~383 , 4:384~511

    ExtendBlocks = 0;   // initial ExtenBlocks
    ExtBkIdx = 0;   // initial ExtBkIdx
    //memset(pReadBuffer, 0x0, sizeof(pReadBuffer));
    //memset(gDPTxInfo[dpTx_ID].DPTX_EDID, 0x0, DPTX_EDID_SIZE);
    AuxCmdTotalCnt = DPTX_EDID_SIZE/16;

    for(i = 0x0; i < AuxCmdTotalCnt; i++)  //0x20 stands for support 512 bytes EDID
    {
        if( g_sPnlInfo.bPnlEdpDETECTHPD )
        {
            if(mdrv_eDPTx_CheckHPD(edptx_id) == FALSE)
            {
                eDPTX_printMsg("[eDPTX] HPD Low at Read EDID Flow !!! \r\n");
                return FALSE;
            }
        }

        if((i % 0x8) == 0x0)
        {
            bTempValue = (i*0x10)%0x100;
            mdrv_eDPTx_AuxEDIDWriteByte(edptx_id, AUX_EDID_SLAVE_ADDR, bTempValue);
        }

        if((i%0x8) == 0x7)
        {
            mdrv_eDPTx_AuxEDIDReadBytes(edptx_id, AUX_EDID_SLAVE_ADDR, 0x10, pReadBuffer);
            for(cp_idx = 0; cp_idx < 0x10; cp_idx++) // cp auxbuf to EDIDbuf
            {
                //gDPTxInfo[dpTx_ID].DPTX_EDID[i*16+cp_idx] = pReadBuffer[cp_idx];
                if(pReadBuffer[cp_idx]<0x10)
                {
                    eDPTX_printf("0x0%x,",pReadBuffer[cp_idx]);
                }
                else
                {
                    eDPTX_printf("0x%x,",pReadBuffer[cp_idx]);
                }
            }

            if(i == 0x7)
            {
                ExtendBlocks = pReadBuffer[0xE]; // 0x7E => update ExtenBlocks
            }

            //if(bTxAutoTestEnable==TRUE)
            //{
            //    mdrv_DPTx_AuxWrite_Bytes(edptx_id, AUX_CMD_NATIVE_W, DPCD_00261, 0x1, &pReadBuffer[0xF]); // DPCD 261 store EDID checksum
                //DPTX_DRV_printData("[DPTX] Read EDID BLOCK[%x] \r\n", ExtBkIdx );
                //DPTX_DRV_printData("[DPTX] Read EDID CHECKSUM = %x \r\n", pReadBuffer[0xF] );
           // }

            if(ExtBkIdx==ExtendBlocks)
            {
                break; //EDID read done.
            }

            if ( ExtBkIdx > 0 ) // Read more than 256Bytes
            {
                if(ExtBkIdx<=ExtendBlocks)
                {
                    bTempValue = (ExtBkIdx+1)/2;
                    //mdrv_DPTx_AuxRead_Bytes(dpTx_ID, AUX_CMD_I2C_R_MOT0, AUX_EDID_SEGMENT_ADDR, 0x1, &bTempValue);
                    mdrv_eDPTx_AuxEDIDWriteByte(edptx_id, AUX_EDID_SEGMENT_ADDR, bTempValue);
                }
                else
                {
                    break; //EDID read done.
                }
            }

            ExtBkIdx++; // increase Etend Block Counter
        }
        else
        {
            mdrv_eDPTx_AuxEDIDReadBytes(edptx_id, AUX_EDID_SLAVE_ADDR, 0x10, pReadBuffer);
            for(cp_idx = 0; cp_idx < 0x10; cp_idx++) // cp auxbuf to EDIDbuf
            {
                if(pReadBuffer[cp_idx]<0x10)
                {
                    eDPTX_printf("0x0%x,",pReadBuffer[cp_idx]);
                }
                else
                {
                    eDPTX_printf("0x%x,",pReadBuffer[cp_idx]);
                }
                //gDPTxInfo[edptx_id].DPTX_EDID[i*16+cp_idx] = pReadBuffer[cp_idx];
                //DPTX_DRV_printData("%x",gDPTxInfo[dpTx_ID].DPTX_EDID[i*16+cp_idx]);
            }
        }
        eDPTX_printf("%d\n",0);
    }
    return TRUE;
}


//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_AuxMCCSReadBytes( BYTE ucInputPort, BYTE Length, BYTE EOF, BYTE *pRxBuf)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxMCCSReadBytes( BYTE ucInputPort, BYTE Length, BYTE EOF, BYTE *pRxBuf)
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE RdCount;
    BYTE HPDPortNum=0;

    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum = edptx_id;
        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {

        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    for(RetryCnt=0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount = AuxWaitReplyLpCntNum+1000;
        mhal_eDPTx_AuxClearIRQ(edptx_id); // [6:0] Clear irq
        if((EOF==0)&&((Length==0)||(Length>16)))
        {
            return bVaildCmd;
        }
        //## AUXTX CMD
        mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear Buf
        mhal_eDPTx_AuxMCCSReadBytesCMD(edptx_id, Length, EOF);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            if(EOF == 0)
            {
                if(mhal_eDPTx_CheckWritePoint(edptx_id)) // [3:0]: Write Point (RX LENG) [7:4] Read Point
                {
                    bVaildCmd=TRUE;
                    break;
                }
                if(mhal_eDPTx_CheckAuxRxFull(edptx_id)) // [1]: Write Point  Buf Full
                {
                    bVaildCmd=TRUE;
                    break;
                }
            }else
            {
                if(WaitReplyCount == 1)
                {
                    bVaildCmd=TRUE;
                    break;
                }
            }
        }

        ReplyCmd=mhal_eDPTx_AuxRxReplyCMD(edptx_id);
        if( (ReplyCmd == 0x00)&&(bVaildCmd == TRUE))
        {
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
        }

        if(bVaildCmd == FALSE)
        {
            eDPTX_printMsg("SINK ACK TimeOut => R MCCS retry");
        }
        else
        {
            eDPTX_printMsg("SINK DEF/NACK => R MCCS retry");
        }
    }

    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
        eDPTX_printMsg("eDP MCCS READ Cmd =>SINK DEF/NACK");
        return FALSE;
    }

    if(bVaildCmd)
    {
        if(EOF == 0)
        {
            mhal_eDPTx_TrigerReadPulse0(edptx_id); // [1]: set 0
            for(RdCount=0;RdCount<Length;RdCount++)
            {
                mhal_eDPTx_TrigerReadPulse1(edptx_id); // [0]: triger read pulse
                WaitRdPluseDelayCount=AuxWaitRdPluseLpCntNum;
                while(WaitRdPluseDelayCount--)
                {
                    _nop_();
                }
                *(pRxBuf+RdCount) = mhal_eDPTx_AuxReadData(edptx_id); // [7:0]: Read DATA
                //eDPTX_printData("D:%x",*(pRxBuf+RdCount));
            }
        }
    }
    else
    {
        eDPTX_printMsg("R MCCS TimeOut");
    }
        mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    return bVaildCmd;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_AuxMCCSWriteBytes(BYTE ucInputPort, BYTE Length, BYTE EOF ,BYTE *pTxBuf)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AuxMCCSWriteBytes(BYTE ucInputPort, BYTE Length, BYTE EOF ,BYTE *pTxBuf)
{
    BYTE RetryCnt;
    Bool bVaildCmd=FALSE;
    BYTE ReplyCmd=0xFF;
    BYTE LengthCnt=0;
    BYTE HPDPortNum=0;

    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum = edptx_id;
        if(mdrv_eDPTx_CheckHPD(HPDPortNum))
        {
        }
        else
        {
            eDPTX_printMsg("[AUX] HPD low");
            return FALSE;
        }
    }

    for(RetryCnt = 0;RetryCnt<5;RetryCnt++)
    {
        WaitReplyCount = AuxWaitReplyLpCntNum;
        mhal_eDPTx_AuxClearIRQ(edptx_id); // [6:0] Clear irq
        if((EOF==0)&&((Length==0)||(Length>16)))
        {
            return bVaildCmd;
        }
        //## AUXTX CMD
        mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear Buf
        mhal_eDPTx_AuxMCCSWriteBytesCMD(edptx_id, EOF);
        for(LengthCnt = 0;LengthCnt<Length;LengthCnt++)
        {
            mhal_eTPTx_AUXTXWriteBuffer(edptx_id, LengthCnt ,pTxBuf); // [7:0] AUX TX Write DATA BUF
            WaitRdPluseDelayCount=AuxWaitWtPluseLpCntNum;
            while(WaitRdPluseDelayCount--)
            {
                _nop_();
            }
        }
        mhal_eTPTx_AUXTXWriteDataBytes(edptx_id, Length, EOF);
        // ## AUXTX RXBUF
        while(WaitReplyCount--)
        {
            _nop_();
            //if(mhal_eDPTx_AuxRXCompleteInterrupt(BYTE edptx_id)) // [3] Rx Cmd irq
            if(mhal_eDPTx_AuxWriteCheckComplete(edptx_id)) //  [6] Rx Transaction Complete
            {
                bVaildCmd=TRUE;
                break;
            }
        }

        ReplyCmd=mhal_eDPTx_AuxRxReplyCMD(edptx_id);
        if((ReplyCmd == 0x00)&&(bVaildCmd == TRUE))
        {
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TxComplete(edptx_id);// [0] Complete CMD ->Clear RX Buf

        }

        if(bVaildCmd==FALSE)
        {
            eDPTX_printMsg("SINK ACK TimeOut => W MCCS retry");
        }
        else
        {
            eDPTX_printMsg("SINK DEF/NACK => W MCCS retry");
        }
    }

    if(ReplyCmd)
    {
        //mhal_eDPTx_TxComplete(edptx_id);   // [0] Complete CMD ->Clear RX Buf
        eDPTX_printMsg("eDP MCCS WRITE Cmd =>SINK DEF/NACK");
        return FALSE;
    }

    if(bVaildCmd)
    {
      //eDPTX_printData("W DPCD ADR:%x",DPCDADDR);
      //eDPTX_printData("REPLY CMD:%x",ReplyCmd);// [3:0]: Rx CMD
    }else
    {
      eDPTX_printMsg("W MCCS TimeOut");
    }
    mhal_eDPTx_TxComplete(edptx_id); // [0] Complete CMD ->Clear RX Buf
    return bVaildCmd;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_ReadDPCDLinkStatus(BYTE ucInputPort)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_ReadDPCDLinkStatus(BYTE ucInputPort)//poll from Raptor : no need change
{
    BYTE AuxRxBuf[3];
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000202,3,AuxRxBuf))
    {
        eDPTX_printData("P%x Link Status",edptx_id);
        eDPTX_printData("[%x]DPCD202",AuxRxBuf[0]); // 202
        eDPTX_printData("[%x]DPCD203",AuxRxBuf[1]); // 203
        eDPTX_printData("[%x]DPCD204",AuxRxBuf[2]); // 204

        if(g_sPnlInfo.u8PnlEdpLaneCnt==1)
        {
            if((AuxRxBuf[0] == 0x07)&&((AuxRxBuf[2]&0x01) == 0x01))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else if(g_sPnlInfo.u8PnlEdpLaneCnt==2)
        {
            if((AuxRxBuf[0] == 0x77)&&((AuxRxBuf[2]&0x01) == 0x01))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            if((AuxRxBuf[0] == 0x77)&&(AuxRxBuf[1] == 0x77)&&((AuxRxBuf[2]&0x01) == 0x01))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }

    }
    else
    {
        return FALSE;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_ConfigLinkRate(BYTE LinkRate, BYTE ucInputPort)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_ConfigLinkRate(BYTE LinkRate, BYTE ucInputPort)//no need to change
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    mhal_eDPTx_ConfigLinkRate(LinkRate, edptx_id);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_SSCEnable(BYTE bEnable, BYTE LinkRate)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
// ###############################################################
// function Name: DPTxSSCEnable(BYTE bEnable, BYTE LinkRate)
// Amp < 0.5% {RBR,HBR}
// 30Khz< MFrq(ModulationFrq) < 33Khz
// ###############################################################
void mdrv_eDPTx_SSCEnable(BYTE bEnable, BYTE LinkRate, BYTE ucInputPort)//no  need to change
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    if(bEnable)
    {
        eDPTX_printMsg("eDPTX SSC Enable");
        eDPTX_printMsg("Amp=0.20");
        eDPTX_printMsg("MFrq=32Khz");
        mhal_eDPTx_SSCEnable(edptx_id, LinkRate, TRUE);//TBD
    }
    else
    {
        eDPTX_printMsg("eDPTX SSC Disable");
        mhal_eDPTx_SSCEnable(edptx_id, LinkRate, FALSE);//TBD
    }
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_MLSignalEn()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_MLSignalEn(BYTE ucInputPort, BOOL bEnable)//no need to change
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);
    //if(edptx_id==0x0){}; // dummy
    if(bEnable==0)
    {
        mhal_eDPTx_MLSignalDisable(edptx_id);
        eDPTX_printData("## eDPTX rate:[%x] PORT ML Signal Off ##\r\n",g_sPnlInfo.u8PnlEdpLinkRate);
    }else
    {
        mhal_eDPTx_MLSignalEnable(edptx_id);
        eDPTX_printData("## eDPTX rate:[%x] PORT ML Signal On ##\r\n",g_sPnlInfo.u8PnlEdpLinkRate);
    }
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_Training(BYTE ucInputPort, BYTE LinkRate, BYTE LaneCount)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_Aux_Read_Test(BYTE ucInputPort)
{
    BYTE pTxBuf[16];
    WORD Count = 0;
    BYTE xdata WdData[16]={0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10};
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);
edptx_id = edptx_id;
    for(Count = 1; Count<=16;Count++)
    {
        mdrv_eDPTx_AuxDPCDWriteBytes(0, 0x1000, Count, WdData);
        mdrv_eDPTx_AuxDPCDReadBytes(0, 0x1000, Count, pTxBuf);
    }
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_Training(BYTE ucInputPort, BYTE LinkRate, BYTE LaneCount)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_Training(BYTE ucInputPort, BYTE LinkRate, BYTE LaneCount)
{
    BYTE AuxRxBuf[8];
    BYTE AuxTxBuf[8];
    BYTE VswingLevel[4]={0,0,0,0};
    BYTE VPremphasisLevel[4]={0,0,0,0};
    BYTE VMaxSwMaxPreMask[4]={0,0,0,0};
    BYTE TrainingLoop=0;
    BYTE EnhanceFrame=0x80;
    BYTE bCDR_LOCK=FALSE;
    BYTE bEQ_LOCK=FALSE;
    BYTE HPDPortNum=0;
    BYTE u8DPCD_107=0;

    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);
    mhal_eDPTx_PatternSkew(edptx_id, 1);

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        HPDPortNum=edptx_id;
        if(mdrv_eDPTx_CheckHPD(HPDPortNum) == TRUE)
        {
        }
        else
        {
            eDPTX_printData("[Training P%d Abort] HPD low",edptx_id);
            mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);return FALSE;
        }

        if(mdrv_eDPTx_CheckHPDIRQ(HPDPortNum))
        {
            eDPTX_printData("[Training P%d Abort] HPDIRQ event",edptx_id);
            mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);return FALSE;
        }
    }

    #if 1
    // msWrite2Byte(REG_MOD1_00, 0x0601);  // dis reset
    #endif

    eDPTX_printData("######## eDP TX PORT#[%x] Training ######",edptx_id);
    eDPTX_printData("######## eDP TX LaneCount = %d ######",LaneCount);
    mdrv_eDPTx_MLSignalEn(edptx_id,TRUE);
    mdrv_eDPTx_ConfigLinkRate(LinkRate, edptx_id);
    mhal_eDPTx_ConfigLaneCount(edptx_id, LaneCount);

    if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000700,1,AuxRxBuf))
    {
        if(AuxRxBuf[0]<4)
        {
            eDPTX_printData("[1.%d]eDP Version",(AuxRxBuf[0]+1));
        }
        else if(AuxRxBuf[0]==4)
        {
            eDPTX_printMsg("[1.4a]eDP Version");
        }
        else if(AuxRxBuf[0]==5)
        {
            eDPTX_printMsg("[1.4b]eDP Version");
        }
        else
        {
            eDPTX_printMsg("eDP Version TBD");
        }
    }
    if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x00000E,1,AuxRxBuf))
    {
        eDPTX_printData("[%x] TRAINING_AUX_RD_INTERVAL * 4ms",AuxRxBuf[0]);
    }
    mdrv_eDPTx_MODSetSwPre(LinkRate, 4, 0, 0, edptx_id);              // swingLv=0 , PreLv=0;
    //mhal_eDPTx_SwingTune(edptx_id,g_sPnlInfo.u8PnlEdpSwDftValue);                        // swing = 0x2E
    //mhal_eDPTx_PreEmphasisTune(edptx_id,g_sPnlInfo.u8PnlEdpPreemphasisDftValue ); // pre = 0x00


    // Check Enhance Mode Cap. from Sink
    mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000002,1,AuxRxBuf);
    if((AuxRxBuf[0]&BIT7)>0)
    {
        eDPTX_printMsg("SINK Support EnHance Frame Mode");
        EnhanceFrame=0x80;
        mhal_eDPTx_EnhanceFrameMode(edptx_id,1);
    }else
    {
        eDPTX_printMsg("SINK Not Support EnHance Frame Mode");
        EnhanceFrame=0x00;
        mhal_eDPTx_EnhanceFrameMode(edptx_id,0);
    }


    if(g_sPnlInfo.bPnlEdpEnASSR) // method #1
    {
        mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x00000D,1,AuxRxBuf);
        eDPTX_printData("[%x]eDP Config Cap",AuxRxBuf[0]);
        if(AuxRxBuf[0]&BIT0)
        {
            eDPTX_printMsg("SINK Support ASSR");
            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00010A,BIT0);
            mhal_eDPTx_ASSR(edptx_id,1);
        }else
        {
            eDPTX_printMsg("SINK Not Support ASSR");
            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00010A,0);
            mhal_eDPTx_ASSR(edptx_id,0);
        }
    }
    else if (g_sPnlInfo.bPnlEdpEnAFR)  //method #2
    {
        mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x00000D,1,AuxRxBuf);
        eDPTX_printData("[%x]eDP Config Cap",AuxRxBuf[0]);
        if(AuxRxBuf[0]&BIT1)
        {
            eDPTX_printMsg("SINK Support AFR");
            EnhanceFrame=0;
            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00010A,BIT1);
            mhal_eDPTx_AFR(edptx_id,1);
        }else
        {
            eDPTX_printMsg("SINK Not Support AFR");
            EnhanceFrame=0x80;
            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00010A,0);

            mhal_eDPTx_AFR(edptx_id,0);
        }
    }


    if( g_sPnlInfo.bPnlEdpEnSSC )
    // read DPCD 00003 BIT0 => Enable SSC
    {
        BYTE DPCD_003;
        mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00003,&DPCD_003);
        eDPTX_printData("[%x]DPCD 003",DPCD_003);
        if(DPCD_003&BIT0)
        {
            eDPTX_printMsg("SINK Support SSC");
            u8DPCD_107 = 0x10;// DPCD 107 [4] = SPREAD_AMP   <0.5% , 30~33Khz
            MAsm_CPU_DelayMs(1);
            mdrv_eDPTx_SSCEnable(TRUE,LinkRate,edptx_id);
        }
        else
        {
            eDPTX_printMsg("SINK NOT Support SSC");
            u8DPCD_107 = 0x00;
            MAsm_CPU_DelayMs(1);
            mdrv_eDPTx_SSCEnable(FALSE,LinkRate,edptx_id);
        }
    }

    mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000007,1,AuxRxBuf);

    if((AuxRxBuf[0]&BIT6)>0)
    {
        eDPTX_printMsg("sink support ignore MSA(DPCD0x07[6]=1");
        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000107, BIT7|u8DPCD_107);
        eDPTX_printMsg("set ignore MSA Enable(DPCD0x107[7]=1");
    }
    else
    {
        eDPTX_printMsg("[eDPTX]sink not support ignore MSA(DPCD0x07[6]=0");
        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000107, u8DPCD_107);
        eDPTX_printMsg("[eDPTX]set ignore MSA Disable(DPCD0x107[7]=0");
    }

    //Read DPCD 600h
    mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000600,1,AuxRxBuf);
    eDPTX_printData("[%x]DPCD600",AuxRxBuf[0]);
    if(AuxRxBuf[0]!=0x01)
    {
        MAsm_CPU_DelayMs(1);
        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000600,0x01);
        MAsm_CPU_DelayMs(1);
    }

    //Read DevCap
    if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000000,4,AuxRxBuf))
    {
          eDPTX_printMsg("Read Dev Cap");
          eDPTX_printData("[%x]DPCD Ver.",AuxRxBuf[0]);
          eDPTX_printData("[%x]MAX LinkRate",AuxRxBuf[1]);
          eDPTX_printData("[%x]MAX LaneCount",AuxRxBuf[2]);
          eDPTX_printData("[%x]MAX DownSpread",AuxRxBuf[3]);
    }
    // DPCD Training Configure
    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000100,LinkRate); // DPCD 100 HBR=0x0A ,RBR=0x06
    MAsm_CPU_DelayMs(1);

    if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_4B )
        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000101,((EnhanceFrame) |(0x02))); // DPCD 101 Enhance Frame , LaneCount
    else
        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000101,((EnhanceFrame) |(LaneCount))); // DPCD 101 Enhance Frame , LaneCount

    MAsm_CPU_DelayMs(1);
    mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000101,1,AuxRxBuf);
    mhal_eDPTx_SetLaneCnt(edptx_id, LaneCount);
    mhal_eDPTx_TrainingPattern_Select(0,edptx_id);

    if( g_sPnlInfo.bPnlEdpNoAuxHandshakeLinkTrain )
    {
        mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000003,1,AuxRxBuf);
        eDPTX_printData("[%x]DPCD 003",AuxRxBuf[0]);
        if(AuxRxBuf[0]&BIT6)
        {
            eDPTX_printMsg("SINK Support No_AUX_HandShake_Training");
            AuxTxBuf[0]=((0x00<<3)|0x01); // DPCD 103 L0 = SW[2:0]  / Pre [5:3]
            AuxTxBuf[1]=((0x00<<3)|0x01); // DPCD 104 L1 = SW[2:0]  / Pre [5:3]
            AuxTxBuf[2]=((0x00<<3)|0x01); // DPCD 105 L2 = SW[2:0]  / Pre [5:3]
            AuxTxBuf[3]=((0x00<<3)|0x01); // DPCD 106 L3 = SW[2:0]  / Pre [5:3]
            mdrv_eDPTx_AuxDPCDWriteBytes(edptx_id,0x000103,4,AuxTxBuf);
            mdrv_eDPTx_MODSetSwPre(LinkRate,4,1,0,edptx_id);        // swingLv=1 , PreLv=0;
            //mhal_eDPTx_SwingTune(4,g_sPnlInfo.u8PnlEdpSwDftValue,edptx_id);
            //mhal_eDPTx_PreEmphasisTune(4,g_sPnlInfo.u8PnlEdpPreemphasisDftValue,edptx_id);
            mhal_eDPTx_TrainingPattern_Select(1,edptx_id);
            mhal_eDPTx_SetPHY_IdlePattern(edptx_id,FALSE); //disable idle pattern
            MAsm_CPU_DelayMs(1);
            mhal_eDPTx_TrainingPattern_Select(2,edptx_id);MAsm_CPU_DelayMs(1);
            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000102,0x00);// DPCD 102  Training Pattern  [1:0]  Scramble disable [5]
            mhal_eDPTx_TrainingPattern_Select(0,edptx_id);
            mhal_eDPTx_PatternSkew(edptx_id, 0);
            eDPTX_printData("Link Rate:[%x] Fast Link Training", LinkRate);
            eDPTX_printData("Lane Count:[%x]", LaneCount);
            eDPTX_printMsg("End Training");
            mdrv_eDPTx_VDClkRst();
            return TRUE;
        }
        else
        {
            eDPTX_printMsg("SINK Not Support No_AUX_HandShake_Training");
        }
    }

    // ##############  Training TP1 ######################
    eDPTX_printMsg("Start Training TP1");
    mhal_eDPTx_TrainingPattern_Select(1,edptx_id);
    mhal_eDPTx_SetPHY_IdlePattern(edptx_id,FALSE); //disable idle pattern
    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000102,0x21);// DPCD 102  Training Pattern  [1:0]  Scramble disable [5]
    for(TrainingLoop=0;TrainingLoop<5;TrainingLoop++)
    {
        if( g_sPnlInfo.bPnlEdpDETECTHPD )
        {
            if(mdrv_eDPTx_CheckHPD(HPDPortNum))
            {

            }
            else
            {
                eDPTX_printData("[Training P%d Abort] HPD low",edptx_id);
                mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);return FALSE;
            }

            if(mdrv_eDPTx_CheckHPDIRQ(HPDPortNum))
            {
                eDPTX_printData("[Training P%d Abort] HPDIRQ event",edptx_id);
                mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);return FALSE;
            }
        }

        eDPTX_printData("TP1 TrainingLoop Count %d",TrainingLoop);
        //>>>>>> Read Swing/PreEmphasis Request <<<<<<
        if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000206,2,AuxRxBuf))
        {
             eDPTX_printMsg("Read Swing/PreEmphasis Req");
             eDPTX_printData("[%x]DPCD 206",AuxRxBuf[0]); // L0 : sw=[1:0] pre=[3:2]
             eDPTX_printData("[%x]DPCD 207",AuxRxBuf[1]);
             // ######### Adjust swing ################
             VswingLevel[0]=AuxRxBuf[0]&0x03;
             VswingLevel[1]=(AuxRxBuf[0]>>4)&0x03;
             VswingLevel[2]=AuxRxBuf[1]&0x03;
             VswingLevel[3]=(AuxRxBuf[1]>>4)&0x03;
             // ######### Adjust pre when TPS1 ###########
             VPremphasisLevel[0]=(AuxRxBuf[0]>>2)&0x03;
             VPremphasisLevel[1]=(AuxRxBuf[0]>>6)&0x03;
             VPremphasisLevel[2]=(AuxRxBuf[1]>>2)&0x03;
             VPremphasisLevel[3]=(AuxRxBuf[1]>>6)&0x03;
             // #########################################
             if(VswingLevel[0]>2){VMaxSwMaxPreMask[0]|=BIT2;}else{VMaxSwMaxPreMask[0]&=~BIT2;}; // MAX Swing = BIT2
             if(VswingLevel[1]>2){VMaxSwMaxPreMask[1]|=BIT2;}else{VMaxSwMaxPreMask[1]&=~BIT2;}; // MAX Swing = BIT2
             if(VswingLevel[2]>2){VMaxSwMaxPreMask[2]|=BIT2;}else{VMaxSwMaxPreMask[2]&=~BIT2;}; // MAX Swing = BIT2
             if(VswingLevel[3]>2){VMaxSwMaxPreMask[3]|=BIT2;}else{VMaxSwMaxPreMask[3]&=~BIT2;}; // MAX Swing = BIT2
             eDPTX_printData("VswingLevel[0]=%x",VswingLevel[0]);
             if(VPremphasisLevel[0]>2){VMaxSwMaxPreMask[0]|=BIT5;}else{VMaxSwMaxPreMask[0]&=~BIT5;}  // MAX Pre=BIT5
             if(VPremphasisLevel[1]>2){VMaxSwMaxPreMask[1]|=BIT5;}else{VMaxSwMaxPreMask[1]&=~BIT5;}  // MAX Pre=BIT5
             if(VPremphasisLevel[2]>2){VMaxSwMaxPreMask[2]|=BIT5;}else{VMaxSwMaxPreMask[2]&=~BIT5;}  // MAX Pre=BIT5
             if(VPremphasisLevel[3]>2){VMaxSwMaxPreMask[3]|=BIT5;}else{VMaxSwMaxPreMask[3]&=~BIT5;}  // MAX Pre=BIT5
             eDPTX_printData("VPremphasisLevel[0]=%x",VPremphasisLevel[0]);
        }

             #if eDPTXAutoTest
                 mdrv_eDPTx_MODSetSwPre(LinkRate,4,VswingLevel[0],VPremphasisLevel[0],edptx_id);
             #else
                 mdrv_eDPTx_MODSetSwPre(LinkRate,0,VswingLevel[0],VPremphasisLevel[0],edptx_id);
                 mdrv_eDPTx_MODSetSwPre(LinkRate,1,VswingLevel[1],VPremphasisLevel[1],edptx_id);
                 mdrv_eDPTx_MODSetSwPre(LinkRate,2,VswingLevel[2],VPremphasisLevel[2],edptx_id);
                 mdrv_eDPTx_MODSetSwPre(LinkRate,3,VswingLevel[3],VPremphasisLevel[3],edptx_id);
                 //mhal_eDPTx_SwingTune(4,g_sPnlInfo.u8PnlEdpSwDftValue,edptx_id);
                 //mhal_eDPTx_PreEmphasisTune(4,g_sPnlInfo.u8PnlEdpPreemphasisDftValue,edptx_id); // pre = 0x00
             #endif

            AuxTxBuf[0]=VMaxSwMaxPreMask[0]|(VPremphasisLevel[0]<<3) |VswingLevel[0]; // DPCD 103 L0 = SW[2:0]  / Pre [5:3]
            AuxTxBuf[1]=VMaxSwMaxPreMask[1]|(VPremphasisLevel[1]<<3) |VswingLevel[1]; // DPCD 104 L1 = SW[2:0]  / Pre [5:3]
            AuxTxBuf[2]=VMaxSwMaxPreMask[2]|(VPremphasisLevel[2]<<3) |VswingLevel[2]; // DPCD 105 L2 = SW[2:0]  / Pre [5:3]
            AuxTxBuf[3]=VMaxSwMaxPreMask[3]|(VPremphasisLevel[3]<<3) |VswingLevel[3]; // DPCD 106 L3 = SW[2:0]  / Pre [5:3]
            mdrv_eDPTx_AuxDPCDWriteBytes(edptx_id,0x000103,4,AuxTxBuf);
            MAsm_CPU_DelayMs(1);
            eDPTX_printData("[%x]DPCD103",AuxTxBuf[0]); // 103
            eDPTX_printData("[%x]DPCD104",AuxTxBuf[1]); // 104
            eDPTX_printData("[%x]DPCD105",AuxTxBuf[2]); // 105
            eDPTX_printData("[%x]DPCD106",AuxTxBuf[3]); // 106
            // Check CDR Lock
           if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000202,3,AuxRxBuf))
           {
                eDPTX_printMsg("Link Status");
                eDPTX_printData("[%x]DPCD202",AuxRxBuf[0]); // 202
                eDPTX_printData("[%x]DPCD203",AuxRxBuf[1]); // 203
                eDPTX_printData("[%x]DPCD204",AuxRxBuf[2]); // 204
                bCDR_LOCK=FALSE;
                eDPTX_printData("[%x]LaneCount",LaneCount); // 204

                switch(LaneCount)
                {
                    case 0x01:
                        if((AuxRxBuf[0]&0x01) == 0x01)
                        {
                            bCDR_LOCK=TRUE;
                        }
                        break;

                    case 0x02:
                        if((AuxRxBuf[0]&0x11) == 0x11)
                        {
                            bCDR_LOCK=TRUE;
                        }
                        break;

                    case 0x04:
                        if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_4B )
                        {
                            if((AuxRxBuf[0]&0x11) == 0x11)
                            {
                                bCDR_LOCK = TRUE;
                            }
                        }
                        else
                        {
                            if(((AuxRxBuf[0]&0x11) == 0x11)&&((AuxRxBuf[1]&0x11)==0x11))
                            {
                                bCDR_LOCK=TRUE;
                            }
                        }
                        break;

                    default:
                        bCDR_LOCK = FALSE;
                        break;
              }

              //if(((AuxRxBuf[0]&0x11)==0x11)&&((AuxRxBuf[1]&0x11)==0x11))
              if(bCDR_LOCK == TRUE)
              {
                eDPTX_printMsg("Link Training CDR Lock!");
                break;
              }
            }

       //#####################################################################
       // Check Loop>5 => End Training
       //#####################################################################

       if((bCDR_LOCK == FALSE)&&(TrainingLoop == 4))
       {
            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000102,0x00);// DPCD 102  Training Pattern  [1:0]
            mhal_eDPTx_TrainingPattern_Select(0,edptx_id);
            eDPTX_printMsg("Training Fail");
            eDPTX_printMsg("End Training");
            mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);
            return FALSE;
       }

    }
    // ##############  Training TP2 ######################
    mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000002,1,AuxRxBuf);

    if((AuxRxBuf[0]&BIT6)>0)
    {
        eDPTX_printMsg("Start Training TP3");
        mhal_eDPTx_TrainingPattern_Select(3,edptx_id);
        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000102,0x23);// DPCD 102  Training Pattern  [1:0]  Scramble disable [5]

    }
    else
    {
        eDPTX_printMsg("Start Training TP2");
        mhal_eDPTx_TrainingPattern_Select(2,edptx_id);
        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000102,0x22);// DPCD 102  Training Pattern  [1:0]  Scramble disable [5]
    }



    for(TrainingLoop = 0;TrainingLoop < 5;TrainingLoop++)
    {
        if( g_sPnlInfo.bPnlEdpDETECTHPD )
        {
            if(mdrv_eDPTx_CheckHPD(HPDPortNum))
            {
            }
            else
            {
                eDPTX_printData("[Training P%d Abort] HPD low",edptx_id);
                mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);return FALSE;
            }

            if(mdrv_eDPTx_CheckHPDIRQ(HPDPortNum))
            {
                eDPTX_printData("[Training P%d Abort] HPDIRQ event",edptx_id);
                mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);return FALSE;
            }
        }
        if(AuxRxBuf[0]&BIT6)
        {
            eDPTX_printData("TP3 TrainingLoop Count %d",TrainingLoop);
        }else
        {
            eDPTX_printData("TP2 TrainingLoop Count %d",TrainingLoop);
        }
        // >>>>> Read Swing/PreEmphasis Request  <<<<<
        if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000206,2,AuxRxBuf))
        {
            eDPTX_printMsg("Read Swing/PreEmphasis Req");
            eDPTX_printData("[%x]DPCD 206",AuxRxBuf[0]);
            eDPTX_printData("[%x]DPCD 207",AuxRxBuf[1]);
            // ######### Adjust swing ################
            VswingLevel[0]=AuxRxBuf[0]&0x03;
            VswingLevel[1]=(AuxRxBuf[0]>>4)&0x03;
            VswingLevel[2]=AuxRxBuf[1]&0x03;
            VswingLevel[3]=(AuxRxBuf[1]>>4)&0x03;
             // ######### Adjust pre  ################
            VPremphasisLevel[0]=(AuxRxBuf[0]>>2)&0x03;
            VPremphasisLevel[1]=(AuxRxBuf[0]>>6)&0x03;
            VPremphasisLevel[2]=(AuxRxBuf[1]>>2)&0x03;
            VPremphasisLevel[3]=(AuxRxBuf[1]>>6)&0x03;
            // #########################################
            if(VswingLevel[0]>2){VMaxSwMaxPreMask[0]|=BIT2;}else{VMaxSwMaxPreMask[0]&=~BIT2;}; // MAX Swing = BIT2
            if(VswingLevel[1]>2){VMaxSwMaxPreMask[1]|=BIT2;}else{VMaxSwMaxPreMask[1]&=~BIT2;}; // MAX Swing = BIT2
            if(VswingLevel[2]>2){VMaxSwMaxPreMask[2]|=BIT2;}else{VMaxSwMaxPreMask[2]&=~BIT2;}; // MAX Swing = BIT2
            if(VswingLevel[3]>2){VMaxSwMaxPreMask[3]|=BIT2;}else{VMaxSwMaxPreMask[3]&=~BIT2;}; // MAX Swing = BIT2
            eDPTX_printData("VswingLevel[0]=%x",VswingLevel[0]);
            if(VPremphasisLevel[0]>2){VMaxSwMaxPreMask[0]|=BIT5;}else{VMaxSwMaxPreMask[0]&=~BIT5;}  // MAX Pre=BIT5
            if(VPremphasisLevel[1]>2){VMaxSwMaxPreMask[1]|=BIT5;}else{VMaxSwMaxPreMask[1]&=~BIT5;}  // MAX Pre=BIT5
            if(VPremphasisLevel[2]>2){VMaxSwMaxPreMask[2]|=BIT5;}else{VMaxSwMaxPreMask[2]&=~BIT5;}  // MAX Pre=BIT5
            if(VPremphasisLevel[3]>2){VMaxSwMaxPreMask[3]|=BIT5;}else{VMaxSwMaxPreMask[3]&=~BIT5;}  // MAX Pre=BIT5
            eDPTX_printData("VPremphasisLevel[0]=%x",VPremphasisLevel[0]);
        }
        #if eDPTXAutoTest
            mdrv_eDPTx_MODSetSwPre(LinkRate,4,VswingLevel[0],VPremphasisLevel[0],edptx_id);
        #else
            mdrv_eDPTx_MODSetSwPre(LinkRate,0,VswingLevel[0],VPremphasisLevel[0],edptx_id);
            mdrv_eDPTx_MODSetSwPre(LinkRate,1,VswingLevel[1],VPremphasisLevel[1],edptx_id);
            mdrv_eDPTx_MODSetSwPre(LinkRate,2,VswingLevel[2],VPremphasisLevel[2],edptx_id);
            mdrv_eDPTx_MODSetSwPre(LinkRate,3,VswingLevel[3],VPremphasisLevel[3],edptx_id);
            //mhal_eDPTx_SwingTune(4,g_sPnlInfo.u8PnlEdpSwDftValue,edptx_id);
            //mhal_eDPTx_PreEmphasisTune(4,g_sPnlInfo.u8PnlEdpPreemphasisDftValue,edptx_id); // pre = 0x00
        #endif
        AuxTxBuf[0]=VMaxSwMaxPreMask[0]|(VPremphasisLevel[0]<<3) |VswingLevel[0]; // DPCD 103 L0 = SW[2:0]  / Pre [5:3]
        AuxTxBuf[1]=VMaxSwMaxPreMask[1]|(VPremphasisLevel[1]<<3) |VswingLevel[1]; // DPCD 104 L1 = SW[2:0]  / Pre [5:3]
        AuxTxBuf[2]=VMaxSwMaxPreMask[2]|(VPremphasisLevel[2]<<3) |VswingLevel[2]; // DPCD 105 L2 = SW[2:0]  / Pre [5:3]
        AuxTxBuf[3]=VMaxSwMaxPreMask[3]|(VPremphasisLevel[3]<<3) |VswingLevel[3]; // DPCD 106 L3 = SW[2:0]  / Pre [5:3]
        mdrv_eDPTx_AuxDPCDWriteBytes(edptx_id,0x000103,4,AuxTxBuf);
        MAsm_CPU_DelayMs(1);
        eDPTX_printData("[%x]DPCD103",AuxTxBuf[0]); // 103
        eDPTX_printData("[%x]DPCD104",AuxTxBuf[1]); // 104
        eDPTX_printData("[%x]DPCD105",AuxTxBuf[2]); // 105
        eDPTX_printData("[%x]DPCD106",AuxTxBuf[3]); // 106
        // Check EQ&Symbol Lock , Aligned
        if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000202,3,AuxRxBuf))
        {
            eDPTX_printMsg("Link Status");
            eDPTX_printData("[%x]DPCD202",AuxRxBuf[0]); // 202
            eDPTX_printData("[%x]DPCD203",AuxRxBuf[1]); // 203
            eDPTX_printData("[%x]DPCD204",AuxRxBuf[2]); // 204
            bEQ_LOCK=FALSE;
            switch(LaneCount)
            {
                case 0x01:
                    if(((AuxRxBuf[0]&0x07)==0x07) &&((AuxRxBuf[2]&0x01)==0x01))
                        {bEQ_LOCK=TRUE;}
                    break;

                case 0x02:
                    if(((AuxRxBuf[0]&0x77)==0x77) &&((AuxRxBuf[2]&0x01)==0x01))
                        {bEQ_LOCK=TRUE;}
                    break;

                case 0x04:
                    if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_4B )
                    {
                         if(((AuxRxBuf[0]&0x77)==0x77)&&((AuxRxBuf[2]&0x01)==0x01))
                            {bEQ_LOCK=TRUE;}
                    }
                    else
                    {
                         if(((AuxRxBuf[0]&0x77)==0x77)&&((AuxRxBuf[1]&0x77)==0x77)&&((AuxRxBuf[2]&0x01)==0x01))
                            {bEQ_LOCK=TRUE;}
                    }
                    break;

                default:
                    bEQ_LOCK=FALSE;
                    break;
            }
          //if((AuxRxBuf[0]==0x77)&&(AuxRxBuf[1]==0x77)&&((AuxRxBuf[2]&0x01)==0x01))
            if(bEQ_LOCK)
            {
                eDPTX_printData("Link Rate:[%x] Link Training PASS!", LinkRate);
                mhal_eDPTx_PatternSkew(edptx_id, 0);
                break;
            }
       }
       //#####################################################################
       // Check Loop>5 => End Training
       //#####################################################################
       if((bEQ_LOCK == FALSE)&&(TrainingLoop == 4))
       {
            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000102,0x00);// DPCD 102  Training Pattern  [1:0]
            mhal_eDPTx_TrainingPattern_Select(0,edptx_id);
            eDPTX_printData("Link Rate:[%x] Link Training FAIL!", LinkRate);
            eDPTX_printMsg("End Training");

            mdrv_eDPTx_MLSignalEn(edptx_id,FALSE);
            return FALSE;
        }
    }

    // End Training

    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x000102,0x00);// DPCD 102  Training Pattern  [1:0]  Scramble disable [5]
    mhal_eDPTx_TrainingPattern_Select(0,edptx_id);
    eDPTX_printData("Lane Count:[%x]", LaneCount);
    eDPTX_printMsg("End Training");

    // ########################
    mdrv_eDPTx_VDClkRst();
    // ########################
    if( g_sPnlInfo.ePnlOutFmt == EN_PNL_OUT_FMT_4B )
        mhal_eDPTx_FMTOutputEnable();

    #if 0
    //mhal_eDPTx_TU_Overwrite(6000);
    #endif

    if(bEQ_LOCK)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

void mdrv_eDPTx_MODSetSwPre(BYTE LinkRate, BYTE LaneNum, BYTE SwLv, BYTE PreLv, BYTE ucInputPort)
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);
    BYTE LaneNumFix;

    LaneNumFix=(LaneNum>3)?4:LaneNum; // LaneNum>3 => means all lane config
    if(LinkRate > eDPTXHBR)
    {
        eDPTX_printData("LinkRate Error = %x",LinkRate);
    }  // dummy message for ignore compiler warning message
    //if((eDPTXHBR2SetSwLv[LaneNumFix]==1)&&(SwLv==2)&&(eDPTXHBR2SetPreLv[LaneNumFix]==0)&&(PreLv==0))

    if( ( ((eDPTXMODSetSwLv[LaneNumFix]==1)&&(SwLv==2))||((eDPTXMODSetSwLv[LaneNumFix]==2)&&(SwLv==1)) )&&((eDPTXMODSetPreLv[LaneNumFix]==0)&&(PreLv==0)))
    {
        #if 0
            if((eDPTXHBR2SetSwLv[LaneNumFix]==1)&&(SwLv==2))
            eDPTX_printData("PHY MID-Trans sw1->sw2 [%x]",((eDPTx_GC_ICTRL_TAP0[1][0])|(eDPTx_GC_ICTRL_TAP0[2][0])));
            if((eDPTXHBR2SetSwLv[LaneNumFix]==2)&&(SwLv==1))
            eDPTX_printData("PHY MID-Trans sw2->sw1 [%x]",((eDPTx_GC_ICTRL_TAP0[1][0])|(eDPTx_GC_ICTRL_TAP0[2][0])));
        #endif

        mhal_eDPTx_SwingTune(LaneNumFix,((eDPTxMOD_GC_SWING[1][0])|(eDPTxMOD_GC_SWING[2][0])),edptx_id);
    }
    mhal_eDPTx_SwingTune(LaneNumFix,eDPTxMOD_GC_SWING[SwLv&0x03][PreLv&0x03],edptx_id);
    mhal_eDPTx_PreEmphasisTune(LaneNumFix,eDPTxMOD_GC_PREEM[SwLv&0x03][PreLv&0x03], edptx_id);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_HPDIRQRst(void)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_HPDIRQRst(BYTE ucInputPort)
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    mhal_eDPTx_HPDIRQRst(edptx_id);
    return;
}
#if eDPTXAutoTest
//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_ATSTReadSwPreReq(BYTE ucInputPort,BYTE LinkRate,BYTE LaneCount)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_ATSTReadSwPreReq(BYTE ucInputPort,BYTE LinkRate,BYTE LaneCount) //no need to change
{
    BYTE AuxRxBuf[2];
    BYTE AuxTxBuf[4];
    BYTE VswingLevel[4]={0,0,0,0};
    BYTE VPremphasisLevel[4]={0,0,0,0};
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);
      //>>>>>> Read Swing Request <<<<<<
    if(mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x000206,2,AuxRxBuf))
    {
        eDPTX_printData("LinkRate [%x]",LinkRate);
        eDPTX_printMsg("Read Sw/Pre Req");
        eDPTX_printData("[%x]DPCD 206",AuxRxBuf[0]); // L0 : sw=[1:0] pre=[3:2]
        eDPTX_printData("[%x]DPCD 207",AuxRxBuf[1]);
        VswingLevel[0]=AuxRxBuf[0]&0x03;
        VswingLevel[1]=(AuxRxBuf[0]>>4)&0x03;
        VswingLevel[2]=AuxRxBuf[1]&0x03;
        VswingLevel[3]=(AuxRxBuf[1]>>4)&0x03;
        VPremphasisLevel[0]=(AuxRxBuf[0]>>2)&0x03;
        VPremphasisLevel[1]=(AuxRxBuf[0]>>6)&0x03;
        VPremphasisLevel[2]=(AuxRxBuf[1]>>2)&0x03;
        VPremphasisLevel[3]=(AuxRxBuf[1]>>6)&0x03;
    }
    mdrv_eDPTx_MODSetSwPre(LinkRate,4,VswingLevel[0],VPremphasisLevel[0],edptx_id);
    //mdrv_eDPTx_MODSetSwPre(LinkRate,0,VswingLevel[0],VPremphasisLevel[0],edptx_id);
    //mdrv_eDPTx_MODSetSwPre(LinkRate,1,VswingLevel[1],VPremphasisLevel[1],edptx_id);
    //mdrv_eDPTx_MODSetSwPre(LinkRate,2,VswingLevel[2],VPremphasisLevel[2],edptx_id);
    //mdrv_eDPTx_MODSetSwPre(LinkRate,3,VswingLevel[3],VPremphasisLevel[3],edptx_id);
    //mhal_eDPTx_SwingTune(4,g_sPnlInfo.u8PnlEdpSwDftValue,edptx_id);
    //mhal_eDPTx_PreEmphasisTune(4,g_sPnlInfo.u8PnlEdpPreemphasisDftValue,edptx_id);
    AuxTxBuf[0]=((VPremphasisLevel[0]<<3) |VswingLevel[0]);  // DPCD 103 L0 = SW[2:0]  / Pre [5:3]

    if(LaneCount>1)
    {
        AuxTxBuf[1] = ((VPremphasisLevel[1]<<3) |VswingLevel[1]);
    } // DPCD 104 L1 = SW[2:0]  / Pre [5:3]

    if(LaneCount > 2)
    {
        AuxTxBuf[2] = ((VPremphasisLevel[2]<<3) |VswingLevel[2]);  // DPCD 105 L2 = SW[2:0]  / Pre [5:3]
        AuxTxBuf[3] = ((VPremphasisLevel[3]<<3) |VswingLevel[3]);       // DPCD 106 L3 = SW[2:0]  / Pre [5:3]
    }

    mdrv_eDPTx_AuxDPCDWriteBytes(edptx_id,0x000103,LaneCount&0x07,AuxTxBuf);
    MAsm_CPU_DelayMs(1);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_ATSTReadSwPreReq(BYTE ucInputPort,BYTE LinkRate,BYTE LaneCount)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_AutoTest(BYTE ucInputPort) //Poll from Raptor
{

    Bool bAutoTestIRQ=FALSE;
    BYTE DPCD_201;
    BYTE DPCD_218;
#if PHY_TEST_PATTERN
        BYTE DPCD_248;
    #if TEST_SYMBERR
            WORD SYMERRCNT_N;          // for sym Error Count
    #endif

    #if TEST_PHY80B
            BYTE DPCDRDBUF[10];          // for  1.sym Error Count (DPCD0210-0217)  / 2.PHY 80b(DPCD_0250-0259)
    #endif
#endif
#if TEST_LINK_TRAINING
        BYTE TEST_LINK_RATE;       // DPCD_219
#endif
    BYTE TEST_LANE_COUNT;  // DPCD_220
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);
    mhal_eDPTx_PatternSkew(edptx_id, 1);
    //Step 1:SINK do HPD Short Pulse (200us ~1.5ms) => trigger DPTX to Read [Link Status] DPCD 0x00200~0x00205
    //Step 2:Read DPCD 0x00201[1] (DEVICE_SERVICE_IRQ_VECTOR)
    //       =>AUTOMATED_TEST_REQUES == 1 ? Yes=>do AUTO TEST(Read 0x218~0x27F)
    mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00201,&DPCD_201);
    mhal_eDPTx_ICP_Setting(edptx_id, FALSE);
    if(DPCD_201&BIT1)
    {
          bAutoTestIRQ=TRUE;
          eDPTX_printMsg("[eDPTX] Check DPCD201 AUTO TEST IRQ=1");
          //Step 3:Write DPCD 0x00201[1]=1 (DEVICE_SERVICE_IRQ_VECTOR) =>Write ¡¥1¡¦ Clear IRQ.
          mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00201,0x02);
          //Step 4: Read DPCD 0x00218 TEST_REQUEST [RO] (Test requested by the Sink Device)??
          //       [0]TEST_LINK_TRAINING
          //       [1]TEST_PATTERN
          //       [2]0 TEST_EDID_READ
          //       [3]0 PHY_TEST_PATTERN = ? See next page for Step 4-1  check DPCD0248
          MAsm_CPU_DelayMs(1);
          mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00218,&DPCD_218);
          switch(DPCD_218&0x0F)
          {
                case 0x01:
                eDPTX_printMsg("[eDPTX] TEST_LINK_TRAINING");
            #if TEST_LINK_TRAINING
                  #if 1//eDPTXHBR2PortEn
                            mhal_eDPTx_PRBS7Enable(edptx_id, 0);
                            mhal_eDPTx_EyePatternEnable(edptx_id, 0);
                            mhal_eDPTx_ProgramPatternEnable(edptx_id, 0);
                  #endif
                      //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                      mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                      eDPTX_printMsg("[eDPTX] Start Do Auto Test");
                      MAsm_CPU_DelayMs(1);
                      mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00219,&TEST_LINK_RATE);
                      eDPTX_printData("[eDPTX] TEST_LINK_RATE %x",TEST_LINK_RATE);
                      mdrv_eDPTx_ConfigLinkRate(TEST_LINK_RATE, edptx_id);

                        if( g_sPnlInfo.bPnlEdpEnSSC )
                            mdrv_eDPTx_SSCEnable(TRUE,TEST_LINK_RATE,edptx_id);

                      mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00220,&TEST_LANE_COUNT);
                      eDPTX_printData("[eDPTX] TEST_LANE_COUNT %x",TEST_LANE_COUNT);
                      // ####################  Call Training Function  ########################
                      mdrv_eDPTx_Training(edptx_id, TEST_LINK_RATE, TEST_LANE_COUNT);
                      // #################################################################
            #else
                      eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
            #endif
                break;

                case 0x02:
                    eDPTX_printMsg("[eDPTX] TEST_PATTERN");
                #if TEST_PATTERN
                        //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                        eDPTX_printMsg("[DPTX] Start Do Auto Test");
                        // #################  Call TEST PATTERN FLOW  ######################

                        // #################################################################
                #else
                        //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                        eDPTX_printMsg("[DPTX] NOT SUPPORT NOW!!");
                #endif
                    break;

                case 0x04:
                    eDPTX_printMsg("[eDPTX] TEST_EDID_READ");
                #if TEST_EDID_READ
                        //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                        eDPTX_printMsg("[eDPTX] Start Do Auto Test");
                        // ####################  Call READ EDID Function ######################

                        // #################################################################
                #else
                        //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                        mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                        eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
                #endif
                    break;

                case 0x08:
                    eDPTX_printMsg("[eDPTX] PHY_TEST_PATTERN");
                    mhal_eDPTx_PRBS7Enable(edptx_id, 0);
                    mhal_eDPTx_EyePatternEnable(edptx_id, 0);
                    mhal_eDPTx_ProgramPatternEnable(edptx_id, 0);
                #if PHY_TEST_PATTERN
                        mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00248,&DPCD_248);
                        eDPTX_printMsg("[eDPTX] DPCD248 0//TBD PHY_TEST_PATTERN_SEL");
                        // ####################### Read DPCD to Set TESTLANECOUNT&SW&PRE ###################
                        mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00219,&TEST_LINK_RATE);MAsm_CPU_DelayMs(1);
                        mdrv_eDPTx_AuxDPCDReadByte(edptx_id,0x00220,&TEST_LANE_COUNT);MAsm_CPU_DelayMs(1);
                        mdrv_eDPTx_ATSTReadSwPreReq(edptx_id,TEST_LINK_RATE,TEST_LANE_COUNT);
                        // #####################################################################################
                   switch(DPCD_248&0x07)
                   {
                         case 0x00:
                            eDPTX_printMsg("[eDPTX] No Test Pattern selected");
                            //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                            mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                            break;

                         case 0x01:
                            eDPTX_printMsg("[eDPTX] D10.2 without scrambling");
                        #if TEST_D102
                            // ####################### Set TX D10.2 transmitter ########################################
                                    mhal_eDPTx_TrainingPattern_Select(1,edptx_id);
                                // #####################################################################################
                                //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                eDPTX_printMsg("[DPTX] Start Do Auto Test");
                                break;
                        #else
                                //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
                        #endif

                         case 0x02:
                                eDPTX_printMsg("[eDPTX] Symbol Err Measure Count");
                            #if TEST_SYMBERR
                                    // ####################### Config TX to transmitt SYMBERR ###################################
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] Start Do Auto Test");
                                    // ####################### Trigger TX transmitt Error Cnt #N ####################################
                                    SYMERRCNT_N=0x0005;
                                    eDPTX_printData("[eDPTX]: Set L0 error = %x",SYMERRCNT_N);
                                    // ####################### Read & PrintOut Error Cnt Value => verify [Value== #N] #################
                                    MAsm_CPU_DelayMs(1000); // Delay 1s
                                    mdrv_eDPTx_ATSTPrintErrCNT(edptx_id);
                                    // #######################################################################################
                            #else
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
                            #endif
                                break;

                         case 0x03:
                                eDPTX_printMsg("[eDPTX] PRBS7");
                            #if TEST_PRBS7
                                    // ############################ Config TX to transmitt PRBS7 ##################################
                                    mhal_eDPTx_PRBS7Enable(edptx_id, 1);
                                    // ########################################################################################
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] Start Do Auto Test");
                                    // ####################### Read & PrintOut Error Cnt Value #####################################
                                    MAsm_CPU_DelayMs(1000); // Delay 1s
                                    mdrv_eDPTx_ATSTPrintErrCNT(edptx_id);
                                    // #######################################################################################
                            #else
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
                            #endif
                                break;

                         case 0x04:
                                eDPTX_printMsg("[eDPTX] 80 bit custom pattern transmitted");
                            #if TEST_PHY80B
                                    mhal_eDPTx_ICP_Setting(edptx_id, TRUE);
                                    // ############################ Read 80b Pattern from DPCD 0250~0259 #########################
                                    mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x00250,10,DPCDRDBUF);
                                    // ############################ Config TX to transmitt 80bit ####################################
                                    mhal_eDPTx_ProgramPatternEnable(edptx_id, 1);
                                    // ########################################################################################
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] Start Do Auto Test");
                                    // ####################### Read & PrintOut Error Cnt Value #####################################
                                    MAsm_CPU_DelayMs(1000); // Delay 1s
                                    mdrv_eDPTx_ATSTPrintErrCNT(edptx_id);
                                    // #######################################################################################
                            #else
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
                            #endif
                                break;

                         case 0x05:
                                eDPTX_printMsg("[eDPTX] HBR2 Compliance EYE pattern");
                            #if TEST_HBR2EYE    //CP2520
                                    // ############################ Read HBR2 Configure from DPCD ##############################
                                    // ############################ Set TX to transmitt HBR2 EYE Pattern ##########################
                                     mhal_eDPTx_EyePatternEnable(edptx_id, 1);
                                    // ########################################################################################
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] Start Do Auto Test");
                                    // ####################### Read & PrintOut Error Cnt Value  ####################################
                                    mdrv_eDPTx_ATSTPrintErrCNT(edptx_id);
                                    // #######################################################################################
                            #else
                                    //Step 5:Write DPCD 0x00260[0]=1 (TEST_ACK) => means [DP TX] tell [DP RX] Start do AUTO TEST.
                                    mdrv_eDPTx_AuxDPCDWriteByte(edptx_id,0x00260,0x01);
                                    eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
                            #endif
                                break;

                         default:
                            eDPTX_printMsg("[eDPTX] RESERVED");
                            break;
                }
            #else
                    eDPTX_printMsg("[eDPTX] NOT SUPPORT NOW!!");
            #endif
                    break;

                default:eDPTX_printMsg("[eDPTX] DPCD218 No Request");
                    break;
          }

        mhal_eDPTx_MLSignalEnable(edptx_id);
    }
    else
    {
          bAutoTestIRQ=FALSE;
          //bAutoTestIRQ=TRUE;
      #if 1//eDPTXHBR2PortEn
                //mhal_eDPTx_PRBS7Enable(edptx_id, 0);
                //mhal_eDPTx_EyePatternEnable(edptx_id, 0);
                //mhal_eDPTx_ProgramPatternEnable(edptx_id, 0);
      #endif
          eDPTX_printMsg("[eDPTX] DPCD_201 AUTO TEST IRQ=0");
          eDPTX_printMsg("[eDPTX] END AUTO TEST FLOW!!");
    }
    return  bAutoTestIRQ;
}


#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_HPDP0Handle01(BYTE ucInputPort)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_HPDHandle(BYTE ucInputPort) // no need to change
{
    WORD HPDStatus=0;
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    HPDStatus = mhal_eDPTx_HPDStatus(edptx_id);

    if(HPDStatus&(eDPTXHPD_IRQEvent<<(edptx_id*3))) // P0 HPD Event
    {
        eDPTX_printData("HPD P%d Event",edptx_id);
        if(mhal_eDPTx_HPDValues(edptx_id) >0) // [3:0] HPD0~3 in value
        {
        #if eDPTXAutoTest
                if(mdrv_eDPTx_AutoTest(edptx_id))
                {
                    //mhal_eDPTx_HPDClearIRQBitAll(edptx_id);
                }else
                {
        #endif
                    if(mdrv_eDPTx_ReadDPCDLinkStatus(edptx_id))
                    {
                        eDPTX_printData("P%d Link Status OK!",edptx_id);
                        //mhal_eDPTx_HPDClearIRQBitAll(edptx_id);
                    }else
                    {
                        eDPTX_printData("P%d ReTraining!",edptx_id);
                        //mhal_eDPTx_HPDClearIRQBitAll(edptx_id);
                        mdrv_eDPTx_Training(edptx_id,g_sPnlInfo.u8PnlEdpLinkRate,g_sPnlInfo.u8PnlEdpLaneCnt);
                    }
        #if eDPTXAutoTest
                }
        #endif
        }
        else
        {
            eDPTX_printData("HPD Event Fail: P%d is Low !",edptx_id);
        }
            //mhal_eDPTx_HPDClearIRQEvent(edptx_id);
    }
    else if(HPDStatus&(eDPTXHPD_IRQConnect<<(edptx_id*3))) // P0 HPD Connect
    {
    //eDPTX_printData("edptx_id = %d",edptx_id);
    //eDPTX_printData("mhal_eDPTx_HPDValues(edptx_id) = %x",mhal_eDPTx_HPDValues(edptx_id));
        if(mhal_eDPTx_HPDValues(edptx_id)>0) // [3:0] HPD0~3 in value
        {
            //mhal_eDPTx_HPDClearIRQBitAll(edptx_id);
            eDPTX_printData("HPD P%d Connect",edptx_id);
    #if eDPTXAutoTest
                if(mdrv_eDPTx_AutoTest(edptx_id))
                {}
                else
                {
    #endif
                    mdrv_eDPTx_Training(edptx_id,g_sPnlInfo.u8PnlEdpLinkRate,g_sPnlInfo.u8PnlEdpLaneCnt);
    #if eDPTXAutoTest
                }
    #endif
        }
        else
        {
            eDPTX_printData("HPD Connect Fail : Port%d is Low !",edptx_id);
            //mhal_eDPTx_HPDClearIRQConnect(edptx_id);
        }
    }
    if(HPDStatus&(eDPTXHPD_IRQDisConnect<<(edptx_id*3))) // P0 HPD DisConnect[1]
    {
        eDPTX_printData("HPD P%d Disconnect",edptx_id);
        //mhal_eDPTx_HPDClearIRQDisconnect(edptx_id);
    }

    return;
}



//**************************************************************************
//  [Function Name]:
//                  Bool mdrv_eDPTx_CheckHPD(BYTE ucInputPort)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_CheckHPD(BYTE ucInputPort) //no need change
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    return mhal_eDPTx_CheckHPD(edptx_id);
}

//**************************************************************************
//  [Function Name]:
//                  Bool mdrv_eDPTx_CheckHPDIRQ(BYTE HPDPortNum)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_eDPTx_CheckHPDIRQ(BYTE ucInputPort)
{

    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    return mhal_eDPTx_CheckHPDIRQ(edptx_id);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_PGSyncRst(void)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void  mdrv_eDPTx_PGSyncRst(void)
{
    mhal_eDPTx_PGSyncRst();
    return;
}

//**************************************************************************
//  [Function Name]:
//                 mdrv_eDPTx_VDClkRst(void)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_VDClkRst(void)
{
    //mhal_eDPTx_VDClkRst();
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_HPDIRQRst(void)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_HPDIRQ_Handler(BYTE ucInputPort)
{
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    WORD HPDStatus = 0;
    //BYTE eDPTXHPD_IRQEvent = 0;
    //BYTE eDPTXHPD_IRQConnect = 0;
    //BYTE eDPTXHPD_IRQDisConnect = 0;
    edptx_id = edptx_id;
    HPDStatus = mhal_eDPTx_HPDStatus(edptx_id);

    if(g_sPnlInfo.u8PnlEdpMODUsePort == 2)
        {
            if((msReadByte(REG_EDP_TX0_P0_53_L)&(BIT0|BIT1|BIT2)))
            {
                if(msReadByte(REG_EDP_TX0_P0_53_L)&(BIT2))
                {
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, BIT2, BIT2);
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, 0, BIT2);
                }else if(msReadByte(REG_EDP_TX0_P0_53_L)&(BIT0))
                {
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, BIT0, BIT0);
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, 0, BIT0);

                }else
                {
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, BIT1, BIT1);
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, 0, BIT1);

                }
            }

            if((msReadByte(REG_EDP_TX0_P0_53_L)&(BIT3|BIT4|BIT5)))
            {
                if(msReadByte(REG_EDP_TX0_P0_53_L)&(BIT5))
                {
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, BIT5, BIT5);
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, 0, BIT5);

                }else if(msReadByte(REG_EDP_TX0_P0_53_L)&(BIT3))
                {
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, BIT3, BIT3);
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, 0, BIT3);

                }else
                {
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, BIT4, BIT4);
                    msWriteByteMask(REG_EDP_TX0_P0_56_L, 0, BIT4);

                }
            }

        }




    msWriteByteMask(REG_EDP_TX0_P0_56_L, eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent,eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent);  // clear IRQ
    msWriteByteMask(REG_EDP_TX0_P0_56_L, 0, eDPTXHPDP0_IRQConnect|eDPTXHPDP0_IRQDisConnect|eDPTXHPDP0_IRQEvent);



    //mhal_eDPTx_HPDIRQ_Handler(edptx_id);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_ATSTPrintErrCNT(BYTE PortNum)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_ATSTPrintErrCNT(BYTE ucInputPort) //no need to change
{
    BYTE DPCDRDERRCNTBUF[8]={0,0,0,0,0,0,0,0};
    eDPTx_ID edptx_id = _mdrv_eDPTx_PortNumber2eDPTxID(ucInputPort);

    // ####################### Read & PrintOut Error Cnt Value => verify [Value== #N] #################
    // L0: DPCD 211+210 [15]=Valid ,[14:0]=ErrValue
    // L1: DPCD 213+212 [15]=Valid ,[14:0]=ErrValue
    // L2: DPCD 215+214 [15]=Valid ,[14:0]=ErrValue
    // L3: DPCD 217+216 [15]=Valid ,[14:0]=ErrValue
    mdrv_eDPTx_AuxDPCDReadBytes(edptx_id,0x00210,8,DPCDRDERRCNTBUF);
    eDPTX_printData("[eDPTX]: L0 error %x",(DPCDRDERRCNTBUF[1]<<8)|DPCDRDERRCNTBUF[0]);
    eDPTX_printData("[eDPTX]: L1 error %x",(DPCDRDERRCNTBUF[3]<<8)|DPCDRDERRCNTBUF[2]);
    eDPTX_printData("[eDPTX]: L2 error %x",(DPCDRDERRCNTBUF[5]<<8)|DPCDRDERRCNTBUF[4]);
    eDPTX_printData("[eDPTX]: L3 error %x",(DPCDRDERRCNTBUF[7]<<8)|DPCDRDERRCNTBUF[6]);
    // #######################################################################################
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_eDPTx_Handle(void)
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_eDPTx_Handle(void)
{
    #if (eDPTXAutoTest == 1)
        MAsm_CPU_DelayMs(200);
        eDPTX_printMsg("mdrv_eDPTx_Handle");
    #endif

    if( g_sPnlInfo.bPnlEdpDETECTHPD )
    {
        if( g_sPnlInfo.u8PnlEdpTotalPorts == 0x02 )
        {
            mdrv_eDPTx_HPDHandle(0);
            mdrv_eDPTx_HPDHandle(1);
        }
        else
        {
            mdrv_eDPTx_HPDHandle(0);
        }
    }

    return;
}
//**************************************************************************
//  [Function Name] :  mdrv_eDPTx_PowerDown(void)
//
//  [Description] : Power down for PHY
//
//  [Arguments] : None
//
//  [Return] : None
//
//**************************************************************************
void mdrv_eDPTx_PowerDown(BOOL bPowerDown) //poll from Raptor
{
    mhal_eDPTx_PowerDown(bPowerDown);
    return;
}

#endif

#endif //#ifndef _mdrv_EDPTX_C_

