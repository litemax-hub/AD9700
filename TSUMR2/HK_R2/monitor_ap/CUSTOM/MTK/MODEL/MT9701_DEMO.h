#ifndef MT9701_DEMO_H
#define MT9701_DEMO_H
////////////////////////////////////////////////////////////
// BOARD SELECTION
////////////////////////////////////////////////////////////
#define CHIP_ID         CHIP_MT9701
//#define MainBoardType   ==> Set in .config

#define CHIP_FAMILY_TYPE CHIP_FAMILY_MST9U // for R2 platform driver
////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define DVI_USB                         0       // 1: DVI from USB
#define DVI_PN_SWAP                     0
#define HDMI_PN_SWAP                    1
#define ENABLE_AUTOEQ                   0
#if MainBoardType == BD_FPGA    //FPGA
#define INPUT_TYPE                      (INPUT_4C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     0 
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_DVI                      0
#define ENABLE_DP_INPUT                 1
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define ENABLE_ARC                      0
#define _NEW_SOG_DET_                   0
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DDR2_1333_512Mb_QFP216
#elif (MainBoardType == BD_MT9701_A1V1_P2_S) || (MainBoardType == BD_MT9701_A3V1_P2_S)
#define INPUT_TYPE                      (INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_DVI                      0
#define ENABLE_DP_INPUT                 1
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define ENABLE_ARC                      0
#define _NEW_SOG_DET_                   0
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DDR2_1333_512Mb_QFP216 // DDR2_1200_128Mb_QFP216
#elif MainBoardType == BD_MT9701_A2V1_P2_S
#define INPUT_TYPE                      (INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_DVI                      0
#define ENABLE_DP_INPUT                 1
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define ENABLE_ARC                      0
#define _NEW_SOG_DET_                   0
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DDR2_1333_512Mb_QFP216 // DDR2_1200_128Mb_QFP216
#elif MainBoardType == BD_MT9701_A4V1_P2_S
#define INPUT_TYPE                      (INPUT_2C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_DVI                      0 
#define ENABLE_DP_INPUT                 1
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define ENABLE_ARC                      0
#define _NEW_SOG_DET_                   0
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DDR2_1333_512Mb_QFP156 // DDR2_1200_128Mb_QFP156
#elif MainBoardType == BD_MT9701_A5V1_P2_S
#define INPUT_TYPE                      (INPUT_2C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_DVI                      0 
#define ENABLE_DP_INPUT                 1
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define ENABLE_ARC                      0
#define _NEW_SOG_DET_                   0
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DDR2_1333_512Mb_QFP156 // DDR2_1200_128Mb_QFP156
#else
#message "please define new board type"
#endif

#define MS_VGA_SOG_EN                   1

////////////////////////////////////////////////////////////
// PANEL SELECTION
////////////////////////////////////////////////////////////
#ifdef TSUMR2_FPGA
#define PanelType                   PanelCMIM236HGJ_L21_FPGA  // FPGA
#else
#define PanelType                   PanelCMIM236HGJ_L21//PanelM250HTN01//PanelAUOM185XW01//PanelCMIM236HGJ_L21//PanelCMIM236HGJ_L21
#endif

////////////////////////////////////////////////////////////
// BRIGHTNESS CONTROL
////////////////////////////////////////////////////////////
#define BrightFreqByVfreq               0   // set brightness freq. by input Vfreq.
#define BRIGHTNESS_INVERSE              1
#define BRIGHTNESS_FREQ                 22000 // unit: Hz
#define BRIGHTNESS_VSYNC_ALIGN          1
#define BRIGHTNESS_HSYNC_ALIGN          0

////////////////////////////////////////////////////////////
// HDMI
////////////////////////////////////////////////////////////
#define COMBO_TMDS_PACKET_UPDATED_BY_VSYNC (0 || ENABLE_DOLBY_HDR || ENABLE_HDR_AUTODETECT) // Updated partial packet received by AP layer.

////////////////////////////////////////////////////////////
// AUDIO
////////////////////////////////////////////////////////////
#define CheckInitialDACTime         0

////////////////////////////////////////////////////////////
// OSD
////////////////////////////////////////////////////////////
#define DisplayLogo                     MstarLOGO
#define LANGUAGE_TYPE                   ASIA    //SIXTEEN

////////////////////////////////////////////////////////////
// SYSTEM
////////////////////////////////////////////////////////////
#define AUTO_DATA_PATH                  0
#define ENABLE_DeltaE                   1 //AutoColorCalibrationTool online calibration
#define ENABLE_ColorMode_FUNCTION       1
#define ENABLE_AC3DLut_FUNCTION         (0 && ENABLE_3DLUT)
#define ENABLE_DICOM_FUNCTION           0
#define ENABLE_GAMMA_FUNCTION           (1 && ENABLE_DeltaE)
#define ENABLE_COLORTEMP_FUNCTION       (1 && ENABLE_DeltaE)
#define ENABLE_COLORTEMP_GAMMA			(0 && ENABLE_COLORTEMP_FUNCTION)
#define ENABLE_COLORTRACK_FUNCTION      (0 && ENABLE_DeltaE)

#define ENABLE_IDX_LOADCOLORMODE_FUNCTION (1 && ENABLE_DeltaE)
#define ENABLE_COlORMODE_ADJUST_PWM     (1 && ENABLE_IDX_LOADCOLORMODE_FUNCTION)
#define ENABLE_SAVE_COlORMODE_VERIFIED_VALUES     0
#define ENABLE_COLORTEMP_WORD_FORMAT    0
#define ENABLE_OUTPUT_PATTERN_WITHOUTSIGNAL		  0
#define ENABLE_DLC                      1
#define ENABLE_DPS                      1
#define ENABLE_SUPER_RESOLUTION         1
#define ENABLE_SHARPNESS                1
#define ENABLE_LOCALDIMMING             0
#define ENABLE_HDR                      0
#define ENABLE_HDR_AUTODETECT 			(1 && ENABLE_HDR)
#define ENABLE_LUT_AUTODOWNLOAD         (1 && ENABLE_AUTOLOAD)
#define ENABLE_ACHDR_FUNCTION           (1 && ENABLE_HDR)
#define ENABLE_HDR_DYNAMIC_FUNCTION     (0 && ENABLE_HDR)
#define ENABLE_DOLBY_HDR                (0 && ENABLE_PQ_R2)
#define ENABLE_YUV420                   1
#define ENABLE_DP_YUV420                ENABLE_YUV420
#define ENABLE_DP_AUDIO_DPCD            1 // For DP1.4/1.2 CTS 5.2.2.6/7.2.1.6, set to 0 for NO DP audio project, otherwise, set to 1
#define ENABLE_DP_RX_MAX_LINKRATE_HBR2  0 // DP1.2 or 1.4 set max link rate as HBR2 or not, MT9700 default DP1.2/1.4 as HBR only
#define ENABLE_POWER_MEASUREMENT        0
#define XVYCC_GAMMA_EXT 1
#define ENABLE_HPERIOD_16LINE_MODE      1 // dynamic 8/16 line mode switch

#define tool_model_panel_name			"MT9701_PanelCMIM236HGJ_L21"
#define USER_PREF_SERIALNUMBER(SN)      (_serialNumber[SN]) //length must smaller than 32

#define FACTORYALIGN_TYPE               FACTORYALIGN_DDCCI

#define ENABLE_FACTORY_SSCADJ           1

#if INPUT_TYPE > INPUT_1A
#define ENABLE_HDCP                     1
#define HDCPKEY_IN_Flash                1
#else
#define ENABLE_HDCP                     0
#define HDCPKEY_IN_Flash                0
#endif

#define ENABLE_MENULOAD                 1
#define ENABLE_MENULOAD_2               (0 && ENABLE_MENULOAD && (!ENABLE_PQ_R2))
#define MENULOAD_TRIG                   OP_TRIG
#define MENULOAD_2_TRIG                 OP_TRIG

#define ENABLE_DYNAMICSCALING           1

#define ENABLE_AUTOLOAD                 1
#define ENABLE_RTE                      1
#define ENABLE_OD_AutoDownTBL           (0 && ENABLE_RTE)
#define ENABLE_OD_VRR                   (0 && ENABLE_RTE)
#define ENABLE_OD_MTG                   (0 && ENABLE_RTE)

#define ENABLE_TOUCH_PANEL                  0
#define ENABLE_TOUCH_PANEL_DATA_FROM_USB    0
#define ENABLE_TOUCH_PANEL_CTRL_OSD         0

#define ENABLE_FREESYNC                     1
#define ENABLE_HDMI_ALLM                    (1 && ENABLE_HDMI)
#define ENABLE_FBL_FRAMEPLL                 0 // uses FBL FPLL instead of FLock mode
#define ENABLE_HDMI_DRR_MCCS                (0 && ENABLE_FREESYNC)

//#define ENABLE_PD_CODE_ON_PSRAM         ENABLE_USB_TYPEC //Wakeup PD51
#define ENABLE_MSPI_FLASH_ACCESS            0 //
#define FLASH_READ_BYTE_BY_CODE_POINTER     1
#define ENABLE_WINISP                       (1 & ENABLE_AUTOLOAD & ENABLE_RTE)
#define ENABLE_ENG_USB_MODE                 0
#define XMODEM_DWNLD_ENABLE                 0
#define ENABLE_SECU_R2                      1 //HDCP function work have to enable
#define ENABLE_HDCP22                       (1 & ENABLE_SECU_R2)
#define ENABLE_PQ_R2                        (TTS || DV)

#define ENABLE_DMA_MIIC                     1
////////////////////////////////////////////////////////////
// USB HOST & FILESYSTEM
////////////////////////////////////////////////////////////
#define ENABLE_USB_HOST                     1
#define ENABLE_USB_HOST_DUAL_IMAGE_SIGN     0
#define ENABLE_USB_HOST_DUAL_IMAGE_OFFSET   0x100000 // 1M bytes limited, per image, 64k aligned
#if ENABLE_USB_HOST_DUAL_IMAGE_SIGN
#define ENABLE_USB_HOST_DUAL_IMAGE_SBOOT    0x12000
#else
#define ENABLE_USB_HOST_DUAL_IMAGE_SBOOT    0x6000
#endif
#define ENABLE_FILESYSTEM                   ENABLE_USB_HOST
#define ENABLE_USB                          ENABLE_USB_HOST

////////////////////////////////////////////////////////////
// USB DEVICE
////////////////////////////////////////////////////////////
#define ENABLE_USB_DEVICE                   (0 && !ENABLE_USB_HOST)
#define ENABLE_USB_DEVICE_UPDATE_CODE       (0 && ENABLE_USB_DEVICE)

////////////////////////////////////////////////////////////
// POWERMANAGEMENT
////////////////////////////////////////////////////////////
#define MS_PM                           1
#define PM_MCUSLEEP                     0
#define LED_PWM                         0

////////////////////////////////////////////////////////////
// WATCHDOG
////////////////////////////////////////////////////////////
#define ENABLE_WATCH_DOG                DISABLE
#define WATCH_DOG_TIME_RESET            (10)    // unit: second
#define WATCH_DOG_TIME_Interrupt        (3)     // unit: second

////////////////////////////////////////////////////////////
// SI
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// INFORMATION
////////////////////////////////////////////////////////////
#define ChipName                "MT9701"
#define Model                   "XXXXXXXXX"
#define FWVersion               "000"
#define ChangeDate              "20220304"


//////////////////////////////////////////////////////////////
// DP CTS test
// if enable, system will keep at normal speed without entering PM,
// and SrcInputType stays at FIXED_PORT
//////////////////////////////////////////////////////////////
#define ENABLE_DP_CTS_TEST         (0 &&ENABLE_DP_INPUT)

///////////////////////////////////////////////////////////////
// Internal Certification
//////////////////////////////////////////////////////////////
#define ENABLE_MTK_TEST                     0
#define HDMI_HPD_AC_ON_EARLY_PULL_LOW       ((0 || ENABLE_MTK_TEST) && (CHIP_ID == CHIP_MT9701) && ENABLE_HDMI)
///////////////////////////////////////////////////////////////
// Custom Related
//////////////////////////////////////////////////////////////
#define CUSTOMIZE_IP_OPTION             1
#define EDID_WEEK_YEAR_SN_REPLACEMENT   0

///////////////////////////////////////////////////////////////
// SYSTEM DEBUG
//////////////////////////////////////////////////////////////
#define ENABLE_COMPILE_INFO         1 // Enable compiling infomation in BIN file, checksum of code will change every time after compile (Better to sync with SB/PM on the same option)
#define ENABLE_DEBUG                ENABLE_MSTV_UART_DEBUG
#define EnableSerialPortTXInterrupt 1

//Using drvUARTPIU.c as PIU_UART drvier
#define ENABLE_UART_PIU             0
#define ENABLE_MSTV_UART_DEBUG_PIU  (0 & ENABLE_UART_PIU) //cannot be used in PM mode
#define UART_PIU_BAUDRATE           115200ul //9600ul
#define UART_PIU_GPIO_PORT          UART_GPIO_RESERVED
#define ENABLE_MSBHK                (0 && UART1) // need sync with MSBHK

#if ENABLE_DP_CTS_TEST
#define DISABLE_AUTO_SWITCH         1 // for DP test
#define FIXED_PORT                  Input_Displayport
#else
#define DISABLE_AUTO_SWITCH         0
#define FIXED_PORT                  Input_HDMI
#endif
#define ENABLE_SPEEDUP_SWITCH       (0 && !DISABLE_AUTO_SWITCH)


#define ENABLE_3DLUT                0
#define LVDS_CLK_1TO2_OFFSET        4

#define WAIT_CORE_POWER_READY(TimeOutMs)   do {\
                                       TimeOutCounter=TimeOutMs;\
                                       while((!(msReadByte(REG_00039C)&BIT1)) && TimeOutCounter);\
                                     } while(0)
                                     
// Core Power Switch
#define Disable_PM_Power()  do{\
                                MEM_MSWRITE_BIT(REG_00042C, 0, BIT0);\
                                MEM_MSWRITE_BIT(REG_00042E, 0, BIT0);\
                              }while(0)

#define Enable_PM_Power()   do{\
                                MEM_MSWRITE_BIT(REG_00042E, BIT0, BIT0);\
                                MEM_MSWRITE_BIT(REG_00042C, BIT0, BIT0);\
                              }while(0)

#define Disable_U3_Power()  do{\
                                MEM_MSWRITE_BIT(REG_000375, 0, BIT4);\
                                MEM_MSWRITE_BIT(REG_000375, 0, BIT5);\
                              }while(0)

#define Enable_U3_Power()   do{\
                                MEM_MSWRITE_BIT(REG_000375, 1, BIT5);\
                                MEM_MSWRITE_BIT(REG_000375, 1, BIT4);\
                              }while(0)
                              
#define DP_EDID_BUSY() ((((msRegs(REG_DPRX_AUX_PM0_1C_L) & 0xF) != 0x08 &&\
        (msRegs(REG_DPRX_AUX_PM0_1C_L) & 0xF) != 0x09 )&&\
        ((((msRegs(REG_DPRX_AUX_PM0_56_L) & BIT4) == FALSE) &&\
        ((msRegs(REG_DPRX_AUX_PM0_74_L) & (BIT5|BIT4)) == 0) &&\
        ((msRegs(REG_DPRX_AUX_PM0_74_H) & (BIT3|BIT2|BIT1|BIT0)) == 0))==FALSE))||\
        (((msRegs(REG_DPRX_AUX_PM0_1C_L+0x300) & 0xF) != 0x08 &&\
        (msRegs(REG_DPRX_AUX_PM0_1C_L+0x300) & 0xF) != 0x09 )&&\
        ((((msRegs(REG_DPRX_AUX_PM0_56_L+0x300) & BIT4) == FALSE) &&\
        ((msRegs(REG_DPRX_AUX_PM0_74_L+0x300) & (BIT5|BIT4)) == 0) &&\
        ((msRegs(REG_DPRX_AUX_PM0_74_H+0x300) & (BIT3|BIT2|BIT1|BIT0)) == 0))==FALSE))||\
        (((msRegs(REG_DPRX_AUX_PM0_1C_L+0x600) & 0xF) != 0x08 &&\
        (msRegs(REG_DPRX_AUX_PM0_1C_L+0x600) & 0xF) != 0x09 )&&\
        ((((msRegs(REG_DPRX_AUX_PM0_56_L+0x600) & BIT4) == FALSE) &&\
        ((msRegs(REG_DPRX_AUX_PM0_74_L+0x600) & (BIT5|BIT4)) == 0) &&\
        ((msRegs(REG_DPRX_AUX_PM0_74_H+0x600) & (BIT3|BIT2|BIT1|BIT0)) == 0))==FALSE)))

#endif

