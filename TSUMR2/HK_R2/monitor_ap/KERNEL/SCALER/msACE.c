/******************************************************************************
 Copyright (c) 2003 MStar Semiconductor, Inc.
 All rights reserved.

 [Module Name]: MsACE.c
 [Date]:        11-Nov-2003
 [Comment]:
   Color Adjust subroutines.
 [Reversion History]:
*******************************************************************************/

#define _MSACE_C_

#include <string.h>
#include <math.h>
#include "types.h"
#include "msACE.h"
//#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Global.h"
//#include "ComboApp.h"
//#include "Common.h"
#include "drvHDR.h"
#define ACE_LIB_VERSION           0x0002
#define _ACE_TSUMG_
#define Y_MINUS16   0

//#define SC_REGS        ((unsigned char volatile xdata *) 0x2F00)

#define _scReadByte(u32Addr)                     msReadByte(u32Addr)//(SC_REGS[(u16Addr)&0xFF])
#define _scWriteByte(u32Addr,u8Value)            msWriteByte(u32Addr,u8Value)//(SC_REGS[(u16Addr)&0xFF]=(u8Value))
#define _scWrite2Byte(u32Addr,u16Value)          msWrite2Byte(u32Addr,u16Value)//{(SC_REGS[(u16Addr)&0xFF]=((u16Value)&0xFF));(SC_REGS[((u16Addr)&0xFF)+1]=((u16Value)>>8));}
#define _scRead2Byte(u32Addr)                    msRead2Byte(u32Addr)//((((WORD)SC_REGS[((u16Addr)&0xFF)+1])<<8)+SC_REGS[(u16Addr)&0xFF])
#define _scWriteByteMask(u32Addr,u8Value,u8Mask) msWriteByteMask(u32Addr,u8Value,u8Mask)//( SC_REGS[(u16Addr)&0xFF] = ( (SC_REGS[(u16Addr)&0xFF] & (~(u8Mask))) | ((u8Value) & (u8Mask))) )

#define ENABLE_MAIN_NEW3X3     1
#define ENABLE_MAIN_NewMatrix     1
#define ENABLE_ACE_ADVANCE     1
///////////////////////////////////
// TSUMD: 8 bit mode
// 8556 : Linear Fix Gamma + Dither
// TSUMC: Linear Fix Gamma + Dither
///////////////////////////////////
//#define _EN_COLOR_ENHANCE_
///////////////////////////////////
//Change Color Matrix from SC8 to SCE for TSUMC/D
///////////////////////////////////
//#define _EN_CM_ADVANCE_

//extern code short tNormalColorCorrectionMatrix[3][3] ;

XDATA BYTE g_bAccOnOff=0;
XDATA BYTE g_bHDTV  = FALSE;
#if ENABLE_MAIN_NewMatrix
XDATA BYTE g_MatrixIdx = 0;
#endif

xdata short (* sFirst)[3];
xdata short  (* sSecond)[3];
xdata short  (* sResult)[3];

XDATA BYTE g_bGlobalHueSat = 0;
XDATA E_WIN_COLOR_FORMAT    g_WinColor[2] = {WIN_COLOR_RGB};
XDATA E_WIN_COLOR_RANGE     g_WinColorRange[2] = {WIN_COLOR_RANGE_AUTO};


code WORD tIPMatrixForLimitRGB[3][3] =
{
    { 0x04AC, 0x0000, 0x0000},
    { 0x0000, 0x04AC, 0x0000},
    { 0x0000, 0x0000, 0x04AC}
};

code WORD tIPMatrixForLimit601[3][3] =
{
    { 0x0667, 0x04AC, 0x0000},
    { 0x1CBD, 0x04AC, 0x1E6E},
    { 0x0000, 0x04AC, 0X0818}
};

code WORD tIPMatrixForLimit709[3][3] =
{
    { 0x0731, 0x04AC, 0x0000},
    { 0x1DDD, 0x04AC, 0x1F25},
    { 0x0000, 0x04AC, 0x0879}
};

code WORD tIPMatrixForLimit2020NCL[3][3] =
{
    { 0x06BC, 0x04AC, 0x0000},
    { 0x1D64, 0x04AC, 0x1F40},
    { 0x0000, 0x04AC, 0x0898}
};

code WORD tIPMatrixForFull601[3][3] =
{
    { 0x059C, 0x0400, 0x0000},
    { 0x1D25, 0x0400, 0x1EA0},
    { 0x0000, 0x0400, 0x0717}
};

code WORD tIPMatrixForFull709[3][3] =
{
    { 0x064D, 0x0400, 0x0000},
    { 0x1E21, 0x0400, 0x1F40},
    { 0x0000, 0x0400, 0x076C}
};

code WORD tIPMatrixForFull2020NCL[3][3] =
{
    { 0x05E6, 0x0400, 0x0000},
    { 0x1DB7, 0x0400, 0x1F58},
    { 0x0000, 0x0400, 0x0787}
};

code short tR2YMatrixForFull601[3][3] =
{
    { 0x0200, -0x01AD, -0x0053},
    { 0x0132, 0x0259, 0x0075},
    { -0x00AD, -0x0153, 0x0200}
};

code short tY2RMatrixForFull601[3][3] =
{
    { 0x059B, 0x0400, 0x0000},
    { -0x02DB, 0x0400, -0x0160},
    { 0x0000, 0x0400, 0x0717}
};
// matrix used to convert RGB color space to YUV color space for SDTV
//code short tSDTVRgb2yuv[3][3] =
//{
//    {  0x0107,  0x0204,  0x0064 }, // 0.257,  0.504,  0.098
//    { -0x0098, -0x012A,  0x01C2 }, // -0.148, -0.291, 0.439
//    {  0x01C2, -0x0179, -0x0049 }  // 0.439,  -0.368, -0.071
//};

// matrix used to convert YUV color space to RGB color space, used for video input for SDTV
/*
code short tSDTVYuv2rgb[3][3] =
{
    {   0x0667, 0x04AB,  0x0000 }, // 1.6007, 1.1678, 0
    {  -0x0342, 0x04AB, -0x0192 }, // -0.8153,1.1678, -0.3929
    {   0x0000, 0x04AB,  0x0817 }  // 0,      1.1678, 2.0232
};
*/
/* // 8bits
{
    {   0x0662, 0x04A8,  0x0000 }, // 1.596,  1.164, 0
    {  -0x0341, 0x04A8, -0x0190 }, // -0.813, 1.164, -0.391
    {   0x0000, 0x04A8,  0x0812 }  // 0,      1.164, 2.018
};
*/

// matrix used to convert RGB color space to YUV color space for HDTV
//code short tHDTVRgb2yuv[3][3] =
//{
//    {  0x00DA,  0x02DC,  0x004A }, // 0.213,  0.715,  0.072
//    { -0x0078, -0x0193,  0x020B }, // -0.117, -0.394, 0.511
//    {  0x020B, -0x01DB, -0x0030 }  // 0.511,  -0.464, -0.047
//};

// matrix used to convert YUV color space to RGB color space, used for video input for HDTV
/*
code short tHDTVYuv2rgb[3][3] =
{
    {   0x0731, 0x04AB,  0x0000 }, // 1.798,  1.1678, 0
    {  -0x0223, 0x04AB, -0x00DA }, // -0.534, 1.1678, -0.2138
    {   0x0000, 0x04AB,  0x0879 }  // 0,      1.1678, 2.1186
};
*/
/* //8bits
{
    {   0x072C, 0x04A8,  0x0000 }, // 1.793,  1.164, 0
    {  -0x0223, 0x04A8, -0x00DA }, // -0.534, 1.164, -0.213
    {   0x0000, 0x04A8,  0x0876 }  // 0,      1.164, 2.115
};
*/

// matrix used to convert YUV color space to RGB color space, used for video input for SDTV
code short tSDTVYuv2rgb_limit[3][3] = //limitYCC601tofullRGB
{
    {    0x0667, 0x04AC,   0x0000 },
    {  -0x0343, 0x04AC, -0x0192 },
    {    0x0000, 0x04AC,   0x0818 }
};

code short tSDTVYuv2rgb[3][3] = //full601tofullRGB
{
    {    0x059C, 0x0400,  0x0000 },
    {  -0x02DB, 0x0400, -0x0160 },
    {    0x0000, 0x0400,  0x0717 }
};

code short tSDTVYuv2rgb_10Bit[3][3] =
{
    {   0x0662, 0x0476,  0x0000 }, // 1.596,  1.164, 0
    {  -0x0341, 0x0476, -0x0190 }, // -0.813, 1.164, -0.391
    {   0x0000, 0x0476,  0x0812 }  // 0,      1.164, 2.018
};
code short tSDTVYuv2rgb_limit_NonSTD[3][3] = //limitYCC601(16-254)tofullRGB
{
    {    0x0667, 0x044C,   0x0000 },
    {   -0x0343, 0x044C,  -0x0192 },
    {    0x0000, 0x044C,   0x0818 }
};

code short tHDTVYuv2rgb_limit_NonSTD[3][3] = //limitYCC709(16-254)tofullRGB
{
    {    0x0731, 0x044C,  0x0000 },
    {   -0x0223, 0x044C, -0x00DB },
    {    0x0000, 0x044C,  0x0879 }
};

code short tITURBT2020_limit_NonSTD[3][3] =//limitYCC2020NCL(16-254)tofullRGB
{
    {   0x06BC, 0x044C,  0x0000 },
    {  -0x029C, 0x044C, -0x00C0 },
    {   0x0000, 0x044C,  0x0898 },
};
// matrix used to convert YUV color space to RGB color space, used for video input for HDTV
code short tHDTVYuv2rgb_limit[3][3] = //limitYCC709tofullRGB
{
    {    0x0731, 0x04AC,  0x0000 },
    {  -0x0223, 0x04AC, -0x00DB },
    {    0x0000, 0x04AC,  0x0879 }
};

code short tHDTVYuv2rgb[3][3] =//fullYCC709tofullRGB
{
    {    0x064D, 0x0400,  0x0000 },
    {  -0x01DF, 0x0400, -0x00C0 },
    {    0x0000, 0x0400,  0x076C }
};

code short tHDTVYuv2rgb_10Bit[3][3] =
{
    {    0x072C, 0x0476,  0x0000 },
    {  -0x0223, 0x0476, -0x00DA },
    {    0x0000, 0x0476,  0x0876 }
};

code short tSDTVYuv2rgbYUV[3][3] =
{
    {   0x0716, 0x0400,  0x0000 }, // 1.772 , 1.0, 0
    {  -0x0160, 0x0400, -0x02DB }, // -0.344, 1.0, -0.714
    {   0x0000, 0x0400,  0x059B }  // 0,      1.0, 1.402
};

code short tHDTVYuv2rgbYUV[3][3] =
{
    {   0x076B, 0x0400,  0x0000 }, // 1.855,  1.0, 0
    {  -0x00BF, 0x0400, -0x01DF }, // -0.187, 1.0, -0.468
    {   0x0000, 0x0400,  0x064B }  // 0,      1.0, 1.574
};

code short tITURBT2020_limit[3][3] =//limitYCC2020NCLtofullRGB
{
    {   0x06BC, 0x04AC,  0x0000 },
    {  -0x029C, 0x04AC, -0x00C0 },
    {   0x0000, 0x04AC,  0x0898 },
};

code short tITURBT2020[3][3] =//fullYCC2020NCLtofullRGB
{
    {    0x05E6, 0x0400,  0x0000 }, // 1.4746,   1.0, 0
    {  -0x0249, 0x0400, -0x00A8 }, // -0.5713,  1.0, -0.1645
    {    0x0000, 0x0400,  0x0787 }  // -0.0001,  1.0, 1.8814
};
code short tNormalColorCorrectionMatrix[3][3] =
{
    { 1024,    0,       0},
    {     0,    1024,       0},
    {    0,     0,      1024}
};
// For video
XDATA BYTE g_ucVideoRCon;
XDATA BYTE g_ucVideoGCon;
XDATA BYTE g_ucVideoBCon;
XDATA BYTE g_ucVideoContrast;
XDATA BYTE g_ucVideoSaturation;
XDATA BYTE g_ucVideoHue;
XDATA WORD g_ucSubContrast;

XDATA short g_sVideoSatHueMatrix[2][3][3];//XDATA short g_sVideoSatHueMatrix[3][3];
XDATA short g_sVideoContrastMatrix[2][3][3];//XDATA short g_sVideoContrastMatrix[3][3];

// For PC
XDATA BYTE g_ucPCRCon;
XDATA BYTE g_ucPCGCon;
XDATA BYTE g_ucPCBCon;
XDATA BYTE g_ucPCContrast;
XDATA short g_sPCConRGBMatrix[2][3][3];
XDATA short g_sPCRGBMatrix_Range[2][3][3];

// For erase the lost of brightness
//XDATA BYTE g_ucMainDigiBrightness;
//XDATA BYTE g_ucSubDigiBrightness;
//XDATA BYTE g_ucBriteComp;

// for tools
//XDATA short g_sCommandMatrix[3][3];

typedef struct
{
    WORD wContrast;
    WORD wSubContrast;
    WORD wRCon;
    WORD wGCon;
    WORD wBCon;
    BYTE ucSaturation;
    WORD uwHue;
    short sContrastRGBMatrix[3][3];
    short sVideoSatHueMatrix[3][3];
    short* psColorCorrectionMatrix; // this variable must pointer to a array[3][3]
    short* psColorCloneMatrix; // this variable must pointer to a array[3][3]
    short* psPCsRGBMatrix; // sRGB matrix for PC, this variable must pointer to a array[3][3]
    short* psYVUtoRGBMatrix; // this variable must pointer to a array[3][3]
    BYTE bForceYUVtoRGB : 1;
    BYTE ucYUVtoRGBMatrixSel : 2;
    short sROffset;
    short sGOffset;
    short sBOffset;
    short sRGBContrast[3];
    short sRGBOffset[3];
    BYTE  bIsUsePostContrast;
    BYTE  bIsForceUseBothContrast;
    BYTE  bIsBlackLevelUsePostOffset;
}StruAceInfo;

static StruAceInfo s_AceInfo[2];

E_WIN_COLOR_FORMAT _CheckCurrentColorDomain(void)
{
    ST_COMBO_COLOR_FORMAT input_cf = IsOSDCSCControl(OSD_CONTROL_CSC);

    if(input_cf.ucColorType == COMBO_COLOR_FORMAT_RGB || input_cf.ucColorType == COMBO_COLOR_FORMAT_DEFAULT)        
    {
        if(g_WinColor[0] == WIN_COLOR_RGB)
        {
            return WIN_COLOR_RGB;
        }
        else
            return WIN_COLOR_YUV;
    }
    else
    {   //YUV case
        return WIN_COLOR_YUV;

    }
}
E_WIN_COLOR_RANGE _CheckCurrentColorRange(void)
{
     ST_COMBO_COLOR_FORMAT input_cf = IsOSDCSCControl(OSD_CONTROL_CSC);
    if(input_cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
    {
        return WIN_COLOR_RANGE_LIMIT;
    }
    else
        return WIN_COLOR_RANGE_FULL;

}

void msACESetIPR2YEnable(Bool bEn)
{
        _scWriteByteMask(SC22_56, (bEn? BIT4:0), BIT4); // 3x3 enable
        _scWriteByteMask(SC22_56, (bEn? BIT5:0), BIT5); // dither
}

//ch 0: R/Cr ; 1: G/Y; 2: B/Cb
void msACESetIPR2YSub16Enable(BYTE ch, Bool bEn)
{
    if (ch == 0)
        _scWriteByteMask(SC22_57, (bEn? BIT2 : 0), BIT2);
    else if (ch == 1)
        _scWriteByteMask(SC22_56, (bEn? BIT3 : 0), BIT3);
    else if (ch == 2)
        _scWriteByteMask(SC22_57, (bEn? BIT1 : 0), BIT1);
}

//ch 0: R ; 1: G; 2: B .    bSub128 : range = -128~127 , else range = 0~255
void msACESetIPR2YRange(BYTE ch, Bool bSub128)
{
    if (ch == 0)
        _scWriteByteMask(SC22_56, (bSub128? BIT2 : 0), BIT2);
    else if (ch == 1)
        _scWriteByteMask(SC22_56, (bSub128? BIT1 : 0), BIT1);
    else if (ch == 2)
        _scWriteByteMask(SC22_56, (bSub128? BIT0 : 0), BIT0);
}

void msACEWriteIPR2YMatrix(WORD *matrix)
{
    BYTE LOCAL_TYPE i,j;

    for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
    {
        _scWrite2Byte( SC22_5A+(i*3 +j) * 2, matrix[i*3 + j]);
        /*
        if (type == E_INPUT_LIMIT_RGB)
            _scWrite2Byte( SC22_5A+i*3 +j, tIPMatixForLimitRGB[i][j] );
        else if (type == E_INPUT_LIMIT_YCC601)
            _scWrite2Byte( SC22_5A, tIPMatixForLimit601[i][j] );
        else if (type == E_INPUT_LIMIT_YCC709)
            _scWrite2Byte( SC22_5A, tIPMatixForLimit709[i][j] );
        else if (type == E_INPUT_LIMIT_2020NCL)
            _scWrite2Byte( SC22_5A, tIPMatixForLimit2020NCL[i][j] );
        else if (type == E_INPUT_FULL_YCC601)
            _scWrite2Byte( SC22_5A, tIPMatixForFull601[i][j] );
        else if (type == E_INPUT_FULL_YCC709)
            _scWrite2Byte( SC22_5A, tIPMatixForFull709[i][j] );
        else if (type == E_INPUT_FULL_2020NCL)
            _scWrite2Byte( SC22_5A, tIPMatixForFull2020NCL[i][j] );
        */
    }
}

void msACESetEnableGlobalHueSat(BYTE bScalerWin, BOOL bEnable)
{
    UNUSED(bScalerWin);
    g_bGlobalHueSat = bEnable;
/*
#if (!ENABLE_FULL_RGB_COLOR_PATH)
    // Main & Sub
    if(MAIN_WINDOW == bScalerWin)
        msWriteByteMask( SC07_40,g_bGlobalHueSat? BIT0 : 0, BIT0);
    else
        msWriteByteMask( SC07_40,g_bGlobalHueSat? BIT4 : 0, BIT4);
#endif
*/
}

BOOL msACEGetEnableGlobalHueSat(void)
{
#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
	return 0;
#else
    return g_bGlobalHueSat; //0;
#endif
}

void msACESetRGBColorRange(BYTE bScalerWin, Bool En, Bool bLimitRange)
{
	xdata BYTE u8Bank=_scReadByte(SC00_00);
	xdata WORD sROffset, sGOffset, sBOffset;

    //gain
    float gain = 1.1678;//0x4AC
    //WORD u16gain =0x400;
    if(mdrv_Adjust_EnableNonStdCSC_Get())//AS input RGB16-254
    {
        gain =1.0746;//0x44C =255/(254-16)
        //HDR output color range only support 16-235 in case of limit Range 
        #if ENABLE_HDR
        if (msGetHDRStatus(MAIN_WINDOW) != HDR_OFF) 
        { 
            gain =1.1678;//0x4AC
        }
        #endif
    }
    else
    {   
        gain =1.1678;//0x4AC
    }
    //BYTE bY_sub16;
	if(bLimitRange)
	{
	    sROffset = 0x3C0;//1024 - 64;
	    sGOffset = 0x3C0;//1024 - 64;
	    sBOffset = 0x3C0;//1024 - 64;
	    g_ucSubContrast = ((1024 *10* gain)+5)/10;
	}
	else
	{
	    g_ucSubContrast = 1024;
	    sROffset = 0x400;
	    sGOffset = 0x400;
	    sBOffset = 0x400;
	}

	_scWriteByte(SC00_00,0x25);
	if(bScalerWin == MAIN_WINDOW)
	{
		_scWriteByteMask( SC25_02, (En?BIT5|BIT4:0), BIT5|BIT4 );
		_scWrite2Byte( SC25_42, sROffset );
		_scWrite2Byte( SC25_44, sGOffset );
		_scWrite2Byte( SC25_46, sBOffset );
		//_scWrite2Byte( SC0E_48, g_ucSubContrast );
		//_scWrite2Byte( SC0E_4A, g_ucSubContrast );
		//_scWrite2Byte( SC0E_4C, g_ucSubContrast );
	}
	else
	{
		_scWriteByteMask( SC25_22, (En?BIT5|BIT4:0), BIT5|BIT4 );
		_scWrite2Byte( SC25_4E, sROffset );
		_scWrite2Byte( SC25_50, sGOffset );
		_scWrite2Byte( SC25_52, sBOffset );
		//_scWrite2Byte( SC0E_54, g_ucSubContrast );
		//_scWrite2Byte( SC0E_56, g_ucSubContrast );
		//_scWrite2Byte( SC0E_58, g_ucSubContrast );
	}

	_scWriteByte(SC00_00, u8Bank);
}


#if ENABLE_MAIN_NewMatrix
void msACESelectYUVtoRGBMatrix(BYTE ucMatrix)
{
    UNUSED(ucMatrix);
    int i;
    for( i = 0; i < 2; i++ )
    {
        s_AceInfo[i].psYVUtoRGBMatrix = (short*)tY2RMatrixForFull601;
#if (!ENABLE_FULL_RGB_COLOR_PATH)
        if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_HDTV )
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tHDTVYuv2rgb;
        else if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT)
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tHDTVYuv2rgb_limit;
        else if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT)
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tSDTVYuv2rgb_limit;
        else  if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_SDTV_10Bit )
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tSDTVYuv2rgb_10Bit;
        else  if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_HDTV_10Bit )
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tHDTVYuv2rgb_10Bit;
        else if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_SDTV_YUV_LMITE)
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tSDTVYuv2rgbYUV;
        else  if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_HDTV_YUV_LMITE )
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tHDTVYuv2rgbYUV;
        else  if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_BT2020 )
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tITURBT2020;
        else  if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_BT2020_LIMIT )
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tITURBT2020_limit;
        else if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT_NONSTD)
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tSDTVYuv2rgb_limit_NonSTD;
        else if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT_NONSTD)
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tHDTVYuv2rgb_limit_NonSTD;
        else  if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_BT2020_LIMIT_NONSTD )
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tITURBT2020_limit_NonSTD;
        else // ACE_YUV_TO_RGB_MATRIX_SDTV
            s_AceInfo[i].psYVUtoRGBMatrix = (short*)tSDTVYuv2rgb;
#endif
    }
}

void msACESetHDTVMode(BYTE isHDTV)
{
	g_MatrixIdx = isHDTV;
}

#else
void msACESetHDTVMode(BYTE isHDTV)
{
    if(isHDTV)
        g_bHDTV = TRUE;
    else
        g_bHDTV = FALSE;

}
#endif

WORD msACE_GetVersion(void)
{
    return ACE_LIB_VERSION;
}

#if 0//defined(_EN_COLOR_ENHANCE_)
void LoadAdvanceFunction(BYTE bScalerWin)
{
    BYTE LOCAL_TYPE ucBank;

    WORD u16Count;

	bScalerWin = bScalerWin;
    ucBank = _scReadByte(SC00_00);


    _scWriteByte(SC00_00, 0x25);
    _scWriteByte(SC25_E2, 0x0F);// All R/G/B
    for (u16Count=0; u16Count <= 0x7FF; u16Count+=8)
    {
        if(u16Count > 0x7FF)
            u16Count = 0x7FF;
        _scWrite2Byte(SC25_E4, u16Count);
    }



     _scWriteByte(SC25_E2, 0x00);
     if(bScalerWin == MAIN_WINDOW)
    {
        //_scWriteByteMask(SC0E_02, _BIT2 ,_BIT2);
        _scWriteByteMask(SC25_03, _BIT3|_BIT2 ,_BIT3|_BIT2);//Fix gamma dither & compress dither
    }
     else
    {
        //_scWriteByteMask(SC0E_22, _BIT2 ,_BIT2);
        _scWriteByteMask(SC25_23, _BIT3|_BIT2 ,_BIT3|_BIT2);//Fix gamma dither & compress dither
    }



    _scWriteByte(SC00_00, ucBank);
}
#endif

static void _mdrv_ACE_ClkGate_SetAllEnable(void)
{
#if (CHIP_ID == CHIP_MT9700)
    msWriteByteMask(SC67_03, BIT1 |BIT2| BIT3 |BIT4| BIT5 |BIT6, BIT1 |BIT2| BIT3 |BIT4| BIT5 |BIT6);
    msWriteByteMask(SC67_44, BIT2, BIT2);
    msWriteByteMask(SC67_50, BIT2, BIT2);
    msWriteByteMask(SC67_64, BIT2, BIT2);
    msWriteByteMask(SC67_7C, BIT2, BIT2);
    msWriteByteMask(SC67_A0, BIT2, BIT2);
    msWriteByteMask(SC67_AC, BIT2, BIT2);
    msWriteByteMask(SC67_B8, BIT2, BIT2);
    msWriteByteMask(SC67_C4, BIT2, BIT2);
#endif
}

void InitACEVar(void)
{
    _mdrv_ACE_ClkGate_SetAllEnable();
    s_AceInfo[MAIN_WINDOW].bIsForceUseBothContrast = 1;
    s_AceInfo[MAIN_WINDOW].wRCon = 1024;
    s_AceInfo[MAIN_WINDOW].wGCon = 1024;
    s_AceInfo[MAIN_WINDOW].wBCon = 1024;
    s_AceInfo[MAIN_WINDOW].sRGBContrast[0] = 1024;
    s_AceInfo[MAIN_WINDOW].sRGBContrast[1] = 1024;
    s_AceInfo[MAIN_WINDOW].sRGBContrast[2] = 1024;
    s_AceInfo[MAIN_WINDOW].wContrast = 0x80; 
    
    s_AceInfo[SUB_WINDOW].bIsForceUseBothContrast = 1;
    s_AceInfo[SUB_WINDOW].wRCon = 1024;
    s_AceInfo[SUB_WINDOW].wGCon = 1024;
    s_AceInfo[SUB_WINDOW].wBCon = 1024;
    s_AceInfo[SUB_WINDOW].sRGBContrast[0] = 1024;
    s_AceInfo[SUB_WINDOW].sRGBContrast[1] = 1024;
    s_AceInfo[SUB_WINDOW].sRGBContrast[2] = 1024;
    s_AceInfo[SUB_WINDOW].wContrast = 0x80; 
    // For video
    g_ucVideoRCon = 0x80;
    g_ucVideoGCon = 0x80;
    g_ucVideoBCon = 0x80;
    g_ucVideoContrast = 0x80;
    g_ucVideoSaturation = 0x80;
    g_ucVideoHue = 50;

    g_sVideoSatHueMatrix[MAIN_WINDOW][0][0] = g_sVideoSatHueMatrix[MAIN_WINDOW][1][1] = g_sVideoSatHueMatrix[MAIN_WINDOW][2][2] = 1024;
    g_sVideoSatHueMatrix[MAIN_WINDOW][0][1] = g_sVideoSatHueMatrix[MAIN_WINDOW][1][0] = g_sVideoSatHueMatrix[MAIN_WINDOW][2][0] =
    g_sVideoSatHueMatrix[MAIN_WINDOW][0][2] = g_sVideoSatHueMatrix[MAIN_WINDOW][1][2] = g_sVideoSatHueMatrix[MAIN_WINDOW][2][1] = 0;

    g_sVideoContrastMatrix[MAIN_WINDOW][0][0] = g_sVideoContrastMatrix[MAIN_WINDOW][1][1] = g_sVideoContrastMatrix[MAIN_WINDOW][2][2] = 1024;
    g_sVideoContrastMatrix[MAIN_WINDOW][0][1] = g_sVideoContrastMatrix[MAIN_WINDOW][1][0] = g_sVideoContrastMatrix[MAIN_WINDOW][2][0] =
    g_sVideoContrastMatrix[MAIN_WINDOW][0][2] = g_sVideoContrastMatrix[MAIN_WINDOW][1][2] = g_sVideoContrastMatrix[MAIN_WINDOW][2][1] = 0;

    g_sVideoSatHueMatrix[SUB_WINDOW][0][0] = g_sVideoSatHueMatrix[SUB_WINDOW][1][1] = g_sVideoSatHueMatrix[SUB_WINDOW][2][2] = 1024;
    g_sVideoSatHueMatrix[SUB_WINDOW][0][1] = g_sVideoSatHueMatrix[SUB_WINDOW][1][0] = g_sVideoSatHueMatrix[SUB_WINDOW][2][0] =
    g_sVideoSatHueMatrix[SUB_WINDOW][0][2] = g_sVideoSatHueMatrix[SUB_WINDOW][1][2] = g_sVideoSatHueMatrix[SUB_WINDOW][2][1] = 0;

    g_sVideoContrastMatrix[SUB_WINDOW][0][0] = g_sVideoContrastMatrix[SUB_WINDOW][1][1] = g_sVideoContrastMatrix[SUB_WINDOW][2][2] = 1024;
    g_sVideoContrastMatrix[SUB_WINDOW][0][1] = g_sVideoContrastMatrix[SUB_WINDOW][1][0] = g_sVideoContrastMatrix[SUB_WINDOW][2][0] =
    g_sVideoContrastMatrix[SUB_WINDOW][0][2] = g_sVideoContrastMatrix[SUB_WINDOW][1][2] = g_sVideoContrastMatrix[SUB_WINDOW][2][1] = 0;

    // For PC
    g_ucPCRCon = 0x80;
    g_ucPCGCon = 0x80;
    g_ucPCBCon = 0x80;
    g_ucPCContrast = 0x80;

    g_sPCConRGBMatrix[MAIN_WINDOW][0][0] = g_sPCConRGBMatrix[MAIN_WINDOW][1][1] = g_sPCConRGBMatrix[MAIN_WINDOW][2][2] = 1024;
    g_sPCConRGBMatrix[MAIN_WINDOW][0][1] = g_sPCConRGBMatrix[MAIN_WINDOW][1][0] = g_sPCConRGBMatrix[MAIN_WINDOW][2][0] =
    g_sPCConRGBMatrix[MAIN_WINDOW][0][2] = g_sPCConRGBMatrix[MAIN_WINDOW][1][2] = g_sPCConRGBMatrix[MAIN_WINDOW][2][1] = 0;

    g_sPCConRGBMatrix[SUB_WINDOW][0][0] = g_sPCConRGBMatrix[SUB_WINDOW][1][1] = g_sPCConRGBMatrix[SUB_WINDOW][2][2] = 1024;
    g_sPCConRGBMatrix[SUB_WINDOW][0][1] = g_sPCConRGBMatrix[SUB_WINDOW][1][0] = g_sPCConRGBMatrix[SUB_WINDOW][2][0] =
    g_sPCConRGBMatrix[SUB_WINDOW][0][2] = g_sPCConRGBMatrix[SUB_WINDOW][1][2] = g_sPCConRGBMatrix[SUB_WINDOW][2][1] = 0;

    // Color matrix initilaize
    //tSrgbMatrix = &tSRGB[0][0];
    tColorCorrectionMatrix = &tNormalColorCorrectionMatrix[0][0];

    // For erase the lost of brightness
    //g_ucMainDigiBrightness = 0x80;
    //g_ucSubDigiBrightness = 0x80;
    //g_ucBriteComp = 0;

    //g_ucACEConfig = 0;
    //msACEPCYUV2RGBCtl( FALSE );
    //msACEPCsRGBCtl( FALSE );

    // for tool
    #if 0
    g_sCommandMatrix[0][0] = g_sCommandMatrix[1][1] = g_sCommandMatrix[2][2] = 1024;
    g_sCommandMatrix[0][1] = g_sCommandMatrix[1][0] = g_sCommandMatrix[2][0] =
    g_sCommandMatrix[0][2] = g_sCommandMatrix[1][2] = g_sCommandMatrix[2][1] = 0;
    #endif
#if 0//defined(_EN_COLOR_ENHANCE_) 
    LoadAdvanceFunction(MAIN_WINDOW);
    LoadAdvanceFunction(SUB_WINDOW);
#endif

#if 1 //avoid compiler warning
    if (g_bHDTV!=g_bHDTV)
    {
	#if ENABLE_MAIN_NewMatrix
	msACESetHDTVMode(g_MatrixIdx);
	#else
        msACESetHDTVMode(FALSE);
	#endif

        msACE_GetVersion();
        #if ENABLE_MAIN_NEW3X3
        msAdjustPCContrast( MAIN_WINDOW, 0x80 );
        msAdjustPCRGB( MAIN_WINDOW, 0x80, 0x80, 0x80 );
        #endif
        msAdjustVideoRGB( SUB_WINDOW, 0x80, 0x80, 0x80 );
    }
#endif

msACESetRGBColorRange(MAIN_WINDOW, TRUE,FALSE);
//msACESetRGBColorRange(SUB_WINDOW, TRUE,FALSE);

}

#define PI  (3.14159265)

// sin function
short sine(BYTE ucValue)
{
    return (short)(sin((ucValue * PI) / 128.) * 1024.);
}

// cos function
short cosine(BYTE ucValue)
{
    return (short)(cos((ucValue * PI) / 128.) * 1024.);
}


//xdata short (* sFirst)[3];
//xdata short  (* sSecond)[3];
//xdata short  (* sResult)[3];

//void ArrayMultiply(short sFirst[3][3], short sSecond[3][3], short sResult[3][3])
static void ArrayMultiply(short first[3][3], short second[3][3], short result[3][3])
{
    BYTE LOCAL_TYPE ucRow, ucCol;

    //go line by line
    for(ucRow=0; ucRow!=3; ucRow++)
    {
        // go column by column
        for(ucCol=0; ucCol!=3; ucCol++)
        {
            result[ucRow][ucCol] = (((long)first[ucRow][0] * second[0][ucCol]) +
                                     ((long)first[ucRow][1] * second[1][ucCol]) +
                                     ((long)first[ucRow][2] * second[2][ucCol])) >> 10;
       } // for
    } // for
}

void SetVideoSatHueMatrix(BYTE bScalerWin)//static void SetVideoSatHueMatrix()
{
    short LOCAL_TYPE sTmp;
    BYTE LOCAL_TYPE ucHue;
#if ENABLE_FULL_RGB_COLOR_PATH
    msACESetEnableGlobalHueSat(bScalerWin, TRUE);
#endif
    ucHue = ((g_ucVideoHue <= 50) ? (50 - g_ucVideoHue) : ~(g_ucVideoHue - 50));

    sTmp = ((short)g_ucVideoSaturation * 8);

    g_sVideoSatHueMatrix[bScalerWin][2][2] = ((((long)cosine(ucHue) * sTmp))>>10);
    g_sVideoSatHueMatrix[bScalerWin][0][0] = ((( (long)cosine(ucHue)* sTmp))>>10);

    g_sVideoSatHueMatrix[bScalerWin][2][0] = ((((long)sine(ucHue)   * sTmp))>>10);
    g_sVideoSatHueMatrix[bScalerWin][0][2] = (((-(long)sine(ucHue)  * sTmp))>>10);

    g_sVideoSatHueMatrix[bScalerWin][1][1] = 1024;
    g_sVideoSatHueMatrix[bScalerWin][0][1] = g_sVideoSatHueMatrix[bScalerWin][1][0] = g_sVideoSatHueMatrix[bScalerWin][1][2] = g_sVideoSatHueMatrix[bScalerWin][2][1] = 0;
}

void SetVideoContrastMatrix(BYTE bScalerWin)//static void SetVideoContrastMatrix()
{
	//BYTE VideoContrast = 0x80;//The contrast is applied to PostGain, not to the Y2R matrix.
#if ENABLE_ACE_ADVANCE
	g_sVideoContrastMatrix[bScalerWin][0][0] = 0x400;
	g_sVideoContrastMatrix[bScalerWin][1][1] = 0x400;
	g_sVideoContrastMatrix[bScalerWin][2][2] = 0x400;
#else
	g_sVideoContrastMatrix[bScalerWin][0][0] = ( (WORD)g_ucVideoRCon * (g_ucVideoContrast)) >> 4;
	g_sVideoContrastMatrix[bScalerWin][1][1] = ( (WORD)g_ucVideoGCon * (g_ucVideoContrast)) >> 4;
	g_sVideoContrastMatrix[bScalerWin][2][2] = ( (WORD)g_ucVideoBCon * (g_ucVideoContrast)) >> 4;
#endif	
}

static xdata short g_psMatrix[9];
void msWriteColorMatrix(BYTE bScalerWin)//static void msWriteColorMatrix()
{
    BYTE LOCAL_TYPE i,j;
    DWORD  LOCAL_TYPE ucAddr ;// SC08_82//BYTE LOCAL_TYPE ucAddr = BK3_5E;
    short LOCAL_TYPE sTmp;
    BYTE LOCAL_TYPE u8Bank=_scReadByte(SC00_00);

    if(bScalerWin == MAIN_WINDOW)
	{
	    	ucAddr = SC10_4C;
		_scWriteByte(SC00_00, 0x10);
	}
    else
	{
		ucAddr = SC0F_3A;
		_scWriteByte(SC00_00, 0x0F);
	}

    for(i=0; i!=3; i++)
    {
        for(j=0; j!=3; j++)
        {
            sTmp = g_psMatrix[i*3+j];
            //printf( "[%X]:", ucAddr );
            if( sTmp >= 0 )
            {
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }
                msWrite2ByteMask( ucAddr, sTmp, 0x1FFF );//mStar_WriteWord( ucAddr+1, sTmp );
            }
            else
            {
                sTmp &= 0xFFFF;
                sTmp |= 0x1000;
                msWrite2ByteMask( ucAddr, sTmp, 0x1FFF );//mStar_WriteWord( ucAddr+1, sTmp );
            }
            ucAddr += 2;
        } // for
    } // for

    _scWriteByte(SC00_00, u8Bank);
}

void msFormatMatrixToS2D10(short* psMatrix) //s2.10 format transfer to short data format;
{
	int idx = 0;
	short val = 0;
	
	for (idx = 0; idx < 9; idx++)
	{
		val = psMatrix[idx];
		//printf("val -before %d", val);
		if ((val & _BIT12) == _BIT12)
		{
			val &= 0xFFF;
			val = 0x1000 - val;
			val *= -1;
		}
		psMatrix[idx] = val;
		//printf("val -after %d", val);
	}
}

void msWritexvYccColorMatrix( BYTE ucWinIndex, short* psMatrix)
{
	if (psMatrix == NULL)
	{
		return;
	}
    BYTE i,j; //, u8ScalerIdx = 0;
    DWORD dwAddr;
    short sTmp;
    //DWORD u32BaseAddr = msDrvScGetRegBase(u8ScalerIdx);
    //WORD u16ScalerMask = msAPIWinGetScMaskOut(ucWinIndex);

    //MST_ASSERT(ucWinIndex <DISPLAY_MAX_NUMS);

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = msDrvScGetRegBase(u8ScalerIdx);
            if(ucWinIndex == MAIN_WINDOW)//if( (u8ScalerIdx % 2) == 1)// Main Win
                dwAddr =  REG_ADDR_SRGBCM_MAIN;
            else
                dwAddr =  REG_ADDR_SRGBCM_SUB;
            for(i=0; i!=3; i++)
            {
                for(j=0; j!=3; j++)
                {
                    sTmp = psMatrix[i*3+j];
                    if( sTmp >= 0 )
                    {
                        if( sTmp > 0xfff )
                        {
                            sTmp = 0xfff;
                        }
                        msWrite2Byte(dwAddr, sTmp);
                    }
                    else
                    {
                        sTmp &= 0xFFFF;
                        sTmp |= 0x1000;

                        msWrite2Byte(dwAddr, sTmp);
                    }
					 
                    dwAddr += 2;
                } // for
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}


//Main/Sub
typedef short Typ3x3Array[3][3];

void msSetVideoColorMatrix(BYTE bScalerWin)//static void msSetVedioColorMatrix()
{
    XDATA short sResultTmp3[3][3];
    XDATA short sResultTmp2[3][3];
    XDATA short sResultTmp1[3][3];
    XDATA short sResultTmp0[3][3];
    short sTempMatrix[3][3];
    BYTE LOCAL_TYPE bCMEn;

#if ENABLE_FULL_RGB_COLOR_PATH
		memcpy(sTempMatrix, tY2RMatrixForFull601, sizeof(Typ3x3Array));
		ArrayMultiply(sTempMatrix, g_sVideoSatHueMatrix[bScalerWin], sResultTmp0);
	
		if(tColorCorrectionMatrix != NULL)
		{
			memcpy(sTempMatrix, tColorCorrectionMatrix, sizeof(Typ3x3Array));
			ArrayMultiply(sTempMatrix, sResultTmp0, sResultTmp1);
		}
		else
		{
			memcpy(sResultTmp1, sResultTmp0, sizeof(Typ3x3Array));
		}
	
		ArrayMultiply(g_sVideoContrastMatrix[bScalerWin], sResultTmp1, sResultTmp2);
		memcpy(sTempMatrix, tR2YMatrixForFull601, sizeof(Typ3x3Array));
		ArrayMultiply(sResultTmp2, sTempMatrix, sResultTmp3);
#else
		msACESelectYUVtoRGBMatrix(g_MatrixIdx);
		memcpy(sTempMatrix, s_AceInfo[bScalerWin].psYVUtoRGBMatrix, sizeof(Typ3x3Array));
		ArrayMultiply(sTempMatrix, g_sVideoSatHueMatrix[bScalerWin], sResultTmp1 );
	
		if(tColorCorrectionMatrix != NULL)
		{
			memcpy(sResultTmp0, tColorCorrectionMatrix, sizeof(Typ3x3Array));
				
			ArrayMultiply(sResultTmp0, sResultTmp1, sResultTmp2);
		}
		else
		{
			memcpy(sResultTmp2, sResultTmp1, sizeof(Typ3x3Array));
		}
	
		ArrayMultiply(g_sVideoContrastMatrix[bScalerWin], sResultTmp2, sResultTmp3);
#endif
	
    memcpy(g_psMatrix, sResultTmp3, sizeof(g_psMatrix));

    msWriteColorMatrix(bScalerWin);

    
#if ENABLE_FULL_RGB_COLOR_PATH
    if (msACEGetEnableGlobalHueSat() == FALSE)
        bCMEn = 0;
    else
        bCMEn = BIT4;
#else
    //if (IsColorspaceRGB() && (UserPrefInputColorFormat == INPUTCOLOR_RGB))   
    bCMEn = BIT4;
#endif

    msACESetEnableY2RMatrix(bScalerWin, bCMEn);
}

void msSetVideoColorMatrix_Contrast(BYTE bScalerWin, BYTE ucContrast)
{
    XDATA short sResultTmp2[3][3];
    BYTE LOCAL_TYPE bCMEn;

    g_ucVideoContrast = ucContrast;
    SetVideoContrastMatrix(bScalerWin);
    memcpy(sResultTmp2, g_sVideoContrastMatrix[bScalerWin], sizeof(Typ3x3Array));

    memcpy(g_psMatrix, sResultTmp2, sizeof(g_psMatrix));
            
    msWriteColorMatrix(bScalerWin);

    bCMEn = msACEGetEnableGlobalHueSat();

    msACESetEnableY2RMatrix(bScalerWin, bCMEn);
}

//Main/Sub
void msAdjustHSC(BYTE bScalerWin,BYTE ucHue, BYTE ucSaturation, BYTE ucContrast)
{
#if (ENABLE_FULL_RGB_COLOR_PATH)
    g_ucVideoSaturation = ucSaturation;
    g_ucVideoContrast = ucContrast;
    g_ucVideoHue = ucHue;
    SetVideoSatHueMatrix(bScalerWin);
    SetVideoContrastMatrix(bScalerWin);

    msSetVideoColorMatrix(bScalerWin);
#else
	UNUSED(bScalerWin);
	UNUSED(ucHue);
	UNUSED(ucSaturation);
	UNUSED(ucContrast);
#endif    
}

void msAdjustRGBGainContrast( BYTE u8DispWin, WORD u16Contrast, WORD u16Red, WORD u16Green, WORD u16Blue)
{
    if (msForceUseBothContrast_Get(u8DispWin))
    {
        msSetContrast(u8DispWin, u16Contrast);
        msSetUserRGBGain(u8DispWin, u16Red<<1, u16Green<<1, u16Blue<<1);
    }
    else
    {
        msSetRGBContrast(u8DispWin, u16Contrast, u16Red, u16Green, u16Blue);
    }
    msAdjustRGBContrast(u8DispWin);
}
//Main/Sub
void msAdjustVideoContrast(BYTE bScalerWin, BYTE ucContrast )// 2004/12/8
{
    g_ucVideoContrast = ucContrast;
    SetVideoContrastMatrix(bScalerWin);
#if ENABLE_ACE_ADVANCE
	msAdjustRGBGainContrast(bScalerWin, ucContrast, g_ucVideoRCon, g_ucVideoGCon, g_ucVideoBCon);
#endif	
    msSetVideoColorMatrix( bScalerWin);
}

//Main/Sub
void msAdjustVideoSaturation(BYTE bScalerWin, BYTE ucSaturation )
{

     g_ucVideoSaturation = ucSaturation;
    SetVideoSatHueMatrix(bScalerWin);
    msSetVideoColorMatrix(bScalerWin);
}

//Main/Sub
void msAdjustVideoHue(BYTE bScalerWin, BYTE ucHue )
{

    g_ucVideoHue = ucHue;
    SetVideoSatHueMatrix(bScalerWin);
    msSetVideoColorMatrix( bScalerWin);
}

//Main/Sub
void msAdjustVideoRGB( BYTE bScalerWin, BYTE ucRed, BYTE ucGreen, BYTE ucBlue )
{
    msAdjustVideoContrastRGB( bScalerWin, g_ucVideoContrast, ucRed, ucGreen, ucBlue );
}

void msAdjustVideoContrastRGB(BYTE bScalerWin, BYTE ucContrast, BYTE ucRCon, BYTE ucGCon, BYTE ucBCon)
{
    g_ucVideoContrast = ucContrast;
    g_ucVideoRCon = ucRCon;
    g_ucVideoGCon = ucGCon;
    g_ucVideoBCon = ucBCon;
    SetVideoContrastMatrix(bScalerWin);
   #if ENABLE_ACE_ADVANCE	
	msAdjustRGBGainContrast(bScalerWin, ucContrast, ucRCon, ucGCon, ucBCon);
   #endif	
	msSetVideoColorMatrix(bScalerWin);
}

#if ENABLE_MAIN_NEW3X3
void SetPCConRGBMatrix(BYTE bScalerWin)
{

#if ENABLE_ACE_ADVANCE
    g_sPCConRGBMatrix[bScalerWin][0][0] =0x400;// ( (WORD)g_ucPCRCon * (g_ucPCContrast)) >> 4;
    g_sPCConRGBMatrix[bScalerWin][1][1] = 0x400;//( (WORD)g_ucPCGCon * (g_ucPCContrast)) >> 4;
    g_sPCConRGBMatrix[bScalerWin][2][2] =0x400; //( (WORD)g_ucPCBCon * (g_ucPCContrast)) >> 4;
#else
    g_sPCConRGBMatrix[bScalerWin][0][0] = ( (WORD)g_ucPCRCon * (g_ucPCContrast)) >> 4;
    g_sPCConRGBMatrix[bScalerWin][1][1] = ( (WORD)g_ucPCGCon * (g_ucPCContrast)) >> 4;
    g_sPCConRGBMatrix[bScalerWin][2][2] = ( (WORD)g_ucPCBCon * (g_ucPCContrast)) >> 4;
#endif	
}


#if ENABLE_FULL_RGB_COLOR_PATH
void msSetPCColorMatrix( BYTE bScalerWin )
{
	BYTE ucBank;
       BYTE LOCAL_TYPE bCMEn;

	ucBank = _scReadByte(SC00_00);

    memcpy(g_psMatrix, g_sPCConRGBMatrix, sizeof(g_psMatrix));
	bCMEn = 0;
	msWriteColorMatrix(bScalerWin);

	_scWriteByte(SC00_00, 0x0F);
	if(bScalerWin == MAIN_WINDOW)
	{
		_scWriteByteMask( SC0F_AE, bCMEn, BIT6);
	}
	else
	{
		_scWriteByteMask( SC0F_AE, bCMEn, BIT7);
	}
	_scWriteByte(SC00_00, ucBank);

	_scWriteByte(SC00_00, 0x10);

	if (bScalerWin == MAIN_WINDOW)
	{
		_scWriteByte(SC00_00, 0x10);
		_scWriteByteMask( SC10_5E, bCMEn, BIT5|BIT4|BIT2|BIT0 );
	}
	else
	{
		_scWriteByte(SC00_00, 0x0F);
		_scWriteByteMask( SC0F_4C, bCMEn, BIT5|BIT4|BIT2|BIT0 );
	}

	_scWriteByte(SC00_00, ucBank);
}
#else
//Main/Sub
void msSetPCColorMatrix( BYTE bScalerWin )
{
	BYTE ucBank;
	BYTE LOCAL_TYPE bCMEn;
	XDATA short sResultTmp[3][3];
	XDATA short sResultTmp2[3][3];
	XDATA short sTmp[3][3];

	ucBank = _scReadByte(SC00_00);

	memcpy(sResultTmp, g_sPCConRGBMatrix, sizeof(g_psMatrix));
	memcpy(sTmp, g_sPCRGBMatrix_Range, sizeof(g_psMatrix));
	ArrayMultiply(sTmp, sResultTmp, sResultTmp2);
	memcpy(g_psMatrix, sResultTmp2, sizeof(g_psMatrix));

	bCMEn = BIT5|BIT4;
	msWriteColorMatrix(bScalerWin);

	_scWriteByte(SC00_00, ucBank);
	_scWriteByte(SC00_00, 0x10);

	if (bScalerWin == MAIN_WINDOW)
	{
		_scWriteByte(SC00_00, 0x10);
		_scWriteByteMask( SC10_5E, bCMEn, BIT5|BIT4|BIT2|BIT0 );
	}
	else
	{
		_scWriteByte(SC00_00, 0x0F);
		_scWriteByteMask( SC0F_4C, bCMEn, BIT5|BIT4|BIT2|BIT0 );
	}

	_scWriteByte(SC00_00, ucBank);
}
#endif

#endif

#if ENABLE_MAIN_NEW3X3
//Main/Sub
void msAdjustPCContrastRGB( BYTE bScalerWin, BYTE ucContrast, BYTE ucRed, BYTE ucGreen, BYTE ucBlue )
{
    g_ucPCContrast = ucContrast;
    g_ucPCRCon = ucRed;
    g_ucPCGCon = ucGreen;
    g_ucPCBCon = ucBlue;
    SetPCConRGBMatrix(bScalerWin);
#if ENABLE_ACE_ADVANCE
	msAdjustRGBGainContrast(bScalerWin, ucContrast, ucRed, ucGreen, ucBlue);
#endif	
    msSetPCColorMatrix( bScalerWin );
}

//Main/Sub
void msAdjustPCContrast( BYTE bScalerWin, BYTE ucContrast )
{
    msAdjustPCContrastRGB( bScalerWin, ucContrast, g_ucPCRCon, g_ucPCGCon, g_ucPCBCon );
}

//Main/Sub
void msAdjustPCRGB( BYTE bScalerWin, BYTE ucRed, BYTE ucGreen, BYTE ucBlue )
{
    msAdjustPCContrastRGB( bScalerWin, g_ucPCContrast, ucRed, ucGreen, ucBlue );
}
#endif


/////////////////////////////////////////////////////////////////////
// ACC
/////////////////////////////////////////////////////////////////////

#define ACC_H_POS       (PANEL_H_START+g_sPnlInfo.sPnlTiming.u16Width/4)
#define ACC_V_POS       (g_sPnlInfo.sPnlTiming.u16Height/4)
#define ACC_H_SIZE      g_sPnlInfo.sPnlTiming.u16Width/2
#define ACC_V_SIZE      g_sPnlInfo.sPnlTiming.u16Height/2

#define ACC_H_POS_SPLIT       (PANEL_H_START+g_sPnlInfo.sPnlTiming.u16Width/2)
#define ACC_V_POS_SPLIT       (0)
#define ACC_H_SIZE_SPLIT      g_sPnlInfo.sPnlTiming.u16Width/2
#define ACC_V_SIZE_SPLIT      g_sPnlInfo.sPnlTiming.u16Height

#define ACC_H_POS_FULL       (PANEL_H_START)
#define ACC_V_POS_FULL       (0)
#define ACC_H_SIZE_FULL      g_sPnlInfo.sPnlTiming.u16Width
#define ACC_V_SIZE_FULL      g_sPnlInfo.sPnlTiming.u16Height

// para[0]: flag
// para[1~2]:H-Start
// para[3~4]:V-Start
// para[5~6]:H-Size
// para[7~8]:V-Size


XDATA WORD g_uiHStart;
XDATA WORD g_uiHEnd;
XDATA WORD g_uiVStart;
XDATA WORD g_uiVEnd;

void msAccSetup( WORD phstart_point, WORD phend_point, WORD pvstart_point, WORD pvend_point)
//void msAccSetup()
{
    BYTE LOCAL_TYPE ucBank;
    WORD LOCAL_TYPE panel_h_start;

    BYTE LOCAL_TYPE u8HDiv=(phstart_point&0x8000?2:1); //Jison 110322

    g_uiHStart = (phstart_point&0x7FFF)/u8HDiv; //Jison 110322, bit15 is used to indicate H divider=2 if set
    g_uiHEnd = phend_point/u8HDiv; //Jison 110322
    g_uiVStart = pvstart_point;
    g_uiVEnd = pvend_point;

    ucBank = _scReadByte( SC00_00 );

    _scWriteByte(SC00_00, 0x00);
    panel_h_start=_scRead2Byte(SC00_12); // Hor. DE start

    g_uiHStart = g_uiHStart + panel_h_start;
    g_uiHEnd   = g_uiHEnd   + panel_h_start;
    g_uiHEnd -=1;
    _scWriteByte(SC00_00, 0x03);
    _scWrite2Byte(SC03_18,g_uiHStart);
    _scWrite2Byte(SC03_1C,g_uiHEnd);

    _scWrite2Byte(SC03_1E,g_uiVStart);
    _scWrite2Byte(SC03_1A,g_uiVEnd);

    _scWriteByte(SC00_00, ucBank);

}

void msAccOnOff( BYTE on )
{
    BYTE LOCAL_TYPE ucBank=_scReadByte(SC00_00);


    _scWriteByte(SC00_00, 0x00);
    if(on != 0)
    {
        g_bAccOnOff = TRUE;
        _scWriteByteMask(SC00_5C, BIT3, BIT3);
    }
    else
    {
        g_bAccOnOff = FALSE;
        _scWriteByteMask( SC00_5C, 0, BIT3 );
    }
    _scWriteByte(SC00_00, ucBank);


}

//Main/Sub
void msAdjustSubBrightness(BYTE bScalerWin,  BYTE u8brightnessR, BYTE u8brightnessG, BYTE u8brightnessB )
{
  BYTE LOCAL_TYPE ucBank;


    DWORD wAddr;

    ucBank = _scReadByte(SC00_00); // Backup bank

    if(bScalerWin == MAIN_WINDOW)
        wAddr = SC08_62;
    else
        wAddr = SC08_68;

    _scWriteByte(SC00_00, 0x08);

    _scWrite2Byte( wAddr, (((WORD)u8brightnessR)<<3) );
    _scWrite2Byte( wAddr+2, (((WORD)u8brightnessG)<<3) );
    _scWrite2Byte( wAddr+4, (((WORD)u8brightnessB)<<3) );

    _scWriteByte(SC00_00, ucBank);



}


#define INIT_SUBWIN_PEAKING_CTL     0x03
#define INIT_SUBWIN_CORING          0x44
#define INIT_SUBWIN_CTI             0x90
//#define INIT_SUBWIN_LTI             0x4C

void InitialPeaking(void)
{
  BYTE u8Bank=_scReadByte(SC00_00);

	BYTE uci;

	_scWriteByte(SC00_00,0x0B);
	//Over/Under shoort Setting  //main and sub use the same setting
	for( uci = 0; uci < 8; uci ++ )
	{
		 _scWriteByteMask(SC0B_40+uci, 0xFF,0xFF);		//overshoort：SC0B_40~SC0B_47
		 _scWriteByteMask(SC0B_48+uci, 0xFF,0xFF);		//undershoort：SC0B_48~SC0B_4F
	}
	for( uci = 0; uci < 4; uci ++ )
	{	//Term Setting for main win
		 _scWriteByteMask(SC0B_38+uci*2	, ((uci*2)<<4)|(uci*2+1)	,0x77);
		 _scWriteByteMask(SC0B_38+uci*2+1, ((uci*2)<<4)|(uci*2+1) ,0x77);
		//Term Setting for sub win
		 _scWriteByteMask(SC0B_58+uci*2	, ((uci*2)<<4)|(uci*2+1)	,0x77);
		 _scWriteByteMask(SC0B_58+uci*2+1, ((uci*2)<<4)|(uci*2+1) ,0x77);
	}
	//_scWriteByteMask( SC0B_20, BIT0	, BIT0 );	//main post peaking enable, Setting in msAdjustSharpness()
	_scWriteByteMask( SC0B_21, 0x01	, 0xFF );	//main Bank1 peaking enable

	//_scWriteByteMask( SC0B_28, BIT0	, BIT0 );	//sub post peaking enable, Setting in msAdjustSharpness()
	_scWriteByteMask( SC0B_29, 0x01	, 0xFF );	//sub Bank1~8 peaking enable
	//main win Post Coring Setting
	_scWriteByteMask( SC0B_26, 0x42	, 0xFF );			//[7:4]:Coring_TH_2 ,[3:0]:Coring_TH_1
	//sub win Post Coring Setting
	_scWriteByteMask( SC0B_2E, 0x42	, 0xFF );			//[7:4]:Coring_TH_2 ,[3:0]:Coring_TH_1
	//coring step - default
	_scWriteByteMask( SC0B_61, 0x33	, 0x33 );			//[5:4]:Sub Coring_TH_Step ,[1:0]:Main Coring_TH_Step


  _scWriteByte(SC00_00,u8Bank);
}
void msAdjustSharpness(BYTE bScalerWin ,BYTE ucSharpness, BYTE ucStep )
{
    BYTE LOCAL_TYPE ucBank;
    BYTE LOCAL_TYPE ucTmp = 0;
    ucBank = _scReadByte(SC00_00);

	_scWriteByte(SC00_00, 0x0B);

	ucSharpness&=0x3F;
	ucStep&=0x03;

	if(ucSharpness>=0x08) //sharp
	{
		ucTmp = 0x20; //strength can be adjusted 
		//ucSharpness -= 0x10; // Sharpness gain, xx.xxxx
	}
	else // blur
	{
		ucTmp = 0x18; //strength can be adjusted
		//ucSharpness = (0x10 - ucSharpness)*3; // Sharpness gain, xx.xxxx
	}

	if(bScalerWin == MAIN_WINDOW)
	{
		_scWriteByte(SC0B_27,  ucTmp); //gain for all band
		_scWriteByte(SC0B_30,  ucSharpness);	//bank1 coeff
		_scWriteByte(SC0B_31,  ucSharpness);	//bank2 coeff

		//_scWriteByte(SC0B_22,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep)); //for band1~4
		//_scWriteByte(SC0B_23,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep)); //for band5~8
		_scWriteByteMask( SC0B_20, BIT0	, BIT0 );	//main post peaking enable
	}
	else
	{
		_scWriteByte(SC0B_2F,  ucTmp); //gain for all band
		_scWriteByte(SC0B_50,  ucSharpness);	//bank1 coeff
		_scWriteByte(SC0B_51,  ucSharpness);	//bank2 coeff
		//_scWriteByte(SC0B_2A,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep)); //for band1~4
		//_scWriteByte(SC0B_2B,  (ucStep<<6 |ucStep<<4|ucStep<<2|ucStep)); //for band5~8
		_scWriteByteMask( SC0B_28, BIT0	, BIT0 );	//sub post peaking enable
    }


    _scWriteByte(SC00_00, ucBank);
}


//Main/Sub
void LoadACETable(BYTE bScalerWin, BYTE * pColorTable,BYTE ucHue,BYTE ucSaturation,BYTE ucSubContrast)// 2004/11/2
{
    //LOCAL_TYPE BYTE *pColorTable;
    //pColorTable=ppColorTable;
    g_ucVideoHue = ucHue;
    g_ucVideoSaturation = ucSaturation;
    g_ucVideoContrast = ucSubContrast;
    // color correct
    tColorCorrectionMatrix=(short *)((void*)pColorTable);
    msAdjustHSC(bScalerWin, g_ucVideoHue,g_ucVideoSaturation,g_ucVideoContrast);
}

code short tMatrix_SDTVR2Y_Limited[3][3]=
{
	{0x20B,  -0x1B6, -0x54}, // {0.511 ,	-0.428 ,	-0.083    },
    {0x132,   0x259,  0x74}, //{0.299 ,	0.587  ,	0.114     } ,
    {-0xB0, -0x15B, 0x20B}, // {-0.172	,-0.339,	0.511     }
};

code short tMatrix_SDTVY2R_Limited[3][3]=
{
	{0x57B,  0x400, 0},  // {1.3707	,1,	0    }
    {-0x2cA,  0x400,  -0x158}, // {-0.6982  ,	1,	-0.3365} ,
    {0,  0x400, 0x6ED},  // {0	,1,	1.7324          }
};

void msWriteXVColorMatrix(BYTE bScalerWin)
{
    BYTE LOCAL_TYPE i,j;
    DWORD  LOCAL_TYPE ucAddr ;// SC08_82//BYTE LOCAL_TYPE ucAddr = BK3_5E;
    short LOCAL_TYPE sTmp;
    BYTE LOCAL_TYPE u8Bank=_scReadByte(SC00_00);

    if(bScalerWin == MAIN_WINDOW)
        ucAddr = SC25_04;// SC08_82
    else
        ucAddr = SC25_24;//
    _scWriteByte(SC00_00, 0x25);

    for(i=0; i!=3; i++)
    {
        for(j=0; j!=3; j++)
        {
            sTmp = g_psMatrix[i*3+j];
            //printf( "[%X]:", ucAddr );
            if( sTmp >= 0 )
            {
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }
                _scWrite2Byte( ucAddr, sTmp );//mStar_WriteWord( ucAddr+1, sTmp );
                //printf( "%X\r\n", sTmp );
            }
            else
            {

                _scWrite2Byte( ucAddr, sTmp );//mStar_WriteWord( ucAddr+1, sTmp );
                //printf( "%X\r\n", sTmp );
            }
            ucAddr += 2;
        }
    }
    _scWriteByte(SC00_00, u8Bank);
}

//Main/Sub
void msSetXVYccVideoColorMatrix(BYTE bScalerWin)
{
	BYTE LOCAL_TYPE ucBank;

	ucBank = _scReadByte(SC00_00); // Backup bank
/*
    //int i = 0;
    XDATA short sResultTmp1[3][3];
    XDATA short sResultTmp2[3][3];
    typedef XDATA short TypArr[3][3];
    BYTE LOCAL_TYPE ucBank;
    short* LOCAL_TYPE psOutTab;

    //BYTE LOCAL_TYPE bCMEn;

//    sFirst=g_sVideoSatHueMatrix[bScalerWin];
//	sSecond = tMatrix_SDTVR2Y_Limited;
//    sResult=sResultTmp1;
///////    ArrayMultiply(g_sVideoSatHueMatrix[bScalerWin], tMatrix_SDTVR2Y_Limited, sResultTmp1);
//    sFirst=tMatrix_SDTVY2R_Limited;//sFirst=g_sVideoContrastMatrix[bScalerWin];
//    sSecond=sResultTmp1;
//    sResult=sResultTmp2;
//////    ArrayMultiply(tMatrix_SDTVY2R_Limited, sResultTmp1, sResultTmp2);

    psOutTab = (short*)sResultTmp2;

//    sFirst=g_sVideoContrastMatrix[bScalerWin];//sFirst=tMatrix_SDTVY2R_Limited;
#if 0 //Leo-temp
    sSecond=(TypArr)psOutTab;
#else
//    sSecond=psOutTab;
#endif
//    sResult=sResultTmp1;
////    ArrayMultiply(g_sVideoContrastMatrix[bScalerWin], psOutTab, sResultTmp1);

    psOutTab = (short*)sResultTmp1;

    ucBank = _scReadByte(SC00_00); // Backup bank

    psMatrix=psOutTab;

    msWriteXVColorMatrix(bScalerWin);
*/
    _scWriteByte(SC00_00, 0x25);
    if (bScalerWin == MAIN_WINDOW)
    {
        _scWriteByteMask(SC25_02, BIT1|BIT7, BIT1|BIT7);
        //_scWriteByteMask(SC0E_03, BIT2, BIT2);
    }
    else
    {
	    _scWriteByteMask(SC25_22, BIT1|BIT7, BIT1|BIT7);
        //_scWriteByteMask(SC0E_23, BIT2, BIT2);
    }

    _scWriteByte(SC00_00, ucBank); // Restore bank

}

//Main/Sub
void msAdjustXVYccVideoSaturation(BYTE bScalerWin, BYTE ucSaturation )
{
     g_ucVideoSaturation = ucSaturation;
    SetVideoSatHueMatrix(bScalerWin);
    msSetXVYccVideoColorMatrix(bScalerWin);
}

//Main/Sub
void msAdjustXVYccVideoHue(BYTE bScalerWin, BYTE ucHue )
{
    g_ucVideoHue = ucHue;
    SetVideoSatHueMatrix(bScalerWin);
    msSetXVYccVideoColorMatrix( bScalerWin);
}

void msAdjustXVYCCVideoContrastRGB(BYTE bScalerWin, BYTE ucContrast, BYTE ucRCon, BYTE ucGCon, BYTE ucBCon)
{
    g_ucVideoContrast = ucContrast;
    g_ucVideoRCon = ucRCon;
    g_ucVideoGCon = ucGCon;
    g_ucVideoBCon = ucBCon;
    SetVideoContrastMatrix(bScalerWin);
    msSetXVYccVideoColorMatrix(bScalerWin);
}


// ---------- PreGain /PostGain from MST9U ----------
BOOL msForceUseBothContrast_Get( BYTE ucWinIndex)
{
    return s_AceInfo[ucWinIndex].bIsForceUseBothContrast;
}

void msForceUseBothContrast( BYTE ucWinIndex, BOOL bForceUseBothContrast)
{
    s_AceInfo[ucWinIndex].bIsForceUseBothContrast = bForceUseBothContrast;
}

void msAdjustRGBContrast(BYTE ucWinIndex )
{
    if (s_AceInfo[ucWinIndex].bIsForceUseBothContrast)
    {
    	#if ENABLE_ACE_ADVANCE
        msAdjustPostRGBContrast(ucWinIndex);
		#endif
        msAdjustPreRGBContrast(ucWinIndex);
    }
    else if (s_AceInfo[ucWinIndex].bIsUsePostContrast)
    {
    	#if ENABLE_ACE_ADVANCE
        msAdjustPostRGBContrast(ucWinIndex);
		#endif
    }
    else
    {
        msAdjustPreRGBContrast(ucWinIndex);
    }
}

void msUsePostContrast( BYTE ucWinIndex, BOOL bUsePostContrast )
{
    s_AceInfo[ucWinIndex].bIsUsePostContrast = bUsePostContrast; // It can't work while "ACE_ADVANCE 0", Contrast/Gain will be fixed in 3x3 matrix.
}

void msEnablePreContrast(BYTE ucWinIndex, BOOL bEnable)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
    	msWriteByteMask(SC0F_A6, (bEnable?BIT0:0), BIT0);
    }
    else
    {
        msWriteByteMask(SC0F_A7, (bEnable?BIT0:0), BIT0);
    }
}

void msEnablePreOffset(BYTE ucWinIndex, BOOL bEnable)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
    	msWriteByteMask(SC0F_A6, (bEnable?BIT1:0), BIT1);
    }
    else
    {
        msWriteByteMask(SC0F_A7, (bEnable?BIT1:0), BIT1);
    }
}

void msEnablePostContrast( BYTE ucWinIndex,BOOL bEnable)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
        msWriteByteMask(SC25_02, (bEnable?BIT5:0), BIT5);
    }
    else
    {
        msWriteByteMask(SC25_22, (bEnable?BIT5:0), BIT5);
    }
}

void msEnablePostOffset( BYTE ucWinIndex,BOOL bEnable)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
    	msWriteByteMask(SC25_02, (bEnable?BIT4:0), BIT4);
    }
    else
    {
        msWriteByteMask(SC25_22, (bEnable?BIT4:0), BIT4);
    }
}

void msAdjustPreRGBContrast(BYTE ucWinIndex)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
        if (s_AceInfo[ucWinIndex].bIsForceUseBothContrast)
	    {
            msWrite2Byte(SC0F_8E, s_AceInfo[ucWinIndex].wRCon);
            msWrite2Byte(SC0F_90, s_AceInfo[ucWinIndex].wGCon);
            msWrite2Byte(SC0F_92, s_AceInfo[ucWinIndex].wBCon);
        }
        else
        {
            msWrite2Byte(SC0F_8E, s_AceInfo[ucWinIndex].sRGBContrast[0]);
            msWrite2Byte(SC0F_90, s_AceInfo[ucWinIndex].sRGBContrast[1]);
            msWrite2Byte(SC0F_92, s_AceInfo[ucWinIndex].sRGBContrast[2]);
        }
    }
    else
    {
        if (s_AceInfo[ucWinIndex].bIsForceUseBothContrast)
	    {
            msWrite2Byte(SC0F_94, s_AceInfo[ucWinIndex].wRCon);
            msWrite2Byte(SC0F_96, s_AceInfo[ucWinIndex].wGCon);
            msWrite2Byte(SC0F_98, s_AceInfo[ucWinIndex].wBCon);
        }
        else
        {
            msWrite2Byte(SC0F_94, s_AceInfo[ucWinIndex].sRGBContrast[0]);
            msWrite2Byte(SC0F_96, s_AceInfo[ucWinIndex].sRGBContrast[1]);
            msWrite2Byte(SC0F_98, s_AceInfo[ucWinIndex].sRGBContrast[2]);
        }
    }

    msEnablePreContrast(ucWinIndex, _ENABLE);
}

void msAdjustPreRGBOffset(BYTE ucWinIndex)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
        msWrite2Byte(SC0F_9A, s_AceInfo[ucWinIndex].sROffset);
        msWrite2Byte(SC0F_9C, s_AceInfo[ucWinIndex].sGOffset);
        msWrite2Byte(SC0F_9E, s_AceInfo[ucWinIndex].sBOffset);
    }
    else
    {
        msWrite2Byte(SC0F_A0, s_AceInfo[ucWinIndex].sROffset);
        msWrite2Byte(SC0F_A2, s_AceInfo[ucWinIndex].sGOffset);
        msWrite2Byte(SC0F_A4, s_AceInfo[ucWinIndex].sBOffset);
    }

    msEnablePreOffset(ucWinIndex, _ENABLE);
}

void msAdjustPostRGBContrast(BYTE ucWinIndex)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
        msWrite2Byte(SC25_48, s_AceInfo[ucWinIndex].sRGBContrast[0]);
        msWrite2Byte(SC25_4A, s_AceInfo[ucWinIndex].sRGBContrast[1]);
        msWrite2Byte(SC25_4C, s_AceInfo[ucWinIndex].sRGBContrast[2]);
    }
    else
    {
        msWrite2Byte(SC25_54, s_AceInfo[ucWinIndex].sRGBContrast[0]);
        msWrite2Byte(SC25_56, s_AceInfo[ucWinIndex].sRGBContrast[1]);
        msWrite2Byte(SC25_58, s_AceInfo[ucWinIndex].sRGBContrast[2]);
    }

    msEnablePostContrast(ucWinIndex, _ENABLE);
}

void msAdjustPostRGBOffset(BYTE ucWinIndex)
{
    if(ucWinIndex == MAIN_WINDOW)
    {
        msWrite2Byte(SC25_42, s_AceInfo[ucWinIndex].sRGBOffset[0]);
        msWrite2Byte(SC25_44, s_AceInfo[ucWinIndex].sRGBOffset[1]);
        msWrite2Byte(SC25_46, s_AceInfo[ucWinIndex].sRGBOffset[2]);
    }
    else
    {
        msWrite2Byte(SC25_4E, s_AceInfo[ucWinIndex].sRGBOffset[0]);
        msWrite2Byte(SC25_50, s_AceInfo[ucWinIndex].sRGBOffset[1]);
        msWrite2Byte(SC25_52, s_AceInfo[ucWinIndex].sRGBOffset[2]);
    }

    msEnablePostOffset(ucWinIndex, _ENABLE);
}

void msSetUserRGBGain(BYTE ucWinIndex, WORD u16RCon, WORD u16GCon, WORD u16BCon)
{
    s_AceInfo[ucWinIndex].wRCon =  u16RCon;
    s_AceInfo[ucWinIndex].wGCon =  u16GCon;
    s_AceInfo[ucWinIndex].wBCon =  u16BCon;
}

void msSetRGBGainContrast(BYTE ucWinIndex)
{
    s_AceInfo[ucWinIndex].sRGBContrast[0] =  ((WORD)s_AceInfo[ucWinIndex].sRGBContrast[0] * (s_AceInfo[ucWinIndex].wContrast)) >> 7;
    s_AceInfo[ucWinIndex].sRGBContrast[1] =  ((WORD)s_AceInfo[ucWinIndex].sRGBContrast[1] * (s_AceInfo[ucWinIndex].wContrast)) >> 7;
    s_AceInfo[ucWinIndex].sRGBContrast[2] =  ((WORD)s_AceInfo[ucWinIndex].sRGBContrast[2] * (s_AceInfo[ucWinIndex].wContrast)) >> 7;

    s_AceInfo[ucWinIndex].sRGBContrast[0] =  ((WORD)s_AceInfo[ucWinIndex].sRGBContrast[0]  * (g_ucSubContrast)) >> 10;
    s_AceInfo[ucWinIndex].sRGBContrast[1] =  ((WORD)s_AceInfo[ucWinIndex].sRGBContrast[1]  * (g_ucSubContrast)) >> 10;
    s_AceInfo[ucWinIndex].sRGBContrast[2] =  ((WORD)s_AceInfo[ucWinIndex].sRGBContrast[2]  * (g_ucSubContrast)) >> 10;
}

void msSetContrast( BYTE ucWinIndex, WORD u16Contrast)
{
    s_AceInfo[ucWinIndex].wContrast = u16Contrast;
    
    mdrv_DeltaE_RGBGain_Get((WORD*)&(s_AceInfo[ucWinIndex].sRGBContrast[0]), (WORD*)&(s_AceInfo[ucWinIndex].sRGBContrast[1]), (WORD*)&(s_AceInfo[ucWinIndex].sRGBContrast[2]));
    msSetRGBGainContrast(ucWinIndex);
}

void msSetRGBGain(BYTE ucWinIndex, WORD u16RCon, WORD u16GCon, WORD u16BCon)
{
    s_AceInfo[ucWinIndex].sRGBContrast[0] =  u16RCon;
    s_AceInfo[ucWinIndex].sRGBContrast[1] =  u16GCon;
    s_AceInfo[ucWinIndex].sRGBContrast[2] =  u16BCon;
    msSetRGBGainContrast(ucWinIndex);
}

void msSetRGBContrast(BYTE ucWinIndex, WORD u16Contrast, WORD u16RCon, WORD u16GCon, WORD u16BCon)
{
    /*printf("[msSetRGBContrast]u16Contrast:%x\n", u16Contrast);
    printf("[msSetRGBContrast]u16RCon:%x\n", u16RCon);
    printf("[msSetRGBContrast]u16GCon:%x\n", u16GCon);
    printf("[msSetRGBContrast]u16BCon:%x\n", u16BCon);*/

    #if !ENABLE_COLORTEMP_WORD_FORMAT
    u16RCon  = GetRealRgbGain(u16RCon);
    u16GCon= GetRealRgbGain(u16GCon);
    u16BCon = GetRealRgbGain(u16BCon);
    #endif
    /*printf("[GetRealRgbGain]u16RCon:%x\n", u16RCon);
    printf("[GetRealRgbGain]u16GCon:%x\n", u16GCon);
    printf("[GetRealRgbGain]u16BCon:%x\n", u16BCon);*/
    BYTE shiftBit = (COLOR_ACCURACY == COLOR_8_BIT)?3:6;
        
    s_AceInfo[ucWinIndex].sRGBContrast[0] =  ( (DWORD)u16RCon * (u16Contrast)) >> shiftBit;
    s_AceInfo[ucWinIndex].sRGBContrast[1] =  ( (DWORD)u16GCon * (u16Contrast)) >> shiftBit;
    s_AceInfo[ucWinIndex].sRGBContrast[2] =  ( (DWORD)u16BCon * (u16Contrast)) >> shiftBit;
    /*printf("[msSetRGBContrast]R:%x\n", s_AceInfo[ucWinIndex].sRGBContrast[0]);
    printf("[msSetRGBContrast]G:%x\n", s_AceInfo[ucWinIndex].sRGBContrast[1]);
    printf("[msSetRGBContrast]B:%x\n", s_AceInfo[ucWinIndex].sRGBContrast[2]);*/
}
void msSetPostOffset(BYTE ucWinIndex, WORD u16Roffset, WORD u16Goffset, WORD u16Boffset)
{
    //post offset
    s_AceInfo[ucWinIndex].sRGBOffset[0] =  u16Roffset;
    s_AceInfo[ucWinIndex].sRGBOffset[1] =  u16Goffset;
    s_AceInfo[ucWinIndex].sRGBOffset[2] =  u16Boffset;
}
void msSetRGBOffset(BYTE ucWinIndex, BYTE ucROffset, BYTE ucGOffset, BYTE ucBOffset)
{
    //pre offset
    s_AceInfo[ucWinIndex].sROffset = (WORD)ucROffset << 3;
    s_AceInfo[ucWinIndex].sGOffset = (WORD)ucGOffset <<3;
    s_AceInfo[ucWinIndex].sBOffset = (WORD)ucBOffset << 3;
}

#if ENABLE_FULL_RGB_COLOR_PATH
void msACECSCControl(void)
{
    WORD* matrix;
    xdata ST_COMBO_COLOR_FORMAT cf;
    cf = IsOSDCSCControl(OSD_CONTROL_CSC);

    //RGB Full
    if ( (cf.ucColorRange!= COMBO_COLOR_RANGE_LIMIT && cf.ucColorType== COMBO_COLOR_FORMAT_RGB) ||(cf.ucColorType == COMBO_COLOR_FORMAT_UNKNOWN) )
    {
        msACESetIPR2YEnable(FALSE);

        msACESetIPR2YSub16Enable(R_CR, FALSE);
        msACESetIPR2YSub16Enable(B_CB, FALSE);
        msACESetIPR2YSub16Enable(G_Y, FALSE);

        msACESetIPR2YRange(R_CR, FALSE);
        msACESetIPR2YRange(B_CB, FALSE);
    }
    else
    {
        msACESetIPR2YEnable(TRUE);
        if (cf.ucColorType == COMBO_COLOR_FORMAT_RGB && cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
        {
            msACESetIPR2YSub16Enable(R_CR, TRUE);
            msACESetIPR2YSub16Enable(B_CB, TRUE);

            msACESetIPR2YRange(R_CR, FALSE);
            msACESetIPR2YRange(B_CB, FALSE);
        }
        else
        {
            msACESetIPR2YSub16Enable(R_CR, FALSE);
            msACESetIPR2YSub16Enable(B_CB, FALSE);

            msACESetIPR2YRange(R_CR, TRUE);
            msACESetIPR2YRange(B_CB, TRUE);
        }

        if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT) // for limit range
            msACESetIPR2YSub16Enable(G_Y, TRUE);
        else
            msACESetIPR2YSub16Enable(G_Y, FALSE);



        if (cf.ucColorType == COMBO_COLOR_FORMAT_RGB) // rgb limit
            matrix = (WORD*)tIPMatrixForLimitRGB;
        else
        {
            switch (cf.ucColorimetry)
            {

                default:
                case COMBO_COLORIMETRY_NONE:
                case COMBO_COLORIMETRY_xvYCC709:
                case COMBO_COLORIMETRY_ITU709:
                case COMBO_COLORIMETRY_ADOBERBG:
                case COMBO_COLORIMETRY_BT2020YcCbcCrc:
                {
                    if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                    {
                        matrix = (WORD*)tIPMatrixForLimit709;
                    }
                    else
                    {
                        matrix = (WORD*)tIPMatrixForFull709;
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
                        matrix = (WORD*)tIPMatrixForLimit601;
                    }
                    else
                    {
                        matrix = (WORD*)tIPMatrixForFull601;
                    }

                }
                break;
                case COMBO_COLORIMETRY_BT2020RGBYCbCr:
                {
                    if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                    {
                        matrix = (WORD*)tIPMatrixForLimit2020NCL;
                    }
                    else
                    {
                        matrix = (WORD*)tIPMatrixForFull2020NCL;
                    }
                }

            }

        }
        if (matrix != NULL)
        {
            msACEWriteIPR2YMatrix(matrix);
        }
    }
}
#else
void msACECSCControl(void)
{
    //WORD* matrix;
    xdata ST_COMBO_COLOR_FORMAT cf;
    cf = IsOSDCSCControl(OSD_CONTROL_CSC);/////
    short gain = (short)(((1024 * 10 * 1.1678)+5)/10);
    float fgain = 1.1678;//0x4AC
    if(mdrv_Adjust_EnableNonStdCSC_Get())//AS input RGB16-254
    {
        fgain =1.0746;//0x44C =255/(254-16)
        //HDR output color range only support 16-235 in case of limit Range 
        #if ENABLE_HDR
        if (msGetHDRStatus(MAIN_WINDOW) != HDR_OFF) 
        { 
            fgain =1.1678;//0x4AC
        }
        #endif
    }
    else
    {   
        fgain =1.1678;//0x4AC
    }
    gain = (short)(((1024 * 10 * fgain)+5)/10);
    msACESetRGBMode( (g_WinColor[0]== WIN_COLOR_YUV) ? FALSE : TRUE);

    if (cf.ucColorType== COMBO_COLOR_FORMAT_RGB || cf.ucColorType == COMBO_COLOR_FORMAT_DEFAULT)
    {
        if(g_WinColor[0]== WIN_COLOR_YUV)
        {
            //, R to Y path, Set R2Y
            //printf("###msACECSCControl_R to Y path, Set R2Y");
	    	msWriteByteMask( SC07_40, BIT4|BIT0, BIT4|BIT0);    // CSC enable
	    	msWriteByteMask(SC0F_30, 0, BIT3);  //B-16, FOR RGB domain
            msWriteByteMask(SC0F_31, 0, BIT1);  //R-16, FOR RGB domain
            if (_CheckCurrentColorRange()== WIN_COLOR_RANGE_LIMIT)
            {
                //RGB Limit to Y
                //printf("Case1 : RGB-> YUV , range = Limit\r\n");
                msWriteByteMask(SC07_41, FALSE, BIT4|BIT0);  // csc range coef. disable
                msWriteByteMask(SC0B_A0, BIT0|BIT4, BIT0|BIT4);
                msWriteByteMask(SC0F_AE, BIT6|BIT7, BIT6|BIT7);
            }
            else
            {
                //RGB Full to Y
                //printf("Case2 : RGB-> YUV , range = FULL\r\n");
    	        msWriteByteMask(SC07_41, BIT4|BIT0, BIT4|BIT0);  // csc range coef. enable
                //msWriteByteMask(SC0F_AE, BIT6, BIT6);
                msWriteByteMask(SC0F_AE, BIT6|BIT7, BIT6|BIT7);
                msWriteByteMask(SC0B_A0, BIT0|BIT4, BIT0|BIT4);
            }
        }
        else
        {
            // Full RGB path
            msWriteByteMask(SC07_40, FALSE, BIT4|BIT0);
            if (_CheckCurrentColorRange()== WIN_COLOR_RANGE_LIMIT)
            {
                //printf("Case3 : RGB-> RGB , range = Limit\r\n");
                g_sPCRGBMatrix_Range[MAIN_WINDOW][0][0] = g_sPCRGBMatrix_Range[MAIN_WINDOW][1][1] = g_sPCRGBMatrix_Range[MAIN_WINDOW][2][2] = gain;
                g_sPCRGBMatrix_Range[SUB_WINDOW][0][0] = g_sPCRGBMatrix_Range[SUB_WINDOW][1][1] = g_sPCRGBMatrix_Range[SUB_WINDOW][2][2] = gain;
                
                //msWriteByteMask(SC0F_AE, BIT6, BIT6);
                msWriteByteMask(SC0F_AE, BIT6|BIT7, BIT6|BIT7);
                msWriteByteMask(SC0F_30, BIT3, BIT3);
                msWriteByteMask(SC0F_31, BIT1, BIT1);
            }
            else
            {
                // keep full path
                //printf("Case4 : RGB-> RGB , range = Limit\r\n");
                g_sPCRGBMatrix_Range[MAIN_WINDOW][0][0] = g_sPCRGBMatrix_Range[MAIN_WINDOW][1][1] = g_sPCRGBMatrix_Range[MAIN_WINDOW][2][2] = 1024;
                g_sPCRGBMatrix_Range[SUB_WINDOW][0][0] = g_sPCRGBMatrix_Range[SUB_WINDOW][1][1] = g_sPCRGBMatrix_Range[SUB_WINDOW][2][2] = 1024;

                msWriteByteMask(SC0F_AE, 0, BIT6|BIT7);
                msWriteByteMask(SC0F_30, 0, BIT3);
                msWriteByteMask(SC0F_31, 0, BIT1);
            }
                
            
        }
    }
    else    //for YUV input, disable CSC2_Y2R
    {
		 msWriteByteMask(SC07_40, FALSE, BIT4|BIT0);
         msWriteByteMask(SC0F_30, 0, BIT3);  //B-16, FOR RGB domain
         msWriteByteMask(SC0F_31, 0, BIT1);  //R-16, FOR RGB domain

        if (cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
        {
                //YUV Limit
                //printf("Case5 : YUV-> RGB , range = Limit\r\n");
            	msWriteByteMask(SC0F_AE, BIT6|BIT7, BIT6|BIT7);
                #if ENABLE_HDR
                if(mapi_Adjust_EnableNonStdCSC_Get() && (msGetHDRStatus(MAIN_WINDOW) == HDR_OFF))
                #else
                if(mapi_Adjust_EnableNonStdCSC_Get())
                #endif
                {
                    if (cf.ucYuvColorimetry == COMBO_YUV_COLORIMETRY_ITU601)
                    g_MatrixIdx = (ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT_NONSTD);    
                else
                    g_MatrixIdx = (ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT_NONSTD);
                }
                else
                {
                if (cf.ucYuvColorimetry == COMBO_YUV_COLORIMETRY_ITU601)
                    g_MatrixIdx = (ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT);    
                else
                    g_MatrixIdx = (ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT);    
                }    
        }
        else 
        {
                //YUV FULL, disable Y-16 & 1.164 gain on 3x3 matrix.
                //printf("Case6 : YUV-> RGB , range = Limit\r\n");
                msWriteByteMask(SC0F_AE, 0, BIT6|BIT7);
                if (cf.ucYuvColorimetry == COMBO_YUV_COLORIMETRY_ITU601)
                    g_MatrixIdx = (ACE_YUV_TO_RGB_MATRIX_SDTV);    
                else
                    g_MatrixIdx = (ACE_YUV_TO_RGB_MATRIX_HDTV);   
        }

        
    }
}
#endif

ST_COMBO_COLOR_FORMAT IsOSDCSCControl(BOOL bEnable)
{
    xdata ST_COMBO_COLOR_FORMAT cf;
    cf = msAPI_combo_IPGetColorFormat(SrcInputType);

    if(bEnable)
    {
        // Set color format based on OSD
        if (UserPrefInputColorFormat == INPUTCOLOR_RGB)
            cf.ucColorType = COMBO_COLOR_FORMAT_RGB;
        else if (UserPrefInputColorFormat == INPUTCOLOR_YUV)
            cf.ucColorType = COMBO_COLOR_FORMAT_YUV_444;

        // Set color range based on OSD
        if (UserPrefInputColorRange == INPUTCOLOR_FULL)
            cf.ucColorRange = COMBO_COLOR_RANGE_FULL;
        else if (UserPrefInputColorRange == INPUTCOLOR_LIMIT)
            cf.ucColorRange = COMBO_COLOR_RANGE_LIMIT;
    }

    return cf;
}

void msACESetEnableY2RMatrix(BYTE bScalerWin, BOOL bEnable)
{
   MENU_LOAD_START(); 
    // Main & Sub
    if(MAIN_WINDOW == bScalerWin)
       DB_WBMask( SC10_5E,bEnable? (BIT5|BIT4):0, BIT5|BIT4);// msWriteByteMask( SC10_5E,bEnable? (BIT5|BIT4):0, BIT5|BIT4);
    else
        DB_WBMask( SC0F_4C,bEnable? (BIT5|BIT4):0, BIT5|BIT4);//msWriteByteMask( SC0F_4C,bEnable? (BIT5|BIT4):0, BIT5|BIT4);

#if (!ENABLE_FULL_RGB_COLOR_PATH)
    if(MAIN_WINDOW == bScalerWin)
        DB_WBMask( SC10_5E,bEnable? (BIT2|BIT0):0, BIT2|BIT0);//msWriteByteMask( SC10_5E,bEnable? (BIT2|BIT0):0, BIT2|BIT0);
    else
        DB_WBMask( SC0F_4C,bEnable? (BIT2|BIT0):0, BIT2|BIT0);//msWriteByteMask( SC0F_4C,bEnable? (BIT2|BIT0):0, BIT2|BIT0);
#endif
  MENU_LOAD_END();
}

void msACESetRGBMode(BOOL bEnable)
{
	if(bEnable)
	{
		msWriteByte( SC0B_A0, 0x11);//rgb to y for 2d Peaking(main/sub_pk_video_in)
		msWriteByte( SC0B_B0, 0x01);//reg_vip0_rgb_mode_sel
		msWriteByte( SC0B_B2, 0x14);
		msWriteByte( SC0B_B3, 0x14);
		msWriteByte( SC0B_B4, 0x14);
		msWriteByte( SC0B_B5, 0x14);
	}
	else
	{
		msWriteByte( SC0B_A0, 0x11);
		msWriteByte( SC0B_B0, 0x00);
		msWriteByte( SC0B_B2, 0x01);
		msWriteByte( SC0B_B3, 0x01);
		msWriteByte( SC0B_B4, 0x01);
		msWriteByte( SC0B_B5, 0x01);
	}
}
//Unused msACESetCSC
void msACESetCSC(BOOL bEnable, BYTE bSel, BOOL bSubEnable)
{
	msWriteByteMask( SC07_40, bEnable, BIT0);
	msWriteByteMask( SC07_41, bSel, BIT1|BIT0);
	msWriteByteMask( SC0F_AE, bSubEnable, BIT6);
}

void msACESetWinColor(BOOL bMain, E_WIN_COLOR_FORMAT cf)
{
	BYTE u8WinIdx = bMain? 0 : 1;

	g_WinColor[u8WinIdx] = cf;

	mStar_WaitForDataBlanking(); 

	msACECSCControl();
	//mStar_WaitForDataBlanking(); 
	if(g_WinColor[u8WinIdx] == WIN_COLOR_RGB)
	{
		msSetPCColorMatrix(u8WinIdx);
	}
	else
	{
		msSetVideoColorMatrix(u8WinIdx);
	}
}

void msACESetWinColorRange(BOOL bMain, E_WIN_COLOR_RANGE cr)
{
	BYTE idx = bMain? 0 : 1;

	g_WinColorRange[idx] = cr;

	mStar_WaitForDataBlanking(); 

	msACECSCControl();
	//mStar_WaitForDataBlanking(); 
	if(g_WinColor[idx]== WIN_COLOR_RGB)
	{
		msSetPCColorMatrix(idx);
	}
	else
	{
		msSetVideoColorMatrix(idx);
	}
}

E_WIN_COLOR_FORMAT msACEGetWinColor(void)
{
	E_WIN_COLOR_FORMAT cf = g_WinColor[0];
	return cf;
}

E_WIN_COLOR_RANGE msACEGetWinColorRange(void)
{
	E_WIN_COLOR_RANGE   cr = _CheckCurrentColorRange();
	return cr;
}
