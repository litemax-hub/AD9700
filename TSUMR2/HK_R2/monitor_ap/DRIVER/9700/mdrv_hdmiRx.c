////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
///////////////////////////////////////////////////////////////////////////////

#ifndef MDRV_HDMIRX_C
#define MDRV_HDMIRX_C

//#include "datatype.h"
//#include "IpCommon.h"
//#include "mapi_combo.h"

#include "board.h"
#include "Mode.h"
#include "Common.h"
#include "mdrv_hdmiRx.h"
#include "misc.h"
#include "HdcpHandler.h"
#include "Ms_rwreg.h"

#if (ENABLE_HDMI || ENABLE_DVI)
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define HDMI_DEBUG_MESSAGE_DRV      1
#if(HDMI_DEBUG_MESSAGE_DRV  && ENABLE_DEBUG)
#define HDMI_DRV_DPUTSTR(str)            printMsg(str)
#define HDMI_DRV_DPRINTF(str, x)         printData(str, x)
#else
#define HDMI_DRV_DPUTSTR(str)
#define HDMI_DRV_DPRINTF(str, x)
#endif

#define HDMI_HDCP2_IRQ_MODE_ENABLE                  0
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
ST_HDMI_RX_SYSTEM_PARAMETER stHDMIRxInfo;
static BYTE ucCurrPort = HDMI_INPUT_PORT_NONE;
const BYTE tEDID_TABLE_COMBO_PORT0[256] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0A, 0x14, 0x01, 0x03, 0x80, 0x46, 0x28, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
    0x12, 0x48, 0x4C, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
    0x58, 0x2C, 0x25, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x53, 0x74, 0x61, 0x72, 0x20, 0x44, 0x65, 0x6D, 0x6F, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x3A, 0x3E, 0x0F, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xA1,
    0x02, 0x03, 0x24, 0x71, 0x4F, 0x94, 0x13, 0x05, 0x03, 0x04, 0x02, 0x01, 0x16, 0x15, 0x07, 0x06,
    0x11, 0x10, 0x12, 0x1F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0C, 0x00,
    0x10, 0x00, 0xA8, 0x2D, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00,
    0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E,
    0x96, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,
    0x10, 0x3E, 0x96, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14, 0x51, 0xF0,
    0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24
};

const BYTE tEDID_TABLE_COMBO_PORT1[256] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0A, 0x14, 0x01, 0x03, 0x80, 0x46, 0x28, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
    0x12, 0x48, 0x4C, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
    0x58, 0x2C, 0x25, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x53, 0x74, 0x61, 0x72, 0x20, 0x44, 0x65, 0x6D, 0x6F, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x3A, 0x3E, 0x0F, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xA1,
    0x02, 0x03, 0x24, 0x71, 0x4F, 0x94, 0x13, 0x05, 0x03, 0x04, 0x02, 0x01, 0x16, 0x15, 0x07, 0x06,
    0x11, 0x10, 0x12, 0x1F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0C, 0x00,
    0x10, 0x00, 0xA8, 0x2D, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00,
    0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E,
    0x96, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,
    0x10, 0x3E, 0x96, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14, 0x51, 0xF0,
    0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24
};

const BYTE tEDID_TABLE_COMBO_PORT2[256] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0A, 0x14, 0x01, 0x03, 0x80, 0x46, 0x28, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
    0x12, 0x48, 0x4C, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
    0x58, 0x2C, 0x25, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x53, 0x74, 0x61, 0x72, 0x20, 0x44, 0x65, 0x6D, 0x6F, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x3A, 0x3E, 0x0F, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xA1,
    0x02, 0x03, 0x24, 0x71, 0x4F, 0x94, 0x13, 0x05, 0x03, 0x04, 0x02, 0x01, 0x16, 0x15, 0x07, 0x06,
    0x11, 0x10, 0x12, 0x1F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0C, 0x00,
    0x10, 0x00, 0xA8, 0x2D, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00,
    0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E,
    0x96, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,
    0x10, 0x3E, 0x96, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14, 0x51, 0xF0,
    0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24
};

const BYTE tEDID_TABLE_COMBO_PORT3[256] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0A, 0x14, 0x01, 0x03, 0x80, 0x46, 0x28, 0x78, 0x0A, 0x0D, 0xC9, 0xA0, 0x57, 0x47, 0x98, 0x27,
    0x12, 0x48, 0x4C, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
    0x58, 0x2C, 0x25, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x53, 0x74, 0x61, 0x72, 0x20, 0x44, 0x65, 0x6D, 0x6F, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x3A, 0x3E, 0x0F, 0x46, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xA1,
    0x02, 0x03, 0x24, 0x71, 0x4F, 0x94, 0x13, 0x05, 0x03, 0x04, 0x02, 0x01, 0x16, 0x15, 0x07, 0x06,
    0x11, 0x10, 0x12, 0x1F, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0C, 0x00,
    0x10, 0x00, 0xA8, 0x2D, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00,
    0xDF, 0xA4, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E,
    0x96, 0x00, 0xDF, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10,
    0x10, 0x3E, 0x96, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xA0, 0x14, 0x51, 0xF0,
    0x16, 0x00, 0x26, 0x7C, 0x43, 0x00, 0x30, 0xA4, 0x21, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24
};

const BYTE tHDMI_EDID_SIZE_PORT[4] =
{
    COMBO_EDID_SIZE_PORT0,
    COMBO_EDID_SIZE_PORT1,
    COMBO_EDID_SIZE_PORT2,
    COMBO_EDID_SIZE_PORT3,
};

const BYTE *tCOMBO_EDID_TALBE_PORT[4] =
{
    tEDID_TABLE_COMBO_PORT0,
    tEDID_TABLE_COMBO_PORT1,
    tEDID_TABLE_COMBO_PORT2,
    tEDID_TABLE_COMBO_PORT3,
};

#endif

#if MS_PM
extern XDATA sPM_Info  sPMInfo;
#endif

#if(TMDS_HDCP2_FUNCTION_SUPPORT)
#define HDCP22_DEBUG_MESSAGE_DRV    1
#if(HDCP22_DEBUG_MESSAGE_DRV  && ENABLE_DEBUG)
#define HDCP22_DRV_DPUTSTR(str)            printMsg(str)
#define HDCP22_DRV_DPRINTF(str, x)         printData(str, x)
#else
#define HDCP22_DRV_DPUTSTR(str)
#define HDCP22_DRV_DPRINTF(str, x)
#endif

static Bool bRomCodeDone = FALSE;
static Bool bFirstDoneFlag = FALSE;
static Bool bKeyDecodeDone = FALSE;
BYTE *pHDCPTxData_CertRx;
#endif

TMDS_HDCP2_CALL_BACK_FUNC pmdrv_tmds_HDCP2RxEventProc = NULL;
#if (ENABLE_HDMI || ENABLE_DVI)
//**************************************************************************
//  [Function Name]:
//                  _mdrv_hdmiRx_ProtConfig()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_hdmiRx_PortConfig(BYTE *usInputPortEnableIndex, BYTE *usInputPortTypeDualDVIIndex, BYTE *usInputPortTypeHDMIIndex)
{
    *usInputPortTypeDualDVIIndex = 0;
    *usInputPortTypeHDMIIndex = 0;
     *usInputPortEnableIndex = 0;

#if 0
    for(InputType = HDMI_INPUT_PORT0; InputType<Input_Nums; InputType++)
    {

            if(InputType == Input_DVI || InputType == Input_DVI2 || InputType == Input_DVI3 || InputType == Input_DVI4)
            {
                   *usInputPortTypeHDMIIndex |= (BIT0 <<InputType);
             }
            else if(InputType == Input_HDMI || InputType == Input_HDMI2 ||InputType == Input_HDMI3 ||InputType == Input_HDMI4)
            {
                   *usInputPortTypeHDMIIndex |= (BIT0 <<InputType);
            }
            else if(InputType == Input_DualDVI || InputType == Input_DualDVIC2)
            {
                    *usInputPortTypeDualDVIIndex |= (BIT0 <<InputType);
                    DualPortCnt++;
            }
    }
#endif

    if(Input_HDMI_C1 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT0;
     }

     if(Input_HDMI_C2 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT1;
     }

    if(Input_HDMI_C3 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT2;
     }

    if(Input_HDMI_C4 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT3;
     }

    if(Input_DVI_C1 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT0;
     }

     if(Input_DVI_C2 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT1;
     }

    if(Input_DVI_C3 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT2;
     }

    if(Input_DVI_C4 != Input_Nothing)
    {
           *usInputPortTypeHDMIIndex |= BIT3;
     }


    if(Input_DualDVI_C1 != Input_Nothing)
    {
           *usInputPortTypeDualDVIIndex |= BIT0;
           //*usInputPortTypeDualDVIIndex |= BIT1;
    }

    if(Input_DualDVI_C2 != Input_Nothing)
    {
           *usInputPortTypeDualDVIIndex |= BIT1 ;
           //*usInputPortTypeDualDVIIndex |= BIT0;
    }

    *usInputPortEnableIndex = (*usInputPortTypeDualDVIIndex) | (*usInputPortTypeHDMIIndex);
}


//**************************************************************************
//  [Function Name]:
//                  mdrv_HDMIRx_LoadEDID()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_HDMIRx_LoadEDID(BYTE ucPortSelect, BYTE *EdidData)
{
    BYTE size = 0;

    if(EdidData[0x7E] == 1) //EEODB check
    {
        if((EdidData[0x84] >= 0xE2) && (EdidData[0x85] == 0x78))
        {
            size = EdidData[0x86] +1;
        }
        else
        {
            size = EdidData[0x7E] +1;
        }
    }
    else
        size = EdidData[0x7E] +1;

    if(size > HDMI_EDID_4BLOCK)
    {
        HDMI_DRV_DPRINTF("[ERROR]input EDID size = %d incorrect , change to 4 block \n",size);
        size = HDMI_EDID_4BLOCK;
    }

    Hal_HDMI_WriteInputPortEDID(ucPortSelect, size, EdidData);
}


//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_SetPowerDown()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_SetPowerDown(BYTE enInputPortType, Bool bPowerDown)
{
    Hal_HDMI_SetPowerDown(enInputPortType, bPowerDown);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_Software_Reset()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_Software_Reset(BYTE enInputPortSelect, WORD u16Reset)
{
    Hal_HDMI_Software_Reset(enInputPortSelect, u16Reset);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_InputPortMapping()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
EN_HDMI_INPUT_PORT mdrv_hdmiRx_InputPortMapping(BYTE enInputPortSelect)
{
    EN_HDMI_INPUT_PORT enInputIndex = HDMI_INPUT_PORT_NONE;
#if 0
    if(enInputPortSelect == Input_Nothing)
    {
        enInputIndex = HDMI_INPUT_PORT_END;
    }
    else if((enInputPortSelect == Input_HDMI) || (enInputPortSelect == Input_DVI))
    {
        enInputIndex = HDMI_INPUT_PORT0;
    }
    else if((enInputPortSelect == Input_HDMI2) || (enInputPortSelect == Input_DVI2))
    {
        enInputIndex = HDMI_INPUT_PORT1;
    }
    else if((enInputPortSelect) == Input_HDMI3 || (enInputPortSelect == Input_DVI3))
    {
        enInputIndex = HDMI_INPUT_PORT2;
    }
#else   //  fix coverity
    if(enInputPortSelect == Input_Nothing)
    {
        enInputIndex = HDMI_INPUT_PORT_END;
    }
    else
    {
        if(Input_HDMI!=Input_Nothing)
        {
            if(enInputPortSelect == Input_HDMI)
            {
                enInputIndex = HDMI_INPUT_PORT0;
            }
        }
        if(Input_HDMI2!=Input_Nothing)
        {
            if(enInputPortSelect == Input_HDMI2)
            {
                enInputIndex = HDMI_INPUT_PORT1;
            }
        }
        if(Input_HDMI3!=Input_Nothing)
        {
            if(enInputPortSelect == Input_HDMI3)
            {
                enInputIndex = HDMI_INPUT_PORT2;
            }
        }
        if(Input_DVI!=Input_Nothing)
        {
            if(enInputPortSelect == Input_DVI)
            {
                enInputIndex = HDMI_INPUT_PORT0;
            }
        }
        if(Input_DVI2!=Input_Nothing)
        {
            if(enInputPortSelect == Input_DVI2)
            {
                enInputIndex = HDMI_INPUT_PORT1;
            }
        }
        if(Input_DVI3!=Input_Nothing)
        {
            if(enInputPortSelect == Input_DVI3)
            {
                enInputIndex = HDMI_INPUT_PORT2;
            }
        }
    }
#endif

    return enInputIndex;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_Initial()
//  [Description]
//
//  [Arguments]:
//                  unMode : 0 = AC (with HPD), 1 = DC
//  [Return]:
//
//**************************************************************************
Bool mdrv_hdmiRx_Initial(BYTE ucMode)
{
    EN_HDMI_INPUT_PORT enInputPortSelect = HDMI_INPUT_PORT_NONE;
    Bool bLowFlag = 0;

    _mdrv_hdmiRx_PortConfig(&stHDMIRxInfo.usInputPortEnableIndex, &stHDMIRxInfo.usInputPortTypeDualDVIIndex, &stHDMIRxInfo.usInputPortTypeHDMIIndex);

    stHDMIRxInfo.b10msTimerFlag = FALSE;
    stHDMIRxInfo.uc10msCounter = HDMI_POLLING_INTERVAL;
    stHDMIRxInfo.ucHPDControlIndex = 0;
    stHDMIRxInfo.ucHDCPWriteDoneIndex = 0;
    stHDMIRxInfo.ucHDCPReadDoneIndex = 0;
    stHDMIRxInfo.ucHDCPSendMessageIndex = 0;
    stHDMIRxInfo.pucHDCPCertRx = 0;
    stHDMIRxInfo.pmdrv_tmds_HDCP2ReceiveEventProc = 0;

    for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, BIT(enInputPortSelect)))
        {
            stHDMIRxInfo.stPollingInfo.bNoInputFlag = TRUE;
            stHDMIRxInfo.stPollingInfo.bClockStableFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.bDEStableFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.bPowerOnLane = FALSE;
            //stHDMIRxInfo.stPollingInfo.bHDMI20Flag = FALSE;
            //stHDMIRxInfo.stPollingInfo.bYUV420Flag = FALSE;
            stHDMIRxInfo.stPollingInfo.bFreeSyncModeEnable = FALSE;
            stHDMIRxInfo.stPollingInfo.bPowerSavingFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.bHDMIModeFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.bTimingStableFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.bAutoEQRetrigger = FALSE;
            stHDMIRxInfo.stPollingInfo.bIsRepeater = FALSE;
            stHDMIRxInfo.stPollingInfo.bAutoEQIgnoreFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.bHDREnableFlag = FALSE;
            //stHDMIRxInfo.stPollingInfo.bHDMI14SettingFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.ucSourceVersion = HDMI_SOURCE_VERSION_NOT_SURE;
            stHDMIRxInfo.stPollingInfo.ucHDCPState = HDMI_HDCP_NO_ENCRYPTION;
            stHDMIRxInfo.stPollingInfo.ucHDCP14IntStatus = 0;
            stHDMIRxInfo.stPollingInfo.bDVIDualLinkFlag = FALSE;
            stHDMIRxInfo.stPollingInfo.ucDVISingleLinkCount = 0;
            stHDMIRxInfo.stPollingInfo.ucDVIDualLinkCount = 0;
            stHDMIRxInfo.stPollingInfo.ucCurrentPort = 0xFF;

            stHDMIRxInfo.stAudioInfo.usAudioFrequency = 0;
            stHDMIRxInfo.stAudioInfo.ulAudioCTSNValue = 0;

            if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortTypeHDMIIndex, (BIT0 << enInputPortSelect)))
            {
                if(enInputPortSelect == 0)
                {
                    Init_hwHDCP_Hpd_Pin();
                }
                else if(enInputPortSelect == 1)
                {
                    Init_hwHDCP_Hpd_Pin2();
                }
                else if(enInputPortSelect == 2)
                {
                    Init_hwHDCP_Hpd_Pin3();
                }

                if(ucMode == 0)
                {
                    mdrv_hdmiRx_HPDControl(enInputPortSelect, FALSE);
                    bLowFlag = 1;
#if ENABLE_HPD_REPLACE_MODE
                    Hal_HDMI_SCDC_config(enInputPortSelect, TRUE);
                    Hal_HDMI_SCDC_Clr(enInputPortSelect);
                    Hal_HDMI_MAC_HDCP_Enable(FALSE);
                }

                // DC on and not in PM: do HPD;
                // g_eBootStatus only be updated in EXIT_PM state.
                // DC off/ON -> DPMS, mdrv_hdmiRx_Initial will be called in checkagain()
                // need to use ePMState == ePM_IDLE to make sure the HPD behavior is set in AP not in PM case.
                if((g_eBootStatus == eBOOT_STATUS_DCON)&& (sPMInfo.ePMState == ePM_IDLE))
                {
                    mdrv_hdmiRx_HPDControl(enInputPortSelect, FALSE);
                    bLowFlag = 1;
#endif
                }

                Hal_HDMI_init(enInputPortSelect);

            }

#if ENABLE_HDMI_EDID
#if ENABLE_HDMI_EDID_INTERNAL_DATA
            mdrv_HDMIRx_LoadEDID(enInputPortSelect, tCOMBO_EDID_TALBE_PORT[enInputPortSelect]);
#endif
#endif
        }
    }

    return bLowFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_HPD_Resume()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_HPD_Resume(void)
{
    EN_HDMI_INPUT_PORT enInputPortSelect = HDMI_INPUT_PORT_NONE;
    ForceDelay1ms(600);
    for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, BIT(enInputPortSelect)))
        {
            mdrv_hdmiRx_ClockRtermControl(enInputPortSelect,TRUE);
        }
    }
    ForceDelay1ms(100);
    for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, BIT(enInputPortSelect)))
        {
            mdrv_hdmiRx_HPDControl(enInputPortSelect, TRUE);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_HPDControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_HPDControl(BYTE enInputPortSelect, Bool bPullHighFlag)
{
    if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << enInputPortSelect)))
    {
        if (bPullHighFlag == TRUE)
        {
            Hal_HDMI_DDCControl(enInputPortSelect, bPullHighFlag);
            Hal_HDMI_HPDControl(enInputPortSelect, bPullHighFlag);
        }
        else
        {
            Hal_HDMI_HPDControl(enInputPortSelect, bPullHighFlag);
            Hal_HDMI_DDCControl(enInputPortSelect, bPullHighFlag);
        }

        if(bPullHighFlag)
        {
            SET_HDMIRX_FLAG(stHDMIRxInfo.ucHPDControlIndex, enInputPortSelect);
        }
        else
        {
            CLR_HDMIRX_FLAG(stHDMIRxInfo.ucHPDControlIndex, enInputPortSelect);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_Handler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_Handler(void)
{
    Bool bCheckPacketReceiveFlag = FALSE;
    //EN_HDMI_INPUT_PORT enInputPortSelect = HDMI_INPUT_PORT_NONE;
    //for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
    if(ucCurrPort < HDMI_INPUT_PORT_END)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << ucCurrPort)))
        {
            Hal_tmds_BusyPolling(ucCurrPort, &stHDMIRxInfo.stPollingInfo);
        }
    }

    if(stHDMIRxInfo.b10msTimerFlag)
    {
        stHDMIRxInfo.b10msTimerFlag = FALSE;

        if(stHDMIRxInfo.ucPacketReceiveCount < HDMI_PACKET_RECEIVE_COUNT)
        {
            stHDMIRxInfo.ucPacketReceiveCount++;
        }
        else
        {
            bCheckPacketReceiveFlag = TRUE;
            stHDMIRxInfo.ucPacketReceiveCount = 0;
        }

        //for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
        if(ucCurrPort < HDMI_INPUT_PORT_END)
        {
            if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << ucCurrPort)))
            {
                if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << ucCurrPort)))
                {
#if ((TMDS_HDCP2_FUNCTION_SUPPORT) && (!DEF_COMBO_HDCP2RX_ISR_MODE))
                        //if((tmds_port[enInputPort].port_type == MsCombo_TMDS_TYPE_HDMI) && (tmds_port[enInputPort].port_power == MsCombo_TMDS_POWER_ON))
                        {
                            _mdrv_tmds_HDCP2Handler();//enInputPort);
                        }

#endif
                    Hal_HDMI_StablePolling(ucCurrPort, &stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo, &stHDMIRxInfo.stPollingInfo);
                    KHal_HDMIRx_CEDCheck(ucCurrPort);

                    if (stHDMIRxInfo.stPollingInfo.bHDMIModeFlag) //hdmi mode
                    {
                        if(bCheckPacketReceiveFlag)
                        {
                            stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo = Hal_HDMI_packet_info(ucCurrPort, NULL);

                            if (stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG)
                            {
                                stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt = 0;
                            }
                            else
                            {
                                if(stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt > GCP_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt = 0;
                                    Hal_HDMI_pkt_reset(ucCurrPort, REST_AVMUTE);
                                }
                            }
                        }
                    }
#if ENABLE_HDMI_BCHErrorIRQ
                    if(!mdrv_hdmiRx_CheckIRQMaskEnable(IRQ_BCHError))
                    {
                        if(mdrv_tmds_HDMIGetErrorStatus(ucCurrPort))
                        {
                            //BCH error happen
                            stHDMIRxInfo.stPollingInfo.ucCheckErrorInterval = 0;
                        }
                        else
                        {
                            //No BCH error
                            stHDMIRxInfo.stPollingInfo.ucCheckErrorInterval ++;
                        }

                        if(stHDMIRxInfo.stPollingInfo.ucCheckErrorInterval > BCH_Count)
                        {
                            //if 50 times no BCH error, enable BCHError IRQ Mask
                            mdrv_hdmiRx_IRQClearStatus(IRQ_BCHError); //clear BCHError IRQ status
                            mdrv_hdmiRx_IRQMaskEnable(IRQ_BCHError, TRUE); // enable BCHError IRQ
                            stHDMIRxInfo.stPollingInfo.u16CheckBCHMuteCnt = 0;
                        }
                    }
#endif
                }
             }
         }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_TimerInterrupt()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_TimerInterrupt(void)
{
    if(ucCurrPort < HDMI_INPUT_PORT_END)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << ucCurrPort)))
        {
            if(stHDMIRxInfo.stPollingInfo.bHDMIModeFlag)
            {
                if(stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt <= GCP_PKT_LOSS_BOUND)
                {
                    stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt++;
                }
            }
            else
            {
                stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt = 0;
            }
        }

        if(stHDMIRxInfo.stPollingInfo.u16CheckBCHMuteCnt > 0)
        {
            stHDMIRxInfo.stPollingInfo.u16CheckBCHMuteCnt--;
        }
    }
}

#if ENABLE_HDMI_BCHErrorIRQ
//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_BCHInterrupt()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_BCHInterrupt(void)
{
    mdrv_hdmiRx_IRQMaskEnable(IRQ_BCHError, FALSE); //if no BCHError Happened, disable BCHError IRQ
}
#endif

//**************************************************************************
//  [Function Name]:
//                  _mdrv_hdmi_SetupInputPort()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
extern void Delay1ms( WORD msNums );
#if ENABLE_HPD_REPLACE_MODE
void mdrv_hdmi_SetupInputPort(BYTE ucPortSelect)
{
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(ucPortSelect);
    BOOL bFlag = 0;

    if(Hal_HDMI_SCDC_status(ucPortMapping))
    {
        bFlag = TRUE;
        mdrv_hdmiRx_HPDControl(ucPortMapping, FALSE);
        ForceDelay1ms(100);
        mdrv_hdmiRx_ClockRtermControl(ucPortMapping,FALSE);
        Hal_HDMI_DataRtermControl(ucPortMapping,FALSE);

    }
    Hal_tmds_Switch2HDMI(ucPortMapping);
    if(bFlag)
    {
        ForceDelay1ms(500);
        mdrv_hdmiRx_ClockRtermControl(ucPortMapping,TRUE);
        Hal_HDMI_DataRtermControl(ucPortMapping,TRUE);
        ForceDelay1ms(100);
        mdrv_hdmiRx_HPDControl(ucPortMapping, TRUE);
    }
    else
    {
        Hal_HDMI_DataRtermControl(ucPortMapping,TRUE);
    }
    Hal_HDMI_MAC_HDCP_Enable(TRUE);
    ucCurrPort = ucPortMapping;
}
#else
void mdrv_hdmi_SetupInputPort(BYTE ucPortSelect)
{
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(ucPortSelect);

    Hal_tmds_Switch2HDMI(ucPortMapping);

    ucCurrPort = ucPortMapping;
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_PMSwitchDVIDetect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_PMSwitchDVIDetect (BYTE ucPortSelect)
{
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(ucPortSelect);

    Hal_HDMI_PM_SwitchHDMIPort(ucPortMapping);
}

//**************************************************************************
//  [Function Name]:
//                  Mdrv_tmds_HDCP2Handler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_tmds_HDCP2Handler(void)
{
    BYTE ucHDCPRxData[TMDS_HDCP2_RX_QUEUE_SIZE]={0};
    MSCombo_TMDS_PORT_INDEX enInputPort = MSCombo_TMDS_PORT_0;

    //for(enInputPort = MSCombo_TMDS_PORT_0; enInputPort <= MSCombo_TMDS_PORT_1; enInputPort++)
    {
        if(mhal_tmds_HDCP2CheckWriteDone(enInputPort))
        {
            mhal_tmds_HDCP2GetRxData(enInputPort, ucHDCPRxData);

            if(pmdrv_tmds_HDCP2RxEventProc != NULL)  // Have call back function
            {
                pmdrv_tmds_HDCP2RxEventProc((BYTE)enInputPort, ucHDCPRxData);
            }
        }

        if(mhal_tmds_HDCP2ReadDone(enInputPort))
        {
            if(pmdrv_tmds_HDCP2RxEventProc != NULL)  // Have call back function
            {
                pmdrv_tmds_HDCP2RxEventProc((BYTE)enInputPort, ucHDCPRxData);
            }
        }
    }

}

#if (TMDS_HDCP2_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  _mdrv_tmds_HDCP2Handler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void _mdrv_tmds_HDCP2Handler(MSCombo_TMDS_PORT_INDEX enInputPort)
{
    BYTE ucHDCPRxData[HDMI_HDCP2_RECEIVE_QUEUE_SIZE]={0};

    //if(tmds_port[enInputPort].port_type == MsCombo_TMDS_TYPE_HDMI)
    {
        // HDCP 2.2 RX
        if(mhal_tmds_HDCP2CheckWriteDone(enInputPort))
        {
            mhal_tmds_HDCP2GetRxData(enInputPort, ucHDCPRxData);

            if(pmdrv_tmds_HDCP2RxEventProc != NULL)  // Have call back function
            {
                //[MT9700]pmdrv_tmds_HDCP2RxEventProc((BYTE)enInputPort, ucHDCPRxData);
                pmdrv_tmds_HDCP2RxEventProc(0, ucHDCPRxData);
            }
        }
    }
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_GetPacketContent()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mdrv_hdmiRx_GetPacketContent(MS_HDMI_PACKET_STATE_t ucPacketType, BYTE ucPacketLength, BYTE *pPacketData, BYTE ucPortSelect)
{
    BOOL bGetContentFlag = FALSE;
    BOOL bSwitchContentFlag = FALSE;
    BYTE u8temp = 0;
    BYTE ucPKTData = 0;
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(ucPortSelect);
    DWORD ulPacketStatus = 0;

    ulPacketStatus = stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo;

    switch(ucPacketType)
    {
        case PKT_MPEG:
            if(ulPacketStatus & HDMI_STATUS_MPEG_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_MPEG_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_MPEG_PACKET_LENGTH;
                }
            }
            break;

        case PKT_AUI:
            if(ulPacketStatus & HDMI_STATUS_AUDIO_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_AUDIO_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_AUDIO_PACKET_LENGTH;
                }
            }
            break;

        case PKT_SPD:
            if(ulPacketStatus &HDMI_STATUS_SPD_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_SPD_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_SPD_PACKET_LENGTH;
                }
            }
            break;

        case PKT_AVI:
            if(ulPacketStatus &HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_AVI_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_AVI_PACKET_LENGTH;
                }
            }
            break;

        case PKT_GC:
            if(ulPacketStatus &HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_GCP_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_GCP_PACKET_LENGTH;
                }
            }
            break;

        case PKT_ASAMPLE:
            break;

        case PKT_ACR:
            break;

        case PKT_VS:
            if(ulPacketStatus &HDMI_STATUS_VS_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_VS_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_VS_PACKET_LENGTH;
                }
            }
            break;

        case PKT_NULL:
            break;

        case PKT_ISRC2:
            if(ulPacketStatus &HDMI_STATUS_ISRC2_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_ISRC2_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_ISRC2_PACKET_LENGTH;
                }
            }
            break;

        case PKT_ISRC1:
            if(ulPacketStatus &HDMI_STATUS_ISRC1_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_ISRC1_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_ISRC1_PACKET_LENGTH;
                }
            }
            break;

        case PKT_ACP:
            if(ulPacketStatus &HDMI_STATUS_ACP_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_ACP_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_ACP_PACKET_LENGTH;
                }
            }
            break;

        case PKT_ONEBIT_AUD:
            break;

        case PKT_GM:
            break;

        case PKT_HBR:
            break;

        case PKT_VBI:
            break;

        case PKT_HDR:
            if(ulPacketStatus & HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;
                bSwitchContentFlag = TRUE;

                if(ucPacketLength > HDMI_HDR_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_HDR_PACKET_LENGTH;
                }
            }
            break;

        case PKT_RSV:
            break;

        case PKT_EDR:
            break;

        default:
            break;
    }

    if(bGetContentFlag)
    {
        for(u8temp = 0; u8temp < ucPacketLength; u8temp++)
        {
            if(Hal_HDMI_get_packet_value(ucPortMapping, ucPacketType, u8temp, &ucPKTData))
            {
                if(bSwitchContentFlag)
                {
                    if(u8temp % 2)
                    {
                        pPacketData[u8temp -1] = ucPKTData;
                    }
                    else
                    {
                        if(u8temp == (ucPacketLength -1))
                        {
                            pPacketData[u8temp] = ucPKTData;
                        }
                        else
                        {
                            pPacketData[u8temp+1] = ucPKTData;
                        }
                    }
                }
                else
                {
                    pPacketData[u8temp] = ucPKTData;
                }
            }
        }
    }

    switch(ucPacketType)
    {
        case PKT_CHANNEL_STATUS:
            if(ulPacketStatus &HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                if(ucPacketLength > HDMI_AUDIO_CHANNEL_STATUS_LENGTH)
                {
                    ucPacketLength = HDMI_AUDIO_CHANNEL_STATUS_LENGTH;
                }

                for(u8temp = 0; u8temp < ucPacketLength; u8temp++)
                {
                    pPacketData[u8temp] = Hal_HDMI_audio_channel_status(ucPortMapping, u8temp);
                }
            }
            break;

        case PKT_MULTI_VS:
            if(ulPacketStatus &HDMI_STATUS_VS_PACKET_RECEIVE_FLAG)
            {
                bGetContentFlag = TRUE;

                pPacketData[HDMI_MULTI_VS_PACKET_LENGTH] = Hal_HDMI_GetDataInfo(E_HDMI_GET_MULTIVS_COUNT, ucPortMapping);

                if(pPacketData[HDMI_MULTI_VS_PACKET_LENGTH] > 4)
                {
                    pPacketData[HDMI_MULTI_VS_PACKET_LENGTH] = 4;
                }

                ucPacketLength = pPacketData[HDMI_MULTI_VS_PACKET_LENGTH] *(HDMI_VS_PACKET_LENGTH -1);

                if(ucPacketLength > HDMI_MULTI_VS_PACKET_LENGTH)
                {
                    ucPacketLength = HDMI_MULTI_VS_PACKET_LENGTH;
                }

                for(u8temp = 0; u8temp < ucPacketLength; u8temp++)
                {
                    if(Hal_HDMI_get_packet_value(ucPortMapping, ucPacketType, u8temp, &ucPKTData))
                    {
                        pPacketData[u8temp] = ucPKTData;
                    }
                }
            }
            break;

        default:
            break;
    }

    return bGetContentFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_GetColorimetry()
//  [Description]:
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
ST_HDMI_RX_COLOR_FORMAT mdrv_hdmiRx_GetColorimetry(BYTE ucPortSelect)
{
    ST_HDMI_RX_COLOR_FORMAT ucColorFormat;
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(ucPortSelect);

    ucColorFormat.ucExtColorimetry = COMBO_COLORIMETRY_NONE;
    ucColorFormat.ucColorimetry= COMBO_YUV_COLORIMETRY_NoData;
    ucColorFormat.ucExtAddColorimetry = COMBO_COLORIMETRY_Additional_NoData;

    do
    {
        if (!(stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG)) //avi packet received;
        {
            break;
        }

        ucColorFormat = Hal_HDMI_GetColorimetry(ucPortMapping);
    }while(FALSE);

    return ucColorFormat;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_IsAudioFmtPCM()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
EN_TMDS_AUDIO_FORMAT mdrv_hdmiRx_IsAudioFmtPCM(BYTE ucPortIndex)
{
    EN_TMDS_AUDIO_FORMAT enAudioFMT = COMBO_AUDIO_FMT_UNKNOWN;
    if(stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_AUDIO_SAMPLE_PACKET_RECEIVE_FLAG)
    {
        enAudioFMT = Hal_HDMI_IsAudioFmtPCM(ucPortIndex);
    }

    return enAudioFMT;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_GetVideoContentInfo()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD mdrv_hdmiRx_GetVideoContentInfo(EN_HDMI_VIDEO_CONTENT_INFO enVideoContentInfo, BYTE ucPortSelect)
{
    WORD usVideoContent = 0;
    BYTE ucInputPort = mdrv_hdmiRx_InputPortMapping(ucPortSelect);
    BYTE ucVSIcontent[32] = {0};

    switch(enVideoContentInfo)
    {
        case HDMI_VIDEO_CONTENT_HDE:
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_HDE, ucInputPort);
            break;

        case HDMI_VIDEO_CONTENT_HTT:
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_HTT, ucInputPort);
            break;

        case HDMI_VIDEO_CONTENT_VDE:
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_VDE, ucInputPort);
            break;

        case HDMI_VIDEO_CONTENT_VTT:
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_VTT, ucInputPort);
            break;

        case HDMI_VIDEO_CLK_STABLE:
            usVideoContent =  stHDMIRxInfo.stPollingInfo.bClockStableFlag;
            break;

        case HDMI_VIDEO_CLK_COUNT:
            usVideoContent =  stHDMIRxInfo.stPollingInfo.usClockCount;
            break;

        case HDMI_VIDEO_DE_STABLE:
            //usVideoContent =  stHDMIRxInfo.stPollingInfo.bDEStableFlag;
            usVideoContent =  Hal_HDMI_GetDEStableStatus(ucInputPort);
            break;

        case HDMI_VIDEO_FREE_SYNC_FLAG:
            if (Hal_HDMI_GetFreeSyncInfo(ucInputPort, stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo, NULL)& HDMI_FREE_SYNC_SUPPORTED) //avi packet received;
            {
                usVideoContent =  TRUE;
            }
            else
            {
                usVideoContent = FALSE;
            }
            break;

        case HDMI_VIDEO_COLOR_FROMAT:
            if (stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG) //avi packet received;
            {
                usVideoContent =  Hal_HDMI_GetColorFormat(ucInputPort);
            }
            else
            {
                usVideoContent = COMBO_COLOR_FORMAT_DEFAULT;
            }
            break;

        case HDMI_VIDEO_COLOR_DEPTH:
            if (stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG) //GC packet received;
            {
                usVideoContent =  Hal_HDMI_GetGCPColorDepth(ucInputPort, &stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo);
            }
            else
            {
                usVideoContent = HDMI_COLOR_DEPTH_NONE;
            }
            break;

        case HDMI_VIDEO_COLOR_RANGE:
            if (stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG) //avi packet received;
            {
                usVideoContent =  Hal_HDMI_GetColorRange(ucInputPort);
            }
            else
            {
                usVideoContent = (BYTE)COMBO_COLOR_RANGE_RESERVED;
            }
            break;

        case HDMI_VIDEO_HDR_FLAG:
            if(stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG)
            {
                usVideoContent = TRUE;
            }
            else
            {
                usVideoContent = FALSE;
            }
            break;

        case HDMI_VIDEO_AVMUTE_FLAG:
            if (stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG) //GC packet received;
            {
                usVideoContent =  (Hal_HDMI_GetAVMuteEnableFlag(ucInputPort) == TRUE) ? 0x01 : 0x00; //convert bool to word;
            }
            else
            {
                usVideoContent = 0x00;
            }
            break;

        case HDMI_VIDEO_VRR_FLAG:
            if(stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & HDMI_STATUS_EM_PACKET_RECEIVE_FLAG) //EM packet received;
            {
                usVideoContent = (Hal_HDMI_GetVRREnableFlag(ucInputPort) == TRUE) ? 0x01 : 0x00; //convert bool to word;
            }
            else
            {
                usVideoContent = 0x00;
            }
            break;

        case HDMI_VIDEO_ALLM_FLAG:
            if(mdrv_hdmiRx_GetPacketContent(HDMI_STATUS_VS_PACKET_RECEIVE_FLAG, 32, ucVSIcontent, ucInputPort)) //VSIF packet received;
            {
                if((ucVSIcontent[5] == 0xD8) && (ucVSIcontent[6] == 0x5D) && (ucVSIcontent[7] == 0xC4))
                {
                    if(ucVSIcontent[9] & 0x02)
                    {
                        usVideoContent = TRUE;
                    }
                    else
                    {
                        usVideoContent = FALSE;
                    }
                }

                if((ucVSIcontent[5] == 0x8B) && (ucVSIcontent[6] == 0x84) && (ucVSIcontent[7] == 0x90)) //HDR10+ VSIF
                {
                    if(ucVSIcontent[31] & 0x10)
                    {
                        usVideoContent = TRUE;
                    }
                    else
                    {
                        usVideoContent = FALSE;
                    }
                }
            }
            else
            {
                usVideoContent = 0x00;
            }
            break;

        case HDMI_VIDEO_HDMI_MODE_FLAG:
            if(stHDMIRxInfo.stPollingInfo.bHDMIModeFlag)
            {
                usVideoContent = 0x01; //hdmi mode
            }
            else
            {
                usVideoContent = 0x00; //dvi mode
            }
            break;

        default:

            break;
    }

    return usVideoContent;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_ClockRtermControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_ClockRtermControl(EN_HDMI_INPUT_PORT enInputPortSelect, Bool bPullHighFlag)
{
    Hal_HDMI_ClockRtermControl(enInputPortSelect, bPullHighFlag);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_DataRtermControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_DataRtermControl(EN_HDMI_INPUT_PORT enInputPortSelect, Bool bPullHighFlag)
{
    Hal_HDMI_DataRtermControl(enInputPortSelect, bPullHighFlag);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_DDCControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_DDCControl(BYTE enInputPortType, Bool bEnable)
{
    Hal_HDMI_DDCControl(enInputPortType,bEnable);
}

#if ENABLE_HPD_REPLACE_MODE
//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_SCDC_Clr()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_SCDC_Clr(BYTE enInputPortType)
{
    Hal_HDMI_SCDC_Clr(enInputPortType);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_SCDC_config()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_SCDC_config(BYTE enInputPortSelect, BYTE bDDC)
{
    Hal_HDMI_SCDC_config(enInputPortSelect, bDDC);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_MAC_HDCP_Enable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_MAC_HDCP_Enable(Bool bEnable)
{
    Hal_HDMI_MAC_HDCP_Enable(bEnable);
}
#endif

#if ENABLE_HDMI_BCHErrorIRQ
//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_EnableHDMIRx_PKT_ParsrIRQ()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_EnableHDMIRx_PKT_ParsrIRQ(Bool bEnable)
{
    Hal_HDMI_EnableHDMIRx_PKT_ParsrIRQ(bEnable);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_IRQMaskEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_IRQMaskEnable(HDMI_IRQ_Mask enIRQType, Bool bEnable)
{
    Hal_HDMI_IRQMaskEnable(enIRQType, bEnable);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_CheckIRQMaskEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_hdmiRx_CheckIRQMaskEnable(HDMI_IRQ_Mask enIRQType)
{
    return Hal_HDMI_CheckIRQMaskEnable(enIRQType);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_IRQClearStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_IRQClearStatus(HDMI_IRQ_Mask enIRQType)
{
    Hal_HDMI_IRQClearStatus(enIRQType);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_IRQCheckStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_hdmiRx_IRQCheckStatus(HDMI_IRQ_Mask enIRQType)
{
    return Hal_HDMI_IRQCheckStatus(enIRQType);
}

#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_PMEnableDVIDetect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if ENABLE_HPD_REPLACE_MODE
void mdrv_hdmiRx_PMEnableDVIDetect(HDMI_PM_MODE_TYPE PMMode)
{
    EN_HDMI_INPUT_PORT ucPortSelect;

    for(ucPortSelect = HDMI_INPUT_PORT0; ucPortSelect < HDMI_INPUT_PORT_END; ucPortSelect++)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << ucPortSelect)))
        {
            Hal_HDMI_PM_SwitchHDMIPort(ucPortSelect);
            if(PMMode == HDMI_PM_POWERON)//disable dvi clock detect!
            {
                Hal_HDMI_PMEnableDVIDetect(ucPortSelect, FALSE, HDMI_PM_POWERON);
                Hal_HDMI_ClockRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DataRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DDCControl(ucPortSelect, TRUE);
                Hal_HDMI_HPDControl(ucPortSelect, TRUE);
                Hal_HDMI_PHYPowerModeSetting(ucPortSelect , PMMode);
                Hal_HDMI_DataRtermControl(ucPortSelect, FALSE);
            }
            else if(PMMode == HDMI_PM_POWEROFF)
            {
                Hal_HDMI_PMEnableDVIDetect(ucPortSelect, FALSE, HDMI_PM_POWEROFF);
                Hal_HDMI_ClockRtermControl(ucPortSelect, FALSE);
                Hal_HDMI_DataRtermControl(ucPortSelect, FALSE);
                Hal_HDMI_DDCControl(ucPortSelect, FALSE);
                Hal_HDMI_SCDC_config(ucPortSelect, TRUE);
                Hal_HDMI_SCDC_Clr(ucPortSelect);
                Hal_HDMI_HPDControl(ucPortSelect, TRUE);
                Hal_HDMI_PHYPowerModeSetting(ucPortSelect , PMMode);
            }
            else//(PMMode == HDMI_PM_STANDBY)
            {
                Hal_HDMI_PMEnableDVIDetect(ucPortSelect, TRUE, HDMI_PM_STANDBY);
                Hal_HDMI_ClockRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DataRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DDCControl(ucPortSelect, TRUE);
                Hal_HDMI_SCDC_config(ucPortSelect, TRUE);
                Hal_HDMI_SCDC_Clr(ucPortSelect);
                Hal_HDMI_HPDControl(ucPortSelect, TRUE);
                Hal_HDMI_PHYPowerModeSetting(ucPortSelect , PMMode);
            }

        }
    }
}
#else
void mdrv_hdmiRx_PMEnableDVIDetect(HDMI_PM_MODE_TYPE PMMode)
{
    EN_HDMI_INPUT_PORT ucPortSelect;

    for(ucPortSelect = HDMI_INPUT_PORT0; ucPortSelect < HDMI_INPUT_PORT_END; ucPortSelect++)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << ucPortSelect)))
        {
            Hal_HDMI_PM_SwitchHDMIPort(ucPortSelect);
            if(PMMode == HDMI_PM_POWERON)//disable dvi clock detect!
            {
                Hal_HDMI_PMEnableDVIDetect(ucPortSelect, FALSE, HDMI_PM_POWERON);
                Hal_HDMI_ClockRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DataRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DDCControl(ucPortSelect, TRUE);
                Hal_HDMI_HPDControl(ucPortSelect, TRUE);
                Hal_HDMI_PHYPowerModeSetting(ucPortSelect , PMMode);
                Hal_HDMI_DataRtermControl(ucPortSelect, FALSE);
            }
            else if(PMMode == HDMI_PM_POWEROFF)
            {
                Hal_HDMI_PMEnableDVIDetect(ucPortSelect, FALSE, HDMI_PM_POWEROFF);
                Hal_HDMI_ClockRtermControl(ucPortSelect, FALSE);
                Hal_HDMI_DataRtermControl(ucPortSelect, FALSE);
                Hal_HDMI_DDCControl(ucPortSelect, FALSE);
                Hal_HDMI_HPDControl(ucPortSelect, TRUE);
                Hal_HDMI_PHYPowerModeSetting(ucPortSelect , PMMode);
            }
            else//(PMMode == HDMI_PM_STANDBY)
            {
                Hal_HDMI_PMEnableDVIDetect(ucPortSelect, TRUE, HDMI_PM_STANDBY);
                Hal_HDMI_ClockRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DataRtermControl(ucPortSelect, TRUE);
                Hal_HDMI_DDCControl(ucPortSelect, TRUE);
                Hal_HDMI_HPDControl(ucPortSelect, TRUE);
                Hal_HDMI_PHYPowerModeSetting(ucPortSelect , PMMode);
            }

        }
    }
}
#endif
#endif //#if (ENABLE_HDMI || ENABLE_DVI)

#if(TMDS_HDCP2_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_SetHDCP2CallBackFunction()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_tmds_SetHDCP2CallBackFunction(TMDS_HDCP2_CALL_BACK_FUNC pHDCP2RxFunc, BYTE *pHDCP2CertRx)
{
    pmdrv_tmds_HDCP2RxEventProc = pHDCP2RxFunc;
    pHDCPTxData_CertRx = pHDCP2CertRx;
    {
        MSCombo_TMDS_PORT_INDEX enInputPort = MSCombo_TMDS_PORT_0;

        //for(enInputPort = MSCombo_TMDS_PORT_0; enInputPort <= MSCombo_TMDS_PORT_1; enInputPort++)
        {
            mhal_tmds_HDCP2Initial(enInputPort);
#if(DEF_COMBO_HDCP2RX_ISR_MODE)
            mhal_tmds_HDCP2InterruptEnable(enInputPort, TRUE);
#endif
        }
    }

}

void mdrv_tmds_HDCP2FetchMsg(BYTE enInputPort)
{
    BYTE ucHDCPRxData[HDMI_HDCP2_RECEIVE_QUEUE_SIZE]={0};

    mhal_tmds_HDCP2GetRxData((MSCombo_TMDS_PORT_INDEX)enInputPort, ucHDCPRxData);

    if(pmdrv_tmds_HDCP2RxEventProc != NULL)  // Have call back function
    {
        pmdrv_tmds_HDCP2RxEventProc(enInputPort, ucHDCPRxData);
    }
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_SetHDCP2TxCallBackFunction()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_SetHDCP2CallBackFunction(BYTE ucPortSelect, COMBO_HDCP2_CALL_BACK_FUNC pHDCP2TxFunc)
{
    if(pHDCP2TxFunc != NULL)
    {
    #if (COMBO_HDCP2_FUNCTION_SUPPORT)
        HDCPHandler_AttachCBFunc(ucPortSelect, pHDCP2TxFunc);
    #endif
    }
    else
    {
        //COMBO_DRV_DPUTSTR("[Err] HDCP2 CB func init failed!!!\r\n");
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_HDCP2Handler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_HDCP2Handler(void)
{
    #if ((defined(DEF_COMBO_HDCP2RX_ISR_MODE) && (DEF_COMBO_HDCP2RX_ISR_MODE == 1)))
        HDCP_ISRModeHandler();
    #else
		HDCP_NonISRModeHandler();
    #endif //((defined(DEF_COMBO_HDCP2RX_ISR_MODE) && (DEF_COMBO_HDCP2RX_ISR_MODE ==1)))

	return;
}

#if(COMBO_HDCP2_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_HDCP2RxEventProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mdrv_combo_HDCP2RxEventProc(BYTE ucPortSelect, BYTE *pMessage)
{
    //start parsing message and store to mbx handler
    return HDCPHandler_Hdcp2MsgDispatcher(ucPortSelect, pMessage);
}

void mdrv_combo_HDCP2ResetRomCodeflag(void)
{
    bRomCodeDone=0;
    bFirstDoneFlag=0;
    bKeyDecodeDone = FALSE;
    HDCPHandler_SetSecR2EventFlag();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_HDCP2RomCodeDone()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_combo_HDCP2RomCodeDone(void)
{
    //static Bool bRomCodeDone = FALSE;
    //static Bool bFirstDoneFlag = FALSE;
    Bool bSecondDoneFlag = FALSE;

    if(!bRomCodeDone)
    {

		if(HDCPHandler_GetR2EventFlag()||  HDCPHandler_UpdateRomR2FirstEventFlag())//[MT9700]
        {
            if (mhal_tmds_HDCP2CheckRomCodeResult())
            {
                bRomCodeDone = TRUE;
                bFirstDoneFlag = TRUE;
                bSecondDoneFlag = TRUE;
                HDCP22_DRV_DPUTSTR("**Combo check HDCP22 ROM code done\r\n");
            }
            else
            {
                bRomCodeDone = TRUE;
                HDCP22_DRV_DPUTSTR("**Combo check HDCP22 ROM done, but Result is NG!\r\n");
            }
        }
    }
    else
    {
        if (!bFirstDoneFlag)
        {
            bFirstDoneFlag = mhal_tmds_HDCP2CheckRomCodeResult();
            bSecondDoneFlag = TRUE;
        }
    }

    return bSecondDoneFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_HDCP2KeyDecodeDone()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_combo_HDCP2KeyDecodeDone(void)
{
    //static Bool bKeyDecodeDone = FALSE;
    Bool bSecondDoneFlag = FALSE;

    if(!bKeyDecodeDone)
    {
		if(HDCPHandler_GetR2EventFlag())
        {
            bKeyDecodeDone = TRUE;
            bSecondDoneFlag = TRUE;

            HDCP22_DRV_DPUTSTR("**Combo check HDCP22 key decode done\r\n");
        }
    }

    return bSecondDoneFlag;
}
#endif

#if(COMBO_HDCP2_FUNCTION_SUPPORT)  //CHIP after 9U3
//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_HDCP2TxEventProc()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_tmds_HDCP2TxEventProc(BYTE enInputPort, BYTE ucMessageID, WORD wDataSize, BYTE *pHDCPTxData)
{
    if(wDataSize <= HDMI_HDCP2_SEND_QUEUE_SIZE)
    {
        //if(tmds_port[enInputPort].port_type == MsCombo_TMDS_TYPE_HDMI)
        {
            // HDCP 2.2 TX
            if(!mhal_tmds_HDCP2CheckWriteStart((MSCombo_TMDS_PORT_INDEX)enInputPort))
            {
                mhal_tmds_HDCP2SetTxData((MSCombo_TMDS_PORT_INDEX)enInputPort, ucMessageID, wDataSize, pHDCPTxData, pHDCPTxData_CertRx);
            }
        }
    }
    #if 0
    else
    {
        DRV_TMDS_PrtData(" TMDS HDCP2.2 Tx queue size too small port %d", enInputPort);
    }
    #endif
}

#if (defined(COMBO_HDCPTX_BLOCK_ENABLE) && (COMBO_HDCPTX_BLOCK_ENABLE == 1))
//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_SetHDCPTxAccessX74CBFunction()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_SetHDCPTxIOCBFunction(BYTE ucPortType, COMBO_HDCPTX_IO_CBFUNC pHDCP2TxIOFunc)
{
	if(pHDCP2TxIOFunc != NULL)
    {
        HDCPHandler_AttachTxIOCBFunc(ucPortType, pHDCP2TxIOFunc);
    }
    else
    {
        COMBO_DRV_DPUTSTR("[Err] HDCP2 CB func init failed!!!\r\n");
    }
}
#endif
#if 0
//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_ConfigRepeaterPair()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_ConfigRepeaterPair(BYTE u8InputPort, BYTE u8OutputPort, Bool bEnableFlag)
{
    HDCPRepeater_ConfigRepeaterPair(u8InputPort, u8OutputPort, bEnableFlag);
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_HDCP2Initial()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_HDCP2Initial(void)
{
	//mhal_combo_HDCP2Initial();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_HDCP14ReadBKSV()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_HDCP14ReadBKSV(BYTE ucPortSelect)
{
    //start parsing message and store to mbx handler
    HDCPHandler_Hdcp14BKSVRead(ucPortSelect);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_HDCP14ReadBKSV()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mdrv_combo_HDCP14CheckVPrimePrepared(BYTE ucPortSelect)
{
	//start parsing message and store to mbx handler
    return HDCPHandler_Hdcp14CheckVPrimePrepared(ucPortSelect);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_SetHDCPRxWriteX74Function()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_SetHDCPRxWriteX74Function(BYTE ucPortType, COMBO_WRITEX74_FUNC pHDCPWX74Func)
{
    if(pHDCPWX74Func != NULL)
    {
        HDCPHandler_AttachWriteX74Func(ucPortType, pHDCPWX74Func);
    }
    else
    {
        printf("[Err] HDCP WriteX74 func init failed!!!\r\n");
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_SetHDCPRxReadX74Function()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_SetHDCPRxReadX74Function(BYTE ucPortType, COMBO_READX74_FUNC pHDCPRX74Func)
{
    if(pHDCPRX74Func != NULL)
    {
        HDCPHandler_AttachReadX74Func(ucPortType, pHDCPRX74Func);
    }
    else
    {
        printf("[Err] HDCP READX74 func init failed!!!\r\n");
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_SetHDCPRepKSVFIFFunction()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_SetHDCPRepKSVFIFOFunction(BYTE ucPortType, COMBO_KSVFIFO_FUNC pHDCPKSVFunc)
{
    if(pHDCPKSVFunc != NULL)
    {
        HDCPHandler_AttachRepKSVFIFOFunc(ucPortType, pHDCPKSVFunc);
    }
    else
    {
        printf("[Err] HDCP KSVFIFO func init failed!!!\r\n");
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_SetHDCPTxCheckRiFunction()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_SetHDCPTxCompareRiFunction(BYTE ucPortType, COMBO_HDCPTX_COMPARE_RI_FUNC pHDCP2TxRiFunc)
{
	if(pHDCP2TxRiFunc != NULL)
    {
        HDCPHandler_AttachTxCompareRiFunc(ucPortType, pHDCP2TxRiFunc);
    }
    else
    {
        printf("[Err] HDCP Compare Ri func init failed!!!\r\n");
    }

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_combo_SetHDCPTxGetRiFunction()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_combo_SetHDCPTxGetDPTXR0Function(BYTE ucPortType, COMBO_HDCPTX_GET_DPR0_FUNC pHDCP2DPTxR0Func)
{
	if(pHDCP2DPTxR0Func != NULL)
    {
        HDCPHandler_AttachTxGetDPTXR0Func(ucPortType, pHDCP2DPTxR0Func);
    }
    else
    {
        printf("[Err] HDCP Get  DPTXR0 func init failed!!!\r\n");
    }

	return;
}

#endif
//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_HDMIGetErrorStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:


//**************************************************************************
Bool mdrv_tmds_HDMIGetErrorStatus(BYTE enInputPortType)
{
    return mhal_tmds_HDMIGetErrorStatus(enInputPortType);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_HDMIGetErrorStatus()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:


//**************************************************************************
Bool mdrv_tmds_HDMIGetBCHErrorStatus(BYTE enInputPortType)
{
    return mhal_tmds_HDMIGetBCHErrorStatus(enInputPortType);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_CheckHDCPState()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mdrv_hdmiRx_CheckHDCPState(void)
{
    return mhal_hdmiRx_CheckHDCPState();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_Cable_5V_Detect()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_BOOL mdrv_hdmiRx_Cable_5V_Detect(BYTE ucInputPort)
{
    MS_BOOL Detect5V = FALSE;
    Detect5V = Hal_HDMIRx_GetSCDC_Tx5V_PwrDetectFlag(ucInputPort);

    return Detect5V ;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_Set_RB_PN_Swap()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mdrv_hdmiRx_Set_RB_PN_Swap(BYTE ucPortIndex, HDMI_SWAP_TYPE enHDMI_SWAP_TYPE, Bool bSwapEnable)
{
    Bool bSwapDone = FALSE;
    bSwapDone = Hal_HDMIRx_Set_RB_PN_Swap(ucPortIndex, enHDMI_SWAP_TYPE, bSwapEnable);

    return bSwapDone ;
}

#endif // MDRV_HDMIRX_C

