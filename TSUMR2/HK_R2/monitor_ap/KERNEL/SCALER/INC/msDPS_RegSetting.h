//===================================================================================================================================================================:
#ifndef _MSDPSREG_H_
#define _MSDPSREG_H_
//===================================================================================================================================================================:

#define REG_ADDR_MWE_ENABLE                 SC00_5C
#define REG_ADDR_INTSTA                     SC00_CC
#define REG_ADDR_INTENB                     SC00_CE
#define REG_ADDR_HISTOGRAM_Acceler_ENABLE   SC07_B5
#define REG_ADDR_HSLU_AP                    SC00_B9
#define REG_ADDR_HISTOGRAM_DATA_32          SC08_C0
#define REG_ADDR_MAIN_CSC_ENABLE            SC07_40
#define REG_ADDR_MAIN_COLOR_MATRIX_ENABLE   SC08_81
#define REG_ADDR_COLOR_MATRIX_Main          0//SC8_82
#define REG_ADDR_COLOR_MATRIX_Sub           0//SC8_6E
#define REG_ADDR_HISTOGRAM_RANGE_H_ENABLE   SC07_27 // [7]&[6]
#define REG_ADDR_HISTOGRAM_RANGE_M_HST      SC07_24 // Low Byte
#define REG_ADDR_HISTOGRAM_RANGE_M_HEN      SC07_26 // Low Byte
#define REG_ADDR_HISTOGRAM_RANGE_V_ENABLE   SC07_B5 // [0]
#define REG_ADDR_HISTOGRAM_RANGE_M_VST      SC07_B6 // Low Byte, High Byte in B9[0]
#define REG_ADDR_HISTOGRAM_RANGE_M_VEN      SC07_B7 // Low Byte, High Byte in B9[2]
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_L      SC07_36
#define REG_ADDR_HISTOGRAM_TOTAL_SUM_H      SC07_37


#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_L    //removed
#define REG_ADDR_HISTOGRAM_TOTAL_COUNT_H

#define REG_ADDR_MAIN_MIN_VALUE             SC07_39
#define REG_ADDR_MAIN_MAX_VALUE             SC07_3A
#define REG_ADDR_DLC_HANDSHAKE              SC07_B8
#define REG_ADDR_MAIN_ICC_DITHER_EN         SC14_02 //[2]

#define REG_ADDR_MAIN_NOISEMASK_DITHER_EN    //removed
#define REG_ADDR_HIST_BF_PK_EN          SC07_B8  //Histogram source before/after peaking. 1: Before peaking. 0: After peaking.

//===================================================================================================================================================================:
#endif
//===================================================================================================================================================================:
