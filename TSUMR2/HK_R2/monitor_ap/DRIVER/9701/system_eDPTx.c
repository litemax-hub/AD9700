//******************************************************************************
//  Copyright (c) 2003-2008 MStar Semiconductor, Inc.
//  All rights reserved.
//
//  [Module Name]:
//      system_eDPTx.c
//  [Abstract]:
//      This module contains code for DisplayPort receiver's application level
//      procedure and subroutin
//  [Author(s)]:
//      Vincent Kuo
//  [Reversion History]:
//      Initial release:    06 May, 2008
//*******************************************************************************

#define _SYSTEM_EDPTX_C_
#include "board.h"
#include "datatype.h"
#include "misc.h"
#include "Ms_rwreg.h"
#include "system_eDPTx.h"
#include "mapi_eDPTx.h"
#include "mhal_eDPTx.h"
#include "asmCPU.h"

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
BYTE HPD_COUNT = 0;

//==============================================================================
//
//                          Local Define
//
//==============================================================================
#define eDPTX_SYSTEM_DEBUG 1
#if (eDPTX_SYSTEM_DEBUG&&ENABLE_MSTV_UART_DEBUG)
#define eDPTX_printData(str, value)   printData(str, value)
#define eDPTX_printMsg(str)               printMsg(str)
#else
#define eDPTX_printData(str, value)
#define eDPTX_printMsg(str)
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//==============================================================================
//
//                              Local FUNCTIONS
//
//==============================================================================


//**************************************************************************
//  [Function Name]:
//                  _System_eDPTx_CheckHPD_and_LT()
//  [Description]
//                  _System_eDPTx_CheckHPD_and_LT
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _System_eDPTx_CheckHPD_and_LT(BYTE ucInputPort)
{
#if (eDPTXDETECTHPD != 0x0)
    for(HPD_COUNT=0;HPD_COUNT<20;HPD_COUNT++)
    {
        if(mapi_eDPTx_CheckHPD(ucInputPort))
        {
            MAsm_CPU_DelayMs(2); // clear HPDIRQ after HPD IRQ delay 1.5ms
#endif

            mapi_eDPTx_HPDIRQRst(ucInputPort); // clear HPDIRQ after HPD IRQ delay 1.5ms
            mapi_eDPTx_Training(ucInputPort,g_eDPTxInfo.u8LinkRate,g_eDPTxInfo.u8LaneCount);
#if (eDPTXDETECTHPD != 0x0)
            break;
        }
        else
        {
            MAsm_CPU_DelayMs(10);
            eDPTX_printData("check HPD Low [%d]\r\n", ucInputPort);
        }
    }
#endif

    ucInputPort = ucInputPort;
}




//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//**************************************************************************
//  [Function Name]:
//                  System_eDPTx_Init_main()
//  [Description]
//                  System_eDPTx_Init_main
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void System_eDPTx_Init_main(void)
{
    BYTE u8PortNum;

    for(u8PortNum=0;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
        {
            mapi_eDPTx_Init(u8PortNum);
            mapi_eDPTx_SetMSA(u8PortNum);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  System_eDPTx_Training()
//  [Description]
//                  System_eDPTx_Training
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void System_eDPTx_Training(void)
{
    BYTE u8PortNum;

    for(u8PortNum=0;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
        {
            mapi_eDPTx_Training(u8PortNum, g_sPnlInfo.u8PnlEdpLinkRate, g_sPnlInfo.u8PnlEdpLaneCnt);
            mhal_eDPTx_HPDClearIRQConnect(u8PortNum);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  System_eDPTx_PowerOnCheck()
//  [Description]
//                  System_eDPTx_PowerOnCheck
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void System_eDPTx_PowerOnCheck(void)
{
    BYTE u8PortNum;

    for(u8PortNum=0;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
        {
            _System_eDPTx_CheckHPD_and_LT(u8PortNum);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  System_eDPTx_PowerOff()
//  [Description]
//                  System_eDPTx_PowerOff
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void System_eDPTx_PowerDown(void)
{
    BYTE u8PortNum;

    for(u8PortNum=0;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
        {
            mapi_eDPTx_PowerDown(u8PortNum);
        }
    }
}
//**************************************************************************
//  [Function Name]:
//                  System_eDPTx_SignalOff()
//  [Description]
//                  System_eDPTx_SignalOff
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void System_eDPTx_SignalOff(void)
{
    BYTE u8PortNum;

    for(u8PortNum=0;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
        {
            mapi_eDPTx_MLSignalEn(u8PortNum, FALSE);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  System_eDPTx_Handler()
//  [Description]
//                  System_eDPTx_Handler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void System_eDPTx_Handler(void)
{
    mapi_eDPTx_Handle();
}

//******************************************************************************
//
//  [Function Name]:
//      System_eDPTx_FineTuneTU(BYTE PortNum, DWORD eDPTX_OUTBL_PixRateKhz)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void System_eDPTx_FineTuneTU_Disable(void)
{
    BYTE u8PortNum;

    for(u8PortNum=0;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
        {
            mapi_eDPTx_FineTuneTU_Disable(u8PortNum);
        }
    }
}

//******************************************************************************
//
//  [Function Name]:
//      System_eDPTx_FineTuneTU(BYTE PortNum, DWORD eDPTX_OUTBL_PixRateKhz)
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//      None
//
//*******************************************************************************
void System_eDPTx_FineTuneTU(DWORD eDPTX_OUTBL_PixRateKhz)
{
    BYTE u8PortNum;

    for(u8PortNum=0;u8PortNum<2;u8PortNum++)
    {
        if(g_eDPTxInfo.u8PortSelect&(BIT0<<u8PortNum))
        {
            mapi_eDPTx_FineTuneTU(u8PortNum,eDPTX_OUTBL_PixRateKhz);
        }
    }
}





