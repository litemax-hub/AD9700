// HSY.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "types.h"
#include "Ms_rwreg.h"
#include "ms_reg.h"
#include "dri_table_gen.h"

#if ENABLE_ADL
#include "drvIMI.h"
#endif

#if !HSY_MODE
#include "drvAdjust.h"
#endif

const WORD SAT_BY_HUE_TABLE[216] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, \
							0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, \
							0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, \
							0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, \
							0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, \
							0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, \
							0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, \
							0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, \
							0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, \
							0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, \
							0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, \
							0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, \
							0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, \
							0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, \
							0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, \
							0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, \
							0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, \
							0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };




int g_dH_UI[6], g_dS_UI[6], g_dY_UI[6];

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

void INIT_TABLE_GENERATOR(int* dHt, int* dSt, int*dYt, int* dSt_h, int* dYt_h, int* dSt_s, int* dYt_s, int* dYt_y)
{
	int i;
	for ( i = 0; i < MT9700_NUM_REF_AXIS; i++)
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

	int luma_alpha1_diff = ALPHA_DENOMINATOR - REG_LUMA_ALPHA1;
	int luma_alpha2_diff = ALPHA_DENOMINATOR - REG_LUMA_ALPHA2;
	int luma_alpha1 = REG_LUMA_ALPHA1;
	int luma_alpha2 = REG_LUMA_ALPHA2;


	int nxtcolor = (color == 5) ? 0 : color + 1;

	int min_ds = min(dS_UI[color], dS_UI[nxtcolor]);
	int max_ds = max(dS_UI[color], dS_UI[nxtcolor]);

	int minabs_dy = min_abs(dY_UI[color], dY_UI[nxtcolor]);
	int maxabs_dy = max_abs(dY_UI[color], dY_UI[nxtcolor]);

	dS_UI_24[color << 2] = dS_UI[color];
	dS_UI_24[(color << 2) + 1] = (dS_UI[color] * sat_alpha1_diff + min_ds * sat_alpha1) >> ALPHA_FRAC_BIT;
	dS_UI_24[(color << 2) + 2] = (min_ds * sat_alpha2_diff + max_ds * sat_alpha2) >> ALPHA_FRAC_BIT;
	dS_UI_24[(color << 2) + 3] = (dS_UI[nxtcolor] * sat_alpha1_diff + min_ds * sat_alpha1) >> ALPHA_FRAC_BIT;

	dY_UI_24[color << 2] = dY_UI[color];
	dY_UI_24[(color << 2) + 1] = (minabs_dy*luma_alpha1 + dY_UI[color] * luma_alpha1_diff) >> ALPHA_FRAC_BIT;
	dY_UI_24[(color << 2) + 2] = (minabs_dy*luma_alpha2_diff + maxabs_dy*luma_alpha2) >> ALPHA_FRAC_BIT;
	dY_UI_24[(color << 2) + 3] = (minabs_dy*luma_alpha1_diff + dY_UI[nxtcolor]*luma_alpha1) >> ALPHA_FRAC_BIT;
#if 0
	dS_UI_24[(color << 2) + 1] = (dS_UI[color] * (ALPHA_DENOMINATOR - REG_SAT_ALPHA1) + min_ds * REG_SAT_ALPHA1) >> ALPHA_FRAC_BIT;
	dS_UI_24[(color << 2) + 2] = (min_ds * (ALPHA_DENOMINATOR - REG_SAT_ALPHA2) + max_ds* REG_SAT_ALPHA2) >> ALPHA_FRAC_BIT;
	dS_UI_24[(color << 2) + 3] = (dS_UI[nxtcolor] * (ALPHA_DENOMINATOR - REG_SAT_ALPHA1) + min_ds * REG_SAT_ALPHA1) >> ALPHA_FRAC_BIT;



	dY_UI_24[(color << 2) + 2] = (minabs_dy*(ALPHA_DENOMINATOR - REG_LUMA_ALPHA2) + maxabs_dy*REG_LUMA_ALPHA2) >> ALPHA_FRAC_BIT;
	dY_UI_24[(color << 2) + 3] = (minabs_dy*(ALPHA_DENOMINATOR - REG_LUMA_ALPHA1) + dY_UI[nxtcolor]*REG_LUMA_ALPHA1) >> ALPHA_FRAC_BIT;
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

	int dYt_s   = ((Yr - L_REF) * (dS_after_hue - S_RATIO_1x)) / S_RATIO_1x;
	dYt[idx] = minmax(dYt_h[idx] + ((dYt_s + dY_after_hue) * REG_HSL_UVNORM_MAX / UVr), DY_LOWER_BOUND, DY_UPPER_BOUND) ;

}




void driver_table_MT9700(int *dH_UI, int *dS_UI, int *dY_UI)
{

	//------------------ Init adjusment value ---------------------------
	int num_user_axis = 6;
	int dHt[MT9700_NUM_REF_AXIS], dSt[MT9700_NUM_REF_AXIS], dYt[MT9700_NUM_REF_AXIS];   // dHt, dSt, dYt: final adjustment for N axis.
	int dSt_h[MT9700_NUM_REF_AXIS], dYt_h[MT9700_NUM_REF_AXIS];  // dSt_h, dYt_h: sat/luma compensation cause from Hue adjust
	int dSt_s[MT9700_NUM_REF_AXIS], dYt_s[MT9700_NUM_REF_AXIS];  // dSt_s: sat adjustment casue from user, dYt_s: luma compensation cause from Sat adjust
	int dYt_y[MT9700_NUM_REF_AXIS];   // dYt_y: luma adjusment cause from user
	int dS_UI_24[MT9700_NUM_REF_AXIS], dY_UI_24[MT9700_NUM_REF_AXIS];


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


// Output
// Monitor hue table. Format T11
	msWrite2Byte(SC69_68, (WORD)(dHt[0]&0xfff));
	msWrite2Byte(SC69_6A, (WORD)(dHt[1]&0xfff));
	msWrite2Byte(SC69_6C, (WORD)(dHt[2]&0xfff));
	msWrite2Byte(SC69_6E, (WORD)(dHt[3]&0xfff));
	msWrite2Byte(SC69_70, (WORD)(dHt[4]&0xfff));
	msWrite2Byte(SC69_72, (WORD)(dHt[5]&0xfff));
	msWrite2Byte(SC69_74, (WORD)(dHt[6]&0xfff));
	msWrite2Byte(SC69_76, (WORD)(dHt[7]&0xfff));
	msWrite2Byte(SC69_78, (WORD)(dHt[8]&0xfff));
	msWrite2Byte(SC69_7A, (WORD)(dHt[9]&0xfff));
	msWrite2Byte(SC69_7C, (WORD)(dHt[10]&0xfff));
	msWrite2Byte(SC69_7E, (WORD)(dHt[11]&0xfff));
	msWrite2Byte(SC69_80, (WORD)(dHt[12]&0xfff));
	msWrite2Byte(SC69_82, (WORD)(dHt[13]&0xfff));
	msWrite2Byte(SC69_84, (WORD)(dHt[14]&0xfff));
	msWrite2Byte(SC69_86, (WORD)(dHt[15]&0xfff));
	msWrite2Byte(SC69_88, (WORD)(dHt[16]&0xfff));
	msWrite2Byte(SC69_8A, (WORD)(dHt[17]&0xfff));
	msWrite2Byte(SC69_8C, (WORD)(dHt[18]&0xfff));
	msWrite2Byte(SC69_8E, (WORD)(dHt[19]&0xfff));
	msWrite2Byte(SC69_90, (WORD)(dHt[20]&0xfff));
	msWrite2Byte(SC69_92, (WORD)(dHt[21]&0xfff));
	msWrite2Byte(SC69_94, (WORD)(dHt[22]&0xfff));
	msWrite2Byte(SC69_96, (WORD)(dHt[23]&0xfff));

// Monitor sat table. Format 1.7
	msWriteByte(SC69_98, (BYTE)(dSt[0]&0xff));
	msWriteByte(SC69_99, (BYTE)(dSt[1]&0xff));
	msWriteByte(SC69_9A, (BYTE)(dSt[2]&0xff));
	msWriteByte(SC69_9B, (BYTE)(dSt[3]&0xff));
	msWriteByte(SC69_9C, (BYTE)(dSt[4]&0xff));
	msWriteByte(SC69_9D, (BYTE)(dSt[5]&0xff));
	msWriteByte(SC69_9E, (BYTE)(dSt[6]&0xff));
	msWriteByte(SC69_9F, (BYTE)(dSt[7]&0xff));
	msWriteByte(SC69_A0, (BYTE)(dSt[8]&0xff));
	msWriteByte(SC69_A1, (BYTE)(dSt[9]&0xff));
	msWriteByte(SC69_A2, (BYTE)(dSt[10]&0xff));
	msWriteByte(SC69_A3, (BYTE)(dSt[11]&0xff));
	msWriteByte(SC69_A4, (BYTE)(dSt[12]&0xff));
	msWriteByte(SC69_A5, (BYTE)(dSt[13]&0xff));
	msWriteByte(SC69_A6, (BYTE)(dSt[14]&0xff));
	msWriteByte(SC69_A7, (BYTE)(dSt[15]&0xff));
	msWriteByte(SC69_A8, (BYTE)(dSt[16]&0xff));
	msWriteByte(SC69_A9, (BYTE)(dSt[17]&0xff));
	msWriteByte(SC69_AA, (BYTE)(dSt[18]&0xff));
	msWriteByte(SC69_AB, (BYTE)(dSt[19]&0xff));
	msWriteByte(SC69_AC, (BYTE)(dSt[20]&0xff));
	msWriteByte(SC69_AD, (BYTE)(dSt[21]&0xff));
	msWriteByte(SC69_AE, (BYTE)(dSt[22]&0xff));
	msWriteByte(SC69_AF, (BYTE)(dSt[23]&0xff));

// Monitor luma table. Format T11
	msWrite2Byte(SC69_B0, (WORD)(dYt[0]&0x7ff));
	msWrite2Byte(SC69_B2, (WORD)(dYt[1]&0x7ff));
	msWrite2Byte(SC69_B4, (WORD)(dYt[2]&0x7ff));
	msWrite2Byte(SC69_B6, (WORD)(dYt[3]&0x7ff));
	msWrite2Byte(SC69_B8, (WORD)(dYt[4]&0x7ff));
	msWrite2Byte(SC69_BA, (WORD)(dYt[5]&0x7ff));
	msWrite2Byte(SC69_BC, (WORD)(dYt[6]&0x7ff));
	msWrite2Byte(SC69_BE, (WORD)(dYt[7]&0x7ff));
	msWrite2Byte(SC69_C0, (WORD)(dYt[8]&0x7ff));
	msWrite2Byte(SC69_C2, (WORD)(dYt[9]&0x7ff));
	msWrite2Byte(SC69_C4, (WORD)(dYt[10]&0x7ff));
	msWrite2Byte(SC69_C6, (WORD)(dYt[11]&0x7ff));
	msWrite2Byte(SC69_C8, (WORD)(dYt[12]&0x7ff));
	msWrite2Byte(SC69_CA, (WORD)(dYt[13]&0x7ff));
	msWrite2Byte(SC69_CC, (WORD)(dYt[14]&0x7ff));
	msWrite2Byte(SC69_CE, (WORD)(dYt[15]&0x7ff));
	msWrite2Byte(SC69_D0, (WORD)(dYt[16]&0x7ff));
	msWrite2Byte(SC69_D2, (WORD)(dYt[17]&0x7ff));
	msWrite2Byte(SC69_D4, (WORD)(dYt[18]&0x7ff));
	msWrite2Byte(SC69_D6, (WORD)(dYt[19]&0x7ff));
	msWrite2Byte(SC69_D8, (WORD)(dYt[20]&0x7ff));
	msWrite2Byte(SC69_DA, (WORD)(dYt[21]&0x7ff));
	msWrite2Byte(SC69_DC, (WORD)(dYt[22]&0x7ff));
	msWrite2Byte(SC69_DE, (WORD)(dYt[23]&0x7ff));

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

	msWrite2ByteMask(SC6B_02, (WORD)(REG_HSL_UVNORM_MAX & 0x7ff) , 0x7FF);
	msWrite2Byte(SC69_0A, (WORD)(REG_HSL_UVNORM_SCALE &0x7ff));

}

/*-------------------------------------------------------------------------
	Function	: msAPI_AdjustCustomHue
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

void msAPI_AdjustCustomHue(int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI)
{
	dH_UI[u8ColorIndex] = s16offset;

	driver_table_MT9700(dH_UI, dS_UI, dY_UI);
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
void msAPI_AdjustCustomSat(int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI)
{
	dS_UI[u8ColorIndex] = s16offset;

	driver_table_MT9700(dH_UI, dS_UI, dY_UI);
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
void msAPI_AdjustCustomLuma(int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI)
{
	dY_UI[u8ColorIndex] = s16offset;

	driver_table_MT9700(dH_UI, dS_UI, dY_UI);
}
#if ENABLE_ADL
void msAPI_ADL_LOAD_HSY_INIT_TABLE(void)
{
	BYTE i;
	IMI_Init(0, 108 * 256);
	for (i = 0; i < 108; i++)
	{
		IMI_WriteByte(i*32, SAT_BY_HUE_TABLE[i*2]);
		IMI_WriteByte(i*32 + 16, SAT_BY_HUE_TABLE[(i*2)+1]);
	}

	msWriteByte(SC67_AE, 0x00); //reg_clientc_baseadr
	msWriteByte(SC67_AF, 0x00);
	msWriteByte(SC67_B0, 0x00);
	msWriteByte(SC67_B1, 0x00);
	msWriteByte(SC67_B2, 0x6C); // reg_clientc_depth
	msWriteByte(SC67_B3, 0x00);
	msWriteByte(SC67_B5, 0x00);
	msWriteByte(SC67_B4, 0x40); //reg_clientc_req_len
	msWriteByte(SC67_B7, 0x00); //reg_clientc_ini_addr
	msWriteByte(SC67_B6, 0x00);

	msWriteByte(SC67_AD, 0x00);
	msWriteByteMask(SC67_AC, BIT0, BIT0); // reg_clientc_en
	msWriteByteMask(SC69_60, BIT2, BIT2);
	//msWriteByte(SC67_AC, 0x01);
}
#endif
/*-------------------------------------------------------------------------
	Function	: msAPI_LOAD_HSY_INIT_TABLE
	Feature	: Load init table for HSY
*--------------------------------------------------------------------------*/
void msAPI_LOAD_HSY_INIT_TABLE(void)
{
// enable hue/sat/luma process
	msWrite2Byte(SC69_04, 0xf891);

// disable bypass mode. reg_hsl_path_bypass = 0. (0:disable 1:enable)
	msWriteBit(SC6A_2E, FALSE, BIT0);

// Enable driver compensation.
	msWriteBit(SC6B_03, TRUE, BIT4);//msWriteBit(SC6B_02, TRUE, BIT12);

// saturation init table
#if ENABLE_ADL
	msAPI_ADL_LOAD_HSY_INIT_TABLE();
#else
	BYTE i;
	for (i = 0; i < 216; i++)
	{
		msWrite2Byte(SC69_64, i);
		msWrite2Byte(SC69_66, SAT_BY_HUE_TABLE[i]);
		msWrite2Byte(SC69_60, 0x4080);
	}
#endif
// Write UVNORM_SCALE and UVNORM_MAX
 	msWrite2ByteMask(SC6B_02, (WORD)(REG_HSL_UVNORM_MAX & 0x7ff) , 0x7FF);
	msWrite2Byte(SC69_0A, (WORD)(REG_HSL_UVNORM_SCALE &0x7ff));

//reg_r2y_xx: spec 601
	msWrite2Byte(SC69_20, 0x653);
	msWrite2Byte(SC69_22, 0x7ad);
	msWrite2Byte(SC69_24, 0x132);
	msWrite2Byte(SC69_26, 0x259);
	msWrite2Byte(SC69_28, 0x75);
	msWrite2Byte(SC69_2A, 0x753);
	msWrite2Byte(SC69_2C, 0x6ad);

// reg_y2r_xx: spec 601
	msWrite2Byte(SC6A_40, 0x59c);
	msWrite2Byte(SC6A_42, 0x1d25);
	msWrite2Byte(SC6A_44, 0x1ea0);
	msWrite2Byte(SC6A_46, 0x717);

//gray protection
	msWrite2Byte(SC69_3C, 0x0);
	msWriteByte(SC69_3E, 0x3F);
	msWriteByte(SC69_3F, 0x3F);


//HSL UV angle offset and scale
	msWrite2Byte(SC6A_30, 0x2e6);
	msWrite2Byte(SC6A_32, 0x1a9);
	msWrite2Byte(SC6A_34, 0x2f6);
	msWrite2Byte(SC6A_36, 0x186);
	msWrite2Byte(SC6A_38, 0x328);
	msWrite2Byte(SC6A_3A,0x2ee);

// reg_Sat_by_luma_0~8
	msWriteByte(SC69_EC, 0x80);
	msWriteByte(SC69_ED, 0x80);
	msWriteByte(SC69_EE, 0x80);
	msWriteByte(SC69_EF, 0x80);
	msWriteByte(SC69_F0, 0x80);
	msWriteByte(SC69_F1, 0x80);
	msWriteByte(SC69_F2, 0x80);
	msWriteByte(SC69_F3, 0x80);
	msWriteByte(SC69_F4, 0x80);

// Init monitor_hue/sat/luma table
// Monitor hue table. Format T11
	msWrite2Byte(SC69_68, 0);
	msWrite2Byte(SC69_6A, 0);
	msWrite2Byte(SC69_6C, 0);
	msWrite2Byte(SC69_6E, 0);
	msWrite2Byte(SC69_70, 0);
	msWrite2Byte(SC69_72, 0);
	msWrite2Byte(SC69_74, 0);
	msWrite2Byte(SC69_76, 0);
	msWrite2Byte(SC69_78, 0);
	msWrite2Byte(SC69_7A, 0);
	msWrite2Byte(SC69_7C, 0);
	msWrite2Byte(SC69_7E, 0);
	msWrite2Byte(SC69_80, 0);
	msWrite2Byte(SC69_82, 0);
	msWrite2Byte(SC69_84, 0);
	msWrite2Byte(SC69_86, 0);
	msWrite2Byte(SC69_88, 0);
	msWrite2Byte(SC69_8A, 0);
	msWrite2Byte(SC69_8C, 0);
	msWrite2Byte(SC69_8E, 0);
	msWrite2Byte(SC69_90, 0);
	msWrite2Byte(SC69_92, 0);
	msWrite2Byte(SC69_94, 0);
	msWrite2Byte(SC69_96, 0);

// Monitor sat table. Format 1.7
	msWriteByte(SC69_98, 0x80);
	msWriteByte(SC69_99, 0x80);
	msWriteByte(SC69_9A, 0x80);
	msWriteByte(SC69_9B, 0x80);
	msWriteByte(SC69_9C, 0x80);
	msWriteByte(SC69_9D, 0x80);
	msWriteByte(SC69_9E, 0x80);
	msWriteByte(SC69_9F, 0x80);
	msWriteByte(SC69_A0, 0x80);
	msWriteByte(SC69_A1, 0x80);
	msWriteByte(SC69_A2, 0x80);
	msWriteByte(SC69_A3, 0x80);
	msWriteByte(SC69_A4, 0x80);
	msWriteByte(SC69_A5, 0x80);
	msWriteByte(SC69_A6, 0x80);
	msWriteByte(SC69_A7, 0x80);
	msWriteByte(SC69_A8, 0x80);
	msWriteByte(SC69_A9, 0x80);
	msWriteByte(SC69_AA, 0x80);
	msWriteByte(SC69_AB, 0x80);
	msWriteByte(SC69_AC, 0x80);
	msWriteByte(SC69_AD, 0x80);
	msWriteByte(SC69_AE, 0x80);
	msWriteByte(SC69_AF, 0x80);

// Monitor luma table. Format T11
	msWrite2Byte(SC69_B0, 0);
	msWrite2Byte(SC69_B2, 0);
	msWrite2Byte(SC69_B4, 0);
	msWrite2Byte(SC69_B6, 0);
	msWrite2Byte(SC69_B8, 0);
	msWrite2Byte(SC69_BA, 0);
	msWrite2Byte(SC69_BC, 0);
	msWrite2Byte(SC69_BE, 0);
	msWrite2Byte(SC69_C0, 0);
	msWrite2Byte(SC69_C2, 0);
	msWrite2Byte(SC69_C4, 0);
	msWrite2Byte(SC69_C6, 0);
	msWrite2Byte(SC69_C8, 0);
	msWrite2Byte(SC69_CA, 0);
	msWrite2Byte(SC69_CC, 0);
	msWrite2Byte(SC69_CE, 0);
	msWrite2Byte(SC69_D0, 0);
	msWrite2Byte(SC69_D2, 0);
	msWrite2Byte(SC69_D4, 0);
	msWrite2Byte(SC69_D6, 0);
	msWrite2Byte(SC69_D8, 0);
	msWrite2Byte(SC69_DA, 0);
	msWrite2Byte(SC69_DC, 0);
	msWrite2Byte(SC69_DE, 0);

}

#ifdef HSY_MODE
void msAPI_Adjust_test(void)
{
	int dH_UI[6], dS_UI[6], dY_UI[6];

//// Init HW default table
	msAPI_LOAD_HSY_INIT_TABLE();

// Enable driver compensation.
	msWriteBit(SC6B_03, TRUE, BIT4);

// Init as default value before user setting.
// Default: Hue: 0  	Sat: 128 		Luma: 0
	msASPI_AdjustCustomInit(dH_UI, dS_UI, dY_UI);

// User setting
// Ex: Adjust "R" Hue +15 degree (+256) by 4 times (Final Hue adjust +60 degree (+1024)),
	msAPI_AdjustCustomHue(0x100, 0, dH_UI, dS_UI, dY_UI);		// offset = current + new = 0 + 0x100
	msAPI_AdjustCustomHue(0x200, 0, dH_UI, dS_UI, dY_UI);		// offset = current + new = 0x100 + 0x100
	msAPI_AdjustCustomHue(0x300, 0, dH_UI, dS_UI, dY_UI);		// offset = current + new = 0x200 + 0x100
	msAPI_AdjustCustomHue(0x400, 0, dH_UI, dS_UI, dY_UI);		// offset = current + new = 0x300 + 0x100

// Adjust "Y" Sat x2 (128x2) by 1 times,
	msAPI_AdjustCustomSat(0x100, 1, dH_UI, dS_UI, dY_UI);	// offset = current x new = 128*2 = 256
// Adjust "Y" Sat x0.5 (128x2x0.5) by 1 times,
	msAPI_AdjustCustomSat(0x80, 1, dH_UI, dS_UI, dY_UI);	// offset = current x new = 256*0.5 = 128

// Adjust "G" Luma +0.5x (1024*0.5) by 1 times.
	msAPI_AdjustCustomLuma(0x200, 2, dH_UI, dS_UI, dY_UI);	// offset = current + new = 0 + 512 = 512.

}

void msAPI_Adjust_ICC_Init(void)
{
        //// Init HW default table
	msAPI_LOAD_HSY_INIT_TABLE();
	// Enable driver compensation.
	msWriteBit(SC6B_03, TRUE, BIT4);
	msASPI_AdjustCustomInit(g_dH_UI, g_dS_UI, g_dY_UI);
}

void msAPI_Adjust_ICC_Value_Reset(void)
{

// Init as default value before user setting.
// Default: Hue: 0  	Sat: 128 		Luma: 0
BYTE i=0;
	for(i = 0 ; i<6 ;i++)
	{
	msAPI_AdjustCustomHue(0, i, g_dH_UI, g_dS_UI, g_dY_UI);
	msAPI_AdjustCustomSat(0x80, i, g_dH_UI, g_dS_UI, g_dY_UI);
	msAPI_AdjustCustomLuma(0, i, g_dH_UI, g_dS_UI, g_dY_UI);
	}
}
#endif

void msAPI_Adjust_Hue(BYTE u8Index, int Offset )
{
    #if HSY_MODE
    // Init as default value before user setting.
    // Default: Hue: 0  	Sat: 128 		Luma: 0
    // u8Index: 0:R, 1:Y, 2:G, 3:C, 4:B, 5:M
    BYTE u8RemapIdx = 0;
    u8RemapIdx = RemappingHSYIndex(u8Index);    
    msAPI_AdjustCustomHue(Offset, u8RemapIdx, g_dH_UI, g_dS_UI, g_dY_UI);
    #else
    // u8Index: 0:R, 1:G, 2:B, 3:C, 4:M, 5:Y
    msDrvAdjustCustomHue(Offset, u8Index);
    #endif
}

void msAPI_Adjust_Sat(BYTE u8Index, WORD u16Offset )
{
    #if HSY_MODE
    // Init as default value before user setting.
    // Default: Hue: 0  	Sat: 128 		Luma: 0
    // u8Index: 0:R, 1:Y, 2:G, 3:C, 4:B, 5:M
    BYTE u8RemapIdx = 0;
    u8RemapIdx = RemappingHSYIndex(u8Index);
    msAPI_AdjustCustomSat(u16Offset, u8RemapIdx, g_dH_UI, g_dS_UI, g_dY_UI);
    #else
    // u8Index: 0:R, 1:G, 2:B, 3:C, 4:M, 5:Y
    msDrvAdjustCustomSat(u16Offset, u8Index);
    #endif
}

void msAPI_Adjust_Bri(BYTE u8Index, int u16Offset )
{
    #if HSY_MODE
    // Init as default value before user setting.
    // Default: Hue: 0  	Sat: 128 		Luma: 0
    // u8Index: 0:R, 1:Y, 2:G, 3:C, 4:B, 5:M
    BYTE u8RemapIdx = 0;
    u8RemapIdx = RemappingHSYIndex(u8Index);
    msAPI_AdjustCustomLuma(u16Offset, u8RemapIdx, g_dH_UI, g_dS_UI, g_dY_UI);
    #else
    // u8Index: 0:R, 1:G, 2:B, 3:C, 4:M, 5:Y
    msDrvAdjustCustomBri(u16Offset, u8Index);
    #endif
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

