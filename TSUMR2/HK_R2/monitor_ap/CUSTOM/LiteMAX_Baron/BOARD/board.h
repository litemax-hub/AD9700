#ifndef _BOARD_H_
#define _BOARD_H_

#include "datatype.h"
#include "GPIO_DEF.h"
#include "chiptype.h"
#include "SW_Config.h"

#define CHIP_TSUML      0
#define CHIP_TSUMU      1
#define CHIP_TSUMB      2
#define CHIP_TSUMY      3
#define CHIP_TSUMV      4
#define CHIP_TSUM2      5
#define CHIP_TSUMC      6
#define CHIP_TSUMD      7
#define CHIP_TSUMJ      8
#define CHIP_TSUM9      9
#define CHIP_TSUMF      10
#define CHIP_TSUMG      11
#define CHIP_MT9700     12
#define CHIP_MT9701     13


#define MST9U3      30
#define MST9U4      31
#define MST9U5      42

////////////////////////////////////////////////////////////
//     DDR parameter
////////////////////////////////////////////////////////////
// MT9701
#define DDR2_1200_128Mb_QFP216      0
#define DDR2_1200_128Mb_QFP156      1
#define DDR2_1333_512Mb_QFP216      2
#define DDR2_1333_512Mb_QFP156      3
#define DDR2_QFP216                 4  // auto select DRAM size by trim value
#define DDR2_QFP156                 5  // auto select DRAM size by trim value

#define DRAM_NOUSE                  0xFF

////////////////////////////////////////////////////////////
//     DDR TYPE
////////////////////////////////////////////////////////////
// MT9701
#define DDR2_WB        0
#define DDR2_ESMT      1

#define DDR2_128Mb     0
#define DDR2_512Mb     1

#define DRAM_UNKNOWN              0xFF

////////////////////////////////////////////////////////////
// INPUT_TYPE DEFINE
////////////////////////////////////////////////////////////
#define INPUT_1A        BIT0    // VGA port
#define INPUT_1C        BIT1    // digital portx1
#define INPUT_2C        BIT2    // digital portx2
#define INPUT_3C        BIT3    // digital portx3
#define INPUT_4C        BIT4    // digital portx4

//////////////////////////////////////////////////////////////
// KEY SELECTION
//////////////////////////////////////////////////////////////
#define KEY_TYPE_IOKEY                  0
#define KEY_TYPE_ADKEY                  1
#define KEY_TYPE_IOWITHADKEY            2
#define KEY_TYPE_I2CKEY_CAPACTIVE       3

////////////////////////////////////////////////////////////
// ODM_NAME DEFINE
////////////////////////////////////////////////////////////
#define ODM_AOC                     0
#define ODM_INL                     1
#define ODM_BENQ                    1

////////////////////////////////////////////////////////////
// FACTORYALIGN_TYPE DEFINE
////////////////////////////////////////////////////////////
#define FACTORYALIGN_RS232              0
#define FACTORYALIGN_DDCCI              1

////////////////////////////////////////////////////////////
// HDCPKEY SAVE DEFINE
////////////////////////////////////////////////////////////
#define HDCPKEY_USEFLASH            0
#define HDCPKEY_USE24C04            1
#define HDCPKEY_USE24C16            2

////////////////////////////////////////////////////////////
// MAINBOARD TYPE DEFINE
////////////////////////////////////////////////////////////
#define BD_FPGA             0x0001   // FPGA
//MT9700
#define BD_MTV19067         0x0100   // QFP 128-A with 1A1H2DP
#define BD_MTV19069         0x0101   // QFP 156-B1 with 1A1H1DP1C, internalCC
#define BD_MTV19072         0x0102   // QFP 156-B2 with 1A2H2DP
#define BD_MTV19071         0x0103   // QFP 156-B3 with 1A1H1DP1C
#define BD_MST162A_A02A_S   0x0104   // T9 with 1A1H2DP
#define BD_MST162F_A01A_S   0x0105   // G with 1A1H
#define BD_MST218B_B01A_S   0x0106   // G with 1A1DP
#define BD_MT9700_A2V1_P2_S 0x0107   // QFP 128-A2 with 1A1H2DP
#define BD_MT9700_A3V2_P2_S 0x0108   // QFP 156-B3 with 1A1H1DP1C
#define BD_MT9700_A4V2_P2_S 0x0109   // QFP 156-B1 with 1A1H1DP1C, internalCC
#define BD_MT9700_A5V2_P2_S 0x010A   // QFP 128-A with 1A1H2DP
#define BD_MT9700_A6V2_P2_S 0x010B   // QFP 156-B2 with 1A2H2DP
#define BD_MT9700_A7V2_P2_S 0x010C   // T9 with 1A1H2DP
#define BD_MST218B_B02A_S   0x010D   // G with 1A1H

//MT9701
#define BD_MT9701_A1V1_P2_S 0x0200   // QFP216 with 1H1DP1C
#define BD_MT9701_A2V1_P2_S 0x0201   // QFP216 with 1H2DP
#define BD_MT9701_A3V1_P2_S 0x0202   // QFP216 with 1H1DP1C(USB3 debug board)
#define BD_MT9701_A4V1_P2_S 0x0203   // QFP156 1H1DP
#define BD_MT9701_A5V1_P2_S 0x0204   // QFP156 1H1DP

//MTxxxx
#define BD_MTVxxxxx         0x0300   // Next chip series to be


//********************LogoType Define*******************************
#define AOCLOGO                         1
#define EnvisionLOGO                    2
#define StaplesLOGO                     3
#define GIGABYTELOGO                    4
#define BELINEALOGO                     5
#define HAIERLOGO                       6
#define IMAGICLOGO                      7
#define FOUNDERLOGO                     8
#define NoBrand                         9
#define FujitsuLOGO                     10
#define CHEMEILOGO                      11
#define MstarLOGO                       12

//*****************************************************************
//*******************Language Define********************************
#define ASIA                            1
//English,Spanish,France,Portugues,Russia,SChina,TChina,Korea,
#define EUROPE                          2
//English,Spanish,France,German,Italian,Russia,Swedish,Polish,Czech,
#define SIXTEEN                         3
//English,German,France,Italian,Spanish,SChina,Japanese,Norsk,Russia,Swedish,Finnish,Dansk,Polish,Dutch,Portugues,Korea,
#define AllLanguage                     4
//TChina,English,France,German,Italian,Japanese,Korea,Russia,Spanish,Swedish,Turkish,SChina,Portugues,Czech,Dutch,Finnish,Polish,

//*****************************************************************
//*****************************************************************
#include "Model.h"
#include "DefaultCompilerOption.h"
#include "default_option_define.h"

#ifndef ENABLE_FACTORY_SSCADJ
#define ENABLE_FACTORY_SSCADJ               0
#endif

#ifndef ENABLE_PANELSERVICEMENU_SSCADJ
#define ENABLE_PANELSERVICEMENU_SSCADJ      0
#endif

#ifndef ENABLE_TIME_MEASUREMENT
#define ENABLE_TIME_MEASUREMENT             (1 && ENABLE_MSTV_UART_DEBUG)
#endif
#ifndef ENABLE_MCUTOPTEST
#define ENABLE_MCUTOPTEST                   0
#endif

#define DCRClearBacklight             // when drc is on in black pattern clear the backlight
//#if !defined(UseInternalDDCRam)
//#define UseVGACableReadWriteAllPortsEDID
//#endif



#define CompressPropFont                1
//*****************************************************************
//*****************************************************************
#if DisplayLogo==NoBrand
#define DisplayPowerOnLogo              0
#else
#define DisplayPowerOnLogo              1
#endif

#define HotInputSelect                  1

#if  (ENABLE_HDMI || ENABLE_DVI)
#define G2_G3_HDMI_AUDIO_FADING         0 // 1
#define EN_HDMI_TOSHIBA_DVD_PATCH       0 //When Toshiba HD_SA1 no disk DVD-P sends the Non-PCM format
#define ENABLE_CEC                      (0 || ENABLE_ARC)
#define ENABLE_CEC_INT                  (0 || ENABLE_ARC)
#define ENABLE_CTSNSYNC                 1
#define ENABLE_CTS_INT                  1
#define ENABLE_CTSN_FILTER              1
#define ENABLE_CTS_HPD_PD100K           0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_HDMI_SW_CTS_STABLE       0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_AUDIO_AUTO_MUTE          1 // auto mute setting
#define ENABLE_AUDIO_AUTO_FADING        1 // auto fading setting
#define ENABLE_HAYDN_FADING_BY_HDMI_STATUS        1
#define ENABLE_HDMI_SCDC                0 // for HDMI support over 3.4G, must config to 1.
#if (CHIP_ID == CHIP_MT9701)
#define ENABLE_HPD_REPLACE_MODE         1 // for HDMI under 3.4G only; if over 3.4G need to config to 0.
#elif (CHIP_ID == CHIP_MT9700)
#define ENABLE_HPD_REPLACE_MODE         0 // for HDMI under 3.4G only; if over 3.4G need to config to 0.
#endif
#define ENABLE_HDMI_BCHErrorIRQ         (0 && (CHIP_ID == CHIP_MT9700))// Use BCHerror IRQ to cover source garbege issue
#else
#define G2_G3_HDMI_AUDIO_FADING         0
#define ENABLE_CEC                      0
#define ENABLE_CEC_INT                  0
#define ENABLE_CTSNSYNC                 0
#define ENABLE_CTS_INT                  0
#define ENABLE_CTSN_FILTER              0
#define ENABLE_CTS_HPD_PD100K           0
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_HDMI_SW_CTS_STABLE       0
#define ENABLE_HDMI_SW_AUDCLK_CHECK     0
#define ENABLE_AUDIO_AUTO_MUTE          0
#define ENABLE_AUDIO_AUTO_FADING        0
#define ENABLE_HAYDN_FADING_BY_HDMI_STATUS        0
#define ENABLE_HDMI_SCDC                0
#define ENABLE_HPD_REPLACE_MODE         0
#define ENABLE_HDMI_BCHErrorIRQ         0
#endif

#define Enable_CascadeMode              0
#define Enable_Gamma                    1
#define USE_NORMAL_GAMMA_TBL            (1&Enable_Gamma)

// HDCP key load from efuse
#define ENABLE_LOAD_KEY_VIA_EFUSE       1

#define DDCCI_ENABLE                    1

#define PANEL_WIDTH                     g_sPnlInfo.sPnlTiming.u16Width
#define PANEL_HEIGHT                    g_sPnlInfo.sPnlTiming.u16Height
#include "Panel.h"

#define Enable_Expansion                0///1

#define AOC_RSerialOSD                  1

#define MWEFunction                     0

#define Enable_Cache                    1

#define DATA_LENGTH_20_BYTE//For HDCP Tool Send 20Byte Data One Block
//*****************************************************************

#define HotExpansion        Enable_Expansion&&!AudioFunc

#if LiteMAX_Baron_OSD_TEST
#define PropFontUseCommonArea           0 // 1
#else
#define PropFontUseCommonArea           1
#endif

#define HDMI_PRETEST                    0

#define PORT_0_2_EXCHANGE               BIT0
#define PORT_0_3_EXCHANGE               BIT1
#define PORT_1_2_EXCHANGE               BIT2
#define PORT_1_3_EXCHANGE               BIT3

// For Preprocessing
#define CInput_Nothing          0xFF

#if (INPUT_TYPE&INPUT_1A)
#define CInput_Digital          1
#define CInput_Digital2         2
#define CInput_Digital3         3
#define CInput_Digital4         4
#else
#define CInput_Digital          0
#define CInput_Digital2         1
#define CInput_Digital3         2
#define CInput_Digital4         3
#endif

#include "REG52.H"
#if (MainBoardType == BD_FPGA)
#define USEFLASH    1
#include "BD_FPGA.h"
#elif (MainBoardType == BD_MTV19067)
#define USEFLASH    1
#include "BD_MTV19067.h"
#elif (MainBoardType == BD_MTV19069)
#define USEFLASH    1
#include "BD_MTV19069.h"
#elif (MainBoardType == BD_MTV19072)
#define USEFLASH    1
#include "BD_MTV19072.h"
#elif (MainBoardType == BD_MTV19071)
#define USEFLASH    1
#include "BD_MTV19071.h"
#elif (MainBoardType == BD_MST162A_A02A_S)
#define USEFLASH    1
#include "BD_MST162A_A02A_S.h"
#elif ((MainBoardType == BD_MST162F_A01A_S) || (MainBoardType == BD_MST218B_B02A_S))
#define USEFLASH    1
#include "BD_MST162F_A01A_S.h"
#elif (MainBoardType == BD_MST218B_B01A_S)
#define USEFLASH    1
#include "BD_MST218B_B01A_S.h"
#elif (MainBoardType == BD_MT9700_A2V1_P2_S)
#define USEFLASH    1
#include "BD_MT9700_A2V1_P2_S.h"
#elif (MainBoardType == BD_MT9700_A3V2_P2_S)
#define USEFLASH    1
#include "BD_MT9700_A3V2_P2_S.h"
#elif (MainBoardType == BD_MT9700_A4V2_P2_S)
#define USEFLASH    1
#include "BD_MT9700_A4V2_P2_S.h"
#elif (MainBoardType == BD_MT9700_A5V2_P2_S)
#define USEFLASH    1
#include "BD_MT9700_A5V2_P2_S.h"
#elif (MainBoardType == BD_MT9700_A6V2_P2_S)
#define USEFLASH    1
#include "BD_MT9700_A6V2_P2_S.h"
#elif (MainBoardType == BD_MT9700_A7V2_P2_S)
#define USEFLASH    1
#include "BD_MT9700_A7V2_P2_S.h"
#elif (MainBoardType == BD_MT9701_A1V1_P2_S) || (MainBoardType == BD_MT9701_A3V1_P2_S)
#define USEFLASH    1
#include "BD_MT9701_A1V1_P2_S.h"
#elif (MainBoardType == BD_MT9701_A2V1_P2_S)
#define USEFLASH    1
#include "BD_MT9701_A2V1_P2_S.h"
#elif (MainBoardType == BD_MT9701_A4V1_P2_S)
#define USEFLASH    1
#include "BD_MT9701_A4V1_P2_S.h"
#elif (MainBoardType == BD_MT9701_A5V1_P2_S)
#define USEFLASH    1
#include "BD_MT9701_A5V1_P2_S.h"
#elif (MainBoardType == BD_MT9700_LITEMAX)
#define USEFLASH    1
#include "BD_MT9700_LITEMAX.h"
#elif (MainBoardType == BD_MT9700_LITEMAX_1DP)
#define USEFLASH    1
#include "BD_MT9700_LITEMAX_1DP.h"
#else
#error Unknow board type
#endif

#if (CInput_UsbTypeC_C3!=CInput_Nothing) || (CInput_UsbTypeC_C4!=CInput_Nothing)
#define ENABLE_USB_TYPEC            1
#define ENABLE_PD_CODE_ON_PSRAM     1
#else
#define ENABLE_USB_TYPEC            0
#define ENABLE_PD_CODE_ON_PSRAM     0
#endif

#define ENABLE_PD_FW_AUTO_UPDATE    (1 && (CHIP_ID == CHIP_MT9700) && ENABLE_USB_TYPEC && !ENABLE_INTERNAL_CC) // For RT7880 F/W Auto Update

#define DPC_USB_ERROR_COUNT_CONFIG_EN          ENABLE_USB_TYPEC

// to reduce power consumption during monitor active
#define _IP_POWER_CTRL_

#ifndef Init_hwHDCP_Hpd_Pin2
#define Init_hwHDCP_Hpd_Pin2()
#endif

#ifndef Init_hwHDCP_Hpd_Pin3
#define Init_hwHDCP_Hpd_Pin3()
#endif

#ifndef CABLE_DETECT_VGA_USE_SAR
#define CABLE_DETECT_VGA_USE_SAR    0
#endif

#ifndef CABLE_DETECT_DVI_USE_SAR
#define CABLE_DETECT_DVI_USE_SAR    0
#endif

#ifndef CABLE_DETECT_HDMI_USE_SAR
#define CABLE_DETECT_HDMI_USE_SAR   0
#endif

#ifndef CABLE_DET_SAR_EN
#define CABLE_DET_SAR_EN            0
#endif

// Chip definition
#ifndef CHIP_ID
#define CHIP_ID                     CHIP_MT9700
#endif

#ifndef ENABLE_DP_INPUT
#define ENABLE_DP_INPUT             0
#endif

#ifndef ENABLE_DP_OUTPUT
#define ENABLE_DP_OUTPUT             0
#endif

#ifndef ENABLE_DP_AUDIO
#define ENABLE_DP_AUDIO             0
#endif

#ifndef ENABLE_DVI_DUAL_LINK
#define ENABLE_DVI_DUAL_LINK        0
#endif

#ifndef ENABLE_XIU_WRITE_KEY
#define ENABLE_XIU_WRITE_KEY        0
#endif

#ifndef ENABLE_SUPER_RESOLUTION
#define ENABLE_SUPER_RESOLUTION     0
#endif

#ifndef ENABLE_SHARPNESS
#define ENABLE_SHARPNESS            0
#endif

#ifndef ENABLE_DLC
#define ENABLE_DLC                  0
#endif

#ifndef ENABLE_DPS
#define ENABLE_DPS                  0
#endif

#ifndef ENABLE_DeBlocking
#define ENABLE_DeBlocking           0
#endif

#ifndef FLASH_BLOCK_SIZE
#define FLASH_BLOCK_SIZE            0x10000
#endif

#ifndef ENABLE_SW_AUTOK
#define ENABLE_SW_AUTOK             0
#endif

#ifndef ENABLE_VGA_EDID_INTERNAL_DATA
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#endif


// keep using DOUBLE_LVDS_CLK_SWING, removed this new define
//#ifndef LVDS_CLK_1TO2
//#define LVDS_CLK_1TO2               0
//#endif

// 120106 coding addition, default set adc dithering to disable
#if 0 // move to DefaultComplierOption.h
#ifndef ENABLE_ADC_DITHERING
#define ENABLE_ADC_DITHERING    0
#endif
#endif

//I_Gen_Tuning Target Select
#define IGT_TARGET_250  0
#define IGT_TARGET_200  1
#define IGT_TARGET_300  2
#define IGT_TARGET_350  3
#define IGT_TARGET_375  4
#define IGT_TARGET_400  5
#define IGT_TARGET_425  6
#define IGT_TARGET_450  7

//I_Gen_Tuning CH Source Select
#if 1
#define CH2             0
#define CH6             1
#define CH8             2
#else
#define CH3_CLK         0
#define CH9_CLK         1
#define CH15_CLK        2
#define CH21_CLK        3
#endif
//LVDS_CHANNEL select
#define MOD_PORTA       BIT0
#define MOD_PORTB       BIT1
#define MOD_PORTC       BIT2
#define MOD_PORTD       BIT3

//TMDS PORT
#define TMDS_PORT_UNUSE 0
#define TMDS_PORT_A     BIT0
#define TMDS_PORT_B     BIT1
#define TMDS_PORT_C     BIT2
#define TMDS_PORT_D     BIT3

//OD mode
#define OD_MODE_444             0
#define OD_MODE_565             1
#define OD_MODE_666             4
#define OD_MODE_485             5 //compress mode
#define OD_MODE_555             6
#define OD_MODE_888             7

#define OD_HSCAL_EN             _ENABLE
#define OD_VSCAL_EN             _ENABLE

#ifndef OD_MODE
#define OD_MODE                 OD_MODE_485
#endif

// 3D glasses type
#define GLASSES_NONE            0
#define GLASSES_IR              1
#define GLASSES_INTEL           2
#define GLASSES_NVIDIA          3

#ifndef GLASSES_TYPE
#define GLASSES_TYPE            GLASSES_NONE
#endif

#define EnableDACDePop		    (1&&AudioFunc)
//---------------------------------------------------------------------
// For reducing depop off time but it is out of pop noise spec(< -75dB)
// EAR_OUT DC off: -54dB, DC on: -75dB
// LINE_OUT DC off: -35dB, DC on: -63dB
//---------------------------------------------------------------------
#define QUICK_DEPOP_OFF         0

////////////////////////////////////////////////////////
// Frame buffer setting
////////////////////////////////////////////////////////
#define FRAME_BUFFER                0
#define FRAME_BFFLESS               1

#ifndef PANEL_3D_PASSIVE_4M
#define PANEL_3D_PASSIVE_4M         0
#endif

#ifndef FRAME_BFF_SEL
#define FRAME_BFF_SEL               FRAME_BFFLESS
#endif

#define DOWN_COUNTER_V_SYNC         0

#ifndef ENABLE_HK_CODE_ON_DRAM
#define ENABLE_HK_CODE_ON_DRAM      0
#endif

#ifndef ENABLE_HK_CODE_ON_PSRAM
#define ENABLE_HK_CODE_ON_PSRAM     0 //  All common bank on PSRAM (Max 16KB)
#endif

#ifndef ENABLE_PD_CODE_ON_PSRAM
#define ENABLE_PD_CODE_ON_PSRAM     0
#endif

#ifndef ENABLE_HK_XDATA_ON_DRAM
#define ENABLE_HK_XDATA_ON_DRAM     0
#endif

#define PANEL_H_DIV                 1 // H relative parameters(display timing) divided by 1 or 2

#if PANEL_H_DIV ==2
#define ACC_HDIV_BIT                BIT15
#else
#define ACC_HDIV_BIT                0
#endif

#ifndef _TMDS_DE_MODE_EN_
#define _TMDS_DE_MODE_EN_           0
#endif

#define EN_V4TAP_SCALING            0 //(CHIP_ID==CHIP_TSUMB || CHIP_ID==CHIP_TSUMY) //for vertical 4-tap scaling mode

#ifndef ENABLE_3D_FUNCTION
#define ENABLE_3D_FUNCTION          0
#endif

#ifndef MST_ASSERT
#define MST_ASSERT(_test_)
#endif

#ifndef EnableTime1Interrupt
#define EnableTime1Interrupt        0
#endif

#ifndef EnableTime2Interrupt
#define EnableTime2Interrupt        0
#endif

#define XTAL_CLOCK_KHZ              (12000ul)
#define CRYSTAL_CLOCK               (12000000ul)
#define RCOSC_4M_KHZ                (4000ul)
#define FRO_12M_KHZ                 (12000ul)
#define DEF_FORCE_DPMS              30 // unit: Second

#define EXT_TIMER0_1MS              (0)     // external timer 0 for 1ms interrupt
#define EXT_TIMER0_CounterTime      (1)     //unit: ms default set to 1ms

// 110905 coding, addition for getting more V to reduce output dclk
#define DECREASE_V_SCALING          1
#define INT_PERIOD                  1// timer interrupt: unit = 1ms
//#define BaudRatePeriod                        (0x10000 - (((unsigned long)CPU_CLOCK_MHZ * 1000000)/((unsigned long) SERIAL_BAUD_RATE / 10 * 32) + 5) / 10)
#define SystemTick                  1000    // 1 second
#define UseINT                      1
#define UsesRGB                     0

#define ClockPeriod                 (0x10000-(((unsigned long)XTAL_CLOCK_KHZ*INT_PERIOD)/12))    // Interrupt every 1ms for Time 0
#define BaudRatePeriod              (0x10000-(((unsigned long)XTAL_CLOCK_KHZ * 1000+(DWORD)SERIAL_BAUD_RATE*16)/((DWORD)SERIAL_BAUD_RATE*32)))
#define T1BaudRatePeriod            (0x100-(((unsigned long)XTAL_CLOCK_KHZ * 1000+(DWORD)SERIAL_BAUD_RATE*192)/((DWORD)SERIAL_BAUD_RATE*384)))
#define HFreq(hPeriod)              ((hPeriod)?(((DWORD)XTAL_CLOCK_KHZ*10+hPeriod/2)/hPeriod):(1)) // 120330 coding modified to avoid divide 0
#define VFreq(hFreq, vTotal)        ((SrcVTotal)?(((DWORD)hFreq*1000+SrcVTotal/2)/SrcVTotal):(1)) // 120330 coding modified to avoid divide 0
#define MST_HPeriod_UINT_16         (XTAL_CLOCK_KHZ*16)
#define MST_HPeriod_UINT            (XTAL_CLOCK_KHZ*8)
#define VFreqIn                     (((VFreq( HFreq( SrcHPeriod ), SrcVTotal )+5)/10) * ((SrcFlags&bInterlaceMode)?2:1))

#endif

