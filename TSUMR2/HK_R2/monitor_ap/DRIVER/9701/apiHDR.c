#include "Global.h"
//#if ENABLE_HDR
#include "apiHDR.h"
#include "drvHDR.h"
#include <string.h>
#include "drvMcu.h"
#include "msFB.h"
#include "ComboApp.h"
#include "AutoGamma.h"
#include "msflash.h"
#include "NVRam.h"
#include "drvIMI.h"
#include "drvLutAutoDownload.h"
#include "drvHDRCommon.h"
#include "drvHDRLUT.h"

extern void SetHDRColorFormat(ST_COMBO_COLOR_FORMAT cf);
extern ST_COMBO_COLOR_FORMAT GetHDRColorFormat(void);

//#define HDR_PrintMsg(str)
//#define HDR_PrintData(str, x)

#define HDR_AUTODOWNLOAD_PATCH 0
#define HDR_3DLUT 0
#define HDR_NEEDTOHELP 1

#define AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR       512
#define XYVCC_ADL_DEGAMMA_600E 1 //wait memary allocate enought size.
#define X2D_MAP_SIZE 0x1000 //mapping size of xdata 2 dram
#ifndef UNUSED //to avoid compile warnings...
#define UNUSED(var) (void)((var)=(var))
#endif
//xdata DWORD _buffer = 0;
xdata WORD _satGain = 0x1000;
xdata DWORD _hk_high_th = 0x20000;

xdata WORD reg_b02_y2r_coef02 = 0x0065;
xdata WORD reg_b02_y2r_coef11 = 0x01F4;
xdata WORD reg_b02_y2r_coef12 = 0x01E2;
xdata WORD reg_b02_y2r_coef21 = 0x0077;
xdata DWORD reg_b02_y2r_off0 = 0x00006500;
xdata DWORD reg_b02_y2r_off1 = 0x000FD600;
xdata DWORD reg_b02_y2r_off2 = 0x00007700;
xdata BYTE _r2yEn = 0xFF;

#define _PK_L_(bank, addr)   (0x902F00 |((MS_U16)bank << 24) | (MS_U16)(addr*2))
#define _PK_H_(bank, addr)   (0x902F00 |((MS_U16)bank << 24) | (MS_U16)(addr*2+1))

typedef enum
{
    HDR_DeGammaTbl,
    HDR_GammaTbl,
    HDR_ToneMappingTbl,
    HDR_3DLutTbl,
    HDR_OOTF,
}HDR_DM_Types;

code float _bypassMatrix[9] =
{
    1,	0, 0,
    0,	1, 0,
    0, 0, 1,
};

code float _limitToFull_Matrix[9] =
{
    1.168,	0, 0,
    0,1.168, 0,
    0, 0, 1.168,
};


code float _y2rBT601_LimitRange[9] =
{
    1.6007,	1.168, 0,
    -0.816,	1.168, -0.3929,
    0	      , 1.168, 2.0235,
};

code float _y2rBT709_LimitRange[9] =
{
    1.7979  , 1.1680, 0.0000,
    -0.5342, 1.1680, -0.2139,
    0.0000  , 1.1680, 2.1182,
};

code float _y2rBT2020NCL_LimitRange[9] =
{
    1.6836  , 1.1680, 0.0000 ,
    -0.6523, 1.1680, -0.1875,
    0.0000  , 1.1680, 2.1484,
};

code float _y2rBT2020CL_LimitRange[9] =
{
    1.6836  , 1.1680, 0.0000 ,
    -0.6523, 1.1680, -0.1875,
    0.0000  , 1.1680, 2.1484,
};

code float _y2rBT601_FullRange[9] =
{
    1.4023  , 1.0000, 0.0000,
    -0.7139, 1.0000, -0.3438,
    0.0000  , 1.0000, 1.7725,
};

code float _y2rBT709_FullRange[9] =
{
    1.5752  , 1.0000, 0.0000,
    -0.4678, 1.0000, -0.1875 ,
    0.0000  , 1.0000, 1.8555 ,
};

code float _y2rBT2020NCL_FullRange[9] =
{
    1.4746  , 1.0000, 0.0000,
    -0.5713, 1.0000, -0.1641,
    0.0000  , 1.0000, 1.8818,
};

code float _y2rBT2020CL_FullRange[9] =
{
    1.4746  , 1.0000, 0.0000,
    -0.5713, 1.0000, -0.1641,
    0.0000  , 1.0000, 1.8818,
};

//------------------------------------------------------
//Full range RGB color space  transfer to various kind of YCC color space.
code float _r2yBT601_LimitRange[9] =
{
    0.4375  , -0.3672, -0.0713,
    0.2559  , 0.5029  , 0.0977,
    -0.1475, -0.2900, 0.4375,
};

code float _r2yBT709_LimitRange[9] =
{
    0.4375  , -0.3975, -0.0400,
    0.1816  , 0.6123  , 0.0615,
    -0.1006, -0.3379, 0.4375,
};

code float _r2yBT2020NCL_LimitRange[9] =
{
    0.4375  , -0.4023, -0.0352,
    0.2246  , 0.5811  , 0.0508,
    -0.1221, -0.3154, 0.4375,
};

code float  _r2yBT601_FullRange[9] =
{
    0.5000  , -0.4189, -0.0811,
    0.2988  , 0.5869  , 0.1143,
    -0.1689, -0.3311, 0.5000,
};

code float _r2yBT709_FullRange[9] =
{
    0.5000  , -0.4541, -0.0459,
    0.2129  , 0.7148  , 0.0723,
    -0.1143, -0.3857, 0.5000,
};

code float _r2yBT2020NCL_FullRange[9] =
{
    0.5000, -0.4600, -0.0400,
    0.2627, 0.6777, 0.0596,
    -0.1396, -0.3604, 0.5000,
};

code DWORD _ycbcrOffset[8][3] = {{0x00000000, 0x00000000, 0x00000000}, //bypass
                                                        {0x064CA000, 0xFD60E000, 0x076C2000}, //HDTV full
                                                        {0x07C69800, 0xFD973C00, 0x090EF800}, //HDTV limit
                                                        {0x059BA000, 0xFBC46000, 0x07168000}, //SDTV full
                                                        {0x06FC9800, 0xFBC05C00, 0x08AD3800},//SDTV limit
                                                        {0x05E60000, 0xFD0E8000, 0x07868000},//BT2020NCL full
                                                        {0x07517800, 0xFD391C00, 0x092D1800},//BT2020NCL limit
                                                        {0x00957C00, 0x00957C00, 0x00957C00},//BT2020NCL limit
                                                        };//BT2020NCL limit

DWORD GetHDRBaseAddr(BYTE u8WinIdx)
{
    if (u8WinIdx == 0)//MainWindow
    {
        return 0;
    }
    else//SubWindow
    {
        //Greg : Not support;
        return 0;
    }
}

DWORD GetADLBaseAddr(BYTE u8WinIdx)
{
    if (u8WinIdx == 0)//MainWindow
    {
        return 0;
    }
    else//SubWindow
    {
         //Greg : Not support;
        return 0;
    }
}


#if 1//XVYCC_AUTODOWNLOAD
code BYTE _SRGBDeGamma[] =
{
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
 0x20, 0x00, 0x30, 0x00, 0x30, 0x00, 0x40, 0x00, 0x50, 0x00, 0x60, 0x00, 0x70, 0x00, 0x80, 0x00,
 0x90, 0x00, 0xB0, 0x00, 0xC0, 0x00, 0xD0, 0x00, 0xF0, 0x00, 0x10, 0x01, 0x30, 0x01, 0x50, 0x01,
 0x70, 0x01, 0x90, 0x01, 0xB0, 0x01, 0xD0, 0x01, 0x00, 0x02, 0x20, 0x02, 0x50, 0x02, 0x80, 0x02,
 0xA0, 0x02, 0xD0, 0x02, 0x00, 0x03, 0x40, 0x03, 0x70, 0x03, 0xA0, 0x03, 0xE0, 0x03, 0x20, 0x04,
 0x50, 0x04, 0x90, 0x04, 0xD0, 0x04, 0x10, 0x05, 0x50, 0x05, 0xA0, 0x05, 0xE0, 0x05, 0x30, 0x06,
 0x80, 0x06, 0xC0, 0x06, 0x10, 0x07, 0x60, 0x07, 0xB0, 0x07, 0x10, 0x08, 0x60, 0x08, 0xC0, 0x08,
 0x10, 0x09, 0x70, 0x09, 0xD0, 0x09, 0x30, 0x0A, 0x90, 0x0A, 0xF0, 0x0A, 0x60, 0x0B, 0xC0, 0x0B,
 0x30, 0x0C, 0xA0, 0x0C, 0x10, 0x0D, 0x80, 0x0D, 0xF0, 0x0D, 0x60, 0x0E, 0xD0, 0x0E, 0x50, 0x0F,
 0xD0, 0x0F, 0x40, 0x10, 0xC0, 0x10, 0x40, 0x11, 0xC0, 0x11, 0x50, 0x12, 0xD0, 0x12, 0x60, 0x13,
 0xE0, 0x13, 0x70, 0x14, 0x00, 0x15, 0x90, 0x15, 0x30, 0x16, 0xC0, 0x16, 0x50, 0x17, 0xF0, 0x17,
 0x90, 0x18, 0x30, 0x19, 0xD0, 0x19, 0x70, 0x1A, 0x10, 0x1B, 0xC0, 0x1B, 0x60, 0x1C, 0x10, 0x1D,
 0xC0, 0x1D, 0x70, 0x1E, 0x20, 0x1F, 0xD0, 0x1F, 0x80, 0x20, 0x40, 0x21, 0xF0, 0x21, 0xB0, 0x22,
 0x70, 0x23, 0x30, 0x24, 0xF0, 0x24, 0xC0, 0x25, 0x80, 0x26, 0x50, 0x27, 0x20, 0x28, 0xF0, 0x28,
 0xC0, 0x29, 0x90, 0x2A, 0x60, 0x2B, 0x40, 0x2C, 0x10, 0x2D, 0xF0, 0x2D, 0xD0, 0x2E, 0xB0, 0x2F,
 0x90, 0x30, 0x70, 0x31, 0x60, 0x32, 0x40, 0x33, 0x30, 0x34, 0x20, 0x35, 0x10, 0x36, 0x00, 0x37,
 0x00, 0x38, 0xF0, 0x38, 0xF0, 0x39, 0xE0, 0x3A, 0xE0, 0x3B, 0xE0, 0x3C, 0xF0, 0x3D, 0xF0, 0x3E,
 0xF0, 0x3F, 0x00, 0x41, 0x10, 0x42, 0x20, 0x43, 0x30, 0x44, 0x40, 0x45, 0x50, 0x46, 0x70, 0x47,
 0x90, 0x48, 0xA0, 0x49, 0xC0, 0x4A, 0xE0, 0x4B, 0x10, 0x4D, 0x30, 0x4E, 0x60, 0x4F, 0x80, 0x50,
 0xB0, 0x51, 0xE0, 0x52, 0x10, 0x54, 0x50, 0x55, 0x80, 0x56, 0xC0, 0x57, 0xF0, 0x58, 0x30, 0x5A,
 0x70, 0x5B, 0xC0, 0x5C, 0x00, 0x5E, 0x40, 0x5F, 0x90, 0x60, 0xE0, 0x61, 0x30, 0x63, 0x80, 0x64,
 0xD0, 0x65, 0x30, 0x67, 0x80, 0x68, 0xE0, 0x69, 0x40, 0x6B, 0xA0, 0x6C, 0x00, 0x6E, 0x60, 0x6F,
 0xD0, 0x70, 0x30, 0x72, 0xA0, 0x73, 0x10, 0x75, 0x80, 0x76, 0xF0, 0x77, 0x70, 0x79, 0xE0, 0x7A,
 0x60, 0x7C, 0xE0, 0x7D, 0x60, 0x7F, 0xE0, 0x80, 0x70, 0x82, 0xF0, 0x83, 0x80, 0x85, 0x00, 0x87,
 0x90, 0x88, 0x30, 0x8A, 0xC0, 0x8B, 0x50, 0x8D, 0xF0, 0x8E, 0x90, 0x90, 0x20, 0x92, 0xD0, 0x93,
 0x70, 0x95, 0x10, 0x97, 0xC0, 0x98, 0x60, 0x9A, 0x10, 0x9C, 0xC0, 0x9D, 0x70, 0x9F, 0x30, 0xA1,
 0xE0, 0xA2, 0xA0, 0xA4, 0x60, 0xA6, 0x20, 0xA8, 0xE0, 0xA9, 0xA0, 0xAB, 0x60, 0xAD, 0x30, 0xAF,
 0x00, 0xB1, 0xD0, 0xB2, 0xA0, 0xB4, 0x70, 0xB6, 0x50, 0xB8, 0x20, 0xBA, 0x00, 0xBC, 0xE0, 0xBD,
 0xC0, 0xBF, 0xA0, 0xC1, 0x80, 0xC3, 0x70, 0xC5, 0x60, 0xC7, 0x40, 0xC9, 0x30, 0xCB, 0x30, 0xCD,
 0x20, 0xCF, 0x10, 0xD1, 0x10, 0xD3, 0x10, 0xD5, 0x10, 0xD7, 0x10, 0xD9, 0x10, 0xDB, 0x20, 0xDD,
 0x30, 0xDF, 0x30, 0xE1, 0x40, 0xE3, 0x50, 0xE5, 0x70, 0xE7, 0x80, 0xE9, 0xA0, 0xEB, 0xC0, 0xED,
 0xE0, 0xEF, 0x00, 0xF2, 0x20, 0xF4, 0x50, 0xF6, 0x70, 0xF8, 0xA0, 0xFA, 0xD0, 0xFC, 0x00, 0xFF,
};

code BYTE _SRGBFixGamma[] =
{
 0x00, 0x4A, 0x10, 0xC4, 0x1E, 0x21,
 0x6A, 0xAC, 0x22, 0xE8, 0x1E, 0x32,
 0x50, 0x7E, 0x33, 0xAA, 0xD4, 0x33,
 0xFA, 0x20, 0x43, 0x44, 0x68, 0x44,
 0x88, 0xAA, 0x44, 0xC8, 0xE8, 0x44,
 0x04, 0x22, 0x55, 0x3E, 0x58, 0x55,
 0x74, 0x8E, 0x55, 0xA8, 0xC0, 0x55,
 0xD8, 0xF0, 0x55, 0x08, 0x20, 0x66,
 0x36, 0x4C, 0x66, 0x62, 0x78, 0x66,
 0x8E, 0xA2, 0x66, 0xB8, 0xCC, 0x66,
 0xE0, 0xF4, 0x66, 0x08, 0x1A, 0x77,
 0x2E, 0x40, 0x77, 0x52, 0x66, 0x77,
 0x78, 0x8A, 0x77, 0x9C, 0xAC, 0x77,
 0xBE, 0xD0, 0x77, 0xE0, 0xF2, 0x77,
 0x02, 0x12, 0x88, 0x22, 0x34, 0x88,
 0x44, 0x54, 0x88, 0x64, 0x72, 0x88,
 0x82, 0x92, 0x88, 0xA0, 0xB0, 0x88,
 0xBE, 0xCE, 0x88, 0xDC, 0xEC, 0x88,
 0xFA, 0x08, 0x98, 0x16, 0x24, 0x99,
 0x32, 0x40, 0x99, 0x4E, 0x5C, 0x99,
 0x6A, 0x78, 0x99, 0x86, 0x94, 0x99,
 0xA0, 0xAE, 0x99, 0xBA, 0xC8, 0x99,
 0xD6, 0xE2, 0x99, 0xEE, 0xFC, 0x99,
 0x08, 0x14, 0xAA, 0x22, 0x2E, 0xAA,
 0x3A, 0x46, 0xAA, 0x54, 0x60, 0xAA,
 0x6C, 0x78, 0xAA, 0x84, 0x90, 0xAA,
 0x9C, 0xA8, 0xAA, 0xB2, 0xBE, 0xAA,
 0xCA, 0xD6, 0xAA, 0xE2, 0xEC, 0xAA,
 0xF8, 0x04, 0xBA, 0x0E, 0x1A, 0xBB,
 0x26, 0x30, 0xBB, 0x3C, 0x46, 0xBB,
 0x52, 0x5C, 0xBB, 0x68, 0x72, 0xBB,
 0x7E, 0x88, 0xBB, 0x92, 0x9E, 0xBB,
 0xA8, 0xB2, 0xBB, 0xBC, 0xC8, 0xBB,
 0xD2, 0xDC, 0xBB, 0xE6, 0xF0, 0xBB,
 0xFC, 0x06, 0xCB, 0x10, 0x1A, 0xCC,
 0x24, 0x2E, 0xCC, 0x38, 0x42, 0xCC,
 0x4C, 0x56, 0xCC, 0x60, 0x6A, 0xCC,
 0x74, 0x7C, 0xCC, 0x86, 0x90, 0xCC,
 0x9A, 0xA4, 0xCC, 0xAE, 0xB6, 0xCC,
 0xC0, 0xCA, 0xCC, 0xD4, 0xDC, 0xCC,
 0xE6, 0xF0, 0xCC, 0xF8, 0x02, 0xDC,
 0x0C, 0x14, 0xDD, 0x1E, 0x26, 0xDD,
 0x30, 0x38, 0xDD, 0x42, 0x4C, 0xDD,
 0x54, 0x5E, 0xDD, 0x66, 0x6E, 0xDD,
 0x78, 0x80, 0xDD, 0x8A, 0x92, 0xDD,
 0x9C, 0xA4, 0xDD, 0xAC, 0xB6, 0xDD,
 0xBE, 0xC6, 0xDD, 0xD0, 0xD8, 0xDD,
 0xE0, 0xEA, 0xDD, 0xF2, 0xFA, 0xDD,
 0x02, 0x0C, 0xEE, 0x14, 0x1C, 0xEE,
 0x24, 0x2C, 0xEE, 0x36, 0x3E, 0xEE,
 0x46, 0x4E, 0xEE, 0x56, 0x5E, 0xEE,
 0x66, 0x70, 0xEE, 0x78, 0x80, 0xEE,
 0x88, 0x90, 0xEE, 0x98, 0xA0, 0xEE,
 0xA8, 0xB0, 0xEE, 0xB8, 0xC0, 0xEE,
 0xC8, 0xD0, 0xEE, 0xD8, 0xE0, 0xEE,
 0xE8, 0xF0, 0xEE, 0xF8, 0x00, 0xFE,
 0x06, 0x0E, 0xFF, 0x16, 0x1E, 0xFF,
 0x26, 0x2E, 0xFF, 0x36, 0x3C, 0xFF,
 0x44, 0x4C, 0xFF, 0x54, 0x5C, 0xFF,
 0x64, 0x6A, 0xFF, 0x72, 0x7A, 0xFF,
 0x82, 0x88, 0xFF, 0x90, 0x98, 0xFF,
 0xA0, 0xA6, 0xFF, 0xAE, 0xB6, 0xFF,
 0xBC, 0xC4, 0xFF, 0xCC, 0xD2, 0xFF,
 0xDA, 0xE2, 0xFF, 0xE8, 0xF0, 0xFF,
};

code BYTE _DefaultPostGamma[] =
{
 0x24, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44, 0x44,
 0x44, 0x44, 0x44, 0x44,
};
#endif

#if HDR_AUTODOWNLOAD
#define _xdata2Dram        ((unsigned char volatile xdata *) 0x0000)

xdata DWORD _dramAddr = HDR_ADL_ADDR;
xdata DWORD _dramXYVCCAddr = HDR_ADL_ADDR + 0x2000;
#if XYVCC_ADL_DEGAMMA_600E
xdata DWORD _dramPostGammaAddr = HDR_ADL_ADDR + 0x2000 + 0x4000;
#else
xdata DWORD _dramPostGammaAddr = HDR_ADL_ADDR + 0x2000 + 0x3000;
#endif
xdata DWORD _xdataAddr = (WIN1_ADDR_START << 10);
#define  AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR    4944

#if CHIP_ID == CHIP_MT9700
#define ADL_I64MODE 0
#if (ADL_I64MODE == 0)
#define BYTES_PER_WORD           (16) // 64 byte.
#define WRITE_TMO_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x09);\
    *(baseaddr + 1) = ((*(baseaddr + 1) & 0xF0) | (((value) >> 8) & 0x0F));\
    *(baseaddr) = ((value) & 0xFF);

#define WRITE_GAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x0A);\
    *(baseaddr + 1) = ((*(baseaddr + 1) & 0x0F) | (((value) << 4) & 0xF0));\
    *(baseaddr + 2) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 3) = ((*(baseaddr + 3) & 0xF0) | (((value) >> 12) & 0x0F));

#define WRITE_DEGAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x0C);\
    *(baseaddr + 3) = ((*(baseaddr + 3) & 0x0F) | (((value) << 4) & 0xF0));\
    *(baseaddr + 4) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 5) = ((*(baseaddr + 5) & 0x80) | (((value) >> 12) & 0x7F));

#define WRITE_GAMMA_CTRL_FORMAT_1(baseaddr, index) \
     *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0A) | 0x0A);


void WriteXVYCCDegamma(DWORD baseaddr, DWORD value)
{
    _xdata2Dram[baseaddr + 15] = 0xFF;
    _xdata2Dram[baseaddr + 14] = 0x00;
    _xdata2Dram[baseaddr + 13] = 0xFF;
    _xdata2Dram[baseaddr + 12] = 0xFF;
    _xdata2Dram[baseaddr + 11] = (((value) >> 16) & 0x07);
    _xdata2Dram[baseaddr + 10] = (((value) >> 8) & 0xFF);
    _xdata2Dram[baseaddr + 9] = ((value) & 0xFF);
    _xdata2Dram[baseaddr + 8] = 0xFF;
    _xdata2Dram[baseaddr + 7] = 0xFF;
    _xdata2Dram[baseaddr + 6] = (((value) >> 12) & 0x7F);
    _xdata2Dram[baseaddr + 5] = (((value) >> 4) & 0xFF);
    _xdata2Dram[baseaddr + 4] = ((value<<4) & 0xF0);
    _xdata2Dram[baseaddr + 3] = 0xFF;
    _xdata2Dram[baseaddr + 2] =  (((value) >> 16) & 0x07);
    _xdata2Dram[baseaddr + 1] = (((value) >> 8) & 0xFF);
    _xdata2Dram[baseaddr + 0] = ((value) & 0xFF);
}

void WriteXVYCCGamma(DWORD baseaddr, DWORD value)
{
    _xdata2Dram[baseaddr + 15] = 0x00;
    _xdata2Dram[baseaddr + 14] = 0x07;
    _xdata2Dram[baseaddr + 13] = 0x00;
    _xdata2Dram[baseaddr + 12] = 0x00;
    _xdata2Dram[baseaddr + 11] = 0x00;
    _xdata2Dram[baseaddr + 10] = (((value) >> 8) & 0x0F)|(((value >> 12) & 0xFF) << 4);
    _xdata2Dram[baseaddr + 9] =((value) & 0xFF);
    _xdata2Dram[baseaddr + 8] = 0x00;
    _xdata2Dram[baseaddr + 7] = 0x00;
    _xdata2Dram[baseaddr + 6] = ((value >> 12) & 0xFF);
    _xdata2Dram[baseaddr + 5] = (((value) >> 4) & 0xFF);
    _xdata2Dram[baseaddr + 4] = ((value<<4) & 0xF0);
    _xdata2Dram[baseaddr + 3] = 0x00;
    _xdata2Dram[baseaddr + 2] = 0x00;
    _xdata2Dram[baseaddr + 1] = (((value) >> 8) & 0x0F) |(((value >> 12) & 0xFF) << 4);
    _xdata2Dram[baseaddr + 0] = ((value) & 0xFF);
}

void WriteVOPPostGamma(DWORD baseaddr, WORD value, BYTE channel)
{
    _xdata2Dram[baseaddr + 15] = 0x00;
    _xdata2Dram[baseaddr + 14] = 0x00;
    _xdata2Dram[baseaddr + 13] = 0x70;//Bit 4, 5, 6 (108,109,110)
    _xdata2Dram[baseaddr + 12] = 0x00;
    _xdata2Dram[baseaddr + 11] = 0x00;

    if(channel == 0)
    {
        _xdata2Dram[baseaddr + 10] = ((value >> 8) & 0x0F);// | (((value >> 12) & 0xFF) << 4);
        _xdata2Dram[baseaddr + 9]  = ((value) & 0xFF);//R_in 72:83
    }

    _xdata2Dram[baseaddr + 8]  = 0x00;
    _xdata2Dram[baseaddr + 7]  = 0x00;

    if(channel == 1)
    {
        _xdata2Dram[baseaddr + 6]  = ((value >> 12) & 0xFF);
        _xdata2Dram[baseaddr + 5]  = ((value >> 4) & 0xFF);
        _xdata2Dram[baseaddr + 4]  = ((value << 4) & 0xF0);//G_in 47:36
    }

    _xdata2Dram[baseaddr + 3]  = 0x00;
    _xdata2Dram[baseaddr + 2]  = 0x00;

    if(channel == 2)
    {
        _xdata2Dram[baseaddr + 1]  = ((value >> 8) & 0x0F);// | (((value >> 12) & 0xFF) << 4);
        _xdata2Dram[baseaddr + 0]  = ((value) & 0xFF);//B_in 11:0
    }
}

void WriteHDRDegamma(DWORD baseaddr, int u16index, DWORD value)
{
    IMI_WriteByte(baseaddr, value&0xFF);
    IMI_WriteByte(baseaddr+1,(value>>8)&0xFF);
    IMI_WriteByte(baseaddr+2,(value>>16)&0xFF);
    IMI_WriteByte(baseaddr+3,(value>>24)&0xFF);
UNUSED(u16index);
}

void WriteHDRGamma(DWORD baseaddr, int u16index, DWORD value)
{
    IMI_WriteByte(baseaddr, value&0xFF);
    IMI_WriteByte(baseaddr+1,(value>>8)&0xFF);
    UNUSED(u16index);
}

#else
#define BYTES_PER_WORD           (8) // 64 byte.
#define WRITE_TMO_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 7) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 6) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x09);\
    *(baseaddr + 1) = ((*(baseaddr + 1) & 0xF0) | (((value) >> 8) & 0x0F));\
    *(baseaddr) = ((value) & 0xFF);

#define WRITE_GAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 7) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 6) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x0A);\
    *(baseaddr + 1) = ((*(baseaddr + 1) & 0x0F) | (((value) << 4) & 0xF0));\
    *(baseaddr + 2) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 3) = ((*(baseaddr + 3) & 0xF0) | (((value) >> 12) & 0x0F));

#define WRITE_DEGAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *(baseaddr + 7) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 6) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x0C);\
    *(baseaddr + 3) = ((*(baseaddr + 3) & 0x0F) | (((value) << 4) & 0xF0));\
    *(baseaddr + 4) = (((value) >> 4) & 0xFF);\
    *(baseaddr + 5) = ((*(baseaddr + 5) & 0x80) | (((value) >> 12) & 0x7F));

#define WRITE_GAMMA_CTRL_FORMAT_1(baseaddr, index) \
     *(baseaddr + 6) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0A) | 0x0A);


void WriteHDRDegamma(DWORD baseaddr, int index, DWORD value)
{
    IMI_WriteByte(baseaddr, value&0xFF);
    IMI_WriteByte(baseaddr+1,(value>>8)&0xFF);
    IMI_WriteByte(baseaddr+2,(value>>16)&0xFF);
    IMI_WriteByte(baseaddr+3,(value>>24)&0xFF);
UNUSED(index);
}

void WriteHDRGamma(DWORD baseaddr, int index, DWORD value)
{
    IMI_WriteByte(baseaddr, value&0xFF);
    IMI_WriteByte(baseaddr+1,(value>>8)&0xFF);
    UNUSED(index);
}
#endif
#define WRITE_3DLUT_DATA_FORMAT_1(baseaddr, index, subindex, rval, gval, bval) \
    *(baseaddr + 15) = (((*(baseaddr + 15)) & 0xC0) | (((index) >> 4) & 0x3F));\
    *(baseaddr + 14) = ((((index) << 4) & 0xF0) | ((*(baseaddr + 14)) & 0x0F) | 0x08);\
    *(baseaddr + 7) = ((*(baseaddr + 7) & 0xF1) | 0x08);\
    *(baseaddr + 7) = ((*(baseaddr + 7) & 0x0F) | (((bval) << 4) & 0xF0));\
    *(baseaddr + 8) = (((bval) >> 4) & 0xFF);\
    *(baseaddr + 9) = ((*(baseaddr + 9) & 0xF0) | (((bval) >> 12) & 0x0F));\
    *(baseaddr + 9) = ((*(baseaddr + 9) & 0x0F) | (((gval) << 4) & 0xF0));\
    *(baseaddr + 10) = (((gval) >> 4) & 0xFF);\
    *(baseaddr + 11) = ((*(baseaddr + 11) & 0xF0) | (((gval) >> 12) & 0x0F));\
    *(baseaddr + 11) = ((*(baseaddr + 11) & 0x0F) | (((rval) << 4) & 0xF0));\
    *(baseaddr + 12) = (((rval) >> 4) & 0xFF);\
    *(baseaddr + 13) = ((*(baseaddr + 13) & 0xF0) | (((rval) >> 12) & 0x0F));\
    *(baseaddr + 13) = ((*(baseaddr + 13) & 0x0F) | (((subindex) << 4) & 0xF0));
#else//MT9701
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define WRITE_TMO_DATA_FORMAT_1(baseaddr, index, value) \
    *((baseaddr) + (index)*2 + 1) = (((value) >> 8) & 0x0F);\
    *((baseaddr) + (index)*2)  = ((value) & 0xFF);

#define WRITE_GAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *((baseaddr) + (index)*2 + 1) = (((value)>>8) & 0xFF);\
    *((baseaddr) + (index)*2) = ((value) & 0xFF);

#define WRITE_OOTF_DATA_FORMAT_1(baseaddr, index, value) \
    *((baseaddr) + (index)*2 + 1) = (((value)>>8) & 0xFF);\
    *((baseaddr) + (index)*2) = ((value) & 0xFF);

#define WRITE_DEGAMMA_DATA_FORMAT_1(baseaddr, index, value) \
    *((baseaddr) + (index)*4 + 2) = (((value)>>16) & 0x07);\
    *((baseaddr) + (index)*4 + 1) = (((value)>>8) & 0xFF);\
    *((baseaddr) + (index)*4) = ((value) & 0xFF);


void WriteXVYCCDegamma(DWORD baseaddr, DWORD value)
{
    _xdata2Dram[baseaddr + 15] = 0xFF;
    _xdata2Dram[baseaddr + 14] = 0x00;
    _xdata2Dram[baseaddr + 13] = 0xFF;
    _xdata2Dram[baseaddr + 12] = 0xFF;
    _xdata2Dram[baseaddr + 11] = (((value) >> 16) & 0x07);
    _xdata2Dram[baseaddr + 10] = (((value) >> 8) & 0xFF);
    _xdata2Dram[baseaddr + 9] = ((value) & 0xFF);
    _xdata2Dram[baseaddr + 8] = 0xFF;
    _xdata2Dram[baseaddr + 7] = 0xFF;
    _xdata2Dram[baseaddr + 6] = (((value) >> 12) & 0x7F);
    _xdata2Dram[baseaddr + 5] = (((value) >> 4) & 0xFF);
    _xdata2Dram[baseaddr + 4] = ((value<<4) & 0xF0);
    _xdata2Dram[baseaddr + 3] = 0xFF;
    _xdata2Dram[baseaddr + 2] =  (((value) >> 16) & 0x07);
    _xdata2Dram[baseaddr + 1] = (((value) >> 8) & 0xFF);
    _xdata2Dram[baseaddr + 0] = ((value) & 0xFF);
}

void WriteXVYCCGamma(DWORD baseaddr, DWORD value)
{
    _xdata2Dram[baseaddr + 15] = 0x00;
    _xdata2Dram[baseaddr + 14] = 0x07;
    _xdata2Dram[baseaddr + 13] = 0x00;
    _xdata2Dram[baseaddr + 12] = 0x00;
    _xdata2Dram[baseaddr + 11] = 0x00;
    _xdata2Dram[baseaddr + 10] = (((value) >> 8) & 0x0F)|(((value >> 12) & 0xFF) << 4);
    _xdata2Dram[baseaddr + 9] =((value) & 0xFF);
    _xdata2Dram[baseaddr + 8] = 0x00;
    _xdata2Dram[baseaddr + 7] = 0x00;
    _xdata2Dram[baseaddr + 6] = ((value >> 12) & 0xFF);
    _xdata2Dram[baseaddr + 5] = (((value) >> 4) & 0xFF);
    _xdata2Dram[baseaddr + 4] = ((value<<4) & 0xF0);
    _xdata2Dram[baseaddr + 3] = 0x00;
    _xdata2Dram[baseaddr + 2] = 0x00;
    _xdata2Dram[baseaddr + 1] = (((value) >> 8) & 0x0F) |(((value >> 12) & 0xFF) << 4);
    _xdata2Dram[baseaddr + 0] = ((value) & 0xFF);
}

void WriteVOPPostGamma(DWORD baseaddr, WORD value, BYTE channel)
{
    _xdata2Dram[baseaddr + 15] = 0x00;
    _xdata2Dram[baseaddr + 14] = 0x00;
    _xdata2Dram[baseaddr + 13] = 0x70;//Bit 4, 5, 6 (108,109,110)
    _xdata2Dram[baseaddr + 12] = 0x00;
    _xdata2Dram[baseaddr + 11] = 0x00;

    if(channel == 0)
    {
        _xdata2Dram[baseaddr + 10] = ((value >> 8) & 0x0F);// | (((value >> 12) & 0xFF) << 4);
        _xdata2Dram[baseaddr + 9]  = ((value) & 0xFF);//R_in 72:83
    }

    _xdata2Dram[baseaddr + 8]  = 0x00;
    _xdata2Dram[baseaddr + 7]  = 0x00;

    if(channel == 1)
    {
        _xdata2Dram[baseaddr + 6]  = ((value >> 12) & 0xFF);
        _xdata2Dram[baseaddr + 5]  = ((value >> 4) & 0xFF);
        _xdata2Dram[baseaddr + 4]  = ((value << 4) & 0xF0);//G_in 47:36
    }

    _xdata2Dram[baseaddr + 3]  = 0x00;
    _xdata2Dram[baseaddr + 2]  = 0x00;

    if(channel == 2)
    {
        _xdata2Dram[baseaddr + 1]  = ((value >> 8) & 0x0F);// | (((value >> 12) & 0xFF) << 4);
        _xdata2Dram[baseaddr + 0]  = ((value) & 0xFF);//B_in 11:0
    }
}
#endif


BOOL WaitADLFinished(BYTE u8WinIdx)
{
    DWORD u32BaseAddr = GetADLBaseAddr(u8WinIdx);
    int timer = 0;
    const int maxCnt = 32;
    UNUSED(u8WinIdx);
    while (msReadByte(u32BaseAddr + SC67_50) & BIT0)
    {
        timer++;
        if (timer > maxCnt)
        {
        break;
        }
        Delay1ms(1);
    }

    if (timer >= maxCnt)
    {
        return FALSE;
    }
    else{
        return TRUE;
    }
}


void FireXVYCCADL(DWORD u32StartAddr)
{
	msWriteByte(SC67_01, 0x00);
	msWriteByte(SC67_2C, u32StartAddr & 0xFF);
	msWriteByte(SC67_2D, u32StartAddr>>8 & 0xFF);
	msWriteByte(SC67_2E, u32StartAddr>>16 & 0xFF);
	msWriteByte(SC67_2F, u32StartAddr>>24 & 0xFF);
#if   XYVCC_ADL_DEGAMMA_600E
	msWriteByte(SC67_34,0x00);
	msWriteByte(SC67_35,0x04);
	msWriteByte(SC67_3A,0x00);
	msWriteByte(SC67_3B,0x04);
#else
	msWriteByte(SC67_34,0x00);
	msWriteByte(SC67_35,0x03);
	msWriteByte(SC67_3A,0x00);
	msWriteByte(SC67_3B,0x03);
#endif
	msWriteByte(SC67_40,0x00);
	msWriteByte(SC67_41,0x00);
	msWriteByteMask(SC67_22, BIT2, BIT2);
	msWriteByte(SC67_03,0x00);
}

void FirePQGammaADL(DWORD u32StartAddr)
{
    //Enable
	msWriteByte(SC67_01, 0x00);

    //Base address
    msWrite4Byte(SC67_04, u32StartAddr);

    //Depth
    msWrite2Byte(SC67_10,0x0100);

    //Length
    msWrite2Byte(SC67_16,0x0100);

    //Init address
    msWrite2Byte(SC67_1C,0x0000);

    //Gamma table enable
	msWriteByteMask(SC67_02, BIT0, BIT0);
	msWriteByte(SC67_03,0x00);
}

void FireAutoDownload(BYTE u8WinIdx)
{
    HDR_PrintMsg("FireAutoDownload\n");
    DWORD u32BaseAddr = 0;
    DWORD u32ADLAddr =  0;//GetADLBaseAddr(u8WinIdx);
    #if CHIP_ID == CHIP_MT9701
    u32ADLAddr = HDR_ADL_ADDR;
    u32ADLAddr = u32ADLAddr >> 5;
    msWriteBit(SC67_50, FALSE, BIT1);
    #endif
    //DWORD phyBaseAddr = _dramAddr & 0x7FFFFFFF;
    //DWORD u32Depth = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;

    msWrite2Byte(u32BaseAddr + SC7A_E0, 0x8000); // AutoDownload On
    msWriteByte(u32BaseAddr + SC7A_E2, 0x20); // vsync delay
    msWriteByte(u32BaseAddr + SC7A_E4, BIT3); // AutoDownload 1 to 4

    msWriteByteMask( SC67_50,BIT2,BIT2); //client8 (hdr) clk gate enable
    msWrite4Byte( SC67_52, u32ADLAddr); //set autodownload address
    msWriteByteMask( SC67_50,BIT0,BIT0); //auto download en
    msWrite2Byte(SC67_56,0x141); //reg_client8_depth
    msWrite2Byte(SC67_58,0x80); //reg_client8_req_len
    msWrite2Byte(SC67_5A,0xFFFF); //reg_client8_ini_addr
    mdrv_HDR_DS_WBMask(SC67_50,BIT1,BIT1); //auto download trigger
	//DWORD addrSWmodeEn = SC67_70;
    //DWORD addrSWmodeTrigger = SC67_72;
    //msWriteBit(addrSWmodeEn, TRUE, BIT3); //sw mode enable
    //mdrv_HDR_DS_WBit(addrSWmodeTrigger, TRUE, BIT3); //sw mode trigger

    msWrite2Byte(SC79_10,g_sPnlInfo.sPnlTiming.u16Width);
    msWrite2Byte(SC79_12,g_sPnlInfo.sPnlTiming.u16Height);
    msWriteByteMask(SC79_E1,BIT7,BIT7); //autodownload2riu mux for TCH hdr
    msWriteByteMask(SC79_E3,BIT7,BIT7); // 0: after vsync cnt    1: hdr vfde end

    WaitADLFinished(u8WinIdx);
}
#endif

//32b to odinson(14b)+exponent(5b)
DWORD ToSramFmt(DWORD val)
{
    DWORD sramVal = 0;
    int idx = 0;
    for (idx = 0; idx < 18; idx++)
    {
        if (val < ((DWORD)0x1 << (idx+14)))
            break;
    }
    sramVal = ( (val >> idx) << 5) | idx;
    return sramVal;
}

void SetDeGammaToDiffDist(DWORD u32BaseAddr, BYTE IsEnDiffDist)
{
    if (IsEnDiffDist)
    {
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_84, 0xFFFF);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A0, 0x0008);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A2, 0x0201);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A4, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A6, 0x0403);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A8, 0x0404);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_AA, 0x0404);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_AC, 0x0504);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_AE, 0x0605);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C0, 0);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C2, 2);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C4, 4);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C6, 8);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C8, 16);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_CA, 32);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_CC, 48);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_CE, 64);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D0, 96);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D2, 128);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D4, 160);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D6, 192);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D8, 224);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_DA, 256);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_DC, 320);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_DE, 384);
    }
    else
    {
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_84, 0xC000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A0, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A2, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A4, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A6, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_A8, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_AA, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_AC, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_AE, 0x070F);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C0, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C2, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C4, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C6, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_C8, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_CA, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_CC, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_CE, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D0, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D2, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D4, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D6, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_D8, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_DA, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_DC, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_DE, 0x0100);
    }
}

void SetGammaToDiffDist(DWORD u32BaseAddr, BYTE IsEnDiffDist)
{
    if (IsEnDiffDist)
    {
        mdrv_HDR_DS_W4B(u32BaseAddr + SC7C_04, 0xFFFFFFFF);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_20, 0x0008);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_22, 0x0201);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_24, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_26, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_28, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_2A, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_2C, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_2E, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_30, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_32, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_34, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_36, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_38, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_3A, 0x0303);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_3C, 0x0403);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_3E, 0x0404);

        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_40, 0);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_42, 2);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_44, 4);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_46, 8);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_48, 16);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_4A, 32);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_4C, 48);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_4E, 64);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_50, 80);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_52, 96);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_54, 112);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_56, 128);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_58, 144);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_5A, 160);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_5C, 176);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_5E, 192);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_60, 208);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_62, 224);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_64, 240);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_66, 256);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_68, 272);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_6A, 288);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_6C, 304);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_6E, 320);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_70, 336);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_72, 352);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_74, 368);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_76, 384);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_78, 400);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_7A, 416);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_7C, 448);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_7E, 480);
    }
    else
    {
        mdrv_HDR_DS_W4B(u32BaseAddr + SC7C_04, 0xFF800000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_20, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_22, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_24, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_26, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_28, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_2A, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_2C, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_2E, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_30, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_32, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_34, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_36, 0x0800);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_38, 0x0100);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_3A, 0x0302);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_3C, 0x0504);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_3E, 0x0706);

        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_40, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_42, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_44, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_46, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_48, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_4A, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_4C, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_4E, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_50, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_52, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_54, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_56, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_58, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_5A, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_5C, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_5E, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_60, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_62, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_64, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_66, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_68, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_6A, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_6C, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_6E, 0x0000);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_70, 0x0002);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_72, 0x0004);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_74, 0x0008);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_76, 0x0010);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_78, 0x0020);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_7A, 0x0040);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_7C, 0x0080);
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7C_7E, 0x0100);
    }
}
#if ENABLE_LUT_AUTODOWNLOAD
void WriteXVYCCTable(BYTE u8WinIdx){
    BYTE *DefaultDeGammaTab[][3] =
    {
        {(BYTE*)mdrv_HDRLUT_DeGamma_Get(), (BYTE*)mdrv_HDRLUT_DeGamma_Get(), (BYTE*)mdrv_HDRLUT_DeGamma_Get(),},
    };
    msAPI_DeGammaLoadTbl_1024E_16B_N(u8WinIdx, DefaultDeGammaTab[0]);

    BOOL u8ExtendMode = XVYCC_GAMMA_EXT;
    const BYTE* pU8FixGamma = mdrv_HDRLUT_FixGamma_Get(u8ExtendMode);
    BYTE *DefaultFixGammaTab[][3]=
    {
        {(BYTE*)pU8FixGamma, (BYTE*)pU8FixGamma, (BYTE*)pU8FixGamma},
    };
    msAPI_FixGammaLoadTbl_256E_12B_Extend(MAIN_WINDOW, DefaultFixGammaTab[0], u8ExtendMode);
    UNUSED(u8WinIdx);
}

#endif

#if CHIP_ID == CHIP_MT9700
void WriteDMTable(BYTE u8WinIdx, HDR_DM_Types type, DWORD* pTabel, BYTE IsDiffDist)
{
    xdata DWORD idx = 0;
    xdata DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    xdata DWORD IMIAddress = 0;
    int BytesPerEntryDe = 4;
    int BytesPerEntryGa = 2;
    xdata DWORD cnt = 0;
    //xdata DWORD checkSum = 0;
    #if HDR_3DLUT
    xdata MS_U16 u32Index= 0;
    xdata MS_U32 i = 0;
    xdata MS_U16 u16Subindex = 0;
    xdata MS_U32 u16BVal = 0;
    xdata MS_U32 u16GVal = 0;
    xdata MS_U32 u16RVal = 0;
    xdata MS_U32 _au32_3dlut_entry_num[8] = {736, 656, 656, 576, 656, 576, 576, 512};
    #endif

#if HDR_AUTODOWNLOAD
    xdata DWORD wrLen = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR;
    xdata MS_U32 i = 0;
#endif
    //memcpy(_buffer, pTabel, sizeof(_buffer));

    switch((int)type)
    {
        case HDR_DeGammaTbl:
        {
            SetDeGammaToDiffDist(u32BaseAddr, IsDiffDist);
            //for(idx = 0; idx < AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR; idx++)
            //{
            //    _buffer[idx] = ToSramFmt(_buffer[idx]);
            //}
            IMIAddress = 0;

        }break;

        case HDR_GammaTbl:
        {
            SetGammaToDiffDist(u32BaseAddr, IsDiffDist);
            IMIAddress = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR*BytesPerEntryDe;
        }break;

        case HDR_ToneMappingTbl:
        {
            SetGammaToDiffDist(u32BaseAddr, IsDiffDist);
            IMIAddress = AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR*BytesPerEntryGa+AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR*BytesPerEntryDe;
        }break;
    }

    #if HDR_AUTODOWNLOAD
    #if (ADL_I64MODE == 0)
    msWrite2Byte(u32BaseAddr + SC7A_E0, 0x8000); // AutoDownload On
    #else
    msWrite2Byte(u32BaseAddr + SC7A_E0, 0xC000); // AutoDownload On & i64 mode
    #endif
    msWriteByte(u32BaseAddr + SC7A_E2, 0x20); // vsync delay
    msWriteByte(u32BaseAddr + SC7A_E4, BIT3); // AutoDownload 1 to 4

    if (type == HDR_3DLutTbl)
    {
        wrLen =  AUTO_DOWNLOAD_HDR_3DLUT_SRAM_MAX_ADDR;
    }


#if HDR_AUTODOWNLOAD
    if(type == HDR_DeGammaTbl)
    cnt = 512*BytesPerEntryDe;
    else if(type == HDR_GammaTbl)
    cnt = 512*BytesPerEntryGa;

    for(i=0;i<cnt;++i)
    IMI_WriteByte(IMIAddress+i+32, 0x00);

    BYTE header[32] = {0xF7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    for(i=0;i<32;++i)
    IMI_WriteByte(i, header[i]&0xFF);
    HDR_PrintMsg("header after\n");
#endif

    for(idx = 0; idx < wrLen; idx++)
    {
        switch((int)type)
        {
            case HDR_DeGammaTbl:
            {

                WriteHDRDegamma(32+ idx * BytesPerEntryDe+IMIAddress, idx, ToSramFmt(pTabel[idx]));

                //WriteHDRDegamma(_xdataAddr +32+ idx * BYTES_PER_WORD - offset, idx, pTabel[idx]);
                //WRITE_DEGAMMA_DATA_FORMAT_1((BYTE*)(_phyBaseAddr + idx * BYTES_PER_WORD - offset), idx, _buffer[idx]);
                //WriteHDRDegamma(_xdataAddr + idx * BYTES_PER_WORD - offset, idx, ToSramFmt(pTabel[idx]));
            }break;
            case HDR_GammaTbl:
            {

                WriteHDRGamma(32 + idx * BytesPerEntryGa+IMIAddress, idx, pTabel[idx]);

                //WRITE_GAMMA_DATA_FORMAT_1((BYTE*)(_phyBaseAddr + idx * BYTES_PER_WORD - offset), idx, _buffer[idx]);
                //WriteHDRGamma(_xdataAddr + idx * BYTES_PER_WORD - offset, idx, pTabel[idx]);
            }break;
            case HDR_ToneMappingTbl:
            {

                WRITE_TMO_DATA_FORMAT_1((BYTE*)(32+ idx * BytesPerEntryGa+IMIAddress), idx, pTabel[idx]);
                //for(i=0 ; i < 8 ; ++i)
                //    WRITE_TMO_DATA_FORMAT_1((BYTE*)(_xdataAddr  +32+ idx * BYTES_PER_WORD - offset), idx, pTabel[idx+IMI_WriteOrder[i]]);
            }break;

            #if HDR_3DLUT
            case HDR_3DLutTbl:
            {
                u16BVal = pTabel[i];
                u16GVal = pTabel[i+ 1];
                u16RVal = pTabel[i + 2];
                u16Subindex = 0;
                if((i / 3 == _au32_3dlut_entry_num[0]) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6])) ||
                   (i / 3 == (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6] + _au32_3dlut_entry_num[7])))
                {
                    u32Index = 0;
                }

                if(i / 3 < _au32_3dlut_entry_num[0])
                {
                    u16Subindex = 0;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1]))
                {
                    u16Subindex = 1;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2]))
                {
                    u16Subindex = 2;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3]))
                {
                    u16Subindex = 3;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4]))
                {
                    u16Subindex = 4;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5]))
                {
                    u16Subindex = 5;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6]))
                {
                    u16Subindex = 6;
                }
                else if(i / 3 < (_au32_3dlut_entry_num[0] + _au32_3dlut_entry_num[1] + _au32_3dlut_entry_num[2] + _au32_3dlut_entry_num[3] + _au32_3dlut_entry_num[4] + _au32_3dlut_entry_num[5] + _au32_3dlut_entry_num[6] + _au32_3dlut_entry_num[7]))
                {
                    u16Subindex = 7;
                }

                WRITE_3DLUT_DATA_FORMAT_1((BYTE*)(_xdataAddr + idx * BYTES_PER_WORD - offset), u32Index , u16Subindex, u16RVal, u16GVal, u16BVal);
                i += 3;
                u32Index++;
            }break;
            #endif
        }
    }

    #else
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    //u32BaseAddr = SCRegBase(MapWin2ScIndex(u8WinIdx));

    msWrite2Byte(u32BaseAddr + SC7A_C2, 0x00); // lut_addr
    msWrite2Byte(u32BaseAddr + SC7A_D2, 0x00); // init addr of fast mode(for fast mode)
    msWriteByteMask(u32BaseAddr + SC7A_E1, 0x00, BIT7); // AutoDownload off
    msWriteByteMask(u32BaseAddr + SC7A_C0, type, BIT0 |BIT1|BIT2); // lut_sel Gamma
    msWriteByteMask(u32BaseAddr + SC7A_D0, 0x00, BIT0 |BIT1|BIT2 |BIT3); // lut_sel_ip
    msWriteByteMask(u32BaseAddr + SC7A_C1, 0x00, BIT4); // lut_fast_md off
    msWriteByteMask(u32BaseAddr + SC7A_C1, BIT5, BIT5); // lut_wd_dup_md
    msWriteByteMask(u32BaseAddr + SC7A_C1, BIT7, BIT7); // lut_load_en

    for(idx = 0; idx < AUTO_DOWNLOAD_HDR_TMO_SRAM_MAX_ADDR; idx++)
    {
        msWrite2Byte(u32BaseAddr + SC7A_C2, idx); // lut_addr
        msWrite4Byte(u32BaseAddr + SC7A_C4, pTabel[idx]); //wd0
        msWriteByteMask(u32BaseAddr + SC7A_C0, BIT4, BIT4); // lut_write_pulse
        while  ((msReadByte(u32BaseAddr + SC7A_C1) & BIT0) != BIT0)
        {
            HDR_PrintMsg("wait a moment...\n");
        }
        msWriteByteMask(u32BaseAddr + SC7A_C1, 0x00, BIT0 |BIT1|BIT2 |BIT3); // lut_flag_clr
    }
    msWriteByte(u32BaseAddr + SC7A_C1, 0x00);
    #endif
}
#else//MT9701
#define BYTE_PER_WORD (32)
#define EOTF_ENTRY_PER_CMD (8)
#define OETF_ENTRY_PER_CMD (16)
#define OOTF_ENTRY_PER_CMD (16)
#define TMO_ENTRY_PER_CMD (16)

void WriteDMTable(BYTE u8WinIdx, HDR_DM_Types type, DWORD* pTabel, BYTE IsDiffDist)
{
    WORD u16Idx = 0;
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    DWORD u32MemAddr = HDR_ADL_ADDR;
    BYTE*  pu8WriteTblAddr =  (U8*)(u32MemAddr| (0x80000000));

    switch((int)type)
    {
        case HDR_DeGammaTbl:
        {
            SetDeGammaToDiffDist(u32BaseAddr, IsDiffDist);
            pu8WriteTblAddr += 0x20; //0x20 is header occupied size
            printf("HDR_DeGammaTbl  Addr  : %x\n", pu8WriteTblAddr);
        }break;

        case HDR_GammaTbl:
        {
            SetGammaToDiffDist(u32BaseAddr, IsDiffDist);
            pu8WriteTblAddr += (0x20 + 0x800 + 0x400); //0x20 is header occupied size, 0x1000 is degamma occupied size
            printf("HDR_GammaTbl  Addr  : %x\n", pu8WriteTblAddr);
        }break;

        case HDR_OOTF:
        {
            //SetGammaToDiffDist(u32BaseAddr, IsDiffDist);
           pu8WriteTblAddr += (0x20 + 0x800); //0x20 is header occupied size, 0x1000 is degamma occupied size
           printf("HDR_OOTF  Addr  : %x\n", pu8WriteTblAddr);
        }break;
    }

    BYTE header[32] = {0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    for(u16Idx = 0; u16Idx < 32; ++u16Idx)
    {
        msMemWriteByte(u32MemAddr + u16Idx, header[u16Idx]);
    }

    for(u16Idx = 0; u16Idx < 512; u16Idx++)
    {
        switch((int)type)
        {
            case HDR_DeGammaTbl:
            {
                WRITE_DEGAMMA_DATA_FORMAT_1(pu8WriteTblAddr, u16Idx%EOTF_ENTRY_PER_CMD, ToSramFmt(pTabel[u16Idx]));
                if((u16Idx%EOTF_ENTRY_PER_CMD) == (EOTF_ENTRY_PER_CMD-1))
                {
                    pu8WriteTblAddr+=BYTE_PER_WORD;
                }
            }break;
            case HDR_GammaTbl:
            {
                WRITE_GAMMA_DATA_FORMAT_1(pu8WriteTblAddr, u16Idx%OETF_ENTRY_PER_CMD, pTabel[u16Idx] );
                if((u16Idx%OETF_ENTRY_PER_CMD) == (OETF_ENTRY_PER_CMD-1))
                {
                    pu8WriteTblAddr+=BYTE_PER_WORD;
                }
            }break;
            case HDR_OOTF:
            {
                WRITE_OOTF_DATA_FORMAT_1(pu8WriteTblAddr, u16Idx%OOTF_ENTRY_PER_CMD, pTabel[u16Idx] );
                if((u16Idx%OOTF_ENTRY_PER_CMD) == (OOTF_ENTRY_PER_CMD-1))
                {
                    pu8WriteTblAddr+=BYTE_PER_WORD;
                }
            }break;
        }
    }
}

#endif
//Format to s1.14 format
BYTE FormatMatrixToS1D14(Matrix3x3 mat, WORD* fmtMat)
{
    float roundVal = 0;
    DWORD fixVal = 0, absVal = 0;
    int idx = 0, row = 0, col = 0;
    int elementVal = 0;
    for (row= 0; row < 3; row++ )
    for (col = 0; col < 3; col++ )
    {
    	roundVal = mat.matrix[row][col] * 0x4000;
    	fixVal = (int)roundVal;
    	absVal = abs(fixVal);

    	if (absVal > 0x7FFF)
    	{
    	    //matrix value is over s1.14 format.
    	    return FALSE;
    	}

    	//convert to 2's complement
    	if (roundVal < 0)
    	    elementVal = 0x10000 - absVal;
    	else
    	    elementVal = fixVal;

        fmtMat[idx++] = elementVal;
    	//fmtMat[idx++] = elementVal & 0xFF; //low byte
    	//fmtMat[idx++] = (elementVal >> 8) & 0xFF;   //high byte
    }
    return TRUE;
}

BYTE FormatMatrixToS2D10(Matrix3x3 mat, WORD* fmtMat)
{
    float roundVal = 0;
    int fixVal = 0, absVal = 0;
    int idx = 0, row = 0, col = 0;
    int elementVal = 0;
    for (row= 0; row < 3; row++ )
    for (col = 0; col < 3; col++ )
    {
    	roundVal = mat.matrix[row][col] * 0x400;
    	fixVal = (int)roundVal;
    	absVal = abs(fixVal);

    	if (absVal > 4095)
    	{
    		//matrix value is over s2.10 format.
    		return FALSE;
    	}

    	//convert to 2's complement
    	//if (roundVal < 0)
    	//    elementVal = 0x1FFF - absVal;
    	//else
    	    elementVal = fixVal;

        fmtMat[idx++] = elementVal;
    	//fmtMat[idx++] = elementVal & 0xFF; //low byte
    	//fmtMat[idx++] = (elementVal >> 8) & 0xFF;   //high byte
    }
    return TRUE;
}

#if 0
void BypassB01(BYTE u8WinIdx, BOOL IsBypass)
{
    DWORD u32BaseAddr = GetWndBaseAddr(u8WinIdx);
    if(TRUE == IsBypass)
    {
        msWriteByteMask(u32BaseAddr + SC7A_02, BIT0, BIT0);
    }
    else
    {
        msWriteByteMask(u32BaseAddr + SC7A_02, 0x00, BIT0);
    }
}
#endif

void ConfigB01_Y2R(BYTE u8WinIdx)
{
    ST_COMBO_COLOR_FORMAT cf;
    int idx = 0;
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    WORD coeff[9] = {0};
    const DWORD* pYCbCrOffset = _ycbcrOffset[0];
    const float *pCoeff = _bypassMatrix;
    cf = GetHDRColorFormat();
    if(cf.ucColorType == COMBO_COLOR_FORMAT_RGB)
    {
        if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
        {
            pYCbCrOffset = _ycbcrOffset[7];
            pCoeff = _limitToFull_Matrix;
            HDR_PrintMsg("Source is RGB limit Range");
        }else{//others are full ragne
                    pYCbCrOffset = _ycbcrOffset[0];
            HDR_PrintMsg("Source is RGB full Range");
        }
    }else{ //YCbCr
        switch((int)cf.ucColorimetry)
        {
            default:
            case COMBO_COLORIMETRY_NONE:
            case COMBO_COLORIMETRY_xvYCC709:
            case COMBO_COLORIMETRY_ITU709:
            case COMBO_COLORIMETRY_ADOBERBG:
                {
                    if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                    {
                        pCoeff = _y2rBT709_LimitRange;
                        pYCbCrOffset = _ycbcrOffset[2];
                        HDR_PrintMsg("Source is BT709 limit Range");
                    }
                    else
                    {//others are full ragne
                        pCoeff = _y2rBT709_FullRange;
                        pYCbCrOffset = _ycbcrOffset[1];
                        HDR_PrintMsg("Source is BT709 full Range");
                    }
                }
                break;

            case COMBO_COLORIMETRY_xvYCC601:
            case COMBO_COLORIMETRY_sYCC601:
            case COMBO_COLORIMETRY_ADOBEYCC601:
            case COMBO_COLORIMETRY_ITU601:
                {
                    if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                    {
                        pCoeff = _y2rBT601_LimitRange;
                        pYCbCrOffset = _ycbcrOffset[4];
                        HDR_PrintMsg("Source is xvycc601 limit Range");
                    }
                    else
                    {//others are full ragne
                        pCoeff = _y2rBT601_FullRange;
                        pYCbCrOffset = _ycbcrOffset[3];
                        HDR_PrintMsg("Source is xvycc601 full Range");
                    }
                }
                break;
            //BT2020 CL
            case COMBO_COLORIMETRY_BT2020YcCbcCrc:
                {
                    if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                    {
                              pCoeff = _y2rBT2020CL_LimitRange;
                              pYCbCrOffset = _ycbcrOffset[6];
                              HDR_PrintMsg("Source is BT2020NCL limit Range");
                    }else{//others are full ragne
                              pCoeff = _y2rBT2020CL_FullRange;
                              pYCbCrOffset = _ycbcrOffset[5];
                              HDR_PrintMsg("Source is BT2020NCL full Range");
                    }
                }break;
            //BT2020 NCL
            case COMBO_COLORIMETRY_BT2020RGBYCbCr:{
                if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                {
                      pCoeff = _y2rBT2020NCL_LimitRange;
                              pYCbCrOffset = _ycbcrOffset[6];
                      HDR_PrintMsg("Source is BT2020NCL limit Range");
                }
                else
                {//others are full ragne
                      pCoeff = _y2rBT2020NCL_FullRange;
                      pYCbCrOffset = _ycbcrOffset[5];
                      HDR_PrintMsg("Source is BT2020NCL full Range");
                }
            }break;

        }
    }

    //Y2R round
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7B_0E, BIT1, BIT1);
    //Y2R shift
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7B_0F, BIT0, BIT0|BIT1);

    for(idx = 0; idx < 9 ; idx++)
    {
        coeff[idx] = pCoeff[idx] * 0x2000;
    }

    //3x3 coefficents
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_10, coeff[1]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_12, coeff[2]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_14, coeff[0]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_16, coeff[4]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_18, coeff[5]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_1A, coeff[3]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_1C, coeff[7]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_1E, coeff[8]);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_20, coeff[6]);

    //YCbCr offset
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7B_22, pYCbCrOffset[0]);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7B_26, pYCbCrOffset[1]);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7B_2A, pYCbCrOffset[2]);

    //range min
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_2E, 0x0000);
     //range max
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_30, 0xFFFF);
    //inv
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7B_34, 0x00001);
    //force disable IP2 CSC
    mdrv_HDR_DS_W2B(u32BaseAddr + SC22_56, 0x0000);

	 //overscan with Green line
    if (cf.ucColorType  == COMBO_COLOR_FORMAT_RGB)
    {
    	mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_06, BIT7, BIT7);
    }
    else
    {
    	mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_06, 0x00, BIT7);
    }

    HDR_PrintData("Pixel Format(RGB/422/444/420) = %d", cf.ucColorType);
    HDR_PrintData("Color Range(Dft/Limit/Full) = %d", cf.ucColorRange);
    HDR_PrintData("Yuv Colorimetry(601/709/Nodata/EC) = %d", cf.ucYuvColorimetry);
    HDR_PrintData("Extend Colorimetry(601/709/s601/adobe601/adobeRGB/2020CL/2020NCL) = %d", cf.ucColorimetry);
}

//GM 3x3
void ConfigB01_CSC(BYTE u8WinIdx)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    int idx = 0;
    WORD byp3x3[9] = {0};
    Matrix3x3 m;
    m.IsValid = TRUE;
    m.matrix[0][0] = 1.0;
    m.matrix[0][1] = 0.0;
    m.matrix[0][2] = 0.0;
    m.matrix[1][0] = 0.0;
    m.matrix[1][1] = 1.0;
    m.matrix[1][2] = 0.0;
    m.matrix[2][0] = 0.0;
    m.matrix[2][1] = 0.0;
    m.matrix[2][2] = 1.0;
    FormatMatrixToS1D14(m, byp3x3);
    //change CSC format to S1.14
    mdrv_HDR_DS_WBit(u32BaseAddr + SC7B_0F, FALSE, BIT2);
    mdrv_HDR_DS_WBit(u32BaseAddr + SC7B_0F, TRUE, BIT3);
    for(idx = 0; idx < 9 ; idx++)
    {
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_40 + idx * 2, byp3x3[idx]);
    }
}

//Full ragne RGB to full range YCbCr
void ConfigB01_R2Y(BYTE u8WinIdx)
{
    int idx = 0;
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    WORD p3x3[9] = {0};
    const float* pY2R = NULL;
    WORD y2r_matrix[9] = {0};
    ST_COMBO_COLOR_FORMAT cf;
    cf = GetHDRColorFormat();

    if(cf.ucColorType == COMBO_COLOR_FORMAT_RGB)
    {
        if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
        {
            HDR_PrintMsg("Source is RGB limit Range");
        }else{//others are full ragne
            HDR_PrintMsg("Source is RGB full Range");
        }
        pY2R = _r2yBT601_FullRange;
    }else{ //YCbCr
        switch((int)cf.ucColorimetry)
        {
            default:
            case COMBO_COLORIMETRY_NONE:
            case COMBO_COLORIMETRY_xvYCC709:
            case COMBO_COLORIMETRY_ITU709:
            case COMBO_COLORIMETRY_ADOBERBG:
            case COMBO_COLORIMETRY_BT2020YcCbcCrc:
                {
                     pY2R = _r2yBT709_FullRange;
                     HDR_PrintMsg("Source is BT709 full Range");
                }
                break;

            //xvycc 601 and xvycc709 are need to check its coefficents
            case COMBO_COLORIMETRY_xvYCC601:
            case COMBO_COLORIMETRY_sYCC601:
            case COMBO_COLORIMETRY_ADOBEYCC601:
            case COMBO_COLORIMETRY_ITU601:
                {
                    pY2R = _r2yBT601_FullRange;
                    HDR_PrintMsg("Source is BT601 full Range");
                }
                break;

            case COMBO_COLORIMETRY_BT2020RGBYCbCr:
                {
                    pY2R = _r2yBT2020NCL_FullRange;
                    HDR_PrintMsg("Source is BT2020NCL full Range");
                }
                break;
        }
    }

    if(NULL != pY2R)
    {
        for(idx = 0; idx < 9; idx++)
        {
            y2r_matrix[idx] = (WORD)(pY2R[idx] * 0x1000);
        }
        p3x3[0] = y2r_matrix[3] ;
        p3x3[1] = y2r_matrix[4] ;
        p3x3[2] = y2r_matrix[5] ;
        p3x3[3] = y2r_matrix[6] ;
        p3x3[4] = y2r_matrix[7] ;
        p3x3[5] = y2r_matrix[8] ;
        p3x3[6] = y2r_matrix[0] ;
        p3x3[7] = y2r_matrix[1] ;
        p3x3[8] = y2r_matrix[2] ;
    }

    //R2Y round
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7B_0E, BIT3, BIT3);
    //R2Y shift
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7B_0F, BIT4, BIT4|BIT5);

    for(idx = 0; idx < 9 ; idx++)
    {
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_52 + idx * 2, p3x3[idx]);
    }
    //Y/Cb/Cr offset
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_64, 0x0000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_66, 0x4000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_68, 0x4000);

    //clip min
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_6A, 0x0000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_6C, 0x8001);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_6E, 0x8001);

     //clip max
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_70, 0x7FFF);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_72, 0x7FFF);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_74, 0x7FFF);

    HDR_PrintData("Pixel Format(RGB/422/444/420) = %d", cf.ucColorType);
    HDR_PrintData("Color Range(Dft/Limit/Full) = %d", cf.ucColorRange);
    HDR_PrintData("Yuv Colorimetry(601/709/Nodata/EC) = %d", cf.ucYuvColorimetry);
    HDR_PrintData("Extend Colorimetry(601/709/s601/adobe601/adobeRGB/2020CL/2020NCL) = %d", cf.ucColorimetry);
}

//BT709 Full range to Full RGB
void VOPY2R(BYTE u8WinIdx, BOOL IsEnable)
{
    int idx = 0;
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    ST_COMBO_COLOR_FORMAT cf;
    cf = GetHDRColorFormat();
    if(IsEnable)
    {
        #if HDR_3DLUT
         //limit range to full RGB 709
        //WORD limitYTofullRGB_BT709[9] = {0x0731, 0x04AC, 0x0000,
        //                               0x1DDD, 0x04AC, 0x1F25,
        //                               0x0000, 0x04AC, 0x0879};
        //limit range to full RGB 601
        WORD limitYTofullRGB_BT601[9] = {0x0667, 0x04AC, 0x0000,
                                                                  0x1CBD, 0x04AC, 0x1E6E,
                                                                  0x0000, 0x04AC, 0x0818};

        WORD *p3x3 = limitYTofullRGB_BT601;
        float* pY2R = NULL;
        if(cf.ucColorType == COMBO_COLOR_FORMAT_RGB)
        {
            if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
            {
                HDR_PrintMsg("Source is RGB limit Range");
            }else{//others are full ragne
                HDR_PrintMsg("Source is RGB full Range");
            }
            pY2R = _y2rBT601_LimitRange;
        }
	  else
	  { //YCbCr

            switch((int)cf.ucColorimetry)
            {
                default:
                case COMBO_COLORIMETRY_NONE:
                case COMBO_COLORIMETRY_xvYCC709:
                case COMBO_COLORIMETRY_ITU709:
                case COMBO_COLORIMETRY_ADOBERBG:
                    {
                         pY2R = _y2rBT709_LimitRange;
                         HDR_PrintMsg("Source is BT709 Limit Range");
                    }
                    break;

                //xvycc 601 and xvycc709 are need to check its coefficents
                case COMBO_COLORIMETRY_xvYCC601:
                case COMBO_COLORIMETRY_sYCC601:
                case COMBO_COLORIMETRY_ADOBEYCC601:
                case COMBO_COLORIMETRY_ITU601:
                    {
                        pY2R = _y2rBT601_LimitRange;
                        HDR_PrintMsg("Source is BT601 Limit Range");
                    }
                    break;
                //BT2020 CL
                case COMBO_COLORIMETRY_BT2020YcCbcCrc:
                    {
                        pY2R = _y2rBT2020CL_LimitRange;
                        HDR_PrintMsg("Source is BT2020CL Limit Range");
                    }
                    break;
                //BT2020 NCL
                case COMBO_COLORIMETRY_BT2020RGBYCbCr:
                    {
                        pY2R = _y2rBT2020NCL_LimitRange;
                        HDR_PrintMsg("Source is BT2020NCL Limit Range");
                    }
                    break;
                }
        }

        if(NULL != pY2R)
        {
            for(idx = 0; idx < 9 ; idx++)
            {
                p3x3[idx] = (WORD)(pY2R[idx] * 0x400);
            }
        }
        for(idx = 0; idx < 9 ; idx++)
        {
            //Greg : TBD
            //msWrite2Byte(u32BaseAddr + SC10_4C + idx * 2,  p3x3[idx]);
        }

        //Greg : TBD
        //Matrix Enable
        //msWriteByte(u32BaseAddr + SC10_5E, 0x35);
        //enable Y sub16
        //msWriteByteMask(u32BaseAddr + SC0F_AE, BIT6, BIT6);
        #else
        WORD fullYTofullRGB_BT601[9] = {0x059C, 0x0400, 0x0000,
                                                                0x1D25, 0x0400, 0x1EA0,
                                                                0x0000, 0x0400, 0x0717};

        WORD *p3x3 = fullYTofullRGB_BT601;
        const float* pY2R = NULL;
        if(cf.ucColorType == COMBO_COLOR_FORMAT_RGB)
        {
            if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
            {
                HDR_PrintMsg("Source is RGB limit Range");
            }else{//others are full ragne
                HDR_PrintMsg("Source is RGB full Range");
            }
            pY2R = _y2rBT601_FullRange;
        }
	  else
	  { //YCbCr

            switch((int)cf.ucColorimetry)
            {
                default:
                case COMBO_COLORIMETRY_NONE:
                case COMBO_COLORIMETRY_xvYCC709:
                case COMBO_COLORIMETRY_ITU709:
                case COMBO_COLORIMETRY_ADOBERBG:
                    {
                         pY2R = _y2rBT709_FullRange;
                         HDR_PrintMsg("Source is BT709 Limit Range");
                    }
                    break;

                //xvycc 601 and xvycc709 are need to check its coefficents
                case COMBO_COLORIMETRY_xvYCC601:
                case COMBO_COLORIMETRY_sYCC601:
                case COMBO_COLORIMETRY_ADOBEYCC601:
                case COMBO_COLORIMETRY_ITU601:
                    {
                        pY2R = _y2rBT601_FullRange;
                        HDR_PrintMsg("Source is BT601 Limit Range");
                    }
                    break;
                //BT2020 CL
                case COMBO_COLORIMETRY_BT2020YcCbcCrc:
                    {
                        pY2R = _y2rBT2020CL_FullRange;
                        HDR_PrintMsg("Source is BT2020CL Limit Range");
                    }
                    break;
                //BT2020 NCL
                case COMBO_COLORIMETRY_BT2020RGBYCbCr:
                    {
                        pY2R = _y2rBT2020NCL_FullRange;
                        HDR_PrintMsg("Source is BT2020NCL Limit Range");
                    }
                    break;
                }
        }

        if(NULL != pY2R)
        {
            for(idx = 0; idx < 9 ; idx++)
            {
                p3x3[idx] = (WORD)(pY2R[idx] * 0x400);
            }
        }

        if (u8WinIdx == 0)
        {
            for(idx = 0; idx < 9 ; idx++)
            {
                mdrv_HDR_DS_W2BMask(SC10_4C + idx * 2, p3x3[idx], 0x1FFF);
            }
            //Main Window Matrix Enable
            mdrv_HDR_DS_WB(SC10_5E, 0x35); //Full ragnge Y2R
            mdrv_HDR_DS_WBMask(SC0F_AE,0 ,BIT6); //Y sub 16 disable

            HDR_PrintMsg("Main Window Matrix Enable Y2R");
        }else{
            for(idx = 0; idx < 9 ; idx++)
            {
                mdrv_HDR_DS_W2B(SC0F_3A + idx * 2, p3x3[idx]);
            }
	    HDR_PrintMsg("Sub Window Matrix Enable Y2R");
            //Sub Window Matrix Enable
            mdrv_HDR_DS_WB(SC0F_4C, 0x35);//Full ragnge Y2R
            mdrv_HDR_DS_WBMask(SC0F_AE,0 ,BIT7); //Y sub 16 disable
        }
#endif
        //Greg : TBD disable r2y
        //Disalbe IP2 R2Y
        //msWriteByte(u32BaseAddr + SC02_56, 0x00);
        //msWriteByte(u32BaseAddr + SC02_57, 0x00);
        //msWriteByte(u32BaseAddr + SC02_80, 0x00);
    }else{
        //Matrix Disable, this is a patch need to check
        if (((cf.ucColorType  == COMBO_COLOR_FORMAT_RGB) &&
           (cf.ucColorRange != COMBO_COLOR_RANGE_LIMIT)) || (msReadByte(u32BaseAddr + SC22_56) == (BIT0|BIT2|BIT4 |BIT5)  ) )
        {
            //disable Y2R colomatrix only in case RGB mode
            if (u8WinIdx == 0)
            {
                if ((msReadByte(u32BaseAddr + SC07_40) & BIT0) == 0)
                {
                    mdrv_HDR_DS_WB(u32BaseAddr + SC10_5E, 0);
		    HDR_PrintMsg("Main Window Matrix disEnable Y2R");
                }
            }else{
                if ((msReadByte(u32BaseAddr + SC07_40) & BIT4) == 0)
                {
                    mdrv_HDR_DS_WB(u32BaseAddr + SC0F_4C, 0);
		    HDR_PrintMsg("sub Window Matrix disEnable Y2R");
                }
            }
        }
    }

     //overscan with Green line
    if ((IsEnable == TRUE)&&(cf.ucColorType  == COMBO_COLOR_FORMAT_RGB))
    {
        //Greg : TBD
    	//msWriteByteMask(u32BaseAddr + SC02_56, BIT4, BIT4);
    	mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_06, BIT7, BIT7);
    }
    else
    {
        ////Greg : TBD
    	//msWriteByteMask(u32BaseAddr + SC02_56, 0x00, BIT4);
    	mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_06, 0x00, BIT7);
    }
}

//extern EN_COMBO_IP_TYPE mapi_combo_GetComboType(EN_COMBO_IP_SELECT ucComboSelect);
void EnableHDRIP(BYTE u8WinIdx, BOOL IsOn)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    ST_COMBO_COLOR_FORMAT cf;
    cf = GetHDRColorFormat();
    UNUSED(cf);
    if(TRUE == IsOn)
    {
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_0D, 0, BIT7);
        //ip2 path en
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_0E, BIT1, BIT1);

        //Greg : HDMI driver do 422 to 444 automatically, so no need to do 422
        // to 444 in HDR IP.
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_0E, BIT4|BIT5, BIT4|BIT5);
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_03, 0x00, BIT7);
    }
    else
    {
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_0D, BIT7, BIT7);
        //ip2 path en
        mdrv_HDR_DS_WB(u32BaseAddr + SC79_0E, 0x00);
    }
    //hdr open standard en
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_0F, BIT7, BIT7);
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_07, 0x00, BIT6|BIT7);
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC79_09, BIT2|BIT0, BIT2|BIT0);
}

//TMO tmo_ingain
void ConfigB01_07(BYTE u8WinIdx, WORD adjChromaWeight)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_7E, adjChromaWeight);
}

//TMO
void BypassB02(BYTE u8WinIdx, BOOL IsBypass)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    if(IsBypass)
    {
        msWriteByteMask(u32BaseAddr + SC7A_04, BIT2, BIT2);
    }else{
        msWriteByteMask(u32BaseAddr + SC7A_04, 0, BIT2);
    }
}

//Referenced CFD only has YFull to RFull case. other case is not supported.
//therefore the coefficients are define as constant.
void ConfigB02_YFullToRFull(BYTE u8WinIdx)
{
    xdata ST_COMBO_COLOR_FORMAT cf;
    xdata DWORD u32BaseAddr;
    cf = GetHDRColorFormat();
    u32BaseAddr = GetHDRBaseAddr(u8WinIdx);

    if ((cf.ucColorimetry == COMBO_COLORIMETRY_ITU601) ||(cf.ucColorType == COMBO_COLOR_FORMAT_RGB)) //White170815 need confirm
    {
        reg_b02_y2r_coef02 = 0x005A;
        reg_b02_y2r_coef11 = 0x01EA;
        reg_b02_y2r_coef12 = 0x01D2;
        reg_b02_y2r_coef21 = 0x0071;
        reg_b02_y2r_off0 = 0x00005A00;
        reg_b02_y2r_off1 = 0x000FBC00;
        reg_b02_y2r_off2 = 0x00007100;
    }else if ((cf.ucColorimetry == COMBO_COLORIMETRY_BT2020RGBYCbCr))
    {
        reg_b02_y2r_coef02 = 0x005E;
        reg_b02_y2r_coef11 = 0x01F5;
        reg_b02_y2r_coef12 = 0x01DB;
        reg_b02_y2r_coef21 = 0x0078;
        reg_b02_y2r_off0 = 0x00005E00;
        reg_b02_y2r_off1 = 0x000FD000;
        reg_b02_y2r_off2 = 0x00007800;
    }
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_E8, reg_b02_y2r_coef02);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_EA, reg_b02_y2r_coef11);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_EC, reg_b02_y2r_coef12);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_EE, reg_b02_y2r_coef21);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_F0, reg_b02_y2r_off0);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_F4, reg_b02_y2r_off1);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_F8, reg_b02_y2r_off2);
}

void ConfigB04(BYTE u8WinIdx)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_80, 0x0000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_82, 0x07FF);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_84, _satGain);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_86, _hk_high_th);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_8A, 0x00000000);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_90, 0x00000000);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_94, 0xB800);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_96, 0x0000);
}

void _hdr3dLutSettings_b502W(BYTE u8WinIdx,  BYTE bIsOn, WORD u16iMaxC, WORD u16iMinC, DWORD u32iDistiCInv, WORD u16iMinY, WORD u16iMaxY, DWORD u32iDistiYInv, BYTE bAlphaRound, BYTE u8OutControl, BYTE bOutControlRound, BYTE bLutBlendMode, BYTE b3dLutDitherMode)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    if(0 == bIsOn)
    {
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_04, 0x10, 0x10);
    }
    else
    {
        #if HDR_3DLUT
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_04,0x00,0x10); //reg_b06_byp_en
        #else
        mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_04,0x10,0x10); //reg_b06_byp_en
        #endif
    }
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_A0,u16iMaxC);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_A2,u16iMinC);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_A4,u32iDistiCInv);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_A8,u16iMinY);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_AA,u16iMaxY);
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_AC,u32iDistiYInv);
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_A0,0x01,bAlphaRound);
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_A0,0x06,u8OutControl<<1);
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_A0,0x08,bOutControlRound<<3);
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_A0,0x08,bLutBlendMode<<3);
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7A_A0,0x10,b3dLutDitherMode<<4);
    #if HDR_3DLUT
    //WriteDMTable(u8WinIdx, HDR_3DLutTbl, (DWORD*)Get3DLutTable(), FALSE);
    #else
    //UNUSED(u8WinIdx);
    #endif
}

// 3D LUT, BK7A_50~BK7A_59
void ConfigB05(BYTE u8WinIdx)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    //below is referanced from CFD(ursa)
    BYTE bIsOn = TRUE;
    BYTE bWriteRegister = TRUE;
    BYTE u8Mode = 2;
    WORD u16iMaxC;
    WORD u16iMinC;
    DWORD u32iDistiCInv;
    WORD u16iMinY;
    WORD u16iMaxY;
    DWORD u32iDistiYInv;
    BYTE bAlphaRound;
    BYTE u8OutControl;
    BYTE bOutControlRound;
    BYTE bLutBlendMode;
    BYTE b3dLutDitherMode;
    //imaxc
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_A0, 0x3FFF);
    //iminc
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_A2, 0x0000);
    //idistc_invs
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_A4, 0x00000000);
    //iminy
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_A8, 0x0000);
    //imaxy
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_AA, 0x7FFF);
    //idisty_invs
    mdrv_HDR_DS_W4B(u32BaseAddr + SC7A_AC, 0x00000000);
    //b05 controls
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_B0, 0x0024);
    //uv_post_ofs
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7A_B2, 0x0800);

    switch(u8Mode)
    {
        case 0:
            u16iMaxC = 0x7FFF;
            u16iMinC = 0x0000;
            u32iDistiCInv = 0x00008000;
            u16iMinY = 0x0000;
            u16iMaxY = 0x7FFF;
            u32iDistiYInv = 0X00008000;
            bAlphaRound = 0;
            u8OutControl = 2;
            bOutControlRound = 0;
            bLutBlendMode = 0;
            b3dLutDitherMode = 0;
            break;
        case 1:
            u16iMaxC = 0x30C6;
            u16iMinC = 0xCF3A;
            u32iDistiCInv = 0x0000A7F5;
            u16iMinY = 0x01EE;
            u16iMaxY = 0x4108;
            u32iDistiYInv = 0X000103A5;
            bAlphaRound = 0;
            u8OutControl = 2;
            bOutControlRound = 0;
            bLutBlendMode = 0;
            b3dLutDitherMode = 0;
            break;
        case 2: //bypass setting
            u16iMaxC = 0x3FFF;
            //u16iMinC = 0x0000;
            u16iMinC = 0xC001;
            u32iDistiCInv = 0x00008000;
            u16iMinY = 0x0000;
            u16iMaxY = 0x7FFF;
            u32iDistiYInv = 0X00008000;
            bAlphaRound = 0;
            u8OutControl = 2;
            bOutControlRound = 0;
            bLutBlendMode = 0;
            b3dLutDitherMode = 0;
            break;
        default:
            return;
    }

    if(1 == bWriteRegister)
    {
        _hdr3dLutSettings_b502W(u8WinIdx, bIsOn, u16iMaxC,  u16iMinC,  u32iDistiCInv,  u16iMinY,  u16iMaxY,  u32iDistiYInv,  bAlphaRound,  u8OutControl,  bOutControlRound,  bLutBlendMode,  b3dLutDitherMode);
    }

}

//Spatio-Temporal Dithering
#if HDR_Uncalled_Segment
void BypassB06(BYTE u8WinIdx, BYTE IsBypass)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    if(IsBypass)
    {
        msWriteByteMask(u32BaseAddr + SC7A_04, BIT6, BIT6);
    }else{
        msWriteByteMask(u32BaseAddr + SC7A_04, 0, BIT6);
    }
}
#endif

WORD getIP22ndImageWidth(BYTE u8WinIdx)
{
    //Greg : Win Size API
    UNUSED(u8WinIdx);
    return 1920;
}

WORD getIP22ndImageHeight(BYTE u8WinIdx)
{
    //Greg : Win Size API
    UNUSED(u8WinIdx);
    return 1080;
}

void msSetHDRWindowSize(BYTE u8WinIdx, WORD width, WORD height)
{
    xdata DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);//SCRegBase(MapWin2ScIndex(u8WinIdx));
    mdrv_HDR_DS_W2B(u32BaseAddr + SC79_10, width);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC79_12, height);
    HDR_PrintData("API HDR Win Width: %d", width);
    HDR_PrintData("API HDR Win height: %d", height);
    UNUSED(u8WinIdx);
}

BOOL ConfigHDRWindow(BYTE u8WinIdx)
{
    //DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    WORD width = getIP22ndImageWidth(u8WinIdx);
    WORD height =getIP22ndImageHeight(u8WinIdx);

    //ST_WINDOW_INFO sAutoWin = mStar_ReadAutoWindow(MapWin2Port(u8WinIdx));
    //width = sAutoWin.width;
    //height = sAutoWin.height;

    if ((height == 0) ||  (width == 0))
    {
        return FALSE;
    }
    else
    {
        //msSetHDRWindowSize(u8WinIdx, width, height);
        return TRUE;
    }
}


void API_InitHDR(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
}

void API_LoadDeGammaTabel(BYTE u8WinIdx, const void* pTable)
{
    if (NULL == pTable)
    {
        HDR_PrintMsg("HDR : DeGamma table is empty.");
        return;
    }

    HDR_PrintMsg("HDR: Degamma table");
    WriteDMTable(u8WinIdx, HDR_DeGammaTbl, (DWORD*)pTable, TRUE);
}


void API_LoadGammaTable(BYTE u8WinIdx, const void* pTable)
{
    if (NULL == pTable)
    {
        HDR_PrintMsg("HDR : Gamma table is empty.");
        return;
    }
    HDR_PrintMsg("HDR : Gamma table.");
    WriteDMTable(u8WinIdx, HDR_GammaTbl, (DWORD*)pTable, TRUE);

}

void API_LoadOOTFTable(BYTE u8WinIdx, const void* pTable)
{
    if (NULL == pTable)
    {
        HDR_PrintMsg("HDR : OOTF  table is empty.");
        return;
    }
    HDR_PrintMsg("HDR : OOTF table.");
    WriteDMTable(u8WinIdx, HDR_OOTF, (DWORD*)pTable, TRUE);

}


#if HDR_Uncalled_Segment

void API_LoadPanelGammaTable(BYTE u8WinIdx, BYTE **pu8Table)
{
    if (NULL == pu8Table)
    {
        //HDR_PrintMsg("HDR : Panel Gamma table is empty.");
        return;
    }
    //HDR_PrintMsg("HDR : Panel Gamma table.");
    WriteDMTable(u8WinIdx, HDR_GammaTbl, (DWORD*)pu8Table, TRUE);
}

void API_LoadTMOCurve(BYTE u8WinIdx, void* pTable)
{
    if(NULL == pTable)
    {
        HDR_PrintMsg("HDR  : tmo table is empty.");
        return;
    }
    WriteDMTable(u8WinIdx, HDR_ToneMappingTbl, (DWORD*)pTable, TRUE);
}
#endif

void API_CSC(BYTE u8WinIdx, HDR_CSC cscType)
{
    switch(cscType)
    {
        case HDR_Y2R:{ConfigB01_Y2R(u8WinIdx);} break;
        case HDR_R2Y:{ConfigB01_R2Y(u8WinIdx);} break;
        case VOP_Y2R:{VOPY2R(u8WinIdx, TRUE);};
    }
}

void API_LoadColorMatrix( BYTE u8WinIdx, Matrix3x3 m)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    int idx= 0;
    WORD fmtMatrix[9] = {0};
    DWORD cmAddr = SC25_04;
    ConfigB01_CSC(u8WinIdx);

    if( u8WinIdx > 0)
    {
        cmAddr = SC25_24;
    }

    if (FormatMatrixToS2D10(m, fmtMatrix))
    {
        HDR_PrintMsg("HDR  : color matrix 3x3");
        for (idx = 0; idx < 9; idx++)
        {
            mdrv_HDR_DS_W2B(cmAddr + idx * 2, fmtMatrix[idx]);
            HDR_PrintData("0x%x ", fmtMatrix[idx]);
        }
    }
    mdrv_HDR_DS_WBit(SC25_02, TRUE, BIT1);
    //csa2csb round
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7B_0E, BIT2, BIT2);
    //csa2csb shift
    mdrv_HDR_DS_WBMask(u32BaseAddr + SC7B_0F, BIT3, BIT2|BIT3);
    //Write Matrix
    //WriteColorMatrix(u8WinIdx, fmtMatrix);
    //offset
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_80, 0x0000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_82, 0x0000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_84, 0x0000);

    //clip min
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_86, 0x0000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_88, 0x0000);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_8A, 0x0000);

    //clip max
    //greg ?? why the defalut value of clip max is 0x9C40.
    //chage to 0xFFFF to avoid color abnormal in bypass mode.
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_8C, 0xFFFF);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_8E, 0xFFFF);
    mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_90, 0xFFFF);
}

void LoadXVyccLUT( BYTE u8WinIdx)
{
#if ENABLE_LUT_AUTODOWNLOAD
    WriteXVYCCTable(u8WinIdx);
#else
    msWriteDeGammaTbl(u8WinIdx,0);
#if XVYCC_GAMMA_EXT
    msWriteFixGammaTbl(u8WinIdx,0,1);
#else
    msWriteFixGammaTbl(u8WinIdx,0,0);
#endif
    msSetFGammaOnOff(u8WinIdx,1);
    msSetFixGammaOnOff(u8WinIdx,1);
#endif

}

void API_OpenHDRMode(BYTE u8WinIdx)
{
    //msPQ_BypassPQ(u8WinIdx, TRUE);
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    //msDrvSCCommonWrite(u16ScalerMask, TRUE);
    //_r2yEn = msReadByte(SC07_40);
    ConfigHDRWindow(u8WinIdx);
    //ConfigB01_CSC(u8WinIdx);
    ConfigB01_07(u8WinIdx, 0x0000);
    ConfigB02_YFullToRFull(u8WinIdx);
    ConfigB04(u8WinIdx);
    ConfigB05(u8WinIdx);
    BypassB02(u8WinIdx, TRUE);
    mStar_WaitForDataBlanking();
    EnableHDRIP(u8WinIdx, TRUE);
    mdrv_HDR_DS_WBMask(SC7A_02,BIT7,BIT7); //reg_uvc_byp_en
    //R2Y
    //msWriteByte(SC07_40, 0x00);
    //API_CSC(u8WinIdx, VOP_Y2R);
    //Enable XVYCC degamma/fixgamna/color matrix
   /* if(u8WinIdx)
    {
        msWriteBit(SC25_22, TRUE, _BIT2|_BIT1|_BIT0);
    }
    else{
        msWriteBit(SC25_02, TRUE, _BIT2|_BIT1|_BIT0);
    }
*/
    //msDrvSCCommonWrite(u16ScalerMask, FALSE);
}

void API_CloseHDRMode(BYTE u8WinIdx)
{
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    //msDrvSCCommonWrite(u16ScalerMask, TRUE);
    //EN_WINDOW_COLOR  eColor = eWIN_COLOR_RGB;
    //msAPIWinGetColorSpaceWindow(u8WinIdx, &eColor);

    //disable HDR IP
    EnableHDRIP(u8WinIdx, FALSE);
    if (MAIN_WINDOW == u8WinIdx)
    {
        //Set Main Window FixGamma, CM, DeGamma Off 
        mdrv_HDR_DS_WBMask(SC25_02, 0x00, BIT0|BIT1|BIT2);
        //Set Main Window PostGamma Off 
        mdrv_HDR_DS_WBMask(SC10_A0, 0x00, _BIT0);
    }
    else
    {
        //Set Sub Window FixGamma, CM, DeGamma Off 
        mdrv_HDR_DS_WBMask(SC25_22, 0x00, BIT0|BIT1|BIT2);
        //Set Sub Window PostGamma Off      
        mdrv_HDR_DS_WBMask(SC0F_AF, 0x00, _BIT4);
    }  
    //BypassB01(u8WinIdx, TRUE);

    //if (_r2yEn != 0xFF)
    //{
    //    msWriteByte(SC07_40, _r2yEn);
    //}
    //VOPY2R(u8WinIdx, FALSE);
    //dlc
    //msDrvSetDLCEnable(u8WinIdx, FALSE, eColor);
    //msDrvSCCommonWrite(u16ScalerMask, FALSE);
}

BOOL API_CheckWindowSize(BYTE u8WinIdx)
{
    return ConfigHDRWindow(u8WinIdx);
}

void API_ConfigB04(BYTE u8WinIdx, WORD gain, DWORD highTh)
{
    _satGain = gain;
    _hk_high_th = highTh;
    ConfigB04(u8WinIdx);
}

//Cofing B01 CSC as RGB Gain, Range is 0~8
void API_SetHDREOTFGain(BYTE u8WinIdx, float gain)
{
    DWORD u32BaseAddr = GetHDRBaseAddr(u8WinIdx);
    int idx = 0;
    WORD val = 0;
    WORD gain3x3[9] = {0};
    if (gain < 0)
    {
        gain = 0;
    }
    if (gain > 8)
    {
        gain = 8;
    }
    val = 0x1000 * gain;
    if (val > 0x7FFF)
    {
        val = 0x7FFF;
    }
    gain3x3[0] = val;
    gain3x3[1] = 0;
    gain3x3[2] = 0;
    gain3x3[3] = 0;
    gain3x3[4] = val;
    gain3x3[5] = 0;
    gain3x3[6] = 0;
    gain3x3[7] = 0;
    gain3x3[8] = val;
    //change CSC format to S3.12
    mdrv_HDR_DS_WBit(u32BaseAddr + SC7B_0F, FALSE, BIT2);
    mdrv_HDR_DS_WBit(u32BaseAddr + SC7B_0F, FALSE, BIT3);
    for(idx = 0; idx < 9 ; idx++)
    {
        mdrv_HDR_DS_W2B(u32BaseAddr + SC7B_40 + idx * 2, gain3x3[idx]);
    }
}
//#endif

void msSetHDRInRange(BYTE u8WinIdx, WORD u16YOffset, WORD u16YGain, WORD u16COffset, WORD u16CGain)
{
    UNUSED(u8WinIdx);
    mdrv_HDR_DS_W2BMask(SC7B_E0, u16YOffset, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_E2, u16YGain, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_E4, u16COffset, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_E6, u16CGain, 0xFFFF);
}

void msSetHDRY2RBypass(BYTE u8WinIdx, BOOL u8IsBypass)
{
    UNUSED(u8WinIdx);
    if(u8IsBypass)
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, BIT2, BIT2);
    }
    else
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, 0, BIT2);
    }
}

void msSetHDRCSCBypass(BYTE u8WinIdx, BOOL u8IsBypass)
{
    UNUSED(u8WinIdx);
    if(u8IsBypass)
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, BIT4, BIT4);
    }
    else
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, 0, BIT4);
    }
}

void msSetHDRR2YBypass(BYTE u8WinIdx, BOOL u8IsBypass)
{
    UNUSED(u8WinIdx);
    if(u8IsBypass)
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, BIT6, BIT6);
    }
    else
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, 0, BIT6);
    }
}

void msSetHDRDeGammaBypass(BYTE u8WinIdx, BOOL u8IsBypass)
{
	UNUSED(u8WinIdx);
	if(u8IsBypass)
	{
		msWrite2ByteMask(SC7A_02, BIT3, BIT3);
	}
	else
	{
		msWrite2ByteMask(SC7A_02, 0, BIT3);
	}
}

void msSetHDRGammaBypass(BYTE u8WinIdx, BOOL u8IsBypass)
{
	UNUSED(u8WinIdx);
	if(u8IsBypass)
	{
		msWrite2ByteMask(SC7A_02, BIT5, BIT5);
	}
	else
	{
		msWrite2ByteMask(SC7A_02, 0, BIT5);
	}
}

void msSetHDRUVCBypass(BYTE u8WinIdx, BOOL u8IsBypass)
{
    UNUSED(u8WinIdx);
    if(u8IsBypass)
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, BIT7, BIT7);
    }
    else
    {
        mdrv_HDR_DS_W2BMask(SC7A_02, 0, BIT7);
    }
}

void msSetHDRTMOBypass(BYTE u8WinIdx, BOOL u8IsBypass)
{
    UNUSED(u8WinIdx);
    if(u8IsBypass)
    {
        mdrv_HDR_DS_W2BMask(SC7A_04, BIT2, BIT2);
    }
    else
    {
        mdrv_HDR_DS_W2BMask(SC7A_04, 0, BIT2);
    }
}

void msSetHDRY2R2YShift(BYTE u8WinIdx, WORD u16Ctrl, WORD u16Mask)
{
    UNUSED(u8WinIdx);
    mdrv_HDR_DS_W2BMask(SC7B_0E, u16Ctrl, u16Mask);
}

void msSetHDRY2R(BYTE u8WinIdx, WORD* pMatrix, DWORD u32Offset0, DWORD u32Offset1, DWORD u32Offset2, WORD u16Min, WORD u16Max, DWORD u32RangeInv)
{
	UNUSED(u8WinIdx);
 	BYTE u8Idx = 0;
	for(u8Idx = 0; u8Idx < 9; u8Idx++)
	{
		msWrite2ByteMask(SC7B_10 + u8Idx * 2, pMatrix[u8Idx], 0xFFFF);
	}

	msWrite2ByteMask(SC7B_22, u32Offset0 & 0xFFFF, 0xFFFF);
	msWrite2ByteMask(SC7B_24, u32Offset0 >> 16, 0xFFFF);
	msWrite2ByteMask(SC7B_26, u32Offset1 & 0xFFFF, 0xFFFF);
	msWrite2ByteMask(SC7B_28, u32Offset1 >> 16, 0xFFFF);
	msWrite2ByteMask(SC7B_2A, u32Offset2 & 0xFFFF, 0xFFFF);
	msWrite2ByteMask(SC7B_2C, u32Offset2 >> 16, 0xFFFF);
	msWrite2ByteMask(SC7B_2E, u16Min, 0xFFFF);
	msWrite2ByteMask(SC7B_30, u16Max, 0xFFFF);
	msWrite2ByteMask(SC7B_32, u32RangeInv & 0xFFFF, 0xFFFF);
	msWrite2ByteMask(SC7B_34, u32RangeInv >> 16, 0xFFFF);
}

void msSetHDRY2R_BT2020(BYTE u8WinIdx, BYTE u8IsEn, BYTE u8CHalf, WORD u16MCr, WORD u16MCb, DWORD u32CrOffset, DWORD u32CbOffset)
{
    UNUSED(u8WinIdx);
    WORD u16Ctrl = u8CHalf;
    if (u8IsEn)
    {
        u16Ctrl |= BIT15;
    }
    mdrv_HDR_DS_W2BMask(SC7B_D0, u16Ctrl, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_D2, u16MCr, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_D4, u16MCb, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_D6, u32CrOffset & 0xFFFF, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_D8, u32CrOffset >> 16, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_DA, u32CbOffset & 0xFFFF, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_DC, u32CbOffset >> 16, 0xFFFF);
}

void msSetHDRCSC(BYTE u8WinIdx, WORD* pMatrix, WORD* pOffset, WORD* pMin, WORD* pMax)
{
    UNUSED(u8WinIdx);
    BYTE u8Idx = 0;
    for(u8Idx = 0; u8Idx < 9; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_40 + u8Idx * 2, pMatrix[u8Idx], 0xFFFF);
    }

    for(u8Idx = 0; u8Idx < 3; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_80 + u8Idx * 2, pOffset[u8Idx], 0xFFFF);
    }

    for(u8Idx = 0; u8Idx < 3; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_86 + u8Idx * 2, pMin[u8Idx], 0xFFFF);
    }

    for(u8Idx = 0; u8Idx < 3; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_8C + u8Idx * 2, pMax[u8Idx], 0xFFFF);
    }
}

void msSetHDROOTFR2Y(BYTE u8WinIdx, WORD u16M0, WORD u16M1, WORD u16M2)
{
    UNUSED(u8WinIdx);
    mdrv_HDR_DS_W2BMask(SC7E_8A, u16M0, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7E_8C, u16M1, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7E_8E, u16M2, 0xFFFF);
}

void msSetHDRR2Y(BYTE u8WinIdx, WORD* pMatrix, WORD* pOffset, WORD* pMin, WORD* pMax)
{
    UNUSED(u8WinIdx);
    BYTE u8Idx = 0;
    for(u8Idx = 0; u8Idx < 9; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_52 + u8Idx * 2, pMatrix[u8Idx], 0xFFFF);
    }

    for(u8Idx = 0; u8Idx < 3; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_64 + u8Idx * 2, pOffset[u8Idx], 0xFFFF);
    }

    for(u8Idx = 0; u8Idx < 3; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_6A+ u8Idx * 2, pMin[u8Idx], 0xFFFF);
    }

    for(u8Idx = 0; u8Idx < 3; u8Idx++)
    {
        mdrv_HDR_DS_W2BMask(SC7B_70 + u8Idx * 2, pMax[u8Idx], 0xFFFF);
    }
}

void msSetHDROutRange(BYTE u8WinIdx, WORD u16YOffset, WORD u16YGain, WORD u16COffset, WORD u16CGain)
{
    UNUSED(u8WinIdx);
    mdrv_HDR_DS_W2BMask(SC7B_E8, u16YOffset, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_EA, u16YGain, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_EC, u16COffset, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC7B_EE, u16CGain, 0xFFFF);
}

void msSetHDR422To444To422(BYTE u8WinIdx, BOOL u8IsBypass444To422, WORD u16CtrlMd, BOOL u8IsEn422)
{
    UNUSED(u8WinIdx);
    if (u8IsBypass444To422)
    {
        mdrv_HDR_DS_W2BMask(SC79_4A, BIT0, BIT0);
    }
    else
    {
        mdrv_HDR_DS_W2BMask(SC79_4A, 0, BIT0);
    }

    mdrv_HDR_DS_W2BMask(SC79_4C, u16CtrlMd, 0xFFFF);

    if (u8IsEn422)
    {
        mdrv_HDR_DS_W2BMask(SC79_4E, BIT4, BIT4);
    }
    else
    {
        mdrv_HDR_DS_W2BMask(SC79_4E, 0, BIT4);
    }
}

void msSetHDR2IPPathEn(BYTE u8WinIdx, WORD u16Ctrl0, WORD u16Ctrl1)
{
    UNUSED(u8WinIdx);
    mdrv_HDR_DS_W2BMask(SC79_0C, u16Ctrl0, 0xFFFF);
    mdrv_HDR_DS_W2BMask(SC79_0E, u16Ctrl1, 0xFFFF);

    //Set HDR loacation at IP.
    mdrv_HDR_DS_W2BMask(SC79_4E, BIT0, BIT0);
    mdrv_HDR_DS_W2BMask(SC79_50,    0, BIT15);
}

void msSetHDRCSCType_Bypass(BYTE u8WinIdx)
{
	//WORD u16ArrY2RMatrix[9] = {0x0000, 0x0000, 0x2000, 0x2000, 0x0000, 0x0000, 0x0000, 0x2000, 0x0000};
	WORD u16ArrCSCMatrix[9] = {0x2000, 0x0000, 0x0000, 0x0000, 0x2000, 0x0000, 0x0000, 0x0000, 0x2000};
	WORD u16ArrR2YMatrix[9] = {0x0000, 0x1000, 0x0000, 0x0000, 0x0000, 0x1000, 0x1000, 0x0000, 0x0000};

	WORD u16ArrCSCOffset[3] = {0x0000, 0x0000, 0x0000};
	WORD u16ArrCSCMin[3] = {0x0000, 0x0000, 0x0000};
	WORD u16ArrCSCMax[3] = {0xFFFF, 0xFFFF, 0xFFFF};

	WORD u16ArrR2YOffset[3] = {0x0000, 0x0000, 0x0000};
	WORD u16ArrR2YMin[3] = {0x0000, 0x8001, 0x8001};
	WORD u16ArrR2YMax[3] = {0x7FFF, 0x7FFF, 0x7FFF};

	//msSetHDRInRange(u8WinIdx, 0x07C0, 0x04AC, 0x07C0, 0x04AC);
	msSetHDRInRange(u8WinIdx, 0, 0x400, 0, 0x400);

	msSetHDRY2RBypass(u8WinIdx, FALSE);
	msSetHDRY2R2YShift(u8WinIdx, 0x102, 0x302);
	//msSetHDRY2R(u8WinIdx, u16ArrY2RMatrix, 0, 0, 0, 0, 0xFFFF, 0x10000);
	msSetHDRY2R_BT2020(u8WinIdx, FALSE, 0x80, 0, 0, 0, 0);

	msSetHDRCSCBypass(u8WinIdx, TRUE);
	msSetHDRY2R2YShift(u8WinIdx, 0x400, 0xC04);
	msSetHDRCSC(u8WinIdx, u16ArrCSCMatrix, u16ArrCSCOffset, u16ArrCSCMin, u16ArrCSCMax);

	msSetHDROOTFR2Y(u8WinIdx, 0x1323, 0x2592, 0x74C);

	msSetHDRR2YBypass(u8WinIdx, FALSE);
	msSetHDRY2R2YShift(u8WinIdx, 0x1008, 0x3008);
	msSetHDRR2Y(u8WinIdx, u16ArrR2YMatrix, u16ArrR2YOffset, u16ArrR2YMin, u16ArrR2YMax);

	//msSetHDRDeGammaBypass(u8WinIdx, TRUE);
	//msSetHDRGammaBypass(u8WinIdx, TRUE);
	msSetHDRTMOBypass(u8WinIdx, TRUE);

	msSetHDROutRange(u8WinIdx, 0x8000, 0x0400, 0x0000, 0x0400);
	msSetHDRUVCBypass(u8WinIdx, TRUE);
	msSetHDR422To444To422(u8WinIdx, TRUE, 0x80, FALSE);
	msSetHDR2IPPathEn(u8WinIdx, 0x100, 0x8002);
}
#if ENABLE_DOLBY_HDR
void API_Dolby_Addr_Init(void)
{
    mdrv_Dolby_Addr_Init();
}
#endif
