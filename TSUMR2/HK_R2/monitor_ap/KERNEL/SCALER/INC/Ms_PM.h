#if (MS_PM)

#ifndef MS_PM_H
#define MS_PM_H

#ifdef _MS_PM_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if CHIP_ID == CHIP_MT9700
#include "MT9700_DEMO_PMSET.h"
#elif CHIP_ID == CHIP_MT9701
#include "MT9701_DEMO_PMSET.h"
#else
#warning "please implement PMSET for new chip/board"
#endif

typedef enum
{
    ePM_POWERON,
    ePM_STANDBY,
    ePM_POWEROFF,
    ePM_POWEROFF_0W,
    ePM_INVAILD
}ePM_Mode;

typedef enum
{
    ePMSTS_NON,
    ePMSTS_VGA_ACT,
#if( PM_SUPPORT_WAKEUP_DVI )
    ePMSTS_DVI_0_ACT,
    ePMSTS_DVI_1_ACT,
    ePMSTS_DVI_2_ACT,
    ePMSTS_DVI_3_ACT,
#endif
    ePMSTS_SAR_ACT,
    ePMSTS_GPIO_ACT,
    ePMSTS_MCCS04_ACT,
    ePMSTS_MCCS05_ACT,
    ePMSTS_MCCS01_ACT,
    ePMSTS_CEC_ACT,
    ePMSTS_FORCE_ON,
#if( PM_SUPPORT_WAKEUP_DP )
    ePMSTS_DP_ACT,
#endif
#if (PM_POWERkEY_GETVALUE)
    ePMSTS_POWERGPIO_ACT,
#endif
#if (PM_CABLE_DETECT_USE_SAR)
    ePMSTS_CABLESAR_ACT,
#endif
#if (ENABLE_USB_TYPEC)
    ePMSTS_TYPECAUTO_CHG,
#if(CHIP_ID == CHIP_MT9701)
    ePMSTS_U3STATE_CHG,
#endif
#endif
    ePMSTS_INVAID
}ePMStatus;

typedef enum
{
    ePMGPIO04_DectDVI5V,
    ePMGPIO02_DectDVI5V,
    ePMGPIO01_DectDVI5V,
    ePMGPIO00_DectDVI5V,
    ePMDVI5V_INVALID
}ePM_DVI5V;

typedef enum
{
    ePMSAR_SAR0 = BIT0,
    ePMSAR_SAR1 = BIT1,
    ePMSAR_SAR2 = BIT2,
    ePMSAR_SAR3 = BIT3,
    ePMSAR_SAR12 = BIT1|BIT2,
    ePMSAR_SAR123 = BIT1|BIT2|BIT3,
    ePMSAR_SAR01 = BIT0|BIT1,
    ePMSAR_SAR012 = BIT0|BIT1|BIT2,
    ePMSAR_INVALID
}ePM_SAR;

typedef enum
{
    ePMTYPEC_OFF, // PD/DP_alt/USB3 OFF, CorePower OFF
    ePMTYPEC_ON,  // PD/DP_alt/USB3 ON, CorePower OFF
    ePMTYPEC_AUTO,// PD/DP_alt/USB3 ON, CorePower OFF, same as ePMTYPEC_ON because CorePower and USB3 power switches are individual
    ePMTYPEC_PDON,// PD/DP_alt ON, USB3 OFF, CorePower OFF
}ePM_TYPEC;

typedef enum
{
    ePM_ENTER_PM = 0,
    ePM_EXIT_PM = 1,
    ePM_WAIT_EVENT = 2,
    ePM_IDLE = 3
}ePM_State;

typedef struct
{
    BYTE bHVSync_enable:1;
    BYTE bSOG_enable:1;
    BYTE bGPIO_enable:1;
    BYTE bSAR_enable:1;
    BYTE bMCCS_enable:1;
    BYTE bEDID_enable:1;
    BYTE bCEC_enable:1;
#if (PM_SUPPORT_WAKEUP_DVI)
    BYTE bDVI_enable:1;
#endif
#if 1 //(PM_SUPPORT_WAKEUP_DP)&&(ENABLE_DP_INPUT)
    BYTE bDP_enable:1;
#endif
    BYTE bMCUSleep:1;
    ePM_SAR ePMSARmode;
    BYTE bTYPEC_enable:1; // config by driver
    ePM_TYPEC eTYPECmode; // config PM TypeC mode: OFF/ON/AUTO/PDON
}sPM_Config;

typedef struct
{
    ePM_Mode ucPMMode;
    ePM_State ePMState;
    sPM_Config  sPMConfig;
#if CABLE_DETECT_VGA_USE_SAR||CABLE_DETECT_VGA_USE_SAR
    BYTE bCABLE_SAR_VALUE;
#endif
}sPM_Info;

typedef enum
{
    ePM_CLK_RCOSC,
    ePM_CLK_XTAL,
    ePMCLK_INVALID
}ePM_CLK;

typedef struct
{
  BYTE u8SAR_KeyMask;
  BYTE u8SAR_CmpLvl;
}sPM_SARDetect;

typedef enum // need sync with dpcommon DPRX_MCCS_WAKEUP
{
    ePM_MCCS_D1 = 0x0,
    ePM_MCCS_D4 = 0x1,
    ePM_MCCS_D5 = 0x2,
    ePM_MCCS_MAX
}ePM_MCCS_WAKEUP;

//**************************************************************************
//  RCOSC = XTAL * Counter / 512 => Counter = RCOSC *512/XTAL,
//  IF RCOSC=4M, Counter=143 =>8Fh
//  CHIP_TSUMC uses FRO_12M /3 =4M to calibration ==> Counter=143
//**************************************************************************
#define RCOSC_TARGET    0x800 //12MHz

#if PM_CLOCK == RCOSC
#define PM_DELAY4US_LOOP        1
#elif PM_CLOCK == XTAL
#define PM_DELAY4US_LOOP        4
#else
#define PM_DELAY4US_LOOP        1
#endif

INTERFACE Bool msPM_IsState_IDLE(void);
INTERFACE void msPM_Init(void);
INTERFACE void msPM_SetFlag_Standby(void);
INTERFACE void msPM_SetFlag_PMDCoff(void);
INTERFACE void msPM_Handler(void);
INTERFACE Bool msPM_StartRCOSCCal(void);
INTERFACE void msPM_Exit(void);
INTERFACE BYTE msPM_GetActiveDPPort(BYTE u8InputPortl);
INTERFACE Bool msPM_0WStatus(void);

#if(CHIP_ID == CHIP_MT9700)
INTERFACE void msPM_SlowClkDetEnable(BOOL bEnable, BYTE u8SlowClkDiv);
#else
INTERFACE void msPM_SlowClkDetEnable(BOOL bEnable, BYTE u8SlowClkDiv, BYTE u8SlowClkDivPD51);
#endif

INTERFACE void msPM_SlowClkDetForceDisable(BOOL bDisable);

#undef INTERFACE

#endif

#endif
