/* For Sargent Compatible */
#ifndef __BD_MSTMCU__
#define __BD_MSTMCU__

#define MCU_SPEED_INDEX         IDX_MCU_CLK_345MHZ
#define SPI_SPEED_INDEX         IDX_SPI_CLK_54MHZ
#define SPI_MODE                SPI_MODE_FR
#define SPI_SSC_SOURCE          0   // 0:mempll, 1:lpll (Current code flow for mempll only)
#define SPI_SSC_DIVIDER         4   // Target Freq => (MEMORY_CLOCK_MHZ*(100+PERCENTAGE)/(SPI_SSC_DIVIDER+1))

//------I2C devices-------------------------------------------------------------
#define USE_SW_I2C              1
// HWi2c for enabling MIIC0 and MIIC1
#define MIIC0_ENABLE            MIIC0_1_ENABLE
#define MIIC1_ENABLE            0
#define USE_MIIC                MIIC0

#define HPD_INVERSE             0   // If it has BJT circuit on board, the HPD_INVERSE should be set to TRUE

#define DFT_MUX_Uart0           UART_GPIO40_41 // default mux uart 0 - VGA
#define DFT_MUX_Uart1           UART_GPIO02_03
#define SERIAL_BAUD_RATE        115200
#define SERIAL_BAUD_RATE_PM     9600

#define ENABLE_INTERNAL_CC      (0&ENABLE_USB_TYPEC)
#define ENABLE_VBUS_DETECT_SAR  (0) // sync to PD code, only available for internal CC w/ RT6190
#define USB_HUB                 (1&ENABLE_USB_TYPEC)

// Mapping to InputPortType
#define Input_DVI_C1            Input_Nothing
#define Input_DVI_C2            Input_Nothing
#define Input_DVI_C3            Input_Nothing
#define Input_DVI_C4            Input_Nothing
#define Input_DualDVI_C1        Input_Nothing
#define Input_DualDVI_C2        Input_Nothing
#define Input_HDMI_C1           Input_Nothing
#define Input_HDMI_C2           Input_Digital
#define Input_HDMI_C3           Input_Nothing
#define Input_HDMI_C4           Input_Nothing
#define Input_Displayport_C1    Input_Nothing
#define Input_Displayport_C2    Input_Nothing
#define Input_Displayport_C3    Input_Nothing
#define Input_Displayport_C4    Input_Nothing
#define Input_UsbTypeC_C3       Input_Nothing
#define Input_UsbTypeC_C4       Input_Nothing

#define CInput_DVI_C1           CInput_Nothing
#define CInput_DVI_C2           CInput_Nothing
#define CInput_DVI_C3           CInput_Nothing
#define CInput_DVI_C4           CInput_Nothing
#define CInput_DualDVI_C1       CInput_Nothing
#define CInput_DualDVI_C2       CInput_Nothing
#define CInput_HDMI_C1          CInput_Nothing
#define CInput_HDMI_C2          CInput_Digital
#define CInput_HDMI_C3          CInput_Nothing
#define CInput_HDMI_C4          CInput_Nothing
#define CInput_Displayport_C1   CInput_Nothing
#define CInput_Displayport_C2   CInput_Nothing
#define CInput_Displayport_C3   CInput_Nothing
#define CInput_Displayport_C4   CInput_Nothing
#define CInput_UsbTypeC_C3      CInput_Nothing
#define CInput_UsbTypeC_C4      CInput_Nothing

#define DVI_PORT                ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) | ((CInput_DVI_C4!=CInput_Nothing)?TMDS_PORT_D:TMDS_PORT_UNUSE) )
#define DVI_HPD_PORT            ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) | ((CInput_DVI_C4!=CInput_Nothing)?TMDS_PORT_D:TMDS_PORT_UNUSE) )
#define DVI_DDC_PORT            ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) | ((CInput_DVI_C4!=CInput_Nothing)?TMDS_PORT_D:TMDS_PORT_UNUSE) )
#define DVI_MASTER_PORT         ( ((CInput_DVI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_DVI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_DVI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) | ((CInput_DVI_C4!=CInput_Nothing)?TMDS_PORT_D:TMDS_PORT_UNUSE) )
#define DVI_SLAVE_PORT          TMDS_PORT_UNUSE //set to 0 if not DVI dual port enable

#define HDMI_PORT               ( ((CInput_HDMI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_HDMI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_HDMI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) | ((CInput_HDMI_C4!=CInput_Nothing)?TMDS_PORT_D:TMDS_PORT_UNUSE)  )
#define HDMI_HPD_PORT           ( ((CInput_HDMI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_HDMI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_HDMI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) | ((CInput_HDMI_C4!=CInput_Nothing)?TMDS_PORT_D:TMDS_PORT_UNUSE)  )
#define HDMI_DDC_PORT           ( ((CInput_HDMI_C1!=CInput_Nothing)?TMDS_PORT_A:TMDS_PORT_UNUSE) | ((CInput_HDMI_C2!=CInput_Nothing)?TMDS_PORT_B:TMDS_PORT_UNUSE) | ((CInput_HDMI_C3!=CInput_Nothing)?TMDS_PORT_C:TMDS_PORT_UNUSE) | ((CInput_HDMI_C4!=CInput_Nothing)?TMDS_PORT_D:TMDS_PORT_UNUSE)  )

#define DP_AUX_DDC_EXCHANGE		0 // (PORT_0_2_EXCHANGE) // (PORT_0_3_EXCHANGE ) // (PORT_1_2_EXCHANGE ) // (PORT_1_3_EXCHANGE)
// DDC port used
#define DDC_Port_D0             ((CInput_DVI_C1!=CInput_Nothing) || (CInput_DualDVI_C1!=CInput_Nothing) || (CInput_HDMI_C1!=CInput_Nothing))
#define DDC_Port_D1             ((CInput_DVI_C2!=CInput_Nothing) || (CInput_DualDVI_C2!=CInput_Nothing) || (CInput_HDMI_C2!=CInput_Nothing))
#define DDC_Port_D2             ((CInput_DVI_C3!=CInput_Nothing) || (CInput_HDMI_C3!=CInput_Nothing))
#define DDC_Port_D3             (USB_HUB)


//PWM
#define BrightnessPWM           _PWM6_GP0_ // Driving Non-LED BL Module
#define Init_BrightnessPWM()    (MEM_MSWRITE_BYTE_MASK(REG_000408,BIT6,BIT6), MEM_MSWRITE_BYTE_MASK(REG_000581,BIT2,BIT2))
#define BACKLIGHT_PWM                       BrightnessPWM  //Driving Non-LED BL Module      [0] PWM0 on GPIO17
#define BACKLIGHT_PWM_FREQ                  240         //unit:Hz
#define BACKLIGHT_PWM_DUTY                  0x7F        //range:0~0xFF  0x7F=50%
#define BACKLIGHT_PWM_CONFIG                _PWM_DB_EN_
#define BACKLIGHT_PWM_VS_ALG_NUM            _NO_USE_    //vsync align number

#if AudioFunc
#define VolumePWM               _NO_USE_//_PWM5_GP0_
#define Init_VolumePWM()

#define VOLUME_PWM                          VolumePWM    // Not Finishhed Yet in CHIP_FAMILY_TSUMC //_DISP_PWM3_GP0_
#define VOLUME_PWM_FREQ                     240
#define VOLUME_PWM_DUTY                     0x7F                //0~0xFF  0x7F=50%
#define VOLUME_PWM_CONFIG                   _PWM_DB_EN_
#define VOLUME_PWM_VS_ALG_NUM               _NO_USE_
#endif

#define PowerKey                (_bit2_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define Init_PowerKey()         (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 1, BIT2))
#define PM_POWERKEY_WAKEUP      GPIO02_INT

//================================================================================================================
//================================================================================================================
// 1.Write protection:
#define hwDDC_WP_Pin            //Pull to Ground in Schematic
#define hwEEPROM_WP             //GPIO03
#define hwFlash_WP_Pin          //GPIO10

// ddc eeprom wp be pulled high to 5V, set to input for output high level
#define hw_SetDDC_WP()          //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 1, BIT5), DDCWPActiveFlag=1)//(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, 0, BIT6), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, 0, BIT6), DDCWPActiveFlag=0)
#define hw_ClrDDC_WP()          //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, 0, BIT5), DDCWPActiveFlag=0)
#define Init_hwDDC_WP_Pin()     //MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, 0, BIT5)//(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, 1, BIT6)) // output disable

#define Set_EEPROM_WP()         MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 1, BIT0)
#define Clr_EEPROM_WP()         MEM_MSWRITE_BIT(_REG_GPIO0_OUT, 0, BIT0)
#define Init_hwEEPROM_WP()      MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, 0, BIT0)

#define hw_SetFlashWP()         MEM_MSWRITE_BIT(_REG_GPIO1_OUT, 0, BIT0)
#define hw_ClrFlashWP()         MEM_MSWRITE_BIT(_REG_GPIO1_OUT, 1, BIT0)
#define Init_hwFlash_WP_Pin()   MEM_MSWRITE_BIT(_REG_GPIO1_OEZ, 0, BIT0)

//================================================================================================================
//================================================================================================================
// 2.Backlight and panelon:
#define hwBlacklit_Pin          //GPIO01  //Driving Non-LED BL Module
#define hwPanel_Pin             //GPIO00
#define hw_SetBlacklit()        (MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT4), BackLightActiveFlag=1)
#define hw_ClrBlacklit()        (MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT4), BackLightActiveFlag=0)
#define Init_hwBlacklit_Pin()   MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT4) // output enable

#define hw_SetPanel()           MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT1)
#define hw_ClrPanel()           MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT1)
#define Init_hwPanel_Pin()      MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0, BIT1) // output enable

//================================================================================================================
//================================================================================================================
// 4.Detect Cable
#define CABLE_DETECT_VGA_USE_SAR        0
//#define CABLE_DETECT_DVI_USE_SAR        0
//#define CABLE_DETECT_HDMI_USE_SAR       0

#if CABLE_DETECT_VGA_USE_SAR
#define hwDSUBCable_Pin                 0
#define Init_hwDSUBCable_Pin()
#define CABLE_DET_VGA_SAR               0
#else
#define hwDSUBCable_Pin                 (_bit6_(MEM_MSREAD_BYTE(_REG_GPIOSAR_IN)))  // SAR2
#define Init_hwDSUBCable_Pin()          (MEM_MSWRITE_BIT(_REG_GPIOSAR_OEZ, 1, BIT2))
#define CABLE_DET_VGA_SAR               0
#endif


#if (CInput_HDMI_C1!=CInput_Nothing)||(CInput_DVI_C1!=CInput_Nothing)   // Digital Input Port 0
#define hwDVI0Cable_Pin                 0
#define Init_hwDVI0Cable_Pin()
#define CABLE_DET_DVI0_SAR              0
#else
#define hwDVI0Cable_Pin                 0
#define Init_hwDVI0Cable_Pin()
#define CABLE_DET_DVI0_SAR              0
#endif

#if (CInput_HDMI_C2!=CInput_Nothing)||(CInput_DVI_C2!=CInput_Nothing)   // Digital Input Port 1
#define hwDVI1Cable_Pin                 (KEYPAD_SAR03)    // SAR3
#define Init_hwDVI1Cable_Pin()          (MEM_MSWRITE_BIT(REG_3A68,1,BIT3))  // Enable SAR ch
#define CABLE_DET_DVI1_SAR              SAR3_EN
#else
#define hwDVI1Cable_Pin                 0
#define Init_hwDVI1Cable_Pin()
#define CABLE_DET_DVI1_SAR              0
#endif

/* In fact, this input is not used in this board. */
#if (CInput_HDMI_C3!=CInput_Nothing)||(CInput_DVI_C3!=CInput_Nothing)
#define hwDVI2Cable_Pin                 0
#define Init_hwDVI2Cable_Pin()
#define CABLE_DET_DVI2_SAR              0
#else
#define hwDVI2Cable_Pin                 0
#define Init_hwDVI2Cable_Pin()
#define CABLE_DET_DVI2_SAR              0
#endif

#if (CInput_HDMI_C4!=CInput_Nothing)||(CInput_DVI_C4!=CInput_Nothing)
#define hwDVI3Cable_Pin                 0
#define Init_hwDVI3Cable_Pin()
#define CABLE_DET_DVI3_SAR              0
#else
#define hwDVI3Cable_Pin                 0
#define Init_hwDVI3Cable_Pin()
#define CABLE_DET_DVI3_SAR              0
#endif

#define DVI5V_GPIO_Sel()

#if (CInput_Displayport_C1!=CInput_Nothing)
#define hwSDM_AuxN_DP0Pin_Pin           0
#define Init_hw_DP0_SDMPin()
#define hwGNDDP0_Pin				    0
#define Init_hw_DP0_GNDPin()
#define DC_DPCable_DET_DP0_SAR          0
#define Init_hw_DP0_PWRPin()
#else
#define hwSDM_AuxN_DP0Pin_Pin           0
#define Init_hw_DP0_SDMPin()
#define hwGNDDP0_Pin				    0
#define Init_hw_DP0_GNDPin()
#define DC_DPCable_DET_DP0_SAR          0
#define Init_hw_DP0_PWRPin()
#endif

#if CInput_Displayport_C2!=CInput_Nothing
#define hwSDM_AuxN_DP1Pin_Pin           0
#define Init_hw_DP1_SDMPin()
#define hwGNDDP1_Pin				    0
#define Init_hw_DP1_GNDPin()
#define DC_DPCable_DET_DP1_SAR          0
#define Init_hw_DP1_PWRPin()
#else
#define hwSDM_AuxN_DP1Pin_Pin           0
#define Init_hw_DP1_SDMPin()
#define hwGNDDP1_Pin				    0
#define Init_hw_DP1_GNDPin()
#define DC_DPCable_DET_DP1_SAR          0
#define Init_hw_DP1_PWRPin()
#endif

#if CInput_Displayport_C3!=CInput_Nothing
#define hwSDM_AuxN_DP2Pin_Pin           0
#define Init_hw_DP2_SDMPin()
#define hwGNDDP2_Pin                    0
#define Init_hw_DP2_GNDPin()
#define DC_DPCable_DET_DP2_SAR          0
#define Init_hw_DP2_PWRPin()
#else
#define hwSDM_AuxN_DP2Pin_Pin           0
#define Init_hw_DP2_SDMPin()
#define hwGNDDP2_Pin                    0
#define Init_hw_DP2_GNDPin()
#define DC_DPCable_DET_DP2_SAR          0
#define Init_hw_DP2_PWRPin()
#endif

#if (CInput_Displayport_C4!=CInput_Nothing)
#define hwSDM_AuxN_DP3Pin_Pin           0
#define Init_hw_DP3_SDMPin()
#define hwGNDDP3_Pin                    0
#define Init_hw_DP3_GNDPin()
#define DC_DPCable_DET_DP3_SAR          0
#define Init_hw_DP3_PWRPin()
#else
#define hwSDM_AuxN_DP3Pin_Pin           0
#define Init_hw_DP3_SDMPin()
#define hwGNDDP3_Pin                    0
#define Init_hw_DP3_GNDPin()
#define DC_DPCable_DET_DP3_SAR          0
#define Init_hw_DP3_PWRPin()
#endif

#if ENABLE_TYPEC_SAR_DET && (CInput_UsbTypeC_C3!=CInput_Nothing)
#define hwSDM_AuxN_TYPEC3Pin_Pin        0
#define hwSDM_AuxP_TYPEC3Pin_Pin        0
#define Init_hw_TYPEC3_SDMPin()
#define DC_SDM_DET_TYPEC3_SAR           0
#else
#define hwSDM_AuxN_TYPEC3Pin_Pin        0
#define hwSDM_AuxP_TYPEC3Pin_Pin        0
#define Init_hw_TYPEC3_SDMPin()
#define DC_SDM_DET_TYPEC3_SAR           0
#endif

#if ENABLE_TYPEC_SAR_DET && (CInput_UsbTypeC_C4!=CInput_Nothing)
#define hwSDM_AuxN_TYPEC4Pin_Pin        0
#define hwSDM_AuxP_TYPEC4Pin_Pin        0
#define Init_hw_TYPEC4_SDMPin()
#define DC_SDM_DET_TYPEC4_SAR           0
#else
#define hwSDM_AuxN_TYPEC4Pin_Pin        0
#define hwSDM_AuxP_TYPEC4Pin_Pin        0
#define Init_hw_TYPEC4_SDMPin()
#define DC_SDM_DET_TYPEC4_SAR           0
#endif

#define CABLE_DET_SAR_EN                (CABLE_DET_VGA_SAR|CABLE_DET_DVI1_SAR)
#define CABLE_DET_SAR                   0
#define CABLE_DET_SAR2_EN               0

//================================================================================================================
//================================================================================================================
// 5.Led
#define hwGreenLed_Pin                  // GPIO22
#define hwAmberLed_Pin                  // GPIO25

#define hw_SetGreenLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,1,BIT2);}
#define hw_ClrGreenLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT2);MEM_MSWRITE_BIT(_REG_GPIO2_OUT,0,BIT2);}
#define Init_hwGreenLed_Pin()           MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0,BIT2)// output enable

#define hw_SetAmberLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 0, BIT3);}
#define hw_ClrAmberLed()                {MEM_MSWRITE_BIT(_REG_GPIO2_OEZ,0,BIT3);MEM_MSWRITE_BIT(_REG_GPIO2_OUT, 1, BIT3);}
#define Init_hwAmberLed_Pin()           MEM_MSWRITE_BIT(_REG_GPIO2_OEZ, 0,BIT3)// output enable

//================================================================================================================
//================================================================================================================
// 6.Audio control:
#define ENABLE_DAC                      (1 && AudioFunc)
#define ENABLE_SPDIF                    (0 && ENABLE_DAC)
#define ENABLE_I2S                      (0 && ENABLE_DAC)
#define ENABLE_LINE_OUT                 (0 && ENABLE_DAC && !ENABLE_I2S)
#define ENABLE_EAR_OUT                  (0 && ENABLE_DAC && !ENABLE_I2S)
#define ENABLE_LINE_IN                  (0 && ENABLE_DAC && !ENABLE_I2S)

#define hw_SetMute()
#define hw_ClrMute()
#define Init_hwMute_Pin()

#if ENABLE_I2S
#define Init_Pin_I2S()
#else
#define Init_Pin_I2S()
#endif

#if ENABLE_SPDIF
#define Init_Pin_SPDIF()		        (MEM_MSWRITE_BIT(REG_000406,BIT0,BIT0))
#else
#define Init_Pin_SPDIF()
#endif

//================================================================================================================
//================================================================================================================
// I2C
#if defined(UseVGACableReadWriteAllPortsEDID) //&&!defined(UseInternalDDCRam)
#define Set_i2c_SDA_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT2))
#define Clr_i2c_SDA_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT2))
#define i2c_SDAHi_VGA()             //(_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define i2c_SDALo_VGA()             //(!i2c_SDAHi())
#define Set_i2c_SCL_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT3))
#define Clr_i2c_SCL_VGA()           //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT3), MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT3))
#define i2c_SCLHi_VGA()             //(_bit3_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define i2c_SCLLo_VGA()             //(!i2c_SCLHi())

#define Set_i2c_SDA_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT1))
#define Clr_i2c_SDA_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, FALSE, BIT1), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, FALSE, BIT1))
#define i2c_SDAHi_DVI()             //(_bit1_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define i2c_SDALo_DVI()             //(!i2c_SDAHi())
#define Set_i2c_SCL_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, TRUE, BIT2))
#define Clr_i2c_SCL_DVI()           //(MEM_MSWRITE_BIT(_REG_PMGPIO2_OUT, FALSE, BIT2), MEM_MSWRITE_BIT(_REG_PMGPIO2_OEZ, FALSE, BIT2))
#define i2c_SCLHi_DVI()             //(_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO2_IN)))
#define i2c_SCLLo_DVI()             //(!i2c_SCLHi())

#define Set_i2c_SDA()               //((LoadEDIDSelectPort == DVI1)?(Set_i2c_SDA_DVI()):(Set_i2c_SDA_VGA()))
#define Clr_i2c_SDA()               //((LoadEDIDSelectPort == DVI1)?(Clr_i2c_SDA_DVI()):(Clr_i2c_SDA_VGA()))
#define i2c_SDAHi()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SDAHi_DVI()):(i2c_SDAHi_VGA()))
#define i2c_SDALo()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SDALo_DVI()):(i2c_SDALo_VGA()))

#define Set_i2c_SCL()               //((LoadEDIDSelectPort == DVI1)?(Set_i2c_SCL_DVI()):(Set_i2c_SCL_VGA()))
#define Clr_i2c_SCL()               //((LoadEDIDSelectPort == DVI1)?(Clr_i2c_SCL_DVI()):(Clr_i2c_SCL_VGA()))
#define i2c_SCLHi()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SCLHi_DVI()):(i2c_SCLHi_VGA()))
#define i2c_SCLLo()                 0//((LoadEDIDSelectPort == DVI1)?(i2c_SCLLo_DVI()):(i2c_SCLLo_VGA()))
#endif

#if !USEFLASH
#define hwI2C_SCL_Pin               // GPIO04
#define hwI2C_SDA_Pin               // GPIO05
#define Init_hwI2C_SCL_Pin()        (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT4))
#define Init_hwI2C_SDA_Pin()        (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT5))

#define Set_i2c_SDA()               (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT5))
#define Clr_i2c_SDA()               (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, FALSE, BIT5), MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, FALSE, BIT5))
#define i2c_SDAHi()                 (_bit5_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define i2c_SDALo()                 (!i2c_SDAHi())
#define Set_i2c_SCL()               (MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, TRUE, BIT4))
#define Clr_i2c_SCL()               (MEM_MSWRITE_BIT(_REG_GPIO0_OUT, FALSE, BIT4), MEM_MSWRITE_BIT(_REG_GPIO0_OEZ, FALSE, BIT4))
#define i2c_SCLHi()                 (_bit4_(MEM_MSREAD_BYTE(_REG_GPIO0_IN)))
#define i2c_SCLLo()                 (!i2c_SCLHi())
#endif

//===================================================================================================================
// HDMI
#define HDMI_DDC_None          0xF
#define HDMI_DDC_0             0
#define HDMI_DDC_1             1
#define HDMI_DDC_2             2

#define HDMI_Rx0               HDMI_DDC_None //no this port in this board
#define HDMI_Rx1               HDMI_DDC_1
#define HDMI_Rx2               HDMI_DDC_None //no this port in this board

//Check each HDMI5v is connected to IC or not
//If there is no HDMI5v connection to IC please set these define to 0
#define HDMI_Cable5v_0        1
#define HDMI_Cable5v_1        1
#define HDMI_Cable5v_2        1

#if ENABLE_HDCP
    #if(!HPD_INVERSE)
        #define hw_Set_HdcpHpd()
        #define hw_Clr_HdcpHpd()
        #define hw_Set_HdcpHpd2()       (MEM_MSWRITE_BIT(REG_PAD_TOP_18_L, FALSE, BIT1), MEM_MSWRITE_BIT(REG_PAD_TOP_19_L, TRUE, BIT1))
        #define hw_Clr_HdcpHpd2()       (MEM_MSWRITE_BIT(REG_PAD_TOP_18_L, FALSE, BIT1), MEM_MSWRITE_BIT(REG_PAD_TOP_19_L, FALSE, BIT1))
        #define hw_Set_HdcpHpd3()
        #define hw_Clr_HdcpHpd3()
    #else
        #define hw_Set_HdcpHpd()
        #define hw_Clr_HdcpHpd()
        #define hw_Set_HdcpHpd2()       (MEM_MSWRITE_BIT(REG_PAD_TOP_18_L, FALSE, BIT1), MEM_MSWRITE_BIT(REG_PAD_TOP_19_L, FALSE, BIT1))
        #define hw_Clr_HdcpHpd2()       (MEM_MSWRITE_BIT(REG_PAD_TOP_18_L, FALSE, BIT1), MEM_MSWRITE_BIT(REG_PAD_TOP_19_L, TRUE, BIT1))
        #define hw_Set_HdcpHpd3()
        #define hw_Clr_HdcpHpd3()
    #endif
        #define Init_hwHDCP_Hpd_Pin()
        #define Init_hwHDCP_Hpd_Pin2()  (MEM_MSWRITE_BYTE_MASK(REG_PAD_TOP_31_L,0,BIT2|BIT1|BIT0), MEM_MSWRITE_BYTE_MASK(REG_DPRX_PHY_PM_65_L, 0, BIT0), MEM_MSWRITE_BYTE_MASK(REG_DPRX_HDMI_30_L, 0, BIT4|BIT0), MEM_MSWRITE_BYTE_MASK(REG_PHY2P1_2_P0_6F_H,BIT2|BIT1,BIT2|BIT1), MEM_MSWRITE_BIT(REG_PAD_TOP_0B_H, FALSE, BIT0))
        #define Init_hwHDCP_Hpd_Pin3()
#else
    #define hw_Set_HdcpHpd()
    #define hw_Clr_HdcpHpd()
    #define hw_Set_HdcpHpd2()
    #define hw_Clr_HdcpHpd2()
    #define hw_Set_HdcpHpd3()
    #define hw_Clr_HdcpHpd3()
    #define Init_hwHDCP_Hpd_Pin()
    #define Init_hwHDCP_Hpd_Pin2()
    #define Init_hwHDCP_Hpd_Pin3()
#endif //ENABLE_HDCP

#if HDMI_PRETEST
#define Init_hwCEC_Pin()            //(MEM_MSWRITE_BIT(REG_0262, TRUE, BIT0))
#endif

#define GPIO27_AS_CEC        1

#if (ENABLE_CEC && GPIO27_AS_CEC)
#define HDMI_EFUSE_ADDRESS_HRX_CEC    0x15F
#define Init_hwCEC_Pin()              (MEM_MSWRITE_BIT(REG_PAD_TOP_03_L, TRUE, BIT4)) //h04_03[4] gpio27 as cec
#endif

//===================================================================================================================
//SARKEY_ENABLE_BIT
#define SAR0_EN         BIT0
#define SAR1_EN         BIT1
#define SAR2_EN         BIT2
#define SAR3_EN         BIT3
#define SAR4_EN         BIT4
#define SAR5_EN         BIT5
#define SAR6_EN         BIT6
#define SAR7_EN         BIT7
#define SAR8_EN         BIT8
#define SAR9_EN         BIT9
#define SAR10_EN        BIT10
#define SAR11_EN        BIT11

#define SARKEY_EN       (SAR0_EN|SAR1_EN)

#define KEYPAD_ADC_A    KEYPAD_SAR00
#define KEYPAD_ADC_B    KEYPAD_SAR01

#define AOVDV_VALUE     4

//#define LVDS_CHANNEL    (MOD_PORTB|MOD_PORTC)//(MOD_PORTA|MOD_PORTB|MOD_PORTC|MOD_PORTD)
//#define I_GEN_CH        CH2

#if ENABLE_TOUCH_PANEL
#define Set_TOCUCH_PANEL_POWER()
#define Clr_TOCUCH_PANEL_POWER()
#define Init_TOCUCH_PANEL_POWER_Pin()
#define SPI_MISO_Pin                //(_bit2_(MEM_MSREAD_BYTE(_REG_PMGPIO1_IN)))
#define Init_SPI_MISO_Pin()         //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, TRUE, BIT2))   // PMGPIO12

#define Set_SPI_SCK()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT3))
#define Clr_SPI_SCK()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT3))
#define Init_SPI_SCK_Pin()          //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT3))  // PMGPIO13

#define Set_SPI_MOSI()              //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT4))
#define Clr_SPI_MOSI()              //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT4))
#define Init_SPI_MOSI_Pin()         //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT4)) // PMGPIO14

#define Set_SPI_SEL()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, TRUE, BIT5))
#define Clr_SPI_SEL()               //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OUT, FALSE, BIT5))
#define Init_SPI_SEL_Pin()          //(MEM_MSWRITE_BIT(_REG_PMGPIO1_OEZ, FALSE, BIT5))// PMGPIO15
#endif

//================================================================================================================
//================================================================================================================
// TYPE C
#define hw_Set_TYPEC_A_AUX_P()
#define hw_Clr_TYPEC_A_AUX_P()
#define Init_TYPEC_A_AUX_P()

#define hw_Set_TYPEC_A_AUX_N()
#define hw_Clr_TYPEC_A_AUX_N()
#define Init_TYPEC_A_AUX_N()

#define hw_Set_TYPEC_A_AUX_TRI()
#define hw_Clr_TYPEC_A_AUX_TRI()
#define Init_TYPEC_A_AUX_TRI()

#define hw_Set_TYPEC_B_AUX_P()
#define hw_Clr_TYPEC_B_AUX_P()
#define Init_TYPEC_B_AUX_P()

#define hw_Set_TYPEC_B_AUX_N()
#define hw_Clr_TYPEC_B_AUX_N()
#define Init_TYPEC_B_AUX_N()

#define hw_Set_TYPEC_B_AUX_TRI()
#define hw_Clr_TYPEC_B_AUX_TRI()
#define Init_TYPEC_B_AUX_TRI()

#define hw_Set_Hub_Vbus()
#define hw_Clr_Hub_Vbus()
#define Init_Hub_Vbus()

#define hw_SetUSB5V()

#define VBUS_DET_SAR_CH             0

//================================================================================================================
//================================================================================================================
// AUX and combo/DPC port mapping
#define AUX_GPIO_31_32_33			0 // GPIO31 -> HPD, GPIO32 -> AUXN, GPIO33 -> AUXP
#define AUX_GPIO_52_53_54			1 // GPIO52 -> HPD, GPIO53 -> AUXN, GPIO54 -> AUXP
#define AUX_GPIO_DPC_3_4_5			2 // GPIO_DPC5 -> HPD, GPIO_DPC3 -> AUXN, GPIO_DPC4 -> AUXP
#define AUX_None					3

#define DPRx_C1_AUX					AUX_None
#define DPRx_C2_AUX					AUX_None
#define DPRx_C3_AUX					AUX_None
#define DPRx_C4_AUX					AUX_None

#define SET_DPC_PORT_AS_DP_PORT		0 // If CInput_Displayport_C4 is not equal to CInput_Nothing, this should set to 1
#define SET_DP_C4_LANE02_PN_SWAP    1 & SET_DPC_PORT_AS_DP_PORT // BD_MTV19072 & BD_MT9700_A6V2_P2_S need to set to 1

#endif

