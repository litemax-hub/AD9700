#ifndef _DEFAULT_COMPILER_OPTION_H_
#define _DEFAULT_COMPILER_OPTION_H_

#include "Panel.h"

////////////////////////////////////////////////////////////
// SI
////////////////////////////////////////////////////////////
#ifndef ODM_NAME
#define ODM_NAME                                ODM_AOC
#endif

#ifndef FACTORYALIGN_TYPE
#define FACTORYALIGN_TYPE                       FACTORYALIGN_DDCCI
#endif

////////////////////////////////////////////////////////////
// APPLICATION
////////////////////////////////////////////////////////////
#ifndef TTS
#define TTS                 0
#endif

#ifndef DV
#define DV                         0
#endif

#ifndef ENABLE_ARC
#define ENABLE_ARC          0
#endif

////////////////////////////////////////////////////////////
// MCU
////////////////////////////////////////////////////////////

#ifndef UART1
#define UART1               0
#endif

#ifndef ENABLE_PIU_UART_ResetCheck   //mcu_if  6.11
#define ENABLE_PIU_UART_ResetCheck        0
#endif

#ifndef ENABLE_PQ_R2
#define ENABLE_PQ_R2        0
#endif

/////////////////////////////////////////////////////////////
// ADC
/////////////////////////////////////////////////////////////
#ifndef ENABLE_ADC_RESET
#define ENABLE_ADC_RESET    1 // 120413 coding addition
#endif

#ifndef ADC_HSYNC_LVL_DEF // each main board may have different setting
#define ADC_HSYNC_LVL_DEF   ADC_HSYNC_LVL_5 // 120725 coding, default set to 5 to compatible with old chips
#endif

#ifndef ENABLE_ADC_DITHERING // ADC dither to fix horizontal/Vertical line noise
#define ENABLE_ADC_DITHERING    1
#endif

////////////////////////////////////////////////////////////
// SCALAR
////////////////////////////////////////////////////////////
#ifndef ENABLE_LBCLK_FIXED_216M
#define ENABLE_LBCLK_FIXED_216M 1
#endif

////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
#ifndef MS_VGA_SOG_EN
#define MS_VGA_SOG_EN               0
#endif

#ifndef DVI_RB_SWAP
#define DVI_RB_SWAP                 0
#endif

#ifndef DVI_PN_SWAP
#define DVI_PN_SWAP                 0
#endif

#ifndef HDMI_RB_SWAP
#define HDMI_RB_SWAP                0
#endif

#ifndef HDMI_PN_SWAP
#define HDMI_PN_SWAP                0
#endif

#ifndef ENABLE_DP_CTS_TEST
#define ENABLE_DP_CTS_TEST          0
#endif

#ifndef DISPLAY_UNDERSCAN_SPACE
#define DISPLAY_UNDERSCAN_SPACE     0// unit: pixel
#endif

#ifndef DISPLAY_UNDERSCAN_FREQ
#define DISPLAY_UNDERSCAN_FREQ      480 // 48Hz
#endif

#ifndef ENABLE_AUTOEQ
#define ENABLE_AUTOEQ               0
#endif

#ifndef ENABLE_MENULOAD
#define ENABLE_MENULOAD             0
#endif

#ifndef ENABLE_MENULOAD_2
#define ENABLE_MENULOAD_2           (0 && ENABLE_MENULOAD && (!ENABLE_PQ_R2))
#endif

#ifndef MENULOAD_TRIG
#define MENULOAD_TRIG               OP_TRIG
#endif

#ifndef MENULOAD_2_TRIG
#define MENULOAD_2_TRIG             IP_TRIG
#endif

#ifndef ENABLE_DYNAMICSCALING
#define ENABLE_DYNAMICSCALING       0
#endif

#ifndef ENABLE_RTE
#define ENABLE_RTE                  0
#endif

#ifndef ENABLE_OD_AutoDownTBL
#define ENABLE_OD_AutoDownTBL       0
#endif

#ifndef ENABLE_OD_VRR
#define ENABLE_OD_VRR               0
#endif

#ifndef ENABLE_OD_MTG
#define ENABLE_OD_MTG               0
#endif

#ifndef  DP_AUX_PortB_PNSWAP
#define DP_AUX_PortB_PNSWAP         0
#endif


#ifndef  DP_AUX_PortC_PNSWAP
#define DP_AUX_PortC_PNSWAP         0
#endif

#ifndef Enable_Multi_OD
#define Enable_Multi_OD             0
#endif

#ifndef USE_VCTRL
#define USE_VCTRL                   0
#endif

#ifndef USE_EXTERNAL_LDO
#define USE_EXTERNAL_LDO            0
#endif

#ifndef DHDMI_SIMPLAYHD_PATCH
#define DHDMI_SIMPLAYHD_PATCH       0
#endif

#ifndef ENABLE_DMA_MIIC
#define ENABLE_DMA_MIIC             0
#endif

////////////////////////////////////////////////////////////
// AUTO FUNC.
////////////////////////////////////////////////////////////
#ifndef ENABLE_HWAUTO_ADCOFFSET
#define ENABLE_HWAUTO_ADCOFFSET     1
#endif

#ifndef DISABLE_AUTO_SWITCH
#define DISABLE_AUTO_SWITCH         0
#endif

#ifndef ENABLE_SPEEDUP_SWITCH
#define ENABLE_SPEEDUP_SWITCH       0
#endif

////////////////////////////////////////////////////////////
// POWERMANAGEMENT
////////////////////////////////////////////////////////////
#ifndef MS_PM
#define MS_PM                       0
#endif

#ifndef PM_StbyMcuAliveForClockExist
#define PM_StbyMcuAliveForClockExist    0
#endif

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#ifndef HDCPKEY_USE_CODE
#define HDCPKEY_USE_CODE            1
#endif

#ifndef _NEW_SOG_DET_
#define _NEW_SOG_DET_               0
#endif

#ifndef ENABLE_WATCH_DOG
#define ENABLE_WATCH_DOG            0
#endif

#ifndef ENABLE_WATCH_DOG_INT
#define ENABLE_WATCH_DOG_INT        0
#endif

#ifndef LGE_DEMO
#define LGE_DEMO  0
#endif

#ifndef ENABLE_FLASH_CURRENT_ADJUSTMENT
#define ENABLE_FLASH_CURRENT_ADJUSTMENT   0 // Set spi & mspi driving current, IC default 4ma.
#endif

#define FLASH_DRIVING_1mA       0
#define FLASH_DRIVING_2mA       1
#define FLASH_DRIVING_4mA       2
#define FLASH_DRIVING_8mA       3

#ifndef FLASH_DEFAULT_CURRENT
#define FLASH_DEFAULT_CURRENT   FLASH_DRIVING_8mA
#endif
#ifndef ENABLE_MSPI_FLASH_ACCESS
#define ENABLE_MSPI_FLASH_ACCESS   0
#endif

#ifndef ENABLE_BOOT_TIME_PROFILING
#define ENABLE_BOOT_TIME_PROFILING 0
#endif
////////////////////////////////////////////////////////////
// DDCCI
////////////////////////////////////////////////////////////
#ifndef DDCCI_FILTER_FUNCTION
#define DDCCI_FILTER_FUNCTION       0
#endif

#ifndef DDCCI_REPLY_NULL_MESSAGE // wait for coding
#define DDCCI_REPLY_NULL_MESSAGE    0 // set to reply null message for special case
#endif



////////////////////////////////////////////////////////////
// application
////////////////////////////////////////////////////////////
// input TMDS of some source may change during cpature window or set panel timing
// record input DE/start and apply to display setting of SC0_05 ~ 0C
#ifndef ENABLE_CHECK_TMDS_DE
#define ENABLE_CHECK_TMDS_DE    1
#endif

////////////////////////////////////////////////////////////
// external device
////////////////////////////////////////////////////////////
//============= TOUCH KEY==============================
#ifndef ENABLE_TOUCH_KEY
#define ENABLE_TOUCH_KEY            0
#endif

#define TOUCH_KEY_SOURCE_ITE        0
#define TOUCH_KEY_SOURCE_SMSC       1
#define TOUCH_KEY_SOURCE_CYPRESS    2
#ifndef TOUCH_KEY_SOURCE
#define TOUCH_KEY_SOURCE            TOUCH_KEY_SOURCE_ITE
#endif

#if ENABLE_TOUCH_KEY && (TOUCH_KEY_SOURCE == TOUCH_KEY_SOURCE_ITE)
#define ENABLE_LOW_CONTACT          1
#else
#define ENABLE_LOW_CONTACT          0
#endif

#ifndef MOBILE_INTERFERENCE
#define MOBILE_INTERFERENCE         0
#endif

#ifndef TOUCH_KEY_CTRL_LED
#define TOUCH_KEY_CTRL_LED          0
#endif

#ifndef TOUCH_KEY_POWER_KEY_DEBOUNCE
#define TOUCH_KEY_POWER_KEY_DEBOUNCE    0
#endif

#define POWER_KEY_DEBOUNCE_PEROID   10 // unit: 50ms
//==========================================================
//==============TOUCH PANEL==================================
#ifndef ENABLE_TOUCH_PANEL
#define ENABLE_TOUCH_PANEL          0
#endif

#ifndef ENABLE_TOUCH_PANEL_DATA_FROM_USB
#define ENABLE_TOUCH_PANEL_DATA_FROM_USB     0
#endif

#ifndef ENABLE_TOUCH_PANEL_CTRL_OSD
#define ENABLE_TOUCH_PANEL_CTRL_OSD     0
#endif
//=============================================================

#ifndef ENABLE_VC_8_5
#define ENABLE_VC_8_5       1 // default enable
#endif

#ifndef ENABLE_OSD_ROTATION
#define ENABLE_OSD_ROTATION       1
#define _OSD_ROTATION_180_       //(1&&ENABLE_OSD_ROTATION)
#define _OSD_ROTATION_270_       //(1&&ENABLE_OSD_ROTATION)
#endif


////////////////////////////////////////////////////////////
// OTHER
////////////////////////////////////////////////////////////
#ifndef PANEL_MINI_LVDS
#define PANEL_MINI_LVDS             0
#endif

#ifndef AdjustVolume_UseTable
#define AdjustVolume_UseTable       0
#endif

#ifndef ENABLE_DeltaE
#define ENABLE_DeltaE 0
#endif

#ifndef LD_ENABLE
#define LD_ENABLE 0
#endif

// 120119 coding test for TPV
// 0: brightness control by PWM duty
// 1: brightness control by string current and fix PWM duty to max
#ifndef ENABLE_3DLUT
#define ENABLE_3DLUT                0
#endif

#ifndef ENABLE_XTAL_LESS
#define ENABLE_XTAL_LESS            0
#endif

#ifndef ENABLE_FREESYNC
#define ENABLE_FREESYNC            0
#endif

#ifndef ENABLE_CABLE_5V_EDID
#define ENABLE_CABLE_5V_EDID       0
#endif

#ifndef FPLL_TUNE_LIMIT_HW_VX1
#define FPLL_TUNE_LIMIT_HW_VX1           40   // 0.4% ,unit:0.01%
#endif

#ifndef FPLL_TUNE_LIMIT_HW_EDP
#define FPLL_TUNE_LIMIT_HW_EDP           10    //0.1% ,unit:0.01%
#endif

#ifndef FPLL_TUNE_LIMIT_HW
#define FPLL_TUNE_LIMIT_HW           40   // 0.4% ,unit:0.01%
#endif

#ifndef DISP_PWM_MAPPING_TYPE
#define DISP_PWM_MAPPING_TYPE       0 // value from 0 to 3
#endif
//__PAD_TOP__||____________________dig_pwm____________________|
//___________||_____________DISP_PWM_MAPPING_TYPE_____________|
//___________||_____0_____|_____1_____|_____2_____|_____3_____|
//_DISP_PWM0_||_DISP_PWM0_|_DISP_PWM2_|_DISP_PWM4_|_DISP_PWM2_|
//_DISP_PWM1_||_DISP_PWM1_|_DISP_PWM3_|_DISP_PWM5_|_DISP_PWM3_|
//_DISP_PWM2_||_DISP_PWM2_|_DISP_PWM4_|_DISP_PWM0_|_DISP_PWM0_|
//_DISP_PWM3_||_DISP_PWM3_|_DISP_PWM5_|_DISP_PWM1_|_DISP_PWM1_|
//_DISP_PWM4_||_DISP_PWM4_|_DISP_PWM0_|_DISP_PWM2_|_DISP_PWM4_|
//_DISP_PWM5_||_DISP_PWM5_|_DISP_PWM1_|_DISP_PWM3_|_DISP_PWM5_|

////////////////////////////////////////////////////////////
//NEW Hue Saturation Brightness
////////////////////////////////////////////////////////////
#define OSD_CONTROL_CSC  1

#define GLOBL_HSL_IP_Y2R 0
#define GLOBL_HSL_IP_HSY 1 //GlobalHueSat_HSY

#define INDEP_HSL_IP_RGB 0
#define INDEP_HSL_IP_HSY 1

#if (CHIP_ID == CHIP_MT9700)
	#define ENABLE_FULL_RGB_COLOR_PATH 		0 //0:Original Flow; 1:Flow with RGB_FULL for MT9700 only

#if (ENABLE_FULL_RGB_COLOR_PATH == 0)
#define INDEP_HSL_IP_SEL INDEP_HSL_IP_RGB //forced set INDEP_HSL_IP_SEL is INDEP_HSL_IP_RGB
#define GLOBL_HSL_IP_SEL GLOBL_HSL_IP_Y2R //forced set GLOBL_HSL_IP_SEL is GLOBL_HSL_IP_Y2R
#else//(ENABLE_FULL_RGB_COLOR_PATH == 1)
#define INDEP_HSL_IP_SEL INDEP_HSL_IP_HSY //forced set INDEP_HSL_IP_SEL is INDEP_HSL_IP_RGB
#define GLOBL_HSL_IP_SEL GLOBL_HSL_IP_Y2R //forced set GLOBL_HSL_IP_SEL is GLOBL_HSL_IP_Y2R
#endif
#else//CHIP_MT9701
	#define ENABLE_FULL_RGB_COLOR_PATH 		0

	#define INDEP_HSL_IP_SEL INDEP_HSL_IP_HSY 
	#define GLOBL_HSL_IP_SEL GLOBL_HSL_IP_Y2R
#endif

#endif

////////////////////////////////////////////////////////////
// Internal Certification
////////////////////////////////////////////////////////////
#ifndef HDMI_HPD_AC_ON_EARLY_PULL_LOW
#define HDMI_HPD_AC_ON_EARLY_PULL_LOW            0
#endif
