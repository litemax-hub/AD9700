#ifndef _DRI_TABLE_GEN_H
#define _DRI_TABLE_GEN_H

#define DEBUG_MODE			0 //TBC
#define HSY_TEST_MODE		1
//#define ENABLE_HSY          1
#define HSY_MODE    (((GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY) || (INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY)) ? 1 : 0)
#define WRITE_HSY_TABLE_BY_ADL	(1 && CHIP_ID==MST9U5 || (CHIP_ID == CHIP_MT9701))	// 1: write HSY tables by autodownloiad    //0:write HSY tables by RIU

//#define MT9701_LOAD_TABLE	1
#define TABLE_GEN_BY_HW	(1 && CHIP_ID==CHIP_MT9701)	// 1: HW     0:FW

#if (CHIP_ID == MST9U5)
#define ROTATION_LIMIT		1	// 1: Enable (MT9701)	0: Disable (MT9700)
#else
/*FW compensation*/
#define ROTATION_LIMIT		(0 && TABLE_GEN_BY_HW)
#endif
#define FCC_MODE_EN		(1 && TABLE_GEN_BY_HW)	// FCC SKIN/MOBA mode. 1:enable   0:diable 

#define INPUT_LIMITATION	1

#define MT9701_V_MODE		(1 && CHIP_ID==MST9U5) 

#if MT9701_V_MODE
#define MT9701_V_MODE_ADD_dY		1

#define RATIO_NUMERATOR			8			/* ratio: 0.5 ~ 1.5, so  RATIO_NUMERATOR = ratio << (RATIO_DENIMINATOR_BIT).
												 Ex: if RATIO_DENIMINATOR_BIT = 3, ratio = 1, then RATIO_NUMERATOR = 1 << 3 = 8 */
#define RATIO_DENIMINATOR_BIT		3
#endif

#define REG_HSL_UVNORM_MAX	512
#define REG_HSL_UVNORM_SCALE	1024 	// when REG_HSL_UVNORM_SCALE = 1024, REG_HSL_UVNORM_MAX = 512
										// when REG_HSL_UVNORM_SCALE = 724, REG_HSL_UVNORM_MAX = 724

// math library
#define min( a , b )     ( (a) < (b) ? (a) : (b) )
#define max( a , b )     ( (a) > (b) ? (a) : (b) )
#define min_abs( a , b ) ( abs(a) < abs(b) ? (a) : (b) )
#define max_abs( a , b ) ( abs(a) > abs(b) ? (a) : (b) )
#define min3(a,b,c)      min( a, min( b, c ) )
#define max3(a,b,c)      max( a, max( b, c ) )
#define med3(a,b,c)      ( a + b + c - max3( a, b, c ) - min3( a, b, c ) )
#define minmax(v,a,b)    (((v)<(a))? (a) : ((v)>(b)) ? (b) : (v))
#define SIGN(v)  ( (v) >= 0 ? 1: -1 )

//#define MT9700_NUM_REF_AXIS	24

//  ---- For ADL --------
#if WRITE_HSY_TABLE_BY_ADL
#define SIZE_HSV_LOCAL_TABLE_ADL       (5184)   // (36*9)entries * (128/8)bytes
#define DEPTH_ADL_HSY_TABLE		    (36*9)	  // Depth of HSY tables for autodownload format
#endif

// ----- Algorithm Parameters ----------
#define MT9700_NUM_REF_AXIS	24

#define MT9701_NUM_REF_AXIS	36

#define MT9701_HUE_TABLE_LEN	MT9701_NUM_REF_AXIS
#define MT9701_SAT_TABLE_LEN	9
#define MT9701_LUMA_TABLE_LEN	9

#define MT9701_GAIN_HUE_BY_SAT	0xff	// T1.7.  range:-2 ~ +2

#define MST9U6_USER_NUM_AXIS      6
#define MST9U6_NUM_REF_AXIS	        24

// API INPUT REGISTER SETTING

#define	REG_HSL_UVNORM_MAX				512     // 724 // 11 bit, when REG_HSL_UVNORM_MAX changes. It need to change "REG_HSL_UVNORM_SCALE"
#define 	REG_HSL_UVNORM_SCALE	    1024 	//724 	// when REG_HSL_UVNORM_MAX = 512, REG_HSL_UVNORM_SCALE will be 1024; when REG_HSL_UVNORM_MAX = 724, REG_HSL_UVNORM_SCALE = 724

#define	REG_HUE_ALPHA1	16; 	//HSLReg.reg_hue_alpha1
#define	REG_HUE_ALPHA2	32; 	//HSLReg.reg_hue_alpha2;
#define	REG_SAT_ALPHA1	32; 	//HSLReg.reg_sat_alpha1;
#define	REG_SAT_ALPHA2	0;	//HSLReg.reg_sat_alpha2;
#define	REG_LUMA_ALPHA1	32; 	//HSLReg.reg_luma_alpha1;
#define	REG_LUMA_ALPHA2	  0;  //HSLReg.reg_luma_alpha2;

#define	REG_HSL_L_Y_BLEND_ALPHA 0
#define LUMA_ALPHA				4		// Alpha: 0 - 32
#define LUMA_BETA				16		// BETA: 0 - 32
#define LUMA_ALPHA_BETA_BIT	5

// ALPHA Value
#define ALPHA_FRAC_BIT		6
#define ALPHA_DENOMINATOR (1 << ALPHA_FRAC_BIT)

// Hue constant
#define H_DEGREE_15			256
#define H_DEGREE_360		6144 	// 256*24 = 15 degree x24 = 360 degree
#define H_DEGREE_15_BIT	8		// (1 << H_DEGREE_15_BIT) means 15 degree
#define H_BLENDING_ROUNDING_FACTOR	(1 << (H_DEGREE_15_BIT - 1))

// Sat constant
#define S_FRAC_BIT		7						// Saturation format: 1.7
#define S_RATIO_1x		(1 << S_FRAC_BIT)		// (1 << S_RATIO_1x) means sat 1x

// Luma constant
#define L_REF	512  				// Reference Y. 512: means 0.5

/*-----------------------
#define dH_FRAC_BIT		10	//dH: (total bit, frac bit) = signed (12,10), dH = 1023 means 60 degree. dH: -1023 ~ +1023
#define dS_FRAC_BIT		7	//dS: (total bit, frac bit) = unsigned [8, 7], dS = 128 means x1, dS = 256 means x2. dS: 0 ~ 255
#define dY_FRAC_BIT		10	//dY: (total bit, frac bit) = signed (12,10), dY = 1023 means +1. dH: -1023 ~ +1023. dY: -1023 ~ +1023
------------------------*/
// dH dS dY limit
#define DH_DEFAULT_VALUE	0
#define DH_UPPER_BOUND		 1535   	// 1023: means 60 degress ; 1535: means 90 degree
#define DH_LOWER_BOUND	-1535

#define DS_DEFAULT_VALUE	128
#define DS_UPPER_BOUND		255
#define DS_LOWER_BOUND	0

#define DY_DEFAULT_VALUE	0
#define DY_UPPER_BOUND 	1023
#define DY_LOWER_BOUND     -1023

#if FCC_MODE_EN
//REGION-1
#define REG_FAVOR_SKIN_REGION_HUE_L_1	0xC0		// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_HUE_U_1	0x280		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_SAT_L_1	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_SAT_U_1	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_L_1 0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_U_1 0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_HUE_GAIN_1		0x8			// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_SAT_GAIN_1		0x1			// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_LUMA_GAIN_1		0x1			// Luma gain: 0 ~ 0xFF

//REGION-2
#define REG_FAVOR_SKIN_REGION_HUE_L_2	0xC0		// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_HUE_U_2	0x280		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_SAT_L_2	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_SAT_U_2	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_L_2 0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_U_2 0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_HUE_GAIN_2		0x8			// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_SAT_GAIN_2		0x1			// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_LUMA_GAIN_2		0x1			// Luma gain: 0 ~ 0xFF

//REGION-3
#define REG_FAVOR_SKIN_REGION_HUE_L_3	0xC0		// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_HUE_U_3	0x280		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_SAT_L_3	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_SAT_U_3	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_L_3 0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_U_3 0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_HUE_GAIN_3		0x8			// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_SAT_GAIN_3		0x1			// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_LUMA_GAIN_3		0x1			// Luma gain: 0 ~ 0xFF

//REGION-4
#define REG_FAVOR_SKIN_REGION_HUE_L_4	0xC0		// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_HUE_U_4	0x280		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_SAT_L_4	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_SAT_U_4	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_L_4 0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_U_4 0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_HUE_GAIN_4		0x8			// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_SAT_GAIN_4		0x1			// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_LUMA_GAIN_4		0x1			// Luma gain: 0 ~ 0xFF

//REGION-5
#define REG_FAVOR_SKIN_REGION_HUE_L_5	0xC0		// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_HUE_U_5	0x280		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_SKIN_REGION_SAT_L_5	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_SAT_U_5	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_L_5 0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_SKIN_REGION_LUMA_U_5 0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_SKIN_HUE_GAIN_5		0x8			// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_SAT_GAIN_5		0x1			// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_SKIN_LUMA_GAIN_5		0x1			// Luma gain: 0 ~ 0xFF

// Outside	
#define REG_FCC_SKIN_YIN_SEL 					0 		// 0:Yin    1:Lin
#define REG_DH_FAVOR_SKIN_OUTSIDE			0		// S10: dH for favor color capture outside
#define REG_DS_FAVOR_SKIN_OUTSIDE			0x80       // 1.7: 128=1x, dS for favor color capture outside
#define REG_DY_FAVOR_SKIN_OUTSIDE			0		// S10: dY for favor color capture outside
#define REG_HSL_FCC_SKIN_OUTSIDE_Y_SEL		0            // 0: Outside FCC dY=reg_dY_favor_outside, 1: Outside FCC to Lin when reg_fcc_yin_sel=0

//REGION-1
#define REG_FAVOR_MOBA_REGION_HUE_L_1	0x0			// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_HUE_U_1	0x190		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_SAT_L_1	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_SAT_U_1	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_L_1 	0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_U_1 	0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_HUE_GAIN_1		0xFF		// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_SAT_GAIN_1		0xFF		// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_LUMA_GAIN_1		0xFF		// Luma gain: 0 ~ 0xFF

//REGION-2
#define REG_FAVOR_MOBA_REGION_HUE_L_2	0x4B0		// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_HUE_U_2	0x800		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_SAT_L_2	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_SAT_U_2	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_L_2 	0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_U_2 	0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_HUE_GAIN_2		0xFF		// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_SAT_GAIN_2		0xFF		// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_LUMA_GAIN_2		0xFF		// Luma gain: 0 ~ 0xFF

//REGION-3
#define REG_FAVOR_MOBA_REGION_HUE_L_3	0x0			// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_HUE_U_3	0x190		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_SAT_L_3	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_SAT_U_3	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_L_3 	0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_U_3 	0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_HUE_GAIN_3		0xFF		// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_SAT_GAIN_3		0xFF		// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_LUMA_GAIN_3		0xFF		// Luma gain: 0 ~ 0xFF

//REGION-4
#define REG_FAVOR_MOBA_REGION_HUE_L_4	0x0			// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_HUE_U_4	0x190		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_SAT_L_4	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_SAT_U_4	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_L_4 	0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_U_4 0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_HUE_GAIN_4		0xFF		// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_SAT_GAIN_4		0xFF		// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_LUMA_GAIN_4		0xFF		// Luma gain: 0 ~ 0xFF

//REGION-5
#define REG_FAVOR_MOBA_REGION_HUE_L_5	0x0			// Hue lower boubd: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_HUE_U_5	0x190		// Hue upper bound: 0~0x17FF
#define REG_FAVOR_MOBA_REGION_SAT_L_5	0x0			// Sat lower boubd: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_SAT_U_5	0xFFF		// Sat upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_L_5 	0x0			// Luma lower bound: 0~0xFFF
#define REG_FAVOR_MOBA_REGION_LUMA_U_5 	0xFFF		// Luma upper bound: 0~0xFFF
#define REG_FAVOR_MOBA_HUE_GAIN_5		0xFF		// Hue gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_SAT_GAIN_5		0xFF		// Sat gain: 0 ~ 0xFF
#define REG_FAVOR_MOBA_LUMA_GAIN_5		0xFF		// Luma gain: 0 ~ 0xFF

// Outside	
#define REG_FCC_MOBA_YIN_SEL 				0 		// 0:Yin    1:Lin
#define REG_DH_FAVOR_MOBA_OUTSIDE			0		// S10: dH for favor color capture outside
#define REG_DS_FAVOR_MOBA_OUTSIDE			0x00       // 1.7: 128=1x, dS for favor color capture outside//0:Sat = 0
#define REG_DY_FAVOR_MOBA_OUTSIDE			0		// S10: dY for favor color capture outside
#define REG_HSL_FCC_MOBA_OUTSIDE_Y_SEL		0            // 0: Outside FCC dY=reg_dY_favor_outside, 1: Outside FCC to Lin when reg_fcc_yin_sel=0


typedef enum
{
	FCC_SKIN_MODE,
	FCC_MOBA_MODE
} HSL_FCC_MODE_SEL;

#endif

typedef enum
{
    GEN_BY_HW,
    GEN_BY_FW
} HSL_COMPEN_GEN_SEL;

//typedef unsigned short WORD;
//typedef unsigned char  BYTE;

/*
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

#ifdef MT9701_LOAD_TABLE
const int Sat_global_Table1[9]	= {0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0, 0x100, 0x120};
const int Sat_global_max_Table1[9] = {0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0, 0x100, 0x120};
const int Sat_global_min_Table1[9] =  {0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0, 0x100, 0x120};
const int Global_Hue_by_Hue_Table1[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int Global_Luma_by_Hue_Table1[14] ={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int Global_Sat_by_Hue_Table1[14] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
const int	Global_Sat_by_Luma_Table1[9] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
const int	Global_Luma_by_MaxRGB_Table1[9] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40};
const int	Luma_gray_Table[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
*/

int gdH_UI[6], gdS_UI[6], gdY_UI[6];


#if WRITE_HSY_TABLE_BY_ADL
typedef struct _HSY_TABLE_ADL_FORMAT
{
#if 1
	MS_U8 hueByhue;			//  8 bit  [7:0]   	Depth: 36
	MS_U8 hueBysat_L;			// 8 bit   [7:0]  	Depth: 36*9
	MS_U8 hueBysat_H;			// 1 bit   [8]  		Depth: 36*9
	MS_U8 hueByluma_L;			// 8 bit   [7:0]      	Depth: 36*9
	MS_U8 hueByluma_H;			// 1 bit   [8]      	Depth: 36*9
	MS_U8 satByhue_global_L;	// 8 bit   [7:0]      	Depth: 36*9	
	MS_U8 satByhue_global_H;	// 1 bit   [8]      	Depth: 36*9	
	MS_U8 satByhue_max_L;		// 8 bit   [7:0]      	Depth: 36*9	
	MS_U8 satByhue_max_H;		// 1 bit   [8]      	Depth: 36*9	
	MS_U8 satByhue_min_L;		// 8 bit   [7:0]      	Depth: 36*9	
	MS_U8 satByhue_min_H;		// 1 bit   [8]      	Depth: 36*9	
	MS_U8 satByluma; 			// 8 bit   [7:0]      	Depth: 36*9
	MS_U8 lumaBysat_L; 			// 8 bit   [7:0]      	Depth: 36*9
	MS_U8 lumaBysat_H; 			// 1 bit   [8]      	Depth: 36*9
	MS_U8 lumaByluma_L;		// 8 bit   [8]      	Depth: 36*9
	MS_U8 lumaByluma_H;             // 2 bit  [1:0]    	Depth: 36*9
#else
	MS_U8 hueByhue;			//  8 bit  [7:0]   	Depth: 36
	MS_U16 hueBysat;				// 9 bit   [16:8]  	Depth: 36*9
	MS_U16 hueByluma;			// 9 bit   [32:24]      Depth: 36*9
	MS_U16 satByhue_global; 		// 9 bit   [48:40]      Depth: 36*9
	MS_U16 satByhue_max; 		// 9 bit   [64:56]      Depth: 36*9
	MS_U16 satByhue_min; 		// 9 bit   [80:72]      Depth: 36*9
	MS_U8 satByluma; 			// 8 bit   [95:88]      Depth: 36*9
	MS_U16 lumaBysat;			// 9 bit   [104:96]     Depth: 36*9
	MS_U16 lumaByluma;                // 10 bit  [121:112]    Depth: 36*9
#endif
	//MS_U8 dummy[16]; //dummy space for 256 bit mode
}S_HSY_TABLE_ADL_FORMAT;

extern S_HSY_TABLE_ADL_FORMAT hsy_table_adl[DEPTH_ADL_HSY_TABLE];
#endif

#if (CHIP_ID == MST9U5) //MT9701_LOAD_TABLE
typedef struct _HSL_REG_
{
    int dbg_dump_dbg_en;
    int dbg_target_h;
    int dbg_target_v;
    int reg_hsl_input_mode;
    int reg_hsl_output_mode;
    int reg_ycbcr_limit_range;
    int reg_y2r_11;
    int reg_y2r_21;
    int reg_y2r_23;
    int reg_y2r_33;
    int reg_hsl_RGB2YUV_dither_en;
    int reg_en_adv_pdn;
    int reg_hsl_rgb_mode;

    int reg_hsl_hue_en;
    int reg_hsl_hue_degree_initial;
    int reg_hsl_hue_by_sat_en;
    int reg_hsl_hue_by_lig_en;
    int reg_hsl_global_hue_delta_theta;

    int reg_hsl_sat_en;
    int reg_hsl_sat_pre_max_limit;
    int reg_hsl_sat_post_max_limit;
    int reg_hsl_global_sat_gain;
    int reg_hsl_global_sat_curve_en;

    int reg_hsl_lig_en;
    int reg_hsl_global_lig_offset;
    int reg_hsl_global_lig_gain;

    int reg_hsl_lig_gp_min_strength;
    int reg_hsl_lig_gp_thrd;
    int reg_hsl_lig_gp_slope;
    int reg_hsl_hue_gp_en;
    int reg_hsl_sat_gp_en;
    int reg_hsl_lig_gp_en;

    //-------add new design--------------
    int reg_hsl_sat_by_lig_en;
    int reg_hsl_sat_by_hue_en;
    int reg_hsl_lig_by_lig_en;
    int reg_hsl_lig_by_sat_en;
    int reg_hsl_hue_by_hue_en;
    int reg_hsl_hue_dither_en;
    int reg_hsl_sat_dither_en;
    int reg_hsl_luma_dither_en;
    int reg_hsl_SY2RGB_dither_en;
    int reg_hsl_H2RGB_dither_en;
    int reg_hsl_sat_increase_mode;
    int reg_hsl_sat_decrease_mode;
    int reg_hsl_sat_global_increase_mode;
    int reg_hsl_sat_global_decrease_mode;
    int reg_hsl_lig_global_gain_mode;
    int reg_hsl_delta_rgb_mode;
    int reg_hsl_hue_mode;
    int reg_hsl_uvangle_scale_0 ;
    int reg_hsl_uvangle_offset_0;
    int reg_hsl_uvangle_scale_1 ;
    int reg_hsl_uvangle_offset_1;
    int reg_hsl_uvangle_scale_2 ;
    int reg_hsl_uvangle_offset_2;
    int reg_hsl_sat_mode;
    int reg_hsl_uvnorm_scale;

    int reg_hsl_rgb_min;
    int reg_hsl_rgb_max;
    int reg_hsl_delta_y_zero_sel_en;

    int reg_hsl_sat_for_hue_process_sel;
    int reg_hsl_sat_for_sat_process_sel;
    int reg_hsl_sat_for_luma_process_sel;
    int reg_hsl_luma_for_hue_process_sel;
    int reg_hsl_luma_for_sat_process_sel;
    int reg_hsl_luma_for_luma_process_sel;
    int reg_hsl_mode;
    int reg_hsl_global_sat_by_luma_mode;
    int reg_hsl_global_sat_by_hue_en;
    int reg_hsl_global_sat_by_luma_en;
    int reg_hsl_global_hue_by_hue_en;
    int reg_hsl_global_luma_by_hue_en;
    int reg_hsl_global_luma_mode;

    int reg_hsl_global_contrast;
    int reg_hsl_global_brightness;

    int reg_hsl_hue_gray;
    int reg_hsl_sat_gray;
    int reg_hsl_luma_global_gain_min;

    int reg_hsl_debug_mode_en;
    int reg_hsl_input_for_debug_sel;
    int reg_hsl_debug_hue_start;
    int reg_hsl_debug_hue_end;
    int reg_hsl_debug_sat_start;
    int reg_hsl_debug_sat_end;
    int reg_hsl_debug_luma_start;
    int reg_hsl_debug_luma_end;
    int reg_hsl_debug_gray_sel;

    int reg_en_debug_pix;
    int reg_debug_pix_sel;

    int reg_hsl_mwe_enable;
    int reg_hsl_window_v_start;
    int reg_hsl_window_v_end;
    int reg_hsl_window_h_start;
    int reg_hsl_window_h_end;

    int reg_hsl_hue_rotate_alpha;
    int reg_uvrotate_t1;
    int reg_uvrotate_t2;
    int reg_uvrotate_t3;
    int reg_uvrotate_t4;
    int reg_uvrotate_t5;
    int reg_uvrotate_t6;
    //-----------------------------------

    int reg_hsl_hue_delta_amp; //2 bit
    int reg_hsl_sat_gain_by_delta_y_limit; //4 bit
    int reg_hsl_hue_lpf_en; //1 bit
    int reg_hsl_sat_lpf_en; //1 bit
    int reg_hsl_lig_lpf_en; //1 bit
    int reg_hsl_hue_lpf_min_strength; //6 bit
    int reg_hsl_sat_lpf_min_strength; //6 bit
    int reg_hsl_lig_lpf_min_strength; //6 bit
    int reg_hsl_hue_lpf_thrd; //10 bit
    int reg_hsl_sat_lpf_thrd; //10 bit
    int reg_hsl_lig_lpf_thrd; //10 bit
    int reg_hsl_hue_lpf_slope; //6 bit
    int reg_hsl_sat_lpf_slope; //6 bit
    int reg_hsl_lig_lpf_slope; //6 bit
    
    int reg_r2y_12; // 13, T0.10
    int reg_r2y_13; // 13, T0.10
    int reg_r2y_21; // 13, T0.10
    int reg_r2y_22; // 13, T0.10
    int reg_r2y_23; // 13, T0.10
    int reg_r2y_31; // 13, T0.10
    int reg_r2y_32; // 13, T0.10

    int reg_hsl_path_bypass;

// For auto download 
    int reg_adl_trig_wdth;
    int reg_en_adl;
    int reg_hsl_path_sel;
    int reg_sram_lut_rd_sel;
    int reg_adl_err_flag;
    int reg_adl_adr_sel;
}stHSL_REG;

#elif (CHIP_ID==CHIP_MT9701)
typedef struct _HSL_REG_
{
    int reg_y2r_11;
    int reg_y2r_21;
     int reg_y2r_23;
    int reg_y2r_33;
    int reg_fcc_yin_sel;
    int reg_pattern_r;
    int reg_pattern_g;
    int reg_pattern_b;
    int reg_pattern_gen_en;
    int YUV_full_random;

    int reg_hsl_hue_en;
    int reg_hsl_hue_degree_initial;
    int reg_hsl_global_hue_delta_theta;

    int reg_hsl_sat_en;
    int reg_hsl_sat_pre_max_limit;
    int reg_hsl_sat_post_max_limit;
    int reg_hsl_global_sat_en;
    int reg_hsl_sat_by_lig_en;
    int reg_hsl_lig_en;


    int reg_sat_global_blending_en;

    int reg_hsl_lig_gp_min_strength;
    int reg_hsl_lig_gp_thrd;
    int reg_hsl_lig_gp_slope;
    int reg_hsl_hue_gp_en;
    int reg_hsl_sat_gp_en;
    int reg_hsl_lig_gp_en;
    int reg_hsl_sy2rgb_dither_en;

    //-------add new design--------------
    int reg_hsl_lig_by_lig_en;
    int reg_hsl_hue_dither_en;
    int reg_hsl_sat_dither_en;
    int reg_hsl_luma_dither_en;
    int reg_hsl_uvangle_scale_0 ;
    int reg_hsl_uvangle_offset_0;
    int reg_hsl_uvangle_scale_1 ;
    int reg_hsl_uvangle_offset_1;
    int reg_hsl_uvangle_scale_2 ;
    int reg_hsl_uvangle_offset_2;
    int reg_hsl_uvnorm_scale;

    int reg_hsl_rgb_min;
    int reg_hsl_rgb_max;

    int reg_hsl_global_hue_by_hue_en;
    int reg_hsl_global_luma_by_hue_en;


    int reg_hsl_hue_gray;
    int reg_hsl_sat_gray;

    int reg_hsl_debug_hue_start;
    int reg_hsl_debug_hue_end;
    int reg_hsl_debug_sat_start;
    int reg_hsl_debug_sat_end;
    int reg_hsl_debug_luma_start;
    int reg_hsl_debug_luma_end;


    //-----------------------------------

    int reg_hsl_hue_delta_amp; // 2 bit

    int reg_r2y_12; // 13, T0.10
    int reg_r2y_13; // 13, T0.10
    int reg_r2y_21; // 13, T0.10
    int reg_r2y_22; // 13, T0.10
    int reg_r2y_23; // 13, T0.10
    int reg_r2y_31; // 13, T0.10
    int reg_r2y_32; // 13, T0.10

// --------- For HW ---------------
    int reg_hsl_path_bypass;


// -------------------------------
    int reg_favor_region;

    int reg_favor_region_1;
    int reg_favor_region_hueu_1;
    int reg_favor_region_huel_1;
    int reg_favor_region_satu_1;
    int reg_favor_region_satl_1;
    int reg_favor_region_lumau_1;
    int reg_favor_region_lumal_1;
    int reg_dh_favor_adjust_1;
    int reg_ds_favor_adjust_1;
    int reg_dy_favor_adjust_1;
    int reg_favor_hue_gain_1;
    int reg_favor_sat_gain_1;
    int reg_favor_luma_gain_1;

    int reg_favor_region_2;
    int reg_favor_region_hueu_2;
    int reg_favor_region_huel_2;
    int reg_favor_region_satu_2;
    int reg_favor_region_satl_2;
    int reg_favor_region_lumau_2;
    int reg_favor_region_lumal_2;
    int reg_dh_favor_adjust_2;
    int reg_ds_favor_adjust_2;
    int reg_dy_favor_adjust_2;
    int reg_favor_hue_gain_2;
    int reg_favor_sat_gain_2;
    int reg_favor_luma_gain_2;


    int reg_favor_region_3;
    int reg_favor_region_hueu_3;
    int reg_favor_region_huel_3;
    int reg_favor_region_satu_3;
    int reg_favor_region_satl_3;
    int reg_favor_region_lumau_3;
    int reg_favor_region_lumal_3;
    int reg_dh_favor_adjust_3;
    int reg_ds_favor_adjust_3;
    int reg_dy_favor_adjust_3;
    int reg_favor_hue_gain_3;
    int reg_favor_sat_gain_3;
    int reg_favor_luma_gain_3;

    int reg_favor_region_4;
    int reg_favor_region_hueu_4;
    int reg_favor_region_huel_4;
    int reg_favor_region_satu_4;
    int reg_favor_region_satl_4;
    int reg_favor_region_lumau_4;
    int reg_favor_region_lumal_4;
    int reg_dh_favor_adjust_4;
    int reg_ds_favor_adjust_4;
    int reg_dy_favor_adjust_4;
    int reg_favor_hue_gain_4;
    int reg_favor_sat_gain_4;
    int reg_favor_luma_gain_4;

    int reg_favor_region_5;
    int reg_favor_region_hueu_5;
    int reg_favor_region_huel_5;
    int reg_favor_region_satu_5;
    int reg_favor_region_satl_5;
    int reg_favor_region_lumau_5;
    int reg_favor_region_lumal_5;
    int reg_dh_favor_adjust_5;
    int reg_ds_favor_adjust_5;
    int reg_dy_favor_adjust_5;
    int reg_favor_hue_gain_5;
    int reg_favor_sat_gain_5;
    int reg_favor_luma_gain_5;
    int reg_dh_favor_outside;
    int reg_ds_favor_outside;
    int reg_dy_favor_outside;
    int reg_hsl_fcc_outside_y_sel;


    int reg_hsl_sat_by_hue_en;
    int Sat_by_Hue_Table_reg[9][24];
    int Sat_by_Luma_Table_reg[9];

    int reg_driver_monitor_en;
    int reg_driver_Monitor_Hue_Table[24];
    int reg_driver_Monitor_Sat_Table[24];
    int reg_driver_Monitor_Luma_Table[24];
    int reg_luma_gray_table[9];
    int reg_huephase_ref[24];
    int reg_satphase_ref[24];
    int reg_lumaphase_ref[24];

    int reg_hsl_uvnorm_max;     // 11 bit
    int reg_hue_ui_r;
    int reg_hue_ui_g;
    int reg_hue_ui_b;
    int reg_hue_ui_c;
    int reg_hue_ui_m;
    int reg_hue_ui_y;

    int reg_sat_ui_r;
    int reg_sat_ui_g;
    int reg_sat_ui_b;
    int reg_sat_ui_c;
    int reg_sat_ui_m;
    int reg_sat_ui_y;

    int reg_luma_ui_r;
    int reg_luma_ui_g;
    int reg_luma_ui_b;
    int reg_luma_ui_c;
    int reg_luma_ui_m;
    int reg_luma_ui_y;

    int reg_hue_alpha1;
    int reg_hue_alpha2;
    int reg_sat_alpha1;
    int reg_sat_alpha2;
    int reg_luma_alpha1;
    int reg_luma_alpha2;

    int reg_en_debug_pix;
    int reg_cursor_x;
    int reg_cursor_y;
    int reg_debug_pix_sel;
    int reg_hsl_input_for_debug_sel;
    int reg_debug_pix0;
    int reg_debug_pix1;
    int reg_debug_pix2;
    int reg_en_cursor_display;
    int reg_cursor_len;
    int reg_cursor_data0;
    int reg_cursor_data1;
    int reg_cursor_data2;
	int reg_de_sel;

    int reg_hsl_mwe_enable;
    int reg_hsl_window_h_start;
    int reg_hsl_window_h_end;
    int reg_hsl_window_v_start;
    int reg_hsl_window_v_end;


    int reg_compensate_start;
    int reg_ui_protect_en;
    int reg_compensate_fire;

    int reg_hsl_sat_linear_blend;
    int reg_hsl_luma_linear_blend;

    int reg_hsl_invuvin_base_hout;
    int reg_hsl_l_y_blend_alpha;

}stHSL_REG;
#endif


// Init internal tables.
#if CHIP_ID==CHIP_MT9701
void INIT_TABLE_GENERATOR(int* dHt, int* dSt, int*dYt, int* dSt_h, int* dYt_h, int* dSt_s, int* dYt_s, int* dYt_y);
#else
void INIT_TABLE_GENERATOR(int* dHt, int* dSt, int*dYt, int* dSt_h, int* dYt_h, int* dSt_s, int* dYt_s, int* dYt_y, int* dHt_MT9701, int* dSt_MT9701, int* dYt_MT9701);
#endif

// Functions of table generator.
void USER_SAT_LUMA_BLEND(int color, int* dS_UI, int *dY_UI, int* dS_UI_24, int* dY_UI_24);
#if MT9701_V_MODE
void HUE_UI(int color, int* dH_UI, int* dHt);
void SAT_LUMA_UI(int color, int* dHt, int* dS_UI_24, int *dY_UI_24, int* dSt, int* dYt);
#else
void HUE_UI(int color, int* dH_UI, int* dHt, int* dSt_h, int* dYt_h);
void SAT_LUMA_UI(int color, int* dHt, int* dS_UI_24, int *dY_UI_24, int* dSt_h, int* dYt_h, int* dSt, int* dYt);
#endif

#if MT9701_V_MODE
void HUE_COMPENSATE(int idx, int dH, int* dHt);
void SAT_LUMA_COMPENSATE(int idx, int dH, int* dS_UI_24, int* dY_UI_24, int* dSt, int* dYt);
#else
void HUE_COMPENSATE(int idx, int dH, int* dHt, int* dSt_h, int* dYt_h);
void SAT_LUMA_COMPENSATE(int idx, int dH, int* dS_UI_24, int* dY_UI_24, int* dSt_h, int* dYt_h, int* dSt, int* dYt);
#endif

// Interpolate 24-axis to 36-axis for MT9701
void INTERPOLATE_24_TO_36_AXIS(int* dHt, int* dSt, int* dYt, int *dHt_MT9701, int* dSt_MT9701, int* dYt_MT9701);

#if WRITE_HSY_TABLE_BY_ADL
void MT9701_PREPARE_ADL_HSY_TABLE(S_HSY_TABLE_ADL_FORMAT *HSY_table_adl, int *dHt_MT9701,  int* dSt_MT9701, int* dYt_MT9701);
#endif
void MT9701_HUE_TABLE_WRITE(BYTE u8WinIdx, int *dHt_MT9701);
void MT9701_SAT_TABLE_WRITE(BYTE u8WinIdx, int* dSt_MT9701);
void MT9701_LUMA_TABLE_WRITE(BYTE u8WinIdx, int* dYt_MT9701);

// MT9701 main function for table generator.
void driver_table_MT9701(BYTE u8WinIdx, int *dH_UI, int *dS_UI, int *dY_UI, bool use_adl);

#if (CHIP_ID==CHIP_MT9701)
// MST9U6 main function for table generator. 
void CHIP_MT9701_DRIVER_TABLE(BYTE u8WinIdx, int *dH_UI, int *dS_UI, int *dY_UI);
void CHIP_MT9701_HW_TABLE(BYTE u8WinIdx, int *dH_UI, int *dS_UI, int *dY_UI);
// MST9U6 HW registers control 
void CHIP_MT9701_DRIVER_TABLE_EN(BYTE u8WinIdx, Bool dri_enable);
void CHIP_MT9701_HW_EN_COMPENSATION(BYTE u8WinIdx);
void CHIP_MT9701_HW_SET_OFFSET(BYTE u8WinIdx, int *dH_UI, int *dS_UI, int *dY_UI);
void CHIP_MT9701_HW_SET_INTERP_ALPHA(BYTE u8WinIdx);
// Write HSY table into HW via RIU.
void CHIP_MT9701_HUE_TABLE_WRITE(BYTE u8WinIdx, int *dHt);
void CHIP_MT9701_SAT_TABLE_WRITE(BYTE u8WinIdx, int* dSt);
void CHIP_MT9701_LUMA_TABLE_WRITE(BYTE u8WinIdx, int* dYt);

#if(ENABLE_MSTV_UART_DEBUG && DEBUG_MODE)
// For Debug
void CHIP_MT9701_DEBUG_HUE_TABLE(BYTE u8WinIdx, HSL_COMPEN_GEN_SEL tablegen_sel);
void CHIP_MT9701_DEBUG_SAT_TABLE(BYTE u8WinIdx, HSL_COMPEN_GEN_SEL tablegen_sel);
void CHIP_MT9701_DEBUG_LUMA_TABLE(BYTE u8WinIdx, HSL_COMPEN_GEN_SEL tablegen_sel);
#endif
#endif

// Init HW registers and write to HW.
#if (CHIP_ID == MST9U5) //MT9701_LOAD_TABLE
void ColorEngine_REG_initial(BYTE u8WinIdx,stHSL_REG *HSL_REG_MAIN, stHSL_REG *HSL_REG_SUB);
void	msAPI_LOAD_HSY_INIT_TABLE(BYTE u8WinIdx, stHSL_REG *HSL_REG_MAIN, stHSL_REG *HSL_REG_SUB);
void ColorEngine_REG_Update(BYTE u8WinIdx,stHSL_REG *HSL_REG_MAIN, stHSL_REG *HSL_REG_SUB);
void msAPI_LOAD_HSY_UPDATE_REG(BYTE u8WinIdx, stHSL_REG *HSL_REG_MAIN, stHSL_REG *HSL_REG_SUB);

#else
void ColorEngine_REG_INIT(stHSL_REG *HSL_REG_MAIN);
void	msAPI_LOAD_HSY_INIT_TABLE(BYTE u8WinIdx, stHSL_REG *HSL_REG_MAIN);
void msAPI_HSY_SkinToneTable(stHSL_REG *HSL_REG_MAIN, BYTE u8SkinToneIdx);
void msAPI_LOAD_HSY_SKINTONE_TABLE(BYTE u8WinIdx, stHSL_REG *HSL_REG_MAIN);
void msAPI_HSY_AdjustSkinTone(BYTE u8SkinToneIdx);
#endif

// msAPI functions
void msASPI_AdjustCustomInit(int *dH_UI, int *dS_UI, int *dY_UI);
#if (CHIP_ID == CHIP_MT9701)
void msAPI_HSY_AdjustCustomHue(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, HSL_COMPEN_GEN_SEL tablegen_sel );
void msAPI_HSY_AdjustCustomSat(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, HSL_COMPEN_GEN_SEL tablegen_sel);
void msAPI_HSY_AdjustCustomLuma(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, HSL_COMPEN_GEN_SEL tablegen_sel );
#else
void msAPI_HSY_AdjustCustomHue(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, bool use_adl);
void msAPI_HSY_AdjustCustomSat(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, bool use_adl);
void msAPI_HSY_AdjustCustomLuma(BYTE u8WinIdx, int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI, bool use_adl);
#endif
BYTE RemappingHSYIndex(BYTE u8Index);
/*
//#if(ENABLE_MSTV_UART_DEBUG && DEBUG_MODE)
void MT9701_HUE_TABLE_READ(BYTE u8WinIdx, int *dbg_hue_by_hue_table, int *dbg_hue_by_sat_table, int *dbg_hue_by_lig_table);
void MT9701_SAT_TABLE_READ(BYTE u8WinIdx, int *dbg_sat_by_hue_table, int *dbg_sat_by_lig_table, int *dbg_sat_max_table, int *dbg_sat_min_table);
void MT9701_LUMA_TABLE_READ(BYTE u8WinIdx, int *dbg_lig_by_sat_table, int *dbg_lig_by_lig_table);
//#endif
*/
#if (CHIP_ID == CHIP_MT9701)
extern void msAPI_Adjust_Hue(BYTE u8Index, int Offset);
extern void msAPI_Adjust_Sat(BYTE u8Index, int Offset );
extern void msAPI_Adjust_Bri(BYTE u8Index, int Offset );
extern void msAPI_HSY_inital(HSL_COMPEN_GEN_SEL tablegen_sel);
extern void msAPI_HSY_Reset(HSL_COMPEN_GEN_SEL tablegen_sel);
#else
extern void msAPI_AdjustHSY_Hue(BYTE u8WinIdx,BYTE u8Index, int Offset, bool use_adl);
extern void msAPI_AdjustHSY_Sat(BYTE u8WinIdx,BYTE u8Index, int Offset, bool use_adl );
extern void msAPI_AdjustHSY_Luma(BYTE u8WinIdx,BYTE u8Index, int Offset, bool use_adl );
extern void msAPI_HSY_inital(void);
extern void msAPI_HSY_Reset(void);
#endif
#if HSY_TEST_MODE
extern void msAPI_test(BYTE EnDebug, BYTE u8select);
#endif
extern void msAPI_Set_SAT_ALPHA(BYTE ucPara1, BYTE ucPara2);
extern void msAPI_Set_LUM_ALPHA(BYTE ucPara1, BYTE ucPara2);

#if FCC_MODE_EN && (CHIP_ID == CHIP_MT9701)
void MST9U6_FCC_MODE_ENABLE(BYTE u8WinIdx);
void MST9U6_FCC_MODE_DISABLE(BYTE u8WinIdx);

void MST9U6_FCC_REGION1_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset,  int sat_offset, int luma_offset);
void MST9U6_FCC_REGION2_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset,  int sat_offset, int luma_offset);
void MST9U6_FCC_REGION3_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset,  int sat_offset, int luma_offset);
void MST9U6_FCC_REGION4_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset,  int sat_offset, int luma_offset);
void MST9U6_FCC_REGION5_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode, BOOL region_enable, int hue_offset,  int sat_offset, int luma_offset);
void MST9U6_FCC_OUTSIDE_ADJUST(BYTE u8WinIdx, HSL_FCC_MODE_SEL mode);


void MST9U6_FCC_SKIN_TEST(void);
void MST9U6_FCC_MOBA_TEST(void);
#endif

#endif
