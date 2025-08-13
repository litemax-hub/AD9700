#ifndef _DRI_TABLE_GEN_H
#define _DRI_TABLE_GEN_H


#define HSY_MODE		(((GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY) || (INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY)) ? 1 : 0)
#define ENABLE_ADL                      0

// math library 
#define min( a , b )     ( (a) < (b) ? (a) : (b) )
#define max( a , b )     ( (a) > (b) ? (a) : (b) )
#define min_abs( a , b ) ( abs(a) < abs(b) ? (a) : (b) )
#define max_abs( a , b ) ( abs(a) > abs(b) ? (a) : (b) )
#define min3(a,b,c)      min( a, min( b, c ) )
#define max3(a,b,c)      max( a, max( b, c ) )
#define med3(a,b,c)      ( a + b + c - max3( a, b, c ) - min3( a, b, c ) )
#define minmax(v,a,b)    (((v)<(a))? (a) : ((v)>(b)) ? (b) : (v))


#define MT9700_NUM_REF_AXIS	24

// API INPUT REGISTER SETTING

#define	REG_HSL_UVNORM_MAX	512     // 724 // 11 bit, when REG_HSL_UVNORM_MAX changes. It need to change "REG_HSL_UVNORM_SCALE"
#define 	REG_HSL_UVNORM_SCALE	    1024 	//724 	// when REG_HSL_UVNORM_MAX = 512, REG_HSL_UVNORM_SCALE will be 1024; when REG_HSL_UVNORM_MAX = 724, REG_HSL_UVNORM_SCALE = 724


#define	REG_HUE_ALPHA1	16; 	//HSLReg.reg_hue_alpha1
#define	REG_HUE_ALPHA2	32; 	//HSLReg.reg_hue_alpha2;
#define	REG_SAT_ALPHA1	  0; 	//HSLReg.reg_sat_alpha1;
#define	REG_SAT_ALPHA2	32;	//HSLReg.reg_sat_alpha2;
#define	REG_LUMA_ALPHA1	32; 	//HSLReg.reg_luma_alpha1;
#define	REG_LUMA_ALPHA2	  0;  //HSLReg.reg_luma_alpha2;	


 
 


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


void INIT_TABLE_GENERATOR(int* dHt, int* dSt, int*dYt, int* dSt_h, int* dYt_h, int* dSt_s, int* dYt_s, int* dYt_y);

void USER_SAT_LUMA_BLEND(int color, int* dS_UI, int *dY_UI, int* dS_UI_24, int* dY_UI_24);

void HUE_UI(int color, int* dH_UI, int* dHt, int* dSt_h, int* dYt_h);
void SAT_LUMA_UI(int color, int* dHt, int* dS_UI_24, int *dY_UI_24, int* dSt_h, int* dYt_h, int* dSt, int* dYt);


void HUE_COMPENSATE(int idx, int dH, int* dHt, int* dSt_h, int* dYt_h);
void SAT_LUMA_COMPENSATE(int idx, int dH, int* dS_UI_24, int* dY_UI_24, int* dSt_h, int* dYt_h, int* dSt, int* dYt);

void driver_table_MT9700(int *dH_UI, int *dS_UI, int *dY_UI);


void msASPI_AdjustCustomInit(int *dH_UI, int *dS_UI, int *dY_UI);
void msAPI_AdjustCustomHue(int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI);
void msAPI_AdjustCustomSat(int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI);
void msAPI_AdjustCustomLuma(int s16offset, BYTE u8ColorIndex, int *dH_UI, int *dS_UI, int *dY_UI);

#ifdef  HSY_MODE
extern void msAPI_Adjust_test(void);
extern void msAPI_Adjust_ICC_Init(void);
extern void msAPI_Adjust_ICC_Value_Reset(void);
#endif
extern void msAPI_Adjust_Hue(BYTE u8Index, int Offset );
extern void msAPI_Adjust_Sat(BYTE u8Index, WORD u16Offset  );
extern void msAPI_Adjust_Bri(BYTE u8Index, int u16Offset );
extern BYTE RemappingHSYIndex(BYTE u8Index);

#endif
