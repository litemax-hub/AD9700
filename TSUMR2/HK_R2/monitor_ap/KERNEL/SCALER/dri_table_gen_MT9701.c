// HSY.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "datatype.h"
#include "Ms_rwreg.h"
#include "ms_reg.h"
#include "dri_table_gen_MT9701.h"
#include "Common.h"

#define TBC 0
int gdH_UI[6], gdS_UI[6], gdY_UI[6];

// for different color space compensation points, refer MATLAB: ColorSpace_Ref.m
#if 0
// 709
const int HueTable[24] = { 1756, 2025, 2363, 2705, 2983, 3234, 3494, 3729, 3920, 4110, 4338, 4588, 4828, 5096, 5433, 5776, 6054, 162, 423, 657, 848, 1037, 1265, 1515 };
const int SatTable[24] = { 525, 450, 420, 444, 514, 489, 499, 541, 609, 546, 507, 500, 525, 451, 421, 444, 514, 489, 499, 541, 609, 546, 507, 500 };
const int LumaTable[24] = { 217, 401, 584, 767, 949, 895, 841, 786, 732, 750, 769, 787, 806, 623, 440, 257, 74, 128, 183, 237, 291, 273, 254, 236 };
#else
// 601, maxRGB is 1024, assume U+ is 0 degree
const int HueTable[24] = {1854, 2090, 2373, 2663, 2914, 3164, 3448, 3723, 3953, 4179, 4437, 4698, 4926, 5161, 5444, 5734, 5986, 93, 378, 652, 881, 1106, 1364, 1625};
const int SatTable[24] = {540, 479, 453, 467, 518, 471, 459, 486, 546, 496, 477, 493, 540, 479, 453, 467, 518, 470, 459, 486, 546, 496, 477, 492};
const int LumaTable[24] = {306, 456, 606, 757, 906, 830, 754, 677, 601, 630, 659, 688, 717, 567, 417, 267, 117, 193, 270, 346, 422, 393, 364, 335};
#endif
const WORD SkinToneTable[11] = {0x1F8, 0x193, 0x12E, 0xC9, 0x64, 0x00, 0x798, 0x738, 0x6D8, 0x668, 0x608};

void INIT_TABLE_GENERATOR(int* dHt, int* dSt, int*dYt, int* dSt_h, int* dYt_h, int* dSt_s, int* dYt_s, int* dYt_y)
{
    int i; 
    for ( i = 0; i < MST9U6_NUM_REF_AXIS; i++)
    {
        dHt[i] = DH_DEFAULT_VALUE;

        dSt[i] = DS_DEFAULT_VALUE;
        dSt_h[i] = DS_DEFAULT_VALUE;
        dSt_s[i] = DS_DEFAULT_VALUE;

        dYt[i] = DY_DEFAULT_VALUE;
        dYt_h[i] = DY_DEFAULT_VALUE;
        dYt_s[i] = DY_DEFAULT_VALUE;
        dYt_y[i] = DY_DEFAULT_VALUE;
    }
}

void USER_SAT_LUMA_BLEND(int color, int* dS_UI, int *dY_UI, int* dS_UI_24, int* dY_UI_24)
{
	//int alpha2_tmp = ; 
	if (color < 0 || color>5)
		assert(0);

	int sat_alpha1_diff = ALPHA_DENOMINATOR - REG_SAT_ALPHA1 ;
	int sat_alpha2_diff = ALPHA_DENOMINATOR - REG_SAT_ALPHA2;
	int sat_alpha1 = REG_SAT_ALPHA1;
	int sat_alpha2 = REG_SAT_ALPHA2;

	//(v6)
	//int luma_alpha1_diff = ALPHA_DENOMINATOR - REG_LUMA_ALPHA1;
	//int luma_alpha2_diff = ALPHA_DENOMINATOR - REG_LUMA_ALPHA2;
	//int luma_alpha1 = REG_LUMA_ALPHA1;
	//int luma_alpha2 = REG_LUMA_ALPHA2;


	int nxtcolor = (color == 5) ? 0 : color + 1;
	
	int min_ds = min(dS_UI[color], dS_UI[nxtcolor]);
	int max_ds = max(dS_UI[color], dS_UI[nxtcolor]);

	//(v6)
	//int minabs_dy = min_abs(dY_UI[color], dY_UI[nxtcolor]);
	//int maxabs_dy = max_abs(dY_UI[color], dY_UI[nxtcolor]);
	
	dS_UI_24[color << 2] = dS_UI[color];
	dS_UI_24[(color << 2) + 1] = (dS_UI[color] * sat_alpha1_diff + min_ds * sat_alpha1) >> ALPHA_FRAC_BIT;
	dS_UI_24[(color << 2) + 2] = (min_ds * sat_alpha2_diff + max_ds * sat_alpha2) >> ALPHA_FRAC_BIT;
	dS_UI_24[(color << 2) + 3] = (dS_UI[nxtcolor] * sat_alpha1_diff + min_ds * sat_alpha1) >> ALPHA_FRAC_BIT;


#if 0//(v6)
	dY_UI_24[color << 2] = dY_UI[color];
	dY_UI_24[(color << 2) + 1] = (minabs_dy*luma_alpha1 + dY_UI[color] * luma_alpha1_diff) >> ALPHA_FRAC_BIT;
	dY_UI_24[(color << 2) + 2] = (minabs_dy*luma_alpha2_diff + maxabs_dy*luma_alpha2) >> ALPHA_FRAC_BIT;
	dY_UI_24[(color << 2) + 3] = (minabs_dy*luma_alpha1_diff + dY_UI[nxtcolor]*luma_alpha1) >> ALPHA_FRAC_BIT;
#else
	// peter-cc test(v7)
	// dY_UI_24[color << 2] = dY_UI[color];
	// dY_UI_24[(color << 2) + 1] = (dY_UI[nxtcolor] + 3*dY_UI[color]) >> 2;
	// dY_UI_24[(color << 2) + 2] = (2*dY_UI[nxtcolor] + 2 * dY_UI[color]) >> 2;
	// dY_UI_24[(color << 2) + 3] = ( 3 * dY_UI[nxtcolor] +dY_UI[color]) >> 2;
	// v8
	dY_UI_24[color << 2]        = dY_UI[color];
	dY_UI_24[(color << 2) + 1]  = (LUMA_ALPHA         * dY_UI[nxtcolor] + (32 - LUMA_ALPHA) * dY_UI[color]) >> LUMA_ALPHA_BETA_BIT;
	dY_UI_24[(color << 2) + 2]  = (LUMA_BETA           * dY_UI[nxtcolor] + (32 - LUMA_BETA)  *  dY_UI[color]) >> LUMA_ALPHA_BETA_BIT;
	dY_UI_24[(color << 2) + 3]  = ((32 - LUMA_ALPHA)  * dY_UI[nxtcolor] + LUMA_ALPHA       *  dY_UI[color]) >> LUMA_ALPHA_BETA_BIT;

#endif 

	
}


void HUE_UI(int color, int* dH_UI, int* dHt, int* dSt_h, int* dYt_h)
{
    if (color < 0 || color>5)
        assert(0);

    int hue_alpha1_diff = ALPHA_DENOMINATOR - REG_HUE_ALPHA1;
    int hue_alpha2_diff = ALPHA_DENOMINATOR - REG_HUE_ALPHA2;
    int hue_alpha1 = REG_HUE_ALPHA1;
    int hue_alpha2 = REG_HUE_ALPHA2;

    int nxtcolor = (color == 5) ? 0 : color + 1;

    // Generate dHt: Hue adjustment by user, 
    //               dSt_h: saturation adjustment cause from Hue
    //               dYt_h: Luma adjustment cause from Hue
    HUE_COMPENSATE(color << 2, dH_UI[color], dHt, dSt_h, dYt_h);
    HUE_COMPENSATE((color << 2) + 1, (dH_UI[color] * hue_alpha1_diff + dH_UI[nxtcolor] * hue_alpha1) >> ALPHA_FRAC_BIT, dHt, dSt_h, dYt_h);
    HUE_COMPENSATE((color << 2) + 2, (dH_UI[color] * hue_alpha2_diff + dH_UI[nxtcolor] * hue_alpha2) >> ALPHA_FRAC_BIT, dHt, dSt_h, dYt_h);
    HUE_COMPENSATE((color << 2) + 3, (dH_UI[color] * hue_alpha1 + dH_UI[nxtcolor] * hue_alpha1_diff ) >> ALPHA_FRAC_BIT, dHt, dSt_h, dYt_h);
}

void SAT_LUMA_UI(int color, int* dHt, int* dS_UI_24, int *dY_UI_24, int* dSt_h, int* dYt_h, int* dSt, int* dYt)
{
    if (color < 0 || color>5)
    	assert(0);

    SAT_LUMA_COMPENSATE(color << 2, dHt[color << 2], dS_UI_24, dY_UI_24, dSt_h, dYt_h, dSt, dYt);
    SAT_LUMA_COMPENSATE((color << 2) + 1, dHt[(color << 2) + 1],  dS_UI_24, dY_UI_24, dSt_h, dYt_h, dSt, dYt);
    SAT_LUMA_COMPENSATE((color << 2) + 2, dHt[(color << 2) + 2],  dS_UI_24, dY_UI_24, dSt_h, dYt_h, dSt, dYt);
    SAT_LUMA_COMPENSATE((color << 2) + 3, dHt[(color << 2) + 3],  dS_UI_24, dY_UI_24, dSt_h, dYt_h, dSt, dYt);
}

void HUE_COMPENSATE(int idx, int dH, int* dHt, int* dSt_h, int* dYt_h)
{
    int Hin = idx * H_DEGREE_15;  // Hin: 0 ~ 6144 (0 degree ~ 360 degree)
    int Huv = HueTable[idx];
    int UVin = SatTable[idx];
    int Yin = LumaTable[idx];

    int Hout = Hin + dH;
    if (Hout < 0)
        Hout = Hout + H_DEGREE_360;
    else if (Hout >= H_DEGREE_360)
        Hout = Hout - H_DEGREE_360;

    int idx_c = Hout / H_DEGREE_15;  // current idx, resolution: 15 degree
    int idx_n = (idx_c == 23) ? 0 : idx_c + 1;  // total 24 phases
    int offset = Hout - idx_c * H_DEGREE_15;

    int H_c = HueTable[idx_c];
    int H_n = HueTable[idx_n];

    H_n = (H_n > H_c)? H_n: (H_n + H_DEGREE_360);


    // blending according by 256-resolutions, then recover 8 bits (256-resolutions); +128 is for rounding
    int Hr = (H_c * (H_DEGREE_15 - offset) + H_n* offset + H_BLENDING_ROUNDING_FACTOR) >> H_DEGREE_15_BIT;  

    int dHout = Hr - Huv;
    if ((dHout > 0) && (dH < 0))
    {
        dHout = dHout - H_DEGREE_360;
    }
    else if ((dHout < 0) && (dH > 0))
    {
        dHout = dHout + H_DEGREE_360;
    }

#if ROTATION_LIMIT
    if (abs(dHout) > abs(dH))
        dHout = dH;
#endif


    int UVr = (SatTable[idx_c] * (H_DEGREE_15 - offset) + SatTable[idx_n] * offset + H_BLENDING_ROUNDING_FACTOR) >> H_DEGREE_15_BIT;
    int Yr = (LumaTable[idx_c] * (H_DEGREE_15 - offset) + LumaTable[idx_n] * offset + H_BLENDING_ROUNDING_FACTOR) >> H_DEGREE_15_BIT;

    // rigo: find min one, and protect dH range between: -1016 < dH < +1016
    dHt[idx] = minmax(dHout, DH_LOWER_BOUND, DH_UPPER_BOUND);

    dSt_h[idx] = UVr * S_RATIO_1x/ UVin;  // UV in format 1.7 so we multiple 128
    dYt_h[idx] = (Yr - Yin) * REG_HSL_UVNORM_MAX / UVin;  // In color engin, UVnorm is divided by 724. So we multiple "*724"
}

void SAT_LUMA_COMPENSATE(int idx, int dH, int* dS_UI_24, int* dY_UI_24, int* dSt_h, int* dYt_h, int* dSt, int* dYt)
{
    int Hin = idx * H_DEGREE_15;  // Hin: 0 ~ 6144 (0 degree ~ 360 degree)

    int Hout = Hin + dH;
    if (Hout < 0)
        Hout = Hout + H_DEGREE_360;
    else if (Hout >= H_DEGREE_360)
        Hout = Hout - H_DEGREE_360;

    int idx_c = Hout / H_DEGREE_15;  // current idx, resolution: 15 degree
    int idx_n = (idx_c == 23) ? 0 : idx_c + 1;  // total 24 phases
    int offset = Hout - idx_c * H_DEGREE_15;

    int UVr = (SatTable[idx_c] * (H_DEGREE_15 - offset) + SatTable[idx_n] * offset + H_BLENDING_ROUNDING_FACTOR) >> H_DEGREE_15_BIT;
    int Yr = (LumaTable[idx_c] * (H_DEGREE_15 - offset) + LumaTable[idx_n] * offset + H_BLENDING_ROUNDING_FACTOR) >> H_DEGREE_15_BIT;		

    int dS_after_hue = (dS_UI_24[idx_c] * (H_DEGREE_15 - offset) + dS_UI_24[idx_n] * offset + H_BLENDING_ROUNDING_FACTOR) >> H_DEGREE_15_BIT;
    int dY_after_hue = (dY_UI_24[idx_c] * (H_DEGREE_15 - offset) + dY_UI_24[idx_n] * offset + H_BLENDING_ROUNDING_FACTOR) >> H_DEGREE_15_BIT;

    dSt[idx] = minmax((dSt_h[idx] * dS_after_hue) >> S_FRAC_BIT, DS_LOWER_BOUND, DS_UPPER_BOUND); 

	int LY_REF = (L_REF*REG_HSL_L_Y_BLEND_ALPHA + LumaTable[idx]* (64-REG_HSL_L_Y_BLEND_ALPHA) + 32)>>6;
    int dYt_s   = ((Yr - LY_REF) * (dS_after_hue - S_RATIO_1x)) / S_RATIO_1x;
    dYt[idx] = minmax(dYt_h[idx] + ((dYt_s + dY_after_hue) * REG_HSL_UVNORM_MAX / UVr), DY_LOWER_BOUND, DY_UPPER_BOUND) ; 
}


// Interpolate 24-axis to 36-axis
void INTERPOLATE_24_TO_36_AXIS(int* dHt, int* dSt, int* dYt, int *dHt_MT9701, int* dSt_MT9701, int* dYt_MT9701)
{
	// MT9700: axis 0 - 4 (15 degree) extent to MT9701: 0 - 15 (3.75 degree)
	int k;
	for (k = 0; k < 4; k++)
	{
	// Hue
		dHt_MT9701[(k<<2)] = dHt[k];
		dHt_MT9701[(k<<2) + 1] = dHt[k] + ((dHt[k+1] - dHt[k])>> 2);
		dHt_MT9701[(k<<2) + 2] = dHt[k] + 2*((dHt[k+1] - dHt[k])>> 2); 
		dHt_MT9701[(k<<2) + 3] = dHt[k] + 3*((dHt[k+1] - dHt[k])>> 2); 

	// Sat
		dSt_MT9701[(k<<2)] = dSt[k];
		dSt_MT9701[(k<<2) + 1] = dSt[k] + ((dSt[k+1] - dSt[k])>> 2);
		dSt_MT9701[(k<<2) + 2] = dSt[k] + 2*((dSt[k+1] - dSt[k])>> 2); 
		dSt_MT9701[(k<<2) + 3] = dSt[k] + 3*((dSt[k+1] - dSt[k])>> 2); 		

	// Luma
		dYt_MT9701[(k<<2)] = dYt[k];
		dYt_MT9701[(k<<2) + 1] = dYt[k] + ((dYt[k+1] - dYt[k])>> 2);
		dYt_MT9701[(k<<2) + 2] = dYt[k] + 2*((dYt[k+1] - dYt[k])>> 2); 
		dYt_MT9701[(k<<2) + 3] = dYt[k] + 3*((dYt[k+1] - dYt[k])>> 2); 		
	}
	
	int m;
	// MT9701 remaing axis (16 - 35) is 15 degree, mapping to MT9700 axis (4-23) 
	for (k  = 4, m = 16; k < MT9700_NUM_REF_AXIS; k++, m++)
	{
		dHt_MT9701[m] = dHt[k]; // Hue
		dSt_MT9701[m] = dSt[k];	// Sat
		dYt_MT9701[m] = dYt[k];	// Luma
	}
}


void MST9U6_HUE_TABLE_WRITE(BYTE u8WinIdx, int *dHt)
{
    UNUSED(u8WinIdx);
    int i;
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx %2)  //Main
            {
                for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                {
                msWrite2Byte(u32BaseAddr + SC69_68 + (2*i), (WORD)(dHt[i]&0xfff));
                }
            }
            //else
            {
                //TBD.
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void MST9U6_SAT_TABLE_WRITE(BYTE u8WinIdx, int* dSt)
{
    UNUSED(u8WinIdx);
    int i;
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx % 2)  //Main
            {
                for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                {
                msWriteByte(u32BaseAddr + SC69_98 + i, (WORD)(dSt[i]&0xff));
                }
            }
            //else
            {
                //TBD.
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void MST9U6_LUMA_TABLE_WRITE(BYTE u8WinIdx, int* dYt)
{
    UNUSED(u8WinIdx);
    int i;
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx %2)  //Main
            {
                for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                {
                msWrite2Byte(u32BaseAddr + SC69_B0  + (2*i), (WORD)(dYt[i]&0x7ff));
                }
            }
            //else
            {
                //TBD.
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void MST9U6_DRIVER_TABLE_EN(BYTE u8WinIdx, Bool dri_enable)
{
    UNUSED(u8WinIdx);
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx %2)  //Main
            {
                // 1:driver 0:hw
                msWriteBit(u32BaseAddr + SC6B_03, dri_enable, BIT4);
            }
            //else
            {
                //TBD.
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void MST9U6_DRIVER_TABLE(BYTE u8WinIdx, int *dH_UI, int *dS_UI, int *dY_UI)
{
    //------------------ Init adjusment value --------------------------- 
    int num_user_axis = 6;
    int dHt[MST9U6_NUM_REF_AXIS], dSt[MST9U6_NUM_REF_AXIS], dYt[MST9U6_NUM_REF_AXIS];   // [MT9700]: dHt, dSt, dYt: final adjustment for N axis. 
    int dSt_h[MST9U6_NUM_REF_AXIS], dYt_h[MST9U6_NUM_REF_AXIS];  // dSt_h, dYt_h: sat/luma compensation cause from Hue adjust
    int dSt_s[MST9U6_NUM_REF_AXIS], dYt_s[MST9U6_NUM_REF_AXIS];  // dSt_s: sat adjustment casue from user, dYt_s: luma compensation cause from Sat adjust
    int dYt_y[MST9U6_NUM_REF_AXIS];   // dYt_y: luma adjusment cause from user 
    int dS_UI_24[MST9U6_NUM_REF_AXIS], dY_UI_24[MST9U6_NUM_REF_AXIS];

    INIT_TABLE_GENERATOR(dHt, dSt, dYt, dSt_h, dYt_h, dSt_s, dYt_s, dYt_y);

    int i; 
    for (i = 0; i < num_user_axis; i++)
    {
        //------------------Generate 24 axis Hue table  ------------------------
        // +60 degree:+1016; -60 degree: -1016 

        HUE_UI(i, dH_UI, dHt, dSt_h, dYt_h);

        USER_SAT_LUMA_BLEND(i , dS_UI, dY_UI, dS_UI_24, dY_UI_24);
    }

    for (i = 0; i < num_user_axis; i++)
    {
    SAT_LUMA_UI(i , dHt, dS_UI_24, dY_UI_24, dSt_h, dYt_h, dSt, dYt);
    }

    // write local table into HW SRAM 
    MST9U6_HUE_TABLE_WRITE(u8WinIdx, dHt);
    MST9U6_SAT_TABLE_WRITE(u8WinIdx, dSt);
    MST9U6_LUMA_TABLE_WRITE(u8WinIdx, dYt); 

    MST9U6_DRIVER_TABLE_EN(u8WinIdx, TRUE);
}

void MST9U6_HW_SET_OFFSET(BYTE u8WinIdx, int *dH_UI, int *dS_UI, int *dY_UI)
{
    UNUSED(u8WinIdx);
    int i;
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx %2)  //Main
            {
                for (i = 0; i < MST9U6_USER_NUM_AXIS; i++)
                {
                    msWrite2Byte(u32BaseAddr + SC6B_04 + (2*i), (dH_UI[i]&0x7ff));
                    msWriteByte(u32BaseAddr + SC6B_10 + i, (dS_UI[i]&0xff));
                    msWrite2Byte(u32BaseAddr + SC6B_16 + (2*i), (dY_UI[i]&0x7ff));				
                }
            }
            //else
            {
                //TBD.
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void MST9U6_HW_SET_INTERP_ALPHA(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
    int reg_hue_alpha1 = REG_HUE_ALPHA1;
    int reg_hue_alpha2 = REG_HUE_ALPHA2;
    int reg_sat_alpha1 = REG_SAT_ALPHA1;
    int reg_sat_alpha2 = REG_SAT_ALPHA2;
    int reg_luma_alpha1 = REG_LUMA_ALPHA1;
    int reg_luma_alpha2 = REG_LUMA_ALPHA2;	

    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx % 2)  //Main
            {
                msWriteByte(u32BaseAddr + SC6B_22, reg_hue_alpha1);
                msWriteByte(u32BaseAddr + SC6B_23, reg_hue_alpha2);
                msWriteByte(u32BaseAddr + SC6B_24, reg_sat_alpha1);
                msWriteByte(u32BaseAddr + SC6B_25, reg_sat_alpha2);
                msWriteByte(u32BaseAddr + SC6B_26, reg_luma_alpha1);
                msWriteByte(u32BaseAddr + SC6B_27, reg_luma_alpha2);				
            }
            //else
            {
                //TBD.
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void MST9U6_HW_EN_COMPENSATION(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    //while(u16ScalerMask)
    {
        //if(u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx %2)  //Main
            {
                msWriteBit(u32BaseAddr + SC6B_8B, TRUE, BIT6);	//reg_ui_protect_en = 1
                msWriteBit(u32BaseAddr + SC6B_8B, TRUE, BIT7);	//reg_compensate_fire = 1
            }
            //else
            {
                //TBD.
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void MST9U6_HW_TABLE(BYTE u8WinIdx, int *dH_UI, int *dS_UI, int *dY_UI)
{
    // set 6-axis user Hue/Sat/luma adjustment to registers  
    MST9U6_HW_SET_OFFSET(u8WinIdx, dH_UI, dS_UI, dY_UI);
    // set interpolation parameters to registers. (note: If alpha1/alpha2 not change, this step could be neglected.)
    MST9U6_HW_SET_INTERP_ALPHA(u8WinIdx);
    // enable compensation computing 
    MST9U6_HW_EN_COMPENSATION(u8WinIdx);
    // swtich to HW table generator.  
    MST9U6_DRIVER_TABLE_EN(u8WinIdx, FALSE);
}

#if(ENABLE_MSTV_UART_DEBUG && DEBUG_MODE)
void MST9U6_DEBUG_HUE_TABLE(BYTE u8WinIdx, HSL_COMPEN_GEN_SEL tablegen_sel)
{
    int i;
    BYTE u8ScalerIdx = 0;
    WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    if (tablegen_sel == GEN_BY_FW)
    {
        printData(" dbg_hue_fw_table \r\n ", 0);
        while(u16ScalerMask)
        {
            if(u16ScalerMask & BIT0)
            {
                u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

                if(u8ScalerIdx %2)  //Main
                {
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msRead2Byte(u32BaseAddr + SC69_68 + (2*i)));
                    }
                }
                else
                {
                    //TBD.
                }
            }
            u8ScalerIdx++;
            u16ScalerMask >>= 1;
        }
    }
    else if (tablegen_sel == GEN_BY_HW)
    {
        while(u16ScalerMask)
        {
            if(u16ScalerMask & BIT0)
            {
                u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

                if(u8ScalerIdx %2)  //Main
                {
                    printData(" dbg_hue_hw_table \r\n ", 0);
                    msWriteBit(u32BaseAddr + SC6B_03, 0, BIT7);   // switch to interpolation debug message 
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msRead2Byte(u32BaseAddr + SC8A_02 + (2*i)));
                    }

                    printData(" dbg_hue_interpolation_table \r\n ", 0);
                    msWriteBit(u32BaseAddr + SC6B_03, 1, BIT7);   // switch to interpolation debug message 
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msRead2Byte(u32BaseAddr + SC8A_02 + (2*i)));
                    }
                }
                else
                {
                    //TBD.
                }
            }
            u8ScalerIdx++;
            u16ScalerMask >>= 1;
        }
    }
    else
    {
        printf("Warning: table gen selection:error setting!\n");
    }

    printData("\r\n ", 0);
}


void MST9U6_DEBUG_SAT_TABLE(BYTE u8WinIdx, HSL_COMPEN_GEN_SEL tablegen_sel)
{
    int i;
    BYTE u8ScalerIdx = 0;
    WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    if (tablegen_sel == GEN_BY_FW)
    {
    	printData(" dbg_sat_fw_table \r\n ", 0);
    	while(u16ScalerMask)
    	{
            if(u16ScalerMask & BIT0)
            {
                u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

                if(u8ScalerIdx % 2)  //Main
                {
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msReadByte(u32BaseAddr + SC69_98 + i ));
                    }
                }
                else
                {
                    //TBD.
                }
            }
            u8ScalerIdx++;
            u16ScalerMask >>= 1;
    	}
    }
    else if (tablegen_sel == GEN_BY_HW)
    {
        while(u16ScalerMask)
        {
            if(u16ScalerMask & BIT0)
            {
                u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

                if(u8ScalerIdx % 2)  //Main
                {
                    printData(" dbg_sat_hw_table \r\n ", 0);
                    msWriteBit(u32BaseAddr + SC6B_03, 0, BIT7);   // switch to interpolation debug message 
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msReadByte(u32BaseAddr + SC8A_32 + i ));
                    }

                    printData(" dbg_sat_interpolation_table \r\n ", 0);
                    msWriteBit(u32BaseAddr + SC6B_03, 1, BIT7);   // switch to interpolation debug message 
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msReadByte(u32BaseAddr + SC69_32 + i ));
                    }
                }
                else
                {
                    //TBD.
                }
            }
            u8ScalerIdx++;
            u16ScalerMask >>= 1;
        }
    }
    else
    {
        printf("Warning: table gen selection:error setting!\n");
    }
    
    printData("\r\n ", 0);
}

void MST9U6_DEBUG_LUMA_TABLE(BYTE u8WinIdx, HSL_COMPEN_GEN_SEL tablegen_sel)
{
    int i;
    BYTE u8ScalerIdx = 0;
    WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

    if (tablegen_sel == GEN_BY_FW)
    {
        printData(" dbg_luma_fw_table \r\n ", 0);
        while(u16ScalerMask)
        {
            if(u16ScalerMask & BIT0)
            {
                u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

                if(u8ScalerIdx %2)  //Main
                {
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msRead2Byte(u32BaseAddr + SC69_B0  + (2*i)));
                    }
                }
                else
                {
                    //TBD.
                }
            }
            u8ScalerIdx++;
            u16ScalerMask >>= 1;
        }
    }
    else if (tablegen_sel == GEN_BY_HW)
    {
        while(u16ScalerMask)
        {
            if(u16ScalerMask & BIT0)
            {
                u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

                if(u8ScalerIdx % 2)  //Main
                {
                    printData(" dbg_luma_hw_table \r\n ", 0);
                    msWriteBit(u32BaseAddr + SC6B_03, 0, BIT7);   // switch to interpolation debug message 
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msRead2Byte(u32BaseAddr + SC8A_4A + (2*i)));
                    }

                    printData(" dbg_luma_interpolation_table \r\n ", 0);
                    msWriteBit(u32BaseAddr + SC6B_03, 1, BIT7);   // switch to interpolation debug message 
                    for (i = 0; i < MST9U6_NUM_REF_AXIS; i++)
                    {
                        printf("  %x ",  msRead2Byte(u32BaseAddr + SC8A_4A + (2*i)));
                    }
                }
                else
                {
                    //TBD.
                }
            }
            u8ScalerIdx++;
            u16ScalerMask >>= 1;
        }
    }
    else
    {
        printf("Warning: table gen selection:error setting!\n");
    }

    printData("\r\n ", 0);
}
#endif




/*-------------------------------------------------------------------------
	Function	:  ColorEngine_REG_INIT
	Input	:  *HSL_REG
	Output	: N/A
	Feature	: set HW register value 
*--------------------------------------------------------------------------*/
void ColorEngine_REG_INIT(stHSL_REG *HSL_REG_MAIN)
{
/*=========================================
		Bank: hsv_reg_main (BK_87)
		Content: COLOR ENGINE
========================================== */
    /* h02 */
    HSL_REG_MAIN->reg_hsl_hue_en = 1;
    HSL_REG_MAIN->reg_hsl_lig_en = 1;
    HSL_REG_MAIN->reg_hsl_lig_by_lig_en = 1;
    HSL_REG_MAIN->reg_hsl_sat_en = 1;
    HSL_REG_MAIN->reg_hsl_global_hue_by_hue_en = 1;
    HSL_REG_MAIN->reg_hsl_global_luma_by_hue_en = 1;
    HSL_REG_MAIN->reg_hsl_global_sat_en = 1;

    /* h03 */
    HSL_REG_MAIN->reg_hsl_hue_gp_en = 1;
    HSL_REG_MAIN->reg_hsl_sat_gp_en = 1;
    HSL_REG_MAIN->reg_hsl_lig_gp_en =  1;
    HSL_REG_MAIN->reg_en_cursor_display = 0; 
    HSL_REG_MAIN->reg_en_debug_pix = 0;
    HSL_REG_MAIN->reg_hsl_hue_dither_en = 0;
    HSL_REG_MAIN->reg_hsl_sat_dither_en = 0;
    HSL_REG_MAIN->reg_hsl_luma_dither_en = 0;	
    HSL_REG_MAIN->reg_hsl_sy2rgb_dither_en = 0;
    HSL_REG_MAIN->reg_sat_global_blending_en = 1;

    /* h04 */
    HSL_REG_MAIN ->reg_hsl_sat_post_max_limit = 4095;

    /* h05 */
    HSL_REG_MAIN->reg_hsl_uvnorm_scale = REG_HSL_UVNORM_SCALE;

    /* h06 */
    HSL_REG_MAIN->reg_hsl_rgb_min = -2047;

    /* h07 */
    HSL_REG_MAIN->reg_hsl_rgb_max = 2047;

    /* h08 */	
    HSL_REG_MAIN->reg_hsl_global_hue_delta_theta = 0;

    /* h0e */
    HSL_REG_MAIN->reg_hsl_hue_degree_initial = 0;

    /* h0f */
    HSL_REG_MAIN->reg_hsl_hue_delta_amp = 0;

    /* h10 */
    HSL_REG_MAIN->reg_r2y_12 = -429;

    /* h11 */	
    HSL_REG_MAIN->reg_r2y_13 = -83;

    /* h12 */
    HSL_REG_MAIN->reg_r2y_21 = 306;

    /* h13 */
    HSL_REG_MAIN->reg_r2y_22 = 601;

    /* h14 */
    HSL_REG_MAIN->reg_r2y_23 = 117;

    /* h15 */
    HSL_REG_MAIN->reg_r2y_31 = -173;

    /* h16 */
    HSL_REG_MAIN->reg_r2y_32 = -339;

    /* h17 */
    HSL_REG_MAIN->reg_hsl_hue_gray = 0;
    HSL_REG_MAIN->reg_hsl_sat_gray = 0;

    //* h1e */
    HSL_REG_MAIN->reg_hsl_lig_gp_thrd = 0;

    /* h1f */
    HSL_REG_MAIN->reg_hsl_lig_gp_min_strength = 63;
    HSL_REG_MAIN->reg_hsl_lig_gp_slope = 63;


    /* h34 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[0] = 0;
    /* h35 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[1] = 0;
    /* h36 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[2] = 0;	
    /* h37 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[3] = 0;
    /* h38 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[4] = 0;
    /* h39 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[5] = 0;
    /* h3a */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[6] = 0;
    /* h3b */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[7] = 0;
    /* h3c */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[8] = 0;
    /* h3d */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[9] = 0;
    /* h3e */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[10] = 0;
    /* h3f */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[11] = 0;
    /* h40 */
    HSL_REG_MAIN ->reg_driver_Monitor_Hue_Table[12] = 0;
    /* h41 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[13] = 0;
    /* h42 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[14] = 0;
    /* h43 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[15] = 0;
    /* h44 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[16] = 0;
    /* h45 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[17] = 0;
    /* h46 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[18] = 0;
    /* h47*/
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[19] = 0;
    /* h48 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[20] = 0;
    /* h49 */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[21] = 0;
    /* h4a */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[22] = 0;
    /* h4b */
    HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[23] = 0;

    /* h4c */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[0] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[1] = 0x80;

    /* h4d */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[2] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[3] = 0x80;

    /* h4e */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[4] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[5] = 0x80;

    /* h4f */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[6] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[7] = 0x80;

    /* h50 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[8] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[9] = 0x80;

    /* h51 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[10] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[11] = 0x80;

    /* h52 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[12] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[13] = 0x80;

    /* h53 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[14] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[15] = 0x80;

    /* h54 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[16] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[17] = 0x80;

    /* h55 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[18] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[19] = 0x80;

    /* h56 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[20] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[21] = 0x80;

    /* h57 */
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[22] = 0x80;
    HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[23] = 0x80;

    /* h58 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[0] = 0;

    /* h59 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[1] = 0;

    /* h5a */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[2] = 0;

    /* h5b */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[3] = 0;

    /* h5c */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[4] = 0;

    /* h5d */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[5] = 0;

    /* h5e */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[6] = 0;

    /* h5f */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[7] = 0;

    /* h60 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[8] = 0;

    /* h61 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[9] = 0;

    /* h62 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[10] = 0;

    /* h63 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[11] = 0;

    /* h64 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[12] = 0;

    /* h65 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[13] = 0;

    /* h66 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[14] = 0;

    /* h67 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[15] = 0;

    /* h68 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[16] = 0;

    /* h69 */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[17] = 0;

    /* h6a */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[18] = 0;

    /* h6b */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[19] = 0;

    /* h6c */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[20] = 0;

    /* h6d */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[21] = 0;

    /* h6e */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[22] = 0;

    /* h6f */
    HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[23] = 0;

    /* h7b */
    HSL_REG_MAIN->reg_luma_gray_table[0] = 0;
    HSL_REG_MAIN->reg_luma_gray_table[1] = 0;

    /* h7c */
    HSL_REG_MAIN->reg_luma_gray_table[2] = 0;
    HSL_REG_MAIN->reg_luma_gray_table[3] = 0;

    /* h7d */
    HSL_REG_MAIN->reg_luma_gray_table[4] = 0;
    HSL_REG_MAIN->reg_luma_gray_table[5] = 0;

    /* h7e */
    HSL_REG_MAIN->reg_luma_gray_table[6] = 0;
    HSL_REG_MAIN->reg_luma_gray_table[7] = 0;

    /* h7f */
    HSL_REG_MAIN->reg_luma_gray_table[8] = 0;


/*=========================================
		Bank: hsv_reg_sub0 (BK_88)
		Content: FCC 
========================================== */

    /* h03 */
    HSL_REG_MAIN->reg_hsl_window_h_start = 55;
    HSL_REG_MAIN->reg_hsl_mwe_enable = 0;
    /* h04 */
    HSL_REG_MAIN->reg_hsl_window_h_end = 1072;
    /* h05 */
    HSL_REG_MAIN->reg_hsl_window_v_start = 7503;

    /* h06 */
    HSL_REG_MAIN->reg_hsl_window_v_end = 4699;

    /* h17 */	
    HSL_REG_MAIN->reg_hsl_path_bypass = 0;

    /* h18 */
    HSL_REG_MAIN->reg_hsl_uvangle_scale_0 = 742;
    /* h19 */
    HSL_REG_MAIN->reg_hsl_uvangle_offset_0 = 425;
    /* h1a */
    HSL_REG_MAIN->reg_hsl_uvangle_scale_1 = 758;
    /* h1b */
    HSL_REG_MAIN->reg_hsl_uvangle_offset_1 = 390;
    /* h1c */
    HSL_REG_MAIN->reg_hsl_uvangle_scale_2 = 808;
    /* h1d */
    HSL_REG_MAIN->reg_hsl_uvangle_offset_2 = 750;

    /* h20 */
    HSL_REG_MAIN->reg_y2r_11 = 0x59C;
    /* h21 */
    HSL_REG_MAIN->reg_y2r_21 = 0x1D25;
    /* h22 */
    HSL_REG_MAIN->reg_y2r_23 = 0x1EA0;
    /* h23 */
    HSL_REG_MAIN->reg_y2r_33 = 0x717;

/*---  FCC: start  ---*/
    /* h24 */
    HSL_REG_MAIN->reg_favor_region_huel_1 = 192;
    /* h25 */
    HSL_REG_MAIN->reg_favor_region_hueu_1 = 688;
    /* h26 */
    HSL_REG_MAIN->reg_favor_region_satl_1 = 0;
    /* h27 */
    HSL_REG_MAIN->reg_favor_region_satu_1 = 1200;
    /* h28 */
    HSL_REG_MAIN->reg_favor_region_lumal_1 = 0;
    /* h29 */
    HSL_REG_MAIN->reg_favor_region_lumau_1 = 4095;
    /* h2b */
    HSL_REG_MAIN->reg_favor_hue_gain_1 = 12;
    /* h2c */
    HSL_REG_MAIN->reg_favor_sat_gain_1 = 5;
    HSL_REG_MAIN->reg_favor_luma_gain_1 = 2;
    /* h2d */
    HSL_REG_MAIN->reg_dh_favor_adjust_1 = 0;
    /* h2e */
    HSL_REG_MAIN->reg_ds_favor_adjust_1 = 255;
    /* h2f */
    HSL_REG_MAIN->reg_dy_favor_adjust_1 = 0;

    /* h30 */
    HSL_REG_MAIN->reg_favor_region_huel_2 = 1200;
    /* h31 */
    HSL_REG_MAIN->reg_favor_region_hueu_2 = 2048;
    /* h32 */
    HSL_REG_MAIN->reg_favor_region_satl_2 = 0;
    /* h33 */
    HSL_REG_MAIN->reg_favor_region_satu_2 = 4095;
    /* h34 */
    HSL_REG_MAIN->reg_favor_region_lumal_2 = 0;
    /* h35 */
    HSL_REG_MAIN->reg_favor_region_lumau_2 = 4095;
    /* h37 */	
    HSL_REG_MAIN->reg_favor_hue_gain_2 = 7;
    /* h38 */
    HSL_REG_MAIN->reg_favor_sat_gain_2 = 2;
    HSL_REG_MAIN->reg_favor_luma_gain_2 = 2;
    /* h39 */
    HSL_REG_MAIN->reg_dh_favor_adjust_2 = 0;
    /* h3a */
    HSL_REG_MAIN->reg_ds_favor_adjust_2 = 255;
    /* h3b */
    HSL_REG_MAIN->reg_dy_favor_adjust_2 = 0;

    /* h3c */
    HSL_REG_MAIN->reg_favor_region_huel_3 = 4000;
    /* h3d */
    HSL_REG_MAIN->reg_favor_region_hueu_3 = 5120;
    /* h3e */
    HSL_REG_MAIN->reg_favor_region_satl_3 = 0;
    /* h3f */
    HSL_REG_MAIN->reg_favor_region_satu_3 = 4095;
    /* h40 */
    HSL_REG_MAIN->reg_favor_region_lumal_3 = 0;
    /* h41 */
    HSL_REG_MAIN->reg_favor_region_lumau_3 = 4095;
    /* h43 */
    HSL_REG_MAIN->reg_favor_hue_gain_3 = 5;
    /* h44 */
    HSL_REG_MAIN->reg_favor_sat_gain_3 = 2;
    HSL_REG_MAIN->reg_favor_luma_gain_3 = 2;
    /* h45 */
    HSL_REG_MAIN->reg_dh_favor_adjust_3 = 0;
    /* h46 */
    HSL_REG_MAIN->reg_ds_favor_adjust_3 = 255;
    /* h47 */
    HSL_REG_MAIN->reg_dy_favor_adjust_3 = 0;

    /* h48 */
    HSL_REG_MAIN->reg_favor_region_huel_4 = 3200;
    /* h49 */
    HSL_REG_MAIN->reg_favor_region_hueu_4 = 3800;
    /* h4a */
    HSL_REG_MAIN->reg_favor_region_satl_4 = 0;
    /* h4b */
    HSL_REG_MAIN->reg_favor_region_satu_4 = 4095;
    /* h4c */
    HSL_REG_MAIN->reg_favor_region_lumal_4 = 0;
    /* h4d */
    HSL_REG_MAIN->reg_favor_region_lumau_4 = 4095;
    /* h4f */
    HSL_REG_MAIN->reg_favor_hue_gain_4 = 10;

    /* h50 */
    HSL_REG_MAIN->reg_favor_sat_gain_4 = 2;	
    HSL_REG_MAIN->reg_favor_luma_gain_4 = 2;

    /* h51 */
    HSL_REG_MAIN->reg_dh_favor_adjust_4 = 0;
    /* h52 */
    HSL_REG_MAIN->reg_ds_favor_adjust_4 = 255;
    /* h53 */
    HSL_REG_MAIN->reg_dy_favor_adjust_4 = 0;

    /* h54 */
    HSL_REG_MAIN->reg_favor_region_huel_5 = 5800;
    /* h55 */
    HSL_REG_MAIN->reg_favor_region_hueu_5 = 5920;
    /* h56 */
    HSL_REG_MAIN->reg_favor_region_satl_5 = 0;
    /* h57 */
    HSL_REG_MAIN->reg_favor_region_satu_5 = 4095;	
    /* h58 */
    HSL_REG_MAIN->reg_favor_region_lumal_5 = 0;
    /* h59 */
    HSL_REG_MAIN->reg_favor_region_lumau_5 = 4095;
    /* h5b */
    HSL_REG_MAIN->reg_favor_hue_gain_5 = 51;
    /* h5c */
    HSL_REG_MAIN->reg_favor_sat_gain_5 = 2;
    HSL_REG_MAIN->reg_favor_luma_gain_5 = 2;
    /* h5d */
    HSL_REG_MAIN->reg_dh_favor_adjust_5 = 0;
    /* h5e */
    HSL_REG_MAIN->reg_ds_favor_adjust_5 = 255;
    /* h5f */
    HSL_REG_MAIN->reg_dy_favor_adjust_5 = 0;

    /* h60 */
    HSL_REG_MAIN->reg_favor_region = 0;
    HSL_REG_MAIN->reg_favor_region_1 = 1;
    HSL_REG_MAIN->reg_favor_region_2 = 1;
    HSL_REG_MAIN->reg_favor_region_3 = 1;
    HSL_REG_MAIN->reg_favor_region_4 = 1;
    HSL_REG_MAIN->reg_favor_region_5 = 1;
    HSL_REG_MAIN->reg_fcc_yin_sel = 0;

    /* h61 */
    HSL_REG_MAIN->reg_dh_favor_outside = 0;
    /* h62 */
    HSL_REG_MAIN->reg_ds_favor_outside = 128;
    /* h63 */
    HSL_REG_MAIN->reg_dy_favor_outside = 0;
    /* h63 */
    HSL_REG_MAIN->reg_hsl_fcc_outside_y_sel = 0;

/*---  FCC: end  ---*/


/*=========================================
		Bank:  hsv_reg_sub1 (BK_89)
		Content: compensation circuit 
========================================== */
    /* h01 */
    HSL_REG_MAIN->reg_hsl_uvnorm_max = REG_HSL_UVNORM_MAX;
    HSL_REG_MAIN->reg_driver_monitor_en = 0;

    /* h02*/
    HSL_REG_MAIN->reg_hue_ui_r = 0;
    /* h03 */
    HSL_REG_MAIN->reg_hue_ui_y = 0;
    /* h04 */
    HSL_REG_MAIN->reg_hue_ui_g = 0;
    /* h05 */
    HSL_REG_MAIN->reg_hue_ui_c = 0;
    /* h06 */
    HSL_REG_MAIN->reg_hue_ui_b = 0;
    /* h07 */
    HSL_REG_MAIN->reg_hue_ui_m = 0;			
    /* h08 */
    HSL_REG_MAIN->reg_sat_ui_r = 128;
    HSL_REG_MAIN->reg_sat_ui_y = 128;

    /* h09 */
    HSL_REG_MAIN->reg_sat_ui_g = 128;
    HSL_REG_MAIN->reg_sat_ui_c = 128;
    /* h0a */
    HSL_REG_MAIN->reg_sat_ui_b = 128;
    HSL_REG_MAIN->reg_sat_ui_m = 128;
    /* h0b */
    HSL_REG_MAIN->reg_luma_ui_r = 0;
    /* h0c */
    HSL_REG_MAIN->reg_luma_ui_y = 0;
    /* h0d */
    HSL_REG_MAIN->reg_luma_ui_g = 0;
    /* h0e  */
    HSL_REG_MAIN->reg_luma_ui_c = 0;
    /* h0f  */
    HSL_REG_MAIN->reg_luma_ui_b = 0;
    /* h10 */
    HSL_REG_MAIN->reg_luma_ui_m = 0;
    				
    /* h11 */
    HSL_REG_MAIN->reg_hue_alpha1 = 16;
    HSL_REG_MAIN->reg_hue_alpha2 = 32;
    /* h12 */
    HSL_REG_MAIN->reg_sat_alpha1 = 0;
    HSL_REG_MAIN->reg_sat_alpha2 = 32;
    /* h13 */
    HSL_REG_MAIN->reg_luma_alpha1 = 32;
    HSL_REG_MAIN->reg_luma_alpha2 = 0;


    /* h21 */
    HSL_REG_MAIN->reg_huephase_ref[0] = 0;
    HSL_REG_MAIN->reg_huephase_ref[1] = 59;
    /* h22 */
    HSL_REG_MAIN->reg_huephase_ref[2] = 130;
    HSL_REG_MAIN->reg_huephase_ref[3] = 202;
    /* h23 */
    HSL_REG_MAIN->reg_huephase_ref[4] = 9;
    HSL_REG_MAIN->reg_huephase_ref[5] = 72;
    /* h24 */
    HSL_REG_MAIN->reg_huephase_ref[6] = 143;
    HSL_REG_MAIN->reg_huephase_ref[7] = 211;
    /* h25 */
    HSL_REG_MAIN->reg_huephase_ref[8] = 13;
    HSL_REG_MAIN->reg_huephase_ref[9] = 69;
    /* h26 */
    HSL_REG_MAIN->reg_huephase_ref[10] = 134;
    HSL_REG_MAIN->reg_huephase_ref[11] = 199;
    /* h27 */
    HSL_REG_MAIN->reg_huephase_ref[12] = 0;
    HSL_REG_MAIN->reg_huephase_ref[13] = 59;
    /* h28 */
    HSL_REG_MAIN->reg_huephase_ref[14] = 129;
    HSL_REG_MAIN->reg_huephase_ref[15] = 202;
    /* h29 */
    HSL_REG_MAIN->reg_huephase_ref[16] = 9;
    HSL_REG_MAIN->reg_huephase_ref[17] = 72;
    /* h2a */
    HSL_REG_MAIN->reg_huephase_ref[18] = 143;
    HSL_REG_MAIN->reg_huephase_ref[19] = 212;	
    /* h2b */
    HSL_REG_MAIN->reg_huephase_ref[20] = 13;
    HSL_REG_MAIN->reg_huephase_ref[21] = 69;
    /* h2c */
    HSL_REG_MAIN->reg_huephase_ref[22] = 134;
    HSL_REG_MAIN->reg_huephase_ref[23] = 199;

    /* h2d */
    HSL_REG_MAIN->reg_satphase_ref[0] = 135;
    HSL_REG_MAIN->reg_satphase_ref[1] = 120;
    /* h2e */
    HSL_REG_MAIN->reg_satphase_ref[2] = 113;
    HSL_REG_MAIN->reg_satphase_ref[3] = 117;
    /* h2f */
    HSL_REG_MAIN->reg_satphase_ref[4] = 130;
    HSL_REG_MAIN->reg_satphase_ref[5] = 118;
    /* h30 */
    HSL_REG_MAIN->reg_satphase_ref[6] = 115;
    HSL_REG_MAIN->reg_satphase_ref[7] = 122;
    /* h31 */
    HSL_REG_MAIN->reg_satphase_ref[8] = 137;
    HSL_REG_MAIN->reg_satphase_ref[9] = 124;
    /* h32 */
    HSL_REG_MAIN->reg_satphase_ref[10] = 119;
    HSL_REG_MAIN->reg_satphase_ref[11] = 123;
    /* h33 */
    HSL_REG_MAIN->reg_satphase_ref[12] = 135;
    HSL_REG_MAIN->reg_satphase_ref[13] = 120;
    /* h34 */
    HSL_REG_MAIN->reg_satphase_ref[14] = 113;
    HSL_REG_MAIN->reg_satphase_ref[15] = 117;	
    /* h35 */
    HSL_REG_MAIN->reg_satphase_ref[16] = 130;
    HSL_REG_MAIN->reg_satphase_ref[17] = 118;
    /* h36 */
    HSL_REG_MAIN->reg_satphase_ref[18] = 115;
    HSL_REG_MAIN->reg_satphase_ref[19] = 122;
    /* h37 */
    HSL_REG_MAIN->reg_satphase_ref[20] = 137;
    HSL_REG_MAIN->reg_satphase_ref[21] = 124;
    /* h38 */
    HSL_REG_MAIN->reg_satphase_ref[22] = 119;
    HSL_REG_MAIN->reg_satphase_ref[23] = 123;


    /* h39 */
    HSL_REG_MAIN->reg_lumaphase_ref[0] = 76;
    HSL_REG_MAIN->reg_lumaphase_ref[1] = 114;
    /* h3a */
    HSL_REG_MAIN->reg_lumaphase_ref[2] = 152;
    HSL_REG_MAIN->reg_lumaphase_ref[3]  = 189;
    /* h3b */
    HSL_REG_MAIN->reg_lumaphase_ref[4] = 227;
    HSL_REG_MAIN->reg_lumaphase_ref[5] = 208;
    /* h3c */
    HSL_REG_MAIN->reg_lumaphase_ref[6] = 188;
    HSL_REG_MAIN->reg_lumaphase_ref[7] = 169;
    /* h3d */
    HSL_REG_MAIN->reg_lumaphase_ref[8] = 150; 
    HSL_REG_MAIN->reg_lumaphase_ref[9] = 157;
    /* h3e */
    HSL_REG_MAIN->reg_lumaphase_ref[10] = 165;
    HSL_REG_MAIN->reg_lumaphase_ref[11] = 172;
    /* h3f */
    HSL_REG_MAIN->reg_lumaphase_ref[12] = 179;
    HSL_REG_MAIN->reg_lumaphase_ref[13] = 141;
    /* h40 */
    HSL_REG_MAIN->reg_lumaphase_ref[14] = 103;
    HSL_REG_MAIN->reg_lumaphase_ref[15] = 66;
    /* h41 */
    HSL_REG_MAIN->reg_lumaphase_ref[16] = 28; 
    HSL_REG_MAIN->reg_lumaphase_ref[17] = 47;
    /* h42 */
    HSL_REG_MAIN->reg_lumaphase_ref[18] = 67;
    HSL_REG_MAIN->reg_lumaphase_ref[19] = 86;
    /* h43 */
    HSL_REG_MAIN->reg_lumaphase_ref[20] = 105;
    HSL_REG_MAIN->reg_lumaphase_ref[21] = 98;
    /* h44 */
    HSL_REG_MAIN->reg_lumaphase_ref[22] = 90;
    HSL_REG_MAIN->reg_lumaphase_ref[23] = 83;

    /* h45 */
    HSL_REG_MAIN->reg_compensate_start = 128;
    HSL_REG_MAIN->reg_ui_protect_en = 1;
    HSL_REG_MAIN->reg_compensate_fire = 0;

    /* h46 */
    HSL_REG_MAIN->reg_hsl_l_y_blend_alpha = REG_HSL_L_Y_BLEND_ALPHA;
    HSL_REG_MAIN->reg_hsl_sat_linear_blend = 0;
    HSL_REG_MAIN->reg_hsl_luma_linear_blend  = 0;
    HSL_REG_MAIN->reg_hsl_invuvin_base_hout = 1;
}

/*-------------------------------------------------------------------------
	Function	:  msAPI_LOAD_HSY_INIT_TABLE
	Input	: *HSL_REG
	Output	: N/A
	Feature	: write register setting into HW
*--------------------------------------------------------------------------*/
void	msAPI_LOAD_HSY_INIT_TABLE(BYTE u8WinIdx, stHSL_REG *HSL_REG_MAIN)
{
    UNUSED(u8WinIdx);
/*=========================================
		Bank: hsv_reg_main (BK_87)
		Content: COLOR ENGINE
========================================== */
    int BK87_reg[128] = {0}; 

    /* h02 */
    BK87_reg[2] += HSL_REG_MAIN->reg_hsl_hue_en & 0x1;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_lig_en & 0x1) << 4;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_lig_by_lig_en & 0x1) << 5;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_sat_en & 0x1) << 7;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_global_hue_by_hue_en & 0x1) << 11;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_global_luma_by_hue_en & 0x1) << 14;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_global_sat_en & 0x1) << 15;

    /* h03 */
    BK87_reg[3] += HSL_REG_MAIN->reg_hsl_hue_gp_en & 0x1;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_sat_gp_en & 0x1) << 1;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_lig_gp_en & 0x1) << 2;
    BK87_reg[3] += (HSL_REG_MAIN->reg_en_cursor_display & 0x1) << 6;
    BK87_reg[3] += (HSL_REG_MAIN->reg_en_debug_pix & 0x1) << 7;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_hue_dither_en & 0x1) << 8;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_sat_dither_en & 0x1) << 9;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_luma_dither_en & 0x1) << 10;	
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_sy2rgb_dither_en & 0x1) << 11;
    BK87_reg[3] += (HSL_REG_MAIN->reg_sat_global_blending_en & 0x1) << 15;

    /* h04 */
    BK87_reg[4] += HSL_REG_MAIN->reg_hsl_sat_post_max_limit & 0xfff;
    /* h05 */
    BK87_reg[5] += HSL_REG_MAIN->reg_hsl_uvnorm_scale & 0x7ff;
    /* h06 */
    BK87_reg[6] += HSL_REG_MAIN->reg_hsl_rgb_min & 0xfff;
    /* h07 */
    BK87_reg[7] += HSL_REG_MAIN->reg_hsl_rgb_max & 0xfff;
    /* h08 */
    BK87_reg[8] += HSL_REG_MAIN->reg_hsl_global_hue_delta_theta & 0x1ff;	
    /* h0e */
    BK87_reg[14] += HSL_REG_MAIN->reg_hsl_hue_degree_initial & 0x3ff;
    /* h0f */
    BK87_reg[15] += (HSL_REG_MAIN->reg_hsl_hue_delta_amp & 0x3) << 4;
    /* h10 */
    BK87_reg[16] += HSL_REG_MAIN->reg_r2y_12 & 0x7ff;
    /* h11 */	
    BK87_reg[17] += HSL_REG_MAIN->reg_r2y_13 & 0x7ff;
    /* h12 */
    BK87_reg[18] += HSL_REG_MAIN->reg_r2y_21 & 0x7ff;
    /* h13 */
    BK87_reg[19] += HSL_REG_MAIN->reg_r2y_22 & 0x7ff;
    /* h14 */
    BK87_reg[20] += HSL_REG_MAIN->reg_r2y_23 & 0x7ff;
    /* h15 */
    BK87_reg[21] += HSL_REG_MAIN->reg_r2y_31 & 0x7ff;
    /* h16 */
    BK87_reg[22] += HSL_REG_MAIN->reg_r2y_32 & 0x7ff;
    /* h17 */
    BK87_reg[23] += HSL_REG_MAIN->reg_hsl_hue_gray & 0xff;
    BK87_reg[23] += (HSL_REG_MAIN->reg_hsl_sat_gray & 0xff) << 8;
    //* h1e */
    BK87_reg[30] += HSL_REG_MAIN->reg_hsl_lig_gp_thrd & 0x3ff;
    /* h1f */
    BK87_reg[31] += HSL_REG_MAIN->reg_hsl_lig_gp_min_strength & 0x3f;
    BK87_reg[31] += (HSL_REG_MAIN->reg_hsl_lig_gp_slope & 0x3f) << 8;



    /* h34 */
    BK87_reg[52] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[0] & 0xfff;
    /* h35 */
    BK87_reg[53] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[1] & 0xfff;
    /* h36 */
    BK87_reg[54] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[2] & 0xfff;	
    /* h37 */
    BK87_reg[55] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[3] & 0xfff;
    /* h38 */
    BK87_reg[56] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[4] & 0xfff;
    /* h39 */
    BK87_reg[57] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[5] & 0xfff;
    /* h3a */
    BK87_reg[58] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[6] & 0xfff;
    /* h3b */
    BK87_reg[59] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[7] & 0xfff;
    /* h3c */
    BK87_reg[60] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[8] & 0xfff;
    /* h3d */
    BK87_reg[61] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[9] & 0xfff;
    /* h3e */
    BK87_reg[62] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[10] & 0xfff;
    /* h3f */
    BK87_reg[63] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[11] & 0xfff;
    /* h40 */
    BK87_reg[64] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[12] & 0xfff;
    /* h41 */
    BK87_reg[65] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[13] & 0xfff;
    /* h42 */
    BK87_reg[66] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[14] & 0xfff;
    /* h43 */
    BK87_reg[67] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[15] & 0xfff;
    /* h44 */
    BK87_reg[68] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[16] & 0xfff;
    /* h45 */
    BK87_reg[69] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[17] & 0xfff;
    /* h46 */
    BK87_reg[70] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[18] & 0xfff;
    /* h47*/
    BK87_reg[71] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[19] & 0xfff;															
    /* h48 */
    BK87_reg[72] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[20] & 0xfff;
    /* h49 */
    BK87_reg[73] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[21] & 0xfff;
    /* h4a */
    BK87_reg[74] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[22] & 0xfff;
    /* h4b */
    BK87_reg[75] += HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[23] & 0xfff;

    /* h4c */
    BK87_reg[76] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[0] & 0xff;
    BK87_reg[76] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[1] & 0xff) << 8;
    /* h4d */
    BK87_reg[77] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[2] & 0xff;
    BK87_reg[77] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[3] & 0xff) << 8;
    /* h4e */
    BK87_reg[78] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[4] & 0xff;
    BK87_reg[78] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[5] & 0xff) << 8;
    /* h4f */
    BK87_reg[79] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[6] & 0xff;
    BK87_reg[79] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[7] & 0xff) << 8;
    /* h50 */
    BK87_reg[80] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[8] & 0xff;
    BK87_reg[80] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[9] & 0xff) << 8;
    /* h51 */
    BK87_reg[81] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[10] & 0xff;
    BK87_reg[81] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[11] & 0xff) << 8;
    /* h52 */
    BK87_reg[82] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[12] & 0xff;
    BK87_reg[82] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[13] & 0xff) << 8;
    /* h53 */
    BK87_reg[83] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[14] & 0xff;
    BK87_reg[83] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[15] & 0xff) << 8;
    /* h54 */
    BK87_reg[84] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[16] & 0xff;
    BK87_reg[84] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[17] & 0xff) << 8;
    /* h55 */
    BK87_reg[85] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[18] & 0xff;
    BK87_reg[85] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[19] & 0xff) << 8;
    /* h56 */
    BK87_reg[86] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[20] & 0xff;
    BK87_reg[86] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[21] & 0xff) << 8;
    /* h57 */
    BK87_reg[87] += HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[22] & 0xff;
    BK87_reg[87] += (HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[23] & 0xff) << 8;

    /* h58 */
    BK87_reg[88] += (HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[0] & 0x7ff);
    /* h59 */
    BK87_reg[89] += (HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[1] & 0x7ff);
    /* h5a */
    BK87_reg[90] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[2] & 0x7ff;
    /* h5b */
    BK87_reg[91] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[3] & 0x7ff;	
    /* h5c */
    BK87_reg[92] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[4] & 0x7ff;
    /* h5d */
    BK87_reg[93] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[5] & 0x7ff;
    /* h5e */
    BK87_reg[94] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[6] & 0x7ff;	
    /* h5f */
    BK87_reg[95] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[7] & 0x7ff;
    /* h60 */
    BK87_reg[96] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[8] & 0x7ff;
    /* h61 */
    BK87_reg[97] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[9] & 0x7ff;	
    /* h62 */
    BK87_reg[98] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[10] & 0x7ff;
    /* h63 */
    BK87_reg[99] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[11] & 0x7ff;
    /* h64 */
    BK87_reg[100] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[12] & 0x7ff;	
    /* h65 */
    BK87_reg[101] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[13] & 0x7ff;
    /* h66 */
    BK87_reg[102] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[14] & 0x7ff;
    /* h67 */
    BK87_reg[103] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[15] & 0x7ff;	
    /* h68 */
    BK87_reg[104] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[16] & 0x7ff;
    /* h69 */
    BK87_reg[105] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[17] & 0x7ff;
    /* h6a */
    BK87_reg[106] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[18] & 0x7ff;	
    /* h6b */
    BK87_reg[107] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[19] & 0x7ff;
    /* h6c */
    BK87_reg[108] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[20] & 0x7ff;
    /* h6d */
    BK87_reg[109] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[21] & 0x7ff;	
    /* h6e */
    BK87_reg[110] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[22] & 0x7ff;
    /* h6f */
    BK87_reg[111] += HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[23] & 0x7ff;

    /* h7b */
    BK87_reg[123] += HSL_REG_MAIN->reg_luma_gray_table[0] & 0xff;
    BK87_reg[123] += (HSL_REG_MAIN->reg_luma_gray_table[1] & 0xff) << 8;
    /* h7c */
    BK87_reg[124] += HSL_REG_MAIN->reg_luma_gray_table[2] & 0xff;
    BK87_reg[124] += (HSL_REG_MAIN->reg_luma_gray_table[3] & 0xff) << 8;
    /* h7d */
    BK87_reg[125] += HSL_REG_MAIN->reg_luma_gray_table[4] & 0xff;
    BK87_reg[125] += (HSL_REG_MAIN->reg_luma_gray_table[5] & 0xff) << 8;
    /* h7e */
    BK87_reg[126] += HSL_REG_MAIN->reg_luma_gray_table[6] & 0xff;
    BK87_reg[126] += (HSL_REG_MAIN->reg_luma_gray_table[7] & 0xff) << 8;
    /* h7f */
    BK87_reg[127] += HSL_REG_MAIN->reg_luma_gray_table[8] & 0xff;


/*=========================================
		Bank: hsv_reg_sub0 (BK_6A)
		Content: FCC 
========================================== */
    int BK88_reg[128] = { 0 };
    /* h03 */
    BK88_reg[3] += HSL_REG_MAIN->reg_hsl_window_h_start & 0x1fff;
    BK88_reg[3] += (HSL_REG_MAIN->reg_hsl_mwe_enable & 01) << 15;
    /* h04 */
    BK88_reg[4] += HSL_REG_MAIN->reg_hsl_window_h_end & 0x1fff;
    /* h05 */
    BK88_reg[5] += HSL_REG_MAIN->reg_hsl_window_v_start & 0x1fff;
    /* h06 */
    BK88_reg[6] += HSL_REG_MAIN->reg_hsl_window_v_end & 0x1fff;
    /* h17 */	
    BK88_reg[23] +=HSL_REG_MAIN->reg_hsl_path_bypass & 0x1;
    /* h18 */
    BK88_reg[24] += HSL_REG_MAIN->reg_hsl_uvangle_scale_0 & 0x3ff;
    /* h19 */
    BK88_reg[25] += HSL_REG_MAIN->reg_hsl_uvangle_offset_0 & 0x3ff;
    /* h1a */
    BK88_reg[26] += HSL_REG_MAIN->reg_hsl_uvangle_scale_1 & 0x3ff;
    /* h1b */
    BK88_reg[27] += HSL_REG_MAIN->reg_hsl_uvangle_offset_1 & 0x3ff;
    /* h1c */
    BK88_reg[28] += HSL_REG_MAIN->reg_hsl_uvangle_scale_2 & 0x3ff;
    /* h1d */
    BK88_reg[29] += HSL_REG_MAIN->reg_hsl_uvangle_offset_2 & 0x3ff;

    /* h20 */
    BK88_reg[32] += HSL_REG_MAIN->reg_y2r_11 & 0x1fff;
    /* h21 */
    BK88_reg[33] += HSL_REG_MAIN->reg_y2r_21 & 0x1fff;
    /* h22 */
    BK88_reg[34] += HSL_REG_MAIN->reg_y2r_23 & 0x1fff;
    /* h23 */
    BK88_reg[35] += HSL_REG_MAIN->reg_y2r_33 & 0x1fff;

/*---  FCC: start  ---*/
    /* h24 */
    BK88_reg[36] += HSL_REG_MAIN->reg_favor_region_huel_1 & 0x1fff;
    /* h25 */
    BK88_reg[37] += HSL_REG_MAIN->reg_favor_region_hueu_1 & 0x1fff;
    /* h26 */
    BK88_reg[38] += HSL_REG_MAIN->reg_favor_region_satl_1 & 0xfff;
    /* h27 */
    BK88_reg[39] += HSL_REG_MAIN->reg_favor_region_satu_1 & 0xfff;
    /* h28 */
    BK88_reg[40] += HSL_REG_MAIN->reg_favor_region_lumal_1 & 0xfff;
    /* h29 */
    BK88_reg[41] += HSL_REG_MAIN->reg_favor_region_lumau_1 & 0xfff;
    /* h2b */
    BK88_reg[43] += (HSL_REG_MAIN->reg_favor_hue_gain_1 & 0xff) << 8;
    /* h2c */
    BK88_reg[44] += HSL_REG_MAIN->reg_favor_sat_gain_1 & 0xff;
    BK88_reg[44] += (HSL_REG_MAIN->reg_favor_luma_gain_1 & 0xff) << 8;
    /* h2d */
    BK88_reg[45] += HSL_REG_MAIN->reg_dh_favor_adjust_1 & 0x7ff;
    /* h2e */
    BK88_reg[46] += HSL_REG_MAIN->reg_ds_favor_adjust_1 & 0xff;
    /* h2f */
    BK88_reg[47] += HSL_REG_MAIN->reg_dy_favor_adjust_1 & 0x7ff;

    /* h30 */
    BK88_reg[48] += HSL_REG_MAIN->reg_favor_region_huel_2 & 0x1fff;
    /* h31 */
    BK88_reg[49] += HSL_REG_MAIN->reg_favor_region_hueu_2 & 0x1fff;
    /* h32 */
    BK88_reg[50] += HSL_REG_MAIN->reg_favor_region_satl_2 & 0xfff;
    /* h33 */
    BK88_reg[51] += HSL_REG_MAIN->reg_favor_region_satu_2 & 0xfff;
    /* h34 */
    BK88_reg[52] += HSL_REG_MAIN->reg_favor_region_lumal_2 & 0xfff;
    /* h35 */
    BK88_reg[53] += HSL_REG_MAIN->reg_favor_region_lumau_2 & 0xfff;
    /* h37 */	
    BK88_reg[55] += (HSL_REG_MAIN->reg_favor_hue_gain_2 & 0xff) << 8;
    /* h38 */
    BK88_reg[56] += HSL_REG_MAIN->reg_favor_sat_gain_2 & 0xff;
    BK88_reg[56] += (HSL_REG_MAIN->reg_favor_luma_gain_2 & 0xff) << 8;
    /* h39 */
    BK88_reg[57] += HSL_REG_MAIN->reg_dh_favor_adjust_2 & 0x7ff;
    /* h3a */
    BK88_reg[58] += HSL_REG_MAIN->reg_ds_favor_adjust_2 & 0xff;
    /* h3b */
    BK88_reg[59] += HSL_REG_MAIN->reg_dy_favor_adjust_2 & 0x7ff;

    /* h3c */
    BK88_reg[60] += HSL_REG_MAIN->reg_favor_region_huel_3 & 0x1fff;
    /* h3d */
    BK88_reg[61] += HSL_REG_MAIN->reg_favor_region_hueu_3 & 0x1fff;
    /* h3e */
    BK88_reg[62] += HSL_REG_MAIN->reg_favor_region_satl_3 & 0xfff;
    /* h3f */
    BK88_reg[63] += HSL_REG_MAIN->reg_favor_region_satu_3 & 0xfff;
    /* h40 */
    BK88_reg[64] += HSL_REG_MAIN->reg_favor_region_lumal_3 & 0xfff;
    /* h41 */
    BK88_reg[65] += HSL_REG_MAIN->reg_favor_region_lumau_3 & 0xfff;
    /* h43 */
    BK88_reg[67] += (HSL_REG_MAIN->reg_favor_hue_gain_3 & 0xff) << 8;
    /* h44 */
    BK88_reg[68] += HSL_REG_MAIN->reg_favor_sat_gain_3 & 0xff;
    BK88_reg[68] += (HSL_REG_MAIN->reg_favor_luma_gain_3 & 0xff) << 8;
    /* h45 */
    BK88_reg[69] += HSL_REG_MAIN->reg_dh_favor_adjust_3 & 0x7ff;
    /* h46 */
    BK88_reg[70] += HSL_REG_MAIN->reg_ds_favor_adjust_3 & 0xff;
    /* h47 */
    BK88_reg[71] += HSL_REG_MAIN->reg_dy_favor_adjust_3 & 0x7ff;

    /* h48 */
    BK88_reg[72] += HSL_REG_MAIN->reg_favor_region_huel_4 & 0x1fff;
    /* h49 */
    BK88_reg[73] += HSL_REG_MAIN->reg_favor_region_hueu_4 & 0x1fff;
    /* h4a */
    BK88_reg[74] += HSL_REG_MAIN->reg_favor_region_satl_4 & 0xfff;
    /* h4b */
    BK88_reg[75] += HSL_REG_MAIN->reg_favor_region_satu_4 & 0xfff;
    /* h4c */
    BK88_reg[76] += HSL_REG_MAIN->reg_favor_region_lumal_4 & 0xfff;
    /* h4d */
    BK88_reg[77] += HSL_REG_MAIN->reg_favor_region_lumau_4 & 0xfff;
    /* h4f */
    BK88_reg[79] += (HSL_REG_MAIN->reg_favor_hue_gain_4 & 0xff) << 8;		
    /* h50 */
    BK88_reg[80] += HSL_REG_MAIN->reg_favor_sat_gain_4 & 0xff;	
    BK88_reg[80] += (HSL_REG_MAIN->reg_favor_luma_gain_4 & 0xff) << 8;
    /* h51 */
    BK88_reg[81] += HSL_REG_MAIN->reg_dh_favor_adjust_4 & 0x7ff;
    /* h52 */
    BK88_reg[82] += HSL_REG_MAIN->reg_ds_favor_adjust_4 & 0xff;
    /* h53 */
    BK88_reg[83] += HSL_REG_MAIN->reg_dy_favor_adjust_4 & 0x7ff;

    /* h54 */
    BK88_reg[84] += HSL_REG_MAIN->reg_favor_region_huel_5 & 0x1fff;
    /* h55 */
    BK88_reg[85] += HSL_REG_MAIN->reg_favor_region_hueu_5 & 0x1fff;
    /* h56 */
    BK88_reg[86] += HSL_REG_MAIN->reg_favor_region_satl_5 & 0xfff;
    /* h57 */
    BK88_reg[87] += HSL_REG_MAIN->reg_favor_region_satu_5 & 0xfff;	
    /* h58 */
    BK88_reg[88] += HSL_REG_MAIN->reg_favor_region_lumal_5 & 0xfff;
    /* h59 */
    BK88_reg[89] += HSL_REG_MAIN->reg_favor_region_lumau_5 & 0xfff;
    /* h5b */
    BK88_reg[91] += (HSL_REG_MAIN->reg_favor_hue_gain_5 & 0xff) << 8;
    /* h5c */
    BK88_reg[92] += HSL_REG_MAIN->reg_favor_sat_gain_5 & 0xff;
    BK88_reg[92] += (HSL_REG_MAIN->reg_favor_luma_gain_5 & 0xff) << 8;
    /* h5d */
    BK88_reg[93] += HSL_REG_MAIN->reg_dh_favor_adjust_5 & 0x7ff;
    /* h5e */
    BK88_reg[94] += HSL_REG_MAIN->reg_ds_favor_adjust_5 & 0xff;
    /* h5f */
    BK88_reg[95] += HSL_REG_MAIN->reg_dy_favor_adjust_5 & 0x7ff;

    /* h60 */
    BK88_reg[96] += HSL_REG_MAIN->reg_favor_region & 0x1;
    BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_1 & 0x1) << 1;
    BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_2 & 0x1) << 2;
    BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_3 & 0x1) << 3;
    BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_4 & 0x1) << 4;
    BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_5 & 0x1) << 5;
    BK88_reg[96] += (HSL_REG_MAIN->reg_fcc_yin_sel & 0x1) << 7;

    /* h61 */
    BK88_reg[97] += HSL_REG_MAIN->reg_dh_favor_outside & 0x7ff;
    /* h62 */
    BK88_reg[98] += HSL_REG_MAIN->reg_ds_favor_outside & 0xff;
    /* h63 */
    BK88_reg[99] += HSL_REG_MAIN->reg_dy_favor_outside & 0x7ff;
    /* h63 */
    BK88_reg[99] += (HSL_REG_MAIN->reg_hsl_fcc_outside_y_sel & 0x1) << 15;
/*---  FCC: end  ---*/


/*=========================================
		Bank:  hsv_reg_sub1 (BK_6B)
		Content: compensation circuit 
========================================== */
    int BK89_reg[128] = { 0 };

    /* h01 */
    BK89_reg[1] += HSL_REG_MAIN->reg_hsl_uvnorm_max & 0x7ff;
    BK89_reg[1] += (HSL_REG_MAIN->reg_driver_monitor_en & 0x1) << 12;

    /* h02*/
    BK89_reg[2] += HSL_REG_MAIN->reg_hue_ui_r & 0x7ff;
    /* h03 */
    BK89_reg[3] += HSL_REG_MAIN->reg_hue_ui_y & 0x7ff;
    /* h04 */
    BK89_reg[4] += HSL_REG_MAIN->reg_hue_ui_g & 0x7ff;
    /* h05 */
    BK89_reg[5] += HSL_REG_MAIN->reg_hue_ui_c & 0x7ff;
    /* h06 */
    BK89_reg[6] += HSL_REG_MAIN->reg_hue_ui_b & 0x7ff;
    /* h07 */
    BK89_reg[7] += HSL_REG_MAIN->reg_hue_ui_m & 0x7ff;			
    /* h08 */
    BK89_reg[8] += HSL_REG_MAIN->reg_sat_ui_r & 0xff;
    BK89_reg[8] += (HSL_REG_MAIN->reg_sat_ui_y & 0xff) << 8;
    /* h09 */
    BK89_reg[9] += HSL_REG_MAIN->reg_sat_ui_g & 0xff;
    BK89_reg[9] += (HSL_REG_MAIN->reg_sat_ui_c & 0xff) << 8;
    /* h0a */
    BK89_reg[10] += HSL_REG_MAIN->reg_sat_ui_b & 0xff;
    BK89_reg[10] += (HSL_REG_MAIN->reg_sat_ui_m & 0xff) << 8;
    /* h0b */
    BK89_reg[11] += HSL_REG_MAIN->reg_luma_ui_r & 0x7ff;
    /* h0c */
    BK89_reg[12] += HSL_REG_MAIN->reg_luma_ui_y & 0x7ff;
    /* h0d */
    BK89_reg[13] += HSL_REG_MAIN->reg_luma_ui_g & 0x7ff;
    /* h0e  */
    BK89_reg[14] += HSL_REG_MAIN->reg_luma_ui_c & 0x7ff;
    /* h0f  */
    BK89_reg[15] += HSL_REG_MAIN->reg_luma_ui_b & 0x7ff;
    /* h10 */
    BK89_reg[16] += HSL_REG_MAIN->reg_luma_ui_m & 0x7ff;
    				
    /* h11 */
    BK89_reg[17] += HSL_REG_MAIN->reg_hue_alpha1 & 0x7f;
    BK89_reg[17] += (HSL_REG_MAIN->reg_hue_alpha2 & 0x7f) << 8;
    /* h12 */
    BK89_reg[18] += HSL_REG_MAIN->reg_sat_alpha1 & 0x7f;
    BK89_reg[18] += (HSL_REG_MAIN->reg_sat_alpha2 & 0x7f) << 8;
    /* h13 */
    BK89_reg[19] += HSL_REG_MAIN->reg_luma_alpha1 & 0x7f;
    BK89_reg[19] += (HSL_REG_MAIN->reg_luma_alpha2 & 0x7f) << 8;


    /* h21 */
    BK89_reg[33] += HSL_REG_MAIN->reg_huephase_ref[0] & 0xff;
    BK89_reg[33] += (HSL_REG_MAIN->reg_huephase_ref[1] & 0xff) << 8;
    /* h22 */
    BK89_reg[34] += HSL_REG_MAIN->reg_huephase_ref[2] & 0xff;
    BK89_reg[34] += (HSL_REG_MAIN->reg_huephase_ref[3] & 0xff) << 8;
    /* h23 */
    BK89_reg[35] += HSL_REG_MAIN->reg_huephase_ref[4] & 0xff;
    BK89_reg[35] += (HSL_REG_MAIN->reg_huephase_ref[5] & 0xff) << 8;
    /* h24 */
    BK89_reg[36] += HSL_REG_MAIN->reg_huephase_ref[6] & 0xff;
    BK89_reg[36] += (HSL_REG_MAIN->reg_huephase_ref[7] & 0xff) << 8;
    /* h25 */
    BK89_reg[37] += HSL_REG_MAIN->reg_huephase_ref[8] & 0xff;
    BK89_reg[37] += (HSL_REG_MAIN->reg_huephase_ref[9] & 0xff) << 8;
    /* h26 */
    BK89_reg[38] += HSL_REG_MAIN->reg_huephase_ref[10] & 0xff;
    BK89_reg[38] += (HSL_REG_MAIN->reg_huephase_ref[11] & 0xff) << 8;	
    /* h27 */
    BK89_reg[39] += HSL_REG_MAIN->reg_huephase_ref[12] & 0xff;
    BK89_reg[39] += (HSL_REG_MAIN->reg_huephase_ref[13] & 0xff) << 8;	
    /* h28 */
    BK89_reg[40] += HSL_REG_MAIN->reg_huephase_ref[14] & 0xff;
    BK89_reg[40] += (HSL_REG_MAIN->reg_huephase_ref[15] & 0xff) << 8;	
    /* h29 */
    BK89_reg[41] += HSL_REG_MAIN->reg_huephase_ref[16] & 0xff;
    BK89_reg[41] += (HSL_REG_MAIN->reg_huephase_ref[17] & 0xff) << 8;	
    /* h2a */
    BK89_reg[42] += HSL_REG_MAIN->reg_huephase_ref[18] & 0xff;
    BK89_reg[42] += (HSL_REG_MAIN->reg_huephase_ref[19] & 0xff) << 8;	
    /* h2b */
    BK89_reg[43] += HSL_REG_MAIN->reg_huephase_ref[20] & 0xff;
    BK89_reg[43] += (HSL_REG_MAIN->reg_huephase_ref[21] & 0xff) << 8;	
    /* h2c */
    BK89_reg[44] += HSL_REG_MAIN->reg_huephase_ref[22] & 0xff;
    BK89_reg[44] += (HSL_REG_MAIN->reg_huephase_ref[23] & 0xff) << 8;	


    /* h2d */
    BK89_reg[45] += HSL_REG_MAIN->reg_satphase_ref[0] & 0xff;
    BK89_reg[45] += (HSL_REG_MAIN->reg_satphase_ref[1] & 0xff) << 8;
    /* h2e */
    BK89_reg[46] += HSL_REG_MAIN->reg_satphase_ref[2] & 0xff;
    BK89_reg[46] += (HSL_REG_MAIN->reg_satphase_ref[3] & 0xff) << 8;	
    /* h2f */
    BK89_reg[47] += HSL_REG_MAIN->reg_satphase_ref[4] & 0xff;
    BK89_reg[47] += (HSL_REG_MAIN->reg_satphase_ref[5] & 0xff) << 8;
    /* h30 */
    BK89_reg[48] += HSL_REG_MAIN->reg_satphase_ref[6] & 0xff;
    BK89_reg[48] += (HSL_REG_MAIN->reg_satphase_ref[7] & 0xff) << 8;	
    /* h31 */
    BK89_reg[49] += HSL_REG_MAIN->reg_satphase_ref[8] & 0xff;
    BK89_reg[49] += (HSL_REG_MAIN->reg_satphase_ref[9] & 0xff) << 8;	
    /* h32 */
    BK89_reg[50] += HSL_REG_MAIN->reg_satphase_ref[10] & 0xff;
    BK89_reg[50] += (HSL_REG_MAIN->reg_satphase_ref[11] & 0xff) << 8;
    /* h33 */
    BK89_reg[51] += HSL_REG_MAIN->reg_satphase_ref[12] & 0xff;
    BK89_reg[51] += (HSL_REG_MAIN->reg_satphase_ref[13] & 0xff) << 8;
    /* h34 */
    BK89_reg[52] += HSL_REG_MAIN->reg_satphase_ref[14] & 0xff;
    BK89_reg[52] += (HSL_REG_MAIN->reg_satphase_ref[15] & 0xff) << 8;	
    /* h35 */
    BK89_reg[53] += HSL_REG_MAIN->reg_satphase_ref[16] & 0xff;
    BK89_reg[53] += (HSL_REG_MAIN->reg_satphase_ref[17] & 0xff) << 8;
    /* h36 */
    BK89_reg[54] += HSL_REG_MAIN->reg_satphase_ref[18] & 0xff;
    BK89_reg[54] += (HSL_REG_MAIN->reg_satphase_ref[19] & 0xff) << 8;
    /* h37 */
    BK89_reg[55] += HSL_REG_MAIN->reg_satphase_ref[20] & 0xff;
    BK89_reg[55] += (HSL_REG_MAIN->reg_satphase_ref[21] & 0xff) << 8;	
    /* h38 */
    BK89_reg[56] += HSL_REG_MAIN->reg_satphase_ref[22] & 0xff;
    BK89_reg[56] += (HSL_REG_MAIN->reg_satphase_ref[23] & 0xff) << 8;


    /* h39 */
    BK89_reg[57] += HSL_REG_MAIN->reg_lumaphase_ref[0] & 0xff;
    BK89_reg[57] += (HSL_REG_MAIN->reg_lumaphase_ref[1] & 0xff) << 8;
    /* h3a */
    BK89_reg[58] += HSL_REG_MAIN->reg_lumaphase_ref[2] & 0xff;
    BK89_reg[58] += (HSL_REG_MAIN->reg_lumaphase_ref[3] & 0xff) << 8;
    /* h3b */
    BK89_reg[59] += HSL_REG_MAIN->reg_lumaphase_ref[4] & 0xff;
    BK89_reg[59] += (HSL_REG_MAIN->reg_lumaphase_ref[5] & 0xff) << 8;
    /* h3c */
    BK89_reg[60] += HSL_REG_MAIN->reg_lumaphase_ref[6] & 0xff;
    BK89_reg[60] += (HSL_REG_MAIN->reg_lumaphase_ref[7] & 0xff) << 8;
    /* h3d */
    BK89_reg[61] += HSL_REG_MAIN->reg_lumaphase_ref[8] & 0xff;
    BK89_reg[61] += (HSL_REG_MAIN->reg_lumaphase_ref[9] & 0xff) << 8;
    /* h3e */
    BK89_reg[62] += HSL_REG_MAIN->reg_lumaphase_ref[10] & 0xff;
    BK89_reg[62] += (HSL_REG_MAIN->reg_lumaphase_ref[11] & 0xff) << 8;
    /* h3f */
    BK89_reg[63] += HSL_REG_MAIN->reg_lumaphase_ref[12] & 0xff;
    BK89_reg[63] += (HSL_REG_MAIN->reg_lumaphase_ref[13] & 0xff) << 8;
    /* h40 */
    BK89_reg[64] += HSL_REG_MAIN->reg_lumaphase_ref[14] & 0xff;
    BK89_reg[64] += (HSL_REG_MAIN->reg_lumaphase_ref[15] & 0xff) << 8;
    /* h41 */
    BK89_reg[65] += HSL_REG_MAIN->reg_lumaphase_ref[16] & 0xff;
    BK89_reg[65] += (HSL_REG_MAIN->reg_lumaphase_ref[17] & 0xff) << 8;
    /* h42 */
    BK89_reg[66] += HSL_REG_MAIN->reg_lumaphase_ref[18] & 0xff;
    BK89_reg[66] += (HSL_REG_MAIN->reg_lumaphase_ref[19] & 0xff) << 8;
    /* h43 */
    BK89_reg[67] += HSL_REG_MAIN->reg_lumaphase_ref[20] & 0xff;
    BK89_reg[67] += (HSL_REG_MAIN->reg_lumaphase_ref[21] & 0xff) << 8;
    /* h44 */
    BK89_reg[68] += HSL_REG_MAIN->reg_lumaphase_ref[22] & 0xff;
    BK89_reg[68] += (HSL_REG_MAIN->reg_lumaphase_ref[23] & 0xff) << 8;

    /* h45 */
    BK89_reg[69] += HSL_REG_MAIN->reg_compensate_start & 0x1fff;
    BK89_reg[69] += (HSL_REG_MAIN->reg_ui_protect_en & 0x1) << 14;
    BK89_reg[69] += (HSL_REG_MAIN->reg_compensate_fire & 0x1) << 15;

    /* h46 */
    BK89_reg[70] += HSL_REG_MAIN->reg_hsl_l_y_blend_alpha & 0x7f;
    BK89_reg[70] += (HSL_REG_MAIN->reg_hsl_sat_linear_blend & 0x1) << 8;
    BK89_reg[70] += (HSL_REG_MAIN->reg_hsl_luma_linear_blend & 0x1) << 9;
    BK89_reg[70] += (HSL_REG_MAIN->reg_hsl_invuvin_base_hout & 0x1) << 10;


/*=========================================
		Write registers into HW
========================================== */
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;
    int i;

    //while (u16ScalerMask)
    {
        //if (u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx % 2)  //Main
            {
                for (i = 1; i < 128; i++)
                {
                    // BK 87
                    msWrite2Byte(u32BaseAddr + SC69_00 + (2*i), (WORD)BK87_reg[i]);
                    // BK 88
                    msWrite2Byte(u32BaseAddr + SC6A_00 + (2*i), (WORD)BK88_reg[i]);
                    // BK 89
                    msWrite2Byte(u32BaseAddr + SC6B_00 + (2*i), (WORD)BK89_reg[i]);
                }	
            }
            //else//Sub
            {
                // TBD. 
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

/*-------------------------------------------------------------------------
	Function	:  msAPI_HSY_AdjustSkinTone
	Input		: *HSL_REG, SkinToneIdx
	Output		: N/A
	Feature		: write register setting into HW
*--------------------------------------------------------------------------*/
void msAPI_HSY_SkinToneTable(stHSL_REG *HSL_REG_MAIN, BYTE u8SkinToneIdx)
{

/*=========================================
		Bank: hsv_reg_main (BK_87)
		Content: COLOR ENGINE
========================================== */
    /* h02 */
    HSL_REG_MAIN->reg_hsl_hue_en = 1;
    HSL_REG_MAIN->reg_hsl_lig_en = 1;
    HSL_REG_MAIN->reg_hsl_lig_by_lig_en = 1;
    HSL_REG_MAIN->reg_hsl_sat_en = 1;
    HSL_REG_MAIN->reg_hsl_global_hue_by_hue_en = 1;
    HSL_REG_MAIN->reg_hsl_global_luma_by_hue_en = 1;
    HSL_REG_MAIN->reg_hsl_global_sat_en = 1;

    /* h03 */
    HSL_REG_MAIN->reg_hsl_hue_gp_en = 1;
    HSL_REG_MAIN->reg_hsl_sat_gp_en = 1;
    HSL_REG_MAIN->reg_hsl_lig_gp_en =  1;
    HSL_REG_MAIN->reg_en_cursor_display = 0; 
    HSL_REG_MAIN->reg_en_debug_pix = 0;
    HSL_REG_MAIN->reg_hsl_hue_dither_en = 0;
    HSL_REG_MAIN->reg_hsl_sat_dither_en = 0;
    HSL_REG_MAIN->reg_hsl_luma_dither_en = 0;	
    HSL_REG_MAIN->reg_hsl_sy2rgb_dither_en = 0;
    HSL_REG_MAIN->reg_sat_global_blending_en = 1;

    /* h04 */
    HSL_REG_MAIN ->reg_hsl_sat_post_max_limit = 4095;

    /* h05 */
    HSL_REG_MAIN->reg_hsl_uvnorm_scale = REG_HSL_UVNORM_SCALE;

    /* h06 */
    HSL_REG_MAIN->reg_hsl_rgb_min = -2047;

    /* h07 */
    HSL_REG_MAIN->reg_hsl_rgb_max = 2047;

    /* h08 */	
    HSL_REG_MAIN->reg_hsl_global_hue_delta_theta = 0;

    /* h0e */
    HSL_REG_MAIN->reg_hsl_hue_degree_initial = 0;

    /* h0f */
    HSL_REG_MAIN->reg_hsl_hue_delta_amp = 0;

    /* h10 */
    HSL_REG_MAIN->reg_r2y_12 = -429;

    /* h11 */	
    HSL_REG_MAIN->reg_r2y_13 = -83;

    /* h12 */
    HSL_REG_MAIN->reg_r2y_21 = 306;

    /* h13 */
    HSL_REG_MAIN->reg_r2y_22 = 601;

    /* h14 */
    HSL_REG_MAIN->reg_r2y_23 = 117;

    /* h15 */
    HSL_REG_MAIN->reg_r2y_31 = -173;

    /* h16 */
    HSL_REG_MAIN->reg_r2y_32 = -339;

    /* h17 */
    HSL_REG_MAIN->reg_hsl_hue_gray = 0;
    HSL_REG_MAIN->reg_hsl_sat_gray = 0;

    //* h1e */
    HSL_REG_MAIN->reg_hsl_lig_gp_thrd = 0;

    /* h1f */
    HSL_REG_MAIN->reg_hsl_lig_gp_min_strength = 63;
    HSL_REG_MAIN->reg_hsl_lig_gp_slope = 63;

	/* h24 */
	HSL_REG_MAIN->reg_cursor_x = 64;
	
	/* h25 */
	HSL_REG_MAIN->reg_cursor_y = 64;

	/* h26 */
	HSL_REG_MAIN->reg_cursor_data0 = 128;
	HSL_REG_MAIN->reg_cursor_data1 = 128;

	/* h27 */
	HSL_REG_MAIN->reg_cursor_data2 = 64;
	HSL_REG_MAIN->reg_cursor_len = 16;

	/* h30 */
	HSL_REG_MAIN->reg_de_sel = 1;
	
    /* h34 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[0] = 0;
    /* h35 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[1] = 0;
    /* h36 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[2] = 0;	
    /* h37 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[3] = 0;
    /* h38 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[4] = 0;
    /* h39 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[5] = 0;
    /* h3a */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[6] = 0;
    /* h3b */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[7] = 0;
    /* h3c */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[8] = 0;
    /* h3d */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[9] = 0;
    /* h3e */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[10] = 0;
    /* h3f */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[11] = 0;
    /* h40 */
    //HSL_REG_MAIN ->reg_driver_Monitor_Hue_Table[12] = 0;
    /* h41 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[13] = 0;
    /* h42 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[14] = 0;
    /* h43 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[15] = 0;
    /* h44 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[16] = 0;
    /* h45 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[17] = 0;
    /* h46 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[18] = 0;
    /* h47*/
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[19] = 0;
    /* h48 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[20] = 0;
    /* h49 */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[21] = 0;
    /* h4a */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[22] = 0;
    /* h4b */
    //HSL_REG_MAIN->reg_driver_Monitor_Hue_Table[23] = 0;

    /* h4c */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[0] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[1] = 0x80;

    /* h4d */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[2] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[3] = 0x80;

    /* h4e */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[4] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[5] = 0x80;

    /* h4f */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[6] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[7] = 0x80;

    /* h50 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[8] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[9] = 0x80;

    /* h51 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[10] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[11] = 0x80;

    /* h52 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[12] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[13] = 0x80;

    /* h53 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[14] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[15] = 0x80;

    /* h54 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[16] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[17] = 0x80;

    /* h55 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[18] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[19] = 0x80;

    /* h56 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[20] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[21] = 0x80;

    /* h57 */
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[22] = 0x80;
    //HSL_REG_MAIN->reg_driver_Monitor_Sat_Table[23] = 0x80;

    /* h58 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[0] = 0;

    /* h59 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[1] = 0;

    /* h5a */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[2] = 0;

    /* h5b */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[3] = 0;

    /* h5c */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[4] = 0;

    /* h5d */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[5] = 0;

    /* h5e */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[6] = 0;

    /* h5f */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[7] = 0;

    /* h60 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[8] = 0;

    /* h61 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[9] = 0;

    /* h62 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[10] = 0;

    /* h63 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[11] = 0;

    /* h64 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[12] = 0;

    /* h65 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[13] = 0;

    /* h66 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[14] = 0;

    /* h67 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[15] = 0;

    /* h68 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[16] = 0;

    /* h69 */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[17] = 0;

    /* h6a */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[18] = 0;

    /* h6b */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[19] = 0;

    /* h6c */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[20] = 0;

    /* h6d */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[21] = 0;

    /* h6e */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[22] = 0;

    /* h6f */
    //HSL_REG_MAIN->reg_driver_Monitor_Luma_Table[23] = 0;

    /* h7b */
    //HSL_REG_MAIN->reg_luma_gray_table[0] = 0;
    //HSL_REG_MAIN->reg_luma_gray_table[1] = 0;

    /* h7c */
    //HSL_REG_MAIN->reg_luma_gray_table[2] = 0;
    //HSL_REG_MAIN->reg_luma_gray_table[3] = 0;

    /* h7d */
    //HSL_REG_MAIN->reg_luma_gray_table[4] = 0;
    //HSL_REG_MAIN->reg_luma_gray_table[5] = 0;

    /* h7e */
    //HSL_REG_MAIN->reg_luma_gray_table[6] = 0;
    //HSL_REG_MAIN->reg_luma_gray_table[7] = 0;

    /* h7f */
    //HSL_REG_MAIN->reg_luma_gray_table[8] = 0;


/*=========================================
		Bank: hsv_reg_sub0 (BK_88)
		Content: FCC 
========================================== */

    /* h03 */
    HSL_REG_MAIN->reg_hsl_window_h_start = 55;
    HSL_REG_MAIN->reg_hsl_mwe_enable = 0;
    /* h04 */
    HSL_REG_MAIN->reg_hsl_window_h_end = 1072;
    /* h05 */
    HSL_REG_MAIN->reg_hsl_window_v_start = 7503;

    /* h06 */
    HSL_REG_MAIN->reg_hsl_window_v_end = 4699;

    /* h17 */	
    HSL_REG_MAIN->reg_hsl_path_bypass = 0;

    /* h18 */
    HSL_REG_MAIN->reg_hsl_uvangle_scale_0 = 742;
    /* h19 */
    HSL_REG_MAIN->reg_hsl_uvangle_offset_0 = 425;
    /* h1a */
    HSL_REG_MAIN->reg_hsl_uvangle_scale_1 = 758;
    /* h1b */
    HSL_REG_MAIN->reg_hsl_uvangle_offset_1 = 390;
    /* h1c */
    HSL_REG_MAIN->reg_hsl_uvangle_scale_2 = 808;
    /* h1d */
    HSL_REG_MAIN->reg_hsl_uvangle_offset_2 = 750;

    /* h20 */
    HSL_REG_MAIN->reg_y2r_11 = 0x59C;
    /* h21 */
    HSL_REG_MAIN->reg_y2r_21 = 0x1D25;
    /* h22 */
    HSL_REG_MAIN->reg_y2r_23 = 0x1EA0;
    /* h23 */
    HSL_REG_MAIN->reg_y2r_33 = 0x717;

/*---  FCC: start  ---*/
    /* h24 */
    HSL_REG_MAIN->reg_favor_region_huel_1 = 192;
    /* h25 */
    HSL_REG_MAIN->reg_favor_region_hueu_1 = 688;
    /* h26 */
    HSL_REG_MAIN->reg_favor_region_satl_1 = 0;
    /* h27 */
    HSL_REG_MAIN->reg_favor_region_satu_1 = 4095;
    /* h28 */
    HSL_REG_MAIN->reg_favor_region_lumal_1 = 0;
    /* h29 */
    HSL_REG_MAIN->reg_favor_region_lumau_1 = 4095;
    /* h2b */
    HSL_REG_MAIN->reg_favor_hue_gain_1 = 8;
    /* h2c */
    HSL_REG_MAIN->reg_favor_sat_gain_1 = 1;
    HSL_REG_MAIN->reg_favor_luma_gain_1 = 1;
    /* h2d */
    HSL_REG_MAIN->reg_dh_favor_adjust_1 = SkinToneTable[u8SkinToneIdx];
    /* h2e */
    HSL_REG_MAIN->reg_ds_favor_adjust_1 = 128;
    /* h2f */
    HSL_REG_MAIN->reg_dy_favor_adjust_1 = 0;

    /* h30 */
    HSL_REG_MAIN->reg_favor_region_huel_2 = 1200;
    /* h31 */
    HSL_REG_MAIN->reg_favor_region_hueu_2 = 2048;
    /* h32 */
    HSL_REG_MAIN->reg_favor_region_satl_2 = 0;
    /* h33 */
    HSL_REG_MAIN->reg_favor_region_satu_2 = 4095;
    /* h34 */
    HSL_REG_MAIN->reg_favor_region_lumal_2 = 0;
    /* h35 */
    HSL_REG_MAIN->reg_favor_region_lumau_2 = 4095;
    /* h37 */	
    HSL_REG_MAIN->reg_favor_hue_gain_2 = 7;
    /* h38 */
    HSL_REG_MAIN->reg_favor_sat_gain_2 = 2;
    HSL_REG_MAIN->reg_favor_luma_gain_2 = 2;
    /* h39 */
    HSL_REG_MAIN->reg_dh_favor_adjust_2 = 0;
    /* h3a */
    HSL_REG_MAIN->reg_ds_favor_adjust_2 = 255;
    /* h3b */
    HSL_REG_MAIN->reg_dy_favor_adjust_2 = 0;

    /* h3c */
    HSL_REG_MAIN->reg_favor_region_huel_3 = 4000;
    /* h3d */
    HSL_REG_MAIN->reg_favor_region_hueu_3 = 5120;
    /* h3e */
    HSL_REG_MAIN->reg_favor_region_satl_3 = 0;
    /* h3f */
    HSL_REG_MAIN->reg_favor_region_satu_3 = 4095;
    /* h40 */
    HSL_REG_MAIN->reg_favor_region_lumal_3 = 0;
    /* h41 */
    HSL_REG_MAIN->reg_favor_region_lumau_3 = 4095;
    /* h43 */
    HSL_REG_MAIN->reg_favor_hue_gain_3 = 5;
    /* h44 */
    HSL_REG_MAIN->reg_favor_sat_gain_3 = 2;
    HSL_REG_MAIN->reg_favor_luma_gain_3 = 2;
    /* h45 */
    HSL_REG_MAIN->reg_dh_favor_adjust_3 = 0;
    /* h46 */
    HSL_REG_MAIN->reg_ds_favor_adjust_3 = 255;
    /* h47 */
    HSL_REG_MAIN->reg_dy_favor_adjust_3 = 0;

    /* h48 */
    HSL_REG_MAIN->reg_favor_region_huel_4 = 3200;
    /* h49 */
    HSL_REG_MAIN->reg_favor_region_hueu_4 = 3800;
    /* h4a */
    HSL_REG_MAIN->reg_favor_region_satl_4 = 0;
    /* h4b */
    HSL_REG_MAIN->reg_favor_region_satu_4 = 4095;
    /* h4c */
    HSL_REG_MAIN->reg_favor_region_lumal_4 = 0;
    /* h4d */
    HSL_REG_MAIN->reg_favor_region_lumau_4 = 4095;
    /* h4f */
    HSL_REG_MAIN->reg_favor_hue_gain_4 = 10;

    /* h50 */
    HSL_REG_MAIN->reg_favor_sat_gain_4 = 2;	
    HSL_REG_MAIN->reg_favor_luma_gain_4 = 2;

    /* h51 */
    HSL_REG_MAIN->reg_dh_favor_adjust_4 = 0;
    /* h52 */
    HSL_REG_MAIN->reg_ds_favor_adjust_4 = 255;
    /* h53 */
    HSL_REG_MAIN->reg_dy_favor_adjust_4 = 0;

    /* h54 */
    HSL_REG_MAIN->reg_favor_region_huel_5 = 5800;
    /* h55 */
    HSL_REG_MAIN->reg_favor_region_hueu_5 = 5920;
    /* h56 */
    HSL_REG_MAIN->reg_favor_region_satl_5 = 0;
    /* h57 */
    HSL_REG_MAIN->reg_favor_region_satu_5 = 4095;	
    /* h58 */
    HSL_REG_MAIN->reg_favor_region_lumal_5 = 0;
    /* h59 */
    HSL_REG_MAIN->reg_favor_region_lumau_5 = 4095;
    /* h5b */
    HSL_REG_MAIN->reg_favor_hue_gain_5 = 51;
    /* h5c */
    HSL_REG_MAIN->reg_favor_sat_gain_5 = 2;
    HSL_REG_MAIN->reg_favor_luma_gain_5 = 2;
    /* h5d */
    HSL_REG_MAIN->reg_dh_favor_adjust_5 = 0;
    /* h5e */
    HSL_REG_MAIN->reg_ds_favor_adjust_5 = 255;
    /* h5f */
    HSL_REG_MAIN->reg_dy_favor_adjust_5 = 0;

    /* h60 */
    HSL_REG_MAIN->reg_favor_region = 1;
    HSL_REG_MAIN->reg_favor_region_1 = 1;
    HSL_REG_MAIN->reg_favor_region_2 = 0;
    HSL_REG_MAIN->reg_favor_region_3 = 0;
    HSL_REG_MAIN->reg_favor_region_4 = 0;
    HSL_REG_MAIN->reg_favor_region_5 = 0;
    HSL_REG_MAIN->reg_fcc_yin_sel = 0;

    /* h61 */
    HSL_REG_MAIN->reg_dh_favor_outside = 0;
    /* h62 */
    HSL_REG_MAIN->reg_ds_favor_outside = 128;
    /* h63 */
    HSL_REG_MAIN->reg_dy_favor_outside = 0;
    /* h63 */
    HSL_REG_MAIN->reg_hsl_fcc_outside_y_sel = 0;

	/* h68 */
	HSL_REG_MAIN->reg_pattern_b = 420;
	/* h69 */
	HSL_REG_MAIN->reg_pattern_g = 2956;
	/* h6a */
	HSL_REG_MAIN->reg_pattern_r = 884;
/*---  FCC: end  ---*/


/*=========================================
		Bank:  hsv_reg_sub1 (BK_89)
		Content: compensation circuit 
========================================== */
    /* h01 */
    HSL_REG_MAIN->reg_hsl_uvnorm_max = REG_HSL_UVNORM_MAX;
    //HSL_REG_MAIN->reg_driver_monitor_en = 0;

    /* h02*/
    HSL_REG_MAIN->reg_hue_ui_r = 0;
    /* h03 */
    HSL_REG_MAIN->reg_hue_ui_y = 0;
    /* h04 */
    HSL_REG_MAIN->reg_hue_ui_g = 0;
    /* h05 */
    HSL_REG_MAIN->reg_hue_ui_c = 0;
    /* h06 */
    HSL_REG_MAIN->reg_hue_ui_b = 0;
    /* h07 */
    HSL_REG_MAIN->reg_hue_ui_m = 0;			
    /* h08 */
    HSL_REG_MAIN->reg_sat_ui_r = 128;
    HSL_REG_MAIN->reg_sat_ui_y = 128;

    /* h09 */
    HSL_REG_MAIN->reg_sat_ui_g = 128;
    HSL_REG_MAIN->reg_sat_ui_c = 128;
    /* h0a */
    HSL_REG_MAIN->reg_sat_ui_b = 128;
    HSL_REG_MAIN->reg_sat_ui_m = 128;
    /* h0b */
    HSL_REG_MAIN->reg_luma_ui_r = 0;
    /* h0c */
    HSL_REG_MAIN->reg_luma_ui_y = 0;
    /* h0d */
    HSL_REG_MAIN->reg_luma_ui_g = 0;
    /* h0e  */
    HSL_REG_MAIN->reg_luma_ui_c = 0;
    /* h0f  */
    HSL_REG_MAIN->reg_luma_ui_b = 0;
    /* h10 */
    HSL_REG_MAIN->reg_luma_ui_m = 0;
    				
    /* h11 */
    HSL_REG_MAIN->reg_hue_alpha1 = 16;
    HSL_REG_MAIN->reg_hue_alpha2 = 32;
    /* h12 */
    HSL_REG_MAIN->reg_sat_alpha1 = 20;
    HSL_REG_MAIN->reg_sat_alpha2 = 20;
    /* h13 */
    HSL_REG_MAIN->reg_luma_alpha1 = 32;
    HSL_REG_MAIN->reg_luma_alpha2 = 0;


    /* h21 */
    HSL_REG_MAIN->reg_huephase_ref[0] = 0;
    HSL_REG_MAIN->reg_huephase_ref[1] = 59;
    /* h22 */
    HSL_REG_MAIN->reg_huephase_ref[2] = 130;
    HSL_REG_MAIN->reg_huephase_ref[3] = 202;
    /* h23 */
    HSL_REG_MAIN->reg_huephase_ref[4] = 9;
    HSL_REG_MAIN->reg_huephase_ref[5] = 72;
    /* h24 */
    HSL_REG_MAIN->reg_huephase_ref[6] = 143;
    HSL_REG_MAIN->reg_huephase_ref[7] = 211;
    /* h25 */
    HSL_REG_MAIN->reg_huephase_ref[8] = 13;
    HSL_REG_MAIN->reg_huephase_ref[9] = 69;
    /* h26 */
    HSL_REG_MAIN->reg_huephase_ref[10] = 134;
    HSL_REG_MAIN->reg_huephase_ref[11] = 199;
    /* h27 */
    HSL_REG_MAIN->reg_huephase_ref[12] = 0;
    HSL_REG_MAIN->reg_huephase_ref[13] = 59;
    /* h28 */
    HSL_REG_MAIN->reg_huephase_ref[14] = 129;
    HSL_REG_MAIN->reg_huephase_ref[15] = 202;
    /* h29 */
    HSL_REG_MAIN->reg_huephase_ref[16] = 9;
    HSL_REG_MAIN->reg_huephase_ref[17] = 72;
    /* h2a */
    HSL_REG_MAIN->reg_huephase_ref[18] = 143;
    HSL_REG_MAIN->reg_huephase_ref[19] = 212;	
    /* h2b */
    HSL_REG_MAIN->reg_huephase_ref[20] = 13;
    HSL_REG_MAIN->reg_huephase_ref[21] = 69;
    /* h2c */
    HSL_REG_MAIN->reg_huephase_ref[22] = 134;
    HSL_REG_MAIN->reg_huephase_ref[23] = 199;

    /* h2d */
    HSL_REG_MAIN->reg_satphase_ref[0] = 135;
    HSL_REG_MAIN->reg_satphase_ref[1] = 120;
    /* h2e */
    HSL_REG_MAIN->reg_satphase_ref[2] = 113;
    HSL_REG_MAIN->reg_satphase_ref[3] = 117;
    /* h2f */
    HSL_REG_MAIN->reg_satphase_ref[4] = 130;
    HSL_REG_MAIN->reg_satphase_ref[5] = 118;
    /* h30 */
    HSL_REG_MAIN->reg_satphase_ref[6] = 115;
    HSL_REG_MAIN->reg_satphase_ref[7] = 122;
    /* h31 */
    HSL_REG_MAIN->reg_satphase_ref[8] = 137;
    HSL_REG_MAIN->reg_satphase_ref[9] = 124;
    /* h32 */
    HSL_REG_MAIN->reg_satphase_ref[10] = 119;
    HSL_REG_MAIN->reg_satphase_ref[11] = 123;
    /* h33 */
    HSL_REG_MAIN->reg_satphase_ref[12] = 135;
    HSL_REG_MAIN->reg_satphase_ref[13] = 120;
    /* h34 */
    HSL_REG_MAIN->reg_satphase_ref[14] = 113;
    HSL_REG_MAIN->reg_satphase_ref[15] = 117;	
    /* h35 */
    HSL_REG_MAIN->reg_satphase_ref[16] = 130;
    HSL_REG_MAIN->reg_satphase_ref[17] = 118;
    /* h36 */
    HSL_REG_MAIN->reg_satphase_ref[18] = 115;
    HSL_REG_MAIN->reg_satphase_ref[19] = 122;
    /* h37 */
    HSL_REG_MAIN->reg_satphase_ref[20] = 137;
    HSL_REG_MAIN->reg_satphase_ref[21] = 124;
    /* h38 */
    HSL_REG_MAIN->reg_satphase_ref[22] = 119;
    HSL_REG_MAIN->reg_satphase_ref[23] = 123;


    /* h39 */
    HSL_REG_MAIN->reg_lumaphase_ref[0] = 76;
    HSL_REG_MAIN->reg_lumaphase_ref[1] = 114;
    /* h3a */
    HSL_REG_MAIN->reg_lumaphase_ref[2] = 152;
    HSL_REG_MAIN->reg_lumaphase_ref[3]  = 189;
    /* h3b */
    HSL_REG_MAIN->reg_lumaphase_ref[4] = 227;
    HSL_REG_MAIN->reg_lumaphase_ref[5] = 208;
    /* h3c */
    HSL_REG_MAIN->reg_lumaphase_ref[6] = 188;
    HSL_REG_MAIN->reg_lumaphase_ref[7] = 169;
    /* h3d */
    HSL_REG_MAIN->reg_lumaphase_ref[8] = 150; 
    HSL_REG_MAIN->reg_lumaphase_ref[9] = 157;
    /* h3e */
    HSL_REG_MAIN->reg_lumaphase_ref[10] = 165;
    HSL_REG_MAIN->reg_lumaphase_ref[11] = 172;
    /* h3f */
    HSL_REG_MAIN->reg_lumaphase_ref[12] = 179;
    HSL_REG_MAIN->reg_lumaphase_ref[13] = 141;
    /* h40 */
    HSL_REG_MAIN->reg_lumaphase_ref[14] = 103;
    HSL_REG_MAIN->reg_lumaphase_ref[15] = 66;
    /* h41 */
    HSL_REG_MAIN->reg_lumaphase_ref[16] = 28; 
    HSL_REG_MAIN->reg_lumaphase_ref[17] = 47;
    /* h42 */
    HSL_REG_MAIN->reg_lumaphase_ref[18] = 67;
    HSL_REG_MAIN->reg_lumaphase_ref[19] = 86;
    /* h43 */
    HSL_REG_MAIN->reg_lumaphase_ref[20] = 105;
    HSL_REG_MAIN->reg_lumaphase_ref[21] = 98;
    /* h44 */
    HSL_REG_MAIN->reg_lumaphase_ref[22] = 90;
    HSL_REG_MAIN->reg_lumaphase_ref[23] = 83;

    /* h45 */
    HSL_REG_MAIN->reg_compensate_start = 0;
    HSL_REG_MAIN->reg_ui_protect_en = 0;
    HSL_REG_MAIN->reg_compensate_fire = 0;

    /* h46 */
    HSL_REG_MAIN->reg_hsl_l_y_blend_alpha = 29;
    HSL_REG_MAIN->reg_hsl_sat_linear_blend = 0;
    HSL_REG_MAIN->reg_hsl_luma_linear_blend  = 0;
    HSL_REG_MAIN->reg_hsl_invuvin_base_hout = 1;
}

/*-------------------------------------------------------------------------
	Function	:  msAPI_LOAD_HSY_SKINTONE_TABLE
	Input	: *HSL_REG
	Output	: N/A
	Feature	: write register setting into HW
*--------------------------------------------------------------------------*/
void msAPI_LOAD_HSY_SKINTONE_TABLE(BYTE u8WinIdx, stHSL_REG *HSL_REG_MAIN)
{
    UNUSED(u8WinIdx);
/*=========================================
		Bank: hsv_reg_main (BK_87)
		Content: COLOR ENGINE
========================================== */
    int BK87_reg[128] = {0}; 

    /* h02 */
    BK87_reg[2] += HSL_REG_MAIN->reg_hsl_hue_en & 0x1;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_lig_en & 0x1) << 4;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_lig_by_lig_en & 0x1) << 5;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_sat_en & 0x1) << 7;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_global_hue_by_hue_en & 0x1) << 11;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_global_luma_by_hue_en & 0x1) << 14;
    BK87_reg[2] += (HSL_REG_MAIN->reg_hsl_global_sat_en & 0x1) << 15;

    /* h03 */
    BK87_reg[3] += HSL_REG_MAIN->reg_hsl_hue_gp_en & 0x1;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_sat_gp_en & 0x1) << 1;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_lig_gp_en & 0x1) << 2;
    BK87_reg[3] += (HSL_REG_MAIN->reg_en_cursor_display & 0x1) << 6;
    BK87_reg[3] += (HSL_REG_MAIN->reg_en_debug_pix & 0x1) << 7;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_hue_dither_en & 0x1) << 8;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_sat_dither_en & 0x1) << 9;
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_luma_dither_en & 0x1) << 10;	
    BK87_reg[3] += (HSL_REG_MAIN->reg_hsl_sy2rgb_dither_en & 0x1) << 11;
    BK87_reg[3] += (HSL_REG_MAIN->reg_sat_global_blending_en & 0x1) << 15;

    /* h04 */
    BK87_reg[4] += HSL_REG_MAIN->reg_hsl_sat_post_max_limit & 0xfff;
    /* h05 */
    BK87_reg[5] += HSL_REG_MAIN->reg_hsl_uvnorm_scale & 0x7ff;
    /* h06 */
    BK87_reg[6] += HSL_REG_MAIN->reg_hsl_rgb_min & 0xfff;
    /* h07 */
    BK87_reg[7] += HSL_REG_MAIN->reg_hsl_rgb_max & 0xfff;
    /* h08 */
    BK87_reg[8] += HSL_REG_MAIN->reg_hsl_global_hue_delta_theta & 0x1ff;	
    /* h0e */
    BK87_reg[14] += HSL_REG_MAIN->reg_hsl_hue_degree_initial & 0x3ff;
    /* h0f */
    BK87_reg[15] += (HSL_REG_MAIN->reg_hsl_hue_delta_amp & 0x3) << 4;
    /* h10 */
    BK87_reg[16] += HSL_REG_MAIN->reg_r2y_12 & 0x7ff;
    /* h11 */	
    BK87_reg[17] += HSL_REG_MAIN->reg_r2y_13 & 0x7ff;
    /* h12 */
    BK87_reg[18] += HSL_REG_MAIN->reg_r2y_21 & 0x7ff;
    /* h13 */
    BK87_reg[19] += HSL_REG_MAIN->reg_r2y_22 & 0x7ff;
    /* h14 */
    BK87_reg[20] += HSL_REG_MAIN->reg_r2y_23 & 0x7ff;
    /* h15 */
    BK87_reg[21] += HSL_REG_MAIN->reg_r2y_31 & 0x7ff;
    /* h16 */
    BK87_reg[22] += HSL_REG_MAIN->reg_r2y_32 & 0x7ff;
    /* h17 */
    BK87_reg[23] += HSL_REG_MAIN->reg_hsl_hue_gray & 0xff;
    BK87_reg[23] += (HSL_REG_MAIN->reg_hsl_sat_gray & 0xff) << 8;
    //* h1e */
    BK87_reg[30] += HSL_REG_MAIN->reg_hsl_lig_gp_thrd & 0x3ff;
    /* h1f */
    BK87_reg[31] += HSL_REG_MAIN->reg_hsl_lig_gp_min_strength & 0x3f;
    BK87_reg[31] += (HSL_REG_MAIN->reg_hsl_lig_gp_slope & 0x3f) << 8;

	/* h24 */
	BK87_reg[36] += HSL_REG_MAIN->reg_cursor_x & 0x1FFF;
	/* h25 */
	BK87_reg[37] += HSL_REG_MAIN->reg_cursor_y & 0x1FFF;
	/* h26 */
	BK87_reg[38] += HSL_REG_MAIN->reg_cursor_data0 & 0xFF;
	BK87_reg[38] += (HSL_REG_MAIN->reg_cursor_data1 & 0xFF) << 8;
	/* h27 */
	BK87_reg[39] += HSL_REG_MAIN->reg_cursor_data2 & 0xFF;
	BK87_reg[39] += (HSL_REG_MAIN->reg_cursor_len  & 0xFF) << 8;
	/* h30 */
	BK87_reg[48] += (HSL_REG_MAIN->reg_de_sel & 0x01) << 14;
/*=========================================
		Bank: hsv_reg_sub0 (BK_88)
		Content: FCC 
========================================== */
    int BK88_reg[128] = { 0 };
	/* h03 */
	BK88_reg[3] += HSL_REG_MAIN->reg_hsl_window_h_start & 0x1fff;
	BK88_reg[3] += (HSL_REG_MAIN->reg_hsl_mwe_enable & 01) << 15;
	/* h04 */
	BK88_reg[4] += HSL_REG_MAIN->reg_hsl_window_h_end & 0x1fff;
	/* h05 */
	BK88_reg[5] += HSL_REG_MAIN->reg_hsl_window_v_start & 0x1fff;
	/* h06 */
	BK88_reg[6] += HSL_REG_MAIN->reg_hsl_window_v_end & 0x1fff;
	/* h17 */	
	BK88_reg[23] +=HSL_REG_MAIN->reg_hsl_path_bypass & 0x1;
	/* h18 */
	BK88_reg[24] += HSL_REG_MAIN->reg_hsl_uvangle_scale_0 & 0x3ff;
	/* h19 */
	BK88_reg[25] += HSL_REG_MAIN->reg_hsl_uvangle_offset_0 & 0x3ff;
	/* h1a */
	BK88_reg[26] += HSL_REG_MAIN->reg_hsl_uvangle_scale_1 & 0x3ff;
	/* h1b */
	BK88_reg[27] += HSL_REG_MAIN->reg_hsl_uvangle_offset_1 & 0x3ff;
	/* h1c */
	BK88_reg[28] += HSL_REG_MAIN->reg_hsl_uvangle_scale_2 & 0x3ff;
	/* h1d */
	BK88_reg[29] += HSL_REG_MAIN->reg_hsl_uvangle_offset_2 & 0x3ff;

	/* h20 */
	BK88_reg[32] += HSL_REG_MAIN->reg_y2r_11 & 0x1fff;
	/* h21 */
	BK88_reg[33] += HSL_REG_MAIN->reg_y2r_21 & 0x1fff;
	/* h22 */
	BK88_reg[34] += HSL_REG_MAIN->reg_y2r_23 & 0x1fff;
	/* h23 */
	BK88_reg[35] += HSL_REG_MAIN->reg_y2r_33 & 0x1fff;

/*---  FCC: start  ---*/
	/* h24 */
	BK88_reg[36] += HSL_REG_MAIN->reg_favor_region_huel_1 & 0x1fff;
	/* h25 */
	BK88_reg[37] += HSL_REG_MAIN->reg_favor_region_hueu_1 & 0x1fff;
	/* h26 */
	BK88_reg[38] += HSL_REG_MAIN->reg_favor_region_satl_1 & 0xfff;
	/* h27 */
	BK88_reg[39] += HSL_REG_MAIN->reg_favor_region_satu_1 & 0xfff;
	/* h28 */
	BK88_reg[40] += HSL_REG_MAIN->reg_favor_region_lumal_1 & 0xfff;
	/* h29 */
	BK88_reg[41] += HSL_REG_MAIN->reg_favor_region_lumau_1 & 0xfff;
	/* h2b */
	BK88_reg[43] += (HSL_REG_MAIN->reg_favor_hue_gain_1 & 0xff) << 8;
	/* h2c */
	BK88_reg[44] += HSL_REG_MAIN->reg_favor_sat_gain_1 & 0xff;
	BK88_reg[44] += (HSL_REG_MAIN->reg_favor_luma_gain_1 & 0xff) << 8;
	/* h2d */
	BK88_reg[45] += HSL_REG_MAIN->reg_dh_favor_adjust_1 & 0x7ff;
	/* h2e */
	BK88_reg[46] += HSL_REG_MAIN->reg_ds_favor_adjust_1 & 0xff;
	/* h2f */
	BK88_reg[47] += HSL_REG_MAIN->reg_dy_favor_adjust_1 & 0x7ff;

	/* h30 */
	BK88_reg[48] += HSL_REG_MAIN->reg_favor_region_huel_2 & 0x1fff;
	/* h31 */
	BK88_reg[49] += HSL_REG_MAIN->reg_favor_region_hueu_2 & 0x1fff;
	/* h32 */
	BK88_reg[50] += HSL_REG_MAIN->reg_favor_region_satl_2 & 0xfff;
	/* h33 */
	BK88_reg[51] += HSL_REG_MAIN->reg_favor_region_satu_2 & 0xfff;
	/* h34 */
	BK88_reg[52] += HSL_REG_MAIN->reg_favor_region_lumal_2 & 0xfff;
	/* h35 */
	BK88_reg[53] += HSL_REG_MAIN->reg_favor_region_lumau_2 & 0xfff;
	/* h37 */	
	BK88_reg[55] += (HSL_REG_MAIN->reg_favor_hue_gain_2 & 0xff) << 8;
	/* h38 */
	BK88_reg[56] += HSL_REG_MAIN->reg_favor_sat_gain_2 & 0xff;
	BK88_reg[56] += (HSL_REG_MAIN->reg_favor_luma_gain_2 & 0xff) << 8;
	/* h39 */
	BK88_reg[57] += HSL_REG_MAIN->reg_dh_favor_adjust_2 & 0x7ff;
	/* h3a */
	BK88_reg[58] += HSL_REG_MAIN->reg_ds_favor_adjust_2 & 0xff;
	/* h3b */
	BK88_reg[59] += HSL_REG_MAIN->reg_dy_favor_adjust_2 & 0x7ff;

	/* h3c */
	BK88_reg[60] += HSL_REG_MAIN->reg_favor_region_huel_3 & 0x1fff;
	/* h3d */
	BK88_reg[61] += HSL_REG_MAIN->reg_favor_region_hueu_3 & 0x1fff;
	/* h3e */
	BK88_reg[62] += HSL_REG_MAIN->reg_favor_region_satl_3 & 0xfff;
	/* h3f */
	BK88_reg[63] += HSL_REG_MAIN->reg_favor_region_satu_3 & 0xfff;
	/* h40 */
	BK88_reg[64] += HSL_REG_MAIN->reg_favor_region_lumal_3 & 0xfff;
	/* h41 */
	BK88_reg[65] += HSL_REG_MAIN->reg_favor_region_lumau_3 & 0xfff;
	/* h43 */
	BK88_reg[67] += (HSL_REG_MAIN->reg_favor_hue_gain_3 & 0xff) << 8;
	/* h44 */
	BK88_reg[68] += HSL_REG_MAIN->reg_favor_sat_gain_3 & 0xff;
	BK88_reg[68] += (HSL_REG_MAIN->reg_favor_luma_gain_3 & 0xff) << 8;
	/* h45 */
	BK88_reg[69] += HSL_REG_MAIN->reg_dh_favor_adjust_3 & 0x7ff;
	/* h46 */
	BK88_reg[70] += HSL_REG_MAIN->reg_ds_favor_adjust_3 & 0xff;
	/* h47 */
	BK88_reg[71] += HSL_REG_MAIN->reg_dy_favor_adjust_3 & 0x7ff;

	/* h48 */
	BK88_reg[72] += HSL_REG_MAIN->reg_favor_region_huel_4 & 0x1fff;
	/* h49 */
	BK88_reg[73] += HSL_REG_MAIN->reg_favor_region_hueu_4 & 0x1fff;
	/* h4a */
	BK88_reg[74] += HSL_REG_MAIN->reg_favor_region_satl_4 & 0xfff;
	/* h4b */
	BK88_reg[75] += HSL_REG_MAIN->reg_favor_region_satu_4 & 0xfff;
	/* h4c */
	BK88_reg[76] += HSL_REG_MAIN->reg_favor_region_lumal_4 & 0xfff;
	/* h4d */
	BK88_reg[77] += HSL_REG_MAIN->reg_favor_region_lumau_4 & 0xfff;
	/* h4f */
	BK88_reg[79] += (HSL_REG_MAIN->reg_favor_hue_gain_4 & 0xff) << 8;	
	/* h50 */
	BK88_reg[80] += HSL_REG_MAIN->reg_favor_sat_gain_4 & 0xff;	
	BK88_reg[80] += (HSL_REG_MAIN->reg_favor_luma_gain_4 & 0xff) << 8;
	/* h51 */
	BK88_reg[81] += HSL_REG_MAIN->reg_dh_favor_adjust_4 & 0x7ff;
	/* h52 */
	BK88_reg[82] += HSL_REG_MAIN->reg_ds_favor_adjust_4 & 0xff;
	/* h53 */
	BK88_reg[83] += HSL_REG_MAIN->reg_dy_favor_adjust_4 & 0x7ff;

	/* h54 */
	BK88_reg[84] += HSL_REG_MAIN->reg_favor_region_huel_5 & 0x1fff;
	/* h55 */
	BK88_reg[85] += HSL_REG_MAIN->reg_favor_region_hueu_5 & 0x1fff;
	/* h56 */
	BK88_reg[86] += HSL_REG_MAIN->reg_favor_region_satl_5 & 0xfff;
	/* h57 */
	BK88_reg[87] += HSL_REG_MAIN->reg_favor_region_satu_5 & 0xfff;	
	/* h58 */
	BK88_reg[88] += HSL_REG_MAIN->reg_favor_region_lumal_5 & 0xfff;
	/* h59 */
	BK88_reg[89] += HSL_REG_MAIN->reg_favor_region_lumau_5 & 0xfff;
	/* h5b */
	BK88_reg[91] += (HSL_REG_MAIN->reg_favor_hue_gain_5 & 0xff) << 8;
	/* h5c */
	BK88_reg[92] += HSL_REG_MAIN->reg_favor_sat_gain_5 & 0xff;
	BK88_reg[92] += (HSL_REG_MAIN->reg_favor_luma_gain_5 & 0xff) << 8;
	/* h5d */
	BK88_reg[93] += HSL_REG_MAIN->reg_dh_favor_adjust_5 & 0x7ff;
	/* h5e */
	BK88_reg[94] += HSL_REG_MAIN->reg_ds_favor_adjust_5 & 0xff;
	/* h5f */
	BK88_reg[95] += HSL_REG_MAIN->reg_dy_favor_adjust_5 & 0x7ff;

	/* h60 */
	BK88_reg[96] += HSL_REG_MAIN->reg_favor_region & 0x1;
	BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_1 & 0x1) << 1;
	BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_2 & 0x1) << 2;
	BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_3 & 0x1) << 3;
	BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_4 & 0x1) << 4;
	BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_5 & 0x1) << 5;
	BK88_reg[96] += (HSL_REG_MAIN->reg_fcc_yin_sel & 0x1) << 7;

	/* h61 */
	BK88_reg[97] += HSL_REG_MAIN->reg_dh_favor_outside & 0x7ff;
	/* h62 */
	BK88_reg[98] += HSL_REG_MAIN->reg_ds_favor_outside & 0xff;
	/* h63 */
	BK88_reg[99] += HSL_REG_MAIN->reg_dy_favor_outside & 0x7ff;
	/* h63 */
	BK88_reg[99] += (HSL_REG_MAIN->reg_hsl_fcc_outside_y_sel & 0x1) << 15;

	/* h68 */
	BK88_reg[104] += HSL_REG_MAIN->reg_pattern_b & 0xFFF;
	/* h69 */
	BK88_reg[105] += HSL_REG_MAIN->reg_pattern_g & 0xFFF;
	/* h6a */
	BK88_reg[106] += HSL_REG_MAIN->reg_pattern_r & 0xFFF;
/*---  FCC: end  ---*/


/*=========================================
		Bank:  hsv_reg_sub1 (BK_89)
		Content: compensation circuit 
========================================== */
    int BK89_reg[128] = { 0 };

    /* h01 */
    //BK89_reg[1] += HSL_REG_MAIN->reg_hsl_uvnorm_max & 0x7ff;
    //BK89_reg[1] += (HSL_REG_MAIN->reg_driver_monitor_en & 0x1) << 12;

    /* h02*/
    //BK89_reg[2] += HSL_REG_MAIN->reg_hue_ui_r & 0x7ff;
    /* h03 */
    //BK89_reg[3] += HSL_REG_MAIN->reg_hue_ui_y & 0x7ff;
    /* h04 */
    //BK89_reg[4] += HSL_REG_MAIN->reg_hue_ui_g & 0x7ff;
    /* h05 */
    //BK89_reg[5] += HSL_REG_MAIN->reg_hue_ui_c & 0x7ff;
    /* h06 */
    //BK89_reg[6] += HSL_REG_MAIN->reg_hue_ui_b & 0x7ff;
    /* h07 */
    //BK89_reg[7] += HSL_REG_MAIN->reg_hue_ui_m & 0x7ff;			
    /* h08 */
    //BK89_reg[8] += HSL_REG_MAIN->reg_sat_ui_r & 0xff;
    //BK89_reg[8] += (HSL_REG_MAIN->reg_sat_ui_y & 0xff) << 8;
    /* h09 */
    //BK89_reg[9] += HSL_REG_MAIN->reg_sat_ui_g & 0xff;
    //BK89_reg[9] += (HSL_REG_MAIN->reg_sat_ui_c & 0xff) << 8;
    /* h0a */
    //BK89_reg[10] += HSL_REG_MAIN->reg_sat_ui_b & 0xff;
    //BK89_reg[10] += (HSL_REG_MAIN->reg_sat_ui_m & 0xff) << 8;
    /* h0b */
    //BK89_reg[11] += HSL_REG_MAIN->reg_luma_ui_r & 0x7ff;
    /* h0c */
    //BK89_reg[12] += HSL_REG_MAIN->reg_luma_ui_y & 0x7ff;
    /* h0d */
    //BK89_reg[13] += HSL_REG_MAIN->reg_luma_ui_g & 0x7ff;
    /* h0e  */
    //BK89_reg[14] += HSL_REG_MAIN->reg_luma_ui_c & 0x7ff;
    /* h0f  */
    //BK89_reg[15] += HSL_REG_MAIN->reg_luma_ui_b & 0x7ff;
    /* h10 */
    //BK89_reg[16] += HSL_REG_MAIN->reg_luma_ui_m & 0x7ff;
    				
    /* h11 */
    //BK89_reg[17] += HSL_REG_MAIN->reg_hue_alpha1 & 0x7f;
    //BK89_reg[17] += (HSL_REG_MAIN->reg_hue_alpha2 & 0x7f) << 8;
    /* h12 */
    BK89_reg[18] += HSL_REG_MAIN->reg_sat_alpha1 & 0x7f;
    BK89_reg[18] += (HSL_REG_MAIN->reg_sat_alpha2 & 0x7f) << 8;
    /* h13 */
    //BK89_reg[19] += HSL_REG_MAIN->reg_luma_alpha1 & 0x7f;
    //BK89_reg[19] += (HSL_REG_MAIN->reg_luma_alpha2 & 0x7f) << 8;


    /* h21 */
    //BK89_reg[33] += HSL_REG_MAIN->reg_huephase_ref[0] & 0xff;
    //BK89_reg[33] += (HSL_REG_MAIN->reg_huephase_ref[1] & 0xff) << 8;
    /* h22 */
    //BK89_reg[34] += HSL_REG_MAIN->reg_huephase_ref[2] & 0xff;
    //BK89_reg[34] += (HSL_REG_MAIN->reg_huephase_ref[3] & 0xff) << 8;
    /* h23 */
    //BK89_reg[35] += HSL_REG_MAIN->reg_huephase_ref[4] & 0xff;
    //BK89_reg[35] += (HSL_REG_MAIN->reg_huephase_ref[5] & 0xff) << 8;
    /* h24 */
    //BK89_reg[36] += HSL_REG_MAIN->reg_huephase_ref[6] & 0xff;
    //BK89_reg[36] += (HSL_REG_MAIN->reg_huephase_ref[7] & 0xff) << 8;
    /* h25 */
    //BK89_reg[37] += HSL_REG_MAIN->reg_huephase_ref[8] & 0xff;
    //BK89_reg[37] += (HSL_REG_MAIN->reg_huephase_ref[9] & 0xff) << 8;
    /* h26 */
    //BK89_reg[38] += HSL_REG_MAIN->reg_huephase_ref[10] & 0xff;
    //BK89_reg[38] += (HSL_REG_MAIN->reg_huephase_ref[11] & 0xff) << 8;	
    /* h27 */
    //BK89_reg[39] += HSL_REG_MAIN->reg_huephase_ref[12] & 0xff;
    //BK89_reg[39] += (HSL_REG_MAIN->reg_huephase_ref[13] & 0xff) << 8;	
    /* h28 */
    //BK89_reg[40] += HSL_REG_MAIN->reg_huephase_ref[14] & 0xff;
    //BK89_reg[40] += (HSL_REG_MAIN->reg_huephase_ref[15] & 0xff) << 8;	
    /* h29 */
    //BK89_reg[41] += HSL_REG_MAIN->reg_huephase_ref[16] & 0xff;
    //BK89_reg[41] += (HSL_REG_MAIN->reg_huephase_ref[17] & 0xff) << 8;	
    /* h2a */
    //BK89_reg[42] += HSL_REG_MAIN->reg_huephase_ref[18] & 0xff;
    //BK89_reg[42] += (HSL_REG_MAIN->reg_huephase_ref[19] & 0xff) << 8;	
    /* h2b */
    //BK89_reg[43] += HSL_REG_MAIN->reg_huephase_ref[20] & 0xff;
    //BK89_reg[43] += (HSL_REG_MAIN->reg_huephase_ref[21] & 0xff) << 8;	
    /* h2c */
    //BK89_reg[44] += HSL_REG_MAIN->reg_huephase_ref[22] & 0xff;
    //BK89_reg[44] += (HSL_REG_MAIN->reg_huephase_ref[23] & 0xff) << 8;	


    /* h2d */
    //BK89_reg[45] += HSL_REG_MAIN->reg_satphase_ref[0] & 0xff;
    //BK89_reg[45] += (HSL_REG_MAIN->reg_satphase_ref[1] & 0xff) << 8;
    /* h2e */
    //BK89_reg[46] += HSL_REG_MAIN->reg_satphase_ref[2] & 0xff;
    //BK89_reg[46] += (HSL_REG_MAIN->reg_satphase_ref[3] & 0xff) << 8;	
    /* h2f */
    //BK89_reg[47] += HSL_REG_MAIN->reg_satphase_ref[4] & 0xff;
    //BK89_reg[47] += (HSL_REG_MAIN->reg_satphase_ref[5] & 0xff) << 8;
    /* h30 */
    //BK89_reg[48] += HSL_REG_MAIN->reg_satphase_ref[6] & 0xff;
    //BK89_reg[48] += (HSL_REG_MAIN->reg_satphase_ref[7] & 0xff) << 8;	
    /* h31 */
    //BK89_reg[49] += HSL_REG_MAIN->reg_satphase_ref[8] & 0xff;
    //BK89_reg[49] += (HSL_REG_MAIN->reg_satphase_ref[9] & 0xff) << 8;	
    /* h32 */
    //BK89_reg[50] += HSL_REG_MAIN->reg_satphase_ref[10] & 0xff;
    //BK89_reg[50] += (HSL_REG_MAIN->reg_satphase_ref[11] & 0xff) << 8;
    /* h33 */
    //BK89_reg[51] += HSL_REG_MAIN->reg_satphase_ref[12] & 0xff;
    //BK89_reg[51] += (HSL_REG_MAIN->reg_satphase_ref[13] & 0xff) << 8;
    /* h34 */
    //BK89_reg[52] += HSL_REG_MAIN->reg_satphase_ref[14] & 0xff;
    //BK89_reg[52] += (HSL_REG_MAIN->reg_satphase_ref[15] & 0xff) << 8;	
    /* h35 */
    //BK89_reg[53] += HSL_REG_MAIN->reg_satphase_ref[16] & 0xff;
    //BK89_reg[53] += (HSL_REG_MAIN->reg_satphase_ref[17] & 0xff) << 8;
    /* h36 */
    //BK89_reg[54] += HSL_REG_MAIN->reg_satphase_ref[18] & 0xff;
    //BK89_reg[54] += (HSL_REG_MAIN->reg_satphase_ref[19] & 0xff) << 8;
    /* h37 */
    //BK89_reg[55] += HSL_REG_MAIN->reg_satphase_ref[20] & 0xff;
    //BK89_reg[55] += (HSL_REG_MAIN->reg_satphase_ref[21] & 0xff) << 8;	
    /* h38 */
    //BK89_reg[56] += HSL_REG_MAIN->reg_satphase_ref[22] & 0xff;
    //BK89_reg[56] += (HSL_REG_MAIN->reg_satphase_ref[23] & 0xff) << 8;


    /* h39 */
    //BK89_reg[57] += HSL_REG_MAIN->reg_lumaphase_ref[0] & 0xff;
    //BK89_reg[57] += (HSL_REG_MAIN->reg_lumaphase_ref[1] & 0xff) << 8;
    /* h3a */
    //BK89_reg[58] += HSL_REG_MAIN->reg_lumaphase_ref[2] & 0xff;
    //BK89_reg[58] += (HSL_REG_MAIN->reg_lumaphase_ref[3] & 0xff) << 8;
    /* h3b */
    //BK89_reg[59] += HSL_REG_MAIN->reg_lumaphase_ref[4] & 0xff;
    //BK89_reg[59] += (HSL_REG_MAIN->reg_lumaphase_ref[5] & 0xff) << 8;
    /* h3c */
    //BK89_reg[60] += HSL_REG_MAIN->reg_lumaphase_ref[6] & 0xff;
    //BK89_reg[60] += (HSL_REG_MAIN->reg_lumaphase_ref[7] & 0xff) << 8;
    /* h3d */
    //BK89_reg[61] += HSL_REG_MAIN->reg_lumaphase_ref[8] & 0xff;
    //BK89_reg[61] += (HSL_REG_MAIN->reg_lumaphase_ref[9] & 0xff) << 8;
    /* h3e */
    //BK89_reg[62] += HSL_REG_MAIN->reg_lumaphase_ref[10] & 0xff;
    //BK89_reg[62] += (HSL_REG_MAIN->reg_lumaphase_ref[11] & 0xff) << 8;
    /* h3f */
    //BK89_reg[63] += HSL_REG_MAIN->reg_lumaphase_ref[12] & 0xff;
    //BK89_reg[63] += (HSL_REG_MAIN->reg_lumaphase_ref[13] & 0xff) << 8;
    /* h40 */
    //BK89_reg[64] += HSL_REG_MAIN->reg_lumaphase_ref[14] & 0xff;
    //BK89_reg[64] += (HSL_REG_MAIN->reg_lumaphase_ref[15] & 0xff) << 8;
    /* h41 */
    //BK89_reg[65] += HSL_REG_MAIN->reg_lumaphase_ref[16] & 0xff;
    //BK89_reg[65] += (HSL_REG_MAIN->reg_lumaphase_ref[17] & 0xff) << 8;
    /* h42 */
    //BK89_reg[66] += HSL_REG_MAIN->reg_lumaphase_ref[18] & 0xff;
    //BK89_reg[66] += (HSL_REG_MAIN->reg_lumaphase_ref[19] & 0xff) << 8;
    /* h43 */
    //BK89_reg[67] += HSL_REG_MAIN->reg_lumaphase_ref[20] & 0xff;
    //BK89_reg[67] += (HSL_REG_MAIN->reg_lumaphase_ref[21] & 0xff) << 8;
    /* h44 */
    //BK89_reg[68] += HSL_REG_MAIN->reg_lumaphase_ref[22] & 0xff;
    //BK89_reg[68] += (HSL_REG_MAIN->reg_lumaphase_ref[23] & 0xff) << 8;

    /* h45 */
    BK89_reg[69] += HSL_REG_MAIN->reg_compensate_start & 0x1fff;
    BK89_reg[69] += (HSL_REG_MAIN->reg_ui_protect_en & 0x1) << 14;
    //BK89_reg[69] += (HSL_REG_MAIN->reg_compensate_fire & 0x1) << 15;

    /* h46 */
    BK89_reg[70] += HSL_REG_MAIN->reg_hsl_l_y_blend_alpha & 0x7f;
    //BK89_reg[70] += (HSL_REG_MAIN->reg_hsl_sat_linear_blend & 0x1) << 8;
    //BK89_reg[70] += (HSL_REG_MAIN->reg_hsl_luma_linear_blend & 0x1) << 9;
    //BK89_reg[70] += (HSL_REG_MAIN->reg_hsl_invuvin_base_hout & 0x1) << 10;

/*=========================================
		Write registers into HW
========================================== */
    //BYTE u8ScalerIdx = 0;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;
    int i;

    //while (u16ScalerMask)
    {
        //if (u16ScalerMask & BIT0)
        {
            //u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            //if(u8ScalerIdx % 2)  //Main
            {
            	for(i = 2; i < 48; i++)
            	{
					// BK 87
                    msWrite2Byte(u32BaseAddr + SC69_00 + (2*i), (WORD)BK87_reg[i]);
            	}
				/*
				// BK 88
				//BK88_reg[39] += HSL_REG_MAIN->reg_favor_region_satu_1 & 0xfff;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+78, (WORD)BK89_reg[39], 0xfff);
				//BK88_reg[43] += (HSL_REG_MAIN->reg_favor_hue_gain_1 & 0xff) << 8;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+86, (WORD)BK89_reg[43], 0xff00);
				//BK88_reg[44] += HSL_REG_MAIN->reg_favor_sat_gain_1 & 0xff;
    			//BK88_reg[44] += (HSL_REG_MAIN->reg_favor_luma_gain_1 & 0xff) << 8;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+88, (WORD)BK89_reg[44], 0xffff);
				//BK88_reg[45] += HSL_REG_MAIN->reg_dh_favor_adjust_1 & 0x7ff;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+90, (WORD)BK89_reg[45], 0x7ff);
				//BK88_reg[46] += HSL_REG_MAIN->reg_ds_favor_adjust_1 & 0xff;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+92, (WORD)BK89_reg[46], 0xff);
				//BK88_reg[96] += HSL_REG_MAIN->reg_favor_region & 0x1;
			    //BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_1 & 0x1) << 1;
			    //BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_2 & 0x1) << 2;
			    //BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_3 & 0x1) << 3;
			    //BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_4 & 0x1) << 4;
			    //BK88_reg[96] += (HSL_REG_MAIN->reg_favor_region_5 & 0x1) << 5;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+192, (WORD)BK89_reg[96], 0x3f);
				//BK88_reg[104] += HSL_REG_MAIN->reg_pattern_b & 0xFFF;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+208, (WORD)BK89_reg[104], 0xfff);
				//BK88_reg[105] += HSL_REG_MAIN->reg_pattern_g & 0xFFF;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+210, (WORD)BK89_reg[105], 0xfff);
				//BK88_reg[106] += HSL_REG_MAIN->reg_pattern_r & 0xFFF;
				msWrite2ByteMask(u32BaseAddr + SC6A_00+212, (WORD)BK89_reg[106], 0xfff);
				*/
                for (i = 3; i < 128; i++)
                {
                    // BK 88
                    msWrite2Byte(u32BaseAddr + SC6A_00 + (2*i), (WORD)BK88_reg[i]);
                }
				// BK 89
				msWrite2ByteMask(u32BaseAddr + SC6B_00+36, (WORD)BK89_reg[18], 0x3fff);
				msWrite2ByteMask(u32BaseAddr + SC6B_00+138, (WORD)BK89_reg[69], 0x3fff);
				msWrite2ByteMask(u32BaseAddr + SC6B_00+140, (WORD)BK89_reg[70], 0x7f);
				
				/*msWrite2ByteMask(u32BaseAddr + SC6B_02, (WORD)BK89_reg[1], 0x7ff);
				for	(i = 2; i < 128; i++)
				{
					// BK 89
                    msWrite2Byte(u32BaseAddr + SC6B_00 + (2*i), (WORD)BK89_reg[i]);
                }*/
            }
            //else//Sub
            {
                // TBD. 
            }
        }
        //u8ScalerIdx++;
        //u16ScalerMask >>= 1;
    }
}

void msAPI_HSY_AdjustSkinTone(BYTE u8SkinToneIdx)
{
    stHSL_REG HSL_REG_MAIN;

	msAPI_HSY_SkinToneTable(&HSL_REG_MAIN, u8SkinToneIdx);

    // Write table into SRAM
    msAPI_LOAD_HSY_SKINTONE_TABLE(MAIN_WINDOW, &HSL_REG_MAIN);
}

/*-------------------------------------------------------------------------
	Function	: msASPI_AdjustCustomInit
	Input	: dH_UI[6], dS_UI[6], dY_UI[6] array for six-axis user setting
	Output	: dH_UI[6], dS_UI[6], dY_UI[6]
	Feature	: Initialize as default value for six-axis user adjustment, 
			   and set UV_NORM_SCALE register for color engine.
*--------------------------------------------------------------------------*/
void msASPI_AdjustCustomInit(int *dH_UI, int *dS_UI, int *dY_UI)
{ 
	int i;
	//------------------User setting --------------------------- 
	// dH_UI, dS_UI, dY_UI: User setting for 6 axis adjusment 
	for (i = 0; i < 6; i++)
	{
		dH_UI[i] = 0; 	//RYGCBM: -1023~+1023
		dS_UI[i] = 128; 	//RYGCBM: 0~255
		dY_UI[i] = 0; 	//RYGCBM: -1023 ~ +1023
	}

}


/*-------------------------------------------------------------------------
	Function	: msAPI_HSY_AdjustCustomHue
	Input	: s16offset: UI adjustment, which is the offset of 0. (-1023 ~ +1023)
			  u8ColorIndex: R,G,B,C,M,Y
			  *dH_UI: Current user 6-axis setting for Hue 
			  *dS_UI: Current user 6-axis setting for Sat
			  *dY_UI: Current user 6-axis setting for Luma
	Output	: *dH_UI: Updated user 6-axis setting for Hue 
			  *dS_UI: Updated user 6-axis setting for Sat
			  *dY_UI: Updated user 6-axis setting for Luma
	Feature	: User adjust one color's hue and write output into HW registers. 
*--------------------------------------------------------------------------*/

void msAPI_HSY_AdjustCustomHue(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, HSL_COMPEN_GEN_SEL tablegen_sel)
{
    int offset;

    // protect input value is reasonable 
    offset = (abs(s16offset) > 1023)? 1023: abs(s16offset);
    offset = offset*SIGN(s16offset);

    dH_UI[u8ColorIndex] = offset;

    if (tablegen_sel == GEN_BY_FW)
        MST9U6_DRIVER_TABLE(u8WinIdx, dH_UI, dS_UI, dY_UI);
    else if (tablegen_sel == GEN_BY_HW)
        MST9U6_HW_TABLE(u8WinIdx, dH_UI, dS_UI, dY_UI);
    else
        printf("table gen format: error setting\n"); 
}


/*-------------------------------------------------------------------------
	Function	: msAPI_AdjustCustomSat
	Input	: s16offset: UI adjustment, which is the ratio of 128. (0 ~ 255)
			  u8ColorIndex: R,G,B,C,M,Y
			  *dH_UI: Current user 6-axis setting for Hue 
			  *dS_UI: Current user 6-axis setting for Sat
			  *dY_UI: Current user 6-axis setting for Luma
	Output	: *dH_UI: Updated user 6-axis setting for Hue 
			  *dS_UI: Updated user 6-axis setting for Sat
			  *dY_UI: Updated user 6-axis setting for Luma
	Feature	: User adjust one color's saturation and write output into HW registers. 
*--------------------------------------------------------------------------*/
void msAPI_HSY_AdjustCustomSat(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, HSL_COMPEN_GEN_SEL tablegen_sel)
{
    int offset;
    // protect input value is reasonable 
    offset = (s16offset > 255)? 255: ((s16offset < 0)? 0 : s16offset);

    dS_UI[u8ColorIndex] = s16offset;

    if (tablegen_sel == GEN_BY_FW)
        MST9U6_DRIVER_TABLE(u8WinIdx, dH_UI, dS_UI, dY_UI);
    else if (tablegen_sel == GEN_BY_HW)
        MST9U6_HW_TABLE(u8WinIdx, dH_UI, dS_UI, dY_UI);
    else
        printf("table gen format: error setting\n");
}


/*-------------------------------------------------------------------------
	Function	: msAPI_AdjustCustomLuma
	Input	: s16offset: UI adjustment, which is the offset of 0. (-1023 ~ +1023)
			  u8ColorIndex: R,G,B,C,M,Y
			  *dH_UI: Current user 6-axis setting for Hue 
			  *dS_UI: Current user 6-axis setting for Sat
			  *dY_UI: Current user 6-axis setting for Luma
	Output	: *dH_UI: Updated user 6-axis setting for Hue 
			  *dS_UI: Updated user 6-axis setting for Sat
			  *dY_UI: Updated user 6-axis setting for Luma
	Feature	: User adjust one color's luma and write output into HW registers. 
*--------------------------------------------------------------------------*/
void msAPI_HSY_AdjustCustomLuma(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, HSL_COMPEN_GEN_SEL tablegen_sel )
{
    int offset;
    // protect input value is reasonable 
    offset = (abs(s16offset) > 1023)? 1023: abs(s16offset);
    offset = offset*SIGN(s16offset);

    dY_UI[u8ColorIndex] = s16offset;

    if (tablegen_sel == GEN_BY_FW)
        MST9U6_DRIVER_TABLE(u8WinIdx, dH_UI, dS_UI, dY_UI);
    else if (tablegen_sel == GEN_BY_HW)
        MST9U6_HW_TABLE(u8WinIdx, dH_UI, dS_UI, dY_UI);
    else
        printf("table gen format: error setting\n");
}


void msAPI_HSY_inital(HSL_COMPEN_GEN_SEL tablegen_sel)
{
    BYTE i;
    stHSL_REG HSL_REG_MAIN;

    /* Init HW register as default value before user setting */	
    // Sets default HW registers
    ColorEngine_REG_INIT(&HSL_REG_MAIN);

    // Write table into SRAM
    msAPI_LOAD_HSY_INIT_TABLE(MAIN_WINDOW, &HSL_REG_MAIN);

    /* Init/Reset internal tables before user setting */
    // Default: Hue: 0  	Sat: 128 		Luma: 0  
    msASPI_AdjustCustomInit(gdH_UI, gdS_UI, gdY_UI);
    for(i = 0 ; i < 6 ; i++)
    {
        msAPI_HSY_AdjustCustomHue(MAIN_WINDOW, 0, i, gdH_UI, gdS_UI, gdY_UI, tablegen_sel);
        msAPI_HSY_AdjustCustomSat(MAIN_WINDOW, 0x80, i, gdH_UI, gdS_UI, gdY_UI, tablegen_sel);
        msAPI_HSY_AdjustCustomLuma(MAIN_WINDOW, 0, i, gdH_UI, gdS_UI, gdY_UI, tablegen_sel);
    }
}

void msAPI_HSY_Reset(HSL_COMPEN_GEN_SEL tablegen_sel)
{
    BYTE i;
    msASPI_AdjustCustomInit(gdH_UI, gdS_UI, gdY_UI);

    for(i = 0 ; i < 6 ; i++)
    {
        msAPI_HSY_AdjustCustomHue(MAIN_WINDOW, 0, i, gdH_UI, gdS_UI, gdY_UI, tablegen_sel);
        msAPI_HSY_AdjustCustomSat(MAIN_WINDOW, 0x80, i, gdH_UI, gdS_UI, gdY_UI, tablegen_sel);
        msAPI_HSY_AdjustCustomLuma(MAIN_WINDOW, 0, i, gdH_UI, gdS_UI, gdY_UI, tablegen_sel);
    }
}

void msAPI_Adjust_Hue(BYTE u8Index, int Offset)
{
    msAPI_HSY_AdjustCustomHue(MAIN_WINDOW, Offset, RemappingHSYIndex(u8Index), gdH_UI, gdS_UI, gdY_UI, GEN_BY_HW);	
}

void msAPI_Adjust_Sat(BYTE u8Index, int Offset)
{
	msAPI_HSY_AdjustCustomSat(MAIN_WINDOW, Offset, RemappingHSYIndex(u8Index), gdH_UI, gdS_UI, gdY_UI, GEN_BY_HW);
}

void msAPI_Adjust_Bri(BYTE u8Index, int Offset)
{
    msAPI_HSY_AdjustCustomLuma(MAIN_WINDOW, Offset, RemappingHSYIndex(u8Index), gdH_UI, gdS_UI, gdY_UI, GEN_BY_HW);	
}

BYTE RemappingHSYIndex(BYTE u8Index)
{
    // u8Index: 0:R, 1:Y, 2:G, 3:C, 4:B, 5:M
    BYTE u8RemapIdx = 0;
    switch(u8Index)
    {
        case 0:
        {
            u8RemapIdx = 0;
        }
        break;
        case 1:
        {
            u8RemapIdx = 2;
        }
        break;
        case 2:
        {
            u8RemapIdx = 4;
        }
        break;
        case 3:
        {
            u8RemapIdx = 3;
        }
        break;
        case 4:
        {
            u8RemapIdx = 5;
        }
        break;
        case 5:
        {
            u8RemapIdx = 1;
        }
        break;
    }

    return u8RemapIdx;
}

#if HSY_TEST_MODE
void msAPI_test(BYTE EnDebug , BYTE u8select)
{	
    //int dH_UI[6], dS_UI[6], dY_UI[6];
    stHSL_REG HSL_REG_MAIN;

    /* Init HW register as default value before user setting */	
    // Sets default HW registers
    ColorEngine_REG_INIT(&HSL_REG_MAIN);

    msAPI_LOAD_HSY_INIT_TABLE(MAIN_WINDOW, &HSL_REG_MAIN);

    /* Init/Reset internal tables before user setting */
    // Default: Hue: 0  	Sat: 128 		Luma: 0  
    msASPI_AdjustCustomInit(gdH_UI, gdS_UI, gdY_UI);

    /*   User setting */
    // Hue Part
    // Ex: Adjust "R" Hue +15 degree (+256) by 4 times (Final Hue adjust +60 degree (+1024)), 

    switch (u8select)
    {
        case 0:
            msAPI_HSY_AdjustCustomHue(MAIN_WINDOW, 0x100, 0, gdH_UI, gdS_UI, gdY_UI, GEN_BY_FW);		// offset = current + new = 0 + 0x100
        break;
        case 1:
            msAPI_HSY_AdjustCustomHue(MAIN_WINDOW, 0x200, 0, gdH_UI, gdS_UI, gdY_UI, GEN_BY_FW);		// offset = current + new = 0x100 + 0x100
        break;
        case 2:
            msAPI_HSY_AdjustCustomHue(MAIN_WINDOW, 0x300, 0, gdH_UI, gdS_UI, gdY_UI, GEN_BY_FW);		// offset = current + new = 0x200 + 0x100
        break;
        case 3:
            msAPI_HSY_AdjustCustomHue(MAIN_WINDOW, 0x3FF, 0, gdH_UI, gdS_UI, gdY_UI, GEN_BY_FW);		// offset = current + new = 0x300 + 0x100
        break;
        // Sat Part
        // Adjust "Y" Sat x2 (128x2) by 1 times, 
        case 4:
            msAPI_HSY_AdjustCustomSat(MAIN_WINDOW, 0xff, 1, gdH_UI, gdS_UI, gdY_UI, GEN_BY_FW);	// offset = current x new = 128*2 = 256
        break;
        // Adjust "Y" Sat x0.5 (128x2x0.5) by 1 times,
        case 5:
            msAPI_HSY_AdjustCustomSat(MAIN_WINDOW, 0x80, 1, gdH_UI, gdS_UI, gdY_UI, GEN_BY_FW);	// offset = current x new = 256*0.5 = 128
        break;
        // Luma Part 
        // Adjust "G" Luma +0.5x (1024*0.5) by 1 times. 
        case 6:
            msAPI_HSY_AdjustCustomLuma(MAIN_WINDOW, -512, 2, gdH_UI, gdS_UI, gdY_UI, GEN_BY_FW);	// offset = current + new = 0 + 512 = 512. 
        break;
    }

#if(ENABLE_MSTV_UART_DEBUG && DEBUG_MODE)
    if(EnDebug)
    {
		
		// GEN_BY_FW DEBUG
        MST9U6_DEBUG_HUE_TABLE(MAIN_WINDOW, GEN_BY_FW);
        MST9U6_DEBUG_SAT_TABLE(MAIN_WINDOW, GEN_BY_FW);
        MST9U6_DEBUG_LUMA_TABLE(MAIN_WINDOW, GEN_BY_FW);
        // GEN_BY_HW DEBUG
        //	MST9U6_DEBUG_HUE_TABLE(MAIN_WINDOW, GEN_BY_HW);
        //	MST9U6_DEBUG_SAT_TABLE(MAIN_WINDOW, GEN_BY_HW);
        //	MST9U6_DEBUG_LUMA_TABLE(MAIN_WINDOW, GEN_BY_HW);
	}
#else
	UNUSED(EnDebug);
#endif
}

#endif


#if FCC_MODE_EN
void MST9U6_FCC_MODE_ENABLE(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
	//int i;
    	//BYTE u8ScalerIdx = 0;
    	//WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	//while(u16ScalerMask)
	{
	   	//if(u16ScalerMask & BIT0)
        	{
            		//u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            		//if(u8ScalerIdx %2)  //Main
            		{
         		// Enable favor color capture : reg_favor_region = 1
				msWriteBit(u32BaseAddr + SC6A_C0, TRUE, BIT0);
            		}
			//else
			{
				//TBD.
			}
		}
		//u8ScalerIdx++;
		//u16ScalerMask >>= 1;
	}

}


void MST9U6_FCC_MODE_DISABLE(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
	//int i;
    	//BYTE u8ScalerIdx = 0;
    	//WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	//while(u16ScalerMask)
	{
	   	//if(u16ScalerMask & BIT0)
        	{
            		//u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            		//if(u8ScalerIdx %2)  //Main
            		{
         		// Disable  favor color capture : reg_favor_region = 0
				msWriteBit(u32BaseAddr + SC6A_C0, FALSE, BIT0);
            		}
			//else
			{
				//TBD.
			}
		}
		//u8ScalerIdx++;
		//u16ScalerMask >>= 1;
	}
}

void MST9U6_FCC_REGION1_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset,  int sat_offset, int luma_offset)
{
    UNUSED(u8WinIdx);
	//int i;
    	//BYTE u8ScalerIdx = 0;
    	//WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	//while(u16ScalerMask)
	{
	   	//if(u16ScalerMask & BIT0)
        	{
            		//u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            		//if(u8ScalerIdx %2)  //Main
            		{

         		// Enable FCC region 1: reg_favor_region_1 = 1 
				msWriteBit(u32BaseAddr + SC6A_C0, region_enable, BIT1);

				if (region_enable)
				{
					// Offset adjust : reg_dh_favor_adjust_1 / reg_ds_favor_adjust_1 / reg_dy_favor_adjust_1
					msWrite2Byte(u32BaseAddr + SC6A_5A, (hue_offset & 0x7ff));
					msWriteByte(u32BaseAddr + SC6A_5C, (sat_offset & 0xff));
					msWrite2Byte(u32BaseAddr + SC6A_5E, (luma_offset & 0x7ff));

		          		if (mode == FCC_SKIN_MODE)
            				{	
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_48,  REG_FAVOR_SKIN_REGION_HUE_L_1 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_4A,  REG_FAVOR_SKIN_REGION_HUE_U_1 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_4C,  REG_FAVOR_SKIN_REGION_SAT_L_1 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_4E,  REG_FAVOR_SKIN_REGION_SAT_U_1 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_50,  REG_FAVOR_SKIN_REGION_LUMA_L_1 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_52,  REG_FAVOR_SKIN_REGION_LUMA_U_1 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_57,  REG_FAVOR_SKIN_HUE_GAIN_1 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_58,  REG_FAVOR_SKIN_SAT_GAIN_1 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_59,  REG_FAVOR_SKIN_LUMA_GAIN_1 & 0xff);
		          		}
					else if (mode == FCC_MOBA_MODE)
					{  		
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_48,  REG_FAVOR_MOBA_REGION_HUE_L_1 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_4A,  REG_FAVOR_MOBA_REGION_HUE_U_1 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_4C,  REG_FAVOR_MOBA_REGION_SAT_L_1 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_4E,  REG_FAVOR_MOBA_REGION_SAT_U_1 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_50,  REG_FAVOR_MOBA_REGION_LUMA_L_1 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_52,  REG_FAVOR_MOBA_REGION_LUMA_U_1 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_57,  REG_FAVOR_MOBA_HUE_GAIN_1 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_58,  REG_FAVOR_MOBA_SAT_GAIN_1 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_59,  REG_FAVOR_MOBA_LUMA_GAIN_1 & 0xff);
            				}
				}
				
            		}
			//else
			{
				//TBD.
			}
		}
		//u8ScalerIdx++;
		//u16ScalerMask >>= 1;	
	}
}


void MST9U6_FCC_REGION2_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset, int sat_offset, int luma_offset)
{
    UNUSED(u8WinIdx);
	//int i;
    	//BYTE u8ScalerIdx = 0;
    	//WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	//while(u16ScalerMask)
	{
	   	//if(u16ScalerMask & BIT0)
        	{
            		//u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            		//if(u8ScalerIdx %2)  //Main
            		{
            		        // Enable FCC region 2: reg_favor_region_2 = 1 
				msWriteBit(u32BaseAddr + SC6A_C0, region_enable, BIT2);

				if (region_enable)
				{
					// Offset adjust : reg_dh_favor_adjust_2  / reg_ds_favor_adjust_2/ reg_dy_favor_adjust_2
					msWrite2Byte(u32BaseAddr + SC6A_72, (hue_offset & 0x7ff));
					msWriteByte(u32BaseAddr + SC6A_74, (sat_offset & 0xff));
					msWrite2Byte(u32BaseAddr + SC6A_76, (luma_offset & 0x7ff));
				
            				if (mode == FCC_SKIN_MODE)
            				{								
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_60,  REG_FAVOR_SKIN_REGION_HUE_L_2 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_62,  REG_FAVOR_SKIN_REGION_HUE_U_2 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_64,  REG_FAVOR_SKIN_REGION_SAT_L_2 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_66,  REG_FAVOR_SKIN_REGION_SAT_U_2 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_68,  REG_FAVOR_SKIN_REGION_LUMA_L_2 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_6A,  REG_FAVOR_SKIN_REGION_LUMA_U_2 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_6E,  REG_FAVOR_SKIN_HUE_GAIN_2 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_70,  REG_FAVOR_SKIN_SAT_GAIN_2 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_71,  REG_FAVOR_SKIN_LUMA_GAIN_2 & 0xff);
            				}
					else if (mode == FCC_MOBA_MODE)
					{							
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_60,  REG_FAVOR_MOBA_REGION_HUE_L_2 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_62,  REG_FAVOR_MOBA_REGION_HUE_U_2 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_64,  REG_FAVOR_MOBA_REGION_SAT_L_2 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_66,  REG_FAVOR_MOBA_REGION_SAT_U_2 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_68,  REG_FAVOR_MOBA_REGION_LUMA_L_2 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_6A,  REG_FAVOR_MOBA_REGION_LUMA_U_2 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_6E,  REG_FAVOR_MOBA_HUE_GAIN_2 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_70,  REG_FAVOR_MOBA_SAT_GAIN_2 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_71,  REG_FAVOR_MOBA_LUMA_GAIN_2 & 0xff);
					}
				}
            		}
			//else
			{
				//TBD.
			}
		}
		//u8ScalerIdx++;
		//u16ScalerMask >>= 1;	
	}
}


void MST9U6_FCC_REGION3_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode,  BOOL region_enable, int hue_offset, int sat_offset, int luma_offset)
{
    UNUSED(u8WinIdx);
	//int i;
    	//BYTE u8ScalerIdx = 0;
    	//WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	//while(u16ScalerMask)
	{
	   	//if(u16ScalerMask & BIT0)
        	{
            		//u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            		//if(u8ScalerIdx %2)  //Main
            		{
         		// Enable FCC region 3: reg_favor_region_3 = 1 
				msWriteBit(u32BaseAddr + SC6A_C0, region_enable, BIT3);

				if (region_enable)
				{
					// Offset adjust : reg_dh_favor_adjust_3 / reg_ds_favor_adjust_3/ reg_dy_favor_adjust_3
					msWrite2Byte(u32BaseAddr + SC6A_8A, (hue_offset & 0x7ff));
					msWriteByte(u32BaseAddr + SC6A_8C, (sat_offset & 0xff));
					msWrite2Byte(u32BaseAddr + SC6A_8E, (luma_offset & 0x7ff));

					if (mode == FCC_SKIN_MODE)
					{
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_78,  REG_FAVOR_SKIN_REGION_HUE_L_3 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_7A,  REG_FAVOR_SKIN_REGION_HUE_U_3 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_7C,  REG_FAVOR_SKIN_REGION_SAT_L_3 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_7E,  REG_FAVOR_SKIN_REGION_SAT_U_3 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_80,  REG_FAVOR_SKIN_REGION_LUMA_L_3 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_82,  REG_FAVOR_SKIN_REGION_LUMA_U_3 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_86,  REG_FAVOR_SKIN_HUE_GAIN_3 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_88,  REG_FAVOR_SKIN_SAT_GAIN_3 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_89,  REG_FAVOR_SKIN_LUMA_GAIN_3 & 0xff);
					}
					else if (mode == FCC_MOBA_MODE)
					{
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_78,  REG_FAVOR_MOBA_REGION_HUE_L_3 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_7A,  REG_FAVOR_MOBA_REGION_HUE_U_3 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_7C,  REG_FAVOR_MOBA_REGION_SAT_L_3 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_7E,  REG_FAVOR_MOBA_REGION_SAT_U_3 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_80,  REG_FAVOR_MOBA_REGION_LUMA_L_3 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_82,  REG_FAVOR_MOBA_REGION_LUMA_U_3 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_86,  REG_FAVOR_MOBA_HUE_GAIN_3 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_88,  REG_FAVOR_MOBA_SAT_GAIN_3 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_89,  REG_FAVOR_MOBA_LUMA_GAIN_3 & 0xff);
					}
				}
			
            		}
			//else
			{
				//TBD.
			}
		}
		//u8ScalerIdx++;
		//u16ScalerMask >>= 1;	
	}
}	


void MST9U6_FCC_REGION4_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset, int sat_offset, int luma_offset)
{
    UNUSED(u8WinIdx);
	//int i;
    	//BYTE u8ScalerIdx = 0;
    	//WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	//while(u16ScalerMask)
	{
	   	//if(u16ScalerMask & BIT0)
        	{
            		//u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            		//if(u8ScalerIdx %2)  //Main
            		{
         		// Enable FCC region 4: reg_favor_region_4 = 1 
				msWriteBit(u32BaseAddr + SC6A_C0, region_enable, BIT4);

				if (region_enable)
				{
					// Offset adjust : reg_dh_favor_adjust_4  / reg_ds_favor_adjust_4/ reg_dy_favor_adjust_4
					msWrite2Byte(u32BaseAddr + SC6A_A2, (hue_offset & 0x7ff));
					msWriteByte(u32BaseAddr + SC6A_A4, (sat_offset & 0xff));
					msWrite2Byte(u32BaseAddr + SC6A_A6, (luma_offset & 0x7ff));

					if (mode == FCC_SKIN_MODE)
					{
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_90,  REG_FAVOR_SKIN_REGION_HUE_L_4 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_92,  REG_FAVOR_SKIN_REGION_HUE_U_4 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_94,  REG_FAVOR_SKIN_REGION_SAT_L_4 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_96,  REG_FAVOR_SKIN_REGION_SAT_U_4 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_98,  REG_FAVOR_SKIN_REGION_LUMA_L_4 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_9A,  REG_FAVOR_SKIN_REGION_LUMA_U_4 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_9E,  REG_FAVOR_SKIN_HUE_GAIN_4 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_A0,  REG_FAVOR_SKIN_SAT_GAIN_4 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_A1,  REG_FAVOR_SKIN_LUMA_GAIN_4 & 0xff);
					}	
					else if (mode == FCC_MOBA_MODE)
					{
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_90,  REG_FAVOR_MOBA_REGION_HUE_L_4 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_92,  REG_FAVOR_MOBA_REGION_HUE_U_4 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_94,  REG_FAVOR_MOBA_REGION_SAT_L_4 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_96,  REG_FAVOR_MOBA_REGION_SAT_U_4 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_98,  REG_FAVOR_MOBA_REGION_LUMA_L_4 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_9A,  REG_FAVOR_MOBA_REGION_LUMA_U_4 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_9E,  REG_FAVOR_MOBA_HUE_GAIN_4 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_A0,  REG_FAVOR_MOBA_SAT_GAIN_4 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_A1,  REG_FAVOR_MOBA_LUMA_GAIN_4 & 0xff);				
					}
					}
            		}
			//else
			{
				//TBD.
			}
		}
		//u8ScalerIdx++;
		//u16ScalerMask >>= 1;	
	}
}


void MST9U6_FCC_REGION5_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset, int sat_offset, int luma_offset)
{
    UNUSED(u8WinIdx);
	//int i;
    	//BYTE u8ScalerIdx = 0;
    	//WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = 0;//g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	//while(u16ScalerMask)
	{
	   	//if(u16ScalerMask & BIT0)
        	{
            		//u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            		//if(u8ScalerIdx %2)  //Main
            		{
         		// Enable FCC region 4: reg_favor_region_5 = 1 
				msWriteBit(u32BaseAddr + SC6A_C0, region_enable, BIT5);

				if (region_enable)
				{
					// Offset adjust : reg_dh_favor_adjust_5  / reg_ds_favor_adjust_5/ reg_dy_favor_adjust_5
					msWrite2Byte(u32BaseAddr + SC6A_BA, (hue_offset & 0x7ff));
					msWriteByte(u32BaseAddr + SC6A_BC, (sat_offset & 0xff));
					msWrite2Byte(u32BaseAddr + SC6A_BE, (luma_offset & 0x7ff));

					if (mode == FCC_SKIN_MODE)
					{
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_A8,  REG_FAVOR_SKIN_REGION_HUE_L_5 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_AA,  REG_FAVOR_SKIN_REGION_HUE_U_5 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_AC,  REG_FAVOR_SKIN_REGION_SAT_L_5 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_AE,  REG_FAVOR_SKIN_REGION_SAT_U_5 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_B0,  REG_FAVOR_SKIN_REGION_LUMA_L_5 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_B2,  REG_FAVOR_SKIN_REGION_LUMA_U_5 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_B6,  REG_FAVOR_SKIN_HUE_GAIN_5 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_B8,  REG_FAVOR_SKIN_SAT_GAIN_5 & 0xff);
						msWriteByte(u32BaseAddr + SC6A_B9,  REG_FAVOR_SKIN_LUMA_GAIN_5 & 0xff);
					}
					else if (mode == FCC_MOBA_MODE)
					{
						// Default value
						msWrite2Byte(u32BaseAddr + SC6A_A8,  REG_FAVOR_MOBA_REGION_HUE_L_5 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_AA,  REG_FAVOR_MOBA_REGION_HUE_U_5 & 0x1fff);
						msWrite2Byte(u32BaseAddr + SC6A_AC,  REG_FAVOR_MOBA_REGION_SAT_L_5 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_AE,  REG_FAVOR_MOBA_REGION_SAT_U_5 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_B0,  REG_FAVOR_MOBA_REGION_LUMA_L_5 & 0xfff);
						msWrite2Byte(u32BaseAddr + SC6A_B2,  REG_FAVOR_MOBA_REGION_LUMA_U_5 & 0xfff);
						msWriteByte(u32BaseAddr + SC6A_B6,  REG_FAVOR_MOBA_HUE_GAIN_5 & 0xff);
					}
				}

            		}
			//else
			{
				//TBD.
			}
		}
		//u8ScalerIdx++;
		//u16ScalerMask >>= 1;	
	}
}

#if TBC
void MST9U6_FCC_OUTSIDE_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode)
{
	//int i;
    	BYTE u8ScalerIdx = 0;
    	WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    	DWORD u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

	int dh_favor_outside = 0x00, ds_favor_outside = 0x00, dy_favor_outside = 0x00;

	if (mode == FCC_SKIN_MODE)
	{
		dh_favor_outside = REG_DH_FAVOR_SKIN_OUTSIDE;//0x00
		ds_favor_outside = REG_DS_FAVOR_SKIN_OUTSIDE;//0x00
		dy_favor_outside = REG_DY_FAVOR_SKIN_OUTSIDE;//0x00
	}
	else if (mode == FCC_MOBA_MODE)
	{
		dh_favor_outside = REG_DH_FAVOR_MOBA_OUTSIDE;
		ds_favor_outside = REG_DS_FAVOR_MOBA_OUTSIDE;
		dy_favor_outside = REG_DY_FAVOR_MOBA_OUTSIDE;	
	}


    while(u16ScalerMask)
    {
        if(u16ScalerMask & BIT0)
        {
            u32BaseAddr = g_ScalerInfo[u8ScalerIdx].u32SiRegBase;

            if(u8ScalerIdx %2)  //Main
            {
                msWriteBit(u32BaseAddr + SC6A_C0, REG_FCC_SKIN_YIN_SEL, BIT7);
                msWrite2Byte(u32BaseAddr + SC6A_C2,  dh_favor_outside & 0x7ff);
                msWriteByte(u32BaseAddr + SC6A_C4, ds_favor_outside & 0xff);
                msWrite2Byte(u32BaseAddr + SC6A_C6,  dy_favor_outside & 0x7ff);
                //msWriteBit(u32BaseAddr + SC6A_C6, REG_HSL_FCC_SKIN_OUTSIDE_Y_SEL, BIT15);
                msWriteBit(u32BaseAddr + SC6A_C7, REG_HSL_FCC_SKIN_OUTSIDE_Y_SEL, BIT7);
            }
            else
            {
                //TBD.
            }
        }
        u8ScalerIdx++;
        u16ScalerMask >>= 1;	
    }
}
#endif

// Example for uses FCC skin mode
void MST9U6_FCC_SKIN_TEST(void)
{

	/* Step 1: Enable FCC*/
	MST9U6_FCC_MODE_ENABLE(MAIN_WINDOW);

	HSL_FCC_MODE_SEL mode;

	/*--- Note: Main difference between SKIN and MOBA mode: "MST9U6_FCC_OUTSIDE_ADJUST", 
	   		    SKIN mode: dh/ds/dy of outside are fixed, 
	                  MOBA mode, dh/ds/dy of outside could be adjusted							---*/

	
	/*   case 1: SkIN mode: 	
	      Example: T0201 assign Hue+15 degreee adjustement for skin region 
	    "reg_dh_favor_adjust_1 = hF8" implies that the largest adjustment of captured skin color is about Hue +15 degree. */
	    
	/* Step 2: Set SKIN/MOBA mode*/
	mode = FCC_SKIN_MODE;

	/* Step 3: User set variables*/
	/* set variable by users  for 5 regions. xxx_offset[i] i is for (i+1)-th region, for example xxx_offset[2] is for REGION-3*/
	int hue_offset[5]	= {0xF8, 0x0, 0x0, 0x0, 0x0}; //{0xf8, 0x0, 0x0, 0x0, 0x0};
	int sat_offset[5]	= {0x80, 0x80, 0x80, 0x80, 0x80};
	int luma_offset[5] 	= {0};
	
	BOOL region_enable[5]   = {TRUE, FALSE, FALSE, FALSE, FALSE};


	/* Step 3: Call function corrsponding region.*/
	MST9U6_FCC_REGION1_ADJUST(MAIN_WINDOW, mode, region_enable[0], hue_offset[0],  sat_offset[0], luma_offset[0]);
	MST9U6_FCC_REGION2_ADJUST(MAIN_WINDOW, mode, region_enable[1], hue_offset[1],  sat_offset[1], luma_offset[1]);
	MST9U6_FCC_REGION3_ADJUST(MAIN_WINDOW, mode, region_enable[2], hue_offset[2],  sat_offset[2], luma_offset[2]);
	MST9U6_FCC_REGION4_ADJUST(MAIN_WINDOW, mode, region_enable[3], hue_offset[3],  sat_offset[3], luma_offset[3]);
	MST9U6_FCC_REGION5_ADJUST(MAIN_WINDOW, mode, region_enable[4], hue_offset[4],  sat_offset[4], luma_offset[4]);

	MST9U6_FCC_OUTSIDE_ADJUST(MAIN_WINDOW, mode);

}


// Example for uses FCC game mode
void MST9U6_FCC_MOBA_TEST(void)
{

	/* Step 1: Enable FCC*/
	MST9U6_FCC_MODE_ENABLE(MAIN_WINDOW);

	HSL_FCC_MODE_SEL mode;

	/*--- Note: Main difference between SKIN and MOBA mode: "MST9U6_FCC_OUTSIDE_ADJUST", 
	   		    SKIN mode: dh/ds/dy of outside are fixed, 
	                  MOBA mode, dh/ds/dy of outside could be adjusted							---*/

	
	/*   case : MOBA mode 
	      Ex: T205 Keep health colors, and set saturation = 0 to the other colors, and gray to Yin */
	    
	/* Step 2: Set SKIN/MOBA mode*/
	mode = FCC_MOBA_MODE;

	/* Step 3: User set variables*/
	/* set variable by users  for 5 regions. xxx_offset[0] i is for REGION1, xxx_offset[2] is for REGION3*/
	int hue_offset[5] 		= {0x0, 0x0, 0x0, 0x0, 0x0};
	int sat_offset[5] 		= {0x80, 0x80, 0x80, 0x80, 0x80};
	int luma_offset[5] 	= {0};
	
	BOOL region_enable[5]   = {TRUE, TRUE, FALSE, FALSE, FALSE};


	/* Step 4: Call function corrsponding region.*/
	MST9U6_FCC_REGION1_ADJUST(MAIN_WINDOW, mode, region_enable[0], hue_offset[0],  sat_offset[0], luma_offset[0]);
	MST9U6_FCC_REGION2_ADJUST(MAIN_WINDOW, mode, region_enable[1], hue_offset[1],  sat_offset[1], luma_offset[1]);
	MST9U6_FCC_REGION3_ADJUST(MAIN_WINDOW, mode, region_enable[2], hue_offset[2],  sat_offset[2], luma_offset[2]);
	MST9U6_FCC_REGION4_ADJUST(MAIN_WINDOW, mode, region_enable[3], hue_offset[3],  sat_offset[3], luma_offset[3]);
	MST9U6_FCC_REGION5_ADJUST(MAIN_WINDOW, mode, region_enable[4], hue_offset[4],  sat_offset[4], luma_offset[4]);

	MST9U6_FCC_OUTSIDE_ADJUST(MAIN_WINDOW, mode);
	

}

#endif

