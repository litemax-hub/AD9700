#ifndef _DDCCOLORHANDLER_H
#define _DDCCOLORHANDLER_H

#include "drvDeltaE.h"

#define MStar_COMMAND               0xCC
#define MS_EN_TEST_PATTERN_WITHOUTSIGNAL       0x81
#if !ENABLE_DeltaE
#define MS_AutoGamma_OnOff          0x30
#define MS_AutoGamma_SetBGColor     0x31
#endif
#define MS_EN_INTERNAL_PATTERN      0x30
#define MS_SET_INTERNAL_PATTERN     0x31
#define MS_EN_COLOR_ENGINE_PATTERN  0x34
#define MS_SET_COLOR_ENGINE_PATTERN 0x35
#define MS_GET_MODEL_NAME           0x36
#define MS_BRIGHTNESS               0x37
#define MS_CheckDDC                 0x38
#define MS_SET_Y2RCM_TEMP_DISABLE   0x3B
#define MS_SET_CSCCM_TEMP_DISABLE   0x43
#define MS_SET_HDR_COLOR_ENGINE_PATTERN     0x3C
#define MS_EN_HDR					0x29
#define MS_EN_XPercentPIP_PATTERN	0x2E
#define MS_SET_XPercentPIP_PATTERN	0x2F
#define MS_SET_OSDBrightness    0x3E
#define MS_GET_MonitorSN            0x3F
#define MS_ColorEngine_OnOff   	    0x40
#define MS_Read_Status              0x8F
#define MS_Read_ACK                 0x8E
#define MS_Read_LastCmd             0x8D
#define MS_R2Reset51Flag		    0x89
#define MS_WR_BLOCK                 0x90
#define MS_WFlash_Test				0x91
#define MS_DDCWriteReg      	    0xA7
#define MS_DDCReadReg               0xA8
#define MS_GetToolFlagStatus        0xA9
#define MS_GetCurrentPatternOutput				0xAC
#define MS_GetCommonVersion         0x3A
//for Local dimming
#define MS_Set_LD_01Region_MaxBri   0xD0
#define MS_Set_LD_01Region_DynaBri  0xD1
#define MS_Set_LD_Gamma_Adjustment  0xD2
#define MS_MSBHK_Bypass             0xD3

#define MS_RunISP                   0xFA
#define MS_RUN_REGMAP_ISP           0xFF
#define MS_GetMemAddr               0xF0

#define MS_ISP_CHECK_CRC            0xF1
#define MS_ISP_SET_RECVINFO         0xF2
#define MS_ISP_GET_RECVINFO         0xF3
#define MS_ISP_SETDATA   		    0xF4
#define MS_ISP_GET_SENTINFO         0xF5
#define MS_ISP_SET_SENTINFO         0xF6
#define MS_ISP_RUN                  0xF7
#define MS_ISP_GET_STATUS           0xF8
#define MS_ISP_GET_ACT_IMG_NUM      0xF9
#define MS_ISP_ACTIVATE_IMG_ONE     0xFA
#define MS_ISP_REBOOT               0xFB
#define MS_ISP_RESTORE_IMG_ONE      0xFC
#define MS_ISP_GET_IMG_ONE_CHECKSUM 0xFD
#define MS_ISP_GET_ALIVE            0xFE	
#define MS_ISP_GET_INFO            0xFF	

typedef enum
{
    PTN_IP2,
    PTN_XVYCC,
    PTN_INTERNAL,
} PatternType;

extern BYTE ColorCalibrationHandler(BYTE u8WinIdx);
extern BYTE CommonHandler(BYTE u8WinIdx);
extern void SetFrameColorRGB(BYTE u8Red, BYTE u8Green, BYTE u8Blue);
extern void msEnableColorEngineTestPattern(Bool bEnable);
//extern void msSetColorEngineTestPattern(BYTE u8Red, BYTE u8Green, BYTE u8Blue);

#if !ENABLE_WINISP
void WinISPDummy(void);
#endif
#endif


