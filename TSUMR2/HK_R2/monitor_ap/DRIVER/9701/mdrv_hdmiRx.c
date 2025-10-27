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
#if(HDMI_DEBUG_MESSAGE_DRV && ENABLE_DEBUG)
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
ST_HPD_CONTROL stHDMIHPD_CTRL[HDMI_INPUT_PORT_END];
static BYTE ucCurrPort = HDMI_INPUT_PORT_NONE;
#if 0   // patch for chroma 2238
static MS_BOOL bIsSignalEncrypt[HDMI_INPUT_PORT_END] = {FALSE,FALSE};
#endif
const BYTE tEDID_TABLE_COMBO_PORT0[256] =
{
#if 0   // H14
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x36,0x74,0x30,0x00,0x01,0x00,0x00,0x00,
    0x0A,0x14,0x01,0x03,0x80,0x46,0x28,0x78,0x0A,0x0D,0xC9,0xA0,0x57,0x47,0x98,0x27,
    0x12,0x48,0x4C,0x20,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,
    0x45,0x00,0xDF,0xA4,0x21,0x00,0x00,0x1E,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,
    0x58,0x2C,0x25,0x00,0xDF,0xA4,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0xFC,0x00,0x4D,
    0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6D,0x6F,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
    0x00,0x3A,0x3E,0x0F,0x46,0x0F,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xA1,
    0x02,0x03,0x34,0x71,0x51,0x94,0x13,0x05,0x03,0x04,0x02,0x01,0x16,0x15,0x07,0x06,
    0x11,0x10,0x12,0x1F,0x5E,0x5F,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x67,0x03,
    0x0C,0x00,0x10,0x00,0xA8,0x2D,0x6D,0x1A,0x00,0x00,0x02,0x0B,0x30,0x90,0x00,0x04,
    0x8B,0x12,0x7D,0x15,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,0x55,0x00,
    0xDF,0xA4,0x21,0x00,0x00,0x1E,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,
    0x96,0x00,0xDF,0xA4,0x21,0x00,0x00,0x18,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,
    0x10,0x3E,0x96,0x00,0x30,0xA4,0x21,0x00,0x00,0x18,0x8C,0x0A,0xA0,0x14,0x51,0xF0,
    0x16,0x00,0x26,0x7C,0x43,0x00,0x30,0xA4,0x21,0x00,0x00,0x98,0x00,0x00,0x00,0xCE
#elif 0 // H20 FHD180
    0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
    0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
    0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
    0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
    0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x06,	0xAE,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
    0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
    0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
    0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x3D,
    0x02,	0x03,	0x66,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
    0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x6E,	0x03,	0x0C,	0x00,	0x10,
    0x00,	0xF8,	0x3C,	0x2F,	0xC0,	0x80,	0x01,	0x02,	0x03,	0x04,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,
    0xC0,	0x07,	0x6D,	0x1A,	0x00,	0x00,	0x02,	0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,
    0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x00,	0x80,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,
    0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,	0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,
    0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xD9,
#elif 0  // H20 FHD175
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x30,	0xA9,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x18,
0x02,	0x03,	0x6C,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x6E,	0x03,	0x0C,	0x00,	0x10,
0x00,	0xF8,	0x3C,	0x0F,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,
0xC0,	0x07,	0x6D,	0x1A,	0x00,	0x00,	0x02,	0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,
0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x20,	0xE0,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,
0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,	0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,
0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,
0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0xD8,
#elif 1  // H20 FHD170
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x20,	0x00,	0x00,	0x1D,	0x40,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x5B,	0xA4,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x2C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x87,
0x02,	0x03,	0x46,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x68,	0x03,	0x0C,	0x00,	0x10,
0x00,	0xB8,	0x44,	0x0F,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x58,	0x80,	0x03,	0x6D,	0x1A,	0x00,	0x00,
0x02,	0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,	0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,
0x20,	0xE0,	0xE3,	0x06,	0x0F,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x68,
#elif 1 // H20 QHD 100
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x30,	0xA9,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x18,
0x02,	0x03,	0x6C,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x6E,	0x03,	0x0C,	0x00,	0x10,
0x00,	0xF8,	0x3C,	0x0F,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,
0xC0,	0x07,	0x6D,	0x1A,	0x00,	0x00,	0x02,	0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,
0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x20,	0xE0,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,
0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,	0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,
0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,
0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0xD8,
#else   //H20
    0x00,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x00,   0x36,   0x8B,   0x96,   0xA2,   0x01,   0x01,   0x01,   0x01,
    0x01,   0x1F,   0x01,   0x03,   0x80,   0x80,   0x48,   0x78,   0x0A,   0xDA,   0xFF,   0xA3,   0x58,   0x4A,   0xA2,   0x29,
    0x17,   0x49,   0x4B,   0x00,   0x00,   0x00,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,
    0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x08,   0xE8,   0x00,   0x30,   0xF2,   0x70,   0x5A,   0x80,   0xB0,   0x58,
    0x8A,   0x00,   0xBA,   0x88,   0x21,   0x00,   0x00,   0x1E,   0x6A,   0x5E,   0x00,   0xA0,   0xA0,   0xA0,   0x29,   0x50,
    0x08,   0x40,   0x35,   0x00,   0xBA,   0x89,   0x21,   0x00,   0x00,   0x1A,   0x00,   0x00,   0x00,   0xFC,   0x00,   0x4D,
    0x54,   0x4B,   0x2D,   0x48,   0x44,   0x4D,   0x49,   0x32,   0x30,   0x3A,   0x50,   0x30,   0x00,   0x00,   0x00,   0xFD,
    0x00,   0x17,   0x3D,   0x0F,   0x88,   0x3C,   0x00,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x01,   0xAA,
    0x02,   0x03,   0x6B,   0xF0,   0x5A,   0x01,   0x03,   0x04,   0x05,   0x07,   0x10,   0x12,   0x13,   0x1F,   0x20,   0x21,
    0x22,   0x27,   0xBF,   0x40,   0x5D,   0x5E,   0x5F,   0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x76,   0x23,
    0x09,   0x7F,   0x01,   0xE2,   0x00,   0x4B,   0x6E,   0x03,   0x0C,   0x00,   0x10,   0x00,   0xF8,   0x3C,   0x2F,   0xC0,
    0x80,   0x01,   0x02,   0x03,   0x04,   0x67,   0xD8,   0x5D,   0xC4,   0x01,   0x78,   0xC0,   0x07,   0x6D,   0x1A,   0x00,
    0x00,   0x02,   0x03,   0x30,   0x90,   0x00,   0x05,   0x61,   0x37,   0x08,   0x2F,   0xE3,   0x05,   0xFF,   0x01,   0xE5,
    0x0F,   0x00,   0x80,   0xFF,   0x07,   0xE3,   0x06,   0x0F,   0x01,   0xEB,   0x01,   0x46,   0xD0,   0x00,   0x48,   0x77,
    0x72,   0x8A,   0x3C,   0x5F,   0x7C,   0xE5,   0x01,   0x8B,   0x84,   0x90,   0x01,   0xD4,   0xBC,   0x00,   0xA0,   0xA0,
    0xA0,   0x29,   0x50,   0x08,   0x40,   0x35,   0x00,   0xBA,   0x89,   0x21,   0x00,   0x00,   0x18,   0x00,   0x00,   0xA6,
#endif
};

const BYTE tEDID_TABLE_COMBO_PORT0_NO_FREESYNC[256] =
{
#if 0// H20 FHD180 , no freesync
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x8B, 0x96, 0xA2, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x1F, 0x01, 0x03, 0x80, 0x80, 0x48, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
    0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x04, 0x74, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x30, 0x20,
    0x35, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x1E, 0x06, 0xAE, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40,
    0x30, 0x20, 0x35, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x54, 0x4B, 0x2D, 0x48, 0x44, 0x4D, 0x49, 0x32, 0x30, 0x3A, 0x50, 0x30, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x30, 0xB4, 0x0F, 0x88, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x3D,
    0x02, 0x03, 0x69, 0xF0, 0x4F, 0x01, 0x03, 0x04, 0x05, 0x07, 0x10, 0x12, 0x13, 0x1F, 0x20, 0x21,
    0x22, 0x27, 0xBF, 0x40, 0x23, 0x09, 0x7F, 0x01, 0xE2, 0x00, 0x4B, 0x72, 0x03, 0x0C, 0x00, 0x10,
    0x00, 0xF8, 0x3C, 0x2F, 0xC0, 0x84, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x67, 0xD8,
    0x5D, 0xC4, 0x01, 0x78, 0xC0, 0x07, 0xE3, 0x05, 0xFF, 0x01, 0xE3, 0x0F, 0x00, 0x80, 0xE3, 0x06,
    0x0F, 0x01, 0xEB, 0x01, 0x46, 0xD0, 0x00, 0x00, 0x77, 0x72, 0x8A, 0x3C, 0x5F, 0x7C, 0xE5, 0x01,
    0x8B, 0x84, 0x90, 0x01, 0xD4, 0xBC, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12,
#elif 0// H20 FHD175 , no freesync
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x30,	0xA9,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x18,
0x02,	0x03,	0x62,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x72,	0x03,	0x0C,	0x00,	0x10,
0x00,	0xF8,	0x3C,	0x0F,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x67,	0xD8,
0x5D,	0xC4,	0x01,	0x78,	0xC0,	0x07,	0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x20,	0xE0,	0xE3,	0x06,
0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,	0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,	0x7C,	0xE5,	0x01,
0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,	0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0xE5,	0x01,	0x8B,	0x84,
0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,	0x35,	0x00,	0x09,	0x25,
0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x22,
#elif 1// H20 FHD170 , no freesync
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x20,	0x00,	0x00,	0x1D,	0x40,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x5B,	0xA4,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x2C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x87,
0x02,	0x03,	0x38,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x68,	0x03,	0x0C,	0x00,	0x10,
0x00,	0xB8,	0x44,	0x0F,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x58,	0x80,	0x03,	0xE3,	0x05,	0xFF,	0x01,
0xE3,	0x0F,	0x20,	0xE0,	0xE3,	0x06,	0x0F,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xBA,
#else //QHD 100, no freesync
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x5B,	0x9D,	0x00,	0xA0,	0xA0,	0xA0,	0x29,	0x50,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x00,	0x00,	0x00,	0x11,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x51,
0x02,	0x03,	0x57,	0xF0,	0x4E,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xC0,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x68,	0x03,	0x0C,	0x00,	0x10,	0x00,
0xF8,	0x3C,	0x0F,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,	0xC0,	0x07,	0xE3,	0x05,	0xFF,	0x01,	0xE3,
0x0F,	0x20,	0x20,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,	0x00,	0x00,	0x77,	0x72,	0x8A,
0x3C,	0x5F,	0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,	0x00,	0xA0,	0x00,	0x00,	0x00,
0x00,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,
0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x30,
#endif
};

const BYTE tEDID_TABLE_COMBO_PORT1[256] =
{
#if 0
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x36,0x74,0x30,0x00,0x01,0x00,0x00,0x00,
    0x0A,0x14,0x01,0x03,0x80,0x46,0x28,0x78,0x0A,0x0D,0xC9,0xA0,0x57,0x47,0x98,0x27,
    0x12,0x48,0x4C,0x20,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x3A,0x80,0x18,0x71,0x38,0x2D,0x40,0x58,0x2C,
    0x45,0x00,0xDF,0xA4,0x21,0x00,0x00,0x1E,0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,
    0x58,0x2C,0x25,0x00,0xDF,0xA4,0x21,0x00,0x00,0x9E,0x00,0x00,0x00,0xFC,0x00,0x4D,
    0x53,0x74,0x61,0x72,0x20,0x44,0x65,0x6D,0x6F,0x0A,0x20,0x20,0x00,0x00,0x00,0xFD,
    0x00,0x3A,0x3E,0x0F,0x46,0x0F,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xA1,
    0x02,0x03,0x34,0x71,0x51,0x94,0x13,0x05,0x03,0x04,0x02,0x01,0x16,0x15,0x07,0x06,
    0x11,0x10,0x12,0x1F,0x5E,0x5F,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,0x67,0x03,
    0x0C,0x00,0x10,0x00,0xA8,0x2D,0x6D,0x1A,0x00,0x00,0x02,0x0B,0x30,0x90,0x00,0x04,
    0x8B,0x12,0x7D,0x15,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,0x6E,0x28,0x55,0x00,
    0xDF,0xA4,0x21,0x00,0x00,0x1E,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,
    0x96,0x00,0xDF,0xA4,0x21,0x00,0x00,0x18,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,
    0x10,0x3E,0x96,0x00,0x30,0xA4,0x21,0x00,0x00,0x18,0x8C,0x0A,0xA0,0x14,0x51,0xF0,
    0x16,0x00,0x26,0x7C,0x43,0x00,0x30,0xA4,0x21,0x00,0x00,0x98,0x00,0x00,0x00,0xCE
#elif 0 // H20 FHD180
    0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
    0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
    0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
    0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
    0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x06,	0xAE,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
    0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
    0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x30,	0x00,	0x00,	0x00,	0xFD,
    0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x3D,
    0x02,	0x03,	0x66,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
    0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x6E,	0x03,	0x0C,	0x00,	0x10,
    0x00,	0xF8,	0x3C,	0x2F,	0xC0,	0x80,	0x01,	0x02,	0x03,	0x04,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,
    0xC0,	0x07,	0x6D,	0x1A,	0x00,	0x00,	0x02,	0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,
    0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x00,	0x80,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,
    0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,	0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,
    0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xD9,
#elif 0 // H20 FHD175
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x30,	0xA9,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x31,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x17,
0x02,	0x03,	0x6C,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x6E,	0x03,	0x0C,	0x00,	0x10,
0x00,	0xF8,	0x3C,	0x0F,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,
0xC0,	0x07,	0x6D,	0x1A,	0x00,	0x00,	0x02,	0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,
0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x20,	0xE0,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,
0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,	0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,
0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,
0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0xD8,
#elif 1 // H20 FHD170
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x20,	0x00,	0x00,	0x1D,	0x40,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x5B,	0xA4,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x31,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x2C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x86,
0x02,	0x03,	0x46,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x68,	0x03,	0x0C,	0x00,	0x20,
0x00,	0xB8,	0x44,	0x0F,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x58,	0x80,	0x03,	0x6D,	0x1A,	0x00,	0x00,
0x02,	0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,	0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,
0x20,	0xE0,	0xE3,	0x06,	0x0F,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x58,
#elif 1 //H20 QHD 100
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x5B,	0x9D,	0x00,	0xA0,	0xA0,	0xA0,	0x29,	0x50,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x00,	0x00,	0x00,	0x11,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x31,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x50,
0x02,	0x03,	0x65,	0xF0,	0x4E,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xC0,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x68,	0x03,	0x0C,	0x00,	0x10,	0x00,
0xF8,	0x3C,	0x0F,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,	0xC0,	0x07,	0x6D,	0x1A,	0x00,	0x00,	0x02,
0x03,	0x30,	0xB4,	0x00,	0x05,	0x61,	0x37,	0x08,	0x2F,	0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x20,
0x20,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,	0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,
0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,	0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0xE5,
0x01,	0x8B,	0x84,	0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,	0x35,
0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xDE,
#else
    0x00,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0xFF,   0x00,   0x36,   0x8B,   0x96,   0xA2,   0x01,   0x01,   0x01,   0x01,
    0x01,   0x1F,   0x01,   0x03,   0x80,   0x80,   0x48,   0x78,   0x0A,   0xDA,   0xFF,   0xA3,   0x58,   0x4A,   0xA2,   0x29,
    0x17,   0x49,   0x4B,   0x00,   0x00,   0x00,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x01,
    0x01,   0x01,   0x01,   0x01,   0x01,   0x01,   0x08,   0xE8,   0x00,   0x30,   0xF2,   0x70,   0x5A,   0x80,   0xB0,   0x58,
    0x8A,   0x00,   0xBA,   0x88,   0x21,   0x00,   0x00,   0x1E,   0x6A,   0x5E,   0x00,   0xA0,   0xA0,   0xA0,   0x29,   0x50,
    0x08,   0x40,   0x35,   0x00,   0xBA,   0x89,   0x21,   0x00,   0x00,   0x1A,   0x00,   0x00,   0x00,   0xFC,   0x00,   0x4D,
    0x54,   0x4B,   0x2D,   0x48,   0x44,   0x4D,   0x49,   0x32,   0x30,   0x3A,   0x50,   0x30,   0x00,   0x00,   0x00,   0xFD,
    0x00,   0x17,   0x3D,   0x0F,   0x88,   0x3C,   0x00,   0x0A,   0x20,   0x20,   0x20,   0x20,   0x20,   0x20,   0x01,   0xAA,
    0x02,   0x03,   0x6B,   0xF0,   0x5A,   0x01,   0x03,   0x04,   0x05,   0x07,   0x10,   0x12,   0x13,   0x1F,   0x20,   0x21,
    0x22,   0x27,   0xBF,   0x40,   0x5D,   0x5E,   0x5F,   0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x76,   0x23,
    0x09,   0x7F,   0x01,   0xE2,   0x00,   0x4B,   0x6E,   0x03,   0x0C,   0x00,   0x10,   0x00,   0xF8,   0x3C,   0x2F,   0xC0,
    0x80,   0x01,   0x02,   0x03,   0x04,   0x67,   0xD8,   0x5D,   0xC4,   0x01,   0x78,   0xC0,   0x07,   0x6D,   0x1A,   0x00,
    0x00,   0x02,   0x03,   0x30,   0x90,   0x00,   0x05,   0x61,   0x37,   0x08,   0x2F,   0xE3,   0x05,   0xFF,   0x01,   0xE5,
    0x0F,   0x00,   0x80,   0xFF,   0x07,   0xE3,   0x06,   0x0F,   0x01,   0xEB,   0x01,   0x46,   0xD0,   0x00,   0x48,   0x77,
    0x72,   0x8A,   0x3C,   0x5F,   0x7C,   0xE5,   0x01,   0x8B,   0x84,   0x90,   0x01,   0xD4,   0xBC,   0x00,   0xA0,   0xA0,
    0xA0,   0x29,   0x50,   0x08,   0x40,   0x35,   0x00,   0xBA,   0x89,   0x21,   0x00,   0x00,   0x18,   0x00,   0x00,   0xA6,
#endif
};

const BYTE tEDID_TABLE_COMBO_PORT1_NO_FREESYNC[256] =
{
#if 0// H20 FHD180 , no freesync
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x8B, 0x96, 0xA2, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x1F, 0x01, 0x03, 0x80, 0x80, 0x48, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
    0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x04, 0x74, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x30, 0x20,
    0x35, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x1E, 0x06, 0xAE, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40,
    0x30, 0x20, 0x35, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x54, 0x4B, 0x2D, 0x48, 0x44, 0x4D, 0x49, 0x32, 0x30, 0x3A, 0x50, 0x30, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x30, 0xB4, 0x0F, 0x88, 0x3C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x3D,
    0x02, 0x03, 0x69, 0xF0, 0x4F, 0x01, 0x03, 0x04, 0x05, 0x07, 0x10, 0x12, 0x13, 0x1F, 0x20, 0x21,
    0x22, 0x27, 0xBF, 0x40, 0x23, 0x09, 0x7F, 0x01, 0xE2, 0x00, 0x4B, 0x72, 0x03, 0x0C, 0x00, 0x10,
    0x00, 0xF8, 0x3C, 0x2F, 0xC0, 0x84, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x67, 0xD8,
    0x5D, 0xC4, 0x01, 0x78, 0xC0, 0x07, 0xE3, 0x05, 0xFF, 0x01, 0xE3, 0x0F, 0x00, 0x80, 0xE3, 0x06,
    0x0F, 0x01, 0xEB, 0x01, 0x46, 0xD0, 0x00, 0x00, 0x77, 0x72, 0x8A, 0x3C, 0x5F, 0x7C, 0xE5, 0x01,
    0x8B, 0x84, 0x90, 0x01, 0xD4, 0xBC, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12,
#elif 0// H20 FHD175 , no freesync
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x30,	0xA9,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x31,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x17,
0x02,	0x03,	0x62,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x72,	0x03,	0x0C,	0x00,	0x10,
0x00,	0xF8,	0x3C,	0x0F,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x67,	0xD8,
0x5D,	0xC4,	0x01,	0x78,	0xC0,	0x07,	0xE3,	0x05,	0xFF,	0x01,	0xE3,	0x0F,	0x20,	0xE0,	0xE3,	0x06,
0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,	0x00,	0x00,	0x77,	0x72,	0x8A,	0x3C,	0x5F,	0x7C,	0xE5,	0x01,
0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,	0x00,	0xA0,	0x00,	0x00,	0x00,	0x00,	0xE5,	0x01,	0x8B,	0x84,
0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,	0x35,	0x00,	0x09,	0x25,
0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x22,
#elif 1// H20 FHD170 , no freesync
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x20,	0x00,	0x00,	0x1D,	0x40,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x04,	0x74,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x5B,	0xA4,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x31,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x2C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x86,
0x02,	0x03,	0x38,	0xF0,	0x4F,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xBF,	0x40,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x68,	0x03,	0x0C,	0x00,	0x20,
0x00,	0xB8,	0x44,	0x0F,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x58,	0x80,	0x03,	0xE3,	0x05,	0xFF,	0x01,
0xE3,	0x0F,	0x20,	0xE0,	0xE3,	0x06,	0x0F,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,
0x30,	0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xAA,
#else // H20 QHD 100, no freesync
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,	0x36,	0x8B,	0x96,	0xA2,	0x01,	0x01,	0x01,	0x01,
0x01,	0x1F,	0x01,	0x03,	0x80,	0x80,	0x48,	0x78,	0x0A,	0xDA,	0xFF,	0xA3,	0x58,	0x4A,	0xA2,	0x29,
0x17,	0x49,	0x4B,	0x00,	0x00,	0x00,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x5B,	0x9D,	0x00,	0xA0,	0xA0,	0xA0,	0x29,	0x50,	0x30,	0x20,
0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1E,	0x00,	0x00,	0x00,	0x11,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x4D,
0x54,	0x4B,	0x2D,	0x48,	0x44,	0x4D,	0x49,	0x32,	0x30,	0x3A,	0x50,	0x31,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x30,	0xB4,	0x0F,	0x88,	0x3C,	0x00,	0x0A,	0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x50,
0x02,	0x03,	0x57,	0xF0,	0x4E,	0x01,	0x03,	0x04,	0x05,	0x07,	0x10,	0x12,	0x13,	0x1F,	0x20,	0x21,
0x22,	0x27,	0xC0,	0x23,	0x09,	0x7F,	0x01,	0xE2,	0x00,	0x4B,	0x68,	0x03,	0x0C,	0x00,	0x10,	0x00,
0xF8,	0x3C,	0x0F,	0x67,	0xD8,	0x5D,	0xC4,	0x01,	0x78,	0xC0,	0x07,	0xE3,	0x05,	0xFF,	0x01,	0xE3,
0x0F,	0x20,	0x20,	0xE3,	0x06,	0x0F,	0x01,	0xEB,	0x01,	0x46,	0xD0,	0x00,	0x00,	0x77,	0x72,	0x8A,
0x3C,	0x5F,	0x7C,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0xD4,	0xBC,	0x00,	0xA0,	0x00,	0x00,	0x00,
0x00,	0xE5,	0x01,	0x8B,	0x84,	0x90,	0x01,	0x38,	0x8B,	0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x30,
0x20,	0x35,	0x00,	0x09,	0x25,	0x21,	0x00,	0x00,	0x1A,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x30,
#endif
};

const BYTE tHDMI_EDID_SIZE_PORT[2] =
{
    COMBO_EDID_SIZE_PORT0,
    COMBO_EDID_SIZE_PORT1,
};

const BYTE *tCOMBO_EDID_TALBE_PORT[2] =
{
    tEDID_TABLE_COMBO_PORT0_NO_FREESYNC,
    tEDID_TABLE_COMBO_PORT1_NO_FREESYNC,
};


const BYTE *tCOMBO_FREESYNC_EDID_TALBE_PORT[2] =
{
    tEDID_TABLE_COMBO_PORT0,
    tEDID_TABLE_COMBO_PORT1,
};
#endif
#if MS_PM
extern XDATA sPM_Info  sPMInfo;
#endif
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
#if(TMDS_HDCP2_FUNCTION_SUPPORT)
#define HDCP22_DEBUG_MESSAGE_DRV    1
#if(ENABLE_DEBUG&&HDCP22_DEBUG_MESSAGE_DRV )
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
//                  mdrv_HDMIRx_SetFreeSyncEDID()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_HDMIRx_SetFreeSyncEDID(BYTE ucPortSelect,BOOL bEnableFreeSync)
{
    if(bEnableFreeSync)
    {
        mdrv_HDMIRx_LoadEDID(ucPortSelect, tCOMBO_FREESYNC_EDID_TALBE_PORT[ucPortSelect]);
    }
    else
    {
        mdrv_HDMIRx_LoadEDID(ucPortSelect, tCOMBO_EDID_TALBE_PORT[ucPortSelect]);
    }
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

    //mhal_hdmiRx_Initial();

    for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
    {
    #if HDMI_HPD_ISR_MODE
        stHDMIHPD_CTRL[enInputPortSelect].u16HPDClkLow  = 0;
        stHDMIHPD_CTRL[enInputPortSelect].u16HPDClkHigh = 0;
        stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1]  = 0;
        stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[0]  = 0;
        stHDMIHPD_CTRL[enInputPortSelect].bIsHPDProcessDone = TRUE;
    #endif
    #if ENABLE_BRINGUP_LOG
        printf("[HDMI]HDMI start init port:%d ,usInputPortEnableIndex:%d , BIT(port):%x ,GET_HDMIRX_FLAG(usInputPortEnableIndex) :%d , GET_HDMIRX_FLAG(usInputPortTypeHDMIIndex):%d \r\n",
        enInputPortSelect,stHDMIRxInfo.usInputPortEnableIndex,BIT(enInputPortSelect),GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, BIT(enInputPortSelect)),GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortTypeHDMIIndex, (BIT0 << enInputPortSelect)));
    #endif
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
            stHDMIRxInfo.stPollingInfo.u8_prev_repetition = 0;
            #ifdef SUPPORT_HDMI_HVSYNC_ALIGN
            stHDMIRxInfo.stPollingInfo.u8HVSynFSM = HDMI_HV_SYNC_P_CHECK;
            #endif

            if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortTypeHDMIIndex, (BIT0 << enInputPortSelect)))
            {
#if (HDMI_HPD_AC_ON_EARLY_PULL_LOW == 0)
                if(enInputPortSelect == 0)
                {
                    Init_hwHDCP_Hpd_Pin();
                }
                else if(enInputPortSelect == 1)
                {
                    Init_hwHDCP_Hpd_Pin2();
                }
#endif
                if(ucMode == 0)
                {
                #if HDMI_HPD_ISR_MODE
#if (HDMI_HPD_AC_ON_EARLY_PULL_LOW == 0)
                mdrv_hdmiRx_HPDControl_By_Duration(enInputPortSelect, 600);
#else
                Hal_HDMI_ClockRtermControl(enInputPortSelect,TRUE);
#endif
                #else
                    mdrv_hdmiRx_HPDControl(enInputPortSelect, FALSE);
                #endif
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
                    mdrv_hdmiRx_HPDControl_By_Duration(enInputPortSelect, 700);
#endif
                }

            #if ENABLE_BRINGUP_LOG
                printf("[HDMI] init port:%d \r\n",enInputPortSelect);
            #endif
                Hal_HDMI_init(enInputPortSelect);

            }

//#if ENABLE_HDMI_EDID_INTERNAL_DATA
//            mdrv_HDMIRx_LoadEDID(enInputPortSelect, tCOMBO_EDID_TALBE_PORT[enInputPortSelect]);
//#endif

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
#if HDMI_HPD_ISR_MODE
    return;
#else
    EN_HDMI_INPUT_PORT enInputPortSelect = HDMI_INPUT_PORT_NONE;
    ForceDelay1ms(600);
    for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, BIT(enInputPortSelect)))
        {
            mdrv_hdmiRx_ClockRtermControl(enInputPortSelect,TRUE);
            //Hal_HDMI_DataRtermControl(enInputPortSelect,TRUE);
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
#endif
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
#if ENABLE_BRINGUP_LOG
    printf("%s [HDMI][%s][%d]enInputPortSelect:%d , bPullHighFlag:%d GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex):%d time :%d %s \n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortSelect,bPullHighFlag,(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << enInputPortSelect))),MsOS_GetSystemTime(), "\033[m");
#endif
    if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << enInputPortSelect)))
    {
        if (bPullHighFlag == TRUE)
        {
#if ENABLE_HDMI_EDID_INTERNAL_DATA
            Hal_HDMI_InternalEDIDEnable(enInputPortSelect, bPullHighFlag);
#endif
            Hal_HDMI_DDCControl(enInputPortSelect, bPullHighFlag);
            Hal_HDMI_HPDControl(enInputPortSelect, bPullHighFlag);
        }
        else
        {
            Hal_HDMI_HPDControl(enInputPortSelect, bPullHighFlag);
            Hal_HDMI_DDCControl(enInputPortSelect, bPullHighFlag);
#if ENABLE_HDMI_EDID_INTERNAL_DATA
            Hal_HDMI_InternalEDIDEnable(enInputPortSelect, bPullHighFlag);
#endif
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
//                  mdrv_hdmiRx_HPDControl_By_Duration()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_HPDControl_By_Duration(BYTE enInputPortSelect, WORD uwDurationMs)
{
#if ENABLE_BRINGUP_LOG
    printf("%s [HDMI][%s][%d]enInputPortSelect:%d , uwDurationMs:%d GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex):%d time :%d %s \n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortSelect,uwDurationMs,(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << enInputPortSelect))),MsOS_GetSystemTime(), "\033[m");
#endif
    if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, (BIT0 << enInputPortSelect)))
    {
        if(uwDurationMs)
        {
            uwDurationMs = (uwDurationMs/10)*10; // align 10ms
            if(uwDurationMs<((WORD)DFT_HPDCLK_DIFF_PERIOD*2+100)) // check reasonable value for HPD CLK to have 100ms each as suggested.
                uwDurationMs = ((WORD)DFT_HPDCLK_DIFF_PERIOD*2+100);

            stHDMIHPD_CTRL[enInputPortSelect].u16HPDClkLow  = uwDurationMs - DFT_HPDCLK_DIFF_PERIOD;
            stHDMIHPD_CTRL[enInputPortSelect].u16HPDClkHigh = DFT_HPDCLK_DIFF_PERIOD;
            stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1]  = uwDurationMs;

            mdrv_hdmiRx_HPDControl(enInputPortSelect, FALSE);
            stHDMIHPD_CTRL[enInputPortSelect].bIsHPDProcessDone = FALSE;
        }
        else
        {
            stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[0] = 0;
            stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1] = 0;

            mdrv_hdmiRx_ClockRtermControl(enInputPortSelect,TRUE);
            Hal_HDMI_DataRtermControl(enInputPortSelect,TRUE);

            mdrv_hdmiRx_HPDControl(enInputPortSelect, TRUE);
        }
    }
}

void _mdrv_tmds_HDMIGetPktStatus(BYTE enInputPort __attribute__ ((unused)))
{
#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)

        //if(enInputPort >= HDMI_INFO_SOURCE_MAX)
        //return ;

        //tmds_port[enInputPort].InputPacketStatus |= pHDMIRes->ulPacketStatus[enInputPort];//((tmds_port[enInputPort].InputPacketStatus & ~(0xFF67|(BIT17)|(BIT18))))|KHal_HDMIRx_packet_info((MS_U8)enInputPort,NULL);
        //stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT2)    //SPD
        {
            stHDMIRxInfo.stPollingInfo.u8InputSPDInfoReceived = 1;
        }

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT25)   // Reserved Pkt for EMP
        {
            stHDMIRxInfo.stPollingInfo.u8InputEMPktReceived = 1;
        }

#else
        //if(enInputPort >= HDMI_INFO_SOURCE_MAX)
        //return ;

        //tmds_port[enInputPort].InputPacketStatus = pHDMIRes->ulPacketStatus[enInputPort];//KHal_HDMIRx_packet_info((MS_U8)enInputPort,NULL);
        //stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo = stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT3)
        {
            stHDMIRxInfo.stPollingInfo.u8InputAVIInfoReceived = 1;
        }

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT2)
        {
            stHDMIRxInfo.stPollingInfo.u8InputSPDInfoReceived = 1;
        }

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT7)
        {
            stHDMIRxInfo.stPollingInfo.u8InputVSPktReceived = 1;
        }

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT4)
        {
            stHDMIRxInfo.stPollingInfo.u8InputGCPktReceived = 1;
        }

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT16)
        {
            stHDMIRxInfo.stPollingInfo.u8InputHDRPktReceived = 1;
        }

        if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT25)   // Reserved Pkt for EMP
        {
            stHDMIRxInfo.stPollingInfo.u8InputEMPktReceived = 1;
        }

#endif

}

#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_HDMI_GetPktReceived_Partial()
//  [Description]
//                  use to update HDMI received status with AVI/GCP/VS/HDR packets.
//  [Arguments]:
//                  enInputPort
//  [Return]:
//                  none. Updated packet received and status flag.
//
//**************************************************************************
void mdrv_tmds_HDMI_GetPktReceived_Partial(BYTE enInputPort __attribute__ ((unused)))
{
    // Note. need modify "mhal_tmds_HDMIGetPktStatus_Partial(MSCombo_TMDS_PORT_INDEX enInputPort)"

    //if(enInputPort >= HDMI_INFO_SOURCE_MAX)
    //return ;

    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo =  (stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & ~(BIT3|BIT4|BIT7|BIT16))|Hal_HDMI_packet_info_partial(ucCurrPort,NULL);
    //pHDMIRes->ulPacketStatus[enInputPort] = pHDMIRes->ulPacketStatus[enInputPort] & ~(BIT3|BIT4|BIT7|BIT16);


    if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT3)
    {
        stHDMIRxInfo.stPollingInfo.u8InputAVIInfoReceived = 1;

        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt = 0;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt < AVI_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputAVIInfoReceived = 1;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT3;
        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_AVI_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt++;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt == AVI_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputAVIInfoReceived = 0;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT3);
        //!!TMP!!
        //stHDMIRxInfo.stPollingInfo.input_color.ucColorType = COMBO_COLOR_FORMAT_RGB;
        //stHDMIRxInfo.stPollingInfo.ScalingInfo = 0;
        //stHDMIRxInfo.stPollingInfo.ITContent = 0;
        //!!TMP!!
    }

    if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT4)
    {
        stHDMIRxInfo.stPollingInfo.u8InputGCPktReceived = 1;
        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt = 0;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt < GCP_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputGCPktReceived = 1;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT4;
        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_GCP_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt++;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt == GCP_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputGCPktReceived = 0;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT4);
        //!!TMP!!
        //tmds_port[enInputPort].ClrAVMute = TRUE;
        //!!TMP!!
    }

    if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT7)
    {
        stHDMIRxInfo.stPollingInfo.u8InputVSPktReceived = 1;
        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_VS_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt = 0;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt < VS_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputVSPktReceived = 1;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT7;
        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_VS_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt++;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt == VS_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputVSPktReceived = 0;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT7);
    }

    if( stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo & BIT16)
    {
        stHDMIRxInfo.stPollingInfo.u8InputHDRPktReceived = 1;
        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt = 0;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt < HDR_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputHDRPktReceived = 1;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT16;
        //pHDMIRes->ulPacketStatus[enInputPort] |= HDMI_STATUS_HDR_PACKET_RECEIVE_FLAG;
        stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt++;
    }
    else if(stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt == HDR_PKT_LOSS_BOUND)
    {
        stHDMIRxInfo.stPollingInfo.u8InputHDRPktReceived = 0;
        stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT16);
    }
}
#endif
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

            #if 0   // patch for chroma 2238
                    if(stHDMIRxInfo.stPollingInfo.ucHDCPState == HDMI_HDCP_NO_ENCRYPTION)
                    {
                        bIsSignalEncrypt[ucCurrPort] = FALSE;
                    }
                    else
                    {
                        bIsSignalEncrypt[ucCurrPort] = TRUE;
                    }
            #endif
                    if (stHDMIRxInfo.stPollingInfo.bHDMIModeFlag) //hdmi mode
                    {
                        if(bCheckPacketReceiveFlag)
                        {
#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
                            stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= Hal_HDMI_packet_info(ucCurrPort, NULL);

#else
                            stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo = Hal_HDMI_packet_info(ucCurrPort, NULL);
#endif

                            stHDMIRxInfo.stPollingInfo.InputPacketError= mdrv_hdmiRx_GetVideoContentInfo(E_HDMI_GET_ERROR_STATUS,ucCurrPort);


                            _mdrv_tmds_HDMIGetPktStatus(ucCurrPort); //updated pkt received.

#if (!COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
                            if (stHDMIRxInfo.stPollingInfo.u8InputAVIInfoReceived)
                            {
                                stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt = 0;

                                //tmds_port[enInputPort].input_color = (ST_COMBO_COLOR_FORMAT)mdrv_tmds_GetColor(enInputPort);

                                //if(tmds_port[enInputPort].bForceColor)
                                {
                                    //tmds_port[enInputPort].input_color.ucColorType = tmds_port[enInputPort].enColorType;
                                }

                                //tmds_port[enInputPort].ScalingInfo = mdrv_tmds_GetScaling_Info(enInputPort);
                                //tmds_port[enInputPort].ITContent = mdrv_tmds_GetITContent(enInputPort);

                                //TO DO
                                //mhal_tmds_CheckSplitter(enInputPort, tmds_port[enInputPort].input_color.ucColorType, &(tmds_port[enInputPort].stTMDSInfo), TRUE);
                            }
                            else
                            {
                                //if(stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt >= AviPktLossBound) //temp
                                {
                                    //stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT3);

                                    //tmds_port[enInputPort].input_color.ucColorType = COMBO_COLOR_FORMAT_RGB;
                                    //tmds_port[enInputPort].ScalingInfo = 0;
                                    //tmds_port[enInputPort].ITContent = 0;
                                }
                                //else

                                if(stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt < AVI_PKT_LOSS_BOUND)

                                {
                                    stHDMIRxInfo.stPollingInfo.u8InputAVIInfoReceived = 1;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT3;
                                    //tmds_port[enInputPort].input_color = (ST_COMBO_COLOR_FORMAT)mdrv_tmds_GetColor(enInputPort);

                                    //if(tmds_port[enInputPort].bForceColor)
                                    {
                                        //tmds_port[enInputPort].input_color.ucColorType = tmds_port[enInputPort].enColorType;
                                    }

                                    //tmds_port[enInputPort].ScalingInfo = mdrv_tmds_GetScaling_Info(enInputPort);
                                    //tmds_port[enInputPort].ITContent = mdrv_tmds_GetITContent(enInputPort);
                                }
                                else if(stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt >= AVI_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt = 0;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT3);
                                    //!!TMP!!
                                    //stHDMIRxInfo.stPollingInfo.input_color.ucColorType = COMBO_COLOR_FORMAT_RGB;
                                    //stHDMIRxInfo.stPollingInfo.ScalingInfo = 0;
                                    //stHDMIRxInfo.stPollingInfo.ITContent = 0;
                                    //!!TMP!!
                                }
                            }

                            if (stHDMIRxInfo.stPollingInfo.u8InputGCPktReceived)
                            {
                                stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt = 0;
                            }
                            else
                            {
                                if(stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt < GCP_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u8InputGCPktReceived = 1;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT4;

                                }
                                else if(stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt >= GCP_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt = 0;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT4);
                                    Hal_HDMI_pkt_reset(ucCurrPort, REST_AVMUTE, &stHDMIRxInfo.stPollingInfo,FALSE);
                                }
                            }

                            if (stHDMIRxInfo.stPollingInfo.u8InputVSPktReceived)
                            {
                                stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt = 0;
                            }
                            else
                            {
                                if(stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt < VS_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u8InputVSPktReceived = 1;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT7;
                                }
                                else if(stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt >= VS_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt = 0;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT7);
                                }
                            }

                            if (stHDMIRxInfo.stPollingInfo.u8InputHDRPktReceived)
                            {
                                stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt = 0;
                            }
                            else
                            {
                                if(stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt < HDR_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u8InputHDRPktReceived = 1;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo |= BIT16;
                                }
                                else if(stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt >= HDR_PKT_LOSS_BOUND)
                                {
                                    stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt = 0;
                                    stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo &= (~BIT16);
                                }
                            }
#endif
                        }
                    }
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
                if(stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt <= AVI_PKT_LOSS_BOUND)
                {
                    stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt++;
                }

                if(stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt <= GCP_PKT_LOSS_BOUND)
                {
                    stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt++;
                }

                if(stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt <= VS_PKT_LOSS_BOUND)
                {
                    stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt++;
                }

                if(stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt <= HDR_PKT_LOSS_BOUND)
                {
                    stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt++;
                }

            }
            else
            {
                stHDMIRxInfo.stPollingInfo.u16AVI_PktLossCnt = 0;
                stHDMIRxInfo.stPollingInfo.u16GC_PktLossCnt = 0;
                stHDMIRxInfo.stPollingInfo.u16VS_PktLossCnt = 0;
                stHDMIRxInfo.stPollingInfo.u16HDR_PktLossCnt = 0;
            }
        }
    }
#if HDMI_HPD_ISR_MODE
    EN_HDMI_INPUT_PORT enInputPortSelect = HDMI_INPUT_PORT_NONE;
    for(enInputPortSelect = HDMI_INPUT_PORT0; enInputPortSelect < HDMI_INPUT_PORT_END; enInputPortSelect++)
    {
        if(GET_HDMIRX_FLAG(stHDMIRxInfo.usInputPortEnableIndex, BIT(enInputPortSelect)))
        {
            if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[0])
            {
                if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[0] > 1)
                    stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[0] -= 1;
                else
                    stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[0] = 0;

                if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[0]==0x00)
                {
                    mdrv_hdmiRx_HPDControl(enInputPortSelect, FALSE);
                }
            }
            if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1])
            {
                if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1] > 1)
                    stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1] -= 1;
                else
                    stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1] = 0;


                if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1]==stHDMIHPD_CTRL[enInputPortSelect].u16HPDClkLow)
                {
                    mdrv_hdmiRx_ClockRtermControl(enInputPortSelect,FALSE);
                    Hal_HDMI_DataRtermControl(enInputPortSelect,FALSE);
                }
                else if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1]==stHDMIHPD_CTRL[enInputPortSelect].u16HPDClkHigh)
                {
                    mdrv_hdmiRx_ClockRtermControl(enInputPortSelect,TRUE);
                    Hal_HDMI_DataRtermControl(enInputPortSelect,TRUE);
                }
                else if(stHDMIHPD_CTRL[enInputPortSelect].u16HPDCnt[1]==0x00)
                {
                    stHDMIHPD_CTRL[enInputPortSelect].bIsHPDProcessDone = TRUE;
                    mdrv_hdmiRx_HPDControl(enInputPortSelect, TRUE);
                }
            }
        }
    }
#endif
}

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
#if (HDMI_HPD_ISR_MODE == DISABLE)
    BOOL bFlag = 0;
#endif
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]ucPortMapping:%d  %s\n", "\033[0;32;31m", __FUNCTION__, __LINE__,ucPortMapping, "\033[m");
#endif
    if(ucPortMapping < HDMI_INPUT_PORT_END)
    {
    #if HDMI_HPD_ISR_MODE
        if(Hal_HDMI_SCDC_status(ucPortMapping)/*||bIsSignalEncrypt[ucPortMapping]*/)
        {
            mdrv_hdmiRx_HPDControl_By_Duration(ucPortMapping, 700);
        }
        else
        {
            if(stHDMIHPD_CTRL[ucPortMapping].bIsHPDProcessDone == TRUE)
            {
                Hal_HDMI_DataRtermControl(ucPortMapping,TRUE);
            }
        }
        Hal_tmds_Switch2HDMI(ucPortMapping);
    #else
        if(Hal_HDMI_SCDC_status(ucPortMapping)/*||bIsSignalEncrypt[ucPortMapping]*/)
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
    #endif
        ucCurrPort = ucPortMapping;
    }
    else    // active port is not HDMI
    {
        Hal_HDMI_SCDC_Clr(HDMI_INPUT_PORT0);
        Hal_HDMI_SCDC_Clr(HDMI_INPUT_PORT1);
        Hal_HDMI_MAC_HDCP_Enable(FALSE);
        Hal_HDMI_SCDC_config(HDMI_INPUT_PORT0, 1);
        Hal_HDMI_SCDC_config(HDMI_INPUT_PORT1, 1);
        ucCurrPort = ucPortMapping;
    }
}
#else
void mdrv_hdmi_SetupInputPort(BYTE ucPortSelect)
{
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(ucPortSelect);
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]ucPortMapping:%d %s\n", "\033[0;32;31m", __FUNCTION__, __LINE__,ucPortMapping, "\033[m");
#endif
    Hal_tmds_Switch2HDMI(ucPortMapping);

    ucCurrPort = ucPortMapping;
}
#endif
//**************************************************************************
//	[Function Name]:
//					mdrv_hdmiRx_PMSwitchDVIDetect()
//	[Description]
//
//	[Arguments]:
//
//	[Return]:
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
                //[MT9701]pmdrv_tmds_HDCP2RxEventProc((BYTE)enInputPort, ucHDCPRxData);
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
                bSwitchContentFlag = FALSE;

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

                pPacketData[HDMI_MULTI_VS_PACKET_LENGTH] = Hal_HDMI_GetDataInfo(E_HDMI_GET_MULTIVS_COUNT, ucPortMapping , &stHDMIRxInfo.stPollingInfo);

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
//                  mdrv_tmds_GetPixelClockHz()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DWORD mdrv_tmds_GetPixelClockHz(BYTE enInputPort, EN_HDMI_PIX_CLK_TYPE enType)
{
    DWORD wTMDSClkValCnt;
    MS_U8 enColorDepth = HDMI_COLOR_DEPTH_8BIT;
    MSCombo_TMDS_PIXEL_REPETITION enPixelRepetition = MSCombo_TMDS_N0_PIX_REP;
    EN_COLOR_FORMAT_TYPE enColorType = COMBO_COLOR_FORMAT_RGB;

    wTMDSClkValCnt = Hal_tmds_GetClockRatePort(enInputPort, stHDMIRxInfo.stPollingInfo.ucSourceVersion, enType);

    if(stHDMIRxInfo.stPollingInfo.bHDMIModeFlag)//mhal_tmds_GetInputType((MSCombo_TMDS_PORT_INDEX)enInputPort)
    {
        enColorDepth = Hal_HDMI_GetGCPColorDepth(enInputPort, &stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo);
        enPixelRepetition = KHal_HDMIRx_GetPixelRepetitionInfo(enInputPort);
        enColorType = Hal_HDMI_GetColorFormat(enInputPort);

        // check HDMI2.0 timing
        if(stHDMIRxInfo.stPollingInfo.ucSourceVersion >= HDMI_SOURCE_VERSION_HDMI20)//mhal_tmds_InputIsHDMI2((MSCombo_TMDS_PORT_INDEX)enInputPort))
        {
            wTMDSClkValCnt = wTMDSClkValCnt * 4;
        }

        // check color format YUV420
        if(enColorType == COMBO_COLOR_FORMAT_YUV_420)
        {
            wTMDSClkValCnt = wTMDSClkValCnt * 2;
        }

        // check color depth
        switch (enColorDepth)
        {
            case HDMI_COLOR_DEPTH_8BIT:
                wTMDSClkValCnt = wTMDSClkValCnt;
                break;

            case HDMI_COLOR_DEPTH_10BIT:
                wTMDSClkValCnt = wTMDSClkValCnt * 8 / 10;
                break;

            case HDMI_COLOR_DEPTH_12BIT:
                wTMDSClkValCnt = wTMDSClkValCnt * 8 / 12;
                break;

            case HDMI_COLOR_DEPTH_16BIT:
                wTMDSClkValCnt = wTMDSClkValCnt * 8 / 16;
                break;

            default:
                break;
        }

        // check pixel repetition
        wTMDSClkValCnt = wTMDSClkValCnt / ((BYTE)enPixelRepetition + 1);

    }
    return wTMDSClkValCnt;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_RxInfo_Get()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mdrv_hdmiRx_RxInfo_Get(BYTE enInputPort, ST_COMBO_RX_INFO_UNION *pRxInfoUnion, EN_COMBO_RX_INFO_SELECT enInfo)
{
    BOOL bValid = FALSE;

    if(pRxInfoUnion == NULL)
        return bValid;

    switch(enInfo) {
    case COMBO_RX_INFO_VAR_PIXEL_CLOCK_10KHZ:
        {
            pRxInfoUnion->ulValue = mdrv_tmds_GetPixelClockHz(enInputPort, HDMI_SIGNAL_PIX_10KHZ);
            bValid = TRUE;
        }
        break;
    default:
        pRxInfoUnion->ulValue = 0;
        break;
    }

    return bValid;
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
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_HDE, ucInputPort , &stHDMIRxInfo.stPollingInfo);
            break;

        case HDMI_VIDEO_CONTENT_HTT:
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_HTT, ucInputPort , &stHDMIRxInfo.stPollingInfo);
            break;

        case HDMI_VIDEO_CONTENT_VDE:
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_VDE, ucInputPort , &stHDMIRxInfo.stPollingInfo);
            break;

        case HDMI_VIDEO_CONTENT_VTT:
            usVideoContent = Hal_HDMI_GetDataInfo(E_HDMI_GET_VTT, ucInputPort , &stHDMIRxInfo.stPollingInfo);
            break;

        case HDMI_VIDEO_CLK_STABLE:
            usVideoContent =  stHDMIRxInfo.stPollingInfo.bClockStableFlag;
            break;

        case HDMI_VIDEO_CLK_COUNT:
            usVideoContent =  mdrv_tmds_GetPixelClockHz(ucInputPort, HDMI_SIGNAL_PIX_MHZ);
            break;

        case HDMI_VIDEO_DE_STABLE:
            //usVideoContent =  stHDMIRxInfo.stPollingInfo.bDEStableFlag;
            usVideoContent =  Hal_HDMI_GetDEStableStatus(ucInputPort);
            break;

        case HDMI_VIDEO_FREE_SYNC_FLAG:
            if (Hal_HDMI_GetFreeSyncInfo(ucInputPort, &stHDMIRxInfo.stPollingInfo.ulPacketStatusInfo, NULL)& HDMI_FREE_SYNC_SUPPORTED) //avi packet received;
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
                usVideoContent = 0x01;//hdmi mode
            }
            else
            {
                usVideoContent = 0x00;//dvi mode
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
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]enInputPortSelect:%d , bPullHighFlag:%d %s  \n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortSelect,bPullHighFlag, "\033[m");
#endif
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
#if ENABLE_BRINGUP_LOG
    printf("%s[HDMI][%s][%d]enInputPortSelect:%d , bPullHighFlag:%d %s  \n", "\033[0;32;31m", __FUNCTION__, __LINE__,enInputPortSelect,bPullHighFlag, "\033[m");
#endif
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
//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_PHY_ISRHandler()
//  [Description]:
//  [Arguments]:
//  [Return]:
//                  none
//
//**************************************************************************
void mdrv_tmds_PHY_ISRHandler(void)
{
    BYTE enInputPort = HDMI_INPUT_PORT0;

    //for(enInputPort = HDMI_INPUT_PORT0; enInputPort <= HDMI_INPUT_PORT1; enInputPort++)
    {
        if(KHAL_HDMIRx_PHY_GetInterruptFlag())
        {
            KHAL_HDMIRx_PHY_ISRHandler(enInputPort);
        }
    }
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_TimingChgISR()
//  [Description]:
//  [Arguments]:
//  [Return]:
//                  none
//
//**************************************************************************
void mdrv_tmds_TimingChgISR(void)
{
    BYTE enInputPort = HDMI_INPUT_PORT0;

    KHal_HDMIRx_TimingChgISR(enInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_SetTimingChgCallBackFunction()
//  [Description]:
//  [Arguments]:
//  [Return]:
//                  none
//
//**************************************************************************
void mdrv_tmds_SetTimingChgCallBackFunction(TimingChg_CALL_BACK_FUNC pFunc)
{
    KHal_HDMIRx_Set_DE_UNSTABLE_ISR_CallBackFunction(pFunc);
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_SetSCDCValue()
//  [Description]:
//  [Arguments]:
//  [Return]:
//                  none
//
//**************************************************************************
void mdrv_hdmiRx_SetSCDCValue(BYTE enInputPort, BYTE u8Offset, BYTE u8Size, BYTE *u8SetValue)
{
    BYTE u8Count = 0;

    if((u8Size == 0)||(u8SetValue == NULL))
    {
        //printf("%s [%s]input value incorrect %s\n", "\033[0;32;31m", __FUNCTION__, "\033[m");
        return;
    }

    for(u8Count = 0 ; u8Count < u8Size ; u8Count++)
    {
        KHal_HDMIRx_SetSCDCValue(enInputPort, u8Offset+u8Count, u8SetValue[u8Count]);
    }
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
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(enInputPortType);
    Hal_HDMI_SCDC_Clr(ucPortMapping);
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
    BYTE ucPortMapping = mdrv_hdmiRx_InputPortMapping(enInputPortSelect);
    Hal_HDMI_SCDC_config(ucPortMapping, bDDC);
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

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_IRQ_Enable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_IRQ_Enable(EN_KDRV_HDMIRX_INT e_int, BYTE enInputPortSelect, MS_U16 bit_msk, Bool bEnableIRQ)
{
    Hal_HDMI_IRQ_OnOff(e_int, enInputPortSelect, bit_msk, bEnableIRQ);
}

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
    if(PMMode == HDMI_PM_POWERON)
    {
        Hal_tmds_SetMacPowerDown(0, FALSE);
    }
    else if(PMMode == HDMI_PM_POWEROFF)
    {
        Hal_tmds_SetMacPowerDown(0, TRUE);
    }
    else//(PMMode == HDMI_PM_STANDBY)
    {
        Hal_tmds_SetMacPowerDown(0, TRUE);
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
    if(PMMode == HDMI_PM_POWERON)
    {
        Hal_tmds_SetMacPowerDown(0, FALSE);
    }
    else if(PMMode == HDMI_PM_POWEROFF)
    {
        Hal_tmds_SetMacPowerDown(0, TRUE);
    }
    else//(PMMode == HDMI_PM_STANDBY)
    {
        Hal_tmds_SetMacPowerDown(0, TRUE);
    }
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
//
//**************************************************************************
BYTE mdrv_tmds_HDMIGetErrorStatus(BYTE enInputPortType __attribute__ ((unused)))
{
    return stHDMIRxInfo.stPollingInfo.InputPacketError;
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_SCDC_ISRHandler()
//  [Description]:
//  [Arguments]:
//  [Return]:
//                  none
//
//**************************************************************************
void mdrv_tmds_SCDC_ISRHandler(void)
{
    BYTE enInputPort = HDMI_INPUT_PORT0;

    for(enInputPort = HDMI_INPUT_PORT0; enInputPort <= HDMI_INPUT_PORT1; enInputPort++)
    {
        if(KHAL_HDMIRx_SCDC_GetInterruptFlag())
        {
            KHAL_HDMIRx_SCDC_ISRHandler(enInputPort);
        }
    }
}

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
        HDCPHandler_AttachCBFunc(ucPortSelect, pHDCP2TxFunc);
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
        if(HDCPHandler_GetR2EventFlag()||  HDCPHandler_UpdateRomR2FirstEventFlag())//[MT9701]
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

#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
//**************************************************************************
//  [Function Name]:
//                  mdrv_tmds_HDMIGetErrorStatus_Partial()
//  [Description]:
//                  use to return HDMI error status
//  [Arguments]:
//                  enInputPort : combo port
//  [Return]:
//                  Checksum error status
//**************************************************************************
BYTE mdrv_tmds_HDMIGetErrorStatus_Partial(BYTE enInputPort)
{
    return mhal_tmds_HDMIGetErrorStatus_Partial(enInputPort);
}
#endif

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
//                  mdrv_hdmiRx_DTOPDEC_IRQ_Mask()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_DTOPDEC_IRQ_Mask(void)
{
    mhal_hdmi_DTOPDEC_IRQ_Mask();
}

//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_DTOPDEC_IRQ_Unmask()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_DTOPDEC_IRQ_Unmask(void)
{
    mhal_hdmi_DTOPDEC_IRQ_Unmask();
}
//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_GetAudioFrequency()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
TMDS_AUDIO_FREQUENCY_INDEX mdrv_hdmiRx_GetAudioFrequency(BYTE enInputPortType)
{

#if ENABLE_HDMI && (CHIP_ID == CHIP_MT9701)
    return Hal_HDMI_GetAudioFrequency(enInputPortType, &stHDMIRxInfo.stPollingInfo);
#else
    UNUSED(enInputPortType);
    return HDMI_AUDIO_FREQUENCY_UNVALID;
#endif

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
    Detect5V = KHal_HDMIRx_GetSCDC_Tx5V_Pwr_DetectFlag(ucInputPort);

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
    bSwapDone = Hal_HDMIRx_Set_RB_PN_Swap(ucPortIndex,enHDMI_SWAP_TYPE,bSwapEnable);

    return bSwapDone ;
}

#endif // MDRV_HDMIRX_C

