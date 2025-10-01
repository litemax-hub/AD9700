//******************************************************************************
//  Copyright (c) 2003-2008 MStar Semiconductor, Inc.
//  All rights reserved.
//
//  [Module Name]:
//      system_eDPTx.c
//  [Abstract]:
//      This module contains code for eDPTx system level
//      procedure and subroutin
//  [Author(s)]:
//      PJ Lee
//  [Reversion History]:
//      Initial release:    03 June, 2025
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
#define eDPTX_printData(str, value)       printData(str, value)
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
    UNUSED(ucInputPort);

#if (eDPTXDETECTHPD != 0x0)
    for(HPD_COUNT=0;HPD_COUNT<20;HPD_COUNT++)
    {
        if(mapi_eDPTx_CheckHPD(ucInputPort))
        {
            MAsm_CPU_DelayMs(2); // clear HPDIRQ after HPD IRQ delay 1.5ms
#endif

            mapi_eDPTx_HPDIRQRst(ucInputPort); // clear HPDIRQ after HPD IRQ delay 1.5ms
            mapi_eDPTx_Training(ucInputPort,g_sPnlInfo.u8PnlEdpLinkRate,g_sPnlInfo.u8PnlEdpLaneCnt);
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
    mapi_eDPTx_Init(0);
    mapi_eDPTx_SetMSA(0);
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
    mapi_eDPTx_Training(0, g_sPnlInfo.u8PnlEdpLinkRate, g_sPnlInfo.u8PnlEdpLaneCnt);
    mhal_eDPTx_HPDClearIRQConnect(0);
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
    _System_eDPTx_CheckHPD_and_LT(0);
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
    mapi_eDPTx_PowerDown(TRUE);
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
    mapi_eDPTx_MLSignalEn(0, FALSE);
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
    mapi_eDPTx_FineTuneTU_Disable(0);
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
    mapi_eDPTx_FineTuneTU(0,eDPTX_OUTBL_PixRateKhz);
}



